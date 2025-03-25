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

// Requires Windows SDK
// https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk

// Direct3D12 Programming Guide
// https://msdn.microsoft.com/en-us/library/windows/desktop/dn899120(v=vs.85).aspx

// DXGI Overview
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205075(v=vs.85).aspx

// D3D12 Helper Structures
// https://msdn.microsoft.com/en-us/library/windows/desktop/mt186617(v=vs.85).aspx

// TODO: To build with Direct3D12, project configuration is forced to build against Windows 10 SDK and minimum platform is Windows 10 
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_5.h>   // v1.5 introduces support for VRR (Variable Refresh Rate)
#include <D3Dcompiler.h>
#include <DirectXMath.h>

// TODO: WA for missing D3D12 define (the same value as D3D11)
#ifndef D3D12_STANDARD_MULTISAMPLE_PATTERN
#define D3D12_STANDARD_MULTISAMPLE_PATTERN 0xffffffff
#endif

// TODO: WA for missing defines in Windows 10 SDK.
//       (they were present in the past and now are removed)
//       It is said D3D12_ASTC_PROFILE should tell when ASTC is 
//       supported or not, but there is no information on how to query that?
//
// TODO: How to detect and re-declare missing enum values at compile time ?
//
#define DXGI_FORMAT_ASTC_4X4_TYPELESS           (DXGI_FORMAT)133
#define DXGI_FORMAT_ASTC_4X4_UNORM              (DXGI_FORMAT)134
#define DXGI_FORMAT_ASTC_4X4_UNORM_SRGB         (DXGI_FORMAT)135
#define DXGI_FORMAT_ASTC_5X4_TYPELESS           (DXGI_FORMAT)137
#define DXGI_FORMAT_ASTC_5X4_UNORM              (DXGI_FORMAT)138
#define DXGI_FORMAT_ASTC_5X4_UNORM_SRGB         (DXGI_FORMAT)139
#define DXGI_FORMAT_ASTC_5X5_TYPELESS           (DXGI_FORMAT)141
#define DXGI_FORMAT_ASTC_5X5_UNORM              (DXGI_FORMAT)142
#define DXGI_FORMAT_ASTC_5X5_UNORM_SRGB         (DXGI_FORMAT)143
#define DXGI_FORMAT_ASTC_6X5_TYPELESS           (DXGI_FORMAT)145
#define DXGI_FORMAT_ASTC_6X5_UNORM              (DXGI_FORMAT)146
#define DXGI_FORMAT_ASTC_6X5_UNORM_SRGB         (DXGI_FORMAT)147
#define DXGI_FORMAT_ASTC_6X6_TYPELESS           (DXGI_FORMAT)149
#define DXGI_FORMAT_ASTC_6X6_UNORM              (DXGI_FORMAT)150
#define DXGI_FORMAT_ASTC_6X6_UNORM_SRGB         (DXGI_FORMAT)151
#define DXGI_FORMAT_ASTC_8X5_TYPELESS           (DXGI_FORMAT)153
#define DXGI_FORMAT_ASTC_8X5_UNORM              (DXGI_FORMAT)154
#define DXGI_FORMAT_ASTC_8X5_UNORM_SRGB         (DXGI_FORMAT)155
#define DXGI_FORMAT_ASTC_8X6_TYPELESS           (DXGI_FORMAT)157
#define DXGI_FORMAT_ASTC_8X6_UNORM              (DXGI_FORMAT)158
#define DXGI_FORMAT_ASTC_8X6_UNORM_SRGB         (DXGI_FORMAT)159
#define DXGI_FORMAT_ASTC_8X8_TYPELESS           (DXGI_FORMAT)161
#define DXGI_FORMAT_ASTC_8X8_UNORM              (DXGI_FORMAT)162
#define DXGI_FORMAT_ASTC_8X8_UNORM_SRGB         (DXGI_FORMAT)163
#define DXGI_FORMAT_ASTC_10X5_TYPELESS          (DXGI_FORMAT)165
#define DXGI_FORMAT_ASTC_10X5_UNORM             (DXGI_FORMAT)166
#define DXGI_FORMAT_ASTC_10X5_UNORM_SRGB        (DXGI_FORMAT)167
#define DXGI_FORMAT_ASTC_10X6_TYPELESS          (DXGI_FORMAT)169
#define DXGI_FORMAT_ASTC_10X6_UNORM             (DXGI_FORMAT)170
#define DXGI_FORMAT_ASTC_10X6_UNORM_SRGB        (DXGI_FORMAT)171
#define DXGI_FORMAT_ASTC_10X8_TYPELESS          (DXGI_FORMAT)173
#define DXGI_FORMAT_ASTC_10X8_UNORM             (DXGI_FORMAT)174
#define DXGI_FORMAT_ASTC_10X8_UNORM_SRGB        (DXGI_FORMAT)175
#define DXGI_FORMAT_ASTC_10X10_TYPELESS         (DXGI_FORMAT)177
#define DXGI_FORMAT_ASTC_10X10_UNORM            (DXGI_FORMAT)178
#define DXGI_FORMAT_ASTC_10X10_UNORM_SRGB       (DXGI_FORMAT)179
#define DXGI_FORMAT_ASTC_12X10_TYPELESS         (DXGI_FORMAT)181
#define DXGI_FORMAT_ASTC_12X10_UNORM            (DXGI_FORMAT)182
#define DXGI_FORMAT_ASTC_12X10_UNORM_SRGB       (DXGI_FORMAT)183
#define DXGI_FORMAT_ASTC_12X12_TYPELESS         (DXGI_FORMAT)185
#define DXGI_FORMAT_ASTC_12X12_UNORM            (DXGI_FORMAT)186
#define DXGI_FORMAT_ASTC_12X12_UNORM_SRGB       (DXGI_FORMAT)187

#endif

#endif
