/*

 Ngine v5.0
 
 Module      : Forsyth algorithm
 Visibility  : Engine internal code
 Requirements: none
 Description : Contains set of functions that optimize
               3D model meshes for faster rendering by
               graphic card.

*/

#include <math.h>

#include "core/rendering/context.h"
#include "resources/forsyth.h"

namespace en
{
   namespace Forsyth
   {
   // TODO: It should be possible to change algorithm 
   //       constants through config file to allow 
   //       automatic search for optimal values without
   //       code recompilation.
   const float CacheDecayPower   = 1.50f;
   const float LastTriScore      = 0.75f;
   const float ValenceBoostScale = 2.0f;
   const float ValenceBoostPower = 0.5f;

   // Precomputed scores
   float precomputedCacheScore[32];   // GpuContext.support.postTransformVertexCacheSize
   float precomputedValenceScore[64];

   struct Vertex
          {
          float  score;       // Vertex score 
          uint32 triangles;   // Offset in array, to the beginning of list 
                              // with triangle indexes used by this vertex
          uint16 count;       // Count of triangles that uses this vertex
          uint16 uses;        // Count of triangles that still need to use this vertex
          uint8  position;    // Position in simulated post transform vertex cache (255 outside)
          uint8  oldPosition; // Position in previous state of simulated cache 

          Vertex();
          };

   struct Triangle
          {
          float  score;       // Triangle score (sum of vertexes scores)
          uint16 vertex[3];   // Indexes to triangle vertexes
          bool   added;       // Is it added to the indices list already?
          };

   Vertex::Vertex() :
      score(0.0f),
      triangles(0),
      count(0),
      uses(0),
      position(255),
      oldPosition(255)
   {
   }

   // Should be used once to precompute scores for
   // given post transform vertex cache size.
   float VertexCacheScore(uint8 position, uint8 size)
   {
   // First three positions are occupied by vertexes
   // that were used in the last triangle, so all of
   // them have the same score because order in which
   // they compose triangle is irrelevant.
   if (position < 3)
      return LastTriScore;
   
   // Be sure that proper values were passed
   assert(position < size);
   
   // Higher position in cache equals to better score
   float score = 1.0f - static_cast<float>((position - 3) / (size - 3));
   return powf(score, CacheDecayPower);
   }

   float VertexValenceScore(uint16 faces)
   {
   // Vertexes that are no more needed have negative score.
   if (faces == 0)
      return -1.0f;
   
   // Current valence score depends on number of triangles, 
   // that still need this vertex. Vertexes that are needed
   // by small amount of triangles are favored and receive
   // higher scores. This allows to faster get rid of rarely
   // used vertexes.
   float valenceBoost = powf(static_cast<float>(faces), -ValenceBoostPower);
   return ValenceBoostScale * valenceBoost;
   }

   float VertexScore(uint16 faces, uint8 position)
   {
   // Check if vertex is still needed
   if (faces == 0)
      return -1.0f;
   
   float score = 0.0f;
   if (position < GpuContext.support.postTransformVertexCacheSize)
      score += precomputedCacheScore[position];
   
   if (faces < 64)
      score += precomputedValenceScore[faces];
   else
      score += VertexValenceScore(faces);
   
   return score;
   }

   void optimize(const vector<uint32>& input, vector<uint32>& output, uint32 vertexes)
   {
   for(uint8 i=0; i<GpuContext.support.postTransformVertexCacheSize; ++i)
      precomputedCacheScore[i] = VertexCacheScore(i, GpuContext.support.postTransformVertexCacheSize);
   
   for(uint16 i=0; i<64; ++i)
      precomputedValenceScore[i] = VertexValenceScore(i);
   
   
   vector<Vertex> vertices;  // Array of vertex description structures
   vector<uint32> triangles; // Set of Arrays per vertex with indices of triangles that use it
   
   vertices.resize(vertexes);
   
   // Compute faces count per vertex
   for(uint32 i=0; i<input.size(); ++i)
      {
      uint32 index = input[i];
      assert(index < vertexes);
      vertices[index].count++;
      }
   
   // Create arrays of triangle indices per vertex      
   uint32 offset = 0;
   for(uint32 i=0; i<vertexes; ++i)
      {
      Vertex& vertex   = vertices[i];
      vertex.score     = VertexScore(vertex.count, vertex.position);
      vertex.triangles = offset;
      offset += vertex.count;
      vertex.count     = 0;
      vertex.uses      = 0;
      }
   triangles.resize(offset);
   
   // Fill that array with data
   for(uint32 i=0; i<input.size(); i+=3)
      for(uint32 j=0; j<3; ++j)
         {
         Vertex& vertex = vertices[input[i+j]];
         triangles[vertex.triangles + vertex.count] = i;
         vertex.count++;
         }
   
   // To this point all that could be done during read of OBJ file
   
   vector<bool> processed; // Array of markers if given triangle was already processed
   processed.resize(input.size());
   
   // Simulated LRU Post-Transform Vertex Cache
   uint32 cache[32]; // GpuContext.support.postTransformVertexCacheSize
   uint8  cacheEntries = 0;
   
   // Previous state of simulated cache
   uint32 oldCache[32]; //GpuContext.support.postTransformVertexCacheSize
   uint8  oldCacheEntries = 0;
   
   // Clear caches 
   memset(cache, 0, GpuContext.support.postTransformVertexCacheSize * sizeof(uint32));
   memset(oldCache, 0, GpuContext.support.postTransformVertexCacheSize * sizeof(uint32));
   
   uint32 bestFace = 0;
   float bestScore = -1.0f;
   
   const float maxValenceScore = VertexScore(1, 255) * 3.0f;  // ?
   
   for(uint32 i=0; i<input.size(); i+=3)
      {
      if (bestScore < 0.0f)
         // Choose triangle with highest score as a new starting point,
         // because vertices in the cache won't be used any more, and 
         // are not needed by unprocessed triangles that left
         for(uint32 face=0; face<input.size(); face+=3)
            if (!processed[face])
               {
               // Calculate triangle score
               float score = 0.0f;
               for(uint32 v=0; v<3; ++v)
                  score += vertices[input[face + v]].score;
   
               if (score > bestScore)
                  {
                  bestScore = score;
                  bestFace  = face;
   
                  assert(bestScore <= maxValenceScore);  // ?
                  if (bestScore >= maxValenceScore)      // ?
                     break;                              // ?
                  }
               }
      processed[bestFace] = true;
   
      // Add triangle vertices to cache and seve them in output list
      for(uint32 v=0; v<3; ++v)
         {
         uint32 index = input[bestFace + v];
         output[i + v] = index;
   
         Vertex& vertex = vertices[index];
   
         // If vertex is not already in updated cache, add it 
         if (vertex.position >= cacheEntries)
            {
            vertex.position = cacheEntries;
            cache[cacheEntries++] = index;
      
            // If this was last triangle that was using that vertex
            // there is no need to reorder it's triangles list.
            if (vertex.count == 1)
               {
               --vertex.count;
               continue;
               }
            }
    
         // Moves index of currently processed triangle to the end of the triangles 
         // list for specified vertex, and decreases count of that list. In result
         // vertex has information only about triangles that haven't been processed
         // yet.
         assert(vertex.count > 0);
         for (uint32 j=0; j<vertex.count; ++j)
             if (triangles[vertex.triangles + j] == bestFace)
                {
                uint32 end = vertex.triangles + vertex.count - 1;
                triangles[vertex.triangles + j] = triangles[end];
                triangles[end] = bestFace;
                break;
                }
         --vertex.count;
   
         // Calculate new vertex score
         vertex.score = VertexScore(vertex.count, vertex.position);
         }
   
      // Vertices of currently processed triangle are moved to the
      // beginning of the cache to simulate LRU policy. If new 
      // vertices were used, last recently used ones will fall out
      // of cache.
      for(uint8 pos=0; pos<oldCacheEntries; ++pos)
         {
         Vertex& vertex = vertices[oldCache[pos]];
   
         // If vertex is not already in cache, add it 
         // and recalculate its score.
         if (vertex.position >= cacheEntries)
            {
            vertex.position = cacheEntries;
            cache[cacheEntries++] = oldCache[pos];
            vertex.score = VertexScore(vertex.count, vertex.position);
            }
   
         if (cacheEntries == GpuContext.support.postTransformVertexCacheSize)
            break;
         }
   
      // find the best scoring triangle in the current cache (including up to 3 that were just evicted)
      bestScore = -1.0f;
      for(uint32 pos=0; pos<cacheEntries; ++pos)
         {
         Vertex& vertex = vertices[cache[pos]];
   
         // Save current vertice cache state as previous one
         vertex.oldPosition = vertex.position;
         vertex.position    = 255;
   
         // From all triangles that still need to use current
         // vertices in cache, find the best triangle to process
         for(uint32 j=0; j<vertex.count; ++j)
            {
            uint32 face = triangles[vertex.triangles + j];
   
            // Calculate triangle score
            float score = 0.0f;
            for(uint32 v=0; v<3; ++v)
               score += vertices[input[face + v]].score;
           
            if (score > bestScore)
               {
               bestScore = score;
               bestFace  = face;
               }
            }
         }
        
      // Save current cache state as previous one
      memcpy(oldCache, cache, GpuContext.support.postTransformVertexCacheSize * sizeof(uint32));
      oldCacheEntries = min(cacheEntries, (uint8)GpuContext.support.postTransformVertexCacheSize);
   
      // Clear current cache state
      memset(cache, 0, GpuContext.support.postTransformVertexCacheSize * sizeof(uint32));
      cacheEntries = 0;
      }
   }

   }
}