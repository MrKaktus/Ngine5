/*

 Ngine v5.0
 
 Module      : Input Layout.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_INPUT_LAYOUT
#define ENG_CORE_RENDERING_INPUT_LAYOUT

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/buffer.h"

namespace en
{
namespace gpu
{

struct AttributeDesc
{
    Attribute format;   ///< Format in which input data are represented
    uint32    buffer;   ///< Index of bound buffer that will be used as source.
    uint32    offset;   ///< Offset in bytes to first element of given attribute
};                      ///< in source buffer. This offset on some architectures
                        ///< needs to be aligned.
      
struct BufferDesc
{
    uint32 elementSize; ///< Size of one element in buffer
                        ///< (all attributes with their padding).
    uint32 stepRate;    ///< Describes how often Input Assembler should switch
};                      ///< to next element. By default it's set to 0 which
                        ///< means buffer will be iterated on per vertex rate.
                        ///< If value is greater, it describes by how many
                        ///< Draw Instances each structured element is shared,
                        ///< before Input Assembler should proceed to next one.

/// Handle for Input Assembler binding layout
class InputLayout
{
    public:
    virtual ~InputLayout() {};
};

} // en::gpu
} // en

#endif
