
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/StringEx1.h

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

#if !defined(_WIN32) && !defined(__unix__)
     #ifndef __LARGE__
          #error Tools ( CMaaString::Format ) needs the large memory model
     #endif
#endif


#define CMAASTRING_CACHE_HASH   // has effects: need to call ClearHash() on buffer direct mod
//#define CMAASTRING_CACHE_HASH64 // has effects: need to call ClearHash() on buffer direct mod
//#ifdef _WIN32
//#define CMAASTRING_HAS_ADD_REF_DATA
//#define CMAASTRING_HAS_ADD_REF_DATA_PTR
//#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA
#ifndef CMAASTRING_CACHE_HASH64
#define CMAASTRING_HAS_ADD_REF_DATA_PTR
#endif
#endif

 //#define CMAASTRING_HAS_ADD_DATA
//#define CMAASTRING_HAS_UTF1632_INFO
//#define MANUAL_STRINGS_REFS false
#define MANUAL_STRINGS_REFS true

//#define TOOLSLIB_STAT_STRING_IMP

#define CMAASTRING_USE_UNALLOC_BUFFER // exhausted unalloc buffer
/*
#if defined(_WIN64)
#define CMAASTRING_USE_UNALLOC_BUFFER
#else
#ifndef _WIN32 // __linux__
#if __SIZEOF_POINTER__ >= 8
#define CMAASTRING_USE_UNALLOC_BUFFER
#endif
#endif
#endif
*/

/*
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif


#include <cstdint>
#if INTPTR_MAX == INT32_MAX
    #define THIS_IS_32_BIT_ENVIRONMENT
#elif INTPTR_MAX == INT64_MAX
    #define THIS_IS_64_BIT_ENVIRONMENT
#else
    #error "Environment not 32 or 64-bit."
#endif
*/

//---------------------------------------------------------------------------
//#define TOOLSLIB_TEST_CALC_HASH_CS
//#define TOOLSLIB_TEST_CALC_HASH64_CS
#ifdef TOOLSLIB_TEST_CALC_HASH_CS
template <int len> constexpr _dword CMaaStaticStandardHashFunctionCLK_ce(const char* p) noexcept
{
    if constexpr (len < (int)sizeof(_dword))
    {
        return CMaaMem04ExpandDwordCLK_ce(p, len);
    }
    else
    {
        _dword x;
#ifdef TRY_HF_OPT
#if defined(_WIN64) || defined(__linux__)
        if constexpr (len >= 2 * (int)sizeof(_uqword))
        {
            _uqword q = CMaaMem08ExpandUqwordCLK_ce(p, (int)sizeof(_uqword));
            p += sizeof(_uqword);
            for (int i = len >> _qword_shift; --i; )
            {
                q = CMaaRol7qwMacro(q) ^ CMaaMem08ExpandUqwordCLK_ce(p, (int)sizeof(_uqword));
                p += sizeof(_uqword);
            }
            x = (_dword)(q >> 32);
            x = CMaaRol7dwMacro(x) ^ (_dword)q;
            if constexpr ((len & (1 << _dword_shift)))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (int)sizeof(_dword));
                p += sizeof(_dword);
            }
            if constexpr ((len & _dword_mask))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (len & _dword_mask));
            }
        }
        else
#endif
        {
            x = CMaaMem04ExpandDwordCLK_ce(p, (int)sizeof(_dword));
            p += sizeof(_dword);
            for (int i = len >> _dword_shift; --i; )
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (int)sizeof(_dword));
                p += sizeof(_dword);
            }
            if constexpr ((len & _dword_mask))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (len & _dword_mask));
            }
        }
#else
        {
            x = 0;
            for (int i = len; i--; )
            {
                x = CMaaRol7dwMacro(x) ^ *p++;
            }
        }
#endif
        return x;
    }
}

template <int len> constexpr _dword CMaaStaticStandardHashFunction_ce(const char* p) noexcept
{
    if constexpr (len <= (int)sizeof(_dword))
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        return CMaaMem04ExpandDwordCLK_ce(p, len) ^ (_dword)len;
#else
        return CMaaMem04ExpandDwordCLK_ce(p, len);
#endif
    }
    else
    {
        _dword x;
#ifdef TRY_HF_OPT
#if defined(_WIN64) || defined(__linux__)
        if constexpr (len >= 2 * (int)sizeof(_uqword))
        {
#ifdef TOOLSLIB_HASH_USE_LEN
            _uqword q = len;
#else
            _uqword q = 0;
#endif
            for (int i = len >> _qword_shift; i--; )
            {
                q = CMaaRol7qwMacro(q) ^ CMaaMem08ExpandUqwordCLK_ce(p, (int)sizeof(_uqword));
                p += sizeof(_uqword);
            }
            x = (_dword)(q >> 32);
            x = CMaaRol7dwMacro(x) ^ (_dword)q;
            if constexpr ((len & (1 << _dword_shift)))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (int)sizeof(_dword));
                p += sizeof(_dword);
            }
            if constexpr ((len & _dword_mask))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (len & _dword_mask));
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
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (int)sizeof(_dword));
                p += sizeof(_dword);
            }
            if constexpr ((len & _dword_mask))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK_ce(p, (len & _dword_mask));
            }
        }
#else
        {
#ifdef TOOLSLIB_HASH_USE_LEN
            x = len;
#else
            x = 0;
#endif
            for (int i = len; i--; )
            {
                x = CMaaRol7dwMacro(x) ^ *p++;
            }
        }
#endif
        return x;
    }
}
#endif
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_TEST_CALC_HASH64_CS
template<int len> constexpr _uqword CMaaStaticStandardHashFunction64CLK_ce(const char* p) noexcept
{
    if constexpr (len < (int)sizeof(_uqword))
    {
        return CMaaMem08ExpandUqwordCLK_ce(p, len);
    }
    else
    {
#ifdef TRY_HF_OPT
        _uqword x = CMaaMem08ExpandUqwordCLK_ce(p, (int)sizeof(_uqword));
        p += sizeof(_uqword);
        for (int i = len >> _qword_shift; --i; )
        {
            x = CMaaRol7qwMacro(x) ^ CMaaMem08ExpandUqwordCLK_ce(p, (int)sizeof(_uqword));
            p += sizeof(_uqword);
        }
        if constexpr ((len & _qword_mask))
        {
            x = CMaaRol7qwMacro(x) ^ CMaaMem08ExpandUqwordCLK_ce(p, (len & _qword_mask));
        }
#else
        _uqword x = 0;
        for (int i = len; i--; )
        {
            x = CMaaRol7qwMacro(x) ^ *p++;
        }
#endif
        return x;
    }
}
template<int len> constexpr _uqword CMaaStaticStandardHashFunction64_ce(const char* p) noexcept
{
    if constexpr (len <= (int)sizeof(_uqword))
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        return CMaaMem08ExpandUqwordCLK_ce(p, len) ^ (_dword)len;
#else
        return CMaaMem08ExpandUqwordCLK_ce(p, len);
#endif
    }
    else
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        _uqword x = len;
#else
        _uqword x = 0;
#endif
#ifdef TRY_HF_OPT
        for (int i = len >> _qword_shift; i--; )
        {
            x = CMaaRol7qwMacro(x) ^ CMaaMem08ExpandUqwordCLK_ce(p, (int)sizeof(_uqword));
            p += sizeof(_uqword);
        }
        if constexpr ((len & _qword_mask))
        {
            x = CMaaRol7qwMacro(x) ^ CMaaMem08ExpandUqwordCLK_ce(p, (len & _qword_mask));
        }
#else
        for (int i = len; i--; )
        {
            x = CMaaRol7qwMacro(x) ^ *p++;
        }
#endif
        return x;
    }
}
#endif
//---------------------------------------------------------------------------


typedef int int_;
typedef unsigned int uint_;
typedef int warning_int;
typedef _dword warning_dword;
typedef _qword warning_int64;
inline warning_int int_strlen(const char* text) noexcept { return (warning_int)strlen(text); } // warning size mismatch
inline warning_int64 int64_strlen(const char* text) noexcept { return (warning_int64)strlen(text); } // warning size mismatch
#ifdef TOOLSLIB_CHAR8T_SUPPORT
inline int int_strlen(const char8_t* text) noexcept { return (warning_int)strlen((const char*)text); } // warning size mismatch
inline int int64_strlen(const char8_t* text) noexcept { return (warning_int)(warning_int64)strlen((const char*)text); } // warning size mismatch
#endif

//------------------------------------------------------------------------------
//#define USE_FIND_KMP // use KMP is slower!!
//------------------------------------------------------------------------------

typedef int* pINT_CMaaTL;
typedef _qword* pINT64_CMaaTL;
typedef const void* pcVOID_CMaaTL;
typedef const char* pcCHAR_CMaaTL;

inline constexpr void Byte2Hex_nzt_L(char* txt, _byte x) noexcept
{
    const _byte x1 = x >> 4;
    *txt = x1 < 10 ? '0' + x1 : 'a' + x1 - 10;
    const _byte x2 = x & 0xf;
    txt[1] = x2 < 10 ? '0' + x2 : 'a' + x2 - 10;
    //txt[2] = '\0';
}

inline constexpr void Byte2Hex_nzt_H(char* txt, _byte x) noexcept
{
    const _byte x1 = x >> 4;
    *txt = x1 < 10 ? '0' + x1 : 'A' + x1 - 10;
    const _byte x2 = x & 0xf;
    txt[1] = x2 < 10 ? '0' + x2 : 'A' + x2 - 10;
    //txt[2] = '\0';
}

inline constexpr void Byte2Hex_nzt_HL(char* txt, _byte x, char aA) noexcept // aA = {'a' | 'A'}
{
    const _byte x1 = x >> 4;
    *txt = x1 < 10 ? '0' + x1 : aA + x1 - 10;
    const _byte x2 = x & 0xf;
    txt[1] = x2 < 10 ? '0' + x2 : aA + x2 - 10;
    //txt[2] = '\0';
}

inline constexpr void Byte2Hex_nzt_LT(char* txt, _byte x, char t = 0) noexcept
{
    const _byte x1 = x >> 4;
    *txt = x1 < 10 ? '0' + x1 : 'a' + x1 - 10;
    const _byte x2 = x & 0xf;
    txt[1] = x2 < 10 ? '0' + x2 : 'a' + x2 - 10;
    txt[2] = t;
}

inline constexpr void Byte2Hex_nzt_HT(char* txt, _byte x, char t = 0) noexcept
{
    const _byte x1 = x >> 4;
    *txt = x1 < 10 ? '0' + x1 : 'A' + x1 - 10;
    const _byte x2 = x & 0xf;
    txt[1] = x2 < 10 ? '0' + x2 : 'A' + x2 - 10;
    txt[2] = t;
}

inline constexpr void Byte2Hex_nzt_HLT(char* txt, _byte x, char aA, char t = 0) noexcept // aA = {'a' | 'A'}
{
    const _byte x1 = x >> 4;
    *txt = x1 < 10 ? '0' + x1 : aA + x1 - 10;
    const _byte x2 = x & 0xf;
    txt[1] = x2 < 10 ? '0' + x2 : aA + x2 - 10;
    txt[2] = t;
}

inline constexpr int_ To0F_LH(char c) noexcept
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return -1;
}

inline constexpr int_ To0F_HL(char c) noexcept
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    return -1;
}

inline constexpr int_ To0F(char c) noexcept
{
    return To0F_HL(c);
}
#define dsscanf3264 dsscanf
#define dsscanfV3264 dsscanfV
bool mysscanf64(const char * str, _qword * ptr, int_ Len = -1, int * StopPos = nullptr, const char * StopChars = nullptr) noexcept;
bool mysscanfu64(const char * str, _uqword * ptr, int_ Len = -1, int * StopPos = nullptr, const char * StopChars = nullptr) noexcept;
bool mysscanf_float(const char* p, float* ptr, int_ Len = -1, int_* StopPos = nullptr, const char* StopChars = nullptr, int_ iStrict = 0) noexcept;
bool mysscanf_double(const char* p, double* ptr, int_ Len = -1, int_* StopPos = nullptr, const char* StopChars = nullptr, int_ iStrict = 0) noexcept;
bool mysscanf_long_double(const char* p, long double* ptr, int_ Len = -1, int_* StopPos = nullptr, const char* StopChars = nullptr, int_ iStrict = 0) noexcept;
int_ CMaaIsMemEq08(const char* p1, const char* p2, int_ count) noexcept; // for count == 0..8, false for count < 0
int_ CMaaIsMemEq0_16(const char* p1, const char* p2, int_ count) noexcept; // for count == 0..16, false for count < 0
int_ MaaMemCmpQ_C(const _qword* ptr1, const _qword* ptr2, int_ count) noexcept; // compares len >= 0 ptr1, ptr2; len < 0 return false
int_ MaaMemCmpQ_C(const _qword* ptr1, const _qword* ptr2, _qword count) noexcept; // compares len >= 0 ptr1, ptr2; len < 0 return false

#if 0 // instrinct memset(ptr, 0, count) is more effective
void CMaaMemZero0_16(char* p, int_ count) noexcept; // for count == 0..16 fill zeros
void MaaMemZeroQ_C(_qword* ptr, int_ count) noexcept; // fill zeros len >= 0 ptr
void MaaMemZeroQ_C(_qword* ptr, _qword count) noexcept; // fill zeros len >= 0 ptr
#endif

#ifdef TOOLSLIB_ASM64
//__utf8_printf("           alloc&fill(0),   ==,       ==\n");
//__utf8_printf("_WIN64 2000000000 bytes compare:\n");
//__utf8_printf("!memcmp:        291,528,  152,631,  152,113, 1, 0\n");
//__utf8_printf("MaaMemCmpQ_C:   294,455,  151,547,  153,506, 1, 0\n");
//__utf8_printf("MaaMemCmpQ_win: 293,042,  150,073,  151,194, 1, 0\n");
//__utf8_printf("_WIN32 2000000000/3 bytes compare:\n");
//__utf8_printf("!memcmp:        101,923,  80,951,  80,210, 1, 0\n");
//__utf8_printf("MaaMemCmpQ_C:   99,424,  64,218,  63,640, 1, 0\n");
//__utf8_printf("__linux__ (64) 2000000000 bytes compare:\n");
//__utf8_printf("!memcmp:        222,304,  135,243,  135,181, 1, 0\n");
//__utf8_printf("MaaMemCmpQ_C:   220,938,  155,321,  155,206, 1, 0\n");
//__utf8_printf("MaaMemCmpQ_win: 220,809,  159,939,  159,709, 1, 0\n");

#ifdef _WIN64________THE_SAME // this asm function is the same performance
extern "C"
{
    int MaaMemCmpQ_win(const void* ptr1, const void* ptr2, _uqword len) noexcept; // compares len ptr1, ptr2, len >= 0
}
#define MaaMemCmpQ MaaMemCmpQ_win
#else
//
#endif

#ifdef __linux__a________SLOW
extern "C"
{
    int MaaMemCmpQ_unix(const void* ptr1, const void* ptr2, _uqword len) noexcept; // compares len ptr1, ptr2, len >= 0
}
#define MaaMemCmpQ MaaMemCmpQ_unix
#endif

#endif

#ifdef MaaMemCmpQ
#define CMaaIsMemEq MaaMemCmpQ
#else
#ifdef __linux__
inline int CMaaIsMemEq(const void* ptr1, const void* ptr2, int count) noexcept // fast __linux__ memcmp
{
    return !memcmp(ptr1, ptr2, count); // fastest
    //return MaaMemCmpQ_C((const _qword*)ptr1, (const _qword*)ptr2, count);
}
#else
#define _WIN64__________
inline int CMaaIsMemEq(const void* ptr1, const void* ptr2, int count) noexcept // fast memcmp, use only count >= 0 for compatibility with !memcmp(ptr1,ptr2,count)
{
#ifdef _WIN64__________
    return !memcmp(ptr1, ptr2, count); // slower
#else
    return MaaMemCmpQ_C((const _qword*)ptr1, (const _qword*)ptr2, count);
#endif
}
inline int CMaaIsMemEq(const void* ptr1, const void* ptr2, _qword count) noexcept // fast memcmp, use only count >= 0 for compatibility with !memcmp(ptr1,ptr2,count)
{
#ifdef _WIN64__________
    return !memcmp(ptr1, ptr2, (size_t)count); // slower
#else
    return MaaMemCmpQ_C((const _qword*)ptr1, (const _qword*)ptr2, count);
#endif
}
inline int CMaaIsMemEq(const void* ptr1, const void* ptr2, size_t count) noexcept // fast memcmp, use only count >= 0 for compatibility with !memcmp(ptr1,ptr2,count)
{
#ifdef _WIN64__________
    return !memcmp(ptr1, ptr2, count); // slower
#else
    return MaaMemCmpQ_C((const _qword*)ptr1, (const _qword*)ptr2, (_qword)count);
#endif
}
#undef _WIN64__________
#endif
#endif

class ToolsExport CMaaString;
//--------------------------------------------------------------------------
//CMaaString DataToString ( const void * ptr, int len );
//--------------------------------------------------------------------------

//extern CMaaLiteMutex * gpCMaaSyncAssignLock;
#ifndef TOOLSLIB_SINGLE_THREAD
void gCMaaSyncAssignLockUpperLock(bool bInit) noexcept;
void gCMaaSyncAssignLockUpperLock() noexcept;
void gCMaaSyncAssignLockUpperUnLock() noexcept;
#endif

enum eCodePageConvert
{
    e_utf8 = 65001,
    e_utf8_rus = 65001,
    e_string_depending = -2,
    e_string_depending_rus = -2,
    e_string_depending_all = e_string_depending_rus
};

template<class T> void CMaaSyncAssign(T &a, const T &b, bool bInc, bool bDel = false, bool bLock = true, bool bInitLock = false) noexcept( noexcept(T(a)) && noexcept(a=b) )
{
#ifdef TOOLSLIB_A_A_ASSIGN_OPT
    if (bInc && b == a)
    {
        return; // 03.11.2024
    }
#endif
#ifndef TOOLSLIB_SINGLE_THREAD
    if (bLock)
    {
        gCMaaSyncAssignLockUpperLock(bInitLock);
    }
#endif
    if (bInc)
    {
        b && ((T &)b)->AddRef();
    }
    T tmp = a;
    a = b;
    if  (tmp)
    {
        if  (!tmp->Release() && bDel)
        {
            delete tmp;
        }
    }
#ifndef TOOLSLIB_SINGLE_THREAD
    if (bLock)
    {
        gCMaaSyncAssignLockUpperUnLock();
    }
#endif
}

template<class T> void CMaaSyncAssignFast(T &a, const T &b, bool bInc, bool bDel = false) noexcept( noexcept(T(a)) && noexcept(a=b) )
{
    //gCMaaSyncAssignLock.Lock();
    if (bInc)
    {
#ifdef TOOLSLIB_A_A_ASSIGN_OPT
        if (b == a)
        {
            return; // 03.11.2024
        }
#endif
        b && ((T &)b)->AddRef();
    }
    T tmp = a;
    a = b;
    if  (tmp)
    {
        if  (!tmp->Release() && bDel)
        {
            delete tmp;
        }
    }
    //gCMaaSyncAssignLock.UnLock();
}

//void CMaaSyncAssign<CMaaString::CMaaStringImp *>(CMaaString::CMaaStringImp * &a, const CMaaString::CMaaStringImp * &b, bool bInc);

//------------------------------------------------------------------------------
//constexpr CMaa256Bits gSpacesBits{ " \t", 2, false }; //, gSpacesBitsComma, gSpacesBitsColon, gSpacesBitsSemicolon, gSpacesBitsSpace, gSpacesBitsDot; // (" \t", 2); (",", 1); (":", 1); (";", 1); (" ", 1); (".", 1);

#ifdef TOOLSLIB_STAT_STRING_IMP
#ifdef TOOLSLIB_SINGLE_THREAD
extern int_ gCMaaStringImpStatShort, gCMaaStringImpStatLong, gCMaaStringImpStatRef, gCMaaStringImpStatMem, gCMaaStringImpStatRefs;
#else
extern std::atomic<int_> gCMaaStringImpStatShort, gCMaaStringImpStatLong, gCMaaStringImpStatRef, gCMaaStringImpStatMem, gCMaaStringImpStatRefs;
#endif
#define INC_TOOLSLIB_STAT_STRING_IMP_REFS(x) (x) && ++gCMaaStringImpStatRefs
#define DEC_TOOLSLIB_STAT_STRING_IMP_REFS --gCMaaStringImpStatRefs
#else
#define INC_TOOLSLIB_STAT_STRING_IMP_REFS(x)
#define DEC_TOOLSLIB_STAT_STRING_IMP_REFS
#endif
