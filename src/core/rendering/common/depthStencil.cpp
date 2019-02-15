/*

 Ngine v5.0
 
 Module      : Depth Stencil.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/rendering/depthStencil.h"

namespace en
{
   namespace gpu
   {
   // These are per face:
   //
   //   D    S      D3D12                   Metal                   Vulkan          OpenGL
   //        -   StencilFailOp       stencilFailureOperation    stencilFailOp       N/A
   //   -    +   StencilDepthFailOp  depthFailureOperation      stencilDepthFailOp  N/A
   //   +    +   StencilPassOp       depthStencilPassOperation  stencilPassOp       N/A

   // S RM F     static                  static ?                  dynamic        dynamic
   // S RM B       --                    static ?                    --           dynamic <- makes sense, we can divide stencil buffer bits per facing
   // S WM F     static                  static ?                  dynamic        dynamic
   // S WM B       --                    static ?                    --           dynamic <- https://www.opengl.org/registry/specs/EXT/stencil_two_side.txt
   // S Ref F    dynamic                 dynamic ?                 dynamic        dynamic    (there is possibility of use case for two sided R/W bitmasks)
   // S Ref B      --                       --                     dynamic        dynamic <- Two sided reference value needs two slots from state, one slot if returned by FS
   // D boun min                                                   dynamic 
   // D boun max                                                   dynamic
   // D Wr  En   static                  static ?                   static        dynamic

   // Defaults:
   //
   // stencilTest - false
   //
   // S Ref F     - 0
   // S Ref B     - 0
   // S Comp F    - Always
   // S Comp B    - Always
   // S RM xxx    - 0xFFFFFFFF
   // S Op xxx    - Keep
   //
   // depthTest   - false
   // D Func      - Less
   //
   // depthWrite  - true
   //
   // depthClamp  - false
   //
   // depthBounds - false



   // Depth-Stencil State:
   //
   // D3D12  - Static, part of Pipeline.
   //          Reference - shared, dynamic -  ID3D12GraphicsCommandList::OMSetStencilRef()
   //          ReadMask  - shared, static
   //          WriteMask - shared, static
   //          Don't support depth bounds!
   //
   // Metal  - Dynamic (bind to encoder) 
   //          Reference value is dynamic (shared only in IOS 8.0+)
   //          Stencil Test is turned on separately for each face!
   //          Don't support depth bounds!
   //
   // Vulkan - Static, part of Pipeline
   //
   // bool supportDepthRange = false for D3D12 and Metal
   // bool supportSeparateStencilReferenceValue = false for D3D12, false for IOS 8.0, true for IOS 9.0, OSX, 
   // bool supportSeparateStencilReadWriteMask = false for D3D12, true for Metal, 


   // Vulkan removed dynamic states because it can patch them easily using Pipeline Object Cache ?



   // Default Values: D3D12                                       Metal                Vulkan                OpenGL
   // 
   // depthTest       T                                         | F (func: Always)  |  ?                  | F
   // depthWrite      T (write mask)                            | F                 |  ?                  |
   // depthBounds     ----                                      | ----              |  ? (Static/Dynamic) |
   // stencilTest     F                                         | F       F         |  ?                  |
   // depthFunc       Less                                      | Always            |  ?                  | Less
   // depthRange      ----                                      | ----              |  ?                  |
   //                                                                                          
   //                 FRONT:  BACK:                               FRONT:  BACK:                
   //                                                                                          
   // function        Always  Always                            | Always  Always    |  ?    ?  |
   // stencilFails    Keep    Keep                              | Keep    Keep      |  ?    ?  |
   // depthFails      Keep    Keep                              | Keep    Keep      |  ?    ?  |
   // bothPass        Keep    Keep                              | Keep    Keep      |  ?    ?  |
   // reference       Shared - Dynamic                          | Dynamic Dynamic   |  ?    ?  |
   // readMask        Shared - D3D12_DEFAULT_STENCIL_READ_MASK  | 0xFFFF  0xFFFF    |  ?    ?  |
   // writeMask       Shared - D3D12_DEFAULT_STENCIL_WRITE_MASK | 0xFFFF  0xFFFF    |  ?    ?  |
   //
   DepthStencilStateInfo::DepthStencilStateInfo() :
      enableDepthTest(false),
      enableDepthWrite(false),
      enableDepthBounds(false), 
      enableStencilTest(false),  
      depthFunction(Always),
      depthRange(0.0f, 1.0f)
   {
   for(uint8 i=0; i<2; ++i)
      {
      stencil[i].function         = Always;
      stencil[i].whenStencilFails = StencilOperation::Keep;
      stencil[i].whenDepthFails   = StencilOperation::Keep;
      stencil[i].whenBothPass     = StencilOperation::Keep;
      stencil[i].reference        = 0;
      stencil[i].readMask         = 0XFFFFFFFF;
      stencil[i].writeMask        = 0xFFFFFFFF;
      }
   }

   }
}
