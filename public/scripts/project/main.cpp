#include "Ngine.h"
using namespace en;

void sample_ClearScreen(const char* title);

int main(int argc, const char* argv[])
{
   // You're ready to go!
   // Consider below example for smooth start
   sample_ClearScreen("Simple rendering");

   return 0;
}

void sample_ClearScreen(const char* title)
{
   using namespace en::gpu;

   Log << title << std::endl;

   std::shared_ptr<GpuDevice> gpu = Graphics->primaryDevice();

   // Position Window at the center of the primary display
   std::shared_ptr<Display> display = Graphics->primaryDisplay();
   uint32v2 resolution  = display->resolution();
   uint32v2 size        = uint32v2(1280, 720);
   uint32v2 position    = uint32v2((resolution.x - size.x) / 2, (resolution.y - size.y) / 2);

   WindowSettings settings;
   settings.display  = display;
   settings.position = position;  // Ignored in Fullscreen.
   settings.size     = size;
   settings.format   = Format::BGRA_8;
   settings.mode     = Windowed;

   std::shared_ptr<Window> window = gpu->createWindow(settings, std::string(title));
   window->active();

   std::shared_ptr<ColorAttachment> attachment = gpu->createColorAttachment(settings.format, 1u);
   attachment->onLoad(LoadOperation::Clear, float4(1.0f, 0.5f, 0.0f, 0.0f));
   attachment->onStore(StoreOperation::Store);
   
   std::shared_ptr<RenderPass> renderPass = gpu->createRenderPass(*attachment, nullptr);

   std::shared_ptr<Semaphore> waitForSwapChain = gpu->createSemaphore();
   std::shared_ptr<Semaphore> waitForGPU       = gpu->createSemaphore();

   std::shared_ptr<Keyboard> keyboard = nullptr;
   assert( Input->available(IO::Keyboard) );
   keyboard = Input->keyboard();
   
   // Cached temporary resources (released after 5 frames)
   #define CommandBufferCacheSize 5
   std::shared_ptr<CommandBuffer> command[CommandBufferCacheSize];
   std::shared_ptr<Texture> swapChainSurface[CommandBufferCacheSize];
   std::shared_ptr<TextureView> swapChainView[CommandBufferCacheSize];
   std::shared_ptr<Framebuffer> framebuffer[CommandBufferCacheSize];

   // This loop is completly non blocking. CPU and GPU execute asynchronously and never synchronize.
   while(!keyboard->pressed(Key::Esc))
      {
      // Input
      Input->update();

      // Rendering
      uint32 id = window->frame() % CommandBufferCacheSize;
      
      command[id] = gpu->createCommandBuffer();
         
      swapChainSurface[id] = window->surface(waitForSwapChain.get());
      command[id]->start(waitForSwapChain.get());
      command[id]->barrier(*swapChainSurface[id],
                           uint32v2(0,1),
                           uint32v2(0,1),
                           TextureAccess::Present,
                           TextureAccess::RenderTargetWrite);

      swapChainView[id] = swapChainSurface[id]->view();
      framebuffer[id] = renderPass->createFramebuffer(settings.size, swapChainView[id]);

      command[id]->startRenderPass(renderPass, framebuffer[id]);

      // Here will be rendering in next sample . . 

      // Finish encoding and Display when processed
      command[id]->endRenderPass();
      
      command[id]->barrier(*swapChainSurface[id],
                       uint32v2(0,1),    // First mipmap
                       uint32v2(0,1),    // First layer
                       TextureAccess::RenderTargetWrite,
                       TextureAccess::Present);
      
      command[id]->commit(waitForGPU.get());
      window->present(waitForGPU.get());
      }
 
   // CommandBuffer is not released here, as it may still be processed by the GPU.
   // GpuDevice keeps it in it's Garbage Collector until completion Fence is signaled,
   // then it is safely and in automatic way destroyed.
   for(uint32 i=0; i<CommandBufferCacheSize; ++i)
      {
      command[i]->waitUntilCompleted();

      framebuffer[i] = nullptr;
      swapChainView[i] = nullptr;
      swapChainSurface[i] = nullptr;
      command[i] = nullptr;
      }

   renderPass = nullptr;
   window = nullptr;
   gpu = nullptr;

   Log << "Sample closing." << std::endl;
}