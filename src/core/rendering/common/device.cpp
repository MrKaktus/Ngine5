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
   support.maxTextureCubeSize               = 0;
   support.maxTexture3DSize                 = 0;
   support.maxTextureLayers                 = 0;
   support.maxTextureBufferSize             = 0;
   support.maxTextureLodBias                = 0.0f;

   // Sampler
   support.maxAnisotropy                    = 1.0f;  // 1.0 means anisotropy is disabled
   
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
   defaultState = new PipelineState(*this);
   }
   
   class CommonGraphicsAPI;

   uint32 CommonDevice::displays(void) const
   {
   // Currently all devices share all available displays
   CommonGraphicAPI* api = reinterpret_cast<CommonGraphicAPI*>(en::Graphics.get());
   return api->displaysCount;
   }
   
   std::shared_ptr<Display> CommonDevice::display(const uint32 index) const
   {
   // Currently all devices share all available displays
   CommonGraphicAPI* api = reinterpret_cast<CommonGraphicAPI*>(en::Graphics.get());
     
   assert( api->displaysCount > index );
   
   return api->displayArray[index];
   }

   uint64 CommonDevice::dedicatedMemorySize(void)
   {
       return support.videoMemorySize;
   }

   uint64 CommonDevice::systemMemorySize(void)
   {
       return support.systemMemorySize;
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
   
   //std::shared_ptr<InputLayout> CommonDevice::createInputLayout(const DrawableType primitiveType,
   //                                                        const uint32 controlPoints,
   //                                                        const uint32 usedAttributes,
   //                                                        const uint32 usedBuffers,
   //                                                        const AttributeDesc* attributes,
   //                                                        const BufferDesc* buffers)
   //{
   //// Should be implemented by API
   //assert(0);
   //return std::shared_ptr<InputLayout>(nullptr);
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
   
   PipelineState::PipelineState(GpuDevice& device)
   {
   // Create default Pipeline State
   // Application still needs to be set those:
   // 
   // viewportState
   // shader[5]
   //
   inputLayout        = device.createInputLayout(TriangleStripes);

   RasterStateInfo defaultRasterState;
   rasterState        = device.createRasterState(defaultRasterState);
   multisamplingState = device.createMultisamplingState(1u, false, false);

   DepthStencilStateInfo defaultDepthStencilState;
   depthStencilState  = device.createDepthStencilState(defaultDepthStencilState);

   BlendStateInfo      defaultBlendState;
   BlendAttachmentInfo defaultBlendAttachmentState;
   blendState         = device.createBlendState(defaultBlendState, 1u, &defaultBlendAttachmentState);

   pipelineLayout     = device.createPipelineLayout(0u, nullptr);

   for(uint32 i=0; i<5; ++i)
      function[i] = std::string("main");
   }

   PipelineState CommonDevice::defaultPipelineState(void)
   {
   // Create copy of Pipeline State so that app can modify 
   // some of the states without affecting shared default ones.
   return *defaultState;
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

   displayArray = new std::shared_ptr<CommonDisplay>[displaysCount];
   virtualDisplay = std::make_shared<winDisplay>();
  
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

         std::shared_ptr<winDisplay> currentDisplay = std::make_shared<winDisplay>();
         
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
         displayArray[activeId] = currentDisplay;
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
   if (Graphics)
      return true;

   Log << "Starting module: Rendering.\n";

   // Load from config file desired Rendering API and Shading Language Version
   // Load choosed API for Android & Windows
   
#if defined(EN_PLATFORM_ANDROID)
   Graphics = std::make_shared<VulkanAPI>();
#endif
#if defined(EN_PLATFORM_BLACKBERRY)
   Graphics = std::make_shared<OpenGLESAPI>();
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
   Graphics = std::make_shared<MetalAPI>();
#endif
#if defined(EN_PLATFORM_WINDOWS)
   // API Selection based on config file / terminal parameters
   if (Config.get("g.api", std::string("d3d12")))
      Graphics = std::make_shared<Direct3DAPI>("Ngine5.0");
   else
      Graphics = std::make_shared<VulkanAPI>("Ngine5.0");      // TODO: Propagate application name !
#endif

   return (Graphics == nullptr) ? false : true;
   }
   
   }
   
std::shared_ptr<gpu::GraphicAPI> Graphics;
}


