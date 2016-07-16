/*

 Ngine v5.0
 
 Module      : D3D12 Pipeline.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#include "core/defines.h"

#if defined(EN_PLATFORM_WINDOWS)

#include "core/log/log.h"

#include "core/rendering/d3d12/dx12Pipeline.h"
#include "core/rendering/d3d12/dx12InputAssembler.h"
#include "core/rendering/d3d12/dx12RenderPass.h"
#include "core/rendering/d3d12/dx12Blend.h"
#include "core/rendering/d3d12/dx12Multisampling.h"
#include "core/rendering/d3d12/dx12Shader.h"

#include "core/rendering/d3d12/dx12Device.h"

namespace en
{
   namespace gpu
   {
   // Topology of primitives to draw
   const D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslateDrawableTopology[DrawableTypesCount]
      {
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT    , // Points
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // Lines
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // LineLoops
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE     , // LineStripes
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // Triangles
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // TriangleFans
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE , // TriangleStripes
      D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH    , // Patches
      };
      
   
   // TODO: Finish all the stuff
   }
}
#endif
