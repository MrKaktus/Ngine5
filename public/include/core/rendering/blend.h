/*

 Ngine v5.0
 
 Module      : Blend State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_BLEND_STATE
#define ENG_CORE_RENDERING_BLEND_STATE

#include <memory>

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
namespace gpu
{
/// Operation performed by blend stage
enum class BlendMode : uint32
{
    Off                         = 0,
    BlendOperation              = 1,
  //LogicOperation              = 2 // Currently not exposed due to lack of support in Metal API.
};
      
/// Color Buffer blend factor
enum class BlendFactor : uint32
{
    Zero                        = 0,
    One                            ,
    Source                         ,
    OneMinusSource                 ,
    Destination                    ,
    OneMinusDestination            ,
    SourceAlpha                    ,
    OneMinusSourceAlpha            ,
    DestinationAlpha               ,
    OneMinusDestinationAlpha       ,
    ConstantColor                  ,
    OneMinusConst                  ,
    ConstantAlpha                  ,
    OneMinusConstantAlpha          ,
    SourceAlphaSaturate            ,
    SecondSource                   ,
    OneMinusSecondSource           ,
    SecondSourceAlpha              ,
    OneMinusSecondSourceAlpha      ,
    Count
};

/// Color Buffer blend operation
enum class BlendOperation : uint32
{
    Add                         = 0,
    Subtract                       ,
    DestinationMinusSource         ,
    Min                            ,
    Max                            ,
    Count
};

/// Color Buffer logic operations (currently unsupported)
enum class LogicOperation : uint32
{
    ClearDestination            = 0,
    Set                            ,
    Copy                           ,
    CopyInverted                   ,
    NoOperation                    ,
    Invert                         ,
    And                            ,
    NAnd                           ,
    Or                             ,
    Nor                            ,
    Xor                            ,
    Equiv                          ,
    AndReverse                     ,
    AndInverted                    ,
    OrReverse                      ,
    OrInverted                     ,
    Count
};

/// Color channels mask
enum class ColorMask : uint32
{
    Red                       = 0x1,
    Green                     = 0x2,
    Blue                      = 0x4,
    Alpha                     = 0x8,
    All                       = 0xF
};

/// Default state:
///
/// - blendColor     = float4(0.0f, 0.0f, 0.0f, 0.0f)
/// - enabledSamples = 0xFFFFFFFF
///
struct BlendStateInfo
{
    float4 blendColor;
    uint32 enabledSamples; // Masks which samples in MSAA RT's should be
                           // affected by blending operation.
    BlendStateInfo();
};

/// Default state:
///
/// - mode           = BlendMode::Off
/// - srcRGB         = BlendFactor::One
/// - dstRGB         = BlendFactor::Zero
/// - rgbFunc        = BlendOperation::Add
/// - srcAlpha       = BlendFactor::One
/// - dstAlpha       = BlendFactor::Zero
/// - alphaFunc      = BlendOperation::Add
/// - logic          = LogicOperation::NoOperation (ignored, unsupported)
/// - writeMask      = ColorMask::All
///
struct BlendAttachmentInfo
{
    BlendMode      mode;
    BlendFactor    srcRGB;         // Blend parameters
    BlendFactor    dstRGB;
    BlendOperation rgb;
    BlendFactor    srcAlpha;
    BlendFactor    dstAlpha;
    BlendOperation alpha;
    LogicOperation logic;          // Logic operation
    ColorMask      writeMask;

    BlendAttachmentInfo();
};

class BlendState
{
    public:
    virtual ~BlendState() {};
};

} // en::gpu
} // en

#endif
