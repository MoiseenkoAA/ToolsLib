//==============================================================================
// PRODUCT: ToolsLib, transferred from prev prj 1.04.2003
//==============================================================================
// FILE:       Socket.cpp
//
// AUTHOR:     Andrey A. Moisseenko  <support@maasoftware.ru>
//
// Old code
//
//==============================================================================

#include "perm.h"
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifdef _WIN32
//---------------------------------------------------------------------------

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
static void Trace ( char * msg, int s )
{
    printf ( msg, s );
    printf ( "\n" );
}

//---------------------------------------------------------------------------
#define ib( x ) ( ( int )( ip >> x ) & 0xff )
//---------------------------------------------------------------------------
const char * CMaaIpToText ( _IP ip, int Mode )
{
    static char txt [ 20 ];
    if  ( ! Mode )
    {
        sprintf ( txt, "%d.%d.%d.%d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
    }
    else
    {
        sprintf ( txt, "%03d.%03d.%03d.%03d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
    }
    return txt;
}
//---------------------------------------------------------------------------
char * CMaaIpToText ( char * txt, _IP ip, int Mode )
{
    if  ( txt )
    {
        if  ( ! Mode )
        {
            sprintf ( txt, "%d.%d.%d.%d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
        }
        else
        {
            sprintf ( txt, "%03d.%03d.%03d.%03d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
        }
    }
    return txt;
}
#undef ib
//---------------------------------------------------------------------------
char * CMaaIpToTextEx ( char * txt, _IP ip, int Mode )
{
    return CMaaIpToText ( txt, ip, Mode );
}
//---------------------------------------------------------------------------
// returns 0 -- means: Ok, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4" is converted to 0x01020304
int CMaaIpToLong ( const char * txt, _IP * RetIp )
{
    unsigned long ip = 0, tmp;
    int i;

    for ( i = 0; i < 4; i++ )
    {
        tmp = 0;
        if  ( txt[0] < '0' || txt[0] > '9' )
        {
            return -1;     // …££®£
        }
        while( txt[0] >= '0' && txt[0] <= '9' && tmp < 0x100 )
        {
            tmp = tmp * 10 + ( long )( txt[0] - '0' );
            ++txt;
        }
        if  ( tmp > 0xff )
        {
            return -1;
        }
        if  ( txt[0] != '.' && i < 3 )
        {
            return -1;
        }
        if  ( txt[0] != 0 && i == 3 )
        {
            return -1;
        }
        // Which byte-ordering do You need ?
        //ip = ( ip >> 8 ) + ( tmp << 24 ); // Network byte order // for Intel-like processors with low-to-hight byte ordering
        ip = ( ip << 8 ) + tmp;
        ++txt;
    }
    * RetIp = ip;
    return 0;         // All Ok
}
//---------------------------------------------------------------------------
// returns len of ip text, -1 on error, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4-1.2.3.55" is converted to 0x01020304, ret=7
int CMaaIpToLongEx ( const char * txt, _IP * RetIp, const char * AvailableTrailers )
{
    unsigned long ip = 0, tmp;
    int i;
    const char * txt0 = txt;

    for ( i = 0; i < 4; i++ )
    {
        tmp = 0;
        if  ( txt[0] < '0' || txt[0] > '9' )
        {
            return -1;     // …££®£
        }
        while( txt[0] >= '0' && txt[0] <= '9' && tmp < 0x100 )
        {
            tmp = tmp * 10 + ( long )( txt[0] - '0' );
            ++txt;
        }
        if  ( tmp > 0xff )
        {
            return -1;
        }
        if  ( txt[0] != '.' && i < 3 )
        {
            return -1;
        }
        if  ( txt[0] != 0 && !strchr ( AvailableTrailers, txt [ 0 ] ) && i == 3 )
        {
            return -1;
        }
        // Which byte-ordering do You need ?
        //ip = ( ip >> 8 ) + ( tmp << 24 ); // Network byte order // for Intel-like processors with low-to-hight byte ordering
        ip = ( ip << 8 ) + tmp;
        ++txt;
    }
    * RetIp = ip;
    return (int)(size_t)(txt - 1 - txt0);         // All Ok
}
//---------------------------------------------------------------------------
_IP CMaaIpAddress ( _byte a, _byte b, _byte c, _byte d )
{
    return ( ( _IP )( a ) << 24 ) | ( ( _IP )( b ) << 16 ) | ( ( _IP )( c ) <<  8 ) | ( _IP )( d );
}
//---------------------------------------------------------------------------
_IP CMaaGetMaskByNum(int Num)
{
    if  (!Num)
    {
        return 0;
    }
    return ~ ((1 << (32 - Num)) - 1);
}
//---------------------------------------------------------------------------
int CMaaGetNumByMask(_IP Mask)
{
    _IP m = 0;
    for (int i = 0; i <= 32; i++)
    {
        if  (m == Mask)
        {
            return i;
        }
        m = (m >> 1) | 0x80000000;
    }
    return -1;
}
//---------------------------------------------------------------------------
#endif

#if defined(__TOOLS_USE_WINSOCK) || defined(__TOOLS_USE_WINSOCK2)

//---------------------------------------------------------------------------
#ifndef TOOLSLIB_SINGLE_THREAD
static CMaaSockStartup * gpSockStartup = nullptr;
#endif
//---------------------------------------------------------------------------

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
//---------------------------------------------------------------------------
// Name of system service
//---------------------------------------------------------------------------
_PCSTR GetServByProtoAndPort ( _word PortValue, _PCSTR IpProtoName )
{
    _PSTR pNameServ = nullptr;
    int Port = ( int ) htons (  ( u_short ) PortValue );
    servent * pServent = getservbyport ( Port, IpProtoName );
    if  ( pServent )
    {
        if  ( pServent -> s_name )
        {
            pNameServ = pServent -> s_name;
        }
    }

    return pNameServ;
}
#endif

//#define CMaaSocket CMaaSocket1
//---------------------------------------------------------------------------

#ifndef __TOOLSLIB_REMOVE_OLD_CODE

#ifndef __TOOLS_USE_WINSOCK2

CMaaSocket::CMaaSocket ( int af, int type, int protocol )
:   m_Socket ( INVALID_SOCKET ),
    m_TimeOut ( 0 ),
    m_Dispatcher ( nullptr )
{
    m_Socket = socket ( af, type, protocol );
    //_f_ ();
    if  ( m_Socket == INVALID_SOCKET )
    {
        Error ( "socket ()" );
    }
}
//---------------------------------------------------------------------------
CMaaSocket::~CMaaSocket ()
{
    if  ( m_Socket != INVALID_SOCKET  )
    {
        AsyncSelect ( 0 );  // gpSockStartup->AsyncRemove ( m_Socket );
        shutdown ( m_Socket, 2 );
        closesocket ( m_Socket );
    }
    //RemoveFdSocket ();
}
//---------------------------------------------------------------------------
void CMaaSocket::Error(const char * msg)
{
    //CMaaString Msg;
    //int Error = WSAGetLastError();
    //
    //Msg.Format(IDS_XTOOSockError, msg, Error);
    //throw XTOOSocketError(Msg, Error, DelObject);
    //throw msg;
    throw XTOOSockErr(msg, nullptr, GetConnectionName());
    //CloseByExeption((char *) msg);
}
//---------------------------------------------------------------------------
void CMaaSocket::StaticError(const char * msg)
{
    throw XTOOSockErr(msg, nullptr, nullptr);
}
//---------------------------------------------------------------------------
SOCKET CMaaSocket::GetSocket ()
{
    return m_Socket;
}
//---------------------------------------------------------------------------
void CMaaSocket::SetSocket ( SOCKET s )
{
    if  ( m_Socket != INVALID_SOCKET && m_Socket != s )
    {
        AsyncSelect ( 0 ); // gpSockStartup->AsyncRemove ( m_Socket );
        shutdown ( m_Socket, 2 );
        closesocket ( m_Socket );
    }
    m_Socket = s;
}
//---------------------------------------------------------------------------
int CMaaSocket::CloseByExeption ( const char * Msg, int Err )
{
    // ending connection
    //DeleteSocket Err ( msg, this );
    //throw Err;
    throw XTOOSockErr ( Msg, this, GetConnectionName () );
    return eDisableAll;
}
//---------------------------------------------------------------------------
/*void CMaaSocket::AddFdSocket ()
{
     m_pFdSockets -> AddSocket ( this );
}*/
//---------------------------------------------------------------------------
/*void CMaaSocket::RemoveFdSocket ()
{
     m_pFdSockets -> RemoveSocket ( this );
}*/
//---------------------------------------------------------------------------
void CMaaSocket::SetTimeOut ( long MilliSeconds )
{
    if  ( MilliSeconds == -1 )
    {
        m_TimeOut = 0;
    }
    else
    {
        m_TimeOut = GetTickCount () + MilliSeconds;
    }
}
//---------------------------------------------------------------------------
void CMaaSocket::AsyncSelect ( int Event )
{
    if  ( m_Dispatcher )
    {
        m_Dispatcher->AsyncSelect ( this, Event );
    }
}
//---------------------------------------------------------------------------
void CMaaSocket::SetDispatcher ( CMaaSockStartup * pDispatcher )
{
    m_Dispatcher = pDispatcher;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_TimeOut ()
{
    // under constructions
    return 0;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Closed ()
{
    AsyncSelect ( 0 );
    Close (); // Connection is closed by other side (normal close operation)
    return 0;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_PreDeletingByException ()
{
    AsyncSelect ( 0 ); // So you can use message box to display error message. This avoids the next messages to be processed by CMaaSocket
    return 0;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Read ()
{
    return eDisableRead;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Write ()
{
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Connected ( _IP Ip, _Port Port, const char * DnsName )
{
    return 0;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Error ( int Err )
{
    return CloseByExeption ( "Socket error occured", Err );
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Accepted ( _IP IpFrom, _Port Port )
{
    return 0;
}
//---------------------------------------------------------------------------
CMaaString CMaaSocket::GetConnectionName() noexcept
{
    return CMaaStringZ;
}
//---------------------------------------------------------------------------
_IP CMaaSocket::GetHostByName ( const char * DnsName, _Port * pPort )
{
    char * p = strchr ( (char *)DnsName, ':' );
    if  ( p && pPort )
    {
        int Port = 0;
        dsscanf ( p + 1, "%d", &Port );
        * pPort = Port;
    }
    CMaaBuffer txt ( DnsName );
    if  ( p )
    {
        // making DNS name with out of port specified after ':'
        txt [ p - DnsName ] = 0;
        DnsName = txt;
    }

    _IP Ip = 0;
    if  ( CMaaIpToLong ( DnsName, &Ip ) )
    {
        // It is not numerical address as name given
        struct hostent * h = gethostbyname ( DnsName );
        if  ( !h )
        {
            CMaaBuffer txt ( 40 + strlen ( DnsName ) );
            sprintf ( txt, "gethostbyname (\"%s\")", DnsName );
            throw XTOOSockDnsErr ( txt, nullptr, GetConnectionName () );
            //Error ( txt );
            return 0;
        }
        Ip = htonl ( * ( _IP * ) h -> h_addr_list [ 0 ] );
    }
    return Ip;
}
//---------------------------------------------------------------------------
char * CMaaSocket::GetHostByAddr ( _IP Ip, char * DnsName )
{
    _IP ip = htonl ( Ip );
    struct hostent * h = gethostbyaddr ( (char*)&ip, sizeof(ip), AF_INET );
    if  ( !h )
    {
        char txt [ 128 ];
        char pip [ 64 ];
        sprintf ( txt, "gethostbyaddrname (\"%s\")", CMaaIpToText ( pip, Ip, 0 ) );
        throw XTOOSockDnsErr ( txt, nullptr, GetConnectionName () );
        //Error ( txt );
        return 0;
    }
    strcpy ( DnsName, h -> h_name );
    return DnsName;
}
//---------------------------------------------------------------------------
void CMaaSocket::SetReuseAddr ( BOOL IsOn )
{
#ifndef __unix__
    BOOL On = IsOn;
#define P_PARAM123 (const char *)
#else
    int On = IsOn;
#define P_PARAM123
#endif
    setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM123 &On, sizeof ( On ) );
#undef P_PARAM123
}
//---------------------------------------------------------------------------
int CMaaSocket::Close()
{
    //shutdown(m_Socket, 0);
    throw XTOOSockClose(this);
    //return CloseByException("Closing connection");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaTcpSocket::CMaaTcpSocket ()
//:    CMaaSocket ( AF_INET, SOCK_STREAM, 0 )
:   CMaaSocket ( AF_INET, SOCK_STREAM, IPPROTO_TCP )
{
    memset ( &m_sin, 0, sizeof ( m_sin ) );

    // Variable to set the SO_KEEPALIVE option with setsockopt ().
    BOOL fBroadcast = TRUE;

    if  ( SOCKET_ERROR == setsockopt ( m_Socket, SOL_SOCKET, SO_KEEPALIVE, ( char * ) & fBroadcast, sizeof ( BOOL ) ) )
    {
        Error ( "setsockopt ()" );
    }
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::SendData ( void * Buffer, int Size )
{
    int Total = 0;
    int ret = 0;

    while( Size > 0 )
    {
        ret = send ( m_Socket, ( char * ) Buffer, Size, 0 );

        if  ( ret == SOCKET_ERROR )
        {
            int Err;
            if  ( ( Err = WSAGetLastError () ) != WSAEWOULDBLOCK )
            {
                CloseByExeption ( "send ()", Err );
            }
            break;
        }
        else
        {
            Total  += ret;
            Size   -= ret;
            Buffer = ret + ( char * ) Buffer;
        }
        break; //!!!!!!!!!!!!!!
    }
    //char txt [ 256 ];
    //sprintf ( txt, "SendData() %d bytes", Total );
    //Trace ( txt );
    return Total;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::RecvData ( void * Buffer, int Size )
{
    int Total = 0;
    int ret = 0;

    while( Size > 0 )
    {
        ret = recv ( m_Socket, ( char * ) Buffer, Size, 0 );
        if  ( ret == SOCKET_ERROR )
        {
            int Err;
            if  ( ( Err = WSAGetLastError () ) != WSAEWOULDBLOCK )
            {
                CloseByExeption ( "recv ()", Err );
                //vAsyncMsg ( m_Socket, FD_READ, err );
            }
            break;
        }
        else if ( ! ret )
        {
            break;
        }
        else
        {
            Total  += ret;
            Size   -= ret;
            Buffer = ret + ( char * ) Buffer;
        }
        break; //!!!!!!!!!!!!!!
    }
    //char txt [ 256 ];
    //sprintf ( txt, "RecvData() %d bytes", Total );
    //Trace ( txt );
    return Total;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::ConnectAgain ( int fCallNotify )
{
    //IfDebug ( 200 ) printf ( "CMaaTcpSocket::ConnectAgain ()\n" );
    _IP Ip = htonl ( m_sin.sin_addr.s_addr );
    _Port Port = htons ( m_sin.sin_port );
    char pip [ 30 ];
    //fflush(stdout);
    int x;
    if  ( (x=connect ( m_Socket, ( struct sockaddr * ) & m_sin, sizeof ( m_sin ) )) )
    {
#ifdef __unix__
#define __CA_ERRNO       errno
        //#define __CA_EISCONN     EISCONN
        //#define __CA_EINPROGRESS EINPROGRESS
        //#define __CA_EALREADY    EALREADY

#define __CA_EISCONN     -123456
#define __CA_EINPROGRESS 115
#define __CA_EALREADY    -123457
#else
#define __CA_ERRNO       WSAGetLastError ()
#define __CA_EINPROGRESS WSAEWOULDBLOCK
#define __CA_EALREADY    WSAEALREADY
#define __CA_EISCONN     WSAEISCONN
#endif
        if  (0)
        {
            printf ( "connect returns %d errno=%d", x, errno );
            XTOOSockErr Err ( "error=", nullptr );
            fflush(stdout);
        }
        switch( __CA_ERRNO )
        {
        case __CA_EINPROGRESS:
        case __CA_EALREADY:
            return fCallNotify ? 0 : 2;

        case __CA_EISCONN:
            if  ( fCallNotify )
            {
                //ChangeFdMode ( eDisableConnect );
                return Notify_Connected ( Ip, Port, CMaaIpToText ( pip, Ip, 0 ) );
            }
            return fCallNotify ? 0 : 3;
        default:
            {
                printf ( "\n\n_____connect returns %d errno=%d", x, errno );
                {XTOOSockErr Err ( "error=", nullptr );}
                printf ( "_____\n" );
                fflush(stdout);
            }
        }

        char txt [ 100 ];
        sprintf ( txt, "connect (%s, %d)", CMaaIpToText ( pip, Ip, 0 ), Port );
        Error ( txt );
        return fCallNotify ? 0 : 4;
    }
    else
    {
        //printf ( "connect returns %d\n", x );
        //fflush(stdout);
    }
    if  ( fCallNotify )
    {
        //ChangeFdMode ( eDisableConnect );
        Notify_Connected ( Ip, Port, CMaaIpToText ( pip, Ip, 0 ) );
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect ( _IP Ip, _Port Port )
{
    //IfDebug ( 100 ) printf ( "CMaaTcpSocket::Connect ( %s, %d )\n", CMaaIpToText ( Ip ), Port );
    memset ( & m_sin, 0, sizeof ( m_sin ) );
    m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons ( Port );
    m_sin.sin_addr.s_addr = htonl ( Ip );
    int Ret = ConnectAgain ( 0 );
    if  ( Ret >= 3 )
    {
        char pip [ 30 ];
        char txt [ 100 ];
        sprintf ( txt, "connect (%s, %d)", CMaaIpToText ( pip, Ip, 0 ), Port );
        Error ( txt );
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect ( const char * DnsName, _Port Port )
{
    //IfDebug ( 100 ) printf ( "CMaaTcpSocket::Connect ( \"%s\", %d )\n", DnsName, Port );
    _IP Ip = GetHostByName ( DnsName, &Port );
    return Connect ( Ip, Port );
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect ( _IP Ip, _Port Port )
{
    //RemoveFdSocket ();
    //ChangeFdMode ( eConnect );
    SetNonBlockingMode ();
    Connect ( Ip, Port );
    //ConnectTask * Task = TL_NEW ConnectTask ( GetSocket (), this, Ip, Port );
    //if   ( Task )
    //{
    //     gCMaaConnectTasks -> AddTask ( Task );
    //     return 0;
    //}
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect ( const char * DnsName, _Port Port )
{
    //RemoveFdSocket ();
    SetNonBlockingMode ();
    //ChangeFdMode ( eConnect );
    Connect ( DnsName, Port );
    //ConnectTask * Task = TL_NEW ConnectTask ( GetSocket (), this, DnsName, Port );
    //if   ( Task )
    //{
    //     gCMaaConnectTasks -> AddTask ( Task );
    //     return 0;
    //}
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Bind ( _Port Port )
{
    //IfDebug ( 100 ) printf ( "CMaaTcpSocket::Bind ( %d )\n", Port );
    m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons ( Port );
    m_sin.sin_addr.s_addr = INADDR_ANY;
#ifndef __unix__
    BOOL On = true;
#define P_PARAM (const char *)
#else
    int On = 1;
#define P_PARAM
#endif
    setsockopt ( m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof ( On ) );
#undef P_PARAM
    if  ( SOCKET_ERROR == bind ( m_Socket, ( struct sockaddr * ) & m_sin, sizeof ( m_sin ) ) )
    {
        Error ( "bind ()" );
        return 2;
    }
    socklen_t len = sizeof ( m_sin );
    if  ( getsockname ( m_Socket, ( struct sockaddr * ) & m_sin, &len ) )
    {
        Error ( "getsockname()" );
        return 22;
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::GetBindedPort ()
{
    socklen_t len = sizeof ( m_sin );
    if  ( getsockname ( m_Socket, ( struct sockaddr * ) & m_sin, &len ) )
    {
        Error ( "getsockname()" );
        return 22;
    }
    return htons ( m_sin.sin_port );
}
//---------------------------------------------------------------------------
_IP CMaaSocket::GetLocalHostIp ()
{
    char txt [ 256 ];
    txt [ 0 ] = 0;
    gethostname ( txt, sizeof ( txt ) );
    struct hostent * h = gethostbyname ( txt );
    if  ( *txt == 0 )
    {
        strcpy ( txt, "<unknown>" );
    }
    if  ( !h )
    {
        char msg [ 512 ];
        sprintf ( msg, "gethostbyname(\"%s\")", txt );
        throw XTOOSockDnsErr ( msg, nullptr, "CMaaSocket::GetLocalHostIp ()" );
        //StaticError ( "gethostbyname () in CMaaSocket::GetLocalHostIp ()" );
        return 1; // 1?
    }
    /*
     printf ( "Host name: %s", txt );
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
    return htonl ( * ( _IP * ) h -> h_addr_list [ 0 ] );
}
//---------------------------------------------------------------------------
const char * CMaaSocket::GetLocalHostName ()
{
    static char txt [ 256 ];
    txt [ 0 ] = 0;
    gethostname ( txt, sizeof ( txt ) );
    struct hostent * h = gethostbyname ( txt );
    if  ( *txt == 0 )
    {
        strcpy ( txt, "<unknown>" );
    }
    return txt;
}
//---------------------------------------------------------------------------
void CMaaSocket::SetNonBlockingMode ()
{
#ifdef __unix__
    if  ( fcntl ( m_Socket, F_SETFL, O_NONBLOCK ) )
    {
        Error ( "CMaaSocket::SetNonBlockingMode ()" );
        //Error ( "fcntl F_SETFL O_NONBLOCK failed" );
    }
#else
    long cmd = FIONBIO;
    u_long IsNonBlocking = 1;     // zero for disabling
    if  ( ioctlsocket ( m_Socket, cmd, &IsNonBlocking ) )
    {
        Error ( "CMaaSocket::SetNonBlockingMode ()" );
    }
#endif
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Listen ( int QueueDepth )
{
    //IfDebug ( 100 ) printf ( "CMaaTcpSocket::Listen ()\n" );
    if  ( SOCKET_ERROR == listen ( m_Socket, QueueDepth ) )
    {
        Error ( "listen ()" );
        return 3;
    }
    return 0;
}
//---------------------------------------------------------------------------
SOCKET CMaaTcpSocket::Accept ( _IP * IpFrom, _Port * Port )
{
    //IfDebug ( 100 ) printf ( "CMaaTcpSocket::Accept () " );
    struct sockaddr_in si;
    socklen_t len;
    memset ( &si, 0, len = sizeof ( si ) );
    SOCKET s = accept ( GetSocket (), (struct sockaddr *) & si, & len );

    if  ( s == SOCKET_ERROR )
    {
        Error ( "accept ()" );
    }
    if  ( IpFrom )
    {
        * IpFrom = htonl ( si.sin_addr.s_addr );
    }
    if  ( Port )
    {
        * Port = htons ( si.sin_port );
    }
    //IfDebug ( 100 ) printf ( "accept from %s:%d\n", CMaaIpToText ( htonl ( si.sin_addr.s_addr ) ), htons ( si.sin_port ) );
    return s;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpSocket::CMaaUdpSocket ()
:   CMaaSocket ( AF_INET, SOCK_DGRAM, 0 )
    //:    CMaaSocket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP )  // may be more correct. need to test
{
    memset ( & m_sin, 0, sizeof ( m_sin ) );
}
//---------------------------------------------------------------------------
void CMaaUdpSocket::Send ( const void * pBuffer, int BufferSize, _IP Ip, _Port Port )
{
    m_sin.sin_family         = AF_INET;
    m_sin.sin_port           = htons ( Port );
    m_sin.sin_addr.s_addr    = htonl ( Ip );
    if  ( SOCKET_ERROR == sendto ( m_Socket, ( char * ) pBuffer, BufferSize, 0, (struct sockaddr *) & m_sin, sizeof ( m_sin ) ) )
    {
        Error ( "sendto ()" );
    }
}
//---------------------------------------------------------------------------
void CMaaUdpSocket::Bind ( _Port Port, struct sockaddr_in * p )
{
    struct sockaddr_in l_sin;
    if  ( !p )
    {
        p = &l_sin;
    }
    memset ( p, 0, sizeof ( l_sin ) );
    p->sin_family = AF_INET;
    p->sin_port   = htons ( Port );
    p->sin_addr.s_addr = htonl ( INADDR_ANY );

    if  ( SOCKET_ERROR == bind ( m_Socket, (struct sockaddr *) p, sizeof ( l_sin ) ) )
    {
        Error ( "bind ()" );
    }
}
//---------------------------------------------------------------------------
void CMaaUdpSocket::SetBroadcast ( BOOL fBroadcast )
{
    // Variable to set the broadcast option with setsockopt ().
    //BOOL fBroadcast = TRUE;

    if  ( SOCKET_ERROR == setsockopt ( m_Socket, SOL_SOCKET, SO_BROADCAST, ( char * ) & fBroadcast, sizeof ( BOOL ) ) )
    {
        Error ( "setsockopt (,,SO_BROADCAST,,)" );
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient ( _Port Port, _IP Ip )
:   m_Port ( Port ),
    m_Ip   ( Ip )
{
}
//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient ( _Port Port, const char * DnsName )
:   m_Port ( Port )
{
    m_Ip = GetHostByName ( DnsName );
}
//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient ()
:   m_Port ( 0 ),
    m_Ip   ( 0 )
{
}
//---------------------------------------------------------------------------
void CMaaUdpClient::Send ( const void * pBuffer, int BufferSize )
{
    CMaaUdpSocket::Send ( pBuffer, BufferSize, m_Ip, m_Port );
}
//---------------------------------------------------------------------------
void CMaaUdpClient::Send ( const void * pBuffer, int BufferSize, _IP Ip )
{
    CMaaUdpSocket::Send ( pBuffer, BufferSize, Ip, m_Port );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpBroadcast::CMaaUdpBroadcast ( _Port Port )
:   CMaaUdpClient ( Port, ( _IP ) 0 )
{
    // Variable to set the broadcast option with setsockopt ().
    BOOL fBroadcast = TRUE;

    if  ( SOCKET_ERROR == setsockopt ( m_Socket, SOL_SOCKET, SO_BROADCAST, ( char * ) & fBroadcast, sizeof ( BOOL ) ) )
    {
        Error ( "setsockopt (,,SO_BROADCAST,,)" );
    }
}
//---------------------------------------------------------------------------
void CMaaUdpBroadcast::Send ( const void * pBuffer, int BufferSize )
{
    CMaaUdpClient::Send ( pBuffer, BufferSize, INADDR_BROADCAST );
}
//---------------------------------------------------------------------------
void CMaaUdpBroadcast::Send ( const void * pBuffer, int BufferSize, _Port Port )
{
    CMaaUdpSocket::Send ( pBuffer, BufferSize, INADDR_BROADCAST, Port );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpServer::CMaaUdpServer ( _Port Port, BOOL fReuseAddr )
{
    //IfDebug ( 100 ) printf ( "CMaaUdpServer::CMaaUdpServer ( %d )\n", Port );
    memset ( &m_sin, 0, sizeof ( m_sin ) );
    m_sin.sin_family = AF_INET;
    m_sin.sin_port   = htons ( Port );
    m_sin.sin_addr.s_addr = htonl ( INADDR_ANY );

    SetReuseAddr ( fReuseAddr );
    if  ( SOCKET_ERROR == bind ( m_Socket, (struct sockaddr *) & m_sin, sizeof ( m_sin ) ) )
    {
        Error ( "bind ()" );
    }
    //AddFdSocket ();
}
//---------------------------------------------------------------------------
int CMaaUdpServer::Recv ( void * pBuffer, int BufferSize )
{
    socklen_t len = sizeof ( m_tmp );

    memset ( &m_tmp, 0, sizeof ( m_tmp ) );
    int Ret = recvfrom ( m_Socket, ( char * ) pBuffer, BufferSize, 0, ( struct sockaddr * ) & m_tmp, & len );
    if  ( Ret == SOCKET_ERROR )
    {
        Error ( "recvfrom ()" );
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaUdpServer::Recv ( void * pBuffer, int BufferSize, _IP * pIp, _Port * pPort )
{
    int len = Recv ( pBuffer, BufferSize );
    if  ( pIp )
    {
        * pIp = GetSourceIp ();
    }
    if  ( pPort )
    {
        * pPort = GetSourcePort ();
    }
    return len;
}
//---------------------------------------------------------------------------
_IP CMaaUdpServer::GetSourceIp ()
{
    return htonl ( m_tmp.sin_addr.s_addr );
}
//---------------------------------------------------------------------------
_Port CMaaUdpServer::GetSourcePort ()
{
    return htons ( m_tmp.sin_port );
}
//---------------------------------------------------------------------------
_Port CMaaUdpServer::GetBindedPort ()
{
    return htons ( m_sin.sin_port );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CMaaSockStartup::CMaaSockStartup ( BOOL fThrow )
{
#ifndef __unix__
    m_IsOk = FALSE;
    m_ErrorCode = 0;
    if  ( WSAStartup ( 0x101, & m_WsaData ) == SOCKET_ERROR )
    {
        //LastWsaError = WSAGetLastError ();
        m_ErrorCode = WSAGetLastError ();
        if  ( fThrow )
        {
            throw XTOOSockErr ( "WSAStartup failed", nullptr );
        }
    }
    else
    {
        m_IsOk = TRUE;
        gpSockStartup = this;
    }
#else
    m_IsOk = TRUE;
#endif
    //gCMaaFdSockets = TL_NEW CMaaFdSockets;
}
//---------------------------------------------------------------------------
CMaaSockStartup::~CMaaSockStartup ()
{
    //delete gCMaaFdSockets;
    //gCMaaFdSockets = nullptr;
#ifndef __unix__
    WSACleanup ();
#endif
}
//---------------------------------------------------------------------------
BOOL CMaaSockStartup::IsOk ()
{
    return m_IsOk;
}
//---------------------------------------------------------------------------
int CMaaSockStartup::AsyncSelect ( CMaaSocket * pSock, HWND hWnd, unsigned int wMsg, long lEvent )
{
    if  ( !pSock )
    {
        return SOCKET_ERROR;
    }
    pSock->SetDispatcher ( this );
    int Ret = ::WSAAsyncSelect ( pSock->GetSocket (), hWnd, wMsg, lEvent );
    if  ( !lEvent )
    {
        return AsyncRemove ( pSock->GetSocket () );
    }
    if  ( Ret != SOCKET_ERROR )
    {
        HashData Data;
        Data.m_pSock = pSock;
        Data.m_Hwnd = hWnd;
        Data.m_Msg = wMsg;
        Data.m_Event = lEvent;
        return m_Hash.AddOver ( pSock->GetSocket (), Data );
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaSockStartup::AsyncRemove ( SOCKET s )
{
    return m_Hash.Remove ( s );
}
//---------------------------------------------------------------------------
void CMaaSockStartup::DispatchMsg ( UINT sock, LPARAM lp )
{
    HashData Data;
    if  ( m_Hash.Find ( sock, &Data ) )
    {
        // handler not found
        return;
    }
    int err = WSAGETSELECTERROR(lp);
    int evn = WSAGETSELECTEVENT(lp);
    int Ret = 0;
    if  ( err )
    {
        Ret = Data.m_pSock->Notify_Error ( err );
    }
    else
    {
        switch( evn )
        {
        case FD_READ:
            Ret = Data.m_pSock->Notify_Read  ();
            break;
        case FD_WRITE:
            Ret = Data.m_pSock->Notify_Write ();
            break;
        case FD_ACCEPT:
            Ret = Data.m_pSock->Notify_Accepted ( 0, 0 );
            break;
        case FD_CONNECT:
            Ret = Data.m_pSock->Notify_Connected ( 0, 0, "" );
            break;
        case FD_CLOSE:
            Ret = Data.m_pSock->Notify_Closed ();
            break;
        }
    }
    int EvntMask = Data.m_Event;
    if  ( Ret & eDisableRead )
    {
        EvntMask &= FD_READ;
    }
    if  ( Ret & eDisableWrite )
    {
        EvntMask &= FD_WRITE;
    }
    if  ( Ret & eDisableConnect )
    {
        EvntMask &= FD_CONNECT;
    }
    if  ( EvntMask != Data.m_Event )
    {
        AsyncSelect ( Data.m_pSock, Data.m_Hwnd, Data.m_Msg, EvntMask );
    }
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Servers
//---------------------------------------------------------------------------
CMaaUnivServer::CMaaUnivServer ( CMaaSockStartup * pDispatcher, int Port, const char * ServerName )
//:    m_ServerName ( ServerName, 50 )
{
    ServerName = ServerName ? ServerName : "CMaaUniversalServer";
    m_ServerName.Format ( "%s on port %d", ServerName, Port );
    if  ( !pDispatcher )
    {
        pDispatcher = gpSockStartup;
    }
    pDispatcher->AsyncSelect ( this, FD_ACCEPT );
    Bind ( Port );
    Listen ();
    //AddFdSocket ();
}
//---------------------------------------------------------------------------
CMaaUnivServer::~CMaaUnivServer ()
{
    //RemoveFdSocket ();
}
//---------------------------------------------------------------------------
//CMaaTcpSocket * CMaaUnivServer::CreateNewConnection ()
//{
//     return nullptr;
//}
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Read ()
{
    return eDisableRead;
}
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Accepted ( _IP IpFrom, _Port Port )
{
    //_IP IpFrom = 0;
    //_Port Port = 0;
    SOCKET s = INVALID_SOCKET;
    CMaaTcpSocket * pConnection = nullptr;
    try
    {
        s = Accept ( &IpFrom, &Port );
        pConnection = CreateNewConnection ();
    }
    catch( ... )
    {
        if  ( s != INVALID_SOCKET )
        {
            closesocket ( s );
        }
        throw;
    }
    if  ( ! pConnection )
    {
        closesocket ( s );
    }
    else
    {
        pConnection -> SetSocket ( s );
        //pConnection -> AddFdSocket ();
        //pConnection -> ChangeFdMode ( pConnection -> Notify_Accepted ( IpFrom, Port ) );
        if  ( m_Dispatcher )
        {
            //pConnection->SetDispatcher ( m_Dispatcher );
            m_Dispatcher->AsyncSelect ( pConnection, FD_READ | FD_WRITE | FD_CLOSE );
        }
        return pConnection -> Notify_Accepted ( IpFrom, Port );
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Write ()
{
    // server does not perform write operations
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Error ( int Err )
{
    CloseByExeption ( "Error in listened socket", Err );
    return 0;
}
//---------------------------------------------------------------------------
CMaaString CMaaUnivServer::GetConnectionName() noexcept
{
    return m_ServerName;
}
//---------------------------------------------------------------------------


#if 0
//---------------------------------------------------------------------------
int CMaaEchoServer::Notify_Read  ()
{
    if  ( !r )
    {
        r = RecvData ( pR, BUFSIZE );
        if  ( !r )
        {
            // ending connection
            //printf ( "\nReceiving 0 bytes of data - closing connection.\n" );
            return CloseByExeption ( "Receiving 0 bytes of data - closing connection.");
        }
        pR [ r ] = 0;
        printf ( "%s", pR );
        if  ( !w_len )
        {
            char * swp = pW; pW = pR; pR = swp;
            w_len = r; w = r = 0;
            return eWrite;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaEchoServer::Notify_Write ()
{
    //if   ( w_len )
    {
        w += SendData ( pW + w, w_len - w );
        if  ( w >= w_len )
        {
            w = w_len = 0;
            return eDisableWrite;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaEchoServer::Notify_Error ()
{
    //printf ( "\nSome error in Echo server...\n" );
    CloseByExeption ( "Some error in Echo server..." );
    return 0;
}
//---------------------------------------------------------------------------
int CMaaEchoServer::Notify_Accepted ( _IP IpFrom, _Port Port )
{
    printf ( "\nAccepted connection from %s:%d\n", CMaaIpToText ( IpFrom ), Port );
    //SendData ( "", 0 );
    return 0;
}
//---------------------------------------------------------------------------
CMaaEchoServer::CMaaEchoServer ()
:   r ( 0 ),
    w ( 0 ),
    w_len ( 0 )
{
    pR = Buffers [ 0 ];
    pW = Buffers [ 1 ];
}
//---------------------------------------------------------------------------
CMaaEchoServer::~CMaaEchoServer ()
{
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// class CMaaTcpConnection
//---------------------------------------------------------------------------
CMaaTcpConnection::Buffer::Buffer ( int Sz, const void * Buff )
:   m_BuffSize ( 0 ),
    m_Pos ( 0 )
{
    m_pBuffer = TL_NEW char [ Sz + 1 ];
    m_pBuffer [ 0 ] = 0;
    if  ( Buff && m_pBuffer )
    {
        memcpy ( m_pBuffer, Buff, m_BuffSize = Sz );
        m_pBuffer [ Sz ] = 0;
    }
}
//---------------------------------------------------------------------------
CMaaTcpConnection::Buffer::~Buffer ()
{
    delete [] m_pBuffer;
}
//---------------------------------------------------------------------------
CMaaString CMaaTcpConnection::GetConnectionName() noexcept
{
    return m_ConnName;
}
//---------------------------------------------------------------------------
void CMaaTcpConnection::Send ( const char * Buff, int Size )
{
    Buffer * pb = TL_NEW Buffer ( Size, Buff );
    m_SendList.AddAtBack ( pb );
    ChangeFdMode ( eAll );
    //Notify_Write ();
}
//---------------------------------------------------------------------------
void CMaaTcpConnection::SetName ( const char * ConnName )
{
    m_ConnName.Format ( "%s %s", m_ClassName, ( const char * ) ConnName );
}
//---------------------------------------------------------------------------
CMaaTcpConnection::CMaaTcpConnection ( const char * ClassName )
:   m_ClassName ( ClassName )
{
}
//---------------------------------------------------------------------------
CMaaTcpConnection::~CMaaTcpConnection ()
{
    Buffer * pb;
    while( ( pb = m_SendList.GetFromFront () ) )
    {
        delete pb;
    }
}
//---------------------------------------------------------------------------
int CMaaTcpConnection::Notify_Accepted ( _IP IpFrom, _Port Port )
{
    CMaaString ConnName;
    ConnName.Format ( "%s:%d->%d", CMaaIpToText ( IpFrom ), Port, GetBindedPort () );
    SetName ( ConnName );
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpConnection::SendBuffer ()
{
    Buffer * pb = m_SendList.GetFromFront();
    Buffer &b = *pb;
    if  ( !pb )
    {
        return eEmptyList;
    }
    b.m_Pos += SendData ( b.m_pBuffer + b.m_Pos, b.m_BuffSize - b.m_Pos );
    //printf ( "%d send %d/%d bytes\n", fIsClientSide, b.m_Pos, b.m_BuffSize );
    if  ( b.m_BuffSize > b.m_Pos )
    {
        IfDebug(200) printf ( "SendBuffer () %d/%d\n%s\n", b.m_Pos, b.m_BuffSize, b.m_pBuffer );
        // not all buffer was sent immediately
        m_SendList.AddAtFront ( &b );
        return eToBeContinued;
    }
    //IfDebug(200) printf ( "SendBuffer () %d\n%s\n", b.m_BuffSize, b.m_pBuffer );
    delete &b;
    return ( m_SendList.LookAtFront () ) ? eBufferSent : eLastBuffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// class CMaaTcpProxy
//---------------------------------------------------------------------------
CMaaTcpProxy::CMaaTcpProxy ( CMaaString & TcpServName )
:   CMaaTcpConnection ( szClassName ),
    fIsClientSide ( 1 ),
    pOtherSide ( nullptr ),
    m_TcpServName ( TcpServName ),
    fCloseByRead0 ( 1 ),
    fNeedToClose ( 0 )
{
    pOtherSide = TL_NEW CMaaTcpProxy ( this, m_TcpServName );
    AddFdSocket ();
}
//---------------------------------------------------------------------------
CMaaTcpProxy::CMaaTcpProxy ( CMaaTcpProxy * pClientSide, CMaaString & TcpServName )
:   CMaaTcpConnection ( szClassName ),
    fIsClientSide ( 0 ),
    pOtherSide ( nullptr ),
    m_TcpServName ( TcpServName ),
    fCloseByRead0 ( 1 ),
    fNeedToClose ( 0 )
{
    CMaaString tmp;
    tmp.Format ( "C ....->%s", ( const char * ) TcpServName );
    SetName ( tmp );
    Connect ( m_TcpServName );
    tmp.Format ( "%d->%s", GetBindedPort (), ( const char * ) TcpServName );
    SetName ( tmp );
    pOtherSide = pClientSide;
    AddFdSocket ();
}
//---------------------------------------------------------------------------
int CMaaTcpProxy::Notify_Read  ()
{
    char Buff [ 4096 + 1 ];
    int r = RecvData ( Buff, sizeof ( Buff ) - 1 );

    if  ( !r && fCloseByRead0 )
    {
        // ending connection
        //.....
        if  ( pOtherSide )
        {
            pOtherSide -> pOtherSide = nullptr;
            pOtherSide -> fNeedToClose = 1;
            pOtherSide = nullptr;
        }
        return CloseByExeption ( "closing connection" );
    }

    if  ( ! pOtherSide )
    {
        try
        {
            pOtherSide = TL_NEW CMaaTcpProxy ( this, m_TcpServName );
        }
        catch( XTOOSockErr & )
        {
        }
        if  ( !pOtherSide )
        {
            //SendHttpErrorResponse ( "Can't connect to server\n\r\n\r" );
            return CloseByExeption ( "Can't create connection to server" );
        }
    }

    if  ( pOtherSide )
    {
        pOtherSide -> Send ( Buff, r );
    }
    return eAll;
    //return eRead;
}
//---------------------------------------------------------------------------
int CMaaTcpProxy::Notify_Write ()
{
    Buffer * pb;
    int fBuff = 0;
    while( ( pb = m_SendList.GetFromFront () ) )
    {
        fBuff++;
        Buffer & b = * pb;
        b.m_Pos += SendData ( b.m_pBuffer + b.m_Pos, b.m_BuffSize - b.m_Pos );
        if  ( b.m_BuffSize > b.m_Pos )
        {
            //printf ( "attach again buffer\n" );
            // not all buffer was sent immediately
            m_SendList.AddAtFront ( pb );
            return eRead;
        }
        delete pb;
    }
    // nothing to be send
    if  ( !fBuff )
    {
        if  ( fNeedToClose )
        {
            return CloseByExeption ( "closing connection (after write all data)" );
        }
    }
    return eAll;
}
//---------------------------------------------------------------------------
CMaaTcpProxy::~CMaaTcpProxy ()
{
    Buffer * pb;
    while( ( pb = m_SendList.GetFromFront () ) )
    {
        delete pb;
    }
    if  ( pOtherSide )
    {
        pOtherSide -> pOtherSide = nullptr;
        IfDebug(100) printf ( "delete        %x %s\n", pOtherSide, (const char *)pOtherSide->GetConnectionName() );
        delete pOtherSide;
    }
}
//---------------------------------------------------------------------------
const char CMaaTcpProxy::szClassName [] = "TcpProxy";
//---------------------------------------------------------------------------
CMaaTcpSocket * CMaaTcpProxyServer::CreateNewConnection ()
{
    return TL_NEW CMaaTcpProxy ( m_ServName );
}
//---------------------------------------------------------------------------
CMaaTcpProxyServer::CMaaTcpProxyServer ( int Port, const char * Server )
:   CMaaUnivServer ( Port, "TcpProxyServer" ),
    m_ServName ( Server )
{
}
//---------------------------------------------------------------------------

#endif // __TOOLS_USE_WINSOCK2

#endif // #if 0

//#undef DebugLevel
//int DebugLevel = 0;

#endif // #ifndef __TOOLSLIB_REMOVE_OLD_CODE

#endif // __TOOLS_USE_WINSOCK || __TOOLS_USE_WINSOCK2

#endif // _WIN32
