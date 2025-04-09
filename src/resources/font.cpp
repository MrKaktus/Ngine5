/*

 Ngine v5.0
 
 Module      : Font files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading font description from *.font files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/utilities/parser.h"
#include "utilities/strings.h"
#include "resources/context.h" 
  
#include "core/rendering/device.h"

namespace en
{
namespace resource
{

//# IMPLEMENTATION
//##########################################################################

Font::~Font()
{
}

FontImp::FontImp() :
    height(0.0f),
    resource(nullptr)
{
    memset(&table[0], 0, 256 * sizeof(Character));
}

FontImp::~FontImp()
{
    resource = nullptr;
}

std::shared_ptr<en::gpu::Texture> FontImp::texture(void) const
{
    return resource;
}

std::shared_ptr<Mesh> FontImp::text(const std::string text, const bool screenspace) const
{
    using namespace gpu;

    // Be sure there is anything to render
    assert( text.size() > 0 );

    // Create buffer with text geometry
    struct CharacterVertex
    {
        float2 pos;
        float2 uv;
    };

    uint32 vertices = static_cast<uint32>(text.size()) * 2 * 3;
    CharacterVertex* data = new CharacterVertex[vertices];

    float2 offset;
    for(uint16 i=0; i<text.size(); ++i)
    {
        uint8 id = (unsigned char)text[i];
 
        if (text[i] == '\n')
        {
            offset.x = 0.0f;
            if (screenspace)
            {
                offset.y += height;
            }
            else
            {
                offset.y -= height;
            }

            continue;
        }
          
        // Default CCW oriented faces.
        //    
        //    offset.xy
        //    |
        //    |
        //           4
        //  5 +------+ 2
        //    |     /|
        //    | 1  / |
        //    |   /  |
        //    |  /   |
        //    | /  0 |  
        //    |/     |
        //  3 +------+ 1  -------- text base  
        //    0      
        //                -------- line height (for "j" "q" etc.)
        //
        float left   = offset.x + table[id].offset.x;
        float right  = left + table[id].width;
        float top    = offset.y - table[id].offset.y;
        float bottom = top - table[id].height;
        if (screenspace)
        {
            top    = offset.y + table[id].offset.y;
            bottom = offset.y + table[id].offset.y + table[id].height;
        }

        // Position in object space, coordinates in OpenGL ST space
        data[i*6+0].pos = float2(left,  bottom);
        data[i*6+1].pos = float2(right, bottom);
        data[i*6+2].pos = float2(right, top);
        data[i*6+0].uv  = float2(table[id].position.u,                   table[id].position.v - table[id].height);
        data[i*6+1].uv  = float2(table[id].position.u + table[id].width, table[id].position.v - table[id].height);
        data[i*6+2].uv  = float2(table[id].position.u + table[id].width, table[id].position.v);

        data[i*6+3].pos = float2(left,  bottom);
        data[i*6+4].pos = float2(right, top);
        data[i*6+5].pos = float2(left,  top);
        data[i*6+3].uv  = float2(table[id].position.u,                   table[id].position.v - table[id].height);
        data[i*6+4].uv  = float2(table[id].position.u + table[id].width, table[id].position.v);
        data[i*6+5].uv  = float2(table[id].position.u,                   table[id].position.v);

        // Update position for next character
        offset.x += table[id].advance;
    }

    // Create buffer in GPU (CCW Triangles)
    Formatting formatting(Attribute::v2f32, Attribute::v2f32); // inPosition, inTexCoord0
    std::unique_ptr<Buffer> vertex(en::ResourcesContext.defaults.enHeapBuffers->createBuffer(vertices, formatting));
   
    // TODO: !!! populate vertex with data
   
    delete [] data;

    // Create mesh with text geometry
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    // TODO: Refactor to new Model description
    //mesh->name = "text";
    //mesh->material.albedo = resource;
    //mesh->geometry.buffer = vertex;
    //mesh->geometry.begin  = 0;
    //mesh->geometry.end    = 0;
    //mesh->elements.type   = Triangles;

    return mesh;
}

//# CONTEXT
//##########################################################################

// uint8 id;
// uint16 x;
// uint16 y;
// uint16 width;
// uint16 height;
// sint16 offsetX;
// sint16 offsetY;
// uint16 advance;

std::shared_ptr<en::resource::Font> loadFont(const std::string& filename)
{
    using namespace en::storage;

    // Open .fnt file
    std::shared_ptr<File> file = Storage->open(filename);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.fonts + filename);
        if (!file)
        {
            Log << std::string("ERROR: There is no such file " + en::ResourcesContext.path.fonts + filename + " !\n");
            return std::shared_ptr<en::resource::Font>(nullptr);
        }
    }

    // Allocate temporary buffer for file content
    uint64 size = file->size();
    uint8* buffer = NULL;
    buffer = new uint8[static_cast<uint32>(size)];
    if (!buffer)
    {
        Log << "ERROR: Not enough memory!\n";
        return std::shared_ptr<en::resource::Font>(nullptr);
    }
   
    // Read file to buffer and close file
    if (!file->read(buffer))
    {
        Log << "ERROR: Cannot read whole font file!\n";
        return std::shared_ptr<en::resource::Font>(nullptr);
    }    
    file = nullptr;

    // Create new font
    std::shared_ptr<FontImp> font = std::make_shared<FontImp>();
   
    // Create parser to quickly process text from file
    Parser text(buffer, size);



    float  texWidth = 1.0f;
    float  texHeight = 1.0f;
    std::string texName;
    uint32 lineHeight;
    uint32 characters;
    uint32 id = 0;
   
    std::string line, word, command, value;
    bool eol = false;
    while(!text.end())
    {
        if (!text.read(word, eol))
        {
            continue;
        }

        if (word == "common")
        {
            while(text.read(word, eol))
            {
                std::size_t found = word.find("=");
                if (found != std::string::npos)
                {
                    command = word.substr(0, found);
                    value   = word.substr(found+1);

                    if (command == "lineHeight")
                    {
                        font->height = static_cast<float>(stringTo<uint32>(value));
                    }
                    else
                    if (command == "scaleW")
                    {
                        texWidth  = static_cast<float>(stringTo<uint32>(value));
                    }
                    else
                    if (command == "scaleH")
                    {
                        texHeight = static_cast<float>(stringTo<uint32>(value));
                        font->height /= texHeight;
                    }
                    else
                    if (command == "pages")
                    {
                        assert(stringTo<uint32>(value) == 1);
                    }

                    // if (command == "packed")
                    // if (command == "alphaChnl")
                    // if (command == "redChnl")
                    // if (command == "greenChnl")
                    // if (command == "blueChnl")
                    // if (command == "base")
                }

                if (eol)
                {
                    break;
                }
            }
        }

        // Load used texture
        if (word == "page")
        {
            while(text.read(word, eol))
            {
                std::size_t found = word.find("=");
                if (found != std::string::npos)
                {
                    command = word.substr(0, found);
                    value   = word.substr(found+1);

                    if (command == "id")
                    {
                        assert(stringTo<uint32>(value) == 0);
                    }
                    else
                    if (command == "file")
                    {
                        std::string name = value.substr(1, value.length()-2);
                        font->resource = Resources.load.texture(name);
                        if (!font->resource)
                        {
                            font = nullptr;
                            Log << "ERROR: Cannot load Font texture!\n";
                            return std::shared_ptr<Font>(nullptr);
                        }
                    }
                }

                if (eol)
                {
                    break;
                }
            }
        }

        // Check described characters count
        if (word == "chars")
        {
            while(text.read(word, eol))
            {
                std::size_t found = word.find("=");
                if (found != std::string::npos)
                {
                    command = word.substr(0, found);
                    value   = word.substr(found+1);

                    if (command == "count")
                    {
                        characters = stringTo<uint32>(value);
                    }
                }

                if (eol)
                {
                    break;
                }
            }
        }
   
        // Decode characters
        if (word == "char")
        {
            while(text.read(word, eol))
            {
                std::size_t found = word.find("=");
                if (found != std::string::npos)
                {
                    command = word.substr(0, found);
                    value   = word.substr(found+1);

                    if (command == "id")
                    {
                        id = stringTo<uint32>(value);
                    }
                    else
                    if (command == "x")
                    {
                        font->table[id].position.x = static_cast<float>(stringTo<uint32>(value)) / texWidth;
                    }
                    else
                    if (command == "y")
                    {
                        font->table[id].position.y = (1.0f - static_cast<float>(stringTo<uint32>(value))) / texHeight;
                    }
                    else
                    if (command == "width")
                    {
                        font->table[id].width      = static_cast<float>(stringTo<uint32>(value)) / texWidth;
                    }
                    else
                    if (command == "height")
                    {
                        font->table[id].height     = static_cast<float>(stringTo<uint32>(value)) / texHeight;
                    }
                    else
                    if (command == "xoffset")
                    {
                        font->table[id].offset.x   = static_cast<float>(stringTo<sint32>(value)) / texWidth;
                    }
                    else
                    if (command == "yoffset")
                    {
                        font->table[id].offset.y   = static_cast<float>(stringTo<sint32>(value)) / texHeight;
                    }
                    else
                    if (command == "xadvance")
                    {
                        font->table[id].advance    = static_cast<float>(stringTo<uint32>(value)) / texWidth;
                    }
                    else
                    if (command == "page")
                    {
                        assert(stringTo<uint32>(value) == 0);
                    }

                    // if (command == "chnl")
                }

                if (eol)
                {
                    break;
                }
            }
        }

        text.skipToNextLine();
    }

    return font;




   //// Check font file version
   //bool eol = false;
   //string line, word;
   //text.readLine(line);
   //if (line != "version 1.0")
   //   {
   //   Log << "ERROR: Font file corrupted!";
   //   return std::shared_ptr<en::resource::Font>(nullptr);
   //   }      

   //// Load texture used by font
   //string textureName;
   //text.skipToNextLine();
   //if (!text.readLine(textureName))
   //   {
   //   Log << "ERROR: Font file corrupted!";
   //   return std::shared_ptr<en::resource::Font>(nullptr);
   //   }

   ////gpu::Texture texture = Resources.load.texture(string("resources/textures/" + textureName));
   //font->texture = Resources.load.texture(string("resources/textures/" + textureName));
   //if (!font->texture)
   //   {
   //   Log << "ERROR: Font texture incorrect!";
   //   return en::resource::Font();
   //   }

   //// Parse font height 
   //text.skipToNextLine();
   //if (!text.readLine(line))
   //   {
   //   Log << "ERROR: Font file corrupted!";
   //   return en::resource::Font();
   //   }
   //font->height = stringTo<float>(line);
   //text.skipToNextLine();

   //// Reads chars descriptors
   //for(uint16 i=0; i<256; i++)
   //   {
   //   // Parse font texcoord S
   //   if (!text.read(word, eol))
   //      {
   //      Log << "ERROR: Font file corrupted!";
   //      return en::resource::Font();
   //      }
   //   font->table[i].pos.s = stringTo<float>(word);

   //   // Parse font texcoord T
   //   if (eol || !text.read(word, eol))
   //      {
   //      Log << "ERROR: Font file corrupted!";
   //      return en::resource::Font();
   //      }
   //   font->table[i].pos.t = stringTo<float>(word);

   //   // Parse font width
   //   if (eol || !text.read(word, eol))
   //      {
   //      Log << "ERROR: Font file corrupted!";
   //      return en::resource::Font();
   //      }
   //   font->table[i].width = stringTo<float>(word);

   //   text.skipToNextLine();
   //   eol = false;
   //   }
   
//   // Try to add new buffer descriptor 
//   en::resource::MaterialDescriptor* material = ResourcesContext.storage.materials.allocate();
//   if (!material)
//      {
//      Log << "ERROR: Cannot allocate new material!";
//      return en::resource::Font();
//      }
//
//   // Create default font program
//   std::string vsCode, fsCode;
//
//#ifdef EN_OPENGL_DESKTOP
//   en::Storage.read(string("resources/engine/shaders/textured.glsl.1.10.vs"), vsCode);
//   en::Storage.read(string("resources/engine/shaders/textured.glsl.1.10.fs"), fsCode);
//#endif
//#ifdef EN_OPENGL_MOBILE
//   en::Storage.read(string("resources/engine/shaders/textured.essl.1.00.vs"), vsCode);
//   en::Storage.read(string("resources/engine/shaders/textured.essl.1.00.fs"), fsCode);
//#endif
//
//   vector<gpu::Shader> shaders(2,gpu::Shader(NULL));
//   shaders[0] = Gpu.shader.create(gpu::Vertex, vsCode);
//   shaders[1] = Gpu.shader.create(gpu::Fragment, fsCode);
//   material->program = Gpu.program.create(shaders);
//
//   // Calculate space required for program parameter values
//   gpu::Program program = material->program;
//   material->parameters.size = 0;
//   for(uint32 i=0; i<program.parameters(); ++i)
//      material->parameters.size += program.parameter(i).size();
//
//   // Allocate shared buffer for program parameter values
//   material->parameters.buffer = new uint8[material->parameters.size];
//   if (!material->parameters.buffer)
//      {
//      ResourcesContext.storage.materials.free(material);
//      return en::resource::Font();
//      }
//
//   // Add all program parameters with default values to material
//   uint32 offset = 0;
//   material->parameters.list.resize(program.parameters());
//   memset(material->parameters.buffer, 0, material->parameters.size);
//   for(uint32 i=0; i<program.parameters(); ++i)
//      {
//      en::resource::MaterialParameter& parameter = material->parameters.list[i];
//      parameter.handle = program.parameter(i);
//      parameter.value  = (void*)((uint8*)(material->parameters.buffer) + offset);
//      parameter.name   = parameter.handle.name();
//
//      offset += program.parameter(i).size();
//      }  
//
//   // Attach texture to material
//   resource::MaterialSampler parameter;
//   parameter.handle = material->program.sampler(string("enDiffuseMap"));
//   if (parameter.handle)
//      {
//      parameter.texture = texture;
//      parameter.name    = parameter.handle.name();
//      material->samplers.push_back(parameter);
//      material->state.texture.diffuse = true;
//      }  
//
//   // Assign material to font
//   font->material = en::resource::Material(material);

   //// Update list of loaded fonts
   //ResourcesContext.fonts.insert(pair<string, en::resource::Font>(filename, font));

   //// Return font interface
   //return en::resource::Font(font);
}

std::shared_ptr<Font> Interface::Load::font(const std::string& filename)
{
    sint64 found;
    uint64 length = filename.length();

    // Try to reuse already loaded models
    if (ResourcesContext.fonts.find(filename) != ResourcesContext.fonts.end())
    {
        return ResourcesContext.fonts[filename];
    }

    found = filename.rfind(".fnt");
    if ( found != std::string::npos &&
         found == (length - 4) )
    {
        return loadFont(filename);
    }

    found = filename.rfind(".FNT");
    if ( found != std::string::npos &&
         found == (length - 4) )
    {
        return loadFont(filename);
    }

    return std::shared_ptr<Font>(nullptr);
}

void Interface::Free::font(const std::string& name)
{
    // Find specified font and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::shared_ptr<FontImp> >::iterator it = ResourcesContext.fonts.find(name);
    if (it != ResourcesContext.fonts.end())
    {
        if (it->second.unique())
        {
            it->second = nullptr;
        }
    }
}

} // en::resource
} // en

