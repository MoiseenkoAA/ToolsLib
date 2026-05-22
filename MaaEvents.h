
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/MaaEvents.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains classes for working with CGI data.
 * Parse, get, send data.
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


#ifndef _WIN32

#define INFINITE 0xffffffff
#define WAIT_OBJECT_0 0
#define WAIT_ABANDONED_0 0x80 // unused
#define WAIT_ABANDONED WAIT_ABANDONED_0
#define WAIT_TIMEOUT 0x102
#define WAIT_FAILED 0xffffffff
#define MAXIMUM_WAIT_OBJECTS_EXCEEDED 0xc
#define MAXIMUM_WAIT_OBJECTS 64
#define HANDLE TOOLSLIB_WIN32_HANDLE

#define INVALID_HANDLE_VALUE nullptr

#endif


#ifndef TOOLSLIB_SINGLE_THREAD

class CMaaFdSockets;
class CMaaSockThread;

#ifndef _WIN32

class CMaaEvent2;
typedef CMaaEvent2 * TOOLSLIB_WIN32_HANDLE;

//
HANDLE CreateEvent(const void * lpEventAttributes, bool bManualReset, bool bInitialState, const void * lpName);
bool CloseHandle(HANDLE h);
bool SetEvent(HANDLE hEvent);
bool ResetEvent(HANDLE hEvent);
// PulseEvent(HANDLE hEvent);

// ret: WAIT_OBJECT_0=0, WAIT_TIMEOUT=0x102, WAIT_ABANDONED=0x80, WAIT_FAILED=0xffffffff
_dword WaitForSingleObject(HANDLE hHandle, _dword dwMilliseconds);
_dword WaitForSingleObject_(HANDLE hHandle, _qword TimeOut); // us

//ret: WAIT_OBJECT_0 to (WAIT_OBJECT_0 + nCount – 1)
//     WAIT_ABANDONED_0 to (WAIT_ABANDONED_0 + nCount – 1)
//     WAIT_TIMEOUT=0x102, WAIT_FAILED=0xffffffff, MAXIMUM_WAIT_OBJECTS_EXCEEDED=0xc
// MAXIMUM_WAIT_OBJECTS=64
_dword WaitForMultipleObjects(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _dword dwMilliseconds);
_dword WaitForMultipleObjects_(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut); // us

class CMaaEvents2Initializer
{
public:
    CMaaEvents2Initializer();
    ~CMaaEvents2Initializer();
    bool CreateInTheNewThread(CMaaSockThread * pThread); // or
    // protected:
    bool Attach(CMaaFdSockets * pFdSockets);             // this
    bool WaitReady();                                    // and this
public:
    void StartClose();
};

#else // _WIN32

class CMaaEvents2Initializer
{
public:
    CMaaEvents2Initializer() {}
    ~CMaaEvents2Initializer() {}
    bool CreateInTheNewThread(CMaaSockThread *) {return true;}
    bool Attach(CMaaFdSockets *) {return true;}
    bool WaitReady() {return true;}
    void StartClose() {}
};

_dword WaitForSingleObject_(HANDLE hHandle, _qword TimeOut); // us
_dword WaitForMultipleObjects_(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut); // us

#endif

#endif
