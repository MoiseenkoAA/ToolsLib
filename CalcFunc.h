
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/CalcFunc.h

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

class CMaaTLAEGlobalStrings // CArithmeticExpression global strings
{
public:
    enum e
    {
        e_ae_Operators = 0,
        e_ae__szFunctions1 = e_ae_Operators + 25,
        e_ae__szFunctions2 = e_ae__szFunctions1 + 9,
        e_ae__modificators = e_ae__szFunctions2 + 4,
        e_ae__Modificators_1st_chars_1 = e_ae__modificators + 24,
        e_ae__Modificators_1st_chars_2,
        e_ae__ch_modificators,
        e_ae__sp_ch_modificators,

        e_max
    };

protected:
    ceCMaaStringImp c[e_max]; // + 1

    CMaaString::CE::S m[e_max]; // + 1

public:
    constexpr CMaaTLAEGlobalStrings() noexcept;
    static const CMaaString& Get(int_ _e) noexcept;
};

inline const CMaaString& CMaaTLAEGlobalString(int_ _e) noexcept
{
    return CMaaTLAEGlobalStrings::Get(_e);
}

class CArithmeticExpression
{
    int m_Style;
    CMaaPtr_<CMaaString, 1> szOperators;
    //     CMaaPtr<CMaaString> szFunctions;// = {"ruleconnections", "shaperconnections", "time", "v", "traffic", "url", "host", "false", "true", "break", ""};
    CMaaPtr_<CMaaString, 1> szFunctions1;// = {"ruleconnections", "shaperconnections", "time", "v", "traffic", "url", "host", ""};
    CMaaPtr_<CMaaString, 1> szFunctions2;// = {"false", "true", "break", ""};
    CMaaPtr_<CMaaString, 1> sz_modificators;//[] = {"kbit/s", "mbit/s", "gbit/s", "tbit/s", "kbits/s", "mbits/s", "gbits/s", "tbits/s", "b/s", "kb/s", "mb/s", "gb/s", "tb/s", "b", "kb", "mb", "gb", "tb", "s", "m", "h", "d", ":", ""};
    CMaaString sz_Modificators_1st_chars_1;// = " :smhdSMHDbkmgtBKMGT)";
    CMaaString sz_Modificators_1st_chars_2;// = "  smhdSMHDbkmgtBKMGT)";
    CMaaPtr_<_qword, 1> qw_multiplicators;//[] =

    CMaaString sz_ch_modificators;
    CMaaString sz_sp_ch_modificators;
    CMaaPtr_<_qword, 1> qw_sp_ch_multiplicators;
protected:
    //CMaaAutoInitObject<int, -1> m_nOpenBracketsArithIfIndex;
    int m_nOpenBracketsArithIfIndex = -1;
public:
    struct sCalkFuncErr
    {
        CMaaString m_Msg;
        int m_Pos;
        CMaaString m_Expr;

        sCalkFuncErr(CMaaString txt, int n, CMaaString expr) noexcept
        :   m_Msg(txt),
            m_Pos(n),
            m_Expr(expr)
        {
        }
        CMaaString GetMsg(CMaaString PrefixText = "error", int PrefixTextLength = -1) const
        {
            PrefixTextLength = PrefixTextLength < 0 ? (warning_int)PrefixText.Length() : PrefixTextLength;
            CMaaString szText;
            if  (m_Pos > PrefixTextLength)
            {
                szText.Format("%S\n%S%*s^\n%S", &m_Expr, &PrefixText, m_Pos - PrefixTextLength, "", &m_Msg);
            }
            else
            {
                szText.Format("%S\n%*s^ %S\n%S", &m_Expr, m_Pos, "", &PrefixText, &m_Msg);
            }
            return szText;
        }
    };
    //#define THROW_CALC_FUNCTION_ERROR(txt, n) throw sCalkFuncErr(txt, n, expr)

    enum eLexemType
    {
        eReserved = -2,
        eError = -1,
        eOperator,
        eNumber,
        eFunc1,
        eFunc2, // with calculating of arguments expressions
        eEnd
    };

    enum
    {
        eBool = 0,
        eInt = 1,
        eBool2 = 2
    };

    struct op_data : public CMaaDLink
    {
        int op;
        int index, startindex, nextindex;
        int nextpos;
        int nargs;
        int pos;
        int oldindex, newindex;
        op_data /**pindex,*/ *pstartindex, *pnextindex, *pnextindex2;
        CMaaString GetText(int nn, CArithmeticExpression *expr, const CMaaUnivHash<int, CMaaString> &_hArgs, const CMaaUnivHash<int, CMaaString> &_hlArgs, CMaaPtr_<_qword, 1> &qResults, CMaaPtr_<bool, 1> &bResults, CMaaPtr_<int, 1> &ResultTypes);
        CMaaString GetText(int nn, CArithmeticExpression *expr, const CMaaUnivHash<int, CMaaString> &_hArgs, const CMaaUnivHash<int, CMaaString> &_hlArgs, _qword *qResults, bool *bResults, int *ResultTypes);
    };

    struct CExprCache : public CMaaDLink
    {
        int m_N;
        CExprCache(int N)
        :   m_N(N),
            hop(N + 10),
            qResults(N + 10),
            ResultTypes(N + 10)
        {
            m_UseCount = 0;
            m_Time = 0;
        }
        CMaaString expr;
        int begin;
        int end;
        CMaaPtr_<op_data, 1> hop;
        CMaaUnivHash<int, CMaaString> hArgs, hlArgs;
        //CMaaUnivHash<int, _qword> hqwConstants;
        CMaaPtr_<_qword, 1> qResults;
        //CMaaPtr<bool> bResults;
        CMaaPtr_<int, 1> ResultTypes;
        int m_UseCount;
        _dword m_Time;
        bool IsValid() const noexcept
        {
            return hop.IsValid() && hArgs.IsOK() && hlArgs.IsOK() && qResults.IsValid() && ResultTypes.IsValid();// && hqwConstants.IsOK();
        }
    };

    //CMaaAutoInitObject<int, 0> m_nExprCacheCount;
    int m_nExprCacheCount = 0;
    CMaaDList<CExprCache> m_CacheList;
    CMaaUnivHash<CMaaString, CExprCache *> m_hCache;

    enum
    {
        eCStyle,
        eBasicStyle
    };

    class CFuncCaller : public CMaaDLink
    {
    public:
        CFuncCaller() noexcept
        {
        }
        ~CFuncCaller() noexcept
        {
        }
        virtual int Func1(int Fn, CMaaString FunctionName, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, int index, int nArgs, _qword &qResult, bool &bResult, int Mode, CArithmeticExpression &ArithmeticExpression)
        {
            if  (Mode)
            {
                // skip calculations
            }
            else
            {
                return _Func1(Fn, FunctionName, hArgs, hlArgs, index, nArgs, qResult, bResult, Mode, ArithmeticExpression);
                qResult = 0;
                //bResult = true;
            }
            return eInt;     // eBool, eInt, eBool2
        }
        virtual int Func2(int Fn, CMaaString FunctionName, _qword &qResult, bool &bResult, int Mode, CArithmeticExpression &ArithmeticExpression)
        {
            if  (Mode)
            {
                // skip calculations
            }
            else
            {
                return _Func2(Fn, FunctionName, qResult, bResult, Mode, ArithmeticExpression);
                qResult = 0;
                //bResult = true;
            }
            return eInt;     // eBool, eInt, eBool2
        }
        virtual int _Func1(int /*Fn*/, CMaaString /*FunctionName*/, CMaaUnivHash<int, CMaaString> &/*hArgs*/, CMaaUnivHash<int, CMaaString> &/*hlArgs*/, int /*index*/, int /*nArgs*/, _qword &qResult, bool &/*bResult*/, int /*Mode*/, CArithmeticExpression &/*ArithmeticExpression*/)
        {
            qResult = 0;
            return eInt;     // eBool, eInt, eBool2
        }
        virtual int _Func2(int /*Fn*/, CMaaString /*FunctionName*/, _qword &qResult, bool &/*bResult*/, int /*Mode*/, CArithmeticExpression &/*ArithmeticExpression*/)
        {
            qResult = 0;
            return eInt;     // eBool, eInt, eBool2
        }
        virtual _dword GetTime()
        {
#ifdef _WIN32
            return (_dword)GetTickCount();
#else
            return (_dword)0;
#endif
        }
        virtual int GetTimeExpireValue()
        {
            return -1;
        }
    };

    CFuncCaller * m_pFnCaller;

    //CMaaAutoInitObject<int, 0> m_nTempExprCacheCount;
    int m_nTempExprCacheCount = 0;

    CArithmeticExpression(CFuncCaller * pFnCaller = nullptr, int Style = eCStyle);
    ~CArithmeticExpression();
    void SetArgFunctions(CMaaString * pNames, int Count);
    void SetKeywordTypeFunctions(CMaaString * pNames, int Count);
    CMaaString GetArgFunctionName(int n, int * pFunctionsCount = nullptr) noexcept;
    CMaaString GetKeywordTypeFunctionName(int n, int * pFunctionsCount = nullptr) noexcept;
    void RemoveFromCache(CMaaString expr) noexcept;
    bool IsInCache(CMaaString expr) noexcept;
    int GetLexem(CMaaString expr, int &index, int &begin, int &end, int &beginArg, int &endArg, int PrevLexemArIf, _qword &qwNumber, CMaaString * pArgs, CMaaString * plArgs, int &nArgsNum, int iUnarMinusIsAllowed);
    int CheckForValidity(CMaaUnivHash<int, op_data> &hop, int hindex0, int hindex, int ProcessingTaskType);
    int Pass1CalcFunction(CMaaString expr, int &begin, int &end, int &hindex, CMaaUnivHash<int, op_data> &hop, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, CMaaUnivHash<int, _qword> &hqwConstants, int ProcessingTaskType);
    int CalcFunction(CMaaString expr, _qword &qResult, bool &bResult, int CacheCount = 10);
    int CalcFunction(CMaaString &expr, int &begin, int end, op_data *hop, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs/*, CMaaUnivHash<int, _qword> &hqwConstants*/, _qword *qResults, bool *bResults, int *ResultTypes, int ProcessingTaskType);
    int CalcFunction1(CMaaString &expr, int &begin, int end, op_data *hop, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs/*, CMaaUnivHash<int, _qword> &hqwConstants*/, _qword *qResults, bool *bResults, int *ResultTypes, int ProcessingTaskType);
    virtual int CalcFunc1(int Fn, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, int index, int nArgs, _qword &qResult, bool &bResult, int &ResultType, int Mode);
    virtual int CalcFunc2(int Fn, _qword &qResult, bool &bResult, int &ResultType, int Mode);
    op_data * AddParentheses(op_data * begin, op_data * end, op_data * new_op, int &allocindex);
    void Prune(int TimeOutThreshold = -1, int method = 1);
};

//#define THROW_CALC_FUNCTION_ERROR(txt, n) throw sCalkFuncErr(txt, n, expr)

#if 0
//static int PrintDbgMode = 1;
static int PrintDbgMode = 0;
//#define PrintDbgMode 0
class CShaperFunctions : public CArithmeticExpression::CFuncCaller
{
    void * m_ptr;
public:
    CShaperFunctions(void * ptr)
    :   m_ptr(ptr)
    {
    }
    int _Func1(int Fn, CMaaString FunctionName, CMaaUnivHash<int, CMaaString> &hArgs, CMaaUnivHash<int, CMaaString> &hlArgs, int index, int nArgs, _qword &qResult, bool &bResult, int Mode, CArithmeticExpression &ArithmeticExpression)
    {
        if  (PrintDbgMode > 0)
        {
            //printf("_Func1(%d)\n", Fn);
        }
        qResult = -1;
        if  (Fn == 7)
        //if   (FunctionName == "test")
        {
            int Ret;
            CMaaString txtnum;
            nArgs && hArgs.Find((index << 16) + 0, &txtnum);
            //if   (m_pExpr)
            {
                Ret = ArithmeticExpression.CalcFunction(txtnum, qResult, bResult, 0x1000);
                //Ret = ArithmeticExpression.CalcFunction(txtnum, qResult, bResult, 0);
            }
            /*
               else
               {
                    Ret = CArithmeticExpression::eInt;
                    if   (mysscanf64(txtnum, &qResult, txtnum.Length()))
                    {
                         Ret = CArithmeticExpression::eInt;
                    }
               }
               */
            if  (PrintDbgMode > 0)
            {
                if  (Ret == CArithmeticExpression::eInt)
                {
                    printf("nArgs == %d, txtnum == %S, return = %D\n", nArgs, &txtnum, qResult);
                }
                else if (Ret == CArithmeticExpression::eBool || Ret == CArithmeticExpression::eBool2)
                {
                    printf("nArgs == %d, txtnum == %S, return = %s (type: %d)\n", nArgs, &txtnum, bResult ? "true" : "false", Ret);
                }
                else
                {
                    printf("nArgs == %d, txtnum == %S, return error (type: %d)\n", nArgs, &txtnum, Ret);
                }
            }
            return Ret;
        }
        if  (PrintDbgMode > 0)
        {
            printf("_Func1(%d), returning %D\n", Fn, qResult);
        }
        return CArithmeticExpression::eInt;     // eBool, eInt, eBool2
    }
    int _Func2(int Fn, CMaaString FunctionName, _qword &qResult, bool &bResult, int Mode, CArithmeticExpression &ArithmeticExpression)
    {
        if  (PrintDbgMode > 0)
        {
            printf("_Func2(%d), returning %D\n", Fn);
        }
        switch(Fn)
        {
        case 0:
            // "false"
            bResult = false;
            return CArithmeticExpression::eBool;
        case 1:
            // "true"
            bResult = true;
            return CArithmeticExpression::eBool;
        case 2:
            // "break"
            bResult = true;
            return CArithmeticExpression::eBool2;
        }
        qResult = 0;
        return CArithmeticExpression::eInt;     // eBool, eInt, eBool2
    }
};

int main()
{
    try
    {
        CShaperFunctions fn(nullptr);
        CArithmeticExpression ArithExp(&fn);
        CMaaString expr = "-1+20";
        expr = "true || ruleconnections(1.2.3) >= 0";
        expr = "(false && ruleconnections(1.2.3) >= 0)";
        //expr = "false && ruleconnections(1.2.3) >= 0";
        //expr = "10 >= 10 ? (1000) : 100000";
        expr = "10 >= 10 ? (1000) : 100000";
        //expr = "10 >= 10 ? 1000 : 100000";
        //expr = "10 >= 10 ? (1000) : 100000";
        expr = "10 > 10 ? test(1000) : test(test(100000+12345))";
        //expr = "10 >= 10 ? 1000 : 100000";
        CMaaTime hrTime(GetHRTime());
        CHrtMultiTimer prof(&hrTime);
        int CyclesCount = 1;//10000;
        CyclesCount = 10000;
        int i;
        int nCycleThr = 3;
        CMaaString x100;
        x100.Format(" x%d", CyclesCount);
        gRecCalls = 0;
        int PrintDbg_DEBUG_MODE_1 = 1;
        PrintDbgMode = PrintDbg_DEBUG_MODE_1;
        for (i = 0; i < CyclesCount; i++)
        {
            //printf("\r%d", i);
            //__utf8_printf_flush();
            int begin = 0;
            int end = expr.Length();
            _qword qResult = 0;
            bool bExpressionStatus;
            if  (i < nCycleThr)
            {
                printf("%S\n", &expr);
            }
            else
            {
                PrintDbgMode = 0;
            }
            int ResultType = ArithExp.CalcFunction(expr, qResult, bExpressionStatus);
            if  (ResultType == 1)
            {
                if  (i < nCycleThr)
                {
                    printf("= %D\n", qResult);
                }
                bExpressionStatus = qResult != 0;
            }
            else if (ResultType == 0)
            {
                if  (i < nCycleThr)
                {
                    printf("= %s\n", bExpressionStatus ? "true" : "false");
                }
            }
            else
            {
                throw CArithmeticExpression::sCalkFuncErr("unknown error", 0, expr);
            }
        }
        _qword t0 = prof.GetTime();
        prof.Stop(x100);
        printf("%S, %d reccalls (%d per calc), %.3lf ms per calc\n", &prof.GetResult(), gRecCalls, gRecCalls / CyclesCount, (double)t0/(double)CyclesCount/1000.0);

        ArithExp.Prune(1000);
        printf("%d...", ArithExp.m_nExprCacheCount);
        ArithExp.Prune(0);
        printf("%d...", ArithExp.m_nExprCacheCount);
        Sleep(1000);
        ArithExp.Prune(1000);
        printf(" %d\n", ArithExp.m_nExprCacheCount);
        Sleep(1000);

        gRecCalls = 0;
        PrintDbgMode = PrintDbg_DEBUG_MODE_1;
        for (i = 0; i < CyclesCount; i++)
        {
            //printf("\r%d", i);
            //__utf8_printf_flush();
            int begin = 0;
            int end = expr.Length();
            _qword qResult = 0;
            bool bExpressionStatus;
            if  (i < nCycleThr)
            {
                printf("%S\n", &expr);
            }
            else
            {
                PrintDbgMode = 0;
            }
            int ResultType = ArithExp.CalcFunction(expr, qResult, bExpressionStatus, 0);
            if  (ResultType == 1)
            {
                if  (i < nCycleThr)
                {
                    printf("= %D\n", qResult);
                }
                bExpressionStatus = qResult != 0;
            }
            else if (ResultType == 0)
            {
                if  (i < nCycleThr)
                {
                    printf("= %s\n", bExpressionStatus ? "true" : "false");
                }
            }
            else
            {
                throw CArithmeticExpression::sCalkFuncErr("unknown error", 0, expr);
            }
        }
        _qword t1 = prof.GetTime();
        prof.Stop(x100);
        printf("%S, %d reccalls (%d per calc), %.3lf ms per calc\n", &prof.GetResult(), gRecCalls, gRecCalls / CyclesCount, (double)t1/(double)CyclesCount/1000.0);
        printf("%.1lf faster\n", (double)t1/(double)t0);
    }
    catch(CArithmeticExpression::sCalkFuncErr err)
    {
        printf("%S\n", &err.GetMsg());
    }
    catch(...)
    {
        printf("unknown error\n");
    }
    return 0;
}
//#undef PrintDbgMode
#endif
