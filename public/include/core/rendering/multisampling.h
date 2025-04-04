/*

 Ngine v5.0
 
 Module      : Multisampling State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/


#ifndef ENG_CORE_RENDERING_MULTISAMPLING_STATE
#define ENG_CORE_RENDERING_MULTISAMPLING_STATE

#include <memory>

#include "core/defines.h"
#include "core/types.h"

#include "core/rendering/state.h"

namespace en
{
namespace gpu
{

class MultisamplingState
{
    public:
    virtual ~MultisamplingState() {};
};

} // en::gpu
} // en

#endif
