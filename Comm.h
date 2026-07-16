// FILE:       comm.h

// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Comm.h

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

#ifndef __TOOLS_COMM_H
#define __TOOLS_COMM_H

//---------------------------------------------------------------------------
// Declaration of CMaaComPort
//---------------------------------------------------------------------------
#ifndef __BORLANDC__
class ToolsExport CMaaComPort
{
protected:
    HANDLE m_hCom;
private:
    BOOL m_fError;
    //char m_StrError [ 128 ];
    CMaaString m_StrError2;
    int m_PortNumber;

protected:
    DCB            m_dcb;
    COMMTIMEOUTS   m_cto;
    BOOL      m_fThrow;

    //CMaaTime m_HrTime;

public:
    static BOOL CHECK_ECHO;
    HANDLE GetHandle () const noexcept { return m_hCom; }
    BOOL EscapeCommFunction ( DWORD dwFunc );
    BOOL PurgeComm ( DWORD Flags = PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
    DWORD WaitCommEvent(DWORD EventMask = EV_RING, DWORD TimeOut = INFINITE);
    CMaaComPort ();
    CMaaComPort ( int Port, BOOL fThrow = TRUE );
    CMaaComPort ( char * Port, BOOL fThrow = TRUE );
    virtual ~CMaaComPort ();

    void SetThrow ( BOOL fThrow = TRUE );
    BOOL Open ( int Port );
    BOOL Open(const char* Port);
    BOOL Open(CMaaString Port);
    BOOL IsOpened ();
    int GetPortNumber ();
    void Close ();
    BOOL Configure ( const char * Settings = nullptr );
    virtual void BeforeApplyingCfg ();
    BOOL GetCommTimeouts ( COMMTIMEOUTS * cto );
    BOOL SetCommTimeouts ( COMMTIMEOUTS & cto );
    BOOL Write ( const void * ptr, int BytesToWrite, int * BytesWritten );
    BOOL WriteLong ( const void * ptr, int BytesToWrite, int * BytesWritten );
    BOOL Read  ( void * ptr, int BytesToRead, int * BytesRead );
    BOOL SendCommand ( const void * cmd, int len );
    BOOL ReceiveCommand ( void * b, int len );
    void printf(const char* format, ...);
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    void printf(const char8_t* format, ...);
#endif
    virtual void Print ( const char * msg );
    virtual void CheckPoint () {}

private:
    // always returns FALSE, makes throw if m_fThrow == TRUE
    //BOOL CheckThrow ();
protected:
public:
    BOOL CheckThrow ( DWORD Error, const char * Format, ... );
};
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
struct COMMTIMEOUTS
{
    DWORD ReadIntervalTimeout;          /* Maximum time between read chars. */
    DWORD ReadTotalTimeoutMultiplier;   /* Multiplier of characters.        */
    DWORD ReadTotalTimeoutConstant;     /* Constant in milliseconds.        */
    DWORD WriteTotalTimeoutMultiplier;  /* Multiplier of characters.        */
    DWORD WriteTotalTimeoutConstant;    /* Constant in milliseconds.        */
};
typedef COMMTIMEOUTS * LPCOMMTIMEOUTS;

#define COMM_DATA_READY        0x01
#define COMM_TRANSMITTER_EMPTY 0x40 // 64

class CMaaComPort
{
public:
    int m_PortNumber, m_BaseAddress;
    COMMTIMEOUTS m_cto;

    CMaaComPort ();
    BOOL Open ( const char * szPort );
    BOOL Open ( int Port );
    void Close () { m_PortNumber = 0; }
    void SetThrow ( BOOL fThrow = TRUE ) {}
    int GetPortNumber ();
    BOOL IsOpened ();
    BOOL Configure ( const char * Settings = nullptr );
    virtual void BeforeApplyingCfg () {}
    BOOL Write ( const void * ptr, int BytesToWrite, int * BytesWritten );
    BOOL WriteLong ( const void * ptr, int BytesToWrite, int * BytesWritten )
    {
        if  ( !IsOpened () )
        {
            return FALSE;
        }
        return Write ( ptr, BytesToWrite, BytesWritten );
    }
    BOOL Read  ( void * ptr, int BytesToRead, int * BytesRead );
    BOOL PurgeComm ();
};

long GetTicks ();
DWORD GetTickCount ();

#endif
//---------------------------------------------------------------------------


#endif // __TOOLS_COMM_H
