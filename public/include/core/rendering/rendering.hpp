/*

 Ngine v5.0
 
 Module      : Rendering.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING
#define ENG_CORE_RENDERING

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_OPENGL)

#include <string>
#include <vector>
using namespace std;

#include "core/types.h"
#include "utilities/Nversion.h"

#include "core/rendering/state.h"
#include "core/rendering/buffer.h"
#include "core/rendering/texture.h"
#include "core/rendering/sampler.h"
#include "core/rendering/program.h"
#include "core/rendering/framebuffer.h"

#include "input/hmd.h"
using namespace en::input;

namespace en
{
   namespace gpu
   {






   #define ESSLRevisions 3
   //extern const Nversion    ESSL_3_00;             // ESSL 1.00 
   //extern const Nversion    ESSL_1_00;             // ESSL 1.00  
   //extern const Nversion    ESSL_Unsupported;      // For marking unsupported features

   #define OpenGLESRevisions 5
   //extern const Nversion    OpenGL_ES_3_1;         // OpenGL ES 3.1 - 
   //extern const Nversion    OpenGL_ES_3_0;         // OpenGL ES 3.0 - 
   //extern const Nversion    OpenGL_ES_2_0;         // OpenGL ES 2.0 - 
   //extern const Nversion    OpenGL_ES_1_1;         // OpenGL ES 1.1 - 
   //extern const Nversion    OpenGL_ES_1_0;         // OpenGL ES 1.0 - 
   //extern const Nversion    OpenGL_ES_Unsupported; // For marking unsupported features

   #define GLSLRevisions 12
   //extern const Nversion    GLSL_Next;             // Future GLSL versions
   //extern const Nversion    GLSL_4_40;             // GLSL 4.40
   //extern const Nversion    GLSL_4_30;             // GLSL 4.30
   //extern const Nversion    GLSL_4_20;             // GLSL 4.20
   //extern const Nversion    GLSL_4_10;             // GLSL 4.10
   //extern const Nversion    GLSL_4_00;             // GLSL 4.00
   //extern const Nversion    GLSL_3_30;             // GLSL 3.30
   //extern const Nversion    GLSL_1_50;             // GLSL 1.50 for OpenGL 3.2
   //extern const Nversion    GLSL_1_40;             // GLSL 1.40 for OpenGL 3.1
   //extern const Nversion    GLSL_1_30;             // GLSL 1.30 for OpenGL 3.0
   //extern const Nversion    GLSL_1_20;             // GLSL 1.20 for OpenGL 2.1
   //extern const Nversion    GLSL_1_10;             // GLSL 1.10 for OpenGL 2.0
   //extern const Nversion    GLSL_Unsupported;      // For marking unsupported features
   
   #define OpenGLRevisions 19
   //extern const Nversion    OpenGL_Next;           // Future OpenGL versions 
   //extern const Nversion    OpenGL_4_4;            // OpenGL 4.4    -
   //extern const Nversion    OpenGL_4_3;            // OpenGL 4.3    -
   //extern const Nversion    OpenGL_4_2;            // OpenGL 4.2    -
   //extern const Nversion    OpenGL_4_1;            // OpenGL 4.1    - 
   //extern const Nversion    OpenGL_4_0;            // OpenGL 4.0    - 11.03.2010 ok
   //extern const Nversion    OpenGL_3_3;            // OpenGL 3.3    - 11.03.2010 ok
   //extern const Nversion    OpenGL_3_2;            // OpenGL 3.2    - 24.07.2009 ok
   //extern const Nversion    OpenGL_3_1;            // OpenGL 3.1    - 24.03.2009 ok
   //extern const Nversion    OpenGL_3_0;            // OpenGL 3.0    - 11.08.2008 ok
   //extern const Nversion    OpenGL_2_1;            // OpenGL 2.1    - 30.07.2006 spec
   //extern const Nversion    OpenGL_2_0;            // OpenGL 2.0    - 22.10.2004 spec
   //extern const Nversion    OpenGL_1_5;            // OpenGL 1.5    - 29.07.2003 / 30.10.2003 spec
   //extern const Nversion    OpenGL_1_4;            // OpenGL 1.4    - 24.07.2002 spec
   //extern const Nversion    OpenGL_1_3;            // OpenGL 1.3    - 14.08.2001 spec
   //extern const Nversion    OpenGL_1_2_1;          // OpenGL 1.2.1  - 01.04.1999 spec
   //extern const Nversion    OpenGL_1_2;            // OpenGL 1.2    -
   //extern const Nversion    OpenGL_1_1;            // OpenGL 1.1    - 29.03.1997
   //extern const Nversion    OpenGL_1_0;            // OpenGL 1.0    - 20.06.1992
   //extern const Nversion    OpenGL_Unsupported;    // For marking unsupported features

   enum class ScreenMode : uint32
        {
        Window            = 0,   // Create Window
        BorderlessWindow     ,   // Create borderless Window
        Fullscreen               // Create full screen surface
        };

   // Screen settings structure
   struct ScreenSettings
          {
          Nversion   shadingLanguage; // Reqested shading language version
          sint16     display;         // Display to use for fullscreen mode (-1 not set)
          uint16     width;           // If width and height are set to zeroe's
          uint16     height;          // native dispaly resolution will be used.
          uint8      samples;         // If more than one, multisampling is on.
          ScreenMode mode;            // Mode in which we create rendering surface

          ScreenSettings();
          };

   struct Interface
          {
          struct Screen                       
                 {
                 bool   create(const string& title);     // Creates window with default settings
                 bool   create(const ScreenMode fullscreen, 
                               const uint16  width, 
                               const uint16  height, 
                               const string& title);     // Creates window with basic settings
                 bool   create(ScreenSettings* settings,
                               const string& title);     // Creates window with advanced settings
                 bool   capture(const string filename);  // Performs screenshot operation
                 void   view(uint32 x, 
                             uint32 y, 
                             uint32 width, 
                             uint32 height);             
                 void   view(uint32v4 viewport);         // Sets current viewport on screen surface
                 bool   created(void);                   // Tells if screen is created
                 uint16 width(void);                     // Returns width of rendering surface
                 uint16 height(void);                    // Returns height of rendering surface
                 bool   fullscreen(void);                // Returns fullscreen status
                 float  aspect(void);                    // Returns aspect ratio of rendering surface
                 sint32 display(void);                   // Display on which screen window was created
                 bool   destroy(void);                   // Closes window and deletes rendering context surface
                 } screen;

          // Supported features
          struct Support
                 {
                 struct Input
                        {
                        static bool (*type)(const en::gpu::Attribute format);         // Checks if specified attribute format is supported
                        } input;

                 struct Buffer
                        {
                        bool type(const en::gpu::BufferType type);                  // Check if specificed buffer type is supported
                        } buffer;

                 struct Texture
                        {
                        bool type(const en::gpu::TextureType type);  // Checks if specified texture type is supported
                        bool format(const en::gpu::Format format);   // Checks if specified texture format is supported

                        //static bool (*type)(const en::gpu::TextureType type);       // Checks if specified texture type is supported
                        //static bool (*format)(const en::gpu::TextureFormat format); // Checks if specified texture format is supported
                        } texture;
                 } support;
          
          
// <<<<< -- NEW RENDERING INTERFACE - BEGIN

          struct IInputLayout
             {
             //shared_ptr<en::gpu::InputLayout> Create(Attribute& attributes[MaxInputLayoutAttributesCount],   // Reference to array specifying each vertex attribute, and it's source buffer
             //                                    shared_ptr<Buffer>    buffer[MaxInputLayoutAttributesCount]);  // Array of buffer handles that will be used
             };

// <<<<< -- NEW RENDERING INTERFACE - END

          struct Buffer
                 {
                 // Unified function for creation different types of buffers in gpu
                 //en::gpu::Buffer create(const BufferSettings& bufferSettings, void* data = NULL);
                 } buffer;
          
          struct ITexture
                 {
                 // Creates texture 
                 shared_ptr<en::gpu::Texture> create(const TextureState& textureState);
                 uint16 bitsPerTexel(const en::gpu::Format format);
                 } texture;

          struct ISampler
                 {
                 // Creates sampler states 
                 shared_ptr<en::gpu::Sampler> create(const SamplerState& state);
                 } sampler;
          
          //struct Shader
          //       {
          //       // Creates shader
          //       en::gpu::Shader create(const PipelineStage stage, const string& code);
          //       en::gpu::Shader create(const PipelineStage stage, const vector<string>& code);                      
          //       } shader;
          //
          //struct Program
          //       {
          //       // Creates program
          //       en::gpu::Program create(const vector<en::gpu::Shader>& shaders);
          //       } program;
          
          // Culling
          struct Culling
                 {
                 void front(const NormalCalculationMethod function); // Sets method for calculating front face
                 void face(const Face face);                         // Sets face that will be culled
                 void off(void);                                     // Turns off culling
                 void on(void);                                      // Restores culling  
                 } culling;
          
          // Scissor
          struct Scissor
                 {
                 void rect(const uint32 left, 
                           const uint32 bottom, 
                           const uint32 width, 
                           const uint32 height); //Set visible area
                 void off(void);                 // Turns off scissor test
                 void on(void);                  // Restores scissor test
                 } scissor;
          
          // Depth
          struct Depth
                 {
                 struct Buffer
                        {
                        void clearValue(const double depth); // Specifies depth buffer clear value that is clamped to [0..1]
                        void clearValue(const float depth);  // Specifies depth buffer clear value that is clamped to [0..1]
                        void clear(void);                    // Clears depth buffer with previously specified value
                        void off(void);                      // Disables writing to depth buffer
                        void on(void);                       // Enables writing to depth buffer
                        } buffer;
          
                 struct Test
                        {
                        void operator() (const CompareMethod function); // Turns on depth test with specified test function
                        void function(const CompareMethod function);    // Sets depth test function
                        void off(void);                                 // Turns off depth test
                        void on(void);                                  // Restores depth test
                        } test;
                 } depth;
          
          // Stencil
          struct Stencil
                 {
                 struct Buffer
                        {
                        void clearValue(const uint8 stencil);  // Specifies stencil buffer clear value that is in range [0..2^8-1]
                        void clear(void);                      // Clears stencil buffer with previously specified value
                        void off(void);                        // Disables writing to stencil buffer
                        void on(void);                         // Enables writing to stencil buffer
                        } buffer;
          
                 struct Test
                        {
                        void function(const CompareMethod compare, 
                                      const sint32 reference, 
                                      const uint8 mask = 0xFF);                        // Specifies stencil test parameters, mask is optional
                        void function(const Face face, 
                                      const CompareMethod compare, 
                                      const sint32 reference, 
                                      const uint8 mask = 0xFF);                        // Specifies stencil test parameters for each face, mask is optional
                        void operation(const StencilModification whenStencilTestFailed,
                                       const StencilModification whenDepthTestFailed,
                                       const StencilModification whenDepthTestPassed); // Specifies what operation should be performed on buffer
                        void operation(const Face face,
                                       const StencilModification whenStencilTestFailed,
                                       const StencilModification whenDepthTestFailed,
                                       const StencilModification whenDepthTestPassed); // Specifies what operation should be performed on buffer for each face
                        void off(void);                                                // Turns off stencil test
                        void on(void);                                                 // Restores stencil test
                        } test;
                 } stencil;
          
          // Color buffer
          struct Color
                 {
                 struct Buffer
                        {
                        void clearValue(const float4 color); // Specifies color buffer clear value that is clamped to [0..1]
                        void clearValue(const float r, 
                                        const float g, 
                                        const float b, 
                                        const float a);      // Specifies color buffer clear value that is clamped to [0..1]
                        void clear(void);                    // Clears color buffer with previously specified value
                        void off(void);                      // Disables writing to color buffer
                        void on(void);                       // Enables writing to color buffer
                        void mask(const bool r,              // Masks specified channels
                                  const bool g,  
                                  const bool b,
                                  const bool a);
                        } buffer;
                 } color;
          
          struct Output
                 {
                 struct Blend
                        {
                        // Operations on default color output 
                        void on(void);   
                        bool function(const BlendFunction srcRGBA,
                                      const BlendFunction dstRGBA);
                        bool function(const BlendFunction srcRGB,
                                      const BlendFunction srcA,
                                      const BlendFunction dstRGB,
                                      const BlendFunction dstA);
                        bool equation(const BlendEquation rgba);
                        bool equation(const BlendEquation rgb,
                                      const BlendEquation a);
                        void off(void);                                      

                        // Operations on color output N
                        bool on(const uint8 n);
                        bool function(const uint8 n,
                                      const BlendFunction srcRGBA,
                                      const BlendFunction dstRGBA);       
                        bool function(const uint8 n,
                                      const BlendFunction srcRGB,
                                      const BlendFunction srcA,
                                      const BlendFunction dstRGB,
                                      const BlendFunction dstA);
                        bool equation(const uint8 n,
                                      const BlendEquation rgba);
                        bool equation(const uint8 n,
                                      const BlendEquation rgb,
                                      const BlendEquation a);
                        bool off(const uint8 n);      

                        // Default blend equation color
                        void color(const float4 color);
                        void color(const float r,      
                                   const float g,  
                                   const float b,
                                   const float a);
                        } blend;
                 
                 // Attaching textures as render targets
                 struct Buffer
                        {
                        shared_ptr<Framebuffer> create(void);

                        bool set(const shared_ptr<Framebuffer> framebuffer);
                        bool setRead(const shared_ptr<Framebuffer> framebuffer);
                        bool setWrite(const shared_ptr<Framebuffer> framebuffer);
                        void setDefault(void);
                        void setDefaultRead(void);
                        void setDefaultWrite(void);
                        void copy(const uint32v4 srcRange, 
                                  const uint32v4 dstRange,
                                  const SamplerFilter filtering); // Copies data from currently bound Read source to Write destination
                        } buffer;

                 // Specifies color space used in shaders
                 void mode(const ColorSpace mode);

                 } output;

          bool vsync(bool state);
          static void (*display)(void);
          void waitForIdle(void);
          };
   }

extern gpu::Interface Gpu;
}
#endif

#endif
