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

    // Stores UUID in binary form (currently for simplicity .metadata files are binary)
    file->write(sizeof(UUID), &uuid);
    delete file;

    std::string resourcePath = filePath.string();

    logDebug("C %s - %s\n", uuid.description().c_str(), resourcePath.c_str());

    // Builds relationship between resource UUID and its current path
    resourceUUIDs[resourcePath] = uuid;
    resourcePaths[uuid] = resourcePath;

    return true;
}

bool AssetManager::loadMetadata(const std::filesystem::path& metadataPath, const std::filesystem::path& filePath)
{
    en::storage::File* file = Storage->open(metadataPath.string());
    if (!file)
    {
        logError("Failed to open metadata file:\n%s\n", metadataPath.c_str());
        return false;
    }

    // Verifies file size is what is expected (currently for simplicity .metadata files are binary).
    uint64 size = file->size();
    uint64 expectedSize = sizeof(UUID);
    if (size != expectedSize)
    {
        logError("Metadata file corrupted (size: %u expected: %u)!\n%s\n", size, expectedSize, metadataPath.c_str());
        return false;
    }

    // Read existing UUID from file
    UUID uuid;
    uint64 readSize = 0;
    if (!file->read(0, expectedSize, &uuid, &readSize))
    {
        logError("Failed to read %s file!\n", metadataPath.c_str());

        if (readSize != expectedSize)
        {
            logError("Expected to read %u bytes, but read %u.\n", expectedSize, readSize);
        }

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

} // en::assets

} // en