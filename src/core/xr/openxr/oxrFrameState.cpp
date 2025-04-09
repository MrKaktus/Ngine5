

#include "assert.h"

#include "core/xr/openxr/oxrFrameState.h"

namespace en
{
namespace xr
{

oxrFrameState::oxrFrameState() :
    viewsArray(nullptr),
    viewsCount(0)
{
}

oxrFrameState::oxrFrameState(const uint32 _viewsCount) :
    viewsArray(new oxrView[_viewsCount]),
    viewsCount(_viewsCount)
{
}

oxrFrameState::~oxrFrameState()
{
    delete [] viewsArray;
}

uint32 oxrFrameState::views(void)
{
    return viewsCount;
}

View* oxrFrameState::view(const uint32 index)
{
    assert( index < viewsCount );

    return &viewsArray[index];
}

} // en::xr
} // en