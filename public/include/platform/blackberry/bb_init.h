/*

 Ngine v5.0
 
 Module      : BlackBerry specific code.
 Requirements: none
 Description : Starts execution of engine code. After 
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#ifndef EN_PLATFORM_BLACKBERRY_INIT
#define EN_PLATFORM_BLACKBERRY_INIT

#ifdef EN_PLATFORM_BLACKBERRY
// Changes entry point name in user code, so 
// that Ngine could take control over it at 
// the beginning and initialize all required
// stuff.
#define main          ApplicationMainC  
#endif

#endif