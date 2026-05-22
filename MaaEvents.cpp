
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/MaaEvents.cpp

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

#include "perm.h"
#include "temp.h"

#if 0 // .h

class CMaaEvent2;
typedef CMaaEvent2 * TOOLSLIB_WIN32_HANDLE;

#define INFINITE 0xffffffff
#define WAIT_OBJECT_0 0
#define WAIT_ABANDONED_0 0x80 // unused
#define WAIT_TIMEOUT 0x102
#define WAIT_FAILED 0xffffffff
#define MAXIMUM_WAIT_OBJECTS_EXCEEDED 0xc
#define MAXIMUM_WAIT_OBJECTS 64
#define HANDLE TOOLSLIB_WIN32_HANDLE

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

#endif // .h

#ifdef _WIN32

_dword WaitForSingleObject_(HANDLE hHandle, _qword TimeOut) // us
{
    if  (!TimeOut)
    {
        return WaitForSingleObject(hHandle, 0);
    }
    if  (TimeOut < 0 || TimeOut + 999 < 0)
    {
        return WaitForSingleObject(hHandle, INFINITE);
    }
    _dword r;
    _dword maxto = 0xfffffffe;
    while(1)
    {
        _dword to = (TimeOut + 999) / 1000 < maxto ? (_dword)((TimeOut + 999) / 1000) : maxto;
        r = WaitForSingleObject(hHandle, to);
        TimeOut -= (_qword)to * 1000;
        if  (r != WAIT_TIMEOUT || TimeOut <= 0)
        {
            break;
        }
    }
    return r;
}

_dword WaitForMultipleObjects_(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut) // us
{
    if  (!TimeOut)
    {
        return WaitForMultipleObjects(nCount, lpHandles, bWaitAll, 0);
    }
    if  (TimeOut < 0 || TimeOut + 999 < 0)
    {
        return WaitForMultipleObjects(nCount, lpHandles, bWaitAll, INFINITE);
    }
    _dword r;
    _dword maxto = 0xfffffffe;
    while(1)
    {
        _dword to = (TimeOut + 999) / 1000 < maxto ? (_dword)((TimeOut + 999) / 1000) : maxto;
        r = WaitForMultipleObjects(nCount, lpHandles, bWaitAll, to);
        TimeOut -= (_qword)to * 1000;
        if  (r != WAIT_TIMEOUT || TimeOut <= 0)
        {
            break;
        }
    }
    return r;
}

#else // _WIN32

#ifndef TOOLSLIB_SINGLE_THREAD

#define TOOLSLIB_WAIT_EVENT_DESTROYED 0xfffffffe
#define TOOLSLIB_WAIT_EVENT_INIT 0xfffffffd

//#define printf __utf8_printf
//#define __utf8_printf printf


#define DBG_PRINTF(x) //x

class CMaaEventsTask;
class CMaaEvent2 : public CMaaReference
{
public:
    bool m_ManualReset, m_State;
    //int m_Count;
    CMaaDList< CMaaDLink_pItem<CMaaEventsTask> > m_Waits;
public:
    CMaaEvent2(bool bManualReset, bool bInitialState);
    ~CMaaEvent2();
    bool Check0();
    bool Check_();
    bool SetEvent();
    bool ResetEvent();

    ADD_ALLOCATOR(CMaaEvent2) // internally synchronized allocator
};

//typedef CMaaEvent2 * HANDLE;

class CMaaEventsTask : public CMaaDLink
{
public:
    CMaaMutex m_mtx;
    _dword m_Result;
    int m_Id;
protected:
public:
    _dword m_nCount;
    bool m_bWaitAll;
    _qword m_TimeOut; // us
    //_dword m_dwMilliseconds;
    int m_Status;

    HANDLE m_h[MAXIMUM_WAIT_OBJECTS];
    CMaaDLink_pItem<CMaaEventsTask> m_hLinks[MAXIMUM_WAIT_OBJECTS];

public:
    CMaaEventsTask(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut);
    ~CMaaEventsTask();
    bool IsWaiting();
    bool IsWaiting(CMaaEvent2 *);

    ADD_UL_ALLOCATOR(CMaaEventsTask) // allocator with extern synchronization
};

class CMaaEventsWait
{
public:
    int m_b, m_N;
    CMaaDList<CMaaEventsTask> m_Tasks[2];
    CMaaMutex m_l[2];
    CMaaUnivHash<int, CMaaEventsTask *> m_hN2T;
public:
    CMaaEventsWait();
    /*virtual*/~CMaaEventsWait();
    CMaaEventsTask * CreateWaitTask(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut, CMaaMutex ** l);
    bool IsPreparedWait(CMaaEventsTask *w);
    _dword GetResultsAndRelease(CMaaEventsTask *w);
    bool WaitInitDone();
};

//---------------------------------------------------------------------------
class CMaaWaitSk : public CMaaUdpSocket // CMaaUdpServer
{
public:
    CMaaSockTimerT<CMaaWaitSk> m_Timer0, m_Timer1/*, m_Timer2*/, m_Timer3;

    CMaaUnivHash<int, CMaaSockTimerT<CMaaWaitSk> *> m_ToTimers;
    CMaaUnivHash<int, CMaaSockTimerT<CMaaWaitSk> *> m_SeTimers;

    CMaaWaitSk(CMaaFdSockets * pFdSockets);
    ~CMaaWaitSk();
    void OnTimer(int f);
    int Notify_Read()
    {
        //printf("enr\n");
        return eDisableRead;
    }
    int Notify_Write()
    {
        //printf("enw\n");
        return eDisableWrite;
    }
    /*int Notify_Error()
     {
        printf("ne\n");
        //return eDisableExept;
     }*/
};
//---------------------------------------------------------------------------
CMaaEventsWait *gpMaaEventsWait = nullptr;
CMaaWaitSk * gpMaaWaitSk = nullptr;

DEF_ALLOCATOR(CMaaEvent2)
DEF_UL_ALLOCATOR(CMaaEventsTask)

//----------------------------------------------------------------------------
CMaaEvent2::CMaaEvent2(bool bManualReset, bool bInitialState)
:   m_ManualReset(bManualReset),
    m_State(bInitialState)//,
    //m_Count(1)
{
    if  (!m_ManualReset)
    {
        m_State = false;
    }
}

CMaaEvent2::~CMaaEvent2()
{
    gLock.LockM();
    CMaaDLink_pItem<CMaaEventsTask> *tt;
    while((tt = m_Waits.GetFromFront()))
    {
        CMaaEventsTask * t = tt->m_pItem;
        if  (t->m_Result == TOOLSLIB_WAIT_EVENT_INIT)
        {
            t->m_Result = TOOLSLIB_WAIT_EVENT_DESTROYED;
            int Id = t->m_Id;
            CMaaSockTimerT<CMaaWaitSk> * pt2 = nullptr;
            gpMaaWaitSk && gpMaaWaitSk->m_SeTimers.Find(Id + 1000 + 500000000, &pt2);
            if  (pt2)
            {
                pt2->Start(1);
            }
        }
    }
    gLock.UnLockM();
}

bool CMaaEvent2::Check0()
{
    bool r = m_State;
    if  (r && !m_ManualReset)
    {
        m_State = false;
    }
    return r;
}

bool CMaaEvent2::Check_()
{
    bool r = m_State;
    return r;
}

bool CMaaEvent2::SetEvent()
{
    gLock.LockM();
    if  (!m_State)
    {
        m_State = true;
        CMaaDLink_pItem<CMaaEventsTask> *tt;
        for (tt = m_Waits.LookAtFront(); tt; tt = m_Waits.Next(tt))
        {
            CMaaEventsTask * t = tt->m_pItem;
            DBG_PRINTF(__utf8_printf("chk wait %p...\n", t);)
            if  (!t->IsWaiting(this))
            {
                int Id = t->m_Id;
                CMaaSockTimerT<CMaaWaitSk> * pt2 = nullptr;
                gpMaaWaitSk && gpMaaWaitSk->m_SeTimers.Find(Id + 1000 + 500000000, &pt2);
                if  (pt2)
                {
                    pt2->Start(1);
                    if  (!m_ManualReset)
                    {
                        break;
                    }
                }
            }
        }
        if  (!m_ManualReset)
        {
            m_State = false;
        }
    }
    gLock.UnLockM();
    return true;
}

bool CMaaEvent2::ResetEvent()
{
    gLock.LockM();
    if  (m_State)
    {
        m_State = false;
    }
    gLock.UnLockM();
    return true;
}
//----------------------------------------------------------------------------
CMaaEventsTask::CMaaEventsTask(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut)
:   m_Result(TOOLSLIB_WAIT_EVENT_INIT), //(WAIT_FAILED),
    m_Id(-1),
    m_nCount(nCount),
    m_bWaitAll(bWaitAll),
    m_TimeOut(TimeOut),
    m_Status(0)
{
    DBG_PRINTF(__utf8_printf("CMaaEventsTask() %p\n", this);)
    memcpy(m_h, lpHandles, nCount * sizeof(HANDLE));
    for (int i = 0; i < (int)nCount; i++)
    {
        m_hLinks[i].m_pItem = this;
        m_h[i]->m_Waits.AddAtBack(&m_hLinks[i]);
    }
}

CMaaEventsTask::~CMaaEventsTask()
{
    DBG_PRINTF(__utf8_printf("~CMaaEventsTask() %p\n", this);)
    for (int i = (int)m_nCount - 1; i >= 0; i--)
    {
        CMaaDList< CMaaDLink_pItem<CMaaEventsTask> >::ReleaseSafe(&m_hLinks[i]);
    }
    DBG_PRINTF(__utf8_printf("~CMaaEventsTask() %p 2\n", this);)
}

bool CMaaEventsTask::IsWaiting()
{
    //if (m_Result == TOOLSLIB_WAIT_EVENT_DESTROYED)
    //{
    //    return false;
    //}
    if  (m_Result != TOOLSLIB_WAIT_EVENT_INIT) // reentrance
    {
        return false;
    }
    int i;
    if  (m_bWaitAll)
    {
        for (i = 0; i < (int)m_nCount; i++)
        {
            if  (!m_h[i]->Check_())
            {
                return true;
            }
        }
        m_Result = WAIT_OBJECT_0;
        for (i = 0; i < (int)m_nCount; i++)
        {
            m_h[i]->Check0();
        }
        return false;
    }
    //else
    //{
    for (i = 0; i < (int)m_nCount; i++)
    {
        if  (m_h[i]->Check0())
        {
            m_Result = WAIT_OBJECT_0 + i;
            return false;
        }
    }
    return true;
    //}
}

bool CMaaEventsTask::IsWaiting(CMaaEvent2 * /*e*/)
{
    return IsWaiting();
#if 0
    //if (m_Result == TOOLSLIB_WAIT_EVENT_DESTROYED)
    //{
    //    return false;
    //}
    if  (m_Result != TOOLSLIB_WAIT_EVENT_INIT) // reentrance
    {
        return false;
    }
    if  (m_bWaitAll)
    {
        if  (!e->Check_())
        {
            return true;
        }
    }
    int i;
    if  (m_bWaitAll)
    {
        for (i = 0; i < (int)m_nCount; i++)
        {
            if  (!m_h[i]->Check_())
            {
                return true;
            }
        }
        m_Result = WAIT_OBJECT_0;
        for (i = 0; i < (int)m_nCount; i++)
        {
            if  (!m_h[i]->Check0())
            {
                //return true; // alg: never
            }
        }
        return false;
    }
    //else
    //{
    for (i = 0; i < (int)m_nCount; i++)
    {
        if  (m_h[i]->Check0())
        {
            m_Result = WAIT_OBJECT_0 + i;
            return false;
        }
    }
    return true;
    //}
#endif
}
//----------------------------------------------------------------------------
CMaaEventsWait::CMaaEventsWait()
:   m_b(-1),
    m_N(0)
{
}

CMaaEventsWait::~CMaaEventsWait()
{
    gLock.LockM();
    while(1)
    {
        if  (gpMaaEventsWait == this)
        {
            //gpMaaEventsWait = nullptr;
            if  (!m_Tasks[0].LookAtFront() && !m_Tasks[1].LookAtFront())
            {
                break;
            }
            if  (gpMaaWaitSk && !gpMaaWaitSk->m_Timer0.IsStarted())
            {
                gpMaaWaitSk->m_Timer0.Start(1);
            }
            usleep(1000);
        }
        else
        {
            break;
        }
    }
    if  (gpMaaEventsWait == this)
    {
        gpMaaEventsWait = nullptr;
    }
    gLock.UnLockM();
}

CMaaEventsTask * CMaaEventsWait::CreateWaitTask(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut, CMaaMutex ** l)
{
    while(!m_hN2T.Find(m_N))
    {
        m_N = (m_N + 1) % 1000000000;
    }
    CMaaEventsTask * t = TL_NEW CMaaEventsTask(nCount, lpHandles, bWaitAll, TimeOut);
    if  (t)
    {
        m_Tasks[0].AddAtBack(t);
        while(m_hN2T.Add(t->m_Id = m_N, t))
        {
            m_N = (m_N + 1) % 500000000;
        }
        m_N = (m_N + 1) % 500000000;
    }
    *l = &m_l[m_b];
    DBG_PRINTF(__utf8_printf("*l = [%d]\n", m_b);)
    if  (gpMaaWaitSk)
    {
        gpMaaWaitSk->m_Timer1.Start(1);
    }
    return t;
}

bool CMaaEventsWait::IsPreparedWait(CMaaEventsTask *w)
{
    return !gpMaaWaitSk || w->m_Status > 0;
}

_dword CMaaEventsWait::GetResultsAndRelease(CMaaEventsTask *w)
{
    _dword r = w->m_Result;
    CMaaDList<CMaaEventsTask>::Release(w);
    delete w;
    if  (r == TOOLSLIB_WAIT_EVENT_DESTROYED || r == TOOLSLIB_WAIT_EVENT_INIT) // Wait on closed event/unknown status
    {
        r = WAIT_FAILED;
    }
    return r;
}

bool CMaaEventsWait::WaitInitDone()
{
    bool r = false;
    while(1)
    {
        bool b;
        gLock.LockM();
        if  (!gpMaaEventsWait)
        {
            b = true;
        }
        else
        {
            b = r = /*gpMaaEventsWait->*/m_b >= 0;
        }
        gLock.UnLockM();
        if  (b)
        {
            break;
        }
        usleep(1);
    }
    return true;
}
//----------------------------------------------------------------------------
CMaaWaitSk::CMaaWaitSk(CMaaFdSockets * pFdSockets)
:   CMaaUdpSocket(pFdSockets),
    //:   CMaaUdpServer(pFdSockets, 11000, 0x7f000001, true),
    m_Timer0(this, 0),
    m_Timer1(this, 1),
    m_Timer3(this, 3)
{
    AddFdSocket();
    m_Timer0.Attach(pFdSockets);
    m_Timer1.Attach(pFdSockets);
    m_Timer3.Attach(pFdSockets);
    m_Timer1.Start(1);
    //m_Timer3.Start(1800000); // test
    //m_Timer3.Start(2800000); // test
}

CMaaWaitSk::~CMaaWaitSk()
{
    DBG_PRINTF(__utf8_printf("~CMaaWaitSk()\n");)
    gLock.LockM();
    CMaaUnivHash<int, CMaaSockTimerT<CMaaWaitSk> *>::iterator it1(m_ToTimers);
    for (; it1; ++it1)
    {
        DBG_PRINTF(__utf8_printf("~CMaaWaitSk():10 %d\n", it1.key());)
        delete it1.data();
        it1.Remove();
    }
    CMaaUnivHash<int, CMaaSockTimerT<CMaaWaitSk> *>::iterator it(m_SeTimers);
    for (; it; ++it)
    {
        DBG_PRINTF(__utf8_printf("~CMaaWaitSk():11 %d\n", it.key());)
        int Id = it.key() - 1000 - 500000000;
        delete it.data();
        it.Remove();
        CMaaEventsTask * t = nullptr;
        gpMaaEventsWait && gpMaaEventsWait->m_hN2T.Find(Id, &t);
        if  (t)
        {
            DBG_PRINTF(__utf8_printf("~CMaaWaitSk():12 %d=-1\n", (int)t->m_Result);)
            t->m_Result = WAIT_FAILED;
            t->m_mtx.UnLockM();
        }
    }
    DBG_PRINTF(__utf8_printf("~CMaaWaitSk():13\n");)
    if  (gpMaaEventsWait && gpMaaEventsWait->m_b >= 0)
    {
        DBG_PRINTF(__utf8_printf("~CMaaWaitSk():14\n");)
        gpMaaEventsWait->m_l[gpMaaEventsWait->m_b].UnLockM();
    }
    DBG_PRINTF(__utf8_printf("~CMaaWaitSk():15\n");)
    if  (gpMaaWaitSk == this)
    {
        gpMaaWaitSk = nullptr;
    }
    DBG_PRINTF(__utf8_printf("~CMaaWaitSk():16\n");)
    gLock.UnLockM();
    DBG_PRINTF(__utf8_printf("~CMaaWaitSk():17\n");)
}

void CMaaWaitSk::OnTimer(int f)
{
    DBG_PRINTF(__utf8_printf("OnTimer(%d)>>\n", f);)
    if  (f == 0)
    {
        DBG_PRINTF(__utf8_printf("OnTimer(%d) - close\n", f);)
        CloseByException("close");
    }
    else if (f == 1)
    {
        DBG_PRINTF(__utf8_printf("OnTimer(%d) - add tasks\n", f);)
        // add tasks
        gLock.LockM();
        m_Timer1.Stop();
        if  (!gpMaaEventsWait)
        {
            gLock.UnLockM();
            CloseByException("err");
        }
        int b = gpMaaEventsWait->m_b;
        int b1 = b >= 0 ? b : 0;
        int b2 = b1 ^ 1;
        while(1)
        {
            CMaaEventsTask * t = gpMaaEventsWait->m_Tasks[0].GetFromFront();
            if  (!t)
            {
                DBG_PRINTF(__utf8_printf("brk\n", f);)
                break;
            }
            DBG_PRINTF(__utf8_printf("task %p\n", t);)

            gpMaaEventsWait->m_Tasks[1].AddAtBack(t);
            CMaaSockTimerT<CMaaWaitSk> * pt = t->m_TimeOut > 0 ? TL_NEW CMaaSockTimerT<CMaaWaitSk>(this, t->m_Id + 1000) : nullptr;
            CMaaSockTimerT<CMaaWaitSk> * pt2 = TL_NEW CMaaSockTimerT<CMaaWaitSk>(this, t->m_Id + 1000 + 500000000);
            if  (pt)
            {
                m_ToTimers.Add(t->m_Id + 1000, pt);
                pt->Attach(m_pFdSockets);
                pt->Start(t->m_TimeOut);
            }
            m_SeTimers.Add(t->m_Id + 1000 + 500000000, pt2);
            pt2->Attach(m_pFdSockets);

            DBG_PRINTF(__utf8_printf("task %p 2\n", t);)
            t->m_Status = 1;
            DBG_PRINTF(__utf8_printf("task %p 3\n", t);)
            t->m_mtx.LockM();
            DBG_PRINTF(__utf8_printf("task %p 4\n", t);)
            if  (!t->IsWaiting())
            {
                DBG_PRINTF(__utf8_printf("task %p 5\n", t);)
                m_SeTimers.Remove(t->m_Id + 1000 + 500000000);
                m_ToTimers.Remove(t->m_Id + 1000);
                delete pt2;
                delete pt;
                t->m_mtx.UnLockM();
            }
            DBG_PRINTF(__utf8_printf("task %p 6\n", t);)
        }
        if  (b >= 0)
        {
            DBG_PRINTF(__utf8_printf("u.%d\n", b);)
            gpMaaEventsWait->m_l[b].UnLockM();
        }
        DBG_PRINTF(__utf8_printf("l.%d\n", b2);)
        gpMaaEventsWait->m_l[b2].LockM();
        gpMaaEventsWait->m_b = b2;
        gLock.UnLockM();
    }
    else if (f == 3)
    {
        // tmp
        m_Timer3.Stop();
        CloseByException("close3");
    }
    else if (f >= 1000)
    {
        int Id = f - 1000;
        CMaaSockTimerT<CMaaWaitSk> * pt = nullptr;
        CMaaSockTimerT<CMaaWaitSk> * pt2 = nullptr;
        gLock.LockM();
        if  (Id < 500000000)
        {
            DBG_PRINTF(__utf8_printf("OnTimer(%d) - to\n", f);)
            m_ToTimers.Remove(f, &pt);
            if  (pt)
            {
                DBG_PRINTF(__utf8_printf("OnTimer(%d) - del pt\n", f);)
                delete pt;
                m_SeTimers.Remove(f + 500000000, &pt2);
                DBG_PRINTF(__utf8_printf("OnTimer(%d) - del pt2=%p\n", f, pt2);)
                delete pt2;
                CMaaEventsTask * t = nullptr;
                gpMaaEventsWait && gpMaaEventsWait->m_hN2T.Find(Id, &t);
                if  (t)
                {
                    t->m_Result = WAIT_TIMEOUT;
                    t->m_mtx.UnLockM();
                }
            }
        }
        else
        {
            Id -= 500000000;
            m_SeTimers.Remove(f, &pt2);
            if  (pt2)
            {
                DBG_PRINTF(__utf8_printf("OnTimer(%d) - del pt2\n", f);)
                //pt2->Stop();
                delete pt2;
                DBG_PRINTF(__utf8_printf("OnTimer(%d) - OnSetEvent\n", f);)
                m_ToTimers.Remove(Id + 1000, &pt);
                if  (pt)
                {
                    DBG_PRINTF(__utf8_printf("OnTimer(%d) - del pt\n", f);)
                    delete pt;
                }
                CMaaEventsTask * t = nullptr;
                gpMaaEventsWait && gpMaaEventsWait->m_hN2T.Find(Id, &t);
                if  (t)
                {
                    t->m_mtx.UnLockM();
                }
            }
        }
        gLock.UnLockM();
    }
    DBG_PRINTF(__utf8_printf("OnTimer(%d)<<\n", f);)
}
//----------------------------------------------------------------------------
HANDLE CreateEvent(const void *, bool bManualReset, bool bInitialState, const void *)
{
    CMaaEvent2 * e = TL_NEW CMaaEvent2(bManualReset, bInitialState);
    return e;
}
bool CloseHandle(HANDLE h)
{
    if  (!h)
    {
        return false;
    }
    h->Release();
    return true;
}

bool SetEvent(HANDLE hEvent)
{
    gLock.LockM();
    bool r = hEvent ? hEvent->SetEvent() : false;
    gLock.UnLockM();
    return r;
}
bool ResetEvent(HANDLE hEvent)
{
    gLock.LockM();
    bool r = hEvent ? hEvent->ResetEvent() : false;
    gLock.UnLockM();
    return r;
}

_dword WaitForSingleObject(HANDLE hHandle, _dword dwMilliseconds)
{
    return WaitForSingleObject_(hHandle, dwMilliseconds != 0xffffffff ? (_qword)((_uqword)1000 * dwMilliseconds) : -1);
}
_dword WaitForSingleObject_(HANDLE hHandle, _qword TimeOut)
{
    if  (!hHandle)
    {
        // error
        return WAIT_FAILED;
    }
    if  (!TimeOut)
    {
        gLock.LockM();
        bool r = hHandle->Check0();
        gLock.UnLockM();
        return r ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }
    // timed wait
    return WaitForMultipleObjects_(1, &hHandle, false, TimeOut);
}

_dword WaitForMultipleObjects(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _dword dwMilliseconds)
{
    return WaitForMultipleObjects_(nCount, lpHandles, bWaitAll, dwMilliseconds != 0xffffffff ? (_qword)((_uqword)1000 * dwMilliseconds) : -1);
}
_dword WaitForMultipleObjects_(_dword nCount, const HANDLE *lpHandles, bool bWaitAll, _qword TimeOut)
{
    if  ((int)nCount > MAXIMUM_WAIT_OBJECTS)
    {
        return MAXIMUM_WAIT_OBJECTS_EXCEEDED;
    }
    if  ((int)nCount <= 0 || !lpHandles)
    {
        // error
        return WAIT_FAILED;
    }
    int i;
    gLock.LockM();
    if  (!TimeOut)
    {
        if  (bWaitAll)
        {
            for (i = 0; i < (int)nCount; i++)
            {
                if  (!lpHandles[i]->Check_())
                {
                    gLock.UnLockM();
                    return WAIT_TIMEOUT;
                }
            }
            for (i = 0; i < (int)nCount; i++)
            {
                if  (!lpHandles[i]->Check0())
                {
                    //gLock.UnLockM();
                    //return WAIT_TIMEOUT;
                }
            }
            gLock.UnLockM();
            return WAIT_OBJECT_0;
        }
        else
        {
            for (i = 0; i < (int)nCount; i++)
            {
                if  (lpHandles[i]->Check0())
                {
                    gLock.UnLockM();
                    return WAIT_OBJECT_0 + i;
                }
            }
            gLock.UnLockM();
            return WAIT_TIMEOUT;
        }
    }
    CMaaMutex * l = nullptr;
    DBG_PRINTF(__utf8_printf("1, to2wait = %,D\n", TimeOut);)
    CMaaEventsTask * w = gpMaaEventsWait ? gpMaaEventsWait->CreateWaitTask(nCount, lpHandles, bWaitAll, TimeOut, &l) : nullptr;
    DBG_PRINTF(__utf8_printf("2 %p\n", w);)
    gLock.UnLockM();
    DBG_PRINTF(__utf8_printf("3\n");)
    if  (!w)
    {
        return WAIT_FAILED;
    }
    bool b = false;
    while(!b)
    {
        DBG_PRINTF(__utf8_printf("4\n");)
        l->LockM();
        DBG_PRINTF(__utf8_printf("5\n");)
        l->UnLockM();
        DBG_PRINTF(__utf8_printf("6\n");)
        gLock.LockM();
        DBG_PRINTF(__utf8_printf("7 %p\n", w);)
        if  (!gpMaaEventsWait)
        {
            gLock.UnLockM();
            return WAIT_FAILED;
        }
        b = gpMaaEventsWait->IsPreparedWait(w);
        DBG_PRINTF(__utf8_printf("8\n");)
        gLock.UnLockM();
        DBG_PRINTF(__utf8_printf("9\n");)
    }
    DBG_PRINTF(__utf8_printf("10\n");)
    w->m_mtx.LockM();
    DBG_PRINTF(__utf8_printf("11\n");)
    w->m_mtx.UnLockM();
    DBG_PRINTF(__utf8_printf("12\n");)
    gLock.LockM();
    DBG_PRINTF(__utf8_printf("13\n");)
    _dword r = gpMaaEventsWait ? gpMaaEventsWait->GetResultsAndRelease(w) : WAIT_FAILED;
    DBG_PRINTF(__utf8_printf("14\n");)
    gLock.UnLockM();
    DBG_PRINTF(__utf8_printf("15\n");)
    return r;
}
//----------------------------------------------------------------------------
CMaaEvents2Initializer::CMaaEvents2Initializer()
{
    if  (!gpMaaEventsWait)
    {
        gpMaaEventsWait = TL_NEW CMaaEventsWait();
        HANDLE h0 = CreateEvent(nullptr, false, false, nullptr);
        CloseHandle(h0);
    }
}
CMaaEvents2Initializer::~CMaaEvents2Initializer()
{
    StartClose();
    delete gpMaaEventsWait;
}
bool CMaaEvents2Initializer::CreateInTheNewThread(CMaaSockThread * pThread)
{
    CMaaSockThread * pEventsThr = pThread->CreateNewThread();
    return pEventsThr && Attach(pEventsThr->m_pFdSockets) && pEventsThr->Create() && WaitReady();
}
bool CMaaEvents2Initializer::Attach(CMaaFdSockets * pFdSockets)
{
    if  (!gpMaaWaitSk)
    {
        gpMaaWaitSk = TL_NEW CMaaWaitSk(pFdSockets);
        return gpMaaWaitSk != nullptr;
    }
    return false;
}
bool CMaaEvents2Initializer::WaitReady()
{
    //gLock.LockM(); // opt
    return gpMaaEventsWait->WaitInitDone();
    //gLock.UnLockM();
}
void CMaaEvents2Initializer::StartClose()
{
    gLock.LockM();
    if  (gpMaaWaitSk)
    {
        gpMaaWaitSk->m_Timer0.Start(1);
    }
    gLock.UnLockM();
}
//----------------------------------------------------------------------------

#endif // #ifndef TOOLSLIB_SINGLE_THREAD
#endif // _WIN32

#if 0 // sample

//---------------------------------------------------------------------------
class CMaaUserSk : public CMaaUdpSocket // CMaaUdpServer
{
    HANDLE m_h, m_e;
public:
    CMaaSockTimerT<CMaaUserSk> m_Timer0, m_Timer1/*, m_Timer2*/, m_Timer3;

    CMaaUserSk(CMaaFdSockets * pFdSockets, HANDLE h, HANDLE e);
    ~CMaaUserSk();
    void OnTimer(int f);
};
//---------------------------------------------------------------------------
CMaaUserSk::CMaaUserSk(CMaaFdSockets * pFdSockets, HANDLE h, HANDLE e)
:   CMaaUdpSocket(pFdSockets),
    //:   CMaaUdpServer(pFdSockets, 11000, 0x7f000001, true),
    m_h(h),
    m_e(e),
    m_Timer0(this, 0),
    m_Timer1(this, 1),
    m_Timer3(this, 3)
{
    AddFdSocket();
    m_Timer0.Attach(pFdSockets);
    m_Timer1.Attach(pFdSockets);
    m_Timer3.Attach(pFdSockets);
    m_Timer1.Start(100000);
    //m_Timer3.Start(1800000); // test
    //m_Timer3.Start(2800000); // test
}

CMaaUserSk::~CMaaUserSk()
{
    __utf8_printf("~CMaaUserSk()>>\n");
    SetEvent(m_e);
    __utf8_printf("~CMaaUserSk()<<\n");
}

void CMaaUserSk::OnTimer(int f)
{
    __utf8_printf("CMaaUserSk::OnTimer(%d)>>\n", f);
    if  (f == 0)
    {
        CloseByException("close");
    }
    else if (f == 1)
    {
        m_Timer1.Stop();
        __utf8_printf("CMaaUserSk::waiting s.o. 1s...\n");
        _dword dw = WaitForSingleObject(m_h, 10000);
        //_dword dw = WaitForMultipleObjects(1, &m_h, true, 10000);
        __utf8_printf("CMaaUserSk::dw = 0x%x\n", dw);
        m_Timer3.Start(1000000);
    }
    else if (f == 3)
    {
        // tmp
        m_Timer3.Stop();
        CloseByException("close3");
    }
    __utf8_printf("CMaaUserSk::OnTimer(%d)<<\n", f);
}
//----------------------------------------------------------------------------


class T1: public CMaaThread
{
public:
    HANDLE m_h;
    _dword m_to;
    T1(HANDLE h, _dword to)
    {
        m_h = h;
        m_to = to;
    }
    unsigned Run()
    {
        usleep(1000000 * m_to);
        __utf8_printf("Set %p...\n", m_h);
        SetEvent(m_h);
        //CloseHandle(m_h);
        usleep(1000000);
        return 0;
    }

    ADD_ALLOCATOR(T1)
};

DEF_ALLOCATOR(T1)


int main_sample1(int argn, char * args[])
{
    try
    {
        //gpMaaEventsWait = TL_NEW CMaaEventsWait();
        CMaaEvents2Initializer wii;

        CMaaSockStartup st;
        gpSockStartup = &st;
        CMaaSockThread thr(nullptr);
        CMaaSockThread *thr2 = TL_NEW CMaaSockThread(&thr);

        //gpMaaWaitSk = TL_NEW CMaaWaitSk(thr.m_pFdSockets);
        wii.Attach(thr.m_pFdSockets);

        thr.Create();          printf("Started\n");

        //gLock.LockM(); // opt
        //gpMaaEventsWait->WaitInitDone();
        wii.WaitReady();
        //gLock.UnLockM();
        printf("Initialized\n");

        HANDLE h1, h2;
        h1 = CreateEvent(nullptr, true, false, nullptr);
        h2 = CreateEvent(nullptr, true, false, nullptr);
        //gLock.LockM();
        //thr2 = thr.CreateNewThread();
        CMaaUserSk * usk = TL_NEW CMaaUserSk(thr2->m_pFdSockets, h1, h2);
        thr2->Create();       printf("thr2 Started\n");
        //gLock.UnLockM();

        HANDLE h[2];
        h[0] = CreateEvent(nullptr, false, false, nullptr);
        h[1] = CreateEvent(nullptr, true, false, nullptr);
        T1 t1(h[0], 2);
        T1 t2(h[1], 1);
        t1.Create();
        t2.Create();
        __utf8_printf("Wait...\n");

        _dword to = 3000; // INFINITE
        _dword dw = WaitForMultipleObjects(2, h, true, to);
        __utf8_printf("Wait1 rets 0x%x\n", dw);
        dw < MAXIMUM_WAIT_OBJECTS && ResetEvent(h[dw]);
        dw = WaitForMultipleObjects(2, h, true, to);
        __utf8_printf("Wait2 rets 0x%x\n", dw);
        //usleep(1500000);
        //dw = WaitForMultipleObjects(2, h, false, 3000);
        //__utf8_printf("Wait3 rets 0x%x\n", dw);

        usleep(200000);
        printf("SetEvent(h1)...\n");
        SetEvent(h1);
        printf("WaitForSingleObject(h2, 20000)...\n");
        dw = WaitForSingleObject(h2, 20000);
        __utf8_printf("Wait h2 rets 0x%x\n", dw);
#ifdef _WIN32
        thr.Wait(INFINITE);
#else
        //usleep(2000000);
        /*
        if (gpMaaWaitSk)
        {
            gpMaaWaitSk->m_Timer3.Start(1);
        }
        */
        wii.StartClose();
        thr.Wait((DWORD)-1);
#endif
        __utf8_printf("Exiting...\n");
        CloseHandle(h1);
        CloseHandle(h2);
        CloseHandle(h[0]);
        CloseHandle(h[1]);

        //delete gpMaaEventsWait;
        __utf8_printf("Exiting2...\n");
    }
    catch(XTOOSockErr err)
    {
        printf("catch(XTOOSockErr): %s\n", err.GetMsg());
    }
    catch(...)
    {
        printf("catch(...)\n");
    }
    gpSockStartup = nullptr;
    __utf8_printf("Exit(0)\n");
    return 0;
}

int main(int argn, char * args[])
{
    try
    {
        CMaaEvents2Initializer wii;

        CMaaSockStartup st;
        gpSockStartup = &st;
        CMaaSockThread thr(nullptr);
        wii.CreateInTheNewThread(&thr);
        //thr.Create();          printf("Started\n");

        HANDLE h1, h2;
        h1 = CreateEvent(nullptr, true, false, nullptr);
        h2 = CreateEvent(nullptr, true, false, nullptr);
        //gLock.LockM();
        // add some sockets to the main thr thread...
        CMaaUserSk * usk = TL_NEW CMaaUserSk(thr.m_pFdSockets, h1, h2);
        //gLock.UnLockM();
        thr.Create();          printf("Started\n");

        HANDLE h[2];
        h[0] = CreateEvent(nullptr, false, false, nullptr);
        h[1] = CreateEvent(nullptr, true, false, nullptr);
        T1 t1(h[0], 2);
        T1 t2(h[1], 1);
        t1.Create();
        t2.Create();
        __utf8_printf("Wait...\n");

        _dword to = 3000; // INFINITE
        _dword dw = WaitForMultipleObjects(2, h, true, to);
        __utf8_printf("Wait1 rets 0x%x\n", dw);
        dw < MAXIMUM_WAIT_OBJECTS && ResetEvent(h[dw]);
        dw = WaitForMultipleObjects(2, h, true, to);
        __utf8_printf("Wait2 rets 0x%x\n", dw);
        //usleep(1500000);
        //dw = WaitForMultipleObjects(2, h, false, 3000);
        //__utf8_printf("Wait3 rets 0x%x\n", dw);

        usleep(200000);
        printf("SetEvent(h1)...\n");
        SetEvent(h1);
        printf("WaitForSingleObject(h2, 20000)...\n");
        dw = WaitForSingleObject(h2, 20000);
        __utf8_printf("Wait h2 rets 0x%x\n", dw);
#ifdef _WIN32
        thr.Wait(INFINITE);
#else
        wii.StartClose(); // finish to work with events
        thr.Wait((DWORD)-1);
#endif
        __utf8_printf("Exiting...\n");
        CloseHandle(h1);
        CloseHandle(h2);
        CloseHandle(h[0]);
        CloseHandle(h[1]);

        __utf8_printf("Exiting2...\n");
    }
    catch(XTOOSockErr err)
    {
        printf("catch(XTOOSockErr): %s\n", err.GetMsg());
    }
    catch(...)
    {
        printf("catch(...)\n");
    }
    gpSockStartup = nullptr;
    __utf8_printf("Exit(0)\n");
    return 0;
}

#endif // sample
