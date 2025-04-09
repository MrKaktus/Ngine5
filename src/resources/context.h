/*

 Ngine v5.0
 
 Module      : Resources manager context
 Visibility  : Engine internal code
 Requirements: none
 Description : Declaration of engine resources context
               internal data for interaction with
               other modules.

*/

#ifndef ENG_RESOURCES_CONTEXT
#define ENG_RESOURCES_CONTEXT

#include "core/configuration.h"
#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/device.h"
//#include "core/utilities/TarrayAdvanced.h"
#include "audio/audio.h"
#include "resources/resources.h"

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

#pragma push_macro("aligned")
#undef aligned

#define FBXSDK_SHARED
#define FBXSDK_NEW_API
#include "fbxsdk.h"
#endif

#pragma pop_macro("aligned")

#include <map>

namespace en
{
namespace resource
{

//struct MaterialParameter
//{
//    gpu::Parameter handle; // Handle to parameter
//          void*    value;  // Pointer to parameters default value
//    const char*    name;   // Pointer to parameters name
//
//    MaterialParameter();
//   ~MaterialParameter();
//};

//struct MaterialSampler
//{
//    gpu::Sampler handle;  // Handle to sampler
//    gpu::Texture texture; // Default texture that should be bound to sampler
//    const char*  name;    // Pointer to samplers name
//   
//    MaterialSampler();
//   ~MaterialSampler();
//};

struct cachealign MaterialDescriptor : public ProxyObject
{
//       gpu::Program program;                  // Program that should be used to render material
//       struct Parameters
//              {
//              vector<MaterialParameter> list; // List of parameters
//              void*  buffer;                  // Buffer containing default values of all parameters
//              uint32 size;                    // Buffer size
//              } parameters;                   // Default program parameter values
//       vector<MaterialSampler>   samplers;    // Default program sampler bindings

//       struct State
//              {
//              struct Textures
//                     {
//                     uint8 ambient      : 1; 
//                     uint8 diffuse      : 1; 
//                     uint8 specular     : 1; 
//                     uint8 normal       : 1; 
//                     uint8 displacement : 1; 
//                     uint8 alpha        : 1; 
//                     uint8              : 2;
//                     } texture;

//              bool transparent;     // Is material rendered as transparent?
//              } state;

    MaterialDescriptor();
   ~MaterialDescriptor();
};

class cachealign ModelDescriptor : public ProxyObject
{
//      public:
//      Mesh** mesh;   // Meshes table
//      uint32 meshes; // Meshes count
//
    ModelDescriptor();
   ~ModelDescriptor();
};

//class cachealign FontDescriptor : public ProxyObject
//      {
//      public:
//      struct Character
//             {
//             float2 pos;    // Top-left character coordinate
//             float  width;  // Width of character
//             } table[256];  // Table of characters
//      float  height;        // Characters height
//      std::shared_ptr<gpu::Texture> texture; // Texture used by font
//      gpu::Program program; // Program used by font
//      //Material material;   // Material used by font

//      FontDescriptor();
//     ~FontDescriptor();
//      };


// All values are normalized to [0..1] range of texture coordinates
struct Character
{
    float2 position; // OpenGL bottom-left
    float  width;    // Size on texture
    float  height;   //
    float2 offset;   // Character offset from it's bottom-left position
    float  advance;  // Offset to next character
};

class FontImp : public Font
{
    public:
    Character table[256]; // Chracters description
  //float     base;       // Text base in line (bottom alignment of characters)
    float     height;     // Total height of line (default distance between lines, if there is no spacing)

    std::shared_ptr<en::gpu::Texture> resource;

    virtual std::shared_ptr<en::gpu::Texture> texture(void) const;            // Returns texture used by font
    virtual std::shared_ptr<Mesh> text(const std::string text, const bool screenspace = false) const;  // Creates mesh that contains geometry representing
 
    FontImp();
   ~FontImp();
};

struct Context
{
    struct Storage
    {
        //TarrayAdvanced<FontDescriptor, EN_MAX_ARRAY_SIZE>     fonts;
        //TarrayAdvanced<ModelDescriptor, EN_MAX_ARRAY_SIZE>    models;
        //TarrayAdvanced<MaterialDescriptor, EN_MAX_ARRAY_SIZE> materials;
    } storage;

    struct Paths
    {
        std::string fonts;
        std::string models;
        std::string materials;
        std::string shaders;
        std::string textures;
        std::string sounds;
        std::string screenshots;
    } path;                   // Default paths to resources of specified type

    struct Defaults
    {
//      std::shared_ptr<gpu::Pipeline> pipeline;            // Default program for materials
//      std::shared_ptr<gpu::Shader>   vertex;
//      std::shared_ptr<gpu
        gpu::Heap*    enHeapBuffers;
        gpu::Heap*    enHeapTextures;
        gpu::Heap*    enStagingHeap;
        std::shared_ptr<gpu::Texture> enAlbedoMap;
        std::shared_ptr<gpu::Texture> enMetallicMap;
        std::shared_ptr<gpu::Texture> enCavityMap;
        std::shared_ptr<gpu::Texture> enRoughnessMap;
        std::shared_ptr<gpu::Texture> enAOMap;
        std::shared_ptr<gpu::Texture> enNormalMap;
        std::shared_ptr<gpu::Texture> enDisplacementMap;
        std::shared_ptr<gpu::Texture> enVectorDisplacementMap;
        std::shared_ptr<gpu::Texture> enEmmisiveMap;
        std::shared_ptr<gpu::Texture> enOpacityMap;
        // std::shared_ptr<gpu::Texture> enEmmisiveMap;
        // std::shared_ptr<gpu::Texture> enAmbientMap;
        // std::shared_ptr<gpu::Texture> enDiffuseMap;
        // std::shared_ptr<gpu::Texture> enSpecularMap;
        // std::shared_ptr<gpu::Texture> enTransparencyMap;
        // std::shared_ptr<gpu::Texture> enNormalMap;
        // std::shared_ptr<gpu::Texture> enDisplacementMap;
        // std::shared_ptr<gpu::Texture> enVectorsMap;
        std::unique_ptr<gpu::Buffer> enAxes;             // Default axes buffer

        Defaults();
       ~Defaults();
    } defaults;

    std::map<std::string, std::shared_ptr<FontImp> >       fonts;
    std::map<std::string, std::shared_ptr<Model> >         models;
    std::map<std::string, Material>            materials;
    std::map<std::string, std::unique_ptr<gpu::Texture> >  textures;
    std::map<std::string, std::shared_ptr<audio::Sample> > sounds;

#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)
    FbxManager*           fbxManager;           // FBX Memory Manager
    FbxIOSettings*        fbxFilter;            // Filters content read from FBX
    FbxGeometryConverter* fbxGeometryConverter; // Geometry Converter responsible for triangulation
#endif

    Context();
    ~Context();

    void create(void);
    void destroy(void);
};

} // en::resource

extern resource::Context ResourcesContext;

} // en

#endif
