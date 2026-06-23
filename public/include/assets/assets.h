/*

 Ngine v5.0

 Module      : Assets manager.
 Requirements: none
 Description : Public interface.

*/

#ifndef ENG_ASSETS
#define ENG_ASSETS

#include <string>

#include "core/defines.h"
#include "core/types.h"
#include "assets/handle.h"
#include "core/rendering/device.h"

namespace en
{
namespace assets
{

class Interface
{
public:

    virtual const std::string& assetsPath(void) const = 0;
    virtual const std::string& screenshotsPath(void) const = 0;

    // Curently information about descriptors, their layout and set 
    // is exposed for direct binding of GPU resources by the app.
    //
    // TODO: Application could allocate its own, independent descriptors
    //       set, based on its own layout, composing only of Sampler and
    //       Uniforms (as example). In such case it wouldn't need any 
    //       such information from AssetManager (but pipeline state
    //       would require interacting with two different sets).
    //       It would also still require AssetManager/Streamer set layout.
    //
    virtual gpu::SetLayout& getDescriptorsLayout(void) const = 0;
    virtual gpu::DescriptorSet& getDescriptors(void) const = 0;
    virtual uint32 getSamplerDescriptorsBaseIndex(void) const = 0;
    virtual uint32 getUniformDescriptorsBaseIndex(void) const = 0;

    virtual ~Interface() {};                       // Polymorphic deletes require a virtual base destructor
};

} // en::assets

assets::Interface& Assets();

} // en

#endif