/*

 Ngine v5.0
 
 Module      : Engine model files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading from and writing to
               engine model file format.

*/

#include "core/rendering/buffer.h"
   

namespace en
{
   namespace model
   {
   using namespace gpu;

   // Engine model file format description
   aligned(1)
   struct Header
          {
          uint32 version;         // Model file format version
          struct Buffers
                 {
                 uint16 geometry; // Geometry buffers count
                 uint16 elements; // Index buffers count
                 } buffers; 
          uint16 meshes;          // Meshes count
          };

   struct BufferDescription
          {
          Formatting formatting;  // Buffer size will be calculated
          union {                 // Buffer can store geometry vertices or 
                uint32 vertices;  // "elements" of some other type. This is
                uint32 elements;  // why two naming conventions are allowed.
                };
          uint64 data;            // Offset to buffer data in memory
          };

   struct MeshDescription
          {
          float4x4 matrix; // Local transformation matrix
          uint16   childs; // Number of child meshes
          uint16   index;  // Index of first child in this array
          };

   // Header  
   // BufferDescription geometry[buffers.geometry]
   // BufferDescription elements[buffers.elements] 
   // MeshDescriptor    tree[header.meshes]  
   // RAW
   // RAW
   aligndefault


   }
}
