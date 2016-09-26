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
#include "core/utilities/TarrayAdvanced.h"
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
using namespace std;


namespace en
{
   namespace resource
   {
   //struct MaterialParameter
   //       {
   //       gpu::Parameter handle; // Handle to parameter
   //             void*    value;  // Pointer to parameters default value
   //       const char*    name;   // Pointer to parameters name

   //       MaterialParameter();
   //      ~MaterialParameter();
   //       };

   //struct MaterialSampler
   //       {
   //       gpu::Sampler handle;  // Handle to sampler
   //       gpu::Texture texture; // Default texture that should be bound to sampler
   //       const char*  name;    // Pointer to samplers name

   //       MaterialSampler();
   //      ~MaterialSampler();
   //       };

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
   //      Ptr<gpu::Texture> texture; // Texture used by font
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

      Ptr<en::gpu::Texture> resource;

      virtual Ptr<en::gpu::Texture> texture(void) const;            // Returns texture used by font
      virtual Ptr<Mesh> text(const string text, const bool screenspace = false) const;  // Creates mesh that contains geometry representing
    
      FontImp();
     ~FontImp();
      };

   struct Context
          {
          struct Storage
                 {
                 //TarrayAdvanced<FontDescriptor, EN_MAX_ARRAY_SIZE>     fonts;
                 //TarrayAdvanced<ModelDescriptor, EN_MAX_ARRAY_SIZE>    models;
                 TarrayAdvanced<MaterialDescriptor, EN_MAX_ARRAY_SIZE> materials;
                 } storage;

          struct Paths
                 {
                 string fonts;
                 string models;
                 string materials;
                 string shaders;
                 string textures;
                 string sounds;
                 string screenshots;
                 } path;                   // Default paths to resources of specified type

          struct Defaults
                 {
//                 Ptr<gpu::Pipeline> pipeline;            // Default program for materials
//                 Ptr<gpu::Shader>   vertex;
//                 Ptr<gpu
                 Ptr<gpu::Heap>    enHeap;
                 Ptr<gpu::Texture> enAlbedoMap;
                 Ptr<gpu::Texture> enMetallicMap;
                 Ptr<gpu::Texture> enCavityMap;
                 Ptr<gpu::Texture> enRoughnessMap;
                 Ptr<gpu::Texture> enAOMap;
                 Ptr<gpu::Texture> enNormalMap;
                 Ptr<gpu::Texture> enDisplacementMap;
                 Ptr<gpu::Texture> enVectorDisplacementMap;
                 Ptr<gpu::Texture> enEmmisiveMap;
                 Ptr<gpu::Texture> enOpacityMap;
                 // Ptr<gpu::Texture> enEmmisiveMap;
                 // Ptr<gpu::Texture> enAmbientMap;
                 // Ptr<gpu::Texture> enDiffuseMap;
                 // Ptr<gpu::Texture> enSpecularMap;
                 // Ptr<gpu::Texture> enTransparencyMap;
                 // Ptr<gpu::Texture> enNormalMap;
                 // Ptr<gpu::Texture> enDisplacementMap;
                 // Ptr<gpu::Texture> enVectorsMap;
                 Ptr<gpu::Buffer>  enAxes;             // Default axes buffer

                 Defaults();
                ~Defaults();
                 } defaults;

          map<string, Ptr<FontImp> >       fonts;
          map<string, Ptr<Model> >         models;
          map<string, Material>            materials;
          map<string, Ptr<gpu::Texture> >  textures;
          map<string, Ptr<audio::Sample> > sounds;

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
   } 

extern resource::Context ResourcesContext;
}

#endif
