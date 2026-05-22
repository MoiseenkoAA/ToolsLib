
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Console.h

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

//--------------------------------------------------------------------------
// Keys
//--------------------------------------------------------------------------
#ifndef __BORLANDC__
// _WIN32
#define KEY_LT 0xe04b
#define KEY_RT 0xe04d
#define KEY_UP 0xe048
#define KEY_DN 0xe050
#define KEY_F11 0xe085
#define KEY_F12 0xe086
#define KEY_CTRL_F11 0xe089
#define KEY_CTRL_F12 0xe08A
#define KEY_INS 0xe052
#define KEY_DEL 0xe053
#define KEY_HOME 0xe047
#define KEY_END  0xe04F
#define KEY_PGUP 0xe049
#define KEY_PGDN 0xe051

#else
// __BORLANDC__
#define KEY_LT 0x014b
#define KEY_RT 0x014d
#define KEY_UP 0x0148
#define KEY_DN 0x0150
#define KEY_F11 0x0185
#define KEY_F12 0x0186
#define KEY_CTRL_F11 0x0189
#define KEY_CTRL_F12 0x018A
#define KEY_INS 0x0152
#define KEY_DEL 0x0153
#define KEY_HOME 0x0147
#define KEY_END  0x014F
#define KEY_PGUP 0x0149
#define KEY_PGDN 0x0151
#define KEY_ALT_X 0x012D
#define KEY_ALT_ESC 0x0101
#define KEY_ALT_ENTER 0x011C
#endif

#define KEY_ESC 0x1b
#define KEY_F1 0x013b
#define KEY_F2 0x013c
#define KEY_F3 0x013d
#define KEY_F4 0x013e
#define KEY_F5 0x013f
#define KEY_F6 0x0140
#define KEY_F7 0x0141
#define KEY_F8 0x0142
#define KEY_F9 0x0143
#define KEY_F10 0x0144
#define KEY_CTRL_F1 0x015E
#define KEY_CTRL_F2 0x015F
#define KEY_CTRL_F3 0x0160
#define KEY_CTRL_F4 0x0161
#define KEY_CTRL_F5 0x0162
#define KEY_CTRL_F6 0x0163
#define KEY_CTRL_F7 0x0164
#define KEY_CTRL_F8 0x0165
#define KEY_CTRL_F9 0x0166
#define KEY_CTRL_F10 0x0167
#define KEY_CTRL(Char) ( ( ( Char ) | 0x20 ) - 'a' + 1 )
//--------------------------------------------------------------------------
#ifdef __BORLANDC__
#define HANDLE int
#define INVALID_HANDLE_VALUE -1
typedef short SHORT;
typedef struct _COORD {
    SHORT X;
    SHORT Y;
} COORD, *PCOORD;
#endif

//--------------------------------------------------------------------------
// class CMaaConsole
//
// Added 13.03.2003
//--------------------------------------------------------------------------

class ToolsExport CMaaConsole
{
protected:
    void GotoXY__locked(int x = -1, int y = -1) noexcept; // move cursor position to (x,y). Upper left corner is (0,0)
    void GotoXY_locked(int x, int y) noexcept;  // move output position to (x,y)
#ifndef __BORLANDC__
    BOOL WriteW_locked(const CMaaString& txt) noexcept; // write wchars (__WC_[]) to output position
    BOOL WriteW_locked(int x, int y, const CMaaString& txt) noexcept; // write wchars (__WC_[]) to output position
#endif
public:
    HANDLE m_Handle;
    COORD m_Cur;
    _word m_Color;
    int m_SizeX, m_SizeY;
#ifndef __BORLANDC__
    CMaaAtomicFastMutex m_Mtx;
#endif
    CMaaConsole(HANDLE h = INVALID_HANDLE_VALUE) noexcept;
    HANDLE GetHandle() noexcept;
    void GotoXY_(int x = -1, int y = -1) noexcept; // move cursor position to (x,y). Upper left corner is (0,0)
    void GotoXY(int x, int y) noexcept;  // move output position to (x,y)
#ifndef __BORLANDC__
    BOOL WriteW(const CMaaString& txt) noexcept; // write wchars (__WC_[]) to output position
    BOOL WriteW(int x, int y, const CMaaString& txt) noexcept; // write wchars (__WC_[]) to output position
#endif
    BOOL Write(CMaaString txt) noexcept; // write to output position
    BOOL Write(int x, int y, CMaaString txt) noexcept; // write to output position
    void printfV(const char* format, va_list list) noexcept;
    void printfV(int x, int y, const char* format, va_list list) noexcept;  // write to output position
    void printf(const char* format, ...) noexcept;  // write to output position
    void printf(int x, int y, const char* format, ...) noexcept;  // write to output position
    void printf(const char8_t* format, ...) noexcept;  // write to output position
    void printf(int x, int y, const char8_t* format, ...) noexcept;  // write to output position
    static BOOL IsKeyPressed() noexcept;
    static int GetCh() noexcept; // get key code ("extended" code)
    void Cls(); // clear screen
};
//--------------------------------------------------------------------------
#if 0
void test_console()
{
    CMaaSetConsoleUtf8AndLocale ConsoleInitializer;

    {
        CMaaConsole c;
        c.Cls();
        //c.GotoXY(0, 10);
        c.printf(0, 10, u8"ABCDE АБВГД %.1lf. ESC - exit", 123.456);
        c.GotoXY_();// c.m_Cur.X, c.m_Cur.Y);
        //c.GotoXY_(0, 11);
        char ch = '*';
        while (1)
        {
            if (c.IsKeyPressed())
            {
                int k = c.GetCh();
                c.printf(" 0x%x   ", k);
                if (k == 0x1b)
                {
                    break;
                }
            }
            //c.GotoXY(0, 11);
            c.printf(0, 11, "%c", ch);
            //c.GotoXY_(c.m_Cur.X, c.m_Cur.Y);
            c.GotoXY_();// c.m_Cur.X, c.m_Cur.Y);
            ch ^= '*' ^ ' ';
        }
        c.GotoXY_(0, 12);
    }
}
#endif
