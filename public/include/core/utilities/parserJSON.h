/*

 Ngine v5.0

 Module      : JSON parser.
 Requirements: none
 Description : Set of helper functions for parsing JSON files.

*/

#ifndef ENG_CORE_UTILITIES_PARSER_JSON
#define ENG_CORE_UTILITIES_PARSER_JSON

#include "core/defines.h"
#include "core/utilities/parser.h"

namespace en
{

enum class JSONType : uint8
{
    None                = 0,  // Reached end of input
    Object                 ,  // Found: {
    ObjectTerminator       ,  // Found: }
    Array                  ,  // Found: [
    ArrayTerminator        ,  // Found: ]
    String                 ,  // Found: "
    UnsignedInteger        ,  // Found number (unsigned integer)
    Integer                ,  // Found number (integer)
    Float                  ,  // Found number (floating point)
    Separator              ,  // Found: :
    Comma                  ,  // Found: ,
    True                   ,  // Found: true
    False                  ,  // Found: false
    Null                   ,  // Found: null
    Boolean                ,
    Count                  ,
};

// Follows JSON specification:
// https://www.json.org/json-en.html
class ParserJSON : public ParserState
{
private:
    JSONType type;  // Last found type (hides base class type)

    // Current depth of complexity that the parser is at
    // (e.g. 3 nested objects/arrays results in parseDepth==3)
    uint32 parseDepth;

    // Internal helper methods

    // Modifies type parameter only on success.
    ParsingResult detectNumberType(JSONType& type);

public:
    // Passes ownership of buffer to parser
    ParserJSON(const uint8* buffer, const uint64 size);
   ~ParserJSON();

    // Progresses in parsed buffer to next parsable element.
    // Modifies type parameter only on success.
    ParsingResult findNextElement(JSONType& type);

    // Returns type of currently detected element
    JSONType currentElement(void) const;

    // Reads value of uint64, if current element is of
    // type UnsignedInteger, otherwise returns false.
    bool readU64(uint64& value) const;

    // Reads value of sint64, if current element is of
    // type Integer or UnsignedInteger, otherwise 
    // returns false.
    bool readS64(sint64& value) const;

    // Reads value of float/double, if current element 
    // is of type Float, Integer or UnsignedInteger, 
    // otherwise returns false.
    bool readFloat(float& value) const;
    bool readDouble(double& value) const;
};

} // en

#endif