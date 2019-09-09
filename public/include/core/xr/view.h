/*

 Ngine v5.0

 Module      : View
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR 
               functionality (AR/MR/VR). It allows 
               programmer to use easy abstraction 
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_VIEW
#define ENG_CORE_XR_VIEW

namespace en
{
namespace xr
{
class View
{
    public:

    virtual ~View() {};
};

} // en::xr
} // en
#endif
