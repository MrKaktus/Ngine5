/*

 Ngine v5.0
 
 Module      : State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_STATE
#define ENG_CORE_RENDERING_STATE

#include "core/types.h"

namespace en
{
namespace gpu
{

/// Color channels mask
enum class ChannelMask : uint32
{
    Red   = 0x1,
    Green = 0x2,
    Blue  = 0x4,
    Alpha = 0x8,
    All   = 0xF
};

/// Types of primitives to draw
///
/// LineLoops    - are unsupported by all modern API's (D3D12, Vulkan, Metal)
/// TriangleFans - are still supported only by Vulkan
enum DrawableType
{
    Points                      = 0,
    Lines                          ,
    LineStripes                    ,
    Triangles                      ,
    TriangleStripes                ,
    Patches                        ,
    DrawableTypesCount          
};

/// Comparison method             
enum CompareMethod               
{                           
    Never                       = 0,
    Less                           ,
    Equal                          ,
    LessOrEqual                    ,
    Greater                        ,
    NotEqual                       ,
    GreaterOrEqual                 ,
    Always                         ,
    CompareMethodsCount
};

enum FillMode
{
    Vertices                    = 0,
    Wireframe                      , 
    Solid                          ,
    FillModesCount
};

/// Surface face
enum class Face : uint32
{
    Front                       = 0,
    Back                           ,
    Count
};

/// Method used to calculate normal vector
enum NormalCalculationMethod
{
    ClockWise                   = 0,
    CounterClockWise               ,
    NormalCalculationMethodsCount
};

struct ColorSpacePrimaries
{
    float2 red;
    float2 green;
    float2 blue;
    float2 whitePoint;
};

/// Color space used
//
// TODO: Introduce proper concept of ColorSpace as complex descriptor struct.
// Color space is:
// - primaries (coordinates)
// - white point (coordinates)
// - transfer function with for e.g. "gamma" value (compression)
// Above can be specified directly through values, or throught enum. For e.g.:
// - sRGB transfer function enum implicitly specifies gamma value
// - raw gamma value explicitly specifies applied compression (its still sub-group of "sRGB" one)
//
// struct ColorSpace
// {
//     ColorSpacePrimaries primaries;
//     ColorSpaceType type;
//     float gamma;
//     . . .
// };
//
// Below would be ColorSpaceType. primaries and other properties would need to be accessed only if ColorSpaceType::Custom would be selected.
enum class ColorSpace : uint8
{      
    Unknown = 0,
    Linear     ,
    SRGB       , // May also be linear, depending on format in which color components are stored.
    CIE_XYZ    ,
    Count
};

enum class TransferFunction : uint8
{
    Unknown = 0,
    Linear     ,
    sRGB       ,
    Gamma2     ,
    Pow2       ,
};

enum class ColorGamut : uint8
{
    Rec709 = 0,
    P3        ,  
};

} // en::gpu
} // en

#endif
