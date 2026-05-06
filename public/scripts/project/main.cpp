#include "Ngine.h"
using namespace en;

void sample_ClearScreen(const char* title);

int main(int argc, const char* argv[])
{
    // You're ready to go!
    // Consider below example for smooth start
    sample_ClearScreen("Simple rendering");

    Scheduler->shutdown();
    return 0;
}

void sample_ClearScreen(const char* title)
{
    *Log << title << std::endl;

    std::shared_ptr<GpuDevice> gpu = Graphics->primaryDevice();
   
    // Position Window at the center of the primary display
    std::shared_ptr<Display> display = Graphics->primaryDisplay();
    uint32v2 resolution = display->resolution();
    uint32v2 size       = uint32v2(1280, 720);
    uint32v2 position   = uint32v2((resolution.x - size.x) / 2, (resolution.y - size.y) / 2);

    // Create Window
    WindowSettings settings;
    settings.display      = display;
    settings.position     = position;  // Ignored in Fullscreen.
    settings.size         = size; 
    settings.format       = Format::BGRA_8;
    settings.mode         = Windowed;
    settings.verticalSync = false;

    // Make sure that desired size of rendering area can be fit in case of windowed mode
    if (settings.mode == Windowed)
    {
        uint32v4 borders = windowBorders();

        uint32 maxWidth  = resolution.width  - borders.left - borders.rigth;
        uint32 maxHeight = resolution.height - borders.top  - borders.bottom;

        if (size.width > maxWidth ||
            size.height > maxHeight)
        {
            if (size.width  > maxWidth)  settings.size.width  = maxWidth;
            if (size.height > maxHeight) settings.size.height = maxHeight;

            settings.position = uint32v2(borders.left, borders.top);
        }
    }

    Window* window = gpu->createWindow(settings, std::string(title));
    assert( window );
    window->active();


    // Sample


    // Render Pass describing color to which Color Attachment will be cleared to
    ColorAttachment* attachment = gpu->createColorAttachment(settings.format);
    attachment->onLoad(LoadOperation::Clear, float4(1.0f, 0.5f, 0.0f, 0.0f));
    attachment->onStore(StoreOperation::Store);
   
    RenderPass* renderPass = gpu->createRenderPass(*attachment);

    // Semaphores that synchronize:
    // finish of presentment with beginning of GPU work, and 
    // finish of GPU work with beginning of presentment.
    std::shared_ptr<Semaphore> waitForSwapChain = gpu->createSemaphore();
    std::shared_ptr<Semaphore> waitForRendering = gpu->createSemaphore();
   
    // Cached temporary resources (released after 5 frames)
    #define CommandBufferCacheSize 5
    std::shared_ptr<CommandBuffer> command[CommandBufferCacheSize];
    TextureView*                   swapChainView[CommandBufferCacheSize];
    std::shared_ptr<Framebuffer> framebuffer[CommandBufferCacheSize];

    for(uint32 i=0; i<CommandBufferCacheSize; ++i)
    {
        swapChainView[i] = nullptr;
    }

    std::shared_ptr<Keyboard> keyboard = nullptr;
    assert( Input->available(IO::Keyboard) );
    keyboard = Input->keyboard();

    // This loop is completly non blocking. CPU and GPU execute asynchronously and never synchronize.
    bool running = true;
    while (running)
    {
        // Gather latest input state
        Input->update();

        // Input events
        Event* event = nullptr;
        while (Input->pullEvent(event))
        {
            if (event->type == AppClose)
            {
                running = false;
            }

            delete event;
        }

        // Input state
        if (keyboard->pressed(Key::Esc))
        {
            running = false;
        }

        // Rendering
        uint32 frame = window->frame();
        uint32 id    = frame % CommandBufferCacheSize;

        command[id] = gpu->createCommandBuffer();
         
        // Swap-Chain surfaces are owned by window so they don't need to be cached
        Texture* swapChainSurface = window->surface(waitForSwapChain.get());

        command[id]->start(waitForSwapChain.get());
        // During first time use, Swap-Chain Surfaces are in Undefined access 
        // state. Later they are recovered from previous Present state.
        command[id]->barrier(*swapChainSurface,
                             frame < CommandBufferCacheSize ? static_cast<TextureAccess>(0) : TextureAccess::Present,
                             TextureAccess::RenderTargetWrite);

        delete swapChainView[id];
        swapChainView[id] = swapChainSurface->view();
        framebuffer[id] = renderPass->createFramebuffer(settings.size, swapChainView[id]);

        command[id]->startRenderPass(*renderPass, *framebuffer[id]);

        // Here you would place rendering commands to be encoded for execution.
        // Currently we create Command Buffer with empty Render Pass, thus the
        // only operations that are performed on Color Attachments are load
        // and store operations. In this case it's clearing attached texture
        // from Swap-Chain to given color.

        // Finish Render Pass 
        command[id]->endRenderPass();
      
        // Make Swap-Chain surface presentable
        command[id]->barrier(*swapChainSurface,
                             TextureAccess::RenderTargetWrite,
                             TextureAccess::Present);
      
        // Finish encoding and Display when processed
        command[id]->commit(waitForRendering.get());
        window->present(waitForRendering.get());
    }
 
    // CommandBuffer is not released here, as it may still be processed by the GPU.
    // GpuDevice keeps it in it's Garbage Collector until completion Fence is signaled,
    // then it is safely and in automatic way destroyed.
    for(uint32 i=0; i<CommandBufferCacheSize; ++i)
    {
        command[i]->waitUntilCompleted();

        framebuffer[i] = nullptr;
        delete swapChainView[i];
        command[i] = nullptr;
    }

    delete renderPass;
    delete attachment;
    delete window;
    gpu = nullptr;

    *Log << "Sample closing.\n";
}