/*

 Ngine v4.0
 
 Module      : MacOS specific code.
 Requirements: none
 Description : Declares defines for safe engine
               startup in background before
               passing control to user code.

*/

#ifndef ENG_MACOS_INIT
#define ENG_MACOS_INIT

#if defined(ENG_MACOS) || defined(ENG_IPHONE)
// Changes entry point name in user code, so 
// that Ngine could take control over it at 
// the beginning and initialize all required
// stuff.
#define main          ApplicationMainC  
#endif

#endif
