/*

 Ngine v5.0

 Module      : Assets handle.
 Requirements: none
 Description : Public interface.

*/

#ifndef ENG_ASSETS_HANDLE
#define ENG_ASSETS_HANDLE

#include "core/defines.h"
#include "core/types.h"

namespace en
{
namespace assets
{

enum class AssetType : uint16
{
    Buffer               = 0,
    Texture                 ,
  //Material                ,
  //Model                   ,
  //SoundSample             ,
    Count                   ,

    Invalid         = 0xFFFF,
};

// Runtime handle of Asset object (used by renderer, etc.)
union AssetHandle
{
    struct
    {
        uint32 index;
        uint16 generation;
        AssetType type;
    };

    uint64 value;

    // By default inits handle as invalid
    AssetHandle();
};

static_assert(sizeof(AssetHandle) == 8, "en::assets::AssetHandle size mismatch!");

//constexpr AssetHandle InvalidAssetHandle = (AssetHandle)(0xFFFFFFFFFFFFFFFF);
#define InvalidAssetHandle AssetHandle();

} // en::assets
} // en

#endif
