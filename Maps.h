
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Ìîèñååíêî Àíäðåé Àëåêñååâè÷)
 */

// ToolsLib/Maps.h

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

struct sCp1251Map
{
    unsigned char m[256];
    constexpr sCp1251Map(bool bUp) noexcept
    {
        for (int_ i = 0; i < 256; i++)
        {
            m[i] = (unsigned char)i;
        }
        constexpr const char* s = "ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß";
        constexpr const char* d = "abcdefghijklmnopqrstuvwxyzàáâãäå¸æçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
        if (!bUp)
        {
            for (int_ i = 0; s[i]; i++)
            {
                m[(unsigned char)s[i]] = (unsigned char)d[i];
            }
        }
        else
        {
            for (int_ i = 0; s[i]; i++)
            {
                m[(unsigned char)d[i]] = (unsigned char)s[i];
            }
        }
    }
};

struct sLatinMap
{
    unsigned char m[256];
    constexpr sLatinMap(bool bUp) noexcept
    {
        if (!bUp)
        {
            for (int_ i = 0; i < 256; i++)
            {
                m[i] = i < 'A' || i > 'Z' ? (unsigned char)i : (unsigned char)(i + 'a' - 'A');
            }
        }
        else
        {
            for (int_ i = 0; i < 256; i++)
            {
                m[i] = i < 'a' || i > 'z' ? (unsigned char)i : (unsigned char)(i - ('a' - 'A'));
            }
        }
    }
};

struct sMyOrBase64DecodeMap
{
    int_ m[256];
    constexpr sMyOrBase64DecodeMap(const char* pszAlphabet) noexcept
    {
        for (int_ i = 0; i < 256; i++)
        {
            m[i] = -1;
        }
        for (int_ i = 0; pszAlphabet[i]; i++)
        {
            m[(unsigned char)pszAlphabet[i]] = i;
        }
    }
};

struct sRussian1251ToWCAlphabetMap
{
    _WC_ m[256];
    constexpr sRussian1251ToWCAlphabetMap() noexcept
    {
        for (int_ i = 0; i < 256; i++)
        {
            m[i] = 0;
        }
        constexpr const char* Rus = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
        for (int_ i = 0; Rus[i]; i++)
        {
            m[(unsigned char)Rus[i]] = 0x410 + i;
        }
        m[(unsigned char)0xb8] = 0x451; // small yo
        m[(unsigned char)0xa8] = 0x401; // casp. yo
        m[(unsigned char)0xb9] = 0x2116; // N (number)
        m[(unsigned char)0xb0] = 0x00B0; // degree
        m[(unsigned char)0x93] = 0x201C; // lt "
        m[(unsigned char)0x94] = 0x201D; // rt "
        m[(unsigned char)0xab] = 0x201C; // <<
        m[(unsigned char)0xbb] = 0x201D; // >>
    }
};

struct sFswapBitsMap
{
    unsigned char m[256];
    constexpr sFswapBitsMap() noexcept
    {
        for (int i = 0; i < 256; i++)
        {
            m[i] = ((i & 1) << 7) | ((i & 2) << 5) | ((i & 4) << 3) | ((i & 8) << 1) | ((i & 0x10) >> 1) | ((i & 0x20) >> 3) | ((i & 0x40) >> 5) | ((i & 0x80) >> 7);
        }
    }
};

const sCp1251Map& gfnLCp1251Map() noexcept;
const sCp1251Map& gfnUCp1251Map() noexcept;
const sLatinMap& gfnLLatinMap() noexcept;
const sLatinMap& gfnULatinMap() noexcept;
const unsigned char* gLcMap(int cp) noexcept;
const unsigned char* gUcMap(int cp) noexcept;

const sMyOrBase64DecodeMap& gfnBase64Map() noexcept;
const sMyOrBase64DecodeMap& gfnMyWeb64Map() noexcept;

const sRussian1251ToWCAlphabetMap& gfnRussian1251ToWCAlphabetMap() noexcept;

const sFswapBitsMap& gfnFswapBitsMap() noexcept;

const CMaa256Bits& gfnSpacesBits() noexcept;
const CMaa256Bits& gfnFsProhibitedChars() noexcept;
const CMaa256Bits& gfnAscii_H() noexcept;
const CMaa256Bits& gfnCp1251RusFlags() noexcept;
const CMaa256Bits& gfnAnySeparatorBits() noexcept;
#if 0
const CMaa256Bits& gfnLatinBits() noexcept;
const CMaa256Bits& gfnLatinmBits() noexcept;
const CMaa256Bits& gfnNotLatinBits() noexcept;
const CMaa256Bits& gfnNotLatinmBits() noexcept;
#endif

#define gSpacesBits gfnSpacesBits()
#define gFsProhibitedChars gfnFsProhibitedChars()
#define gAscii_H gfnAscii_H()
#define gCp1251RusFlags gfnCp1251RusFlags()
#define gAnySeparatorBits gfnAnySeparatorBits()
