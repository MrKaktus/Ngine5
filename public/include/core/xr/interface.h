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

#include "core/xr/headset.h"

namespace en
{
namespace xr
{

class Interface
{
    public:

    /// Creates this class instance (API specific) and assigns it to "XR"
    static bool create(void);

    virtual uint32 headsets(void) = 0;
    virtual Headset* headset(const uint32 index) = 0;
};

} // en::xr

extern std::unique_ptr<xr::Interface> XR;

} // en

#endif