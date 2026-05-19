/*

 Ngine v5.0

 Module      : Assets manager.
 Requirements: none
 Description : Loads, stores and manages all types
               of supported resources. Also protects
               from loading duplicates of already
               loaded resources.

*/

#include "assert.h"
#include "core/storage.h"
#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/utilities/parserJSON.h"
#include "core/utilities/writerJSON.h"
#include "assets/interface.h"

namespace en
{
static std::unique_ptr<assets::Interface> assetsManager = nullptr;

assets::Interface& Assets()
{
    assert(assetsManager);

    return *assetsManager;
}

namespace assets
{
forceinline bool getExtensionPosition(const std::string& filename, uint64& extensionPosition)
{
    uint64 position = filename.rfind(".");

    // If there is no dot, or dot is last character in filename
    // there is no valid file extension to extract.
    if (position == std::string::npos ||
        (position + 1) == std::string::npos)
    {
        return false;
    }

    extensionPosition = position + 1;

    return true;
}

FileExtension fileExtension(const std::string& filename)
{
    uint64 position = 0;
    if (!getExtensionPosition(filename, position))
    {
        return FileExtension::Unknown;
    }

    // Extracts file extension and converts it to lower case
    std::string extension = filename.substr(position);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    // Images

    if (extension == "bmp")
    {
        return FileExtension::BMP;
    }
    if (extension == "dds")
    {
        return FileExtension::DDS;
    }
    if (extension == "exr")
    {
        return FileExtension::EXR;
    }
    if (extension == "hdr")
    {
        return FileExtension::HDR;
    }
    if (extension == "png")
    {
        return FileExtension::PNG;
    }
    if (extension == "tga")
    {
        return FileExtension::TGA;
    }

    // Materials

    if (extension == "mtl")
    {
        return FileExtension::MTL;
    }
    if (extension == "material")
    {
        return FileExtension::Material;
    }

    // Models / Scenes

    if (extension == "fbx")
    {
        return FileExtension::FBX;
    }
    if (extension == "obj")
    {
        return FileExtension::OBJ;
    }

    // Metadata

    if (extension == "metadata")
    {
        return FileExtension::Metadata;
    } 
    if (extension == "asset")
    {
        return FileExtension::Asset;
    }

    return FileExtension::Unknown;
}

bool isImageFileExtension(const FileExtension& extension)
{
    if (extension == FileExtension::BMP ||
        extension == FileExtension::DDS ||
        extension == FileExtension::EXR ||
        extension == FileExtension::HDR ||
        extension == FileExtension::PNG ||
        extension == FileExtension::TGA)
    {
        return true;
    }

    return false;
}

bool isMaterialFileExtension(const FileExtension& extension)
{
    if (extension == FileExtension::MTL ||
        extension == FileExtension::Material)
    {
        return true;
    }

    return false;
}

bool isSupportedResourceFileExtension(const FileExtension extension)
{
    // Images
    if (extension == FileExtension::BMP ||
        extension == FileExtension::DDS ||
        extension == FileExtension::EXR ||
        extension == FileExtension::HDR ||
        extension == FileExtension::PNG ||
        extension == FileExtension::TGA ||
        // Materials
        extension == FileExtension::MTL ||
        // Models
        extension == FileExtension::OBJ ||
        extension == FileExtension::FBX)    // TODO: WAV
    {
        return true;
    }

    return false;
}

std::filesystem::path removeSuffix(const std::filesystem::path& filepath, const std::string& suffix)
{
    std::string name = filepath.filename().string();

    if (name.size() >= suffix.size() &&
        name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
        name.erase(name.size() - suffix.size());
        return filepath.parent_path() / name;
    }

    // File has no specified suffix (returns unchanged)
    return filepath;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AssetManager::create(void)
{
    if (!assetsManager)
    {
        assetsManager = std::make_unique<AssetManager>();
    }

    return true;
}

void AssetManager::destroy(void)
{
    // Trigger destructor if not destroyed already
    en::assetsManager.reset();
}

AssetManager::AssetManager() :
    Interface()
{
    enLog("Starting module: Assets.\n");

    pathAssets = "./assets/";
    pathScreenshots = "./screenshots/";

    buildResourcesCatalog();
}

AssetManager::~AssetManager()
{
    enLog("Closing module: Assets.\n");
}

const std::string& AssetManager::assetsPath(void) const
{
    return pathAssets;
}

const std::string& AssetManager::screenshotsPath(void) const
{
    return pathScreenshots;
}

bool AssetManager::findResourceUUIDByPath(const std::string& filePath, UUID& uuid) const
{
    std::unordered_map<std::string, UUID>::const_iterator it = resourceUUIDs.find(filePath);
    if (it != resourceUUIDs.end())
    {
        uuid = it->second;
        return true;
    }

    return false;
}

bool AssetManager::findResourcePathByUUID(const UUID& uuid, std::string& filePath) const
{
    std::unordered_map<UUID, std::string>::const_iterator it = resourcePaths.find(uuid);
    if (it != resourcePaths.end())
    {
        filePath = it->second;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// .metadata format v1:
//
// {
//     "version" : 1,
//     "uuid" : "f0593503-3168-4904-91b1-a5c9d09ae57b"
// }
// 
bool AssetManager::storeMetadata(const std::filesystem::path& metadataPath, const std::filesystem::path& filePath)
{
    // Creates .metadata file for storing UUID of matching resource
    en::storage::File* file = Storage->open(metadataPath.string(), en::storage::FileAccess::Write);
    if (!file) // unlikely
    {
        logError("Failed to create metadata file:\n%s\n", metadataPath.c_str());
        return false;
    }

    // Creates new UUID
    UUID uuid;
    uuid.init();

    // File will be stored on exit of scope
    {
        WriterJSON writer(*file);
        writer.addKey("version", 1);
        writer.addKey("uuid", uuid.description());
    }

    delete file;

    std::string resourcePath = filePath.string();

    logDebug("C %s - %s\n", uuid.description().c_str(), resourcePath.c_str());

    // Builds relationship between resource UUID and its current path
    resourceUUIDs[resourcePath] = uuid;
    resourcePaths[uuid] = resourcePath;

    return true;
}

ParsingResult parseMetadataV1(const uint8* buffer, const uint64 size, uint64& version, UUID& uuid)
{
    // Don't pass buffer ownership
    ParserJSON parser(buffer, size, false);

    // Metadata needs to start with root object
    JSONType type = JSONType::None;
    ParsingResult result = parser.findNextElement(type);
    if (result != ParsingResult::Success)
    {
        return result;
    }
    if (type != JSONType::Object)
    {
        return ParsingResult::InvalidFormat;
    }

    version = 0;

    do
    {
        result = parser.findNextElement(type);
        if (result != ParsingResult::Success)
        {
            break;
        }

        if (type == JSONType::Comma)
        {
            continue;
        }
        if (type == JSONType::ObjectTerminator)
        {
            // Root object ended
            break;
        }

        if (type != JSONType::String)
        {
            logError("Invalid JSON syntax when parsing .metadata file: Was expecting Key name!\n");
            return ParsingResult::InvalidFormat;
        }

        if (parser.isStringMatching("version"))
        {
            result = parser.parseKeyU64(version);
            if (result != ParsingResult::Success)
            {
                break;
            }
        }
        else
        if (parser.isStringMatching("uuid"))
        {
            std::string uuidString;
            result = parser.parseKeyString(uuidString);
            if (result != ParsingResult::Success)
            {
                break;
            }

            // Convert string into actual UUID
            uuid.init(uuidString);
        }
        else
        {
            // Ignoring unrecognized keys
            result = parser.skipKeyValuePair();
            if (result != ParsingResult::Success)
            {
                break;
            }
        }

    } while (result == ParsingResult::Success);

    return result;
}

bool AssetManager::loadMetadata(const std::filesystem::path& metadataPath, const std::filesystem::path& filePath)
{
    en::storage::File* file = Storage->open(metadataPath.string());
    if (!file)
    {
        logError("Failed to open metadata file:\n%s\n", metadataPath.c_str());
        return false;
    }

    // Verifies file has content
    uint64 size = file->size();
    if (!size) // unlikely
    {
        logError("Metadata file is empty:\n%s\n", filePath.c_str());
        return false;
    }

    // Allocates temporary buffer for parsing purposes
    uint8* buffer = allocate<uint8>(size);
    if (!buffer) // unlikely
    {
        logCritical("Run out of memory while trying to allocate buffer for parsing metadata file:\n%s\n", filePath.c_str());
        return false;
    }

    // Read metadata file content to buffer
    uint64 readSize = 0;
    if (!file->read(0, size, buffer, &readSize))
    {
        logError("Failed to read file:\n%s\n", filePath.c_str());

        if (readSize != size)
        {
            logError("Expected to read %u bytes, but read %u.\n", size, readSize);
        }

        deallocate<uint8>(buffer);
        return false;
    }

    uint64 version = 0;
    UUID uuid;

    // Read .metadata file state
    ParsingResult result = parseMetadataV1(buffer, size, version, uuid);
    deallocate<uint8>(buffer);
    if (result != ParsingResult::Success)
    {
        if (result == ParsingResult::InvalidFormat)
        {
            logError("Invalid JSON syntax when parsing .metadata file!\n");
        }
        else
        if (result == ParsingResult::IncompleteData)
        {
            logError("Buffer storing .metadata file content is incomplete!\n");
        }
       
        return false;
    }

    if (version != 1)
    {
        logError("Unsupported version of .metadata file!\n%s\n", metadataPath.string());
        return false;
    }

    std::string resourcePath = filePath.string();

    // .metadata file may be examined after its matching resource file 
    // was already processed (and its relationship already built).
    if (resourceUUIDs[resourcePath] == uuid &&
        resourcePaths[uuid] == resourcePath)
    {
        return true;
    }

    logDebug("L %s - %s\n", uuid.description().c_str(), resourcePath.c_str());

    // Builds relationship between resource UUID and its current path
    resourceUUIDs[resourcePath] = uuid;
    resourcePaths[uuid] = resourcePath;

    return true;
}

bool AssetManager::buildResourcesCatalog(void)
{
    // Ensures assets catalog root path is already resolved and absolute
    std::filesystem::path assetsRoot = std::filesystem::weakly_canonical(assetsPath());

    // Sanity check, making sure assets directory exists
    if (!std::filesystem::exists(assetsRoot) || !std::filesystem::is_directory(assetsRoot))
    {
        logError("Assets directory not found!\n%s\n", assetsRoot.c_str());
        return false;
    }

    // Recursively iterates over every sub-directory, and every file in it.
    // Order of iteration is not deterministic, symbolic links are not followed.
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(assetsRoot))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        // Full paths are used by the engine for any type of tracking or comparison
        // (will generate absolute, resolved path to the file, even if the path itself doesn't exist yet)
        std::filesystem::path filePath = std::filesystem::weakly_canonical(entry.path());

        // TODO: Make metadata file JSON. Add in it aside from UUID,
        //       last file modification date. This way we can auto
        //       detect if file was modified since last time application
        //       was executiong, and import that file/asset only in
        //       such case, instead of importing everything from 
        //       scratch every time. This will also allow in the 
        //       future hot reloading of assets while application
        //       is running. If date read from .metadata file is
        //       older than reported by file system (file was modified).
        //       This will require updating .metadata file with new
        //       date once its re-imported.
        //       Alternative to "last modified date" could be "content
        //       hash" which is bulletproff (resistent to FS issues).
        //       We also need to add "produced assets" list, which
        //       stores those assets UUIDs. When resource file changes
        //       that list tells us which assets need to get re-imported.

        // If currently examined file is resource of supported type,
        // its .metadata file is created (when missing) or loaded to
        // build relation between current file path and its UUID.
        std::filesystem::path extension = filePath.extension();
        FileExtension extensionType = fileExtension(extension.string());
        if (isSupportedResourceFileExtension(extensionType))
        {
            // Expected accompanying it .metadata file
            std::filesystem::path metadataPath = filePath;
            metadataPath += ".metadata";

            if (std::filesystem::exists(metadataPath))
            {
                if (!loadMetadata(metadataPath, filePath))
                {
                    // Try to fix .metadata file by re-generating it
                    storeMetadata(metadataPath, filePath);
                }
            }
            else
            {
                storeMetadata(metadataPath, filePath);
            }
        }
        else 
        if (extensionType == FileExtension::Metadata)
        {
            // If its .metadata file, find its matching resource file
            // and build relation between current file path and its UUID.
            std::filesystem::path metadataPath = filePath;
            std::filesystem::path filePath     = removeSuffix(metadataPath, ".metadata");

            if (std::filesystem::exists(filePath))
            {
                if (!loadMetadata(metadataPath, filePath))
                {
                    // Try to fix .metadata file by re-generating it
                    storeMetadata(metadataPath, filePath);
                }
            }
            else
            {
                logError("Orphaned metadata file detected!\n%s\n", metadataPath.c_str());
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AssetDescriptor::AssetDescriptor(const AssetType _type) :
    assetID(),
    type(_type)
{
}

AssetDescriptor::AssetDescriptor(const AssetID id, const AssetType _type) :
    assetID(id),
    type(_type)
{
}

ImageAssetDescriptor::ImageAssetDescriptor(const UUID& uuid) :
    AssetDescriptor(AssetType::Image)
{
    // Source:

    // If there is only one source element in the vector, 
    // it implicitly identifies this surface as not having 
    // any existing mip maps stored in separate files on disk.
    sourceFile.push_back(uuid);

    // TODO: Verify descriptor on creation time?
}

ImageAssetDescriptor::ImageAssetDescriptor(const std::vector<UUID>& uuids) :
    AssetDescriptor(AssetType::Image)
{
    sourceFile = uuids;
}

// .asset format v1:
//
// {
//     "version" : 1,
//     "id" : 0xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX,
//     "type" : "image",
//     "resources" : 
//     [ 
//         "f0593503-3168-4904-91b1-a5c9d09ae57b" 
//     ]
// }
//
bool AssetManager::storeAssetDescriptor(AssetDescriptor& descriptor, const std::filesystem::path& assetPath)
{
    // Creates .asset file
    en::storage::File* file = Storage->open(assetPath.string(), en::storage::FileAccess::Write);
    if (!file) // unlikely
    {
        logError("Failed to create asset file:\n%s\n", assetPath.c_str());
        return false;
    }

    AssetID id = descriptor.getID();

    // File will be stored on exit of scope
    {
        WriterJSON writer(*file);
        writer.addKey("version", 1);
        writer.addKey("id", id.description());

        if (descriptor.getType() == AssetType::Image)
        {
            ImageAssetDescriptor& imageDescriptor = *((ImageAssetDescriptor*)&descriptor);

            writer.addKey("type", "image");
            writer.addKeyArray("resources");
            for(uint32 i=0; i<imageDescriptor.sourceFilesCount(); ++i)
            {
                UUID uuid = imageDescriptor.sourceFileUUID(i);
                writer.addValue(uuid.description());
            }
            writer.leaveKeyArray();
        }
    }

    delete file;

    logDebug("C %s - %s\n", id.description().c_str(), assetPath.c_str());

    // Builds relationship between asset ID and its descriptor
    assetDescriptors[id] = &descriptor;

    return true;
}

ParsingResult parseKeyArrayOfUUIDs(ParserJSON& parser, std::vector<UUID>& uuids)
{
    // Array value needs to start with key separator
    JSONType type = JSONType::None;
    ParsingResult result = parser.findNextElement(type);
    if (result != ParsingResult::Success)
    {
        return result;
    }
    if (type != JSONType::Separator)
    {
        return ParsingResult::InvalidFormat;
    }

    // Value is array
    result = parser.findNextElement(type);
    if (result != ParsingResult::Success)
    {
        return result;
    }
    if (type != JSONType::Array)
    {
        return ParsingResult::InvalidFormat;
    }

    do
    {
        result = parser.findNextElement(type);
        if (result != ParsingResult::Success)
        {
            return result;
        }

        if (type == JSONType::Comma)
        {
            continue;
        }
        if (type == JSONType::ArrayTerminator)
        {
            // Value array ended
            break;
        }

        if (type != JSONType::String)
        {
            logError("Invalid JSON syntax when parsing .asset file: Was expecting Value name!\n");
            return ParsingResult::InvalidFormat;
        }

        std::string uuidString(parser.string(), parser.stringLength());

        // Convert string into actual UUID
        UUID uuid;
        if (!uuid.init(uuidString))
        {
            logError("Failed to parse UUID value!\n");
            return ParsingResult::InvalidFormat;
        }

        uuids.push_back(uuid);
        
    } 
    while (result == ParsingResult::Success);

    return result;
}

ParsingResult parseAssetV1(const uint8* buffer, const uint64 size, AssetDescriptor*& descriptor)
{
    // Don't pass buffer ownership
    ParserJSON parser(buffer, size, false);

    // Metadata needs to start with root object
    JSONType type = JSONType::None;
    ParsingResult result = parser.findNextElement(type);
    if (result != ParsingResult::Success)
    {
        return result;
    }
    if (type != JSONType::Object)
    {
        return ParsingResult::InvalidFormat;
    }

    // Parsed state
    uint64 version = 0;
    AssetID id;
    AssetType assetType = AssetType::Unknown;
    std::vector<UUID> resources;

    do
    {
        result = parser.findNextElement(type);
        if (result != ParsingResult::Success)
        {
            return result;
        }

        if (type == JSONType::Comma)
        {
            continue;
        }
        if (type == JSONType::ObjectTerminator)
        {
            // Root object ended
            break;
        }

        if (type != JSONType::String)
        {
            logError("Invalid JSON syntax when parsing .asset file: Was expecting Key name!\n");
            return ParsingResult::InvalidFormat;
        }

        if (parser.isStringMatching("version"))
        {
            result = parser.parseKeyU64(version);
            if (result != ParsingResult::Success)
            {
                return result;
            }
        }
        else
        if (parser.isStringMatching("id"))
        {
            std::string idString;
            result = parser.parseKeyString(idString);
            if (result != ParsingResult::Success)
            {
                return result;
            }

            // Convert string into actual asset ID
            if (!id.init(idString))
            {
                logError("Failed to parse asset ID value!\n");
                return ParsingResult::InvalidFormat;
            }
        }
        else
        if (parser.isStringMatching("type"))
        {
            std::string typeString;
            result = parser.parseKeyString(typeString);
            if (result != ParsingResult::Success)
            {
                return result;
            }

            // Convert string into AssetType
            if (typeString == "image")
            {
                assetType = AssetType::Image;
            }
            else
            {
                logError("Failed to parse asset type!\n");
                return ParsingResult::InvalidFormat;
            }
        }
        else
        if (parser.isStringMatching("resources"))
        {
            result = parseKeyArrayOfUUIDs(parser, resources);
            if (result != ParsingResult::Success)
            {
                return result;
            }
        }
        else
        {
            // Ignoring unrecognized keys
            result = parser.skipKeyValuePair();
            if (result != ParsingResult::Success)
            {
                break;
            }
        }
    } while (result == ParsingResult::Success);

    if (result == ParsingResult::Success)
    {
        if (version != 1)
        {
            logError("Unsupported version of .asset file!\n");
            return ParsingResult::Unsupported;
        }

        if (assetType == AssetType::Image)
        {
            descriptor = new ImageAssetDescriptor(resources);
            descriptor->setID(id);
        }
        else
        {
            logError("Unsupported asset type!\n");
            return ParsingResult::Unsupported;
        }
    }

    return result;
}

bool AssetManager::loadAssetDescriptor(const std::filesystem::path& assetPath)
{
    en::storage::File* file = Storage->open(assetPath.string());
    if (!file) // unlikely
    {
        logError("Failed to open asset file:\n%s\n", assetPath.c_str());
        return false;
    }

    // Verifies file has content
    uint64 size = file->size();
    if (!size) // unlikely
    {
        logError("Asset file is empty:\n%s\n", assetPath.c_str());
        return false;
    }

    // Allocates temporary buffer for parsing purposes
    uint8* buffer = allocate<uint8>(size);
    if (!buffer) // unlikely
    {
        logCritical("Run out of memory while trying to allocate buffer for parsing asset file:\n%s\n", assetPath.c_str());
        return false;
    }

    // Read asset file content to buffer
    uint64 readSize = 0;
    if (!file->read(0, size, buffer, &readSize))
    {
        logError("Failed to read file:\n%s\n", assetPath.c_str());

        if (readSize != size)
        {
            logError("Expected to read %u bytes, but read %u.\n", size, readSize);
        }

        deallocate<uint8>(buffer);
        return false;
    }

    // Read .asset file state and create asset descriptor out of it
    AssetDescriptor* descriptor = nullptr;
    ParsingResult result = parseAssetV1(buffer, size, descriptor);
    deallocate<uint8>(buffer);
    if (result != ParsingResult::Success)
    {
        if (result == ParsingResult::InvalidFormat)
        {
            logError("Invalid JSON syntax when parsing .asset file:\n%s\n", assetPath.c_str());
        }
        else
        if (result == ParsingResult::IncompleteData)
        {
            logError("Buffer storing .asset file content is incomplete!\n%s\n", assetPath.c_str());
        }
        else
        if (result == ParsingResult::Unsupported)
        {
            // Only asset file syntax get validated during load. 
            // Presence of referenced resources, nor their properties are validated
            // until asset is actually created (all referenced resourcs loaded and
            // asset constructed in RAM and VRAM).
            logError("Unsupported asset description in file:\n%s\n", assetPath.c_str());
        }

        return false;
    }

    if (!descriptor) // unlikely
    {
        logCritical("Asset parser code is corrupted!\n");
        return false;
    }

    // If this .asset file was already loaded and added to assets catalog,
    // no additional action is needed (currently parsed descriptor can be released).
    if (assetDescriptors[descriptor->getID()])
    {
        delete descriptor;
        return true;
    }

    logDebug("L %s - %s\n", descriptor->getID().description().c_str(), assetPath.c_str());

    // Populates assets catalog with asset descriptor
    // (builds relationship between asset ID and descriptor)
    assetDescriptors[descriptor->getID()] = descriptor;

    return true;
}

} // en::assets

} // en