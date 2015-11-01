/*

 Ngine v5.0
 
 Module      : Input Assembler.
 Requirements: none
 Description : Rendering context supports window
               creation and management of graphics
               resources. It allows programmer to
               use easy abstraction layer that 
               removes from him platform dependent
               implementation of graphic routines.

*/

#ifndef ENG_CORE_RENDERING_INPUT_ASSEMBLER
#define ENG_CORE_RENDERING_INPUT_ASSEMBLER

#include "core/defines.h"
#include "core/types.h"
#include "core/utilities/TintrusivePointer.h"
//#include "core/rendering/buffer.h"

namespace en
{
   namespace gpu
   {
   #define MaxInputAssemblerAttributesCount 16

   // Attributes representing data in fixed point shemes like 16.16, 8.0, 16.0 
   // were introduced in OpenGL ES for low end devices without HW acceleration. 
   // Currently all mobile devices has HW acceleration for OpenGL ES which means 
   // that floating point values will be better choose in almost all cases. 
   // Therefore fixed attribute formats are not supported by engine.

   // Format of attribute data
   enum AttributeFormat
        {
        None                      = 0,   
        Half                         ,
        Half2                        ,
        Half3                        ,
        Half4                        ,
        Float                        ,
        Float2                       ,
        Float3                       ,
        Float4                       ,
        Double                       ,
        Double2                      ,
        Double3                      ,
        Double4                      ,
        Int8                         ,
        Int8v2                       ,
        Int8v3                       ,
        Int8v4                       ,
        Int16                        ,
        Int16v2                      ,
        Int16v3                      ,
        Int16v4                      ,
        Int32                        ,
        Int32v2                      ,
        Int32v3                      ,
        Int32v4                      ,
        Int64                        ,
        Int64v2                      ,
        Int64v3                      ,
        Int64v4                      ,
        UInt8                        ,
        UInt8v2                      ,
        UInt8v3                      ,
        UInt8v4                      ,
        UInt16                       ,
        UInt16v2                     ,
        UInt16v3                     ,
        UInt16v4                     ,
        UInt32                       ,
        UInt32v2                     ,
        UInt32v3                     ,
        UInt32v4                     ,
        UInt64                       ,
        UInt64v2                     ,
        UInt64v3                     ,
        UInt64v4                     ,
        Float8_SNorm                 ,
        Float8v2_SNorm               ,
        Float8v3_SNorm               ,
        Float8v4_SNorm               ,
        Float16_SNorm                ,
        Float16v2_SNorm              ,
        Float16v3_SNorm              ,
        Float16v4_SNorm              ,
        Float8_Norm                  ,
        Float8v2_Norm                ,
        Float8v3_Norm                ,
        Float8v4_Norm                ,
        Float16_Norm                 ,
        Float16v2_Norm               ,
        Float16v3_Norm               ,
        Float16v4_Norm               ,
        Float4_10_10_10_2_SNorm      ,
        Float4_10_10_10_2_Norm       ,
        AttributeFormatsCount
        };

   // TEMP:
   class BufferView;

   struct InputAssemblerSettings
      {
      AttributeFormat format[MaxInputAssemblerAttributesCount]; // Format of each Vertex Attribute
      Ptr<BufferView> buffer[MaxInputAssemblerAttributesCount]; // Source buffer of each Vertex Attribute 
                                                                // (if several attributes share the same buffer, 
                                                                //  their order in buffer needs to match order in this array)
      InputAssemblerSettings();
      };

   //struct BufferSettings
   //   {
   //   // General buffer settings
   //   BufferType type;        // Buffer type
   //   union {                 // Vertex buffer can store geometry vertices
   //         uint32 vertices;  // or like Index buffer "elements" of some 
   //         uint32 elements;  // other type, while other buffers just need
   //         uint32 size;      // their size specified.
   //         };

   //   // Input Assembler compatible buffer settings
   //   AttributeFormat attribute[MaxInputAssemblerAttributesCount]; // Describes each element in the buffer
   //   uint32 step;            // Update rate, by default set to 0 - per vertex update, 
   //                           // otherwise describes after how many instances it should 
   //                           // proceed to next data
   //   };


   // Handle for Input assembler binding specification
   class InputAssembler : public SafeObject
      {
      public:
      virtual ~InputAssembler();                 // Polymorphic deletes require a virtual base destructor
      };

   // Creates InputAssembler description based on single buffer attributes
   Ptr<InputAssembler> Create(Ptr<BufferView> buffer); 

   // Creates InputAssembler description combining attributes from several buffers
   Ptr<InputAssembler> Create(InputAssemblerSettings& attributes); 


   //class InputAssemblerMTL : public InputAssembler
   //   {
   //   private:
   //   MTLVertexDescriptor* desc;
   //
   //   public:
   //   InputAssemblerMTL();
   //  ~InputAssemblerMTL();
   //   };

   }
}

#endif