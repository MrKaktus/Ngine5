/*

 Ngine v5.0
 
 Module      : Resources manager.
 Requirements: none
 Description : Loads, stores and manages all types
               of supported resources. Also protects
               from loading duplicates of already
               loaded resources.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/rendering/device.h"
#include "core/rendering/texture.h" // calculateMipMapsCount()
#include "resources/common.h" 
#include "resources/font.h" 
#include "resources/bmp.h"  
#include "resources/tga.h"
#include "resources/png.h" 
#include "resources/exr.h"
#include "resources/hdr.h"
#include "resources/dds.h"
#include "resources/material.h"   
#include "resources/mtl.h"       
#include "resources/obj.h"   
#include "resources/fbx.h"  
#include "resources/wav.h" 

#ifdef EN_PROFILE
#include "utilities/timer.h"
#endif

#include <string>

using namespace en::gpu;

namespace en
{
namespace resources
{

// Coordinate system
// LineStripes
// (x,y,z)(r,g,b)
float axes[] = 
{ 
    0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f, // Y Arrow - Green
    0.0f,   1.0f,   0.0f,   0.0f, 1.0f, 0.0f,
    0.125f, 0.5f,   0.0f,   0.0f, 1.0f, 0.0f,
    0.0f,   0.5f,   0.0f,   0.0f, 1.0f, 0.0f,
    0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f,

    0.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f, // X Arrow - Red
    1.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f, 
    0.5f,   0.0f,   0.125f, 1.0f, 0.0f, 0.0f,
    0.5f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,
    0.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,

    0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 1.0f, // Z Arrow - Blue 
    0.0f,   0.0f,   1.0f,   0.0f, 0.0f, 1.0f, 
    0.0f,   0.125f, 0.5f,   0.0f, 0.0f, 1.0f, 
    0.0f,   0.0f,   0.5f,   0.0f, 0.0f, 1.0f
};

enum class FileExtension : uint8
{
    Unknown = 0,
    BMP        ,
    PNG        ,
    MTL        ,
    Material   ,
};

bool isBMP(const std::string& filename)
{
    uint32 length = filename.length();

    uint32 found = filename.rfind(".bmp");
    if (found == std::string::npos)
    {
        found = filename.rfind(".BMP");
    }
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return true;
    }

    return false;
}

bool isPNG(const std::string& filename)
{
    uint32 length = filename.length();

    uint32 found = filename.rfind(".png");
    if (found == std::string::npos)
    {
        found = filename.rfind(".PNG");
    }
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return true;
    }

    return false;
}

bool isMTL(const std::string& filename)
{
    uint32 length = filename.length();

    uint32 found = filename.rfind(".mtl");
    if (found == std::string::npos)
    {
        found = filename.rfind(".MTL");
    }
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return true;
    }

    return false;
}

bool isMATERIAL(const std::string& filename)
{
    uint32 length = filename.length();

    uint32 found = filename.rfind(".material");
    if (found == std::string::npos)
    {
        found = filename.rfind(".MATERIAL");
    }
    if (found != std::string::npos &&
        found == (length - 9))
    {
        return true;
    }

    return false;
}

FileExtension fileExtension(const std::string& filename)
{
    // Images

    if (isBMP(filename))
    {
        return FileExtension::BMP;
    }
    if (isPNG(filename))
    {
        return FileExtension::PNG;
    }

    // Materials

    if (isMTL(filename))
    {
        return FileExtension::BMP;
    }
    if (isMATERIAL(filename))
    {
        return FileExtension::PNG;
    }

    return FileExtension::Unknown;
}

bool isImageFileExtension(const FileExtension& extension)
{
    if (extension == FileExtension::BMP ||
        extension == FileExtension::PNG)
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ResourcesManager::ResourcesManager(gpu::GpuDevice& _gpu) :
    gpu(_gpu),
    gpuStreamer(gpu),
    gpuIndex(0),  // TODO: Query index from GPU!
    enAlbedoMap(InvalidHash),
    enMetallicMap(InvalidHash),
    enCavityMap(InvalidHash),
    enRoughnessMap(InvalidHash),
    enAmbientOcclusionMap(InvalidHash),
    enNormalMap(InvalidHash),
    enHeightMap(InvalidHash),
    enDisplacementMap(InvalidHash),
    enEmmisiveMap(InvalidHash),
    enOpacityMap(InvalidHash),
    enReflectionMap(InvalidHash),
    enAmbientMap(InvalidHash),
    enBumpMap(InvalidHash),
    enSpecularExponentMap(InvalidHash),
    enDecalMap(InvalidHash)
    // TODO: Should be EN_SUPPORTS_FBX (and best solution would be plugin system for different formats support through DLLs).
#if defined(EN_PLATFORM_WINDOWS)
    ,
    fbxManager(nullptr),
    fbxFilter(nullptr),
    fbxGeometryConverter(nullptr)
#endif
{
    logInfo("Starting module: Resources.\n");

    path.assets      = std::string("assets/");
    path.fonts       = path.assets + std::string("fonts/");
    path.models      = path.assets + std::string("models/");
    path.materials   = path.assets + std::string("materials/");
    path.shaders     = path.assets + std::string("shaders/");
    path.textures    = path.assets + std::string("textures/");
    path.sounds      = path.assets + std::string("sounds/"); 
    path.screenshots = std::string("screenshots/");

    // Creates textures for default material
    // When selected texture/material/model is missing, it is substituted with default one (and error is logged).
    // See UV Checker Map Maker: https://uvchecker.vinzi.xyz/
    createTexture(path.textures + "enDefaultAlbedoMap.png",        enAlbedoMap);
    createTexture(path.textures + "enDefaultMetallicMap.png",      enMetallicMap);
    createTexture(path.textures + "enDefaultCavityMap.png",        enCavityMap);
    createTexture(path.textures + "enDefaultRoughness.png",        enRoughnessMap);
    createTexture(path.textures + "enDefaultAmbientOcclusion.png", enAmbientOcclusionMap);
    createTexture(path.textures + "enDefaultNormal.png",           enNormalMap);
    createTexture(path.textures + "enDefaultHeight.png",           enHeightMap);
    createTexture(path.textures + "enDefaultDisplacement.png",     enDisplacementMap);
    createTexture(path.textures + "enDefaultEmmisive.png",         enEmmisiveMap);
    createTexture(path.textures + "enDefaultOpacity.png",          enOpacityMap);
  //createTexture(path.textures + "enReflection.png",       enReflectionMap);
  //createTexture(path.textures + "enAmbient.png",          enAmbientMap);
  //createTexture(path.textures + "enBump.png",             enBumpMap);
  //createTexture(path.textures + "enSpecularExponent.png", enSpecularExponentMap);
  //createTexture(path.textures + "enDecal.png",            enDecalMap);

    // TODO: Load/create default material/model!

#if defined(EN_PLATFORM_WINDOWS)
    fbxManager = FbxManager::Create();
    fbxFilter  = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(fbxFilter);
    fbxGeometryConverter = new FbxGeometryConverter(fbxManager);
#endif
}

ResourcesManager::~ResourcesManager()
{
    logInfo("Closing module: Resources.\n");

#if defined(EN_PLATFORM_WINDOWS)
    if (fbxManager)
    {
        fbxManager->Destroy();
        delete fbxGeometryConverter;
    }
#endif

    freeTexture(enAlbedoMap);
    freeTexture(enMetallicMap);
    freeTexture(enCavityMap);
    freeTexture(enRoughnessMap);
    freeTexture(enAmbientOcclusionMap);
    freeTexture(enNormalMap);
    freeTexture(enHeightMap);
    freeTexture(enDisplacementMap);
    freeTexture(enEmmisiveMap);
    freeTexture(enOpacityMap);
  //freeTexture(enReflectionMap);
  //freeTexture(enAmbientMap);
  //freeTexture(enBumpMap);
  //freeTexture(enSpecularExponentMap);
  //freeTexture(enDecalMap);
}

void ResourcesManager::init(void)
{
}

std::string& ResourcesManager::assetsPath(void) const
{
}

std::string& ResourcesManager::screenshotsPath(void) const
{
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool ResourcesManager::createTexture(const std::string& filename, hash& handle, const bool withMipMaps)
{
    // TODO: Resources Manager is loading textures in a way that is optimal for renderer 
    //       (so creates assets in predefined way, for e..g packing them into texture atlasses?)

    if (filename.empty())
    {
        logError("Cannot load texture because file name is empty!\n");
        assert(0);

        return false;
    }

    TextureAllocation* textureAllocation = findTexture(filename);
    if (textureAllocation)
    {
        handle = hashString(filename);
        return true;
    }

    FileExtension extension = fileExtension(filename);
    if (!isImageFileExtension(extension))
    {
        logError("File format is not recognized as image:\n%s\n", filename.c_str());
        return false;
    }

    // Automatically creates texture state based on information from loaded file
    gpu::TextureState textureState;

    if (extension == FileExtension::PNG)
    {
        gpu::ColorSpace colorSpace = gpu::ColorSpaceSRGB;
        if (!png::loadMetadata(filename, textureState, colorSpace))
        {
            logError("Could not parse PNG metadata!\n");
            return false;
        }
    }
    else
    {
        // TODO: Add support for other recognized image file extensions
        assert(0);
        return false;
    }

    if (withMipMaps)
    {
        textureState.mipmaps = gpu::calculateMipMapsCount(textureState);
    }

    // Allocate resource
    // TODO: Based on passed TextureUsage in TextureState, Streamer detects if
    //       texture should be partially backed in VRAM (Sparse), or fully.
    //       This will also dicta
    bool result = gpuStreamer.allocateMemory(textureAllocation, textureState);
    assert(result);

    // TODO: Notify Streamer, that app wants to update given portion of resource.
    //       This portion can be one surface (layer of mipmap), or tile.
    //       Streamer decides, if it will make temporary allocation for that
    //       upload, or if it will cache given slice in CPU RAM for faster
    //       future uploads (in case of eviction).
    // TODO: Or maybe this decision should be made by higher level?
    //       Whoever decides what should be cached in CPU RAM or not, will be
    //       responsible for making sure that it's reuploaded to that RAM,
    //       when needed (VRAM eviction, and missing in RAM -> HDD upload needed).


    // Populate resource with data

    // Retrieve system memory adress for given texture surface
    // (mipmap, layer, plane are optional parameters).
    uint8* address = reinterpret_cast<uint8*>(gpuStreamer.systemMemory(*textureAllocation));

    // Decompress texture directly to staging buffer, taking into notice padding
    if (extension == FileExtension::PNG)
    {
        result = png::load(filename,
                           address,
                           textureAllocation->state.mipWidth(0),
                           textureAllocation->state.mipHeight(0),
                           textureAllocation->state.format,
                           gpu.textureMemoryAlignment(textureAllocation->state, 0, 0),
                           false);
        assert(result);
    }
    else
    {
        // TODO: Add support for other recognized image file extensions
        assert(0);
        return false;
    }

    // Allocate copy in VRAM
    result = gpuStreamer.makeResident(*textureAllocation, true);
    assert(result);

    // Update copy in VRAM (async from this thread)
    result = gpuStreamer.transferSurface(*textureAllocation, 0, 0, 0, TransferDirection::DeviceUpload);
    assert(result);

    // Adds texture allocation to namespace
    hash handle = hashString(filename);
    textures[handle] = textureAllocation;

    return true;
}

bool ResourcesManager::createCubeMap(const std::string& filename, hash& handle, const bool withMipMaps)
{
    if (filename.empty())
    {
        logError("Cannot load texture because file name is empty!\n");
        assert(0);

        return false;
    }

    TextureAllocation* textureAllocation = findTexture(filename);
    if (textureAllocation)
    {
        if (textureAllocation->state.type != TextureType::TextureCubeMap)
        {
            logError("There is already texture of different type than CubeMap, stored under this filename!\n");
            assert(0);

            return false;
        }

        handle = hashString(filename);
        return true;
    }

    FileExtension extension = fileExtension(filename);
    if (!isImageFileExtension(extension))
    {
        logError("File format is not recognized as image:\n%s\n", filename.c_str());
        return false;
    }

    // Automatically creates texture state based on information from loaded file
    gpu::TextureState textureState;

    if (extension == FileExtension::PNG)
    {
        gpu::ColorSpace colorSpace = gpu::ColorSpaceSRGB;
        if (!png::loadMetadata(filename, textureState, colorSpace))
        {
            logError("Could not parse PNG metadata!\n");
            return false;
        }
    }
    else
    {
        // TODO: Add support for other recognized image file extensions
        assert(0);
        return false;
    }

    // Verify given image file can be loaded as cube map face
    if (textureState.width != textureState.height)
    {
        logError("Specified image file is not square!\n");
        return false;
    }

    textureState.type = TextureType::TextureCubeMap;
    if (withMipMaps)
    {
        textureState.mipmaps = gpu::calculateMipMapsCount(textureState);
    }

    // Allocate resource
    // TODO: Based on passed TextureUsage in TextureState, Streamer detects if
    //       texture should be partially backed in VRAM (Sparse), or fully.
    //       This will also dicta
    bool result = gpuStreamer.allocateMemory(textureAllocation, textureState);
    assert(result);

    // TODO: Notify Streamer, that app wants to update given portion of resource.
    //       This portion can be one surface (layer of mipmap), or tile.
    //       Streamer decides, if it will make temporary allocation for that
    //       upload, or if it will cache given slice in CPU RAM for faster
    //       future uploads (in case of eviction).
    // TODO: Or maybe this decision should be made by higher level?
    //       Whoever decides what should be cached in CPU RAM or not, will be
    //       responsible for making sure that it's reuploaded to that RAM,
    //       when needed (VRAM eviction, and missing in RAM -> HDD upload needed).


    // Populate resource with data

    // Retrieve system memory adress for given texture surface
    // (mipmap, layer, plane are optional parameters).
    uint8* address = reinterpret_cast<uint8*>(gpuStreamer.systemMemory(*textureAllocation));

    // Decompress texture directly to staging buffer, taking into notice padding
    if (extension == FileExtension::PNG)
    {
        result = png::load(filename,
            address,
            textureAllocation->state.mipWidth(0),
            textureAllocation->state.mipHeight(0),
            textureAllocation->state.format,
            gpu.textureMemoryAlignment(textureAllocation->state, 0, 0),
            false);
        assert(result);
    }
    else
    {
        // TODO: Add support for other recognized image file extensions
        assert(0);
        return false;
    }

    // Allocate copy in VRAM
    result = gpuStreamer.makeResident(*textureAllocation, true);
    assert(result);

    // Update copy in VRAM (async from this thread)
    result = gpuStreamer.transferSurface(*textureAllocation, 0, 0, 0, TransferDirection::DeviceUpload);
    assert(result);

    // Adds texture allocation to namespace
    hash handle = hashString(filename);
    textures[handle] = textureAllocation;

    return true;
}

bool ResourcesManager::loadTexture(
    const std::string& filename, 
    const hash handle, 
    const uint8 mipMapLevel,
    const uint16 layer)
{
    if (filename.empty())
    {
        logError("Cannot load texture because file name is empty!\n");
        assert(0);

        return false;
    }

    std::unordered_map<hash, TextureAllocation*>::iterator it = textures.find(handle);
    if (it == textures.end())
    {
        logError("Cannot find texture for hash: %llu\n", handle);
        return false;
    }

    TextureAllocation* textureAllocation = it->second;

    FileExtension extension = fileExtension(filename);
    if (!isImageFileExtension(extension))
    {
        logError("File format is not recognized as image:\n%s\n", filename.c_str());
        return false;
    }

    // Automatically creates texture state based on information from loaded file
    gpu::TextureState textureState;

    if (extension == FileExtension::PNG)
    {
        gpu::ColorSpace colorSpace = gpu::ColorSpaceSRGB;
        if (!png::loadMetadata(filename, textureState, colorSpace))
        {
            logError("Could not parse PNG metadata!\n");
            return false;
        }
    }
    else
    {
        // TODO: Add support for other recognized image file extensions
        assert(0);
        return false;
    }

    // TODO: Notify Streamer, that app wants to update given portion of resource.
    //       This portion can be one surface (layer of mipmap), or tile.
    //       Streamer decides, if it will make temporary allocation for that
    //       upload, or if it will cache given slice in CPU RAM for faster
    //       future uploads (in case of eviction).
    // TODO: Or maybe this decision should be made by higher level?
    //       Whoever decides what should be cached in CPU RAM or not, will be
    //       responsible for making sure that it's reuploaded to that RAM,
    //       when needed (VRAM eviction, and missing in RAM -> HDD upload needed).


    // Populate resource with data

    // Retrieve system memory adress for given texture surface
    // (mipmap, layer, plane are optional parameters).
    uint8* address = reinterpret_cast<uint8*>(gpuStreamer.systemMemory(*textureAllocation));

    // Decompress texture directly to staging buffer, taking into notice padding
    bool result = false;
    if (extension == FileExtension::PNG)
    {
        result = png::load(filename,
                           address,
                           textureAllocation->state.mipWidth(mipMapLevel),
                           textureAllocation->state.mipHeight(mipMapLevel),
                           textureAllocation->state.format,
                           gpu.textureMemoryAlignment(textureAllocation->state, mipMapLevel, layer),
                           false);
        assert(result);
    }
    else
    {
        // TODO: Add support for other recognized image file extensions
        assert(0);
        return false;
    }

    // Allocate copy in VRAM
    result = gpuStreamer.makeResident(*textureAllocation, true);
    assert(result);

    // Update copy in VRAM (async from this thread)
    result = gpuStreamer.transferSurface(*textureAllocation, mipMapLevel, layer, 0, TransferDirection::DeviceUpload);
    assert(result);

    // Adds texture allocation to namespace
    hash handle = hashString(filename);
    textures[handle] = textureAllocation;

    return true;
}

TextureAllocation* ResourcesManager::findTexture(const std::string& filename)
{
    return findTexture(hashString(filename));
}

TextureAllocation* ResourcesManager::findTexture(const hash handle)
{
    std::unordered_map<hash, TextureAllocation*>::iterator it = textures.find(handle);
    if (it != textures.end())
    {
        return it->second;
    }

    return nullptr;
}

bool ResourcesManager::freeTexture(const hash handle)
{
    std::unordered_map<hash, TextureAllocation*>::iterator it = textures.find(handle);
    if (it != textures.end())
    {
        gpuStreamer.deallocateMemory(*it->second);
        textures.erase(it);

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool ResourcesManager::createMaterialFromMTL(mtl::Material& srcMaterial, hash& handle)
{
    resources::Material* dstMaterial = findMaterial(srcMaterial.name);
    if (dstMaterial)
    {
        handle = hashString(srcMaterial.name);
        return true;
    }

    // Verify source material
    if (srcMaterial.shadingModel == ShadingModel::PBR)
    {
        // TODO: Error message, release temp resources
        return false;
    }

    // TODO: Allocate new material in some global (or Scene) pool?
    // Material -> component (filename) -> parse file -> extract texture state + data -> create GPU Texture -> create TextureAllocation -> stream data in.
    dstMaterial = new resources::Material();
    if (!dstMaterial)
    {
        logError("Out of memory.\n");
        return false;
    }

    hash handle = hashString(srcMaterial.name);

    // MTL does not have ability to describe those properties of PBR
    dstMaterial->components[underlyingType(MaterialComponent::Metallic)]         = InvalidHash;
    dstMaterial->components[underlyingType(MaterialComponent::Roughness)]        = InvalidHash;
    dstMaterial->components[underlyingType(MaterialComponent::AmbientOcclusion)] = InvalidHash;
    dstMaterial->components[underlyingType(MaterialComponent::Emissive)]         = InvalidHash;

    // Optional: Ambient map
    if (!srcMaterial.ambient.map.filename.empty())
    {
        if (!createTexture(srcMaterial.ambient.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Ambient)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }
    else
    {
        if (srcMaterial.ambient.color.r != 0.0f &&
            srcMaterial.ambient.color.g != 0.0f &&
            srcMaterial.ambient.color.b != 0.0f)
        {
            // TODO: Generate 4x4 ambient map from constant color (to unify binding model)
            //       Alternative is to put this value into Constant Buffer.
            //       Renderer material needs to be composed from two parts:
            //       a) Array of texture views (descriptors) pointing at different material components
            //       b) Struct with constants
            //       c) Sampler states to use for textures from (a) ?
            //       Both are sub-allocated frm some big materials array in materials descriptors set and materials storage buffer
        }
    }

    // Mandatory: Diffuse map
    if (!srcMaterial.diffuse.map.filename.empty())
    {
        // TODO: Error message, release temp resources
        return false;
    }

    if (!createTexture(srcMaterial.diffuse.map.filename,
                       dstMaterial->components[underlyingType(MaterialComponent::Albedo)]))
    {
        // TODO: Error message, release temp resources
        return false;
    }

    // Optional: Specular map
    if (!srcMaterial.specular.map.filename.empty())
    {
        if (!createTexture(srcMaterial.specular.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Cavity)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }
    // TODO: Else default specular map 4x4 with no specular reflection (default, only diffuse term)

    // Optional: Specular exponent map
    dstMaterial->components[underlyingType(MaterialComponent::SpecularExponent)] = InvalidHash;
    if (!srcMaterial.specular.exponentMap.filename.empty())
    {
        if (!createTexture(srcMaterial.specular.exponentMap.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::SpecularExponent)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }

    // Optional: Opacity map
    dstMaterial->components[underlyingType(MaterialComponent::Opacity)] = InvalidHash;
    if (!srcMaterial.opacity.map.filename.empty())
    {
        if (!createTexture(srcMaterial.opacity.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Opacity)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }

    // Optional: Bump map
    dstMaterial->components[underlyingType(MaterialComponent::Bump)] = InvalidHash;
    if (!srcMaterial.bump.map.filename.empty())
    {
        if (!createTexture(srcMaterial.bump.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Bump)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }

    // Optional: Normal map
    dstMaterial->components[underlyingType(MaterialComponent::Normal)] = InvalidHash;
    if (!srcMaterial.normal.map.filename.empty())
    {
        if (!createTexture(srcMaterial.normal.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Normal)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }
    // TODO: Else default normal map 4x4 with normal vector pointing straight up (so no effect)

    // Optional: Height map
    dstMaterial->components[underlyingType(MaterialComponent::Height)] = InvalidHash;
    if (!srcMaterial.height.map.filename.empty())
    {
        if (!createTexture(srcMaterial.height.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Height)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }

    // Optional: Vector displacement map
    dstMaterial->components[underlyingType(MaterialComponent::Displacement)] = InvalidHash;
    if (!srcMaterial.displacement.map.filename.empty())
    {
        if (!createTexture(srcMaterial.displacement.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Displacement)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }

    // Optional: Decal map
    dstMaterial->components[underlyingType(MaterialComponent::Decal)] = InvalidHash;
    if (!srcMaterial.decal.map.filename.empty())
    {
        if (!createTexture(srcMaterial.decal.map.filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Decal)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }

    // Optional: Local reflection map
    dstMaterial->components[underlyingType(MaterialComponent::Reflection)] = InvalidHash;
    if (srcMaterial.reflection.projection == mtl::ProjectionType::Cube)
    {
        // 6 reflection source textures (composing one reflection cube-map)
        if (!createCubeMap(srcMaterial.reflection.map[0].filename,
                          dstMaterial->components[underlyingType(MaterialComponent::Reflection)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }

        for(uint32 i=1; i<6; ++i)
        {
            if (!loadTexture(srcMaterial.reflection.map[i].filename,
                             dstMaterial->components[underlyingType(MaterialComponent::Reflection)], 
                             i, 0))
            {
                // TODO: Error message, release temp resources
                return false;
            }
        }
    }
    else
    if (srcMaterial.reflection.projection == mtl::ProjectionType::Equirectangular)
    {
        if (!createTexture(srcMaterial.reflection.map[0].filename,
                           dstMaterial->components[underlyingType(MaterialComponent::Reflection)]))
        {
            // TODO: Error message, release temp resources
            return false;
        }
    }
    else
    {
        // TODO: Error message, release temp resources
        assert(0);

        return false;
    }

    // TODO: consider carrying over other properties of srcMaterial

    if (materials.find(handle) != materials.end())
    {
        // Entry was not found at the start of the method, so this should not happen unless we have mutli-threaded collision and we need lock on resource manager
        assert(0);

        logError("Out of memory.\n");
        return false;
    }

    materials[handle] = dstMaterial;
    return true;
}

bool ResourcesManager::loadMaterials(const std::string& filename)
{
    if (filename.empty())
    {
        logError("Cannot load materials because file name is empty!\n");
        assert(0);

        return false;
    }

    FileExtension extension = fileExtension(filename);
    if (!isMaterialFileExtension(extension))
    {
        logError("File format is not recognized as material:\n%s\n", filename.c_str());
        return false;
    }

    if (extension == FileExtension::MTL)
    {
        std::vector<mtl::Material*>* srcMaterials = mtl::load(filename);
        if (!srcMaterials)
        {
            return false;
        }

        for(uint32 i=0; i<srcMaterials->size(); ++i)
        {
            mtl::Material* srcMaterial = (*srcMaterials)[i];
            if (!srcMaterial)
            {
                // TODO: this should never happen
                assert(0);
                continue;
            }

            hash handle = InvalidHash;
            if (!createMaterialFromMTL(*srcMaterial, handle))
            {
                // TODO: Whats the best behavior in such case?
                continue;
            }
        }
    }
    else
    if (extension == FileExtension::Material)
    {
        // material::load(filename);
    }
    else
    {
        // TODO: Add support for other recognized material file extensions
        assert(0);
        return false;
    }
}

Material* ResourcesManager::findMaterial(const std::string& filename)
{
    return findMaterial(hashString(filename));
}

Material* ResourcesManager::findMaterial(const hash handle)
{
    std::unordered_map<hash, Material*>::iterator it = materials.find(handle);
    if (it != materials.end())
    {
        return it->second;
    }

    return nullptr;
}

bool ResourcesManager::freeMaterial(const hash handle)
{
    std::unordered_map<hash, Material*>::iterator it = materials.find(handle);
    if (it != materials.end())
    {
        // TODO: release backing textures: gpuStreamer.deallocateMemory(*it->second);
        materials.erase(it);

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









//Material Interface::Load::material(const std::string& filename, const std::string& name)
//{
//uint32 found;
//uint32 length = filename.length();

//// Try to reuse already loaded material
//if (ResourcesContext.materials.find(name) != ResourcesContext.materials.end())
//   return ResourcesContext.materials[name];

//found = 

//return Material();
//}

//Material Interface::Load::material(const std::string& filename)
//{
//uint32 found;
//uint32 length = filename.length();

//// Try to reuse already loaded material
//if (ResourcesContext.materials.find(filename) != ResourcesContext.materials.end())
//   return ResourcesContext.materials[filename];



//return Material();
//}


















Context::Defaults::Defaults() :
    //program(nullptr),
    enHeapBuffers(nullptr),
    enHeapTextures(nullptr),
    enStagingHeap(nullptr),

    enAxes(nullptr)
{
}





void Context::create(void)
{
    using namespace en::gpu;

    

    models.clear();
    materials.clear();
    textures.clear();

    // Create default program for materials
    std::string vsCode, fsCode;
/*
#ifdef EN_OPENGL_DESKTOP
    en::Storage->read(std::string("resources/engine/shaders/default.glsl.1.10.vs"), vsCode);
    en::Storage->read(std::string("resources/engine/shaders/default.glsl.1.10.fs"), fsCode);
#endif
#ifdef EN_OPENGL_MOBILE
    en::Storage->read(std::string("resources/engine/shaders/default.essl.1.00.vs"), vsCode);
    en::Storage->read(std::string("resources/engine/shaders/default.essl.1.00.fs"), fsCode);
#endif
//*/
    //vector<gpu::Shader> shaders(2, Shader(nullptr));
    //shaders[0] = Gpu.shader.create(Vertex, vsCode);
    //shaders[1] = Gpu.shader.create(Fragment, fsCode);
    //defaults.program     = Gpu.program.create(shaders);
}

void Context::destroy(void)
{
    //defaults.program                 = nullptr;
 
    defaults.enAxes            = nullptr;

    models.clear();
    materials.clear();
    textures.clear();


}

 //MaterialParameter::MaterialParameter() :
 //    handle(NULL),
 //    value(NULL),
 //    name(NULL)
 //{
 //}

 //MaterialParameter::~MaterialParameter()
 //{
 //    // Don't delete "value" as it points to shared buffer
 //    // Don't delete "name" as it points to shared string
 //}

 //MaterialSampler::MaterialSampler() :
 //    handle(NULL),
 //    texture(NULL),
 //    name(NULL)
 //{
 //}

 //MaterialSampler::~MaterialSampler()
 //{
 //    // Don't delete "name" as it points to shared string
 //}

MaterialDescriptor::MaterialDescriptor() /*:*/
//    program(nullptr)
{
/*
    parameters.list.clear();
    parameters.buffer = NULL;
    parameters.size   = 0;
    
    samplers.clear();
    
    state.texture.ambient      = false;
    state.texture.diffuse      = false;
    state.texture.specular     = false;
    state.texture.displacement = false;
    state.texture.normal       = false;
    state.texture.alpha        = false;
    
    state.transparent          = false;
//*/
}

MaterialDescriptor::~MaterialDescriptor()
{
    //parameters.list.clear();
    //delete static_cast<uint8*>(parameters.buffer);
    //samplers.clear();
}

bool MaterialDestroy(MaterialDescriptor* const material)
{
    // Fill material with null pointer
    //ResourcesContext.storage.materials.free(material);
    return true; 
}

//Material::Material() :
//    ProxyInterface<struct MaterialDescriptor>(NULL)
//{
//}
       
//Material::Material(struct MaterialDescriptor* src) :
//    ProxyInterface<struct MaterialDescriptor>(src)
//{
//}

//gpu::Program& Material::program(void)
//{
//assert(pointer);

//return pointer->program;
//}

//uint32 Material::parameters(void)
//{
//assert(pointer);

//return pointer->parameters.list.size();
//}

//uint32 Material::samplers(void)
//{
//assert(pointer);

//return pointer->samplers.size();
//}

//bool Material::uses(enum MaterialFeature feature)
//{
//assert(pointer);

//MaterialDescriptor& material = *pointer;

//// TODO: In future, use lookup table for direct access
//if (feature == AmbientMap)
//   return material.state.texture.ambient;
//if (feature == DiffuseMap)
//   return material.state.texture.diffuse;
//if (feature == SpecularMap)
//   return material.state.texture.specular;
//if (feature == DisplacementMap)
//   return material.state.texture.displacement;
//if (feature == NormalMap)
//   return material.state.texture.normal;
//if (feature == AlphaMap)
//   return material.state.texture.alpha;

//return false;
//}

//void Material::update(void)
//{
//assert(pointer);

//MaterialDescriptor& material = *pointer;

//// Update material parameters with default values
//for(uint32 i=0; i<material.parameters.list.size(); ++i)
//   {
//   MaterialParameter& parameter = material.parameters.list[i];
//   if (parameter.value)
//      parameter.handle.set(parameter.value);
//   }
//
//// Bind textures to samplers
//for(uint32 i=0; i<material.samplers.size(); ++i)
//   material.samplers[i].handle.set(material.samplers[i].texture);
//}

//Mesh::Geometry::Geometry(const gpu::Buffer& vertex) :
//   buffer(vertex),
//   begin(0),
//   end(0)
//{
//}

//Mesh::Elements::Elements(const en::gpu::Buffer& index) :
//   buffer(index),
//   begin(0),
//   end(0)
//{
//}

//Mesh::Mesh(const en::gpu::Buffer& vertex, const en::gpu::Buffer& index) :
//   name(""),
//   material(NULL),
//   type(en::gpu::Triangles),
//   geometry(vertex),
//   elements(index)
//{
//}

//Material::Material() :
//   name("default")
//{
//assert(Gpu.screen.created());
//emmisive.map         = ResourcesContext.defaults.enEmmisiveMap;
//emmisive.color       = float3(1.0f, 1.0f, 1.0f);
//ambient.map          = ResourcesContext.defaults.enAmbientMap;
//ambient.color        = float3(1.0f, 1.0f, 1.0f);
//diffuse.map          = ResourcesContext.defaults.enDiffuseMap;
//diffuse.color        = float3(1.0f, 1.0f, 1.0f);
//specular.map         = ResourcesContext.defaults.enSpecularMap;
//specular.color       = float3(1.0f, 1.0f, 1.0f);
//specular.exponent    = 1.0f;
//transparency.on      = false;
//transparency.map     = ResourcesContext.defaults.enTransparencyMap;
//transparency.color   = float3(1.0f, 1.0f, 1.0f);
//normal.map           = ResourcesContext.defaults.enNormalMap;
//displacement.map     = ResourcesContext.defaults.enDisplacementMap;
//displacement.vectors = ResourcesContext.defaults.enVectorsMap;
//};


Material::Material() :
    name("default")
{
/*
    //assert(Gpu.screen.created());
    albedo       = ResourcesContext.defaults.enAlbedoMap;
    metallic     = ResourcesContext.defaults.enMetallicMap;
    cavity       = ResourcesContext.defaults.enCavityMap;
    roughness    = ResourcesContext.defaults.enRoughnessMap;
    AO           = ResourcesContext.defaults.enAOMap;
    normal       = ResourcesContext.defaults.enNormalMap;
    displacement = ResourcesContext.defaults.enDisplacementMap;
    vector       = ResourcesContext.defaults.enVectorDisplacementMap;
    emmisive     = ResourcesContext.defaults.enEmmisiveMap;
    opacity      = ResourcesContext.defaults.enOpacityMap;
//*/
};








Mesh::Mesh()
{
    memset(this, 0, sizeof(Mesh));
}

Mesh& Mesh::operator=(const Mesh& src)
{
    memcpy(this, &src, sizeof(Mesh));
    return *this;
}

/*
InputLayout* Mesh::inputLayout(GpuDevice& gpu)
{
    /// Specialized function for creation of any type of InputLayout.
    return gpu->createInputLayout(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const uint32 usedAttributes,
        const uint32 usedBuffers,
        const AttributeDesc * attributes,
        const BufferDesc * buffers) = 0;
}
//*/



/*
// It's easier to recompute below values at runtime each time they are needed, 
// rather than caching them, which would increase total Mesh size. We try to 
// minimize memory loads in exchange of computation.

// Called each time, given LOD becomes resident in GPU dedicated memory
void updateModelLevel(Model& model, const uint32 level)
{
    assert(model.levelCount > level);

    ModelLevel& modelLevel = model.level[level];

    // Patch offsets of input and index buffers in each mesh belonging to this level
    for(uint32 i=0; i<modelLevel.meshRange.count; ++i)
    {
        Mesh& mesh = model.mesh[modelLevel.meshRange.first + i];

        // Update offsets of input buffers, based on current GPU residency
        for(uint32 j=0; j<mesh.bufferCount; ++j)
        {
            mesh.gpuOffset[j] = modelLevel.buffer.gpuOffset + mesh.offset[j];
        }

        // Calculate offset to first Vertex, for draws using only one input buffer
        mesh.firstVertex = mesh.gpuOffset[0] / mesh.elementSize[0];

        // Calculate offset to optional sub-allocated IndexBuffer
        if (mesh.indexCount)
        {
            mesh.firstIndex = (modelLevel.buffer.gpuOffset + mesh.indexOffset) >> mesh.indexShift;
        }
    }
}
//*/




Model::Model(const std::string& _name, const uint32 meshesCount, const uint32 levelsOfDetail, const uint8 gpuIndex)
{
    // TODO: Search by hash if model already exists, if so, load only if it is for GPU its not loaded for
    name = hashString(_name);
    meshCount = meshesCount;
    levelsODetailCount(levelsOfDetail);
    setBit(gpuMask, gpuIndex);
    mesh = new resources::Mesh[meshesCount]; // TODO: Allocate from some global meshes array
    meshRange = nullptr; // Threre is always only one LOD in one OBJ file.
    matrix = nullptr; // OBJ file does not store animation, nor mesh-tree transforms
    padding[0] = 0;
    padding[1] = 0;

    for (uint32 i = 0; i < MaxSupportedGPUCount; ++i)
    {
        backing[i] = nullptr;
    }

    padding2 = 0;
}

CommandState::CommandState(gpu::CommandBuffer& _command) :
    command(_command),
    inputHash(0),
    indexHash(0),
    indexShift(0),
    gpuIndex(0)
{
}

// bufferMask - bitmask identifying which buffers to bind
// TODO: Should this be Renderer method because renderer has broader context that Mesh is missing (like which material to use in case of instanced draw and instanced buffer setup).
//       Most of Mesh internals are used here, but some Renderer state as well.
void Model::drawLOD(
    CommandState& state,
    const uint32 levelOfDetail, 
          uint32 bufferMask,
    const uint32 firstInstance,
    const uint32 instanceCount)
{
    assert(levelCount > levelOfDetail);

    // If there is only one LOD, all meshes belong to it (and LOD array is not needed).
    uint16v2 levelRange(0, meshCount);
    if (meshRange)
    {
        levelRange = meshRange[levelOfDetail];
    }

    // Whoever calls this method, should ensure that model is managed by
    // GPU streamer for GPU for which it is supposed to be drawn
    assert(checkBit(gpuMask, state.gpuIndex));

    if (!backing[state.gpuIndex]) // [[unlikely]]
    {
        // Silently skips drawing if backing buffer is not allocated for selected GPU
        return;
    }

    BufferAllocation& levelBacking = *backing[state.gpuIndex];

    if (!levelBacking.resident)
    {
        // Silently skips drawing if backing buffer is not resident on selected GPU
        return;
    }

    // Single input buffer is used (Position and optional UV)
    if (bufferMask == 1U)
    {
        // Bind first input buffer, only if it wasn't bound yet, it was bound 
        // with offset different than 0 in multi buffer draw, or other one was 
        // previously in use.
        uint64 inputHash = reinterpret_cast<uint64>(levelBacking.gpuBuffer);
        if (state.inputHash != inputHash)
        {
            // TODO: Shouldn't second parameter be 2 if given mesh has UVs?
            state.command.setInputBuffer(0, 1, *(levelBacking.gpuBuffer));
    
            state.inputHash = inputHash;
        }
    }
    else
    {
        // Input buffers will need to be bound for every mesh separately due to 
        // different starting offsets. Single buffer cache still needs to be
        // updated to currently bound backing.
        state.inputHash = reinterpret_cast<uint64>(levelBacking.gpuBuffer);
    }
    
    // Draw each mesh that is part of this model Level Of Detail
    for(uint32 i=0; i<levelRange.count; ++i)
    {
        Mesh& currentMesh = mesh[levelRange.first + i];

        // Will bind only buffers selected for draw, and available in the mesh
        bufferMask &= currentMesh.bufferMask;
    
        // This can happen, but means something is off.. 
        if (bufferMask == 0)
        {
            continue;
        }

        // TODO: Here set pipeline, material, etc. ?


        // If draw uses only one input buffer of a mesh, firstVertex can be used to pass 
        // sub-allocation offset at draw call without a need to rebinding Buffer with
        // different starting offset.
        uint32 firstVertex = 0;

        // Single input buffer is used (Position and optional UV)
        if (bufferMask == 1)
        {
            // Calculate offset to first Vertex, for draws using only first input buffer
            // Shift by 4 is equal to division by 16. First buffer always has elementSize of 16bytes.

//TODO: This is not true. First buffer may or may not have UV attribute. So size is either 12 or 16 bytes.

            firstVertex = (levelBacking.gpuOffset + currentMesh.offset[0]) >> 4;
        }
        else
        {
            // All input buffers are suballocated from the same backing Buffer, thus they
            // have different starting offsets in it (those offsets change each time this
            // mesh becomes resident in GPU dedicated memory).
            uint64 gpuOffset[MaxMeshInputBuffers]; 
          
            // Update offsets of input buffers, based on current GPU residency. 
            uint32 first = 0;
            uint32 slots = 0;
            for(uint32 j=0; j<MaxMeshInputBuffers; ++j)
            {
                // If given buffer is unused, bind previous buffers range with their offsets.
                // Below algorithm minimizes amount of API calls, by grouping used buffers
                // together, and making the call only when really necessary.
                if (!checkBit(bufferMask, j))
                {
                    // This call cannot be cached, as we pass different set of offsets for each mesh.
                    if (slots > 0)
                    {
                        state.command.setInputBuffer(first, slots,
                                                     *(levelBacking.gpuBuffer),
                                                     &gpuOffset[first]);
                    }
    
                    first = j + 1;
                    slots = 0;

                    continue;
                }

                gpuOffset[j] = levelBacking.gpuOffset + currentMesh.offset[j];
                slots++;
            }
    
            // This call cannot be cached, as we pass different set of offsets for each mesh.
            if (slots > 0)
            {
                state.command.setInputBuffer(first, slots,
                                             *(levelBacking.gpuBuffer),
                                             &gpuOffset[first]);
            }
        }
    
        if (currentMesh.indexCount)
        {
            // Bind Buffer, backing model level shared sub-allocation, as one big 
            // Index buffer. This mesh 'firstIndex' contains offset to beginning of 
            // mesh Index Buffer. Do this only when index buffer is not bound yet,
            // other one was bound for previous encoded draw call, or if indexShift 
            // differs between previously drawn mesh, and currently drawn one.
            uint64 indexHash = reinterpret_cast<uint64>(levelBacking.gpuBuffer);
            if (state.indexHash != indexHash ||
                state.indexShift != currentMesh.indexShift)
            {
                state.command.setIndexBuffer(*(levelBacking.gpuBuffer),
                                             currentMesh.indexShift == 1 ? Attribute::u16 : Attribute::u32);
    
                state.indexHash  = indexHash;
                state.indexShift = currentMesh.indexShift;
            }
    
            // Calculate offset to optional sub-allocated IndexBuffer from total offset in backing dedicated buffer.
            uint32 firstIndex = (levelBacking.gpuOffset + currentMesh.indexOffset) >> currentMesh.indexShift;
    
            state.command.drawIndexed(currentMesh.indexCount, // Count of indexes to use for primitve assembly
                                      instanceCount,          // Instance count
                                      firstIndex,             // First index in Index Buffer (index offset)
                                      firstVertex,            // First vertex to use (added to vertex index read, vertex offset)
                                      firstInstance);         // First instance to use (instance offset)
        }
        else
        {
            state.command.draw(currentMesh.vertexCount, // Vertices to draw
                               firstVertex,             // First vertex to use (vertex offset in buffer)
                               instanceCount,           // Instance count
                               firstInstance);          // First instance to use (instance offset)
        }
    }
}









//Model::Model()
//{
//}

//Model::Model(std::shared_ptr<gpu::Buffer> buffer, gpu::DrawableType type) :
//   name("custom")
//{
//Mesh temp;
//temp.geometry.buffer  = buffer;
//temp.elements.type    = type;

//mesh.push_back(temp);
//}

//Model::~Model()
//{
//mesh.clear();
//}


ModelDescriptor::ModelDescriptor() /*:*/
   //mesh(NULL),
   //meshes(0)
{
}

ModelDescriptor::~ModelDescriptor()
{
//if (mesh)
//   {
//   for(uint8 i=0; i<meshes; ++i)
//      delete mesh[i];
//   delete mesh;
//   mesh = NULL;
//   }
}

//Font::Font() :
//    ProxyInterface<class FontDescriptor>(NULL)
//{
//}
//       
//Font::Font(class FontDescriptor* src) :
//    ProxyInterface<class FontDescriptor>(src)
//{
//}

//FontDescriptor::FontDescriptor() :
//   height(0),
//   //material(NULL)
//   texture(nullptr),
//   program(nullptr)
//{
//memset(&table[0], 0, 256*sizeof(FontDescriptor::Character));
//}
//
//FontDescriptor::~FontDescriptor()
//{
//}

//bool FontDestroy(FontDescriptor* const font)
//{
//// Fill font with null pointer
//ResourcesContext.storage.fonts.free(font);
//return true; 
//}




bool ModelDestroy(ModelDescriptor* const model)
{
   // Fill model with null pointer
//   ResourcesContext.storage.models.free(model);
   return true; 
}

//Model::Model(uint32 meshes) :
//   mesh(meshes)
//{
//}



//Model::Model() :
//    ProxyInterface<class ModelDescriptor>(NULL)
//{
//}
//       
//Model::Model(class ModelDescriptor* src) :
//    ProxyInterface<class ModelDescriptor>(src)
//{
//}

//uint8 Model::meshes(void)
//{
// assert(pointer);

// return pointer->meshes;
//}

/*  
gpu::DrawableType Model::drawableType(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);

    return pointer->mesh[mesh]->type;
}

Material& Model::material(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);
    
    return pointer->mesh[mesh]->material;    
}

gpu::Buffer Model::geometry(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);

    return pointer->mesh[mesh]->geometry.buffer;
}

gpu::Buffer Model::elements(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);

    return pointer->mesh[mesh]->elements.buffer;
}
//*/



std::shared_ptr<Model> Interface::Load::model(const std::string& filename, const std::string& name)
{
    sint64 found;
    uint64 length = filename.length();

    // Try to reuse already loaded models
    if (ResourcesContext.models.find(name) != ResourcesContext.models.end())
    {
        return ResourcesContext.models[name];
    }

    found = filename.rfind(".obj");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return nullptr; // TODO: obj::load(filename, name, streamer);
    }

    found = filename.rfind(".OBJ");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return nullptr; // TODO: obj::load(filename, name, streamer);
    }

    found = filename.rfind(".fbx");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return fbx::load(filename, name);
    }

    found = filename.rfind(".FBX");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return fbx::load(filename, name);
    }

    return std::shared_ptr<Model>(NULL);
}


void Interface::Free::model(const std::string& name)
{
    // Find specified model and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::shared_ptr<en::resources::Model> >::iterator it = ResourcesContext.models.find(name);
    if (it != ResourcesContext.models.end())
    {
        if (it->second.unique())
        {
            it->second = nullptr;
        }
    }
}

//void Interface::Free::material(const std::string& name)
//{
//// Find specified material and check if it is used
//// by other part of code. If it isn't it can be
//// safely deleted (assigment operator will perform
//// automatic resource deletion).
//map<string, Material>::iterator it = ResourcesContext.materials.find(name);
//if (it != ResourcesContext.materials.end())
//   if (it->second.references() == 1)
//      it->second = NULL;
//}

std::shared_ptr<audio::Sample> Interface::Load::sound(const std::string& filename)
{
    sint64 found;
    uint64 length = filename.length();

    found = filename.rfind(".wav");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return wav::load(filename);
    }

    found = filename.rfind(".WAV");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return wav::load(filename);
    }

    return std::shared_ptr<audio::Sample>(NULL);
}

enum FileExtension
{
    ExtensionUnknown = 0,
    ExtensionBMP        ,
    ExtensionDDS        ,
    ExtensionEXR        ,
    ExtensionHDR        ,
    ExtensionPNG        ,
    ExtensionTGA        ,
    ExtensionsCount
 };

static const std::pair<std::string, FileExtension> TranslateFileExtension[] =
{
    { std::string(".bmp"), ExtensionBMP },
    { std::string(".BMP"), ExtensionBMP },
    { std::string(".dds"), ExtensionDDS },
    { std::string(".DDS"), ExtensionDDS },
    { std::string(".exr"), ExtensionEXR },
    { std::string(".EXR"), ExtensionEXR },
    { std::string(".hdr"), ExtensionHDR },
    { std::string(".HDR"), ExtensionHDR },
    { std::string(".png"), ExtensionPNG },
    { std::string(".PNG"), ExtensionPNG },
    { std::string(".tga"), ExtensionTGA },
    { std::string(".TGA"), ExtensionTGA }
};

std::shared_ptr<en::gpu::Texture> Interface::Load::texture(const std::string& filename, const gpu::ColorSpace colorSpace)
{
    uint64 length = filename.length();

#ifdef EN_PROFILE
    Timer timer;
    timer.start();
#endif

// TODO: In future PNG textures should be reversed offline for Release build for performance reasons.
//
//#ifdef EN_DEBUG
    bool invertHorizontal = true;
//#else
//    bool invertHorizontal = false;
//#endif

    std::shared_ptr<gpu::Texture> texture = nullptr;

    // Iterate over all registered file extensions and execute proper loading function
    FileExtension type = ExtensionUnknown;
    uint32 entries = sizeof(TranslateFileExtension) / sizeof(std::pair<std::string, FileExtension>);
    for(uint32 i=0; i<entries; ++i)
    {
        uint64 extensionLength = TranslateFileExtension[i].first.length();
        if (filename.compare(length - extensionLength, extensionLength, TranslateFileExtension[i].first) == 0)
        {
            type = TranslateFileExtension[i].second;
            break;
        }
    }

    // Return if file type is unsupported
    if (type == ExtensionUnknown)
    {
        return texture;
    }

    // Load selected file type
    switch(type)
    {
        case ExtensionBMP:
        {
            //texture = bmp::load(filename);
            break;
        }

        case ExtensionDDS:
        {
            texture = dds::load(filename);
            break;
        }

        case ExtensionEXR:
        {
            texture = exr::load(filename);
            break;
        }
      
        case ExtensionHDR:
        {
            texture = hdr::load(filename);
            break;
        }

        case ExtensionPNG:
        {
            // TODO: This whole abstraction needs to be redone to streamer.
            // texture = png::load(filename, colorSpace, invertHorizontal);
            break;
        }

        case ExtensionTGA:
        {
            // TODO: All that need to be rewritten
            //texture = tga::load(filename);
            break;
        }

        default:
        {
            // How we get here?
            assert( 0 );
            break;
        }
    };

#ifdef EN_PROFILE
    enLog << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - " << filename << std::endl;
#endif

    return texture;
}

bool Interface::Load::texture(
    std::shared_ptr<gpu::Texture> dst, 
    const uint16 layer, 
    const std::string& filename, 
    const gpu::ColorSpace colorSpace)
{
    uint64 length = filename.length();

#ifdef EN_PROFILE
    Timer timer;
    timer.start();
#endif

// TODO: In future PNG textures should be reversed offline for Release build for performance reasons.
//
//#ifdef EN_DEBUG
    bool invertHorizontal = true;
//#else
//    bool invertHorizontal = false;
//#endif

    // Iterate over all registered file extensions and execute proper loading function
    FileExtension type = ExtensionUnknown;
    uint32 entries = sizeof(TranslateFileExtension) / sizeof(std::pair<std::string, FileExtension>);
    for(uint32 i=0; i<entries; ++i)
    {
        uint64 extensionLength = TranslateFileExtension[i].first.length();
        if (filename.compare(length - extensionLength, extensionLength, TranslateFileExtension[i].first) == 0)
        {
            type = TranslateFileExtension[i].second;
            break;
        }
    }

    // Return if file type is unsupported
    if (type == ExtensionUnknown)
    {
        return false;
    }

    // Load selected file type
    bool result = false;
    switch(type)
    {
        case ExtensionBMP:
        {
            //result = bmp::load(dst, layer, filename);
            break;
        }

        case ExtensionPNG:
        {
            // TODO: This whole abstraction needs to be redone to streamer.
            //result = png::load(dst, layer, filename, colorSpace, invertHorizontal);
            break;
        }

        case ExtensionTGA:
        {
            // TODO: This whole abstraction needs to be redone to streamer.
            //result = tga::load(dst, layer, filename);
            break;
        }

        default:
        {
            break;
        }
    };

// found = filename.rfind(".bmp");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return bmp::load();
// 
// found = filename.rfind(".BMP");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return bmp::load(dst, layer, filename);
// 
// found = filename.rfind(".png");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return png::load(dst, layer, filename, colorSpace, invertHorizontal);
// 
// found = filename.rfind(".PNG");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return png::load(dst, layer, filename, colorSpace, invertHorizontal);
// 
// found = filename.rfind(".tga");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return tga::load(dst, layer, filename);
// 
// found = filename.rfind(".TGA");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return tga::load(dst, layer, filename);


#ifdef EN_PROFILE
    enLog << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - " << filename << std::endl;
#endif

    return result;
}

//   bool Interface::Load::texture(std::shared_ptr<gpu::Texture> dst, const gpu::TextureFace face, const uint16 layer, const std::string& filename, const gpu::ColorSpace colorSpace)
//   {
//  uint32 found;
//   uint32 length = filename.length();
//
//// TODO: In future PNG textures should be reversed offline for Release build for performance reasons.
////
////#ifdef EN_DEBUG
//   bool invertHorizontal = true;
////#else
////   bool invertHorizontal = false;
////#endif
//
//   found = filename.rfind(".png");
//   if ( found != std::string::npos &&
//        found == (length - 4) )
//      return png::load(dst, face, layer, filename, colorSpace, invertHorizontal);
//
//   found = filename.rfind(".PNG");
//   if ( found != std::string::npos &&
//        found == (length - 4) )
//      return png::load(dst, face, layer, filename, colorSpace, invertHorizontal);
//
//   return false;
//   }

void Interface::Free::sound(const std::string& filename)
{
    // Find specified sample and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::shared_ptr<audio::Sample> >::iterator it = ResourcesContext.sounds.find(filename);
    if (it != ResourcesContext.sounds.end())
    {
        if (it->second.unique())
        {
            it->second = nullptr;
        }
    }
}

void Interface::Free::texture(const std::string& filename)
{
    // Find specified texture and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::unique_ptr<gpu::Texture> >::iterator it = ResourcesContext.textures.find(filename);

    // TODO: This whole abstraction needs to be redone to streamer.
    //if (it != ResourcesContext.textures.end())
    //   if (it->second.unique())
    //      it->second = NULL;
}

} // en::resource

} // en

//template<> bool (*ProxyInterface<en::resources::FontDescriptor>::destroy)(en::resources::FontDescriptor* const)         = en::resources::FontDestroy;
//template<> bool (*ProxyInterface<en::resources::MaterialDescriptor>::destroy)(en::resources::MaterialDescriptor* const) = en::resources::MaterialDestroy;
template<> bool (*ProxyInterface<en::resources::ModelDescriptor>::destroy)(en::resources::ModelDescriptor* const)       = en::resources::ModelDestroy;
