
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/StringEx.h

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

#define TOOLSLIB_STR_HELPER_printf(x) 
//#define TOOLSLIB_STR_HELPER_printf(x) printf(x);

// Notes 04.01.2024:
// 1. (const char *)"A1" is forced casts to (char *), not to (const void *): e.AddAttribute("a2", (const char*)"A2");
// 2. CMaaString m[] = { "a", "bc", "def", "gh"}; is always checks for last '\0' for each element of array (is not optimized) VS2022
#define TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
#define TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr_TR
#ifdef TOOLSLIB_MAX_SPEED
#define CHECK_TXT_N1(txt, N) 0 // txt[N - 1]
#define TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
#else
#define CHECK_TXT_N1(txt, N) txt[N - 1] // 0
#endif
#define GET_CHECK_TXT_N1_N(txt, N) (!CHECK_TXT_N1(txt, N) ? N - 1 : N)

#if TOOLSLIB_USE_CMAASTRING64 == 1
typedef _uqword StrUInt;
#define int _qword
#define int_strlen int64_strlen
#define TOOLSLIB_CMAASTRING64 1
typedef int StrInt;
#endif

#if TOOLSLIB_USE_CMAASTRING64 == 0
typedef unsigned int StrUInt;
typedef int StrInt;
#define TOOLSLIB_CMAASTRING64 0
#endif

#if TOOLSLIB_USE_CMAASTRING64 == 2
typedef uint_ StrUInt;
typedef int_ StrInt;
#ifndef TOOLSLIB_CMAASTRING64
#define TOOLSLIB_CMAASTRING64 0
class CMaaString64;
//struct CMaaString64::CMaaStringImp;
#endif
#endif


_qword maa_wcslen(const _WC_* pText) noexcept;
_qword maa_wcslen32(const char32_t* pText) noexcept;

class ToolsExport CMaaString;
//--------------------------------------------------------------------------
//CMaaString DataToString ( const void * ptr, int len );
//--------------------------------------------------------------------------

#ifdef CMaaStringImp_NOT_NULL_MODE
#define PIMP(x) true
#define sp_NullImp (CMaaStringImp*)&CMaaString::C().snZ //sNullImp
#ifdef __linux__
//#define CHK_ERR_NULL_PIMP
#endif
#else
#define PIMP(x) (x)
#define sp_NullImp nullptr
#endif

class CMaaString;
class CMaaStringG;
//struct C;
#if (TOOLSLIB_USE_CMAASTRING64 == 2) && (TOOLSLIB_CMAASTRING64 == 0)
class CMaaString64;
typedef CMaaString CMaaString32;
//typedef C CMaaStringConsts32;
#endif

CMaaString Utf8ToUnicode(const CMaaString& Text, int_ AddRemLastCh = 0) noexcept; // (noexcept_new);
CMaaString Utf8ToUnicode32(const CMaaString& Text, int_ AddRemLastCh = 0) noexcept; // (noexcept_new);
CMaaString UnicodeToUnicodeBigEndian(const CMaaString& Text) noexcept(noexcept_new); // 16
CMaaString UnicodeBigEndianToUnicode(const CMaaString& Text) noexcept(noexcept_new); // 16

//
CMaaString UnicodeString(const _WC_* pText, int Len = -1, int_ AddRemLastCh = 0, bool bMemoryString = false) noexcept(noexcept_new);
CMaaString UnicodeToUtf8(const CMaaString& Text, int_ AddRemLastCh = 0) noexcept;// (noexcept_new);
CMaaString UnicodeToUtf8(const _WC_* pText, int Len = -1, int_ AddRemLastCh = 0) noexcept (noexcept_new);
//CMaaString UnicodeToUtf8(const _WC_ * pText, int Len, bool RemoveLastNullChar = true);

CMaaString Unicode32String(const char32_t* pText, int Len = -1, int_ AddRemLastCh = 0, bool bMemoryString = false) noexcept(noexcept_new);
CMaaString Unicode32ToUtf8(const CMaaString& Text, int_ AddRemLastCh = 0) noexcept; // (noexcept_new);
CMaaString Unicode32ToUtf8(const char32_t* pText, int Len = -1, int_ AddRemLastCh = 0) noexcept(noexcept_new);
//CMaaString Unicode32ToUtf8(const char32_t * pText, bool RemoveLastNullChar = true);

//CMaaString Utf8ToAnsiEx(const CMaaString& str);

// Linux: sizeof(wchar_t) == 4;  Windows: sizeof(wchar_t) == 2.
// Use char16_t *  ' _WC_ * str = u"\u4e66\u4e2d\u81ea\u6709\u9ec4\u91d1\u5c4b\u0000\u0000"; ',
// not          ' wchar_t * str = U"\U0001F37A\n"; ' for compatible strings
CMaaString wchar_t_ToUtf8(const wchar_t* pText, int Len = -1, int_ AddRemLastCh = 0) noexcept(noexcept_new);
//------------------------------------------------------------------------------
//#define CMaaString_TEST1
#if TOOLSLIB_CMAASTRING64 <= 1

#ifdef CMaaString_TEST1
/*
struct CC_Helper
{
public:
    const char* m;
    CC_Helper(const char* s) noexcept : m(s) {}
    //~CC_Helper() {}
};
*/
#endif

#ifdef _WIN32
#pragma pack( push, temp_pack_id_3, 1 )
#endif

// A literal class
struct CMaaConstStr
{
    const char* const p;
    const int len;
#ifdef TOOLSLIB_TEST_CALC_HASH_CS
    const _dword Hash;
#endif
#ifdef TOOLSLIB_TEST_CALC_HASH64_CS
    const _uqword Hash64;
#endif
public:
    template<int N> constexpr CMaaConstStr(const char(&txt)[N]) noexcept
    :   p(txt),
        len(GET_CHECK_TXT_N1_N(txt, N))
#ifdef TOOLSLIB_TEST_CALC_HASH_CS
        , Hash{ CMaaStaticStandardHashFunction_ce<GET_CHECK_TXT_N1_N(txt, N)>(txt) }
#endif
#ifdef TOOLSLIB_TEST_CALC_HASH64_CS
        , Hash64{ CMaaStaticStandardHashFunction64_ce<GET_CHECK_TXT_N1_N(txt, N)>(txt) }
#endif
    {
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT_0000000000000000
    template<int N> constexpr CMaaConstStr(const char8_t(&txt)[N]) noexcept
        : p((const char *)txt), // error
        len(GET_CHECK_TXT_N1_N(txt, N))
#ifdef TOOLSLIB_TEST_CALC_HASH_CS
        , Hash(CMaaStaticStandardHashFunction_ce<GET_CHECK_TXT_N1_N(txt, N)>(txt))
#endif
#ifdef TOOLSLIB_TEST_CALC_HASH64_CS
        , Hash64(CMaaStaticStandardHashFunction64_ce<GET_CHECK_TXT_N1_N(txt, N)>(txt))
#endif
    {
    }
#endif
    CMaaConstStr(char* txt) noexcept = delete;

    // constexpr functions signal errors by throwing exceptions
    // in C++11, they must do so from the conditional operator ?:
    /*constexpr char operator[](std::size_t n) const
    {
        return n < len ? p[n] : throw std::out_of_range("");
    }
    constexpr std::size_t Size() const noexcept
    {
        return sz;
    }*/

    constexpr int Length() const noexcept
    {
        return len;// sz > 0 && !p[sz - 1] ? sz - 1 : sz;
    }
} GCC_PACKED;
//#define TR_CMaaConstStr
//#define TOOLSLIB_USE_CMaaConstStr

struct CMaaConstStr2
{
    const char* p;
    //std::size_t sz;
    std::size_t len;
public:
    template<std::size_t N> constexpr CMaaConstStr2(const char(&txt)[N]) noexcept
        : p(txt),
        //sz(N),
        len(GET_CHECK_TXT_N1_N(txt, N))
    {
    }

    // constexpr functions signal errors by throwing exceptions
    // in C++11, they must do so from the conditional operator ?:
    /*constexpr char operator[](std::size_t n) const
    {
        return n < len ? p[n] : throw std::out_of_range("");
    }
    constexpr std::size_t Size() const noexcept
    {
        return sz;
    }*/

    constexpr std::size_t Length() const noexcept
    {
        return len;// sz > 0 && !p[sz - 1] ? sz - 1 : sz;
    }
} GCC_PACKED;

#ifdef _WIN32
#pragma pack( pop, temp_pack_id_3 )
#endif

#define gsCMaaStringZ ((const CMaaString&)CMaaString::C().sZ)
#define CMaaStringZ ((const CMaaString&)CMaaString::C().sZ)
#define CMaaString32Z ((const CMaaString32&)CMaaString32::C().sZ)
//#define CMaaStringZ CMaaString {} // or CMaaString() // !! avoid to use the address of temporarily
#define CMaaString0 ((const CMaaString&)CMaaString::C().s0)
#define CMaaString00 ((const CMaaString&)CMaaString::C().s00)
#define CMaaStringWC0 ((const CMaaString&)CMaaString::C().sWC0)
#define CMaaStringWChar0 ((const CMaaString&)CMaaString::C().sWChar0)
#define CMaaStringWC32_0 ((const CMaaString&)CMaaString::C().sWC32_0)
#define CMaaStringEta ((const CMaaString&)CMaaString::C().sEta)
#define CMaaStringSp ((const CMaaString&)CMaaString::C().sSp)
#define CMaaString_a ((const CMaaString&)CMaaString::C().s_a)
#define CMaaString_x ((const CMaaString&)CMaaString::C().s_x)
#define CMaaStringCr ((const CMaaString&)CMaaString::C().sCr)
#define CMaaStringLf ((const CMaaString&)CMaaString::C().sLf)
#define CMaaStringCrLf ((const CMaaString&)CMaaString::C().sCrLf)
#define CMaaStringSpSp ((const CMaaString&)CMaaString::C().sSpSp)
#define CMaaStringJsonVal ((const CMaaString&)CMaaString::C2().sJsonVal)
#define CMaaStringJsonNode ((const CMaaString&)CMaaString::C2().sJsonNode)
#define CMaaStringJsonArray ((const CMaaString&)CMaaString::C2().sJsonArray)
#define CMaaStringJsonName ((const CMaaString&)CMaaString::C2().sJsonName)
#define CMaaStringJsonValue ((const CMaaString&)CMaaString::C2().sJsonValue)
#define CMaaStringJsonType ((const CMaaString&)CMaaString::C2().sJsonType)
#define CMaaStringJsonInt ((const CMaaString&)CMaaString::C2().sJsonInt)
#define CMaaStringJsonBool ((const CMaaString&)CMaaString::C2().sJsonBool)
#define CMaaStringJsonFloat ((const CMaaString&)CMaaString::C2().sJsonFloat)
#define CMaaStringJsonNull ((const CMaaString&)CMaaString::C2().sJsonNull)
#define CMaaStringJsonString ((const CMaaString&)CMaaString::C2().sJsonString)

#define CMaaString_Nums ((const CMaaString*)CMaaString::C3().s_Nums)
#define CMaaString_0 ((const CMaaString&)CMaaString::C3().s_Nums[0])
#define CMaaString_1 ((const CMaaString&)CMaaString::C3().s_Nums[1])
#define CMaaString_2 ((const CMaaString&)CMaaString::C3().s_Nums[2])
#define CMaaString_3 ((const CMaaString&)CMaaString::C3().s_Nums[3])
#define CMaaString_4 ((const CMaaString&)CMaaString::C3().s_Nums[4])
#define CMaaString_5 ((const CMaaString&)CMaaString::C3().s_Nums[5])
#define CMaaString_6 ((const CMaaString&)CMaaString::C3().s_Nums[6])
#define CMaaString_7 ((const CMaaString&)CMaaString::C3().s_Nums[7])
#define CMaaString_8 ((const CMaaString&)CMaaString::C3().s_Nums[8])
#define CMaaString_9 ((const CMaaString&)CMaaString::C3().s_Nums[9])
/*
#define CMaaString_a ((const CMaaString&)CMaaString::C().s_a)
#define CMaaString_z ((const CMaaString&)CMaaString::C().s_z)
#define CMaaString_A ((const CMaaString&)CMaaString::C().s_A)
#define CMaaString_Z ((const CMaaString&)CMaaString::C().s_Z)
*/
#define CMaaStringSemicolon ((const CMaaString&)CMaaString::C().sSemicolon)
#define CMaaStringComma ((const CMaaString&)CMaaString::C().sComma)
#define CMaaStringDot ((const CMaaString&)CMaaString::C().sDot)
#define CMaaStringDotDot ((const CMaaString&)CMaaString::C().sDotDot)
//#define CMaaStringEllipse ((const CMaaString&)CMaaString::C().sEllipse)
#define CMaaStringDoc ((const CMaaString&)CMaaString::C().sDoc)
#define CMaaStringStr2HtmlValEsc ((const CMaaString&)CMaaString::C().sStr2HtmlValEsc)

#else

//------------------------------------------------------------------------------
#define gsCMaaString64Z ((const CMaaString&)CMaaString64::C().sZ)
#define CMaaString64Z ((const CMaaString&)CMaaString64::C().sZ)
//#define CMaaString64Z CMaaString64 {} // or CMaaString64() // !! avoid to use the address of temporarily
#define CMaaString640 ((const CMaaString&)CMaaString64::C().s0)
#define CMaaString6400 ((const CMaaString&)CMaaString64::C().s00)
#define CMaaString64WC0 ((const CMaaString&)CMaaString64::C().sWC0)
#define CMaaString64WChar0 ((const CMaaString&)CMaaString64::C().sWChar0)
#define CMaaString64WC32_0 ((const CMaaString&)CMaaString64::C().sWC32_0)
#define CMaaString64Eta ((const CMaaString&)CMaaString64::C().sEta)
#define CMaaString64Sp ((const CMaaString&)CMaaString64::C().sSp)
#define CMaaString64_a ((const CMaaString&)CMaaString64::C().s_a)
#define CMaaString64_x ((const CMaaString&)CMaaString64::C().s_x)
#define CMaaString64Cr ((const CMaaString&)CMaaString64::C().sCr)
#define CMaaString64Lf ((const CMaaString&)CMaaString64::C().sLf)
#define CMaaString64CrLf ((const CMaaString&)CMaaString64::C().sCrLf)
#define CMaaString64SpSp ((const CMaaString&)CMaaString64::C().sSpSp)
#define CMaaString64JsonVal ((const CMaaString&)CMaaString64::C2().sJsonVal)
#define CMaaString64JsonNode ((const CMaaString&)CMaaString64::C2().sJsonNode)
#define CMaaString64JsonArray ((const CMaaString&)CMaaString64::C2().sJsonArray)
#define CMaaString64JsonName ((const CMaaString&)CMaaString64::C2().sJsonName)
#define CMaaString64JsonValue ((const CMaaString&)CMaaString64::C2().sJsonValue)
#define CMaaString64JsonType ((const CMaaString&)CMaaString64::C2().sJsonType)
#define CMaaString64JsonInt ((const CMaaString&)CMaaString64::C2().sJsonInt)
#define CMaaString64JsonBool ((const CMaaString&)CMaaString64::C2().sJsonBool)
#define CMaaString64JsonFloat ((const CMaaString&)CMaaString64::C2().sJsonFloat)
#define CMaaString64JsonNull ((const CMaaString&)CMaaString64::C2().sJsonNull)
#define CMaaString64JsonString ((const CMaaString&)CMaaString64::C2().sJsonString)

#define CMaaString64_Nums ((const CMaaString*)CMaaString64::C3().s_Nums)
#define CMaaString64_0 ((const CMaaString&)CMaaString64::C3().s_Nums[0])
#define CMaaString64_1 ((const CMaaString&)CMaaString64::C3().s_Nums[1])
#define CMaaString64_2 ((const CMaaString&)CMaaString64::C3().s_Nums[2])
#define CMaaString64_3 ((const CMaaString&)CMaaString64::C3().s_Nums[3])
#define CMaaString64_4 ((const CMaaString&)CMaaString64::C3().s_Nums[4])
#define CMaaString64_5 ((const CMaaString&)CMaaString64::C3().s_Nums[5])
#define CMaaString64_6 ((const CMaaString&)CMaaString64::C3().s_Nums[6])
#define CMaaString64_7 ((const CMaaString&)CMaaString64::C3().s_Nums[7])
#define CMaaString64_8 ((const CMaaString&)CMaaString64::C3().s_Nums[8])
#define CMaaString64_9 ((const CMaaString&)CMaaString64::C3().s_Nums[9])
/*
#define CMaaString64_a ((const CMaaString&)CMaaString64::C().s_a)
#define CMaaString64_z ((const CMaaString&)CMaaString64::C().s_z)
#define CMaaString64_A ((const CMaaString&)CMaaString64::C().s_A)
#define CMaaString64_Z ((const CMaaString&)CMaaString64::C().s_Z)
*/
#define CMaaString64Semicolon ((const CMaaString&)CMaaString64::C().sSemicolon)
#define CMaaString64Comma ((const CMaaString&)CMaaString64::C().sComma)
#define CMaaString64Dot ((const CMaaString&)CMaaString64::C().sDot)
#define CMaaString64DotDot ((const CMaaString&)CMaaString64::C().sDotDot)
//#define CMaaString64Ellipse ((const CMaaString&)CMaaString64::C().sEllipse)
#define CMaaString64Doc ((const CMaaString&)CMaaString64::C().sDoc)
#define CMaaString64Str2HtmlValEsc ((const CMaaString&)CMaaString64::C().sStr2HtmlValEsc)

#endif
//------------------------------------------------------------------------------
bool IsCi(const char* s1, const char* s2, int Len, int_ cp = -1 /*default: +rus*/) noexcept; // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
bool IsCi0(const char* s1, const char* s2, int Len1, int_ cp = -1 /*default: +rus*/, bool bIsLeft1 = false) noexcept; // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
bool IsCi00(const char* s1, const char* s2, int_ cp = -1 /*default: +rus*/) noexcept; // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
//------------------------------------------------------------------------------
int_ dsscanfV3264(const char* s, int_ len, const char* fmt, va_list list) noexcept;
int_ dsscanf3264(const char* s, int_ len, const char* fmt, ...) noexcept;
int_ dsscanf3264(const char* s, const char* fmt, ...) noexcept;
//------------------------------------------------------------------------------
// class CMaaString
//------------------------------------------------------------------------------
// Imp flags
#define CMAASTRFL_eDontDelete   0x0001
#define CMAASTRFL_eHashExists   0x0002
#ifdef CMAASTRING_CACHE_HASH64
#define CMAASTRFL_eHash64Exists 0x0004
#define CMAASTRFL_eHash64_FreeBit__ 0
#else
#define CMAASTRFL_eHash64Exists 0
#define CMAASTRFL_eHash64_FreeBit__ 0x0004
#endif
#define CMAASTRFL_eHasReffered  0x0008
#define CMAASTRFL_eROString     0x0010
#define CMAASTRFL_eImp32        0x0020
#define CMAASTRFL_eImp64        0x0040
//#define CMAASTRFL_eImpMask (CMAASTRFL_eImp32 | CMAASTRFL_eImp64)
#define CMAASTRFL_eMemoryString 0x0080
#define CMAASTRFL_eLengthLimit1 0x0100
#define CMAASTRFL_eLengthLimit2 0x0200
#define CMAASTRFL_eLengthLimit4 0x0400
//#define CMAASTRFL_eLengthLimit (CMAASTRFL_eLengthLimit1 | CMAASTRFL_eLengthLimit2 | CMAASTRFL_eLengthLimit4)
#define CMAASTRFL_eCryptoKey_   0x0800

#ifdef CMAASTRING_HAS_ADD_REF_DATA

#ifdef CMAASTRING_HAS_ADD_DATA
#undef CMAASTRING_HAS_ADD_DATA
#endif
#define CMAASTRFL_eHasAddData          0
#define CMAASTRFL_eHasAddDataFreeBit__ 0
#ifdef CMAASTRING_HAS_UTF1632_INFO
#undef CMAASTRING_HAS_UTF1632_INFO
#endif
#define CMAASTRFL_eUtf16Flag 0
#define CMAASTRFL_eUtf32Flag 0
#define CMAASTRFL_eUtf1632Flag_FreeBits__ 0

#define CMAASTRFL_eHasAddRefData       0x1000
#define CMAASTRFL_eHasAddRefData0Eq    0x2000
#define CMAASTRFL_eHasAddRefData1      0x4000 // AddRefData is Data1

#else

#define CMAASTRFL_eHasAddRefData       0
#define CMAASTRFL_eHasAddRefData0Eq    0
#define CMAASTRFL_eHasAddRefData1      0

#ifdef CMAASTRING_HAS_ADD_DATA
#define CMAASTRFL_eHasAddData          0x1000
#define CMAASTRFL_eHasAddDataFreeBit__ 0
#else
#define CMAASTRFL_eHasAddDataFreeBit__ 0x1000
#define CMAASTRFL_eHasAddData          0
#endif
#ifdef CMAASTRING_HAS_UTF1632_INFO
#define CMAASTRFL_eUtf16Flag    0x2000
#define CMAASTRFL_eUtf32Flag    0x4000
#define CMAASTRFL_eUtf1632Flag_FreeBits__ 0
#else
#define CMAASTRFL_eUtf16Flag 0
#define CMAASTRFL_eUtf32Flag 0
#define CMAASTRFL_eUtf1632Flag_FreeBits__ (0x2000 | 0x4000)
#endif

#endif
#define CMAASTRFL_e_FreeBits__  0x8000
//------------------------------------------------------------------------------
/*
#ifdef CMAASTRING_UNALLOC_PTR
#undef CMAASTRING_UNALLOC_PTR
#endif
#ifdef CMAASTRING_UNALLOC_PTR_
#undef CMAASTRING_UNALLOC_PTR_
#endif
#ifdef CMAASTRING_USE_UNALLOC_BUFFER
#define CMAASTRING_UNALLOC_PTR (char *)&m_iCapacity
#define CMAASTRING_UNALLOC_PTR_(x) (char *)&(x)->m_iCapacity
#endif
*/
//------------------------------------------------------------------------------
struct ceCMaaStringImp
:
#if MANUAL_STRINGS_REFS
    public CMaaManualReference
#else
    public CMaaReference
#endif
{
    static const char sChar0_1[8];

#if defined(_WIN64)
    mutable _dword m_Padding0;
#endif
    // offset = (for win64/linux64)
#ifndef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
//#ifdef CMAASTRING_CACHE_HASH
    mutable _dword m_Hash; // uninitialized if (!(m_Flag & 2))
    //#endif
#endif
#endif
    mutable int    m_iLength;  // offset = 4/8 // length of string. real size 8 byte longer generally
#ifdef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
    //#ifdef CMAASTRING_CACHE_HASH
    mutable _dword m_Hash; // uninitialized if (!(m_Flag & 2))
    //#endif
#endif
#endif
    mutable char* m_pszStr;   // offset = 8/16

    union _f2
    {
        struct _f12
        {
#ifdef TOOLSLIB_LITTLE_ENDIAN
            mutable unsigned char m_Flags0, m_Flags1;
#else
            mutable unsigned char m_Flags1, m_Flags0;
#endif
            constexpr _f12() noexcept {}
            constexpr _f12(unsigned short x) noexcept : m_Flags0((unsigned char)x), m_Flags1((unsigned char)(x >> 8)) {}
            unsigned short GetFlags12() const noexcept { return ((unsigned short)m_Flags0) | (((unsigned short)m_Flags1) << 8); }
            void SetFlags12(unsigned short x) noexcept { m_Flags0 = (unsigned char)x; m_Flags1 = (unsigned char)(x >> 8); }
        };
        struct _a12
        {
#ifdef TOOLSLIB_LITTLE_ENDIAN
            mutable std::atomic<unsigned char> m_Flags0, m_Flags1;
#else
            mutable std::atomic<unsigned char> m_Flags1, m_Flags0;
#endif
            constexpr _a12() noexcept {}
            constexpr _a12(unsigned short x) noexcept : m_Flags0((unsigned char)x), m_Flags1((unsigned char)(x >> 8)) {}
            unsigned short GetFlags12() const noexcept { return ((unsigned short)m_Flags0) | (((unsigned short)m_Flags1) << 8); }
            void SetFlags12(unsigned short x) noexcept { m_Flags0 = (unsigned char)x; m_Flags1 = (unsigned char)(x >> 8); }
        };
#ifdef TOOLSLIB_SINGLE_THREAD
        _f12 f12, a12;
        mutable unsigned short m_Flags12, m_AFlags12;
#else
        _f12 f12;
        _a12 a12;
        mutable unsigned short m_Flags12;
        mutable std::atomic<unsigned short> m_AFlags12;
#endif
        constexpr _f2() noexcept {}
        constexpr _f2(unsigned short x) noexcept : f12(x) {}
        unsigned short GetFlags12() const noexcept { return m_Flags12; /*f12.GetFlags12();*/ }
        void SetFlags12(unsigned short x) noexcept { m_Flags12 = x; /*f12.SetFlags12(x);*/ }

/*
#ifdef TOOLSLIB_SINGLE_THREAD
        unsigned short& operator() () noexcept { return m_Flags12; }
        const unsigned short& operator() () const noexcept { return m_Flags12; }
        unsigned char& f0() noexcept { return f12.m_Flags0; }
        unsigned char& f1() noexcept { return f12.m_Flags1; }
        const unsigned char& f0() const noexcept { return f12.m_Flags0; }
        const unsigned char& f1() const noexcept { return f12.m_Flags1; }
#else
        std::atomic<unsigned short>& operator() () noexcept { return (std::atomic<unsigned short>&)m_Flags12; }
        const std::atomic<unsigned short>& operator() () const noexcept { return (const std::atomic<unsigned short>&)(std::atomic<unsigned short>&)m_Flags12; }
        std::atomic<unsigned char>& f0() noexcept { return (std::atomic<unsigned char>&)f12.m_Flags0; }
        std::atomic<unsigned char>& f1() noexcept { return (std::atomic<unsigned char>&)f12.m_Flags1; }
        const std::atomic<unsigned char>& f0() const noexcept { return (const std::atomic<unsigned char>&)(std::atomic<unsigned char>&)f12.m_Flags0; }
        const std::atomic<unsigned char>& f1() const noexcept { return (const std::atomic<unsigned char>&)(std::atomic<unsigned char>&)f12.m_Flags1; }
#endif
*/
    } f2;


    mutable unsigned short m_Padding;
#if !((TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0))
    //#ifdef CMAASTRING_CACHE_HASH
    mutable _dword m_Hash; // uninitialized if (!(m_Flag & 2))
    //#endif
#endif
    enum
    {
        //eDefault    =     0,
        eDontDelete = CMAASTRFL_eDontDelete,     // 0x0001
        eHashExists = CMAASTRFL_eHashExists,     // 0x0002
        eHash64Exists = CMAASTRFL_eHash64Exists, // 0x0004
        eHasReffered = CMAASTRFL_eHasReffered,   // 0x0008
        eROString = CMAASTRFL_eROString,         // 0x0010
        eImp32 = CMAASTRFL_eImp32,               // 0x0020
        eImp64 = CMAASTRFL_eImp64,               // 0x0040
        eImpMask = eImp32 | eImp64,
        eImpType =
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        eImp32
#else
        eImp64
#endif
        ,
        eMemoryString = CMAASTRFL_eMemoryString, // 0x0080
        eLengthLimit1 = CMAASTRFL_eLengthLimit1, // 0x0100
        eLengthLimit2 = CMAASTRFL_eLengthLimit2, // 0x0200
        eLengthLimit4 = CMAASTRFL_eLengthLimit4, // 0x0400
        eLengthLimit = eLengthLimit1 | eLengthLimit2 | eLengthLimit4,
        eCryptoKey_ = CMAASTRFL_eCryptoKey_,     // 0x0800
        eHasAddData = CMAASTRFL_eHasAddData,     // 0 / 0x1000
        eHasAddDataFreeBit__ = CMAASTRFL_eHasAddDataFreeBit__, // 0x1000 / 0
        eUtf16Flag_ = CMAASTRFL_eUtf16Flag,      // 0 / 0x2000 // CMaaString::eUtf16Flag
        eUtf32Flag_ = CMAASTRFL_eUtf32Flag,      // 0 / 0x4000 // CMaaString::eUtf32Flag
        eUtf1632Flag_FreeBits__ = CMAASTRFL_eUtf1632Flag_FreeBits__, // 0 / 0x2000 | 0x4000,
        e1632All_ = eUtf16Flag_ | eUtf32Flag_ | eCryptoKey_,
        eHasAddRefData = CMAASTRFL_eHasAddRefData, // 0x1000 / 0
        eData0Eq = CMAASTRFL_eHasAddRefData0Eq,    // 0x2000 / 0
        eData1 = CMAASTRFL_eHasAddRefData1,        // 0x4000 / 0
        ePaddingMask = CMAASTRFL_e_FreeBits__ | CMAASTRFL_eHash64_FreeBit__ | eUtf1632Flag_FreeBits__ | eHasAddDataFreeBit__
        //---eOftenCompared = ___
    };

    mutable int m_iCapacity; // max length, valid iof !eDontDelete && m_pszStr
    mutable ceCMaaStringImp* m_pRefferedTo; // , * m_pExcessiveUnallocBuffer;
#ifdef CMAASTRING_CACHE_HASH64
    mutable _uqword m_Hash64; // uninitialized if (!(m_Flag & 0x0004))
#define CMAASTRING_CACHE_HASH64_SIZE sizeof(_uqword)
#else
#define CMAASTRING_CACHE_HASH64_SIZE 0
#endif

#ifdef CMAASTRING_HAS_ADD_DATA
#if defined(_WIN64) || defined(__unix__)
    mutable _dword m_Padding4;
#endif
    mutable int_   m_iDataLength;// = 0;  // length of WCHAR m_pszData string // eHasAddData
    mutable char* m_pszData;// = nullptr; // eHasAddData
#endif

#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
    mutable ceCMaaStringImp* m_pAddRefDataPtr;
#endif

#ifdef CMaaStringImp_NOT_NULL_MODE
    enum _eNotInitImp
    {
        eNotInitImp
    };

    constexpr ceCMaaStringImp(_eNotInitImp) noexcept
    :
#if MANUAL_STRINGS_REFS
        CMaaManualReference(CMaaReferenceBase + 1), // to false of IsSingleOwner()
#else
        CMaaReference(CMaaReferenceBase + 1),
#endif
        //m_lRefs(CMaaReferenceBase + 1),
#if defined(_WIN64)
        m_Padding0{ 0 },
#endif
#ifndef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif

        m_iLength{ 0 },
#ifdef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif
        m_pszStr{ (char*)sChar0_1 },
        //m_pszStr{ nullptr },
        f2{ eDontDelete | eHashExists | eHash64Exists | eImpType /*| eROString*/ },
        m_Padding{ 0 },
#if !((TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0))
        m_Hash{ 0 },
#endif
        m_iCapacity{ 0 },
        m_pRefferedTo{ nullptr }
#ifdef CMAASTRING_CACHE_HASH64
        , m_Hash64{ 0 }
#endif
#ifdef CMAASTRING_HAS_ADD_DATA
        ,
#if defined(_WIN64) || defined(__unix__)
        m_Padding4{ 0 },
#endif
        m_iDataLength{ 0 },
        m_pszData{ nullptr }
#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
        , m_pAddRefDataPtr{ nullptr }
#endif
    {
    }
#endif
    constexpr ceCMaaStringImp() noexcept
    //:   m_lRefs(CMaaReferenceBase)
    {
    }
    constexpr ceCMaaStringImp(const char* txt, int len, int Flags /*= CMAASTRFL_eROString | 0*/ /* | eMemString0*/ /*eROMemString0*/) noexcept
    :
#if MANUAL_STRINGS_REFS
        CMaaManualReference(CMaaReferenceBase + 1), // to false of IsSingleOwner()
#else
        CMaaReference(CMaaReferenceBase + 1),
#endif
        //m_lRefs(CMaaReferenceBase + 1),
#if defined(_WIN64)
        m_Padding0{ 0 },
#endif
#ifndef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif

        m_iLength{ len },
#ifdef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif
        m_pszStr{ (char*)txt },
        //m_pszStr{ nullptr },
        f2{ (unsigned short)(eDontDelete | eMemoryString | eImpType | ((eROString | e1632All_) & (int_)Flags) | (eLengthLimit & ~(int_)Flags)) },
        m_Padding{ 0 },
#if !((TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0))
        m_Hash{ 0 },
#endif
        m_iCapacity{ 0 },
        m_pRefferedTo{ nullptr }
#ifdef CMAASTRING_CACHE_HASH64
        , m_Hash64{ 0 }
#endif
#ifdef CMAASTRING_HAS_ADD_DATA
        ,
#if defined(_WIN64) || defined(__unix__)
        m_Padding4{ 0 },
#endif
        m_iDataLength{ 0 },
        m_pszData{ nullptr }
#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
        , m_pAddRefDataPtr{ nullptr }
#endif
    {
    }
    template<int N> constexpr ceCMaaStringImp(const char(&txt)[N]) noexcept
    :
#if MANUAL_STRINGS_REFS
        CMaaManualReference(CMaaReferenceBase + 1), // to false of IsSingleOwner()
#else
        CMaaReference(CMaaReferenceBase + 1),
#endif
        //m_lRefs(CMaaReferenceBase + 1),
#if defined(_WIN64)
        m_Padding0{ 0 },
#endif
#ifndef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif

        m_iLength{ GET_CHECK_TXT_N1_N(txt, N) },
#ifdef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif
        m_pszStr{ (char*)txt },
        //m_pszStr{ nullptr },
        f2{ (unsigned short)(eDontDelete | eMemoryString | eImpType | eROString | (e1632All_ & (int_)CMAASTRFL_eLengthLimit1) | (eLengthLimit & ~(int_)CMAASTRFL_eLengthLimit1)) },
        m_Padding{ 0 },
#if !((TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0))
        m_Hash{ 0 },
#endif
        m_iCapacity{ 0 },
        m_pRefferedTo{ nullptr }
#ifdef CMAASTRING_CACHE_HASH64
        , m_Hash64{ 0 }
#endif
#ifdef CMAASTRING_HAS_ADD_DATA
        ,
#if defined(_WIN64) || defined(__unix__)
        m_Padding4{ 0 },
#endif
        m_iDataLength{ 0 },
        m_pszData{ nullptr }
#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
        , m_pAddRefDataPtr{ nullptr }
#endif
    {
    }
#ifdef CMAASTRING_USE_UNALLOC_BUFFER000000000
    template<int N> constexpr ceCMaaStringImp(const char(&txt)[N], bool) noexcept
    :
#if MANUAL_STRINGS_REFS
        CMaaManualReference(CMaaReferenceBase + 1), // to false of IsSingleOwner()
#else
        CMaaReference(CMaaReferenceBase + 1),
#endif
        //m_lRefs(CMaaReferenceBase + 1),
#if defined(_WIN64)
        m_Padding0{ 0 },
#endif
#ifndef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif

        m_iLength{ GET_CHECK_TXT_N1_N(txt, N) },
#ifdef TOOLSLIB_LITTLE_ENDIAN
#if (TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0)
        m_Hash{ 0 },
#endif
#endif
        m_pszStr{ (char*)&m_iCapacity }, // { (char*)txt },
        //m_pszStr{ nullptr },
        //f2{ (unsigned short)(eDontDelete | eMemoryString | eImpType | ((eROString | e1632All_))) },// &(int_)Flags) | (eLengthLimit & ~(int_)Flags)) },
        f2{ (unsigned short)(eDontDelete | eImpType) },
        m_Padding{ 0 },
#if !((TOOLSLIB_USE_CMAASTRING64 == 2 && TOOLSLIB_CMAASTRING64 == 0) || (TOOLSLIB_USE_CMAASTRING64 == 0))
        m_Hash{ 0 },
#endif
        m_iCapacity{ 0 },
        m_pRefferedTo{ nullptr }
#ifdef CMAASTRING_CACHE_HASH64
        , m_Hash64{ 0 }
#endif
#ifdef CMAASTRING_HAS_ADD_DATA
        ,
#if defined(_WIN64) || defined(__unix__)
        m_Padding4{ 0 },
#endif
        m_iDataLength{ 0 },
        m_pszData{ nullptr }
#endif
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
        , m_pAddRefDataPtr{ nullptr }
#endif
    {
        //if (m_iLength <= CMAASTRING_MAX_UNALLOC_LENGTH)
        m_pszStr = (char*)&m_iCapacity; // CMAASTRING_UNALLOC_PTR;
        //ZeroUnallocTail0;
        //memcpy(m_pszStr, pszStr, m_iLength);
        for (int i = 0; i < m_iLength; i++)
        {
            m_pszStr[i] = txt[i];
        }
        //f2.m_Flags12 = eDontDelete | eImpType;
        //INC_TOOLSLIB_STAT_STRING_IMP_SHORT;
    }
#endif
    //constexpr ~ceCMaaStringImp() noexcept {}
};
//------------------------------------------------------------------------------
struct CMaaTmpSprintfBuffer;
struct CMaaTmpSprintf2StringsArray;
//------------------------------------------------------------------------------
class CMaaString
{
public:
    class Helper;
#if TOOLSLIB_CMAASTRING64 == 0
    static constexpr size_t sMaxStringLen = (((_dword)-1) >> 1) - 8;
#else
    static constexpr size_t sMaxStringLen = (((_uqword)-1) >> 1) - 8;
#endif

private:
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
    friend class ::CMaaString64;
#else
#undef CMaaString
    friend class ::CMaaString;
#define CMaaString CMaaString64
#endif
#endif
    static const char /*sChar0_1[8],*/ sChar0_2[8], sChar0_3[10];
    static CMaaString s_szSpaces;

public:
    static void GetRetSprintfBuffers(int a, CMaaTmpSprintfBuffer*& pTmpBuffer, CMaaTmpSprintfBuffer*& pTmpFormatBuffer) noexcept;
    static void GetRetSprintf2Buffers(int a, CMaaTmpSprintfBuffer*& pTmpBuffer, CMaaTmpSprintfBuffer*& pTmpFormatBuffer, CMaaTmpSprintf2StringsArray*& pStringsArray) noexcept;
    enum _e1632
    {
        eUtf8Flag = 0,
        eUtf16Flag = CMAASTRFL_eUtf16Flag, // 0x200 / 0,
        eUtf32Flag = CMAASTRFL_eUtf32Flag, // 0x400 / 0,
        eCryptoKey = CMAASTRFL_eCryptoKey_, // 0x4000,
        eUtf8CryptoKey = eUtf8Flag | eCryptoKey,
        eUtf16CryptoKey = eUtf16Flag | eCryptoKey,
        eUtf32CryptoKey = eUtf32Flag | eCryptoKey,
        e1632All = eUtf8Flag | eUtf16Flag | eUtf32Flag | eCryptoKey
    };

    enum _e1_
    {
        eNotInitMem = 0, // == -1
        eNotInitMem_16 = CMAASTRFL_eUtf16Flag, // 0x200 / 0,
        eNotInitMem_32 = CMAASTRFL_eUtf32Flag, // 0x400 / 0,
        eNotInitMemKey = eNotInitMem | CMAASTRFL_eCryptoKey_,
        eNotInitMem_16Key = eNotInitMem_16 | CMAASTRFL_eCryptoKey_,
        eNotInitMem_32Key = eNotInitMem_32 | CMAASTRFL_eCryptoKey_

        //e1_ = eNotInitMem
    };
    enum _e0
    {
        // eMemXXXX - eLengthLimitX flags are inversed

        eMemString = 0, //= 0,

        // for
        //eROString     =    8,
        //eLengthLimit1 = 0x10,
        //eLengthLimit2 = 0x20,
        //eLengthLimit4 = 0x40,
        // :

        eMemString0 = CMAASTRFL_eLengthLimit1,                                                                   // 0x10,
        eMemString00 = CMAASTRFL_eLengthLimit2 | CMAASTRFL_eLengthLimit1,                                  // 0x20 | 0x10,
        eMemString0000 = CMAASTRFL_eLengthLimit4 | CMAASTRFL_eLengthLimit2 | CMAASTRFL_eLengthLimit1, // 0x40 | 0x20 | 0x10,

        eROMemString = CMAASTRFL_eROString,                  // 8 |                  0,
        eROMemString0 = CMAASTRFL_eROString | eMemString0,    // 8 |               0x10,
        eROMemString00 = CMAASTRFL_eROString | eMemString00,   // 8 |        0x20 | 0x10,
        eROMemString0000 = CMAASTRFL_eROString | eMemString0000, // 8 | 0x40 | 0x20 | 0x10,

        eMemString_16 = CMAASTRFL_eUtf16Flag, // 0x200 / 0,
        eMemString_32 = CMAASTRFL_eUtf32Flag, // 0x400 / 0,
        eROMemString0000_16 = eROMemString0000 | CMAASTRFL_eUtf16Flag,
        eROMemString0000_32 = eROMemString0000 | CMAASTRFL_eUtf32Flag

        //e0 = eMemString
    };
    enum _e1
    {
        eStrlenMemString //= 0,
        //eROStrlenMemString = 8,
        //e1 = eStrlenMemString
    };
    enum _e2
    {
        //eStrlenMemString = 0,
        eROStrlenMemString //= 8,
    };

    struct CE
    {
        ceCMaaStringImp
#ifdef CMaaStringImp_NOT_NULL_MODE
            snZ,
#endif
            sn0,
            sn00,
            snWC0,
            snWChar0,
            snWC32_0,
            snEta, snSp,
            sn_a, sn_x, snLf, snCr, snCrLf,
            snSpSp, // sn_a, sn_z, sn_A, sn_Z
            snSemicolon, snComma, snDot, snDotDot, // snEllipse{ "..." },
            snDoc, snStr2HtmlValEsc;
        // another sample: // snJsonFloat{ "float", 5, eROMemString0 };

        struct S
        {
            const ceCMaaStringImp* m_pImp;
            constexpr S() noexcept : m_pImp(nullptr) {}
            constexpr S(const ceCMaaStringImp* p) noexcept : m_pImp(p) {}
            constexpr S & operator = (const ceCMaaStringImp* p) noexcept { m_pImp = p; return *this; }
            //constexpr S & operator = (const S &That) noexcept { m_pImp = That.m_pImp; return *this; }
            //constexpr ~S() {}
        };

        S sZ;
        S s0, s00, sWC0, sWChar0, sWC32_0;

        S sEta, sSp,
            s_a, s_x,
            sLf, sCr, sCrLf, sSpSp,
            // s_a, s_z, s_A, s_Z,
            sSemicolon, sComma, sDot, sDotDot, // sEllipse { &snEllipse },
            sDoc, sStr2HtmlValEsc;
       
        constexpr CE() noexcept;
    };
    struct CE2 // rarely used objects
    {
        ceCMaaStringImp
            snJsonVal, snJsonNode, snJsonArray, snJsonName, snJsonValue, snJsonType,
            snJsonInt, snJsonBool, snJsonFloat, snJsonNull, snJsonString;
        CMaaString::CE::S
            sJsonVal, sJsonNode, sJsonArray, sJsonName,
            sJsonValue, sJsonType,
            sJsonInt, sJsonBool, sJsonFloat, sJsonNull, sJsonString;

        constexpr CE2() noexcept;
    };
    struct CE3 // rarely used objects
    {
        ceCMaaStringImp sn_Nums[10];
        CMaaString::CE::S s_Nums[10];

        constexpr CE3() noexcept;
    };
    static const CE& C() noexcept;
    static const CE2& C2() noexcept;
    static const CE3& C3() noexcept;

private:
public:
    struct CMaaStringImp : public ceCMaaStringImp
    {
        friend class CMaaString;
    private:
        CMaaStringImp(const char* pszStr) noexcept(noexcept_new);
        CMaaStringImp(size_t Length) = delete;
        CMaaStringImp(size_t Length, _e1_ Flags, int AddCapacity = 0) noexcept(noexcept_new); // NotZero
        CMaaStringImp(const void* pszStr, size_t Length, _e1632 Flags = eUtf8Flag) noexcept(noexcept_new);
        CMaaStringImp(char* pStr, bool/*const void* pszStr*/, int Length, _e1632 Flags = eUtf8Flag) noexcept;
        CMaaStringImp(const void* pszStr, size_t Length, int_ Flags) = delete; // need "int constexpr Flags" parameter
        CMaaStringImp(const void* pszStr, size_t Length, _e0 Flags) noexcept;
        CMaaStringImp(const CMaaConstStr &s, _e0 Flags) noexcept;
        //CMaaStringImp(const void* pszStr, size_t Length, _e1 Flags) noexcept;
        CMaaStringImp(CMaaStringImp* pRefferedTo, size_t Start, size_t Length) noexcept;
        void DerefReffered() noexcept;
#ifdef CMAASTRING_HAS_ADD_REF_DATA_PTR
        void DerefAddRefDataPtr() noexcept;
#endif
        ~CMaaStringImp(); // noexcept;

        //void operator=(CMaaStringImp &&That) noexcept;
        //void re_new(const char* pszTxt, size_t Len) noexcept;
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
        CMaaStringImp(const CMaaString64& That) noexcept;
#else
        CMaaStringImp(const CMaaString32& That) noexcept;
#endif
#endif
    public:
        static int GetMaxUnAllocLength() noexcept;
        static int GetMaxUnAllocLengthWithPadding() noexcept;
        constexpr int Length() const noexcept { return m_iLength; }
        constexpr operator const char* () const noexcept { return m_pszStr; }
        constexpr operator char* () noexcept { return m_pszStr; }
        constexpr const char* const_ptr() const noexcept { return m_pszStr; }
        constexpr char* ptr() noexcept { return m_pszStr; }
        bool IsSingleOwner() const noexcept
        {
            return
                //                IsSingleOwner__()
#if MANUAL_STRINGS_REFS
                CMaaManualReference::IsSingleOwner()
#else
                CMaaReference::IsSingleOwner()
#endif
                && !(f2.f12.m_Flags0 & eHasReffered);
        }
        bool IsRWSingleOwner() const noexcept
        {
            return
                //                IsSingleOwner__()
#if MANUAL_STRINGS_REFS
                CMaaManualReference::IsSingleOwner()
#else
                CMaaReference::IsSingleOwner()
#endif
                && !(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags0) & (eHasReffered | eROString));
        }
        bool HasReffered() const noexcept
        {
            return (f2.f12.m_Flags0 & eHasReffered);
        }
        bool HasAddRefData0() const noexcept
        {
#ifdef CMAASTRING_HAS_ADD_REF_DATA
            const unsigned char m = TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1);
            if ((m & (eData0Eq >> 8)))
            {
                return true;
            }
            return (m & ((eHasAddRefData | eData1) >> 8)) == (eHasAddRefData >> 8);
#else
            return false;
#endif
        }
        bool HasAddRefData1() const noexcept
        {
#ifdef CMAASTRING_HAS_ADD_REF_DATA
            return (TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & ((eHasAddRefData | eData1) >> 8)) == ((eHasAddRefData | eData1) >> 8);
#else
            return false;
#endif
        }
        void ClearHash() noexcept; // call after direct string modifications
        bool Is0Behind() const noexcept
        {
            return !(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eLengthLimit1 >> 8)) && m_pszStr[m_iLength] == '\0';
        }
        bool IsWC0Behind() const noexcept
        {
            return !(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eLengthLimit2 >> 8)) && *((m_iLength + (int_)sizeof(_WC_) - 1) / (int_)sizeof(_WC_) + (const _WC_*)m_pszStr) == 0;
        }
        bool IsWChar0Behind() const noexcept
        {
            if constexpr (sizeof(wchar_t) == 2)
            {
                return !(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eLengthLimit2 >> 8)) && *((m_iLength + (int_)sizeof(wchar_t) - 1) / (int_)sizeof(wchar_t) + (const wchar_t*)m_pszStr) == 0;
            }
            else
            {
                return !(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eLengthLimit4 >> 8)) && *((m_iLength + (int_)sizeof(wchar_t) - 1) / (int_)sizeof(wchar_t) + (const wchar_t*)m_pszStr) == 0;
            }
        }
        bool IsWC32_0Behind() const noexcept
        {
            return !(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags1) & (eLengthLimit4 >> 8)) && *((m_iLength + (int_)sizeof(char32_t) - 1) / (int_)sizeof(char32_t) + (const char32_t*)m_pszStr) == 0;
        }
        bool IsMemoryString() const noexcept
        {
            return (TOOLSLIB_LOAD_ATOMIC((f2.f12.m_Flags0 & eHasReffered) ? m_pRefferedTo->f2.a12.m_Flags0 : f2.a12.m_Flags0) & eMemoryString);
        }
        bool IsROString() const noexcept
        {
            return (TOOLSLIB_LOAD_ATOMIC((f2.f12.m_Flags0 & eHasReffered) ? m_pRefferedTo->f2.a12.m_Flags0 : f2.a12.m_Flags0) & eROString);
        }
        bool IsMemoryNotROString() const noexcept // is memory, but not const string
        {
            return (TOOLSLIB_LOAD_ATOMIC((f2.f12.m_Flags0 & eHasReffered) ? m_pRefferedTo->f2.a12.m_Flags0 : f2.a12.m_Flags0) & (eMemoryString | eROString)) == eMemoryString;
        }
        bool IsUtf16() const noexcept
        {
#ifdef CMAASTRING_HAS_UTF1632_INFO
            return (((f2.f12.m_Flags0 & eHasReffered) ? m_pRefferedTo->f2.f12.m_Flags1 : f2.f12.m_Flags1) & ((eUtf16Flag | eUtf32Flag) >> 8)) == (eUtf16Flag >> 8);
#else
            return false;
#endif
        }
        bool IsUtf32() const noexcept
        {
#ifdef CMAASTRING_HAS_UTF1632_INFO
            return (((f2.f12.m_Flags0 & eHasReffered) ? m_pRefferedTo->f2.f12.m_Flags1 : f2.f12.m_Flags1) & ((eUtf16Flag | eUtf32Flag)) >> 8) == (eUtf32Flag >> 8);
#else
            return false;
#endif
        }
        _e1632 GetUtf1632Flags() const noexcept
        {
            //#ifdef CMAASTRING_HAS_UTF1632_INFO
            return (_e1632)((TOOLSLIB_LOAD_ATOMIC((f2.f12.m_Flags0 & eHasReffered) ? m_pRefferedTo->f2.a12.m_Flags1 : f2.a12.m_Flags1) << 8) & e1632All);
            //#else
            //            return eUtf8Flag;
            //#endif
        }
        bool SetUtf16Flag(bool bSet = true) noexcept
        {
#ifdef CMAASTRING_HAS_UTF1632_INFO
            if ((f2.f12.m_Flags0 & eHasReffered))
            {
                return IsUtf16() == bSet;
            }
            if (bSet)
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~(eUtf32Flag >> 8));
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eUtf16Flag >> 8));
            }
            else
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~(eUtf16Flag >> 8));
            }
            return true;
#else
            return !bSet;
#endif
        }
        bool SetUtf32Flag(bool bSet = true) noexcept
        {
#ifdef CMAASTRING_HAS_UTF1632_INFO
            if ((f2.f12.m_Flags0 & eHasReffered))
            {
                return IsUtf32() == bSet;
            }
            if (bSet)
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &= ~(eUtf16Flag >> 8));
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eUtf32Flag >> 8));
            }
            else
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~(eUtf32Flag >> 8));
            }
            return true;
#else
            return !bSet;
#endif
        }
        bool SetUtf1632Flags(_e1632 Flags = eUtf8Flag) noexcept
        {
#ifdef CMAASTRING_HAS_UTF1632_INFO
            if ((f2.f12.m_Flags0 & eHasReffered))
            {
                return GetUtf1632Flags() == Flags;
            }
            if (Flags == eUtf8Flag)
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~((eUtf16Flag | eUtf32Flag) >> 8));
            }
            else if ((Flags & eUtf16Flag))
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~(eUtf32Flag >> 8));
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eUtf16Flag >> 8));
            }
            else
            {
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_and, &=, ~(eUtf16Flag >> 8));
                TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eUtf32Flag >> 8));
            }
            return true;
#else
            return Flags == eUtf8Flag;
#endif
        }
    private:
        bool SetAddRefData(CMaaStringImp* pImp, int Idx = 0, bool bNeedSingleOwner = 
#ifdef TOOLSLIB_SINGLE_THREAD
            false
#else
            true
#endif
            , int iLock = -1) noexcept; // iLock == -1 == no lock on bNeedSingleOwner == true; iLock == 0 == external lock

#ifdef CMAASTRING_HAS_ADD_DATA
        const char* SetAdditionalData(const char* ptr, int_ length) noexcept(noexcept_new);
        const char* GetAdditionalData(int_* pchsize = nullptr) const noexcept;
        int_ GetAdditionalLength() const noexcept;
#else
        int_ GetAdditionalLength() const noexcept { return 0; }
#endif
    public:
        bool IsUnAllocString() const noexcept; // is unallocated (short) string
        bool CanSetAddRefData(CMaaStringImp* pImp, int Idx, bool bNeedSingleOwner = 
#ifdef TOOLSLIB_SINGLE_THREAD
            false
#else
            true
#endif
        ) const noexcept;
        void SetCryptoKeyBit() noexcept
        {
            TOOLSLIB_ATOMIC_OP_EQ(f2.a12.m_Flags1, fetch_or, |=, (eCryptoKey_ >> 8));
            if ((f2.f12.m_Flags0 & eHasReffered))
            {
                TOOLSLIB_ATOMIC_OP_EQ(m_pRefferedTo->f2.a12.m_Flags1, fetch_or, |=, (eCryptoKey_ >> 8));
            }
        }
        int_ GetImpVersion() const noexcept // 0 - empty, CMAASTRFL_eImp32 - imp32, CMAASTRFL_eImp64 - imp64, -1 - error
        {
            //if (!this)
            //{
            //    return 0;
            //}
            const int_ Flag = f2.GetFlags12() & (eImp32 | eImp64 | ePaddingMask);
            return (Flag == CMaaStringImp::eImp32 || Flag == CMaaStringImp::eImp64) ? Flag : -1;
        }
        bool operator== (const CMaaStringImp& That) const noexcept
        {
            // 0% // return m_iLength == That.m_iLength && CMaaIsMemEq(m_pszStr, That.m_pszStr, m_iLength);
            return 
#ifdef CMaaStringImp_NOT_NULL_MODE
                this == &That ||
#endif
                m_iLength == That.m_iLength && (!(TOOLSLIB_LOAD_ATOMIC(f2.a12.m_Flags0) & eHashExists) || !(TOOLSLIB_LOAD_ATOMIC(That.f2.a12.m_Flags0) & eHashExists) || m_Hash == That.m_Hash) && CMaaIsMemEq(m_pszStr, That.m_pszStr, m_iLength);
            
            // -3% // return m_iLength == That.m_iLength && (m_pszStr == That.m_pszStr || ((!(f2.f12.m_Flags0 & That.f2.f12.m_Flags0 & eHashExists) || m_Hash == That.m_Hash) && CMaaIsMemEq(m_pszStr, That.m_pszStr, m_iLength)));
        }

        int Find(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1) const noexcept;
        int Find2(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len = -1, int SubStr2Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
        int Find3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len = -1, int SubStr2Len = -1, int SubStr3Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
        int FindCi(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1, int_ cp = 0) const noexcept; // cp: -1, 0, 1251
        int Find2Ci(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len = -1, int SubStr2Len = -1, int_* pIndex = nullptr, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is Length() - the position after the end of source string
        int Find3Ci(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len = -1, int SubStr2Len = -1, int SubStr3Len = -1, int_* pIndex = nullptr, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is Length() - the position after the end of source string
        int Find(int StartPos, char c, int EndPos = -1) const noexcept;
        // Find c1 or c2
        int Find2(int StartPos, char c1, char c2, int EndPos = -1) const noexcept;
        // Find c1 or c2 or c3
        int Find3(int StartPos, char c1, char c2, char c3, int EndPos = -1) const noexcept;
        int Find(int StartPos, const CMaa256Bits& Bits, int EndPos = -1) const noexcept;
        int Count(int StartPos, char c0, char c1, int EndPos = -1) const noexcept;
        int Count(int StartPos, unsigned char c0, unsigned char c1, int EndPos = -1) const noexcept;
        int Count(int StartPos, const CMaa256Bits& Bits, int EndPos = -1) const noexcept;
        int CountNum() const noexcept;
        int ReverseFind(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1) const noexcept; // Typically EndPos is a -1 - the position before start of source string
        int ReverseFind(int StartPos, char c, int EndPos = -1) const noexcept;
        int ReverseFind2(int StartPos, char c1, char c2, int EndPos = -1) const noexcept;
        int ReverseFind3(int StartPos, char c1, char c2, char c3, int EndPos = -1) const noexcept;
        int ReverseFind(int StartPos, const CMaa256Bits& Bits, int EndPos = -1) const noexcept;
        int ReverseFindCi(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is a -1 - the position before start of source string
        int ReverseFindCi(int StartPos, char c, int EndPos = -1, int_ cp = 0) const noexcept;

        ADD_ALLOCATOR(CMaaStringImp)
    };

protected:
    CMaaStringImp* m_pImp;
    //static CMaaStringImp *sp_NullImp;
    //static constexpr ceCMaaStringImp sNullImp{ ceCMaaStringImp::eNotInitImp };

public:

    // return text representation of ptr memory of len bytes
    static CMaaString DataToString(const void* ptr, int len);

    //CMaaString() noexcept;
    constexpr_ CMaaString() noexcept : m_pImp(sp_NullImp) {}
protected:
public:
    CMaaString(CMaaStringImp* pImp, bool bInc, bool) noexcept;
public:
    //constexpr CMaaString(ceCMaaStringImp *p) noexcept : m_pImp((CMaaStringImp*)p) {}

#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
    CMaaString(const CMaaString64& That) noexcept;
#else
    CMaaString(const CMaaString32& That) noexcept;
#endif
#endif

#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template<std::size_t N> CMaaString(const char(&txt)[N]) noexcept
    {
        try
        {
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
            m_pImp = GET_CHECK_TXT_N1_N(txt, N) ? TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0) : nullptr;
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
#else
            m_pImp = GET_CHECK_TXT_N1_N(txt, N) ? TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0) : nullptr;
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            return;
#endif
        }
        catch (...)
        {
        }
        m_pImp = sp_NullImp;
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    template<std::size_t N> CMaaString(const char8_t(&txt)[N]) noexcept
    {
        try
        {
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
            m_pImp = GET_CHECK_TXT_N1_N(txt, N) ? TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0) : nullptr;
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
#else
            m_pImp = GET_CHECK_TXT_N1_N(txt, N) ? TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0) : nullptr;
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            return;
#endif
        }
        catch (...)
        {
        }
        m_pImp = sp_NullImp;
    }
    CMaaString(char8_t* pszStr) noexcept(noexcept_new);
#endif
    CMaaString(const void* pszStr) noexcept(noexcept_new);
    CMaaString(char* pszStr) noexcept(noexcept_new);
#else
    CMaaString(const char* pszStr) noexcept(noexcept_new);
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString(const char8_t* pszStr) noexcept(noexcept_new);
#endif
#endif

    // samples:
    // CMaaConstStr const_strs[] = { "abc", "def", "ghij" };
    // CMaaString txt_ = CMaaConstStr("abcd");
    // txt_ = const_strs[1];
    CMaaString(const CMaaConstStr &s) noexcept
    {
        if (s.len)
        {
            try
            {
                m_pImp = TL_NEW CMaaStringImp(s, eROMemString0);
                if (!m_pImp)
                {
#ifdef CMaaStringImp_NOT_NULL_MODE
                    m_pImp = sp_NullImp;
#endif
                    return;
                }
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
                if (m_pImp->m_iLength)
#endif
                {
                    INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                    return;
                }
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
                delete m_pImp;
#endif
            }
            catch (...)
            {
            }
        }
        m_pImp = sp_NullImp;
    }

protected:
    // copy of pMem or Len bytes zeroes if !pMem
    void ctor(const void* pMem, size_t Len) noexcept(noexcept_new);
public:
    // copy of pMem or Len bytes zeroes if !pMem
    CMaaString(const void* pMem, size_t Len) noexcept(noexcept_new) { ctor(pMem, Len); }
#if TOOLSLIB_CMAASTRING64 <= 0
    CMaaString(const void* pMem, size_t Len, _e1632 Flags) noexcept(noexcept_new);  // CMaaString a10("aaaa", 4, CMaaString::eUtf8Flag); // - error  if TOOLSLIB_USE_CMAASTRING64 == 1
#endif
    CMaaString(const void* pMem, int Len, _e1632 Flags) noexcept(noexcept_new);

    // CMaaString(nullptr, Len, -1) allocates Len bytes non-initialized memory, or direct memory referenced string if ptr != nullptr Len bytes
    // CMaaString(ptr, >0, 1) - CMaaString(ptr, 1, 1) creates direct memory referenced string strlen(ptr) size, noexcept: m_pImp=nullptr on New error
    // CMaaString(ptr, Len>0, (Flags!=-1||ptr) && Flags!=1) - CMaaString(ptr, Len, 0) creates direct memory referenced string Len size, noexcept: m_pImp=nullptr on New error
#if TOOLSLIB_CMAASTRING64 <= 0
    CMaaString(const void* pMem, size_t Len, int_ Flags /**/) = delete; // need "int constexpr Flags" parameter
#endif

    // CMaaString(nullptr, Len, CMaaString::eNotInitMem) allocates Len bytes non-initialized memory, or (temp!) direct memory referenced string if ptr != nullptr Len bytes
    // CMaaString(ptr, Len>0, CMaaString::eMemString) - CMaaString(ptr, Len, CMaaString::eMemString) creates direct memory referenced string Len size, noexcept, m_pImp=nullptr on (New error || Len <= 0)
    // CMaaString(ptr, any, CMaaString::eStrlenMemString) - CMaaString(ptr, 1, CMaaString::eStrlenMemString) creates direct memory referenced string strlen(ptr) size, noexcept, m_pImp=nullptr on (New error || Len <= 0)
#if TOOLSLIB_CMAASTRING64 <= 0
    CMaaString(const void* pMem, size_t Len, _e1_ Flags /**/, int AddCapacity = 0) noexcept(noexcept_new);
    CMaaString(const void* pMem, size_t Len, _e0 Flags /**/) noexcept;
    CMaaString(const void* pMem, size_t Len, _e1 Flags /**/) noexcept = delete;
    CMaaString(const void* pMem, size_t Len, _e2 Flags /**/) noexcept = delete;
#endif
    CMaaString(const void* pMem, _e1 Flags /**/) noexcept;
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    //CMaaString(const CMaaConstStr &s, _e2 Flags /**/) noexcept;
    template<std::size_t N> CMaaString(const char(&txt)[N], _e2 /*Flags*/) noexcept
    {
        try
        {
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
            m_pImp = GET_CHECK_TXT_N1_N(txt, N) ? TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0) : nullptr;
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
#else
            m_pImp = GET_CHECK_TXT_N1_N(txt, N) ? TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0) : nullptr;
            if (!m_pImp)
            {
#ifdef CMaaStringImp_NOT_NULL_MODE
                m_pImp = sp_NullImp;
#endif
                return;
            }
            INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            return;
#endif
        }
        catch (...)
        {
        }
        m_pImp = sp_NullImp;
    }
    CMaaString(char* pMem, _e2 Flags /**/) noexcept;
#endif
    CMaaString(const void* pMem, _e2 Flags /**/) noexcept;

    // CMaaString(nullptr, Len, -1) allocates Len bytes non-initialized memory, or direct memory referenced string if ptr != nullptr Len bytes
    // CMaaString(ptr, >0, 1) - CMaaString(ptr, 1, 1) creates direct memory referenced string strlen(ptr) size, noexcept: m_pImp=nullptr on New error
    // CMaaString(ptr, Len>0, (Flags!=-1||ptr) && Flags!=1) - CMaaString(ptr, Len, 0) creates direct memory referenced string Len size, noexcept: m_pImp=nullptr on New error
#if TOOLSLIB_CMAASTRING64 <= 0
    CMaaString(const void* pMem, int_ Len, int_ Flags /**/) = delete; // need "int constexpr Flags" parameter
#endif

    // CMaaString(nullptr, Len, CMaaString::eNotInitMem) allocates Len bytes non-initialized memory, or (temp!) direct memory referenced string if ptr != nullptr Len bytes
    // CMaaString(ptr, Len>0, CMaaString::eMemString) - CMaaString(ptr, Len, CMaaString::eMemString) creates direct memory referenced string Len size, noexcept, m_pImp=nullptr on (New error || Len <= 0)
    // CMaaString(ptr, any, CMaaString::eStrlenMemString) - CMaaString(ptr, 1, CMaaString::eStrlenMemString) creates direct memory referenced string strlen(ptr) size, noexcept, m_pImp=nullptr on (New error || Len <= 0)
    CMaaString(const void* pMem, int Len, _e1_ Flags /**/, int AddCapacity = 0) noexcept(noexcept_new);
    CMaaString(const void* pMem, int Len, _e0 Flags /**/) noexcept;
    CMaaString(const void* pMem, int Len, _e1 Flags /**/) noexcept = delete;
    CMaaString(const void* pMem, int Len, _e2 Flags /**/) noexcept = delete;

#if TOOLSLIB_CMAASTRING64 <= 0
    CMaaString(const CMaaString& That, size_t Start, size_t Len) noexcept;
#endif
    CMaaString(const CMaaString& That, int Start, int Len) noexcept;
    CMaaString(const CMaaString& That) noexcept;
    CMaaString(CMaaString&& That) noexcept { m_pImp = That.m_pImp; That.m_pImp = sp_NullImp; }
    static bool CheckRuntimeConditions() noexcept { return sizeof(std::atomic<int>) == sizeof(int) && sizeof(std::atomic<unsigned short>) == 2 && sizeof(std::atomic<unsigned char>) == 1 && sizeof(wchar_t) <= 4 && sizeof(char32_t) <= 4; }
    CMaaString& operator=(const CMaaString& That) noexcept;
    CMaaString& operator=(CMaaString&& That) noexcept;
#if TOOLSLIB_USE_CMAASTRING64 == 2
#if TOOLSLIB_CMAASTRING64 == 0
    CMaaString& operator=(const CMaaString64& That) noexcept;
#else
    CMaaString& operator=(const CMaaString32& That) noexcept;
#endif
#endif
    CMaaString& operator=(const CMaaConstStr& s) noexcept;
    CMaaString& operator_eq(const void* pszTxt, int l) noexcept(noexcept_new);
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    CMaaString& operator=(char* pszTxt) noexcept(noexcept_new) { return operator_eq(pszTxt, pszTxt ? int_strlen(pszTxt) : 0); }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& operator=(char8_t* pszTxt) noexcept(noexcept_new) { return operator_eq(pszTxt, pszTxt ? int_strlen(pszTxt) : 0); }
#endif
    CMaaString& operator=(const void* pszTxt) noexcept(noexcept_new) { return operator_eq(pszTxt, pszTxt ? int_strlen((const char *)pszTxt) : 0); }
protected:
    CMaaString& operator_const_eq(const char *txt, size_t N) noexcept
    {
        //opt of "   CMaaString tmp(txt); return (*this = std::move(tmp));  "

        if (!GET_CHECK_TXT_N1_N(txt, N))
        {
            Empty();
        }
        else if (m_pImp != sp_NullImp && m_pImp->IsSingleOwner()) // just the optimization
        {
            m_pImp->~CMaaStringImp();
            ::new(m_pImp) CMaaString::CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0);
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
            if (!m_pImp->m_iLength)
            {
                DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
                delete m_pImp;
                m_pImp = sp_NullImp;
            }
#endif
        }
        else
        {
            try
            {
                CMaaStringImp* pImp = TL_NEW CMaaStringImp(txt, GET_CHECK_TXT_N1_N(txt, N), eROMemString0);
#ifndef TOOLSLIB_TXT_CONSTANTS_ARE_SHORT
                if (!pImp || !pImp->m_iLength)
                {
                    delete pImp;
                }
                else
#else
                if (pImp)
#endif
                {
                    INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
                    AssignImpFast(m_pImp, pImp, false, MANUAL_STRINGS_REFS);
                    return *this;
                }
            }
            catch (...)
            {
            }
            Empty();
        }
        return *this;
    }
public:
    template<std::size_t N> CMaaString& operator=(const char(&txt)[N]) noexcept
    {
        return operator_const_eq(txt, N);
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    template<std::size_t N> CMaaString& operator=(const char8_t(&txt)[N]) noexcept
    {
        return operator_const_eq((const char *)txt, N);
    }
#endif
#else
    CMaaString& operator=(const char* pszTxt) { return operator_eq(pszTxt, pszTxt ? int_strlen(pszTxt) : 0); }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& operator=(const char8_t* pszTxt) { return operator_eq(pszTxt, pszTxt ? int_strlen(pszTxt) : 0); }
#endif
#endif

    CMaaString& operator=(const CMaaString& That) const noexcept = delete;
    CMaaString& operator=(CMaaString&& That) const noexcept = delete;
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    CMaaString& operator=(char* pszTxt) const = delete;
#else
    CMaaString& operator=(const char* pszTxt) const = delete;
#endif

#ifdef TOOLSLIB_CHAR8T_SUPPORT
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    CMaaString& operator=(char8_t* pszTxt) const = delete;
#else
    CMaaString& operator=(const char8_t* pszTxt) const = delete;
#endif
#endif

    constexpr_ bool IsEmpty() const noexcept { return m_pImp == sp_NullImp; /*!Length();*/ }
    constexpr_ bool IsNotEmpty() const noexcept { return m_pImp != sp_NullImp; /*Length();*/ }

    // inline fn:
    // 
    constexpr_ ~CMaaString() // noexcept
    {
        //Release(m_pImp);
        CMaaStringImp* p = m_pImp;
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
    CMaaStringImp* Get_pImp() const noexcept
    {
        AddRef(m_pImp);
        return m_pImp;
    }
    static constexpr_ void AddRef(CMaaStringImp* p) noexcept
    {
        if (p != sp_NullImp)
        {
            p->AddRef();
        }
    }
    static constexpr_ void Release(CMaaStringImp* p) noexcept
    {
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
    constexpr operator const char* () const noexcept
    {
        TOOLSLIB_STR_HELPER_printf("S::op const ch*() const\n")
        return PIMP(m_pImp) ? m_pImp->m_pszStr : ceCMaaStringImp::sChar0_1;
    }
    //operator const char8_t* () const noexcept;
    //char* GetBuffer() noexcept;
    //int Length() const noexcept;
    //int __CharCount() const noexcept;
    //int __Pos(int CharIndex) const noexcept;
    //CMaaString __Left(int CharIndex);
    //CMaaString __Ellipse(int CharIndex = 64, CMaaString el = CMaaStringEllipse);
    int_ LastChar() const noexcept; // -0x100 if length == 0, else (int)(char)[..]
    int_ LastUChar() const noexcept; // -0x100 if length == 0, else (int)(unsigned char)(char)[..]
    ////CMaaString operator += (char c);
    //bool IsLeft(const char * ptr, int nCount) const noexcept;
    //bool IsRight(const char * ptr, int nCount) const noexcept;
    //bool IsMid(const char * ptr, int nFirst, int nCount) const noexcept;
    _dword Hash() const noexcept;
    _uqword Hash64() const noexcept;
    //int GetLength() const noexcept;
    void ClearHash() noexcept // call just before before/after direct string modifications
    {
#if defined(CMAASTRING_CACHE_HASH) || defined(CMAASTRING_CACHE_HASH64)
        if (m_pImp != sp_NullImp)
        {
            m_pImp->ClearHash();
        }
#endif
    }
    void Empty() noexcept;
    //bool Is0Behind() const noexcept;
    //bool IsWC0Behind() const noexcept;
    //bool IsWChar0Behind() const noexcept;
    //bool IsWC32_0Behind() const noexcept;

    // alloc copy of string
    CMaaString NewCopy() const noexcept(noexcept_new)
    {
        return CMaaString((const char*)*this, Length(), GetUtf1632Flags());
    }
    CMaaString Str0Copy() const noexcept(noexcept_new)
    {
        return Is0Behind() ? *this : NewCopy();
    }
    template<bool z = false> CMaaString z_str() const noexcept(!z || noexcept_new)
    {
        const int z_pos = Find('\0');
        if constexpr (z)
        {
            return z_pos >= 0 ? RefLeft(z_pos) : Str0Copy();
        }
        return z_pos >= 0 ? RefLeft(z_pos) : *this;
    }

#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    bool operator!=(char* pszTxt) const noexcept;
    bool operator!=(const void* pszTxt) const noexcept { return operator!=((char*)pszTxt); }
    bool operator==(char* pszTxt) const noexcept;
    bool operator==(const void* pszTxt) const noexcept { return operator==((char*)pszTxt); }
    template<std::size_t N> bool operator!=(const char(&txt)[N]) const noexcept
    {
        //const size_t l = GET_CHECK_TXT_N1_N(txt, N);
        //return Length() != (int)l || memcmp(*this, txt, l);
        return !(*this == txt);
    }
    template<std::size_t N> bool operator==(const char(&txt)[N]) const noexcept
    {
        const size_t l = GET_CHECK_TXT_N1_N(txt, N);
        return Length() == (int)l &&
            CMaaIsMemEq(*this, txt, l);
            //!memcmp(*this, txt, l);
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    bool operator!=(char8_t* pszTxt) const noexcept { return operator!=((char*)pszTxt); }
    bool operator==(char8_t* pszTxt) const noexcept { return operator==((char*)pszTxt); }
    template<std::size_t N> bool operator!=(const char8_t(&txt)[N]) const noexcept
    {
        //const size_t l = GET_CHECK_TXT_N1_N(txt, N);
        //return Length() != (int)l || memcmp(*this, txt, l);
        return !(*this == txt);
    }
    template<std::size_t N> bool operator==(const char8_t(&txt)[N]) const noexcept
    {
        const size_t l = GET_CHECK_TXT_N1_N(txt, N);
        return Length() == (int)l &&
            CMaaIsMemEq(*this, txt, l);
            //!memcmp(*this, txt, l);
    }
#endif
#else
    bool operator != (const char* str) const noexcept;
    bool operator == (const char* str) const noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    bool operator != (const char8_t* str) const noexcept { return !operator==((const char *)str); }
    bool operator == (const char8_t* str) const noexcept { return operator==((const char *)str); }
#endif
#endif
    bool operator != (const CMaaConstStr &s) const noexcept;
    bool operator == (const CMaaConstStr &s) const noexcept;
    bool operator != (const CMaaString& That) const noexcept;
    bool operator == (const CMaaString& That) const noexcept;
    constexpr bool StrictEq(const CMaaString& That) const noexcept // ===
    {
        return m_pImp == That.m_pImp;
    }
    /*
    bool EqEx(const CMaaString& That) const noexcept
    {
        return m_pImp == That.m_pImp || *this == That;
    }
    */

    bool operator < (const CMaaString& That) const noexcept;
    bool operator > (const CMaaString& That) const noexcept;
    bool operator <= (const CMaaString& That) const noexcept;
    bool operator >= (const CMaaString& That) const noexcept;

    static int_ Compare0(const CMaaString& a, const char* str) noexcept; // signed comparison
    static int_ Compare0(const CMaaString& a, const unsigned char* str) noexcept; // unsigned comparison
    static int_ Compare0(const char* str, const CMaaString& a) noexcept { return -Compare0(a, str); } // signed comparison
    static int_ Compare0(const unsigned char* str, const CMaaString& a) noexcept { return -Compare0(a, str); } // unsigned comparison

    // unsigned comparison
    bool operator < (const char* str) const noexcept { return Compare0(*this, (const unsigned char*)str) < 0; }
    bool operator > (const char* str) const noexcept { return Compare0(*this, (const unsigned char*)str) > 0; }
    bool operator <= (const char* str) const noexcept { return Compare0(*this, (const unsigned char*)str) <= 0; }
    bool operator >= (const char* str) const noexcept { return Compare0(*this, (const unsigned char*)str) >= 0; }

    // unsigned comparison
    bool operator < (const unsigned char* str) const noexcept { return Compare0(*this, str) < 0; }
    bool operator > (const unsigned char* str) const noexcept { return Compare0(*this, str) > 0; }
    bool operator <= (const unsigned char* str) const noexcept { return Compare0(*this, str) <= 0; }
    bool operator >= (const unsigned char* str) const noexcept { return Compare0(*this, str) >= 0; }

#ifdef TOOLSLIB_CHAR8T_SUPPORT_______ // def ctor of literal is preffered, but byte comparison is used
    bool operator < (const char8_t* str) const noexcept(noexcept_new)
    {
        return Compare(*this, CMaaString(str), 2) < 0;
    }
    bool operator > (const char8_t* str) const noexcept(noexcept_new)
    {
        return Compare(*this, CMaaString(str), 2) > 0;
    }
    bool operator <= (const char8_t* str) const noexcept(noexcept_new)
    {
        return Compare(*this, CMaaString(str), 2) <= 0;
    }
    bool operator >= (const char8_t* str) const noexcept(noexcept_new)
    {
        return Compare(*this, CMaaString(str), 2) >= 0;
    }
#endif

    static int_ Compare(const CMaaString& a, const CMaaString& b) noexcept;
    static int_ Compare(const CMaaString& a, const CMaaString& b, int_ Type /*1 - unicode, 2 - utf8, 3 - utf8 with qSortOptimmizationWithStringInjection*/)
#ifndef CMAASTRING_HAS_ADD_DATA
        noexcept
#else
        noexcept(noexcept_new)
#endif
        ;

    char* GetBuffer() noexcept
    {
        if (PIMP(m_pImp))
        {
            ClearHash(); // ADD_REF_DATA assumption: IsSingleOwner or UnAlloc string
            return m_pImp->m_pszStr;
        }
        return (char*)ceCMaaStringImp::sChar0_1;
        //return (char*)(const char*)(*this);
    }
    CMaaString GetAddRefData(int Idx = 0, bool bLock = false) const noexcept;
    bool SetAddRefData(const CMaaString& x, int Idx = 0, bool bNeedSingleOwner = 
#ifdef TOOLSLIB_SINGLE_THREAD
        false
#else
        true
#endif
        , int iLock = -1) noexcept // iLock == -1 == no lock on bNeedSingleOwner == true; iLock == 0 == external lock
    {
#ifdef CMAASTRING_HAS_ADD_REF_DATA
        return (m_pImp != sp_NullImp) && m_pImp->SetAddRefData(x.m_pImp, Idx, bNeedSingleOwner, iLock);
#else
        return false;
#endif
    }
    static bool IsBadStrPtr(CMaaString *p); // check is the string memory readable/writable

    //char operator [] ( size_t Index ) const;

    constexpr int Length() const noexcept
    {
        return PIMP(m_pImp) ? m_pImp->m_iLength : 0;
    }
    //int Utf8CharCount() const {return Utf8CharCount(true);}
    int Utf8CharCount(bool CutAsFull = true) const noexcept;
    CMaaString Utf8ToAnsiEx() const noexcept(noexcept_new);
    _sdword Utf8CharsSequenceToUnicodeChar(int &pos, bool bMovePos = false, bool bAllowExtendedWideChars = true) noexcept;
    CMaaString Utf8ToPrintable(bool bAllowCrLf = true) noexcept(noexcept_new); // characters range < ' ' --> '_'
    //int Utf8Pos(int CharIndex) const {return Utf8Pos(CharIndex, true);}
    int Utf8Pos(int CharIndex, bool CutAsFull = true) const noexcept;
    int Utf8PosFromPos0(int CharIndex, int pos0 = 0, bool CutAsFull = true) const noexcept; // pos0 - bytes pos. pos0=0 === Utf8Pos
    int Utf8CharsLenFromPos0(int CharsLen, int pos0 = 0, bool bRetErrors = true, bool bUtf32 = false) const noexcept; // pos0 - bytes pos. returts len of CharsLen chars in bytes, -1 on errors && bRetErrors, -4 on utf32 char && !bUtf32
    int __CharCount() const noexcept
    {
#if defined(_UNICODE) || defined(__unix__)
        return Utf8CharCount();
#else
        return Length();
#endif
    }
    int __Pos(int CharIndex) const noexcept
    {
#if defined(_UNICODE) || defined(__unix__)
        return Utf8Pos(CharIndex);
#else
        return CharIndex < 0 ? 0 : CharIndex < Length() ? CharIndex : Length();
#endif
    }
    CMaaString __Left(int CharIndex) noexcept(noexcept_new)
    {
        return Left(__Pos(CharIndex));
    }
    CMaaString __Ellipse(int CharIndex = 64, CMaaString el = "...") noexcept(noexcept_new) // = CMaaStringEllipse
    {
        CMaaString r = __Left(CharIndex);
        if  (r.Length() != Length())
        {
            r += el;
        }
        return r;
    }

    CMaaString RemoveFromRight(int nCount) const noexcept(noexcept_new);

    constexpr_ bool IsValid() const noexcept
    {
        return m_pImp != sp_NullImp;
    }

    constexpr_ bool isEmpty() const noexcept
    {
        return m_pImp == sp_NullImp; // Length() == 0; // or !m_pImp
    }

    //CMaaString operator + ( const char * pszStr );
    //CMaaString & operator += ( const char * pszStr );
    CMaaString & operator += (const CMaaString& That) noexcept(noexcept_new);
    CMaaString & operator -= (int n) noexcept(noexcept_new);
    CMaaString & Add(const void * pMem, int Len) noexcept(noexcept_new); // slow
    CMaaString & operator += (const char * szString) noexcept(noexcept_new);
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& operator += (const char8_t* szString) noexcept(noexcept_new) { return operator += ((const char*)szString); }
#endif
    CMaaString & operator += (char c) noexcept(noexcept_new) { return Add(&c, 1); }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString & operator += (char8_t c) noexcept(noexcept_new) { return Add(&c, 1); }
#endif
    //CMaaString & operator += (_WC_ c) { return Add(&c, (int)sizeof(c)); }
    //CMaaString & operator += (char32_t c) { return Add(&c, (int)sizeof(c)); }
    //CMaaString & operator += (wchar_t c) { return Add(&c, (int)sizeof(c)); }
public:
    class Helper
    {
    protected:
        CMaaStringImp* m_pImp;
    public:
        Helper(const CMaaString& x) noexcept
        {
            TOOLSLIB_STR_HELPER_printf("H(c S&)\n")
            if ((m_pImp = x.m_pImp) != sp_NullImp)
            {
                m_pImp->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            }
        }
        ~Helper()
        {
            TOOLSLIB_STR_HELPER_printf("~H()\n")
            if (m_pImp != sp_NullImp)
            //if (PIMP(m_pImp))
            {
                DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
                if (!m_pImp->Release())
                {
#if MANUAL_STRINGS_REFS
                    delete m_pImp;
#endif
                }
            }
        }
        Helper(const Helper& That) noexcept
        {
            TOOLSLIB_STR_HELPER_printf("H(c H&)\n")
            if ((m_pImp = That.m_pImp) != sp_NullImp)
            {
                m_pImp->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            }
        }
        Helper(Helper&& That) noexcept
        {
            TOOLSLIB_STR_HELPER_printf("H(H &&)\n")
            m_pImp = That.m_pImp;
            That.m_pImp = sp_NullImp;
        }
        Helper& operator=(const Helper& That) noexcept = delete;
        Helper& operator=(Helper&& That) noexcept = delete;
        /*Helper(const Helper& That) noexcept
        {
            if ((m_pImp = That.m_pImp) != sp_NullImp)
            {
                m_pImp->AddRef(); INC_TOOLSLIB_STAT_STRING_IMP_REFS(true);
            }
        }
        */
        template<class S> Helper& operator +=(const S& txt) noexcept(noexcept_new)
        {
            TOOLSLIB_STR_HELPER_printf("H& op+=(T)\n")
            (*(CMaaString*)this) += txt;
            return *this;
        }
        template<class S> Helper& operator +(const S& txt) noexcept(noexcept_new)
        {
            TOOLSLIB_STR_HELPER_printf("H& op+(T)\n")
            (*(CMaaString*)this) += txt;
            return *this;
        }
        Helper& operator -=(int n) noexcept(noexcept_new)
        {
            TOOLSLIB_STR_HELPER_printf("H& op-=(n)\n")
            (*(CMaaString*)this) -= n;
            return *this;
        }
        Helper& operator -(int n) noexcept(noexcept_new)
        {
            TOOLSLIB_STR_HELPER_printf("H& op-(n)\n")
            (*(CMaaString*)this) -= n;
            return *this;
        }
        operator const CMaaString& () const noexcept
        {
            TOOLSLIB_STR_HELPER_printf("H::op c S&() const\n")
            return *(const CMaaString*)this;
        }
        operator CMaaString& () noexcept
        {
            TOOLSLIB_STR_HELPER_printf("H::op S&()\n")
            return *(CMaaString*)this;
        }
        //operator const char* () const noexcept { return *(const CMaaString*)this; }
        const CMaaString& s() const noexcept
        {
            TOOLSLIB_STR_HELPER_printf("const S& H::s() const\n")
            return *(const CMaaString*)this;
        }
        CMaaString& s() noexcept
        {
            TOOLSLIB_STR_HELPER_printf("S& H::s()\n")
            return *(CMaaString*)this;
        }
        const char* c() const noexcept
        {
            TOOLSLIB_STR_HELPER_printf("c ch* H::c() const\n")
            return *(const CMaaString*)this;
        }
    };
    const CMaaString& s() const noexcept
    {
        TOOLSLIB_STR_HELPER_printf("const S& S::s() const\n")
        return *this;
    }
    CMaaString& s() noexcept
    {
        TOOLSLIB_STR_HELPER_printf("S& S::s()\n")
        return *this;
    }
    const char* c() const noexcept
    {
        TOOLSLIB_STR_HELPER_printf("const ch* S::c() const\n")
        return *this;
    }
    Helper operator + (const char * szString) const noexcept(noexcept_new);
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    Helper operator + (const char8_t* szString) const noexcept(noexcept_new) { return operator + ((const char*)szString); }
#endif
    Helper operator + (const CMaaString& That) const noexcept(noexcept_new);
    Helper operator - (int n) const noexcept(noexcept_new);

    char at(int idx) {return (char)(*this)[idx];} // temp fn, not use it
    int length() const noexcept {return Length(); } // temp fn, not use it

    static CMaaString _sFormat(int_ SrcLine, const char* SrcFile, const char* format, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    static CMaaString _sFormat(int_ SrcLine, const char* SrcFile, const char8_t* format, ...) noexcept;
#endif
    static CMaaString _sFormat(int_ SrcLine, const char* SrcFile, CMaaString format, ...) noexcept;

    static CMaaString _sFormat2(int_ SrcLine, const char* SrcFile, const char* format, const char* text, ...) noexcept;
    static CMaaString _sFormat2(int_ SrcLine, const char* SrcFile, const char* format, CMaaString text, ...) noexcept;
    static CMaaString _sFormat2(int_ SrcLine, const char* SrcFile, CMaaString format, const char* text, ...) noexcept;
    static CMaaString _sFormat2(int_ SrcLine, const char* SrcFile, CMaaString format, CMaaString text, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    static CMaaString _sFormat2(int_ SrcLine, const char* SrcFile, const char* format, const char8_t* text, ...) noexcept;
    static CMaaString _sFormat2(int_ SrcLine, const char* SrcFile, CMaaString format, const char8_t* text, ...) noexcept;
#endif

    CMaaString& _Format(int_ SrcLine, const char* SrcFile, const char* format, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& _Format(int_ SrcLine, const char* SrcFile, const char8_t* format, ...) noexcept;
#endif
    CMaaString& _Format(int_ SrcLine, const char* SrcFile, CMaaString format, ...) noexcept;

    CMaaString& _Format2(int_ SrcLine, const char* SrcFile, const char* format, const char* text, ...) noexcept;
    CMaaString& _Format2(int_ SrcLine, const char* SrcFile, const char* format, CMaaString text, ...) noexcept;
    CMaaString& _Format2(int_ SrcLine, const char* SrcFile, CMaaString format, const char* text, ...) noexcept;
    CMaaString& _Format2(int_ SrcLine, const char* SrcFile, CMaaString format, CMaaString text, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& _Format2(int_ SrcLine, const char* SrcFile, const char* format, const char8_t* text, ...) noexcept;
    CMaaString& _Format2(int_ SrcLine, const char* SrcFile, CMaaString format, const char8_t* text, ...) noexcept;
#endif

    CMaaString& _FormatV(int_ SrcLine, const char* SrcFile, const char* format, va_list list) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& _FormatV(int_ SrcLine, const char* SrcFile, const char8_t* format, va_list list) noexcept;
#endif
    CMaaString& _FormatV(int_ SrcLine, const char* SrcFile, CMaaString format, va_list list) noexcept;

    CMaaString& _FormatV2(int_ SrcLine, const char* SrcFile, const char* format, const char* text, va_list list) noexcept;
    CMaaString& _FormatV2(int_ SrcLine, const char* SrcFile, const char* format, CMaaString text, va_list list) noexcept;
    CMaaString& _FormatV2(int_ SrcLine, const char* SrcFile, CMaaString format, const char* text, va_list list) noexcept;
    CMaaString& _FormatV2(int_ SrcLine, const char* SrcFile, CMaaString format, CMaaString text, va_list list) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& _FormatV2(int_ SrcLine, const char* SrcFile, const char* format, const char8_t* text, va_list list) noexcept;
    CMaaString& _FormatV2(int_ SrcLine, const char* SrcFile, CMaaString format, const char8_t* text, va_list list) noexcept;
#endif

    static CMaaString sFormat(const char* format, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    static CMaaString sFormat(const char8_t* format, ...) noexcept;
#endif
    static CMaaString sFormat(CMaaString format, ...) noexcept;

    static CMaaString sFormat2(const char* format, const char* text, ...) noexcept;
    static CMaaString sFormat2(const char* format, CMaaString text, ...) noexcept;
    static CMaaString sFormat2(CMaaString format, const char* text, ...) noexcept;
    static CMaaString sFormat2(CMaaString format, CMaaString text, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    static CMaaString sFormat2(const char* format, const char8_t* text, ...) noexcept;
    static CMaaString sFormat2(CMaaString format, const char8_t* text, ...) noexcept;
#endif

    CMaaString& Format(const char* format, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& Format(const char8_t* format, ...) noexcept;
#endif
    CMaaString& Format(CMaaString format, ...) noexcept;
    
    CMaaString& Format2(const char* format, const char* text, ...) noexcept;
    CMaaString& Format2(const char* format, CMaaString text, ...) noexcept;
    CMaaString& Format2(CMaaString format, const char* text, ...) noexcept;
    CMaaString& Format2(CMaaString format, CMaaString text, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& Format2(const char* format, const char8_t* text, ...) noexcept;
    CMaaString& Format2(CMaaString format, const char8_t* text, ...) noexcept;
#endif

    CMaaString& FormatV(const char* format, va_list list) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& FormatV(const char8_t* format, va_list list) noexcept;
#endif
    CMaaString& FormatV(CMaaString format, va_list list) noexcept;

    CMaaString& FormatV2(const char* format, const char* text, va_list list) noexcept;
    CMaaString& FormatV2(const char* format, CMaaString text, va_list list) noexcept;
    CMaaString& FormatV2(CMaaString format, CMaaString text, va_list list) noexcept;
    CMaaString& FormatV2(CMaaString format, const char* text, va_list list) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& FormatV2(const char* format, const char8_t* text, va_list list) noexcept;
    CMaaString& FormatV2(CMaaString format, const char8_t* text, va_list list) noexcept;
#endif

    CMaaString& Append(const char* format, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& Append(const char8_t* format, ...) noexcept;
#endif
    CMaaString& Append(CMaaString format, ...) noexcept;

    CMaaString& Append2(const char* format, const char* text, ...) noexcept;
    CMaaString& Append2(const char* format, CMaaString text, ...) noexcept;
    CMaaString& Append2(CMaaString format, const char* text, ...) noexcept;
    CMaaString& Append2(CMaaString format, CMaaString text, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString& Append2(const char* format, const char8_t* text, ...) noexcept;
    CMaaString& Append2(CMaaString format, const char8_t* text, ...) noexcept;
#endif

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
    enum EStripType
    {
        eBegin    = 1,
        eEnd      = 2,
        eBoth     = 3,
        eCutCR    = 0x10,
        eCutLF    = 0x20,
        eCutCRLF  = 0x30,
        eCutEnd   = eCutLF, // Remove "\n" from end of string
        eCutSlash = 0x40    // Remove "\\" from end of string
    };
    CMaaString & Strip(EStripType Type = eBoth);
#endif

    CMaaString Left(int nCount) const noexcept(noexcept_new);
    CMaaString Right(int nCount) const noexcept(noexcept_new);
    CMaaString Mid(int nFirst, int nCount = -1) const noexcept(noexcept_new);
    CMaaString RefLeft(int nCount) const noexcept;
    CMaaString RefRight(int nCount) const noexcept;
    CMaaString RefMid(int nFirst, int nCount = -1) const noexcept;
    CMaaString LeftMid(int nLeft, int nFirst, int nCount = -1) const noexcept(noexcept_new);
    CMaaString LeftInsMid(int nLeft, const CMaaString& Ins, int nFirst, int nCount = -1) const noexcept(noexcept_new);
    CMaaString LeftInsRight(int nLeft, const CMaaString& Ins, int nRight) const noexcept(noexcept_new);
    CMaaString & SetLeft(int nCount) noexcept(noexcept_new);
    CMaaString & SetRight(int nCount) noexcept(noexcept_new);
    CMaaString & SetMid(int nFirst, int nCount = -1) noexcept(noexcept_new);
    CMaaString & SetLeftMid(int nLeft, int nFirst, int nCount = -1) noexcept(noexcept_new);
    CMaaString & SetLeftInsMid(int nLeft, const CMaaString& Ins, int nFirst, int nCount = -1) noexcept(noexcept_new);
    CMaaString & SetLeftInsRight(int nLeft, const CMaaString& Ins, int nRight) noexcept(noexcept_new);
    int_ TruncateRWS(int Len) noexcept;
    bool Truncate(int Len) noexcept(noexcept_new);
    bool IsLeft(const char * ptr, int nCount) const noexcept
    {
        return nCount >= 0 && Length() >= nCount && CMaaIsMemEq((const char *)*this, ptr, nCount);
    }
    bool IsRight(const char * ptr, int nCount) const noexcept
    {
        const int Len = Length();
        return Len >= nCount && CMaaIsMemEq(Len - nCount + (const char *)*this, ptr, nCount);
    }
    bool IsMid(const char * ptr, int nFirst, int nCount) const noexcept
    {
        const int Len = Length();
        return nCount >= 0 && nFirst >= 0 && Len >= nFirst + nCount && CMaaIsMemEq(nFirst + (const char *)*this, ptr, nCount);
    }
    bool IsCi(const char* ptr, int Len, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        return Length() == Len && ::IsCi((const char*)*this, ptr, Len, cp);
    }
    bool IsCi0(const char* ptr, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        return ::IsCi0((const char*)*this, ptr, Length(), cp);
    }
    bool IsLeftCi(const char* ptr, int nCount, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        return nCount >= 0 && Length() >= nCount && ::IsCi((const char*)*this, ptr, nCount, cp);
    }
    bool IsLeftCi0(const char* ptr, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        return ::IsCi0((const char*)*this, ptr, Length(), cp, true);
    }
    bool IsRightCi(const char* ptr, int nCount, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        const int Len = Length();
        return Len >= nCount && ::IsCi(Len - nCount + (const char*)*this, ptr, nCount, cp);
    }
    bool IsMidCi(const char* ptr, int nFirst, int nCount, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        const int Len = Length();
        return nCount >= 0 && nFirst >= 0 && Len >= nFirst + nCount && ::IsCi(nFirst + (const char*)*this, ptr, nCount, cp);
    }
    bool IsMidCi0(const char* ptr, int nFirst, int_ cp = -1) const noexcept //default -1: +rus // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    {
        const int Len = Length();
        return nFirst >= 0 && Len >= nFirst && ::IsCi0(nFirst + (const char*)*this, ptr, Len - nFirst, cp, true);
    }
    bool IsLeft(const CMaaString& That) const noexcept;
    bool IsRight(const CMaaString& That) const noexcept;
    bool IsMid(const CMaaString& That, int nFirst) const noexcept;
    bool IsCi(const CMaaString& That, int_ cp = -1) const noexcept;
    bool IsLeftCi(const CMaaString& That, int_ cp = -1) const noexcept;
    bool IsRightCi(const CMaaString& That, int_ cp = -1) const noexcept;
    bool IsMidCi(const CMaaString& That, int nFirst, int_ cp_ = -1) const noexcept;
    void Swap(CMaaString& That) noexcept { CMaaSwap(m_pImp, That.m_pImp); }
    CMaaString Rewind() const noexcept(noexcept_new);
    CMaaString RewindAdd(const CMaaString &AtBegin, const CMaaString &AtEnd) noexcept(noexcept_new); // rewind *this, add to result

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
    // FoundStartPos or -1
    int FindIp(int StartPos, _IP * pIp = nullptr, int * pEndOfFoundIpPos = nullptr, int EndPos = -1, bool bStrict = false) const noexcept;
#endif
//#define TOOLSLIB_STAT_FIND
#define FINDN_RET_FIXED_1(x) -1 // (x) // -1 // (x)
    static bool KMPComputePrefixFunction(const char * pP, int m, CMaaPtr_<int, -1>& f) noexcept;
    bool KMPComputePrefixFunction(CMaaPtr_<int, -1>& f) const noexcept;
    // Knuth-Morris-Pratt fast substring find algorithm, uses int[P.Length()] temp prefix array, return first shift or -1 not found or -2 alloc error or throw
    int KMPFind(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1) const noexcept;
    // Originally KMPFind2 is not the same as Find2: was
    // abcdefghij 1, "cde", "bcdef" : 1 1 // Find2
    // abcdefghij 1, "cde", "bcdef" : 2 0 // KMPFind2
    // Find SubStr1 or SubStr2 what is earlier
    int KMPFind2(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len = -1, int SubStr2Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    // Find SubStr1 or SubStr2 or SubStr3 what is earlier
    int KMPFind3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len = -1, int SubStr2Len = -1, int SubStr3Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    int KMPFind(const char* SubStr, int SubStrLen = -1) const noexcept;
    int KMPFind(const CMaaString &P) const noexcept;
    // Knuth-Morris-Pratt fast all substring's find algorithm, uses int[P.Length()] temp prefix array, return Ret: pos[Ret] array of shifts or Ret=-1 not found or Ret=-2 alloc error or throw
    int KMPFindAll(const CMaaString &P, CMaaPtr_<int, -1> &pos) const; //  except
    template<typename Fn> int KMPFindAll(int StartPos, const void* SubStr, int SubStrLen, int EndPos, Fn Proc) const noexcept(noexcept(Proc(Length())))
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
        const char* pT = StartPos + (const char*)*this;
        const char* pP = (const char*)SubStr;
        if (!KMPComputePrefixFunction(pP, m, f))
        {
            return FINDN_RET_FIXED_1(-2); // -1?  !! -2 === new[] err
        }
        int Ret = 0;
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
                //Proc(*this, P, i + 1 - m);
                if (Proc(i + 1 - m))
                {
                    Ret++;
                }
                q = pf[q - 1];
            }
        }
        return Ret;
    }
    template<typename Fn> int FindAll(int StartPos, const void* SubStr, int SubStrLen, int EndPos, Fn Proc) const noexcept(noexcept(Proc(Length())))
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
            return 0;
        }
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
        int Ret = 0;
        for (int pos = StartPos; ;)
        {
            if (Hw == Hs && *s == ch0 && s[SubStrLen - 1] == ch1 && CMaaIsMemEq(s, SubStr, SubStrLen - 1))
            {
                if (Proc(pos))
                {
                    Ret++;
                }
            }
            if (++pos > Len - SubStrLen)
            {
                break;
            }
            Hs += (unsigned char)s[SubStrLen] - (unsigned char)*s;
            s++;
        }
        return Ret;
    }
    int FindCi(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1, int_ cp = 0) const noexcept; // cp: -1, 0, 1251 // Typically EndPos is Length() - the position after the end of source string
    int FindCi(int StartPos, const CMaaString &SubStr, int EndPos = -1, int_ cp = 0) const noexcept; // cp: -1, 0, 1251 // Typically EndPos is Length() - the position after the end of source string
#ifndef USE_FIND_KMP
    int Find(int StartPos, const void * SubStr, int SubStrLen = -1, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
#else
    int Find(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const int r = KMPFind(StartPos, SubStr, SubStrLen, EndPos);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
#endif
    int Find(int StartPos, const CMaaString &SubStr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    // pRetN: -1 - not found, -2 - search exited by the other string found
    // Find SubStr1 or SubStr2 what is earlier
#ifndef USE_FIND_KMP
    int Find2(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len = -1, int SubStr2Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
#else
    int Find2(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len = -1, int SubStr2Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const int r = KMPFind2(StartPos, SubStr1, SubStr2, SubStr1Len, SubStr2Len, pIndex, EndPos);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
#endif
    int Find2(int StartPos, const CMaaString &SubStr1, const CMaaString &SubStr2, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    int Find2Ci(int StartPos, const char* SubStr1, const char* SubStr2, int SubStr1Len = -1, int SubStr2Len = -1, int_* pIndex = nullptr, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    int Find2Ci(int StartPos, const CMaaString& SubStr1, const CMaaString& SubStr2, int_* pIndex = nullptr, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    // Find SubStr1 or SubStr2 or SubStr3 what is earlier
#ifndef USE_FIND_KMP
    int Find3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len = -1, int SubStr2Len = -1, int SubStr3Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
#else
    int Find3(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len = -1, int SubStr2Len = -1, int SubStr3Len = -1, int_* pIndex = nullptr, int EndPos = -1) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const int r = KMPFind3(StartPos, SubStr1, SubStr2, SubStr3, SubStr1Len, SubStr2Len, SubStr3Len, pIndex, EndPos);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
#endif
    int Find3(int StartPos, const CMaaString &SubStr1, const CMaaString &SubStr2, const CMaaString &SubStr3, int_* pIndex = nullptr, int EndPos = -1) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    int Find3Ci(int StartPos, const char* SubStr1, const char* SubStr2, const char* SubStr3, int SubStr1Len = -1, int SubStr2Len = -1, int SubStr3Len = -1, int_* pIndex = nullptr, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    int Find3Ci(int StartPos, const CMaaString& SubStr1, const CMaaString& SubStr2, const CMaaString& SubStr3, int_* pIndex = nullptr, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is Length() - the position after the end of source string
    enum
    {
        eFindN_Unknown = 0x0,
        eFindN_StrOk = 0x1, // all str lens > 0, str[0] is the smallest length string
        eFindN_ReturnTailPos = 0x2 // return pos + strlen
    };
protected:
    template<int_ N> int FindN__(int StartPos, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_ *Index, int_ *pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
        const int SubStr1Len = SubStrLen[0];
        char ch0[N], ch1[N];
        int_ Hw[N], Hs[N];
        int i, j;
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
                if (pIndex)
                {
                    *pIndex = Index[0];
                }
                return pos + ((Flags & eFindN_ReturnTailPos) ? SubStr1Len : 0);
            }
            for (j = 1; j < N; j++)
            {
                if (pos + SubStrLen[j] <= Len && Hw[j] == Hs[j] && *s == ch0[j] && s[SubStrLen[j] - 1] == ch1[j] && CMaaIsMemEq(s, SubStr[j], SubStrLen[j] - 1))
                {
                    if (pIndex)
                    {
                        *pIndex = Index[j];
                    }
                    return pos + ((Flags & eFindN_ReturnTailPos) ? SubStrLen[j] : 0);
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
        /*
        if (pIndex)
        {
            *pIndex = -1;
        }
        */
        return -1;
    }
    template<int_ N> int FindN__Ci(int StartPos, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex, int_ cp) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const unsigned char* m = gLcMap(cp);
        const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
        const int SubStr1Len = SubStrLen[0];
        unsigned char ch0[N], ch1[N];
        int_ Hw[N], Hs[N];
        int i, j;
        for (i = 0; i < N; i++)
        {
            ch0[i] = m[(unsigned char)SubStr[i][0]];
            ch1[i] = m[(unsigned char)SubStr[i][SubStrLen[i] - 1]];
            Hw[i] = 0;
            Hs[i] = 0;
        }
        const char* s = StartPos + (const char*)*this;
        for (j = 0; j < N; j++)
        {
            for (i = 0; i < SubStrLen[j]; i++)
            {
                Hw[j] += m[(unsigned char)SubStr[j][i]];
                Hs[j] += m[(unsigned char)s[i]];
            }
        }

        for (int pos = StartPos; ;)
        {
            if (Hw[0] == Hs[0] && m[(unsigned char)*s] == ch0[0] && m[(unsigned char)s[SubStr1Len - 1]] == ch1[0] && ::IsCi(s, SubStr[0], SubStr1Len - 1, cp))
            {
                if (pIndex)
                {
                    *pIndex = Index[0];
                }
                return pos + ((Flags & eFindN_ReturnTailPos) ? SubStr1Len : 0);
            }
            for (j = 1; j < N; j++)
            {
                if (pos + SubStrLen[j] <= Len && Hw[j] == Hs[j] && m[(unsigned char)*s] == ch0[j] && m[(unsigned char)s[SubStrLen[j] - 1]] == ch1[j] && ::IsCi(s, SubStr[j], SubStrLen[j] - 1, cp))
                {
                    if (pIndex)
                    {
                        *pIndex = Index[j];
                    }
                    return pos + ((Flags & eFindN_ReturnTailPos) ? SubStrLen[j] : 0);
                }
            }
            if (++pos > Len - SubStr1Len)
            {
                break;
            }
            Hs[0] += m[(unsigned char)s[SubStr1Len]] - m[(unsigned char)*s];
            for (j = 1; j < N; j++)
            {
                if (pos + SubStrLen[j] <= Len)
                {
                    Hs[j] += m[(unsigned char)s[SubStrLen[j]]] - m[(unsigned char)*s];
                }
            }
            s++;
        }
        /*
        if (pIndex)
        {
            *pIndex = -1;
        }
        */
        return -1;
    }
    template<int_ N> int KMPFindN__(int StartPos, const char** SubStr, const int* SubStrLen, int EndPos, const int** pf, int_ Flags, const int_ *Index, int_ *pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
        //const int SubStr1Len = SubStrLen[0];
        //int MaxLen = SubStr2Len > SubStr1Len ? SubStr2Len : SubStr1Len; ...
        int_ j;
        int MaxLen = SubStrLen[0];
        int n = Len - StartPos;
        //const int m = SubStrLen;
        //CMaaPtr_<int, -1> f[N];
        const char* pT = StartPos + (const char*)*this;
        const char** pP = SubStr;//[N];
        //int* pf[N];
        int q[N] = {};
        for (j = 0; j < N; j++)
        {
            if (SubStrLen[j] > MaxLen)
            {
                MaxLen = SubStrLen[j];
            }
            /*
            CMaaPtr_<int, -1> ff(SubStrLen[j]);
            f[j].Swap(ff);
            if (!KMPComputePrefixFunction(pP[j], SubStrLen[j], f[j]))
            {
                return FINDN_RET_FIXED_1(-2); // -1?  !! -2 === new[] err
            }
            pf[j] = f[j];
            */
        }
        int Pos = -1, Ret = -1;
        int_ idx = -1;
        for (int i = 0; i < n; i++)
        {
            for (j = 0; j < N; j++)
            {
                while (q[j] > 0 && pP[j][q[j]] != pT[i])
                {
                    q[j] = pf[j][q[j] - 1];
                }
                if (pP[j][q[j]] == pT[i])
                {
                    q[j]++;
                }
                if (q[j] == SubStrLen[j])
                {
                    if (Pos < 0 || StartPos + (i + 1 - SubStrLen[j]) < Pos || (StartPos + (i + 1 - SubStrLen[j]) == Pos && Index[j] < idx))
                    {
                        if (Pos < 0 && i + MaxLen - SubStrLen[j] + 1 < n)
                        {
                            n = i + MaxLen - SubStrLen[j] + 1;
                        }
                        Pos = StartPos + (i + 1 - SubStrLen[j]);
                        Ret = Pos + ((Flags & eFindN_ReturnTailPos) ? SubStrLen[j] : 0);
                        idx = Index[j];
                    }
                    q[j] = pf[j][q[j] - 1];
                }
            }
        }
        if (/*Pos >= 0 &&*/ pIndex)
        {
            *pIndex = idx;
        }
        return Ret;
    }
    template<int_ N> int FindN_(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        return n < N ?
            FindN_<N - 1>(StartPos, n, SubStr, SubStrLen, EndPos, Flags, Index, pIndex) :
            FindN__<N>(StartPos, SubStr, SubStrLen, EndPos, Flags, Index, pIndex);
    }
    template<int_ N> int FindN_Ci(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex, int_ cp) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        return n < N ?
            FindN_Ci<N - 1>(StartPos, n, SubStr, SubStrLen, EndPos, Flags, Index, pIndex, cp) :
            FindN__Ci<N>(StartPos, SubStr, SubStrLen, EndPos, Flags, Index, pIndex, cp);
    }
    template<int_ N> int KMPFindN_(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, const int** pf, int_ Flags, const int_ *Index, int_ *pIndex) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        return n < N ? 
            KMPFindN_<N - 1>(StartPos, n, SubStr, SubStrLen, EndPos, pf, Flags, Index, pIndex) :
            KMPFindN__<N>(StartPos, SubStr, SubStrLen, EndPos, pf, Flags, Index, pIndex);
    }
public:
#ifndef USE_FIND_KMP
    template<int_ N> struct sFindNCalcData
    {
        int SubStrLen[N];
        sFindNCalcData() noexcept
        {
            //memset(SubStrLen, -1, sizeof(SubStrLen));
            SubStrLen[0] = -1;
        }
        sFindNCalcData(const char ** Str) noexcept
        {
            for (int_ i = 0; i < N; i++)
            {
                SubStrLen[i] = int_strlen(Str[i]);
            }
        }
    };
    template<int_ N> int FindN(int StartPos, const char** SubStr, int* SubStrLen = nullptr, int_* pIndex = nullptr, int EndPos = -1, int_ Flags = eFindN_StrOk) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        int _SubStrLen[N];
        int_ Index[N];
        pcCHAR_CMaaTL _SubStr[N];
        //int tmpIndex;
        if (!SubStrLen)
        {
            //memset(SubStrLen = _SubStrLen, -1, N * sizeof(int));
            SubStrLen = _SubStrLen;
            SubStrLen[0] = -1;
        }
        if (pIndex)
        {
            *pIndex = -1;
        }
        if (StartPos < 0)
        {
            return -1;
        }
        memcpy(_SubStr, SubStr, N * sizeof(const char*));
        //memcpy(_SubStrLen, SubStrLen, N * sizeof(int));
        int_ i;
        if (SubStrLen[0] == -1)
        {
            for (i = 0; i < N; i++)
            {
                Index[i] = i;
                //_SubStr[i] = SubStr[i];
                //_SubStrLen[i] = SubStrLen[i];
                SubStrLen[i] = int_strlen(SubStr[i]);
            }
        }
        else
        {
            for (i = 0; i < N; i++)
            {
                Index[i] = i;
            }
        }
        SubStr = _SubStr;
        //SubStrLen = _SubStrLen;
        const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
        int_ j = 0;
        for (i = 0; i < N; i++)
        {
            if (SubStrLen[i] > 0 && StartPos + SubStrLen[i] <= Len)
            {
                SubStr[j] = SubStr[i];
                _SubStrLen[j] = SubStrLen[i];
                Index[j] = Index[i];
                if (!(Flags & eFindN_StrOk) && _SubStrLen[j] < _SubStrLen[0])
                {
                    CMaaSwap(SubStr[j], SubStr[0]);
                    CMaaSwap(_SubStrLen[j], _SubStrLen[0]);
                    CMaaSwap(Index[j], Index[0]);
                }
                j++;
            }
        }
        return FindN_<N>(StartPos, j, SubStr, _SubStrLen, EndPos, Flags, Index, pIndex);// | eFindN_StrOk);
    }
    template<int_ N> int FindNCi(int StartPos, const char** SubStr, int* SubStrLen = nullptr, int_* pIndex = nullptr, int EndPos = -1, int_ Flags = eFindN_StrOk, int_ cp = 0) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        int _SubStrLen[N];
        int_ Index[N];
        pcCHAR_CMaaTL _SubStr[N];
        //int tmpIndex;
        if (!SubStrLen)
        {
            //memset(SubStrLen = _SubStrLen, -1, N * sizeof(int));
            SubStrLen = _SubStrLen;
            SubStrLen[0] = -1;
        }
        if (pIndex)
        {
            *pIndex = -1;
        }
        if (StartPos < 0)
        {
            return -1;
        }
        memcpy(_SubStr, SubStr, N * sizeof(const char*));
        //memcpy(_SubStrLen, SubStrLen, N * sizeof(int));
        int_ i;
        if (SubStrLen[0] == -1)
        {
            for (i = 0; i < N; i++)
            {
                Index[i] = i;
                //_SubStr[i] = SubStr[i];
                //_SubStrLen[i] = SubStrLen[i];
                SubStrLen[i] = int_strlen(SubStr[i]);
            }
        }
        else
        {
            for (i = 0; i < N; i++)
            {
                Index[i] = i;
            }
        }
        SubStr = _SubStr;
        //SubStrLen = _SubStrLen;
        const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
        int_ j = 0;
        for (i = 0; i < N; i++)
        {
            if (SubStrLen[i] > 0 && StartPos + SubStrLen[i] <= Len)
            {
                SubStr[j] = SubStr[i];
                _SubStrLen[j] = SubStrLen[i];
                Index[j] = Index[i];
                if (!(Flags & eFindN_StrOk) && _SubStrLen[j] < _SubStrLen[0])
                {
                    CMaaSwap(SubStr[j], SubStr[0]);
                    CMaaSwap(_SubStrLen[j], _SubStrLen[0]);
                    CMaaSwap(Index[j], Index[0]);
                }
                j++;
            }
        }
        return FindN_Ci<N>(StartPos, j, SubStr, _SubStrLen, EndPos, Flags, Index, pIndex, cp);// | eFindN_StrOk);
    }
#else
    template<int_ N> int FindN(int StartPos, const char** SubStr, int* SubStrLen = nullptr, int_* pIndex = nullptr, int EndPos = -1, int_ Flags = eFindN_StrOk) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        const int r = KMPFindN<N>(StartPos, SubStr, SubStrLen, pIndex, EndPos, nullptr, Flags);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
#endif
    template<int_ N> struct sKMPFindNCalcData
    {
        int SubStrLen[N];
        CMaaPtr_<int, -1> f[N];
        sKMPFindNCalcData(const sKMPFindNCalcData &That) = delete;
        sKMPFindNCalcData & operator=(const sKMPFindNCalcData &That) = delete;
        sKMPFindNCalcData(bool bInit = true) noexcept
        {
            if (bInit)
            {
                memset(SubStrLen, -1, sizeof(SubStrLen));
            }
        }
    };
    template<int_ N> int KMPFindN(int StartPos, const char** SubStr, int* SubStrLen = nullptr, int_* pIndex = nullptr, int EndPos = -1, sKMPFindNCalcData<N> * f = nullptr, int_ Flags = eFindN_StrOk) const noexcept // Typically EndPos is Length() - the position after the end of source string
    {
        int _SubStrLen[N];
        int_ Index[N];
        pcCHAR_CMaaTL _SubStr[N];
        const int* pf[N];
        sKMPFindNCalcData<N> ff(!f);
        f = f ? f : &ff;
        //int tmpIndex;
        if (!SubStrLen)
        {
            SubStrLen = f->SubStrLen;
            //memset(SubStrLen = _SubStrLen, -1, N * sizeof(int));
            //for (i = 0; i < N; i++)
            //{
            //    SubStrLen[i] = -1;
            //}
        }
        if (pIndex)
        {
            *pIndex = -1;
        }
        if (StartPos < 0)
        {
            return -1;
        }
        memcpy(_SubStr, SubStr, N * sizeof(const char*));
        //memcpy(_SubStrLen, SubStrLen, N * sizeof(int));
        int_ i;
        for (i = 0; i < N; i++)
        {
            Index[i] = i;
            //_SubStr[i] = SubStr[i];
            //_SubStrLen[i] = SubStrLen[i];
            if (SubStrLen[i] == -1)
            {
                SubStrLen[i] = int_strlen(SubStr[i]);
            }
        }
        SubStr = _SubStr;
        const int Len = EndPos != -1 && EndPos <= Length() ? EndPos : Length();
        int_ j = 0;
        for (i = 0; i < N; i++)
        {
            if (SubStrLen[i] > 0 && StartPos + SubStrLen[i] <= Len)
            {
                SubStr[j] = SubStr[i];
                _SubStrLen[j] = SubStrLen[i];
                Index[j] = Index[i];
                if ((int)f->f[i].MaxIndex() != SubStrLen[i])
                {
                    CMaaPtr_<int, -1> ff(SubStrLen[i]);
                    f->f[i].Swap(ff);
                    if (!KMPComputePrefixFunction(SubStr[i], SubStrLen[i], f->f[i]))
                    {
                        return -2;//FINDN_RET_FIXED_1(-2); // -1?  !! -2 === new[] err
                    }
                }
                pf[j] = f->f[i];
                /*
                if (!(Flags & eFindN_StrOk) && SubStrLen[j] < SubStrLen[0])
                {
                    CMaaSwap(SubStr[j], SubStr[0]);
                    CMaaSwap(SubStrLen[j], SubStrLen[0]);
                    CMaaSwap(Index[j], Index[0]);
                }
                */
                j++;
            }
        }
        return KMPFindN_<N>(StartPos, j, SubStr, _SubStrLen, EndPos, pf, Flags, Index, pIndex);// | eFindN_StrOk);
    }
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
    int FindN_old(int StartPos, int_ N, const char** SubStr, int* SubStrLen = nullptr, int* pPos = nullptr, int EndPos = -1, int_ Flags = eFindN_StrOk) const noexcept; // N <= 10 Typically EndPos is Length() - the position after the end of source string // FindN_old(,3,...) is 2.5% lower then FindN<3>(...)
    int FindNEx(int StartPos, int_ N, const char **SubStr, int *SubStrLen = nullptr, int *pPos = nullptr, int EndPos = -1, int_ Flags = eFindN_Unknown) const; // Typically EndPos is Length() - the position after the end of source string
#endif
    int Find(int StartPos, char c, int EndPos = -1) const noexcept;
    // Find c1 or c2
    int Find2(int StartPos, char c1, char c2, int EndPos = -1) const noexcept;
    // Find c1 or c2 or c3
    int Find3(int StartPos, char c1, char c2, char c3, int EndPos = -1) const noexcept;
    int Find(int StartPos, const CMaa256Bits &Bits, int EndPos = -1) const noexcept;
    int Count(int StartPos, char c0, char c1, int EndPos = -1) const noexcept;
    int Count(int StartPos, unsigned char c0, unsigned char c1, int EndPos = -1) const noexcept;
    int Count(int StartPos, const CMaa256Bits& Bits, int EndPos = -1) const noexcept;
    int CountNum(int StartPos, int EndPos = -1) const noexcept;
    int CountNum() const noexcept;
    int FindCi(int StartPos, char c, int EndPos = -1, int_ cp = 0) const noexcept;
    int Find2Ci(int StartPos, char c1, char c2, int EndPos = -1, int_ cp = 0) const noexcept;
    int Find3Ci(int StartPos, char c1, char c2, char c3, int EndPos = -1, int_ cp = 0) const noexcept;
    int FindCrLf(int StartPos, int * pCrLfLen = nullptr, int * pOffset_from_StartPos_i_e_LineLength = nullptr, int EndPos = -1, int * pAutoMovePos = nullptr, CMaaString * pCrLf = nullptr) const noexcept;
#ifndef USE_FIND_KMP
    int Find(const char* substr) const noexcept;
#else
    int Find(const char* substr) const noexcept
    {
        const int r = KMPFind(0, substr);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
#endif
    int FindCi(const char* substr, int_ cp = 0) const noexcept; // cp: -1, 0, 1251
    int FindCi(const CMaaString &substr, int_ cp = 0) const noexcept; // cp: -1, 0, 1251
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    int Find(const char8_t* substr) const noexcept { return Find((const char*) substr); }
#endif
    int Find(char c) const noexcept;
    int FindCi(char c, int_ cp = 0) const noexcept;
    int Find(const CMaa256Bits& Bits) const noexcept;
    int Find() const noexcept = delete;
#ifndef USE_FIND_KMP
    int Find(const CMaaString& That) const noexcept;
#else
    int Find(const CMaaString& That) const noexcept
    {
        const int r = KMPFind(That);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
    int Find(int StartPos, const CMaaString& That, int EndPos = -1) const noexcept
    {
        const int r = KMPFind(StartPos, (const char*)That, That.Length(), EndPos);
#if FINDN_RET_FIXED_1(-2) == -2
        return r >= 0 ? r : -1;
#else
        return r;
#endif
    }
#endif
    int ReverseFind(int StartPos, const void * SubStr, int SubStrLen = -1, int EndPos = -1) const noexcept; // Typically EndPos is a -1 - the position before start of source string
    int ReverseFindCi(int StartPos, const void* SubStr, int SubStrLen = -1, int EndPos = -1, int_ cp = 0) const noexcept; // Typically EndPos is a -1 - the position before start of source string
    int ReverseFind(int StartPos, char c, int EndPos = -1) const noexcept;
    int ReverseFind2(int StartPos, char c1, char c2, int EndPos = -1) const noexcept;
    int ReverseFind3(int StartPos, char c1, char c2, char c3, int EndPos = -1) const noexcept;
    int ReverseFind(int StartPos, const CMaa256Bits &Bits, int EndPos = -1) const noexcept;
    int ReverseFindCi(int StartPos, char c, int EndPos = -1, int_ cp = 0) const noexcept;
    int ReverseFind(const char * substr) const noexcept;
    //int ReverseFind(const CMaaString &substr) const noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    int ReverseFind(const char8_t * substr) const noexcept { return ReverseFind((const char*)substr); }
#endif
    int ReverseFind(char c) const noexcept;
    int ReverseFind2(char c1, char c2) const noexcept;
    int ReverseFind3(char c1, char c2, char c3) const noexcept;
    int ReverseFind(const CMaa256Bits &Bits) const noexcept;
    int ReverseFind() const noexcept = delete;
    int ReverseFind(const CMaaString &substr) const noexcept;
    int ReverseFind(int StartPos, const CMaaString &substr, int EndPos = -1) const noexcept;
    int ReverseFindCi(const CMaaString& substr, int_ cp = 0) const noexcept;
    int ReverseFindCi(int StartPos, const CMaaString &substr, int EndPos = -1, int_ cp = 0) const noexcept;
    int Fill(int StartPos, int Len, char c) noexcept(noexcept_new);
    int Fill(char c = 0) noexcept(noexcept_new);
    int Copy(int pos, const void * ptr, int size) noexcept(noexcept_new);
    int Copy(int pos, const CMaaString &src, int src_offset, int length) noexcept(noexcept_new);

    // Java-style System.arraycopy(src, src_offset, dst, dst_offset, length);
    // --> dst.Copy(src, src_offset, dst_offset, length);
    int Copy(const CMaaString &src, int src_offset, int dst_offset, int length) noexcept(noexcept_new);

    CMaaString UnicodeLittleUnmarked() noexcept(noexcept_new);

    constexpr int GetLength() const noexcept
    {
        return Length();
    }

    // import binary string (can not to have terminating '\0')
    bool Import(const void * ptr, int maxlen) noexcept(noexcept_new); // returns false on alloc error
    // export binary string (can not to add terminating '\0' to ptr, if string length >= maxlen)
    bool Export(void * ptr, int maxlen) const noexcept; // returns false if string was truncated
    /*
    void Empty() noexcept
    {
        //*this = CMaaString {};
        CMaaStringImp* p = m_pImp;
        m_pImp = nullptr;
        if (p)
        {
            if (!p->Release())
            {
#if MANUAL_STRINGS_REFS
                delete p;
#endif
            }
        }
    }
    */
    // returns number of replacements, -1 on new[] error
    int ReplaceNN(char What, char By, int StartPos = 0, int EndPos = -1) noexcept(noexcept_new);
    int ReplaceNN(char What, const CMaaString &By, int StartPos = 0, int EndPos = -1) noexcept(noexcept_new);
    int ReplaceNN(char What, const char * strBy, int StartPos = 0, int EndPos = -1) noexcept(noexcept_new)
    {
        CMaaString By(strBy, eStrlenMemString);
        return ReplaceNN(What, By, StartPos, EndPos);
    }
    int ReplaceNN(int_ N, const char * pWhat, const CMaaString * const pBy, int StartPos = 0, int EndPos = -1) noexcept(noexcept_new);
#ifndef USE_FIND_KMP
    // returns number of replacements, -1 on new[] error
    int ReplaceNN(const CMaaString& What, const CMaaString& By, bool bCountOnly = false, int StartPos = 0, int EndPos = -1) noexcept(noexcept_new);
#else
    int ReplaceNN(const CMaaString &What, const CMaaString &By, bool bCountOnly = false) noexcept(noexcept_new)
    {
        return KMPReplaceNN(What, By, bCountOnly);
    }
#endif
    int ReplaceNN(const char* strWhat, const char* strBy, bool bCountOnly = false) noexcept(noexcept_new)
    {
        CMaaString What(strWhat, eStrlenMemString), By(strBy, eStrlenMemString);
        return ReplaceNN(What, By, bCountOnly);
    }
    int ReplaceNN(const CMaaString &What, const char* strBy, bool bCountOnly = false) noexcept(noexcept_new)
    {
        CMaaString By(strBy, eStrlenMemString);
        return ReplaceNN(What, By, bCountOnly);
    }
    int ReplaceNN(const char* strWhat, const CMaaString &By, bool bCountOnly = false) noexcept(noexcept_new)
    {
        CMaaString What(strWhat, eStrlenMemString);
        return ReplaceNN(What, By, bCountOnly);
    }

    // *pNN - number of replacements, returns CMaaStringZ and *pNN = -1 on new[] error
    CMaaString Replace(char What, char By, bool bStoreResult = true, int *pNN = nullptr) noexcept(noexcept_new);
    // *pNN - number of replacements, returns CMaaStringZ and *pNN = -1 on new[] error
    CMaaString Replace(const CMaaString &What, const CMaaString &By, bool bStoreResult = true, int* pNN = nullptr) noexcept(noexcept_new);
    CMaaString Replace(const char* strWhat, const char* strBy, bool bStoreResult = true, int* pNN = nullptr) noexcept(noexcept_new)
    {
        CMaaString What(strWhat, eStrlenMemString), By(strBy, eStrlenMemString);
        return Replace(What, By, bStoreResult, pNN);
    }
    CMaaString Replace(const CMaaString &What, const char* strBy, bool bStoreResult = true, int* pNN = nullptr) noexcept(noexcept_new)
    {
        CMaaString By(strBy, eStrlenMemString);
        return Replace(What, By, bStoreResult, pNN);
    }
    CMaaString Replace(const char* strWhat, const CMaaString &By, bool bStoreResult = true, int* pNN = nullptr) noexcept(noexcept_new)
    {
        CMaaString What(strWhat, eStrlenMemString);
        return Replace(What, By, bStoreResult, pNN);
    }

#ifdef TOOLSLIB_CHAR8T_SUPPORT
    int ReplaceNN(const char8_t* strWhat, const char8_t* strBy, bool bCountOnly = false) noexcept(noexcept_new)
    {
        CMaaString What(strWhat, eStrlenMemString), By(strBy, eStrlenMemString);
        return ReplaceNN(What, By, bCountOnly);
    }
    int ReplaceNN(const CMaaString &What, const char8_t* strBy, bool bCountOnly = false) noexcept(noexcept_new)
    {
        CMaaString By(strBy, eStrlenMemString);
        return ReplaceNN(What, By, bCountOnly);
    }
    int ReplaceNN(const char8_t* strWhat, const CMaaString &By, bool bCountOnly = false) noexcept(noexcept_new)
    {
        CMaaString What(strWhat, eStrlenMemString);
        return ReplaceNN(What, By, bCountOnly);
    }
#endif
    int KMPReplaceNN(const CMaaString &What, const CMaaString &By, bool bCountOnly = false);
    bool IsSingleOwner() const noexcept
    {
        return !PIMP(m_pImp) || m_pImp->IsSingleOwner();
    }
    bool IsRWSingleOwner() const noexcept
    {
        return !PIMP(m_pImp) || m_pImp->IsRWSingleOwner();
    }
    bool HasReffered() const noexcept
    {
        return PIMP(m_pImp) && m_pImp->HasReffered();
    }
    bool HasAddRefData0() const noexcept
    {
#ifdef CMAASTRING_HAS_ADD_REF_DATA
        return PIMP(m_pImp) && m_pImp->HasAddRefData0();
#else
        return false;
#endif
    }
    bool HasAddRefData1() const noexcept
    {
#ifdef CMAASTRING_HAS_ADD_REF_DATA
        return PIMP(m_pImp) && m_pImp->HasAddRefData1();
#else
        return false;
#endif
    }
    bool Is0Behind() const noexcept
    {
        return !PIMP(m_pImp) || m_pImp->Is0Behind();
    }
    bool IsWC0Behind() const noexcept
    {
        return !PIMP(m_pImp) || m_pImp->IsWC0Behind();
    }
    bool IsWChar0Behind() const noexcept
    {
        return !PIMP(m_pImp) || m_pImp->IsWChar0Behind();
    }
    bool IsWC32_0Behind() const noexcept
    {
        return !PIMP(m_pImp) || m_pImp->IsWC32_0Behind();
    }
    // It is not need to known if the string is 'Memory' string in general way
    bool IsMemoryString() const noexcept
    {
        return PIMP(m_pImp) && m_pImp->IsMemoryString();
    }
    bool IsROString() const noexcept
    {
        return PIMP(m_pImp) && m_pImp->IsROString();
    }
    bool IsMemoryNotROString() const noexcept // is memory, but not const string
    {
        return PIMP(m_pImp) && m_pImp->IsMemoryNotROString();
    }
    bool IsUtf16() const noexcept
    {
#ifdef CMAASTRING_HAS_UTF1632_INFO
        return PIMP(m_pImp) && m_pImp->IsUtf16();
#else
        return false;
#endif
    }
    bool IsUtf32() const noexcept
    {
#ifdef CMAASTRING_HAS_UTF1632_INFO
        return PIMP(m_pImp) && m_pImp->IsUtf32();
#else
        return false;
#endif
    }
    _e1632 GetUtf1632Flags() const noexcept
    {
//#ifdef CMAASTRING_HAS_UTF1632_INFO
        return PIMP(m_pImp) ? m_pImp->GetUtf1632Flags() : eUtf8Flag;
//#else
//        return eUtf8Flag;
//#endif
    }
    bool SetUtf16Flag(bool bSet = true) noexcept
    {
#ifndef CMAASTRING_HAS_UTF1632_INFO
        return PIMP(m_pImp) && m_pImp->SetUtf16Flag(bSet);
#else
        return !bSet;
#endif
    }
    bool SetUtf32Flag(bool bSet = true) noexcept
    {
#ifdef CMAASTRING_HAS_UTF1632_INFO
        return PIMP(m_pImp) && m_pImp->SetUtf32Flag(bSet);
#else
        return !bSet;
#endif
    }
    bool SetUtf1632Flags(_e1632 Flags = eUtf8Flag) const noexcept
    {
#ifdef CMAASTRING_HAS_UTF1632_INFO
        return PIMP(m_pImp) && m_pImp->SetUtf1632Flags(Flags);
#else
        return Flags == eUtf8Flag;
#endif
    }
    bool IsUnAllocString() const noexcept // is empty or unallocated (short) string
    {
        return !PIMP(m_pImp) || m_pImp->IsUnAllocString();
    }
    void SetCryptoKeyBit() noexcept
    {
        if (m_pImp != sp_NullImp)
        {
            m_pImp->SetCryptoKeyBit();
        }
    }
    CMaaString SetCryptoKeyBitEx() noexcept
    {
        SetCryptoKeyBit();
        return *this;
    }
    int_ GetImpVersion() const noexcept // 0 - empty, CMAASTRFL_eImp32 - imp32, CMAASTRFL_eImp64 - imp64, -1 - error
    {
        return !PIMP(m_pImp) ? 0 : m_pImp->GetImpVersion();
    }
    bool ClearPassword() noexcept;
    bool IsMatchMask(const CMaaString &Mask, CMaaPtr_<unsigned char, 1>* pm = nullptr) const; // Case sensitive mask match
    //#ifdef _UNICODE
#if defined(_UNICODE) || defined(__unix__)
    bool IsMatchMask__(const CMaaString &Mask, CMaaPtr_<unsigned char, 1>* pm = nullptr) const {CMaaString u = ::Utf8ToUnicode(*this); return u.IsMatchUnicodeMask(::Utf8ToUnicode(Mask), pm);}
#else
    bool IsMatchMask__(const CMaaString &Mask) {return IsMatchMask(Mask);}
#endif
    bool IsMatchUnicodeMask(const CMaaString &Mask, CMaaPtr_<unsigned char, 1>* pm = nullptr) const;
    bool IsMatchFileMask(const CMaaString &Mask, int_ bCaseSensitive = 0/*-1 - no, 1 - yes, 0 - default*/, CMaaPtr_<unsigned char, 1> *pm = nullptr) const;
    CMaaString ToLower(int_ cp = -1 /*default: +rus*/) const noexcept(noexcept_new); // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    CMaaString ToUpper(int_ cp = -1 /*default: +rus*/) const noexcept(noexcept_new); // 0: acts for englist letters only, 1251 - +rus, e_utf8 (rus+lat), e_utf8_rus (rus+lat)
    int CountChars(char c, int Start = 0, int End = -1) const noexcept;
    int CountChars(const CMaa256Bits& Chars, int Start = 0, int End = -1) const noexcept;
    CMaaString GetArg() noexcept(noexcept_new);
    CMaaString CharToOem() const noexcept(noexcept_new);
    CMaaString OemToChar() const noexcept(noexcept_new);
    int RussianAscii1251CharsCount(int_ CountOne = 0) const noexcept;
    int RussianOemCharsCount(int_ CountOne = 0) const noexcept;
    int Utf8SpecificCharsCount(int_ CountOne = 0) const noexcept;
    int Utf8PartOfCharsLength() const noexcept;
    CMaaString Translit(int_ cp = 1251) const noexcept(noexcept_new);
    CMaaString FromTranslit(int *translated_chars_count = nullptr, int *unchanged_chars_count = nullptr, int_ allowed_untranslated_chars_count = -1) const noexcept(noexcept_new);
    CMaaString Base64Encode(int LineLen = -1, const CMaaString &Lf = CMaaStringCrLf) const noexcept(noexcept_new);
    CMaaString Base64Decode(bool* pbOk = nullptr, int* pValidCharNums = nullptr, const CMaaString &IgnoreChars = CMaaStringCrLf) const noexcept(noexcept_new);
    // web encode generic alphabet based, unused
    CMaaString MyEncode(int LineLen = -1, const CMaaString &Lf = CMaaStringCrLf) const noexcept(noexcept_new);
    CMaaString MyDecode(bool* pbOk = nullptr, int* pValidCharNums = nullptr, const CMaaString &IgnoreChars = CMaaStringCrLf) const noexcept(noexcept_new);
    CMaaString ToMD5() const noexcept(noexcept_new);
    CMaaString DisplayHex(bool bUpper = false) const noexcept(noexcept_new);
    CMaaString FromDisplayedHex(bool *pbErr0 = nullptr) const noexcept(noexcept_new);
    CMaaString RemoveSpaces(bool bAtBegin = true, bool bAtEnd = true, CMaaString Spaces = s_szSpaces) const noexcept(noexcept_new);
    CMaaString RemoveCrLfAtEOL() const noexcept(noexcept_new);
    CMaaString GetWord0(const CMaa256Bits &Bits = gSpacesBits) noexcept(noexcept_new); // unsafe mod *this string
    CMaaString GetWord0(char c) noexcept(noexcept_new); // unsafe mod *this string
    CMaaString GetWord0(CMaaString Spaces) noexcept(noexcept_new) // unsafe mod *this string
    {
        return GetWord0(CMaa256Bits(Spaces, (int_)Spaces.Length()));
    }
    CMaaString GetWord0(const char * pszSpaces) noexcept(noexcept_new) // unsafe mod *this string
    {
        return GetWord0(CMaa256Bits(pszSpaces, (int_)strlen(pszSpaces)));
    }
    CMaaString GetWord(const CMaa256Bits &Bits = gSpacesBits, bool bRemoveWord = true, bool bRemoveSpacesAtTheBeginning = true, bool bOptMid = false) noexcept(noexcept_new);
    CMaaString GetWord(char c, bool bRemoveWord = true, bool bRemoveSpacesAtTheBeginning = true, bool bOptMid = false) noexcept(noexcept_new);
    CMaaString GetWord(bool bRemoveWord, bool bRemoveSpacesAtTheBeginning, CMaaString Spaces, bool bOptMid = false) = delete;
    CMaaString GetWord(bool bRemoveWord, bool bRemoveSpacesAtTheBeginning = true, const char * pszSpaces = " \t", bool bOptMid = false) = delete;
    CMaaString GetWord(CMaaString Spaces /*= " \t"*/, bool bRemoveWord = true, bool bRemoveSpacesAtTheBeginning = true, bool bOptMid = false) noexcept(noexcept_new)
    {
        return GetWord(CMaa256Bits(Spaces, (int_)Spaces.Length()), bRemoveWord, bRemoveSpacesAtTheBeginning, bOptMid);
    }
    CMaaString GetWord(const char * pszSpaces /*= " \t"*/, bool bRemoveWord = true, bool bRemoveSpacesAtTheBeginning = true, bool bOptMid = false) noexcept(noexcept_new)
    {
        return GetWord(CMaa256Bits(pszSpaces, (int_)strlen(pszSpaces)), bRemoveWord, bRemoveSpacesAtTheBeginning, bOptMid);
    }
    bool HaveWord(const char* pszWord, const CMaa256Bits& Bits = gSpacesBits) noexcept;
    bool HaveWord(const CMaaString &strWord, const CMaa256Bits& Bits = gSpacesBits) noexcept;
    bool HaveWordCi(const char* pszWord, const CMaa256Bits& Bits = gSpacesBits, int_ cp = 0) noexcept; // cp: 0, -1, 1251
    bool HaveWordCi(const CMaaString &strWord, const CMaa256Bits& Bits = gSpacesBits, int_ cp = 0) noexcept; // cp: 0, -1, 1251
    /*
    CMaaString GetWord(CMaaString Spaces / * = " \t" * /) noexcept(noexcept_new)
    {
        return GetWord(Spaces, true, true);
    }
    CMaaString GetWord(const char * Spaces / * = " \t" * /) noexcept(noexcept_new)
    {
        return GetWord(Spaces, true, true);
    }
    */
    CMaaString GetCsvWord(CMaaString SrcCodePage = "1251") noexcept(noexcept_new);
    void AppendCsvWord(CMaaString ww, CMaaString DstCodePage = "1251") noexcept(noexcept_new);
    CMaaString GetLine0(/*bool bRemove = true, bool bRemoveCrLf = true, bool bOptimizeMid = true,*/ CMaaString* pCrLf = nullptr) noexcept(noexcept_new); // unsafe mod *this string
    CMaaString GetLine(bool bRemove = true, bool bRemoveCrLf = true, bool bOptimizeMid = true, CMaaString * pCrLf = nullptr) noexcept(noexcept_new);
    CMaaString GetLine(const CMaaString &crlf, bool bRemove = true, bool bRemoveCrLf = true, bool bOptimizeMid = true) noexcept(noexcept_new);
    CMaaString ConvertHttpParam() const noexcept(noexcept_new); // for param name and value, result is utf8 string (usually)
    CMaaString ToHttpHtmlDisplayedText(bool bWeak = false, const CMaaString& strEsc = gsCMaaStringZ) const noexcept(noexcept_new);//CMaaString());
    //CMaaString ToHttpHtmlDisplayedText(bool bWeak, CMaaString strEsc) noexcept(noexcept_new);
    CMaaString Str2HtmlVal(const CMaaString &strEsc = CMaaStringStr2HtmlValEsc /*"&\"'<>"*/) const noexcept(noexcept_new);
    CMaaString ToHttpHtmlAreaText() const noexcept(noexcept_new);
    CMaaString ToHttpHtmlARef() const noexcept(noexcept_new); // CMaaString::sFormat("<a href="%S">%S</a>", &ToHttpHtmlARef(Url), &ToHttpHtmlDisplayedText(Text));
    CMaaString ToHttpHtmlCgiParam() const noexcept(noexcept_new); // ToHttpHtmlCgiParam() and encodeURIComponent() are the same
    CMaaString encodeURIComponent(bool bUppercaseHex = true) const noexcept(noexcept_new); // ToHttpHtmlCgiParam() and encodeURIComponent() are the same
    CMaaString encodeURIComponents(bool bUppercaseHex = true) const noexcept(noexcept_new); // components are '/' separated
    CMaaString decodeURIComponent() const noexcept(noexcept_new); // decodeURIComponent() is UrlWithMixedUtf8ToUtf8(*this)
    bool SplitUrl(CMaaString &UrlPfx, CMaaString &Domain, CMaaString &UrlPath) const noexcept(noexcept_new); // "http://" "суб.дом.рф" "/path" // return true always
#if TOOLSLIB_CMAASTRING64 <= 1
    CMaaString ToCanonicalUrl(CMaaString* pRs = nullptr, CMaaString* pHost = nullptr, CMaaString* pPath = nullptr, CMaaString* pArgs = nullptr, CMaaString* pIntHost = nullptr, bool bIntRet = false) const noexcept(noexcept_new);
    CMaaString IDN_EncodeDomainUrl() const noexcept(noexcept_new);
    CMaaString IDN_DecodeDomainUrl() const noexcept(noexcept_new);
#endif
    CMaaString ToJsArgValue(char quote = 0) const noexcept(noexcept_new);
    CMaaString GetCookie(const char * pszCookieNameEq /*= "COOKIENAME3=" */, bool bOptMid = true) const noexcept(noexcept_new);
    CMaaString QuoteShArg(bool * pbErr = nullptr) const noexcept(noexcept_new);
    bool IsValidVarName() const noexcept;
    bool IsValidPassword() const noexcept;
    bool IsValidPath(int_ Flags = 0) const noexcept; // Is simple path [0-9a-zA-Z_/.]
    CMaaString CutZeroFractionPart(bool bCutLeadingZerosToo = false) const noexcept(noexcept_new);

    int_ dsscanf(const char* fmt, ...) const noexcept;
    int_ dsscanfV(const char* fmt, va_list list) const noexcept;
    int_ sscanfInt(int_ *pRet) const noexcept; // sscanf(ptr,"%d", &x) - like
    int_ ToInt(int_ iDefault = 0, int_ iStrict = 0) const noexcept; // on overflow or errors return iDefault
    _qword ToQword(_qword qDefault = 0, int_ iStrict = 1) const noexcept; // on overflow or errors return qDefault
    
    template<typename T> T ToDouble_internal(T xDefault = 0, bool* pbOk = nullptr, int_ iStrict = 0) const noexcept; // on errors return xDefault
    float ToFloat(float xDefault = 0.0f, bool* pbOk = nullptr, int_ iStrict = 0) const noexcept; // on errors return xDefault
    double ToDouble(double xDefault = 0.0, bool* pbOk = nullptr, int_ iStrict = 0) const noexcept; // on errors return xDefault
    long double ToLongDouble(long double xDefault = 0.0, bool* pbOk = nullptr, int_ iStrict = 0) const noexcept; // on errors return xDefault
    float ToFloat1(float xDefault = 0.0f, bool* pbOk = nullptr) const noexcept; // on errors return xDefault, iStrict = 1
    double ToDouble1(double xDefault = 0.0, bool* pbOk = nullptr) const noexcept; // on errors return xDefault, iStrict = 1
    long double ToLongDouble1(long double xDefault = 0.0, bool* pbOk = nullptr) const noexcept; // on errors return xDefault, iStrict = 1

    /*
    template<std::size_t N> CMaaString& ToString(const char(&txt)[N]) noexcept
    {
        return *this = CMaaString(txt);
    }
    */

    /*
        errorneous compillation (no errors, no warnings):

        CMaaString a1(0, ',');
        CMaaString a2(0ll, ',');
        CMaaString a3(0u, ',');
        CMaaString a4(0ull, ',');

        CMaaString a5(0, 0);
        CMaaString a6(0ll, 0);
        CMaaString a7(0u, 0);
        CMaaString a8(0ull, 0);

        ==> as CMaaString a4(nullptr, ',') /  CMaaString a4(nullptr, 0);
    */
    CMaaString(int_ x) noexcept;
    CMaaString(_qword x) noexcept;
    CMaaString(int_ x, char modem, bool) noexcept; // mode = 0, '+', ',', '+'^','
    CMaaString(_qword x, char mode, bool) noexcept;

    CMaaString(uint_ x) noexcept;
    CMaaString(_uqword x) noexcept;
    CMaaString(uint_ x, char mode, bool) noexcept; // mode = 0, '+', ',', '+'^','
    CMaaString(_uqword x, char mode, bool) noexcept;

    CMaaString(float x) noexcept;
    CMaaString(double x) noexcept;
    CMaaString(long double x) noexcept;

    template<typename T> CMaaString& ToString(const T& x) noexcept
    {
        return *this = CMaaString(x);
    }
    CMaaString& ToString(int_ x, char mode) noexcept // mode = 0, '+', ',', '+'^','
    {
        return *this = CMaaString(x, mode, false);
    }
    CMaaString& ToString(_qword x, char mode) noexcept
    {
        return *this = CMaaString(x, mode, false);
    }
    CMaaString& ToString(uint_ x, char mode) noexcept // mode = 0, '+', ',', '+'^','
    {
        return *this = CMaaString(x, mode, false);
    }
    CMaaString& ToString(_uqword x, char mode) noexcept
    {
        return *this = CMaaString(x, mode, false);
    }

    /*
    template<std::size_t N> static CMaaString sToString(const char(&txt)[N]) noexcept
    {
        return CMaaString(txt);
    }
    */

    template<typename T> static CMaaString sToString(const T& x) noexcept
    {
        return CMaaString(x);
    }

    static CMaaString sToString(int_ x, char mode) noexcept // mode = 0, '+', ',', '+'^','
    {
        return CMaaString(x, mode, false);
    }
    static CMaaString sToString(_qword x, char mode) noexcept
    {
        return CMaaString(x, mode, false);
    }
    static CMaaString sToString(uint_ x, char mode) noexcept // mode = 0, '+', ',', '+'^','
    {
        return CMaaString(x, mode, false);
    }
    static CMaaString sToString(_uqword x, char mode) noexcept
    {
        return CMaaString(x, mode, false);
    }


    int_ sscanfUInt(uint_ *pRet) const noexcept; // sscanf(ptr,"%u", &x) - like
    uint_ ToUInt(uint_ iDefault = 0, int_ iStrict = 0) const noexcept; // on overflow or errors return iDefault
    _uqword ToUQword(_uqword qDefault = 0, int_ iStrict = 1) const noexcept; // on overflow or errors return qDefault

    CMaaString Extention(int_ WithDot = 1) const noexcept;
    CMaaString ReplaceAddExtention(const CMaaString &NewExtention) noexcept(noexcept_new);

    bool is_a_email(int * pErrorPos = nullptr, CMaaString * pResult = nullptr, int_ Flags = 0) const; // Flags: 1 for removing '<' *@* '>', 2 - DigMx, 4 - fail on dig error
    bool is_a_domain(int * pErrorPos = nullptr) noexcept(noexcept_new); // modidied string to lower
    CMaaString ToFsAllowed(bool brus = true, bool bdirectoryes = false, int * pErrNum = nullptr, int_ fCutPath = -1) const noexcept(noexcept_new);
    CMaaString ToFsAndShellAllowed(bool brus = true, bool bdirectoryes = false, int * pErrNum = nullptr) const noexcept(noexcept_new);

    bool ListHas(const CMaaString &prop, const CMaaString &d = CMaaStringSemicolon) const noexcept(noexcept_new);
    bool ListNotHas(const CMaaString &prop, const CMaaString &d = CMaaStringSemicolon) const noexcept(noexcept_new);
    bool ListAdd(const CMaaString &prop, int MaxLen = -1, const CMaaString &d = CMaaStringSemicolon) noexcept(noexcept_new);
    bool ListRem(const CMaaString &prop, const CMaaString &d = CMaaStringSemicolon) noexcept(noexcept_new);
    CMaaString List2Sql(const CMaaString &d = CMaaStringSemicolon) const noexcept(noexcept_new);
    CMaaString List4Sql(const CMaaString &d = CMaaStringSemicolon) const noexcept(noexcept_new);
    //CMaaString ListGetFirst(bool bRemove = true, CMaaString d = CMaaStringSemicolon);
    CMaaString Pass1Replace(CMaaUnivHash<CMaaString, CMaaString> &hVars, CMaaString Prefix = "{{{{{", CMaaString Suffix = "}}}}}") const noexcept(noexcept_new);
    CMaaString Utf8ToUtf8_3_Compatible(char chReplace = '_', bool bCut = false) const noexcept;
    CMaaString Utf8ToUtf8_4_Compatible(char chReplace = '_', bool bCut = false) const noexcept;
    CMaaString ToCSourceString() const noexcept(noexcept_new);

    //-----------------------------------------------------------------------------------------
    // for http req/resp: "GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n" / "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n", etc
    CMaaString HttpGetValue(const char* Name, int* pos = nullptr, int* nextpos = nullptr) const;
    CMaaString HttpGetValue(const CMaaString& Name, int* pos = nullptr, int* nextpos = nullptr) const;
    //-----------------------------------------------------------------------------------------
    // for cgi resp: "Content-Length: 0\r\n\r\n" / "Status: 200\r\nContent-Length: 0\r\n\r\n", etc
    CMaaString CgiGetValue(const char* Name, int* pos = nullptr, int* nextpos = nullptr) const;
    CMaaString CgiGetValue(const CMaaString& Name, int* pos = nullptr, int* nextpos = nullptr) const;
    //-----------------------------------------------------------------------------------------

    struct XX
    {
        CMaaString &s;
        int idx;
        bool bConst;

        constexpr XX(CMaaString &x, int n) noexcept
        :   s(x),
            idx(n),
            bConst(false)
        {
        }
        constexpr XX(const CMaaString &x, int n) noexcept
        :   s((CMaaString &)x),
            idx(n),
            bConst(true)
        {
        }
        constexpr operator const char& () const noexcept
        {
            return (PIMP(s.m_pImp) && idx < s.m_pImp->m_iLength && idx >= 0) ? s.m_pImp->m_pszStr[idx] : sChar0_2[0]; // (sChar0_2[0] = 0);
        }
        constexpr const char * operator& () const noexcept
        {
            return &(const char &)*this;
        }
        const char& operator=(char c) noexcept(noexcept_new)
        {
            if  (PIMP(s.m_pImp) && s.Length() > idx && idx >= 0 && !bConst)
            {
                if  (!s.m_pImp->IsRWSingleOwner())
                {
                    CMaaString temp = s.NewCopy();
                    if (temp.m_pImp != sp_NullImp)
                    {
                        s = std::move(temp);
                    }
                }
                if  (PIMP(s.m_pImp) && s.Length() > idx && s.m_pImp->IsRWSingleOwner())
                {
                    return *(idx + s.GetBuffer()) = c;
                }
            }
            static char c2;
            c2 = c;
            return c2;
        }
        const char& operator=(const XX& That) noexcept(noexcept_new)
        {
            return *this = (char)That;
        }
        /*
          bool operator == (const XX &That) const {return (char)*this == (char)That;}
          bool operator != (const XX &That) const {return (char)*this != (char)That;}
          bool operator > (const XX &That) const {return (char)*this > (char)That;}
          bool operator < (const XX &That) const {return (char)*this < (char)That;}
          bool operator >= (const XX &That) const {return (char)*this >= (char)That;}
          bool operator <= (const XX &That) const {return (char)*this <= (char)That;}
        */
    };

    constexpr XX operator[](int_ idx) noexcept
    {
        return XX(*this, idx);
    }

    constexpr XX operator[](_qword idx) noexcept
    {
        return XX(*this, (int)idx);
    }

    constexpr XX operator[](size_t idx) noexcept
    {
        return XX(*this, (int)idx);
    }

    constexpr const char& operator[](int_ idx) const noexcept // rarely used
    {
        return (PIMP(m_pImp) && idx < m_pImp->m_iLength && idx >= 0) ? m_pImp->m_pszStr[idx] : sChar0_2[0]; // (sChar0_2[0] = 0);
    }
    constexpr const char& operator[](_qword idx) const noexcept // rarely used
    {
        return (PIMP(m_pImp) && idx < m_pImp->m_iLength && idx >= 0) ? m_pImp->m_pszStr[idx] : sChar0_2[0]; // (sChar0_2[0] = 0);
    }
    constexpr const char& operator[](size_t idx) const noexcept // rarely used
    {
        return (PIMP(m_pImp) && idx < (size_t)m_pImp->m_iLength) ? m_pImp->m_pszStr[idx] : sChar0_2[0]; // (sChar0_2[0] = 0);
    }

    static int GetMaxUnAllocLength() noexcept;
    static int GetMaxUnAllocLengthWithPadding() noexcept;

#ifndef __TOOLSLIB_REMOVE_OLD_CODE
    static void StrCutSlash(char * Str);
    static void StrAddSlash(char * Str);
#endif

#ifdef CMAASTRING_HAS_ADD_DATA
    const char * SetAdditionalData(const char * ptr, int_ length, int_ bDefaultNullAccept = 0) noexcept(noexcept_new);
    _WC_ * SetAdditionalData(const _WC_ * ptr, int_ length, int_ bDefaultNullAccept = 0) noexcept(noexcept_new);
    _WC_ * Utf8ToUnicode(int_ bDefaultNullAccept = -1) noexcept(noexcept_new);
    const char * GetAdditionalDataA(int_ bDefaultNullAccept = -1, int_ * pchsize = nullptr) const noexcept;
    _WC_ * GetAdditionalDataW(int_ bDefaultNullAccept = -1, int_ * pwsize = nullptr) const noexcept;
#endif
protected:
    CMaaString& Sprintf(const char* strFormat, int FormatLen, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf(const char * Format, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf(CMaaString strFormat, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf2(const char* strFormat, int FormatLen, const char* strText, int TextLen, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf2(const char* Format, const char* Text, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf2(CMaaString strFormat, const char *Text, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf2(const char * Format, CMaaString strText, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;
    CMaaString& Sprintf2(CMaaString strFormat, CMaaString strText, va_list list, int_ SrcLine = -1, const char* SrcFile = "unknown") noexcept;

private:
//public:
    //struct CMaaStringImp { ... }

    friend struct CMaaString::XX;

    //CMaaStringImp* m_pImp;

    static void AssignImpFast(CMaaStringImp * &a, CMaaStringImp * &b, bool bInc, bool bDel = MANUAL_STRINGS_REFS) noexcept
    {
#ifdef CHK_ERR_NULL_PIMP
        if (!b)
        {
            //printf("AssignImpFast() - err b == NULL\n");
            b = sp_NullImp;
        }
#endif
        //gCMaaSyncAssignLock.Lock();
        if (bInc)
        {
#ifdef TOOLSLIB_A_A_ASSIGN_OPT
            if (b == a)
            {
                return; // 03.11.2024
            }
#endif
            b != sp_NullImp && b->AddRef();
            INC_TOOLSLIB_STAT_STRING_IMP_REFS(b != sp_NullImp);
        }
        CMaaStringImp * tmp = a;
        a = b;
        if  (tmp != sp_NullImp)
        {
            DEC_TOOLSLIB_STAT_STRING_IMP_REFS;
            if  (!tmp->Release() && bDel)
            {
                delete tmp;
            }
        }
        //gCMaaSyncAssignLock.UnLock();
    }

    ADD_ALLOCATOR(CMaaString)
};

template <> void CMaaSwap<CMaaString>(CMaaString& a, CMaaString& b)  noexcept;

//-----------------------------------------------------------------------------------------
#if 0
// for http req/resp: "GET / HTTP/1.1\r\nContent-Length: 0\r\n\r\n" / "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n", etc
CMaaString HttpGetValue(const CMaaString& Hdr, const char* Name, int* pos = nullptr, int* nextpos = nullptr);
CMaaString HttpGetValue(const CMaaString& Hdr, const CMaaString& Name, int* pos = nullptr, int* nextpos = nullptr);
//-----------------------------------------------------------------------------------------
// for cgi resp: "Content-Length: 0\r\n\r\n" / "Status: 200\r\nContent-Length: 0\r\n\r\n", etc
CMaaString CgiGetValue(const CMaaString& Hdr, const char* Name, int* pos = nullptr, int* nextpos = nullptr);
CMaaString CgiGetValue(const CMaaString& Hdr, const CMaaString& Name, int* pos = nullptr, int* nextpos = nullptr);
#endif
//-----------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//inline CMaaString CMaaStringRO(const char* x) noexcept { return CMaaString(x, CMaaString::eROStrlenMemString); }
//------------------------------------------------------------------------------
#if TOOLSLIB_CMAASTRING64 <= 1
#define CMaaStringRO(x) CMaaString(x, CMaaString::eROStrlenMemString)
//------------------------------------------------------------------------------
inline CMaaString operator""_s(const char* __str, size_t __len) noexcept { return CMaaString(__str, __len, CMaaString::eROMemString0); }
#define CMaaStringRO_s(x) x##_s
//------------------------------------------------------------------------------
#endif

template<> int CMaaString::FindN_<0>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex) const noexcept;
template<> int CMaaString::FindN_<1>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex) const noexcept;
template<> int CMaaString::FindN<0>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags) const noexcept;
template<> int CMaaString::FindN<1>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags) const noexcept;

template<> int CMaaString::FindN_Ci<0>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex, int_ cp) const noexcept;
template<> int CMaaString::FindN_Ci<1>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, int_ Flags, const int_* Index, int_* pIndex, int_ cp) const noexcept;
template<> int CMaaString::FindNCi<0>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags, int_ cp) const noexcept;
template<> int CMaaString::FindNCi<1>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, int_ Flags, int_ cp) const noexcept;

template<> int CMaaString::KMPFindN_<0>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, const int** pf, int_ Flags, const int_* Index, int_* pIndex) const noexcept;
template<> int CMaaString::KMPFindN_<1>(int StartPos, int_ n, const char** SubStr, const int* SubStrLen, int EndPos, const int** pf, int_ Flags, const int_* Index, int_* pIndex) const noexcept;
template<> int CMaaString::KMPFindN<0>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, sKMPFindNCalcData<0>* f, int_ Flags) const noexcept;
template<> int CMaaString::KMPFindN<1>(int StartPos, const char** SubStr, int* SubStrLen, int_* pIndex, int EndPos, sKMPFindNCalcData<1>* f, int_ Flags) const noexcept;

//------------------------------------------------------
template<class T> const CMaaString & CMaaStr0() noexcept
{
    static const T ch{};
    static const CMaaString s(&ch, (int_)sizeof(ch), CMaaString::eROMemString);
    return s;
}
template<> const CMaaString & CMaaStr0<void>() noexcept;
template<class T, T c> const CMaaString& CMaaStrCh() noexcept
{
    static const T ch{c};
    static const CMaaString s(&ch, (int_)sizeof(ch), CMaaString::eROMemString);
    return s;
}

inline const CMaaString & CMaaStrCh(char C, bool bInit = false) noexcept
{
    static char ch[2 * 256];
    static CMaaString s[256];
    if (bInit && !ch[2 * 255])
    {
        for (int_ i = 0; i < 256; i++)
        {
            ch[2 * i] = (char)i;
            //ch[2 * i + 1] = '\0';
            s[i] = CMaaString(&ch[2 * i], 1, CMaaString::eROMemString0);
        }
    }
    return s[(unsigned char)C];
}
//------------------------------------------------------
/*
#define CMaaString_0 CMaaStrCh('0')
#define CMaaString_1 CMaaStrCh('1')
#define CMaaString_2 CMaaStrCh('2')
#define CMaaString_3 CMaaStrCh('3')
#define CMaaString_4 CMaaStrCh('4')
#define CMaaString_5 CMaaStrCh('5')
#define CMaaString_6 CMaaStrCh('6')
#define CMaaString_7 CMaaStrCh('7')
#define CMaaString_8 CMaaStrCh('8')
#define CMaaString_9 CMaaStrCh('9')
#define CMaaString_a CMaaStrCh('a')
#define CMaaString_z CMaaStrCh('z')
#define CMaaString_A CMaaStrCh('A')
#define CMaaString_Z CMaaStrCh('Z')
*/

#if TOOLSLIB_CMAASTRING64 <= 1

#if defined(_WIN32) && defined(_UNICODE)
#define _uc_Utf8ToUnicode_str(x) Utf8ToUnicode(x)
#define _uc_Utf8ToUnicode(x) (_WC_*)(const char *)Utf8ToUnicode(x)
#else
#define _uc_Utf8ToUnicode_str(x) CMaaString(x)
#define _uc_Utf8ToUnicode(x) (const char *)(x)
#endif

#ifdef __unix__
#define CP_ACP 1251
#endif

#endif

CMaaString UnicodeToAnsi(const CMaaString &Text, bool RemoveLastNullChar = true, _dword cp = CP_ACP) noexcept(noexcept_new);
CMaaString AnsiToUnicode(const CMaaString &Text, bool AddTerminatingNullChar = false, _dword cp = CP_ACP) noexcept(noexcept_new);

inline CMaaString Utf8ToAnsi(const CMaaString& Text, int_ bDefaultNullAccept = false) noexcept(noexcept_new) { return UnicodeToAnsi(std::move(Utf8ToUnicode(Text, bDefaultNullAccept ? 1 : 0)), false); }
//inline CMaaString AnsiToUtf8(const CMaaString& Text, int_ AddTerminatingNullChar  = false) { return UnicodeToUtf8(std::move(AnsiToUnicode(Text, AddTerminatingNullChar ? 1 : 0)), false); }

#ifdef CMaaStringReplace
CMaaString Export(const void * p, int len);
//static CMaaString DataToString(const void * ptr, int len);
//#define CTOOString CMaaString
#endif

CMaaString Export(const CMaaString& Data);

/*
#ifdef __TOOLSLIB_TEMP_H
inline CMaaString GetSourceTimestamp() noexcept { return __TIMESTAMP__; }
#endif
*/
// "Mon Jun 4 09:38:30 2007" --> "Build date: Mon, 4.07.2007 09:38:30"
// __TIMESTAMP__ is a source file modification date, not a date of compillation
CMaaString GetBuildDate(CMaaString TimeStamp /* __TIMESTAMP__ */, const char** Dow = nullptr /* g_pszRusDayOfWeekLong */, const char* pszBuildDate = "Build date: " /* "Время сборки: " */) noexcept;

// for CMS
CMaaString GetHumanLikeTextDate(time_t t, int_ mode /*=0*/, const CMaaString* const Month, const CMaaString& strY /* = " г." */, const CMaaString* const Dow = nullptr) noexcept; // Dow[0] = "Mon"
// new, for other
CMaaString GetHumanLikeTextDate(time_t t, int_ mode /*=0*/, const char** Month, const CMaaString& strY = gsCMaaStringZ /* = " г." */, const char** Dow = nullptr, bool bTime = false) noexcept;  // Dow[0] = "Sun"

time_t ConvertTextDateTime(const CMaaString& Date, const CMaaString& Time, int_ mode = 0, bool bStrong = false) noexcept;
time_t ConvertTextDateTime(const CMaaString& DateTime, int_ mode = 0, bool bStrong = false) noexcept;
CMaaString GetFromToMailText(const CMaaString& TextUtf8UserName, const CMaaString& EMail) noexcept(noexcept_new);
CMaaString MakeMailSubject(const CMaaString& Utf8Text) noexcept(noexcept_new);
CMaaString GetMailSubject(const CMaaString& Subject) noexcept(noexcept_new);
CMaaString MakeMailText(CMaaString Text, int LineLen = 72) noexcept(noexcept_new);
bool ConvertFromWebDateTime(const CMaaString& s, time_t* t) noexcept;
bool ConvertFromApacheDateTime(const CMaaString& s, time_t* t) noexcept;
bool CMaaIsValidDate(const CMaaString& Date, const CMaaString& Time = gsCMaaStringZ, int_ mode = -1); // -1 - both 0,2 modes, '/','S' are not
bool CMaaIsValidDateTime(CMaaString dt, int_ mode = -1); // dt - is valid or empty
_qword GetHexNum(const char* ptr, int len = -1, int* pStopPos = nullptr, CMaaString StopChars = CMaaString0, bool* pbError = nullptr) noexcept;
_qword GetHexNum(const char* ptr, int len /*= -1*/, _qword ErrorRetValue /*= -1*/, int* pStopPos = nullptr, CMaaString StopChars = CMaaString0, bool* pbError = nullptr) noexcept;
CMaaString UrlWithMixedUtf8ToUtf8(const CMaaString& Text) noexcept(noexcept_new); // noexept // exceptions are suppressed
CMaaString UrlWithMixedUtf8ToAscii(const CMaaString& Text) noexcept(noexcept_new);
CMaaString CRAM_MD5_Auth(const CMaaString& Base64Request, const CMaaString& email, CMaaString pass) noexcept(noexcept_new);
time_t CMaaMkTime(tm* tt1) noexcept;
time_t CMaaMkTimeOrig(tm* tt1) noexcept;

#ifndef CMAASTRING64___

extern const char* g_pszEngMonth[12];
extern const char* g_pszEngDayOfWeek[8];
extern const char* g_pszEngMonthLong[12];
extern const char* g_pszEngDayOfWeekLong[8];

extern const char* g_pszRusMonth[12];
extern const char* g_pszRusDayOfWeek[8];
extern const char* g_pszRusMonthLong[12];
extern const char* g_pszRusMonthLong2[12];
extern const char* g_pszRusDayOfWeekLong[8];

CMaaString GetLogDatePath(time_t t) noexcept;
CMaaString GetTextDate(time_t t, int_ mode = 0) noexcept;
CMaaString GetTextTime(time_t t) noexcept;
CMaaString GetTextDateTime(time_t t, int_ mode = 0) noexcept; // mode | 0x100 -> TimeDate
inline CMaaString GetTextTimeDate(time_t t, int_ mode = 0) noexcept { return GetTextDateTime(t, mode | 0x100); }
CMaaString GetTextDateOrTimeToday(time_t t, time_t current_time = time(nullptr), int_ mode = 0) noexcept;
CMaaString GetTextTimeDiff(double x) noexcept;  // x - time difference int secounds, by example diff of GetTickCount()/1000.0
CMaaString GetTextTimeDiff3(double x) noexcept; // x - time difference int secounds, by example diff of GetTickCount()/1000.0
inline CMaaString CMaaSqlDate(time_t t) noexcept { return GetTextDate(t, 'S'); } // "2022-12-31"
CMaaString GetDateMailFormat(time_t t = time(nullptr)) noexcept;
CMaaString GetDateRKNFormat(time_t t = time(nullptr)) noexcept;
CMaaString GetDateTimeYandexdFormat(time_t t = time(nullptr));
CMaaString GetDateTimeYandexdFormatForMWS(time_t t = time(nullptr));
CMaaString GetDateTimeNalogFormat(time_t t = time(nullptr));
CMaaString GetW3CDateTime(time_t t = time(nullptr)); // for sitemaps
CMaaString GetDateApacheFormat(time_t t = time(nullptr)) noexcept;
CMaaString GetDateWebFormat(time_t t) noexcept;

CMaaString GetTextSizeKBMBGB(_qword q, const char * m[] = nullptr) noexcept;
// const char * mr[6] = {"Байт", "КБ", "МБ", "ГБ", "ТБ", nullptr}; // default
// const char * me[6] = {"Bytes", "KB", "MB", "GB", "TB", nullptr};
// CMaaString sz = GetTextSizeKBMBGB(q, me);

bool CMaaGetStartTime(time_t &t, char c) noexcept; // c is 'y' 'm' 'd' 'h', '.'
bool CMaaGetNextTime(time_t &t, char c) noexcept; // c is 'y' 'm' 'd' 'h', '.'

//<div class="_additional_info_" align="right" style="font-family: 'Times New Roman,font1'; font-size: 14px; text-decoration: none;">
//Copyright &copy; 2011  <a href="http://www.maasoftware.ru/" style="font-family: 'Times New Roman,font1'; font-size: 14px; font-weight: bold; color: #FF4040; text-decoration: none;" target="_top">MaaSoftware</a>&nbsp;</div>

//CMaaString GetCopyrightText(int StartYear = 0, const char * format = "Copyright &copy; %1 MaaSoftware", time_t t = time(nullptr));
CMaaString GetCopyrightText(int_ StartYear = 0, const char * format = "Copyright &copy; %1 "
"<a href=\"http://maasoft.ru/\" style=\"font-family: 'Times New Roman,font1'; font-size: 14px; font-weight: bold; color: #FF4040; text-decoration: none;\" target=\"_top\">"
"MaaSoftware"
"</a>"
, time_t t = time(nullptr)) noexcept;

#endif

#if (TOOLSLIB_USE_CMAASTRING64 < 2) || (TOOLSLIB_CMAASTRING64 > 0)
//#define CMaaConcatString_TEST

template <int xThrow = 0, bool bCountMode = false> class CMaaConcatString_
{
    char * m_ptr;
    CMaaPtr_<char, xThrow> m_Buffer;
    size_t m_StringLength, m_BufferSize;
    bool m_bIsValid;
public:
    class Helper
    {
    protected:
        CMaaConcatString_<xThrow, bCountMode>& m;
    public:
        Helper(CMaaConcatString_<xThrow, bCountMode>& x) noexcept
        :   m(x)
        {
        }
        ~Helper() {}
        Helper(const Helper& That) noexcept
        :   m(That.m)
        {
            TOOLSLIB_STR_HELPER_printf("CH(c CH &)\n")
        }
        Helper(Helper&& That) noexcept
        :   m(That.m)
        {
            TOOLSLIB_STR_HELPER_printf("CH(CH &&)\n")
        }
        Helper& operator=(const Helper& That) noexcept = delete;
        Helper& operator=(Helper&& That) noexcept = delete;
        template<class S> Helper& operator +=(const S& txt) noexcept(xThrow <= 0 || bCountMode)
        {
            TOOLSLIB_STR_HELPER_printf("CH::op +=(c S&)\n")
            m += txt;
            return *this;
        }
        template<class S> Helper& operator +(const S& txt) noexcept(xThrow <= 0 || bCountMode)
        {
            TOOLSLIB_STR_HELPER_printf("CH::op +(c S&)\n")
            m += txt;
            return *this;
        }
        Helper& operator -=(int n) noexcept
        {
            TOOLSLIB_STR_HELPER_printf("CH::op -=(n)\n")
            m -= n;
            return *this;
        }
        Helper& operator -(int n) noexcept
        {
            TOOLSLIB_STR_HELPER_printf("CH::op -(n)\n")
            m -= n;
            return *this;
        }
    };
    //old: CMaaConcatString_(int ApproxLen = 1024, int_ xThrow = 0, bool bCountMode = false)
    CMaaConcatString_(int ApproxLen = 1024) noexcept(xThrow <= 0 || bCountMode)
    :   m_Buffer(bCountMode ? 0 : ApproxLen >= 0 ? ApproxLen : 1024),
        m_StringLength(0)
    {
        m_bIsValid = !bCountMode && m_Buffer.IsValid();
        m_ptr = m_Buffer;
        m_BufferSize = m_Buffer.Size();
    }

    //old: CMaaConcatString_(char * pExtBuffer, size_t ExtBufferSize, int ApproxLen = 1024, int_ xThrow = 0, bool bCountMode = false)
    CMaaConcatString_(char* pExtBuffer, size_t ExtBufferSize, int ApproxLen = 1024) noexcept(xThrow <= 0 || bCountMode)
    :   m_ptr(pExtBuffer),
        m_Buffer(!bCountMode && ApproxLen >= 0 && ((size_t)ApproxLen > ExtBufferSize || !pExtBuffer) ? ApproxLen : 0),
        m_StringLength(0)
    {
        if constexpr (bCountMode)
        {
            m_bIsValid = false;
            m_BufferSize = 0;
        }
        else if (ApproxLen >= 0 && ((size_t)ApproxLen > ExtBufferSize || !pExtBuffer))
        {
            m_bIsValid = m_Buffer.IsValid();
            m_ptr = m_Buffer;
            m_BufferSize = m_Buffer.Size();
        }
        else
        {
            m_bIsValid = pExtBuffer != nullptr;
            if (!pExtBuffer && ExtBufferSize)
            {
                if constexpr (xThrow > 0)
                {
                    CMaaPtr_THROW1_BAD_IMP;
                }
                /*else if (!xThrow)
                {
                    CMaaPtr_THROW_BAD_IMP;
                }*/
                m_BufferSize = 0;
            }
            else
            {
                m_BufferSize = ExtBufferSize;
            }
        }
    }

    void Swap(CMaaConcatString_<xThrow, bCountMode>& That) noexcept
    {
        CMaaSwap(m_ptr, That.m_ptr);
        m_Buffer.Swap(That.m_Buffer);
        CMaaSwap(m_StringLength, That.m_StringLength);
        CMaaSwap(m_BufferSize, That.m_BufferSize);
        CMaaSwap(m_bIsValid, That.m_bIsValid);
    }
    CMaaConcatString_(const CMaaConcatString_<xThrow, bCountMode>& That) noexcept // =delete
    :   //m_Buffer(0),
        m_StringLength(0)
    {
        m_bIsValid = false;
        m_ptr = m_Buffer;
        m_BufferSize = m_Buffer.Size();
        Swap((CMaaConcatString_<xThrow, bCountMode>&)That);
    }
    CMaaConcatString_<xThrow>& operator=(const CMaaConcatString_<xThrow, bCountMode>& That) noexcept // =delete
    {
        Swap((CMaaConcatString_<xThrow, bCountMode>&)That);
        return *this;
    }
    CMaaConcatString_(CMaaConcatString_<xThrow, bCountMode>&& That) noexcept = delete;
    CMaaConcatString_<xThrow, bCountMode>& operator=(CMaaConcatString_<xThrow, bCountMode>&& That) noexcept = delete;

    constexpr size_t GetLength() const noexcept
    {
        return m_StringLength;
    }
    constexpr size_t Length() const noexcept
    {
        return m_StringLength;
    }
    constexpr bool IsValid() const noexcept
    {
        return bCountMode || m_bIsValid;
    }
    void Empty(bool bResizeBuffer = false) noexcept
    {
        if constexpr (!bCountMode)
        {
            if ((bResizeBuffer || !m_bIsValid))
            {
                CMaaPtr_<char, xThrow> Buffer;
                m_Buffer.Swap(Buffer);
                m_bIsValid = true; // !bCountMode && m_Buffer.IsValid();
                m_ptr = m_Buffer;
                m_BufferSize = 0;
            }
        }
        m_StringLength = 0;
    }
    bool SetNewLengthValue(size_t Length) noexcept
    {
        if  (IsValid() && Length <= m_StringLength)
        {
            m_StringLength = Length;
            return true;
        }
        return false;
    }
    template<class S> Helper Append(S format, ...) noexcept((xThrow <= 0 || bCountMode) && noexcept(S(format)))
    {
#if TOOLSLIB_USE_CMAASTRING64 == 2
        CMaaString32 txt;
#else
        CMaaString txt;
#endif

        va_list list;
        va_start(list, format);
        txt._FormatV(-1, "unknown", format, list);
        va_end(list);

        Add(txt, txt.Length());
        return *this;
    }
#if 0
    template<> Helper Append<const char*>(const char *format, ...) noexcept((xThrow <= 0 || bCountMode))
    {
        CMaaString32 txt;

        va_list list;
        va_start(list, format);
        txt._FormatV(-1, "unknown", format, list);
        va_end(list);

        Add(txt, txt.Length());
        return *this;
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    template<> Helper Append<const char8_t*>(const char8_t* format, ...) noexcept((xThrow <= 0 || bCountMode))
    {
        CMaaString32 txt;

        va_list list;
        va_start(list, format);
        txt._FormatV(-1, "unknown", format, list);
        va_end(list);

        Add(txt, txt.Length());
        return *this;
    }
#endif
#endif
    template<class S1, class S2> Helper Append2(const S1& format, S2 text, ...) noexcept((xThrow <= 0 || bCountMode) && noexcept(S2(text)))
    {
#if TOOLSLIB_USE_CMAASTRING64 == 2
        CMaaString32 txt;
#else
        CMaaString txt;
#endif

        va_list list;
        va_start(list, text);
        txt._FormatV2(-1, "unknown", format, text, list);
        va_end(list);

        Add(txt, txt.Length());
        return *this;
    }
#if 0
    typedef char * pchar1;
    template<> Helper Append2<pchar1, const char*>(const pchar1& format, const char *text, ...) noexcept((xThrow <= 0 || bCountMode))
    {
        CMaaString32 txt;

        va_list list;
        va_start(list, text);
        txt._FormatV2(-1, "unknown", format, text, list);
        va_end(list);

        Add(txt, txt.Length());
        return *this;
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    typedef char* pchar1;
    template<> Helper Append2<pchar1, const char8_t*>(const pchar1& format, const char8_t* text, ...) noexcept((xThrow <= 0 || bCountMode))
    {
        CMaaString32 txt;

        va_list list;
        va_start(list, text);
        txt._FormatV2(-1, "unknown", format, text, list);
        va_end(list);

        Add(txt, txt.Length());
        return *this;
    }
#endif
#endif
    void operator -= (int n) noexcept
    {
        TOOLSLIB_STR_HELPER_printf("C::op-=(n)\n")
        n = n >= 0 ? n : -n;
        m_StringLength = m_StringLength >= n ? m_StringLength - n : 0;
    }
    void operator += (const CMaaString& str) noexcept(xThrow <= 0 || bCountMode)
    {
        TOOLSLIB_STR_HELPER_printf("C::op+=(c S&)\n")
        Add(str, str.Length());
    }
#if TOOLSLIB_USE_CMAASTRING64 == 2
    void operator += (const CMaaString32& str) noexcept(xThrow <= 0 || bCountMode)
    {
        Add(str, str.Length());
    }
#endif
    void operator += (const CMaaConstStr& str) noexcept(xThrow <= 0 || bCountMode)
    {
        Add(str.p, str.len);
    }
    void operator += (const char* str) noexcept(xThrow <= 0 || bCountMode)
    {
        Add(str, (int)strlen(str));
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    void operator += (const char8_t* str) noexcept(xThrow <= 0 || bCountMode)
    {
        Add(str, int_strlen(str));
    }
#endif
    void operator += (char c) noexcept(xThrow <= 0 || bCountMode)
    {
        if constexpr (!bCountMode)
        {
            Required(1);
            if (m_bIsValid)
            {
                m_ptr[m_StringLength] = c;
            }
        }
        m_StringLength++;
    }
    void operator += (unsigned char c) noexcept(xThrow <= 0 || bCountMode)
    {
        operator += ((char)c);
    }
    void Add(char c) noexcept(xThrow <= 0 || bCountMode)
    {
        operator += (c);
    }
    void Add(unsigned char c) noexcept(xThrow <= 0 || bCountMode)
    {
        operator += ((char)c);
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    void operator += (char8_t c) noexcept(xThrow <= 0 || bCountMode)
    {
        operator += ((char)c);
    }
#endif
    void Add(const char* str, int len) noexcept(xThrow <= 0 || bCountMode)
    {
        if (len <= 0)
        {
            return;
        }
        if constexpr (!bCountMode)
        {
            Required(len);
            if (m_bIsValid)
            {
                memmove(m_StringLength + m_ptr, str, len);
            }
        }
        m_StringLength += len;
    }
    void Add(char c, int len) noexcept(xThrow <= 0 || bCountMode)
    {
        if (len <= 0)
        {
            return;
        }
        if constexpr (!bCountMode)
        {
            Required(len);
            if (m_bIsValid)
            {
                memset(m_StringLength + m_ptr, (unsigned char)c, len);
            }
        }
        m_StringLength += len;
    }
    void Add(unsigned char c, int len) noexcept(xThrow <= 0 || bCountMode)
    {
        Add((char)c, len);
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    void Add(const char8_t* str, int len) noexcept(xThrow <= 0 || bCountMode)
    {
        Add((const char*)str, len);
    }
#endif
#if TOOLSLIB_USE_CMAASTRING64 < 2
    operator CMaaString() const noexcept(noexcept_new && noexcept(GetMemoryString()))
    {
        return GetMemoryString().NewCopy();
    }
    CMaaString GetMemoryString() const noexcept(xThrow < 0)
    {
        if  (m_bIsValid && m_StringLength <= CMaaString::sMaxStringLen)
        {
            CMaaString str(m_ptr, (StrInt)m_StringLength, CMaaString::eMemString); // Must be not RO string for += support
            if constexpr (xThrow >= 0)
            {
                if  ((size_t)str.Length() != m_StringLength)
                {
                    CMaaPtr_THROW1;
                }
            }
            return str;
        }
        if constexpr (xThrow >= 0)
        {
            CMaaPtr_THROW1;
        }
        return CMaaStringZ;
    }
    CMaaString str2() const noexcept //(noexcept_new)
    {
        try
        {
            return GetMemoryString2().NewCopy();
        }
        catch (...)
        {
        }
        return CMaaStringZ;
    }
    CMaaString GetMemoryString2() const noexcept
    {
        if (m_bIsValid && m_StringLength <= CMaaString::sMaxStringLen)
        {
            return CMaaString(m_ptr, (StrInt)m_StringLength, CMaaString::eMemString); // Must be not RO string for += support
        }
        return CMaaStringZ;
    }
#else
    operator CMaaString32() const noexcept(noexcept_new && noexcept(GetMemoryString()))
    {
        return GetMemoryString().NewCopy();
    }
    CMaaString32 GetMemoryString() const noexcept(xThrow < 0)
    {
        if (m_bIsValid && m_StringLength <= CMaaString32::sMaxStringLen)
        {
            CMaaString32 str(m_ptr, (int_)m_StringLength, CMaaString32::eMemString); // Must be not RO string for += support
            if constexpr (xThrow >= 0)
            {
                if ((size_t)str.Length() != m_StringLength)
                {
                    CMaaPtr_THROW1;
                }
            }
            return str;
        }
        if constexpr (xThrow >= 0)
        {
            CMaaPtr_THROW1;
        }
        return CMaaString32Z;
    }
    CMaaString32 str2() const noexcept //(noexcept_new)
    {
        try
        {
            return GetMemoryString2().NewCopy();
        }
        catch (...)
        {
        }
        return CMaaString32Z;
    }
    CMaaString32 GetMemoryString2() const noexcept
    {
        if (m_bIsValid && m_StringLength <= CMaaString32::sMaxStringLen)
        {
            return CMaaString32(m_ptr, (int_)m_StringLength, CMaaString32::eMemString); // Must be not RO string for += support
        }
        return CMaaString32Z;
    }

    operator CMaaString() const noexcept(noexcept_new && noexcept(GetMemoryString()))
    {
        return GetMemoryString64().NewCopy();
    }
    CMaaString GetMemoryString64() const noexcept(xThrow < 0)
    {
        if (m_bIsValid && m_StringLength <= CMaaString::sMaxStringLen)
        {
            CMaaString str(m_ptr, (_qword)m_StringLength, CMaaString::eMemString); // Must be not RO string for += support
            if constexpr (xThrow >= 0)
            {
                if ((size_t)str.Length() != m_StringLength)
                {
                    CMaaPtr_THROW1;
                }
            }
            return str;
        }
        if constexpr (xThrow >= 0)
        {
            CMaaPtr_THROW1;
        }
        return CMaaString64Z;
    }
    CMaaString str264() const noexcept //(noexcept_new)
    {
        try
        {
            return GetMemoryString264().NewCopy();
        }
        catch (...)
        {
        }
        return CMaaString64Z;
    }
    CMaaString GetMemoryString264() const noexcept
    {
        if (m_bIsValid && m_StringLength <= CMaaString::sMaxStringLen)
        {
            return CMaaString(m_ptr, (_qword)m_StringLength, CMaaString::eMemString); // Must be not RO string for += support
        }
        return CMaaString64Z;
    }
#endif
protected:
    void Required(int len) noexcept(xThrow <= 0 || bCountMode)
    {
        if constexpr (!bCountMode)
        {
            if (len <= 0)
            {
                return;
            }
            if (m_bIsValid)
            {
                if (m_BufferSize < m_StringLength + (size_t)len)
                {
                    size_t NewLen = 2 * ((size_t)m_StringLength + (size_t)len);
#if TOOLSLIB_USE_CMAASTRING64 == 0
                    constexpr size_t MaxStringLen = (((uint_)-1) >> 1) - 8;
#else
                    constexpr size_t MaxStringLen = (((_uqword)-1) >> 1) - 8;
#endif
                    if  constexpr (sizeof(int) == 4)
                    {
                        if (NewLen > 0x7fffffff - 8 && (size_t)len + (size_t)m_StringLength <= 0x7fffffff - 8)
                        {
                            NewLen = 0x7fffffff - 8;
                        }
                    }
                    else
                    {
                        if (NewLen > 0x7fffffffffffffffull - 8 && (size_t)len + (size_t)m_StringLength <= 0x7fffffffffffffffull - 8)
                        {
                            NewLen = (size_t)0x7fffffffffffffffull - 8;
                        }
                    }
                    NewLen = NewLen < 32 ? 32 : NewLen;
                    if (NewLen < m_StringLength + len || NewLen < m_StringLength || NewLen < (size_t)len || NewLen > MaxStringLen)
                    {
                        m_bIsValid = false;
                    }
                    else
                    {
                        CMaaPtr_<char, xThrow> Buffer(NewLen);
                        if ((m_bIsValid = Buffer.IsValid()))
                        {
                            memcpy((char*)Buffer, m_ptr, m_StringLength);
                            m_ptr = Buffer;
                            m_BufferSize = Buffer.Size();
                            m_Buffer.Swap(Buffer);
                        }
                    }
                }
            }
        }
    }
};

typedef CMaaConcatString_<0, false> CMaaConcatString;
typedef CMaaConcatString_<0, false> CMaaConcatString0;
typedef CMaaConcatString_<1, false> CMaaConcatString1;
typedef CMaaConcatString_<-1, false> CMaaConcatString_1;

#endif

template <> _dword CMaaHashFunction<CMaaString>(const CMaaString & k) noexcept;
inline _dword CMaaHashFunction(const CMaaString & k) noexcept
{
    return k.Hash();
}
template <> _uqword CMaaHashFunction64<CMaaString>(const CMaaString& k) noexcept;
inline _uqword CMaaHashFunction64(const CMaaString& k) noexcept
{
    return k.Hash64();
}

//------------------------------------------------------------------------------
// class CMaaString
//------------------------------------------------------------------------------
class CMaaStringG
{
    CMaaString s;
    struct lk
    {
        lk() noexcept
        {
#ifndef TOOLSLIB_SINGLE_THREAD
            gCMaaSyncAssignLockUpperLock();
#endif
        }
        ~lk() noexcept
        {
#ifndef TOOLSLIB_SINGLE_THREAD
            gCMaaSyncAssignLockUpperUnLock();
#endif
        }
    };
    void Lock() noexcept
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        gCMaaSyncAssignLockUpperLock();
#endif
    }
    void UnLock() noexcept
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        gCMaaSyncAssignLockUpperUnLock();
#endif
    }
public:
    CMaaStringG() noexcept
    {
    }
    CMaaStringG(const char * pszStr) noexcept(noexcept_new)
    :   s(pszStr)
    {
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaStringG(const char8_t * pszStr) noexcept(noexcept_new)
        : s(pszStr)
    {
    }
#endif
    CMaaStringG(const void * pMem, size_t Len) noexcept(noexcept_new)
    :   s(pMem, Len)
    {
    }
    CMaaStringG(const CMaaString & That) noexcept
    :   s(That)
    {
    }
    CMaaString operator=(const CMaaString & That) noexcept
    {
        lk l;
        s = That;
        return s;
    }
    CMaaString operator=(const char * pszTxt) noexcept(noexcept_new)
    {
        lk l;
        s = pszTxt;
        return s;
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString operator=(const char8_t * pszTxt) noexcept(noexcept_new)
    {
        lk l;
        s = pszTxt;
        return s;
    }
#endif
    CMaaString operator=(const CMaaString & That) const noexcept = delete;
    CMaaString operator=(const char * pszTxt) const = delete;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    CMaaString operator=(const char8_t * pszTxt) const = delete;
#endif

    ~CMaaStringG() // noexcept
    {
    }

    bool operator == (const char * str) const noexcept
    {
        lk l;
        return s == (char*)str;
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    bool operator == (const char8_t * str) const noexcept
    {
        lk l;
        return s == (char*)str;
    }
#endif
    bool operator == (const CMaaString & That) const noexcept
    {
        lk l;
        return s == That;
    }
    bool operator != (const char * str) const noexcept
    {
        lk l;
        return s != (char*)str;
    }
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    bool operator != (const char8_t * str) const noexcept
    {
        lk l;
        return s != (char*)str;
    }
#endif
    bool operator != (const CMaaString & That) const noexcept
    {
        lk l;
        return s != That;
    }
    bool operator < (const CMaaString & That) const noexcept
    {
        lk l;
        return s < That;
    }
    bool operator > (const CMaaString & That) const noexcept
    {
        lk l;
        return s > That;
    }
    operator CMaaString () const noexcept
    {
        lk l;
        return s;
    }
    void Format2(const char * format, CMaaString text, ...) noexcept
    {
        CMaaString txt;

        va_list list;
        va_start(list, text);
        txt._FormatV2(-1, "unknown", format, text, list);
        va_end(list);

        *this = std::move(txt);
    }
    void Empty() noexcept
    {
        lk l;
        s.Empty(); // *this = CMaaString {};
    }
};

class ToolsExport CMaaStringsContinedBuffer
{
protected:
    int m_BufferSize, m_Start = 0;
    int m_PaddingBytes;
    CMaaString m_CurrentBuffer;
public:
    CMaaStringsContinedBuffer(int BufferSize = 2 * 1024 * 1024, int PaddingBytes = 8) noexcept;
    ~CMaaStringsContinedBuffer() = default;
    CMaaStringsContinedBuffer(const CMaaStringsContinedBuffer &) = delete;
    CMaaStringsContinedBuffer & operator = (const CMaaStringsContinedBuffer &) = delete;
    CMaaString Put(CMaaString s, bool bMemoryString = false) noexcept(noexcept_new);
    void Reset() noexcept;
};

//------------------------------------------------------------------------------
CMaaString SummToTextSumm_rub(const CMaaString &input, bool bFullText, CMaaString *pSumm = nullptr, CMaaString *pSummSp = nullptr, CMaaString *pSummComma = nullptr);
CMaaString SummToTextSumm_usd(const CMaaString &input, bool bFullText, CMaaString *pSumm = nullptr, CMaaString *pSummSp = nullptr, CMaaString *pSummComma = nullptr);
CMaaString SummToTextSumm(const CMaaString &input, int_ lang = 0, CMaaString *pSumm = nullptr, CMaaString *pSummSp = nullptr, CMaaString *pSummComma = nullptr);
CMaaString NormalizeSummKIB(int len, const CMaaString &input);
CMaaString NormalizeSummSpComma(const CMaaString &input);
//------------------------------------------------------------------------------

#if TOOLSLIB_CMAASTRING64 <= 1
//-----------------------------------------------------------------------------------------------
template<class T> int SignedIntegerToString_d(T x, char* Buffer, char mode = 0, int width = -1, char z = 0) noexcept //, int prefix = 0)
{
    const bool mode_s = (mode == '+' || mode == ('+' ^ ','));
    const bool mode_c = (mode == ',' || mode == ('+' ^ ','));
    //prefix = prefix ? 2 : 0;
    //prefix = 0;
    if (width < 0)
    {
        z = 0;
    }
    int width2 = width >= 0 ? width : -width;
    int Len;
    char s;
    if (!x)
    {
        if (z == '0' && width2)
        {
            memset(Buffer, '0', width2);
            Buffer[Len = width2] = 0;
            return Len;
        }
        else
        {
            Buffer[0] = '0';
            Len = 1;
            s = 0;
            //prefix = 0;
        }
    }
    else
    {
        s = x < 0 ? '-' : mode_s ? '+' : 0;
        char* p = Buffer;
        const int_ d10 = 10;
        int nDigits = 0;
        for (x = x > 0 ? -x : x; x; x /= d10)
        {
            if (mode_c)
            {
                if (nDigits && !(nDigits % 3))
                {
                    *p++ = ',';
                }
                nDigits++;
            }
            *p++ = '0' - (char)(x % d10);
        }
        Len = (int)(p - Buffer);
        if (z != '0' || ((s /* || prefix*/) && width > 0 && width <= Len + (s ? 1 : 0) /* +prefix */))
        {
            /*
            if (prefix)
            {
                *p++ = 'x';
                *p++ = '0';
            }
            */
            if (s)
            {
                *p++ = s;
            }
            //s = prefix = 0;
            s = 0;
            Len = (int)(p - Buffer);
        }
        else if (width <= 0)
        {
            /*
            if (prefix)
            {
                *p++ = 'x';
                *p++ = '0';
            }
            */
            if (s)
            {
                *p++ = s;
                // s = 0;
            }
            Len = (int)(p - Buffer);
        }
        for (int i = 0; i < Len / 2; i++)
        {
            char c = Buffer[i];
            Buffer[i] = Buffer[Len - 1 - i];
            Buffer[Len - 1 - i] = c;
        }
    }
    if (width < 0 && width2 > Len)
    {
        memset(Buffer + Len, ' ', width2 - Len);
        Len = width2;
    }
    else if (width > Len + (s ? 1 : 0) /* +prefix */)
    {
        memmove(Buffer + width - Len, Buffer, Len);
        memset(Buffer, z == '0' ? '0' : ' ', width - Len);
        if (s)
        {
            Buffer[0] = s;
            /*
            if (prefix)
            {
                Buffer[1] = '0';
                Buffer[2] = 'x';
            }
            */
        }
        /*
        else if (prefix)
        {
            Buffer[0] = '0';
            Buffer[1] = 'x';
        }
        */
        Len = width;
    }
    Buffer[Len] = 0;
    return Len;
}
//-----------------------------------------------------------------------------------------------
template<class T> int UnsignedIntegerToString_d(T x, char* Buffer, char mode = 0, int width = -1, char z = 0) noexcept //, int prefix = 0)
{
    const bool mode_s = (mode == '+' || mode == ('+' ^ ','));
    const bool mode_c = (mode == ',' || mode == ('+' ^ ','));
    //prefix = prefix ? 2 : 0;
    //prefix = 0;
    if (width < 0)
    {
        z = 0;
    }
    int width2 = width >= 0 ? width : -width;
    int Len;
    char s;
    if (!x)
    {
        if (z == '0' && width2)
        {
            memset(Buffer, '0', width2);
            Buffer[Len = width2] = 0;
            return Len;
        }
        else
        {
            Buffer[0] = '0';
            Len = 1;
            s = 0;
            //prefix = 0;
        }
    }
    else
    {
        s = /*x < 0 ? '-' :*/ mode_s ? '+' : 0;
        char* p = Buffer;
        const uint_ d10 = 10;
        int nDigits = 0;
        for (; x; x /= d10)
        {
            if (mode_c)
            {
                if (nDigits && !(nDigits % 3))
                {
                    *p++ = ',';
                }
                nDigits++;
            }
            *p++ = '0' + (char)(x % d10);
        }
        Len = (int)(p - Buffer);
        if (z != '0' || ((s /* || prefix*/) && width > 0 && width <= Len + (s ? 1 : 0) /* +prefix */))
        {
            /*
            if (prefix)
            {
                *p++ = 'x';
                *p++ = '0';
            }
            */
            if (s)
            {
                *p++ = s;
            }
            //s = prefix = 0;
            s = 0;
            Len = (int)(p - Buffer);
        }
        else if (width <= 0)
        {
            /*
            if (prefix)
            {
                *p++ = 'x';
                *p++ = '0';
            }
            */
            if (s)
            {
                *p++ = s;
                // s = 0;
            }
            Len = (int)(p - Buffer);
        }
        for (int i = 0; i < Len / 2; i++)
        {
            char c = Buffer[i];
            Buffer[i] = Buffer[Len - 1 - i];
            Buffer[Len - 1 - i] = c;
        }
    }
    if (width < 0 && width2 > Len)
    {
        memset(Buffer + Len, ' ', width2 - Len);
        Len = width2;
    }
    else if (width > Len + (s ? 1 : 0) /* +prefix */)
    {
        memmove(Buffer + width - Len, Buffer, Len);
        memset(Buffer, z == '0' ? '0' : ' ', width - Len);
        if (s)
        {
            Buffer[0] = s;
            /*
            if (prefix)
            {
                Buffer[1] = '0';
                Buffer[2] = 'x';
            }
            */
        }
        /*
        else if (prefix)
        {
            Buffer[0] = '0';
            Buffer[1] = 'x';
        }
        */
        Len = width;
    }
    Buffer[Len] = 0;
    return Len;
}
//-----------------------------------------------------------------------------------------------
template<class T> int UnsignedIntegerToString_x(T x, char* Buffer, int width = -1, char mode = 'x', char z = 0, char prefix = 0) noexcept
{
    mode = mode == 'X' ? 'A' - 10 : 'a' - 10;
    int width2 = width >= 0 ? width : -width;
    if (width < 0)
    {
        z = 0;
    }
    int Len;
    if (!x)
    {
        Buffer[0] = '0';
        Len = 1;
    }
    else
    {
        char* p = Buffer;
        int nDigits = 0;
        for (; x; x >>= 4)
        {
            char c = (char)(x & 0x0f);
            c = c < 10 ? c + '0' : c + mode;
            *p++ = c;
        }
        Len = (int)(p - Buffer);
        {
            if (prefix)
            {
                *p++ = prefix == 'X' ? 'X' : 'x';
                *p++ = '0';
                Len += 2;
            }
        }
        for (int i = 0; i < Len / 2; i++)
        {
            char c = Buffer[i];
            Buffer[i] = Buffer[Len - 1 - i];
            Buffer[Len - 1 - i] = c;
        }
    }
    if (width < 0 && width2 > Len)
    {
        memset(Buffer + Len, ' ', width2 - Len);
        Len = width2;
    }
    else if (width > Len)
    {
        memmove(Buffer + width - Len, Buffer, Len);
        if (z == '0')
        {
            memset(Buffer, '0', width - Len + (prefix ? 2 : 0));
            if (prefix)
            {
                Buffer[0] = '0';
                Buffer[1] = prefix == 'X' ? 'X' : 'x';
            }
        }
        else
        {
            memset(Buffer, ' ', width - Len);
        }
        Len = width;
    }
    Buffer[Len] = 0;
    return Len;
}
//-----------------------------------------------------------------------------------------------
template<class T> int UnsignedIntegerToString_o(T x, char* Buffer, int width = -1, char z = 0, char prefix = 0) noexcept
{
    //mode = mode == 'X' ? 'A' - 10 : 'a' - 10;
    int width2 = width >= 0 ? width : -width;
    if (width < 0)
    {
        z = 0;
    }
    int Len;
    if (!x)
    {
        Buffer[0] = '0';
        Len = 1;
    }
    else
    {
        char* p = Buffer;
        int nDigits = 0;
        for (; x; x >>= 3)
        {
            *p++ = (char)(x & 0x07) + '0';
        }
        Len = (int)(p - Buffer);
        {
            if (prefix)
            {
                *p++ = '0';
                Len++;
            }
        }
        for (int i = 0; i < Len / 2; i++)
        {
            char c = Buffer[i];
            Buffer[i] = Buffer[Len - 1 - i];
            Buffer[Len - 1 - i] = c;
        }
    }
    if (width < 0 && width2 > Len)
    {
        memset(Buffer + Len, ' ', width2 - Len);
        Len = width2;
    }
    else if (width > Len)
    {
        memmove(Buffer + width - Len, Buffer, Len);
        memset(Buffer, z == '0' ? '0' : ' ', width - Len);
        Len = width;
    }
    Buffer[Len] = 0;
    return Len;
}
//-----------------------------------------------------------------------------------------------
template<class T> int UnsignedIntegerToString_p(T x, char* Buffer, int width = -1, char mode = 'x', char prefix = 0) noexcept
{
    mode = mode == 'X' ? 'A' - 10 : 'a' - 10;
    int width2 = width >= 0 ? width : -width;
    int Len;
    if (prefix)
    {
        Buffer[0] = '0';
        Buffer[1] = prefix == 'X' ? 'X' : 'x';
        Len = 2;
    }
    else
    {
        Len = 0;
    }
    {
        char* p = Buffer + Len + 2 * sizeof(T);
        for (; p > Buffer + Len; x >>= 4)
        {
            char c = (char)(x & 0x0f);
            c = c < 10 ? c + '0' : c + mode;
            *(--p) = c;
        }
        Len += (int)(2 * sizeof(T));
    }
    if (width < 0 && width2 > Len)
    {
        memset(Buffer + Len, ' ', width2 - Len);
        Len = width2;
    }
    else if (width > Len)
    {
        memmove(Buffer + width - Len, Buffer, Len);
        memset(Buffer, ' ', width - Len);
        Len = width;
    }
    Buffer[Len] = 0;
    return Len;
}
//-----------------------------------------------------------------------------------------------

#ifdef __linux__
int_ IsBadReadPtr(const void *ptr, size_t s) noexcept;
int_ IsBadWritePtr(void *ptr, size_t s) noexcept;
int_ IsBadStringPtrA(const char *ptr, size_t s) noexcept;
int_ IsBadStringPtrW(const wchar_t *ptr, size_t s) noexcept;
int_ IsBadStringPtrWC2(const _WC_ *ptr, size_t s) noexcept;
#endif
#if defined(_WIN32) || defined(__linux__)
#ifdef _WIN32
void CMaaAccessMemoryCheckFailed(const void* ptr, int_ len, const char* funcName, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
#endif
bool __is_bad_read_ptr(const void* ptr, int_ len, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_write_ptr(void* ptr, int_ len, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_string_ptr(const char* ptr, int_ maxlen, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_wstring_ptr(const wchar_t* ptr, int_ maxlen, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_wc2string_ptr(const _WC_* ptr, int_ maxlen, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;

bool __is_bad_read_ptr(const void* ptr, size_t len, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_write_ptr(void* ptr, size_t len, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_string_ptr(const char* ptr, size_t maxlen, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_wstring_ptr(const wchar_t* ptr, size_t maxlen, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
bool __is_bad_wc2string_ptr(const _WC_* ptr, size_t maxlen, bool bAccert = true, const char* SrcFile = "unknown", int_ SrcLine = -1) noexcept;
#endif

#endif


class CMaaTLGlobalStrings
{
public:
    enum e
    {
        e_encodeURIComponent_GoodCharacters = 0,
        /*
        e_ae_Operators,
        e_ae__szFunctions1 = e_ae_Operators + 25,
        e_ae__szFunctions2 = e_ae__szFunctions1 + 9,
        e_ae__modificators = e_ae__szFunctions2 + 4,
        e_ae__Modificators_1st_chars_1 = e_ae__modificators + 24,
        e_ae__Modificators_1st_chars_2,
        e_ae__ch_modificators,
        e_ae__sp_ch_modificators,
        */
        e_RussianAlphabet2,
        e_FullRussianAlphabet2,
        //e_LatRus1251Lower,
        //e_LatRus1251Upper,
        e_Cr,
        e_Lf,
        e_CrLf,
        e_LfCr,

        eNullErr,
        eBadPtrErr,
        eBad_wstrptr_Err,
        eBad_wc2ptr_Err,
        eBad_string_flag_Err,
        eBad_string_ptr_Err,

        eNullFormatErr,
        eBadFormatErr,
        eNullTextErr,
        eBadTextErr,
        eGetDateErr,
        eGetTimeErr,

        e_max
    };

protected:
    ceCMaaStringImp c[e_max]; // + 1

    CMaaString::CE::S m[e_max]; // + 1

public:
    constexpr CMaaTLGlobalStrings() noexcept;
    static const CMaaString & Get(int_ _e) noexcept;
};

//int TestRepace();

//const CMaaString & CMaaTLGlobalString(int_ _e) noexcept;
//inline const CMaaString & CMaaTLGlobalString(CMaaTLGlobalStrings::e _e, bool bInit = false) noexcept { return CMaaTLGlobalString((int_)_e, bInit); }
//inline const CMaaString & CMaaTLGlobalString2(CMaaTLGlobalStrings::e _e) noexcept { return CMaaTLGlobalString((int_)_e/*, true*/); }
inline const CMaaString & CMaaTLGlobalString(int_ _e) noexcept
{
    return CMaaTLGlobalStrings::Get(_e);
}
#define CMaaTLGlobalString2 CMaaTLGlobalString

//------------------------------------------------------------------------------
//
// Documentation.
//
// CMaaString & CMaaString::Format(const char * format, ...) uses
// standart type of format string exept of outputting strings (%s) and chars (%c).
// For thease types you can use parameters like '%-20.10s" where
// '-' - 'left alignment' ( alignment is right by default )
// 20  - width for restricting output
// 10  - means 'does not output more than 10 chars from string'
//

//CMaa_CLASSDEF ( CMaaString )

//==============================================================================

#if TOOLSLIB_USE_CMAASTRING64 == 1
#undef int
#undef int_strlen
#endif

#undef TOOLSLIB_CMAASTRING64
