/*

 Ngine v5.0
 
 Module      : D3D12 Buffer.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Buffer.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/rendering/d3d12/dx12Validate.h"
#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
namespace gpu
{

BufferD3D12::BufferD3D12(
        HeapD3D12& _heap,
        ID3D12Resource* _handle,
        const BufferType _type,
        const uint64 _offset,
        const uint64 _size) :
    heap(_heap),
    handle(_handle),
    offset(_offset),
    range{0, 0},
    signature(nullptr),
    signatureIndexed(nullptr),
    CommonBuffer(_type, _size)
{
    // For Indirect draw buffer, D3D12 requires detailed signatures
    // to be created (as D3D12 allows mixed commands to be encoded
    // in Indirect buffers). This is cumbersome for simple Indirect
    // buffer usages. See Indirect Draw method in CommandBuffer for
    // more information.
    if (apiType == BufferType::Indirect)
    {
        UINT MaxCommandCount = 0;
        D3D12_INDIRECT_ARGUMENT_DESC* argDescs = nullptr;

        // Signature for Indirect Indexed Draws
        {
            MaxCommandCount = size / sizeof(IndirectIndexedDrawArgument);
      
            argDescs = new D3D12_INDIRECT_ARGUMENT_DESC[MaxCommandCount];
            for(uint32 i=0; i<MaxCommandCount; ++i)
            {
                argDescs[i].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
            }

            // IndirectIndexedDrawArgument can be directly cast to D3D12_DRAW_INDEXED_ARGUMENTS.
            D3D12_COMMAND_SIGNATURE_DESC desc;
            desc.ByteStride       = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
            desc.NumArgumentDescs = MaxCommandCount;
            desc.pArgumentDescs   = argDescs;
            desc.NodeMask         = 0;  // Multi-GPU clusters are not supported yet.
      
            // Engine currently doesn't support updates of DescriptorSets. 
            // When it will, RootSignature pointer needs to be passed here.
            Validate( heap.gpu, CreateCommandSignature(&desc,
                                                       nullptr,
                                                       IID_PPV_ARGS(&signatureIndexed)) ) // __uuidof(ID3D12CommandSignature), reinterpret_cast<void**>(&signature)
        }

        // Signature for Indirect draws
        {
            MaxCommandCount = size / sizeof(IndirectDrawArgument);
      
            argDescs = new D3D12_INDIRECT_ARGUMENT_DESC[MaxCommandCount];
            for(uint32 i=0; i<MaxCommandCount; ++i)
            {
                argDescs[i].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
            }

            // IndirectDrawArgument can be directly cast to D3D12_DRAW_ARGUMENTS.
            D3D12_COMMAND_SIGNATURE_DESC desc;
            desc.ByteStride       = sizeof(D3D12_DRAW_ARGUMENTS);
            desc.NumArgumentDescs = MaxCommandCount;
            desc.pArgumentDescs   = argDescs;
            desc.NodeMask         = 0;  // Multi-GPU clusters are not supported yet.
      
            // Engine currently doesn't support updates of DescriptorSets. 
            // When it will, RootSignature pointer needs to be passed here.
            Validate( heap.gpu, CreateCommandSignature(&desc,
                                                       nullptr,
                                                       IID_PPV_ARGS(&signature)) ) // __uuidof(ID3D12CommandSignature), reinterpret_cast<void**>(&signature)
        }
   
        delete [] argDescs;
    }
}

BufferD3D12::~BufferD3D12()
{
    if (apiType == BufferType::Indirect)
    {
        ValidateCom( signature->Release() )
        signature = nullptr;

        ValidateCom( signatureIndexed->Release() )
        signatureIndexed = nullptr;
    }

    assert( handle );
    ValidateCom( handle->Release() )
    handle = nullptr;

    // Deallocate from the Heap (let Heap allocator know that memory region is available again)
    heap.allocator->deallocate(offset, size);
}
   
// Map whole buffer for read or write depending on backing Heap type
volatile void* BufferD3D12::map(void)
{
    // Buffers can be mapped only on Upload, Download and Immediate Heaps.
    assert( heap._usage == MemoryUsage::Upload   ||
            heap._usage == MemoryUsage::Download ||
            heap._usage == MemoryUsage::Immediate );

    // Range mapped for read (by default mapped for write)
    range.Begin = 0; 
    range.End   = 0;
    if (heap._usage == MemoryUsage::Download)
    {
        range.Begin = 0; 
        range.End   = size;
    }

    void* pointer = nullptr;
    ValidateCom( handle->Map(0, &range, &pointer) )

    return pointer;
}

volatile void* BufferD3D12::map(const uint64 _offset, const uint64 _size)
{
    assert( _offset + _size <= size );

    // Buffers can be mapped only on Upload, Download and Immediate Heaps.
    assert( heap._usage == MemoryUsage::Upload   ||
            heap._usage == MemoryUsage::Download ||
            heap._usage == MemoryUsage::Immediate );

#ifdef EN_ARCHITECTURE_X86    
    // TODO: 
    // SIZE_T is 32bit on 32bit systems (64bit on 64bit systems),
    // thus on 32bit target, we need to limit settings.maxBufferSize
    // to 4GB.
    assert( _offset + _size <= 0xFFFFFFFF );
#endif

    // If possible, map this buffer only for write, otherwise for CPU read
    if (heap._usage != MemoryUsage::Download &&
        _offset == 0 &&
        _size == size)
    {
        range.Begin = 0; 
        range.End   = 0;
    }
    else
    {
        range.Begin = static_cast<SIZE_T>(_offset); 
        range.End   = static_cast<SIZE_T>(_offset + _size);
    }

    void* pointer = nullptr;
    ValidateCom( handle->Map(0, &range, &pointer) )
   
    return pointer;
}
   
void BufferD3D12::unmap(void)
{
    ValidateComNoRet( handle->Unmap(0, &range) )
}
   
// Create unformatted generic buffer of given type and size.
// This method can still be used to create Vertex or Index buffers,
// but it's adviced to use ones with explicit formatting.
Buffer* HeapD3D12::createBuffer(const BufferType type, const uint32 size)
{
    BufferD3D12* result = nullptr;

    // Buffers cannot be created in Heaps dedicated to Texture storage
    assert( _usage != MemoryUsage::Tiled   &&
            _usage != MemoryUsage::Renderable );

    // Buffer descriptor
    D3D12_RESOURCE_DESC desc;
    desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment          = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    desc.Width              = static_cast<UINT64>(size);
    desc.Height             = 1u;
    desc.DepthOrArraySize   = 1u;
    desc.MipLevels          = 1u;
    desc.Format             = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count   = 1u;
    desc.SampleDesc.Quality = 0u;
    desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

    // Acquire buffer alignment and size
    D3D12_RESOURCE_ALLOCATION_INFO allocInfo;
    allocInfo = gpu->device->GetResourceAllocationInfo(0u /* currently no multi-GPU support */, 1u, &desc);

    // Find memory region in the Heap where this texture can be placed.
    // If allocation succeeded, texture is mapped to given offset.
    uint64 offset = 0;
    if (!allocator->allocate(static_cast<uint64>(allocInfo.SizeInBytes),
                             static_cast<uint64>(allocInfo.Alignment),
                             offset))
    {
        return result;
    }

    // Patch descriptor with proper alignment and rounded-up size
    desc.Alignment = allocInfo.Alignment;
    desc.Width     = allocInfo.SizeInBytes;

    // Initial resource state is dictate by type of backing Heap
    D3D12_RESOURCE_STATES initState;
    if (_usage == MemoryUsage::Upload ||
        _usage == MemoryUsage::Immediate)
    {
        initState = D3D12_RESOURCE_STATE_GENERIC_READ;
    }
    else
    if (_usage == MemoryUsage::Download ||
        _usage == MemoryUsage::Linear)
    {
        initState = D3D12_RESOURCE_STATE_COPY_DEST;
    }

    ID3D12Resource* bufferHandle = nullptr;

    Validate( gpu, CreatePlacedResource(handle,
                                        static_cast<UINT64>(offset),
                                        &desc,
                                        initState,
                                        nullptr,                       // Clear value - currently not supported
                                        IID_PPV_ARGS(&bufferHandle)) ) // __uuidof(ID3D12Resource), reinterpret_cast<void**>(&bufferHandle)
      
    if ( SUCCEEDED(gpu->lastResult[en::currentThreadId()]) )
    {
        result = new BufferD3D12(*this,
                                 bufferHandle,
                                 type,
                                 offset,
                                 static_cast<uint64>(allocInfo.SizeInBytes) );
    }

    return result;
}
   
} // en::gpu
} // en
#endif
