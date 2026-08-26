
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/SprintfEx.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains strings and string's functions implementation.
 * This CMaaString does not throws throws exception out of boundaries.
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

//template <int_ xThrow, bool bCountMode> class CMaaConcatString_
//---------------------------------------------------------------------------
void SprintfEx(const char* strFormat, int FormatLen, va_list list, int_ SrcLine, const char* SrcFile)
{
    if (!IsValid())
    {
        return;
    }
    const size_t Length0 = Length();
    try
    {
        sSprintfBuffers Buffers;
        char* Buffer = Buffers.pTmpBuffer ? Buffers.pTmpBuffer->ptr() : nullptr;
        char* FormatBuffer = Buffers.pTmpFormatBuffer ? Buffers.pTmpFormatBuffer->ptr() : nullptr;

        char Char2[2] = { 0, 0 };
        char i64_txt[64];

        CMaaString S_String;

        CMaaPtr_<char, -1> Format_(FormatLen + 10 <= MAX_format_s_LEN + 1 ? 0 : FormatLen + 10);
        char* Format = FormatLen + 10 <= MAX_format_s_LEN + 1 ? FormatBuffer : Format_;

        if (!Buffer || !Format)
        {
            return;
        }

        memcpy(Format, strFormat, FormatLen);
        Format[FormatLen] = 0;

        for (int Start = 0; Start < FormatLen; )
        {
            int i;
            for (i = Start; i < FormatLen; i++)
            {
                if (Format[i] == '%')
                {
                    break;
                }
            }
            if (i != Start)
            {
                Add(&Format[Start], i - Start);
                if (i >= FormatLen)
                {
                    break;
                }
            }
            Start = i;
            char c = Format[++i];
            if (c == '%')
            {
                *this += '%';
                Start = i + 1;
                continue;
            }

            char sign_ch = c == '+' || c == '-' ? c : 0;
            int_ Alignment = 0; // right
            if (c == '-')
            {
                Alignment++;  // left
                c = Format[++i];
            }
            switch (c)
            {
                //case '#': not supported
            case '+':
            case ' ':
                c = Format[++i];
            }
            char prefix_ch;
            if (c == '#')
            {
                prefix_ch = '#';
                c = Format[++i];
                if (!sign_ch && (c == '+' || c == '-'))
                {
                    sign_ch = c;
                    c = Format[++i];
                }
            }
            else
            {
                prefix_ch = 0;
            }
            int_ width, precision = -1;
            char c0 = c;
            TOOLSLIB_SPRINTF_GET_NUMBER(width);
            if (c == '.')
            {
                c = Format[++i];
                TOOLSLIB_SPRINTF_GET_NUMBER(precision);
            }
            int_ Float = 1;     // simple float
            /*
            if   (c == 'l' && Format[i + 1] == 'S')
            {
                c = 'U'; i++;
            }
            */
            if (c == 'z' && (Format[i + 1] == 'u' || Format[i + 1] == 'x' || Format[i + 1] == 'X'))
            {
                i++; // %zu %zx size_t unsigned or hex, addition: %zX
            }
            if (c == 'h')
            {
                Float--;
                c = Format[++i];
                if (c == 'h')
                {
                    Float--;
                    c = Format[++i];
                }
            }
            else
            {
                switch (c)
                {
                case 'L': Float++; CMaa_fallthrough;
                case 'l': Float++; CMaa_fallthrough;
                case 'F':
                case 'N':
                    //case 'h':
                    c = Format[++i];
                }
                if (Float == 2 && c == 'l')
                {
                    Float++;
                    c = Format[++i];
                }
            }
            int_ arg_width = width, arg_precision = precision;
            int_ l, r;
            bool bFastStrCpy = false;

            char c1, c2;
            c1 = c2 = c;
            if (c1 == '_')
            {
                c2 = c = Format[++i];
            }
            if (c2 == ',')
            {
                c = Format[++i];
            }
            const char BackUp = Format[++i];
            Format[i] = 0;
            char* pChar = nullptr;

            float xf;
            double xd;
            long double xld;
            void* pVoid;
            size_t xSz;
            int_ Int = 0;
            const int_ c_add = (Float == 2 && (c == 'c' || c == 's')) ? 0x100 : 0;

            if (c != 'l')
            {
                if (width == -2)
                {
                    arg_width = va_arg(list, int_);
                }
                if (precision == -2)
                {
                    arg_precision = va_arg(list, int_);
                }
            }

            int BufStrLen = -1;
            switch (c + c_add)
            {
            case 'c':
                Char2[0] = (char)va_arg(list, int_);
                pChar = Char2;
                BufStrLen = 1;
                CMaa_fallthrough;
            case ':': // MAC address
                //case 'M':
                if (!pChar)
                {
                    pChar = i64_txt;
                    const unsigned char* x = va_arg(list, unsigned char*);
                    if (!x)
                    {
                        pChar = (char*)"(null)";
                    }
                    else if (_is_bad_read_ptr(x, 6))
                    {
                        pChar = (char*)"(bad_ptr)";
                    }
                    else
                    {
                        MacToText(pChar, (const _byte*)x);
                    }
                }
                CMaa_fallthrough;
#ifndef __BORLANDC__
            case 'D':
                if (!pChar)
                {
                    const __int64 x = va_arg(list, __int64);
                    if (!x)
                    {
                        pChar = (char*)"0";
                    }
                    else
                    {
                        pChar = &i64_txt[(int_)sizeof(i64_txt) - 1];
                        *pChar = 0;
#if 1
                        if (c1 == '_')
                        {
                            const uint_ d10 = 10;
                            if (c2 != ',')
                            {
                                for (__uint64 y = (__uint64)x; y; y /= d10)
                                {
                                    *(--pChar) = '0' + (char)(y % d10);
                                }
                            }
                            else
                            {
                                __uint64 y = (__uint64)x;
                                *(--pChar) = '0' + (char)(y % d10);
                                int_ nDigits = 0;
                                for (; y /= d10; )
                                {
                                    if (!((++nDigits) % 3))
                                    {
                                        *(--pChar) = ',';
                                    }
                                    *(--pChar) = '0' + (char)(y % d10);
                                }
                                if (nDigits >= 3)
                                {
                                    width = -1;
                                    c0 = 'D';
                                }
                            }
                            if (sign_ch == '+')
                            {
                                *(--pChar) = '+';
                                width = -1;
                                c0 = 'D';
                            }
                        }
                        else
                        {
                            const int_ d10 = 10;
                            if (c2 != ',')
                            {
                                for (__int64 y = x > 0 ? -x : x; y; y /= d10)
                                {
                                    *(--pChar) = '0' - (char)(y % d10);
                                }
                            }
                            else
                            {
                                __int64 y = x > 0 ? -x : x;
                                *(--pChar) = '0' - (char)(y % d10);
                                int_ nDigits = 0;
                                for (; y /= d10; )
                                {
                                    if (!((++nDigits) % 3))
                                    {
                                        *(--pChar) = ',';
                                    }
                                    *(--pChar) = '0' - (char)(y % d10);
                                }
                                if (nDigits >= 3)
                                {
                                    width = -1;
                                    c0 = 'D';
                                }
                            }
                            if (x < 0)
                            {
                                *(--pChar) = '-';
                                width = -1;
                                c0 = 'D';
                            }
                            else if (sign_ch == '+')
                            {
                                *(--pChar) = '+';
                                width = -1;
                                c0 = 'D';
                            }
                        }
#else
                        char tmp[64];
                        pChar = tmp;
                        if (c1 == '_')
                        {
                            __uint64 xx = x;
                            __uint64 y = x;
                            const __uint64 d10 = 10;
                            int_ l;
                            for (l = 0; y; l++, y /= d10);
                            pChar[l] = 0;
                            for (; l--; )
                            {
                                pChar[l] = '0' + (char)(xx % d10);
                                xx /= d10;
                            }
                        }
                        else
                        {
                            if (x < 0)
                            {
                                *pChar++ = '-';
                                width = -1;
                                c0 = 'D';
                            }
                            else
                            {
                                if (sign_ch == '+')
                                {
                                    *pChar++ = '+';
                                }
                                x = -x;
                            }
                            __int64 y = x;
                            const __int64 d10 = 10;
                            int_ l;
                            for (l = 0; y; l++, y /= d10);
                            pChar[l] = 0;
                            for (; l--; )
                            {
                                pChar[l] = '0' - (char)(x % d10);
                                x /= d10;
                            }
                        }
                        if (c2 != ',')
                        {
                            strcpy(i64_txt, tmp);
                        }
                        else
                        {
                            char* pp = i64_txt;
                            if (tmp[0] == '-' || tmp[0] == '+')
                            {
                                *pp++ = tmp[0];
                            }
                            const int_ l = (int_)strlen(pChar);
                            pp += l + (l - 1) / 3;
                            *pp-- = 0;

                            for (int_ i = 0; i < l; i++)
                            {
                                if (i && !(i % 3))
                                {
                                    *pp-- = ',';
                                }
                                *pp-- = pChar[l - i - 1];
                            }
                        }
                        pChar = i64_txt;
#endif
                    }
                }
                CMaa_fallthrough;
#endif // __BORLANDC__
            case 'I': // IP Address
                if (!pChar)
                {
                    //pChar = i64_txt;
                    const _dword x = va_arg(list, _dword);
                    //sprintf ( pChar, "%d.%d.%d.%d", (int_)(x >> 24), (int_)( x & 0xff0000L ) >> 16, (int_)( x & 0xff00 ) >> 8, (int_)(x & 0xff) );
                    CMaaIpToTextEx(pChar = i64_txt, (_IP)x);
                }
                CMaa_fallthrough;
            case 'J': // IP Address
                if (!pChar)
                {
                    pChar = i64_txt;
                    const _byte* x = va_arg(list, _byte*);
                    if (!x)
                    {
                        pChar = (char*)"(null)";
                    }
                    else if (_is_bad_read_ptr(x, 16))
                    {
                        pChar = (char*)"(bad_ptr)";
                    }
                    else
                    {
                        *pChar = '\0';
                        CMaaIpToTextEx(i64_txt, x);
                    }
                }
                CMaa_fallthrough;
            case 's':
                if (!pChar)
                {
                    pChar = va_arg(list, char*);
                    if (!pChar)
                    {
                        pChar = (char*)"(null)";
                    }
                    else if (_is_bad_string_ptr(pChar, -1))
                    {
                        pChar = (char*)"(bad_strptr)";
                    }
                }
                // my format
                precision = BufStrLen >= 0 ? (warning_int)BufStrLen : (warning_int)int_strlen(pChar);

                if (precision > arg_precision && arg_precision >= 0)
                {
                    precision = arg_precision;
                }
                if (arg_width > precision)
                {
                    if (Alignment)
                    {
                        // left alignment
                        l = 0;
                        r = arg_width - precision;
                        c0 = ' ';
                    }
                    else
                    {
                        // right alignment
                        if (c == 'D' && c0 == '0')
                        {
                        }
                        else
                        {
                            c0 = ' ';
                        }
                        l = arg_width - precision;
                        r = 0;
                    }
                }
                else
                {
                    l = r = 0;
                }
                BufStrLen = precision;
                bFastStrCpy = true;
                break;
            case 'm':
                //if (c == 'm')
            {
                const unsigned char* ptr = va_arg(list, unsigned char*);
                int_ len = va_arg(list, int_);
                if (!ptr)
                {
                    S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                }
                else if (_is_bad_read_ptr(ptr, len))
                {
                    S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadPtrErr);
                }
                else
                {
                    CMaaConcatString_<xThrow, bCountMode> bf(Buffer, MAX_proc_s_LEN);
                    char tmp[64 + 8];
                    while (len > 0)
                    {
                        char* p = tmp;
                        int_ i;
                        for (i = 0; i < 16; i++)
                        {
                            unsigned char c = (i < len) ? ptr[i] : ' ';
                            c = (c >= ' ' && c != 0x7f) ? c : '.';
                            *p++ = (char)c;
                        }
                        *p++ = ' ';
                        //*p++ = ' ';
                        for (i = 0; i < 16 && i < len; i++)
                        {
                            //sprintf(p, " %02X", ptr[i]);
                            //p += strlen(p);
                            *p++ = ' ';
                            Byte2Hex_nzt_H(p, ptr[i]);
                            p += 2;
                        }
                        *p++ = '\n';
                        bf.Add(tmp, (int_)(p - tmp));
                        len -= 16;
                        ptr += 16;
                    }

#if 0
                    if (len > 0)
                    {
                        char* p = tmp;
                        memcpy(p, (const char*)tmp1, tmp1.Length());
                        memcpy(p += tmp1.Length(), " of ", 4);
                        memcpy(p += 4, (const char*)tmp2, tmp2.Length());
                        memcpy(p += tmp2.Length(), " bytes are dumped\n", int_strlen(" bytes are dumped\n"));
                        p += int_strlen(" bytes are dumped\n");
                        bf.Add(tmp, (int_)(p - tmp));
                    }
#endif
                    if constexpr (bCountMode)
                    {
                        BufStrLen = (int)bf.Length();
                        break;
                    }
                    else
                    {
                        S_String = (CMaaString)bf;
                    }
                }
            }
            CMaa_fallthrough;
            case 'T':
            case 't':
                if (c == 'T' || c == 't')
                {
                    const time_t t = va_arg(list, time_t);
                    const int_ mode = c == 'T' ? va_arg(list, int_) : 0;
                    S_String = GetTextDateTime(t, mode);
                }
                CMaa_fallthrough;
            case 0x100 + 's':
                if (c == 's')
                {
#ifdef _WIN32_000
                    wchar_t* p = va_arg(list, wchar_t*);
                    int_ n = WideCharToMultiByte(CP_ACP, 0, p, -1, nullptr, 0, nullptr, nullptr);
                    CMaaString txt(nullptr, n);
                    n = WideCharToMultiByte(CP_ACP, 0, p, -1, (char*)(const char*)txt, txt.Length(), nullptr, nullptr);
                    if (n > 0 && txt[n - 1] == 0)
                    {
                        n--;
                    }
                    txt = txt.Left(n);
                    S_String = txt;
#else
                    const wchar_t* p = va_arg(list, wchar_t*);
                    if (!p)
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                    }
                    else if (_is_bad_wstring_ptr(p, -1))
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_wstrptr_Err);
                    }
                    else
                    {
                        S_String = wchar_t_ToUtf8(p);
                    }
#endif
                }
                CMaa_fallthrough;
            case 'U':
                if (c == 'U')
                {
                    const _WC_* p = va_arg(list, _WC_*);
                    if (!p)
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                    }
                    else if (_is_bad_wc2string_ptr(p, -1))
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_wc2ptr_Err);
                    }
                    else
                    {
                        S_String = UnicodeToUtf8(p);
                    }
                }
                CMaa_fallthrough;
            case 0x100 + 'c':
                if (c == 'c')
                {
                    const uint_ u = va_arg(list, uint_);
                    const wchar_t wch = (wchar_t)u;
                    S_String = wchar_t_ToUtf8(&wch, 1);
                }
                CMaa_fallthrough;
            case 'S':
            case 'M':
            {
                if (c == 'S' || c == 'M')
                {
                    void* pS = va_arg(list, void*);
                    int_ nFirst = 0, nCount = 0;
                    if (c == 'M')
                    {
                        nFirst = va_arg(list, int_);
                        nCount = va_arg(list, int_);
                    }
                    if (!pS)
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr); // CMaaStringRO("(null)");
                    }
                    else
                    {
                        const CMaaString* s = (const CMaaString*)pS;
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
#ifndef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        int_ b = !s->m_pImp ? 1 : 0;
#else
                        int_ b = !s->m_pImp ? 1 :
                            __is_bad_write_ptr((void*)&s->m_pImp, sizeof(void*)) ? 2 :
                            __is_bad_write_ptr((void*)&s->m_pImp->f2.m_Flags12, sizeof(s->m_pImp->f2.m_Flags12)) ? 2 : 0;
#endif
                        const CMaaString64* s64 = nullptr;
                        switch (b)
                        {
                        case 0:
                            switch (s->GetImpVersion())
                            {
                            case CMAASTRFL_eImp32:
                                //S_String = *s;
                                break;
                            case CMAASTRFL_eImp64:
                                //S_String = *(const CMaaString64 *)s;
                                s64 = (const CMaaString64*)s;
                                s = nullptr;
                                break;
                            default:
                                s = nullptr;
                                S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_string_flag_Err);
                                //S_String.Empty();
                            }
                            break;
                        case 1:
                            S_String.Empty();
                            s = nullptr;
                            b = 0;
                        }
#ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s64 && CMaaString64::IsBadStrPtr((CMaaString64*)s64))))
#else
                        if (false)
#endif
#else
#ifndef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        int_ b = !s->IsValid() ? 1 : 0;
#else
                        int_ b = !s->m_pImp ? 1 :
                            __is_bad_write_ptr((void*)&s->m_pImp, sizeof(void*)) ? 2 :
                            __is_bad_write_ptr((void*)&s->m_pImp->f2.m_Flags12, sizeof(s->m_pImp->f2.m_Flags12)) ? 2 : 0;
#endif
                        const CMaaString32* s32 = nullptr;
                        switch (b)
                        {
                        case 0:
                            switch (s->GetImpVersion())
                            {
                            case CMAASTRFL_eImp32:
                                //S_String = *(const CMaaString32*)s;
                                s32 = (const CMaaString32*)s;
                                s = nullptr;
                                break;
                            case CMAASTRFL_eImp64:
                                //S_String = *s;
                                break;
                            default:
                                s = nullptr;
                                S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_string_flag_Err);
                                //S_String.Empty();
                            }
                            break;
                        case 1:
                            S_String.Empty();
                            s = nullptr;
                            b = 0;
                        }
#ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s32 && CMaaString32::IsBadStrPtr((CMaaString32*)s32))))
#else
                        if (false)
#endif
#endif
#else
#ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        if (CMaaString::IsBadStrPtr((CMaaString*)s))
#else

#ifndef CMaaStringImp_NOT_NULL_MODE
                        if (false)
#else
                        if (!s->m_pImp)
#endif

#endif
#endif
#ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        {
#ifdef _WIN32
                            FILE* f = fopen("c:\\maa\\BadStrPtr.txt", "a+b");
#else
                            FILE* f = fopen("/home/maa/mylogs/BadStrPtr.txt", "a+b");
#endif
                            if (f)
                            {
                                const time_t t = time(nullptr);
                                tm t0;
                                const tm* p = localtime(&t);
                                if (p)
                                {
                                    memcpy(&t0, p, sizeof(t0));
                                }
                                else
                                {
                                    memset(&t0, 0, sizeof(t0));
                                }
                                fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: BadStrPtr#0: %s:%d '", t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec, SrcFile, SrcLine);
                                fwrite(strFormat, 1, FormatLen, f);
                                fwrite("'\r\n", 1, 3, f);
                                fclose(f);
                            }
                            S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_string_ptr_Err);
                        }
#else
                        {
                        }
#endif
                        else
                        {
                            if (s)
                            {
                                S_String = *s;
                            }
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
                            else if (s64)
                            {
                                S_String = *s64;
                            }
#else
                            else if (s32)
                            {
                                S_String = *s32;
                            }
#endif
#endif
                            if (c == 'M')
                            {
                                S_String = S_String.RefMid(nFirst, nCount);
                            }
                        }
                    }
                }

                //if (c != 'm')
                if (arg_precision >= 0 || arg_width > 0)
                {
                    precision = (warning_int)S_String.Utf8CharCount();

                    if (arg_precision >= 0 && arg_precision < precision)
                    {
                        if (Alignment || arg_width < 0)
                        {
                            // left alignment
                            S_String = S_String.RefLeft(S_String.Utf8Pos(arg_precision));
                        }
                        else
                        {
                            // right alignment
                            S_String = S_String.RefMid(S_String.Utf8Pos(precision - arg_precision));
                        }
                        precision = (warning_int)S_String.Utf8CharCount();
                    }
                    if (arg_width > precision)
                    {
                        if (Alignment)
                        {
                            // left alignment
                            l = 0;
                            r = arg_width - precision;
                        }
                        else
                        {
                            // right alignment
                            l = arg_width - precision;
                            r = 0;
                        }
                        c0 = ' ';
                        bFastStrCpy = true;
                    }
                }
                c = 'S';
            }
            break;
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                if (Float == 3)
                {
                    const unsigned long long x = va_arg(list, unsigned long long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    //break;
                    BufStrLen =
                        c == 'd' || c == 'i' ? SignedIntegerToString_d<long long>((long long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ? UnsignedIntegerToString_d<unsigned long long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'x' || c == 'X' ? UnsignedIntegerToString_x<unsigned long long>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(x) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                        UnsignedIntegerToString_o<unsigned long long>(x, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                    break;
                }
                if (Float == 2)
                {
                    const unsigned long x = va_arg(list, unsigned long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    //break;
                    BufStrLen =
                        c == 'd' || c == 'i' ? SignedIntegerToString_d<long>((long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ? UnsignedIntegerToString_d<unsigned long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'x' || c == 'X' ? UnsignedIntegerToString_x<unsigned long>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(x) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                        UnsignedIntegerToString_o<unsigned long>(x, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                    break;
                }
                // Float:
                // case 0:
                // unsigned short / short as int
                // case -1:
                // unsigned char x / char as int

                //default:
                //Int = va_arg(list, int_);
                //CMaa_fallthrough;

                Int = va_arg(list, int_);
                BufStrLen =
                    c == 'd' || c == 'i' ? SignedIntegerToString_d<int_>(Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                    c == 'u' ? UnsignedIntegerToString_d<uint_>((uint_)Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                    c == 'x' || c == 'X' ? UnsignedIntegerToString_x<uint_>((uint_)Int, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(uint_) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                    UnsignedIntegerToString_o<uint_>((uint_)Int, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                break;
            case 'w':
            case 'W':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                if (Float == 3)
                {
                    const _uqword x = va_arg(list, _uqword);
                    //__utf8_printf("so1=%0llW, so1_=%0llW, *m=%0llW\n", so1, so1_, *(_qword*)(t1 + len - 7));
                    BufStrLen = UnsignedIntegerToString_x<_uqword>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(_uqword) : arg_width, c == 'w' ? 'x' : 'X', c0 == '0' ? '0' : 0, prefix_ch ? c : 0);
                    break;
                }
                if (Float == 2)
                {
                    const _dword x = va_arg(list, _dword);
                    BufStrLen = UnsignedIntegerToString_x<_dword>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(_dword) : arg_width, c == 'w' ? 'x' : 'X', c0 == '0' ? '0' : 0, prefix_ch ? c : 0);
                    break;
                }
                Int = va_arg(list, int_);
                BufStrLen = UnsignedIntegerToString_x<_word>((_word)Int, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(_word) : arg_width, c == 'w' ? 'x' : 'X', c0 == '0' ? '0' : 0, prefix_ch ? c : 0);
                break;
            case 'p':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                pVoid = va_arg(list, void*);
                xSz = (size_t)pVoid;
                BufStrLen = UnsignedIntegerToString_p<size_t>(xSz, Buffer, arg_width, 'X', prefix_ch ? 'x' : 0);
                break;
            case 'e':
            case 'E':
            case 'f':
            case 'g':
            case 'G':
                switch (Float)
                {
                case 3:
                    xld = va_arg(list, long double);
                    TOOLSLIB_SPRINTF_OUTPUT(xld);
                    break;
                case 2:
                    xd = va_arg(list, double);
                    TOOLSLIB_SPRINTF_OUTPUT(xd);
                    break;
                default: // 1
                    xf = (float)va_arg(list, double); // bc31&vc60 feature: they push double to stack instead of float arguments
                    TOOLSLIB_SPRINTF_OUTPUT(xf);
                }
                break;
            case 'z':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                xSz = va_arg(list, size_t);
                BufStrLen = Format[i - 1] == 'x' || Format[i - 1] == 'X' ?
                    UnsignedIntegerToString_x<size_t>(xSz, Buffer, arg_width, Format[i - 1], c0 == '0' ? '0' : 0, prefix_ch ? 'x' : 0) :
                    UnsignedIntegerToString_d<size_t>(xSz, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0);
                break;
                //case 'p':
                //    pVoid = va_arg(list, void*);
                //    TOOLSLIB_SPRINTF_OUTPUT(pVoid);
                //    break;
                //case 'z':
                //    xSz = va_arg(list, size_t);
                //    TOOLSLIB_SPRINTF_OUTPUT(xSz);
                //    break;
            case 'l':
                strcpy(Buffer, Format + Start + 1);
                /*
                if (BackUp == 'u')
                {
                    Format[i] = BackUp;
                    BackUp = Format[++i];
                    Format[i] = 0;
                    unsigned long long x = va_arg(list, unsigned long long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    Buffer[0] = 0;
                }
                else
                {
                    long long x = va_arg(list, long long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    Buffer[0] = 0;
                }
                */
                break;
            case 'n':
            {
                int_* p = va_arg(list, int_*);
                *p = (int_)GetMemoryString().RefMid((StrInt)Length0).__CharCount();
                Buffer[BufStrLen = 0] = 0;
            }
            break;
            default:
                TOOLSLIB_SPRINTF_OUTPUT(Int);
            }
            Format[i] = BackUp;
            if (c == 'S')
            {
                if (bFastStrCpy)
                {
                    Add(c0, l);
                    *this += S_String;
                    Add(c0, r);
                }
                else
                {
                    *this += S_String;
                }
            }
            else
            {
                if (bFastStrCpy)
                {
                    Add(c0, l);
                    Add(pChar, BufStrLen);
                    Add(c0, r);
                }
                else
                {
                    Add(Buffer, BufStrLen >= 0 ? BufStrLen : int_strlen(Buffer));
                }
            }
            Start = i;
        }
        if (!IsValid())
        {
            m_bIsValid = true;
            m_StringLength = Length0;
        }
    }
    catch (...)
    {
        if (!IsValid())
        {
            m_bIsValid = true;
            m_StringLength = Length0;
        }
        throw;
    }
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void Sprintf2Ex(const char* strFormat, int FormatLen, const char* strText, int TextLen, va_list list, int_ SrcLine, const char* SrcFile)
{
    if (!IsValid())
    {
        return;
    }
    size_t Length0 = Length();
    try
    {
        sSprintf2Buffers Buffers;
        char* Buffer = Buffers.pTmpBuffer ? Buffers.pTmpBuffer->ptr() : nullptr;
        char* FormatBuffer = Buffers.pTmpFormatBuffer ? Buffers.pTmpFormatBuffer->ptr() : nullptr;
        sSprintf2Buffers::CMaaTmpSprintf2StringsArray*& pStringsArray = Buffers.pStringsArray;

        char Char2[2] = { 0, 0 };
        char i64_txt[64];
        CMaaString S_String;

        CMaaPtr_<char, -1> Format_(FormatLen + 10 <= MAX_format_s_LEN + 1 ? 0 : FormatLen + 10);
        char* Format = FormatLen + 10 <= MAX_format_s_LEN + 1 ? FormatBuffer : Format_;

        if (!Buffer || !Format) // || !Text)
        {
            //SetNewLengthValue(Length0); // Empty();
            return;
        }

        memcpy(Format, strFormat, FormatLen);
        Format[FormatLen] = 0;

        int NewStringApproxLen = TextLen;

        int i;

        int_ MaxN = 0;
        for (i = 0; i < FormatLen; i++)
        {
            if (Format[i] == '%')
            {
                MaxN++;
            }
        }
        if (pStringsArray)
        {
            pStringsArray->m.CheckResize(MaxN);
        }
        else
        {
            pStringsArray = TL_NEW sSprintf2Buffers::CMaaTmpSprintf2StringsArray(MaxN);
        }
        if (!pStringsArray || (int_)pStringsArray->m.MaxIndex() < MaxN)
        {
            //SetNewLengthValue(Length0); // Empty();
            return;
        }
        sSprintf2Buffers::CMaaTmpSprintf2StringsArray::Rec* ReplText = pStringsArray->m;
        int_& N = pStringsArray->N;

        for (int Start = 0; Start < FormatLen && N < MaxN; )
        {
            for (i = Start; i < FormatLen; i++)
            {
                if (Format[i] == '%')
                {
                    break;
                }
            }
            if (i >= FormatLen)
            {
                break;
            }
            //if (i != Start)
            //{
                //c = Format[i];
                //Format [ i ] = 0;
                //*this += & Format [ Start ];
                //Format [ i ] = c;
            //}
            Start = i;
            char c = Format[++i];
            if (c == '%')
            {
                //*this += "%";
                Start = i + 1;
                continue;
            }

            char sign_ch = c == '+' || c == '-' ? c : 0;
            int_ Alignment = 0; // right
            if (c == '-')
            {
                Alignment++;  // left
                c = Format[++i];
            }
            char prefix_ch;
            if (c == '#')
            {
                prefix_ch = '#';
                c = Format[++i];
                if (!sign_ch && (c == '+' || c == '-'))
                {
                    sign_ch = c;
                    c = Format[++i];
                }
            }
            else
            {
                prefix_ch = 0;
            }
            if (c == 'z' && (Format[i + 1] == 'u' || Format[i + 1] == 'x' || Format[i + 1] == 'X'))
            {
                i++; // %zu %zx size_t unsigned or hex, addition: %zX
            }
            switch (c)
            {
                //case '#': not supported
            case '+':
            case ' ':
                c = Format[++i];
            }
            int_ width, precision = -1;
            int_ l, r;
            bool bFastStrCpy = false;

            char c0 = c;
            TOOLSLIB_SPRINTF_GET_NUMBER(width);
            if (c == '.')
            {
                c = Format[++i];
                TOOLSLIB_SPRINTF_GET_NUMBER(precision);
            }
            int_ Float = 1;     // simple float
            if (c == 'h')
            {
                Float--;
                c = Format[++i];
                if (c == 'h')
                {
                    Float--;
                    c = Format[++i];
                }
            }
            else
            {
                switch (c)
                {
                case 'L': Float++; CMaa_fallthrough;
                case 'l': Float++; CMaa_fallthrough;
                case 'F':
                case 'N':
                    //case 'h':
                    c = Format[++i];
                }
                if (Float == 2 && c == 'l')
                {
                    Float++;
                    c = Format[++i];
                }
            }
            int_ arg_width = width, arg_precision = precision;
            char c1, c2;
            c1 = c2 = c;
            if (c1 == '_')
            {
                c2 = c = Format[++i];
            }
            if (c2 == ',')
            {
                c = Format[++i];
            }
            const char BackUp = Format[++i];
            Format[i] = 0;
            char* pChar = nullptr;

            float xf;
            double xd;
            long double xld;
            void* pVoid;
            size_t xSz;
            int_ Int = 0;
            const int_ c_add = (Float == 2 && (c == 'c' || c == 's')) ? 0x100 : 0;

            if (c != 'l')
            {
                if (width == -2)
                {
                    arg_width = va_arg(list, int_);
                }
                if (precision == -2)
                {
                    arg_precision = va_arg(list, int_);
                }
            }

            int BufStrLen = -1;
            switch (c + c_add)
            {
            case 'c':
                Char2[0] = (char)va_arg(list, int_);
                pChar = Char2;
                BufStrLen = 1;
                CMaa_fallthrough;
            case ':': // MAC address
                //case 'M':
                if (!pChar)
                {
                    pChar = i64_txt;
                    const unsigned char* x = va_arg(list, unsigned char*);
                    if (!x)
                    {
                        pChar = (char*)"(null)";
                    }
                    else if (_is_bad_read_ptr(x, 6))
                    {
                        pChar = (char*)"(bad_ptr)";
                    }
                    else
                    {
                        MacToText(pChar, (const _byte*)x);
                    }
                }
                CMaa_fallthrough;
    #ifndef __BORLANDC__
            case 'D':
                if (!pChar)
                {
                    const __int64 x = va_arg(list, __int64);
                    if (!x)
                    {
                        pChar = (char*)"0";
                    }
                    else
                    {
                        pChar = &i64_txt[(int_)sizeof(i64_txt) - 1];
                        *pChar = 0;
    #if 1
                        if (c1 == '_')
                        {
                            const uint_ d10 = 10;
                            if (c2 != ',')
                            {
                                for (__uint64 y = (__uint64)x; y; y /= d10)
                                {
                                    *(--pChar) = '0' + (char)(y % d10);
                                }
                            }
                            else
                            {
                                __uint64 y = (__uint64)x;
                                *(--pChar) = '0' + (char)(y % d10);
                                int_ nDigits = 0;
                                for (; y /= d10; )
                                {
                                    if (!((++nDigits) % 3))
                                    {
                                        *(--pChar) = ',';
                                    }
                                    *(--pChar) = '0' + (char)(y % d10);
                                }
                                if (nDigits >= 3)
                                {
                                    width = -1;
                                    c0 = 'D';
                                }
                            }
                            if (sign_ch == '+')
                            {
                                *(--pChar) = '+';
                                width = -1;
                                c0 = 'D';
                            }
                        }
                        else
                        {
                            const int_ d10 = 10;
                            if (c2 != ',')
                            {
                                for (__int64 y = x > 0 ? -x : x; y; y /= d10)
                                {
                                    *(--pChar) = '0' - (char)(y % d10);
                                }
                            }
                            else
                            {
                                __int64 y = x > 0 ? -x : x;
                                *(--pChar) = '0' - (char)(y % d10);
                                int_ nDigits = 0;
                                for (; y /= d10; )
                                {
                                    if (!((++nDigits) % 3))
                                    {
                                        *(--pChar) = ',';
                                    }
                                    *(--pChar) = '0' - (char)(y % d10);
                                }
                                if (nDigits >= 3)
                                {
                                    width = -1;
                                    c0 = 'D';
                                }
                            }
                            if (x < 0)
                            {
                                *(--pChar) = '-';
                                width = -1;
                                c0 = 'D';
                            }
                            else if (sign_ch == '+')
                            {
                                *(--pChar) = '+';
                                width = -1;
                                c0 = 'D';
                            }
                        }
    #else
                        char tmp[64];
                        pChar = tmp;
                        if (c1 == '_')
                        {
                            __uint64 xx = x;
                            __uint64 y = x;
                            const __uint64 d10 = 10;
                            int_ l;
                            for (l = 0; y; l++, y /= d10);
                            pChar[l] = 0;
                            for (; l--; )
                            {
                                pChar[l] = '0' + (char)(xx % d10);
                                xx /= d10;
                            }
                        }
                        else
                        {
                            if (x < 0)
                            {
                                *pChar++ = '-';
                                width = -1;
                                c0 = 'D';
                            }
                            else
                            {
                                if (sign_ch == '+')
                                {
                                    *pChar++ = '+';
                                }
                                x = -x;
                            }
                            __int64 y = x;
                            const __int64 d10 = 10;
                            int_ l;
                            for (l = 0; y; l++, y /= d10);
                            pChar[l] = 0;
                            for (; l--; )
                            {
                                pChar[l] = '0' - (char)(x % d10);
                                x /= d10;
                            }
                        }
                        if (c2 != ',')
                        {
                            strcpy(i64_txt, tmp);
                        }
                        else
                        {
                            char* pp = i64_txt;
                            if (tmp[0] == '-' || tmp[0] == '+')
                            {
                                *pp++ = tmp[0];
                            }
                            const int_ l = (int_)strlen(pChar);
                            pp += l + (l - 1) / 3;
                            *pp-- = 0;

                            for (int_ i = 0; i < l; i++)
                            {
                                if (i && !(i % 3))
                                {
                                    *pp-- = ',';
                                }
                                *pp-- = pChar[l - i - 1];
                            }
                        }
                        pChar = i64_txt;
    #endif
                    }
                    //fprintf(stderr, "-- arg_width=%d, Alignment=%d, precision=%d, arg_width-precision=%d\n", arg_width, Alignment, precision, arg_width-precision);
                }
                CMaa_fallthrough;
    #endif // __BORLANDC__
            case 'I': // IP Address
                if (!pChar)
                {
                    const _dword x = va_arg(list, _dword);
                    CMaaIpToTextEx(pChar = i64_txt, (_IP)x);
                }
                CMaa_fallthrough;
            case 'J': // IP Address
                if (!pChar)
                {
                    pChar = i64_txt;
                    const _byte* x = va_arg(list, _byte*);
                    if (!x)
                    {
                        pChar = (char*)"(null)";
                    }
                    else if (_is_bad_read_ptr(x, 16))
                    {
                        pChar = (char*)"(bad_ptr)";
                    }
                    else
                    {
                        *pChar = '\0';
                        CMaaIpToTextEx(i64_txt, x);
                    }
                }
                CMaa_fallthrough;
            case 's':
                if (!pChar)
                {
                    pChar = va_arg(list, char*);
                }
                if (!pChar)
                {
                    pChar = (char*)"(null)";
                }
                else if (_is_bad_string_ptr(pChar, -1))
                {
                    pChar = (char*)"(bad_strptr)";
                }
                // my format
                precision = BufStrLen >= 0 ? (warning_int)BufStrLen : (warning_int)int_strlen(pChar);

                if (precision > arg_precision && arg_precision >= 0)
                {
                    precision = arg_precision;
                }
                if (arg_width <= precision)
                {
                    l = r = 0;
                }
                else
                {
                    if (Alignment)
                    {
                        // left alignment
                        l = 0;
                        r = arg_width - precision;
                        c0 = ' ';
                    }
                    else
                    {
                        // right alignment
                        if (c == 'D' && c0 == '0')
                        {
                        }
                        else
                        {
                            c0 = ' ';
                        }
                        l = arg_width - precision;
                        r = 0;
                    }
                }
                BufStrLen = precision;
                bFastStrCpy = true;
                break;
            case 'm':
                //if (c == 'm')
            {
                const unsigned char* ptr = va_arg(list, unsigned char*);
                int_ len = va_arg(list, int_);
                if (!ptr)
                {
                    S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                }
                else if (_is_bad_read_ptr(ptr, len))
                {
                    S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadPtrErr);
                }
                else
                {
                    CMaaConcatString_<xThrow, bCountMode> bf(Buffer, MAX_proc_s_LEN);
                    char tmp[64 + 8];
                    while (len > 0)
                    {
                        char* p = tmp;
                        int_ i;
                        for (i = 0; i < 16; i++)
                        {
                            unsigned char c = (i < len) ? ptr[i] : ' ';
                            c = (c >= ' ' && c != 0x7f) ? c : '.';
                            *p++ = (char)c;
                        }
                        *p++ = ' ';
                        //*p++ = ' ';
                        for (i = 0; i < 16 && i < len; i++)
                        {
                            //sprintf(p, " %02X", ptr[i]);
                            //p += strlen(p);
                            *p++ = ' ';
                            Byte2Hex_nzt_H(p, ptr[i]);
                            p += 2;
                        }
                        *p++ = '\n';
                        bf.Add(tmp, (int_)(p - tmp));
                        len -= 16;
                        ptr += 16;
                    }
    #if 0
                    if (len > 0)
                    {
                        char* p = tmp;
                        memcpy(p, (const char*)tmp1, tmp1.Length());
                        memcpy(p += tmp1.Length(), " of ", 4);
                        memcpy(p += 4, (const char*)tmp2, tmp2.Length());
                        memcpy(p += tmp2.Length(), " bytes are dumped\n", int_strlen(" bytes are dumped\n"));
                        p += int_strlen(" bytes are dumped\n");
                        bf.Add(tmp, (int_)(p - tmp));
                    }
    #endif
                    if constexpr (bCountMode)
                    {
                        bFastStrCpy = true;
                        pChar = nullptr;
                        BufStrLen = (int)bf.Length();
                        l = r = 0;
                        break;
                    }
                    else
                    {
                        S_String = (CMaaString)bf;
                    }
                }
            }
            CMaa_fallthrough;
            case 'T':
            case 't':
                if (c == 'T' || c == 't')
                {
                    const time_t t = va_arg(list, time_t);
                    const int_ mode = c == 'T' ? va_arg(list, int_) : 0;
                    S_String = GetTextDateTime(t, mode);
                }
                CMaa_fallthrough;
            case 0x100 + 's':
                if (c == 's')
                {
    #ifdef _WIN32_000
                    wchar_t* p = va_arg(list, wchar_t*);
                    int_ n = WideCharToMultiByte(CP_ACP, 0, p, -1, nullptr, 0, nullptr, nullptr);
                    CMaaString txt(nullptr, n);
                    n = WideCharToMultiByte(CP_ACP, 0, p, -1, (char*)(const char*)txt, txt.Length(), nullptr, nullptr);
                    if (n > 0 && txt[n - 1] == 0)
                    {
                        n--;
                    }
                    txt = txt.Left(n);
                    S_String = txt;
    #else
                    const wchar_t* p = va_arg(list, wchar_t*);
                    if (!p)
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                    }
                    else if (_is_bad_wstring_ptr(p, -1))
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_wstrptr_Err);
                    }
                    else
                    {
                        S_String = wchar_t_ToUtf8(p);
                    }
    #endif
                }
                CMaa_fallthrough;
            case 'U':
                if (c == 'U')
                {
                    const _WC_* p = va_arg(list, _WC_*);
                    if (!p)
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                    }
                    else if (_is_bad_wc2string_ptr(p, -1))
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_wc2ptr_Err);
                    }
                    else
                    {
                        S_String = UnicodeToUtf8(p);
                    }
                }
                CMaa_fallthrough;
            case 0x100 + 'c':
                if (c == 'c')
                {
                    const uint_ u = va_arg(list, uint_);
                    const wchar_t wch = (wchar_t)u;
                    S_String = wchar_t_ToUtf8(&wch, 1);
                }
                CMaa_fallthrough;
            case 'S':
            case 'M':
            {
                if (c == 'S' || c == 'M')
                {
                    const void* pS = va_arg(list, void*);
                    int_ nFirst = 0, nCount = 0;
                    if (c == 'M')
                    {
                        nFirst = va_arg(list, int_);
                        nCount = va_arg(list, int_);
                    }
                    if (!pS)
                    {
                        S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                    }
                    else
                    {
                        const CMaaString* s = (const CMaaString*)pS;
    #if TOOLSLIB_USE_CMAASTRING64 == 2
    #if TOOLSLIB_CMAASTRING64 == 0
    #ifndef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        int_ b = !s->m_pImp ? 1 : 0;
    #else
                        int_ b = !s->m_pImp ? 1 :
                            __is_bad_write_ptr((void*)&s->m_pImp, sizeof(void*)) ? 2 :
                            __is_bad_write_ptr((void*)&s->m_pImp->f2.m_Flags12, sizeof(s->m_pImp->f2.m_Flags12)) ? 2 : 0;
    #endif
                        const CMaaString64* s64 = nullptr;
                        switch (b)
                        {
                        case 0:
                            switch (s->GetImpVersion())
                            {
                            case CMAASTRFL_eImp32:
                                //S_String = *s;
                                break;
                            case CMAASTRFL_eImp64:
                                //S_String = *(const CMaaString64 *)s;
                                s64 = (const CMaaString64*)s;
                                s = nullptr;
                                break;
                            default:
                                s = nullptr;
                                S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_string_flag_Err);
                                //S_String.Empty();
                            }
                            break;
                        case 1:
                            S_String.Empty();
                            s = nullptr;
                            b = 0;
                        }
    #ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s64 && CMaaString64::IsBadStrPtr((CMaaString64*)s64))))
    #else
                        if (false)
    #endif
    #else
    #ifndef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        int_ b = !s->m_pImp ? 1 : 0;
    #else
                        int_ b = !s->m_pImp ? 1 :
                            __is_bad_write_ptr((void*)&s->m_pImp, sizeof(void*)) ? 2 :
                            __is_bad_write_ptr((void*)&s->m_pImp->f2.m_Flags12, sizeof(s->m_pImp->f2.m_Flags12)) ? 2 : 0;
    #endif
                        const CMaaString32* s32 = nullptr;
                        switch (b)
                        {
                        case 0:
                            switch (s->GetImpVersion())
                            {
                            case CMAASTRFL_eImp32:
                                //S_String = *(const CMaaString32*)s;
                                s32 = (const CMaaString32*)s;
                                s = nullptr;
                                break;
                            case CMAASTRFL_eImp64:
                                //S_String = *s;
                                break;
                            default:
                                s = nullptr;
                                S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_string_flag_Err);
                                //S_String.Empty();
                            }
                            break;
                        case 1:
                            S_String.Empty();
                            s = nullptr;
                            b = 0;
                        }
    #ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s32 && CMaaString32::IsBadStrPtr((CMaaString32*)s32))))
    #else
                        if (false)
    #endif
    #endif
    #else
    #ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        if (CMaaString::IsBadStrPtr((CMaaString*)s))
    #else

    #ifndef CMaaStringImp_NOT_NULL_MODE
                        if (false)
    #else
                        if (!s->m_pImp)
    #endif

    #endif
    #endif
    #ifdef TOOLSLIB_SPRINTF_CHECK_STRINGS
                        {
    #ifdef _WIN32
                            FILE* f = fopen("c:\\maa\\BadStrPtr.txt", "a+b");
    #else
                            FILE* f = fopen("/home/maa/mylogs/BadStrPtr.txt", "a+b");
    #endif
                            if (f)
                            {
                                const time_t t = time(nullptr);
                                tm t0;
                                const tm* p = localtime(&t);
                                if (p)
                                {
                                    memcpy(&t0, p, sizeof(t0));
                                }
                                else
                                {
                                    memset(&t0, 0, sizeof(t0));
                                }
                                fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: BadStrPtr#1: %s:%d '", t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec, SrcFile, SrcLine);
                                fwrite(strFormat, 1, FormatLen, f);
                                fwrite("' -- '", 1, 6, f);
                                fwrite(strText, 1, TextLen, f);
                                fwrite("'\r\n", 1, 3, f);
                                fclose(f);
                            }
                            S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBad_string_ptr_Err);
                        }
    #else
                        {
                        }
    #endif
                        else
                        {
                            if (s)
                            {
                                S_String = *s;
                            }
    #if TOOLSLIB_USE_CMAASTRING64 == 2
    #if TOOLSLIB_CMAASTRING64 == 0
                            else if (s64)
                            {
                                S_String = *s64;
                            }
    #else
                            else if (s32)
                            {
                                S_String = *s32;
                            }
    #endif
    #endif
                            if (c == 'M')
                            {
                                S_String = S_String.RefMid(nFirst, nCount);
                            }
                        }
                    }
                }

                //if (c != 'm')
                if (arg_precision >= 0 || arg_width > 0)
                {
                    precision = (warning_int)S_String.Utf8CharCount();

                    if (arg_precision >= 0 && arg_precision < precision)
                    {
                        if (Alignment || arg_width < 0)
                        {
                            // left alignment
                            S_String = S_String.RefLeft(S_String.Utf8Pos(arg_precision));
                        }
                        else
                        {
                            // right alignment
                            S_String = S_String.RefMid(S_String.Utf8Pos(precision - arg_precision));
                        }
                        precision = (warning_int)S_String.Utf8CharCount();
                    }
                    if (arg_width > precision)
                    {
                        if (Alignment)
                        {
                            // left alignment
                            l = 0;
                            r = arg_width - precision;
                        }
                        else
                        {
                            // right alignment
                            l = arg_width - precision;
                            r = 0;
                        }
                        c0 = ' ';
                        bFastStrCpy = true;
                    }
                }
                c = 'S';
            }
            break;
            case 'd':
            case 'i':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                if (Float == 3)
                {
                    const unsigned long long x = va_arg(list, unsigned long long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    //break;
                    BufStrLen =
                        c == 'd' || c == 'i' ? SignedIntegerToString_d<long long>((long long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ? UnsignedIntegerToString_d<unsigned long long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'x' || c == 'X' ? UnsignedIntegerToString_x<unsigned long long>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(x) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                        UnsignedIntegerToString_o<unsigned long long>(x, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                    break;
                }
                if (Float == 2)
                {
                    const unsigned long x = va_arg(list, unsigned long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    //break;
                    BufStrLen =
                        c == 'd' || c == 'i' ? SignedIntegerToString_d<long>((long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ? UnsignedIntegerToString_d<unsigned long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'x' || c == 'X' ? UnsignedIntegerToString_x<unsigned long>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(x) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                        UnsignedIntegerToString_o<unsigned long>(x, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                    break;
                }
                // Float:
                // case 0:
                // unsigned short / short as int
                // case -1:
                // unsigned char x / char as int
                //default:

                //Int = va_arg(list, int_);
                //CMaa_fallthrough;

                Int = va_arg(list, int_);
                BufStrLen =
                    c == 'd' || c == 'i' ? SignedIntegerToString_d<int_>(Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                    c == 'u' ? UnsignedIntegerToString_d<uint_>((uint_)Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                    c == 'x' || c == 'X' ? UnsignedIntegerToString_x<uint_>((uint_)Int, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(uint_) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                    UnsignedIntegerToString_o<uint_>((uint_)Int, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                break;
            case 'w':
            case 'W':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                if (Float == 3)
                {
                    const _uqword x = va_arg(list, _uqword);
                    BufStrLen = UnsignedIntegerToString_x<_uqword>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(_uqword) : arg_width, c == 'w' ? 'x' : 'X', c0 == '0' ? '0' : 0, prefix_ch ? c : 0);
                    break;
                }
                if (Float == 2)
                {
                    const _dword x = va_arg(list, _dword);
                    BufStrLen = UnsignedIntegerToString_x<_dword>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(_dword) : arg_width, c == 'w' ? 'x' : 'X', c0 == '0' ? '0' : 0, prefix_ch ? c : 0);
                    break;
                }
                Int = va_arg(list, int_);
                BufStrLen = UnsignedIntegerToString_x<_word>((_word)Int, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(_word) : arg_width, c == 'w' ? 'x' : 'X', c0 == '0' ? '0' : 0, prefix_ch ? c : 0);
                break;
            case 'p':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                pVoid = va_arg(list, void*);
                xSz = (size_t)pVoid;
                BufStrLen = UnsignedIntegerToString_p<size_t>(xSz, Buffer, arg_width, 'X', prefix_ch ? 'x' : 0);
                break;
            case 'e':
            case 'E':
            case 'f':
            case 'g':
            case 'G':
                switch (Float)
                {
                case 3:
                    xld = va_arg(list, long double);
                    TOOLSLIB_SPRINTF_OUTPUT(xld);
                    break;
                case 2:
                    xd = va_arg(list, double);
                    TOOLSLIB_SPRINTF_OUTPUT(xd);
                    break;
                default: // 1
                    xf = (float)va_arg(list, double); // bc31&vc60 feature: they push double to stack instead of float arguments
                    TOOLSLIB_SPRINTF_OUTPUT(xf);
                }
                break;
            case 'z':
                arg_width = width == -2 || sign_ch != '-' ? arg_width : -arg_width;
                arg_width = (arg_width <= 0 || arg_width <= MAX_proc_s_LEN) ? arg_width : MAX_proc_s_LEN;
                arg_width = (arg_width >= 0 || arg_width >= -MAX_proc_s_LEN) ? arg_width : -MAX_proc_s_LEN;
                sign_ch = sign_ch != '-' ? sign_ch : 0;
                xSz = va_arg(list, size_t);
                BufStrLen = Format[i - 1] == 'x' || Format[i - 1] == 'X' ?
                    UnsignedIntegerToString_x<size_t>(xSz, Buffer, arg_width, Format[i - 1], c0 == '0' ? '0' : 0, prefix_ch ? 'x' : 0) :
                    UnsignedIntegerToString_d<size_t>(xSz, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0);
                break;
                //case 'p':
                //    pVoid = va_arg(list, void*);
                //    TOOLSLIB_SPRINTF_OUTPUT(pVoid);
                //    break;
                //case 'z':
                //    xSz = va_arg(list, size_t);
                //    TOOLSLIB_SPRINTF_OUTPUT(xSz);
                //    break;
            case 'l':
                Buffer[BufStrLen = 0] = 0;
                //strcpy(Buffer, Format + Start);
                /*
                if (BackUp == 'u')
                {
                    Format[i] = BackUp;
                    BackUp = Format[++i];
                    Format[i] = 0;
                    unsigned long long x = va_arg(list, unsigned long long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    Buffer[0] = 0;
                }
                else
                {
                    long long x = va_arg(list, long long);
                    //TOOLSLIB_SPRINTF_OUTPUT(x);
                    Buffer[0] = 0;
                }
                */
                break;
            case 'n':
            {
                int_* p = va_arg(list, int_*);
                *p = 0;
                Buffer[BufStrLen = 0] = 0;
            }
            break;
            default:
                TOOLSLIB_SPRINTF_OUTPUT(Int);
            }
            Format[i] = BackUp;
            //*this += CMaaString ( Buffer );
            if (c != 'l')
            {
                if (c == 'S')
                {
                    ReplText[N].s = S_String;
                    if (bFastStrCpy)
                    {
                        ReplText[N].l = l;
                        ReplText[N].r = r;
                        ReplText[N].c0 = c0;
                    }
                    else
                    {
                        ReplText[N].l = ReplText[N].r = 0;
                    }
                }
                else
                {
                    if (bFastStrCpy)
                    {
                        ReplText[N].s = CMaaString(pChar, BufStrLen);
                        ReplText[N].l = l;
                        ReplText[N].r = r;
                        ReplText[N].c0 = c0;
                    }
                    else
                    {
                        ReplText[N].s = CMaaString(Buffer, BufStrLen >= 0 ? BufStrLen : int_strlen(Buffer));
                        ReplText[N].l = ReplText[N].r = 0;
                    }
                }
                NewStringApproxLen += ReplText[N].s.Length() + ReplText[N].l + ReplText[N].r;
                N++;
            }
            Start = i;
        }

        {
            //SetNewLengthValue(Length0); // Empty();
            Required(NewStringApproxLen); // +100);
            int j = 0;
            const char* p = strText;
            for (int i = 0; i < TextLen; )
            {
                if (p[i] != '%')
                {
                    i++;
                    continue;
                }
                //p[i] = 0;
                Add(p + j, i - j);
                i++;
                if (i < TextLen && p[i] >= '0' && p[i] <= '9')
                {
                    int_ n = p[i++] - '0';
                    while (i < TextLen && p[i] >= '0' && p[i] <= '9')
                    {
                        n = n * 10 + (p[i++] - '0');
                    }
                    n--;
                    if (n >= 0 && n < N)
                    {
                        Add(ReplText[n].c0, ReplText[n].l);
                        *this += ReplText[n].s;
                        Add(ReplText[n].c0, ReplText[n].r);
                    }
                }
                else
                {
                    *this += '%';
                    if (i < TextLen && p[i] == '%')
                    {
                        i++;
                    }
                }
                j = i;
            }
            Add(p + j, TextLen - j);
        }
        if (!IsValid())
        {
            m_bIsValid = true;
            m_StringLength = Length0;
        }
    }
    catch (...)
    {
        if (!IsValid())
        {
            m_bIsValid = true;
            m_StringLength = Length0;
        }
        throw;
    }
}
//---------------------------------------------------------------------------
