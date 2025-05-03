/*

 Ngine v5.0
 
 Module      : OBJ files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading models from *.obj files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/rendering/device.h"
#include "core/utilities/parser.h"
#include "utilities/strings.h"
#include "utilities/gpcpu/gpcpu.h"
#include "resources/context.h" 
#include "resources/forsyth.h" 
#include "resources/obj.h"     
#include "resources/mtl.h"     

namespace en
{
namespace obj
{

bool optimizeIndexSize  = true;   // Reduces index size UInt -> UShort -> UByte

bool optimizeIndexOrder = true;   // Optimizes indexes order for Post-Transform Vertex Cache Size

// Internal, custom OBJ vertex representation.
// TODO: Unify as "UnpackedVertex"
struct Vertex
{
    // Index 0 means that given component is not used by vertex.

    uint32 position; // Position
    uint32 uv;       // Texture Coordinate
    uint32 normal;   // Normal vector
    
    bool operator ==(const Vertex& b);
};

// Internal, custom OBJ face representation.
// TODO: Unify as "UnpackedFace"
struct Face
{
    Vertex vertex[3]; // Vertex
    uint32 material;  // Material
};

//struct Material
//{
//    std::string                 name;   // Material name
//    en::resources::Material handle; // Material handle
//};

// Internal, custom OBJ mesh representation.
// Stores unoptimized contents after parsing.
// TODO: Unify as "UnpackedMesh"
struct Mesh
{
    std::string         name;             // Mesh name
    std::string         material;         // Material name (only one per group allowed)
    std::vector<Vertex> vertices;         // Array of vertices composition data
    std::vector<float3> tangents;         // Tangents (uncompressed, generated from UVs)
    std::vector<float3> bitangents;       // BiTangents (uncompressed, generated from UVs)
    std::vector<uint32> indexes;          // Array of indexes
    std::vector<uint32> optimized;        // Array of indexes after Forsyth optimization
    bool                hasTextureCoords; // Does mesh contain texture coordinates 
    bool                hasTextureCoordV; // Does mesh uses 2 component texture coordinates
    bool                hasTextureCoordW; // Does mesh uses 3 component texture coordinates
    bool                hasNormals;       // Does mesh contain normal vectors

    Mesh();
};

// Internal, custom OBJ model representation.
// Stores unoptimized contents after parsing.
// TODO: Unify as "UnpackedModel"
struct Model
{
    std::vector<float3> vertices;                  // Vertices (uncompressed)
    std::vector<float3> normals;                   // Normals (uncompressed)
    std::vector<float3> coordinates;               // Texture coordinates (uncompressed)
    std::vector<std::string> libraries;            // Material libraries
    std::vector<en::obj::Mesh> meshes;             // Meshes
    std::vector<en::resources::Material> materials; // Materials <- TODO: This should be OBJ internal temp material? OR reference to final material stored in some global arrray

    Model();

    obj::Mesh* addMesh(void);
    obj::Mesh* currentMesh(void);
};

bool Vertex::operator ==(const Vertex& b)
{
    return !memcmp(this, &b, 12); // 12 = sizeof(obj::Vertex)
}

Mesh::Mesh() :
    hasTextureCoords(false),
    hasTextureCoordV(false),
    hasTextureCoordW(false),
    hasNormals(false)
{
}

Model::Model()
{
    // Reserve memory for incoming data, to minimise
    // occurences of possible relocations during 
    // vectors growth
    vertices.reserve(16384);
    normals.reserve(16384);
    coordinates.reserve(16384);
    libraries.reserve(16);
    meshes.reserve(16);
    materials.reserve(32);

    // Model always has minimum one mesh
    obj::Mesh* mesh = addMesh();

    // Fill first mesh descriptor with default data
    mesh->name = "default";
    mesh->material = "default";
    mesh->vertices.reserve(8192);
    mesh->indexes.reserve(8192);
    mesh->optimized.reserve(8192);
    mesh->hasTextureCoords = false;
    mesh->hasTextureCoordW = false;
    mesh->hasNormals = false;
}

obj::Mesh* Model::addMesh(void)
{
    meshes.push_back(en::obj::Mesh());  // TODO: Shouldn't it be: new Mesh() ???
    return &meshes[meshes.size() - 1];
}

obj::Mesh* Model::currentMesh(void)
{
    return &meshes[meshes.size() - 1];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// OBJ File format specification:
// https://paulbourke.net/dataformats/obj/
// https://paulbourke.net/dataformats/obj/obj_spec.pdf
//
class ParserOBJ : public Parser
{

    public:

    // Passes ownership of buffer to parser
    ParserOBJ(const uint8* buffer, const uint64 size);

    bool readVector3f(float(&vector)[3]);
    bool readTextureCoordinates(float(&vector)[3]);
    bool parseGroupName(const char*& name, uint32& length);
    bool parseMaterialLibraryNames(obj::Model& model);
    bool parseVertex(obj::Vertex& vertex, bool& hasTextureCoords, bool& hasNormals);
    bool parseFace(obj::Mesh& mesh);
};

ParserOBJ::ParserOBJ(const uint8* buffer, const uint64 size) :
    Parser(buffer, size)
{
}

bool ParserOBJ::readVector3f(float(&vector)[3])
{
    for(uint32 i=0; i<3; ++i)
    {
        ParserType type = findNextElement();
        if (type == ParserType::Float)
        {
            if (!readF32(vector[i]))
            {
                // TODO: logError("Vector parsing failed. Failed to read float value for %i component.", i);
                return false;
            }
        }
        else // Its possible that float value is represented with integer (for e.g. simply "0").
        if (type == ParserType::Integer)
        {
            sint64 temp = 0;
            if (!readS64(temp))
            {
                // TODO: logError("Vector parsing failed. Failed to read integer value for %i component.", i);
                return false;
            }

            vector[i] = (float)temp;
        }
        else
        {
            // TODO: logError("Vector parsing failed. Expected float value for %i component.", i);
            return false;
        }
    }

    return true;
}

bool ParserOBJ::readTextureCoordinates(float(&vector)[3])
{
    // Component U (mandatory):

    ParserType type = findNextElement();
    if (type == ParserType::Float)
    {
        if (!readF32(vector[0]))
        {
            // TODO: logError("Vector parsing failed. Failed to read float value for U component.", i);
            return false;
        }
    }
    else // Its possible that float value is represented with integer (for e.g. simply "0").
    if (type == ParserType::Integer)
    {
        sint64 temp = 0;
        if (!readS64(temp))
        {
            // TODO: logError("Vector parsing failed. Failed to read integer value for U component.", i);
            return false;
        }

        vector[0] = (float)temp;
    }
    else
    {
        // TODO: logError("Vector parsing failed. Expected float value for U component.", i);
        return false;
    }

    // Component V (optional):

    vector[1] = NAN;
    vector[2] = NAN;
    type = findNextElement();
    if (type == ParserType::Float)
    {
        if (!readF32(vector[1]))
        {
            // TODO: logError("Vector parsing failed. Failed to read float value for V component.", i);
            return false;
        }
    }
    else // Its possible that float value is represented with integer (for e.g. simply "0").
    if (type == ParserType::Integer)
    {
        sint64 temp = 0;
        if (!readS64(temp))
        {
            // TODO: logError("Vector parsing failed. Failed to read integer value for V component.", i);
            return false;
        }

        vector[1] = (float)temp;
    }

    if (!isnan(vector[1]))
    {
        // Component W (optional):

        type = findNextElement();
        if (type == ParserType::Float)
        {
            if (!readF32(vector[2]))
            {
                // TODO: logError("Vector parsing failed. Failed to read float value for W component.", i);
                return false;
            }
        }
        else // Its possible that float value is represented with integer (for e.g. simply "0").
        if (type == ParserType::Integer)
        {
            sint64 temp = 0;
            if (!readS64(temp))
            {
                // TODO: logError("Vector parsing failed. Failed to read integer value for W component.", i);
                return false;
            }

            vector[2] = (float)temp;
        }
    }

    return true;
}

bool ParserOBJ::parseGroupName(const char*& name, uint32& length)
{
    ParserType type = ParserType::None;
    uint32 namesCount = 0;
    do
    {
        type = findNextElement();
        if (type == ParserType::String)
        {
            if (namesCount == 0)
            {
                name   = string();
                length = stringLength();
            }
            else
            {
                // TODO: logDebug("Multiple group names detected. Group name %u: %s.", namesCount, string());
            }

            namesCount++;
        }
    } // WA for OBJ files storing "g" without name as last element in the file. This is not conforming to OBJ specification. We will prune such empty mesh in final model gemeration.
    while(type != ParserType::None &&
          type != ParserType::EndOfLine);

    if (namesCount == 0)
    {
        // At least one group need needs to be specified.
        // See specification page B1-41: https://paulbourke.net/dataformats/obj/obj_spec.pdf
        // logError("OBJ file corrupted. Group name not found.");

        // Parser returns true, as OBJ loader has relaxed this requirement.
        // For each unnamed group, "unnamedN" name is assigned in automatic way.
        return true;
    }

    if (namesCount > 1)
    {
        // logError("Unsupported OBJ file. Multiple group names are not supported.");
        return false;
    }

    return true;
}

bool ParserOBJ::parseMaterialLibraryNames(obj::Model& model)
{
    ParserType type = ParserType::None;
    uint32 namesCount = 0;
    do
    {
        type = findNextElement();
        if (type == ParserType::String)
        {
            std::string libraryName = std::string(string(), stringLength());

            // Prevent from adding the same library more than once
            bool found = false;
            for (uint32 i = 0; i < model.libraries.size(); ++i)
            {
                if (model.libraries[i] == libraryName)
                {
                    found = true;
                    break;
                }
            }

            // Add new library to the list
            if (!found)
            {
                model.libraries.push_back(libraryName);
            }

            namesCount++;
        }
    } 
    while(type != ParserType::EndOfLine);

    if (namesCount == 0)
    {
        // logError("OBJ file corrupted. Library name not found.");
        return false;
    }

    return true;
}

bool ParserOBJ::parseVertex(obj::Vertex& vertex, bool& hasTextureCoords, bool& hasNormals)
{
    if (type != ParserType::Integer)
    {
        // Expected vertex index (mandatory).
        return false;
    }

    sint64 temp = 0;
    if (!readS64(temp))
    {
        // Failed to read vertex index.
        return false;
    }

    if (temp < 0)
    {
        // Relative vertex indices are not supported.
        return false;
    }

    if (temp > 0xFFFFFFFF)
    {
        // Vertex index too big.
        return false;
    }

    vertex.position = (uint32)temp;

    if (buffer[offset] != '/')
    {
        hasTextureCoords = false;
        hasNormals       = false;

        if (buffer[offset] == ' ')
        {
            return true;
        }
        else
        {
            // Corrupted OBJ file. Exptected whitespace after vertex index.
            return false;
        }
    }

    // Case v/...
    offset++;

    // Case: v/-.... OR v/c...
    if (buffer[offset] == '-' || isCypher(buffer[offset]))
    {
        type = findNextElement();
        if (type != ParserType::Integer)
        {
            // Expected texture coordinates index
            return false;
        }

        if (!readS64(temp))
        {
            // Failed to read texture coordinates index.
            return false;
        }

        if (temp < 0)
        {
            // Relative texture coordinates indices are not supported.
            return false;
        }

        if (temp > 0xFFFFFFFF)
        {
            // Texture coordinates index too big.
            return false;
        }

        vertex.uv = (uint32)temp;
        hasTextureCoords = true;
    }
    else 
    {
        hasTextureCoords = false;

        if (buffer[offset] != '/')
        {
            // OBJ file is corrupted. Expected integer index of texture coordinates while parsing face vertex.
            return false;
        }
    }

    // Case: v//... OR v/t/....
    offset++;

    // Case: v//-... OR v/t/c...
    if (buffer[offset] == '-' || isCypher(buffer[offset]))
    {
        type = findNextElement();
        if (type != ParserType::Integer)
        {
            // Expected normal index
            return false;
        }

        if (!readS64(temp))
        {
            // Failed to read normal index.
            return false;
        }

        if (temp < 0)
        {
            // Relative normal indices are not supported.
            return false;
        }

        if (temp > 0xFFFFFFFF)
        {
            // Normal index too big.
            return false;
        }

        vertex.normal = (uint32)temp;
        hasNormals = true;
        return true;
    }

    hasNormals = false;

    // OBJ file is corrupted. Expected integer index of normal while parsing face vertex.
    return false;
}

bool ParserOBJ::parseFace(obj::Mesh& mesh)
{
    obj::Vertex vertex;

    bool faceHasTextureCoordinates = false;
    bool faceHasNormals            = false;
    uint32 vertexCount = 0;
    while(findNextElement() != ParserType::EndOfLine)
    {
        bool vertexHasTextureCoords = false;
        bool vertexHasNormals       = false;
        if (!parseVertex(vertex, vertexHasTextureCoords, vertexHasNormals))
        {
            return false;
        }

        if (vertexCount == 0)
        {
            // First vertex determines if face uses texture coordinates 
            // (without specifying if those are U, UV or UVW) and normals
            faceHasTextureCoordinates = vertexHasTextureCoords;
            faceHasNormals            = vertexHasNormals;
        }
        else // This is N-th vertex of this face
        {
            if (faceHasTextureCoordinates != vertexHasTextureCoords ||
                faceHasNormals != vertexHasNormals)
            {
                // All vertices in given face needs to have the same sub-components.
                return false;
            }
        }

        // This is first face in a mesh
        if (mesh.vertices.empty())
        {
            mesh.hasTextureCoords = faceHasTextureCoordinates;
            mesh.hasNormals       = faceHasNormals;
        }
        else // This is first vertex of N-th face
        if (vertexCount == 0)
        {
            if (mesh.hasTextureCoords != faceHasTextureCoordinates ||
                mesh.hasNormals != faceHasNormals)
            {
                // All faces in given mesh needs to have the same sub-components.
                return false;
            }

            // TODO: Verify that all vertices of all faces have the same TC components count (U, UV or UVW).
            //       This needs to be done on obj::Model level.
        }

        // First triangle is generated from first three
        // vertices of the face. If there is more of them,
        // face is divided into triangles on a basis of
        // triangle fan.
        if (vertexCount == 3)
        {
            uint64 size   = mesh.indexes.size();
            uint32 indexA = mesh.indexes[size - 3];
            uint32 indexB = mesh.indexes[size - 1];
            mesh.indexes.push_back(indexA);
            mesh.indexes.push_back(indexB);
            vertexCount = 2;
        }

        // Try to reuse vertex if it already exist
        bool found = false;
        for(uint32 i = 0; i < mesh.vertices.size(); ++i)
        {
            if (mesh.vertices[i] == vertex)
            {
                mesh.indexes.push_back(i);
                found = true;
                break;
            }
        }

        // If vertex is new, add it to vertex array
        if (!found)
        {
            mesh.indexes.push_back(static_cast<uint32>(mesh.vertices.size()));
            mesh.vertices.push_back(vertex);
        }

        vertexCount++;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


obj::Model* parseOBJ(const uint8* buffer, const uint64 size)
{
    // Create parser to quickly process text from file
    ParserOBJ parser(buffer, size);

    // Resulting unoptimized OBJ model
    obj::Model* result = new obj::Model();
    if (!result)
    {
        return nullptr;
    }

    // Current mesh and material handles
    obj::Mesh* mesh = result->currentMesh();
    std::string material = "default";

    // Collects geometry primitives data that will
    // be used to generate final meshes and perform
    // primitives assembly into unoptimized meshes
    std::string command;
    std::string word;
    uint32 unnamedMeshes = 0;
    bool eol = false;

    // Parses input file line by line
    ParserType type = ParserType::None;
    while (!parser.end())
    {
        type = parser.findNextElement();
        if (type == ParserType::String)
        {
            // Vertex
            if (parser.isStringMatching("v"))
            {
                // TODO: Ignores optional W component.
                float3 vertex(0.0f, 0.0f, 0.0f);
                if (!parser.readVector3f(vertex.value))
                {
                    // TODO: logError("OBJ file corrupted. Failed to parse vertex vector.");
                    delete result;
                    return nullptr;
                }

                result->vertices.push_back(vertex);
            }
            else // Normal vector
            if (parser.isStringMatching("vn"))
            {
                // All three components are mandatory
                float3 normal(0.0f, 0.0f, 0.0f);
                if (!parser.readVector3f(normal.value))
                {
                    // TODO: logError("OBJ file corrupted. Failed to parse normal vector.");
                    delete result;
                    return nullptr;
                }

                result->normals.push_back(normal);
            }
            else // Texture coordinates
            if (parser.isStringMatching("vt"))
            {
                // NAN indicates that given component is not used
                float3 coord(0.0f, NAN, NAN);
                if (!parser.readTextureCoordinates(coord.value))
                {
                    // TODO: logError("OBJ file corrupted. Failed to parse texture coordinates.");
                    delete result;
                    return nullptr;
                }

                result->coordinates.push_back(coord);
            }
            else // Group (expected grouping by material - mesh)
            if (parser.isStringMatching("g"))
            {
                const char* groupName = nullptr;
                uint32 groupLength = 0;
                if (!parser.parseGroupName(groupName, groupLength))
                {
                    delete result;
                    return nullptr;
                }

                // If model specifies mesh before any face
                // occurence, default mesh is not needed and
                // it can be reused.
                if (mesh->indexes.size() == 0)
                {
                    if (!groupName)
                    {
                        // If group is not named, substitute name with "unnamedN" where N is the index.
                        mesh->name = std::string("unnamed");
                        mesh->name += stringFrom(unnamedMeshes);
                        unnamedMeshes++;
                    }
                    else
                    {
                        mesh->name = std::string(groupName, groupLength);
                    }
                }
                else
                {
                    // Push new mesh description on the stack
                    mesh = result->addMesh();

                    // Fill mesh descriptor with default data
                    if (!groupName)
                    {
                        // If group is not named, substitute name with "unnamedN" where N is the index.
                        mesh->name = std::string("unnamed");
                        mesh->name += stringFrom(unnamedMeshes);
                        unnamedMeshes++;
                    }
                    else
                    {
                        mesh->name = std::string(groupName, groupLength);
                    }
                    mesh->material = material;
                    mesh->vertices.reserve(8192);
                    mesh->indexes.reserve(8192);
                    mesh->optimized.reserve(8192);
                    mesh->hasTextureCoords = false;
                    mesh->hasTextureCoordW = false;
                    mesh->hasNormals = false;
                }
            }
            else // Add materials library to the list
            if (parser.isStringMatching("mtllib"))
            {
                const char* groupName = nullptr;
                uint32 groupLength = 0;
                if (!parser.parseMaterialLibraryNames(*result))
                {
                    delete result;
                    return nullptr;
                }
            }
            else // Set current material
            if (parser.isStringMatching("usemtl"))
            {
                ParserType type = parser.findNextElement();
                if (type != ParserType::String)
                {
                    // logError("OBJ file corrupted. Expected name of material to use.");
                    delete result;
                    return nullptr;
                }
                
                material = std::string(parser.string(), parser.stringLength());

                // Prevent from adding the same material more than once
                bool found = false;
                for(uint32 i=0; i<result->materials.size(); ++i)
                {
                    if (result->materials[i].name == material)
                    {
                        found = true;
                        break;
                    }
                }

                // Add new material to the list
                if (!found)
                {
                    //obj::Material material;
                    en::resources::Material enMaterial;
                    enMaterial.name = material;
                    //material.handle = NULL;

                    result->materials.push_back(enMaterial);
                }

                // Mesh should specify material before any 
                // face will occur or use previous material
                // for whole mesh.
                if (mesh->indexes.size() == 0)
                {
                    mesh->material = material;
                }

            }
            else // Primitive Assembly (faces)
            if (parser.isStringMatching("f"))
            {
                if (!parser.parseFace(*mesh))
                {
                    // TODO: logError("OBJ file corrupted. Failed to parse face.");
                    delete result;
                    return nullptr;
                }
            }
        }

        // Skip not relevant part of the line
        parser.skipToNextLine();
    }

    // Removes meshes that are empty
    // This is WA for case when OBJ file is ended with empty group "g"
    std::vector<obj::Mesh>::iterator it = result->meshes.begin();
    while(it != result->meshes.end())
    {
        if (it->vertices.empty())
        {
            it = result->meshes.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Verify that all vertices of each mesh have the same amount of components 
    // (in case of texture coordinates, determines if they are U, UV or UVW).
    for(uint32 i=0; i<result->meshes.size(); ++i)
    {
        obj::Mesh& mesh = result->meshes[i];

        // First vertex in the mesh, determines components that all
        // following vertices in that mesh should have set up.
        uint32 coordsId = mesh.vertices[0].uv - 1;
        if (!isnan(result->coordinates[coordsId].v))
        {
            mesh.hasTextureCoordV = true;
        }
        if (!isnan(result->coordinates[coordsId].w))
        {
            mesh.hasTextureCoordW = true;
        }

        for(uint32 j=0; j<mesh.vertices.size(); ++j)
        {
            // Position is mandatory
            if (mesh.vertices[j].position == 0)
            {
                // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i doesn't have position specified. Position is mandatory.", i, j);
                delete result;
                return nullptr;
            }

            uint32 vertexId = mesh.vertices[j].position - 1;
            if (isnan(result->vertices[vertexId].x) ||
                isnan(result->vertices[vertexId].y) ||
                isnan(result->vertices[vertexId].z))
            {
                // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i points to model vertex %i which has invalid data.", i, j, vertexId);
                delete result;
                return nullptr;
            }

            if (mesh.hasTextureCoords)
            {
                if (mesh.vertices[j].uv == 0)
                {
                    // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i doesn't have texture coordinates specified. This is inconsistent with other vertices.", i, j);
                    delete result;
                    return nullptr;
                }

                uint32 coordsId = mesh.vertices[j].uv - 1;
                if (mesh.hasTextureCoordV &&
                    isnan(result->coordinates[coordsId].v))
                {
                    // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i points to model texture coordinates %i which has missing V component. This is inconsistent with other vertices.", i, j, coordsId);
                    delete result;
                    return nullptr;
                }
                if (mesh.hasTextureCoordW &&
                    isnan(result->coordinates[coordsId].w))
                {
                    // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i points to model texture coordinates %i which has missing W component. This is inconsistent with other vertices.", i, j, coordsId);
                    delete result;
                    return nullptr;
                }
            }

            if (mesh.hasNormals)
            {
                if (mesh.vertices[j].normal == 0)
                {
                    // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i doesn't have normal specified. This is inconsistent with other vertices.", i, j);
                    delete result;
                    return nullptr;
                }

                uint32 normalId = mesh.vertices[j].normal - 1;
                if (isnan(result->normals[normalId].x) ||
                    isnan(result->normals[normalId].y) ||
                    isnan(result->normals[normalId].z))
                {
                    // logDebug("OBJ file corrupted. Parsed mesh %i vertice %i points to model normal %i which has invalid data (%f %f %f).", i, j, normalId, result->normals[normalId].x, result->normals[normalId].y, result->normals[normalId].z);
                    delete result;
                    return nullptr;
                }
            }
        }
    }

    return result;
}

bool verifyModel(const obj::Model& srcModel)
{
    // Verify imported model
    if (srcModel.meshes.empty())
    {
        // logError("Unoptimized model is always expected to have at least one mesh.");
        return false;
    }

    for (uint32 i = 0; i < srcModel.meshes.size(); ++i)
    {
        const obj::Mesh& srcMesh = srcModel.meshes[i];

        if (srcMesh.vertices.empty())
        {
            // logError("Unoptimized mesh is always expected to have position component.");
            return false;
        }

        if (srcMesh.vertices.size() >= 0xFFFFFFFF)
        {
            // logError("Unoptimized mesh vertex count is exceeding maximum (2^32-1): %u.", srcMesh.vertices.size());
            return false;
        }

        if (srcMesh.indexes.size() >= 0xFFFFFFFF)
        {
            // logError("Unoptimized mesh index count is exceeding maximum (2^32-1): %u.", srcMesh.indexes.size());
            return false;
        }

        if (srcMesh.hasTextureCoordW)
        {
            // logError("Engine does not support three component texture coordinates.");
            return false;
        }
    }

    return true;
}

// Based on: 
// https://web.archive.org/web/20110708081637/http://www.terathon.com/code/tangent.html
// https://gamedev.stackexchange.com/questions/68612/how-to-compute-tangent-and-bitangent-vectors
//
bool generateTangentsAndBiTangents(obj::Model& srcModel, const uint32 meshIndex)
{
    if (meshIndex >= srcModel.meshes.size())
    {
        return false;
    }

    obj::Mesh& srcMesh = srcModel.meshes[meshIndex];

    // Clear arrays
    for(uint32 i=0; i<srcMesh.vertices.size(); ++i)
    {
        srcMesh.tangents[i]   = float3(0.0f, 0.0f, 0.0f);
        srcMesh.bitangents[i] = float3(0.0f, 0.0f, 0.0f);
    }

    // Generate tangent and bitangent vectors for every triangle
    for(uint32 j=0; j<srcMesh.indexes.size(); j += 3)
    {
        const obj::Vertex& v0ids = srcMesh.vertices[srcMesh.indexes[j]];
        const obj::Vertex& v1ids = srcMesh.vertices[srcMesh.indexes[j + 1]];
        const obj::Vertex& v2ids = srcMesh.vertices[srcMesh.indexes[j + 2]];

        const float3& v0 = srcModel.vertices[v0ids.position];
        const float3& v1 = srcModel.vertices[v1ids.position];
        const float3& v2 = srcModel.vertices[v2ids.position];

        const float3& c0 = srcModel.coordinates[v0ids.uv];
        const float3& c1 = srcModel.coordinates[v1ids.uv];
        const float3& c2 = srcModel.coordinates[v2ids.uv];

        float3 vec0 = v1 - v0;
        float3 vec1 = v2 - v0;

        float3 coord0 = c1 - c0;
        float3 coord1 = c2 - c0;

        float r = 1.0f / (coord0.u * coord1.v - coord1.u * coord0.v);

        float3 tangent   = (coord1.v * vec0 - coord0.v * vec1) * r;
        float3 bitangent = (coord0.u * vec1 - coord1.u * vec0) * r;

        // Average tangents of triangles that share vertex
        srcMesh.tangents[srcMesh.indexes[j]]     += tangent;
        srcMesh.tangents[srcMesh.indexes[j + 1]] += tangent;
        srcMesh.tangents[srcMesh.indexes[j + 2]] += tangent;

        // Average bitangents of triangles that share vertex
        srcMesh.bitangents[srcMesh.indexes[j]]     += bitangent;
        srcMesh.bitangents[srcMesh.indexes[j + 1]] += bitangent;
        srcMesh.bitangents[srcMesh.indexes[j + 2]] += bitangent;
    }

    // Normalize and orthogonalize tangent space vectors
    for(uint32 j=0; j<srcMesh.vertices.size(); ++j)
    {
        const obj::Vertex& v0ids = srcMesh.vertices[j];

        // Orthogonalize with preserving normal direction
        float3 t = normalize(srcMesh.tangents[j]);            // T - normalized tangent
        float3 b = normalize(srcMesh.bitangents[j]);          // B - normalized bitangent
        float3 n = normalize(srcModel.normals[v0ids.normal]); // N - normalized normal

        // Gram-Schmidt orthogonalize
        float3 tangent   = normalize(t - n * dot(n, t));   // Tangent in normal plane
        float3 bitangent = normalize(b - n * dot(n, b));   // Bitangent in normal plane
        float3 between   = normalize(tangent + bitangent); // Vector between T and B
        float3 side      = normalize(cross(between, n));   // Side vector
        float3 newt      = normalize(between + side);      // Corrected tangent vector
        float3 newb      = normalize(between - side);      // Corrected bitangent vector

        // Calculate handedness
        // TODO: Compare with: tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
        // TODO: Determine handedness used in this code (why I didn't wrote that down)

        // Check if corrected tangent and bitangent shouldn't be swapped
        if (dot(cross(t, b), cross(newt, newb)) < 0.0f)
        {
            srcMesh.tangents[j]   = newb;
            srcMesh.bitangents[j] = newt;
        }
        else
        {
            srcMesh.tangents[j]   = newt;
            srcMesh.bitangents[j] = newb;
        }
    }

    return true;
}


/* Common constants and functions and headers
#include <cmath>
#include <algorithm>

#include <cmath>
#include <stdint.h>
//*/

// Returns a number in range [-1.0f, 1.0f] 
float snormToFloat(sint16 bits)
{
    return clamp(float(bits) * (1.0f / 32767.0f), -1.0f, 1.0f);
}

// this function will be used to find the top left corner of the corresponding square to allow for discrete searching
sint16 flooredSnorms(float f)
{
    return (sint16)(floor(clamp(f, -1.0f, 1.0f) * 32767.0f));
}

inline float signNotZero(float v)
{
    return (v < 0.0f) ? -1.0f : 1.0f;
}

// CPU side encoder of oct32P method from: "Survey of Efficient Representations for Independent Unit Vectors"
// http://jcgt.org/published/0003/02/01/

void decode_oct32(const sint16v2 projected, float3& vec)
{
    vec.x = snormToFloat(projected.x);
    vec.y = snormToFloat(projected.y);
    vec.z = 1.0f - (fabs(vec.x) + fabs(vec.y));
    if (vec.z < 0.0f)
    {
        float oldX = vec.x;
        vec.x = ((1.0f) - fabs(vec.y)) * signNotZero(oldX);
        vec.y = ((1.0f) - fabs(oldX)) * signNotZero(vec.y);
    }
}

sint16v2 encode_oct32P(const float3 vector)
{
    sint16v2 projected;

    const float invL1Norm = (1.0f) / (fabs(vector.x) + fabs(vector.y) + fabs(vector.z));

    if (vector.z <= 0.0f)
    {
        projected.x = flooredSnorms((1.0f - fabs(vector.y * invL1Norm)) * signNotZero(vector.x));
        projected.y = flooredSnorms((1.0f - fabs(vector.x * invL1Norm)) * signNotZero(vector.y));
    }
    else
    {
        projected.x = flooredSnorms(vector.x * invL1Norm);
        projected.y = flooredSnorms(vector.y * invL1Norm);
    }

    sint16v2 bestProjected;
    sint16 uBits = projected.x;
    sint16 vBits = projected.y;

    float error = 0;
    for (sint16 i = 0; i < 2; ++i)
    {
        for (sint16 j = 0; j < 2; ++j)
        {
            projected.x = uBits + i;
            projected.y = vBits + j;
            float3 decoded;
            decode_oct32(projected, decoded);
            const float altError = fabs((vector.x * decoded.x) + (vector.y * decoded.y) + (vector.z * decoded.z));
            if (altError > error)
            {
                error = altError;
                bestProjected.x = projected.x;
                bestProjected.y = projected.y;
            }
        }
    }

    projected = bestProjected;
    return projected;
}







// Creates scene model
// - support for multile MTL files
// - automatic polygon tesselation using triangle fan
// - reduces identical vertices inside mesh
// - reduces index buffer size
// - optimizes indices order
std::shared_ptr<en::resources::Model> load(
    const std::string& filename,
    const std::string& name,
          en::Streamer& streamer,
    const uint8 gpuIndex)
{
    using namespace en::storage;

    assert(!name.empty());
    assert(gpuIndex < 8);

    // Try to reuse already loaded models
    if (ResourcesContext.models.find(name) != ResourcesContext.models.end())
    {
        return ResourcesContext.models[name];
    }

    // Open model file
    File* file = Storage->open(filename);
    if (!file)
    {
        file = Storage->open(en::ResourcesContext.path.models + filename);
        if (!file)
        {
            enLog << en::ResourcesContext.path.models + filename << std::endl;
            enLog << "ERROR: There is no such file!\n";
            return std::shared_ptr<en::resources::Model>(nullptr);
        }
    }
   
    // Allocate temporary buffer for file content ( 4GB max size! )
    uint64 size = file->size();
    if (size > 0xFFFFFFFF)
    {
        enLog << "ERROR: Maximum supported size of imported OBJ file is 4GB!\n";
        return std::shared_ptr<en::resources::Model>(nullptr);
    }

    uint8* buffer = nullptr;
    buffer = new uint8[static_cast<uint32>(size)];
    if (!buffer)
    {
        enLog << "ERROR: Not enough memory!\n";
        delete file;
        return std::shared_ptr<en::resources::Model>(nullptr);
    }
   
    // Read file to buffer and close file
    if (!file->read(buffer))
    {
        enLog << "ERROR: Cannot read whole obj file!\n";
        delete file;
        return std::shared_ptr<en::resources::Model>(nullptr);
    }    
    delete file;


    // Step 1 - Parsing the file


    obj::Model* srcModel = parseOBJ(buffer, size);
    if (!srcModel)
    {
        return nullptr;
    }

    if (!verifyModel(*srcModel))
    {
        delete srcModel;
        return nullptr;
    }
 

    // Step 2 - Loading materials


    // Load materials used by model
    for(uint8 i=0; i< srcModel->materials.size(); ++i)
    {
        bool loaded = false;

        // Search for material in used MTL files and load it
        for(uint8 j=0; j< srcModel->libraries.size(); ++j)
        {
            loaded = mtl::load(srcModel->libraries[j], srcModel->materials[i].name, srcModel->materials[i]);
            if (loaded)
            {
                break;
            }
        }

        // Check if material was found and properly loaded
        if (!loaded)
        {
            enLog << "ERROR! Cannot find material: " << srcModel->materials[i].name.c_str() << " !\n";
        }
    }  


    // Step 3 - Performing optimizations on vertex and index buffers


    // Model may store multiple meshes using the same material, as a way of 
    // separating smoothing groups (and maintaining hard edges between them).
    // Due to that meshes with the same material are not coalesced together.

    // Generate Tangents and BiTangents
    for(uint32 i=0; i<srcModel->meshes.size(); ++i)
    {
        obj::Mesh& srcMesh = srcModel->meshes[i];

        // Search source mesh and material
        en::resources::Material srcMaterial;
        for(uint8 j=0; j<srcModel->materials.size(); ++j)
        {
            if (srcMesh.material == srcModel->materials[j].name)
            {
                srcMaterial = srcModel->materials[j];
            }
        }



        //if (srcMaterial.normal.map ||
        //    srcMaterial.displacement.map)
        if (srcMaterial.normal ||
            srcMaterial.displacement) // srcMesh.hasTextureCoords
        {
            // Calculate tangent space vectors
            // (based on: https://gamedev.stackexchange.com/questions/68612/how-to-compute-tangent-and-bitangent-vectors)
            generateTangentsAndBiTangents(*srcModel, i);
        }
    }

    // Optimize indexes order for Post-Transform Vertex Cache Size
    if (obj::optimizeIndexOrder)
    {
        for(uint32 i=0; i<srcModel->meshes.size(); ++i)
        {
            obj::Mesh& srcMesh = srcModel->meshes[i];

            void* srcIndex = srcMesh.indexes.data();

/*
            srcMesh.optimized.resize(srcMesh.indexes.size(), 0);
            Forsyth::optimize(srcMesh.indexes, srcMesh.optimized, vertexes);
//*/
        }
    }


    // Step 4 - Generating final model


    // TODO: Allocate model from global models array (scene independent or scene dependent?)
    //// Create model
    //en::resources::ModelDescriptor* model = ResourcesContext.storage.models.allocate();
    //if (model == NULL)
    //{
    //    enLog << "ERROR: Models pool is full!\n";
    //    return std::shared_ptr<en::resources::Model>(NULL);
    //} 
    //
    // TODO: Search by hash if model already exists, if so, load only if it is for GPU its not loaded for
    std::shared_ptr<resources::Model> model = std::make_shared<resources::Model>(name.c_str(), srcModel->meshes.size(), 1, gpuIndex);
    if (!model)
    {
        assert(0);

        delete srcModel;
        return nullptr;
    }

    // TODO: Generalize obj::Mesh to be used by all model loader formats (like FBX, PLY, etc.) so that this part can be shared by all in Resources Manager (engine renderer specific).
    // TODO: Query alignment requirements for each input buffer from GPU Streamer
    //       Calculate final size of each input buffer of each mesh (and their padded starting addresses)
    //       Sum all above to get final size of single memory allocation for all LODs and meshes in model
    //       Verify it will work great on all backing arch (Vertex + Index  buffer both)
    // TODO: Invoke placement new Mesh constructor
    uint64 allocationSize = 0;
    for(uint32 i=0; i<srcModel->meshes.size(); ++i)
    {
        obj::Mesh&      srcMesh = srcModel->meshes[i];
        resources::Mesh& dstMesh = model->mesh[i];

        const uint32 vertexCount = static_cast<uint32>(srcMesh.vertices.size());
        const uint32 indexCount  = static_cast<uint32>(srcMesh.indexes.size());

        dstMesh.init(gpu::DrawableType::Triangles, 0); // OBJ meshes are always triangulated (no support for points nad lines yet)
        dstMesh.vertexCount = vertexCount;
        dstMesh.indexCount  = indexCount;
        dstMesh.bufferMask  = 0u;
        dstMesh.hasPosition = true; // Position buffer is mandatory for OBJ
        dstMesh.hasNormal   = srcMesh.hasNormals;
        dstMesh.indexShift  = 1;
        if (dstMesh.vertexCount >= 0xFFFF)
        {
            dstMesh.indexShift = 2;
        }
        dstMesh.hasUV       = srcMesh.hasTextureCoords;
        // If it has both texture coords, it will have Tangent (autogenerated)
        dstMesh.hasTangent  = srcMesh.hasTextureCoords && srcMesh.hasTextureCoordV;

        //dstMesh.materialIndex = // TODO: Index of one of 262144 materials in global materials array.

        for(uint32 i=0; i<MaxMeshInputBuffers; ++i)
        {
            dstMesh.offset[i] = 0;
        }

        // Calculates input and index buffers offsets in backing allocation
        dstMesh.offset[0] = allocationSize;

        // First input buffer size
        uint32 elementSize = gpu::Formatting(gpu::Attribute::v3f32, gpu::Attribute::v2f16).elementSize();
        uint32 bufferSize = vertexCount * elementSize;
        allocationSize += bufferSize;

        if (dstMesh.hasNormal)
        {
            dstMesh.offset[1] = allocationSize;

            // Second input buffer size
            gpu::Formatting format(gpu::Attribute::v2s16); // Normal
            if (dstMesh.hasTangent)
            {
                format.column[1] = gpu::Attribute::v2s16;  // Tangent
            }
            elementSize = format.elementSize();
            bufferSize  = vertexCount * elementSize;
            allocationSize += bufferSize;
        }

        // OBJ models have no animation

        dstMesh.indexOffset = allocationSize;

        // Index buffer size
        bufferSize = indexCount << dstMesh.indexShift;
        allocationSize += bufferSize;
    }

    // There is single memory allocation backing all meshes of given model (of given LOD or all LODs?)

    // Allocate resource
    if (!streamer.allocateMemory(model->backing[gpuIndex], allocationSize))
    {
        // logError("Not enough memory to back loaded model.");
        delete srcModel;
        model = nullptr; // Delete
        return nullptr;
    }

    // Populate resource with data
    volatile void* backing = model->backing[gpuIndex]->cpuPointer;
    for(uint32 i=0; i<srcModel->meshes.size(); ++i)
    {
        obj::Mesh&      srcMesh = srcModel->meshes[i];
        resources::Mesh& dstMesh = model->mesh[i];

        // Compose geometry buffer
        uint8* buffer = (uint8*)(backing) + dstMesh.offset[0];
        uint32 offset = 0;
        for(uint32 j=0; j<srcMesh.vertices.size(); ++j)
        {
            obj::Vertex& vertex = srcMesh.vertices[j];
            uint32 vertexId   = vertex.position - 1;
            
            // Position
            *((float3*)(buffer + offset)) = srcModel->vertices[vertexId];
            offset += sizeof(float3);

            // UV (optional)
            if (srcMesh.hasTextureCoords)
            {
                uint32 texCoordId = vertex.uv - 1;

                half compressedU(srcModel->coordinates[texCoordId].u);

                *((half*)(buffer + offset)) = compressedU;
                offset += sizeof(half);

                if (srcMesh.hasTextureCoordV)
                {
                    half compressedV(srcModel->coordinates[texCoordId].v);

                    *((half*)(buffer + offset)) = compressedV;
                }

                offset += sizeof(half);
            }
            else
            {
                // If there are no texture coordinates, single entry size is still padded to 16 bytes.
                offset += 2 * sizeof(half);
            }
        }

        // Compose Normals buffer
        if (srcMesh.hasNormals)
        {
            buffer = (uint8*)(backing) + dstMesh.offset[1];
            offset = 0;
            for(uint32 j=0; j<srcMesh.vertices.size(); ++j)
            {
                obj::Vertex& vertex = srcMesh.vertices[j];
                uint32 normalId = vertex.normal - 1;
            
                float3 normal = srcModel->normals[normalId];
                sint16v2 compressedNormal = encode_oct32P(normal);

                *((sint16v2*)(buffer + offset)) = compressedNormal;
                offset += sizeof(sint16v2);

                if (dstMesh.hasTangent)
                {
                    float3 tangent = srcMesh.tangents[j];
                    sint16v2 compressedTangent = encode_oct32P(tangent);

                    *((sint16v2*)(buffer + offset)) = compressedTangent;
                    offset += sizeof(sint16v2);

                    // TODO: What about biTangent sign? Do we need to store it? 
                    // See: https://discussions.unity.com/t/question-about-binormal-calculation/715758
                    // "Vector3 binormal = Vector3.Cross(normal, tangent) * binormalSign;"
                    // "The w component is used to handle mirrored UVs, so it might be -1 or 1 depending on the UVs. Performance wise, since the value needs to exist, it doesn’t matter if it defaults to -1 or 1. Correctness wise, the bitangent / binormal is defined as the cross product of the normal and tangent, so it makes sense to keep the calculations correctly in the space & handedness it’s actually in, and use -1 to be explicit about the orientation flip of the spaces."
                    // We don't need to store sign if we're not supporting mirrored UVs.
                    // biTangent sign usually stored in Tangent/Normal W component defines Tangent Space handedness.
                    // See also: http://jerome.jouvie.free.fr/opengl-tutorials/Lesson8.php
                }
            }
        }

        // OBJ does not store animation information

        // Compose Index buffer
        buffer = (uint8*)(backing) + dstMesh.indexOffset;
        offset = 0;
        for(uint32 j=0; j<srcMesh.indexes.size(); ++j)
        {
            uint32 index = 0;
            if (!srcMesh.optimized.empty())
            {
                index = srcMesh.optimized[j];
            }
            else
            {
                index = srcMesh.indexes[j];
            }

            // Store as u16 or u32
            if (dstMesh.indexShift == 1)
            {
                // 0xFFFF is reserved for primitive restart (even though its discouraged to use it)
                assert(index < 0xFFFF);

                *((uint16*)(buffer + offset)) = (uint16)index;
                offset += sizeof(uint16);
            }
            else
            {
                *((uint32*)(buffer + offset)) = index;
                offset += sizeof(uint32);
            }
        }
    }

    // Upload to VRAM
    bool result = streamer.makeResident(*model->backing[gpuIndex], true);
    assert(result);
    
    // Update list of loaded models
    ResourcesContext.models.insert(std::pair<std::string, std::shared_ptr<resources::Model> >(name, model));
 
    // Return model instance
    return model;
}

} // en::obj
} // en
