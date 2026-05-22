
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Ìîèñååíêî Àíäðåé Àëåêñååâè÷)
 */

// ToolsLib/Maps.cpp

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

static constexpr sCp1251Map lCp1251Map(false);
static constexpr sCp1251Map uCp1251Map(true);
static constexpr sLatinMap lLatinMap(false);
static constexpr sLatinMap uLatinMap(true);

const sCp1251Map& gfnLCp1251Map() noexcept { return lCp1251Map; }
const sCp1251Map& gfnUCp1251Map() noexcept { return uCp1251Map; }
const sLatinMap& gfnLLatinMap() noexcept { return lLatinMap; }
const sLatinMap& gfnULatinMap() noexcept { return uLatinMap; }
const unsigned char* gLcMap(int cp) noexcept { return !cp || !(cp == 1251 || cp == -1) ? gfnLLatinMap().m : gfnLCp1251Map().m; }
const unsigned char* gUcMap(int cp) noexcept { return !cp || !(cp == 1251 || cp == -1) ? gfnULatinMap().m : gfnUCp1251Map().m; }


static constexpr sMyOrBase64DecodeMap sBase64Map("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
static constexpr sMyOrBase64DecodeMap sMyWeb64Map("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_/");

const sMyOrBase64DecodeMap& gfnBase64Map() noexcept { return sBase64Map; }
const sMyOrBase64DecodeMap& gfnMyWeb64Map() noexcept { return sMyWeb64Map; }


static constexpr sRussian1251ToWCAlphabetMap sRussian1251ToWCAlphabetMap__;
const sRussian1251ToWCAlphabetMap& gfnRussian1251ToWCAlphabetMap() noexcept { return sRussian1251ToWCAlphabetMap__; }

static constexpr sFswapBitsMap sFswapBitsMap__;
const sFswapBitsMap& gfnFswapBitsMap() noexcept { return sFswapBitsMap__; }

static constexpr CMaa256Bits sSpacesBits{ " \t", 2, false };
static constexpr CMaa256Bits sFsProhibitedChars(szOTHER_FILESYSTEM_SLASH ":;*?\"<>|&!%$\0", 14, false);
static constexpr CMaa256Bits sAscii_H('\x80', '\xff', false);
static constexpr CMaa256Bits sCp1251RusFlags("ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäå¸æçèéêëìíîïðñòóôõö÷øùúûüýþÿ", 2 * 33, false);
static constexpr CMaa256Bits sAnySeparatorBits{ szFILESYSTEM_SLASH szOTHER_FILESYSTEM_SLASH, 2, false };
#if 0
static constexpr CMaa256Bits sLatin256{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 2 * 26, false };
static constexpr CMaa256Bits sLatin256m{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-", 2 * 26 + 1, false };
static constexpr CMaa256Bits sNotLatin256{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 2 * 26, true };
static constexpr CMaa256Bits sNotLatin256m{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-", 2 * 26 + 1, true };
#endif

const CMaa256Bits& gfnSpacesBits() noexcept { return sSpacesBits; }
const CMaa256Bits& gfnFsProhibitedChars() noexcept { return sFsProhibitedChars; }
const CMaa256Bits& gfnAscii_H() noexcept { return sAscii_H; }
const CMaa256Bits& gfnCp1251RusFlags() noexcept { return sCp1251RusFlags; }
const CMaa256Bits& gfnAnySeparatorBits() noexcept { return sAnySeparatorBits; }
#if 0
const CMaa256Bits& gfnLatinBits() noexcept { return sLatin256; }
const CMaa256Bits& gfnLatinmBits() noexcept { return sLatin256m; }
const CMaa256Bits& gfnNotLatinBits() noexcept { return sNotLatin256; }
const CMaa256Bits& gfnNotLatinmBits() noexcept { return sNotLatin256m; }
#endif
