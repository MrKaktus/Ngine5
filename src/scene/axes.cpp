/*

 Ngine v5.0
 
 Module      : Coordinate System Axes
 Requirements: none
 Description : This class describes scene primitive
               used to display axes of given coordinate
               system.

*/

#include "scene/axes.h"
#include "resources/context.h"
#include "resources/effect.h"

using namespace en::resource;

namespace en
{
   namespace scene
   {

   Axes::Axes() :
      buffer(en::ResourcesContext.defaults.enAxes),
      program(nullptr),
      enModelMatrix(nullptr),
      Drawable()
   {
   Effect effect(eGLSL_1_40, "enAxes");
   program = effect.program();
   enModelMatrix = program.parameter("enModelMatrix");
   enScene       = program.block("enScene");
   }
   
   void Axes::draw(const Buffer& sceneParameters)
   {
   enModelMatrix.set(*pWorldMatrix);
   enScene.set(sceneParameters);
   program.draw(buffer, LineStripes);
   }

   }
}