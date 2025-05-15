/*

 Ngine v5.0
 
 Module      : MTL files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading materials from *.mtl files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/utilities/parser.h"
#include "utilities/strings.h"
#include "resources/interface.h" 
#include "resources/mtl.h"    

namespace en
{
namespace mtl
{

Map::Map() :
    blendU(false),
    blendV(false),
    clampToEdge(false),
    colorCorrection(false),
    bumpMultiplier(1.0f),
    boost(0.0f)
{
}

// Wavefront Material Template Library (MTL) File Format
// https://paulbourke.net/dataformats/mtl/
//
class ParserMTL : public Parser
{

public:
    // Passes ownership of buffer to parser
    ParserMTL(const uint8* buffer, const uint64 size);

    bool readVector3f(float(&vector)[3]);
    bool readVectorProperty(float(&vector)[3]);

    bool parseOpacityFactor(mtl::Material::Opacity& opacity);
    bool parseMap(const resources::MaterialComponent component, mtl::Map& map);
    bool parseReflectionMap(mtl::Material& material);
};

ParserMTL::ParserMTL(const uint8* buffer, const uint64 size) :
    Parser(buffer, size)
{
}

bool ParserMTL::readVector3f(float(&vector)[3])
{
    // 1st component (mandatory):

    findNextElement();
    if (type == ParserType::Float)
    {
        if (!readF32(vector[0]))
        {
            logWarning("Vector parsing failed. Failed to read float value for 1st component.");
            return false;
        }
    }
    else // Its possible that float value is represented with integer (for e.g. simply "0").
    if (type == ParserType::Integer)
    {
        sint64 temp = 0;
        if (!readS64(temp))
        {
            logWarning("Vector parsing failed. Failed to read integer value for 1st component.");
            return false;
        }

        vector[0] = (float)temp;
    }
    else
    {
        logWarning("Vector parsing failed. Expected float value for 1st component.");
        return false;
    }

    // 2nd component (optional):

    vector[1] = NAN;
    vector[2] = NAN;
    type = findNextElement();
    if (type == ParserType::Float)
    {
        if (!readF32(vector[1]))
        {
            logWarning("Vector parsing failed. Failed to read float value for 2nd component.");
            return false;
        }
    }
    else // Its possible that float value is represented with integer (for e.g. simply "0").
    if (type == ParserType::Integer)
    {
        sint64 temp = 0;
        if (!readS64(temp))
        {
            logWarning("Vector parsing failed. Failed to read integer value for 2nd component.");
            return false;
        }

        vector[1] = (float)temp;
    }

    if (!isnan(vector[1]))
    {
        // 3rd component (optional):

        type = findNextElement();
        if (type == ParserType::Float)
        {
            if (!readF32(vector[2]))
            {
                logWarning("Vector parsing failed. Failed to read float value for 3rd component.");
                return false;
            }
        }
        else // Its possible that float value is represented with integer (for e.g. simply "0").
        if (type == ParserType::Integer)
        {
            sint64 temp = 0;
            if (!readS64(temp))
            {
                logWarning("Vector parsing failed. Failed to read integer value for 3rd component.");
                return false;
            }

            vector[2] = (float)temp;
        }
        else
        {
            vector[2] = vector[0];
        }
    }
    else
    {
        vector[1] = vector[0];
        vector[2] = vector[0];
    }

    return true;
}

bool ParserMTL::readVectorProperty(float(&vector)[3])
{
    if (!readVector3f(vector))
    {
        if (type == ParserType::String)
        {
            if (isStringMatching("spectral"))
            {
                logWarning("Unsupported MTL feature. Ka spectral modifier.");
                return false;
            }
            else
            if (isStringMatching("xyz"))
            {
                float xyz[3] = {};
                if (readVector3f(xyz))
                {
                    // TODO: Convert CIEXYZ to RGB (when knowing which color space is used and its primaries).
                    // See: http://www.brucelindbloom.com/

                    logWarning("Unsupported MTL feature. Ka xyz modifier.");
                    return false;
                }
                else
                {
                    logWarning("MTL file corrupted. Failed to parse Ka xyz values.");
                    return false;
                }
            }
            else
            {
                logWarning("MTL file corrupted. Unrecognized Ka property modifier: %s.", string());
                return false;
            }
        }
        else
        {
            logWarning("MTL file corrupted. Couldn't parse Ka property.");
            return false;
        }
    }

    return true;
}

bool ParserMTL::parseOpacityFactor(mtl::Material::Opacity& opacity)
{
    findNextElement();
    if (type == ParserType::String)
    {
        if (isStringMatching("-halo"))
        {
            findNextElement();
            if (type == ParserType::Float)
            {
                if (!readF32(opacity.factor))
                {
                    logWarning("MTL file corrupted. Failed to read float value of opacity (d -halo).");
                    return false;
                }

                // TODO: Mark in material that its "-halo" factor!
            }
            else
            {
                logWarning("MTL file corrupted. Expected float value for opacity (d -halo).");
                return false;
            }
        }
        else
        {
            logWarning("MTL file corrupted. Expected \"-halo\" modifier for opacity (d).");
            return false;
        }
    }
    else
    if (type == ParserType::Float)
    {
        if (!readF32(opacity.factor))
        {
            logWarning("MTL file corrupted. Failed to read float value of opacity (d).");
            return false;
        }
    }
    else
    {
        logWarning("MTL file corrupted. Expected float value or \"-halo\" modifier for opacity (d).");
        return false;
    }

    return true;
}

bool ParserMTL::parseMap(const resources::MaterialComponent component, mtl::Map& map)
{
    // Parses map options one at a time
    do
    {
        findNextElement();
        if (type != ParserType::String)
        {
            logWarning("MTL file corrupted. Expected string representing option or filename for parsed map.");
            return false;
        }

        // Texture blending in the horizontal direction
        if (isStringMatching("-blendu"))
        {
            findNextElement();
            if (type == ParserType::String)
            {
                if (isStringMatching("on"))
                {
                    map.blendU = true;
                }
                else
                if (isStringMatching("off"))
                {
                    map.blendU = false;
                }
                else
                {
                    logWarning("MTL file corrupted. Expected \"on\"/\"off\" property of currently parsed option (-blendu).");
                    return false;
                }
            }
            else
            {
                logWarning("MTL file corrupted. Expected string property of currently parsed option (-blendu).");
                return false;
            }
        }
        else // Texture blending in the vertical direction
        if (isStringMatching("-blendv"))
        {
            findNextElement();
            if (type == ParserType::String)
            {
                if (isStringMatching("on"))
                {
                    map.blendV = true;
                }
                else
                if (isStringMatching("off"))
                {
                    map.blendV = false;
                }
                else
                {
                    logWarning("MTL file corrupted. Expected \"on\"/\"off\" property of currently parsed option (-blendv).");
                    return false;
                }
            }
            else
            {
                logWarning("MTL file corrupted. Expected string property of currently parsed option (-blendv).");
                return false;
            }
        }
        else // Clamps texture mapping to [0.1] range (clamp to edge)
        if (isStringMatching("-clamp"))
        {
            findNextElement();
            if (type == ParserType::String)
            {
                if (isStringMatching("on"))
                {
                    map.clampToEdge = true;
                }
                else
                if (isStringMatching("off"))
                {
                    map.clampToEdge = false;
                }
                else
                {
                    logWarning("MTL file corrupted. Expected \"on\"/\"off\" property of currently parsed option (-clamp).");
                    return false;
                }
            }
            else
            {
                logWarning("MTL file corrupted. Expected string property of currently parsed option (-clamp).");
                return false;
            }
        }
        else
        if (isStringMatching("-mm"))
        {
            // From the spec:
            //
            // -mm base gain
            // 
            //  The -mm option modifies the range over which scalar or color texture
            // values may vary.  This has an effect only during rendering and does not
            // change the file.
            // 
            //  "base" adds a base value to the texture values.  A positive value makes
            // everything brighter; a negative value makes everything dimmer.  The
            // default is 0; the range is unlimited.
            // 
            //  "gain" expands the range of the texture values.  Increasing the number
            // increases the contrast.  The default is 1; the range is unlimited.

            // TODO: Consider silently skipping this property
            logWarning("Unsupported MTL feature. Engine does not support specifying material option (-mm base gain).");
            return false;
        }
        else
        if (isStringMatching("-o"))
        {
            // From the spec:
            //
            // The -o option offsets the position of the texture map on the surface by
            // shifting the position of the map origin.  The default is 0, 0, 0.
            // 
            //  "u" is the value for the horizontal direction of the texture
            // 
            //  "v" is an optional argument.
            //  "v" is the value for the vertical direction of the texture.
            // 
            //  "w" is an optional argument.
            //  "w" is the value used for the depth of a 3D texture.
            //
            float3 uvwOffset;
            if (!readVector3f(uvwOffset.value))
            {
                logWarning("MTL file corrupted. Failed to parse -o option values.");
                return false;
            }

            if (uvwOffset.u != 0.0f ||
                uvwOffset.v != 0.0f ||
                uvwOffset.w != 0.0f)
            {
                logWarning("Unsupported MTL feature. Engine does not support material UVW offsetting.");
                return false;
            }

            // UVW offset is default: 0, 0, 0 and can be safely ignored.
        }
        else
        if (isStringMatching("-s"))
        {
            // From the spec:
            //
            // The -s option scales the size of the texture pattern on the textured
            // surface by expanding or shrinking the pattern.  The default is 1, 1, 1.
            //
            // "u" is the value for the horizontal direction of the texture
            //
            // "v" is an optional argument.
            // "v" is the value for the vertical direction of the texture.
            //
            // "w" is an optional argument.
            // "w" is a value used for the depth of a 3D texture.
            // "w" is a value used for the amount of tessellation of the displacement map.
            //
            float3 uvwScale;
            if (!readVector3f(uvwScale.value))
            {
                logWarning("MTL file corrupted. Failed to parse -s option values.");
                return false;
            }

            if (uvwScale.u != 1.0f ||
                uvwScale.v != 1.0f ||
                uvwScale.w != 1.0f)
            {
                logWarning("Unsupported MTL feature. Engine does not support material UVW scaling.");
                return false;
            }

            // UVW scale is default: 1, 1, 1 and can be safely ignored.
        }
        else
        if (isStringMatching("-t"))
        {
            // From the spec:
            //
            // The -t option turns on turbulence for textures.  Adding turbulence to a
            // texture along a specified direction adds variance to the original image
            // and allows a simple image to be repeated over a larger area without
            // noticeable tiling effects.
            // 
            //  turbulence also lets you use a 2D image as if it were a solid texture,
            // similar to 3D procedural textures like marble and granite.
            // 
            //  "u" is the value for the horizontal direction of the texture
            // turbulence.
            // 
            //  "v" is an optional argument.
            //  "v" is the value for the vertical direction of the texture turbulence.
            // 
            //  "w" is an optional argument.
            //  "w" is a value used for the depth of the texture turbulence.
            // 
            //  By default, the turbulence for every texture map used in a material is
            // uvw = (0,0,0).  This means that no turbulence will be applied and the 2D
            // texture will behave normally.
            // 
            //  Only when you raise the turbulence values above zero will you see the
            // effects of turbulence.
            //
            float3 uvwTurbulence;
            if (!readVector3f(uvwTurbulence.value))
            {
                logWarning("MTL file corrupted. Failed to parse -t option values.");
                return false;
            }

            if (uvwTurbulence.u != 0.0f ||
                uvwTurbulence.v != 0.0f ||
                uvwTurbulence.w != 0.0f)
            {
                logWarning("Unsupported MTL feature. Engine does not support material UVW turbulence.");
                return false;
            }

            // UVW turbulence is default: 0, 0, 0 and can be safely ignored.
        }
        else
        if (isStringMatching("-texres"))
        {
            // From the spec:
            //
            // The -texres option specifies the resolution of texture created when an
            // image is used.  The default texture size is the largest power of two
            // that does not exceed the original image size.
            // 
            //  If the source image is an exact power of 2, the texture cannot be built
            // any larger.  If the source image size is not an exact power of 2, you
            // can specify that the texture be built at the next power of 2 greater
            // than the source image size.
            // 
            //  The original image should be square, otherwise, it will be scaled to
            // fit the closest square size that is not larger than the original.
            // Scaling reduces sharpness.
            //
            float resolution;
            findNextElement();
            if (type == ParserType::Float)
            {
                if (!readF32(resolution))
                {
                    logWarning("MTL file corrupted. Failed to read float value for currently parsed option (-texres).");
                    return false;
                }
            }
            else // Its possible that float value is represented with integer (for e.g. simply "0").
            if (type == ParserType::Integer)
            {
                sint64 temp = 0;
                if (!readS64(temp))
                {
                    logWarning("MTL file corrupted. Failed to read integer value for currently parsed option (-texres).");
                    return false;
                }

                resolution = (float)temp;
            }
            else
            {
                logWarning("MTL file corrupted. Expected float or integer value for currently parsed option (-texres).");
                return false;
            }

            // TODO: Consider silently skipping this property
            logWarning("Unsupported MTL feature. Engine does not support specifying material components resolution.");
            return false;
        }
        else
        if (isStringMatching("-cc"))
        {
            if (component != resources::MaterialComponent::Ambient &&
                component != resources::MaterialComponent::Diffuse &&
                component != resources::MaterialComponent::Specular)
            {
                logWarning("MTL file corrupted. Currently parsed option (-cc) is not allowed for specified material component.");
                return false;
            }

            findNextElement();
            if (type == ParserType::String)
            {
                if (isStringMatching("on"))
                {
                    map.colorCorrection = true;
                }
                else
                if (isStringMatching("off"))
                {
                    map.colorCorrection = false;
                }
                else
                {
                    logWarning("MTL file corrupted. Expected \"on\"/\"off\" property of currently parsed option (-cc).");
                    return false;
                }
            }
            else
            {
                logWarning("MTL file corrupted. Expected string property of currently parsed option (-cc).");
                return false;
            }
        }
        else
        if (isStringMatching("-imfchan"))
        {
            // From the spec:
            //
            // The - imfchan option specifies the channel used to create a scalar or
            // bump texture.Scalar textures are applied to :
            // 
            // transparency
            // specular exponent
            // decal
            // displacement
            // 
            // The channel choices are :
            // 
            // r specifies the red channel.
            // g specifies the green channel.
            // b specifies the blue channel.
            // m specifies the matte channel.
            // l specifies the luminance channel.
            // z specifies the z - depth channel.
            // 
            // The default for bump and scalar textures is "l" (luminance), unless you
            // are building a decal.In that case, the default is "m" (matte).
            //
            if (component != resources::MaterialComponent::Opacity && 
                component != resources::MaterialComponent::SpecularExponent &&
                component != resources::MaterialComponent::Decal &&
                component != resources::MaterialComponent::Height &&
                component != resources::MaterialComponent::Bump)
            {
                logWarning("MTL file corrupted. Currently parsed option (-imfchan) is not allowed for specified material component.");
                return false;
            }

            // Specifies source channel in texture map for given material component
            findNextElement();
            if (type == ParserType::String)
            {
                if (isStringMatching("r"))
                {
                    map.channels = gpu::ChannelMask::Red;
                }
                else
                if (isStringMatching("g"))
                {
                    map.channels = gpu::ChannelMask::Green;
                }
                else
                if (isStringMatching("b"))
                {
                    map.channels = gpu::ChannelMask::Blue;
                }
                else // Matte channel (aplha)
                if (isStringMatching("m"))
                {
                    map.channels = gpu::ChannelMask::Alpha;
                }
                else // Luminance
                if (isStringMatching("l"))
                {
                    logWarning("Unsupported MTL feature. Engine does not support material option (-imfchan) with source channel being Luminance (-l).");
                    return false;
                }
                else // Z-Depth
                if (isStringMatching("z"))
                {
                    logWarning("Unsupported MTL feature. Engine does not support material option (-imfchan) with source channel being Depth (-z).");
                    return false;
                }
                else
                {
                    logWarning("MTL file corrupted. Expected r/g/b/m/l/z property of currently parsed option (-imfchan).");
                    return false;
                }
            }
            else
            {
                logWarning("MTL file corrupted. Expected string property of currently parsed option (-imfchan).");
                return false;
            }
        }
        else // Bump Multiplier - applied to surface normal (height remains unchanged)
        if (isStringMatching("-bm"))
        {
            if (component != resources::MaterialComponent::Bump)
            {
                logWarning("MTL file corrupted. Currently parsed option (-bm) is not allowed for specified material component.");
                return false;
            }

            // TODO: Consider silently skipping this property
            logWarning("Unsupported MTL feature. Engine does not support specifying material option (-bm mult).");
            return false;
        }
        else
        if (isStringMatching("-boost"))
        {
            // TODO: Can this be set in material or specific material map? or is it global?
            // boost increases image clarity.
            // non-negative floating point value representing the degree of increased clarity

            // TODO: Consider silently skipping this property
            logWarning("Unsupported MTL feature. Engine does not support specifying material option (-boost value).");
            return false;
        }
        else
        {
            // Every other string is treated as filename 
            // (it should be last element before EOL)
            findNextElement();
            if (type != ParserType::EndOfLine)
            {
                logWarning("MTL file corrupted. Unrecognized option %s.", string());
                return false;
            }

            map.filename = std::string(string(), stringLength());
            return true;
        }
    }
    while(type != ParserType::EndOfLine);

    // Should never reach this place
    assert( 0 );
    return true;
}

bool ParserMTL::parseReflectionMap(mtl::Material& material)
{
    // TODO: Requires illumination model 3+ (check)

    // refl -type sphere -options -args filename
    //
    // OR
    //
    // refl -type cube_side -options -args filenames

    // Possible options:
    //
    // -blendu on | off
    // - blendv on | off
    // - cc on | off
    // - clamp on | off
    // - mm base gain
    // - o u v w
    // - s u v w
    // - t u v w
    // - texres value

    // TODO: Check if given -type is not redefined. 
    // TODO: Check if all cube faces are defined in case of cube.
    // TODO: Check that cube and spherical are not mixed.

    findNextElement();
    if (type != ParserType::String)
    {
        logWarning("MTL file corrupted. Expected string property of parsed reflection map (-type).");
        return false;
    }

    if (!isStringMatching("-type"))
    {
        logWarning("MTL file corrupted. Expected \"-type\" string for parsed reflection map.");
        return false;
    }

    findNextElement();
    if (type != ParserType::String)
    {
        logWarning("MTL file corrupted. Expected string property of parsed reflection map (-type specifier).");
        return false;
    }

    if (isStringMatching("sphere"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Spherical)]))
        {
            return false;
        }

        material.reflection.projection = ProjectionType::Equirectangular;
    }
    else
    if (isStringMatching("cube_top"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Top)]))
        {
            return false;
        }

        // TODO: unlikely
        if (material.reflection.projection != ProjectionType::Unknown &&
            material.reflection.projection != ProjectionType::Cube)
        {
            logWarning("MTL file corrupted. Reflection map has both (sphere) and (%s) types specified.", string());
            return false;
        }

        material.reflection.projection = ProjectionType::Cube;
    }
    else
    if (isStringMatching("cube_bottom"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Bottom)]))
        {
            return false;
        }

        // TODO: unlikely
        if (material.reflection.projection != ProjectionType::Unknown &&
            material.reflection.projection != ProjectionType::Cube)
        {
            logWarning("MTL file corrupted. Reflection map has both (sphere) and (%s) types specified.", string());
            return false;
        }

        material.reflection.projection = ProjectionType::Cube;
    }
    else
    if (isStringMatching("cube_front"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Front)]))
        {
            return false;
        }

        // TODO: unlikely
        if (material.reflection.projection != ProjectionType::Unknown &&
            material.reflection.projection != ProjectionType::Cube)
        {
            logWarning("MTL file corrupted. Reflection map has both (sphere) and (%s) types specified.", string());
            return false;
        }

        material.reflection.projection = ProjectionType::Cube;
    }
    else
    if (isStringMatching("cube_back"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Back)]))
        {
            return false;
        }

        // TODO: unlikely
        if (material.reflection.projection != ProjectionType::Unknown &&
            material.reflection.projection != ProjectionType::Cube)
        {
            logWarning("MTL file corrupted. Reflection map has both (sphere) and (%s) types specified.", string());
            return false;
        }

        material.reflection.projection = ProjectionType::Cube;
    }
    else
    if (isStringMatching("cube_left"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Left)]))
        {
            return false;
        }

        // TODO: unlikely
        if (material.reflection.projection != ProjectionType::Unknown &&
            material.reflection.projection != ProjectionType::Cube)
        {
            logWarning("MTL file corrupted. Reflection map has both (sphere) and (%s) types specified.", string());
            return false;
        }

        material.reflection.projection = ProjectionType::Cube;
    }
    else
    if (isStringMatching("cube_right"))
    {
        if (!parseMap(resources::MaterialComponent::Reflection, material.reflection.map[underlyingType(CubeFace::Right)]))
        {
            return false;
        }

        // TODO: unlikely
        if (material.reflection.projection != ProjectionType::Unknown &&
            material.reflection.projection != ProjectionType::Cube)
        {
            logWarning("MTL file corrupted. Reflection map has both (sphere) and (%s) types specified.", string());
            return false;
        }

        material.reflection.projection = ProjectionType::Cube;
    }
    else
    {
        logWarning("MTL file corrupted. Expected string property of parsed reflection map (-type specifier).");
        return false;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<mtl::Material*>* parseMTL(const uint8* buffer, const uint64 size)
{
    // Create parser to quickly process text from file
    ParserMTL parser(buffer, size);

    // Resulting array of parsed materials
    std::vector<mtl::Material*>* materials = new std::vector<mtl::Material*>();
    if (!materials)
    {
        logError("Could not allocate array of Material pointers!");
        return nullptr;
    }

    mtl::Material* material = nullptr;
    bool foundKa = false;
    bool foundKd = false;
    bool foundKs = false;
    bool foundTf = false;

    // Parses input file line by line
    ParserType type = ParserType::None;
    while (!parser.end())
    {
        type = parser.findNextElement();
        if (type == ParserType::String)
        {
            // Material
            if (parser.isStringMatching("newmtl"))
            {
                // If there was previous material being parsed,
                // it is being finalized and added to array.
                if (material)
                {
                    materials->push_back(material);
                    foundKa = false;
                    foundKd = false;
                    foundKs = false;
                    foundTf = false;
                }

                type = parser.findNextElement();
                if (type != ParserType::String)
                {
                    logWarning("MTL file corrupted. Failed to parse material name.");
                    delete materials;
                    return nullptr;
                }

                material = new mtl::Material();
                if (!material)
                {
                    logError("Run out of memory while allocating mtl::Material!");
                    delete materials;
                    return nullptr;
                }

                material->name = std::string(parser.string(), parser.stringLength());
            }
            else // Ambient constat
            if (parser.isStringMatching("Ka"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (foundKa)
                {
                    logWarning("MTL file corrupted. Ka property redefined.");
                    delete materials;
                    delete material;
                    return nullptr;
                }

                if (!parser.readVectorProperty(material->ambient.color.value))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }

                foundKa = true;
            }
            else
            if (parser.isStringMatching("Kd"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (foundKd)
                {
                    logWarning("MTL file corrupted. Kd property redefined.");
                    delete materials;
                    delete material;
                    return nullptr;
                }

                if (!parser.readVectorProperty(material->diffuse.color.value))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }

                foundKd = true;
            }
            else
            if (parser.isStringMatching("Ks"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (foundKs)
                {
                    logWarning("MTL file corrupted. Ks property redefined.");
                    delete materials;
                    delete material;
                    return nullptr;
                }

                if (!parser.readVectorProperty(material->specular.color.value))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }

                foundKs = true;
            }
            else
            if (parser.isStringMatching("Tf"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (foundTf)
                {
                    logWarning("MTL file corrupted. Tf property redefined.");
                    delete materials;
                    delete material;
                    return nullptr;
                }

                if (!parser.readVectorProperty(material->transmissionFilter.color.value))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }

                foundTf = true;
            }
            else
            if (parser.isStringMatching("illum"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                type = parser.findNextElement();
                if (type != ParserType::Integer)
                {
                    logWarning("MTL file corrupted. Expected illumination model number (illum).");
                    delete materials;
                    delete material;
                    return nullptr;
                }

                uint64 temp = 0;
                if (!parser.readU64(temp))
                {
                    logWarning("MTL file corrupted. Failed to read integer value for illumination model (illum).");
                    delete materials;
                    delete material;
                    return nullptr;
                }

                material->model = (IlluminationModel)temp;
            }
            else
            if (parser.isStringMatching("Ns"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                ParserType type = parser.findNextElement();
                if (type == ParserType::Integer)
                {
                    uint64 temp = 0;
                    if (!parser.readU64(temp))
                    {
                        logWarning("MTL file corrupted. Failed to read integer value of specular exponent (Ns).");
                        delete materials;
                        delete material;
                        return nullptr;
                    }

                    material->specular.exponent = (float)temp;
                }
                else // Its possible that exponent is provided as float value
                if (type == ParserType::Float)
                {
                    if (!parser.readF32(material->specular.exponent))
                    {
                        logWarning("MTL file corrupted. Failed to read float value of specular exponent (Ns).");
                        delete materials;
                        delete material;
                        return nullptr;
                    }
                }
                else
                {
                    logWarning("MTL file corrupted. Expected integer/float value for specular exponent (Ns).");
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Optical density (index of refraction)
            if (parser.isStringMatching("Ni"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                ParserType type = parser.findNextElement();
                if (type == ParserType::Float)
                {
                    if (!parser.readF32(material->transmissionFilter.indexOfRefraction))
                    {
                        logWarning("MTL file corrupted. Failed to read float value of Index Of Refraction (Ni).");
                        delete materials;
                        delete material;
                        return nullptr;
                    }
                }
                else
                {
                    logWarning("MTL file corrupted. Expected float for Index Of Refraction (Ni).");
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Dissolve (opacity)
            if (parser.isStringMatching("d"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseOpacityFactor(material->opacity))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Sharpness of the reflections from the local reflection map
            if (parser.isStringMatching("sharpness"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                ParserType type = parser.findNextElement();
                if (type == ParserType::Integer)
                {
                    uint64 temp = 0;
                    if (!parser.readU64(temp))
                    {
                        logWarning("MTL file corrupted. Failed to read integer value of reflection map sharpness (sharpness).");
                        delete materials;
                        delete material;
                        return nullptr;
                    }

                    material->reflection.sharpness = (uint16)temp;
                }
                else
                {
                    logWarning("MTL file corrupted. Expected integer value for reflection map sharpness (sharpness).");
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Ambient map (to be multiplied by Ka)
            if (parser.isStringMatching("map_Ka"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Ambient, material->ambient.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Diffuse map (to be multiplied by Kd)
            if (parser.isStringMatching("map_Kd"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Diffuse, material->diffuse.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Specular map (to be multiplied by Ks)
            if (parser.isStringMatching("map_Ks"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Specular, material->specular.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Specular exponent map (single channel R)
            if (parser.isStringMatching("map_Ns"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::SpecularExponent, material->specular.exponentMap))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else // Opacity map
            if (parser.isStringMatching("map_d"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Opacity, material->opacity.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else
            if (parser.isStringMatching("map_aat"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                type = parser.findNextElement();
                if (type == ParserType::String)
                {
                    if (parser.isStringMatching("on"))
                    {
                        material->antiAliasing = true;
                    }
                    else
                    {
                        logWarning("MTL file corrupted. Expected \"on\" for string specifying anty-aliasing (map_aat).");
                        delete materials;
                        delete material;
                        return nullptr;
                    }
                }
                else
                {
                    logWarning("MTL file corrupted. Expected string enabling anty-aliasing (map_aat).");
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else
            if (parser.isStringMatching("disp"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Height, material->height.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else
            if (parser.isStringMatching("decal"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Decal, material->decal.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else
            if (parser.isStringMatching("bump"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseMap(resources::MaterialComponent::Bump, material->bump.map))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
            else
            if (parser.isStringMatching("refl"))
            {
                if (!material)
                {
                    logWarning("MTL file corrupted. Missing material declaration before its properties are getting defined.");
                    delete materials;
                    return nullptr;
                }

                if (!parser.parseReflectionMap(*material))
                {
                    delete materials;
                    delete material;
                    return nullptr;
                }
            }
        }

        // Skip not relevant part of the line
        parser.skipToNextLine();
    }

    return materials;
}

// Performs engine specific checks
bool verifyMaterial(const mtl::Material& material)
{
    // TODO: check for spectral & xyz modifiers
    // TODO: uvwOffset, uvwScale, uvwTurbulence
    // TODO: -mm, -bm
    // TODO: -boost
    // TODO: texres
    // TODO: l/m channels

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<mtl::Material*>* load(const std::string& filename)
{
    using namespace en::storage;

    // Opens MTL file 
    File* file = Storage->open(filename);
    if (!file)
    {
        std::string fullPath = Resources->assetsPath() + filename;
        file = Storage->open(fullPath);
        if (!file)
        {
            logError("There is no such file!\n%s\n", fullPath.c_str());
            return nullptr;
        }
    }

    // Allocates temporary buffer for file content
    uint64 size = file->size();
    if (size > 0xFFFFFFFF)
    {
        logError("Maximum supported size of imported MTL file is 4GB!\n");
        return nullptr;
    }

    uint8* buffer = nullptr;
    buffer = new uint8[static_cast<uint32>(size)];
    if (!buffer)
    {
        logError("Not enough memory!\n");
        delete file;
        return nullptr; 
    }

    // Reads file to buffer and close file
    if (!file->read(buffer))
    {
        logError("Cannot read whole mtl file!\n");
        delete file;
        return nullptr;
    }
    delete file;

    // Parses file and extracts unoptimized material
    std::vector<mtl::Material*>* materials = parseMTL(buffer, size);
    if (!materials)
    {
        return nullptr;
    }

    for(uint32 i=0; i<materials->size(); ++i)
    {
        if (!verifyMaterial(*(materials[i])))
        {
            delete materials;
            return nullptr;
        }
    }

    return materials;
}


// TODO: DEPRECATE BELOW (once we have converter parsing unoptimized materials into engine specific ones - matching loaded shader?)


bool load(const std::string& filename, const std::string& name, en::resources::Material& material)
{
    using namespace en::storage;

    // Try to reuse already loaded material
    //if (ResourcesContext.materials.find(name) != ResourcesContext.materials.end())
    //   return ResourcesContext.materials[name];

    // Opens MTL file 
    File* file = Storage->open(filename);
    if (!file)
    {
        std::string fullPath = Resources->assetsPath() + filename;
        file = Storage->open(fullPath);
        if (!file)
        {
            logError("There is no such file!\n%s\n", fullPath.c_str());
            return nullptr;
        }
    }

    // Allocates temporary buffer for file content
    uint64 size = file->size();
    if (size > 0xFFFFFFFF)
    {
        logError("Maximum supported size of imported MTL file is 4GB!\n");
        return nullptr;
    }

    uint8* buffer = nullptr;
    buffer = new uint8[static_cast<uint32>(size)];
    if (!buffer)
    {
        logError("Not enough memory!\n");
        delete file;
        return false; //en::resources::Material();
    }

    // Read file to buffer and close file
    if (!file->read(buffer))
    {
        enLog << "ERROR: Cannot read whole mtl file!\n";
        delete file;
        return false; //en::resources::Material();
    }    
    delete file;


    // Step 1 - Parsing the file


    std::vector<mtl::Material*>* materials = parseMTL(buffer, size);
    if (!materials)
    {
        return nullptr;
    }


    // Step 2 - Loading materials









//    // MTL files doesn't contain shaders so attach default shader to it
//    material->program = ResourcesContext.defaults.program;
//    if (!material->program)
//    {
//        enLog << "ERROR: Cannot attach default shader to material!";
//        ResourcesContext.storage.materials.free(material);
//        return en::resources::Material();
//    }

//    // Calculate space required for program parameter values
//    gpu::Program program = material->program;
//    material->parameters.size = 0;
//    for(uint32 i=0; i<program.parameters(); ++i)
//    {
//        material->parameters.size += program.parameter(i).size();
//    }

//    // Allocate shared buffer for program parameter values
//    material->parameters.buffer = new uint8[material->parameters.size];
//    if (!material->parameters.buffer)
//    {
//        ResourcesContext.storage.materials.free(material);
//        return en::resources::Material();
//    }

//    // Add all program parameters with default values to material
//    uint32 offset = 0;
//    material->parameters.list.resize(program.parameters());
//    memset(material->parameters.buffer, 0, material->parameters.size);
//    for(uint32 i=0; i<program.parameters(); ++i)
//    {
//        en::resources::MaterialParameter& parameter = material->parameters.list[i];
//        parameter.handle = program.parameter(i);
//        parameter.value  = (void*)((uint8*)(material->parameters.buffer) + offset);
//        parameter.name   = parameter.handle.name();
//    
//        offset += program.parameter(i).size();
//    }  

    // WA: Ensure that the same texture is not used twice as ambient and diffuse map.
    std::string AmbientMapName;

    // Search for specified material in file
    std::string command;
    std::string word;
    bool eol = false;
    bool found = false;
    while(!text.end())
    {
        // Read commands until it is end of file
        if (!text.read(command, eol))
        {
            continue;
        }

        // Material name
        if (command == "newmtl")
        {
            if (!eol && text.read(word, eol))    
            { 
                // If material is still not found, check if 
                // this is it's searched declaration.
                if (!found)
                {
                    if (word == name)
                    {
                        found = true;
                    }
                }
                else
                // If material was already found and parsed,
                // this is declaration of next material which
                // means that the parsing is finished.
                {
                    break;
                }
            }
        }

        if (found)
        {
            // Ambient texture
            if (command == "map_Ka")
            {
                if (!eol && text.read(word, eol))
                {
                    // Read last word in line
                    while(!eol)
                    {
                        text.read(word, eol);
                    }
 
                    //material.ambient.map = en::Resources.load.texture(word);
                    //if (material.ambient.map)
                    {
                        // WA: Store information about used texture, to ensure that 
                        //     it won't be used twice as ambient and diffuse map.
                        AmbientMapName = word;
                    }

                    //resources::MaterialSampler parameter;
                    //parameter.handle = material->program.sampler(string("enAmbientMap"));
                    //if (parameter.handle)
                    //{
                    //    parameter.texture = en::Resources.load.texture(word);
                    //    parameter.name    = parameter.handle.name();
                    //    if (parameter.texture)
                    //    {
                    //        material->samplers.push_back(parameter);
                    //        material->state.texture.ambient = true;
                    
                    //        // WA: Store information about used texture, to ensure that 
                    //        //     it won't be used twice as ambient and diffuse map.
                    //        AmbientMapName = word;
                    //    }
                    //}         
                }     
            }

            // Diffuse texture
            if (command == "map_Kd")
            {
                if (!eol && text.read(word, eol))
                {
                    // Read last word in line
                    while(!eol)
                    {
                        text.read(word, eol);
                    }

                    // WA: Ensure that the same texture won't be used twice,
                    //     as ambient and diffuse map.
                    //if (material.ambient.map)
                    //{
                    //    if (AmbientMapName == word)
                    //    {
                    //        material.ambient.map = en::ResourcesContext.defaults.enAmbientMap;
                    //    }
                    //}

                    material.albedo = en::Resources.load.texture(word);

                    //resources::MaterialSampler parameter;
                    //parameter.handle = material->program.sampler(string("enDiffuseMap"));
                    //if (parameter.handle)
                    //   {
                    //   // WA: Ensure that the same texture won't be used twice,
                    //   //     as ambient and diffuse map.
                    //   if (material->state.texture.ambient)
                    //      if (AmbientMapName == word)
                    //         for(uint8 i=0; i<material->samplers.size(); ++i)
                    //            if (material->samplers[i].name == word.c_str())
                    //               {
                    //               material->samplers[i].texture = en::ResourcesContext.defaults.enAmbientMap;
                    //               break;
                    //               }
                    //
                    //   parameter.texture = en::Resources.load.texture(word);
                    //   parameter.name    = parameter.handle.name();
                    //   if (parameter.texture)
                    //      {
                    //      material->samplers.push_back(parameter);
                    //      material->state.texture.diffuse = true;
                    //      }
                    //   }         
                }
            }

            // // Specular texture
            // if (command == "map_Ks")
            //    if (!eol && text.read(word, eol))
            //       {
            //       // Read last word in line
            //       while(!eol)
            //            text.read(word, eol);
            // 
            //       material.specular = en::Resources.load.texture(word);
            // 
            ///*     resources::MaterialSampler parameter;
            //       parameter.handle = material->program.sampler(string("enSpecularMap"));
            //       if (parameter.handle)
            //          {
            //          parameter.texture = en::Resources.load.texture(word);
            //          parameter.name    = parameter.handle.name();
            //          if (parameter.texture)
            //             {
            //             material->samplers.push_back(parameter);
            //             material->state.texture.specular = true;
            //             }
            //          } */        
            //       }

            // Normalmap / bumpmap texture
            if ( command == "map_bump" ||
                 command == "bump" )
            {
                if (!eol && text.read(word, eol))
                {
                    // Read last word in line
                    while(!eol)
                    {
                        text.read(word, eol);
                    }

                    material.normal = en::Resources.load.texture(word);

                 /* resources::MaterialSampler parameter;
                    parameter.handle = material->program.sampler(string("enNormalMap"));
                    if (parameter.handle)
                    {
                        parameter.texture = en::Resources.load.texture(word);
                        parameter.name    = parameter.handle.name();
                        if (parameter.texture)
                        {
                            material->samplers.push_back(parameter);
                            material->state.texture.normal = true;
                        }
                    }*/         
                }
            }

            // Displacement / heightmap texture
            if ( command == "decal" ||
                 command == "disp" )
            {
                if (!eol && text.read(word, eol))
                {
                    // Read last word in line
                    while(!eol)
                    {
                        text.read(word, eol);
                    }

                    material.displacement = en::Resources.load.texture(word);

/*                  resources::MaterialSampler parameter;
                    parameter.handle = material->program.sampler(string("enDisplacementMap"));
                    if (parameter.handle)
                    {
                        parameter.texture = en::Resources.load.texture(word);
                        parameter.name    = parameter.handle.name();
                        if (parameter.texture)
                        {
                            material->samplers.push_back(parameter);
                            material->state.texture.displacement = true;
                        }
                    } */        
                }
            }

            // Alpha channel texture
            if (command == "map_d")
            {
                if (!eol && text.read(word, eol))
                {
                    // Read last word in line
                    while(!eol)
                    {
                        text.read(word, eol);
                    }

                    material.opacity = en::Resources.load.texture(word);

/*                  resources::MaterialSampler parameter;
                    parameter.handle = material->program.sampler(string("enAlphaMap"));
                    if (parameter.handle)
                    {
                        parameter.texture = en::Resources.load.texture(word);
                        parameter.name    = parameter.handle.name();
                        if (parameter.texture)
                        {
                            material->samplers.push_back(parameter);
                            material->state.texture.alpha = true;
                        }
                    } */        
                }
            }

            // Not supported in PBR Model

            // // Emmisive coefficient
            // if (command == "Ke")
            //    if (!eol && text.read(word, eol))
            //       {
            //       //float3 coefficient(0.0f, 0.0f, 0.0f);
            //       float3& coefficient = material.emmisive.color;
            //       coefficient.r = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.g = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.b = stringTo<float>(word);
            // 
            //       //// Overload with value from material file
            //       //for(uint32 i=0; i<material->program.parameters(); ++i)
            //       //   {
            //       //   en::resources::MaterialParameter& parameter = material->parameters.list[i];
            //       //   if (strcmp(parameter.name, "enKe") == 0)
            //       //      *((float3*)parameter.value) = coefficient;
            //       //   }
            //       }
            // 
            // // Ambient coefficient
            // if (command == "Ka")
            //    if (!eol && text.read(word, eol))
            //       {
            //       //float3 coefficient(0.0f, 0.0f, 0.0f);
            //       float3& coefficient = material.ambient.color;
            //       coefficient.r = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.g = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.b = stringTo<float>(word);
            // 
            //       //// Overload with value from material file
            //       //for(uint32 i=0; i<material->program.parameters(); ++i)
            //       //   {
            //       //   en::resources::MaterialParameter& parameter = material->parameters.list[i];
            //       //   if (strcmp(parameter.name, "enKa") == 0)
            //       //      *((float3*)parameter.value) = coefficient;
            //       //   }       
            //       }
            // 
            // // Diffuse coefficient
            // if (command == "Kd")
            //    if (!eol && text.read(word, eol))
            //       {
            //       //float3 coefficient(0.0f, 0.0f, 0.0f);
            //       float3& coefficient = material.diffuse.color;
            //       coefficient.r = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.g = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.b = stringTo<float>(word);
            // 
            //       //// Overload with value from material file
            //       //for(uint32 i=0; i<material->program.parameters(); ++i)
            //       //   {
            //       //   en::resources::MaterialParameter& parameter = material->parameters.list[i];
            //       //   if (strcmp(parameter.name, "enKd") == 0)
            //       //      *((float3*)parameter.value) = coefficient;
            //       //   }        
            //       }
            // 
            // // Specular coefficient
            // if (command == "Ks")
            //    if (!eol && text.read(word, eol))
            //       {
            //       //float3 coefficient(0.0f, 0.0f, 0.0f);
            //       float3& coefficient = material.specular.color;
            //       coefficient.r = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.g = stringTo<float>(word);
            //       if (!eol && text.read(word, eol))
            //          coefficient.b = stringTo<float>(word);
            // 
            //       //// Overload with value from material file
            //       //for(uint32 i=0; i<material->program.parameters(); ++i)
            //       //   {
            //       //   en::resources::MaterialParameter& parameter = material->parameters.list[i];
            //       //   if (strcmp(parameter.name, "enKs") == 0)
            //       //      *((float3*)parameter.value) = coefficient;
            //       //   }        
            //       }
            // 
            // // Specular exponenr
            // if (command == "Ns")
            //    if (!eol && text.read(word, eol))
            //       {
            //       material.specular.exponent = stringTo<float>(word);
            // 
            //       //// Overload with value from material file
            //       //for(uint32 i=0; i<material->program.parameters(); ++i)
            //       //   {
            //       //   en::resources::MaterialParameter& parameter = material->parameters.list[i];
            //       //   if (strcmp(parameter.name, "enShininess") == 0)
            //       //      *((float*)parameter.value) = stringTo<float>(word);
            //       //   }        
            //       }
            //
            // // Transparency "dissolve"
            // if (command == "d")
            //    if (!eol && text.read(word, eol))
            //       {
            //       float3& coefficient = material.transparency.color;
            //       coefficient.r = stringTo<float>(word);
            //       coefficient.g = stringTo<float>(word);
            //       coefficient.b = stringTo<float>(word);
            //       }

            // float3 translucency;         // Tf r g b  - transmission filter, which colors are filtered when light goes through object          
            // float  refraction;           // Ni - optical density  [0.001 - 10]  1.0 - light does not bend as it passes through an object
        }

        // Skip not relevant part of the line
        text.skipToNextLine();    
    }

    // Attach default values to parameters not found in material
    // TODO: Finish!!!
    //if (!material->state.texture.specular)
    //   {
    //   resources::MaterialSampler parameter;
    //   parameter.handle = material->program.sampler(string("enSpecularMap"));
    //   if (parameter.handle)
    //      {
    //      parameter.texture = en::ResourcesContext.defaults.enSpecularMap;
    //      parameter.name    = parameter.handle.name();
    //      if (parameter.texture)
    //         {
    //         material->samplers.push_back(parameter);
    //         material->state.texture.specular = true;
    //         }
    //      }         
    //   }
    //if (!material->state.texture.normal)
    //   {
    //   resources::MaterialSampler parameter;
    //   parameter.handle = material->program.sampler(string("enNormalMap"));
    //   if (parameter.handle)
    //      {
    //      parameter.texture = en::ResourcesContext.defaults.enNormalMap;
    //      parameter.name    = parameter.handle.name();
    //      if (parameter.texture)
    //         {
    //         material->samplers.push_back(parameter);
    //         material->state.texture.normal = true;
    //         }
    //      }         
    //   }



    //if (!material->state.texture.alpha)
    //   {
    //   resources::MaterialSampler parameter;
    //   parameter.handle = material->program.sampler(string("enAlphaMap"));
    //   if (parameter.handle)
    //      {
    //      parameter.texture = en::ResourcesContext.defaults.enAlphaMap;
    //      parameter.name    = parameter.handle.name();
    //      if (parameter.texture)
    //         {
    //         material->samplers.push_back(parameter);
    //         material->state.texture.alpha = true;
    //         }
    //      }         
    //   }

    // If material was not found in this file return nothing
    if (!found)
    {
        enLog << "Error! Material \"" << name << "\" was not found in file: " << filename << " !\n";
        //ResourcesContext.storage.materials.free(material);
        return false; //en::resources::Material();
    }

    //// Update list of loaded materials
    //ResourcesContext.materials.insert(pair<string, en::resources::Material>(name, material));

    // Return material interface
    return true; //en::resources::Material(material);
}

} // en::mtl
} // en

// Creating material:
// + check if material already exist in resources manager
// ~ check if material has information about correct shader for current platform ( es, ogl, dx?)
// - try to compile shader
// + if there is no shader, or it cannot compile correctly
//   use default engine shader for this material
// - for each value in material file, find active parameter or sampler in shader program
//   if it is found, add it to material parameters/samplers list and set it's default value/binding
// - for each active parameter/sampler in shader program, if not already in
//   material parameter list, try to match engine default parameter value/binding
// - report all errors

// TODO: Iteration by program parameters/samplers, taking their names and types

   // object -> (Uses) model
   // - private        - reference
   //   copy of          model material
   //   material         holds default
   //  (overloads        values
   //   material       - here are stored
   //   of model)        default values
   // - here are
   //   stored 
   //   values of
   //   material
   //   parameters
