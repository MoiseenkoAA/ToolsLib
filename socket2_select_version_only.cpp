// old 30.08.20 12:18 select() version

// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: РњРѕРёСЃРµРµРЅРєРѕ РђРЅРґСЂРµР№ РђР»РµРєСЃРµРµРІРёС‡)
 */

// ToolsLib/socket2.cpp

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
#include "ToolsLib\temp.h"
static int DebugLevel = 0;
#else
#include "temp.h"
int DebugLevel = 0;
#endif
#define IfDebug(x) if ( x <= DebugLevel )

#define LOG1(x) //m_C == 1 && log1(x)
#define LOG1_(x) //m_pFdSockets->m_C == 1 && m_pFdSockets->log1(x)

extern SOCKET g_____socket;

int gInt = 0;

CMaaString gShaperPerformanceInfo;
CMaaString gShaperTempInfo;
int iSockThread_pt = 0;

CMaaTime * gpShaper_hrt = NULL;
CMaaFile * gpm_logfile = NULL;

#define m_hrt (*gpShaper_hrt)
#define m_logfile (*gpm_logfile)

_qword SockThread_pt[100];

#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)

#ifdef __SOCK_NEW_TIMERS
template<> CMaaFixedAllocator<CMaaHeap<_qword, CMaaSocketTimer *>::Rec> * CMaaHeap<_qword, CMaaSocketTimer *>::Rec::s_pAllocator = NULL;
#endif

CMaaFdSockets * gCMaaFdSockets = NULL;
CMaaSockStartup * gpSockStartup = NULL;


#define __FORCED_CLOSE_NOTIFICATION

void ExtLog(const char * msg);

//void Log(const char * txt);

/*
void pppp(const char *fmt, ...)
{
     va_list list;
     va_start(list, fmt);
     char txt[1024];
        _vsnprintf(txt, sizeof(txt), fmt, list);
     Log(txt);
     va_end ( list );
}

#define printf pppp
*/

void Trace ( const char * msg, int s )
{
    printf ( msg, s );
    printf ( "\n" );
    fflush(stdout);
}

void Trace ( const char * msg, int s, const char * str )
{
    printf ( msg, s, str );
    printf ( "\n" );
    fflush(stdout);
}

//----------------------------------------------------------------------------------------------------------

CMaaFixedAllocator<CMaaConnectionInTimeCharacteristics::DataBlock> * CMaaConnectionInTimeCharacteristics::DataBlock::s_pAllocator = NULL;

CMaaConnectionInTimeCharacteristics::CMaaConnectionInTimeCharacteristics(int MaxPoints, _qword InitialValue, _qword Units)
:   m_bptr(((m_max_points = (MaxPoints >= 0 ? MaxPoints : -1) + 1) + 7) / 8)
{
    m_Units = Units;
    m_bptr.Zero();
    Reset(InitialValue);
}

CMaaConnectionInTimeCharacteristics::~CMaaConnectionInTimeCharacteristics()
{
    for (int i = (int)m_bptr.MaxIndex(); --i >= 0; )
    {
        delete m_bptr[i];
    }
}

void CMaaConnectionInTimeCharacteristics::Reset(_qword InitialValue, bool bToLastData)
{
    if  (bToLastData)
    {
        if  (m_pos > 0)
        {
            int RealPos = (m_pos - 1) % m_max_points;
            InitialValue = m_bptr[RealPos / 8]->Data[RealPos % 8];
            m_pos = 0;
            Set_Inc(m_InitialValue = InitialValue);
            return;
        }
    }
    m_pos = 0;
    Set_Inc(m_Accumulator = m_InitialValue = InitialValue);
}

void CMaaConnectionInTimeCharacteristics::Set_Inc(_qword x)
{
    if  (m_max_points <= 0)
    {
        return;
    }
    if  (m_pos < m_max_points && m_bptr[m_pos / 8] == NULL)
    {
        m_bptr[m_pos / 8] = new DataBlock;
        if  (!m_bptr[m_pos / 8])
        {
            return;
        }
    }
    int RealPos = m_pos % m_max_points;
    m_bptr[RealPos / 8]->Data[RealPos % 8] = x;
    m_pos++;
}

void CMaaConnectionInTimeCharacteristics::Accumulate(_qword x, bool bSet_and_Inc)
{
    m_Accumulator += x;
    if  (bSet_and_Inc)
    {
        Set_Inc(m_Accumulator);
    }
}

_qword CMaaConnectionInTimeCharacteristics::GetDataDifference(int Period)
{
    if  (m_pos <= 1 || Period <= 0)
    {
        if  (Period == -1 && m_pos > 0)
        {
            int RealPos = (m_pos - 1) % m_max_points;
            return m_bptr[RealPos / 8]->Data[RealPos % 8] - m_InitialValue;
        }
        return 0;
    }

    if  (Period >= m_pos - 1 && m_pos < m_max_points)
    {
        int RealPos = (m_pos - 1) % m_max_points;
        return m_bptr[RealPos / 8]->Data[RealPos % 8] - m_bptr[0]->Data[0];
    }

    if  (Period >= m_max_points)
    {
        Period = m_max_points - 1;
    }

    int StartPos = (m_pos - 1 - Period) % m_max_points;
    int EndPos = (m_pos - 1) % m_max_points;
    return m_bptr[EndPos / 8]->Data[EndPos % 8] - m_bptr[StartPos / 8]->Data[StartPos % 8];
}

_qword CMaaConnectionInTimeCharacteristics::GetAverageDataDifference(int Period)
{
    if  (m_pos <= 1 || Period <= 0)
    {
        if  (Period == -1 && m_pos > 0)
        {
            int RealPos = (m_pos - 1) % m_max_points;
            return (m_bptr[RealPos / 8]->Data[RealPos % 8] - m_InitialValue) / m_pos / m_Units;
        }
        return 0;
    }

    if  (Period >= m_pos - 1 && m_pos < m_max_points)
    {
        int RealPos = (m_pos - 1) % m_max_points;
        return (m_bptr[RealPos / 8]->Data[RealPos % 8] - m_bptr[0]->Data[0]) / (m_pos - 1) / m_Units;
    }

    if  (Period >= m_max_points)
    {
        Period = m_max_points - 1;
    }

    int StartPos = (m_pos - 1 - Period) % m_max_points;
    int EndPos = (m_pos - 1) % m_max_points;
    return (m_bptr[EndPos / 8]->Data[EndPos % 8] - m_bptr[StartPos / 8]->Data[StartPos % 8]) / Period / m_Units;
}

int CMaaConnectionInTimeCharacteristics::GetCurPos()
{
    return m_pos > 0 ? m_pos - 1 : 0;
}

int CMaaConnectionInTimeCharacteristics::GetCurMaxPeriod()
{
    int x = GetCurPos();
    return x < m_max_points || x == 0 ? x : m_max_points - 1;
}
//----------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------
#define ib( x ) ( ( int )( ip >> x ) & 0xff )
//---------------------------------------------------------------------------
#ifdef __unix__
char * CMaaIpToTextEx ( char * txt, _IP ip, int Mode )
{
    //static char txt [ 20 ];
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
const char * CMaaIpToText ( _IP ip, int Mode )
{
    static char txt [ 20 ];
    return CMaaIpToTextEx(txt, ip, Mode);
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
// returns 0 -- means: Ok, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4" is converted to 0x01020304
int CMaaIpToLong ( const char * txt, _IP * RetIp )
{
    _dword ip = 0, tmp;
    int i;

    for ( i = 0; i < 4; i++ )
    {
        tmp = 0;
        if  ( txt[0] < '0' || txt[0] > '9' )
        {
            return -1;     // …ЈЈ®Ј
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
_IP CMaaIpAddress ( _byte a, _byte b, _byte c, _byte d )
{
    return ( ( _IP )( a ) << 24 ) | ( ( _IP )( b ) << 16 ) | ( ( _IP )( c ) <<  8 ) | ( _IP )( d );
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
            return -1;     // …ЈЈ®Ј
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
#endif // #ifdef __unix__

#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)

//---------------------------------------------------------------------------
// Name of system service
//---------------------------------------------------------------------------
#ifdef __unix1__
_PCSTR GetServByProtoAndPort ( const _word PortValue, _PCSTR IpProtoName )
{
    _PSTR pNameServ = NULL;
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
#endif // #ifdef __unix1__

static _qword __GetTime0AndResetHrt(CMaaTime &hrTimer)
{
    hrTimer.StartWatch();
    return GetCurrentTime_us();
}
//---------------------------------------------------------------------------
_qword CMaaFdSocket::UpdateHrTime(int index)
{
    _qword t;
    static CMaaTime hrTimer(GetHRTime());
    static _qword t0 = __GetTime0AndResetHrt(hrTimer);
    if  (index >= 0 && index < e_fds_max_time)
    {
        t = hrTimer.GetTime(1000000) + t0;
        m_hrTime[index] = t;
    }
    else if (index == e_fds_current_time)
    {
        t = hrTimer.GetTime(1000000) + t0;
    }
    else
    {
        t = -1;
    }
    return t;
}
_qword CMaaFdSocket::SetHrTime(int index, _qword t)
{
    if  (index >= 0 && index < e_fds_max_time)
    {
        m_hrTime[index] = t;
    }
    else
    {
        t = -1;
    }
    return t;
}
_qword CMaaFdSocket::GetHrTime_(int index)
{
    _qword t;
    if  (index >= 0 && index < e_fds_max_time)
    {
        t = m_hrTime[index];
    }
    else// if (index == e_fds_current_time)
    {
        t = UpdateHrTime(e_fds_current_time);
    }
    /*
     else
     {
          t = -1;
     }
     */
    return t;
}
_qword CMaaFdSocket::GetHrTime(int index, int RelatedTo, bool bAbsoluteTime, int factor)
{
    _qword t2 = GetHrTime_(RelatedTo);
    if  (t2 <= 0)
    {
        return -1;
    }
    _qword t = GetHrTime_(index);
    if  ((t -= t2) < 0)
    {
        return -1;
    }
    /*
     if   (bAbsoluteTime)
     {
          t += m_AbsTimeCreated;
     }
     */
    if  (factor == 1000000)
    {
    }
    else if (factor == 1000)
    {
        t /= 1000;
    }
    else if (factor == 1)
    {
        t /= 1000000;
    }
    else
    {
#ifdef _WIN32
        t = MaaMulDiv64(t, factor, 1000000);
#else
        // not implemented
#endif
    }
    return t;
}
CMaaFdSocket::CMaaFdSocket(CMaaFdSockets * pFdSockets)
:   m_pFdSockets (pFdSockets)
{
#ifdef _WIN32
    m_LastMode = -1;
    m_Ev = WSACreateEvent();
#endif
    m_AcceptFlag = 0;
    m_AbsTimeCreated = GetCurrentTime_us();
    UpdateHrTime(e_fds_created_time);
}
//---------------------------------------------------------------------------
CMaaFdSocket::~CMaaFdSocket ()
{
#ifdef _WIN32
    WSACloseEvent(m_Ev);
#endif
}
//---------------------------------------------------------------------------
void CMaaFdSocket::SetFdSockets ( CMaaFdSockets * pFdSockets )
{
    m_pFdSockets = pFdSockets;
}
//---------------------------------------------------------------------------
void CMaaFdSocket::SetFdSockets ( CMaaFdSocket * FdSocket )
{
    m_pFdSockets = FdSocket -> m_pFdSockets;
}
//---------------------------------------------------------------------------
void CMaaFdSocket::ChangeFdMode ( int Mode )
{
    if  ( m_pFdSockets )
    {
        m_pFdSockets -> ChangeMode ( ( CMaaSocket * ) this, Mode );
    }
}
//---------------------------------------------------------------------------
int CMaaFdSocket::GetFdMode()
{
    if  (m_pFdSockets)
    {
        return m_pFdSockets->GetMode((CMaaSocket *)this);
    }
    return 0;
}
//---------------------------------------------------------------------------
CMaaSocket::CMaaSocket(CMaaFdSockets * pFdSockets, int af, int type, int protocol )
:   CMaaFdSocket(pFdSockets),
    m_Socket ( INVALID_SOCKET )
{
    //m_bIsClosing = false;
    m_Socket = socket ( af, type, protocol );
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
        //shutdown ( m_Socket, 0 );
        closesocket ( m_Socket );
    }
    RemoveFdSocket ();
}
//---------------------------------------------------------------------------
void CMaaSocket::Error ( const char * msg )
{
    //CMaaString Msg;
    //int Error = WSAGetLastError ();
    //
    //Msg.Format ( IDS_XTOOSockError, msg, Error );
    //throw XTOOSocketError ( Msg, Error, DelObject );
    //throw msg;
    throw XTOOSockErr ( msg, NULL, GetConnectionName () );
    //CloseByException ( ( char * ) msg );
}
//---------------------------------------------------------------------------
void CMaaSocket::StaticError ( const char * msg )
{
    throw XTOOSockErr ( msg, NULL, NULL );
}
//---------------------------------------------------------------------------
SOCKET CMaaSocket::GetSocket ()
{
    return m_Socket;
}
//---------------------------------------------------------------------------
void CMaaSocket::SetSocket ( SOCKET s )
{
    ChangeFdMode(eDisableAll);
    if  ( m_Socket != INVALID_SOCKET  )
    {
        closesocket ( m_Socket );
    }
    m_Socket = s;
#ifdef _WIN32
    m_LastMode = -1;
#endif
}
//---------------------------------------------------------------------------
void CMaaSocket::SwapSockets(SOCKET &s)
{
    SOCKET s1 = m_Socket;
    m_Socket = s;
    s = s1;
#ifdef _WIN32
    m_LastMode = -1;
#endif
}
//---------------------------------------------------------------------------
int CMaaSocket::CloseByExeption ( const char * msg )
{
    Notify_PreClosingByException(msg);
    // ending connection
    //DeleteSocket Err ( msg, this );
    //throw Err;
    throw XTOOSockErr ( msg, this, GetConnectionName () );
    return eDisableAll;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_PreDeletingByException ()
{
    return 0;
}
//---------------------------------------------------------------------------
void CMaaSocket::AddFdSocket ()
{
    m_pFdSockets -> AddSocket ( this );
}
//---------------------------------------------------------------------------
void CMaaSocket::RemoveFdSocket ()
{
    m_pFdSockets -> RemoveSocket ( this );
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
    return eAll;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Error ()
{
    return CloseByException ( "Some socket error occured" );
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Accepted ( _IP IpFrom, _Port Port )
{
    return 0;
}
//---------------------------------------------------------------------------
const char * CMaaSocket::GetConnectionName ()
{
    return NULL;
}
//---------------------------------------------------------------------------
_IP CMaaSocket::GetHostByName(const char * DnsName, _Port * pPort, _IP * pAllIPs, int * pAllIPsCount)
{
    int xAI = 10;
    if  (!pAllIPsCount)
    {
        pAllIPsCount = &xAI;
    }
    if  (!pAllIPs)
    {
        *pAllIPsCount = 0;
    }
    char * p = (char *)strchr ( DnsName, ':' );
    if  ( p && pPort )
    {
        int Port = 0;
        sscanf ( p + 1, "%d", &Port );
        * pPort = Port;
    }
    //CMaaBuffer txt ( DnsName );
    CMaaString txt(NULL, (DnsName ? (strlen(DnsName)) : 0) + 1024);
    if  (DnsName)
    {
        strcpy((char *)(const char *)txt, DnsName);
    }
    if  ( p )
    {
        // making DNS name with out of port specified after ':'
        txt [ (size_t)(p - DnsName) ] = 0;
        DnsName = txt;
    }

    _IP Ip = 0;
    if  ( CMaaIpToLong ( DnsName, &Ip ) )
    {
        // It is not numerical address as name given
        struct hostent * h = gethostbyname ( DnsName );
        if  ( !h )
        {
            CMaaString txt;
            txt.Format("gethostbyname(\"%s\")", DnsName);
            throw XTOOSockDnsErr ( txt, NULL, GetConnectionName () );
            //Error ( txt );
            return 0;
        }
        Ip = ntohl ( * ( _IP * ) h -> h_addr_list [ 0 ] );
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            pAllIPs[i] = ntohl(*(_IP *)h->h_addr_list[i]);
        }
        if  (i < *pAllIPsCount)
        {
            pAllIPs[i] = 0;
        }
        *pAllIPsCount = i;
    }
    else
    {
        if  (*pAllIPsCount > 1)
        {
            pAllIPs[1] = 0;
        }
        if  (*pAllIPsCount > 0)
        {
            pAllIPs[0] = Ip;
            *pAllIPsCount = 1;
        }
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
        sprintf ( txt, "gethostbyaddrname (\"%s\")", CMaaIpToTextEx ( pip, Ip, 0 ) );
        throw XTOOSockDnsErr ( txt, NULL, GetConnectionName () );
        //Error ( txt );
        return 0;
    }
    strcpy ( DnsName, h -> h_name );
    return DnsName;
}
//---------------------------------------------------------------------------
int CMaaSocket::Close ()
{
    //shutdown ( m_Socket, 0 );
    throw XTOOSockClose ( this );
    //return CloseByException ( "Closing connection" );
}
//---------------------------------------------------------------------------

int gDisableNagleAlgoritm = 1;
//---------------------------------------------------------------------------
CMaaTcpSocket::CMaaTcpSocket(CMaaFdSockets * pFdSockets)
//:    CMaaSocket(pFdSockets, AF_INET, SOCK_STREAM, 0 )
:   CMaaSocket(pFdSockets, AF_INET, SOCK_STREAM, IPPROTO_TCP )
#ifdef __SHAPERS
    ,
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_SndLLShaper(this),
    m_RcvLLShaper(this)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
#endif
{
    m_ShaperConditionLastCheckTime = 0;
    memset(m_pConnectionInTimeCharacteristics, 0, sizeof(m_pConnectionInTimeCharacteristics));

    InitCounters(false);
    memset ( &m_sin, 0, sizeof ( m_sin ) );

    // Variable to set the SO_KEEPALIVE option with setsockopt ().
    BOOL fKeepAlive = 1;//TRUE;

    if  ( SOCKET_ERROR == setsockopt ( m_Socket, SOL_SOCKET, SO_KEEPALIVE, ( char * ) &fKeepAlive, sizeof ( BOOL ) ) )
    {
        Error ( "setsockopt ()" );
    }

    if  (gDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gDisableNagleAlgoritm != 0);
    }

    /*
     char txt[1024];
     sprintf(txt, "CMaaTcpSocket::CMaaTcpSocket(): this = %p\n", this);
     ExtLog(txt);
     */
}
CMaaTcpSocket::~CMaaTcpSocket()
{
    DeleteConnectionInTimeCharacteristics();
}
//---------------------------------------------------------------------------
bool CMaaTcpSocket::DisableNagleAlgoritm(bool bDisable, SOCKET s)
{
#ifdef __unix__
    int x = bDisable ? 1 : 0;
#else
    BOOL x = bDisable ? TRUE : FALSE;
#endif
    return setsockopt(s != INVALID_SOCKET ? s : m_Socket, IPPROTO_TCP, TCP_NODELAY, (char *)&x, sizeof(x)) == 0;
}
//---------------------------------------------------------------------------
void CMaaTcpSocket::DeleteConnectionInTimeCharacteristics()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            delete m_pConnectionInTimeCharacteristics[i][j];
            m_pConnectionInTimeCharacteristics[i][j] = NULL;
        }
    }
}

void CMaaTcpSocket::CreateConnectionInTimeCharacteristics(int s, int m, int h, int d)
{
    DeleteConnectionInTimeCharacteristics();
    int i;
    for (i = 0; i < 4; i++)
    {
        _qword InitialValue = !(i & 1) ? m_BytesSend : m_BytesRecv;
        m_pConnectionInTimeCharacteristics[i][0] = TL_NEW CMaaConnectionInTimeCharacteristics(s, InitialValue, 1);
        m_pConnectionInTimeCharacteristics[i][1] = TL_NEW CMaaConnectionInTimeCharacteristics(m, InitialValue, 60);
        m_pConnectionInTimeCharacteristics[i][2] = TL_NEW CMaaConnectionInTimeCharacteristics(h, InitialValue, 3600);
        m_pConnectionInTimeCharacteristics[i][3] = TL_NEW CMaaConnectionInTimeCharacteristics(d, InitialValue, 24 * 3600);
    }
    int null_ptrs = 0;
    for (i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if  (!m_pConnectionInTimeCharacteristics[i][j])
            {
                null_ptrs++;
            }
        }
    }
    if  (null_ptrs)
    {
        DeleteConnectionInTimeCharacteristics();
    }
}
void CMaaTcpSocket::ResetConnectionInTimeCharacteristics(bool bSecondPartOnly)
{
    if  (m_pConnectionInTimeCharacteristics[0][0])
    {
        for (int i = !bSecondPartOnly ? 0 : 2; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                _qword value = !(i & 1) ? m_BytesSend : m_BytesRecv;
                m_pConnectionInTimeCharacteristics[i][j]->Reset(value, true);
            }
        }
    }
}
void CMaaTcpSocket::InitCounters(bool bConnected)
{
    m_BytesSend = m_BytesRecv = m_BytesSend0 = m_BytesRecv0 = 0;
    m_SendSpeed = m_RecvSpeed = bConnected ? 0 : -1;
    ResetConnectionInTimeCharacteristics(false);
}
//---------------------------------------------------------------------------
void CMaaTcpSocket::Timer1sSpeed()
{
    m_SendSpeed = m_BytesSend != (_qword)0 ? (int)(m_BytesSend - m_BytesSend0) : m_SendSpeed;
    m_BytesSend0 = m_BytesSend;

    m_RecvSpeed = m_BytesRecv != (_qword)0 ? (int)(m_BytesRecv - m_BytesRecv0) : m_RecvSpeed;
    m_BytesRecv0 = m_BytesRecv;

    if  (m_pConnectionInTimeCharacteristics[0][0])
    {
        for (int i = 0; i < 4; i++)
        {
            _qword value = !(i & 1) ? m_BytesSend : m_BytesRecv;
            //_qword value = !(i & 1) ? m_SendSpeed : m_RecvSpeed;
            m_pConnectionInTimeCharacteristics[i][0]->Set_Inc(value);
            if  (m_pConnectionInTimeCharacteristics[i][0]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[i][1]->Set_Inc(value);
                if  (m_pConnectionInTimeCharacteristics[i][1]->GetCurPos() % 60 == 0)
                {
                    m_pConnectionInTimeCharacteristics[i][2]->Set_Inc(value);
                    if  (m_pConnectionInTimeCharacteristics[i][2]->GetCurPos() % 24 == 0)
                    {
                        m_pConnectionInTimeCharacteristics[i][3]->Set_Inc(value);
                        //Accumulate(value, true);
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void CMaaTcpSocket::GetStatistics(_qword * pSentBytes, _qword * pRecvBytes, int * pSendSpeed, int * pRecvSpeed, bool bShapersSpeed)
{
    if  (pSentBytes)
    {
        *pSentBytes = m_BytesSend;
    }
    if  (pRecvBytes)
    {
        *pRecvBytes = m_BytesRecv;
    }
    if  (pSendSpeed)
    {
        *pSendSpeed = m_SendSpeed;
    }
    if  (pRecvSpeed)
    {
        *pRecvSpeed = m_RecvSpeed;
    }
    if  (bShapersSpeed)
    {
        gLock.LockM();
        if  (pSendSpeed && m_SndLLShaper.m_Shaper && m_BytesSend != (_qword)0)
        {
            *pSendSpeed = m_SndLLShaper.m_VAverageLast;
        }
        if  (pRecvSpeed && m_RcvLLShaper.m_Shaper && m_BytesRecv != (_qword)0)
        {
            *pRecvSpeed = m_RcvLLShaper.m_VAverageLast;
        }
        gLock.UnLock();
    }
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::SendData ( const void * Buffer, int Size )
{
    int Total = 0;
    int ret = 0;

    int rr;
    while(Size > 0)
    {
        rr = -1;
#ifdef __SHAPERS
        gLock.LockM();
        if  (m_SndLLShaper.m_Shaper)
        {
            rr = m_SndLLShaper.GetLenReserved(Size);
        }
        gLock.UnLockM();
#endif
        if  (rr == 0)
        {
            break;
        }
        //printf ( "send(%d,%p,%d,%d) ", m_Socket, Buffer, Size, 0 );fflush(stdout);
        //printf("send(%d, len=%d)...\n", m_Socket, rr > 0 ? rr : Size); fflush(stdout);
        ret = send(m_Socket, ( char * ) Buffer, rr > 0 ? rr : Size, 0);
        //printf("send(%d) returns %d\n", m_Socket, ret); fflush(stdout);

        /*
          if ( ret < 0 )
          {
               printf ( "send: ret = %d ", ret );fflush(stdout);
          }
          */

        if  ( ret == SOCKET_ERROR )
        {
            DWORD dw = 0;
#ifdef _WIN32
            if  ( (dw = WSAGetLastError ()) != WSAEWOULDBLOCK )
#else
            // unix
            if  (errno != EAGAIN)
#endif
            {
                CloseByException ( "send ()" );
            }

            //char txt[256];
            //sprintf(txt, "ret == SOCKET_ERROR, WSAGetLastError() == WSAEWOULDBLOCK");
            //ExtLog("----- ret == SOCKET_ERROR, WSAGetLastError() == WSAEWOULDBLOCK");

            Total = -1;
            // 27.07.2012 one cycle, break
            //Total = Total > 0 ? Total : -1;
            // 27.07.2012


            break;
        }
        else
        {
            UpdateHrTime(e_fds_last_sent_time);
            Total  += ret;
            Size   -= ret;
            Buffer = ret + ( char * ) Buffer;
            if  (m_BytesSend == (_qword)-1)
            {
                m_BytesSend = 0;
            }
            m_BytesSend += ret;
        }
#ifdef __SHAPERS
        gLock.LockM();
        if  (m_SndLLShaper.m_Shaper)
        {
            m_SndLLShaper.SubLenReserved(ret);
        }
        gLock.UnLockM();
#endif
        break; //!!!!!!!!!!!!!!
    }
    int Mode;
    if  ((Total > 0 || Total == -1) && !m_pFdSockets->m_SockHash.Find(this, &Mode))
    {
        //if (Total != -1)
        {
            Mode &= ~eEventWrite;
            m_pFdSockets->m_SockHash.AddOver(this, Mode);
        }
        Total = Total > 0 ? Total : 0;
    }
    //printf("SendData() for %d: Total = %d", m_Socket, Total); fflush(stdout);
    return Total;
}
//---------------------------------------------------------------------------
bool gCMaaTcpSocketsCompatibleRead = false; //only with out of shapers
//---------------------------------------------------------------------------
bool CMaaTcpSocket::IsClosed(int LastReceivedLen)
{
#ifdef __SHAPERS
    if  (LastReceivedLen < 0 || (!m_RcvLLShaper.m_Shaper && gCMaaTcpSocketsCompatibleRead && LastReceivedLen == 0))
    {
        return true;
    }
#else
    if  (LastReceivedLen < 0 || (gCMaaTcpSocketsCompatibleRead && LastReceivedLen == 0))
    // or if   (LastReceivedLen <= 0)
    {
        return true;
    }
#endif
    return false;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::RecvData ( void * Buffer, int Size )
{
    int Total = 0;
    int ret = 0;

    bool bCompatibleRead = false;

    int rr;
    while(Size > 0)
    {
        rr = -1;
#ifdef __SHAPERS
        gLock.LockM();
        if  (m_RcvLLShaper.m_Shaper)
        {
            rr = m_RcvLLShaper.GetLenReserved(Size);
        }
        else
#endif
        {
            bCompatibleRead = gCMaaTcpSocketsCompatibleRead;
        }
#ifdef __SHAPERS
        gLock.UnLockM();
#endif
        if  (rr == 0)
        {
            break;
        }
        //printf("recv(%d, len=%d, rr=%d, Size=%d)...\n", m_Socket, rr > 0 ? rr : Size, rr, Size); fflush(stdout);
#ifdef __unix__
        if  (m_Socket == fileno(stdin))
        {
            ret = read(m_Socket, ( char * ) Buffer, rr > 0 ? rr : Size);
            if  (ret < 0 && errno == EAGAIN)
            {
                ret = 0;
            }
        }
        else
#endif
        ret = recv(m_Socket, ( char * ) Buffer, rr > 0 ? rr : Size, 0);
        //printf("recv(%d) returns %d\n", m_Socket, ret); fflush(stdout);
        if  (ret == SOCKET_ERROR)
        {
            //XTOOSockErr err("recv", NULL);
            //printf("%s\n", (const char *)err.GetMsg()); fflush(stdout);

            //if   ( WSAGetLastError () != WSAEWOULDBLOCK )
            {
                CloseByException("recv ()");
                //vAsyncMsg ( m_Socket, FD_READ, err );
            }
            break;
        }
        else if (!ret)
        {
            Total = -1; // on close
            break;
        }
        else
        {
            UpdateHrTime(e_fds_last_receive_time);
            Total  += ret;
            Size   -= ret;
            Buffer = ret + ( char * ) Buffer;
            if  (m_BytesRecv == (_qword)-1)
            {
                m_BytesRecv = 0;
            }
            m_BytesRecv += ret;
        }
        if  (Total > 0)
        {
#ifdef __SHAPERS
            gLock.LockM();
            if  (m_RcvLLShaper.m_Shaper)
            {
                m_RcvLLShaper.SubLenReserved(Total);
            }
            gLock.UnLockM();
#endif
        }
        break; //!!!!!!!!!!!!!!
    }
    int Mode;
#ifdef __unix__
    if  ((Total > 0 || m_Socket == fileno(stdin)) && !m_pFdSockets->m_SockHash.Find(this, &Mode))
#else
    if  (Total > 0 && !m_pFdSockets->m_SockHash.Find(this, &Mode))
#endif
    {
#ifdef __unix__
        if  (m_Socket == fileno(stdin))
        {
            //printf("Mode = 0x%x -->", Mode);
            Mode &= ~eEventRead;
            //printf(" 0x%x\n", Mode); fflush(stdout);
        }
        else
        {
#endif
            char c = 0;
            //printf("recv(%d, len=1, MSG_PEEK)...\n", m_Socket); fflush(stdout);
            int l = recv(m_Socket, &c, 1, MSG_PEEK);
            //printf("recv(%d) returns %d\n", m_Socket, l); fflush(stdout);
            if  (l >= 0)
            {
                Mode |= eEventRead;
            }
            else
            {
                Mode &= ~eEventRead;
            }
#ifdef __unix__
        }
#endif
        m_pFdSockets->m_SockHash.AddOver(this, Mode);
    }


    //printf("RecvData for %d: Total = %d\n", m_Socket, Total); fflush(stdout);

    if  (bCompatibleRead && Total < 0)
    {
        Total = 0;
    }
    //char txt [ 256 ];
    //sprintf ( txt, "RecvData() %d bytes", Total );
    //Trace ( txt );
    return Total;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::ConnectAgain ( int fCallNotify )
{
    //     printf("%p: CMaaTcpSocket::ConnectAgain (%d)\n", this, fCallNotify);

    IfDebug ( 200 ) printf ( "CMaaTcpSocket::ConnectAgain ()\n" );
    _IP Ip = htonl ( m_sin.sin_addr.s_addr );
    _Port Port = htons ( m_sin.sin_port );
    char pip [ 30 ];
    //fflush(stdout);
#ifdef _WIN32
    if  (fCallNotify == 100)
    {
        ChangeFdMode ( eDisableConnect );
#ifdef __unix__
        //ChangeFdMode(eConnect | eExept);
        SetNonBlockingMode();
#endif
        UpdateHrTime(e_fds_connected_at_time);
        return Notify_Connected ( Ip, Port, CMaaIpToTextEx ( pip, Ip, 0 ) );
    }
#endif
    int x;
    if  ( (x=connect ( m_Socket, ( struct sockaddr * ) & m_sin, sizeof ( m_sin ) )) )
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
        if  (0)
        {
            printf ( "connect returns %d errno=%d", x, errno );
            XTOOSockErr Err ( "error=", NULL );
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
                ChangeFdMode ( eDisableConnect );
#ifdef __unix__
                //printf("SetNonBlockingMode()\n"); fflush(stdout);
                SetNonBlockingMode();
#endif
                UpdateHrTime(e_fds_connected_at_time);
                return Notify_Connected ( Ip, Port, CMaaIpToTextEx ( pip, Ip, 0 ) );
            }
            return fCallNotify ? 0 : 3;
        default:
            {
                IfDebug(2) printf ( "\n\n_____connect returns %d errno=%d", x, errno );
                {XTOOSockErr Err ( "error=", NULL );}
                IfDebug(2) printf ( "_____\n" );
                fflush(stdout);
            }
        }

        char txt [ 100 ];
        sprintf ( txt, "connect (%s, %d)", CMaaIpToTextEx ( pip, Ip, 0 ), Port );
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
        ChangeFdMode ( eDisableConnect );
#ifdef __unix__
        //ChangeFdMode(eConnect | eExept);
        SetNonBlockingMode();
#endif
        UpdateHrTime(e_fds_connected_at_time);
        return /**/ Notify_Connected ( Ip, Port, CMaaIpToTextEx ( pip, Ip, 0 ) );
        // TEST: return 0;
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect ( _IP Ip, _Port Port )
{
    IfDebug ( 100 ) printf ( "CMaaTcpSocket::Connect ( %s, %d )\n", CMaaIpToText ( Ip ), Port );
    memset ( & m_sin, 0, sizeof ( m_sin ) );
    m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons ( Port );
    m_sin.sin_addr.s_addr = htonl ( Ip );
    ChangeFdMode(eConnect);
    //printf("\n\nMode: %d\n", GetFdMode());
    int Ret = ConnectAgain ( 0 );
    IfDebug(1)
    {
        printf("Connect for %d returns %d\n", m_Socket, Ret); fflush(stdout);
    }
    if  ( Ret >= 3 )
    {
        char pip [ 30 ];
        char txt [ 100 ];
        sprintf ( txt, "connect (%s, %d)", CMaaIpToTextEx ( pip, Ip, 0 ), Port );
        Error ( txt );
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect ( const char * DnsName, _Port Port )
{
    IfDebug ( 100 ) printf ( "CMaaTcpSocket::Connect ( \"%s\", %d )\n", DnsName, Port );
    _IP Ip = GetHostByName ( DnsName, &Port );
    return Connect ( Ip, Port );
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect ( _IP Ip, _Port Port )
{
    //RemoveFdSocket ();
    ChangeFdMode(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect ( Ip, Port );
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect ( const char * DnsName, _Port Port )
{
    //RemoveFdSocket ();
    ChangeFdMode(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect ( DnsName, Port );
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Bind(_Port Port, _IP Ip, bool bReuse)
{
    IfDebug ( 100 ) printf ( "CMaaTcpSocket::Bind ( %d )\n", Port );
    m_sin.sin_family      = AF_INET;
    m_sin.sin_port        = htons(Port);
    m_sin.sin_addr.s_addr = htonl(Ip);
    if  (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
#define P_PARAM (const char *)
#else
        int On = 1;
#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof(On));
    }
    if  (::bind(m_Socket, (struct sockaddr *) &m_sin, sizeof(m_sin)) == SOCKET_ERROR)
    {
        Error("bind ()");
        return 2;
    }
    socklen_t len = sizeof ( m_sin );
    if  (getsockname(m_Socket, (struct sockaddr *)&m_sin, &len))
    {
        Error("getsockname()");
        return 22;
    }
    SetAcceptFlag();
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Bind(CMaaString strIpPort, bool bReuse)
{
    _IP BindIp = 0;
    int Port = 0;
    int trailer_pos = CMaaIpToLongEx(strIpPort, &BindIp, ":");

    strIpPort = strIpPort.Mid(trailer_pos > 0 ? trailer_pos + 1: 1);
    sscanf(strIpPort, "%d", &Port);
    return Bind(Port, BindIp, strIpPort.LastChar() == 'r');
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
    return ntohs ( m_sin.sin_port );
}
//---------------------------------------------------------------------------
void CMaaTcpSocket::GetConnInfo(_IP * pLocalIp, _Port * pLocalPort, _IP * pRemoteIp, _Port * pRemotePort)
{
    struct sockaddr_in s;
    socklen_t len;
    if  (pLocalIp || pLocalPort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if  (getsockname(m_Socket, (struct sockaddr *)&s, &len))
        {
            Error("getsockname()");
        }
        if  (pLocalIp)
        {
            *pLocalIp = ntohl(s.sin_addr.s_addr);
        }
        if  (pLocalPort)
        {
            *pLocalPort = ntohs(s.sin_port);
        }
    }
    if  (pRemoteIp || pRemotePort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if  (getpeername(m_Socket, (struct sockaddr *)&s, &len))
        {
            Error("getpeername()");
        }
        if  (pRemoteIp)
        {
            *pRemoteIp = ntohl(s.sin_addr.s_addr);
        }
        if  (pRemotePort)
        {
            *pRemotePort = ntohs(s.sin_port);
        }
    }
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
        throw XTOOSockDnsErr ( msg, NULL, "CMaaSocket::GetLocalHostIp ()" );
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
     for   ( i = 0; h -> h_aliases [ i ] != NULL; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != NULL; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    return htonl ( * ( _IP * ) h -> h_addr_list [ 0 ] );
}
//---------------------------------------------------------------------------
const char * CMaaSocket::GetLocalHostName ()
{
    static char txt [ 1024 ];
    txt [ 0 ] = 0;
    gethostname ( txt, sizeof ( txt ) );
    //struct hostent * h = gethostbyname ( txt );
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
    IfDebug(100)
    {
        Trace("SetNonBlockingMode() for %d - done", (int)m_Socket);
    }
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Listen ( int QueenDepth )
{
    IfDebug ( 100 ) printf ( "CMaaTcpSocket::Listen ()\n" );
    if  ( SOCKET_ERROR == listen ( m_Socket, QueenDepth ) )
    {
        Error ( "listen ()" );
        return 3;
    }
    return 0;
}
//---------------------------------------------------------------------------
SOCKET CMaaTcpSocket::Accept ( _IP * IpFrom, _Port * Port )
{
    //printf("Accept()\n");
    IfDebug ( 100 ) printf ( "CMaaTcpSocket::Accept () " );
    struct sockaddr_in si;
    socklen_t len;
    memset ( &si, 0, len = sizeof ( si ) );
    SOCKET s = accept ( GetSocket (), (struct sockaddr *) & si, & len );
    //printf("accepted socket %d\n", s);
    int Mode;
    if  (!m_pFdSockets->m_SockHash.Find(this, &Mode))
    {
        Mode &= ~eEventRead;
        m_pFdSockets->m_SockHash.AddOver(this, Mode);
    }

    if  ( s == SOCKET_ERROR )
    {
        //printf("accept error %d\n", errno);
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
    //printf("....\n");
    IfDebug ( 100 )
    {
        printf ( "accept from %s:%d, socket = %d\n", CMaaIpToText ( htonl ( si.sin_addr.s_addr ) ), htons ( si.sin_port ), s);
        fflush(stdout);
    }

    if  (gDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gDisableNagleAlgoritm != 0, s);
    }

    return s;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpSocket::CMaaUdpSocket(CMaaFdSockets * pFdSockets)
:   CMaaSocket(pFdSockets, AF_INET, SOCK_DGRAM, IPPROTO_UDP)
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
    else
    {
        UpdateHrTime(e_fds_last_sent_time);
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient(CMaaFdSockets * pFdSockets, _Port Port, _IP Ip )
:   CMaaUdpSocket(pFdSockets),
    m_Port ( Port ),
    m_Ip   ( Ip )
{
}
//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient(CMaaFdSockets * pFdSockets, _Port Port, const char * DnsName )
:   CMaaUdpSocket(pFdSockets),
    m_Port ( Port )
{
    m_Ip = GetHostByName ( DnsName );
}
//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient(CMaaFdSockets * pFdSockets)
:   CMaaUdpSocket(pFdSockets),
    m_Port ( 0 ),
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
CMaaUdpBroadcast::CMaaUdpBroadcast(CMaaFdSockets * pFdSockets, _Port Port)
:   CMaaUdpClient(pFdSockets, Port, ( _IP ) 0 )
{
    // Variable to set the broadcast option with setsockopt ().
    BOOL fBroadcast = 1;//TRUE;

    if  ( SOCKET_ERROR == setsockopt ( m_Socket, SOL_SOCKET, SO_BROADCAST, ( char * ) & fBroadcast, sizeof ( BOOL ) ) )
    {
        Error ( "setsockopt ()" );
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
CMaaUdpServer::CMaaUdpServer(CMaaFdSockets * pFdSockets, _Port Port, _IP BindIp, bool bReuse)
:   CMaaUdpSocket(pFdSockets)
{
    IfDebug ( 100 ) printf ( "CMaaUdpServer::CMaaUdpServer ( %d )\n", Port );
    memset(&m_sin, 0, sizeof(m_sin));
    m_sin.sin_family = AF_INET;
    m_sin.sin_port   = htons(Port);
    m_sin.sin_addr.s_addr = htonl(BindIp);

    if  (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
        //#define P_PARAM (const char *)
#else
        int On = 1;
        //#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof(On));
    }

    if  (SOCKET_ERROR == ::bind(m_Socket, (struct sockaddr *) &m_sin, sizeof(m_sin)))
    {
        Error("bind()");
    }
    AddFdSocket();
}
void CMaaUdpServer::AddMembership(_IP MulticastAddr /*= CMaaIpAddress(224,0,0,0)*/, _IP InterfaceIpOrIndex /*= 0*/)
{
#ifdef _WIN32
    struct ip_mreq mreq;   // Used in adding or dropping multicasting addresses
    mreq.imr_multiaddr.s_addr = htonl(MulticastAddr);
    mreq.imr_interface.s_addr = htonl(InterfaceIpOrIndex);

    if  (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR *)&mreq, sizeof (mreq)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
    }
#else
    StaticError("Not implemented");
#endif
}
void CMaaUdpServer::SetIpMulticastIfaceAndTTL(_IP IfaceIpOrIfIndex, int ttl) // use Sent//sendto internally// to send to multicast IP and port later
{
#ifdef _WIN32
    DWORD dwIfaceIpOrIfIndex = htonl(IfaceIpOrIfIndex);
    if  (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_IF, (char FAR *)&dwIfaceIpOrIfIndex, sizeof (DWORD)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_MULTICAST_IF) failed");
    }
    int iTTL = ttl;
    if  (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char FAR *)&iTTL, sizeof (int)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
    }
#else
    StaticError("SetIpMulticastTTL for unix -- Not implemented");
#endif
}
//---------------------------------------------------------------------------
int CMaaUdpSocket::Recv(void * pBuffer, int BufferSize)
{
    socklen_t len = sizeof(m_tmp);

    memset(&m_tmp, 0, sizeof(m_tmp));
    int Ret = recvfrom(m_Socket, (char *)pBuffer, BufferSize, 0, (struct sockaddr *)&m_tmp, &len);
    if  (Ret == SOCKET_ERROR)
    {
        Error("recvfrom()");
    }
    else
    {
        UpdateHrTime(e_fds_last_receive_time);
    }
    int Mode = 0;
    if  (!m_pFdSockets->m_SockHash.Find(this, &Mode))
    {
        Mode &= ~eEventRead;
        m_pFdSockets->m_SockHash.AddOver(this, Mode);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaUdpSocket::Recv(void * pBuffer, int BufferSize, _IP * pIp, _Port * pPort)
{
    int len = Recv(pBuffer, BufferSize);
    if  (pIp)
    {
        *pIp = GetSourceIp();
    }
    if  (pPort)
    {
        *pPort = GetSourcePort();
    }
    return len;
}
//---------------------------------------------------------------------------
_IP CMaaUdpSocket::GetSourceIp ()
{
    return htonl ( m_tmp.sin_addr.s_addr );
}
//---------------------------------------------------------------------------
_Port CMaaUdpSocket::GetSourcePort ()
{
    return htons ( m_tmp.sin_port );
}
//---------------------------------------------------------------------------
_Port CMaaUdpSocket::GetBindedPort ()
{
    return htons ( m_sin.sin_port );
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CMaaFdSockets::CMaaFdSockets(CMaaSockThread * pThread)
:   m_SockHash ( FD_SETSIZE ),
    m_SelectSockets(0),
    m_SelectModes(0),
#ifdef _WIN32
    m_Select_hh(0),
#else
    m_Select_rev(0),
    m_Select_wev(0),
#endif
    m_pThread(pThread)
{
    m_bForceMode = false;
    //1111111
    //FD_ZERO(&m_Read);
    //FD_ZERO(&m_Write);
    //FD_ZERO(&m_Exept);
    m_Timeval.tv_sec = 1;
    m_Timeval.tv_usec = 0;
}
//---------------------------------------------------------------------------
CMaaFdSockets::~CMaaFdSockets ()
{
    while(m_Timers.LookAtFront())
    {
        Detach(m_Timers.LookAtFront());
    }
}
//---------------------------------------------------------------------------
void CMaaFdSockets::AddSocket ( CMaaSocket * pSock, int Mode )
{
    IfDebug(201)
    {
        printf ( "%p: AddSocket %p\n", this, pSock ); fflush(stdout);
    }
    RemoveSocket ( pSock );
    if  ( !m_SockHash.AddOver ( pSock, Mode ) )
    {
        IfDebug(201)
        {
            printf ( "AddSocket %p - ok\n", pSock ); fflush(stdout);
        }
        SOCKET s = pSock -> GetSocket ();
        if  ( IsWinStdioSock ( s ) )
        {
            return;
        }
#ifdef _WIN32
        ChangeMode(pSock, Mode);
#else
        /*
111111111

          if   ( ( Mode & eRead ) )
          {
               FD_SET( s, & m_Read );
          }
          if   ( ( Mode & eWrite ) )
          {
               FD_SET( s, & m_Write );
          }
          if   ( ( Mode & eExept ) )
          {
               FD_SET( s, & m_Exept );
          }
*/
#endif
    }
}
//---------------------------------------------------------------------------
void CMaaFdSockets::RemoveSocket ( CMaaSocket * pSock )
{
    IfDebug(201)
    {
        printf ( "%p: RemoveSocket %p\n", this, pSock ); fflush(stdout);
    }

    int Mode = 0;
    if  ( !m_SockHash.Remove ( pSock, &Mode ) && pSock )
    {
        SOCKET s = pSock -> GetSocket ();
        IfDebug(201)
        {
            printf ( "SOCKET=%d \n", s ); fflush(stdout);
        }
        if  ( !IsWinStdioSock ( s ) )
        {
            /*111111
               FD_CLR( s, & m_Read );
               FD_CLR( s, & m_Write );
               FD_CLR( s, & m_Exept );
*/
        }
    }
}
//---------------------------------------------------------------------------
int CMaaFdSockets::IsWinStdioSock ( SOCKET sock )
{
#ifdef __unix__
    return 0;
#else
    return 0;
    /*
     int s = sock;
     return ( s == _fileno ( stdin ) || s == _fileno ( stdout ) || s == _fileno ( stderr ) ) ? 1 : 0;
     */
#endif
}
//---------------------------------------------------------------------------
void CMaaFdSockets::SetTimeOut ( int Sec, int MicroSec )
{
    m_Timeval.tv_sec  = Sec;
    m_Timeval.tv_usec = MicroSec;
}
//---------------------------------------------------------------------------
int CMaaFdSockets::GetSockCount()
{
    /*
     int k = (warning_int)m_SockHash.GetMaxIndex();
     //CMaaPtr < CMaaSocket * > Sockets ( k );
     //CMaaPtr < int > Modes ( k );
     CMaaPtr < CMaaSocket * > Sockets ( k );
     CMaaPtr < int > Modes ( k );
     k = m_SockHash.EnumerateItems ( k, Sockets, Modes );
     */
    int k = m_SockHash.GetItemCount();
    return k;
}
//---------------------------------------------------------------------------
int CMaaFdSockets::GetSockets(CMaaSocket ** m, int k)
{
    //CMaaPtr < int > Modes(k);
    CMaaPtr < int > Modes(k);
    k = m_SockHash.EnumerateItems(k, m, Modes);
    return k;
}
//---------------------------------------------------------------------------
#ifdef _WIN32
void pr3e(const char * ttt, HANDLE * h, HANDLE h0, HANDLE h1, CMaaSocket *s)
{
    char txt[512];
    txt[0] = 0;
    for (int i = 0; i < 3; i++)
    {
        if  (*h == h0)
        {
            strcat(txt, "0");
        }
        else if (*h == h1)
        {
            strcat(txt, "1");
        }
        else if (*h == s->m_Ev)
        {
            strcat(txt, "e");
        }
        else
        {
#ifdef __SHAPERS
            CMaaTcpSocket * t = dynamic_cast<CMaaTcpSocket *>(s);
            if  (t)
            {
                if  (*h == t->m_SndLLShaper.m_hEv)
                {
                    strcat(txt, "s");
                }
                else if (*h == t->m_RcvLLShaper.m_hEv)
                {
                    strcat(txt, "r");
                }
                else
                {
                    strcat(txt, "?");
                }
            }
            else
            {
                strcat(txt, "?");
            }
#endif
        }
        if  (WaitForSingleObject(*h, 0) == WAIT_OBJECT_0)
        {
            strcat(txt, "+");
        }
        else
        {
            strcat(txt, "-");
        }
        h++;
    }
    printf(ttt, txt);
}
//---------------------------------------------------------------------------
bool CMaaFdSockets::log1(int point)
{
    /*
     char txt[100];
     int n = sprintf(txt, " log1: %d\n", point);
     CMaaFile f("c:\\maa\\timerlog.txt", "AC|SrSw", false);
     f.Write(GetTextDateTime(time(NULL)) + txt);
     return n != 0;
     */
    return false;
}
int CMaaFdSockets::Select()
{
    LOG1(1);
    gLock.LockM();
    static HANDLE hEv0 = CreateEvent(NULL, TRUE, FALSE, NULL);
    static HANDLE hEv1 = CreateEvent(NULL, TRUE, TRUE, NULL);
    gLock.UnLockM();
    LOG1(2);
    struct timeval Timeval;
    Timeval = m_Timeval;

    LOG1(3);
    int k = (warning_int)m_SockHash.GetMaxIndex ();

    int SelectArraysUsedCount = 0;

    LOG1(4);
    CMaaPtr<CMaaSocket *> tempSockets(0);
    CMaaPtr<int> tempModes(0);
    CMaaPtr<HANDLE> temp_hh(0);

    LOG1(5);
    if  (m_SelectArraysUsedCount == 0)
    {
        if  (m_SelectSockets.MaxIndex() >= (size_t)k && m_SelectModes.MaxIndex() >= (size_t)k && m_Select_hh.MaxIndex() >= (size_t)(2 + 3 * k))
        {
        }
        else
        {
            CMaaPtr<CMaaSocket *> Sockets(k);
            CMaaPtr<int> Modes(k);
            CMaaPtr<HANDLE> hh(2 + 3 * k);
            m_SelectSockets.Swap(Sockets);
            m_SelectModes.Swap(Modes);
            m_Select_hh.Swap(hh);
        }
        m_SelectArraysUsedCount = SelectArraysUsedCount = 1;
    }
    else
    {
        CMaaPtr<CMaaSocket *> Sockets(k);
        CMaaPtr<int> Modes(k);
        CMaaPtr<HANDLE> hh(2 + 3 * k);
        tempSockets.Swap(Sockets);
        tempModes.Swap(Modes);
        temp_hh.Swap(hh);
    }

    LOG1(6);
    CMaaPtr<CMaaSocket *> &Sockets = SelectArraysUsedCount ? m_SelectSockets : tempSockets;
    CMaaPtr<int> &Modes = SelectArraysUsedCount ? m_SelectModes : tempModes;

    try
    {
        /*
     CMaaPtr < CMaaSocket * > Sockets ( k );
     CMaaPtr < int > Modes ( k );
*/
        LOG1(7);
        k = m_SockHash.EnumerateItems ( k, Sockets, Modes );

        /*
     IfDebug(202)
     {
         printf ( " k=%d ", k ); fflush(stdout);
     }
     */
        //errno = 0;
        LOG1(8);
#ifndef __unix__
        WSASetLastError ( 0 );
#endif
        LOG1(9);
        IfDebug(201)
        {
            printf ( "selecting " ); fflush(stdout);
        }
        {
            int us = Timeval.tv_sec * 1000000 + Timeval.tv_usec;
            int us11 = us;
            GetWaitForTime(&us);
            if  (us == 0 && us11 > 0)
            {
                static int aa = 0;
                if  (++aa == 1000)
                {
                    static int bb = 0;
                    bb++;
                }
            }
            if  (m_bForceMode)
            {
                us = 0;
                m_bForceMode = false;
            }
            Timeval.tv_sec = us / 1000000;
            Timeval.tv_usec = us % 1000000;
            //printf("tv2: %d %d\r\n", Timeval.tv_sec, Timeval.tv_usec);
            //fflush(stdout);
        }
        LOG1(10);
        LOG1(Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000 + 1000000000);
        //CMaaPtr<HANDLE> hh(2 + 3 * k);
        //CMaaPtr<HANDLE> hh(2 + 3 * k);
        CMaaPtr<HANDLE> &hh = SelectArraysUsedCount ? m_Select_hh : temp_hh;
        hh[0] = hh[1] = m_pThread->m_hWakeup;
        int i;
        for (i = 0; i < k; i++)
        {
            hh[2 + 3 * i] = Sockets[i]->m_Ev;
            hh[2 + 3 * i + 1] = hEv0;
            hh[2 + 3 * i + 2] = hEv0;
            //Sockets[i]->SetTestValue(-1);
            //Sockets[i]->SetTestMessage(CMaaString::sFormat("%p: to = %d\n", Sockets[i], Timeval.tv_sec * 1000000 + Timeval.tv_usec));
            CMaaTcpSocket * t = dynamic_cast<CMaaTcpSocket *>(Sockets[i]);
            if  (Sockets[i]->GetFdMode() & CMaaEnum::eHold)
            {
                t = NULL;
                hh[2 + 3 * i] = hEv0;
            }
            if  (t)
            {
                int wr = -1, rr = -1;
#ifdef __SHAPERS
                LOG1(11);
                gLock.LockM();
                if  (t->m_SndLLShaper.m_Shaper)
                {
                    wr = t->m_SndLLShaper.GetLenReserved(1000000);
                    /*
                    if   (wr == 0)
                    {
                         ResetEvent(t->m_SndLLShaper.m_hEv);
                    }
                    */
                }
                if  (t->m_RcvLLShaper.m_Shaper)
                {
                    rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                    /*
                    if   (rr == 0)
                    {
                         ResetEvent(t->m_RcvLLShaper.m_hEv);
                    }
                    */
                }
                /*
               char txt[1024];
               sprintf(txt, "%p: wr = %d, rr = %d\n", t, wr, rr);
               ExtLog(txt);
               */
                gLock.UnLockM();
                LOG1(12);
                if  (rr == 0 && wr == 0)
                {
                    hh[2 + 3 * i] = hEv0;
                    hh[2 + 3 * i + 1] = t->m_SndLLShaper.m_hEv;
                    hh[2 + 3 * i + 2] = t->m_RcvLLShaper.m_hEv;
                }
                else if (rr && wr)
                {
                    //hh[2 + 3 * i + 1] = hh[2 + 3 * i + 2] = m_hEv0;
                }
                else if (rr == 0 && wr)
                {
                    hh[2 + 3 * i + 2] = t->m_RcvLLShaper.m_hEv;
                    if  ((Modes[i] & (eEventWrite|eWrite)) != eWrite)
                    {
                        hh[2 + 3 * i] = hEv0;
                    }
                }
                else if (rr && wr == 0)
                {
                    hh[2 + 3 * i + 1] = t->m_SndLLShaper.m_hEv;
                    //if   ((Modes[i] & (eEventRead|eRead)) != eRead)
                    if  (!(Modes[i] & (eRead|eExept)) || (Modes[i] & eEventRead))
                    {
                        hh[2 + 3 * i] = hEv0;
                    }
                }
#endif
                LOG1(13);
                if  (rr)
                {
                    if  ((Modes[i] & (eEventRead|eRead)) == (eEventRead|eRead))
                    {
                        hh[2 + 3 * i] = hEv1;
                    }
                }
                if  (wr)
                {
                    if  ((Modes[i] & (eEventWrite|eWrite)) == (eEventWrite|eWrite))
                    {
                        hh[2 + 3 * i] = hEv1;
                    }
                }
            }
        }
        /*
     gLock.LockM();
     {
          printf("ThreadId = %x, ", GetCurrentThreadId());
          if   (WaitForSingleObject(hh[0], 0) == WAIT_OBJECT_0)
          {
               printf("WakeUp+");
          }
          else
          {
               printf("WakeUp-");
          }
          printf (" time=%dms ", Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000);
          for  (int i = 0; i < k; i++)
          {
               printf(" sk=%d: ", Sockets[i]->GetSocket());
               pr3e("%s", &hh[2 + 3 * i], hEv0, hEv1, Sockets[i]);
          }
          printf("\n");
     }
     gLock.UnLockM();
     */
        LOG1(14);
#if 0
        {
            for (int i = 0; i < k; i++)
            {
                if  (!m_SockHash.Find(Sockets[i]))
                {
                    Sockets[i]->SetTestValue((int)(Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000));
                }
            }
        }
#endif
        LOG1(15);
        DWORD w = WaitForMultipleObjects(2 + 3 * k, hh, FALSE, Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000);
        LOG1(16);
#if 0
        {
            for (int i = 0; i < k; i++)
            {
                if  (!m_SockHash.Find(Sockets[i]))
                {
                    Sockets[i]->SetTestValue(-(int)(Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000));
                }
            }
        }
#endif
        LOG1(17);
        {
            static int aa = 0;
            aa++;
            //#define __log__
#ifdef __log__
            static CMaaFile f("c:\\temp\\1.txt", "WCD|SrSw", false);
            static CMaaTime _time_ = GetHRTime();
#endif
            if  (aa > 30000)
            {
                static int aa2 = 0;
                if  (!aa2)
                {
                    m_temp_SelectFlag = 1;
                }
                if  (m_temp_SelectFlag)
                {
                    static int nn = 0;
#ifdef __log__
                    f.fprintf("%7d. w = %d, to = %D us, waiting to was %d ms\n", ++nn, w, _time_.GetTime(), Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000);
                    _time_.StartWatch();
#endif
                }
                aa2++;
                aa2++;
                aa2++;
            }
        }
        LOG1(18);
        if  (w == WAIT_TIMEOUT)
        {
            w = WAIT_OBJECT_0;
            //return 0;
        }
        w -= WAIT_OBJECT_0;
        if  ((int)w < 0 || (int)w >= 2 + 3 * k)
        {
            LOG1(19);
            return -1;
        }
        if  (w < 2)
        {
            LOG1(20);
            ResetEvent(m_pThread->m_hWakeup);
        }
        LOG1(21);
        //if   ( Ret > 0 )
        //if   (w >= 2)
        {
            IfDebug(1) printf ( "\r" );

            /*
          i = (int)w - 2;
          WSANETWORKEVENTS ev;
             ev.lNetworkEvents = 0;
             WSAEnumNetworkEvents(Sockets [ i ]->GetSocket(), hh[2 + i], &ev);
          */

            for (int i = 0; i < k; i++)
            //for  (int i = w - 2; i < w - 2; i++)
            //i = w - 2;
            {
                LOG1(22);
                /*
{
char txt[512];
sprintf(txt, "i = %d, Sockets[i] = %p", i, Sockets[i]);
Log(txt);
}
*/
                int Mode;
                if  (!m_SockHash.Find(Sockets[i], &Mode))
                {
                    if  (Mode & CMaaEnum::eHold)
                    {
                        continue;
                    }
#if 0
                    Sockets[i]->SetTestValue((w & 0xffff) + ((2 + 3 * i) << 16));
#endif
                    //bool bAcc123 = false;
                    WSANETWORKEVENTS ev;
                    memset(&ev, 0, sizeof(ev));
                    //if   (w < 2 || (w >= 2 + 3 * i && w < 2 + 3 * (i + 1))) //  + || 1 //--> 10.01.2008
                    //Sockets[i]->SetTestMessage(CMaaString::sFormat("%p: w = %d, i = %d, Mode = %x\n", Sockets[i], w, i, Mode));
                    if  (w >= 2 && (w - 2) / 3 != i)
                    {
                        //continue;
                    }
                    //CMaaString txt12;
                    if  (1)//w < 2 || (w - 2) / 3 == i) //  + || 1 //--> 10.01.2008
                    {
                        ev.lNetworkEvents = 0;
                        WSAEnumNetworkEvents(Sockets[i]->GetSocket(), Sockets[i]->m_Ev, &ev);
                        WSAResetEvent(Sockets[i]->m_Ev);
                        {
                            int Flag = 1;
                            int Mode = Sockets[i]->GetFdMode();
                            if  ((ev.lNetworkEvents & FD_READ) && !(Mode & eRead))
                            {
                                Flag = -1;
                            }
                            if  ((ev.lNetworkEvents & FD_WRITE) && !(Mode & eWrite))
                            {
                                Flag = -1;
                            }
#ifdef DEBUG_SET_TEST_MESSAGE
                            Sockets[i]->SetTestMessage(CMaaString::sFormat("(1) ev.lNetworkEvents = 0x%x, Mode = %x\n", ev.lNetworkEvents, Mode), Flag);
#endif
                        }
                        if  (ev.lNetworkEvents & FD_ACCEPT)
                        {
                            static int aa = 0;
                            aa++;
                            //printf("FD_ACCEPT, Mode = 0x%x, Sockets[i]->m_AcceptFlag = %d\n", Mode, Sockets[i]->m_AcceptFlag);
                            Mode |= eEventRead;
                            //bAcc123 = true;
                        }
                        /*
                         if   (Mode & eConnect)
                         {
                              if   ((ev.lNetworkEvents & FD_WRITE) && ev.iErrorCode[FD_WRITE_BIT] == 0)
                              {
                                   Mode |= eEventWrite;
                              }
                         }
                         */
                        if  (!Sockets[i]->m_AcceptFlag && !(Mode & eConnect) && dynamic_cast<CMaaTcpSocket*>(Sockets[i]) != NULL)
                        {
                            //if(0)
                            {
                                if  ((ev.lNetworkEvents & FD_WRITE) && ev.iErrorCode[FD_WRITE_BIT] == 0)
                                {
                                    Mode |= eEventWrite;
                                }
                                if  (!(ev.lNetworkEvents & FD_WRITE) && (Mode & (eWrite|eEventWrite)) == (eWrite|eEventWrite))
                                {
                                    ev.lNetworkEvents |= FD_WRITE;
                                    ev.iErrorCode[FD_WRITE_BIT] = 0;
                                }
                                if  ((ev.lNetworkEvents & FD_READ) && ev.iErrorCode[FD_READ_BIT] == 0)
                                {
                                    if  (!(Mode & eRead) && Sockets[i]->m_bDebug)
                                    {
                                        gInt++;
                                        gInt++;
                                        gInt++;
                                    }
                                    //ev.lNetworkEvents &= ~FD_READ;
                                    Mode |= eEventRead;
                                }
                                if  ((ev.lNetworkEvents & (FD_READ|FD_CLOSE)) && (Mode & (eRead|eExept)) == eExept) // today
                                {
                                    if  (ev.iErrorCode[FD_READ_BIT] == 0)
                                    {
                                        //CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket*>(Sockets[i]);
                                        char c = 0;
                                        int l = recv(Sockets[i]->GetSocket(), &c, 1, MSG_PEEK);

                                        if  (l == 0)
                                        {
                                            ev.lNetworkEvents |= FD_READ;
                                            ev.iErrorCode[FD_READ_BIT] = CMaa_CONN_CLOSED_ERROR;//WSAENOTCONN;
                                            //ExtLog("ev.lNetworkEvents |= FD_READ; ev.iErrorCode[FD_READ_BIT] = 0;\n");
                                            //Mode |= eEventRead;
                                        }
                                        else if (l < 0)
                                        {
                                            DWORD x = WSAGetLastError();
                                            if  (x != WSAEWOULDBLOCK)
                                            {
                                                ev.lNetworkEvents |= FD_READ;
                                                ev.iErrorCode[FD_READ_BIT] = x;
                                                //Mode |= eEventRead;
                                            }
                                        }
                                        else
                                        {
                                            Sockets[i]->ChangeFdMode(0);
                                        }
                                    }
                                }
                                if  (!(ev.lNetworkEvents & FD_READ) && (Mode & eRead)) //10.01.2008
                                {
                                    CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket*>(Sockets[i]);

                                    char c = 0;
                                    int l = recv(Sockets[i]->GetSocket(), &c, 1, MSG_PEEK);
                                    if  (l > 0)
                                    {
                                        ev.lNetworkEvents |= FD_READ;
                                        ev.iErrorCode[FD_READ_BIT] = 0;
                                        //ExtLog("ev.lNetworkEvents |= FD_READ; ev.iErrorCode[FD_READ_BIT] = 0;\n");
                                        Mode |= eEventRead;
                                    }
                                    /*if   (l == 0)
                                        {
                                        }
                                        */else if (l < 0)
                                    {
                                        DWORD x = WSAGetLastError();
                                        if  (x != WSAEWOULDBLOCK)
                                        {
                                            ev.lNetworkEvents |= FD_READ;
                                            ev.iErrorCode[FD_READ_BIT] = x;
                                            Mode |= eEventRead;
                                            /*
                                                  char txt[256];
                                                  sprintf(txt, "ev.lNetworkEvents |= FD_READ; ev.iErrorCode[FD_READ_BIT] = %d;\n", x);
                                                  //ExtLog(txt);
                                                  FILE * f = fopen("c:\\ftp_sk2log.txt", "a+b");
                                                  if   (f)
                                                  {
                                                       fprintf(f, "%s", txt);
                                                       fclose(f);
                                                  }
                                                  */
                                        }
                                    }
                                }
                                if  (!(ev.lNetworkEvents & FD_READ) && (Mode & (eRead|eEventRead)) == (eRead|eEventRead))
                                {
                                    ev.lNetworkEvents |= FD_READ;
                                    ev.iErrorCode[FD_READ_BIT] = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        /*
                         if   ((Mode & eRead) && Sockets [ i ]->m_bIsClosing)
                         {
                              ev.lNetworkEvents |= FD_READ;
                         }
                         */
                    }
                    //CMaaString _txt = CMaaString::sFormat("%p: (2) ev.lNetworkEvents = 0x%x\n", Sockets[i], ev.lNetworkEvents);
                    //txt12 = CMaaString("  ") + txt12 + "  " + _txt;

                    m_SockHash.AddOver(Sockets[i], Mode);

                    {
                        int Flag = 1;
                        //int Mode = Sockets[i]->GetFdMode();
                        if  ((ev.lNetworkEvents & FD_READ) && !(Mode & eRead))
                        {
                            Flag = -1;
                        }
                        if  ((ev.lNetworkEvents & FD_WRITE) && !(Mode & eWrite))
                        {
                            Flag = -1;
                        }
#ifdef DEBUG_SET_TEST_MESSAGE
                        Sockets[i]->SetTestMessage(CMaaString::sFormat("(2) ev.lNetworkEvents = 0x%x\n", ev.lNetworkEvents), Flag);
#endif
                    }

                    //Sockets[i]->SetTestMessage(CMaaString("\n") + txt12 + "\n");

                    if  (ev.lNetworkEvents != 0)
                    {
                        //printf("Set mode 0x%x for socket %d, ev.lNetworkEvents = 0x%x\n", Mode, Sockets[i]->GetSocket(), ev.lNetworkEvents);
                    }
                    /*
                    if   (Mode & eWrite)
                    {
                         ev.lNetworkEvents |= FD_WRITE;
                    }
                    */
                    LOG1(23);
                    bool bReadIsCalled = false;
                    int m0 = 0;
                    m_SockHash.Find(Sockets[i], &m0);
                    for ( int j = 0; j < 5; j++ )
                    {
                        /*
{
char txt[512];
sprintf(txt, "j = %d, Sockets[i] = %p", j, Sockets[i]);
Log(txt);
}
*/
                        LOG1(24);
                        int Mode;
                        if  ( !m_SockHash.Find ( Sockets [ i ], &Mode ) )
                        {
                            if  (Mode & eHold)
                            {
                                continue;
                            }
                            /*
                              if   (bAcc123)
                              {
                                   printf("acc: j=%d, Mode=0x%x\n", j, Mode);
                              }
                              */
                            /*
{
char txt[512];
sprintf(txt, "j = %d, Sockets[i] = %p  +", j, Sockets[i]);
Log(txt);
}
*/
                            if  (j == 1 && !(Mode & eRead)) // read
                            {
                                continue;
                            }
                            if  (j == 3 && !(Mode & eWrite)) // write
                            {
                                continue;
                            }
                            bool bConnect = false;
                            try
                            {
                                unsigned int Act = 0;
                                CMaaSocket * pSock = Sockets [ i ];
                                SOCKET s = pSock -> GetSocket ();
                                switch( j )
                                {
                                case 1:
                                    /*
                              if   (bAcc123)
                              {
                                   printf(">>>acc: j=%d, Mode=0x%x, ev.lNetworkEvents=0x%x\n", j, Mode, ev.lNetworkEvents);
                              }
                              */
                                    if  (ev.lNetworkEvents & (FD_READ|FD_ACCEPT))
                                    //if   ( FD_ISSET( s, &Read ) )
                                    {
                                        //Trace ( "Notify_Read for %d", s );
                                        //printf ( "R%d ", s );
                                        if  (pSock->m_bDebug)
                                        {
                                            int &aa = gInt;
                                            aa++;
                                            aa++;
                                            aa++;
                                            aa++;
                                        }
                                        CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                        if  (p && p->m_CloseTimer.IsStarted() && p->m_CloseTimer.GetPeriod() > 2000)
                                        {
                                            p->m_CloseTimer.Start(2000);
                                        }
                                        bReadIsCalled = true;
                                        Act = pSock -> Notify_Read ();
                                        //Trace("Notify_Read returns %d", Act);
                                    }
                                    break;
                                case 2:
                                    if  (ev.lNetworkEvents & FD_CLOSE)
                                    //if   ( FD_ISSET( s, &Read ) )
                                    {
                                        //printf ( "\r\n\n\n\n\n\nFD_CLOSE\r\n\n\n" );
                                        CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(Sockets [ i ]);
                                        if  (p)
                                        {
                                            p->m_CloseTimer.OnClose(p, bReadIsCalled);
                                        }

                                        //Sockets [ i ]->m_bIsClosing = true;
                                        //Act = pSock -> Notify_Read ();
                                    }
                                    break;
                                case 3:
                                    if  ((ev.lNetworkEvents & FD_WRITE) || ((Mode & eWrite) && !(Mode & eConnect)))// && !(m0 & eWrite)))
                                    //if   ( FD_ISSET( s, &Write ) )
                                    {
                                        //printf ( "W%d ", s );
                                        /*
                                             if   ( Mode & eConnect )
                                             {
                                                  Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                             }
                                             else
                                             */
                                        {
                                            //Trace ( "Notify_Write for %d", s );
                                            CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                            _qword x = p ? p->GetDataSentSize() : 0;
                                            Act = pSock->Notify_Write();
                                            if  ((Act & eWrite) && p && p->GetDataSentSize() != x)
                                            {
                                                WakeUp();
                                            }
                                        }
                                    }
                                    break;
                                case 4:
                                    if  (ev.lNetworkEvents & FD_CONNECT)
                                    if  ( Mode & eConnect )
                                    {
                                        //Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                        bConnect = true;
                                        Act = ((CMaaTcpSocket *)pSock)->ConnectAgain(100);
                                    }
                                    break;
                                case 0:
                                    if  ((ev.lNetworkEvents & FD_CONNECT) && ev.iErrorCode[FD_CONNECT_BIT])
                                    {
                                        WSASetLastError(ev.iErrorCode[FD_CONNECT_BIT]);
                                        Act = pSock -> Notify_Error ();
                                    }
                                    else if ((ev.lNetworkEvents & FD_READ) && ev.iErrorCode[FD_READ_BIT])
                                    {
                                        WSASetLastError(ev.iErrorCode[FD_READ_BIT]);
                                        Act = pSock -> Notify_Error ();
                                    }
                                    else if ((ev.lNetworkEvents & FD_WRITE) && ev.iErrorCode[FD_WRITE_BIT])
                                    {
                                        WSASetLastError(ev.iErrorCode[FD_WRITE_BIT]);
                                        Act = pSock -> Notify_Error ();
                                    }
                                    else if ((ev.lNetworkEvents & FD_CLOSE) && ev.iErrorCode[FD_CLOSE_BIT])
                                    {
                                        WSASetLastError(ev.iErrorCode[FD_CLOSE_BIT]);
                                        Act = pSock -> Notify_Error ();
                                    }
                                    break;
                                }
                                if  ( Act )
                                {
#ifdef DEBUG_SET_TEST_MESSAGE
                                    pSock->SetTestMessage(CMaaString::sFormat("ChangeMode(0x%0x) from Notify%s\n", Act, j == 1 ? "_Read" : j == 3 ? "_Write" : ""));
#endif
                                    ChangeMode ( pSock, Act );
                                }
                            }
                            catch( XTOOSockClose & x)
                            {
                                x;
                                /*
char txt[512];
sprintf(txt, "catch ( XTOOSockClose & ) %s", x.GetMsg());
Log(txt);
*/
                                IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                            }
                            catch( XTOOSockErr & SockErr )
                            {
                                //printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p, msg = %s\n", SockErr.GetSock (), SockErr.GetMsg() );
                                /*
char txt[512];
sprintf(txt, "catch(XTOOSockErr&) Sock = %p, msg = %s", SockErr.GetSock(), SockErr.GetMsg());
Log(txt);
*/
                                //DWORD x = WSAGetLastError();
                                //printf("Error: %d (0x%x)\n", x, x);
                                IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                                int xx;
                                if  (bConnect && SockErr.GetSock() == NULL && !m_SockHash.Find(Sockets[i], &xx))
                                {
                                    delete Sockets[i];
                                }
                                //RemoveSocket ( SockErr.GetSock () );
                                //delete SockErr.GetSock ();
                                //DelSock.m_pSock = NULL;
                            }
                        }
                        int Mode2;
                        if  (!m_SockHash.Find(Sockets[i], &Mode2) && Mode2 != Mode)
                        {
#ifdef DEBUG_SET_TEST_MESSAGE
                            Sockets[i]->SetTestMessage(CMaaString::sFormat("CurrentMode is changed to 0x%0x\n", Mode2));
#endif
                        }
                    }
                }
            }
            //return 1;
        }
        if  (w >= 2)
        {
            LOG1(25);
            return 1;
        }

    }
    catch(...)
    {
        LOG1(26);
        if  (SelectArraysUsedCount)
        {
            m_SelectArraysUsedCount -= SelectArraysUsedCount;
        }
        throw;
    }
    LOG1(27);
    if  (SelectArraysUsedCount)
    {
        m_SelectArraysUsedCount -= SelectArraysUsedCount;
    }
    LOG1(28);
    return 0;
}

#else // _WIN32

int CMaaFdSockets::Select ()
{
    fd_set Read, Write, Exept;
    struct timeval Timeval;
    int fStdin = 0;

    //Read  = m_Read;
    //Write = m_Write;
    //Exept = m_Exept;
    Timeval = m_Timeval;
    //printf("tv: %d %d\r\n", Timeval.tv_sec, Timeval.tv_usec);
    FD_ZERO ( &Read );
    FD_ZERO ( &Write );
    FD_ZERO ( &Exept );

    IfDebug(201)
    {
        printf ( "." ); fflush(stdout);
    }

    int k = m_SockHash.GetMaxIndex ();
    //CMaaPtr < CMaaSocket * > Sockets ( k );
    //CMaaPtr < int > Modes ( k );
    int SelectArraysUsedCount = 0;

    CMaaPtr<CMaaSocket *> tempSockets(0);
    CMaaPtr<int> tempModes(0);
    CMaaPtr<bool> temp_rev(0), temp_wev(0);

    if  (m_SelectArraysUsedCount == 0)
    {
        if  (m_SelectSockets.MaxIndex() >= (size_t)k && m_SelectModes.MaxIndex() >= (size_t)k &&
             m_Select_rev.MaxIndex() >= (size_t)(k + 16) &&
             m_Select_wev.MaxIndex() >= (size_t)(k + 16)
             )
        {
        }
        else
        {
            CMaaPtr<CMaaSocket *> Sockets(k, 1);
            CMaaPtr<int> Modes(k, 1);
            CMaaPtr<bool> rev(k + 16, 1), wev(k + 16, 1);
            m_SelectSockets.Swap(Sockets);
            m_SelectModes.Swap(Modes);
            m_Select_rev.Swap(rev);
            m_Select_wev.Swap(wev);
        }
        m_SelectArraysUsedCount = SelectArraysUsedCount = 1;
    }
    else
    {
        CMaaPtr<CMaaSocket *> Sockets(k, 1);
        CMaaPtr<int> Modes(k, 1);
        CMaaPtr<bool> rev(k + 16, 1), wev(k + 16, 1);
        tempSockets.Swap(Sockets);
        tempModes.Swap(Modes);
        temp_rev.Swap(rev);
        temp_wev.Swap(wev);
    }

    CMaaPtr<CMaaSocket *> &Sockets = SelectArraysUsedCount ? m_SelectSockets : tempSockets;
    CMaaPtr<int> &Modes = SelectArraysUsedCount ? m_SelectModes : tempModes;
    CMaaPtr <bool> &rev = SelectArraysUsedCount ? m_Select_rev : temp_rev;
    CMaaPtr <bool> &wev = SelectArraysUsedCount ? m_Select_wev : temp_wev;

    int Ret = -1;

    try
    {

        //CMaaPtr < CMaaSocket * > Sockets ( k, 1 );
        //CMaaPtr < int > Modes ( k, 1 );
        k = m_SockHash.EnumerateItems ( k, Sockets, Modes );
        //--CMaaPtr <bool> rev(k + 16), wev(k + 16);
        //CMaaPtr <bool> rev(k + 16, 1), wev(k + 16, 1);
        bool ev = false;

        IfDebug(202)
        {
            printf ( " k=%d ", k ); fflush(stdout);
        }

        bool b12 = false;
        bool brwev = false;
        int fSet = 0;
        for ( int i = 0; i < k; i++ )
        {
            IfDebug(201)
            {
                printf ( "pSock %p ", Sockets [ i ] ); fflush(stdout);
            }
            {
                int Mode;
                if  (Sockets[i] && (Sockets[i]->GetFdMode() & CMaaEnum::eHold))
                {
                    if  (--k == 0)
                    {
                        int us;
                        {
                            us = Timeval.tv_sec * 1000000 + Timeval.tv_usec;
                            GetWaitForTime(&us);
                            Timeval.tv_sec = us / 1000000;
                            Timeval.tv_usec = us % 1000000;
                        }
                        usleep(us);
                        return 0;
                    }
                    for (int j = i; j < k; j++)
                    {
                        Sockets[j] = Sockets[j + 1];
                        Modes[j] = Modes[j + 1];
                    }
                    i--;
                    continue;
                }
            }
            if  ( Sockets [ i ] )
            {
                SOCKET s = Sockets [ i ] -> GetSocket ();
                bool bIsTcpSock = dynamic_cast<CMaaTcpSocket*>(Sockets[i]) != NULL;
                int nsk = s;
                switch(nsk)
                {
                case 12:
                case 14:
                case 15:
                case 16:
                    //case 13:
                    b12 = true;
                }
                b12 = false;
                IfDebug(201)
                {
                    printf ( "SOCKET %d\n", s ); fflush(stdout);
                }
                int Mode = Modes [ i ];
                if  ( IsWinStdioSock ( s ) )
                {
                    if  ( ( Mode & eRead ) )
                    {
                        fStdin++;
                    }
                    continue;
                }
                if  ( Mode & eConnect )
                {
                    Mode = ( Mode | eWrite ) & ~eRead;
                    Mode = Mode|eWrite|eRead;
                }
#ifndef __FORCED_CLOSE_NOTIFICATION
                if  ((Mode & (eRead|eEventRead)) == eRead)
#else
                if  ((Mode & (eRead|eEventRead)) == eRead || (bIsTcpSock && (Mode & (eExcept|eEventRead)) == eExcept))
#endif
                {
                    FD_SET( s, & Read );     fSet++;
                    //__utf8_printf("FD_SET(%d, &Read)\n", s);
                }
                if  ((Mode & (eWrite|eEventWrite)) == eWrite)
                {
                    FD_SET( s, & Write );    fSet++;
                }
                if  ( ( Mode & eExept ) )
                {
                    FD_SET( s, & Exept );    fSet++;
                }

                rev[i] = wev[i] = false;
                CMaaTcpSocket * t = dynamic_cast<CMaaTcpSocket *>(Sockets[i]);
                if  (t)
                {
                    int wr = -1, rr = -1;
#ifdef __SHAPERS
                    if  (!(Mode & eConnect))
                    {
                        gLock.LockM();
                        if  (t->m_SndLLShaper.m_Shaper)
                        {
                            wr = t->m_SndLLShaper.GetLenReserved(1000000);
                        }
                        if  (t->m_RcvLLShaper.m_Shaper)
                        {
                            rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                        }
                        gLock.UnLockM();
                    }
                    IfDebug(1)
                    {
                        printf("rr = %d, wr = %d for socket %d\n", rr, wr, t->GetSocket());
                        fflush(stdout);
                    }
                    if  (b12)
                    {
                        printf("1. sk=%d, Mode = 0x%x\n", nsk, Mode);
                    }
                    if  (rr == 0 && wr == 0)
                    {
#ifndef __FORCED_CLOSE_NOTIFICATION
                        if  ((Mode & eRead))
#else
                        if  ((Mode & eRead) || (bIsTcpSock && (Mode & eRead|eExcept)))
#endif
                        {
                            FD_CLR(s, &Read);     fSet--;
                            //__utf8_printf("FD_CLR(%d, &Read)\n", s);
                        }
                        if  ((Mode & eWrite))
                        {
                            FD_CLR(s, &Write);    fSet--;
                        }
                        // hh[2 + 3 * i] = hEv0;
                        rev[i] = t->m_RcvLLShaper.m_hEv;
                        wev[i] = t->m_SndLLShaper.m_hEv;
                    }
                    else if (rr && wr)
                    {
                        //hh[2 + 3 * i + 1] = hh[2 + 3 * i + 2] = m_hEv0;
                    }
                    else if (rr == 0 && wr)
                    {
                        rev[i] = t->m_RcvLLShaper.m_hEv;
                        if  ((Modes[i] & (eEventWrite|eWrite)) != eWrite)
                        {
                            if  (FD_ISSET( s, & Read ))
                            {
                                FD_CLR( s, & Read );     fSet--;
                                //__utf8_printf("FD_CLR(%d, &Read)\n", s);
                            }
                            if  (FD_ISSET( s, & Write ))
                            {
                                FD_CLR( s, & Write );     fSet--;
                            }
                        }
                    }
                    else if (rr && wr == 0)
                    {
                        wev[i] = t->m_SndLLShaper.m_hEv;
#ifndef __FORCED_CLOSE_NOTIFICATION
                        if  ((Modes[i] & (eEventRead|eRead)) != eRead)
#else
                        if  ((Modes[i] & (eEventRead|eRead)) != eRead && (!bIsTcpSock || (Modes[i] & (eEventRead|eExcept)) != eExcept))
#endif
                        {
                            if  (FD_ISSET( s, & Read ))
                            {
                                FD_CLR( s, & Read );     fSet--;
                                //__utf8_printf("FD_CLR(%d, &Read)\n", s);
                            }
                            if  (FD_ISSET( s, & Write ))
                            {
                                FD_CLR( s, & Write );     fSet--;
                            }
                        }
                    }
#endif
                    if  (rr)
                    {
#ifndef __FORCED_CLOSE_NOTIFICATION
                        if  ((Modes[i] & (eEventRead|eRead)) == (eEventRead|eRead))
#else
                        if  ((Modes[i] & (eEventRead|eRead)) == (eEventRead|eRead) || (bIsTcpSock && (Modes[i] & (eEventRead|eExcept)) == (eEventRead|eExcept)))
#endif
                        {
                            ev = true;
                            if  (FD_ISSET( s, & Read ))
                            {
                                FD_CLR( s, & Read );     fSet--;
                                //__utf8_printf("FD_CLR(%d, &Read)\n", s);
                            }
                        }
                    }
                    if  (wr)
                    {
                        if  ((Modes[i] & (eEventWrite|eWrite)) == (eEventWrite|eWrite))
                        {
                            ev = true;
                            if  (FD_ISSET( s, & Write ))
                            {
                                FD_CLR( s, & Write );     fSet--;
                            }
                        }
                    }
                }
                ev = ev || rev[i] || wev[i];
                if  (b12)
                {
                    printf("2. sk=%d, Mode = 0x%x, ev = %s, rev[i]=%s, wev[i]=%s\n", nsk, Mode,
                         ev ? "true" : "false",
                         rev[i] ? "true" : "false",
                         wev[i] ? "true" : "false"
                         );
                    fflush(stdout);
                }
            }
        }
        //__utf8_printf("\n");
        errno = 0;
#ifndef __unix__
        WSASetLastError ( 0 );
#endif
        IfDebug(201)
        {
            printf ( "selecting " ); fflush(stdout);
        }
        int us;
        {
            us = Timeval.tv_sec * 1000000 + Timeval.tv_usec;
            GetWaitForTime(&us);
            if  (ev)
            {
                us = 0;
            }
            IfDebug(1)
            {
                Trace("us = %d\n", us);
            }
            Timeval.tv_sec = us / 1000000;
            Timeval.tv_usec = us % 1000000;
            //printf("tv2: %d %d\r\n", Timeval.tv_sec, Timeval.tv_usec);
            //fflush(stdout);
        }

        IfDebug(1) fflush(stdout);

        if  (b12)
        {
            Trace("b12: select, us = %d\n", us);
            for (int i = 0; i < k; i++)
            {
                //printf("%d\n", i); fflush(stdout);
                int Mode;
                if  (m_SockHash.Find(Sockets[i], &Mode))
                {
                    continue;
                }
                int s = Sockets[i]->GetSocket();
                bool bwu = false;

                const char * cn = Sockets[i]->GetConnectionName();

                if  (cn && !strcmp(cn, "CMaaWakeUpPair"))
                {
                    bwu = true;
                    //continue;
                }
                char txt[4];
                char * p = txt;

                if  (FD_ISSET(s, &Read))
                {
                    *p++ = !bwu ? 'r' : '1';
                }
                if  (FD_ISSET(s, &Write))
                {
                    *p++ = !bwu ? 'w' : '2';
                }
                if  (FD_ISSET(s, &Exept))
                {
                    *p++ = 'e';
                }
                *p = 0;
                printf("%d:m=0x%x:%s? ", s, Mode, txt);
                fflush(stdout);
            }
        }
        Ret = select ( FD_SETSIZE, &Read, &Write, &Exept, &Timeval );
        if  (b12)
        {
            Trace("b12: select() returns %d\n", Ret);

            for (int i = 0; i < k; i++)
            {
                int Mode;
                if  (m_SockHash.Find(Sockets[i], &Mode))
                {
                    continue;
                }
                int s = Sockets[i]->GetSocket();
                const char * cn = Sockets[i]->GetConnectionName();
                if  (cn && !strcmp(cn, "CMaaWakeUpPair"))
                {
                    continue;
                }
                char txt[4];
                char * p = txt;

                if  (FD_ISSET(s, &Read))
                {
                    *p++ = 'R';
                }
                if  (FD_ISSET(s, &Write))
                {
                    *p++ = 'W';
                }
                if  (FD_ISSET(s, &Exept))
                {
                    *p++ = 'E';
                }
                *p = 0;
                printf("%d:m=0x%x:=%s ", s, Mode, txt);
                fflush(stdout);
            }

        }
        if  ( Ret )
        {
            IfDebug(199) printf ( "select returns %d ",Ret );
            IfDebug(199) fflush(stdout);
        }
        IfDebug(201)
        {
            printf ( "ok. ret=%d ",Ret ); fflush(stdout);
        }


        // 20.11.2008
        if  (Ret > 0)
        {
            for ( int i = 0; i < k; i++ )
            {
                int Mode;
                if  (!m_SockHash.Find(Sockets[i], &Mode))
                {
                    //if   ( Sockets [ i ] )
                    SOCKET s = Sockets [ i ] -> GetSocket ();
                    /*
                    if   ( IsWinStdioSock ( s ) )
                    {
                         continue;
                    }
                    */
                    if  (FD_ISSET(s, &Read))
                    {
                        Mode |= eEventRead;
                    }
                    if  (FD_ISSET(s, &Write))
                    {
                        Mode |= eEventWrite;
                    }
                    /*
                    if   (FD_ISSET(s, &Exept))
                    {
                         strcat ( txt, "E" );
                    }
                    */
                    m_SockHash.AddOver(Sockets[i], Mode);
                }
            }
        }

        if  (b12)
        {
            for (int i = 0; i < k; i++)
            {
                int Mode;
                if  (m_SockHash.Find(Sockets[i], &Mode))
                {
                    continue;
                }
                int s = Sockets[i]->GetSocket();
                bool bwu = false;
                if  (!strcmp(Sockets[i]->GetConnectionName(), "CMaaWakeUpPair"))
                {
                    bwu = true;
                    //continue;
                }
                char txt[4];
                char * p = txt;

                if  (FD_ISSET(s, &Read))
                {
                    *p++ = !bwu ? 'R' : '1';
                }
                if  (FD_ISSET(s, &Write))
                {
                    *p++ = !bwu ? 'W' : '2';
                }
                if  (FD_ISSET(s, &Exept))
                {
                    *p++ = 'E';
                }
                *p = 0;
                printf("%d:m=0x%x:=%s ", s, Mode, txt);
                fflush(stdout);
            }
        }


        // 24.10.2004
        if  ( 0 && Ret > 0 )
        {
            for ( int i = 0; i < k; i++ )
            {
                //IfDebug(201) printf ( "pSock %p ", Sockets [ i ] ); fflush(stdout);
                if  ( Sockets [ i ] )
                {
                    SOCKET s = Sockets [ i ] -> GetSocket ();
                    //IfDebug(201) printf ( "SOCKET %d\n", s ); fflush(stdout);
                    //int Mode = Modes [ i ];
                    if  ( IsWinStdioSock ( s ) )
                    {
                        continue;
                    }
                    char txt [ 256 ];
                    * txt = 0;
                    if  ( FD_ISSET( s, &Read ) )
                    {
                        strcat ( txt, "R" );
                    }
                    if  ( FD_ISSET( s, &Write ) )
                    {
                        strcat ( txt, "W" );
                    }
                    if  ( FD_ISSET( s, &Exept ) )
                    {
                        strcat ( txt, "E" );
                    }
                    if  ( txt [ 0 ] )// && strcmp(txt,"R") && strcmp(txt,"W") )
                    {
                        printf ( "Sock %d: %s\n", s, txt );fflush(stdout);
                    }
                }
            }
        }
        if  ( Ret < 0 && !fSet )
        {
            FD_ZERO ( &Read );
            FD_ZERO ( &Write );
            FD_ZERO ( &Exept );
        }
        if  ( Ret < 0 && !fSet )
        {
          #ifndef __unix__
            int s = m_Timeval.tv_sec * 1000 + m_Timeval.tv_usec / 1000; s = s ? s : 1;
            Sleep ( s );
          #else
            struct timespec req, rem;
            req.tv_sec  = m_Timeval.tv_sec;
            req.tv_nsec = m_Timeval.tv_usec * 1000;
            nanosleep ( &req, &rem );
          #endif
            Ret = 0;
        }
        if  ( fStdin )
        {
#ifndef __unix__
            fStdin = kbhit ();
            if  ( Ret >= 0 && fStdin )
            {
                Ret++;
                FD_SET( _fileno ( stdin ), & Read );
            }
#endif
        }
        if  ( Ret > 0 || ev )
        {
            IfDebug(1) printf ( "\n" );
            int Errno = errno;
            for ( int i = 0; i < k; i++ )
            {
                for ( int j = 0; j < 3; j++ )
                {
                    int Mode;
                    if  ( !m_SockHash.Find ( Sockets [ i ], &Mode ) )
                    {
                        if  (b12 && j == 0)
                        {
                            printf("b12: i=%d, socket=%d, Mode=0x%x\n", i, Sockets[i]->GetSocket(), Mode);
                            fflush(stdout);
                        }
                        //bool bConnect = false;
                        try
                        {
                            unsigned int Act = 0;
                            CMaaSocket * pSock = Sockets [ i ];
                            SOCKET s = pSock -> GetSocket ();
                            switch( j )
                            {
                            case 1:
                                if  ((Mode & eEventRead) && (Mode & (eRead|eConnect)))
                                {
                                    if  (Mode & eConnect)
                                    {
                                        IfDebug(100)
                                        Trace ( "ConnectAgain for %d %s", s, Sockets[i]->GetConnectionName() );
                                        //bConnect = true;
                                        try
                                        {
                                            Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                        }
                                        catch(XTOOSockErr & SockErr)
                                        {
                                            int xx;
                                            if  (SockErr.GetSock() == NULL && !m_SockHash.Find(pSock, &xx))
                                            {
                                                Act = pSock->Notify_Error();
                                            }
                                            else
                                            {
                                                throw;
                                            }
                                        }
                                        IfDebug(100)
                                        Trace ( "ConnectAgain for %d %s - done", s, Sockets[i]->GetConnectionName() );
                                    }
                                    else
                                    //if   ( FD_ISSET( s, &Read ) )
                                    {
                                        IfDebug(100)
                                        Trace ( "Notify_Read for %d %s", s, Sockets[i]->GetConnectionName() );
                                        //printf ( "R%d ", s );
                                        Act = pSock -> Notify_Read ();
                                        IfDebug(100)
                                        Trace ( "Notify_Read for %d %s - done", s, Sockets[i]->GetConnectionName() );
                                    }
                                }
                                break;
                            case 2:
                                if  ((Mode & eEventWrite) && (Mode & (eWrite|eConnect)))
                                //if   ( FD_ISSET( s, &Write ) )
                                {
                                    //printf ( "W%d ", s );
                                    if  ( Mode & eConnect )
                                    {
                                        IfDebug(100)
                                        Trace ( "ConnectAgain for %d %s", s, Sockets[i]->GetConnectionName() );
                                        try
                                        {
                                            Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                        }
                                        catch(XTOOSockErr & SockErr)
                                        {
                                            int xx;
                                            if  (SockErr.GetSock() == NULL && !m_SockHash.Find(pSock, &xx))
                                            {
                                                Act = pSock->Notify_Error();
                                            }
                                            else
                                            {
                                                throw;
                                            }
                                        }
                                        IfDebug(100)
                                        Trace ( "ConnectAgain for %d %s - done", s, Sockets[i]->GetConnectionName() );
                                    }
                                    else
                                    {
                                        IfDebug(100)
                                        Trace ( "Notify_Write for %d %s", s, Sockets[i]->GetConnectionName() );
                                        //CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                        //_qword x = p ? p->GetDataSentSize() : 0;
                                        Act = pSock->Notify_Write();
                                        //if   ((Act & eWrite) && p && p->GetDataSentSize() != x)
                                        //{
                                        //     WakeUp();
                                        //}
                                        IfDebug(100)
                                        Trace ( "Notify_Write for %d %s - done", s, Sockets[i]->GetConnectionName() );
                                    }
                                }
                                break;
                            case 0:
                                if  ( FD_ISSET( s, &Exept ) )
                                {
                                    IfDebug(100)
                                    Trace ( "Notify_Exept for %d %s", s, Sockets[i]->GetConnectionName() );
                                    //printf ( "E%d ", s );
                                    IfDebug( 1 )
                                    {
                                        printf ( "!!! exeption for socket %d !!!\n", s );
                                        printf ( "ConnectionName: %s\n", pSock -> GetConnectionName () );
                                    }
                                    errno = Errno;
                                    Act = pSock -> Notify_Error ();
                                    IfDebug(100)
                                    Trace ( "Notify_Exept for %d %s - returned", s, Sockets[i]->GetConnectionName() );
                                }
#ifdef __FORCED_CLOSE_NOTIFICATION
                                else if ((Mode & eEventRead) && (Mode & (eRead|eConnect|eExcept)) == eExcept)
                                {
                                    CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket*>(pSock);
                                    if  (p)
                                    {
                                        char c = 0;
                                        int l = recv(pSock->GetSocket(), &c, 1, MSG_PEEK);

                                        if  (l == 0)
                                        {
                                            int ErrnoBackup = errno;
                                            errno = CMaa_CONN_CLOSED_ERROR;
                                            Act = pSock -> Notify_Error ();
                                            errno = ErrnoBackup;
                                        }
                                        else if (l < 0)
                                        {
                                            int ErrnoBackup = errno;
                                            Act = pSock -> Notify_Error ();
                                            errno = ErrnoBackup;
                                        }
                                    }
                                }
#endif
                                break;
                            }
                            if  ( Act )
                            {
                                IfDebug(100)
                                {
                                    char txt[1024];
                                    sprintf(txt, "ChangeMode(pSock=%p (%d), 0x%x), %s", pSock, s, Act, Sockets[i]->GetConnectionName());
                                    Trace(txt, 0);
                                }
                                ChangeMode ( pSock, Act );
                            }
                        }
                        catch( XTOOSockClose & )
                        {
                            IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                        }
                        catch( XTOOSockErr & SockErr )
                        {
                            IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                            /*
                              int xx;
                              if (bConnect && SockErr.GetSock() == NULL && !m_SockHash.Find(Sockets[i], &xx))
                              {
                                  delete Sockets[i];
                              }
                              */

                            //RemoveSocket ( SockErr.GetSock () );
                            //delete SockErr.GetSock ();
                            //DelSock.m_pSock = NULL;
                        }
                    }
                }
            }
        }
    }
    catch(...)
    {
        if  (SelectArraysUsedCount)
        {
            m_SelectArraysUsedCount -= SelectArraysUsedCount;
        }
        throw;
    }
    if  (SelectArraysUsedCount)
    {
        m_SelectArraysUsedCount -= SelectArraysUsedCount;
    }
    return Ret;
}

#endif
//---------------------------------------------------------------------------
int CMaaFdSockets::GetMode(CMaaSocket * pSock)
{
    int Mode = 0;
    m_SockHash.Find(pSock, &Mode);
    return Mode;
}
//---------------------------------------------------------------------------
void CMaaFdSockets::ChangeMode ( CMaaSocket * pSock, int Act )
{
    /*
if (Act & eConnect)
{
    printf("Act = %x\n", Act);
    fflush(stdout);
}
*/
    LOG1(200);
    int Mode = 0;
    if  ( !m_SockHash.Find ( pSock, &Mode ) && Act && pSock )
    {
#ifdef DEBUG_SET_TEST_MESSAGE
        pSock->SetTestMessage(CMaaString::sFormat("Changing mode, Act=0x%x\n", Act), Act == 0x100 ? 0x100 : -0x100);
#endif
        /*
if (Act & eConnect)
{
    printf("Act = %x, Mode = %x, Mode | Act = %x, ((Act & eDisableAll) >> eDisableShift) = %x\n", Act, Mode, Mode | Act, ((Act & eDisableAll) >> eDisableShift));
    fflush(stdout);
}
*/
        LOG1(201);
        int PrevMode = Mode;
        Mode = ( Mode | Act ) & ~((Act & (eDisableAll|eDisableAllEvents|eDisableHold)) >> eDisableShift );
        Mode = Mode & ~(eDisableAll|eDisableAllEvents|eDisableHold);
        if  (g_____socket == pSock->GetSocket() && (Mode & (eRead|eConnect)) == eRead)
        {
            static int aa = 0;
            aa++;
            aa++;
            aa++;
#ifdef DEBUG_SET_TEST_MESSAGE
            pSock->SetTestMessage(CMaaString::sFormat("Changing mode, Act=0x%x\n", Act), 0x12345678);
#endif
        }
        m_SockHash.AddOver ( pSock, Mode );
        /*
if (Act & eConnect)
{
    printf("Mode = %x\n", Mode);
    fflush(stdout);
}
*/
        LOG1(202);
        SOCKET s = pSock -> GetSocket ();
        if  ( IsWinStdioSock ( s ) )
        {
            return;
        }
#ifdef _WIN32
        int a = (((Act & (eRead|eWrite|eConnect)) & ~((Act & (eDisableRead|eDisableWrite|eDisableConnect)) >> eDisableShift)));
        a = Mode;

#ifdef DEBUG_SET_TEST_MESSAGE
        pSock->SetTestMessage(CMaaString::sFormat("ChangeMode(from 0x%04x to 0x%04x, Act=0x%x)\n", PrevMode, Mode, Act));
#endif
        long m = 0;
        if  ((a & (eRead|eExept)) && (a & (eRead|eEventRead)) != eEventRead)
        {
            if  (pSock->m_AcceptFlag && (a & eRead))
            {
                m |= FD_ACCEPT;
            }
            else if (!pSock->m_AcceptFlag)
            {
                m |= FD_READ;
                if  (dynamic_cast<CMaaTcpSocket *>(pSock))
                {
                    m |= FD_CLOSE;
                }
            }
        }
        if  (a & eWrite)
        {
            m |= FD_WRITE;
        }
        if  (a & eConnect)
        {
            m |= FD_CONNECT;
            m &= ~(FD_READ|FD_WRITE|FD_CLOSE);
        }
        if  (Mode & eHold)
        {
            m = 0;
        }

        LOG1(203);
        if  (pSock->m_LastMode == -1 || pSock->m_LastMode != a)
        {
            if  (pSock->m_LastMode == -1)
            {
                pSock->m_pm = -1;
                m_bForceMode = true;
            }
            if  ((pSock->m_LastMode ^ a) & eSensitiveEvents)
            {
                m_bForceMode = true;
            }
            pSock->m_LastMode = a;
        }
        //printf("WSAEventSelect(%d, , 0x%x)\n", (int)s, (int)m);
        LOG1(204);
        if  (m != pSock->m_pm)
        {
            pSock->m_pm = m;

            LOG1(205);
            /*
               static CMaaMutex mtx;
               mtx.Lock();
               try
               {
               */
            if  (WSAEventSelect(s, pSock->m_Ev, m))
            {
                LOG1(206);
                DWORD dw = WSAGetLastError();
                //
            }
            LOG1(207);
            /*
               }
               catch(...)
               {
                   mtx.UnLock();
                   throw;
               }
               mtx.UnLock();
               */

            if  ((m & (FD_READ|FD_WRITE|FD_CLOSE))) //10.01.2008
            {
                if  (pSock->m_bDebug)
                {
#ifdef DEBUG_SET_TEST_MESSAGE
                    pSock->SetTestMessage(CMaaString::sFormat("WSAEventSelect(, %x); WSASetEvent()\n", m));
#endif
                }
                //printf("WSASetEvent()\n");
                //WSASetEvent(pSock->m_Ev);
                LOG1(208);
                WakeUp();
            }
        }
#else
        /*
        if   ( ( Act & eRead ) )
          {
               FD_SET( s, & m_Read );
          }
          if   ( ( Act & eWrite ) )
          {
               FD_SET( s, & m_Write );
          }
          if   ( ( Act & eExept ) )
          {
               FD_SET( s, & m_Exept );
          }
          Act = Act >> eDisableShift;
          if   ( ( Act & eRead ) )
          {
               FD_CLR( s, & m_Read );
          }
          if   ( ( Act & eWrite ) )
          {
               FD_CLR( s, & m_Write );
          }
          if   ( ( Act & eExept ) )
          {
               FD_CLR( s, & m_Exept );
          }
*/
#endif
    }
    else
    {
        /*
if (Act & eConnect)
{
    printf("not in hash\n");
    fflush(stdout);
}
*/
    }
    LOG1(209);
}
//---------------------------------------------------------------------------
void CMaaFdSockets::DeleteAllSockets ()
{
    IfDebug(100) printf ( "CMaaFdSockets::DeleteAllSockets ()\n" );
    int k = (warning_int)m_SockHash.GetMaxIndex ();
    //CMaaPtr < CMaaSocket * > Sockets ( k );
    //CMaaPtr < int > Modes ( k );
    CMaaPtr < CMaaSocket * > Sockets ( k );
    CMaaPtr < int > Modes ( k );
    k = m_SockHash.EnumerateItems ( k, Sockets, Modes );

    for ( int i = 0; i < k; i++ )
    {
        if  ( Sockets [ i ] && !m_SockHash.Find ( Sockets [ i ], NULL ) )
        {
            IfDebug(100)
            {
                printf ( "delete socket %p ", Sockets [ i ] );
                const char * p = Sockets [ i ] -> GetConnectionName ();
                p = p ? p : "<unknown>";
                printf ( "%s\n", p );
            }
            delete Sockets [ i ];
        }
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CMaaSockStartup::CMaaSockStartup(BOOL fThrow)
#ifdef __SOCK_NEW_TIMERS
:   m_Time(GetHRTime())
#endif
{
    /*
     FILE * f = fopen("c:\\ftp2.txt", "a+b");
     if   (f)
     {
          fprintf(f, "CMaaSockStartup::CMaaSockStartup\n");
          fclose(f);
     }
     */

    m_IsOk = false;
    gCMaaFdSockets = NULL; //new CMaaFdSockets();
    m_pTcpFileMainThread = NULL;
#ifndef __unix__
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(2, 2);
    if  (WSAStartup(wVersionRequested, &m_WsaData) == SOCKET_ERROR)
    {
        //LastWsaError = WSAGetLastError ();
        if  (fThrow)
        {
            throw XTOOSockErr ( "WSAStartup failed", NULL );
        }
        else
        {
            return;
        }
    }
#endif
    //gCMaaConnectTasks = TL_NEW CMaaTasks;
    //m_pTcpFileMainThread = TL_NEW CMaaTcpFileMainThread;
    StartTcpFileSockets();

    m_IsOk = m_pTcpFileMainThread ? true : false;
}
//---------------------------------------------------------------------------
#ifdef __SOCK_NEW_TIMERS
_qword CMaaSockStartup::GetTime()
{
    m_TimerMutex.LockM();
    _qword t = m_Time.GetTime(1000000);
    m_TimerMutex.UnLockM();
    return t;
}
#endif
//---------------------------------------------------------------------------
bool CMaaSockStartup::StopTcpFileSockets()
{
    gLock.LockM();
    CMaaTcpFileMainThread * p = m_pTcpFileMainThread;
    m_pTcpFileMainThread = NULL;
    gLock.UnLockM();
    delete p;
    return p != NULL;
}

bool CMaaSockStartup::StartTcpFileSockets()
{
    bool bRetCode = true;
    gLock.LockM();
    if  (!m_pTcpFileMainThread)
    {
        m_pTcpFileMainThread = TL_NEW CMaaTcpFileMainThread(NULL, false);
        gLock.UnLockM();
        if  (m_pTcpFileMainThread)
        {
            if  (m_pTcpFileMainThread->Create())
            {
                return true;
            }
            StopTcpFileSockets();
        }
        bRetCode = false;
    }
    else
    {
        gLock.UnLockM();
    }
    return bRetCode;
}
//---------------------------------------------------------------------------
CMaaSockStartup::~CMaaSockStartup ()
{
    //printf("CMaaSockStartup::~CMaaSockStartup() - SetExitFileSockThreads()\n"); fflush(stdout);
    SetExitFileSockThreads();

    //printf("CMaaSockStartup::~CMaaSockStartup() - 1\n"); fflush(stdout);
    delete m_pTcpFileMainThread;

    //printf("CMaaSockStartup::~CMaaSockStartup() - 2\n"); fflush(stdout);
    delete gCMaaFdSockets;
    gCMaaFdSockets = NULL;
    //printf("CMaaSockStartup::~CMaaSockStartup() - 3\n"); fflush(stdout);

#ifdef __SHAPERS
    //printf("CMaaSockStartup::~CMaaSockStartup() - 4\n"); fflush(stdout);
    m_ShaperThread.Exit();
#endif

#ifndef __unix__
    //printf("CMaaSockStartup::~CMaaSockStartup() - 5\n"); fflush(stdout);
    WSACleanup ();
#endif
}
//---------------------------------------------------------------------------
bool CMaaSockStartup::IsOk()
{
    return m_IsOk;
}
//---------------------------------------------------------------------------
int CMaaSockConsole::EnterString ( const char * str )
{
    if  ( ! strcmp ( str, "exit" ) || ! strcmp ( str, "quit" ) )
    {
        printf ( "Exiting..." );
        throw "User break";
    }
    return 0;
}
//---------------------------------------------------------------------------
void CMaaSockConsole::Escape ()
{
    // for windows only
    printf ( "Exiting..." );
    throw "User break";
}
//---------------------------------------------------------------------------
int CMaaSockConsole::Notify_Read  ()
{
#ifndef __unix__
    int fRet = 1;
    while( kbhit () && m_Pos < sizeof ( m_Buff ) - 1 )
    {
        char c = getch ();
        if  ( c == 0x1b )
        {
            Escape ();
            continue;
        }
        if  ( c != '\r' && c != '\n' )
        {
            printf ( "%c", c );
            m_Buff [ m_Pos++ ] = c;
            if  ( m_Pos >= sizeof ( m_Buff ) - 1 )
            {
                m_Buff [ m_Pos ] = 0;
                fRet = 0;
                break;
            }
        }
        if  ( c == '\r' || c == '\n' )
        {
            //printf ( "\n", c );
            printf("\n");
            m_Buff [ m_Pos ] = 0;
            fRet = 0;
            break;
        }
    }
    if  ( fRet )
    {
        return 0;
    }
    m_Pos = 0;
#else
    if  ( !fgets ( m_Buff, sizeof ( m_Buff ), stdin ) )
    {
        return 0;
    }
    if  ( m_Buff [ strlen ( m_Buff ) - 1 ] == '\n' )
    {
        m_Buff [ strlen ( m_Buff ) - 1 ] = 0;
    }
#endif
    return EnterString ( m_Buff );
}
//---------------------------------------------------------------------------
int CMaaSockConsole::Notify_Write ()
{
    return eDisableWrite;
}
//---------------------------------------------------------------------------
const char * CMaaSockConsole::GetConnectionName ()
{
    return "stdin console";
}
//---------------------------------------------------------------------------
CMaaSockConsole::CMaaSockConsole(CMaaFdSockets * pFdSockets)
:   CMaaTcpSocket(pFdSockets),
    m_Pos ( 0 )
{
#ifdef __unix__
    SetSocket ( 0 );
#else
    SetSocket ( _fileno ( stdin ) );
#endif
    AddFdSocket ();
    ChangeFdMode ( eRead );
}
//---------------------------------------------------------------------------
CMaaSockConsole::~CMaaSockConsole ()
{
    RemoveFdSocket ();
    m_Socket = INVALID_SOCKET;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Read ()
{
    if  (m_len < (int)sizeof(m_Buffer))
    {
        int r = RecvData(&m_Buffer[m_len], (int)sizeof(m_Buffer) - m_len);
        if  (IsClosed(r))
        {
            // ending connection
            //printf ( "\nReceiving 0 bytes of data - closing connection.\n" );
            return CloseByException ( "Receiving 0 bytes of data - closing connection.");
        }
        fwrite(&m_Buffer[m_pos], 1, r, stdout);
        m_len += r;
        return eWrite;
    }
    return eDisableRead;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Write ()
{
    if  (m_pos < m_len)
    {
        int w = SendData(&m_Buffer[m_pos], m_len - m_pos);
        m_pos += w;
        if  (w > 0)
        {
            memmove(m_Buffer, m_Buffer + m_pos, m_len - m_pos);
            m_len -= m_pos;
            m_pos = 0;
        }
        return (m_len < (int)sizeof(m_Buffer)) ? eRead : 0;
    }
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Error ()
{
    //printf ( "\nSome error in Echo server...\n" );
    CloseByException ( "Some error in Echo server..." );
    return 0;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Accepted ( _IP IpFrom, _Port Port )
{
    //printf ( "\nAccepted connection from %s:%d\n", CMaaIpToText ( IpFrom ), Port );
    //SendData ( "", 0 );
    return 0;
}
//---------------------------------------------------------------------------
CMaaEchoConnection::CMaaEchoConnection(CMaaFdSockets * pFdSockets, CMaaEchoServer * pServer)
:   CMaaTcpSocket(pFdSockets)
{
    m_pos = m_len = 0;
}
//---------------------------------------------------------------------------
CMaaEchoConnection::~CMaaEchoConnection()
{
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// Servers
//---------------------------------------------------------------------------
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets * pFdSockets, int Port, const char * ServerName)
:   CMaaTcpSocket(pFdSockets)
{
    ServerName = ServerName ? ServerName : "CMaaUniversalServer";
    m_ServerName.Format("%s on port %d", ServerName, Port);
    Bind ( Port );
    Listen ();
    AddFdSocket ();
}
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets * pFdSockets, CMaaString Port, const char * ServerName)
:   CMaaTcpSocket(pFdSockets)
{
    ServerName = ServerName ? ServerName : "CMaaUniversalServer";
    m_ServerName.Format("%s on %S", ServerName, &Port);
    Bind ( Port );
    Listen ();
    AddFdSocket ();
}
//---------------------------------------------------------------------------
CMaaUnivServer::~CMaaUnivServer ()
{
    RemoveFdSocket ();
}
//---------------------------------------------------------------------------
//CMaaTcpSocket * CMaaUnivServer::CreateNewConnection(CMaaFdSockets * pFdSockets)
//{
//     return NULL;
//}
//---------------------------------------------------------------------------
extern int gAcceptCatchErrors;
int gAcceptCatchErrors = 0;
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Read ()
{
    // TODO: Create thread, and CMaaFdSockets
    //
    // CMaaFdSockets * pFdSockets

    CMaaSockThread * Thr = NULL;

    _IP IpFrom = 0;
    _Port Port = 0;
    SOCKET s = INVALID_SOCKET;

    CMaaTcpSocket * pConnection = NULL;
    try
    {
        s = Accept ( &IpFrom, &Port );
        Thr = m_pFdSockets->m_pThread->CreateNewThread();
        if  (Thr)
        {
            //printf("Thr->m_pFdSockets = %p\n", Thr->m_pFdSockets);
            pConnection = CreateNewConnection(Thr->m_pFdSockets);
            //printf("pConnection = %p\n", pConnection);
        }
    }
    catch(...)
    {
        if  ( s != INVALID_SOCKET )
        {
            closesocket ( s );
        }
        if  (Thr)
        {
            delete Thr;
        }
        throw;
    }
    if  ( ! pConnection )
    {
        closesocket ( s );
        if  (Thr)
        {
            delete Thr;
        }
    }
    else
    {
        //printf("1.  Thr->m_pFdSockets = %p, pConnection = %p\n", Thr->m_pFdSockets, pConnection);
        pConnection -> SetSocket ( s );
        pConnection -> AddFdSocket ();
        // :::TEST:::
#ifdef __unix__
        IfDebug(1)
        {
            //printf("");
            //Trace("12345: Accepted socket %d, setting nonblocking mode", s);
        }
        pConnection->SetNonBlockingMode();
#endif

        //printf("2.  Thr->m_pFdSockets = %p, pConnection = %p\n", Thr->m_pFdSockets, pConnection);
        bool bError = false;
        try
        {
            pConnection->UpdateHrTime(e_fds_connected_at_time);
            pConnection -> ChangeFdMode ( pConnection -> Notify_Accepted ( IpFrom, Port ) );
            //printf("after Notify_Accepted() and ChangeFdMode() pConnection->GetFdMode() returns %x\n", pConnection->GetFdMode());

            //printf("3.  Thr->m_pFdSockets = %p, pConnection = %p\n", Thr->m_pFdSockets, pConnection);
            if  (!Thr->Create())
            {
                //printf("!Thr->Create()\n");
                delete Thr;
                Thr = NULL;
                StaticError("Can't create thread");
            }
        }
        catch(...)
        {
            bError = true;
            gAcceptCatchErrors++;
        }
        if  (bError)
        {
            delete Thr;
        }
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
int CMaaUnivServer::Notify_Error ()
{
    CloseByException ( "Some error in listened socket...\n" );
    return 0;
}
//---------------------------------------------------------------------------
const char * CMaaUnivServer::GetConnectionName ()
{
    return m_ServerName;
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
const char * CMaaTcpConnection::GetConnectionName ()
{
    return m_ConnName;
}
//---------------------------------------------------------------------------
void CMaaTcpConnection::Send ( const char * Buff, int Size )
{
    Buffer * pb = TL_NEW Buffer ( Size, Buff );
    m_SendList.AddAtBack ( pb );
    //printf("Send(): GetFdMode() for %d is 0x%x\n", m_Socket, GetFdMode());
    if  (GetFdMode() & eConnect)
    {
        return;
    }
    ChangeFdMode ( eAll );
    Notify_Write ();
}
//---------------------------------------------------------------------------
void CMaaTcpConnection::SetName ( const char * ConnName )
{
    m_ConnName.Format ( "%s %s", m_ClassName, ( const char * ) ConnName );
}
//---------------------------------------------------------------------------
CMaaTcpConnection::CMaaTcpConnection(CMaaFdSockets * pFdSockets, const char * ClassName )
:   CMaaTcpSocket(pFdSockets),
    m_ClassName ( ClassName )
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
    Buffer & b = * m_SendList.GetFromFront ();
    if  ( ! &b )
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
CMaaTcpProxy::CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaString & TcpServName)
:   CMaaTcpConnection(pFdSockets, szClassName),
    fIsClientSide ( 1 ),
    pOtherSide ( NULL ),
    m_TcpServName ( TcpServName ),
    fCloseByRead0 ( 1 ),
    fNeedToClose ( 0 ),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    pOtherSide = TL_NEW CMaaTcpProxy(this->m_pFdSockets, this, m_TcpServName);
    AddFdSocket();
    m_Timer0.Attach(m_pFdSockets);
}
//---------------------------------------------------------------------------
CMaaTcpProxy::CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaTcpProxy * pClientSide, CMaaString & TcpServName )
:   CMaaTcpConnection(pFdSockets, szClassName),
    fIsClientSide ( 0 ),
    pOtherSide ( NULL ),
    m_TcpServName ( TcpServName ),
    fCloseByRead0 ( 1 ),
    fNeedToClose ( 0 ),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    CMaaString tmp;
    tmp.Format ( "C ....->%s", ( const char * ) TcpServName );
    SetName ( tmp );
    AddFdSocket ();
#ifdef __SHAPERS
    gLock.LockM();
    if  (gpSockStartup)
    {
        /*
          IfDebug(100)
          {
               printf("Adding shapers...\n"); fflush(stdout);
          }
          gpSockStartup->m_ShaperThread.m_Snd1000->Add(m_SndLLShaper);
          gpSockStartup->m_ShaperThread.m_Rcv1000->Add(m_RcvLLShaper);
*/
    }
    gLock.UnLockM();
#endif
    int x = AsyncConnect ( m_TcpServName );
    //     IfDebug(2) printf ( "Connect() retunts %d, socket = %d\n", x, GetSocket () );
    IfDebug(2) printf ( "AsyncConnect() retunts %d, socket = %d\n", x, GetSocket () );
    tmp.Format ( "%d->%s", GetBindedPort (), ( const char * ) TcpServName );
    SetName ( tmp );
    pOtherSide = pClientSide;
    //     AddFdSocket ();
    m_Timer0.Attach(m_pFdSockets);
    /*
     if (!x)
     {
        ChangeFdMode ( eDisableConnect|eAll );
#ifdef __unix__
        SetNonBlockingMode();
#endif
     }
     else
     {
        StaticError("Connect");
        //throw 1;
     }
*/
}
//---------------------------------------------------------------------------
void CMaaTcpProxy::OnTimer(int f)
{
    switch(f)
    {
    case 0:
        CloseByException("Close");
        break;
    }
}
//---------------------------------------------------------------------------
int CMaaTcpProxy::Notify_Read  ()
{
    char Buff [ 4096 + 1 ];
    IfDebug(100)
    {
        printf("CMaaTcpProxy::Notify_Read() for socket %d,  Recving Data...\n", m_Socket); fflush(stdout);
    }
    int r = RecvData ( Buff, sizeof ( Buff ) - 1 );
    IfDebug(100)
    {
        printf("RecvData: %d bytes !!!!!!!!\n", r); fflush(stdout);
    }

    if  (IsClosed(r))
    {
        // ending connection
        //.....
        if  ( pOtherSide )
        {
            pOtherSide -> pOtherSide = NULL;
            pOtherSide -> fNeedToClose = 1;
            pOtherSide->m_Timer0.Start(1);
            pOtherSide = NULL;
        }
        return CloseByException ( "closing connection" );
    }
    /*
     if   ( ! pOtherSide )
     {
          try
          {
               pOtherSide = TL_NEW CMaaTcpProxy(this->m_pFdSockets, this, m_TcpServName);
          }
          catch ( XTOOSockErr & )
          {
          }
          if   ( !pOtherSide )
          {
               //SendHttpErrorResponse ( "Can't connect to server\n\r\n\r" );
               return CloseByException ( "Can't create connection to server" );
          }
     }
*/
    if  ( pOtherSide && r > 0 )
    {
        IfDebug(100)
        {
            printf("CMaaTcpProxy::Notify_Read() for socket %d,  writing %d bytes data to socket %d...\n", m_Socket, r, pOtherSide->m_Socket); fflush(stdout);
        }
        pOtherSide -> Send ( Buff, r );
        IfDebug(100)
        {
            printf("CMaaTcpProxy::Notify_Read() for socket %d,  writing %d bytes data to socket %d - return from Send()\n", m_Socket, r, pOtherSide->m_Socket); fflush(stdout);
        }
    }
    return eRead;
}
//---------------------------------------------------------------------------
int CMaaTcpProxy::Notify_Write ()
{
    //printf("GetFdMode() for %d is 0x%x\n", m_Socket, GetFdMode());
    IfDebug(100)
    {
        printf("CMaaTcpProxy::Notify_Write() for socket %d\n", m_Socket); fflush(stdout);
    }
    if  (GetFdMode() & eConnect)
    {
        IfDebug(100)
        {
            Trace("CMaaTcpProxy::Notify_Write(): return 0 for socket %d: (GetFdMode() & eConnect)\n", (int)m_Socket);
        }
        return 0;
    }
    Buffer * pb;
    int fBuff = 0;
    while( ( pb = m_SendList.GetFromFront () ) )
    {
        fBuff++;
        Buffer & b = * pb;
        IfDebug(100)
        {
            printf("CMaaTcpProxy::Notify_Write(): sending %d bytes to socket %d...\n", b.m_BuffSize - b.m_Pos, m_Socket); fflush(stdout);
        }
        int xx = 0;
        b.m_Pos += (xx = SendData ( b.m_pBuffer + b.m_Pos, b.m_BuffSize - b.m_Pos));
        IfDebug(100)
        {
            printf("CMaaTcpProxy::Notify_Write(): %d bytes are sent to socket %d...\n", xx, m_Socket); fflush(stdout);
        }

        //printf("b.m_Pos = %d, b.m_BuffSize = %d\n", b.m_Pos, b.m_BuffSize);
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
            //printf("point 1: fNeedToClose\n");
            return CloseByException ( "closing connection (after write all data)" );
        }
        IfDebug(100)
        {
            Trace("CMaaTcpProxy::Notify_Write(): return (eAll|eDisableWrite) for socket %d\n", (int)m_Socket);
        }
        return eDisableWrite;
    }
    IfDebug(100)
    {
        Trace("CMaaTcpProxy::Notify_Write(): return eAll for socket %d\n", (int)m_Socket);
    }
    return eWrite;
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
        pOtherSide -> pOtherSide = NULL;
        IfDebug(100) printf ( "delete        %p %s\n", pOtherSide, pOtherSide -> GetConnectionName () );
        delete pOtherSide;
    }
}
//---------------------------------------------------------------------------
const char CMaaTcpProxy::szClassName [] = "TcpProxy";
//---------------------------------------------------------------------------
CMaaTcpSocket * CMaaTcpProxyServer::CreateNewConnection(CMaaFdSockets * pFdSockets)
{
    return TL_NEW CMaaTcpProxy(pFdSockets, m_ServName);
}
//---------------------------------------------------------------------------
CMaaTcpProxyServer::CMaaTcpProxyServer(CMaaFdSockets * pFdSockets, int Port, const char * Server)
:   CMaaUnivServer(pFdSockets, Port, "TcpProxyServer"),
    m_ServName ( Server )
{
}
//---------------------------------------------------------------------------


//------------
//---------------------------------------------------------------------------
// class CMaaTcpStdoutConnection
//---------------------------------------------------------------------------
CMaaTcpStdoutConnection::CMaaTcpStdoutConnection(CMaaFdSockets * pFdSockets, CMaaString & TcpServName)
:   CMaaTcpConnection(pFdSockets, szClassName),
    m_TcpServName ( TcpServName ),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    //printf("::()\n");
    AddFdSocket();
    m_Timer0.Attach(m_pFdSockets);
}
//---------------------------------------------------------------------------
void CMaaTcpStdoutConnection::OnTimer(int f)
{
    switch(f)
    {
    case 0:
        CloseByException("Close");
        break;
    }
}
//---------------------------------------------------------------------------
int CMaaTcpStdoutConnection::Notify_Read  ()
{
    //printf("r");
    char Buff [ 4096 + 1 ];
    int r = RecvData ( Buff, sizeof ( Buff ) - 1 );
    if  (IsClosed(r))
    {
        return CloseByException ( "closing connection" );
    }
    if  (r > 0)
    {
        fwrite(Buff, 1, r, stdout);
    }
    return eRead;
}
//---------------------------------------------------------------------------
int CMaaTcpStdoutConnection::Notify_Write ()
{
    //printf("w");
    /*
     if (GetFdMode() & eConnect)
     {
        return 0;
     }
     */
    return eDisableWrite;
}
//---------------------------------------------------------------------------
CMaaTcpStdoutConnection::~CMaaTcpStdoutConnection()
{
}
//---------------------------------------------------------------------------
const char CMaaTcpStdoutConnection::szClassName [] = "TcpStdoutConnection";
//---------------------------------------------------------------------------
CMaaTcpSocket * CMaaTcpStdoutServer::CreateNewConnection(CMaaFdSockets * pFdSockets)
{
    //printf("create\n");
    return TL_NEW CMaaTcpStdoutConnection(pFdSockets, m_ServName);
}
//---------------------------------------------------------------------------
CMaaTcpStdoutServer::CMaaTcpStdoutServer(CMaaFdSockets * pFdSockets, int Port)
:   CMaaUnivServer(pFdSockets, Port, "TcpStdoutServer")
{
    //printf("constructor\n");
}
//---------------------------------------------------------------------------

//------------



#ifdef DebugLevel
#undef DebugLevel
#endif
//int DebugLevel = 0;

#endif // __TOOLS_USE_WINSOCK || __TOOLS_USE_WINSOCK2



#ifndef __unix__

#else

//typedef long long __int64;

#include <signal.h>

static void OnMySIGPIPE(int x)
{
    signal(SIGPIPE, OnMySIGPIPE);

    //printf ( "OnMySIGPIPE\n" );
    //fflush ( stdout );

    return;
}

static void OnMySIGBUS(int x)
{
    signal(SIGBUS, OnMySIGBUS);

    //printf ( "OnMySIGBUS\n" );
    //fflush ( stdout );

    return;
}

#endif

int gSockThreads = 0;

CMaaSockThread::CMaaSockThread(CMaaSockThread * pParent)
:   m_pParent(pParent)
{
    gLock.LockM();
    gSockThreads++;
    static int _Id = 0;
    Id = ++_Id;
    gLock.UnLockM();
    m_Refs = 0;
    m_bExiting = false;
    m_pFdSockets = TL_NEW CMaaFdSockets(this);
    //m_pFdSockets->SetTimeOut(0, 10 * 1000);
#ifdef __unix__
    m_WakeupPair = NULL;
    if  (m_pFdSockets)
    {
        m_WakeupPair = TL_NEW CMaaWakeUpPair(m_pFdSockets);
    }
#else
    m_hWakeup = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
#ifdef __unix__
    if  (!m_pFdSockets || !m_WakeupPair)
#else
    if  (!m_pFdSockets)
#endif
    {
#ifdef __unix__
        delete m_WakeupPair;
#endif
        delete m_pFdSockets;
        gLock.LockM();
        gSockThreads--;
        gLock.UnLockM();
        throw 0;
    }
    gLock.LockM();
    if  (pParent)
    {
        pParent->m_ChildThreads.AddAtBack(this);
    }
    gLock.UnLockM();
}

int CMaaSockThread::AddRef()
{
    gLock.LockM();
    int r = ++m_Refs;
    gLock.UnLockM();
    return r;
}

int CMaaSockThread::DelRef()
{
    gLock.LockM();
    int r = --m_Refs;
    gLock.UnLockM();
    return r;
}

void CMaaSockThread::AddCommand(int Cmd, CMaaSockThread * Par1, void * Par2)
{
    AddCommand(TL_NEW SCmd(Cmd, Par1, Par2));
}

void CMaaSockThread::AddCommand(SCmd * c)
{
    if  (c)
    {
        gLock.LockM();
        m_List.AddAtBack(c);
        WakeUp();
        gLock.UnLockM();
    }
}
CMaaSockThread::~CMaaSockThread()
{
    //printf("--1--\n"); fflush(stdout);

    AddCommand(-1);

    //printf("--2--\n"); fflush(stdout);

    Join();

    //printf("--2.1--\n"); fflush(stdout);


    gLock.LockM();

    //printf("--2.2--\n"); fflush(stdout);

#ifdef __unix__

    delete m_WakeupPair;
    m_WakeupPair = NULL;
#else
    CloseHandle(m_hWakeup); m_hWakeup = NULL;
#endif

    gSockThreads--;


    //printf("--3--\n"); fflush(stdout);

    m_pFdSockets->DeleteAllSockets();

    //printf("--4--\n"); fflush(stdout);

    delete m_pFdSockets;

    //printf("--5--\n"); fflush(stdout);


    for (;;)
    {

        //printf("--6--\n"); fflush(stdout);

        SCmd * p = m_List.GetFromFront();
        if  (!p)
        {
            break;
        }
        delete p;
    }

    if  (m_pParent)
    {

        //printf("--7--\n"); fflush(stdout);

        m_pParent->m_ChildThreads.Release(this);
    }

    gLock.UnLockM();
}

void CMaaSockThread::WakeUpChildThreads()
{
    gLock.LockM();

    for (CMaaSockThread * t = m_ChildThreads.LookAtFront(); t; t = m_ChildThreads.Next(t))
    {
        t->WakeUp();
    }

    gLock.UnLockM();
}

int gggggggg = 0;

void CMaaSockThread::WakeUp()
{
#ifdef __unix__
    gLock.LockM();

    if  (m_WakeupPair)
    {
        m_WakeupPair->WakeUp();
    }

    gLock.UnLockM();
#else
    if  (m_pFdSockets && m_pFdSockets->m_temp_SelectFlag)
    {
        static int aa = 0;
        aa++;
        aa++;
        aa++;
        gggggggg = aa;
    }
    SetEvent(m_hWakeup);
#endif
}

unsigned CMaaSockThread::Run()
{
    SetThreadName((_dword)-1, "CMaaSockThread");
    //    printf("CMaaSockThread::Run(), m_pFdSockets=%p\n", m_pFdSockets);
    int Ret = 0;
#ifndef __unix__

#else
    signal(SIGPIPE, OnMySIGPIPE);
    signal(SIGBUS, OnMySIGBUS);
#endif

    try
    {
        Process();
    }
    catch(const char * Msg)
    {
        //printf("catch(\"%s\");\n", Msg);
        XTOOWsaErr  WsaErr;
        XTOOErrno   Errno;
        XTOOSockErr ErrMsg(Msg, NULL);
        Ret = 1;
    }
    catch(XTOOError &/*x*/)
    {
        //printf("catch(XTOOError &);\n", x.GetMsg());
        XTOOErrMsg  ErrMsg ( "\n" );
        Ret = 2;
    }
    catch(eExitThread &)
    {
        //printf("catch(eExitThread &);\n");
        //printf("catch(eExitThread &)\r\n");
        //fflush(stdout);
        Ret = 4;
    }
    catch(...)
    {
        //printf("catch(...)\r\n");
        //fflush(stdout);
        try
        {
            XTOOWsaErr  WsaErr;
            XTOOErrno   Errno;
            XTOOErrMsg  ErrMsg ( "\nUnknown error." );
        }
        catch(...)
        {
            char * p = strerror ( errno );
            printf("\nUnknown error. Errno = %d %s\n", errno, p ? p : "");
        }
        Ret = 3;
    }
    //printf("exiting...\r\n");
    //fflush(stdout);
    //    printf("CMaaSockThread::Run() - if (m_pParent) m_pParent->AddCommand(1, this);\n");

    /*--------------------------------*/
    //m_pFdSockets->DeleteAllSockets();
    /*--------------------------------*/
    if  (m_pParent)
    {
        m_pParent->AddCommand(1, this);
    }
    //printf("exit threadwith code=%d\r\n", Ret);
    //fflush(stdout);
    //    printf("CMaaSockThread::Run() - return\n");
    return (unsigned)Ret;
}

CMaaSockThread * CMaaSockThread::CreateNewThread()
{
    return TL_NEW CMaaSockThread(this);
}

#ifdef __unix__
bool CMaaSocketPair(CMaaSocket &s1, CMaaSocket &s2)
{
    int d[2];
    d[0] = d[1] = -1;
    CMaaTcpSocket * p1 = dynamic_cast<CMaaTcpSocket *>(&s1);
    CMaaTcpSocket * p2 = dynamic_cast<CMaaTcpSocket *>(&s2);
    //printf("p1 = %p, p2 = %p\r\n", p1, p2);
    //fflush(stdout);
    int r;
    if  (p1 && p2)
    {
        r = socketpair(AF_UNIX, SOCK_STREAM, 0, d);
    }
    else
    {
        r = socketpair(AF_UNIX, SOCK_DGRAM, 0, d);
    }
    if  (r)
    {
        //      printf("error %d\r\n\n\n", errno);
        return false;
    }
    //printf("d[0] = %d, d[1] = %d\r\n", d[0], d[1]);
    //fflush(stdout);
    s1.SetSocket(d[0]);
    s2.SetSocket(d[1]);
    return true;
}
#endif


CMaaSocketTimer::CMaaSocketTimer(int PeriodUs)
{
    m_bActive = false;
    m_PeriodUs = PeriodUs;
    m_pFdSockets = NULL;

#ifdef __unix__
    memset(&m_Next, 0, sizeof(m_Next));
#else
    m_Next = 0;
#endif
#ifdef __SOCK_NEW_TIMERS
    m_Handle = NULL;
#endif
}
CMaaSocketTimer::~CMaaSocketTimer()
{
    Detach();
}

_qword CMaaSocketTimer::GetPeriod()
{
    _qword r;
    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.Lock();
        r = m_PeriodUs;
        gpSockStartup->m_TimerMutex.UnLock();
    }
    else
    {
        r = m_PeriodUs;
    }
    return r;
}
bool CMaaSocketTimer::IsStarted()
{
    return m_bActive;
}

void CMaaSocketTimer::Start(_qword PeriodUs, int iWakeUp)
{
    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.LockM();
    }
    bool bSkipWakeUp = m_bActive && (PeriodUs == 0 || (PeriodUs >= m_PeriodUs && PeriodUs > 0));
    //m_bActive = false;
    if  (PeriodUs)
    {
        m_PeriodUs = PeriodUs;
    }
#ifdef __unix__
    if  (m_PeriodUs <= 0)
#else
    if  (m_PeriodUs <= 0) //1000)
#endif
    {
        m_bActive = false;
        if  (gpSockStartup)
        {
            gpSockStartup->m_TimerMutex.UnLockM();
        }
        return;
    }

#ifndef __SOCK_NEW_TIMERS

#ifdef __unix__
    gettimeofday(&m_Next, NULL);
    _qword q = m_PeriodUs + m_Next.tv_usec;
    m_Next.tv_usec = q % 1000000;
    m_Next.tv_sec += q / 1000000;
    /*
     if   (m_Next.tv_usec > 1000000)
     {
          m_Next.tv_sec += m_Next.tv_usec / 1000000;
          m_Next.tv_usec %= 1000000;
     }
     */
#else
    m_Next = GetTickCount() + (_dword)(m_PeriodUs / 1000);
    if  (m_PeriodUs / 1000 == 0)
    {
        //m_Next++;
    }
#endif

#else

    if  (gpSockStartup)
    {
        m_Next = gpSockStartup->GetTime() + m_PeriodUs;
    }
    else
    {
        m_Next = -1;
    }
    if  (m_pFdSockets)
    {
        if  (gpSockStartup)
        {
            gpSockStartup->m_TimerMutex.LockM();
            if  (m_Handle)
            {
                m_pFdSockets->m_TimerHeap.Del(m_Handle);  m_Handle = NULL;
            }
            if  (m_Next != -1)
            {
                m_Handle = m_pFdSockets->m_TimerHeap.Add(m_Next, this);
            }
            gpSockStartup->m_TimerMutex.UnLockM();
        }
    }

#endif

    m_bActive = true;

    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.UnLockM();
    }

    if  (iWakeUp > 1 || (iWakeUp == 1 && !bSkipWakeUp))
    {
        if  (m_pFdSockets && m_pFdSockets->m_pThread)
        {
            m_pFdSockets->m_pThread->WakeUp();
        }
    }
}

void CMaaSocketTimer::Stop()
{
    m_bActive = false;
}

void CMaaSocketTimer::Attach(CMaaFdSockets * pFdSockets)
{
    Detach();
    m_pFdSockets = pFdSockets;
    m_pFdSockets->m_Timers.AddAtBack(this);
#ifdef __SOCK_NEW_TIMERS
    if  (m_bActive)
    {
        if  (gpSockStartup)
        {
            _qword t0 = gpSockStartup->GetTime();
            if  (m_Next == -1 || m_Next < t0)
            {
                m_Next = t0 + m_PeriodUs;
            }
            if  (m_pFdSockets)
            {
                gpSockStartup->m_TimerMutex.LockM();
                /*
                    if (m_Handle)
                    {
                         m_pFdSockets->m_TimerHeap.Del(m_Handle);  m_Handle = NULL;
                    }
                    */
                if  (m_Next != -1)
                {
                    m_Handle = m_pFdSockets->m_TimerHeap.Add(m_Next, this);
                }
                gpSockStartup->m_TimerMutex.UnLockM();
            }
        }
        else
        {
            m_Next = -1;
        }
    }
#endif
}

void CMaaSocketTimer::Detach()
{
    //printf("void CMaaSocketTimer::Detach()\r\n");
    //fflush(stdout);
    if  (m_pFdSockets)
    {
        //printf("void CMaaSocketTimer::Detach() - Release\r\n");
        //fflush(stdout);
        m_pFdSockets->m_Timers.Release(this);
        //printf("void CMaaSocketTimer::Detach() - Release - ok\r\n");
        //fflush(stdout);
#ifdef __SOCK_NEW_TIMERS
        if  (gpSockStartup)
        {
            gpSockStartup->m_TimerMutex.LockM();
            if  (m_Handle)
            {
                m_pFdSockets->m_TimerHeap.Del(m_Handle);  m_Handle = NULL;
            }
            gpSockStartup->m_TimerMutex.UnLockM();
        }
#endif
        m_pFdSockets = NULL;
    }
    Stop();
}

#ifndef __SOCK_NEW_TIMERS

#ifdef __unix__
void CMaaSocketTimer::PreTimer(timeval * t)
{
    if  (!m_bActive)
    {
        return;
    }
    timeval tv;
    if  (!t)
    {
        gettimeofday(t = &tv, NULL);
    }
    if  ((t->tv_sec > m_Next.tv_sec) || (t->tv_sec == m_Next.tv_sec && t->tv_usec >= m_Next.tv_usec))
    {
        m_Next.tv_usec += m_PeriodUs;
        if  (m_Next.tv_usec > 1000000)
        {
            m_Next.tv_sec += m_Next.tv_usec / 1000000;
            m_Next.tv_usec %= 1000000;
        }
        if  ((t->tv_sec > m_Next.tv_sec) || (t->tv_sec == m_Next.tv_sec && t->tv_usec >= m_Next.tv_usec))
        {
            m_Next = *t; // Some timer ticks will be skipped
        }
        OnTimer();
    }
}

void CMaaSocketTimer::GetWaitForTime(int *us, timeval *t)
{
    if  (!m_bActive)
    {
        return;
    }
    timeval tv;
    if  (!t)
    {
        gettimeofday(t = &tv, NULL);
    }
    _qword q = (_qword)(m_Next.tv_sec - t->tv_sec) * 1000000 + (m_Next.tv_usec - t->tv_usec);
    //_qword max_dword = (((1 << 30) - 1) << 1) + 1;
    _qword max_dword = ((1 << 30) - 1);
    int u = (int)(q < max_dword ? q : max_dword);
    if  (u < *us)
    {
        if  (u < 0)
        {
            u = 0;
        }
        *us = u;
    }
}
#else
void CMaaSocketTimer::PreTimer(DWORD * t)
{
    if  (!m_bActive)
    {
        return;
    }
    DWORD tv;
    if  (!t)
    {
        t = &tv;
        *t = GetTickCount();
    }
    if  ((int)(*t - m_Next) >= 0)
    {
        m_Next += (_dword)(m_PeriodUs / 1000);
        if  ((int)(*t - m_Next) >= 0)
        {
            m_Next = *t; // Some timer ticks will be skipped
        }
        OnTimer();
    }
}

void CMaaSocketTimer::GetWaitForTime(int *us, DWORD *t)
{
    if  (!m_bActive)
    {
        return;
    }
    DWORD tv;
    if  (!t)
    {
        t = &tv;
        tv = GetTickCount();
    }
    _qword q = (_qword)(m_Next - *t) * 1000;
    //_qword max_dword = (((1 << 30) - 1) << 1) + 1;
    _qword max_dword = ((1 << 30) - 1);
    int u = (int)(q < max_dword ? q : max_dword);
    if  (u < *us)
    {
        if  (u < 0)
        {
            u = 0;
        }
        *us = u;
    }
}
#endif

#else

void CMaaSocketTimer::PreTimer(_qword t)
{
    if  (!m_bActive)
    {
        return;
    }
    /*
     _qword tv;
     if   (!t)
     {
          t = &tv;
          *t = gpSockStartup ? gpSockStartup->GetTime() : 0;
     }
     */
    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.LockM();
    }
    if  ((_qword)(t - m_Next) >= 0)
    {
        m_Next += m_PeriodUs;
        if  ((_qword)(t - m_Next) >= 0)
        {
            m_Next = t + 1; // Some timer ticks will be skipped
        }
        if  (m_pFdSockets && gpSockStartup)
        {
            gpSockStartup->m_TimerMutex.LockM();
            if  (m_Handle)
            {
                m_pFdSockets->m_TimerHeap.Del(m_Handle);  m_Handle = NULL;
            }
            if  (m_Next != -1)
            {
                m_Handle = m_pFdSockets->m_TimerHeap.Add(m_Next, this);
            }
            gpSockStartup->m_TimerMutex.UnLockM();
        }
        if  (gpSockStartup)
        {
            gpSockStartup->m_TimerMutex.UnLockM();
        }
        OnTimer();
        return;
    }
    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.UnLockM();
    }
}

void CMaaSocketTimer::GetWaitForTime(int *us, _qword t)
{
    if  (!m_bActive)
    {
        return;
    }
    /*
    _qword tv;
    if   (!t)
    {
        t = &tv;
        tv = gpSockStartup ? gpSockStartup->GetTime() : 0;
    }
    */
    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.LockM();
    }
    _qword q = (_qword)(m_Next - t);
    //_qword max_dword = (((1 << 30) - 1) << 1) + 1;
    _qword max_dword = ((1 << 30) - 1);
    int u = (int)(q < max_dword ? q : max_dword);
    if  (u < *us)
    {
        if  (u < 0)
        {
            u = 0;
        }
        *us = u;
    }
    if  (gpSockStartup)
    {
        gpSockStartup->m_TimerMutex.UnLockM();
    }
}
#endif

void CMaaSocketTimer::OnTimer()
{
}

//---------------------------------------------------------------------------

void CMaaFdSockets::Attach(CMaaSocketTimer * t)
{
    t->Attach(this);
}

void CMaaFdSockets::Detach(CMaaSocketTimer * t)
{
    t->Detach();
}

void CMaaFdSockets::PreTimer()
{
#ifndef __SOCK_NEW_TIMERS
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, NULL);
#else // __unix__
    DWORD tv = GetTickCount();
#endif
    CMaaSocketTimer * t, * n;
    for (t = m_Timers.LookAtFront(); t; t = m_Timers.Next(t))
    {
        t->m_TmpFlg = 0;
    }
    bool bRepeat = true;
    while(bRepeat)
    {
        bRepeat = false;
        for (t = m_Timers.LookAtFront(); t; t = n)
        {
            n = m_Timers.Next(t);
            if  (t->m_TmpFlg == 0)
            {
                t->m_TmpFlg++;
                try
                {
                    t->PreTimer(&tv);
                }
                catch(...)
                {
                    n = NULL;
                    bRepeat = true;
                }
            }
        }
    }
#else
    if  (!gpSockStartup)
    {
        return;
    }
    _qword tv = gpSockStartup->GetTime() + 1;
    CMaaSocketTimer * t;
    gpSockStartup->m_TimerMutex.LockM();
    while(1)
    {
        _qword tx;
        if  (!m_TimerHeap.Look(&tx, &t) || tx > tv)
        {
            break;
        }
        m_TimerHeap.Del();
        t->ResetHandle();
        if  (t->IsStarted())
        {
            gpSockStartup->m_TimerMutex.UnLockM();
            try
            {
                t->PreTimer(tv);
            }
            catch(...)
            {
            }
            gpSockStartup->m_TimerMutex.LockM();
        }
    }
    gpSockStartup->m_TimerMutex.UnLockM();
#endif
}

void CMaaFdSockets::GetWaitForTime(int * us)
{
#ifndef __SOCK_NEW_TIMERS
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, NULL);
#else
    DWORD tv = GetTickCount();
#endif
    for (CMaaSocketTimer * t = m_Timers.LookAtFront(); t; t = m_Timers.Next(t))
    {
        t->GetWaitForTime(us, &tv);
    }
#else
    if  (!gpSockStartup)
    {
        return;
    }
    _qword tv = gpSockStartup->GetTime();
    CMaaSocketTimer * t;
    gpSockStartup->m_TimerMutex.LockM();
    while(1)
    {
        _qword tx;
        if  (!m_TimerHeap.Look(&tx, &t))
        {
            t = NULL;
            break;
        }
        if  (!t->IsStarted())
        {
            m_TimerHeap.Del();
            t->ResetHandle();
        }
        else
        {
            break;
        }
    }
    gpSockStartup->m_TimerMutex.UnLockM();
    if  (t)
    {
        int us2 = *us;
#ifdef _WIN32
        if  ((us2 -= 998) < 0) // 1 ms round next
        {
            us2 = 0;
        }
#endif
        t->GetWaitForTime(&us2, tv);
#ifdef _WIN32
        us2 += 998; // 1 ms (-1 us) round next
#endif
        if  (us2 < *us)
        {
            *us = us2;
        }
    }
#endif
}

void CMaaSockThread::Process()
{
    LOG1_(99);
    while(Process1())
    {
        LOG1_(100);
    }
}

void CMaaSockThread::OnExitCommand(SCmd *)
{
    m_bExiting = true;
    m_pFdSockets->DeleteAllSockets();
}

bool CMaaSockThread::Process1()
{
    for (;;)
    {
        for (;;)
        {
            LOG1_(101);
            gLock.LockM();
            SCmd * c = m_List.GetFromFront();
            gLock.UnLockM();
            //printf("Got command: %p\r\n", c); fflush(stdout);

            if  (!c)
            {
                break;
            }
            //printf("Got command: %d, %p, %p\r\n", c->Cmd, c->m_Par1, c->m_Par2); fflush(stdout);

            if  (c && c->Cmd == 1)
            {
                //printf("c->Cmd == 1 - Join()\r\n");fflush(stdout);//P
                c->m_Par1->Join();
                //printf("c->Cmd == 1 - Join() - ok\r\n");fflush(stdout);//P
                delete c->m_Par1;
                //printf("c->Cmd == 1 - delete - ok\r\n");fflush(stdout);//P
                delete c;
                c = NULL;
            }
            if  (c && c->Cmd == -1)
            {
                //printf("OnExitCommand();\n");
                OnExitCommand(c);
                delete c;
                c = NULL;
            }
            delete c;
        }

        LOG1_(102);
        if  (!m_pParent)
        {
            //printf("000: point\r\n");fflush(stdout);//P
        }
        else
        {
            //printf("111: point\r\n");fflush(stdout);//P
        }

        if  (m_ChildThreads.LookAtFront() && m_pFdSockets->GetSockCount() <= 0)
        {
            //Log("if (m_ChildThreads.LookAtFront() && m_pFdSockets->GetSockCount() <= 0) { }");
            /*
          FILE * f = fopen("c:\\ftp2.txt", "a+b");
          if   (f)
          {
               fprintf(f, "if (m_ChildThreads.LookAtFront() && m_pFdSockets->GetSockCount() <= 0) { }\n");
               fclose(f);
          }
          */
        }
        if  (m_Refs <= 0 && m_ChildThreads.LookAtFront() == NULL)
        {
            if  (m_bExiting)
            {
                //printf("Point n2\n");
                break;
            }
            //printf("Point n2.5\n");
#ifdef __unix__
            if  (m_pFdSockets->GetSockCount() <= 1)
#else
            if  (m_pFdSockets->GetSockCount() <= 0)
#endif
            {
                //printf("Point n3\n");
                //if (m_pParent)
                {
                    break;
                }
            }
            //if (m_pParent || gSockThreads <= 1)
            {
                CheckPoint(0);
            }
            /*
            if (bExitSockThread)
            {
                if (m_pParent || gSockThreads <= 1)
                {
                    break;
                }
            }
            */
        }
        //WDTimerFlags[2] = 1;
        LOG1_(105);
        if  (m_pFdSockets->Select() < 0)
        {
            //printf("Point n4\n");
            CMaaSocket::StaticError("Select()");
            //Console.Error("select()");
            break;
        }
        LOG1_(106);
        m_pFdSockets->PreTimer();
        LOG1_(107);
        return true;
    }
    //printf("Point n5\n");
    LOG1_(108);
    return false;
}
//---------------------------------------------------------------------------
#ifdef __unix__
//bool CMaaSocketPair(CMaaSocket &s1, CMaaSocket &s2);
CMaaWakeUpPair::CMaaWakeUpPair(CMaaFdSockets *pFdSockets)
:   CMaaUdpSocket(pFdSockets),
    m_s2(pFdSockets)
{
    m_Bytes = 0;
    RemoveFdSocket();
    m_s2.RemoveFdSocket();
    CMaaSocketPair(*this, m_s2);
    //FILE * f = fopen("TEST.txt", "a+b");
    try
    {
        SetNonBlockingMode();
        //      fprintf(f, "CMaaWakeUpPair::CMaaWakeUpPair(): SetNonBlockingMode() - ok\n");
    }
    catch(XTOOSockErr e)
    {
        //      fprintf(f, "CMaaWakeUpPair::CMaaWakeUpPair(): catch %s\n", (const char *)e.GetMsg());
        e.Eat();
    }
    catch(...)
    {
        //      fprintf(f, "CMaaWakeUpPair::CMaaWakeUpPair(): catch(...)\n");
    }
    //fclose(f);
    AddFdSocket();
    ChangeFdMode(eDisableAll);
    ChangeFdMode(eRead);
}

CMaaWakeUpPair::~CMaaWakeUpPair()
{
#ifdef __unix__
    gLock.LockM();
    m_pFdSockets->m_pThread->m_WakeupPair = NULL;
    gLock.UnLockM();
#endif
    //printf("CMaaWakeUpPair::~CMaaWakeUpPair()\r\n");
    //fflush(stdout);
    //abort();
}

/*
void CMaaFdSocket::ResetEvent(int ev)
{
    if  (m_pFdSockets)
    {
        m_pFdSockets->ResetEvent(ev);
    }
}
*/

static CMaaMutex sWakeUpMutex;
int CMaaWakeUpPair::Notify_Read()
{
    char Buffer[10*1024];
    sWakeUpMutex.Lock();
    bool bLocked = true;
    try
    {
        if  (m_Bytes > 0 || 1)
        {
            //gLock.LockM();
            //          socklen_t len = sizeof(m_tmp);
            //          sockadd_in m_tmp;
            //          memset(&m_tmp, 0, sizeof(m_tmp));
            //          int n = recvfrom(m_Socket, Buffer, sizeof(BufferSize), 0, (struct sockaddr *)&m_tmp, &len);

            int n = recv(m_Socket, Buffer, sizeof(Buffer), 0);
            //__utf8_printf("CMaaWakeUpPair::Notify_Read(): m_Bytes = %d, n = %d\n", m_Bytes, n);
            if  (n > 0)
            {
                if  ((m_Bytes -= n) < 0)
                {
                    m_Bytes = 0;
                }
            }
            //RecvData(Buffer, sizeof(Buffer));
            //gLock.UnLockM();
        }
        sWakeUpMutex.UnLock();
        bLocked = false;
        ChangeFdMode(eDisableEventRead);
    }
    catch(XTOOSockErr &e)
    {
        if  (bLocked)
        {
            sWakeUpMutex.UnLock();
        }
        //gLock.UnLockM();
        ChangeFdMode(eDisableEventRead);
        //Trace("CMaaWakeUpPair::Notify_Read(): catch(XTOOSockErr &e), errno=%d", errno);
        gLock.LockM();
        delete this;
        gLock.UnLockM();
        e.Eat();
        throw;
    }
    catch(...)
    {
        if  (bLocked)
        {
            sWakeUpMutex.UnLock();
        }
        //gLock.UnLockM();
        ChangeFdMode(eDisableEventRead);
        //Trace("CMaaWakeUpPair::Notify_Read(): catch(...)", 0);
        throw;
    }
    return 0;
}

int CMaaWakeUpPair::Notify_Write()
{
    return eDisableWrite;
}

//int Notify_Error ();

const char * CMaaWakeUpPair::GetConnectionName () {return "CMaaWakeUpPair"; }

void CMaaWakeUpPair::WakeUp()
{
    //printf("CMaaWakeUpPair::WakeUp()\n");
    char c = '1';
    sWakeUpMutex.Lock();
    if  (m_Bytes == 0)
    {
        m_Bytes++;
        sWakeUpMutex.UnLock();
        try
        {
            send(m_s2.m_Socket, &c, 1, 0);
            //m_s2.SendData(&c, 1);
        }
        catch(XTOOSockErr &e)
        {
            //printf("catch(XTOOSockErr &e) in CMaaWakeUpPair::WakeUp()\r\n");fflush(stdout);
            e.Eat();
            // ignoring error
        }
        catch(...)
        {
            //printf("catch(...) in CMaaWakeUpPair::WakeUp()\r\n"); fflush(stdout);
        }
    }
    else
    {
        sWakeUpMutex.UnLock();
    }
}
#endif

CMaaTcpSocket::CCloseTimer::CCloseTimer(int PeriodUs)
:   CMaaSocketTimer(PeriodUs),
    m_pSocket(NULL)
{
}

CMaaTcpSocket::CCloseTimer::~CCloseTimer()
{
    //Stop();
    //Detach();
}

void CMaaTcpSocket::CCloseTimer::OnTimer()
{
    // only for windows implementation
    if  (m_pSocket)
    {
        if  (m_pSocket->GetFdMode() & CMaaEnum::eHold)
        {
            return;
        }
        char c = 0;
        int l = recv(m_pSocket->GetSocket(), &c, 1, MSG_PEEK);

        if  (l == 0)
        {
            Stop();
#ifdef _WIN32
            WSASetLastError(CMaa_CONN_CLOSED_ERROR);
#endif
            m_pSocket->ChangeFdMode(m_pSocket->Notify_Error());
            //ev.lNetworkEvents |= FD_READ;
            //ev.iErrorCode[FD_READ_BIT] = CMaa_CONN_CLOSED_ERROR;//WSAENOTCONN;
        }
        else if (l < 0)
        {
#ifdef _WIN32
            DWORD x = WSAGetLastError();
            if  (x != WSAEWOULDBLOCK)
#endif
            {
                Stop();
                //ev.lNetworkEvents |= FD_READ;
                //ev.iErrorCode[FD_READ_BIT] = x;
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Error());
            }
        }
        else
        {
            if  (m_pSocket->GetFdMode() & eRead)
            {
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Read());
            }
        }
    }
    else
    {
        Detach();
    }
}

void CMaaTcpSocket::CCloseTimer::OnClose(CMaaTcpSocket * pSocket, bool bShortPeriod)
{
    m_pSocket = pSocket;
    Attach(pSocket->m_pFdSockets);
    Start(bShortPeriod ? 2000 : 200000);//pSocket->GetFdMode());
}

void CMaaFdSockets::WakeUp()
{
    if  (m_pThread)
    {
        m_pThread->WakeUp();
    }
}

void CMaaFdSocket::WakeUp()
{
    if  (m_pFdSockets)
    {
        m_pFdSockets->WakeUp();
    }
}

//---------------------------------------------------------------------------------
// Shapers
//---------------------------------------------------------------------------------
#ifdef __SHAPERS
//--------------------------------------------------------------------------
int CMaaTcpLLShaper::GetLenReserved(int Len)
{
    int Len0 = Len;
    if  (m_MaxSpeedLimit >= 0 && Len > m_MaxSpeedLimit - m_V)
    {
        Len = m_MaxSpeedLimit - m_V;
    }
    int V0 = 1;
    if  (V0 > m_Vsrd)
    {
        if  (Len > V0 - m_V)
        {
            Len = V0 - m_V;
        }
    }
    else
    {
        if  (Len > m_Vsrd - m_V)
        {
            Len = m_Vsrd - m_V;
        }
    }
    int Len2 = Len;
    if  (m_Shaper)
    {
        Len2 = m_Shaper->GetLenReserved(Len);
    }

    // test
    // not strict shaper limits support (gStrictTcpShaperLimits <= 0, m_Vsr >= 0)
    if  (Len2 < m_Vsr - m_V)
    {
        Len2 = m_Vsr - m_V;
    }
    /*
     if   (Len2 > m_Vsr - m_V && m_V == 0)
     {
          Len2 = m_Vsr - m_V;
     }
     */
    /*
     if   (Len2 < m_Vsrd - m_V)
     {
          Len2 = m_Vsrd - m_V;
     }
     */
    if  (Len < Len2)
    {
        Len2 = Len;
    }
    Len2 = Len2 >= 0 ? Len2 : 0;
    if  (Len0 > 0)
    {
        if  (Len2 > 0)
        {
#ifdef _WIN32
            SetEvent(m_hEv);
#else
            m_hEv = true;
            /*
               if (m_pSocket)
               {
                   m_pSocket->WakeUp();
               }
               */
#endif
        }
        else
        {
#ifdef _WIN32
            ResetEvent(m_hEv);
#else
            m_hEv = false;
#endif
        }
    }
    return Len2;
}
//--------------------------------------------------------------------------
void CMaaTcpLLShaper::SubLenReserved(int Len)
{
    m_V += Len;
    if  (m_Shaper)
    {
        m_Shaper->SubLenReserved(Len);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpLLShaper::OnTimer(int i)
{
    m_VTempSum += m_V;
    m_V = 0;
    if  (i)
    {
        m_VAverageLast = m_VTempSum;
        m_VTempSum = 0;
        if  (m_Shaper && m_pSocket && m_Shaper->m_Conditions.LookAtFront() && m_pSocket->m_ShaperConditionLastCheckTime != i)
        {
            m_pSocket->m_ShaperConditionLastCheckTime = i;
            m_pSocket->CheckConditions(m_Shaper);//, m_Shaper->m_Conditions);
        }
    }
}

void CMaaTcpLLShaper::SetConnectionLimits(int MaxSpeed, int bHardLimit)
{
    if  (m_bHardLimit && bHardLimit < 0)
    {
        return;// false;
    }
    if  (MaxSpeed == -2)
    {
        return;
    }
    if  (MaxSpeed < 0)
    {
        MaxSpeed = -1;
    }
    else if ((MaxSpeed /= _SHAPER_TICKS_PER_SECOND) < 1)
    {
        MaxSpeed = 1;
    }
    m_MaxSpeedLimit = MaxSpeed;
    if  (bHardLimit >= 0)
    {
        m_bHardLimit = bHardLimit > 0;
    }
}
//--------------------------------------------------------------------------
CMaaTcpShaper::CMaaTcpShaper(int Type, CMaaString Name, double W, int MaxSpeed, int UserId)
{
    Init(Type, Name, W, MaxSpeed, UserId);
}
//--------------------------------------------------------------------------
CMaaTcpShaper::CMaaTcpShaper(CMaaString GeneralShaperName, double W, int MaxSpeed) // Type = eGeneral
{
    Init(eGeneral, GeneralShaperName, W, MaxSpeed);
}
//--------------------------------------------------------------------------
CMaaTcpShaper::CMaaTcpShaper(CMaaString UserName, int UserId, double W, int MaxSpeed) // Type = eUserSubshaper
{
    Init(eUserSubshaper, UserName, W, MaxSpeed, UserId);
}
//--------------------------------------------------------------------------
/*
CMaaTcpShaper::CMaaTcpShaper(double W, int MaxSpeed) //  // Type = AutoUserSubShaper
{
     Init(AutoUserSubShaper, CMaaString(), W, MasSpeed, -2);
}
*/
//--------------------------------------------------------------------------
CMaaTcpShaper::~CMaaTcpShaper()
{
    if  (m_Shaper)
    {
        m_Shaper->Del(*this);
    }
    DeleteConnectionInTimeCharacteristics();
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SetConnectionLimits(int MaxSpeed, bool bRecursive)
{
    if  (MaxSpeed == -2)
    {
        return;
    }
    if  (m_ChildsLL.LookAtFront())
    {
        int max_speed_limit;
        if  (MaxSpeed < 0)
        {
            max_speed_limit = -1;
        }
        else
        {
            if  ((max_speed_limit = (MaxSpeed /= _SHAPER_TICKS_PER_SECOND)) < 1)
            {
                max_speed_limit = 1;
            }
        }
        for (CMaaTcpLLShaper * p = m_ChildsLL.LookAtFront(); p; p = m_ChildsLL.Next(p))
        {
            if  (!p->m_bHardLimit)
            {
                p->m_MaxSpeedLimit = max_speed_limit;
            }
        }
    }
    if  (bRecursive)
    {
        for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            l->p->SetConnectionLimits(MaxSpeed, true);
        }
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SetConditions(CMaaDListAD<Condition> &List, bool bAllowToMoveAllListData)
{
    m_Conditions.Clear();
    if  (bAllowToMoveAllListData)
    {
        m_Conditions.MoveThatToTheFront(List);
    }
    else
    {
        CopyList(m_Conditions, List);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::CopyList(CMaaDList<Condition> &dst, CMaaDList<Condition> &src)
{
    for (Condition * p = src.LookAtFront(); p; p = src.Next(p))
    {
        Condition * n = p->Copy();
        if  (!n)
        {
            break;
        }
        dst.AddAtBack(n);
    }
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetNotEmptyChildrenShapersCount()
{
    return m_ConnectionsCount[2];
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetConnectionsCount(bool bRecursive)
{
    return m_ConnectionsCount[bRecursive ? 1 : 0];
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::Init(int Type, CMaaString Name, double W, int MaxSpeed, int UserId)
{
    memset(m_pConnectionInTimeCharacteristics, 0, sizeof(m_pConnectionInTimeCharacteristics));
    memset(m_ConnectionsCount, 0, sizeof(m_ConnectionsCount));

    m_L.p = this;

    m_Type = Type;
    m_Name = Name;
    if  (MaxSpeed < 0)
    {
        MaxSpeed = 0x7fffffff;
    }
    m_MaxSpeed = MaxSpeed / _SHAPER_TICKS_PER_SECOND;
    if  (m_MaxSpeed < 1)
    {
        m_MaxSpeed = 1;
    }
    m_UserId = UserId;
    //m_MaxSpeed = 50000 / 5;
    //m_MaxSpeed = 100000 / 5;
    //m_MaxSpeed = 5 / 5;
    m_Bank = m_MaxSpeed;
    m_Shaper = NULL;
    m_Weight = W;
    m_V0 = m_Vsr = m_Bank;
    m_V = m_VTempSum = m_VAverageLast = 0;
    m_fVsr = (double)m_Vsr;
    if  (Type == eGeneral)
    {
        CreateConnectionInTimeCharacteristics();
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::DeleteConnectionInTimeCharacteristics()
{
    for (int j = 0; j < 4; j++)
    {
        delete m_pConnectionInTimeCharacteristics[j];
        m_pConnectionInTimeCharacteristics[j] = NULL;
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::CreateConnectionInTimeCharacteristics(int s, int m, int h, int d)
{
    DeleteConnectionInTimeCharacteristics();
    //int i;
    //for  (i = 0; i < 2; i++)
    {
        _qword InitialValue = 0;//!(i & 1) ? m_BytesSend : m_BytesRecv;
        m_pConnectionInTimeCharacteristics[0] = TL_NEW CMaaConnectionInTimeCharacteristics(s, InitialValue, 1);
        m_pConnectionInTimeCharacteristics[1] = TL_NEW CMaaConnectionInTimeCharacteristics(m, InitialValue, 60);
        m_pConnectionInTimeCharacteristics[2] = TL_NEW CMaaConnectionInTimeCharacteristics(h, InitialValue, 3600);
        m_pConnectionInTimeCharacteristics[3] = TL_NEW CMaaConnectionInTimeCharacteristics(d, InitialValue, 24 * 3600);
    }
    int null_ptrs = 0;
    //for  (i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if  (!m_pConnectionInTimeCharacteristics[j])
            {
                null_ptrs++;
            }
        }
    }
    if  (null_ptrs)
    {
        DeleteConnectionInTimeCharacteristics();
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::ResetConnectionInTimeCharacteristics()
{
    if  (m_pConnectionInTimeCharacteristics[0])
    {
        //for  (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                _qword value = 0;//!(i & 1) ? m_BytesSend : m_BytesRecv;
                m_pConnectionInTimeCharacteristics[j]->Reset(value, true);
            }
        }
    }
}
//--------------------------------------------------------------------------
CMaaString CMaaTcpShaper::GetDispName()
{
    switch(m_Type)
    {
    case eGeneral:
        return m_Name;
    case eUserSubshaper:
        return CMaaString::sFormat("u:%s(%d)", (const char *)m_Name, m_UserId);
    case eAutoUserSubshaper:
        return CMaaString::sFormat("a:%s(%d)", (const char *)m_Name, m_UserId);
    }
    return CMaaString();
}
//--------------------------------------------------------------------------
CMaaString CMaaTcpShaper::GetShaperNameRawRecursiveLibName()
{
    CMaaString Name;
    {
        switch(m_Type)
        {
        case eGeneral:
            Name = m_Name;
            break;
        case eUserSubshaper:
            Name.Format("user(%d)", m_UserId);
            break;
        case eAutoUserSubshaper:
            //Name = "AutoUserSubshaper";
            Name.Format("auto(%d)", m_UserId);
            break;
        }
        if  (m_Shaper)
        {
            CMaaString ParentShaperName = m_Shaper->GetShaperNameRawRecursiveLibName();
            if  (ParentShaperName.Length() > 0)
            {
                Name = ParentShaperName + "." + Name;
            }
        }
    }
    return Name;
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetLenReserved(int Len)
{
    int Len2 = GetLenReserved2(Len);
    /*
     if   (Len2 < m_Vsr - m_V)
     {
          Len2 = m_Vsr - m_V;
     }
     if   (Len < Len2)
     {
          Len2 = Len;
     }
     */
    return Len2;
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetLenReserved2(int Len)
{
    if  (m_Bank < Len)
    {
        Len = m_Bank;
    }
    // test
    if  (Len < 0)
    {
        Len = 0;
    }
    if  (m_Shaper)
    {
        Len = m_Shaper->GetLenReserved(Len);
    }
    // test
    if  (Len < 0)
    {
        Len = 0;
    }
    return Len;
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SubLenReserved(int Len)
{
    m_Bank -= Len;
    m_V += Len;
    if  (m_pConnectionInTimeCharacteristics[0])
    {
        _qword value = Len;
        m_pConnectionInTimeCharacteristics[0]->Accumulate(value);
        //if   (m_pConnectionInTimeCharacteristics[0]->GetCurPos() % 60 == 0)
        {
            m_pConnectionInTimeCharacteristics[1]->Accumulate(value);
            //if   (m_pConnectionInTimeCharacteristics[1]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[2]->Accumulate(value);
                //if   (m_pConnectionInTimeCharacteristics[2]->GetCurPos() % 24 == 0)
                {
                    m_pConnectionInTimeCharacteristics[3]->Accumulate(value);
                }
            }
        }
    }
    if  (m_Shaper)
    {
        m_Shaper->SubLenReserved(Len);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::OnTimerUpdate(/*CMaaShaperThread * pThread*/)
{
    if  (m_Shaper && !m_Shaper->m_Shaper)
    {
        Update(/*pThread*/);
    }
}
//--------------------------------------------------------------------------
const double dThresold = 0.0001;
//--------------------------------------------------------------------------
void CMaaTcpShaper::Update(/*CMaaShaperThread * pThread*/)
{
    if  (m_Shaper && !m_Shaper->m_Shaper)
    {
        m_Shaper->m_Reserved = 0;
        m_Shaper->m_v0 = 0;//m_Shaper->m_v1 = m_Shaper->m_v2 = 0;
        m_Shaper->m_Max = 0x7fffffff;
        m_Shaper->m_bFullFilled = m_Shaper->m_bFinished = false;

        int Result = 0;
        RecCalc(0, Result); // Init
        m_Reserved = m_Max;

        int r;
        do
        {
            r = 0;
            do
            {
                Result = 0;
                RecCalc(1, Result); // Распределение согласно весам вершин величины m_Reserved, correct m_Reserver on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
                r |= Result;

            }    while(Result);

            Result = 0;
            RecCalc(2, Result); // Popup m_Reserved

        }    while(Result || (m_Reserved > dThresold && r));

        Result = 0;
        RecCalc(3, Result); // Store m_v1 = m_v0 (== Vsr)

        Result = 0;
        RecCalc(4, Result); // Compare m_V == 0 --> add parent->m_Reserved, mark m_bFullFilled
        do
        {
            r = 0;
            do
            {
                Result = 0;
                RecCalc(1, Result); // Распределение согласно весам вершин величины m_Reserved, correct m_Reserver on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
                r |= Result;

            }    while(Result);

            Result = 0;
            RecCalc(2, Result); // Popup m_Reserved

        }    while(Result || (m_Reserved > dThresold && r));

        Result = 0;
        RecCalc(5, Result); // Store m_v2 = m_v0 (== Vsr2)

        Result = 0;
        RecCalc(6, Result); // Compare m_V < m_v0 (m_Vsr2) --> add parent->m_Reserved, mark m_bFullFilled
        do
        {
            r = 0;
            do
            {
                Result = 0;
                RecCalc(1, Result); // Распределение согласно весам вершин величины m_Reserved, correct m_Reserver on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
                r |= Result;

            }    while(Result);

            Result = 0;
            RecCalc(2, Result); // Popup m_Reserved

        }    while(Result || (m_Reserved > dThresold && r));

        Result = 0;
        RecCalc(7, Result); // GetResults (Vsr = m_v2, Vsrd = m_v0)
    }
}
//--------------------------------------------------------------------------
int gStrictTcpShaperLimits = 0; // -1 - no, 0 - default, 1 - yes
void CMaaTcpShaper::RecCalc(int Step, int &Result)
{
    _qword t0 = 0;
    static int depth = -1;
    if  (++depth <= 1)
    {
#ifdef __SHAPERS_PROFILING
        t0 = m_hrt.GetTime();
#endif
    }
    iSockThread_pt = depth;
    L * l;
    CMaaTcpLLShaper * x;
    double tmp, ww, t;
    switch(Step)
    {
    case -1: // Update VAve
        if  (Result != 0 && m_pConnectionInTimeCharacteristics[0])
        {
            _qword value = 0;
            m_pConnectionInTimeCharacteristics[0]->Accumulate(value, true);
            if  (m_pConnectionInTimeCharacteristics[0]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[1]->Accumulate(value, true);
                if  (m_pConnectionInTimeCharacteristics[1]->GetCurPos() % 60 == 0)
                {
                    m_pConnectionInTimeCharacteristics[2]->Accumulate(value, true);
                    if  (m_pConnectionInTimeCharacteristics[2]->GetCurPos() % 24 == 0)
                    {
                        m_pConnectionInTimeCharacteristics[3]->Accumulate(value, true);
                    }
                }
            }
        }
        OnTimer(Result);
        L * next; // is not very need to be in the currecnt implementation (10.01.2010)
        for (l = m_Childs.LookAtFront(); l; l = next)
        {
            next = m_Childs.Next(l);
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        break;
    case 0: // Init
        m_Reserved = 0;
        m_v0 = 0; //m_v1 = m_v2 = 0;
        m_Max = m_MaxSpeed > 0 ? (double)m_MaxSpeed : 0x7fffffff;
        m_bFullFilled = m_bFinished = false;
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            //x->RecCalc(Step, Result);
            x->m_Reserved = 0; //unused
            x->m_v0 = x->m_v1 = x->m_v2 = 0;
            x->m_Max = x->m_MaxSpeedLimit > 0 ? (double)x->m_MaxSpeedLimit : 0x7fffffff;
            x->m_bFullFilled = x->m_bFinished = false;
        }
        break;
    case 1: // Распределение согласно весам вершин величины m_Reserved, correct m_Reserved on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
        if  (m_Reserved > dThresold)
        {
            ww = 0;
            for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
            {
                CMaaTcpShaper * p = l->p;
                if  (!p->m_bFullFilled)
                {
                    ww += p->m_Weight;
                }
                //p->RecCalc(Step, Result);
            }
            for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
            {
                if  (!x->m_bFullFilled)
                {
                    ww += x->m_Weight;
                }
                //x->RecCalc(Step, Result);
            }
            if  (ww > 0)
            {
                tmp = m_Reserved;
                for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
                {
                    CMaaTcpShaper * p = l->p;
                    if  (!p->m_bFullFilled)
                    {
                        t = tmp * p->m_Weight / ww;
                        if  (t + p->m_Reserved + p->m_v0 >= p->m_Max)
                        {
                            t = p->m_Max - (p->m_v0 + p->m_Reserved);
                            p->m_Reserved = p->m_Max - p->m_v0;
                            p->m_bFullFilled = true;
                            Result++;
                        }
                        else
                        {
                            p->m_Reserved += t;
                        }
                        if  (gStrictTcpShaperLimits > 0)
                        {
                            p->m_v0 += t; // can add to version > 1.2.9
                        }
                        m_Reserved -= t;
                    }
                    //p->RecCalc(Step, Result);
                }
                for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
                {
                    if  (!x->m_bFullFilled)
                    {
                        t = tmp * x->m_Weight / ww;
                        if  (t + x->m_v0 >= x->m_Max)
                        {
                            t = x->m_Max - x->m_v0;
                            if  (x->m_V < x->m_Max)
                            {
                                //t -= x->m_Max - x->m_V;
                                t = x->m_V - x->m_v0;
                            }
                            x->m_v0 = x->m_Max;
                            x->m_bFullFilled = true;
                            Result++;
                        }
                        else
                        {
                            x->m_v0 += t;
                        }
                        m_Reserved -= t;
                    }
                    //x->RecCalc(Step, Result);
                }
            }
        }

        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        /*
          for  (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
          {
               x->RecCalc(Step, Result);
          }
          */
        break;
    case 2: // Popup m_Reserved
        if  (m_Shaper->m_Shaper && m_Reserved > dThresold)
        {
            m_Shaper->m_Reserved += m_Reserved;
            /// ++++++++ m_v0 -= m_Reserved; // can add to version > 1.2.9
            m_Reserved = 0;
            m_bFullFilled = true;
            m_bFinished = true;
            Result++;
        }
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        /*
          for  (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
          {
               x->RecCalc(Step, Result);
          }
          */
        break;
    case 3: // Store m_v1 = m_v0 (== Vsr)
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            x->m_v1 = x->m_v0;
        }
        break;
    case 4: // Compare m_V == 0 --> add parent->m_Reserved, mark m_bFullFilled
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            if  (!x->m_bFullFilled)
            {
                if  (x->m_V == 0) //< x->m_v0)
                {
                    x->m_bFullFilled = true;
                    m_Reserved += x->m_v0 - x->m_V;
                    Result++;
                }
            }
        }
        break;
    case 5: // Store m_v2 = m_v0 (== Vsr2)
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            x->m_v2 = x->m_v0;
        }
        break;
    case 6: // Compare m_V < m_v0 (m_Vsr2) --> add parent->m_Reserved, mark m_bFullFilled
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            if  (!x->m_bFullFilled)
            {
                if  ((double)x->m_V < x->m_v0)
                {
                    x->m_bFullFilled = true;
                    m_Reserved += x->m_v0 - x->m_V;
                    Result++;
                }
            }
        }
        break;
    case 7: // GetResults (Vsr = m_v2, Vsrd = m_v0)
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            x->m_Vsr = (int)(x->m_v2 + 0.5);
            x->m_Vsrd = (int)(x->m_v0 + 0.5);
        }
        break;
    }
    if  ((depth == 0 && Step == -1) || (depth == 1 && Step != -1))
    {
#ifdef __SHAPERS_PROFILING
        _qword t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SRecCalc(%d, %d) = %,D\n", &gShaperTempInfo, Step, Result, t - t0);
        int index = Step >= 0 ? Step : 99;
        SockThread_pt[index] += t - t0;
#endif
    }
    depth--;
}
//--------------------------------------------------------------------------
#ifdef _WIN32
//--------------------------------------------------------------------------
//int CMaaSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep, const char * FuncName);
//--------------------------------------------------------------------------
/*
STATUS_ACCESS_VIOLATION Reading or writing to an inaccessible memory location.
STATUS_BREAKPOINT Encountering a hardware-defined breakpoint; used only by debuggers.
STATUS_DATATYPE_MISALIGNMENT Reading or writing to data at an address that is not properly aligned; for example, 16-bit entities must be aligned on 2-byte boundaries. (Not applicable to Intel 80x86 processors.)
STATUS_FLOATING_DIVIDE_BY_ZERO Dividing floating-point type by 0.0.
STATUS_FLOATING_OVERFLOW Exceeding maximum positive exponent of floating-point type.
STATUS_FLOATING_UNDERFLOW Exceeding magnitude of lowest negative exponent of floating-point type.
STATUS_FLOATING_RESEVERED_OPERAND Using a reserved floating-point format (invalid use of format).
STATUS_ILLEGAL_INSTRUCTION Attempting to execute an instruction code not defined by the processor.
STATUS_PRIVILEGED_INSTRUCTION Executing an instruction not allowed in current machine mode.
STATUS_INTEGER_DIVIDE_BY_ZERO Dividing an integer type by 0.
STATUS_INTEGER_OVERFLOW Attempting an operation that exceeds the range of the integer.
STATUS_SINGLE_STEP Executing one instruction in single-step mode; used only by debuggers.
*/
int CMaaSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep, const char * FuncName)
{
    char txt1[256], txt[256];
    sprintf(txt1, "Exception code = %x, Address = %p", code, ep->ExceptionRecord->ExceptionAddress);
    txt[0] = 0;

    int Ret = EXCEPTION_CONTINUE_SEARCH;

    switch(code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        /*
          for  (int i = 0; i < (int)ep->ExceptionRecord->NumberParameters; i++)
          {
               printf("ei[%d] = %08x\t", i, ep->ExceptionRecord->ExceptionInformation[i]);
          }
          printf("\n");
          */
        if  (ep->ExceptionRecord->NumberParameters >= 2)
        {
            sprintf(txt, "EXCEPTION_ACCESS_VIOLATION for data at %p can not be %s", (void *)ep->ExceptionRecord->ExceptionInformation[1], ep->ExceptionRecord->ExceptionInformation[0] == 0 ? "read" : "written");
        }
        else
        {
            sprintf(txt, "EXCEPTION_ACCESS_VIOLATION, data pointer is unknown");
        }
        Ret = EXCEPTION_EXECUTE_HANDLER;
        break;

    case EXCEPTION_BREAKPOINT:
        sprintf(txt, "EXCEPTION_BREAKPOINT");
        break;

    case EXCEPTION_DATATYPE_MISALIGNMENT:
        sprintf(txt, "EXCEPTION_DATATYPE_MISALIGNMENT");
        break;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        sprintf(txt, "EXCEPTION_FLT_DIVIDE_BY_ZERO");
        break;

    case EXCEPTION_FLT_OVERFLOW:
        sprintf(txt, "EXCEPTION_FLT_OVERFLOW");
        break;

    case EXCEPTION_FLT_UNDERFLOW:
        sprintf(txt, "EXCEPTION_FLT_UNDERFLOW");
        break;

        /*
     case STATUS_FLOATING_RESEVERED_OPERAND:
          sprintf(txt, "STATUS_FLOATING_RESEVERED_OPERAND");
          break;
     */

    case EXCEPTION_ILLEGAL_INSTRUCTION:
        sprintf(txt, "EXCEPTION_ILLEGAL_INSTRUCTION");
        break;

    case EXCEPTION_PRIV_INSTRUCTION:
        sprintf(txt, "EXCEPTION_PRIV_INSTRUCTION");
        break;

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        sprintf(txt, "EXCEPTION_INT_DIVIDE_BY_ZERO");
        break;

    case EXCEPTION_INT_OVERFLOW:
        sprintf(txt, "EXCEPTION_INT_OVERFLOW");
        break;

    case EXCEPTION_SINGLE_STEP:
        sprintf(txt, "EXCEPTION_SINGLE_STEP");
        break;
    }

#ifdef __unix__
    mkdir("logs");
#else
    _mkdir("logs");
#endif
    FILE * f = fopen("logs\\exceptions.txt", "a+b");
    f = f ? f : fopen("c:\\maa\\exceptions.txt", "a+b"); // do not change reserved folder
    if  (f)
    {
        time_t t = time(NULL);
        tm t0;
        tm * p = localtime(&t);
        if  (p)
        {
            memcpy(&t0, p, sizeof(t0));
        }
        else
        {
            memset(&t0, 0, sizeof(t0));
        }
        fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: %s: %s\r\n%s\r\n\r\n",
             t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec,
             FuncName, txt1, txt);
        fclose(f);
    }

    //return EXCEPTION_CONTINUE_EXECUTION;
    return Ret; // EXCEPTION_CONTINUE_SEARCH;
    //return EXCEPTION_EXECUTE_HANDLER;
}
//--------------------------------------------------------------------------
static int CMaaShaperThread_RRSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
    return CMaaSEFilter(code, ep, "CMyShaperThread_RRSEFilter()");
}
#endif
//--------------------------------------------------------------------------
unsigned CMaaShaperThread::Run()
{
    unsigned Ret = (unsigned)-1;
    //__try
    {
        Ret = Run1();
    }
    //__except(CMaaShaperThread_RRSEFilter(GetExceptionCode(), GetExceptionInformation()))
    {
    }
    return Ret;
}
//--------------------------------------------------------------------------
unsigned CMaaShaperThread::Run1()
{
    //try
    {
        return Run2();
    }
    /*
     catch(eExitThread)
     {
          throw;
     }
     catch(const char * msg)
     {
          RRCatchHandler("CMaaShaperThread::Run1()", m_Point, msg);
     }
     catch(...)
     {
          RRCatchHandler("CMaaShaperThread::Run1()", m_Point);
          throw;
     }
     */
}
//--------------------------------------------------------------------------
unsigned CMaaShaperThread::Run2()
{
    SetThreadName((_dword)-1, "CMaaShaperThread");
    /*
     {
                CProtocolListColor c(CProtocolListColor::eGreen);
                printf("CMaaTcpShaper thread id = %x\n", GetCurrentThreadId());
        }
        */
#ifdef _WIN32
    DWORD t0 = GetTickCount();
#else
    timeval t0;
    gettimeofday(&t0, NULL);
#endif
    try
    {
#ifdef __SHAPERS_PROFILING
        m_hrt.StartWatch();
#endif

        while(1)
        {
#ifdef _WIN32
            int t = (int)(t0 + 1000 / _SHAPER_TICKS_PER_SECOND - GetTickCount());
            CheckPoint(t > 0 ? (DWORD)t : 0);
            t0 += 1000 / _SHAPER_TICKS_PER_SECOND;
#else
            timeval tt;
            gettimeofday(&tt, NULL);
            int t = 1000 / _SHAPER_TICKS_PER_SECOND - ((tt.tv_sec - t0.tv_sec) * 1000 + (tt.tv_usec - t0.tv_usec) / 1000);
            CheckPoint(t > 0 ? t : 0);
            t0.tv_usec += 1000000 / _SHAPER_TICKS_PER_SECOND;
            if  (t0.tv_usec >= 1000000)
            {
                t0.tv_sec += t0.tv_usec / 1000000;
                t0.tv_usec %= 1000000;
            }
#endif
#ifdef __SHAPERS_PROFILING
            memset(SockThread_pt, 0, sizeof(SockThread_pt));
#endif
            OnTimerUpdate();
        }
    }
    catch(...)
    {
    }
    return 0;
}
//--------------------------------------------------------------------------
void CMaaShaperThread::OnTimerUpdate()
{
    //printf("CMaaShaperThread::OnTimerUpdate() - 1\n"); fflush(stdout);
    gLock.LockM();
    //printf("CMaaShaperThread::OnTimerUpdate() - 2\n"); fflush(stdout);

#ifdef __SHAPERS_PROFILING
    _qword t0 = m_hrt.GetTime();
#endif
    int nnn1 = 0;
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
    {
        for (CMaaTcpShaper::L * l = p->m_Childs.LookAtFront(); l; l = p->m_Childs.Next(l))
        {
            l->p->OnTimerUpdate(/*this*/);
            nnn1++;
        }
        //p->OnTimerUpdate();
    }
    //gShaperTempInfo.Format("%Sm_ShaperList items count = %d\n", &gShaperTempInfo, nnn1);
#ifdef __SHAPERS_PROFILING
    SockThread_pt[20] = nnn1;
    _qword t = m_hrt.GetTime();
    //gShaperTempInfo.Format("%Stime1 = %,D\n", &gShaperTempInfo, t - t0);
    SockThread_pt[21] = t - t0;

    t0 = m_hrt.GetTime();
#endif

    int i = m_AveIndex + 1;
    i = i < _SHAPER_TICKS_PER_SECOND ? i : 0;
    m_AveIndex = i;
    int Result = i ? 0 : ++m_ShaperConditionLastCheckTime;
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
    {
        p->RecCalc(-1, Result);
    }
#ifdef __SHAPERS_PROFILING
    t = m_hrt.GetTime();
    //gShaperTempInfo.Format("%SRecCalc(-1) = %,D\n", &gShaperTempInfo, t - t0);
    SockThread_pt[22] = t - t0;
#endif
    try
    {
#ifdef __SHAPERS_PROFILING
        t0 = m_hrt.GetTime();
#endif
        int N = 1024, M = 0;
        CMaaPtrAE<CMaaTcpLLShaper *> Ev(N);
        try
        {
            for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
            {
                p->FillEvents(Ev, M);
            }
        }
        catch(int)
        {
        }
        catch(...)
        {
        }
#ifdef __SHAPERS_PROFILING
        t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SFillEvents() = %,D\n", &gShaperTempInfo, t - t0);
        SockThread_pt[23] = t - t0;
        // Randomizing events positions
        t0 = m_hrt.GetTime();
#endif
        if  (M > 1)
        {
            for (int i = 0; i < M; i++)
            {
                int x = M * rand() / RAND_MAX;
                if  (x >= 0 && x < M)
                {
                    CMaaTcpLLShaper * hi = Ev[i];
                    Ev[i] = Ev[x];
                    Ev[x] = hi;
                }
            }
        }
#ifdef __SHAPERS_PROFILING
        t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%Srand() = %,D\n", &gShaperTempInfo, t - t0);
        SockThread_pt[24] = t - t0;
        t0 = m_hrt.GetTime();
#endif
        for (int i = 0; i < M; i++)
        {
#ifdef _WIN32
            SetEvent(Ev[i]->m_hEv);
#else
            if  (!Ev[i]->m_hEv)
            {
                Ev[i]->m_hEv = true;
                if  (Ev[i]->m_pSocket)
                {
                    //printf("w(%d=%d)", Ev[i]->m_pSocket->GetSocket(), Ev[i]->GetLenReserved(1000000));
                    Ev[i]->m_pSocket->WakeUp();
                }
            }
#endif
        }
        //printf("((ww))\n");  fflush(stdout);
#ifdef __SHAPERS_PROFILING
        t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SSetEvent() = %,D\n", &gShaperTempInfo, t - t0);
        SockThread_pt[25] = t - t0;
#endif
    }
    catch(...)
    {
    }
    //printf("CMaaShaperThread::OnTimerUpdate() - 3\n"); fflush(stdout);
    gShaperPerformanceInfo = gShaperTempInfo;
    gLock.UnLockM();
    //printf("CMaaShaperThread::OnTimerUpdate() - 4\n"); fflush(stdout);
#ifdef __SHAPERS_PROFILING
    t = m_hrt.GetTime();
    if  (t >= 90000000 && t < (90 + 4)*1000000)
    {
        //printf("CMaaShaperThread::OnTimerUpdate():\n%s", (const char *)gShaperPerformanceInfo);
        //m_logfile.fprintf("CMaaShaperThread::OnTimerUpdate():\n%S", &gShaperPerformanceInfo);
        static int nn = 0;
        if  (!nn++)
        {
            CMaaConcatString text;
            for (int i = 0; i < 100; i++)
            {
                if  (SockThread_pt[i])
                {
                    CMaaString txt;
                    txt.Format("[%d] = %,D\n", i, SockThread_pt[i]);
                    text += txt;
                }
            }
            m_logfile.fprintf("CMaaShaperThread::OnTimerUpdate():\n");
            m_logfile.Write(text);
        }
    }
#endif
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::FillEvents(CMaaPtrAE<CMaaTcpLLShaper *> &Ev, int &pos)
{
    for (CMaaTcpLLShaper * x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
    {
        Ev[pos] = x;
        if  (pos >= (int)Ev.MaxIndex())
        {
            throw 1;
        }
        pos++;
    }
    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        l->p->FillEvents(Ev, pos);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SetNewShaperSpeedLimit(int MaxSpeed, double W)
{
    if  (MaxSpeed < 0)
    {
        MaxSpeed = 0x7fffffff;
    }
    m_MaxSpeed = MaxSpeed / _SHAPER_TICKS_PER_SECOND;
    if  (m_MaxSpeed < 1)
    {
        m_MaxSpeed = 1;
    }
    if  (W > 0.0)
    {
        m_Weight = W;
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::ReInit(double W, int MaxSpeed) // old name: ReInit1
{
    if  (MaxSpeed < 0)
    {
        MaxSpeed = 0x7fffffff;
    }
    m_MaxSpeed = MaxSpeed / _SHAPER_TICKS_PER_SECOND;
    if  (m_MaxSpeed < 1)
    {
        m_MaxSpeed = 1;
    }
    m_Weight = W;
    if  (m_Type == eAutoUserSubshaper)
    {
        m_Type = eUserSubshaper;
    }
    if  (m_Type == eUserSubshaper && m_UserId == -2 && m_Shaper)
    {
        for (CMaaTcpShaper::L * l = m_Shaper->m_Childs.LookAtFront(); l; l = m_Shaper->m_Childs.Next(l))
        {
            if  ((l->p->m_Type == eAutoUserSubshaper || (l->p->m_Type == eUserSubshaper)) && l->p != this)// && l->p->m_UserId == -1))
            {
                ReInitAutoUserSubshaper(l->p);
            }
        }
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::ReInitAutoUserSubshaper(CMaaTcpShaper * s)
{
    // this - template shaper
    // s - shaper to modify
    if  (s->m_Type != eUserSubshaper)
    {
        s->m_MaxSpeed = m_MaxSpeed;
        s->m_Weight = m_Weight;
    }
    s->SetConditions(m_Conditions, false);

    /*
     __lib_printf("src: %S\ndst: %S: type=%d, userid=%d, weight=%d, maxspeed=%d\n",
          &GetShaperNameRawRecursiveLibName(),
          &s->GetShaperNameRawRecursiveLibName(),
          s->m_Type, s->m_UserId, s->m_Weight, _SHAPER_TICKS_PER_SECOND * s->m_MaxSpeed);
     if   (m_Conditions.GetCount() || s->m_Conditions.GetCount())
     {
          __lib_printf("src conditions count = %d, dst conditions count=%d\n", m_Conditions.GetCount(), s->m_Conditions.GetCount());
     }
     */

    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        CMaaTcpShaper *p = NULL;
        if  (l->p->m_Type == eGeneral)
        {
            s->m_GeneralShapers.Find(l->p->m_Name, &p);
        }
        else
        {
            s->m_UserShapers.Find(l->p->m_UserId, &p);
        }
        if  (!p)
        {
            p = TL_NEW CMaaTcpShaper(l->p->m_Type, l->p->m_Name, l->p->m_Weight, l->p->m_MaxSpeed * _SHAPER_TICKS_PER_SECOND, l->p->m_UserId);
            if  (!p)
            {
                continue;
            }
            //if   (!l->p->Add(*p))
            if  (!s->Add(*p)) // 3.12.2009
            {
                delete p;
                continue;
            }
        }
        l->p->ReInitAutoUserSubshaper(p);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::DumpAll()
{
    CMaaString __GetShaperNameRawRecursiveLibName = GetShaperNameRawRecursiveLibName();
    __lib_printf("%S type=%d, userid=%d, weight=%d, maxspeed=%d, conditions count = %d, %d childs, %d connections, (instant): bank=%d, m_V=%d\n",
         &__GetShaperNameRawRecursiveLibName,
         m_Type, m_UserId, m_Weight, _SHAPER_TICKS_PER_SECOND * m_MaxSpeed,
         m_Conditions.GetCount(),
         m_Childs.GetCount(),
         m_ChildsLL.GetCount(),
         m_Bank * _SHAPER_TICKS_PER_SECOND,
         m_V * _SHAPER_TICKS_PER_SECOND
         );

    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        CMaaTcpShaper *p = l->p;
        if  (p)
        {
            p->DumpAll();
        }
    }
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Add(CMaaTcpShaper &s)
{
    if  (!s.m_Shaper)
    {
        s.m_Shaper = this;
        m_Childs.AddAtBack(&s.m_L);
        bool b = true;
        if  (s.m_Type != eGeneral)
        {
            b = m_UserShapers.Add(s.m_UserId, &s) == 0;
        }
        else
        {
            b = m_GeneralShapers.Add(s.m_Name, &s) == 0;
        }
        if  (!b)
        {
            m_Childs.Release(&s.m_L);
            s.m_Shaper = NULL;
        }
        else
        {
            if  (m_Shaper)
            {
                m_Shaper->AddRecursiveConnectionCounter(m_ConnectionsCount[0]);
                /*
                    if   (m_UserId != -2)
                    {
                         ++m_Shaper->m_ConnectionsCount[2];
                    }
                    */
            }
            //m_UseCount++;
        }
        return b;
    }
    return false;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Add(CMaaTcpLLShaper &l)
{
    if  (!l.m_Shaper)
    {
        int N = 0;
        CMaaTcpLLShaper * pl;
        int V0 = 0, Vsr = 0, Vsrd = 0;
        for (pl = m_ChildsLL.LookAtFront(); pl; pl = m_ChildsLL.Next(pl))
        {
            V0 += pl->m_V0;
            Vsr += pl->m_Vsr;
            Vsrd += pl->m_Vsrd;
            N++;
        }
        if  (N > 0)
        {
            double k = (double)N / (double)(N + 1);

            for (pl = m_ChildsLL.LookAtFront(); pl; pl = m_ChildsLL.Next(pl))
            {
                pl->m_V0 = (int)(pl->m_V0 * k); pl->m_V0 = pl->m_V0 > 0 ? pl->m_V0 : 1;
                pl->m_Vsr = (int)(pl->m_Vsr * k); pl->m_Vsr = pl->m_Vsr > 0 ? pl->m_Vsr : 1;
                pl->m_Vsrd = (int)(pl->m_Vsrd * k); pl->m_Vsrd = pl->m_Vsrd > 0 ? pl->m_Vsrd : 1;
            }
        }
        V0 /= (N + 1); V0 = V0 > 0 ? V0 : 1;
        Vsr /= (N + 1); Vsr = Vsr > 0 ? Vsr : 1;
        Vsrd /= (N + 1); Vsrd = Vsrd > 0 ? Vsrd : 1;

        l.m_V0 = V0;
        l.m_Vsr = Vsr;
        l.m_Vsrd = Vsrd;
        l.m_Shaper = this;
        m_ChildsLL.AddAtBack(&l);
        ++m_ConnectionsCount[0];
        /*
          if   (++m_ConnectionsCount[0] == 1 && m_ConnectionsCount[1] == 0)
          {
               if   (m_Shaper)
               {
                    m_Shaper->m_ConnectionsCount[2]++;
               }
          }
          */
        AddRecursiveConnectionCounter(1);
        //m_UseCount++;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Del(CMaaTcpShaper &s)
{
    if  (s.m_Shaper == this)
    {
        m_Childs.Release(&s.m_L);
        if  (s.m_Type != eGeneral)
        {
            m_UserShapers.Remove(s.m_UserId);
        }
        else
        {
            m_GeneralShapers.Remove(s.m_Name);
        }
        s.m_Shaper = NULL;
        if  (m_Shaper)
        {
            m_Shaper->AddRecursiveConnectionCounter(-m_ConnectionsCount[0]);
            /*
               if   (m_UserId != -2)
               {
                    --m_Shaper->m_ConnectionsCount[2];
               }
               */
        }
        //m_UseCount--;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Del(CMaaTcpLLShaper &l)
{
    if  (l.m_Shaper == this)
    {
        m_ChildsLL.Release(&l);
        l.m_Shaper = NULL;
        --m_ConnectionsCount[0];
        AddRecursiveConnectionCounter(-1);
        //m_UseCount--;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::AddRecursiveConnectionCounter(int val)
{
    m_ConnectionsCount[1] += val;
    if  (val > 0 && m_ConnectionsCount[1] == val && m_Shaper)
    {
        m_Shaper->m_ConnectionsCount[2]++;
    }
    if  (val < 0 && m_ConnectionsCount[1] == 0 && m_Shaper)
    {
        m_Shaper->m_ConnectionsCount[2]--;
    }
    if  (m_Shaper)
    {
        m_Shaper->AddRecursiveConnectionCounter(val);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::CopyFrom(CMaaTcpShaper *n, CMaaShaperThread *r)
{
    SetConditions(n->m_Conditions, false);

    for (L * l = n->m_Childs.LookAtFront(); l; l = n->m_Childs.Next(l))
    {
        CMaaTcpShaper *p = TL_NEW CMaaTcpShaper(l->p->m_Type, l->p->m_Name, l->p->m_Weight, l->p->m_MaxSpeed * _SHAPER_TICKS_PER_SECOND, l->p->m_UserId);
        if  (p)
        {
            if  (Add(*p))
            {
                //r->m_ShaperList.AddAtBack(p);
                p->CopyFrom(l->p, r);
            }
            else
            {
                delete p;
            }
        }
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::AddClone(int UserId, CMaaTcpShaper *n, CMaaShaperThread *r)
{
    CMaaTcpShaper *p = TL_NEW CMaaTcpShaper(eAutoUserSubshaper, CMaaString(), n->m_Weight, n->m_MaxSpeed * _SHAPER_TICKS_PER_SECOND, UserId);
    if  (p)
    {
        if  (Add(*p))
        {
            //r->m_ShaperList.AddAtBack(p);
            p->CopyFrom(n, r);
            p->m_UserId = UserId;
        }
        else
        {
            delete p;
        }
    }
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::DeleteUnused()
{
    int Ret = 0;
    L * np;
    for (L * p = m_Childs.LookAtFront(); p; p = np)
    {
        np = m_Childs.Next(p);
        CMaaTcpShaper &s = *(p->p);
        int n = s.DeleteUnused();
        if  (n == 0)
        {
            Del(s);
            delete &s;
        }
        Ret += n;
    }
    Ret += m_ChildsLL.GetCount();
    return Ret;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::HasAttachedSockets()
{
    if  (m_ChildsLL.LookAtFront())
    {
        return true;
    }
    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        if  (l->p->HasAttachedSockets())
        {
            return true;
        }
    }
    return false;
}
//--------------------------------------------------------------------------
void CMaaShaperThread::DeleteUnused()
{
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
    {
        p->DeleteUnused();
    }
}
//--------------------------------------------------------------------------
void CMaaShaperThread::CleanupShapers()
{
    CMaaTcpShaper * n;
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = n)
    {
        n = m_ShaperList.Next(p);
        if  (p->DeleteUnused() == 0)
        {
            m_ShaperList.Release(p);
            if  (p == m_SndSh)
            {
                m_SndSh = NULL;
            }
            if  (p == m_RcvSh)
            {
                m_RcvSh = NULL;
            }
            delete p;
        }
    }
}
//--------------------------------------------------------------------------
/*
void CMaaShaperThread::SetShapers(CMaaXmlNode n0)
{
        for     (CMaaTcpShaper * s = m_ShaperList.LookAtFront(); s; s = m_ShaperList.Next(s))
        {
                s->m_bMarkedForDeletion = true;
        }
        SetShapers(n0, m_SndSh, false);
        SetShapers(n0, m_RcvSh, true);
        m_SndSh->m_bMarkedForDeletion = false;
        m_RcvSh->m_bMarkedForDeletion = false;
        DeleteUnused(m_SndSh);
        DeleteUnused(m_RcvSh);
        CalcWeights();

        if(0)
        {
                CMaaTcpShaper *s, *r;
          FindShaper("Test\x01\x02\x01http", 8, &s, &r);
                CalcWeights();
        }
}

void CMaaShaperThread::SetShapers(CMaaXmlNode n0, CMaaTcpShaper *s, bool IsRecv)
{
        CMaaXmlNode n = n0.FindNode(gCon.m_ShapersListsElm2);
        for     (; !n.IsNull(); n = n.FindNext())
        {
                double w = 1.0;
                int MaxSpeed = -1;
                if      (IsRecv)
                {
                        sscanf(n.FindAttribute(gCon.m_ShapersListsElm2_aRWeight), "%lg", &w);
                        sscanf(n.FindAttribute(gCon.m_ShapersListsElm2_aRMaxSpeed), "%d", &MaxSpeed);
                }
                else
                {
                        sscanf(n.FindAttribute(gCon.m_ShapersListsElm2_aSWeight), "%lg", &w);
                        sscanf(n.FindAttribute(gCon.m_ShapersListsElm2_aSMaxSpeed), "%d", &MaxSpeed);
                }
                if      (n.FindAttribute(gCon.m_ShapersListsElm2_aType) == gCon.m_ShapersListsElm2_aType_vGeneral)
                {
                        CMaaString Name = n.FindAttribute(gCon.m_ShapersListsElm2_aName);
                        CMaaTcpShaper * p = NULL;
                        s->m_GeneralShapers.Find(Name, &p);
                        if      (!p)
                        {
                                p = TL_NEW CMaaTcpShaper(CMaaTcpShaper::eGeneral, Name, w, MaxSpeed);
                                if      (p)
                                {
                                        if      (!s->Add(*p))
                                        {
                                                delete p;
                                                continue;
                                        }
                                        else
                                        {
                                                m_ShaperList.AddAtBack(p);
                                        }
                                }
                        }
                        else
                        {
                                p->ReInit1(w, MaxSpeed);
                        }
                        SetShapers(n, p, IsRecv);
                }
                else
                {
                        int Id = -3;
                        sscanf(n.FindAttribute(gCon.m_ShapersListsElm2_aUserId), "%d", &Id);
                        CMaaTcpShaper * p = NULL;
                        if      (Id == -3)
                        {
                                continue;
                        }
                        s->m_UserShapers.Find(Id, &p);
                        if      (!p)
                        {
                                p = TL_NEW CMaaTcpShaper(CMaaTcpShaper::eUserSubshaper, CMaaString(), w, MaxSpeed, Id);
                                if      (p)
                                {
                                        if      (!s->Add(*p))
                                        {
                                                delete p;
                                                continue;
                                        }
                                        else
                                        {
                                                m_ShaperList.AddAtBack(p);
                                        }
                                }
                        }
                        else
                        {
                                p->m_Type = CMaaTcpShaper::eUserSubshaper;
                                p->ReInit1(w, MaxSpeed);
                        }
                        if      (Id == -2)
                        {
                    SetShapers(n, p, IsRecv);
                                for     (CMaaTcpShaper::L * l = s->m_Childs.LookAtFront(); l; l = s->m_Childs.Next(l))
                                {
                                        p = l->p;
                                        if      (p->m_Type == CMaaTcpShaper::eAutoUserSubshaper && p->HasAttachedSockets())
                                        {
                                                p->ReInit1(w, MaxSpeed);
                                                SetShapers(n, p, IsRecv);
                                        }
                                        else if (p->m_Type == CMaaTcpShaper::eUserSubshaper && p->HasAttachedSockets())
                                        {
                                                p->ReInit1(w, MaxSpeed);
                                                SetShapers(n, p, IsRecv);
                                        }
                                }
                        }
                }
        }
}
*/
void CMaaShaperThread::FindShaper(CMaaString ShaperNameEx, int UserId, CMaaTcpShaper *&s, CMaaTcpShaper *&r)
{
    s = FindShaper(ShaperNameEx, 0, UserId, m_SndSh);
    r = FindShaper(ShaperNameEx, 0, UserId, m_RcvSh);
}

CMaaTcpShaper * CMaaShaperThread::FindShaper(CMaaString NameEx, int pos, int UserId, CMaaTcpShaper *s, bool bAutoCopy)
{
    int End = NameEx.Length();

    if  (pos >= End || !s)
    {
        return NULL;
    }
    while(pos < End)
    {
        if  (NameEx[pos] == '\x02')
        {
            CMaaTcpShaper * n = NULL;
            s->m_UserShapers.Find(UserId, &n);
            if  (!n)
            {
                if  (bAutoCopy)
                {
                    s->m_UserShapers.Find(-2, &n);
                    if  (n)
                    {
                        s->AddClone(UserId, n, this);
                        // CalcWeights(); will be called from user function
                    }
                    n = NULL;
                    s->m_UserShapers.Find(UserId, &n);
                    if  (!n)
                    {
                        return NULL;
                    }
                }
                else
                {
                    return NULL;
                }
            }
            else// if (UserId != -2)
            {
                if  (!n->m_Childs.LookAtFront() && bAutoCopy)
                {
                    CMaaTcpShaper * nn = NULL;
                    s->m_UserShapers.Find(-2, &nn);
                    if  (nn)
                    {
                        n->CopyFrom(nn, this);
                    }
                }
            }
            pos++;

            s = n;
            int x = NameEx.Find(pos, '\x01');
            pos = x >= 0 ? x + 1 : End;
        }
        else
        {
            int x = NameEx.Find(pos, '\x01');
            x = x >= 0 ? x : End;
            CMaaString Name;
            Name = NameEx.Mid(pos, x - pos);
            pos = x;
            CMaaTcpShaper * n = NULL;
            s->m_GeneralShapers.Find(Name, &n);
            if  (!n)
            {
                return NULL;
            }
            s = n;
            pos++;
        }
    }
    return s;
}

void CMaaTcpShaper::OnTimer(int i)
{
    //static DWORD dw0 = GetTickCount();
    //DWORD dw = GetTickCount();
    //int dwt = (int)(dw - dw0);
    //static CMaaFile f("c:\\temp\\shapers.txt", "WCD|SrSw", false);
    m_VTempSum += m_V;
    if  (i)
    {
        m_VAverageLast = m_VTempSum;
        m_VTempSum = 0;
    }
    if  (m_MaxSpeed == 1)
    {
        static int aa = 0;
        aa++;
    }
    //CMaaString txt;
    //if   (dwt <= 100000)
    //{
    //     txt.Format("%4d.%03d %S: m_Bank=%d, m_MaxSpeed=%d, m_V=%d", dwt / 1000, dwt % 1000, &GetShaperNameRawRecursiveLibName(), m_Bank, m_MaxSpeed, m_V);
    //}
    if  (m_Bank >= 0)
    {
        m_Bank = m_MaxSpeed;
    }
    else
    {
        m_Bank += m_MaxSpeed;
        if  (m_Bank < 0)
        {
            m_Bank = 0;
        }
    }
    //if   (dwt <= 100000)
    //{
    //     txt.Format("%S, m_Bank=%d\r\n", &txt, m_Bank);
    //     if   (f.IsOpen())
    //     {
    //          f.Write(txt);
    //     }
    //}
    //else
    //{
    //     f.Close();
    //}
    m_V = 0;

    {
#ifdef __SHAPERS_PROFILING
        _qword t0 = m_hrt.GetTime();
#endif
        CMaaTcpLLShaper * next;
        for (CMaaTcpLLShaper * x = m_ChildsLL.LookAtFront(); x; x = next)
        {
            next = m_ChildsLL.Next(x);
            x->OnTimer(i);
        }
#ifdef __SHAPERS_PROFILING
        _qword t = m_hrt.GetTime();
        SockThread_pt[32] += t -t0;
#endif
    }
}

CMaaString CMaaTcpLLShaper::Dump()
{
    CMaaString txt;
    txt.Format("CMaaTcpLLShaper: this=%p, m_Shaper=%p, m_V=%d, m_Vsrd = %d, m_Vsr=%d, m_VTempSum=%d, m_VAverageLast=%d, m_V0=%d, m_Weight=%.4lf, m_MaxSpeedLimit=%d, "
         "m_v0=%.4lf, m_v1=%.4lf, m_v2=%.4lf, m_Reserved=%.4lf, m_Max=%.4lf, m_bFullFilled=%s, m_bFinished=%s"
         "\r\n",
         this, m_Shaper, _SHAPER_TICKS_PER_SECOND*m_V, _SHAPER_TICKS_PER_SECOND*m_Vsrd, _SHAPER_TICKS_PER_SECOND*m_Vsr, m_VTempSum, m_VAverageLast, _SHAPER_TICKS_PER_SECOND*m_V0, m_Weight, _SHAPER_TICKS_PER_SECOND*m_MaxSpeedLimit,
         m_v0, m_v1, m_v2, m_Reserved, m_Max, m_bFullFilled ? "true" : "false", m_bFinished ? "true" : "false"
         );
    return txt;
}

CMaaString CMaaTcpShaper::Dump()
{
    CMaaString txt;
    txt.Format("CMaaTcpShaper: this=%p, m_Shaper(parent)=%p, m_Type=%d, m_UserId=%d, m_Name=%s, m_MaxSpeed=%d, m_Bank=%d, "
         "m_Weight=%.4lf, m_V0=%d, m_Vsrd=%d, m_Vsr=%d, m_V=%d"
         "m_v0=%.4lf, m_Reserved=%.4lf, m_Max=%.4lf, m_bFullFilled=%s, m_bFinished=%s"
         "\r\n",
         this, m_Shaper, m_Type, m_UserId, (const char *)m_Name, _SHAPER_TICKS_PER_SECOND*m_MaxSpeed, _SHAPER_TICKS_PER_SECOND*m_Bank,
         m_Weight, _SHAPER_TICKS_PER_SECOND*m_V0, _SHAPER_TICKS_PER_SECOND*m_Vsrd, _SHAPER_TICKS_PER_SECOND*m_Vsr, _SHAPER_TICKS_PER_SECOND*m_V,
         m_v0, m_Reserved, m_Max, m_bFullFilled ? "true" : "false", m_bFinished ? "true" : "false"
         );
    CMaaString txt2;
    txt2.Format("m_Childs: %d, m_ChildsLL: %d\r\n", m_Childs.GetCount(), m_ChildsLL.GetCount());
    txt += txt2;
    txt2.Format("%p: L: ", this);
    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        txt += txt2 + l->p->Dump();
    }
    txt2.Format("%p: LL: ", this);
    for (CMaaTcpLLShaper * x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
    {
        txt += txt2 + x->Dump();
    }
    txt += "\r\n\r\n";
    return txt;
}

//---------------------------------------------------------------------------------

CMaaTcpLLShaper::~CMaaTcpLLShaper()
{
    if  (m_Shaper)
    {
        gLock.LockM();
        if  (m_Shaper)
        {
            m_Shaper->Del(*this);
        }
        gLock.UnLockM();
    }
#ifndef __unix__
    CloseHandle(m_hEv);
    m_hEv = NULL;
#else
    m_hEv = false;
#endif
}

#endif // __SHAPERS

#endif // #if defined(__TOOLS_USE_WINSOCK2)

_qword htonq(_qword x)
{
    x = (_qword)htonl(x >> 32) + ((_qword)htonl(x & 0xffffffff) << 32);
    return x;
}

_qword ntohq(_qword x)
{
    x = (_qword)htonl(x >> 32) + ((_qword)htonl(x & 0xffffffff) << 32);
    return x;
}

CMaaShaperThread::CMaaShaperThread()
#ifdef __SHAPERS_PROFILING
:   m_logfile___("c:\\maa\\logfile.txt", "WC|SrSw", false)
#endif
{
    //gpShaper_hrt = &m_hrt___;
    //gpm_logfile = &m_logfile___;

    m_pUserObject = NULL;

    gLock.LockM();
    gLock.AddRef();
    gLock.UnLockM();

    m_AveIndex = m_ShaperConditionLastCheckTime = 0;
    m_ShaperList.AddAtBack(m_SndSh = TL_NEW CMaaTcpShaper("send"));
    m_ShaperList.AddAtBack(m_RcvSh = TL_NEW CMaaTcpShaper("recv"));

    //m_Snd1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 1024000);
    //m_Rcv1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 1024000);
    m_Snd1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 10);
    m_Rcv1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 10);
    if  (m_Snd1000)
    {
        m_SndSh->Add(*m_Snd1000);
    }
    if  (m_Rcv1000)
    {
        m_RcvSh->Add(*m_Rcv1000);
    }

    Create();
}

CMaaShaperThread::~CMaaShaperThread()
{
    Exit();
    //_RPT0(_CRT_WARN,"CMaaShaperThread::~CMaaShaperThread()\n");
    gLock.LockM();
    {
        CMaaTcpShaper * n;
        for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = n)
        {
            n = m_ShaperList.Next(p);
            if  (p->m_Shaper)
            {
                p->m_Shaper->Del(*p);
            }
        }
    }
    CleanupShapers();
    delete m_pUserObject; m_pUserObject = NULL;
    gLock.UnRef();
    if  (gLock.UnLockM() < 0)
    {
        //gpLock = NULL;
    }
}
