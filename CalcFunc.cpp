
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/CalcFunc.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file is integer function from string parser and implementation.
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

//#define GLOCK_LIB2 gLock_lib2

/*
#include "../MaaMfcLib/MaaMfcLib.h"
#include "perm.h"

#include "utf8_printf_.h"

#include "CalcFunc.h"
*/

#ifdef _DEBUG

#define printf __utf8_printf
#define printf2 __utf8_printf2

//#define _print_debug_to_console

#endif

//#define debug_ops dbg_printf

#ifndef _print_debug_to_console
static int dbg_printf(...) {return 0;}
static int dbg_printf2(...) {return 0;}

//#define debug_ops printf

#else
#define dbg_printf printf
#define dbg_printf2 printf2
#define debug_ops printf
#endif

constexpr CMaaTLAEGlobalStrings::CMaaTLAEGlobalStrings() noexcept
:   c
    {
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

        //"End"
    }
    //, m{}
{
    for (int i = 0; i < e_max; i++)
    {
        m[i] = &c[i];
    }
#ifdef CMaaStringImp_NOT_NULL_MODE
    //m[CMaaTLAEGlobalStrings::e_max] = &CMaaString::C.snZ;
#else
    ////m[CMaaTLAEGlobalStrings::e_max] = &CMaaString::C.sn0; // complex test only
    ////m[CMaaTLAEGlobalStrings::e_max] = nullptr;
#endif
}

static constexpr CMaaTLAEGlobalStrings s_CMaaTLAEGlobalStrings;
const CMaaString& CMaaTLAEGlobalStrings::Get(int_ _e) noexcept
{
    return (const CMaaString&)s_CMaaTLAEGlobalStrings.m[_e];
}


#define THROW_CALC_FUNCTION_ERROR(txt, n) throw sCalkFuncErr(txt, n, expr)

static constexpr int PrecedenceLevel[] = {0, 1, 1, 1, 2, 2, 3, 3, 5, 5, 5, 5, 5, 5, 7, 7, 7, 8, 8, 100, 101, 102, 103, 104, 105};

static constexpr _qword _qw_multiplicators[] =
{
    1024 / 8, 1024 * 1024 / 8, 1024 * 1024 * 1024 / 8, 1024LL * 1024 * 1024 * 1024 / 8,
    1024 / 8, 1024 * 1024 / 8, 1024 * 1024 * 1024 / 8, 1024LL * 1024 * 1024 * 1024 / 8,
    1, 1024, 1024 * 1024, 1024 * 1024 * 1024, 1024LL * 1024 * 1024 * 1024,
    1, 1024, 1024 * 1024, 1024 * 1024 * 1024, 1024LL * 1024 * 1024 * 1024,
    1, 60, 60 * 60, 24 * 60 * 60, 1,
    0
};
static constexpr _qword _qw_sp_ch_multiplicators[] = { 1, 60, 60 * 60, 24 * 60 * 60, 1, 60, 60 * 60, 24 * 60 * 60 };
CArithmeticExpression::CArithmeticExpression(CFuncCaller * pFnCaller, int Style)
:   m_Style(Style),
    szOperators(0),
    //     szFunctions(0),
    szFunctions1(0),
    szFunctions2(0),
    sz_modificators(0),
    qw_multiplicators(0),
    qw_sp_ch_multiplicators(0),
    m_pFnCaller(pFnCaller)
{
    {
        const int n = 25;// (int)(sizeof(_szOperators) / sizeof(_szOperators[0]));
        CMaaPtr_<CMaaString, 1> __szOperators(n);
        m_nOpenBracketsArithIfIndex = 0;
        if  (__szOperators.IsValid())
        {
            for (int i = 0; i < n; i++)
            {
                __szOperators[i] = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae_Operators + i);// _szOperators[i];
                if  (__szOperators[i] == "(")
                {
                    m_nOpenBracketsArithIfIndex = i;
                }
            }
            szOperators.Swap(__szOperators);
        }
    }

    {
        const int n = 9;// (int)(sizeof(_szFunctions1) / sizeof(_szFunctions1[0]));
        CMaaPtr_<CMaaString, 1> __szFunctions1(n);
        if  (__szFunctions1.IsValid())
        {
            for (int i = 0; i < n; i++)
            {
                __szFunctions1[i] = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__szFunctions1 + i);// _szFunctions1[i];
            }
            szFunctions1.Swap(__szFunctions1);
        }
    }

    {
        const int n = 4;// (int)(sizeof(_szFunctions2) / sizeof(_szFunctions2[0]));
        CMaaPtr_<CMaaString, 1> __szFunctions2(n);
        if  (__szFunctions2.IsValid())
        {
            for (int i = 0; i < n; i++)
            {
                __szFunctions2[i] = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__szFunctions2 + i);// _szFunctions2[i];
            }
            szFunctions2.Swap(__szFunctions2);
        }
    }

    {
        const int n = 24; // (int)(sizeof(_sz_modificators) / sizeof(_sz_modificators[0]));
        const int n2 = (int)(sizeof(_qw_multiplicators) / sizeof(_qw_multiplicators[0]));
        CMaaPtr_<CMaaString, 1> __sz_modificators(n);
        CMaaPtr_<_qword, 1> __qw_multiplicators(n);
        if  (__sz_modificators.IsValid() && __qw_multiplicators.IsValid())
        {
            for (int i = 0; i < n; i++)
            {
                __sz_modificators[i] = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__modificators + i);// _sz_modificators[i];
                if  (i < n2)
                {
                    __qw_multiplicators[i] = _qw_multiplicators[i];
                }
            }
            sz_modificators.Swap(__sz_modificators);
            qw_multiplicators.Swap(__qw_multiplicators);
        }
    }

    sz_Modificators_1st_chars_1 = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__Modificators_1st_chars_1);// _sz_Modificators_1st_chars_1;
    sz_Modificators_1st_chars_2 = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__Modificators_1st_chars_2);// _sz_Modificators_1st_chars_2;

    {
        const int n2 = (int)(sizeof(_qw_sp_ch_multiplicators) / sizeof(_qw_sp_ch_multiplicators[0]));
        sz_ch_modificators = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__ch_modificators);// _sz_ch_modificators;
        sz_sp_ch_modificators = CMaaTLAEGlobalString(CMaaTLAEGlobalStrings::e_ae__sp_ch_modificators);// _sz_sp_ch_modificators;
        CMaaPtr_<_qword, 1> __qw_sp_ch_multiplicators(n2);
        if  (__qw_sp_ch_multiplicators.IsValid())
        {
            for (int i = 0; i < n2; i++)
            {
                __qw_sp_ch_multiplicators[i] = _qw_sp_ch_multiplicators[i];
            }
            qw_sp_ch_multiplicators.Swap(__qw_sp_ch_multiplicators);
        }
    }
}

CArithmeticExpression::~CArithmeticExpression()
{
    CExprCache * p;
    while((p = m_CacheList.GetFromFront()))
    {
        m_hCache.Remove(p->expr);
        delete p;
    }
}

void CArithmeticExpression::SetArgFunctions(CMaaString * pFunctionNames, int Count)
{
    //static CMaaString _szFunctions1[] = {"ruleconnections", "shaperconnections", "time", "v", "traffic", "url", "host", "test", ""};
    const int n = Count > 0 && pFunctionNames[Count - 1].IsEmpty() ? Count : Count + 1;
    CMaaPtr_<CMaaString, 1> __szFunctions1(n);
    if  (__szFunctions1.IsValid())
    {
        for (int i = 0; i < n && i < Count; i++)
        {
            __szFunctions1[i] = pFunctionNames[i];
        }
        //__szFunctions1[n - 1].Empty();
        szFunctions1.Swap(__szFunctions1);
    }
}

void CArithmeticExpression::SetKeywordTypeFunctions(CMaaString * pFunctionNames, int Count)
{
    const int n = Count + 1;
    CMaaPtr_<CMaaString, 1> __szFunctions2(n);
    if  (__szFunctions2.IsValid())
    {
        for (int i = 0; i < n - 1; i++)
        {
            __szFunctions2[i] = pFunctionNames[i];
        }
        //__szFunctions2[n - 1].Empty();
        szFunctions2.Swap(__szFunctions2);
    }
}

int CArithmeticExpression::CalcFunc1(int Fn, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, int index, int nArgs, _qword &qResult, bool &bResult, int &ResultType, int Mode)
{
    CMaaString FunctionName;
    if  (Fn >= 0 && Fn < (int)szFunctions1.MaxIndex())
    {
        FunctionName = szFunctions1[Fn];
    }
    if  (m_pFnCaller)
    {
        return ResultType = m_pFnCaller->Func1(Fn, FunctionName, hArgs, hlArgs, index, nArgs, qResult, bResult, Mode, *this);
    }
    CFuncCaller TemporarilyCall;
    return ResultType = TemporarilyCall.Func1(Fn, FunctionName, hArgs, hlArgs, index, nArgs, qResult, bResult, Mode, *this);
}
int CArithmeticExpression::CalcFunc2(int Fn, _qword &qResult, bool &bResult, int &ResultType, int Mode)
{
    CMaaString FunctionName;
    if  (Fn >= 0 && Fn < (int)szFunctions2.MaxIndex())
    {
        FunctionName = szFunctions2[Fn];
    }
    if  (m_pFnCaller)
    {
        return ResultType = m_pFnCaller->Func2(Fn, FunctionName, qResult, bResult, Mode, *this);
    }
    CFuncCaller TemporarilyCall;
    return ResultType = TemporarilyCall.Func2(Fn, FunctionName, qResult, bResult, Mode, *this);
}

/*
Operator       Name or Meaning               Associativity

::             Scope resolution              None
.              Member selection (object)     Left to right
->             Member selection (pointer)    Left to right
[]             Array subscript               Left to right
()             Function call member initialization     Left to right
++             Postfix increment             Left to right
--             Postfix decrement             Left to right
typeid()       type name                     Left to right
const_cast     Type cast (conversion)        Left to right
dynamic_cast   Type cast (conversion)        Left to right
reinterpret_cast Type cast (conversion)      Left to right
static_cast    Type cast (conversion)        Left to right
sizeof         Size of object or type        Right to left
++             Prefix increment              Right to left
--             Prefix decrement              Right to left
~              One's complement              Right to left
!              Logical not                   Right to left
-              Unary minus                   Right to left
+              Unary plus                    Right to left
&              Address-of                    Right to left
*              Indirection                   Right to left
new            Create object                 Right to left
delete         Destroy object                Right to left
()             Cast                          Right to left
.*             Pointer-to-member (objects)   Left to right
->*            Pointer-to-member (pointers)  Left to right
*              Multiplication                Left to right
/              Division                      Left to right
%              Modulus                       Left to right
+              Addition                      Left to right
-              Subtraction                   Left to right
<<             Left shift                    Left to right
>>             Right shift                   Left to right
<              Less than                     Left to right
>              Greater than                  Left to right
<=             Less than or equal to         Left to right
>=             Greater than or equal to      Left to right
==             Equality                      Left to right
!=             Inequality                    Left to right
&              Bitwise AND                   Left to right
^              Bitwise exclusive OR          Left to right
|              Bitwise inclusive OR          Left to right
&&             Logical AND                   Left to right
||             Logical OR                    Left to right
e1?e2:e3       Conditional                   Right to left
=              Assignment                    Right to left
*=             Multiplication assignment     Right to left
/=             Division assignment           Right to left
%=             Modulus assignment            Right to left
+=             Addition assignment           Right to left
-=             Subtraction assignment        Right to left
<<=            Left-shift assignment         Right to left
>>=            Right-shift assignment        Right to left
&=             Bitwise AND assignment        Right to left
|=             Bitwise inclusive OR assignment Right to left
^=             Bitwise exclusive OR assignment Right to left
throw expr     throw expression              Right to left
,              Comma                         Left to right
*/

static CMaaString szFunctions[] = {"ruleconnections", "shaperconnections", "time", "v", "traffic", "url", "host", "false", "true", "break", ""};
static CMaaString szFunctions1[] = {"ruleconnections", "shaperconnections", "time", "v", "traffic", "url", "host", ""};
static CMaaString szFunctions2[] = {"false", "true", "break", ""};
static CMaaString sz_modificators[] = {"kbit/s", "mbit/s", "gbit/s", "tbit/s", "kbits/s", "mbits/s", "gbits/s", "tbits/s", "b/s", "kb/s", "mb/s", "gb/s", "tb/s", "b", "kb", "mb", "gb", "tb", "s", "m", "h", "d", ":", ""};
static CMaaString sz_Modificators_1st_chars_1 = " :smhdSMHDbkmgtBKMGT)";
static CMaaString sz_Modificators_1st_chars_2 = "  smhdSMHDbkmgtBKMGT)";
static constexpr _qword qw_multiplicators[] =
{
    1024 / 8, 1024 * 1024 / 8, 1024 * 1024 * 1024 / 8, 1024LL * 1024 * 1024 * 1024 / 8,
    1024 / 8, 1024 * 1024 / 8, 1024 * 1024 * 1024 / 8, 1024LL * 1024 * 1024 * 1024 / 8,
    1, 1024, 1024 * 1024, 1024 * 1024 * 1024, 1024LL * 1024 * 1024 * 1024,
    1, 1024, 1024 * 1024, 1024 * 1024 * 1024, 1024LL * 1024 * 1024 * 1024,
    1, 60, 60 * 60, 24 * 60 * 60, 1,
    0
};

int CArithmeticExpression::GetLexem(CMaaString expr, int &index, int &begin, int &end, int &beginArg, int &endArg, int PrevLexemArIf, _qword &qwNumber, CMaaString * pArgs, CMaaString * plArgs, int &nArgsNum, int iUnarMinusIsAllowed)
{
    int pos = begin;
    while(pos < end && expr[pos] == ' '/* && Parenthesis[pos] == '\0'*/) pos++;
    if  (pos >= end)
    {
        return eEnd;
    }
    if  (expr[pos] == '(')// || Parenthesis[pos] == '(')
    {
        begin = pos + 1;
        beginArg = endArg = begin;
        index = 0;
        return eOperator;
    }
    if  (expr[pos] == ')')// || Parenthesis[pos] == ')')
    {
        begin = pos + 1;
        beginArg = endArg = begin;
        index = -1;
        return eOperator;
    }
    int FoundOperator = -1;
    for (int opn = 1; /*szOperators[opn].Length()*/ opn < (int)szOperators.MaxIndex() - 2; opn++)
    {
        if  (szOperators[opn].IsNotEmpty() && pos + szOperators[opn].Length() <= end && expr.RefMid(pos, szOperators[opn].Length()) == szOperators[opn] && (FoundOperator < 0 || szOperators[opn].Length() > szOperators[FoundOperator].Length()))
        {
            FoundOperator = opn;
        }
    }
    if  (FoundOperator > 0)
    {
        if  (!iUnarMinusIsAllowed && (FoundOperator == 4 || FoundOperator == 5)) // '+', '-'
        {
            //d0 = d;
        }
        else
        {
            //THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("unexpected operator %S at position %d", &szOperators[FoundOperator], pos + 1), pos);
        }
        begin = pos + (warning_int)szOperators[FoundOperator].Length();
        beginArg = endArg = begin;
        index = FoundOperator;
        return eOperator;
    }

    if  (expr[pos] >= '0' && expr[pos] <= '9')
    {
        _qword x = 0;
        int nNum = 0;
        //for  (nNum = 0; ; nNum++)
        {
            if  (expr[pos] < '0' || expr[pos] > '9')
            {
                //break;
            }

            int StopPos = 0;
            bool btmp = mysscanf64(pos + expr, &x, -1, &StopPos, !PrevLexemArIf ? sz_Modificators_1st_chars_1 : sz_Modificators_1st_chars_2);
            if  (!btmp)
            {
                btmp = mysscanf64(pos + expr, &x, StopPos, &StopPos, !PrevLexemArIf ? sz_Modificators_1st_chars_1 : sz_Modificators_1st_chars_2);
            }
            //__utf8_printf("<1> %d\n", pos);
            pos += StopPos;
            //__utf8_printf("<2> %d\n", pos);
            int ch = (unsigned char)(char)expr[pos];
            while(expr[pos] == ' ') pos++;
            //__utf8_printf("<3> %d\n", pos);
            int i;
            for (i = 0; sz_modificators[i].Length(); i++)
            {
                const int l = (warning_int)sz_modificators[i].Length();
                CMaaString TestMod = expr.RefMid(pos, l);//.ToLower(0);
                if  (TestMod.IsCi(sz_modificators[i], 0) && TestMod != ":")//(!PrevLexemArIf || sz_modificators[i] != ":"))
                {
                    x *= qw_multiplicators[i];
                    pos += l;
                    ch = -1;
                    break;
                }
            }

            if  (ch < 0)
            {
                ch = (unsigned char)(char)expr[pos];
            }
            while(expr[pos] == ' ') pos++;

            if  (nNum == 0)
            {
                if  (expr[pos] >= '0' && expr[pos] <= '9')
                {
                    if  (sz_modificators[i].IsEmpty() || ch/*sz_modificators[i][0]*/ == ':')
                    {
                        _qword ds = 0;
                        if  (sz_modificators[i].IsEmpty())
                        {
                            ds = x * 24 * 3600;
                            x = 0;
                        }
                        while(expr[pos] >= '0' && expr[pos] <= '9')
                        {
                            _qword xx = 0;
                            int StopPos = 0;
                            if  (!mysscanf64(pos + expr, &xx, -1, &StopPos, ":) "))
                            {
                                mysscanf64(pos + expr, &xx, -1, &StopPos, ":) ");
                            }
                            pos += StopPos;
                            x = x * 60 + xx;
                            if  (expr[pos] == ':')
                            {
                                pos++;
                                continue;
                            }
                            break;
                        }
                        x += ds;
                    }
                    const int ns = (warning_int)sz_Modificators_1st_chars_1.Find('s');
                    const int nD = (warning_int)sz_Modificators_1st_chars_1.Find('D');
                    if  (sz_modificators[i].IsNotEmpty() && i >= ns && i <= nD)
                    {
                        while(expr[pos] >= '0' && expr[pos] <= '9')
                        {
                            _qword xx = 0;
                            //int pos0 = pos;
                            int StopPos = 0;
                            if  (!mysscanf64(pos + expr, &xx, -1, &StopPos, sz_sp_ch_modificators))
                            {
                                mysscanf64(pos + expr, &xx, -1, &StopPos, sz_sp_ch_modificators);
                            }
                            pos += StopPos;
                            //while(expr[pos] == ' ') pos++;
                            const int fct = expr[pos] != ' ' ? (warning_int)sz_sp_ch_modificators.Find(expr[pos]) : -1;
                            if  (fct >= 0)
                            {
                                pos++;
                                xx *= qw_sp_ch_multiplicators[fct];
                            }
                            x += xx;
                            while(expr[pos] == ' ') pos++;
                        }
                    }
                }
            }
            nNum++;
            //break;
        }
        qwNumber = x;
        index = 0;
        begin = pos;
        beginArg = endArg = pos;
        return eNumber;
    }

    //int argn = 0;
    //CMaaString args[2][10];
    int i;
    for (i = 0; szFunctions1[i].Length(); i++)
    {
        const int l = (warning_int)szFunctions1[i].Length();
        if  (pos + l <= end && expr.IsMidCi(szFunctions1[i], pos, 0))
        {
            //               __lib_printf("func: %S, argstext = %S\n", &szFunctions1[i], &expr.Mid(pos + l,end - (pos + l)));
            //               int tmp____ = szFunctions1[i] == "getvar" ? 1 : 0;
            //try
            //{
            pos += l;
            while(expr[pos] == ' ') pos++;

            if  (expr[pos] != '(' || pos >= end)
            {
                if  (i == 1)
                {
                    beginArg = endArg = pos;
                    //__lib_printf("temp_point_1\n");
                    break;
                }
                THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed opening '(' for a function %S at position %d", &szFunctions1[i], pos + 1), pos);
            }

            ++pos;
            int arg_begin = pos;
            int argn = 0;
            char che = 0;
            while(che != ')')
            {
                while(pos < end && expr[pos] == ' ') pos++;
                CMaaString arg;
                if  (pos >= end)
                {
                    THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("unexpected end of arguments lists for a function %S at position %d", &szFunctions1[i], pos + 1), pos);
                }
                char ch = expr[pos];
                if  (ch == '"')
                {
                    const int n = (warning_int)expr.Find(pos + 1, '"', end);
                    if  (n < 0)
                    {
                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed closing '\"' for a function %S argument %d at position beginning from %d", &szFunctions1[i], argn + 1, pos + 2), pos + 1);
                    }
                    int n2 = n + 1;
                    while(expr[n2] == ' ' && n2 < end)
                    {
                        n2++;
                    }
                    if  (n2 < end && (expr[n2] == ',' || expr[n2] == ')'))
                    {
                        arg = CMaaString(expr, pos, n + 1 - pos);
                        pos = n2;
                        if  (expr[n2] == ')')
                        {
                            //ch = expr[n2];
                        }
                        else
                        {
                            pos++;
                        }
                    }
                    else
                    {
                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed ',' or closing ')' for a function %S argument %d at position %d", &szFunctions1[i], argn + 1, n2 + 1), n2);
                    }
                }
                if  (ch != '"' && ch != ')')
                {
                    int pos2 = pos;
                    /*
                         while(pos2 < end && ((expr[pos2] >= 'a' && expr[pos2] <= 'z') ||
                                expr[pos2] == '_' ||
                                (expr[pos2] >= 'A' && expr[pos2] <= 'Z') ||
                                (pos2 > pos + 1 && expr[pos2] >= '0' && expr[pos2] <= '9')
                                )
                         {
                              pos2++;
                         }
                         */
                    //int pos3 = -1;
                    //if   (pos2 > pos + 1)
                    {
                        /*
                              while(pos2 < end && expr[pos2] == ' ')
                              {
                                   pos2++;
                              }
                              */
                        //if   (pos2 < end && expr[pos2] == '(')
                        if  (pos2 < end)
                        {
                            int n = 0;
                            char ch_ = 0;
                            while(pos2 < end && n >= 0)
                            {
                                ch = expr[pos2];
                                if  (ch == '"')
                                {
                                    ch_ ^= ch;
                                }
                                if  (!ch_)
                                {
                                    if  (ch == '(')
                                    {
                                        n++;
                                    }
                                    if  (ch == ')')
                                    {
                                        n--;
                                        if  (n < 0)
                                        {
                                            che = ch;
                                            arg = CMaaString(expr, pos, pos2 - pos);
                                            pos = pos2 + 1;
                                            break;
                                        }
                                    }
                                    if  (n == 0 && ch == ',')
                                    {
                                        arg = CMaaString(expr, pos, pos2 - pos);
                                        pos = pos2 + 1;
                                        break;
                                    }
                                }
                                pos2++;
                            }
                        }
                    }
                    if  (pos <= pos2)
                    {
                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed closing ',' or ')' for a function %S at position beginning from %d", &szFunctions1[i], pos + 1), pos);
                    }
                    /*
                         int n = expr.Find(pos + 1, ',', end);
                         int n2 = expr.Find(pos + 1, ')', end);
                         if   (n < 0 && n2 < 0)
                         {
                              THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed closing ',' or ')' for a function %S at position beginning from %d", &szFunctions1[i], pos + 2), pos + 1);
                         }
                         n = n >= 0 && (n < n2 || n2 < 0) ? n : n2;
                         arg = CMaaString(expr, pos + 1, n - (pos + 1));
                         che = expr[n];
                         pos = n + 1;
                         */
                }
                else if (ch == ')')
                {
                    //
                    pos++;
                    break;
                }
                if  (argn < nArgsNum)
                {
                    if  (pArgs)
                    {
                        pArgs[argn] = arg;
                    }
                    if  (plArgs)
                    {
                        plArgs[argn] = arg.ToLower(e_string_depending_all); // !!2020!! supporting russian letters
                    }
                    //                         if   (tmp____)
                    //                         {
                    //                              __lib_printf("arg[%d] = %S\n", argn, &arg);
                    //                         }
                }
                argn++;
            }
            index = i;
            begin = pos;
            beginArg = arg_begin;
            endArg = pos - 1;
            nArgsNum = argn;
            //}
            //catch(CArithmeticExpression::sCalkFuncErr err)
            //{
            //     __lib_printf("%S\n", &err.GetMsg());
            //}
            return eFunc1;
        }
    }

    for (i = 0; szFunctions2[i].Length(); i++)
    {
        const int l = (warning_int)szFunctions2[i].Length();
        if  (pos + l <= end && expr.IsMidCi(szFunctions2[i], pos, 0))
        {
            pos += l;
            index = i;
            begin = pos;
            beginArg = endArg = pos;
            nArgsNum = 0;
            return eFunc2;
        }
    }

    begin = pos;
    return eError;
}

int CArithmeticExpression::CheckForValidity(CMaaUnivHash<int, op_data> &hop, int hindex0, int hindex, int ProcessingTaskType)
{
    if  (hindex == hindex0)
    {
        return ProcessingTaskType == 1 ? 0 : 1;
    }
    int ResType = -1;
    int i = hindex0;
    //op_data d0;
    while(i < hindex)
    {
        op_data d;
        hop.Find(i, &d);
        if  (d.op == eEnd)
        {
            break;
        }
        if  (d.op == eOperator)
        {
            if  (d.index == 0)
            {
                if  (d.nextindex > i && d.nextindex <= hindex)
                {
                    if  (CheckForValidity(hop, i + 1, d.nextindex, 1))
                    {
                        i = d.nextindex - 1;
                        d.op = eNumber;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
            else
            {
                if  (i == hindex0)
                {
                    if  (d.index == 4 || d.index == 5) // '+', '-'
                    {
                        //d0 = d;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
        }
        if  (d.op == eOperator && ResType != eOperator)
        {
            ResType = eOperator;
        }
        else if (d.op != eOperator && ResType != eNumber)
        {
            ResType = eNumber;
        }
        else
        {
            return 0;
        }
        i++;
    }
    if  (ResType != eOperator)
    {
        return 1;
    }
    return 0;
}

int CArithmeticExpression::Pass1CalcFunction(CMaaString expr, int &begin, int &end, int &hindex, CMaaUnivHash<int, op_data> &hop, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, CMaaUnivHash<int, _qword> &hqwConstants, int ProcessingTaskType)
{
    //CMaaUnivHash<int, CMaaString> hArgs, hlArgs;
    //CMaaUnivHash<int, _qword> hqwConstants;
    //CMaaUnivHash<int, int> hboolConstants;

    const int hindex0 = hindex;
#if 0
    if  (ProcessingTaskType = 0)
    {
        if  (Parenthesis.Length() != expr.Length())
        {
            Parenthesis = CMaaString(nullptr, expr.Length());
        }
        /*
          int np = 0;
          for  (int i = 0; i < expr.Length(); i++)
          {
               char c = expr[i];
               if   (c == '(')
               {
                    hop.Add(np++, i);
               }
               else if (c == ')')
               {
                    //
               }
          }
          */
    }
#endif
    int ResType = -1;

    const int PrevLexemArIf = 0;
    //int begin = 0, end = expr.Length();
    //int PrevLexem = eReserved;
    int start_par = -1;
    const int constMaxArgs = 100;
    CMaaString Args[constMaxArgs], lArgs[constMaxArgs];
    while(begin < end)
    {
        int index;
        _qword qwNumber = 0;
        int nArgsNum = constMaxArgs;
        int beginArg, endArg;
        const int pos0 = begin;
        const int Lexem = GetLexem(expr, index, begin, end, beginArg, endArg, PrevLexemArIf, qwNumber, (CMaaString *)Args, (CMaaString *)lArgs, nArgsNum, hindex == hindex0 ? 1 : 0);

        dbg_printf("Lexem=%d, index=%d, qwNumber=%D\n", Lexem, index, qwNumber);

        nArgsNum = nArgsNum >= 0 && nArgsNum < constMaxArgs ? nArgsNum : 0;

        int pos = pos0;
        while(expr[pos] == ' ' && pos < end) pos++;
        op_data d;
        d.op = Lexem;
        d.index = index;
        d.nextindex = hindex + 1;
        d.nextpos = -1;
        d.nargs = Lexem == eFunc1 ? nArgsNum : 0;
        d.pos = pos;
        hop.Add(hindex, d);
        hindex++;
        if  (Lexem == eNumber || Lexem == eFunc1 || Lexem == eFunc2 || (Lexem == eOperator && (index == 0 || index == -1)))
        {
            if  (Lexem == eOperator)
            {
                if  (index == 0)
                {
                    start_par = begin - 1;
                    const int hindex0 = hindex - 1;
                    if  (Pass1CalcFunction(expr, begin, end, hindex, hop, hArgs, hlArgs, hqwConstants, 1))
                    {
                        d.nextindex = hindex;
                        d.nextpos = begin;
                        hop.AddOver(hindex0, d);
                        start_par = -1;
                    }
                    else
                    {
                        // error
                        return 0;
                    }
                    d.op = eNumber;
                }
                else
                {
                    --hindex;
                    hop.Remove(hindex);
                    if  (ProcessingTaskType == 1)
                    {
                        return CheckForValidity(hop, hindex0, hindex, ProcessingTaskType);
                    }
                    THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("unexpected ')' at position %d", begin), begin - 1);
                    return 0;
                    /*
                         if   (expr[start_par] == '(')
                         {
                              start_par = -1;
                         }
                         */
                }
            }
            else if (Lexem == eNumber)
            {
                hqwConstants.Add(hindex - 1, qwNumber);
            }
            else if (Lexem == eFunc1)
            {
                for (int i = 0; i < nArgsNum && i < constMaxArgs; i++)
                {
                    hArgs.Add(((hindex - 1) << 16) + i, Args[i]);
                    hlArgs.Add(((hindex - 1) << 16) + i, lArgs[i]);
                }
            }
            /*
               if   (hindex == hindex0 + 1)
               {
                    if   (d.index == 4 || d.index == 5) // '+', '-'
                    {
                         //d0 = d;
                    }
                    else
                    {
                         return 1;
                    }
               }
               */
            /*
               if   (PrevLexem == eOperator || Lexem == eReserved)
               {
                    PrevLexem = Lexem;
                    continue;
               }
               while(pos0 < end && expr[pos0] == ' ')// && Parenthesis[pos0] == 0)
               {
                    pos0++;
               }
               THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed operator at position %d", pos + 1), pos);
               */
        }
        /*
          if   (d.op == eError && (expr[pos] == '?' || expr[pos] == ':'))
          {
               int i = 0;
               for  (; szOperators[i][0] != expr[pos]; )
               {
                    i++;
               }
               begin = pos + 1; //szOperators[i].Length()
               d.op = eOperator;
               d.index = index = i;
               //d.nextindex = hindex;
               //d.nextpos = -1;
               hop.AddOver(hindex - 1, d);
          }
          */
        if  (d.op == eError)
        {
            THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("syntax error at position %d", pos + 1), pos);
        }
        if  (Lexem == eEnd)
        {
            break;
        }

        if  (d.op == eOperator && ResType != eOperator)
        {
            ResType = eOperator;
        }
        else if (d.op != eOperator && ResType != eNumber)
        {
            ResType = eNumber;
        }
        else if (d.op == eOperator)
        {
            THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator following by operator is found at position %d", pos + 1), pos);
        }
        else
        {
            THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("expected operator at position %d", pos + 1), pos);
        }
        /*
          if   (Lexem == eError)
          {
               if   (PrevLexem == eOperator)
               {
                    THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator following by operator is found at position %d", pos + 1), pos);
               }
               PrevLexem = Lexem;
               continue;
          }
          */
    }
    //
    return CheckForValidity(hop, hindex0, hindex, ProcessingTaskType);
}

bool CArithmeticExpression::IsInCache(CMaaString expr) noexcept
{
    CExprCache * cache = nullptr;
    m_hCache.Find(expr, &cache);
    return cache != nullptr;
}

void CArithmeticExpression::RemoveFromCache(CMaaString expr) noexcept
{
    CExprCache * cache = nullptr;
    if  (!m_hCache.Remove(expr, &cache) && cache)
    {
        m_CacheList.Release(cache);
        delete cache;
        --m_nExprCacheCount;
    }
}

CMaaString CArithmeticExpression::op_data::GetText(int nn, CArithmeticExpression *expr, const CMaaUnivHash<int, CMaaString> &_hArgs, const CMaaUnivHash<int, CMaaString> &_hlArgs, CMaaPtr_<_qword, 1> &qResults, CMaaPtr_<bool, 1> &bResults, CMaaPtr_<int, 1> &ResultTypes)
{
    return GetText(nn, expr, _hArgs, _hlArgs, (_qword *)qResults, (bool *)bResults, (int *)ResultTypes);
}

CMaaString CArithmeticExpression::op_data::GetText(int nn, CArithmeticExpression *expr, const CMaaUnivHash<int, CMaaString> &_hArgs, const CMaaUnivHash<int, CMaaString> &/*_hlArgs*/, _qword *qResults, bool * /*bResults*/, int * /*ResultTypes*/)
{
    CMaaString txt;
    {
        CMaaString szOperators[] =
        {
            "(",
            "*", "/", "%", "+", "-", "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "&", "^", "|", "&&", "||",
            "", ":", "?", "", "(", ""
        };
        txt.Format("%2d. ", nn);

        switch(op)
        {
        case eReserved:
            txt.Format("%Sreserved", &txt);
            break;
        case eError:
            txt.Format("%Serror", &txt);
            break;
        case eOperator:
            txt.Format("%S%S", &txt, &szOperators[index]);
            break;
        case eNumber:
            txt.Format("%S[%D]", &txt, qResults[newindex]);
            break;
        case eFunc1:
            {
                CMaaString arg;
                const int index_ = newindex;
                const int nArgs = nargs;
                for (int i = 0; i < nArgs; i++)
                {
                    CMaaString tmp;
                    _hArgs.Find((index_ << 16) + i, &tmp);
                    arg = i == 0 ? tmp : (arg + ", " + tmp).s();
                }
                txt.Format("%S%S(%S)", &txt, &expr->szFunctions1[index], &arg);
            }
            break;
        case eFunc2: // with calculating of arguments expressions
            txt.Format("%S%S", &txt, &expr->szFunctions2[index]);
            break;
        case eEnd:
            txt.Format("%Send", &txt);
            break;
        default:
            txt.Format("%Sunknown", &txt);
        }
        txt.Format("%S \t(%d,%d)", &txt, startindex, nextindex);
    }
    return txt;
}

CArithmeticExpression::op_data * CArithmeticExpression::AddParentheses(op_data * begin, op_data * end, op_data * new_op, int &allocindex)
{
    {
        op_data * p0 = begin;
        if  (p0 && p0->op == eOperator && (p0->index == 4 || p0->index == 5))
        {
            op_data *p = p0->pnextindex;
            op_data *nn = p ? p->pnextindex : nullptr;
            //p = p0->pnextindex;

            if  (p && (p->op != eOperator || p->index == 0 || p->index == 0x100 || p->index == 0x101))
            {
                op_data *n = &new_op[allocindex++];
                memset(n, 0, sizeof(*n));
                n->op = eOperator;
                n->index = -0x100;
                n->startindex = -1;
                n->pstartindex = p0;
                n->pnextindex = nn;
                n->pnextindex2 = nn;
                n->InsertBefore(p0);
                begin = n;
            }
        }
    }

    op_data * p = begin;
    while(p && p != end)
    {
        if  (p->op == eOperator && p->index == 0)
        {
            p = AddParentheses(p->pnextindex2, p->pnextindex, new_op, allocindex);
        }
        else
        {
            p = p->pnextindex2;
        }
    }

    //int FoundOpp = -1;
    for (int opp1 = 1; opp1 < (int)szOperators.MaxIndex() - 2; opp1++)
    {
        op_data * p = begin;
        op_data *n;
        op_data * parg1 = nullptr;
        while(p && p != end)
        {
            if  ((p->op != eOperator || p->index == 0 || p->index == -0x100 || p->index == -0x101))
            {
                parg1 = p;
            }
            for (int opp2 = opp1; PrecedenceLevel[opp2] == PrecedenceLevel[opp1]; opp2++)
            {
                const int opp = opp2;
                //&& ([opp] < FoundOpp || ([opp] == FoundOpp && ))
                if  (p->op == eOperator && p->index == opp && parg1 && parg1->pnextindex && parg1->pnextindex->pnextindex)
                {
                    //if (FoundOpp == -1 || [FoundOpp] == [opp] &&
                    //if   (p->op == eOperator && p->index == opp && ([opp] < FoundOpp || ([opp] == FoundOpp && )) && parg1 && parg1->pnextindex && parg1->pnextindex->pnextindex)
                    //{
                    dbg_printf("opp=%d, index=%d ", opp, p->index);
                    n = &new_op[allocindex++];
                    //memset(n, 0, sizeof(*n));
                    memcpy(n, parg1, sizeof(*n));
                    //n->op = eOperator;
                    parg1->op = eOperator;
                    parg1->index = (szOperators[p->index] == ":") ? -0x101 : -0x100;
                    //n->pstartindex = parg1->pstartindex;
                    parg1->pstartindex = n;
                    parg1->pnextindex = p->pnextindex ? p->pnextindex->pnextindex : nullptr;
                    //n->pnextindex2 = parg1->pnextindex2;
                    parg1->pnextindex2 = n;
                    //parg1->pnextindex2 = p->pnextindex2 ? p->pnextindex2->pnextindex2 : nullptr;
                    n->InsertAfter(parg1);
                    p = parg1;
                }
            }
            p = p->pnextindex;
        }
    }
    return end;
}

int CArithmeticExpression::CalcFunction(CMaaString expr, _qword &qResult, bool &bResult, int CacheCount)
{
    if  (CacheCount > 0)
    {
        CExprCache * cache = nullptr;
        if  (!m_hCache.Find(expr, &cache) && cache)
        {
            CMaaPtr_<op_data, 1> hop(cache->hop.MaxIndex());
            if  (hop.IsValid() && cache->hop.IsValid())
            {
                int hindex0 = cache->begin;
                const int hindex = cache->end;
                CMaaPtr_<_qword, 1> qResults(cache->m_N + 10);
                CMaaPtr_<bool, 1> bResults(cache->m_N + 10);
                CMaaPtr_<int, 1> ResultTypes(cache->m_N + 10);
                if  (cache->m_N + 10 >= hindex && hindex0 >= 0 && hindex > hindex0 && qResults.IsValid() && bResults.IsValid() && ResultTypes.IsValid() &&
                      hop.Size() == cache->hop.Size() && qResults.Size() == cache->qResults.Size() && ResultTypes.Size() == cache->ResultTypes.Size())
                {
                    //__utf8_printf("copy1\n");
                    memcpy((op_data*)hop, (op_data *)cache->hop, hop.Size());
                    //hindex0 = 0;
                    //hindex = N;
                    //__utf8_printf("copy2\n");
                    memcpy((_qword*)qResults, (_qword*)cache->qResults, cache->qResults.Size());
                    //__utf8_printf("copy3\n");
                    memcpy((int*)ResultTypes, (int*)cache->ResultTypes, cache->ResultTypes.Size());
                    //__utf8_printf("calc\n");
                    if  (m_pFnCaller)
                    {
                        cache->m_Time = m_pFnCaller->GetTime();
                    }
                    else
                    {
                        CFuncCaller TemporarilyCall;
                        cache->m_Time = TemporarilyCall.GetTime();
                    }
                    m_CacheList.Release(cache);
                    m_CacheList.AddAtFront(cache);

                    cache->m_UseCount++;
                    int res;
                    try
                    {
                        res = CalcFunction(expr, hindex0, hindex, hop, cache->hArgs, cache->hlArgs, (_qword *)qResults, (bool *)bResults, (int *)ResultTypes, 0);
                    }
                    catch(...)
                    {
                        cache->m_UseCount--;
                        throw;
                    }
                    cache->m_UseCount--;
                    if  (res >= 0)
                    {
                        qResult = qResults[0];
                        bResult = bResults[0];
                        return res;
                    }
                    return -1;
                }
                else
                {
                    /*
                         printf("no cache....\n");
                         printf("%d %d %d   %d %d %d   %D %D   %D %D   %D %D\n",
                              (int)cache->m_N, (int)hindex0, (int)hindex,
                              (int)qResults.IsValid(), (int)bResults.IsValid(), (int)ResultTypes.IsValid(),
                              (_qword)hop.Size(), (_qword)cache->hop.Size(), (_qword)qResults.Size(), (_qword)cache->qResults.Size(), (_qword)ResultTypes.Size(), (_qword)cache->ResultTypes.Size());
                         */
                    //getch();
                    /**/
                }
            }
        }
    }
    //CMaaString Parenthesis;
    //CMaaUnivHash<int, op_data> hop;
    //CMaaUnivHash<int, int> hpar;
    int begin = 0, end = (warning_int)expr.Length(), hindex = 0;
    CMaaUnivHash<int, op_data> hop;
    CMaaUnivHash<int, CMaaString> hArgs, hlArgs;
    CMaaUnivHash<int, _qword> hqwConstants;
    if  (Pass1CalcFunction(expr, begin, end, hindex, hop, hArgs, hlArgs, hqwConstants, 0))
    {
        //begin = 0, end = expr.Length();
        int hindex0 = 0;
        /*
          CMaaPtr<_qword> qResults(hindex + 10);
          CMaaPtr<bool> bResults(hindex + 10);
          CMaaPtr<int> ResultTypes(hindex + 10);
          */
        CMaaPtr_<op_data, 1> new_op(2 * hindex + 10);
        CMaaDList<op_data> List;
        //CMaaPtr<op_data> _hop(hindex + 10);
        for (int i = 0; i < hindex; i++)
        {
            hop.Find(i, &new_op[i]);
            dbg_printf("%d ", new_op[i].op);
            //op_data *pindex, *pnextindex;
            //               new_op[i].pindex = &new_op[i + 1];//(new_op[i].index >= 0 && new_op[i].index <= hindex) ? &new_op[new_op[i].index] : nullptr;
            new_op[i].startindex = -1;
            new_op[i].pstartindex = &new_op[i];
            if  (new_op[i].op == eOperator)
            {
                CMaaString op = szOperators[new_op[i].index];
                if  (new_op[i].index == 0 || op == "?" || op == ":")
                {
                    new_op[i].pstartindex++;
                }
            }
            new_op[i].pnextindex = (new_op[i].nextindex >= 0 && new_op[i].nextindex <= hindex) ? &new_op[new_op[i].nextindex] : &new_op[i + 1];
            new_op[i].pnextindex2 = &new_op[i + 1];//new_op[i].pindex;// ? new_op[i].pindex : &new_op[hindex];
            new_op[i].oldindex = i;
            //new_op[i]..pindex
            _qword x = -1;
            hqwConstants.Find(i, &x);
            const op_data *ptr = &new_op[i];
            //dbg_printf("%p %d\n", ptr->pstartindex, ptr->pstartindex->newindex);
            dbg_printf(ptr->op == eNumber ? "%2d. %d %d (%d,%d) [%D],\n" : "%2d. %d %d (%d,%d),\n", i, ptr->op, ptr->index, ptr->startindex, ptr->nextindex, x);

            List.AddAtBack(&new_op[i]);
        }
        dbg_printf("\n");

        int allocindex = hindex;

        op_data * p = &new_op[allocindex++];
        memset(p, 0, sizeof(*p));
        p->op = eEnd;
        List.AddAtBack(p);

        /*op_data * begin =*/ AddParentheses(List.LookAtFront(), nullptr/*List.LookAtBack()*/, new_op, allocindex);

        int N = 0;
        CMaaUnivHash<int, CMaaString> _hArgs, _hlArgs;
        CMaaUnivHash<int, _qword> _hqwConstants;

        {
            op_data * p = List.LookAtFront();
            while(p)
            {
                p->newindex = N++;
                p = List.Next(p);
            }
            p = List.LookAtFront();
            while(p)
            {
                if  (p->op == eFunc1)
                {
                    for (int i = 0; i < p->nargs; i++)
                    {
                        CMaaString a, la;
                        const int idx  = (p->oldindex << 16) + i;
                        const int _idx = (p->newindex << 16) + i;
                        hArgs.Find(idx, &a);
                        hlArgs.Find(idx, &la);
                        _hArgs.Add(_idx, a);
                        _hlArgs.Add(_idx, la);
                    }
                }
                if  (p->op == eNumber)
                {
                    _qword x;
                    if  (!hqwConstants.Find(p->oldindex, &x))
                    {
                        _hqwConstants.Add(p->newindex, x);
                    }
                }
                //p->index = p->newindex;
                p->nextindex = p->pnextindex ? p->pnextindex->newindex : -1;
                p = List.Next(p);
            }
        }

        CMaaPtr_<op_data, 1> _hop(N + 10);

        hindex0 = 0;
        hindex = N;
        CMaaPtr_<_qword, 1> qResults(hindex + 10);
        CMaaPtr_<bool, 1> bResults(hindex + 10);
        CMaaPtr_<int, 1> ResultTypes(hindex + 10);

        int nn = 0;
        {
            op_data * p = List.LookAtFront();
            --N;
            while(p)
            {
                _hop[nn] = *p;

                {
                    CMaaString op = _hop[nn].index >= 0 ? szOperators[_hop[nn].index] : CMaaString("");
                    if  (_hop[nn].index == -0x100 || _hop[nn].index == -0x101 || _hop[nn].index == 0 || op == "?" || op == ":")
                    {
                        _hop[nn].index = _hop[nn].index == -0x101 ? (int)m_nOpenBracketsArithIfIndex : _hop[nn].index == -0x100 ? 0 : _hop[nn].index;
                        //dbg_printf("%p %d\n", _hop[nn].pstartindex, _hop[nn].pstartindex->newindex);
                        _hop[nn].startindex = _hop[nn].pstartindex ? _hop[nn].pstartindex->newindex : nn; /////// at the end ???
                    }
                }
                //dbg_printf(_hop[nn].op == eNumber ? "%2d. %d %d (%d,%d) [%D],\n" : "%2d. %d %d (%d,%d),\n", nn, _hop[nn].op, _hop[nn].index, _hop[nn].startindex, _hop[nn].nextindex, x);
                /*
                    if   (_hop[nn].index < 0 || _hop[nn].index > N)
                    {
                         //error
                    }
                    */
                if  (_hop[nn].nextindex < 0 || _hop[nn].nextindex > N)
                {
                    //error
                }
                if  (_hop[nn].op == eNumber)
                {
                    _qword x;
                    if  (!_hqwConstants.Find(_hop[nn].newindex, &x))
                    {
                        qResults[_hop[nn].newindex] = x;
                        //bResults
                        ResultTypes[_hop[nn].newindex] = eInt;
                    }
                    else
                    {
                        ResultTypes[_hop[nn].newindex] = -1;
                    }
                }
                if  (0)
                {
                    CMaaString txt = _hop[nn].GetText(nn, this, _hArgs, _hlArgs, qResults, bResults, ResultTypes);
                    dbg_printf("%S\n", &txt);
                }
                nn++;
                p = List.Next(p);
            }
        }
        --nn;

        /*
          int res = CalcFunction(expr, hindex0, hindex, _hop, _hArgs, _hlArgs, _hqwConstants, (_qword *)qResults, (bool *)bResults, (int *)ResultTypes, 0);
          if   (res >= 0)
          {
               qResult = qResults[0];
               bResult = bResults[0];
               return res;
          }
          */

        CExprCache * cache = CacheCount > 0 /*|| CacheCount == -1*/ ? TL_NEW CExprCache(N) : nullptr;

        int res;
        if  (!cache || !cache->IsValid())
        {
            delete cache; cache = nullptr;
            //THROW_CALC_FUNCTION_ERROR("memory allocation error", 0);
            //return -10;
            res = CalcFunction(expr, hindex0, hindex, _hop, _hArgs, _hlArgs, (_qword *)qResults, (bool *)bResults, (int *)ResultTypes, 0);
        }
        else
        {
            memcpy((op_data *)cache->hop, (op_data*)_hop, cache->hop.Size());
            memcpy((_qword*)cache->qResults, (_qword*)qResults, cache->qResults.Size());
            memcpy((int*)cache->ResultTypes, (int*)ResultTypes, cache->ResultTypes.Size());
            cache->expr = expr;
            cache->begin = hindex0;
            cache->end = hindex;
            cache->hArgs.Swap(_hArgs);
            cache->hlArgs.Swap(_hlArgs);
            //cache->hqwConstants.Swap(_hqwConstants);
            CExprCache * p = m_CacheList.LookAtBack();
            while(m_nExprCacheCount >= CacheCount && p && CacheCount >= 0)
            {
                CExprCache * nxt = m_CacheList.Prev(p);
                if  (p->m_UseCount <= 1)
                {
                    m_hCache.Remove(p->expr);
                    delete p;
                    --m_nExprCacheCount;
                }
                p = nxt;
            }
            if  (m_pFnCaller)
            {
                cache->m_Time = m_pFnCaller->GetTime();
            }
            else
            {
                CFuncCaller TemporarilyCall;
                cache->m_Time = TemporarilyCall.GetTime();
            }
            if  (!m_hCache.Add(expr, cache))
            {
                cache->m_UseCount++;
                m_CacheList.AddAtFront(cache);
                m_nExprCacheCount++;
                res = CalcFunction(expr, hindex0, hindex, _hop, cache->hArgs, cache->hlArgs, (_qword *)qResults, (bool *)bResults, (int *)ResultTypes, 0);
            }
            else
            {
                try
                {
                    res = CalcFunction(expr, hindex0, hindex, _hop, cache->hArgs, cache->hlArgs, (_qword *)qResults, (bool *)bResults, (int *)ResultTypes, 0);
                }
                catch(...)
                {
                    delete cache;
                    throw;
                }
            }
        }
        if  (res >= 0)
        {
            qResult = qResults[0];
            bResult = bResults[0];
            return res;
        }
        //return -1;
    }
    return -1;
}
void CArithmeticExpression::Prune(int TimeOutThreshold, int method)
{
    int N = -1;
    if  (TimeOutThreshold < 0)
    {
        if  (m_pFnCaller)
        {
            TimeOutThreshold = m_pFnCaller->GetTimeExpireValue();
        }
        else
        {
            CFuncCaller TemporarilyCall;
            TimeOutThreshold = TemporarilyCall.GetTimeExpireValue();
        }
    }
    if  (TimeOutThreshold >= 0)
    {
        _dword Time;
        if  (m_pFnCaller)
        {
            Time = m_pFnCaller->GetTime();
        }
        else
        {
            CFuncCaller TemporarilyCall;
            Time = TemporarilyCall.GetTimeExpireValue();
        }
        //
        N = 0;
        // method 1 (fast and generic)
        if  (method != 2)
        {
            while(1)
            {
                CExprCache * cache = m_CacheList.LookAtBack();
                if  (!cache)
                {
                    break;
                }
                if  ((int)(Time - cache->m_Time) >= TimeOutThreshold && cache->m_UseCount <= 1)
                {
                    m_CacheList.Release(cache);
                    m_hCache.Remove(cache->expr);
                    delete cache;
                    N++;
                    m_nExprCacheCount--;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            // method 2 (full scan)
            CExprCache * cache = m_CacheList.LookAtBack();
            while(cache)
            {
                CExprCache * nxt = m_CacheList.Prev(cache);
                if  ((int)(Time - cache->m_Time) >= TimeOutThreshold && cache->m_UseCount <= 1)
                {
                    m_CacheList.Release(cache);
                    m_hCache.Remove(cache->expr);
                    delete cache;
                    N++;
                    m_nExprCacheCount--;
                }
                cache = nxt;
            }
        }
    }
}
int CArithmeticExpression::CalcFunction(CMaaString &expr, int &begin, int end, op_data *hop, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, _qword *qResults, bool *bResults, int * ResultTypes, int ProcessingTaskType)
{
    unsigned Ret = (unsigned)-1;
    //__try
    {
        Ret = CalcFunction1(expr, begin, end, hop, hArgs, hlArgs, qResults, bResults, ResultTypes, ProcessingTaskType);
    }
    //__except(My_RRSEFilter(GetExceptionCode(), GetExceptionInformation()))
    {
    }
    return Ret;
}

int gRecCalls = 0;
int CArithmeticExpression::CalcFunction1(CMaaString &expr, int &begin, int end, op_data *hop, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, _qword *qResults, bool *bResults, int * ResultTypes, int ProcessingTaskType)
{
    dbg_printf("CalcFunction1(%d - %d, ProcessingTaskType = 0x%04x\n", begin, end, ProcessingTaskType);

    ++gRecCalls;
    //printf("CalcFunction1(%d, %d, 0x%x)\n", begin, end, ProcessingTaskType);
    dbg_printf("CalcFunction1(%d, %d, 0x%x)\n", begin, end, ProcessingTaskType);
    //int begin0 = begin;
    try
    {
        const int pos = begin;
        //while(expr[pos] == ' ') pos++;
        if  (pos >= end)
        {
            //break;
        }
        /*
Operator       Name or Meaning               Associativity

::             Scope resolution              None
.              Member selection (object)     Left to right
->             Member selection (pointer)    Left to right
[]             Array subscript               Left to right
()             Function call member initialization     Left to right
++             Postfix increment             Left to right
--             Postfix decrement             Left to right
typeid()       type name                     Left to right
const_cast     Type cast (conversion)        Left to right
dynamic_cast   Type cast (conversion)        Left to right
reinterpret_cast Type cast (conversion)      Left to right
static_cast    Type cast (conversion)        Left to right
sizeof         Size of object or type        Right to left
++             Prefix increment              Right to left
--             Prefix decrement              Right to left
~              One's complement              Right to left
!              Logical not                   Right to left
-              Unary minus                   Right to left
+              Unary plus                    Right to left
&              Address-of                    Right to left
*              Indirection                   Right to left
new            Create object                 Right to left
delete         Destroy object                Right to left
()             Cast                          Right to left
.*             Pointer-to-member (objects)   Left to right
->*            Pointer-to-member (pointers)  Left to right
*              Multiplication                Left to right
/              Division                      Left to right
%              Modulus                       Left to right
+              Addition                      Left to right
-              Subtraction                   Left to right
<<             Left shift                    Left to right
>>             Right shift                   Left to right
<              Less than                     Left to right
>              Greater than                  Left to right
<=             Less than or equal to         Left to right
>=             Greater than or equal to      Left to right
==             Equality                      Left to right
!=             Inequality                    Left to right
&              Bitwise AND                   Left to right
^              Bitwise exclusive OR          Left to right
|              Bitwise inclusive OR          Left to right
&&             Logical AND                   Left to right
||             Logical OR                    Left to right
e1?e2:e3       Conditional                   Right to left
=              Assignment                    Right to left
*=             Multiplication assignment     Right to left
/=             Division assignment           Right to left
%=             Modulus assignment            Right to left
+=             Addition assignment           Right to left
-=             Subtraction assignment        Right to left
<<=            Left-shift assignment         Right to left
>>=            Right-shift assignment        Right to left
&=             Bitwise AND assignment        Right to left
|=             Bitwise inclusive OR assignment Right to left
^=             Bitwise exclusive OR assignment Right to left
throw expr     throw expression              Right to left
,              Comma                         Left to right
*/

        bool bExit = false;
        while(1)
        {
            bExit = bExit || (pos >= end);

            // Simplification
            {
                //CMaaString szOperators[] =
                //{
                //     "(",
                //     "*", "/", "%", "+", "-", "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "&", "^", "|", "&&", "||",
                //     ""
                //};
                /*
                  eReserved = -2,
                  eError = -1,
                  eOperator,
                  eNumber,
                  eFunc1,
                  eFunc2, // with calculating of arguments expressions
                  eEnd
                */
                int nChanges1 = 1;
                while(nChanges1 > 0)
                {
                    int i;

                    nChanges1 = -1;
                    const int i0 = begin;
                    int nChanges = 1;
                    while(nChanges > 0)
                    {
                        nChanges1 += nChanges;
                        nChanges = 0;
                        int PrevOpPrecedence = -1;
                        int FirstOp = -1;
                        int Res_1 = -1;
                        _qword Res_2 = 0;
                        int ilast = 0;
                        i = i0;
                        const int iiiii = 0;
                        if  (0)
                        {
                            CMaaString txt = hop[i].GetText(i, this, hArgs, hlArgs, qResults, bResults, ResultTypes);
                            dbg_printf("___1___ %S\n", &txt);
                        }

                        while(i < end)
                        {
                            if  (hop[i].op == eEnd)
                            {
                                break;
                            }
                            if  (hop[i].nextindex < 0 || hop[i].nextindex > end)
                            {
                                THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("CArithmeticExpression::CalcFunction1(): internal error, i=%d, hop[i].op=%d, hop[i].index=%d, hop[i].nextindex=%d, end=%d\n", i, hop[i].op, hop[i].index, hop[i].nextindex, end), -1);
                                //__utf8_printf("i=%d, hop[i].op=%d, hop[i].index=%d, hop[i].nextindex=%d, end=%d\n", i, hop[i].op, hop[i].index, hop[i].nextindex, end);
                                //iiiii++;
                            }
                            if  (i == i0 && hop[i].op == eOperator && (hop[i].index == 4 || hop[i].index == 5))
                            {
                                const int ii = hop[i].nextindex;
                                if  (ii < end && hop[ii].op == eNumber)
                                {
                                    if  (ResultTypes[ii] == eInt)
                                    {
                                        qResults[i] = (hop[i].index == 5) ? -qResults[ii] : qResults[ii];
                                        ResultTypes[i] = ResultTypes[ii];//eInt;
                                        hop[i].op = eNumber;
                                        hop[i].nextindex = hop[ii].nextindex;
                                        nChanges++;
                                        continue;
                                    }
                                    else
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("unary operator '%S' for not integer data at position %d", &szOperators[hop[i].index], hop[ii].pos + 1), hop[ii].pos);
                                    }
                                }
                                //continue;
                            }
                            else if (hop[i].op == eOperator && hop[i].index == m_nOpenBracketsArithIfIndex)
                            {
                                int ii = hop[i].startindex;//i + 1;
                                if  (ii <= i)
                                {
                                    //error
                                }
                                const int ii0 = ii;
                                int res;
                                static int nnnn = 0;
                                const int _nnnn = ++nnnn;
                                dbg_printf(">[%d] CalcFunction1(\"%S\", %d(%d))\n", _nnnn, &szOperators[hop[i].index], i, ii);
                                {
                                    if  (hop[ii].op != eNumber)
                                    {
                                        res = CalcFunction1(expr, ii, hop[i].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes,
                                             /*ProcessingTaskType*/ (ProcessingTaskType & 0x30) ? (ProcessingTaskType & 0x30) | 1 : 0x101);
                                    }
                                    else
                                    {
                                        res = ResultTypes[ii];
                                        ii = hop[ii].nextindex;//hop[i].nextindex;
                                    }
                                }
#ifndef ___ToolsLibQT___
#ifdef _print_debug_to_console
                                CMaaString tmp11 = CMaaString::sFormat2("%D%s", res == eInt ? "%1" : "%2", qResults[ii0], bResults[ii0] ? "true" : "false");
                                dbg_printf("<[%d] CalcFunction1 returned res=%d(%S), next = %d\n", _nnnn, res, &tmp11, ii );
#endif
#endif
                                if  (res <= eError)
                                {
                                    return res;
                                }
                                //int i_index = hop[i].index;
                                ResultTypes[i] = res;
                                hop[i].op = eNumber;
                                hop[i].nextindex = ii;
                                if  (ii0 != i)
                                {
                                    qResults[i] = qResults[ii0];
                                    bResults[i] = bResults[ii0];
                                }

                                int arg2_index = -1;
                                {
                                    if  (hop[ii].op == eOperator && szOperators[hop[ii].index] == ":")
                                    {
                                        ii = hop[ii].nextindex; // or  ii = hop[ii].startindex;
                                        arg2_index = ii;
                                        if  (hop[ii].op != eNumber)
                                        {
                                            res = CalcFunction1(expr, ii, hop[ii].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes,
                                                 /*ProcessingTaskType*/ (ProcessingTaskType & 0x20) ? 1 : 0x101);//(ProcessingTaskType & 0x110) == 0x10 ? 1 : 0x101);
                                        }
                                        else
                                        {
                                            res = ResultTypes[ii];
                                            hop[arg2_index].op = eNumber;
                                            qResults[arg2_index] = qResults[ii];
                                            bResults[arg2_index] = bResults[ii];
                                            ResultTypes[arg2_index] = res;
                                            ii = hop[ii].nextindex;//hop[i].nextindex;
                                            hop[arg2_index].index = eNumber;
                                        }
                                        hop[arg2_index].nextindex = ii;
                                        if  (res <= eError)
                                        {
                                            return res;
                                        }
                                        /*
                                                  if   (ProcessingTaskType & 0x20)
                                                  {
                                                       qResults[i] = qResults[arg2_index];
                                                       bResults[i] = bResults[arg2_index];
                                                       ResultTypes[i] = res;
                                                       hop[i].op = eNumber;
                                                  }
                                                  hop[i].nextindex = ii;
                                                  */
                                    }
                                }

                                nChanges++;
                                //continue;
                                if  (Res_1 == -1)
                                {
                                    Res_1 = (res == eBool || res == eBool2) ? bResults[i] ? 1 : 0 : -2;
                                    Res_2 = (res == eInt) ? qResults[i] : 0;
                                }
                                continue;
                            }
                            else if (hop[i].op == eOperator && szOperators[hop[i].index] == "?")
                            {
                                int ii = hop[i].startindex;//i + 1;
                                if  (ii <= i)
                                {
                                    //error
                                }
                                //treat: hop[ii].index == m_nOpenBracketsArithIfIndex
                                if  (hop[ii].op == eOperator && (hop[ii].index == m_nOpenBracketsArithIfIndex || hop[ii].index == 0))
                                {
                                    int ii0 = ii;
                                    int res;
                                    static int nnnn = 0;
                                    const int _nnnn = ++nnnn;
                                    dbg_printf(">>[%d] CalcFunction1(\"%S\", %d(%d))\n", _nnnn, &szOperators[hop[i].index], i, ii);
                                    if  (
                                          (ProcessingTaskType & 0x100) ||
                                          (FirstOp > 0 && m_Style == eCStyle &&
                                            (
                                                (szOperators[FirstOp] == "||" && Res_1 == 1) ||
                                                (szOperators[FirstOp] == "&&" && Res_1 == 0)
                                            )
                                          )
                                         )
                                    {
                                        res = CalcFunction1(expr, ii, hop[ii].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes, 0x101);
                                    }
                                    else
                                    {
                                        // Res_1 == -1 ?
                                        const int Mode = (Res_1 == 1 || (Res_1 == -2 && Res_2 != 0)) ? 0x10 : 0x20;
                                        res = CalcFunction1(expr, ii, hop[Mode ? ii : i].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes, 1 | Mode);
                                    }
#ifndef ___ToolsLibQT___
#ifdef _print_debug_to_console
                                    CMaaString tmp11 = CMaaString::sFormat2("%D%s", res == eInt ? "%1" : "%2", qResults[ii0], bResults[ii0] ? "true" : "false");
                                    dbg_printf("<<[%d] CalcFunction1 returned res=%d(%S), next = %d\n", _nnnn, res, &tmp11, ii);
#endif
#endif
                                    if  (res <= eError)
                                    {
                                        return res;
                                    }
                                    //int i_index = hop[i].index;
                                    ResultTypes[ii0] = res;
                                    hop[ii0].op = eNumber;
                                    hop[ii0].nextindex = ii;
                                    nChanges++;
                                    //continue;
                                    if  (Res_1 == -1)
                                    {
                                        Res_1 = (res == eBool || res == eBool2) ? bResults[i] ? 1 : 0 : -2;
                                        Res_2 = (res == eInt) ? qResults[i] : 0;
                                    }
                                    continue;
                                }
                            }
                            else if (hop[i].op == eOperator && (hop[i].index == 0 || hop[i].index == m_nOpenBracketsArithIfIndex))// || szOperators[hop[i].index] == "?"))// || szOperators[hop[i].index] == ":"))
                            {
                                int ii = hop[i].startindex;//i + 1;
                                if  (ii <= i)
                                {
                                    //error
                                }
                                const int ii0 = ii;
                                int res;
                                static int nnnn = 0;
                                const int _nnnn = ++nnnn;
                                dbg_printf("[%d] CalcFunction1(\"%S\", %d(%d))\n", _nnnn, &szOperators[hop[i].index], i, ii);
                                if  (hop[i].index == m_nOpenBracketsArithIfIndex)
                                {
                                    if  (hop[ii].op != eNumber)
                                    {
                                        res = CalcFunction1(expr, ii, hop[i].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes,
                                             /*ProcessingTaskType*/ (ProcessingTaskType & 0x10) ? 1 : 0x101);//(ProcessingTaskType & 0x110) == 0x10 ? 1 : 0x101);
                                    }
                                    else
                                    {
                                        res = ResultTypes[ii];
                                        ii = hop[ii].nextindex;//hop[i].nextindex;
                                    }
                                    if  (res <= eError)
                                    {
                                        return res;
                                    }
                                }
                                else if (
                                           (ProcessingTaskType & 0x100) ||
                                           (m_Style == eCStyle &&
                                             (
                                                  (FirstOp > 0 &&
                                                       (
                                                           (szOperators[FirstOp] == "||" && Res_1 == 1) ||
                                                           (szOperators[FirstOp] == "&&" && Res_1 == 0)
                                                       )
                                                  )
#if 000001
                                                  || (ProcessingTaskType & 0x3030) == 0x0020 || (ProcessingTaskType & 0x3030) == 0x1010 //|| (ProcessingTaskType & 0x3000) == 0x3000
#endif
                                             )
                                           )
                                          )
                                /*
                                             if
                                             (
                                              (ProcessingTaskType & 0x100) ||
                                              (FirstOp > 0 && m_Style == eCStyle &&
                                                (
                                                    (szOperators[FirstOp] == "||" && Res_1 == 1) ||
                                                    (szOperators[FirstOp] == "&&" && Res_1 == 0) ||
                                                    (szOperators[FirstOp] == "?" && (Res_1 == 0 || (Res_1 == -2 && Res_2 == 0))) ||
                                                    (ProcessingTaskType & 0x430) == 0x20
                                                )
                                              )
                                             )
                                             */
                                {
                                    res = CalcFunction1(expr, ii, hop[i].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes, 0x101);
                                    //res = CalcFunc1(hop[i].index, hArgs, hlArgs, i, hop[i].nargs, qResults[i], bResults[i], ResultTypes[i], 0x100);
                                    if  ((ProcessingTaskType & 0x30))
                                    {
                                        if  (!(ProcessingTaskType & 0x3000))
                                        {
                                            if  (hop[i].nextindex < end && hop[i].nextindex > i && hop[hop[i].nextindex].op == eOperator && (szOperators[hop[hop[i].nextindex].index] == ":"))// || szOperators[hop[hop[i].nextindex].index] == "?"
                                            {
                                                ProcessingTaskType |= 0x1000;
                                            }
                                        }
                                        else
                                        {
                                            ProcessingTaskType |= 0x2000;
                                        }
                                    }
                                    if  (res <= eError)
                                    {
                                        return res;
                                    }
                                }
                                else
                                {
                                    int Mode = (szOperators[hop[i].index] == "?") ? (Res_1 == 1 || (Res_1 == -2 && Res_2 != 0)) ? 0x10 : 0x20 : 0;
                                    if  ((ProcessingTaskType & 0x3000))
                                    {
                                        Mode |= 1;
                                    }
                                    else
                                    {
                                        Mode = Mode | (ProcessingTaskType & 0x30);
                                    }
                                    res = CalcFunction1(expr, ii, hop[Mode || szOperators[hop[i].index] == ":" ? ii : i].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes, 1 | Mode);
                                    if  (res <= eError)
                                    {
                                        return res;
                                    }
                                    if  ((ProcessingTaskType & 0x30))
                                    {
                                        if  (!(ProcessingTaskType & 0x3000))
                                        {
                                            if  (hop[i].nextindex < end && hop[i].nextindex > i && hop[hop[i].nextindex].op == eOperator && (szOperators[hop[hop[i].nextindex].index] == ":"))// || szOperators[hop[hop[i].nextindex].index] == "?"
                                            {
                                                ProcessingTaskType |= 0x1000;
                                            }
                                        }
                                        else
                                        {
                                            ProcessingTaskType |= 0x2000;
                                        }
                                    }
#ifndef ___ToolsLibQT___
#ifdef _print_debug_to_console
                                    CMaaString tmp11 = CMaaString::sFormat2("%D%s", res == eInt ? "%1" : "%2", qResults[ii0], bResults[ii0] ? "true" : "false");
                                    dbg_printf("[%d] CalcFunction1 returned res=%d(%S), next = %d\n", _nnnn, res, &tmp11, ii);
#endif
#endif
                                }
                                const int i_index = hop[i].index;
                                ResultTypes[i] = res;
                                hop[i].op = eNumber;
                                hop[i].nextindex = ii;
                                if  (ii0 != i)
                                {
                                    qResults[i] = qResults[ii0];
                                    bResults[i] = bResults[ii0];
                                }

                                if  (i_index == m_nOpenBracketsArithIfIndex)
                                {
                                    if  (hop[ii].op == eOperator && szOperators[hop[ii].index] == ":")
                                    {
                                        int respos = ii = hop[ii].nextindex;
                                        if  (hop[ii].op != eNumber)
                                        {
                                            res = CalcFunction1(expr, ii, hop[ii].nextindex, hop, hArgs, hlArgs, qResults, bResults, ResultTypes,
                                                 /*ProcessingTaskType*/ (ProcessingTaskType & 0x20) ? 1 : 0x101);//(ProcessingTaskType & 0x110) == 0x10 ? 1 : 0x101);
                                        }
                                        else
                                        {
                                            res = ResultTypes[ii];
                                            ii = hop[ii].nextindex;//hop[i].nextindex;
                                        }
                                        if  (res <= eError)
                                        {
                                            return res;
                                        }
                                        //ResultTypes[respos] = res;
                                        //qResults[respos] = qResults[respos];
                                        //bResults[resposi] = bResults[respos];
                                        hop[respos].op = eNumber;
                                        hop[respos].nextindex = ii;
                                    }
                                    else
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("missed operator ':' at position %d", hop[ii].pos + 1), hop[ii].pos);
                                    }
                                }

                                nChanges++;
                                //continue;
                                if  (Res_1 == -1)
                                {
                                    Res_1 = (res == eBool || res == eBool2) ? bResults[i] ? 1 : 0 : -2;
                                    Res_2 = (res == eInt) ? qResults[i] : 0;
                                }
                                //qResult[i] =
                                //bResult[i] =
                                continue;
                            }
                            else if (hop[i].op == eFunc1)
                            {
                                int res;
                                if  (
                                      (ProcessingTaskType & 0x100) ||
                                      (m_Style == eCStyle &&
                                        (
                                             (FirstOp > 0 &&
                                                  (
                                                      (szOperators[FirstOp] == "||" && Res_1 == 1) ||
                                                      (szOperators[FirstOp] == "&&" && Res_1 == 0)
                                                  )
                                             )
#if 000001
                                             || (ProcessingTaskType & 0x3030) == 0x0020 || (ProcessingTaskType & 0x3030) == 0x1010 //|| (ProcessingTaskType & 0x3000) == 0x3000
#endif
                                        )
                                      )
                                     )
                                {
                                    //res = CalcFunction(expr, ii, hop[i].nextindex, hop, hArgs, hlArgs, hqwConstants, qResults, bResults, ResultTypes, 0x101);
                                    res = CalcFunc1(hop[i].index, hArgs, hlArgs, i, hop[i].nargs, qResults[i], bResults[i], ResultTypes[i], 0x100);
                                }
                                else
                                {
                                    //CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs
                                    res = CalcFunc1(hop[i].index, hArgs, hlArgs, i, hop[i].nargs, qResults[i], bResults[i], ResultTypes[i], 0);
                                }
                                if  ((ProcessingTaskType & 0x30))
                                {
                                    if  (!(ProcessingTaskType & 0x3000))
                                    {
                                        if  (hop[i].nextindex < end && hop[i].nextindex > i && hop[hop[i].nextindex].op == eOperator && (szOperators[hop[hop[i].nextindex].index] == ":"))// || szOperators[hop[hop[i].nextindex].index] == "?"
                                        {
                                            ProcessingTaskType |= 0x1000;
                                        }
                                    }
                                    else
                                    {
                                        ProcessingTaskType |= 0x2000;
                                    }
                                }
                                //res = ResultTypes[i] = eInt;
                                //qResults[i] = 0;
                                //bResults[i] =;
                                ResultTypes[i] = res;
                                hop[i].op = eNumber;
                                nChanges++;
                                //continue;
                                if  (Res_1 == -1)
                                {
                                    Res_1 = (res == eBool || res == eBool2) ? bResults[i] ? 1 : 0 : -2;
                                    Res_2 = (res == eInt) ? qResults[i] : 0;
                                }
                                //qResult[i] =
                                //bResult[i] =
                                continue;
                            }
                            else if (hop[i].op == eFunc2)
                            {
                                int res;
                                if  (
                                      (ProcessingTaskType & 0x100) ||
                                      (m_Style == eCStyle &&
                                        (
                                             (FirstOp > 0 &&
                                                  (
                                                      (szOperators[FirstOp] == "||" && Res_1 == 1) ||
                                                      (szOperators[FirstOp] == "&&" && Res_1 == 0)
                                                  )
                                             )
#if 000001
                                             || (ProcessingTaskType & 0x3030) == 0x0020 || (ProcessingTaskType & 0x3030) == 0x1010 //|| (ProcessingTaskType & 0x3000) == 0x3000
#endif
                                        )
                                      )
                                     )
                                {
                                    res = CalcFunc2(hop[i].index, qResults[i], bResults[i], ResultTypes[i], 0x100);
                                }
                                else
                                {
                                    res = CalcFunc2(hop[i].index, qResults[i], bResults[i], ResultTypes[i], 0);
                                }
                                if  ((ProcessingTaskType & 0x30))
                                {
                                    if  (!(ProcessingTaskType & 0x3000))
                                    {
                                        if  (hop[i].nextindex < end && hop[i].nextindex > i && hop[hop[i].nextindex].op == eOperator && (szOperators[hop[hop[i].nextindex].index] == ":"))// || szOperators[hop[hop[i].nextindex].index] == "?"
                                        {
                                            ProcessingTaskType |= 0x1000;
                                        }
                                    }
                                    else
                                    {
                                        ProcessingTaskType |= 0x2000;
                                    }
                                }
                                ResultTypes[i] = res;
                                hop[i].op = eNumber;
                                nChanges++;

                                if  (Res_1 == -1)
                                {
                                    Res_1 = (res == eBool || res == eBool2) ? bResults[i] ? 1 : 0 : -2;
                                    Res_2 = (res == eInt) ? qResults[i] : 0;
                                }
                                continue;
                            }
                            else if(hop[i].op == eNumber && i == i0 + 1 && (hop[i - 1].index == 4 || hop[i - 1].index == 5))
                            {
                                if  (ResultTypes[i] == eInt)
                                {
                                    ResultTypes[i - 1] = ResultTypes[i];
                                    qResults[i - 1] = hop[i - 1].index == 5 ? -qResults[i] : qResults[i];
                                    hop[i - 1].nextindex = hop[i].nextindex;
                                    hop[i - 1].op = eNumber;
                                    nChanges++;
                                }
                                else
                                {
                                    THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%S' for not integer data at position %d", &szOperators[hop[i - 1].index], hop[i - 1].pos + 1), hop[i - 1].pos);
                                }
                            }
                            else if (hop[i].op == eOperator)
                            {
                                if  (PrevOpPrecedence < 0 || hop[i].index <= PrevOpPrecedence)
                                {
                                    PrevOpPrecedence = hop[i].index;
                                    FirstOp = FirstOp < 0 ? PrevOpPrecedence : FirstOp;
                                }
                                else
                                {
                                    ilast = i;
                                    break;
                                }
                            }
                            if  (hop[i].op == eNumber)
                            {
                                if  (Res_1 == -1)
                                {
                                    Res_1 = (ResultTypes[i] == eBool || ResultTypes[i] == eBool2) ? bResults[i] ? 1 : 0 : -2;
                                    Res_2 = (ResultTypes[i] == eInt) ? qResults[i] : 0;
                                }
                            }
                            if  (hop[i].nextindex < 0 || hop[i].nextindex > end)
                            {
                                //error
                                break;
                            }
                            i = hop[i].nextindex;
                        }
                        if  (iiiii)
                        {
                            //__utf8_printf("exit from while\n");
                        }

                        int SearchingOpPrecedence = PrevOpPrecedence;
                        //int MaxOpPrecedence = PrevOpPrecedence;
                        while(1)
                        {
                            ilast = ilast > 0 ? ilast : end;
                            PrevOpPrecedence = -1;
                            int op_index = -1;
                            i = i0;
                            int i__1 = i0;
                            while(i < ilast && i >= 0)
                            {
                                if  (0)
                                {
                                    CMaaString txt = hop[i].GetText(i, this, hArgs, hlArgs, qResults, bResults, ResultTypes);
                                    dbg_printf("___2___ %S\n", &txt);
                                }
                                if  (hop[i].op == eOperator)
                                {
                                    if  (hop[i].index == SearchingOpPrecedence)
                                    {
                                        PrevOpPrecedence = hop[i].index;
                                        op_index = i;
                                        break;
                                    }
                                    if  (PrevOpPrecedence < 0 || (hop[i].index < PrevOpPrecedence && hop[i].index > 0))
                                    {
                                        PrevOpPrecedence = hop[i].index;
                                    }
                                }
                                else if(hop[i].op == eNumber)
                                {
                                    i__1 = i;
                                }
                                i = hop[i].nextindex;
                            }
                            if  (op_index < 0 && PrevOpPrecedence > 0)
                            {
                                SearchingOpPrecedence = PrevOpPrecedence;
                                continue;
                            }
                            if  (op_index < 0)
                            //if   (PrevOpPrecedence < 0)
                            {
                                break;
                            }
                            //if   (Operators[i] == OperatorPrecedence)
                            {
                                // -(N)
                                const int OperatorPrecedence = SearchingOpPrecedence;
                                i = op_index;
                                CMaaString op = szOperators[OperatorPrecedence];
                                const int i_p1 = hop[i].nextindex;
#ifdef debug_ops
                                if  (ResultTypes[i__1] == eInt)
                                {
                                    debug_ops("%D", qResults[i__1]);
                                }
                                else
                                {
                                    debug_ops("%s", bResults[i__1] ? "true" : "false");
                                }
                                debug_ops(" %S ", &op);
                                if  (ResultTypes[i_p1] == eInt)
                                {
                                    debug_ops("%D\n", qResults[i_p1]);
                                }
                                else
                                {
                                    debug_ops("%s\n", bResults[i_p1] ? "true" : "false");
                                }
#endif
                                if  (OperatorPrecedence < 8 || (OperatorPrecedence >= 14 && OperatorPrecedence <= 16))
                                {
                                    if  (ResultTypes[i__1] != eInt || ResultTypes[i_p1] != eInt)
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%S' for not integer data at position %d", &op, hop[i].pos + 1), hop[i].pos);
                                    }
                                    if  (ProcessingTaskType & 0x100)
                                    {
                                        dbg_printf("skip (%x)\n", ProcessingTaskType);
                                    }
                                    else if (FirstOp > 0 && m_Style == eCStyle && ((szOperators[FirstOp] == "||" && Res_1 == 1) || (szOperators[FirstOp] == "&&" && Res_1 == 0)))
                                    {
                                    }
                                    else
                                    {
                                        if  (op == "*")
                                        {
                                            qResults[i__1] *= qResults[i_p1];
                                        }
                                        else if (op == "/")
                                        {
                                            if  (qResults[i_p1] == 0)
                                            {
                                                THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '/' : division by zero at position %d", hop[i].pos + 1), hop[i].pos);
                                            }
                                            qResults[i__1] /= qResults[i_p1];
                                        }
                                        else if (op == "%")
                                        {
                                            if  (qResults[i_p1] == 0)
                                            {
                                                THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%' : division by zero at position %d", hop[i].pos + 1), hop[i].pos);
                                            }
                                            qResults[i__1] %= qResults[i_p1];
                                        }
                                        else if (op == "+")
                                        {
                                            qResults[i__1] += qResults[i_p1];
                                        }
                                        else if (op == "-")
                                        {
                                            qResults[i__1] -= qResults[i_p1];
                                        }
                                        else if (op == "<<")
                                        {
                                            qResults[i__1] <<= (int)qResults[i_p1];
                                        }
                                        else if (op == ">>")
                                        {
                                            qResults[i__1] >>= (int)qResults[i_p1];
                                        }
                                        else if (op == "&")
                                        {
                                            qResults[i__1] &= qResults[i_p1];
                                        }
                                        else if (op == "^")
                                        {
                                            qResults[i__1] ^= qResults[i_p1];
                                        }
                                        else if (op == "|")
                                        {
                                            qResults[i__1] |= qResults[i_p1];
                                        }
                                    }
                                    //ResultTypes[i__1] = ResultTypes[i_p1];
                                    //hop[i__1].nextindex = hop[i_p1].nextindex;
                                }
                                else if (OperatorPrecedence >= 8 && OperatorPrecedence < 12)
                                {
                                    if  (ResultTypes[i__1] != eInt || ResultTypes[i_p1] != eInt)
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%S' for not integer data at position %d", &op, hop[i].pos + 1), hop[i].pos);
                                    }
                                    if  (ProcessingTaskType & 0x100)
                                    {
                                        bResults[i__1] = true;
                                        dbg_printf("skip (%x)\n", ProcessingTaskType);
                                    }
                                    else if (FirstOp > 0 && m_Style == eCStyle && ((szOperators[FirstOp] == "||" && Res_1 == 1) || (szOperators[FirstOp] == "&&" && Res_1 == 0)))
                                    {
                                    }
                                    else
                                    {
                                        if  (op == "<")
                                        {
                                            bResults[i__1] = qResults[i__1] < qResults[i_p1];
                                        }
                                        else if (op == ">")
                                        {
                                            bResults[i__1] = qResults[i__1] > qResults[i_p1];
                                        }
                                        else if (op == "<=")
                                        {
                                            bResults[i__1] = qResults[i__1] <= qResults[i_p1];
                                        }
                                        else if (op == ">=")
                                        {
                                            bResults[i__1] = qResults[i__1] >= qResults[i_p1];
                                        }
                                    }
                                    ResultTypes[i__1] = eBool;
                                    //hop[i__1].nextindex = hop[i_p1].nextindex;
                                    nChanges++;
                                }
                                else if (OperatorPrecedence >= 12 && OperatorPrecedence < 14)
                                {
                                    if  (ResultTypes[i__1] != ResultTypes[i_p1] && ((ResultTypes[i__1] | ResultTypes[i_p1]) & 1) == 1)
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%S' for a different types of operands at position %d", &op, hop[i].pos + 1), hop[i].pos);
                                    }
                                    if  (ResultTypes[i__1] != eInt && ResultTypes[i__1] != eBool && ResultTypes[i__1] != eBool2)
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%S' for unknown operand types at position %d", &op, hop[i].pos + 1), hop[i].pos);
                                    }
                                    if  (ResultTypes[i__1] == eInt)
                                    {
                                        if  (FirstOp > 0 && m_Style == eCStyle && ((szOperators[FirstOp] == "||" && Res_1 == 1) || (szOperators[FirstOp] == "&&" && Res_1 == 0)))
                                        {
                                            dbg_printf("skip\n");
                                        }
                                        else
                                        {
                                            if  (ProcessingTaskType & 0x100)
                                            {
                                                bResults[i__1] = true;
                                                dbg_printf("skip (%x)\n", ProcessingTaskType);
                                            }
                                            else
                                            {
                                                if  (op == "==")
                                                {
                                                    bResults[i__1] = qResults[i__1] == qResults[i_p1];
                                                }
                                                else if (op == "!=")
                                                {
                                                    bResults[i__1] = qResults[i__1] != qResults[i_p1];
                                                }
                                            }
                                        }
                                        ResultTypes[i__1] = eBool;
                                    }
                                    else //if (ResultTypes[i] == 0)
                                    {
                                        if  (FirstOp > 0 && m_Style == eCStyle && ((szOperators[FirstOp] == "||" && Res_1 == 1) || (szOperators[FirstOp] == "&&" && Res_1 == 0)))
                                        {
                                            dbg_printf("skip\n");
                                        }
                                        else
                                        {
                                            if  (ProcessingTaskType & 0x100)
                                            {
                                                bResults[i__1] = true;
                                                dbg_printf("skip (%x)\n", ProcessingTaskType);
                                            }
                                            else if (op == "==")
                                            {
                                                bResults[i__1] = bResults[i__1] == bResults[i_p1];
                                            }
                                            else if (op == "!=")
                                            {
                                                bResults[i__1] = bResults[i__1] != bResults[i_p1];
                                            }
                                            if  (bResults[i__1]) ///////test
                                            {
                                                ResultTypes[i__1] |= ResultTypes[i_p1];
                                            }
                                        }
                                    }
                                    nChanges++;
                                }
                                else if (OperatorPrecedence >= 17 && OperatorPrecedence <= 18)
                                {
                                    //"<", ">", "<=", ">=", "==", "!=", "&&", "||",
                                    if  ((ResultTypes[i__1] != eBool && ResultTypes[i__1] != eBool2) ||
                                         (ResultTypes[i_p1] != eBool && ResultTypes[i_p1] != eBool2)
                                         )
                                    {
                                        THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("operator '%S' for not boolean data at position %d", &op, hop[i].pos + 1), hop[i].pos);
                                    }
                                    if  (op == "&&")
                                    {
                                        bResults[i__1] = bResults[i__1] && bResults[i_p1];
                                    }
                                    else if (op == "||")
                                    {
                                        bResults[i__1] = bResults[i__1] || bResults[i_p1];
                                    }
                                    if  (bResults[i__1])
                                    {
                                        ResultTypes[i__1] |= ResultTypes[i_p1];
                                    }
                                    nChanges++;
                                }
                                else if (op == "?")
                                {
                                    qResults[i__1] = qResults[i_p1];
                                    bResults[i__1] = bResults[i_p1];
                                    ResultTypes[i__1] = ResultTypes[i_p1];
                                }
                                else if (op == ":")//OperatorPrecedence == m_nOpenBracketsArithIfIndex)
                                {
                                    if  (ProcessingTaskType & 0x20)
                                    {
                                        qResults[i__1] = qResults[i_p1];
                                        bResults[i__1] = bResults[i_p1];
                                        ResultTypes[i__1] = ResultTypes[i_p1];
                                    }
                                }
                                else
                                {
                                    THROW_CALC_FUNCTION_ERROR(CMaaString::sFormat("unexpected operator '%S' at position %d", &op, hop[i].pos + 1), hop[i].pos);
                                }
                                hop[i__1].nextindex = hop[i_p1].nextindex;
                            }
                        }
                    }
                }
                if  (nChanges1 <= 0)
                {
                    break;
                }
            }
        }
        dbg_printf("begin=%d, end=%d, hop[begin].nextindex= %d, ResultTypes[begin]=%d\n", begin, end, hop[begin].nextindex, ResultTypes[begin]);
        if  (begin < end &&
              (hop[begin].nextindex == end ||
               (begin < end && hop[begin].nextindex >= 0 && hop[begin].nextindex <= end && hop[hop[begin].nextindex].op == eEnd)
              )
             )
        {
            int res = ResultTypes[begin];
            if  (res == eInt)
            {
                //#undef dbg_printf
                //#define dbg_printf printf
                dbg_printf("qResults[%d] = %D\n", begin, qResults[begin]);
            }
            else
            {
                dbg_printf("bResults[%d] = %s\n", begin, bResults[begin] ? "true" : "false");
            }
            begin = end;//hop[begin].nextindex;
            dbg_printf("begin is moved to %d\n", begin);
            return res;
        }
        /*
          if   (ProcessingTaskType == 0)
          {
               qResults[begin] = 0;
               bResults[begin] = false;
               ResultTypes[begin] = eBool;
               return eBool;
          }
        */
        dbg_printf("CalcFunction1() return -1\n");
        return -1;
    }
    catch(...)
    {
        throw;
    }
    dbg_printf("CalcFunction1() return -1\n");
    return -1;
}

CMaaString CArithmeticExpression::GetArgFunctionName(int n, int * pFunctionsCount) noexcept
{
    if  (pFunctionsCount)
    {
        *pFunctionsCount = (int)szFunctions1.MaxIndex();
    }
    if  (n >= 0 && n < (int)szFunctions1.MaxIndex())
    {
        return szFunctions1[n];
    }
    return gsCMaaStringZ;
}

CMaaString CArithmeticExpression::GetKeywordTypeFunctionName(int n, int * pFunctionsCount) noexcept
{
    if  (pFunctionsCount)
    {
        *pFunctionsCount = (int)szFunctions2.MaxIndex();
    }
    if  (n >= 0 && n < (int)szFunctions2.MaxIndex())
    {
        return szFunctions2[n];
    }
    return gsCMaaStringZ;
}
