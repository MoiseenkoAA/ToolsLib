
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/mthrlib.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * threads, mutexes and so on.
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

/*
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/io.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>

#include <curses.h>
*/

// default: std::memory_order_seq_cst

#define TOOLSLIB_CE_WS_MO // std
////#define TOOLSLIB_CE_WS_MO , std::memory_order_acq_rel, std::memory_order_acquire
//#define TOOLSLIB_CE_WS_MO , std::memory_order_acq_rel, std::memory_order_relaxed

#define TOOLSLIB_LOAD_MO // std
//#define TOOLSLIB_LOAD_MO std::memory_order_acquire
//.load(std::memory_order_acquire)

#define TOOLSLIB_STORE_MO // std
//#define TOOLSLIB_STORE_MO , std::memory_order_release

#ifdef TOOLSLIB_SINGLE_THREAD
#define TOOLSLIB_LOAD_ATOMIC(x) (x)
#define TOOLSLIB_ATOMIC_OP_EQ(x, a, n, y) x n y
#else
//#define TOOLSLIB_LOAD_ATOMIC(x) (x).load(std::memory_order_acquire)
//#define TOOLSLIB_ATOMIC_OP_EQ(x, a, n, y) (x).a(y, std::memory_order_acq_rel)
#define TOOLSLIB_LOAD_ATOMIC(x) (x)
#define TOOLSLIB_ATOMIC_OP_EQ(x, a, n, y) x n y
#endif

int gCMaaToolLib_crt_Initializer() noexcept;

#define TOOLSLIB_FAST_MTX CMaaAtomicFastMutexW
//#define TOOLSLIB_FAST_MTX CMaaFastMutex // TOOLSLIB FAST MUTEX TYPE
//#define TOOLSLIB_RW_LOCKS_EXPERIMENTAL // use slow test version of CMaaRWLock, CMaaRWLock2, CMaaRWLockWp2

#ifdef TOOLSLIB_MAX_SPEED
#ifdef _WIN32
//#define TOOLSLIB_FAST_MUTEXES // bad performance
#endif
#endif

#ifdef TOOLSLIB_SINGLE_THREAD
#ifndef TOOLSLIB_FAST_MUTEXES
#define TOOLSLIB_FAST_MUTEXES
#endif
#endif

#ifdef _WIN32
class CMaaWin32Lock;
#define CMaaMutex2 CMaaWin32Lock
//#define CMaaMutex2 TOOLSLIB_FAST_MTX // CMaaWin32Lock
#else
#endif

#ifdef TOOLSLIB_FAST_MUTEXES
#define CMaaMutex TOOLSLIB_FAST_MTX // CMaaAtomicFastMutex
#else
#define CMaaMutex CMaaMutex2
#endif

//#define TOOLSLIB_FAST_gLock_lib_Mutex // TOOLSLIB_FAST_MTX - bad performance
//#define TOOLSLIB_FAST_gLock_usr_Mutex // TOOLSLIB_FAST_MTX - bad performance
#define TOOLSLIB_FAST_gLock_usr3_Mutex

#ifdef TOOLSLIB_FAST_gLock_lib_Mutex
#define CMaa_gLock_lib_Mutex TOOLSLIB_FAST_MTX // CMaaAtomicFastMutex
#else
#define CMaa_gLock_lib_Mutex CMaaMutex // CMaaStdRecursiveMutex // CMaaMutex
#endif

#ifdef TOOLSLIB_FAST_gLock_usr_Mutex
#define CMaa_gLock_usr_Mutex TOOLSLIB_FAST_MTX // CMaaAtomicFastMutex
#else
#define CMaa_gLock_usr_Mutex CMaaMutex // CMaaStdRecursiveMutex // CMaaMutex
#endif

#ifdef TOOLSLIB_FAST_gLock_usr3_Mutex
#define CMaa_gLock_usr3_Mutex CMaaAtomicFastMutex // CMaaAtomicFastMutex is safe here
#else
#define CMaa_gLock_usr3_Mutex CMaaMutex
#endif


#ifdef _WIN32

typedef DWORD CMaaThreadIdType;
inline constexpr CMaaThreadIdType CMaaInvalidThreadId() noexcept { return (CMaaThreadIdType)0; }
#define CMaaGetCurrentThreadId GetCurrentThreadId
inline bool CMaaThreadIdsEqual(CMaaThreadIdType a, CMaaThreadIdType b) noexcept { return a == b; }
inline bool CMaaThreadExists(CMaaThreadIdType abThreadId) noexcept
{
    const HANDLE h = OpenThread(SYNCHRONIZE, FALSE, abThreadId);
    if (h)
    {
        CloseHandle(h);
    }
    return h;
}

#else

typedef pthread_t CMaaThreadIdType;
inline constexpr CMaaThreadIdType CMaaInvalidThreadId() noexcept { return (CMaaThreadIdType)0; }
#define CMaaGetCurrentThreadId pthread_self
inline bool CMaaThreadIdsEqual(CMaaThreadIdType a, CMaaThreadIdType /*valid*/b) noexcept { return a != CMaaInvalidThreadId() && pthread_equal(a, b); }
inline bool CMaaThreadExists(CMaaThreadIdType abThreadId) noexcept
{
    return !pthread_kill(abThreadId, 0);
}

#endif


#define DEFAULT_FAST_MUTEX_SPINS 4000
#define DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS 10

#define DEFAULT_R_MUTEX_SPINS DEFAULT_FAST_MUTEX_SPINS // 4000
#define DEFAULT_W_MUTEX_SPINS DEFAULT_FAST_MUTEX_SPINS // 4000
#define DEFAULT_R_MUTEX_TRY_NEXT_SPINS1 1
#define DEFAULT_W_MUTEX_TRY_NEXT_SPINS1 1

#ifdef _WIN32
class CMaaCsFastMutex
{
    CRITICAL_SECTION m_cs;
public:
    CMaaCsFastMutex(int Spins = DEFAULT_FAST_MUTEX_SPINS) noexcept
    {
        //InitializeCriticalSectionEx(&m_cs, Spins, CRITICAL_SECTION_NO_DEBUG_INFO);
        InitializeCriticalSection(&m_cs);
        SetCriticalSectionSpinCount(&m_cs, Spins);
    }
    ~CMaaCsFastMutex() noexcept
    {
        DeleteCriticalSection(&m_cs);
    }
    void Lock() noexcept
    {
        EnterCriticalSection(&m_cs);
    }
    bool TryLock() noexcept
    {
        return TryEnterCriticalSection(&m_cs);
    }
    void UnLock() noexcept
    {
        LeaveCriticalSection(&m_cs);
    }
};
#endif

//std::thread::id NoThrId;
//std::thread::id t_id = std::this_thread::get_id();
//std::this_thread::yield();

#ifdef TOOLSLIB_SINGLE_THREAD
#define CMaaAtomicFastMutex CMaaAtomicFastMutexST
#define CMaaAtomicFastMutexW CMaaAtomicFastMutexWST
#endif
class CMaaAtomicFastMutex
{
    static constexpr CMaaThreadIdType InvalidThrId{ CMaaInvalidThreadId() };

    std::atomic<int> m_Lock;
    std::atomic<CMaaThreadIdType> m_ThreadId;

    //const int m_Spins;
public:
    CMaaAtomicFastMutex(/*int Spins = DEFAULT_FAST_MUTEX_SPINS*/) noexcept;
    ~CMaaAtomicFastMutex();
    _dword Lock() noexcept;
    _dword Lock_us(_qword us) noexcept;
    _dword Lock(_dword ms) noexcept;
    bool TryLock() noexcept;
    int UnLock() noexcept;
    void AddRef() const noexcept;
    int UnRef() const noexcept;
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept;
    _dword Lock(const char* txt) noexcept;
    _dword LockF(const char* file, int line) noexcept;
    int UnLockF(const char* file, int line) noexcept;
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept;

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};
class CMaaAtomicFastMutexW
{
    static constexpr CMaaThreadIdType InvalidThrId{ CMaaInvalidThreadId() };

    std::atomic<int> m_Lock;
    std::atomic<CMaaThreadIdType> m_ThreadId;

    //const int m_Spins;
public:
    CMaaAtomicFastMutexW(/*int Spins = DEFAULT_FAST_MUTEX_SPINS*/) noexcept;
    ~CMaaAtomicFastMutexW();
    _dword Lock() noexcept;
    _dword Lock_us(_qword us) noexcept;
    _dword Lock(_dword ms) noexcept;
    bool TryLock() noexcept;
    int UnLock() noexcept;
    void AddRef() const noexcept;
    int UnRef() const noexcept;
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept;
    _dword Lock(const char* txt) noexcept;
    _dword LockF(const char* file, int line) noexcept;
    int UnLockF(const char* file, int line) noexcept;
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept;
    int IsLocked() const noexcept { return m_Lock;  }

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};
#ifdef TOOLSLIB_SINGLE_THREAD
#undef CMaaAtomicFastMutexW
#undef CMaaAtomicFastMutex
#endif

#ifdef TOOLSLIB_SINGLE_THREAD

class CMaaAtomicFastMutex
{
    int m_Lock;
public:
    CMaaAtomicFastMutex(int Spins = DEFAULT_FAST_MUTEX_SPINS) noexcept { m_Lock = 0; }
    ~CMaaAtomicFastMutex() {}
    _dword Lock() noexcept { ++m_Lock; return 0; }
    _dword Lock_us(_qword us) noexcept { ++m_Lock; return 0; }
    _dword Lock(_dword ms) noexcept { ++m_Lock; return 0; }
    bool TryLock() noexcept { ++m_Lock; return true; }
    int UnLock() noexcept { return --m_Lock; }
    void AddRef() const noexcept {}
    int UnRef() const noexcept { return 1; }
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept { return false; }
    _dword Lock(const char* txt) noexcept { return Lock(); }
    _dword LockF(const char* file, int line) noexcept { return Lock(); }
    int UnLockF(const char* file, int line) noexcept { return UnLock(); }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}
    int IsLocked() const noexcept { return m_Lock; }

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaAtomicFastMutexW
{
    int m_Lock;
public:
    CMaaAtomicFastMutexW(int Spins = DEFAULT_FAST_MUTEX_SPINS) noexcept { m_Lock = 0; }
    ~CMaaAtomicFastMutexW() {}
    _dword Lock() noexcept { ++m_Lock; return 0; }
    _dword Lock_us(_qword us) noexcept { ++m_Lock; return 0; }
    _dword Lock(_dword ms) noexcept { ++m_Lock; return 0; }
    bool TryLock() noexcept { ++m_Lock; return true; }
    int UnLock() noexcept { return --m_Lock; }
    void AddRef() const noexcept {}
    int UnRef() const noexcept { return 1; }
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept { return false; }
    _dword Lock(const char* txt) noexcept { return Lock(); }
    _dword LockF(const char* file, int line) noexcept { return Lock(); }
    int UnLockF(const char* file, int line) noexcept { return UnLock(); }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}
    int IsLocked() const noexcept { return m_Lock; }

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

#define CMaaStdRecursiveMutex CMaaAtomicFastMutex

class CMaaFastMutex
{
    int m_Lock;
public:
    CMaaFastMutex() noexcept {m_Lock = 0;}
    ~CMaaFastMutex() {}
    _dword Lock() noexcept {++m_Lock; return 0;}
    _dword Lock_us(_qword us) noexcept {++m_Lock; return 0;}
    _dword Lock(_dword ms) noexcept {++m_Lock; return 0;}
    bool TryLock() noexcept {++m_Lock; return true;}
    int UnLock() noexcept {return --m_Lock;}
    void AddRef() const noexcept {}
    int UnRef() const noexcept {return 1;}
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept {return false;}
    _dword Lock(const char* txt) noexcept {return Lock();}
    _dword LockF(const char* file, int line) noexcept {return Lock();}
    int UnLockF(const char* file, int line) noexcept {return UnLock();}
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}
    int IsLocked() const noexcept { return m_Lock; }

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaFastMutexCv
{
    int m_Lock;
public:
    CMaaFastMutexCv() noexcept { m_Lock = 0; }
    ~CMaaFastMutexCv() {}
    _dword Lock() noexcept { ++m_Lock; return 0; }
    _dword Lock_us(_qword us) noexcept { ++m_Lock; return 0; }
    _dword Lock(_dword ms) noexcept { ++m_Lock; return 0; }
    bool TryLock() noexcept { ++m_Lock; return true; }
    int UnLock() noexcept { return --m_Lock; }
    void AddRef() const noexcept {}
    int UnRef() const noexcept { return 1; }
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept { return false; }
    _dword Lock(const char* txt) noexcept { return Lock(); }
    _dword LockF(const char* file, int line) noexcept { return Lock(); }
    int UnLockF(const char* file, int line) noexcept { return UnLock(); }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaAtomicFastMutex1 // simple, fast mutex // can be non recursive
{
    mutable int m_Lock;
public:
    constexpr CMaaAtomicFastMutex1(/*int Spins = 10000*/) noexcept {m_Lock = 0;}
    constexpr ~CMaaAtomicFastMutex1() {}
    _dword Lock() noexcept {++m_Lock; return 0;}
    bool TryLock() noexcept {++m_Lock; return true;}
    int UnLock() noexcept {return --m_Lock;}
    void AddRef() const noexcept {}
    int UnRef() const noexcept {return 1;}
#ifdef _WIN32_000
    DWORD Lock(DWORD to) noexcept {return Lock();}
#endif
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept {return false;}
    _dword Lock(const char* txt) noexcept {return Lock();}
    _dword LockF(const char* file, int line) noexcept {return Lock();}
    int UnLockF(const char* file, int line) noexcept {return UnLock();}
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaAtomicFastMutex0 // simplest, fast mutex // non recursive
{
    //mutable int m_Lock;
public:
    constexpr CMaaAtomicFastMutex0() noexcept {} // m_Lock = 0;
    constexpr ~CMaaAtomicFastMutex0() {}
    _dword Lock() noexcept { /*++m_Lock;*/ return 0; }
    bool TryLock() noexcept { /*++m_Lock;*/ return true; }
    int UnLock() noexcept { return 0; /*--m_Lock;*/ }
    void AddRef() const noexcept {}
    int UnRef() const noexcept { return 1; }
#ifdef _WIN32_000
    DWORD Lock(DWORD to) noexcept { return Lock(); }
#endif
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept { return false; }
    _dword Lock(const char* txt) noexcept { return Lock(); }
    _dword LockF(const char* file, int line) noexcept { return Lock(); }
    int UnLockF(const char* file, int line) noexcept { return UnLock(); }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};
#define CMaaAtomicFastMutex0W CMaaAtomicFastMutex0
#else

class CMaaStdRecursiveMutex
{
    std::recursive_mutex m;
    int n = 0;
public:
    CMaaStdRecursiveMutex() noexcept {}
    ~CMaaStdRecursiveMutex() {}

    _dword Lock() noexcept { m.lock(); ++n; return 0; }
    //_dword Lock_us(_qword us) noexcept;
    //_dword Lock(_dword ms) noexcept;
    bool TryLock() noexcept { if (m.try_lock()) { ++n; return true; } return false; }
    int UnLock() noexcept { int r = --n; m.unlock(); return r; }
    void AddRef() const noexcept {}
    int UnRef() const noexcept { return 1; }
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept { return false; }
    _dword Lock(const char* txt) noexcept { return Lock(); }
    _dword LockF(const char* file, int line) noexcept { return Lock(); }
    int UnLockF(const char* file, int line) noexcept { return UnLock(); }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaFastMutex
{
    static constexpr CMaaThreadIdType InvalidThrId{ CMaaInvalidThreadId() };

    std::atomic<int> m_Lock;
    std::atomic<CMaaThreadIdType> m_ThreadId;

    //const int m_Spins;
public:
    CMaaFastMutex() noexcept;
    ~CMaaFastMutex();
    _dword Lock() noexcept;
    _dword Lock_us(_qword us) noexcept;
    _dword Lock(_dword ms) noexcept;
    bool TryLock() noexcept;
    int UnLock() noexcept;
    void AddRef() const noexcept;
    int UnRef() const noexcept;
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept;
    _dword Lock(const char* txt) noexcept;
    _dword LockF(const char* file, int line) noexcept;
    int UnLockF(const char* file, int line) noexcept;
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept;

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

#if 0
class CMaaFastMutexCv
{
    CMaaFastMutex m_Mutex;
    std::mutex m;
    std::condition_variable m_cv;
public:
    CMaaFastMutexCv() noexcept;
    ~CMaaFastMutexCv();
    _dword Lock() noexcept;
    _dword Lock_us(_qword us) noexcept;
    _dword Lock(_dword ms) noexcept;
    bool TryLock() noexcept;
    int UnLock() noexcept;
    void AddRef() const noexcept;
    int UnRef() const noexcept;
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept;
    _dword Lock(const char* txt) noexcept;
    _dword LockF(const char* file, int line) noexcept;
    int UnLockF(const char* file, int line) noexcept;
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept;

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};
#endif

class CMaaAtomicFastMutex1 // simple, fast mutex // can be non recursive
{
    static constexpr CMaaThreadIdType InvalidThrId{ CMaaInvalidThreadId() };

    mutable std::atomic<int> m_Lock;
    mutable std::atomic<CMaaThreadIdType> m_ThreadId;

    //const int m_Spins;
public:
    constexpr CMaaAtomicFastMutex1() noexcept
    :   m_Lock(0),
        m_ThreadId(InvalidThrId)
    {
    }
    constexpr ~CMaaAtomicFastMutex1() {}
    _dword Lock() noexcept;
    bool TryLock() noexcept;
    int UnLock() noexcept;
    constexpr void AddRef() const noexcept {}
    constexpr int UnRef() const noexcept { return 1; }
#ifdef _WIN32_000
    DWORD Lock(DWORD to) noexcept
    {
        if  (!to)
        {
            return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
        }
        return Lock();
    }
#endif
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept;
    _dword Lock(const char* txt) noexcept;
    _dword LockF(const char* file, int line) noexcept;
    int UnLockF(const char* file, int line) noexcept;
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept;

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaAtomicFastMutex0 // the simplest, fast mutex // non recursive
{
    mutable std::atomic_flag m_Lock;
public:
    constexpr CMaaAtomicFastMutex0() noexcept
    :   m_Lock ATOMIC_FLAG_INIT
    {
    }
    constexpr ~CMaaAtomicFastMutex0() {}
    void lock() noexcept
    {
        while (m_Lock.test_and_set(std::memory_order_acquire))
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
            // Since C++20, locks can be acquired only after notification in the unlock,
            // avoiding any unnecessary spinning.
            // Note that even though wait guarantees it returns only after the value has
            // changed, the lock is acquired after the next condition check.
            //m_Lock.wait(true, std::memory_order_relaxed)
#endif
            ;
    }
    void unlock() noexcept
    {
        m_Lock.clear(std::memory_order_release);
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
        //m_Lock.notify_one();
#endif
    }
    bool try_lock() noexcept
    {
        return !m_Lock.test_and_set(std::memory_order_acquire);
    }
    _dword Lock() noexcept
    {
        lock();
#ifdef _WIN32
        return WAIT_OBJECT_0;
#else
        return 0;
#endif
    }
    bool TryLock() noexcept
    {
        return try_lock();
    }
    int UnLock() noexcept
    {
        unlock();
        return 0;
    }
    constexpr void AddRef() const noexcept {}
    constexpr int UnRef() const noexcept { return 1; }
#ifdef _WIN32_000
    DWORD Lock(DWORD to) noexcept
    {
        if (!to)
        {
            return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
        }
        return Lock();
    }
#endif
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept
    {
        return false;
    }
    _dword Lock(const char* txt) noexcept
    {
        return Lock();
    }
    _dword LockF(const char* file, int line) noexcept
    {
        return Lock();
    }
    int UnLockF(const char* file, int line) noexcept
    {
        return UnLock();
    }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}
};

class CMaaAtomicFastMutex0W // the simplest, fast mutex // non recursive // wait() + notify_one() version
{
    mutable std::atomic_flag m_Lock;
public:
    constexpr CMaaAtomicFastMutex0W() noexcept
        : m_Lock ATOMIC_FLAG_INIT
    {
    }
    constexpr ~CMaaAtomicFastMutex0W() {}
    void lock() noexcept
    {
        while (m_Lock.test_and_set(std::memory_order_acquire))
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
            // Since C++20, locks can be acquired only after notification in the unlock,
            // avoiding any unnecessary spinning.
            // Note that even though wait guarantees it returns only after the value has
            // changed, the lock is acquired after the next condition check.
            m_Lock.wait(true, std::memory_order_relaxed)
#endif
            ;
    }
    void unlock() noexcept
    {
        m_Lock.clear(std::memory_order_release);
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
        m_Lock.notify_one();
#endif
    }
    bool try_lock() noexcept
    {
        return !m_Lock.test_and_set(std::memory_order_acquire);
    }
    _dword Lock() noexcept
    {
        lock();
#ifdef _WIN32
        return WAIT_OBJECT_0;
#else
        return 0;
#endif
    }
    bool TryLock() noexcept
    {
        return try_lock();
    }
    int UnLock() noexcept
    {
        unlock();
        return 0;
    }
    constexpr void AddRef() const noexcept {}
    constexpr int UnRef() const noexcept { return 1; }
#ifdef _WIN32_000
    DWORD Lock(DWORD to) noexcept
    {
        if (!to)
        {
            return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
        }
        return Lock();
    }
#endif
    bool GetLockHolder(int x, char* txt, int buffer_len) const noexcept
    {
        return false;
    }
    _dword Lock(const char* txt) noexcept
    {
        return Lock();
    }
    _dword LockF(const char* file, int line) noexcept
    {
        return Lock();
    }
    int UnLockF(const char* file, int line) noexcept
    {
        return UnLock();
    }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void FlushLog(bool bForced) const noexcept {}
};
//#define CMaaAtomicFastMutex0 CMaaAtomicFastMutex0W // can make cpu usage lower and can be something slower
#endif

template<class T = CMaaAtomicFastMutex> class CMaaAtomicFastMutexLocker
{
    T& m;
public:
    CMaaAtomicFastMutexLocker(T& x) noexcept
    :   m(x)
    {
        Lock();
    }
    ~CMaaAtomicFastMutexLocker()
    {
        UnLock();
    }
    void Lock() noexcept
    {
        m.Lock();
    }
    void UnLock() noexcept
    {
        m.UnLock();
    }
};

template<class T = CMaaAtomicFastMutex> class CMaaManualAtomicFastMutexLocker
{
    T& m;
    int n;
public:
    CMaaManualAtomicFastMutexLocker(T& x) noexcept
    :   m(x),
        n(0)
    {
    }
    ~CMaaManualAtomicFastMutexLocker()
    {
        for (int i = n; i--; )
        {
            m.UnLock();
        }
        // n = 0;
    }
    void Lock() noexcept
    {
        m.Lock();
        ++n;
    }
    void UnLock() noexcept
    {
        if (n)
        {
            --n;
            m.UnLock();
        }
    }
    int IsLocked() const noexcept
    {
        return n;
    }
};

template<class T = CMaaAtomicFastMutex> class CMaaManualMutexLocker1
{
    T& m;
    bool b;
public:
    CMaaManualMutexLocker1(T& x) noexcept
    :   m(x),
        b(false)
    {
    }
    ~CMaaManualMutexLocker1()
    {
        if (b)
        {
            m.UnLock();
        }
        // b = false;
    }
    void Lock() noexcept
    {
        m.Lock();
        b = true;
    }
    void UnLock() noexcept
    {
        if (b)
        {
            b = false;
            m.UnLock();
        }
    }
    int IsLocked() const noexcept
    {
        return b;
    }
};

template<class T = CMaaAtomicFastMutex> class CMaaManualMutexUnLocker1
{
    T& m;
    bool b;
public:
    CMaaManualMutexUnLocker1(T& x) noexcept
    :   m(x),
        b(false)
    {
    }
    ~CMaaManualMutexUnLocker1()
    {
        if (b)
        {
            m.Lock();
        }
        // b = false;
    }
    void UnLock() noexcept
    {
        b = true;
        m.UnLock();
    }
    void Lock() noexcept
    {
        if (b)
        {
            m.Lock();
            b = false;
        }
    }
    int IsLocked() const noexcept
    {
        return !b;
    }
};

#ifdef _WIN32
#define CMaaLiteMutex CMaaAtomicFastMutex0
//#define CMaaLiteMutex CMaaMutex2
#else
#define CMaaLiteMutex CMaaAtomicFastMutex0
//#define CMaaLiteMutex CMaaMutex2
#endif

#ifdef _WIN32

#if 0
class CMaaLiteMutex
{
    HANDLE m_hMutex;
    int m_RefCount;
public:
    /*
     CMaaLiteMutex(BOOL fInitiallyLocked = FALSE);
     ~CMaaLiteMutex();
     operator HANDLE() const
     {
          return m_hMutex;
     }
     void Lock();
     void UnLock();
     */
    CMaaLiteMutex(BOOL fInitiallyLocked = FALSE) noexcept
    {
        m_RefCount = 1;
        m_hMutex = CreateMutex(nullptr, fInitiallyLocked, nullptr);
    }
    ~CMaaLiteMutex() noexcept
    {
        CloseHandle(m_hMutex);
    }
    void Lock() noexcept
    {
        DWORD x = WaitForSingleObject(m_hMutex, INFINITE);
        if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            m_RefCount++;
        }
    }
    int UnLock() noexcept
    {
        int n = --m_RefCount;
        if  (!ReleaseMutex(m_hMutex))
        {
            n = ++m_RefCount;
        }
        if  (!n)
        {
            delete this;
            return -1;
        }
        return n - 1;
    }
    void AddRef() noexcept
    {
        Lock();
        ++m_RefCount;
        UnLock();
    }
    int UnRef() noexcept
    {
        Lock();
        --m_RefCount;
        return UnLock();
    }
};
#endif

//#define CMaaMutex CMaaWin32Lock
#define CMaaMutex1 CMaaMutex

//#pragma message("defining CTooDLink and CMaaDList")
#define CTooDLink CMaaDLink
#define CTooDList CMaaDList

class CMaaMutex;

//extern CMaaMutex * gpAtomicLock;

#else

#define CMaaWin32Lock CMaaMutex
#define CMaaWin32Semaphore CMaaSemaphore
//#define CMaaMutex1 CMaaMutex

#ifdef DEBUG_MTX

#define LockM() LockF(__FILE__, __LINE__)
#define UnLockM() UnLockF(__FILE__, __LINE__)

#else

#define LockM Lock
#define UnLockM UnLock

#endif


//struct eExitThread
//{
//};

#define CTooDLink CMaaDLink
#define CTooDList CMaaDList

void SetThreadName(_dword ThreadId, const char * Name) noexcept;

class CMaaMutex1
{
protected:
    pthread_mutex_t m;

    friend class CMaaEvent;
    friend class CMaaTimedMutex;

    int m_RefCount;
public:
    CMaaMutex1(bool bRecursive = false) noexcept;
    ~CMaaMutex1() noexcept;

    _dword Lock() noexcept;
    //_dword LockF(const char * f, int l);
    int UnLock() noexcept;
    //bool UnLockF(const char * f, int l) noexcept {}
    void StillLocked(const char* SrcFile, int SrcLine) noexcept {}
    void AddRef() noexcept;
    int UnRef() noexcept;
    bool TryLock() noexcept;

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

class CMaaMutex2 : protected CMaaMutex1
{
    int n;
#ifdef _DEBUG_MTX
    const char * ff[20], * ff2[20];
    int ll[20], ll2[20];
#endif

public:
    CMaaMutex2() noexcept;
    ~CMaaMutex2() noexcept;
    //void Lock();
    _dword LockF(const char* f, int l) noexcept;
    int UnLockF(const char * f, int l) noexcept;
#ifndef DEBUG_MTX
    // mtx.Lock/UnLock is 130x faster under linux64(not timed version) rather than win64
    _dword Lock() noexcept { return CMaaMutex1::Lock(); }
    int UnLock() noexcept { return CMaaMutex1::UnLock(); }
    bool TryLock() noexcept { return CMaaMutex1::TryLock(); }

    void lock() noexcept { CMaaMutex1::lock(); }
    void unlock() noexcept { CMaaMutex1::unlock(); }
    bool try_lock() noexcept { return CMaaMutex1::try_lock(); }
#endif
    int GetDepthEx() noexcept;
    bool GetLockHolder(int x, char * buffer, int buffer_len) noexcept;
    int GetRefCount() noexcept;
    void StillLocked(const char* SrcFile, int SrcLine) noexcept;
    void AddRef() noexcept { CMaaMutex1::AddRef(); }
    int UnRef() noexcept { return CMaaMutex1::UnRef(); }
};

//2022 extern CMaaMutex1 gAtomicLock;

#if 0 //ndef TOOLSLIB_MAX_SPEED
class CMaaLiteMutex : public CMaaMutex1
{
public:
    CMaaLiteMutex(bool fInitiallyLocked = false) noexcept
    :   CMaaMutex1(true)
    {
        if  (fInitiallyLocked)
        {
            Lock();
        }
    }
    ~CMaaLiteMutex() noexcept
    {
    }
};
#endif

#ifdef TOOLSLIB_SINGLE_THREAD
class CMaaSemaphore
{
    int m_Count;
public:
    CMaaSemaphore(int MaxCount, int InitialCount = -1) noexcept {m_Count = InitialCount < 0 ? MaxCount : InitialCount;} // -1 is substituted by MaxCount
    ~CMaaSemaphore() noexcept;
    DWORD Lock(DWORD dwTimeToWait = (DWORD)-1) noexcept { --m_Count; return 0; }
    void UnLock(int Count = 1) noexcept { m_Count += Count; }
};
#else
class CMaaSemaphore
{
    sem_t m_hSemaphore;
    std::atomic<int> m_RefCount;
public:
    CMaaSemaphore(int MaxCount, int InitialCount = -1) noexcept; // -1 is substituted by MaxCount
    ~CMaaSemaphore() noexcept;
    operator sem_t() const noexcept
    {
        return m_hSemaphore;
    }
    //DWORD Lock(const char * txt);
    DWORD Lock(DWORD dwTimeToWait = (DWORD)-1) noexcept;
    //DWORD TryLock() noexcept;
    void UnLock(int Count = 1) noexcept;
};
#endif


#endif

//-----------------------------------------------

#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
inline CMaaLiteMutex* __GLock__AllocatorBasicLock(bool bInit = false) noexcept // allocator single lock
{
    static CMaaLiteMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
        gptr = TL_NEW_ CMaaLiteMutex;
        gptr->AddRef();
    }
    return gptr;
}
#else
inline CMaaLiteMutex* __GLock__AllocatorBasicLock(bool bInit = false) noexcept // allocator single lock
{
    if (bInit)
    {
        TL_DbgAlloc;
    }
    static constexpr CMaaLiteMutex g;
    return (CMaaLiteMutex*)&g;
}
#endif
#define gLock_lib (*__GLock__lib())
#define gLock_lib2 (*__GLock__lib2())
#define gLock_usr (*__GLock__usr())
#define gLock_usr2 (*__GLock__usr2())
#define gLock_usr3 (*__GLock__usr3())
#define gAllocatorBasicLock (*__GLock__AllocatorBasicLock())

// allocator multiple lock, need to call __GLock__LockFn((T *)nullptr) for each types T at startup or create static objects with looks needed to avoid race conditions and faults in multithread environment
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
template<class T> CMaaLiteMutex* __GLock__LockFn(T* obj, bool bInit = false) noexcept
{
    static CMaaLiteMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
        gptr = TL_NEW_ CMaaLiteMutex;
        gptr->AddRef();
    }
    return gptr;
}
#else
template<class T> CMaaLiteMutex* __GLock__LockFn(T* obj, bool bInit = false) noexcept
{
    if (bInit)
    {
        TL_DbgAlloc;
    }
    static constexpr CMaaLiteMutex g;
    return (CMaaLiteMutex*)&g;
}
#endif

/*
inline CMaa_gLock_lib_Mutex & Get_gLock() noexcept
{
    return gLock_lib;
}
*/

/*
inline CMaa_gLock_usr_Mutex& Get_gLock_usr() noexcept
{
    return gLock_usr;
}
*/


#define gLock gLock_lib
//#define gLock2 gLock_lib2

class CMaaString;

#ifndef TOOLSLIB_SINGLE_THREAD
class CMaaThread : public CMaaDLink
{
#ifdef _WIN32
    static unsigned __stdcall RunThread(void * Param);
#else
    // __unix__
    static void * RunThread(void * arg);

    //#define DWORD unsigned long
#define DWORD _dword

#endif
    unsigned Run0();
public:
#ifdef _WIN32
    HANDLE m_hThread;
    DWORD m_ThreadId;
    int m_Point;
    HANDLE m_hExit;
#else
    // __unix__
    pthread_attr_t m_attr;
    pthread_t m_Handle;
    int m_Point;
    bool m_bExit;
#endif

    CMaaThread() noexcept;
    virtual ~CMaaThread() = 0;

    bool Create() noexcept;
    unsigned Join() noexcept;

    virtual unsigned Run();
    virtual bool Exit(DWORD MaxWaitTime = 10000) noexcept;
    bool Wait(DWORD MaxWaitTime = 10000) noexcept;
    virtual void CheckPoint(DWORD WaitTime = 10);
    void SetPoint(int x = 0xaa5555aa) noexcept
    {
        m_Point = x;
    }
    void SetPriority(int pr, int sched_fifo = 2);

    //ADD_ALLOCATOR(CMaaThread)
};
#endif

#if 0
template <class T> class AutoDeref
{
    int m_CreatedPattern;
    T ** m_ptr;
    void (* pfn_)(int bCreate, void * param);
    void * param_;
public:
    AutoDeref(T ** ptr, bool /*bCreate*/ = true, void (* pfn)(int bCreate, void * param) = nullptr, void * param = nullptr)
    :   m_ptr(ptr),
        pfn_(pfn),
        param_(param)
    {
        m_CreatedPattern = 0x12345678;
        if  (!*m_ptr)
        {
            *m_ptr = TL_NEW T;
            if  (*m_ptr)
            {
                if  (pfn_)
                {
                    pfn_(1, param_);
                }
            }
            else
            {
                m_CreatedPattern = 0;
            }
        }
    }
    ~AutoDeref()
    {
        if  (*m_ptr)
        {
            if  ((*m_ptr)->UnRef() < 0)
            {
                *m_ptr = nullptr;
                if  (pfn_)
                {
                    pfn_(0, param_);
                }
            }
        }
        m_CreatedPattern = 0;
        m_ptr = nullptr;
    }
    bool IsDestoyed()
    {
        return m_CreatedPattern != 0x12345678;
    }
};
#endif


#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
inline CMaaLiteMutex* __GLock__Atomic(bool bInit = false) noexcept
{
    static CMaaLiteMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
        gptr = TL_NEW_ CMaaLiteMutex;
        gptr->AddRef();
    }
    return gptr;
}
#else
inline CMaaLiteMutex* __GLock__Atomic(bool bInit = false) noexcept
{
    if (bInit)
    {
        TL_DbgAlloc;
    }
    static constexpr CMaaLiteMutex g;
    return (CMaaLiteMutex*)&g;
}
#endif

#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
inline CMaaLiteMutex* __GLock__Atomic2(bool bInit = false) noexcept
{
    static CMaaLiteMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
        gptr = TL_NEW_ CMaaLiteMutex;
        gptr->AddRef();
    }
    return gptr;
}
#else
inline CMaaLiteMutex* __GLock__Atomic2(bool bInit = false) noexcept
{
    if (bInit)
    {
        TL_DbgAlloc;
    }
    static constexpr CMaaLiteMutex g;
    return (CMaaLiteMutex*)&g;
}
#endif
#define gLock_Atomic (*__GLock__Atomic())
#define gLock_Atomic2 (*__GLock__Atomic2())

/*
void AtomicLock() noexcept;// { __GLock__Atomic()->Lock(); }
void AtomicUnLock() noexcept;// { __GLock__Atomic()->UnLock(); }
void AtomicLock2() noexcept;// { __GLock__Atomic()->Lock(); }
void AtomicUnLock2() noexcept;// { __GLock__Atomic()->UnLock(); }
*/

#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
inline CMaaLiteMutex* __GLock__SyncAssignLockUpperLock(bool bInit = false) noexcept
{
    static CMaaLiteMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
        gptr = TL_NEW_ CMaaLiteMutex;
        gptr->AddRef();
    }
    return gptr;
}
#else
inline CMaaLiteMutex* __GLock__SyncAssignLockUpperLock(bool bInit = false) noexcept
{
    if (bInit)
    {
        TL_DbgAlloc;
    }
    static constexpr CMaaLiteMutex g;
    return (CMaaLiteMutex*)&g;
}
#endif


#if 0
// CAtomicLocker Locker; is replaced by CMaaWin32Locker Locker(gLock_Atomic);
class CAtomicLocker // for internal non-recursive atomic lock
{
    bool m_bLocked;
public:
    CAtomicLocker(bool bLock = false) noexcept
    {
        m_bLocked = false;
        if  (bLock)
        {
            Lock();
        }
    }
    ~CAtomicLocker()
    {
        if  (m_bLocked)
        {
            UnLock();
        }
    }
    void Lock() noexcept
    {
        AtomicLock();
        m_bLocked = true;
    }
    void UnLock() noexcept
    {
        m_bLocked = false;
        AtomicUnLock();
    }
};

class CAtomicLocker2 // for internal non-recursive atomic lock2, CAtomicLocker2::Lock can be locked before CAtomicLocker::Lock
{
    bool m_bLocked;
public:
    CAtomicLocker2(bool bLock = false) noexcept
    {
        m_bLocked = false;
        if  (bLock)
        {
            Lock();
        }
    }
    ~CAtomicLocker2()
    {
        if  (m_bLocked)
        {
            UnLock();
        }
    }
    void Lock() noexcept
    {
        AtomicLock2();
        m_bLocked = true;
    }
    void UnLock() noexcept
    {
        m_bLocked = false;
        AtomicUnLock2();
    }
};
#endif

/*
#define Lock_Function(name, T, fn, param) \
int name(bool bLock) \
{ \
     static T *gptr = nullptr; \
     static AutoDeref<T> tmp(&gptr, true, fn, param); \
     typedef void (* tpfn_)(int bCreate, void * param_); \
     static tpfn_ pfn_ = fn; \
     static void * param_ = param; \
     if   (bLock) \
     { \
          if   (!gptr) \
          { \
               gptr = TL_NEW T; \
               if   (gptr && pfn_) \
               { \
                    pfn_(1, param_); \
               } \
          } \
          gptr->Lock(); return 1; \
     } \
     else \
     { \
          int n = gptr->UnLock(); \
          if   (n < 0) \
          { \
               gptr = nullptr; if (pfn_) pfn_(0, param_); \
          } \
          if   (n == 0 && tmp.IsDestoyed()) \
          { \
               delete gptr; \
               gptr = nullptr;  if (pfn_) pfn_(0, param_); \
          } \
          return gptr ? 1 : -1; \
     } \
     return 0; \
}
*/

//=======================================
#ifdef _WIN32
bool fnMaaSafeMakeStatic(LONG volatile *pFlag) noexcept;
void fnMaaSafeStaticMade(LONG volatile *pFlag) noexcept;
#else
bool fnMaaSafeMakeStatic(int volatile *pFlag, CMaaMutex &m) noexcept;
void fnMaaSafeStaticMade(int volatile *pFlag, CMaaMutex &m) noexcept;
#endif

class CMaaSafeMakeStatic
{
#ifdef _WIN32
    LONG volatile *m_pFlag;
#else
    //static CMaaMutex s_Mutex;
    int volatile *m_pFlag;
#endif
    int m_f;
public:
#ifdef _WIN32
    CMaaSafeMakeStatic(LONG volatile *pFlag) noexcept;
#else
    CMaaSafeMakeStatic(int volatile *pFlag) noexcept;
#endif
    bool Make() noexcept;
    bool ToInit_() noexcept;
    ~CMaaSafeMakeStatic() noexcept;
    bool Done() noexcept;
};

#define MAA_CACHE_LINE  32 // min.: 4
#ifdef _WIN32
#define MAA_CACHE_ALIGN __declspec(align(MAA_CACHE_LINE))
#define MAA_STATIC_FLAG __declspec(align(MAA_CACHE_LINE)) static volatile LONG
#else
#define MAA_STATIC_FLAG static volatile int
#endif

#if 0
class CMaaStaticStringsAllocator
{
    int m_n;
    CMaaPtrAE_<CMaaString *, 1> m;
    bool m_bDontDel;
public:
    CMaaStaticStringsAllocator(int N = 250);
    CMaaString & Alloc(const char * str);
    CMaaString & Alloc(CMaaString str);
    ~CMaaStaticStringsAllocator();
    void SetDontDel(bool f);
};
extern CMaaStaticStringsAllocator gStaticStringsAllocator;
#endif

struct GLocks
{
    CMaa_gLock_lib_Mutex* m_gLock_lib;
    CMaaMutex* m_gLock_lib2;
    CMaa_gLock_usr_Mutex* m_gLock_usr;
    CMaaMutex* m_gLock_usr2;
    CMaa_gLock_usr3_Mutex* m_gLock_usr3;
    CMaaLiteMutex* m_gAllocatorBasicLock;
    CMaaLiteMutex* m_Atomic;
    CMaaLiteMutex* m_Atomic2;
    CMaaMutex2 * m_gVpnLock = nullptr;
    //CMaaStdRecursiveMutex* m_gVpnLock = nullptr;
    std::atomic<int> gRW_RErrors, gRW_WErrors, gRW_WRErrors, gRW_RWErrors, gRW_WUErrors, gRW_RUErrors;

    GLocks();
};

extern GLocks gGLocks;
