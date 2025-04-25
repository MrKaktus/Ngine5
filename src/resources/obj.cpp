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
struct Vertex
{
    // Index 0 means that given component is not used by vertex.

    uint32 position; // Position
    uint32 uv;       // Texture Coordinate
    uint32 normal;   // Normal vector
    
    bool operator ==(const Vertex& b);
};

// Internal, custom OBJ face representation.
struct Face
{
    Vertex vertex[3]; // Vertex
    uint32 material;  // Material
};

//struct Material
//{
//    std::string                 name;   // Material name
//    en::resource::Material handle; // Material handle
//};

// Internal, custom OBJ mesh representation.
// Stores unoptimized contents after parsing.
struct Mesh
{
    std::string         name;             // Mesh name
    std::string         material;         // Material name (only one per group allowed)
    std::vector<Vertex> vertices;         // Array of vertices composition data
    std::vector<uint32> indexes;          // Array of indexes
    std::vector<uint32> optimized;        // Array of indexes after Forsyth optimization
    bool                hasTextureCoords; // Does mesh contain texture coordinates 
    bool                hasTextureCoordV; // Does mesh uses 2 component texture coordinates
    bool                hasTextureCoordW; // Does mesh uses 3 component texture coordinates
    bool                hasNormals;       // Does mesh contain normal vectors
};

// Internal, custom OBJ model representation.
// Stores unoptimized contents after parsing.
struct Model
{
    std::vector<float3> vertices;                  // Vertices (uncompressed)
    std::vector<float3> normals;                   // Normals (uncompressed)
    std::vector<float3> coordinates;               // Texture coordinates (uncompressed)
    std::vector<std::string> libraries;            // Material libraries
    std::vector<en::obj::Mesh> meshes;             // Meshes
    std::vector<en::resource::Material> materials; // Materials <- TODO: This should be OBJ internal temp material?

    Model();

    obj::Mesh* addMesh(void);
    obj::Mesh* currentMesh(void);
};

bool Vertex::operator ==(const Vertex& b)
{
    return !memcmp(this, &b, 12); // 12 = sizeof(obj::Vertex)
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

    if (vector[1] != NAN)
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


obj::Model* parseOBJ(const uint8* buffer, const uint32 size)
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
                    en::resource::Material enMaterial;
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

    return result;
}

// Creates scene model
// - support for multile MTL files
// - automatic polygon tesselation using triangle fan
// - reduces identical vertices inside mesh
// - reduces index buffer size
// - optimizes indices order
std::shared_ptr<en::resource::Model> load(const std::string& filename, const std::string& name)
{
    using namespace en::storage;

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
            Log << en::ResourcesContext.path.models + filename << std::endl;
            Log << "ERROR: There is no such file!\n";
            return std::shared_ptr<en::resource::Model>(nullptr);
        }
    }
   
    // Allocate temporary buffer for file content ( 4GB max size! )
    uint64 size = file->size();
    uint8* buffer = nullptr;
    buffer = new uint8[static_cast<uint32>(size)];
    if (!buffer)
    {
        Log << "ERROR: Not enough memory!\n";
        delete file;
        return std::shared_ptr<en::resource::Model>(nullptr);
    }
   
    // Read file to buffer and close file
    if (!file->read(buffer))
    {
        Log << "ERROR: Cannot read whole obj file!\n";
        delete file;
        return std::shared_ptr<en::resource::Model>(nullptr);
    }    
    delete file;


    // Step 1 - Parsing the file


    obj::Model* objModel = parseOBJ(buffer, size);
    if (!objModel)
    {
        return nullptr;
    }
 
    // Step 2 - Generating final model

    // Load materials used by model
    for(uint8 i=0; i< objModel->materials.size(); ++i)
    {
        bool loaded = false;

        // Search for material in used MTL files and load it
        for(uint8 j=0; j< objModel->libraries.size(); ++j)
        {
            loaded = mtl::load(objModel->libraries[j], objModel->materials[i].name, objModel->materials[i]);
            if (loaded)
            {
                break;
            }
        }

        // Check if material was found and properly loaded
        if (!loaded)
        {
            Log << "ERROR! Cannot find material: " << objModel->materials[i].name.c_str() << " !\n";
        }
    }  

    //// Create model
    //en::resource::ModelDescriptor* model = ResourcesContext.storage.models.allocate();
    //if (model == NULL)
    //{
    //    Log << "ERROR: Models pool is full!\n";
    //    return std::shared_ptr<en::resource::Model>(NULL);
    //} 
    //model->mesh   = new en::resource::Mesh*[meshes.size()];
    //model->meshes = meshes.size();
    
    std::shared_ptr<en::resource::Model> model = std::make_shared<en::resource::Model>();
    if (!model)
    {
        assert(0);

        delete objModel;
        return nullptr;
    }

    // TODO: Coalesce meshes with the same material.

    // Count unique meshes

    // Generate meshes
    for(uint8 i=0; i<objModel->meshes.size(); ++i)
    {
        using namespace en::resource;

        // Search source mesh and material
        obj::Mesh& srcMesh = objModel->meshes[i];
        en::resource::Material srcMaterial;

        for(uint8 j=0; j<objModel->materials.size(); ++j)
        {
            if (srcMesh.material == objModel->materials[j].name)
            {
                srcMaterial = objModel->materials[j];
            }
        }

        assert( srcMesh.vertices.size() < 0xFFFFFFFF );
        assert( srcMesh.indexes.size() < 0xFFFFFFFF );
        uint32 vertexes = static_cast<uint32>(srcMesh.vertices.size());
        uint32 indexes  = static_cast<uint32>(srcMesh.indexes.size());
      
        // Create geometry buffer
        gpu::Formatting formatting;
        uint32 columns  = 1;

        // Position
        formatting.column[0] = gpu::Attribute::v3f32; // inPosition

        uint32 rowSize = 12;
   
        // Normals
        if (srcMesh.hasNormals)
        {
            rowSize += 12;
            formatting.column[columns] = gpu::Attribute::v3f32; // inNormal
            columns++;
        }

        // Bitangents
        //if (srcMaterial.normal.map ||
        //    srcMaterial.displacement.map)
        if (srcMaterial.normal ||
            srcMaterial.displacement)
        {
            rowSize += 12;
            formatting.column[columns] = gpu::Attribute::v3f32; // inBitangent
            columns++;
        }
           
        // Texture Coordinates
        if (srcMesh.hasTextureCoords)
        {
            rowSize += 8;
            formatting.column[columns] = gpu::Attribute::v2f32; // inTexCoord0
            if (srcMesh.hasTextureCoordW)
            {
                rowSize += 4;
                formatting.column[columns] = gpu::Attribute::v3f32;
            }
            columns++;
        }
   
        // Calculate tangent space vectors
        float3* tangents   = nullptr;
        float3* bitangents = nullptr;   
        //if (srcMaterial.normal.map ||
        //    srcMaterial.displacement.map)
        if (srcMaterial.normal ||
            srcMaterial.displacement)
        {
            // Tangents and bitangents arrays
            tangents   = new float3[srcMesh.vertices.size()];
            bitangents = new float3[srcMesh.vertices.size()];

            // Clear arrays
            memset(tangents, 0, sizeof(float3) * srcMesh.vertices.size());
            memset(bitangents, 0, sizeof(float3) * srcMesh.vertices.size());
 
            // Generate tangent and bitangent vectors for every triangle
            for(uint32 j=0; j<srcMesh.indexes.size(); j+=3)
            {
                const en::obj::Vertex& v0ids = srcMesh.vertices[ srcMesh.indexes[j]   ];
                const en::obj::Vertex& v1ids = srcMesh.vertices[ srcMesh.indexes[j+1] ];
                const en::obj::Vertex& v2ids = srcMesh.vertices[ srcMesh.indexes[j+2] ];

                const float3& v0 = objModel->vertices[ v0ids.position ];
                const float3& v1 = objModel->vertices[ v1ids.position ];
                const float3& v2 = objModel->vertices[ v2ids.position ];

                const float3& c0 = objModel->coordinates[ v0ids.uv ];
                const float3& c1 = objModel->coordinates[ v1ids.uv ];
                const float3& c2 = objModel->coordinates[ v2ids.uv ];

                float3 vec0 = v1 - v0;
                float3 vec1 = v2 - v0;

                float3 coord0 = c1 - c0;
                float3 coord1 = c2 - c0;

                float r = 1.0f / (coord0.u * coord1.v - coord1.u * coord0.v);

                float3 tangent   = (coord1.v * vec0 - coord0.v * vec1) * r;
                float3 bitangent = (coord0.u * vec1 - coord1.u * vec0) * r;
              
                // Average tangents of triangles that share vertex
                tangents[ srcMesh.indexes[j]   ] += tangent;
                tangents[ srcMesh.indexes[j+1] ] += tangent;
                tangents[ srcMesh.indexes[j+2] ] += tangent;
            
                // Average bitangents of triangles that share vertex
                bitangents[ srcMesh.indexes[j]   ] += bitangent;
                bitangents[ srcMesh.indexes[j+1] ] += bitangent;
                bitangents[ srcMesh.indexes[j+2] ] += bitangent;
            }

            // Normalize and orthogonalize tangent space vectors
            for(uint32 j=0; j<srcMesh.vertices.size(); ++j)
            {
                const en::obj::Vertex& v0ids = srcMesh.vertices[j];
 
                // Orthogonalize with preserving normal direction
                float3 t = normalize( tangents[j] );                       // T - normalized tangent
                float3 b = normalize( bitangents[j] );                     // B - normalized bitangent
                float3 n = normalize( objModel->normals[ v0ids.normal ] ); // N - normalized normal

                float3 tangent   = normalize(t - n * dot(n, t));   // Tangent in normal plane
                float3 bitangent = normalize(b - n * dot(n, b));   // Bitangent in normal plane
                float3 between   = normalize(tangent + bitangent); // Vector between T and B
                float3 side      = normalize(cross(between, n));   // Side vector
                float3 newt      = normalize(between + side);      // Corrected tangent vector
                float3 newb      = normalize(between - side);      // Corrected bitangent vector

                // Check if corrected tangent and bitangent shouldn't be swapped
                if (dot(cross(t,b),cross(newt,newb)) < 0.0f)
                {
                    tangents[j]   = newb;
                    bitangents[j] = newt;
                }
                else
                {
                    tangents[j]   = newt;
                    bitangents[j] = newb;
                }
            }
        }

        // Compose geometry buffer
        uint8* geometry = new uint8[vertexes * rowSize];
        uint32 offset = 0;
        for(uint32 j=0; j<vertexes; ++j)
        {
            en::obj::Vertex& vertex = srcMesh.vertices[j];

            // Position
            *((float3*)(geometry + offset))    = objModel->vertices[vertex.position - 1];
            offset += 12;

            // Normals
            if (srcMesh.hasNormals)
            {
                *((float3*)(geometry + offset)) = objModel->normals[vertex.normal - 1];
                offset += 12;
            }

            // Bitangents
            //if (srcMaterial.normal.map ||
            //    srcMaterial.displacement.map)
            if (srcMaterial.normal ||
                srcMaterial.displacement)
            {
                *((float3*)(geometry + offset)) = bitangents[j];
                offset += 12;
            }

            // Texture Coordinates
            if (srcMesh.hasTextureCoords)
            {
                *((float*)(geometry + offset))  = objModel->coordinates[vertex.uv - 1].u;
                offset += 4;
                *((float*)(geometry + offset))  = objModel->coordinates[vertex.uv - 1].v;
                offset += 4;
                if (srcMesh.hasTextureCoordW)
                {
                    *((float*)(geometry + offset)) = objModel->coordinates[vertex.uv - 1].w;
                    offset += 4;
                }
            }
        }
        std::unique_ptr<gpu::Buffer> vertexBuffer(en::ResourcesContext.defaults.enHeapBuffers->createBuffer(vertexes, formatting, 0u));

        // Create staging buffer
        uint32 stagingSize = vertexes * rowSize;
        std::unique_ptr<gpu::Buffer> staging(en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, stagingSize));
        if (!staging)
        {
            Log << "ERROR: Cannot create staging buffer!\n";
            return std::shared_ptr<en::resource::Model>(nullptr);
        }

        // Read texture to temporary buffer
        volatile void* dst = staging->map();
        memcpy((void*)dst, geometry, stagingSize);
        staging->unmap();
       
        // TODO: In future distribute transfers to different queues in the same queue type family
        gpu::QueueType queueType = gpu::QueueType::Universal;
        if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
        {
            queueType = gpu::QueueType::Transfer;
        }

        // Copy data from staging buffer to final texture
        std::shared_ptr<gpu::CommandBuffer> command = Graphics->primaryDevice()->createCommandBuffer(queueType);
        command->start();
        command->copy(*staging, *vertexBuffer);
        command->commit();
      
        // TODO:
        // here return completion handler callback !!! (no waiting for completion)
        // - this callback destroys CommandBuffer object
        // - destroys staging buffer
        //
        // Till it's done, wait for completion:
      
        command->waitUntilCompleted();
        command = nullptr;
        staging = nullptr;

        delete [] geometry;
        delete [] tangents;
        delete [] bitangents;

        // Create indices buffer
        stagingSize = indexes * 4;
        formatting.column[0] = gpu::Attribute::u32;
        for(uint8 j=1; j<16; ++j)
        {
            formatting.column[j] = gpu::Attribute::None;
        }

        // Optimize indexes order for Post-Transform Vertex Cache Size
        void* srcIndex = &srcMesh.indexes[0];
        if (en::obj::optimizeIndexOrder)
        {
            srcMesh.optimized.resize(indexes, 0);
            //Forsyth::optimize(srcMesh.indexes, srcMesh.optimized, vertexes);
            srcIndex = &srcMesh.optimized[0];
        }
   
        // Optimize size of Index Buffer
        bool compressed = false;
        if (en::obj::optimizeIndexSize)
        {
            if (vertexes < 255)
            {
                stagingSize = indexes;
                formatting.column[0] = gpu::Attribute::u8;
                uint8* ibo = new uint8[indexes];
                for(uint32 j=0; j<indexes; j++)
                {
                    uint32 index = *((uint32*)(srcIndex) + j);
                    assert(index < 256);
                    ibo[j] = (uint8)index;
                }
                compressed = true;
                srcIndex = ibo;
            }
            else
            if (vertexes < 65535)
            {
                stagingSize = indexes * 2;
                formatting.column[0] = gpu::Attribute::u16;
                uint16* ibo = new uint16[indexes];
                for(uint32 j=0; j<indexes; j++)
                {
                    uint32 index = *((uint32*)(srcIndex) + j);
                    assert(index < 65536);
                    ibo[j] = (uint16)index;
                }
                compressed = true;
                srcIndex = ibo;
            }
        }
        std::unique_ptr<gpu::Buffer> indexBuffer(en::ResourcesContext.defaults.enHeapBuffers->createBuffer(indexes, formatting, 0u));
      
        // Create staging buffer
        staging.swap(std::unique_ptr<gpu::Buffer>(en::ResourcesContext.defaults.enStagingHeap->createBuffer(gpu::BufferType::Transfer, stagingSize)));
        if (!staging)
        {
            Log << "ERROR: Cannot create staging buffer!\n";
            return std::shared_ptr<en::resource::Model>(nullptr);
        }

        // Read buffer to temporary buffer
        dst = staging->map();
        memcpy((void*)dst, srcIndex, stagingSize);
        staging->unmap();

        // Copy data from staging buffer to final texture
        command = Graphics->primaryDevice()->createCommandBuffer(queueType);
        command->start();
        command->copy(*staging, *vertexBuffer);
        command->commit();
      
        // TODO:
        // here return completion handler callback !!! (no waiting for completion)
        // - this callback destroys CommandBuffer object
        // - destroys staging buffer
        //
        // Till it's done, wait for completion:
      
        command->waitUntilCompleted();
        command = nullptr;
        staging = nullptr;
      
        if (compressed)
        {
            delete [] static_cast<uint8*>(srcIndex);
        }

        // Create mesh
        en::resource::Mesh mesh;

        // TODO: Refactor to new Model description
        //mesh.name              = srcMesh.name;
        //mesh.material          = srcMaterial;
        //mesh.geometry.buffer   = vertexBuffer;
        //mesh.geometry.begin    = 0;
        //mesh.geometry.end      = vertexes;
        //mesh.elements.type     = gpu::Triangles;
        //mesh.elements.buffer   = indexBuffer;
        //mesh.elements.offset   = 0;
        //mesh.elements.indexes  = indexes;

        //model->mesh.push_back(mesh);
    }
    
    // Update list of loaded models
    ResourcesContext.models.insert(std::pair<std::string, std::shared_ptr<en::resource::Model> >(name, model));
 
    // Return model interface
    return model;
}

} // en::obj
} // en
