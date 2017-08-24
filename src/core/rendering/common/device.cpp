/*
 
 Ngine v5.0
 
 Module      : Common Device.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that
               removes from him platform dependent
               implementation of graphic routines.
 
 */

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)
// atomics.inl prevents GDI
#ifdef NOGDI
#undef NOGDI
#endif
#include <windows.h> // Displays enumeration
#include <wingdi.h>
#endif

#include "core/log/log.h"
#include "core/rendering/common/device.h"

#if defined(EN_PLATFORM_OSX)
#include "core/rendering/metal/mtlAPI.h"
//extern class en::gpu::MetalAPI;
#endif
#if defined(EN_PLATFORM_WINDOWS)
#include "core/rendering/d3d12/dx12Device.h" // TODO: Separate header in future ?
#include "core/rendering/vulkan/vkDevice.h"  // TODO: Separate header in future ?
#include "core/rendering/windows/winDisplay.h" 
#endif

namespace en
{
   namespace gpu
   {
   const Nversion    Vulkan_1_0(0, EN_Vulkan, 0, 0, 0);                  // Vulkan 1.0

   const Nversion    Metal_OSX_1_0(0,EN_Metal_OSX,1,0,0);                // Metal on OSX 10.11
   const Nversion    Metal_OSX_Unsupported(255,EN_Metal_OSX,255,255,15); // For marking unsupported features
   
   const Nversion    Metal_IOS_1_0(0,EN_Metal_IOS,1,0,0);
   const Nversion    Metal_IOS_Unsupported(255,EN_Metal_IOS,255,255,15); // For marking unsupported features


#ifdef EN_OPENGL_DESKTOP
   #define SLVersions  12
   #define APIVersions 19
#endif
#ifdef EN_OPENGL_MOBILE
   #define SLVersions  2
   #define APIVersions 6
#endif
  
   const Nversion    ESSL_3_00(1,EN_ESSL,3,0,0);                         // ESSL 3.00  
   const Nversion    ESSL_1_00(0,EN_ESSL,1,0,0);                         // ESSL 1.00  
   const Nversion    ESSL_Unsupported(255,EN_ESSL,255,255,15);           // For marking unsupported features

   const Nversion    OpenGL_ES_3_2(5,EN_OpenGL_ES,3,2,0);                // OpenGL ES 3.2 -
   const Nversion    OpenGL_ES_3_1(4,EN_OpenGL_ES,3,1,0);                // OpenGL ES 3.1 - 
   const Nversion    OpenGL_ES_3_0(3,EN_OpenGL_ES,3,0,0);                // OpenGL ES 3.0 - 
   const Nversion    OpenGL_ES_2_0(2,EN_OpenGL_ES,2,0,0);                // OpenGL ES 2.0 - 
   const Nversion    OpenGL_ES_1_1(1,EN_OpenGL_ES,1,1,0);                // OpenGL ES 1.1 - 
   const Nversion    OpenGL_ES_1_0(0,EN_OpenGL_ES,1,0,0);                // OpenGL ES 1.0 - 
   const Nversion    OpenGL_ES_Unsupported(255,EN_OpenGL_ES,255,255,15); // For marking unsupported features

   const Nversion    GLSL_Next(255,EN_GLSL,9,90,0);                      // Future GLSL versions
   const Nversion    GLSL_4_50(11,EN_GLSL,4,50,0);                       // GLSL 4.50
   const Nversion    GLSL_4_40(10,EN_GLSL,4,40,0);                       // GLSL 4.40
   const Nversion    GLSL_4_30(9,EN_GLSL,4,30,0);                        // GLSL 4.30
   const Nversion    GLSL_4_20(8,EN_GLSL,4,20,0);                        // GLSL 4.20
   const Nversion    GLSL_4_10(7,EN_GLSL,4,10,0);                        // GLSL 4.10
   const Nversion    GLSL_4_00(6,EN_GLSL,4,0,0);                         // GLSL 4.00
   const Nversion    GLSL_3_30(5,EN_GLSL,3,30,0);                        // GLSL 3.30
   const Nversion    GLSL_1_50(4,EN_GLSL,1,50,0);                        // GLSL 1.50 for OpenGL 3.2
   const Nversion    GLSL_1_40(3,EN_GLSL,1,40,0);                        // GLSL 1.40 for OpenGL 3.1
   const Nversion    GLSL_1_30(2,EN_GLSL,1,30,0);                        // GLSL 1.30 for OpenGL 3.0
   const Nversion    GLSL_1_20(1,EN_GLSL,1,20,0);                        // GLSL 1.20 for OpenGL 2.1
   const Nversion    GLSL_1_10(0,EN_GLSL,1,10,0);                        // GLSL 1.10 for OpenGL 2.0
   const Nversion    GLSL_Unsupported(255,EN_GLSL,255,255,15);           // For marking unsupported features

   const Nversion    OpenGL_Next(255,EN_OpenGL,9,9,0);                   // Future OpenGL versions
   const Nversion    OpenGL_4_5(18,EN_OpenGL,4,5,0);                     // OpenGL 4.5    -  
   const Nversion    OpenGL_4_4(17,EN_OpenGL,4,4,0);                     // OpenGL 4.4    -   
   const Nversion    OpenGL_4_3(16,EN_OpenGL,4,3,0);                     // OpenGL 4.3    -   
   const Nversion    OpenGL_4_2(15,EN_OpenGL,4,2,0);                     // OpenGL 4.2    -
   const Nversion    OpenGL_4_1(14,EN_OpenGL,4,1,0);                     // OpenGL 4.1    - 
   const Nversion    OpenGL_4_0(13,EN_OpenGL,4,0,0);                     // OpenGL 4.0    - 11.03.2010 ok
   const Nversion    OpenGL_3_3(12,EN_OpenGL,3,3,0);                     // OpenGL 3.3    - 11.03.2010 ok
   const Nversion    OpenGL_3_2(11,EN_OpenGL,3,2,0);                     // OpenGL 3.2    - 24.07.2009 ok
   const Nversion    OpenGL_3_1(10,EN_OpenGL,3,1,0);                     // OpenGL 3.1    - 24.03.2009 ok
   const Nversion    OpenGL_3_0(9,EN_OpenGL,3,0,0);                      // OpenGL 3.0    - 11.08.2008 ok
   const Nversion    OpenGL_2_1(8,EN_OpenGL,2,1,0);                      // OpenGL 2.1    - 30.07.2006 spec
   const Nversion    OpenGL_2_0(7,EN_OpenGL,2,0,0);                      // OpenGL 2.0    - 22.10.2004 spec
   const Nversion    OpenGL_1_5(6,EN_OpenGL,1,5,0);                      // OpenGL 1.5    - 29.07.2003 / 30.10.2003 spec
   const Nversion    OpenGL_1_4(5,EN_OpenGL,1,4,0);                      // OpenGL 1.4    - 24.07.2002 spec
   const Nversion    OpenGL_1_3(4,EN_OpenGL,1,3,0);                      // OpenGL 1.3    - 14.08.2001 spec
   const Nversion    OpenGL_1_2_1(3,EN_OpenGL,1,2,1);                    // OpenGL 1.2.1  - 01.04.1999 spec
   const Nversion    OpenGL_1_2(2,EN_OpenGL,1,2,0);                      // OpenGL 1.2    -
   const Nversion    OpenGL_1_1(1,EN_OpenGL,1,1,0);                      // OpenGL 1.1    - 29.03.1997
   const Nversion    OpenGL_1_0(0,EN_OpenGL,1,0,0);                      // OpenGL 1.0    - 20.06.1992
   const Nversion    OpenGL_Unsupported(255,EN_OpenGL,255,255,15);       // For marking unsupported features

   CommonDevice::CommonDevice() :
      defaultState(nullptr)
   {
   support.attribute.reset();
   support.sampling.reset();
   support.rendering.reset();
      
   // Input Assembler
   support.maxInputLayoutBuffersCount       = 0;
   support.maxInputLayoutAttributesCount    = 0;

   // Texture
   support.maxTextureSize                   = 0;
   support.maxTextureRectSize               = 0;
   support.maxTextureCubeSize               = 0;
   support.maxTexture3DSize                 = 0;
   support.maxTextureLayers                 = 0;
   support.maxTextureBufferSize             = 0;
   support.maxTextureLodBias                = 0.0f;

   // Sampler
   support.maxAnisotropy                    = 0.0f;
   
   // Rasterizer
   support.maxColorAttachments              = 0;
   
   

//   maxRenderTargets               = 0;
//   maxClipDistances               = 0;
//   subpixelBits                   = 0;
//   maxTextureUnits                = 0;
//   maxRenderbufferSize            = 0;
//   maxViewportWidth               = 0;
//   maxViewportHeight              = 0;
//   maxViewports                   = 0;
//   viewportSubpixelBits           = 0;
//   viewportBoundsRange            = 0.0f;
//   layerProvokingVertex           = 0;
//   viewportProvokingVertex        = 0;
//   pointSizeMin                   = 0.0f;
//   pointSizeMax                   = 0.0f;
//   pointSizeGranularity           = 0.0f;
//   aliasedLineWidthMin            = 0.0f;
//   aliasedLineWidthMax            = 0.0f;
//   antialiasedLineWidthMin        = 0.0f;  
//   antialiasedLineWidthMax        = 0.0f;
//   antialiasedLineGranularity     = 0.0f;  
//   maxElementIndices              = 0;
//   maxElementVerices              = 0;
//   compressedTextureFormats       = 0; // TODO: This should be vector of formats!
//   numCompressedTextureFormats    = 0;
//   maxTextureBufferSize           = 0;
//   maxRectTextureSize             = 0;
//   programBinaryFormats           = 0; // TODO: This should be vector of formats!
//   numProgramBinaryFormats        = 0;
//   shaderBinaryFormats            = 0; // TODO: This should be vector of formats!
//   numShaderBinaryFormats         = 0;
//   shaderCompiler                 = false;
//   minMapBufferAligment           = 0;
//   maxFramebufferColorAttachments = 0;
//   postTransformVertexCacheSize   = 32;
//   maxPatchSize                   = 0;
   }

   void CommonDevice::init(void)
   {
   defaultState = new PipelineState(this);
   }
   
   class CommonGraphicsAPI;

   uint32 CommonDevice::displays(void) const
   {
   // Currently all devices share all available displays
   Ptr<CommonGraphicAPI> api = ptr_reinterpret_cast<CommonGraphicAPI>(&en::Graphics);
   return api->displaysCount;
   }
   
   Ptr<Display> CommonDevice::display(uint32 index) const
   {
   // Currently all devices share all available displays
   Ptr<CommonGraphicAPI> api = ptr_reinterpret_cast<CommonGraphicAPI>(&en::Graphics);
     
   assert( api->displaysCount > index );
   
   return ptr_reinterpret_cast<Display>(&api->displayArray[index]);
   }


   CommonDevice::~CommonDevice()
   {
   // Release default device objects
   defaultState->rasterState        = nullptr;
   defaultState->multisamplingState = nullptr;
   defaultState->depthStencilState  = nullptr;
   defaultState->blendState         = nullptr;
   delete defaultState;
   }
   
   //Ptr<InputLayout> CommonDevice::createInputLayout(const DrawableType primitiveType,
   //                                                    const uint32 controlPoints,
   //                                                    const uint32 usedAttributes,
   //                                                    const uint32 usedBuffers,
   //                                                    const AttributeDesc* attributes,
   //                                                    const BufferDesc* buffers)
   //{
   //// Should be implemented by API
   //assert(0);
   //return Ptr<InputLayout>(nullptr);
   //}
   

   // INFO: Descriptor is used when we want to be able to override any field with pur custom settings, but overall we want to use default ones

   // Device -> default State
   //           default States > default Pipeline description :
   //                            user optional changes
   //                            create pipeline with this description

   // Those objects need to be created from specific device, -> thus this whole structure need to be queried from device, that is then init with default objects kept by device and created on it's init


   PipelineState::PipelineState() :
      renderPass(nullptr),
      inputLayout(nullptr),
      viewportState(nullptr),
      rasterState(nullptr),
      multisamplingState(nullptr),
      depthStencilState(nullptr),
      blendState(nullptr),
      pipelineLayout(nullptr)
   {
   for(uint32 i=0; i<5; ++i)
      shader[i] = nullptr;
   }

   PipelineState::PipelineState(const PipelineState& src) :
      renderPass(src.renderPass),
      inputLayout(src.inputLayout),
      viewportState(src.viewportState),
      rasterState(src.rasterState),
      multisamplingState(src.multisamplingState),
      depthStencilState(src.depthStencilState),
      blendState(src.blendState),
      pipelineLayout(src.pipelineLayout)

   {
   for(uint32 i=0; i<5; ++i)
      {
      shader[i]   = src.shader[i];
      function[i] = src.function[i];
      }
   }
   
   PipelineState::PipelineState(GpuDevice* device)
   {
   // Create default Pipeline State
   // Application still needs to be set those:
   // 
   // viewportState
   // shader[5]
   //
   inputLayout        = device->createInputLayout(TriangleStripes);

   RasterStateInfo defaultRasterState;
   rasterState        = device->createRasterState(defaultRasterState);
   multisamplingState = device->createMultisamplingState(1u, false, false);

   DepthStencilStateInfo defaultDepthStencilState;
   depthStencilState  = device->createDepthStencilState(defaultDepthStencilState);

   BlendStateInfo      defaultBlendState;
   BlendAttachmentInfo defaultBlendAttachmentState;
   blendState         = device->createBlendState(defaultBlendState, 1u, &defaultBlendAttachmentState);

   pipelineLayout     = device->createPipelineLayout(0u, nullptr);

   for(uint32 i=0; i<5; ++i)
      function[i] = string("main");
   }

   PipelineState CommonDevice::defaultPipelineState(void)
   {
   // Create copy of Pipeline State so that app can modify 
   // some of the states without affecting shared default ones.
   return PipelineState(*raw_reinterpret_cast<PipelineState>(&defaultState));
   }

   CommonGraphicAPI::CommonGraphicAPI() :
      displayArray(nullptr),
      virtualDisplay(nullptr),
      displaysCount(0),
      displayPrimary(0),
      GraphicAPI()
   {
   // Windows OS - Windowing System Query (API Independent)
   //-------------------------------------------------------

#if defined(EN_PLATFORM_WINDOWS)
   // Display Device settings
   DISPLAY_DEVICE Device;
   memset(&Device, 0, sizeof(Device));
   Device.cb = sizeof(Device);

   // Calculate amount of available displays
   uint32 index = 0;
   while(EnumDisplayDevices(nullptr, index++, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
      if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
         displaysCount++;

   displayArray = new Ptr<CommonDisplay>[displaysCount];
   virtualDisplay = new winDisplay();
  
   // Clear structure for next display (to ensure there is no old data)
   memset(&Device, 0, sizeof(Device));
   Device.cb = sizeof(Device);
      
   // Gather information about available displays
   uint32 displayId = 0;
   uint32 activeId = 0;
   while(EnumDisplayDevices(nullptr, displayId, &Device, EDD_GET_DEVICE_INTERFACE_NAME))
      {
      // Only displays that are part of Virtual Desktop are used by engine
      // (so HMD displays won't be queried until they work in legacy mode as part of desktop)
      if (Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
         {
         uint32v2 desktopPosition;     // Display position on Virtual Desktop
         uint32v2 currentResolution;   // Displays current resolution
         uint32v2 nativeResolution;    // Displays native resolution (assumed largest possible)
         
         DEVMODE DispMode;
         memset(&DispMode, 0, sizeof(DispMode));
         DispMode.dmSize = sizeof(DispMode);

         // Query displays current resolution before game started
         assert( EnumDisplaySettingsEx(Device.DeviceName, ENUM_CURRENT_SETTINGS, &DispMode, 0u) );
 
         // Verify that proper values were returned by query
         assert( checkBitmask(DispMode.dmFields, (DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT)) );

         desktopPosition.x   = DispMode.dmPosition.x;
         desktopPosition.y   = DispMode.dmPosition.y;
         currentResolution.x = DispMode.dmPelsWidth;
         currentResolution.y = DispMode.dmPelsHeight;
            
         // Calculate amount of available display modes (count only modes supported by display and no rotation ones)
         uint32 modesCount = 0;
         while(EnumDisplaySettingsEx(Device.DeviceName, modesCount, &DispMode, 0u))
            modesCount++;

         Ptr<winDisplay> currentDisplay = new winDisplay();
         
         currentDisplay->modeResolution = new uint32v2[modesCount];
     
         // Gather information about all supported display modes
         uint32 modeId = 0;
         while(EnumDisplaySettingsEx(Device.DeviceName, modeId, &DispMode, 0u))
            {
            // Verify that proper values were returned by query
            assert( checkBitmask(DispMode.dmFields, DM_PELSWIDTH | DM_PELSHEIGHT) );
            
            currentDisplay->modeResolution[modeId].x = DispMode.dmPelsWidth;
            currentDisplay->modeResolution[modeId].y = DispMode.dmPelsHeight;

            // Find largest supported resolution and assume it's display native
            if ( (DispMode.dmPelsWidth  > nativeResolution.x) ||
                 (DispMode.dmPelsHeight > nativeResolution.y) )
               {
               nativeResolution.x = DispMode.dmPelsWidth;
               nativeResolution.y = DispMode.dmPelsHeight;
               }
               
            modeId++;
            }
        
         currentDisplay->_position          = desktopPosition;
         currentDisplay->_resolution        = nativeResolution;
         currentDisplay->observedResolution = currentResolution;
         currentDisplay->modesCount         = modesCount;
         currentDisplay->index              = displayId;
       
         // Select Primary Display
         if (Device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
            {
            displayPrimary = activeId;
            }
       
         // Calculate upper-left corner position, and size of virtual display.
         // It's assumed that X axis increases right, and Y axis increases down.
         // Virtual Display is a bounding box for all available displays.
         if (activeId == 0)
            {
            virtualDisplay->_position   = currentDisplay->_position;
            virtualDisplay->_resolution = currentDisplay->_resolution;
            }
         else
            {
            if (currentDisplay->_position.x < virtualDisplay->_position.x)
               {
               virtualDisplay->_resolution.width += (virtualDisplay->_position.x - currentDisplay->_position.x);
               virtualDisplay->_position.x = currentDisplay->_position.x;
               }
            if (currentDisplay->_position.y < virtualDisplay->_position.y)
               {
               virtualDisplay->_resolution.height += (virtualDisplay->_position.y - currentDisplay->_position.y);
               virtualDisplay->_position.y = currentDisplay->_position.y;
               }
            uint32 virtualRightBorder = virtualDisplay->_position.x + virtualDisplay->_resolution.width;
            uint32 currentRightBorder = currentDisplay->_position.x + currentDisplay->_resolution.width;
            if (virtualRightBorder < currentRightBorder)
               virtualDisplay->_resolution.width = currentRightBorder - virtualDisplay->_position.x;
            uint32 virtualBottomBorder = virtualDisplay->_position.y + virtualDisplay->_resolution.height;
            uint32 currentBottomBorder = currentDisplay->_position.y + currentDisplay->_resolution.height;
            if (virtualBottomBorder < currentBottomBorder)
               virtualDisplay->_resolution.height = currentBottomBorder - virtualDisplay->_position.y;
            }
       
         // Add active display to the list
         displayArray[activeId] = ptr_dynamic_cast<CommonDisplay, winDisplay>(currentDisplay);
         activeId++;
         }
         
      // Clear structure for next display (to ensure there is no old data)
      memset(&Device, 0, sizeof(Device));
      Device.cb = sizeof(Device);
      displayId++;
      }
#endif
   }

   CommonGraphicAPI::~CommonGraphicAPI()
   {
   virtualDisplay = nullptr;
   for(uint32 i=0; i<displaysCount; ++i)
      displayArray[i] = nullptr;
   delete [] displayArray;
   }


   // This static function should be in .mm file if we include Metal headers !!!
   bool GraphicAPI::create(void)
   {
   Log << "Starting module: Rendering.\n";

   // Load from config file desired Rendering API and Shading Language Version
   // Load choosed API for Android & Windows
   
#if defined(EN_PLATFORM_ANDROID)
   // Graphics = ptr_dynamic_cast<GraphicAPI, OpenGLESAPI>(Ptr<OpenGLESAPI>(new OpenGLESAPI()));
   // Graphics = ptr_dynamic_cast<GraphicAPI, VulkanAPI>  (Ptr<VulkanAPI>(new VulkanAPI()));
#endif
#if defined(EN_PLATFORM_BLACKBERRY)
   Graphics = ptr_dynamic_cast<GraphicAPI, OpenGLESAPI>(Ptr<OpenGLESAPI>(new OpenGLESAPI()));
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   Graphics = ptr_dynamic_cast<GraphicAPI, MetalAPI>(Ptr<MetalAPI>(new MetalAPI()));
#endif
#if defined(EN_PLATFORM_WINDOWS)

   // TODO: API Selection based on config file / terminal parameters
   if (Config.get("g.api", string("d3d12")))
      Graphics = ptr_dynamic_cast<GraphicAPI, Direct3DAPI>(Ptr<Direct3DAPI>(new Direct3DAPI("Ngine5.0")));
   else
      Graphics = ptr_dynamic_cast<GraphicAPI, VulkanAPI>(Ptr<VulkanAPI>(new VulkanAPI("Ngine5.0")));      // TODO: Propagate application name !

   // Graphics = ptr_dynamic_cast<GraphicAPI, OpenGLAPI>(Ptr<OpenGLAPI>(new OpenGLAPI()));
#endif

   return (Graphics == nullptr) ? false : true;
   }
   
   }
   
Ptr<gpu::GraphicAPI> Graphics;
}


