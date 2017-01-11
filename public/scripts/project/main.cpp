#include "Ngine.h"
using namespace en;

/*
Things that still need to be set manually:

Solution:

Common Properties
  Startup Project : Current selection

Project (per target, here for win64):

Configuration Properties
  General
    Output Directory:       $(SolutionDir)\..\..\bin\win64\$(Configuration)\
    Intermediate Directory: $(SolutionDir)\..\..\build\win64\$(Configuration)\
    Build Log File:         .\..\..\build\win64\$(Configuration)\$(MSBuildProjectName).log

Remove ALL_BUILD and INSTALL projects from Solution.

*/

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

   Log << title << endl;

   Ptr<GpuDevice> gpu = Graphics->primaryDevice();

   // Position Window at the center of the primary display
   uint32v2 resolution = Graphics->primaryDisplay()->resolution();
   uint32v2 size       = uint32v2(1280, 720);
   uint32v2 position   = uint32v2((resolution.x - size.x) / 2, (resolution.y - size.y) / 2);

   WindowSettings settings;
   settings.display  = Graphics->primaryDisplay();
   settings.position = position;  // Ignored in Fullscreen.
   settings.size     = size;
   settings.format   = Format::BGRA_8;
   settings.mode     = Windowed;

   Ptr<Window> window = gpu->createWindow(settings, string(title));
   window->active();

   Ptr<ColorAttachment> attachment = gpu->createColorAttachment(settings.format, 1u);
   attachment->onLoad(LoadOperation::Clear, float4(1.0f, 0.5f, 0.0f, 0.0f));
   attachment->onStore(StoreOperation::Store);
   
   Ptr<RenderPass> renderPass = gpu->createRenderPass(attachment, nullptr);

   Ptr<Semaphore> waitForSwapChain = gpu->createSemaphore();
   Ptr<Semaphore> waitForGPU       = gpu->createSemaphore();

   Ptr<Keyboard> keyboard = nullptr;
   assert( Input->available(IO::Keyboard) );
   keyboard = Input->keyboard();
   
   while(!keyboard->pressed(Key::Esc))
      {
      // Input
      Input->update();

      // Rendering
      Ptr<CommandBuffer> command = gpu->createCommandBuffer();
         
      Ptr<Texture> swapChainSurface = window->surface(waitForSwapChain);
      command->start(waitForSwapChain);
      command->barrier(swapChainSurface,
                       uint32v2(0,1),
                       uint32v2(0,1),
                       TextureAccess::Present,
                       TextureAccess::RenderTargetWrite);

      Ptr<TextureView> swapChainView = swapChainSurface->view();
      Ptr<Framebuffer> framebuffer = renderPass->createFramebuffer(settings.size, swapChainView);

      command->startRenderPass(renderPass, framebuffer);

      // Here will be rendering in next sample . . 

      // Finish encoding and Display when processed
      command->endRenderPass();
      
      command->barrier(swapChainSurface,
                       uint32v2(0,1),    // First mipmap
                       uint32v2(0,1),    // First layer
                       TextureAccess::RenderTargetWrite,
                       TextureAccess::Present);
      
      command->commit(waitForGPU);
      window->present(waitForGPU);

      // CommandBuffer is not released here, as it may still be processed by the GPU.
      // GpuDevice keeps it in it's Garbage Collector until completion Fence is signaled,
      // then it is safely and in automatic way destroyed.
      command          = nullptr;
      framebuffer      = nullptr;
      swapChainView    = nullptr;
      swapChainSurface = nullptr;
      }
 
   renderPass = nullptr;
   window = nullptr;
   gpu = nullptr;

   Log << "Sample closing." << endl;
}