/*

 Ngine v5.0

 Module      : Resources manager common interface.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_RESOURCES_COMMON_INTERFACE
#define ENG_RESOURCES_COMMON_INTERFACE


#include "resources/interface.h"

#include <unordered_map>

// FBX
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#pragma push_macro("aligned")
#undef aligned

#define FBXSDK_SHARED
#define FBXSDK_NEW_API
#include "fbxsdk.h"

#pragma pop_macro("aligned")

#endif
// FBX

namespace en
{
namespace resources
{

class ResourcesManager : public Interface
{
    gpu::GpuDevice& gpu;
    Streamer gpuStreamer;
    uint8    gpuIndex;

    std::unordered_map< hash, resources::Material* > materials;
    std::unordered_map< hash, TextureAllocation* > textures;

    // Default paths
    struct Paths
    {
        std::string assets;
        std::string fonts;
        std::string models;
        std::string materials;
        std::string shaders;
        std::string textures;
        std::string sounds;
        std::string screenshots;
    } path;                   // Default paths to resources of specified type

    // Default material components
    hash enAlbedoMap;
    hash enMetallicMap;
    hash enCavityMap;
    hash enRoughnessMap;
    hash enAmbientOcclusionMap;
    hash enNormalMap;
    hash enHeightMap;
    hash enDisplacementMap;
    hash enEmmisiveMap;
    hash enOpacityMap;
    hash enReflectionMap;
    hash enAmbientMap;
    hash enBumpMap;
    hash enSpecularExponentMap;
    hash enDecalMap;

    // TODO: Need to resolve how materials and shader programs interact.
    //       I think sampler state should be defined on per-material basis? But maybe it should be shader property?
    //       For sure material constants are stored in some one huge storage buffer, and are independent from programs 
    //       (programs know their layout, but their additional per-program specific constants are stored separately).
    // 
    // std::shared_ptr<gpu::Pipeline> pipeline;            // Default program for materials
    // std::shared_ptr<gpu::Shader>   vertex;
    // std::shared_ptr<gpu
    BufferAllocation* enAxes;   // Default axes buffer (shouldn't it be model?)

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
    FbxManager*           fbxManager;           // FBX Memory Manager
    FbxIOSettings*        fbxFilter;            // Filters content read from FBX
    FbxGeometryConverter* fbxGeometryConverter; // Geometry Converter responsible for triangulation
#endif

public:

    // Internal 

    ResourcesManager(gpu::GpuDevice& gpu);
    virtual void init(void);
    virtual ~ResourcesManager();

    // Interface

    std::string& assetsPath(void) const;
    std::string& screenshotsPath(void) const;

    // Textures

    // It does check if given file was already loaded, and if so,
    // returns its handle instead (as otherwise there would be
    // multiple copies of the same resouce, all having the same
    // hash as their handle).

    // Creates simple 1D/2D/3D texture with mip-map chain
    virtual bool createTexture(const std::string& filename, hash& handle, const bool withMipMaps = true);

    // Creates cube-map texture based on provided texture properties
    // and populates its Top face with that texture content. 
    virtual bool createCubeMap(const std::string& filename, hash& handle, const bool withMipMaps = true);

    // Loads texture from file, into specific mip-map and layer 
    // of already created texture allocation (referenced through 
    // hash handle). This allows populating any vtype of complex
    // texture types (like Cube, Array, etc.).
    virtual bool loadTexture(const std::string& filename, const hash handle, const uint8 mipMapLevel, const uint16 layer = 0u);

    virtual TextureAllocation* findTexture(const std::string& filename);
    virtual TextureAllocation* findTexture(const hash handle);
    virtual bool freeTexture(const hash handle);

    // Materials

    // Internal?
    bool createMaterialFromMTL(mtl::Material& srcMaterial, hash& handle);

    virtual bool loadMaterials(const std::string& filename);
    virtual Material* findMaterial(const std::string& name);
    virtual Material* findMaterial(const hash handle);
    virtual bool freeMaterial(const hash handle);
};

} // en::resources
} // en

#endif

