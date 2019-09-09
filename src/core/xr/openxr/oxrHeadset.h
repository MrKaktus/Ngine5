/*

 Ngine v5.0

 Module      : OpenXR Headset
 Requirements: none
 Description : XR context abstracts system specific
               backend implementation exposing XR
               functionality (AR/MR/VR). It allows
               programmer to use easy abstraction
               layer on all platforms.

*/

#ifndef ENG_CORE_XR_OPENXR_HEADSET
#define ENG_CORE_XR_OPENXR_HEADSET

#include "OpenXR/openxr.h"

#include "core/rendering/device.h"
#include "core/xr/headset.h"

namespace en
{
namespace xr
{
class oxrInterface;

class HeadsetProperties;
class HeadsetPresentationProperties;

class HeadsetProperties
{
};

class HeadsetPresentationProperties
{
};

class oxrHeadset : public Headset
{
public:
    oxrInterface* session;
    XrSystemId    runtime;   // Runtime handle

    // Runtime properties
    ///////////////////////

    XrSystemProperties             systemInfo;

    // Headset properties
    ///////////////////////

    gpu::GpuDevice*                gpu;

    // Blend modes
    uint32                         blendModesCount;
    XrEnvironmentBlendMode*        blendModes;

    // View Configurations
    uint32                         viewConfigurationsCount;
    XrViewConfigurationType*       viewConfigurationTypes;
    XrViewConfigurationProperties* viewConfigurations;

    bool supportsPresentationModeDirect;
	bool supportsPresentationModeMergedDigitally;
	bool supportsPresentationModeMergedOptically;




    gpu::GpuDevice*      device(void);
    HeadsetProperties*   properties(void);
    HeadsetPresentationProperties* properties(const PresentationSessionDescriptor* descriptor); // TODO: Error code
    PresentationSessionDescriptor* prefferedPresentationSessionDescriptor(void);
    bool supportsPresentation(const PresentationSessionDescriptor* descriptor); // TODO: Error code
    PresentationSession* createPresentationSession(const PresentationSessionDescriptor* descriptor);

    oxrHeadset(oxrInterface& session);
    ~oxrHeadset();
};

} // en::xr
} // en

#endif