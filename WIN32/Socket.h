//==============================================================================
// PRODUCT: ToolsLib
//==============================================================================
// FILE:       socket.h
//
// AUTHOR:     Andrey A. Moisseenko
//==============================================================================

typedef unsigned long  _IP;
typedef unsigned short _Port;

#ifdef _WIN32
#define socklen_t int
#endif

/*
#ifdef __unix__
#ifndef SOCKET
#define SOCKET int
#endif
#endif
*/


enum
{
    eNothing  = 0,
    eRead     = 1,
    eWrite    = 2,
    eExept    = 4,
    eConnect  = 8,
    eAll      = eRead | eWrite | eExept,
    eDisableShift = 8,
    eDisableRead    = eRead    << eDisableShift,
    eDisableWrite   = eWrite   << eDisableShift,
    eDisableExept   = eExept   << eDisableShift,
    eDisableConnect = eConnect << eDisableShift,
    eDisableAll     = (-1)     << eDisableShift
};

/*
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
*/

//---------------------------------------------------------------------------

#if defined(__TOOLS_USE_WINSOCK) || defined(__TOOLS_USE_WINSOCK2)

//---------------------------------------------------------------------------


// Name of system service
ToolsExport _PCSTR GetServByProtoAndPort ( _word PortValue, _PCSTR IpProtoName );

class CMaaSocket;
#ifndef __TOOLS_USE_WINSOCK2
//---------------------------------------------------------------------------
class ToolsExport CMaaSockStartup
{
#ifndef __unix__
    WSADATA m_WsaData;
    BOOL m_IsOk;
public:
    DWORD m_ErrorCode;
#endif
public:
    CMaaSockStartup ( BOOL fThrow = TRUE );
    ~CMaaSockStartup ();

    BOOL IsOk ();

    struct HashData
    {
        CMaaSocket * m_pSock;
        HWND m_Hwnd;
        unsigned int m_Msg;
        long m_Event;
    };
    CMaaUnivHash < SOCKET, HashData > m_Hash;

    HWND m_hWnd;
    unsigned int m_wMsg;

    void SetAsyncParams ( HWND hWnd, unsigned int wMsg )
    {
        m_hWnd = hWnd;
        m_wMsg = wMsg;
    }
    int AsyncSelect ( CMaaSocket * pSock, HWND hWnd, unsigned int wMsg, long lEvent = 0 );
    int AsyncSelect ( CMaaSocket * pSock, long lEvent = 0 )
    {
        return AsyncSelect ( pSock, m_hWnd, m_wMsg, lEvent );
    }
    int AsyncRemove ( SOCKET s );
    void DispatchMsg ( UINT sock, LPARAM lp );
};
//---------------------------------------------------------------------------
class ToolsExport CMaaSocket
{
public:
    SOCKET    m_Socket;
    long      m_TimeOut;
    CMaaSockStartup * m_Dispatcher;

    CMaaSocket ( int af, int type, int protocol );
    virtual ~CMaaSocket ();

    SOCKET GetSocket ();
    void   SetSocket ( SOCKET s );
    void SetTimeOut ( long MilliSeconds = -1 );
    void SetDispatcher ( CMaaSockStartup * pDispatcher );
    void AsyncSelect ( int Event );

    virtual int Notify_Read  ();
    virtual int Notify_Write ();
    virtual int Notify_Error ( int Err );
    virtual int Notify_Accepted ( _IP IpFrom, _Port Port );
    virtual int Notify_Connected ( _IP Ip, _Port Port, const char * DnsName );
    virtual int Notify_Closed ();
    virtual int Notify_TimeOut ();
    virtual int Notify_PreDeletingByExeption ();
    virtual CMaaString GetConnectionName () noexcept;

    void Error ( const char * msg );
    static void StaticError ( const char * msg );
    int CloseByExeption ( const char * Msg = nullptr, int Err = -1 );
    int Close ();

    //void AddFdSocket ();
    //void RemoveFdSocket ();

    static const char * GetLocalHostName ();
    static _IP GetLocalHostIp ();
    _IP GetHostByName ( const char * DnsName, _Port * Port = nullptr );
    char * GetHostByAddr ( _IP Ip, char * DnsName );
    void SetNonBlockingMode ();
    void SetReuseAddr ( BOOL IsOn = TRUE );
};

//---------------------------------------------------------------------------
//
//  TCP sockets & services
//
//---------------------------------------------------------------------------
class ToolsExport CMaaTcpSocket : public CMaaSocket
{
public:
    struct sockaddr_in m_sin;

    CMaaTcpSocket ();
    int SendData ( void * Buffer, int Size );
    int RecvData ( void * Buffer, int Size );
    int Connect ( _IP Ip, _Port Port );
    int Connect ( const char * DnsName, _Port Port = (_Port)-1 ); // DnsName can be as "www.ru:80"
    int AsyncConnect ( _IP Ip, _Port Port );
    int AsyncConnect ( const char * DnsName, _Port Port = (_Port)-1 ); // DnsName can be as "www.ru:80"
    int ConnectAgain ( int fCallNotify ); // for AsyncConnect, call it from fd_write
    int Bind ( _Port Port );                // 0 - ok
    int Listen ( int QueueDepth = 5 );      // 0 - ok
    SOCKET Accept ( _IP * IpFrom = nullptr, _Port * Port = nullptr );
    int GetBindedPort ();                  // returns binded port number
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpSocket : public CMaaSocket
{
protected:
    struct sockaddr_in m_sin;

public:
    CMaaUdpSocket ();
    void Send ( const void * pBuffer, int BufferSize, _IP Ip, _Port Port );
    void Bind ( _Port Port, struct sockaddr_in * psin = nullptr );
    void SetBroadcast ( BOOL fBroadcast = TRUE );
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpClient : public CMaaUdpSocket
{
    _Port  m_Port;
    _IP    m_Ip;

public:
    CMaaUdpClient ();
    CMaaUdpClient ( _Port Port, _IP Ip = 0 );
    CMaaUdpClient ( _Port Port, const char * DnsName );

    void Send ( const void * pBuffer, int BufferSize, _IP Ip );
    void Send ( const void * pBuffer, int BufferSize );
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpBroadcast : public CMaaUdpClient
{
public:
    CMaaUdpBroadcast ( _Port Port = 0 );

    void Send ( const void * pBuffer, int BufferSize );
    void Send ( const void * pBuffer, int BufferSize, _Port Port );
};
//---------------------------------------------------------------------------
class ToolsExport CMaaUdpServer : public CMaaUdpSocket
{
    struct sockaddr_in m_tmp;
public:
    CMaaUdpServer ( _Port Port, BOOL fReuseAddr = TRUE );

    // returns size of received datagram
    int Recv ( void * pBuffer, int BufferSize );

    int Recv ( void * pBuffer, int BufferSize, _IP * pIp, _Port * pPort = nullptr );

    // returns source IP address of the last received datagram
    _IP GetSourceIp ();

    // returns source port of the last received datagram
    _Port GetSourcePort ();

    // returns binded port
    _Port GetBindedPort ();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class ToolsExport CMaaUnivServer : public CMaaTcpSocket
{
    virtual CMaaTcpSocket * CreateNewConnection () = 0;
    int Notify_Read  () override;
    int Notify_Write () override;
    int Notify_Accepted ( _IP IpFrom, _Port Port ) override;
    int Notify_Error ( int Err ) override;

    CMaaString m_ServerName;
    CMaaString GetConnectionName () noexcept override;

public:
    CMaaUnivServer ( CMaaSockStartup * pDispatcher, int Port, const char * ServerName = nullptr );
    ~CMaaUnivServer ();
};
//---------------------------------------------------------------------------
// class T is deriver from CMaaTcpSocket, it indentify a connection
//---------------------------------------------------------------------------
template < class T > class CMaaServer : public CMaaUnivServer
{
    CMaaTcpSocket * CreateNewConnection ()
    {
        return TL_NEW T;
    }
public:
    CMaaServer ( int Port, const char * ServerName = nullptr )
    :   CMaaUnivServer ( nullptr, Port, ServerName ? ServerName : "CMaaServer" )
    {
    }
};
//---------------------------------------------------------------------------

#endif // __TOOLS_USE_WINSOCK2

#endif // #if defined(__TOOLS_USE_WINSOCK) || defined(__TOOLS_USE_WINSOCK2)
