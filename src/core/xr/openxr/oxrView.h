/*

 Ngine v5.0

 Module      : OpenXR View
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_VIEW
#define ENG_CORE_XR_OPENXR_VIEW

#include "core/xr/view.h"

namespace en
{
namespace xr
{

class oxrView : public View
{
    public:
 
    oxrView();
   ~oxrView();
};

} // en::xr
} // en

#endif