/*

 Ngine v5.0
 
 Module      : FBX files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading models from *.fbx files.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/utilities/parser.h"
#include "core/rendering/device.h"
#include "utilities/strings.h"
#include "utilities/gpcpu/gpcpu.h"
#include "resources/interface.h" 
#include "resources/forsyth.h" 
#include "resources/fbx.h"     

#pragma push_macro("aligned")
#undef aligned

#define FBXSDK_SHARED
#define FBXSDK_NEW_API
#include "fbxsdk.h"

#pragma pop_macro("aligned")

namespace en
{
namespace raw
{

struct Buffer
{
    gpu::Formatting     formatting;
    gpu::BufferType     type;
    uint32              elements;

    //gpu::BufferSettings settings;     // Columns types and names
    uint32              columns;      // Columns in buffer
    uint32              rowSize;      // Buffer row size in bytes
    void*               data;         // Pointer to data buffer
};

struct Material
{ 
    std::string name;              // Material name

    struct Emmisive                   
    {                          
        std::string map;           // Ke emmisive coefficients for each point of surface
        float3 color;              // Ke default emissive coefficient 
    } emmisive;

    struct Ambient
    {
        std::string map;           // Ka incoming irradiance coefficients for each point of surface in Phong/Lambert models
        float3 color;              // Ka default incoming irradiance coefficient in Phong/Lambert models
    } ambient;                 

    struct Diffuse                    
    {                          
        std::string map;           // Kd leaving irradiance coefficients for each point of surface in Phong/Lambert models
        float3 color;              // Kd base color of material, it's albedo, or diffuse term in Phong/Lambert models
    } diffuse;                 

    struct Specular                   
    {                          
        std::string map;           // Ks reflection coefficients for each point of surface in Phong model
        float3 color;              // Ks default reflection coefficient in Phong model
        float  exponent;           // Specular exponent "shininess" factor for specular equation
    } specular;

    struct Transparency
    {
        std::string map;           // Transparency coefficients for each point of surface
        float3 color;              // Transparency default coefficient
    } transparency;            

    struct Normal                     
    {                          
        std::string map;           // Normal vector for each point of surface
    } normal;                  

    struct Displacement               
    {                          
        std::string map;           // Displacement value for each point of surface
        std::string vectors;       // Vector Displacement map for each point of surface
    } displacement;
};

struct Mesh
{
    std::string name;              // Mesh name
    float4x4 matrix;               // Local transformation matrix
    Material material;             // Material
    
    struct Geometry
    {
        uint32 id;                 // Geometry buffer ID
        uint32 begin;              // First vertice in VBO describing this mesh
        uint32 end;                // Last vertice in VBO describing this mesh (for ranged drawing)
    } geometry; 
    
    struct Elements
    {
        gpu::DrawableType type;    // Type of geometry primitives
        uint32            id;      // Element buffer ID
        uint32            offset;  // Offset to starting index in buffer
        uint32            indexes; // Count of indexes describing geometry primitives
    } elements;   
};

struct Model
{
    std::string         name;
    std::vector<Buffer> geometry;
    std::vector<Buffer> elements;
    std::vector<Mesh>   mesh;
};

} // en::raw

namespace fbx
{
#if defined(EN_PLATFORM_OSX) || defined(EN_PLATFORM_WINDOWS)

/* Tab character ("\t") counter */
int numTabs = 0; 

/**
 * Print the required number of tabs.
 */
void PrintTabs() 
{
    for(int i=0; i<numTabs; i++)
    {
        printf("\t");
    }
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) 
{
    switch(type) 
    {
        case FbxNodeAttribute::eUnknown: 
        {
            return "unknown";
        }

        case FbxNodeAttribute::eNull: 
        {
            return "null";
        }

        case FbxNodeAttribute::eMarker: 
        {
            return "marker";
        }

        case FbxNodeAttribute::eSkeleton: 
        {
            return "skeleton";
        }

        case FbxNodeAttribute::eMesh: 
        {
            return "mesh";
        }

        case FbxNodeAttribute::eNurbs: 
        {
            return "nurbs";
        }

        case FbxNodeAttribute::ePatch: 
        {
            return "patch";
        }

        case FbxNodeAttribute::eCamera: 
        {
            return "camera";
        }

        case FbxNodeAttribute::eCameraStereo: 
        {   
            return "stereo";
        }

        case FbxNodeAttribute::eCameraSwitcher: 
        {
            return "camera switcher";
        }

        case FbxNodeAttribute::eLight: 
        {
            return "light";
        }

        case FbxNodeAttribute::eOpticalReference: 
        {
            return "optical reference";
        }

        case FbxNodeAttribute::eOpticalMarker: 
        {
            return "marker";
        }

        case FbxNodeAttribute::eNurbsCurve: 
        {
            return "nurbs curve";
        }

        case FbxNodeAttribute::eTrimNurbsSurface: 
        {
            return "trim nurbs surface";
        }

        case FbxNodeAttribute::eBoundary: 
        {
            return "boundary";
        }

        case FbxNodeAttribute::eNurbsSurface: 
        {
            return "nurbs surface";
        }

        case FbxNodeAttribute::eShape:
        {        
            return "shape";
        }

        case FbxNodeAttribute::eLODGroup: 
        {    
            return "lodgroup";
        }

        case FbxNodeAttribute::eSubDiv: 
        {      
            return "subdiv";
        }

        case FbxNodeAttribute::eCachedEffect: 
        {
            return "cached effect";
        }

        case FbxNodeAttribute::eLine: 
        {        
            return "line";
        }

        default: 
        {
            return "[Not Supported by Ngine]";
        }
    }
}

/**
 * Print an attribute.
 */
void PrintAttribute(FbxNodeAttribute* pAttribute) 
{
    if (!pAttribute)
    {
        return;
    }

    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs();

    // Note: to retrieve the chararcter array of a FbxString, use its Buffer() method.
    printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

/**
 * Print a node, its attributes, and all its children recursively.
 */
void PrintNode(FbxNode* pNode)
{
    PrintTabs();
    const char* nodeName = pNode->GetName();
    FbxDouble3 translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();

    // print the contents of the node.
    printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n", 
        nodeName, 
        translation[0], translation[1], translation[2],
        rotation[0], rotation[1], rotation[2],
        scaling[0], scaling[1], scaling[2]);

    numTabs++;

    // Print the node's attributes.
    for(int i = 0; i < pNode->GetNodeAttributeCount(); i++)
    {
        PrintAttribute(pNode->GetNodeAttributeByIndex(i));
    }

    // Recursively print the children nodes.
    for(int j = 0; j < pNode->GetChildCount(); j++)
    {
        PrintNode(pNode->GetChild(j));
    }

    numTabs--;
    PrintTabs();
    printf("</node>\n");
}








using namespace en::gpu;



struct Vertex
{
    sint32 position;    // Position Id
    sint32 normal;      // Normal vector Id
    sint32 tangent;     // Tangent vector Id
    sint32 bitangent;   // Bitangent vector Id
    sint32 texcoord[4]; // Texture Coordinate sets Ids

    bool operator ==(const Vertex& b);
};

struct UnpackedMesh
{
    std::string   name;       // Name
    en::resources::Material material;        // Material
    uint32   polygons;        // Polygons count
    std::vector<Vertex> vertices;  // Array of references describing geometry
    std::vector<uint32> indexes;   // Polygons references to vertices
    std::vector<uint32> optimized; // Optimized index array
    en::resources::BoundingBox AABB;        
};

bool Vertex::operator ==(const Vertex& b)
{
    return memcmp(this, &b, sizeof(en::fbx::Vertex)) == 0; 
}





void LoadMatrix(float4x4* destination, FbxAMatrix* fbxMatrix)
{
    // FbxAMatrix is an array of 16 double precision floating
    // point values representing matrix in column major order.
    const double* src = reinterpret_cast<const double*>(fbxMatrix);
    for(uint8 i=0; i<16; ++i)
    {
        destination->m[i] = static_cast<float>(src[i]);
    }
}

bool IsMaterialUsingShaders(FbxSurfaceMaterial* fbxMaterial)
{
    // Check if material has it's own shaders
    const FbxImplementation* fbxImplementation = NULL;
   
    fbxImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_MENTALRAY);
    if (fbxImplementation)
    {
        enLog << "ERROR: Engine is not supporting MentalRay shaders in FBX materials!\n";
        return true;
    }
    fbxImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_CGFX);
    if (fbxImplementation)
    {
        enLog << "ERROR: Engine is not supporting CgFX shaders in FBX materials!\n";
        return true;
    }
    fbxImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL);
    if (fbxImplementation)
    {
        enLog << "ERROR: Engine is not supporting HLSL shaders in FBX materials!\n";
        return true;
    }
    fbxImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_OGS);
    if (fbxImplementation)
    {
        enLog << "ERROR: Engine is not supporting OGS shaders in FBX materials!\n";
        return true;
    }

    return false;
}

bool LoadMaterialProperty(FbxSurfaceMaterial* material, const char* fbxPropertyName, float* value)
{
    const FbxProperty fbxProperty = material->FindProperty( fbxPropertyName );
    if (!fbxProperty.IsValid())
    {
        return false;
    }

    double propertyValue = fbxProperty.Get<double>();
    *value = static_cast<float>(propertyValue);
    return true;
}

bool LoadMaterialProperty(FbxSurfaceMaterial* material, const char* fbxPropertyName, const char* fbxPropertyFactorName, float* value)
{
    const FbxProperty fbxProperty = material->FindProperty( fbxPropertyName );
    if (!fbxProperty.IsValid())
    {
        return false;
    }

    double propertyValue = fbxProperty.Get<double>();

    // Multiply by factor, if required
    const FbxProperty fbxPropertyFactor = material->FindProperty( fbxPropertyFactorName );
    if (fbxPropertyFactor.IsValid())
    {
        double factor = fbxPropertyFactor.Get<double>();
        if (factor != 1.0)
        {
            propertyValue *= factor;
        }
    }

    *value = static_cast<float>(propertyValue);
    return true;
}

bool LoadMaterialProperty(FbxSurfaceMaterial* material, const char* fbxPropertyName, const char* fbxPropertyFactorName, float3* value)
{
    const FbxProperty fbxProperty = material->FindProperty( fbxPropertyName );
    if (!fbxProperty.IsValid())
    {
        return false;
    }

    FbxDouble3 propertyValue = fbxProperty.Get<FbxDouble3>();

    // Multiply by factor, if required
    const FbxProperty fbxPropertyFactor = material->FindProperty( fbxPropertyFactorName );
    if (fbxPropertyFactor.IsValid())
    {
        double factor = fbxPropertyFactor.Get<double>();
        if (factor != 1.0)
        {
            propertyValue[0] *= factor;
            propertyValue[1] *= factor;
            propertyValue[2] *= factor;
        }
    }

    *value = float3(static_cast<float>(propertyValue[0]), static_cast<float>(propertyValue[1]), static_cast<float>(propertyValue[2]));
    return true;
}

bool LoadMaterialPropertyMap(FbxSurfaceMaterial* material, const char* fbxPropertyName, std::shared_ptr<en::gpu::Texture> map)
{
    const FbxProperty fbxProperty = material->FindProperty( fbxPropertyName );
    if (!fbxProperty.IsValid())
    {
        return false;
    }

    const sint32 texturesCount = fbxProperty.GetSrcObjectCount();
    if (texturesCount == 0)
    {
        return false;
    }

    const FbxFileTexture* fbxTextureFile = static_cast<const FbxFileTexture*>(fbxProperty.GetSrcObject(0));
    if (!fbxTextureFile)
    {
        return false;
    }

    // FBX WA: Remove global path, leaving only filename
    std::string filename(fbxTextureFile->GetFileName());
    sint64 nameStart = filename.rfind('\\');
    if (nameStart != std::string::npos)
    {
        filename.erase(0, nameStart + 1);
    }

    // Texture should be already loaded and this call should
    // only link it to material. If it isn't loaded yet, 
    // resources manager will ensure to do that.
    map = Resources.load.texture(filename);
    return true;
}

en::resources::Material LoadMaterial(FbxSurfaceMaterial* fbxMaterial)
{
    assert(fbxMaterial);
    en::resources::Material material;

    // Check if material is not using shaders
    if (IsMaterialUsingShaders(fbxMaterial))
    {
        return material;
    }

    // Name
    material.name = std::string(fbxMaterial->GetName());
   
    // // Emmisive
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sEmissive, material.emmisive.map);
    // LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &material.emmisive.color);
    // 
    // // Ambient
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sAmbient, material.ambient.map);
    // LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor,  &material.ambient.color);
    // 
    // // Diffuse
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sDiffuse, material.diffuse.map);
    // LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor,  &material.diffuse.color);
    // 
    // // Specular
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sSpecular, material.specular.map);
    // LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &material.specular.color);
    // LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sShininess, &material.specular.exponent);
    // 
    // // Transparency
    // material.transparency.on = LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sTransparentColor, material.transparency.map);
    // LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sTransparentColor, FbxSurfaceMaterial::sTransparencyFactor, &material.transparency.color);
    // 
    // // Normal
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sNormalMap, material.normal.map);
    // 
    // // Displacement
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sDisplacementColor, material.displacement.map);  
    // LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sVectorDisplacementColor, material.displacement.vectors);   

    // Emmisive
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sEmissive, material.emmisive);
    //LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &material.emmisive.color);
   
    // Ambient
    //LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sAmbient, material.ambient.map);
    //LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor,  &material.ambient.color);
   
    // Diffuse
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sDiffuse, material.albedo);
    //LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor,  &material.diffuse.color);
   
    // Specular
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sSpecular, material.cavity);
    //LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &material.specular.color);
    //LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sShininess, &material.specular.exponent);
   
    // Transparency
    //material.transparency.on = 
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sTransparentColor, material.opacity);
    //LoadMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sTransparentColor, FbxSurfaceMaterial::sTransparencyFactor, &material.transparency.color);
   
    // Normal
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sNormalMap, material.normal);
   
    // Displacement
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sDisplacementColor, material.displacement);  
    LoadMaterialPropertyMap(fbxMaterial, FbxSurfaceMaterial::sVectorDisplacementColor, material.vector);  

    return material;
}

// Checks if mesh materials use the same mapping mode
bool MaterialsShareMappingMode(FbxMesh* fbxMesh, FbxGeometryElement::EMappingMode& fbxMode)
{
    uint32 numMaterials = fbxMesh->GetElementMaterialCount();
   
    FbxGeometryElementMaterial* fbxMaterial = fbxMesh->GetElementMaterial(0);
    FbxGeometryElement::EMappingMode mode = fbxMaterial->GetMappingMode();
   
    for(uint32 material=0; material<numMaterials; ++material)
    {
        fbxMaterial = fbxMesh->GetElementMaterial(material);
        if (mode != fbxMaterial->GetMappingMode())
        {
            return false;
        }
    }

    fbxMode = mode;
    return true;
}




std::vector<en::resources::Mesh> LoadMesh(FbxMesh* fbxMesh)
{
    std::vector<en::resources::Mesh> meshes;

    enum MeshMaterialType
    {
        Unsupported  = 0,  // Materials are assigned to mesh in unsupported way
        Default         ,  // Mesh has no material, default should be used
        One             ,  // Mesh uses one material, default case
        Multiple           // Mesh uses several materials on per polygon basis, it needs to be splited
    };

    uint32 numMeshes       = 1;
  //sint32 numLayers       = fbxMesh->GetLayerCount();
    sint32 numMaterials    = fbxMesh->GetElementMaterialCount();
    uint32 numTriangles    = fbxMesh->GetPolygonCount();
  //uint32 numVertices     = fbxMesh->GetControlPointsCount();
    sint32 numNormals      = fbxMesh->GetElementNormalCount();
    sint32 numTangents     = fbxMesh->GetElementTangentCount();
    sint32 numBitangents   = fbxMesh->GetElementBinormalCount();
    sint32 numTexCoordSets = fbxMesh->GetUVLayerCount();
    sint32 numTexCoords    = fbxMesh->GetElementUVCount();

    // Currently support up to 4 different texture coordinate sets
    assert( numTexCoordSets <= 4 );

    // Mesh needs to be triangulated before importing
    assert( fbxMesh->IsTriangleMesh() );

    // Determine type of materials used in mesh
    MeshMaterialType type = Unsupported;
    if (numMaterials == 0)
    {
        type = Default;
    }
    else
    if (numMaterials == 1)
    {
        type = One;
    }
    else
    {
        FbxGeometryElement::EMappingMode mode = FbxGeometryElement::eNone;
        if (MaterialsShareMappingMode(fbxMesh, mode))
        {
            // Currently only ByPolygon mapping of multiple materials is supported
            if (mode == FbxGeometryElement::eByPolygon)
            {
                type = Multiple;
            }
        }
    }

    // If input mesh uses unsupported way of mapping
    // materials into geometry it cannot be loaded.
    if (type == Unsupported)
    {
        // TODO: Create proper return !
        enLog << "WARNING! Unsupported mode of mapping materials onto mesh in FBX file!\n";
        return meshes;
    }

    // If input mesh uses several different materials, it 
    // needs to be splitted to separate meshes, each one 
    // using unique material.
    if (type == Multiple)
    {
        numMeshes = numMaterials;
    }

    // Allocate array of temporary meshes for unpacking.
    UnpackedMesh* unpackedMesh = new UnpackedMesh[numMeshes];
    assert( unpackedMesh );

    // Init meshes AABB's to invalid
    for(uint32 i=0; i<numMeshes; ++i)
    {
        en::resources::BoundingBox& box = unpackedMesh[i].AABB;
        box.axisX.minimum =  1000.0f;
        box.axisX.maximum = -1000.0f;
        box.axisY.minimum =  1000.0f;
        box.axisY.maximum = -1000.0f;
        box.axisZ.minimum =  1000.0f;
        box.axisZ.maximum = -1000.0f;
    }

    // All meshes has the same name as source mesh
    for(uint32 i=0; i<numMeshes; ++i)
    {
        unpackedMesh[i].name = fbxMesh->GetName();
    }

    // If mesh has no material, default one should be used.
    if (type == Default)
    {
        unpackedMesh->material = en::resources::Material();
    }

    // If mesh uses one or more materials, load them
    for(sint32 i=0; i<numMaterials; ++i)
    {
        FbxGeometryElementMaterial* fbxMaterial = fbxMesh->GetElementMaterial(i);
   
        // Material array in node is referenced through id's
        assert( fbxMaterial );
        assert( fbxMaterial->GetReferenceMode() == FbxGeometryElement::eIndex ||
                fbxMaterial->GetReferenceMode() == FbxGeometryElement::eIndexToDirect );

        // Load material and assign it to mesh
        unpackedMesh[i].material = LoadMaterial( fbxMesh->GetNode()->GetMaterial(i) );
    }

    // Count polygons used by each mesh
    unpackedMesh[0].polygons = numTriangles;
    unpackedMesh[0].indexes.resize(numTriangles * 3, 0);
    unpackedMesh[0].optimized.resize(numTriangles * 3, 0);
    if (type == Multiple)
    {
        for(uint32 i=0; i<numMeshes; ++i)
        {
            unpackedMesh[i].polygons = 0;
        }

        FbxLayerElementArrayTemplate<sint32>* materialsIndices = &fbxMesh->GetElementMaterial()->GetIndexArray();
        for(uint32 polygon=0; polygon<numTriangles; ++polygon)
        {
            const uint32 materialId = materialsIndices->GetAt(polygon);
            unpackedMesh[materialId].polygons++;
        }

        for(uint32 i=0; i<numMeshes; ++i)
        {
            unpackedMesh[i].indexes.resize(unpackedMesh[i].polygons * 3, 0);
            unpackedMesh[i].optimized.resize(unpackedMesh[i].polygons * 3, 0);
        }
    }
   
    // Sources of geometry data
    const FbxVector4*                 fbxVertices   = fbxMesh->GetControlPoints();
    const FbxGeometryElementNormal*   fbxNormals    = fbxMesh->GetElementNormal(0);
    const FbxGeometryElementTangent*  fbxTangents   = fbxMesh->GetElementTangent(0);
    const FbxGeometryElementBinormal* fbxBitangents = fbxMesh->GetElementBinormal(0);
    const FbxGeometryElementUV*       fbxTexCoords  = fbxMesh->GetElementUV(0);

    bool hasNormals    = ( numNormals > 0 );
    bool hasTangents   = ( numTangents > 0 );
    bool hasBitangents = ( numBitangents > 0 );
    bool hasTexCoords  = ( numTexCoords > 0 );

    // Compose for each sub mesh, arrays of references to 
    // vertices and their attributes values, together with 
    // polygons lists referencing them.
    FbxLayerElementArrayTemplate<sint32>* materialsIndices = nullptr;
    if (numMeshes > 1)
    {
        FbxGeometryElementMaterial* elementMaterial = fbxMesh->GetElementMaterial();
        materialsIndices = &elementMaterial->GetIndexArray();
    }

    uint32 polygonVertex = 0;
    Vertex vertex;
    for(uint32 polygon=0; polygon<numTriangles; ++polygon)
    {
        const uint32 mesh = numMeshes > 1 ? materialsIndices->GetAt(polygon) : 0;

        // Mesh geometry should be triangulated now
        assert( fbxMesh->GetPolygonSize(polygon) == 3 );
        for(uint32 vertice=0; vertice<3; ++vertice)
        {
            vertex.position    = fbxMesh->GetPolygonVertex(polygon, vertice);
            vertex.normal      = -1;
            vertex.tangent     = -1;
            vertex.bitangent   = -1;
            vertex.texcoord[0] = -1;
            vertex.texcoord[1] = -1;
            vertex.texcoord[2] = -1;
            vertex.texcoord[3] = -1;

            // Normal
            if (hasNormals)
            {
                FbxGeometryElement::EMappingMode mapping = fbxNormals->GetMappingMode();
                FbxGeometryElement::EReferenceMode reference = fbxNormals->GetReferenceMode();

                if (mapping == FbxGeometryElement::eByControlPoint)
                {
                    vertex.normal = vertex.position;
                    if (reference != FbxGeometryElement::eDirect)
                    {
                        vertex.normal = fbxNormals->GetIndexArray().GetAt( vertex.position );
                    }
                }
                if (mapping == FbxGeometryElement::eByPolygonVertex)
                {
                    vertex.normal = polygonVertex;
                    if (reference != FbxGeometryElement::eDirect)
                    {
                        vertex.normal = fbxNormals->GetIndexArray().GetAt(polygonVertex);
                    }
                }
            }

            // Tangent
            if (hasTangents)
            {
                FbxGeometryElement::EMappingMode mapping = fbxTangents->GetMappingMode();
                FbxGeometryElement::EReferenceMode reference = fbxTangents->GetReferenceMode();

                if (mapping == FbxGeometryElement::eByControlPoint)
                {
                    vertex.tangent = vertex.position;
                    if (reference != FbxGeometryElement::eDirect)
                    {
                        vertex.tangent = fbxTangents->GetIndexArray().GetAt(vertex.position);
                    }
                }
                if (mapping == FbxGeometryElement::eByPolygonVertex)
                {
                    vertex.tangent = polygonVertex;
                    if (reference != FbxGeometryElement::eDirect)
                    {
                        vertex.tangent = fbxTangents->GetIndexArray().GetAt(polygonVertex);
                    }
                }
            }

            // Bitangent
            if (hasBitangents)
            {
                FbxGeometryElement::EMappingMode mapping = fbxBitangents->GetMappingMode();
                FbxGeometryElement::EReferenceMode reference = fbxBitangents->GetReferenceMode();

                if (mapping == FbxGeometryElement::eByControlPoint)
                {
                    vertex.bitangent = vertex.position;
                    if (reference != FbxGeometryElement::eDirect)
                    {
                        vertex.bitangent = fbxBitangents->GetIndexArray().GetAt(vertex.position);
                    }
                }
                if (mapping == FbxGeometryElement::eByPolygonVertex)
                {
                    vertex.bitangent = polygonVertex;
                    if (reference != FbxGeometryElement::eDirect)
                    {
                        vertex.bitangent = fbxBitangents->GetIndexArray().GetAt(polygonVertex);
                    }
                }
            }

            // Texture Coordinates
            if (hasTexCoords)
            {
                for(sint32 set=0; set<numTexCoordSets; ++set)
                {
                    fbxTexCoords = fbxMesh->GetElementUV(set);

                    FbxGeometryElement::EMappingMode mapping = fbxTexCoords->GetMappingMode();
                    FbxGeometryElement::EReferenceMode reference = fbxTexCoords->GetReferenceMode();

                    if (mapping == FbxGeometryElement::eByControlPoint)
                    {
                        vertex.texcoord[set] = vertex.position;
                        if (reference != FbxGeometryElement::eDirect)
                        {
                            vertex.texcoord[set] = fbxTexCoords->GetIndexArray().GetAt(vertex.position);
                        }
                    }
                    if (mapping == FbxGeometryElement::eByPolygonVertex)
                    {
                        vertex.texcoord[set] = polygonVertex;
                        if (reference != FbxGeometryElement::eDirect)
                        {
                            vertex.texcoord[set] = fbxTexCoords->GetIndexArray().GetAt(polygonVertex);
                        }
                    }
                }            
            }

            // Try to reuse vertice if already exist. This part 
            // of code makes mesh loading time O(N2)/2 by vertices 
            // count which is very slow. Best possible mesh 
            // buffer is prefered and FBX should be loaded only
            // once during offline conversion to engine format.
            sint32 reuseIndex = -1;
            //
            // TODO: Enable reusing vertexes only for format conversion, building final optimized mesh
            //
            //uint32 vertices = unpackedMesh[mesh].vertices.size();
            //for(uint32 i=0; i<vertices; ++i)
            //{   
            //    if (unpackedMesh[mesh].vertices[i] == vertex)
            //    {
            //        reuseIndex = i;
            //        break;
            //    }
            //}

            // Add vertice and polygon to mesh 
            if (reuseIndex < 0)
            {
                unpackedMesh[mesh].indexes[(polygon * 3) + vertice] = static_cast<uint32>(unpackedMesh[mesh].vertices.size());
                unpackedMesh[mesh].vertices.push_back(vertex);

                // Update mesh AABB
                en::resources::BoundingBox& box = unpackedMesh[mesh].AABB;

                float3 position;
                position.x = static_cast<float>(fbxVertices[ vertex.position ][0]);
                position.y = static_cast<float>(fbxVertices[ vertex.position ][1]);
                position.z = static_cast<float>(fbxVertices[ vertex.position ][2]);

                if (position.x < box.axisX.minimum)
                {
                    box.axisX.minimum = position.x;
                }
                if (position.x > box.axisX.maximum)
                {
                    box.axisX.maximum = position.x;
                }
                if (position.y < box.axisY.minimum)
                {
                    box.axisY.minimum = position.y;
                }
                if (position.y > box.axisY.maximum)
                {
                    box.axisY.maximum = position.y;
                }
                if (position.z < box.axisZ.minimum)
                {
                    box.axisZ.minimum = position.z;
                }
                if (position.z > box.axisZ.maximum)
                {
                    box.axisZ.maximum = position.z;
                }
            }
            else
            {
                unpackedMesh[mesh].indexes[(polygon * 3) + vertice] = reuseIndex;
            }

            polygonVertex++;
        }
    }

    // Optimize indexes order for Post-Transform Vertex Cache Size
    //
    // TODO: Enable Forsyth only for format conversion, building final optimized mesh
    //
    //for(uint32 mesh=0; mesh<numMeshes; ++mesh)
    //   Forsyth::optimize(unpackedMesh[mesh].indexes, 
    //                     unpackedMesh[mesh].optimized, 
    //                     unpackedMesh[mesh].vertices.size());
    //

    // Optimization TODO: Generate triangle stripes here



    // After gathering reference data for each mesh,
    // it is time to create GPU optimized buffer for
    // geometry. It will store data of all sub-meshes.
    Formatting formatting;
    uint32 vertices   = 0;
    uint32 columns    = 1;
    uint32 rowSize    = 12;

    // Calculate total count of vertices in buffer
    for(uint32 mesh=0; mesh<numMeshes; ++mesh)
    {
        vertices += unpackedMesh[mesh].vertices.size();
    }

    // Position
    formatting.column[0] = Attribute::v3f32; // inPosition

    // Normal
    if (hasNormals)
    {
        rowSize += 12;
        formatting.column[columns] = Attribute::v3f32; // inNormal
        columns++;
    }

    // Tangent
    // Tangenst will be reconstructed in the shader
    // using equation cross(inBitangent, inNormal)

    // Bitangent
    if (hasNormals && (hasTangents || hasBitangents) )
    {
        rowSize += 12;
        formatting.column[columns] = Attribute::v3f32; // inBitangent
        columns++;
    }

    // Texture Coordinates
    if (hasTexCoords)
    {
        // Texture Coordinate Sets names
        FbxStringList texCoordSetNames;
        fbxMesh->GetUVSetNames(texCoordSetNames);

        // If there is more than one texture coordinate set,
        // only first one has default name, and all additional
        // ones will hold their original names.
        for(sint32 set=0; set<numTexCoordSets; ++set)
        {   
            rowSize += 8;
            formatting.column[columns] = Attribute::v2f32; // (set == 0) ? "inTexCoord0" : string(texCoordSetNames.GetStringAt(set));
            columns++;
        }
    }

    // Generate geometry buffer 
    uint8* geometry = new uint8[vertices * rowSize];
    assert( geometry );

    // Fill buffer with data
    uint32 offset = 0;
    for(uint32 mesh=0; mesh<numMeshes; ++mesh)
    {
        uint32 vertexes = static_cast<uint32>(unpackedMesh[mesh].vertices.size());
        for(uint32 vertice=0; vertice<vertexes; ++vertice)
        {
            Vertex& vertex = unpackedMesh[mesh].vertices[vertice];

            // Vertice
            *((float*)(geometry + offset + 0*sizeof(float))) = static_cast<float>(fbxVertices[ vertex.position ][0]);
            *((float*)(geometry + offset + 1*sizeof(float))) = static_cast<float>(fbxVertices[ vertex.position ][1]);
            *((float*)(geometry + offset + 2*sizeof(float))) = static_cast<float>(fbxVertices[ vertex.position ][2]);
            offset += 3*sizeof(float);

            // Normal
            if (hasNormals)
            {
                FbxVector4 normal = fbxNormals->GetDirectArray().GetAt( vertex.normal );
                *((float*)(geometry + offset + 0*sizeof(float))) = static_cast<float>(normal[0]);
                *((float*)(geometry + offset + 1*sizeof(float))) = static_cast<float>(normal[1]);
                *((float*)(geometry + offset + 2*sizeof(float))) = static_cast<float>(normal[2]);
                offset += 3*sizeof(float);
            }

            // Bitangent
            if (hasNormals && hasTangents && !hasBitangents)
            {
                // Bitangent = cross(inNormal, inTangent)
                FbxVector4 normal  = fbxNormals->GetDirectArray().GetAt( vertex.normal );
                FbxVector4 tangent = fbxTangents->GetDirectArray().GetAt( vertex.tangent );

                float3 vNormal   = float3(static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2]));
                float3 vTangent  = float3(static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2]));
                float3 bitangent = cross(vNormal, vTangent);

                *((float*)(geometry + offset + 0*sizeof(float))) = bitangent.x;
                *((float*)(geometry + offset + 1*sizeof(float))) = bitangent.y;
                *((float*)(geometry + offset + 2*sizeof(float))) = bitangent.z;
                offset += 3*sizeof(float);
            }
            if (hasNormals && hasBitangents)
            {
                FbxVector4 bitangent = fbxBitangents->GetDirectArray().GetAt( vertex.bitangent );
                *((float*)(geometry + offset + 0*sizeof(float))) = static_cast<float>(bitangent[0]);
                *((float*)(geometry + offset + 1*sizeof(float))) = static_cast<float>(bitangent[1]);
                *((float*)(geometry + offset + 2*sizeof(float))) = static_cast<float>(bitangent[2]);
                offset += 3*sizeof(float);
            }

            // Texture Coordinates
            if (hasTexCoords)
            {
                for(sint32 set=0; set<numTexCoordSets; ++set)
                {   
                    fbxTexCoords = fbxMesh->GetElementUV(set);
                    FbxVector2 texcoord = fbxTexCoords->GetDirectArray().GetAt( vertex.texcoord[set] );
                    *((float*)(geometry + offset + 0*sizeof(float))) = static_cast<float>(texcoord[0]);
                    *((float*)(geometry + offset + 1*sizeof(float))) = static_cast<float>(texcoord[1]);
                    offset += 2*sizeof(float);
                }
            }
        }
    }

    std::unique_ptr<Buffer> vbo(en::ResourcesContext.defaults.enHeapBuffers->createBuffer(vertices, formatting, 0u));

    // Create staging buffer
    uint32 stagingSize = vertices * rowSize;
    std::unique_ptr<gpu::Buffer> staging(en::ResourcesContext.defaults.enStagingHeap->createBuffer(BufferType::Transfer, stagingSize));
    if (!staging)
    {
        enLog << "ERROR: Cannot create staging buffer!\n";
        return meshes;
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
    command->copy(*staging, *vbo);
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

    // Now there will be created index buffer optimized
    // for GPU usage. It will store data of all sub-meshes.
    uint32 indexCount = 0;
    for(uint32 mesh=0; mesh<numMeshes; ++mesh)
    {
        indexCount += unpackedMesh[mesh].optimized.size();
    }

    // Create indices buffer
    Attribute format = Attribute::u32;

    // Determine index buffer entry size. For each
    // range, count one vertex less than maximum, 
    // to leave place for primitive restart index.
    uint32 indexSize = 4;
    if (vertices <= 255)
    {
        format = Attribute::u8;
        indexSize = 1;
    }
    else
    if (vertices <= 65535)
    {
        format = Attribute::u16;
        indexSize = 2;
    }

    // Generate buffer
    uint8* elements = new uint8[indexCount * indexSize];
    assert( elements );

    // Fill buffer with data and send to GPU
    uint32 vboBegin = 0;
    uint32 iboBegin = 0;
    offset = 0;
    for(uint32 mesh=0; mesh<numMeshes; ++mesh)
    {
        for(uint32 indice=0; indice<unpackedMesh[mesh].optimized.size(); ++indice)
        {
            uint32 index = vboBegin + unpackedMesh[mesh].optimized[indice];
            if (format == Attribute::u32)
            {
                *((uint32*)(elements + offset)) = index;
            }
            else
            if (format == Attribute::u16)
            {
                *((uint16*)(elements + offset)) = static_cast<uint16>(index);
            }
            else
            {
                *((uint8*)(elements + offset)) = static_cast<uint8>(index);
            }

            offset += indexSize;
        }

        vboBegin += unpackedMesh[mesh].vertices.size();
    }
    std::unique_ptr<gpu::Buffer> ibo(en::ResourcesContext.defaults.enHeapBuffers->createBuffer(indexCount, format));

    // Create staging buffer
    stagingSize = indexCount * indexSize;
    staging.swap(std::unique_ptr<gpu::Buffer>(en::ResourcesContext.defaults.enStagingHeap->createBuffer(BufferType::Transfer, stagingSize)));
    if (!staging)
    {
        enLog << "ERROR: Cannot create staging buffer!\n";
        return meshes;
    }

    // Read texture to temporary buffer
    dst = staging->map();
    memcpy((void*)dst, elements, stagingSize);
    staging->unmap();
    
    // TODO: In future distribute transfers to different queues in the same queue type family
    queueType = gpu::QueueType::Universal;
    if (Graphics->primaryDevice()->queues(gpu::QueueType::Transfer) > 0u)
    {
        queueType = gpu::QueueType::Transfer;
    }

    // Copy data from staging buffer to final texture
    command = Graphics->primaryDevice()->createCommandBuffer(queueType);
    command->start();
    command->copy(*staging, *vbo);
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

    delete [] elements;

    // Local transformation matrix
    FbxDouble3 translation = fbxMesh->GetNode()->LclTranslation.Get();
    FbxDouble3 rotation    = fbxMesh->GetNode()->LclRotation.Get();
    FbxDouble3 scale       = fbxMesh->GetNode()->LclScaling.Get();
   
    float4x4 matrix( float3(static_cast<float>(translation[0]), static_cast<float>(translation[1]), static_cast<float>(translation[2])),
                     float3(static_cast<float>(rotation[0]),    static_cast<float>(rotation[1]),    static_cast<float>(rotation[2])),
                     float3(static_cast<float>(scale[0]),       static_cast<float>(scale[1]),       static_cast<float>(scale[2]))       );

    // Describe output meshes
    meshes.resize(numMeshes);
    vboBegin = 0;
    iboBegin = 0;
    for(uint32 mesh=0; mesh<numMeshes; ++mesh)
    {
        // TODO: Refactor to new Model/Mesh description

        //meshes[mesh].name             = unpackedMesh[mesh].name;
        //meshes[mesh].material         = unpackedMesh[mesh].material;
        //meshes[mesh].matrix           = matrix;
        //meshes[mesh].AABB             = unpackedMesh[mesh].AABB;
        //meshes[mesh].geometry.buffer  = vbo;
        //meshes[mesh].geometry.begin   = vboBegin;
        //meshes[mesh].geometry.end     = vboBegin + static_cast<uint32>(unpackedMesh[mesh].vertices.size()) - 1u;
        //meshes[mesh].elements.buffer  = ibo;
        //meshes[mesh].elements.type    = Triangles;
        //meshes[mesh].elements.offset  = iboBegin * indexSize;
        //meshes[mesh].elements.indexes = iboBegin + static_cast<uint32>(unpackedMesh[mesh].optimized.size()) - 1u;

        vboBegin += unpackedMesh[mesh].vertices.size();
        iboBegin += unpackedMesh[mesh].optimized.size();
    }

    // Free temporary data
    delete [] unpackedMesh;

    return meshes;
}

std::shared_ptr<en::resources::Model> load(const std::string& filename, const std::string& name)
{
    using namespace en::storage;

    // Try to reuse already loaded models
    if (ResourcesContext.models.find(name) != ResourcesContext.models.end())
    {
        return ResourcesContext.models[name];
    }

    // Create importer of FBX content
    FbxImporter* fbxImporter = FbxImporter::Create(ResourcesContext.fbxManager, "");
    if (!fbxImporter) 
    {
        enLog << "ERROR: Cannot create FBX importer!\n";
        return std::shared_ptr<en::resources::Model>(NULL);
    }

    // Load FBX file to importer
    if (!fbxImporter->Initialize(filename.c_str(), -1, ResourcesContext.fbxManager->GetIOSettings()))
    {
        if (!fbxImporter->Initialize((ResourcesContext.path.models + filename).c_str(), -1, ResourcesContext.fbxManager->GetIOSettings())) 
        {
            enLog << std::string("ERROR: FBX importer initialization failed with error: " + std::string(fbxImporter->GetStatus().GetErrorString()) + "\n");
            return std::shared_ptr<en::resources::Model>(NULL);
        }
    }
      
    // Create local scene for imported model
    FbxScene* fbxScene = FbxScene::Create(ResourcesContext.fbxManager, "fbxScene");

    // Import the contents of the file into the scene
    fbxImporter->Import(fbxScene);

    // After file is loaded, importer is not needed anymore
    fbxImporter->Destroy();
    
    // Convert scene coordinate system to OpenGL
    FbxAxisSystem::OpenGL.ConvertScene(fbxScene);

    // (1) Load textures used in scene
#ifdef EN_DEBUG
    enLog << std::string("FBX uses " + stringFrom(fbxScene->GetTextureCount()) + " textures:\n");
#endif
    for(sint32 i=0; i<fbxScene->GetTextureCount(); ++i)
    {
        FbxTexture*     fbxTexture  = fbxScene->GetTexture(i);
        FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(fbxTexture);

        // Check importer consistency
        if (!fileTexture)
        {
            enLog << std::string("ERROR: FBX file has corrupted texture description!\n");
            return std::shared_ptr<en::resources::Model>(NULL);
        }

        // Check if texture is resident in FBX
        if (fileTexture->GetUserDataPtr())
        {
            enLog << std::string("ERROR: Engine doesn't support import of textures resident in FBX files!\n");
            return std::shared_ptr<en::resources::Model>(NULL);
        }

        // Load texture from file
        std::string filename(fileTexture->GetFileName());

        // FBX WA: Remove global path, leaving only filename
        sint64 nameStart = filename.rfind('\\');
        if (nameStart != std::string::npos)
        {
            filename.erase(0, nameStart + 1);
        }
#ifdef EN_DEBUG
        enLog << filename << std::endl;
#endif

        Resources.load.texture(filename);
    }



    //vector<en::resources::Material> materials;

    //// Load scene materials
    //sint32 materialCount = fbxScene->GetMaterialCount();
    //materials.reserve( materialCount );
    //for(sint32 i=0; i<materialCount; ++i)
    //{
    //   materials.push_back( LoadMaterial( fbxScene->GetMaterial(i) ) );
    //}

#ifdef EN_DEBUG
    // Print the nodes of the scene and their attributes recursively.
    // Note that we are not printing the root node, because it should
    // not contain any attributes.
    FbxNode* fbxRootNode = fbxScene->GetRootNode();
    if (fbxRootNode) 
    {
        for(sint32 i=0; i<fbxRootNode->GetChildCount(); ++i)
        {
            PrintNode(fbxRootNode->GetChild(i));
        }
    }
#endif

    // Create raw model structure for storing processed data
    en::raw::Model rawModel;
    rawModel.name = fbxScene->GetName();


    // Create model
    std::shared_ptr<en::resources::Model> model = nullptr; // TODO: Refactor this code: std::make_shared<en::resources::Model>();

    // TODO: Refactor to new Model description
    // model->name = std::string(fbxScene->GetName());

    // (x) Load skeleton
    //     There should be only one skeleton child node of root
    FbxNode* node  = fbxScene->GetRootNode();
    FbxNode* child = NULL;
    uint32 childs  = node->GetChildCount();



    //FbxAnimEvaluator* evaluator = fbxScene->GetEvaluator();      
    //sint32 numStacks = fbxScene->GetSrcObjectCount(FBX_TYPE(FbxAnimStack));

    //for(uint32 stack=0; stack<numStacks; ++stack)
    //{
    //    FbxAnimStack* fbxAnimStack = FbxCast<FbxAnimStack>(fbxScene->GetSrcObject(FBX_TYPE(FbxAnimStack), stack));
    //    sint32 numAnimLayers = fbxAnimStack->GetMemberCount(FBX_TYPE(FbxAnimLayer));
    //    for(uint32 layer=0; layer<numAnimLayers; ++layer)
    //    {
    //        FbxAnimLayer* fbxAnimLayer = fbxAnimStack->GetMember(FBX_TYPE(FbxAnimLayer), layer);
    //    }
    //}



    // Get information about Control Points affected by bone
    //FbxCluster* cluster;
    //sint32 boneControlPointsAffected = cluster->GetControlPointIndicesCount();
    //sint32* boneControlPointsIndices = cluster->GetControlPointIndices();
    //double* boneControlPointsWeights = cluster->GetControlPointWeights();



    //animation stacks (FbxAnimStack), 
    //animation layers (FbxAnimLayer), 
    //animation curve nodes (FbxAnimCurveNode), 
    //animation curves (FbxAnimCurve), 
    //animation curve keys (FbxAnimCurveKey).
    //
    //KFbxSkeleton - in scene, for skeletal animation?
    
    enum InterpolationMode
    {
        Constant                = 0, // Constant value until next key
        Linear                     , // Linear progression to next key
        Cubic                      , // Cubic progression to next key
        InterpolationModesCount
    };
    
    enum TangentMode
    {
        Auto              = 0, // Spline cardinal (takes prev,cur,next key cp, + tension influence to compute curved poly-line)
        SplineTCB            , // Tension, Continuity, Bias
        User                 , // Next slope at the left equal to slope at the right
        TangentModesCount
    };

    //Tangent options:
    //
    //Break (Independent left and right slopes)
    //Clamp (Key should be flat if next or previous key has the same value)
    //Time independent

    struct CurveControlPoint
    {
        double        time;
        InterpolationMode interpolation;
        TangentMode       tangent;
    };

    //FbxAnimCurve*    fbxCurve = NULL;
    //FbxAnimCurveKey fbxKey;
    //
    //sint32 keys = fbxCurve->KeyGetCount();
    //for(sint32 i=0; i<keys; ++i)
    //   {
    //   CurveControlPoint cp;
    //
    //   fbxKey = fbxCurve->KeyGet(i);
    //
    //   // Interpolation mode
    //   FbxAnimCurveDef::EInterpolationType interpolation = fbxKey.GetInterpolation();
    //   if (interpolation == FbxAnimCurveDef::eInterpolationConstant) cp.interpolation = Constant;
    //	if (interpolation == FbxAnimCurveDef::eInterpolationLinear)   cp.interpolation = Linear;
    //	if (interpolation == FbxAnimCurveDef::eInterpolationCubic)    cp.interpolation = Cubic;
    //
    //   // Tangent mode
    //   FbxAnimCurveDef::ETangentMode tangent = fbxKey.GetTangentMode();
    //// eTangentAuto = 0x00000100,                                                  //! Auto key (spline cardinal).
    //// eTangentTCB = 0x00000200,                                                   //! Spline TCB (Tension, Continuity, Bias)
    //// eTangentUser = 0x00000400,                                                  //! Next slope at the left equal to slope at the right.
    //// eTangentGenericBreak = 0x00000800,                                          //! Independent left and right slopes.
    //// eTangentBreak = eTangentGenericBreak|eTangentUser,                          //! Independent left and right slopes, with next slope at the left equal to slope at the right.
    //// eTangentAutoBreak = eTangentGenericBreak|eTangentAuto,                      //! Independent left and right slopes, with auto key.
    //// eTangentGenericClamp = 0x00001000,                                          //! Clamp: key should be flat if next or previous key has the same value (overrides tangent mode).
    //// eTangentGenericTimeIndependent = 0x00002000,                                //! Time independent tangent (overrides tangent mode).
    //// eTangentGenericClampProgressive = 0x00004000|eTangentGenericTimeIndependent //! Clamp progressive: key should be flat if tangent control point is outside [next-previous key] range (overrides tangent mode).
    //
    //   // Velocity mode
    //   FbxAnimCurveDef::EVelocityMode velocity = fbxKey.GetTangentVelocityMode();
    //// eVelocityNone = 0x00000000,                      //! No velocity (default).
    //// eVelocityRight = 0x10000000,                     //! Right tangent has velocity.
    //// eVelocityNextLeft = 0x20000000,                  //! Left tangent has velocity.
    //// eVelocityAll = eVelocityRight|eVelocityNextLeft  //! Both left and right tangents have velocity.
    //
    //   FbxTime time = fbxKey.GetTime();
    //   
    //   }
    //

    double timeStep = 1.0 / 46186158000.0;  // FBX Time Unit - Fraction of second





    //array<float4x4> bone; 


    //struct Skeleton
    //{
    //    float4x4* bone; // Array of bones
    //    Bone*     tree; // Parent-Childs bone references tree 
    //};

    //struct Bone
    //{
    //    float4x4 matrix;
    //    sint32   parent;
    //    uint32   childs;
    //};

    //{
    //// Skeleton with minimum one layer of depth
    //vector< vector<Bone> > skeleton;
    //skeleton.resize(1);
    //uint32 depth = 0;



    //// This function should be called for scene root node.
    //// We assume that there is one and only one skeleton 
    //// child node of root, if skeletal animation is present.
    //void LoadBones(FbxNode* node, uint32 depth, sint32 parent)
    //{
    //uint32 childs = node->GetChildCount();
    //for(uint32 i = 0; i<childs; ++i)  
    //   {
    //   FbxNode* child = node->GetChild(i);
    //   uint32 attributes = child->GetNodeAttributeCount();
    //   FbxNodeAttribute* attribute = NULL;
    
    //   // Check if this child node is a skeleton
    //   for(uint32 j=0; j<attributes; ++j)
    //      {
    //      attribute = child->GetNodeAttributeByIndex(j);
    //      assert( attribute );
    
    //      // If this attribute is not pointing to skeleton, maybe next will
    //      FbxNodeAttribute::EType type = attribute->GetAttributeType();
    //      if (type != FbxNodeAttribute::eSkeleton)
    //         continue;
    
    //      // If this attribute is 
    //      FbxSkeleton* fbxSkeleton = (FbxSkeleton*)attribute;
    //      if (fbxSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
    //         rootFound = true;
    
    //      // Add new layer in temporary skeleton structure
    //      if (depth == skeleton.size())
    //         skeleton.resize(skeleton.size() + 1);
    
    //      // This child node is a bone, we will add it to skeleton
    //      sint32 index = skeleton[depth].size();
    //      Bone bone;
    //      bone.matrix = float4x4( fbxScene->GetEvaluator()->GetNodeLocalTransform(child) );  // USE: LoadMatrix(&bone.matrix, &fbxMatrix);
    //      bone.parent = parent;
    //      bone.childs = 0;       
    //      skeleton[depth].push_back(bone);
    
    //      // Increment parent child bones count
    //      if (parent >= 0)
    //         skeleton[depth-1][parent].childs++;
    
    
    
    // // Only draw the skeleton if it's a limb node and if 
    // // the parent also has an attribute of type skeleton.
    // if (fbxSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode
    
    // if (fbxSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
    //    DisplayDouble("    Limb Length: ", lSkeleton->LimbLength.Get());
    // else 
    // if (fbxSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
    //    DisplayDouble("    Limb Node Size: ", lSkeleton->Size.Get());
    // else 
    // if (fbxSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
    //    DisplayDouble("    Limb Root Size: ", lSkeleton->Size.Get());
    // else 
    // if (fbxSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
    
    // DisplayColor("    Color: ", lSkeleton->GetLimbNodeColor());
    
    
    
    //      // Recursively load child bones of this bone
    //      LoadBones(child, depth++, index);
    //      break;
    //      }
    //   }
    //}
    
    //}
    
    
    //// Search for skeleton root node
    //for(uint32 index = 0; index<childs; ++index)  
    //   {
    //   child = node->GetChild(index);
    //   uint32 attributes = child->GetNodeAttributeCount();
    //   FbxNodeAttribute* attribute = NULL;
    //   
    //   // Check if this child node is a skeleton
    //   for(uint32 j=0; j<attributes; ++j)
    //      {
    //      attribute = child->GetNodeAttributeByIndex(j);
    //      assert( attribute );
    
    //      FbxNodeAttribute::EType type = attribute->GetAttributeType();
    //      if (type != FbxNodeAttribute::eSkeleton)
    //         continue;
    
    //      // We found at least one skeleton root node
    //      Bone bone;
    //      //bone.matrix = ;
    //      bone.parent = -1;
    //      bone.childs = child->GetChildCount();
    //      skeleton[0].push_back(bone);
    //      break;
    //      }
    //   }
    
    
    // (2) Load meshes from fbx
    //     Currently engine is not supporting:
    //     - loading separate mesh by name from FBX as model
    //     - models with mesh hierarchy
    //     - models with multiple LOD's
    bool found = false;
    for(uint32 index=0; index<childs; ++index)  
    {
        child = node->GetChild(index);
        uint32 attributes = child->GetNodeAttributeCount();
        FbxNodeAttribute* attribute = NULL;
      
        // Check if this child node is a mesh
        for(uint32 j=0; j<attributes; ++j)
        {
            attribute = child->GetNodeAttributeByIndex(j);
            assert( attribute );

            FbxNodeAttribute::EType type = attribute->GetAttributeType();
            if (type != FbxNodeAttribute::eMesh)
            {
                continue;
            }

            // Mesh is triangulated (patches are not supported)
            FbxMesh* fbxMesh = child->GetMesh();
            if (!fbxMesh->IsTriangleMesh())
            {
                FbxNodeAttribute* triangulated = ResourcesContext.fbxGeometryConverter->Triangulate(attribute, true);
                assert( triangulated );

                fbxMesh = (FbxMesh*)triangulated;
            }

            // Load mesh as set of submeshes
            std::vector<en::resources::Mesh> submeshes = LoadMesh( fbxMesh );
            
            // Each Node has additional Geometry offset that is local
            // and therefore need to be post-multiplied locally and
            // cannot be passed to child nodes for transformation.
            //
            // FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
            // FbxAMatrix lGlobalOffPosition = lGlobalPosition * GetGeometry(child);
            
            // FBX 2014:
            //
            // WA: Calculate global transformation matrix taking into notice pivot transformation
            //FbxAnimEvaluator* mySceneEvaluator = fbxScene->GetEvaluator();
            //FbxAMatrix fbxMatrix = mySceneEvaluator->GetNodeLocalTransform( child ); 

            // 2015 WA for 2014 WA:
            FbxAMatrix fbxMatrix = child->EvaluateLocalTransform();

            // FBX 2015 Animation Global Matrix Evaluation:
            //
            // FbxAMatrix& lGlobalMatrix = lNode->GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(lNode, lTime);
            // or the exact equivalent :
            // FbxAMatrix& lGlobalMatrix = lNode->EvaluateGlobalTransform(lTime);


            // TODO: Refactor to new Model description
            //for(uint32 i=0; i<submeshes.size(); ++i)
            //   LoadMatrix(&submeshes[i].matrix, &fbxMatrix);

            // TODO: Refactor to new Model description
            //model->mesh.insert(model->mesh.end(), submeshes.begin(), submeshes.end());
            break;
        }
    }

    return model;
}

#endif
} // en::fbx
} // en



   //// Scene traversal info

   //vector<FbxNode*> node;

   //uint32 depth  = 0;

   //vector<uint32> index;


   //childs = node->GetChildCount();



   //node[depth]->GetChild(index[depth])

   //for(uint32 index[depth]=0; index[depth] < node[depth]->GetChildCount(); ++index[depth])
   //   {
   //   
   //   }


   //


   //FbxNodeAttribute* attribute = node->GetNodeAttribute();
   //if (attribute)
   //   {
   //   FbxNodeAttribute::EType type = attribute->GetAttributeType();
   //   switch(type) 
   //      {
   //      case FbxNodeAttribute::eMesh:
   //           {



   //           break;
   //           }
   //      case FbxNodeAttribute::eUnknown: 
   //      case FbxNodeAttribute::eNull: 
   //      case FbxNodeAttribute::eMarker: 
   //      case FbxNodeAttribute::eSkeleton: 
   //      case FbxNodeAttribute::eNurbs: 
   //      case FbxNodeAttribute::ePatch: 
   //      case FbxNodeAttribute::eCamera: 
   //      case FbxNodeAttribute::eCameraStereo:    
   //      case FbxNodeAttribute::eCameraSwitcher: 
   //      case FbxNodeAttribute::eLight: 
   //      case FbxNodeAttribute::eOpticalReference: 
   //      case FbxNodeAttribute::eOpticalMarker: 
   //      case FbxNodeAttribute::eNurbsCurve: 
   //      case FbxNodeAttribute::eTrimNurbsSurface: 
   //      case FbxNodeAttribute::eBoundary: 
   //      case FbxNodeAttribute::eNurbsSurface: 
   //      case FbxNodeAttribute::eShape:        
   //      case FbxNodeAttribute::eLODGroup:     
   //      case FbxNodeAttribute::eSubDiv:       
   //      case FbxNodeAttribute::eCachedEffect: 
   //      case FbxNodeAttribute::eLine:         
   //      default: 
   //           enLog << "WARNING! Unsupported FBX element!\n";
   //           break;
   //      }

   //   }


   //// TODO: Most important part !
   ////       Convert data from fbxScene to engine scene and model representation !

