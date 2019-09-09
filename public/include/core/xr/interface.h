/*

 Ngine v5.0

 Module      : Interface
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_INTERFACE
#define ENG_CORE_XR_INTERFACE

namespace en
{
namespace xr
{
class Interface
{
    public:

    /// Creates this class instance (API specific) and assigns it to "XR"
    static bool create(void);
};

} // en::xr

extern std::shared_ptr<xr::Interface> XR;

} // en

#endif