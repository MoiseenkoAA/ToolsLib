
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/sk2add.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file is a socket2.{h|cpp} additional classes for parsing raw HTTP
 * requests and responces, syncronious sockets read/write with timeouts
 * like a files io. First dated 10.10.2009.
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
#ifdef __unix__
#ifndef SOCKET
#define SOCKET int
#endif
#endif
*/

//-----------------------------------------------------------------------------------------
class CHttpResponceHandlerForReader
{
    CMaaConcatString1 m_Buffer;
    CMaaString m_Method;
    CMaaConcatString1 m_InHeader_ccstr, m_InData;
    CMaaString m_InHeader;// , m_InHeader2;
    //int m_MaxDataSize;
    int m_Status;
protected:
    int FillReturnResults(CMaaString &Method, CMaaString &Header, CMaaString &Html, int &Status, CMaaString *NextData, int CleanType = 1);
    int FillReturnResults(CMaaString &Method, CMaaString &Header, CMaaString &Html, int &Status, CMaaString &NextData, int CleanType = 1);
public:
    enum eHandlerType
    {
        eRequest = 21,
        eResponce = 20,
        eResponceWithContinueStatusIsAllowed = 19
    };
protected:
    eHandlerType m_hType;
public:
    CHttpResponceHandlerForReader(eHandlerType hType = eResponce) noexcept;
    ~CHttpResponceHandlerForReader();
    int AppendData(CMaaString &Method, CMaaString Data, CMaaString &Header, CMaaString &Html, int MaxDataSize, int &Status, CMaaString &NextData /*= *(CMaaString *)nullptr*/);
    int AppendData(CMaaString &Method, const void * ptrData, int DataLength, CMaaString &Header, CMaaString &Html, int MaxDataSize, int &Status, CMaaString &NextData /*= *(CMaaString *)nullptr*/);
    int GetResultsOnUnexpectedConnectionClosed(CMaaString &Header, CMaaString &Html, int &Status);
    int AppendData(CMaaString &Method, const void * ptrData, int DataLength, int MaxDataSize = 100 * 1024)
    {
        return AppendData(Method, ptrData, DataLength, resHeader, resHtml, MaxDataSize, resStatus, resNextData);
    }
    int AppendData(CMaaString &Method, CMaaString Data, int MaxDataSize = 100 * 1024)
    {
        return AppendData(Method, Data, (warning_int)Data.Length(), resHeader, resHtml, MaxDataSize, resStatus, resNextData);
    }
    //CMaaString &Method;
    CMaaString resHeader;
    CMaaString resHtml;
    //int MaxDataSize;
    int resStatus;
    CMaaString resNextData;
};
//-----------------------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef TOOLSLIB_SINGLE_THREAD
#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)
/*
#ifdef _WIN32
int gettimeofday(timeval *p, void *);
void usleep(int us);
#endif
*/

void SetExitFileSockThreads();

class CMaaTcpFileMainThread : public CMaaSockThread
{
    int m_Ref0Count;
public:
    typedef void (* tCreateNewThread)(void * param, CMaaSockThread * MainThr);
    //void (* m_pCreateNewThread)(void * param, CMaaSockThread * MainThr);
    tCreateNewThread m_pCreateNewThread;
    void * m_Param;
    CMaaEvent m_evReady, m_evStart;
    CMaaTcpFileMainThread(CMaaSockThread * pParent = nullptr, bool bCreate = true);
    ~CMaaTcpFileMainThread();
    bool AddTask(tCreateNewThread p, void * param);
    void Process();

    ADD_ALLOCATOR(CMaaTcpFileMainThread)
};

class CMaaTcpFile;

//--------------------------------------------------------------------------------
class CMaaTcpFileConnection : public CMaaTcpSocket
{
public:
    CMaaTcpFile * m_pHandler;

    _IP6 m_LocalIp6, m_RemoteIp6;
    _IP m_LocalIp, m_RemoteIp;
    _Port m_LocalPort, m_RemotePort;

    CMaaSockTimerT<CMaaTcpFileConnection> m_Timer0, m_Timer1, m_Timer11, m_Timer2, m_Timer3, m_Timer4, m_Timer33, m_Timer44, m_InterruptTimer;
    //CMaaAutoInitObject<int, 0> m_TotalBytesReceived, m_TotalBytesSent;
    int m_TotalBytesReceived = 0, m_TotalBytesSent = 0;
    //int m_TotalBytesReceived = 0;
    //int m_TotalBytesSent = 0;
    CMaaString m_TestMessage;
    //CMaaAutoInitObject<int, 0> m_flg1;
    int m_flg1 = 0;
    void SetTestMessage(CMaaString msg, int Flag = 0);
protected:
    char * m_ptr;
    int m_pos, m_len, m_wpos, m_wlen;
    int m_Mode;
    _dword m_ReadTimeOutMs, m_WriteTimeOutMs, m_AdoptiveReadTimeOutMs, m_AdoptiveWriteTimeOutMs;
    void Completed(int Mode = 0);
    void CheckInterruptEvent(int From = 0 /*eRead / eWrite*/);
public:
    CMaaString m_ConnName;

    CMaaString m_SendData;

    CMaaString GetConnectionName() noexcept override;
    CMaaTcpFileConnection(CMaaFdSockets * pFdSockets, const char * Server, int DefPort, CMaaTcpFile * pHandler);
    CMaaTcpFileConnection(CMaaFdSockets * pFdSockets, SOCKET s, int domain, CMaaTcpFile * pHandler);
    ~CMaaTcpFileConnection();
    int Notify_Read() override;
    int Notify_Write() override;
    int Notify_Error() override;
    int Notify_PreDeletingByException() override;
    int Notify_Connected(_IP Ip, _Port Port, const char * DnsName) override;
    int Notify_Accepted(_IP IpFrom, _Port Port) override;
    int Notify_Connected6(_byte * Ip6, _Port Port, const char * DnsName) override;
    int Notify_Accepted6(_byte * IpFrom6, _Port Port) override;
    void OnTimer(int f);
    bool AddSendData(CMaaString Data, _dword TimeOutMs = (_dword)-1, _dword AdoptiveTimeOutMs = (_qword)-1); // -1 - infinite, -2 - one block
    bool SetRead(void * ptr, int len, _dword TimeOutMs = (_dword)-2, _dword AdoptiveTimeOutMs = (_qword)-1); // -1 - infinite, -2 - one block
    void EnableInterruptEvent();
};
//--------------------------------------------------------------------------------
class CMaaTcpFileServer : public CMaaTcpSocket
{
public:
    CMaaTcpFile * m_pHandler;
    CMaaSockTimerT<CMaaTcpFileServer> m_Timer0, m_Timer1, m_Timer2, m_Timer3, m_InterruptTimer;
protected:
    int m_Mode;
    _dword m_AcceptTimeOutMs;//, m_WriteTimeOutMs;

    struct sAcceptedSocketsLink : public CMaaDLink
    {
        SOCKET s;
        _IP ip;
        _IP6 ip6;
        _Port port;
    };
    int m_Accepting;
    CMaaDList<sAcceptedSocketsLink> m_AcceptedSocketsList;

    int Notify_Read  () override;
    int Notify_Write () override;
    int Notify_Error () override;

    CMaaString m_ServerName;
    CMaaString GetConnectionName() noexcept override;

    void Completed();
    void CheckInterruptEvent(int From = 0 /**/);

public:
    void OnTimer(int f);

    SOCKET m_AcceptedSocket, * m_pAcceptedSocket;
    _IP6 m_IpFrom6;    _byte * m_pIpFrom6;
    _IP m_IpFrom,             * m_pIpFrom;
    _Port m_PortFrom, * m_pPortFrom;

    CMaaTcpFileServer(CMaaFdSockets * pFdSockets, int Port, _IP6 Ip6, CMaaTcpFile * pHandler, bool bReuse = true, const char * ServerName = nullptr);
    ~CMaaTcpFileServer();

    bool AcceptSocket(SOCKET * s, _IP * IpFrom, _Port * Port, _dword TimeOutMs = (_dword)-2); // -1 - infinite, -2 - one accept
    bool AcceptSocket6(SOCKET * s, _byte * IpFrom6, _Port * Port, _dword TimeOutMs = (_dword)-2); // -1 - infinite, -2 - one accept
    void EnableInterruptEvent();
};
//--------------------------------------------------------------------------------
class CMaaTcpFile
{
    friend class CMaaTcpFileConnection;
    friend class CMaaTcpFileServer;

    CMaaEvent m_ev, *m_pInterruptEvent;
    //CMaaAutoInitObject<_dword, 0> m_InterruptCheckingTime;
    _dword m_InterruptCheckingTime = 0;
    XTOOSockErr * m_err;
    bool m_bThrowExcept;
    //bool m_bIsOpened;
    int m_EvType, m_len, m_wlen;

    CMaaTcpFileConnection * m_pConn;
    CMaaTcpFileServer * m_pServer;

    enum
    {
        eRead = 0x01,
        eWrite = 0x02,
        eClosed = 0x04,
        eExcept = 0x08,
        eDestroyed = 0x10,
        eConnected = 0x20,
        eSoftError = 0x40,
        eCreated = 0x80,
        eAdoptiveReadTimeOut = 0x100,
        eAdoptiveWriteTimeOut = 0x200,
        eComplete = 0x1000
    };

    CMaaString m_Url;
    int m_DefPort;
    SOCKET m_Socket;
    int m_domain;

    _Port m_BindPort;
    _IP6 m_BindIp;
    bool m_bBindReuse;

    static void fnCreateNewThread(void * param, CMaaSockThread * MainThr);
    static void fnCreateNewThread2(void * param, CMaaSockThread * MainThr);
    static void fnCreateNewThreadServer(void * param, CMaaSockThread * MainThr);
#ifdef SAFE_GLOCKS
    bool IsError_locked(CMaaWin32Locker<CMaa_gLock_lib_Mutex>& Locker, bool bThrow = true); // if (bThrow && m_bThrowExcept && error) { unlock; throw error; }
#else
    bool IsError_locked(bool bThrow = true); // if (bThrow && m_bThrowExcept && error) { unlock; throw error; }
#endif

    CMaaString m_ReadData, m_DelayedReadData;

    int m_DelayedReadDataLen;

private:
    CMaaTcpFile(const CMaaTcpFile &);
    CMaaTcpFile & operator=(const CMaaTcpFile &);
public:
    _IP m_LocalIp, m_RemoteIp;
    _Port m_LocalPort, m_RemotePort;
    _IP6 m_LocalIp6, m_RemoteIp6;

    CMaaTcpFile(bool bThrowExcept = true);
    CMaaTcpFile(SOCKET s, int domain, bool bThrowExcept = true);
    CMaaTcpFile(const char * pszUrl, int DefPort = -1, bool bThrowExcept = true);
    CMaaTcpFile(CMaaString Url, int DefPort = -1, bool bThrowExcept = true);
    ~CMaaTcpFile();
    bool Open(CMaaString Url, int DefPort = -1);
    bool Open(SOCKET s, int domain);// = AF_INET);
    bool IsOpen();
    bool IsOpened()
    {
        return IsOpen();
    }
    bool Close();
    void SetInterruptEvent(CMaaEvent *pInterruptEvent /*= nullptr*/, _dword InterruptCheckingTime = (_dword)1000); // Object is not deleted from destructor now
    // timeout: -1 - infinite, -2 - one portion get infinit to, time out | 0x80000000 - for one portion get time out
    // adoptive time out: for allowed timeout between portions
    int Read(void * ptr, int len, _dword TimeOutMs = (_dword)-1, _dword AdoptiveTimeOutMs = (_qword)-1);
    bool StartDelayedRead(int len, _dword TimeOutMs = (_dword)-1, _dword AdoptiveTimeOutMs = (_qword)-1, bool bThrow = false);
    int GetDelayedReadData(CMaaString &txt, _dword to, bool bThrow = false);
    int ReadLine(void * ptr, int len, _dword TimeOutMs = 30000, CMaaString eof = CMaaStringCrLf);
    bool Peek(CMaaString txt);
    int Write(const void * ptr, int len, _dword TimeOutMs = (_dword)-1, _dword AdoptiveTimeOutMs = (_qword)-1);
    int Write(CMaaString SendMsg, _dword TimeOutMs = (_dword)-1, _dword AdoptiveTimeOutMs = (_qword)-1);
    //bool Bind(_Port Port, _IP Ip = 0, bool bReuse = true);
    bool Bind(_Port Port, _IP6 Ip6, bool bReuse = true);
    SOCKET Accept(_dword TimeOutMs = (_dword)-1, int * pDomain = nullptr);
    bool GetError(XTOOSockErr &err, int iThrowExcept = 0);
    bool IsError();
};
//--------------------------------------------------------------------------------
#else

#endif
#endif
//---------------------------------------------------------------------------
