/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_OSX
#define ENG_INPUT_OSX

#include "input/common.h"

#if defined(EN_PLATFORM_OSX)

namespace en
{
namespace input
{

class OSXMouse : public CommonMouse
{
    public:
    bool   position(const uint32 x, const uint32 y);
    bool   position(const std::shared_ptr<Display> display, const uint32 x, const uint32 y);
    uint32v2 virtualPosition(void) const;     
    bool     virtualPosition(const uint32 x, const uint32 y);
    void   show(void);
    void   hide(void);
    
    OSXMouse();
    virtual ~OSXMouse();                           // Polymorphic deletes require a virtual base destructor
};
      
class macInput : public CommonInput
{
    public:
    uint32 stateFlags; // Keys lock state
    
    virtual void updateIO(void);                   
    
    macInput();
    virtual ~macInput();                           // Polymorphic deletes require a virtual base destructor
};

} // en::input
} // en
#endif

#endif
