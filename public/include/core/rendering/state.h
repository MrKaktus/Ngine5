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
   enum DrawableType
        {
        Points                      = 0,
        Lines                          ,
        LineLoops                      ,
        LineStripes                    ,
        Triangles                      ,
        TriangleFans                   ,
        TriangleStripes                ,
        Patches                        ,
        DrawableTypesCount          
        };                          
                                    
   // Comparison method             
   enum CompareMethod               
        {                           
        Never                       = 0,    
        Always                         ,
        Less                           ,
        LessOrEqual                    ,
        Equal                          ,
        Greater                        ,
        GreaterOrEqual                 ,
        NotEqual                       ,
        CompareMethodsCount
        };

   // Stencil Buffer Modification Methods
   enum StencilModification
        {
        Keep                        = 0,
        Clear                          ,
        Reference                      ,
        Increase                       ,
        IncreaseWrap                   ,
        Decrease                       ,
        DecreaseWrap                   ,
        InvertBits                     ,
        StencilModificationsCount
        };

   // Color Buffer blend functions
   enum BlendFunction
        {
        Zero                        = 0,
        One                            ,
        Source                         ,
        OneMinusSource                 ,
        Destination                    ,
        OneMinusDestination            ,
        SourceAlpha                    ,
        OneMinusSourceAlpha            ,
        DestinationAlpha               ,
        OneMinusDestinationAlpha       ,
        ConstantColor                  ,
        OneMinusConst                  ,
        ConstantAlpha                  ,
        OneMinusConstantAlpha          ,
        SourceAlphaSaturate            ,
        SecondSource                   ,
        OneMinusSecondSource           ,
        SecondSourceAlpha              ,
        OneMinusSecondSourceAlpha      ,
        BlendFunctionsCount
        };

   // Color Buffer blend equations
   enum BlendEquation
        {
        Add                         = 0,
        Subtract                       ,
        DestinationMinusSource         ,
        Min                            ,
        Max                            ,
        BlendEquationsCount
        };

   enum LogicOperation
        {
        ClearDestination            = 0,
        Set                            ,
        Copy                           ,
        CopyInverted                   ,
        NoOperation                    ,
        Invert                         ,
        And                            ,
        NAnd                           ,
        Or                             ,
        Nor                            ,
        Xor                            ,
        Equiv                          ,
        AndReverse                     ,
        AndInverted                    ,
        OrReverse                      ,
        OrInverted                     ,
        LogicOperationsCount
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
        ColorSpaceLinear        = 0,
        ColorSpaceSRGB        
        };
   }
}

#endif