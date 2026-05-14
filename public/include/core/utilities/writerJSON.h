/*

 Ngine v5.0

 Module      : JSON writer.
 Requirements: none
 Description : Set of helper functions for writing JSON files.

*/

#ifndef ENG_CORE_UTILITIES_WRITER_JSON
#define ENG_CORE_UTILITIES_WRITER_JSON

#include <vector>

#include "core/defines.h"
#include "core/storage.h"
#include "core/utilities/parserJSON.h"

namespace en
{

class WriterJSON
{
    storage::File& file;
    uint64 offset;
    std::vector<JSONType> nesting; // Keeps track of nested types were currently in, and current indentation depth
    bool keysAdded;                // True if at least one Key was added to current object (cleared on entering object)
    bool waitingForValue;          // True when Key was added but its Value didn't

private:

    void indent(void);

public:

    // Automatically creates main object.
    // Tracks indentation in automatic way.
    WriterJSON(storage::File& file);
   ~WriterJSON();

    bool enterObject(void);  // {

    // Returns false if we're currently not inside of an object (if its a JSON syntax breaking operation)
    bool leaveObject(void);  // }

    // Simple keys:

    // Returns false if its a JSON syntax breaking operation
    // (for example we've already defined key name and now should provide its value)
    bool addKeyU64(const std::string& name, const uint64 value);
    bool addKeyString(const std::string& name, const std::string& value);

/*
    bool enterArray(void);   // [
    // Returns false if we're currently not inside of an array (if its a JSON syntax breaking operation)
    bool leaveArray(void);   // ]
//*/

};
} // en

#endif