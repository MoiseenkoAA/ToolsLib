
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/mthrlib2.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * events, timed locks in Linux. Experimental code.
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

//#define DEBUG_RW_LOCKS

#ifdef CONSTEXPR_GLOCK_LIB_ATOMIC // for #define CMaa_gLock_lib_Mutex CMaaAtomicFastMutex2W
inline CMaa_gLock_lib_Mutex* __GLock__lib() noexcept
{
    static constexpr CMaa_gLock_lib_Mutex g;
    return (CMaa_gLock_lib_Mutex*)&g;
}
inline CMaa_gLock_lib_Mutex* __GLock__lib(bool) noexcept
{
    return __GLock__lib();
}
#else
inline CMaa_gLock_lib_Mutex* __GLock__lib(bool bInit = false) noexcept
{
    static CMaa_gLock_lib_Mutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
        gptr = TL_NEW_ CMaa_gLock_lib_Mutex;
#else
        static CMaa_gLock_lib_Mutex g;
        gptr = &g;
#endif
#ifndef TOOLSLIB_FAST_gLock_lib_Mutex
        //gptr->AddRef();
#endif
    }
    return gptr;
}
#endif

inline CMaaMutex* __GLock__lib2(bool bInit = false) noexcept
{
    static CMaaMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
        gptr = TL_NEW_ CMaaMutex;
#else
        static CMaaMutex g;
        gptr = &g;
#endif
#ifndef TOOLSLIB_FAST_MUTEXES
        //gptr->AddRef();
#endif
    }
    return gptr;
}
#ifdef CONSTEXPR_GLOCK_USR_ATOMIC
inline CMaa_gLock_usr_Mutex* __GLock__usr() noexcept
{
    static constexpr CMaa_gLock_usr_Mutex g;
    return (CMaa_gLock_usr_Mutex*)&g;
}
inline CMaa_gLock_usr_Mutex* __GLock__usr(bool) noexcept
{
    return __GLock__usr();
}
#else
inline CMaa_gLock_usr_Mutex* __GLock__usr(bool bInit = false) noexcept
{
    static CMaa_gLock_usr_Mutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
        gptr = TL_NEW_ CMaa_gLock_usr_Mutex;
#else
        static CMaa_gLock_usr_Mutex g;
        gptr = &g;
#endif
#ifndef TOOLSLIB_FAST_gLock_usr_Mutex
        //gptr->AddRef();
#endif
    }
    return gptr;
}
#endif
inline CMaaMutex* __GLock__usr2(bool bInit = false) noexcept
{
    static CMaaMutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
        gptr = TL_NEW_ CMaaMutex;
#else
        static CMaaMutex g;
        gptr = &g;
#endif
#ifndef TOOLSLIB_FAST_MUTEXES
        //gptr->AddRef();
#endif
    }
    return gptr;
}
#ifdef CONSTEXPR_GLOCK_USR_ATOMIC
inline CMaa_gLock_usr3_Mutex* __GLock__usr3() noexcept
{
    static constexpr CMaa_gLock_usr3_Mutex g;
    return (CMaa_gLock_usr3_Mutex*)&g;
}
inline CMaa_gLock_usr3_Mutex* __GLock__usr3(bool) noexcept
{
    return __GLock__usr3();
}
#else
inline CMaa_gLock_usr3_Mutex* __GLock__usr3(bool bInit = false) noexcept
{
    static CMaa_gLock_usr3_Mutex* gptr = nullptr;
    if (bInit && !gptr)
    {
        TL_DbgAlloc;
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
        gptr = TL_NEW_ CMaa_gLock_usr3_Mutex;
#else
        static CMaa_gLock_usr3_Mutex g;
        gptr = &g;
#endif
#ifndef TOOLSLIB_FAST_gLock_usr3_Mutex
        //gptr->AddRef();
#endif
    }
    return gptr;
}
#endif
inline CMaa_gLock_lib_Mutex& Get_gLock() noexcept
{
    return gLock_lib;
}


class CMaaEvent
{
#ifdef __unix__
    CMaaMutex1 m_Mutex;
    pthread_cond_t m_cond;
    bool m_bManualReset, m_bState, m_Padding[2];
#else
    CMaaMutex m_Mutex;
    bool m_bManualReset, m_Padding[3];
    HANDLE m_hEvent;
    //CMaaAutoInitObject<int, 0> m_ExternalEvent;
    int m_ExternalEvent = 0;
#endif
    int m_nWaiters, m_nWakeups, m_nPulseCount;
public:
    //CMaaEvent(bool bManualReset = false, bool bInitialState = false /*for ManualReset event*/);
    CMaaEvent(bool bManualReset = true, bool bInitialState = false /*for ManualReset event*/) noexcept;
    CMaaEvent(const CMaaEvent &That) = delete;
    CMaaEvent(CMaaEvent&& That) = delete;
    CMaaEvent& operator=(const CMaaEvent &That) = delete;
    CMaaEvent& operator=(CMaaEvent&& That) = delete;
#ifdef _WIN32
    CMaaEvent(HANDLE h, bool bManualReset = true, int DoNotCloseEventHandle = 1) noexcept;
    operator HANDLE () const noexcept
    {
        return m_hEvent;
    }
#endif
    ~CMaaEvent();
    void WakeOne() noexcept;
    void WakeAll() noexcept;
    bool Wait(_dword TimeMs = (_dword)-1) noexcept;
    bool Wait(_qword TimeUs) noexcept;
    bool Reset() noexcept; // Reset manual reset mutex
    bool Set() noexcept
    {
        WakeAll();
        return true;
    }
    bool Pulse() noexcept; // with race conditions
    /*
     bool Pulse() noexcept
     {
          if   (m_bManualReset)
          {
               // wake all and reset
          }
          else
          {
               // wake one and reset
          }
          return true;
     }
     */
};

class CMaaTimedMutex : public
#ifdef __unix__
CMaaMutex2
#else
CMaaMutex2
#endif
{
#ifdef __unix__
    pthread_cond_t m_cond;
    int m_Status; //number of recursive locks
    pthread_t m_curr;
#endif
public:
    CMaaTimedMutex() noexcept;
    ~CMaaTimedMutex() noexcept;
#ifdef __unix__
    bool Lock(_dword ms) noexcept;
    bool Lock(_qword us = (_qword)-1) noexcept;
#else
    // mtx.Lock/UnLock is 11x faster under linux64(timed mtx) rather than win64
    bool Lock(_dword ms = (_dword)-1) noexcept;
    bool Lock(_qword us) noexcept;
#endif
    bool UnLock() noexcept;
    bool TryLock() noexcept;

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock(); }
};

#ifdef TOOLSLIB_RW_LOCKS_EXPERIMENTAL
class CMaaRWLock // native win32 & linux timed rw lock
{
    //CMaaTimedMutex m_m1;
    //CMaaAtomicFastMutex m_m1;
    CMaaFastMutex m_m1;
    //CMaaMutex m_m2;
    CMaaAtomicFastMutex1 m_m2;
    CMaaEvent m_h;
    int m_n;           // read locks count
    char m_AccessType; // 'w' - write lock, 'r' - read lock, '\0' - not locked
    //bool m_hState;
public:
    CMaaRWLock() noexcept;
    ~CMaaRWLock() noexcept;
    CMaaRWLock(const CMaaRWLock &That) = delete;
    CMaaRWLock(CMaaRWLock&& That) = delete;
    CMaaRWLock& operator=(const CMaaRWLock &That) = delete;
    CMaaRWLock& operator=(CMaaRWLock&& That) = delete;
    // mtx.Lock/UnLock is 130x faster under linux64(not timed version) rather than win64
    // mtx.Lock/UnLock is 11x faster under linux64(timed mtx) rather than win64

    // RLock+RUnLock is 2.5x slower then mtx.Lock/UnLock (win64)
    // RLock+RUnLock is 16x slower then mtx.Lock/UnLock (linux64)
    bool RLock(_dword Time = (_dword)-1, int n = 1) noexcept;
    bool RUnLock(int n = 1) noexcept;
    // WLock+WUnLock is 3.5x slower then mtx.Lock/UnLock (win64)
    // WLock+WUnLock is 17x slower then mtx.Lock/UnLock (linux64)
    bool WLock(_dword Time = (_dword)-1) noexcept;
    void WUnLock() noexcept;
    void UnLock(int n = 1) noexcept; // Unlock Single Write lock or n Read locks
    bool W2RLock(int n = 1) noexcept;
    bool R2WLock(_dword Time = (_dword)-1, int n = 1) noexcept;
};

class CMaaRWLock2 // native win32 & linux not timed rw lock other linux implementation
{
#ifdef _WIN32
    CMaaRWLock m_Lock; // Linux can use CMaaRWLock too
#else
    pthread_mutex_t m_Mutex;
    pthread_cond_t m_CondRead, m_CondWrite;
    int m_Readers, m_Writers, m_Read_waiters, m_Write_waiters;
#endif
public:
    CMaaRWLock2() noexcept;
    ~CMaaRWLock2() noexcept;
    void RLock(int n = 1) noexcept;
    void RUnLock(int n = 1) noexcept;
    void WLock() noexcept;
    void WUnLock() noexcept;
    bool W2RLock(int n = 1) noexcept;
    bool R2WLock_(int n = 1) noexcept;
};
#endif
//---------------------------------------------------------------------------------------------
class CMaaRWLockRmp // atomic win32 & linux not timed rw lock Readers max priority
{
    std::atomic<_dword> m_WriterReaders;
#ifdef _WIN32
    static constexpr DWORD InvalidThrId{ CMaaInvalidThreadId() };
    std::atomic<DWORD> m_WriterThreadId;
#else
    static constexpr pthread_t InvalidThrId{ CMaaInvalidThreadId() };
    std::atomic<pthread_t> m_WriterThreadId;
#endif
    static constexpr _dword cWriter1 = ((_dword)1) << (sizeof(_dword) * 8 - 8);
    static constexpr _dword cWriterMask = ((_dword)-1) << (sizeof(_dword) * 8 - 8);
public:
    CMaaRWLockRmp() noexcept;
    ~CMaaRWLockRmp() noexcept;
    void RLock() noexcept;
    void RUnLock() noexcept;
    void WLock() noexcept;
    bool WUnLock() noexcept; // false on error
    //bool W2RLock(int n = 1) noexcept;
    //bool R2WLock_(int n = 1) noexcept;

    CMaaRWLockRmp(const CMaaRWLockRmp&) = delete;
    CMaaRWLockRmp(CMaaRWLockRmp&&) = delete;
    CMaaRWLockRmp& operator=(const CMaaRWLockRmp&) = delete;
    CMaaRWLockRmp& operator=(CMaaRWLockRmp&&) = delete;
};
//---------------------------------------------------------------------------------------------
#ifdef DEBUG_RW_LOCKS
int rw1_err();
#endif
class CMaaRWLockRp // atomic win32 & linux not timed rw lock Writers priority
{
    // If where is one or more readers when RLock is available
    // If no readers and no writers when WLock has more priority then RLock

    static constexpr CMaaThreadIdType InvalidThrId{ CMaaInvalidThreadId() };

    std::atomic<_dword> m_WriterReaders;
    std::atomic<CMaaThreadIdType> m_WriterThreadId;

    static constexpr _dword cR2WUnchangedFlag = ((_dword)1) << (sizeof(_dword) * 8 - 1);
    static constexpr _dword cWriter1 = ((_dword)1) << (sizeof(_dword) * 8 - 6);
    static constexpr _dword cMaxWriterLocks = (_dword)0x1f;
    static constexpr _dword cWriterMask = cMaxWriterLocks << (sizeof(_dword) * 8 - 6); // & ~ cR2WUnchangedFlag;
    static constexpr _dword cWritersWaiting1 = ((_dword)1) << (sizeof(_dword) * 8 - 6 - 13);
    static constexpr _dword cMaxWaitingWriters = (_dword)0x1fff;
    static constexpr _dword cWritersWaitingMask = cMaxWaitingWriters << (sizeof(_dword) * 8 - 6 - 13);
    static constexpr _dword cWritersMask = (((_dword)-1) << (sizeof(_dword) * 8 - 6 - 13)) & ~ cR2WUnchangedFlag;
    static constexpr _dword cMaxReaders = (_dword)0x1fff;
    static constexpr _dword cReadersWaitingMask = ((_dword)-1) >> (6 + 13);
public:
    CMaaRWLockRp() noexcept;
    ~CMaaRWLockRp() noexcept;
    void RLock() noexcept;
    void RLock_() noexcept { RLock(); }
    void RUnLock() noexcept;
    void WLock() noexcept;
    bool WUnLock() noexcept; // false on error
    bool W2RLock(int n) noexcept;
    bool W2RLock() noexcept;
    bool /*pbUnchanged*/ R2WLock(int nR) noexcept;
    bool /*pbUnchanged*/ R2WLock() noexcept; // nR = 1
    bool IsWOwner() const noexcept;

    bool RLock_us(_qword us) noexcept; // false on timeout
    bool WLock_us(_qword us) noexcept; // false on timeout
    bool RLock(_dword ms) noexcept; // false on timeout
    bool WLock(_dword ms) noexcept; // false on timeout

    CMaaRWLockRp(const CMaaRWLockRp&) = delete;
    CMaaRWLockRp(CMaaRWLockRp&&) = delete;
    CMaaRWLockRp& operator=(const CMaaRWLockRp&) = delete;
    CMaaRWLockRp& operator=(CMaaRWLockRp&&) = delete;
};
//---------------------------------------------------------------------------------------------
#ifdef _WIN32
//#define CMaaRWLockWp_DBG
#endif
class CMaaRWLockWp
{
#ifdef CMaaRWLockWp_DBG
    void SetRDbg(int n = 1) noexcept;
    void UnSetRDbg(int n = 1) noexcept;
#endif

    // If where is one or more waiting writers or writer when WLock has more priority then RLock
    // If no readers and no writers when WLock has more priority then RLock

    static constexpr CMaaThreadIdType InvalidThrId{ CMaaInvalidThreadId() };

    std::atomic<_dword> m_WriterReaders;
    std::atomic<CMaaThreadIdType> m_WriterThreadId;

#ifdef CMaaRWLockWp_DBG
    static constexpr int MaxRCount = 32;
    std::atomic<CMaaThreadIdType> m_R[MaxRCount], m_W, m_W1;
#endif
    static constexpr _dword cR2WUnchangedFlag = ((_dword)1) << (sizeof(_dword) * 8 - 1);
    static constexpr _dword cWriter1 = ((_dword)1) << (sizeof(_dword) * 8 - 6);
    static constexpr _dword cMaxWriterLocks = (_dword)0x1f;
    static constexpr _dword cWriterMask = cMaxWriterLocks << (sizeof(_dword) * 8 - 6); // & ~ cR2WUnchangedFlag;
    static constexpr _dword cWritersWaiting1 = ((_dword)1) << (sizeof(_dword) * 8 - 6 - 13);
    static constexpr _dword cMaxWaitingWriters = (_dword)0x1fff;
    static constexpr _dword cWritersWaitingMask = cMaxWaitingWriters << (sizeof(_dword) * 8 - 6 - 13);
    static constexpr _dword cWritersMask = (((_dword)-1) << (sizeof(_dword) * 8 - 6 - 13)) & ~cR2WUnchangedFlag;
    static constexpr _dword cMaxReaders = (_dword)0x1fff;
    static constexpr _dword cReadersWaitingMask = ((_dword)-1) >> (6 + 13);
public:
    CMaaRWLockWp() noexcept;
    ~CMaaRWLockWp() noexcept;
    void RLock(bool bForced) noexcept;
    void RLock_() noexcept { RLock(false); }
    void RUnLock() noexcept;
    void WLock() noexcept;
    bool WUnLock() noexcept; // false on error
    bool W2RLock(int n) noexcept;
    bool W2RLock() noexcept;
    bool /*pbUnchanged*/ R2WLock(int nR) noexcept;
    bool /*pbUnchanged*/ R2WLock() noexcept; // nR = 1
    bool IsWOwner() const noexcept;

    bool RLock_us(_qword us, bool bForced) noexcept; // false on timeout
    bool WLock_us(_qword us) noexcept; // false on timeout
    bool RLock(_dword ms, bool bForced) noexcept; // false on timeout
    bool WLock(_dword ms) noexcept; // false on timeout

    void GetStat(int& r, int& w, int& ww) noexcept
    {
        _dword x = m_WriterReaders;
        r = x & cMaxReaders;
        w = (x >> (sizeof(_dword) * 8 - 6)) & cMaxWriterLocks;
        ww = (x >> (sizeof(_dword) * 8 - 6 - 13)) & cMaxWaitingWriters;
    }
#ifdef CMaaRWLockWp_DBG
    int RCnt() const noexcept
    {
#ifdef CMaaRWLockWp_DBG
        const CMaaThreadIdType dw = CMaaGetCurrentThreadId();
        if ((m_WriterReaders & cReadersWaitingMask))
        {
            int nn = 0;
            for (int i = 0; i < MaxRCount; i++)
            {
                if (CMaaThreadIdsEqual(m_R[i].load(TOOLSLIB_LOAD_MO), dw))
                {
                    nn++;
                }
            }
            return nn;
        }
        return 0;
#else
        return m_WriterReaders & cReadersWaitingMask;
#endif
    }
    int WCnt() const noexcept
    {
#ifdef CMaaRWLockWp_DBG
        if (CMaaThreadIdsEqual(m_WriterThreadId.load(TOOLSLIB_LOAD_MO), CMaaGetCurrentThreadId()))
        {
            return (m_WriterReaders & cWriterMask) >> (sizeof(_dword) * 8 - 6);
        }
        return 0;
#else
        return (m_WriterReaders & cWriterMask) >> (sizeof(_dword) * 8 - 6);
        /* or wcount for current thread:
        if (CMaaThreadIdsEqual(m_WriterThreadId.load(TOOLSLIB_LOAD_MO), CMaaGetCurrentThreadId()))
        {
            return (m_WriterReaders & cWriterMask) >> (sizeof(_dword) * 8 - 6);
        }
        return 0;
        */
#endif
    }
#endif

    CMaaRWLockWp(const CMaaRWLockWp&) = delete;
    CMaaRWLockWp(CMaaRWLockWp&&) = delete;
    CMaaRWLockWp& operator=(const CMaaRWLockWp&) = delete;
    CMaaRWLockWp& operator=(CMaaRWLockWp&&) = delete;
};
//---------------------------------------------------------------------------------------------
#ifdef DEBUG_RW_LOCKS
int rw_err();
#endif
template<class T> class CMaaRWLockerRWp
{
protected:
    T *m_lk; // CMaaRWLockRp or CMaaRWLockWp
    int w, r;
public:
    CMaaRWLockerRWp(T& lk) noexcept
    :   m_lk(&lk),
        w(0),
        r(0)
    {
    }
    ~CMaaRWLockerRWp() noexcept
    {
        UnLockAll();
    }
    CMaaRWLockerRWp(CMaaRWLockerRWp<T>&& o) noexcept
    :   m_lk(o.m_lk),
        w(o.w),
        r(o.r)
    {
        o.w = o.r = 0;
    }
    CMaaRWLockerRWp& operator=(CMaaRWLockerRWp<T>&& o) noexcept
    {
        if (this != &o)
        {
            UnLockAll();
            m_lk = o.m_lk;
            w = o.w;
            r = o.r;
            o.w = o.r = 0;
        }
        return *this;
    }
    void RLock() noexcept;
    bool RUnLock() noexcept
    {
        if (r)
        {
            --r;
            m_lk->RUnLock();
            return true;
        }
#ifdef CMaaRWLockWp_DBG
        ++gGLocks.gRW_RUErrors;
#endif
        return false;
    }
    void WLock() noexcept
    {
#ifdef CMaaRWLockWp_DBG
        if (r && !w)
        {
            ++gGLocks.gRW_WErrors;
        }
#endif
        m_lk->WLock();
        w++;
    }
    bool WUnLock() noexcept
    {
        if (w && m_lk->WUnLock())
        {
            --w;
            return true;
        }
#ifdef CMaaRWLockWp_DBG
        ++gGLocks.gRW_WUErrors;
#endif
        return false;
    }
    bool W2RLock(int n) noexcept
    {
        if (w && m_lk->W2RLock(n))
        {
            --w;
            r += n;
            return true;
        }
#ifdef CMaaRWLockWp_DBG
        gGLocks.gRW_WRErrors += n;
#endif
        return false;
    }
    bool W2RLock() noexcept
    {
        if (w && m_lk->W2RLock())
        {
            --w;
            ++r;
            return true;
        }
#ifdef CMaaRWLockWp_DBG
        ++gGLocks.gRW_WRErrors;
#endif
        return false;
    }
    bool /*bUnchanged*/ R2WLock(bool *pOk) noexcept
    {
        if (r == 1 || (r > 0 && w))
        {
            if (pOk)
            {
                *pOk = true;
            }
            --r;
            ++w;
            return m_lk->R2WLock(); // bUnchanged
        }
        if (pOk)
        {
            *pOk = false;
        }
#ifdef CMaaRWLockWp_DBG
        ++gGLocks.gRW_RWErrors;
#endif
        return false; // but error
    }
    bool /*bUnchanged*/ R2WLock() noexcept
    {
        if (r == 1 || (r > 0 && w))
        {
            --r;
            ++w;
            return m_lk->R2WLock(); // bUnchanged
        }
#ifdef CMaaRWLockWp_DBG
        ++gGLocks.gRW_RWErrors;
#endif
        return false; // but error
    }
    bool IsWOwner() const noexcept
    {
        return m_lk->IsWOwner();
    }
    bool RLock_us(_qword us) noexcept;
    bool WLock_us(_qword us) noexcept
    {
        if (m_lk->WLock_us(us))
        {
            w++;
            return true;
        }
        return false;
    }
    bool RLock(_dword ms) noexcept
    {
        if (m_lk->RLock(ms))
        {
            r++;
            return true;
        }
        return false;
    }
    bool WLock(_dword ms) noexcept
    {
        if (m_lk->WLock(ms))
        {
            w++;
            return true;
        }
        return false;
    }
    void UnLockAll() noexcept
    {
        while (w)
        {
            --w;
            m_lk->WUnLock();
        }
        while (r)
        {
            --r;
            m_lk->RUnLock();
        }
    }
    int WCount() const noexcept
    {
        return w;
    }
    int RCount() const noexcept
    {
        return r;
    }

    CMaaRWLockerRWp(const CMaaRWLockerRWp&) = delete;
    CMaaRWLockerRWp& operator=(const CMaaRWLockerRWp&) = delete;
};
//---------------------------------------------------------------------------------------------

#ifndef TOOLSLIB_RW_LOCKS_EXPERIMENTAL
#define CMaaRWLock CMaaRWLockWp
#define CMaaRWLocker CMaaRWLockerRWp
#endif

#ifdef TOOLSLIB_RW_LOCKS_EXPERIMENTAL
class CMaaRWLockWp2 // atomic win32 & linux not timed rw lock Writers more priority than CMaaRWLockRp but less then CMaaRWLockWp
{
    std::atomic<_dword> m_WriterReaders;
#ifdef _WIN32
    static constexpr DWORD InvalidThrId{ CMaaInvalidThreadId() };
    std::atomic<DWORD> m_WriterThreadId;
#else
    static constexpr pthread_t InvalidThrId{ CMaaInvalidThreadId() };
    std::atomic<pthread_t> m_WriterThreadId;
#endif
    static constexpr _dword cWriter1 = ((_dword)1) << (sizeof(_dword) * 8 / 2);
    static constexpr _dword cWritersMask = ((_dword)-1) << (sizeof(_dword) * 8 / 2);
    static constexpr _dword cReadersMask = ((_dword)-1) >> (sizeof(_dword) * 8 / 2);
public:
    CMaaRWLockWp2() noexcept;
    ~CMaaRWLockWp2() noexcept;
    void RLock() noexcept;
    void RUnLock() noexcept;
    void WLock() noexcept;
    bool WUnLock() noexcept; // false on error
    //bool W2RLock(int n = 1) noexcept;
    //bool R2WLock_(int n = 1) noexcept;

    CMaaRWLockWp2(const CMaaRWLockWp2&) = delete;
    CMaaRWLockWp2(CMaaRWLockWp2&&) = delete;
    CMaaRWLockWp2& operator=(const CMaaRWLockWp2&) = delete;
    CMaaRWLockWp2& operator=(CMaaRWLockWp2&&) = delete;
};
#endif

#ifdef __unix__

template<class T> class CMaaWin32Locker
{
    T &m_Lock;
#ifdef _DEBUG_MTX
    CMaaString m_Name;
#else
#define m_Name CMaaStringZ
#endif
    int m_x;
    bool m_bUnLockerMode;
public:
    CMaaWin32Locker(T &Lock, const CMaaString
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
    CMaaWin32Locker(T &Lock, const char * Name) noexcept
    :   m_Lock(Lock)
        //  , m_Name(Name)
    {
        m_x = 0;
        m_bUnLockerMode = false;
    }
    CMaaWin32Locker(T &Lock) noexcept
    :   m_Lock(Lock)
    {
        m_x = 0;
        m_bUnLockerMode = false;
    }
    ~CMaaWin32Locker() noexcept
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
    _dword Lock() noexcept
    {
        _dword x =
#ifdef DEBUG_MTX
        m_Lock.LockF("unknown", -2);
#else
        m_Lock.Lock();
#endif
        if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
#endif
    _dword Lock(const char * txt) noexcept
    {
        _dword x =
#ifdef DEBUG_MTX
        m_Lock.LockF("unknown", -2);
#else
        m_Lock.Lock();
#endif
        if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
    _dword LockF(const char * file, int line) noexcept
    {
        _dword x = m_Lock.LockF(file, line);
        if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
    //     DWORD Lock(DWORD dwTimeToWait);
    int UnLockF(const char * file, int line) noexcept
    {
        if  (m_x <= 0 && !m_bUnLockerMode)
        {
            return -1;
        }
        --m_x;
        return m_Lock.UnLockF(file, line);
    }
    int UnLock() noexcept
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
    void SetUnLockerMode(bool bSet = true) noexcept
    {
        m_bUnLockerMode = bSet;
    }
#ifndef _DEBUG_MTX
#undef m_Name
#endif
};
/*
_dword CMaaWin32Locker::Lock(_dword dwTimeToWait)
{
     DWORD x = m_Lock.Lock(dwTimeToWait);
     if   (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
     {
          ++m_x;
     }
     return x;
}
*/
#endif

#ifndef TOOLSLIB_SINGLE_THREAD

//#define SAFE_GLOCKS
#ifdef SAFE_GLOCKS
// exception safe
//#define CMaaWin32Locker_aLocker_gLock_Atomic CMaaWin32Locker<CMaaLiteMutex> aLocker(gLock_Atomic)
#define CMaaWin32Locker_aLocker_gLock_Atomic CMaaManualAtomicFastMutexLocker aLocker(gLock_Atomic)
#define aLocker_Lock aLocker.Lock()
#define aLocker_UnLock aLocker.UnLock()

//#define CMaaWin32Locker_a2Locker_gLock_Atomic2 CMaaWin32Locker<CMaaLiteMutex> a2Locker(gLock_Atomic2)
#define CMaaWin32Locker_a2Locker_gLock_Atomic2 CMaaManualAtomicFastMutexLocker a2Locker(gLock_Atomic2)
#define a2Locker_Lock a2Locker.Lock()
#define a2Locker_UnLock a2Locker.UnLock()

#define CMaaWin32Locker_Locker_gLock CMaaWin32Locker<CMaa_gLock_lib_Mutex> Locker(gLock_lib)
#define Locker_Lock Locker.Lock()
#define Locker_UnLock Locker.UnLock()
#else
// exception unsafe
#define CMaaWin32Locker_aLocker_gLock_Atomic //CMaaWin32Locker<CMaaLiteMutex> //or// CMaaManualAtomicFastMutexLocker aLocker(gLock_Atomic)
#define aLocker_Lock gLock_Atomic.Lock()
#define aLocker_UnLock gLock_Atomic.UnLock()

#define CMaaWin32Locker_a2Locker_gLock_Atomic2 //CMaaWin32Locker<CMaaLiteMutex> //or// CMaaManualAtomicFastMutexLocker a2Locker(gLock_Atomic2)
#define a2Locker_Lock gLock_Atomic2.Lock()
#define a2Locker_UnLock gLock_Atomic2.UnLock()

#define CMaaWin32Locker_Locker_gLock //CMaaWin32Locker<CMaa_gLock_lib_Mutex> Locker(gLock_lib)
#define Locker_Lock gLock_lib.LockM()
#define Locker_UnLock gLock_lib.UnLockM()
#endif

// exception safe
////#define CMaaWin32Locker_aLocker_gLock_Atomic_ CMaaWin32Locker<CMaaLiteMutex> aLocker(gLock_Atomic)
//#define CMaaWin32Locker_aLocker_gLock_Atomic_ CMaaManualAtomicFastMutexLocker aLocker(gLock_Atomic)
//#define aLocker_Lock_ aLocker.Lock()
//#define aLocker_UnLock_ aLocker.UnLock()

//#define CMaaWin32Locker_a2Locker_gLock_Atomic2_ CMaaWin32Locker<CMaaLiteMutex> a2Locker(gLock_Atomic2)
#define CMaaWin32Locker_a2Locker_gLock_Atomic2_ CMaaManualAtomicFastMutexLocker a2Locker(gLock_Atomic2)
#define a2Locker_Lock_ a2Locker.Lock()
#define a2Locker_UnLock_ a2Locker.UnLock()

//#define CMaaWin32Locker_Locker_gLock_ CMaaWin32Locker<CMaa_gLock_lib_Mutex> Locker(gLock)
//#define Locker_Lock_ Locker.Lock()
//#define Locker_UnLock_ Locker.UnLock()

#else

#define CMaaWin32Locker_aLocker_gLock_Atomic
#define aLocker_Lock 0
#define aLocker_UnLock 0

#define CMaaWin32Locker_a2Locker_gLock_Atomic2
#define a2Locker_Lock 0
#define a2Locker_UnLock 0

#define CMaaWin32Locker_Locker_gLock
#define Locker_Lock 0
#define Locker_UnLock 0

#define CMaaWin32Locker_a2Locker_gLock_Atomic2_
#define a2Locker_Lock_ 0
#define a2Locker_UnLock_ 0

#endif

#define CMaaWin32Locker_ CMaaWin32Locker<CMaa_gLock_usr_Mutex>

//=============
template < class Key, class Data = TOOLSLIB_HT_UNUSED_DATA_TYPE, bool bStandartHash = true, int AllocatorType =
#ifdef TOOLSLIB_SHARED_ALLOCATOR
    1
#else
    - 1
#endif
    , class Lock = CMaaRWLockWp
    , class Locker = CMaaRWLockerRWp<Lock>
> class CMaaUnivHashL
{
protected:
    CMaaClassPtr<Lock> m_lk;
    CMaaUnivHash<Key, Data, bStandartHash, AllocatorType> m_h;

public:

    class iterator // : public CMaaDLink
    {
        //friend class CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>;
        CMaaUnivHash<Key, Data, bStandartHash, AllocatorType>::iterator it;
        Lock* m_lk;
    public:
        iterator() noexcept
            : m_lk(nullptr)
        {
        }
        iterator(CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& ht) noexcept
        {
            m_lk = ht.m_lk;
            m_lk->WLock();
            it = ht.m_h.it();
        }
        iterator(const iterator& That) noexcept
            : m_lk(nullptr)
        {
            *this = That;
        }
        ~iterator() noexcept
        {
            Release();
        }
        void Release() noexcept
        {
            it.Release();
            if (m_lk)
            {
                m_lk->WUnLock();
                m_lk = nullptr;
            }
        }
        iterator& operator=(const iterator& that) noexcept
        {
            iterator& That = (iterator&)that;
            if (this != &That)
            {
                Release();
                if (m_lk = That.m_lk)
                {
                    m_lk->WLock();
                }
                it = That.it;
            }
            return *this;
        }
        iterator& operator=(CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& ht) noexcept
        {
            Release();
            m_lk = ht.m_lk;
            m_lk->WLock();

            it = ht.m_h.it();
            return *this;
        }
        bool IsValid() const noexcept
        {
            return it.IsValid();
        }
        operator bool() const noexcept
        {
            return IsValid();
        }
        // fast, noexcept functions w/o of validity check
        const Key& ckey() const noexcept
        {
            return it.ckey();
        }
        Key key() noexcept(noexcept(it.key())) //const
        {
            return it.key();
        }
        Data& rdata() noexcept
        {
            return it.rdata();
        }
        Data data() noexcept(noexcept(it.data())) //const
        {
            return it.data();
        }

        const Key& ckeyEx() const
        {
            return it.ckeyEx();
        }
        Key keyEx() //const
        {
            return it.keyEx();
        }
        Data& rdataEx()
        {
            return it.rdataEx();
        }
        Data dataEx() //const
        {
            return it.dataEx();
        }

        iterator operator++(int) noexcept
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        iterator operator++() noexcept
        {
            ++it;
            return *this;
        }
#ifdef TOOLSLIB_FAST_IT_REMOVE
        // bMoveP_OnCurrIndex = true for ++it
        // bMoveP_OnCurrIndex = false in it.Remove(/*&d*/)
        bool Inc(bool bMoveP_OnCurrIndex = true) noexcept
#else
        bool Inc() noexcept
#endif
        {
            return it.Inc();
        }
        //#ifdef TOOLSLIB_FAST_IT_REMOVE
        int Remove(Data* d) noexcept(noexcept(*d = *d)) //noexcept( noexcept(h->Remove(r, p, d)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
            //#else    
            //        int Remove(Data* d) noexcept(noexcept(r->K == r->K) && noexcept(*d = *d)) //noexcept( noexcept(h->Remove(r->K, d)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
            //#endif
        {
            return it.Remove(d);
        }
        //#ifdef TOOLSLIB_FAST_IT_REMOVE
        int Remove() noexcept //noexcept( noexcept(h->Remove(r, p)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
            //#else    
            //        int Remove() noexcept(noexcept(r->K == r->K)) //noexcept( noexcept(h->Remove(r->K)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
            //#endif
        {
            return it.Remove();
        }
    };

#if 0
    class AutoSqueezeLocker
    {
        //friend class CMaaUnivHash_New<Key, Data>;
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& h;
    public:
        AutoSqueezeLocker(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& ht) noexcept
            : h(ht)
        {
            Lock();
        }
        AutoSqueezeLocker(const AutoSqueezeLocker& That) = delete;
        AutoSqueezeLocker& operator=(const AutoSqueezeLocker& That) = delete;
        ~AutoSqueezeLocker() noexcept
        {
            UnLock();
        }
        void Lock() noexcept
        {
            h.LockAutoSqueeze();
        }
        void UnLock() noexcept
        {
            h.UnLockAutoSqueeze();
        }
    };
#endif

public:
    iterator it() noexcept
    {
        return iterator(*this);
    }
#if 0
    void InvalidateIterators() noexcept
    {
        iterator* p;
        while ((p = m_Iterators.GetFromFront()))
        {
            if (p->h)
            {
                UnLockAutoSqueeze();
                p->h = nullptr;
            }
        }
    }
#ifdef TOOLSLIB_FAST_IT_REMOVE
    void MoveIterators(const Record* r, Record** prev) noexcept // from Remove
#else
    void MoveIterators(const Record* r) noexcept // from Remove
#endif
    {
        iterator* n;
        for (iterator* p = m_Iterators.LookAtFront(); p; p = n)
        {
            n = m_Iterators.Next(p);
            if (p->h)
            {
                if (p->r == r)
                {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                    p->Inc(false);
#else
                    p->Inc();
#endif
                }
#ifdef TOOLSLIB_FAST_IT_REMOVE
                else if (r->pNext == p->r)
                {
                    p->p = prev;
                }
#endif
            }
        }
    }
    void ChangeIterators(const Record* pOld, Record* pNew) noexcept
    {
        iterator* n;
        for (iterator* p = m_Iterators.LookAtFront(); p; p = n)
        {
            n = m_Iterators.Next(p);
            if (p->h && p->r == pOld)
            {
                p->r = pNew;
            }
        }
    }
#endif
    virtual void Swap(CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& That) noexcept
    {
        if (this != &That)
        {
            if (m_lk < That.m_lk)
            {
                m_lk->WLock();
                That.m_lk->WLock();
                m_h.Swap(That.m_h);
                //m_lk.Swap(That.m_lk);
                That.m_lk->WUnLock();
                m_lk->WUnLock();
            }
            else
            {
                That.m_lk->WLock();
                m_lk->WLock();
                m_h.Swap(That.m_h);
                //m_lk.Swap(That.m_lk);
                m_lk->WUnLock();
                That.m_lk->WUnLock();
            }
        }
    }
    
    virtual void Swap(CMaaUnivHash<Key, Data, bStandartHash, AllocatorType> &ht) noexcept
    {
        m_lk->WLock();
        m_h.Swap(ht);
        m_lk->WUnLock();
    }

    CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& operator=(CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>&& That) noexcept
    {
        Swap(That);
        That.Cleanup();
        return *this;
    }

#if 0
    virtual CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>* MakeNewTable(size_t Size, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>* ptr, int Flags) noexcept(noexcept_new) //, int preallocate = 0, int allocator_type = -1)
    {
        return TL_NEW CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>(Size, ptr, 0, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1);
    }
#endif

private:
#if 0
    void Init(size_t Size, int Flags) noexcept(noexcept_new)
    {
        if (Size < 2)
        {
            Size = 0;
        }
        Flags &= ~eNothingMask;
        const size_t MaxSz = ((size_t)-1) / sizeof(Record*) / 2;
        m_HashSize = (Size + Size / 10 <= MaxSz ? Size + Size / 10 : MaxSz);
        m_TableSize = Size;
        m_ItemsCount = 0;
        m_Hash = Size && Size < SIZE_MAX / sizeof(void*) / 11 * 10 ? TL_NEW Record * [m_HashSize] : nullptr;
        if (m_Hash)
        {
            Memzero(m_Hash, m_HashSize * sizeof(*m_Hash));
            m_Flags = Flags;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
            gHashTablesAllocSize += (_qword)(m_HashSize * sizeof(*m_Hash));
#endif
        }
        else
        {
            m_Flags = Flags | (Size ? eErr1 : eOk0);
        }
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        m_AutoSqueezeLock = 0;
#endif
    }
#endif

public:
    CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& operator = (const CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& That) noexcept(noexcept(Add(*(const Key*)nullptr, *(const Data*)nullptr)))
    {
        if (this != &That)
        {
            iterator it((CMaaUnivHash<Key, Data, bStandartHash, AllocatorType> &)That);
            Cleanup();
            Locker lk(*m_lk);
            lk.WLock();
            for (; it; ++it)
            {
                Add(it.key(), it.data());
            }
            //lk.WUnLock();
        }
        return *this;
    }
    Lock& lk() const noexcept
    {
        return *m_lk;
    }
    CMaaUnivHashL() noexcept(noexcept_new)
    {
        //m_lk = new Lock;
    }
    CMaaUnivHashL(size_t Size) noexcept(noexcept_new)
    :   m_h(Size)
    {
        //m_lk = new Lock;
    }
    CMaaUnivHashL(size_t Size, CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>* ptr, int iPreallocate, int Flags
#ifndef TOOLSLIB_CHECK_HT_USAGE
        = TOOLSLIB_HT_DEF_FLAG
#else
        TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#endif
    ) noexcept(noexcept_new)
    :   m_h(Size, ptr, iPreallocate, Flags)
    {
        //m_lk = new Lock;
        //GenericCtor(Size, ptr, iPreallocate, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1);
    }
    bool PreAllocate(size_t CountTotal, size_t FreeCountRequired = 0, size_t CountAdd = 0) // noexcept(noexcept_new)
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.PreAllocate(CountTotal, FreeCountRequired, CountAdd);
    }
public:
#if 0
    void SetAutoSqueeze(bool b) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        m_Flags = b ? (m_Flags | eAutoSqueeze) : (m_Flags & ~eAutoSqueeze);
#endif
    }
    void LockAutoSqueeze() noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        ++m_AutoSqueezeLock;
#endif
    }
    void UnLockAutoSqueeze() noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        --m_AutoSqueezeLock;
#endif
    }
    bool IsNeedToSqueeze() const noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        if ((m_Flags & eAutoSqueeze) && m_AutoSqueezeLock <= 0 && m_ItemsCount <= m_TableSize / 4 && m_TableSize > 16)
        {
            return true;
        }
#else
        if (!m_Iterators.LookAtFront() && m_ItemsCount <= m_TableSize / 4 && m_TableSize > 16)
        {
            return true;
        }
#endif
        return false;
    }
#endif
    CMaaUnivHashL(CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>&& That) noexcept(noexcept_new)
    {
        //m_lk = new Lock;
        Swap(That);
    }

#if 0
    CMaaUnivHashL(const CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>& That) noexcept(noexcept_new)
    :   m_h(That)
    {
        //m_lk = new Lock;
        m_lk.Swap(((CMaaUnivHashL<Key, Data, bStandartHash, AllocatorType, Lock, Locker>&)That).m_lk);
        //*this = That;
    }

    void unite(const CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& OtherTable)
    {
        iterator it((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &)OtherTable);
        for (; it; ++it)
        {
            Add(it.key(), it.data());
        }
    }
    const Data value(const Key& key, const Data& defaultValue) const noexcept(noexcept(key == key) && noexcept((Data&)defaultValue = defaultValue))
    {
        //Data d = defaultValue;
        //((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>&)*this).Find(key, &d);
        //return d;
        Data* p = (*this)[key];
        if (p)
        {
            return *p;
        }
        return defaultValue;
    }
#endif
    bool isEmpty() const noexcept
    {
        m_lk->RLock_();
        const bool r = m_h.isEmpty();
        m_lk->RUnLock();
        return r;
    }
    bool isEmpty(Locker &lk) const noexcept
    {
        lk.RLock();
        const bool r = m_h.isEmpty();
        lk.RUnLock();
        return r;
    }

    bool IsOK() const noexcept
    {
        m_lk->RLock_();
        const bool r = m_h.IsOK();
        m_lk->RUnLock();
        return r;
    }
    bool IsOK(Locker& lk) const noexcept
    {
        lk.RLock();
        const bool r = m_h.IsOK();
        lk.RUnLock();
        return r;
    }
private:
#if 0
    void destr() noexcept
    {
        InvalidateIterators();

        if (m_ItemsCount > 0 && m_Hash)
        {
            //printf("deleting %d objects...", (int)m_ItemsCount);
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                Record* n;
                for (Record* r = m_Hash[i]; r; r = n)
                {
                    n = r->pNext;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                    gHashTablesAllocSize -= (_qword)(sizeof(*r));
#endif
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                    r->~Record();
                    m_pAllocator->DeleteEx(r TOOLSLIB_HT_AllocPtr2);
#else
                    delete r;
#endif
                }
            }
        }
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        if (m_Hash)
        {
            gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
        }
#endif
        delete[] m_Hash;  m_Hash = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if (m_pAllocator)
        {
            m_pAllocator->DelRefEx(TOOLSLIB_HT_AllocPtr);
            m_pAllocator = nullptr; // 24.09.2024
        }
#else
        // 2023+:
        Record::s_pAllocator->DelRefEx(&Record::s_pAllocator);
#endif
        m_Flags |= eErr1;
    }
#endif

public:
    virtual ~CMaaUnivHashL() noexcept
    {
        //delete m_lk; //m_lk = nullptr;
        //destr();
    }

    int RemoveSomeItems(int N) noexcept
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.RemoveSomeItems(N);
    }

    void Cleanup() noexcept
    {
        m_lk->WLock();
        m_h.Cleanup();
        m_lk->WUnLock();
    }

    void Cleanup(int NewTableSize) noexcept(noexcept_new)
    {
        Locker lk(*m_lk);
        lk.WLock();
        m_h.Cleanup(NewTableSize);
    }

    size_t GetMaxIndex() const noexcept
    {
        m_lk->RLock_();
        const size_t r = m_h.GetMaxIndex();
        m_lk->RUnLock();
        return r;
    }
    size_t GetMaxIndex(Locker& lk) const noexcept
    {
        lk.RLock();
        const size_t r = m_h.GetMaxIndex();
        lk.RUnLock();
        return r;
    }

protected:
#if 0
    void FastAddRec(Record* rec) noexcept
    {
        Record** Hash = m_Hash + HashFunction___(rec->K) % m_HashSize;
        rec->pNext = *Hash;
        *Hash = rec;
        m_ItemsCount++;
    }
#endif

public:
    int NewMoveSwap(size_t sz) noexcept(noexcept_new)
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.NewMoveSwap(sz);
    }

protected:
#if 0
    inline void Delete0(Record* rec) noexcept
    {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        rec->~Record();
        m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
#else
        delete rec;
#endif
    }
    inline void Delete(Record* rec) noexcept
    {
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize -= (_qword)(sizeof(*rec));
#endif
        Delete0(rec);
    }
#endif

public:
    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key& K, const Data& D/*, int fOverwrite*/) noexcept(noexcept_new && noexcept(m_h.Add(K, D/*, fOverwrite*/)))
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.Add(K, D/*, fOverwrite*/);
    }

    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int AddOver(const Key& K, const Data& D/*, int fOverwrite*/) noexcept(noexcept_new && noexcept(m_h.AddOver(K, D/*, fOverwrite*/)))
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.AddOver(K, D/*, fOverwrite*/);
    }

public:
    /*
    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    int AddOver(const Key& K, const Data& D) noexcept(noexcept(Add(K, D, 1)))
    {
        return Add(K, D, 1);
    }

    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory, 200 - Table failure in Data::Data() or other exception
    int Add(const Key& K, const Data& D) noexcept(noexcept(Add(K, D, 0)))
    {
        return Add(K, D, 0);
    }
    */

    void insert(const Key& K, const Data& D) noexcept(noexcept(Add(K, D)))
    {
        Add(K, D);
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find(const Key& K, Data* D /*= nullptr*/) const noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        Locker lk(*m_lk);
        lk.RLock();
        return m_h.Find(K, D);
    }
    int Find(const Key& K, Data* D /*= nullptr*/, Locker &lk) const noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        if constexpr (noexcept(K == K) && noexcept(*D = *D))
        {
            lk.RLock();
            const int r = m_h.Find(K, D);
            lk.RUnLock();
            return r;
        }
        else
        {
            lk.RLock();
            try
            {
                const int r = m_h.Find(K, D);
                lk.RUnLock();
                return r;
            }
            catch (...)
            {
                lk.RUnLock();
                throw;
            }
        }
    }
    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find(const Key& K) const noexcept(noexcept(K == K))
    {
        Locker lk(*m_lk);
        lk.RLock();
        return m_h.Find(K);
    }
    int Find(const Key& K, Locker &lk) const noexcept(noexcept(K == K))
    {
        if constexpr (noexcept(K == K))
        {
            lk.RLock();
            const int r = m_h.Find(K);
            lk.RUnLock();
            return r;
        }
        else
        {
            lk.RLock();
            try
            {
                const int r = m_h.Find(K);
                lk.RUnLock();
                return r;
            }
            catch (...)
            {
                lk.RUnLock();
                throw;
            }
        }
    }
    bool contains(const Key& K) const noexcept(noexcept(Find(K)))
    {
        return !Find(K);
    }
#if 0
    Data value(const Key& K) const noexcept(noexcept(K == K) && noexcept(Data(*(Data*)nullptr)))
    {
        //Data d;
        //Find(K, &d);
        //return d;
        return *((*this)[K]);
    }
#endif
    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to rerurn deleted context of Data
    int Remove(const Key& K, Data* D /*= nullptr*/)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(K == K) && noexcept(*D = *D))
        //#endif
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.Remove(K, D);
    }

    // Removes element.
    // Returns 0 if ok.
    int Remove(const Key& K)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(K == K))
        //#endif
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.Remove(K);
    }

#if 0
    int Remove(Record* r, Record** prev, Data* D /*= nullptr*/)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(*D = *D))
        //#endif
    {
        int r;
        m_lk->WLock();
        try
        {
            r = m_h.Remove(r, prev, D);
        }
        catch (...)
        {
            m_lk->WUnLock();
            throw;
        }
        m_lk->WUnLock();
        return r;
    }

    int Remove(Record* r, Record** prev)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept
        //#endif
    {
        int r;
        m_lk->WLock();
        try
        {
            r = m_h.Remove(r, prev);
        }
        catch (...)
        {
            m_lk->WUnLock();
            throw;
        }
        m_lk->WUnLock();
        return r;
    }

    //-==================================================
    int Remove(Record* r, Data* D /*= nullptr*/)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(*D = *D))
        //#endif
    {
        int r;
        m_lk->WLock();
        try
        {
            r = m_h.Remove(r, D);
        }
        catch (...)
        {
            m_lk->WUnLock();
            throw;
        }
        m_lk->WUnLock();
        return r;
    }

    int Remove(Record* r)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept
        //#endif
    {
        int r;
        m_lk->WLock();
        try
        {
            r = m_h.Remove(r);
        }
        catch (...)
        {
            m_lk->WUnLock();
            throw;
        }
        m_lk->WUnLock();
        return r;
    }
#endif

    int Squeeze() noexcept //noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        int r;
        m_lk->WLock();
        r = m_h.Squeeze();
        m_lk->WUnLock();
        return r;
    }

    Data* operator [] (const Key& K) const noexcept(noexcept(K == K))
    {
        m_lk->RLock_();
        Data* r = m_h[K];
        m_lk->RUnLock();
        return r;
    }
    Data* operator [] (const Key& K, Locker& lk) const noexcept(noexcept(K == K))
    {
        if constexpr (noexcept(K == K))
        {
            lk.RLock();
            Data* r = m_h[K];
            lk.RUnLock();
            return r;
        }
        else
        {
            lk.RLock();
            try
            {
                Data* r = m_h[K];
                lk.RUnLock();
                return r;
            }
            catch(...)
            {
                lk.RUnLock();
                throw;
            }
        }
    }
    Data operator [] (const Key& K, const Data& DefaultValue) const noexcept(noexcept(K == K) && noexcept(Data(DefaultValue)))
    {
        m_lk->RLock_();
        Data* r = m_h[K];
        m_lk->RUnLock();
        return r ? *r : DefaultValue;
    }
    Data operator [] (const Key& K, const Data& DefaultValue, Locker& lk) const noexcept(noexcept(K == K) && noexcept(Data(DefaultValue)))
    {
        if constexpr (noexcept(K == K))
        {
            lk.RLock();
            Data* r = m_h[K];
            lk.RUnLock();
            return r ? *r : DefaultValue;
        }
        else
        {
            lk.RLock();
            try
            {
                Data* r = m_h[K];
                lk.RUnLock();
                return r ? *r : DefaultValue;
            }
            catch (...)
            {
                lk.RUnLock();
                throw;
            }
        }
    }

    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumeratePtrs2(int Size, Key** Keys, Data** Datas) const noexcept
    {
        m_lk->RLock_();
        const int r = m_h.EnumeratePtrs(Size, Keys, Datas);
        m_lk->RUnLock();
        return r;
    }
    int EnumeratePtrs2(int Size, Key** Keys, Data** Datas, Locker& lk) const noexcept
    {
        lk.RLock();
        const int r = m_h.EnumeratePtrs(Size, Keys, Datas);
        lk.RUnLock();
        return r;
    }

    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumerateItems2(int Size, Key* Keys, Data* Datas) const noexcept(noexcept(*Keys = *Keys) && noexcept(*Datas = *Datas))
    {
        Locker lk(*m_lk);
        lk.RLock();
        return m_h.EnumerateItems(Size, Keys, Datas);
    }
    int EnumerateItems2(int Size, Key* Keys, Data* Datas, Locker &lk) const noexcept(noexcept(*Keys = *Keys) && noexcept(*Datas = *Datas))
    {
        if constexpr (noexcept(*Keys = *Keys) && noexcept(*Datas = *Datas))
        {
            lk.RLock();
            const int r = m_h.EnumerateItems(Size, Keys, Datas);
            lk.RUnLock();
            return r;
        }
        else
        {
            lk.RLock();
            try
            {
                const int r = m_h.EnumerateItems(Size, Keys, Datas);
                lk.RUnLock();
                return r;
            }
            catch(...)
            {
                lk.RUnLock();
                throw;
            }
        }
    }

    int GetItemCount() const noexcept
    {
        m_lk->RLock_();
        const int r = m_h.GetItemCount();
        m_lk->RUnLock();
        return r;
    }
    int GetItemCount(Locker& lk) const noexcept
    {
        lk.RLock();
        const int r = m_h.GetItemCount();
        lk.RUnLock();
        return r;
    }

    typedef void (*EnumerateProc)(const Key& K, Data& D, void* Param);
    typedef void (*EnumerateProcEx)(const Key& K, Data& D, CMaaUnivHash<Key, Data, bStandartHash, AllocatorType>& ht, void* Param);
    typedef void (*EnumerateProcNE)(const Key& K, Data& D, void* Param) noexcept;
    typedef void (*EnumerateProcExNE)(const Key& K, Data& D, CMaaUnivHash<Key, Data, bStandartHash, AllocatorType>& ht, void* Param) noexcept;

    // true to Remove() item
    typedef bool (*DelEnumerateProc)(const Key& K, Data& D, void* Param);
    typedef bool (*DelEnumerateProcEx)(const Key& K, Data& D, CMaaUnivHash<Key, Data, bStandartHash, AllocatorType>& ht, void* Param);
    typedef bool (*DelEnumerateProcNE)(const Key& K, Data& D, void* Param) noexcept;
    typedef bool (*DelEnumerateProcExNE)(const Key& K, Data& D, CMaaUnivHash<Key, Data, bStandartHash, AllocatorType>& ht, void* Param) noexcept;

    void EnumerateByProc(EnumerateProc Proc, void* Param)
    {
        Locker lk(*m_lk);
        lk.WLock();
        m_h.EnumerateByProc(Proc, Param);
    }

    void EnumerateByProc(EnumerateProcEx Proc, void* Param)
    {
        Locker lk(*m_lk);
        lk.WLock();
        m_h.EnumerateByProc(Proc, Param);
    }

    void EnumerateByProc(EnumerateProcNE Proc, void* Param) noexcept
    {
        m_lk->WLock();
        m_h.EnumerateByProc(Proc, Param);
        m_lk->WUnLock();
    }

    void EnumerateByProc(EnumerateProcExNE Proc, void* Param) noexcept
    {
        m_lk->WLock();
        m_h.EnumerateByProc(Proc, Param);
        m_lk->WUnLock();
    }

    int DelByEnumerateByProc(DelEnumerateProc Proc, void* Param)
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.DelByEnumerateByProc(Proc, Param);
    }

    int DelByEnumerateByProc(DelEnumerateProcEx Proc, void* Param)
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.DelByEnumerateByProc(Proc, Param);
    }

    int DelByEnumerateByProc(DelEnumerateProcNE Proc, void* Param) noexcept
    {
        m_lk->WLock();
        const int r = m_h.DelByEnumerateByProc(Proc, Param);
        m_lk->WUnLock();
        return r;
    }

    int DelByEnumerateByProc(EnumerateProcExNE Proc, void* Param) noexcept
    {
        m_lk->WLock();
        const int r = m_h.DelByEnumerateByProc(Proc, Param);
        m_lk->WUnLock();
        return r;
    }

    template<typename Fn> void EnumerateByProc(Fn Proc)
    {
        Locker lk(*m_lk);
        lk.WLock();
        m_h.EnumerateByProc(Proc);
    }

    template<typename Fn> int DelByEnumerateByProc(Fn Proc)
    {
        Locker lk(*m_lk);
        lk.WLock();
        return m_h.DelByEnumerateByProc(Proc);
    }

    static _qword GetHashTablesAllocSize() noexcept
    {
        return CMaaUnivHash<Key, Data, bStandartHash, AllocatorType>::GetHashTablesAllocSize();
    }
};
//=============
