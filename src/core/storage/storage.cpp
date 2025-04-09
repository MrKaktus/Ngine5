/*
 
 Ngine v4.0
 
 Module      : File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.
 
*/

#include "assert.h"

#include "core/log/log.h"

#include "core/utilities/parser.h"   // isWhitespace, isEol
#include "utilities/strings.h"

#include "core/storage/andStorage.h"
#include "core/storage/bbStorage.h"
#include "core/storage/osxStorage.h"
#include "core/storage/winStorage.h"
namespace en
{
namespace storage
{

CommonFile::CommonFile() :
    fileSize(0u),
    File()
{
}

uint64 CommonFile::size(void)
{
    return fileSize;
}

bool CommonFile::read(volatile void* buffer)
{
    return read(0u, fileSize, buffer, nullptr);
}

// Need to be declared so that above method has something to call?
bool CommonFile::read(const uint64 offset, const uint64 size, volatile void* buffer, uint64* readBytes)
{
    // Should be implemented by specialization class
    assert( 0 );
    return false;
}
   
// This is super inefficient! Neet to remove it and replace with parser!
uint32 CommonFile::read(const uint64 offset, const uint32 maxSize, std::string& word)
{
    word.clear();
    uint64 fileOffset = offset;
    uint32 counter=0;
    uint8 letter;
    for(; counter<maxSize; ++counter)
    {
        if (!read(fileOffset, 1, &letter))
        {
            return counter;
        }

        if (!letter)
        {
            return counter;
        }

        fileOffset++;
        word.push_back(letter);
    }

    return counter;
}

// This is super inefficient! Neet to remove it and replace with parser!
uint32 CommonFile::readWord(const uint64 offset, const uint32 maxSize, std::string& word)
{
    word.clear();
    uint64 fileOffset = offset;
    uint32 counter=0;
    uint8 letter;
    for(; counter<maxSize; ++counter)
    {
        if (!read(fileOffset, 1, &letter))
        {
            return counter;
        }

        if (isWhitespace(letter))
        {
            return counter;
        }

        fileOffset++;
        word.push_back(letter);
    }

    return counter;
}

// This is super inefficient! Neet to remove it and replace with parser!
uint32 CommonFile::readLine(const uint64 offset, const uint32 maxSize, std::string& line)
{
    line.clear();
    uint64 fileOffset = offset;
    uint32 counter=0;
    uint8 letter;
    for(; counter<maxSize; ++counter)
    {
        if (!read(fileOffset, 1, &letter))
        {
            return counter;
        }

        if (isEol(letter))
        {
            return counter;
        }

        fileOffset++;
        line.push_back(letter);
    }

    return counter;
}






CommonStorage::CommonStorage() :
    Interface()
{
    processPath.clear();
}

CommonStorage::~CommonStorage()
{
}

uint64 CommonStorage::read(const std::string& filename, std::string& dst)
{
    std::shared_ptr<File> file = open(filename);
    if (!file)
    {
        dst.clear();
        return 0u;
    }
      
    uint64 sizeToRead = file->size();
    if (sizeToRead == 0)
    {
        Log << std::string("File " + filename + " is empty!");
        dst.clear();
        return 0;
    }

    // Creating destination buffer and reading file
    uint8* buffer = new uint8[sizeToRead+1];
    uint64 readSize;
    if (!file->read(0u, sizeToRead, buffer, &readSize))
    {
        Log << std::string("Error when reading file, read " + stringFrom(readSize) + " from " + stringFrom(sizeToRead) + " bytes!");
        dst.clear();
        return 0;
    }
    file = nullptr;

    // Converts file content to string
    buffer[readSize] = 0;
    dst = std::string((const char *)buffer);
    delete [] buffer;
   
    // Return results
    return readSize+1;
}
   
// This static function should be in .mm file if we include iOS/OSX headers !!!
bool Interface::create(void)
{
    //Log << "Starting module: Storage.\n";

#if defined(EN_PLATFORM_ANDROID)
    Storage = std::make_shared<AndInterface>();
#endif
#if defined(EN_PLATFORM_BLACKBERRY)
    Storage = std::make_shared<BBInterface>();
#endif
#if defined(EN_PLATFORM_IOS) || defined(EN_PLATFORM_OSX)
    Storage = std::make_shared<OSXInterface>();
#endif
#if defined(EN_PLATFORM_WINDOWS)
    Storage = std::make_shared<WinInterface>();
#endif

    return (Storage == nullptr) ? false : true;
}

} // en::storage
   
std::shared_ptr<storage::Interface> Storage = nullptr;

} // en
