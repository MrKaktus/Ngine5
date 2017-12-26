/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none

*/

#ifndef ENG_INPUT_HMD_OCULUS
#define ENG_INPUT_HMD_OCULUS

#include "core/defines.h"

#if defined(EN_MODULE_OCULUS)

#include "input/common.h"
#include "input/hmd.h"

// WA: Undefines "Profile" Macro defined in rendering to avoid collision with OVR::Profile
#ifdef Profile
#undef Profile
#endif
#include "OculusSDK/LibOVR/Include/OVR_CAPI.h"                // Oculus SDK 3.x

#include "OculusSDK/LibOVR/Include/OVR_CAPI_GL.h"
#include "core/rendering/texture.h"
#include "core/rendering/framebuffer.h"

// Remember to link to: winmm.lib to remove missing dependiences in 0.2.5 SDK:
//1>Ngine4dbg.lib(OVR_Timer.obj) : error LNK2019: unresolved external symbol __imp__timeGetTime@0 referenced in function "public: static unsigned long __stdcall OVR::Timer::GetTicksMs(void)" (?GetTicksMs@Timer@OVR@@SGKXZ)
//1>Ngine4dbg.lib(OVR_Timer.obj) : error LNK2019: unresolved external symbol __imp__timeBeginPeriod@4 referenced in function "private: static void __cdecl OVR::Timer::initializeTimerSystem(void)" (?initializeTimerSystem@Timer@OVR@@CAXXZ)
//1>Ngine4dbg.lib(OVR_Timer.obj) : error LNK2019: unresolved external symbol __imp__timeEndPeriod@4 referenced in function "private: static void __cdecl OVR::Timer::shutdownTimerSystem(void)" (?shutdownTimerSystem@Timer@OVR@@CAXXZ)

using namespace en::gpu;

namespace en
{
   namespace input
   {
   class OculusDK2 : public HMD
      {
      public:
      bool             enabled;           // Is it turned on
      bool             sharedRT;          // True if using shared RT, otherwise using two separate RT's per eye (for saving VRAM)
      HMDType          deviceType;        // Type of HMD 
      uint32v2         resolutionRT;      // RenderTarget resolution
      uint32v4         view[2];           // Viewport position and size per eye
      sint32           displayId;         // Display number on desktop
      HMDMode          displayMode;       // Desktop or Direct rendering
      
      ovrSession       session;
      ovrGraphicsLuid  luid;
      
    //ovrHmd           hmd;               // Display
      ovrPosef         eyePose[2];        // Eye position and orientation 
      ovrTrackingState hmdState;          // Head position and orientation

      ovrTextureSwapChain swapChain[2];       // Texture Swap-Chain
      sint32              swapChainLength[2]; // Amount of textures in Swap-Chain
      sint32              currentIndex[2];    // Current texture in Swap-Chain to use
      ovrMirrorTexture    mirrorTexture;      // Texture used to mirror HMD display to window
      shared_ptr<Texture> swap[2][2];         // Up to two sets, 2 textures each
      shared_ptr<Texture> mirror;             // Mirror texture interface

      shared_ptr<Framebuffer> fbo;            // Mirroring framebuffer

      ovrEyeRenderDesc    EyeRenderDesc[2];   // Device per eye rendering information  
      double              sensorSampleTime;   // sensorSampleTime is fed into the layer later  
      uint32              frameIndex;         // Current rendered frame index

      ovrHmdDesc       info;           // Device information    
           
    //ovrFrameTiming   startTime;      // Time at which frame rendering started


      virtual bool      on(uint32v2 windowResolution,
                           bool sharedRendertarget = true);
      virtual bool      off(void);
      virtual bool      state(void) const;
      virtual HMDType   device(void) const;
      virtual HMDVendor vendor(void) const; 
      virtual HMDMode   mode(void) const;
      virtual sint32    display(void) const;
      virtual uint32v2  resolution(void) const;
      virtual uint32v2  renderTarget(void) const;
      virtual bool      sharedRendertarget(void) const;
      virtual float4    frustumTangents(Eye eye) const;
      virtual uint32v4  viewport(uint8 eye) const;
      virtual void      reset(void);
      virtual float2    playAreaDimensions(void);
      virtual bool      playAreaLocation(float3* corners);
      virtual bool      debugHUD(DebugHUD mode);
        
      virtual void      startFrame(const uint32 frameIndex = 0); // Acquires predicted head position for given frame
      virtual void      update(void);
      virtual float3    position(void) const;                    // Head position (when head tracking is supported)
                        
      virtual float3    position(Eye eye) const; 
      virtual float4x4  orientation(Eye eye) const;               
      virtual float3    vector(Eye eye) const;
      virtual shared_ptr<en::gpu::Texture> color(Eye eye = LeftEye) const;          // Returns current swap-chain texture that should be used for given eye
      //virtual shared_ptr<en::gpu::Texture> framebuffer(void) const;                 // Returns texture used for read content of HMD display
      
      //virtual void      waitForSync(void);
      virtual void      endRendering(Eye eye = LeftEye);
      virtual void      endFrame(bool mirrorToScreen = true);
      
      // Private API
      
      //ovrHmd          handle(void);       // Returns HMD handle in Oculus SDK
      
      OculusDK2(uint8 index);
      virtual ~OculusDK2();
      };  

   void InitOculusSDK(void); 
   void CloseOculusSDK(void); 
   }
}
#endif

#endif
