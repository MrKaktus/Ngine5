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
using namespace std;

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
   //       string name;                       // Material name

   //       struct Emmisive
   //              {
   //              Ptr<en::gpu::Texture> map;       // Ke emmisive coefficients for each point of surface
   //              float3           color;     // Ke default emissive coefficient 
   //              } emmisive;
   //      
   //       struct Ambient
   //              {
   //              Ptr<en::gpu::Texture> map;       // Ka incoming irradiance coefficients for each point of surface in Phong/Lambert models
   //              float3           color;     // Ka default incoming irradiance coefficient in Phong/Lambert models
   //              } ambient;
 
   //       struct Diffuse   
   //              {
   //              Ptr<en::gpu::Texture> map;       // Kd leaving irradiance coefficients for each point of surface in Phong/Lambert models
   //              float3           color;     // Kd base color of material, it's albedo, or diffuse term in Phong/Lambert models
   //              } diffuse;      

   //       struct Specular 
   //              {
   //              Ptr<en::gpu::Texture> map;       // Ks reflection coefficients for each point of surface in Phong model
   //              float3           color;     // Ks default reflection coefficient in Phong model
   //              float            exponent;  // Specular exponent "shininess" factor for specular equation
   //              } specular;
   //      
   //       struct Transparency
   //              {
   //              Ptr<en::gpu::Texture> map;       // Transparency coefficients for each point of surface
   //              float3           color;     // Transparency default coefficient
   //              bool             on;        // Is transparency enabled ?
   //              } transparency;
   //      
   //       struct Normal
   //              {
   //              Ptr<en::gpu::Texture> map;       // Normal vector for each point of surface
   //              } normal;
   //      
   //       struct Displacement
   //              {
   //              Ptr<en::gpu::Texture> map;       // Displacement value for each point of surface
   //              Ptr<en::gpu::Texture> vectors;   // Vector Displacement map for each point of surface
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

   enum MaterialType
      {
      Material_PhysicallyBased = 0,
      Material_Phong              
      };

   enum DisplacementType
      {
      DisplacementHeight = 0,
      DisplacementVector    ,
      };

   // Material interface
   class IMaterial : public SafeObject<IMaterial>
      {
      string name;
      MaterialType type;
      };

   // Editor Material, each surface can be bound separately or can share texture with other 

   // Editor Material
   // Flexible description that allows any combination of surface layers as well as their packing in shared textures.
   class EMaterial : public IMaterial
      {
      Ptr<en::gpu::Texture> layer[9];

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
   //   Ptr<en::gpu::Texture> layer0; // [sRGB] Albedo , optional [Linear] Alpha/Opacity
   //
   //   Ptr<en::gpu::Texture> metallic;     // 
   //   Ptr<en::gpu::Texture> cavity;       // 
   //   Ptr<en::gpu::Texture> roughness;    // 
   //   Ptr<en::gpu::Texture> AO;           // 
   //   Ptr<en::gpu::Texture> normal;       // 
   //   Ptr<en::gpu::Texture> displacement; // 
   //   Ptr<en::gpu::Texture> vector;       // Vector displacement map
   //   Ptr<en::gpu::Texture> emmisive;     // Emited irradiance coefficients for each point of surface
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
      string name;                       // Material name

      Ptr<en::gpu::Texture> albedo;       // Leaving irradiance coefficients for each point of surface 
      Ptr<en::gpu::Texture> metallic;     // 
      Ptr<en::gpu::Texture> cavity;       // 
      Ptr<en::gpu::Texture> roughness;    // 
      Ptr<en::gpu::Texture> AO;           // 
      Ptr<en::gpu::Texture> normal;       // 
      Ptr<en::gpu::Texture> displacement; // 
      Ptr<en::gpu::Texture> vector;       // Vector displacement map
      Ptr<en::gpu::Texture> emmisive;     // Emited irradiance coefficients for each point of surface
      Ptr<en::gpu::Texture> opacity;      //
      
      Material();                        // Inits material with default resources provided by engine
      };

   struct BoundingBox
      {
      float2 axisX;
      float2 axisY;
      float2 axisZ;
      };

   struct Mesh
      {
      string      name;                 // Name
      float4x4    matrix;               // Default local transformation matrix in model
      Material    material;             // Material
      BoundingBox AABB;                 // Axis-Aligned Bounding-Box

      struct Geometry
             {
             Ptr<gpu::Buffer> buffer;   // VBO with vertices/control points
             uint32           begin;    // First vertice in VBO describing this mesh
             uint32           end;      // Last vertice in VBO describing this mesh (for ranged drawing)
             } geometry;
      
      struct Elements
             {
             Ptr<gpu::Buffer>  buffer;  // IBO with vertices/control points assembly order
             gpu::DrawableType type;    // Type of geometry primitives
             uint32            cps;     // Control Points count (for Patch primitives)
             uint32            offset;  // Offset to starting index in buffer
             uint32            indexes; // Count of indexes describing geometry primitives
             } elements;
      
      Mesh();
      Mesh& operator=(const Mesh& src); // Copy Assigment operator
     ~Mesh();      
      };

   class Model : public SafeObject<Model>
      {
      public:
      string       name; // Name
                         // Mesh tree stored from top to bottom in array
      vector<Mesh> mesh; // Meshes creating model, their hierarchy is described in animation
                         // There can be several meshes with the same name, they are subMeshes
                         // of one mesh that uses different materials.
      
      Model();
      Model(Ptr<gpu::Buffer> buffer, gpu::DrawableType type); // Create model from custom buffer
     ~Model();
      };


   class Font : public SafeObject<Font>
      {
      public:
      virtual Ptr<en::gpu::Texture> texture(void) const = 0;            // Returns texture used by font
      virtual Ptr<Mesh> text(const string text, const bool screenspace = false) const = 0;  // Creates mesh that contains geometry representing
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

   //// Model
   //class Model : public ProxyInterface<class ModelDescriptor>
   //      {
   //      public:
   //      //using default ProxyInterface<class ModelDescriptor>; <= C++11

   //      uint8 meshes(void);
   //      gpu::DrawableType drawableType(uint8 mesh);
   //      Material&   material(uint8 mesh);
   //      gpu::Buffer geometry(uint8 mesh);
   //      gpu::Buffer elements(uint8 mesh);

   //      Model();
   //      Model(class ModelDescriptor* src);
   //      };



   struct Interface
          {
          struct Load
                 {
                 Ptr<Font>  font(const string& filename); 
                 Ptr<Model> model(const string& filename, const string& name);  // For multi-model files and nameless model files like *.obj
                 Ptr<Model> model(const string& filename);                           
                 //Material material(const string& filename, const string& name); // For multi-material files like *.mtl
                 //Material material(const string& filename);                     // For single-material files like *.material
                 Ptr<en::audio::Sample> sound(const string& filename);
                 Ptr<en::gpu::Texture>  texture(const string& filename, 
                                          const gpu::ColorSpace colorSpace = gpu::ColorSpaceLinear);
                 bool             texture(Ptr<gpu::Texture> dst, 
                                          const uint16 layer, 
                                          const string& filename, 
                                          const gpu::ColorSpace colorSpace = gpu::ColorSpaceLinear); // Load image to given texture layer
                 } load;

          struct Free
                 {
                 void font(const string& name);
                 void model(const string& name);
                 //void material(const string& name);
                 void sound(const string& filename);
                 void texture(const string& filename);
                 } free;
          };
   } 

extern resource::Interface Resources;
}

#endif
