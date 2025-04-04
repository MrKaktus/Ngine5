/*

 Ngine v5.0
 
 Module      : Depth-Stencil State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_DEPTH_STENCIL_STATE
#define ENG_CORE_RENDERING_DEPTH_STENCIL_STATE

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
namespace gpu
{

/// Stencil Buffer Modification Methods
enum class StencilOperation : uint32
{
    Keep                        = 0,
    Clear                          ,
    Reference                      ,
    Increase                       ,
    Decrease                       ,
    InvertBits                     ,
    IncreaseWrap                   ,
    DecreaseWrap                   ,
    Count
};
      
/// Default state:
///
/// - enableDepthTest   = false
/// - enableDepthWrite  = false
/// - enableDepthBounds = false
/// - enableStencilTest = false
/// - depthFunction     = Always
/// - depthRange        = float2(0.0f, 1.0f)
///
/// Stencil state for each face:
///
/// - function         = Always;
/// - whenStencilFails = StencilOperation::Keep;
/// - whenDepthFails   = StencilOperation::Keep;
/// - whenBothPass     = StencilOperation::Keep;
/// - reference        = 0;
/// - readMask         = 0XFFFFFFFF;
/// - writeMask        = 0xFFFFFFFF;
///
struct DepthStencilStateInfo
{
    bool enableDepthTest;
    bool enableDepthWrite;
    bool enableDepthBounds;
    bool enableStencilTest;
    CompareMethod depthFunction;
    float2 depthRange;

    /// function         - Compare method to use for each test
    /// whenStencilFails - What to do if stencil test fails (depth test passes)
    /// whenDepthFails   - What to do if stencil test passes (depth test fails)
    /// whenBothPass     - What to do if both tests passes
    /// reference        - Reference value
    /// readMask         - Mask that is ANDed with both the reference value and
    ///                    the stored stencil value before the test. It's also
    ///                    called "compare mask".
    /// writeMask        - Mask that is ANDed with output stencil value before
    ///                    writing to stencil attachment.
    ///                    Masks allow Front and Back Stencil operations to
    ///                    work on separate ranges of bits in Stencil buffer.
    struct StencilState
    {
        CompareMethod    function;
        StencilOperation whenStencilFails;
        StencilOperation whenDepthFails;
        StencilOperation whenBothPass;
        uint32           reference;
        uint32           readMask;
        uint32           writeMask;
    } stencil[2];                        

    DepthStencilStateInfo();
};

class DepthStencilState
{
    public:
    virtual ~DepthStencilState() {};
};

} // en::gpu
} // en

#endif
