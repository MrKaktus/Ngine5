/*

 Ngine v5.0

 Module      : Presentation Session
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_PRESENTATION_SESSION
#define ENG_CORE_XR_PRESENTATION_SESSION

#include "core/rendering/texture.h"
#include "core/xr/pose.h"
#include "core/xr/frameState.h"
#include "core/xr/frame.h"

namespace en
{
namespace xr
{
enum class PresentationMode : uint8
{
    Direct          = 0,
    MergedDigitally = 1,
    MergedOptically = 2,
};

enum class SurfaceLayout : uint8
{
    Dedicated = 0,
    Shared    = 1,
    Layered   = 2,
};

enum class TransferFunction : uint8
{
    Linear = 0,
    sRGB   = 1,
    Gamma2 = 2,
    Pow2   = 3,
};

enum class ColorGamut : uint8
{
    Rec709 = 0,
    P3     = 1,
};

struct PresentationSessionDescriptor
{
    PresentationMode  presentationMode;
    float             samplingQuality;
    uint32            samplesCount;
    bool              dynamicScaling;
    gpu::Format       colorFormat;
    TransferFunction  transferFunction;
    ColorGamut        colorGamut;
    gpu::TextureUsage colorUsage;
    SurfaceLayout     surfaceLayout;
    gpu::Format       depthFormat;
    gpu::TextureUsage depthUsage;
    float2            depthRange;
    uint32            frames;
};

class PresentationSession
{
    public:
    virtual void queryNextEvent(void) = 0;

    virtual uint64 startFrame(void) = 0;
    virtual FrameState* waitForOptimalPrediction(void) = 0;

    virtual void   startEncoding(void) = 0;
    /// Application receives ownership of returned frame, and should release it once it's done.
    virtual Frame* currentFrame(void) = 0;
    virtual void   presentWithPose(const Pose& pose) = 0;
    virtual void   endEncoding(void) = 0;

    virtual ~PresentationSession() {};
};

} // en::xr
} // en
#endif