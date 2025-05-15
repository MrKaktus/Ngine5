/*

 Ngine v5.0

 Module      : Interface
 Requirements: none
 Description : Resources manager abstract interface.

*/

#ifndef ENG_CORE_RESOURCES_INTERFACE
#define ENG_CORE_RESOURCES_INTERFACE

#include "core/algorithm/hash.h"
#include "rendering/streamer.h"

namespace en
{
namespace resources
{

// Engine supports up to 8 GPU's
#define MaxSupportedGPUCount 8

enum class ShadingModel : uint8
{
    Unknown = 0,
    Lambert    ,
    Phong      ,
    Blinn      ,
    PBR        ,
    Count      ,
};

// *) Not part of the shading model, but optional addition by the engine
// *) Opacity (0=fully transparent/translucent 1=fully opaque)
// *) SpecularExponent (also called Glosiness/Shininess) map represents "Shininess" term in both Phong and Blinn shading models
// *) Reflectivity (Specular Strength) is exension to Phong and Blinn shading models (in [0.0, 1.0] or RGB color).
//    Reflectivity maps are not supported by the engine (as they are part of legacy shading models).
//
enum class MaterialComponent : uint8
{                         // Channels &     Materials:
                          // Quantization   PBR  Blinn  Phong  Lambert
    Albedo           = 0, // RGB (sRGB)     x    x      x      x
    Metallic            , // R   (sRGB)     x           
    Cavity              , // RGB (sRGB)     x    x      x
    Roughness           , // R   (Linear)   x           
    AmbientOcclusion    , // R   (sRGB)     x           
    Normal              , // RG  (Linear)   x    x      x
    Height              , // R   (Linear)   x           
    Displacement        , // RGB (Linear)   x           
    Emissive            , // RGB (sRGB)     x    *      *      *    
    Opacity             , // A   (Linear)   x    *      *      *
    Reflection          , // RGB (xRGB)

    // Legacy:
    Ambient             , // RGB (sRGB)          x      x      x
    Bump                , // R
    SpecularExponent    , // R                          x
    Decal               , // A   (Linear)
    Count               ,

    Diffuse             = Albedo,
    Specular            = Cavity,
    Transparency        = Opacity, // Inverted?
};

// TODO: Material optimized for renderer
// TODO: We should have already specified very specific packing of various components on various channels of backing textures!
//
// Textures backing material components are stored by resource manager.
// Multiple materials may reference the same textures. They are owned
// by resource manager itself.
//
struct Material
{
    hash  components[underlyingType(MaterialComponent::Count)];        // Surfaces storing material components (may be less than components count)
    uint8 componentId[underlyingType(MaterialComponent::Count)];       // Specifies surface storing given material component (components can be packed)
    uint8 componentChannels[underlyingType(MaterialComponent::Count)]; // Specifies surface channels storing given material component
};


//// Release Material
//class Material : public IMaterial
//   {
//   std::shared_ptr<en::gpu::Texture> layer0; // [sRGB] Albedo , optional [Linear] Alpha/Opacity
//
//   std::shared_ptr<en::gpu::Texture> metallic;     // 
//   std::shared_ptr<en::gpu::Texture> cavity;       // 
//   std::shared_ptr<en::gpu::Texture> roughness;    // 
//   std::shared_ptr<en::gpu::Texture> AO;           // 
//   std::shared_ptr<en::gpu::Texture> normal;       // 
//   std::shared_ptr<en::gpu::Texture> displacement; // 
//   std::shared_ptr<en::gpu::Texture> vector;       // Vector displacement map
//   std::shared_ptr<en::gpu::Texture> emmisive;     // Emited irradiance coefficients for each point of surface
//
//   };


// PBR Material
//
// - RGB - Albedo / BaseColor (sRGB)  [Diffuse for legacy models]
// - R   - Metallic (sRGB)
// - RGB - Cavity (sRGB)              [Specular for legacy models]
// - R   - Roughness (Linear)
// - R   - AO (sRGB)
// - RG  - Normal (Linear)
// - R   - Displacement (Linear)
// - RGB - Emmisive (sRGB)
// - R   - Opacity (Linear) [ 0=fully transparent/translucent 1=fully opaque]
//
/*
struct Material
{
    std::string name;                       // Material name

    // TODO: Those are shared_ptrs bc they might get assigned default textures when given map is missing. So defaults can be shared by multiple Materials. 
    //       But we want material to expose its maps as unique to whoever uses it (to prevent user from grabbing shared ownership of material resources). How to solve it?
    //       Use streamer allocations instead?
    //
    TextureAllocation albedo;       // Leaving irradiance coefficients for each point of surface 
    TextureAllocation metallic;     // 
    TextureAllocation cavity;       // 
    TextureAllocation roughness;    // 
    TextureAllocation AO;           // 
    TextureAllocation normal;       // 
    TextureAllocation displacement; // Either Height map or Vector Displacement map
    TextureAllocation emmisive;     // Emited irradiance coefficients for each point of surface
    TextureAllocation opacity;      //

    Material();                        // Inits material with default resources provided by engine
};
//*/

// Materials will have renderer specific representation.
// (making them efficient for given renderer use, for e.g. being collected into Texture Array, or Array of Texture Views in some Descriptor Set).
// Thus we should distinguish between "unpacked" / "parsed" Material, and "GPU" optimized Material (used by renderer).
// First one might be treated as Editor material perhaps?
//
// In Ngine 5 we go all in with bindless model, thus we do:
// - Array of Texture Views (or Array of Structs of Texture Views) in some Descriptor Set
// - Structure of Texture Views is one Material instance
// - We have Array of those Structs (materials) indexed by MaterialID.
// - Each drawn Instance on a Scene, points to used Model (collection of meshes), LOD, and Material.
// - Each drawn instance also has some Constants 

// Materials buffer:
// - Array of Material structures:
//   - albedo, metalness, ... <- Texture Views

// Meshes buffer:
// - Vertex0..3, Index <- Buffer pointers

// Scene buffer:
// - Array of Instance structures:
//  - ModelId -> Index of Model in Models buffer
//  - MaterialId -> Index of Material in Materials buffer













// Mesh can have any number of input attributes up to MaxInputLayoutAttributesCount,
// but they will always be stored in no more than 4 separate input buffers, to 
// optimize cache usage. Those input buffers always share backing sub-allocation
// as attributes cannot be partially resident. Idea here is not to keep resident,
// only needed sub-set of input buffers, but to bind and load to GPU cache only
// those that are needed for current pass (for e.g. only position one for shadow
// maps generation).
#define MaxMeshInputBuffers 4

// Mesh represents unique geometry, using single material. Mesh is not carrying 
// per-instance data, or other properties that can be bound from separate buffers 
// in InputLayout. It can be composed of up to 4 input buffers. 
//
// First three buffers always store data for first 6 Attrubites of predefined type:
//
// Buffer 0, Attribute 0 - (v3f32) Position 
// Buffer 0, Attribute 1 - (v2f16) UV 
// Buffer 1, Attribute 2 - (v2f16) Normal (Oct32P compressed)
// Buffer 1, Attribute 3 - (v2f16) Tangent (Oct32P compressed)
// Buffer 2, Attribute 4 - (v4u8)  BoneIndex
// Buffer 2, Attribute 5 - (v4u8)  BoneWeight
//
// Fourth buffer stores all other mesh specific attributes together (like decal UV):
// Buffer 3, Attribute 6+ - All remaining attributes composed together.
//
// Then mesh can be drawn using any subset of existing buffers, by passing bitmask
// of ones to use. Index buffer, if present is automatically taken into notice.
//
// Index Offset and Index Shift optimization:
//
// Optional index buffer may be sub-allocated in the same backing buffer that
// backs input buffers. In such case "indexOffset" specifies offset to start 
// of index buffer data. When backing buffer is bound as index buffer, it is
// treated as such in full. This means that to properly process only index 
// buffer data sub-allocated in it, first element in index buffer needs to be 
// pointed at by using "firstIndex" (or "firstElement") property of indexed 
// draw call.
// "indexShift" declares by how many bits, "indexOffset" should be r-shifted
// to convert it into index buffer elements count (depending on the type of
// element used in index buffer - u16 or u32), and thus, to specify "firstIndex".
//
// Currently mesh is not storing it's detailed description:
//
// Max element size is 512bytes (32 Attributes, max 4 channels each, max 32bits 
// per channel). If 64bit attributes would be supported that would be 1024bytes. 
// This means that element size can be stored on 10 bits (U10+1). Assuming each 
// element size would be padded to power of two, that could be reduced to r-shift 
// mask size of 4 bits. Element size could be calculated on the flight if whole
// Input Layout would be stored (which would also allow to query per-attribute 
// and per input-buffer element sizes):
//
// There is currently 41 distinct Attribute types. Rounding it up to 63 means
// 6 bits per attribute are needed. This means whole Input Layout can be packed
// on 24bytes (32x6 -> 192 bits), or excluding first 6 predefined attributes, 
// 26x6 -> 156bits, 19.5 bytes.
//
struct Mesh
{
    uint32 vertexCount;            // It's the same for all input buffers.
    uint32 indexCount;             // Count of indices in optional Index buffer.

    union
    {
        struct
        {
            uint32 bufferMask         : 4;  // Bitmask of available buffers, that can be bound to Input Assembler (1..4)
            uint32 indexShift         : 2;  // Amount of bits by which final offset needs to be r-shifted to calculate 
                                            // "firstIndex" / "firstElement" from which indexed draw call should start 
                                            // interpreting backing buffer data as Index Buffer.
                                            // It is assuming offset is padded to element size. (uint16 - >> 1 = /2, uint32 - >> 2 = /4)
            uint32 hasUV              : 1;  // There may be only Position stored in first buffer.
            uint32 hasTangent         : 1;  // If Tangent is stored, BiTangent (aligned with normal map V direction) is reconstructed with Normal

            uint32 packedType         : 6;  // Combined DrawableType and ControlPointsCount - count of elements composing single primitive (1..32).
                                            // Values in range [0 to DrawableTypesCount-1] represent DrawableType
                                            // Values bigger than 4 represent Patches with controlPointsCount + 4.
            uint32 materialIndex      : 18; // Index of one of 262144 materials in global materials array.
        };

        // 'hasPosition' bit is overlapping with 1st bit of bufferMask (Position is mandatory)
        // 'hasNormal'   bit is overlapping with 2nd bit of bufferMask (Normal is mandatory)
        // 'hasSkin'     bit is overlapping with 3rd bit of bufferMask
        struct
        {
            uint32 hasPosition        : 1;  // If there is no position, UV needs to be stored in 4th buffer, as this indicates special case.
            uint32 hasNormal          : 1;  // May store only Normal (without Tangent & BiTangent)
            uint32 hasSkin            : 1;  // Up to 4 contributing bones on each vertex, and their weights
            uint32                    : 29; 
        };
    };

    uint32 indexOffset;                 // Offset to optional Index buffer in shared sub-allocation 
    // 16 bytes
    uint32 offset[MaxMeshInputBuffers]; // Offset of each separate input buffer, in shared sub-allocation.
                                        // Mesh is not keeping reference to backing allocation itself, it
                                        // is expected that parent Model class stores such reference.
                                        // Each offset is aligned to given input elementSize.
    // 32 bytes

    // TODO: Consider additional 24 bytes of cold data storing detailed Input Layout description.

    Mesh();
    Mesh& operator=(const Mesh& src); // Copy Assigment operator

    forceinline void init(const gpu::DrawableType primitiveType, 
                          const uint8 controlPointsCount = 0u);

    forceinline gpu::DrawableType primitiveType(void) const;

    // Control points count, if stores Patches, otherwise 0
    forceinline uint32 controlPointsCount(void) const;
};

static_assert(sizeof(Mesh) == 32, "en::renderer::Mesh size mismatch.");

forceinline void Mesh::init(const gpu::DrawableType primitiveType, const uint8 controlPointsCount) 
{
    assert(primitiveType != gpu::DrawableType::DrawableTypesCount);

    if (primitiveType < gpu::DrawableType::Patches)
    {
        packedType = primitiveType;
    }
    else
    {
        assert(controlPointsCount > 0 && controlPointsCount <= 32);

        // This is equivalent to DrawableType::PatchesN where N is in [1..32] range.
        packedType = 4u + controlPointsCount;
    }
}

forceinline gpu::DrawableType Mesh::primitiveType(void) const
{
    if (packedType < gpu::DrawableType::Patches)
    {
        return static_cast<gpu::DrawableType>(packedType);
    }

    return gpu::DrawableType::Patches;
}

forceinline uint32 Mesh::controlPointsCount(void) const
{
    if (packedType < gpu::DrawableType::Patches)
    {
        return 0u;
    }

    return packedType - 4u;
}

struct Model
{
    hash   name;            // Name

    uint64 meshCount  : 16; // Total count of meshes this model has (max 64K meshes for all LOD's).

    // TODO: "levelsOfDetailCount" , "lodCount" , "levelsCount" ?
    uint64 levelCount : 4;  // Count of LOD's this model has (max 16 unique LOD's)
                            // Stored as U4 + 1
                            // LOD count:
                            // Megascans             - 7 (Highpoly + 6)
                            // Unity                 - unlimited
                            // "Killzone Shadow Fal" - 7
    uint64            : 4;  //
    uint64 gpuMask    : 8;  // Mask identifying on which GPU's this mesh has local copy 
                            // Bits corresponds to GPU order assigned by renderer, this way
                            // bit0 always indicates renderer primary GPU.
    uint64            : 32; //

    // 16 bytes
    Mesh*     mesh;      // Pointer to array of Mesh descriptors (for all LOD's, from LOD0 to N)
    uint16v2* meshRange; // Pointer to optional array of Model LOD mesh ranges (first and count) 
                         // in Model global array of meshes (each LOD may have different mesh 
                         // count). Single LOD model is not using it.
                         // Maximum size: 16 x 4 = 64bytes
                         // TODO: Could optimize by storing only first mesh indexz per LOD and size for last LOD. (34 bytes total). This pointer plus both paddings are 32 bytes. There are also 4 bytes free in mask above.

    // 32bytes
    float4x4* matrix;    // Pointer to optional array of Mesh transformation matrices (global)
                         // Each mesh has its own transformation, so bone weights and indices are not needed at all.
    uint64    padding[2];

    // 56 bytes
    // Array of per-GPU pointers, to arrays of LOD backings (backing[gpu][lod]).
    // backing[0] pointer lays at 56 byte offset, so it fits at the end of first 
    // 64 bytes boundary, which means it will be read in, at first cache line read.
    BufferAllocation* backing[MaxSupportedGPUCount]; 
                                  
    // 120 bytes
    uint64    padding2;
    
    // 128 bytes
 
    Model(const std::string& name,
          const uint32 meshesCount, 
          const uint32 levelsOfDetail, 
          const uint8 gpuIndex);

    forceinline bool levelsODetailCount(const uint8 count);

    forceinline uint8 levelsODetailCount(void) const;

    void drawLOD(CommandState& state,
                 const uint32 levelOfDetail,  
                       uint32 bufferMask    = 0xF, // All available buffers by default
                 const uint32 firstInstance = 0,   // Single instance
                 const uint32 instanceCount = 1);
};

static_assert(sizeof(Model) == 128, "en::renderer::Model size mismatch.");

forceinline bool Model::levelsODetailCount(const uint8 count)
{
    if (count < 1 || count > 16) // unlikely
    {
        return false;
    }

    levelCount = count - 1u;

    return true;
}

forceinline uint8 Model::levelsODetailCount(void) const
{
    return static_cast<uint8>(levelCount + 1u);
}

class Interface
{
public:
    // Creates instance of this class and assigns it to "Resources".
    static bool create(void);

    // Interface

    virtual std::string& assetsPath(void) const = 0;
    virtual std::string& screenshotsPath(void) const = 0;

    // Textures

    virtual bool createTexture(const std::string& filename, hash& handle, const bool withMipMaps = true) = 0;
    // Loads texture from file, into specific mip-map and layer of
    // already created texture allocation (referenced through hash handle).
    virtual bool loadTexture(const std::string& filename, const hash handle, const uint8 mipMapLevel, const uint16 layer = 0u) = 0;
    virtual TextureAllocation* findTexture(const std::string& filename) = 0;
    virtual TextureAllocation* findTexture(const hash handle) = 0;
    virtual bool freeTexture(const hash handle) = 0;

    // Materials

    // Loads all materials from specified file and optimizes them for engine renderer.
    // Those materials can later be accessed through their name/hash.
    virtual bool loadMaterials(const std::string& filename) = 0;

    virtual Material* findMaterial(const std::string& name) = 0;
    virtual Material* findMaterial(const hash handle) = 0;

    virtual bool freeMaterial(const hash handle) = 0;


    virtual ~Interface() {};
};

} // en::resources

extern std::unique_ptr<resources::Interface> Resources;

} // en

#endif