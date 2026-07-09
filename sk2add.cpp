
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/sk2add.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file is a socket2.{h|cpp} additional classes for parsing raw HTTP
 * requests and responces, syncronious sockets read/write with timeouts
 * like a files io. Frst dated 10.10.2009.
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

//-----------------------------------------------------------------------------------------
CHttpResponceHandlerForReader::CHttpResponceHandlerForReader(eHandlerType hType) noexcept
:   m_hType(hType)
{
}
//-----------------------------------------------------------------------------------------
CHttpResponceHandlerForReader::~CHttpResponceHandlerForReader()
{
}
//-----------------------------------------------------------------------------------------
int CHttpResponceHandlerForReader::AppendData(CMaaString &Method, CMaaString Data, CMaaString &Header, CMaaString &Html, int MaxDataSize, int &Status, CMaaString &NextData)
{
    return AppendData(Method, Data, (warning_int)Data.Length(), Header, Html, MaxDataSize, Status, NextData);
}
//-----------------------------------------------------------------------------------------
int CHttpResponceHandlerForReader::AppendData(CMaaString &Method, const void * ptrData, int DataLength, CMaaString &Header, CMaaString &Html, int MaxDataSize, int &Status, CMaaString &NextData)
{
    while(1)
    {
        if  (!m_InHeader.IsRight("\r\n\r\n", 4))
        {
            const int MaxHeaderLength_appox = MaxDataSize;
            if  (m_Buffer.GetLength() != 0)
            {
                CMaaString str = m_Buffer.GetMemoryString();
                m_InHeader_ccstr += str;
                str.Empty();
                m_Buffer.Empty();
            }
            m_InHeader_ccstr.Add((const char *)ptrData, DataLength);
            DataLength = 0;
            CMaaString InHeader = m_InHeader_ccstr.GetMemoryString();
            const int n = (warning_int)InHeader.Find("\r\n\r\n");
            if  (n > 0)
            {
                m_InHeader = InHeader.Left(n + 4);
                m_Buffer.AddMid(InHeader, n + 4);
                m_Status = -1;
                if  (m_hType == eRequest)
                {
                    m_Status = 0;
                    const int n2 = (warning_int)InHeader.Find(0, ' ', n);
                    if  (n2 > 0)
                    {
                        Method = m_Method = InHeader.Left(n2);
                    }
                    else
                    {
                        throw CMaaString("Header error in request");
                    }
                }
                else
                {
                    if  (m_InHeader.IsLeftCi("http/", (StrInt)5))
                    {
                        int i;
                        //for (i = 5; (m_InHeader2[i] >= '0' && m_InHeader2[i] <= '9') || m_InHeader2[i] == '.'; i++) ;
                        //if  (dsscanf(&m_InHeader2[i], "%d", &i) == 1)
                        for (i = 5; (m_InHeader[i] >= '0' && m_InHeader[i] <= '9') || m_InHeader[i] == '.'; i++);
                        if (dsscanf(&m_InHeader[i], "%d", &i) == 1)
                        {
                            m_Status = i;
                        }
                    }
                    if  (m_Status == -1)
                    {
                        throw CMaaString("Header error");
                    }
                    if  ((size_t)m_InHeader.GetLength() > (size_t)MaxHeaderLength_appox)
                    {
                        throw CMaaString::sFormat("Too long header error (>%d)\n", MaxHeaderLength_appox);
                    }
                    if  (m_Status == 100)
                    {
                        if  (m_hType == eResponceWithContinueStatusIsAllowed)
                        {
                            return FillReturnResults(Method, Header, Html, Status, NextData);
                        }
                        // support of 100 continue status code
                        m_InHeader_ccstr.Empty();
                        m_InHeader.Empty();
                        //m_InHeader2.Empty();//++
                        m_InHeader_ccstr += (CMaaString)m_Buffer;
                        m_Buffer.Empty();
                        //DataLength = 0;
                        continue;
                    }
                    if  (Method.IsCi0("HEAD", 0))
                    {
                        return FillReturnResults(Method, Header, Html, Status, NextData);
                    }
                    if  (m_Status == 304 || m_Status == 204 || (m_Status == 101 &&
                        m_InHeader.HttpGetValue("content-length:").RemoveSpaces().IsEmpty() &&
                        m_InHeader.HttpGetValue("transfer-encoding:", nullptr).RemoveSpaces().IsEmpty()))
                    {
                        return FillReturnResults(Method, Header, Html, Status, NextData);
                    }
                }
            }
            else if (n < 0)
            {
                if  (m_InHeader_ccstr.GetLength() > (size_t)MaxHeaderLength_appox)
                {
                    throw CMaaString::sFormat("Too long header error (>%d)\n", MaxHeaderLength_appox);
                }
            }
        }
        if  (m_InHeader.IsRight("\r\n\r\n", 4))
        {
            m_Buffer.Add((const char *)ptrData, DataLength);
            DataLength = 0;
            CMaaString Encoding = m_InHeader.HttpGetValue("transfer-encoding:");
            CMaaString ContentLength = m_InHeader.HttpGetValue("content-length:");
            int n;
            if  (m_hType != eRequest && Encoding.IsCi0("chunked", 0))
            {
                while(1)
                {
                    CMaaString Buffer = m_Buffer.GetMemoryString();

                    int l = 0;
                    int i;
                    for (i = 0; ;i++)
                    {
                        const char c = Buffer[i];
                        const int h = (c >= '0' && c <= '9') ? c - '0' :
                        (c >= 'a' && c <= 'f') ? c - 'a' + 10 :
                        (c >= 'A' && c <= 'F') ? c - 'A' + 10 : -1;
                        if  (h >= 0)
                        {
                            l = (l << 4) + h;
                            if  (l < 0)
                            {
                                throw CMaaString("Chunk error 1");
                            }
                        }
                        else
                        {
                            if  (i >= Buffer.Length())
                            {
                                return 0;
                            }
                            if  (c == ';' || c == '\r' || c == ' ')
                            {
                                break;
                            }
                            throw CMaaString("Chunk error 2");
                        }
                    }
                    if  (i == 0)
                    {
                        throw CMaaString("Chunk error 3");
                    }
                    if  (l == 0 && Buffer[1] != '\r')
                    {
                        throw CMaaString("Chunk error 4");
                    }
                    CMaaString InData = m_InData.GetMemoryString();
                    if  (InData.Length() + l >= MaxDataSize)
                    {
                        //CProtocolListColor c(CProtocolListColor::eRed);
                        //printf("Too long chunked data (>%d)\n", m_MaxDataSize);
                        throw CMaaString::sFormat("Too long chunked data (>%d)\n", MaxDataSize);
                    }
                    n = (warning_int)Buffer.Find(CMaaStringCrLf);
                    if  (n < 0)
                    {
                        int MaxChunkExtNameLength = 4096;
                        if  (Buffer.Length() > MaxChunkExtNameLength)
                        {
                            //CProtocolListColor c(CProtocolListColor::eRed);
                            //printf("Too long chunk-ext-name(s) (>2048)\n");
                            //CloseByException("Too long chunk-ext-name(s) (>2048)");
                            throw CMaaString::sFormat("Too long chunk-ext-name(s) (>%d)", MaxChunkExtNameLength);
                        }
                        return 0;
                    }
                    if  (n + 2 + l + 2 > Buffer.Length())
                    {
                        return 0;
                    }
                    if  (l == 0)
                    {
                        int n2;
                        if  (Buffer.IsMid("\r\n\r\n", n, 4))
                        {
                            n2 = n;
                        }
                        else
                        {
                            n2 = (warning_int)Buffer.Find(n + 2, "\r\n\r\n", 4);
                        }
                        if  (n2 > 0)
                        {
                            //m_InHeader.SetNewLengthValue(m_InHeader.Length() - 2);
                            m_InHeader = m_InHeader - 2 + Buffer.RefMid(n + 2, n2 - n);
                            CMaaString txt;
                            txt.Format("Content-Length: %D\r\n\r\n", (_qword)InData.Length());
                            m_InHeader += txt;
                            CMaaString Temp(Buffer, n2 + 4, Buffer.Length() - (n2 + 4));
                            //m_Buffer = m_Buffer.Mid(n2 + 4);
                            m_Buffer.Empty(true);
                            m_Buffer += Temp;
                            while(1)
                            {
                                StrInt pos = 0, nextpos = 0;
                                const CMaaString txt_ = m_InHeader.HttpGetValue("transfer-encoding:", &pos, &nextpos);
                                if  (nextpos > pos)
                                {
                                    m_InHeader.SetLeftMid(pos, nextpos + 2);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            // All data is received
                            //m_Status = m_StatusOk;
                            return FillReturnResults(Method, Header, Html, Status, NextData);
                        }
                        return 0;
                    }
                    if  (Buffer[n + 2 + l] != '\r' || Buffer[n + 2 + l + 1] != '\n')
                    {
                        throw CMaaString("Chunk error 4");
                    }
                    m_InData += CMaaString(Buffer, n + 2, l);
                    Buffer = CMaaString(Buffer, (n + 2 + l + 2), Buffer.Length() - (n + 2 + l + 2));
                    m_Buffer.Empty(true);
                    m_Buffer += Buffer;
                }
            }
            else if (ContentLength.IsNotEmpty())
            {
                _qword l = 0;
                mysscanf64(ContentLength, &l, (warning_int)ContentLength.Length());
                m_InData += m_Buffer.GetMemoryString();
                m_Buffer.Empty();
                if  (m_InData.GetLength() > (size_t)MaxDataSize)
                {
                    throw CMaaString::sFormat("Too long data (>%d)\n", MaxDataSize);
                }
                if  (m_InData.GetLength() >= (size_t)l || l < 0)
                {
                    // All data is received
                    //m_Status = m_StatusOk;
                    CMaaString Temp = m_InData.GetMemoryString();
                    m_Buffer += CMaaString(Temp, (warning_int)l, Temp.Length() - (warning_int)l);
                    m_InData.SetNewLengthValue((size_t)l);
                    return FillReturnResults(Method, Header, Html, Status, NextData);
                }
            }
            else if (m_hType == eRequest)
            {
                return FillReturnResults(Method, Header, Html, Status, NextData);
            }
            else
            {
                m_InData += m_Buffer.GetMemoryString();
                m_Buffer.Empty();
                if  (m_InData.GetLength() > (size_t)MaxDataSize)
                {
                    throw CMaaString::sFormat("Too long data (>%d)\n", MaxDataSize);
                }
            }
        }
        break;
    }
    return 0;
}
//-----------------------------------------------------------------------------------------
int CHttpResponceHandlerForReader::GetResultsOnUnexpectedConnectionClosed(CMaaString &Header, CMaaString &Html, int &Status)
{
    CMaaString Method;
    return FillReturnResults(Method, Header, Html, Status, (CMaaString *)nullptr, 2);
}
//-----------------------------------------------------------------------------------------
/*
CMaaString tmp_assign_tl_111(CMaaString *a, CMaaString b)
{
    if (a)
    {
        *a = b;
        return *a;
    }
    return b;
}
*/
//-----------------------------------------------------------------------------------------
int CHttpResponceHandlerForReader::FillReturnResults(CMaaString &Method, CMaaString &Header, CMaaString &Html, int &Status, CMaaString &NextData, int CleanType)
{
    return FillReturnResults(Method, Header, Html, Status, &NextData, CleanType);
}
//-----------------------------------------------------------------------------------------
int CHttpResponceHandlerForReader::FillReturnResults(CMaaString &Method, CMaaString &Header, CMaaString &Html, int &Status, CMaaString *NextData, int CleanType)
{
    Method = m_Method;
    Header = m_InHeader;
    Html = (CMaaString)m_InData;
    Status = m_Status;
    if  (NextData != nullptr) // !! 2017
    {
        *NextData = (CMaaString)m_Buffer;
    }
    //tmp_assign_tl_111(&NextData, m_Buffer);
    switch(CleanType)
    {
    case 0:
        break;
    case 3:
    case 2:
        m_Buffer.Empty();
        CMaa_fallthrough;
    case 1:
        m_InHeader.Empty();
        //m_InHeader2.Empty();//++
        m_InHeader_ccstr.Empty();
        m_InData.Empty();
        m_Method.Empty();
        m_Status = -1;
    }
    if  (CleanType == 2)
    {
        if  (Header.IsRight("\r\n\r\n", 4) && Header.FindCi("\r\ncontent-length: ") < 0 && Header.FindCi("\r\ntransfer-encoding: chunked\r\n") < 0)
        {
            return 3;
        }
    }
    return CleanType == 2 ? 2 : 1;
}
//-----------------------------------------------------------------------------------------


//SOCKET g_____socket = -1;

#ifndef TOOLSLIB_SINGLE_THREAD
#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)

//===============================================================================
class CMaaWaitSk0 : public CMaaUdpSocket // CMaaUdpServer
{
public:
    CMaaSockTimerT<CMaaWaitSk0> m_Timer0, m_Timer1/*, m_Timer2*/, m_Timer3;

    CMaaWaitSk0(CMaaFdSockets * pFdSockets);
    ~CMaaWaitSk0();
    void OnTimer(int f);
};
//---------------------------------------------------------------------------
CMaaWaitSk0 * gpMaaWaitSk0 = nullptr;
//----------------------------------------------------------------------------
CMaaWaitSk0::CMaaWaitSk0(CMaaFdSockets * pFdSockets)
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
    //m_Timer1.Start(1);
    //m_Timer3.Start(1800000); // test
    //m_Timer3.Start(2800000); // test
}

CMaaWaitSk0::~CMaaWaitSk0()
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (gpMaaWaitSk0 == this)
    {
        gpMaaWaitSk0 = nullptr;
    }
    Locker_UnLock;
}

void CMaaWaitSk0::OnTimer(int f)
{
    if  (f == 0)
    {
        CloseByException("close");
    }
    else if (f == 1)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        m_Timer1.Stop();
        Locker_UnLock;
    }
    else if (f == 3)
    {
        // tmp
        m_Timer3.Stop();
        //CloseByException("close3");
    }
}
//===============================================================================

#define _Set Set(); Locker_Lock; if (gpMaaWaitSk0) {gpMaaWaitSk0->m_Timer1.Start(1);} Locker_UnLock

class CMaaFileSockThread;
CMaaDList < CMaaDLink_pItem < CMaaFileSockThread > > gTcpFileThreadsList;

class CMaaFileSockThread : public CMaaSockThread
{
    CMaaDLink_pItem<CMaaFileSockThread> m_Link;
public:
    CMaaFileSockThread(CMaaSockThread * pParent)
    :   CMaaSockThread(pParent)
    {
        m_Link = this;
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        gTcpFileThreadsList.AddAtBack(&m_Link);
        Locker_UnLock;
    }
    /*
     unsigned Run()
     {
          return CMaaSockThread::Run()
     }
     */
    ~CMaaFileSockThread()
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        CMaaDList < CMaaDLink_pItem < CMaaFileSockThread > >::Release(&m_Link);
        Locker_UnLock;
    }

    ADD_ALLOCATOR(CMaaFileSockThread)
};
DEF_ALLOCATOR(CMaaFileSockThread)

void SetExitFileSockThreads()
{
    CMaaDLink_pItem<CMaaFileSockThread> * p;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    for (p = gTcpFileThreadsList.LookAtFront(); p; p = gTcpFileThreadsList.Next(p))
    {
        (*p)->Exit(0);
        (*p)->AddCommand(-1);
        //(*p)->WakeUp();
    }
    Locker_UnLock;
}

/*
#ifdef _WIN32
int gettimeofday(timeval *p, void *)
{
     DWORD x = GetTickCount();
     p->tv_sec = 0;
     p->tv_usec = x * 1000;
     return 0;
}
void usleep(int us)
{
     Sleep(us / 1000);
}
#endif
*/

DEF_ALLOCATOR(CMaaTcpFileMainThread)
//--------------------------------------------------------------------------------
CMaaTcpFileMainThread::CMaaTcpFileMainThread(CMaaSockThread * pParent, bool bCreate)
:   CMaaSockThread(pParent),
    m_pCreateNewThread(nullptr),
    m_Param(nullptr),
    m_evReady(true, false),
    m_evStart(true, false)
{
    gpMaaWaitSk0 = TL_NEW CMaaWaitSk0(m_pFdSockets);
    m_Ref0Count = AddRef();
    DelRef();
    if  (bCreate)
    {
        Create();
    }
}

CMaaTcpFileMainThread::~CMaaTcpFileMainThread()
{
    Exit(0);
    CMaaWin32Locker_Locker_gLock;
    m_evReady._Set;
    //Exit(0);
    SetExitFileSockThreads();
    Exit();
}

bool CMaaTcpFileMainThread::AddTask(tCreateNewThread p, void * param)
{
    AddRef();
    bool Ret = false;
    try
    {
        while(!Ret)
        {
            CheckPoint(0);
            if  (!m_evReady.Wait((_dword)1000))
            {
                continue;
            }
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            try
            {
                CheckPoint(0);
            }
            catch(...)
            {
                Locker_UnLock;
                throw;
            }
            if  (!m_pCreateNewThread)
            {
                m_pCreateNewThread = p;
                m_Param = param;
                m_evReady.Reset();
                CMaaWin32Locker_Locker_gLock;
                m_evStart._Set;
                Ret = true;
            }
            Locker_UnLock;
        }
    }
    catch(...)
    {
    }
    DelRef();
    return Ret;
}
void CMaaTcpFileMainThread::Process()
{
    /*int Refs =*/ AddRef();
    m_pFdSockets->SetTimeOut(0, 10000);
#ifdef TL_EPOLL
    m_pFdSockets->SetTimeOut(100, 0);
    //m_pFdSockets->SetTimeOut(0, 10000000);
#endif
    CMaaWin32Locker_Locker_gLock;
    m_evReady._Set;
    try
    {
        while(Process1())
        {
            //printf("1.\n"); fflush(stdout);
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            const int nRefs = AddRef();
            if  (nRefs <= m_Ref0Count + 1 && !m_ChildThreads.LookAtFront())
            {
                //if  (nRefs < m_Ref0Count + 1)
                //{
                //}
                try
                {
                    CheckPoint(0);
                }
                catch(...)
                {
                    DelRef();
                    Locker_UnLock;
                    throw;
                }
            }
            DelRef();
            Locker_UnLock;
            if  (m_evStart.Wait((_dword)1000))
            {
                // add/start
                m_evStart.Reset();
                CMaaWin32Locker_Locker_gLock;
                Locker_Lock;
                CMaaTcpFileMainThread * pThis = this;
                try
                {
                    CheckPoint(0);
                }
                catch(...)
                {
                    pThis = nullptr;
                }
                if  (m_pCreateNewThread)
                {
                    tCreateNewThread p = m_pCreateNewThread;
                    void * tmp = m_Param;
                    m_pCreateNewThread = nullptr;
                    m_Param = nullptr;
                    Locker_UnLock;

                    //gLock.LockM();
                    (*p)(tmp, pThis);
                    m_evReady._Set;
                    //gLock.UnLock();
                }
                else
                {
                    Locker_UnLock;
                }
            }
            //printf("3.\n"); fflush(stdout);
        }
        //printf("4.\n"); fflush(stdout);
    }
    catch(...)
    {
        //printf("CMaaTcpFileMainThread::Process(): catch(...)\n"); fflush(stdout);
        //DelRef();
    }
    //printf("CMaaTcpFileMainThread::Process(): return\n"); fflush(stdout);
    DelRef();
    Process1();
}
//--------------------------------------------------------------------------------
CMaaTcpFileConnection::CMaaTcpFileConnection(CMaaFdSockets * pFdSockets, const char * Server, int DefPort, CMaaTcpFile * pHandler)
:   CMaaTcpSocket(pFdSockets, AF_INET),
    m_pHandler(pHandler),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0),
    m_Timer1(this, 1),
    m_Timer11(this, 11),
    m_Timer2(this, 2),
    m_Timer3(this, 3),
    m_Timer4(this, 4),
    m_Timer33(this, 33),
    m_Timer44(this, 44),
    m_InterruptTimer(this, 0x8)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    {
        _IP6 tmp;
        if  (CMaaIpToLongEx46(Server, tmp.ptr(), ":") >= 0)
        {
            bool b;
            tmp.v4(&b);
            if  (!b)
            {
                int domain = AF_INET6;
                SOCKET s = socket(domain, SOCK_STREAM, IPPROTO_TCP);
                SwapSockets(s, domain);
                closesocket(s);
            }
        }
    }
    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;

    m_Mode = 0;
    //m_EvType = 0;
    m_len = m_wlen = m_pos = m_wpos = 0;
    m_ConnName.Format("Connection to %s", Server);
    AddFdSocket();
    m_Timer0.Attach(pFdSockets);
    m_Timer1.Attach(pFdSockets);
    m_Timer11.Attach(pFdSockets);
    m_Timer3.Attach(pFdSockets);
    m_Timer4.Attach(pFdSockets);
    m_Timer33.Attach(pFdSockets);
    m_Timer44.Attach(pFdSockets);
    m_InterruptTimer.Attach(pFdSockets);
    EnableInterruptEvent();
    //g_____socket = GetSocket();
    if  (GetDomainSock() == AF_INET6)
    {
        AsyncConnect6(Server, DefPort);
    }
    else
    {
        AsyncConnect(Server, DefPort);
    }
}
//--------------------------------------------------------------------------------
CMaaTcpFileConnection::CMaaTcpFileConnection(CMaaFdSockets * pFdSockets, SOCKET s, int domain, CMaaTcpFile * pHandler)
:   CMaaTcpSocket(pFdSockets),
    m_pHandler(pHandler),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0),
    m_Timer1(this, 1),
    m_Timer11(this, 11),
    m_Timer2(this, 2),
    m_Timer3(this, 3),
    m_Timer4(this, 4),
    m_Timer33(this, 33),
    m_Timer44(this, 44),
    m_InterruptTimer(this, 0x8)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;

    m_Mode = 0;
    //m_EvType = 0;
    m_len = m_wlen = m_pos = m_wpos = 0;
    m_ConnName.Format("Connection");
    //    AddFdSocket(); // ++
    /*
    m_Timer0.Attach(pFdSockets);
    m_Timer1.Attach(pFdSockets);
    m_Timer11.Attach(pFdSockets);
    m_Timer2.Attach(pFdSockets);
    m_Timer3.Attach(pFdSockets);
    m_Timer4.Attach(pFdSockets);
    m_Timer33.Attach(pFdSockets);
    m_Timer44.Attach(pFdSockets);
    m_InterruptTimer.Attach(pFdSockets);
    */
    //    AddFdSocket(); // --
    //ChangeFdMode(eDisableAll);
    //ChangeFdMode(eDisableRead|eDisableWrite|eExcept);--
    SetSocket(s, domain); //, AF_INET);
    //
    AddFdSocket (); // ++
    //
    //ChangeFdMode(eRead|eWrite|eExcept); // ++
    ChangeFdModeEx(eDisableRead|eDisableWrite|eExcept, true); // --
    //ChangeFdMode(eRead|eDisableWrite|eExcept); // ++
#ifdef __unix__
    SetNonBlockingMode();
#endif
    m_Timer0.Attach(pFdSockets);
    m_Timer1.Attach(pFdSockets);
    m_Timer11.Attach(pFdSockets);
    m_Timer2.Attach(pFdSockets);
    m_Timer3.Attach(pFdSockets);
    m_Timer4.Attach(pFdSockets);
    m_Timer33.Attach(pFdSockets);
    m_Timer44.Attach(pFdSockets);
    m_InterruptTimer.Attach(pFdSockets);
    try
    {
        if  (GetDomainSock() == AF_INET6)
        {
            GetConnInfo6(m_LocalIp6.ptr(), &m_LocalPort, m_RemoteIp6.ptr(), &m_RemotePort);
            m_LocalIp = m_LocalIp6.v4();
            m_RemoteIp = m_RemoteIp6.v4();
            m_ConnName.Format("Connection from [%J]:%d-->[%J]:%d", m_RemoteIp6.ptr(), m_RemotePort, m_LocalIp6.ptr(), m_LocalPort);
            //__utf8_printf("1.Connection from [%J]:%d-->[%J]:%d\n", m_RemoteIp6.ptr(), m_RemotePort, m_LocalIp6.ptr(), m_LocalPort);
        }
        else
        {
            GetConnInfo(&m_LocalIp, &m_LocalPort, &m_RemoteIp, &m_RemotePort);
            _IP6 l(m_LocalIp), r(m_RemoteIp);
            m_LocalIp6 = l; m_RemoteIp6 = r;
            m_ConnName.Format("Connection from %I:%d-->%I:%d", m_RemoteIp, m_RemotePort, m_LocalIp, m_LocalPort);
        }
    }
    catch(...)
    {
    }

#if 0
    // excessive code. info is filled in Notify_Accepted(0,0)/Notify_Accepted6(000000,0)
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_LocalIp = m_LocalIp;
        m_pHandler->m_LocalIp6 = m_LocalIp6;
        m_pHandler->m_LocalPort = m_LocalPort;
        m_pHandler->m_RemoteIp = m_RemoteIp;
        m_pHandler->m_RemoteIp6 = m_RemoteIp6;
        m_pHandler->m_RemotePort = m_RemotePort;
        //m_pHandler->m_EvType |= CMaaTcpFile::eConnected;
        //m_pHandler->m_ev._Set;
        //__utf8_printf("2.Connection from [%J]:%d-->[%J]:%d\n", m_RemoteIp6.ptr(), m_RemotePort, m_LocalIp6.ptr(), m_LocalPort);
    }
    Locker_UnLock;
#endif

    m_Timer2.Start(1);
    EnableInterruptEvent();
    //g_____socket = GetSocket();
}

CMaaTcpFileConnection::~CMaaTcpFileConnection()
{
    //g_____socket = -1;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_EvType |= CMaaTcpFile::eDestroyed;
        m_pHandler->m_pConn = nullptr;
        m_pHandler->m_ev._Set;
        m_pHandler = nullptr;
    }
    Locker_UnLock;
}

CMaaString CMaaTcpFileConnection::GetConnectionName() noexcept
{
    return m_ConnName;
}

int g_____r = 0;
int g_____mode = 0;
const char * gpsz____ = nullptr;
void CMaaTcpFileConnection::SetTestMessage(CMaaString msg, int Flag)
{
    msg = msg.RemoveCrLfAtEOL();
    msg.Format("%S, Flag = 0x%x, m_flg1 = %d", &msg, Flag, (int)m_flg1);
    m_TestMessage = m_TestMessage + CMaaStringCrLf + msg;
    /*
     if   (msg.IsNotEmpty() && Flag == 0x12345678 && m_flg1 == 1)
     {
          m_TestMessage = msg;
     }
     */
}
int CMaaTcpFileConnection::Notify_Read()
{
    char Buffer[128 * 1024];
    //int req;
    int r = RecvData(Buffer, /*req=*/m_len - m_pos <= (int)sizeof(Buffer) ? m_len - m_pos : (int)sizeof(Buffer));
    //Buffer[r >= 0 ? r : 0] = 0;
    //printf("read %d: req:%d\n", r, req, Buffer); fflush(stdout);
    g_____r = r;
    g_____mode = GetFdMode();
#ifdef DEBUG_SET_TEST_MESSAGE
    SetTestMessage(CMaaString::sFormat("read %d bytes", r));
#endif
    if  (r < 0)
    {
        gpsz____ = m_TestMessage;
        /*
        static int aa = 0;
        aa++;
        aa++;
        aa++;
        */
    }
    if  (IsClosed(r))
    {
        //printf("2.cl! read %d: req:%d\n", r, req, Buffer); fflush(stdout);
        /*
          {
               CMaaFile f("test.txt", CMaaFile::eWCD_SrSw, eNoExcept);
               f.Write(m_TestMessage);
               f.Close();
          }
          */
        Completed();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        if  (m_pHandler)
        {
            m_pHandler->m_EvType |= CMaaTcpFile::eClosed;
            m_pHandler->m_ev._Set;
        }
        Locker_UnLock;
        CloseByException("Close");
    }
    if  (r == 0)
    {
        /*
        static int aa = 0;
        aa++;
        aa++;
        aa++;
        */
    }
    m_TotalBytesReceived += r;
    memcpy(m_ptr + m_pos, Buffer, r);
    m_pos += r;
    if  (r && m_AdoptiveReadTimeOutMs != (_dword)-1)
    {
        m_Timer33.Start(m_AdoptiveReadTimeOutMs * 1000, false);
    }
    if  (r && (m_pos >= m_len || m_ReadTimeOutMs == (_dword)-2 || (m_ReadTimeOutMs < (_dword)-2 && (m_ReadTimeOutMs & 0x80000000))))
    {
        Completed(eRead);
        return eDisableRead;
    }
    CheckInterruptEvent(eRead);
    return eRead;
}

int CMaaTcpFileConnection::Notify_Write()
{
    int MaxSendSize = 10 * 1024 * 1024;
    while(1)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        CMaaString tmp = m_SendData.RefMid(m_wpos, m_wlen - m_wpos <= MaxSendSize ? m_wlen - m_wpos : MaxSendSize);
        Locker_UnLock;
        if  (tmp.IsEmpty())
        {
            Completed(eWrite);
            return eDisableWrite;
        }
        const int w = SendData(tmp, (warning_int)tmp.Length());
        m_TotalBytesSent += w;
        //printf("write %d\n", w); fflush(stdout);
        m_wpos += w;
        if  (w && m_AdoptiveWriteTimeOutMs != (_dword)-1)
        {
            m_Timer44.Start(m_AdoptiveWriteTimeOutMs * 1000, false);
        }
        CheckInterruptEvent(eWrite);
        if  (!w || m_WriteTimeOutMs == (_dword)-2)
        {
            break;
        }
    }
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    int Ret = m_wlen > m_wpos ? eWrite : eDisableWrite;
    Locker_UnLock;
    return Ret;
}

int CMaaTcpFileConnection::Notify_PreDeletingByException()
{
    return 0;
}

bool CMaaTcpFileConnection::AddSendData(CMaaString Data, _dword TimeOutMs, _dword AdoptiveTimeOutMs)
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    EnableInterruptEvent();
    m_SendData += Data;
    m_WriteTimeOutMs = TimeOutMs;
    m_AdoptiveWriteTimeOutMs = AdoptiveTimeOutMs;
    if  (m_SendData.IsNotEmpty() || 1)
    {
        m_Mode |= CMaaTcpFile::eWrite;
        m_wlen = (warning_int)m_SendData.Length();
        m_Timer4.Stop();
        m_Timer11.Start(1);
    }
    Locker_UnLock;
    return true;
}

bool CMaaTcpFileConnection::SetRead(void * ptr, int len, _dword ReadTimeOutMs, _dword AdoptiveTimeOutMs)
{
    bool Ret = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    EnableInterruptEvent();
    if  (!(m_Mode & CMaaTcpFile::eRead))
    {
        m_ptr = (char *)ptr;
        m_pos = 0;
        m_len = len;
        if  (m_len == 0)
        {
            /*
            static int aa = 0;
            aa++;
            aa++;
            aa++;
            */
        }
        m_Mode |= CMaaTcpFile::eRead;
        m_ReadTimeOutMs = ReadTimeOutMs;
        m_AdoptiveReadTimeOutMs = AdoptiveTimeOutMs;
        m_Timer3.Stop();
        m_Timer1.Start(1);
        Ret = true;
    }
    Locker_UnLock;
    return Ret;
}

int CMaaTcpFileConnection::Notify_Error()
{
    if  ((m_Mode & (CMaaTcpFile::eRead | CMaaTcpFile::eWrite)) &&
#ifdef _WIN32
    WSAGetLastError()
#else
    errno
#endif    
     == CMaa_CONN_CLOSED_ERROR)
    {
        //
        if  (m_TotalBytesReceived == 0)
        {
            /*
            static int aa = 0;
            aa++;
            aa++;
            aa++;
            */
        }

        Completed();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        if  (m_pHandler)
        {
            m_pHandler->m_EvType |= CMaaTcpFile::eClosed;
            m_pHandler->m_ev._Set;
        }
        Locker_UnLock;
        CloseByException("Connection is closed");
    }
    XTOOSockErr err("TCP:Notify_Error()", nullptr);
    //printf("error %s\n", (const char *)err.GetOemMsg()); fflush(stdout);
#ifdef DEBUG_SET_TEST_MESSAGE
    SetTestMessage(err.GetMsg());
#endif
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        delete m_pHandler->m_err;
        m_pHandler->m_err = TL_NEW XTOOSockErr(err);
        m_pHandler->m_EvType |= CMaaTcpFile::eExcept;
        m_pHandler->m_ev._Set;
    }
    Locker_UnLock;
    return CloseByException("Error");
}

int CMaaTcpFileConnection::Notify_Connected(_IP /*Ip*/, _Port /*Port*/, const char * /*DnsName*/)
{
    try
    {
        GetConnInfo(&m_LocalIp, &m_LocalPort, &m_RemoteIp, &m_RemotePort);
        _IP6 l(m_LocalIp), r(m_RemoteIp);
        m_LocalIp6 = l; m_RemoteIp6 = r;
    }
    catch(...)
    {
    }

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_LocalIp = m_LocalIp;
        m_pHandler->m_LocalIp6 = m_LocalIp6;
        m_pHandler->m_LocalPort = m_LocalPort;
        m_pHandler->m_RemoteIp = m_RemoteIp;
        m_pHandler->m_RemoteIp6 = m_RemoteIp6;
        m_pHandler->m_RemotePort = m_RemotePort;

        m_pHandler->m_EvType |= CMaaTcpFile::eConnected;
        m_pHandler->m_ev._Set;
    }
    Locker_UnLock;
    m_flg1 = 1;
#ifdef DEBUG_SET_TEST_MESSAGE
    SetTestMessage("connected");
#endif
    return eDisableRead|eDisableWrite|eExcept;
}

int CMaaTcpFileConnection::Notify_Connected6(_byte * /*Ip*/, _Port /*Port*/, const char * /*DnsName*/)
{
    try
    {
        GetConnInfo6(m_LocalIp6.ptr(), &m_LocalPort, m_RemoteIp6.ptr(), &m_RemotePort);
        m_LocalIp = m_LocalIp6.v4();
        m_RemoteIp = m_RemoteIp6.v4();
    }
    catch(...)
    {
    }

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_LocalIp = m_LocalIp;
        m_pHandler->m_LocalIp6 = m_LocalIp6;
        m_pHandler->m_LocalPort = m_LocalPort;
        m_pHandler->m_RemoteIp = m_RemoteIp;
        m_pHandler->m_RemoteIp6 = m_RemoteIp6;
        m_pHandler->m_RemotePort = m_RemotePort;

        m_pHandler->m_EvType |= CMaaTcpFile::eConnected;
        m_pHandler->m_ev._Set;
    }
    Locker_UnLock;
    m_flg1 = 1;
#ifdef DEBUG_SET_TEST_MESSAGE
    SetTestMessage("connected");
#endif
    return eDisableRead|eDisableWrite|eExcept;
}

int CMaaTcpFileConnection::Notify_Accepted(_IP /*IpFrom*/, _Port /*Port*/)
{
    try
    {
        GetConnInfo(&m_LocalIp, &m_LocalPort, &m_RemoteIp, &m_RemotePort);
        _IP6 l(m_LocalIp), r(m_RemoteIp);
        m_LocalIp6 = l; m_RemoteIp6 = r;
    }
    catch(...)
    {
    }

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_LocalIp = m_LocalIp;
        m_pHandler->m_LocalIp6 = m_LocalIp6;
        m_pHandler->m_LocalPort = m_LocalPort;
        m_pHandler->m_RemoteIp = m_RemoteIp;
        m_pHandler->m_RemoteIp6 = m_RemoteIp6;
        m_pHandler->m_RemotePort = m_RemotePort;

        m_pHandler->m_EvType |= CMaaTcpFile::eConnected;
        m_pHandler->m_ev._Set;
    }
    Locker_UnLock;
    return eDisableRead|eDisableWrite|eExcept;
}

int CMaaTcpFileConnection::Notify_Accepted6(_byte * /*IpFrom*/, _Port /*Port*/)
{
    try
    {
        GetConnInfo6(m_LocalIp6.ptr(), &m_LocalPort, m_RemoteIp6.ptr(), &m_RemotePort);
        m_LocalIp = m_LocalIp6.v4();
        m_RemoteIp = m_RemoteIp6.v4();
    }
    catch(...)
    {
    }

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_LocalIp = m_LocalIp;
        m_pHandler->m_LocalIp6 = m_LocalIp6;
        m_pHandler->m_LocalPort = m_LocalPort;
        m_pHandler->m_RemoteIp = m_RemoteIp;
        m_pHandler->m_RemoteIp6 = m_RemoteIp6;
        m_pHandler->m_RemotePort = m_RemotePort;

        m_pHandler->m_EvType |= CMaaTcpFile::eConnected;
        m_pHandler->m_ev._Set;
    }
    Locker_UnLock;
    return eDisableRead|eDisableWrite|eExcept;
}

void CMaaTcpFileConnection::EnableInterruptEvent()
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler && m_pHandler->m_pInterruptEvent)
    {
        //_qword Period = m_pHandler->m_InterruptCheckingTime * 1000;
        int Period = m_pHandler->m_InterruptCheckingTime * 1000;
        if  (!m_InterruptTimer.IsStarted() || m_InterruptTimer.GetPeriod() != Period)
        {
            m_InterruptTimer.Start(Period);
        }
    }
    else
    {
        m_InterruptTimer.Stop();
    }
    Locker_UnLock;
}

void CMaaTcpFileConnection::CheckInterruptEvent(int /*From*/)
{
    bool bInterrupted = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler && m_pHandler->m_pInterruptEvent)
    {
        if  (m_pHandler->m_pInterruptEvent->Wait((_dword)0))
        {
            XTOOSockErr err("CMaaTcpFileConnection: Interrupt event occured", nullptr, nullptr, 0, -1);
            {
                delete m_pHandler->m_err;
                m_pHandler->m_err = TL_NEW XTOOSockErr(err);
                m_pHandler->m_EvType |= CMaaTcpFile::eExcept;
                m_pHandler->m_ev._Set;
            }
            bInterrupted = true;
        }
    }
    Locker_UnLock;
    if  (bInterrupted)
    {
        CloseByException("Interrupt event");
    }
}

void CMaaTcpFileConnection::OnTimer(int f)
{
    //printf("OnTimer(%d)\n", f); fflush(stdout);
    switch(f)
    {
    case 0:
        CloseByException("OnTimer(0)");
        break; // CMaa_fallthrough;
    case 1:
        {
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            m_Timer1.Stop();
            int Mode = m_Mode;
            CMaaString tmp = m_SendData;
            Locker_UnLock;
            if  (Mode & CMaaTcpFile::eRead)
            {
#ifdef DEBUG_SET_TEST_MESSAGE
                SetTestMessage(CMaaString::sFormat("read task for %d data length", m_len));
#endif
                if  (m_len == 0)
                {
                    Completed(CMaaTcpFile::eRead);
                }
                else
                {
                    if  (m_ReadTimeOutMs < (_dword)-2)
                    {
                        m_Timer3.Start((m_ReadTimeOutMs & ~0x80000000) * 1000);
                    }
                    if  (m_AdoptiveReadTimeOutMs != (_dword)-1)
                    {
                        m_Timer33.Start(m_AdoptiveReadTimeOutMs * 1000);
                    }
                    m_flg1 = 2;
                    ChangeFdMode(eRead);
                }
            }
            /*
               if   (Mode & eWrite)
               {
                    if   (tmp.IsEmpty())
                    {
                         Completed(eWrite);
                    }
                    else
                    {
                         if   (m_WriteTimeOutMs < (_dword)-2)
                         {
                              m_Timer4.Start(m_WriteTimeOutMs * 1000);
                         }
                         ChangeFdMode(eWrite);
                    }
               }
               */
        }
        break;
    case 11:
        {
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            m_Timer11.Stop();
            int Mode = m_Mode;
            CMaaString tmp = m_SendData;
            Locker_UnLock;
            if  (Mode & CMaaTcpFile::eWrite)
            {
                if  (tmp.IsEmpty())
                {
                    Completed(CMaaTcpFile::eWrite);
                }
                else
                {
                    if  (m_WriteTimeOutMs < (_dword)-2)
                    {
                        m_Timer4.Start(m_WriteTimeOutMs * 1000);
                    }
                    if  (m_AdoptiveWriteTimeOutMs != (_dword)-1)
                    {
                        m_Timer44.Start(m_AdoptiveWriteTimeOutMs * 1000);
                    }
                    ChangeFdMode(eWrite);
                }
            }
        }
        break;
    case 2:
        {
            m_Timer2.Stop();
            _IP6 tmp;
            ChangeFdMode(m_domain == AF_INET6 ? Notify_Accepted6(tmp.ptr(), 0) : Notify_Accepted(0, 0));
            break;
        }
    case 3:
    case 33:
        if  (f == 3)
        {
#ifdef DEBUG_SET_TEST_MESSAGE
            SetTestMessage(CMaaString::sFormat("timeout, fdmode = 0x%x, %d bytes are received", GetFdMode(), (int)m_TotalBytesReceived));
#endif
            /*
               {
                    CMaaFile f("test_(timeout).txt", CMaaFile::eWCD_SrSw, eNoExcept);
                    f.Write(m_TestMessage);
                    f.Close();
               }
               */
        }
        {
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            m_Timer3.Stop();
            m_Timer33.Stop();
            int Mode = m_Mode;
            Locker_UnLock;
            if  (Mode & CMaaTcpFile::eRead)
            {
                Completed(CMaaTcpFile::eRead | (f == 33 ? CMaaTcpFile::eAdoptiveReadTimeOut : 0));
            }
        }
        break;
    case 4:
    case 44:
        {
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            m_Timer4.Stop();
            m_Timer44.Stop();
            int Mode = m_Mode;
            Locker_UnLock;
            if  (Mode & CMaaTcpFile::eWrite)
            {
                Completed(CMaaTcpFile::eWrite | (f == 44 ? CMaaTcpFile::eAdoptiveWriteTimeOut : 0));
            }
        }
        break;
    case 0x8:
        CheckInterruptEvent();
        break;
    }
}

void CMaaTcpFileConnection::Completed(int Mode)
{
    if  (!Mode)
    {
        Mode = m_Mode & (CMaaTcpFile::eRead | CMaaTcpFile::eWrite);
    }

    //printf("Completed\n"); fflush(stdout);

    int NewMode = 0;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (Mode & CMaaTcpFile::eWrite)
    {
        int len = (warning_int)m_SendData.Length() - m_wpos;
        //m_SendData = m_SendData.Mid(m_wpos);
        m_SendData.Empty();
        len = 0;

        m_Timer4.Stop();

        if  (m_SendData.IsEmpty())
        {
            if  (m_pHandler)
            {
                m_pHandler->m_wlen += m_wpos;
                m_pHandler->m_EvType |= CMaaTcpFile::eWrite | CMaaTcpFile::eComplete | (Mode & CMaaTcpFile::eAdoptiveWriteTimeOut) | (len == 0 ? 0 : CMaaTcpFile::eSoftError);
                m_pHandler->m_ev._Set;
            }
            m_wlen = m_wpos = 0;
            m_Mode &= ~CMaaTcpFile::eWrite;
            NewMode |= eDisableWrite;
        }
        else
        {
            if  (m_pHandler)
            {
                m_pHandler->m_wlen += m_wpos;
                m_pHandler->m_EvType |= CMaaTcpFile::eWrite;
                m_pHandler->m_ev._Set;
            }
            m_wlen -= m_wpos;
            m_wpos = 0;
        }
    }
    else if (Mode & CMaaTcpFile::eRead)
    {
        if  (m_pos >= m_len || 1)
        {
            m_Timer3.Stop();
            if  (m_pHandler)
            {
                m_pHandler->m_len += m_pos;
                m_pHandler->m_EvType |= CMaaTcpFile::eRead | CMaaTcpFile::eComplete | (Mode & CMaaTcpFile::eAdoptiveReadTimeOut);
                m_pHandler->m_ev._Set;
            }
            m_len = m_pos = 0;
            m_Mode &= ~CMaaTcpFile::eRead;
            NewMode |= eDisableRead;
        }
        else
        {
            if  (m_pHandler)
            {
                m_pHandler->m_len += m_pos;
                m_pHandler->m_EvType |= CMaaTcpFile::eRead;
                m_pHandler->m_ev._Set;
            }
            m_len -= m_pos;
            m_ptr += m_pos;
            m_pos = 0;
        }
    }
    Locker_UnLock;
    ChangeFdMode(NewMode);
}
//--------------------------------------------------------------------------------


CMaaTcpFile::CMaaTcpFile(bool bThrowExcept)
:   m_ev(true, false),
    m_pInterruptEvent(nullptr),
    m_err(nullptr),
    m_bThrowExcept(bThrowExcept)
{
    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;

    m_DelayedReadDataLen = -100;

    m_Socket = INVALID_SOCKET;
    m_domain = AF_INET;
    m_pConn = nullptr;
    m_pServer = nullptr;
    m_EvType = 0;
}

CMaaTcpFile::CMaaTcpFile(SOCKET s, int domain, bool bThrowExcept)
:   m_ev(true, false),
    m_pInterruptEvent(nullptr),
    m_err(nullptr),
    m_bThrowExcept(bThrowExcept)
{
    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;

    m_DelayedReadDataLen = -100;

    m_Socket = INVALID_SOCKET;
    m_domain = domain;
    m_pConn = nullptr;
    m_pServer = nullptr;
    m_EvType = 0;
    try
    {
        Open(s, domain);
    }
    catch(...)
    {
        Close();
        throw;
    }
}

CMaaTcpFile::CMaaTcpFile(const char * pszUrl, int DefPort, bool bThrowExcept)
:   m_ev(true, false),
    m_pInterruptEvent(nullptr),
    m_err(nullptr),
    m_bThrowExcept(bThrowExcept)
{
    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;

    m_DelayedReadDataLen = -100;

    m_Socket = INVALID_SOCKET;
    m_domain = AF_INET;
    m_pConn = nullptr;
    m_pServer = nullptr;
    m_EvType = 0;
    try
    {
        Open(CMaaString(pszUrl), DefPort);
    }
    catch(...)
    {
        Close();
        throw;
    }
}

CMaaTcpFile::CMaaTcpFile(CMaaString Url, int DefPort, bool bThrowExcept)
:   m_ev(true, false),
    m_pInterruptEvent(nullptr),
    m_err(nullptr),
    m_bThrowExcept(bThrowExcept)
{
    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;

    m_DelayedReadDataLen = -100;

    m_Socket = INVALID_SOCKET;
    m_domain = AF_INET;
    m_pConn = nullptr;
    m_pServer = nullptr;
    m_EvType = 0;
    try
    {
        Open(Url, DefPort);
    }
    catch(...)
    {
        Close();
        throw;
    }
}
CMaaTcpFile::~CMaaTcpFile()
{
    //printf("CMaaTcpFile::~CMaaTcpFile(): Close()\n"); fflush(stdout);
    Close();
    //printf("CMaaTcpFile::~CMaaTcpFile(): exit...\n"); fflush(stdout);
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    m_pInterruptEvent = nullptr;
    Locker_UnLock;
}

void CMaaTcpFile::fnCreateNewThread(void * param, CMaaSockThread * MainThr)
{
    //unlocked

    CMaaFileSockThread * t = nullptr;
    CMaaTcpFileConnection * c = nullptr;
    CMaaTcpFile * p = (CMaaTcpFile *)param;
    try
    {
        if  (!MainThr)
        {
            throw -1;
        }
        t = TL_NEW CMaaFileSockThread(MainThr);
        if  (!t)
        {
            throw 1;
        }
        c = TL_NEW CMaaTcpFileConnection(t->m_pFdSockets, p->m_Url, p->m_DefPort, p);
        if  (!c)
        {
            throw 2;
        }

        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_pConn = c;
        p->m_EvType |= CMaaTcpFile::eCreated;
        p->m_ev._Set;
        Locker_UnLock;

        if  (!t->Create())
        {
            XTOOLastError err("Create thread error");
            throw XTOOSockErr(err.GetMsg(), nullptr, nullptr, 0, -1);
        }
    }
    catch(XTOOSockErr err)
    {
        /*
                         CMaaWin32Locker_Locker_gLock;
                         Locker_Lock;
                         if   (m_pHandler)
                         {
                              m_pHandler->m_EvType |= CMaaTcpFile::eDestroyed;
                              m_pHandler->m_pConn = nullptr;
                              m_pHandler->m_ev._Set;
                              m_pHandler = nullptr;
                         }
                         Locker_UnLock;
          */
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_err = TL_NEW XTOOSockErr(err);
        p->m_EvType |= CMaaTcpFile::eExcept;// | CMaaTcpFile::eDestroyed;
        //p->m_ev._Set;
        p->m_EvType |= CMaaTcpFile::eDestroyed;
        p->m_ev._Set;
        Locker_UnLock;
        delete c;
        delete t;

        //Locker_Lock;
        //p->m_EvType |= CMaaTcpFile::eDestroyed;
        //p->m_ev._Set;
        //Locker_UnLock;
    }
    catch(...)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_EvType |= CMaaTcpFile::eDestroyed;
        p->m_ev._Set;
        Locker_UnLock;
        delete c;
        delete t;
    }
}

void CMaaTcpFile::fnCreateNewThread2(void * param, CMaaSockThread * MainThr)
{
    //unlocked

    CMaaFileSockThread * t = nullptr;
    CMaaTcpFileConnection * c = nullptr;
    CMaaTcpFile * p = (CMaaTcpFile *)param;
    SOCKET s = p->m_Socket;
    int domain = p->m_domain;
    try
    {
        if  (!MainThr)
        {
            throw -1;
        }
        t = TL_NEW CMaaFileSockThread(MainThr);
        if  (!t)
        {
            throw 1;
        }
        c = TL_NEW CMaaTcpFileConnection(t->m_pFdSockets, s, domain, p);
        if  (!c)
        {
            throw 2;
        }
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;

        s = p->m_Socket = INVALID_SOCKET;
        //p->m_domain = AF_INET;
        p->m_pConn = c;
        //__utf8_printf("%J  %J\n", c->m_RemoteIp6.ptr(), p->m_RemoteIp6.ptr());

        p->m_EvType |= CMaaTcpFile::eCreated;
        p->m_ev._Set;
        Locker_UnLock;

        if  (!t->Create())
        {
            XTOOLastError err("Create thread error");
            throw XTOOSockErr(err.GetMsg(), nullptr, nullptr, 0, -1);
        }
    }
    catch(XTOOSockErr err)
    {
        if  (s != INVALID_SOCKET)
        {
            closesocket(s);
        }
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_err = TL_NEW XTOOSockErr(err);
        p->m_EvType |= CMaaTcpFile::eExcept;// | CMaaTcpFile::eDestroyed;
        //p->m_ev._Set;
        p->m_EvType |= CMaaTcpFile::eDestroyed;
        p->m_ev._Set;
        Locker_UnLock;
        delete c;
        delete t;

        //gLock.LockM();
        //p->m_EvType |= CMaaTcpFile::eDestroyed;
        //p->m_ev._Set;
        //gLock.UnLockM();
    }
    catch(...)
    {
        if  (s != INVALID_SOCKET)
        {
            closesocket(s);
        }
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_EvType |= CMaaTcpFile::eDestroyed;
        p->m_ev._Set;
        Locker_UnLock;
        delete c;
        delete t;
    }
}

void CMaaTcpFile::fnCreateNewThreadServer(void * param, CMaaSockThread * MainThr)
{
    //unlocked

    CMaaFileSockThread * t = nullptr;
    CMaaTcpFileServer * s = nullptr;
    CMaaTcpFile * p = (CMaaTcpFile *)param;
    try
    {
        //CMaaFile f("log__1.txt", CMaaFile::eAC_SrSw, eNoExcept);
        //f.fprintf("point 1\n"); f.Flush();
        if  (!MainThr)
        {
            throw -1;
        }
        //f.fprintf("point 2\n"); f.Flush();
        t = TL_NEW CMaaFileSockThread(MainThr);
        if  (!t)
        {
            throw 1;
        }
        //f.fprintf("point 3\n"); f.Flush();
        s = TL_NEW CMaaTcpFileServer(t->m_pFdSockets, p->m_BindPort, p->m_BindIp, p, p->m_bBindReuse);
        if  (!s)
        {
            throw 2;
        }

        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;

        p->m_pServer = s;

        p->m_EvType |= CMaaTcpFile::eCreated;
        p->m_ev._Set;
        Locker_UnLock;

        if  (!t->Create())
        {
            XTOOLastError err("Create thread error");
            throw XTOOSockErr(err.GetMsg(), nullptr, nullptr, 0, -1);
        }
    }
    catch(XTOOSockErr err)
    {
        //__utf8_printf("err\n"); __utf8_printf_flush();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_err = TL_NEW XTOOSockErr(err);
        p->m_EvType |= CMaaTcpFile::eExcept;// | CMaaTcpFile::eDestroyed;
        //p->m_ev._Set;
        p->m_EvType |= CMaaTcpFile::eDestroyed;
        p->m_ev._Set;
        Locker_UnLock;
        delete s;
        delete t;

        //gLock.LockM();
        //p->m_EvType |= CMaaTcpFile::eDestroyed;
        //p->m_ev._Set;
        //gLock.UnLockM();
    }
    catch(...)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        p->m_EvType |= CMaaTcpFile::eDestroyed;
        p->m_ev._Set;
        Locker_UnLock;
        delete s;
        delete t;
    }
}

bool CMaaTcpFile::GetError(XTOOSockErr &err, int iThrowExcept)
{
    bool bbb = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_EvType & (eExcept|eDestroyed|eSoftError))
    {
        if  (m_err && ((m_bThrowExcept && iThrowExcept >= 0) || (!m_bThrowExcept && iThrowExcept > 0)))
        {
            XTOOSockErr err(*m_err);
            delete m_err; m_err = nullptr;
            Locker_UnLock;
            throw err;
        }
        else if (m_err)
        {
            err = *m_err;
            bbb = true;
        }
    }
    Locker_UnLock;
    return bbb;
}

bool CMaaTcpFile::IsError()
{
    bool bbb = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_EvType & (eExcept|eDestroyed|eSoftError))
    {
        if  (m_err && m_bThrowExcept)
        {
            XTOOSockErr err(*m_err);
            delete m_err; m_err = nullptr;
            Locker_UnLock;
            throw err;
        }
        else if (m_err)
        {
            bbb = true;
        }
    }
    if  ((m_EvType & eClosed) || (!m_pConn && !m_pServer))
    {
        if  (m_bThrowExcept)
        {
            XTOOSockErr err("CMaaTcpFile: ", nullptr, nullptr, CMaa_CONN_CLOSED_ERROR);
            Locker_UnLock;
            throw err;
        }
        bbb = true;
    }
    Locker_UnLock;
    return bbb;
}

bool CMaaTcpFile::Open(CMaaString Url, int DefPort)
{
    bool Ret = false;

    //if   (m_pConn || m_pServer)
    {
        Close();
    }

    if  (Url[0] == '|')
    {
        bool bAccept = false;
        if  (Url[1] == '+')
        {
            bAccept = true;
        }
        //__utf8_printf("url = %S, bAccept = %s\n", &Url, bAccept ? "true" : "false");
        Url = Url.Mid(bAccept ? 2 : 1);

        //__utf8_printf("url = %S\n", &Url);
        _IP6 BindIp;
        int trailer_pos = CMaaIpToLongEx46(Url, BindIp.ptr(), ":");

        //__utf8_printf("%d %I\n", trailer_pos, BindIp);
        Url = Url.Mid(trailer_pos > 0 ? trailer_pos + 1: Url[0] == ':' ? 1 : 0);
        //__utf8_printf("Url = %S\n", &Url);
        Url.sscanfInt(&DefPort);
        //__utf8_printf("opening %I:%d for listening\n", BindIp, DefPort);
        Ret = Bind(DefPort, BindIp, Url.LastChar() == 'r');
        if  (Ret && bAccept)
        {
            int domain;
            SOCKET s = Accept((_dword)-1, &domain);
            Ret = Open(s, domain);
        }
        return Ret;
    }

    /*
     if   (!gpSockStartup || !gpSockStartup->m_pTcpFileMainThread)
     {
          if   (m_bThrowExcept)
          {
               XTOOSockErr err("Sockets library is not initialized", nullptr, nullptr, 0);
               throw err;
          }
          return false;
     }
     */

    m_Url = Url;
    m_DefPort = DefPort;

    //printf("m_EvType = %x\n", m_EvType);

    m_ev.Reset();
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    bool bAdded = false;
    if  (gpSockStartup && gpSockStartup->m_pTcpFileMainThread)
    {
        gpSockStartup->m_pTcpFileMainThread->AddRef();
        Locker_UnLock;
        bAdded = gpSockStartup->m_pTcpFileMainThread->AddTask(fnCreateNewThread, this);
        Locker_Lock;
        gpSockStartup->m_pTcpFileMainThread->DelRef();
        if  (!bAdded)
        {
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                XTOOSockErr err("CMaaTcpFile::Open(): task not added", nullptr, nullptr, 0, -1);
                throw err;
            }
            return false;
        }
    }
    Locker_UnLock;

    if  (!bAdded)
    {
        if  (m_bThrowExcept)
        {
            XTOOSockErr err("Sockets library is not initialized", nullptr, nullptr, 0, -1);
            throw err;
        }
        return false;
    }

    while(!Ret)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //int EvType = m_EvType;
        //printf("EvType = %x\n", EvType);
        if  (m_EvType & eDestroyed)
        {
            m_EvType &= eExcept;
            Locker_UnLock;
            break;
        }
        if  (m_EvType & eCreated)
        {
            Ret = true;
        }
        Locker_UnLock;
    }

    while(Ret)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //printf("m_EvType = %x\n", m_EvType);
        if  (m_EvType & eConnected)
        {
            m_ev.Reset();
            Locker_UnLock;
            break;
        }
        else if ((m_EvType & eExcept) && m_err)
        {
            m_EvType &= ~eExcept;
            XTOOSockErr err(*m_err);
            delete m_err;  m_err = nullptr;
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                throw err;
            }
            Ret = false;
        }
        else
        {
            m_ev.Reset();
            Locker_UnLock;
        }
    }
    if  (!Ret)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        if  ((m_EvType & eExcept) && m_err)
        {
            m_EvType &= ~eExcept;
            XTOOSockErr err(*m_err);
            delete m_err;  m_err = nullptr;
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                throw err;
            }
        }
        else
        {
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                XTOOSockErr err("CMaaTcpFile::Open(): Unknown error", nullptr, nullptr, 0, -1);
                throw err;
            }
        }
    }

    //printf("Open(): return %s\n", Ret ? "true" : "false");

    return Ret;
}

bool CMaaTcpFile::IsOpen()
{
    if  (m_pConn || m_pServer)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        int ev = (m_EvType & (eClosed|eExcept|eDestroyed|eSoftError));
        Locker_UnLock;
        return ev ? false : true;
    }
    return false;
}

bool CMaaTcpFile::Open(SOCKET s, int domain)
{
    bool Ret = false;

    if  (m_pConn || m_pServer)
    {
        Close();
    }

    m_ev.Reset();

    m_Socket = s;
    m_domain = domain;

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    bool bAdded = false;
    if  (gpSockStartup && gpSockStartup->m_pTcpFileMainThread)
    {
        gpSockStartup->m_pTcpFileMainThread->AddRef();
        Locker_UnLock;
        bAdded = gpSockStartup->m_pTcpFileMainThread->AddTask(fnCreateNewThread2, this);
        Locker_Lock;
        gpSockStartup->m_pTcpFileMainThread->DelRef();
        if  (!bAdded)
        {
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                XTOOSockErr err("CMaaTcpFile::Open(): task not added", nullptr, nullptr, 0, -1);
                throw err;
            }
            return false;
        }
    }
    Locker_UnLock;

    if  (!bAdded)
    {
        closesocket(s);
        m_Socket = INVALID_SOCKET;
        m_domain = AF_INET;
        if  (m_bThrowExcept)
        {
            XTOOSockErr err("Sockets library is not initialized", nullptr, nullptr, 0, -1);
            throw err;
        }
        return false;
    }

    while(!Ret)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //int EvType = m_EvType;
        //printf("EvType = %x\n", EvType);
        if  (m_EvType & eDestroyed)
        {
            m_Socket = INVALID_SOCKET;
            m_domain = AF_INET;
            //m_EvType &= eExcept;
            Locker_UnLock;
            break;
        }
        if  (m_EvType & eCreated)
        {
            m_Socket = INVALID_SOCKET;
            //m_domain = AF_INET;
            Ret = true;
        }
        Locker_UnLock;
    }

    while(Ret)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //printf("m_EvType = %x\n", m_EvType);
        if  (m_EvType & eConnected)
        {
            m_ev.Reset();
            Locker_UnLock;
            break;
        }
        else if ((m_EvType & eExcept) && m_err)
        {
            m_EvType &= ~eExcept;
            XTOOSockErr err(*m_err);
            delete m_err;  m_err = nullptr;
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                throw err;
            }
            Ret = false;
        }
        else
        {
            m_ev.Reset();
            Locker_UnLock;
        }
    }

    if  (!Ret)
    {
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        if  ((m_EvType & eExcept) && m_err)
        {
            m_EvType &= ~eExcept;
            XTOOSockErr err(*m_err);
            delete m_err;  m_err = nullptr;
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                throw err;
            }
        }
        else
        {
            m_EvType &= ~eExcept;
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                XTOOSockErr err("CMaaTcpFile::Open(): Unknown error", nullptr, nullptr, 0, -1);
                throw err;
            }
        }
    }

    //printf("Open(): return %s\n", Ret ? "true" : "false");

    return Ret;
}

bool CMaaTcpFile::Bind(_Port Port, _IP6 Ip, bool bReuse)
{
    bool Ret = false;

    if  (m_pConn || m_pServer)
    {
        Close();
    }
    if  (!gpSockStartup || !gpSockStartup->m_pTcpFileMainThread)
    {
        return false;
    }

    m_BindPort = Port;
    m_BindIp = Ip;
    m_bBindReuse = bReuse;

    m_ev.Reset();
    //__utf8_printf("gpSockStartup->m_pTcpFileMainThread->AddTask()\n");
    gpSockStartup->m_pTcpFileMainThread->AddTask(fnCreateNewThreadServer, this);
    while(!Ret)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //int EvType = m_EvType;
        //printf("EvType = %x\n", EvType);
        if  (m_EvType & eDestroyed)
        {
            m_EvType &= eExcept;
            Locker_UnLock;
            break;
        }
        if  (m_EvType & eCreated)
        {
            Ret = true;
        }
        Locker_UnLock;
    }

    while(Ret)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //printf("m_EvType = %x\n", m_EvType);
        if  (m_EvType & eConnected)
        {
            m_ev.Reset();
            Locker_UnLock;
            break;
        }
        else if ((m_EvType & eExcept) && m_err)
        {
            m_EvType &= ~eExcept;
            XTOOSockErr err(*m_err);
            delete m_err;  m_err = nullptr;
            Locker_UnLock;
            if  (m_bThrowExcept)
            {
                throw err;
            }
            Ret = false;
        }
        else
        {
            m_ev.Reset();
            Locker_UnLock;
        }
    }

    //printf("Bind(): return %s\n", Ret ? "true" : "false");

    //__utf8_printf("Bind(): return %s\n", Ret ? "true" : "false");
    return Ret;
}

bool CMaaTcpFile::Close()
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;

    m_LocalIp = m_RemoteIp = 0;
    m_LocalPort = m_RemotePort = 0;
    m_LocalIp6.Zero();
    m_RemoteIp6.Zero();

    if  (m_pConn)
    {
        m_EvType &= ~eDestroyed;
        m_ev.Reset();
        m_pConn->m_Timer0.Start(1);
        Locker_UnLock;
        for (;;)
        {
            m_ev.Wait();
            Locker_Lock;
            int EvType = m_EvType;
            Locker_UnLock;
            if  (EvType & eDestroyed)
            {
                break;
            }
        }
        Locker_Lock;
        m_EvType = 0;
        delete m_err;  m_err = nullptr;
        Locker_UnLock;
        //printf("Close(): m_EvType = %x, m_pConn = %p, return true\n", m_EvType, m_pConn);
        return true;
    }
    else if (m_pServer)
    {
        m_EvType &= ~eDestroyed;
        m_ev.Reset();
        m_pServer->m_Timer0.Start(1);
        Locker_UnLock;
        for (;;)
        {
            m_ev.Wait();
            Locker_Lock;
            int EvType = m_EvType;
            Locker_UnLock;
            if  (EvType & eDestroyed)
            {
                break;
            }
        }
        Locker_Lock;
        m_EvType = 0;
        delete m_err;  m_err = nullptr;
        Locker_UnLock;
        //printf("Close(): m_EvType = %x, m_pServer = %p, return true\n", m_EvType, m_pServer);
        return true;
    }
    else
    {
        m_EvType = 0;
        delete m_err; m_err = nullptr;
        m_ev.Reset();
    }
    Locker_UnLock;
    //printf("Close(): m_EvType = %x, m_pConn = %p, m_pServer = %p, return false\n", m_EvType, m_pConn, m_pServer);
    return false;
}

void CMaaTcpFile::SetInterruptEvent(CMaaEvent *pInterruptEvent, _dword InterruptCheckingTime)
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    m_pInterruptEvent = pInterruptEvent;
    m_InterruptCheckingTime = InterruptCheckingTime;
    if  (m_pConn)
    {
        m_pConn->EnableInterruptEvent();
    }
    if  (m_pServer)
    {
        m_pServer->EnableInterruptEvent();
    }
    Locker_UnLock;
}

bool CMaaTcpFile::StartDelayedRead(int len, _dword TimeOutMs, _dword AdoptiveTimeOutMs, bool bThrow)
{
    if  (m_DelayedReadDataLen != -100)
    {
        return false;
    }
    if  (m_DelayedReadData.Length() < 100 * 1024)
    {
        m_DelayedReadData = CMaaString(nullptr, 100 * 1024);
    }
    if  (m_DelayedReadData.Length() < 100 * 1024)
    {
        return false;
    }

    len = len < 100 * 1024 ? len : 100 * 1024;

    m_len = 0;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
#ifdef SAFE_GLOCKS
    if  (IsError_locked(Locker, bThrow))
#else
    if  (IsError_locked(bThrow))
#endif
    {
        Locker_UnLock;
        return false;//x > 0 ? x : -1;
    }
    if  (!m_pConn)
    {
        Locker_UnLock;
        return false;//x > 0 ? x : -1;
    }
    if  (!m_pConn->SetRead((char *)(const char *)m_DelayedReadData, len, TimeOutMs, AdoptiveTimeOutMs))
    {
        Locker_UnLock;
        return false;//x > 0 ? x : -1;
    }
    Locker_UnLock;
    m_DelayedReadDataLen = -1;
    return true;
}

int gtmp123 = 0, gtmp4567 = 0;
int gtmpstep890 = 0;

int CMaaTcpFile::GetDelayedReadData(CMaaString &txt, _dword to, bool bThrow)
{
    gtmp123 = 0;
    txt.Empty();
    if  (m_DelayedReadDataLen == -1)
    {
        to = to & 0x7fffffff;
        _dword t = GetTickCount();
        bool b1st = true;
        for (;GetTickCount() - t <= to || b1st;)
        {
            b1st = false;
            int _to = (int)(to - (GetTickCount() - t));
            _to = _to >= 0 ? _to : 0;
            m_ev.Wait((_dword)_to);
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            if  ((m_EvType & eComplete) || (m_EvType & (eClosed|eExcept|eDestroyed|eSoftError)) == eClosed)
            {
                if  (m_len == 0)
                {
                    /*
                    static int aa = 0;
                    aa++;
                    aa++;
                    aa++;
                    static int xxx = 0;
                    xxx = m_EvType;
                    */
                }
                int ev = m_EvType;
                if  ((m_EvType & (eRead | eWrite)) == (eRead | eWrite))
                {
                    m_EvType &= ~(eRead | eAdoptiveReadTimeOut);
                }
                else
                {
                    m_EvType &= ~(eComplete | eRead | eAdoptiveReadTimeOut);
                }
                if  (!m_EvType)
                {
                    m_ev.Reset();
                }
                Locker_UnLock;
                //txt = m_DelayedReadData.Left(m_len >= 0 ? m_len : 0);
                txt = CMaaString((const char *)m_DelayedReadData, m_len >= 0 ? m_len : 0);
                gtmp123 = m_len;
                int len2 = m_len;
                m_len = 0;
                m_DelayedReadDataLen = -100;
                return (ev & eAdoptiveReadTimeOut) ? -1 : len2;
            }
#ifdef SAFE_GLOCKS
            if  (IsError_locked(Locker, false))
            {
                m_DelayedReadDataLen = -100;
                IsError_locked(Locker, bThrow);
                Locker_UnLock;
                return -10;
            }
#else
            if  (IsError_locked(false))
            {
                m_DelayedReadDataLen = -100;
                IsError_locked(bThrow);
                Locker_UnLock;
                return -10;
            }
#endif
            m_ev.Reset();
            Locker_UnLock;
        }
        return -100;
    }
    else if (m_DelayedReadDataLen == -100)
    {
        return -2;
    }
    return -10;
}

int CMaaTcpFile::Read(void * ptr, int len, _dword TimeOutMs, _dword AdoptiveTimeOutMs)
{
    gtmp4567 = 0;
    if  (m_ReadData.IsNotEmpty() && (TimeOutMs & 0x80000000))
    {
        const int l = len < (warning_int)m_ReadData.Length() ? len : (warning_int)m_ReadData.Length();
        memcpy(ptr, m_ReadData, l);
        m_ReadData = m_ReadData.Mid(l);
        gtmp4567 = l;
        return l;
    }
    {
        const int l = len - (warning_int)m_ReadData.Length();
        if  (l > 0)
        {
            StartDelayedRead(l, TimeOutMs, AdoptiveTimeOutMs, false);
        }
    }

    int x = 0;

    int dd = 0;
    if  (m_ReadData.Length() <= len)
    {
        CMaaString txt;
        dd = GetDelayedReadData(txt, 0, false);
        m_ReadData += txt;
    }

    int nn = 0;
    _dword t0 = GetTickCount();
    while(1)
    {
        _dword t = GetTickCount() - t0;

        nn++;
        char * p = (char *)ptr;
        const int y = (warning_int)m_ReadData.Length() <= len - x ? (warning_int)m_ReadData.Length() : len - x;
        memcpy(p + x, m_ReadData, y);
        m_ReadData = m_ReadData.Mid(y);
        gtmp4567 += y;
        x += y;
        if  (dd >= 0 && (TimeOutMs & 0x80000000))
        {
            gtmpstep890 = 4;
            break;
        }
        if  (x < len)
        {
            if  (dd == -100)
            {
                CMaaString txt;
                gtmpstep890 = 1;
                dd = GetDelayedReadData(txt, (_dword)-1, true);
                gtmpstep890 = 0;
                m_ReadData += txt;
            }
            else
            {
                gtmpstep890 = 10000 + dd;
                break;
            }
            const int l = len - x - (warning_int)m_ReadData.Length();
            if  (l > 0 && dd > 0 && !(TimeOutMs & 0x80000000))
            {
                if  ((TimeOutMs & 0x7fffffff) > t)
                {
                    _dword t2 = (TimeOutMs & 0x7fffffff) - t;
                    t2 |= (TimeOutMs & 0x80000000);
                    if  (StartDelayedRead(l, t2, AdoptiveTimeOutMs, false))
                    {
                        dd = -100;
                        continue;
                    }
                }
            }
            if  ((dd > 0 || dd == -1) && m_ReadData.IsNotEmpty())
            {
                continue;
            }
            gtmpstep890 = 100000 + dd;
            break;
        }
        gtmpstep890 = 55;
        break;
    }

    if  (x <= 0)
    {
        gtmpstep890 += 20000;
        x = IsError() ? -1 : x;
        gtmpstep890 -= 20000;
    }
    return x;

    /*
     m_len = 0;
     CMaaWin32Locker_Locker_gLock;
     Locker_Lock;
     if   (IsError_locked())
     {
          Locker_UnLock;
          return x > 0 ? x : -1;
     }
     if   (!m_pConn)
     {
          Locker_UnLock;
          return x > 0 ? x : -1;
     }
     if   (!m_pConn->SetRead(ptr, len - x, TimeOutMs))
     {
          Locker_UnLock;
          return x > 0 ? x : -1;
     }
     Locker_UnLock;
     for  (;;)
     {
          m_ev.Wait();
          Locker_Lock;
          if   ((m_EvType & eComplete) || (m_EvType & (eClosed|eExcept|eDestroyed|eSoftError)) == eClosed)
          {
               if   ((m_EvType & (eRead | eWrite)) == (eRead | eWrite))
               {
                    m_EvType &= ~(eRead);
               }
               else
               {
                    m_EvType &= ~(eComplete | eRead);
               }
               if   (!m_EvType)
               {
                    m_ev.Reset();
               }
               Locker_UnLock;
               return m_len + x;
          }
          if   (IsError_locked())
          {
               Locker_UnLock;
               return x > 0 ? x : -1;
          }
          m_ev.Reset();
          Locker_UnLock;
     }
     return x > 0 ? x : -1;
     */
}

bool CMaaTcpFile::Peek(CMaaString txt)
{
    StrInt NewLen = txt.Length() + m_ReadData.Length();

    m_ReadData = txt + m_ReadData;

    return m_ReadData.Length() == NewLen;
}

int CMaaTcpFile::ReadLine(void * ptr, int len, _dword TimeOutMs, CMaaString eof)
{
    char * p = (char *)ptr;
    int z = (warning_int)m_ReadData.Find(0, eof, len);
    if  (z >= 0)
    {
        memcpy(p, m_ReadData, z += (warning_int)eof.Length());
        m_ReadData = m_ReadData.Mid(z);
        return z;
    }
    int x = (warning_int)m_ReadData.Length() <= len ? (warning_int)m_ReadData.Length() : len;
    memcpy(p, m_ReadData, x);
    m_ReadData = m_ReadData.Mid(x);

    _dword tk = GetTickCount() + TimeOutMs;
    while(x < len)
    {
        int to = (int)(tk - GetTickCount());
        if  (to <= 0)
        {
            break;
        }
        int y = Read(p + x, len - x, (_dword)to | 0x80000000/*, (_dword)-1*/);

        //printf("ReadLine(): y = %d\n", y); fflush(stdout);

        if  (y < 0)
        {
            break;
        }
        x += y;
        CMaaString tmp(p, x);
        const int z = (warning_int)tmp.Find(eof);

        //printf("ReadLine(): tmp = %s, z = %d, eof.Length() == %d\n", (const char *)tmp, z, eof.Length()); fflush(stdout);

        if  (z >= 0)
        {
            m_ReadData = tmp.Mid(x = z + (warning_int)eof.Length()) + m_ReadData;
            break;
        }
    }
    return x;
}

SOCKET CMaaTcpFile::Accept(_dword TimeOutMs, int * pDomain)
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    //__utf8_printf("e0 point 0001\n");
#ifdef SAFE_GLOCKS
    if  (IsError_locked(Locker))
#else
    if  (IsError_locked())
#endif
    {
        Locker_UnLock;
        //__utf8_printf("e1 point 0001\n");
        return INVALID_SOCKET;
    }
    if  (!m_pServer)
    {
        Locker_UnLock;
        //__utf8_printf("e2 point 0001\n");
        return INVALID_SOCKET;
    }
    SOCKET s = INVALID_SOCKET;
    _IP IpFrom = 0;
    _Port PortFrom = 0;
    _IP6 IpFrom6;
    //__utf8_printf("point 0001\n");
    if  (m_pServer->GetDomainSock() == AF_INET6)
    {
        if  (!m_pServer->AcceptSocket6(&s, IpFrom6.ptr(), &PortFrom, TimeOutMs))
        {
            Locker_UnLock;
            return INVALID_SOCKET;
        }

        if  (pDomain)
        {
            *pDomain = AF_INET6;
        }
    }
    else
    {
        if  (!m_pServer->AcceptSocket(&s, &IpFrom, &PortFrom, TimeOutMs))
        {
            Locker_UnLock;
            return INVALID_SOCKET;
        }
        if  (pDomain)
        {
            *pDomain = AF_INET;
        }
    }
    Locker_UnLock;
    for (;;)
    {
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        if  ((m_EvType & eComplete) || (m_EvType & (eClosed|eExcept|eDestroyed|eSoftError)) == eClosed)
        {
            m_EvType &= ~eComplete;
            if  (!m_EvType)
            {
                m_ev.Reset();
            }
            Locker_UnLock;
            return s;
        }
#ifdef SAFE_GLOCKS
        if  (IsError_locked(Locker))
#else
        if  (IsError_locked())
#endif
        {
            Locker_UnLock;
            return INVALID_SOCKET;
        }
        m_ev.Reset();
        Locker_UnLock;
    }
    return INVALID_SOCKET;
}

#ifdef SAFE_GLOCKS
bool CMaaTcpFile::IsError_locked(CMaaWin32Locker<CMaa_gLock_lib_Mutex> &Locker, bool bThrow)
{
    //__utf8_printf("m_EvType & (eExcept|eDestroyed|eSoftError) ? : %08x %08x %08x %08x\n", m_EvType, eExcept, eDestroyed, eSoftError);
    bool bbb = false;
    if  (m_EvType & (eExcept|eDestroyed|eSoftError))
    {
        if  (m_err && bThrow && m_bThrowExcept)
        {
            XTOOSockErr err(*m_err);
            delete m_err; m_err = nullptr;
            Locker_UnLock;
            throw err;
        }
        bbb = true;
    }
    if  ((m_EvType & eClosed) || (!m_pConn && !m_pServer))
    {
        if  (bThrow && m_bThrowExcept)
        {
            XTOOSockErr err("CMaaTcpFile: ", nullptr, nullptr, CMaa_CONN_CLOSED_ERROR);
            Locker_UnLock;
            throw err;
        }
        bbb = true;
    }
    return bbb;
}
#else
bool CMaaTcpFile::IsError_locked(bool bThrow)
{
    //__utf8_printf("m_EvType & (eExcept|eDestroyed|eSoftError) ? : %08x %08x %08x %08x\n", m_EvType, eExcept, eDestroyed, eSoftError);
    bool bbb = false;
    if  (m_EvType & (eExcept | eDestroyed | eSoftError))
    {
        if  (m_err && bThrow && m_bThrowExcept)
        {
            XTOOSockErr err(*m_err);
            delete m_err; m_err = nullptr;
            gLock.UnLockM();
            throw err;
        }
        bbb = true;
    }
    if  ((m_EvType & eClosed) || (!m_pConn && !m_pServer))
    {
        if  (bThrow && m_bThrowExcept)
        {
            XTOOSockErr err("CMaaTcpFile: ", nullptr, nullptr, CMaa_CONN_CLOSED_ERROR);
            gLock.UnLockM();
            throw err;
        }
        bbb = true;
    }
    return bbb;
}
#endif
int CMaaTcpFile::Write(const void * ptr, int len, _dword TimeOutMs, _dword AdoptiveTimeOutMs)
{
    CMaaString SendMsg(ptr, len);
    return Write(SendMsg, TimeOutMs, AdoptiveTimeOutMs);
}
int CMaaTcpFile::Write(CMaaString SendMsg, _dword TimeOutMs, _dword AdoptiveTimeOutMs)
{
    /*
     {
          CMaaString txt;
          int dd = GetDelayedReadData(txt, (_dword)-1);
          m_ReadData += txt;
     }
     */
    m_wlen = 0;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
#ifdef SAFE_GLOCKS
    if  (IsError_locked(Locker))
#else
    if  (IsError_locked())
#endif
    {
        Locker_UnLock;
        //printf("err -1\n");
        return -1;
    }
    if  (!m_pConn)
    {
        Locker_UnLock;
        //printf("err -1\n");
        return -1;
    }
    if  (!m_pConn->AddSendData(SendMsg, TimeOutMs, AdoptiveTimeOutMs))
    {
        Locker_UnLock;
        //printf("err -1\n");
        return -1;
    }
    Locker_UnLock;
    for (;;)
    {
        //printf("wait\n");
        m_ev.Wait();
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        //printf("m_EvType = %x\n", m_EvType);
        if  ((m_EvType & eComplete) || (m_EvType & (eClosed|eExcept|eDestroyed|eSoftError)) == eClosed)
        {
            //printf("...1...\n");
            if  ((m_EvType & (eRead | eWrite)) == (eRead | eWrite))
            {
                m_EvType &= ~(eWrite | eAdoptiveWriteTimeOut);
            }
            else
            {
                m_EvType &= ~(eComplete | eWrite | eAdoptiveWriteTimeOut);
            }
            if  (!m_EvType)
            {
                m_ev.Reset();
            }
            Locker_UnLock;
            return m_wlen;
        }
#ifdef SAFE_GLOCKS
        if  (IsError_locked(Locker))
#else
        if  (IsError_locked())
#endif
        {
            Locker_UnLock;
            //printf("err -1/2\n");
            return -1;
        }
        m_ev.Reset();
        Locker_UnLock;
    }
    //printf("err -1/3\n");
    return -1;
}

//---------------------------------------------------------------------------
// Servers
//---------------------------------------------------------------------------
CMaaTcpFileServer::CMaaTcpFileServer(CMaaFdSockets * pFdSockets, int Port, _IP6 Ip, CMaaTcpFile * pHandler, bool bReuse, const char * ServerName)
:   CMaaTcpSocket(pFdSockets),
    m_pHandler(pHandler),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0),
    m_Timer1(this, 1),
    m_Timer2(this, 2),
    m_Timer3(this, 3),
    m_InterruptTimer(this, 0x8)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    bool b;
    _IP Ip4 = Ip.v4(&b);
    if  (!b)
    {
        int domain = AF_INET6;
        SOCKET s = socket(domain, SOCK_STREAM, IPPROTO_TCP);
        SwapSockets(s, domain);
        closesocket(s);
    }
    //__utf8_printf("CMaaTcpFileServer::CMaaTcpFileServer(Port=%d, Ip=%I)\n", Port, Ip);
    m_Mode = 0;
    m_Accepting = 0;

    m_AcceptedSocket = INVALID_SOCKET;
    m_IpFrom = 0;
    m_PortFrom = 0;

    m_pAcceptedSocket = nullptr;
    m_pIpFrom = nullptr;
    m_pPortFrom = nullptr;

    ServerName = ServerName ? ServerName : "CMaaTcpFileServer";
    m_ServerName.Format("%s on port %d", ServerName, Port);

    m_Timer0.Attach(pFdSockets);
    m_Timer1.Attach(pFdSockets);
    m_Timer2.Attach(pFdSockets);
    m_Timer3.Attach(pFdSockets);
    m_InterruptTimer.Attach(pFdSockets);

    //CMaaFile f("log_1.txt", CMaaFile::eAC_SrSw, eNoExcept);
    //f.fprintf("Bind(%I:%d)\n", Ip, Port); f.Flush();
    //__utf8_printf("%d: Bind(%I:%d)\n", (int)GetSocket(), Ip, Port);
    if  (!b)
    {
        Bind6(Port, Ip.ptr(), bReuse);
    }
    else
    {
        Bind(Port, Ip4, bReuse);
    }
    //f.fprintf("Listen()\n"); f.Flush();
    //__utf8_printf("%d: Listen()\n", (int)GetSocket());
    Listen();
    //__utf8_printf("%d: ...\n", (int)GetSocket());
    //f.fprintf("\n"); f.Flush();
    //f.Close();
    AddFdSocket();
    //ChangeFdMode(eDisableRead); // orig - local ftp client PORT connect to srv err
    //ChangeFdMode(eRead);
    m_Timer2.Start(1);
}
//---------------------------------------------------------------------------
void CMaaTcpFileServer::EnableInterruptEvent()
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler && m_pHandler->m_pInterruptEvent)
    {
        //_qword Period = m_pHandler->m_InterruptCheckingTime * 1000;
        int Period = m_pHandler->m_InterruptCheckingTime * 1000;
        if  (!m_InterruptTimer.IsStarted() || m_InterruptTimer.GetPeriod() != Period)
        {
            m_InterruptTimer.Start(Period);
        }
    }
    else
    {
        m_InterruptTimer.Stop();
    }
    Locker_UnLock;
}
//---------------------------------------------------------------------------
CMaaTcpFileServer::~CMaaTcpFileServer()
{
    {
        sAcceptedSocketsLink * pl;
        while((pl = m_AcceptedSocketsList.GetFromFront()))
        {
            closesocket(pl->s);
            delete pl;
        }
    }

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        m_pHandler->m_EvType |= CMaaTcpFile::eDestroyed;
        m_pHandler->m_pServer = nullptr;
        m_pHandler->m_ev._Set;
        m_pHandler = nullptr;
    }
    Locker_UnLock;

    RemoveFdSocket();
}
//---------------------------------------------------------------------------
int CMaaTcpFileServer::Notify_Read()
{
    //__utf8_printf("CMaaTcpFileServer::Notify_Read()\n");

    if  (m_pAcceptedSocket && m_Accepting)// && m_pIpFrom && m_pPortFrom)
    {
        m_AcceptedSocket = INVALID_SOCKET;
        m_IpFrom = 0;
        m_PortFrom = 0;
        m_IpFrom6.Zero();

        sAcceptedSocketsLink * pl = m_AcceptedSocketsList.GetFromFront();
        if  (pl)
        {
            m_AcceptedSocket = pl->s;
            m_IpFrom = pl->ip;
            m_IpFrom6 = pl->ip6;
            m_PortFrom = pl->port;
            delete pl;
        }
        else
        {
            //__utf8_printf("CMaaTcpFileServer::Notify_Read(): Accept()\n");
            if  (GetDomainSock() == AF_INET6)
            {
                m_AcceptedSocket = Accept6(m_IpFrom6.ptr(), &m_PortFrom);
                bool b4;
                m_IpFrom = m_IpFrom6.v4(&b4);
                if  (!b4)
                {
                    m_IpFrom = 0;
                }
            }
            else
            {
                m_AcceptedSocket = Accept(&m_IpFrom, &m_PortFrom);
                _IP6 tmp(m_IpFrom);
                m_IpFrom6 = tmp;
            }
            //__utf8_printf("CMaaTcpFileServer::Notify_Read(): Accept() ret = %d\n", (int)m_AcceptedSocket);

            if  (m_AcceptedSocket == INVALID_SOCKET)
            {
                return 0;
            }
        }
        //m_Accepting = 0;
        Completed();
    }
    else
    {
        sAcceptedSocketsLink * pl = TL_NEW sAcceptedSocketsLink;
        if  (pl)
        {
            //__utf8_printf("CMaaTcpFileServer::Notify_Read(): Accept() (2)\n");
            if  (GetDomainSock() == AF_INET6)
            {
                pl->s = Accept6(pl->ip6.ptr(), &pl->port);
                bool b4;
                pl->ip = pl->ip6.v4(&b4);
                if  (!b4)
                {
                    pl->ip = 0;
                }
            }
            else
            {
                pl->s = Accept(&pl->ip, &pl->port);
                _IP6 tmp(pl->ip);
                pl->ip6 = tmp;
            }
            //__utf8_printf("CMaaTcpFileServer::Notify_Read(): Accept() ret = %d\n", (int)pl->s);
            m_AcceptedSocketsList.AddAtBack(pl);
        }
        else
        {
            closesocket(GetDomainSock() == AF_INET6 ? Accept6() : Accept());
        }
        if  (m_AcceptedSocketsList.GetItemCount() >= 101)
        {
            pl = m_AcceptedSocketsList.GetFromFront();
            closesocket(pl->s);
            delete pl;
        }
        //closesocket(s);
    }
    //CheckInterruptEvent(eRead);

    return 0;//eDisableRead;
}
//---------------------------------------------------------------------------
int CMaaTcpFileServer::Notify_Write()
{
    //__utf8_printf("CMaaTcpFileServer::Notify_Write()\n");
    // server does not perform write operations
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaTcpFileServer::Notify_Error()
{
    XTOOSockErr err("TCPServer:Notify_Error()", nullptr);
    //__utf8_printf("CMaaTcpFileServer::Notify_Error() %s\n", err.GetMsg());
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler)
    {
        delete m_pHandler->m_err;
        m_pHandler->m_err = TL_NEW XTOOSockErr(err);
        m_pHandler->m_EvType |= CMaaTcpFile::eExcept;
        m_pHandler->m_ev._Set;
    }
    Locker_UnLock;
    return CloseByException("Some error in listened socket...\n");
}
//---------------------------------------------------------------------------
CMaaString CMaaTcpFileServer::GetConnectionName() noexcept
{
    return m_ServerName;
}
//---------------------------------------------------------------------------
void CMaaTcpFileServer::CheckInterruptEvent(int /*From*/)
{
    bool bInterrupted = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_pHandler && m_pHandler->m_pInterruptEvent)
    {
        if  (m_pHandler->m_pInterruptEvent->Wait((_dword)0))
        {
            XTOOSockErr err("CMaaTcpFileServer: Interrupt event occured", nullptr, nullptr, 0, -1);
            {
                delete m_pHandler->m_err;
                m_pHandler->m_err = TL_NEW XTOOSockErr(err);
                m_pHandler->m_EvType |= CMaaTcpFile::eExcept;
                m_pHandler->m_ev._Set;
            }
            bInterrupted = true;
        }
    }
    Locker_UnLock;
    if  (bInterrupted)
    {
        CloseByException("Interrupt event");
    }
}
//---------------------------------------------------------------------------
void CMaaTcpFileServer::OnTimer(int f)
{
    switch(f)
    {
    case 0:
        CloseByException("OnTimer(0)");
        break; // CMaa_fallthrough;
    case 1:
        {
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            m_Timer1.Stop();
            int Mode = m_Mode;
            Locker_UnLock;
            if  (Mode & CMaaTcpFile::eRead)
            {
                m_AcceptedSocket = INVALID_SOCKET;
                m_IpFrom = 0;
                m_PortFrom = 0;
                m_IpFrom6.Zero();
                if  (m_AcceptTimeOutMs < (_dword)-2)
                {
                    m_Timer3.Start(m_AcceptTimeOutMs * 1000);
                }
                m_Accepting = 1;
                ChangeFdMode(eRead);
                if  (m_pAcceptedSocket && m_Accepting && m_AcceptedSocketsList.LookAtFront())
                {
                    Notify_Read();
                }
            }
        }
        break;
    case 2:
        {
            m_Timer2.Stop();
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            if  (m_pHandler)
            {
                m_pHandler->m_EvType |= CMaaTcpFile::eConnected;
                m_pHandler->m_ev._Set;
            }
            Locker_UnLock;
            //ChangeFdMode(eDisableRead|eDisableWrite|eExcept);
        }
        break;
    case 3:
        {
            CMaaWin32Locker_Locker_gLock;
            Locker_Lock;
            m_Timer3.Stop();
            int Mode = m_Mode;
            Locker_UnLock;
            if  (Mode & CMaaTcpFile::eRead)
            {
                Completed();
            }
        }
        break;
    case 0x8:
        CheckInterruptEvent();
        break;
    }
}

bool CMaaTcpFileServer::AcceptSocket(SOCKET * s, _IP * IpFrom, _Port * PortFrom, _dword TimeOutMs) // -1 - infinite
{
    bool Ret = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    EnableInterruptEvent();
    if  (!(m_Mode & CMaaTcpFile::eRead) && s)
    {
        m_pAcceptedSocket = s;
        m_pIpFrom = IpFrom;
        m_pIpFrom6 = nullptr;
        m_pPortFrom = PortFrom;
        *m_pAcceptedSocket = INVALID_SOCKET;
        m_Mode |= CMaaTcpFile::eRead;
        m_AcceptTimeOutMs = TimeOutMs;
        m_Timer3.Stop();
        m_Timer1.Start(1);
        Ret = true;
    }
    Locker_UnLock;
    return Ret;
}

bool CMaaTcpFileServer::AcceptSocket6(SOCKET * s, _byte * IpFrom6, _Port * PortFrom, _dword TimeOutMs) // -1 - infinite
{
    bool Ret = false;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    EnableInterruptEvent();
    if  (!(m_Mode & CMaaTcpFile::eRead) && s)
    {
        m_pAcceptedSocket = s;
        m_pIpFrom = nullptr;
        m_pIpFrom6 = IpFrom6;
        m_pPortFrom = PortFrom;
        *m_pAcceptedSocket = INVALID_SOCKET;
        m_Mode |= CMaaTcpFile::eRead;
        m_AcceptTimeOutMs = TimeOutMs;
        m_Timer3.Stop();
        m_Timer1.Start(1);
        Ret = true;
    }
    Locker_UnLock;
    return Ret;
}

void CMaaTcpFileServer::Completed()
{
    //int NewMode = 0;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (m_Mode & CMaaTcpFile::eRead)
    {
        //if   (m_AcceptedSocket != INVALID_SOCKET)
        {
            m_Timer3.Stop();
            Locker_Lock;
            if  (m_pAcceptedSocket)
            {
                *m_pAcceptedSocket = m_AcceptedSocket;
                if  (m_pIpFrom)
                {
                    *m_pIpFrom = m_IpFrom;
                }
                //__utf8_printf("A : %J\n", m_IpFrom6.ptr());
                if  (m_pIpFrom6)
                {
                    memcpy(m_pIpFrom6, m_IpFrom6.ptr(), 16);
                    //__utf8_printf("A:  %J\n", m_pIpFrom6);
                }
                if  (m_pPortFrom)
                {
                    *m_pPortFrom = m_PortFrom;
                }
                m_pHandler->m_EvType |= CMaaTcpFile::eRead | CMaaTcpFile::eComplete;
                m_pHandler->m_ev._Set;
            }
            Locker_UnLock;
            m_Mode &= ~CMaaTcpFile::eRead;
            m_Accepting = 0;
            //NewMode |= eDisableRead;
        }
    }
    Locker_UnLock;
    //ChangeFdMode(NewMode);
}
//---------------------------------------------------------------------------


#endif
#endif
