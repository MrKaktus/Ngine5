/*

 Ngine v5.0
 
 Module      : Basic Allocator.
 Requirements: none
 Description : Provides logic for blocks allocation 
               in given address space.

*/

#include "core/utilities/basicAllocator.h"

namespace en
{   
   BasicAllocator::BasicAllocator(uint64 _size) :
      size(_size),
      available(_size),
      freeHead(new MemoryChunk())
   {
   freeHead->offset = 0u;
   freeHead->size   = size;
   freeHead->prev   = nullptr;
   freeHead->next   = nullptr;
   }

   void BasicAllocator::insertBefore(MemoryChunk* ptr, uint64 offset, uint64 size)
   {
   if (!ptr)
      return;
      
   MemoryChunk* insert = new MemoryChunk();
   insert->offset = offset;
   insert->size   = size;
   insert->prev   = ptr->prev;
   insert->next   = ptr;
      
   if (ptr->prev)
      ptr->prev->next = insert;
   ptr->prev = insert;
   }
   
   void BasicAllocator::insertAfter(MemoryChunk* ptr, uint64 offset, uint64 size)
   {
   if (!ptr)
      return;
      
   MemoryChunk* insert = new MemoryChunk();
   insert->offset = offset;
   insert->size   = size;
   insert->prev   = ptr;
   insert->next   = ptr->next;
      
   if (ptr->next)
      ptr->next->prev = insert;
   ptr->next = insert;
   }

   void BasicAllocator::remove(MemoryChunk* ptr)
   {
   if (ptr->next)
      ptr->next->prev = ptr->prev;
         
   if (ptr->prev)
      ptr->prev->next = ptr->next;
      
   delete ptr;
   }
   
   bool BasicAllocator::allocate(const uint64 requestedSize, const uint64 requestedAlignment, uint64& offset)
   {
   if (requestedSize > available)
      return false;

   // Iterate over list of free memory regions
   MemoryChunk* ptr = freeHead;
   while(ptr)
      {
      if (ptr->size >= requestedSize)
         {
         uint64 alignedOffset = roundUp(ptr->offset, requestedAlignment);
         uint64 endOffset     = ptr->offset + ptr->size;
         uint64 newEndOffset  = alignedOffset + requestedSize;
         if (newEndOffset <= endOffset)
            {
            // Allocation can be performed, split free block
            uint64 baseOffset = ptr->offset;
            uint64 baseSize   = ptr->size;
            
            // New empty block in front of allocation
            bool reusedEntry = false;
            if (alignedOffset > baseOffset)
               {
               ptr->size = alignedOffset - baseOffset;
               reusedEntry = true;
               }
            
            // New empty block at end of allocation
            if (newEndOffset < endOffset)
               {
               if (reusedEntry)
                  insertAfter(ptr, newEndOffset, endOffset - newEndOffset);
               else
                  {
                  ptr->offset = newEndOffset;
                  ptr->size   = alignedOffset - baseOffset;
                  reusedEntry = true;
                  }
               }
            
            // If allocation covers whole empty block, remove it from the list
            if (!reusedEntry)
               remove(ptr);
              
            available -= requestedSize;
            return true;
            }
         }
         
      ptr = ptr->next;
      }
   
   return false;
   }
   
   bool BasicAllocator::deallocate(const uint64 offset, const uint64 freedSize)
   {
   uint64 newEndOffset = offset + freedSize;
   
   if (newEndOffset > size)
      return false;
      
   if (freeHead == nullptr)
      {
      freeHead = new MemoryChunk();
      freeHead->offset = offset;
      freeHead->size   = freedSize;
      freeHead->prev   = nullptr;
      freeHead->next   = nullptr;
      
      available += freedSize;
      return true;
      }
      
   // Iterate over list of free memory regions
   // (add new free memory region, grow or merge two)
   MemoryChunk* ptr = freeHead;
   while(ptr)
      {
      uint64 endOffset = ptr->offset + ptr->size;

      if (ptr->offset > offset)
         {
         // Make sure those blocks don't overlapp
         assert( ptr->offset >= newEndOffset );

         // Touching next block.
         if (ptr->offset == newEndOffset)
            {
            ptr->offset = offset;
            ptr->size  += freedSize;
            
            MemoryChunk* prev = ptr->prev;
            if (prev)
               {
               // Touching previous block, and next block.
               if (prev->offset + prev->size == offset)
                  {
                  prev->size += ptr->size;
                  remove(ptr);
                  }
               }
            
            available += freedSize;
            return true;
            }
         else // In front of this block, but not touching it.
            {
            MemoryChunk* prev = ptr->prev;
            if (prev)
               {
               // Touching previous block.
               if (prev->offset + prev->size == offset)
                  {
                  prev->size += freedSize;
                  
                  available += freedSize;
                  return true;
                  }
               }
            
            // Not touching any block.
            insertBefore(ptr, offset, freedSize);
            available += freedSize;
            return true;
            }
         }
      else // Make sure those blocks don't overlapp
         assert( ptr->offset + ptr->size <= offset);
      }
   
   // If there is completly no empty space, list of blocks is empty.
   return false;
   }
   
   BasicAllocator::~BasicAllocator()
   {
   MemoryChunk* ptr = freeHead;
   
   // Free all elements on the linked list
   while(ptr)
      {
      MemoryChunk* node = ptr->next;
      remove(ptr);
      ptr = node;
      }
   }
}