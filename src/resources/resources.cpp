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
#include "core/utilities/TintrusivePointer.h"
#include "core/rendering/device.h"
#include "resources/context.h" 
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
using namespace std;

namespace en
{
   namespace resource
   {
   // Coordinate system
   // LineStripes
   // (x,y,z)(r,g,b)
   float axes[] = { 0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f, // Y Arrow - Green
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
                    0.0f,   0.0f,   0.5f,   0.0f, 0.0f, 1.0f  };

   Context::Defaults::Defaults() :
      program(nullptr),
      enAlbedoMap(nullptr),
      enMetallicMap(nullptr),
      enCavityMap(nullptr),
      enRoughnessMap(nullptr),
      enAOMap(nullptr),
      enNormalMap(nullptr),
      enDisplacementMap(nullptr),
      enVectorDisplacementMap(nullptr),
      enEmmisiveMap(nullptr),
      enOpacityMap(nullptr),
      //enEmmisiveMap(nullptr),
      //enAmbientMap(nullptr),
      //enDiffuseMap(nullptr),
      //enSpecularMap(nullptr),
      //enTransparencyMap(nullptr),
      //enNormalMap(nullptr),
      //enDisplacementMap(nullptr),
      //enVectorsMap(nullptr),
      enAxes(nullptr)
   {
   }

   Context::Defaults::~Defaults()
   {
   }

#ifdef EN_PLATFORM_WINDOWS
   Context::Context() :
      fbxManager(nullptr),
      fbxFilter(nullptr),
      fbxGeometryConverter(nullptr)
#else
   Context::Context()
#endif
   {
   //storage.fonts.create(128);
//   storage.models.create(4096);
   storage.materials.create(4096);

   //fonts.clear();
   models.clear();
   materials.clear();
   textures.clear();

   path.fonts       = string("resources/fonts/");
   path.models      = string("resources/models/");
   path.materials   = string("resources/materials/");
   path.shaders     = string("resources/shaders/");
   path.textures    = string("resources/textures/");
   path.sounds      = string("resources/sounds/"); 
   path.screenshots = string("screenshots/");
   }

   Context::~Context()
   {
   }

   void Context::create(void)
   {
   using namespace en::gpu;

   Log << "Starting module: Resources.\n";

   models.clear();
   materials.clear();
   textures.clear();

   // Create default program for materials
   string vsCode, fsCode;
#ifdef EN_OPENGL_DESKTOP
   en::Storage.read(string("resources/engine/shaders/default.glsl.1.10.vs"), vsCode);
   en::Storage.read(string("resources/engine/shaders/default.glsl.1.10.fs"), fsCode);
#endif
#ifdef EN_OPENGL_MOBILE
   en::Storage.read(string("resources/engine/shaders/default.essl.1.00.vs"), vsCode);
   en::Storage.read(string("resources/engine/shaders/default.essl.1.00.fs"), fsCode);
#endif

   vector<gpu::Shader> shaders(2, Shader(nullptr));
   shaders[0] = Gpu.shader.create(Vertex, vsCode);
   shaders[1] = Gpu.shader.create(Fragment, fsCode);
   defaults.program     = Gpu.program.create(shaders);

   // Create default textures for materials
   defaults.enAlbedoMap             = png::load(string("./resources/engine/textures/enDefaultAlbedoMap.png"));   
   defaults.enMetallicMap           = png::load(string("./resources/engine/textures/enDefaultMetallicMap.png"));
   defaults.enCavityMap             = png::load(string("./resources/engine/textures/enDefaultCavityMap.png"));
   defaults.enRoughnessMap          = png::load(string("./resources/engine/textures/enDefaultRoughnessMap.png"));
   defaults.enAOMap                 = png::load(string("./resources/engine/textures/enDefaultAOMap.png"));
   defaults.enNormalMap             = png::load(string("./resources/engine/textures/enDefaultNormalMap.png"));
   defaults.enDisplacementMap       = png::load(string("./resources/engine/textures/enDefaultDisplacementMap.png"));
   defaults.enVectorDisplacementMap = png::load(string("./resources/engine/textures/enDefaultVectorDisplacementMap.png"));
   defaults.enEmmisiveMap           = png::load(string("./resources/engine/textures/enDefaultEmmisiveMap.png"));
   defaults.enOpacityMap            = png::load(string("./resources/engine/textures/enDefaultOpacityMap.png"));
   //defaults.enEmmisiveMap     = png::load(string("./resources/engine/textures/enDefaultEmmisiveMap.png"));
   //defaults.enAmbientMap      = png::load(string("./resources/engine/textures/enDefaultAmbientMap.png"));       
   //defaults.enDiffuseMap      = png::load(string("./resources/engine/textures/enDefaultDiffuseMap.png"));       
   //defaults.enSpecularMap     = png::load(string("./resources/engine/textures/enDefaultSpecularMap.png"));   
   //defaults.enTransparencyMap = png::load(string("./resources/engine/textures/enDefaultAlphaMap.png"));     
   //defaults.enNormalMap       = png::load(string("./resources/engine/textures/enDefaultNormalMap.png")); 
   //defaults.enDisplacementMap = png::load(string("./resources/engine/textures/enDefaultDisplacementMap.png"));
   //defaults.enVectorsMap      = png::load(string("./resources/engine/textures/enDefaultVectorMap.png")); 

   // Create default mesh for corrdinate system axes
   Formatting formatting(Attribute::v3f32, Attribute::v3f32); // inPosition, inColor
   defaults.enAxes = en::Graphics->primaryDevice()->create(14, formatting, 0, &axes);

#ifdef EN_PLATFORM_WINDOWS
   fbxManager = FbxManager::Create();
   fbxFilter  = FbxIOSettings::Create(fbxManager, IOSROOT);
   fbxManager->SetIOSettings(fbxFilter);
   fbxGeometryConverter = new FbxGeometryConverter(fbxManager);
#endif
   }

   void Context::destroy(void)
   {
   Log << "Closing module: Resources.\n";

   defaults.program                 = nullptr;
                                    
   defaults.enAlbedoMap             = nullptr;
   defaults.enMetallicMap           = nullptr;
   defaults.enCavityMap             = nullptr;
   defaults.enRoughnessMap          = nullptr;
   defaults.enAOMap                 = nullptr;
   defaults.enNormalMap             = nullptr;
   defaults.enDisplacementMap       = nullptr;
   defaults.enVectorDisplacementMap = nullptr;
   defaults.enEmmisiveMap           = nullptr;
   defaults.enOpacityMap            = nullptr;
   //defaults.enEmmisiveMap     = nullptr;
   //defaults.enAmbientMap      = nullptr;
   //defaults.enDiffuseMap      = nullptr;
   //defaults.enSpecularMap     = nullptr;
   //defaults.enTransparencyMap = nullptr;
   //defaults.enNormalMap       = nullptr;
   //defaults.enDisplacementMap = nullptr;
   //defaults.enVectorsMap      = nullptr;
   defaults.enAxes            = nullptr;

   models.clear();
   materials.clear();
   textures.clear();

#ifdef EN_PLATFORM_WINDOWS
   if (fbxManager)
      {
      fbxManager->Destroy();
      delete fbxGeometryConverter;
      }
#endif
   }

   //MaterialParameter::MaterialParameter() :
   //   handle(NULL),
   //   value(NULL),
   //   name(NULL)
   //{
   //}

   //MaterialParameter::~MaterialParameter()
   //{
   //// Don't delete "value" as it points to shared buffer
   //// Don't delete "name" as it points to shared string
   //}

   //MaterialSampler::MaterialSampler() :
   //   handle(NULL),
   //   texture(NULL),
   //   name(NULL)
   //{
   //}

   //MaterialSampler::~MaterialSampler()
   //{
   //// Don't delete "name" as it points to shared string
   //}

   MaterialDescriptor::MaterialDescriptor() /*:*/
 /*     program(NULL)*/
   {
   //parameters.list.clear();
   //parameters.buffer = NULL;
   //parameters.size   = 0;

   //samplers.clear();

   //state.texture.ambient      = false;
   //state.texture.diffuse      = false;
   //state.texture.specular     = false;
   //state.texture.displacement = false;
   //state.texture.normal       = false;
   //state.texture.alpha        = false;

   //state.transparent          = false;
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
   ResourcesContext.storage.materials.free(material);
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
   assert(Gpu.screen.created());
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
   };

   Mesh::Mesh() 
   {
   geometry.buffer      = nullptr;
   geometry.begin       = 0;
   geometry.end         = 0;
   elements.buffer      = nullptr;
   elements.type        = en::gpu::Triangles;
   elements.cps         = 3;
   elements.offset      = 0;
   elements.indexes     = 0;
   }

   Mesh::~Mesh()
   {
   }

   Model::Model()
   {
   }

   Model::Model(Ptr<gpu::Buffer> buffer, gpu::DrawableType type) :
      name("custom")
   {
   Mesh temp;
   temp.geometry.buffer  = buffer;
   temp.elements.type    = type;

   mesh.push_back(temp);
   }

   Model::~Model()
   {
   mesh.clear();
   }


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

   Mesh& Mesh::operator=(const Mesh& src)
   {
   this->name             = src.name;
   this->matrix           = src.matrix;
   this->material         = src.material;
   this->geometry.buffer  = src.geometry.buffer;
   this->geometry.begin   = src.geometry.begin;
   this->geometry.end     = src.geometry.end;
   this->elements.buffer  = src.elements.buffer;
   this->elements.offset  = src.elements.offset;
   this->elements.indexes = src.elements.indexes;
   this->elements.type    = src.elements.type;
   return *this;
   }

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

 /*  gpu::DrawableType Model::drawableType(uint8 mesh)
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
   }*/



   Ptr<Model> Interface::Load::model(const string& filename, const string& name)
   {
   sint64 found;
   uint64 length = filename.length();

   // Try to reuse already loaded models
   if (ResourcesContext.models.find(name) != ResourcesContext.models.end())
      return ResourcesContext.models[name];

   found = filename.rfind(".obj");
   if ( found != string::npos &&
        found == (length - 4) )
      return obj::load(filename, name);

   found = filename.rfind(".OBJ");
   if ( found != string::npos &&
        found == (length - 4) )
      return obj::load(filename, name);

   found = filename.rfind(".fbx");
   if ( found != string::npos &&
        found == (length - 4) )
      return fbx::load(filename, name);

   found = filename.rfind(".FBX");
   if ( found != string::npos &&
        found == (length - 4) )
      return fbx::load(filename, name);

   return Ptr<Model>(NULL);
   }

   //Material Interface::Load::material(const string& filename, const string& name)
   //{
   //uint32 found;
   //uint32 length = filename.length();

   //// Try to reuse already loaded material
   //if (ResourcesContext.materials.find(name) != ResourcesContext.materials.end())
   //   return ResourcesContext.materials[name];

   //found = filename.rfind(".mtl");
   //if ( found != string::npos &&
   //     found == (length - 4) )
   //   return mtl::load(filename, name);

   //found = filename.rfind(".MTL");
   //if ( found != string::npos &&
   //     found == (length - 4) )
   //   return mtl::load(filename, name);

   //return Material();
   //}

   //Material Interface::Load::material(const string& filename)
   //{
   //uint32 found;
   //uint32 length = filename.length();

   //// Try to reuse already loaded material
   //if (ResourcesContext.materials.find(filename) != ResourcesContext.materials.end())
   //   return ResourcesContext.materials[filename];

   //found = filename.rfind(".material");
   //if ( found != string::npos &&
   //     found == (length - 9) )
   //   return material::load(filename);

   //found = filename.rfind(".MATERIAL");
   //if ( found != string::npos &&
   //     found == (length - 9) )
   //   return material::load(filename);

   //return Material();
   //}

   void Interface::Free::model(const string& name)
   {
   // Find specified model and check if it is used
   // by other part of code. If it isn't it can be
   // safely deleted (assigment operator will perform
   // automatic resource deletion).
   map<string, Ptr<en::resource::Model> >::iterator it = ResourcesContext.models.find(name);
   if (it != ResourcesContext.models.end())
      if (it->second->references == 1)
         it->second = NULL;
   }

   //void Interface::Free::material(const string& name)
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

   Ptr<audio::Sample> Interface::Load::sound(const string& filename)
   {
   sint64 found;
   uint64 length = filename.length();

   found = filename.rfind(".wav");
   if ( found != string::npos &&
        found == (length - 4) )
      return wav::load(filename);

   found = filename.rfind(".WAV");
   if ( found != string::npos &&
        found == (length - 4) )
      return wav::load(filename);

   return Ptr<audio::Sample>(NULL);
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

   static const pair<std::string, FileExtension> TranslateFileExtension[] =
      {
      { string(".bmp"), ExtensionBMP },
      { string(".BMP"), ExtensionBMP },
      { string(".dds"), ExtensionDDS },
      { string(".DDS"), ExtensionDDS },
      { string(".exr"), ExtensionEXR },
      { string(".EXR"), ExtensionEXR },
      { string(".hdr"), ExtensionHDR },
      { string(".HDR"), ExtensionHDR },
      { string(".png"), ExtensionPNG },
      { string(".PNG"), ExtensionPNG },
      { string(".tga"), ExtensionTGA },
      { string(".TGA"), ExtensionTGA }
      };

   Ptr<en::gpu::Texture> Interface::Load::texture(const string& filename, const gpu::ColorSpace colorSpace)
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
//   bool invertHorizontal = false;
//#endif

   Ptr<gpu::Texture> texture = nullptr;

   // Iterate over all registered file extensions and execute proper loading function
   FileExtension type = ExtensionUnknown;
   uint32 entries = sizeof(TranslateFileExtension) / sizeof(pair<std::string, FileExtension>);
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
      return texture;

   // Load selected file type
   switch(type)
      {
      case ExtensionBMP:
         texture = bmp::load(filename);
         break;

      case ExtensionDDS:
         texture = dds::load(filename);
         break;

      case ExtensionEXR:
         texture = exr::load(filename);
         break;
      
      case ExtensionHDR:
         texture = hdr::load(filename);
         break;

      case ExtensionPNG:
         texture = png::load(filename, colorSpace, invertHorizontal);
         break;

      case ExtensionTGA:
         texture = tga::load(filename);
         break;

      default:
         // How we get here?
         assert( 0 );
         break;
      };

#ifdef EN_PROFILE
   Log << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - " << filename << endl;
#endif

   return texture;
   }

   bool Interface::Load::texture(Ptr<gpu::Texture> dst, const uint16 layer, const string& filename, const gpu::ColorSpace colorSpace)
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
//   bool invertHorizontal = false;
//#endif

   // Iterate over all registered file extensions and execute proper loading function
   FileExtension type = ExtensionUnknown;
   uint32 entries = sizeof(TranslateFileExtension) / sizeof(pair<std::string, FileExtension>);
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
      return false;

   // Load selected file type
   bool result = false;
   switch(type)
      {
      case ExtensionBMP:
         result = bmp::load(dst, layer, filename);
         break;

      case ExtensionPNG:
         result = png::load(dst, layer, filename, colorSpace, invertHorizontal);
         break;

      case ExtensionTGA:
         result = tga::load(dst, layer, filename);
         break;

      default:
         break;
      };

   // found = filename.rfind(".bmp");
   // if ( found != string::npos &&
   //      found == (length - 4) )
   //    return bmp::load();
   // 
   // found = filename.rfind(".BMP");
   // if ( found != string::npos &&
   //      found == (length - 4) )
   //    return bmp::load(dst, layer, filename);
   // 
   // found = filename.rfind(".png");
   // if ( found != string::npos &&
   //      found == (length - 4) )
   //    return png::load(dst, layer, filename, colorSpace, invertHorizontal);
   // 
   // found = filename.rfind(".PNG");
   // if ( found != string::npos &&
   //      found == (length - 4) )
   //    return png::load(dst, layer, filename, colorSpace, invertHorizontal);
   // 
   // found = filename.rfind(".tga");
   // if ( found != string::npos &&
   //      found == (length - 4) )
   //    return tga::load(dst, layer, filename);
   // 
   // found = filename.rfind(".TGA");
   // if ( found != string::npos &&
   //      found == (length - 4) )
   //    return tga::load(dst, layer, filename);


#ifdef EN_PROFILE
   Log << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - " << filename << endl;
#endif

   return result;
   }

//   bool Interface::Load::texture(Ptr<gpu::Texture> dst, const gpu::TextureFace face, const uint16 layer, const string& filename, const gpu::ColorSpace colorSpace)
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
//   if ( found != string::npos &&
//        found == (length - 4) )
//      return png::load(dst, face, layer, filename, colorSpace, invertHorizontal);
//
//   found = filename.rfind(".PNG");
//   if ( found != string::npos &&
//        found == (length - 4) )
//      return png::load(dst, face, layer, filename, colorSpace, invertHorizontal);
//
//   return false;
//   }

   void Interface::Free::sound(const string& filename)
   {
   // Find specified sample and check if it is used
   // by other part of code. If it isn't it can be
   // safely deleted (assigment operator will perform
   // automatic resource deletion).
   map<string, Ptr<audio::Sample> >::iterator it = ResourcesContext.sounds.find(filename);
   if (it != ResourcesContext.sounds.end())
      if (it->second.references() == 1)
         it->second = NULL;
   }

   void Interface::Free::texture(const string& filename)
   {
   // Find specified texture and check if it is used
   // by other part of code. If it isn't it can be
   // safely deleted (assigment operator will perform
   // automatic resource deletion).
   map<string, Ptr<gpu::Texture> >::iterator it = ResourcesContext.textures.find(filename);
   if (it != ResourcesContext.textures.end())
      if (it->second.references() == 1)
         it->second = NULL;
   }

   }

resource::Context   ResourcesContext;
resource::Interface Resources;
}

//template<> bool (*ProxyInterface<en::resource::FontDescriptor>::destroy)(en::resource::FontDescriptor* const)         = en::resource::FontDestroy;
//template<> bool (*ProxyInterface<en::resource::MaterialDescriptor>::destroy)(en::resource::MaterialDescriptor* const) = en::resource::MaterialDestroy;
template<> bool (*ProxyInterface<en::resource::ModelDescriptor>::destroy)(en::resource::ModelDescriptor* const)       = en::resource::ModelDestroy;
