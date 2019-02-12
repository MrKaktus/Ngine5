/*

 Ngine v5.0
 
 Module      : Frustum Wireframe
 Requirements: 
 Description : This class describes scene primitive
               used to display wireframe of given
               camera frustum.

*/

#include "resources/effect.h"
#include "scene/frustum.h"
#include "resources/context.h"

namespace en
{
   namespace scene
   {

   Frustum::Frustum(const FrustumSettings settings) :
      buffer(settings.wireframe(*en::ResourcesContext.defaults.enHeapBuffers)),
      //program(nullptr),
      //enModelMatrix(nullptr),
      Drawable()
   {
   //Effect effect(eGLSL_1_10, "red");
   //program = effect.program();
   //enModelMatrix = program.parameter("enModelMatrix");
   //enScene = program.block("enScene");
   }
   
   void Frustum::draw(const std::shared_ptr<gpu::Buffer> sceneParameters)
   {
   //enModelMatrix.set(*pWorldMatrix);
   //enScene.set(sceneParameters);
   //program.draw(buffer, LineStripes);
   }

   }
}

