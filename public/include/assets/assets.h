/*

 Ngine v5.0

 Module      : Assets manager.
 Requirements: none
 Description : Public interface.

*/

#ifndef ENG_ASSETS
#define ENG_ASSETS

#include <string>

#include "core/defines.h"
#include "core/types.h"

namespace en
{
namespace assets
{

class Interface
{
public:

    virtual const std::string& assetsPath(void) const = 0;
    virtual const std::string& screenshotsPath(void) const = 0;

    virtual ~Interface() {};                       // Polymorphic deletes require a virtual base destructor
};

} // en::assets

assets::Interface& Assets();

} // en

#endif