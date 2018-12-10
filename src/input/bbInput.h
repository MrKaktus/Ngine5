/*

 Ngine v5.0
 
 Module      : Input controllers and devices.
 Requirements: none
 Description : PRIVATE HEADER

*/

#ifndef ENG_INPUT_BLACKBERRY
#define ENG_INPUT_BLACKBERRY

#include "input/common.h"

#if defined(EN_PLATFORM_BLACKBERRY)
namespace en
{
   namespace input
   {      
   class BBInterface : public CommonInput
      {
      public:

      void update(void);                             // Gets actual input state, call function handling cached events
      
      BBInterface();
      virtual ~BBInterface();                       // Polymorphic deletes require a virtual base destructor
      };
   }
}
#endif

#endif