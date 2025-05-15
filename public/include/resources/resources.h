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
namespace resources
{



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





// illum : specifies an illumination model, using a numeric value.See Notes below for more detail on the illum keyword.The value 0 represents the simplest illumination model, relying on the Kd for the material modified by a texture map specified in a map_Kd statement if present.The compilers of this resource believe that the choice of illumination model is irrelevant for 3D printing use and is ignored on import by some software applications.For example, the MTL Loader in the threejs Javascript library appears to ignore illum statements.Comments welcome.

















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




// Animation - set of keyframes
//           - each keyframe -> set of bones (matrices related to each other)
struct Skeleton
{
    uint32 bonesCount;   // Count of bones in skeleton
    float4x4* matrix;    // Pointer to array of bone transformation matrices (local)
};














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

/*
class Interface
{
public:
    static bool create(void);                      // Creates instance of this class (OS specific) and assigns it to "Resources".

    // Load

    // Loads all materials from specified file.
    // THose materials can later be accessed through they name/hash.
    virtual bool loadMaterials(const std::string& filename) = 0;

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


    // Find

    virtual Material* findMaterial(const std::string& name) = 0;
    virtual Material* findMaterial(const hash handle) = 0;

    // Free
    
    virtual bool freeMaterial(const hash handle) = 0;

        void font(const std::string& name);
        void model(const std::string& name);
        //void material(const std::string& name);
        void sound(const std::string& filename);
        void texture(const std::string& filename);


    virtual ~Interface() {};                       // Polymorphic deletes require a virtual base destructor
};
//*/


} // en::resource

//extern std::unique_ptr<resources::Interface> Resources = nullptr;

} // en

#endif
