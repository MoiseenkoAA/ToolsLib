// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Ìîèñååíêî Àíäðåé Àëåêñååâè÷)
 */

// ToolsLib/Comm.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This files is for COM port working.
 * Initially source code is transferred from my UKT38 project 29.11.2002.
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

#ifndef __BORLANDC__

BOOL CMaaComPort::CHECK_ECHO = FALSE;
//BOOL CHECK_ECHO = TRUE;

//---------------------------------------------------------------------------
// Definition of CMaaComPort
//---------------------------------------------------------------------------
CMaaComPort::CMaaComPort ()
:   m_hCom ( INVALID_HANDLE_VALUE ),
    m_fError ( FALSE ),
    m_fThrow ( TRUE ),
    m_PortNumber ( 0 )
    //, m_HrTime(gHRTime)
{
}
//---------------------------------------------------------------------------
CMaaComPort::CMaaComPort ( int Port, BOOL fThrow )
:   m_hCom ( INVALID_HANDLE_VALUE ),
    m_fError ( FALSE ),
    m_fThrow ( fThrow ),
    m_PortNumber ( 0 )
{
    Open ( Port );
}
//---------------------------------------------------------------------------
CMaaComPort::CMaaComPort ( char * Port, BOOL fThrow )
:   m_hCom ( INVALID_HANDLE_VALUE ),
    m_fError ( FALSE ),
    m_fThrow ( fThrow ),
    m_PortNumber ( 0 )
{
    Open ( Port );
}
//---------------------------------------------------------------------------
CMaaComPort::~CMaaComPort ()
{
    Close ();
}
//---------------------------------------------------------------------------
void CMaaComPort::Close ()
{
    if  ( m_hCom != INVALID_HANDLE_VALUE )
    {
        try
        {
            PurgeComm ();
        }
        catch(...)
        {
        }
        CloseHandle ( m_hCom );
        m_hCom = INVALID_HANDLE_VALUE;
        m_PortNumber = 0;
    }
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::IsOpened ()
{
    return m_hCom != INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------
int CMaaComPort::GetPortNumber ()
{
    return m_PortNumber;
}
//---------------------------------------------------------------------------
void CMaaComPort::SetThrow ( BOOL fThrow )
{
    m_fThrow = fThrow;
}
//---------------------------------------------------------------------------
/*BOOL CMaaComPort::CheckThrow ()
{
     static char ErrMsg [ 1024 ];
     m_fError = TRUE;
     if   ( m_fThrow )
     {
          strcpy ( ErrMsg, m_StrError );
          throw ErrMsg;
     }
     return FALSE;
}*/
//---------------------------------------------------------------------------
BOOL CMaaComPort::CheckThrow ( DWORD Error, const char * Format, ... )
{
    va_list List;
    va_start ( List, Format );
    m_StrError2.FormatV ( Format, List );
    va_end ( List );
    XTOOLastError e ( m_StrError2, Error );
    m_StrError2 = e.GetMsg ();
    if  ( m_fThrow )
    {
        throw e;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::Open ( int Port )
{
    char p [ 20 ];
    sprintf ( p, "COM%u", Port );
    return Open ( p );
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::Open ( const char * Port )
{
    Close ();
    char p [ 40 ];
    memset ( p, 0, sizeof ( p ) );
    if  ( strlen ( Port ) > 30 )
    {
        return CheckThrow ( ERROR_INVALID_PARAMETER, "Bad Port name length: \"%s\"", Port );
    }
    strcpy ( p, Port );
    if  ( !CMaaIsMemEq(p, "\\\\.\\", 4) )
    {
        sprintf ( p, "\\\\.\\%s", Port );
    }
    m_hCom = CreateFileA (
         p,        // file name
         GENERIC_READ | GENERIC_WRITE, // access mode
         0,        //FILE_SHARE_READ|FILE_SHARE_WRITE,  // share mode
         nullptr,     // SD
         OPEN_EXISTING,   // how to create
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,  // file attributes
         nullptr   // handle to template file
         );
    if  ( m_hCom == INVALID_HANDLE_VALUE )
    {
        //return CheckThrow ( -1, "Error opening communication port \"%s\"", Port );
        return CheckThrow ( -1, "Error opening communication port \"%s\"", p + 4 );
    }
    //m_PortNumber = 0;
    dsscanf ( p + 7, "%d", &m_PortNumber );
    return TRUE;
}
BOOL CMaaComPort::Open(CMaaString Port)
{
    Close();
    if (Port.Length() > 30)
    {
        return CheckThrow(ERROR_INVALID_PARAMETER, "Bad Port name length: \"%S\"", &Port);
    }
    if (!Port.IsLeft("\\\\.\\", 4))
    {
        Port = CMaaString("\\\\.\\") + Port;
    }
    Port = Port.Str0Copy();
    m_hCom = CreateFileA(
        Port,        // file name
        GENERIC_READ | GENERIC_WRITE, // access mode
        0,        //FILE_SHARE_READ|FILE_SHARE_WRITE,  // share mode
        nullptr,     // SD
        OPEN_EXISTING,   // how to create
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,  // file attributes
        nullptr   // handle to template file
    );
    if (m_hCom == INVALID_HANDLE_VALUE)
    {
        //return CheckThrow ( -1, "Error opening communication port \"%s\"", Port );
        return CheckThrow(-1, "Error opening communication port \"%s\"", (Port.Length() > 4 ? 4 : 0) + (const char *)Port);
    }
    //m_PortNumber = 0;
    dsscanf((Port.Length() > 7 ? 7 : 0) + (const char *)Port, "%d", &m_PortNumber);
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::Configure ( const char * Settings )
{
    /*
     N O E M S
     "None" Parity checking not used 
     "Odd" Sets the parity bit so that the count of bits set is an odd number 
     "Even" Sets the parity bit so that the count of bits set is an even number 
     "Mark" Leaves the parity bit set to 1 
     "Space" Leaves the parity bit set to 0 
     */

    memset ( &m_dcb, 0, sizeof ( m_dcb ) );
    m_dcb.DCBlength = sizeof ( m_dcb );
    m_dcb.fBinary = TRUE;
    // Another method of getting DBC:
    //if   ( !GetCommState ( m_hCom, &m_dcb ) )
    //{
    //     strcpy ( m_StrError, "Error GetCommState ()" );
    //     return CheckThrow ();
    //}

    //char p [ 1024 ];
    if  ( Settings == nullptr )
    {
        //Settings = "baud=9600 parity=E data=8 stop=2";
    }
    else
    {
        CMaaString p(Settings);

        if  ( !BuildCommDCBA ( p, &m_dcb ) )
        {
            return CheckThrow ( -1, "Error BuildCommDCB ()" );
        }
    }
    if  ( !GetCommTimeouts ( &m_cto ) )
    {
        return FALSE;
    }

    BeforeApplyingCfg ();

    if  ( !SetCommState ( m_hCom, &m_dcb ) )
    {
        return CheckThrow ( -1, "Error SetCommState ()" );
    }
    if  ( !SetCommTimeouts ( m_cto ) )
    {
        return FALSE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------
void CMaaComPort::BeforeApplyingCfg ()
{
    //m_dcb.BaudRate = CBR_9600;
    //m_dcb.ByteSize = 8;
    //m_dcb.Parity = EVENPARITY;
    //m_dcb.StopBits = TWOSTOPBITS;
    m_cto.ReadIntervalTimeout = 0; // not used
    m_cto.ReadTotalTimeoutMultiplier = 0;
    m_cto.ReadTotalTimeoutConstant = 200;
    m_cto.WriteTotalTimeoutMultiplier = 0;
    m_cto.WriteTotalTimeoutConstant = 50;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::GetCommTimeouts ( COMMTIMEOUTS * cto )
{
    if  ( !::GetCommTimeouts( m_hCom, cto ) )
    {
        return CheckThrow ( -1, "Error GetCommTimeouts()" );
    }
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::SetCommTimeouts ( COMMTIMEOUTS & cto )
{
    if  ( !::SetCommTimeouts( m_hCom, &cto ) )
    {
        return CheckThrow ( -1, "Error SetCommTimeouts()" );
    }
    return TRUE;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::Write ( const void * ptr, int BytesToWrite, int * BytesWritten )
{
    OVERLAPPED ov;
    memset ( &ov, 0, sizeof ( ov ) );
    ov.hEvent = CreateEvent ( nullptr, FALSE, FALSE, nullptr );
    if  (!ov.hEvent)
    {
        CheckThrow(-1, "Can't create event object");
        return FALSE;
    }
    DWORD x = 0, e;
    BOOL Ret = WriteFile ( m_hCom, ptr, BytesToWrite, &x, &ov );
    if  ( !Ret && ( e = GetLastError () ) == ERROR_IO_PENDING )
    {
        Ret = GetOverlappedResult ( m_hCom, &ov, &x, TRUE );
        /* or
          if   ( WaitForSingleObject ( ov.hEvent, 10000 ) == WAIT_OBJECT_0 )
          {
               GetOverlappedResult ( m_hCom, &ov, &x, FALSE );
          }
          else
          {
               // Ž¡à ¡®âª  ®è¨¡ª¨
          }
          */
        // added 25.02.2003
        if  ( !Ret )
        {
            e = GetLastError ();
        }
    }
    CloseHandle ( ov.hEvent );
    if  ( BytesWritten )
    {
        * BytesWritten = x;
    }
    if  ( !Ret )
    {
        return CheckThrow ( e, "Error WriteFile()" );
    }
    return Ret;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::Read ( void * ptr, int BytesToRead, int * BytesRead )
{
    OVERLAPPED ov;
    memset ( &ov, 0, sizeof ( ov ) );
    ov.hEvent = CreateEvent ( nullptr, FALSE, FALSE, nullptr );
    DWORD x = 0, e;
    BOOL Ret = ReadFile ( m_hCom, ptr, BytesToRead, &x, &ov );
    if  ( !Ret && ( e = GetLastError () ) == ERROR_IO_PENDING )
    {
        Ret = GetOverlappedResult ( m_hCom, &ov, &x, TRUE );
        /* or
          if   ( WaitForSingleObject ( ov.hEvent, 10000 ) == WAIT_OBJECT_0 )
          {
               GetOverlappedResult ( m_hCom, &ov, &x, FALSE );
          }
          else
          {
               // Ž¡à ¡®âª  ®è¨¡ª¨
          }
          */
        // added 25.02.2003
        if  ( !Ret )
        {
            e = GetLastError ();
        }
    }
    CloseHandle ( ov.hEvent );
    if  ( BytesRead )
    {
        * BytesRead = x;
    }
    if  ( !Ret )
    {
        return CheckThrow ( e, "Error ReadFile()" );
    }
    return Ret;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::SendCommand ( const void * cmd, int len )
{
    int x;
    CMaaString buf(nullptr, len);
    char * b = (char *)(const char *)buf;
    memcpy ( b, cmd, len );
    BOOL Ret = Write ( cmd, len, &x );
    if  ( Ret )
    {
        if  ( x != len )
        {
            //Read ( b, len, nullptr );
            PurgeComm ();
            return CheckThrow ( -1, "SendCommand (): only %d of %d bytes written to port\n", x, len );
        }
        Ret = Read ( b, len, &x );
        if  ( Ret && ( x != len ) ) //|| memcmp ( b, cmd, len ) ) ) memcmp checks echo
        {
            if  ( !x )
            {
                return CheckThrow ( 0, "There is no echo" );
            }
            return CheckThrow ( 0, "Bad echo length" );
        }
        if  ( Ret && x == len && CHECK_ECHO )
        {
            if  ( !CMaaIsMemEq( b, cmd, len ) )
            {
                printf ( " Send:Warning: Echo is not match " );
                int i;
                for ( i = 0; i < x; i++ )
                {
                    printf ( "%02x ", ( unsigned )( unsigned char ) b [ i ] );
                }
                printf ( "must be " );
                for ( i = 0; i < len; i++ )
                {
                    printf ( "%02x ", ( unsigned )( (( unsigned char * ) cmd ) [ i ] ) );
                }
                // return TRUE; // returns ok
            }
        }
    }
    return Ret;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::ReceiveCommand ( void * b, int len )
{
    int x;
    BOOL Ret = Read ( b, len, &x );
    //return CheckThrow ( 0, "Test No answer" );
    if  ( Ret && x != len )
    {
        if  ( !x )
        {
            return CheckThrow ( 0, "No answer" );
        }
        return CheckThrow ( 0, "Bad answer length" );
    }
    return Ret;
}
//---------------------------------------------------------------------------
void CMaaComPort::printf(const char* format, ...)
{
    CMaaString s;
    va_list l;
    va_start(l, format);
    s.FormatV(format, l);
    Print(s);
    va_end(l);
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_CHAR8T_SUPPORT
void CMaaComPort::printf(const char8_t* format, ...)
{
    CMaaString s;
    va_list l;
    va_start(l, format);
    s.FormatV(format, l);
    Print(s);
    va_end(l);
}
#endif
//---------------------------------------------------------------------------
void CMaaComPort::Print ( const char * msg )
{
    int LL = (int)strlen ( msg );
    CMaaString b(msg);
    // For DOS console
    CharToOemA(msg, (char *)(const char *)b);
    ::printf("%s", (const char *)b);
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::PurgeComm ( DWORD Flags )
{
    BOOL Ret = TRUE;
    if  ( m_hCom != INVALID_HANDLE_VALUE )
    {
        Ret = ::PurgeComm ( m_hCom, Flags );
    }
    if  ( !Ret )
    {
        return CheckThrow ( -1, "Error PurgeComm()" );
    }
    return Ret;
}
//---------------------------------------------------------------------------
BOOL CMaaComPort::EscapeCommFunction ( DWORD dwFunc )
{
    BOOL Ret = ::EscapeCommFunction ( m_hCom, dwFunc );
    if  ( !Ret )
    {
        return CheckThrow ( -1, "Error EscapeCommFunction()" );
    }
    return Ret;
}
//---------------------------------------------------------------------------
DWORD CMaaComPort::WaitCommEvent(DWORD EventMask, DWORD TimeOut)
{
    //DWORD EventMask = (EV_RING|EV_CTS|EV_DSR);
    if  (!IsOpened())
    {
        CheckThrow(-1, "File not opened");
        return 0;
    }
    SetCommMask(m_hCom, EventMask);
    EventMask = 0;
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(ov));
    //ov.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    ov.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if  (!ov.hEvent)
    {
        CheckThrow(-1, "Can't create event object");
        return 0;
    }

    BOOL Ret = ::WaitCommEvent(m_hCom, &EventMask, &ov);
    DWORD e = 0, x = 0;
    if  (!Ret && (e = GetLastError ()) == ERROR_IO_PENDING)
    {
        //::printf ( "WaitCommEvent()=FALSE,err=%d(ERROR_IO_PENDING)\n", e );
        //Ret = GetOverlappedResult ( m_hCom, &ov, &x, TRUE );
        ////Ret = GetOverlappedResult ( m_hCom, &ov, &x, FALSE );
        ///* or
        if  (WaitForSingleObject(ov.hEvent, TimeOut) == WAIT_OBJECT_0)
        {
            Ret = GetOverlappedResult(m_hCom, &ov, &x, TRUE);
        }
        else
        {
            // the error handling
            SetCommMask(m_hCom, 0);
            Ret = GetOverlappedResult(m_hCom, &ov, &x, TRUE);
        }
        //*/
        // added 25.02.2003
        if  (!Ret)
        {
            e = GetLastError();
        }
    }
    CloseHandle(ov.hEvent);
    if  (!Ret)
    {
        return CheckThrow ( e, "Error WaitCommEvent()" );
    }
    return EventMask;
}
//---------------------------------------------------------------------------
// created 25.02.2003
BOOL CMaaComPort::WriteLong ( const void * ptr, int BytesToWrite, int * BytesWritten )
{
    if  ( !BytesToWrite )
    {
        return Write ( ptr, BytesToWrite, BytesWritten );
    }
    BOOL Ret = TRUE;
    int TotalWritten = 0;
    while( TotalWritten < BytesToWrite )
    {
        int x = 0;
        Ret = Write ( TotalWritten + ( const char * ) ptr, BytesToWrite - TotalWritten, &x );
        if  ( Ret )
        {
            TotalWritten += x;
            if  (BytesWritten)
            {
                *BytesWritten = TotalWritten;
            }
            if  (TotalWritten < BytesToWrite)
            {
                //WaitCommEvent(EV_TXEMPTY, 100);
            }
        }
        else
        {
            break;
        }
    }
    if  ( BytesWritten )
    {
        * BytesWritten = TotalWritten;
    }
    return Ret;
}
//---------------------------------------------------------------------------

#else // __BORLANDC__

CMaaComPort::CMaaComPort ()
:   m_PortNumber ( 0 ),
    m_BaseAddress ( 0x2f8 )
{
    memset ( &m_cto, 0, sizeof ( m_cto ) );
}

BOOL CMaaComPort::Open ( const char * szPort )
{
    if  ( strlen ( szPort ) > 3 )
    {
        dsscanf ( szPort + 3, "%d", &m_PortNumber );
    }
    else
    {
        m_PortNumber = 0;
    }
    if  ( m_PortNumber < 1 || m_PortNumber > 4 )
    {
        m_PortNumber = 0;
        return FALSE;
    }
    static int BaseAddresses [ 4 ] = { 0x3f8, 0x2f8, 0x3e8, 0x2e8 };
    m_BaseAddress = BaseAddresses [ m_PortNumber - 1 ];
    return TRUE;
}

BOOL CMaaComPort::Open ( int Port )
{
    char szPort [ 10 ];
    sprintf ( szPort, "COM%d", Port );
    return Open ( szPort );
}

//---------------------------------------------------------------------------
int CMaaComPort::GetPortNumber ()
{
    return m_PortNumber;
}
//---------------------------------------------------------------------------

BOOL CMaaComPort::Configure ( const char * Settings )
{
    // "baud=2400 parity=N data=8 stop=1"; // dos init
    int baud=2400, data=8, stop=1;
    char parity = 'N';
    dsscanf ( Settings, "baud=%d parity=%c data=%d stop=%d", &baud, &parity, &data, &stop );
    BeforeApplyingCfg ();
    return TRUE;
}

BOOL CMaaComPort::IsOpened ()
{
    return m_PortNumber != 0;
}

BOOL CMaaComPort::Write ( const void * ptr, int BytesToWrite, int * BytesWritten )
{
    if  ( !IsOpened () )
    {
        return FALSE;
    }
    int x = 0;
    for ( int i = 0; i < BytesToWrite; i++ )
    {
        outportb ( m_BaseAddress, * ( i + ( unsigned char * ) ptr ) );
        while( ( inportb ( m_BaseAddress + 5 ) & COMM_TRANSMITTER_EMPTY ) == 0 );
        x = i + 1;
    }
    if  ( BytesWritten )
    {
        * BytesWritten = x;
    }
    return TRUE;
}

BOOL CMaaComPort::Read  ( void * ptr, int BytesToRead, int * BytesRead )
{
    if  ( !IsOpened () )
    {
        return FALSE;
    }
    int x = 0;
    //inportb ( m_BaseAddress );
    DWORD t = GetTickCount ();
    DWORD tp = m_cto.ReadTotalTimeoutConstant + m_cto.ReadTotalTimeoutMultiplier * BytesToRead;
    for ( int i = 0; i < BytesToRead; i++ )
    {
        unsigned char fReady;
        do
        {
            fReady = inportb ( m_BaseAddress + 5 ) & COMM_DATA_READY;

        }    while ( fReady == 0 && GetTickCount () - t < tp );

        if  ( fReady )
        {
            * ( i + ( unsigned char * ) ptr ) =  inportb ( m_BaseAddress );
            x = i + 1;
        }
        if  ( GetTickCount () - t >= tp )
        {
            break;
        }
    }
    if  ( BytesRead )
    {
        * BytesRead = x;
    }
    return TRUE;
}

BOOL CMaaComPort::PurgeComm  ()
{
    if  ( !IsOpened () )
    {
        return FALSE;
    }
    inportb ( m_BaseAddress );    // PURGE_RXCLEAR
    return TRUE;
}

long GetTicks ()
{
    return * ( long * ) MK_FP ( 0, 0x46c );
}

DWORD GetTickCount ()
{
    return GetTicks () * 55U;
}


#endif // __BORLANDC__
