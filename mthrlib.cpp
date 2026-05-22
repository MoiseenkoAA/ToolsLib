
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/mthrlib.cpp

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

//#include "stdafx.h"
#include "perm.h"
#ifdef _WIN32
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif
#else
#include "temp.h"
#endif

GLocks gGLocks;

GLocks::GLocks()
:   gRW_RErrors(0), gRW_WErrors(0), gRW_WRErrors(0), gRW_RWErrors(0), gRW_WUErrors(0), gRW_RUErrors(0)
{
    m_Atomic = __GLock__Atomic(true);
    m_Atomic2 = __GLock__Atomic2(true);
    m_gLock_lib = __GLock__lib(true);
    m_gLock_lib2 = __GLock__lib2(true);
    m_gLock_usr = __GLock__usr(true);
    m_gLock_usr2 = __GLock__usr2(true);
    m_gLock_usr3 = __GLock__usr3(true);
    m_gAllocatorBasicLock = __GLock__AllocatorBasicLock(true);
}

#ifdef _WIN32
#ifndef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
#pragma message( "Compiling woth no TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY" )
#else
//#pragma message( "Compiling with TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY" )
#endif
#endif

#ifdef _WIN32
#ifndef TOOLSLIB_KEEP_GLOBALS2_IN_MEMORY
#pragma message( "Compiling woth no TOOLSLIB_KEEP_GLOBALS2_IN_MEMORY" )
#else
//#pragma message( "Compiling with TOOLSLIB_KEEP_GLOBALS2_IN_MEMORY" )
#endif
#endif

#ifdef _WIN32
#ifndef TOOLSLIB_KEEP_GLOBALS_IN_MEMORY
#pragma message( "Compiling woth no TOOLSLIB_KEEP_GLOBALS_IN_MEMORY" )
#else
//#pragma message( "Compiling with TOOLSLIB_KEEP_GLOBALS_IN_MEMORY" )
#endif
#endif

#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
#ifdef TOOLSLIB_SINGLE_THREAD
_qword gHashTablesAllocSize = 0;
#else
std::atomic<_qword> gHashTablesAllocSize = 0;
#endif
#endif


#ifdef _WIN32
/*
CMaaLiteMutex::CMaaLiteMutex(BOOL fInitiallyLocked)
{
     m_hMutex = CreateMutex(nullptr, fInitiallyLocked, nullptr);
}
CMaaLiteMutex::~CMaaLiteMutex()
{
     CloseHandle(m_hMutex);
}
void CMaaLiteMutex::Lock()
{
     WaitForSingleObject(m_hMutex, INFINITE);
}
void CMaaLiteMutex::UnLock()
{
     ReleaseMutex(m_hMutex);
}
*/
#else

#endif

//===========================================================
#ifdef TOOLSLIB_SINGLE_THREAD
#define CMaaAtomicFastMutex CMaaAtomicFastMutexST
#define CMaaAtomicFastMutexW CMaaAtomicFastMutexWST
#endif
CMaaAtomicFastMutex::CMaaAtomicFastMutex(/*int Spins*/) noexcept
:   m_Lock(0),
    m_ThreadId(InvalidThrId)
    //, m_Spins(/*CMaaGetCpuCount() > 1 &&*/ Spins > 0 ? Spins : 1)
{
    //TL_DbgAlloc;
}
CMaaAtomicFastMutex::~CMaaAtomicFastMutex()
{
    //if  (m_Lock.load(std::TL_memory_order_acquire) != 0)
    //{
        // error: locked
    //}
}
_dword CMaaAtomicFastMutex::Lock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if  (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        int s = DEFAULT_FAST_MUTEX_SPINS; // m_Spins;
        while(1)
        {
            int y = 0;
            if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (--s)
            {
                continue;
            }
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            s = DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS; // m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;

            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if  (abThreadId != InvalidThrId)
            {
                if  (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
                // SwitchToThread(); // win32,win64
                // sched_yield(); // linux
            }
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
_dword CMaaAtomicFastMutex::Lock(_dword ms) noexcept
{
    if  (ms == INFINITE)
    {
        return Lock();
    }
    if  (!ms)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    return Lock_us(1000 * (_uqword)ms);
}
_dword CMaaAtomicFastMutex::Lock_us(_qword us) noexcept
{
    if  (us == -1)
    {
        return Lock();
    }
    if  (!us)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    const CMaaTime &tt = gHRTime;
    const _uqword nxt = tt.GetNextTime(us);
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        int s = DEFAULT_FAST_MUTEX_SPINS; // m_Spins;
        while(1)
        {
            int y = 0;
            if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            const bool b = (tt.GetCounter() >= nxt);
            if (--s && !b)
            {
                continue;
            }
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if  (b || (tt.GetCounter() >= nxt))
            {
                return WAIT_TIMEOUT;
            }
            s = DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS; // m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;

            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if  (abThreadId != InvalidThrId)
            {
                if (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
                // SwitchToThread(); // win32,win64
                // sched_yield(); // linux
            }
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
bool CMaaAtomicFastMutex::TryLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if (CMaaThreadIdsEqual(abThreadId, ThreadId))
    {
        ++m_Lock;
        return true;
    }
    int y = 0;
    if  (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
    {
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
        return true;
    }
    //abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if  (abThreadId != InvalidThrId)
    {
        if (!CMaaThreadExists(abThreadId))
        {
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                {
                    return true; // WAIT_OBJECT_0;
                }
            }
            else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                m_Lock.store(1, std::TL_memory_order_release);
                return true; // WAIT_ABANDONED;
            }
        }
    }
    return false;
}
int CMaaAtomicFastMutex::UnLock() noexcept
{
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId()))
    {
        if  (m_Lock.load(std::TL_memory_order_acquire) == 1)
        {
            m_ThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        return --m_Lock;
    }
    return -1;
}
void CMaaAtomicFastMutex::AddRef() const noexcept
{
    /*
    Lock();
    ++m_RefCount;
    UnLock();
    */
}
int CMaaAtomicFastMutex::UnRef() const noexcept
{
    /*
    Lock();
    --m_RefCount;
    return UnLock();
    */
    return 1;
}
bool CMaaAtomicFastMutex::GetLockHolder(int x, char* txt, int buffer_len) const noexcept
{
    return false;
}
_dword CMaaAtomicFastMutex::Lock(const char* txt) noexcept
{
    return Lock();
}
_dword CMaaAtomicFastMutex::LockF(const char* file, int line) noexcept
{
    return Lock();
}
int CMaaAtomicFastMutex::UnLockF(const char* file, int line) noexcept
{
    return UnLock();
}
void CMaaAtomicFastMutex::FlushLog(bool bForced) const noexcept
{
}


CMaaAtomicFastMutexW::CMaaAtomicFastMutexW(/*int Spins*/) noexcept
:   m_Lock(0),
    m_ThreadId(InvalidThrId)
    //, m_Spins(/*CMaaGetCpuCount() > 1 &&*/ Spins > 0 ? Spins : 1)
{
    //TL_DbgAlloc;
}
CMaaAtomicFastMutexW::~CMaaAtomicFastMutexW()
{
    //if  (m_Lock.load(std::TL_memory_order_acquire) != 0)
    //{
        // error: locked
    //}
}
_dword CMaaAtomicFastMutexW::Lock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        //int s = DEFAULT_FAST_MUTEX_SPINS; // m_Spins;
        while (1)
        {
            int y = 0;
            //if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            //{
            //    break;
            //}
            //if (--s)
            //{
            //    continue;
            //}
            //y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            //s = DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS; // m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;

            m_Lock.wait(y, std::memory_order_relaxed);

#if 0
            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if (abThreadId != InvalidThrId)
            {
                if (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
                // SwitchToThread(); // win32,win64
                // sched_yield(); // linux
            }
#endif
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
_dword CMaaAtomicFastMutexW::Lock(_dword ms) noexcept
{
    if (ms == INFINITE)
    {
        return Lock();
    }
    if (!ms)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    return Lock_us(1000 * (_uqword)ms);
}
_dword CMaaAtomicFastMutexW::Lock_us(_qword us) noexcept
{
    if (us == -1)
    {
        return Lock();
    }
    if (!us)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    const CMaaTime& tt = gHRTime;
    const _uqword nxt = tt.GetNextTime(us);
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        int s = DEFAULT_FAST_MUTEX_SPINS; // m_Spins;
        while (1)
        {
            int y = 0;
            if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            const bool b = (tt.GetCounter() >= nxt);
            if (--s && !b)
            {
                continue;
            }
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (b || (tt.GetCounter() >= nxt))
            {
                return WAIT_TIMEOUT;
            }
            s = DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS; // m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;

            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if (abThreadId != InvalidThrId)
            {
                if (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
                // SwitchToThread(); // win32,win64
                // sched_yield(); // linux
            }
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
bool CMaaAtomicFastMutexW::TryLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if (CMaaThreadIdsEqual(abThreadId, ThreadId))
    {
        ++m_Lock;
        return true;
    }
    int y = 0;
    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
    {
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
        return true;
    }
    //abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if (abThreadId != InvalidThrId)
    {
        if (!CMaaThreadExists(abThreadId))
        {
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                {
                    return true; // WAIT_OBJECT_0;
                }
            }
            else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                m_Lock.store(1, std::TL_memory_order_release);
                return true; // WAIT_ABANDONED;
            }
        }
    }
    return false;
}
int CMaaAtomicFastMutexW::UnLock() noexcept
{
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId()))
    {
        if (m_Lock.load(std::TL_memory_order_acquire) == 1)
        {
            m_ThreadId.store(InvalidThrId, std::TL_memory_order_release);
            --m_Lock;
            m_Lock.notify_one();
        }
        else
        {
            return --m_Lock;
        }
    }
    return -1;
}
void CMaaAtomicFastMutexW::AddRef() const noexcept
{
    /*
    Lock();
    ++m_RefCount;
    UnLock();
    */
}
int CMaaAtomicFastMutexW::UnRef() const noexcept
{
    /*
    Lock();
    --m_RefCount;
    return UnLock();
    */
    return 1;
}
bool CMaaAtomicFastMutexW::GetLockHolder(int x, char* txt, int buffer_len) const noexcept
{
    return false;
}
_dword CMaaAtomicFastMutexW::Lock(const char* txt) noexcept
{
    return Lock();
}
_dword CMaaAtomicFastMutexW::LockF(const char* file, int line) noexcept
{
    return Lock();
}
int CMaaAtomicFastMutexW::UnLockF(const char* file, int line) noexcept
{
    return UnLock();
}
void CMaaAtomicFastMutexW::FlushLog(bool bForced) const noexcept
{
}
#ifdef TOOLSLIB_SINGLE_THREAD
#undef CMaaAtomicFastMutexW
#undef CMaaAtomicFastMutex
#endif
//===========================================================
#ifndef TOOLSLIB_SINGLE_THREAD
CMaaFastMutex::CMaaFastMutex(/*int Spins*/) noexcept
:   m_Lock(0),
    m_ThreadId(InvalidThrId)
    //, m_Spins(/*CMaaGetCpuCount() > 1 &&*/ Spins > 0 ? Spins : 1)
{
    //TL_DbgAlloc;
}
CMaaFastMutex::~CMaaFastMutex()
{
    //if  (m_Lock.load(std::TL_memory_order_acquire) != 0)
    //{
        // error: locked
    //}
}
_dword CMaaFastMutex::Lock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        //int s = m_Spins;
        while(1)
        {
            int y = 0;
            /*
            if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (--s)
            {
                continue;
            }
            y = 0;
            */
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            //s = m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;

            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if  (abThreadId != InvalidThrId)
            {
                if (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
            }
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
_dword CMaaFastMutex::Lock(_dword ms) noexcept
{
    if  (ms == INFINITE)
    {
        return Lock();
    }
    if  (!ms)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    return Lock_us(1000 * (_uqword)ms);
}
_dword CMaaFastMutex::Lock_us(_qword us) noexcept
{
    if  (us == -1)
    {
        return Lock();
    }
    if  (!us)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    const CMaaTime &tt = gHRTime;
    const _uqword nxt = tt.GetNextTime(us);
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        //int s = m_Spins;
        while(1)
        {
            int y = 0;
            /*
            if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            const bool b = (tt.GetCounter() >= nxt);
            if (--s && !b)
            {
                continue;
            }
            y = 0;
            */
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if  (/*b ||*/ (tt.GetCounter() >= nxt))
            {
                return WAIT_TIMEOUT;
            }
            //s = m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;

            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if  (abThreadId != InvalidThrId)
            {
                if (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
            }
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
bool CMaaFastMutex::TryLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if (CMaaThreadIdsEqual(abThreadId, ThreadId))
    {
        ++m_Lock;
        return true;
    }
    int y = 0;
    if  (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
    {
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
        return true;
    }
    //abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if  (abThreadId != InvalidThrId)
    {
        if (!CMaaThreadExists(abThreadId))
        {
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                {
                    return true; // WAIT_OBJECT_0;
                }
            }
            else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                m_Lock.store(1, std::TL_memory_order_release);
                return true; // WAIT_ABANDONED;
            }
        }
    }
    return false;
}
int CMaaFastMutex::UnLock() noexcept
{
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId()))
    {
        if  (m_Lock.load(std::TL_memory_order_acquire) == 1)
        {
            m_ThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        return --m_Lock;
    }
    return -1;
}
void CMaaFastMutex::AddRef() const noexcept
{
    /*
    Lock();
    ++m_RefCount;
    UnLock();
    */
}
int CMaaFastMutex::UnRef() const noexcept
{
    /*
    Lock();
    --m_RefCount;
    return UnLock();
    */
    return 1;
}
bool CMaaFastMutex::GetLockHolder(int x, char* txt, int buffer_len) const noexcept
{
    return false;
}
_dword CMaaFastMutex::Lock(const char* txt) noexcept
{
    return Lock();
}
_dword CMaaFastMutex::LockF(const char* file, int line) noexcept
{
    return Lock();
}
int CMaaFastMutex::UnLockF(const char* file, int line) noexcept
{
    return UnLock();
}
void CMaaFastMutex::FlushLog(bool bForced) const noexcept
{
}
//===========================================================

//===========================================================
#if 0
CMaaFastMutexCv::CMaaFastMutexCv() noexcept
{
}
CMaaFastMutexCv::~CMaaFastMutexCv()
{
    //if  (m_Lock.load(std::TL_memory_order_acquire) != 0)
    //{
        // error: locked
    //}
}
_dword CMaaFastMutexCv::Lock() noexcept
{
    std::unique_lock lk(m);
    m_cv.wait(lk, [this] { return TryLock(); });
    return WAIT_OBJECT_0;
}
_dword CMaaFastMutexCv::Lock(_dword ms) noexcept
{
    if (ms == INFINITE)
    {
        return Lock();
    }
    if (!ms)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    return Lock_us(1000 * (_uqword)ms);
}
_dword CMaaFastMutexCv::Lock_us(_qword us) noexcept
{
    if (us == -1)
    {
        return Lock();
    }
    if (!us)
    {
        return TryLock() ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    std::unique_lock lk(m);
    return m_cv.wait_for(lk, std::chrono::microseconds(us), [this] { return TryLock(); }) ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
}
bool CMaaFastMutexCv::TryLock() noexcept
{
    return m_Mutex.TryLock();
}
int CMaaFastMutexCv::UnLock() noexcept
{
    const int r = m_Mutex.UnLock();
    if (!r)
    {
        m_cv.notify_one();
    }
    return r;
}
void CMaaFastMutexCv::AddRef() const noexcept
{
    m_Mutex.AddRef();
}
int CMaaFastMutexCv::UnRef() const noexcept
{
    return m_Mutex.UnRef();
}
bool CMaaFastMutexCv::GetLockHolder(int x, char* txt, int buffer_len) const noexcept
{
    return m_Mutex.GetLockHolder(x, txt, buffer_len); // false;
}
_dword CMaaFastMutexCv::Lock(const char* txt) noexcept
{
    return Lock();
}
_dword CMaaFastMutexCv::LockF(const char* file, int line) noexcept
{
    return Lock();
}
int CMaaFastMutexCv::UnLockF(const char* file, int line) noexcept
{
    return UnLock();
}
void CMaaFastMutexCv::FlushLog(bool bForced) const noexcept
{
}
#endif
//===========================================================


#if 0
CMaaAtomicFastMutex1::CMaaAtomicFastMutex1(/*int Spins = DEFAULT_FAST_MUTEX_SPINS*/) noexcept
:   m_Lock(0),
    m_ThreadId(InvalidThrId)
    //, m_Spins(/*CMaaGetCpuCount() > 1 ?*/ DEFAULT_FAST_MUTEX_SPINS /*: 1*/)
{
    //TL_DbgAlloc;
}
CMaaAtomicFastMutex1::~CMaaAtomicFastMutex1()
{
    //if  (m_Lock.load(std::TL_memory_order_acquire) != 0)
    //{
        // error: locked
    //}
}
#endif
_dword CMaaAtomicFastMutex1::Lock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        ++m_Lock;
    }
    else
    {
        int s = DEFAULT_FAST_MUTEX_SPINS; // m_Spins;
        while(1)
        {
            int y = 0;
            if (m_Lock.compare_exchange_weak(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (--s)
            {
                continue;
            }
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            s = DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS; // m_Spins > 1 ? DEFAULT_FAST_MUTEX_TRY_NEXT_SPINS : 1;
            
            CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
            if  (abThreadId != InvalidThrId)
            {
                if (!CMaaThreadExists(abThreadId))
                {
                    y = 0;
                    if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                        {
                            return WAIT_OBJECT_0;
                        }
                    }
                    else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        m_Lock.store(1, std::TL_memory_order_release);
                        return WAIT_ABANDONED; // возможно ложное срабатывание - возврат WAIT_ABANDONED вместо WAIT_OBJECT_0, не критично
                    }
                }
                std::this_thread::yield();
            }
        }
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return WAIT_OBJECT_0;
}
bool CMaaAtomicFastMutex1::TryLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    CMaaThreadIdType abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if (CMaaThreadIdsEqual(abThreadId, ThreadId))
    {
        ++m_Lock;
        return true;
    }
    int y = 0;
    if  (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
    {
        m_ThreadId.store(ThreadId, std::TL_memory_order_release);
        return true;
    }
    //abThreadId = m_ThreadId.load(std::TL_memory_order_acquire);
    if  (abThreadId != InvalidThrId)
    {
        if (!CMaaThreadExists(abThreadId))
        {
            y = 0;
            if (m_Lock.compare_exchange_strong(y, 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
                {
                    return true; // WAIT_OBJECT_0;
                }
            }
            else if (m_ThreadId.compare_exchange_strong(abThreadId, ThreadId, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                m_Lock.store(1, std::TL_memory_order_release);
                return true; // WAIT_ABANDONED;
            }
        }
    }
    return false;
}
int CMaaAtomicFastMutex1::UnLock() noexcept
{
    if (CMaaThreadIdsEqual(m_ThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId()))
    {
        if  (m_Lock.load(std::TL_memory_order_acquire) == 1)
        {
            m_ThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        return --m_Lock;
    }
    return -1;
}
/*
void CMaaAtomicFastMutex1::AddRef() const noexcept
{
}
int CMaaAtomicFastMutex1::UnRef() const noexcept
{
    return 1;
}
*/
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
bool CMaaAtomicFastMutex1::GetLockHolder(int x, char* txt, int buffer_len) const noexcept
{
    return false;
}
_dword CMaaAtomicFastMutex1::Lock(const char* txt) noexcept
{
    return Lock();
}
_dword CMaaAtomicFastMutex1::LockF(const char* file, int line) noexcept
{
    return Lock();
}
int CMaaAtomicFastMutex1::UnLockF(const char* file, int line) noexcept
{
    return UnLock();
}
void CMaaAtomicFastMutex1::FlushLog(bool bForced) const noexcept
{
}
#endif
//===========================================================

#ifdef __unix__

void SetThreadName(_dword /*ThreadId*/, const char * /*Name*/) noexcept
{
}

#ifndef TOOLSLIB_SINGLE_THREAD
CMaaSemaphore::CMaaSemaphore(int MaxCount, int InitialCount) noexcept // -1 is substituted by MaxCount
:   m_RefCount(0)
{
    sem_init(&m_hSemaphore, 0, InitialCount < 0 ? MaxCount : InitialCount);
}
CMaaSemaphore::~CMaaSemaphore() noexcept
{
    sem_destroy(&m_hSemaphore);
}
//DWORD Lock(const char * txt);
DWORD CMaaSemaphore::Lock(DWORD dwTimeToWait) noexcept
{
    sem_wait(&m_hSemaphore);
    ++m_RefCount;
    return 0;
}
//DWORD CMaaSemaphore::TryLock() noexcept;
void CMaaSemaphore::UnLock(int Count) noexcept
{
    for (int i = 0; i < Count; i++)
    {
        --m_RefCount;
        sem_post(&m_hSemaphore);
    }
}
#endif

CMaaMutex1::CMaaMutex1(bool bRecrsive) noexcept
{
    m_RefCount = 1;
    memset(&m, 0, sizeof(m));
    pthread_mutexattr_t a;
    pthread_mutexattr_init(&a);

    if  (bRecrsive)
    {
        //pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE_NP);
        pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
    }
    else
    {
        //pthread_mutexattr_settype(&a, PTHREAD_MUTEX_TIMED_NP); // NORMAL
        pthread_mutexattr_settype(&a, PTHREAD_MUTEX_ERRORCHECK); // NORMAL
    }
    pthread_mutex_init(&m, &a);

    pthread_mutexattr_destroy(&a);
}

CMaaMutex1::~CMaaMutex1() noexcept
{
    pthread_mutex_destroy(&m);
}

_dword CMaaMutex1::Lock() noexcept
{
    /*int n = */pthread_mutex_lock(&m);
    //printf("n = %d\n", n);
    ++m_RefCount;
    return 0;
}

int CMaaMutex1::UnLock() noexcept
{
    int x = --m_RefCount; // TODO: check result
    /*int n = */pthread_mutex_unlock(&m);
    //printf("n = %d\n", n);
    return x;
}

bool CMaaMutex1::TryLock() noexcept
{
    int n = pthread_mutex_trylock(&m);
    if (!n)
    {
        ++m_RefCount;
    }
    return !n;
}

void CMaaMutex1::AddRef() noexcept
{
    Lock();
    ++m_RefCount;
    UnLock();
}
int CMaaMutex1::UnRef() noexcept
{
    Lock();
    --m_RefCount;
    return UnLock();
}

CMaaMutex2::CMaaMutex2() noexcept
:   CMaaMutex1(true)
{
    n = 0;
#ifdef _DEBUG_MTX
    for (int i = 0; i < 20; i++)
    {
        ff[i] = ff2[i] = "?";
        ll[i] = ll2[i] = -1;
    }
#endif
}

CMaaMutex2::~CMaaMutex2() noexcept
{
}

void CMaaMutex2::StillLocked(const char* SrcFile, int SrcLine) noexcept
{
#ifdef _DEBUG_MTX
    int x = n - 1;
    if (x >= 0 && x < 20)
    {
        ff[x] = SrcFile;
        ll[x] = SrcLine;
        ff2[x] = "s";
        ll2[x] = 0;
    }
#endif
}

_dword CMaaMutex2::LockF(const char * f, int l) noexcept
{
    Lock();
    int x = n++;
#ifdef _DEBUG_MTX
    if  (x >= 0 && x < 20)
    {
        ff[x] = f;
        ll[x] = l;
        ff2[x] = ".";
        ll2[x] = -1;
    }
#endif
    return 0;
}

int * gpf = nullptr;

int CMaaMutex2::UnLockF(const char * f, int l) noexcept
{
    int x = --n;
#ifdef _DEBUG_MTX
    if  (x >= 0 && x < 20)
    {
        ff2[x] = f;
        ll2[x] = l;
        if  (this == &gLock)
        {
            //          printf("gLock.UnLock(%s, %d), previous lock was from %s, %d\n", ff2[x], ll2[x], ff[x], ll[x]); fflush(stdout);
        }
        else
        {
            //          printf("%p.UnLock(%s, %d), previous lock was from %s, %d\n", this, ff2[x], ll2[x], ff[x], ll[x]); fflush(stdout);
        }
    }
    if  (x < 0)
    {
        *gpf = 0;
    }
#endif
    CMaaMutex1::UnLock();
    return x;
    //printf("UnLockF(%s,%d), x = %d ......\n", f, l, x); fflush(stdout);
}

int CMaaMutex2::GetDepthEx() noexcept
{
    LockF(__FILE__, __LINE__);
    int n_ = n - 1;
    UnLock();
    return n_;
}

bool CMaaMutex2::GetLockHolder(int x, char * txt, int buffer_len) noexcept
{
    if  (x < 0 || x >= n || buffer_len < 100)
    {
        return false;
    }

#ifdef _DEBUG_MTX
    {
        sprintf(txt, "[%d]: ", x);
        int n0 = strlen(txt);
        const char * FileName = ff2[x];
        int LineNum = ll2[x];
        FileName = FileName ? FileName : "null";

        int n = (int)strlen(FileName);
        n = n0 + n < buffer_len - 30 ? n : buffer_len - 30 - n0;
        n = n >= 0 ? n : 0;
        memcpy(n0 + txt, FileName, n);
        sprintf(n0 + n + txt, "(%d)", LineNum);
    }
#else
    sprintf(txt, "[%d]: ???", x);
#endif
    return true;
}

int CMaaMutex2::GetRefCount() noexcept
{
    return m_RefCount;
}



#endif

#ifndef TOOLSLIB_SINGLE_THREAD
#ifdef _WIN32
unsigned __stdcall CMaaThread::RunThread(void * arg)
#else
// __unix__
void * CMaaThread::RunThread(void * arg)
#endif
{
    CMaaThread * p = (CMaaThread *)arg;

    unsigned r = (unsigned) -1;
    try
    {
        r = p->Run0();
    }
    catch(...)
    {
        if  (p->m_Point == (int)0xaa5555aa)
        {
        }
        else
        {
        }
    }
    //CProtocolListColor c(CProtocolListColor::eGreen);
    //printf("Exiting thread with id=%x, code=%d\n", GetCurrentThreadId(), r);

    //printf("CMaaThread::RunThread(): %p: returning %d\n", arg, r);

#ifdef _WIN32
    return r;
#else
    //return (void *)r;
    return reinterpret_cast<void *>(static_cast<long long>(r)); // !!WARNING!!
#endif
}

void CMaaThread::SetPriority(int /*pr*/, int sched_fifo)
{
#ifdef __unix__
    struct sched_param sp;
    //printf("sched_getscheduler(getpid()) returns %d\n", sched_getscheduler(getpid()));
    //fflush(stdout);
    int x = -1;
    if  (sched_fifo)
    {
        sp.sched_priority = 50; // 0..99
        x = sched_setscheduler(getpid(), SCHED_FIFO, &sp);
    }
    else
    {
        sp.sched_priority = 0;
        x = sched_setscheduler(getpid(), SCHED_OTHER, &sp);
    }
#endif
}

CMaaThread::CMaaThread() noexcept
{
#ifdef _WIN32
    m_hExit = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_ThreadId = CMaaInvalidThreadId();
    m_hThread = INVALID_HANDLE_VALUE;
#else
    // __unix__
    pthread_attr_init(&m_attr);
#ifndef ___ToolsLibQTAndroid___
    pthread_attr_setinheritsched(&m_attr, PTHREAD_EXPLICIT_SCHED);
#endif
    m_Handle = CMaaInvalidThreadId();
    m_bExit = false;

    /*
    int shed_policy;
    if  (!pthread_attr_getschedpolicy(&a, &shed_policy))
    {
        int prio_max = sched_get_priority_max(shed_policy);
        if  (prio_max != -1)
        {
            sched_param sp;
            sp.sched_priority = prio_max; // == 0 for SCHED_OTHER
            //
            printf("prio_max = %d\n", prio_max);
            pthread_attr_setinheritsched(&a, PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedparam(&a, &sp);
        }
    }
    pthread_attr_setinheritsched(&a2, PTHREAD_EXPLICIT_SCHED);

    pthread_t t, t2;

    //int code = pthread_create(&t, &a, StartProc, nullptr);
    */

#endif
    SetPoint();
}

bool CMaaThread::Create() noexcept
{
#ifdef __unix__
    int x = pthread_create(&m_Handle, &m_attr, RunThread, this);

    //printf("pthread_create: m_Handle = %p\n", (void *)m_Handle); fflush(stdout);

    //fprintf(ff, "CMaaThread::Create() %p - %d(%s)\r\n", this, x, x == 0 ? "OK" : "ERROR"); fflush(ff);

    return x == 0;

    // m_bExit = false;
#else
    m_hThread = (HANDLE) _beginthreadex (nullptr, 0, &RunThread, this, 0, (UINT*) &m_ThreadId);
    //printf ("Created new thread with Id = %x\n", m_ThreadId);
    return m_hThread != INVALID_HANDLE_VALUE;
#endif
}

unsigned CMaaThread::Join() noexcept
{
    void * p = nullptr;
#ifdef __unix__
    m_bExit = true;
    pthread_t t{ CMaaInvalidThreadId() };
    if  (m_Handle == t)
    {
        return 0;
    }

    //printf("pthread_join m_Handle = %p\n", (void *)m_Handle); fflush(stdout);

    pthread_join(m_Handle, &p);
    m_Handle = t; // QT found bug of re-join

#else
    if  (m_hExit)
    {
        SetEvent(m_hExit);
    }
    if  (m_hThread && m_hThread != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(m_hThread, INFINITE); // == WAIT_OBJECT_0;
        return 0;
    }
#endif
    //return (unsigned)(unsigned long)p;
    return static_cast<unsigned>(reinterpret_cast<long long>(p));
}

//int g_rc = -123;

bool CMaaThread::Wait(DWORD MaxWaitTime) noexcept
{
    void * p = nullptr;
#ifdef __unix__
    // For unix wait infinitly

    //m_bExit = true;
    pthread_t t { CMaaInvalidThreadId() };
    if  (m_Handle == t)
    {
        return true;
    }

#if 12345

    timespec tspc;
    //__utf8_printf("--- %d\n", clock_gettime(CLOCK_REALTIME, &tspc));
    //__utf8_printf("%d.%09d %d\n", tspc.tv_sec, tspc.tv_nsec, time(nullptr));
    //#define __use_rt
#ifdef __use_rt
#ifdef CLOCK_REALTIME_HR
    //__utf8_printf("%d %d\n", clock_gettime(CLOCK_REALTIME_HR, &tspc), clock_gettime(CLOCK_REALTIME, &tspc));
#else
    //__utf8_printf("--- %d\n", clock_gettime(CLOCK_REALTIME, &tspc));
#endif
#else
    timeval tv;
#endif

    if  (
#ifdef __use_rt
#ifdef CLOCK_REALTIME_HR
    clock_gettime(CLOCK_REALTIME_HR, &tspc) == -1 &&
#endif
    clock_gettime(CLOCK_REALTIME, &tspc) == -1
#else
    gettimeofday(&tv, nullptr) == -1
#endif
    )
    {
        // Handle error
        tspc.tv_sec = time(nullptr);
        tspc.tv_nsec = 0;
    }
#ifndef __use_rt
    else
    {
        tspc.tv_sec = tv.tv_sec;
        tspc.tv_nsec = tv.tv_usec * 1000;
    }
#endif
    //__utf8_printf("%d.%09d %d\n", tspc.tv_sec, tspc.tv_nsec, time(nullptr));
    tspc.tv_nsec += (MaxWaitTime % 1000) * 1000000;
    tspc.tv_sec += MaxWaitTime / 1000;
    if  (tspc.tv_nsec >= 1000000000)
    {
        tspc.tv_sec++;
        tspc.tv_nsec -= 1000000000;
    }
    //    clock_gettime(CLOCK_REALTIME, &tspc);

    int rc;
    //while(1)
    {
#ifndef ___ToolsLibQTAndroid___
        rc = MaxWaitTime == 0 ? 	pthread_tryjoin_np(m_Handle, &p) :
        MaxWaitTime != (DWORD)-1 ? pthread_timedjoin_np(m_Handle, &p, &tspc) :
        pthread_join(m_Handle, &p);
        if  (!rc)
        {
            m_Handle = t; // QT found bug of re-join
        }
#else
        rc = pthread_join(m_Handle, &p); // !!!!!!!!!!!!!!! Android !!!!!!!!!!!!
        m_Handle = t; // QT found bug of re-join
#endif
        //__utf8_printf("rc=%d, errno=%d\n", rc, errno);
        //	if (!(rc && errno == EINPROGRESS))
        {
            //	    break;
        }
    }
    //__utf8_printf("r\n---------------\nc=%d, errno=%d\n", rc, errno);
    //__utf8_printf_flush();

    //g_rc = rc;

    if  (MaxWaitTime != (DWORD)-1)
    {
        return rc == 0;
    }
    //    return rc == 0;
#else
    pthread_join(m_Handle, &p);
    m_Handle = t; // QT found bug of re-join
#endif

    return true;

#else
    /*
     if (m_hExit)
     {
          SetEvent(m_hExit);
     }
     */
    if  (m_hThread && m_hThread != INVALID_HANDLE_VALUE)
    {
        return WaitForSingleObject(m_hThread, MaxWaitTime) == WAIT_OBJECT_0;
    }
    return true;
#endif
}

CMaaThread::~CMaaThread()
{
#ifdef _WIN32
    Exit();
    CloseHandle(m_hExit);
    if  (m_hThread && m_hThread != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hThread);
    }
#else
    Join();
#endif
}

unsigned CMaaThread::Run0()
{
    return Run();
}

unsigned CMaaThread::Run()
{
    return 0;
}

bool CMaaThread::Exit(DWORD MaxWaitTime) noexcept
{
#ifdef _WIN32
    if  (m_hExit)
    {
        SetEvent(m_hExit);
    }
    if  (m_hThread && m_hThread != INVALID_HANDLE_VALUE)
    {
        return WaitForSingleObject(m_hThread, MaxWaitTime) == WAIT_OBJECT_0;
    }
    return false;
#else
    pthread_t t{ CMaaInvalidThreadId() };
    if  (m_Handle == t)
    {
        return false;
    }
    m_bExit = true;
    return Wait(MaxWaitTime);
    /*
    if   (MaxWaitTime == 0)
    {
        m_bExit = true;
        return false;
    }
    */
    //Join();
    //return true;
#endif
}

void CMaaThread::CheckPoint(DWORD WaitTime)
{
#ifdef _WIN32
    DWORD RetEvent = WaitForSingleObject(m_hExit, WaitTime);
    if  (RetEvent == WAIT_OBJECT_0 || RetEvent == WAIT_FAILED)
    {
        throw eExitThread ();
    }
#else
    if  (m_bExit)
    {
        //printf("throw eExitThread();\n"); fflush(stdout);
        throw eExitThread ();
    }
    if  (!WaitTime)
    {
        return;
    }
    timeval tv;
    gettimeofday(&tv, nullptr);
    for (;;)
    {
        usleep(1000);
        if  (m_bExit)
        {
            throw eExitThread ();
        }
        timeval tv2;
        gettimeofday(&tv2, nullptr);
        int ms = (tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec) / 1000;
        if  (ms >= (int)WaitTime)
        {
            return;
        }
    }
#endif
}
#endif

//---------------------------------------------------------------------------------------------
CMaaTimedMutex::CMaaTimedMutex() noexcept
{
#ifdef __unix__
    pthread_cond_init(&m_cond, nullptr);
    m_Status = 0;
    //m_curr = nullptr;
    const pthread_t InvalidThrId{ CMaaInvalidThreadId() };
    m_curr = InvalidThrId;
#endif
}
CMaaTimedMutex::~CMaaTimedMutex() noexcept
{
#ifdef __unix__
    pthread_cond_destroy(&m_cond);
#endif
}
bool CMaaTimedMutex::Lock(_dword ms) noexcept
{
#ifdef __unix__
    if  (ms == (_dword)-1)
    {
        return Lock((_qword)-1);
    }
    return Lock((_qword)ms * 1000);
#else
    return CMaaMutex2::Lock(ms) == WAIT_OBJECT_0;
#endif
}
bool CMaaTimedMutex::Lock(_qword us) noexcept
{
#ifdef __unix__

    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    pthread_t curr = m_curr;
    aLocker_UnLock;
    const pthread_t InvalidThrId{ CMaaInvalidThreadId() };
    if  (curr != InvalidThrId && pthread_equal(curr, pthread_self()))
    {
        m_Status++;
        return true;
    }

    CMaaMutex2::Lock();
    int code = 0;
    while(m_Status > 0)
    {
        if  (us != (_qword)-1)
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);

            timespec ti;
            ti.tv_nsec = (tv.tv_usec + us % 1000000UL) * 1000UL;
            ti.tv_sec = tv.tv_sec + (us / 1000000UL) + (ti.tv_nsec / 1000000000);
            ti.tv_nsec %= 1000000000;
            code = pthread_cond_timedwait(&m_cond, &m, &ti);
            //code = pthread_mutex_timedlock(&m, &ti);
        }
        else
        {
            code = pthread_cond_wait(&m_cond, &m);
            //code = pthread_mutex_lock(&m);
        }
        if  (code == 0 && m_Status > 0)
        {
            // many vendors warn of spurios wakeups from
            // pthread_cond_wait(), especially after signal delivery,
            // even though POSIX doesn't allow for it... sigh
            continue;
        }
        break;
    }

    if  (code == 0)
    {
        aLocker_Lock;
        m_curr = pthread_self();
        aLocker_UnLock;
        m_Status++;
    }

    //if       (code != 0)
    {
        CMaaMutex2::UnLock();
    }

    //if (code && code != ETIMEDOUT) report_error(code, "QWaitCondition::wait()", "cv wait");

    //printf("code = %d\n", code);
    return (code == 0);
#else
    if  (us == (_qword)-1)
    {
        return Lock((_dword)-1);
    }
    return Lock((_dword)(us / 1000UL));
#endif
}

bool CMaaTimedMutex::TryLock() noexcept
{
#ifdef __unix__

    /*
    int n = pthread_mutex_trylock(&m);
    if (!n)
    {
        m_curr = pthread_self();
        m_Status++;
    }
    return !n;
    */

    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    pthread_t curr = m_curr;
    aLocker_UnLock;
    const pthread_t InvalidThrId{ CMaaInvalidThreadId() };
    if (curr != InvalidThrId && pthread_equal(curr, pthread_self()))
    {
        m_Status++;
        return true;
    }

    CMaaMutex2::Lock();
    int code = 0;
    while (m_Status > 0)
    {
        timespec ti;
        ti.tv_nsec = 0;
        ti.tv_sec = 0;
        code = pthread_cond_timedwait(&m_cond, &m, &ti);

        if (code == 0 && m_Status > 0)
        {
            // many vendors warn of spurios wakeups from
            // pthread_cond_wait(), especially after signal delivery,
            // even though POSIX doesn't allow for it... sigh
            continue;
        }
        break;
    }

    if (code == 0)
    {
        aLocker_Lock;
        m_curr = pthread_self();
        aLocker_UnLock;
        m_Status++;
    }

    //if       (code != 0)
    {
        CMaaMutex2::UnLock();
    }

    //if (code && code != ETIMEDOUT) report_error(code, "QWaitCondition::wait()", "cv wait");

    //printf("code = %d\n", code);
    return (code == 0);
#else
    return Lock((_dword)0);
#endif
}

bool CMaaTimedMutex::UnLock() noexcept
{
#ifdef __unix__
    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    pthread_t curr = m_curr;
    aLocker_UnLock;
    if  (!pthread_equal(curr, pthread_self()))
    {
        return false;
    }
    //aLocker_Lock;
    if  (--m_Status == 0)
    {
        //m_curr = nullptr;
        const pthread_t InvalidThrId{ CMaaInvalidThreadId() };
        aLocker_Lock;
        m_curr = InvalidThrId;
        aLocker_UnLock;
        //CMaaMutex1::UnLock();
        pthread_cond_signal(&m_cond);
    }
    //else
    //{
        //aLocker_UnLock;
    //}
    return true;
#else
    return CMaaMutex2::UnLock() >= 0;
#endif
}

#ifdef _WIN32
CMaaEvent::CMaaEvent(HANDLE h, bool bManualReset, int DoNotCloseEventHandle) noexcept
/*
#ifdef __unix__
:    m_Mutex(false),
     m_bManualReset(bManualReset),
     m_bState(bManualReset ? bInitialState : false),
#else
*/
:   m_bManualReset(bManualReset),
//#endif
    m_nWaiters(0),
    m_nWakeups(0),
    m_nPulseCount(0)
{
/*
#ifdef __unix__
     pthread_cond_init(&m_cond, nullptr);
#else
*/
    m_hEvent = h;
    m_ExternalEvent = DoNotCloseEventHandle;
    //#endif
}
#endif

CMaaEvent::CMaaEvent(bool bManualReset, bool bInitialState /*for ManualReset event*/) noexcept
#ifdef __unix__
:   m_Mutex(false),
    m_bManualReset(bManualReset),
    m_bState(bManualReset ? bInitialState : false),
#else
    :   m_bManualReset(bManualReset),
#endif
    m_nWaiters(0),
    m_nWakeups(0),
    m_nPulseCount(0)
{
#ifdef __unix__
    pthread_cond_init(&m_cond, nullptr);
#else
    m_hEvent = CreateEvent(nullptr, TRUE, bManualReset && bInitialState ? TRUE : FALSE, nullptr);
#endif
}
CMaaEvent::~CMaaEvent()
{
#ifdef __unix__
    pthread_cond_destroy(&m_cond);
#else
    if  (m_ExternalEvent <= 0)
    {
        CloseHandle(m_hEvent);
    }
#endif
}
void CMaaEvent::WakeOne() noexcept
{
#ifdef __unix__
    m_Mutex.Lock();
    m_nWakeups = m_nWakeups + 1 <= m_nWaiters ? m_nWakeups + 1 : m_nWaiters;
    m_nPulseCount = 0;
    if  (m_bManualReset)
    {
        m_bState = true;
    }
    pthread_cond_signal(&m_cond);
    m_Mutex.UnLock();
#else
    m_Mutex.Lock();
    m_nWakeups = m_nWakeups + 1 <= m_nWaiters ? m_nWakeups + 1 : m_nWaiters;
    m_nPulseCount = 0;
    SetEvent(m_hEvent);
    m_Mutex.UnLock();
#endif
}
void CMaaEvent::WakeAll() noexcept
{
#ifdef __unix__
    m_Mutex.Lock();
    m_nWakeups = m_nWaiters;
    m_nPulseCount = 0;
    if  (m_bManualReset)
    {
        m_bState = true;
    }
    pthread_cond_broadcast(&m_cond);
    m_Mutex.UnLock();
#else
    m_Mutex.Lock();
    m_nWakeups = m_nWaiters;
    m_nPulseCount = 0;
    SetEvent(m_hEvent);
    m_Mutex.UnLock();
#endif
}
bool CMaaEvent::Wait(_dword TimeMs) noexcept
{
#ifdef __unix__
    if  (TimeMs == (_dword)-1)
    {
        return Wait((_qword)-1);
    }
    return Wait((_qword)TimeMs * (_qword)1000UL);
#else
    m_Mutex.Lock();
    if  (m_nWaiters++ == 0 && !m_bManualReset)
    {
        m_nWakeups = m_nPulseCount = 0;
        ResetEvent(m_hEvent);
    }
    m_Mutex.UnLock();
    while(1)
    {
        const DWORD dw = WaitForSingleObject(m_hEvent, TimeMs);
        if  (dw != WAIT_OBJECT_0 && dw != WAIT_ABANDONED)
        {
            const DWORD xx = GetLastError();
            m_Mutex.Lock();
            m_nWaiters--;
            m_Mutex.UnLock();
            return false;
        }
        m_Mutex.Lock();
        if  (m_nWakeups <= 0 && !m_bManualReset)
        {
            ResetEvent(m_hEvent);
            m_Mutex.UnLock();
        }
        else
        {
            m_nWakeups = m_nWakeups > 0 ? m_nWakeups - 1 : 0;
            m_nWaiters--;
            if  (m_nPulseCount > 0)
            {
                if  (--m_nPulseCount == 0)
                {
                    ResetEvent(m_hEvent);
                }
            }
            if  (m_nWaiters <= 0 && !m_bManualReset)
            {
                ResetEvent(m_hEvent);
            }
            m_Mutex.UnLock();
            break;
        }
    }
    return true;
#endif
}

bool CMaaEvent::Wait(_qword TimeUs) noexcept
{
#ifdef __unix__
    int code = 0;
    m_Mutex.Lock();
    //m_nWaiters++;
    if  (m_nWaiters++ == 0 && !m_bManualReset)
    {
        m_nWakeups = m_nPulseCount = 0;
        // /**/ m_bState = false;
    }
    while(1)
    {
        if  (m_bManualReset && m_bState)
        {
            break;
        }
        if  (TimeUs != (_qword)-1)
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);

            timespec ti;
            ti.tv_nsec = (tv.tv_usec + TimeUs % 1000000UL) * 1000UL;
            ti.tv_sec = tv.tv_sec + (TimeUs / 1000000UL) + (ti.tv_nsec / 1000000000);
            ti.tv_nsec %= 1000000000;
            code = pthread_cond_timedwait(&m_cond, &m_Mutex.m, &ti);
        }
        else
        {
            code = pthread_cond_wait(&m_cond, &m_Mutex.m);
        }
        /*
          if   (code == 0 && wakeups == 0)
          {
               // many vendors warn of spurios wakeups from
               // pthread_cond_wait(), especially after signal delivery,
               // even though POSIX doesn't allow for it... sigh
               continue;
          }
          */
        break;
    }
    if  (code == 0)
    {
        m_nWakeups = m_nWakeups > 0 ? m_nWakeups - 1 : 0;
    }
    m_nWaiters--;
    /*
     if   ((m_nWaiters == 0 || m_nWakeups == 0) && !m_bManualReset)
     {
          m_bState = false;
     }
     */
    if  (code == 0 && m_nPulseCount > 0)
    {
        if  (--m_nPulseCount == 0)
        {
            m_bState = false;
        }
    }
    m_Mutex.UnLock();

    //if (code && code != ETIMEDOUT) report_error(code, "QWaitCondition::wait()", "cv wait");

    return (code == 0);
#else
    if  (TimeUs == (_qword)-1)
    {
        return Wait((_dword)-1);
    }
    return Wait((_dword)(TimeUs / 1000UL));
#endif
}

bool CMaaEvent::Reset() noexcept
{
    // Reset manual reset mutex
    if  (!m_bManualReset)
    {
        return false;
    }
#ifdef __unix__
    m_Mutex.Lock();
    m_bState = false;
    m_Mutex.UnLock();
#else
    ResetEvent(m_hEvent);
#endif
    return true;
}

bool CMaaEvent::Pulse() noexcept
{
    if  (m_bManualReset)
    {
        // wake all and reset
#ifdef __unix__
        m_Mutex.Lock();
        m_nWakeups = m_nWaiters;
        m_nPulseCount = m_nWakeups;
        if  (m_bManualReset && m_nWakeups > 0)
        {
            m_bState = true;
        }
        else if (m_bManualReset) // && m_nWakeups == 0)
        {
            m_bState = false;
        }
        pthread_cond_broadcast(&m_cond);
        m_Mutex.UnLock();
#else
        m_Mutex.Lock();
        m_nWakeups = m_nWaiters;
        m_nPulseCount = m_nWakeups;
        if  (m_nWakeups > 0)
        {
            SetEvent(m_hEvent);
        }
        else
        {
            ResetEvent(m_hEvent);
        }
        m_Mutex.UnLock();
#endif
    }
    else
    {
        // wake one and reset
#ifdef __unix__
        m_Mutex.Lock();
        m_nWakeups = /*m_nWakeups +*/ 1 <= m_nWaiters ? /*m_nWakeups +*/ 1 : 0 /*m_nWaiters*/;
        if  (m_nWakeups > 0)
        {
            m_nPulseCount = 1;
        }
        if  (m_bManualReset && m_nWakeups > 0)
        {
            m_bState = true;
        }
        else if (m_bManualReset) // && m_nWakeups == 0)
        {
            m_bState = false;
        }
        pthread_cond_signal(&m_cond);
        m_Mutex.UnLock();
#else
        m_Mutex.Lock();
        m_nWakeups = /*m_nWakeups +*/ 1 <= m_nWaiters ? /*m_nWakeups +*/ 1 : 0 /*m_nWaiters*/;
        if  (m_nWakeups > 0)
        {
            m_nPulseCount = 1;
            SetEvent(m_hEvent);
        }
        else
        {
            m_nPulseCount = 0;
            ResetEvent(m_hEvent);
        }
        m_Mutex.UnLock();
#endif
    }
    return true;
}
//---------------------------------------------------------------------------------------------
#ifdef TOOLSLIB_RW_LOCKS_EXPERIMENTAL
CMaaRWLock::CMaaRWLock() noexcept
:   m_h(true, true),
    m_n(0),
    m_AccessType('\0')
{
    //m_h = CreateEvent(nullptr, TRUE, TRUE, nullptr);
    //m_hState = true;
}

CMaaRWLock::~CMaaRWLock() noexcept
{
}

bool CMaaRWLock::RLock(_dword Time, int n) noexcept
{
    /*
    const bool x = m_m1.Lock(Time);
    if (!x) //x != WAIT_OBJECT_0 && x != WAIT_ABANDONED)
    {
        return false;
    }
    */
    if (m_m1.Lock(Time) == WAIT_TIMEOUT)
    {
        return false;
    }
    m_m2.Lock();
    if  ((m_n += n) == n)
    {
        m_AccessType = m_AccessType == '\0' ? 'r' : '*';
    }
    m_m2.UnLock();
    m_m1.UnLock();
    return true;
}

bool CMaaRWLock::RUnLock(int n) noexcept
{
    m_m2.Lock();
    if  (m_n < n)
    {
        m_m2.UnLock();
        return false;
    }
    if  (!(m_n -= n))
    {
        if (!(m_AccessType = m_AccessType == 'r' ? '\0' : 'w'))
        {
            //m_hState = true;
            m_h.WakeAll();
        }
    }
    m_m2.UnLock();
    return true;
}

bool CMaaRWLock::WLock(_dword Time) noexcept
{
    /*
    bool x = m_m1.Lock(Time);
    if (!x)//x != WAIT_OBJECT_0 && x != WAIT_ABANDONED)
    {
        return false;
    }
    */
    _dword t0 = GetTickCount(), t1;
    if (m_m1.Lock(Time) == WAIT_TIMEOUT)//x != WAIT_OBJECT_0 && x != WAIT_ABANDONED)
    {
        return false;
    }
    while(1)
    {
        /*
        x = m_h.Wait(Time);
        if (!x)
        {
            // time out
            m_m1.UnLock();
            return false;
        }
        */
        t1 = GetTickCount();
        if (!m_h.Wait(Time == (_dword)-1 ? Time : t1 - t0 < Time ? Time - (t1 - t0) : 0))
        {
            // time out
            m_m1.UnLock();
            return false;
        }
        //if  (x)// == WAIT_OBJECT_0)
        {
            m_m2.Lock();
            const int nn = m_n;
            if  (nn > 0)
            {
                //m_hState = false;
                m_h.Reset();
            }
            m_m2.UnLock();
            if  (nn <= 0)
            {
                break;
            }
        }
    }
    m_AccessType = 'w';
    return true;
}

void CMaaRWLock::WUnLock() noexcept
{
    m_AccessType = m_AccessType == 'w' ? '\0' : 'r';
    m_m1.UnLock();
}

// Wlock must have current thread
bool CMaaRWLock::W2RLock(int n) noexcept
{
    if (m_AccessType == 'r')
    {
        return false;
    }
    m_m2.Lock();
    m_n = n;
    //if (m_n > 0)
    //{
        m_AccessType = 'r';
    //}
    m_m2.UnLock();
    m_m1.UnLock();
    return true;
}

bool CMaaRWLock::R2WLock(_dword Time, int n) noexcept
{
#if 1
    return RUnLock(n) && WLock(Time);
#else
    if (Time == (_dword)-1)
    {
        return RUnLock(n) && WLock();
    }

    /*
    bool x = m_m1.Lock(Time);
    if (!x)//x != WAIT_OBJECT_0 && x != WAIT_ABANDONED)
    {
        return false;
    }
    */
    _dword t0 = GetTickCount(), t1;
    if (!m_m1.Lock(Time))//x != WAIT_OBJECT_0 && x != WAIT_ABANDONED)
    {
        return false;
    }
    if (!RUnLock(n))
    {
        m_m1.UnLock();
        return false;
    }
    while (1)
    {
        /*
        x = m_h.Wait(Time);
        if (!x)
        {
            // time out
            m_m1.UnLock();
            return false;
        }
        */
        t1 = GetTickCount();
        if (!m_h.Wait(Time == (_dword)-1 ? Time : t1 - t0 < Time ? Time - (t1 - t0) : 0))
        {
            // time out
            m_m2.Lock();
            m_n += n;
            if (m_n > 0)
            {
                m_AccessType = 'r';
            }
            m_m2.UnLock();
            m_m1.UnLock();
            return false;
        }
        //if  (x)// == WAIT_OBJECT_0)
        {
            m_m2.Lock();
            const int nn = m_n;
            if (nn > 0)
            {
                //m_hState = false;
                m_h.Reset();
            }
            m_m2.UnLock();
            if (nn <= 0)
            {
                break;
            }
        }
    }
    m_AccessType = 'w';
    return true;
#endif
}

void CMaaRWLock::UnLock(int n) noexcept
{
    switch(m_AccessType)
    {
    case 'w':
        WUnLock();
        break;
    case 'r':
        RUnLock(n);
        break;
    case '*':
        WUnLock();
        RUnLock(n);
        break;
    //default: // error
    }
}
//---------------------------------------------------------------------------------------------
#ifdef _WIN32

CMaaRWLock2::CMaaRWLock2() noexcept
{
}
CMaaRWLock2::~CMaaRWLock2() noexcept
{
}
void CMaaRWLock2::RLock(int n) noexcept
{
    m_Lock.RLock((_dword)-1, n);
}
void CMaaRWLock2::RUnLock(int n) noexcept
{
    m_Lock.RUnLock(n);
}
void CMaaRWLock2::WLock() noexcept
{
    m_Lock.WLock((_dword)-1);
}
void CMaaRWLock2::WUnLock() noexcept
{
    m_Lock.WUnLock();
}
bool CMaaRWLock2::W2RLock(int n) noexcept
{
    return m_Lock.W2RLock(n);
}
bool CMaaRWLock2::R2WLock_(int n) noexcept
{
    return m_Lock.R2WLock((_dword)-1, n);
}

#else

CMaaRWLock2::CMaaRWLock2() noexcept
{
    pthread_mutex_init(&m_Mutex, NULL);
    pthread_cond_init(&m_CondRead, NULL);
    pthread_cond_init(&m_CondWrite, NULL);

    m_Readers = m_Writers = m_Read_waiters = m_Write_waiters = 0;
}
CMaaRWLock2::~CMaaRWLock2() noexcept
{
    pthread_cond_destroy(&m_CondWrite);
    pthread_cond_destroy(&m_CondRead);
    pthread_mutex_destroy(&m_Mutex);
}
void CMaaRWLock2::RLock(int n) noexcept
{
    pthread_mutex_lock(&m_Mutex);
    if  (m_Writers || m_Write_waiters)
    {
        m_Read_waiters += n;
        do
        {
            pthread_cond_wait(&m_CondRead, &m_Mutex);

        } while (m_Writers || m_Write_waiters);

        m_Read_waiters -= n;
    }
    m_Readers += n;
    pthread_mutex_unlock(&m_Mutex);
}
void CMaaRWLock2::RUnLock(int n) noexcept
{
    pthread_mutex_lock(&m_Mutex);
    m_Readers -= n;
    if  (m_Write_waiters)
    {
        pthread_cond_signal(&m_CondWrite);
    }
    pthread_mutex_unlock(&m_Mutex);
}
void CMaaRWLock2::WLock() noexcept
{
    pthread_mutex_lock(&m_Mutex);
    if  (m_Readers || m_Writers)
    {
        m_Write_waiters++;
        do
        {
            pthread_cond_wait(&m_CondWrite, &m_Mutex);

        } while (m_Readers || m_Writers);

        m_Write_waiters--;
    }
    m_Writers = 1;
    pthread_mutex_unlock(&m_Mutex);
}
void CMaaRWLock2::WUnLock() noexcept
{
    pthread_mutex_lock(&m_Mutex);
    m_Writers = 0;
    if  (m_Write_waiters)
    {
        pthread_cond_signal(&m_CondWrite);
    }
    else if (m_Read_waiters)
    {
        pthread_cond_broadcast(&m_CondRead);
    }
    pthread_mutex_unlock(&m_Mutex);
}
bool CMaaRWLock2::W2RLock(int n) noexcept
{
    pthread_mutex_lock(&m_Mutex);
    if (!m_Writers)
    {
        pthread_mutex_unlock(&m_Mutex);
        return false;
    }
    m_Writers = 0;
    m_Readers += n;
    if (m_Read_waiters)
    {
        pthread_cond_broadcast(&m_CondRead);
    }
    pthread_mutex_unlock(&m_Mutex);
    return true;
}
bool CMaaRWLock2::R2WLock_(int n) noexcept
{
    RUnLock(n);
    WLock();
    return true;
}
//---------------------------------------------------------------------------------------------
// Since C++ 17 (VS2015):
/*
#include <shared_mutex>

typedef std::shared_mutex Lock;
typedef std::unique_lock< Lock >  WriteLock;
typedef std::shared_lock< Lock >  ReadLock;

Lock myLock;

void ReadFunction()
{
     ReadLock r_lock(myLock);
     //Do reader stuff
}

void WriteFunction()
{
     WriteLock w_lock(myLock);
     //Do writer stuff
}
*/
//---------------------------------------------------------------------------------------------

#endif
#endif
//---------------------------------------------------------------------------------------------
CMaaRWLockRmp::CMaaRWLockRmp() noexcept
:   m_WriterReaders(0),
    m_WriterThreadId(InvalidThrId)
{
}
CMaaRWLockRmp::~CMaaRWLockRmp() noexcept
{
}
void CMaaRWLockRmp::RLock() noexcept
{
    if (++m_WriterReaders >= cWriter1)
    {
        const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
        if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
        {
            return;
        }
        int s = DEFAULT_R_MUTEX_SPINS;
        while ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) >= cWriter1)
        {
            if (--s)
            {
                continue;
            }
            s = DEFAULT_R_MUTEX_TRY_NEXT_SPINS1;
            std::this_thread::yield();
        }
    }
}
void CMaaRWLockRmp::RUnLock() noexcept
{
    m_WriterReaders.fetch_sub(1, std::TL_memory_order_acq_rel);
}
void CMaaRWLockRmp::WLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1, std::TL_memory_order_acq_rel);
    }
    else
    {
        int s = DEFAULT_W_MUTEX_SPINS;
        while (1)
        {
            _dword y = 0;
            if (m_WriterReaders.compare_exchange_strong(y, cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (--s)
            {
                continue;
            }
            s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
            std::this_thread::yield();
        }
        m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    }
}
bool CMaaRWLockRmp::WUnLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1, std::TL_memory_order_acq_rel);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------
CMaaRWLockRp::CMaaRWLockRp() noexcept
:   m_WriterReaders(0),
    m_WriterThreadId(InvalidThrId)
{
}
CMaaRWLockRp::~CMaaRWLockRp() noexcept
{
}
void CMaaRWLockRp::RLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(1, std::TL_memory_order_acq_rel);
        return;
    }
    int s = DEFAULT_R_MUTEX_SPINS;
    while(1)//(m_WriterReaders & cWritersMask) >= cWritersWaitingBit)
    {
        _dword y = m_WriterReaders.load(std::TL_memory_order_acquire);
        y = !(y & cWriterMask) && (y & cReadersWaitingMask) ? y & (cWritersWaitingMask | cReadersWaitingMask | cR2WUnchangedFlag) :
            //!(y & cWriterMask) && (y & cWritersWaitingMask) ? 0 : // skip compare_exchange_xxx()
            y & cReadersWaitingMask;
        if (m_WriterReaders.compare_exchange_strong(y, y + 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            break;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_R_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
}
bool CMaaRWLockRp::RLock_us(_qword us) noexcept
{
    if (us == -1)
    {
        RLock();
        return true;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(1, std::TL_memory_order_acq_rel);
        return true;
    }
    const CMaaTime& hrt = GetHRTime(true);
    const _uqword e = hrt.GetNextTime(us, 1000000);
    int s = DEFAULT_R_MUTEX_SPINS;
    while (1)//(m_WriterReaders & cWritersMask) >= cWritersWaitingBit)
    {
        _dword y = m_WriterReaders.load(std::TL_memory_order_acquire);
        y = !(y & cWriterMask) && (y & cReadersWaitingMask) ? y & (cWritersWaitingMask | cReadersWaitingMask | cR2WUnchangedFlag) :
            //!(y & cWriterMask) && (y & cWritersWaitingMask) ? 0 : // skip compare_exchange_xxx()
            y & cReadersWaitingMask;
        if (m_WriterReaders.compare_exchange_strong(y, y + 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            break;
        }
        if (hrt.GetCounter() >= e)
        {
            return false;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_R_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
    return true;
}
void CMaaRWLockRp::RUnLock() noexcept
{
    m_WriterReaders.fetch_sub(1, std::TL_memory_order_acq_rel);
}
void CMaaRWLockRp::WLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1, std::TL_memory_order_acq_rel);
    }
    else
    {
        m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);
        int s = DEFAULT_W_MUTEX_SPINS;
        while (1)
        {
            _dword y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
            if (m_WriterReaders.compare_exchange_strong(y, (y & ~ cR2WUnchangedFlag) - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (--s)
            {
                continue;
            }
            s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
            std::this_thread::yield();
        }
        m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    }
}
bool CMaaRWLockRp::WLock_us(_qword us) noexcept
{
    if (us == -1)
    {
        WLock();
        return true;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1, std::TL_memory_order_acq_rel);
    }
    else
    {
        m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);
        const CMaaTime& hrt = GetHRTime(true);
        const _uqword e = hrt.GetNextTime(us, 1000000);
        int s = DEFAULT_W_MUTEX_SPINS;
        while (1)
        {
            _dword y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
            if (m_WriterReaders.compare_exchange_strong(y, (y & ~cR2WUnchangedFlag) - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (hrt.GetCounter() >= e)
            {
                m_WriterReaders.fetch_sub(cWritersWaiting1, std::TL_memory_order_acq_rel);
                return false;
            }
            if (--s)
            {
                continue;
            }
            s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
            std::this_thread::yield();
        }
        m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    }
    return true;
}
bool CMaaRWLockRp::RLock(_dword ms) noexcept
{
    if (ms == INFINITE)
    {
        RLock();
        return true;
    }
    return RLock_us(1000 * (_uqword)ms);
}
bool CMaaRWLockRp::WLock(_dword ms) noexcept
{
    if (ms == INFINITE)
    {
        WLock();
        return true;
    }
    return WLock_us(1000 * (_uqword)ms);
}

bool CMaaRWLockRp::WUnLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1, std::TL_memory_order_acq_rel);
        return true;
    }
    return false;
}
bool CMaaRWLockRp::W2RLock(int n) noexcept
{
    if (n <= 0)
    {
        return false;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1 - (_dword)n, std::TL_memory_order_acq_rel);
        return true;
    }
    return false;
}
bool CMaaRWLockRp::W2RLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1 - 1, std::TL_memory_order_acq_rel);
        return true;
    }
    return false;
}
bool CMaaRWLockRp::R2WLock(int nR) noexcept
{
    if (nR <= 0)
    {
        return false; // this is error case, not !bUnchanged
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1 - nR, std::TL_memory_order_acq_rel); // 1;
        return true;
    }
 
    _dword y = m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);

    // avoid ABA problem by restricting the cases of flag set:
    // 1R, 2R, 1R2W(interrupt)+2R2W, 2W2R, 2R2W(no flag)+1R2W(continue)
    if (!(y & cWritersWaitingMask))
    {
        m_WriterReaders.fetch_or(cR2WUnchangedFlag, std::TL_memory_order_acq_rel);
    }

    m_WriterReaders.fetch_sub(nR, std::TL_memory_order_acq_rel); // --m_WriterReaders;

    bool bUnchanged;
    int s = DEFAULT_W_MUTEX_SPINS;
    while (1)
    {
        y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
        if (m_WriterReaders.compare_exchange_strong(y, y - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            bUnchanged = (m_WriterReaders.load(std::TL_memory_order_acquire) & cR2WUnchangedFlag);
            m_WriterReaders.fetch_and(~ cR2WUnchangedFlag, std::TL_memory_order_acq_rel);
            break;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
    m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    return bUnchanged;
}
bool CMaaRWLockRp::R2WLock() noexcept // int nR == 1
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1 - 1, std::TL_memory_order_acq_rel);
        return true;
    }

    _dword y = m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);

    // avoid ABA problem by restricting the cases of flag set:
    // 1R, 2R, 1R2W(interrupt)+2R2W, 2W2R, 2R2W(no flag)+1R2W(continue)
    if (!(y & cWritersWaitingMask))
    {
        m_WriterReaders.fetch_or(cR2WUnchangedFlag, std::TL_memory_order_acq_rel);
    }

    m_WriterReaders.fetch_sub(1, std::TL_memory_order_acq_rel);

    bool bUnchanged;
    int s = DEFAULT_W_MUTEX_SPINS;
    while (1)
    {
        y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
        if (m_WriterReaders.compare_exchange_strong(y, y - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            bUnchanged = (m_WriterReaders.load(std::TL_memory_order_acquire) & cR2WUnchangedFlag);
            m_WriterReaders.fetch_and(~ cR2WUnchangedFlag, std::TL_memory_order_acq_rel);
            break;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
    m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    return bUnchanged;
}
bool CMaaRWLockRp::IsWOwner() const noexcept
{
    return CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId());
}
//---------------------------------------------------------------------------------------------
#ifdef CMaaRWLockWp_DBG
void CMaaRWLockWp::SetRDbg(int n) noexcept
{
#ifdef CMaaRWLockWp_DBG
    const CMaaThreadIdType dw = CMaaGetCurrentThreadId();
    for (int i = 0; i < MaxRCount; i++)
    {
        CMaaThreadIdType dw0 = InvalidThrId;
        if (m_R[i].compare_exchange_strong(dw0, dw, std::TL_memory_order_acq_rel, std::memory_order_relaxed) && !--n)
        {
            return;
        }
    }
#endif
}
void CMaaRWLockWp::UnSetRDbg(int n) noexcept
{
#ifdef CMaaRWLockWp_DBG
    const CMaaThreadIdType dw = CMaaGetCurrentThreadId();
    for (int i = 0; i < MaxRCount; i++)
    {
        if (CMaaThreadIdsEqual(m_R[i].load(std::TL_memory_order_acquire), dw))
        {
            m_R[i] = InvalidThrId;
            if (!--n)
            {
                return;
            }
        }
    }
#endif
}
#endif
CMaaRWLockWp::CMaaRWLockWp() noexcept
:   m_WriterReaders(0),
    m_WriterThreadId(InvalidThrId)
{
#ifdef CMaaRWLockWp_DBG
    for (int i = 0; i < MaxRCount; i++)
    {
        m_R[i] = InvalidThrId;
    }
    m_W = m_W1 = InvalidThrId;
#endif
}
CMaaRWLockWp::~CMaaRWLockWp() noexcept
{
}
void CMaaRWLockWp::RLock(bool bForced) noexcept
{
#ifdef CMaaRWLockWp_DBG
    SetRDbg();
#endif
    if (bForced || CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId()))
    {
#ifdef DEBUG_RW_LOCKS
        //if (bForced && RCnt() > 0 /*|| (WCnt() > 0)*/) { rw1_err(); }
#endif
        m_WriterReaders.fetch_add(1, std::TL_memory_order_acq_rel);
        return;
    }
    int s = DEFAULT_R_MUTEX_SPINS;
    while (1)//(m_WriterReaders.load(std::TL_memory_order_acquire) & cWritersMask) >= cWritersWaitingBit)
    {
        _dword y = m_WriterReaders.load(std::TL_memory_order_acquire) & cReadersWaitingMask;
        /*
        _dword y = m_WriterReaders.load(std::TL_memory_order_acquire);
        y = !(y & cWriterMask) && (y & cReadersWaitingMask) ? y & (cWritersWaitingMask | cReadersWaitingMask | cR2WUnchangedFlag) :
            //!(y & cWriterMask) && (y & cWritersWaitingMask) ? 0 : // skip compare_exchange_xxx()
            y & cReadersWaitingMask;
        */
        if (m_WriterReaders.compare_exchange_strong(y, y + 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            break;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_R_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
}
bool CMaaRWLockWp::RLock_us(_qword us, bool bForced) noexcept
{
    if (us == -1)
    {
        RLock(bForced);
        return true;
    }
#ifdef CMaaRWLockWp_DBG
    SetRDbg();
#endif
    if (bForced || CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId()))
    {
        m_WriterReaders.fetch_add(1, std::TL_memory_order_acq_rel);
        return true;
    }
    const CMaaTime& hrt = GetHRTime(true);
    const _uqword e = hrt.GetNextTime(us, 1000000);
    int s = DEFAULT_R_MUTEX_SPINS;
    while (1)//(m_WriterReaders.load(std::TL_memory_order_acquire) & cWritersMask) >= cWritersWaitingBit)
    {
        _dword y = m_WriterReaders.load(std::TL_memory_order_acquire) & cReadersWaitingMask;
        /*
        _dword y = m_WriterReaders;
        y = !(y & cWriterMask) && (y & cReadersWaitingMask) ? y & (cWritersWaitingMask | cReadersWaitingMask | cR2WUnchangedFlag) :
            //!(y & cWriterMask) && (y & cWritersWaitingMask) ? 0 : // skip compare_exchange_xxx()
            y & cReadersWaitingMask;
        */
        if (m_WriterReaders.compare_exchange_strong(y, y + 1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            break;
        }
        if (hrt.GetCounter() >= e)
        {
#ifdef CMaaRWLockWp_DBG
            UnSetRDbg();
#endif
            return false;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_R_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
    return true;
}
void CMaaRWLockWp::RUnLock() noexcept
{
#ifdef CMaaRWLockWp_DBG
    UnSetRDbg();
#endif
#ifdef DEBUG_RW_LOCKS
    if (RCnt() <= 0) { rw1_err(); }
#endif
    m_WriterReaders.fetch_sub(1, std::TL_memory_order_acq_rel);
}
void CMaaRWLockWp::WLock() noexcept
{
#ifdef CMaaRWLockWp_DBG
    ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) ? m_W1 : m_W).store(CMaaGetCurrentThreadId(), std::TL_memory_order_release);
#endif
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1, std::TL_memory_order_acq_rel);
    }
    else
    {
        m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);
        int s = DEFAULT_W_MUTEX_SPINS;
        while (1)
        {
            _dword y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
            if (m_WriterReaders.compare_exchange_strong(y, (y & ~cR2WUnchangedFlag) - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (--s)
            {
                continue;
            }
            s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
            std::this_thread::yield();
        }
        m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    }
#ifdef CMaaRWLockWp_DBG
    m_W.store(CMaaGetCurrentThreadId(), std::TL_memory_order_release);
#endif
}
bool CMaaRWLockWp::WLock_us(_qword us) noexcept
{
    if (us == -1)
    {
        WLock();
        return true;
    }
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
#ifdef CMaaRWLockWp_DBG
    ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) ? m_W1 : m_W).store(ThreadId, std::TL_memory_order_release);
#endif
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1, std::TL_memory_order_acq_rel);
    }
    else
    {
        m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);
        const CMaaTime& hrt = GetHRTime(true);
        const _uqword e = hrt.GetNextTime(us, 1000000);
        int s = DEFAULT_W_MUTEX_SPINS;
        while (1)
        {
            _dword y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
            if (m_WriterReaders.compare_exchange_strong(y, (y & ~cR2WUnchangedFlag) - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
            if (hrt.GetCounter() >= e)
            {
                m_WriterReaders.fetch_sub(cWritersWaiting1, std::TL_memory_order_acq_rel);
#ifdef CMaaRWLockWp_DBG
                //m_W = InvalidThrId;
#endif
                return false;
            }
            if (--s)
            {
                continue;
            }
            s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
            std::this_thread::yield();
        }
        m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    }
#ifdef CMaaRWLockWp_DBG
    m_W.store(ThreadId, std::TL_memory_order_release);
#endif
    return true;
}
bool CMaaRWLockWp::RLock(_dword ms, bool bForced) noexcept
{
    if (ms == INFINITE)
    {
        RLock(bForced);
        return true;
    }
    return RLock_us(1000 * (_uqword)ms, bForced);
}
bool CMaaRWLockWp::WLock(_dword ms) noexcept
{
    if (ms == INFINITE)
    {
        WLock();
        return true;
    }
    return WLock_us(1000 * (_uqword)ms);
}

bool CMaaRWLockWp::WUnLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1, std::TL_memory_order_acq_rel);
        return true;
    }
#ifdef DEBUG_RW_LOCKS
    rw1_err();
#endif
    return false;
}
bool CMaaRWLockWp::W2RLock(int n) noexcept
{
    if (n <= 0)
    {
#ifdef DEBUG_RW_LOCKS
        rw1_err();
#endif
        return false;
    }
#ifdef CMaaRWLockWp_DBG
    SetRDbg(n);
#endif
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1 - (_dword)n, std::TL_memory_order_acq_rel);
        return true;
    }
#ifdef DEBUG_RW_LOCKS
    rw1_err();
#endif
#ifdef CMaaRWLockWp_DBG
    UnSetRDbg(n);
#endif
    return false;
}
bool CMaaRWLockWp::W2RLock() noexcept
{
#ifdef CMaaRWLockWp_DBG
    SetRDbg();
#endif
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        if ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders.fetch_sub(cWriter1 - 1, std::TL_memory_order_acq_rel);
        return true;
    }
#ifdef DEBUG_RW_LOCKS
    rw1_err();
#endif
#ifdef CMaaRWLockWp_DBG
    UnSetRDbg();
#endif
    return false;
}
bool CMaaRWLockWp::R2WLock(int nR) noexcept
{
    if (nR <= 0)
    {
#ifdef DEBUG_RW_LOCKS
        rw1_err();
#endif
        return false; // this is error case, not !bUnchanged
    }
#ifdef CMaaRWLockWp_DBG
    UnSetRDbg(nR);
    ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) ? m_W1 : m_W).store(CMaaGetCurrentThreadId(), std::TL_memory_order_release);
#endif
#ifdef DEBUG_RW_LOCKS
    if (RCnt() < nR) { rw1_err(); }
#endif
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1 - nR, std::TL_memory_order_acq_rel); // 1;
#ifdef CMaaRWLockWp_DBG
        m_W.store(ThreadId, std::TL_memory_order_release);
#endif
        return true;
    }

    _dword y = m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);

    // avoid ABA problem by restricting the cases of flag set:
    // 1R, 2R, 1R2W(interrupt)+2R2W, 2W2R, 2R2W(no flag)+1R2W(continue)
    if (!(y & cWritersWaitingMask))
    {
        m_WriterReaders.fetch_or(cR2WUnchangedFlag, std::TL_memory_order_acq_rel);
    }

    m_WriterReaders.fetch_sub(nR, std::TL_memory_order_acq_rel); // --m_WriterReaders;

    bool bUnchanged;
    int s = DEFAULT_W_MUTEX_SPINS;
    while (1)
    {
        y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
        if (m_WriterReaders.compare_exchange_strong(y, y - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            bUnchanged = (m_WriterReaders.fetch_and(~ cR2WUnchangedFlag, std::TL_memory_order_acq_rel) & cR2WUnchangedFlag);
            break;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
    m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
#ifdef CMaaRWLockWp_DBG
    m_W.store(ThreadId, std::TL_memory_order_release);
#endif
    return bUnchanged;
}
bool CMaaRWLockWp::R2WLock() noexcept // int nR == 1
{
#ifdef DEBUG_RW_LOCKS
    if (RCnt() <= 0) { rw1_err(); }
#endif
#ifdef CMaaRWLockWp_DBG
    UnSetRDbg();
    ((m_WriterReaders.load(std::TL_memory_order_acquire) & cWriterMask) ? m_W1 : m_W).store(CMaaGetCurrentThreadId(), std::TL_memory_order_release);
#endif
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), ThreadId))
    {
        m_WriterReaders.fetch_add(cWriter1 - 1, std::TL_memory_order_acq_rel);
#ifdef CMaaRWLockWp_DBG
        m_W.store(ThreadId, std::TL_memory_order_release);
#endif
        return true;
    }

    _dword y = m_WriterReaders.fetch_add(cWritersWaiting1, std::TL_memory_order_acq_rel);

    // avoid ABA problem by restricting the cases of flag set:
    // 1R, 2R, 1R2W(interrupt)+2R2W, 2W2R, 2R2W(no flag)+1R2W(continue)
    if (!(y & cWritersWaitingMask))
    {
        m_WriterReaders.fetch_or(cR2WUnchangedFlag, std::TL_memory_order_acq_rel);
    }

    m_WriterReaders.fetch_sub(1, std::TL_memory_order_acq_rel);

    bool bUnchanged;
    int s = DEFAULT_W_MUTEX_SPINS;
    while (1)
    {
        y = m_WriterReaders.load(std::TL_memory_order_acquire) & (cWritersWaitingMask | cR2WUnchangedFlag);
        if (m_WriterReaders.compare_exchange_strong(y, y - cWritersWaiting1 + cWriter1, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
        {
            bUnchanged = (m_WriterReaders.fetch_and(~ cR2WUnchangedFlag, std::TL_memory_order_acq_rel) & cR2WUnchangedFlag);
            break;
        }
        if (--s)
        {
            continue;
        }
        s = DEFAULT_W_MUTEX_TRY_NEXT_SPINS1;
        std::this_thread::yield();
    }
    m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
#ifdef CMaaRWLockWp_DBG
    m_W.store(ThreadId, std::TL_memory_order_release);
#endif
    return bUnchanged;
}
bool CMaaRWLockWp::IsWOwner() const noexcept
{
    return CMaaThreadIdsEqual(m_WriterThreadId.load(std::TL_memory_order_acquire), CMaaGetCurrentThreadId());
}
//---------------------------------------------------------------------------------------------
template<> void CMaaRWLockerRWp<CMaaRWLockRp>::RLock() noexcept
{
    m_lk->RLock(); r++;
}
template<> void CMaaRWLockerRWp<CMaaRWLockWp>::RLock() noexcept
{
    m_lk->RLock(r != 0); r++;
}
template<> bool CMaaRWLockerRWp<CMaaRWLockRp>::RLock_us(_qword us) noexcept
{
    if (m_lk->RLock_us(us))
    {
        r++;
        return true;
    }
    return false;
}
template<> bool CMaaRWLockerRWp<CMaaRWLockWp>::RLock_us(_qword us) noexcept
{
    if (m_lk->RLock_us(us, r != 0))
    {
        r++;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------
#ifdef TOOLSLIB_RW_LOCKS_EXPERIMENTAL
CMaaRWLockWp2::CMaaRWLockWp2() noexcept
    : m_WriterReaders(0),
    m_WriterThreadId(InvalidThrId)
{
}
CMaaRWLockWp2::~CMaaRWLockWp2() noexcept
{
}
void CMaaRWLockWp2::RLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(TOOLSLIB_LOAD_MO), ThreadId))
    {
        ++m_WriterReaders;
        return;
    }
    while(1)
    {
        _dword y = m_WriterReaders & cReadersMask;
        if (m_WriterReaders.compare_exchange_strong(y, y + 1 TOOLSLIB_CE_WS_MO))
        {
            break;
        }
        //std::this_thread::yield();
#ifdef _WIN32
        if (!SwitchToThread())
        {
            //Sleep(1);
        }
#else
        if (sched_yield())
        {
            //usleep(1000);
        }
#endif
    }
}
void CMaaRWLockWp2::RUnLock() noexcept
{
    --m_WriterReaders;
}
void CMaaRWLockWp2::WLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(TOOLSLIB_LOAD_MO), ThreadId))
    {
        m_WriterReaders += cWriter1;
    }
    else
    {
        while (1)
        {
            _dword y = 0;
            if (m_WriterReaders.compare_exchange_strong(y, cWriter1 TOOLSLIB_CE_WS_MO))
            {
                break;
            }
            //std::this_thread::yield();
#ifdef _WIN32
            if (!SwitchToThread())
            {
                //Sleep(1);
            }
#else
            if (sched_yield())
            {
                //usleep(1000);
            }
#endif
        }
        m_WriterThreadId.store(ThreadId, std::TL_memory_order_release);
    }
}
bool CMaaRWLockWp2::WUnLock() noexcept
{
    const CMaaThreadIdType ThreadId = CMaaGetCurrentThreadId();
    if (CMaaThreadIdsEqual(m_WriterThreadId.load(TOOLSLIB_LOAD_MO), ThreadId))
    {
        if ((m_WriterReaders & cWritersMask) == cWriter1)
        {
            m_WriterThreadId.store(InvalidThrId, std::TL_memory_order_release);
        }
        m_WriterReaders -= cWriter1;
        return true;
    }
    return false;
}
#endif
//---------------------------------------------------------------------------------------------


#ifdef __unix__
//---------------------------------------------------------------------------------------------
#if 0
CMaaWin32Locker::CMaaWin32Locker(CMaaMutex &Lock, CMaaString
#ifndef _DEBUG_MTX
         &
#endif
         Name) noexcept
:   m_Lock(Lock)
#ifdef _DEBUG_MTX
    , m_Name(Name)
#endif
{
    m_x = 0;
    m_bUnLockerMode = false;
}
CMaaWin32Locker::CMaaWin32Locker(CMaaMutex &Lock, const char * Name) noexcept
:   m_Lock(Lock)
    //, m_Name(Name)
{
    m_x = 0;
    m_bUnLockerMode = false;
}
CMaaWin32Locker::CMaaWin32Locker(CMaaMutex &Lock) noexcept
:   m_Lock(Lock)
{
    m_x = 0;
    m_bUnLockerMode = false;
}
CMaaWin32Locker::~CMaaWin32Locker() noexcept
{
    if  ((m_x > 0 && !m_bUnLockerMode) || m_x != 0)
    {
        while(m_x > 0)
        {
            m_Lock.UnLockM();
            --m_x;
        }
        while(m_x < 0)
        {
            m_Lock.LockM();
            ++m_x;
        }
    }
}
#ifndef DEBUG_MTX
DWORD CMaaWin32Locker::Lock() noexcept
{
    //DWORD x = m_Lock.Lock();
#ifdef DEBUG_MTX
    m_Lock.LockF("unknown", -2);
#else
    m_Lock.Lock();
#endif
    //if   (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    {
        ++m_x;
    }
    //     return x;
    return 0;
}
#endif
DWORD CMaaWin32Locker::Lock(const char * /*txt*/) noexcept
{
    //     DWORD x = m_Lock.Lock(txt);
    //DWORD x = m_Lock.Lock();
#ifdef DEBUG_MTX
    m_Lock.LockF("unknown", -2);
#else
    m_Lock.Lock();
#endif
    //if   (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    {
        ++m_x;
    }
    //     return x;
    return 0;
}
/*
DWORD CMaaWin32Locker::Lock(DWORD dwTimeToWait)
{
     DWORD x = m_Lock.Lock(dwTimeToWait);
     if   (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
     {
          ++m_x;
     }
     return x;
}
*/
DWORD CMaaWin32Locker::LockF(const char * file, int line) noexcept
{
    //     DWORD x = m_Lock.LockF(file, line);
    //     if   (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    if  (m_Lock.LockF(file, line))
    {
        ++m_x;
    }
    //     return x;
    return 0;
}
int CMaaWin32Locker::UnLock() noexcept
{
#ifdef DEBUG_MTX
    return UnLockF("unknown", -2);
#else
    if  (m_x <= 0 && !m_bUnLockerMode)
    {
        return -1;
    }
    --m_x;
    return m_Lock.UnLock();
#endif
}
int CMaaWin32Locker::UnLockF(const char * file, int line) noexcept
{
    if  (m_x <= 0 && !m_bUnLockerMode)
    {
        return -1;
    }
    --m_x;
    return m_Lock.UnLockF(file, line);
}
void CMaaWin32Locker::SetUnLockerMode(bool bSet) noexcept
{
    m_bUnLockerMode = bSet;
}
#endif
//---------------------------------------------------------------------------------------------
#endif


//==============================================================

/*
LONG __cdecl InterlockedCompareExchange(
  _Inout_ LONG volatile *Destination,
  _In_    LONG          Exchange,
  _In_    LONG          Comparand
)
{
     // Lock
     LONG r = *Destination;
     if (r == Comparand)
     {
          *Destination = Exchange;
     }
     // UnLock
     return r;
}
*/

#ifdef _WIN32
bool fnMaaSafeMakeStatic(LONG volatile *pFlag) noexcept
{
    if  (*pFlag == 2)
    {
        return false;
    }
    LONG f;
    while((f = InterlockedCompareExchange(pFlag, 1, 0)) == 1)
    {
        Sleep(1);
    }
    if  (f == 0)
    {
        return true;
    }
    //if (*pFlag == 2)
    //{
    return false;
    //}
}
void fnMaaSafeStaticMade(LONG volatile *pFlag) noexcept
{
    InterlockedCompareExchange(pFlag, 2, 1);
}
#else
bool fnMaaSafeMakeStatic(int volatile *pFlag, CMaa_gLock_lib_Mutex &m) noexcept
{
    if  (*pFlag == 2)
    {
        return false;
    }
    m.LockM();
    if  (*pFlag == 0)
    {
        *pFlag = 1;
        return true;
    }
    // error
    m.UnLockM();
    return false;
}
void fnMaaSafeStaticMade(int volatile *pFlag, CMaa_gLock_lib_Mutex &m) noexcept
{
    if  (*pFlag == 1)
    {
        *pFlag = 2;
        m.UnLockM();
    }
}
#endif

#ifdef _WIN32
CMaaSafeMakeStatic::CMaaSafeMakeStatic(LONG volatile *pFlag) noexcept
#else
CMaaSafeMakeStatic::CMaaSafeMakeStatic(int volatile *pFlag) noexcept
#endif
{
    m_pFlag = pFlag;
    m_f = 0;
}
bool CMaaSafeMakeStatic::Make() noexcept
{
    if  (m_f == 0)
    {
#ifdef _WIN32
        bool f = fnMaaSafeMakeStatic(m_pFlag);
#else
        bool f = fnMaaSafeMakeStatic(m_pFlag, gLock);
#endif
        m_f = f ? 1 : 2;
        return f;
    }
    return m_f == 1;
}

bool CMaaSafeMakeStatic::ToInit_() noexcept
{
    return m_f == 1;
}

CMaaSafeMakeStatic::~CMaaSafeMakeStatic() noexcept
{
    Done();
}

bool CMaaSafeMakeStatic::Done() noexcept
{
    if  (m_f == 1)
    {
#ifdef _WIN32
        fnMaaSafeStaticMade(m_pFlag);
#else
        fnMaaSafeStaticMade(m_pFlag, gLock);
#endif
        m_f = 2;
        return true;
    }
    return false;
}

#ifndef _WIN32
//CMaaMutex CMaaSafeMakeStatic::s_Mutex;
#endif

#if 0
CMaaStaticStringsAllocator gStaticStringsAllocator;

CMaaStaticStringsAllocator::CMaaStaticStringsAllocator(int N)
:   m_n(0),
    m(N),
    m_bDontDel(false)
{
}
CMaaString & CMaaStaticStringsAllocator::Alloc(const char * str)
{
    CMaaString * p = TL_NEW CMaaString(str);
    if  (p)
    {
        CMaaWin32Locker_aLocker_gLock_Atomic;
        aLocker_Lock;
        try
        {
            m[m_n] = p;
            m_n++;
        }
        catch(...)
        {
        }
        aLocker_UnLock;
    }
    return *p;
}
CMaaString & CMaaStaticStringsAllocator::Alloc(CMaaString str)
{
    CMaaString * p = TL_NEW CMaaString(str);
    if  (p)
    {
        CMaaWin32Locker_aLocker_gLock_Atomic;
        aLocker_Lock;
        try
        {
            m[m_n] = p;
            m_n++;
        }
        catch(...)
        {
        }
        aLocker_UnLock;
    }
    return *p;
}
CMaaStaticStringsAllocator::~CMaaStaticStringsAllocator()
{
    if  (!m_bDontDel)
    {
        for (int i = m_n; i--; )
        {
            //__utf8_printf("delete m[%d];\n", i);
            delete m[i];
        }
    }
}
void CMaaStaticStringsAllocator::SetDontDel(bool f)
{
    m_bDontDel = f;
}
#endif

#if 0
MAA_STATIC_FLAG Flag1;
MAA_STATIC_FLAG Flag2;
MAA_STATIC_FLAG Flag3;
static CMaaMutex * pMutex = nullptr;

class CMaaThreadTestStaticCreate : public CMaaThread
{
    HANDLE &m_h;
    int m_n;
public:
    CMaaThreadTestStaticCreate(HANDLE &h, int n)
    :   m_h(h),
        m_n(n)
    {
    }
    unsigned Run()
    {
        WaitForSingleObject(m_h, INFINITE);
        {
            CMaaSafeMakeStatic lc1(&Flag3);
            /*
               if (lc1.Make())
               {
                    __utf8_printf("1(%d)\n", m_n);
                    return 1;
               }
               else
               {
                    __utf8_printf("0(%d)\n", m_n);
               }
               */
            lc1.Make();
            static CMaaString &x = gStaticStringsAllocator.Alloc("aaaaaaaaa");
            static CMaaString  y = gStaticStringsAllocator.Alloc("bbbbbbbbb");
            lc1.Done();
            __utf8_printf(" %02d %S %p %p\n", m_n, &x, &x, (const char *)x);
            //CMaaString xx = *x;
            //__utf8_printf("%S\n", x);
            //__utf8_printf("%S\n", &xx);
        }
        return 0;
    }

    ADD_ALLOCATOR(CMaaThreadTestStaticCreate)
};

DEF_ALLOCATOR(CMaaThreadTestStaticCreate)

int m99()
{
    if  (fnMaaSafeMakeStatic(&Flag1))
    {
        pMutex = TL_NEW CMaaMutex();
        fnMaaSafeStaticMade(&Flag1);
        return 1;
    }
    //pMutex->
    return 0;
}
int main()
{
    /*__utf8_printf("%d ", m99());
     __utf8_printf("%d ", m99());
     __utf8_printf("%d\n", m99());*/
    __utf8_printf("%d %d %d\n", m99(), m99(), m99());

    {
        CMaaSafeMakeStatic lc1(&Flag2);
        if  (lc1.Make())
        {
            __utf8_printf("1\n");
        }
        else
        {
            __utf8_printf("0\n");
        }
    }
    CMaaSafeMakeStatic lc2(&Flag2);
    if  (lc2.Make())
    {
        __utf8_printf("1\n");
    }
    else
    {
        __utf8_printf("0\n");
    }

    HANDLE h = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    const int N = 20;
    CMaaThreadTestStaticCreate * pt[N];
    int i;
    for (i = 0; i < N; i++)
    {
        pt[i] = TL_NEW CMaaThreadTestStaticCreate(h, i);
        pt[i]->Create();
    }
    Sleep(100);
    SetEvent(h);
    Sleep(100);
    for (i = 0; i < N; i++)
    {
        delete pt[i];
    }
    CloseHandle(h);
    return 0;
}
#endif

static bool gCMaaToolLib_crt_Initialized = false;
int gCMaaToolLib_crt_Initializer() noexcept
{
    if (!gCMaaToolLib_crt_Initialized)
    {
        if (!CMaaString::CheckRuntimeConditions())
        {
            exit(101);
        }
        TL_DbgAlloc;

        gCMaaToolLib_crt_Initialized = true;

        __GLock__Atomic(true);
        __GLock__Atomic2(true);
        __GLock__lib(true);
        __GLock__lib2(true);
        __GLock__usr(true);
        __GLock__usr2(true);
        __GLock__usr3(true);
        __GLock__AllocatorBasicLock(true);
        //__GLock__sfnInitLock(true);
        __GLock__SyncAssignLockUpperLock(true);
        //CMaaString::Init_sp_NullImp();
#if TOOLSLIB_USE_CMAASTRING64 == 2
        //CMaaString64::Init_sp_NullImp();
#endif
        //CMaaString aaa = CMaaTLGlobalString(CMaaTLGlobalStrings::e_max/*, true*/);
//#if TOOLSLIB_USE_CMAASTRING64 == 2
        //CMaaString64 ccc = CMaaTLGlobalString64(CMaaTLGlobalStrings64::e_max/*, true*/);
//#endif
        //const CMaaString aaa2 = CMaaStringJsonFloat;

        CMaaTmpSprintfBuffer* pTmpBuffer;
        CMaaTmpSprintfBuffer* pTmpFormatBuffer;
        CMaaTmpSprintf2StringsArray* pStringsArray;
        CMaaString::GetRetSprintfBuffers(0, pTmpBuffer, pTmpFormatBuffer); // init static CMaaDListAD 's
        CMaaString::GetRetSprintf2Buffers(0, pTmpBuffer, pTmpFormatBuffer, pStringsArray); // init static CMaaDListAD 's
#if TOOLSLIB_USE_CMAASTRING64 == 2
        CMaaTmpSprintf2StringsArray64 * pStringsArray64;
        CMaaString64::GetRetSprintfBuffers(0, pTmpBuffer, pTmpFormatBuffer); // init static CMaaDListAD 's
        CMaaString64::GetRetSprintf2Buffers(0, pTmpBuffer, pTmpFormatBuffer, pStringsArray64); // init static CMaaDListAD 's
#endif

        //CMaaGetCpuCount();
        
        GetHRTime(true);

        GetWsaErrorMessage(0);

        //InitsCMaaXml256Bits();

        gCMaaToolLib_crt_Initialized = true;
    }
    return 0;
}

struct CMaaToolLib_crt_Initializer
{
    CMaaToolLib_crt_Initializer()
    {
        gCMaaToolLib_crt_Initializer();
        /*
        {
            CMaaWin32Locker<CMaaLiteMutex> l_2(gLock_Atomic2);
            l_2.Lock();
            CMaaWin32Locker<CMaaLiteMutex> l_1(gLock_Atomic);
            l_1.Lock();
            //l_1.UnLock();
            //l_2.UnLock();
        }
        */
        
        //CMaaGetCpuCount();
        
        GetUsTime();
        GetMsTime();

        //CMaaString s1, s2;
        //s1.Format( "%d", 1);
        //s2.Format2("%d", "%1", 1);
#if 0
        CMaaXmlDocument doc(CMaaStringDoc, CMaaStringDoc);
        CMaaXmlElement e = doc.DocumentElement(); // doc.CreateElement("a");
        //e.AddAttribute("a", CMaaStringZ);
        e.AddAttribute(CMaaString_a /*CMaaStrCh('a', true)*/, CMaaStringZ);
        //e.FindNodeWithAttrRO("b", "c", "d");
#endif
        //Init_CMaaXmlNodeImpl_statics();

#if TOOLSLIB_USE_CMAASTRING64 == 2
        //CMaaString64 s64_1, s64_2;
        //s64_1.Format("%d", 1);
        //s64_2.Format2("%d", "%1", 1);
#endif

#ifndef TOOLSLIB_SINGLE_THREAD
#if 0
        CMaaConnectionInTimeCharacteristics p1(1);
#endif
#ifndef _WIN32
        //HANDLE h3 = CreateEvent(nullptr, false, false, nullptr);
        //CloseHandle(h3);
#endif
#endif
    }
};

static CMaaToolLib_crt_Initializer s_CMaaToolLib_crt_Initializer;

#ifdef DEBUG_RW_LOCKS
std::atomic<int> f_n_rw_err = 0;
std::atomic<int> f_n_rw1_err = 0;

int rw_err()
{
    return ++f_n_rw_err;
}

int rw1_err()
{
    return ++f_n_rw_err;
}
#endif
