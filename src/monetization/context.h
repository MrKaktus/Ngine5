/*

 Ngine v5.0
 
 Module      : Monetization modules.
 Requirements: none
 Description : Contains set of easy to use modules
               for monetization like advertisement
               banner, in app purchases support and
               more.

*/

#ifndef ENG_MONETIZATION_CONTEXT
#define ENG_MONETIZATION_CONTEXT

#ifdef EN_PLATFORM_BLACKBERRY
#include <unistd.h>
#include <bbads/bbads.h>
#include <bbads/event.h>
#endif

#include "monetization/monetization.h"

namespace en
{
   namespace monetization
   {
#ifdef EN_PLATFORM_BLACKBERRY
   class bbBanner : public Banner
         {
         public:                         
         bbads_banner_t* advertisement;      // Advertisement banner handle

         bbBanner( const uint32 x,
            const uint32 y,
            const uint32 width,
            const uint32 height,
            const uint32 seconds,
            const uint32 zoneID,          
            const std::string placeholder ); 
         ~bbBanner();                        // Polymorphic deletes require a virtual base destructor
         };
#endif

   struct Context
          {
#ifdef EN_PLATFORM_BLACKBERRY
          vector< std::shared_ptr<Banner> > banners;
#endif

          Context();
         ~Context();

          bool create(void);
          void destroy(void);
          };
   }

extern monetization::Context MonetizationContext;
}

#endif
