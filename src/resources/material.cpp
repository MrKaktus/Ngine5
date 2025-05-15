/*

 Ngine v5.0
 
 Module      : MATERIAL files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading materials from *.material files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/utilities/parser.h"
#include "utilities/strings.h"
#include "resources/interface.h" 
#include "resources/material.h"    

namespace en
{
namespace material
{

en::resources::Material load(const std::string& filename)
{
    using namespace en::storage;

    // Try to reuse already loaded material
    if (ResourcesContext.materials.find(filename) != ResourcesContext.materials.end())
    {
        return ResourcesContext.materials[filename];
    }

    // Open MATERIAL file
    File* file = Storage->open(filename);
    if (!file)
    {
        std::string fullPath = Resources->assetsPath() + filename;
        file = Storage->open(fullPath);
        if (!file)
        {
            logError("There is no such file!\n%s\n", fullPath.c_str());
            return nullptr;
        }
    }

    // Allocate temporary buffer for file content
    uint64 size = file->size();
    uint8* buffer = nullptr;
    buffer = new uint8[static_cast<uint32>(size)];
    if (!buffer)
    {
        enLog << "ERROR: Not enough memory!\n";
        delete file;
        return en::resources::Material();
    }
   
    // Read file to buffer and close file
    if (!file->read(buffer))
    {
        enLog << "ERROR: Cannot read whole material file!\n";
        delete file;
        return en::resources::Material();
    }    
    delete file;
   
    // Create parser to quickly process text from file
    Parser text(buffer, size);

    // Try to add new material descriptor 
    // TODO: Refactor to new Model description
    //en::resources::MaterialDescriptor* material = ResourcesContext.storage.materials.allocate();
    //if (!material)
    //   {
    //   enLog << "ERROR: Cannot allocate new material!";
    //   return en::resources::Material();
    //   }

    // TODO: Finish this !!!!
    return en::resources::Material();


    //// Check material file version
    //string line;
    //bool eol = false;
    //bool found = false;
    //text.readLine(line);
    //if (line != "version 1.0")
    //   {
    //   enLog << "ERROR: Font file corrupted!";
    //   return en::resources::Font();
    //   }      

    //// MTL files doesn't contain shaders so attach default shader to it
    //material->program = ResourcesContext.defaults.program;
    //if (!material->program)
    //   {
    //   enLog << "ERROR: Cannot attach default shader to material!";
    //   ResourcesContext.storage.materials.free(material);
    //   return en::resources::Material();
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
    //   return en::resources::Material();
    //   }

    //// Add all program parameters with default values to material
    //uint32 offset = 0;
    //material->parameters.list.resize(program.parameters());
    //memset(material->parameters.buffer, 0, material->parameters.size);
    //for(uint32 i=0; i<program.parameters(); ++i)
    //   {
    //   en::resources::MaterialParameter& parameter = material->parameters.list[i];
    //   parameter.handle = program.parameter(i);
    //   parameter.value  = (void*)((uint8*)(material->parameters.buffer) + offset);
    //   parameter.name   = parameter.handle.name();
    //
    //   offset += program.parameter(i).size();
    //   }  

    //// Search for specified material in file
    //string command;
    //string word;
    //while(!text.end())
    //     {
    //     // Read commands until it is end of file
    //     if (!text.read(command, eol))
    //        continue;

    //     // Diffuse texture
    //     if (command == "diffuse")
    //        if (!eol && text.read(word, eol))    
    //           {
    //           //material->program =
    //           // TODO: FINISH !!

    //           // If material is still not found, check i
    //           }

    //      // TODO: Finish !!!


    //     // Diffuse texture
    //     if (command == "diffuse")
    //        if (!eol && text.read(word, eol))
    //           {
    //           // Read last word in line
    //           while(!eol)
    //                text.read(word, eol);
    //     
    //           resources::MaterialSampler parameter;
    //           parameter.handle = material->program.sampler(string("enDiffuseMap"));
    //           if (parameter.handle)
    //              {
    //              // WA: Ensure that the same texture won't be used twice,
    //              //     as ambient and diffuse map.
    //              if (material->state.texture.ambient)
    //                 if (AmbientMapName == word)
    //                    for(uint8 i=0; i<material->samplers.size(); ++i)
    //                       if (material->samplers[i].name == word.c_str())
    //                          {
    //                          material->samplers[i].texture = en::ResourcesContext.defaults.enAmbientMap;
    //                          break;
    //                          }
    //     
    //              parameter.texture = en::Resources.load.texture(word);
    //              parameter.name    = parameter.handle.name();
    //              if (parameter.texture)
    //                 {
    //                 material->samplers.push_back(parameter);
    //                 material->state.texture.diffuse = true;
    //                 }
    //              }         
    //           }
    //     
    //     



    //      // Skip not relevant part of the line
    //      text.skipToNextLine();   
    //      }
}

} // en::material
} // en
