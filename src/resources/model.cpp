/*

 Ngine v5.0
 
 Module      : Engine model files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading from and writing to
               engine model file format.

*/

#include "core/memory/alignment.h"
#include "core/rendering/buffer.h"
   

namespace en
{
namespace model
{

using namespace gpu;

// Engine model file format description
alignTo(1)
struct Header
{
    uint32 version;      // Model file format version
    struct Buffers
    {
        uint16 geometry; // Geometry buffers count
        uint16 elements; // Index buffers count
    } buffers;
    uint16 meshes;       // Meshes count
};

struct BufferDescription
{
    Formatting formatting; // Buffer size will be calculated
    union 
    {                      // Buffer can store geometry vertices or 
        uint32 vertices;   // "elements" of some other type. This is
        uint32 elements;   // why two naming conventions are allowed.
    };
    uint64 data;           // Offset to buffer data in memory
};

struct MeshDescription
{
    float4x4 matrix; // Local transformation matrix
    uint16   childs; // Number of child meshes
    uint16   index;  // Index of first child in this array

    // TODO: Which vertex/index buffer correspond to which mesh? (Meshes count may differ from buffers count)
    // TODO: Buffers are per UVmapping/materia? etc.
};

// File structure:
// Header  
// BufferDescription geometry[buffers.geometry]
// BufferDescription elements[buffers.elements] 
// MeshDescriptor    tree[header.meshes]  
// RAW vertices buffers
// RAW indices buffers
alignToDefault

// TODO: Missing actual implementation loading and storing data in such file format.
// TODO: Should take into notice, legacy model file formats used by Engine v2.0

} // en::model
} // en
