/*

 Ngine v5.0
 
 Module      : Main engine initialization and deinitialization path.
 Requirements: none
 Description : Starts execution of engine code. After
               all start up procedures are finished,
               it passes control to user application.
               It also handles exit from user program
               and safe clean-up.

*/

#ifndef ENG_PLATFORM_COMMON_MAIN
#define ENG_PLATFROM_COMMON_MAIN

// Container for main task
struct Arguments
{
    int argc;
    const char **argv;
   
    Arguments(int _argc, const char **_argv) : argc(_argc), argv(_argv) {};
};

namespace en
{
    void init(int argc, const char **argv);  ///< Entry point for engine initialization
    void destroy(void);                      ///< Entry point for engine de-initialization
}
#endif
