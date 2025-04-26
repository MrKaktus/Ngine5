/*

 Ngine v5.0
 
 Module      : Resources manager.
 Requirements: none
 Description : Loads, stores and manages all types
               of supported resources. Also protects
               from loading duplicates of already
               loaded resources.

*/

#ifndef ENG_RESOURCES
#define ENG_RESOURCES

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/Tproxy.h"
#include "core/utilities/array.h" 
#include "audio/audio.h"
#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"
#include "core/rendering/state.h"    // For ColorSpace
#include "resources/font.h"

#include <string>
#include <vector>

#include "core/algorithm/hash.h"
#include "core/rendering/commandBuffer.h" // For CommandState
#include "core/rendering/state.h"         // For Mesh (DrawableType)

#include "rendering/streamer.h"

namespace en
{
namespace resource
{

// Phong Material
// - RGB - Ambient
// - RGB - Diffuse
// - RGB - Specular
// - RGB - Emmisive
// - A   - Transparency
// - S   - Shininess
// - R   - Reflectivity
//
// Lambert Material
// - RGB - Ambient
// - RGB - Diffuse
// - RGB - Emmisive
// - A   - Transparency
//
// PBR Material
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

// "Specular-Glosiness"
// "Metal-Roughness"
//
// PBR Materials:
//
//                       Unreal Engine 4       Unity 5            VRay  
//                       
// Reflection:           GGX                   Blinn-Phong        Blinn-Phong
// Microsurface:         Gloss                 Gloss              Gloss
// Reflectivity:         Metalness             Specular           Specular
// Normal Y:             -                     +                  +
//
//                       Unreal Engine 4       Unity 5            VRay
//
// Metalness    Linear   x (Metallic?)         -                  -       - 
// Albedo       sRGB     x (Base Color)        -                  -       - FormatBC7_RGBA_sRGB
// Cavity       sRGB     x (Cavity)            -                  -       - FormatBC4_R (un) sRGB!
// Gloss        Linear   x (Roughness?)        -                  -       - FormatBC4_R (un)
// AO           sRGB     x                     x                  x       - FormatBC4_R (un) sRGB!
// Normal       Linear   x                     x                  x       - FormatBC5_RG (in tangent space) -> eventually revert to FormatBC1_RGA if not enough memory (2x smaller)
// Displacement Linear   x                     x                  x       - FormatBC4_R (un)
// GlossBlinn   Linear   -                     x                  x       - FormatBC4_R (un)
// Diffuse      sRGB     -                     x                  x
// Specular     sRGB     -                     x                  x
//
// Cavity replaces Specular term

// Roughness - 0=max gloss 1=max rough
// Gloss     - 0=max rough 1=max gloss



//struct Material
//       { 
//       std::string name;                       // Material name

//       struct Emmisive
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Ke emmisive coefficients for each point of surface
//              float3           color;     // Ke default emissive coefficient 
//              } emmisive;
//      
//       struct Ambient
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Ka incoming irradiance coefficients for each point of surface in Phong/Lambert models
//              float3           color;     // Ka default incoming irradiance coefficient in Phong/Lambert models
//              } ambient;

//       struct Diffuse   
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Kd leaving irradiance coefficients for each point of surface in Phong/Lambert models
//              float3           color;     // Kd base color of material, it's albedo, or diffuse term in Phong/Lambert models
//              } diffuse;      

//       struct Specular 
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Ks reflection coefficients for each point of surface in Phong model
//              float3           color;     // Ks default reflection coefficient in Phong model
//              float            exponent;  // Specular exponent "shininess" factor for specular equation
//              } specular;
//      
//       struct Transparency
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Transparency coefficients for each point of surface
//              float3           color;     // Transparency default coefficient
//              bool             on;        // Is transparency enabled ?
//              } transparency;
//      
//       struct Normal
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Normal vector for each point of surface
//              } normal;
//      
//       struct Displacement
//              {
//              std::shared_ptr<en::gpu::Texture> map;       // Displacement value for each point of surface
//              std::shared_ptr<en::gpu::Texture> vectors;   // Vector Displacement map for each point of surface
//              } displacement;

//       Material();                        // Inits material with default resources provided by engine
//       };



enum SurfaceChannel
{
    ChannelR             = 0,
    ChannelG                ,
    ChannelB                ,
    ChannelA                ,
    SurfaceChannelsCount
};

enum class MaterialType : uint8
{
    PhysicallyBased = 0,
    Phong              ,
    Lambert
};

// Lambert: (diffuse term)
// - Simulates matte, non-shiny surfaces like chalk, matte paint, or unpolished materials. 
// - Often used as a default shader for objects that don't require specular highlights. 
// 
// Phong: (specular term)
// - Takes into account surface curvature, amount of light, and camera angle to get accurate shading and highlights.
// - Results in tight highlights that are excellent for polished shiny surfaces, such as plastic, porcelain, and glazed ceramic.
// 
// Blinn-Phong: (specular term)
// - A modification to the Phong model, using a "halfway vector" to calculate specular highlights, resulting in a smoother, more realistic specular reflectance shading model. 
// - Good for simulating metallic surfaces with soft highlights, such as brass or aluminum. 
// 
// Specular-Glosiness:
// "MetalRoughness"
//
// Good comparison of the models:
// https://www.jordanstevenstechart.com/lighting-models


enum class DisplacementType : uint8
{
    Height = 0,
    Vector    ,
};

// Material interface
class IMaterial
{
    std::string name;
    MaterialType type;
};

// Editor Material, each surface can be bound separately or can share texture with other 

// Editor Material
// Flexible description that allows any combination of surface layers as well as their packing in shared textures.
class EMaterial : public IMaterial
{
    std::shared_ptr<en::gpu::Texture> layer[9];

    struct Albedo
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } albedo;  

    struct Opacity
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } opacity;  

    struct Metallic
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } metallic; 

    struct Roughness
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } roughness; 

    struct Cavity
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } cavity; 

    struct AO
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } ao; 

    struct Normal
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } normal; 

    struct Displacement
    {
        uint32           map;      // Texture layer and channel ID
        SurfaceChannel   channel;
        DisplacementType type;
    } displacement; 

    struct Emissive
    {
        uint32         map;      // Texture layer and channel ID
        SurfaceChannel channel;
    } emissive;  
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
struct Material
{ 
    std::string name;                       // Material name

    // TODO: Those are shared_ptrs bc they might get assigned default textures when given map is missing. So defaults can be shared by multiple Materials. 
    //       But we want material to expose its maps as unique to whoever uses it (to prevent user from grabbing shared ownership of material resources). How to solve it?
    //       Use streamer allocations instead?
    //
    std::shared_ptr<gpu::Texture> albedo;       // Leaving irradiance coefficients for each point of surface 
    std::shared_ptr<gpu::Texture> metallic;     // 
    std::shared_ptr<gpu::Texture> cavity;       // 
    std::shared_ptr<gpu::Texture> roughness;    // 
    std::shared_ptr<gpu::Texture> AO;           // 
    std::shared_ptr<gpu::Texture> normal;       // 
    std::shared_ptr<gpu::Texture> displacement; // 
    std::shared_ptr<gpu::Texture> vector;       // Vector displacement map
    std::shared_ptr<gpu::Texture> emmisive;     // Emited irradiance coefficients for each point of surface
    std::shared_ptr<gpu::Texture> opacity;      //
   
    Material();                        // Inits material with default resources provided by engine
};














// PAK, FBX, OBJ - those are all container types, storing multiple basic
//                 resources, connected through metadata description
//               - those resources have their human readable Id's and hash
//               - name needs to be global and unique, the same way hash
//               - they need to be converted to representation used by engine
//               - engine representation ties data with metadata per-component
//                 (renderer metadata, physics, animation, etc.)
//
// container - description of multiple assets
//
// Buffer - size
// Geometry - Formatting, elements, [Buffer, offset] <- this is buffer view in fact
// Index    - Formatting, elements, [Buffer, offset] <- this is buffer view in fact
// Mesh     - [Geometry, Index], [Material*]
// LOD      - tree[Mesh] (parent-child hierarchical structure), [Animation*]
// Model    - array[LOD]
//
// glTF Accessor is generalized form of engine Formatting sctructure
//
// Resource Manager:
// - is capable of translating container metadata into engine metadata description
// - extracts resources from containers, into memory location
// - copy in memmory has virtual memory mapping
//   - this alows no-copy transfer to per streamer staging resource
//
// Metal memory-mapped Staging buffer:
//
// - (id<MTLBuffer>)newBufferWithBytesNoCopy:(void *)pointer
//                                    length:(NSUInteger)length
//                                   options:(MTLResourceOptions)options
//                               deallocator:(void (^)(void *pointer, NSUInteger length))deallocator;
//
// Resource Streamer:
// - streams resources into cache buffers and cache textures on GPU
// - returns GPU resource handle and offset in it, to streamed resource
// - manages resource residency, streaming bandwith


// Both D3D12 and Vulkan provide mapping mechanism for resources -> by exposing sysmem pointer.
// Metal allows creating Buffer on top of user allocation (receives sysmem pointer).
// Thus in Metal several GPU's can trivially share sysmem allocation created and
// mananged by application. In D3D12 and Vulkan, each GPU will have different sysmem pointer after mapping. 
// Thus explicit copy to other GPU will be required.

// Hash whole object, so not only name, filename, but also type, mipmap, slice, tile, etc. ?



// Resource Streamer:
// - input: sysmem resource
// - output: vidmem resource, handle

// Buffer 
// BufferView - [Buffer, Offset, Size]

// Primary Resource Streamer (keeps primary resource copy in RAM).
// Secondary Resource Streamers -> ask Primary RS for resource copy.
// PRS marshals all the disk reads?







// Primary GPU is always selected by engine as most performant GPU on local machine.
// External GPU's can never be set as primary one, as they can be detached at any
// moment in time.

// uint8 gpuMask;   // Bitmask representing to which GPU resource streamer this Model is attached.

// Mesh is always tied to one GPU.
// CpuMesh 1...N GpuMesh (GpuStreamer)
// - This relation is not simple broadcast
// - Arbitrary logic saying, on which GPU's it should be tracked
// - Mesh::Mesh(Gpu* primaryGpu) <- mesh is always visible on some GPU (it's primary one)
//                                  Different meshes can have different primary GPU's
// - Mesh::makeVisibleOn(Gpu* gpu) <- creates copy of Mesh on other GPU

// Mesh can be resident on:
// - single GPU
// - multiple GPU (bitmask?)









// If several threads encode in parallel, they use separate CommandBuffers 
// (or separate secondary CommandBuffers), each with its own state.
struct CommandState
{
    gpu::CommandBuffer& command;    // Currently encoded Command Buffer
    uint64         inputHash;  // Hash of currently bound buffer to input slot 0 (cast ptr)
    uint64         indexHash;  // Hash of currently bound buffer to index slot (cast ptr)
    uint8          indexShift; // Determines type of Index Attribute used
    uint8          gpuIndex;   // Index of GPU on which those commands are encoded (index according to renderer order)

    CommandState(gpu::CommandBuffer& command);
};

















// CLEAN START:

// Mesh can have any number of input attributes up to MaxInputLayoutAttributesCount,
// but they will always be stored in no more than 4 separate input buffers, to 
// optimize cache usage. Those input buffers always share backing sub-allocation
// as attributes cannot be partially resident. Idea here is not to keep resident
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
// Mesh currently is not storing it's detailed description:
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

// CLEAN END:





//// Model variant for given Level of Detail.
//struct ModelLevel
//   {
//   uint16v2 meshRange;    // Range of meshes (first and count) in Model global array of meshes, that compose this LOD (each LOD may have different mesh count)
//   BufferAllocation* buffer; // Reference to structure managed by Streamer for primary GPU (or selected GPU)
//                             // All input buffers and index buffers of all meshes of given LOD
//                             // share the same backing allocation.
//
//   ModelLevel();
//   };


struct BoundingBox
{
    float2 axisX;
    float2 axisY;
    float2 axisZ;
}; // 24 bytes


// Engine supports up to 8 GPU's
#define MaxSupportedGPUCount 8


// Animation - set of keyframes
//           - each keyframe -> set of bones (matrices related to each other)
struct Skeleton
{
    uint32 bonesCount;   // Count of bones in skeleton
    float4x4* matrix;    // Pointer to array of bone transformation matrices (local)
};



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













//
//      // Material - Pipeline object, has baked input state.
//

//
//      // TODO: Move to physics mesh description
//      BoundingBox AABB;                 // Axis-Aligned Bounding-Box
//
//
//// TODO: Meshes hierarchy in ModelLevel
////
//// Meshes can be ordered in tree of transformation relations.
////
//// In such case, mesh tree is stored from top to bottom in array.
////
//// Mesh transformations can be stored in 2 ways:
//// - Combined transformation from parent tree? (global mesh transform) -> for static meshes?
//// - Or transformation to parent space? (local mesh transform) -> For animated meshes?
//// Animated Model, may have "skeleton" that is not matching meshes at all (bones are shared with weights per vertex)
//// Meshes represent atomic units of geometry, having one material, but may be influenced by several transforms.
//
//      float4x4    matrix;               // Default local transformation matrix in model (not instance)
//
//
//                         // Mesh tree stored from top to bottom in array
//                         // Meshes creating model, their hierarchy is described in animation
//                         // There can be several meshes with the same name, they are subMeshes
//                         // of one mesh that uses different materials.
//
//
//
//
//
//



// TODO: Set Pipeline

   // Instancing is currently unsupported.
   // Instancing data is stored in separate sub-allocation (may point to the same Buffer).




class Font
{
    public:
    virtual std::shared_ptr<en::gpu::Texture> texture(void) const = 0;            // Returns texture used by font
    virtual std::shared_ptr<Mesh> text(const std::string text, const bool screenspace = false) const = 0;  // Creates mesh that contains geometry representing
                                                     // passed text. Upper left corner of first letter of 
                                                     // text lays in position (0,0). Text is aligned to X
                                                     // axis. If created in World Space (default), Y axis
                                                     // goes up, if created in Screen Space, Y axis goes 
                                                     // down (to simplify screen-space text rendering).
 
    virtual ~Font();                                // Polymorphic deletes require a virtual base destructor
};


//// Features of Material
//enum MaterialFeature
//     {  
//     AmbientMap                = 0,
//     DiffuseMap                   ,
//     SpecularMap                  ,
//     DisplacementMap              ,
//     NormalMap                    ,
//     AlphaMap                     ,
//     MaterialFeaturesCount
//     };

//// Material
//class Material : public ProxyInterface<struct MaterialDescriptor>
//      {
//      public:
//      //using default ProxyInterface<struct MaterialDescriptor>; <= C++11

//      gpu::Program& program(void); // Returns handle to materials program
//      uint32 parameters(void);     
//      uint32 samplers(void);
//      bool   uses(enum MaterialFeature feature);
//      void   update(void);         // Updates program parameters to default/material values

//      Material();
//      Material(struct MaterialDescriptor* src);
//      };



struct Interface
{
    struct Load
    {
        std::shared_ptr<Font>  font(const std::string& filename); 
        std::shared_ptr<Model> model(const std::string& filename, const std::string& name);  // For multi-model files and nameless model files like *.obj
        std::shared_ptr<Model> model(const std::string& filename);                           
      //Material material(const std::string& filename, const std::string& name); // For multi-material files like *.mtl
      //Material material(const std::string& filename);                     // For single-material files like *.material
        std::shared_ptr<en::audio::Sample> sound(const std::string& filename);
        std::shared_ptr<en::gpu::Texture>  texture(const std::string& filename, 
                                 const gpu::ColorSpace colorSpace = gpu::ColorSpaceLinear);
        bool texture(std::shared_ptr<gpu::Texture> dst, 
                     const uint16 layer, 
                     const std::string& filename, 
                     const gpu::ColorSpace colorSpace = gpu::ColorSpaceLinear); // Load image to given texture layer
    } load;

    struct Free
    {
        void font(const std::string& name);
        void model(const std::string& name);
      //void material(const std::string& name);
        void sound(const std::string& filename);
        void texture(const std::string& filename);
    } free;
};

} // en::resource

extern resource::Interface Resources;

} // en

#endif
