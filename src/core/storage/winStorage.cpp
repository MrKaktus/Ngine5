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
using namespace std;

#include <assert.h>

namespace en
{
   namespace storage
   {

#if UseFStreamOverWinAPI
   WinFile::WinFile(fstream* _handle) :
      handle(_handle),
      CommonFile()
   {
   assert( handle );
   handle->seekg(0u, ios::end);
   fileSize = handle->tellg();
   handle->seekg(0u, ios::beg);
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
   handle->seekg(offset, ios::beg);
   handle->read((char*)buffer, _size);
   uint64 read = handle->gcount();
   if (readBytes != nullptr)
      *readBytes = read;

   if (read != _size)
      return false;
   
   if (handle->bad() || handle->eof())
      return false;
      
   return true;
   }

   // You should not write into your app bundle, and instead
   // use one of the specific folders to store files.
   //
   // TODO: Which folders are those on BlackBerryOS 10?

   bool WinFile::write(const uint64 _size, void* buffer)
   {
   assert( handle );

   handle->clear();
   handle->seekg(0, ios::beg);
   handle->write((char*)buffer, _size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
   
   bool WinFile::write(const uint64 offset, const uint64 _size, void* buffer)
   {
   assert( handle );

   handle->clear();
   handle->seekg(offset, ios::beg);
   handle->write((char*)buffer, _size);
   if (handle->bad() || handle->eof())
      return false;
   return true;
   }
 
   WinInterface::WinInterface() :
      CommonInterface()
   {

   }

   WinInterface::~WinInterface()
   {
   }
   
   bool WinInterface::exist(const string& filename)
   {
   ifstream* rfile;
          
   // Creates input stream
   rfile = new ifstream(filename.c_str(), ios::in | ios::binary | ios::ate);
   if (rfile->fail())
      return false;

   rfile->close();
   return true;
   }
   
   shared_ptr<File> WinInterface::open(const string& filename, const FileAccess mode)
   {
   shared_ptr<WinFile> result = nullptr;
   
   fstream* handle = nullptr;

   if (mode == Read)
      handle = new fstream(filename.c_str(), ios::in  | ios::binary);
   else if (mode == Write)
      handle = new fstream(filename.c_str(), ios::out | ios::binary); 
   else
      handle = new fstream(filename.c_str(), ios::in  | ios::out | ios::binary);

   if (handle->good())
      result = make_shared<WinFile>(handle);
   else
      delete handle;

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
   //
   // TODO: Which folders are those on BlackBerryOS 10?

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
      CommonInterface()
   {

   }

   WinInterface::~WinInterface()
   {
   }

   bool WinInterface::exist(const string& filename)
   {
   DWORD dwAttrib = GetFileAttributes((LPCWSTR)filename.c_str());

   return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
   }

   shared_ptr<File> WinInterface::open(const string& filename, const FileAccess mode)
   {
   shared_ptr<WinFile> result = nullptr;
   
   HANDLE handle = nullptr;

   if (mode == Read)
      handle = CreateFile((LPCWSTR)filename.c_str(), 
                          GENERIC_READ, 
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          nullptr,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          nullptr);
   else if (mode == Write)
      handle = CreateFile((LPCWSTR)filename.c_str(), 
                          GENERIC_WRITE, 
                          FILE_SHARE_READ,
                          nullptr,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          nullptr);
   else
      handle = CreateFile((LPCWSTR)filename.c_str(), 
                          GENERIC_READ | GENERIC_WRITE, 
                          FILE_SHARE_READ,
                          nullptr,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          nullptr);

   if (handle != INVALID_HANDLE_VALUE)
      result = make_shared<WinFile>(handle);
   else
      CloseHandle(handle);

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

   }
}
#endif
