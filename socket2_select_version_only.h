// old 30.08.20 12:18 select() version

// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/socket2.h

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


#define __SOCK_NEW_TIMERS

//typedef unsigned long  _IP;
//typedef unsigned short _Port;
typedef _dword  _IP;
typedef _word _Port;

//#define DEBUG_SET_TEST_MESSAGE

/*
#ifdef __unix__
#ifndef SOCKET
#define SOCKET int
#endif
#endif
*/

#ifdef __unix__
ToolsExport const char * CMaaIpToText ( _IP ip, int Mode = 0 );
ToolsExport char * CMaaIpToTextEx ( char * txt, _IP ip, int Mode = 0 );

//---------------------------------------------------------------------------
// returns 0 -- means: Ok, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4" is converted to 0x01020304
ToolsExport int CMaaIpToLong ( const char * txt, _IP * RetIp );
// returns len of ip text, -1 on error, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4-1.2.3.55" is converted to 0x01020304, ret=7
ToolsExport int CMaaIpToLongEx ( const char * txt, _IP * RetIp, const char * AvailableTrailers = "-/ " );
ToolsExport _IP CMaaIpAddress ( _byte a, _byte b, _byte c, _byte d );
ToolsExport _IP CMaaGetMaskByNum(int Num); // Num = [0..32]
ToolsExport int CMaaGetNumByMask(_IP Mask); // -1 on error

#endif // __unix__

//---------------------------------------------------------------------------

//#if defined(__TOOLS_USE_WINSOCK) || defined(__TOOLS_USE_WINSOCK2)
#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)

//---------------------------------------------------------------------------


// Name of system service
ToolsExport _PCSTR GetServByProtoAndPort ( const _word PortValue, _PCSTR IpProtoName );

//---------------------------------------------------------------------------
class CMaaFdSockets;
class CMaaSocket;
extern CMaaFdSockets * gCMaaFdSockets;
//extern CMaaTasks     * gCMaaConnectTasks;
//---------------------------------------------------------------------------
extern bool gCMaaTcpSocketsCompatibleRead; //only with out of shapers

class CMaaConnectionInTimeCharacteristics
{
    struct DataBlock
    {
        _qword Data[8];
        ADD_ALLOCATOR(DataBlock)
    };

    int m_pos, m_max_points;
    CMaaPtr<DataBlock *> m_bptr;
    _qword m_Accumulator, m_InitialValue, m_Units;

public:

    CMaaConnectionInTimeCharacteristics(int MaxPoints, _qword InitialValue = 0, _qword Units = 1);
    virtual ~CMaaConnectionInTimeCharacteristics();
    void Reset(_qword InitialValue = 0, bool bToLastData = false);
    void Set_Inc(_qword x);
    void Accumulate(_qword x, bool bSet_and_Inc = false);
    _qword GetDataDifference(int Period);
    _qword GetAverageDataDifference(int Period);
    int GetCurPos();
    int GetCurMaxPeriod();
};

#define __SHAPERS
#ifdef __SHAPERS
#define _SHAPER_TICKS_PER_SECOND 5

extern int gStrictTcpShaperLimits;

struct CMaaTcpShaper;
class CMaaTcpSocket;
class CMaaShaperThread;
struct CMaaTcpLLShaper : public CMaaDLink // Low Level Shaper
{
    CMaaTcpShaper *m_Shaper;
    CMaaTcpSocket *m_pSocket;
#ifndef __unix__
    HANDLE m_hEv;
#else
    bool m_hEv;
#endif
    int m_MaxSpeedLimit;
    int m_V;
    int m_VTempSum, m_VAverageLast;
    double m_Weight;
    int m_V0, m_Vsr, m_Vsrd;
    double m_fVsr;
    bool m_bHardLimit;

    double m_Reserved, m_v0, m_v1, m_v2, m_Max;
    bool m_bFullFilled, m_bFinished;
    void RecCalc();

    CMaaString Dump();

    CMaaTcpLLShaper(CMaaTcpSocket * pSocket)
    :   m_pSocket(pSocket)
    {
        m_Shaper = NULL;
#ifndef __unix__
        m_hEv = CreateEvent(NULL, TRUE, TRUE, NULL);
#else
        m_hEv = false;
#endif
        m_MaxSpeedLimit = -1;
        m_bHardLimit = false;
        m_V = m_VTempSum = m_VAverageLast = 0;
        m_V0 = m_Vsr = m_Vsrd = 0x7fffffff;
        m_Weight = 1.0; //
        m_fVsr = (double)m_Vsr;
    }
    ~CMaaTcpLLShaper();

    bool IsAttached()
    {
        return m_Shaper != NULL;
    }

    void SetConnectionLimits(int MaxSpeed, int bHardLimit = -1);
    /*
     void CalcNWeights(double w, double MaxSpeed)
     {
          m_NWeight = w;
          bool f = m_V0 == m_Vsr;
          m_fVsr = MaxSpeed * m_NWeight;
          m_Vsr = (int)(MaxSpeed * m_NWeight);
          if   (m_Vsr <= 0)
          {
               m_Vsr = 1;
               m_fVsr = (double)m_Vsr;
               if   (MaxSpeed <= 0)
               {
                    m_Vsr = 0x7fffffff;
                    m_fVsr = (double)m_Vsr;
               }
          }
          if   (f || m_V0 < m_Vsr)
          {
               m_V0 = m_Vsr;
          }
     }
        */
    void OnTimer(int i);
    int GetLenReserved(int Len = 0x7fffffff);
    void SubLenReserved(int Len);
};

#ifdef _WIN32
int CMaaSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep, const char * FuncName);
#endif

struct CMaaTcpShaper : public CMaaDLink
{
    int m_Type; // 0 - General, 1 - User, 2 - AutoUser
    enum
    {
        eGeneral = 0,
        eUserSubshaper = 1,
        eAutoUserSubshaper = 2
    };
    int m_UserId; // -1 - anonymous, -2 - default, -3 - error(not set), n - user id = n
    CMaaString m_Name;
    CMaaUnivHash<int, CMaaTcpShaper *> m_UserShapers;
    CMaaUnivHash_KeyIsClass<CMaaString, CMaaTcpShaper *> m_GeneralShapers;

    int m_MaxSpeed;
    int m_Bank;

    double m_Weight;
    int m_V0, m_Vsr, m_Vsrd;
    int m_V, m_VTempSum, m_VAverageLast;
    double m_fVsr;

    double m_Reserved, m_v0/*, m_v1, m_v2*/, m_Max;
    bool m_bFullFilled, m_bFinished;
    int m_ConnectionsCount[3];

    void RecCalc(int Step, int &Result);
    void Update(/*CMaaShaperThread * pThread*/);

    struct CMaaTcpShaper * m_Shaper;

    struct L : public CMaaDLink
    {
        CMaaTcpShaper *p;
    };

    L m_L;

    CMaaConnectionInTimeCharacteristics * m_pConnectionInTimeCharacteristics[4];

    struct Condition : public CMaaDLink
    {
        Condition() {}
        virtual ~Condition() {}
        virtual Condition * Copy() {return TL_NEW Condition;}
    };

    CMaaDList<L> m_Childs;
    CMaaDList<CMaaTcpLLShaper> m_ChildsLL;
    CMaaDListAD<Condition> m_Conditions;

    void DeleteConnectionInTimeCharacteristics();
    void CreateConnectionInTimeCharacteristics(int s = 120, int m = 120, int h = 120, int d = 183);
    void ResetConnectionInTimeCharacteristics();

    void SetConditions(CMaaDListAD<Condition> &List, bool bAllowToMoveAllListData);
    static void CopyList(CMaaDList<Condition> &dst, CMaaDList<Condition> &src);

    int GetNotEmptyChildrenShapersCount();
    int GetConnectionsCount(bool bRecursive = false);
    void SetConnectionLimits(int MaxSpeed, bool bRecursive = false);

    CMaaString Dump();
    void DumpAll();

    /*
     static CMaaString GenName(CMaaString GeneralShaperName);
     static CMaaString GenName(int UserId);
     static CMaaString GenName(); // AutoUserSubShaper
     */
    CMaaTcpShaper(int Type, CMaaString Name, double W = 1.0, int MaxSpeed = -1, int UserId = -3);
    CMaaTcpShaper(CMaaString GeneralShaperName, double W = 1.0, int MaxSpeed = -1); // Type = eGeneral
    CMaaTcpShaper(CMaaString UserName, int UserId /*= -1*/, double W = 1.0, int MaxSpeed = -1); // Type = eUserSubshaper, UserId = -1 == anonymous, -2 - default user shaper
    //CMaaTcpShaper(double W = 1.0, int MaxSpeed = -1); //  // Type = AutoUserSubShaper
    ~CMaaTcpShaper();

private:
    void Init(int Type, CMaaString Name, double W = 1.0, int MaxSpeed = -1, int UserId = -3);
public:

    CMaaString GetName() {return m_Name;}
    CMaaString GetDispName();
    CMaaString GetShaperNameRawRecursiveLibName();

    int GetLenReserved(int Len = 0x7fffffff);
    int GetLenReserved2(int Len);
    void SubLenReserved(int Len);

    void OnTimerUpdate(/*CMaaShaperThread * pThread*/);
    void OnTimer(int i);

    void ReInit(double W, int MaxSpeed);
    void ReInitAutoUserSubshaper(CMaaTcpShaper * s);

    void SetNewShaperSpeedLimit(int MaxSpeed, double W = -1.0);

    bool Add(CMaaTcpShaper &s);
    bool Add(CMaaTcpLLShaper &l);
    bool Del(CMaaTcpShaper &s);
    bool Del(CMaaTcpLLShaper &l);
    void AddRecursiveConnectionCounter(int val);

    void CopyFrom(CMaaTcpShaper *n, CMaaShaperThread *r);
    void AddClone(int UserId, CMaaTcpShaper *n, CMaaShaperThread *r);
    int DeleteUnused();
    bool HasAttachedSockets();
    void FillEvents(CMaaPtrAE<CMaaTcpLLShaper *> &Ev, int &pos); //////
};

//--------------------------------------------------------------------------
class CMaaShaperThread : public CMaaThread //CMyThread
{
public:
    CMaaDList<CMaaTcpShaper> m_ShaperList;
    CMaaTcpShaper * m_SndSh, * m_RcvSh;
    int m_AveIndex, m_ShaperConditionLastCheckTime;

    CMaaTcpShaper * m_Snd1000, * m_Rcv1000;

#ifdef __SHAPERS_PROFILING
    CMaaFile m_logfile___;
    CMaaTime m_hrt___;
#endif

    CMaaShaperThread();
    ~CMaaShaperThread();

    //void GetV(int n, int *sv, int *rv);
    //void SetShapers(CMaaXmlNode n0);

    unsigned Run();
    unsigned Run1();
    unsigned Run2();
private:
    void OnTimerUpdate();
    void DeleteUnused();
    //void SetShapers(CMaaXmlNode n0, CMaaTcpShaper *s, bool IsRecv);
public:
    void CleanupShapers();
    CMaaTcpShaper * FindShaper(CMaaString ShaperNameEx, int pos, int UserId, CMaaTcpShaper *s, bool bAutoCopy = true);
    void FindShaper(CMaaString ShaperNameEx, int UserId, CMaaTcpShaper *&s, CMaaTcpShaper *&r);

    //CMaaShaperThread::UserObject
    struct UserObject
    {
        UserObject()
        {
        }
        virtual ~UserObject()
        {
        }
    };

    UserObject * m_pUserObject;
};
#endif
//---------------------------------------------------------------------------
class CMaaTcpFileMainThread;
//---------------------------------------------------------------------------
class CMaaSockStartup
{
#ifndef __unix__
    WSADATA m_WsaData;
#endif
    bool m_IsOk;
#ifdef __SOCK_NEW_TIMERS
public:
    CMaaMutex m_TimerMutex;
private:
    CMaaTime m_Time;
#endif
public:
    CMaaSockStartup(BOOL fThrow = TRUE);
    ~CMaaSockStartup();
    bool IsOk();

#ifdef __SHAPERS
    CMaaShaperThread m_ShaperThread;
#endif
    CMaaTcpFileMainThread * m_pTcpFileMainThread;
    bool StopTcpFileSockets();
    bool StartTcpFileSockets();
#ifdef __SOCK_NEW_TIMERS
    _qword GetTime();
#endif
};
//---------------------------------------------------------------------------
class CMaaEnum
{
public:
    enum eMode
    {
        eNothing  = 0,
        eRead     = 1,
        eWrite    = 2,
        eExept    = 4,
        eConnect  = 8,
        eHold     = 0x10,
        eAll      = eRead | eWrite | eExept,
        eSensitiveEvents = eRead | eWrite | eExept | eConnect | eHold,

        eDisableShift = 8,
        eDisableRead    = eRead    << eDisableShift,
        eDisableWrite   = eWrite   << eDisableShift,
        eDisableExept   = eExept   << eDisableShift,
        eDisableConnect = eConnect << eDisableShift,
        eDisableHold    = eHold    << eDisableShift,
        eDisableAll     = (0x1F)   << eDisableShift,

        eEventShift = 16,
        eEventRead    = eRead    << eEventShift,
        eEventWrite   = eWrite   << eEventShift,
        eEventClose   = 4        << eEventShift,

        eDisableEventShift = 24,
        eDisableEventRead    = eRead    << eDisableEventShift,
        eDisableEventWrite   = eWrite   << eDisableEventShift,
        eDisableEventClose   = 4        << eDisableEventShift,
        eDisableAllEvents    = (0x1F)   << eDisableEventShift,

        eExcept        = eExept,
        eDisableExcept = eDisableExept
    };

    CMaaEnum () {};
    virtual ~CMaaEnum () {};
};
//---------------------------------------------------------------------------
class CMaaFdSocket : public CMaaEnum
{
public:
    CMaaFdSockets * m_pFdSockets;
    CMaaAutoInitObject<bool, false> m_bDebug;
    enum
    {
        e_fds_current_time = -1,
        e_fds_created_time = 0,
        e_fds_connected_at_time,
        e_fds_last_receive_time,
        e_fds_last_sent_time,
        e_fds_user_time,
        e_fds_max_time = e_fds_user_time + 7
    };
    _qword m_AbsTimeCreated;
    CMaaAutoInitObject<_qword, -1> m_hrTime[e_fds_max_time]; // e_fds_start_time, e_fds_last_receive_time, e_fds_last_sent_time and user time values (hrt)

    _qword UpdateHrTime(int index);
    _qword SetHrTime(int index, _qword t);
    _qword GetHrTime_(int index);
    _qword GetHrTime(int index, int RelatedTo = e_fds_connected_at_time, bool bAbsoluteTime = false, int factor = 1000000);

#ifdef _WIN32
    int m_LastMode;
    WSAEVENT m_Ev;
    long m_pm;
#endif
    int m_AcceptFlag;
    CMaaFdSocket(CMaaFdSockets * pFdSockets);
    ~CMaaFdSocket ();

    void SetFdSockets ( CMaaFdSockets * pFdSockets );
    void SetFdSockets ( CMaaFdSocket * FdSocket );

    virtual void ChangeFdMode ( int Mode );
    int GetFdMode();
    void SetAcceptFlag(int f = 1)
    {
        m_AcceptFlag = f;
    }
    void WakeUp();
    //void ResetEvent(int ev);
};
//---------------------------------------------------------------------------
class CMaaSocketTimer : public CTooDLink
{
    bool m_bActive;
    _qword m_PeriodUs;
    CMaaFdSockets * m_pFdSockets;
#ifdef __SOCK_NEW_TIMERS
    _qword m_Next;
    CMaaHeap<_qword, CMaaSocketTimer *>::Handle m_Handle;
public:
    void ResetHandle()
    {
        m_Handle = NULL;
    }
private:
#else

#ifdef __unix__
    timeval m_Next;
#else
    DWORD m_Next;
#endif

#endif

public:
    int m_TmpFlg;

    CMaaSocketTimer(int PeriodUs = 0);
    virtual ~CMaaSocketTimer();
    void Attach(CMaaFdSockets * pFdSockets);
    void Start(_qword PeriodUs, int iWakeUp);
    void Start(_qword PeriodUs = 0, bool bWakeUp = true)
    {
        Start(PeriodUs, bWakeUp ? 1 : 0);
    }
    void Stop();
    void Detach();

#ifndef __SOCK_NEW_TIMERS
#ifdef __unix__
    void PreTimer(timeval *t = NULL);
    void GetWaitForTime(int *us, timeval *t = NULL);
#else
    void PreTimer(DWORD *t = NULL);
    void GetWaitForTime(int *us, DWORD *t = NULL);
#endif
#else
    void PreTimer(_qword t);
    void GetWaitForTime(int *us, _qword t);
#endif
    virtual void OnTimer() = 0;

#ifndef __SOCK_NEW_TIMERS
#endif
    _qword GetPeriod();// {return m_PeriodUs;}
    bool IsStarted();// {return m_bActive;}
    void ReStart()
    {
        Start(m_PeriodUs);
    }
};
//---------------------------------------------------------------------------
template<class S> class CMaaSockTimerT : public CMaaSocketTimer
{
    S * pS;
    int f;
public:
    CMaaSockTimerT(S * s, int Func = 1)
    {
        pS = s;
        f = Func;
    }
    ~CMaaSockTimerT()
    {
    }
    void OnTimer()
    {
        pS->OnTimer(f);
    }
};
//---------------------------------------------------------------------------
class ToolsExport CMaaSocket : public CMaaFdSocket
{
public:
    SOCKET    m_Socket;
    //bool      m_bIsClosing;

    CMaaSocket(CMaaFdSockets *pFdSockets, int af, int type, int protocol );
    virtual ~CMaaSocket ();

    SOCKET GetSocket ();
    void   SetSocket ( SOCKET s );
    void SwapSockets(SOCKET &s);

    virtual void SetTestValue(int x) {}
    virtual void SetTestMessage(CMaaString msg, int Flag = 0) {}
    virtual int Notify_Read  ();
    virtual int Notify_Write ();
    virtual int Notify_Error ();
    virtual int Notify_Accepted ( _IP IpFrom, _Port Port );
    virtual int Notify_Connected ( _IP Ip, _Port Port, const char * DnsName );
    virtual int Notify_PreDeletingByException ();
    virtual int Notify_PreClosingByException(const char * msg) {return 0;}
    virtual const char * GetConnectionName ();

    void Error ( const char * msg );
    static void StaticError ( const char * msg );
    int CloseByExeption (const char * msg = NULL);
    int CloseByException(const char * msg = NULL)
    {
        return CMaaSocket::CloseByExeption(msg);
    }
    int Close ();

    void AddFdSocket ();
    void RemoveFdSocket ();

    static const char * GetLocalHostName ();
    static _IP GetLocalHostIp ();
    _IP GetHostByName(const char * DnsName, _Port * Port = NULL, _IP * pAllIPs = NULL, int * pAllIPsCount = NULL);
    char * GetHostByAddr ( _IP Ip, char * DnsName );
    void SetNonBlockingMode ();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  TCP sockets & services
//
//---------------------------------------------------------------------------
class ToolsExport CMaaTcpSocket : public CMaaSocket
{
    class CCloseTimer : public CMaaSocketTimer
    {
    public:
        CMaaTcpSocket * m_pSocket;

        CCloseTimer(int PeriodUs = 2000); // 2 ms repeatedly calling Notify_Read() after FD_CLOSE received on Windows
        ~CCloseTimer();
        void OnTimer();
        void OnClose(CMaaTcpSocket * pSocket, bool bShortPeriod);
    };
    _qword m_BytesSend, m_BytesRecv;
    _qword m_BytesSend0, m_BytesRecv0;
    int m_SendSpeed, m_RecvSpeed;
protected:
    CMaaConnectionInTimeCharacteristics * m_pConnectionInTimeCharacteristics[2+2][4];
public:
    CCloseTimer m_CloseTimer;
#ifdef __SHAPERS
    // friend CMaaTcpLLShaper;
    int m_ShaperConditionLastCheckTime;
    CMaaTcpLLShaper m_SndLLShaper, m_RcvLLShaper;
    virtual void CheckConditions(CMaaTcpShaper * pShaper) {} // != NULL
#endif
    struct sockaddr_in m_sin;

    CMaaTcpSocket(CMaaFdSockets *pFdSockets);
    ~CMaaTcpSocket();
    int SendData ( const void * Buffer, int Size );
    int RecvData ( void * Buffer, int Size );
    int Connect ( _IP Ip, _Port Port );
    int Connect ( const char * DnsName, _Port Port = (_Port)-1 ); // DnsName can be as "www.ru:80"
    int AsyncConnect ( _IP Ip, _Port Port );
    int AsyncConnect ( const char * DnsName, _Port Port = (_Port)-1 ); // DnsName can be as "www.ru:80"
    int ConnectAgain ( int fCallNotify ); // for AsyncConnect, call it from fd_write
    int Bind(_Port Port, _IP Ip = 0, bool bReuse = true);              // 0 - ok
    int Bind(CMaaString strIpPort, bool bReuse = true);              // 0 - ok
    int Listen ( int QueenDepth = 5 );      // 0 - ok
    SOCKET Accept ( _IP * IpFrom = NULL, _Port * Port = NULL );
    int GetBindedPort ();                  // returns binded port number
    void GetConnInfo(_IP * pLocalIp, _Port * pLocalPort, _IP * pRemoteIp, _Port * pRemotePort);
    bool IsClosed(int LastReceivedLen);
    virtual void InitCounters(bool bConnected = true);
    virtual void Timer1sSpeed();
    virtual void CreateConnectionInTimeCharacteristics(int s = 120, int m = 120, int h = 120, int d = 183);
    virtual void DeleteConnectionInTimeCharacteristics();
    virtual void ResetConnectionInTimeCharacteristics(bool bSecondPartOnly = true);
    void GetStatistics(_qword * pSentBytes, _qword * pRecvBytes, int * pSendSpeed, int * pRecvSpeed, bool bShapersSpeed = true);
    bool DisableNagleAlgoritm(bool bDisable = true, SOCKET s = INVALID_SOCKET);
    _qword GetDataSentSize() {return m_BytesSend;}
    _qword GetReceivedDataSize() {return m_BytesRecv;}
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpSocket : public CMaaSocket
{
protected:
    struct sockaddr_in m_sin;

    struct sockaddr_in m_tmp;
public:
    CMaaUdpSocket(CMaaFdSockets * pFdSockets);
    void Send(const void * pBuffer, int BufferSize, _IP Ip, _Port Port);

    // returns size of received datagram
    int Recv(void * pBuffer, int BufferSize);

    int Recv(void * pBuffer, int BufferSize, _IP * pIp/*returns Ip of source*/, _Port * pPort = NULL /*returns port of source*/);

    // returns source IP address of the last received datagram
    _IP GetSourceIp();

    // returns source port of the last received datagram
    _Port GetSourcePort();

    // returns binded port
    _Port GetBindedPort();
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpClient : public CMaaUdpSocket
{
    _Port  m_Port;
    _IP    m_Ip;

public:
    CMaaUdpClient(CMaaFdSockets * pFdSockets);
    CMaaUdpClient(CMaaFdSockets * pFdSockets, _Port Port, _IP Ip = 0);
    CMaaUdpClient(CMaaFdSockets * pFdSockets, _Port Port, const char * DnsName);

    void Send(const void * pBuffer, int BufferSize, _IP Ip);
    void Send(const void * pBuffer, int BufferSize);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpBroadcast : public CMaaUdpClient
{
public:
    CMaaUdpBroadcast(CMaaFdSockets * pFdSockets, _Port Port = 0);

    void Send(const void * pBuffer, int BufferSize);
    void Send(const void * pBuffer, int BufferSize, _Port Port);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpServer : public CMaaUdpSocket
{
public:
    CMaaUdpServer(CMaaFdSockets * pFdSockets, _Port Port, _IP BindIp = 0 /*INADDR_ANY*/, bool bReuse = false);

    // For UDP multicast receiving
    void AddMembership(_IP MulticastAddr /*= CMaaIpAddress(224,0,0,0)*/, _IP InterfaceIpOrIndex = 0);

    // For UDP multicast sending
    void SetIpMulticastIfaceAndTTL(_IP IfaceIpOrIfIndex = 0, int ttl = 64); // use Sent//sendto internally// to send to multicast IP and port later
};
//---------------------------------------------------------------------------

// FD_SETSIZE == 64
#define CMaa_MAX_SOCKS  FD_SETSIZE


typedef CMaaSocket * pCMaaSocket;

#ifdef __unix__
bool CMaaSocketPair(CMaaSocket &s1, CMaaSocket &s2);
class CMaaWakeUpPair : public CMaaUdpSocket
{
    CMaaUdpSocket m_s2;
    int m_Bytes;
public:
    CMaaWakeUpPair(CMaaFdSockets *pFdSockets);
    ~CMaaWakeUpPair();
    int Notify_Read();
    int Notify_Write();
    //int Notify_Error ();
    const char * GetConnectionName ();
    void WakeUp();
};
#endif

extern int gSockThreads;

class CMaaSockThread : public CMaaThread
{
public:
    //CMaaAutoInitObject<int, 0> m_temp_SelectFlag;
    struct SCmd : public CTooDLink
    {
        int Cmd;
        CMaaSockThread * m_Par1;
        void * m_Par2;
        SCmd(int Command, CMaaSockThread * Par1 = NULL, void * Par2 = NULL)
        {
            Cmd = Command;
            m_Par1 = Par1;
            m_Par2 = Par2;
        }
    };
protected:
    CTooDList<SCmd> m_List;
    CTooDList<CMaaSockThread> m_ChildThreads;
    int m_Refs;
    bool m_bExiting;
#ifdef __unix__
    friend class CMaaWakeUpPair;
    CMaaWakeUpPair * m_WakeupPair;
#else
public:
    HANDLE m_hWakeup;
#endif
public:
    CMaaSockThread * m_pParent;
    CMaaFdSockets * m_pFdSockets;

    int Id;

    CMaaSockThread(CMaaSockThread * pParent);
    ~CMaaSockThread();

    int AddRef();
    int DelRef();
    virtual CMaaSockThread * CreateNewThread();
    void AddCommand(int Cmd, CMaaSockThread * Par1 = NULL, void * Par2 = NULL);
    void AddCommand(SCmd * c);
    virtual void OnCommand(SCmd * c) {}
    virtual void OnExitCommand(SCmd * c);
    unsigned Run();
    virtual void Process();
    virtual bool Process1();
    void WakeUp();
    void WakeUpChildThreads();
};
//---------------------------------------------------------------------------
class CMaaFdSockets : public CMaaEnum
{
    friend class CMaaTcpSocket;
    friend class CMaaUdpSocket;
    //     fd_set m_Read, m_Write, m_Exept;
    //public:
    struct timeval m_Timeval;

private:

    bool m_bForceMode;

    CMaaUnivHash < CMaaSocket *, int > m_SockHash;

    int IsWinStdioSock ( SOCKET s );

    CTooDList<CMaaSocketTimer> m_Timers;

    friend class CMaaSocketTimer;

protected:
    CMaaAutoInitObject<int, 0> m_SelectArraysUsedCount;
    CMaaPtr<CMaaSocket *> m_SelectSockets;
    CMaaPtr<int> m_SelectModes;
#ifdef _WIN32
    CMaaPtr<HANDLE> m_Select_hh;
#else
    CMaaPtr<bool> m_Select_rev, m_Select_wev;
#endif

public:
    CMaaAutoInitObject<int, 0> m_temp_SelectFlag;

    CMaaSockThread * m_pThread;

#ifdef __SOCK_NEW_TIMERS
    CMaaHeap<_qword, CMaaSocketTimer *> m_TimerHeap;
#endif

    CMaaFdSockets(CMaaSockThread * pThread);
    ~CMaaFdSockets();

    void Attach(CMaaSocketTimer * t);
    void Detach(CMaaSocketTimer * t);
    void PreTimer();
    void GetWaitForTime(int *us);

    int GetSockCount();
    int GetSockets(CMaaSocket ** m, int n);
    void AddSocket ( CMaaSocket * pSock, int Mode = eRead | eWrite | eExept );
    void RemoveSocket ( CMaaSocket * pSock );
    void SetTimeOut ( int Sec = 0, int MicroSec = 0 );
    int  Select ();
    void ChangeMode ( CMaaSocket * pSock, int Mode );
    int GetMode(CMaaSocket * pSock);

    void DeleteAllSockets ();
    void WakeUp();

    CMaaAutoInitObject<int, 0> m_C;
    void SetC(int c) { m_C = c; }
    bool log1(int point);
};

//---------------------------------------------------------------------------
class CMaaUnivServer : public CMaaTcpSocket
{
    virtual CMaaTcpSocket * CreateNewConnection(CMaaFdSockets * pFdSockets) = 0;
    int Notify_Read  ();
    int Notify_Write ();
    int Notify_Error ();

    CMaaString m_ServerName;
    const char * GetConnectionName ();

public:
    CMaaUnivServer(CMaaFdSockets * pFdSockets, int Port, const char * ServerName = NULL );
    CMaaUnivServer(CMaaFdSockets * pFdSockets, CMaaString strIpPort, const char * ServerName = NULL );
    ~CMaaUnivServer();
};
//---------------------------------------------------------------------------
// class T is deriver from CMaaTcpSocket, it indentify a connection
//---------------------------------------------------------------------------
template < class T > class CMaaServer : public CMaaUnivServer
{
    CMaaTcpSocket * CreateNewConnection(CMaaFdSockets * pFdSockets)
    {
        return TL_NEW T(pFdSockets, this);
    }
public:
    CMaaServer(CMaaFdSockets * pFdSockets, int Port, const char * ServerName = NULL )
    :   CMaaUnivServer(pFdSockets, Port, ServerName ? ServerName : "CMaaServer" )
    {
    }
};
//---------------------------------------------------------------------------
class CMaaSockConsole : public CMaaTcpSocket
{
    char m_Buff [ 1024 ];
    int  m_Pos;

    virtual int EnterString ( const char * str );
    virtual void Escape (); // for windows only

    int Notify_Read  ();
    int Notify_Write ();
    const char * GetConnectionName ();

public:
    CMaaSockConsole(CMaaFdSockets * pFdSockets);
    ~CMaaSockConsole();
};
//---------------------------------------------------------------------------
class CMaaEchoServer;
class CMaaEchoConnection : public CMaaTcpSocket
{
    char m_Buffer[4096];
    int m_pos, m_len;

    int Notify_Read  ();
    int Notify_Write ();
    int Notify_Error ();
    int Notify_Accepted ( _IP IpFrom, _Port Port );

public:
    CMaaEchoConnection(CMaaFdSockets * pFdSockets, CMaaEchoServer * pServer);
    ~CMaaEchoConnection();
};
//---------------------------------------------------------------------------
class CMaaEchoServer : public CMaaUnivServer
{
    CMaaString m_ServName;
    CMaaTcpSocket * CreateNewConnection(CMaaFdSockets * pFdSockets)
    {
        return TL_NEW CMaaEchoConnection(pFdSockets, this);
    }

public:
    CMaaEchoServer(CMaaFdSockets * pFdSockets, int Port)
    :   CMaaUnivServer(pFdSockets, Port, "EchoServer"),
        m_ServName("EchoServer")
    {
    }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// class CMaaTcpConnection
//---------------------------------------------------------------------------
class CMaaTcpConnection : public CMaaTcpSocket
{
public:
    struct Buffer : public CTooDLink
    {
        char *    m_pBuffer;
        int       m_BuffSize;
        int       m_Pos;

        Buffer ( int Sz, const void * Buff = NULL );
        ~Buffer ();
    };

    enum
    {
        eEmptyList     = 0, // nothing in the list
        eLastBuffer    = 1, // last buffer was sent, list is empty
        eToBeContinued = 2, // not all buffer was sent immediately
        eBufferSent    = 3, // there is(are) buffer(s) in the list
    };


    CTooDList < Buffer > m_SendList;

    const char * m_ClassName;
    CMaaString m_ConnName;

    const char * GetConnectionName ();
    void Send ( const char * Buff, int Size );
    void SetName ( const char * ConnName );
    CMaaTcpConnection(CMaaFdSockets * pFdSockets, const char * ClassName);
    ~CMaaTcpConnection ();
    int Notify_Accepted ( _IP IpFrom, _Port Port );
    int SendBuffer ();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// class CMaaTcpProxy
//---------------------------------------------------------------------------
class CMaaTcpProxy : public CMaaTcpConnection
{
    int fIsClientSide;
    CMaaTcpProxy * pOtherSide;
    CMaaString m_TcpServName;

    int fCloseByRead0;

    int fNeedToClose;

    CMaaSockTimerT<CMaaTcpProxy> m_Timer0;

    int Notify_Read  ();
    int Notify_Write ();
    //int Notify_Connected ( _IP Ip, _Port Port, const char * DnsName );

    static const char szClassName[];

    CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaTcpProxy * pClientSide, CMaaString & TcpServName );

public:
    CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaString & TcpServName);
    ~CMaaTcpProxy ();
    void OnTimer(int f);
};
//---------------------------------------------------------------------------
class CMaaTcpProxyServer : public CMaaUnivServer
{
    CMaaString m_ServName;
    CMaaTcpSocket * CreateNewConnection(CMaaFdSockets * pFdSockets);

public:
    CMaaTcpProxyServer(CMaaFdSockets * pFdSockets, int Port, const char * Server );
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class CMaaTcpStdoutConnection : public CMaaTcpConnection
{
    CMaaString m_TcpServName;

    CMaaSockTimerT<CMaaTcpStdoutConnection> m_Timer0;

    int Notify_Read  ();
    int Notify_Write ();
    //int Notify_Connected ( _IP Ip, _Port Port, const char * DnsName );

    static const char szClassName[];

    //CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaTcpProxy * pClientSide, CMaaString & TcpServName );

public:
    CMaaTcpStdoutConnection(CMaaFdSockets * pFdSockets, CMaaString & TcpServName);
    ~CMaaTcpStdoutConnection();
    void OnTimer(int f);
};
//---------------------------------------------------------------------------
class CMaaTcpStdoutServer : public CMaaUnivServer
{
    CMaaString m_ServName;
    CMaaTcpSocket * CreateNewConnection(CMaaFdSockets * pFdSockets);

public:
    CMaaTcpStdoutServer(CMaaFdSockets * pFdSockets, int Port);
};
//---------------------------------------------------------------------------

extern CMaaSockStartup * gpSockStartup;




//---------------------------------------------------------------------------
//CMaaUnivHash_CleanupData < SOCKET, HashData > m_ClientsHash;
//---------------------------------------------------------------------------

#else

/*
class ToolsExport CMaaSocket
{
};
*/

#endif // __TOOLS_USE_WINSOCK2   //#endif // __TOOLS_USE_WINSOCK || __TOOLS_USE_WINSOCK2

_qword htonq(_qword x);
_qword ntohq(_qword x);

//---------------------------------------------------------------------------
