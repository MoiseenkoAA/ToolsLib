
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/iocp_sock.h

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


#ifdef TOOLSLIB_IOCP
class CMaaIOCPSockThread : public CMaaThread
{
public:
    HANDLE m_hIOCP = INVALID_HANDLE_VALUE;

    struct SCmd : public CMaaDLink
    {
        int Cmd;
        CMaaIOCPSockThread* m_Par1;
        void* m_Par2;
        SCmd(int Command, CMaaIOCPSockThread* Par1 = nullptr, void* Par2 = nullptr)  noexcept
        {
            Cmd = Command;
            m_Par1 = Par1;
            m_Par2 = Par2;
        }
        ADD_ALLOCATOR(SCmd)
    };
protected:
    CMaaDList<SCmd> m_List;
    CMaaDList<CMaaIOCPSockThread> m_ChildThreads;
    std::atomic<int> m_Refs;
    bool m_bExiting;
    CMaaLiteMutex m_Mutex;
public:
    WSAEVENT m_hWakeup;
    std::atomic<unsigned char> m_WakeUpFlags;
public:
    CMaaIOCPSockThread* m_pParent;
    //CMaaFdSockets* m_pFdSockets;
    //void RemoveWakeUpPair() noexcept;

    int Id;

    CMaaIOCPSockThread(CMaaIOCPSockThread* pParent);
    /*virtual*/ ~CMaaIOCPSockThread();

    int AddRef() noexcept;
    int DelRef() noexcept;
    virtual CMaaIOCPSockThread* CreateNewThread();
    void AddCommand(int Cmd, CMaaIOCPSockThread* Par1 = nullptr, void* Par2 = nullptr);
    void AddCommand(SCmd* c) noexcept;
    //virtual void OnCommand(SCmd * /*c*/) {}
    virtual void OnExitCommand(SCmd* c) noexcept;
    unsigned Run();
    virtual void Process();
    virtual bool Process1();
    void WakeUp(unsigned char flags = 0) noexcept;
    void WakeUpChildThreads() noexcept;
    void SetExitChildThreads() noexcept;

    ADD_ALLOCATOR(CMaaIOCPSockThread)
};
//---------------------------------------------------------------------------
class ToolsExport CMaaIOCPSocket //: public CMaaFdSocket
{
public:
    union sin46
    {
        struct sockaddr_in m_sin;
        struct sockaddr_in6 m_sin6;
    };
    SOCKET m_Socket;// = INVALID_SOCKET;
    int m_domain;// = -1; // AF_INET / AF_INET6
    //bool      m_bIsClosing;

#ifdef _WIN32
    //int m_Err = 0;
    //long m_wsa_ev = 0;
#endif

    CMaaDLink_pItem<CMaaIOCPSocket> m_ListItem;

    CMaaIOCPSocket(/*CMaaFdSockets* pFdSockets,*/ int af, int type, int protocol);
    virtual ~CMaaIOCPSocket();

    SOCKET GetSocket() const noexcept;
    void   SetSocket(SOCKET s, int domain = -1) noexcept /*~~~*/;
    void SwapSockets(SOCKET& s, int& domain) noexcept;

    virtual int Notify_Read();
    virtual int Notify_Write();
    virtual int Notify_Error();
    virtual void Notify_Act() noexcept;
    virtual int Notify_Accepted(_IP IpFrom, _Port Port);
    virtual int Notify_Connected(_IP Ip, _Port Port, const char* DnsName);
    //#ifdef TOOLSLIB_USE_IPV6
    virtual int Notify_Accepted6(_byte* Ip6From, _Port Port);
    virtual int Notify_Connected6(_byte* Ip6, _Port Port, const char* DnsName);
    //#endif
    virtual int Notify_PreDeletingByException();
    virtual int Notify_PreClosingByException(const char* /*msg*/) { return 0; }
    virtual CMaaString GetConnectionName() noexcept;

    void Error(const char* msg);
    static void StaticError(const char* msg);
    int CloseByException(const char* msg = nullptr);
    int Close();

    //void AddFdSocket();
    //void RemoveFdSocket(bool bFromDestructor = false) noexcept;
    //void MoveToFront() noexcept;
    //void MoveToTail() noexcept;

    static const char* GetLocalHostName() noexcept;
    static _IP GetLocalHostIp();
    _IP GetHostByName(const char* DnsName, _Port* Port = nullptr, _IP* pAllIPs = nullptr, int* pAllIPsCount = nullptr);
    char* GetHostByAddr(_IP Ip, char* DnsName);
#ifdef TOOLSLIB_USE_IPV6
    static _byte* GetLocalHostIp6(_byte* RetIp);
    _byte* GetHostByName6(_byte* RetIp, const char* DnsName, _Port* Port = nullptr, _byte** pAllIPs = nullptr, int* pAllIPsCount = nullptr);
    char* GetHostByAddr6(_byte* Ip6, char* DnsName);
#endif
    void SetNonBlockingMode();
    int GetDomainSock() const noexcept;
};
//---------------------------------------------------------------------------
//
//  TCP sockets & services
//
//---------------------------------------------------------------------------
class ToolsExport CMaaIOCPTcpSocket : public CMaaIOCPSocket
{
    /*
    class CCloseTimer : public CMaaSocketTimer
    {
    public:
        CMaaTcpSocket* m_pSocket;

        CCloseTimer(int PeriodUs = 2000); // 2 ms repeatedly calling Notify_Read() after FD_CLOSE received on Windows
        ~CCloseTimer();
        void OnTimer() override;
        void OnClose(CMaaTcpSocket* pSocket, bool bShortPeriod);
    };
    */
    _qword m_BytesSend, m_BytesRecv;
    _qword m_BytesSend0, m_BytesRecv0;
    int m_SendSpeed, m_RecvSpeed;
public:
#ifdef _WIN32
    //DWORD m_dwLastSendErr = 0;
#endif
    //CCloseTimer m_CloseTimer;
    sin46 m_s; // m_sin, m_sin6

    CMaaIOCPTcpSocket(/*CMaaFdSockets* pFdSockets,*/ int domain = AF_INET);
    ~CMaaIOCPTcpSocket();
    int SendData(const void* Buffer, int Size);
    int RecvData(void* Buffer, int Size);
    int Connect(_IP Ip, _Port Port);
    int Connect(const char* DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
    int Connect(const CMaaString& DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
#ifdef TOOLSLIB_USE_IPV6
    int Connect6(const _byte* Ip6, _Port Port);
    int Connect6(const char* DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
    int Connect6(const CMaaString& DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
#endif
    int Detect64(const CMaaString& DnsName_Port, CMaaString* pDnsName_Port = nullptr) noexcept;
    int Detect46(const CMaaString& DnsName_Port, CMaaString* pDnsName_Port = nullptr) noexcept;
    int AsyncConnect(_IP Ip, _Port Port);
    int AsyncConnect(const char* DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
    int AsyncConnect(const CMaaString& DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
#ifdef TOOLSLIB_USE_IPV6
    int AsyncConnect6(const _byte* Ip6, _Port Port);
    int AsyncConnect6(const char* DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
    int AsyncConnect6(const CMaaString& DnsName, _Port Port = (_Port)-1); // DnsName can be as "www.ru:80"
#endif
    int ConnectAgain(int fCallNotify); // for AsyncConnect, call it from fd_write
    int Bind(_Port Port, _IP Ip = 0, bool bReuse = true);              // 0 - ok
    int Bind(CMaaString strIpPort, bool bReuse = true);              // 0 - ok
#ifdef TOOLSLIB_USE_IPV6
    int Bind6(_Port Port, const _byte* Ip6 = nullptr, bool bReuse = true);              // 0 - ok
    int Bind6(CMaaString strIpPort, bool bReuse = true);              // 0 - ok
#endif
    int Listen(int QueenDepth = 5);      // 0 - ok
    SOCKET Accept(_IP* IpFrom = nullptr, _Port* Port = nullptr);
#ifdef TOOLSLIB_USE_IPV6
    SOCKET Accept6(_byte* Ip6From = nullptr, _Port* Port = nullptr);
#endif
    int GetBindedPort();                  // returns binded port number
    bool GetConnInfo(_IP* pLocalIp, _Port* pLocalPort, _IP* pRemoteIp, _Port* pRemotePort, bool bThrow = true);
#ifdef TOOLSLIB_USE_IPV6
    bool GetConnInfo6(_byte* pLocalIp, _Port* pLocalPort, _byte* pRemoteIp, _Port* pRemotePort, bool bThrow = true);
#endif
    /*
    bool IsClosed(int LastReceivedLen) const noexcept;
    int RealRcvSndLen(int LastReceivedLen, bool* pbIsClosed = nullptr) const noexcept
    {
        if (pbIsClosed)
        {
            *pbIsClosed = IsClosed(LastReceivedLen);
        }
        return LastReceivedLen >= 0 ? LastReceivedLen : -1 - LastReceivedLen;
    }
    */
    virtual void InitCounters(bool bConnected = true);
    virtual void Timer1sSpeed();
    void GetStatistics(_qword* pSentBytes, _qword* pRecvBytes, int* pSendSpeed, int* pRecvSpeed, bool bShapersSpeed =
#ifdef __SHAPERS
        true
#else
        false
#endif
    ) const noexcept;
    bool DisableNagleAlgoritm(bool bDisable = true, SOCKET s = INVALID_SOCKET) const noexcept;
    _qword GetDataSentSize() const noexcept { return m_BytesSend; }
    _qword GetReceivedDataSize() const noexcept { return m_BytesRecv; }
};
//---------------------------------------------------------------------------
#endif
