/*

 Ngine v5.0
 
 Module      : D3D12 Window.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/d3d12/dx12Window.h"

#if defined(EN_MODULE_RENDERER_DIRECT3D12)

#include "core/log/log.h"
#include "core/memory/alignedAllocator.h"
#include "core/rendering/d3d12/dx12Validate.h"
#include "core/rendering/d3d12/dx12Device.h"
#include "core/rendering/d3d12/dx12Texture.h"
#include "parallel/scheduler.h"
#include "utilities/strings.h"

namespace en
{
namespace gpu
{ 

WindowD3D12::WindowD3D12(
        Direct3D12Device* _gpu,
        const std::shared_ptr<CommonDisplay> selectedDisplay,
        const uint32v2 selectedResolution,
        const WindowSettings& settings,
        const std::string title) :
    gpu(_gpu),
    // Create native OS window or assert.
    winWindow(std::dynamic_pointer_cast<winDisplay>(selectedDisplay), selectedResolution, settings, title)
{
    // Calculate amount of backing images in Swap-Chain
    //--------------------------------------------------
   
    // Typical Swap-Chain consist of 3 surfaces, one is presented on display, next is
    // queued to be presented, and the third one is used by application for rendering.
    // We use 4, to be able to use Mailbox mode rendering (where we can replace last
    // waiting frame with more recent one at any time, and still have one frame to query).
    swapChainImages = 4;
   
    // TODO: In future check what's the most optimal count

    // Verify requested Swap-Chain resolution
    //----------------------------------------

    // Final determined resolution of backing surface (may be smaller than window resolution)
    bool enableScaling = false;
    uint32v2 swapChainResolution = selectedResolution;

    // In windowed mode Swap-Chain resolution can differ from Window size (be smaller)
    if ( (settings.mode != Fullscreen) &&
         (settings.resolution.width  > 0) &&
         (settings.resolution.width  < selectedResolution.width) &&
         (settings.resolution.height > 0) &&
         (settings.resolution.height < selectedResolution.height) )
    {
        // Windowing System allows scaling of Swap-Chain surfaces to window size
        swapChainResolution = settings.resolution;
        enableScaling = true;
    }

    // Create Swap-Chain surface attached to Window
    //----------------------------------------------

    assert( settings.format == Format::BGRA_8 ||
            settings.format == Format::RGBA_8 ||
            settings.format == Format::RGBA_16_hf );

    // More details about presentation modes:
    // https://www.youtube.com/watch?v=E3wTajGZOsA
    //
    DXGI_SWAP_CHAIN_DESC1 desc;
    desc.Width              = swapChainResolution.width;
    desc.Height             = swapChainResolution.height;
    desc.Format             = TranslateTextureFormat[underlyingType(settings.format)];
    desc.Stereo             = false;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // DXGI_USAGE_BACK_BUFFER ?
    desc.BufferCount        = swapChainImages;
    desc.Scaling            = enableScaling ? DXGI_SCALING_ASPECT_RATIO_STRETCH : DXGI_SCALING_NONE; // Will use DWM scaling.
    desc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // Flip discard is optimal, and compatible with UWP
    desc.AlphaMode          = DXGI_ALPHA_MODE_IGNORE;             // DXGI_ALPHA_MODE_STRAIGHT
    desc.Flags              = 0;                                  // We want Vsync'ed presentment with discarding extra frames.
                                                                  // DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING - disables VSync

    // TODO: Query Display refresh rate!
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC descFullscreen;
    descFullscreen.RefreshRate.Numerator   = 60;                  // Assume 60Hz display
    descFullscreen.RefreshRate.Denominator = 1;
    descFullscreen.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    descFullscreen.Scaling                 = DXGI_MODE_SCALING_CENTERED; // No HW scaling in display, when resolution doesn't match native on LCD
    descFullscreen.Windowed                = false;

    // Create Swap-Chain connecting Window to Device
    //-----------------------------------------------
   
    Direct3DAPI* api = reinterpret_cast<Direct3DAPI*>(Graphics.get());

    // Engine assumes that queue family handling QueueType::Universal is supporting Present.
    // Presenting is always performed from first queue of type QueueType::Universal (queue 0).
    // TODO: Create Windows explicitly assigned to specific CommandQueue
    ValidateCom( api->factory->CreateSwapChainForHwnd(gpu->queue[0],
                                                      hWnd,
                                                      &desc,
                                                      settings.mode == WindowMode::Fullscreen ? &descFullscreen : nullptr,
                                                      nullptr,
                                                      &swapChain) )

#if defined(EN_DEBUG)
    // Name Swap-Chain for debugging
    std::string name("SwapChain1");
    swapChain->SetPrivateData( WKPDID_D3DDebugObjectName, name.length(), name.c_str() );
#endif

    // After creating Swap-Chain v1, get access to latest v4
    swapChain->QueryInterface(&swapChain4);

#if defined(EN_DEBUG)
    // Name Swap-Chain4 for debugging
    name = "SwapChain4";
    swapChain4->SetPrivateData( WKPDID_D3DDebugObjectName, name.length(), name.c_str() );
#endif

    // Create array of textures backed by Swap-Chain images
    //------------------------------------------------------
 
    // Create textures backed with handles
    TextureState textureState = TextureState(TextureType::Texture2D, 
                                             settings.format, 
                                             TextureUsage::RenderTargetWrite,
                                             swapChainResolution.width,
                                             swapChainResolution.height);

    swapChainTexture = new TextureD3D12*[swapChainImages];
    for(uint32 i=0; i<swapChainImages; ++i)
    {
        TextureD3D12* texture = new TextureD3D12(gpu, textureState);
        swapChain->GetBuffer(i, IID_PPV_ARGS(&texture->handle));  // __uuidof(ID3D12Resource), reinterpret_cast<void**>(&texture->handle)
        swapChainTexture[i] = texture;

#if defined(EN_DEBUG)
        // Name Swap-Chain Surface for debugging
        name = "SwapChainTexture N: ";
        name += stringFrom(i);
        texture->handle->SetPrivateData( WKPDID_D3DDebugObjectName, name.length(), name.c_str() );
#endif
    }

    // Set Window background color to Transparent Black
    const float4 color(0.0f, 0.0f, 0.0f, 0.0f);
    swapChain->SetBackgroundColor(reinterpret_cast<const DXGI_RGBA *>(&color));
   
    _resolution = swapChainResolution;
}

WindowD3D12::~WindowD3D12()
{
    for(uint32 i=0; i<swapChainImages; ++i)
    {
        delete swapChainTexture[i];
    }

    delete [] swapChainTexture;

    // Inherited interface of the same Swap-Chain object, thus its not released.
    swapChain4 = nullptr;
   
    swapChain->Release();
    swapChain = nullptr;
}

void WindowD3D12::resize(const uint32v2 size)
{
    // TODO: Finish !

    // See: https://msdn.microsoft.com/en-us/library/windows/desktop/dn903945(v=vs.85).aspx#creating_swap_chains
    //
    // IDXGISwapChain::ResizeTarget
    // IDXGISwapChain::ResizeBuffers
    //

    assert( 0 );
}

Texture* WindowD3D12::surface(const Semaphore* surfaceAvailableSemaphore)
{
    // TODO: Is there a way to sync on GPU side CB execution with Swap-Chain ?

    if (needNewSurface)
    {
        surfaceAcquire.lock();

        if (needNewSurface)
        {
            swapChainCurrentImageIndex = swapChain4->GetCurrentBackBufferIndex();
            needNewSurface = false;
        }

        surfaceAcquire.unlock();
    }

    return swapChainTexture[swapChainCurrentImageIndex];
}

void WindowD3D12::present(const Semaphore* surfaceRenderedSemaphore)
{
    // TODO: Is there a way to sync on GPU side CB execution with Swap-Chain ?

    surfaceAcquire.lock();
    if (!needNewSurface)
    {
        DXGI_PRESENT_PARAMETERS parameters;
        parameters.DirtyRectsCount = 0;
        parameters.pDirtyRects     = nullptr;
        parameters.pScrollRect     = nullptr;
        parameters.pScrollOffset   = nullptr;

        if (verticalSync && (_frame > 0))
        {
            swapChain->Present1(1, 0, &parameters);
        }
        else
        {
            swapChain->Present1(0, DXGI_PRESENT_RESTART, &parameters);
        }

        needNewSurface = true;
        _frame++;

        // TODO: Measure here Window rendering time (average between present calls) ?
    }

    surfaceAcquire.unlock();
}

struct taskCreateWindowState
{
    Direct3D12Device* gpu;
    std::shared_ptr<CommonDisplay> selectedDisplay;
    uint32v2 selectedResolution;
    const WindowSettings* settings;
    std::string title;
    WindowD3D12* result;

    taskCreateWindowState(const WindowSettings* _settings);
};

taskCreateWindowState::taskCreateWindowState(const WindowSettings* _settings) :
    gpu(nullptr),
    selectedDisplay(nullptr),
    selectedResolution(),
    settings(_settings),
    title(),
    result(nullptr)
{
}

void taskCreateWindow(void* data)
{
    taskCreateWindowState& state = *(taskCreateWindowState*)(data);

    state.result = new WindowD3D12(state.gpu, state.selectedDisplay, state.selectedResolution, *state.settings, state.title);
}

Window* Direct3D12Device::createWindow(const WindowSettings& settings, const std::string title)
{
    WindowD3D12* result = nullptr;

    // Select destination display
    std::shared_ptr<CommonDisplay> display;
    if (settings.display)
    {
        display = std::dynamic_pointer_cast<CommonDisplay>(settings.display);
    }
    else
    {
        display = std::dynamic_pointer_cast<CommonDisplay>(Graphics->primaryDisplay());
    }
      
    // Checking if app wants to use default resolution
    bool useNativeResolution = false;
    if (settings.size.width  == 0 ||
        settings.size.height == 0)
    {
        useNativeResolution = true;
    }

    // Select resolution to use (to which display should switch in Fullscreen mode)
    uint32v2 selectedResolution = settings.size;
    if (useNativeResolution)
    {
        selectedResolution = display->_resolution;
    }

    // Verify apps custom size in Fullscreen mode is supported, and that app is not using
    // size and resolution at once in this mode.
    if (settings.mode == Fullscreen)
    {
        if (!useNativeResolution)
        {
            // Verify that requested resolution is supported by display
            bool validResolution = false;
            for(uint32 i=0; i<display->modesCount; ++i)
            {
                if (selectedResolution == display->modeResolution[i])
                {
                    validResolution = true;
                }
            }

            if (!validResolution)
            {
                enLog << "Error! Requested window size for Fullscreen mode is not supported by selected display.\n";
                return result;
            }
        }
         
        // Cannot use Display HW Scaler to emulate smaller resolution and Swap-Chain upsampling at the same time.
        if (settings.resolution.x != 0 ||
            settings.resolution.y != 0)
        {
            enLog << "Error! In Fullscreen mode resolution shouldn't be used, use size setting instead.\n";
            return result;
        }
    }
    else // Verify that desired resolution together with window borders and bars fits on the display
    if (settings.mode == Windowed)
    {
        uint32v4 borders = windowBorders(); // width and height that needs to be added to content size

        if ((selectedResolution.width  + borders.left + borders.rigth) > display->_resolution.width ||
            (selectedResolution.height + borders.top + borders.bottom) > display->_resolution.height)
        {
            enLog << "Error! In Windowed mode, final window size (requested size plus borders) is greater than selected display native resolution.\n";
            return result;
        }
    }

    taskCreateWindowState state(&settings);
    state.gpu                = this;
    state.selectedDisplay    = display;
    state.selectedResolution = selectedResolution;
    state.title              = title;

    TaskState taskState;

    // Window needs to be created on main thread
    en::Scheduler->runOnMainThread(taskCreateWindow, (void*)&state, &taskState, true);
    en::Scheduler->wait(&taskState);

    return state.result;
}

} // en::gpu
} // en

#endif
