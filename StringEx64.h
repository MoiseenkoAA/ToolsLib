
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

#if TOOLSLIB_USE_CMAASTRING64 == 2
#ifndef TOOLSLIB_CMAASTRING64

#define CMaaString64RO(x) CMaaString64(x, CMaaString64::eROStrlenMemString)

#define TOOLSLIB_CMAASTRING64 2
#define CMaaString CMaaString64
#define CMaaStringG CMaaString64G
#define CMaaStringsContinedBuffer CMaaStrings64ContinedBuffer
#define CMaaTLGlobalStrings CMaaTLGlobalStrings64
#define CMaaTLGlobalString CMaaTLGlobalString64
#define int _qword
#define int_strlen int64_strlen
#define IsCi IsCi64
#define IsCi0 IsCi640
#define IsCi00 IsCi6400

#define Utf8ToUnicode Utf8ToUnicode64
#define UnicodeString UnicodeString64
#define UnicodeToUtf8 UnicodeToUtf864
#define Unicode32String Unicode32String64
#define Unicode32ToUtf8 Unicode32ToUtf864
//#define Utf8ToAnsiEx Utf8ToAnsiEx64
#define wchar_t_ToUtf8 wchar_t_ToUtf864

#define GetBuildDate GetBuildDate64
#define GetLogDatePath GetLogDatePath64 
#define GetTextDate GetTextDate64
#define GetTextTime GetTextTime64
#define GetTextDateTime GetTextDateTime64
#define GetTextTimeDate GetTextTimeDate64
#define GetTextDateOrTimeToday GetTextDateOrTimeToday64
#define GetTextTimeDiff GetTextTimeDiff64
#define GetTextTimeDiff3 GetTextTimeDiff364
#define CMaaSqlDate CMaaSqlDate64
#define GetDateMailFormat GetDateMailFormat64
#define GetDateRKNFormat GetDateRKNFormat64
#define GetDateTimeYandexdFormat GetDateTimeYandexdFormat64
#define GetDateTimeYandexdFormatForMWS GetDateTimeYandexdFormatForMWS64
#define GetDateTimeNalogFormat GetDateTimeNalogFormat64
#define GetW3CDateTime GetW3CDateTime64
#define GetDateApacheFormat GetDateApacheFormat64
#define GetDateWebFormat GetDateWebFormat64
#define GetTextSizeKBMBGB GetTextSizeKBMBGB64
#define CMaaGetStartTime CMaaGetStartTime64
#define CMaaGetNextTime CMaaGetNextTime64
#define GetCopyrightText GetCopyrightText64
#define ConvertFromWebDateTime ConvertFromWebDateTime64
#define Export Export64
#define MakeMailSubject MakeMailSubject64
#define AnsiToUnicode AnsiToUnicode64
#define ConvertTextDateTime ConvertTextDateTime64
#define CMaaStr0 CMaaStr640
#define CMaaStrCh CMaaStr64Ch
#define SummToTextSumm_rub SummToTextSumm_rub64
#define SummToTextSumm_usd SummToTextSumm_usd64
#define SummToTextSumm SummToTextSumm64
#define NormalizeSummKIB NormalizeSummKIB64
#define NormalizeSummSpComma NormalizeSummSpComma64
#define CMaaTmpSprintf2StringsArray CMaaTmpSprintf2StringsArray64
#define ceCMaaStringImp ceCMaaStringImp64
#undef dsscanf3264
#undef dsscanfV3264
#define dsscanf3264 dsscanf64
#define dsscanfV3264 dsscanfV64
//#define CMaaStringConsts CMaaStringConsts64

#include "StringEx.h"

#undef TOOLSLIB_CMAASTRING64
#undef CMaaString
#undef CMaaStringG
#undef CMaaStringsContinedBuffer
#undef CMaaTLGlobalStrings
#undef CMaaTLGlobalString
#undef int
#undef int_strlen
#undef IsCi
#undef IsCi0
#undef IsCi00

#undef Utf8ToUnicode
#undef UnicodeString
#undef UnicodeToUtf8
#undef Unicode32String
#undef Unicode32ToUtf8
//#undef Utf8ToAnsiEx
#undef wchar_t_ToUtf8

#undef GetBuildDate
#undef GetLogDatePath
#undef GetTextDate
#undef GetTextTime
#undef GetTextDateTime
#undef GetTextTimeDate
#undef GetTextDateOrTimeToday
#undef GetTextTimeDiff
#undef GetTextTimeDiff3
#undef CMaaSqlDate
#undef GetDateMailFormat
#undef GetDateRKNFormat
#undef GetDateTimeYandexdFormat
#undef GetDateTimeYandexdFormatForMWS
#undef GetDateTimeNalogFormat
#undef GetW3CDateTime
#undef GetDateApacheFormat
#undef GetDateWebFormat
#undef GetTextSizeKBMBGB
#undef CMaaGetStartTime
#undef CMaaGetNextTime
#undef GetCopyrightText
#undef ConvertFromWebDateTime
#undef Export
#undef MakeMailSubject
#undef AnsiToUnicode
#undef ConvertTextDateTime
#undef CMaaStr0
#undef CMaaStrCh
#undef SummToTextSumm_rub
#undef SummToTextSumm_usd
#undef SummToTextSumm
#undef NormalizeSummKIB
#undef NormalizeSummSpComma
#undef CMaaTmpSprintf2StringsArray
#undef ceCMaaStringImp
#undef dsscanf3264
#undef dsscanfV3264
#define dsscanf3264 dsscanf
#define dsscanfV3264 dsscanfV
//#undef CMaaStringConsts

#else

#define CMaaString CMaaString64
#define CMaaStringG CMaaString64G
#define CMaaStringsContinedBuffer CMaaStrings64ContinedBuffer
#define CMaaTLGlobalStrings CMaaTLGlobalStrings64
#define CMaaTLGlobalString CMaaTLGlobalString64
#define int _qword
#define int_strlen int64_strlen
#define IsCi IsCi64
#define IsCi0 IsCi640
#define IsCi00 IsCi6400

#define Utf8ToUnicode Utf8ToUnicode64
#define UnicodeString UnicodeString64
#define UnicodeToUtf8 UnicodeToUtf864
#define Unicode32String Unicode32String64
#define Unicode32ToUtf8 Unicode32ToUtf864
//#define Utf8ToAnsiEx Utf8ToAnsiEx64
#define wchar_t_ToUtf8 wchar_t_ToUtf864

#define GetBuildDate GetBuildDate64
#define GetLogDatePath GetLogDatePath64 
#define GetTextDate GetTextDate64
#define GetTextTime GetTextTime64
#define GetTextDateTime GetTextDateTime64
#define GetTextTimeDate GetTextTimeDate64
#define GetTextDateOrTimeToday GetTextDateOrTimeToday64
#define GetTextTimeDiff GetTextTimeDiff64
#define GetTextTimeDiff3 GetTextTimeDiff364
#define CMaaSqlDate CMaaSqlDate64
#define GetDateMailFormat GetDateMailFormat64
#define GetDateRKNFormat GetDateRKNFormat64
#define GetDateTimeYandexdFormat GetDateTimeYandexdFormat64
#define GetDateTimeYandexdFormatForMWS GetDateTimeYandexdFormatForMWS64
#define GetDateTimeNalogFormat GetDateTimeNalogFormat64
#define GetW3CDateTime GetW3CDateTime64
#define GetDateApacheFormat GetDateApacheFormat64
#define GetDateWebFormat GetDateWebFormat64
#define GetTextSizeKBMBGB GetTextSizeKBMBGB64
#define CMaaGetStartTime CMaaGetStartTime64
#define CMaaGetNextTime CMaaGetNextTime64
#define GetCopyrightText GetCopyrightText64
#define ConvertFromWebDateTime ConvertFromWebDateTime64
#define Export Export64
#define MakeMailSubject MakeMailSubject64
#define AnsiToUnicode AnsiToUnicode64
#define ConvertTextDateTime ConvertTextDateTime64
#define CMaaStr0 CMaaStr640
#define CMaaStrCh CMaaStr64Ch
#define SummToTextSumm_rub SummToTextSumm_rub64
#define SummToTextSumm_usd SummToTextSumm_usd64
#define SummToTextSumm SummToTextSumm64
#define NormalizeSummKIB NormalizeSummKIB64
#define NormalizeSummSpComma NormalizeSummSpComma64
#define CMaaTmpSprintf2StringsArray CMaaTmpSprintf2StringsArray64
#define ceCMaaStringImp ceCMaaStringImp64
#undef dsscanf3264
#undef dsscanfV3264
#define dsscanf3264 dsscanf64
#define dsscanfV3264 dsscanfV64
//#define CMaaStringConsts CMaaStringConsts64

#endif
#endif
