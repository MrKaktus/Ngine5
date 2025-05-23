/*

 Ngine v5.0
 
 Module      : Windows File system operations.
 Requirements: none
 Description : Holds methods that support reading and
               writing to files on storage devices or
               on virtual file system.

*/

#include "core/storage/winStorage.h"

#if defined(EN_PLATFORM_WINDOWS)

#include <iostream>
#include <fstream>

#include <assert.h>

namespace en
{
namespace storage
{

#if UseFStreamOverWinAPI
WinFile::WinFile(std::fstream* _handle) :
    handle(_handle),
    CommonFile()
{
    assert( handle );
    handle->seekg(0u, std::ios::end);
    fileSize = handle->tellg();
    handle->seekg(0u, std::ios::beg);
}
   
WinFile::~WinFile()
{
    assert( handle );
    handle->close();
    delete handle;
}
   
bool WinFile::read(const uint64 offset, const uint64 _size, volatile void* buffer, uint64* readBytes)
{
    assert( handle );
    assert( offset + _size <= fileSize );
   
    handle->clear();
    handle->seekg(offset, std::ios::beg);
    handle->read((char*)buffer, _size);
    uint64 read = handle->gcount();
    if (readBytes != nullptr)
    {
        *readBytes = read;
    }

    if (read != _size)
    {
        return false;
    }

    if (handle->bad() || handle->eof())
    {
        return false;
    }

    return true;
}

// You should not write into your app bundle, and instead
// use one of the specific folders to store files.

bool WinFile::write(const uint64 _size, void* buffer)
{
    assert( handle );

    handle->clear();
    handle->seekg(0, std::ios::beg);
    handle->write((char*)buffer, _size);
    if (handle->bad() || handle->eof())
    {
        return false;
    }

    return true;
}
   
bool WinFile::write(const uint64 offset, const uint64 _size, void* buffer)
{
    assert( handle );

    handle->clear();
    handle->seekg(offset, std::ios::beg);
    handle->write((char*)buffer, _size);
    if (handle->bad() || handle->eof())
    {
        return false;
    }

    return true;
}
 
WinInterface::WinInterface() :
   CommonStorage()
{

}

WinInterface::~WinInterface()
{
}

bool WinInterface::exist(const std::string& filename)
{
    std::ifstream* rfile;
          
    // Creates input stream
    rfile = new std::ifstream(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    if (rfile->fail())
    {
        return false;
    }

    rfile->close();
    return true;
}
   
// TODO: This is not "Win" interface but "Std" interface that should work on all platforms.
File* WinInterface::open(const std::string& filename, const FileAccess mode)
{
    WinFile* result = nullptr;
   
    std::fstream* handle = nullptr;

    if (mode == Read)
    {
        handle = new std::fstream(filename.c_str(), std::ios::in  | std::ios::binary);
    }
    else 
    if (mode == Write)
    {
        handle = new std::fstream(filename.c_str(), std::ios::out | std::ios::binary);
    }
    else
    {
        handle = new std::fstream(filename.c_str(), std::ios::in  | std::ios::out | std::ios::binary);
    }

    if (handle->good())
    {
        result = new WinFile(handle);
    }
    else
    {
        delete handle;
    }

    return result;
}

#else

WinFile::WinFile(HANDLE _handle) :
    handle(_handle),
    CommonFile()
{
    assert( handle );

    fileSize = GetFileSize(handle, nullptr);
}

WinFile::~WinFile()
{
    assert( handle );

    CloseHandle(handle);
}

bool WinFile::read(const uint64 offset, const uint64 _size, void* buffer, uint64* readBytes)
{
    assert( handle );
    assert( offset + _size <= fileSize );
    assert( _size <= 0xFFFFFFFF );          // Currently ReadFile supports only sizes of max 4GB 

    OVERLAPPED desc;
    desc.Offset     = static_cast<DWORD>(offset);
    desc.OffsetHigh = static_cast<DWORD>(offset >> 32);

    return ReadFile(handle, buffer, static_cast<DWORD>(_size), reinterpret_cast<LPDWORD>(readBytes), &desc);
}

// You should not write into your app bundle, and instead
// use one of the specific folders to store files.

bool WinFile::write(const uint64 _size, void* buffer)
{
    assert( handle );

    DWORD writtenBytes = 0;
    return WriteFile(handle, buffer, static_cast<DWORD>(_size), &writtenBytes, nullptr);
}
   
bool WinFile::write(const uint64 offset, const uint64 _size, void* buffer)
{
    assert( handle );

    OVERLAPPED desc;
    desc.Offset     = static_cast<DWORD>(offset);
    desc.OffsetHigh = static_cast<DWORD>(offset >> 32);

    DWORD writtenBytes = 0;
    return WriteFile(handle, buffer, static_cast<DWORD>(_size), &writtenBytes, &desc);
}

WinInterface::WinInterface() :
    CommonStorage()
{

}

WinInterface::~WinInterface()
{
}

bool WinInterface::exist(const std::string& filename)
{
    DWORD dwAttrib = GetFileAttributes((LPCWSTR)filename.c_str());

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
          !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

File* WinInterface::open(const std::string& filename, const FileAccess mode)
{
    WinFile* result = nullptr;
   
    HANDLE handle = nullptr;

    if (mode == Read)
    {
        handle = CreateFile((LPCWSTR)filename.c_str(), 
                            GENERIC_READ, 
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            nullptr,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            nullptr);
    }
    else
    if (mode == Write)
    {
        handle = CreateFile((LPCWSTR)filename.c_str(), 
                            GENERIC_WRITE, 
                            FILE_SHARE_READ,
                            nullptr,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            nullptr);
    }
    else
    {
        handle = CreateFile((LPCWSTR)filename.c_str(), 
                            GENERIC_READ | GENERIC_WRITE, 
                            FILE_SHARE_READ,
                            nullptr,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            nullptr);
    }

    if (handle != INVALID_HANDLE_VALUE)
    {
        result = WinFile(handle);
    }
    else
    {
        CloseHandle(handle);
    }

    return result;
}

//OF_CREATE
//OF_DELETE
//OF_REOPEN
//
//OF_VERIFY
//
//BOOL WINAPI ReadFileEx(
//  _In_      HANDLE                          hFile,
//  _Out_opt_ LPVOID                          lpBuffer,
//  _In_      DWORD                           nNumberOfBytesToRead,
//  _Inout_   LPOVERLAPPED                    lpOverlapped,
//  _In_      LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
//);
//
//// Hot reload observer:
//
//HANDLE WINAPI FindFirstChangeNotification(
//  _In_ LPCTSTR lpPathName,
//  _In_ BOOL    bWatchSubtree,
//  _In_ DWORD   dwNotifyFilter
//);
//
//BOOL WINAPI ReadDirectoryChangesW(
//  _In_        HANDLE                          hDirectory,
//  _Out_       LPVOID                          lpBuffer,
//  _In_        DWORD                           nBufferLength,
//  _In_        BOOL                            bWatchSubtree,
//  _In_        DWORD                           dwNotifyFilter,
//  _Out_opt_   LPDWORD                         lpBytesReturned,
//  _Inout_opt_ LPOVERLAPPED                    lpOverlapped,
//  _In_opt_    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
//);

#endif

} // en::storage
} // en

#endif
