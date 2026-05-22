
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Defs.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains wide used definitions for ToolsLib and other projects.
 * The library implementation written
 * by Andrey A. Moiseenko (support@maasoftware.ru).
 * This library and applications are
 * FREE FOR COMMERCIAL AND NON-COMMERCIAL USE
 * as long as the following conditions are aheared to.
 *
 * Copyright remains Andrey A. Moiseenko, and as such any Copyright notices in
 * the code are not to be removed.  If this code is used in a product,
 * Andrey A. Moiseenko should be given attribution as the author of the parts used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Andrey A. Moiseenko (support@maasoftware.ru)
 *
 * THIS SOFTWARE IS PROVIDED BY ANDREY A. MOISEENKO ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifndef __TOOLS_DEFS_H
#define __TOOLS_DEFS_H

#define TOOLSLIB_LITTLE_ENDIAN

#define NO_TOOLSLIB_ARCHIVERS
//#define TOOLSLIB_DONT_USE_ALLOCATORS

#ifdef _WIN32
#if __cplusplus >= 199711L
//#define CMaa_fallthrough
#define CMaa_fallthrough [[fallthrough]]
#else
#define CMaa_fallthrough
#endif
#else
#if __cplusplus >= 201402L
#define CMaa_fallthrough [[fallthrough]]
#else
#define CMaa_fallthrough
#endif
#endif

#ifdef _WIN64
#define _____AMD64_____
#endif

#if defined(__unix__) // || defined(__GNUC__)
//#define __UNIX64__
#define _UNICODE      // COMMONLY USED WINDOWS MACROS !!!
#endif

//#define VC60

#ifdef VC60
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
#endif

#ifdef _WIN32
     #if defined ( BUILD_TOOLS )
          #define ToolsExport _declspec ( dllexport )
     #else
          #define ToolsExport _declspec ( dllimport )
     #endif

// For tools static library...
          #undef ToolsExport
          #define ToolsExport

#ifndef _CPPRTTI
#ifndef VC60
#error "RTTI option is not set in project options"
#endif
#endif

#else
     #define ToolsExport
#endif

/*
#ifdef _WIN32
     typedef bool _bool;
     #define _true    true
     #define _false   false
     #define socklen_t int
#else
     typedef _dword  _IP;
     typedef int _bool;
     #define _true    1
     #define _false   0
     typedef _bool BOOL;
#ifndef TRUE
        #define TRUE _true
#endif
#ifndef FALSE
        #define FALSE _false
#endif
       #define INT3
       #define SOCKET int
       #define INVALID_SOCKET -1
       #define SOCKET_ERROR -1
       #define closesocket close
       #define memicmp memcmp
       //#define stricmp(a,b) memicmp(a,b,strlen(a))
       //#define strnicmp(a,b,l) memicmp(a,b,l>strlen(a)?strlen(a):l)
       #define stricmp(a,b) strcmp(a,b)
       #define strnicmp(a,b,l) ((unsigned int)(l)>strlen(a)||(unsigned int)(l)>strlen(b)?stricmp(a,b):memicmp(a,b,l))
#endif
*/

#define TOOLSLIB_CHAR8T_SUPPORT

#ifdef _WIN32

typedef unsigned char   _byte;
typedef unsigned short  _uword;
typedef short           _sword;
typedef unsigned long   _udword;
typedef long            _sdword;
typedef __int64         _sqword;
typedef unsigned __int64 __uint64;
typedef __uint64        _uqword;

typedef WCHAR _WC_;

#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef long ssize_t;
#endif

#else

typedef __uint8_t     _byte;
typedef __uint16_t    _uword;
typedef __int16_t     _sword;
typedef __uint32_t    _udword;
typedef __int32_t     _sdword;
typedef __int64_t     _sqword;
typedef __uint64_t    _uqword;

typedef _uqword       __uint64;
typedef _sqword       __int64;

typedef unsigned short _WC_;
//typedef wchar_t _WC_;

typedef char TCHAR;
//typedef _WC_  WCHAR;
#endif

typedef _uword          _word;
typedef _udword         _dword;
typedef _sqword         _qword;

typedef unsigned int     _uint;

#define _dword_shift 2
#define _dword_mask 3
#define _dword_bits 32
#define _qword_shift 3
#define _qword_mask 7
#define _qword_bits 64


#ifdef _WIN32
typedef bool _bool;
     #define _true    true
     #define _false   false
     #define socklen_t int
#else
typedef _dword  _IP;
#ifndef __unix__
typedef int _bool;
     #define _true    1
     #define _false   0
#else
typedef bool _bool;
     #define _true    true
     #define _false   false
#endif
typedef int BOOL;
#ifndef TRUE
        #define TRUE 1
#endif
#ifndef FALSE
        #define FALSE 0
#endif
       #define INT3
       #define SOCKET int
       #define INVALID_SOCKET -1
       #define SOCKET_ERROR -1
       #define closesocket close
       #define memicmp memcmp
//#define stricmp(a,b) memicmp(a,b,strlen(a))
//#define strnicmp(a,b,l) memicmp(a,b,l>strlen(a)?strlen(a):l)
       #define stricmp(a,b) strcmp(a,b)
       #define strnicmp(a,b,l) ((unsigned int)(l)>strlen(a)||(unsigned int)(l)>strlen(b)?stricmp(a,b):memicmp(a,b,l))
#endif


#if defined(__BORLANDC__) || defined(__unix__)
//typedef _bool BOOL;
typedef _byte BYTE;
typedef _word WORD;
typedef _dword DWORD;
//typedef _qword QWORD;
typedef void * LPVOID;
//const int  TRUE  = 1;
//const int  FALSE = 0;
const _dword MAXDWORD = (_dword)-1;
#endif

#ifdef __unix__
#define INT3
#endif


typedef const char * _PCSTR;
typedef char * _PSTR;

struct eExitThread {};

typedef int _ErrCode;

#ifdef __unix__
#define CMaa_CLASSDEF(ClassName)
#else
#define CMaa_CLASSDEF(ClassName)                       \
typedef ClassName & R_##ClassName;                \
typedef const ClassName & CR_##ClassName;         \
typedef ClassName * P_##ClassName;                \
typedef const ClassName * CP_##ClassName;
#endif
//---------------------------------------------------------------------------
#ifndef GCC_PACKED
    #ifndef __unix__
        #define GCC_PACKED
    #else
        #define GCC_PACKED __attribute__((packed))
        #define TOOLSLIB_GCC
    #endif
#endif
//---------------------------------------------------------------------------

typedef _dword  _IP;
typedef _word _Port;

//---------------------------------------------------------------------------
#if defined(_WIN32) && !defined(_WIN64)
typedef int int3264;
typedef unsigned int uint3264;
#else
typedef _qword int3264;
typedef _uqword uint3264;
#endif
//---------------------------------------------------------------------------

enum eNotInit_
{
    eNotInit
};
enum eNoExcept_
{
    eNoExcept
};

#endif // __TOOLS_DEFS_H

//==============================================================================

