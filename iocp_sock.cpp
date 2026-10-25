
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/iocp_sock.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * sockets (TCP, UDP), IPv4 addresses, shapers, connections and
 * servers templates, perfect timers, htonq/ntohq functions for qword.
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

#ifdef TOOLSLIB_IOCP

static int DebugLevel = 0;

//#define IfDebug(x) if ( x <= DebugLevel )
#define IfDebug(x) if ( false )

#define LOG1(x) //m_C == 1 && log1(x)
#define LOG1_(x) //m_pFdSockets->m_C == 1 && m_pFdSockets->log1(x)
#define DP(x) //x
#define DP1(x) x
#define DP2(x) //x

//extern SOCKET g_____socket;

//int gInt = 0;

//SOCKET gH1 = -1;

//const _IP6 gIp6_0/*(_IP6::eInitByZero)*/, gIp6_1(_IP6::eInitByOne), gIp6_v4Supp(_IP6::eV4Supp);

// Глобальные указатели на расширения Winsock (загружаются динамически)
LPFN_ACCEPTEX gfnAcceptEx = nullptr;
LPFN_CONNECTEX gfnConnectEx = nullptr;

// Вспомогательная функция для загрузки расширений API
bool LoadWinsockExtensions()//SOCKET sock)
{
    SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (sock != INVALID_SOCKET)
    {
        DWORD bytes = 0;
        GUID guidAcceptEx = WSAID_ACCEPTEX;
        const int i1 = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &gfnAcceptEx, sizeof(gfnAcceptEx), &bytes, NULL, NULL);

        GUID guidConnectEx = WSAID_CONNECTEX;
        const int i2 = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), &gfnConnectEx, sizeof(gfnConnectEx), &bytes, NULL, NULL);

        closesocket(sock);
        return !i1 && !i2;
    }
    return false;
}

//---------------------------------------------------------------------------
DEF_ALLOCATOR(CMaaIOCPSockThread::SCmd)
DEF_ALLOCATOR(CMaaIOCPSockThread)
//DEF_ALLOCATOR(CMaaFdSockets)
//---------------------------------------------------------------------------
CMaaIOCPSockThread::CMaaIOCPSockThread(CMaaIOCPSockThread * pParent)
:   m_Refs(0),
    m_WakeUpFlags(0),
    m_pParent(pParent)
{
    if (!pParent)
    {
        m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        LoadWinsockExtensions();
    }
    else
    {
        m_hIOCP = pParent->m_hIOCP;
    }

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    gSockThreads++;
    static int _Id = 0;
    Id = ++_Id;
    Locker_UnLock;
    m_bExiting = false;
    //m_pFdSockets = TL_NEW CMaaFdSockets(this);
    ////m_pFdSockets->SetTimeOut(0, 10 * 1000);
    //m_hWakeup = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_hWakeup = WSACreateEvent();

    //CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if (pParent)
    {
        pParent->m_ChildThreads.AddAtBack(this);
    }
    Locker_UnLock;
}

int CMaaIOCPSockThread::AddRef() noexcept
{
    return ++m_Refs;
}

int CMaaIOCPSockThread::DelRef() noexcept
{
    return --m_Refs;
}

void CMaaIOCPSockThread::AddCommand(int Cmd, CMaaIOCPSockThread * Par1, void* Par2)
{
    AddCommand(TL_NEW SCmd(Cmd, Par1, Par2));
}

void CMaaIOCPSockThread::AddCommand(SCmd * c) noexcept
{
    if (c)
    {
        m_Mutex.LockM();
        m_List.AddAtBack(c);
        m_Mutex.UnLockM();

        WakeUp();
    }
}
CMaaIOCPSockThread::~CMaaIOCPSockThread()
{
    try
    {
        AddCommand(-1);
    }
    catch (...)
    {
    }

    Join();


    //CMaaWin32Locker_Locker_gLock;
    Locker_Lock;

    //CloseHandle(m_hWakeup); m_hWakeup = nullptr;
    WSACloseEvent(m_hWakeup); m_hWakeup = nullptr;

    gSockThreads--;

    if (m_pParent)
    {
        m_pParent->m_ChildThreads.Release(this);
        m_pParent = nullptr;
    }
    Locker_UnLock;

    //try
    //{
//    m_pFdSockets->DeleteAllSockets(); // let sockets be in unlocked mode in ~   (2016)
    //}
    //catch(...)
    //{
    //}

    Locker_Lock; //??

    //delete m_pFdSockets; // sync need?

    Locker_UnLock; //?? //27.03.2025

    for (;;)
    {
        m_Mutex.LockM();
        SCmd* p = m_List.GetFromFront();
        m_Mutex.UnLockM();

        if (!p)
        {
            break;
        }
        delete p;
    }

    //if  (m_pParent)
    //{
        //m_pParent->m_ChildThreads.Release(this);
    //}

    //Locker_UnLock;
}

void CMaaIOCPSockThread::WakeUpChildThreads() noexcept
{
    //CMaaWin32Locker_Locker_gLock;
    Locker_Lock;

    for (CMaaIOCPSockThread* t = m_ChildThreads.LookAtFront(); t; t = m_ChildThreads.Next(t))
    {
        t->WakeUp();
    }

    Locker_UnLock;
}

void CMaaIOCPSockThread::SetExitChildThreads() noexcept
{
    //CMaaWin32Locker_Locker_gLock;
    Locker_Lock;

    try
    {
        for (CMaaIOCPSockThread* t = m_ChildThreads.LookAtFront(); t; t = m_ChildThreads.Next(t))
        {
            //t->AddCommand(-1);
            t->Exit(0);
        }
    }
    catch (...)
    {
    }

    Locker_UnLock;
}

//void CMaaIOCPSockThread::RemoveWakeUpPair() noexcept
//{
//}
void CMaaIOCPSockThread::WakeUp(unsigned char flags) noexcept
{
    m_WakeUpFlags |= flags;
    //SetEvent(m_hWakeup);
    WSASetEvent(m_hWakeup);
}

unsigned CMaaIOCPSockThread::Run()
{
    SetThreadName(-1, "CMaaIOCPSockThread");
    int Ret = 0;

    try
    {
        Process();
    }
    catch (const char* Msg)
    {
        //printf("catch(\"%s\");\n", Msg);
        XTOOWsaErr  WsaErr;
        XTOOErrno   Errno;
        XTOOSockErr ErrMsg(Msg, nullptr);
        Ret = 1;
    }
    catch (XTOOError&/*x*/)
    {
        //printf("catch(XTOOError &);\n", x.GetMsg());
        XTOOErrMsg  ErrMsg("\n");
        Ret = 2;
    }
    catch (eExitThread&)
    {
        //printf("catch(eExitThread &);\n");
        //printf("catch(eExitThread &)\r\n");
        Ret = 4;
    }
    catch (...)
    {
        //printf("catch(...)\r\n");
        try
        {
            XTOOWsaErr  WsaErr;
            XTOOErrno   Errno;
            XTOOErrMsg  ErrMsg("\nUnknown error.");
        }
        catch (...)
        {
            char* p = strerror(errno);
            printf("\nUnknown error. Errno = %d %s\n", errno, p ? p : ""); //!!! printf
        }
        Ret = 3;
    }
    /*--------------------------------*/
    //m_pFdSockets->DeleteAllSockets();
    /*--------------------------------*/
    if (m_pParent)
    {
        m_pParent->AddCommand(1, this);
    }
    return (unsigned)Ret;
}

CMaaIOCPSockThread* CMaaIOCPSockThread::CreateNewThread()
{
    return TL_NEW CMaaIOCPSockThread(this);
}

void CMaaIOCPSockThread::Process()
{
    if (!m_pParent)
    {
        const int N = 2 * CMaaGetHardwareConcurrency(1, 64);
        for (int i = 0; i < N; i++)
        {
            CMaaIOCPSockThread * t = CreateNewThread();
            t && t->Create();
        }
    }

    while (Process1())
    {
    }
}

void CMaaIOCPSockThread::OnExitCommand(SCmd*) noexcept
{
    m_bExiting = true;
    //m_pFdSockets->DeleteAllSockets();
}

bool CMaaIOCPSockThread::Process1()
{
    for (;;)
    {
        //CMaaWin32Locker_Locker_gLock;
        for (;;)
        {
            //Locker_Lock;
            m_Mutex.LockM();
            SCmd* c = m_List.GetFromFront();
            m_Mutex.UnLockM();
            //Locker_UnLock;
            //printf("Got command: %p\r\n", c); fflush(stdout);

            if (!c)
            {
                break;
            }
            //printf("Got command: %d, %p, %p\r\n", c->Cmd, c->m_Par1, c->m_Par2); fflush(stdout);

            switch (c->Cmd)
            {
            case 1:
                //printf("c->Cmd == 1 - Join()\r\n");fflush(stdout);//P
                c->m_Par1->Join();
                //__utf8_printf("c->Cmd == 1 - Join() - ok\r\n");//fflush(stdout);
                delete c->m_Par1;
                //printf("c->Cmd == 1 - delete - ok\r\n");fflush(stdout);//P
                break;
            case -1:
                //printf("OnExitCommand();\n");
                OnExitCommand(c);
            }
            delete c;
        }

        if (!m_pParent)
        {
            //printf("000: point\r\n");fflush(stdout);//P
        }
        else
        {
            //printf("111: point\r\n");fflush(stdout);//P
        }

        //if (m_ChildThreads.IsNotEmpty() && m_pFdSockets->GetSockCount() <= 0)
        {
            //Log("if (m_ChildThreads.IsNotEmpty() && m_pFdSockets->GetSockCount() <= 0) { }");
        }
        Locker_Lock;
        if (m_Refs <= 0 && m_ChildThreads.IsEmpty())
        {
            Locker_UnLock;
            if (m_bExiting)
            {
                break;
            }
            /*
            if (m_pFdSockets->GetSockCount() <= 0)
            {
                //printf("Point n3\n");
                //if (m_pParent)
                {
                    break;
                }
            }
            */
            //if (m_pParent || gSockThreads <= 1)
            {
                CheckPoint(100);
            }
        }
        else
        {
            Locker_UnLock;
            if (!m_pParent)
            {
                static bool b = true;
                if (b)
                {
                    try
                    {
                        CheckPoint(100);
                    }
                    catch (...)
                    {
                        b = false;
                        SetExitChildThreads();
                    }
                }
                else
                {
                    Sleep(10);
                }
            }
            else
            {
                CheckPoint(100);
            }
        }

        /*
        if (m_pFdSockets->Select() < 0)
        {
            //2022
            m_Mutex.LockM();
            const bool b_c = m_List.LookAtFront() ? true : false;
            m_Mutex.UnLockM();
            Locker_Lock;
            const bool b_t = m_ChildThreads.LookAtFront() ? true : false;
            Locker_UnLock;
            if (b_c)
            {
                return true;
            }
            if (b_t)
            {
#ifdef _WIN32
                if (!SwitchToThread())
                {
                    Sleep(1);
                }
#else
                if (sched_yield())
                {
                    usleep(1000);
                }
#endif
                return true;
            }
            CMaaSocket::StaticError("Select()");
            //Console.Error("select()");
            break;
        }
        */
        //m_pFdSockets->PreTimer();
        return true;
    }
    //printf("Point n5\n");
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CMaaIOCPSocket::CMaaIOCPSocket(/*CMaaFdSockets* pFdSockets,*/ int af, int type, int protocol)
//    : CMaaFdSocket(pFdSockets)
{
    /*m_ModeChangeItem.m_pItem =*/ m_ListItem.m_pItem = this;
    if ((m_Socket = WSASocket(m_domain = af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        Error("socket()");
    }
}
//---------------------------------------------------------------------------
CMaaIOCPSocket::~CMaaIOCPSocket()
{
    //m_TcpSocketFlag = false;
    //RemoveFdSocket(true);
    if (m_Socket != INVALID_SOCKET)
    {
        //shutdown(m_Socket, 0);
        closesocket(m_Socket);
    }
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::Error(const char* msg)
{
    throw XTOOSockErr(msg, nullptr, GetConnectionName());
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::StaticError(const char* msg)
{
    throw XTOOSockErr(msg, nullptr, nullptr);
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::GetDomainSock() const noexcept
{
    return m_domain;
}
//---------------------------------------------------------------------------
SOCKET CMaaIOCPSocket::GetSocket() const noexcept
{
    return m_Socket;
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::SetSocket(SOCKET s, int domain) noexcept
{
    //ChangeFdModeEx(eDisableAll);
    if (m_Socket != INVALID_SOCKET)
    {
        closesocket(m_Socket);
    }
    m_Socket = s;
    if (domain != -1)
    {
        m_domain = domain;
    }
#ifdef _WIN32
    //m_LastMode = sNegResetMode;
#endif
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::SwapSockets(SOCKET & s, int& d) noexcept // domain
{
    SOCKET s1 = m_Socket;
    m_Socket = s;
    s = s1;

    int d1 = m_domain;
    m_domain = d;
    d = d1;

#ifdef _WIN32
    //m_LastMode = sNegResetMode;
#endif
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::CloseByException(const char* msg)
{
    //printf("CMaaIOCPSocket::CloseByExeption('%s') %s\n", msg, (const char *)GetConnectionName()); fflush(stdout);

    Notify_PreClosingByException(msg);
    // ending connection
    //DeleteSocket Err ( msg, this );
    //throw Err;
    throw XTOOSockErr(-1, msg, this, GetConnectionName());
    return 0;// eDisableAll;
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_PreDeletingByException()
{
    return 0;
}
//---------------------------------------------------------------------------
/*
void CMaaIOCPSocket::AddFdSocket()
{
    //m_pFdSockets->AddSocket(this);
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::RemoveFdSocket(bool bFromDestructor) noexcept
{
    //m_pFdSockets->RemoveSocket(this, bFromDestructor);
}
*/
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Read()
{
    return 0;// eDisableRead;
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::Notify_Act() noexcept
{
    //ChangeFdModeEx(0);
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Write()
{
    return 0;// eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Connected(_IP /*Ip*/, _Port /*Port*/, const char* /*DnsName*/)
{
    return 0;// eAll;
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Connected6(_byte* /*Ip6*/, _Port /*Port*/, const char* /*DnsName*/)
{
    return 0;// eAll;
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Error()
{
    return CloseByException("Some socket error occured");
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Accepted(_IP /*IpFrom*/, _Port /*Port*/)
{
    return 0;
}
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Notify_Accepted6(_byte* /*Ip6From*/, _Port /*Port*/)
{
    return 0;
}
//---------------------------------------------------------------------------
CMaaString CMaaIOCPSocket::GetConnectionName() noexcept
{
    return CMaaStringZ;
}
//---------------------------------------------------------------------------
_IP CMaaIOCPSocket::GetHostByName(const char* DnsName, _Port * pPort, _IP * pAllIPs, int* pAllIPsCount)
{
    int xAI = 10;
    if (!pAllIPsCount)
    {
        pAllIPsCount = &xAI;
    }
    if (!pAllIPs)
    {
        *pAllIPsCount = 0;
    }
    _IP Ip = 0;
    CMaaString txt(DnsName);
    int x = CMaaIpToLongEx(txt, &Ip, ":");
    if (x >= 0)
    {
        if (txt[x] == ':' && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
        }
        if (*pAllIPsCount > 1)
        {
            pAllIPs[1] = 0;
        }
        if (*pAllIPsCount > 0)
        {
            pAllIPs[0] = Ip;
            *pAllIPsCount = 1;
        }
    }
    else
    {
        x = (warning_int)txt.ReverseFind(':');
        if (x >= 0 && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
            txt = txt.Left(x);
        }
        // It is not numerical address as name given
        struct hostent* h = gethostbyname(txt);
        if (!h)
        {
            txt.Format("gethostbyname(\"%S\")", &txt);
            throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
            //Error ( txt );
            return 0;
        }
        Ip = ntohl(*(_IP*)h->h_addr_list[0]);
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            pAllIPs[i] = ntohl(*(_IP*)h->h_addr_list[i]);
        }
        if (i < *pAllIPsCount)
        {
            pAllIPs[i] = 0;
        }
        *pAllIPsCount = i;
    }
    return Ip;
}
//---------------------------------------------------------------------------
char* CMaaIOCPSocket::GetHostByAddr(_IP Ip, char* DnsName)
{
    _IP ip = htonl(Ip);
    struct hostent* h = gethostbyaddr((char*)&ip, sizeof(ip), AF_INET);
    if (!h)
    {
        CMaaString txt;
        txt.Format("gethostbyaddrname (\"%I\")", Ip);
        throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
        //Error ( txt );
        return 0;
    }
    strcpy(DnsName, h->h_name);
    return DnsName;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
_byte* CMaaIOCPSocket::GetHostByName6(_byte * RetIp, const char* DnsName, _Port * pPort, _byte * *pAllIPs, int* pAllIPsCount)
{
    int xAI = 10;
    if (!pAllIPsCount)
    {
        pAllIPsCount = &xAI;
    }
    if (!pAllIPs)
    {
        *pAllIPsCount = 0;
    }
    CMaaString txt(DnsName);
    _byte Ip[16];
    memset(Ip, 0, sizeof(Ip));
    int x = CMaaIpToLongEx(txt, Ip, ":");
    if (x >= 0)
    {
        if (txt[x] == ':' && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
        }
        if (*pAllIPsCount > 1)
        {
            memset(pAllIPs[1], 0, 16);
        }
        if (*pAllIPsCount > 0)
        {
            memcpy(pAllIPs[0], Ip, 16);
            *pAllIPsCount = 1;
        }
    }
    else
    {
        x = (warning_int)txt.ReverseFind(':');
        if (x >= 0 && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
            txt = txt.Left(x);
        }
#ifdef _WIN32
        CMaaString buffer;
        CMaaHe1* h = CMaa_gethostbyname2(txt, buffer);
        if (!h)
        {
            txt.Format("ipv6 CMaa_gethostbyname2(\"%S\")", &txt);
            throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
            //Error ( txt );
            return 0;
        }
        memcpy(Ip, h->h_addr_list[0], 16);
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            memcpy(pAllIPs[i], h->h_addr_list[i], 16);
        }
        if (i < *pAllIPsCount)
        {
            memset(pAllIPs[i], 0, 16);
        }
        *pAllIPsCount = i;
#else
        // It is not numerical address as name given
        struct hostent* h = gethostbyname2(txt, AF_INET6);
        if (!h)
        {
            txt.Format("ipv6 gethostbyname2(\"%S\")", &txt);
            throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
            //Error ( txt );
            return 0;
        }
        //int h_length; // Length of address.
        memcpy(Ip, h->h_addr_list[0], 16);
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            memcpy(pAllIPs[i], h->h_addr_list[i], 16);
        }
        if (i < *pAllIPsCount)
        {
            memset(pAllIPs[i], 0, 16);
        }
        *pAllIPsCount = i;
#endif
    }
    memcpy(RetIp, Ip, 16);
    return RetIp;
}
//---------------------------------------------------------------------------
char* CMaaIOCPSocket::GetHostByAddr6(_byte * Ip6, char* DnsName)
{
    _byte Ip[16];
    memcpy(Ip, Ip6, 16);
    struct hostent* h = gethostbyaddr((char*)Ip, sizeof(Ip), AF_INET6);
    if (!h)
    {
        CMaaString txt;
        txt.Format("gethostbyaddrname (\"%J\")", Ip6);
        throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
        //Error ( txt );
        return 0;
    }
    strcpy(DnsName, h->h_name);
    return DnsName;
}
#endif // #ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaIOCPSocket::Close()
{
    //shutdown(m_Socket, 0);
    throw XTOOSockClose(this);
    //return CloseByException("Closing connection");
}
//---------------------------------------------------------------------------

int gIOCPDisableNagleAlgoritm = -1;
//---------------------------------------------------------------------------
CMaaIOCPTcpSocket::CMaaIOCPTcpSocket(/*CMaaFdSockets* pFdSockets,*/ int domain) // AF_INET
    : CMaaIOCPSocket(/*pFdSockets,*/ domain, SOCK_STREAM, IPPROTO_TCP)
{
    //m_TcpSocketFlag = true;

    InitCounters(false);
    memset(&m_s, 0, sizeof(m_s));

    // Variable to set the SO_KEEPALIVE option with setsockopt ().
    BOOL fKeepAlive = 1;//TRUE;

    if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&fKeepAlive, sizeof(BOOL))) // == SOCKET_ERROR)
    {
        Error("setsockopt ()");
    }

    if (gIOCPDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gIOCPDisableNagleAlgoritm != 0);
    }

    /*
     char txt[1024];
     sprintf(txt, "CMaaIOCPTcpSocket::CMaaIOCPTcpSocket(): this = %p\n", this);
     ExtLog(txt);
    */
}
CMaaIOCPTcpSocket::~CMaaIOCPTcpSocket()
{
}
//---------------------------------------------------------------------------
bool CMaaIOCPTcpSocket::DisableNagleAlgoritm(bool bDisable, SOCKET s) const noexcept
{
#ifdef __unix__
    int x = bDisable ? 1 : 0;
#else
    BOOL x = bDisable ? TRUE : FALSE;
#endif
    struct linger chLinger = { 1, 5 }; // Включить linger, таймаут 5 секунд на очистку буферов

    return !setsockopt(s != INVALID_SOCKET ? s : m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&x, sizeof(x)) &&
        !setsockopt(s != INVALID_SOCKET ? s : m_Socket, SOL_SOCKET, SO_LINGER, (const char*)&chLinger, sizeof(chLinger));
}
void CMaaIOCPTcpSocket::InitCounters(bool bConnected)
{
    m_BytesSend = m_BytesRecv = m_BytesSend0 = m_BytesRecv0 = 0;
    m_SendSpeed = m_RecvSpeed = bConnected ? 0 : -1;
}
//---------------------------------------------------------------------------
void CMaaIOCPTcpSocket::Timer1sSpeed()
{
    m_SendSpeed = m_BytesSend != (_qword)0 ? (int)(m_BytesSend - m_BytesSend0) : m_SendSpeed;
    m_BytesSend0 = m_BytesSend;

    m_RecvSpeed = m_BytesRecv != (_qword)0 ? (int)(m_BytesRecv - m_BytesRecv0) : m_RecvSpeed;
    m_BytesRecv0 = m_BytesRecv;
}
//---------------------------------------------------------------------------
void CMaaIOCPTcpSocket::GetStatistics(_qword * pSentBytes, _qword * pRecvBytes, int* pSendSpeed, int* pRecvSpeed, bool bShapersSpeed) const noexcept
{
    if (pSentBytes)
    {
        *pSentBytes = m_BytesSend;
    }
    if (pRecvBytes)
    {
        *pRecvBytes = m_BytesRecv;
    }
    if (pSendSpeed)
    {
        *pSendSpeed = m_SendSpeed;
    }
    if (pRecvSpeed)
    {
        *pRecvSpeed = m_RecvSpeed;
    }
    if (bShapersSpeed)
    {
        if (pSendSpeed)
        {
            *pSendSpeed = 0;
        }
        if (pRecvSpeed)
        {
            *pRecvSpeed = 0;
        }
    }
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::SendData(const void* Buffer, int Size)
{
    int Total = 0;
    int ret = 0;

    int rr;
    while (Size > 0)
    {
        rr = -1;
        const int req = rr > 0 ? rr : Size;
#ifdef TOOLSLIB_IO_PROFILE
        if (m_domain == AF_INET)
        {
            ++g_nTcp4SendIO;
        }
        else
        {
            ++g_nTcp6SendIO;
        }
#endif
        ret = send(m_Socket, (char*)Buffer, req, 0);
#ifdef _WIN32
        const DWORD dw =
#ifdef _WIN32
            //m_dwLastSendErr =
#endif
            ret < 0 ? WSAGetLastError() : 0;
#endif
        if (ret < 0) // == SOCKET_ERROR)
        {
            if (dw != WSAEWOULDBLOCK)
            {
                CloseByException("send()");
            }
            // 27.07.2012 one cycle, break
            Total = Total > 0 ? Total : -1;
            // 27.07.2012

            break;
        }
        else
        {
            //UpdateHrTime(e_fds_last_sent_time);
            Total += ret;
            Size -= ret;
            Buffer = ret + (char*)Buffer;
            if (m_BytesSend == (_qword)-1)
            {
                m_BytesSend = 0;
            }
            m_BytesSend += ret;
        }
        // 12.12.2016-2020
#ifndef TL_EPOLLET_MULTISEND
        break; // one portion read only
#else
        if (ret < req)
        {
            break; // egde
        }
#endif
    }
    //int Mode;
    //if   ((Total > 0 || Total <= -1) && !m_pFdSockets->m_SockHash.Find(this, &Mode))
    //if  ((Total > 0 || Total <= -1) && !m_pFdSockets->SockHashFind(this, &Mode))
    if ((Total > 0 || Total <= -1))
    {
        /*
        Mode = m_pFdSockets->SockHashFindMode(this);
        if (!(m_RealState & ~eWrite))
            //if (Total != -1)
        {
            Mode &= ~eEventWrite;
            //m_pFdSockets->m_SockHash.AddOver(this, Mode);
            m_pFdSockets->SockHashAddOver(this, Mode);
        }
        */
        Total = Total > 0 ? Total : 0;
    }
    //printf("SendData() for %d: Total = %d", m_Socket, Total); fflush(stdout);
    return Total >= -1 ? Total : -1;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::RecvData(void* Buffer, int Size)
{
    int Total = 0, Total2 = 0;
    int ret = 0;

    bool bRAStateReset = false;

    int rr;
    while (Size > 0)
    {
        rr = -1;
        if (!rr)
        {
            break;
        }
        //printf("recv(%d, len=%d, rr=%d, Size=%d)...\n", m_Socket, rr > 0 ? rr : Size, rr, Size); fflush(stdout);
        const int req = rr > 0 ? rr : Size;
        {
#ifdef TOOLSLIB_IO_PROFILE
            if (m_domain == AF_INET)
            {
                ++g_nTcp4RecvIO;
            }
            else
            {
                ++g_nTcp6RecvIO;
            }
#endif
            ret = recv(m_Socket, (char*)Buffer, req, 0);
        }
#ifdef _WIN32
        const DWORD e = ret < 0 ? WSAGetLastError() : 0;
        if (ret >= 0 && ret < req)
        {
            //m_RealState &= ~eRead;
            bRAStateReset = true;
        }
        //m_pFdSockets->ResetRAState(this, eRead);
        bRAStateReset = true;
#endif
        if (ret < 0) // == SOCKET_ERROR)
        {
#ifdef _WIN32
            if (e == WSAEWOULDBLOCK)
            {
                //m_RealState &= ~eRead;
                //m_pFdSockets->ResetRAState(this, eRead);
                bRAStateReset = true;
                break;
            }
#endif

#ifdef TL_EPOLLET_MULTIREAD
            if (Total <= 0)
#endif
                //if   ( WSAGetLastError () != WSAEWOULDBLOCK )
            {
                CloseByException("recv()");
                //vAsyncMsg ( m_Socket, FD_READ, err );
            }
            break;
        }
        else if (!ret)
        {
            Total2 = -1; // on close
            //printf("[%d] ret=0 (Total=%d) --> Total2 = -1; // on close\n", m_Socket, Total);
            break;
        }
        else
        {
            //UpdateHrTime(e_fds_last_receive_time);
            Total += ret;
            Size -= ret;
            Buffer = ret + (char*)Buffer;
            if (m_BytesRecv == (_qword)-1)
            {
                m_BytesRecv = 0;
            }
            m_BytesRecv += ret;
        }
        if (ret > 0)
        {
        }
        //12.12.2016-2020
#ifndef TL_EPOLLET
        break;
#endif
#ifdef TL_EPOLLET_MULTIREAD
        if (ret >= 0 && ret < req)
#endif
        {
            break;
        }
    }
    //int Mode;
    if (Total > 0)
    {
        //Mode = m_pFdSockets->SockHashFindMode(this);
        const int l = bRAStateReset ? -1 : 1;
        //printf("recv(%d) returns %d\n", m_Socket, l); fflush(stdout);
        if (l >= 0)
        {
        }
        else
        {
            //         Mode &= ~eEventRead;
        }
        //m_pFdSockets->SockHashAddOver(this, Mode);
    }


    //printf("RecvData for %d: Total = %d\n", m_Socket, Total); fflush(stdout);

    if (Total2 < 0 && Total >= 0)
    {
        if (Total == 0)
        {
            Total = -1;
        }
    }
    /*if (bCompatibleRead && Total < 0)
    {
        Total = 0;
    }*/
    return Total;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::ConnectAgain(int fCallNotify)
{
    return 0;
}
#if 0
int CMaaIOCPTcpSocket::ConnectAgain(int fCallNotify)
{
    // printf("%p: CMaaIOCPTcpSocket::ConnectAgain (%d)\n", this, fCallNotify);

    IfDebug(200) { DP(printf("CMaaIOCPTcpSocket::ConnectAgain ()\n");) }
    const int domain = GetDomainSock();
    const _IP Ip = htonl(m_s.m_sin.sin_addr.s_addr);
    _byte* Ip6 = (_byte*)m_s.m_sin6.sin6_addr.s6_addr;
    const _Port Port = domain == AF_INET6 ? htons(m_s.m_sin6.sin6_port) : htons(m_s.m_sin.sin_port);
    char pip[256];
#ifdef _WIN32
    if (fCallNotify == 100)
    {
        ChangeFdModeEx(eDisableConnect);
#ifdef __unix__
        //ChangeFdModeEx(eConnect | eExept);
        SetNonBlockingMode();
#endif
        UpdateHrTime(e_fds_connected_at_time);
        return domain == AF_INET6 ? Notify_Connected6(Ip6, Port, CMaaIpToTextEx(pip, Ip6, 0)) : Notify_Connected(Ip, Port, CMaaIpToTextEx(pip, Ip, 0));
    }
#endif
    int x;
    if ((x = (domain == AF_INET6 ? connect(m_Socket, (struct sockaddr*)&m_s.m_sin6, sizeof(m_s.m_sin6)) : connect(m_Socket, (struct sockaddr*)&m_s.m_sin, sizeof(m_s.m_sin)))))
    {
#ifdef __unix__
#define __CA_ERRNO       errno
#define __CA_EISCONN     EISCONN      // FreeBSD
#define __CA_EINPROGRESS EINPROGRESS  // FreeBSD
#define __CA_EALREADY    EALREADY     // FreeBSD

        //#define __CA_EISCONN     -123456
        //#define __CA_EINPROGRESS 115
        //#define __CA_EALREADY    -123457
#else
#define __CA_ERRNO       WSAGetLastError ()
#define __CA_EINPROGRESS WSAEWOULDBLOCK
#define __CA_EALREADY    WSAEALREADY
#define __CA_EISCONN     WSAEISCONN
#endif
        if (0)
        {
            DP(printf("connect returns %d errno=%d", x, errno);
            XTOOSockErr Err("error=", nullptr);
            fflush(stdout);)
        }
        switch (__CA_ERRNO)
        {
        case __CA_EINPROGRESS:
        case __CA_EALREADY:
            return fCallNotify ? 0 : 2;

        case __CA_EISCONN:
            if (fCallNotify)
            {
                ChangeFdModeEx(eDisableConnect);
#ifdef __unix__
                //printf("SetNonBlockingMode()\n"); fflush(stdout);
                SetNonBlockingMode();
#endif
                UpdateHrTime(e_fds_connected_at_time);
                return domain == AF_INET6 ? Notify_Connected6(Ip6, Port, CMaaIpToTextEx(pip, Ip6, 0)) : Notify_Connected(Ip, Port, CMaaIpToTextEx(pip, Ip, 0));
            }
            return fCallNotify ? 0 : 3;
        default:
        {
            IfDebug(2) { DP(printf("\n\n_____connect returns %d errno=%d", x, errno);) }
            { XTOOSockErr Err("error=", nullptr); }
            IfDebug(2) { DP(printf("_____\n"); fflush(stdout);) }
        }
        }

        CMaaString txt;
        if (domain == AF_INET6)
        {
            txt.Format("connect (%J, %d)", Ip6, Port);
        }
        else
        {
            txt.Format("connect (%I, %d)", Ip, Port);
        }
        Error(txt);
        return fCallNotify ? 0 : 4;
    }
    else
    {
        //printf ( "connect returns %d\n", x );
        //fflush(stdout);
    }
    if (fCallNotify)
    {
        ChangeFdModeEx(eDisableConnect);
#ifdef __unix__
        //ChangeFdModeEx(eConnect | eExept);
        SetNonBlockingMode();
#endif
        UpdateHrTime(e_fds_connected_at_time);
        return domain == AF_INET6 ? Notify_Connected6(Ip6, Port, CMaaIpToTextEx(pip, Ip6, 0)) : Notify_Connected(Ip, Port, CMaaIpToTextEx(pip, Ip, 0));
        // TEST: return 0;
    }
    return 0;
}
#endif
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Connect(_IP Ip, _Port Port)
{
    if (GetDomainSock() == AF_INET6)
    {
        Error("Connect() on IPv6 socket called");
    }
    m_s.m_sin.sin_family = AF_INET;
    m_s.m_sin.sin_port = htons(Port);
    m_s.m_sin.sin_addr.s_addr = htonl(Ip);
    //ChangeFdModeEx(eConnect);
    int Ret = ConnectAgain(0);
    if (Ret >= 3)
    {
        CMaaString txt;
        txt.Format("connect (%I, %d)", Ip, Port);
        Error(txt);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Connect(const char* DnsName, _Port Port)
{
    const _IP Ip = GetHostByName(DnsName, &Port);
    return Connect(Ip, Port);
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Connect(const CMaaString & DnsName, _Port Port)
{
    const _IP Ip = GetHostByName((const char*)DnsName.Str0Copy(), &Port);
    return Connect(Ip, Port);
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::AsyncConnect(_IP Ip, _Port Port)
{
    //ChangeFdModeEx(eConnect | eExept);
    Connect(Ip, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Detect64(const CMaaString & DnsName_Port, CMaaString * pDnsName_Port) noexcept
{
    if (pDnsName_Port)
    {
        *pDnsName_Port = DnsName_Port;
    }
#ifndef TOOLSLIB_USE_IPV6
    return AF_INET;
#else
    {
        _IP6 tmp;
        if (CMaaIpToLongEx46(DnsName_Port, tmp.ptr(), ":") >= 0)
        {
            bool b;
            tmp.v4(&b);
            return b ? AF_INET : AF_INET6;
        }
        else
        {
            _Port Port = 0;
            _IP Ip = 0;
            try
            {
                /*const _byte* Ip6 =*/ GetHostByName6(tmp.ptr(), DnsName_Port, &Port);
                bool b;
                Ip = tmp.v4(&b);
                if (!b)
                {
                    if (pDnsName_Port)
                    {
                        pDnsName_Port->Format("[%J]:%d", tmp.ptr(), (int)Port);
                    }
                    return AF_INET6;
                }
                if (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch (...)
            {
            }
            try
            {
                Ip = GetHostByName(DnsName_Port, &Port);
                if (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch (...)
            {
            }
        }
    }
    return -1;
#endif
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Detect46(const CMaaString & DnsName_Port, CMaaString * pDnsName_Port) noexcept
{
    if (pDnsName_Port)
    {
        *pDnsName_Port = DnsName_Port;
    }
#ifndef TOOLSLIB_USE_IPV6
    return AF_INET;
#else
    {
        _IP6 tmp;
        if (CMaaIpToLongEx46(DnsName_Port, tmp.ptr(), ":") >= 0)
        {
            bool b;
            tmp.v4(&b);
            return b ? AF_INET : AF_INET6;
        }
        else
        {
            _Port Port = 0;
            _IP Ip = 0;
            try
            {
                Ip = GetHostByName(DnsName_Port, &Port);
                if (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch (...)
            {
            }
            try
            {
                /*const _byte* Ip6 =*/ GetHostByName6(tmp.ptr(), DnsName_Port, &Port);
                bool b;
                Ip = tmp.v4(&b);
                if (!b)
                {
                    if (pDnsName_Port)
                    {
                        pDnsName_Port->Format("[%J]:%d", tmp.ptr(), (int)Port);
                    }
                    return AF_INET6;
                }
                if (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch (...)
            {
            }
        }
    }
    return -1;
#endif
}
#ifdef TOOLSLIB_USE_IPV6
int CMaaIOCPTcpSocket::Connect6(const _byte * Ip6, _Port Port)
{
    if (GetDomainSock() != AF_INET6)
    {
        Error("Connect() on not IPv6 socket called");
    }
    m_s.m_sin6.sin6_family = AF_INET6;
    m_s.m_sin6.sin6_port = htons(Port);
    memcpy(m_s.m_sin6.sin6_addr.s6_addr, Ip6, 16);
    //ChangeFdModeEx(eConnect);
    //printf("\n\nMode: %d\n", GetFdMode());
    int Ret = ConnectAgain(0);
    if (Ret >= 3)
    {
        CMaaString txt;
        txt.Format("connect (%J, %d)", Ip6, Port);
        Error(txt);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Connect6(const char* DnsName, _Port Port)
{
    _byte b[16];
    _byte* Ip6 = GetHostByName6(b, DnsName, &Port);
    return Connect6(Ip6, Port);
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Connect6(const CMaaString & DnsName, _Port Port)
{
    _byte b[16];
    _byte* Ip6 = GetHostByName6(b, (const char*)DnsName.Str0Copy(), &Port);
    return Connect6(Ip6, Port);
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::AsyncConnect6(const _byte * Ip6, _Port Port)
{
    //ChangeFdModeEx(eConnect | eExept);
    Connect6(Ip6, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::AsyncConnect6(const char* DnsName, _Port Port)
{
    //ChangeFdModeEx(eConnect | eExept);
    Connect6(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::AsyncConnect6(const CMaaString & DnsName, _Port Port)
{
    //ChangeFdModeEx(eConnect | eExept);
    Connect6(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
#endif //#ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::AsyncConnect(const char* DnsName, _Port Port)
{
    //ChangeFdModeEx(eConnect | eExept);
    Connect(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::AsyncConnect(const CMaaString & DnsName, _Port Port)
{
    //ChangeFdModeEx(eConnect | eExept);
    Connect(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Bind(_Port Port, _IP Ip, bool bReuse)
{
    if (GetDomainSock() == AF_INET6)
    {
        Error("Bind() on IPv6 socket called");
    }
    m_s.m_sin.sin_family = AF_INET;
    m_s.m_sin.sin_port = htons(Port);
    m_s.m_sin.sin_addr.s_addr = htonl(Ip);
    if (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
#define P_PARAM (const char *)
#else
        int On = 1;
#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM & On, sizeof(On));
    }
    if (::bind(m_Socket, (struct sockaddr*)&m_s.m_sin, sizeof(m_s.m_sin))) // == SOCKET_ERROR)
    {
        Error("bind ()");
        return 2;
    }
    socklen_t len = sizeof(m_s.m_sin);
    if (getsockname(m_Socket, (struct sockaddr*)&m_s.m_sin, &len))
    {
        Error("getsockname()");
        return 22;
    }
    //SetAcceptFlag();
    return 0;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
int CMaaIOCPTcpSocket::Bind6(_Port Port, const _byte * Ip6, bool bReuse)
{
    if (GetDomainSock() != AF_INET6)
    {
        Error("Bind() on not IPv6 socket called");
    }

    //__utf8_printf("Bind6(%d, %J, %s)\n", Port, Ip6, bReuse ? "true" : "false");

    m_s.m_sin6.sin6_family = AF_INET6;
    m_s.m_sin6.sin6_port = htons(Port);
    if (Ip6)
    {
        memcpy(m_s.m_sin6.sin6_addr.s6_addr, Ip6, 16);
    }
    else
    {
        memset(m_s.m_sin6.sin6_addr.s6_addr, 0, 16);
    }
    if (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
#define P_PARAM (const char *)
#else
        int On = 1;
#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM & On, sizeof(On));
    }
    if (::bind(m_Socket, (struct sockaddr*)&m_s.m_sin6, sizeof(m_s.m_sin6))) // == SOCKET_ERROR)
    {
        Error("bind ()");
        return 2;
    }
    socklen_t len = sizeof(m_s.m_sin6);
    if (getsockname(m_Socket, (struct sockaddr*)&m_s.m_sin6, &len))
    {
        Error("getsockname()");
        return 22;
    }
    //SetAcceptFlag();
    return 0;
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Bind6(CMaaString strIpPort, bool bReuse)
{
    _byte BindIp[16];
    memset(BindIp, 0, sizeof(BindIp));
    int Port = 0;
    int trailer_pos = CMaaIpToLongEx(strIpPort, BindIp, ":");

    strIpPort = strIpPort.RefMid(trailer_pos > 0 ? trailer_pos + 1 : 1);
    strIpPort.sscanfInt(&Port);

    return Bind6(Port, BindIp, strIpPort.LastChar() == 'r');
}
#endif //#ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Bind(CMaaString strIpPort, bool bReuse)
{
    _IP BindIp = 0;
    int Port = 0;
    int trailer_pos = CMaaIpToLongEx(strIpPort, &BindIp, ":");

    strIpPort = strIpPort.RefMid(trailer_pos > 0 ? trailer_pos + 1 : 1);
    strIpPort.sscanfInt(&Port);
    return Bind(Port, BindIp, strIpPort.LastChar() == 'r');
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::GetBindedPort()
{
    if (GetDomainSock() == AF_INET6)
    {
        socklen_t len = sizeof(m_s.m_sin6);
        if (getsockname(m_Socket, (struct sockaddr*)&m_s.m_sin6, &len))
        {
            Error("getsockname()");
            return 22;
        }
        return ntohs(m_s.m_sin6.sin6_port);
    }

    socklen_t len = sizeof(m_s.m_sin);
    if (getsockname(m_Socket, (struct sockaddr*)&m_s.m_sin, &len))
    {
        Error("getsockname()");
        return 22;
    }
    return ntohs(m_s.m_sin.sin_port);
}
//---------------------------------------------------------------------------
bool CMaaIOCPTcpSocket::GetConnInfo(_IP * pLocalIp, _Port * pLocalPort, _IP * pRemoteIp, _Port * pRemotePort, bool bThrow)
{
    bool Ret = (pLocalIp || pLocalPort || pRemoteIp || pRemotePort);
    struct sockaddr_in s;
    socklen_t len;
    if (pLocalIp || pLocalPort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if (getsockname(m_Socket, (struct sockaddr*)&s, &len))
        {
            if (bThrow)
            {
                Error("getsockname()");
            }
            Ret = false;
        }
        else
        {
            if (pLocalIp)
            {
                *pLocalIp = ntohl(s.sin_addr.s_addr);
            }
            if (pLocalPort)
            {
                *pLocalPort = ntohs(s.sin_port);
            }
        }
    }
    if (pRemoteIp || pRemotePort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if (getpeername(m_Socket, (struct sockaddr*)&s, &len))
        {
            if (bThrow)
            {
                Error("getpeername()");
            }
            Ret = false;
        }
        else
        {
            if (pRemoteIp)
            {
                *pRemoteIp = ntohl(s.sin_addr.s_addr);
            }
            if (pRemotePort)
            {
                *pRemotePort = ntohs(s.sin_port);
            }
        }
    }
    return Ret;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
bool CMaaIOCPTcpSocket::GetConnInfo6(_byte * pLocalIp, _Port * pLocalPort, _byte * pRemoteIp, _Port * pRemotePort, bool bThrow)
{
    bool Ret = (pLocalIp || pLocalPort || pRemoteIp || pRemotePort);
    struct sockaddr_in6 s;
    socklen_t len;
    if (pLocalIp || pLocalPort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if (getsockname(m_Socket, (struct sockaddr*)&s, &len))
        {
            if (bThrow)
            {
                Error("getsockname()");
            }
            Ret = false;
        }
        else
        {
            if (pLocalIp)
            {
                memcpy(pLocalIp, s.sin6_addr.s6_addr, 16);
            }
            if (pLocalPort)
            {
                *pLocalPort = ntohs(s.sin6_port);
            }
        }
    }
    if (pRemoteIp || pRemotePort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if (getpeername(m_Socket, (struct sockaddr*)&s, &len))
        {
            if (bThrow)
            {
                Error("getpeername()");
            }
            Ret = false;
        }
        else
        {
            if (pRemoteIp)
            {
                memcpy(pRemoteIp, s.sin6_addr.s6_addr, 16);
            }
            if (pRemotePort)
            {
                *pRemotePort = ntohs(s.sin6_port);
            }
        }
    }
    return Ret;
}
//---------------------------------------------------------------------------
_byte* CMaaIOCPSocket::GetLocalHostIp6(_byte * RetIp)
{
    char txt[256];
    txt[0] = 0;
    gethostname(txt, sizeof(txt));
#ifdef _WIN32
    CMaaString buffer;
    CMaaHe1* h = CMaa_gethostbyname2(txt, buffer);
    if (!*txt)
    {
        strcpy(txt, "<unknown>");
    }
    if (!h)
    {
        CMaaString msg;
        msg.Format("gethostbyname(\"%s\")", txt);
        throw XTOOSockDnsErr(msg, nullptr, "CMaaIOCPSocket::GetLocalHostIp()");
        //StaticError("gethostbyname() in CMaaIOCPSocket::GetLocalHostIp()");
        //memset(RetIp, 0, sizeof(RetIp);
        return nullptr;//RetIp; // 1? // !!!!!!!!!2017
    }
    /*
     printf("Host name: %s", txt);
     if   ( h -> h_name )
     {
          printf ( " (%s)", h -> h_name );
     }
     printf ( "\n" );
     int i;
     for   ( i = 0; h -> h_aliases [ i ] != nullptr; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != nullptr; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    memcpy(RetIp, h->h_addr_list[0], 16);
#else
    struct hostent* h = gethostbyname2(txt, AF_INET6);
    if (!*txt)
    {
        strcpy(txt, "<unknown>");
    }
    if (!h)
    {
        CMaaString msg;
        msg.Format("gethostbyname(\"%s\")", txt);
        throw XTOOSockDnsErr(msg, nullptr, "CMaaIOCPSocket::GetLocalHostIp()");
        //StaticError("gethostbyname() in CMaaIOCPSocket::GetLocalHostIp ()");
        //memset(RetIp, 0, sizeof(RetIp);
        return nullptr;//RetIp; // 1? // !!!!!!!!!2017
    }
    /*
     printf("Host name: %s", txt);
     if   ( h -> h_name )
     {
          printf ( " (%s)", h -> h_name );
     }
     printf ( "\n" );
     int i;
     for   ( i = 0; h -> h_aliases [ i ] != nullptr; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != nullptr; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    memcpy(RetIp, h->h_addr_list[0], 16);
#endif
    return RetIp;
}
#endif // #ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
_IP CMaaIOCPSocket::GetLocalHostIp()
{
    char txt[256];
    txt[0] = 0;
    gethostname(txt, sizeof(txt));
    struct hostent* h = gethostbyname(txt);
    if (!*txt)
    {
        strcpy(txt, "<unknown>");
    }
    if (!h)
    {
        CMaaString msg;
        msg.Format("gethostbyname(\"%s\")", txt);
        throw XTOOSockDnsErr(msg, nullptr, "CMaaIOCPSocket::GetLocalHostIp()");
        //StaticError("gethostbyname () in CMaaIOCPSocket::GetLocalHostIp()" );
        return 1; // 1?
    }
    /*
     printf("Host name: %s", txt);
     if   ( h -> h_name )
     {
          printf ( " (%s)", h -> h_name );
     }
     printf ( "\n" );
     int i;
     for   ( i = 0; h -> h_aliases [ i ] != nullptr; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != nullptr; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    return htonl(*(_IP*)h->h_addr_list[0]);
}
//---------------------------------------------------------------------------
const char* CMaaIOCPSocket::GetLocalHostName() noexcept
{
    static char txt[1024];
    txt[0] = 0;
    gethostname(txt, sizeof(txt));
    //struct hostent * h = gethostbyname ( txt );
    if (!*txt)
    {
        strcpy(txt, "<unknown>");
    }
    return txt;
}
//---------------------------------------------------------------------------
void CMaaIOCPSocket::SetNonBlockingMode()
{
#ifdef __unix__
    int flags = fcntl(m_Socket, F_GETFL);
    if (flags != -1)
    {
        if (fcntl(m_Socket, F_SETFL, flags | O_NONBLOCK))
        {
            Error("CMaaIOCPSocket::SetNonBlockingMode() fcntl F_SETFL");
            //Error("fcntl F_SETFL O_NONBLOCK failed");
        }
    }
    else
    {
        Error("CMaaIOCPSocket::SetNonBlockingMode() fcntl F_GETFL");
    }
#else
    long cmd = FIONBIO;
    u_long IsNonBlocking = 1;     // zero for disabling
    if (ioctlsocket(m_Socket, cmd, &IsNonBlocking))
    {
        Error("CMaaIOCPSocket::SetNonBlockingMode()");
    }
#endif
}
//---------------------------------------------------------------------------
int CMaaIOCPTcpSocket::Listen(int QueenDepth)
{
    if (listen(m_Socket, QueenDepth)) // == SOCKET_ERROR)
    {
        Error("listen ()");
        return 3;
    }
    return 0;
}
//---------------------------------------------------------------------------
SOCKET CMaaIOCPTcpSocket::Accept(_IP * IpFrom, _Port * Port)
{
    struct sockaddr_in si;
    socklen_t len;
    memset(&si, 0, len = sizeof(si));
    const SOCKET s = accept(GetSocket(), (struct sockaddr*)&si, &len);
    /*
    int Mode;
    if (!m_pFdSockets->SockHashFind(this, &Mode))
    {
        Mode &= ~eEventRead;
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
#ifdef TL_EPOLL
        m_pFdSockets->ResetRAState(this, eRead);
#endif
    }
    */

    if (s == INVALID_SOCKET) // if (s == SOCKET_ERROR) 
    {
        //printf("accept error %d\n", errno);
        Error("accept()");
    }
    if (IpFrom)
    {
        *IpFrom = htonl(si.sin_addr.s_addr);
    }
    if (Port)
    {
        *Port = htons(si.sin_port);
    }

    if (gIOCPDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gIOCPDisableNagleAlgoritm != 0, s);
    }

    return s;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
SOCKET CMaaIOCPTcpSocket::Accept6(_byte * Ip6From, _Port * Port)
{
    struct sockaddr_in6 si;
    socklen_t len;
    memset(&si, 0, len = sizeof(si));
    const SOCKET s = accept(GetSocket(), (struct sockaddr*)&si, &len);
    /*
    int Mode;
    if (!m_pFdSockets->SockHashFind(this, &Mode))
    {
        Mode &= ~eEventRead;
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
#ifdef TL_EPOLL
        m_pFdSockets->ResetRAState(this, eRead);
#endif
    }
    */

    if (s == INVALID_SOCKET) // if (s == SOCKET_ERROR) 
    {
        //printf("accept error %d\n", errno);
        Error("accept()");
    }
    if (Ip6From)
    {
        memcpy(Ip6From, si.sin6_addr.s6_addr, 16);
    }
    if (Port)
    {
        *Port = htons(si.sin6_port);
    }

    if (gIOCPDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gIOCPDisableNagleAlgoritm != 0, s);
    }

    return s;
}
#endif
//---------------------------------------------------------------------------

#endif
