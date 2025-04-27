/*

 Ngine v5.0
 
 Module      : Resources manager.
 Requirements: none
 Description : Loads, stores and manages all types
               of supported resources. Also protects
               from loading duplicates of already
               loaded resources.

*/

#include "core/storage.h"
#include "core/log/log.h"
#include "core/rendering/device.h"
#include "resources/context.h" 
#include "resources/font.h" 
#include "resources/bmp.h"  
#include "resources/tga.h"
#include "resources/png.h" 
#include "resources/exr.h"
#include "resources/hdr.h"
#include "resources/dds.h"
#include "resources/material.h"   
#include "resources/mtl.h"       
#include "resources/obj.h"   
#include "resources/fbx.h"  
#include "resources/wav.h" 

#ifdef EN_PROFILE
#include "utilities/timer.h"
#endif

#include <string>

using namespace en::gpu;

namespace en
{
namespace resources
{

// Coordinate system
// LineStripes
// (x,y,z)(r,g,b)
float axes[] = 
{ 
    0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f, // Y Arrow - Green
    0.0f,   1.0f,   0.0f,   0.0f, 1.0f, 0.0f,
    0.125f, 0.5f,   0.0f,   0.0f, 1.0f, 0.0f,
    0.0f,   0.5f,   0.0f,   0.0f, 1.0f, 0.0f,
    0.0f,   0.0f,   0.0f,   0.0f, 1.0f, 0.0f,

    0.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f, // X Arrow - Red
    1.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f, 
    0.5f,   0.0f,   0.125f, 1.0f, 0.0f, 0.0f,
    0.5f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,
    0.0f,   0.0f,   0.0f,   1.0f, 0.0f, 0.0f,

    0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 1.0f, // Z Arrow - Blue 
    0.0f,   0.0f,   1.0f,   0.0f, 0.0f, 1.0f, 
    0.0f,   0.125f, 0.5f,   0.0f, 0.0f, 1.0f, 
    0.0f,   0.0f,   0.5f,   0.0f, 0.0f, 1.0f
};

Context::Defaults::Defaults() :
    //program(nullptr),
    enHeapBuffers(nullptr),
    enHeapTextures(nullptr),
    enStagingHeap(nullptr),
    enAlbedoMap(nullptr),
    enMetallicMap(nullptr),
    enCavityMap(nullptr),
    enRoughnessMap(nullptr),
    enAOMap(nullptr),
    enNormalMap(nullptr),
    enDisplacementMap(nullptr),
    enVectorDisplacementMap(nullptr),
    enEmmisiveMap(nullptr),
    enOpacityMap(nullptr),
    //enEmmisiveMap(nullptr),
    //enAmbientMap(nullptr),
    //enDiffuseMap(nullptr),
    //enSpecularMap(nullptr),
    //enTransparencyMap(nullptr),
    //enNormalMap(nullptr),
    //enDisplacementMap(nullptr),
    //enVectorsMap(nullptr),
    enAxes(nullptr)
{
}

Context::Defaults::~Defaults()
{
}

// TODO: Should be EN_SUPPORTS_FBX (and best solution would be plugin system for different formats support through DLLs).
#if defined(EN_PLATFORM_WINDOWS)
Context::Context() :
    fbxManager(nullptr),
    fbxFilter(nullptr),
    fbxGeometryConverter(nullptr)
#else
Context::Context()
#endif
{
/*
    storage.fonts.create(128);
    storage.models.create(4096);
    storage.materials.create(4096);

    fonts.clear();
    models.clear();
    materials.clear();
//*/

    textures.clear();

    path.fonts       = std::string("resources/fonts/");
    path.models      = std::string("resources/models/");
    path.materials   = std::string("resources/materials/");
    path.shaders     = std::string("resources/shaders/");
    path.textures    = std::string("resources/textures/");
    path.sounds      = std::string("resources/sounds/"); 
    path.screenshots = std::string("screenshots/");
}

Context::~Context()
{
}

void Context::create(void)
{
    using namespace en::gpu;

    Log << "Starting module: Resources.\n";

    models.clear();
    materials.clear();
    textures.clear();

    // Create default program for materials
    std::string vsCode, fsCode;
/*
#ifdef EN_OPENGL_DESKTOP
    en::Storage->read(std::string("resources/engine/shaders/default.glsl.1.10.vs"), vsCode);
    en::Storage->read(std::string("resources/engine/shaders/default.glsl.1.10.fs"), fsCode);
#endif
#ifdef EN_OPENGL_MOBILE
    en::Storage->read(std::string("resources/engine/shaders/default.essl.1.00.vs"), vsCode);
    en::Storage->read(std::string("resources/engine/shaders/default.essl.1.00.fs"), fsCode);
#endif
//*/
    //vector<gpu::Shader> shaders(2, Shader(nullptr));
    //shaders[0] = Gpu.shader.create(Vertex, vsCode);
    //shaders[1] = Gpu.shader.create(Fragment, fsCode);
    //defaults.program     = Gpu.program.create(shaders);

    // Create GPU Heap used as backing store for all resources (for now 256MB)
    defaults.enHeapBuffers  = Graphics->primaryDevice()->createHeap(MemoryUsage::Linear, 64*1024*1024);
    defaults.enHeapTextures = Graphics->primaryDevice()->createHeap(MemoryUsage::Tiled, 256*1024*1024);

    // Create Heap as temporary location for resources upload
    defaults.enStagingHeap = Graphics->primaryDevice()->createHeap(MemoryUsage::Upload, 64*1024*1024);

    // TODO: This is temporary solution. Resources should be dynamically streamed in,
    //       from storage to RAM and then VRAM. Also different Heaps should be used
    //       for different kinds of resources / gpu's.
   
    // Create default textures for materials
    //defaults.enAlbedoMap             = png::load(string("./resources/engine/textures/enDefaultAlbedoMap.png"));   
    //defaults.enMetallicMap           = png::load(string("./resources/engine/textures/enDefaultMetallicMap.png"));
    //defaults.enCavityMap             = png::load(string("./resources/engine/textures/enDefaultCavityMap.png"));
    //defaults.enRoughnessMap          = png::load(string("./resources/engine/textures/enDefaultRoughnessMap.png"));
    //defaults.enAOMap                 = png::load(string("./resources/engine/textures/enDefaultAOMap.png"));
    //defaults.enNormalMap             = png::load(string("./resources/engine/textures/enDefaultNormalMap.png"));
    //defaults.enDisplacementMap       = png::load(string("./resources/engine/textures/enDefaultDisplacementMap.png"));
    //defaults.enVectorDisplacementMap = png::load(string("./resources/engine/textures/enDefaultVectorDisplacementMap.png"));
    //defaults.enEmmisiveMap           = png::load(string("./resources/engine/textures/enDefaultEmmisiveMap.png"));
    //defaults.enOpacityMap            = png::load(string("./resources/engine/textures/enDefaultOpacityMap.png"));

    //defaults.enEmmisiveMap     = png::load(string("./resources/engine/textures/enDefaultEmmisiveMap.png"));
    //defaults.enAmbientMap      = png::load(string("./resources/engine/textures/enDefaultAmbientMap.png"));       
    //defaults.enDiffuseMap      = png::load(string("./resources/engine/textures/enDefaultDiffuseMap.png"));       
    //defaults.enSpecularMap     = png::load(string("./resources/engine/textures/enDefaultSpecularMap.png"));   
    //defaults.enTransparencyMap = png::load(string("./resources/engine/textures/enDefaultAlphaMap.png"));     
    //defaults.enNormalMap       = png::load(string("./resources/engine/textures/enDefaultNormalMap.png")); 
    //defaults.enDisplacementMap = png::load(string("./resources/engine/textures/enDefaultDisplacementMap.png"));
    //defaults.enVectorsMap      = png::load(string("./resources/engine/textures/enDefaultVectorMap.png")); 

    // Create default mesh for corrdinate system axes
    Formatting formatting(Attribute::v3f32, Attribute::v3f32); // inPosition, inColor
    defaults.enAxes = std::unique_ptr<Buffer>(defaults.enHeapBuffers->createBuffer(14u, formatting, 0u));

    // Create staging buffer
    uint32 stagingSize = 14u;
    std::unique_ptr<gpu::Buffer> staging(defaults.enStagingHeap->createBuffer(BufferType::Transfer, stagingSize));
    assert( staging );

    // Read texture to temporary buffer
    volatile void* dst = staging->map();
    memcpy((void*)dst, &axes, stagingSize);
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
    command->copy(*staging,*defaults.enAxes);
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

#if defined(EN_PLATFORM_WINDOWS)
    fbxManager = FbxManager::Create();
    fbxFilter  = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(fbxFilter);
    fbxGeometryConverter = new FbxGeometryConverter(fbxManager);
#endif
}

void Context::destroy(void)
{
    Log << "Closing module: Resources.\n";

    //defaults.program                 = nullptr;
                                    
    defaults.enAlbedoMap             = nullptr;
    defaults.enMetallicMap           = nullptr;
    defaults.enCavityMap             = nullptr;
    defaults.enRoughnessMap          = nullptr;
    defaults.enAOMap                 = nullptr;
    defaults.enNormalMap             = nullptr;
    defaults.enDisplacementMap       = nullptr;
    defaults.enVectorDisplacementMap = nullptr;
    defaults.enEmmisiveMap           = nullptr;
    defaults.enOpacityMap            = nullptr;
    //defaults.enEmmisiveMap     = nullptr;
    //defaults.enAmbientMap      = nullptr;
    //defaults.enDiffuseMap      = nullptr;
    //defaults.enSpecularMap     = nullptr;
    //defaults.enTransparencyMap = nullptr;
    //defaults.enNormalMap       = nullptr;
    //defaults.enDisplacementMap = nullptr;
    //defaults.enVectorsMap      = nullptr;
    defaults.enAxes            = nullptr;

    models.clear();
    materials.clear();
    textures.clear();

#if defined(EN_PLATFORM_WINDOWS)
    if (fbxManager)
    {
        fbxManager->Destroy();
        delete fbxGeometryConverter;
    }
#endif
}

 //MaterialParameter::MaterialParameter() :
 //    handle(NULL),
 //    value(NULL),
 //    name(NULL)
 //{
 //}

 //MaterialParameter::~MaterialParameter()
 //{
 //    // Don't delete "value" as it points to shared buffer
 //    // Don't delete "name" as it points to shared string
 //}

 //MaterialSampler::MaterialSampler() :
 //    handle(NULL),
 //    texture(NULL),
 //    name(NULL)
 //{
 //}

 //MaterialSampler::~MaterialSampler()
 //{
 //    // Don't delete "name" as it points to shared string
 //}

MaterialDescriptor::MaterialDescriptor() /*:*/
//    program(nullptr)
{
/*
    parameters.list.clear();
    parameters.buffer = NULL;
    parameters.size   = 0;
    
    samplers.clear();
    
    state.texture.ambient      = false;
    state.texture.diffuse      = false;
    state.texture.specular     = false;
    state.texture.displacement = false;
    state.texture.normal       = false;
    state.texture.alpha        = false;
    
    state.transparent          = false;
//*/
}

MaterialDescriptor::~MaterialDescriptor()
{
    //parameters.list.clear();
    //delete static_cast<uint8*>(parameters.buffer);
    //samplers.clear();
}

bool MaterialDestroy(MaterialDescriptor* const material)
{
    // Fill material with null pointer
    //ResourcesContext.storage.materials.free(material);
    return true; 
}

//Material::Material() :
//    ProxyInterface<struct MaterialDescriptor>(NULL)
//{
//}
       
//Material::Material(struct MaterialDescriptor* src) :
//    ProxyInterface<struct MaterialDescriptor>(src)
//{
//}

//gpu::Program& Material::program(void)
//{
//assert(pointer);

//return pointer->program;
//}

//uint32 Material::parameters(void)
//{
//assert(pointer);

//return pointer->parameters.list.size();
//}

//uint32 Material::samplers(void)
//{
//assert(pointer);

//return pointer->samplers.size();
//}

//bool Material::uses(enum MaterialFeature feature)
//{
//assert(pointer);

//MaterialDescriptor& material = *pointer;

//// TODO: In future, use lookup table for direct access
//if (feature == AmbientMap)
//   return material.state.texture.ambient;
//if (feature == DiffuseMap)
//   return material.state.texture.diffuse;
//if (feature == SpecularMap)
//   return material.state.texture.specular;
//if (feature == DisplacementMap)
//   return material.state.texture.displacement;
//if (feature == NormalMap)
//   return material.state.texture.normal;
//if (feature == AlphaMap)
//   return material.state.texture.alpha;

//return false;
//}

//void Material::update(void)
//{
//assert(pointer);

//MaterialDescriptor& material = *pointer;

//// Update material parameters with default values
//for(uint32 i=0; i<material.parameters.list.size(); ++i)
//   {
//   MaterialParameter& parameter = material.parameters.list[i];
//   if (parameter.value)
//      parameter.handle.set(parameter.value);
//   }
//
//// Bind textures to samplers
//for(uint32 i=0; i<material.samplers.size(); ++i)
//   material.samplers[i].handle.set(material.samplers[i].texture);
//}

//Mesh::Geometry::Geometry(const gpu::Buffer& vertex) :
//   buffer(vertex),
//   begin(0),
//   end(0)
//{
//}

//Mesh::Elements::Elements(const en::gpu::Buffer& index) :
//   buffer(index),
//   begin(0),
//   end(0)
//{
//}

//Mesh::Mesh(const en::gpu::Buffer& vertex, const en::gpu::Buffer& index) :
//   name(""),
//   material(NULL),
//   type(en::gpu::Triangles),
//   geometry(vertex),
//   elements(index)
//{
//}

//Material::Material() :
//   name("default")
//{
//assert(Gpu.screen.created());
//emmisive.map         = ResourcesContext.defaults.enEmmisiveMap;
//emmisive.color       = float3(1.0f, 1.0f, 1.0f);
//ambient.map          = ResourcesContext.defaults.enAmbientMap;
//ambient.color        = float3(1.0f, 1.0f, 1.0f);
//diffuse.map          = ResourcesContext.defaults.enDiffuseMap;
//diffuse.color        = float3(1.0f, 1.0f, 1.0f);
//specular.map         = ResourcesContext.defaults.enSpecularMap;
//specular.color       = float3(1.0f, 1.0f, 1.0f);
//specular.exponent    = 1.0f;
//transparency.on      = false;
//transparency.map     = ResourcesContext.defaults.enTransparencyMap;
//transparency.color   = float3(1.0f, 1.0f, 1.0f);
//normal.map           = ResourcesContext.defaults.enNormalMap;
//displacement.map     = ResourcesContext.defaults.enDisplacementMap;
//displacement.vectors = ResourcesContext.defaults.enVectorsMap;
//};


Material::Material() :
    name("default")
{
    //assert(Gpu.screen.created());
    albedo       = ResourcesContext.defaults.enAlbedoMap;
    metallic     = ResourcesContext.defaults.enMetallicMap;
    cavity       = ResourcesContext.defaults.enCavityMap;
    roughness    = ResourcesContext.defaults.enRoughnessMap;
    AO           = ResourcesContext.defaults.enAOMap;
    normal       = ResourcesContext.defaults.enNormalMap;
    displacement = ResourcesContext.defaults.enDisplacementMap;
    vector       = ResourcesContext.defaults.enVectorDisplacementMap;
    emmisive     = ResourcesContext.defaults.enEmmisiveMap;
    opacity      = ResourcesContext.defaults.enOpacityMap;
};








Mesh::Mesh()
{
    memset(this, 0, sizeof(Mesh));
}

Mesh& Mesh::operator=(const Mesh& src)
{
    memcpy(this, &src, sizeof(Mesh));
    return *this;
}

/*
InputLayout* Mesh::inputLayout(GpuDevice& gpu)
{
    /// Specialized function for creation of any type of InputLayout.
    return gpu->createInputLayout(
        const DrawableType primitiveType,
        const bool primitiveRestart,
        const uint32 controlPoints,
        const uint32 usedAttributes,
        const uint32 usedBuffers,
        const AttributeDesc * attributes,
        const BufferDesc * buffers) = 0;
}
//*/



/*
// It's easier to recompute below values at runtime each time they are needed, 
// rather than caching them, which would increase total Mesh size. We try to 
// minimize memory loads in exchange of computation.

// Called each time, given LOD becomes resident in GPU dedicated memory
void updateModelLevel(Model& model, const uint32 level)
{
    assert(model.levelCount > level);

    ModelLevel& modelLevel = model.level[level];

    // Patch offsets of input and index buffers in each mesh belonging to this level
    for(uint32 i=0; i<modelLevel.meshRange.count; ++i)
    {
        Mesh& mesh = model.mesh[modelLevel.meshRange.first + i];

        // Update offsets of input buffers, based on current GPU residency
        for(uint32 j=0; j<mesh.bufferCount; ++j)
        {
            mesh.gpuOffset[j] = modelLevel.buffer.gpuOffset + mesh.offset[j];
        }

        // Calculate offset to first Vertex, for draws using only one input buffer
        mesh.firstVertex = mesh.gpuOffset[0] / mesh.elementSize[0];

        // Calculate offset to optional sub-allocated IndexBuffer
        if (mesh.indexCount)
        {
            mesh.firstIndex = (modelLevel.buffer.gpuOffset + mesh.indexOffset) >> mesh.indexShift;
        }
    }
}
//*/




Model::Model(const std::string& _name, const uint32 meshesCount, const uint32 levelsOfDetail, const uint8 gpuIndex)
{
    // TODO: Search by hash if model already exists, if so, load only if it is for GPU its not loaded for
    name = hashString(_name);
    meshCount = meshesCount;
    levelsODetailCount(levelsOfDetail);
    setBit(gpuMask, gpuIndex);
    mesh = new resources::Mesh[meshesCount]; // TODO: Allocate from some global meshes array
    meshRange = nullptr; // Threre is always only one LOD in one OBJ file.
    matrix = nullptr; // OBJ file does not store animation, nor mesh-tree transforms
    padding[0] = 0;
    padding[1] = 0;

    for (uint32 i = 0; i < MaxSupportedGPUCount; ++i)
    {
        backing[i] = nullptr;
    }

    padding2 = 0;
}

CommandState::CommandState(gpu::CommandBuffer& _command) :
    command(_command),
    inputHash(0),
    indexHash(0),
    indexShift(0),
    gpuIndex(0)
{
}

// bufferMask - bitmask identifying which buffers to bind
// TODO: Should this be Renderer method because renderer has broader context that Mesh is missing (like which material to use in case of instanced draw and instanced buffer setup).
//       Most of Mesh internals are used here, but some Renderer state as well.
void Model::drawLOD(
    CommandState& state,
    const uint32 levelOfDetail, 
          uint32 bufferMask,
    const uint32 firstInstance,
    const uint32 instanceCount)
{
    assert(levelCount > levelOfDetail);

    // If there is only one LOD, all meshes belong to it (and LOD array is not needed).
    uint16v2 levelRange(0, meshCount);
    if (meshRange)
    {
        levelRange = meshRange[levelOfDetail];
    }

    // Whoever calls this method, should ensure that model is managed by
    // GPU streamer for GPU for which it is supposed to be drawn
    assert(checkBit(gpuMask, state.gpuIndex));

    if (!backing[state.gpuIndex]) // [[unlikely]]
    {
        // Silently skips drawing if backing buffer is not allocated for selected GPU
        return;
    }

    BufferAllocation& levelBacking = *backing[state.gpuIndex];

    if (!levelBacking.resident)
    {
        // Silently skips drawing if backing buffer is not resident on selected GPU
        return;
    }

    // Single input buffer is used (Position and optional UV)
    if (bufferMask == 1U)
    {
        // Bind first input buffer, only if it wasn't bound yet, it was bound 
        // with offset different than 0 in multi buffer draw, or other one was 
        // previously in use.
        uint64 inputHash = reinterpret_cast<uint64>(levelBacking.gpuBuffer);
        if (state.inputHash != inputHash)
        {
            // TODO: Shouldn't second parameter be 2 if given mesh has UVs?
            state.command.setInputBuffer(0, 1, *(levelBacking.gpuBuffer));
    
            state.inputHash = inputHash;
        }
    }
    else
    {
        // Input buffers will need to be bound for every mesh separately due to 
        // different starting offsets. Single buffer cache still needs to be
        // updated to currently bound backing.
        state.inputHash = reinterpret_cast<uint64>(levelBacking.gpuBuffer);
    }
    
    // Draw each mesh that is part of this model Level Of Detail
    for(uint32 i=0; i<levelRange.count; ++i)
    {
        Mesh& currentMesh = mesh[levelRange.first + i];

        // Will bind only buffers selected for draw, and available in the mesh
        bufferMask &= currentMesh.bufferMask;
    
        // This can happen, but means something is off.. 
        if (bufferMask == 0)
        {
            continue;
        }

        // TODO: Here set pipeline, material, etc. ?


        // If draw uses only one input buffer of a mesh, firstVertex can be used to pass 
        // sub-allocation offset at draw call without a need to rebinding Buffer with
        // different starting offset.
        uint32 firstVertex = 0;

        // Single input buffer is used (Position and optional UV)
        if (bufferMask == 1)
        {
            // Calculate offset to first Vertex, for draws using only first input buffer
            // Shift by 4 is equal to division by 16. First buffer always has elementSize of 16bytes.

//TODO: This is not true. First buffer may or may not have UV attribute. So size is either 12 or 16 bytes.

            firstVertex = (levelBacking.gpuOffset + currentMesh.offset[0]) >> 4;
        }
        else
        {
            // All input buffers are suballocated from the same backing Buffer, thus they
            // have different starting offsets in it (those offsets change each time this
            // mesh becomes resident in GPU dedicated memory).
            uint64 gpuOffset[MaxMeshInputBuffers]; 
          
            // Update offsets of input buffers, based on current GPU residency. 
            uint32 first = 0;
            uint32 slots = 0;
            for(uint32 j=0; j<MaxMeshInputBuffers; ++j)
            {
                // If given buffer is unused, bind previous buffers range with their offsets.
                // Below algorithm minimizes amount of API calls, by grouping used buffers
                // together, and making the call only when really necessary.
                if (!checkBit(bufferMask, j))
                {
                    // This call cannot be cached, as we pass different set of offsets for each mesh.
                    if (slots > 0)
                    {
                        state.command.setInputBuffer(first, slots,
                                                     *(levelBacking.gpuBuffer),
                                                     &gpuOffset[first]);
                    }
    
                    first = j + 1;
                    slots = 0;

                    continue;
                }

                gpuOffset[j] = levelBacking.gpuOffset + currentMesh.offset[j];
                slots++;
            }
    
            // This call cannot be cached, as we pass different set of offsets for each mesh.
            if (slots > 0)
            {
                state.command.setInputBuffer(first, slots,
                                             *(levelBacking.gpuBuffer),
                                             &gpuOffset[first]);
            }
        }
    
        if (currentMesh.indexCount)
        {
            // Bind Buffer, backing model level shared sub-allocation, as one big 
            // Index buffer. This mesh 'firstIndex' contains offset to beginning of 
            // mesh Index Buffer. Do this only when index buffer is not bound yet,
            // other one was bound for previous encoded draw call, or if indexShift 
            // differs between previously drawn mesh, and currently drawn one.
            uint64 indexHash = reinterpret_cast<uint64>(levelBacking.gpuBuffer);
            if (state.indexHash != indexHash ||
                state.indexShift != currentMesh.indexShift)
            {
                state.command.setIndexBuffer(*(levelBacking.gpuBuffer),
                                             currentMesh.indexShift == 1 ? Attribute::u16 : Attribute::u32);
    
                state.indexHash  = indexHash;
                state.indexShift = currentMesh.indexShift;
            }
    
            // Calculate offset to optional sub-allocated IndexBuffer from total offset in backing dedicated buffer.
            uint32 firstIndex = (levelBacking.gpuOffset + currentMesh.indexOffset) >> currentMesh.indexShift;
    
            state.command.drawIndexed(currentMesh.indexCount, // Count of indexes to use for primitve assembly
                                      instanceCount,          // Instance count
                                      firstIndex,             // First index in Index Buffer (index offset)
                                      firstVertex,            // First vertex to use (added to vertex index read, vertex offset)
                                      firstInstance);         // First instance to use (instance offset)
        }
        else
        {
            state.command.draw(currentMesh.vertexCount, // Vertices to draw
                               firstVertex,             // First vertex to use (vertex offset in buffer)
                               instanceCount,           // Instance count
                               firstInstance);          // First instance to use (instance offset)
        }
    }
}









//Model::Model()
//{
//}

//Model::Model(std::shared_ptr<gpu::Buffer> buffer, gpu::DrawableType type) :
//   name("custom")
//{
//Mesh temp;
//temp.geometry.buffer  = buffer;
//temp.elements.type    = type;

//mesh.push_back(temp);
//}

//Model::~Model()
//{
//mesh.clear();
//}


ModelDescriptor::ModelDescriptor() /*:*/
   //mesh(NULL),
   //meshes(0)
{
}

ModelDescriptor::~ModelDescriptor()
{
//if (mesh)
//   {
//   for(uint8 i=0; i<meshes; ++i)
//      delete mesh[i];
//   delete mesh;
//   mesh = NULL;
//   }
}

//Font::Font() :
//    ProxyInterface<class FontDescriptor>(NULL)
//{
//}
//       
//Font::Font(class FontDescriptor* src) :
//    ProxyInterface<class FontDescriptor>(src)
//{
//}

//FontDescriptor::FontDescriptor() :
//   height(0),
//   //material(NULL)
//   texture(nullptr),
//   program(nullptr)
//{
//memset(&table[0], 0, 256*sizeof(FontDescriptor::Character));
//}
//
//FontDescriptor::~FontDescriptor()
//{
//}

//bool FontDestroy(FontDescriptor* const font)
//{
//// Fill font with null pointer
//ResourcesContext.storage.fonts.free(font);
//return true; 
//}




bool ModelDestroy(ModelDescriptor* const model)
{
   // Fill model with null pointer
//   ResourcesContext.storage.models.free(model);
   return true; 
}

//Model::Model(uint32 meshes) :
//   mesh(meshes)
//{
//}



//Model::Model() :
//    ProxyInterface<class ModelDescriptor>(NULL)
//{
//}
//       
//Model::Model(class ModelDescriptor* src) :
//    ProxyInterface<class ModelDescriptor>(src)
//{
//}

//uint8 Model::meshes(void)
//{
// assert(pointer);

// return pointer->meshes;
//}

/*  
gpu::DrawableType Model::drawableType(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);

    return pointer->mesh[mesh]->type;
}

Material& Model::material(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);
    
    return pointer->mesh[mesh]->material;    
}

gpu::Buffer Model::geometry(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);

    return pointer->mesh[mesh]->geometry.buffer;
}

gpu::Buffer Model::elements(uint8 mesh)
{
    assert(pointer);
    assert(pointer->meshes > mesh);

    return pointer->mesh[mesh]->elements.buffer;
}
//*/



std::shared_ptr<Model> Interface::Load::model(const std::string& filename, const std::string& name)
{
    sint64 found;
    uint64 length = filename.length();

    // Try to reuse already loaded models
    if (ResourcesContext.models.find(name) != ResourcesContext.models.end())
    {
        return ResourcesContext.models[name];
    }

    found = filename.rfind(".obj");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return nullptr; // TODO: obj::load(filename, name, streamer);
    }

    found = filename.rfind(".OBJ");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return nullptr; // TODO: obj::load(filename, name, streamer);
    }

    found = filename.rfind(".fbx");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return fbx::load(filename, name);
    }

    found = filename.rfind(".FBX");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return fbx::load(filename, name);
    }

    return std::shared_ptr<Model>(NULL);
}

//Material Interface::Load::material(const std::string& filename, const std::string& name)
//{
//uint32 found;
//uint32 length = filename.length();

//// Try to reuse already loaded material
//if (ResourcesContext.materials.find(name) != ResourcesContext.materials.end())
//   return ResourcesContext.materials[name];

//found = filename.rfind(".mtl");
//if ( found != std::string::npos &&
//     found == (length - 4) )
//   return mtl::load(filename, name);

//found = filename.rfind(".MTL");
//if ( found != std::string::npos &&
//     found == (length - 4) )
//   return mtl::load(filename, name);

//return Material();
//}

//Material Interface::Load::material(const std::string& filename)
//{
//uint32 found;
//uint32 length = filename.length();

//// Try to reuse already loaded material
//if (ResourcesContext.materials.find(filename) != ResourcesContext.materials.end())
//   return ResourcesContext.materials[filename];

//found = filename.rfind(".material");
//if ( found != std::string::npos &&
//     found == (length - 9) )
//   return material::load(filename);

//found = filename.rfind(".MATERIAL");
//if ( found != std::string::npos &&
//     found == (length - 9) )
//   return material::load(filename);

//return Material();
//}

void Interface::Free::model(const std::string& name)
{
    // Find specified model and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::shared_ptr<en::resources::Model> >::iterator it = ResourcesContext.models.find(name);
    if (it != ResourcesContext.models.end())
    {
        if (it->second.unique())
        {
            it->second = nullptr;
        }
    }
}

//void Interface::Free::material(const std::string& name)
//{
//// Find specified material and check if it is used
//// by other part of code. If it isn't it can be
//// safely deleted (assigment operator will perform
//// automatic resource deletion).
//map<string, Material>::iterator it = ResourcesContext.materials.find(name);
//if (it != ResourcesContext.materials.end())
//   if (it->second.references() == 1)
//      it->second = NULL;
//}

std::shared_ptr<audio::Sample> Interface::Load::sound(const std::string& filename)
{
    sint64 found;
    uint64 length = filename.length();

    found = filename.rfind(".wav");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return wav::load(filename);
    }

    found = filename.rfind(".WAV");
    if (found != std::string::npos &&
        found == (length - 4))
    {
        return wav::load(filename);
    }

    return std::shared_ptr<audio::Sample>(NULL);
}

enum FileExtension
{
    ExtensionUnknown = 0,
    ExtensionBMP        ,
    ExtensionDDS        ,
    ExtensionEXR        ,
    ExtensionHDR        ,
    ExtensionPNG        ,
    ExtensionTGA        ,
    ExtensionsCount
 };

static const std::pair<std::string, FileExtension> TranslateFileExtension[] =
{
    { std::string(".bmp"), ExtensionBMP },
    { std::string(".BMP"), ExtensionBMP },
    { std::string(".dds"), ExtensionDDS },
    { std::string(".DDS"), ExtensionDDS },
    { std::string(".exr"), ExtensionEXR },
    { std::string(".EXR"), ExtensionEXR },
    { std::string(".hdr"), ExtensionHDR },
    { std::string(".HDR"), ExtensionHDR },
    { std::string(".png"), ExtensionPNG },
    { std::string(".PNG"), ExtensionPNG },
    { std::string(".tga"), ExtensionTGA },
    { std::string(".TGA"), ExtensionTGA }
};

std::shared_ptr<en::gpu::Texture> Interface::Load::texture(const std::string& filename, const gpu::ColorSpace colorSpace)
{
    uint64 length = filename.length();

#ifdef EN_PROFILE
    Timer timer;
    timer.start();
#endif

// TODO: In future PNG textures should be reversed offline for Release build for performance reasons.
//
//#ifdef EN_DEBUG
    bool invertHorizontal = true;
//#else
//    bool invertHorizontal = false;
//#endif

    std::shared_ptr<gpu::Texture> texture = nullptr;

    // Iterate over all registered file extensions and execute proper loading function
    FileExtension type = ExtensionUnknown;
    uint32 entries = sizeof(TranslateFileExtension) / sizeof(std::pair<std::string, FileExtension>);
    for(uint32 i=0; i<entries; ++i)
    {
        uint64 extensionLength = TranslateFileExtension[i].first.length();
        if (filename.compare(length - extensionLength, extensionLength, TranslateFileExtension[i].first) == 0)
        {
            type = TranslateFileExtension[i].second;
            break;
        }
    }

    // Return if file type is unsupported
    if (type == ExtensionUnknown)
    {
        return texture;
    }

    // Load selected file type
    switch(type)
    {
        case ExtensionBMP:
        {
            //texture = bmp::load(filename);
            break;
        }

        case ExtensionDDS:
        {
            texture = dds::load(filename);
            break;
        }

        case ExtensionEXR:
        {
            texture = exr::load(filename);
            break;
        }
      
        case ExtensionHDR:
        {
            texture = hdr::load(filename);
            break;
        }

        case ExtensionPNG:
        {
            // TODO: This whole abstraction needs to be redone to streamer.
            // texture = png::load(filename, colorSpace, invertHorizontal);
            break;
        }

        case ExtensionTGA:
        {
            // TODO: All that need to be rewritten
            //texture = tga::load(filename);
            break;
        }

        default:
        {
            // How we get here?
            assert( 0 );
            break;
        }
    };

#ifdef EN_PROFILE
    Log << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - " << filename << std::endl;
#endif

    return texture;
}

bool Interface::Load::texture(
    std::shared_ptr<gpu::Texture> dst, 
    const uint16 layer, 
    const std::string& filename, 
    const gpu::ColorSpace colorSpace)
{
    uint64 length = filename.length();

#ifdef EN_PROFILE
    Timer timer;
    timer.start();
#endif

// TODO: In future PNG textures should be reversed offline for Release build for performance reasons.
//
//#ifdef EN_DEBUG
    bool invertHorizontal = true;
//#else
//    bool invertHorizontal = false;
//#endif

    // Iterate over all registered file extensions and execute proper loading function
    FileExtension type = ExtensionUnknown;
    uint32 entries = sizeof(TranslateFileExtension) / sizeof(std::pair<std::string, FileExtension>);
    for(uint32 i=0; i<entries; ++i)
    {
        uint64 extensionLength = TranslateFileExtension[i].first.length();
        if (filename.compare(length - extensionLength, extensionLength, TranslateFileExtension[i].first) == 0)
        {
            type = TranslateFileExtension[i].second;
            break;
        }
    }

    // Return if file type is unsupported
    if (type == ExtensionUnknown)
    {
        return false;
    }

    // Load selected file type
    bool result = false;
    switch(type)
    {
        case ExtensionBMP:
        {
            //result = bmp::load(dst, layer, filename);
            break;
        }

        case ExtensionPNG:
        {
            // TODO: This whole abstraction needs to be redone to streamer.
            //result = png::load(dst, layer, filename, colorSpace, invertHorizontal);
            break;
        }

        case ExtensionTGA:
        {
            // TODO: This whole abstraction needs to be redone to streamer.
            //result = tga::load(dst, layer, filename);
            break;
        }

        default:
        {
            break;
        }
    };

// found = filename.rfind(".bmp");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return bmp::load();
// 
// found = filename.rfind(".BMP");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return bmp::load(dst, layer, filename);
// 
// found = filename.rfind(".png");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return png::load(dst, layer, filename, colorSpace, invertHorizontal);
// 
// found = filename.rfind(".PNG");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return png::load(dst, layer, filename, colorSpace, invertHorizontal);
// 
// found = filename.rfind(".tga");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return tga::load(dst, layer, filename);
// 
// found = filename.rfind(".TGA");
// if ( found != std::string::npos &&
//      found == (length - 4) )
//    return tga::load(dst, layer, filename);


#ifdef EN_PROFILE
    Log << "Profiler: Resource load time: " << std::setw(6) << timer.elapsed().miliseconds() << std::setw(1) << "ms - " << filename << std::endl;
#endif

    return result;
}

//   bool Interface::Load::texture(std::shared_ptr<gpu::Texture> dst, const gpu::TextureFace face, const uint16 layer, const std::string& filename, const gpu::ColorSpace colorSpace)
//   {
//  uint32 found;
//   uint32 length = filename.length();
//
//// TODO: In future PNG textures should be reversed offline for Release build for performance reasons.
////
////#ifdef EN_DEBUG
//   bool invertHorizontal = true;
////#else
////   bool invertHorizontal = false;
////#endif
//
//   found = filename.rfind(".png");
//   if ( found != std::string::npos &&
//        found == (length - 4) )
//      return png::load(dst, face, layer, filename, colorSpace, invertHorizontal);
//
//   found = filename.rfind(".PNG");
//   if ( found != std::string::npos &&
//        found == (length - 4) )
//      return png::load(dst, face, layer, filename, colorSpace, invertHorizontal);
//
//   return false;
//   }

void Interface::Free::sound(const std::string& filename)
{
    // Find specified sample and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::shared_ptr<audio::Sample> >::iterator it = ResourcesContext.sounds.find(filename);
    if (it != ResourcesContext.sounds.end())
    {
        if (it->second.unique())
        {
            it->second = nullptr;
        }
    }
}

void Interface::Free::texture(const std::string& filename)
{
    // Find specified texture and check if it is used
    // by other part of code. If it isn't it can be
    // safely deleted (assigment operator will perform
    // automatic resource deletion).
    std::map<std::string, std::unique_ptr<gpu::Texture> >::iterator it = ResourcesContext.textures.find(filename);

    // TODO: This whole abstraction needs to be redone to streamer.
    //if (it != ResourcesContext.textures.end())
    //   if (it->second.unique())
    //      it->second = NULL;
}

} // en::resource

resource::Context   ResourcesContext;
resource::Interface Resources;

} // en

//template<> bool (*ProxyInterface<en::resources::FontDescriptor>::destroy)(en::resources::FontDescriptor* const)         = en::resources::FontDestroy;
//template<> bool (*ProxyInterface<en::resources::MaterialDescriptor>::destroy)(en::resources::MaterialDescriptor* const) = en::resources::MaterialDestroy;
template<> bool (*ProxyInterface<en::resources::ModelDescriptor>::destroy)(en::resources::ModelDescriptor* const)       = en::resources::ModelDestroy;
