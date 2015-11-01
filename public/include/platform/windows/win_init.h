/*

 Ngine v5.0
 
 Module      : Windows specific code.
 Requirements: none
 Description : Declares defines for safe engine
               startup in background before
               passing control to user code.

*/

#ifndef EN_PLATFORM_WINDOWS_INIT
#define EN_PLATFORM_WINDOWS_INIT

#ifdef EN_PLATFORM_WINDOWS
// Changes entry point name in user code, so 
// that Ngine could take control over it at 
// the beginning and initialize all required
// stuff.
#define main          ApplicationMainC  
#define WinMain       ApplicationMainW  
#endif

#endif
