/*

 Ngine v5.0
 
 Module      : Stereoscopy rendering
 Requirements: none
 Description : Supports rendering in stereoscopy mode to HMD.

*/

#ifndef ENG_RENDERING_STEREO
#define ENG_RENDERING_STEREO

#include "input/input.h"
#include "scene/cam.h"
#include "utilities/timer.h"

using namespace en::input;
using namespace en::gpu;
using namespace en::resources;

namespace en
{

//   class Stereoscopy
//         {
//         private:   
//         std::shared_ptr<OculusX> device;
//
//         // GPU Resource handles
//         std::shared_ptr<Model>  model;
//         Program     program;
//         uint32      sampler;
//         Parameter   eyeToSourceUVScale;
//         Parameter   eyeToSourceUVOffset;
//         Parameter   eyeRotationStart;
//         Parameter   eyeRotationEnd;
//         Program     latencyProgram;
//         Parameter   latencyProjection;
//         Parameter   latencyColor;
//         Buffer      latencyBuffer;
//   
//         // Values
//         bool      ready;
//         bool      enable;
//         uint32v2  size;
//         DistortionSettings settings[2];
//   
//         // HMD orientation at the beginning of the frame
//         float3    position;
//         float3    rotation;
//   
//         // Debug:
//         std::shared_ptr<Texture> texture;
//         
//         Timer     timerR, timerS;
//         Time      rendering; // real frame time
//         Time      syncing;   // SDK sync time
//   
//         public:
//         Stereoscopy(std::shared_ptr<HMD> hmd);  
//        ~Stereoscopy();
//   
//         //void screenshot(void);
//   
//         void     on(void);
//         void     off(void);
//         bool     enabled(void);
//         void     startFrame(const uint32 frameIndex = 0); // Beginning of frame rendering, returns orientation of HMD as Euler angles 
//         uint32v4 viewport(uint8 eye);                     // Get viewport for current eye
//         void     source(std::shared_ptr<Texture> texture);                 // Allows adjusting render target size every frame, need to be called at least once to set source for distortion
//         bool     display(void);                           // End of frame rendering, performs distortion reprojection, VSync and buffer swap
//         };

} // en

#endif
