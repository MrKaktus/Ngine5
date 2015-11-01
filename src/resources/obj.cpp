/*

 Ngine v5.0
 
 Module      : OBJ files support
 Visibility  : Engine internal code
 Requirements: none
 Description : Supports reading models from *.obj files.

*/

#include "core/storage.h"
#include "core/log/log.h"
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

   struct Vertex
          {
          uint32 position; // Position
          uint32 uv;       // Texture Coordinate
          uint32 normal;   // Normal vector

          bool operator ==(const Vertex& b);
          };

   struct Face
          {
          Vertex vertex[3]; // Vertex
          uint32 material;  // Material
          };

   //struct Material
   //       {
   //       string                 name;   // Material name
   //       en::resource::Material handle; // Material handle
   //       };

   struct Mesh
          {
          string         name;      // Mesh name
          string         material;  // Material name (only one per group allowed)
          vector<Vertex> vertices;  // Array of vertices composition data
          vector<uint32> indexes;   // Array of indexes
          vector<uint32> optimized; // Array of indexes after Forsyth optimization
          bool           coords;    // Does mesh contain texture coordinates 
          bool           coordW;    // Does mesh uses 3 component texture coordinates
          bool           normals;   // Does mesh contain normal vectors
          };

   bool Vertex::operator ==(const Vertex& b)
   {
   return !memcmp(this, &b, 12); // 12 = sizeof(obj::Vertex)
   }

   Vertex parseVertex(string& word)
   {
   Vertex vertex;
   size_t s1 = word.find('/');
   size_t s2 = word.find('/',s1+1);
   
   // There is always vertex id
   if (s1 != string::npos)
      vertex.position = stringTo<uint32>(word.substr(0,s1));
   else
      vertex.position = stringTo<uint32>(word);
   
   // First slash needs to be present, and there need to be 
   // number just after it, otherwise there is no Tex Coord
   vertex.uv = 0;
   // (case: v/t)
   if ( (s1 != string::npos) &&
        (s2 == string::npos) &&
        ((s1+1) < word.length()) )
      vertex.uv = stringTo<uint32>(word.substr(s1+1));
   // (case: v/t/n)
   if ( (s1 != string::npos) &&
        (s2 != string::npos) &&
        ((s1+1) < s2) )
      vertex.uv = stringTo<uint32>(word.substr(s1+1,s2-s1-1));
   
   // If there is no second slash there is no Normal
   // (cases: v and v/t)
   vertex.normal = 0;
   if (s2 != string::npos)
      vertex.normal = stringTo<uint32>(word.substr(s2+1));

   return vertex;
   }

   // Creates scene model
   // - support for multile MTL files
   // - automatic polygon tesselation using triangle fan
   // - reduces identical vertices inside mesh
   // - reduces index buffer size
   // - optimizes indices order
   Ptr<en::resource::Model> load(const string& filename, const string& name)
   {
   using namespace en::storage;

   // Try to reuse already loaded models
   if (ResourcesContext.models.find(name) != ResourcesContext.models.end())
      return ResourcesContext.models[name];
        
   // Open model file 
   Nfile* file = NULL;
   if (!Storage.open(filename, &file))
      if (!Storage.open(ResourcesContext.path.models + filename, &file))
         {
         Log << en::ResourcesContext.path.models + filename << endl;
         Log << "ERROR: There is no such file!\n";
         return Ptr<en::resource::Model>(NULL);
         }
   
   // Allocate temporary buffer for file content ( 4GB max size! )
   uint64 size = file->size();
   uint8* buffer = NULL;
   buffer = new uint8[static_cast<uint32>(size)];
   if (!buffer)
      {
      Log << "ERROR: Not enough memory!\n";
      return Ptr<en::resource::Model>(NULL);
      }
   
   // Read file to buffer and close file
   if (!file->read(buffer))
      {
      Log << "ERROR: Cannot read whole obj file!\n";
      return Ptr<en::resource::Model>(NULL);
      }    
   delete file;
   

   // Step 1 - Parsing the file
   

   // Create parser to quickly process text from file
   Nparser text(buffer, size);
   
   // Temporary data
   vector<float3> vertices;             // Vertices
   vector<float3> normals;              // Normals
   vector<float3> coordinates;          // Texture coordinates
   vector<string> libraries;            // Material libraries
   vector<en::obj::Mesh> meshes;        // Meshes
   vector<en::resource::Material> materials; // Materials

   // Reserve memory for incoming data, to minimise
   // occurences of possible relocations during 
   // vectors growth
   vertices.reserve(16384);
   normals.reserve(16384);
   coordinates.reserve(16384);
   meshes.reserve(16);
   materials.reserve(32);

   // Model always has minimum one mesh
   meshes.push_back(en::obj::Mesh());

   // Current mesh and material handles
   en::obj::Mesh* mesh = &meshes[meshes.size() - 1];
   string material = "default";

   // Fill first mesh descriptor with default data
   mesh->name     = "default";
   mesh->material = material;
   mesh->vertices.reserve(8192);
   mesh->indexes.reserve(8192);
   mesh->optimized.reserve(8192);
   mesh->coords   = false;
   mesh->coordW   = false;
   mesh->normals  = false;

   // Collects geometry primitives data that will
   // be used to generate final meshes and perform
   // primitives assembly into unoptimized meshes
   string command;
   string word;
   bool eol = false;
   while(!text.end())
        {
        // Read commands until it is end of file
        if (!text.read(command, eol))
           continue;
   
        // Handle vertices
        if (command == "v")
           {
           float3 vertex(0.0f, 0.0f, 0.0f);
           if (!eol && text.read(word, eol))
              vertex.x = stringTo<float>(word);
           if (!eol && text.read(word, eol))
              vertex.y = stringTo<float>(word);
           if (!eol && text.read(word, eol))
              vertex.z = stringTo<float>(word);
           vertices.push_back(vertex);
           }
        else
        // Handle normals
        if (command == "vn")
           {
           float3 normal(0.0f, 0.0f, 0.0f);
           if (!eol && text.read(word, eol))
              normal.x = stringTo<float>(word);
           if (!eol && text.read(word, eol))
              normal.y = stringTo<float>(word);
           if (!eol && text.read(word, eol))
              normal.z = stringTo<float>(word);
           normals.push_back(normal);
           }
        else
        // Handle texture coordinates
        if (command == "vt")
           {
           float3 coord(0.0f, 0.0f, 0.0f);
           if (!eol && text.read(word, eol))
              coord.u = stringTo<float>(word);
           if (!eol && text.read(word, eol))
              coord.v = stringTo<float>(word);
           if (!eol && text.read(word, eol))
              {
              coord.w = stringTo<float>(word);
              if (mesh->coordW == false)
                 if (coord.w != 0.0)
                    mesh->coordW = true;
              }
           coordinates.push_back(coord);
           }
        else
        // Create next mesh description
        if (command == "g")
           {
           // If model specifies mesh before any face
           // occurence, default mesh is not needed and
           // it can be reused.
           if (mesh->indexes.size() == 0)
              {
              if (!eol && text.read(word, eol))
                 mesh->name = word;
              }
           else
              {
              // Push new mesh description on the stack
              meshes.push_back(en::obj::Mesh());
              mesh = &meshes[meshes.size() - 1];

              // Fill mesh descriptor with default data
              if (!eol && text.read(word, eol))
                 mesh->name  = word;
              mesh->material = material;
              mesh->vertices.reserve(8192);
              mesh->indexes.reserve(8192);
              mesh->optimized.reserve(8192);
              mesh->coords   = false;
              mesh->coordW   = false;
              mesh->normals  = false;
              }
           }
        else
        // Add materials library to the list
        if (command == "mtllib")
           {
           if (!eol && text.read(word, eol))
              {
              // Prevent from adding the same library more than once
              bool found = false;
              for (uint32 i=0; i<libraries.size(); ++i)
                  if (libraries[i] == word)
                     {
                     found = true;
                     break;
                     }
   
              // Add new library to the list
              if (!found)
                 libraries.push_back(word);
              }
           }
        else
        // Set current material
        if (command == "usemtl")
           {
           if (!eol && text.read(word, eol))
              {
              material = word;
   
              // Prevent from adding the same material more than once
              bool found = false;
              for (uint32 i=0; i<materials.size(); ++i)
                  if (materials[i].name == word)
                     {
                     found = true;
                     break;
                     }
   
              // Add new material to the list
              if (!found)
                 {
                 //obj::Material material;
                 en::resource::Material material;
                 material.name   = word;
                 //material.handle = NULL;
                 materials.push_back(material);
                 }
   
              // Mesh should specify material before any 
              // face will occur or use previous material
              // for whole mesh.
              if (mesh->indexes.size() == 0)
                 mesh->material = material;
              }
           }
        else
        // Primitive Assembly
        if (command == "f")
           {
           uint32 counter = 0;
           while(!eol)
                {
                if (text.read(word, eol))
                   {
                   en::obj::Vertex vertex = en::obj::parseVertex(word);
                   
                   // Check presence of texture coordinates
                   if (mesh->coords == false)
                      if (vertex.uv) 
                         mesh->coords = true;

                   // Check presence of normals
                   if (mesh->normals == false)
                      if (vertex.normal) 
                         mesh->normals = true;
                   
                   // First triangle is generated from first three
                   // vertices of the face. If there is more of them,
                   // face is divided into triangles on a basis of
                   // triangle fan.
                   if (counter == 3)
                      {
                      uint32 size   = mesh->indexes.size();
                      uint32 indexA = mesh->indexes[size-3];
                      uint32 indexB = mesh->indexes[size-1];
                      mesh->indexes.push_back(indexA);
                      mesh->indexes.push_back(indexB);
                      counter = 2;
                      }
   
                   // Try to reuse vertex if it already exist
                   bool found = false;
                   for(uint32 i=0; i<mesh->vertices.size(); ++i)
                      if (mesh->vertices[i] == vertex)
                         {
                         mesh->indexes.push_back(i);
                         found = true;
                         break;
                         }
   
                   // If vertex is new, add it to vertex array
                   if (!found)
                      {
                      mesh->indexes.push_back(mesh->vertices.size());
                      mesh->vertices.push_back(vertex);
                      }
   
                   counter++;
                   }
                }
           }
   
        // Skip not relevant part of the line
        text.skipToNextLine();       
        }
 
 
   // Step 2 - Generating final model


   // Load materials used by model
   for(uint8 i=0; i<materials.size(); ++i)
      {
      bool loaded = false;

      // Search for material in used MTL files and load it
      for(uint8 j=0; j<libraries.size(); ++j)
         {
         loaded = mtl::load(libraries[j], materials[i].name, materials[i]);
         if (loaded)
            break;
         }

      // Check if material was found and properly loaded
      if (!loaded)
         Log << "ERROR! Cannot find material: " << materials[i].name.c_str() << " !" << endl;
      }  

   //// Create model
   //en::resource::ModelDescriptor* model = ResourcesContext.storage.models.allocate();
   //if (model == NULL)
   //   {
   //   Log << "ERROR: Models pool is full!\n";
   //   return Ptr<en::resource::Model>(NULL);
   //   } 
   //model->mesh   = new en::resource::Mesh*[meshes.size()];
   //model->meshes = meshes.size();
    
   Ptr<en::resource::Model> model = new en::resource::Model();
   assert(model);

   // Generate meshes
   for(uint8 i=0; i<meshes.size(); ++i)
      {
      using namespace en::resource;

      // Search source mesh and material
      en::obj::Mesh& srcMesh = meshes[i];
      en::resource::Material srcMaterial;

      for(uint8 j=0; j<materials.size(); ++j)
         if (srcMesh.material == materials[j].name)
            srcMaterial = materials[j];

      uint32 vertexes = srcMesh.vertices.size();
      uint32 indexes  = srcMesh.indexes.size();
      
      // Create geometry buffer
      en::gpu::BufferSettings settings;
      settings.type      = gpu::VertexBuffer;
      settings.vertices  = vertexes;
      uint32 columns     = 1;

      // Position
      settings.column[0].type = gpu::Float3; 
      settings.column[0].name = "inPosition";

      uint32 rowSize = 12;
   
      // Normals
      if (srcMesh.normals)
         {
         rowSize += 12;
         settings.column[columns].type = gpu::Float3;
         settings.column[columns].name = "inNormal";
         columns++;
         }

      // Bitangents
      //if (srcMaterial.normal.map ||
      //    srcMaterial.displacement.map)
      if (srcMaterial.normal ||
          srcMaterial.displacement)
         {
         rowSize += 12;
         settings.column[columns].type = gpu::Float3;
         settings.column[columns].name = "inBitangent";
         columns++;
         }
           
      // Texture Coordinates
      if (srcMesh.coords)
         {
         rowSize += 8;
         settings.column[columns].type = gpu::Float2;
         settings.column[columns].name = "inTexCoord0";
         if (srcMesh.coordW)
            {
            rowSize += 4;
            settings.column[columns].type = gpu::Float3;
            }
         columns++;
         }
   
      // Calculate tangent space vectors
      float3* tangents   = NULL;
      float3* bitangents = NULL;   
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

            const float3& v0 = vertices[ v0ids.position ];
            const float3& v1 = vertices[ v1ids.position ];
            const float3& v2 = vertices[ v2ids.position ];

            const float3& c0 = coordinates[ v0ids.uv ];
            const float3& c1 = coordinates[ v1ids.uv ];
            const float3& c2 = coordinates[ v2ids.uv ];

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
            float3 t = normalize( tangents[j] );               // T - normalized tangent
            float3 b = normalize( bitangents[j] );             // B - normalized bitangent
            float3 n = normalize( normals[ v0ids.normal ] );   // N - normalized normal

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
         *((float3*)(geometry + offset))    = vertices[vertex.position - 1];
         offset += 12;
         // Normals
         if (srcMesh.normals)
            {
            *((float3*)(geometry + offset)) = normals[vertex.normal - 1];
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
         if (srcMesh.coords)
            {
            *((float*)(geometry + offset))  = coordinates[vertex.uv - 1].u;
            offset += 4;
            *((float*)(geometry + offset))  = coordinates[vertex.uv - 1].v;
            offset += 4;
            if (srcMesh.coordW)
               {
               *((float*)(geometry + offset)) = coordinates[vertex.uv - 1].w;
               offset += 4;
               }
            }
         }
      gpu::Buffer vertexBuffer = Gpu.buffer.create(settings, geometry);
      delete [] geometry;
      delete [] tangents;
      delete [] bitangents;

      // Create indices buffer
      settings.type      = gpu::IndexBuffer;
      settings.elements  = indexes;
      settings.column[0] = gpu::UInt;
      for(uint8 j=1; j<16; ++j)
         settings.column[j] = gpu::None;
   
      // Optimize indexes order for Post-Transform Vertex Cache Size
      void* srcIndex = &srcMesh.indexes[0];
      if (en::obj::optimizeIndexOrder)
         {
         srcMesh.optimized.resize(indexes, 0);
         Forsyth::optimize(srcMesh.indexes, srcMesh.optimized, vertexes);
         srcIndex = &srcMesh.optimized[0];
         }
   
      // Optimize size of Index Buffer
      bool compressed = false;
      if (en::obj::optimizeIndexSize)
         {
         if (vertexes < 255)
            {
            settings.column[0] = gpu::UByte;
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
            settings.column[0] = gpu::UShort;
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
      gpu::Buffer indexBuffer = Gpu.buffer.create(settings, srcIndex);
      if (compressed)
         delete [] static_cast<uint8*>(srcIndex);
   
      // Create mesh
      en::resource::Mesh mesh;
      mesh.name              = srcMesh.name;
      mesh.material          = srcMaterial;
      mesh.geometry.buffer   = vertexBuffer;
      mesh.geometry.begin    = 0;
      mesh.geometry.end      = vertexes;
      mesh.elements.type     = gpu::Triangles;
      mesh.elements.buffer   = indexBuffer;
      mesh.elements.offset   = 0;
      mesh.elements.indexes  = indexes;

      model->mesh.push_back(mesh);
      }
    
   // Update list of loaded models
   ResourcesContext.models.insert(pair<string, Ptr<en::resource::Model> >(name, model));
 
   // Return model interface
   return model;
   }

   }
}
