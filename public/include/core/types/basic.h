/*

 Ngine v5.0
 
 Module      : Types.
 Requirements: none
 Description : Defines all basic types used by Ngine.
 
*/

typedef signed   char          sint8;
typedef unsigned char          uint8;
typedef signed   short         sint16;
typedef unsigned short         uint16;
typedef signed   int           sint32;
typedef unsigned int           uint32;
typedef signed   long long int sint64;
typedef unsigned long long int uint64;

static_assert(sizeof(sint8)  == 1, "en::sint8 size mismatch!");
static_assert(sizeof(uint8)  == 1, "en::uint8 size mismatch!");
static_assert(sizeof(sint16) == 2, "en::sint16 size mismatch!");
static_assert(sizeof(uint16) == 2, "en::uint16 size mismatch!");
static_assert(sizeof(sint32) == 4, "en::sint32 size mismatch!");
static_assert(sizeof(uint32) == 4, "en::uint32 size mismatch!");
static_assert(sizeof(sint64) == 8, "en::sint64 size mismatch!");
static_assert(sizeof(uint64) == 8, "en::uint64 size mismatch!");

#define KB 1024
#define MB 1024*1024
#define GB 1024*1024*1024
