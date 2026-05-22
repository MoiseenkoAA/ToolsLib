
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Ìîèñååíêî Àíäðåé Àëåêñååâè÷)
 */

// ToolsLib/StringLib.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains international domain names convertion for ToolsLib.
 * This code is originally from RFC.
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

// IDN/IDNA - International Domain Names for Application support

/*
int Test()
{
     {
          CMaaString src = UnicodeToUtf8(UnicodeString(L"ðîññèÿ.ðó"));
          CMaaString dst = "xn--h1alffa9f.xn--p1ag";
          printf("test1 should show results: %S --> %S\n", &src, &dst);
          
          src = UnicodeToUtf8(UnicodeString(L"òåñò.ðîññèÿ.ðó"));
          dst = "xn--e1aybc.xn--h1alffa9f.xn--p1ag";
          printf("test2 should show results: %S --> %S\n", &src, &dst);

          // words encoding results:
          // ðîññèÿ --> h1alffa9f
          // ðó --> p1ag
     }
     CMaaString srcUtf8String = UnicodeToUtf8(UnicodeString(L"òåñò.ðîññèÿ.ðó"));
     CMaaString IDN_Name;
     if   (IDN_Encode(srcUtf8String, IDN_Name))
     {
          printf("%S --> %S\n", &srcUtf8String, &IDN_Name);
          CMaaString IDNA_Utf8String;
          if   (IDN_Decode(IDN_Name, IDNA_Utf8String))
          {
               printf("%S --> %S\n", &IDN_Name, &IDNA_Utf8String);
          }
          else
          {
               printf("IDN_Decode() error\n");
          }
     }
     else
     {
          printf("IDN_Encode() error\n");
     }
     return 0;
}
*/
//----------------------------------------------------------------------------------------
/*
punycode.c from RFC 3492
http://www.nicemice.net/idn/
Adam M. Costello
http://www.nicemice.net/amc/

This is ANSI C code (C89) implementing Punycode (RFC 3492).

*/

/************************************************************/
/* Public interface (would normally go in its own .h file): */

//#include <limits.h>

enum punycode_status {
    punycode_success,
    punycode_bad_input,   /* Input is invalid.                       */
    punycode_big_output,  /* Output would exceed the space provided. */
    punycode_overflow     /* Input needs wider integers to process.  */
};

/*
#if UINT_MAX >= (1 << 26) - 1
typedef unsigned int punycode_uint;
#else
typedef unsigned long punycode_uint;
#endif
*/

typedef _udword punycode_uint;

enum punycode_status punycode_encode(
  punycode_uint input_length,
  const punycode_uint input[],
  const unsigned char case_flags[],
  punycode_uint *output_length,
  char output[] );

/* punycode_encode() converts Unicode to Punycode.  The input     */
/* is represented as an array of Unicode code points (not code    */
/* units; surrogate pairs are not allowed), and the output        */
/* will be represented as an array of ASCII code points.  The     */
/* output string is *not* null-terminated; it will contain        */
/* zeros if and only if the input contains zeros.  (Of course     */
/* the caller can leave room for a terminator and add one if      */
/* needed.)  The input_length is the number of code points in     */
/* the input.  The output_length is an in/out argument: the       */
/* caller passes in the maximum number of code points that it     */
/* can receive, and on successful return it will contain the      */
/* number of code points actually output.  The case_flags array   */
/* holds input_length boolean values, where nonzero suggests that */
/* the corresponding Unicode character be forced to uppercase     */
/* after being decoded (if possible), and zero suggests that      */
/* it be forced to lowercase (if possible).  ASCII code points    */
/* are encoded literally, except that ASCII letters are forced    */
/* to uppercase or lowercase according to the corresponding       */
/* uppercase flags.  If case_flags is a null pointer then ASCII   */
/* letters are left as they are, and other code points are        */
/* treated as if their uppercase flags were zero.  The return     */
/* value can be any of the punycode_status values defined above   */
/* except punycode_bad_input; if not punycode_success, then       */
/* output_size and output might contain garbage.                  */

enum punycode_status punycode_decode(
  punycode_uint input_length,
  const char input[],
  punycode_uint *output_length,
  punycode_uint output[],
  unsigned char case_flags[] );

/* punycode_decode() converts Punycode to Unicode.  The input is  */
/* represented as an array of ASCII code points, and the output   */
/* will be represented as an array of Unicode code points.  The   */
/* input_length is the number of code points in the input.  The   */
/* output_length is an in/out argument: the caller passes in      */
/* the maximum number of code points that it can receive, and     */
/* on successful return it will contain the actual number of      */
/* code points output.  The case_flags array needs room for at    */
/* least output_length values, or it can be a null pointer if the */
/* case information is not needed.  A nonzero flag suggests that  */
/* the corresponding Unicode character be forced to uppercase     */
/* by the caller (if possible), while zero suggests that it be    */
/* forced to lowercase (if possible).  ASCII code points are      */
/* output already in the proper case, but their flags will be set */
/* appropriately so that applying the flags would be harmless.    */
/* The return value can be any of the punycode_status values      */
/* defined above; if not punycode_success, then output_length,    */
/* output, and case_flags might contain garbage.  On success, the */
/* decoder will never need to write an output_length greater than */
/* input_length, because of how the encoding is defined.          */


// Internationalized Domain Names (IDN) in Applications (IDNA)

bool IsHas_IDN_EncodingPart(const CMaaString &srcUtf8String) noexcept
{
    return srcUtf8String.IsLeftCi("xn--", 4, 0) || (srcUtf8String.FindCi(".xn--") > 0) || (srcUtf8String.FindCi("@xn--") > 0);
}

bool IDN_EncodeWord(CMaaString srcUtf8String, CMaaString &dstName)
{
    try
    {
        CMaaPtr_<punycode_uint, 1> input(2 * srcUtf8String.Length() + 1024);
        CMaaPtr_<char, 1> output((5u + 10u) * srcUtf8String.Length() + 1024u);
        //CMaaPtr_<unsigned char, 1> case_flags((5 + 10) * srcUtf8String.Length() + 1024);

        if  (input.IsValid() && output.IsValid())// && case_flags.IsValid())
        {
            int uCharsCount = 0;
            const unsigned char * pp = (const unsigned char *)(const char *)srcUtf8String;
            const unsigned char * ee = pp + srcUtf8String.Length();
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
                /*
                    if   (UnicodeChar >= 0x410 && UnicodeChar < 0x450 - 32)
                    {
                         //"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß" "àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
                         UnicodeChar += 32;
                    }
                    else if (UnicodeChar == 0x401) // '¨'
                    {
                         UnicodeChar = 0x451; // '¸'
                    }
                    else if (UnicodeChar >= 'A' && UnicodeChar <= 'Z')
                    {
                         UnicodeChar = UnicodeChar + 'a' - 'A';
                    }
                    */
                input[uCharsCount++] = UnicodeChar;
            }
            //memset(case_flags, 0, case_flags.MaxIndex());

            punycode_uint input_length = (punycode_uint)uCharsCount;
            punycode_uint output_length = (punycode_uint)output.MaxIndex() - 1;
            punycode_status Status = punycode_encode(input_length, input, nullptr/*case_flags*/, &output_length, output);
            if  (Status == punycode_success && output_length < output.MaxIndex())
            {
                dstName = CMaaString(output, output_length);
                return true;
            }
        }
    }
    catch(...)
    {
    }
    return false;
}

bool IDN_Encode(CMaaString srcUtf8String, CMaaString &dstName)
{
    char Buffer[TOOLSLIB_CS_32K];
    int nChangesCount = 0;
    //CMaaString Suffix = ".aced.net";
    CMaaString Suffix = "";
    //CMaaString Suffix;// = ".aced.net";
    try
    {
        CMaaConcatString ResultString(Buffer, sizeof(Buffer), (5 + 10) * srcUtf8String.Length() + 1024);
        if  (ResultString.IsValid())
        {
            int pos0 = -1;
            while(++pos0 < srcUtf8String.Length())
            {
                int pos = (warning_int)srcUtf8String.Find(pos0, '.');
                int pos2 = (warning_int)srcUtf8String.Find(pos0, '@');
                if  (pos2 >= 0 && pos2 < pos)
                {
                    pos = pos2;
                }
                CMaaString strWord;
                if  (pos >= 0)
                {
                    strWord = srcUtf8String.Mid(pos0, pos - pos0);
                }
                else
                {
                    strWord = srcUtf8String.Mid(pos0);
                }
                CMaaString ResultWord;
                if  (!IDN_EncodeWord(strWord, ResultWord))
                {
                    return false;
                }
                bool bIDN_Name;
                if  ((bIDN_Name = (ResultWord != strWord + "-")))
                {
                    ResultString += "xn--";
                    nChangesCount++;
                }
                else
                {
                    ResultWord = strWord;
                }
                ResultString += ResultWord;
                if  (pos >= 0)
                {
                    ResultString += srcUtf8String[pos];//'.';
                    pos0 = pos;
                }
                else
                {
                    if  (bIDN_Name)
                    {
                        ResultString += Suffix;
                    }
                    break;
                }
            }
            CMaaString Result = CMaaString(ResultString);
            if  (Result != srcUtf8String && nChangesCount > 0)
            {
                dstName = Result;
                return true;
            }
            // else no conversion
        }
    }
    catch(...)
    {
    }
    return false;
}

bool IDN_DecodeWord(CMaaString srcName, CMaaString &dstUtf8String)
{
    try
    {
        CMaaPtr_<char, 1> input(srcName.Length() + 10);
        CMaaPtr_<punycode_uint, 1> output((5 + 10) * srcName.Length() + 1024);
        //CMaaPtr_<unsigned char, 1> case_flags((5 + 10) * srcName.Length() + 1024);

        if  (input.IsValid() && output.IsValid())// && case_flags.IsValid())
        {
            memcpy(input, srcName, srcName.Length() + 1);
            punycode_uint input_length = (warning_int)srcName.Length();

            //memset(case_flags, 0, case_flags.MaxIndex());

            punycode_uint output_length = (punycode_uint)output.MaxIndex() - 1;
            punycode_status Status = punycode_decode(input_length, input, &output_length, output, nullptr);//case_flags);

            if  (Status == punycode_success && output_length < output.MaxIndex())
            {
                int uCharsCount = (int)output_length;
                char Buffer[TOOLSLIB_CS_64K];
                CMaaConcatString ResultString(Buffer, sizeof(Buffer), 4 * uCharsCount + 10);
                if  (ResultString.IsValid())
                {
                    for (int i = 0; i < uCharsCount; i++)
                    {
                        long UnicodeChar = output[i];
                        /*
                              if   (UnicodeChar >= 0x410 && UnicodeChar < 0x450 - 32)
                              {
                                   //"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß" "àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
                                   UnicodeChar += 32;
                              }
                              else if (UnicodeChar == 0x401) // '¨'
                              {
                                   UnicodeChar = 0x451; // '¸'
                              }
                              else if (UnicodeChar >= 'A' && UnicodeChar <= 'Z')
                              {
                                   UnicodeChar = UnicodeChar + 'a' - 'A';
                              }
                              */

                        char ch[4];
                        if  (UnicodeChar <= 0x7f)
                        {
                            ResultString += (char)UnicodeChar;
                        }
                        else if (UnicodeChar <= 0x7ff)
                        {
                            ch[0] = (char)(0xC0 | ((UnicodeChar >> 6) & 0x1f));
                            ch[1] = (char)(0x80 | (UnicodeChar & 0x3f));
                            ResultString.Add(ch, 2);
                        }
                        else if (UnicodeChar <= 0xffff)
                        {
                            ch[0] = (char)(0xE0 | ((UnicodeChar >> 12) & 0x0f));
                            ch[1] = (char)(0x80 | ((UnicodeChar >> 6 ) & 0x3f));
                            ch[2] = (char)(0x80 | (UnicodeChar & 0x3f));
                            ResultString.Add(ch, 3);
                        }
                        else if (UnicodeChar <= 0x1fffff)
                        {
                            ch[0] = (char)(0xF0 | ((UnicodeChar >> 18) & 0x07));
                            ch[1] = (char)(0x80 | ((UnicodeChar >> 12) & 0x0f));
                            ch[2] = (char)(0x80 | ((UnicodeChar >> 6 ) & 0x3f));
                            ch[3] = (char)(0x80 | (UnicodeChar & 0x3f));
                            ResultString.Add(ch, 4);
                        }
                    }
                    dstUtf8String = CMaaString(ResultString);
                    return true;
                }
            }
        }
    }
    catch(...)
    {
    }
    return false;
}

bool IDN_EncodeWordAndAddPrefix(CMaaString srcUtf8Word, CMaaString &dstWordWithPrefix)
{
    if  (IDN_EncodeWord(srcUtf8Word, dstWordWithPrefix))
    {
        dstWordWithPrefix = CMaaString("xn--") + dstWordWithPrefix;
        return true;
    }
    return false;
}
bool IDN_DecodeWordWithPrefix(CMaaString srcWordWithPrefix, CMaaString &/*dstUtf8String*/)
{
    if  (srcWordWithPrefix.IsLeftCi("xn--", 4, 0))
    {
        srcWordWithPrefix = srcWordWithPrefix.Mid(4);
    }
    return false;
}
bool IDN_Decode(CMaaString srcName, CMaaString &dstUtf8String)
{
    int nChangesCount = 0;
    //CMaaString Suffix = ".aced.net";
    CMaaString Suffix = "";
    try
    {
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString ResultString(Buffer, sizeof(Buffer), 4 * (5 + 10) * srcName.Length() + 1024);
        if  (ResultString.IsValid())
        {
            int pos0 = -1;
            while(++pos0 < srcName.Length())
            {
                int pos = (warning_int)srcName.Find(pos0, '.');
                int pos2 = (warning_int)srcName.Find(pos0, '@');
                if  (pos2 >= 0 && pos2 < pos)
                {
                    pos = pos2;
                }
                CMaaString strWord;
                if  (pos >= 0)
                {
                    strWord = srcName.Mid(pos0, pos - pos0);
                }
                else
                {
                    strWord = srcName.Mid(pos0);
                }
                CMaaString ResultWord;
                if  (strWord.IsLeftCi("xn--", 4, 0))
                {
                    strWord = strWord.Mid(4);
                    if  (!IDN_DecodeWord(strWord, ResultWord))
                    {
                        return false;
                    }
                    nChangesCount++;
                }
                else
                {
                    ResultWord = strWord;
                }
                ResultString += ResultWord;
                if  (pos >= 0)
                {
                    ResultString += srcName[pos];//'.';
                    pos0 = pos;
                }
                else
                {
                    break;
                }
            }
            if  (nChangesCount > 0)
            {
                dstUtf8String = CMaaString(ResultString);
                if  (dstUtf8String.IsRight(Suffix))
                {
                    dstUtf8String -= Suffix.Length();
                }
                return true;
            }
        }
    }
    catch(...)
    {
    }
    return false;
}


/**********************************************************/
/* Implementation (would normally go in its own .c file): */

//#include <string.h>

/*** Bootstring parameters for Punycode ***/

enum { base = 36, tmin = 1, tmax = 26, skew = 38, damp = 700,
    initial_bias = 72, initial_n = 0x80, delimiter = 0x2D };

/* basic(cp) tests whether cp is a basic code point: */
#define basic(cp) ((punycode_uint)(cp) < 0x80)

/* delim(cp) tests whether cp is a delimiter: */
#define delim(cp) ((cp) == delimiter)

/* decode_digit(cp) returns the numeric value of a basic code */
/* point (for use in representing integers) in the range 0 to */
/* base-1, or base if cp is does not represent a value.       */

static punycode_uint decode_digit(punycode_uint _cp)
{
    unsigned int cp = (unsigned int)_cp;
    unsigned int _base = (unsigned int)base;
    return  (punycode_uint)(cp - 48 < 10 ? cp - 22 :  cp - 65 < 26 ? cp - 65 :
    cp - 97 < 26 ? cp - 97 :  _base);
}

/* encode_digit(d,flag) returns the basic code point whose value      */
/* (when used for representing integers) is d, which needs to be in   */
/* the range 0 to base-1.  The lowercase form is used unless flag is  */
/* nonzero, in which case the uppercase form is used.  The behavior   */
/* is undefined if flag is nonzero and digit d has no uppercase form. */

static char encode_digit(punycode_uint d, int flag)
{
    return (char)(d + 22 + 75 * (d < 26) - ((flag != 0) << 5));
    /*  0..25 map to ASCII a..z or A..Z */
    /* 26..35 map to ASCII 0..9         */
}

/* flagged(bcp) tests whether a basic code point is flagged */
/* (uppercase).  The behavior is undefined if bcp is not a  */
/* basic code point.                                        */

#define flagged(bcp) ((punycode_uint)(bcp) - 65 < 26)

/* encode_basic(bcp,flag) forces a basic code point to lowercase */
/* if flag is zero, uppercase if flag is nonzero, and returns    */
/* the resulting code point.  The code point is unchanged if it  */
/* is caseless.  The behavior is undefined if bcp is not a basic */
/* code point.                                                   */

static char encode_basic(punycode_uint bcp, int flag)
{
    bcp -= (bcp - 97 < 26) << 5;
    return (char)(bcp + ((!flag && (bcp - 65 < 26)) << 5));
}

/*** Platform-specific constants ***/

/* maxint is the maximum value of a punycode_uint variable: */
static const punycode_uint maxint = -1;
/* Because maxint is unsigned, -1 becomes the maximum value. */

/*** Bias adaptation function ***/

static punycode_uint adapt(
  punycode_uint delta, punycode_uint numpoints, int firsttime )
{
    punycode_uint k;

    delta = firsttime ? delta / damp : delta >> 1;
    /* delta >> 1 is a faster way of doing delta / 2 */
    delta += delta / numpoints;

    for (k = 0;  delta > ((base - tmin) * tmax) / 2;  k += base) {
        delta /= base - tmin;
    }

    return k + (base - tmin + 1) * delta / (delta + skew);
}

/*** Main encode function ***/

enum punycode_status punycode_encode(
  punycode_uint input_length,
  const punycode_uint input[],
  const unsigned char case_flags[],
  punycode_uint *output_length,
  char output[] )
{
    punycode_uint n, delta, h, b, out, max_out, bias, j, m, q, k, t;
    unsigned int _k, _bias, _bt, _bmt;

    /* Initialize the state: */

    n = initial_n;
    delta = out = 0;
    max_out = *output_length;
    bias = initial_bias;

    /* Handle the basic code points: */

    for (j = 0;  j < input_length;  ++j) {
        if  (basic(input[j])) {
            if  (max_out - out < 2) return punycode_big_output;
            output[out++] =
        case_flags ?  encode_basic(input[j], case_flags[j]) : (char)input[j];
        }
        /* else if (input[j] < n) return punycode_bad_input; */
        /* (not needed for Punycode with unsigned code points) */
    }

    h = b = out;

    /* h is the number of code points that have been handled, b is the  */
    /* number of basic code points, and out is the number of characters */
    /* that have been output.                                           */

    if  (b > 0) output[out++] = delimiter;

    /* Main encoding loop: */

    while(h < input_length) {
        /* All non-basic code points < n have been     */
        /* handled already.  Find the next larger one: */

        for (m = maxint, j = 0;  j < input_length;  ++j) {
            /* if (basic(input[j])) continue; */
            /* (not needed for Punycode) */
            if  (input[j] >= n && input[j] < m) m = input[j];
        }

        /* Increase delta enough to advance the decoder's    */
        /* <n,i> state to <m,0>, but guard against overflow: */

        if  (m - n > (maxint - delta) / (h + 1)) return punycode_overflow;
        delta += (m - n) * (h + 1);
        n = m;

        for (j = 0;  j < input_length;  ++j) {
            /* Punycode does not need to check whether input[j] is basic: */
            if  (input[j] < n /* || basic(input[j]) */ ) {
                if  (++delta == 0) return punycode_overflow;
            }

            if  (input[j] == n) {
                /* Represent delta as a generalized variable-length integer: */

                for (q = delta, k = base;  ;  k += base) {
                    if  (out >= max_out) return punycode_big_output;
                    _k = (unsigned int)k;
                    _bias = (unsigned int)bias;
                    _bt = (unsigned int)bias + (unsigned int)tmax;
                    _bmt = (unsigned int)k - (unsigned int)bias;
                    t = _k <= _bias /* + tmin */ ? (punycode_uint)tmin :     /* +tmin not needed */
                    _k >= _bt ? (punycode_uint)tmax : (punycode_uint)_bmt;
                    if  (q < t) break;
                    output[out++] = encode_digit(t + (q - t) % (base - t), 0);
                    q = (q - t) / (base - t);
                }

                output[out++] = encode_digit(q, case_flags && case_flags[j]);
                bias = adapt(delta, h + 1, h == b);
                delta = 0;
                ++h;
            }
        }

        ++delta, ++n;
    }

    *output_length = out;
    return punycode_success;
}

/*** Main decode function ***/

enum punycode_status punycode_decode(
  punycode_uint input_length,
  const char input[],
  punycode_uint *output_length,
  punycode_uint output[],
  unsigned char case_flags[] )
{
    punycode_uint n, out, i, max_out, bias,
    b, j, in, oldi, w, k, digit, t;

    /* Initialize the state: */

    n = initial_n;
    out = i = 0;
    max_out = *output_length;
    bias = initial_bias;

    /* Handle the basic code points:  Let b be the number of input code */
    /* points before the last delimiter, or 0 if there is none, then    */
    /* copy the first b code points to the output.                      */

    for (b = j = 0;  j < input_length;  ++j) if (delim(input[j])) b = j;
    if  (b > max_out) return punycode_big_output;

    for (j = 0;  j < b;  ++j) {
        if  (case_flags) case_flags[out] = flagged(input[j]);
        if  (!basic(input[j])) return punycode_bad_input;
        output[out++] = input[j];
    }

    /* Main decoding loop:  Start just after the last delimiter if any  */
    /* basic code points were copied; start at the beginning otherwise. */

    for (in = b > 0 ? b + 1 : 0;  in < input_length;  ++out) {

        /* in is the index of the next character to be consumed, and */
        /* out is the number of code points in the output array.     */

        /* Decode a generalized variable-length integer into delta,  */
        /* which gets added to i.  The overflow checking is easier   */
        /* if we increase i as we go, then subtract off its starting */
        /* value at the end to obtain delta.                         */

        for (oldi = i, w = 1, k = base;  ;  k += base) {
            if  (in >= input_length) return punycode_bad_input;
            digit = decode_digit(input[in++]);
            if  (digit >= base) return punycode_bad_input;
            if  (digit > (maxint - i) / w) return punycode_overflow;
            i += digit * w;
            t = (unsigned int)k <= (unsigned int)bias /* + tmin */ ? (punycode_uint)tmin :     /* +tmin not needed */
            k >= (unsigned int)((unsigned int)bias + (unsigned int)tmax) ? (punycode_uint)tmax : (punycode_uint)((unsigned int)k - (unsigned int)bias);
            if  (digit < t) break;
            if  (w > maxint / (base - t)) return punycode_overflow;
            w *= (base - t);
        }

        bias = adapt(i - oldi, out + 1, oldi == 0);

        /* i was supposed to wrap around from out+1 to 0,   */
        /* incrementing n each time, so we'll fix that now: */

        if  (i / (out + 1) > maxint - n) return punycode_overflow;
        n += i / (out + 1);
        i %= (out + 1);

        /* Insert n at position i of the output: */

        /* not needed for Punycode: */
        /* if (decode_digit(n) <= base) return punycode_invalid_input; */
        if  (out >= max_out) return punycode_big_output;

        if  (case_flags) {
            memmove(case_flags + i + 1, case_flags + i, out - i);

            /* Case of last character determines uppercase flag: */
        case_flags[i] = flagged(input[in - 1]);
        }

        memmove(output + i + 1, output + i, (out - i) * sizeof *output);
        output[i++] = n;
    }

    *output_length = out;
    return punycode_success;
}

#if 0

/******************************************************************/
/* Wrapper for testing (would normally go in a separate .c file): */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* For testing, we'll just set some compile-time limits rather than */
/* use malloc(), and set a compile-time option rather than using a  */
/* command-line option.                                             */

enum {
    unicode_max_length = 256,
    ace_max_length = 256
};

static void usage(char **argv)
{
    fprintf(stderr,
      "\n"
      "%s -e reads code points and writes a Punycode string.\n"
      "%s -d reads a Punycode string and writes code points.\n"
      "\n"
      "Input and output are plain text in the native character set.\n"
      "Code points are in the form u+hex separated by whitespace.\n"
      "Although the specification allows Punycode strings to contain\n"
      "any characters from the ASCII repertoire, this test code\n"
      "supports only the printable characters, and needs the Punycode\n"
      "string to be followed by a newline.\n"
      "The case of the u in u+hex is the force-to-uppercase flag.\n"
      , argv[0], argv[0]);
    exit(EXIT_FAILURE);
}

static void fail(const char *msg)
{
    fputs(msg,stderr);
    exit(EXIT_FAILURE);
}

static const char too_big[] =
"input or output is too large, recompile with larger limits\n";
static const char invalid_input[] = "invalid input\n";
static const char overflow[] = "arithmetic overflow\n";
static const char io_error[] = "I/O error\n";

/* The following string is used to convert printable */
/* characters between ASCII and the native charset:  */

static const char print_ascii[] =
"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
" !\"#$%&'()*+,-./"
"0123456789:;<=>?"
"@ABCDEFGHIJKLMNO"
"PQRSTUVWXYZ[\\]^_"
"`abcdefghijklmno"
"pqrstuvwxyz{|}~\n";

int main(int argc, char **argv)
{
    enum punycode_status status;
    int r;
    unsigned int input_length, output_length, j;
    unsigned char case_flags[unicode_max_length];

    if  (argc != 2) usage(argv);
    if  (argv[1][0] != '-') usage(argv);
    if  (argv[1][2] != 0) usage(argv);

    if  (argv[1][1] == 'e') {
        punycode_uint input[unicode_max_length];
        unsigned long codept;
        char output[ace_max_length+1], uplus[3];
        int c;

        /* Read the input code points: */

        input_length = 0;

        for (;;) {
            r = scanf("%2s%lx", uplus, &codept);
            if  (ferror(stdin)) fail(io_error);

            if  (r == EOF || r == 0) break;

            if  (r != 2 || uplus[1] != '+' || codept > (punycode_uint)-1) {
                fail(invalid_input);
            }

            if  (input_length == unicode_max_length) fail(too_big);

            if  (uplus[0] == 'u') case_flags[input_length] = 0;
            else if (uplus[0] == 'U') case_flags[input_length] = 1;
            else fail(invalid_input);

            input[input_length++] = codept;
        }

        /* Encode: */

        output_length = ace_max_length;
        status = punycode_encode(input_length, input, case_flags,
                                 &output_length, output);
        if  (status == punycode_bad_input) fail(invalid_input);
        if  (status == punycode_big_output) fail(too_big);
        if  (status == punycode_overflow) fail(overflow);
        assert(status == punycode_success);

        /* Convert to native charset and output: */

        for (j = 0;  j < output_length;  ++j) {
            c = output[j];
            assert(c >= 0 && c <= 127);
            if  (print_ascii[c] == 0) fail(invalid_input);
            output[j] = print_ascii[c];
        }

        output[j] = 0;
        r = puts(output);
        if  (r == EOF) fail(io_error);
        return EXIT_SUCCESS;
    }

    if  (argv[1][1] == 'd') {
        char input[ace_max_length+2], *p, *pp;
        punycode_uint output[unicode_max_length];

        /* Read the Punycode input string and convert to ASCII: */

        fgets(input, ace_max_length+2, stdin);
        if  (ferror(stdin)) fail(io_error);

        if  (feof(stdin)) fail(invalid_input);
        input_length = strlen(input) - 1;
        if  (input[input_length] != '\n') fail(too_big);
        input[input_length] = 0;

        for (p = input;  *p != 0;  ++p) {
            pp = strchr(print_ascii, *p);
            if  (pp == 0) fail(invalid_input);
            *p = pp - print_ascii;
        }

        /* Decode: */

        output_length = unicode_max_length;
        status = punycode_decode(input_length, input, &output_length,
                                 output, case_flags);
        if  (status == punycode_bad_input) fail(invalid_input);
        if  (status == punycode_big_output) fail(too_big);
        if  (status == punycode_overflow) fail(overflow);
        assert(status == punycode_success);

        /* Output the result: */

        for (j = 0;  j < output_length;  ++j) {
            r = printf("%s+%04lX\n",
                       case_flags[j] ? "U" : "u",
                       (unsigned long) output[j] );
            if  (r < 0) fail(io_error);
        }

        return EXIT_SUCCESS;
    }

    usage(argv);
    return EXIT_SUCCESS;  /* not reached, but quiets compiler warning */
}
#endif
//----------------------------------------------------------------------------------------
