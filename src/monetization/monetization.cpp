/*

 Ngine v5.0
 
 Module      : Monetization modules.
 Requirements: none
 Description : Contains set of easy to use modules
               for monetization like advertisement
               banner, in app purchases support and
               more.

*/

#include "core/defines.h"
#include "core/types.h"
#include "core/log/log.h"

#include "core/rendering/context.h"
#include "monetization/context.h"

namespace en
{
   namespace monetization
   {
#ifdef EN_PLATFORM_BLACKBERRY
   bbBanner::bbBanner( const uint32 x,
      const uint32 y,
      const uint32 width,
      const uint32 height,
      const uint32 seconds,
      const uint32 zoneID,          
      const string placeholder ) :  
      advertisement(NULL)
   {
   bbads_error_t ret;

   // Create advertisement banner
   ret = bbads_banner_create(&advertisement, 
                              en::GpuContext.device.screen.window, 
                              en::GpuContext.device.screen.windowGroupName, 
                              zoneID);
   if (ret != BBADS_EOK)
      {
      Log << "ERROR: Cannot create advertisement!\n";
      advertisement = NULL;
      return;
      }

   // No border, just advertisement area
   if (bbads_banner_set_border_width(advertisement, 0) != BBADS_EOK)
      {
      Log << "ERROR: Cannot create advertisement without border!\n";
      bbads_banner_destroy(advertisement);
      advertisement = NULL;
      return;
      }

   // Set banner position and size
   ret = bbads_banner_set_position(advertisement, x, y);
   if (ret != BBADS_EOK)
      {
      Log << "ERROR: Cannot set advertisement position!\n";
      bbads_banner_destroy(advertisement);
      advertisement = NULL;
      return;
      }

   ret = bbads_banner_set_size(advertisement, width, height);
   if (ret != BBADS_EOK)
      {
      Log << "ERROR: Cannot set advertisement size!\n";
      bbads_banner_destroy(advertisement);
      advertisement = NULL;
      return;
      }

   // Set banner refresh rate in seconds (60 sec is min and default)
   ret = bbads_banner_set_refresh_rate(advertisement, seconds);
   if (ret != BBADS_EOK)
      {
      Log << "ERROR: Cannot set advertisement refresh rate!\n";
      bbads_banner_destroy(advertisement);
      advertisement = NULL;
      return;
      }

   // Set link to image, that will be used when no internet connection is available (like "buy this game to remove ads!")
   // The placeholder_url must be a NULL terminated local URL (using the "file://" protocol).
   char appPath[1024];
   getcwd(appPath, 1024);
   string url = string("file://" + string(appPath) + "/app/native/resources/textures/" + placeholder);
   ret = bbads_banner_set_placeholder_url(advertisement, url.c_str());
   if (ret != BBADS_EOK)
      Log << "WARNING: Cannot set advertisement placeholder!\n";

   // Register banner for receiving events for him through BPS
   ret = bbads_banner_request_events(advertisement);
   if (ret != BBADS_EOK)
      {
      Log << "ERROR: Cannot link advertisement banner with events system!\n";
      bbads_banner_destroy(advertisement);
      advertisement = NULL;
      return;
      }

   // Load ads from the Advertising Service web portal.
   ret = bbads_banner_load(advertisement);
   if (ret != BBADS_EOK)
      {
      Log << "ERROR: Cannot load advertisement from Advertising Service!\n";
      bbads_banner_destroy(advertisement);
      advertisement = NULL;
      return;
      }
   }

   bbBanner::~bbBanner()
   {
   if (!advertisement)
      return;

   // Unregister asynchronous events delivery for banner
   bbads_banner_stop_events(advertisement);
   
   // Destroy ad at the end
   bbads_banner_destroy(advertisement);
   }
#endif

   Banner::~Banner()
   {
   }

   Context::Context()
   {
   }

   Context::~Context()
   {
   }

   bool Context::create(void)
   {
   Log << "Starting module: Monetization." << endl;
   return true;
   }

   void Context::destroy(void)
   {
   Log << "Closing module: Monetization." << endl;
   }

#ifdef EN_PLATFORM_BLACKBERRY
   shared_ptr<monetization::Banner> Interface::Banner::create(const uint32 x,            // Banners upper left corner x position on the screen 
                                                       const uint32 y,            // Banners upper left corner y position on the screen 
                                                       const uint32 width,        // Width in pixels
                                                       const uint32 height,       // Height in pixels
                                                       const uint32 seconds,      // Refresh rate in seconds (60 is default and minimum)
                                                       const uint32 zoneID,       // Banners unique ID in Advertising Service, trial zone is 117145 or 31848 or 16741
                                                       const string placeholder)  // Name of placeholder image (must be in resources/textures path)
   {
   bbBanner* ptr = new bbBanner(x, y, width, height, seconds, zoneID, placeholder);
// TODO: Finish it
//   if (ptr->advertisement)
//      banners.

   return shared_ptr<monetization::Banner>((monetization::Banner*) ptr);
   }
#endif

   }
}
