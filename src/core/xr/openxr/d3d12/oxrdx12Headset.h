/*

 Ngine v5.0

 Module      : OpenXR D3D12 Headset
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_D3D12_HEADSET
#define ENG_CORE_XR_OPENXR_D3D12_HEADSET

#include "core/xr/openxr/oxrHeadset.h"

namespace en
{
namespace xr
{

class oxrHeadsetD3D12 : public oxrHeadset
{
public:
    void loadInterfaceFunctionPointers(void);
    void clearInterfaceFunctionPointers(void);

    oxrHeadsetD3D12(oxrInterface& session);
   ~oxrHeadsetD3D12();
};

} // en::xr
} // en

#endif