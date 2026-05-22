
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Console.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains console io working with Borland C++ 3.1 (DOS).
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

//--------------------------------------------------------------------------
CMaaConsole::CMaaConsole(HANDLE h) noexcept
{
    m_SizeX = 80;
    m_SizeY = 24;
    if  (h == INVALID_HANDLE_VALUE)
    {
#ifndef __BORLANDC__
        h = ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif
    }
    m_Handle = h;
    memset(&m_Cur, 0, sizeof(m_Cur));
#ifdef __BORLANDC__
    m_Color = 0x07;
#else
    m_Color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (::GetConsoleScreenBufferInfo(m_Handle, &csbi))
    {
        m_SizeX = csbi.dwSize.X;
        m_SizeY = csbi.dwSize.Y;
    }
#endif
}

HANDLE CMaaConsole::GetHandle() noexcept
{
    return m_Handle;
}

void CMaaConsole::GotoXY__locked(int x, int y) noexcept
{
#ifndef __BORLANDC__
    //CMaaAtomicFastMutexLocker lk(m_Mtx);
#endif
    x = x >= 0 ? x : m_Cur.X;
    y = y >= 0 ? y : m_Cur.Y;
#ifndef __BORLANDC__
    COORD coord;
    coord.X = x;
    coord.Y = y;
    ::SetConsoleCursorPosition(m_Handle, coord);
#else
    gotoxy(x + 1, y + 1);
#endif
    GotoXY_locked(x, y);
}

void CMaaConsole::GotoXY_locked(int x, int y) noexcept
{
#ifndef __BORLANDC__
    //CMaaAtomicFastMutexLocker lk(m_Mtx);
#endif
    m_Cur.X = x;
    m_Cur.Y = y;
}

void CMaaConsole::GotoXY_(int x, int y) noexcept
{
#ifndef __BORLANDC__
    CMaaAtomicFastMutexLocker lk(m_Mtx);
#endif
    x = x >= 0 ? x : m_Cur.X;
    y = y >= 0 ? y : m_Cur.Y;
#ifndef __BORLANDC__
    COORD coord;
    coord.X = x;
    coord.Y = y;
    ::SetConsoleCursorPosition(m_Handle, coord);
#else
    gotoxy(x + 1, y + 1);
#endif
    GotoXY_locked(x, y);
}

void CMaaConsole::GotoXY(int x, int y) noexcept
{
#ifndef __BORLANDC__
    CMaaAtomicFastMutexLocker lk(m_Mtx);
#endif
    m_Cur.X = x;
    m_Cur.Y = y;
}

#ifndef __BORLANDC__
BOOL CMaaConsole::WriteW_locked(const CMaaString &txt) noexcept
{
    const int len = (warning_int)txt.Length() / 2;
    const _WC_* w = (const _WC_*)(const char*)txt;
    if (len > 512)
    {
        return FALSE;
    }
    CHAR_INFO Buffer [ 512 ];
    for ( int i = 0; i < len; i++ )
    {
        //Buffer[i].Char.AsciiChar = ptr[i];
        Buffer[i].Char.UnicodeChar = w[i];
        Buffer[i].Attributes = m_Color;
    }
    COORD dwBufferSize;
    dwBufferSize.X = len;
    dwBufferSize.Y = 1;
    COORD dwBufferCoord;
    dwBufferCoord.X = dwBufferCoord.Y = 0;

    //CMaaAtomicFastMutexLocker lk(m_Mtx);

    SMALL_RECT WriteRegion;
    WriteRegion.Left = m_Cur.X;
    WriteRegion.Right = m_Cur.X + len - 1;
    WriteRegion.Top = m_Cur.Y;
    WriteRegion.Bottom = m_Cur.Y;
    BOOL Ret = ::WriteConsoleOutputW(m_Handle, Buffer, dwBufferSize, dwBufferCoord, &WriteRegion);
    m_Cur.X += len;
    m_Cur.Y += m_Cur.X / m_SizeX;
    m_Cur.X %= m_SizeX;
    return Ret;
}
BOOL CMaaConsole::WriteW_locked(int x, int y, const CMaaString& txt) noexcept
{
    //CMaaAtomicFastMutexLocker lk(m_Mtx);
    GotoXY_locked(x, y);
    return WriteW_locked(txt);
}

BOOL CMaaConsole::WriteW(const CMaaString& txt) noexcept
{
    const int len = (warning_int)txt.Length() / 2;
    const _WC_* w = (const _WC_*)(const char*)txt;
    if (len > 512)
    {
        return FALSE;
    }
    CHAR_INFO Buffer[512];
    for (int i = 0; i < len; i++)
    {
        //Buffer[i].Char.AsciiChar = ptr[i];
        Buffer[i].Char.UnicodeChar = w[i];
        Buffer[i].Attributes = m_Color;
    }
    COORD dwBufferSize;
    dwBufferSize.X = len;
    dwBufferSize.Y = 1;
    COORD dwBufferCoord;
    dwBufferCoord.X = dwBufferCoord.Y = 0;

    CMaaAtomicFastMutexLocker lk(m_Mtx);

    SMALL_RECT WriteRegion;
    WriteRegion.Left = m_Cur.X;
    WriteRegion.Right = m_Cur.X + len - 1;
    WriteRegion.Top = m_Cur.Y;
    WriteRegion.Bottom = m_Cur.Y;
    BOOL Ret = ::WriteConsoleOutputW(m_Handle, Buffer, dwBufferSize, dwBufferCoord, &WriteRegion);
    m_Cur.X += len;
    m_Cur.Y += m_Cur.X / m_SizeX;
    m_Cur.X %= m_SizeX;
    return Ret;
}
BOOL CMaaConsole::WriteW(int x, int y, const CMaaString& txt) noexcept
{
    CMaaAtomicFastMutexLocker lk(m_Mtx);
    GotoXY_locked(x, y);
    return WriteW_locked(txt);
}
#endif

BOOL CMaaConsole::Write(CMaaString txt) noexcept
{
#ifndef __BORLANDC__
    txt = Utf8ToUnicode(txt);
    return WriteW(txt);
#else
    int len = txt.Length();
    if (len > 512)
    {
        return FALSE;
    }
    // puttext can be used instead of direct video output
    char* pVideo = m_Cur.Y * 160 + 2 * m_Cur.X + (char*)MK_FP(0xb800, 0x0);
    for (int i = 0; i < len; i++)
    {
        *pVideo = ptr[i];
        pVideo[1] = m_Color;
        pVideo += 2;
    }
    m_Cur.X += len;
    m_Cur.Y += m_Cur.X / m_SizeX;
    m_Cur.X %= m_SizeX;
    return TRUE;
#endif
}

BOOL CMaaConsole::Write(int x, int y, CMaaString txt) noexcept
{
#ifndef __BORLANDC__
    txt = Utf8ToUnicode(txt);
    return WriteW(x, y, txt);
#else
    GotoXY(x, y);
    int len = txt.Length();
    if (len > 512)
    {
        return FALSE;
    }
    // puttext can be used instead of direct video output
    char* pVideo = m_Cur.Y * 160 + 2 * m_Cur.X + (char*)MK_FP(0xb800, 0x0);
    for (int i = 0; i < len; i++)
    {
        *pVideo = ptr[i];
        pVideo[1] = m_Color;
        pVideo += 2;
    }
    m_Cur.X += len;
    m_Cur.Y += m_Cur.X / m_SizeX;
    m_Cur.X %= m_SizeX;
    return TRUE;
#endif
}

void CMaaConsole::printfV(const char* format, va_list list) noexcept
{
    CMaaString txt;
    txt.FormatV(format, list);
#ifndef __BORLANDC__
    txt = Utf8ToUnicode(txt);
    WriteW(txt);
#else
    Write(txt);
#endif
}

void CMaaConsole::printfV(int x, int y, const char* format, va_list list) noexcept  // write to output position
{
    CMaaString txt;
    txt.FormatV(format, list);
#ifndef __BORLANDC__
    txt = Utf8ToUnicode(txt);
    WriteW(x, y, txt);
#else
    Write(x, y, txt);
#endif
}

void CMaaConsole::printf(const char * format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    printfV(format, list);
    va_end(list);
}

void CMaaConsole::printf(const char8_t* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    printfV((const char*)format, list);
    va_end(list);
}

void CMaaConsole::printf(int x, int y, const char* format, ...) noexcept  // write to output position
{
    va_list list;
    va_start(list, format);
    printfV(x, y, format, list);
    va_end(list);
}

void CMaaConsole::printf(int x, int y, const char8_t* format, ...) noexcept  // write to output position
{
    va_list list;
    va_start(list, format);
    printfV(x, y, (const char*)format, list);
    va_end(list);
}

BOOL CMaaConsole::IsKeyPressed() noexcept
{
    return kbhit();
}

int CMaaConsole::GetCh() noexcept
{
    int c = getch();
#ifdef __BORLANDC__
    if  (!c)
    {
        c = (KEY_LT & 0xff00) + getch();
    }
#else
    if  (c == (KEY_LT >> 8) || !c)
    {
        if  (c)
        {
            c = (c << 8) + getch();
        }
        else
        {
            c = (KEY_F1 & 0xff00) + getch();
        }
    }
#endif
    return c;
}

void CMaaConsole::Cls()
{
#ifdef __BORLANDC__
    clrscr ();
#else
    CMaaAtomicFastMutexLocker lk(m_Mtx);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if  (::GetConsoleScreenBufferInfo(m_Handle, &csbi))
    {
        m_SizeY = csbi.dwSize.Y;
        //CMaaString txt(nullptr, (m_SizeX = csbi.dwSize.X), CMaaString::eNotInitMem);
        CMaaString txt(nullptr, (m_SizeX = csbi.dwSize.X));
        if  (!txt.IsValid())
        {
            throw "Memory allocation error";
        }
        //txt.Fill(' ');
        //txt.Fill();
#ifndef __BORLANDC__
        txt = Utf8ToUnicode(txt);
#endif

        for (int i = csbi.dwSize.Y; i--;)
        {
#ifndef __BORLANDC__
            GotoXY_locked(0, i);
            WriteW_locked(txt);
#else
            GotoXY(0, i);
            Write(txt);
#endif
        }
#ifndef __BORLANDC__
        GotoXY__locked(0, 0);
#else
        GotoXY_(0, 0);
#endif
    }
    else
    {
        ::system("cls");
    }
#endif
}

//--------------------------------------------------------------------------
