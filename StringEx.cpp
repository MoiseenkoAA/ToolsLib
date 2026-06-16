
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/StringEx.cpp

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

#include "perm.h"
#include "temp.h"

 //#define m_Flags12_ m_Flags12
//#define f12_ f12

#if TOOLSLIB_USE_CMAASTRING64 == 1
#define int _qword
#define int_strlen int64_strlen
#define TOOLSLIB_CMAASTRING64 1
typedef int StrInt;
#endif

#if TOOLSLIB_USE_CMAASTRING64 == 0
typedef int StrInt;
#define TOOLSLIB_CMAASTRING64 0
#endif

#if TOOLSLIB_USE_CMAASTRING64 == 2
typedef int_ StrInt;
#ifndef TOOLSLIB_CMAASTRING64
#define TOOLSLIB_CMAASTRING64 0
#endif
#endif

#define tmp_my_off_x(x) (int)((_qword)(char *)&(((CMaaString::CMaaStringImp *)nullptr)->x)) // linux warning off

#ifdef CMAASTRING_USE_UNALLOC_BUFFER
#if defined(CMAASTRING_HAS_ADD_DATA) && (defined(_WIN64) || defined(__unix__))
#define CMAASTRING_MAX_UNALLOC_LENGTH_WITH_PADDING (int)(sizeof(m_iCapacity) + sizeof(m_pRefferedTo) + CMAASTRING_CACHE_HASH64_SIZE + sizeof(m_Padding4)) //(int)(tmp_my_off_x(m_Flags) - tmp_my_off_x(m_pRefferedTo))
#else
#define CMAASTRING_MAX_UNALLOC_LENGTH_WITH_PADDING (int)(tmp_my_off_x(m_pRefferedTo) - tmp_my_off_x(m_iCapacity) + sizeof(m_pRefferedTo) + CMAASTRING_CACHE_HASH64_SIZE)
#endif
#define CMAASTRING_MAX_UNALLOC_LENGTH (CMAASTRING_MAX_UNALLOC_LENGTH_WITH_PADDING - 4)
#define CMAASTRING_UNALLOC_PTR (char *)&m_iCapacity
#define CMAASTRING_UNALLOC_PTR_(x) (char *)&(x)->m_iCapacity
#define ZeroUnallocTail0 memset(m_pszStr, 0, CMAASTRING_MAX_UNALLOC_LENGTH_WITH_PADDING)
#else
#define CMAASTRING_MAX_UNALLOC_LENGTH_WITH_PADDING 0
#define CMAASTRING_MAX_UNALLOC_LENGTH 0
//#define ZeroUnallocTail(pos)
//#define ZeroUnallocTail0
#endif

#ifdef TOOLSLIB_STAT_STRING_IMP
#if TOOLSLIB_CMAASTRING64 <= 1
#ifdef TOOLSLIB_SINGLE_THREAD
int_ gCMaaStringImpStatShort = 0, gCMaaStringImpStatLong = 0, gCMaaStringImpStatRef = 0, gCMaaStringImpStatMem = 0, gCMaaStringImpStatRefs = 0;
#else
std::atomic<int_> gCMaaStringImpStatShort = 0, gCMaaStringImpStatLong = 0, gCMaaStringImpStatRef = 0, gCMaaStringImpStatMem = 0, gCMaaStringImpStatRefs = 0;
#endif
#endif
#define INC_TOOLSLIB_STAT_STRING_IMP_SHORT ++gCMaaStringImpStatShort
#define DEC_TOOLSLIB_STAT_STRING_IMP_SHORT --gCMaaStringImpStatShort
#define INC_TOOLSLIB_STAT_STRING_IMP_LONG ++gCMaaStringImpStatLong
#define DEC_TOOLSLIB_STAT_STRING_IMP_LONG --gCMaaStringImpStatLong
#define INC_TOOLSLIB_STAT_STRING_IMP_REF ++gCMaaStringImpStatRef
#define DEC_TOOLSLIB_STAT_STRING_IMP_REF --gCMaaStringImpStatRef
#define INC_TOOLSLIB_STAT_STRING_IMP_MEM ++gCMaaStringImpStatMem
#define DEC_TOOLSLIB_STAT_STRING_IMP_MEM --gCMaaStringImpStatMem
#else
#define INC_TOOLSLIB_STAT_STRING_IMP
#define DEC_TOOLSLIB_STAT_STRING_IMP
#define INC_TOOLSLIB_STAT_STRING_IMP_SHORT
#define DEC_TOOLSLIB_STAT_STRING_IMP_SHORT
#define INC_TOOLSLIB_STAT_STRING_IMP_LONG
#define DEC_TOOLSLIB_STAT_STRING_IMP_LONG
#define INC_TOOLSLIB_STAT_STRING_IMP_REF
#define DEC_TOOLSLIB_STAT_STRING_IMP_REF
#define INC_TOOLSLIB_STAT_STRING_IMP_MEM
#define DEC_TOOLSLIB_STAT_STRING_IMP_MEM
#endif

int CMaaString::GetMaxUnAllocLength() noexcept
{
    return CMaaStringImp::GetMaxUnAllocLength();
}
int CMaaString::CMaaStringImp::GetMaxUnAllocLength() noexcept
{
    return CMAASTRING_MAX_UNALLOC_LENGTH;
}
int CMaaString::GetMaxUnAllocLengthWithPadding() noexcept
{
    return CMaaStringImp::GetMaxUnAllocLengthWithPadding();
}
int CMaaString::CMaaStringImp::GetMaxUnAllocLengthWithPadding() noexcept
{
    return CMAASTRING_MAX_UNALLOC_LENGTH_WITH_PADDING;
}
bool CMaaString::CMaaStringImp::IsUnAllocString() const noexcept
{
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
    if (f2.f12.m_Flags0 & eHasReffered)
    {
#if TOOLSLIB_USE_CMAASTRING64 == 2
        if ((m_pRefferedTo->f2.f12.m_Flags0 & eImpMask) == eImp32)
        {
            CMaaString32::CMaaStringImp* p = (CMaaString32::CMaaStringImp*)m_pRefferedTo;
            return p->m_pszStr == CMAASTRING_UNALLOC_PTR_(p);
        }
        else
        {
            CMaaString64::CMaaStringImp* p = (CMaaString64::CMaaStringImp*)m_pRefferedTo;
            return p->m_pszStr == CMAASTRING_UNALLOC_PTR_(p);
        }
#else
        return m_pRefferedTo->m_pszStr == CMAASTRING_UNALLOC_PTR_(m_pRefferedTo);
#endif
    }
    return m_pszStr == CMAASTRING_UNALLOC_PTR;
#else
    return false;
#endif
}
bool CMaaString::CMaaStringImp::CanSetAddRefData(CMaaStringImp* pImp, int Idx, bool bNeedSingleOwner) const noexcept
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA
    if (pImp && pImp->HasReffered() && this == pImp->m_pRefferedTo)
    {
        return false;
    }
    if (Idx && pImp == this)
    {
        return false;
    }
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
    return (!bNeedSingleOwner || IsSingleOwner());
#else
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
    return (!bNeedSingleOwner || IsSingleOwner()) && !HasReffered() && m_pszStr != CMAASTRING_UNALLOC_PTR;
#else
    return (!bNeedSingleOwner || IsSingleOwner()) && !HasReffered();
#endif
#endif
#else
    return false;
#endif
}

#if 0
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX       2147483647
#define UINT_MAX      0xffffffff
#define LLONG_MAX     9223372036854775807i64
#define LLONG_MIN   (-9223372036854775807i64 - 1)
#define ULLONG_MAX    0xffffffffffffffffui64
#endif

//#define GLOCK_LIB2 gLock_lib2

//int gggg___aa = 0;

#ifdef _WIN32__
#define CHECK_POINTERS 2 // 0 - no, 1 - check, 2 - check & log
#else
#define CHECK_POINTERS 0 // 0 - no, 1 - check, 2 - check & log
#endif

#if CHECK_POINTERS != 0
#define CHECK_STRINGS // check '%S'
#endif

//#define UNSAFE_GetLine0_OPTS
//#define UNSAFE_GetWord0_OPTS


#if TOOLSLIB_CMAASTRING64 <= 1
//constexpr CMaa256Bits gSpacesBits{ " \t", 2, false };
//const CMaa256Bits gSpacesBitsComma{ ",", 1 };
//const CMaa256Bits gSpacesBitsColon{ ":", 1 };
//const CMaa256Bits gSpacesBitsSemicolon{ ";", 1 };
//const CMaa256Bits gSpacesBitsSpace{ " ", 1 };
//const CMaa256Bits gSpacesBitsDot{ ".", 1 };

#ifndef TOOLSLIB_SINGLE_THREAD
void gCMaaSyncAssignLockUpperLock(bool bInit) noexcept
{
    __GLock__SyncAssignLockUpperLock(bInit)->Lock();
}
void gCMaaSyncAssignLockUpperLock() noexcept
{
    __GLock__SyncAssignLockUpperLock()->Lock();
}
void gCMaaSyncAssignLockUpperUnLock() noexcept
{
    __GLock__SyncAssignLockUpperLock()->UnLock();
}
#endif
#endif

//#define CMaaSyncAssign AssignImpFast
////#define CMaaSyncAssign CMaaSyncAssignFast

//------------------------------------------------------------------------------
static inline const char* dsscanf_skip_s_whitesp(const char* s, int_ &len) noexcept
{
    if (len >= 0)
    {
        while (len)
        {
            const char c = *s;
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                s++;
                len--;
                continue;
            }
            break;
        }
    }
    else
    {
        while (1)
        {
            const char c = *s;
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                s++;
                continue;
            }
            break;
        }
    }
    return s;
}

static inline const char* dsscanf_skip_s_not_whitesp(const char* s, int_& len) noexcept
{
    if (len >= 0)
    {
        while (len)
        {
            const char c = *s;
            if (!(c == ' ' || c == '\t' || c == '\n' || c == '\r'))
            {
                s++;
                len--;
                continue;
            }
            break;
        }
    }
    else
    {
        while (1)
        {
            const char c = *s;
            if (!(c == ' ' || c == '\t' || c == '\n' || c == '\r' || !c))
            {
                s++;
                continue;
            }
            break;
        }
    }
    return s;
}

static inline const char* dsscanf_skip_s_not_whitesp_or_number(const char* s, int_& len) noexcept
{
    if (len >= 0)
    {
        while (len)
        {
            const char c = *s;
            if (!(c == ' ' || c == '\t' || c == '\n' || c == '\r' || (c >= '0' && c <= '9') || c == '-' || c == '+'))
            {
                s++;
                len--;
                continue;
            }
            break;
        }
    }
    else
    {
        while (1)
        {
            const char c = *s;
            if (!(c == ' ' || c == '\t' || c == '\n' || c == '\r' || (c >= '0' && c <= '9') || c == '-' || c == '+' || !c))
            {
                s++;
                continue;
            }
            break;
        }
    }
    return s;
}

template<typename T> T dsscanf_get_int(const char *&s, int_ &len, char c0, char c) noexcept
{
    T x = '0' - c;
    s++;
    if (len >= 0)
    {
        len--;
        while (len)
        {
            c = *s;
            if (c < '0' || c > '9')
            {
                break;
            }
            x = x * 10 - (c - '0');
            s++;
            len--;
        }
    }
    else
    {
        while (1)
        {
            c = *s;
            if (c < '0' || c > '9')
            {
                break;
            }
            x = x * 10 - (c - '0');
            s++;
        }
    }
    return c0 != '-' ? -x : x;
}

template<typename T> T dsscanf_get_uint(const char *&s, int_ &len, char c) noexcept
{
    T x = c - '0';
    s++;
    if (len >= 0)
    {
        len--;
        while (len)
        {
            c = *s;
            if (c < '0' || c > '9')
            {
                break;
            }
            x = x * 10 + (c - '0');
            s++;
            len--;
        }
    }
    else
    {
        while (1)
        {
            c = *s;
            if (c < '0' || c > '9')
            {
                break;
            }
            x = x * 10 + (c - '0');
            s++;
        }
    }
    return x;
}

//#define USE_dsscanf_get_uint // Excessive

template<typename T> T dsscanf_get_hex(const char*& s, int_& len, char c0, char c) noexcept
{
    T x;
    int_ Flags = 0;
    if ((c >= '0' && c <= '9'))
    {
        x = c - '0';
    }
    else if ((c >= 'a' && c <= 'f'))
    {
        x = 10 + c - 'a';
        Flags = 2;
    }
    else //if ((c >= 'A' && c <= 'F'))
    {
        x = 10 + c - 'A';
        Flags = 1;
    }
    s++;
    if (len >= 0)
    {
        len--;
        while (len)
        {
            c = *s;
            if ((c >= '0' && c <= '9'))
            {
                x = (x << 4) | (c - '0');
            }
            else if ((c >= 'a' && c <= 'f') && !(Flags & 1))
            {
                x = (x << 4) | (10 + c - 'a');
                Flags = 2;
            }
            else if ((c >= 'A' && c <= 'F') && !(Flags & 2))
            {
                x = (x << 4) | (10 + c - 'A');
                Flags = 1;
            }
            else
            {
                break;
            }
            s++;
            len--;
        }
    }
    else
    {
        while (1)
        {
            c = *s;
            if ((c >= '0' && c <= '9'))
            {
                x = (x << 4) | (c - '0');
            }
            else if ((c >= 'a' && c <= 'f') && !(Flags & 1))
            {
                x = (x << 4) | (10 + c - 'a');
                Flags = 2;
            }
            else if ((c >= 'A' && c <= 'F') && !(Flags & 2))
            {
                x = (x << 4) | (10 + c - 'A');
                Flags = 1;
            }
            else
            {
                break;
            }
            s++;
        }
    }
    return c0 == '-' ? -x : x;
}

template<typename T> T dsscanf_get_oct(const char*& s, int_& len, char c0, char c) noexcept
{
    T x = c - '0';
    s++;
    if (len >= 0)
    {
        len--;
        while (len)
        {
            c = *s;
            if (c < '0' || c > '7')
            {
                break;
            }
            x = (x << 3) | (c - '0');
            s++;
            len--;
        }
    }
    else
    {
        while (1)
        {
            c = *s;
            if (c < '0' || c > '7')
            {
                break;
            }
            x = (x << 3) | (c - '0');
            s++;
        }
    }
    return c0 == '-' ? -x : x;
}

// sscanf with digital format like %[l][l]d/%[l][l]u/%[l][l]x/%[l][l]o/%X/%O/%[l][l]f/%[l][l]g/%D/%n/%N/%c/%-0x%[l][l]w/%[l][l]W

static int_ dsscanfV(const CMaaString* pStr, const char* s, int_ len, const char* fmt, va_list list) noexcept
{
    if (!s)
    {
        return 0;
    }
    const char* s0 = s;
    int_ n = 0;
    bool bSp = true;
    char c0 = '+';
    while (1)
    {
        char c = *fmt;
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            fmt++;
            bSp = true;
            continue;
        }
        if (!c)
        {
            break;
        }
        if (c == '%' && fmt[1] == 'n')
        {
            fmt += 2;
            int_* p = va_arg(list, int_*);
            *p = (int_)(s - s0);
            n++;
            continue;
        }
        if (bSp)
        {
            bSp = false;
            s = dsscanf_skip_s_whitesp(s, len);
        }
        if (!len || (len < 0 && !*s))
        {
            if (c == '%' && fmt[1] == 'N')
            {
                int_* p = va_arg(list, int_*);
                *p = (int_)(s - s0);
                n++;
            }
            break;
        }
        if (c == '%')
        {
            fmt++;
            c = *fmt;
            if (c == 'd' || c == 'u')
            {
#ifdef USE_dsscanf_get_uint
                const char f0 = c;
#endif
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                const char c0 = *s;
                if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                {
                    c = s[1];
                    s++;
                    len = len > 0 ? len - 1 : len;
                }
                else
                {
                    c = c0;
                }
                if (c >= '0' && c <= '9')
                {
                    int_* p = va_arg(list, int_*);
#ifdef USE_dsscanf_get_uint
                    * p = f0 == 'd' ? dsscanf_get_int<int_>(s, len, c0, c) : (int_)dsscanf_get_uint<uint_>(s, len, c);
#else
                    * p = dsscanf_get_int<int_>(s, len, c0, c);
#endif
                    n++;
                    bSp = true;
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (c == 'l')
            {
                const char f1 = fmt[1];
                if (f1 == 'l')
                {
                    const char f2 = fmt[2];
                    if (f2 == 'd' || f2 == 'u')
                    {
                        c = f2;
                        fmt += 3;
                        s = dsscanf_skip_s_whitesp(s, len);
                        if (!len || (len < 0 && !*s))
                        {
                            break;
                        }
                        const char c0 = *s;
                        if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                        {
                            c = s[1];
                            s++;
                            len = len > 0 ? len - 1 : len;
                        }
                        else
                        {
                            c = c0;
                        }
                        if (c >= '0' && c <= '9')
                        {
                            long long* p = va_arg(list, long long*);
#ifdef USE_dsscanf_get_uint
                            * p = f2 == 'd' ? dsscanf_get_int<long long>(s, len, c0, c) : (long long)dsscanf_get_uint<unsigned long long>(s, len, c);
#else
                            * p = dsscanf_get_int<long long>(s, len, c0, c);
#endif
                            n++;
                            bSp = true;
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if (f2 == 'f' || f2 == 'g')
                    {
                        fmt += 3;
                        s = dsscanf_skip_s_whitesp(s, len);
                        if (!len || (len < 0 && !*s))
                        {
                            break;
                        }
                        long double x = 0.0;
                        int_ pos;
                        if (!mysscanf_long_double(s, &x, len, &pos))
                        {
                            break;
                        }
                        long double* p = va_arg(list, long double*);
                        *p = x;
                        s += pos;
                        len = len > 0 ? len - pos : len;
                        n++;
                        bSp = true;
                        continue;
                    }
                    else if (f2 == 'w' || f2 == 'W')
                    {
                        fmt += 3;
                        s = dsscanf_skip_s_whitesp(s, len);
                        if (!len || (len < 0 && !*s))
                        {
                            break;
                        }
                        /*
                        const char c0 = *s;
                        if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                        {
                            c = s[1];
                            s++;
                            len = len > 0 ? len - 1 : len;
                        }
                        else
                        {
                            c = c0;
                        }
                        */
                        c = *s;
                        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                        {
                            _qword* p = va_arg(list, _qword*);
                            *p = dsscanf_get_hex<_qword>(s, len, c0, c);
                            c0 = '+';
                            n++;
                            bSp = true;
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if (f2 == 'x' || f2 == 'X')
                    {
                        fmt += 3;
                        s = dsscanf_skip_s_whitesp(s, len);
                        if (!len || (len < 0 && !*s))
                        {
                            break;
                        }
                        /*
                        const char c0 = *s;
                        if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                        {
                            c = s[1];
                            s++;
                            len = len > 0 ? len - 1 : len;
                        }
                        else
                        {
                            c = c0;
                        }
                        */
                        c = *s;
                        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                        {
                            long long* p = va_arg(list, long long*);
                            *p = dsscanf_get_hex<long long>(s, len, c0, c);
                            c0 = '+';
                            n++;
                            bSp = true;
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else if (f2 == 'o' || f2 == 'O')
                    {
                        fmt += 3;
                        s = dsscanf_skip_s_whitesp(s, len);
                        if (!len || (len < 0 && !*s))
                        {
                            break;
                        }
                        /*
                        const char c0 = *s;
                        if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                        {
                            c = s[1];
                            s++;
                            len = len > 0 ? len - 1 : len;
                        }
                        else
                        {
                            c = c0;
                        }
                        */
                        c = *s;
                        if (c >= '0' && c <= '7')
                        {
                            long long* p = va_arg(list, long long*);
                            *p = dsscanf_get_oct<long long>(s, len, c0, c);
                            c0 = '+';
                            n++;
                            bSp = true;
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if (f1 == 'f' || f1 == 'g')
                {
                    fmt += 2;
                    s = dsscanf_skip_s_whitesp(s, len);
                    if (!len || (len < 0 && !*s))
                    {
                        break;
                    }
                    double x = 0.0;
                    int_ pos;
                    if (!mysscanf_double(s, &x, len, &pos))
                    {
                        break;
                    }
                    double* p = va_arg(list, double*);
                    *p = x;
                    s += pos;
                    len = len > 0 ? len - pos : len;
                    n++;
                    bSp = true;
                    continue;
                }
                else if (f1 == 'd' || f1 == 'u')
                {
                    c = f1;
                    fmt += 2;
                    s = dsscanf_skip_s_whitesp(s, len);
                    if (!len || (len < 0 && !*s))
                    {
                        break;
                    }
                    const char c0 = *s;
                    if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                    {
                        c = s[1];
                        s++;
                        len = len > 0 ? len - 1 : len;
                    }
                    else
                    {
                        c = c0;
                    }
                    if (c >= '0' && c <= '9')
                    {
                        long* p = va_arg(list, long*);
#ifdef USE_dsscanf_get_uint
                        * p = f1 == 'd' ? dsscanf_get_int<long>(s, len, c0, c) : (long)dsscanf_get_uint<unsigned long>(s, len, c);
#else
                        * p = dsscanf_get_int<long>(s, len, c0, c);
#endif
                        n++;
                        bSp = true;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (f1 == 'w' || f1 == 'W')
                {
                    fmt += 2;
                    s = dsscanf_skip_s_whitesp(s, len);
                    if (!len || (len < 0 && !*s))
                    {
                        break;
                    }
                    /*
                    const char c0 = *s;
                    if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                    {
                        c = s[1];
                        s++;
                        len = len > 0 ? len - 1 : len;
                    }
                    else
                    {
                        c = c0;
                    }
                    */
                    c = *s;
                    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                    {
                        _sdword* p = va_arg(list, _sdword*);
                        *p = dsscanf_get_hex<_sdword>(s, len, c0, c);
                        c0 = '+';
                        n++;
                        bSp = true;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (f1 == 'x' || f1 == 'X')
                {
                    fmt += 2;
                    s = dsscanf_skip_s_whitesp(s, len);
                    if (!len || (len < 0 && !*s))
                    {
                        break;
                    }
                    /*
                    const char c0 = *s;
                    if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                    {
                        c = s[1];
                        s++;
                        len = len > 0 ? len - 1 : len;
                    }
                    else
                    {
                        c = c0;
                    }
                    */
                    c = *s;
                    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                    {
                        long* p = va_arg(list, long*);
                        *p = dsscanf_get_hex<long>(s, len, c0, c);
                        c0 = '+';
                        n++;
                        bSp = true;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (f1 == 'o' || f1 == 'O')
                {
                    fmt += 2;
                    s = dsscanf_skip_s_whitesp(s, len);
                    if (!len || (len < 0 && !*s))
                    {
                        break;
                    }
                    /*
                    const char c0 = *s;
                    if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                    {
                        c = s[1];
                        s++;
                        len = len > 0 ? len - 1 : len;
                    }
                    else
                    {
                        c = c0;
                    }
                    */
                    c = *s;
                    if (c >= '0' && c <= '7')
                    {
                        long* p = va_arg(list, long*);
                        *p = dsscanf_get_oct<long>(s, len, c0, c);
                        c0 = '+';
                        n++;
                        bSp = true;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else if (c == 'f' || c == 'g')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                float x = 0.0f;
                int_ pos;
                if (!mysscanf_float(s, &x, len, &pos))
                {
                    break;
                }
                float* p = va_arg(list, float*);
                *p = x;
                s += pos;
                len = len > 0 ? len - pos : len;
                n++;
                bSp = true;
                continue;
            }
            else if (c == 'D')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                const char c0 = *s;
                if (c0 == '-' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                {
                    c = s[1];
                    s++;
                    len = len > 0 ? len - 1 : len;
                }
                else
                {
                    c = c0;
                }
                if (c >= '0' && c <= '9')
                {
                    __int64* p = va_arg(list, __int64*);
                    *p = dsscanf_get_int<__int64>(s, len, c0, c);
                    n++;
                    bSp = true;
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (c == 'N')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                int_* p = va_arg(list, int_*);
                *p = (int_)(s - s0);
                n++;
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                continue;
            }
            else if (c == 'x' || c == 'X')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                /*
                const char c0 = *s;
                if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                {
                    c = s[1];
                    s++;
                    len = len > 0 ? len - 1 : len;
                }
                else
                {
                    c = c0;
                }
                */
                c = *s;
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                {
                    int_* p = va_arg(list, int_*);
                    *p = dsscanf_get_hex<int_>(s, len, c0, c);
                    c0 = '+';
                    n++;
                    bSp = true;
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (c == 'p')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                c = *s;
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                {
                    char** p = va_arg(list, char**);
                    *p = (char *)dsscanf_get_hex<_qword>(s, len, c0, c);
                    c0 = '+';
                    n++;
                    bSp = true;
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (c == 'w' || c == 'W')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                /*
                const char c0 = *s;
                if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                {
                    c = s[1];
                    s++;
                    len = len > 0 ? len - 1 : len;
                }
                else
                {
                    c = c0;
                }
                */
                c = *s;
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                {
                    _sword* p = va_arg(list, _sword*);
                    *p = dsscanf_get_hex<_sword>(s, len, c0, c);
                    c0 = '+';
                    n++;
                    bSp = true;
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (c == 'o' || c == 'O')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                /*
                const char c0 = *s;
                if (c0 == '-' && c == 'd' && (len < 0 || len > 1) && s[1] >= '0' && s[1] <= '9')
                {
                    c = s[1];
                    s++;
                    len = len > 0 ? len - 1 : len;
                }
                else
                {
                    c = c0;
                }
                */
                c = *s;
                if (c >= '0' && c <= '7')
                {
                    int_* p = va_arg(list, int_*);
                    *p = dsscanf_get_oct<int_>(s, len, c0, c);
                    c0 = '+';
                    n++;
                    bSp = true;
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (c == 'c')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                char* p = va_arg(list, char*);
                *p = *s;
                s++;
                len = len > 0 ? len - 1 : len;
                n++;
                bSp = true;
                continue;
            }
            else if (c == '-' /*|| c == '+'*/)
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                if (*s == '+' || *s == '-')
                {
                    c0 = *s;
                    s++;
                    len = len > 0 ? len - 1 : len;
                    bSp = true;
                }
                else
                {
                    c0 = '+';
                }
                //n++;
                continue;
            }
            else if (c == 'S' || c == 's')
            {
                fmt++;
                s = dsscanf_skip_s_whitesp(s, len);
                if (!len || (len < 0 && !*s))
                {
                    break;
                }
                const char* tmp = s;
                s = c == 'S' ? dsscanf_skip_s_not_whitesp(s, len) : dsscanf_skip_s_not_whitesp_or_number(s, len);

                CMaaString* p = va_arg(list, CMaaString*);
                if (pStr)
                {
                    *p = pStr->RefMid((int)(tmp - s0), (int)(s - tmp));
                }
                else
                {
                    try
                    {
                        *p = CMaaString(tmp, (int)(s - tmp));
                    }
                    catch (...)
                    {
                        break;
                    }
                }
                n++;
                bSp = true;
                continue;
            }
            if (c != '%')
            {
                fmt--;
            }
        }
        if (*s++ != *fmt++)
        {
            break;
        }
        len = len > 0 ? len - 1 : len;
    }
    return n;
}

int_ dsscanfV3264(const char* s, int_ len, const char* fmt, va_list list) noexcept
{
    return dsscanfV(nullptr, s, len, fmt, list);
}

int_ dsscanf3264(const char* s, int_ len, const char* fmt, ...) noexcept
{
    va_list list;
    va_start(list, fmt);
    int_ n = dsscanfV(nullptr, s, len, fmt, list);
    va_end(list);
    return n;
}

int_ dsscanf3264(const char* s, const char* fmt, ...) noexcept
{
    va_list list;
    va_start(list, fmt);
    int_ n = dsscanfV(nullptr, s, -1, fmt, list);
    va_end(list);
    return n;
}

#if TOOLSLIB_CMAASTRING64 <= 1

bool mysscanf64(const char * str, _qword * ptr, int_ Len /*= -1*/, int_ * StopPos /*= nullptr*/, const char * StopChars /*= nullptr*/) noexcept
{
    int pos = 0;
    _qword x;
    char Sgn;
    bool bErr;
    const _qword thres = (LLONG_MIN + 9) / 10;
    if (Len < 0)
    {
        Sgn = *str;
        if (Sgn == '-' || Sgn == '+')
        {
            pos++;
        }
        if (str[pos] < '0' || str[pos] > '9')
        {
            if (StopPos)
            {
                *StopPos = (warning_int)pos;
            }
            return false;
        }
        x = -(str[pos++] - '0');
        while (str[pos] >= '0' && str[pos] <= '9')
        {
            if (x >= thres)
            {
                x = x * 10 - (str[pos++] - '0');
            }
            else
            {
                _qword y = x;
                x *= 10;
                if (x / 10 != y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos;
                    }
                    return false;
                }
                y = x;
                x -= (str[pos++] - '0');
                if (x > y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos - 1;
                    }
                    return false;
                }
            }
        }
        if (StopPos)
        {
            *StopPos = (warning_int)pos;
        }
        bErr = (str[pos] && (!StopChars || !strchr(StopChars, str[pos])));
    }
    else
    {
        // Len >= 0
        if (Len == pos)
        {
            if (StopPos)
            {
                *StopPos = (warning_int)pos;
            }
            return false;
        }

        Sgn = *str;
        if (Sgn == '-' || Sgn == '+')
        {
            pos++;
            if (Len == pos)
            {
                if (StopPos)
                {
                    *StopPos = (warning_int)pos;
                }
                return false;
            }
        }
        if (str[pos] < '0' || str[pos] > '9')
        {
            if (StopPos)
            {
                *StopPos = (warning_int)pos;
            }
            return false;
        }
        x = -(str[pos++] - '0');
        while (pos < Len && str[pos] >= '0' && str[pos] <= '9')
        {
            if (x >= thres)
            {
                x = x * 10 - (str[pos++] - '0');
            }
            else
            {
                _qword y = x;
                x *= 10;
                if (x / 10 != y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos;
                    }
                    return false;
                }
                y = x;
                x -= (str[pos++] - '0');
                if (x > y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos - 1;
                    }
                    return false;
                }
            }
        }
        if (StopPos)
        {
            *StopPos = (warning_int)pos;
        }
        bErr = (pos < Len && str[pos] && (!StopChars || !strchr(StopChars, str[pos])));
    }
    if  (Sgn != '-')
    {
        x = -x;
        if  (x < 0)
        {
            if  (StopPos)
            {
                *StopPos = (warning_int)pos - 1;
            }
            bErr = true;
        }
    }
    if  (!bErr)
    {
        *ptr = x;
        return true;
    }
    return false;
}

bool mysscanfu64(const char * str, _uqword * ptr, int_ Len /*= -1*/, int_ * StopPos /*= nullptr*/, const char * StopChars /*= nullptr*/) noexcept
{
    int pos = 0;
    _uqword x;
    bool bErr;
    const _uqword thres = (ULLONG_MAX - 9) / 10;
    if (Len < 0)
    {
        if (*str == '+')
        {
            pos++;
        }
        if (str[pos] < '0' || str[pos] > '9')
        {
            if (StopPos)
            {
                *StopPos = (warning_int)pos;
            }
            return false;
        }
        x = (str[pos++] - '0');
        while (str[pos] >= '0' && str[pos] <= '9')
        {
            if (x <= thres)
            {
                x = x * 10 + (str[pos++] - '0');
            }
            else
            {
                _uqword y = x;
                x *= 10;
                if (x / 10 != y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos;
                    }
                    return false;
                }
                y = x;
                x += (str[pos++] - '0');
                if (x < y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos - 1;
                    }
                    return false;
                }
            }
        }
        if (StopPos)
        {
            *StopPos = (warning_int)pos;
        }
        bErr = (str[pos] && (!StopChars || !strchr(StopChars, str[pos])));
    }
    else
    {
        // Len >= 0
        if (Len == pos)
        {
            if (StopPos)
            {
                *StopPos = (warning_int)pos;
            }
            return false;
        }

        if (*str == '+')
        {
            pos++;
            if (Len == pos)
            {
                if (StopPos)
                {
                    *StopPos = (warning_int)pos;
                }
                return false;
            }
        }
        if (str[pos] < '0' || str[pos] > '9')
        {
            if (StopPos)
            {
                *StopPos = (warning_int)pos;
            }
            return false;
        }
        x = (str[pos++] - '0');
        while (pos < Len && str[pos] >= '0' && str[pos] <= '9')
        {
            if (x <= thres)
            {
                x = x * 10 + (str[pos++] - '0');
            }
            else
            {
                _uqword y = x;
                x *= 10;
                if (x / 10 != y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos;
                    }
                    return false;
                }
                y = x;
                x += (str[pos++] - '0');
                if (x < y)
                {
                    if (StopPos)
                    {
                        *StopPos = (warning_int)pos - 1;
                    }
                    return false;
                }
            }
        }
        if (StopPos)
        {
            *StopPos = (warning_int)pos;
        }
        bErr = (pos < Len && str[pos] && (!StopChars || !strchr(StopChars, str[pos])));
    }
    if  (!bErr)
    {
        *ptr = x;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------------
// [ ][{+|-}][цифры][.цифры][{e|E}[{+|-}]цифры]
template<typename T> bool mysscanf_double_internal(const char* p, T * ptr, int_ Len /*= -1*/, int_* StopPos /*= nullptr*/, const char* StopChars /*= nullptr*/, int_ iStrict = 0) noexcept
{
    bool bOk = false;
    const char* p0 = p;

    T x = 0;
    if (iStrict >= 0)
    {
        while ((Len > 0 || (Len < 0 && *p)) && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
        {
            p++;
            Len = Len > 0 ? Len - 1 : Len;
        }
    }
    while ((Len > 0 || (Len < 0 && *p)))
    {
        char c = *p;
        if (c == '+' || c == '-')
        {
            p++;
            Len = Len > 0 ? Len - 1 : Len;
        }
        if ((Len > 0 || (Len < 0 && *p)) && ((*p >= '0' && *p <= '9') || *p == '.'))
        {
            int_ digits = 0;
            while ((Len > 0 || (Len < 0 && *p)) && *p >= '0' && *p <= '9')
            {
                x = x * 10 + (*p++ - '0');
                Len = Len > 0 ? Len - 1 : Len;
                digits++;
            }
            int_ ex = 0;
            if ((Len > 0 || (Len < 0 && *p)) && *p == '.')
            {
                p++;
                Len = Len > 0 ? Len - 1 : Len;
                if (!digits && ((!Len || !*p) || *p < '0' || *p > '9'))
                {
                    break;
                }
                while ((Len > 0 || (Len < 0 && *p)) && *p >= '0' && *p <= '9')
                {
                    x = x * 10 + (*p++ - '0');
                    Len = Len > 0 ? Len - 1 : Len;
                    ex--;
                }
            }
            if (c == '-')
            {
                x = -x;
            }
            if (ex)
            {
                x *= (T)pow((T)10, ex);
            }
            if ((Len > 0 || (Len < 0 && *p)) && (*p == 'E' || *p == 'e'))
            {
                p++;
                Len = Len > 0 ? Len - 1 : Len;
                if (Len > 0 || (Len < 0 && *p))
                {
                    c = *p;
                    if (c == '+' || c == '-')
                    {
                        p++;
                        Len = Len > 0 ? Len - 1 : Len;
                    }
                    if ((Len > 0 || (Len < 0 && *p)) && (*p >= '0' && *p <= '9'))
                    {
                        ex = (*p++ - '0');
                        Len = Len > 0 ? Len - 1 : Len;
                        while ((Len > 0 || (Len < 0 && *p)) && (*p >= '0' && *p <= '9'))
                        {
                            ex = ex * 10 + (*p++ - '0');
                            Len = Len > 0 ? Len - 1 : Len;
                        }
                        if (c == '-')
                        {
                            ex = -ex;
                        }
                        if (ex)
                        {
                            x *= (T)pow((T)10, ex);
                        }
                        bOk = true;
                    }
                }
            }
            else
            {
                bOk = true;
            }
        }
        break;
    }
    if (bOk)
    {
        if (iStrict > 0)
        {
            while ((Len > 0 || (Len < 0 && *p)) && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
            {
                p++;
                Len = Len > 0 ? Len - 1 : Len;
            }
            /*
            if (iStrict == 2 && (Len > 0 || (Len < 0 && *p)) && !*p)
            {
                if (Len > 0)
                {
                    //p = e;
                    p += Len;
                    Len = 0;
                }
            }
            */
        }
        ////if (!(iStrict && p != e))
        if (!iStrict || !Len || (Len && (!StopChars || strchr(StopChars, *p))))
        {
            *ptr = x;
            if (StopPos)
            {
                *StopPos = (warning_int)(p - p0);
            }
            return true;
        }
    }
    if (StopPos)
    {
        *StopPos = (warning_int)(p - p0);
    }
    return false;
}
//-----------------------------------------------------------------------------------------------
bool mysscanf_float(const char* p, float* ptr, int_ Len /*= -1*/, int_* StopPos /*= nullptr*/, const char* StopChars /*= nullptr*/, int_ iStrict /*= 0*/) noexcept
{
    return mysscanf_double_internal(p, ptr, Len, StopPos, StopChars, iStrict);
}
bool mysscanf_double(const char* p, double* ptr, int_ Len /*= -1*/, int_* StopPos /*= nullptr*/, const char* StopChars /*= nullptr*/, int_ iStrict /*= 0*/) noexcept
{
    return mysscanf_double_internal(p, ptr, Len, StopPos, StopChars, iStrict);
}
bool mysscanf_long_double(const char* p, long double* ptr, int_ Len /*= -1*/, int_* StopPos /*= nullptr*/, const char* StopChars /*= nullptr*/, int_ iStrict /*= 0*/) noexcept
{
    return mysscanf_double_internal(p, ptr, Len, StopPos, StopChars, iStrict);
}
//-----------------------------------------------------------------------------------------------

#if 1
int_ CMaaIsMemEq08(const char* p1, const char* p2, int_ count) noexcept // for count == 0..8, false for count < 0
{
    switch (count)
    {
    case 0:
        return true;
    case 1:
        return *p1 == *p2;
    case 2:
        return *(_word*)p1 == *(_word*)p2;
    case 3:
        return *(_word*)p1 == *(_word*)p2 && p1[2] == p2[2];
    case 4:
        return *(_dword*)p1 == *(_dword*)p2;
    case 5:
        return *(_dword*)p1 == *(_dword*)p2 && p1[4] == p2[4];
    case 6:
        return *(_dword*)p1 == *(_dword*)p2 && *(_word*)(p1 + 4) == *(_word*)(p2 + 4);
    case 7:
        return *(_dword*)p1 == *(_dword*)p2 && *(_word*)(p1 + 4) == *(_word*)(p2 + 4) && p1[6] == p2[6];
    case 8:
        return *(_qword*)p1 == *(_qword*)p2;
    default:
        break;
    }
    return false;
}

int_ CMaaIsMemEq0_16(const char* p1, const char* p2, int_ count) noexcept // for count == 0..16, false for count < 0
{
    switch (count)
    {
    case 0:
        return true;
    case 1:
        return *p1 == *p2;
    case 2:
        return *(_word*)p1 == *(_word*)p2;
    case 3:
        return *(_word*)p1 == *(_word*)p2 && p1[2] == p2[2];
    case 4:
        return *(_dword*)p1 == *(_dword*)p2;
    case 5:
        return *(_dword*)p1 == *(_dword*)p2 && p1[4] == p2[4];
    case 6:
        return *(_dword*)p1 == *(_dword*)p2 && *(_word*)(p1 + 4) == *(_word*)(p2 + 4);
    case 7:
        return *(_dword*)p1 == *(_dword*)p2 && *(_word*)(p1 + 4) == *(_word*)(p2 + 4) && p1[6] == p2[6];
    case 8:
        return *(_qword*)p1 == *(_qword*)p2;
    case 9:
        return *(_qword*)p1 == *(_qword*)p2 && p1[8] == p2[8];
    case 10:
        return *(_qword*)p1 == *(_qword*)p2 && *(_word*)(p1 + 8) == *(_word*)(p2 + 8);
    case 11:
        return *(_qword*)p1 == *(_qword*)p2 && *(_word*)(p1 + 8) == *(_word*)(p2 + 8) && p1[10] == p2[10];
    case 12:
        return *(_qword*)p1 == *(_qword*)p2 && *(_dword*)(p1 + 8) == *(_dword*)(p2 + 8);
    case 13:
        return *(_qword*)p1 == *(_qword*)p2 && *(_dword*)(p1 + 8) == *(_dword*)(p2 + 8) && p1[12] == p2[12];
    case 14:
        return *(_qword*)p1 == *(_qword*)p2 && *(_dword*)(p1 + 8) == *(_dword*)(p2 + 8) && *(_word*)(p1 + 12) == *(_word*)(p2 + 12);
    case 15:
        return *(_qword*)p1 == *(_qword*)p2 && *(_dword*)(p1 + 8) == *(_dword*)(p2 + 8) && *(_word*)(p1 + 12) == *(_word*)(p2 + 12) && p1[14] == p2[14];
    case 16:
        return *(_qword*)p1 == *(_qword*)p2 && *(_qword*)(p1 + 8) == *(_qword*)(p2 + 8);
    default:
        break;
    }
    return false;
}

int_ MaaMemCmpQ_C(const _qword* ptr1, const _qword* ptr2, int_ count) noexcept // compares len >= 0 ptr1, ptr2; len < 0 return false
{
    if (count < 0)
    {
        return false;
    }
    for (int_ i = count >> 4; i > 0; i--)
    {
#if 0
        if (*ptr1++ != *ptr2++)
        {
            return false;
        }
        if (*ptr1++ != *ptr2++)
        {
            return false;
        }
#else
        if (ptr1[0] != ptr2[0] || ptr1[1] != ptr2[1])
        {
            return false;
        }
        ptr1 += 2;
        ptr2 += 2;
#endif
    }
    return CMaaIsMemEq0_16((const char*)ptr1, (const char*)ptr2, count & 0xf);
}
//#else
int_ MaaMemCmpQ_C(const _qword* ptr1, const _qword* ptr2, _qword count) noexcept // compares len >= 0 ptr1, ptr2; len < 0 return false
{
    if (count < 0)
    {
        return false;
    }
    for (_qword i = count >> 4; i > 0; i--)
    {
#if 0
        if (*ptr1++ != *ptr2++)
        {
            return false;
        }
        if (*ptr1++ != *ptr2++)
        {
            return false;
        }
#else
        if (ptr1[0] != ptr2[0] || ptr1[1] != ptr2[1])
        {
            return false;
        }
        ptr1 += 2;
        ptr2 += 2;
#endif
    }
    return CMaaIsMemEq0_16((const char*)ptr1, (const char*)ptr2, (int_)(count & 0x0f));
}
#endif

#if 0
void CMaaMemZero0_16(char* p, int_ count) noexcept // for count == 0..16 fill zeros
{
    switch (count)
    {
    case 0:
        break;
    case 1:
        *p = 0;
        break;
    case 2:
        *(_word*)p = 0;
        break;
    case 3:
        *(_word*)p = 0;
        p[2] = 0;
        break;
    case 4:
        *(_dword*)p = 0;
        break;
    case 5:
        *(_dword*)p = 0;
        p[4] = 0;
        break;
    case 6:
        *(_dword*)p = 0;
        *(_word*)(p + 4) = 0;
        break;
    case 7:
        *(_dword*)p = 0;
        *(_word*)(p + 4) = 0;
        p[6] = 0;
        break;
    case 8:
        *(_qword*)p = 0;
        break;
    case 9:
        *(_qword*)p = 0;
        p[8] = 0;
        break;
    case 10:
        *(_qword*)p = 0;
        *(_word*)(p + 8) = 0;
        break;
    case 11:
        *(_qword*)p = 0;
        *(_word*)(p + 8) = 0;
        p[10] = 0;
        break;
    case 12:
        *(_qword*)p = 0;
        *(_dword*)(p + 8) = 0;
        break;
    case 13:
        *(_qword*)p = 0;
        *(_dword*)(p + 8) = 0;
        p[12] = 0;
        break;
    case 14:
        *(_qword*)p = 0;
        *(_dword*)(p + 8) = 0;
        *(_word*)(p + 12) = 0;
        break;
    case 15:
        *(_qword*)p = 0;
        *(_dword*)(p + 8) = 0;
        *(_word*)(p + 12) = 0;
        p[14] = 0;
        break;
    case 16:
        *(_qword*)p = 0;
        *(_qword*)(p + 8) = 0;
        break;
    default:
        break;
    }
}
void MaaMemZeroQ_C(_qword* ptr, int_ count) noexcept // fill zeros len >= 0 ptr
{
    for (int_ i = count >> 4; i > 0; i--)
    {
#if 0
        *ptr++ = 0;
        *ptr++ = 0;
#else
        ptr[1] = ptr[0] = 0;
        ptr += 2;
#endif
    }
    CMaaMemZero0_16((char*)ptr, count & ((1 << (sizeof(int_) * 8 - 1)) | 0xf));
}
void MaaMemZeroQ_C(_qword* ptr, _qword count) noexcept // fill zeros len >= 0 ptr
{
    for (_qword i = count >> 4; i > 0; i--)
    {
#if 0
        * ptr++ = 0;
        *ptr++ = 0;
#else
        ptr[1] = ptr[0] = 0;
        ptr += 2;
        ptr += 2;
#endif
    }
    CMaaMemZero0_16((char*)ptr, count < 0 ? (int_)((count & 0x0f) | (1 << (sizeof(int_) * 8 - 1))) : (int_)(count & 0x0f));
}
#endif // 0

#endif

//const CMaaString CMaaStringZ;
//const CMaaString CMaaString0("\0", 1);
//const CMaaString CMaaStringWC0("\0\0", (int)sizeof(_WC_));
//const CMaaString CMaaStringWChar0("\0\0\0\0", (int)sizeof(wchar_t));
//const CMaaString CMaaStringWC32_0("\0\0\0\0", (int)sizeof(char32_t));

template<> const CMaaString & CMaaStr0<void>() noexcept
{
    static const CMaaString s;
    return s;
}

const char ceCMaaStringImp::sChar0_1[8] = {};
//const char CMaaString::sChar0_1[8] = {};
const char CMaaString::sChar0_2[8] = {};
const char CMaaString::sChar0_3[10] = {};
CMaaString CMaaString::s_szSpaces{ " " };

//------------------------------------------------------------------------------
// class CMaaString
//------------------------------------------------------------------------------
/*
CMaaString::CMaaString() noexcept
:   m_pImp(nullptr)
{
}
*/
//---------------------------------------------------------------------------
/*
void CMaaString::CMaaStringImp::operator=(CMaaString::CMaaStringImp&& That) noexcept
{
    this->~CMaaStringImp();
    memcpy(this, &That, sizeof(CMaaStringImp));
    if (That.m_pszStr == CMAASTRING_UNALLOC_PTR_(&That))
    {
        m_pszStr = CMAASTRING_UNALLOC_PTR;
    }
#ifdef TOOLSLIB_STAT_STRING_IMP
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
#endif
    That.m_iLength = 0;
    That.f2.m_Flags12 = eDontDelete;// | eImpType;
    //memset(&That, 0, sizeof(CMaaStringImp));
}
*/
/*
void CMaaString::CMaaStringImp::re_new(const char* pszTxt, size_t Len) noexcept
{
    this->~CMaaStringImp();
    char* p = (char*)this;
    ::new(p) CMaaStringImp(pszTxt, Len);
}
*/
CMaaString::CMaaString(CMaaStringImp* pImp, bool bInc, bool) noexcept
{
    if (bInc)
    {
        pImp->AddRef();
    }
#if TOOLSLIB_USE_CMAASTRING64 == 2
    if (pImp->GetImpVersion() != ceCMaaStringImp::eImpType)
    {
#if TOOLSLIB_CMAASTRING64 == 0
        CMaaString64 s;
        s.m_pImp = (CMaaString64::CMaaStringImp*)pImp;
#else
        CMaaString32 s;
        s.m_pImp = (CMaaString32::CMaaStringImp*)pImp;
#endif
        s.m_pImp->AddRef();
        m_pImp = nullptr;
        *this = s;
    }
    else
#endif
        m_pImp = pImp;
}
CMaaString CMaaString::GetAddRefData(int Idx, bool bLock) const noexcept
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA
    if (PIMP(m_pImp))
    {
        if (!Idx && (TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags1) & (CMaaStringImp::eData0Eq >> 8)))
        {
            return *this;
        }
        CMaaWin32Locker_aLocker_gLock_Atomic; // assumption: SetAddRefData(bNeedSingleOwner = true) or external lock
        bLock && aLocker_Lock;
        const unsigned char m = TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags1);
        if ((m & (CMaaStringImp::eHasAddRefData >> 8)) && !(!Idx && (m & (CMaaStringImp::eData1 >> 8))))
        {
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
            CMaaString::CMaaStringImp* pImp = (CMaaString::CMaaStringImp*)m_pImp->m_pAddRefDataPtr;
#else
            CMaaString::CMaaStringImp* pImp = (CMaaString::CMaaStringImp*)m_pImp->m_pRefferedTo;
#endif
            pImp->AddRef();
            bLock && aLocker_UnLock;
            return CMaaString(pImp, false, false);
        }
        bLock && aLocker_UnLock;
    }
#endif
    return CMaaStringZ;
}
//---------------------------------------------------------------------------
CMaaString & CMaaString::operator= (const CMaaConstStr& s) noexcept
{
    if (!s.len)
    {
        Empty();
    }
    else if (PIMP(m_pImp) // m_pImp != sp_NullImp 
        && m_pImp->IsSingleOwner()) // just the optimization
    {
        m_pImp->~CMaaStringImp();
        ::new(m_pImp) CMaaString::CMaaStringImp(s.p, s.len, eROMemString0); // noexcept
    }
    else
    {
        CMaaString tmp(s); // noexcept
        *this = std::move(tmp);
    }
    return *this;
}
CMaaString& CMaaString::operator_eq(const void* pszTxt, int l) noexcept(noexcept_new)
{
    if (/*!pszTxt ||*/ l <= 0)
    {
        Empty();
    }
    else if (PIMP(m_pImp) // m_pImp != sp_NullImp 
        && m_pImp->IsSingleOwner()) // just the optimization
    {
        char b[128]; // 128 should be more then CMaaStringImp::GetMaxUnAllocLength()
        //const int l = (warning_int)strlen(pszTxt); // warning: 2GB limit
        if (!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)) && m_pImp->m_iCapacity >= l) // && l > 0
        {
#ifndef CMaaStringImp_NOT_NULL_MODE
            m_pImp->ClearHash();
#else
            ClearHash();
#endif
            memmove(m_pImp->m_pszStr, pszTxt, m_pImp->m_iLength = l);
            *(_qword*)(m_pImp->m_pszStr + l) = 0;
        }
        else
        {
            char* p = nullptr;
            if (l > CMaaStringImp::GetMaxUnAllocLength())
            {
                if (l + 8 > l)
                {
                    p = TL_NEW char[l + 8];
                    if (p)
                    {
                        memcpy(p, pszTxt, l);
                        *(_qword*)(p + l) = 0;
                    }
                    else
                    {
                        Empty();
                    }
                }
                else
                {
                    Empty();
                }
            }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
            else //if (l > 0)
            {
                if (m_pImp->m_pszStr == CMAASTRING_UNALLOC_PTR_(m_pImp)) // && !(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eROString)))
                {
#ifndef CMaaStringImp_NOT_NULL_MODE
                    m_pImp->ClearHash();
#else
                    ClearHash();
#endif
                    memmove(m_pImp->m_pszStr, pszTxt, m_pImp->m_iLength = l);
                    memset(m_pImp->m_pszStr + l, 0, m_pImp->GetMaxUnAllocLength() - l);
                }
                else if ((int)sizeof(b) >= CMaaStringImp::GetMaxUnAllocLength())
                {
                    memcpy(p = b, pszTxt, l);
                    //*(_qword*)(p + l) = 0;
                }
                else
                {
                    Empty();
                }
            }
#else
            else
            {
                Empty();
            }
#endif
            if (p)
            {
                m_pImp->~CMaaStringImp();
                ::new(m_pImp) CMaaString::CMaaStringImp(p, false, l); // noexcept
            }
            /*
            {
                CMaaStringImp Imp(pszTxt, Len);
                *m_pImp = std::move(Imp);
            }
            */
        }
    }
    else
    {
        CMaaStringImp* pImp = TL_NEW CMaaStringImp(pszTxt, l);
        if (!pImp || !pImp->m_iLength)
        {
            delete pImp;
            Empty();
        }
        else
        {
            INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            AssignImpFast(m_pImp, pImp, false, MANUAL_STRINGS_REFS);
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
CMaaString::CMaaString(const CMaaString64& That) noexcept
{
    m_pImp = TL_NEW CMaaStringImp(That);
    if (!m_pImp)
    {
#ifdef CMaaStringImp_NOT_NULL_MODE
        m_pImp = sp_NullImp;
#endif
        return;
    }
    if (!m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
CMaaString::CMaaStringImp::CMaaStringImp(const CMaaString64& That) noexcept
{
    CMaaString64::CMaaStringImp* pImp = That.m_pImp;
    if (pImp 
#ifdef CMaaStringImp_NOT_NULL_MODE
        != (CMaaString64::CMaaStringImp*)&CMaaString64::C::snCMaaStringZ
#endif
        && pImp->m_iLength <= 0x7fffffff - 8)
    {
        m_iLength = (int)pImp->m_iLength;
        m_pszStr = pImp->m_pszStr;
        m_Hash = pImp->m_Hash;
#ifdef CMAASTRING_CACHE_HASH64
        m_Hash64 = pImp->m_Hash64;
#endif
        m_pRefferedTo = (CMaaStringImp*)((pImp->f2.f12.m_Flags0 & eHasReffered) ? pImp->m_pRefferedTo : pImp);
        if ((m_pRefferedTo->f2.f12.m_Flags0 & eImpMask) == eImp32)
        {
            m_pRefferedTo->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
        }
        else
        {
            ((CMaaString64::CMaaStringImp*)m_pRefferedTo)->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
        }
        f2.m_Flags12 = (eDontDelete | eHasReffered | eImp32) | (pImp->f2.m_Flags12 & (eHashExists | eHash64Exists | eROString | eLengthLimit | eMemoryString | e1632All_));

#ifdef CMAASTRING_HAS_ADD_DATA_00000000
        if ((TOOLSLIB_LOAD_ATOMIC(pImp->f2.a12.m_Flags1) & (eHasAddData >> 8)))
        {
            const int_ iDataLength = pImp->m_iDataLength;
            const char* pszData = pImp->m_pszData;
            if (pszData && iDataLength > 0)
            {
                try
                {
                    m_pszData = TL_NEW char[iDataLength];
                    if (m_pszData)
                    {
                        memcpy(m_pszData, pszData, m_iDataLength = iDataLength);
                        f2.f12_.m_Flags1 |= (eHasAddData >> 8);
                    }
                }
                catch (...)
                {
                }
            }
        }
#endif
        INC_TOOLSLIB_STAT_STRING_IMP_REF;
        return;
    }
    //m_pszStr = nullptr;
    m_iLength = 0;
    f2.m_Flags12 = eDontDelete;
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
}
#else
CMaaString::CMaaString(const CMaaString32& That) noexcept
{
    m_pImp = TL_NEW CMaaStringImp(That);
    if (!m_pImp)
    {
#ifdef CMaaStringImp_NOT_NULL_MODE
        m_pImp = sp_NullImp;
#endif
        return;
    }
    if (!m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
CMaaString::CMaaStringImp::CMaaStringImp(const CMaaString32& That) noexcept
{
    CMaaString32::CMaaStringImp* pImp = That.m_pImp;
    if (pImp 
#ifdef CMaaStringImp_NOT_NULL_MODE
        != (CMaaString32::CMaaStringImp*)&CMaaString32::C::snCMaaStringZ
#endif
        )
    {
        m_iLength = pImp->m_iLength;
        m_pszStr = pImp->m_pszStr;
        m_Hash = pImp->m_Hash;
#ifdef CMAASTRING_CACHE_HASH64
        m_Hash64 = pImp->m_Hash64;
#endif
        m_pRefferedTo = (CMaaStringImp*)((pImp->f2.f12.m_Flags0 & eHasReffered) ? pImp->m_pRefferedTo : pImp);
        if ((m_pRefferedTo->f2.f12.m_Flags0 & eImpMask) == eImp64)
        {
            m_pRefferedTo->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
        }
        else
        {
            ((CMaaString32::CMaaStringImp*)m_pRefferedTo)->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
        }
        f2.m_Flags12 = (eDontDelete | eHasReffered | eImp64) | (pImp->f2.m_Flags12 & (eHashExists | eHash64Exists | eROString | eLengthLimit | eMemoryString | e1632All_));

#ifdef CMAASTRING_HAS_ADD_DATA_00000000
        if ((TOOLSLIB_LOAD_ATOMIC(pImp->f2.a12.m_Flags1) & (eHasAddData >> 8)))
        {
            const int_ iDataLength = pImp->m_iDataLength;
            const char* pszData = pImp->m_pszData;
            if (pszData && iDataLength > 0)
            {
                try
                {
                    m_pszData = TL_NEW char[iDataLength];
                    if (m_pszData)
                    {
                        memcpy(m_pszData, pszData, m_iDataLength = iDataLength);
                        f2.f12_.m_Flags1 |= (eHasAddData >> 8);
                    }
                }
                catch (...)
                {
                }
            }
        }
#endif
        INC_TOOLSLIB_STAT_STRING_IMP_REF;
        return;
    }
    //m_pszStr = nullptr;
    m_iLength = 0;
    f2.m_Flags12 = eDontDelete;
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
}
#endif
#endif
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
CMaaString::CMaaString(const void* pszStr) noexcept(noexcept_new)
{
    m_pImp = TL_NEW CMaaStringImp((const char*)pszStr);
    if (m_pImp && !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
CMaaString::CMaaString(char* pszStr) noexcept(noexcept_new)
#else
CMaaString::CMaaString(const char* pszStr) noexcept(noexcept_new)
#endif
{
    m_pImp = TL_NEW CMaaStringImp(pszStr);
    if (!m_pImp || !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
CMaaString::CMaaString(char8_t* pszStr) noexcept(noexcept_new)
#else
CMaaString::CMaaString(const char8_t* pszStr) noexcept(noexcept_new)
#endif
{
    m_pImp = TL_NEW CMaaStringImp((const char *)pszStr);
    if (!m_pImp || !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
#endif
//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(const char * pszStr) noexcept(noexcept_new)
//:   f2(eImpType)
{
    if  (pszStr)
    {
        m_iLength = (int)strlen(pszStr);
        if (m_iLength > CMAASTRING_MAX_UNALLOC_LENGTH)
        {
            f2.SetFlags12(eImpType);
            m_pszStr = (m_iLength + 8 > m_iLength) ? TL_NEW char[m_iLength + 8] : nullptr;
            if (m_pszStr)
            {
                memcpy(m_pszStr, pszStr, m_iCapacity = m_iLength);
                *(_qword*)(m_pszStr + m_iLength) = 0;
                INC_TOOLSLIB_STAT_STRING_IMP_LONG;
                return;
            }
        }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
        else if (m_iLength > 0)
        {
            m_pszStr = CMAASTRING_UNALLOC_PTR;
            ZeroUnallocTail0;
            memcpy(m_pszStr, pszStr, m_iLength);
            f2.SetFlags12(eDontDelete | eImpType);
            INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
            return;
        }
#endif
    }
    //m_pszStr = nullptr;
    m_iLength = 0;
    f2.SetFlags12(eDontDelete);
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
}

//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(size_t Length, _e1_ Flags, int AddCapacity) noexcept(noexcept_new)
{
    m_iLength = (int)Length; // warning: 2GB limit
    if (m_iLength > CMAASTRING_MAX_UNALLOC_LENGTH)
    {
        f2.SetFlags12(((int_)Flags & (int_)e1632All_) | eImpType);
        m_pszStr = (m_iLength + AddCapacity + 8 > m_iLength) ? TL_NEW char[(m_iCapacity = m_iLength + AddCapacity) + 8] : 
            (m_iLength + 8 > m_iLength) ? TL_NEW char[(m_iCapacity = m_iLength) + 8] : nullptr;
        if (m_pszStr)
        {
            //if (!bNotZero)
            //{
            //    memset(m_pszStr, 0, m_iLength + 8);
            //}
            //else
            //{
                *(_qword*)(m_pszStr + m_iLength) = 0; // memory not initialized by zeros
            //}
            INC_TOOLSLIB_STAT_STRING_IMP_LONG;
            return;
        }
    }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
    else if (m_iLength > 0)
    {
        m_pszStr = CMAASTRING_UNALLOC_PTR;
        ZeroUnallocTail0;
        f2.SetFlags12(((int_)Flags & (int_)e1632All_) | eDontDelete | eImpType);
        INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
        return;
    }
#endif
    //m_pszStr = nullptr;
    m_iLength = 0;
    f2.SetFlags12(eDontDelete);
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
}
//---------------------------------------------------------------------------
void CMaaString::ctor(const void* pMem, size_t Len) noexcept(noexcept_new)
{
    m_pImp = TL_NEW CMaaStringImp(pMem, Len);
    if (!m_pImp || !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
//---------------------------------------------------------------------------
/*
CMaaString::CMaaString(const void* pMem, size_t Len) noexcept(noexcept_new)
{
    
    m_pImp = TL_NEW CMaaStringImp(pMem, Len);
    if (!m_pImp || !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
*/
//---------------------------------------------------------------------------
#if TOOLSLIB_CMAASTRING64 <= 0
CMaaString::CMaaString(const void* pMem, size_t Len, _e1632 Flags) noexcept(noexcept_new)
{
    m_pImp = TL_NEW CMaaStringImp(pMem, Len, Flags);
    if (m_pImp && !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
#endif
//---------------------------------------------------------------------------
CMaaString::CMaaString(const void* pMem, int Len, _e1632 Flags) noexcept(noexcept_new)
{
    m_pImp = TL_NEW CMaaStringImp(pMem, Len, Flags);
    if (!m_pImp || !m_pImp->m_iLength)
    {
        delete m_pImp;
        m_pImp = sp_NullImp;
    }
    INC_TOOLSLIB_STAT_STRING_IMP_REFS((m_pImp != sp_NullImp));
}
//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(const void * pszStr, size_t Length, _e1632 Flags) noexcept(noexcept_new)
{
    m_iLength = (int)Length; // warning: 2GB limit
    if  (m_iLength > CMAASTRING_MAX_UNALLOC_LENGTH)
    {
        f2.SetFlags12((e1632All_ & (int)Flags) | eImpType);
        m_pszStr = (m_iLength + 8 > m_iLength) ? TL_NEW char[(m_iCapacity = m_iLength) + 8] : nullptr;
        if  (m_pszStr)
        {
            if  (pszStr)
            {
                memcpy(m_pszStr, pszStr, m_iLength);
                *(_qword *)(m_pszStr + m_iLength) = 0;
            }
            else
            {
                memset(m_pszStr, 0, m_iLength + 8);
            }
            INC_TOOLSLIB_STAT_STRING_IMP_LONG;
            return;
        }
    }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
    else if (m_iLength > 0)
    {
        m_pszStr = CMAASTRING_UNALLOC_PTR;
        ZeroUnallocTail0;
        if (pszStr)
        {
            memcpy(m_pszStr, pszStr, m_iLength);
        }
        f2.SetFlags12((e1632All_ & (int_)Flags) | eDontDelete | eImpType);
        INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
        return;
    }
#endif
    //m_pszStr = nullptr;
    m_iLength = 0;
    f2.SetFlags12(eDontDelete);
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
}
//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(char* pStr, bool/*const void* pszStr*/, int Length, _e1632 Flags) noexcept
{
    m_iLength = (int)Length; // warning: 2GB limit
    if (m_iLength > CMAASTRING_MAX_UNALLOC_LENGTH)
    {
        m_iCapacity = m_iLength;
        f2.SetFlags12((e1632All_ & (int)Flags) | eImpType);
        m_pszStr = pStr;
        INC_TOOLSLIB_STAT_STRING_IMP_LONG;
    }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
    else if (m_iLength > 0)
    {
        m_pszStr = CMAASTRING_UNALLOC_PTR;
        ZeroUnallocTail0;
        memcpy(m_pszStr, pStr, m_iLength);
        f2.SetFlags12((e1632All_ & (int_)Flags) | eDontDelete | eImpType);
        //!!delete []pStr;
        INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
    }
#endif
    else
    {
        //m_pszStr = nullptr;
        m_iLength = 0;
        f2.SetFlags12(eDontDelete);
        //!!delete []pStr;
        INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
    }
}
//---------------------------------------------------------------------------
#if TOOLSLIB_CMAASTRING64 <= 0
CMaaString::CMaaString(const CMaaString &That, size_t Start, size_t Len) noexcept
{
    if  (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(That.m_pImp, Start, Len);
            if (!m_pImp)
            {
                m_pImp = sp_NullImp;
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch(...)
        {
        }
    }
    m_pImp = sp_NullImp;
}
#endif
//---------------------------------------------------------------------------
CMaaString::CMaaString(const CMaaString &That, int Start, int Len) noexcept
{
    if  (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(That.m_pImp, (size_t)Start, (size_t)Len);
            if (!m_pImp)
            {
                m_pImp = sp_NullImp;
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch(...)
        {
        }
    }
    m_pImp = sp_NullImp;
}
//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(CMaaStringImp* pRefferedTo, size_t Start, size_t Length) noexcept
{
    f2.SetFlags12(eDontDelete | eHasReffered | eImpType);
    //if  (pRefferedTo && Length + Start >= Start && Length + Start <= (size_t)pRefferedTo->m_iLength && (pRefferedTo->m_pszStr || !pRefferedTo->m_iLength))
    if (pRefferedTo && Length + Start > Start && Length + Start <= (size_t)pRefferedTo->m_iLength)
    {
        f2.f12.m_Flags1 |= ((e1632All_ >> 8) & (int_)TOOLSLIB_LOAD_ATOMIC(pRefferedTo->f2.a12.m_Flags1));
        {
            //f2.f12.m_Flags0 |= pRefferedTo->f2.f12.m_Flags0 & eMemoryString;
            CMaaStringImp* p = (pRefferedTo->f2.f12.m_Flags0 & eHasReffered) ? (CMaaStringImp*)pRefferedTo->m_pRefferedTo : pRefferedTo;
            p->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            m_pRefferedTo = p;
            /*const int iLength =*/ m_iLength = (int)Length; // warning: 2GB limit
            //m_iLength = m_iLength >= 0 ? m_iLength : 0;
            /*const char* s =*/ m_pszStr = pRefferedTo->m_pszStr + Start;
            if ((pRefferedTo->f2.f12.m_Flags1 & (eLengthLimit >> 8)))
            {
                const char* e = p->m_pszStr + p->m_iLength;
                const char* s = m_pszStr;
                if (s + m_iLength >= e)
                {
                    f2.f12.m_Flags1 |= ((eLengthLimit1 | eLengthLimit2 | eLengthLimit4) >> 8);
                }
                else if (s + ((m_iLength + 3) & ~1) > e)
                {
                    f2.f12.m_Flags1 |= ((eLengthLimit2 | eLengthLimit4) >> 8);
                }
                else if (s + ((m_iLength + 7) & ~3) > e)
                {
                    f2.f12.m_Flags1 |= (eLengthLimit4 >> 8);
                }
            }
        }
#ifdef CMAASTRING_CACHE_HASH
        //m_Hash = 0; // (Start == 0 && (int)Length == pRefferedTo->m_iLength) ? pRefferedTo->m_Hash : 0;
#endif
#ifdef CMAASTRING_CACHE_HASH64
        //m_Hash64 = 0; // (Start == 0 && (int)Length == pRefferedTo->m_iLength) ? pRefferedTo->m_Hash64 : 0;
#endif
        INC_TOOLSLIB_STAT_STRING_IMP_REF;
        return;
    }
    //m_pszStr = nullptr;
    m_iLength = 0;
    f2.f12.m_Flags0 = eDontDelete;// &= ~eHasReffered;
    INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
}
//---------------------------------------------------------------------------
CMaaString::CMaaString(const void* pMem, int Len, _e1_ Flags, int AddCapacity) noexcept(noexcept_new)
{
    if (Len > 0)
    {
        //try
        {
            m_pImp = !pMem ? TL_NEW CMaaStringImp(Len, Flags, AddCapacity) : TL_NEW CMaaStringImp(pMem, Len, (_e0)Flags /*eMemString / 16 / 32*/);
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        /*
        catch (...)
        {
            if (!pMem)
            {
                throw;
            }
        }
        */
    }
    m_pImp = sp_NullImp;
}
//---------------------------------------------------------------------------
CMaaString::CMaaString(const void* pMem, int Len, _e0 Flags) noexcept
{
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, Flags);
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = sp_NullImp;
}
//---------------------------------------------------------------------------
#if 0 // unchg
CMaaString::CMaaString(const void* pMem, int Len, _e1 /*Flags*/) noexcept
{
    Len = int_strlen((const char*)pMem);
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, eMemString0);
            if (!m_pImp || m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(m_pImp);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = nullptr;
}
//---------------------------------------------------------------------------
CMaaString::CMaaString(const void* pMem, int Len, _e2 /*Flags*/) noexcept
{
    Len = int_strlen((const char*)pMem);
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, eROMemString0);
            if (!m_pImp || m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(m_pImp);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = nullptr;
}
#endif
//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(const void* pszStr, size_t Length, _e0 Flags) noexcept
{
    f2.SetFlags12(eDontDelete | eMemoryString | eImpType | ((eROString | e1632All_) & (int_)Flags) | (eLengthLimit & ~(int_)Flags));
    m_iLength = (int)Length; // warning: 2GB limit
    if (m_iLength > 0)
    {
        m_pszStr = (char*)pszStr;
        INC_TOOLSLIB_STAT_STRING_IMP_MEM;
    }
    else
    {
        //m_pszStr = nullptr;
        m_iLength = 0;
        f2.SetFlags12(eDontDelete);
        INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
    }
}
//---------------------------------------------------------------------------
CMaaString::CMaaStringImp::CMaaStringImp(const CMaaConstStr &s, _e0 Flags) noexcept
{
    f2.SetFlags12(
#ifdef TOOLSLIB_TEST_CALC_HASH_CS
        eHashExists |
#endif
#ifdef TOOLSLIB_TEST_CALC_HASH64_CS
        eHash64Exists |
#endif
        eDontDelete | eMemoryString | eImpType | ((eROString | e1632All_) & (int_)Flags) | (eLengthLimit & ~(int_)Flags));

    if ((m_iLength = (int)s.len) > 0) // warning: 2GB limit
    {
        m_pszStr = (char*)s.p;
#ifdef TOOLSLIB_TEST_CALC_HASH_CS
        m_Hash = s.Hash;
#endif
#if defined(TOOLSLIB_TEST_CALC_HASH64_CS) && defined(CMAASTRING_CACHE_HASH64)
        m_Hash64 = s.Hash64;
#endif
        INC_TOOLSLIB_STAT_STRING_IMP_MEM;
    }
    else
    {
        //m_pszStr = nullptr;
        m_iLength = 0;
        f2.SetFlags12(eDontDelete);
        INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
    }
}
//---------------------------------------------------------------------------
#if 0
CMaaString::CMaaStringImp::CMaaStringImp(const void* pszStr, size_t Length, _e1 /*Flags*/) noexcept
:   f2 (eDontDelete | eLengthLimit | eMemoryString | eImpType)
{
    //f2.m_Flags12 = (eDontDelete | eLengthLimit | eMemoryString | eImpType);

    INC_TOOLSLIB_STAT_STRING_IMP_MEM;
    /*
    if (Flags == eStrlenMemString)
    {
        m_iLength = (int)strlen(m_pszStr = (char*)pszStr); // warning: 2GB limit
        f2.f12_.m_Flags1 &= (eLengthLimit1 >> 8);
    }
    else
    */
    {
        m_pszStr = (char*)pszStr; // warning: 2GB limit
        m_iLength = (int)Length;
    }
    /*
    else
    {
        m_iLength = (int)Length; // warning: 2GB limit
        m_iLength = m_iLength >= 0 ? m_iLength : 0;
        m_pszStr = (char*)pszStr;
    }
    */
    /*
     if   (m_iLength == 0)
     {
          m_pszStr = nullptr;
     }
    */
}
#endif
//---------------------------------------------------------------------------
CMaaString::CMaaString(const CMaaString & That) noexcept
{
#ifdef CHK_ERR_NULL_PIMP
    if (!That.m_pImp)
    {
        //printf("CMaaString::CMaaString(const CMaaString & That) - err NULL\n");
        ((CMaaString&)That).m_pImp = sp_NullImp;
    }
#endif
    m_pImp = (CMaaStringImp *)That.m_pImp;
    if (m_pImp != sp_NullImp)
    {
        m_pImp->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
    }
}
//---------------------------------------------------------------------------
/*
CMaaString::CMaaString(CMaaString&& That) noexcept
{
    m_pImp = That.m_pImp;
    That.m_pImp = sp_NullImp;
}
*/
//---------------------------------------------------------------------------
/*
CMaaString::~CMaaString() // noexcept
{
    //Empty();
    CMaaStringImp* p = m_pImp;
    //m_pImp = nullptr;
    if (p != sp_NullImp)
    {
        DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
        if (!p->Release())
        {
#if MANUAL_STRINGS_REFS
            delete p;
#endif
        }
    }
}
*/
//---------------------------------------------------------------------------
void CMaaString::CMaaStringImp::DerefReffered() noexcept
{
    //if ((Flags & (eHasReffered | eHasAddRefData)))
    {
#if TOOLSLIB_USE_CMAASTRING64 == 2
        if ((m_pRefferedTo->f2.f12.m_Flags0 & eImpMask) == eImp32)
        {
            CMaaString32::CMaaStringImp* p = (CMaaString32::CMaaStringImp*)m_pRefferedTo;
            DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
            if (!p->Release())
            {
#if MANUAL_STRINGS_REFS
                delete p;
#endif
            }
        }
        else
        {
            CMaaString64::CMaaStringImp* p = (CMaaString64::CMaaStringImp*)m_pRefferedTo;
            DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
            if (!p->Release())
            {
#if MANUAL_STRINGS_REFS
                delete p;
#endif
            }
        }
#else
        DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
        if (!m_pRefferedTo->Release())
        {
#if MANUAL_STRINGS_REFS
            delete (CMaaStringImp*)m_pRefferedTo;
#endif
        }
#endif
    }
}

#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
void CMaaString::CMaaStringImp::DerefAddRefDataPtr() noexcept
{
    //if ((Flags & eHasAddRefData))
    {
#if TOOLSLIB_USE_CMAASTRING64 == 2
        if ((m_pAddRefDataPtr->f2.f12.m_Flags0 & eImpMask) == eImp32)
        {
            CMaaString32::CMaaStringImp* p = (CMaaString32::CMaaStringImp*)m_pAddRefDataPtr;
            DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
            if (!p->Release())
            {
#if MANUAL_STRINGS_REFS
                delete p;
#endif
            }
        }
        else
        {
            CMaaString64::CMaaStringImp* p = (CMaaString64::CMaaStringImp*)m_pAddRefDataPtr;
            DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
            if (!p->Release())
            {
#if MANUAL_STRINGS_REFS
                delete p;
#endif
            }
        }
#else
        DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
        if (!m_pAddRefDataPtr->Release())
        {
#if MANUAL_STRINGS_REFS
            delete (CMaaStringImp*)m_pAddRefDataPtr;
#endif
        }
#endif
    }
}
#endif

void CMaaString::CMaaStringImp::ClearHash() noexcept // call after direct string modifications
{
#ifndef CMAASTRING_HAS_ADD_REF_DATA
    //TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags0, fetch_and, &=, ~(CMaaStringImp::eHashExists | CMaaStringImp::eHash64Exists));
    f2.f12.m_Flags0 &= ~(CMaaStringImp::eHashExists | CMaaStringImp::eHash64Exists);
#else
    //CMaaWin32Locker_aLocker_gLock_Atomic; // comment assumption: IsSingleOwner or UnAlloc string
    //aLocker_Lock;
    if (TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (CMaaStringImp::eHasAddRefData >> 8))
    {
        f2() &= (unsigned short)~(CMaaStringImp::eHashExists | CMaaStringImp::eHash64Exists | CMaaStringImp::eHasAddRefData | CMaaStringImp::eData0Eq | CMaaStringImp::eData1);
#ifndef CMAASTRING_HAS_ADD_REF_DATA_PTR
        DerefReffered();
#else
        DerefAddRefDataPtr();
#endif
    }
    else
    {
        f2() &= (unsigned short)~(CMaaStringImp::eHashExists | CMaaStringImp::eHash64Exists | CMaaStringImp::eHasAddRefData | CMaaStringImp::eData0Eq | CMaaStringImp::eData1);
    }
    //aLocker_UnLock;
#endif
}

bool CMaaString::CMaaStringImp::SetAddRefData(CMaaStringImp* pImp, int Idx, bool bNeedSingleOwner, int iLock) noexcept
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA
    if (this == pImp)
    {
        if (!Idx)
        {
            TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eData0Eq >> 8));
            return true;
        }
    }
    else if (CanSetAddRefData(pImp, 0, bNeedSingleOwner))
    {
        CMaaWin32Locker_aLocker_gLock_Atomic;
        (iLock > 0 || (iLock < 0 && !bNeedSingleOwner)) && aLocker_Lock;
        if (TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eHasAddRefData >> 8))
        {
            if (PIMP(pImp))
            {
#ifndef CMAASTRING_HAS_ADD_REF_DATA_PTR
                DerefReffered();
#else
                DerefAddRefDataPtr();
#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
                (m_pAddRefDataPtr = pImp)->AddRef();
#else
                (m_pRefferedTo = pImp)->AddRef();
#endif
                if (!Idx)
                {
                    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, (unsigned char)~((eData0Eq | eData1) >> 8));
                }
                else
                {
                    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, eData1 >> 8);
                }
            }
            else
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, (unsigned char)(!Idx ? ~((eHasAddRefData | eData0Eq | eData1) >> 8) : ~((eHasAddRefData | eData1) >> 8)));
#ifndef CMAASTRING_HAS_ADD_REF_DATA_PTR
                DerefReffered();
#else
                DerefAddRefDataPtr();
#endif
            }
        }
        else
        {
            if (PIMP(pImp))
            {
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
                (m_pAddRefDataPtr = pImp)->AddRef();
#else
                (m_pRefferedTo = pImp)->AddRef();
#endif
                if (!Idx)
                {
                    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, (unsigned char)~((eData0Eq | eData1) >> 8));
                    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, eHasAddRefData >> 8);
                }
                else
                {
                    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eHasAddRefData | eData1) >> 8);
                }
            }
            else
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, (unsigned char)(!Idx ? ~((eData0Eq | eData1) >> 8) : ~(eData1 >> 8)));
            }
        }
        (iLock > 0 || (iLock < 0 && !bNeedSingleOwner)) && aLocker_UnLock;
        return true;
    }
#endif
    return false;
}

CMaaString::CMaaStringImp::~CMaaStringImp() // noexcept
{
    //const unsigned char Flag0 = f2.f12.m_Flags0;
    ////const unsigned char Flag1 = TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1);
    //const unsigned char Flag1 = f2.f12.m_Flags1;
    const int_ Flags = f2.GetFlags12();
#ifdef TOOLSLIB_STAT_STRING_IMP
    if (Flags & eHasReffered)
    {
        DEC_TOOLSLIB_STAT_STRING_IMP_REF;
    }
    else if (Flags & eMemoryString)
    {
        DEC_TOOLSLIB_STAT_STRING_IMP_MEM;
    }
    else if (/*m_iLength > CMAASTRING_MAX_UNALLOC_LENGTH ||*/ !(Flags & eDontDelete))
    {
        DEC_TOOLSLIB_STAT_STRING_IMP_LONG;
    }
    else
    {
        DEC_TOOLSLIB_STAT_STRING_IMP_SHORT;
    }
#endif
#ifdef CMAASTRING_HAS_ADD_DATA
    if ((Flags & eHasAddData))
    {
        if ((Flags & eCryptoKey_) && m_pszData)
        {
            memset(m_pszData, 0, m_iDataLength);
        }
        delete[] m_pszData;
    }
#endif
    if (!(Flags & eDontDelete))
    {
        if ((Flags & eCryptoKey_))// && m_pszStr)
        {
            //printf("(Flags & eCryptoKey_) --> memset(m_pszStr, 0, m_iLength==%d);\n", m_iLength);
            memset(m_pszStr, 0, m_iLength);
            //memset(m_pszStr, 0, m_iCapacity);
        }
        delete[] m_pszStr;
    }
    else if ((Flags & (eHasReffered
#ifndef CMAASTRING_HAS_ADD_REF_DATA_PTR
        | eHasAddRefData
#endif
        )))
    {
        DerefReffered();
    }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
    else if ((Flags & eCryptoKey_) && m_pszStr == CMAASTRING_UNALLOC_PTR)
    {
        ZeroUnallocTail0;
    }
#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
    if ((Flags & eHasAddRefData))
    {
        DerefAddRefDataPtr();
    }
#endif
#if TOOLSLIB_USE_CMAASTRING64 == 2
    //f2.m_Flags12 = 0;
#endif
}
//---------------------------------------------------------------------------
void CMaaString::Empty() noexcept
{
    //*this = CMaaString {};
    CMaaStringImp* p = m_pImp;
    m_pImp = sp_NullImp;
    if (p != sp_NullImp)
    {
        DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
        if (!p->Release())
        {
#if MANUAL_STRINGS_REFS
            delete p;
#endif
        }
    }
}
//---------------------------------------------------------------------------
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
CMaaString& CMaaString::operator=(const CMaaString64& That) noexcept
{
    return (*this = CMaaString(That));
}
#else
CMaaString& CMaaString::operator=(const CMaaString32& That) noexcept
{
    return (*this = CMaaString(That));
}
#endif
#endif
//---------------------------------------------------------------------------
CMaaString & CMaaString::operator= (const CMaaString & That) noexcept
{
    TOOLSLIB_STR_HELPER_printf("C::op=(c S&)\n");
#ifdef CHK_ERR_NULL_PIMP
    if (!That.m_pImp)
    {
        //printf("CMaaString::operator=(const CMaaString & That) - err NULL\n");
        ((CMaaString&)That).m_pImp = sp_NullImp;
    }
#endif
    AssignImpFast(m_pImp, (CMaaStringImp * &)That.m_pImp, true, MANUAL_STRINGS_REFS);
    return *this;
}
//---------------------------------------------------------------------------
CMaaString & CMaaString::operator= (CMaaString &&That) noexcept
{
    TOOLSLIB_STR_HELPER_printf("S::op=(S&&)\n");
    if  (m_pImp != That.m_pImp)
    {
        AssignImpFast(m_pImp, (CMaaStringImp*&)That.m_pImp, false, MANUAL_STRINGS_REFS);
        That.m_pImp = sp_NullImp;
#if 0
        // v1
        CMaaStringImp * pImp = m_pImp;
        m_pImp = That.m_pImp;
        That.m_pImp = sp_NullImp;
        if (pImp != sp_NullImp)
        {
            DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
            if  (!pImp->Release())
            {
#if MANUAL_STRINGS_REFS
                delete pImp;
#endif
            }
        }
#endif
    }
    return *this;
}
//---------------------------------------------------------------------------
// 01.10.2005
/*
bool CMaaString::operator== (const char* str) const noexcept
{
    //return strcmp ( *this, str ) == 0;
    const char *p = *this;
    const char *e = p + Length();
    while(p < e)
    {
        if  (*p != *str || *str == 0)
        {
            return false;
        }
        p++;
        str++;
    }
    return *str == 0;
}
*/

int_ CMaaString::Compare0(const CMaaString& a, const char* str) noexcept
{
    const char* p = a;
    const char* e = p + a.Length();
    while (p < e && *str)
    {
        if (*p != *str)
        {
            return *p > *str ? 1 : -1;
        }
        p++;
        str++;
    }
    return (p == e && !*str) ? 0 : p == e ? -1 : 1;
}
int_ CMaaString::Compare0(const CMaaString& a, const unsigned char* str) noexcept
{
    const unsigned char* p = (const unsigned char*)(const char*)a;
    const unsigned char* e = p + a.Length();
    while (p < e && *str)
    {
        if (*p != *str)
        {
            return *p > *str ? 1 : -1;
        }
        p++;
        str++;
    }
    return (p == e && !*str) ? 0 : p == e ? -1 : 1;
}

#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
bool CMaaString::operator!= (char* str) const noexcept
#else
bool CMaaString::operator!= (const char* str) const noexcept
#endif
{
    return !(*this == str);
}
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
bool CMaaString::operator== (char* str) const noexcept
#else
bool CMaaString::operator== (const char* str) const noexcept
#endif
{
    const char *p = *this;
    const char *e = p + Length();
    while (p < e && *str)
    {
        if (*p++ != *str++)
        {
            return false;
        }
    }
    return p == e && !*str;
}
bool CMaaString::operator!= (const CMaaConstStr &s) const noexcept
{
    return !(*this == s);
}
bool CMaaString::operator== (const CMaaConstStr &s) const noexcept
{
    return Length() == s.len && CMaaIsMemEq(*this, s.p, s.len);
}

bool CMaaString::operator!= (const CMaaString& That) const noexcept
{
    return !(*this == That);
}

bool CMaaString::operator== (const CMaaString& That) const noexcept
{
#ifndef CMaaStringImp_NOT_NULL_MODE
    return m_pImp == That.m_pImp || (m_pImp && That.m_pImp && *m_pImp == *(That.m_pImp));
#else
    return /*m_pImp && That.m_pImp &&*/ *m_pImp == *(That.m_pImp);
#endif
}

/*
CMaaString::operator const char* () const noexcept
{
    return m_pImp ? m_pImp->m_pszStr : sChar0_1;
}
*/

/*
#error "dup operator cast"
CMaaString::operator const char8_t* () const noexcept
{
    return m_pImp ? (const char8_t *)m_pImp->m_pszStr : (const char8_t *)sChar0_1;
}
*/

/*
int CMaaString::Length() const noexcept
{
    return m_pImp ? m_pImp->m_iLength : 0;
}
*/

/*
bool CMaaString::IsValid() const
{
    return m_pImp != nullptr;
}
*/
int_ CMaaString::Compare(const CMaaString &aa, const CMaaString &bb, int_ Type /*1 - unicode, 2 - utf8*/)
#ifndef CMAASTRING_HAS_ADD_DATA
    noexcept
#else
    noexcept(noexcept_new)
#endif
{
    if (Type == 2)
    {
        const unsigned char* p1 = (const unsigned char*)(const char*)aa;
        const unsigned char* p2 = (const unsigned char*)(const char*)bb;
        const int aalen = aa.Length();
        const int bblen = bb.Length();
        const unsigned char* e1 = p1 + aalen;
        const unsigned char* e2 = p2 + bblen;
        const unsigned char* e11 = aalen <= bblen ? e1 : p1 + bblen;
        while (p1 < e11)
        {
            _dword u1, u2;

            if (p1 + 1 < e1 && (p1[0] & 0xE0) == 0xC0 && (p1[1] & 0xC0) == 0x80)
            {
                u1 = (p1[1] & 0x3f) | ((p1[0] & 0x1f) << 6);
                p1 += 2;
            }
            else if (p1 + 2 < e1 && (p1[0] & 0xf0) == 0xe0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80)
            {
                u1 = (p1[2] & 0x3f) | ((p1[1] & 0x3f) << 6) | ((p1[0] & 0x0f) << 12);
                p1 += 3;
            }
            else if (p1 + 3 < e1 && (p1[0] & 0xf8) == 0xf0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80 && (p1[3] & 0xc0) == 0x80)
            {
                u1 = (p1[3] & 0x3f) | ((p1[2] & 0x3f) << 6) | ((p1[1] & 0x3f) << 12) | ((p1[0] & 0x07) << 18);
                p1 += 4;
            }
            else
            {
                u1 = *p1++;
            }

            if (p2 + 1 < e2 && (p2[0] & 0xE0) == 0xC0 && (p2[1] & 0xC0) == 0x80)
            {
                u2 = (p2[1] & 0x3f) | ((p2[0] & 0x1f) << 6);
                p2 += 2;
            }
            else if (p2 + 2 < e2 && (p2[0] & 0xf0) == 0xe0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80)
            {
                u2 = (p2[2] & 0x3f) | ((p2[1] & 0x3f) << 6) | ((p2[0] & 0x0f) << 12);
                p2 += 3;
            }
            else if (p2 + 3 < e2 && (p2[0] & 0xf8) == 0xf0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80 && (p2[3] & 0xc0) == 0x80)
            {
                u2 = (p2[3] & 0x3f) | ((p2[2] & 0x3f) << 6) | ((p2[1] & 0x3f) << 12) | ((p2[0] & 0x07) << 18);
                p2 += 4;
            }
            else
            {
                u2 = *p2++;
            }

            if (u1 == 0x401)
            {
                u1 = 0x416 - 1;
            }
            else if (u1 == 0x451)
            {
                u1 = 0x436;
            }
            else if (u1 >= 0x436 && u1 <= 0x44f)
            {
                u1++;
            }
            else if (u1 >= 0x402/*0x410*/ && u1 <= 0x415)
            {
                u1--;
            }
            else if (u1 == 0x201C || u1 == 0x0AB || u1 == 0x201D || u1 == 0x0BB) // quotes
            {
                u1 = '"';
            }

            if (u2 == 0x401)
            {
                u2 = 0x416 - 1;
            }
            else if (u2 == 0x451)
            {
                u2 = 0x436;
            }
            else if (u2 >= 0x436 && u2 <= 0x44f)
            {
                u2++;
            }
            else if (u2 >= 0x402/*0x410*/ && u2 <= 0x415)
            {
                u2--;
            }
            else if (u2 == 0x201C || u2 == 0x0AB || u2 == 0x201D || u2 == 0x0BB) // quotes
            {
                u2 = '"';
            }

            if (u1 != u2)
            {
                return u1 > u2 ? 1 : -1;
            }
        }
        return p1 == e1 && p2 == e2 ? 0 : p2 == e2 ? 1 : -1;
    }
    CMaaString a = aa;
    CMaaString b = bb;
    if  (Type == 3)
    {
#ifdef CMAASTRING_HAS_ADD_DATA
        _WC_ * wa = a.GetAdditionalDataW(1);
        if  (!wa)
        {
            wa = a.Utf8ToUnicode(-1);
        }
        _WC_ * wb = b.GetAdditionalDataW(1);
        if  (!wb)
        {
            wb = b.Utf8ToUnicode(-1);
        }
        while(1)
        {
            if  (*wa != *wb)
            {
                return *wa > *wb ? 1 : -1;
            }
            if  (!*wa)
            {
                return 0;
            }
            wa++;
            wb++;
        }
#else
        const unsigned char* p1 = (const unsigned char*)(const char*)aa;
        const unsigned char* p2 = (const unsigned char*)(const char*)bb;
        const int aalen = aa.Length();
        const int bblen = bb.Length();
        const unsigned char* e1 = p1 + aalen;
        const unsigned char* e2 = p2 + bblen;
        const unsigned char* e11 = aalen <= bblen ? e1 : p1 + bblen;
        while (p1 < e11)
        {
            _dword u1, u2;

            if (p1 + 1 < e1 && (p1[0] & 0xE0) == 0xC0 && (p1[1] & 0xC0) == 0x80)
            {
                u1 = (p1[1] & 0x3f) | ((p1[0] & 0x1f) << 6);
                p1 += 2;
            }
            else if (p1 + 2 < e1 && (p1[0] & 0xf0) == 0xe0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80)
            {
                u1 = (p1[2] & 0x3f) | ((p1[1] & 0x3f) << 6) | ((p1[0] & 0x0f) << 12);
                p1 += 3;
            }
            else if (p1 + 3 < e1 && (p1[0] & 0xf8) == 0xf0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80 && (p1[3] & 0xc0) == 0x80)
            {
                u1 = (p1[3] & 0x3f) | ((p1[2] & 0x3f) << 6) | ((p1[1] & 0x3f) << 12) | ((p1[0] & 0x07) << 18);
                p1 += 4;
            }
            else
            {
                u1 = *p1++;
            }

            if (p2 + 1 < e2 && (p2[0] & 0xE0) == 0xC0 && (p2[1] & 0xC0) == 0x80)
            {
                u2 = (p2[1] & 0x3f) | ((p2[0] & 0x1f) << 6);
                p2 += 2;
            }
            else if (p2 + 2 < e2 && (p2[0] & 0xf0) == 0xe0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80)
            {
                u2 = (p2[2] & 0x3f) | ((p2[1] & 0x3f) << 6) | ((p2[0] & 0x0f) << 12);
                p2 += 3;
            }
            else if (p2 + 3 < e2 && (p2[0] & 0xf8) == 0xf0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80 && (p2[3] & 0xc0) == 0x80)
            {
                u2 = (p2[3] & 0x3f) | ((p2[2] & 0x3f) << 6) | ((p2[1] & 0x3f) << 12) | ((p2[0] & 0x07) << 18);
                p2 += 4;
            }
            else
            {
                u2 = *p2++;
            }
            if (u1 != u2)
            {
                return u1 > u2 ? 1 : -1;
            }
        }
        return p1 == e1 && p2 == e2 ? 0 : p2 == e2 ? 1 : -1;
#endif
    }
    /*
    if (Type == 2)
    {
        a = ::Utf8ToUnicode(a);
        b = ::Utf8ToUnicode(b);
        Type = 1;
    }
    */
    if  (Type == 1)
    {
        const _WC_ * wa = (_WC_*)(const char *)a;
        const _WC_ * wb = (_WC_*)(const char *)b;
        const int L1 = a.Length() / 2;
        const int L2 = b.Length() / 2;
        const int L = L1 < L2 ? L1 : L2;
        for (int i = 0; i < L; i++)
        {
            _WC_ _a = wa[i];
            _WC_ _b = wb[i];
            for (int_ xx = 0; xx < 2; xx++)
            {
                _WC_ &x = !xx ? _a : _b;
                if  (x == 0x401)
                {
                    x = 0x416 - 1;
                }
                else if (x == 0x451)
                {
                    x = 0x436;
                }
                else if (x >= 0x436 && x <= 0x44f)
                {
                    x++;
                }
                else if (x >= 0x402/*0x410*/ && x <= 0x415)
                {
                    x--;
                }
                else if (x == 0x201C || x == 0x0AB || x == 0x201D || x == 0x0BB) // quotes
                {
                    x = '"';
                }
            }
            if  (_a != _b)
            {
                return _a > _b ? 1 : -1;
            }
            /*
            if   (wa[i] != wb[i])
            {
                if ((wa[i] != 0x401 && wb[i] != 0x401 && wa[i] != 0x451 && wb[i] != 0x451) ||
                   ( (wa[i] == 0x401 || wb[i] == 0x451) && (wb[i] == 0x401 || wb[i] == 0x451)) )
                {
                    return wa[i] > wb[i] ? 1 : -1;
                }
                else
                {
                    if (wa[i] == 0x401)
                    {
                        return (wb[i] <= 0x415) ? 1 : -1;
                    }
                    else if (wb[i] == 0x401)
                    {
                        return (wa[i] > 0x415) ? 1 : -1;
                    }
                    else if (wa[i] == 0x451)
                    {
                        return (wb[i] <= 0x435) ? 1 : -1;
                    }
                    else //if (wb[i] == 0x451)
                    {
                        return (wa[i] > 0x435) ? 1 : -1;
                    }
                }
            }
            */
        }
        return L1 > L2 ? 1 : L1 == L2 ? 0 : -1;
    }
    return Compare(a, b);
}
int_ CMaaString::Compare(const CMaaString &a, const CMaaString &b) noexcept
{
    const int L1 = a.Length();
    const int L2 = b.Length();
    const int L = L1 < L2 ? L1 : L2;
    const char * p1 = a;
    const char * p2 = b;
    for (int i = 0; i < L; i++)
    {
        if  (p1[i] != p2[i])
        {
            return p1[i] > p2[i] ? 1 : -1;
        }
    }
    return L1 > L2 ? 1 : L1 == L2 ? 0 : -1;
}

bool CMaaString::operator < (const CMaaString & That) const noexcept
{
    return (That > *this);
}

bool CMaaString::operator > (const CMaaString & That) const noexcept
{
    const unsigned char * p1 = (const unsigned char *)(const char *)*this;
    const unsigned char * p2 = (const unsigned char *)(const char *)That;
    const int Len1 = Length();
    const int Len2 = That.Length();
    const int Len = Len1 < Len2 ? Len1 : Len2;
    const unsigned char * p1e = p1 + Len;
    while(p1 < p1e)
    {
        if  (*p1 != *p2)
        {
            return *p1 > *p2;
        }
        p1++;
        p2++;
    }
    return Len1 > Len2;
}

bool CMaaString::operator <= (const CMaaString & That) const noexcept
{
    return (That >= *this);
}

bool CMaaString::operator >= (const CMaaString & That) const noexcept
{
    const unsigned char * p1 = (const unsigned char *)(const char *)*this;
    const unsigned char * p2 = (const unsigned char *)(const char *)That;
    const int Len1 = Length();
    const int Len2 = That.Length();
    const int Len = Len1 < Len2 ? Len1 : Len2;
    const unsigned char * p1e = p1 + Len;
    while(p1 < p1e)
    {
        if  (*p1 != *p2)
        {
            return *p1 > *p2;
        }
        p1++;
        p2++;
    }
    return Len1 >= Len2;
}



/*
char CMaaString::operator [] (size_t Index) const
{
     return m_pImp->m_pszStr[Index];
}
*/
#ifdef CMAASTRING_HAS_ADD_DATA
//---------------------------------------------------------------------------
const char * CMaaString::SetAdditionalData(const char * ptr, int_ length, int_ bDefaultNullAccept) noexcept(noexcept_new)
{
    const char * ret = m_pImp != sp_NullImp ? m_pImp->SetAdditionalData(ptr, length) : nullptr;
    if  (!ret && bDefaultNullAccept <= 0)
    {
        if (bDefaultNullAccept < 0 || ptr)
        {
            //sChar0_3[0] = 0;
            ret = sChar0_3;
        }
    }
    return ret;
}
//---------------------------------------------------------------------------
_WC_ * CMaaString::SetAdditionalData(const _WC_ * ptr, int_ length, int_ bDefaultNullAccept) noexcept(noexcept_new)
{
    _WC_ * ret = (_WC_ *)SetAdditionalData((const char *)ptr, (int_)sizeof(_WC_) * length, 1);
    if  (!ret && bDefaultNullAccept <= 0)
    {
        if (bDefaultNullAccept < 0 || ptr)
        {
            //*(_WC_*)sChar0_3 = 0;
            ret = (_WC_*)sChar0_3;
        }
    }
    return ret;
}
//---------------------------------------------------------------------------
const char * CMaaString::GetAdditionalDataA(int_ bDefaultNullAccept, int_ * pchsize) const noexcept
{
    if  (pchsize)
    {
        *pchsize = 0;
    }
    const char * ret = PIMP(m_pImp) ? m_pImp->GetAdditionalData(pchsize) : nullptr;
    if  (!ret && bDefaultNullAccept <= 0)
    {
        if  (bDefaultNullAccept <= 0)
        {
            //sChar0_3[0] = 0;
            ret = sChar0_3;
            if  (pchsize)
            {
                *pchsize = 1;
            }
        }
    }
    return ret;
}
//---------------------------------------------------------------------------
_WC_ * CMaaString::GetAdditionalDataW(int_ bDefaultNullAccept, int_ * pwsize) const noexcept
{
    int_ sz = 0;
    _WC_ * ret = PIMP(m_pImp) ? (_WC_ *)m_pImp->GetAdditionalData(&sz) : nullptr;
    if  (!ret && bDefaultNullAccept <= 0)
    {
        if  (bDefaultNullAccept <= 0)
        {
            //*(_WC_ *)sChar0_3 = 0;
            ret = (_WC_*)sChar0_3;
            sz = (int_)sizeof(_WC_);
        }
    }
    if  (pwsize)
    {
        *pwsize = sz / (int_)sizeof(_WC_);
    }
    return ret;
}
//---------------------------------------------------------------------------
const char * CMaaString::CMaaStringImp::SetAdditionalData(const char * ptr, int_ length) noexcept(noexcept_new)
{
    if  ((TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eHasAddData >> 8)))
    {
        if ((TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eCryptoKey_ >> 8)) && m_pszData)
        {
            memset(m_pszData, 0, m_iDataLength);
        }
        delete[] m_pszData;
    }
    m_pszData = nullptr;
    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~(eHasAddData >> 8));
    if  (ptr)
    {
        m_pszData = TL_NEW char[length];
    }
    if  (m_pszData)
    {
        memcpy(m_pszData, ptr, m_iDataLength = length);
    }
    else
    {
        m_iDataLength = 0;
    }
    TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eHasAddData >> 8));
    return m_pszData;
}
//---------------------------------------------------------------------------
const char* CMaaString::CMaaStringImp::GetAdditionalData(int_* pchsize) const noexcept
{
    if  ((TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eHasAddData >> 8)))
    {
        if  (pchsize)
        {
            *pchsize = m_iDataLength;
        }
        return m_pszData;
    }
    if  (pchsize)
    {
        *pchsize = 0;
    }
    return nullptr;
}
//---------------------------------------------------------------------------
int_ CMaaString::CMaaStringImp::GetAdditionalLength() const noexcept
{
    return (TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eHasAddData >> 8)) ? m_iDataLength : 0;
}
//---------------------------------------------------------------------------
#endif
#if TOOLSLIB_CMAASTRING64 <= 0
CMaaString::CMaaString(const void* pMem, size_t Len, _e1_ Flags, int AddCapacity) noexcept(noexcept_new)
{
    if (Len > 0)
    {
        //try
        {
            m_pImp = !pMem ? TL_NEW CMaaStringImp(Len, Flags, AddCapacity) : TL_NEW CMaaStringImp(pMem, Len, (_e0)Flags /*eMemString / 16 / 32*/);
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        /*
        catch (...)
        {
            if (!pMem)
            {
                throw;
            }
        }
        */
    }
    m_pImp = sp_NullImp;
}

CMaaString::CMaaString(const void* pMem, size_t Len, _e0 Flags) noexcept
{
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, Flags);
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = sp_NullImp;
}
#endif

#if 0
CMaaString::CMaaString(const void* pMem, size_t Len, _e1 /*Flags*/) noexcept
{
    Len = int_strlen((const char*)pMem);
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, eMemString0);
            if (!m_pImp || m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(m_pImp);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = nullptr;
}


CMaaString::CMaaString(const void* pMem, size_t Len, _e2 /*Flags*/) noexcept
{
    Len = int_strlen((const char*)pMem);
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, eROMemString0);
            if (!m_pImp || m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(m_pImp);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = nullptr;
}
#endif

CMaaString::CMaaString(const void* pMem, _e1 /*Flags*/) noexcept
{
    const int Len = int_strlen((const char*)pMem);
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, eMemString0);
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = sp_NullImp;
}

CMaaString::CMaaString(const void* pMem, _e2 /*Flags*/) noexcept
{
    const int Len = int_strlen((const char *)pMem);
    if (Len > 0)
    {
        try
        {
            m_pImp = TL_NEW CMaaStringImp(pMem, Len, eROMemString0);
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = sp_NullImp;
}
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
//CMaaString::CMaaString(const CMaaConstStr &s, _e2 /*Flags*/) noexcept
CMaaString::CMaaString(char* pMem, _e2 /*Flags*/) noexcept
{
    const int Len = int_strlen(pMem);
    if (Len > 0)
    {
        try
        {
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr0
            m_pImp = TL_NEW CMaaStringImp(s.p, s.len, eROMemString0);
#else
            m_pImp = TL_NEW CMaaStringImp((const void *)pMem, Len, eROMemString0);
#endif
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            if (m_pImp->m_iLength)
            {
                INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                return;
            }
            delete m_pImp;
        }
        catch (...)
        {
        }
    }
    m_pImp = sp_NullImp;
}
#endif
//---------------------------------------------------------------------------
CMaaString & CMaaString::operator += (const CMaaString & That) noexcept(noexcept_new)
{
    if  (m_pImp == sp_NullImp)
    {
        if  (That.m_pImp != sp_NullImp)
        {
            if  (That.m_pImp->IsMemoryNotROString()) // is memory, but not const string
            {
                * this = CMaaString(That.m_pImp->m_pszStr, That.m_pImp->m_iLength, That.m_pImp->GetUtf1632Flags()); // That.NewCopy();
            }
            else
            {
                * this = That;
            }
        }
    }
    else if (That.m_pImp != sp_NullImp)
    {
        if (m_pImp->IsSingleOwner() && 
            ((!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)) && m_pImp->m_iCapacity >= m_pImp->m_iLength + That.m_pImp->m_iLength)
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
            || (m_pImp->m_pszStr == CMAASTRING_UNALLOC_PTR_(m_pImp) && m_pImp->m_iLength + That.m_pImp->m_iLength <= CMaaStringImp::GetMaxUnAllocLength())
#endif
            )
           )
        {
#ifndef CMaaStringImp_NOT_NULL_MODE
            m_pImp->ClearHash();
#else
            ClearHash();
#endif
            memmove(m_pImp->m_pszStr + m_pImp->m_iLength, That.m_pImp->m_pszStr, That.m_pImp->m_iLength);
            m_pImp->m_iLength += That.m_pImp->m_iLength;
            TOOLSLIB_ATOMIC_OP_EQ(m_pImp->f2.a12.m_Flags1, fetch_or, |=, ((That.m_pImp->GetUtf1632Flags() & eCryptoKey) >> 8));
            if (!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)))
            {
                *(_qword *)(m_pImp->m_pszStr + m_pImp->m_iLength) = 0;
            }
        }
        else
        {
            CMaaString NewString(nullptr, (size_t)m_pImp->m_iLength + (size_t)That.m_pImp->m_iLength, (_e1_)(m_pImp->GetUtf1632Flags() | (That.m_pImp->GetUtf1632Flags() & eCryptoKey)) /*eNotInitMem*/, 256);
            if (NewString.IsValid())
            {
                memcpy(NewString.m_pImp->m_pszStr, m_pImp->m_pszStr, m_pImp->m_iLength);
                memcpy(NewString.m_pImp->m_pszStr + m_pImp->m_iLength, That.m_pImp->m_pszStr, That.m_pImp->m_iLength);
                //NewString.m_pImp->SetUtf1632Flags(m_pImp->GetUtf1632Flags());
                *this = std::move(NewString);
            }
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::operator -= (int n) noexcept(noexcept_new)
{
    const int N = n >= 0 ? n : -n;
    const int Len = Length() - N;
    if (TruncateRWS(Len) < 0)
    {
        *this = n >= 0 ? CMaaString(m_pImp->m_pszStr, Len, m_pImp->GetUtf1632Flags()) : CMaaString(*this, 0, Len); //RefLeft(Len);
    }
    return *this;
}
//---------------------------------------------------------------------------
CMaaString & CMaaString::Add(const void* pMem, int Len) noexcept(noexcept_new) // slow
{
    if (Len > 0)
    {
        if (m_pImp == sp_NullImp)
        {
            *this = CMaaString(pMem, Len);
        }
        else
        {
            if (m_pImp->IsSingleOwner() &&
                ((!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)) && m_pImp->m_iCapacity >= m_pImp->m_iLength + Len)
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
                || (m_pImp->m_pszStr == CMAASTRING_UNALLOC_PTR_(m_pImp) && m_pImp->m_iLength + Len <= CMaaStringImp::GetMaxUnAllocLength())
#endif
                 )
                )
            {
#ifndef CMaaStringImp_NOT_NULL_MODE
                m_pImp->ClearHash();
#else
                ClearHash();
#endif
                memmove(m_pImp->m_pszStr + m_pImp->m_iLength, pMem, Len);
                m_pImp->m_iLength += Len;
                //m_pImp->f2.f12.m_Flags1 |= ((That.m_pImp->GetUtf1632Flags() & eCryptoKey) >> 8);
                if (!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)))
                {
                    *(_qword*)(m_pImp->m_pszStr + m_pImp->m_iLength) = 0;
                }
            }
            else
            {
                CMaaString NewString(nullptr, (size_t)m_pImp->m_iLength + (size_t)Len, (_e1_)m_pImp->GetUtf1632Flags() /*eNotInitMem*/, 256);
                if (NewString.IsValid())
                {
                    memcpy(NewString.m_pImp->m_pszStr, m_pImp->m_pszStr, m_pImp->m_iLength);
                    memcpy(NewString.m_pImp->m_pszStr + m_pImp->m_iLength, pMem, Len);
                    //NewString.m_pImp->SetUtf1632Flags(m_pImp->GetUtf1632Flags());
                    *this = std::move(NewString);
                }
            }
        }
    }
    return *this;
}
//---------------------------------------------------------------------------
CMaaString & CMaaString::operator += (const char * szString) noexcept(noexcept_new)
{
    if (szString)
    {
        return Add(szString, int_strlen(szString));
    }
    return *this;
}
//---------------------------------------------------------------------------
CMaaString::Helper CMaaString::operator + (const CMaaString & That) const noexcept(noexcept_new)
{
    TOOLSLIB_STR_HELPER_printf("H S::op+(c S&)\n");
    CMaaString tmp(*this);
    tmp += That;
    return tmp;
}
//---------------------------------------------------------------------------
CMaaString::Helper CMaaString::operator + (const char * szString) const noexcept(noexcept_new)
{
    if (szString)
    {
        CMaaString tmp(*this);
        tmp += szString;
        TOOLSLIB_STR_HELPER_printf("H S::op+(c c*)\n");
        return tmp;
    }
    TOOLSLIB_STR_HELPER_printf("H S::op+(c c* nullptr)\n");
    return *this;
}
//---------------------------------------------------------------------------
CMaaString::Helper CMaaString::operator - (int n) const noexcept(noexcept_new)
{
    TOOLSLIB_STR_HELPER_printf("H S::op-(n)\n");
    return RemoveFromRight(n);
}
//---------------------------------------------------------------------------
#ifndef __unix__
#define GET_NUMBER( var )                         \
{                                                 \
    var = -1;                                    \
    while( '0' <= c && c <= '9' )               \
    {                                            \
        if  ( var == -1 )                      \
        {                                       \
            var = 0;                           \
        }                                       \
        var = var * 10 + ( int_ )( c - '0' );    \
        c = Format [ ++i ];                     \
    }                                            \
    if  ( c == '*' && var == -1 )                 \
    {                                            \
        var = -2;                               \
        c = Format [ ++i ];                     \
    }                                            \
}
//---------------------------------------------------------------------------
#define OUTPUT( arg )                             \
{                                                 \
    if  ( width == -2 )                         \
    {                                            \
        if  ( precision == -2 )                \
        {                                       \
            sprintf ( Buffer, Format + Start, arg_width, arg_precision, arg ); \
        }                                       \
        else                                    \
        {                                       \
            sprintf ( Buffer, Format + Start, arg_width, arg );        \
        }                                       \
    }                                            \
    else                                         \
    {                                            \
        if  ( precision == -2 )                \
        {                                       \
            sprintf ( Buffer, Format + Start, arg_precision, arg );    \
        }                                       \
        else                                    \
        {                                       \
            sprintf ( Buffer, Format + Start, arg );                   \
        }                                       \
    }                                            \
}
#else
#define GET_NUMBER(var) {var=-1;while('0'<=c&&c<='9'){if(var==-1){var=0;}var=var*10+(int_)(c-'0');c=Format[++i];}if(c=='*'&&var==-1){var=-2;c=Format[++i];}}
//---------------------------------------------------------------------------
#define OUTPUT(arg) {if(width==-2){if(precision==-2){sprintf(Buffer,Format+Start,arg_width,arg_precision,arg);}else{sprintf(Buffer,Format+Start,arg_width,arg);}}else{if(precision==-2){sprintf(Buffer,Format+Start,arg_precision,arg);}else{sprintf(Buffer,Format+Start,arg);}}}
#endif
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_LESS_MEMORY
#define MAX_proc_s_LEN (64*1024-1)  // added by MAA 17.02.2003
#else
//#define MAX_proc_s_LEN (10*64*1024-1)  // added by MAA 17.02.2003
#define MAX_proc_s_LEN (64*1024-1)  // 23.12.2024 - it is not need to use large buffers (large buffer it is not required: '%s', '%S', '%m' are not limited)
#endif
#define MAX_format_s_LEN (32*1024-1)   // for preallocated buffers

#ifdef _WIN32
#if TOOLSLIB_CMAASTRING64 <= 1
void CMaaAccessMemoryCheckFailed(const void * ptr, int_ len, const char * funcName, const char * SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    _mkdir("logs");
    FILE * f = fopen("logs" szFILESYSTEM_SLASH "memory_access.txt", "a+b");
    if  (f)
    {
        const time_t t = time(nullptr);
        tm t0;
        const tm * p = localtime(&t);
        if  (p)
        {
            memcpy(&t0, p, sizeof(t0));
        }
        else
        {
            memset(&t0, 0, sizeof(t0));
        }
        fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: %s: memory access error at ptr=%p, len=%lld, SrcFile=%s, SrcLine=%d\n", t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec, funcName, ptr, (long long)len, SrcFile, SrcLine);
        fclose(f);
    }
#endif
}

bool __is_bad_read_ptr(const void * ptr, int_ len, bool bAccert, const char * SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if  (IsBadReadPtr(ptr, (UINT_PTR)len))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_read_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_write_ptr(void * ptr, int_ len, bool bAccert, const char * SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if  (IsBadWritePtr(ptr, (UINT_PTR)len))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_write_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_string_ptr(const char * ptr, int_ maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if  (IsBadStringPtrA(ptr, maxlen >= 0 ? (UINT_PTR)maxlen : 0x7fffffff))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_string_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_wstring_ptr(const wchar_t * ptr, int_ maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if  (IsBadStringPtrW((LPCWSTR)ptr, maxlen >= 0 ? (UINT_PTR)maxlen : 0x7fffffff))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_wstring_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_wc2string_ptr(const _WC_ * ptr, int_ maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if  (IsBadStringPtrW((LPCWSTR)ptr, maxlen >= 0 ? (UINT_PTR)maxlen : 0x7fffffff))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_wc2string_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

//#else
void CMaaAccessMemoryCheckFailed(const void* ptr, size_t len, const char* funcName, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept
{
#ifdef _WIN32
    _mkdir("logs");
    FILE* f = fopen("logs" szFILESYSTEM_SLASH "memory_access.txt", "a+b");
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
        fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: %s: memory access error at ptr=%p, len=%lld, SrcFile=%s, SrcLine=%d\n", t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec, funcName, ptr, (long long)len, SrcFile, SrcLine);
        fclose(f);
    }
#endif
}

bool __is_bad_read_ptr(const void* ptr, size_t len, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if (IsBadReadPtr(ptr, (UINT_PTR)len))
    {
        if (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_read_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_write_ptr(void* ptr, size_t len, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if (IsBadWritePtr(ptr, (UINT_PTR)len))
    {
        if (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_write_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_string_ptr(const char* ptr, size_t maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if (IsBadStringPtrA(ptr, maxlen >= 0 ? (UINT_PTR)maxlen : 0x7fffffff))
    {
        if (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_string_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_wstring_ptr(const wchar_t* ptr, size_t maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if (IsBadStringPtrW((LPCWSTR)ptr, maxlen >= 0 ? (UINT_PTR)maxlen : 0x7fffffff))
    {
        if (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_wstring_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_wc2string_ptr(const _WC_* ptr, size_t maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef _WIN32
    if (IsBadStringPtrW((LPCWSTR)ptr, maxlen >= 0 ? (UINT_PTR)maxlen : 0x7fffffff))
    {
        if (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_wc2string_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}
#else
#endif

#else

#ifdef __linux__
//#if (TOOLSLIB_USE_CMAASTRING64 < 2) || (TOOLSLIB_CMAASTRING64 == 0)
#if TOOLSLIB_CMAASTRING64 <= 1
static thread_local_ int_ jmpbuf_installed;
static thread_local_ sigjmp_buf jmpbuf;
//static
extern "C" void PtrTestHandler(int_ nSig)
{
    if  (jmpbuf_installed)
    siglongjmp(jmpbuf, 1);
}

//extern char gChZ00000a;
//char gChZ00000a;
int_ IsBadReadPtr(const void* ptr, size_t s) noexcept
{
    size_t i;
    volatile const char* p = (volatile const char*)ptr;
    //char a = '\0';
    int_ r;
    void (*PrevHandler)(int_);
    jmpbuf_installed = 0;
    PrevHandler = signal(SIGSEGV, PtrTestHandler);
    jmpbuf_installed = 1;
    int_ v = sigsetjmp(jmpbuf, 1);
    if  (v)
    {
        r = 1;
        goto Ret;
    }

    for (i = 0; i != s; i++)
    {
        *p++;
        //a += *p++;
    }
    //gChZ00000a = a;
    r = 0;

    Ret:
    jmpbuf_installed = 0;
    signal(SIGSEGV, PrevHandler);

    return r;
}

#ifdef __linux__
extern "C"
{
    void CMaaTestMemForWrite_unix(void * p, _uqword len) noexcept;
}
int_ IsBadWritePtr(void* ptr, size_t s) noexcept
{
    int_ r;
    void (*PrevHandler)(int_);
    jmpbuf_installed = 0;
    PrevHandler = signal(SIGSEGV, PtrTestHandler);
    jmpbuf_installed = 1;
    int_ v = sigsetjmp(jmpbuf, 1);
    if  (v)
    {
        r = 1;
        goto Ret;
    }

    CMaaTestMemForWrite_unix(ptr, s);
    r = 0;

    Ret:
    jmpbuf_installed = 0;
    signal(SIGSEGV, PrevHandler);

    return r;
}
#else
extern char gChZ00000;
char gChZ00000 = '\0';
int_ IsBadWritePtr(void* ptr, size_t s) noexcept
{
    size_t i;
    //volatile char* p = (volatile char*)ptr;
    char* p = (char*)ptr;
    char ch = gChZ00000;
    int_ r;
    void (*PrevHandler)(int_);
    jmpbuf_installed = 0;
    PrevHandler = signal(SIGSEGV, PtrTestHandler);
    jmpbuf_installed = 1;
    int_ v = sigsetjmp(jmpbuf, 1);
    if  (v)
    {
        r = 1;
        goto Ret;
    }

    for (i = 0; i != s; i++)
    {
        //*p++ += '\0';
        p[i] += ch;
    }
    r = 0;

    Ret:
    jmpbuf_installed = 0;
    signal(SIGSEGV, PrevHandler);

    return r;
}
#endif

int_ IsBadStringPtrA(const char* ptr, size_t s) noexcept
{
    size_t i;
    volatile const char* p = (volatile const char*)ptr;
    int_ r;
    void (*PrevHandler)(int_);
    jmpbuf_installed = 0;
    PrevHandler = signal(SIGSEGV, PtrTestHandler);
    jmpbuf_installed = 1;
    int_ v = sigsetjmp(jmpbuf, 1);
    if  (v)
    {
        r = 1;
        goto Ret;
    }

    for (i = 0; i != s; i++)
    {
        if  (!p[i])
        {
            break;
        }
    }
    r = 0;

    Ret:
    jmpbuf_installed = 0;
    signal(SIGSEGV, PrevHandler);

    return r;
}

int_ IsBadStringPtrW(const wchar_t* ptr, size_t s) noexcept
{
    size_t i;
    volatile const wchar_t* p = (volatile const wchar_t*)ptr;
    int_ r;
    void (*PrevHandler)(int_);
    jmpbuf_installed = 0;
    PrevHandler = signal(SIGSEGV, PtrTestHandler);
    jmpbuf_installed = 1;
    int_ v = sigsetjmp(jmpbuf, 1);
    if  (v)
    {
        r = 1;
        goto Ret;
    }

    for (i = 0; i != s; i++)
    {
        if  (!p[i])
        {
            break;
        }
    }
    r = 0;

    Ret:
    jmpbuf_installed = 0;
    signal(SIGSEGV, PrevHandler);

    return r;
}

int_ IsBadStringPtrWC2(const _WC_ * ptr, size_t s) noexcept
{
    size_t i;
    volatile const _WC_ * p = (volatile const _WC_ *)ptr;
    int_ r;
    void (*PrevHandler)(int_);
    jmpbuf_installed = 0;
    PrevHandler = signal(SIGSEGV, PtrTestHandler);
    jmpbuf_installed = 1;
    int_ v = sigsetjmp(jmpbuf, 1);
    if  (v)
    {
        r = 1;
        goto Ret;
    }

    for (i = 0; i != s; i++)
    {
        if  (!p[i])
        {
            break;
        }
    }
    r = 0;

    Ret:
    jmpbuf_installed = 0;
    signal(SIGSEGV, PrevHandler);

    return r;
}

void CMaaAccessMemoryCheckFailed(const void * ptr, int len, const char * funcName, const char * SrcFile = "unknown", int_ SrcLine = -1) noexcept
{
#ifdef __linux__
    ::mkdir("logs", S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
    FILE * f = fopen("logs" szFILESYSTEM_SLASH "memory_access.txt", "a+b");
    if  (f)
    {
        time_t t = time(nullptr);
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
        fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: %s: memory access error at ptr=%p, len=%lld, SrcFile=%s, SrcLine=%d\n", t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec, funcName, ptr, (long long)len, SrcFile, SrcLine);
        fclose(f);
    }
#endif
}

bool __is_bad_read_ptr(const void* ptr, int_ len, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef __linux__
    if  (IsBadReadPtr(ptr, len))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_read_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_read_ptr(const void* ptr, size_t len, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef __linux__
    if (IsBadReadPtr(ptr, len))
    {
        if (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_read_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_write_ptr(void* ptr, int len, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef __linux__
    if  (IsBadWritePtr(ptr, len))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, len, "__is_bad_write_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_string_ptr(const char * ptr, int maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef __linux__
    if  (IsBadStringPtrA(ptr, maxlen >= 0 ? maxlen : 0x7fffffff))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_string_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_wstring_ptr(const wchar_t * ptr, int maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef __linux__
    if  (IsBadStringPtrW((wchar_t*)ptr, maxlen >= 0 ? maxlen : 0x7fffffff))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_wstring_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}

bool __is_bad_wc2string_ptr(const _WC_ * ptr, int maxlen, bool bAccert, const char* SrcFile, int_ SrcLine) noexcept
{
#ifdef __linux__
    if  (IsBadStringPtrWC2(ptr, maxlen >= 0 ? maxlen : 0x7fffffff))
    {
        if  (bAccert)
        {
            CMaaAccessMemoryCheckFailed(ptr, maxlen, "__is_bad_wc2string_ptr()", SrcFile, SrcLine);
        }
        return true;
    }
#endif
    return false;
}
#endif
#endif
#endif

#if CHECK_POINTERS == 0
#define _is_bad_read_ptr(ptr, len) 0
#define _is_bad_write_ptr(ptr, len) 0
#define _is_bad_string_ptr(ptr, maxlen) 0
#define _is_bad_wstring_ptr(ptr, maxlen) 0
#define _is_bad_wc2string_ptr(ptr, maxlen) 0
#else
#define _is_bad_read_ptr(ptr, len) __is_bad_read_ptr(ptr, len, CHECK_POINTERS == 2, SrcFile, SrcLine)
#define _is_bad_write_ptr(ptr, len) __is_bad_write_ptr(ptr, len, CHECK_POINTERS == 2, SrcFile, SrcLine)
#define _is_bad_string_ptr(ptr, maxlen) __is_bad_string_ptr(ptr, maxlen, CHECK_POINTERS == 2, SrcFile, SrcLine)
#define _is_bad_wstring_ptr(ptr, maxlen) __is_bad_wstring_ptr(ptr, maxlen, CHECK_POINTERS == 2, SrcFile, SrcLine)
#define _is_bad_wc2string_ptr(ptr, maxlen) __is_bad_wc2string_ptr(ptr, maxlen, CHECK_POINTERS == 2, SrcFile, SrcLine)
#endif

/*
char* mysprintf64(char* txt, _qword x)
{
    sprintf(txt, "%lld", (long long)x);
    return txt;
}
char* mysprintf64(char* txt, _uqword x)
{
    sprintf(txt, "%llu", (unsigned long long)x);
    return txt;
}
*/
struct CMaaTmpSprintfBuffer : public CMaaDLink
{
    CMaaPtr_<char, 1> m_Buffer;

    CMaaTmpSprintfBuffer(size_t len) // = MAX_proc_s_LEN + 1)
    :   m_Buffer(len)
    {
    }
    ~CMaaTmpSprintfBuffer() = default;
    CMaaTmpSprintfBuffer(const CMaaTmpSprintfBuffer&) = delete;
    CMaaTmpSprintfBuffer & operator = (const CMaaTmpSprintfBuffer&) = delete;
};
struct CMaaTmpSprintf2StringsArray : public CMaaDLink
{
    struct Rec
    {
        CMaaString s;
        int_ l, r;
        char c0;
    };
    CMaaPtr_<Rec, 1> m;

    CMaaTmpSprintf2StringsArray(int n)
    :   m(n)
    {
    }
    ~CMaaTmpSprintf2StringsArray() = default;
    CMaaTmpSprintf2StringsArray(const CMaaTmpSprintf2StringsArray&) = delete;
    CMaaTmpSprintf2StringsArray& operator = (const CMaaTmpSprintf2StringsArray&) = delete;
};
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf(const char* Format, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    if (!Format)
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullFormatErr));
    }
    if (_is_bad_string_ptr(Format, -1))
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadFormatErr));
    }
    return Sprintf(Format, int_strlen(Format), list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf(CMaaString strFormat, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    return Sprintf(strFormat, strFormat.Length(), list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
void CMaaString::GetRetSprintfBuffers(int a, CMaaTmpSprintfBuffer*& pTmpBuffer, CMaaTmpSprintfBuffer*& pTmpFormatBuffer) noexcept
{
    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    static CMaaDListAD<CMaaTmpSprintfBuffer> m_BuffersList;
    static CMaaDListAD<CMaaTmpSprintfBuffer> m_FormatBuffersList;
    if (a > 0)
    {
        pTmpBuffer = m_BuffersList.GetFromFront();
        pTmpFormatBuffer = m_FormatBuffersList.GetFromFront();
        aLocker_UnLock;
        if (!pTmpBuffer)
        {
            try
            {
                pTmpBuffer = TL_NEW CMaaTmpSprintfBuffer(MAX_proc_s_LEN + 1);
            }
            catch (...)
            {
            }
        }
        if (!pTmpFormatBuffer)
        {
            try
            {
                pTmpFormatBuffer = TL_NEW CMaaTmpSprintfBuffer(MAX_format_s_LEN + 1);
            }
            catch (...)
            {
            }
        }
        return;
    }
    if (a < 0)
    {
        if (pTmpFormatBuffer)
        {
            m_FormatBuffersList.AddAtFront(pTmpFormatBuffer);
        }
        if (pTmpBuffer)
        {
            m_BuffersList.AddAtFront(pTmpBuffer);
        }
    }
    aLocker_UnLock;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf(const char * strFormat, int FormatLen, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    char csBuffer[TOOLSLIB_CS_64K];
    CMaaPtr_<char, -1> Format_(FormatLen + 10 <= MAX_format_s_LEN + 1 ? 0 : FormatLen + 10);

    CMaaTmpSprintfBuffer* pTmpBuffer;
    CMaaTmpSprintfBuffer* pTmpFormatBuffer;
    GetRetSprintfBuffers(1, pTmpBuffer, pTmpFormatBuffer);

    char* Buffer = pTmpBuffer ? (char*)pTmpBuffer->m_Buffer : nullptr;
    char* FormatBuffer = pTmpFormatBuffer ? (char*)pTmpFormatBuffer->m_Buffer : nullptr;

    try
    {
        char Char2[2] = { 0, 0 };
        char i64_txt[64];

        CMaaString S_String;

        CMaaConcatString NewString(csBuffer, sizeof(csBuffer)); // CMaaString --> CMaaConcatString 2.12.2020

        char* Format = FormatLen + 10 <= MAX_format_s_LEN + 1 ? FormatBuffer : Format_;
        if (Buffer && Format)
        {
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
                    NewString.Add(&Format[Start], i - Start);
                    if (i >= FormatLen)
                    {
                        break;
                    }
                }
                Start = i;
                char c = Format[++i];
                if (c == '%')
                {
                    NewString += '%';
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
                GET_NUMBER(width);
                if (c == '.')
                {
                    c = Format[++i];
                    GET_NUMBER(precision);
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

                if  (c != 'l')
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
                                    for ( ; y /= d10; )
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
                                    for ( ; y /= d10; )
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
                                int_ l = (int_)strlen(pChar);
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
                    precision = (warning_int)int_strlen(pChar);

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
                        unsigned char* ptr = va_arg(list, unsigned char*);
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
                            CMaaConcatString bf(Buffer, MAX_proc_s_LEN);
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
                            S_String = (CMaaString)bf;
                        }
                    }
                    CMaa_fallthrough;
                case 'T':
                case 't':
                    if (c == 'T' || c == 't')
                    {
                        time_t t = va_arg(list, time_t);
                        int_ mode = c == 'T' ? va_arg(list, int_) : 0;
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
                        wchar_t* p = va_arg(list, wchar_t*);
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
                        _WC_* p = va_arg(list, _WC_*);
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
#ifndef CHECK_STRINGS
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
#ifdef CHECK_STRINGS
                            if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s64 && CMaaString64::IsBadStrPtr((CMaaString64*)s64))))
#else
                            if (false)
#endif
#else
#ifndef CHECK_STRINGS
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
#ifdef CHECK_STRINGS
                            if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s32 && CMaaString32::IsBadStrPtr((CMaaString32*)s32))))
#else
                            if (false)
#endif
#endif
#else
#ifdef CHECK_STRINGS
                            if (CMaaString::IsBadStrPtr((CMaaString*)s))
#else

#ifndef CMaaStringImp_NOT_NULL_MODE
                            if (false)
#else
                            if (!s->m_pImp)
#endif

#endif
#endif
#ifdef CHECK_STRINGS
                            {
#ifdef _WIN32
                                FILE* f = fopen("c:\\maa\\BadStrPtr.txt", "a+b");
#else
                                FILE* f = fopen("/home/maa/mylogs/BadStrPtr.txt", "a+b");
#endif
                                if (f)
                                {
                                    time_t t = time(nullptr);
                                    tm t0;
                                    tm* p = localtime(&t);
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
                        //OUTPUT(x);
                        //break;
                        BufStrLen =
                        c == 'd' || c == 'i' ? SignedIntegerToString_d<long long>((long long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ?             UnsignedIntegerToString_d<unsigned long long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'x' || c == 'X' ? UnsignedIntegerToString_x<unsigned long long>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(x) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                                               UnsignedIntegerToString_o<unsigned long long>(x, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                        break;
                    }
                    if (Float == 2)
                    {
                        const unsigned long x = va_arg(list, unsigned long);
                        //OUTPUT(x);
                        //break;
                        BufStrLen =
                        c == 'd' || c == 'i' ?   SignedIntegerToString_d<long>((long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ?             UnsignedIntegerToString_d<unsigned long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
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
                    c == 'd' || c == 'i' ?   SignedIntegerToString_d<int_>(Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                    c == 'u' ?             UnsignedIntegerToString_d<uint_>((uint_)Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
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
                        OUTPUT(xld);
                        break;
                    case 2:
                        xd = va_arg(list, double);
                        OUTPUT(xd);
                        break;
                    default: // 1
                        xf = (float)va_arg(list, double); // bc31&vc60 feature: they push double to stack instead of float arguments
                        OUTPUT(xf);
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
                //    OUTPUT(pVoid);
                //    break;
                //case 'z':
                //    xSz = va_arg(list, size_t);
                //    OUTPUT(xSz);
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
                        //OUTPUT(x);
                        Buffer[0] = 0;
                    }
                    else
                    {
                        long long x = va_arg(list, long long);
                        //OUTPUT(x);
                        Buffer[0] = 0;
                    }
                    */
                    break;
                case 'n':
                    {
                        int_ *p = va_arg(list, int_*);
                        *p = (int_)NewString.GetMemoryString().__CharCount();
                        Buffer[BufStrLen = 0] = 0;
                    }
                    break;
                default:
                    OUTPUT(Int);
                }
                Format[i] = BackUp;
                if (c == 'S')
                {
                    if (bFastStrCpy)
                    {
                        NewString.Add(c0, l);
                        NewString += S_String;
                        NewString.Add(c0, r);
                    }
                    else
                    {
                        NewString += S_String;
                    }
                }
                else
                {
                    if (bFastStrCpy)
                    {
                        NewString.Add(c0, l);
                        NewString.Add(pChar, BufStrLen);
                        NewString.Add(c0, r);
                    }
                    else
                    {
                        NewString.Add(Buffer, BufStrLen >= 0 ? BufStrLen : int_strlen(Buffer));
                    }
                }
                Start = i;
            }

        }
        *this = (CMaaString)NewString;
    }
    catch (...)
    {
        Empty();
    }

    if  (pTmpBuffer || pTmpFormatBuffer)
    {
        GetRetSprintfBuffers(-1, pTmpBuffer, pTmpFormatBuffer);
    }

    return * this;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf2(const char* Format, const char* Text, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    if (!Format)
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullFormatErr));
    }
    if (_is_bad_string_ptr(Format, -1))
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadFormatErr));
    }
    if (!Text)
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullTextErr));
    }
    if (_is_bad_string_ptr(Text, -1))
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadTextErr));
    }
    return Sprintf2(Format, int_strlen(Format), Text, int_strlen(Text), list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf2(CMaaString strFormat, const char* Text, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    if (!Text)
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullTextErr));
    }
    if (_is_bad_string_ptr(Text, -1))
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadTextErr));
    }
    return Sprintf2(strFormat, strFormat.Length(), Text, int_strlen(Text), list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf2(const char* Format, CMaaString strText, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    if (!Format)
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullFormatErr));
    }
    if (_is_bad_string_ptr(Format, -1))
    {
        return (*this = CMaaTLGlobalString2(CMaaTLGlobalStrings::eBadFormatErr));
    }
    return Sprintf2(Format, int_strlen(Format), strText, strText.Length(), list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf2(CMaaString strFormat, CMaaString strText, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    return Sprintf2(strFormat, strFormat.Length(), strText, strText.Length(), list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
void CMaaString::GetRetSprintf2Buffers(int a, CMaaTmpSprintfBuffer*& pTmpBuffer, CMaaTmpSprintfBuffer*& pTmpFormatBuffer, CMaaTmpSprintf2StringsArray*& pStringsArray) noexcept
{
    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    static CMaaDListAD<CMaaTmpSprintfBuffer> m_BuffersList;
    static CMaaDListAD<CMaaTmpSprintfBuffer> m_FormatBuffersList;
    static CMaaDListAD<CMaaTmpSprintf2StringsArray> m_StringsArrayList;
    if (a > 0)
    {
        pTmpBuffer = m_BuffersList.GetFromFront();
        pTmpFormatBuffer = m_FormatBuffersList.GetFromFront();
        pStringsArray = m_StringsArrayList.GetFromFront();
        aLocker_UnLock;
        if (!pTmpBuffer)
        {
            try
            {
                pTmpBuffer = TL_NEW CMaaTmpSprintfBuffer(MAX_proc_s_LEN + 1);
            }
            catch (...)
            {
            }
        }
        if (!pTmpFormatBuffer)
        {
            try
            {
                pTmpFormatBuffer = TL_NEW CMaaTmpSprintfBuffer(MAX_format_s_LEN + 1);
            }
            catch (...)
            {
            }
        }
        return;
    }
    if (a < 0)
    {
        if (pTmpFormatBuffer)
        {
            m_FormatBuffersList.AddAtFront(pTmpFormatBuffer);
        }
        if (pTmpBuffer)
        {
            m_BuffersList.AddAtFront(pTmpBuffer);
        }
        if (pStringsArray)
        {
            m_StringsArrayList.AddAtFront(pStringsArray);
        }
    }
    aLocker_UnLock;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Sprintf2(const char* strFormat, int FormatLen, const char* strText, int TextLen, va_list list, int_ SrcLine, const char* SrcFile) noexcept
{
    CMaaPtr_<char, -1> Format_(FormatLen + 10 <= MAX_format_s_LEN + 1 ? 0 : FormatLen + 10);

    CMaaTmpSprintfBuffer* pTmpBuffer;
    CMaaTmpSprintfBuffer* pTmpFormatBuffer;
    CMaaTmpSprintf2StringsArray * pStringsArray;
    GetRetSprintf2Buffers(1, pTmpBuffer, pTmpFormatBuffer, pStringsArray);

    char* Buffer = pTmpBuffer ? (char*)pTmpBuffer->m_Buffer : nullptr;
    char* FormatBuffer = pTmpFormatBuffer ? (char*)pTmpFormatBuffer->m_Buffer : nullptr;
    int_ N = 0;

    try
    {
        char Char2[2] = { 0, 0 };
        char i64_txt[64];
        CMaaString S_String;

        char* Format = FormatLen + 10 <= MAX_format_s_LEN + 1 ? FormatBuffer : Format_;

        if (Buffer && Format) // && Text)
        {
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
                pStringsArray = TL_NEW CMaaTmpSprintf2StringsArray(MaxN);
            }
            CMaaTmpSprintf2StringsArray::Rec* ReplText = pStringsArray->m;

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
                    //NewString += & Format [ Start ];
                    //Format [ i ] = c;
                //}
                Start = i;
                char c = Format[++i];
                if (c == '%')
                {
                    //NewString += "%";
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
                GET_NUMBER(width);
                if (c == '.')
                {
                    c = Format[++i];
                    GET_NUMBER(precision);
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

                if  (c != 'l')
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
                                int_ l = (int_)strlen(pChar);
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
                    precision = (warning_int)int_strlen(pChar);

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
                        unsigned char* ptr = va_arg(list, unsigned char*);
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
                            CMaaConcatString bf(Buffer, MAX_proc_s_LEN);
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
                            S_String = (CMaaString)bf;
                        }
                    }
                    CMaa_fallthrough;
                case 'T':
                case 't':
                    if (c == 'T' || c == 't')
                    {
                        time_t t = va_arg(list, time_t);
                        int_ mode = c == 'T' ? va_arg(list, int_) : 0;
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
                        wchar_t* p = va_arg(list, wchar_t*);
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
                        _WC_* p = va_arg(list, _WC_*);
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
                            S_String = CMaaTLGlobalString2(CMaaTLGlobalStrings::eNullErr);
                        }
                        else
                        {
                            const CMaaString* s = (const CMaaString*)pS;
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
#ifndef CHECK_STRINGS
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
#ifdef CHECK_STRINGS
                            if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s64 && CMaaString64::IsBadStrPtr((CMaaString64*)s64))))
#else
                            if (false)
#endif
#else
#ifndef CHECK_STRINGS
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
#ifdef CHECK_STRINGS
                            if (b || (s && CMaaString::IsBadStrPtr((CMaaString*)s) || (s32 && CMaaString32::IsBadStrPtr((CMaaString32*)s32))))
#else
                            if (false)
#endif
#endif
#else
#ifdef CHECK_STRINGS
                            if (CMaaString::IsBadStrPtr((CMaaString*)s))
#else

#ifndef CMaaStringImp_NOT_NULL_MODE
                            if (false)
#else
                            if (!s->m_pImp)
#endif

#endif
#endif
#ifdef CHECK_STRINGS
                            {
#ifdef _WIN32
                                FILE* f = fopen("c:\\maa\\BadStrPtr.txt", "a+b");
#else
                                FILE* f = fopen("/home/maa/mylogs/BadStrPtr.txt", "a+b");
#endif
                                if (f)
                                {
                                    time_t t = time(nullptr);
                                    tm t0;
                                    tm* p = localtime(&t);
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
                        //OUTPUT(x);
                        //break;
                        BufStrLen =
                        c == 'd' || c == 'i' ?   SignedIntegerToString_d<long long>((long long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ?             UnsignedIntegerToString_d<unsigned long long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'x' || c == 'X' ? UnsignedIntegerToString_x<unsigned long long>(x, Buffer, c0 == '0' && !arg_width ? 2 * (int)sizeof(x) : arg_width, c, c0 == '0' ? '0' : 0, prefix_ch ? c : 0) :
                                               UnsignedIntegerToString_o<unsigned long long>(x, Buffer, arg_width, c0 == '0' ? '0' : 0, prefix_ch);
                        break;
                    }
                    if (Float == 2)
                    {
                        const unsigned long x = va_arg(list, unsigned long);
                        //OUTPUT(x);
                        //break;
                        BufStrLen =
                        c == 'd' || c == 'i' ?   SignedIntegerToString_d<long>((long)x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                        c == 'u' ?             UnsignedIntegerToString_d<unsigned long>(x, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
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
                    c == 'd' || c == 'i' ?   SignedIntegerToString_d<int_>(Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
                    c == 'u' ?             UnsignedIntegerToString_d<uint_>((uint_)Int, Buffer, sign_ch ^ (c2 == ',' ? ',' : 0), arg_width, c0 == '0' ? '0' : 0) :
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
                        OUTPUT(xld);
                        break;
                    case 2:
                        xd = va_arg(list, double);
                        OUTPUT(xd);
                        break;
                    default: // 1
                        xf = (float)va_arg(list, double); // bc31&vc60 feature: they push double to stack instead of float arguments
                        OUTPUT(xf);
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
                //    OUTPUT(pVoid);
                //    break;
                //case 'z':
                //    xSz = va_arg(list, size_t);
                //    OUTPUT(xSz);
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
                        //OUTPUT(x);
                        Buffer[0] = 0;
                    }
                    else
                    {
                        long long x = va_arg(list, long long);
                        //OUTPUT(x);
                        Buffer[0] = 0;
                    }
                    */
                    break;
                case 'n':
                    {
                        int_ *p = va_arg(list, int_*);
                        *p = 0;
                        Buffer[BufStrLen = 0] = 0;
                    }
                    break;
                default:
                    OUTPUT(Int);
                }
                Format[i] = BackUp;
                //NewString += CMaaString ( Buffer );
                if  (c != 'l')
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
                CMaaConcatString NewString(Buffer, MAX_proc_s_LEN + 1, NewStringApproxLen + 100); // CMaaString --> CMaaConcatString 2.12.2020
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
                    NewString.Add(p + j, i - j);
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
                            NewString.Add(ReplText[n].c0, ReplText[n].l);
                            NewString += ReplText[n].s;
                            NewString.Add(ReplText[n].c0, ReplText[n].r);
                        }
                    }
                    else
                    {
                        NewString += '%';
                        if (i < TextLen && p[i] == '%')
                        {
                            i++;
                        }
                    }
                    j = i;
                }
                NewString.Add(p + j, TextLen - j);

                *this = (CMaaString)NewString;
            }
        }
        else // (!Buffer || !Format)
        {
            Empty();
        }
    }
    catch (...)
    {
        Empty();
    }

    if (pStringsArray)
    {
        N = N <= (int_)pStringsArray->m.MaxIndex() ? N : (int_)pStringsArray->m.MaxIndex();
        CMaaTmpSprintf2StringsArray::Rec* ReplText = pStringsArray->m;
        while(--N >= 0)
        {
            ReplText[N].s.Empty();
        }
    }

    if  (pTmpBuffer || pTmpFormatBuffer || pStringsArray) // || pTmpBuffer2)
    {
        GetRetSprintf2Buffers(-1, pTmpBuffer, pTmpFormatBuffer, pStringsArray);
    }

    return * this;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Format(const char* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    Sprintf(format, list);
    va_end(list);
    return *this;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::Format(const char8_t* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    Sprintf((const char*)format, list);
    va_end(list);
    return *this;
}
#endif
CMaaString& CMaaString::Format(CMaaString format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    Sprintf(format, list);
    va_end(list);
    return *this;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Format2(const char* format, const char* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::Format2(const char* format, CMaaString text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::Format2(CMaaString format, const char* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::Format2(CMaaString format, CMaaString text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list);
    va_end(list);
    return *this;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::Format2(const char* format, const char8_t* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, (const char*)text, list);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::Format2(CMaaString format, const char8_t* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, (const char*)text, list);
    va_end(list);
    return *this;
}
#endif
//---------------------------------------------------------------------------
CMaaString& CMaaString::Append(const char* format, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, format);
    a.Sprintf(format, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::Append(const char8_t* format, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, format);
    a.Sprintf((const char*)format, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
#endif
CMaaString& CMaaString::Append(CMaaString format, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, format);
    a.Sprintf(format, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::Append2(const char* format, const char* text, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, text);
    a.Sprintf2(format, text, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
CMaaString& CMaaString::Append2(const char* format, CMaaString text, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, text);
    a.Sprintf2(format, text, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
CMaaString& CMaaString::Append2(CMaaString format, const char* text, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, text);
    a.Sprintf2(format, text, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
CMaaString& CMaaString::Append2(CMaaString format, CMaaString text, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, text);
    a.Sprintf2(format, text, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::Append2(const char* format, const char8_t* text, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, text);
    a.Sprintf2(format, (const char*)text, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
CMaaString& CMaaString::Append2(CMaaString format, const char8_t* text, ...) noexcept
{
    CMaaString a;
    va_list list;
    va_start(list, text);
    a.Sprintf2(format, (const char*)text, list);
    va_end(list);
    try
    {
        (*this) += a;
    }
    catch (...)
    {
    }
    return *this;
}
#endif
//---------------------------------------------------------------------------
CMaaString CMaaString::sFormat(const char* format, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, format);
    Ret.Sprintf(format, list);
    va_end(list);
    return Ret;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString CMaaString::sFormat(const char8_t* format, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, format);
    Ret.Sprintf((const char*)format, list);
    va_end(list);
    return Ret;
}
#endif
CMaaString CMaaString::sFormat(CMaaString format, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, format);
    Ret.Sprintf(format, list);
    va_end(list);
    return Ret;
}
//---------------------------------------------------------------------------
CMaaString CMaaString::sFormat2(const char * format, const char * text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::sFormat2(const char* format, CMaaString text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::sFormat2(CMaaString format, const char * text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::sFormat2(CMaaString format, CMaaString text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list);
    va_end(list);
    return Ret;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString CMaaString::sFormat2(const char* format, const char8_t* text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, (const char*)text, list);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::sFormat2(CMaaString format, const char8_t* text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, (const char*)text, list);
    va_end(list);
    return Ret;
}
#endif
//---------------------------------------------------------------------------
CMaaString& CMaaString::FormatV(const char* format, va_list list) noexcept
{
    return Sprintf(format, list);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::FormatV(const char8_t* format, va_list list) noexcept
{
    return Sprintf((const char*)format, list);
}
#endif
CMaaString& CMaaString::FormatV(CMaaString format, va_list list) noexcept
{
    return Sprintf(format, list);
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::FormatV2(const char* format, const char* text, va_list list) noexcept
{
    return Sprintf2(format, text, list);
}
CMaaString& CMaaString::FormatV2(const char* format, CMaaString text, va_list list) noexcept
{
    return Sprintf2(format, text, list);
}
CMaaString& CMaaString::FormatV2(CMaaString format, CMaaString text, va_list list) noexcept
{
    return Sprintf2(format, text, list);
}
CMaaString& CMaaString::FormatV2(CMaaString format, const char* text, va_list list) noexcept
{
    return Sprintf2(format, text, list);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::FormatV2(const char* format, const char8_t* text, va_list list) noexcept
{
    return Sprintf2(format, (const char*)text, list);
}
CMaaString& CMaaString::FormatV2(CMaaString format, const char8_t* text, va_list list) noexcept
{
    return Sprintf2(format, (const char*)text, list);
}
#endif
//---------------------------------------------------------------------------
CMaaString& CMaaString::_Format(int_ SrcLine, const char* SrcFile, const char* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    Sprintf(format, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::_Format(int_ SrcLine, const char* SrcFile, const char8_t* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    Sprintf((const char *)format, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
#endif
CMaaString& CMaaString::_Format(int_ SrcLine, const char* SrcFile, CMaaString format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    Sprintf(format, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::_Format2(int_ SrcLine, const char* SrcFile, const char* format, const char* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::_Format2(int_ SrcLine, const char* SrcFile, const char* format, CMaaString text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::_Format2(int_ SrcLine, const char* SrcFile, CMaaString format, const char* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::_Format2(int_ SrcLine, const char* SrcFile, CMaaString format, CMaaString text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::_Format2(int_ SrcLine, const char* SrcFile, const char* format, const char8_t* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, (const char*)text, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
CMaaString& CMaaString::_Format2(int_ SrcLine, const char* SrcFile, CMaaString format, const char8_t* text, ...) noexcept
{
    va_list list;
    va_start(list, text);
    Sprintf2(format, (const char*)text, list, SrcLine, SrcFile);
    va_end(list);
    return *this;
}
#endif
//---------------------------------------------------------------------------
CMaaString& CMaaString::_FormatV(int_ SrcLine, const char* SrcFile, const char* format, va_list list) noexcept
{
    return Sprintf(format, list, SrcLine, SrcFile);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::_FormatV(int_ SrcLine, const char* SrcFile, const char8_t* format, va_list list) noexcept
{
    return Sprintf((const char*)format, list, SrcLine, SrcFile);
}
#endif
CMaaString& CMaaString::_FormatV(int_ SrcLine, const char* SrcFile, CMaaString format, va_list list) noexcept
{
    return Sprintf(format, list, SrcLine, SrcFile);
}
//---------------------------------------------------------------------------
CMaaString& CMaaString::_FormatV2(int_ SrcLine, const char* SrcFile, const char* format, const char* text, va_list list) noexcept
{
    return Sprintf2(format, text, list, SrcLine, SrcFile);
}
CMaaString& CMaaString::_FormatV2(int_ SrcLine, const char* SrcFile, const char* format, CMaaString text, va_list list) noexcept
{
    return Sprintf2(format, text, list, SrcLine, SrcFile);
}
CMaaString& CMaaString::_FormatV2(int_ SrcLine, const char* SrcFile, CMaaString format, const char* text, va_list list) noexcept
{
    return Sprintf2(format, text, list, SrcLine, SrcFile);
}
CMaaString& CMaaString::_FormatV2(int_ SrcLine, const char* SrcFile, CMaaString format, CMaaString text, va_list list) noexcept
{
    return Sprintf2(format, text, list, SrcLine, SrcFile);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString& CMaaString::_FormatV2(int_ SrcLine, const char* SrcFile, const char* format, const char8_t* text, va_list list) noexcept
{
    return Sprintf2(format, (const char*)text, list, SrcLine, SrcFile);
}
CMaaString& CMaaString::_FormatV2(int_ SrcLine, const char* SrcFile, CMaaString format, const char8_t* text, va_list list) noexcept
{
    return Sprintf2(format, (const char*)text, list, SrcLine, SrcFile);
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaString CMaaString::_sFormat(int_ SrcLine, const char * SrcFile, const char * format, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, format);
    Ret.Sprintf(format, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString CMaaString::_sFormat(int_ SrcLine, const char* SrcFile, const char8_t* format, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, format);
    Ret.Sprintf((const char *)format, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
#endif
CMaaString CMaaString::_sFormat(int_ SrcLine, const char* SrcFile, CMaaString format, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, format);
    Ret.Sprintf(format, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
//---------------------------------------------------------------------------
CMaaString CMaaString::_sFormat2(int_ SrcLine, const char* SrcFile, const char* format, const char* text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::_sFormat2(int_ SrcLine, const char* SrcFile, const char* format, CMaaString text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::_sFormat2(int_ SrcLine, const char* SrcFile, CMaaString format, const char* text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::_sFormat2(int_ SrcLine, const char* SrcFile, CMaaString format, CMaaString text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, text, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
CMaaString CMaaString::_sFormat2(int_ SrcLine, const char* SrcFile, const char* format, const char8_t* text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, (const char *)text, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
CMaaString CMaaString::_sFormat2(int_ SrcLine, const char* SrcFile, CMaaString format, const char8_t* text, ...) noexcept
{
    CMaaString Ret;
    va_list list;
    va_start(list, text);
    Ret.Sprintf2(format, (const char*)text, list, SrcLine, SrcFile);
    va_end(list);
    return Ret;
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Выкусить пробелы из строки
//---------------------------------------------------------------------------
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
CMaaString & CMaaString::Strip(EStripType Type)
{
    int Start = 0, End = Length() - 1;
    const char * p = *this;
    if  (Type & eBegin)
    {
        while(Start <= End && p[Start] == ' ') Start++;
    }
    if  (Type & eEnd)
    {
        while(End >= Start && p[End] == ' ') End--;
    }
    if  (Type & eCutSlash)
    {
        while(End >= Start && p[End] == '\\') End--;
    }
    if  ((Type & eCutCRLF) == eCutCRLF)
    {
        while(End >= Start && (p[End] == '\r' || p[End] == '\n')) End--;
    }
    else
    {
        if  (Type & eCutCR)
        {
            while(End >= Start && p[End] == '\r') End--;
        }
        if  (Type & eCutLF)
        {
            while(End >= Start && p[End] == '\r') End--;
        }
    }
    return (*this = Mid(Start, End - Start + 1));
}
#endif

CMaaString CMaaString::Left(int nCount) const noexcept(noexcept_new)
{
    const int Len = Length();
    return nCount >= Len ? *this : nCount > 0 ? CMaaString((const char*)*this, nCount, GetUtf1632Flags()) : CMaaStringZ;
}

CMaaString CMaaString::Right(int nCount) const noexcept(noexcept_new)
{
    const int Len = Length();
    return nCount >= Len ? *this : nCount > 0 ? CMaaString(Len - nCount + (const char*)*this, nCount, GetUtf1632Flags()) : CMaaStringZ;
}

#if 0
#ifdef _WIN32
#define llog(fmt,a,b,c) {FILE * f = fopen("C:\\maa\\mid.log", "a+b"); if (f) { fprintf(f, fmt, a, b, c); fclose(f); } }
#else
#define llog(fmt,a,b,c) {FILE * f = fopen("/temp/rw/mid.log", "a+b"); if (f) { fprintf(f, fmt, a, b, c); fclose(f); } }
#endif
#endif

CMaaString CMaaString::Mid(int nFirst, int nCount /*= -1*/) const noexcept(noexcept_new)
{
/*
#if defined(_DEBUG)
     if   (nFirst < 0)
     {
          FILE * f = fopen("c:\\mid.txt", "a+b");
          if   (f)
          {
               fprintf(f, "Called CMaaString::Mid(%d, %d)\r\n", (int_)nFirst, (int_)nCount);
               fclose(f);
          }
     }
#endif
*/
    const int Len = Length ();
    if  (nFirst < 0)
    {
        if  (nCount >= -nFirst)
        {
            //llog("Mid(%d, %d)\n", (int_)nFirst, (int_)nCount, 0);
            //nCount -= nFirst;
            nCount += nFirst; // 17.12.2021
        }
        else
        {
            nCount = 0;
        }
        nFirst = 0;
    }
    if  (nFirst >= Len || !nCount)
    {
        return CMaaStringZ;
    }
    if  (nCount < 0)
    {
        return Right(Len - nFirst);
    }
    if  (nFirst == 0 && nCount == Len)
    {
        return *this;
    }
    if  (nFirst + nCount >= Len)
    {
        return Right(Len - nFirst);
    }
    return CMaaString(nFirst + (const char *)*this, nCount, GetUtf1632Flags());
}

CMaaString CMaaString::RefLeft(int nCount) const noexcept
{
    const int Len = Length();
    return nCount >= Len ? *this : nCount > 0 ? CMaaString(*this, 0, nCount) : CMaaStringZ;
}

CMaaString CMaaString::RefRight(int nCount) const noexcept
{
    const int Len = Length();
    return nCount >= Len ? *this : nCount > 0 ? CMaaString(*this, Len - nCount, nCount) : CMaaStringZ;
}

CMaaString CMaaString::RefMid(int nFirst, int nCount) const noexcept
{
    const int Len = Length ();
    if  (nFirst < 0)
    {
        if  (nCount >= -nFirst)
        {
            //llog("RefMid(%d, %d)\n", (int_)nFirst, (int_)nCount, 0);
            //nCount -= nFirst;
            nCount += nFirst; // 17.12.2021
        }
        else
        {
            nCount = 0;
        }
        nFirst = 0;
    }
    if  (nFirst >= Len || !nCount)
    {
        return CMaaStringZ;
    }
    if  (nCount < 0)
    {
        return nFirst ? CMaaString(*this, nFirst, Len - nFirst) : *this;
    }
    if  (nFirst == 0 && nCount == Len)
    {
        return *this;
    }
    if  (nFirst + nCount >= Len)
    {
        return CMaaString(*this, nFirst, Len - nFirst);
    }
    return CMaaString(*this, nFirst, nCount);
}

CMaaString CMaaString::LeftMid(int nLeft, int nFirst, int nCount) const noexcept(noexcept_new)
{
    return (RefLeft(nLeft) + RefMid(nFirst, nCount)).s().Str0Copy();
}

CMaaString & CMaaString::SetLeftMid(int nLeft, int nFirst, int nCount) noexcept(noexcept_new)
{
    const int Len = Length();
    nLeft = nLeft >= 0 ? nLeft : 0;
    nLeft = nLeft <= Len ? nLeft : Len;
    if (IsRWSingleOwner() && nLeft <= nFirst)
    {
        /*
        if (nFirst < 0)
        {
            if (nCount >= -nFirst)
            {
                nCount += nFirst; // 17.12.2021
            }
            else
            {
                nCount = 0;
            }
            nFirst = 0;
        }
        */
        if (nFirst >= Len || !nCount)
        {
            nFirst = nCount = 0;
        }
        else if (nCount < 0)
        {
            nCount = Len - nFirst;
        }
        else if (nFirst == 0 && nCount == Len)
        {
        }
        else if (nFirst + nCount >= Len)
        {
            nCount = Len - nFirst;
        }
        //else
        //{
            //return CMaaString(*this, nFirst, nCount);
        //}
        if (nLeft != nFirst)
        {
            char* p = GetBuffer();
            memmove(p + nLeft, p + nFirst, nCount);
        }
        TruncateRWS(nLeft + nCount);
    }
    else
    {
        *this = (RefLeft(nLeft) + RefMid(nFirst, nCount)).s().Str0Copy();
    }
    return *this;
}

CMaaString CMaaString::LeftInsMid(int nLeft, const CMaaString& Ins, int nFirst, int nCount) const noexcept(noexcept_new)
{
    const CMaaString strLeft = RefLeft(nLeft);
    const CMaaString strRight = RefMid(nFirst, nCount);
    const int ltL = strLeft.Length();
    const int iL = Ins.Length();
    const int rtL = strRight.Length();
    CMaaString NewString(nullptr, (size_t)(ltL + iL + rtL), (_e1_)(GetUtf1632Flags() | (Ins.GetUtf1632Flags() & eCryptoKey)) /*eNotInitMem*/, 0 /*256*/);
    if (NewString.IsValid())
    {
        char* p = NewString.GetBuffer();
        memcpy(p, strLeft, ltL);
        memcpy(p + ltL, Ins, iL);
        memcpy(p + ltL + iL, strRight, rtL);
    }
    return NewString;
}

CMaaString CMaaString::LeftInsRight(int nLeft, const CMaaString& Ins, int nRight) const noexcept(noexcept_new)
{
    const int L = Length();
    return LeftInsMid(nLeft, Ins, nRight <= L ? L - nRight : 0);
}

CMaaString& CMaaString::SetLeftInsMid(int nLeft, const CMaaString& Ins, int nFirst, int nCount) noexcept(noexcept_new)
{
    const int L = Length();
    const int iL = Ins.Length();
    nLeft = nLeft >= 0 ? nLeft : 0;
    nLeft = nLeft <= L ? nLeft : L;
    if (IsRWSingleOwner() && !StrictEq(Ins) && nLeft <= nFirst)
    {
        /*
        if (nFirst < 0)
        {
            if (nCount >= -nFirst)
            {
                nCount += nFirst; // 17.12.2021
            }
            else
            {
                nCount = 0;
            }
            nFirst = 0;
        }
        */
        if (nFirst >= L || !nCount)
        {
            nFirst = nCount = 0;
        }
        else if (nCount < 0)
        {
            nCount = L - nFirst;
        }
        else if (nFirst == 0 && nCount == L)
        {
        }
        else if (nFirst + nCount >= L)
        {
            nCount = L - nFirst;
        }
        //else
        //{
            //return CMaaString(*this, nFirst, nCount);
        //}
        const int N = nLeft + iL + nCount;
        if (m_pImp != sp_NullImp)
        {
            if ((!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)) && m_pImp->m_iCapacity >= N)
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
                || (m_pImp->m_pszStr == CMAASTRING_UNALLOC_PTR_(m_pImp) && N <= CMaaStringImp::GetMaxUnAllocLength())
#endif
                )
            {
                if (iL || (nFirst != nLeft && nCount))
                {
                    char* p = GetBuffer();
                    memmove(p + nLeft + iL, p + nFirst, nCount);
                    memcpy(p + nLeft, Ins, iL);
                    TOOLSLIB_ATOMIC_OP_EQ(m_pImp->f2.a12.m_Flags1, fetch_or, |=, ((Ins.GetUtf1632Flags() & eCryptoKey) >> 8));
                }
                if (N <= m_pImp->m_iLength)
                {
                    TruncateRWS(N);
                }
                else
                {
                    m_pImp->m_iLength = N;
                    if (!(TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & (CMaaStringImp::eDontDelete)))
                    {
                        *(_qword*)(m_pImp->m_pszStr + m_pImp->m_iLength) = 0;
                    }
                }
                return *this;
            }
        }
        CMaaString NewString(nullptr, (size_t)N, (_e1_)(GetUtf1632Flags() | (Ins.GetUtf1632Flags() & eCryptoKey)) /*eNotInitMem*/, 256);
        if (NewString.IsValid())
        {
            char* p = NewString.GetBuffer();
            const char* c = *this;
            memcpy(p, c, nLeft);
            memcpy(p + nLeft, Ins, iL);
            memcpy(p + nLeft + iL, c + nFirst, nCount);
        }
        return (*this = NewString);
    }
    const CMaaString strLeft = RefLeft(nLeft);
    const CMaaString strRight = RefMid(nFirst, nCount);
    const int ltL = strLeft.Length();
    const int rtL = strRight.Length();
    CMaaString NewString(nullptr, (size_t)(ltL + iL + rtL), (_e1_)(GetUtf1632Flags() | (Ins.GetUtf1632Flags() & eCryptoKey)) /*eNotInitMem*/, 256);
    if (NewString.IsValid())
    {
        char* p = NewString.GetBuffer();
        memcpy(p, strLeft, ltL);
        memcpy(p + ltL, Ins, iL);
        memcpy(p + ltL + iL, strRight, rtL);
    }
    return (*this = NewString);
}

CMaaString & CMaaString::SetLeftInsRight(int nLeft, const CMaaString& Ins, int nRight) noexcept(noexcept_new)
{
    const int L = Length();
    return SetLeftInsMid(nLeft, Ins, nRight <= L ? L - nRight : 0);
}

CMaaString & CMaaString::SetLeft(int nCount) noexcept(noexcept_new)
{
    Truncate(nCount);
    return *this;
}

CMaaString & CMaaString::SetRight(int nCount) noexcept(noexcept_new)
{
    if (IsRWSingleOwner())
    {
        if (nCount <= 0)
        {
            Empty();
        }
        else
        {
            const int L = Length();
            if (nCount < L)
            {
                char* p = GetBuffer();
                memmove(p, p + L - nCount, nCount);
                TruncateRWS(nCount);
            }
        }
    }
    else
    {
        *this = RefRight(nCount).Str0Copy();
    }
    return *this;
}

CMaaString & CMaaString::SetMid(int nFirst, int nCount /*= -1*/) noexcept(noexcept_new)
{
    const int Len = Length();
    if (nFirst < 0)
    {
        if (nCount >= -nFirst)
        {
            nCount += nFirst; // 17.12.2021
        }
        else
        {
            nCount = 0;
        }
        nFirst = 0;
    }
    if (nFirst >= Len || nCount == 0)
    {
        Empty();
        return *this;
    }
    if (nCount < 0)
    {
        return SetRight(Len - nFirst);
    }
    if (nFirst == 0 && nCount == Len)
    {
        return *this;
    }
    if (nFirst + nCount >= Len)
    {
        return SetRight(Len - nFirst);
    }
    if (IsRWSingleOwner())
    {
        if (nFirst)
        {
            char* p = GetBuffer();
            memmove(p, p + nFirst, nCount);
        }
        TruncateRWS(nCount);
    }
    else
    {
        *this = CMaaString(nFirst + (const char*)*this, nCount, GetUtf1632Flags());
    }
    return *this;
}

/*
bool CMaaString::IsLeft(const char * ptr, int nCount) const noexcept
{
    return nCount >= 0 && Length() >= nCount && CMaaIsMemEq((const char *)*this, ptr, nCount);
}
bool CMaaString::IsRight(const char * ptr, int nCount) const noexcept
{
    const int Len = Length();
    return Len >= nCount && CMaaIsMemEq(Len - nCount + (const char *)*this, ptr, nCount);
}
bool CMaaString::IsMid(const char * ptr, int nFirst, int nCount) const noexcept
{
    const int Len = Length();
    return nCount >= 0 && nFirst >= 0 && Len >= nFirst + nCount && CMaaIsMemEq(nFirst + (const char *)*this, ptr, nCount);
}
*/
bool CMaaString::IsLeft(const CMaaString &That) const noexcept
{
    const int nCount = That.Length();
    return Length() >= nCount && CMaaIsMemEq((const char *)*this, (const char *)That, nCount);
}
bool CMaaString::IsRight(const CMaaString &That) const noexcept
{
    const int nCount = That.Length();
    const int Len = Length();
    return Len >= nCount && CMaaIsMemEq(Len - nCount + (const char *)*this, (const char *)That, nCount);
}
bool CMaaString::IsMid(const CMaaString &That, int nFirst) const noexcept
{
    const int nCount = That.Length();
    const int Len = Length();
    return nFirst >= 0 && Len >= nFirst + nCount && CMaaIsMemEq(nFirst + (const char *)*this, (const char *)That, nCount);
}
bool CMaaString::IsCi(const CMaaString& That, int_ cp) const noexcept
{
    const int nCount = That.Length();
    return Length() == nCount && ::IsCi((const char*)*this, (const char*)That, nCount, cp);
}
bool CMaaString::IsLeftCi(const CMaaString& That, int_ cp) const noexcept
{
    const int nCount = That.Length();
    return Length() >= nCount && ::IsCi((const char*)*this, (const char*)That, nCount, cp);
}
bool CMaaString::IsRightCi(const CMaaString& That, int_ cp) const noexcept
{
    const int nCount = That.Length();
    const int Len = Length();
    return Len >= nCount && ::IsCi(Len - nCount + (const char*)*this, (const char*)That, nCount, cp);
}
bool CMaaString::IsMidCi(const CMaaString& That, int nFirst, int_ cp) const noexcept
{
    const int nCount = That.Length();
    const int Len = Length();
    return nFirst >= 0 && Len >= nFirst + nCount && ::IsCi(nFirst + (const char*)*this, (const char*)That, nCount, cp);
}

CMaaString CMaaString::Rewind() const noexcept(noexcept_new)
{
    const int len = Length();
    if  (len > 1)
    {
        CMaaString r(nullptr, len, (_e1_)GetUtf1632Flags());
        if  (r.Length() == len)
        {
            const char * p0 = *this;
            char * p = r.GetBuffer();
            for (int i = 0; i < len; i++)
            {
                p[i] = p0[len - 1 - i];
            }
        }
        return r;
    }
    return *this;
}
CMaaString CMaaString::RewindAdd(const CMaaString &AtBegin, const CMaaString &AtEnd) noexcept(noexcept_new)
{
    int pos0 = AtBegin.Length();
    int len = Length() + pos0 + AtEnd.Length();
    CMaaString r(nullptr, len, (_e1_)GetUtf1632Flags());
    if  (r.Length() == len)
    {
        len = Length();
        const char * p0 = *this;
        char * p = r.GetBuffer();
        memcpy(p, AtBegin, pos0);
        for (int i = 0; i < len; i++)
        {
            p[pos0 + i] = p0[len - 1 - i];
        }
        memcpy(p + pos0 + len, AtEnd, AtEnd.Length());
    }
    return r;
}
//---------------------------------------------------------------------------
static _dword CMaaStaticStandardHashFunction(const unsigned char* p, int len) noexcept
{
    if ((_dword)len <= (_dword)sizeof(_dword))
    {
        _dword x = 0;
        memcpy(&x, p, len);
#ifdef TOOLSLIB_HASH_USE_LEN
        return x ^ (_dword)len;
#else
        return x;
#endif
    }
    _dword x;
#ifdef TRY_HF_OPT
#if defined(_WIN64) || defined(__linux__)
    if (len >= 4 * (int)sizeof(_dword))
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        _uqword q = len;
#else
        _uqword q = 0;
#endif
        for (int i = len >> _qword_shift; i--; )
        {
            q = CMaaRol7qwMacro(q) ^ *(_uqword*)p;
            p += sizeof(_uqword);
        }
        x = (_dword)(q >> 32);
        x = CMaaRol7dwMacro(x) ^ (_dword)q;
        if ((len & (1 << _dword_shift)))
        {
            x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
            p += sizeof(_dword);
        }
        if ((len & _dword_mask))
        {
            _dword y = 0;
            memcpy(&y, p, (len & _dword_mask));
            x = CMaaRol7dwMacro(x) ^ y;
        }
    }
    else
#endif
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        x = (_dword)len;
#else
        x = 0;
#endif
        for (int i = len >> _dword_shift; i--; )
        {
            x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
            p += sizeof(_dword);
        }
        if ((len & _dword_mask))
        {
            _dword y = 0;
            memcpy(&y, p, (len & _dword_mask));
            x = CMaaRol7dwMacro(x) ^ y;
        }
    }
#else
#ifdef TOOLSLIB_HASH_USE_LEN
    x = len;
#else
    x = 0;
#endif
    for (int i = len; i--; )
    {
        x = CMaaRol7dwMacro(x) ^ *p++;
    }
#endif
    return x;
}
//---------------------------------------------------------------------------
static _uqword CMaaStaticStandardHashFunction64(const unsigned char* p, int len) noexcept
{
    if ((_dword)len <= (_dword)sizeof(_uqword))
    {
        _uqword x = 0;
        memcpy(&x, p, len);
#ifdef TOOLSLIB_HASH_USE_LEN
        return x ^ (_dword)len;
#else
        return x;
#endif
    }
#ifdef TOOLSLIB_HASH_USE_LEN
    _uqword x = len;
#else
    _uqword x = 0;
#endif
#ifdef TRY_HF_OPT
    {
        for (int i = len >> _qword_shift; i--; )
        {
            x = CMaaRol7qwMacro(x) ^ *(_uqword*)p;
            p += sizeof(_uqword);
        }
        if ((len & _qword_mask))
        {
            _uqword y = 0;
            memcpy(&y, p, (len & _qword_mask));
            x = CMaaRol7qwMacro(x) ^ y;
        }
    }
#else
    for (int i = len; i--; )
    {
        x = CMaaRol7qwMacro(x) ^ *p++;
    }
#endif
    return x;
}
//---------------------------------------------------------------------------
_dword CMaaString::Hash() const noexcept
{
    if  (PIMP(m_pImp))
    {
#ifdef CMAASTRING_CACHE_HASH
        if  ((TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & CMaaStringImp::eHashExists))
        {
            return m_pImp->m_Hash;
        }
        const _dword h = m_pImp->m_Hash = CMaaStaticStandardHashFunction((const unsigned char *)m_pImp->m_pszStr, m_pImp->m_iLength);
        TOOLSLIB_ATOMIC_OP_EQ(m_pImp->f2.a12.m_Flags0, fetch_or, |=, CMaaStringImp::eHashExists);
        return h;
#else
        return CMaaStaticStandardHashFunction((const unsigned char *)m_pImp->m_pszStr, m_pImp->m_iLength);
#endif
    }
    return 0;
}
_uqword CMaaString::Hash64() const noexcept
{
    if (PIMP(m_pImp))
    {
#ifdef CMAASTRING_CACHE_HASH64
        if ((TOOLSLIB_LOAD_ATOMIC(m_pImp->f2.a12.m_Flags0) & CMaaStringImp::eHash64Exists))
        {
            return m_pImp->m_Hash64;
        }
        const _uqword h = CMaaStaticStandardHashFunction64((const unsigned char *)m_pImp->m_pszStr, m_pImp->m_iLength);
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
        if (m_pImp->m_pszStr != CMAASTRING_UNALLOC_PTR_(m_pImp))
#endif
        {
            m_pImp->m_Hash64 = h;
            TOOLSLIB_ATOMIC_OP_EQ(m_pImp->f2.a12.m_Flags0, fetch_or, |=, CMaaStringImp::eHash64Exists);
        }
        return h;
#else
        return CMaaStaticStandardHashFunction64((const unsigned char*)m_pImp->m_pszStr, m_pImp->m_iLength);
#endif
    }
    return 0;
}
//---------------------------------------------------------------------------


//--------------------------------------------------------------------------
// 11.03.2003 -- 13.01.2008
//--------------------------------------------------------------------------
CMaaString CMaaString::DataToString(const void * ptr, int len)
{
    unsigned char * p = (unsigned char *)ptr;
    CMaaConcatString s(len * 3 + 1);
    for (; len > 0; len--)
    {
        char txt[20];
        //sprintf(txt, len > 1 ? "%02X " : "%02X", *p++);
        //s += txt;
        Byte2Hex_nzt_H(txt, *p++);
        if (len > 1)
        {
            txt[2] = ' ';
            s.Add(txt, 3);
        }
        else
        {
            s.Add(txt, 2);
        }
    }
    return s;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// 28.09.2005 - for Forum project  --- convert 2008
//--------------------------------------------------------------------------
bool CMaaString::Import(const void * ptr, int maxlen) noexcept(noexcept_new)
{
    const char * p = (const char *)ptr;
    int len;
    for (len = 0; len < maxlen && p[len]; len++) ;

    CMaaString NewString(p, len);
    if  (NewString.IsValid())
    {
        *this = std::move(NewString);
        return true;
    }
    // alloc error
    Empty(); // *this = ""; // 2008
    return false;
}
//--------------------------------------------------------------------------
// 28.09.2005
//--------------------------------------------------------------------------
bool CMaaString::Export(void * ptr, int maxlen) const noexcept
{
    char * p = (char *)ptr;
    int len = GetLength();
    bool Ret;
    if  (len > maxlen)
    {
        len = maxlen;
        Ret = false;
    }
    else
    {
        Ret = true;
    }
    memcpy(p, (const char *)*this, len);
    memset(p + len, 0, maxlen - len);
    return Ret;
}
//--------------------------------------------------------------------------
int CMaaString::ReplaceNN(char What, char By, int StartPos, int EndPos) noexcept(noexcept_new)
{
    int pos = Find(StartPos, What, EndPos);
    if  (pos < 0)
    {
        return 0;
    }
    if  (!m_pImp->IsRWSingleOwner())
    {
        *this = std::move(m_pImp->NewCopy());
    }
    char * p = GetBuffer();
    char * e = p + Length();
    e = EndPos < 0 ? e : p + EndPos < e ? p + EndPos : e;
    if (e == p)
    {
        // TL_NEW[] error
        return -1;
    }
    int n = 1;
    p += pos;
    *p++ = By;
    for (; p < e; p++)
    {
        if  (*p == What)
        {
            *p = By;
            n++;
        }
    }
    return n;
}
//--------------------------------------------------------------------------
int CMaaString::ReplaceNN(char What, const CMaaString &By, int StartPos, int EndPos) noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_64K];

    int p = Find(StartPos, What, EndPos);
    if (p < 0)
    {
        return 0;
    }
    const int SrcLen = Length();
    const int Len = EndPos < 0 || EndPos > SrcLen ? SrcLen : EndPos;
    CMaaConcatString r(Buffer, sizeof(Buffer), SrcLen + 1024/*, 0, false /*bCountOnly*/);
    int n = 1;
    const char* ptr = *this;
    r.Add(ptr, p);
    r += By;
    int pos = ++p;
    while (pos < Len)
    {
        p = Find(pos, What, Len);
        if (p < 0)
        {
            break;
        }
        n++;
        r.Add(pos + ptr, p - pos);
        r += By;
        pos = ++p;
    }
    r.Add(pos + ptr, SrcLen - pos);
    //if (!bCountOnly)
    {
        *this = (CMaaString)r;
        if (!IsValid() && r.GetLength())
        {
            // TL_NEW[] error
            return -1;
        }
    }
    return n;
}
//--------------------------------------------------------------------------
int CMaaString::ReplaceNN(int_ N, const char* pWhat, const CMaaString * const pBy, int StartPos, int EndPos) noexcept(noexcept_new)
{
    if (N <= 0 || StartPos < 0)
    {
        return 0;
    }
    const int SrcLen = Length();
    const int Len = EndPos < 0 || EndPos > SrcLen ? SrcLen : EndPos;
    if (StartPos >= Len)
    {
        return 0;
    }

    int_ idx[256];
    memset(idx, -1, sizeof(idx));
    int_ i, x;
    for (i = 0; i < N; i++)
    {
        x = *(unsigned char*)pWhat++;
        if (idx[x] < 0)
        {
            idx[x] = i;
        }
    }
    char Buffer[TOOLSLIB_CS_64K];
    const char* ptr = *this;

    int p = -1;
    for (int ii = StartPos; ii < Len; ii++)
    {
        x = (unsigned char)ptr[ii];
        if (idx[x] >= 0)
        {
            p = ii;
            break;
        }
    }
    if (p < 0)
    {
        return 0;
    }
    CMaaConcatString r(Buffer, sizeof(Buffer), SrcLen + 1024/*, 0, false /*bCountOnly*/);
    int n = 1;
    r.Add(ptr, p);
    r += pBy[idx[x]];
    int pos = ++p;
    while (p < Len)
    {
        x = (unsigned char)ptr[p];
        if (idx[x] >= 0)
        {
            n++;
            r.Add(pos + ptr, p - pos);
            r += pBy[idx[x]];
            pos = ++p;
        }
        else
        {
            p++;
        }
    }
    r.Add(pos + ptr, SrcLen - pos);
    //if (!bCountOnly)
    {
        *this = (CMaaString)r;
        if (!IsValid() && r.GetLength())
        {
            // TL_NEW[] error
            return -1;
        }
    }
    return n;
}
//--------------------------------------------------------------------------
int_ CMaaString::TruncateRWS(int Len) noexcept
{
    const int OldLen = Length();
    if (Len >= OldLen)
    {
        return 0;
    }
    else if (Len <= 0)
    {
        Empty();
    }
    else //if (Len < OldLen)
    {
        if (IsRWSingleOwner())
        {
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
            if (m_pImp->m_pszStr == CMAASTRING_UNALLOC_PTR_(m_pImp))
            {
                memset(m_pImp->m_pszStr + Len, 0, OldLen - Len);
            }
            else
#endif
            {
                if (OldLen - Len < 8)
                {
                    memset(m_pImp->m_pszStr + Len, 0, OldLen - Len);
                    TOOLSLIB_ATOMIC_OP_EQ(m_pImp->f2.a12.m_Flags1, fetch_and, &=, ~(CMaaStringImp::eLengthLimit1 >> 8));
                }
                else
                {
                    *(_qword*)(m_pImp->m_pszStr + Len) = 0;
                    TOOLSLIB_ATOMIC_OP_EQ(m_pImp->f2.a12.m_Flags1, fetch_and, &=, ~(CMaaStringImp::eLengthLimit >> 8));
                }
            }
            m_pImp->m_iLength = Len;
#ifndef CMaaStringImp_NOT_NULL_MODE
            m_pImp->ClearHash();
#else
            ClearHash();
#endif
        }
        else
        {
            //*this = CMaaString((const char*)*this, Len, GetUtf1632Flags()); // i.e. *this = Left(Len);
            //*this = CMaaString(m_pImp->m_pszStr, Len, m_pImp->GetUtf1632Flags());
            return -1;
        }
    }
    return 1;
}
//--------------------------------------------------------------------------
bool CMaaString::Truncate(int Len) noexcept(noexcept_new)
{
    const int_ x = TruncateRWS(Len);
    if (x < 0)
    {
        //*this = CMaaString((const char*)*this, Len, GetUtf1632Flags()); //Left(Len);
        *this = CMaaString(m_pImp->m_pszStr, Len, m_pImp->GetUtf1632Flags());
    }
    return x;
}
//--------------------------------------------------------------------------
#ifndef USE_FIND_KMP
#if 1
int CMaaString::ReplaceNN(const CMaaString &What, const CMaaString &By, bool bCountOnly, int StartPos, int EndPos) noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_64K];

    const int WLen = What.Length();
    int p = Find(StartPos, What, WLen, EndPos);
    if (p < 0)
    {
        return 0;
    }
    const int SrcLen = m_pImp->Length();
    const int Len = EndPos < 0 || EndPos > SrcLen ? SrcLen : EndPos;
    int n = 1;
    if (bCountOnly)
    {
        //const bool bOverride = m_pImp->IsRWSingleOwner() && WLen >= By.Length();
        //CMaaConcatString_<0, true> r;
        const char* ptr = *this;
        //r.Add(ptr, p);
        //r += By;
        int pos = p + WLen;
        while (pos < Len)
        {
            p = Find(pos, What, WLen, Len);
            if (p < 0)
            {
                break;
            }
            n++;
            //r.Add(pos + ptr, p - pos);
            //r += By;
            pos = p + WLen;
        }
        //r.Add(pos + ptr, SrcLen - pos);
    }
    else
    {
        const bool bOverride = m_pImp->IsRWSingleOwner() && WLen >= By.Length();
        CMaaConcatString r(bOverride ? (char*)*m_pImp : Buffer, bOverride ? SrcLen : sizeof(Buffer), bOverride ? SrcLen : SrcLen + 1024);
        const char* ptr = *this;
        r.Add(ptr, p);
        r += By;
        int pos = p + WLen;
        while (pos < Len)
        {
            p = Find(pos, What, WLen, Len);
            if (p < 0)
            {
                break;
            }
            n++;
            r.Add(pos + ptr, p - pos);
            r += By;
            pos = p + WLen;
        }
        r.Add(pos + ptr, SrcLen - pos);
        //if (!bCountOnly)
        {
            if (bOverride)
            {
                TruncateRWS((int)r.Length());
            }
            else
            {
                *this = (CMaaString)r;
                if (!IsValid() && r.GetLength())
                {
                    // TL_NEW[] error
                    return -1;
                }
            }
        }
    }
    return n;
}
#else
int CMaaString::ReplaceNN(const CMaaString &What, const CMaaString &By, bool bCountOnly)
{
    char Buffer[TOOLSLIB_CS_64K];
    const int Len = Length();
    const int WLen = What.Length();
    CMaaConcatString r(Buffer, sizeof(Buffer), Length() + 1024, 0, bCountOnly);
    int pos = 0;
    const char* ptr = *this;
    const int n = FindAll(0, What, What.Length(), -1, [&r, &pos, &ptr, &WLen, &By](int x)
    //const int n = FindAll(0, What, What.Length(), -1, [&](int x)
        {
            if (x >= pos)
            {
                r.Add(pos + ptr, x - pos);
                r += By;
                pos = x + WLen;
                return true;
            }
            return false;
        });
    if (n > 0)
    {
        r.Add(pos + ptr, Len - pos);
        if (!bCountOnly)
        {
            *this = r;
            if (!IsValid() && r.GetLength())
            {
                // TL_NEW[] error
                return -1;
            }
        }
        return n;
    }
    return 0;
}
#endif
#endif
//--------------------------------------------------------------------------
int CMaaString::KMPReplaceNN(const CMaaString &What, const CMaaString &By, bool bCountOnly)
{
    char Buffer[TOOLSLIB_CS_64K];
    const int Len = Length();
    const int WLen = What.Length();
    if (bCountOnly)
    {
        //CMaaConcatString_<0, true> r;
        int pos = 0;
        const char* ptr = *this;
        const int n = KMPFindAll(0, What, What.Length(), -1, [/*&r,*/ &pos, &ptr, &WLen, &By](int x)
            //const int n = KMPFindAll(0, What, What.Length(), -1, [&](int x)
            {
                if (x >= pos)
                {
                    //r.Add(pos + ptr, x - pos);
                    //r += By;
                    pos = x + WLen;
                    return true;
                }
                return false;
            });
        if (n > 0)
        {
            /*
            r.Add(pos + ptr, Len - pos);
            if (!bCountOnly)
            {
                *this = r;
                if (!IsValid() && r.GetLength())
                {
                    // TL_NEW[] error
                    return -1;
                }
            }
            */
            return n;
        }
    }
    else
    {
        CMaaConcatString r(Buffer, sizeof(Buffer), Length() + 1024);
        int pos = 0;
        const char* ptr = *this;
        const int n = KMPFindAll(0, What, What.Length(), -1, [&r, &pos, &ptr, &WLen, &By](int x)
            //const int n = KMPFindAll(0, What, What.Length(), -1, [&](int x)
            {
                if (x >= pos)
                {
                    r.Add(pos + ptr, x - pos);
                    r += By;
                    pos = x + WLen;
                    return true;
                }
                return false;
            });
        if (n > 0)
        {
            r.Add(pos + ptr, Len - pos);
            if (!bCountOnly)
            {
                *this = (CMaaString)r;
                if (!IsValid() && r.GetLength())
                {
                    // TL_NEW[] error
                    return -1;
                }
            }
            return n;
        }
    }
    return 0;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::Replace(char What, char By, bool bStoreResult, int* pNN) noexcept(noexcept_new)
{
    int i_tmp;
    pNN = pNN ? pNN : &i_tmp;
    if (!bStoreResult)
    {
        CMaaString tmp(*this);
        *pNN = tmp.ReplaceNN(What, By);
        return tmp;
    }
    *pNN = ReplaceNN(What, By);
    return *this;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::Replace(const CMaaString &What, const CMaaString &By, bool bStoreResult, int* pNN) noexcept(noexcept_new)
{
    int i_tmp;
    pNN = pNN ? pNN : &i_tmp;
    if (!bStoreResult)
    {
        CMaaString tmp(*this);
        *pNN = tmp.ReplaceNN(What, By);
        return tmp;
    }
    *pNN = ReplaceNN(What, By);
    return *this;
}
//--------------------------------------------------------------------------
#if 0
int TestRepace()
{
    int x;
    CMaaString tmp = CMaaString::sFormat("%.3lf", 1.234).Replace('.', ',', true, &x), tmp2;
    __utf8_printf("%d %d %S\n", tmp == "1,234" && x == 1, (int_)x, &tmp);
    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace(".", ",", true, &x);
    __utf8_printf("%d %d %S\n", tmp == "1,234" && x == 1, (int_)x, &tmp);

    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace('.', ',', false, &x);
    __utf8_printf("%d %d %S\n", tmp == "1,234" && x == 1, (int_)x, &tmp);

    tmp = CMaaString::sFormat("%.3lf", 1.234);
    tmp2 = tmp;
    tmp2 = tmp2.Replace('.', ',', false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234" && tmp2 == "1,234" && x == 1, (int_)x, &tmp, &tmp2);

    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace(".", ",", false, &x);
    __utf8_printf("%d %d %S\n", tmp == "1,234" && x == 1, (int_)x, &tmp);

    tmp = CMaaString::sFormat("%.3lf", 1.234);
    tmp2 = tmp;
    tmp2 = tmp2.Replace(".", ",", false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234" && tmp2 == "1,234" && x == 1, (int_)x, &tmp, &tmp2);

    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace('A', ',', true, &x);
    __utf8_printf("%d %d %S\n", tmp == "1.234" && x == 0, (int_)x, &tmp);
    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace("A", ",", true, &x);
    __utf8_printf("%d %d %S\n", tmp == "1.234" && x == 0, (int_)x, &tmp);
    tmp = CMaaString::sFormat("%.3lf.", 1.234).Replace('.', ',', true, &x);
    __utf8_printf("%d %d %S\n", tmp == "1,234," && x == 2, (int_)x, &tmp);
    tmp = CMaaString::sFormat("%.3lf.", 1.234).Replace(".", ",", true, &x);
    __utf8_printf("%d %d %S\n", tmp == "1,234," && x == 2, (int_)x, &tmp);
    tmp = CMaaString::sFormat("%.3lf.", 1.234).Replace("A", ",", true, &x);
    __utf8_printf("%d %d %S\n", tmp == "1.234.", (int_)x, &tmp);

    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace('A', ',', false, &x);
    __utf8_printf("%d %d %S\n", tmp == "1.234" && x == 0, (int_)x, &tmp);

    tmp = CMaaString::sFormat("%.3lf", 1.234);
    tmp2 = tmp;
    tmp2 = tmp2.Replace('A', ',', false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234" && tmp2 == "1.234" && x == 0, (int_)x, &tmp, &tmp2);

    tmp = CMaaString::sFormat("%.3lf", 1.234).Replace("A", ",", false, &x);
    __utf8_printf("%d %d %S\n", tmp == "1.234" && x == 0, (int_)x, &tmp);

    tmp = CMaaString::sFormat("%.3lf", 1.234);
    tmp2 = tmp;
    tmp2 = tmp2.Replace("A", ",", false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234" && tmp2 == "1.234" && x == 0, (int_)x, &tmp, &tmp2);

    tmp = CMaaString::sFormat("%.3lf.", 1.234);
    tmp2 = tmp;
    tmp2 = tmp.Replace('.', ',', false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234." && tmp2 == "1,234," && x == 2, (int_)x, &tmp, &tmp2);
    tmp = CMaaString::sFormat("%.3lf.", 1.234);
    tmp2 = tmp;
    tmp2 = tmp2.Replace(".", ",", false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234." && tmp2 == "1,234," && x == 2, (int_)x, &tmp, &tmp2);
    tmp = CMaaString::sFormat("%.3lf.", 1.234);
    tmp2 = tmp;
    tmp2 = tmp2.Replace("A", ",", false, &x);
    __utf8_printf("%d %d %S %S\n", tmp == "1.234." && tmp2 == "1.234." && x == 0, (int_)x, &tmp, &tmp2);

    return 0;
}
#endif
//--------------------------------------------------------------------------
typedef char QChar;
typedef CMaaString QString;
static bool CmpQStrSimpleRegExp(QString txt, QString reg, CMaaPtr_<unsigned char, 1>* pm = nullptr);
static bool CmpQStrSimpleFileExp(QString txt, QString reg, bool IsCaseSensitive = true, CMaaPtr_<unsigned char, 1>* pm = nullptr); // IsCaseSensitive is not working now (always == true)
//--------------------------------------------------------------------------
class Bit2_2
{
    unsigned char * m;
    int m_x, m_y;
    CMaaPtr_<unsigned char, 1>* pm;
public:
    Bit2_2(int x, int y)
    {
        pm = nullptr;
        m_x = x;
        m_y = y;
        CMaaPtr_<unsigned char, 1> tmp((x * y + 3) / 4); //  except
        m = tmp.Extract();
        memset(m, 0, (x * y + 3) / 4);
    }
    Bit2_2(int x, int y, CMaaPtr_<unsigned char, 1> *pmm)
    {
        m_x = x;
        m_y = y;
        if ((pm = pmm))
        {
            pmm->CheckResize((x * y + 3) / 4); //  except
            m = *pmm;
        }
        else
        {
            CMaaPtr_<unsigned char, 1> tmp((x * y + 3) / 4); //  except
            m = tmp.Extract();
        }
        memset(m, 0, (x * y + 3) / 4);
    }
    ~Bit2_2() noexcept
    {
        if (!pm)
        {
            delete[] m;
        }
    }
    int_ Get(int x, int y) noexcept
    {
        if  (x < 0 || x >= m_x)
        {
            return -1;
        }
        if  (y < 0 || y >= m_y)
        {
            return -1;
        }
        const int p = m_x * y + x;
        int_ a = m [p / 4];
        a >>= ((int_)(p & 3)) << 1;
        return (a & 3);
    }
    void Set(int x, int y, int_ val) noexcept
    {
        if  (x < 0 || x >= m_x)
        {
            return;
        }
        if  (y < 0 || y >= m_y)
        {
            return;
        }
        if  (val < 0 || val >= 4)
        {
            return;
        }
        const int p = m_x * y + x;
        const int_ mask = 3 << (((int_)(p & 3)) << 1);
        val <<= ((int_)(p & 3)) << 1;
        m [p / 4] = (int_)(m [p / 4] & ~mask) | val;
    }
};
//--------------------------------------------------------------------------
template<typename _QChar> int_ Comb(const _QChar * a, int lena, const _QChar * b, int lenb, Bit2_2 &dt)
{
    if  (lena == 0 || lenb == 0)
    {
        if  (lena == 0 && lenb == 0)
        {
            return 1;
        }
        if  (lena)
        {
            return 3;
        }
        if  (lenb == 1 && *b == _QChar('*'))
        {
            return 1;
        }
        return 3;
    }
    const int la = lena, lb = lenb;
    int_ c = dt.Get(la, lb);
    if  ( c != 0 )
    {
        return c;
    }
    _QChar qq('?');
    _QChar aa('*');
    int i, j;
    c = 3;
    while( 1 )
    {
        if  (*b == qq)
        {
            c = Comb(a+1, lena-1, b+1, lenb-1, dt);
            break;
        }
        if  ( *b != aa )
        {
            for (i = 1; i < lenb && b[i] != aa; i++) ; // const char * s = strchr ( b + 1, '*' );
            if  (i == lenb)
            {
                if  (lena == lenb)
                {
                    for (i =0; i < lena && (a[i] == b[i] || b[i] == qq); i++) ;
                    if  (i == lena)
                    {
                        c = 1;
                    }
                }
                break;
            }
            if  (lena < i)
            {
                break;
            }
            for (j = 0; j < i; j++)
            {
                if  (a[j]!=b[j] && b[j]!=qq)
                {
                    break;
                }
            }
            if  (j <i)
            {
                break;
            }
            lena -= i;
            lenb -= i;
            a += i;
            b += i;
        }
        // *b == '*'
        b++; lenb--;
        if  (lenb == 0)
        {
            c = 1;
            break;
        }
        for (i = 0; i < lenb && b[i] != aa; i++) ; // const char * s = strchr ( b, '*' );
        if  (i == lenb)
        {
            if  (lena >= lenb)
            {
                for (i =0; i < lenb && (a[i+lena-lenb] == b[i] || b[i] == qq); i++) ;
                if  (i == lenb)
                {
                    c = 1;
                }
            }
            break;
        }
        int len = i;
        const _QChar * ea = a + ( lena -= len );
        lenb -= len;
        for ( const _QChar * aa = a; aa <= ea; aa++ )
        {
            for (i = 0; i < len && (aa[i]==b[i] || b[i]==qq); i++) ;
            if  (i == len) // if ( !memcmp ( aa, b, len ) )
            {
                int_ cc = Comb ( aa + len, lena - (int)( aa - a ), b + len, lenb, dt );
                if  (cc != 3)
                {
                    c = cc;
                    break;
                }
            }
        }
        break;
    }
    dt.Set(la, lb, c);
    return c;
}
//--------------------------------------------------------------------------
static bool CmpQStrSimpleRegExp(QString txt, QString reg, CMaaPtr_<unsigned char, 1>* pm)
{
    int RegLen;
    do
    {
        //RegLen = reg.length();
        //reg.replace("**", "*");
        RegLen = reg.Length();
        reg.ReplaceNN("**", "*");

    }   while(reg.Length() != RegLen);

    const int TxtLen = txt.Length();
    Bit2_2 dt(TxtLen + 1, RegLen + 1, pm);
    //int_ c = Comb(txt.unicode(), TxtLen, reg.unicode(), RegLen, dt);
    const int_ c = Comb<char>(txt, TxtLen, reg, RegLen, dt);
    return (c == 1 || c == 2); // enough: return c == 1;
}
static bool CmpWCStrSimpleRegExp(CMaaString txt, CMaaString reg, CMaaPtr_<unsigned char, 1>* pm = nullptr)
{
    {
        reg = reg.NewCopy();
        _WC_ * p0 = (_WC_ *)(const char *)reg;
        int i = 0;
        int j = 0;
        for (; i < reg.Length() / (int_)sizeof(_WC_); i++)
        {
            if  (p0[i] != (_WC_)'*')
            {
                p0[j++] = p0[i];
            }
            else if (j == 0 || p0[j - 1] != (_WC_)'*')
            {
                p0[j++] = p0[i];
            }
        }
        reg = reg.Left(j * (int_)sizeof(_WC_));
        //CMaaFile f("/var/ram/maa/reg.txt", CMaaFile::eACD_SrSw, eNoExcept);
        //f.fprintf("\n\n%S\n\n", &reg);
    }

    /*
    int RegLen = reg.Length() / (int_)(sizeof(_WC_));
    CMaaConcatString reg2;
    _WC_ * p0 = (_WC_ *)(const char *)reg;
    _WC_ * p00 = p0;
    _WC_ * e = p0 + RegLen;
    _WC_ * p = p0;
    while(p < e)
    {
        if   (*p == (_WC_)'*' && p[1] == p[0])
        {
            reg2.Add((const char *)p0, (warning_int)((p + 1 - p0) * sizeof(_WC_)));
            while(*p == (_WC_)'*')
            {
                p++;
            }
            p0 = p;
        }
        else
        {
            p++;
        }
    }
    if   (p0 != p00)
    {
        reg2.Add((const char *)p0, (warning_int)((e - p0) * sizeof(_WC_)));
        reg = reg2;
    }
    */

    const int RegLen = reg.Length() / (int_)(sizeof(_WC_));

    /*
        CMaaFile f("/var/ram/maa/big.txt", CMaaFile::eACD_SrSw, eNoExcept);
    {
        CMaaString r = UnicodeToUtf8(reg);
        CMaaString t = UnicodeToUtf8(txt).Left(100);
        f.fprintf("\n%S\n%S\n-", &r, &t);
    }
    */
    const int TxtLen = txt.Length() / (int_)(sizeof(_WC_));
    Bit2_2 dt(TxtLen + 1, RegLen + 1, pm);
    //int_ c = Comb(txt.unicode(), TxtLen, reg.unicode(), RegLen, dt);
    const int_ c = Comb<_WC_>((_WC_ *)(const char *)txt, TxtLen, (_WC_ *)(const char *)reg, RegLen, dt);
    //    f.fprintf("%d (%s)\n-", c, c == 1 || c == 2 ? "+" : "-");
    return (c == 1 || c == 2); // enough: return c == 1;
}
//--------------------------------------------------------------------------
static bool CmpQStrSimpleFileExp(QString txt, QString reg, bool IsCaseSensitive, CMaaPtr_<unsigned char, 1>* pm)
{
    int RegLen;
    do
    {
        //RegLen = reg.length();
        //reg.replace ("**", "*");
        RegLen = reg.Length();
        reg.ReplaceNN("**", "*");
        reg.ReplaceNN("*.*", "*");

    }   while(reg.Length() != RegLen);
    if  (reg == "*.*")
    {
        return true;
    }
    const QChar cSlash('/');
#ifdef _WIN320000
    reg.replace(QChar('/'), QChar('\\'));
    txt.replace(QChar('/'), QChar('\\'));
    cSlash = QChar('\\');
#endif
    if  (!IsCaseSensitive)
    {
        reg = reg.ToUpper(e_string_depending_all);
        txt = txt.ToUpper(e_string_depending_all);
    }
    if  (reg == "*.")
    {
        for (int i = txt.Length(); i--;)
        {
            if  (txt[/*(size_t)*/i] == cSlash)
            {
                return true;
            }
            if  (txt[/*(size_t)*/i] == QChar('.'))
            {
                return false;
            }
        }
        return true;
    }
#ifdef _UNICODE
    return CmpWCStrSimpleRegExp(::Utf8ToUnicode(txt), ::Utf8ToUnicode(reg), pm);
#else
    return CmpQStrSimpleRegExp(txt, reg, pm);
#endif
}
//--------------------------------------------------------------------------
bool CMaaString::ClearPassword() noexcept
{
    if (IsRWSingleOwner())
    {
        try
        {
            Fill();
            return true;
        }
        catch (...)
        {
        }
    }
    SetCryptoKeyBit();
    Empty();
    return false;
}
//--------------------------------------------------------------------------
bool CMaaString::IsMatchMask(const CMaaString &Mask, CMaaPtr_<unsigned char, 1>* pm) const
{
    return CmpQStrSimpleRegExp(*this, Mask, pm);
}
bool CMaaString::IsMatchUnicodeMask(const CMaaString &Mask, CMaaPtr_<unsigned char, 1>* pm) const
{
    return CmpWCStrSimpleRegExp(*this, Mask, pm);
}
//--------------------------------------------------------------------------
bool CMaaString::IsMatchFileMask(const CMaaString &Mask, int_ bCaseSensitive, CMaaPtr_<unsigned char, 1>* pm) const
{
    bool bcs;
#ifdef _WIN32
    bcs = false;
#else
    bcs = true;
#endif
    bcs = bCaseSensitive == 0 ? bcs : bCaseSensitive < 0 ? false : true;
    return CmpQStrSimpleFileExp(*this, Mask, bcs, pm);
}
//--------------------------------------------------------------------------
CMaaString CMaaString::Utf8ToAnsiEx() const noexcept(noexcept_new)
{
    const int N = Length();
    const int H = CountChars(gAscii_H);
    return H <= 0 || Utf8CharCount() > N - H + H / 2 ? *this : UnicodeToAnsi(std::move(Utf8ToUnicode(*this)));
}
//--------------------------------------------------------------------------
int CMaaString::Utf8CharCount(bool CutAsFull) const noexcept
{
    int nCount = 0;
    const unsigned char * pp = (const unsigned char *)(const char *)*this;
    const unsigned char * ee = pp + Length();
    while(pp < ee)
    {
        nCount++;
        //long UnicodeChar = *pp;
        if  ((*pp & 0xE0) == 0xC0 && ((CutAsFull && pp + 1 >= ee) || (pp + 1 < ee && (pp[1] & 0xC0) == 0x80)))
        {
            //UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
            pp += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && ((CutAsFull && pp + 2 >= ee) || (pp + 2 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
            pp += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && ((CutAsFull && pp + 3 >= ee) || (pp + 3 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
            //UnicodeChar = '_';
            pp += 4;
        }
        else
        {
            pp++;
        }
    }
    return nCount;
}

// pos0 - bytes pos
int CMaaString::Utf8PosFromPos0(int CharIndex, int pos0, bool CutAsFull) const noexcept
{
    const int Len = Length();
    if (pos0 >= Len)
    {
        return Len;
    }
    int i = 0;
    const unsigned char* pp, * pp0;
    pp0 = (const unsigned char*)(const char*)*this;
    const unsigned char* ee = pp0 + Len;
    pp = pp0 + pos0;
    while (i < CharIndex && pp < ee)
    {
        i++;
        //long UnicodeChar = *pp;
        if ((*pp & 0xE0) == 0xC0 && ((CutAsFull && pp + 1 >= ee) || (pp + 1 < ee && (pp[1] & 0xC0) == 0x80)))
        {
            //UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
            pp += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && ((CutAsFull && pp + 2 >= ee) || (pp + 2 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
            pp += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && ((CutAsFull && pp + 3 >= ee) || (pp + 3 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
            //UnicodeChar = '_';
            pp += 4;
        }
        else
        {
            pp++;
        }
    }
    //return (ptrdiff_t)(pp - pp0);
    return (int)(pp - pp0);
}

// pos0 - bytes pos
int CMaaString::Utf8CharsLenFromPos0(int CharsLen, int pos0, bool bRetErrors, bool bUtf32) const noexcept // pos0 - bytes pos. returts len of CharsLen chars in bytes, -1 on errors && bRetErrors
{
    const bool CutAsFull = true;
    const int Len = Length();
    if (pos0 >= Len)
    {
        return 0;
    }
    int i = 0;
    const unsigned char* pp, * pp0;
    pp0 = (const unsigned char*)(const char*)*this;
    const unsigned char* ee = pp0 + Len;
    pp = pp0 + pos0;
    while (i < CharsLen && pp < ee)
    {
        i++;
        //long UnicodeChar = *pp;
        if ((*pp & 0xE0) == 0xC0 && ((CutAsFull && pp + 1 >= ee) || (pp + 1 < ee && (pp[1] & 0xC0) == 0x80)))
        {
            //UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
            pp += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && ((CutAsFull && pp + 2 >= ee) || (pp + 2 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
            pp += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && ((CutAsFull && pp + 3 >= ee) || (pp + 3 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
            //UnicodeChar = '_';
            pp += 4;
            if (!bUtf32)
            {
                return -4;
            }
        }
        else
        {
            pp++;
        }
        if (pp > ee && bRetErrors)
        {
            return -1; // truncated utf char by end of string
        }
    }
    //return (ptrdiff_t)(pp - pp0);
    return (int)((pp <= ee ? pp : ee) - pp0 - pos0);
}

int CMaaString::Utf8Pos(int CharIndex, bool CutAsFull) const noexcept
{
    return Utf8PosFromPos0(CharIndex, 0, CutAsFull);
#if 0
    int i = 0;
    const unsigned char * pp, * pp0;
    pp = pp0 = (const unsigned char *)(const char *)*this;
    const unsigned char * ee = pp + Length();
    while(i < CharIndex && pp < ee)
    {
        i++;
        //long UnicodeChar = *pp;
        if  ((*pp & 0xE0) == 0xC0 && ((CutAsFull && pp + 1 >= ee) || (pp + 1 < ee && (pp[1] & 0xC0) == 0x80)))
        {
            //UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
            pp += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && ((CutAsFull && pp + 2 >= ee) || (pp + 2 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
            pp += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && ((CutAsFull && pp + 3 >= ee) || (pp + 3 < ee && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)))
        {
            //UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
            //UnicodeChar = '_';
            pp += 4;
        }
        else
        {
            pp++;
        }
    }
    //return (ptrdiff_t)(pp - pp0);
    return (int)(pp - pp0);
#endif
}

int_ CMaaString::LastChar() const noexcept
{
    const int l = Length();
    return l > 0 ? *(l - 1 + (const char *)*this) : -0x100;
}

int_ CMaaString::LastUChar() const noexcept
{
    const int l = Length();
    return l > 0 ? *(l - 1 + (const unsigned char *)(const char *)*this) : -0x100;
}

/*
void CMaaString::ClearHash() noexcept // call after direct string modifications
{
#if defined(CMAASTRING_CACHE_HASH) || defined(CMAASTRING_CACHE_HASH64)
    if  (m_pImp)
    {
        m_pImp->ClearHash();
    }
#endif
}
*/

CMaaString CMaaString::RemoveFromRight(int nCount) const noexcept(noexcept_new)
{
    return nCount >= 0 ? Left(Length() - nCount) : RefLeft(Length() + nCount);
}


bool IsCi(const char* s1, const char* s2, int Len, int_ cp) noexcept // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
{
    if (cp == e_string_depending_rus)
    {
#ifdef _UNICODE
        cp = e_utf8_rus;
#else
        cp = 1251;
#endif
    }
    else if (cp == e_string_depending)
    {
#ifdef _UNICODE
        cp = e_utf8;
#else
        cp = -1;
#endif
    }
    if (cp == e_utf8_rus || cp == e_utf8)
    {
//#ifdef _WIN32
//        CMaaString new_str = ::Utf8ToUnicode(*this);
//        CharLowerBuffW((wchar_t*)(const char*)new_str, new_str.Length() / 2);
//        return ::UnicodeToUtf8(new_str);
//#else
        const unsigned char* p1 = (const unsigned char*)s1;
        const unsigned char* p2 = (const unsigned char*)s2;
        while (Len > 0)
        {
            _dword UnicodeChar1 = *p1, UnicodeChar2 = *p2;
            if (Len >= 2 && (*p1 & 0xE0) == 0xC0 && (p1[1] & 0xC0) == 0x80)
            {
                UnicodeChar1 = (p1[1] & 0x3f) | ((*p1 & 0x1f) << 6);
                p1 += 2;
                if ((*p2 & 0xE0) == 0xC0 && (p2[1] & 0xC0) == 0x80)
                {
                    UnicodeChar2 = (p2[1] & 0x3f) | ((*p2 & 0x1f) << 6);
                    p2 += 2;
                    Len -= 2;
                }
                else
                {
                    return false;
                }
            }
            else if (Len >= 3 && (*p1 & 0xf0) == 0xe0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80)
            {
                UnicodeChar1 = (p1[2] & 0x3f) | ((p1[1] & 0x3f) << 6) | ((*p1 & 0x0f) << 12);
                p1 += 3;
                if ((*p2 & 0xf0) == 0xe0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80)
                {
                    UnicodeChar2 = (p2[2] & 0x3f) | ((p2[1] & 0x3f) << 6) | ((*p2 & 0x0f) << 12);
                    p2 += 3;
                    Len -= 3;
                }
                else
                {
                    return false;
                }
            }
            else if (Len >= 4 && (*p1 & 0xf8) == 0xf0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80 && (p1[3] & 0xc0) == 0x80)
            {
                UnicodeChar1 = (p1[3] & 0x3f) | ((p1[2] & 0x3f) << 6) | ((p1[1] & 0x3f) << 12) | ((*p1 & 0x07) << 18);
                //UnicodeChar = '_';
                p1 += 4;
                if ((*p2 & 0xf8) == 0xf0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80 && (p2[3] & 0xc0) == 0x80)
                {
                    UnicodeChar2 = (p2[3] & 0x3f) | ((p2[2] & 0x3f) << 6) | ((p2[1] & 0x3f) << 12) | ((*p2 & 0x07) << 18);
                    //UnicodeChar = '_';
                    p2 += 4;
                    Len -= 4;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                p1++;
                p2++;
                Len--;
            }

            if (UnicodeChar1 >= 0x410 && UnicodeChar1 < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar1 += 32;
            }
            else if (UnicodeChar1 == 0x401) // 'Ё'
            {
                UnicodeChar1 = 0x451; // 'ё'
            }
            else if (UnicodeChar1 >= 'A' && UnicodeChar1 <= 'Z')
            {
                UnicodeChar1 = UnicodeChar1 + 'a' - 'A';
            }

            if (UnicodeChar2 >= 0x410 && UnicodeChar2 < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar2 += 32;
            }
            else if (UnicodeChar2 == 0x401) // 'Ё'
            {
                UnicodeChar2 = 0x451; // 'ё'
            }
            else if (UnicodeChar2 >= 'A' && UnicodeChar2 <= 'Z')
            {
                UnicodeChar2 = UnicodeChar2 + 'a' - 'A';
            }

            if (UnicodeChar1 != UnicodeChar2)
            {
                return false;
            }
        }
        return !Len;
//#endif
    }

    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    const unsigned char* m = gLcMap(cp);
    for (; Len > 0; Len--)
    {
        if (m[*p1++] != m[*p2++])
        {
            return false;
        }
    }
    return !Len;
}
bool IsCi0(const char* s1, const char* s2, int Len/*1*/, int_ cp, bool bIsLeft1) noexcept // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
{
    // bIsLeft1 - not all Len (s1 len) chars can be scanned, i.e. just the left comparison
    if (Len < 0)
    {
        return false;
    }
    if (cp == e_string_depending_rus)
    {
#ifdef _UNICODE
        cp = e_utf8_rus;
#else
        cp = 1251;
#endif
    }
    else if (cp == e_string_depending)
    {
#ifdef _UNICODE
        cp = e_utf8;
#else
        cp = -1;
#endif
    }
    if (cp == e_utf8_rus || cp == e_utf8)
    {
//#ifdef _WIN32
//        CMaaString new_str = ::Utf8ToUnicode(*this);
//        CharLowerBuffW((wchar_t*)(const char*)new_str, new_str.Length() / 2);
//        return ::UnicodeToUtf8(new_str);
//#else
        const unsigned char* p1 = (const unsigned char*)s1;
        const unsigned char* p2 = (const unsigned char*)s2;
        while (Len > 0 && *p2)
        {
            _dword UnicodeChar1 = *p1, UnicodeChar2 = *p2;
            if (Len >= 2 && (*p1 & 0xE0) == 0xC0 && (p1[1] & 0xC0) == 0x80)
            {
                UnicodeChar1 = (p1[1] & 0x3f) | ((*p1 & 0x1f) << 6);
                p1 += 2;
                if ((*p2 & 0xE0) == 0xC0 && (p2[1] & 0xC0) == 0x80)
                {
                    UnicodeChar2 = (p2[1] & 0x3f) | ((*p2 & 0x1f) << 6);
                    p2 += 2;
                    Len -= 2;
                }
                else
                {
                    return false;
                }
            }
            else if (Len >= 3 && (*p1 & 0xf0) == 0xe0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80)
            {
                UnicodeChar1 = (p1[2] & 0x3f) | ((p1[1] & 0x3f) << 6) | ((*p1 & 0x0f) << 12);
                p1 += 3;
                if ((*p2 & 0xf0) == 0xe0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80)
                {
                    UnicodeChar2 = (p2[2] & 0x3f) | ((p2[1] & 0x3f) << 6) | ((*p2 & 0x0f) << 12);
                    p2 += 3;
                    Len -= 3;
                }
                else
                {
                    return false;
                }
            }
            else if (Len >= 4 && (*p1 & 0xf8) == 0xf0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80 && (p1[3] & 0xc0) == 0x80)
            {
                UnicodeChar1 = (p1[3] & 0x3f) | ((p1[2] & 0x3f) << 6) | ((p1[1] & 0x3f) << 12) | ((*p1 & 0x07) << 18);
                //UnicodeChar = '_';
                p1 += 4;
                if ((*p2 & 0xf8) == 0xf0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80 && (p2[3] & 0xc0) == 0x80)
                {
                    UnicodeChar2 = (p2[3] & 0x3f) | ((p2[2] & 0x3f) << 6) | ((p2[1] & 0x3f) << 12) | ((*p2 & 0x07) << 18);
                    //UnicodeChar = '_';
                    p2 += 4;
                    Len -= 4;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                p1++;
                p2++;
                Len--;
            }

            if (UnicodeChar1 >= 0x410 && UnicodeChar1 < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar1 += 32;
            }
            else if (UnicodeChar1 == 0x401) // 'Ё'
            {
                UnicodeChar1 = 0x451; // 'ё'
            }
            else if (UnicodeChar1 >= 'A' && UnicodeChar1 <= 'Z')
            {
                UnicodeChar1 = UnicodeChar1 + 'a' - 'A';
            }

            if (UnicodeChar2 >= 0x410 && UnicodeChar2 < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar2 += 32;
            }
            else if (UnicodeChar2 == 0x401) // 'Ё'
            {
                UnicodeChar2 = 0x451; // 'ё'
            }
            else if (UnicodeChar2 >= 'A' && UnicodeChar2 <= 'Z')
            {
                UnicodeChar2 = UnicodeChar2 + 'a' - 'A';
            }

            if (UnicodeChar1 != UnicodeChar2)
            {
                return false;
            }
        }
        return (bIsLeft1 || !Len) && !*p2;
//#endif
    }

    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    const unsigned char* m = gLcMap(cp);
    for (; Len > 0 && *p2; Len--)
    {
        if (m[*p1++] != m[*p2++])
        {
            return false;
        }
    }
    return (bIsLeft1 || !Len) && !*p2;
}
bool IsCi00(const char* s1, const char* s2, int_ cp) noexcept // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
{
    if (cp == e_string_depending_rus)
    {
#ifdef _UNICODE
        cp = e_utf8_rus;
#else
        cp = 1251;
#endif
    }
    else if (cp == e_string_depending)
    {
#ifdef _UNICODE
        cp = e_utf8;
#else
        cp = -1;
#endif
    }
    if (cp == e_utf8_rus || cp == e_utf8)
    {
        //#ifdef _WIN32
        //        CMaaString new_str = ::Utf8ToUnicode(*this);
        //        CharLowerBuffW((wchar_t*)(const char*)new_str, new_str.Length() / 2);
        //        return ::UnicodeToUtf8(new_str);
        //#else
        const unsigned char* p1 = (const unsigned char*)s1;
        const unsigned char* p2 = (const unsigned char*)s2;
        while (*p1 && *p2)
        {
            _dword UnicodeChar1 = *p1, UnicodeChar2 = *p2;
            if ((*p1 & 0xE0) == 0xC0 && (p1[1] & 0xC0) == 0x80)
            {
                UnicodeChar1 = (p1[1] & 0x3f) | ((*p1 & 0x1f) << 6);
                p1 += 2;
                if ((*p2 & 0xE0) == 0xC0 && (p2[1] & 0xC0) == 0x80)
                {
                    UnicodeChar2 = (p2[1] & 0x3f) | ((*p2 & 0x1f) << 6);
                    p2 += 2;
                }
                else
                {
                    return false;
                }
            }
            else if ((*p1 & 0xf0) == 0xe0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80)
            {
                UnicodeChar1 = (p1[2] & 0x3f) | ((p1[1] & 0x3f) << 6) | ((*p1 & 0x0f) << 12);
                p1 += 3;
                if ((*p2 & 0xf0) == 0xe0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80)
                {
                    UnicodeChar2 = (p2[2] & 0x3f) | ((p2[1] & 0x3f) << 6) | ((*p2 & 0x0f) << 12);
                    p2 += 3;
                }
                else
                {
                    return false;
                }
            }
            else if ((*p1 & 0xf8) == 0xf0 && (p1[1] & 0xc0) == 0x80 && (p1[2] & 0xc0) == 0x80 && (p1[3] & 0xc0) == 0x80)
            {
                UnicodeChar1 = (p1[3] & 0x3f) | ((p1[2] & 0x3f) << 6) | ((p1[1] & 0x3f) << 12) | ((*p1 & 0x07) << 18);
                //UnicodeChar = '_';
                p1 += 4;
                if ((*p2 & 0xf8) == 0xf0 && (p2[1] & 0xc0) == 0x80 && (p2[2] & 0xc0) == 0x80 && (p2[3] & 0xc0) == 0x80)
                {
                    UnicodeChar2 = (p2[3] & 0x3f) | ((p2[2] & 0x3f) << 6) | ((p2[1] & 0x3f) << 12) | ((*p2 & 0x07) << 18);
                    //UnicodeChar = '_';
                    p2 += 4;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                p1++;
                p2++;
            }

            if (UnicodeChar1 >= 0x410 && UnicodeChar1 < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar1 += 32;
            }
            else if (UnicodeChar1 == 0x401) // 'Ё'
            {
                UnicodeChar1 = 0x451; // 'ё'
            }
            else if (UnicodeChar1 >= 'A' && UnicodeChar1 <= 'Z')
            {
                UnicodeChar1 = UnicodeChar1 + 'a' - 'A';
            }

            if (UnicodeChar2 >= 0x410 && UnicodeChar2 < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar2 += 32;
            }
            else if (UnicodeChar2 == 0x401) // 'Ё'
            {
                UnicodeChar2 = 0x451; // 'ё'
            }
            else if (UnicodeChar2 >= 'A' && UnicodeChar2 <= 'Z')
            {
                UnicodeChar2 = UnicodeChar2 + 'a' - 'A';
            }

            if (UnicodeChar1 != UnicodeChar2)
            {
                return false;
            }
        }
        return !p1 && !*p2;
        //#endif
    }

    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    const unsigned char* m = gLcMap(cp);
    while (*p1 && *p2)
    {
        if (m[*p1++] != m[*p2++])
        {
            return false;
        }
    }
    return !*p1 && !*p2;
}

CMaaString CMaaString::ToLower(int_ cp) const noexcept(noexcept_new)// acts for englist letters only
{
    if  (cp == e_string_depending_rus)
    {
#ifdef _UNICODE
        cp = e_utf8_rus;
#else
        cp = 1251;
#endif
    }
    else if (cp == e_string_depending)
    {
#ifdef _UNICODE
        cp = e_utf8;
#else
        cp = -1;
#endif
    }
    if  (cp == e_utf8_rus || cp == e_utf8)
    {
#ifdef _WIN32
        CMaaString new_str = std::move(::Utf8ToUnicode(*this));
        CharLowerBuffW((wchar_t*)new_str.GetBuffer(), (warning_dword)(new_str.Length() / 2));
        return ::UnicodeToUtf8(new_str);
#else
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString new_str(Buffer, sizeof(Buffer), Length() + 10);
        const unsigned char * pp = (const unsigned char *)(const char *)*this;
        const unsigned char * ee = pp + Length();
        bool bChanged = false;
        while(pp < ee)
        {
            long UnicodeChar = *pp;
            if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
            {
                UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
                pp += 2;
            }
            else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
                pp += 3;
            }
            else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
                //UnicodeChar = '_';
                pp += 4;
            }
            else
            {
                pp++;
            }

            if  (UnicodeChar >= 0x410 && UnicodeChar < 0x450 - 32)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar += 32;
                bChanged = true;
            }
            else if (UnicodeChar == 0x401) // 'Ё'
            {
                UnicodeChar = 0x451; // 'ё'
                bChanged = true;
            }
            else if (UnicodeChar >= 'A' && UnicodeChar <= 'Z')
            {
                UnicodeChar = UnicodeChar + 'a' - 'A';
                bChanged = true;
            }

            char ch[4];
            if  (UnicodeChar <= 0x7f)
            {
                new_str += (char)UnicodeChar;
            }
            else if (UnicodeChar <= 0x7ff)
            {
                ch[0] = (char)(0xC0 | ((UnicodeChar >> 6) & 0x1f));
                ch[1] = (char)(0x80 | (UnicodeChar & 0x3f));
                new_str.Add(ch, 2);
            }
            else if (UnicodeChar <= 0xffff)
            {
                ch[0] = (char)(0xE0 | ((UnicodeChar >> 12) & 0x0f));
                ch[1] = (char)(0x80 | ((UnicodeChar >> 6 ) & 0x3f));
                ch[2] = (char)(0x80 | (UnicodeChar & 0x3f));
                new_str.Add(ch, 3);
            }
            else if (UnicodeChar <= 0x1fffff)
            {
                ch[0] = (char)(0xF0 | ((UnicodeChar >> 18) & 0x07));
                ch[1] = (char)(0x80 | ((UnicodeChar >> 12) & 0x3f));
                ch[2] = (char)(0x80 | ((UnicodeChar >> 6 ) & 0x3f));
                ch[3] = (char)(0x80 | (UnicodeChar & 0x3f));
                new_str.Add(ch, 4);
            }
        }
        return !bChanged ? *this : CMaaString(new_str);
#endif
    }
#if 1
    const unsigned char* p0 = (const unsigned char*)(const char*)*this;
    const unsigned char* p = p0;
    const unsigned char* e = p + Length();
    const unsigned char* m = gLcMap(cp);
    for (; p < e; p++)
    {
        if (*p != m[*p])
        {
            CMaaString Buffer = std::move(m_pImp->NewCopy());
            if (Buffer.Length() == Length())
            {
                unsigned char* pp = (unsigned char*)(char *)Buffer.GetBuffer();
                const unsigned char* ee = pp + Buffer.Length();
                pp += (p - p0);
                for (; pp < ee; pp++)
                {
                    *pp = m[*pp];
                }
            }
            return Buffer;
        }
    }
    return *this;
#else
    CMaaString Buffer = NewCopy();
    char * p = Buffer.GetBuffer();
    char * e = p + Buffer.Length();
    const unsigned char* m = gLcMap(cp);
    for (; p < e; p++)
    {
        *p = Map.m[(unsigned char)*p];
    }
    return Buffer;
#endif
}
//--------------------------------------------------------------------------
CMaaString CMaaString::ToUpper(int_ cp) const noexcept(noexcept_new) // acts for englist letters only
{
    if  (cp == e_string_depending_rus)
    {
#ifdef _UNICODE
        cp = e_utf8_rus;
#else
        cp = 1251;
#endif
    }
    else if (cp == e_string_depending)
    {
#ifdef _UNICODE
        cp = e_utf8;
#else
        cp = -1;
#endif
    }

    if  (cp == e_utf8_rus || cp == e_utf8)
    {
#ifdef _WIN32
        CMaaString new_str = std::move(::Utf8ToUnicode(*this));
        CharUpperBuffW((wchar_t*)new_str.GetBuffer(), (warning_dword)(new_str.Length() / 2));
        return ::UnicodeToUtf8(new_str);
#else
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString new_str(Buffer, sizeof(Buffer), Length() + 10);
        const unsigned char * pp = (const unsigned char *)(const char *)*this;
        const unsigned char * ee = pp + Length();
        bool bChanged = false;
        while(pp < ee)
        {
            long UnicodeChar = *pp;
            if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
            {
                UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
                pp += 2;
            }
            else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
                pp += 3;
            }
            else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
                //UnicodeChar = '_';
                pp += 4;
            }
            else
            {
                pp++;
            }

            if  (UnicodeChar >= 0x410 + 32 && UnicodeChar < 0x450)
            {
                //"АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" "абвгдежзийклмнопрстуфхцчшщъыьэюя";
                UnicodeChar -= 32;
                bChanged = true;
            }
            else if (UnicodeChar == 0x451) // 'ё'
            {
                UnicodeChar = 0x401; // 'Ё'
                bChanged = true;
            }
            else if (UnicodeChar >= 'a' && UnicodeChar <= 'z')
            {
                UnicodeChar = UnicodeChar + 'A' - 'a';
                bChanged = true;
            }

            char ch[4];
            if  (UnicodeChar <= 0x7f)
            {
                new_str += (char)UnicodeChar;
            }
            else if (UnicodeChar <= 0x7ff)
            {
                ch[0] = (char)(0xC0 | ((UnicodeChar >> 6) & 0x1f));
                ch[1] = (char)(0x80 | (UnicodeChar & 0x3f));
                new_str.Add(ch, 2);
            }
            else if (UnicodeChar <= 0xffff)
            {
                ch[0] = (char)(0xE0 | ((UnicodeChar >> 12) & 0x0f));
                ch[1] = (char)(0x80 | ((UnicodeChar >> 6 ) & 0x3f));
                ch[2] = (char)(0x80 | (UnicodeChar & 0x3f));
                new_str.Add(ch, 3);
            }
            else if (UnicodeChar <= 0x1fffff)
            {
                ch[0] = (char)(0xF0 | ((UnicodeChar >> 18) & 0x07));
                ch[1] = (char)(0x80 | ((UnicodeChar >> 12) & 0x3f));
                ch[2] = (char)(0x80 | ((UnicodeChar >> 6 ) & 0x3f));
                ch[3] = (char)(0x80 | (UnicodeChar & 0x3f));
                new_str.Add(ch, 4);
            }
        }
        return !bChanged ? *this : CMaaString(new_str);
#endif
    }

#if 1
    const unsigned char* p0 = (const unsigned char*)(const char*)*this;
    const unsigned char* p = p0;
    const unsigned char* e = p + Length();
    const unsigned char* m = gUcMap(cp);
    for (; p < e; p++)
    {
        if (*p != m[*p])
        {
            CMaaString Buffer = std::move(m_pImp->NewCopy());
            if (Buffer.Length() == Length())
            {
                unsigned char* pp = (unsigned char*)(char*)Buffer.GetBuffer();
                const unsigned char* ee = pp + Buffer.Length();
                pp += (p - p0);
                for (; pp < ee; pp++)
                {
                    *pp = m[*pp];
                }
            }
            return Buffer;
        }
    }
    return *this;
#else
    CMaaString Buffer = std::move(NewCopy());
    char * p = Buffer.GetBuffer();
    char * e = p + Buffer.Length();
    const unsigned char* m = gUcMap(cp);
    for (; p < e; p++)
    {
        *p = m[(unsigned char)*p];
    }
    return Buffer;
#endif
}
//-----------------------------------------------------------------------------------------------
int CMaaString::CountChars(char c, int Start, int End) const noexcept
{
    int n = 0;
    const char * p = *this;
    const char * e = p + (End >= 0 && End <= Length() ? End : Length());
    p += Start;
    for (; p < e; p++)
    {
        if  (*p == c)
        {
            n++;
        }
    }
    return n;
}
//--------------------------------------------------------------------------
int CMaaString::CountChars(const CMaa256Bits& Chars, int Start, int End) const noexcept
{
    int n = 0;
    const char* p = *this;
    const char* e = p + (End >= 0 && End <= Length() ? End : Length());
    p += Start;
    for (; p < e; p++)
    {
        if (Chars.Test(*p))
        {
            n++;
        }
    }
    return n;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::GetArg() noexcept(noexcept_new)
{
    CMaaString Ret;
    {
        const char* p = *this;
        int n = 0;
        const int l = Length();
        while ((p[n] == ' ' || p[n] == '\t') && n < l) n++;
        *this = std::move(RefMid(n));
    }
    if  (*(const char *)*this == '"')
    {
        *this = RefMid(1);
        const int n = Find('"');
        if  (n >= 0)
        {
            Ret = std::move(Left(n));
            *this = std::move(RefMid(n + 1));
        }
        else
        {
            Ret = *this;
            Empty();
        }
    }
    else
    {
        const int n = Find2(0, ' ', '\t');
        if  (n >= 0)
        {
            Ret = std::move(Left(n));
            *this = std::move(RefMid(n + 1));
        }
        else
        {
            Ret = *this;
            Empty();
        }
    }
    {
        const char* p = *this;
        int n = 0;
        const int l = Length();
        while ((p[n] == ' ' || p[n] == '\t') && n < l) n++;
        *this = std::move(RefMid(n));
    }

    return Ret;
}
//--------------------------------------------------------------------------
bool CMaaString::HaveWord(const char* pszWord, const CMaa256Bits& Bits) noexcept
{
    CMaaString tmp = *this;
    while (tmp.IsNotEmpty())
    {
        CMaaString w = tmp.GetWord(Bits, true, true, true);
        if (w == (char *)pszWord)
        {
            return true;
        }
    }
    return false;
}
bool CMaaString::HaveWord(const CMaaString& strWord, const CMaa256Bits& Bits) noexcept
{
    CMaaString tmp = *this;
    while (tmp.IsNotEmpty())
    {
        CMaaString w = tmp.GetWord(Bits, true, true, true);
        if (w == strWord)
        {
            return true;
        }
    }
    return false;
}
bool CMaaString::HaveWordCi(const char* pszWord, const CMaa256Bits& Bits, int_ cp) noexcept
{
    CMaaString tmp = *this;
    while (tmp.IsNotEmpty())
    {
        CMaaString w = tmp.GetWord(Bits, true, true, true);
        if (w.IsCi0(pszWord, cp))
        {
            return true;
        }
    }
    return false;
}
bool CMaaString::HaveWordCi(const CMaaString& strWord, const CMaa256Bits& Bits, int_ cp) noexcept
{
    CMaaString tmp = *this;
    while (tmp.IsNotEmpty())
    {
        CMaaString w = tmp.GetWord(Bits, true, true, true);
        if (w.IsCi(strWord, cp))
        {
            return true;
        }
    }
    return false;
}
CMaaString CMaaString::GetWord(const CMaa256Bits &Bits, bool bRemoveWord, bool bRemoveSpacesAtTheBeginning, bool bOptMid) noexcept(noexcept_new)
{
    //CMaa256Bits Bits(Spaces, Spaces.Length());
    CMaaString Ret;
    const unsigned char* p = (const unsigned char*)(const char *)*this;
    const int len = Length();
    int b = 0;
    if (bRemoveSpacesAtTheBeginning)
    {
        for (; b < len; b++)
        {
            if (!Bits.Test(p[b]))
            {
                break;
            }
        }
    }
    int l;
    for (l = b; l < len; l++)
    {
        if (Bits.Test(p[l]))
        {
            break;
        }
    }
    //Ret = RefMid(b, l - b);
#ifdef UNSAFE_GetWord0_OPTS
    if (bRemoveWord /*&& !bOptMid*/ && !IsROString())
    {
        if (l < len)
        {
            *(l + GetBuffer()) = '\0';
            //Ret = bOptMid ? RefMid(b, l - b) : Mid(b, l - b);
            Ret = RefMid(b, l - b);
            //if (bRemoveWord)
            {
                for (b = l + 1; b < len; b++)
                {
                    if (!Bits.Test(p[b]))
                    {
                        break;
                    }
                }
            }
            *this = RefMid(b);
            //*this = bOptMid ? RefMid(b) : Mid(b);
        }
        else
        {
            Ret = Str0Copy();
            Empty();
        }
        return Ret;
    }
    else
    {
        Ret = bOptMid ? RefMid(b, l - b) : Mid(b, l - b);
    }
#else
    Ret = bOptMid ? std::move(RefMid(b, l - b)) : std::move(Mid(b, l - b));
#endif
    if  (bRemoveWord)
    {
        for (b = l; b < len; b++)
        {
            if (!Bits.Test(p[b]))
            {
                break;
            }
        }
        //*this = RefMid(b);
        *this = bOptMid ? std::move(RefMid(b)) : std::move(Mid(b));
    }
    return Ret;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::GetWord(char c, bool bRemoveWord, bool bRemoveSpacesAtTheBeginning, bool bOptMid) noexcept(noexcept_new)
{
    CMaaString Ret;
    const unsigned char* p = (const unsigned char*)(const char *)*this;
    const int len = Length();
    int b = 0;
    if (bRemoveSpacesAtTheBeginning)
    {
        for (; b < len; b++)
        {
            if (p[b] != c)
            {
                break;
            }
        }
    }
    int l;
    for (l = b; l < len; l++)
    {
        if (p[l] == c)
        {
            break;
        }
    }
    //Ret = RefMid(b, l - b);
#ifdef UNSAFE_GetWord0_OPTS
    if (bRemoveWord /*&& !bOptMid*/ && !IsROString())
    {
        if (l < len)
        {
            *(l + GetBuffer()) = '\0';
            //Ret = bOptMid ? RefMid(b, l - b) : Mid(b, l - b);
            Ret = RefMid(b, l - b);
            //if (bRemoveWord)
            {
                for (b = l + 1; b < len; b++)
                {
                    if (p[b] != c)
                    {
                        break;
                    }
                }
            }
            *this = RefMid(b);
            //*this = bOptMid ? RefMid(b) : Mid(b);
        }
        else
        {
            Ret = Str0Copy();
            Empty();
        }
        return Ret;
    }
    else
    {
        Ret = bOptMid ? RefMid(b, l - b) : Mid(b, l - b);
    }
#else
    Ret = bOptMid ? std::move(RefMid(b, l - b)) : std::move(Mid(b, l - b));
#endif
    if  (bRemoveWord)
    {
        for (b = l; b < len; b++)
        {
            if (p[b] != c)
            {
                break;
            }
        }
        //*this = RefMid(b);
        *this = bOptMid ? std::move(RefMid(b)) : std::move(Mid(b));
    }
    return Ret;
}

// unsafe mod *this string
CMaaString CMaaString::GetWord0(const CMaa256Bits &Bits) noexcept(noexcept_new)
{
    if (IsROString())
    {
        *this = std::move(m_pImp->NewCopy());
    }
    //CMaa256Bits Bits(Spaces, Spaces.Length());

    CMaaString Ret;
    unsigned char* p = (unsigned char*)GetBuffer();
    const int len = Length();
    int b = 0;
    for (; b < len; b++)
    {
        if (!Bits.Test(p[b]))
        {
            break;
        }
    }
    int l;
    for (l = b; l < len; l++)
    {
        if (Bits.Test(p[l]))
        {
            break;
        }
    }
    if (l < len)
    {
        p[l] = '\0';
        Ret = RefMid(b, l - b);
        for (b = l + 1; b < len; b++)
        {
            if (!Bits.Test(p[b]))
            {
                break;
            }
        }
        *this = std::move(RefMid(b));
    }
    else
    {
        Ret = std::move(Str0Copy());
        Empty();
    }
    return Ret;
}

// unsafe mod *this string
CMaaString CMaaString::GetWord0(char c) noexcept(noexcept_new)
{
    if (IsROString())
    {
        *this = std::move(m_pImp->NewCopy());
    }

    CMaaString Ret;
    unsigned char* p = (unsigned char*)GetBuffer();
    const int len = Length();
    int b = 0;
    for (; b < len; b++)
    {
        if (p[b] != c)
        {
            break;
        }
    }
    int l;
    for (l = b; l < len; l++)
    {
        if (p[l] == c)
        {
            break;
        }
    }
    if (l < len)
    {
        p[l] = '\0';
        Ret = RefMid(b, l - b);
        for (b = l + 1; b < len; b++)
        {
            if (p[b] != c)
            {
                break;
            }
        }
        *this = std::move(RefMid(b));
    }
    else
    {
        Ret = std::move(Str0Copy());
        Empty();
    }
    return Ret;
}

CMaaString CMaaString::GetCsvWord(CMaaString SrcCodePage /*= "1251"*/) noexcept(noexcept_new)
{
    int pos = 0;
    CMaaString &Line = *this;
    CMaaString cell;
    if  (Line[0] == '"')
    {
        for (pos++;;pos++)
        {
            if  (pos >= Line.Length())
            {
                cell = std::move(Line.RefMid(1));
                cell.ReplaceNN("\"\"", "\"");
                Line.Empty();
                break;
            }
            if  (Line[pos] == '"')
            {
                if  (Line[pos + 1] == '"')
                {
                    pos++;
                }
                else if (pos + 1 == Line.Length() || Line[pos + 1] == ';')
                {
                    cell = std::move(Line.Mid(1, pos - 1));
                    cell.ReplaceNN("\"\"", "\"");
                    Line = std::move(Line.RefMid(pos + 2));
                    break;
                }
            }
        }
    }
    else
    {
        pos = Line.Find(';');
        if  (pos < 0)
        {
            cell = Line;
            Line.Empty();
        }
        else
        {
            cell = std::move(Line.Left(pos));
            Line = std::move(Line.RefMid(pos + 1));
        }
    }
    if  (SrcCodePage == "1251")
    {
        cell = std::move(UnicodeToUtf8(std::move(AnsiToUnicode(cell))));
    }
    return cell;
}

void CMaaString::AppendCsvWord(CMaaString w, CMaaString DstCodePage /*= "1251"*/) noexcept(noexcept_new)
{
    if  (DstCodePage == "1251")
    {
        w = UnicodeToAnsi(::Utf8ToUnicode(w));
    }
    w.ReplaceNN('"', "\"\"");
    if  (IsNotEmpty())
    {
        *this = *this + ";\"" + w + "\"";
    }
    else
    {
        *this = CMaaString("\"") + w + "\"";
    }
}

CMaaString CMaaString::GetLine(bool bRemove, bool bRemoveCrLf, bool bOptimizeMid, CMaaString* pCrLf) noexcept(noexcept_new)
{
    CMaaString Line;
    int len = 0;
    int eol_pos = FindCrLf(0, &len, nullptr, -1, nullptr, pCrLf);
    if (eol_pos < 0)
    {
        eol_pos = 0; // empty string
        len = 0;
    }
    const int next_pos = eol_pos + len;
#ifdef UNSAFE_GetLine0_OPTS
    if (bRemoveCrLf && bRemove && !IsROString())
    {
        if (len)
        {
            *(eol_pos + GetBuffer()) = '\0';
            Line = CMaaString(*this, 0, eol_pos);
            *this = CMaaString(*this, next_pos, Length() - next_pos); // + HasReffered()
        }
        else
        {
            Line = Str0Copy();
            Empty();
        }
        return Line;
    }
#endif
    //Line = bOptimizeMid ? RefLeft(bRemoveCrLf ? eol_pos : next_pos) : Left(bRemoveCrLf ? eol_pos : next_pos);
    Line = bOptimizeMid ? CMaaString(*this, (int_)0, bRemoveCrLf ? eol_pos : next_pos) : Left(bRemoveCrLf ? eol_pos : next_pos);
    if (bRemove)
    {
        //*this  = bOptimizeMid ? RefMid(next_pos) : Mid(next_pos);
        *this = bOptimizeMid ? CMaaString(*this, next_pos, Length() - next_pos) : Mid(next_pos);
    }
    return Line;
}
//--------------------------------------------------------------------------
// unsafe mod *this string
CMaaString CMaaString::GetLine0(CMaaString * pCrLf) noexcept(noexcept_new)
{
    if (IsROString())
    {
        *this = std::move(m_pImp->NewCopy());
    }
    CMaaString Line;
    int len = 0;
    int eol_pos = FindCrLf(0, &len, nullptr, -1, nullptr, pCrLf);
    if  (eol_pos < 0)
    {
        eol_pos = 0; // empty string
        len = 0;
    }
    const int next_pos = eol_pos + len;
    if (len)
    {
        *(eol_pos + GetBuffer()) = '\0';
        Line = CMaaString(*this, 0, eol_pos);
        *this = CMaaString(*this, next_pos, Length() - next_pos);
    }
    else
    {
        Line = std::move(Str0Copy());
        Empty();
    }
    return Line;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::GetLine(const CMaaString &crlf, bool bRemove, bool bRemoveCrLf, bool bOptimizeMid) noexcept(noexcept_new)
{
    CMaaString Line;
    int len = crlf.Length();
    int eol_pos = Find(crlf);
    if  (eol_pos < 0)
    {
        eol_pos = 0; // empty string
        len = 0;
    }
    const int next_pos = eol_pos + len;
    //Line = bOptimizeMid ? RefLeft(bRemoveCrLf ? eol_pos : next_pos) : Left(bRemoveCrLf ? eol_pos : next_pos);
    Line = bOptimizeMid ? CMaaString(*this, 0, bRemoveCrLf ? eol_pos : next_pos) : Left(bRemoveCrLf ? eol_pos : next_pos);
    if  (bRemove)
    {
        //*this = bOptimizeMid ? RefMid(next_pos) : Mid(next_pos);
        *this = bOptimizeMid ? CMaaString(*this, next_pos, Length() - next_pos) : Mid(next_pos);
    }
    return Line;
}
//--------------------------------------------------------------------------
#ifdef CMaaStringReplace

CMaaString Export(const void * p, int len)
{
    CMaaPtr_<unsigned char, 1> m(2 * len + 1);
    unsigned char * mm = m;
    for (int i = 0; i < len; i++)
    {
        unsigned char c2 = *((unsigned char *)p + i);
        unsigned char c1 = c2 >> 4;
        c2 &= 0x0f;
        c1 = c1 < 0x0a ? c1 + '0' : c1 - 0x0a + 'A';
        c2 = c2 < 0x0a ? c2 + '0' : c2 - 0x0a + 'A';
        mm[2 * (len - i - 1)] = c1;
        mm[2 * (len - i - 1) + 1] = c2;
    }
    mm[2 * len] = 0;
    return CMaaString((const char *)(unsigned char *)m, 2 * len);
}

CMaaString DataToString(const void * ptr, int len)
{
    return CMaaString::DataToString(ptr, len);
}

#endif
//--------------------------------------------------------------------------
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
// FoundStartPos or -1
int CMaaString::FindIp(int StartPos, _IP * pIp, int * pEndOfFoundIpPos, int EndPos, bool bStrict) const noexcept
{
    const int l = Length();
    //const CMaaString &txt = *this;
    const char* txt = *this;
    const char* e = txt + l;
    EndPos = EndPos >= 0 ? EndPos : l;
    EndPos = EndPos <= l ? EndPos : l;
    //1.1.1.1
    while(StartPos >= 0 && StartPos + 7 <= EndPos)
    {
        const int pos = Find(StartPos, '.', EndPos);
        if  (pos < 0)
        {
            return -1;
        }
        int FoundStartPos = pos - 1;
        while(FoundStartPos >= StartPos)
        {
            if (FoundStartPos >= 0 && txt[FoundStartPos] >= '0' && txt[FoundStartPos] <= '9')
            {
                --FoundStartPos;
            }
            else
            {
                break;
            }
        }
        ++FoundStartPos;
        const char c = FoundStartPos < EndPos ? txt[FoundStartPos] : 0;
        if  (c >= '0' && c <= '9')
        {
            int FoundEndPos = FoundStartPos;
            _IP ip = 0, tmp;

            for (int_ i = 0; i < 4; i++)
            {
                tmp = 0;
                if (FoundEndPos >= EndPos || txt[FoundEndPos] < '0' || txt[FoundEndPos] > '9')
                {
                    break;
                }
                while(FoundEndPos < EndPos && txt[FoundEndPos] >= '0' && txt[FoundEndPos] <= '9' && tmp < 0x100)
                {
                    tmp = tmp * 10 + (_IP)(txt[FoundEndPos] - '0');
                    ++FoundEndPos;
                    if  (!tmp && bStrict && FoundEndPos < EndPos && txt[FoundEndPos] >= '0' && txt[FoundEndPos] <= '9')
                    {
                        tmp = 0x100;
                        break;
                    }
                }
                if  (tmp > 0xff)
                {
                    break;
                }
                if  (FoundEndPos < EndPos && txt[FoundEndPos] != '.' && i < 3)
                {
                    break;
                }
                ip = (ip << 8) + tmp;
                if  (/*FoundEndPos < EndPos && txt[FoundEndPos] != 0 &&*/ i == 3)
                {
                    if  (FoundEndPos <= EndPos)
                    {
                        if  (pIp)
                        {
                            *pIp = ip;
                        }
                        if  (pEndOfFoundIpPos)
                        {
                            *pEndOfFoundIpPos = FoundEndPos;
                        }
                        return FoundStartPos;
                    }
                }
                ++FoundEndPos;
            }
        }
        StartPos = pos + 1;
    }
    return -1;
}
#endif
//--------------------------------------------------------------------------
#ifdef TOOLSLIB_STAT_FIND
CMaaUnivHash<CMaaString, int> ghToolsLbStatFind;
#endif
//--------------------------------------------------------------------------
bool CMaaString::KMPComputePrefixFunction(const char* pP, int m, CMaaPtr_<int, -1>& f) noexcept
{
    //const int m = Length();
    if (!f.IsValid() || (int)f.MaxIndex() < m)
    {
        return false;
    }
    /*
    if (!f.CheckResize(m))
    {
        return false;
    }
    */
    //const char* pP = *this;
    int* pf = f;
    pf[0] = 0;
    //__utf8_printf("%d", (int_)pf[0]);
    int k = 0;
    for (int q = 1; q < m; q++)
    {
        while (k > 0 && pP[k] != pP[q])
        {
            k = pf[k - 1];
        }
        if (pP[k] == pP[q])
        {
            k++;
        }
        pf[q] = k;
        //__utf8_printf(" %d", (int_)pf[q]);
    }
    //__utf8_printf("\n");
    return true;
}
//--------------------------------------------------------------------------
bool CMaaString::KMPComputePrefixFunction(CMaaPtr_<int, -1>& f) const noexcept
{
    return KMPComputePrefixFunction((const char*)*this, Length(), f);
}
//--------------------------------------------------------------------------
int CMaaString::KMPFind(int StartPos, const void* SubStr, int SubStrLen, int EndPos) const noexcept
{
    if (StartPos < 0)
    {
        return -1;
    }
    if (SubStrLen == -1)
    {
        SubStrLen = int_strlen((const char*)SubStr);
    }
#ifdef TOOLSLIB_STAT_FIND00
    {
        int xx = 0;
        ghToolsLbStatFind.Find(CMaaString((const char*)SubStr, SubStrLen), &xx);
        xx++;
        ghToolsLbStatFind.AddOver(CMaaString((const char*)SubStr, SubStrLen), xx);
    }
#endif
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStrLen > Len || SubStrLen <= 0)
    {
        return -1;
    }

    const int n = Len - StartPos;
    const int m = SubStrLen;
    CMaaPtr_<int, -1> f(m);
    const char* pT = StartPos + (const char *)*this;
    const char* pP = (const char*)SubStr;
    if (!KMPComputePrefixFunction(pP, m, f))
    {
        return FINDN_RET_FIXED_1(-2); // -1?  !! -2 === new[] err
    }
    int* pf = f;
    int q = 0;
    for (int i = 0; i < n; i++)
    {
        while (q > 0 && pP[q] != pT[i])
        {
            q = pf[q - 1];
        }
        if (pP[q] == pT[i])
        {
            q++;
        }
        if (q == m)
        {
            //Ret = i + 1 - m;
            //break;
            return StartPos + (i + 1 - m);
            //Ret = Ret < 0 ? i + 1 - m : Ret;
            //__utf8_printf("shift = %d\n", (int_)(i + 1 - m));
            //q = pf[q - 1];
        }
    }
    return -1;// Ret;
}
//--------------------------------------------------------------------------
int CMaaString::KMPFind2(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len, int SubStr2Len, int_* pIndex, int EndPos) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    if (StartPos < 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (SubStr1Len == -1)
    {
        SubStr1Len = int_strlen(SubStr1);
    }
    if (SubStr2Len == -1)
    {
        SubStr2Len = int_strlen(SubStr2);
    }
    int_ Index[2] = { 0, 1 };
    if (!SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    if (SubStr2Len && SubStr2Len < SubStr1Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStr1Len > Len || SubStr1Len <= 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (StartPos + SubStr2Len > Len || SubStr2Len <= 0)
    {
        const int r = KMPFind(StartPos, SubStr1, SubStr1Len, EndPos);
        if (pIndex)
        {
            *pIndex = Index[0];
        }
        return r;
    }

    const int MaxLen = SubStr2Len > SubStr1Len ? SubStr2Len : SubStr1Len;
    int n = Len - StartPos;
    //const int m = SubStrLen;
    CMaaPtr_<int, -1> f0(SubStr1Len), f1(SubStr2Len);
    const char* pT = StartPos + (const char*)*this;
    const char* pP[2] = { (const char*)SubStr1, (const char*)SubStr2 };
    //int SubStrLen[2] = { SubStr1Len, SubStr2Len };
    if (!KMPComputePrefixFunction(pP[0], SubStr1Len, f0) || !KMPComputePrefixFunction(pP[1], SubStr2Len, f1))
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return FINDN_RET_FIXED_1(-2); // -1?  !! -2 === new[] err
    }
    int* pf[2] = { f0, f1 };
    int q[2] = {};
    int Ret = -1;
    int_ idx = -1;
    for (int i = 0; i < n; i++)
    {
        while (q[0] > 0 && pP[0][q[0]] != pT[i])
        {
            q[0] = pf[0][q[0] - 1];
        }
        if (pP[0][q[0]] == pT[i])
        {
            q[0]++;
        }
        if (q[0] == SubStr1Len)
        {
            if (Ret < 0 || StartPos + (i + 1 - SubStr1Len) < Ret || (StartPos + (i + 1 - SubStr1Len) == Ret && Index[0] < idx))
            {
                if (Ret < 0 && i + MaxLen - SubStr1Len + 1 < n)
                {
                    n = i + MaxLen - SubStr1Len + 1;
                }
                Ret = StartPos + (i + 1 - SubStr1Len);
                idx = Index[0];
            }
            q[0] = pf[0][q[0] - 1];
        }

        while (q[1] > 0 && pP[1][q[1]] != pT[i])
        {
            q[1] = pf[1][q[1] - 1];
        }
        if (pP[1][q[1]] == pT[i])
        {
            q[1]++;
        }
        if (q[1] == SubStr2Len)
        {
            if (Ret < 0 || StartPos + (i + 1 - SubStr2Len) < Ret || (StartPos + (i + 1 - SubStr2Len) == Ret && Index[1] < idx))
            {
                if (Ret < 0 && i + MaxLen - SubStr2Len + 1 < n)
                {
                    n = i + MaxLen - SubStr2Len + 1;
                }
                Ret = StartPos + (i + 1 - SubStr2Len);
                idx = Index[1];
            }
            q[1] = pf[1][q[1] - 1];
        }
    }
    if (/*Ret >= 0 &&*/ pIndex)
    {
        *pIndex = idx;
    }
    return Ret;
}
//--------------------------------------------------------------------------
int CMaaString::KMPFind3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len, int SubStr2Len, int SubStr3Len, int_* pIndex, int EndPos) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    if (StartPos < 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (SubStr1Len == -1)
    {
        SubStr1Len = int_strlen(SubStr1);
    }
    if (SubStr2Len == -1)
    {
        SubStr2Len = int_strlen(SubStr2);
    }
    if (SubStr3Len == -1)
    {
        SubStr3Len = int_strlen(SubStr3);
    }
    int_ Index[3] = { 0, 1, 2 };
    if (!SubStr1Len && SubStr3Len)
    {
        CMaaSwap(SubStr1, SubStr3);
        CMaaSwap(SubStr1Len, SubStr3Len);
        CMaaSwap(Index[0], Index[2]);
    }
    if (!SubStr2Len && SubStr3Len)
    {
        CMaaSwap(SubStr2, SubStr3);
        CMaaSwap(SubStr2Len, SubStr3Len);
        CMaaSwap(Index[1], Index[2]);
    }
    if (!SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr2, SubStr1);
        CMaaSwap(SubStr2Len, SubStr1Len);
        CMaaSwap(Index[0], Index[1]);
    }

    if (SubStr3Len < SubStr1Len && SubStr3Len)
    {
        CMaaSwap(SubStr1, SubStr3);
        CMaaSwap(SubStr1Len, SubStr3Len);
        CMaaSwap(Index[0], Index[2]);
    }
    if (SubStr2Len < SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    if (SubStr3Len < SubStr2Len && SubStr3Len)
    {
        CMaaSwap(SubStr2, SubStr3);
        CMaaSwap(SubStr2Len, SubStr3Len);
        CMaaSwap(Index[1], Index[2]);
    }
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStr1Len > Len || SubStr1Len <= 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (StartPos + SubStr2Len > Len || SubStr2Len <= 0)
    {
        const int r = Find(StartPos, SubStr1, SubStr1Len, EndPos);
        if (pIndex)
        {
            *pIndex = Index[0];
        }
        return r;
    }
    if (StartPos + SubStr3Len > Len || SubStr3Len <= 0)
    {
        int_ idx;
        const int r = Find2(StartPos, SubStr1, SubStr2, SubStr1Len, SubStr2Len, &idx, EndPos);
        if (pIndex)
        {
            *pIndex = idx >= 0 ? Index[idx] : -1;
        }
        return r;
    }

    //int MaxLen = SubStr2Len > SubStr1Len ? SubStr2Len : SubStr1Len; ...
    const int MaxLen = SubStr3Len;
    int n = Len - StartPos;
    //const int m = SubStrLen;
    CMaaPtr_<int, -1> f0(SubStr1Len), f1(SubStr2Len), f2(SubStr3Len);
    const char* pT = StartPos + (const char*)*this;
    const char* pP[3] = { (const char*)SubStr1, (const char*)SubStr2, (const char*)SubStr3 };
    //int SubStrLen[3] = { SubStr1Len, SubStr2Len, SubStr3Len };
    if (!KMPComputePrefixFunction(pP[0], SubStr1Len, f0) || !KMPComputePrefixFunction(pP[1], SubStr2Len, f1) || !KMPComputePrefixFunction(pP[2], SubStr3Len, f2))
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return FINDN_RET_FIXED_1(-2); // -1?  !! -2 === new[] err
    }
    int* pf[3] = { f0, f1, f2 };
    int q[3] = {};
    int Ret = -1;
    int_ idx = -1;
    for (int i = 0; i < n; i++)
    {
        while (q[0] > 0 && pP[0][q[0]] != pT[i])
        {
            q[0] = pf[0][q[0] - 1];
        }
        if (pP[0][q[0]] == pT[i])
        {
            q[0]++;
        }
        if (q[0] == SubStr1Len)
        {
            if (Ret < 0 || StartPos + (i + 1 - SubStr1Len) < Ret || (StartPos + (i + 1 - SubStr1Len) == Ret && Index[0] < idx))
            {
                if (Ret < 0 && i + MaxLen - SubStr1Len + 1 < n)
                {
                    n = i + MaxLen - SubStr1Len + 1;
                }
                Ret = StartPos + (i + 1 - SubStr1Len);
                idx = Index[0];
            }
            q[0] = pf[0][q[0] - 1];
        }

        while (q[1] > 0 && pP[1][q[1]] != pT[i])
        {
            q[1] = pf[1][q[1] - 1];
        }
        if (pP[1][q[1]] == pT[i])
        {
            q[1]++;
        }
        if (q[1] == SubStr2Len)
        {
            if (Ret < 0 || StartPos + (i + 1 - SubStr2Len) < Ret || (StartPos + (i + 1 - SubStr2Len) == Ret && Index[1] < idx))
            {
                if (Ret < 0 && i + MaxLen - SubStr2Len + 1 < n)
                {
                    n = i + MaxLen - SubStr2Len + 1;
                }
                Ret = StartPos + (i + 1 - SubStr2Len);
                idx = Index[1];
            }
            q[1] = pf[1][q[1] - 1];
        }

        while (q[2] > 0 && pP[2][q[2]] != pT[i])
        {
            q[2] = pf[2][q[2] - 1];
        }
        if (pP[2][q[2]] == pT[i])
        {
            q[2]++;
        }
        if (q[2] == SubStr3Len)
        {
            if (Ret < 0 || StartPos + (i + 1 - SubStr3Len) < Ret || (StartPos + (i + 1 - SubStr3Len) == Ret && Index[2] < idx))
            {
                if (Ret < 0 && i + MaxLen - SubStr3Len + 1 < n)
                {
                    n = i + MaxLen - SubStr3Len + 1;
                }
                Ret = StartPos + (i + 1 - SubStr3Len);
                idx = Index[2];
            }
            q[2] = pf[2][q[2] - 1];
        }
    }
    if (/*Ret >= 0 &&*/ pIndex)
    {
        *pIndex = idx;
    }
    return Ret;
}
//--------------------------------------------------------------------------
int CMaaString::KMPFind(const char * pP, int m) const noexcept
{
    //const int Ret = -1;
    const int n = Length();
    m = m < 0 ? int_strlen(pP) : m;
    if (!m)
    {
        return -1;// Ret;
    }
    CMaaPtr_<int, -1> f(m);
    const char* pT = *this;
    //const char* pP = P;
    if (!KMPComputePrefixFunction(pP, m, f))
    {
        return FINDN_RET_FIXED_1(-2);
    }
    int* pf = f;
    int q = 0;
    for (int i = 0; i < n; i++)
    {
        while (q > 0 && pP[q] != pT[i])
        {
            q = pf[q - 1];
        }
        if (pP[q] == pT[i])
        {
            q++;
        }
        if (q == m)
        {
            //Ret = i + 1 - m;
            //break;
            return i + 1 - m;
            //Ret = Ret < 0 ? i + 1 - m : Ret;
            //__utf8_printf("shift = %d\n", (int_)(i + 1 - m));
            //q = pf[q - 1];
        }
    }
    return -1;// Ret;
}
//--------------------------------------------------------------------------
int CMaaString::KMPFind(const CMaaString &P) const noexcept
{
    return KMPFind(P, P.Length());
}
//--------------------------------------------------------------------------
int CMaaString::KMPFindAll(const CMaaString &P, CMaaPtr_<int, -1> &pos) const
{
    int Ret = 0;
    pos.Empty();
    const int n = Length();
    const int m = P.Length();
    if (!m)
    {
        return Ret;
    }
    CMaaPtr_<int, -1> f(m);
    const char* pT = *this;
    const char* pP = P;
    if (!KMPComputePrefixFunction(pP, m, f))
    {
        return FINDN_RET_FIXED_1(-2);
    }
    /*
    if (!pos.CheckResize(1))
    {
        return FINDN_RET_FIXED_1(-2);
    }
    pos.Empty();
    */

    int* pf = f;
    int q = 0;
    for (int i = 0; i < n; i++)
    {
        while (q > 0 && pP[q] != pT[i])
        {
            q = pf[q - 1];
        }
        if (pP[q] == pT[i])
        {
            q++;
        }
        if (q == m)
        {
            if (!pos.CheckExpand(Ret + 1))
            {
                return FINDN_RET_FIXED_1(-2);
            }
            pos[Ret++] = i + 1 - m;
            //__utf8_printf("shift = %d\n", (int_)(i + 1 - m));
            q = pf[q - 1];
        }
    }
    return Ret;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::FindCi(int StartPos, const void* SubStr, int SubStrLen, int EndPos, int_ cp) const noexcept
{
    if (StartPos < 0)
    {
        return -1;
    }
    if (SubStrLen == -1)
    {
        SubStrLen = int_strlen((const char*)SubStr);
    }
#ifdef TOOLSLIB_STAT_FIND
    {
        int xx = 0;
        ghToolsLbStatFind.Find(CMaaString((const char*)SubStr, SubStrLen), &xx);
        xx++;
        ghToolsLbStatFind.AddOver(CMaaString((const char*)SubStr, SubStrLen), xx);
    }
#endif
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStrLen > Len || SubStrLen <= 0)
    {
        return -1;
    }
    const unsigned char* m = gLcMap(cp);
    const char* w = (const char*)SubStr;
    const char* s = StartPos + (const char*)*this;
    const unsigned char ch0 = m[(unsigned char)w[0]];
    int_ Hw = 0, Hs = 0;
    for (int i = 0; i < SubStrLen; i++)
    {
        //Hw += (unsigned char)w[i];
        Hw += m[(unsigned char)w[i]];
        Hs += m[(unsigned char)s[i]];
    }
    const unsigned char ch1 = m[(unsigned char)w[SubStrLen - 1]];
    for (int pos = StartPos; ;)
    {
        if (Hw == Hs && m[(unsigned char)*s] == ch0 && m[(unsigned char)s[SubStrLen - 1]] == ch1 && ::IsCi(s, (const char *)SubStr, SubStrLen - 1, cp))
        {
            return pos;
        }
        if (++pos > Len - SubStrLen)
        {
            break;
        }
        Hs += m[(unsigned char)s[SubStrLen]] - m[(unsigned char)*s];
        s++;
    }
    return -1;
}
int CMaaString::CMaaStringImp::Find2Ci(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len, int SubStr2Len, int_* pIndex, int EndPos, int_ cp) const noexcept
{
    if (StartPos < 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (SubStr1Len == -1)
    {
        SubStr1Len = int_strlen(SubStr1);
    }
    if (SubStr2Len == -1)
    {
        SubStr2Len = int_strlen(SubStr2);
    }
    int_ Index[2] = { 0, 1 };
    if (!SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    if (SubStr2Len && SubStr2Len < SubStr1Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStr1Len > Len || SubStr1Len <= 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (StartPos + SubStr2Len > Len || SubStr2Len <= 0)
    {
        const int r = FindCi(StartPos, SubStr1, SubStr1Len, EndPos, cp);
        if (pIndex)
        {
            *pIndex = Index[0];
        }
        return r;
    }
    const unsigned char* m = gLcMap(cp);
    const char* s = StartPos + (const char*)*this;
    const unsigned char ch01 = m[(unsigned char)SubStr1[0]], ch02 = m[(unsigned char)SubStr2[0]];
    int_ Hw1 = 0, Hs1 = 0;
    int_ Hw2 = 0, Hs2 = 0;
    for (int i = 0; i < SubStr2Len; i++)
    {
        Hw2 += m[(unsigned char)SubStr2[i]];
        Hs2 += m[(unsigned char)s[i]];
    }
    for (int i = 0; i < SubStr1Len; i++)
    {
        Hw1 += m[(unsigned char)SubStr1[i]];
        Hs1 += m[(unsigned char)s[i]];
    }
    const unsigned char ch11 = m[(unsigned char)SubStr1[SubStr1Len - 1]], ch12 = m[(unsigned char)SubStr2[SubStr2Len - 1]];
    for (int pos = StartPos; ;)
    {
        if (Hw1 == Hs1 && m[(unsigned char)*s] == ch01 && m[(unsigned char)s[SubStr1Len - 1]] == ch11 && ::IsCi(s, SubStr1, SubStr1Len - 1, cp))
        {
            if (pIndex)
            {
                *pIndex = Index[0];
            }
            return pos;
        }
        if (pos + SubStr2Len <= Len && Hw2 == Hs2 && m[(unsigned char)*s] == ch02 && m[(unsigned char)s[SubStr2Len - 1]] == ch12 && ::IsCi(s, SubStr2, SubStr2Len - 1, cp))
        {
            if (pIndex)
            {
                *pIndex = Index[1];
            }
            return pos;
        }
        if (++pos > Len - SubStr1Len)
        {
            break;
        }
        Hs1 += m[(unsigned char)s[SubStr1Len]] - m[(unsigned char)*s];
        if (pos + SubStr2Len <= Len)
        {
            Hs2 += m[(unsigned char)s[SubStr2Len]] - m[(unsigned char)*s];
        }
        s++;
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
int CMaaString::CMaaStringImp::Find3Ci(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len, int SubStr2Len, int SubStr3Len, int_* pIndex, int EndPos, int_ cp) const noexcept
{
    if (StartPos < 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (SubStr1Len == -1)
    {
        SubStr1Len = int_strlen(SubStr1);
    }
    if (SubStr2Len == -1)
    {
        SubStr2Len = int_strlen(SubStr2);
    }
    if (SubStr3Len == -1)
    {
        SubStr3Len = int_strlen(SubStr3);
    }
    int_ Index[3] = { 0, 1, 2 };
    if (!SubStr1Len && SubStr3Len)
    {
        CMaaSwap(SubStr1, SubStr3);
        CMaaSwap(SubStr1Len, SubStr3Len);
        CMaaSwap(Index[0], Index[2]);
    }
    if (!SubStr2Len && SubStr3Len)
    {
        CMaaSwap(SubStr2, SubStr3);
        CMaaSwap(SubStr2Len, SubStr3Len);
        CMaaSwap(Index[1], Index[2]);
    }
    if (!SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr2, SubStr1);
        CMaaSwap(SubStr2Len, SubStr1Len);
        CMaaSwap(Index[0], Index[1]);
    }

    if (SubStr3Len < SubStr1Len && SubStr3Len)
    {
        CMaaSwap(SubStr1, SubStr3);
        CMaaSwap(SubStr1Len, SubStr3Len);
        CMaaSwap(Index[0], Index[2]);
    }
    if (SubStr2Len < SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    if (SubStr3Len < SubStr2Len && SubStr3Len)
    {
        CMaaSwap(SubStr2, SubStr3);
        CMaaSwap(SubStr2Len, SubStr3Len);
        CMaaSwap(Index[1], Index[2]);
    }
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStr1Len > Len || SubStr1Len <= 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (StartPos + SubStr2Len > Len || SubStr2Len <= 0)
    {
        const int r = FindCi(StartPos, SubStr1, SubStr1Len, EndPos, cp);
        if (pIndex)
        {
            *pIndex = Index[0];
        }
        return r;
    }
    if (StartPos + SubStr3Len > Len || SubStr3Len <= 0)
    {
        int_ idx;
        const int r = Find2Ci(StartPos, SubStr1, SubStr2, SubStr1Len, SubStr2Len, &idx, EndPos, cp);
        if (pIndex)
        {
            *pIndex = idx >= 0 ? Index[idx] : -1;
        }
        return r;
    }
    const unsigned char* m = gLcMap(cp);
    const char* s = StartPos + (const char*)*this;
    const unsigned char ch01 = m[(unsigned char)SubStr1[0]], ch02 = m[(unsigned char)SubStr2[0]], ch03 = m[(unsigned char)SubStr3[0]];
    int_ Hw1 = 0, Hs1 = 0;
    int_ Hw2 = 0, Hs2 = 0;
    int_ Hw3 = 0, Hs3 = 0;
    for (int i = 0; i < SubStr3Len; i++)
    {
        Hw3 += m[(unsigned char)SubStr3[i]];
        Hs3 += m[(unsigned char)s[i]];
    }
    for (int i = 0; i < SubStr2Len; i++)
    {
        Hw2 += m[(unsigned char)SubStr2[i]];
        Hs2 += m[(unsigned char)s[i]];
    }
    for (int i = 0; i < SubStr1Len; i++)
    {
        Hw1 += m[(unsigned char)SubStr1[i]];
        Hs1 += m[(unsigned char)s[i]];
    }
    const unsigned char ch11 = m[(unsigned char)SubStr1[SubStr1Len - 1]], ch12 = m[(unsigned char)SubStr2[SubStr2Len - 1]], ch13 = m[(unsigned char)SubStr3[SubStr3Len - 1]];
    for (int pos = StartPos; ;)
    {
        if (Hw1 == Hs1 && m[(unsigned char)*s] == ch01 && m[(unsigned char)s[SubStr1Len - 1]] == ch11 && ::IsCi(s, SubStr1, SubStr1Len - 1, cp))
        {
            if (pIndex)
            {
                *pIndex = Index[0];
            }
            return pos;
        }
        if (pos + SubStr2Len <= Len && Hw2 == Hs2 && m[(unsigned char)*s] == ch02 && m[(unsigned char)s[SubStr2Len - 1]] == ch12 && ::IsCi(s, SubStr2, SubStr2Len - 1, cp))
        {
            if (pIndex)
            {
                *pIndex = Index[1];
            }
            return pos;
        }
        if (pos + SubStr3Len <= Len && Hw3 == Hs3 && m[(unsigned char)*s] == ch03 && m[(unsigned char)s[SubStr3Len - 1]] == ch13 && ::IsCi(s, SubStr3, SubStr3Len - 1, cp))
        {
            if (pIndex)
            {
                *pIndex = Index[2];
            }
            return pos;
        }
        if (++pos > Len - SubStr1Len)
        {
            break;
        }
        Hs1 += m[(unsigned char)s[SubStr1Len]] - m[(unsigned char)*s];
        if (pos + SubStr2Len <= Len)
        {
            Hs2 += m[(unsigned char)s[SubStr2Len]] - m[(unsigned char)*s];
        }
        if (pos + SubStr3Len <= Len)
        {
            Hs3 += m[(unsigned char)s[SubStr3Len]] - m[(unsigned char)*s];
        }
        s++;
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
int CMaaString::CMaaStringImp::Find(int StartPos, const void * SubStr, int SubStrLen, int EndPos) const noexcept
{
    if  (StartPos >= 0)
    {
        SubStrLen = SubStrLen == -1 ? int_strlen((const char*)SubStr) : SubStrLen;
#ifdef TOOLSLIB_STAT_FIND
        {
            int xx = 0;
            ghToolsLbStatFind.Find(CMaaString((const char*)SubStr, SubStrLen), &xx);
            xx++;
            ghToolsLbStatFind.AddOver(CMaaString((const char*)SubStr, SubStrLen), xx);
        }
#endif
        int Len = Length();
        Len = EndPos >= 0 && EndPos <= Len ? EndPos : Len;
        if (StartPos + SubStrLen <= Len && SubStrLen > 0)
        {
            const char* w = (const char*)SubStr;
            const char* s = StartPos + (const char*)*this;
            const char ch0 = w[0];
            int_ Hw = 0, Hs = 0;
            for (int i = 0; i < SubStrLen; i++)
            {
                Hw += (unsigned char)w[i];
                Hs += (unsigned char)s[i];
            }
            const char ch1 = w[SubStrLen - 1];
            const int end = Len - SubStrLen;
            int pos = StartPos;
            while (1)
            {
                if (Hw == Hs && *s == ch0 && s[SubStrLen - 1] == ch1 && CMaaIsMemEq(s, SubStr, SubStrLen - 1))
                {
                    return pos;
                }
                if (++pos > end)
                {
                    break;
                }
                Hs += (unsigned char)s[SubStrLen] - (unsigned char)*s;
                s++;
            }
        }
    }
    return -1;
}
int CMaaString::CMaaStringImp::Find2(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len, int SubStr2Len, int_* pIndex, int EndPos) const noexcept
{
    if (StartPos < 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (SubStr1Len == -1)
    {
        SubStr1Len = int_strlen(SubStr1);
    }
    if (SubStr2Len == -1)
    {
        SubStr2Len = int_strlen(SubStr2);
    }
    int_ Index[2] = { 0, 1 };
    if (!SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    if (SubStr2Len && SubStr2Len < SubStr1Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStr1Len > Len || SubStr1Len <= 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (StartPos + SubStr2Len > Len || SubStr2Len <= 0)
    {
        const int r = Find(StartPos, SubStr1, SubStr1Len, EndPos);
        if (pIndex)
        {
            *pIndex = Index[0];
        }
        return r;
    }
    const char* s = StartPos + (const char*)*this;
    const char ch01 = SubStr1[0], ch02 = SubStr2[0];
    int_ Hw1 = 0, Hs1 = 0;
    int_ Hw2 = 0, Hs2 = 0;
    for (int i = 0; i < SubStr2Len; i++)
    {
        Hw2 += (unsigned char)SubStr2[i];
        Hs2 += (unsigned char)s[i];
    }
    for (int i = 0; i < SubStr1Len; i++)
    {
        Hw1 += (unsigned char)SubStr1[i];
        Hs1 += (unsigned char)s[i];
    }
    const char ch11 = SubStr1[SubStr1Len - 1], ch12 = SubStr2[SubStr2Len - 1];
    for (int pos = StartPos; ;)
    {
        if (Hw1 == Hs1 && *s == ch01 && s[SubStr1Len - 1] == ch11 && CMaaIsMemEq(s, SubStr1, SubStr1Len - 1))
        {
            if (pIndex)
            {
                *pIndex = Index[0];
            }
            return pos;
        }
        if (pos + SubStr2Len <= Len && Hw2 == Hs2 && *s == ch02 && s[SubStr2Len - 1] == ch12 && CMaaIsMemEq(s, SubStr2, SubStr2Len - 1))
        {
            if (pIndex)
            {
                *pIndex = Index[1];
            }
            return pos;
        }
        if (++pos > Len - SubStr1Len)
        {
            break;
        }
        Hs1 += (unsigned char)s[SubStr1Len] - (unsigned char)*s;
        if (pos + SubStr2Len <= Len)
        {
            Hs2 += (unsigned char)s[SubStr2Len] - (unsigned char)*s;
        }
        s++;
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
int CMaaString::CMaaStringImp::Find3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len, int SubStr2Len, int SubStr3Len, int_* pIndex, int EndPos) const noexcept
{
    if (StartPos < 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (SubStr1Len == -1)
    {
        SubStr1Len = int_strlen(SubStr1);
    }
    if (SubStr2Len == -1)
    {
        SubStr2Len = int_strlen(SubStr2);
    }
    if (SubStr3Len == -1)
    {
        SubStr3Len = int_strlen(SubStr3);
    }
    int_ Index[3] = { 0, 1, 2 };
    if (!SubStr1Len && SubStr3Len)
    {
        CMaaSwap(SubStr1, SubStr3);
        CMaaSwap(SubStr1Len, SubStr3Len);
        CMaaSwap(Index[0], Index[2]);
    }
    if (!SubStr2Len && SubStr3Len)
    {
        CMaaSwap(SubStr2, SubStr3);
        CMaaSwap(SubStr2Len, SubStr3Len);
        CMaaSwap(Index[1], Index[2]);
    }
    if (!SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr2, SubStr1);
        CMaaSwap(SubStr2Len, SubStr1Len);
        CMaaSwap(Index[0], Index[1]);
    }

    if (SubStr3Len < SubStr1Len && SubStr3Len)
    {
        CMaaSwap(SubStr1, SubStr3);
        CMaaSwap(SubStr1Len, SubStr3Len);
        CMaaSwap(Index[0], Index[2]);
    }
    if (SubStr2Len < SubStr1Len && SubStr2Len)
    {
        CMaaSwap(SubStr1, SubStr2);
        CMaaSwap(SubStr1Len, SubStr2Len);
        CMaaSwap(Index[0], Index[1]);
    }
    if (SubStr3Len < SubStr2Len && SubStr3Len)
    {
        CMaaSwap(SubStr2, SubStr3);
        CMaaSwap(SubStr2Len, SubStr3Len);
        CMaaSwap(Index[1], Index[2]);
    }
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    if (StartPos + SubStr1Len > Len || SubStr1Len <= 0)
    {
        if (pIndex)
        {
            *pIndex = -1;
        }
        return -1;
    }
    if (StartPos + SubStr2Len > Len || SubStr2Len <= 0)
    {
        const int r = Find(StartPos, SubStr1, SubStr1Len, EndPos);
        if (pIndex)
        {
            *pIndex = Index[0];
        }
        return r;
    }
    if (StartPos + SubStr3Len > Len || SubStr3Len <= 0)
    {
        int_ idx;
        const int r = Find2(StartPos, SubStr1, SubStr2, SubStr1Len, SubStr2Len, &idx, EndPos);
        if (pIndex)
        {
            *pIndex = idx >= 0 ? Index[idx] : -1;
        }
        return r;
    }
    const char* s = StartPos + (const char*)*this;
    const char ch01 = SubStr1[0], ch02 = SubStr2[0], ch03 = SubStr3[0];
    int_ Hw1 = 0, Hs1 = 0;
    int_ Hw2 = 0, Hs2 = 0;
    int_ Hw3 = 0, Hs3 = 0;
    for (int i = 0; i < SubStr3Len; i++)
    {
        Hw3 += (unsigned char)SubStr3[i];
        Hs3 += (unsigned char)s[i];
    }
    for (int i = 0; i < SubStr2Len; i++)
    {
        Hw2 += (unsigned char)SubStr2[i];
        Hs2 += (unsigned char)s[i];
    }
    for (int i = 0; i < SubStr1Len; i++)
    {
        Hw1 += (unsigned char)SubStr1[i];
        Hs1 += (unsigned char)s[i];
    }
    const char ch11 = SubStr1[SubStr1Len - 1], ch12 = SubStr2[SubStr2Len - 1], ch13 = SubStr3[SubStr3Len - 1];
    for (int pos = StartPos; ;)
    {
        if (Hw1 == Hs1 && *s == ch01 && s[SubStr1Len - 1] == ch11 && CMaaIsMemEq(s, SubStr1, SubStr1Len - 1))
        {
            if (pIndex)
            {
                *pIndex = Index[0];
            }
            return pos;
        }
        if (pos + SubStr2Len <= Len && Hw2 == Hs2 && *s == ch02 && s[SubStr2Len - 1] == ch12 && CMaaIsMemEq(s, SubStr2, SubStr2Len - 1))
        {
            if (pIndex)
            {
                *pIndex = Index[1];
            }
            return pos;
        }
        if (pos + SubStr3Len <= Len && Hw3 == Hs3 && *s == ch03 && s[SubStr3Len - 1] == ch13 && CMaaIsMemEq(s, SubStr3, SubStr3Len - 1))
        {
            if (pIndex)
            {
                *pIndex = Index[2];
            }
            return pos;
        }
        if (++pos > Len - SubStr1Len)
        {
            break;
        }
        Hs1 += (unsigned char)s[SubStr1Len] - (unsigned char)*s;
        if (pos + SubStr2Len <= Len)
        {
            Hs2 += (unsigned char)s[SubStr2Len] - (unsigned char)*s;
        }
        if (pos + SubStr3Len <= Len)
        {
            Hs3 += (unsigned char)s[SubStr3Len] - (unsigned char)*s;
        }
        s++;
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
int CMaaString::FindCi(int StartPos, const void* SubStr, int SubStrLen, int EndPos, int_ cp) const noexcept
{
    return m_pImp != sp_NullImp ? m_pImp->FindCi(StartPos, SubStr, SubStrLen, EndPos, cp) : -1;
}
int CMaaString::FindCi(int StartPos, const CMaaString &SubStr, int EndPos, int_ cp) const noexcept
{
    return m_pImp != sp_NullImp ? m_pImp->FindCi(StartPos, SubStr, SubStr.Length(), EndPos, cp) : -1;
}
#ifndef USE_FIND_KMP
int CMaaString::Find(int StartPos, const void* SubStr, int SubStrLen, int EndPos) const noexcept
{
    return m_pImp != sp_NullImp ? m_pImp->Find(StartPos, SubStr, SubStrLen, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find2(int StartPos, const char * SubStr1, const char * SubStr2, int SubStr1Len, int SubStr2Len, int_ * pIndex, int EndPos) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find2(StartPos, SubStr1, SubStr2, SubStr1Len, SubStr2Len, pIndex, EndPos);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len, int SubStr2Len, int SubStr3Len, int_* pIndex, int EndPos) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find3(StartPos, SubStr1, SubStr2, SubStr3, SubStr1Len, SubStr2Len, SubStr3Len, pIndex, EndPos);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
#endif
//--------------------------------------------------------------------------
int CMaaString::Find2Ci(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len, int SubStr2Len, int_* pIndex, int EndPos, int_ cp) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find2Ci(StartPos, SubStr1, SubStr2, SubStr1Len, SubStr2Len, pIndex, EndPos, cp);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find3Ci(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len, int SubStr2Len, int SubStr3Len, int_* pIndex, int EndPos, int_ cp) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find3Ci(StartPos, SubStr1, SubStr2, SubStr3, SubStr1Len, SubStr2Len, SubStr3Len, pIndex, EndPos, cp);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find(int StartPos, const CMaaString& SubStr, int EndPos) const noexcept
{
    return m_pImp != sp_NullImp ? m_pImp->Find(StartPos, SubStr, SubStr.Length(), EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find2(int StartPos, const CMaaString &SubStr1, const CMaaString &SubStr2, int_* pIndex, int EndPos) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find2(StartPos, SubStr1, SubStr2, SubStr1.Length(), SubStr2.Length(), pIndex, EndPos);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find3(int StartPos, const CMaaString &SubStr1, const CMaaString &SubStr2, const CMaaString &SubStr3, int_* pIndex, int EndPos) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find3(StartPos, SubStr1, SubStr2, SubStr3, SubStr1.Length(), SubStr2.Length(), SubStr3.Length(), pIndex, EndPos);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find2Ci(int StartPos, const CMaaString& SubStr1, const CMaaString& SubStr2, int_* pIndex, int EndPos, int_ cp) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find2Ci(StartPos, SubStr1, SubStr2, SubStr1.Length(), SubStr2.Length(), pIndex, EndPos, cp);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find3Ci(int StartPos, const CMaaString& SubStr1, const CMaaString& SubStr2, const CMaaString& SubStr3, int_* pIndex, int EndPos, int_ cp) const noexcept
{
    if (m_pImp != sp_NullImp)
    {
        return m_pImp->Find3Ci(StartPos, SubStr1, SubStr2, SubStr3, SubStr1.Length(), SubStr2.Length(), SubStr3.Length(), pIndex, EndPos, cp);
    }
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
//--------------------------------------------------------------------------
template<> int CMaaString::FindN_<0>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    return -1;
}
template<> int CMaaString::FindN_<1>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    int j = n <= 0 ? -1 : Find(StartPos, SubStr[0], SubStrLen[0], EndPos);
    if (j >= 0)
    {
        if ((Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
        if (pIndex)
        {
            *pIndex = Index[0];
        }
    }
    return j;
}
template<> int CMaaString::FindN<0>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
template<> int CMaaString::FindN<1>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    int j = Find(StartPos, SubStr[0], SubStrLen ? SubStrLen[0] : -1, EndPos);
    if (j >= 0)
    {
        if (pIndex)
        {
            *pIndex = 0;
        }
        if ((Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
    }
    else if (pIndex)
    {
        *pIndex = -1;
    }
    return j;
}
//--------------------------------------------------------------------------
template<> int CMaaString::FindN_Ci<0>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex, int_ cp) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    return -1;
}
template<> int CMaaString::FindN_Ci<1>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex, int_ cp) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    int j = n <= 0 ? -1 : FindCi(StartPos, SubStr[0], SubStrLen[0], EndPos, cp);
    if (j >= 0)
    {
        if ((Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
        if (pIndex)
        {
            *pIndex = Index[0];
        }
    }
    return j;
}
template<> int CMaaString::FindNCi<0>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags, int_ cp) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
template<> int CMaaString::FindNCi<1>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags, int_ cp) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    int j = FindCi(StartPos, SubStr[0], SubStrLen ? SubStrLen[0] : -1, EndPos, cp);
    if (j >= 0)
    {
        if (pIndex)
        {
            *pIndex = 0;
        }
        if ((Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
    }
    else if (pIndex)
    {
        *pIndex = -1;
    }
    return j;
}
//--------------------------------------------------------------------------
template<> int CMaaString::KMPFindN_<0>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, const int** pf, int_ Flags, const int_* Index, int_* pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    return -1;
}
template<> int CMaaString::KMPFindN_<1>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, const int** pf, int_ Flags, const int_* Index, int_* pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    int j = n <= 0 ? -1 : KMPFind(StartPos, SubStr[0], SubStrLen[0], EndPos);
    if (j >= 0)
    {
        if ((Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
        if (pIndex)
        {
            *pIndex = Index[0];
        }
    }
    return j;
}
template<> int CMaaString::KMPFindN<0>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, sKMPFindNCalcData<0>* f, int_ Flags) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    if (pIndex)
    {
        *pIndex = -1;
    }
    return -1;
}
template<> int CMaaString::KMPFindN<1>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, sKMPFindNCalcData<1>* f, int_ Flags) const noexcept // Typically EndPos is Length() - the position after the end of source string
{
    int j = KMPFind(StartPos, SubStr[0], SubStrLen ? SubStrLen[0] : -1, EndPos);
    if (j >= 0)
    {
        if (pIndex)
        {
            *pIndex = 0;
        }
        if ((Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
    }
    else if (pIndex)
    {
        *pIndex = -1;
    }
    return j;
}
//--------------------------------------------------------------------------

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
#define MAX_FINDN_N 10
//typedef int* pINT_CMaaTL;
////typedef const void * pcVOID_CMaaTL;
//typedef const char* pcCHAR_CMaaTL;
int CMaaString::FindN_old(int StartPos, int_ N, const char** SubStr, int* SubStrLen, int* pPos, int EndPos, int_ Flags) const noexcept // N <= 10 FindN_old(,3,...) is 2.5% lower then FindN<3>(...)
{
    //const int N0 = N;
    int_ i;
    int _SubStrLen[MAX_FINDN_N], _Pos[MAX_FINDN_N];
#if TOOLSLIB_CMAASTRING64 <= 0
    pINT_CMaaTL  _pPos[MAX_FINDN_N];
#else
    pINT64_CMaaTL  _pPos[MAX_FINDN_N];
#endif
    pcCHAR_CMaaTL _SubStr[MAX_FINDN_N];
    if (!SubStrLen)
    {
        SubStrLen = _SubStrLen;
        for (i = 0; i < N; i++)
        {
            SubStrLen[i] = -1;
        }
    }
    if (!pPos)
    {
        pPos = _Pos;
    }
    for (i = 0; i < N; i++)
    {
        pPos[i] = -1;
    }
    if (StartPos < 0)
    {
        return -1;
    }
    for (i = 0; i < N; i++)
    {
        _SubStr[i] = SubStr[i];
        _SubStrLen[i] = SubStrLen[i];
        _pPos[i] = pPos + i;
        if (SubStrLen[i] == -1)
        {
            SubStrLen[i] = int_strlen(SubStr[i]);
        }
    }
    SubStr = _SubStr;
    SubStrLen = _SubStrLen;
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    int_ j = 0;
    for (i = 0; i < N; i++)
    {
        if (SubStrLen[i] > 0 && StartPos + SubStrLen[i] <= Len)
        {
            SubStr[j] = SubStr[i];
            SubStrLen[j] = SubStrLen[i];
            _pPos[j] = _pPos[i];
            j++;
        }
    }
    if ((N = j) <= 0)
    {
        return -1;
    }
    if (N == 1)
    {
        int j = Find(StartPos, SubStr[0], SubStrLen[0], EndPos);
        if (j >= 0 && (Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
        return *(_pPos[0]) = j;
    }
    if (!(Flags & eFindN_StrOk))
    {
        for (i = 1; i < N; i++)
        {
            if (SubStrLen[i] < SubStrLen[0])
            {
                CMaaSwap(SubStr[0], SubStr[i]);
                CMaaSwap(SubStrLen[0], SubStrLen[i]);
                CMaaSwap(_pPos[0], _pPos[i]);
            }
        }
    }
    const int SubStr1Len = SubStrLen[0];
    char ch0[MAX_FINDN_N], ch1[MAX_FINDN_N];
    int_ Hw[MAX_FINDN_N], Hs[MAX_FINDN_N];
    for (i = 0; i < N; i++)
    {
        ch0[i] = SubStr[i][0];
        ch1[i] = SubStr[i][SubStrLen[i] - 1];
        Hw[i] = 0;
        Hs[i] = 0;
    }
    const char* s = StartPos + (const char*)*this;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < SubStrLen[j]; i++)
        {
            Hw[j] += (unsigned char)SubStr[j][i];
            Hs[j] += (unsigned char)s[i];
        }
    }

    for (int pos = StartPos; ;)
    {
        if (Hw[0] == Hs[0] && *s == ch0[0] && s[SubStr1Len - 1] == ch1[0] && CMaaIsMemEq(s, SubStr[0], SubStr1Len - 1))
        {
            for (i = 0; i < N; i++)
            {
                //if (pPos + i != _pPos[0])
                {
                    pPos[i] = FINDN_RET_FIXED_1(-2);
                }
            }
            return *(_pPos[0]) = pos + ((Flags & eFindN_ReturnTailPos) ? SubStr1Len : 0);
        }
        for (j = 1; j < N; j++)
        {
            if (pos + SubStrLen[j] <= Len && Hw[j] == Hs[j] && *s == ch0[j] && s[SubStrLen[j] - 1] == ch1[j] && CMaaIsMemEq(s, SubStr[j], SubStrLen[j] - 1))
            {
                for (i = 0; i < N; i++)
                {
                    //if (pPos + i != _pPos[j])
                    {
                        pPos[i] = FINDN_RET_FIXED_1(-2);
                    }
                }
                return *(_pPos[j]) = pos + ((Flags & eFindN_ReturnTailPos) ? SubStrLen[j] : 0);
            }
        }
        if (++pos > Len - SubStr1Len)
        {
            break;
        }
        Hs[0] += (unsigned char)s[SubStr1Len] - (unsigned char)*s;
        for (j = 1; j < N; j++)
        {
            if (pos + SubStrLen[j] <= Len)
            {
                Hs[j] += (unsigned char)s[SubStrLen[j]] - (unsigned char)*s;
            }
        }
        s++;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::FindNEx(int StartPos, int_ N, const char **SubStr, int *SubStrLen, int *pPos, int EndPos, int_ Flags) const
{
    //const int N0 = N;
    int_ i;
    CMaaPtr_<int, 1> _SubStrLen(N), _Pos(N);
    CMaaPtr_<int *, 1>  _pPos(N);
    CMaaPtr_<const char *, 1> _SubStr(N);
    if (!SubStrLen)
    {
        SubStrLen = _SubStrLen;
        for (i = 0; i < N; i++)
        {
            SubStrLen[i] = -1;
        }
    }
    if (!pPos)
    {
        pPos = _Pos;
    }
    for (i = 0; i < N; i++)
    {
        pPos[i] = -1;
        //_pPos[i] = pPos + i;
    }
    if (StartPos < 0)
    {
        return -1;
    }
    for (i = 0; i < N; i++)
    {
        _SubStr[i] = SubStr[i];
        _SubStrLen[i] = SubStrLen[i];
        _pPos[i] = pPos + i;
        if (SubStrLen[i] == -1)
        {
            SubStrLen[i] = int_strlen(SubStr[i]);
        }
    }
    SubStr = _SubStr;
    SubStrLen = _SubStrLen;
    const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
    int_ j = 0;
    for (i = 0; i < N; i++)
    {
        if (SubStrLen[i] > 0 && StartPos + SubStrLen[i] <= Len)
        {
            SubStr[j] = SubStr[i];
            SubStrLen[j] = SubStrLen[i];
            _pPos[j] = _pPos[i];
            j++;
        }
    }
    if ((N = j) <= 0)
    {
        return -1;
    }
    if (N == 1)
    {
        int j = Find(StartPos, SubStr[0], SubStrLen[0], EndPos);
        if (j >= 0 && (Flags & eFindN_ReturnTailPos))
        {
            j += SubStrLen[0];
        }
        return *(_pPos[0]) = j;
    }
    if (!(Flags & eFindN_StrOk))
    {
        for (i = 1; i < N; i++)
        {
            if (SubStrLen[i] < SubStrLen[0])
            {
                CMaaSwap(SubStr[0], SubStr[i]);
                CMaaSwap(SubStrLen[0], SubStrLen[i]);
                CMaaSwap(_pPos[0], _pPos[i]);
            }
        }
    }
    const int SubStr1Len = SubStrLen[0];
    CMaaPtr_<char, 1> ch0(N), ch1(N);
    CMaaPtr_<int_, 1> Hw(N), Hs(N);
    for (i = 0; i < N; i++)
    {
        ch0[i] = SubStr[i][0];
        ch1[i] = SubStr[i][SubStrLen[i] - 1];
        Hw[i] = 0;
        Hs[i] = 0;
    }
    const char* s = StartPos + (const char*)*this;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < SubStrLen[j]; i++)
        {
            Hw[j] += (unsigned char)SubStr[j][i];
            Hs[j] += (unsigned char)s[i];
        }
    }
    //s += SubStr1Len;

    for (int pos = StartPos; ;)
    {
        if (Hw[0] == Hs[0] && *s == ch0[0] && s[SubStr1Len - 1] == ch1[0] && CMaaIsMemEq(s, SubStr[0], SubStr1Len - 1))
        {
            for (i = 0; i < N; i++)
            {
                //if (pPos + i != _pPos[0])
                {
                    pPos[i] = FINDN_RET_FIXED_1(-2);
                }
            }
            return *(_pPos[0]) = pos + ((Flags & eFindN_ReturnTailPos) ? SubStr1Len : 0);
        }
        for (j = 1; j < N; j++)
        {
            if (pos + SubStrLen[j] <= Len && Hw[j] == Hs[j] && *s == ch0[j] && s[SubStrLen[j] - 1] == ch1[j] && CMaaIsMemEq(s, SubStr[j], SubStrLen[j] - 1))
            {
                for (i = 0; i < N; i++)
                {
                    //if (pPos + i != _pPos[j])
                    {
                        pPos[i] = FINDN_RET_FIXED_1(-2);
                    }
                }
                return *(_pPos[j]) = pos + ((Flags & eFindN_ReturnTailPos) ? SubStrLen[j] : 0);
            }
        }
        if (++pos > Len - SubStr1Len)
        {
            break;
        }
        Hs[0] += (unsigned char)s[SubStr1Len] - (unsigned char)*s;
        for (j = 1; j < N; j++)
        {
            if (pos + SubStrLen[j] <= Len)
            {
                Hs[j] += (unsigned char)s[SubStrLen[j]] - (unsigned char)*s;
            }
        }
        s++;
    }
    return -1;
}
#endif
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Find(int StartPos, char c, int EndPos) const noexcept
{
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                if (*p == c)
                {
                    return (int)(p - b);
                }
                p++;
            } while (--l);
        }
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Find(int StartPos, char c) const noexcept
{
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length() - StartPos;
        if (l > 0)
        {
            do
            {
                if (*p == c)
                {
                    return (int)(p - b);
                }
                p++;
            } while (--l);
        }
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Find(char c) const noexcept
{
    const char* b = *this;
    const char* p = b;
    int l = Length();
    if (l)
    {
        do
        {
            if (*p == c)
            {
                return (int)(p - b);
            }
            p++;
        } while (--l);
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Find2(int StartPos, char c1, char c2, int EndPos) const noexcept
{
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                if (*p == c1 || *p == c2)
                {
                    return (int)(p - b);
                }
                p++;
            } while (--l);
        }
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Find3(int StartPos, char c1, char c2, char c3, int EndPos) const noexcept
{
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                const char c = *p;
                if (c == c1 || c == c2 || c == c3)
                {
                    return (int)(p - b);
                }
                p++;
            } while (--l);
        }
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Find(int StartPos, const CMaa256Bits& Bits, int EndPos) const noexcept
{
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                if (Bits.Test(*p))
                {
                    return (int)(p - b);
                }
                p++;
            } while (--l);
        }
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Count(int StartPos, char c0, char c1, int EndPos) const noexcept
{
    int N = 0;
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                const char c = *p;
                if (c0 <= c && c <= c1)
                {
                    N++;
                }
                p++;
            } while (--l);
        }
    }
    return N;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Count(int StartPos, unsigned char c0, unsigned char c1, int EndPos) const noexcept
{
    int N = 0;
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                const unsigned char c = (unsigned char)*p;
                if (c0 <= c && c <= c1)
                {
                    N++;
                }
                p++;
            } while (--l);
        }
    }
    return N;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::Count(int StartPos, const CMaa256Bits& Bits, int EndPos) const noexcept
{
    int N = 0;
    if (StartPos >= 0)
    {
        const char* b = *this;
        const char* p = b + StartPos;
        int l = Length();
        l = (EndPos >= 0 && EndPos <= l ? EndPos : l) - StartPos;
        if (l > 0)
        {
            do
            {
                if (Bits.Test(*p))
                {
                    N++;
                }
                p++;
            } while (--l);
        }
    }
    return N;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::CountNum() const noexcept
{
    int N = 0;
    const char* b = *this;
    const char* p = b;
    int l = Length();
    if (l)
    {
        do
        {
            if (*p >= '0' && *p <= '9')
            {
                N++;
            }
            p++;
        } while (--l);
    }
    return N;
}
//--------------------------------------------------------------------------
int CMaaString::Find(int StartPos, char c, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Find(StartPos, c, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find(int StartPos, char c) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Find(StartPos, c) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find2(int StartPos, char c1, char c2, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Find2(StartPos, c1, c2, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find3(int StartPos, char c1, char c2, char c3, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Find3(StartPos, c1, c2, c3, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find(int StartPos, const CMaa256Bits &Bits, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Find(StartPos, Bits, EndPos) : -1;
}
int CMaaString::Count(int StartPos, char c0, char c1, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Count(StartPos, c0, c1, EndPos) : 0;
}
int CMaaString::Count(int StartPos, unsigned char c0, unsigned char c1, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Count(StartPos, c0, c1, EndPos) : 0;
}
int CMaaString::Count(int StartPos, const CMaa256Bits& Bits, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Count(StartPos, Bits, EndPos) : 0;
}
int CMaaString::CountNum(int StartPos, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Count(StartPos, '0', '9', EndPos) : 0;
}
int CMaaString::CountNum() const noexcept
{
    return PIMP(m_pImp) ? m_pImp->CountNum() : 0;
}
//--------------------------------------------------------------------------
int CMaaString::FindCi(int StartPos, char c, int EndPos, int_ cp) const noexcept
{
    if (PIMP(m_pImp))
    {
        const unsigned char* m1 = gLcMap(cp);
        const unsigned char* m2 = gUcMap(cp);
        unsigned char c1 = m1[(unsigned char)c], c2 = m2[(unsigned char)c];
        return m_pImp->Find2(StartPos, (char)c1, (char)c2, EndPos);
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find2Ci(int StartPos, char c1, char c2, int EndPos, int_ cp) const noexcept
{
    if (PIMP(m_pImp))
    {
        CMaa256Bits Bits;
        const unsigned char* m1 = gLcMap(cp);
        const unsigned char* m2 = gUcMap(cp);
        Bits.Set(m1[(unsigned char)c1]);
        Bits.Set(m1[(unsigned char)c2]);
        Bits.Set(m2[(unsigned char)c1]);
        Bits.Set(m2[(unsigned char)c2]);
        return m_pImp->Find(StartPos, Bits, EndPos);
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find3Ci(int StartPos, char c1, char c2, char c3, int EndPos, int_ cp) const noexcept
{
    if (PIMP(m_pImp))
    {
        CMaa256Bits Bits;
        const unsigned char* m1 = gLcMap(cp);
        const unsigned char* m2 = gUcMap(cp);
        Bits.Set(m1[(unsigned char)c1]);
        Bits.Set(m1[(unsigned char)c2]);
        Bits.Set(m1[(unsigned char)c3]);
        Bits.Set(m2[(unsigned char)c1]);
        Bits.Set(m2[(unsigned char)c2]);
        Bits.Set(m2[(unsigned char)c3]);
        return m_pImp->Find(StartPos, Bits, EndPos);
    }
    return -1;
}
int CMaaString::FindCi(char c, int_ cp) const noexcept
{
    return FindCi(0, c, -1, cp);
}
//--------------------------------------------------------------------------
int CMaaString::FindCrLf(int StartPos, int * pCrLfLen, int * pOffset, int EndPos, int * pAutoMovePos, CMaaString * pCrLf) const noexcept
{
    int Ret = -1;
    int_ crlf_len = 0;

    const int Len = EndPos != -1 ? EndPos : Length();
    if  (StartPos >= 0)
    {
        const char *p = *this;
        for (int i = StartPos; i < Len; i++)
        {
            const char c = p[i];
            if  (c == '\r' || c == '\n')
            {
                crlf_len = (i + 1 < Len && (p[i + 1] ^ c) == ('\r' ^ '\n')) ? 2 : 1;
                if  (pCrLf)
                {
                    // CMaaTLGlobalStrings::e_Cr, CMaaTLGlobalStrings::e_Lf, CMaaTLGlobalStrings::e_CrLf, CMaaTLGlobalStrings::e_LfCr
                    *pCrLf = CMaaTLGlobalString(CMaaTLGlobalStrings::e_Cr + (crlf_len & 2) + (1 - (c & 1)));
                }
                Ret = i;
                break;
            }
        }
        if  (StartPos < Len && Ret < 0)
        {
            Ret = Len;
        }
    }
    if  (Ret >= 0)
    {
        if  (pOffset)
        {
            *pOffset = Ret - StartPos;
        }
        if  (pAutoMovePos)
        {
            *pAutoMovePos = Ret + crlf_len; //crlf.Length();
        }
    }
    else
    {
        if  (pAutoMovePos)
        {
            // the point of consideration, ex: (EndPos < 0 ? Length() : EndPos)
            *pAutoMovePos = -1;
        }
        if  (pOffset)
        {
            *pOffset = 0;
        }
    }
    if  (pCrLfLen)
    {
        *pCrLfLen = crlf_len; // crlf.Length();
    }
    if  (!crlf_len && pCrLf)
    {
        pCrLf->Empty();
    }
    return Ret;
}
#ifndef USE_FIND_KMP
int CMaaString::Find(const char * substr) const noexcept
{
    return Find(0, substr, int_strlen(substr));
}
#endif
//--------------------------------------------------------------------------
int CMaaString::FindCi(const char* substr, int_ cp) const noexcept
{
    //return FindCi(0, substr, int_strlen(substr), -1, cp);
    return m_pImp != sp_NullImp ? m_pImp->FindCi(0, substr, int_strlen(substr), -1, cp) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::FindCi(const CMaaString &substr, int_ cp) const noexcept
{
    //return FindCi(0, substr, substr.Length(), -1, cp);
    return m_pImp != sp_NullImp ? m_pImp->FindCi(0, substr, substr.Length(), -1, cp) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::Find(char c) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->Find(c) : -1;
}
int CMaaString::Find(const CMaa256Bits& Bits) const noexcept
{
    return Find(0, Bits);
}
//--------------------------------------------------------------------------
#ifndef USE_FIND_KMP
int CMaaString::Find(const CMaaString &That) const noexcept
{
    return Find(0, (const char *)That, That.Length());
}
#endif
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFind(int StartPos, const void* SubStr, int SubStrLen, int EndPos) const noexcept
{
    if (StartPos < 0)
    {
        return -1;
    }
    if (SubStrLen == -1)
    {
        SubStrLen = int_strlen((const char*)SubStr);
    }
    const int Len = Length();
    if (StartPos + SubStrLen > Len || SubStrLen <= 0 || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    /*
    if (SubStrLen < 1)
    {
        return -1; //!StartPos;
    }
    */
    const char* w = (const char*)SubStr + SubStrLen;
    const char* s = StartPos + SubStrLen + (const char*)*this;
    int_ Hw = 0, Hs = 0;
    const char ch1 = w[-1];
    for (int i = 0; i < SubStrLen; i++)
    {
        Hw += (unsigned char)*(--w);
        Hs += (unsigned char)*(--s);
    }
    const char ch0 = *w;
    //const int SubStrLen_1 = SubStrLen - 1;
    for (int pos = StartPos; ;)
    {
        if (Hw == Hs && *s == ch0 && s[SubStrLen - 1] == ch1 && CMaaIsMemEq(s, SubStr, SubStrLen - 1))
        {
            return pos;
        }
        --pos;
        if (pos <= EndPos || pos < 0)
        {
            break;
        }
        s--;
        Hs += (unsigned char)*s - (unsigned char)s[SubStrLen];
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFindCi(int StartPos, const void* SubStr, int SubStrLen, int EndPos, int_ cp) const noexcept
{
    if (StartPos < 0)
    {
        return -1;
    }
    if (SubStrLen == -1)
    {
        SubStrLen = int_strlen((const char*)SubStr);
    }
    const int Len = Length();
    if (StartPos + SubStrLen > Len || SubStrLen <= 0 || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    /*
    if (SubStrLen < 1)
    {
        return -1; //!StartPos;
    }
    */
    const unsigned char* m = gLcMap(cp);
    const char* w = (const char*)SubStr + SubStrLen;
    const char* s = StartPos + SubStrLen + (const char*)*this;
    int_ Hw = 0, Hs = 0;
    const unsigned char ch1 = m[(unsigned char)w[-1]];
    for (int i = 0; i < SubStrLen; i++)
    {
        Hw += m[(unsigned char)*(--w)];
        Hs += m[(unsigned char)*(--s)];
    }
    const unsigned char ch0 = m[(unsigned char)*w];
    //const int SubStrLen_1 = SubStrLen - 1;
    for (int pos = StartPos; ;)
    {
        if (Hw == Hs && m[(unsigned char)*s] == ch0 && m[(unsigned char)s[SubStrLen - 1]] == ch1 && ::IsCi(s, (const char *)SubStr, SubStrLen - 1, cp))
        {
            return pos;
        }
        --pos;
        if (pos <= EndPos || pos < 0)
        {
            break;
        }
        s--;
        Hs += m[(unsigned char)*s] - m[(unsigned char)s[SubStrLen]];
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFind(int StartPos, char c, int EndPos) const noexcept
{
    const int Len = Length();
    if (StartPos < 0 || StartPos >= Len || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    const char* b = *this;
    const char* e = b + EndPos;
    const char* p = b + StartPos;
    while (p > e)
    {
        if (*p == c)
        {
            return (int)(p - b);
        }
        p--;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFind2(int StartPos, char c1, char c2, int EndPos) const noexcept
{
    const int Len = Length();
    if (StartPos < 0 || StartPos >= Len || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    const char* b = *this;
    const char* e = b + EndPos;
    const char* p = b + StartPos;
    while (p > e)
    {
        if (*p == c1 || *p == c2)
        {
            return (int)(p - b);
        }
        p--;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFind3(int StartPos, char c1, char c2, char c3, int EndPos) const noexcept
{
    const int Len = Length();
    if (StartPos < 0 || StartPos >= Len || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    const char* b = *this;
    const char* e = b + EndPos;
    const char* p = b + StartPos;
    while (p > e)
    {
        if (*p == c1 || *p == c2 || *p == c3)
        {
            return (int)(p - b);
        }
        p--;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFind(int StartPos, const CMaa256Bits& Bits, int EndPos) const noexcept
{
    const int Len = Length();
    if (StartPos < 0 || StartPos >= Len || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    const char* b = *this;
    const char* e = b + EndPos;
    const char* p = b + StartPos;
    while (p > e)
    {
        if (Bits.Test(*p))
        {
            return (int)(p - b);
        }
        p--;
    }
    return -1;
}
//--------------------------------------------------------------------------
int CMaaString::CMaaStringImp::ReverseFindCi(int StartPos, char c, int EndPos, int_ cp) const noexcept
{
    const int Len = Length();
    if (StartPos < 0 || StartPos >= Len || EndPos < -1 || EndPos >= StartPos)
    {
        return -1;
    }
    const unsigned char* m = gLcMap(cp);
    char c1 = (char)m[(unsigned char)c], c2 = (char)m[(unsigned char)c];
    const char* b = *this;
    const char* e = b + EndPos;
    const char* p = b + StartPos;
    while (p > e)
    {
        if (*p == c1 || *p == c2)
        {
            return (int)(p - b);
        }
        p--;
    }
    return -1;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
int CMaaString::ReverseFind(int StartPos, const void* SubStr, int SubStrLen, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFind(StartPos, SubStr, SubStrLen, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFindCi(int StartPos, const void* SubStr, int SubStrLen, int EndPos, int_ cp) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFindCi(StartPos, SubStr, SubStrLen, EndPos, cp) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(int StartPos, const CMaaString &SubStr, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFind(StartPos, SubStr, SubStr.Length(), EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFindCi(int StartPos, const CMaaString &SubStr, int EndPos, int_ cp) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFindCi(StartPos, SubStr, SubStr.Length(), EndPos, cp) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(int StartPos, char c, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFind(StartPos, c, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind2(int StartPos, char c1, char c2, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFind2(StartPos, c1, c2, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind3(int StartPos, char c1, char c2, char c3, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFind3(StartPos, c1, c2, c3, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(int StartPos, const CMaa256Bits &Bits, int EndPos) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFind(StartPos, Bits, EndPos) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFindCi(int StartPos, char c, int EndPos, int_ cp) const noexcept
{
    return PIMP(m_pImp) ? m_pImp->ReverseFindCi(StartPos, c, EndPos, cp) : -1;
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(const char * substr) const noexcept
{
    const int Len = (int)strlen(substr);
    return ReverseFind(Length() - Len, substr, Len);
}
//--------------------------------------------------------------------------
/*
int CMaaString::ReverseFind(const CMaaString& substr) const noexcept
{
    const int Len = substr.Length();
    return ReverseFind(Length() - Len, substr, Len);
}
*/
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(char c) const noexcept
{
    return ReverseFind(Length() - 1, c);
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind2(char c1, char c2) const noexcept
{
    return ReverseFind2(Length() - 1, c1, c2);
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind3(char c1, char c2, char c3) const noexcept
{
    return ReverseFind3(Length() - 1, c1, c2, c3);
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(const CMaa256Bits &Bits) const noexcept
{
    return ReverseFind(Length() - 1, Bits);
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFind(const CMaaString &substr) const noexcept
{
    //return ReverseFind(Length() - That.Length(), (const char *)That, That.Length());
    const int Len = substr.Length();
    return ReverseFind(Length() - Len, substr, Len);
}
//--------------------------------------------------------------------------
int CMaaString::ReverseFindCi(const CMaaString &substr, int_ cp) const noexcept
{
    //return ReverseFindCi(Length() - That.Length(), (const char *)That, That.Length(), cp);
    const int Len = substr.Length();
    return ReverseFindCi(Length() - Len, substr, Len, cp);
}
//--------------------------------------------------------------------------
int CMaaString::Fill(char c) noexcept(noexcept_new)
{
    return Fill(0, Length(), c);
}
//--------------------------------------------------------------------------
int CMaaString::Fill(int StartPos, int Len, char c) noexcept(noexcept_new)
{
    const int L = Length();
    if  (StartPos < 0 || StartPos >= L || Len <= 0)
    {
        return 0;
    }
    if  (StartPos + Len > L)
    {
        //Len = L - StartPos;
        return 0;
    }
    if  (!m_pImp->IsRWSingleOwner())
    {
        CMaaString temp = Len == L ? CMaaString(nullptr, L, (_e1_)(GetUtf1632Flags())) : m_pImp->NewCopy();
        if  (temp.IsNotEmpty())
        {
            *this = std::move(temp);
        }
    }
    if  (/*m_pImp && StartPos + Len <= L &&*/ m_pImp->IsRWSingleOwner())
    {
        memset(StartPos + GetBuffer(), (unsigned char)c, Len);
        return Len;
    }
    return 0;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::Extention(int_ WithDot) const noexcept
{
    CMaaString ext;
    const int n = ReverseFind('.');
    if  (n >= 0)
    {
        ext = std::move(RefMid(WithDot ? n : n + 1));
    }
    return ext;
}

CMaaString CMaaString::ReplaceAddExtention(const CMaaString &NewExtention) noexcept(noexcept_new)
{
    int n = ReverseFind('.');
    if  (n < 0)
    {
        n = Length();
    }
    if  (NewExtention[0] == '.')
    {
        *this = std::move(RefLeft(n) + NewExtention);
    }
    else if (NewExtention.IsNotEmpty())
    {
        *this = std::move(RefLeft(n) + "." + NewExtention);
    }
    else
    {
        *this = std::move(Left(n));
    }
    return *this;
}
//--------------------------------------------------------------------------
bool CMaaString::is_a_email(int * pErrorPos, CMaaString * pResult, int_ Flags) const
{
    int error_pos;
    int &err = pErrorPos ? *pErrorPos : error_pos;
    err = -1;
    int i;
    for (i = 0; i < Length(); i++)
    {
        const char c = (*this)[i];
        if  ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' || c == '.' || c == '-' || c == '@' ||
            (i == 0 && (Flags & 1) && c == '<' && (*this)[Length() - 1] == '>') ||
            (i > 0 && i == Length() - 1 && (Flags & 1) && c == '>' && (*this)[0] == '<')
            )
        {
            if  ((c == '-' || c == '.' || c == '@') && (i == 0 || i == Length() - 1 || (i == 1 && (*this)[0] == '<') || (i == Length() - 2 && (*this)[Length() - 1] == '>')))
            {
                err = i;
                return false;
            }
        }
        else
        {
            err = i;
            return false;
        }
    }
    CMaaString txt = *this;
    int n = txt.ReplaceNN('@', '@');
    if  (n != 1)
    {
        err = -1;
        return false;
    }
    n = txt.Find("..");
    if  (n >= 0)
    {
        err = n + 1;
        return false;
    }
    n = txt.Find(".@");
    if  (n >= 0)
    {
        err = n;
        return false;
    }
    n = txt.Find("@.");
    if  (n >= 0)
    {
        err = n + 1;
        return false;
    }
    txt = *this;
    if  ((Flags & 1) && txt[0] == '<' && txt[Length() - 1] == '>')
    {
        txt = txt.Mid(1, Length() - 2);
    }
    int shift = (Length() - txt.Length()) / 2;
    if  (txt[0] == '.')
    {
        err = 0 + shift;
        return false;
    }
    if  (txt[n = Length() - 1] == '.')
    {
        err = n + shift;
        return false;
    }
    n = txt.Find('@'); // 2018
    const int ntld = txt.ReverseFind('.');
    if  (ntld <= n) // 2018 '.' should be exist in domain name
    {
        err = txt.Length() + shift;
        return false;
    }
#ifndef TOOLSLIB_SINGLE_THREAD
#ifdef __unix__
#if TOOLSLIB_CMAASTRING64 <= 1
    if  ((Flags & 2)) // DigMx
    {
        CMaaFile log;
        //CMaaFile log("/var/ram/maa/dig_.txt", CMaaFile::eWC_SrSw, eNoExcept);
        bool bOnErrors = !(Flags & 4); // true (no error) if no 4 flag on dig exec errors
        bool Ret = bOnErrors;
        CMaaString ErrorText;
        try
        {
            CMaaString domain = txt.Mid(n + 1);
            //CMaaString cmd = CMaaString("dig -t mx ") + domain;
            C_os_StartProcess proc;
            CMaaString command = "dig";
            const char * pargs[5] = {command, "-t", "mx", domain, nullptr};
            log.fprintf("%s %s %s %s\n", pargs[0], pargs[1], pargs[2], pargs[3]);
            //proc.m_StdInSend = txt;

            //proc.m_fdStdIn = -1; // -1 got error terminate by 6 signal on deb 11
            //proc.m_fdStdOut = -3;
            //proc.m_fdStdErr = -1; // -1 got error terminate by 6 signal on deb 11

            proc.m_fdStdIn = -3; //proc.m_StdInSend.Empty();
            proc.m_fdStdOut = -3;
            proc.m_fdStdErr = -3;

            //log.fprintf("PATH=%s\n", getenv("PATH"));
            //log.fprintf("%s %s %s %s\n", pargs[0], pargs[1], pargs[2], pargs[3]);
            Ret = proc.fork_and_StartProcess(C_os_StartProcess::eWaitChild|C_os_StartProcess::eSearchPath, command, pargs); //envs, bThrow);
            log.fprintf("%s ", Ret ? "true" : "false");
            int_ ExitCode = Ret ? proc.GetExitCode() : -1;
            log.fprintf("%d\n", ExitCode);
            if  (ExitCode == 0)
            {
                CMaaString txt = proc.m_StdOutRecv;
                log.fprintf("==========================================\n%S\n==========================================\n", &txt);
                Ret = false;
                while(txt.IsNotEmpty() && !Ret)
                {
                    CMaaString Line = txt.GetLine();
                    if  (Line.IsEmpty() || Line[0] == ';')
                    {
                        continue;
                    }
                    // YANDEX.ru.              135     IN      MX      10 mx.yandex.ru.
                    CMaaString d = Line.GetWord();
                    CMaaString to = Line.GetWord();
                    CMaaString i = Line.GetWord();
                    CMaaString t = Line.GetWord();
                    if  (t.IsCi("mx", 2, 0))
                    {
                        log.fprintf("t = %S, OK!\n", &t);
                        Ret = true;
                    }
                }
            }
            else
            {
                log.fprintf("on != 0 exit code\n");
                Ret = bOnErrors; // unchecked
            }
        }
        catch(XTOOFile2Error err)
        {
            ErrorText = err.GetMsg();
            log.fprintf("%S\n", &ErrorText);
            Ret = bOnErrors;
        }
        catch(const char * err)
        {
            ErrorText = err;
            log.fprintf("%S\n", &ErrorText);
            Ret = bOnErrors;
        }
        catch(...)
        {
            ErrorText = "catch(...)";
            log.fprintf("...\n", &ErrorText);
            Ret = bOnErrors;
        }
        if  (!Ret)
        {
            err = n + 1 + shift;
            return false;
        }
    }
#endif
#endif
#endif
    // root domain name can not be numerical in human readable form now,
    // but root domain name can be .xn--80aswg (.СЃР°Р№С‚) for example
    /*
    for (n = ntld + 1; n < txt.Length(); n++)
    {
        char c = txt[n];
        if  ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
        }
        else
        {
            // (c >= '0' && c <= '9') = error
            err = n + shift;
            return false;
        }
    }
    */
    n = -2;
    if  (pResult)
    {
        *pResult = txt;
    }
    return true;
}
//--------------------------------------------------------------------------
bool CMaaString::is_a_domain(int * pErrorPos) noexcept(noexcept_new)
{
    int error_pos;
    int &err = pErrorPos ? *pErrorPos : error_pos;
    err = -1;
    int i;
    CMaaString d = ToLower();
    const int dLen = d.Length();
    for (i = 0; i < dLen; i++)
    {
        char c = d[i];
        if  ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-')
        {
            if  (c == '-' || c == '.')
            {
                if  (i == 0 || i == dLen - 1)
                {
                    err = i;
                    return false;
                }
                c = d[i - 1];
                if  (c == '.')
                {
                    err = i;
                    return false;
                }
                if  (c == '-')
                {
                    if  (/*(d[i] == '-' && d[i - 2] == '-') ||*/ d[i] == '.')
                    {
                        err = i;
                        return false;
                    }
                }
            }
        }
        else
        {
            err = i;
            return false;
        }
    }
    //if (d.ReplaceNN(".", "."))
    //if (d.ReplaceNN(".", ".", true))
    if  (d.Find('.') >= 0)
    {
        *this = std::move(d);
        return true;
    }
    err = Length();
    return false;
}
//--------------------------------------------------------------------------
#if TOOLSLIB_CMAASTRING64 <= 1
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::ToCanonicalUrl(CMaaString* pRs, CMaaString* pHost, CMaaString* pPath, CMaaString* pArgs, CMaaString* pIntHost, bool bIntRet) const noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_32K];
    CMaaString rs, host, inthost, args, src = *this;
    CMaaConcatString path(Buffer, sizeof(Buffer));
    int pos = src.Find('/');
    if (pos > 0 && src[pos - 1] == ':' && src[pos + 1] == '/')
    {
        rs = CMaaString(src, 0, pos + 2).ToLower(e_utf8_rus);
        src = src.RefMid(pos + 2);
        pos = src.Find('/');
    }
    else if (pos == 0 && src[1] == '/')
    {
        rs = CMaaString(src, 2);
        src = src.RefMid(2);
        pos = src.Find('/');
    }
    pos = pos < 0 ? src.Length() : pos;
    host = CMaaString(src, 0, pos).ToLower(e_utf8_rus);
    src = src.RefMid(pos);
    int a = src.Find('?');
    a = a < 0 ? src.Length() : a;
    args = src.RefMid(a);
    src = CMaaString(src, 0, a);
    while (src.IsNotEmpty())
    {
        const int pos1 = src.Find('/');
        pos = pos1 < 0 ? src.Length() : pos1;
        path += CMaaString(src, 0, pos).decodeURIComponent().encodeURIComponent();
        if (pos1 >= 0)
        {
            path += '/';
            src = src.RefMid(pos + 1);
        }
        else
        {
            break;
        }
    }

    CMaaString tmp = path;
    if (pRs)
    {
        *pRs = rs;
    }
    inthost = host;
    IDN_Encode(host, host);
    if (pHost)
    {
        *pHost = host;
    }
    if (pIntHost || bIntRet)
    {
        IDN_Decode(inthost, inthost);
    }
    if (pIntHost)
    {
        *pIntHost = inthost;
    }
    if (pPath)
    {
        *pPath = tmp;
    }
    if (pArgs)
    {
        *pArgs = args;
    }
    return rs + (!bIntRet ? host : inthost) + tmp + args;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::IDN_EncodeDomainUrl() const noexcept(noexcept_new)
{
    CMaaString UrlPfx, Domain, UrlPath;
    if  (SplitUrl(UrlPfx, Domain, UrlPath))
    {
        IDN_Encode(Domain, Domain);
        return UrlPfx + Domain + UrlPath;
    }
    return *this;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::IDN_DecodeDomainUrl() const noexcept(noexcept_new)
{
    CMaaString UrlPfx, Domain, UrlPath;
    if  (SplitUrl(UrlPfx, Domain, UrlPath))
    {
        IDN_Decode(Domain, Domain);
        return UrlPfx + Domain + UrlPath;
    }
    return *this;
}
//-----------------------------------------------------------------------------------------------
#endif
bool CMaaString::SplitUrl(CMaaString &UrlPfx, CMaaString &Domain, CMaaString &UrlPath) const noexcept(noexcept_new)
{
    // "http://" "СЃСѓР±.РґРѕРј.СЂС„" "/path"
    Domain = *this;
    int pos = Domain.Find('/');
    if  (pos > 0 && Domain[pos - 1] == ':' && Domain[pos + 1] == '/')
    {
        pos += 2;
    }
    else if (pos == 0 && Domain[pos + 1] == '/')
    {
        pos += 2;
    }
    else
    {
        pos = -1;
    }
    if  (pos > 0)
    {
        UrlPfx = std::move(Domain.Left(pos));
        Domain = std::move(Domain.RefMid(pos));
        pos = Domain.Find('/');
        if  (pos > 0)
        {
            UrlPath = std::move(Domain.Mid(pos));
            Domain = std::move(Domain.Left(pos));
        }
    }
    Domain = std::move(Domain.ToLower(e_utf8_rus));
    return true;
}
//static constexpr CMaa256Bits gFsProhibitedChars(szOTHER_FILESYSTEM_SLASH ":;*?\"<>|&!%$\0", 14, false);
//--------------------------------------------------------------------------
CMaaString CMaaString::ToFsAllowed(bool brus, bool bdirectoryes, int * pErrNum, int_ fCutPath) const noexcept(noexcept_new)
{
    // fCutPath: 0 - not cut
    //           1 - cut os-dependend (unix implementation)
    //           2 - cut (unix + Windows (todo) using unix implementation only
    //          -1 - default (currently as =1)

    CMaaString Ret;
    
    //static const char * pszProhibitedChars { szOTHER_FILESYSTEM_SLASH ":;*?\"<>|\0" }; // && < ' ' //
    int nErr = 0;
    if  (!bdirectoryes && (Find("../") >= 0 || Find("..\\") >= 0))
    {
        if  (pErrNum)
        {
            *pErrNum = -1;
        }
        return Ret;
    }
    /*if  ((Find(':') >= 0)
    {
        if  (pErrNum)
        {
            *pErrNum = -1;
        }
        return Ret;
    }*/
    //if   (cp == e_utf8_rus || cp == e_utf8)
    {
        char Buffer[4096/*PATH_MAX=4096 linux MAXPATHLEN=1024 linux MAX_PATH=260 windows*/ + 2048];
        CMaaConcatString new_str(Buffer, sizeof(Buffer), Length() + 10);
        const unsigned char * pp = (const unsigned char *)(const char *)*this;
        const unsigned char * pp0;
        const unsigned char * ee = pp + Length();
        //const unsigned char * _pp = pp;
        const CMaa256Bits& sFsProhibitedChars_ = gFsProhibitedChars;
        while(pp < ee && *pp == ' ') pp++; // for windows
        while(pp < ee && ee[-1] == ' ') ee--; // for windows
        while(pp < ee)
        {
            int_ type = 0;
            _sdword UnicodeChar = *pp;
            pp0 = pp;
            if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
            {
                UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
                pp += 2;
            }
            else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
                pp += 3;
            }
            else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
                // UnicodeChar = '_';
                type = -1; // act as '_' replacement // windows disallowed chars
                //type = 8; // act as rus chars
                pp += 4;
            }
            else
            {
                pp++;
            }

            if  (!UnicodeChar)
            {
                type = -2; // null char inside the name
            }
            else if  (UnicodeChar < ' ' || (UnicodeChar < 256 && sFsProhibitedChars_.Test((char)UnicodeChar)/*strchr(pszProhibitedChars, (char)UnicodeChar)*/))
            {
                type = -1; // windows disallowed chars
            }
            else if  (UnicodeChar >= 'a' && UnicodeChar <= 'z')
            {
                type = 1;
            }
            else if  (UnicodeChar >= 'A' && UnicodeChar <= 'Z')
            {
                type = 2;
            }
            else if  (UnicodeChar >= '0' && UnicodeChar <= '9')
            {
                type = 3;
            }
            //if  (UnicodeChar < 256 && strchr(" _" /*"/\\"*/ szFILESYSTEM_SLASH "(),-.^~", (char)UnicodeChar))
            else if  (UnicodeChar >= ' ' && UnicodeChar < 256) // < 128 ? // && !strchr(pszProhibitedChars, (char)UnicodeChar))
            {
                type = 4;
            }
            else if  (UnicodeChar >= 0x410 && UnicodeChar < 0x450 - 32)
            {
                type = 5;
            }
            else if  (UnicodeChar >= 0x410 + 32 && UnicodeChar < 0x450)
            {
                type = 7;
            }
            else if  (UnicodeChar == 0x401) // 'Ё'
            {
                type = 5;
            }
            else if  (UnicodeChar == 0x451)
            {
                type = 7;
            }

            if  (type < 0 || ((!type || type >= 5) && !brus))
            {
                new_str += '_';
                nErr++;
            }
            else
            {
                new_str.Add((char *)pp0, (int)(pp - pp0));
            }
        }
        Ret = (CMaaString)new_str;
    }
    //Ret = *this;
    if  (pErrNum)
    {
        *pErrNum = nErr;
    }
    if  (fCutPath)
    {
#ifdef __unix__
        // using
        // #define NAME_MAX         255  /* # chars in a file name */
        // --- #define PATH_MAX        4096 /* # chars in a path name including nul */
        //
        CMaaString fn = Ret;
        Ret.Empty();
        int pos;
        while((pos = fn.Find(FILESYSTEM_SLASH)) >= 0)
        {
            CMaaString d = fn.RefLeft(pos);
            if  (d.Length() > NAME_MAX)
            {
                int x = d.RefLeft(NAME_MAX).Utf8CharCount();
                d = d.RefLeft(fn.Utf8Pos(x));
                while(d.Length() > NAME_MAX)
                {
                    x = d.Utf8CharCount();
                    d = d.RefLeft(fn.Utf8Pos(x - 1));
                }
            }
            Ret += d + szFILESYSTEM_SLASH;
            fn = fn.RefMid(pos + 1);
        }
        if  (fn.Length() > NAME_MAX)
        {
            CMaaString ext = fn.Extention();
            fn -= -ext.Length();
            if  (ext.Length() > NAME_MAX / 2)
            {
                int x = ext.RefLeft(NAME_MAX / 2).Utf8CharCount();
                ext = ext.RefLeft(ext.Utf8Pos(x));
            }
            while(ext.Length() > NAME_MAX / 2)
            {
                int x = ext.Utf8CharCount();
                ext = ext.RefLeft(ext.Utf8Pos(x - 1));
            }
            fn = ext + fn;
            if  (fn.Length() > NAME_MAX)
            {
                int x = fn.RefLeft(NAME_MAX).Utf8CharCount();
                fn = fn.RefLeft(fn.Utf8Pos(x));
            }
            while(fn.Length() > NAME_MAX)
            {
                int x = fn.Utf8CharCount();
                fn = fn.RefLeft(fn.Utf8Pos(x - 1));
            }
            fn = (fn.RefMid(ext.Length()) + ext).s().Str0Copy();
        }
        Ret += fn;
#endif
    }
    return Ret;
}
//--------------------------------------------------------------------------
CMaaString CMaaString::ToFsAndShellAllowed(bool brus, bool bdirectoryes, int * pErrNum) const noexcept(noexcept_new)
{
    CMaaString Ret;
    int nErr = 0;
    if  (!bdirectoryes && (Find("../") >= 0 || Find("..\\") >= 0))
    {
        if  (pErrNum)
        {
            *pErrNum = -1;
        }
        return Ret;
    }
    //if   (cp == e_utf8_rus || cp == e_utf8)
    {
        char Buffer[4096/*PATH_MAX=4096 linux MAXPATHLEN=1024 linux MAX_PATH=260 windows*/ + 2048];
        CMaaConcatString new_str(Buffer, sizeof(Buffer), Length() + 10);
        const unsigned char * pp = (const unsigned char *)(const char *)*this;
        const unsigned char * pp0;
        const unsigned char * ee = pp + Length();
        const unsigned char * _pp = pp;
        while(pp < ee)
        {
            _sdword UnicodeChar = *pp;
            pp0 = pp;
            if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
            {
                UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
                pp += 2;
            }
            else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
                pp += 3;
            }
            else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
                //                    UnicodeChar = '_';
                pp += 4;
            }
            else
            {
                pp++;
            }

            int_ type = 0;
            if  (UnicodeChar >= 'a' && UnicodeChar <= 'z')
            {
                type = 1;
            }
            if  (UnicodeChar >= 'A' && UnicodeChar <= 'Z')
            {
                type = 2;
            }
            if  (UnicodeChar >= '0' && UnicodeChar <= '9')
            {
                type = 3;
            }
            if  (UnicodeChar == ' ' || UnicodeChar == '_' || UnicodeChar == '/' || UnicodeChar == '%')
            {
                type = 4;
            }
            if  (UnicodeChar >= ' ' && UnicodeChar <= '@' && UnicodeChar != '!' && UnicodeChar != '"')
            {
                type = 4;
            }
            if  (pp0 != _pp && (UnicodeChar == '.' || UnicodeChar == '-' || UnicodeChar == '=' || UnicodeChar == '(' || UnicodeChar == ')'))
            {
                type = 4;
            }
            if  (UnicodeChar >= 0x410 && UnicodeChar < 0x450 - 32)
            {
                type = 5;
            }
            if  (UnicodeChar >= 0x410 + 32 && UnicodeChar < 0x450)
            {
                type = 7;
            }
            if  (UnicodeChar == 0x401) // 'Ё'
            {
                type = 5;
            }
            if  (UnicodeChar == 0x451)
            {
                type = 7;
            }

            if  (type >= 5 && !brus)
            {
                new_str += '_';
                nErr++;
            }
            else if (type == 0)
            {
                new_str += '_';
                nErr++;
            }
            else
            {
                new_str.Add((char *)pp0, (int)(pp - pp0));
            }
        }
        Ret = (CMaaString)new_str;
    }
    //Ret = *this;
    if  (pErrNum)
    {
        *pErrNum = nErr;
    }
    return Ret;
}
//--------------------------------------------------------------------------
int CMaaString::Copy(int pos, const void * ptr, int size) noexcept(noexcept_new)
{
    if  (pos < 0 || size < 0)
    {
        return -1;
    }
    if  ((_qword)pos + (_qword)size > (_qword)Length())
    {
        return -1;
    }
    if (!size)
    {
        return 0;
    }
    if  (!m_pImp->IsRWSingleOwner())
    {
        CMaaString temp = m_pImp->NewCopy();
        /*
          if   (temp.m_pImp)
          {
               *this = temp;
          }
          */
        if  (temp.Length() == Length())
        {
            memmove(pos + temp.GetBuffer(), ptr, size);
            *this = std::move(temp);
            return size;
        }
        return -1;
    }
    memmove(pos + GetBuffer(), ptr, size);
    return size;
}
//--------------------------------------------------------------------------
int CMaaString::Copy(int pos, const CMaaString &src, int src_offset, int length) noexcept(noexcept_new)
{
    if  (src_offset >= 0 && length >= 0 && (_qword)src_offset + (_qword)length <= (_qword)src.Length())
    {
        return Copy(pos, src_offset + (const char *)src, length);
    }
    return -1;
}
//--------------------------------------------------------------------------
// Java-style System.arraycopy(src, pos, dst, src_offset, src_length);
// --> dst.Copy(src, pos, src_offset, src_length);
//--------------------------------------------------------------------------
int CMaaString::Copy(const CMaaString &src, int src_offset, int dst_offset, int length) noexcept(noexcept_new)
{
    return Copy(dst_offset, src, src_offset, length);
}
//--------------------------------------------------------------------------
CMaaString CMaaString::UnicodeLittleUnmarked() noexcept(noexcept_new)
{
    CMaaString r(nullptr, 2 * Length(), CMaaString::eNotInitMem);
    if  ((_qword)r.Length() == (_qword)2 * (_qword)Length())
    {
        char * d = r.GetBuffer();
        const char * s = (const char *)*this;
        for (int i = Length(); i--; )
        {
            *d++ = *s++;
            *d++ = 0;
        }
        return r;
    }
    return CMaaStringZ;
}
//--------------------------------------------------------------------------

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
void CMaaString::StrCutSlash(char * Str)
{
    if  (Str[0])
    {
        if  (Str[strlen(Str) - 1] == '\\')
        {
            Str[strlen(Str) - 1] = 0;
        }
    }
}

void CMaaString::StrAddSlash(char * Str)
{
    if  (Str[0])
    {
        if  (Str[strlen(Str) - 1] != '\\')
        {
            strcat(Str, "\\");
        }
    }
    else
    {
        strcpy(Str, "\\");
    }
}
#endif

//==============================================================================
//char gChar0_00000000_2 = 0;
/*
CMaaString::XX::operator const char& () const noexcept
{
    //return (idx >= 0 && idx < s.Length()) ? *(idx + (const char*)s) : (sChar0_2[0] = 0);
    return (s.m_pImp && idx < s.m_pImp->m_iLength && idx >= 0) ? s.m_pImp->m_pszStr[idx] : (sChar0_2[0] = 0);
}
const char& CMaaString::operator[](int idx) const noexcept
{
    return (m_pImp && idx < m_pImp->m_iLength && idx >= 0) ? m_pImp->m_pszStr[idx] : (sChar0_2[0] = 0);
}
const char& CMaaString::operator[](size_t idx) const noexcept
{
    return (m_pImp && idx < (size_t)m_pImp->m_iLength && idx >= 0) ? m_pImp->m_pszStr[idx] : (sChar0_2[0] = 0);
}
*/

CMaaString GetLogDatePath(time_t t) noexcept
{
    CMaaString r;
    const tm * x = localtime(&t);
    if  (!x)
    {
        r = CMaaTLGlobalString2(CMaaTLGlobalStrings::eGetDateErr); // "error_getting_date";
    }
    else
    {
        r.Format("%04d%c%02d%c%02d", x->tm_year + 1900, FILESYSTEM_SLASH, x->tm_mon + 1, FILESYSTEM_SLASH, x->tm_mday);
    }
    return r;
}

CMaaString GetTextDate(time_t t, int_ mode) noexcept
{
    CMaaString r;
    const tm * x = localtime(&t);
    if  (!x)
    {
        r = CMaaTLGlobalString2(CMaaTLGlobalStrings::eGetDateErr); // "error_getting_date";
    }
    else
    {
        switch (mode & 0xff)
        {
        case 2:
            r.Format("%02d.%02d.%d", x->tm_mday, x->tm_mon + 1, x->tm_year + 1900);
            break;
        case 0:
            r.Format("%2d.%02d.%d", x->tm_mday, x->tm_mon + 1, x->tm_year + 1900);
            break;
        case '/':
            r.Format("%04d/%02d/%02d", x->tm_year + 1900, x->tm_mon + 1, x->tm_mday);
            break;
        case 'S': // SQL Date
        case '-': // SQL Date
            r.Format("%04d-%02d-%02d", x->tm_year + 1900, x->tm_mon + 1, x->tm_mday);
            break;
        default:
            r.Format("%d.%02d.%d", x->tm_mday, x->tm_mon + 1, x->tm_year + 1900);
        }
    }
    return r;
}

CMaaString GetTextTime(time_t t) noexcept
{
    CMaaString r;
    const tm * x = localtime(&t);
    if  (!x)
    {
        r = CMaaTLGlobalString2(CMaaTLGlobalStrings::eGetTimeErr); // "error_getting_time";
    }
    else
    {
        r.Format("%02d:%02d:%02d", x->tm_hour, x->tm_min, x->tm_sec);
    }
    return r;
}

CMaaString GetTextDateTime(time_t t, int_ mode) noexcept
{
    CMaaString r;
    const tm * x = localtime(&t);
    if (!x)
    {
        r = CMaaTLGlobalString2(CMaaTLGlobalStrings::eGetTimeErr); // "error_getting_time";
    }
    else if ((mode & 0x100)) // TimeDate
    {
        switch(mode & 0xff)
        {
        case 2:
            r.Format("%02d:%02d:%02d %02d.%02d.%d", x->tm_hour, x->tm_min, x->tm_sec, x->tm_mday, x->tm_mon + 1, x->tm_year + 1900);
            break;
        case 0:
            r.Format("%02d:%02d:%02d %2d.%02d.%d", x->tm_hour, x->tm_min, x->tm_sec, x->tm_mday, x->tm_mon + 1, x->tm_year + 1900);
            break;
        case '/':
            r.Format("%02d:%02d:%02d %04d/%02d/%02d", x->tm_hour, x->tm_min, x->tm_sec, x->tm_year + 1900, x->tm_mon + 1, x->tm_mday);
            break;
        case 'S': // SQL Date
        case '-': // SQL Date
            r.Format("%02d:%02d:%02d %04d-%02d-%02d", x->tm_hour, x->tm_min, x->tm_sec, x->tm_year + 1900, x->tm_mon + 1, x->tm_mday);
            break;
        default:
            r.Format("%02d:%02d:%02d %d.%02d.%d", x->tm_hour, x->tm_min, x->tm_sec, x->tm_mday, x->tm_mon + 1, x->tm_year + 1900);
        }
    }
    else // DateTime
    {
        switch (mode & 0xff)
        {
        case 2:
            r.Format("%02d.%02d.%d %02d:%02d:%02d", x->tm_mday, x->tm_mon + 1, x->tm_year + 1900, x->tm_hour, x->tm_min, x->tm_sec);
            break;
        case 0:
            r.Format("%2d.%02d.%d %02d:%02d:%02d", x->tm_mday, x->tm_mon + 1, x->tm_year + 1900, x->tm_hour, x->tm_min, x->tm_sec);
            break;
        case '/':
            r.Format("%04d/%02d/%02d %02d:%02d:%02d", x->tm_year + 1900, x->tm_mon + 1, x->tm_mday, x->tm_hour, x->tm_min, x->tm_sec);
            break;
        case 'S': // SQL Date
        case '-': // SQL Date
            r.Format("%04d-%02d-%02d %02d:%02d:%02d", x->tm_year + 1900, x->tm_mon + 1, x->tm_mday, x->tm_hour, x->tm_min, x->tm_sec);
            break;
        default:
            r.Format("%d.%02d.%d %02d:%02d:%02d", x->tm_mday, x->tm_mon + 1, x->tm_year + 1900, x->tm_hour, x->tm_min, x->tm_sec);
        }
    }
    return r;
}

/*
CMaaString GetTextTimeDate(time_t t, int_ mode) noexcept
{
    return GetTextDateTime(t, mode | 0x100);
}
*/

CMaaString GetTextTimeDiff(double x) noexcept // x - time difference int secounds, by example diff of GetTickCount()/1000.0
{
    _qword t = (_qword)(x + (x >= 0.0 ? 0.5 : -0.5));
    CMaaString r;
    const char * pSign = t < 0 ? "-" : "";
    t = t < 0 ? -t : t;
    const int_ s = (int_)(t % 60); t /= 60;
    const int_ m = (int_)(t % 60); t /= 60;
    const int_ h = (int_)(t % 24); t /= 24;
    r.Format2("%s%D%02d%02d%02d", t ? "%1%2.%3:%4:%5" : h ? "%1%3:%4:%5" : m ? "%1%4:%5" : "%1%5", pSign, t, h, m, s);
    return r;
}

CMaaString GetTextTimeDiff3(double x) noexcept // x - time difference int secounds, by example diff of GetTickCount()/1000.0
{
    x *= 1000.0;
    _qword t = (_qword)(x + (x >= 0.0 ? 0.5 : -0.5));
    CMaaString r;
    const char * pSign = t < 0 ? "-" : "";
    t = t < 0 ? -t : t;
    const int_ ms = (int_)(t % 1000); t /= 1000;
    const int_ s = (int_)(t % 60); t /= 60;
    const int_ m = (int_)(t % 60); t /= 60;
    const int_ h = (int_)(t % 24); t /= 24;
    r.Format2("%s%D%02d%02d%02d%03d", t ? "%1%2.%3:%4:%5.%6" : h ? "%1%3:%4:%5.%6" : m ? "%1%4:%5.%6" : s ? "%1%5.%6":  "%10.%6", pSign, t, h, m, s, ms);
    return r;
}

CMaaString GetTextDateOrTimeToday(time_t t, time_t current_time, int_ mode) noexcept
{
    CMaaString txt = std::move(GetTextDate(t, mode));
    CMaaString currdate = std::move(GetTextDate(current_time, mode));
    if  (txt == currdate)
    {
        txt = std::move(GetTextTime(t));
    }
    return txt;
}

#if TOOLSLIB_CMAASTRING64 <= 1
const char* g_pszEngMonth[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
const char* g_pszEngDayOfWeek[8] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
const char* g_pszEngMonthLong[12] = { "January","February","March","April","May","June","July","August","September","October","November","December" };
const char* g_pszEngDayOfWeekLong[8] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

#ifdef _WIN32
const char* g_pszRusMonth[12] = { (const char *)u8"Янв", (const char*)u8"Фев", (const char*)u8"Мар", (const char*)u8"Апр", (const char*)u8"Май", (const char*)u8"Июн", (const char*)u8"Июл", (const char*)u8"Авг", (const char*)u8"Сен", (const char*)u8"Окт", (const char*)u8"Ноя", (const char*)u8"Дек"};
const char* g_pszRusDayOfWeek[8] = { (const char*)u8"Вс", (const char*)u8"Пн", (const char*)u8"Вт", (const char*)u8"Ср", (const char*)u8"Чт", (const char*)u8"Пт", (const char*)u8"Сб", (const char*)u8"Вс" };
const char* g_pszRusMonthLong[12] = { (const char*)u8"Январь", (const char*)u8"Февраль", (const char*)u8"Март", (const char*)u8"Апрель", (const char*)u8"Май", (const char*)u8"Июнь", (const char*)u8"Июль", (const char*)u8"Август", (const char*)u8"Сентябрь", (const char*)u8"Октябрь", (const char*)u8"Ноябрь", (const char*)u8"Декабрь" };
const char* g_pszRusMonthLong2[12] = { (const char*)u8"Января", (const char*)u8"Февраля", (const char*)u8"Марта", (const char*)u8"Апреля", (const char*)u8"Мая", (const char*)u8"Июня", (const char*)u8"Июля", (const char*)u8"Августа", (const char*)u8"Сентября", (const char*)u8"Октября", (const char*)u8"Ноября", (const char*)u8"Декабря" };
const char* g_pszRusDayOfWeekLong[8] = { (const char*)u8"Воскресенье", (const char*)u8"Понедельник", (const char*)u8"Вторник", (const char*)u8"Среда", (const char*)u8"Четверг", (const char*)u8"Пятница", (const char*)u8"Суббота", (const char*)u8"Воскресенье" };
#else
// __unix__
const char* g_pszRusMonth[12] = { "РЇРЅРІ", "Р¤РµРІ", "РњР°СЂ", "РђРїСЂ", "РњР°Р№", "РСЋРЅ", "РСЋР»", "РђРІРі", "РЎРµРЅ", "РћРєС‚", "РќРѕСЏ", "Р”РµРє"};
const char* g_pszRusDayOfWeek[8] = { "Р’СЃ", "РџРЅ", "Р’С‚", "РЎСЂ", "Р§С‚", "РџС‚", "РЎР±", "Р’СЃ" };
const char* g_pszRusMonthLong[12] = { "РЇРЅРІР°СЂСЊ", "Р¤РµРІСЂР°Р»СЊ", "РњР°СЂС‚", "РђРїСЂРµР»СЊ", "РњР°Р№", "РСЋРЅСЊ", "РСЋР»СЊ", "РђРІРіСѓСЃС‚", "РЎРµРЅС‚СЏР±СЂСЊ", "РћРєС‚СЏР±СЂСЊ", "РќРѕСЏР±СЂСЊ", "Р”РµРєР°Р±СЂСЊ" };
const char* g_pszRusMonthLong2[12] = { "РЇРЅРІР°СЂСЏ", "Р¤РµРІСЂР°Р»СЏ", "РњР°СЂС‚Р°", "РђРїСЂРµР»СЏ", "РњР°СЏ", "РСЋРЅСЏ", "РСЋР»СЏ", "РђРІРіСѓСЃС‚Р°", "РЎРµРЅС‚СЏР±СЂСЏ", "РћРєС‚СЏР±СЂСЏ", "РќРѕСЏР±СЂСЏ", "Р”РµРєР°Р±СЂСЏ" };
const char* g_pszRusDayOfWeekLong[8] = { "Р’РѕСЃРєСЂРµСЃРµРЅСЊРµ", "РџРѕРЅРµРґРµР»СЊРЅРёРє", "Р’С‚РѕСЂРЅРёРє", "РЎСЂРµРґР°", "Р§РµС‚РІРµСЂРі", "РџСЏС‚РЅРёС†Р°", "РЎСѓР±Р±РѕС‚Р°", "Р’РѕСЃРєСЂРµСЃРµРЅСЊРµ" };
#endif
#endif

#if 0
/*
январь января январю январь январём январе
февраль февраля февралю февраль февралём феврале
март марта марту март мартом марте
апрель апреля апрелю апрель апрелем апреле
май мая маю май маем мае
июнь июня июню июнь июнем июне
июль июля июлю июль июлем июле
август августа августу август августом августе
сентябрь сентября сентябрю сентябрь сентябрём сентябре
октябрь октября октябрю октябрь октябрём октябре
ноябрь ноября ноябрю ноябрь ноябрём ноябре
декабрь декабря декабрю декабрь декабрём декабре

понедельник
вторник
среда
четверг
пятница
суббота
воскресенье
*/

/*
January January January January January January
February February February February February
March March March March March
April April April April April
May May May May May May
June June June June June June
July July July July July July
August August August August August August
September September September September September September
October October October October October
November November November November November November
December December December December December December

Monday
Tuesday
Wednesday
Thursday
Friday
Saturday
Sunday
*/

/*
СЏРЅРІР°СЂСЊ СЏРЅРІР°СЂСЏ СЏРЅРІР°СЂСЋ СЏРЅРІР°СЂСЊ СЏРЅРІР°СЂС‘Рј СЏРЅРІР°СЂРµ
С„РµРІСЂР°Р»СЊ С„РµРІСЂР°Р»СЏ С„РµРІСЂР°Р»СЋ С„РµРІСЂР°Р»СЊ С„РµРІСЂР°Р»С‘Рј С„РµРІСЂР°Р»Рµ
РјР°СЂС‚ РјР°СЂС‚Р° РјР°СЂС‚Сѓ РјР°СЂС‚ РјР°СЂС‚РѕРј РјР°СЂС‚Рµ
Р°РїСЂРµР»СЊ Р°РїСЂРµР»СЏ Р°РїСЂРµР»СЋ Р°РїСЂРµР»СЊ Р°РїСЂРµР»РµРј Р°РїСЂРµР»Рµ
РјР°Р№ РјР°СЏ РјР°СЋ РјР°Р№ РјР°РµРј РјР°Рµ
РёСЋРЅСЊ РёСЋРЅСЏ РёСЋРЅСЋ РёСЋРЅСЊ РёСЋРЅРµРј РёСЋРЅРµ
РёСЋР»СЊ РёСЋР»СЏ РёСЋР»СЋ РёСЋР»СЊ РёСЋР»РµРј РёСЋР»Рµ
Р°РІРіСѓСЃС‚ Р°РІРіСѓСЃС‚Р° Р°РІРіСѓСЃС‚Сѓ Р°РІРіСѓСЃС‚ Р°РІРіСѓСЃС‚РѕРј Р°РІРіСѓСЃС‚Рµ
СЃРµРЅС‚СЏР±СЂСЊ СЃРµРЅС‚СЏР±СЂСЏ СЃРµРЅС‚СЏР±СЂСЋ СЃРµРЅС‚СЏР±СЂСЊ СЃРµРЅС‚СЏР±СЂС‘Рј СЃРµРЅС‚СЏР±СЂРµ
РѕРєС‚СЏР±СЂСЊ РѕРєС‚СЏР±СЂСЏ РѕРєС‚СЏР±СЂСЋ РѕРєС‚СЏР±СЂСЊ РѕРєС‚СЏР±СЂС‘Рј РѕРєС‚СЏР±СЂРµ
РЅРѕСЏР±СЂСЊ РЅРѕСЏР±СЂСЏ РЅРѕСЏР±СЂСЋ РЅРѕСЏР±СЂСЊ РЅРѕСЏР±СЂС‘Рј РЅРѕСЏР±СЂРµ
РґРµРєР°Р±СЂСЊ РґРµРєР°Р±СЂСЏ РґРµРєР°Р±СЂСЋ РґРµРєР°Р±СЂСЊ РґРµРєР°Р±СЂС‘Рј РґРµРєР°Р±СЂРµ

РїРѕРЅРµРґРµР»СЊРЅРёРє
РІС‚РѕСЂРЅРёРє
СЃСЂРµРґР°
С‡РµС‚РІРµСЂРі
РїСЏС‚РЅРёС†Р°
СЃСѓР±Р±РѕС‚Р°
РІРѕСЃРєСЂРµСЃРµРЅСЊРµ
*/

#endif

/*
    CMaaString bd_eng = GetBuildDate(__TIMESTAMP__);
    CMaaString bd_rus = GetBuildDate(__TIMESTAMP__, g_pszRusDayOfWeek, "Время сборки: ");
    __utf8_printf("%s\n%S\n%S\n", __TIMESTAMP__, &bd_eng, &bd_rus);
*/

// "Mon Jun 4 09:38:30 2007" --> "Build date: Mon, 4.07.2007 09:38:30"
// __TIMESTAMP__ is a source file modification date, not a date of compillation
CMaaString GetBuildDate(CMaaString TimeStamp, const char** Dow, const char* pszBuildDate) noexcept
{
    CMaaString s;
    int_ d, m, y, hh, mm, ss;
    CMaaString ts7 = TimeStamp.RefMid(7);
    //int n = sscanf(ts7, "%d %02d:%02d:%02d %4d", &d, &hh, &mm, &ss, &y);
    int n = ts7.dsscanf("%d %d:%d:%d %d", &d, &hh, &mm, &ss, &y);
    CMaaString Month = TimeStamp.RefMid(4, 3);
    m = 0;
    for (int_ i = 0; i < 12; i++)
    {
        if (Month == (char *)g_pszEngMonth[i])
        {
            m = i + 1;
            break;
        }
    }
    if (n == 5 && m > 0)
    {
        s = std::move(TimeStamp.RefLeft(3));
        if (Dow)
        {
            for (int_ i = 0; i < 7; i++)
            {
                if (s == (char*)g_pszEngDayOfWeek[i])
                {
                    s = CMaaString(Dow[i], CMaaString::eROStrlenMemString);
                    break;
                }
            }
        }
        s.Format("%s%S, %d.%02d.%d %02d:%02d:%02d", pszBuildDate ? pszBuildDate : "", &s, d, m, y, hh, mm, ss);
    }
    else
    {
        s.Format("%s%S", pszBuildDate ? pszBuildDate : "", &TimeStamp);
    }
    return s;
}

// for CMS
CMaaString GetHumanLikeTextDate(time_t t, int_ mode /*=0*/, const CMaaString* const Month, const CMaaString& strY, const CMaaString* const Dow /*= nullptr*/) noexcept
{
    const tm* x = localtime(&t);
    if (!x)
    {
        return CMaaTLGlobalString2(CMaaTLGlobalStrings::eGetDateErr); // "error_getting_date";
    }

    CMaaString s;
    if (mode == 2)
    {
        s.Format("%02d %S %d%S", x->tm_mday, &Month[x->tm_mon], x->tm_year + 1900, &strY);
    }
    else if (mode == 0)
    {
        s.Format("%2d %S %d%S", x->tm_mday, &Month[x->tm_mon], x->tm_year + 1900, &strY);
    }
    else
    {
        s.Format("%d %S %d%S", x->tm_mday, &Month[x->tm_mon], x->tm_year + 1900, &strY);
    }
    if (Dow)
    {
        s.Append(", %S", &Dow[(x->tm_wday + 6) % 7]);
    }
    return s;
}

// new, for other
CMaaString GetHumanLikeTextDate(time_t t, int_ mode /*=0*/, const char ** Month, const CMaaString &strY, const char ** Dow /*= nullptr*/, bool bTime) noexcept
{
    const tm * x = localtime(&t);
    if  (!x)
    {
        return CMaaTLGlobalString2(CMaaTLGlobalStrings::eGetDateErr); // "error_getting_date";
    }

    CMaaString s;
    if  (mode == 2)
    {
        s.Format("%02d %s %d%S", x->tm_mday, Month[x->tm_mon], x->tm_year + 1900, &strY);
    }
    else if (mode == 0)
    {
        s.Format("%2d %s %d%S", x->tm_mday, Month[x->tm_mon], x->tm_year + 1900, &strY);
    }
    else
    {
        s.Format("%d %s %d%S", x->tm_mday, Month[x->tm_mon], x->tm_year + 1900, &strY);
    }
    if  (Dow)
    {
        s.Append(", %s", Dow[x->tm_wday % 7]);
    }
    if (bTime)
    {
        s.Append(", %02d:%02d:%02d", x->tm_hour, x->tm_min, x->tm_sec);
    }
    return s;
}

/*
CMaaString CMaaSqlDate(time_t t) noexcept
{
    return GetTextDate(t, 'S'); // "2022-12-31"
}
*/

time_t ConvertTextDateTime(const CMaaString &Date, const CMaaString &Time, int_ mode, bool bStrong) noexcept
{
    time_t t = (time_t)-1;
    int Time_Length = Time.Length();

    const char cDiv = (mode == 'S' || mode == '-') ? '-' : mode == '/' ? '/' : '.';
    const char* p = Date;
    const char* e = p + Date.Length();
    while(p < e && *p == ' ') p++;

    int n = 0;
    int_ x = 0;
    while(p < e && p[n] >= '0' && p[n] <= '9')
    {
        x = x * 10 + (p[n] - '0');
        n++;
    }
    if  (n > 0 && p[n] == cDiv)
    {
        tm tt;
        memset(&tt, 0, sizeof(tt));

        if  (cDiv != '.')
        {
            tt.tm_year = x - 1900;
        }
        else
        {
            tt.tm_mday = x;
        }
        p += n + 1;

        n = 0; x = 0;
        while(p < e && p[n] >= '0' && p[n] <= '9')
        {
            x = x * 10 + (p[n] - '0');
            n++;
        }
        if  (n > 0 && p[n] == cDiv)
        {
            tt.tm_mon = x - 1;
            p += n + 1;

            n = 0; x = 0;
            while(p < e && p[n] >= '0' && p[n] <= '9')
            {
                x = x * 10 + (p[n] - '0');
                n++;
            }
            p += n;
            if  (n > 0 && (p == e || *p == ' '))
            {
                if  (cDiv != '.')
                {
                    tt.tm_mday = x;
                }
                else
                {
                    tt.tm_year = x - 1900;
                }
                while(p < e && *p == ' ') p++;
                if  (p != e && !bStrong)
                {
                    //Time.Empty();
                    Time_Length = 0;
                }
                if  (p == e || !bStrong)
                {
                    p = Time;
                    e = p + Time_Length;// Time.Length();

                    while(p < e && *p == ' ') p++;

                    n = 0, x = 0;
                    while(p < e && p[n] >= '0' && p[n] <= '9')
                    {
                        x = x * 10 + (p[n] - '0');
                        n++;
                    }
                    tt.tm_hour = x;
                    p += n;
                    if  (n > 0 && p < e && *p == ':')
                    {
                        p++;
                    }

                    while(p < e && *p == ' ') p++;
                    n = 0; x = 0;
                    while(p < e && p[n] >= '0' && p[n] <= '9')
                    {
                        x = x * 10 + (p[n] - '0');
                        n++;
                    }
                    tt.tm_min = x;
                    p += n;
                    if  (n > 0 && p < e && *p == ':')
                    {
                        p++;
                    }

                    while(p < e && *p == ' ') p++;
                    n = 0; x = 0;
                    while(p < e && p[n] >= '0' && p[n] <= '9')
                    {
                        x = x * 10 + (p[n] - '0');
                        n++;
                    }
                    tt.tm_sec = x;
                    p += n;
                    while(p < e && *p == ' ') p++;
                    if  (p == e || !bStrong)
                    {
                        tt.tm_isdst = -1;
                        t = tt.tm_year >= 0 ? mktime(&tt) : (time_t)-1;
                    }
                }
            }
        }
    }
    return t;
}

time_t ConvertTextDateTime(const CMaaString &DateTime, int_ mode, bool bStrong) noexcept
{
    const char * p = DateTime;
    const int len = DateTime.Length();

    int b1 = 0;
    while(b1 < len && p[b1] == ' ') b1++;
    int e1 = b1;
    while(e1 < len && p[e1] != ' ') e1++;

    int b2 = e1;
    while(b2 < len && p[b2] == ' ') b2++;
    int e2 = b2;
    while(e2 < len && p[e2] != ' ') e2++;

    CMaaString Date = DateTime.RefMid(b1, e1 - b1);
    CMaaString Time = DateTime.RefMid(b2, bStrong ? -1 : e2 - b2);

    return ConvertTextDateTime(Date, Time, mode, bStrong);
}

CMaaString Export(const CMaaString &Data)
{
    return Export(Data, Data.Length());
}

CMaaString CMaaString::CharToOem() const noexcept(noexcept_new)
{
#ifdef _WIN32
    //CMaaString s((const char*)*this);
    //::CharToOemA(s, s.GetBuffer());

    CMaaString s(nullptr, Length(), CMaaString::eNotInitMem);
    ::CharToOemBuffA((const char *)*this, s.GetBuffer(), (warning_dword)s.Length());
    return s;
#else
    return *this;
#endif
}

CMaaString CMaaString::OemToChar() const noexcept(noexcept_new)
{
#ifdef _WIN32
    //CMaaString s((const char *)*this);
    //::OemToCharA(s, s.GetBuffer());
    
    CMaaString s(nullptr, Length(), CMaaString::eNotInitMem);
    ::OemToCharBuffA((const char*)*this, s.GetBuffer(), (warning_dword)s.Length());
    return s;
#else
    return *this;
#endif
}

class CRevertTranslitTable : private CMaaUnivHash<CMaaString, char>
{
    int_ m_InitPattern;
    int_ m_MaxLetterLength;
public:
    CRevertTranslitTable()
    {
        m_MaxLetterLength = 0;
        Add("Ye", 'Е');
        Add("ye", 'е');
        Add("Kh", 'Х');
        Add("kh", 'х');
        for (int_ i = 0; i < 0x100; i++)
        {
            const char c = i;
            if  (c != 'ё' && c != 'Ё' && c != 'ъ' && c != 'Ъ')
            {
                CMaaString ch(&c, 1);
                CMaaString translitted = ch.Translit();
                if  (translitted != ch)
                {
                    m_MaxLetterLength = m_MaxLetterLength < translitted.Length() ? (int_)translitted.Length() : m_MaxLetterLength;
                    Add(translitted, c);
                }
            }
        }
        m_InitPattern = 0x12345678;
    }
    ~CRevertTranslitTable()
    {
        m_InitPattern = -1;
    }
    CMaaString FromTranslit(CMaaString src, int *translated_chars_count = nullptr, int *unchanged_chars_count = nullptr, int_ allowed_untranslated_chars_count = -1) const noexcept
    {
        if  (IsCreated())
        {
            try
            {
                char Buffer[TOOLSLIB_CS_64K];
                int a = 0, b = 0;
                const int n = allowed_untranslated_chars_count >= 0 ? allowed_untranslated_chars_count : src.Length();
                CMaaConcatString res(Buffer, sizeof(Buffer), src.Length() + 1);
                int pos = 0;
                while(pos < src.Length())
                {
                    int_ i;
                    for (i = m_MaxLetterLength; i > 0; i--)
                    {
                        CMaaString src_ = src.RefMid(pos, i);
                        char c;
                        if  (!Find(src_, &c))
                        {
                            res += c;
                            pos += src_.Length();
                            a++;
                            break;
                        }
                    }
                    if  (i <= 0)
                    {
                        res += src[pos++];
                        if  (++b > n)
                        {
                            throw 1;
                        }
                    }
                }
                if  (translated_chars_count)
                {
                    *translated_chars_count = a;
                }
                if  (unchanged_chars_count)
                {
                    *unchanged_chars_count = b;
                }
                return res;
            }
            catch(...)
            {
            }
        }
        return src;
    }
    bool IsCreated() const noexcept
    {
        return m_InitPattern == 0x12345678;
    }
};

CMaaString CMaaString::FromTranslit(int *translated_chars_count, int *unchanged_chars_count, int_ allowed_untranslated_chars_count) const noexcept(noexcept_new)
{
    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    //static bool b1st = true;
    static CRevertTranslitTable Table;
    aLocker_UnLock;

    return Table.IsCreated() ? Table.FromTranslit(*this, translated_chars_count, unchanged_chars_count, allowed_untranslated_chars_count) : *this;
}

CMaaString CMaaString::Translit(int_ cp) const noexcept(noexcept_new)
{
    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    static bool b1st = true;
    static int_ MapOffset[256], MapLengths[256];
    static const unsigned char * pSrc = (const unsigned char *)"абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    static const char * pDst = "a.b.v.g.d.e.e.zh.z.i.y.k.l.m.n.o.p.r.s.t.u.f.h.ts.ch.sh.sch.'.y.'.e.yu.ya.A.B.V.G.D.E.E.Zh.Z.I.Y.K.L.M.N.O.P.R.S.T.U.F.H.Ts.Ch.Sh.Sch.'.Y.'.E.Yu.Ya.";
    static const int pDstLen = int_strlen(pDst);
    static CMaaUnivHash<CMaaString, unsigned char> MapUtf8Ru2Cp1251;
    if  (b1st)
    {
        int_ pos = 0;
        int_ i;
        for (i = 0; i < 256; i++)
        {
            MapOffset[i] = -1;
            MapLengths[i] = 0;
        }
        for (i = 0; pSrc[i]; i++)
        {
            MapOffset[pSrc[i]] = pos;
            while(pDst[pos])
            {
                if  (pDst[pos++] == '.')
                {
                    break;
                }
            }
            MapLengths[pSrc[i]] = pos - 1 - MapOffset[pSrc[i]];
        }
        CMaaString RussianAlphabet2 = "Р°Р±РІРіРґРµС‘Р¶Р·РёР№РєР»РјРЅРѕРїСЂСЃС‚СѓС„С…С†С‡С€С‰СЉС‹СЊСЌСЋСЏРђР‘Р’Р“Р”Р•РЃР–Р—РР™РљР›РњРќРћРџР РЎРўРЈР¤РҐР¦Р§РЁР©РЄР«Р¬Р­Р®РЇ";
                                 // u8"абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
        i = 0;
        while(RussianAlphabet2.IsNotEmpty())
        {
            const int l = RussianAlphabet2.Utf8Pos(1);
            CMaaString w = RussianAlphabet2.RefMid(0, l);
            RussianAlphabet2 = RussianAlphabet2.RefMid(l);
            MapUtf8Ru2Cp1251.Add(w, pSrc[i++]);
        }
        b1st = false;
    }
    aLocker_UnLock;

    char Buffer[TOOLSLIB_CS_64K];
    CMaaConcatString txt(Buffer, sizeof(Buffer));
    //int pos = 0;
    //int j = 0;
    if  (cp == 1251)
    {
        for (int i = 0; i < Length(); i++)
        {
            const unsigned char c = (char)(*this)[i];
            if  (MapOffset[c] < 0 || MapOffset[c] >= pDstLen || MapLengths[c] > 10 || MapLengths[c] < 0 || MapOffset[c] + MapLengths[c] > pDstLen)
            {
                txt += (char)c;
            }
            else
            {
                txt.Add(pDst + MapOffset[c], MapLengths[c]);
            }
        }
    }
    else
    {
        CMaaString tmp = *this;
        while(tmp.IsNotEmpty())
        {
            const int l = tmp.Utf8Pos(1);
            CMaaString w = tmp.RefMid(0, l);
            tmp = tmp.RefMid(l);
            unsigned char c = 0;
            MapUtf8Ru2Cp1251.Find(w, &c);
            if  (MapOffset[c] < 0 || MapOffset[c] >= pDstLen || MapLengths[c] > 10 || MapLengths[c] < 0 || MapOffset[c] + MapLengths[c] > pDstLen)
            {
                txt += w;
            }
            else
            {
                txt.Add(pDst + MapOffset[c], MapLengths[c]);
            }
        }
    }
    return txt;
}

//----------------------------------------------------------------------------
static CMaaString MyOrBase64Encode(const CMaaString &Src, int LineLen, const CMaaString &Lf, const char* pszAlphabet) noexcept(noexcept_new)
{
    char csBuffer[TOOLSLIB_CS_64K];
    char Buffer[1024 + 32];
    int pos = 0, pos0 = 0, len = 0;
    bool bEndedByLf = false;
    int SrcPos = 0;
    const int SrcLen = Src.Length();
    CMaaConcatString Ret(csBuffer, sizeof(csBuffer), SrcLen * 8 / 6 + SrcLen * 8 / 6 / (LineLen >= 10 ? LineLen : 10) * Lf.Length() + 100);
    const unsigned char * pSrc = (const unsigned char *)(const char *)Src;
    int_ Bits = 0;
    uint_ x = 0;
    while(SrcPos < SrcLen)
    {
        x = (x << 8) | pSrc[SrcPos++];
        Bits += 8;

        while(Bits >= 6)
        {
            const uint_ y = (x >> (Bits - 6)) & 0x3f;
            Buffer[pos++] = pszAlphabet[y];
            x ^= (y << (Bits - 6));
            Bits -= 6;
        }
        while(LineLen > 0 && pos + pos0 >= LineLen)
        {
            Ret.Add(Buffer, LineLen - pos0);    Ret += Lf;
            len += LineLen - pos0;
            bEndedByLf = true;
            memmove(Buffer, Buffer + LineLen - pos0, pos -= (LineLen - pos0));
            pos0 = 0;
        }
        if  (pos >= (int)sizeof(Buffer) - 32)
        {
            Ret.Add(Buffer, pos);
            len += pos;
            bEndedByLf = false;
            pos0 += pos;
            pos = 0;
        }
    }
    if  (Bits > 0)
    {
        Buffer[pos++] = pszAlphabet[(x << (6 - Bits)) & 0x3f];
    }
    len += pos;
    while(len % 4 != 0)
    {
        Buffer[pos++] = '=';
        len++;
    }
    while(LineLen > 0 && pos + pos0 >= LineLen)
    {
        Ret.Add(Buffer, LineLen - pos0);        Ret += Lf;
        //len += LineLen - pos0;
        bEndedByLf = true;
        memmove(Buffer, Buffer + LineLen - pos0, pos -= (LineLen - pos0));
        pos0 = 0;
    }
    if  (pos > 0)
    {
        Ret.Add(Buffer, pos);
        //len += pos;
        bEndedByLf = false;
        //pos0 += pos;
        //pos = 0;
    }
    CMaaString R = Ret.GetMemoryString();
    return bEndedByLf ? R.Left(R.Length() - Lf.Length()) : R.NewCopy();
}

static CMaaString MyOrBase64Decode(const CMaaString &Src, bool *pbOk, int *pValidCharNums, const CMaaString &IgnoreChars, const sMyOrBase64DecodeMap &Map) noexcept(noexcept_new)
{
    char csBuffer[TOOLSLIB_CS_64K];
    char Buffer[1024 + 32];
    int pos = 0, len = 0;
    //CMaaString Ret;
    int SrcPos = 0;
    const int SrcLen = Src.Length();
    CMaaConcatString Ret(csBuffer, sizeof(csBuffer), SrcLen * 6 / 8 + 100);
    const unsigned char * pSrc = (const unsigned char *)(const char *)Src;
    int_ Bits = 0;
    uint_ x = 0;
    while(SrcPos < SrcLen)
    {
        const int_ y = Map.m[pSrc[SrcPos] & 0x7f];
        if  (y < 0)
        {
            if  (IgnoreChars.Find((char)pSrc[SrcPos]) >= 0)
            {
                SrcPos++;
                continue;
            }
            break;
        }
        SrcPos++;
        len++;
        x = (x << 6) | y;
        Bits += 6;
        if  (Bits >= 8)
        {
            const uint_ z = ((x >> (Bits - 8)) & 0xff);
            Buffer[pos++] = z;
            x ^= z << (Bits - 8);
            Bits -= 8;
            if  (pos >= (int)sizeof(Buffer) - 32)
            {
                Ret.Add(Buffer, pos);
                pos = 0;
            }
        }
    }
    Ret.Add(Buffer, pos);
    pos = 0;
    bool bOk = false;
    if  (x == 0)
    {
        while(SrcPos < SrcLen && (len % 4) != 0)
        {
            if  (pSrc[SrcPos] == '=' || IgnoreChars.Find((char)pSrc[SrcPos]) >= 0)
            {
                SrcPos++;
                len++;
            }
            else
            {
                break;
            }
        }
        bOk = (SrcPos == SrcLen && (len % 4) == 0);
    }
    if  (pbOk)
    {
        *pbOk = bOk;
    }
    if  (pValidCharNums)
    {
        *pValidCharNums = SrcPos;
    }
    return Ret;
}
//----------------------------------------------------------------------------
static constexpr const char * sBase64Alphabet      = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static constexpr const char * sMyWebEncodeAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_/";
//----------------------------------------------------------------------------
CMaaString CMaaString::Base64Encode(int LineLen, const CMaaString &Lf) const noexcept(noexcept_new)
{
    return MyOrBase64Encode(*this, LineLen, Lf, sBase64Alphabet);
}
CMaaString CMaaString::Base64Decode(bool* pbOk, int* pValidCharNums, const CMaaString &IgnoreChars) const noexcept(noexcept_new)
{
    return MyOrBase64Decode(*this, pbOk, pValidCharNums, IgnoreChars, gfnBase64Map());
}
// web encode generic alphabet based, unused
CMaaString CMaaString::MyEncode(int LineLen, const CMaaString &Lf) const noexcept(noexcept_new)
{
    return MyOrBase64Encode(*this, LineLen, Lf, sMyWebEncodeAlphabet);
}
// web decode generic alphabet based, unused
CMaaString CMaaString::MyDecode(bool* pbOk, int* pValidCharNums, const CMaaString &IgnoreChars) const noexcept(noexcept_new)
{
    return MyOrBase64Decode(*this, pbOk, pValidCharNums, IgnoreChars, gfnMyWeb64Map());
}
//----------------------------------------------------------------------------

CMaaString CMaaString::ToMD5() const noexcept(noexcept_new)
{
    CMD5Cacl c;
    c.Update((const char *)*this, (warning_int)Length());
    char hash[16];
    c.GetHash(hash);
    return CMaaString(hash, 16);
}

CMaaString CMaaString::RemoveCrLfAtEOL() const noexcept(noexcept_new)
{
    const int Len = Length();
    if  (IsRight("\r\n", 2))
    {
        return Left(Len - 2);
    }
    if (Len)
    {
        char c = *(Len - 1 + (const char*)*this);
        if (c == '\r' || c == '\n')
        {
            return Left(Len - 1);
        }
    }
    return *this;
}

CMaaString CMaaString::DisplayHex(bool bUpper) const noexcept(noexcept_new)
{
    const int len = Length();
    CMaaString Result(nullptr, len * 2, CMaaString::eNotInitMem);
    if (Result.Length() == len * 2 && len * 2 >= len)
    {
        const _byte * p = (const _byte *)(const char *)*this;
        char * txt = Result.GetBuffer();
        const char aA = bUpper ? 'A' : 'a';
        for (int i = 0; i < len; i++)
        {
            Byte2Hex_nzt_HL(txt + 2 * i, p[i], aA);
        }
        return Result;
    }
    return CMaaStringZ;
}

CMaaString CMaaString::FromDisplayedHex(bool *pbErr0) const noexcept(noexcept_new)
{
    bool b;
    pbErr0 = pbErr0 ? pbErr0 : &b;
    *pbErr0 = false;
    CMaaString Result;
    const int len = Length();
    if  (len % 2)
    {
        *pbErr0 = true;
    }
    else
    {
        Result = CMaaString(nullptr, len / 2, CMaaString::eNotInitMem);
        *pbErr0 = Result.Length() * 2 != len;
        const char * pSrc = (const char*)*this;
        if  (!*pbErr0)
        {
            char * txt = Result.GetBuffer();
            for (int i = 0; i < len; i += 2)
            {
                char c1 = pSrc[i];
                if  (c1 >= '0' && c1 <= '9')
                {
                    c1 -= '0';
                }
                else if (c1 >= 'a' && c1 <= 'f')
                {
                    c1 = c1 - 'a' + 10;
                }
                else if (c1 >= 'A' && c1 <= 'F')
                {
                    c1 = c1 - 'A' + 10;
                }
                else
                {
                    *pbErr0 = true;
                    break;
                }

                char c0 = pSrc[i + 1];
                if  (c0 >= '0' && c0 <= '9')
                {
                    c0 -= '0';
                }
                else if (c0 >= 'a' && c0 <= 'f')
                {
                    c0 = c0 - 'a' + 10;
                }
                else if (c0 >= 'A' && c0 <= 'F')
                {
                    c0 = c0 - 'A' + 10;
                }
                else
                {
                    *pbErr0 = true;
                    break;
                }

                *txt++ = (c1 << 4) + c0;
            }
        }
    }
    if  (*pbErr0)
    {
        Result.Empty();
    }
    return Result;
}

CMaaString GetDateMailFormat(time_t t) noexcept
{
    // mail string sample "Date: Tue, 31 Mar 2009 04:23:55 +0400\r\n"
    // returns                  "Tue, 31 Mar 2009 04:23:55 +0400"

    CMaaString Ret;
    //static const char * Month[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    //static const char * EngDayOfWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    tm ltt, gtt;
    {
        const tm* p = localtime(&t);
        if (p)
        {
            memcpy(&ltt, p, sizeof(ltt));
        }
        else
        {
            memset(&ltt, 0, sizeof(ltt));
        }
    }
    {
        const tm* p = gmtime(&t);
        if  (p)
        {
            memcpy(&gtt, p, sizeof(gtt));
        }
        else
        {
            memset(&gtt, 0, sizeof(gtt));
        }
    }
#ifdef _WIN32
    const int_ Bias = (int_)(_mkgmtime(&ltt) - _mkgmtime(&gtt)) / 60;
#else
    const int_ Bias = (int_)(timegm(&ltt) - timegm(&gtt)) / 60;
#endif
    const int_ Bias4 = (Bias / 60) * 100 + (Bias % 60);

    // "Tue, 31 Mar 2009 04:23:55 +0400"
    Ret.Format("%s, %d %s %d %02d:%02d:%02d %c%04d",
         g_pszEngDayOfWeek[ltt.tm_wday % 7], ltt.tm_mday, g_pszEngMonth[ltt.tm_mon % 12], ltt.tm_year + 1900,
         ltt.tm_hour, ltt.tm_min, ltt.tm_sec, Bias4 >= 0 ? '+' : '-', Bias4 >= 0 ? Bias4 : -Bias4);

    return Ret;
}


CMaaString GetDateRKNFormat(time_t t) noexcept
{
    // returns "2025-10-01 10:25:04 +0000"

    CMaaString Ret;

    tm ltt, gtt;
    {
        const tm* p = localtime(&t);
        if (p)
        {
            memcpy(&ltt, p, sizeof(ltt));
        }
        else
        {
            memset(&ltt, 0, sizeof(ltt));
        }
    }
    {
        const tm* p = gmtime(&t);
        if (p)
        {
            memcpy(&gtt, p, sizeof(gtt));
        }
        else
        {
            memset(&gtt, 0, sizeof(gtt));
        }
    }
#ifdef _WIN32
    const int_ Bias = (int_)(_mkgmtime(&ltt) - _mkgmtime(&gtt)) / 60;
#else
    const int_ Bias = (int_)(timegm(&ltt) - timegm(&gtt)) / 60;
#endif
    const int_ Bias4 = (Bias / 60) * 100 + (Bias % 60);

    // "Tue, 31 Mar 2009 04:23:55 +0400"
    Ret.Format("%04d-%02d-%02d %02d:%02d:%02d %c%04d",
        ltt.tm_year + 1900, ltt.tm_mon % 12, ltt.tm_mday,
        ltt.tm_hour, ltt.tm_min, ltt.tm_sec, Bias4 >= 0 ? '+' : '-', Bias4 >= 0 ? Bias4 : -Bias4);

    return Ret;
}

CMaaString GetDateTimeYandexdFormat(time_t t)
{
    // sample "2015-08-10T21:44:58.000+03:00"
    // 21:44:58 is local time

    CMaaString Ret;

    tm ltt, gtt;
    memset(&ltt, 0, sizeof(ltt));
    memset(&gtt, 0, sizeof(gtt));
    tm * p = localtime(&t);
    if  (p)
    {
        memcpy(&ltt, p, sizeof(ltt));
    }
    else
    {
        throw "can't get localtime()";
    }
    p = gmtime(&t);
    if  (p)
    {
        memcpy(&gtt, p, sizeof(gtt));
    }
    else
    {
        throw "can't get gmtime()";
    }
#ifdef _WIN32
    const int_ Bias = (int_)(_mkgmtime(&ltt) - _mkgmtime(&gtt)) / 60;
#else
    const int_ Bias = (int_)(timegm(&ltt) - timegm(&gtt)) / 60;
#endif
    //const int_ Bias4 = (Bias / 60) * 100 + (Bias % 60);
    const int_ Bias4p = Bias > 0 ? Bias : -Bias;

    // "2015-08-10T21:44:58.000+03:00"
    Ret.Format("%04d-%02d-%02dT%02d:%02d:%02d.000%c%02d:%02d",
        ltt.tm_year + 1900, ltt.tm_mon + 1, ltt.tm_mday,
        ltt.tm_hour, ltt.tm_min, ltt.tm_sec,
        Bias >= 0 ? '+' : '-', Bias4p / 60, Bias4p % 60);

    return Ret;
}

CMaaString GetDateTimeYandexdFormatForMWS(time_t t)
{
    // sample "2015-08-10T21:44:58.000Z"
    // 21:44:58 is local time

    CMaaString Ret;

    tm ltt;
    memset(&ltt, 0, sizeof(ltt));
    const tm * p = localtime(&t);
    if  (p)
    {
        memcpy(&ltt, p, sizeof(ltt));
    }
    else
    {
        throw "can't get localtime()";
    }

    // "2015-08-10T21:44:58.000Z"
    Ret.Format("%04d-%02d-%02dT%02d:%02d:%02d.000Z",
        ltt.tm_year + 1900, ltt.tm_mon + 1, ltt.tm_mday,
        ltt.tm_hour, ltt.tm_min, ltt.tm_sec);

    return Ret;
}

/*
// PHP
function _getUtcTime()
{
    $datetime = new DateTime('now');
    $datetime->setTimeZone(new DateTimeZone('Europe/Moscow'));
    $utcTime = $datetime->format('c');
    //$this->utcTime = $utcTime;
    return $utcTime;
}
*/
CMaaString GetDateTimeNalogFormat(time_t t)
{
    // sample "2015-08-10T21:44:58+03:00"
    // 21:44:58 is local time
    //        "2024-03-02T06:03:49+03:00"

    CMaaString Ret;

    tm ltt, gtt;
    memset(&ltt, 0, sizeof(ltt));
    memset(&gtt, 0, sizeof(gtt));
    tm * p = localtime(&t);
    if  (p)
    {
        memcpy(&ltt, p, sizeof(ltt));
    }
    else
    {
        throw "can't get localtime()";
    }
    p = gmtime(&t);
    if  (p)
    {
        memcpy(&gtt, p, sizeof(gtt));
    }
    else
    {
        throw "can't get gmtime()";
    }
#ifdef _WIN32
    const int_ Bias = (int_)(_mkgmtime(&ltt) - _mkgmtime(&gtt)) / 60;
#else
    const int_ Bias = (int_)(timegm(&ltt) - timegm(&gtt)) / 60;
#endif
    //const int_ Bias4 = (Bias / 60) * 100 + (Bias % 60);
    const int_ Bias4p = Bias > 0 ? Bias : -Bias;

    // "2015-08-10T21:44:58+03:00"
    Ret.Format("%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
        ltt.tm_year + 1900, ltt.tm_mon + 1, ltt.tm_mday,
        ltt.tm_hour, ltt.tm_min, ltt.tm_sec,
        Bias >= 0 ? '+' : '-', Bias4p / 60, Bias4p % 60);

    return Ret;
}

CMaaString GetW3CDateTime(time_t t)
{
    // sample "2015-08-10T21:44:58+03:00"
    // 21:44:58 is local time

    CMaaString Ret;

    if  (t == (time_t)-1 || !t)
    {
        return Ret;
    }

    tm ltt, gtt;
    memset(&ltt, 0, sizeof(ltt));
    memset(&gtt, 0, sizeof(gtt));
    tm * p = localtime(&t);
    if  (p)
    {
        memcpy(&ltt, p, sizeof(ltt));
    }
    else
    {
        throw "can't get localtime()";
    }
    p = gmtime(&t);
    if  (p)
    {
        memcpy(&gtt, p, sizeof(gtt));
    }
    else
    {
        throw "can't get gmtime()";
    }
#ifdef _WIN32
    const int_ Bias = (int_)(_mkgmtime(&ltt) - _mkgmtime(&gtt)) / 60;
#else
    const int_ Bias = (int_)(timegm(&ltt) - timegm(&gtt)) / 60;
#endif
    //const int_ Bias4 = (Bias / 60) * 100 + (Bias % 60);
    const int_ Bias4p = Bias > 0 ? Bias : -Bias;

    // "2015-08-10T21:44:58.000+03:00"
    Ret.Format("%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
        ltt.tm_year + 1900, ltt.tm_mon + 1, ltt.tm_mday,
        ltt.tm_hour, ltt.tm_min, ltt.tm_sec,
        Bias >= 0 ? '+' : '-', Bias4p / 60, Bias4p % 60);

    return Ret;
}

CMaaString GetFromToMailText(const CMaaString &TextUtf8UserName, const CMaaString &EMail) noexcept(noexcept_new)
{
    return CMaaString("=?utf-8?B?") + TextUtf8UserName.Base64Encode(-1, "") + "?= <" + EMail + ">";
}

CMaaString MakeMailSubject(const CMaaString &Utf8Text) noexcept(noexcept_new)
{
    return CMaaString("=?utf-8?B?") + Utf8Text.Base64Encode(-1, "") + "?=";
}

CMaaString GetMailSubject(const CMaaString &Subject) noexcept(noexcept_new)
{
    if  (Subject.IsLeft("=?utf-8?B?", 10) && Subject.IsRight("?=", 2))
    {
        bool bOk;
        CMaaString _Subject = Subject.Mid(10, Subject.Length() - 12).Base64Decode(&bOk);
        if  (bOk)
        {
            return _Subject;
        }
    }
    return Subject;
}

CMaaString MakeMailText(CMaaString Text, int LineLen /*= 72*/) noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_64K];
    CMaaConcatString c(Buffer, sizeof(Buffer), Text.Length() + 4096);
    CMaaString Line;
    while(Text.IsNotEmpty())
    {
        int nc, p, sp = 1;
        nc = Text.RefMid(0, LineLen + 2).Utf8CharCount();
        for (; (p = Text.Utf8Pos(nc, false)) > LineLen + 2; nc--) ;
        Line = Text.RefMid(0, p);
        if  (Line.IsRight("\r\n", 2))
        {
            p = LineLen;
            sp = 2;
        }
        else
        {
            if  (p <= LineLen + 1)
            {
                Line = std::move(Text.RefMid(0, p));
            }
            else
            {
                Line = std::move(Text.RefMid(0, Text.Utf8Pos(--nc, false)));
            }
            for (p = Line.Length() - 1; p >= 0; p--)
            {
                if  (Line[p] == ' ' || Line[p] == '\t')
                {
                    break;
                }
                if  (Line.IsMid("\r\n", p, 2))
                {
                    sp = 2;
                    break;
                }
            }
            if  (p <= 0)
            {
                for (; (p = Text.Utf8Pos(nc, false)) > LineLen; nc--) ;
                sp = 0;
            }
        }
        c.Add((const char *)Text, p);
        c += '\n';
        Text = std::move(Text.RefMid(p + sp));
    }
    //Text = c;
    //return Text; // std::move(Text); - warning: redundant move in return statement. note: remove вЂstd::moveвЂ™ call
    return c;
}

bool ConvertFromWebDateTime(const CMaaString &s, time_t *t) noexcept
{
    //Sun, 26 Apr 2009 02:27:40 GMT
    int n = s.Find(',');
    if  (n <= 0)
    {
        return false;
    }
    n++;
    while(s[n] == ' ') n++;
    int_ d;
    if  (dsscanf(n + (const char *)s, "%d", &d) != 1)
    {
        return false;
    }
    n = s.Find(n, ' ');
    if  (n <= 0)
    {
        return false;
    }
    while(s[n] == ' ') n++;
    const int n2 = s.Find(n, ' ');
    if  (n2 <= 0)
    {
        return false;
    }
    CMaaString Month = s.RefMid(n, n2 - n);
    //static const char * pszMonth[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    int_ m;
    for (m = 0; m < 12; m++)
    {
        if  (Month == (char*)g_pszEngMonth[m]) //pszMonth[m])
        {
            break;
        }
    }
    if  (m >= 12)
    {
        return false;
    }
    n = n2;
    while(s[n] == ' ') n++;
    int_ y;
    if  (dsscanf(n + (const char *)s, "%d", &y) != 1)
    {
        return false;
    }
    n = s.Find(n, ' ');
    if  (n <= 0)
    {
        return false;
    }
    while(s[n] == ' ') n++;
    int_ hh, mm, ss;
    if  (dsscanf(n + (const char *)s, "%d:%d:%d", &hh, &mm, &ss) != 3)
    {
        if  (dsscanf(n + (const char *)s, "%d", &hh) != 1)
        {
            return false;
        }
        n = s.Find(n, ':');
        if  (n <= 0)
        {
            return false;
        }
        if  (dsscanf(++n + (const char *)s, "%d", &mm) != 1)
        {
            return false;
        }
        n = s.Find(n, ':');
        if  (n <= 0)
        {
            return false;
        }
        if  (dsscanf(++n + (const char *)s, "%d", &ss) != 1)
        {
            return false;
        }
    }
    tm tt;
    memset(&tt, 0, sizeof(tt));
    tt.tm_hour = hh;
    tt.tm_min = mm;
    tt.tm_sec = ss;
    tt.tm_mday = d;
    tt.tm_mon = m;
    tt.tm_year = y - 1900;
    tt.tm_isdst = -1;

    time_t t2;
#ifdef _WIN32
    t2 = _mkgmtime(&tt);
#else
    t2 = timegm(&tt);
#endif
    if  (t2 != (time_t)-1)
    {
        *t = t2;
        return true;
    }
    return false;
}

bool ConvertFromApacheDateTime(const CMaaString &s, time_t *t) noexcept
{
    //08/Aug/2011:21:59:06 +0400
    int n = s.Find(' ');
    if  (n <= 0 || n != s.Length() - 6)
    {
        return false;
    }
    n++;
    int_ d;
    if  (s.sscanfInt(&d) != 1)
    {
        return false;
    }
    n = s.Find('/');
    if  (n <= 0)
    {
        return false;
    }
    CMaaString Month = s.RefMid(n + 1, 3);
    //static const char * pszMonth[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    int_ m;
    for (m = 0; m < 12; m++)
    {
        if  (Month == (char*)g_pszEngMonth[m]) //pszMonth[m])
        {
            break;
        }
    }
    if  (m >= 12)
    {
        return false;
    }
    n += 5;
    int_ y, hh, mm, ss;
    if  (dsscanf(n + (const char *)s, "%d:%d:%d:%d", &y, &hh, &mm, &ss) != 4)
    {
        return false;
    }
    n = s.Find(' ');
    if  (n <= 0)
    {
        return false;
    }
    n++;
    if  (s[n] != '+' && s[n] != '-')
    {
        return false;
    }
    int offh = 0, offm = 0;
    char ch1, ch2;
    ch1 = s[n + 1];
    ch2 = s[n + 2];
    if  (ch1 < '0' || ch1 > '9' || ch2 < '0' || ch2 > '9')
    {
        return false;
    }
    offh = (ch1 - '0') * 10 + (ch2 - '0');
    ch1 = s[n + 3];
    ch2 = s[n + 4];
    if  (ch1 < '0' || ch1 > '9' || ch2 < '0' || ch2 > '9')
    {
        return false;
    }
    offm = (ch1 - '0') * 10 + (ch2 - '0');
    int off = (offh * 60 + offm) * 60;
    off = s[n] == '-' ? off : -off;

    tm tt;
    memset(&tt, 0, sizeof(tt));
    tt.tm_hour = hh;
    tt.tm_min = mm;
    tt.tm_sec = ss;
    tt.tm_mday = d;
    tt.tm_mon = m;
    tt.tm_year = y - 1900;
    tt.tm_isdst = -1;

    time_t t2;
#ifdef _WIN32
    t2 = _mkgmtime(&tt);
#else
    t2 = timegm(&tt);
#endif
    if  (t2 != (time_t)-1)
    {
        *t = t2 + off;
        return true;
    }
    return false;
}

CMaaString GetDateWebFormat(time_t t) noexcept
{
    CMaaString Ret;
    //static const char * Month[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    //static const char * EngDayOfWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    tm gtt;
    //memset(&gtt, 0, sizeof(gtt));
    const tm * p = gmtime(&t);
    if  (p)
    {
        memcpy(&gtt, p, sizeof(gtt));
    }
    else
    {
        return CMaaString::sFormat("gmtime(%D) returns nullptr", (_qword)t);
    }

    // "Tue, 31 Mar 2009 04:23:55 GMT"
    Ret.Format("%s, %02d %s %d %02d:%02d:%02d GMT",
         g_pszEngDayOfWeek[gtt.tm_wday % 7], gtt.tm_mday, g_pszEngMonth[gtt.tm_mon % 12], gtt.tm_year + 1900,
         gtt.tm_hour, gtt.tm_min, gtt.tm_sec);

    /*
     Ret.Format("%S\r\ntst: %s, %02d %s %d %02d:%02d:%02d GMT (time_t=%d)", &Ret,
          g_pszEngDayOfWeek[gtt.tm_wday % 7], gtt.tm_mday, g_pszEngMonth[gtt.tm_mon % 12], gtt.tm_year + 1900,
          gtt.tm_hour, gtt.tm_min, gtt.tm_sec, (int_)t);

    */

    //return Ret;
    return Ret;
}

CMaaString GetDateApacheFormat(time_t t) noexcept
{
    // mail string sample "Date: Tue, 31 Mar 2009 04:23:55 +0400\r\n"
    // returns                  "Tue, 31 Mar 2009 04:23:55 +0400"

    CMaaString Ret;
    //static const char* Month[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
    //static const char* EngDayOfWeek[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

    tm ltt, gtt;
    {
        const tm* p = localtime(&t);
        if (p)
        {
            memcpy(&ltt, p, sizeof(ltt));
        }
        else
        {
            memset(&ltt, 0, sizeof(ltt));
        }
    }
    {
        const tm* p = gmtime(&t);
        if (p)
        {
            memcpy(&gtt, p, sizeof(gtt));
        }
        else
        {
            memset(&gtt, 0, sizeof(gtt));
        }
    }
#ifdef _WIN32
    const int_ Bias = (int_)(_mkgmtime(&ltt) - _mkgmtime(&gtt)) / 60;
#else
    const int_ Bias = (int_)(timegm(&ltt) - timegm(&gtt)) / 60;
#endif
    const int_ Bias4 = (Bias / 60) * 100 + (Bias % 60);

    // "Tue, 31 Mar 2009 04:23:55 +0400"
    // "08/Aug/2011:21:59:06 +0400"
    Ret.Format("%02d/%s/%d:%02d:%02d:%02d %c%04d",
         ltt.tm_mday, g_pszEngMonth[ltt.tm_mon % 12], ltt.tm_year + 1900,
         ltt.tm_hour, ltt.tm_min, ltt.tm_sec, Bias4 >= 0 ? '+' : '-', Bias4 >= 0 ? Bias4 : -Bias4);

    return Ret;
}

CMaaString CMaaString::RemoveSpaces(bool bAtBegin, bool bAtEnd, CMaaString Spaces) const noexcept(noexcept_new)
{
    int b = 0;
    int e = Length();
    const char * pSrc = (const char *)*this;
    if  (bAtBegin)
    {
        while(b < e && Spaces.Find(pSrc[b]) >= 0)
        {
            b++;
        }
    }
    if  (bAtEnd)
    {
        while(e > b && Spaces.Find(pSrc[e - 1]) >= 0)
        {
            e--;
        }
    }
    //return b || e != Length() ? Mid(b, e - b) : *this;
    return Mid(b, e - b); // return *this in Mid() when b == 0 && e == Length()
}


//-----------------------------------------------------------------------------------------------
/*
class ctm
{
     time_t t;
     tm m;
public:
     ctm(time_t x = (time_t)-1);
     time_t Get() const;
     void Set(time_t x);
     bool operator < (const ctm &That) const;
     bool operator > (const ctm &That) const;
     bool operator == (const ctm &That) const;
     bool operator != (const ctm &That) const;
     bool GetStartTime(char c);
     bool GetNextTime(char c); // c is 'y' 'm' 'd' 'h', '.'
};
*/
class ctm
{
    time_t t;
    tm m;
public:
    ctm(time_t x = (time_t)-1) noexcept
    {
        Set(x);
    }
    ctm(tm * p) noexcept
    {
        Set(p);
    }
    time_t Get() const noexcept
    {
        return t;
    }
    void Set(time_t x) noexcept
    {
        t = x;
        const tm * p = localtime(&t);
        if  (p)
        {
            memcpy(&m, p, sizeof(tm));
        }
        else
        {
            memset(&m, 0, sizeof(tm));
        }
    }
    bool operator < (const ctm &That) const noexcept
    {
        int_ x = (m.tm_year - That.m.tm_year) * 31 * 12 + (m.tm_mon - That.m.tm_mon) * 31 + (m.tm_mday - That.m.tm_mday);
        if  (x)
        {
            return x < 0;
        }
        x = (m.tm_hour - That.m.tm_hour) * 3600 + (m.tm_min - That.m.tm_min) * 60 + (m.tm_sec - That.m.tm_sec);
        return x < 0;
    }
    bool operator > (const ctm &That) const noexcept
    {
        int_ x = (m.tm_year - That.m.tm_year) * 31 * 12 + (m.tm_mon - That.m.tm_mon) * 31 + (m.tm_mday - That.m.tm_mday);
        if  (x)
        {
            return x > 0;
        }
        x = (m.tm_hour - That.m.tm_hour) * 3600 + (m.tm_min - That.m.tm_min) * 60 + (m.tm_sec - That.m.tm_sec);
        return x > 0;
    }
    bool operator == (const ctm &That) const noexcept
    {
        int_ x = (m.tm_year - That.m.tm_year) * 31 * 12 + (m.tm_mon - That.m.tm_mon) * 31 + (m.tm_mday - That.m.tm_mday);
        if  (x)
        {
            return false;
        }
        x = (m.tm_hour - That.m.tm_hour) * 3600 + (m.tm_min - That.m.tm_min) * 60 + (m.tm_sec - That.m.tm_sec);
        return x == 0;
    }
    bool operator != (const ctm &That) const noexcept
    {
        return !(*this == That);
    }
    bool GetStartTime(char c) noexcept
    {
        ctm xt(*this);
        switch(c)
        {
        case 'y':
            xt.m.tm_mon = 0;
            CMaa_fallthrough;
        case 'm':
            xt.m.tm_mday = 1;
            CMaa_fallthrough;
        case 'd':
            xt.m.tm_hour = 0;
            CMaa_fallthrough;
        case 'h':
            xt.m.tm_min = 0;
            CMaa_fallthrough;
        case '.':
            xt.m.tm_sec = 0;
            break;
        default:
            return false;
        }
        ctm x1(t - 366 * 24 * 3600), x2(t + 1);
        while(x1.t != x2.t)
        {
            ctm x((x1.t + x2.t)/2);
            if  (x == xt)
            {
                *this = x;
                return true;
            }
            if  (x > xt)
            {
                x2 = x;
            }
            else
            {
                if  (x == x1)
                {
                    // x < xt
                    *this = x2;
                    return true; // approximately found
                    //return false;
                }
                x1 = x;
            }
        }
        *this = x1;
        return *this == xt;
    }

    bool GetNextTime(char c) noexcept // c is 'y' 'm' 'd' 'h', '.'
    {
        if  (!GetStartTime(c))
        {
            return false;
        }
        time_t tt = t;
        switch(c)
        {
        case 'y':
            tt += 367 * 24 * 3600;
            break;
        case 'm':
            tt += (31 * 24 + 2) * 3600;
            break;
        case 'd':
            tt += (24 + 2) * 3600;
            break;
        case 'h':
            tt += 3600;
            return true;
        case '.':
            tt += 60;
            return true;
        default:
            return false;
        }
        ctm x(tt);
        if  (x.GetStartTime(c))
        {
            *this = x;
            return true;
        }
        return false;
    }
    time_t Set(tm * tt1) noexcept
    {
        if  (!tt1)
        {
            return t = (time_t)-1;
        }
        if  (tt1->tm_isdst != -1)
        {
            return t = mktime(tt1);
        }
        memcpy(&m, tt1, sizeof(tm));
        t = mktime(tt1);
        if  (t != (time_t)-1)
        {
            /*tm * p = localtime(&t);
               if (p)
               {
                    tt.tm_isdst = p->tm_isdst;
                    t = mktime(&tt);
               }*/
            ctm x1(t - 24 * 3600), x2(t + 24 * 3600 + 1);
            while(1)
            {
                if  (x1.t == x2.t)
                {
                    *this = x1;
                    break;
                }
                ctm x((x1.t + x2.t)/2);
                if  (x == *this)
                {
                    *this = x;
                    break;
                }
                if  (x > *this)
                {
                    x2 = x;
                }
                else
                {
                    if  (x == x1)
                    {
                        // x < xt
                        *this = x2;
                        break; // approximately found
                    }
                    x1 = x;
                }
            }
        }
        return t;
    }
};
//-----------------------------------------------------------------
#ifdef __unix__
bool CMaaGetStartTime(time_t &t, char c) noexcept // c is 'y' 'm' 'd' 'h', '.'
{
    tm tt;
    tm *p = localtime(&t);
    if  (p)
    {
        memcpy(&tt, p, sizeof(tt));
    }
    else
    {
        memset(&tt, 0, sizeof(tt));
    }
    switch(c)
    {
    case 'y':
        tt.tm_mon = 0;
        CMaa_fallthrough;
    case 'm':
        tt.tm_mday = 1;
        CMaa_fallthrough;
    case 'd':
        tt.tm_hour = 0;
        CMaa_fallthrough;
    case 'h':
        tt.tm_min = 0;
        CMaa_fallthrough;
    case '.':
        tt.tm_sec = 0;
        break;
    default:
        return false;
    }
    //tt.tm_isdst = -1;
    t = mktime(&tt);
    return true;
}
#else
bool CMaaGetStartTime(time_t &t, char c) noexcept // c is 'y' 'm' 'd' 'h', '.'
{
    ctm tt(t);
    if  (tt.GetStartTime(c))
    {
        t = tt.Get();
        return true;
    }
    return false;
}
#endif
bool CMaaGetNextTime(time_t &t, char c) noexcept // c is 'y' 'm' 'd' 'h', '.'
{
    if  (!CMaaGetStartTime(t, c))
    {
        return false;
    }
    tm tt;
    const tm *p = localtime(&t);
    if  (p)
    {
        memcpy(&tt, p, sizeof(tt));
    }
    else
    {
        memset(&tt, 0, sizeof(tt));
    }
    switch(c)
    {
    case 'y':
        t += 367 * 24 * 3600;
        break;
    case 'm':
        t += (31 * 24 + 2) * 3600;
        break;
    case 'd':
        t += (24 + 2) * 3600;
        break;
    case 'h':
        t += 3600;
        return true;
    case '.':
        t += 60;
        return true;
    default:
        return false;
    }
    return CMaaGetStartTime(t, c);
}
//-----------------------------------------------------------------------------------------------
#if TOOLSLIB_CMAASTRING64 <= 1
time_t CMaaMkTime(tm * tt1) noexcept
{
    /*
     if (!tt1)
     {
          return (time_t)-1;
     }
     if (tt1->tm_isdst != -1)
     {
          return mktime(tt1);
     }
     */
    /*
     tm tt;
     memcpy(&tt, tt1, sizeof(tm));
     time_t t = mktime(tt1);
     if (t != (time_t)-1)
     {
          tm * p = localtime(&t);
          if (p)
          {
               tt.tm_isdst = p->tm_isdst;
               t = mktime(&tt);
          }
     }
     return t;
     */
    const ctm t(tt1);
    return t.Get();
}
//-----------------------------------------------------------------------------------------------
time_t CMaaMkTimeOrig(tm * tt1) noexcept
{
    //return (time_t)-1;
    return mktime(tt1);
}
//-----------------------------------------------------------------------------------------------
#if 0
int_ To0F(char c) noexcept
{
    if  (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if  (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}
#endif
#endif
_qword GetHexNum(const char * ptr, int len, int *pStopPos, CMaaString StopChars, bool * pbError) noexcept
{
    _qword Ret = 0;
    int pos = 0;
    bool bError = false;
    while(len - pos != 0 && StopChars.Find(ptr[pos]) < 0)
    {
        const int_ Nibble = To0F(ptr[pos]);
        if  (Nibble < 0)
        {
            bError = true;
            break;
        }
        Ret = (Ret << 4) + Nibble;
        pos++;
    }
    bError = bError || pos == 0;
    if  (pStopPos)
    {
        *pStopPos = pos;
    }
    if  (pbError)
    {
        *pbError = bError;
    }
    return Ret;
}
_qword GetHexNum(const char * ptr, int len, _qword ErrorRetValue, int *pStopPos, CMaaString StopChars, bool * pbError) noexcept
{
    bool bError;
    pbError = pbError ? pbError : &bError;
    const _qword Ret = GetHexNum(ptr, len, pStopPos, StopChars, pbError);
    return *pbError ? ErrorRetValue : Ret;
}
//-----------------------------------------------------------------------------------------------
CMaaString UrlWithMixedUtf8ToUtf8(const CMaaString &Text) noexcept(noexcept_new) // noexept // exceptions are suppressed
{
    try
    {
        char csBuffer[TOOLSLIB_CS_32K];
        const int l = Text.GetLength();
        const int l_2 = l - 2;
        CMaaConcatString Buffer(csBuffer, sizeof(csBuffer), l);
        const char * p = Text;
        //int j = 0;
        for (int i = 0; i < l; i++)
        {
            char c = p[i];
            if  (c == '%' && i < l_2)
            {
                const int_ h1 = To0F(p[i+1]);
                const int_ h2 = To0F(p[i+2]);
                if  (h1 >= 0 && h2 >= 0)
                {
                    c = (h1 << 4) + h2;
                    i += 2;
                }
            }
            else if (c == '+')
            {
                c = ' ';
            }
            Buffer += c;
        }

        return CMaaString(Buffer);
    }
    catch(...)
    {
    }
    return Text;
}
//-----------------------------------------------------------------------------------------------
CMaaString UrlWithMixedUtf8ToAscii(const CMaaString &Text) noexcept(noexcept_new)
{
    return Utf8ToAnsi(std::move(UrlWithMixedUtf8ToUtf8(Text)));
}
//-----------------------------------------------------------------------------------------------

DEF_ALLOCATOR(CMaaString::CMaaStringImp)
DEF_ALLOCATOR(CMaaString)
//DEF_ALLOCATOR(ceCMaaStringImp)

//Lock_Function(CMaaAllocatorBasicLock, CMaaLiteMutex, nullptr, nullptr)

#if 0
int CMaaAllocatorBasicLock(bool bLock)
{
    static CMaaLiteMutex *gptr = nullptr;
    static AutoDeref<CMaaLiteMutex> tmp(&gptr, true, nullptr, nullptr);
    typedef void (* tpfn_)(int_ bCreate, void * param_);
    static tpfn_ pfn_ = nullptr;
    static void * param_ = nullptr;
    if  (bLock)
    {
        if  (!gptr)
        {
#ifdef TOOLSLIB_KEEP_GLOBAL_MUTEXES_IN_MEMORY
            gptr = TL_NEW_ CMaaLiteMutex;
#else
            static CMaaLiteMutex g;
            gptr = &g;
#endif
            if  (gptr && pfn_)
            {
                pfn_(1, param_);
            }
        }
        gptr->Lock(); return 1;
    }
    else
    {
        int_ n = gptr->UnLock();
        if  (n < 0)
        {
            gptr = nullptr; if (pfn_) pfn_(0, param_);
        }
        if  (n == 0 && tmp.IsDestoyed())
        {
            delete gptr;
            gptr = nullptr;  if (pfn_) pfn_(0, param_);
        }
        return gptr ? 1 : -1;
    }
    return 0;
}
#endif

_sdword CMaaString::Utf8CharsSequenceToUnicodeChar(int &pos, bool bMovePos, bool bAllowExtendedWideChars) noexcept
{
    if  (pos < 0 || pos >= Length())
    {
        return 0;
    }
    int i = pos;
    const unsigned char * pp = (unsigned char *)(const char *)*this;
    _sdword UnicodeChar = pp[i];
    if  ((pp[i] & 0xE0) == 0xC0 &&  (pp[i+1] & 0xC0) == 0x80)
    {
        UnicodeChar = (pp[i+1] & 0x3f) | ((pp[i] & 0x1f) << 6);
        i++;
    }
    else if ((pp[i] & 0xf0) == 0xe0 && (pp[i+1] & 0xc0) == 0x80 && (pp[i+2] & 0xc0) == 0x80)
    {
        UnicodeChar = (pp[i+2] & 0x3f) | ((pp[i+1] & 0x3f) << 6) | ((pp[i] & 0x0f) << 12);
        i += 2;
    }
    else if ((pp[i] & 0xf8) == 0xf0 && (pp[i+1] & 0xc0) == 0x80 && (pp[i+2] & 0xc0) == 0x80 && (pp[i+3] & 0xc0) == 0x80)
    {
        UnicodeChar = (pp[i+3] & 0x3f) | ((pp[i+2] & 0x3f) << 6) | ((pp[i+1] & 0x3f) << 12) | ((pp[i] & 0x07) << 18);
        if  (!bAllowExtendedWideChars)
        {
            UnicodeChar = '_';
        }
        i += 3;
    }
    if  (bMovePos)
    {
        pos = i + 1;
    }
    return UnicodeChar;
}

// characters range < ' ' --> '_'
CMaaString CMaaString::Utf8ToPrintable(bool bAllowCrLf) noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_64K];
    const int l = GetLength();
    try
    {
        CMaaConcatString str(Buffer, sizeof(Buffer), l);

        const char * p = *this;
        const unsigned char * pp = (unsigned char *)p;
        int_ cl = 1;
        for (int i = 0; i < l; i += cl)
        {
            cl = 1;
            _sdword UnicodeChar = (unsigned short)(unsigned char)p[i];
            if  ((pp[i] & 0xE0) == 0xC0 &&  (pp[i+1] & 0xC0) == 0x80)
            {
                UnicodeChar = (pp[i+1] & 0x3f) | ((pp[i] & 0x1f) << 6);
                cl++;
            }
            else if ((pp[i] & 0xf0) == 0xe0 && (pp[i+1] & 0xc0) == 0x80 && (pp[i+2] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[i+2] & 0x3f) | ((pp[i+1] & 0x3f) << 6) | ((pp[i] & 0x0f) << 12);
                cl += 2;
            }
            else if ((pp[i] & 0xf8) == 0xf0 && (pp[i+1] & 0xc0) == 0x80 && (pp[i+2] & 0xc0) == 0x80 && (pp[i+3] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[i+3] & 0x3f) | ((pp[i+2] & 0x3f) << 6) | ((pp[i+1] & 0x3f) << 12) | ((pp[i] & 0x07) << 18);
                //UnicodeChar = '_';
                cl += 3;
            }
            if  (UnicodeChar >= ' ' || ((UnicodeChar == '\r' || UnicodeChar == '\n') && bAllowCrLf))
            {
                str.Add(p + i, cl);
            }
            else
            {
                str += '_';
            }
        }
        return str;
    }
    catch(...)
    {
    }
    return CMaaString{};
}
#ifdef CMAASTRING_HAS_ADD_DATA
_WC_ * CMaaString::Utf8ToUnicode(int_ bDefaultNullAccept) noexcept(noexcept_new)
{
    const int l = GetLength();
    int un = 0, rn = 0; // , en = 0;
    try
    {
        CMaaPtr_<_WC_, 1> BufferU(l + 4);
        /*
        if (!BufferU.IsValid())
        {
            throw 1;
        }
        */
        _WC_ * u = BufferU;

        const char * p = *this;
        const unsigned char * pp = (unsigned char *)p;
        int j = 0;
        for (int i = 0; i < l; i++)
        {
            u[j++] = (unsigned short)(unsigned char)p[i];
            _sdword UnicodeChar = -1;
            if  ((pp[i] & 0xE0) == 0xC0 &&  (pp[i+1] & 0xC0) == 0x80)
            {
                UnicodeChar = (pp[i+1] & 0x3f) | ((pp[i] & 0x1f) << 6);
                i++;
            }
            else if ((pp[i] & 0xf0) == 0xe0 && (pp[i+1] & 0xc0) == 0x80 && (pp[i+2] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[i+2] & 0x3f) | ((pp[i+1] & 0x3f) << 6) | ((pp[i] & 0x0f) << 12);
                i += 2;
            }
            else if ((pp[i] & 0xf8) == 0xf0 && (pp[i+1] & 0xc0) == 0x80 && (pp[i+2] & 0xc0) == 0x80 && (pp[i+3] & 0xc0) == 0x80)
            {
                UnicodeChar = (pp[i+3] & 0x3f) | ((pp[i+2] & 0x3f) << 6) | ((pp[i+1] & 0x3f) << 12) | ((pp[i] & 0x07) << 18);
                UnicodeChar = '_';
                i += 3;
            }
            if  (UnicodeChar >= 0)
            {
                u[j - 1] = (_WC_)UnicodeChar;
                un++;
                //u[j - 1] = w;
                /*
                    if (UnicodeChar == 0)
                    {
                         p[j - 1] = ' ';
                    }
                    else if (UnicodeChar < 0x80)
                    {
                         p[j - 1] = (char)UnicodeChar;
                    }
                    else if (UnicodeChar >= 0x410 && UnicodeChar < 0x450)
                    {
                         const char * Rus = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя";
                         p[j - 1] = (UnicodeChar - 0x410) < (int)strlen(Rus) ? Rus[UnicodeChar - 0x410] : '!';
                    }
                    else if (UnicodeChar == 0x451)
                    {
                         p[j - 1] = 'ё';
                    }
                    else if (UnicodeChar == 0x401)
                    {
                         p[j - 1] = 'Ё';
                    }
                    else
                    {
                         p[j - 1] = '?';
                    }
                    */
            }
            else if (u[j-1] >= 0x80)
            {
                rn++;
            }
        }
        if  (j == 0 || (j > 0 && u[j - 1] != 0))
        {
            u[j++] = 0;
        }
        /*
#ifdef _WIN32
          j = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)(WCHAR*)(_WC_*)BufferU, j, (char *)Buffer, (int)(Buffer.Size() - 1), nullptr, nullptr);
#else
          char * pb = Buffer;
          for  (int i = 0; i < j; i++)
          {
               _sdword UnicodeChar = BufferU[i];
               if (UnicodeChar == 0)
               {
                    pb[i] = ' ';
               }
               else if (UnicodeChar < 0x80)
               {
                    pb[i] = (char)UnicodeChar;
               }
               else if (UnicodeChar >= 0x410 && UnicodeChar < 0x450)
               {
                    static const char * Rus = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя";
                    pb[i] = (UnicodeChar - 0x410) < (int)strlen(Rus) ? Rus[UnicodeChar - 0x410] : '!';
               }
               else if (UnicodeChar == 0x451)
               {
                    pb[i] = 'ё';
               }
               else if (UnicodeChar == 0x401)
               {
                    pb[i] = 'Ё';
               }
               else
               {
                    pb[i] = '?';
                    en++;
               }
          }
#endif
*/
        /*
          if   (j < 0)
          {
               j = 0;
          }
          if   (un >= rn)
          {
               return CMaaString(Buffer, j);
          }
          */
        return SetAdditionalData(u, (int_)j, bDefaultNullAccept);
    }
    catch(...)
    {
    }
    const _WC_ wchZero = 0;
    return SetAdditionalData(&wchZero, 1, bDefaultNullAccept);
}
#endif
CMaaString Utf8ToUnicode(const CMaaString &Text, int_ bDefaultNullAccept) noexcept // (noexcept_new)
{
    const int l = Text.Length();
    try
    {
        const int max_sz = l + 1;
        _WC_ BufferU0[TOOLSLIB_CS_4K];
        CMaaPtr_<_WC_, -1> BufferU(max_sz <= TOOLSLIB_CS_4K ? 0 : max_sz);
        if (BufferU.IsValid())
        {
            _WC_* u = max_sz <= TOOLSLIB_CS_4K ? BufferU0 : BufferU;

            const unsigned char* pp = (unsigned char*)(const char*)Text;
            int i = 0, j = 0;
            const int l_4 = l - 4;
            while (i < l_4)
            {
                const unsigned char p_i = pp[i++];
                if ((p_i & 0xE0) == 0xC0 && (pp[i] & 0xC0) == 0x80)
                {
                    u[j++] = (_WC_)((pp[i] & 0x3f) | ((p_i & 0x1f) << 6));
                    i++;
                }
                else if ((p_i & 0xf0) == 0xe0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80)
                {
                    u[j++] = (_WC_)((pp[i + 1] & 0x3f) | ((pp[i] & 0x3f) << 6) | ((p_i & 0x0f) << 12));
                    i += 2;
                }
                else if ((p_i & 0xf8) == 0xf0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80 && (pp[i + 2] & 0xc0) == 0x80)
                {
                    //u[j++] = (_WC_)((pp[i + 2] & 0x3f) | ((pp[i + 1] & 0x3f) << 6) | ((pp[i] & 0x3f) << 12) | ((p_i & 0x07) << 18));
                    u[j++] = (_WC_)'_';
                    i += 3;
                }
                else
                {
                    u[j++] = p_i;
                }
            }
            while (i < l)
            {
                const unsigned char p_i = pp[i++];
                if (i < l && (p_i & 0xE0) == 0xC0 && (pp[i] & 0xC0) == 0x80)
                {
                    u[j++] = (_WC_)((pp[i] & 0x3f) | ((p_i & 0x1f) << 6));
                    i++;
                }
                else if (i + 1 < l && (p_i & 0xf0) == 0xe0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80)
                {
                    u[j++] = (_WC_)((pp[i + 1] & 0x3f) | ((pp[i] & 0x3f) << 6) | ((p_i & 0x0f) << 12));
                    i += 2;
                }
                else if (i + 2 < l && (p_i & 0xf8) == 0xf0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80 && (pp[i + 2] & 0xc0) == 0x80)
                {
                    //u[j++] = (_WC_)((pp[i + 2] & 0x3f) | ((pp[i + 1] & 0x3f) << 6) | ((pp[i] & 0x3f) << 12) | ((p_i & 0x07) << 18));
                    u[j++] = (_WC_)'_';
                    i += 3;
                }
                else
                {
                    u[j++] = p_i;
                }
            }
            if (bDefaultNullAccept && (j == 0 || (j > 0 && u[j - 1] != 0)))
            {
                u[j++] = 0;
            }

#ifndef TOOLSLIB_GCC
            return CMaaString(u, j * (int_)sizeof(*u), CMaaString::eUtf16Flag);
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return CMaaString(u, j * (int_)sizeof(*u), CMaaString::eUtf16Flag);
#pragma GCC diagnostic pop
#endif
        }
    }
    catch(...)
    {
    }
    return CMaaStringWC0;
}

CMaaString Utf8ToUnicode32(const CMaaString &Text, int_ bDefaultNullAccept) noexcept // (noexcept_new)
{
    const int l = Text.GetLength();
    try
    {
        const int max_sz = l + 1;
        char32_t BufferU0[TOOLSLIB_CS_4K];
        CMaaPtr_<char32_t, -1> BufferU(max_sz <= TOOLSLIB_CS_4K ? 0 : max_sz);
        if (BufferU.IsValid())
        {
            char32_t* u = max_sz <= TOOLSLIB_CS_4K ? BufferU0 : BufferU;

            const unsigned char* pp = (unsigned char*)(const char*)Text;
            int i = 0, j = 0;
            const int l_4 = l - 4;
            while (i < l_4)
            {
                const unsigned char p_i = pp[i++];
                if ((p_i & 0xE0) == 0xC0 && (pp[i] & 0xC0) == 0x80)
                {
                    u[j++] = (char32_t)((pp[i] & 0x3f) | ((p_i & 0x1f) << 6));
                    i++;
                }
                else if ((p_i & 0xf0) == 0xe0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80)
                {
                    u[j++] = (char32_t)((pp[i + 1] & 0x3f) | ((pp[i] & 0x3f) << 6) | ((p_i & 0x0f) << 12));
                    i += 2;
                }
                else if ((p_i & 0xf8) == 0xf0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80 && (pp[i + 2] & 0xc0) == 0x80)
                {
                    u[j++] = (char32_t)((pp[i + 2] & 0x3f) | ((pp[i + 1] & 0x3f) << 6) | ((pp[i] & 0x3f) << 12) | ((char32_t)(p_i & 0x07) << 18));
                    //u[j++] = (char32_t)'_';
                    i += 3;
                }
                else
                {
                    u[j++] = p_i;
                }
            }
            while (i < l)
            {
                const unsigned char p_i = pp[i++];
                if (i < l && (p_i & 0xE0) == 0xC0 && (pp[i] & 0xC0) == 0x80)
                {
                    u[j++] = (char32_t)((pp[i] & 0x3f) | ((p_i & 0x1f) << 6));
                    i++;
                }
                else if (i + 1 < l && (p_i & 0xf0) == 0xe0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80)
                {
                    u[j++] = (char32_t)((pp[i + 1] & 0x3f) | ((pp[i] & 0x3f) << 6) | ((p_i & 0x0f) << 12));
                    i += 2;
                }
                else if (i + 2 < l && (p_i & 0xf8) == 0xf0 && (pp[i] & 0xc0) == 0x80 && (pp[i + 1] & 0xc0) == 0x80 && (pp[i + 2] & 0xc0) == 0x80)
                {
                    u[j++] = (char32_t)((pp[i + 2] & 0x3f) | ((pp[i + 1] & 0x3f) << 6) | ((pp[i] & 0x3f) << 12) | ((char32_t)(p_i & 0x07) << 18));
                    //u[j++] = (char32_t)'_';
                    i += 3;
                }
                else
                {
                    u[j++] = p_i;
                }
            }
            if (bDefaultNullAccept && (j == 0 || (j > 0 && u[j - 1] != 0)))
            {
                u[j++] = 0;
            }
#ifndef TOOLSLIB_GCC
            return CMaaString(u, j * (int_)sizeof(*u), CMaaString::eUtf32Flag);
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return CMaaString(u, j * (int_)sizeof(*u), CMaaString::eUtf32Flag);
#pragma GCC diagnostic pop
#endif
        }
    }
    catch(...)
    {
    }
    return CMaaStringWChar0;
}

#if TOOLSLIB_CMAASTRING64 <= 1
_qword maa_wcslen(const _WC_ * pText) noexcept
{
    _qword len = 0;
    if  (pText)
    {
        for (; pText[len]; len++) ;
    }
    return len;
}
_qword maa_wcslen32(const char32_t * pText) noexcept
{
    _qword len = 0;
    if  (pText)
    {
        for (; pText[len]; len++) ;
    }
    return len;
}
#endif

CMaaString UnicodeString(const _WC_ * pText, int Len, int_ AddRemLastCh, bool bMemoryString) noexcept(noexcept_new)
{
    if  (!pText)
    {
        return AddRemLastCh > 0 ? CMaaStringWC0 : CMaaStringZ;
    }
    if  (Len < 0)
    {
        Len = (int)maa_wcslen(pText);
        if  (Len > (int)((0x7fffffff - 8 - sizeof(_WC_)) / sizeof(_WC_)))
        {
            return CMaaStringZ;
        }
        Len = AddRemLastCh > 0 ? Len + 1 : Len;
        return bMemoryString ? CMaaString(pText, Len * (int_)sizeof(_WC_), CMaaString::eMemString_16) : CMaaString(pText, Len * (int_)sizeof(_WC_), CMaaString::eUtf16Flag); // RO?
    }
    if  (Len > (int)((0x7fffffff - 8 - sizeof(_WC_)) / sizeof(_WC_)))
    {
        return CMaaStringZ;
    }
    if  (AddRemLastCh > 0)
    {
        CMaaString s(nullptr, (Len + 1) * sizeof(_WC_), CMaaString::eNotInitMem_16); //bMemoryString = false; Not fill by zeros
        if  (s.Length() == (Len + 1) * (int_)sizeof(_WC_))
        {
            _WC_ * p = (_WC_ *)(const char *)s;
            memcpy(p, pText, Len * sizeof(_WC_));
            p[Len] = 0;
            return s;
        }
        return CMaaStringZ;
    }
    if  (AddRemLastCh < 0 && pText && Len > 0 && pText[Len - 1] == 0)
    {
        Len--;
    }
    return bMemoryString ? CMaaString(pText, Len * (int_)sizeof(_WC_), CMaaString::eMemString_16) : CMaaString(pText, Len * (int_)sizeof(_WC_), CMaaString::eUtf16Flag); // RO?
}

CMaaString Unicode32String(const char32_t * pText, int Len, int_ AddRemLastCh, bool bMemoryString) noexcept(noexcept_new)
{
    if  (!pText)
    {
        return AddRemLastCh > 0 ? CMaaStringWC32_0 : CMaaStringZ;
    }
    if  (Len < 0)
    {
        Len = (int)maa_wcslen32(pText);
        if  (Len > (int)((0x7fffffff - 8 - sizeof(char32_t)) / sizeof(char32_t)))
        {
            return CMaaStringZ;
        }
        Len = AddRemLastCh > 0 ? Len + 1 : Len;
        return bMemoryString ? CMaaString(pText, Len * (int_)sizeof(char32_t), CMaaString::eMemString_32) : CMaaString(pText, Len * (int_)sizeof(char32_t), CMaaString::eUtf32Flag); // RO?
    }
    if  (Len > (int)((0x7fffffff - 8 - sizeof(char32_t)) / sizeof(char32_t)))
    {
        return CMaaStringZ;
    }
    if  (AddRemLastCh > 0)
    {
        CMaaString s(nullptr, (Len + 1) * sizeof(char32_t), CMaaString::eNotInitMem_32); //bMemoryString = false; Not fill by zeros
        if  (s.Length() == (Len + 1) * (int_)sizeof(char32_t))
        {
            char32_t * p = (char32_t *)(const char *)s;
            memcpy(p, pText, Len * sizeof(char32_t));
            p[Len] = 0;
            return s;
        }
        return CMaaStringZ;
    }
    if  (AddRemLastCh < 0 && pText && Len > 0 && pText[Len - 1] == 0)
    {
        Len--;
    }
    return bMemoryString ? CMaaString(pText, Len * (int_)sizeof(char32_t), CMaaString::eMemString_32) : CMaaString(pText, Len * (int_)sizeof(char32_t), CMaaString::eUtf32Flag); // RO?
}

CMaaString UnicodeToUtf8(const CMaaString &Text, int_ AddRemLastCh) noexcept // (noexcept_new)
{
    int l = Text.GetLength();
    if  (l % 2)
    {
        return CMaaStringZ;
    }

    try
    {
        l /= 2;
        const int max_sz = 3 * l + 1;
        char Buffer0[TOOLSLIB_CS_8K];
        CMaaPtr_<char, -1> Buffer(max_sz <= TOOLSLIB_CS_8K ? 0 : max_sz);
        if (Buffer.IsValid() && l < (0x7fffffff - 15) / 3)
        {
            char* p = max_sz <= TOOLSLIB_CS_8K ? Buffer0 : Buffer;
            const unsigned char* w0 = (const unsigned char*)(const char*)Text;

            int j = 0;
            for (int i = 0; i < l; i++)
            {
                const _WC_ u = (_WC_)(((_WC_)w0[2 * i]) | (((_WC_)w0[2 * i + 1]) << 8));

                if (u <= 0x7f)
                {
                    p[j++] = (unsigned char)u;
                }
                else if (u <= 0x7ff)
                {
                    p[j++] = (char)(0xC0 | ((u >> 6) & 0x1f));
                    p[j++] = (char)(0x80 | (u & 0x3f));
                }
                else //if (u <= 0xffff)
                {
                    p[j++] = (char)(0xE0 | ((u >> 12) & 0x0f));
                    p[j++] = (char)(0x80 | ((u >> 6) & 0x3f));
                    p[j++] = (char)(0x80 | (u & 0x3f));
                }
                /*
                else if (u <= 0x1fffff)
                {
                    p[j++] = (char)(0xF0 | ((u >> 18) & 0x07));
                    p[j++] = (char)(0x80 | ((u >> 12) & 0x3f));
                    p[j++] = (char)(0x80 | ((u >> 6 ) & 0x3f));
                    p[j++] = (char)(0x80 | (u & 0x3f));
                }
                */
            }
            if (j > 0 && AddRemLastCh < 0 && !p[j - 1])
            {
                j--;
            }
            if (AddRemLastCh > 0)// && p[j - 1])
            {
                p[j++] = 0;
            }
#ifndef TOOLSLIB_GCC
            return CMaaString(p, j);
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return CMaaString(p, j);
#pragma GCC diagnostic pop
#endif
        }
    }
    catch(...)
    {
    }
    return CMaaStringZ;
}

CMaaString Unicode32ToUtf8(const CMaaString &Text, int_ AddRemLastCh) noexcept // (noexcept_new)
{
    int l = Text.GetLength();
    if  (l % 4)
    {
        return CMaaStringZ;
    }

    try
    {
        l /= 4;
        const int max_sz = 4 * l + 1;
        char Buffer0[TOOLSLIB_CS_8K];
        CMaaPtr_<char, -1> Buffer(max_sz <= TOOLSLIB_CS_8K ? 0 : max_sz);
        if (Buffer.IsValid() && l < (0x7fffffff - 15) / 4)
        {
            char* p = max_sz <= TOOLSLIB_CS_8K ? Buffer0 : Buffer;
            const unsigned char* w0 = (const unsigned char*)(const char*)Text;

            int j = 0;
            for (int i = 0; i < l; i++)
            {
                //_dword u = (_dword)pText[i];
                const _dword u = (_dword)(((_dword)w0[4 * i]) | (((_dword)w0[4 * i + 1]) << 8) | (((_dword)w0[4 * i + 2]) << 16) | (((_dword)w0[4 * i + 3]) << 24));

                if (u <= 0x7fU)
                {
                    p[j++] = (unsigned char)u;
                }
                else if (u <= 0x7ffU)
                {
                    p[j++] = (char)(0xC0 | ((u >> 6) & 0x1f));
                    p[j++] = (char)(0x80 | (u & 0x3f));
                }
                else if (u <= 0xffffU)
                {
                    p[j++] = (char)(0xE0 | ((u >> 12) & 0x0f));
                    p[j++] = (char)(0x80 | ((u >> 6) & 0x3f));
                    p[j++] = (char)(0x80 | (u & 0x3f));
                }
                else if (u <= 0x1fffffU)
                {
                    p[j++] = (char)(0xF0 | ((u >> 18) & 0x07));
                    p[j++] = (char)(0x80 | ((u >> 12) & 0x3f));
                    p[j++] = (char)(0x80 | ((u >> 6) & 0x3f));
                    p[j++] = (char)(0x80 | (u & 0x3f));
                }
            }
            if (j > 0 && AddRemLastCh < 0 && !p[j - 1])
            {
                j--;
            }
            if (AddRemLastCh > 0)// && p[j - 1])
            {
                p[j++] = 0;
            }
#ifndef TOOLSLIB_GCC
            return CMaaString(p, j);
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
            return CMaaString(p, j);
#pragma GCC diagnostic pop
#endif
        }
    }
    catch(...)
    {
    }
    return CMaaStringZ;
}

CMaaString UnicodeToUtf8(const _WC_ * pText, int Len, int_ AddRemLastCh) noexcept(noexcept_new)
{
    //return UnicodeToUtf8(UnicodeString(pText, Len, 0, true), AddRemLastCh < 0 && Len >= 0 ? 0 : AddRemLastCh);
    return UnicodeToUtf8(std::move(UnicodeString(pText, Len, 0, true)), AddRemLastCh);
}

CMaaString Unicode32ToUtf8(const char32_t * pText, int Len, int_ AddRemLastCh) noexcept(noexcept_new)
{
    //return Unicode32ToUtf8(UnicodeString(pText, Len, 0, true), AddRemLastCh < 0 && Len >= 0 ? 0 : AddRemLastCh);
    return Unicode32ToUtf8(std::move(Unicode32String(pText, Len, 0, true)), AddRemLastCh);
}

CMaaString UnicodeToUnicodeBigEndian(const CMaaString &Text) noexcept(noexcept_new)
{
    int l = Text.GetLength();
    if  (l % 2)
    {
        return CMaaStringZ;
    }
    CMaaString Ret = std::move(Text.NewCopy());
    char * p = Ret.GetBuffer();
    l = Ret.Length() / 2;
    for (int i = 0; i < l; i++)
    {
        const char ch = p[2 * i];
        p[2 * i] = p[2 * i + 1];
        p[2 * i + 1] = ch;
    }
    return Ret;
}

CMaaString UnicodeBigEndianToUnicode(const CMaaString &Text) noexcept(noexcept_new)
{
    return UnicodeToUnicodeBigEndian(Text);
}

CMaaString wchar_t_ToUtf8(const wchar_t * pText, int Len, int_ AddRemLastCh) noexcept(noexcept_new)
{
    if constexpr (sizeof(wchar_t) == 4)
    {
        return Unicode32ToUtf8((const char32_t *)pText, Len, AddRemLastCh);
    }
    else if constexpr (sizeof(wchar_t) == 2)
    {
        return UnicodeToUtf8((const _WC_ *)pText, Len, AddRemLastCh);
    }
    return CMaaStringZ; // error
}

CMaaString UnicodeToAnsi(const CMaaString &Text, bool RemoveLastNullChar, _dword cp) noexcept(noexcept_new)
{
    CMaaString Result;

    try
    {
        int l = (warning_int)Text.Length();
        if  (l % 2)
        {
            throw 1;
        }
        l /= 2;

        const _WC_ * w = (const _WC_ *)(const char *)Text;

        CMaaPtr_<char, 1> Buffer (4 * l + 40);
        //if  (!Buffer.IsValid())
        //{
        //    throw 1;
        //}
        if  (RemoveLastNullChar && l > 0 && w[l - 1] == 0)
        {
            l--;
        }

#ifdef _WIN32
        if  (cp != 1251)
        {
            l = WideCharToMultiByte(cp, 0, (LPCWSTR)(WCHAR*)w, (warning_dword)l, Buffer, (warning_dword)Buffer.MaxIndex(), nullptr, nullptr);
        }
        else
#endif
        {
            char * pBuffer = Buffer;
            //const char * Rus = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя";
            //int RusLen = (int)strlen(Rus);
            CMaaString RussianAlphabet = CMaaTLGlobalString(CMaaTLGlobalStrings::e_RussianAlphabet2); // "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя"
            const char * Rus = RussianAlphabet;
            const int_ RusLen = (int_)RussianAlphabet.Length();
            for (int i = 0; i < l; i++)
            {
                const _WC_ UnicodeChar = w[i];
                if  (UnicodeChar < 0x80)
                {
                    pBuffer[i] = (char)UnicodeChar;
                }
                else if (UnicodeChar >= 0x410 && UnicodeChar < 0x450)
                {
                    pBuffer[i] = (UnicodeChar - 0x410) < RusLen ? Rus[UnicodeChar - 0x410] : '!';
                }
                else if (UnicodeChar == 0x451)
                {
                    pBuffer[i] = 'ё';
                }
                else if (UnicodeChar == 0x401)
                {
                    pBuffer[i] = 'Ё';
                }
                else if (UnicodeChar == 0x2116) // 'в„–'
                {
                    pBuffer[i] = (char)0xb9;
                }
                else
                {
                    pBuffer[i] = '?';
                }
            }
        }
        Result = CMaaString(Buffer, l);
    }
    catch(...)
    {
    }
    return Result;
}

CMaaString AnsiToUnicode(const CMaaString &Text, bool AddTerminatingNullChar, _dword cp) noexcept(noexcept_new)
{
    if  (cp == (_dword)-2)
    {
        if  (Text.Utf8SpecificCharsCount() > 0 && Text.Utf8PartOfCharsLength() > Text.RussianAscii1251CharsCount())
        {
            return Utf8ToUnicode(Text);
        }
        if  (Text.RussianAscii1251CharsCount() > 0)
        {
            cp = 1251;
        }
        else
        {
            cp = 0;
        }
    }
    CMaaString Result;
    try
    {
        int l = Text.Length() + (AddTerminatingNullChar ? 1 : 0);
        CMaaPtr_<_WC_, 1> BufferU (l + 4);
        //if  (!BufferU.IsValid())
        //{
        //    throw 1;
        //}
        _WC_ * w = BufferU;

#ifdef _WIN32
        if  (cp != 1251)
        {
            l = MultiByteToWideChar(cp, 0, (char *)(const char *)Text/*.GetBuffer()*/, (warning_dword)l, (LPWSTR)(WCHAR*)w, (warning_dword)(l + 4));
        }
        else
#endif
        {
            const sRussian1251ToWCAlphabetMap& Map = gfnRussian1251ToWCAlphabetMap();
            const unsigned char * p = (const unsigned char *)(const char *)Text;
            for (int i = 0; i < l; i++)
            {
                const unsigned char c = p[i];
                _WC_ UnicodeChar;
                if  (c <= 0x7f)
                {
                    UnicodeChar = (_WC_)c;
                }
                else
                {
                    UnicodeChar = Map.m[c] != 0 ? Map.m[c] : (_WC_)'_';
                }
                w[i] = UnicodeChar;
            }
        }
        Result = CMaaString(w, l * (int_)sizeof(_WC_), CMaaString::eUtf16Flag);
    }
    catch(...)
    {
    }
    return Result;
}

//-----------------------------------------------------------------------------------------------
struct sConvertToWeb
{
    char c;
    const char * r;
};
//-----------------------------------------------------------------------------------------------
// for param name and value, result is utf8 string (usually)
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::ConvertHttpParam() const noexcept(noexcept_new)
{
    static constexpr sConvertToWeb sWebConvertionsSafe[] =
    {
        {'"', "&quot;"},
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'«', "&laquo;"},
        {'»', "&raquo;"},
        //{' ', "%20"},
        //{'%', "%25"},
        //{'+', "%2B"},
        {'\0', nullptr}
    };

    if  (Length() > 0)
    {
        char Buffer[TOOLSLIB_CS_32K];
        CMaaConcatString r(Buffer, sizeof(Buffer), Length());
        const char *p0, *p;
        p0 = p = *this;
        const char * e = p + Length();
        while(p < e)
        {
            if  (*p == '&')
            {
                int_ i;
                for (i = 0; sWebConvertionsSafe[i].r; i++)
                {
                    const int len = (int)strlen(sWebConvertionsSafe[i].r);
                    if  ((e - p) >= (std::ptrdiff_t)len && CMaaIsMemEq(p, sWebConvertionsSafe[i].r, len)) // std::ptrdiff_t
                    {
                        r.Add(p0, (int)(p - p0));
                        r += sWebConvertionsSafe[i].c;
                        p0 = ++p;
                        break;
                    }
                }
                if  (sWebConvertionsSafe[i].r)
                {
                    continue;
                }
            }
            else if (*p == '%' && (e - p) >= 3)
            {
                const int_ a = To0F(p[1]);
                const int_ b = To0F(p[2]);
                if  (a >= 0 && b >= 0)
                {
                    const char c = (a << 4) | b;
                    r.Add(p0, (int)(p - p0));
                    r += c;
                    p0 = (p += 3);
                    continue;
                }
            }
            else if (*p == '+')
            {
                r.Add(p0, (int)(p - p0));
                r += ' ';
                p0 = ++p;
                continue;
            }
            p++;
        }
        r.Add(p0, (int)(e - p0));
        return r;
    }
    return CMaaString{};
}
//-----------------------------------------------------------------------------------------------
// CMaaString::sFormat("<a href="%S">%S</a>", &ToHttpHtmlARef(Url), &ToHttpHtmlDisplayedText(Text));
//-----------------------------------------------------------------------------------------------
static int_ my_sprintf_html_dec_char_code(char* b, unsigned char c)
{
    //sprintf(b, "&#%02d;", c);
    *b++ = '&';
    *b++ = '#';
    if (c >= 100)
    {
        *b++ = (c / 100) + '0';
        *b++ = (c / 10) % 10 + '0';
        *b++ = c % 10 + '0';
        *b++ = ';';
        *b = 0;
        return 6;
    }
    *b++ = (c / 10) + '0';
    *b++ = c % 10 + '0';
    *b++ = ';';
    *b = 0;
    return 5;
}

CMaaString CMaaString::ToHttpHtmlDisplayedText(bool bWeak, const CMaaString &strEsc) const noexcept(noexcept_new)
{
    static constexpr sConvertToWeb sWebConvertionsSafe[] =
    {
        {'"', "&quot;"},
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'«', "&laquo;"},
        {'»', "&raquo;"},
        {' ', "&nbsp;"},
        //        {'%', "%25"},
        {'\0', nullptr}
    };
    static constexpr sConvertToWeb sWebConvertionsSafe2[] =
    {
        {'"', "&quot;"},
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'«', "&laquo;"},
        {'»', "&raquo;"},
        //{' ', "&nbsp;"},
        //        {'%', "%25"},
        {'\0', nullptr}
    };
    const sConvertToWeb * ptr = bWeak ? sWebConvertionsSafe2 : sWebConvertionsSafe;
    const int_ EL = (int_)strEsc.Length();
    CMaaPtrAE_<char, 1> Rep((EL + 10) * 10 + 20);
    int_ Map[256];
    memset(Map, 0, sizeof(Map));
    int_ i, pos = 1;
    for (i = 0; ptr[i].r; i++)
    {
        unsigned char c = (unsigned char)ptr[i].c;
        if  (!Map[c])
        {
            Map[c] = pos;
            strcpy(pos + (char *)Rep, ptr[i].r);
            pos += 1 + (int_)strlen(ptr[i].r);
        }
    }
    for (i = 0; i < EL; i++)
    {
        unsigned char c = (unsigned char)(char)strEsc[i];
        if  (!Map[c])
        {
            Map[c] = pos;
            pos += 1 + my_sprintf_html_dec_char_code(pos + (char*)Rep, c);
        }
    }
    char Buffer[TOOLSLIB_CS_64K];
    CMaaConcatString r(Buffer, sizeof(Buffer), Length() * 3 + 64);
    const char *s0, *s;
    s = s0 = *this;
    const char * e = s + Length();
    while(s < e)
    {
        const unsigned char * pp = (unsigned char *)s;
        if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
        {
            s += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
        {
            s += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
        {
            s += 4;
        }
        else
        {
            const int_ c = (unsigned char)*s;
            if  (Map[c])
            {
                r.Add(s0, (int)(s - s0));
                r += (Map[c] + (char *)Rep);
                s0 = ++s;
            }
            else
            {
                s++;
            }
        }
    }
    r.Add(s0, (int)(e - s0));
    return r;
}
//-----------------------------------------------------------------------------------------------
/*CMaaString CMaaString::ToHttpHtmlDisplayedText(bool bWeak, CMaaString strEsc)
{
    CMaaString x = ToHttpHtmlDisplayedText(bWeak);
    for (int_ i = 0; i < strEsc.Length(); i++)
    {
        char w = strEsc[i];
        char b[10];
        sprintf(b, "&#%02d;", (int_)(unsigned char)w);
        x.ReplaceNN(CMaaString(&w, 1, 0), b);
    }
    return x;
}*/
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::Str2HtmlVal(const CMaaString &strEsc) const noexcept(noexcept_new)
{
    char RepBuffer[256];
    const int_ EL = (int_)strEsc.Length();
    CMaaPtr_<char, 1> More;
    char * Rep;
    if (EL * 8 + 8 <= (int_)sizeof(RepBuffer))
    {
        Rep = RepBuffer;
    }
    else
    {
        More.CheckResize(EL * 8 + 8);
        Rep = More;
    }
    _word Map[256];
    memset(Map, 0, sizeof(Map));

    Map[(int_)'&'] = 1;
    memcpy(Rep + 1, "&amp;", 6);
    _word pos = 7;

    for (int_ i = 0; i < EL; i++)
    {
        unsigned char c = (unsigned char)(char)strEsc[i];
        if  (!Map[c])
        {
            Map[c] = pos;
            pos += 1 + my_sprintf_html_dec_char_code(Rep + pos, c);
        }
    }
    char Buffer[TOOLSLIB_CS_32K];
    CMaaConcatString r(Buffer, sizeof(Buffer), Length() * 3 + 64);
    const char *s0, *s;
    s = s0 = *this;
    const char * e = s + Length();
    while(s < e)
    {
        const unsigned char * pp = (unsigned char *)s;
        if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
        {
            s += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
        {
            s += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
        {
            s += 4;
        }
        else
        {
            const int_ c = (unsigned char)*s;
            if  (Map[c])
            {
                r.Add(s0, (int)(s - s0));
                r += (Rep + Map[c]);
                s0 = ++s;
            }
            else
            {
                s++;
            }
        }
    }
    if (!r.GetLength())
    {
        return *this;
    }
    r.Add(s0, (int)(e - s0));
    return r;
}
//-----------------------------------------------------------------------------------------------
// CMaaString::sFormat("<a href="%S">%S</a>", &ToHttpHtmlARef(Url), &ToHttpHtmlDisplayedText(Text));
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::ToHttpHtmlAreaText() const noexcept(noexcept_new)
{
    static constexpr sConvertToWeb sWebConvertionsSafe[] =
    {
        //        {'"', "&quot;"},
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        //        {'«', "&laquo;"},
        //        {'»', "&raquo;"},
        //        {' ', "&nbsp;"},
        //        {'%', "%25"},
        {'\0', nullptr}
    };
    /*static constexpr sConvertToWeb sWebConvertionsSafe2[] =
    {
        {'"', "&quot;"},
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'«', "&laquo;"},
        {'»', "&raquo;"},
        //{' ', "&nbsp;"},
        //        {'%', "%25"},
        {'\0', nullptr}
    };*/
    char Buffer[TOOLSLIB_CS_64K];
    const sConvertToWeb * ptr = /*bWeak ? sWebConvertionsSafe2 :*/ sWebConvertionsSafe;
    CMaaConcatString r(Buffer, sizeof(Buffer), Length() * 3 + 64);
    const char *s0, *s;
    s = s0 = *this;
    const char * e = s + Length();
    while(s < e)
    {
        const unsigned char * pp = (const unsigned char *)s;
        if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
        {
            s += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
        {
            s += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
        {
            s += 4;
        }
        else
        {
            const char c = *s;
            int_ i;
            for (i = 0; ptr[i].r; i++)
            {
                if  (ptr[i].c == c)
                {
                    r.Add(s0, (int)(s - s0));
                    r += ptr[i].r;
                    s0 = ++s;
                    break;
                }
            }
            if  (ptr[i].r)
            {
                //continue;
            }
            else
            {
                //r += c;
                s++;
            }
        }
    }
    r.Add(s0, (int)(e - s0));
    CMaaString Ret = r;
    //Ret.ReplaceNN("--", CMaaString::sFormat("-%%%02X", (int)(unsigned char)'-'));
    return Ret;
}
//-----------------------------------------------------------------------------------------------
// CMaaString::sFormat("<a href="%S">%S</a>", &ToHttpHtmlARef(Url), &ToHttpHtmlDisplayedText(Text));
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::ToHttpHtmlARef() const noexcept(noexcept_new)
{
    static constexpr sConvertToWeb sWebConvertionsSafe[] =
    {
        {'"', "&quot;"},
        {'&', "&amp;"},
        {'<', "&lt;"},
        {'>', "&gt;"},
        {'«', "&laquo;"},
        {'»', "&raquo;"},
        {' ', "%20"},
        {'%', "%25"},
        {'+', "%2B"},
        {'\0', nullptr}
    };
    char Buffer[TOOLSLIB_CS_32K];
    CMaaConcatString r(Buffer, sizeof(Buffer), Length() * 3 + 64);
    const char *s0, *s;
    s = s0 = *this;
    const char * e = s + Length();
    while(s < e)
    {
        const unsigned char * pp = (const unsigned char *)s;
        char hex[16];
        const char c = *s;
        int_ i;
        for (i = 0; sWebConvertionsSafe[i].r; i++)
        {
            if  (sWebConvertionsSafe[i].c == c)
            {
                r.Add(s0, (int)(s - s0));
                r += sWebConvertionsSafe[i].r;
                s0 = ++s;
                break;
            }
        }
        if  (sWebConvertionsSafe[i].r)
        {
            continue;
        }
        if  ((*pp & 0xE0) == 0xC0 && s + 1 < e && (pp[1] & 0xC0) == 0x80)
        {
            r.Add(s0, (int)(s - s0));
            //sprintf(hex, "%%%02X%%%02X", pp[0], pp[1]);
            //r += hex;
            hex[0] = '%';
            Byte2Hex_nzt_H(hex + 1, pp[0]);
            hex[3] = '%';
            Byte2Hex_nzt_H(hex + 4, pp[1]);
            r.Add(hex, 6);
            s0 = (s += 2);
        }
        else if ((*pp & 0xf0) == 0xe0 && s + 2 < e && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
        {
            r.Add(s0, (int)(s - s0));
            //sprintf(hex, "%%%02X%%%02X%%%02X", pp[0], pp[1], pp[2]);
            //r += hex;
            hex[0] = '%';
            Byte2Hex_nzt_H(hex + 1, pp[0]);
            hex[3] = '%';
            Byte2Hex_nzt_H(hex + 4, pp[1]);
            hex[6] = '%';
            Byte2Hex_nzt_H(hex + 7, pp[2]);
            r.Add(hex, 9);
            s0 = (s += 3);
        }
        else if ((*pp & 0xf8) == 0xf0 && s + 3 < e && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
        {
            r.Add(s0, (int)(s - s0));
            //sprintf(hex, "%%%02X%%%02X%%%02X%%%02X", pp[0], pp[1], pp[2], pp[3]);
            //r += hex;
            hex[0] = '%';
            Byte2Hex_nzt_H(hex + 1, pp[0]);
            hex[3] = '%';
            Byte2Hex_nzt_H(hex + 4, pp[1]);
            hex[6] = '%';
            Byte2Hex_nzt_H(hex + 7, pp[2]);
            hex[9] = '%';
            Byte2Hex_nzt_H(hex + 10, pp[3]);
            r.Add(hex, 12);
            s0 = (s += 4);
        }
        else if ((c < '-' || (c > '9' && c < 'A') || c > 'z') && c != '(' && c != ')' && c != ':')
        {
            r.Add(s0, (int)(s - s0));
            //sprintf(hex, "%%%02X", (int)(unsigned char)c);
            //r += hex;
            hex[0] = '%';
            Byte2Hex_nzt_H(hex + 1, (unsigned char)c);
            r.Add(hex, 3);
            s0 = ++s;
        }
        else
        {
            s++;
        }
    }
    r.Add(s0, (int)(e - s0));
    CMaaString Ret = r;
    Ret.ReplaceNN("--", CMaaString::sFormat("-%%%02X", (int_)(unsigned char)'-'));
    return Ret;
}
//-----------------------------------------------------------------------------------------------
// ToHttpHtmlCgiParam(), encodeURIComponent()
// do not encode a-z, a-Z and "-_.!~*'()"
CMaaString CMaaString::ToHttpHtmlCgiParam() const noexcept(noexcept_new)
{
    return encodeURIComponent();
}
//MAA_STATIC_FLAG sFlag_encodeURIComponent;
//static CMaaString encodeURIComponent_GoodCharacters = CMaaTLGlobalString(CMaaTLGlobalStrings::e_encodeURIComponent_GoodCharacters); // gStaticStringsAllocator.Alloc("-_.!~*'()");
CMaaString CMaaString::encodeURIComponent(bool bUppercaseHex) const noexcept(noexcept_new)
{
    //CMaaSafeMakeStatic msms(&sFlag_encodeURIComponent);
    //msms.Make();

    //CMaaWin32Locker Locker(GLOCK_LIB2, "");
    //CMaaWin32Locker_a2Locker_gLock_Atomic2_;
    //a2Locker_Lock_;
    //static CMaaString GoodCharacters = gStaticStringsAllocator.Alloc("-_.!~*'()");
    //a2Locker_UnLock_;
    //msms.Done();

#if 0
    CMaaString encodeURIComponent_GoodCharacters = CMaaTLGlobalString(CMaaTLGlobalStrings::e_encodeURIComponent_GoodCharacters); // gStaticStringsAllocator.Alloc("-_.!~*'()");
#endif

    char Buffer[TOOLSLIB_CS_64K];
    CMaaConcatString r(Buffer, sizeof(Buffer), Length() * 3 + 64);
    const char *s0, *s;
    s = s0 = *this;
    const char * e = s + Length();
    char prev_char = 0;
    //const char * hex_fmt = bUppercaseHex ? "%%%02X" : "%%%02x";
    char hex[3];
    hex[0] = '%';
    const char aA = bUppercaseHex ? 'A' : 'a';
    while(s < e)
    {
        //char hex[8];
        char c = *s;
#if 0
        if  ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || encodeURIComponent_GoodCharacters.Find(c) >= 0)
        {
            s++;
        }
#else
        //if ((c >= '0' || c == '-' || c == '.') && (c >= 'A' || c <= '9') && (c <= 'Z' || c >= 'a' || c == '_') || (c <= 'z'))
        if  ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '-' && prev_char != '-') || c == '.' || c == '_')
        {
            s++;
        }
#endif
        else
        {
            r.Add(s0, (int)(s - s0));
            if  (c == ' ')
            {
                r += '+';
            }
            else
            {
                //sprintf(hex, hex_fmt, (unsigned char)c);
                //r += hex;
                Byte2Hex_nzt_HL(hex + 1, (_byte)c, aA);
                r.Add(hex, 3);
                c = 0;
            }
            s0 = (s += 1);
        }
        prev_char = c;
    }
    r.Add(s0, (int)(e - s0));
    return r;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::encodeURIComponents(bool bUppercaseHex) const noexcept(noexcept_new)
{
    CMaaString Ret;
    int pos = 0;
    while(pos < Length())
    {
        const int pos2 = Find(pos, '/');
        if (pos2 < 0)
        {
            Ret += RefMid(pos).encodeURIComponent(bUppercaseHex);
            break;
        }
        Ret += RefMid(pos, pos2 - pos).encodeURIComponent(bUppercaseHex) + "/";
        pos = pos2 + 1;
    }
    return Ret;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::decodeURIComponent() const noexcept(noexcept_new)
{
    return UrlWithMixedUtf8ToUtf8(*this);
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::ToJsArgValue(char quote) const noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_16K];
    CMaaString src_special_chars = "\'\"&\\\n\r\t\b\f";
    CMaaString dst_replacements = "\\'\\\"\\&\\\\\\n\\r\\t\\b\\f";

    CMaaConcatString str(Buffer, sizeof(Buffer));
    int N = 0;
    const char *pSrc = *this;
    const int Len = Length();
    char xx[4];
    xx[0] = '\\';
    xx[1] = 'x';
    for (int i = 0; i < Len; i++)
    {
        const char c = pSrc[i];
        const int n = src_special_chars.Find(c);
        if  (n < 0)
        {
            str += c;
        }
        else
        {
            if  (quote == 'x')
            {
                //char txt[32];
                //sprintf(txt, "\\x%02X", (int_)(unsigned char)c);
                //str += txt;
                Byte2Hex_nzt_H(xx + 2, (_byte)c);
                str.Add(xx, 4);
            }
            else if ((c == '"' || c == '\'') && quote)
            {
                if  (c == '"' && quote == '\'')
                {
                    str += "\"";
                }
                else if (c == '"' && quote == '\"')
                {
                    str += "\" + '\"' + \"";
                }
                else if (c == '\'' && quote == '\"')
                {
                    str += "'";
                }
                else if (c == '\'' && quote == '\'')
                {
                    str += "' + \"'\" + '";
                }
            }
            else
            {
                str += dst_replacements.RefMid(2 * n, 2);
            }
            N++;
        }
    }
    if  (quote && quote != '\'' && quote != '"' && quote != 'x')
    {
        quote = '\'';
    }
    CMaaString Text = str;
    if  (quote && quote != 'x')
    {
        Text.Format("%c%S%c", quote, &Text, quote);
    }
    return Text;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::GetCookie(const char * pszCookieNameEq, bool bOptMid) const noexcept(noexcept_new)
{
    CMaaString cookie;
    //const char * p = getenv("HTTP_COOKIE");
    //* this = p ? p : "";
    int n = Find(pszCookieNameEq); // Find(COOKIE3 "=");
    if  (n >= 0)
    {
        n += (int)strlen(pszCookieNameEq);
        const int e = Find(n, ';');
        cookie = bOptMid ? std::move(RefMid(n, e >= n ? e - n : -1)) : std::move(Mid(n, e >= n ? e - n : -1));
    }
    return cookie;
}
//-----------------------------------------------------------------------------------------------
int_ CMaaString::dsscanf(const char* fmt, ...) const noexcept
{
    va_list list;
    va_start(list, fmt);
    int_ n = ::dsscanfV(this, (const char*)*this, (warning_int)Length(), fmt, list);
    va_end(list);
    return n;
}
//-----------------------------------------------------------------------------------------------
int_ CMaaString::dsscanfV(const char* fmt, va_list list) const noexcept
{
    return ::dsscanfV(this, (const char*)*this, (warning_int)Length(), fmt, list);
}
//-----------------------------------------------------------------------------------------------
int_ CMaaString::sscanfInt(int_ *pRet) const noexcept // sscanf(ptr,"%d", &x) - like
{
    const char * p = *this;
    const char * e = p + Length();
    while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    if (p < e)
    {
        const char c = *p;
        if (c == '+' || c == '-')
        {
            p++;
        }
        if (p < e && *p >= '0' && *p <= '9')
        {
            int_ x = -(*p++ - '0');
            while (p < e && *p >= '0' && *p <= '9')
            {
                x = x * 10 - (*p++ - '0');
            }
            //while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
            //if (p == e)
            {
                *pRet = c == '-' ? x : -x;
                return 1;
            }
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------
int_ CMaaString::ToInt(int_ iDefault, int_ iStrict) const noexcept // on overflow or errors return iDefault
{
    const char * p = *this;
    const char * e = p + Length();
    if (iStrict >= 0)
    {
        while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    }
    if (p < e)
    {
        const char c = *p;
        if (c == '+' || c == '-')
        {
            p++;
        }
        if (p < e && *p >= '0' && *p <= '9')
        {
            int_ x = -(*p++ - '0');
            const int_ thres = (INT_MIN + 9) / 10;
            while (p < e && *p >= '0' && *p <= '9')
            {
                if (x >= thres)
                {
                    x = x * 10 - (*p++ - '0');
                }
                else
                {
                    int_ y = x;
                    x *= 10;
                    if  (x / 10 != y)
                    {
                        return iDefault;
                    }
                    y = x;
                    x -= *p++ - '0';
                    if  (x > y)
                    {
                        return iDefault;
                    }
                }
            }
            if (iStrict > 0)
            {
                while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
                if (iStrict == 2 && p < e && !*p)
                {
                    p = e;
                }
            }
            if (iStrict && p != e)
            {
                return iDefault;
            }
            if (c != '-')
            {
                x = -x;
                if (x < 0)
                {
                    return iDefault;
                }
            }
            return x;
        }
    }
    return iDefault;
}
//-----------------------------------------------------------------------------------------------
_qword CMaaString::ToQword(_qword qDefault, int_ iStrict) const noexcept // on overflow or errors ret qDefault
{
    const char * p = *this;
    const char * e = p + Length();
    if (iStrict >= 0)
    {
        while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    }
    if (p < e)
    {
        const char c = *p;
        if (c == '+' || c == '-')
        {
            p++;
        }
        if (p < e && *p >= '0' && *p <= '9')
        {
            _qword x = -(*p++ - '0');
            const _qword thres = (LLONG_MIN + 9) / 10;
            while (p < e && *p >= '0' && *p <= '9')
            {
                if (x >= thres)
                {
                    x = x * 10 - (*p++ - '0');
                }
                else
                {
                    _qword y = x;
                    x *= 10;
                    if  (x / 10 != y)
                    {
                        return qDefault;
                    }
                    y = x;
                    x -= *p++ - '0';
                    if  (x > y)
                    {
                        return qDefault;
                    }
                }
            }
            if (iStrict > 0)
            {
                while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
                if (iStrict == 2 && p < e && !*p)
                {
                    p = e;
                }
            }
            if (iStrict && p != e)
            {
                return qDefault;
            }
            if (c != '-')
            {
                x = -x;
                if (x < 0)
                {
                    return qDefault;
                }
            }
            return x;
        }
    }
    return qDefault;
}
//-----------------------------------------------------------------------------------------------
#if 1
//-----------------------------------------------------------------------------------------------
int_ CMaaString::sscanfUInt(uint_ *pRet) const noexcept // sscanf(ptr,"%u", &x) - like
{
    const char * p = *this;
    const char * e = p + Length();
    while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    if (p < e)
    {
        const char c = *p;
        if (c == '+')
        {
            p++;
        }
        if (p < e && *p >= '0' && *p <= '9')
        {
            uint_ x = (*p++ - '0');
            while (p < e && *p >= '0' && *p <= '9')
            {
                x = x * 10 + (*p++ - '0');
            }
            //while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
            //if (p == e)
            {
                *pRet = x;
                return 1;
            }
        }
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------
uint_ CMaaString::ToUInt(uint_ iDefault, int_ iStrict) const noexcept // on overflow or errors return iDefault
{
    const char * p = *this;
    const char * e = p + Length();
    if (iStrict >= 0)
    {
        while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    }
    if (p < e)
    {
        const char c = *p;
        if (c == '+')
        {
            p++;
        }
        if (p < e && *p >= '0' && *p <= '9')
        {
            uint_ x = (*p++ - '0');
            const uint_ thres = (UINT_MAX - 9) / 10;
            while (p < e && *p >= '0' && *p <= '9')
            {
                if (x <= thres)
                {
                    x = x * 10 + (*p++ - '0');
                }
                else
                {
                    uint_ y = x;
                    x *= 10;
                    if  (x / 10 != y)
                    {
                        return iDefault;
                    }
                    y = x;
                    x += *p++ - '0';
                    if  (x < y)
                    {
                        return iDefault;
                    }
                }
            }
            if (iStrict > 0)
            {
                while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
                if (iStrict == 2 && p < e && !*p)
                {
                    p = e;
                }
            }
            if (iStrict && p != e)
            {
                return iDefault;
            }
            return x;
        }
    }
    return iDefault;
}
//-----------------------------------------------------------------------------------------------
_uqword CMaaString::ToUQword(_uqword qDefault, int_ iStrict) const noexcept // on overflow or errors ret qDefault
{
    const char * p = *this;
    const char * e = p + Length();
    if (iStrict >= 0)
    {
        while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    }
    if (p < e)
    {
        const char c = *p;
        if (c == '+')
        {
            p++;
        }
        if (p < e && *p >= '0' && *p <= '9')
        {
            _uqword x = (*p++ - '0');
            const _uqword thres = (ULLONG_MAX - 9) / 10;
            while (p < e && *p >= '0' && *p <= '9')
            {
                if (x <= thres)
                {
                    x = x * 10 + (*p++ - '0');
                }
                else
                {
                    _uqword y = x;
                    x *= 10;
                    if  (x / 10 != y)
                    {
                        return qDefault;
                    }
                    y = x;
                    x += *p++ - '0';
                    if  (x < y)
                    {
                        return qDefault;
                    }
                }
            }
            if (iStrict > 0)
            {
                while(p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
                if (iStrict == 2 && p < e && !*p)
                {
                    p = e;
                }
            }
            if (iStrict && p != e)
            {
                return qDefault;
            }
            return x;
        }
    }
    return qDefault;
}
//-----------------------------------------------------------------------------------------------
// 0xffffffff == 4294967295
// 0xffffffffffffffff == 18446744073709551600
//0
//-2147483647
//-2,147,483,647
//4294967295
//+4,294,967,295
//-9223372036854775807
//-9,223,372,036,854,775,807
//18446744073709551615
//+18,446,744,073,709,551,615
//#define MAX_QWORD_STR_LEN (21 + 20/3)
#define MAX_QWORD_STR_LEN 32
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(int_ x) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const int_ d10 = 10;
            for (int_ y = x > 0 ? -x : x; y; y /= d10)
            {
                *(--p) = '0' - (char)(y % d10);
            }
            if (x < 0)
            {
                *(--p) = '-';
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(_qword x) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const int_ d10 = 10;
            for (_qword y = x > 0 ? -x : x; y; y /= d10)
            {
                *(--p) = '0' - (char)(y % d10);
            }
            if (x < 0)
            {
                *(--p) = '-';
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(int_ x, char mode, bool) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            const bool mode_s = (mode == '+' || mode == ('+' ^ ','));
            const bool mode_c = (mode == ',' || mode == ('+' ^ ','));
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const int_ d10 = 10;
            int_ nDigits = 0;
            for (int_ y = x > 0 ? -x : x; y; y /= d10)
            {
                if (mode_c)
                {
                    if (nDigits && !(nDigits % 3))
                    {
                        *(--p) = ',';
                    }
                    nDigits++;
                }
                *(--p) = '0' - (char)(y % d10);
            }
            if (x < 0)
            {
                *(--p) = '-';
            }
            else if (mode_s)
            {
                *(--p) = '+';
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(_qword x, char mode, bool) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            const bool mode_s = (mode == '+' || mode == ('+' ^ ','));
            const bool mode_c = (mode == ',' || mode == ('+' ^ ','));
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const int_ d10 = 10;
            int_ nDigits = 0;
            for (_qword y = x > 0 ? -x : x; y; y /= d10)
            {
                if (mode_c)
                {
                    if (nDigits && !(nDigits % 3))
                    {
                        *(--p) = ',';
                    }
                    nDigits++;
                }
                *(--p) = '0' - (char)(y % d10);
            }
            if (x < 0)
            {
                *(--p) = '-';
            }
            else if (mode_s)
            {
                *(--p) = '+';
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
/*
CMaaString CMaaString(const CMaaStringG& x) noexcept
{
    return x;
}
CMaaString CMaaString(const CMaaConstStr& x) noexcept
{
    return CMaaString(x);
}
CMaaString CMaaString(int_ x) ToString_noexcept
{
    CMaaString r;
    r.ToString(x);
    return r;
}
CMaaString CMaaString(_qword x) ToString_noexcept
{
    CMaaString r;
    r.ToString(x);
    return r;
}
CMaaString CMaaString(int_ x, char mode) ToString_noexcept // mode = 0, '+', ',', '+'^','
{
    CMaaString r;
    r.ToString(x, mode);
    return r;
}
CMaaString CMaaString(_qword x, char mode) ToString_noexcept
{
    CMaaString r;
    r.ToString(x, mode);
    return r;
}
*/
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(uint_ x) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const uint_ d10 = 10;
            for (uint_ y = x; y; y /= d10)
            {
                *(--p) = '0' + (char)(y % d10);
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(_uqword x) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const uint_ d10 = 10;
            for (_uqword y = x; y; y /= d10)
            {
                *(--p) = '0' + (char)(y % d10);
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(uint_ x, char mode, bool) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const uint_ d10 = 10;
            int_ nDigits = 0;
            for (uint_ y = x; y; y /= d10)
            {
                if (mode == ',' || mode == ('+' ^ ','))
                {
                    if (nDigits && !(nDigits % 3))
                    {
                        *(--p) = ',';
                    }
                    nDigits++;
                }
                *(--p) = '0' + (char)(y % d10);
            }
            if (mode == '+' || mode == ('+' ^ ','))
            {
                *(--p) = '+';
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(_uqword x, char mode, bool) noexcept
{
    try
    {
        if (!x)
        {
            ctor("0", 1);
        }
        else
        {
            char tmp[MAX_QWORD_STR_LEN];
            char* p = &tmp[MAX_QWORD_STR_LEN];
            const uint_ d10 = 10;
            int_ nDigits = 0;
            for (_uqword y = x; y; y /= d10)
            {
                if (mode == ',' || mode == ('+' ^ ','))
                {
                    if (nDigits && !(nDigits % 3))
                    {
                        *(--p) = ',';
                    }
                    nDigits++;
                }
                *(--p) = '0' + (char)(y % d10);
            }
            if (mode == '+' || mode == ('+' ^ ','))
            {
                *(--p) = '+';
            }
            ctor(p, (int)(&tmp[MAX_QWORD_STR_LEN] - p));
        }
    }
    catch (...)
    {
        m_pImp = sp_NullImp;
    }
}
//-----------------------------------------------------------------------------------------------
CMaaString::CMaaString(float x) noexcept
:   m_pImp(sp_NullImp)
{
    static constexpr int l = std::numeric_limits<float>::digits10 + 1;
    Format("%.*g", l, x);
}
CMaaString::CMaaString(double x) noexcept
:   m_pImp(sp_NullImp)
{
    static constexpr int l = std::numeric_limits<double>::digits10 + 1;
    Format("%.*lg", l, x);
}
CMaaString::CMaaString(long double x) noexcept
:   m_pImp(sp_NullImp)
{
    static constexpr int l = std::numeric_limits<long double>::digits10 + 1;
    Format("%.*llg", l, x);
}
//-----------------------------------------------------------------------------------------------
// [ ][{+|-}][цифры][.цифры][{e|E}[{+|-}]цифры]
template<typename T> T CMaaString::ToDouble_internal(T xDefault, bool* pbOk, int_ iStrict) const noexcept // on errors return xDefault
{
    bool bOk = false;

    T x = 0;
    const char* p = *this;
    const char* e = p + Length();
    if (iStrict >= 0)
    {
        while (p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
    }
    while (p < e)
    {
        char c = *p;
        if (c == '+' || c == '-')
        {
            p++;
        }
        if (p < e && ((*p >= '0' && *p <= '9') || *p == '.'))
        {
            int_ digits = 0;
            while (p < e && *p >= '0' && *p <= '9')
            {
                x = x * 10 + (*p++ - '0');
                digits++;
            }
            int_ ex = 0;
            if (p < e && *p == '.')
            {
                p++;
                if (!digits && (p >= e || *p < '0' || *p > '9'))
                {
                    break;
                }
                while (p < e && *p >= '0' && *p <= '9')
                {
                    x = x * 10 + (*p++ - '0');
                    ex--;
                }
            }
            if (c == '-')
            {
                x = -x;
            }
            if (ex)
            {
                x *= (T)pow((T)10, ex);
            }
            if (p < e && (*p == 'E' || *p == 'e'))
            {
                p++;
                if (p < e)
                {
                    c = *p;
                    if (c == '+' || c == '-')
                    {
                        p++;
                    }
                    if (p < e && (*p >= '0' && *p <= '9'))
                    {
                        ex = (*p++ - '0');
                        while (p < e && (*p >= '0' && *p <= '9'))
                        {
                            ex = ex * 10 + (*p++ - '0');
                        }
                        if (c == '-')
                        {
                            ex = -ex;
                        }
                        if (ex)
                        {
                            x *= (T)pow((T)10, ex);
                        }
                        bOk = true;
                    }
                }
            }
            else
            {
                bOk = true;
            }
        }
        break;
    }
    if (bOk)
    {
        if (iStrict > 0)
        {
            while (p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) p++;
            if (iStrict == 2 && p < e && !*p)
            {
                p = e;
            }
        }
        //if (!(iStrict && p != e))
        if (!iStrict || p == e)
        {
            if (pbOk)
            {
                *pbOk = true;
            }
            return x;
        }
    }
    if (pbOk)
    {
        *pbOk = false;
    }
    return xDefault;
}
//-----------------------------------------------------------------------------------------------
float CMaaString::ToFloat(float xDefault, bool* pbOk, int_ iStrict) const noexcept // on errors return xDefault
{
    return ToDouble_internal<float>(xDefault, pbOk, iStrict);
}
//-----------------------------------------------------------------------------------------------
double CMaaString::ToDouble(double xDefault, bool* pbOk, int_ iStrict) const noexcept // on errors return xDefault
{
    return ToDouble_internal<double>(xDefault, pbOk, iStrict);
}
//-----------------------------------------------------------------------------------------------
long double CMaaString::ToLongDouble(long double xDefault, bool* pbOk, int_ iStrict) const noexcept // on errors return xDefault
{
    return ToDouble_internal<long double>(xDefault, pbOk, iStrict);
}
//-----------------------------------------------------------------------------------------------
float CMaaString::ToFloat1(float xDefault, bool* pbOk) const noexcept // on errors return xDefault
{
    return ToDouble_internal<float>(xDefault, pbOk, 1);
}
//-----------------------------------------------------------------------------------------------
double CMaaString::ToDouble1(double xDefault, bool* pbOk) const noexcept // on errors return xDefault
{
    return ToDouble_internal<double>(xDefault, pbOk, 1);
}
//-----------------------------------------------------------------------------------------------
long double CMaaString::ToLongDouble1(long double xDefault, bool* pbOk) const noexcept // on errors return xDefault
{
    return ToDouble_internal<long double>(xDefault, pbOk, 1);
}
#endif
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::QuoteShArg(bool * pbErr) const noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_16K];

    // not right. fix me!!
    CMaaString src_special_chars = "\'\"&\\\n\r\t\b\f!|<>?$`";
    CMaaString dst_replacements = "\\'\\\"\\&\\\\\\n\\r\\t\\b\\f\\!\\|\\<\\>\\?\\$\\`";

    bool b;
    pbErr = pbErr ? pbErr : &b;

    const char* pSrc = *this;
    const int Len = Length();

    CMaaConcatString str(Buffer, sizeof(Buffer));
    for (int i = 0; i < Len; i++)
    {
        const char c = pSrc[i];
        if  ((unsigned char)c < (unsigned char)' ')
        {
            str += ' ';
            *pbErr = true;
            continue;
        }
        const int n = src_special_chars.Find(c);
        if  (n < 0)
        {
            str += c;
        }
        else
        {
            str += dst_replacements.RefMid(2 * n, 2);
        }
    }
    return str;
}
//-----------------------------------------------------------------------------------------------
bool CMaaString::IsValidVarName() const noexcept
{
    const char* pSrc = *this;
    const int Len = Length();

    for (int i = 0; i < Len; i++)
    {
        const char c = pSrc[i];
        if  (c >= '0' && c <= '9' && i > 0)
        {
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        {
        }
        else
        {
            return false;
        }
    }
    return Len > 0;
}
//-----------------------------------------------------------------------------------------------
bool CMaaString::IsValidPassword() const noexcept
{
    const char* pSrc = *this;
    const int Len = Length();

    for (int i = 0; i < Len; i++)
    {
        const char c = pSrc[i];
        if  (c >= '0' && c <= '9')
        {
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
        {
        }
        else
        {
            return false;
        }
    }
    return Len > 0;
}
//-----------------------------------------------------------------------------------------------
bool CMaaString::IsValidPath(int_ /*Flags*/) const noexcept // Is simple path [0-9a-zA-Z_/.]
{
    const char* pSrc = *this;
    const int Len = Length();

    for (int i = 0; i < Len; i++)
    {
        const char c = pSrc[i];
        if  (c >= '0' && c <= '9')
        {
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == FILESYSTEM_SLASH || c == '.')
        {
        }
        else
        {
            return false;
        }
    }
    return Len > 0;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::CutZeroFractionPart(bool bCutLeadingZerosToo) const noexcept(noexcept_new)
{
    CMaaString s = *this;
    int dot = s.Find('.');
    int l;
    for (l = dot + 1; l < s.Length() && s[l] >= '0' && s[l] <= '9'; l++);
    int nz;
    for (nz = l - 1; s[nz] == '0'; nz--);
    if  (nz == dot && nz > 0 && s[nz - 1] >= '0' && s[nz - 1] <= '9')
    {
        s = std::move(s.RefLeft(dot) + s.RefMid(l));
    }
    else if (nz == dot && l > dot + 1)
    {
        s = std::move(s.RefLeft(dot) + "0" + s.RefMid(l));
        dot++;
    }
    else if (nz > dot && nz < l - 1)
    {
        s = std::move(s.RefLeft(nz + 1) + s.RefMid(l));
    }
    if  (bCutLeadingZerosToo)
    {
        if  (dot < 0)
        {
            for (l = 0; l < s.Length() && (s[l] < '0' || s[l] > '9'); l++);
            if  (l < s.Length())
            {
                for (dot = l + 1; dot < s.Length() && s[dot] >= '0' && s[dot] <= '9'; dot++);
            }
        }
        if  (dot > 0)
        {
            for (l = dot - 1; l >= 0 && s[l] >= '0' && s[l] <= '9'; l--);
            for (nz = l + 1; nz < dot && s[nz] == '0'; nz++);
            if  (nz == dot)
            {
                s = std::move(s.RefLeft(l < 0 ? 0 : l + 1) + "0" + s.RefMid(dot));
            }
            else if (nz > l + 1 && nz < dot)
            {
                s = std::move(s.RefLeft(l < 0 ? 0 : l + 1) + s.RefMid(nz));
            }
        }
    }
    return s;
}
//-----------------------------------------------------------------------------------------------
//static constexpr CMaa256Bits gCp1251RusFlags("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя", 2 * 33, false);
int CMaaString::RussianAscii1251CharsCount(int_ CountOne) const noexcept
{
    //CMaaString RussianAlphabet = CMaaTLGlobalString(CMaaTLGlobalStrings::e_FullRussianAlphabet2); // "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя"
    const CMaa256Bits& sCp1251RusFlags_ = gCp1251RusFlags;
    int Count = 0;
    const char * pp = (const char *)*this;
    const char * ee = pp + Length();
    while(pp < ee)
    {
        if  (sCp1251RusFlags_.Test(*pp++))
        {
            Count++;
            if  (CountOne)
            {
                break;
            }
        }
    }
    return Count;
}
//-----------------------------------------------------------------------------------------------
int CMaaString::RussianOemCharsCount(int_ CountOne) const noexcept
{
    CMaaString RussianAlphabet = CMaaTLGlobalString(CMaaTLGlobalStrings::e_FullRussianAlphabet2); // "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя"
    int Count = 0;
    CMaaString str = OemToChar(); // TODO: except
    const char * pp = (const char *)(const char *)str;
    const char * ee = pp + str.Length();
    while(pp < ee)
    {
        const char c = *pp++;
        if  (RussianAlphabet.Find(c) >= 0)
        {
            Count++;
            if  (CountOne)
            {
                break;
            }
        }
    }
    return Count;
}
int CMaaString::Utf8SpecificCharsCount(int_ CountOne) const noexcept
{
    int Count = 0;
    const unsigned char * pp = (const unsigned char *)(const char *)*this;
    const unsigned char * ee = pp + Length();
    while(pp < ee)
    {
        //Count++;
        //_sdword UnicodeChar = *pp;
        if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
        {
            Count++;
            //UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
            pp += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
        {
            Count++;
            //UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
            pp += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
        {
            Count++;
            //UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
            //UnicodeChar = '_';
            pp += 4;
        }
        else
        {
            pp++;
        }
        if  (Count && CountOne)
        {
            break;
        }
    }
    return Count;
}
int CMaaString::Utf8PartOfCharsLength() const noexcept
{
    int Count = 0;
    const unsigned char * pp = (const unsigned char *)(const char *)*this;
    const unsigned char * ee = pp + Length();
    while(pp < ee)
    {
        //Count++;
        //_sdword UnicodeChar = *pp;
        if  ((*pp & 0xE0) == 0xC0 && (pp[1] & 0xC0) == 0x80)
        {
            //UnicodeChar = (pp[1] & 0x3f) | ((*pp & 0x1f) << 6);
            Count += 2;
            pp += 2;
        }
        else if ((*pp & 0xf0) == 0xe0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80)
        {
            //UnicodeChar = (pp[2] & 0x3f) | ((pp[1] & 0x3f) << 6) | ((*pp & 0x0f) << 12);
            Count += 3;
            pp += 3;
        }
        else if ((*pp & 0xf8) == 0xf0 && (pp[1] & 0xc0) == 0x80 && (pp[2] & 0xc0) == 0x80 && (pp[3] & 0xc0) == 0x80)
        {
            //UnicodeChar = (pp[3] & 0x3f) | ((pp[2] & 0x3f) << 6) | ((pp[1] & 0x3f) << 12) | ((*pp & 0x07) << 18);
            //UnicodeChar = '_';
            Count += 4;
            pp += 4;
        }
        else
        {
            pp++;
        }
    }
    return Count;
}
bool CMaaString::ListHas(const CMaaString &prop, const CMaaString &d) const noexcept(noexcept_new)
{
    CMaaString f = d;
    (f += prop) += d;
    return (f.Length() == 2 * d.Length() + prop.Length()) && Find(f) >= 0;
}
bool CMaaString::ListNotHas(const CMaaString &prop, const CMaaString &d) const noexcept(noexcept_new)
{
    CMaaString f = d;
    (f += prop) += d;
    return (f.Length() != 2 * d.Length() + prop.Length()) || Find(f) < 0;
}
bool CMaaString::ListAdd(const CMaaString &prop, int MaxLen, const CMaaString &d) noexcept(noexcept_new)
{
    if (IsRWSingleOwner())
    {
        const int len0 = Length();
        if (IsEmpty())
        {
            *this = d;
        }
        const int l = Length() + prop.Length() + d.Length();
        (*this += prop) += d;
        if (Length() != l || (MaxLen >= 0 && l > MaxLen))
        {
            TruncateRWS(len0);
            return false;
        }
        return true;
    }
    CMaaString x;
    if (IsEmpty())
    {
        x = d;
    }
    else
    {
        x = *this;
    }
    const int l = x.Length() + prop.Length() + d.Length();
    (x += prop) += d;
    if (x.Length() != l || (MaxLen >= 0 && l > MaxLen))
    {
        return false;
    }
    *this = std::move(x);
    return true;
}
bool CMaaString::ListRem(const CMaaString &prop, const CMaaString &d) noexcept(noexcept_new)
{
    CMaaString f = d;
    (f += prop) += d;
    if (f.Length() != 2 * d.Length() + prop.Length())
    {
        return false;
    }
    const int n = Find(f);
    if  (n > 0)
    {
        const int CutLen = d.Length() + prop.Length();
        const int NewLen = Length() - CutLen;
        if (IsRWSingleOwner())
        {
            memmove(m_pImp->m_pszStr + n, m_pImp->m_pszStr + n + CutLen, NewLen - n);
            TruncateRWS(NewLen);
            return true;
        }
        CMaaString x = RefLeft(n) + RefMid(n + CutLen);
        if (x.Length() != NewLen)
        {
            return false;
        }
        *this = std::move(x);
        return true;
    }
    else if (!n)
    {
        Empty();
        return true;
    }
    return false;
}
CMaaString CMaaString::List2Sql(const CMaaString &d) const noexcept(noexcept_new)
{
    CMaaString Ret;
    if  (Length() > 2 * d.Length())
    {
        Ret = std::move(RefMid(d.Length(), Length() - 2 * d.Length()));
        Ret.ReplaceNN(d, CMaaStringComma); // ","
    }
    return Ret;
}
CMaaString CMaaString::List4Sql(const CMaaString &d) const noexcept(noexcept_new)
{
    CMaaString Ret = *this;
    if  (Ret.IsNotEmpty())
    {
        Ret.ReplaceNN(CMaaStringComma, d); // ","
        Ret = std::move(d + Ret + d);
    }
    return Ret;
}
CMaaString CMaaString::Pass1Replace(CMaaUnivHash<CMaaString, CMaaString> &hVars, CMaaString Prefix, CMaaString Suffix) const noexcept(noexcept_new)
{
    CMaaString txt = *this;
    while(txt.Find(Prefix) >= 0)
    {
        Prefix = std::move(CMaaString("{[") + Prefix);
    }
    while(txt.Find(Suffix) >= 0)
    {
        Suffix = std::move(Suffix + "]}");
    }
    CMaaPtrAE_<CMaaString, 1> m(100);
    CMaaUnivHash<CMaaString, CMaaString>::iterator it(hVars);
    int_ N = 0;
    for (; it; ++it)
    {
        CMaaString v;
        v.Format("%S%d%S", &Prefix, N, &Suffix);
        txt.ReplaceNN(it.key(), v);
        m[N++] = it.data();
    }
    int pos = 0;
    while(pos < txt.Length())
    {
        pos = txt.Find(pos, Prefix);
        if  (pos < 0)
        {
            break;
        }
        int pos2 = pos + Prefix.Length();
        int_ n = -1;
        if  ((txt[pos2] >= '1' && txt[pos2] <= '9') || (txt[pos2] == '0' && txt[pos2 + 1] != '0'))
        {
            n = 0;
            while(txt[pos2] >= '0' && txt[pos2] <= '9')
            {
                n = n * 10 + ((char)txt[pos2++]) - '0';
            }
            if  (txt.IsMid(Suffix, pos2))
            {
                pos2 += Suffix.Length();
            }
            else
            {
                n = -1;
            }
        }
        if (n >= 0 && n < N)
        {
            txt = std::move(txt.RefMid(0, pos) + m[n] + txt.RefMid(pos2));
            pos += m[n].Length();
        }
        else
        {
            pos++;
        }
    }
    return txt;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::Utf8ToUtf8_3_Compatible(char chReplace, bool bCut) const noexcept
{
    try
    {
        const int l = Length();
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString r(Buffer, sizeof(Buffer), /*l + 1*/ (int)sizeof(Buffer));

        const unsigned char* pp = (unsigned char*)(const char*)*this;
        int pos0 = 0;
        for (int i = 0; i < l; )
        {
            //_sdword UnicodeChar = -1;
            if ((pp[i] & 0xE0) == 0xC0 && (i + 1 >= l || (pp[i + 1] & 0xC0) == 0x80))
            {
                //UnicodeChar = (pp[i + 1] & 0x3f) | ((pp[i] & 0x1f) << 6);
                if (i + 1 >= l)
                {
                    r.Add((const char*)pp + pos0, i - pos0);
                    if (!bCut)
                    {
                        r += chReplace;
                    }
                    pos0 = (i += 2);
                }
                else
                {
                    i += 2;
                }
            }
            else if ((pp[i] & 0xf0) == 0xe0 && (i + 1 >= l || ((pp[i + 1] & 0xc0) == 0x80 && (i + 2 >= l || (pp[i + 2] & 0xc0) == 0x80))))
            {
                //UnicodeChar = (pp[i + 2] & 0x3f) | ((pp[i + 1] & 0x3f) << 6) | ((pp[i] & 0x0f) << 12);
                if (i + 2 >= l)
                {
                    r.Add((const char*)pp + pos0, i - pos0);
                    if (!bCut)
                    {
                        r += chReplace;
                    }
                    pos0 = (i += 3);
                }
                else
                {
                    i += 3;
                }
            }
            else if ((pp[i] & 0xf8) == 0xf0 && (i + 1 >= l || ((pp[i + 1] & 0xc0) == 0x80 && (i + 2 >= l || ((pp[i + 2] & 0xc0) == 0x80 && (i + 3 >= l || (pp[i + 3] & 0xc0) == 0x80))))))
            {
                //UnicodeChar = (pp[i + 3] & 0x3f) | ((pp[i + 2] & 0x3f) << 6) | ((pp[i + 1] & 0x3f) << 12) | ((pp[i] & 0x07) << 18);
                //UnicodeChar = '_';
                //if (i + 3 >= l)
                {
                    r.Add((const char*)pp + pos0, i - pos0);
                    if (!bCut)
                    {
                        r += chReplace;
                    }
                    pos0 = (i += 4);
                }
                //else
                //{
                //    i += 4;
                //}
            }
            else
            {
                i++;
            }
        }
        if (!pos0)
        {
            return *this;
        }
        if (pos0 < l)
        {
            r.Add((const char*)pp + pos0, l - pos0);
        }
        return r;
    }
    catch (...)
    {
    }
    return CMaaStringZ;
}
//-----------------------------------------------------------------------------------------------
CMaaString CMaaString::Utf8ToUtf8_4_Compatible(char chReplace, bool bCut) const noexcept
{
    try
    {
        const int l = Length();
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString r(Buffer, sizeof(Buffer), /*l + 1*/ (int)sizeof(Buffer));

        const unsigned char* pp = (unsigned char*)(const char*)*this;
        int pos0 = 0;
        for (int i = 0; i < l; )
        {
            //_sdword UnicodeChar = -1;
            if ((pp[i] & 0xE0) == 0xC0 && (i + 1 >= l || (pp[i + 1] & 0xC0) == 0x80))
            {
                //UnicodeChar = (pp[i + 1] & 0x3f) | ((pp[i] & 0x1f) << 6);
                if (i + 1 >= l)
                {
                    r.Add((const char*)pp + pos0, i - pos0);
                    if (!bCut)
                    {
                        r += chReplace;
                    }
                    pos0 = (i += 2);
                }
                else
                {
                    i += 2;
                }
            }
            else if ((pp[i] & 0xf0) == 0xe0 && (i + 1 >= l || ((pp[i + 1] & 0xc0) == 0x80 && (i + 2 >= l || (pp[i + 2] & 0xc0) == 0x80))))
            {
                //UnicodeChar = (pp[i + 2] & 0x3f) | ((pp[i + 1] & 0x3f) << 6) | ((pp[i] & 0x0f) << 12);
                if (i + 2 >= l)
                {
                    r.Add((const char*)pp + pos0, i - pos0);
                    if (!bCut)
                    {
                        r += chReplace;
                    }
                    pos0 = (i += 3);
                }
                else
                {
                    i += 3;
                }
            }
            else if ((pp[i] & 0xf8) == 0xf0 && (i + 1 >= l || ((pp[i + 1] & 0xc0) == 0x80 && (i + 2 >= l || ((pp[i + 2] & 0xc0) == 0x80 && (i + 3 >= l || (pp[i + 3] & 0xc0) == 0x80))))))
            {
                //UnicodeChar = (pp[i + 3] & 0x3f) | ((pp[i + 2] & 0x3f) << 6) | ((pp[i + 1] & 0x3f) << 12) | ((pp[i] & 0x07) << 18);
                //UnicodeChar = '_';
                if (i + 3 >= l)
                {
                    r.Add((const char*)pp + pos0, i - pos0);
                    if (!bCut)
                    {
                        r += chReplace;
                    }
                    pos0 = (i += 4);
                }
                else
                {
                    i += 4;
                }
            }
            else
            {
                i++;
            }
        }
        if (!pos0)
        {
            return *this;
        }
        if (pos0 < l)
        {
            r.Add((const char*)pp + pos0, l - pos0);
        }
        return r;
    }
    catch (...)
    {
    }
    return CMaaStringZ;
}
//-----------------------------------------------------------------------------------------------
/*
\\      Literal backslash
\"      Double quote
\'      Single quote
\n      Newline (line feed)
\r      Carriage return
\b      Backspace
\t      Horizontal tab
\f      Form feed
\a      Alert (bell)
\v      Vertical tab
\?      Question mark (used to escape trigraphs)
\nnn    Character with octal value nnn
\xhh    Character with hexadecimal value hh
*/
CMaaString CMaaString::ToCSourceString() const noexcept(noexcept_new)
{
    CMaaConcatString d(2 * Length() + 100);
    int pos0 = 0;
    const char* p0 = (const char*)*this;
    const char* p = p0;
    const char* e = p + Length();
    for (; p < e; )
    {
        char c = *p++;
        switch (c)
        {
        case '\0':
        case '\r':
        case '\n':
        case '\t':
        case '\\':
        case '"':
        case '\'':
        case '\b':
        case '\f':
        case '\a':
        case '\v':
            d.Add(pos0 + p0, (int)(p - 1 - p0) - pos0);
            pos0 = (int)(p - p0);
            switch (c)
            {
            case '\0':
                d += (p < e ? "\\0\" \"" : "\\0");
                break;
            case '\r':
                d += "\\r";
                break;
            case '\n':
                d += "\\n";
                break;
            case '\t':
                d += "\\t";
                break;
            case '\\':
                d += "\\\\";
                break;
            case '"':
                d += "\\\"";
                break;
            case '\'':
                d += "\\'";
                break;
            case '\b':
                d += "\\b";
                break;
            case '\f':
                d += "\\f";
                break;
            case '\a':
                d += "\\a";
                break;
            case '\v':
                d += "\\v";
                break;
            }
        }
    }
    if (pos0)
    {
        d.Add(pos0 + p0, (int)(e - p0) - pos0);
        return d;
    }
    return *this;
}
//-----------------------------------------------------------------------------------------------
// *this == Hdr
CMaaString CMaaString::HttpGetValue(const CMaaString& Name0, int* pos, int* nextpos) const
{
    CMaaString Name = CMaaStringCrLf + Name0;

    CMaaString Val;
    int n2;
    int n = (warning_int)FindCi((n2 = pos ? *pos : 0), Name);
    if (n >= 0)
    {
        if (pos)
        {
            *pos = n + 2;
        }
        const int n3 = (warning_int)Find(n + Name.Length(), CMaaStringCrLf);
        if (n3 > 0)
        {
            n2 = n3;
            Val = Mid(n + Name.Length(), n2 - n - Name.Length());

            int n0 = n2 + 2;
            while ((*this)[n0] == '\t' || (*this)[n0] == ' ')
            {
                const int n3 = (warning_int)Find(n0, CMaaStringCrLf);
                if (n3 < 0)
                {
                    break;
                }
                Val += Mid(n0 + 1, n3 - n0 - 1);
                n2 = n3;
                n0 = n3 + 2;
            }

            int i;
            for (i = 0; i < (warning_int)Val.Length() && Val[i] == ' '; i++);
            Val = Val.RefMid(i);
            for (i = (warning_int)Val.Length(); i > 0 && Val[i - 1] == ' '; i--);
            Val = Val.Left(i);
        }
    }
    if (nextpos)
    {
        *nextpos = n2;
    }
    return Val;
}
//-----------------------------------------------------------------------------------------------
// *this == Hdr
CMaaString CMaaString::HttpGetValue(const char* Name0, int* pos, int* nextpos) const
{
    int len = (int)strlen(Name0);
    if (len > 68)
    {
        return HttpGetValue(CMaaString(Name0, len, CMaaString::eROMemString0), pos, nextpos);
    }

    char Name[68 + 4]; // Name[68 + 3];
    Name[0] = '\r';
    Name[1] = '\n';
    memcpy(Name + 2, Name0, (++len)++);

    CMaaString Val;
    int n2;
    int n = (warning_int)FindCi((n2 = pos ? *pos : 0), Name, len);
    if (n >= 0)
    {
        if (pos)
        {
            *pos = n + 2;
        }
        const int n3 = (warning_int)Find(n + len, CMaaStringCrLf);
        if (n3 > 0)
        {
            n2 = n3;
            Val = Mid(n + len, n2 - n - len);

            int n0 = n2 + 2;
            while ((*this)[n0] == '\t' || (*this)[n0] == ' ')
            {
                const int n3 = (warning_int)Find(n0, CMaaStringCrLf);
                if (n3 < 0)
                {
                    break;
                }
                Val += Mid(n0 + 1, n3 - n0 - 1);
                n2 = n3;
                n0 = n3 + 2;
            }

            int i;
            for (i = 0; i < (warning_int)Val.Length() && Val[i] == ' '; i++);
            Val = Val.RefMid(i);
            for (i = (warning_int)Val.Length(); i > 0 && Val[i - 1] == ' '; i--);
            Val = Val.Left(i);
        }
    }
    if (nextpos)
    {
        *nextpos = n2;
    }
    return Val;
}
//-----------------------------------------------------------------------------------------------
// *this == Hdr
CMaaString CMaaString::CgiGetValue(const CMaaString& Name, int* pos, int* nextpos) const
{
    CMaaString Ret = (CMaaStringCrLf + *this).s().HttpGetValue(Name, pos, nextpos);
    if (pos)
    {
        *pos = *pos >= 2 ? *pos - 2 : 0;
    }
    if (nextpos)
    {
        *nextpos = *nextpos >= 2 ? *nextpos - 2 : 0;
    }
    return Ret;
}
//-----------------------------------------------------------------------------------------------
// *this == Hdr
CMaaString CMaaString::CgiGetValue(const char* Name, int* pos, int* nextpos) const
{
    CMaaString Ret = (CMaaStringCrLf + *this).s().HttpGetValue(Name, pos, nextpos);
    if (pos)
    {
        *pos = *pos >= 2 ? *pos - 2 : 0;
    }
    if (nextpos)
    {
        *nextpos = *nextpos >= 2 ? *nextpos - 2 : 0;
    }
    return Ret;
}
//-----------------------------------------------------------------------------------------------
#if 0
CMaaString HttpGetValue(const CMaaString& Hdr, const char* Name, int* pos, int* nextpos)
{
    return Hdr.HttpGetValue(Name, pos, nextpos);
}
CMaaString HttpGetValue(const CMaaString& Hdr, const CMaaString& Name, int* pos, int* nextpos)
{
    return Hdr.HttpGetValue(Name, pos, nextpos);
}
CMaaString CgiGetValue(const CMaaString& Hdr, const char* Name, int* pos, int* nextpos)
{
    return Hdr.CgiGetValue(Name, pos, nextpos);
}
CMaaString CgiGetValue(const CMaaString& Hdr, const CMaaString& Name, int* pos, int* nextpos)
{
    return Hdr.CgiGetValue(Name, pos, nextpos);
}
#endif
//-----------------------------------------------------------------------------------------------
template <> void CMaaSwap<CMaaString>(CMaaString& a, CMaaString& b) noexcept
{
    a.Swap(b);
}
//-----------------------------------------------------------------------------------------------

#if TOOLSLIB_CMAASTRING64 <= 1
#ifndef TOOLSLIB_SINGLE_THREAD
void CMaaAllocatorBasicLock() noexcept
{
    // CMaaAllocatorBasicLock(true); // version before 19.10.2021
    gAllocatorBasicLock.Lock();
}

void CMaaAllocatorBasicUnLock() noexcept
{
    //CMaaAllocatorBasicLock(false); // version before 19.10.2021
    gAllocatorBasicLock.UnLock();
}
#endif
#endif

template <> _dword CMaaHashFunction<CMaaString>(const CMaaString& k) noexcept
{
    return k.Hash();
}
template <> _uqword CMaaHashFunction64<CMaaString>(const CMaaString& k) noexcept
{
    return k.Hash64();
}
//template <> _dword CMaaHashFunction<CMaaString>(const CMaaString& k) noexcept
//{
//    const _qword h = k.Hash64();
//    return (_dword)h ^ (_dword)(h >> 32);
//}

//CMaaString GetCopyrightText(int StartYear = 0, const char * format = "Copyright &copy; %1 MaaSoftware", time_t t = time(nullptr));
CMaaString GetCopyrightText(int_ StartYear, const char * format, time_t t) noexcept
{
    CMaaString r;
    int_ CurrentYear = StartYear;
    const tm * x = localtime(&t);
    if  (x)
    {
        CurrentYear = x->tm_year + 1900;
    }
    if  (StartYear <= 0)
    {
        StartYear = CurrentYear;
    }
    if  (StartYear <= 0)
    {
        StartYear = 2010;
    }
    char text[128];
    const int_ ly = (int_)SignedIntegerToString_d(StartYear, text);
    if  (CurrentYear > StartYear)
    {
        text[ly] = '-';
        SignedIntegerToString_d(CurrentYear, text + ly + 1);
    }
    r.Format2("%s%d", format, text, StartYear);
    return r;
}

//CRAM-MD5
//client: AUTH CRAM-MD5
//server: 334 PDM0NzcxNDI1NjAuNjYzMjU1N0BtYWFzb2Z0d2FyZS5jb20+
//client: c3VwcG9ydEBtYWFzb2Z0d2FyZS5ydSA1MzIxYzE5OWQzNWM1OWI4ODE4NjQxN2YyMTBmM2IwZg==
CMaaString CRAM_MD5_Auth(const CMaaString &Base64Request, const CMaaString &email, CMaaString pass) noexcept(noexcept_new)
{
    CMaaString Responce;
    bool bOk;
    CMaaString dec = std::move(Base64Request.Base64Decode(&bOk));
    if  (!bOk)
    {
        return Responce;
    }
    //printf("%S\n", &dec);
    //<1852080648.6626195@maasoftware.com>

    if  (pass.Length() > 64)
    {
        CMD5Cacl calc;
        calc.Update(pass, (warning_int)pass.Length());
        pass = calc.GetHash();
    }

    pass += CMaaString(nullptr, 64 - pass.Length());

    //CMaaString opad(nullptr, 64);
    //opad.Fill('\x5C');
    const char opad = '\x5C';

    //CMaaString ipad(nullptr, 64);
    //ipad.Fill('\x36');
    const char ipad = '\x36';

    char Data1[64];
    char Data2[64];

    for (int_ i = 0; i < 64; i++)
    {
        Data1[i] = (char)pass[i] ^ opad;
        Data2[i] = (char)pass[i] ^ ipad;
    }

    CMD5Cacl calc_intermediate;
    calc_intermediate.Update(Data2, 64);
    calc_intermediate.Update(dec, (warning_int)dec.Length());
    char Data3[16];
    calc_intermediate.GetHash(Data3);

    CMD5Cacl calc;
    calc.Update(Data1, 64);
    calc.Update(Data3, 16);

#if TOOLSLIB_CMAASTRING64 <= 1
    CMaaString md5 = std::move(calc.GetTextHash());
#else
    CMaaString md5 = std::move(calc.GetTextHash64());
#endif

    CMaaString src = std::move(email + " " + md5);
    Responce = std::move(src.Base64Encode());

    dec.Fill();
    pass.Fill();
    memset(Data1, 0, 64);
    memset(Data2, 0, 64);
    memset(Data3, 0, 16);
    md5.Fill();
    src.Fill();

    return Responce;
}

CMaaString GetTextSizeKBMBGB(_qword q, const char * m[] /*= nullptr*/) noexcept
{
    CMaaString Text;
    double q2 = (double)q;
    const char * mr[6] = {"Р‘Р°Р№С‚", "РљР‘", "РњР‘", "Р“Р‘", "РўР‘", nullptr};
    if  (!m)
    {
        m = mr;
    }
    for (int_ i = 0; ; i++)
    {
        if  (q < 1024 || !m[i + 1])
        {
            if  (!i)
            {
                Text.Format("%,D %s", q, m[i]);
            }
            else
            {
                if  (q2 >= 99.5)
                {
                    Text.Format("%.0lf %s", q2, m[i]);
                }
                else if (q2 >= 9.95)
                {
                    Text.Format("%.1lf %s", q2, m[i]);
                }
                else
                {
                    Text.Format("%.2lf %s", q2, m[i]);
                }
            }
            break;
        }
        q2 /= 1024.0;
        q /= 1024;
    }
    return Text;
}

//CMaaString SummToTextSumm(CMaaString input, int lang = 0, CMaaString *pSumm = nullptr, CMaaString *pSummSp = nullptr, CMaaString *pSummComma = nullptr);
//CMaaString NormalizeSummKIB(int len, CMaaString input);

//-------------------------------------------------------------------------
// F:\H-old\Inst_\L\Disks_Installs\200-3\D\13_3\d\4\Users\Andrey\1996_97\DOC.(_)
//-------------------------------------------------------------------------
static void copy_str(CMaaConcatString &txt, int &k, const CMaaString &text)
{
    txt += text;
    k += text.Length();
}
//-------------------------------------------------------------------------
static int NormalizeSumm(CMaaString &s, int maxlen = 64)
{
    char Buffer[128];
    CMaaConcatString o(Buffer, sizeof(Buffer), 128);

    int i, j, point = -1;
    char sym;

    o += '0'; j = 1;
    const int len = s.Length();
    const char* psz = s;
    for (i = 0; i < maxlen && i < len; i++)
    {
        if  (point >= 0 && j > point + 2)
        {
            break;
        }
        sym = psz[i];
        if  (sym >= '0' && sym <= '9')
        {
            o += sym; j++;
        }
        else if (sym == '.' && point < 0)
        {
            point = j;
            o += sym; j++;
        }
    }
    if  (point < 0)
    {
        point = j;
        o += '.'; j++;
    }
    while((j - point) < 3)
    {
        o += '0'; j++;
    }

    s = (CMaaString)o;
    for (psz = s, j = 0; psz[j] == '0' && j + 1 < point; j++) ;

    s = s.RefMid(j);
    point -= j;

    return point;
}
//-------------------------------------------------------------------------
CMaaString NormalizeSummKIB(int len, const CMaaString &s)
{
    char Buffer[256];
    int i, point;

    CMaaString input = s;
    point = NormalizeSumm(input);
    CMaaConcatString good(Buffer, sizeof(Buffer), input.Length() / 3 * 4 + 10);

    const char* psz = input;
    for (i = 0; i < point - 1; i++)
    {
        good += psz[i];
        if  ((point - i - 1) % 3 == 0)
        {
            good += ',';
        }
    }
    good += psz[i];
    good += '=';
    good += input.RefMid(point + 1);
    CMaaString r = good;
    if  (r.Length() < len)
    {
        CMaaString z(nullptr, len - r.Length(), CMaaString::eNotInitMem);
        z.Fill(' ');
        r = std::move(z + r);
    }
    else
    {
        r = std::move(r.Right(len));
        if  (r[0] == ',')
        {
            r[0] = ' ';
        }
    }
    return r;
}
CMaaString NormalizeSummSpComma(const CMaaString &s)
{
    char Buffer[256];
    int i, point;

    CMaaString input = s;
    point = NormalizeSumm(input);
    CMaaConcatString good(Buffer, sizeof(Buffer), input.Length() / 3 * 4 + 10);

    const char* psz = input;
    for (i = 0; i < point - 1; i++)
    {
        good += psz[i];
        if  ((point - i - 1) % 3 == 0)
        {
            good += ' ';
        }
    }
    good += psz[i];
    good += ',';
    good += input.RefMid(point + 1);
    return good;
}
//-------------------------------------------------------------------------
static int move_cop_rus(CMaaConcatString &txt, int &k, const char * c, const CMaaString& s1, const CMaaString& s2, const CMaaString& s3)
{
    txt.Add(c, 2); k += 2;
    copy_str(txt, k, " ");
    int i = (c[0] - '0') * 10 + (c[1] - '0');

    //if   (i != 0 || need != 0)
    {
        //i %= 100;
        if  (i > 4 && i < 20)
        {
            copy_str(txt, k, s3);
        }
        else
        {
            i %= 10;
            if  (i == 1)
            {
                copy_str(txt, k, s1);
            }
            else if (i > 1 && i < 5)
            {
                copy_str(txt, k, s2);
            }
            else
            {
                copy_str(txt, k, s3);
            }
        }
        //copy_str(txt, k, " ");
    }
    return 0;
}
//-------------------------------------------------------------------------
static int move_num_rus(CMaaConcatString &txt, int &k, const char* num3, const CMaaString& s1, const CMaaString& s2, const CMaaString& s3, int need = 0, char t = 0)
{
    static const CMaaString number3[10] = { "", "СЃС‚Рѕ", "РґРІРµСЃС‚Рё", "С‚СЂРёСЃС‚Р°", "С‡РµС‚С‹СЂРµСЃС‚Р°", "РїСЏС‚СЊСЃРѕС‚", "С€РµСЃС‚СЊСЃРѕС‚", "СЃРµРјСЊСЃРѕС‚", "РІРѕСЃРµРјСЊСЃРѕС‚", "РґРµРІСЏС‚СЊСЃРѕС‚" };
    static const CMaaString number2[10] = { "", "", "РґРІР°РґС†Р°С‚СЊ", "С‚СЂРёРґС†Р°С‚СЊ", "СЃРѕСЂРѕРє", "РїСЏС‚СЊРґРµСЃСЏС‚", "С€РµСЃС‚СЊРґРµСЃСЏС‚", "СЃРµРјСЊРґРµСЃСЏС‚", "РІРѕСЃРµРјСЊРґРµСЃСЏС‚", "РґРµРІСЏРЅРѕСЃС‚Рѕ" };
    static const CMaaString number1[20] = { "РЅРѕР»СЊ", "РѕРґРёРЅ", "РґРІР°", "С‚СЂРё", "С‡РµС‚С‹СЂРµ", "РїСЏС‚СЊ", "С€РµСЃС‚СЊ", "СЃРµРјСЊ", "РІРѕСЃРµРјСЊ", "РґРµРІСЏС‚СЊ",
        "РґРµСЃСЏС‚СЊ", "РѕРґРёРЅРЅР°РґС†Р°С‚СЊ", "РґРІРµРЅР°РґС†Р°С‚СЊ", "С‚СЂРёРЅР°РґС†Р°С‚СЊ", "С‡РµС‚С‹СЂРЅР°РґС†Р°С‚СЊ", "РїСЏС‚РЅР°РґС†Р°С‚СЊ", "С€РµСЃС‚РЅР°РґС†Р°С‚СЊ", "СЃРµРјРЅР°РґС†Р°С‚СЊ", "РІРѕСЃРµРјРЅР°РґС†Р°С‚СЊ", "РґРµРІСЏС‚РЅР°РґС†Р°С‚СЊ" };
    static const CMaaString number1t[2] = { "РѕРґРЅР°", "РґРІРµ" };
    int i;

    i = (int)(num3[2] - '0');
    if  (i > 0)
    {
        copy_str(txt, k, number3[i]);
        copy_str(txt, k, " ");
    }
    i = (int)(num3[1] - '0') * 10 + (int)(num3[0] - '0');
    if  (i < 20 && (i != 0 || need == 2))
    {
        if  ((i == 1 || i == 2) && t == 't')
        {
            copy_str(txt, k, number1t[i - 1]);
            copy_str(txt, k, " ");
        }
        else if (i != 0 || k == 0)
        {
            copy_str(txt, k, number1[i]);
            copy_str(txt, k, " ");
        }
    }
    else if (i != 0)
    {
        copy_str(txt, k, number2[i / 10]);
        copy_str(txt, k, " ");
        if  (i % 10 != 0)
        {
            if  ((i % 10 == 1 || i % 10 == 2) && t == 't')
            {
                copy_str(txt, k, number1t[i % 10 - 1]);
            }
            else
            {
                copy_str(txt, k, number1[i % 10]);
            }
            copy_str(txt, k, " ");
        }
    }
    i += 100 * (int)(num3[2] - '0');
    if  (i != 0 || need != 0)
    {
        i %= 100;
        if  (i > 4 && i < 20)
        {
            copy_str(txt, k, s3);
        }
        else
        {
            i %= 10;
            if  (i == 1)
            {
                copy_str(txt, k, s1);
            }
            else if (i > 1 && i < 5)
            {
                copy_str(txt, k, s2);
            }
            else
            {
                copy_str(txt, k, s3);
            }
        }
        copy_str(txt, k, " ");
    }
    return 0;
}
//-------------------------------------------------------------------------
CMaaString SummToTextSumm_rub(const CMaaString &s, bool bFullText, CMaaString *pSumm, CMaaString *pSummSp, CMaaString *pSummComma)
{
    char Buffer[512];
    CMaaConcatString o(Buffer, sizeof(Buffer), 512);
    char num3[3];
    int i, j, k, point;

    CMaaString input = s;
    point = NormalizeSumm(input);

    k = 0;
    const char* psz = input;
    for (i = 13; i > 0; i -= 3)
    {
        for (j = i + 2; j >= i; j--)
        {
            if  (point - j >= 0)
            {
                num3[j - i] = psz[point - j];
            }
            else
            {
                num3[j - i] = '0';
            }
        }
        switch(i)
        {
        case 13: move_num_rus(o, k, num3, "С‚СЂРёР»Р»РёРѕРЅ", "С‚СЂРёР»Р»РёРѕРЅР°", "С‚СЂРёР»Р»РёРѕРЅРѕРІ"); break;
        case 10: move_num_rus(o, k, num3, "РјРёР»Р»РёР°СЂРґ", "РјРёР»Р»РёР°СЂРґР°", "РјРёР»Р»РёР°СЂРґРѕРІ"); break;
        case  7: move_num_rus(o, k, num3, "РјРёР»Р»РёРѕРЅ", "РјРёР»Р»РёРѕРЅР°", "РјРёР»Р»РёРѕРЅРѕРІ"); break;
        case  4: move_num_rus(o, k, num3, "С‚С‹СЃСЏС‡Р°", "С‚С‹СЃСЏС‡Рё", "С‚С‹СЃСЏС‡", 0, 't'); break;
        case  1:
            if  (k != 0)
            {
                bFullText ? move_num_rus(o, k, num3, "СЂСѓР±Р»СЊ", "СЂСѓР±Р»СЏ", "СЂСѓР±Р»РµР№", 1) : move_num_rus(o, k, num3, "СЂСѓР±.", "СЂСѓР±.", "СЂСѓР±.", 1);
            }
            else
            {
                bFullText ? move_num_rus(o, k, num3, "СЂСѓР±Р»СЊ", "СЂСѓР±Р»СЏ", "СЂСѓР±Р»РµР№", 2) : move_num_rus(o, k, num3, "СЂСѓР±.", "СЂСѓР±.", "СЂСѓР±.", 2);
            }
        }
    }
    bFullText ? move_cop_rus(o, k, point + 1 + (const char *)input, "РєРѕРїРµР№РєР°", "РєРѕРїРµР№РєРё", "РєРѕРїРµРµРє") : move_cop_rus(o, k, point + 1 + (const char *)input, "РєРѕРї.", "РєРѕРї.", "РєРѕРї.");

    if  (pSumm)
    {
        *pSumm = input;
    }
    CMaaString Ret = o;
    if  (pSummSp)
    {
        o.Empty();
        // 1.00 10.00 100.00
        // 1000.00
        i = (point - 1) / 3;
        o += input.RefLeft(point - i * 3);
        for (; i > 0; i--)
        {
            o += ' ';
            o += input.RefMid(point - 3 * i, 3);
        }
        o += input.RefMid(point);
        *pSummSp = (CMaaString)o;
    }
    if  (pSummComma)
    {
        o.Empty();
        // 1.00 10.00 100.00
        // 1000.00
        i = (point - 1) / 3;
        o += input.RefLeft(point - i * 3);
        for (; i > 0; i--)
        {
            o += ',';
            o += input.RefMid(point - 3 * i, 3);
        }
        o += input.RefMid(point);
        *pSummComma = (CMaaString)o;
    }
    return Ret;
}
//-------------------------------------------------------------------------
static int move_cop_usd(CMaaConcatString &txt, int &k, const char * c, const CMaaString& s1, const CMaaString& s2, const CMaaString& s3)
{
    txt.Add(c, 2); k += 2;
    copy_str(txt, k, " ");
    int i = (c[0] - '0') * 10 + (c[1] - '0');

    //if   (i != 0 || need != 0)
    {
        //i %= 100;
        if  (i > 4 && i < 20)
        {
            copy_str(txt, k, s3);
        }
        else
        {
            i %= 10;
            if  (i == 1)
            {
                copy_str(txt, k, s1);
            }
            else if (i > 1 && i < 5)
            {
                copy_str(txt, k, s2);
            }
            else
            {
                copy_str(txt, k, s3);
            }
        }
        //copy_str(txt, k, " ");
    }
    return 0;
}
//-------------------------------------------------------------------------
static int move_num_usd(CMaaConcatString &txt, int &k, const char* num3, const CMaaString& s1, const CMaaString& s2, const CMaaString& s3, int need = 0, char t = 0)
{
    static const CMaaString number3[10] = {"", "one hundred", "two hundreds", "three hundreds", "four hundreds", "five hundreds", "six hundreds", "seven hundreds", "eight hundreds", "nine hundreds"};
    static const CMaaString number2[10] = {"", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};
    static const CMaaString number1[20] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
        "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
    static const CMaaString number1t[2] = {"one", "two"};

    int i;

    i = (int)(num3[2] - '0');
    if  (i > 0)
    {
        copy_str(txt, k, number3[i]);
        copy_str(txt, k, " ");
    }
    i = (int)(num3[1] - '0') * 10 + (int)(num3[0] - '0');
    if  (i < 20 && (i != 0 || need == 2))
    {
        if  ((i == 1 || i == 2) && t == 't')
        {
            copy_str(txt, k, number1t[i - 1]);
            copy_str(txt, k, " ");
        }
        else if (i != 0 || k == 0)
        {
            copy_str(txt, k, number1[i]);
            copy_str(txt, k, " ");
        }
    }
    else if (i != 0)
    {
        copy_str(txt, k, number2[i / 10]);
        copy_str(txt, k, " ");
        if  (i % 10 != 0)
        {
            if  ((i % 10 == 1 || i % 10 == 2) && t == 't')
            {
                copy_str(txt, k, number1t[i % 10 - 1]);
            }
            else
            {
                copy_str(txt, k, number1[i % 10]);
            }
            copy_str(txt, k, " ");
        }
    }
    i += 100 * (int)(num3[2] - '0');
    if  (i != 0 || need != 0)
    {
        i %= 100;
        if  (i > 4 && i < 20)
        {
            copy_str(txt, k, s3);
        }
        else
        {
            i %= 10;
            if  (i == 1)
            {
                copy_str(txt, k, s1);
            }
            else if (i > 1 && i < 5)
            {
                copy_str(txt, k, s2);
            }
            else
            {
                copy_str(txt, k, s3);
            }
        }
        copy_str(txt, k, " ");
    }
    return 0;
}
//-------------------------------------------------------------------------
CMaaString SummToTextSumm_usd(const CMaaString &s, bool bFullText, CMaaString *pSumm, CMaaString *pSummSp, CMaaString *pSummComma)
{
    char Buffer[512];
    CMaaConcatString o(Buffer, sizeof(Buffer), 512);
    char num3[3];
    int i, j, k, point;

    CMaaString input = s;
    point = NormalizeSumm(input);

    k = 0;
    const char* psz = input;
    for (i = 13; i > 0; i -= 3)
    {
        for (j = i + 2; j >= i; j--)
        {
            if  (point - j >= 0)
            {
                num3[j - i] = psz[point - j];
            }
            else
            {
                num3[j - i] = '0';
            }
        }
        switch(i)
        {
        case 13: move_num_usd(o, k, num3, "trillion", "trillions", "trillions"); break;
        case 10: move_num_usd(o, k, num3, "billion", "billions", "billions"); break;
        case  7: move_num_usd(o, k, num3, "million", "millions", "millions"); break;
        case  4: move_num_usd(o, k, num3, "thousand", "thousands", "thousands", 0, 't'); break;
        case  1:
            if  (k != 0)
            {
                bFullText ? move_num_usd(o, k, num3, "dollar", "dollars", "dollars", 1) : move_num_usd(o, k, num3, "dollar", "dollars", "dollars", 1);
            }
            else
            {
                bFullText ? move_num_usd(o, k, num3, "dollar", "dollars", "dollars", 2) : move_num_usd(o, k, num3, "dollar", "dollars", "dollars", 2);
            }
        }
    }
    bFullText ? move_cop_usd(o, k, point + 1 + (const char *)input, "cent", "cents", "cents") : move_cop_usd(o, k, point + 1 + (const char *)input, "c.", "c.", "c.");

    if  (pSumm)
    {
        *pSumm = input;
    }
    CMaaString Ret = o;
    if  (pSummSp)
    {
        o.Empty();
        i = (point - 1) / 3;
        o += input.RefLeft(point - i * 3);
        for (; i > 0; i--)
        {
            o += ' ';
            o += input.RefMid(point - 3 * i, 3);
        }
        o += input.RefMid(point);
        *pSummSp = (CMaaString)o;
    }
    if  (pSummComma)
    {
        o.Empty();
        i = (point - 1) / 3;
        o += input.RefLeft(point - i * 3);
        for (; i > 0; i--)
        {
            o += ',';
            o += input.RefMid(point - 3 * i, 3);
        }
        o += input.RefMid(point);
        *pSummComma = (CMaaString)o;
    }
    return Ret;
}
//-------------------------------------------------------------------------
CMaaString SummToTextSumm(const CMaaString &input, int_ lang, CMaaString *pSumm, CMaaString *pSummSp, CMaaString *pSummComma)
{
    CMaaString s;
    switch(lang & ~1)
    {
    case 6:
    case 4:
        s = std::move(SummToTextSumm_usd(input, true, pSumm, pSummSp, pSummComma));
        break;
    case 2:
        s = std::move(SummToTextSumm_rub(input, false, pSumm, pSummSp, pSummComma));
        break;
    case 0:
    default:
        s = std::move(SummToTextSumm_rub(input, true, pSumm, pSummSp, pSummComma));
    }
    if  (lang & 1)
    {
        s = std::move(s.Left(s.__Pos(1)).ToUpper(e_utf8_rus) + s.RefMid(s.__Pos(1)));
    }
    return s;
}
//-------------------------------------------------------------------------
#if 0
int test_text_summ(const char * n)
{
    CMaaString ns, sp, sc, k = NormalizeSummKIB(20, n);
    CMaaString u = SummToTextSumm(n, 4|1);
    CMaaString r1 = SummToTextSumm(n, 2|1);
    CMaaString r = SummToTextSumm(n, 0|1, &ns, &sp, &sc);

    //s = Utf8ToAnsi(s);
    ////s = AnsiToUnicode(s, false, 1251);
    ////UnicodeTo
    //s = s.Translit();
    __utf8_printf("%S : %S\n", &k, &u);
    __utf8_printf("%S : %S\n", &k, &r);
    __utf8_printf("%S : %S\n", &k, &r1);
    __utf8_printf("%S\n%S\n%S\n", &ns, &sp, &sc);
    return 0;
}
#endif
//-------------------------------------------------------------------------

bool CMaaIsValidDate(const CMaaString &Date, const CMaaString &Time, int_ mode)
{
    tm tt;
    memset(&tt, 0, sizeof(tt));
    int_ hh, mm, ss, d, m, y;
    hh = mm = ss = d = m = y = 0;
    if  ((mode == '/' && Date.dsscanf("%d/%d/%d", &y, &m, &d) == 3) || (mode != '/' && Date.dsscanf("%d.%d.%d", &d, &m, &y) == 3))
    {
        if  (y >= 1900 && y <= 2200 && m >= 1 && m <= 12 && d >= 1 && d <= 31)
        {
            if  (Time.IsNotEmpty() && Time.dsscanf("%d:%d:%d", &hh, &mm, &ss) < 2)
            {
                return false;
            }
            tt.tm_hour = 12;
            tt.tm_min = 0;
            tt.tm_sec = 0;
            tt.tm_mday = 1;
            tt.tm_mon = m - 1;
            tt.tm_year = y - 1900;
            tt.tm_isdst = -1;
            time_t t = mktime(&tt);
            if  (t != (time_t)-1)
            {
                t += (d - 1) * 24 * 3600;
                CMaaString s = GetTextDate(t, mode == -1 ? 0 : mode);
                if  (mode == -1)
                {
                    CMaaString s2 = GetTextDate(t, 2);
                    CMaaString s3 = s;
                    if  (s3.IsMid(".0", 2, 2))
                    {
                        s3.SetLeftMid(3, 4);
                    }
                    if  (s2 != Date && s != Date && s != CMaaString(" ") + Date && s3 != Date && s3 != CMaaString(" ") + Date)
                    {
                        return false;
                    }
                }
                else if (mode == 0)
                {
                    if  (s != Date && s != CMaaString(" ") + Date)
                    {
                        return false;
                    }
                }
                else if (s != Date)
                {
                    return false;
                }
                if  (hh >= 0 && hh < 24 && mm >= 0 && mm < 60 && ss >= 0 && ss < 60)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
// dt - is valid or empty
bool CMaaIsValidDateTime(CMaaString dt, int_ mode)
{
    CMaaString Date = dt.GetWord();
    CMaaString Time = dt.GetWord();
    if  (Time.IsEmpty())
    {
        //Time = "00:00:00";
    }
    return dt.IsEmpty() && CMaaIsValidDate(Date, Time, mode);
}

CMaaStringsContinedBuffer::CMaaStringsContinedBuffer(int BufferSize, int PaddingBytes) noexcept
:   m_BufferSize(BufferSize),
    m_PaddingBytes(PaddingBytes < 0 || PaddingBytes > 8 ? 8 : PaddingBytes)
{
}
CMaaString CMaaStringsContinedBuffer::Put(CMaaString s, bool bMemoryString) noexcept(noexcept_new)
{
    CMaaString Ret;
    const int len = s.Length();
    const int len2 = len + m_PaddingBytes;
    if  (m_CurrentBuffer.Length() - m_Start < len2 && len2 <= m_BufferSize)
    {
        m_CurrentBuffer = CMaaString(nullptr, m_BufferSize, CMaaString::eNotInitMem);
        m_Start = 0;
    }
    if  (m_CurrentBuffer.Length() - m_Start >= len2 && len2 >= 0 && (len > CMaaString::GetMaxUnAllocLength() || bMemoryString || len2 > CMaaString::GetMaxUnAllocLengthWithPadding()))
    {
        memcpy(m_Start + (char*)(const char*)m_CurrentBuffer, s, len);
        memset(len + m_Start + (char*)(const char*)m_CurrentBuffer, 0, m_PaddingBytes);
        Ret = std::move(m_CurrentBuffer.RefMid(m_Start, len));
        m_Start += len2;
    }
    else
    {
        Ret = !bMemoryString ? std::move(s) : CMaaString((const char*)s, len);
    }
    return Ret;
}
void CMaaStringsContinedBuffer::Reset() noexcept
{
    m_CurrentBuffer.Empty();
    m_Start = 0;
}


bool CMaaString::IsBadStrPtr(CMaaString *p)
{
    const char * SrcFile = __FILE__;
    const int_ SrcLine = __LINE__;
    //return !p || __is_bad_write_ptr(p, (int_)sizeof(CMaaString), CHECK_POINTERS == 2, SrcFile, SrcLine) || (p->m_pImp && (__is_bad_write_ptr(p->m_pImp, (int_)sizeof(CMaaStringImp), CHECK_POINTERS == 2, SrcFile, SrcLine) || __is_bad_read_ptr(p->m_pImp->m_pszStr, (size_t)p->m_pImp->m_iLength, CHECK_POINTERS == 2, SrcFile, SrcLine)));
    bool b_pImp = false;
    const bool b = !p || __is_bad_write_ptr(p, (int_)sizeof(CMaaString), CHECK_POINTERS == 2, SrcFile, SrcLine) || 
        (p->m_pImp != sp_NullImp && (
            __is_bad_write_ptr(p->m_pImp, (int_)sizeof(CMaaStringImp), CHECK_POINTERS == 2, SrcFile, SrcLine) ||
            //(b_pImp = !CMaaStringImp::s_pAllocator->IsObjPtr(p->m_pImp)) ||
            __is_bad_read_ptr(p->m_pImp->m_pszStr, (size_t)p->m_pImp->m_iLength, CHECK_POINTERS == 2, SrcFile, SrcLine)));
#ifdef _WIN32
    if (CHECK_POINTERS == 2 && b_pImp)
    {
        CMaaAccessMemoryCheckFailed(p->m_pImp, (int_)sizeof(CMaaStringImp), "CMaaStringImp::s_pAllocator->IsObjPtr()", SrcFile, SrcLine);
    }
#endif
    return b;
}

constexpr CMaaString::CE::CE() noexcept
:
#ifdef CMaaStringImp_NOT_NULL_MODE
    snZ{ ceCMaaStringImp::eNotInitImp },
#endif
    sn0{ "\0\0\0\0\0\0\0\0", 1, eROMemString0000 },
    sn00{ "\0\0\0\0\0\0\0\0", 2, eROMemString0000_16 },
    snWC0{ "\0\0\0\0\0\0\0\0", (int)sizeof(_WC_), eROMemString0000_16 },
    snWChar0{ "\0\0\0\0\0\0\0\0", (int)sizeof(wchar_t), sizeof(wchar_t) == 2 ? eROMemString0000_16 : eROMemString0000_32 },
    snWC32_0{ "\0\0\0\0\0\0\0\0", (int)sizeof(char32_t), eROMemString0000_32 },
    //static constexpr ceCMaaStringImp
    snEta{ "@" }, snSp{ " " },
    sn_a{ "a" }, sn_x{ "x" }, snLf{ "\n" }, snCr{ "\r" }, snCrLf{ "\r\n" },
    snSpSp{ "  " }, // sn_a, sn_z, sn_A, sn_Z
    snSemicolon{ ";" }, snComma{ "," }, snDot{ "." }, snDotDot{ ".." }, // snEllipse{ "..." },
    snDoc{ "d""oc" }, snStr2HtmlValEsc{ "&\"'<>" },

#ifdef CMaaStringImp_NOT_NULL_MODE
    sZ{ &snZ },
#else
    sZ{},
#endif
    s0{ &sn0 }, s00{ &sn00 }, sWC0{ &snWC0 }, sWChar0{ &snWChar0 }, sWC32_0{ &snWC32_0 },

    sEta{ &snEta }, sSp{ &snSp },
    s_a{ &sn_a }, s_x{ &sn_x },
    sLf{ &snLf }, sCr{ &snCr }, sCrLf{ &snCrLf }, sSpSp{ &snSpSp },
    // s_a, s_z, s_A, s_Z,
    sSemicolon{ &snSemicolon }, sComma{ &snComma }, sDot{ &snDot }, sDotDot{ &snDotDot }, // sEllipse { &snEllipse },
    sDoc{ &snDoc }, sStr2HtmlValEsc{ &snStr2HtmlValEsc }
{
}

constexpr CMaaString::CE2::CE2() noexcept
:
    snJsonVal{ "val" }, snJsonNode{ "node" }, snJsonArray{ "array" }, snJsonName{ "name" }, snJsonValue{ "value" }, snJsonType{ "type" },
    snJsonInt{ "int" }, snJsonBool{ "bool" }, snJsonFloat{ "float" }, snJsonNull{ "null" }, snJsonString{ "string" },

    sJsonVal{ &snJsonVal }, sJsonNode{ &snJsonNode }, sJsonArray{ &snJsonArray }, sJsonName{ &snJsonName },
    sJsonValue{ &snJsonValue }, sJsonType{ &snJsonType },
    sJsonInt{ &snJsonInt }, sJsonBool{ &snJsonBool }, sJsonFloat{ &snJsonFloat }, sJsonNull{ &snJsonNull }, sJsonString{ &snJsonString }
{
}

constexpr CMaaString::CE3::CE3() noexcept
:
    sn_Nums{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" }
    //, s_Nums {}
{
    for (int i = 0; i < 10; i++)
    {
        s_Nums[i] = &sn_Nums[i];
    }
}

static constexpr CMaaString::CE sCMaaStringC;
static constexpr CMaaString::CE2 sCMaaStringC2;
static constexpr CMaaString::CE3 sCMaaStringC3;
const CMaaString::CE & CMaaString::C() noexcept
{
    return sCMaaStringC;
}
const CMaaString::CE2 & CMaaString::C2() noexcept
{
    return sCMaaStringC2;
}
const CMaaString::CE3 & CMaaString::C3() noexcept
{
    return sCMaaStringC3;
}

constexpr CMaaTLGlobalStrings::CMaaTLGlobalStrings() noexcept
:   c
    {
        "-_.!~*'()",

        /*
        
        // _szOperators

        "(",

        "*",
        "/",
        "%",
        "+",
        "-",
        "<<",
        ">>",
        "<",
        ">",
        "<=",
        ">=",
        "==",
        "!=",
        "&",
        "^",
        "|",
        "&&",
        "||",

        "",
        ":",
        "?",
        "",
        "(",
        "",

        // _szFunctions1

        "ruleconnections",
        "shaperconnections",
        "time",
        "v",
        "traffic",
        "url",
        "host",
        "test",
        "",

        // _szFunctions2

        "false",
        "true",
        "break",
        "",

        // _sz_modificators

        "kbit/s",
        "mbit/s",
        "gbit/s",
        "tbit/s",
        "kbits/s",
        "mbits/s",
        "gbits/s",
        "tbits/s",
        "b/s",
        "kb/s",
        "mb/s",
        "gb/s",
        "tb/s",
        "b",
        "kb",
        "mb",
        "gb",
        "tb",
        "s",
        "m",
        "h",
        "d",
        ":",
        "",

        ////
        " :smhdSMHDbkmgtBKMGT)",
        "  smhdSMHDbkmgtBKMGT)",
        "smhdSMHD",
        " " "smhdSMHD",

        */

        "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя",
        "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя",
        //"abcdefghijklmnopqrstuvwxyzабвгдеёжзийклмнопрстуфхцчшщъыьэюя",
        //"ABCDEFGHIJKLMNOPQRSTUVWXYZАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ",

        "\r",
        "\n",
        "\r\n",
        "\n\r", // MAC?

        "(null)",
        "(bad_ptr)",
        "(bad_wstrptr)",
        "(bad_wc2ptr)",
        "(bad string flag)",
        "(bad string ptr)",

        "(null format string)",
        "(bad format string)",
        "(null text string)",
        "(bad text string)",
        "localtime() error, error getting date",
        "localtime() error, error getting time"//,

        //"End"
    }
    //, m{}
{
    for (int i = 0; i < e_max; i++)
    {
        m[i] = &c[i];
    }
#ifdef CMaaStringImp_NOT_NULL_MODE
    //m[CMaaTLGlobalStrings::e_max] = &CMaaString::C.snZ;
#else
    ////m[CMaaTLGlobalStrings::e_max] = &CMaaString::C.sn0; // complex test only
    ////m[CMaaTLGlobalStrings::e_max] = nullptr;
#endif
}

static constexpr CMaaTLGlobalStrings s_CMaaTLGlobalStrings;
const CMaaString& CMaaTLGlobalStrings::Get(int_ _e) noexcept
{
    return (const CMaaString&)s_CMaaTLGlobalStrings.m[_e];
}

#if 000
const CMaaString & CMaaStrCh(char C, bool bInit) noexcept
{
    static char ch[2 * 256];
    static CMaaString s[256];
    if (bInit && !ch[2 * 255])
    {
        for (int_ i = 0; i < 256; i++)
        {
            ch[2 * i] = (char)i;
            //ch[2 * i + 1] = '\0';
            s[i] = CMaaString(&ch[2 * i], 1, 0);
        }
    }
    return s[(unsigned char)C];
}
#endif

DEF_ALLOCATOR_CMaaUnivHash(CMaaString, char)
DEF_ALLOCATOR_CMaaUnivHash(CMaaString, unsigned char)
DEF_ALLOCATOR_CMaaUnivHash(CMaaString, CMaaString)

#if TOOLSLIB_USE_CMAASTRING64 == 1
#undef int
#undef int_strlen
#undef TOOLSLIB_CMAASTRING64
#endif
