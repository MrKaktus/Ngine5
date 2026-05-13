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
#include "assets/assets.h"

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

class AssetManager : public Interface
{
    std::string pathAssets;
    std::string pathScreenshots;

    // Resources catalog 

    std::unordered_map<std::string, UUID> resourceUUIDs; // Translates source file path into its UUID.
    std::unordered_map<UUID, std::string> resourcePaths; // Translates UUIDs into source file paths.

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

    // Recursively parses assets directory.
    // - creates UUIDs and .metadata files for resource files missing them
    // - parses .metadata files and builds relations between current resource file paths and their UUIDs
    bool buildResourcesCatalog(void);

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
