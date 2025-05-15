/*

 Ngine v5.0
 
 Module      : MTL files support
 Requirements: none
 Description : Supports reading materials from *.mtl files.

*/

#ifndef ENG_RESOURCES_MTL
#define ENG_RESOURCES_MTL

#include "core/defines.h"
#include "core/types.h"
#include "core/rendering/state.h"
#include "resources/resources.h"

namespace en
{
namespace mtl
{

enum class ProjectionType : uint8
{
    Unknown         = 0,
    Cube               , // Cube map using 6 separate textures
    Equirectangular    , // See: https://en.wikipedia.org/wiki/Equirectangular_projection
    Count              ,
};

// Reflection map faces 
// (coordinate system independent)
enum class CubeFace : uint8
{
    Top    = 0,
    Bottom    ,
    Front     ,
    Back      ,
    Left      ,
    Right     ,
    Count     ,
    Spherical = Top,
};


// "illum" illumination modes:

// 0 - Color on and Ambient off
// 1 - Color on and Ambient on
// 2 - Highlight on
// 3 - Reflection on and Ray trace on
// 4 - Transparency: Glass on
//     Reflection : Ray trace on
// 5 - Reflection : Fresnel on and Ray trace on
// 6 - Transparency : Refraction on
//     Reflection : Fresnel off and Ray trace on
// 7 - Transparency : Refraction on
//     Reflection : Fresnel on and Ray trace on
// 8 - Reflection on and Ray trace off
// 9 - Transparency : Glass on
//     Reflection : Ray trace off
// 10 - Casts shadows onto invisible surfaces

enum class IlluminationModel : uint8
{
};

struct Map
{
    std::string filename; // File name with optional relative path from model location
    bool blendU;
    bool blendV;
    bool clampToEdge;

    gpu::ChannelMask channels; // Source channels

    bool colorCorrection;

    float bumpMultiplier; // Adviced to stay in [0 .. 1] range, but can be negative.
    float boost;          // 0.0+

    Map();
};

// Need good name conveying message that its result of file parsing, but its not optimized for renderer nor engine.
// 
// UnoptimizedMaterial
// IntermediateMaterial
// ParsedMaterial         <-
// RawMaterial
// MaterialDescription    <-
//
struct Material
{
    std::string name;
    resources::ShadingModel shadingModel;
    IlluminationModel model;
    bool antiAliasing; // false by default.

    struct Ambient
    {
        Map    map;
        float3 color;  // Ambient light reflectivity.
    } ambient;

    struct Diffuse
    {
        Map    map;   // Diffuse map.
                      // MTL "map_Kd" (to be multiplied by Kd).
        float3 color; // Diffuse reflectivity color.
    } diffuse;

    struct Specular
    {
        Map    map;          //  Specular map.
                             // MTL "map_Ks" (to be multiplied by Ks).
        Map    exponentMap;  // Specular exponent map.
                             // MTL " map_Ns"
        float3 color;        // Specular reflectivity color.
        float  exponent;     // Focus of specular highlights (exponent). [0..1000]. High value resulting in a tight, concentrated highlight.
                             // Phong "shininess".
    } specular;

    struct Opacity
    {
        Map    map;; // MTL "map_d"
        float  factor; // [0.0 .. 1.0] 0.0 - transparent 1.0 - opaque
        // MTL: Specified by "d" - dissolve property
        //      "-halo" modifier (indicates that transparency is dependent on the viewer direction, for e.g. glass ball).
        //      dissolve = 1.0 - (N*v)(1.0-factor)
    } opacity;

    struct Transmission
    {
        float3 color; // Transmission filter color.
        float indexOfRefraction; // [0.001 to 10]. A value of 1.0 means that light does not bend as it passes through an object. Glass has IOR = 1.5.
    } transmissionFilter;

    // Single channel greyscale values:
    // black - indented
    // grey  - surface level
    // white - extruded
    struct Bump
    {
        Map map;  // MTL "bump"
    } bump;

    // Vectors specifying direction surface is facing (but not height) at given sample point
    struct Normal
    {
        Map map;
    } normal;

    // Scalar specifying surface height (but not direction it is facing) at given sample point
    // Single channel greyscale values.
    struct Height
    {
        Map map; // MTL: "disp"
    } height;

    // Vectors specifying direction surface is facing, and its height 
    // (through vector length), at given sample point
    struct Displacement
    {
        Map map;
    } displacement;

    struct Decal
    {
        Map map; // Scalar map [0.0 .. 1.0] specifies blending factor between "texture" and "material" colors ("texture" color contribution): 
                 // MTL "decal" - result_color = tex_color(tv) * decal(tv) + mtl_color * (1.0-decal(tv))
                 // TODO: This should be Alpha channel of decal Albedo/material blended on top?
    } decal;

    struct Reflection
    {
        Map map[underlyingType(CubeFace::Count)];   // MTL "refl"
        uint16 sharpness;  // [0..1000]. 60 is default value.
        ProjectionType projection;
    } reflection;
};

std::vector<mtl::Material*>* parseMTL(const uint8* buffer, const uint64 size);

std::vector<mtl::Material*>* load(const std::string& filename);

// TODO: Deprecate
bool load(const std::string& filename, 
          const std::string& name, 
          resources::Material& material);

} // en::mtl
} // en

#endif