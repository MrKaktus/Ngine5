/*

 Ngine v5.0
 
 Module      : Android specific code.
 Requirements: none
 Description : Declares defines for safe engine
               startup in background before
               passing control to user code.

*/

#ifndef EN_PLATFORM_ANDROID_INIT
#define EN_PLATFORM_ANDROID_INIT

#ifdef EN_PLATFORM_ANDROID
// Changes entry point name in user code, so 
// that Ngine could take control over it at 
// the beginning and initialize all required
// stuff.
#define main          ApplicationMainC   
#define android_main  ApplicationMainC
#endif

#endif