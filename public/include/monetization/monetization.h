/*

 Ngine v5.0
 
 Module      : Monetization modules.
 Requirements: none
 Description : Contains set of easy to use modules
               for monetization like advertisement
               banner, in app purchases support and
               more.

*/

#ifndef ENG_MONETIZATION
#define ENG_MONETIZATION

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"

namespace en
{
   namespace monetization
   {
   class Banner : public SafeObject<Banner>
         {
         public:
         virtual ~Banner();              // Polymorphic deletes require a virtual base destructor
         };

   struct Interface
          {
          struct Banner
                 {
#ifdef EN_PLATFORM_BLACKBERRY
                 Ptr<monetization::Banner> create(const uint32 x,            // Banners upper left corner x position on the screen 
                                                  const uint32 y,            // Banners upper left corner y position on the screen 
                                                  const uint32 width,        // Width in pixels
                                                  const uint32 height,       // Height in pixels
                                                  const uint32 seconds,      // Refresh rate in seconds (60 is default and minimum)
                                                  const uint32 zoneID,       // Banners unique ID in Advertising Service, trial zone is 117145 or 31848 or 16741
                                                  const string placeholder); // Name of placeholder image (must be in resources/textures path)
#endif               
                 } banner;
          };
   }

extern monetization::Interface Monetization;
}

#endif
