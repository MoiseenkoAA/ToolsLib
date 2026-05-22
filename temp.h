
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/temp.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains includes of this library files.
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

#ifndef __TOOLSLIB_TEMP_H
#define __TOOLSLIB_TEMP_H

#define TOOLSLIB_MAX_SPEED

#define TOOLSLIB_DELETE_UNSAFE
//#define TOOLSLIB_SINGLE_THREAD
#ifdef TOOLSLIB_SINGLE_THREAD
#define thread_local_
#ifdef FAST_CGI_SUPP
#undef FAST_CGI_SUPP
#endif
#else
#define thread_local_ thread_local

#endif
//#define TOOLSLIB_LESS_MEMORY
#ifdef _WIN64
#define TOOLSLIB_ASM64
#else
#ifdef _WIN32
//#define TOOLSLIB_ASM32
#endif
#endif

//#define CMaaStringImp_NOT_NULL_MODE
#ifdef CMaaStringImp_NOT_NULL_MODE
#define constexpr_
#else
#define constexpr_ constexpr
#endif
#define TOOLSLIB_USE_CMAASTRING64 0 // 0 - no, 1 - repl 32 by 64, 2 - both 32 & 64
#define TOOLSLIB_A_A_ASSIGN_OPT

//#define TL_memory_order_acq_rel memory_order_acq_rel // the same speed, I7-7700K
#define TL_memory_order_acquire memory_order_acquire // |--> Speedup 
#define TL_memory_order_release memory_order_release // |-->   by 18%, WIN64, I7-7700K; by 0% linux64, KVM

#define TL_memory_order_acq_rel memory_order_seq_cst
//#define TL_memory_order_acquire memory_order_seq_cst
//#define TL_memory_order_release memory_order_seq_cst

#ifdef __linux__
#define TOOLSLIB_ASM64
//#define TOOLSLIB_ASM32
#endif

#ifndef TOOLSLIB_LESS_MEMORY
// typical stack functions buffer sizes
#define TOOLSLIB_CS_4K (4 * 1024)
#define TOOLSLIB_CS_8K (8 * 1024)
#define TOOLSLIB_CS_16K (16 * 1024)
#define TOOLSLIB_CS_32K (32 * 1024)
#define TOOLSLIB_CS_64K (64 * 1024)
#else
#define TOOLSLIB_CS_4K (1 * 1024)
#define TOOLSLIB_CS_8K (2 * 1024)
#define TOOLSLIB_CS_16K (4 * 1024)
#define TOOLSLIB_CS_32K (8 * 1024)
#define TOOLSLIB_CS_64K (16 * 1024)
#endif
#define TOOLSLIB_FGETS_64K (64 * 1024)

#ifdef ___ToolsLibQT___
#define NO_GD
#define NO_CURL_WRP

#ifndef _WIN32
#define ___ToolsLibQTAndroid___
#endif

#else

#ifdef _WIN32
#define NO_GD
#endif

#endif

//#define printf __log

//#define DEBUG_MTX // pass file, line to functions

#ifdef _WIN32

//#define _DEBUG_MTX // debug file, line //  [20]

#ifdef TL_NEW
#undef TL_NEW
#endif

#ifdef _DEBUG
void* __cdecl operator new(size_t nSize, const char* lpszFileName, int nLine, sDbgTmp *);
void* __cdecl operator new[](size_t nSize, const char* lpszFileName, int nLine, sDbgTmp *);
void __cdecl operator delete(void* pData, const char* /*lpszFileName*/, int /*nLine*/, sDbgTmp*);
void __cdecl operator delete[](void* pData, const char* lpszFileName, int nLine, sDbgTmp *);

//#define _new new
//#define _delete delete

#define TL_NEW new(__FILE__, __LINE__, (sDbgTmp *)nullptr)
//#define new TL_NEW

#else
#define TL_NEW new
#endif

#else

#ifdef TL_NEW
#undef TL_NEW
#endif

#define TL_NEW new

#endif

#define new_(ptr, Type) new(ptr) Type
#define new__ new

#if defined(_WIN32) && defined(_DEBUG)
#define new_with_chk new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) // generic new with check
//#define new DEBUG_NEW
#else
#define new_with_chk new
#endif

#define TL_NEW_ new //new_with_chk // skip check
//#define TL_NEW_ TL_NEW // check
//#define new TL_NEW // error, make fixed replace "= new " by "= TL_NEW "
//#define new_psz "new"

#ifdef _WIN32
#define noexcept_new false
#else
#define noexcept_new true
#endif

/*
int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetBreakAlloc(114);
    _CrtSetBreakAlloc(9554);
    _CrtSetBreakAlloc(9553);
    _CrtSetBreakAlloc(9552);
#endif
    //...
}
*/

#ifdef __TOOLSLIB_REMOVE_OLD_CODE
#ifndef __TOOLSLIB_REMOVE_OLD_CODE_SKELETONS
#include "err2.h"
#endif
#endif

#if !defined(_DEBUG) && !defined(__linux__) && !defined(_WIN32)
#define TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
#define TOOLSLIB_KEEP_GLOBALS_IN_MEMORY
#define TOOLSLIB_KEEP_GLOBALS2_IN_MEMORY
#endif

#ifdef __unix__abc
int out_to_file(const char * fmt, ...);
#define printf out_to_file
#endif


#ifdef _WIN32
#pragma pack( push, temp_pack_id_1, 1) // using (=1 byte) pack value
#endif

#include "Defs.h"

extern void (* gptr_error_dbg_printf_function)(const char *) /*= nullptr*/;

void SetProgramNameForDebugOutput(const TCHAR *txt = nullptr);

#ifdef BUILD_TOOLS
//     #include "ToolsLib\RESOURCE\Resource.h"
#endif
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "SmartPtr.h"
#endif
#include "MaaTL.h"
//#include "cheap.h"
#include "smartpt_.h"
#include "Timer.h"
#include "mthrlib.h"
#include "MaaEvents.h"

#ifndef __unix__
#include "Console.h"
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "Instance.h"
#include "Int64.h"
#endif
#endif

#include "Refs.h"
#include "SysInfo.h"
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "String.h"
#endif
#include "StringEx1.h"
#include "Maps.h"
#include "StringEx.h"
#include "StringEx64.h"
#include "CalcFunc.h"
#include "StringLib.h"
#include "md4.h"
#include "md5.h"
//#include "ToolsLib\INCLUDE\Path.h"

#include "Errors.h"
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "Time.h"
#include "Date.h"
#include "DateTime.h"
#include "File.h"
#endif
#include "File2.h"
#include "cheap.h"
#include "BTree.h"
#include "rbtree.h"
#include "avl_tree.h"
#include "avl_tree2.h"
#include "avl_tree3.h"
#include "a_trie.h"

#ifndef __unix__
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "Folder.h"
#include "FindFile.h"
#endif
#endif


#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "Dlink.h"
#include "Univhash.h"
#endif

#ifndef __unix__
#include "comm.h"
#endif

#ifdef _WIN32
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
     #include "ToolsLib\WIN32\AdvStr.h"
     #include "ToolsLib\WIN32\FILESTRM.h"
     #ifdef __TOOLS_USE_WINSOCK2222
          #include "ToolsLib\WIN32\srv.h"
          #include "ToolsLib\WIN32\conn.h"
     #endif
#endif
#ifndef ___ToolsLibQT___
	#include "ToolsLib\WIN32\maareg.h"
	#include "ToolsLib\WIN32\maareg2.h"
	#include "ToolsLib\WIN32\Drv.h"
#else
	#include "WIN32\maareg.h"
	#include "WIN32\maareg2.h"
	#include "WIN32\Drv.h"
#endif
#else
// NOT defined _WIN32 - Nothing interesting
#endif

#include "mthrlib2.h"

#ifndef __unix__
//#include "mthrlib.h"
#ifndef ___ToolsLibQT___
//#include "ToolsLib\WIN32\socket.h"
#else
#include "WIN32\socket.h"
#endif
#include "socket2.h"
#include "MaaXML.h"
//#endif
#else
//#include "File2.h"
//#include "mthrlib.h"
//#include "socket.h"
#include "socket2.h"
#include "MaaXml.h"
#endif

#include "Lib.h"

#include "sk2add.h"
#include "FileTable.h"
#include "templates.h"
#include "resize_lim.h"

//#include "TRexpp2.h"

//#include "wwwcgi/AntiRobotCaptcha.h"
#include "wwwcgi/cgi_helper.h"
#ifdef __unix__
#ifdef ___ToolsLibQT___
#define NO_GD
#endif
//#include "wwwcgi/gd_helper.h"
#endif
#include "wwwcgi/gd_helper.h"

#include "archiver/haio.h"
#include "archiver/acoder.h"
#include "archiver/hsc.h"
// Haffman archiver originally made by Moiseenko A.A., 2016
#include "archiver/haffman.h"

#include "curl_wrp.h"
#include "utils.h"

#ifdef _WIN32
#pragma pack( pop, temp_pack_id_1 )
#endif

#endif // __TOOLSLIB_TEMP_H

//==============================================================================
