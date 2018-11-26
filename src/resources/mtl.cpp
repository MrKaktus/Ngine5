/*

 Ngine v5.0
 
 Module      : MTL files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading materials from *.mtl files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/utilities/parser.h"
#include "utilities/strings.h"
#include "resources/context.h" 
#include "resources/mtl.h"    

namespace en
{
   namespace mtl
   {

   bool load(const std::string& filename, const std::string& name, en::resource::Material& material)
   {
   using namespace en::storage;

   // Try to reuse already loaded material
   //if (ResourcesContext.materials.find(name) != ResourcesContext.materials.end())
   //   return ResourcesContext.materials[name];

   // Open MTL file 
   std::shared_ptr<File> file = Storage->open(filename);
   if (!file)
      {
      file = Storage->open(en::ResourcesContext.path.materials + filename);
      if (!file)
         {
         Log << en::ResourcesContext.path.materials + filename << std::endl;
         Log << "ERROR: There is no such file!\n";
         return false; //en::resource::Material();
         }
      }

   // Allocate temporary buffer for file content ( 4GB max size! )
   uint64 size = file->size();
   uint8* buffer = NULL;
   buffer = new uint8[static_cast<uint32>(size)];
   if (!buffer)
      {
      Log << "ERROR: Not enough memory!\n";
      return false; //en::resource::Material();
      }
   
   // Read file to buffer and close file
   if (!file->read(buffer))
      {
      Log << "ERROR: Cannot read whole mtl file!\n";
      return false; //en::resource::Material();
      }    
   file = nullptr;
   
   // Create parser to quickly process text from file
   Nparser text(buffer, size);

   // Try to add new buffer descriptor 
   //en::resource::MaterialDescriptor* material = ResourcesContext.storage.materials.allocate();
   //if (!material)
   //   {
   //   Log << "ERROR: Cannot allocate new material!";
   //   return en::resource::Material();
   //   }

   //// MTL files doesn't contain shaders so attach default shader to it
   //material->program = ResourcesContext.defaults.program;
   //if (!material->program)
   //   {
   //   Log << "ERROR: Cannot attach default shader to material!";
   //   ResourcesContext.storage.materials.free(material);
   //   return en::resource::Material();
   //   }

   //// Calculate space required for program parameter values
   //gpu::Program program = material->program;
   //material->parameters.size = 0;
   //for(uint32 i=0; i<program.parameters(); ++i)
   //   material->parameters.size += program.parameter(i).size();

   //// Allocate shared buffer for program parameter values
   //material->parameters.buffer = new uint8[material->parameters.size];
   //if (!material->parameters.buffer)
   //   {
   //   ResourcesContext.storage.materials.free(material);
   //   return en::resource::Material();
   //   }

   //// Add all program parameters with default values to material
   //uint32 offset = 0;
   //material->parameters.list.resize(program.parameters());
   //memset(material->parameters.buffer, 0, material->parameters.size);
   //for(uint32 i=0; i<program.parameters(); ++i)
   //   {
   //   en::resource::MaterialParameter& parameter = material->parameters.list[i];
   //   parameter.handle = program.parameter(i);
   //   parameter.value  = (void*)((uint8*)(material->parameters.buffer) + offset);
   //   parameter.name   = parameter.handle.name();

   //   offset += program.parameter(i).size();
   //   }  

   // WA: Ensure that the same texture is not used twice as ambient and diffuse map.
   std::string AmbientMapName;

   // Search for specified material in file
   std::string command;
   std::string word;
   bool eol = false;
   bool found = false;
   while(!text.end())
        {
        // Read commands until it is end of file
        if (!text.read(command, eol))
           continue;

        // Material name
        if (command == "newmtl")
           if (!eol && text.read(word, eol))    
              { 
              // If material is still not found, check if 
              // this is it's searched declaration.
              if (!found)
                 {
                 if (word == name)
                    found = true;
                 }
              else
              // If material was already found and parsed,
              // this is declaration of next material which
              // means that the parsing is finished.
                 break;
              }

        if (found)
           {
           // Ambient texture
           if (command == "map_Ka")
              if (!eol && text.read(word, eol))
                 {
                 // Read last word in line
                 while(!eol)
                      text.read(word, eol);
 
                 //material.ambient.map = en::Resources.load.texture(word);
                 //if (material.ambient.map)
                    {
                    // WA: Store information about used texture, to ensure that 
                    //     it won't be used twice as ambient and diffuse map.
                    AmbientMapName = word;
                    }

                 //resource::MaterialSampler parameter;
                 //parameter.handle = material->program.sampler(string("enAmbientMap"));
                 //if (parameter.handle)
                 //   {
                 //   parameter.texture = en::Resources.load.texture(word);
                 //   parameter.name    = parameter.handle.name();
                 //   if (parameter.texture)
                 //      {
                 //      material->samplers.push_back(parameter);
                 //      material->state.texture.ambient = true;

                 //      // WA: Store information about used texture, to ensure that 
                 //      //     it won't be used twice as ambient and diffuse map.
                 //      AmbientMapName = word;
                 //      }
                 //   }         
                 }     

           // Diffuse texture
           if (command == "map_Kd")
              if (!eol && text.read(word, eol))
                 {
                 // Read last word in line
                 while(!eol)
                      text.read(word, eol);

                 // WA: Ensure that the same texture won't be used twice,
                 //     as ambient and diffuse map.
                 //if (material.ambient.map)
                 //   if (AmbientMapName == word)
                 //      material.ambient.map = en::ResourcesContext.defaults.enAmbientMap;

                 material.albedo = en::Resources.load.texture(word);

                 //resource::MaterialSampler parameter;
                 //parameter.handle = material->program.sampler(string("enDiffuseMap"));
                 //if (parameter.handle)
                 //   {
                 //   // WA: Ensure that the same texture won't be used twice,
                 //   //     as ambient and diffuse map.
                 //   if (material->state.texture.ambient)
                 //      if (AmbientMapName == word)
                 //         for(uint8 i=0; i<material->samplers.size(); ++i)
                 //            if (material->samplers[i].name == word.c_str())
                 //               {
                 //               material->samplers[i].texture = en::ResourcesContext.defaults.enAmbientMap;
                 //               break;
                 //               }

                 //   parameter.texture = en::Resources.load.texture(word);
                 //   parameter.name    = parameter.handle.name();
                 //   if (parameter.texture)
                 //      {
                 //      material->samplers.push_back(parameter);
                 //      material->state.texture.diffuse = true;
                 //      }
                 //   }         
                 }

           // // Specular texture
           // if (command == "map_Ks")
           //    if (!eol && text.read(word, eol))
           //       {
           //       // Read last word in line
           //       while(!eol)
           //            text.read(word, eol);
           // 
           //       material.specular = en::Resources.load.texture(word);
           // 
/*         //         resource::MaterialSampler parameter;
           //       parameter.handle = material->program.sampler(string("enSpecularMap"));
           //       if (parameter.handle)
           //          {
           //          parameter.texture = en::Resources.load.texture(word);
           //          parameter.name    = parameter.handle.name();
           //          if (parameter.texture)
           //             {
           //             material->samplers.push_back(parameter);
           //             material->state.texture.specular = true;
           //             }
           //          } */        
           //       }

           // Normalmap / bumpmap texture
           if ( command == "map_bump" ||
                command == "bump" )
              if (!eol && text.read(word, eol))
                 {
                 // Read last word in line
                 while(!eol)
                      text.read(word, eol);

                 material.normal = en::Resources.load.texture(word);

/*                 resource::MaterialSampler parameter;
                 parameter.handle = material->program.sampler(string("enNormalMap"));
                 if (parameter.handle)
                    {
                    parameter.texture = en::Resources.load.texture(word);
                    parameter.name    = parameter.handle.name();
                    if (parameter.texture)
                       {
                       material->samplers.push_back(parameter);
                       material->state.texture.normal = true;
                       }
                    }*/         
                 }

           // Displacement / heightmap texture
           if ( command == "decal" ||
                command == "disp" )
              if (!eol && text.read(word, eol))
                 {
                 // Read last word in line
                 while(!eol)
                      text.read(word, eol);

                 material.displacement = en::Resources.load.texture(word);

/*                 resource::MaterialSampler parameter;
                 parameter.handle = material->program.sampler(string("enDisplacementMap"));
                 if (parameter.handle)
                    {
                    parameter.texture = en::Resources.load.texture(word);
                    parameter.name    = parameter.handle.name();
                    if (parameter.texture)
                       {
                       material->samplers.push_back(parameter);
                       material->state.texture.displacement = true;
                       }
                    } */        
                 }

           // Alpha channel texture
           if (command == "map_d")
              if (!eol && text.read(word, eol))
                 {
                 // Read last word in line
                 while(!eol)
                      text.read(word, eol);

                 material.opacity = en::Resources.load.texture(word);

/*                 resource::MaterialSampler parameter;
                 parameter.handle = material->program.sampler(string("enAlphaMap"));
                 if (parameter.handle)
                    {
                    parameter.texture = en::Resources.load.texture(word);
                    parameter.name    = parameter.handle.name();
                    if (parameter.texture)
                       {
                       material->samplers.push_back(parameter);
                       material->state.texture.alpha = true;
                       }
                    } */        
                 }

           // Not supported in PBR Model

           // // Emmisive coefficient
           // if (command == "Ke")
           //    if (!eol && text.read(word, eol))
           //       {
           //       //float3 coefficient(0.0f, 0.0f, 0.0f);
           //       float3& coefficient = material.emmisive.color;
           //       coefficient.r = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.g = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.b = stringTo<float>(word);
           // 
           //       //// Overload with value from material file
           //       //for(uint32 i=0; i<material->program.parameters(); ++i)
           //       //   {
           //       //   en::resource::MaterialParameter& parameter = material->parameters.list[i];
           //       //   if (strcmp(parameter.name, "enKe") == 0)
           //       //      *((float3*)parameter.value) = coefficient;
           //       //   }
           //       }
           // 
           // // Ambient coefficient
           // if (command == "Ka")
           //    if (!eol && text.read(word, eol))
           //       {
           //       //float3 coefficient(0.0f, 0.0f, 0.0f);
           //       float3& coefficient = material.ambient.color;
           //       coefficient.r = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.g = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.b = stringTo<float>(word);
           // 
           //       //// Overload with value from material file
           //       //for(uint32 i=0; i<material->program.parameters(); ++i)
           //       //   {
           //       //   en::resource::MaterialParameter& parameter = material->parameters.list[i];
           //       //   if (strcmp(parameter.name, "enKa") == 0)
           //       //      *((float3*)parameter.value) = coefficient;
           //       //   }       
           //       }
           // 
           // // Diffuse coefficient
           // if (command == "Kd")
           //    if (!eol && text.read(word, eol))
           //       {
           //       //float3 coefficient(0.0f, 0.0f, 0.0f);
           //       float3& coefficient = material.diffuse.color;
           //       coefficient.r = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.g = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.b = stringTo<float>(word);
           // 
           //       //// Overload with value from material file
           //       //for(uint32 i=0; i<material->program.parameters(); ++i)
           //       //   {
           //       //   en::resource::MaterialParameter& parameter = material->parameters.list[i];
           //       //   if (strcmp(parameter.name, "enKd") == 0)
           //       //      *((float3*)parameter.value) = coefficient;
           //       //   }        
           //       }
           // 
           // // Specular coefficient
           // if (command == "Ks")
           //    if (!eol && text.read(word, eol))
           //       {
           //       //float3 coefficient(0.0f, 0.0f, 0.0f);
           //       float3& coefficient = material.specular.color;
           //       coefficient.r = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.g = stringTo<float>(word);
           //       if (!eol && text.read(word, eol))
           //          coefficient.b = stringTo<float>(word);
           // 
           //       //// Overload with value from material file
           //       //for(uint32 i=0; i<material->program.parameters(); ++i)
           //       //   {
           //       //   en::resource::MaterialParameter& parameter = material->parameters.list[i];
           //       //   if (strcmp(parameter.name, "enKs") == 0)
           //       //      *((float3*)parameter.value) = coefficient;
           //       //   }        
           //       }
           // 
           // // Specular exponenr
           // if (command == "Ns")
           //    if (!eol && text.read(word, eol))
           //       {
           //       material.specular.exponent = stringTo<float>(word);
           // 
           //       //// Overload with value from material file
           //       //for(uint32 i=0; i<material->program.parameters(); ++i)
           //       //   {
           //       //   en::resource::MaterialParameter& parameter = material->parameters.list[i];
           //       //   if (strcmp(parameter.name, "enShininess") == 0)
           //       //      *((float*)parameter.value) = stringTo<float>(word);
           //       //   }        
           //       }
           //
           // // Transparency "dissolve"
           // if (command == "d")
           //    if (!eol && text.read(word, eol))
           //       {
           //       float3& coefficient = material.transparency.color;
           //       coefficient.r = stringTo<float>(word);
           //       coefficient.g = stringTo<float>(word);
           //       coefficient.b = stringTo<float>(word);
           //       }

           // float3 translucency;         // Tf r g b  - transmission filter, which colors are filtered when light goes through object          
           // float  refraction;           // Ni - optical density  [0.001 - 10]  1.0 - light does not bend as it passes through an object
           }

        // Skip not relevant part of the line
        text.skipToNextLine();    
        }

   // Attach default values to parameters not found in material
   // TODO: Finish!!!
   //if (!material->state.texture.specular)
   //   {
   //   resource::MaterialSampler parameter;
   //   parameter.handle = material->program.sampler(string("enSpecularMap"));
   //   if (parameter.handle)
   //      {
   //      parameter.texture = en::ResourcesContext.defaults.enSpecularMap;
   //      parameter.name    = parameter.handle.name();
   //      if (parameter.texture)
   //         {
   //         material->samplers.push_back(parameter);
   //         material->state.texture.specular = true;
   //         }
   //      }         
   //   }
   //if (!material->state.texture.normal)
   //   {
   //   resource::MaterialSampler parameter;
   //   parameter.handle = material->program.sampler(string("enNormalMap"));
   //   if (parameter.handle)
   //      {
   //      parameter.texture = en::ResourcesContext.defaults.enNormalMap;
   //      parameter.name    = parameter.handle.name();
   //      if (parameter.texture)
   //         {
   //         material->samplers.push_back(parameter);
   //         material->state.texture.normal = true;
   //         }
   //      }         
   //   }



   //if (!material->state.texture.alpha)
   //   {
   //   resource::MaterialSampler parameter;
   //   parameter.handle = material->program.sampler(string("enAlphaMap"));
   //   if (parameter.handle)
   //      {
   //      parameter.texture = en::ResourcesContext.defaults.enAlphaMap;
   //      parameter.name    = parameter.handle.name();
   //      if (parameter.texture)
   //         {
   //         material->samplers.push_back(parameter);
   //         material->state.texture.alpha = true;
   //         }
   //      }         
   //   }

   // If material was not found in this file return nothing
   if (!found)
      {
      Log << "Error! Material \"" << name << "\" was not found in file: " << filename << " !\n";
      //ResourcesContext.storage.materials.free(material);
      return false; //en::resource::Material();
      }

   //// Update list of loaded materials
   //ResourcesContext.materials.insert(pair<string, en::resource::Material>(name, material));

   // Return material interface
   return true; //en::resource::Material(material);
   }

   }
}

// Creating material:
// + check if material already exist in resources manager
// ~ check if material has information about correct shader for current platform ( es, ogl, dx?)
// - try to compile shader
// + if there is no shader, or it cannot compile correctly
//   use default engine shader for this material
// - for each value in material file, find active parameter or sampler in shader program
//   if it is found, add it to material parameters/samplers list and set it's default value/binding
// - for each active parameter/sampler in shader program, if not already in
//   material parameter list, try to match engine default parameter value/binding
// - report all errors

// TODO: Iteration by program parameters/samplers, taking their names and types

   // object -> (Uses) model
   // - private        - reference
   //   copy of          model material
   //   material         holds default
   //  (overloads        values
   //   material       - here are stored
   //   of model)        default values
   // - here are
   //   stored 
   //   values of
   //   material
   //   parameters
