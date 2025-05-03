



namespace en
{
namespace xr
{

// MACROS: Safe OpenXR function binding with fallback
//         Both Macros should be used only inside oxrInterface class methods.

#if defined(EN_PLATFORM_LINUX)
#define LoadProcAddress dlsym
#endif

#if defined(EN_PLATFORM_WINDOWS)
#define LoadProcAddress GetProcAddress
#endif

#define LoadFunction(function)                                              \
{                                                                           \
    function = (PFN_xr##function)LoadProcAddress(library, #function);       \
    if (function == nullptr)                                                \
    {                                                                       \
        function = (PFN_##function) &unbindedOpenXRFunctionHandler;         \
        enLog << "Error: Cannot bind function " << #function << std::endl;    \
    }                                                                       \
}

#define LoadGlobalFunction(function)                                                          \
{                                                                                             \
    XrResult result = xrGetInstanceProcAddr(nullptr,                                          \
                                            #function,                                        \
                                            (PFN_xrVoidFunction*)&function);                  \
                                                                                              \
    if (en::xr::IsError(result))                                                              \
    {                                                                                         \
        assert(0);                                                                            \
    }                                                                                         \
    en::xr::IsWarning(result);                                                                \
    if (function == nullptr)                                                                  \
    {                                                                                         \
        function = (PFN_##function)&unbindedOpenXRFunctionHandler;                            \
        enLog << "Error: Cannot bind global function " << #function << std::endl;               \
    }                                                                                         \
}

#define LoadInstanceFunction(_interface, function)                                            \
{                                                                                             \
    uint32 threadId = currentThreadId();                                                      \
    assert(threadId < MaxSupportedThreads);                                                   \
    _interface->lastResult[threadId] = xrGetInstanceProcAddr(_interface->instance,            \
                                                             #function,                       \
                                                             (PFN_xrVoidFunction*)&function); \
    if (en::xr::IsError(_interface->lastResult[threadId]))                                    \
    {                                                                                         \
        assert(0);                                                                            \
    }                                                                                         \
    en::xr::IsWarning(_interface->lastResult[threadId]);                                      \
    if (function == nullptr)                                                                  \
    {                                                                                         \
        function = (PFN_##function)&unbindedOpenXRFunctionHandler;                            \
        enLog << "Error: Cannot bind instance function " << #function << std::endl;             \
    }                                                                                         \
}

} // en::xr
} // en
