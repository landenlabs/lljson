//-------------------------------------------------------------------------------------------------
// File: Directory.hpp
// Author: Dennis Lang
//
// Desc: This class is used to obtain the names of files in a directory.
//
// Usage::
//      Create a Directory_files object by providing the name of the directory
//      to use.  'next_file_name()' returns the next file name found in the
//      directory, if any.  You MUST check for the existance of more files
//      by using 'more_files()' between each call to "next_file_name()",
//      it tells you if there are more files AND sequences you to the next
//      file in the directory.
//
//      The normal usage will be something like this:
//          Directory_files dirfiles( dirName);
//          while (dirfiles.more_files())
//          {   ...
//              lstring filename = dirfiles.name();
//              ...
//          }
//-------------------------------------------------------------------------------------------------
// ----- License ----
//
// Copyright (c) 2024  Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "ll_stdhdr.hpp"


#ifdef HAVE_WIN
#define byte win_byte_override  // Fix for c++ v17
#include <windows.h>
#undef byte                     // Fix for c++ v17
#else
    typedef unsigned int  DWORD;
    typedef struct dirent Dirent;
    typedef struct timespec Timespec;

    #define _strtoi64 strtoll

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/dirent.h>
    #include <dirent.h>
    #include <unistd.h>
    #include <limits.h>

    const DWORD FILE_ATTRIBUTE_DIRECTORY = S_IFDIR;
    const DWORD FILE_ATTRIBUTE_DEVICE = S_IFBLK;
    const DWORD FILE_ATTRIBUTE_NORMAL = S_IFREG;

    const DWORD FILE_ATTRIBUTE_READ = S_IRUSR; // owner has read permission
    const DWORD FILE_ATTRIBUTE_WRIT = S_IWUSR; // has write permission
    const DWORD FILE_ATTRIBUTE_EXEC = S_IXUSR; // has execute permission

#endif

#ifdef HAVE_WIN
    const char SLASH_CHAR('\\');
    #include <assert.h>
    #define strncasecmp _strnicmp
    #if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
        #define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
    #endif
#else
    const char SLASH_CHAR('/');
    #include <sys/fcntl.h>
#endif

class DirEntry;
typedef void* HANDLE;

class Directory_files {
public:
    Directory_files(const lstring& dirName);
    ~Directory_files();

    // Start at beginning of directory, return true if any files.
    bool   begin();

    // Advance to next file or directory and return true if more items are present.
    bool   more();

    // Return true if current file is a directory
    bool   is_directory() const;

    // Return file/directory entry name
    const char* name() const;

    // Return directory path and entry name.
    const lstring& fullName(lstring& fname) const;

    // Close current directory
    void close();

    static const char SLASH_CHAR;   // '/'  linux, or '\\' windows (escaped slash)
    static const lstring SLASH;     // "/"  linux, or "\\" windows
    static const lstring SLASH2;    // "//" linux, or "\\\\" windows

private:
    Directory_files(const Directory_files&);

#ifdef HAVE_WIN
    WIN32_FIND_DATA my_dirent;      // Data structure describes the file found

    HANDLE      my_dir_hnd;     // Search handle returned by FindFirstFile
    lstring     my_dirName;     // Directory name
#else
    bool        my_is_more;
    DIR*        my_pDir;
    Dirent*     my_pDirEnt;         // Data structure describes the file found
    lstring     my_baseDir;
    char        my_fullname[PATH_MAX];

#endif
};

enum DIR_TYPES { IS_FILE, IS_DIR_BEG, IS_DIR_END };

namespace DirUtil {
 lstring& getDir(lstring& outName, const lstring& inPath);
 lstring& getName(lstring& outName, const lstring& inPath);
 lstring& getExt(lstring& outExt, const lstring& inPath);
 lstring& removeExtn(lstring& outName, const lstring& inPath);
 bool deleteFile(bool dryRun, const char* inPath);
 bool setPermission(const char* inPath, unsigned permission, bool setAllParts = false);
 size_t fileLength(const lstring& path);
 bool fileExists(const char* path);bool makeWriteableFile(const char* filePath, struct stat* info);
inline bool isWriteableFile(const struct stat& info) {
#ifdef HAVE_WIN
    size_t mask = _S_IFREG + _S_IWRITE;
#else
    size_t mask = S_IFREG + S_IWRITE;
#endif
    return ((info.st_mode & mask) == mask);
}

 inline unsigned int minU(unsigned int A, unsigned int B) { return (A<=B) ? A:B; }

 // Utility to join directory and name and replace any double slashes with a single slash.
inline const lstring& join(lstring& outPath, const char* inDir, const char* inName, unsigned int pathOff = 0) {
     // return realpath(fname.c_str(), my_fullname) or   GetFullPath(fname);
     return ReplaceAll(( outPath = lstring(inDir+pathOff) + Directory_files::SLASH + inName ), Directory_files::SLASH2, Directory_files::SLASH);
 }
inline const lstring& join(lstring& outPath, lstring& inDir, const char* inName) {
     return ReplaceAll(( outPath = inDir + Directory_files::SLASH + inName ), Directory_files::SLASH2, Directory_files::SLASH);
 }
}