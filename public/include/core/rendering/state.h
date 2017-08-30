/*

 Ngine v5.0
 
 Module      : State.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_STATE
#define ENG_CORE_RENDERING_STATE

namespace en
{
   namespace gpu
   {
   // Data access type
   enum DataAccess
      {
      Read                      = 0,  // Read data from GPU
      Write                        ,  // Write data to GPU
      ReadWrite                    ,  // Transfer data between CPU and GPU
      DataAccessTypes
      };

   // Types of primitives to draw
   //
   // LineLoops    - are unsupported by all modern API's (D3D12, Vulkan, Metal)
   // TriangleFans - are still supported only by Vulkan
   enum DrawableType
      {
      Points                      = 0,
      Lines                          ,
      LineStripes                    ,
      Triangles                      ,
      TriangleStripes                ,
      Patches                        ,
      DrawableTypesCount          
      };


                                    
   // Comparison method             
   enum CompareMethod               
      {                           
      Never                       = 0,
      Less                           ,
      Equal                          ,
      LessOrEqual                    ,
      Greater                        ,
      NotEqual                       ,
      GreaterOrEqual                 ,
      Always                         ,
      CompareMethodsCount
      };

   // Stencil Buffer Modification Methods
   enum StencilModification
      {
      Keep                        = 0,
      Clear                          ,
      Reference                      ,
      Increase                       ,
      Decrease                       ,
      InvertBits                     ,
      IncreaseWrap                   ,
      DecreaseWrap                   ,
      StencilModificationsCount
      };


   enum FillMode
      {
      Vertices                    = 0,
      Wireframe                      , 
      Solid                          ,
      FillModesCount
      };

   // Surface face
   enum Face
      {
      FrontFace                   = 0,
      BackFace                       ,
      BothFaces                      ,
      FaceChoosesCount
      };

   // Method used to calculate normal vector
   enum NormalCalculationMethod
      {
      ClockWise                   = 0,
      CounterClockWise               ,
      NormalCalculationMethodsCount
      };

   // Color space used
   enum ColorSpace
      {
      ColorSpaceLinear            = 0,
      ColorSpaceSRGB        
      };
   }
}

#endif
