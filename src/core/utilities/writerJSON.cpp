/*

 Ngine v5.0

 Module      : JSON writer.
 Requirements: none
 Description : Set of helper functions for writing JSON files.

*/

#include "core/log/log.h"
#include "core/utilities/writerJSON.h"
#include "utilities/strings.h"

namespace en
{

WriterJSON::WriterJSON(storage::File& _file) :
    file(_file),
    offset(0),
    keysAdded(false),
    waitingForValue(false)
{
    enterObject();
}

WriterJSON::~WriterJSON()
{
    leaveObject();
}

void WriterJSON::indent(void)
{
    // This code is not designed for speed, but for easy of use.
    // It is assumed that implementation of file system operations
    // is already buffering those writes allowing fine grained
    // writes like below in efficient way.
    uint16 indentation = nesting.size() * 4;
    if (indentation > 0)
    {
        for (uint16 i = 0; i < indentation; ++i)
        {
            file.write(offset, 1, " ");
            ++offset;
        }
    }
}

bool WriterJSON::enterObject(void)
{
    indent();
    file.write(offset, 2, "{\n");
    offset += 2;

    nesting.push_back(JSONType::Object);

    keysAdded = false;
    return true;
}

bool WriterJSON::leaveObject(void)
{
    if (nesting.back() != JSONType::Object)
    {
        return false;
    }

    // Finish last key line (without comma as there won't be next key)
    if (keysAdded)
    {
        file.write(offset, 1, "\n");
        ++offset;
    }

    nesting.pop_back();

    indent();
    file.write(offset, 2, "}\n");
    offset += 2;

    return true;
}

bool WriterJSON::enterArray(void)
{
    if (!waitingForValue)
    {
        return false;
    }

    // Arrays are always values (either of Key:Value pair or of other Array)
    indent();
    file.write(offset, 2, "[\n");
    offset += 2;

    nesting.push_back(JSONType::Array);

    keysAdded = false;
    return true;
}

bool WriterJSON::leaveArray(void)
{
    if (nesting.back() != JSONType::Array)
    {
        return false;
    }

    // Finish last value line (without comma as there won't be next value)
    if (keysAdded)
    {
        file.write(offset, 1, "\n");
        ++offset;
    }

    nesting.pop_back();

    // Array is always a value, in either Key:Value pair, or inside of
    // other Array. Thus on its termination its line is never terminated,
    // as its conforming to "keysAdded" comma rule. "keysAdded" flag
    // remains set even though we leave this array scope, because it 
    // is value in outer scope, which means keys/values were added.
    indent();
    file.write(offset, 1, "]");
    ++offset;

    return true;
}

bool WriterJSON::addKey(const std::string& name, const uint64 value)
{
    if (nesting.back() != JSONType::Object)
    {
        logError("JSON syntax breaking operation on write:\nTrying to add Key while not in Object.");
        return false;
    }

    if (waitingForValue)
    {
        logError("JSON syntax breaking operation on write:\nKey cannot be added, when previous Key is waiting for its Value to be specified.");
        return false;
    }

    if (name.empty())
    {
        logError("Cannot add JSON key, as its name is empty!\n");
        return false;
    }

    // Finish previous key line
    if (keysAdded)
    {
        file.write(offset, 2, ",\n");
        offset += 2;
    }

    indent();
    file.write(offset, 1, "\"");
    ++offset;

    file.write(offset, name.length(), (void*)name.c_str());
    offset += name.length();

    file.write(offset, 4, "\" : ");
    offset += 4;

    std::string valueString = stringFrom(value);
    file.write(offset, valueString.length(), (void*)valueString.c_str());
    offset += valueString.length();

    keysAdded = true;
    return true;
}

bool WriterJSON::addKey(const std::string& name, const std::string& value)
{
    if (nesting.back() != JSONType::Object)
    {
        logError("JSON syntax breaking operation on write:\nTrying to add Key while not in Object.");
        return false;
    }

    if (waitingForValue)
    {
        logError("JSON syntax breaking operation on write:\nKey cannot be added, when previous Key is waiting for its Value to be specified.");
        return false;
    }

    if (name.empty())
    {
        logError("Cannot add JSON key, as its name is empty!\n");
        return false;
    }

    if (value.empty())
    {
        logError("Cannot add JSON key, as its string value is empty!\n");
        return false;
    }

    // Finish previous key line
    if (keysAdded)
    {
        file.write(offset, 2, ",\n");
        offset += 2;
    }

    indent();
    file.write(offset, 1, "\"");
    ++offset;

    file.write(offset, name.length(), (void*)name.c_str());
    offset += name.length();

    file.write(offset, 5, "\" : \"");
    offset += 5;

    file.write(offset, value.length(), (void*)value.c_str());
    offset += value.length();

    file.write(offset, 1, "\"");
    ++offset;

    keysAdded = true;
    return true;
}

bool WriterJSON::addKeyArray(const std::string& name)
{
    if (nesting.back() != JSONType::Object)
    {
        logError("JSON syntax breaking operation on write:\nTrying to add Key while not in Object.");
        return false;
    }

    if (waitingForValue)
    {
        logError("JSON syntax breaking operation on write:\nKey cannot be added, when previous Key is waiting for its Value to be specified.");
        return false;
    }

    if (name.empty())
    {
        logError("Cannot add JSON key, as its name is empty!\n");
        return false;
    }

    // Finish previous key line
    if (keysAdded)
    {
        file.write(offset, 2, ",\n");
        offset += 2;
    }

    indent();
    file.write(offset, 1, "\"");
    ++offset;

    file.write(offset, name.length(), (void*)name.c_str());
    offset += name.length();

    file.write(offset, 4, "\" :\n");
    offset += 4;

    enterArray();

    waitingForValue = true;
    return true;
}

bool WriterJSON::addArrayValue(const std::string& value)
{
    if (nesting.back() != JSONType::Array)
    {
        return false;
    }

    if (value.empty())
    {
        logError("Cannot add JSON array value, as its string is empty!\n");
        return false;
    }

    // Finish last value line (without comma as there won't be next value)
    if (keysAdded)
    {
        file.write(offset, 1, "\n");
        ++offset;
    }

    indent();
    file.write(offset, 1, "\"");
    ++offset;

    file.write(offset, value.length(), (void*)value.c_str());
    offset += value.length();

    file.write(offset, 1, "\"");
    ++offset;

    keysAdded = true;
    return true;
}

bool WriterJSON::leaveKeyArray(void)
{
    leaveArray();

    waitingForValue = false;
    return true;
}

}