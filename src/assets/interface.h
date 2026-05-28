/*

 Ngine v5.0

 Module      : Assets manager.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_ASSETS_INTERFACE
#define ENG_ASSETS_INTERFACE

#include <unordered_map>
#include <filesystem>

#include "core/types/uuid.h"
#include "core/algorithm/hash.h"
#include "assets/assets.h"

// Comment out to use deterministic hash128 instead of UUID
#define EN_ASSET_ID_IS_UUID 

namespace en
{
namespace assets
{
#if defined(EN_ASSET_ID_IS_UUID)
    // Unique identifier
    typedef UUID AssetID;
#else
    // Deterministic imported asset identity, used to track 
    // assets across multiple application runs. Answers 
    // question: which asset is that?
    typedef hash128 AssetID;
#endif

    // Defines runtime asset descriptors equivalence. Answers
    // question: are two asset descriptors semantically identical? 
    // This is used for runtime descriptor reuse, and to
    // avoid duplicate asset catalog entries when creating
    // assets in programmable way.
    typedef hash128 AssetSignature;

} // en::assets

} // en

// Declaring and defining hash operator for AssetID so that it can be used with std::unordered_map
namespace std
{

#if !defined(EN_ASSET_ID_IS_UUID)
template<>
struct hash<en::assets::AssetID>
{
    size_t operator()(const en::assets::AssetID& id) const noexcept
    {
        // AssetID is already unique and result of hashing itself,
        // thus it can be passed as is, as a hash of itself.
        return static_cast<size_t>(id.qword[0]);
    }
};
#else
template<>
struct hash<en::assets::AssetSignature>
{
    size_t operator()(const en::assets::AssetSignature& id) const noexcept
    {
        // AssetSignature is already hash itself, 
        // thus it can be used as its own hash.
        return static_cast<size_t>(id.qword[0]);
    }
};
#endif

} // std

namespace en
{
enum class FileExtension : uint8
{
    Unknown = 0,
    BMP,
    DDS,
    EXR,
    HDR,
    PNG,
    TGA,
    MTL,
    OBJ,
    FBX,
    Material,
    Metadata,
    Asset,
};

namespace assets
{

// Invalid AssetID is 0
#if defined(EN_ASSET_ID_IS_UUID)
#define InvalidAssetID UUID()
#else
#define InvalidAssetID hash128(0, 0)
#endif

// Invalid AssetSignature is 0
#define InvalidAssetSignature hash128(0, 0)

enum class AssetType : uint32
{
    Unknown              = 0,
    Image                   ,
    Count                   ,
};

class AssetDescriptor
{
protected:

    AssetID   assetID;  // Deterministic 128-bit hash
    AssetType type;

public:

    AssetDescriptor(const AssetType type);
    AssetDescriptor(const AssetID id, const AssetType type);

    forceinline void setID(AssetID id) { assetID = id; }; 
    forceinline AssetID getID(void) const { return assetID; };
    forceinline AssetType getType(void) const { return type; };

    virtual AssetSignature getSignature(void) const = 0;
};

class ImageAssetDescriptor : public AssetDescriptor
{
protected:

    friend class Interface;

    // Source:

    // There might be multiple backing files (each for each mip map level).
    std::vector<UUID> sourceFile; // Files storing data backing this image (from mip0 till mip level N, where N is count of elements in this vector). 
                                  // Image may not have mip maps generated for all mip levels (may skip mip tail). 

public:

    // Creates image asset backed by single image file (it has no existing mipmaps).
    ImageAssetDescriptor(const UUID& uuid);

    // Creates image asset backed by group of images, defining consecutive mip levels of its mip-map chain.
    // Amount of passed source image file UUIDs, define amount of mip map levels backed by them.
    // Not all mip-maps need to be backed by resource files. 
    // Remaining mip chain may be unspecified, or requested to be generated at runtime.
    ImageAssetDescriptor(const std::vector<UUID>& uuids);

    forceinline uint32 sourceFilesCount(void) const { return (uint32)sourceFile.size(); };
    forceinline UUID sourceFileUUID(const uint8 mipLevel) const { if (mipLevel >= sourceFilesCount()) { return UUID(); } return sourceFile[mipLevel]; };

    virtual AssetSignature getSignature(void) const;
};

class AssetManager : public Interface
{
    std::string pathAssets;
    std::string pathScreenshots;

    // Resources catalog 

    std::unordered_map<std::string, UUID> resourceUUIDs; // Translates source file path into its UUID.
    std::unordered_map<UUID, std::string> resourcePaths; // Translates UUIDs into source file paths.

    // Asset catalog

    std::unordered_map<AssetID, AssetDescriptor*> assetDescriptors; // Asset description pointed at by AssetID
                                                                    // Either created at runtime, or loaded from disk.

    std::unordered_map<AssetSignature, AssetID> assetSignatureToID; // Used only to avoid duplicate asset descriptors in asset catalog.

private:

    // Resources:

    // Needed when parsing objects that reference other objects by filename
    // on their import time (before those relations are described with UUIDs).
    // Passed in file path is absolute.
    bool findResourceUUIDByPath(const std::string& filePath, UUID& uuid) const;

    // When time comes to load given resource to memory, its current path is acquired based on its UUID.
    bool findResourcePathByUUID(const UUID& uuid, std::string& filePath) const;

    // Creates UUID and stores it in .metadata file. Then creates relation between it and resource file path
    bool storeMetadata(const std::filesystem::path& metadataPath, const std::filesystem::path& filePath);

    // Loads UUID from .metadata file, and creates relation between it and resource file path
    bool loadMetadata(const std::filesystem::path& metadataPath, const std::filesystem::path& filePath);

    // Adds given file to resources catalog (if it is of recognized file format)
    bool addResource(const std::filesystem::path& absolutePath, const bool knownType);

    // Assets:

    // Stores contents of AssetDescriptor object in specified .asset file
    bool storeAssetDescriptor(AssetDescriptor& descriptor, const std::filesystem::path& assetPath);

    // Loads contents of .asset file, parses them creating AssetDescriptor object and adds it to assets catalog.
    bool loadAssetDescriptor(const std::filesystem::path& assetPath);

    // Recursively parses assets directory.
    // - creates UUIDs and .metadata files for resource files missing them
    // - parses .metadata files and builds relations between current resource file paths and their UUIDs
    bool buildResourcesCatalog(void);

    bool buildAssetsCatalog(void);

    bool findAssetIDBySignature(const AssetSignature& signature, AssetID& id) const;
    bool findAssetDescriptorByID(const AssetID id, AssetDescriptor& descriptor) const;

    bool addAssetDescriptor(AssetDescriptor& descriptor);

public:
    static bool create(void);                      // Creates instance of this class (implementation specific) and assigns it to "en::Assets".
    static void destroy(void);                     // Destroys instance of this class

    AssetManager();
   ~AssetManager();

   virtual const std::string& assetsPath(void) const;
   virtual const std::string& screenshotsPath(void) const;
};

} // en::assets

} // en

#endif
