
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/perm.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This files is including main headers whitch is to be precompiled
 * It is not need to include STL headers but if you need it probably
 * here is a best place.
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

#ifndef __TOOLS_PERM_H
#define __TOOLS_PERM_H

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
#include <string>	//MF
#include <new>
#include <atomic>
#include <bit>
#include <mutex>
#include <condition_variable>
//using namespace std;

//#include <stdint.h> // char8_t

//#include <assert.h> //MF

/*
#ifdef _WIN32
#define TURN_OFF_THIS_USED_IN_BASE_MEMEBER_INITIALIZATION_WARNING #pragma warning(disable:4355) // this used in base member initialization list
#define TURN_ON_THIS_USED_IN_BASE_MEMEBER_INITIALIZATION_WARNING  #pragma warning(default:4355) // this used in base member initialization list
#else
#define TURN_OFF_THIS_USED_IN_BASE_MEMEBER_INITIALIZATION_WARNING
#define TURN_ON_THIS_USED_IN_BASE_MEMEBER_INITIALIZATION_WARNING
#endif
*/

#define CMaaStringReplace
#define __TOOLSLIB_REMOVE_OLD_CODE

#ifdef __TOOLSLIB_REMOVE_OLD_CODE
#define __TOOLSLIB_REMOVE_OLD_CODE_SKELETONS
#endif

#ifdef __linux__
//#define FAST_CGI_SUPP
#endif
//#define NEW_DELETE_PRINTF

struct sDbgTmp
{
};

#if defined(_WIN32) && defined(_DEBUG)
int gTL_DbgAlloc() noexcept;
#define TL_DbgAlloc gTL_DbgAlloc()
#else
#define TL_DbgAlloc //0
#endif

/*
#ifdef _WIN32
#ifdef _DEBUG
struct sDbgTmp
{
};
#endif
#endif
*/

#ifndef __unix__
#define NO_CURL_WRP

#pragma warning(disable:4996)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#define NOT_USE_WINSOCK
#define __TOOLS_USE_WINSOCK2

#pragma warning( disable : 4251 )

#ifdef __BORLANDC__
     #define NOT_USE_WINSOCK
#endif

#ifdef _WIN32
#pragma pack( push, temp_pack_id_1, 8 ) // using standart(=8) pack value for MS Visual C++ 6.0 for standart headers // eliminates bug in wincon.h

#ifndef NOT_USE_WINSOCK

#ifdef __TOOLS_USE_WINSOCK2
     #include <winsock2.h>
     #include <ws2tcpip.h>
     #pragma comment(lib,"ws2_32.lib")
#else
     #define __TOOLS_USE_WINSOCK
     #include <winsock.h>
     #pragma comment(lib,"wsock32.lib")
#endif
#include <psapi.h>

#endif // NOT_USE_WINSOCK

     #include <windows.h>
     #include <tchar.h>


//     #include <new>
//#include <string>
     #include <stdio.h>
     #include <direct.h>
     #include <time.h>
     #include <SYS\stat.h>
     #include <SYS\utime.h>
     #include <io.h>
     #include <fcntl.h>
     #include <errno.h>
//#include <list>
     #include <process.h>
     #include <winioctl.h>
#else
     #include <stdio.h>
     #include <stdlib.h>
     #include <string.h>
     #include <dir.h>
     #include <io.h>
     #include <dos.h>
     #include <direct.h>
     #include <time.h>
     #include <errno.h>
#endif

#include <stdarg.h>
#include <conio.h>

#ifdef _WIN32
#pragma pack( pop, temp_pack_id_1 )
#endif


#else // __unix__

     #include <stdio.h>
     #include <stdlib.h>
     #include <string.h>
     #include <stdarg.h>
//     #include <dir.h>
//     #include <io.h>
//     #include <dos.h>
//     #include <direct.h>
     #include <time.h>
     #include <sys/time.h>
     #include <errno.h>
     #include <sys/stat.h>  // for umask()
     #include <sys/param.h> // for MAXPATHLEN
     #include <netinet/in.h>
     #include <net/if.h>
     #include <sys/socket.h>
     #include <sys/epoll.h>
     #include <sys/resource.h>
     #include <sys/times.h>
//#include <sys/vtimes.h>
     #include <new>

#define _ITS_FREEBSD_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/select.h>
#include <unistd.h>
#ifndef _ITS_FREEBSD_
#include <sys/io.h>
#endif
#include <pthread.h>
#include <semaphore.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <fts.h>

//#include <curses.h>

//#include "lib/perm.h"
//#include "lib/temp.h"

#ifndef _ITS_FREEBSD_
#include "io4.h"
#else
#include <sys/stat.h>
#endif

#include <signal.h>
//#ifdef __linux__
#include <setjmp.h>
//#endif
#include <sys/wait.h>

#endif

#ifndef NO_CURL_WRP
// #apt install curl libcurl4-openssl-dev
#include <curl/curl.h>
#endif

/*+++++++++++*/
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#include "errcodes.h"
#endif


#ifdef FAST_CGI_SUPP
#include "fcgi_config.h"
#include "fcgiapp.h"
#endif

//#define ___ToolsLibQT___

#endif // __TOOLS_PERM_H

//==============================================================================
