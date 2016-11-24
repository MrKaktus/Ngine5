/*

 Ngine v5.0
 
 Module      : Direct3D 12 API
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.
               This module implements OpenGL backend.

*/

#ifndef ENG_CORE_RENDERING_D3D12
#define ENG_CORE_RENDERING_D3D12

#include "core/defines.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

// https://msdn.microsoft.com/en-us/library/windows/desktop/dn899120(v=vs.85).aspx

#include <Windows.h>
#include "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/um/D3d12.h"

#endif

#endif
