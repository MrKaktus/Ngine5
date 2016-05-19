/*

 Ngine v5.0
 
 Module      : Program.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/log/log.h"
#include "core/rendering/context.h"



#include "core/rendering/opengl/glBuffer.h"
#include "core/rendering/opengl/glTexture.h"
#include "core/rendering/opengl/glInputAssembler.h"
#include "core/rendering/opengl/gl33Sampler.h"
#include "core/rendering/device.h"

namespace en
{
   namespace gpu
   {

   Shader::Shader(ShaderDescriptor* src) :
       ProxyInterface<class ShaderDescriptor>(src)
   {
   }

   //Shader::~Shader()
   //{
   //}

   Program::Program(class ProgramDescriptor* src) :
       ProxyInterface<class ProgramDescriptor>(src)
   {
   }

   //Program::~Program()
   //{
   //}

   uint32 Program::parameters(void)
   {
   assert(pointer);

   return pointer->parameters.count;
   }

   Parameter Program::parameter(const uint32 id)
   {
   assert(pointer);
   
   // Check if id is in range
   if (id >= pointer->parameters.count)
      return Parameter(NULL);
      
   // Return parameter interface
   return Parameter(&pointer->parameters.list[id]);
   }

   Parameter Program::parameter(const string& name)
   {
   assert(pointer);

   // Try to find location of specified parameter
   map<string, ParameterDescriptor*>::iterator it;
   it = pointer->parameters.names.find(name);

   if (it == pointer->parameters.names.end())
      return Parameter(NULL);
   
   // Return parameter interface
   return Parameter(it->second);
   }

   uint32 Program::blocks(void)
   {
   assert(pointer);
   return pointer->blocks.count;
   }

   Block Program::block(const uint32 id)
   {
   assert(pointer);
   
   // Check if id is in range
   if (id >= pointer->blocks.count)
      return Block(NULL);
      
   // Return parameter interface
   return Block(&pointer->blocks.list[id]);
   }

   Block Program::block(const string& name)
   {
   assert(pointer);

   // Try to find location of specified uniform block or storage block
   map<string, BlockDescriptor*>::iterator it;
   it = pointer->blocks.names.find(name);

   if (it == pointer->blocks.names.end())
      return Block(NULL);
   
   // Return block interface
   return Block(it->second);
   }

   uint32 Program::samplers(void)
   {
   assert(pointer);
   return pointer->samplers;
   }
             
   //Sampler Program::sampler(const uint32 id)
   //{
   //assert(pointer);
   //
   //// Check if id is in range
   //if (id >= pointer->samplers.count)
   //   return Sampler(NULL);
   //   
   //// Return parameter interface
   //return Sampler(&pointer->samplers.list[id]);
   //}

   bool Program::samplerLocation(const char* name, uint32& location)
   {
   assert(pointer);

   for(uint32 i=0; i<pointer->samplers; ++i)
      if (strcmp(name, pointer->sampler[i].name) == 0)
         {
         location = i;
         return true; 
         }
   return false;
   }

   bool Program::sampler(const uint32 location, Ptr<gpu::Sampler> sampler)
   {
   assert(pointer);
   assert( location < pointer->samplers );

   pointer->sampler[location].sampler = sampler;
   return true;
   }

   bool Program::texture(const uint32 location, Ptr<Texture> texture)
   {
   assert(pointer);
   assert( location < pointer->samplers );

   pointer->sampler[location].texture = texture;
   return true;
   }

   //Sampler Program::sampler(const string& name)
   //{  
   //assert(pointer);
   //
   //// Try to find location of specified parameter
   //map<string, SamplerDescriptor*>::iterator it;
   //it = pointer->samplers.names.find(name);
   //if (it == pointer->samplers.names.end())
   //   return Sampler(NULL);
   //
   //// Return sampler interface
   //return Sampler(it->second);
   //}
   
   bool Program::draw(const DrawableType type, const uint32 patchSize, const uint32 inst)
   {
   assert(pointer);
   return gl20::ProgramDraw(pointer, nullptr, nullptr, type, patchSize, inst);
   }

   bool Program::draw(const Ptr<Buffer> buffer, const DrawableType type, const uint32 patchSize, const uint32 inst)
   {
   assert(pointer);
   return gl20::ProgramDraw(pointer, buffer, nullptr, type, patchSize, inst);
   }

   bool Program::draw(const Ptr<Buffer> buffer, const Ptr<Buffer> indexBuffer, const DrawableType type, const uint32 patchSize, const uint32 inst)
   {
   assert(pointer);
   return gl20::ProgramDraw(pointer, buffer, indexBuffer, type, patchSize, inst);
   }

   Parameter::Parameter(class ParameterDescriptor* src) :
       ProxyInterface<class ParameterDescriptor>(src)
   {
   }

   //Parameter::~Parameter()
   //{
   //}

   const char* Parameter::name(void)
   {
   assert(pointer);
   return pointer->name;
   }

   uint32 Parameter::size(void)
   {
   return pointer == NULL ? 0 : gl20::ProgramParameterSize(pointer->type); 
   }

   bool Parameter::set(const void* value)
   {
   assert(pointer);

   // Treat pointed memory as variable of parameter type and save it
   uint32 size = gl20::ProgramParameterSize(pointer->type);
   memcpy(pointer->value, value, size);
   
   // If this parameter is changed first time since last
   // update in driver, put it to the list of params to
   // update.
   if (!pointer->dirty)
      {
      pointer->dirty = true;
      pointer->program->parameters.update[pointer->program->parameters.dirty] = pointer;
      pointer->program->parameters.dirty++;
      }
   return true;
   }

   template<typename type, uint16 oglType>
   bool ProgramParameterUpdate(ParameterDescriptor* parameter, const type value)
   {
   assert(parameter);
   
   // Check if type is correct
   if (parameter->type != oglType)
      return false;
   
   // Save new value of parameter    
   *(type*)(parameter->value) = value;
   
   // If this parameter is changed first time since last
   // update in driver, put it to the list of params to
   // update.
   if (!parameter->dirty)
      {
      parameter->dirty = true;
      parameter->program->parameters.update[parameter->program->parameters.dirty] = parameter;
      parameter->program->parameters.dirty++;
      }
   return true;
   }

   bool Parameter::set(const float value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<float, GL_FLOAT>(pointer, value);
   }
   
   bool Parameter::set(const float2 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<float2, GL_FLOAT_VEC2>(pointer, value);
   }
   
   bool Parameter::set(const float3 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<float3, GL_FLOAT_VEC3>(pointer, value);
   }
   
   bool Parameter::set(const float4 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<float4, GL_FLOAT_VEC4>(pointer, value);
   }
       
   bool Parameter::set(const float4x4 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<float4x4, GL_FLOAT_MAT4>(pointer, value);
   }
 
   bool Parameter::set(const sint32 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<sint32, GL_INT>(pointer, value);
   }

   bool Parameter::set(const uint32 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<uint32, GL_UNSIGNED_INT>(pointer, value);
   }

   bool Parameter::set(const sint16 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<sint16, GL_SHORT>(pointer, value);
   }

   bool Parameter::set(const uint16 value)
   {
   return pointer == NULL ? false : ProgramParameterUpdate<uint16, GL_UNSIGNED_SHORT>(pointer, value);
   }

   Block::Block(class BlockDescriptor* src) :
       ProxyInterface<class BlockDescriptor>(src)
   {
   }

   const char* Block::name(void)
   {
   assert(pointer);
   return pointer->name;
   }

   uint32 Block::size(void)
   {
   assert(pointer);
   return pointer->size; 
   }

//   bool Block::set(const Buffer& buffer)
//   {
//   if (!pointer)    
//      return false;
//   if (!buffer)     
//      return false;
//  
//   // Check if buffer type matches block type in shader
//   BufferDescriptor* buf = *((BufferDescriptor**)(&buffer));
//   if (buf->type != pointer->type)
//      return false;
//
//   pointer->buffer = buffer;
//   return true;
//   }

   Shader Interface::Shader::create(const PipelineStage stage, const string& code)
   {
   // TODO: Dynamic linking in this point
   return gl20::ShaderCreate(stage, code); 
   }
   
   Shader Interface::Shader::create(const PipelineStage stage, const vector<string>& code)
   {
   // TODO: Dynamic linking in this point
   return gl20::ShaderCreateComposed(stage, code); 
   }  

   Program Interface::Program::create(const vector<en::gpu::Shader>& shaders)
   {
   // TODO: Dynamic linking in this point
   return gl20::ProgramCreate(shaders); 
   } 

   InputDescriptor::InputDescriptor() :
       program(NULL),
       name(NULL),
       elements(0),
       type(0),
       location(0)
   {
   }

   InputDescriptor::~InputDescriptor()
   {
   delete [] name;
   }

   ParameterDescriptor::ParameterDescriptor() :
       program(NULL),
       value(NULL),
       name(NULL),
       elements(0),
       type(0),
       location(0),
       dirty(true)
   {
   }

   ParameterDescriptor::~ParameterDescriptor()
   {
    delete [] static_cast<uint8*>(value);
    delete name;
   }

   BlockDescriptor::BlockDescriptor() :
      program(nullptr),
      buffer(nullptr),
      name(nullptr),
      size(0),
      type(BufferType::Uniform),
      id(0)
   {
   }

   BlockDescriptor::~BlockDescriptor()
   {
   delete name;
   }

   ProgramDescriptor::ProgramDescriptor() :
      id(0)
   {
   inputs.names.clear();
   inputs.list       = NULL;
   inputs.count      = 0;

   parameters.names.clear();
   parameters.list   = NULL;
   parameters.update = NULL;
   parameters.dirty  = 0;
   parameters.count  = 0;

   sampler = nullptr;
   sampler = 0;
   //samplers.names.clear();
   //samplers.list     = NULL;
   //samplers.update   = NULL;
   //samplers.dirty    = 0;
   //samplers.count    = 0;
   }

   ProgramDescriptor::~ProgramDescriptor()
   {
   inputs.names.clear();
   delete [] inputs.list;

   parameters.names.clear();
   delete [] parameters.list;
   delete [] parameters.update;

   //samplers.names.clear();
   //delete [] samplers.list;
   //delete [] samplers.update;

   for(uint32 i=0; i<samplers; ++i)
      delete sampler[i].name;
   delete [] sampler;
   }

      namespace gl
      {
      bool PipelineStageSupported[PipelineStagesCount];

      #ifdef EN_OPENGL_DESKTOP

      const PipelineStageTranslation PipelineStage[PipelineStagesCount] = 
         {
         { OpenGL_2_0, GL_VERTEX_SHADER          },    //Vertex    
         { OpenGL_4_0, GL_TESS_CONTROL_SHADER    },    //Control   
         { OpenGL_4_0, GL_TESS_EVALUATION_SHADER },    //Evaluation
         { OpenGL_3_3, GL_GEOMETRY_SHADER        },    //Geometry  
         { OpenGL_2_0, GL_FRAGMENT_SHADER        }     //Fragment
         };

      #endif
      #ifdef EN_OPENGL_MOBILE

      const PipelineStageTranslation PipelineStage[PipelineStagesCount] = 
         {
         { OpenGL_ES_2_0,         GL_VERTEX_SHADER                },    //Vertex    
         { OpenGL_ES_Unsupported, 0 /*GL_TESS_CONTROL_SHADER*/    },    //Control   
         { OpenGL_ES_Unsupported, 0 /*GL_TESS_EVALUATION_SHADER*/ },    //Evaluation
         { OpenGL_ES_Unsupported, 0 /*GL_GEOMETRY_SHADER*/        },    //Geometry  
         { OpenGL_ES_2_0,         GL_FRAGMENT_SHADER              }     //Fragment
         };

      #endif
      #ifdef EN_DEBUG

      const char* PipelineStageName[PipelineStagesCount] = 
         {
         "Vertex",   
         "Control",   
         "Evaluation",
         "Geometry",
         "Fragment" 
         };

      #endif
      }

      namespace gl20
      {
      void ProgramsInit(void)
      {
      // Init information about currently supported pipeline stages
      for(uint16 i=0; i<PipelineStagesCount; ++i)
         {
         gl::PipelineStageSupported[i] = false;
         if (GpuContext.screen.api.release >= gl::PipelineStage[i].opengl.release)
            gl::PipelineStageSupported[i] = true;
         }
      }

      void ProgramsClear(void)
      {
      // Clear information 
      for(uint16 i=0; i<PipelineStagesCount; ++i)
         gl::PipelineStageSupported[i] = false;
      }

      Program ProgramCreate(const vector<Shader>& shaders)
      {   
      // Check if there are any shaders passed
      if (shaders.size() == 0)
         return Program(NULL);
      
      // Check if passed shaders exists
      for(uint32 i=0; i<shaders.size(); ++i)
         if (!shaders[i])
            return Program(NULL);     
      
      // Try to allocate program descriptor 
      ProgramDescriptor* program = GpuContext.programs.allocate();
      if (!program)
         return Program(NULL);
      
      // Create program in driver
      Profile( program->id = glCreateProgram() );
      
      // Attach shaders to program
      ShaderDescriptor* shader;
      for(uint32 i=0; i<shaders.size(); ++i)
         {
         // Extracts ShaderDescriptor from its interface
         shader = *(ShaderDescriptor**)&shaders[i];
      
         Profile( glAttachShader(program->id, shader->id) );
         //if (CheckError("glAttachShader"))
         //   {
         //   Log << "Error! Cannot attach shader to program!\n"; 
         //   glDeleteProgram(program->id);
         //   GpuContext.programs.free(program);
         //   return Program(NULL);
         //   }
         }
      
      // Compile program
      sint32 ret = 0;
      Profile( glLinkProgram(program->id) );
      Profile( glGetProgramiv(program->id, GL_LINK_STATUS, (GLint*)&ret) );
      if (ret == GL_FALSE)
         {
         Log << "Error! Program linking failed!\n";
      
         // Obtaining compilation log
         sint32 length = 0;
         Profile( glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, (GLint*)&length) );
         if (length > 1)
            {
            char* buffer = new char[length];
            if (buffer)
               {
               // Compilation log
               Profile( glGetProgramInfoLog(program->id, length, NULL, buffer) );
               string info = string("Program compilation log:\n");
               info.append(buffer);
               info.append("\n");
      
               Log << info; 
               delete [] buffer;
               }
            }
      
         Profile( glDeleteProgram(program->id) );
         GpuContext.programs.free(program);
         return Program(NULL);
         }
      
      // Allocate temporary buffer for parameter names
      sint32 maxAttribLength = 0;
      sint32 maxUniformLength = 0;
      sint32 maxUniformBlockLength = 0;
      sint32 nameMaxLength = 0;
      sint32 nameLength = 0;
      char*  name = NULL;
      Profile( glGetProgramiv(program->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, (GLint*)&maxAttribLength) )
      Profile( glGetProgramiv(program->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, (GLint*)&maxUniformLength) )
      Profile( glGetProgramiv(program->id, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, (GLint*)&maxUniformBlockLength) )
      nameMaxLength = max(max(maxAttribLength, maxUniformLength), maxUniformBlockLength); //maxAttribLength > maxUniformLength ? maxAttribLength : maxUniformLength;
      name = new char[nameMaxLength];
      
      // Count number of inputs, parameters, blocks and samplers in program
      uint32 total    = 0;
      sint32 elements = 0;
      uint32 type     = 0;
      Profile( glGetProgramiv(program->id, GL_ACTIVE_ATTRIBUTES, (GLint*)&program->inputs.count) )
      Profile( glGetProgramiv(program->id, GL_ACTIVE_UNIFORMS, (GLint*)&total) )
      Profile( glGetProgramiv(program->id, GL_ACTIVE_UNIFORM_BLOCKS, (GLint*)&program->blocks.count) )

      program->samplers = 0;
      for(uint32 i=0; i<total; ++i)
         {
         Profile( glGetActiveUniform(program->id, i, nameMaxLength, (GLsizei*)&nameLength, (GLint*)&elements, (GLenum*)&type, name) );
         if (ProgramParameterIsSampler(type))
            program->samplers++;
         else
            program->parameters.count++;
         }

      // Prepare program descriptor for inputs, parameters, blocks and samplers
      program->inputs.list       = new InputDescriptor[program->inputs.count];
      program->sampler           = new TextureSamplerSet[program->samplers];
      //program->samplers.update   = new SamplerDescriptor*[program->samplers.count];
      //program->samplers.dirty    = program->samplers.count;
      program->parameters.list   = new ParameterDescriptor[program->parameters.count];
      program->parameters.update = new ParameterDescriptor*[program->parameters.count];
      program->parameters.dirty  = program->parameters.count;
      program->blocks.list       = new BlockDescriptor[program->blocks.count];
      
      // Describe inputs
      InputDescriptor* input;
      for(uint32 i=0; i<program->inputs.count; ++i)
         {
         Profile( glGetActiveAttrib(program->id, i, nameMaxLength, (GLsizei*)&nameLength, (GLint*)&elements, (GLenum*)&type, name) );
         name[nameLength] = 0;
      
         // Input parameters can be returned out of order.
         // They can also have location index higher than
         // their total count.   
         input = &program->inputs.list[i];
         input->program  = program;
         input->elements = elements;
         input->name     = new char[nameLength+1];
         memcpy(input->name, name, nameLength+1);
         Profile( input->location = glGetAttribLocation(program->id, name) );
         input->type     = type;
      
         // Assing input to it's name 
         program->inputs.names[string(name, nameLength)] = input;
         }
      
      // Describe parameters and samplers
      //SamplerDescriptor* sampler;
      ParameterDescriptor* parameter;
      uint32 parametersSet = 0;
      uint32 samplersSet = 0;
      for(uint32 i=0; i<total; ++i)
         {
         Profile( glGetActiveUniform(program->id, i, nameMaxLength, (GLsizei*)&nameLength, (GLint*)&elements, (GLenum*)&type, name) );
         name[nameLength] = 0;
         if (ProgramParameterIsSampler(type))
            {
            // Describe sampler
            TextureSamplerSet& sampler = program->sampler[samplersSet];
            Profile( sampler.location = glGetUniformLocation(program->id, name) );
            sampler.name = new char[nameLength+1];
            memcpy(sampler.name, name, nameLength+1);

            //sampler = &program->samplers.list[samplersSet];
            //sampler->program  = program;
            //sampler->unit     = new SamplerUnitDescriptor();
            //sampler->name     = new char[nameLength+1];
            //memcpy(sampler->name, name, nameLength+1);
            //sampler->elements = elements;
            //Profile( sampler->location = glGetUniformLocation(program->id, name) );
            //sampler->type     = type;
      
            // Assing sampler to it's name and mark as initially dirty
            //program->samplers.names[string(name, nameLength)] = sampler;
            //program->samplers.update[samplersSet] = sampler;
            samplersSet++;
            }
         else
            {
            // Describe parameter
            parameter = &program->parameters.list[parametersSet];
            parameter->program  = program;
            parameter->value    = new uint8[ProgramParameterSize(type) * elements];
            parameter->name     = new char[nameLength+1];
            memcpy(parameter->name, name, nameLength+1);
            parameter->elements = elements;
            Profile( parameter->location = glGetUniformLocation(program->id, name) );
            parameter->type     = type;
      
            // Assing parameter to it's name and mark as initially dirty
            program->parameters.names[string(name, nameLength)] = parameter;
            program->parameters.update[parametersSet] = parameter;
            parametersSet++;
            }
         }
    
      // Describe uniform blocks
      BlockDescriptor* block;
      for(uint32 i=0; i<program->blocks.count; ++i)
         {
         Profile( glGetActiveUniformBlockName(program->id, i, nameMaxLength, (GLint*)&nameLength, name) )
         name[nameLength] = 0;

         // Describe block
         block = &program->blocks.list[i];
         block->program = program;
         block->name    = new char[nameLength+1];
         memcpy(block->name, name, nameLength+1);
         Profile( block->id = glGetUniformBlockIndex(program->id, name) )
         Profile( glGetActiveUniformBlockiv( program->id, block->id, GL_UNIFORM_BLOCK_DATA_SIZE, (GLint*)&block->size ) )

         // Assing block to it's name 
         program->blocks.names[string(name, nameLength)] = block;
         }
   
      // Clean temp data
      delete [] name;
      
      // Return program interface
      return Program(program);
      }

      // Draws geometry from input buffer using selected program
      bool ProgramDraw(ProgramDescriptor* program, 
         const Ptr<Buffer> buffer,
         const Ptr<Buffer> indexBuffer,
         const DrawableType type, 
         const uint32 patchSize, 
         const uint32 inst )
      {
      assert(program);

      // Programmable Vertex Fetch is supported since OpenGL 3.0 through VAO
      if (!buffer && !GpuContext.screen.support(OpenGL_3_0))
         return false;

      // Check if drawable type is supported in current context
      if (GpuContext.screen.api.release < gl::Drawable[type].opengl.release)
         return false;
      
      // Update driver state
#ifndef GPU_IMMEDIATE_MODE
      for(uint32 i=0; i<GpuContext.state.dirty; ++i)
         {
         GpuContext.state.update[i]();
         GpuContext.state.update[i] = NULL;  // For debug purposes
         }
      GpuContext.state.dirty = 0;
#endif

      // Assign program to use
      Profile( glUseProgram(program->id) );
      
      // Update program parameters
      for(uint16 i=0; i<program->parameters.dirty; ++i)
         {
         ParameterDescriptor* parameter = program->parameters.update[i];
         // Uniforms inside Uniform Blocks are marked as
         // active, but don't receive location. 
         if (parameter->location != -1)
            ProgramParameterUpdate(parameter);
         parameter->dirty = false;
         }
      program->parameters.dirty = 0;
      
      // Update program blocks bindings
      for(uint16 i=0; i<program->blocks.count; ++i)
         {
         BlockDescriptor&  block  = program->blocks.list[i];
         assert(block.buffer);
         Ptr<BufferGL> buffer = ptr_dynamic_cast<BufferGL, Buffer>(block.buffer);

         // In future we should minimise amount of UBO/SSBO bindings
         // by reusing their previously bound locations from range
         // [0 . . GL_MAX_UNIFORM_BUFFER_BINDINGS] and
         // [0 . . GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS]
         if (buffer->type() == BufferType::Uniform)
            {
            GLuint bindingId = block.id; 
            glBindBufferBase( GL_UNIFORM_BUFFER, bindingId, buffer->handle );
            glUniformBlockBinding(program->id, block.id, bindingId);
            }
#if !defined(EN_PLATFORM_OSX)
         if (buffer->type() == BufferType::Storage)
            {
            GLuint bindingId = block.id; 
            glBindBufferBase( GL_SHADER_STORAGE_BUFFER, bindingId, buffer->handle );
            glUniformBlockBinding(program->id, block.id, bindingId);
            }
#endif
         }

      //// At first program draw bind sampler parameters to Texture Image Units
      //// in order they were reported by driver.  
      //for(uint16 i=0; i<program->samplers.dirty; ++i)
      //   {
      //   SamplerDescriptor* sampler = program->samplers.update[i];
      //   Profile( glUniform1i(program->samplers.update[i]->location, i) );
      //   sampler->dirty = false;
      //   }
      //program->samplers.dirty = 0;
      



      // Bind textures and samplers 
      //----------------------------

      // Increase age of all texture units that are currently used. Age of
      // units that will be reused by this program, will be restored. This
      // way, only unused units age.
      for(uint16 slot=0; slot<GpuContext.support.maxTextureUnits; ++slot)
         ++GpuContext.textureUnit.metric[slot];   

      // For each program sampler parameter, bind required texture-sampler
      // pair to one of texture units for rendering. 
      for(uint16 i=0; i<program->samplers; ++i)
         {
         TextureSamplerSet& sampler = program->sampler[i];

         // Check if used texture-sampler pair is still bound
         uint16 slot = sampler.slot;
         if (GpuContext.textureUnit.texture[slot] == sampler.texture &&
             GpuContext.textureUnit.sampler[slot] == sampler.sampler)
            {
            // Decrease age of used texture unit
            --GpuContext.textureUnit.metric[slot];
            continue;
            }

         // Check if required texture-sampler pair is not bound to other texture unit.
         // At the same time find texture-sampler pair that is oldest (wasn't used for 
         // rendering for a longest period of time in comparison to other pairs).
         bool found = false;
         uint16 id = 0;
         uint32 oldestAge = 0;
         uint32 oldestId  = 0;
         for(uint16 slot=0; slot<GpuContext.support.maxTextureUnits; ++slot)
            {
            if (GpuContext.textureUnit.texture[slot] == sampler.texture &&
                GpuContext.textureUnit.sampler[slot] == sampler.sampler)
               {
               found = true;
               id    = slot;
               }

            if (GpuContext.textureUnit.metric[slot] > oldestAge)
               {
               oldestAge = GpuContext.textureUnit.metric[slot];
               oldestId  = slot;
               }
            }

         // If texture-sampler pair is bound to other texture unit, update 
         // sampler parameter binding with new texture unit index.
         if (found)
            {
            sampler.slot = id;
            Profile( glUniform1i(sampler.location, sampler.slot) )

            // Decrease age of used texture unit
            --GpuContext.textureUnit.metric[sampler.slot];
            continue;
            }

         // Texture-sampler pair needed by sampler parameter is no longer bound
         // to any texture unit. Bind it to oldest used texture unit slot.
         sampler.slot = oldestId;
         if (GpuContext.textureUnit.texture[oldestId] != sampler.texture)
            {
            Ptr<TextureGL> textureGL = ptr_dynamic_cast<TextureGL, Texture>(sampler.texture);
            uint16 glType = TranslateTextureType[underlyingType(textureGL->type())];

            Profile( glActiveTexture(GL_TEXTURE0 + oldestId) )
            Profile( glBindTexture(glType, textureGL->id ) )
            }
         if (GpuContext.textureUnit.sampler[oldestId] != sampler.sampler)
            {
            Ptr<SamplerGL33> samplerGL = ptr_dynamic_cast<SamplerGL33, Sampler>(sampler.sampler);

            Profile( glBindSampler( oldestId, samplerGL->id ) )
            }

         Profile( glUniform1i(sampler.location, sampler.slot) ) 

         // Decrease age of used texture unit
         --GpuContext.textureUnit.metric[sampler.slot];
         }






// glGetUniformLocation



            // it texture != new texture -> rebind it
            // if sampler != new sampler -> rebind it
            // 





// N - amount of Texture Units
// Trate this like cache of Textures.
// Each program, for each texture, searches it in cache:
// - if not found, rebind last used texture in cache with needed one
// - attach sampler to it
//
//
// [0 .. GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS]
// glBindSampler( textureUnit, samplerID );
//
// ? What if several different samplers wants to use the same texture ?
// Need also some kind of sampler cache, or maybe it should be "texture-sampler" pair cache?
// Program Sampler Parameter needs : sampler and texture <- so "texture-sampler" pair
// thus it should be such a cache:
// N - slot sized cache of "texture-sampler" cache
// inf - infinite size of "sampler" state's for reuse


//      for(uint16 i=0; i<program->samplers.count; ++i)
//         {
//         SamplerUnitDescriptor* samplerUnit = program->samplers.list[i].unit;
//      
//         // Check if sampler has binded texture
//         if (!samplerUnit->texture)
//            continue;
//      
//         TextureDescriptor* textureDescriptor = *(TextureDescriptor**)&samplerUnit->texture;
//         uint16 type = gl::TextureType[textureDescriptor->type].type;
//         Profile( glActiveTexture(GL_TEXTURE0 + i) );
//         Profile( glBindTexture(type, textureDescriptor->id) );
//#ifdef EN_OPENGL_DESKTOP
//         SamplerUnitUpdate(samplerUnit);
//#endif
//#ifdef EN_OPENGL_MOBILE
//         en::gpu::es20::SamplerUnitUpdate(samplerUnit);
//#endif
//         }
      
      // It is possible to draw without any input buffer.
      // In such situation instances count specifies 
      // amount of Vertex Shader invocations.
      if (!buffer)
         Profile( glBindVertexArray(GpuContext.emptyVAO) )
      else
         {
         Ptr<BufferGL> glBuffer = ptr_dynamic_cast<BufferGL, Buffer>(buffer);

         // For each active program input, search proper buffer
         // column by name or by location. If column cannot be
         // found, try to use default value specified for
         // input parameter name or return error (TODO).
         if (GpuContext.screen.support(OpenGL_3_0))
            Profile( glBindVertexArray(glBuffer->vao) )
         Profile( glBindBuffer(GL_ARRAY_BUFFER, glBuffer->handle) )
         uint64 offset = 0;
         for(uint8 i=0; i<program->inputs.count; ++i)
            {
            InputDescriptor& input = program->inputs.list[i];
         
//            // Find buffer column that match by name input parameter
//            if (buffer->named)
//               {
//               bool found = false;
//               for(uint8 j=0; j<buffer->columns; ++j)
//                  if (strcmp(buffer->column[j].name, input.name) == 0)
//                     {
//                     Attribute format = buffer->column[j].type;
//                     uint16 type       = gl::TranslateAttribute[underlyingType(format)].type;
//                     //uint8  size       = gl::BufferColumn[columnType].size;
//                     uint16 channels   = gl::TranslateAttribute[underlyingType(format)].channels;
//                     bool   normalized = gl::TranslateAttribute[underlyingType(format)].normalized;
//                     
//                     Profile( glEnableVertexAttribArray( input.location ) );
//                     if (gl::TranslateAttribute[underlyingType(format)].qword)
//                        {
//                        Profile( glVertexAttribLPointer(input.location, channels, type, buffer->rowSize, reinterpret_cast<const GLvoid*>(buffer->offset[j]) ) );
//                        }
//                     else
//                     if (gl::TranslateAttribute[underlyingType(format)].integer)
//                        {
//                        Profile( glVertexAttribIPointer(input.location, channels, type, buffer->rowSize, reinterpret_cast<const GLvoid*>(buffer->offset[j]) ) );
//                        }
//                     else
//                        {
//                        Profile( glVertexAttribPointer(input.location, channels, type, normalized, buffer->rowSize, reinterpret_cast<const GLvoid*>(buffer->offset[j]) ) );
//                        }
//         
//                     found = true;
//                     break;
//                     }
//         
//            //    if (!found)
//            //       log << "Error! Can't find column matching programs input attribute named: " << input.name << endl; 
//               }
//            // Match columns by locations
//            else
               {
               Attribute format = glBuffer->formatting.column[input.location];
               uint32 elementSize = glBuffer->formatting.elementSize();
               uint16 type       = TranslateAttribute[underlyingType(format)].type;
               uint16 size       = TranslateAttribute[underlyingType(format)].size;
               uint16 channels   = TranslateAttribute[underlyingType(format)].channels;
               bool   normalized = TranslateAttribute[underlyingType(format)].normalized;
               
               Profile( glEnableVertexAttribArray( input.location ) );
               if (TranslateAttribute[underlyingType(format)].qword)
                  {
                  Profile( glVertexAttribLPointer(input.location, channels, type, elementSize, reinterpret_cast<const GLvoid*>(offset) ) );
                  }
               else
               if (TranslateAttribute[underlyingType(format)].integer)
                  {
                  Profile( glVertexAttribIPointer(input.location, channels, type, elementSize, reinterpret_cast<const GLvoid*>(offset) ) );
                  }
               else
                  {
                  Profile( glVertexAttribPointer(input.location, channels, type, normalized, elementSize, reinterpret_cast<const GLvoid*>(offset) ) );
                  }
         
               offset += size;
               }
            }
         }

      // Tessellation of Patches
      if (type == Patches)
         {
         assert(patchSize <= GpuContext.support.maxPatchSize);
         glPatchParameteri(GL_PATCH_VERTICES, patchSize);
         }

      // Draw Call
      if (GpuContext.screen.support(OpenGL_3_1) &&
          inst > 1)
         {
         // Draw Instanced
         if (indexBuffer &&
             indexBuffer->type() == BufferType::Index)
            {
            Ptr<BufferGL> glIndex = ptr_dynamic_cast<BufferGL, Buffer>(indexBuffer);
            Profile( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndex->handle) )
            Profile( glDrawElementsInstanced(gl::Drawable[type].type, glIndex->elements, TranslateAttribute[underlyingType(glIndex->formatting.column[0])].type, nullptr, inst) )
            }
         else
         if (buffer &&
             buffer->type() == BufferType::Vertex)
            {
            Ptr<BufferGL> glBuffer = ptr_dynamic_cast<BufferGL, Buffer>(buffer);
            Profile( glDrawArraysInstanced(gl::Drawable[type].type, 0, glBuffer->elements, inst) )
            }
         else
            Profile( glDrawArrays(gl::Drawable[type].type, 0, inst) )
         }
      else
         {
         // Draw specified data. There is no instancing
         // support in OpenGL 2.0 so draw it only once.
         if (indexBuffer &&
             indexBuffer->type() == BufferType::Index)
            {
            Ptr<BufferGL> glIndex = ptr_dynamic_cast<BufferGL, Buffer>(indexBuffer);
            Profile( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndex->handle) )
            Profile( glDrawElements(gl::Drawable[type].type, glIndex->elements, TranslateAttribute[underlyingType(glIndex->formatting.column[0])].type, nullptr) )
            }
         else
         if (buffer &&
             buffer->type() == BufferType::Vertex)
            {
            Ptr<BufferGL> glBuffer = ptr_dynamic_cast<BufferGL, Buffer>(buffer);
            
            Profile( glDrawArrays(gl::Drawable[type].type, 0, glBuffer->elements) )
            }
         else
            Profile( glDrawArrays(gl::Drawable[type].type, 0, inst) )
         }
     
      // Clean after drawing
      Profile( glBindBuffer(GL_ARRAY_BUFFER, 0) )
      Profile( glBindVertexArray(0) )
      return true;
      } 

      #ifdef EN_OPENGL_DESKTOP

      // Returns size in bytes of specified GLSL program parameter
      uint32 ProgramParameterSize(uint16 type)
      {
      // OpenGL 2.0 uniform types
      if ( type == GL_FLOAT             ) return 4;   
      if ( type == GL_FLOAT_VEC2        ) return 8;   
      if ( type == GL_FLOAT_VEC3        ) return 12;  
      if ( type == GL_FLOAT_VEC4        ) return 16;
      if ( type == GL_INT               ) return 4;   
      if ( type == GL_INT_VEC2          ) return 8;  
      if ( type == GL_INT_VEC3          ) return 12;  
      if ( type == GL_INT_VEC4          ) return 16; 
      if ( type == GL_BOOL              ) return 1;   
      if ( type == GL_BOOL_VEC2         ) return 2;  
      if ( type == GL_BOOL_VEC3         ) return 3;  
      if ( type == GL_BOOL_VEC4         ) return 4;  
      if ( type == GL_FLOAT_MAT2        ) return 16;  
      if ( type == GL_FLOAT_MAT3        ) return 36; 
      if ( type == GL_FLOAT_MAT4        ) return 64; 
      if ( type == GL_SAMPLER_1D        ) return 4;
      if ( type == GL_SAMPLER_2D        ) return 4;
      if ( type == GL_SAMPLER_3D        ) return 4;
      if ( type == GL_SAMPLER_CUBE      ) return 4;
      if ( type == GL_SAMPLER_1D_SHADOW ) return 4;
      if ( type == GL_SAMPLER_2D_SHADOW ) return 4;
     
      // OpenGL 2.1 uniform types
      if ( type == GL_FLOAT_MAT2x3 ) return 24;  
      if ( type == GL_FLOAT_MAT2x4 ) return 32;  
      if ( type == GL_FLOAT_MAT3x2 ) return 24;  
      if ( type == GL_FLOAT_MAT3x4 ) return 48;  
      if ( type == GL_FLOAT_MAT4x2 ) return 32;  
      if ( type == GL_FLOAT_MAT4x3 ) return 48;  
     
      // OpenGL 3.0 uniform types
      if ( type == GL_UNSIGNED_INT                  ) return 4;   
      if ( type == GL_UNSIGNED_INT_VEC2             ) return 8;   
      if ( type == GL_UNSIGNED_INT_VEC3             ) return 12;  
      if ( type == GL_UNSIGNED_INT_VEC4             ) return 16;  
      if ( type == GL_SAMPLER_1D_ARRAY              ) return 4;   
      if ( type == GL_SAMPLER_2D_ARRAY              ) return 4;   
      if ( type == GL_SAMPLER_CUBE_SHADOW           ) return 4;   
      if ( type == GL_SAMPLER_1D_ARRAY_SHADOW       ) return 4;   
      if ( type == GL_SAMPLER_2D_ARRAY_SHADOW       ) return 4;   
      if ( type == GL_INT_SAMPLER_1D                ) return 4;   
      if ( type == GL_INT_SAMPLER_2D                ) return 4;   
      if ( type == GL_INT_SAMPLER_3D                ) return 4;   
      if ( type == GL_INT_SAMPLER_CUBE              ) return 4;   
      if ( type == GL_INT_SAMPLER_1D_ARRAY          ) return 4;   
      if ( type == GL_INT_SAMPLER_2D_ARRAY          ) return 4;   
      if ( type == GL_UNSIGNED_INT_SAMPLER_1D       ) return 4;   
      if ( type == GL_UNSIGNED_INT_SAMPLER_2D       ) return 4;   
      if ( type == GL_UNSIGNED_INT_SAMPLER_3D       ) return 4;   
      if ( type == GL_UNSIGNED_INT_SAMPLER_CUBE     ) return 4;   
      if ( type == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY ) return 4;   
      if ( type == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY ) return 4;   
     
      // OpenGL 3.1 uniform types
      if ( type == GL_SAMPLER_BUFFER                ) return 4;
      if ( type == GL_SAMPLER_2D_RECT               ) return 4;
      if ( type == GL_SAMPLER_2D_RECT_SHADOW        ) return 4;
      if ( type == GL_INT_SAMPLER_BUFFER            ) return 4;
      if ( type == GL_INT_SAMPLER_2D_RECT           ) return 4;
      if ( type == GL_UNSIGNED_INT_SAMPLER_BUFFER   ) return 4;
      if ( type == GL_UNSIGNED_INT_SAMPLER_2D_RECT  ) return 4;
     
      // OpenGL 3.2 uniform types
      if ( type == GL_SAMPLER_2D_MULTISAMPLE                    ) return 4;
      if ( type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY              ) return 4;
      if ( type == GL_INT_SAMPLER_2D_MULTISAMPLE                ) return 4;
      if ( type == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY          ) return 4;
      if ( type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE       ) return 4;
      if ( type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ) return 4;
     
      // OpenGL 4.0 uniform types
      if ( type == GL_DOUBLE        ) return 8;   
      if ( type == GL_DOUBLE_VEC2   ) return 16;  
      if ( type == GL_DOUBLE_VEC3   ) return 24;  
      if ( type == GL_DOUBLE_VEC4   ) return 32;  
      if ( type == GL_DOUBLE_MAT2   ) return 32; 
      if ( type == GL_DOUBLE_MAT3   ) return 72; 
      if ( type == GL_DOUBLE_MAT4   ) return 128;
      if ( type == GL_DOUBLE_MAT2x3 ) return 48;
      if ( type == GL_DOUBLE_MAT2x4 ) return 64;
      if ( type == GL_DOUBLE_MAT3x2 ) return 48;
      if ( type == GL_DOUBLE_MAT3x4 ) return 96;
      if ( type == GL_DOUBLE_MAT4x2 ) return 64;
      if ( type == GL_DOUBLE_MAT4x3 ) return 96;
     
      // OpenGL 4.2 uniform types
#if !defined(EN_PLATFORM_OSX)
      if ( type == GL_IMAGE_1D                                ) return 4;
      if ( type == GL_IMAGE_2D                                ) return 4;
      if ( type == GL_IMAGE_3D                                ) return 4;
      if ( type == GL_IMAGE_2D_RECT                           ) return 4;
      if ( type == GL_IMAGE_CUBE                              ) return 4;
      if ( type == GL_IMAGE_BUFFER                            ) return 4;
      if ( type == GL_IMAGE_1D_ARRAY                          ) return 4;
      if ( type == GL_IMAGE_2D_ARRAY                          ) return 4;
      if ( type == GL_IMAGE_2D_MULTISAMPLE                    ) return 4;
      if ( type == GL_IMAGE_2D_MULTISAMPLE_ARRAY              ) return 4;
      if ( type == GL_INT_IMAGE_1D                            ) return 4;
      if ( type == GL_INT_IMAGE_2D                            ) return 4;
      if ( type == GL_INT_IMAGE_3D                            ) return 4;
      if ( type == GL_INT_IMAGE_2D_RECT                       ) return 4;
      if ( type == GL_INT_IMAGE_CUBE                          ) return 4;
      if ( type == GL_INT_IMAGE_BUFFER                        ) return 4;
      if ( type == GL_INT_IMAGE_1D_ARRAY                      ) return 4;
      if ( type == GL_INT_IMAGE_2D_ARRAY                      ) return 4;
      if ( type == GL_INT_IMAGE_2D_MULTISAMPLE                ) return 4;
      if ( type == GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY          ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_1D                   ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_2D                   ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_3D                   ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_2D_RECT              ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_CUBE                 ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_BUFFER               ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_1D_ARRAY             ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_2D_ARRAY             ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE       ) return 4;
      if ( type == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY ) return 4;
      if ( type == GL_UNSIGNED_INT_ATOMIC_COUNTER             ) return 4;
#endif

      // Unsupported parameter type
      return 0;
      }

      bool ProgramParameterIsSampler(uint16 type)
      {
           // OpenGL 2.0 uniform types
      if (    type == GL_SAMPLER_1D        
           || type == GL_SAMPLER_2D        
           || type == GL_SAMPLER_3D        
           || type == GL_SAMPLER_CUBE      
           || type == GL_SAMPLER_1D_SHADOW 
           || type == GL_SAMPLER_2D_SHADOW 
           // OpenGL 3.0 uniform types
           || type == GL_SAMPLER_1D_ARRAY                
           || type == GL_SAMPLER_2D_ARRAY                
           || type == GL_SAMPLER_CUBE_SHADOW             
           || type == GL_SAMPLER_1D_ARRAY_SHADOW         
           || type == GL_SAMPLER_2D_ARRAY_SHADOW         
           || type == GL_INT_SAMPLER_1D                   
           || type == GL_INT_SAMPLER_2D                   
           || type == GL_INT_SAMPLER_3D                   
           || type == GL_INT_SAMPLER_CUBE                 
           || type == GL_INT_SAMPLER_1D_ARRAY             
           || type == GL_INT_SAMPLER_2D_ARRAY             
           || type == GL_UNSIGNED_INT_SAMPLER_1D          
           || type == GL_UNSIGNED_INT_SAMPLER_2D          
           || type == GL_UNSIGNED_INT_SAMPLER_3D          
           || type == GL_UNSIGNED_INT_SAMPLER_CUBE        
           || type == GL_UNSIGNED_INT_SAMPLER_1D_ARRAY    
           || type == GL_UNSIGNED_INT_SAMPLER_2D_ARRAY    
           // OpenGL 3.1 uniform types
           || type == GL_SAMPLER_BUFFER                
           || type == GL_SAMPLER_2D_RECT               
           || type == GL_SAMPLER_2D_RECT_SHADOW        
           || type == GL_INT_SAMPLER_BUFFER            
           || type == GL_INT_SAMPLER_2D_RECT           
           || type == GL_UNSIGNED_INT_SAMPLER_BUFFER   
           || type == GL_UNSIGNED_INT_SAMPLER_2D_RECT  
           // OpenGL 3.2 uniform types
           || type == GL_SAMPLER_2D_MULTISAMPLE                    
           || type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY              
           || type == GL_INT_SAMPLER_2D_MULTISAMPLE                
           || type == GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY          
           || type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE       
           || type == GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY 
           // OpenGL 4.0 uniform types
           || type == GL_SAMPLER_CUBE_MAP_ARRAY
           || type == GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
           || type == GL_INT_SAMPLER_CUBE_MAP_ARRAY
           || type == GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY
           // OpenGL 4.2 uniform types
#if !defined(EN_PLATFORM_OSX)
           || type == GL_IMAGE_1D                                
           || type == GL_IMAGE_2D                                
           || type == GL_IMAGE_3D                                
           || type == GL_IMAGE_2D_RECT                           
           || type == GL_IMAGE_CUBE                              
           || type == GL_IMAGE_BUFFER                            
           || type == GL_IMAGE_1D_ARRAY                          
           || type == GL_IMAGE_2D_ARRAY                          
           || type == GL_IMAGE_2D_MULTISAMPLE                    
           || type == GL_IMAGE_2D_MULTISAMPLE_ARRAY              
           || type == GL_INT_IMAGE_1D                            
           || type == GL_INT_IMAGE_2D                            
           || type == GL_INT_IMAGE_3D                            
           || type == GL_INT_IMAGE_2D_RECT                       
           || type == GL_INT_IMAGE_CUBE                          
           || type == GL_INT_IMAGE_BUFFER                        
           || type == GL_INT_IMAGE_1D_ARRAY                      
           || type == GL_INT_IMAGE_2D_ARRAY                      
           || type == GL_INT_IMAGE_2D_MULTISAMPLE                
           || type == GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY          
           || type == GL_UNSIGNED_INT_IMAGE_1D                   
           || type == GL_UNSIGNED_INT_IMAGE_2D                   
           || type == GL_UNSIGNED_INT_IMAGE_3D                   
           || type == GL_UNSIGNED_INT_IMAGE_2D_RECT              
           || type == GL_UNSIGNED_INT_IMAGE_CUBE                 
           || type == GL_UNSIGNED_INT_IMAGE_BUFFER               
           || type == GL_UNSIGNED_INT_IMAGE_1D_ARRAY             
           || type == GL_UNSIGNED_INT_IMAGE_2D_ARRAY             
           || type == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE       
           || type == GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY 
           || type == GL_UNSIGNED_INT_ATOMIC_COUNTER
#endif 
           )
         return true;
     
      // This parameter is not a sampler
      return false;
      }

      bool ProgramParameterUpdate(ParameterDescriptor* parameter)
      {
      // OpenGL 2.0 uniform types
      if ( parameter->type == GL_FLOAT             ) { Profile( glUniform1fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_VEC2        ) { Profile( glUniform2fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_VEC3        ) { Profile( glUniform3fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_VEC4        ) { Profile( glUniform4fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_INT               ) { Profile( glUniform1iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_INT_VEC2          ) { Profile( glUniform2iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_INT_VEC3          ) { Profile( glUniform3iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_INT_VEC4          ) { Profile( glUniform4iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_BOOL              ) { return false; }
      if ( parameter->type == GL_BOOL_VEC2         ) { return false; }
      if ( parameter->type == GL_BOOL_VEC3         ) { return false; }
      if ( parameter->type == GL_BOOL_VEC4         ) { return false; }
      if ( parameter->type == GL_FLOAT_MAT2        ) { Profile( glUniformMatrix2fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_MAT3        ) { Profile( glUniformMatrix3fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_MAT4        ) { Profile( glUniformMatrix4fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }
      // OpenGL 2.1 uniform types
      if ( parameter->type == GL_FLOAT_MAT2x3      ) { Profile( glUniformMatrix2x3fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }  
      if ( parameter->type == GL_FLOAT_MAT2x4      ) { Profile( glUniformMatrix2x4fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }  
      if ( parameter->type == GL_FLOAT_MAT3x2      ) { Profile( glUniformMatrix3x2fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }  
      if ( parameter->type == GL_FLOAT_MAT3x4      ) { Profile( glUniformMatrix3x4fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }  
      if ( parameter->type == GL_FLOAT_MAT4x2      ) { Profile( glUniformMatrix4x2fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }  
      if ( parameter->type == GL_FLOAT_MAT4x3      ) { Profile( glUniformMatrix4x3fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }  
      // OpenGL 3.0 uniform types
      if ( parameter->type == GL_UNSIGNED_INT      ) { Profile( glUniform1uiv(parameter->location, parameter->elements, (GLuint*)parameter->value) )  return true; }  
      if ( parameter->type == GL_UNSIGNED_INT_VEC2 ) { Profile( glUniform2uiv(parameter->location, parameter->elements, (GLuint*)parameter->value) )  return true; }  
      if ( parameter->type == GL_UNSIGNED_INT_VEC3 ) { Profile( glUniform3uiv(parameter->location, parameter->elements, (GLuint*)parameter->value) )  return true; }  
      if ( parameter->type == GL_UNSIGNED_INT_VEC4 ) { Profile( glUniform4uiv(parameter->location, parameter->elements, (GLuint*)parameter->value) )  return true; }
      // OpenGL 4.0 uniform types
      if ( parameter->type == GL_DOUBLE            ) { Profile( glUniform1dv(parameter->location, parameter->elements, (GLdouble*)parameter->value) ) return true; }  
      if ( parameter->type == GL_DOUBLE_VEC2       ) { Profile( glUniform2dv(parameter->location, parameter->elements, (GLdouble*)parameter->value) ) return true; }  
      if ( parameter->type == GL_DOUBLE_VEC3       ) { Profile( glUniform3dv(parameter->location, parameter->elements, (GLdouble*)parameter->value) ) return true; }  
      if ( parameter->type == GL_DOUBLE_VEC4       ) { Profile( glUniform4dv(parameter->location, parameter->elements, (GLdouble*)parameter->value) ) return true; }  
      if ( parameter->type == GL_DOUBLE_MAT2       ) { Profile( glUniformMatrix2dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT3       ) { Profile( glUniformMatrix3dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT4       ) { Profile( glUniformMatrix4dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT2x3     ) { Profile( glUniformMatrix2x3dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT2x4     ) { Profile( glUniformMatrix2x4dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT3x2     ) { Profile( glUniformMatrix3x2dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT3x4     ) { Profile( glUniformMatrix3x4dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT4x2     ) { Profile( glUniformMatrix4x2dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
      if ( parameter->type == GL_DOUBLE_MAT4x3     ) { Profile( glUniformMatrix4x3dv(parameter->location, parameter->elements, false, (GLdouble*)parameter->value) ) return true; }
     
      // Unsupported parameter type
      return false;
      }

      #endif
      #ifdef EN_OPENGL_MOBILE

      // Returns size in bytes of specified GLSL program parameter
      uint32 ProgramParameterSize(uint16 type)
      {
      // OpenGL ES 2.0 uniform types
      if ( type == GL_FLOAT             ) return 4;   
      if ( type == GL_FLOAT_VEC2        ) return 8;   
      if ( type == GL_FLOAT_VEC3        ) return 12;  
      if ( type == GL_FLOAT_VEC4        ) return 16;
      if ( type == GL_INT               ) return 4;   
      if ( type == GL_INT_VEC2          ) return 8;  
      if ( type == GL_INT_VEC3          ) return 12;  
      if ( type == GL_INT_VEC4          ) return 16; 
      if ( type == GL_BOOL              ) return 1;   
      if ( type == GL_BOOL_VEC2         ) return 2;  
      if ( type == GL_BOOL_VEC3         ) return 3;  
      if ( type == GL_BOOL_VEC4         ) return 4;  
      if ( type == GL_FLOAT_MAT2        ) return 16;  
      if ( type == GL_FLOAT_MAT3        ) return 36; 
      if ( type == GL_FLOAT_MAT4        ) return 64; 
      if ( type == GL_SAMPLER_2D        ) return 4;
      if ( type == GL_SAMPLER_CUBE      ) return 4;

      // Unsupported parameter type
      return 0;
      }

      bool ProgramParameterIsSampler(uint16 type)
      {
           // OpenGL ES 2.0 uniform types
      if (    type == GL_SAMPLER_2D           
           || type == GL_SAMPLER_CUBE )
         return true;
     
      // This parameter is not a sampler
      return false;
      }

      bool ProgramParameterUpdate(ParameterDescriptor* parameter)
      {
      // OpenGL ES 2.0 uniform types
      if ( parameter->type == GL_FLOAT             ) { Profile( glUniform1fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_VEC2        ) { Profile( glUniform2fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_VEC3        ) { Profile( glUniform3fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_VEC4        ) { Profile( glUniform4fv(parameter->location, parameter->elements, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_INT               ) { Profile( glUniform1iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_INT_VEC2          ) { Profile( glUniform2iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_INT_VEC3          ) { Profile( glUniform3iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_INT_VEC4          ) { Profile( glUniform4iv(parameter->location, parameter->elements, (GLint*)parameter->value) )   return true; }
      if ( parameter->type == GL_BOOL              ) { return false; }
      if ( parameter->type == GL_BOOL_VEC2         ) { return false; }
      if ( parameter->type == GL_BOOL_VEC3         ) { return false; }
      if ( parameter->type == GL_BOOL_VEC4         ) { return false; }
      if ( parameter->type == GL_FLOAT_MAT2        ) { Profile( glUniformMatrix2fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_MAT3        ) { Profile( glUniformMatrix3fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }
      if ( parameter->type == GL_FLOAT_MAT4        ) { Profile( glUniformMatrix4fv(parameter->location, parameter->elements, false, (GLfloat*)parameter->value) ) return true; }
  
      // Unsupported parameter type
      return false;
      }

      #endif

      bool ProgramDestroy(ProgramDescriptor* const program)
      {
      // Frees program in driver
      Profile( glDeleteProgram(program->id) );
      GpuContext.programs.free(program);
      return true; 
      }

      Shader ShaderCreate(const PipelineStage stage, const string& code)
      {  
      // Check if pipeline stage is supported in current Context
      if (!gl::PipelineStageSupported[stage])
         return Shader(nullptr);

      // Try to allocate shader descriptor 
      ShaderDescriptor* shader = GpuContext.shaders.allocate();
      if (!shader) 
         return Shader(nullptr);

      // Fill internal shader descriptor with data
      shader->stage = stage;
     
      // Perform shader compilation
      const char* pointer = code.c_str();
      shader->id = glCreateShader(gl::PipelineStage[stage].type);
      Profile( glShaderSource(shader->id, 1, (const GLchar**)&pointer, nullptr) );
      Profile( glCompileShader(shader->id) ); 

      //Check compilation status
      sint32 ret = 0;
      Profile( glGetShaderiv(shader->id, GL_COMPILE_STATUS, (GLint*)&ret) );
      if (ret == GL_FALSE)
         {
         // Error message
         string info = string("Error! Shader compilation failed!\n");
         Log << info;
         Log << code;

         // Obtaining compilation log
         sint32 length = 0;
         Profile( glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, (GLint*)&length) );
         if (length > 1)
            {
            char* buffer = new char[length];
            if (buffer)
               {
               // Compilation log
               Profile( glGetShaderInfoLog(shader->id, length, nullptr, buffer) );
               info = string("Shader compilation log:\n");
               info.append(buffer);
               info.append("\n");
               Log << info; 
               delete [] buffer;
               }
            }
     
         Profile( glDeleteShader(shader->id) );
         GpuContext.shaders.free(shader);
         return Shader(nullptr);
         }
  
      // Return shader interface
      return Shader(shader);
      }

      Shader ShaderCreateComposed(const PipelineStage stage, const vector<string>& code)
      {  
      // Check if pipeline stage is supported in current Context
      if (!gl::PipelineStageSupported[stage])
         return Shader(NULL);

      // Try to allocate shader descriptor 
      ShaderDescriptor* shader = GpuContext.shaders.allocate();
      if (!shader) 
         return Shader(NULL);

      // Fill internal shader descriptor with data
      shader->stage = stage;
     
      // Create array of shader code strings 
      char** pointer = new char*[code.size()];
      for(uint8 i=0; i<code.size(); ++i)
         pointer[i] = (char*)code[i].c_str();

      // Perform shader compilation
      shader->id = glCreateShader(gl::PipelineStage[stage].type);
      Profile( glShaderSource(shader->id, static_cast<sint32>(code.size()), (const GLchar**)pointer, NULL) );
      Profile( glCompileShader(shader->id) ); 
      delete [] pointer;

      //Check compilation status
      sint32 ret = 0;
      Profile( glGetShaderiv(shader->id, GL_COMPILE_STATUS, (GLint*)&ret) );
      if (ret == GL_FALSE)
         {
         // Error message
         string info = string("Error! Shader compilation failed!\n");
         Log << info;
         for(uint8 i=0; i<code.size(); ++i)
            {
            Log << "Code section #" << i << endl;
            Log << code[i];
            }

         // Obtaining compilation log
         sint32 length = 0;
         Profile( glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, (GLint*)&length) );
         if (length > 1)
            {
            char* buffer = new char[length];
            if (buffer)
               {
               // Compilation log
               Profile( glGetShaderInfoLog(shader->id, length, NULL, buffer) );
               info = string("Shader compilation log:\n");
               info.append(buffer);
               info.append("\n");
               Log << info; 
               delete [] buffer;
               }
            }
     
         Profile( glDeleteShader(shader->id) );
         GpuContext.shaders.free(shader);
         return Shader(NULL);
         }
  
      // Return shader interface
      return Shader(shader);
      }

      bool ShaderDestroy(ShaderDescriptor* const shader)
      {  
      // Frees shader in driver
      Profile( glDeleteShader(shader->id) )
      GpuContext.shaders.free(shader);
      return true; 
      }

      bool ParameterDestroy(ParameterDescriptor* const parameter)
      {
      // Parameters are destroyed together with program.
      return true;
      }

      bool BlockDestroy(BlockDescriptor* const block)
      {
      // Blocks are destroyed together with program.
      return true;
      }

      }
   }
}

template<> bool (*ProxyInterface<en::gpu::ShaderDescriptor>::destroy)(en::gpu::ShaderDescriptor* const)       = en::gpu::gl20::ShaderDestroy;
template<> bool (*ProxyInterface<en::gpu::ProgramDescriptor>::destroy)(en::gpu::ProgramDescriptor* const)     = en::gpu::gl20::ProgramDestroy;
template<> bool (*ProxyInterface<en::gpu::ParameterDescriptor>::destroy)(en::gpu::ParameterDescriptor* const) = en::gpu::gl20::ParameterDestroy;
template<> bool (*ProxyInterface<en::gpu::BlockDescriptor>::destroy)(en::gpu::BlockDescriptor* const)         = en::gpu::gl20::BlockDestroy;


 
