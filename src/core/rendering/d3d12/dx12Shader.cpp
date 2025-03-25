/*

 Ngine v5.0
 
 Module      : D3D12 Shader.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Shader.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/rendering/state.h"
#include "core/rendering/d3d12/dx12Validate.h"
#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
namespace gpu
{

ShaderD3D12::ShaderD3D12(const ShaderStage _stage, const uint8* binary, const uint32 length) :
    stage(_stage)
{
    // Keeps local copy of shader binary
    state.pShaderBytecode = allocate<uint8>(length, 4096);
    state.BytecodeLength  = length;

    memcpy((void*)(state.pShaderBytecode), binary, length);
}
   
ShaderD3D12::~ShaderD3D12()
{
    deallocate<uint8>((uint8*)state.pShaderBytecode);
}

// HLSL
// FXC (DX ASM)
// frontend compilation
// pipeline creation (final compilation)

// HLSL 5.1 introduced with D3D12 and D3D11.3 (Descriptor Sets etc.)
// 
// HLSL -> FXC -> D3DCompile
// https://blogs.msdn.microsoft.com/chuckw/2012/05/07/hlsl-fxc-and-d3dcompile/
// ( also project configuration for run-time compile and compiler DLL)
//
// Specifying RootSignature in HLSL:
// - https://msdn.microsoft.com/en-us/library/windows/desktop/dn913202(v=vs.85).aspx
//
// Offline and Online shader compilation:
// - https://msdn.microsoft.com/en-us/library/windows/desktop/bb509710(v=vs.85).aspx
//
// FXC flags:
// - https://msdn.microsoft.com/en-us/library/windows/desktop/bb509709(v=vs.85).aspx

// D3DCOMPILE_DEBUG
// D3DCOMPILE_SKIP_VALIDATION                      
// D3DCOMPILE_SKIP_OPTIMIZATION                    
// D3DCOMPILE_PACK_MATRIX_ROW_MAJOR                
// D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR             
// D3DCOMPILE_PARTIAL_PRECISION                    
// D3DCOMPILE_FORCE_VS_SOFTWARE_NO_OPT             
// D3DCOMPILE_FORCE_PS_SOFTWARE_NO_OPT             
// D3DCOMPILE_NO_PRESHADER                         
// D3DCOMPILE_AVOID_FLOW_CONTROL                   
// D3DCOMPILE_PREFER_FLOW_CONTROL                  
// D3DCOMPILE_ENABLE_STRICTNESS                    
// D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY       
// D3DCOMPILE_IEEE_STRICTNESS                      
// D3DCOMPILE_OPTIMIZATION_LEVEL0                  
// D3DCOMPILE_OPTIMIZATION_LEVEL1                  
// D3DCOMPILE_OPTIMIZATION_LEVEL2                  
// D3DCOMPILE_OPTIMIZATION_LEVEL3                  
// D3DCOMPILE_WARNINGS_ARE_ERRORS                  
// D3DCOMPILE_RESOURCES_MAY_ALIAS                  
// D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES   
// D3DCOMPILE_ALL_RESOURCES_BOUND                  

static const LPCSTR TranslateTargetType[underlyingType(ShaderStage::Count)] =
{
    "vs_5_1",  // Vertex
    "hs_5_1",  // Control
    "ds_5_1",  // Evaluation
    "gs_5_1",  // Geometry
    "fs_5_1"   // Fragment
};

std::shared_ptr<Shader> Direct3D12Device::createShader(const ShaderStage stage, const std::string& source)
{
    std::shared_ptr<ShaderD3D12> result = nullptr; 

#if defined(EN_DEBUG)
    // Enable better shader debugging with the graphics debugging tools
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    // ThrowIfFailed(D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &binary, nullptr));

    ID3DBlob* binary = nullptr;
    ID3DBlob* errors = nullptr;

    // Compile shader from HLSL
    ValidateCom( D3DCompile(source.c_str(),
                           source.length(),
                           nullptr,            // Optional: Shader source name for debugging
                           nullptr,            // Optional: Shader macros - D3D_SHADER_MACRO Shader_Macros[] = { "zero", "0", NULL, NULL };
                           nullptr,            // Optional: Pointers to included files if shader uses #include
                           nullptr,   // TODO: can be null for FX's but here they want final entrypoint!
                           TranslateTargetType[underlyingType(stage)],
                           compileFlags,
                           0,                  // Effect: Compile Effect Flags
                           &binary,
                           &errors) )

    if (errors)
    {
        // Log compilation error message
        std::string errorLog("Error: D3D12 HLSL compilation error:\n");
        errorLog += std::string(reinterpret_cast<char*>(errors->GetBufferPointer()), errors->GetBufferSize());
        errorLog += "\n\n";
        Log << errorLog;
    }
    else
    if (binary)
    {
        result = std::make_shared<ShaderD3D12>(stage,
                                               reinterpret_cast<const uint8*>(binary->GetBufferPointer()),
                                               binary->GetBufferSize());
    }

    return result;
}

std::shared_ptr<Shader> Direct3D12Device::createShader(const ShaderStage stage, const uint8* data, const uint64 size)
{
    return std::make_shared<ShaderD3D12>(stage, data, size);
}

} // en::gpu
} // en
#endif
