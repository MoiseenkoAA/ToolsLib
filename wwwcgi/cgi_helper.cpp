
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

 // ToolsLib/gd_helper.h

 /* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
  * All rights reserved.
  *
  * This library contains classes for working with CGI data.
  * Parse, get, send data.
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
//#include "fcgi_config.h"
//#include "fcgiapp.h"
#include "temp.h"

#ifdef _WIN32
// returns utf8 env variable or nullptr
char * my_getenv(const char * name)
{
    static thread_local_ CMaaString bb;
    wchar_t Buffer[1024 + 1];
    CMaaString aa = Utf8ToUnicode(CMaaStringRO(name));
    //CMaaString bb = Utf8ToUnicode(value);
    //SetEnvironmentVariableW((_WC_*)(const char*)aa, (_WC_*)(const char*)bb);
    DWORD x = GetEnvironmentVariableW((_WC_*)(const char*)aa, Buffer, sizeof(Buffer) / sizeof(wchar_t));
    if (x && x >= sizeof(Buffer) / sizeof(wchar_t))
    {
        CMaaPtr_<wchar_t, -1> Buf2(x + 1);
        if (!Buf2.IsValid())
        {
            x = 0;
        }
        else
        {
            x = GetEnvironmentVariableW((_WC_*)(const char*)aa, Buf2, (DWORD)(Buf2.Size() / sizeof(wchar_t)));
            if (x && x < Buf2.Size() / sizeof(wchar_t))
            {
                bb = UnicodeToUtf8(CMaaString(Buf2, 2 * x));
                return (char*)(const char*)bb;
            }
            else
            {
                x = 0;
            }
        }
    }
    if (x)
    {
        bb = UnicodeToUtf8(CMaaString(Buffer, 2 * x));
        return (char*)(const char*)bb;
    }
    return nullptr;
}
// set utf8 envirinment string
int my_setenv(const char* name, const char* value, int overwrite)
{
    if (!overwrite && my_getenv(name))
    {
        SetLastError(ERROR_ALREADY_EXISTS);
        return 1;
    }
    CMaaString aa = Utf8ToUnicode(CMaaStringRO(name));
    CMaaString bb = Utf8ToUnicode(CMaaStringRO(value));
    SetEnvironmentVariableW((_WC_*)(const char*)aa, (_WC_*)(const char*)bb);
    return 0;
}
// unset utf8 envirinment string
int my_unsetenv(const char* name)
{
    CMaaString aa = Utf8ToUnicode(CMaaStringRO(name));
    SetEnvironmentVariableW((_WC_*)(const char*)aa, nullptr);
    return 0;
}
#endif

bool gAllow_PATH_INFO = false;

thread_local_ CCGIHelper * CCGIHelper::g_imp = nullptr;

CCGIHelper::AttrVal::AttrVal() noexcept
{
}

CCGIHelper::AttrVal::AttrVal(const AttrVal &That) noexcept
:   //CMaaSLink(),
    m_Attr(That.m_Attr),
    m_Val(That.m_Val)
{
}

CCGIHelper::AttrVal::~AttrVal()
{
}

CCGIHelper::Attr2Val::Attr2Val()
:   m_nLines(-1)
{
}

CCGIHelper::Attr2Val::Attr2Val(const AttrVal &/*That*/)
:   m_nLines(-1)
{
}

CCGIHelper::Attr2Val::~Attr2Val()
{
}

/*
CCGIHelper::~CCGIHelper();
int CCGIHelper::GetParam(CMaaString Name, int defvalue = 0, bool *isexists = nullptr);
int CCGIHelper::GetParam(CMaaString Name, CMaaString *val = nullptr);
int CCGIHelper::SendReply(CMaaString Data, CMaaString Header = "");
int CCGIHelper::SendReply(CMaaFile f, CMaaString Header = "", _qword Start = 0, _qword End = -1);
int CCGIHelper::SendIndependentReply(CMaaString Data, CMaaString Header = ""); // with no if-mofified-since, partial responce and so on conversions
int CCGIHelper::SendIndependentReply(CMaaFile f, CMaaString Header = "");//, _qword Start = 0, _qword End = -1);
*/

char * CCGIHelper::getenv(const char * name)
{
    CMaaString *pval;
    if (m_phCgiParamOverride && (pval = (*m_phCgiParamOverride)[name]))
    {
        return (char *)(const char *)*pval;
    }
#ifdef FAST_CGI_SUPP
    if (m_pFastCgiRequest)
    {
        return FCGX_GetParam(name, m_pFastCgiRequest->envp);
    }
#endif
    return ::getenv(name);
}

CCGIHelper::CCGIHelper(_qword MaxContentLength, 
#ifdef FAST_CGI_SUPP
    FCGX_Request * 
#else
    void *
#endif
        pFastCgiRequest, CMaaUnivHash<CMaaString, CMaaString> *phCgiParamOverride, const CMaaString &ProgressFn, const CMaaString &ProgressFmt)
//:   m_fProgress((const char *)nullptr, CMaaFile::eNoMode, false)
{
    m_Send500 = false;
#ifdef FAST_CGI_SUPP
    m_pFastCgiRequest = pFastCgiRequest;
#endif
    m_phCgiParamOverride = phCgiParamOverride;
    if (!pFastCgiRequest)
    {
        if  (!gAllow_PATH_INFO && getenv("PATH_INFO") != nullptr)
        {
            exit(1);
        }
        m_fStdOut = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true);
    }
    m_subst_imp = g_imp;
    m_SubstOut = s_SubstOut;

    m_EndFileSent = false;
    m_ReplySentStatus = -1;
    m_IsHttp100ContinueHeaderPreceeded = 0;
#ifdef FAST_CGI_SUPP
    if (pFastCgiRequest && !gAllow_PATH_INFO && FCGX_GetParam("PATH_INFO", pFastCgiRequest->envp) != nullptr)
    {
        m_Send500 = true;
    }
    else
#endif
    {
        Initialize(MaxContentLength, /*phCgiParamOverride,*/ pFastCgiRequest, ProgressFn, ProgressFmt);
    }
    if  (!g_imp)
    {
        g_imp = this;
    }
}

//extern CMaaString g_temp;

//#define __log(x) { {CMaaFile f("__log___.txt", CMaaFile::eAC_SrSw, eNoExcept); f.fprintf("%S\n", x);}

void CCGIHelper::ReinitializeGetQS()
{
    if  (!m_Method.IsCi0("POST", 0))
    {
        Initialize(-1
#ifdef FAST_CGI_SUPP
            , m_pFastCgiRequest
            //, m_phCgiParamOverride
#endif
        ); // for 404 subst pages. still no cleanups before (warning)
    }
}

void CCGIHelper::Initialize(_qword MaxContentLength, 
#ifdef FAST_CGI_SUPP
    FCGX_Request * 
#else
    void *
#endif
        pFastCgiRequest, /*CMaaUnivHash<CMaaString, CMaaString> *phCgiParamOverride,*/ const CMaaString &ProgressFn, CMaaString ProgressFmt)
{
#ifdef FAST_CGI_SUPP
    m_pFastCgiRequest = pFastCgiRequest;
    //m_phCgiParamOverride = phCgiParamOverride;
#endif

    //{CMaaFile f("attr.txt", CMaaFile::eWC_SrSw, eNoExcept);}
    //    g_temp += "\nInitialize\n";
    MaxContentLength = MaxContentLength == -1 ? 512 * 1024 * 1024 : MaxContentLength;
    // MaxContentLength <= -2 is unlimited
    m_Error = 0;
    m_AttrCount = 0;
    {
        const char * envvarNames[] =
        {
            "REQUEST_METHOD", "CONTENT_TYPE", "CONTENT_LENGTH", "QUERY_STRING", "HTTP_RANGE", nullptr
        };
        CMaaString * envVars[] =
        {
            &m_envMethod, &m_envContentType, &m_envContentLength, &m_envQS, &m_envRange
        };
        const char * pszConvertingType = "ulr=r";
        CMaaString * destVars[] =
        {
            &m_Method, &m_ContentType, &m_ContentLength, &m_QS, &m_Range
        };
        const char * pszOnEmptySubstitution = "ynnnn";
        CMaaString Empty;
        /*static*/ CMaaString strOnEmptySubstitution[] =
        {
            CMaaString("GET"), Empty, Empty, Empty, Empty
        };

        for (int i = 0; envvarNames[i]; i++)
        {
            const char * env_str = getenv(envvarNames[i]);
            if  (env_str)
            {
                (*envVars[i]) = env_str;
            }
            else
            {
                (*envVars[i]).Empty();
            }
            switch(pszConvertingType[i])
            {
            case 'u':
            case 'U':
                (*destVars[i]) = (*envVars[i]).ToUpper(0);
                break;
            case 'l':
            case 'L':
                (*destVars[i]) = (*envVars[i]).ToLower(0);
                break;
            case 'r':
                (*destVars[i]) = (*envVars[i]).RemoveSpaces(true, true, " ");
                break;
            case '=':
            default:
                (*destVars[i]) = (*envVars[i]);
            }
            if  (!env_str && pszOnEmptySubstitution[i] == 'y')
            {
                (*destVars[i]) = strOnEmptySubstitution[i];
            }
        }
        m_Start = 0, m_End = -1;
        if  (m_Range.IsNotEmpty())
        {
            CMaaString txt = m_Range;
            if  (txt.IsLeftCi("bytes=", 6, 0))
            {
                txt = txt.Mid(6);
                int StopPos = -1;
                if  (txt[0] != '-')
                {
                    mysscanf64(txt, &m_Start, -1, &StopPos, "-");
                }
                else
                {
                    StopPos = 0;
                }
                if  (StopPos >= 0 && txt[StopPos] == '-')
                {
                    mysscanf64(StopPos + 1 + txt, &m_End, -1, nullptr, ","); // , -1, nullptr, "," added 24.03.2022 - get first range only
                }
            }
        }
    }
    //m_cl = m_rl = -1234;
    if  (m_Method == "POST")
    {
        m_Last100Cont = time(nullptr) - 3600;
        _qword cl = 0;
        mysscanf64(m_ContentLength, &cl);
        //m_cl = cl;
        if (m_phCgiParamOverride && !m_phCgiParamOverride->Find(CMaaStringZ, &m_PostData))
        {
            m_QS = m_PostData;
        }
        else if (cl > 0 && (MaxContentLength < 0 || cl <= MaxContentLength))
        {
            if  (ProgressFmt.IsEmpty())
            {
                ProgressFmt = "%1 / %2 = %3%%";
            }
            m_fProgress = CMaaFile(ProgressFn, CMaaFile::eWCD_SrSw, eNoExcept);
            if  (m_fProgress.IsOpen())
            {
                m_ProgressFn = ProgressFn;
            }
            CMaaFile fStdIn;
#ifdef FAST_CGI_SUPP
            if (!m_pFastCgiRequest)
#endif
            {
                fStdIn = CMaaFile(CMaaFileStdin, CMaaFile::eR_SrSw, eNoExcept);
            }
            CMaaConcatString1 str;
            _qword len = 0;
            CMaaPtr_<char, 1> Buffer(1024 * 1024);
            while((
#ifdef FAST_CGI_SUPP
                m_pFastCgiRequest ||
#endif
                 fStdIn.IsOpen()) && len < cl)
            {
#ifdef FAST_CGI_SUPP
                _dword x = !m_pFastCgiRequest ? 
                    fStdIn.Read(Buffer, cl - len >= (_qword)Buffer.Size() ? (_dword)Buffer.Size() : (_dword)(cl - len)) :
                    FCGX_GetStr(Buffer, cl - len >= (_qword)Buffer.Size() ? (_dword)Buffer.Size() : (_dword)(cl - len), m_pFastCgiRequest->in);
#else
                _dword x = fStdIn.Read(Buffer, cl - len >= (_qword)Buffer.Size() ? (_dword)Buffer.Size() : (_dword)(cl - len));
#endif
                if  (/*x < 0 ||*/ (x == 0 && 1==1))
                {
                    break;
                }
                str.Add(Buffer, (int)x);
                len += x;
                if  (m_fProgress.IsOpen())
                {
                    m_fProgress.Seek(0);
                    m_fProgress.fprintf2("%,D%,D%.1lf%.lf%.2lf", ProgressFmt, len, cl, (double)len * 100.0 / cl, (double)len * 100.0 / cl, (double)len * 100.0 / cl);
                }
            }
            //m_fProgress.Close();
            //fprintf(stderr, "len = %d, cl = %d\n", (int)len, (int)cl);
            //m_rl = len;
            if  (len == cl)
            {
                m_PostData = m_QS = (CMaaString)str;
                //m_PostData = (CMaaString)str;
            }
            else
            {
                m_Error = 1;
            }
        }
        else if (cl > 0)
        {
            m_Error = 2;
        }
    }

    /*
    g_temp += "\n";
    g_temp += m_ContentType;
    g_temp += "\n";
    */
    //CMaaFile ll(s_SubstIn ? "/var/ram/maa/ssi.txt" : nullptr, CMaaFile::eAC_SrSw, eNoExcept);
    //ll.fprintf("m=%S, qs=%S, %s|%S\n", &m_Method, &m_QS, getenv("QUERY_STRING"), &s_Qs);
    //if (s_SubstIn)
    //{
    //CMaaString e;
    //e.Format("REQUEST_METHOD=%S", &Method);
    //putenv((char *)(const char *)e);
    //e.Format("QUERY_STRING=%S", &s_Qs);
    //ll.fprintf("putenv: %S\n", &e);
    //putenv((char *)(const char *)e);
    //ll.fprintf("getenv: %s\n", getenv("QUERY_STRING"));
    //}
    if  (m_ContentType.Find("application/x-www-form-urlencoded") >= 0 || m_ContentType.IsEmpty() || m_Method == "GET")
    {
        //        CMaaFile fff("/var/ram/maa/temp1.txt", CMaaFile::eACD_SrSw, eNoExcept);
        //fprintf(stderr, "attributes m_QS.Length() = %d\n", m_QS.Length());

        //        fff.fprintf("m_QS = %S\n\n", &m_QS);

        StrInt n = 0;
        while(n < m_QS.Length())
        {
            StrInt n1 = m_QS.Find(n, '=');
            StrInt n2 = m_QS.Find(n, '&');
            //fprintf(stderr, "n1 = %d, n2 = %d\n", n1, n2);
            AttrVal av;
            if  (n1 < 0 && n2 < 0)
            {
                av.m_Attr = m_QS.Mid(n);
                //av.m_Val;
                n = m_QS.Length();
            }
            else
            {
                n1 = n1 < 0 ? n2 : n1;
                n2 = n2 < 0 ? m_QS.Length() : n2;
                av.m_Attr = m_QS.Mid(n, n1 - n);
                if  (n1 + 1 <= n2)
                {
                    av.m_Val = m_QS.Mid(n1 + 1, n2 - (n1 + 1));
                }
                n = n2 + 1;
            }
            //__log(&CMaaString::sFormat("%S=%S\n", &av.m_Attr, &av.m_Val));
            AttrVal * s_pav = TL_NEW AttrVal(av);
            if  (s_pav)
            {
                //m_srcAttrList.AddAtBack(s_pav);
                //__log(&CMaaString::sFormat("m_srcAttrList: %p\n", pav));
            }
            else
            {
                m_Error = 10;
                break;
            }
            AttrVal * pav = nullptr;
            try
            {
                pav = TL_NEW AttrVal;
            }
            catch(...)
            {
                delete s_pav;
                throw;
            }


            if  (pav)
            {
                m_srcAttrList.AddAtBack(s_pav);
                pav->m_Attr = UrlWithMixedUtf8ToUtf8(av.m_Attr);
                pav->m_Val = UrlWithMixedUtf8ToUtf8(av.m_Val);//.ConvertHttpParam();
                m_AttrList.AddAtBack(pav);
            }
            else
            {
                //delete m_srcAttrList.GetFromBack();
                delete s_pav;
                m_Error = 10;
                break;
            }
            //            fff.fprintf("attr: %S\nval : %S\n", &av.m_Attr, &av.m_Val);
            //            fff.fprintf("attr: %S\nval : %S\n\n", &pav->m_Attr, &pav->m_Val);
            m_AttrCount++;
            m_hsrcAttr.AddOver(av.m_Attr, av.m_Val);
            m_hAttr.AddOver(pav->m_Attr, pav->m_Val);
            //fprintf(stderr, "attribute n %d\n", m_AttrCount);
        }
        {
            //fprintf(stderr, "filling...\n");
            AttrVal * pav;
            int n = 0;
            for (pav = m_AttrList.LookAtFront(); pav; pav = m_AttrList.Next(pav))
            {
                /*int x =*/ m_hNum2AttrMap.Add(n++, pav);
                //fprintf(stderr, "%d %d\n", n - 1, x);
            }
        }
    }
    else if (m_ContentType.Find("multipart/form-data") >= 0)
    {
        /*
    g_temp += "\n";
    g_temp += m_ContentType;
    g_temp += "\n";
*/
        CMaaString srcData = m_QS;

        /*int Length =*/ srcData.Length();
        CMaaString Boundary = srcData.GetLine(false);
        //time_t Time = time(nullptr);
        //int nfile = -1;
        CMaaString Data, lastData; //last unparced data
        Data = lastData = srcData;
        //g_temp.Format("%S\nData.Length()=%d\n", &g_temp, Data.Length());
        while(Data.IsNotEmpty())
        {
            const StrInt pos0 = Data.Find(Boundary);
            if  (pos0 < 0)
            {
                break;
            }
            Data = Data.RefMid(pos0);
            StrInt CrLfLen = 0;
            StrInt n = Data.FindCrLf(0, &CrLfLen);
            if  (n < 0 || CrLfLen == 0)
            {
                break;
            }
            Attr2Val * p = TL_NEW Attr2Val;
            if  (!p)
            {
                break;
            }
            //p->m_nLines = -1;
            CMaaString crlf;// = Data.RefMid(pos2 >= 0 ? pos2 : 0, CrLfLen);
            CMaaString Line = Data.GetLine(true, true, true, &crlf);
            p->m_hLinesAttr.AddOver(p->m_nLines++, Line);
            if  (crlf.IsEmpty())
            {
                delete p;
                break;
            }
            //g_temp.Format("%S\nData.Length()=%d\n", &g_temp, Data.Length());
            while(Data.IsNotEmpty())
            {
                //g_temp.Format("%S\nData.Length()=%d\n", &g_temp, Data.Length());
                CMaaString Line = Data.GetLine(crlf);
                if  (Line.IsEmpty())
                {
                    break;
                }
                p->m_hLinesAttr.AddOver(p->m_nLines++, Line);
                if  (Line.IsLeftCi("content-disposition: ", (int)strlen("Content-Disposition: "), 0))
                {
                    p->m_ContentDisposition = Line.Mid((int)strlen("Content-Disposition: "));
                }
                else if (Line.IsLeftCi("content-type: ", (int)strlen("Content-type: "), 0))
                {
                    p->m_ContentType = Line.Mid((int)strlen("Content-type: "));
                }
            }
            n = Data.Find(Boundary);
            if  (n < 0)
            {
                delete p;
                break;
            }

            p->m_ContentData = Data.RefMid(0, n);
            /*
CMaaFile log("log.txt", CMaaFile::eAC_SrSw, eNoExcept);
if (!log.IsOpen())
{
    g_temp += log.GetMsg();
    log = CMaaFile("/var/ram/log.txt", CMaaFile::eAC_SrSw, eNoExcept);
    if (!log.IsOpen())
    {
	g_temp += log.GetMsg();
    }
}
log.fprintf("\npoint 1\n---%S---%S---\n", &p->m_ContentData.Right(crlf.Length()), &crlf);
g_temp.Format("%S\npoint 1\n---%S---%S---\n", &g_temp, &p->m_ContentData.Right(crlf.Length()), &crlf);
*/
            if  (p->m_ContentData.IsRight(crlf))
            {
                //log.fprintf("\npoint 2\n%d\n", p->m_ContentData.Length());
                //g_temp.Format("%S\npoint 2\n%d\n", &g_temp, p->m_ContentData.Length());
                p->m_ContentData = p->m_ContentData.RefMid(0, p->m_ContentData.Length() - crlf.Length());
                //log.fprintf("\npoint 2\n%d\n", p->m_ContentData.Length());
                //g_temp.Format("%S\npoint 2\n%d\n", &g_temp, p->m_ContentData.Length());
            }
            Data = Data.RefMid(n);
            lastData = Data;

            n = 0;
            int nFormData = 0;
            while(1)
            {
                CMaaString AttrName;
                CMaaString AttrValue;

                //Content-Disposition: form-data; name="LoginInfo"
                while(n < p->m_ContentDisposition.Length() && (p->m_ContentDisposition[n] == ';' || p->m_ContentDisposition[n] == ' '))
                {
                    n++;
                }
                if  (n >= p->m_ContentDisposition.Length())
                {
                    break;
                }

                //Content-Disposition: form-data; name="LoginInfo"
                StrInt x = p->m_ContentDisposition.Find(n, '=');
                StrInt x2 = p->m_ContentDisposition.Find(n, ';');
                x2 = x2 < 0 ? p->m_ContentDisposition.Length() : x2;
                if  (x < x2 && x >= 0)
                {
                    AttrName = p->m_ContentDisposition.Mid(n, x - n);
                    n = x + 1;
                    if  (p->m_ContentDisposition[n] == '"')
                    {
                        StrInt x3 = p->m_ContentDisposition.Find(n + 1, '"');
                        if  (x3 >= 0)
                        {
                            AttrValue = p->m_ContentDisposition.Mid(n + 1, x3 - (n + 1));
                            n = p->m_ContentDisposition.Find(x3 + 1, ';');
                            n = n < 0 ? p->m_ContentDisposition.Length() : n + 1;
                        }
                        else
                        {
                            n = x2 + 1;
                        }
                    }
                    else
                    {
                        AttrValue = p->m_ContentDisposition.Mid(n + 1, x2 - (n + 1));
                        n = x2 + 1;
                    }
                }
                else if (x2 >= 0)
                {
                    AttrName = p->m_ContentDisposition.Mid(n, x2 - n);
                    n = x2 + 1;
                    if  (AttrName == "form-data")
                    {
                        nFormData = 1;
                    }
                }
                //g_temp.Format("%S\n%S=%S\n", &g_temp, &AttrName, &AttrValue);
                if  (AttrName == "name")
                {
                    p->m_Name = AttrValue;
                }
                else if (AttrName == "filename")
                {
                    p->m_FileName = AttrValue;
                    //g_temp.Format("%S\nFileName=%S\n", &g_temp, &p->m_FileName);
                }
                m_hsrcAttr2.AddOver(AttrName, AttrValue);
                m_hAttr2.AddOver(AttrName, AttrValue);
                if  (nFormData == 1)
                {
                    nFormData++;
                }
                else if (nFormData > 1)
                {
                    AttrName = AttrValue;
                    AttrValue = p->m_ContentData;
                    m_hAttr.AddOver(AttrName, AttrValue);
                    //{CMaaFile f("attr.txt", CMaaFile::eAC_SrSw, eNoExcept); f.fprintf("%S=%S\n", &AttrName, &AttrValue);}
                    m_hsrcAttr.AddOver(AttrName, AttrValue);
                    AttrVal * pav = TL_NEW AttrVal;
                    if  (pav)
                    {
                        pav->m_Attr = AttrName;
                        pav->m_Val = AttrValue;
                        m_srcAttrList.AddAtBack(pav);
                        AttrVal * pav2 = TL_NEW AttrVal(*pav);
                        if  (pav2)
                        {
                            m_AttrList.AddAtBack(pav2);
                        }
                    }
                }
            }
            m_Attr2List.AddAtBack(p);
        }
        {
            //fprintf(stderr, "filling...\n");
            AttrVal * pav;
            int n = 0;
            for (pav = m_AttrList.LookAtFront(); pav; pav = m_AttrList.Next(pav))
            {
                /*int x =*/ m_hNum2AttrMap.Add(n++, pav);
                //fprintf(stderr, "%d %d\n", n - 1, x);
            }
        }
    }
}

CCGIHelper::~CCGIHelper()
{
    //fprintf(stderr, "CCGIHelper::~CCGIHelper()\nm_Attr2List\n"); fflush(stderr);
    m_Attr2List.RemoveAll();
    //fprintf(stderr, "m_AttrList\n"); fflush(stderr);
    m_AttrList.RemoveAll();
    //fprintf(stderr, "m_srcAttrList\n"); fflush(stderr);
    m_srcAttrList.RemoveAll();
    //fprintf(stderr, "CCGIHelper::~CCGIHelper() - ret\n"); fflush(stderr);
    if  (g_imp == this)
    {
        g_imp = nullptr;
        if  (m_SubstOut)
        {
            g_imp = m_subst_imp;
            m_subst_imp = nullptr;
        }
    }
    m_fProgress.Close();
    if  (m_ProgressFn.IsNotEmpty())
    {
        CMaaFile::Remove(m_ProgressFn, false);
    }
}

bool CCGIHelper::GetParamByNum(int num, CMaaString &Attr, CMaaString &Val) const noexcept
{
    // AttrVal* pav = nullptr;
    // /*int r =*/ m_hNum2AttrMap.Find(num, &pav);
    AttrVal *pav = m_hNum2AttrMap[num, nullptr];
    if  (pav)
    {
        Attr = pav->m_Attr;
        Val = pav->m_Val;
        return true;
    }
    return false;
}

int CCGIHelper::GetParam(const CMaaString &Name, int defvalue, bool *isexists) const noexcept
{
    CMaaString val;
    if  (!m_hAttr.Find(Name, &val))
    {
        int retval;
        if  (val.sscanfInt(&retval) == 1)
        {
            if (isexists)
            {
                *isexists = true;
            }
            return retval;
        }
    }
    if (isexists)
    {
        *isexists = false;
    }
    return defvalue;
}

int CCGIHelper::GetParam(const CMaaString &Name, CMaaString *val) const noexcept
{
    return m_hAttr.Find(Name, val);
}

int CCGIHelper::SetParam(const CMaaString &Name, const CMaaString &Val)
{
    return m_hAttr.AddOver(Name, Val);
}

CMaaString CCGIHelper::GetContentTypeByFileName(const CMaaString &FileName, const CMaaString &DefaultContentType)
{
    return GetContentTypeByExtention(FileName.Extention(0), DefaultContentType);
}

CMaaString CCGIHelper::GetContentTypeByFileExtention(const CMaaString &Ext, const CMaaString &DefaultContentType)
{
    return GetContentTypeByExtention(Ext, DefaultContentType);
}

static constexpr CMaaConstStr s_ct[][2] =
{
    {"xml", "text/xml"},
    {"d""oc", "application/msword"},
    {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    //application/vnd.ms-excel   xls xlm xla xlc xlt xlw
    {"xls", "application/vnd.ms-excel"},
    {"xla", "application/vnd.ms-excel"},
    {"xlc", "application/vnd.ms-excel"},
    {"xlt", "application/vnd.ms-excel"},
    //application/vnd.openxmlformats-officedocument.spreadsheetml.sheet   xlsx
    {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    //application/vnd.ms-powerpoint   ppt pps pot
    {"ppt", "application/vnd.ms-powerpoint"},
    {"pps", "application/vnd.ms-powerpoint"},
    //application/vnd.openxmlformats-officedocument.presentationml.presentation   pptx
    {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {"bin", "application/octet-stream"},
    {"dll", "application/octet-stream"},
    {"exe", "application/octet-stream"},
    {"pdf", "application/pdf"},
    {"p7c", "application/pkcs7-mime"},
    {"ai",  "application/postscript"},
    {"eps", "application/postscript"},
    {"ps",  "application/postscript"},
    {"rtf", "application/rtf"},
    {"fdf", "application/vnd.fdf"},
    {"arj", "application/x-arj"},
    {"gz",  "application/x-gzip"},
    {"class", "application/x-java-class"},
    {"js",  "application/x-javascript"},
    {"lzh", "application/x-lzh"},
    {"lnk", "application/x-ms-shortcut"},
    {"tar", "application/x-tar"},
    {"hlp", "application/x-winhelp"},
    {"cert", "application/x-x509-ca-cert"},
    {"zip", "application/zip"},
    {"7z", "application/x-7z-compressed"},
    //          {"zip", "application/octet-stream"},
    {"rar", "application/x-rar-compressed"},
    {"cab", "application/x-compressed"},
    {"arj", "application/x-compressed"},
    {"aif", "audio/aiff"},
    {"aifc", "audio/aiff"},
    {"aiff", "audio/aiff"},
    {"au",  "audio/basic"},
    {"snd", "audio/basic"},
    {"mid", "audio/midi"},
    {"rmi", "audio/midi"},
    {"mp3", "audio/mpeg"},
    {"vox", "audio/voxware"},
    {"wav", "audio/wav"},
    {"ra",  "audio/x-pn-realaudio"},
    {"ram", "audio/x-pn-realaudio"},
    {"bmp", "image/bmp"},
    {"gif", "image/gif"},
    {"png", "image/png"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"svg", "image/svg+xml"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"xbm", "image/xbm"},
    {"wrl", "model/vrml"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"c",   "text/plain"},
    {"cpp", "text/plain"},
    {"def", "text/plain"},
    {"h",   "text/plain"},
    {"txt", "text/plain"},
    {"rtx", "text/richtext"},
    {"rtf", "text/richtext"},
    {"java", "text/x-java-source"},
    {"css", "text/css"},
    {"mpeg", "video/mpeg"},
    {"mpg", "video/mpeg"},
    {"mpe", "video/mpeg"},
    {"avi", "video/msvideo"},
    {"mov", "video/quicktime"},
    {"qt",  "video/quicktime"},
    {"shtml", "wwwserver/html-ssi"},
    {"asa", "wwwserver/isapi"},
    {"asp", "wwwserver/isapi"},
    {"cfm", "wwwserver/isapi"},
    {"dbm", "wwwserver/isapi"},
    {"isa", "wwwserver/isapi"},
    {"plx", "wwwserver/isapi"},
    {"url", "wwwserver/isapi"},
    {"cgi", "wwwserver/isapi"},
    {"php", "wwwserver/isapi"},
    {"wcgi", "wwwserver/isapi"},
    {"m3u", "audio/mpegurl"},
    {"pls", "audio/scpls"},
    {"ogv", "video/ogg"},
    {"flv", "video/flv"},
    {"mp4", "video/mp4"},
    {"m4v", "video/x-m4v"},
    {"mov", "video/quicktime"},
    {"f4v", "video/x-f4v"},
    {"webm", "video/webm"},
    {"apk", "application/vnd.android.package-archive"},
    {"", ""}
};

static const CMaaUnivHash<CMaaString, CMaaString>* s_pMimeTable = nullptr;
const CMaaUnivHash<CMaaString, CMaaString>* CCGIHelper::MakeMimeTable()
{
    static CMaaUnivHash<CMaaString, CMaaString> t(sizeof(s_ct) / sizeof(s_ct[0]), [&]()
        {
            for (int i = 0; s_ct[i][0].len && s_ct[i][1].len; i++)
            {
                t.Add(s_ct[i][0], s_ct[i][1]);
            }
            s_pMimeTable = &t;
        });
    return s_pMimeTable;
}
CMaaString CCGIHelper::GetContentTypeByExtention(CMaaString Ext, const CMaaString &DefaultContentType)
{
    Ext = Ext.ToLower(0);
    if (s_pMimeTable)
    {
        return (*s_pMimeTable)[Ext, DefaultContentType];
    }
    for (int i = 0; s_ct[i][0].len && s_ct[i][1].len; i++)
    {
        if  (Ext == s_ct[i][0])
        {
            return s_ct[i][1];
        }
    }
    return DefaultContentType;
}

const char * szHTTPS_ADD_HEADER = "";
const char * szHTTP_ADD_HEADER = "";

/*
const char * szHTTPS_ADD_HEADER = 
"Strict-Transport-Security: max-age=7775999; includeSubDomains\r\n"
//"Content-Security-Policy: default-src 'none'; script-src 'self'; connect-src 'self'; img-src 'self'; style-src 'self';base-uri 'self';form-action 'self';\r\n"
"X-Frame-Options: SAMEORIGIN\r\n"
"X-XSS-Protection: 1; mode=block\r\n"
"X-Content-Type-Options: nosniff\r\n"
//"Referrer-Policy: no-referrer\r\n"
;

const char * szHTTP_ADD_HEADER = 
//"Strict-Transport-Security: max-age=7775999; includeSubDomains\r\n"
"X-Frame-Options: SAMEORIGIN\r\n"
"X-XSS-Protection: 1; mode=block\r\n"
"X-Content-Type-Options: nosniff\r\n"
//"Referrer-Policy: no-referrer\r\n"
;
*/

#ifdef FAST_CGI_SUPP
int CCGIHelper::SendFCgiHeaderEx(CMaaString txt)
{
    //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out); return txt.Length();

    const int l0 = txt.Length();
    CMaaString ContentType;
    while(txt.IsNotEmpty())
    {
        CMaaString Line = txt.GetLine(CMaaStringCrLf, true, false, true);
        if (Line.Length() == 2)
        {
            if (ContentType.IsNotEmpty())
            {
                FCGX_PutStr(ContentType, ContentType.Length(), m_pFastCgiRequest->out);
            }
            FCGX_PutStr(Line, Line.Length(), m_pFastCgiRequest->out);
            break;
        }
        if (Line.IsEmpty())
        {
            // error
            //if (ContentType.IsNotEmpty())
            {
                //FCGX_PutStr(ContentType, ContentType.Length(), m_pFastCgiRequest->out);
            }
            break;
        }
        if (Line.IsLeftCi("Content-type: ", 14, 0))
        {
            ContentType = Line;
            continue;
        }
        //if (Line.IsLeftCi("Content-Length: ", 16, 0))
        {
            //continue;
        }
        FCGX_PutStr(Line, Line.Length(), m_pFastCgiRequest->out);
    }
    if (txt.IsNotEmpty())
    {
        FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
    }
    return l0 - txt.Length();
}
#endif

int CCGIHelper::SendReply(const CMaaString &Data, const CMaaString &ContentType, const CMaaString &FileName, const CMaaString &Header, const CMaaString &ErrorText, time_t t) //, CMaaFile fStdOut)
{
    return SendReply(Data, ContentType, FileName, Header, ErrorText, t, false); // , fStdOut);
}
CMaaString CCGIHelper::GetMethod() const noexcept
{
    return m_Method;
}
int CCGIHelper::SendReply(CMaaString Data, CMaaString ContentType, CMaaString FileName, CMaaString Header, const CMaaString &ErrorText, time_t t, bool bInline) //, CMaaFile fStdOut)
{
    //ErrorText = ErrorText.ToHttpHtmlDisplayedText(true);
    if (!IsSubsted() && m_n100Count > 0)
    {
        if (Header.IsLeft("HTTP/1.", 7))
        {
        }
        else if (Header.IsLeft("Status: ", 8))
        {
            int pos = 8;
            while(Header[pos] == ' ')
            {
                pos++;
            }
            Header = CMaaString("HTTP/1.1 ") + Header.Mid(pos);
        }
        else
        {
            Header = CMaaString("HTTP/1.1 200 OK\r\n") + Header;
        }
    }
    m_EndFileSent = false;
    m_ReplySentStatus = 0;

    int Ret = 0;
    
    if (m_Send500)
    {
        CMaaString txt = "Status: 500 Error\r\nContent-type: text/plain\r\nCache-control: no-cache\r\n\r\n";
        //if   (m_n100Cont > 0)
        {
            //txt = "HTTP/1.1 500 Error\r\nContent-type: text/plain\r\nCache-control: no-cache\r\n\r\n";
        }
#ifdef FAST_CGI_SUPP
        if (m_pFastCgiRequest)
        {
            SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
        }
        else
#endif
        {
            m_fStdOut.Write(txt);
        }
        return 0;
    }

    time_t tims = 0;
    if  (0)
    {
        const char * strIms = getenv("HTTP_IF_MODIFIED_SINCE");
        if  (strIms)// && txt.Find("no-cache") < 0 && Pragma.Find("no-cache") < 0)
        {
            if  (ConvertFromWebDateTime(CMaaStringRO(strIms), &tims))
            {
                // time is got
            }
        }
    }

    CMaaString CacheControl, Pragma;
    {
        const char * p = getenv("HTTP_CACHE_CONTROL");
        CacheControl = p ? p : "";
    }
    {
        const char * p = getenv("HTTP_PRAGMA");
        Pragma = p ? p : "";
    }
    if  (CacheControl.FindCi("no-cache") >= 0)
    {
        CacheControl = "Cache-Control: no-cache\r\n";
        Pragma.Empty();
    }
    else
    {
        CacheControl.Empty();
        if  (Pragma.FindCi("no-cache") >= 0)
        {
            CacheControl = "Pragma: no-cache\r\n";
        }
        Pragma.Empty();
    }
    CMaaString ManualStatus;
    {
        StrInt n1 = 0, n2 = 0;
        Header.CgiGetValue("Status: ", &n1, &n2);
        if (n2 > n1)
        {
            ManualStatus = Header.Mid(n1, n2 + 2 - n1);
            Header.SetLeftMid(n1, n2 + 2);
        }
    }
    CMaaString cc = Header.CgiGetValue("Cache-Control: ");
    CMaaString cp = Header.CgiGetValue("Pragma: ");
    CMaaString Connection;// = "Connection: Keep-Alive\r\n"; //27.04.2024
    if  (Header.CgiGetValue("Connection: ").IsEmpty())
    {
        const char * p = getenv("HTTP_CONNECTION");
        if  (!p || *p == '\0')
        {
            Connection = "Connection: close\r\n";
            //Connection = "Connection: keep-alive\r\n";
        }
    }
    if  (CacheControl.IsNotEmpty())
    {
        if  (cc.IsNotEmpty() || cp.Find("no-cache") >= 0)
        {
        }
        else
        {
            Header = CacheControl + Header;
        }
    }
    //Header = CMaaString("Content-Encoding: none\r\n") + Header; // 10.05.2024
    CMaaString cc2 = Header.CgiGetValue("Cache-Control: "); // 26.11.2015
    if  (cc2.Find("no-cache") >= 0)
    {
        t = 0; // or -1
    }
    //{
    //    CMaaFile log("/var/ram/maa/cache.txt", CMaaFile::eACD_SrSw, eNoExcept);
    //    log.fprintf(":::%S\n::%S\n:%d\n", &Header, &cc2, cc2.Find("no-cache"));
    //}

    _qword Start = 0, End = -1;
    Start = m_Start, End = m_End;
    /*
    {
	const char * p = getenv("HTTP_RANGE");
	if  (p)
	{
	    CMaaString txt(p);
	    if	(txt.IsLeftCi("bytes=", 6))
	    {
		txt = txt.Mid(6);
		int StopPos = -1;
		if (txt[0] != '-')
		{
		    mysscanf64(txt, &Start, -1, &StopPos, "-");
		}
		else
		{
		    StopPos = 0;
		}
		if (StopPos >= 0)
		{
		    mysscanf64(StopPos + 1 + txt, &End);
		}
	    }
	}
    }
    */

    CMaaString Method = m_Method;
    /*
    {
	const char * RequestMethod = getenv("REQUEST_METHOD");
	Method = RequestMethod ? RequestMethod : "GET";
	Method = Method.ToUpper(0);
    }
    */

    if  (Method == "HEAD")
    {
        Start = 0;
        End = -1;
    }

    if  (ErrorText.IsNotEmpty())
    {
        Data = ErrorText.ToHttpHtmlDisplayedText(true);
        ContentType = "text/plain";
        FileName.Empty();
        //Start = 0;
        //End = Data.Length() - 1;
        Start = 0;
        End = -1;
        if  (Data.FindCi("<html>") < 0)
        {
            ContentType = "text/html";
            Data.Format2("%S",
       "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n"
       "\n"
       "<HTML>\n"
       "<HEAD>\n"
       "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\">\n"
       "<TITLE>Ошибка</TITLE>\n"
       "</HEAD>\n"
       "<BODY>\n"
       "<pre>%1</pre>\n"
       "</BODY>\n"
       "</HTML>", &Data);

        }
    }

    _qword len = Data.Length();
    //_qword pos = 0;

    //CMaaFile m_fStdOut(CMaaFileStdout, CMaaFile::eW_SrSw, eNoExcept);

    if  (Method != "HEAD" && Method != "GET" && Method != "POST")
    {
        CMaaString txt;
        txt.Format(
            "Status: 405 Method Not Allowed\r\n"
            "Content-Length: 0\r\n"
            // "Connection: close\r\n"
            // "Connection: keep-alive\r\n"
            "\r\n"
           );
        try
        {
            if  (m_SubstOut)
            {
                s_Output += txt;
            }
            else
            {
#ifdef FAST_CGI_SUPP
                if (m_pFastCgiRequest)
                {
                    SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
                }
                else
#endif
                {
                    m_fStdOut.Write(txt);
                }
            }
        }
        catch(...)
        {
            Ret = 1;
        }
        return Ret;
        //return 1;
    }

    if  (End == -1)
    {
        End = len - 1;
    }

    if  (End == len -1)
    {
        //	    bStat = true;
    }

    if  (FileName.IsNotEmpty())
    {
        if  (ContentType.IsEmpty())
        {
            ContentType = GetContentTypeByFileName(FileName, "application/octet-stream");
        }
        if  (!bInline)
        {
            FileName.Format("Content-Disposition: attachment; filename=\"%S\"\r\n", &FileName);
        }
        else
        {
            // https://tools.ietf.org/html/rfc1806
            FileName.Format("Content-Disposition: inline; filename=\"%S\"\r\n", &FileName);
        }
    }
    if  (ContentType.IsEmpty())
    {
        ContentType = "application/octet-stream";
    }

    CMaaString LastModified;
    if  (t && t != (time_t)-1)
    {
        CMaaString dt = GetDateWebFormat(t);
        LastModified.Format("Last-Modified: %S\r\n", &dt);
    }

    //int m_IsHttp100ContinueHeaderPreceeded = 0;
    if  (Header.IsLeft("HTTP/1.", 7))
    {
        m_IsHttp100ContinueHeaderPreceeded = 10;
        StrInt n = Header.Find(CMaaStringCrLf);
        if  (n > 0)
        {
            Header = Header.Mid(n + 2);
        }
    }

    bool bEndFileSent = false;

    if  (m_IsHttp100ContinueHeaderPreceeded)
    {
        ManualStatus.Empty();
    }
    CMaaString rs(getenv("REQUEST_SCHEME") ? getenv("REQUEST_SCHEME") : "");
    CMaaString txt;
    if  (t > 0 && t <= tims)
    {
        txt.Format(
            m_IsHttp100ContinueHeaderPreceeded ?
            "HTTP/1.1 304 Not Modified\r\n"
            // "Connection: close\r\n"
            // "Connection: keep-alive\r\n"
            "\r\n"
            :
            "Status: 304 Not Modified\r\n"
            // "Connection: close\r\n"
            // "Connection: keep-alive\r\n"
            "\r\n"
           );
            try
        {
            if  (m_SubstOut)
            {
                s_Output += txt;
            }
            else
            {
#ifdef FAST_CGI_SUPP
                if (m_pFastCgiRequest)
                {
                    SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
                }
                else
#endif
                {
                    m_fStdOut.Write(txt);
                }
            }
        }
        catch(...)
        {
            Ret = 1;
        }
        return Ret;
    }
    else if ((Start == 0 && End == len - 1))// || ErrorText.IsNotEmpty())
    {
        m_ReplySentStatus = 7;
        txt.Format(
            m_IsHttp100ContinueHeaderPreceeded ?
            "%S"
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %S\r\n"
            "%s"
            "%S"
            "%S"
            "Content-Length: %D\r\n"
            "%S"
            "%S"
            "\r\n"
            :
            //"Status: 200 OK\r\n"
            "%S"
            "Content-Type: %S\r\n"
            "%s"
            "%S"
            "%S"
            "Content-Length: %D\r\n"
            "%S"
            "%S"
            "\r\n",
            &ManualStatus,
            &ContentType, rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER, &FileName, &LastModified,
            len, //Start, End, len,
            &Connection, &Header);
            bEndFileSent = true;
        }
        else if (Start >= 0 && End >= Start && End < len)
    {
        if  (End == len - 1)
        {
            bEndFileSent = true;
        }
        m_ReplySentStatus = 2 | (Start == 0 ? 1 : 0) | (End == len - 1 ? 4 : 0);
        _qword _len = End + 1 - Start;
        Data = Data.RefMid((StrInt)Start, (StrInt)_len);
        txt.Format(
            m_IsHttp100ContinueHeaderPreceeded ?
            "HTTP/1.1 206 Partial content\r\n"
            "Content-Type: %S\r\n"
            "%s"
            "%S"
            "%S"
            "Content-Length: %D\r\n"
            "Content-Range: bytes %D-%D/%D\r\n"
            "%S"
            "%S"
            "\r\n"
            :
            "Status: 206 Partial content\r\n"
            //"Content-Type: application/octet-stream\r\n"
            "Content-Type: %S\r\n"
            "%s"
            "%S"
            "%S"
            "Content-Length: %D\r\n"
            "Content-Range: bytes %D-%D/%D\r\n"
//            "Cache-control: no-cache\r\n"
            "%S"
            "%S"
            "\r\n",
            &ContentType, rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER, &FileName, &LastModified,
            _len, Start, End, len,
            &Connection, &Header);
        }
        else
    {
        m_ReplySentStatus = 0x100000;
        ContentType = "text/plain";
        t = time(nullptr);
        Data.Format("unknown error at point 1");
        txt.Format(
            m_IsHttp100ContinueHeaderPreceeded ?
            //"HTTP/1.1 200 OK\r\n"
            "HTTP/1.1 416 Requested Range Not Satisfiable\r\n"
            "Content-Type: %S\r\n"
            "%s"
            //"Last-Modified: %s\r\n"
            "Content-Length: %D\r\n"
            //"Content-Range: bytes %D-%D/%D\r\n"
            "Cache-control: no-cache\r\n"
            "%S"
            "\r\n"
            :
            "Status: 416\r\n"
            "Content-Type: %S\r\n"
            "%s"
            //"Last-Modified: %s\r\n"
            "Content-Length: %D\r\n"
            //"Content-Range: bytes %D-%D/%D\r\n"
            "Cache-control: no-cache\r\n"
            "%S"
            "\r\n",
            &ContentType,
            rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER,
            // &FileName,
            //(const char *)GetDateWebFormat(t),
            (_qword)Data.Length(),
            &Connection);
        }
        try
    {
        if  (m_SubstOut)
        {
            s_Output += txt;
            s_Output += Data;
        }
        else
        {
#ifdef FAST_CGI_SUPP
            if (m_pFastCgiRequest)
            {
                SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
                FCGX_PutStr(Data, Data.Length(), m_pFastCgiRequest->out);
            }
            else
#endif
            {
                m_fStdOut.Write(txt);
                m_fStdOut.Write(Data);
            }
        }
        m_EndFileSent = m_Method != "HEAD" && bEndFileSent;
    }
    catch(...)
    {
        Ret = 1; //??...
    }
    /*
{
    for (int i = 0; i < 1000; i++)
    {
	CMaaString fn;
	fn.Format("test_dump_SendReply()_%04d.txt", i);
	CMaaFile f(fn, CMaaFile::eWCDN_SrSw, eNoExcept);
	if (f.IsOpen())
	{
	    f.Write(txt);
	    f.Write(Data);
	    break;
	}
    }
}
*/
    return 0;
}

int CCGIHelper::SendReply(CMaaFile f, const CMaaString &Header, const CMaaString &FileName, time_t t, const CMaaString &ContentType, _qword fStart, _qword fEnd) //, CMaaFile fStdOut)
{
    return SendReply(f, Header, FileName, t, ContentType, fStart, fEnd, false); // , fStdOut);
}

int CCGIHelper::SendReply(CMaaFile f, CMaaString Header, CMaaString FileName, time_t t, CMaaString ContentType, _qword fStart, _qword fEnd, bool bInline) //, CMaaFile fStdOut)
{
    if (!IsSubsted() && m_n100Count > 0)
    {
        if (Header.IsLeft("HTTP/1.", 7))
        {
        }
        else if (Header.IsLeft("Status: ", 8))
        {
            int pos = 8;
            while(Header[pos] == ' ')
            {
                pos++;
            }
            Header = CMaaString("HTTP/1.1 ") + Header.Mid(pos);
        }
        else
        {
            Header = CMaaString("HTTP/1.1 200 OK\r\n") + Header;
        }
    }
    m_EndFileSent = false;
    m_ReplySentStatus = 0;

    int Ret = 0;

    if (m_Send500)
    {
        CMaaString txt = "Status: 500 Error\r\nContent-type: text/plain\r\nCache-control: no-cache\r\n\r\n";
#ifdef FAST_CGI_SUPP
        if (m_pFastCgiRequest)
        {
            SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
        }
        else
#endif
        {
            m_fStdOut.Write(txt);
        }
        return 0;
    }

    CMaaString //ContentType,
    ErrorText;

    time_t tims = 0;
    //    if   (0)
    if  (!m_SubstOut) // can be commented : subst is not intened for file output
    {
        const char * strIms = getenv("HTTP_IF_MODIFIED_SINCE");
        if  (strIms)// && txt.Find("no-cache") < 0 && Pragma.Find("no-cache") < 0)
        {
            if  (ConvertFromWebDateTime(CMaaStringRO(strIms), &tims))
            {
                // time is got
            }
        }
    }

    CMaaString CacheControl, Pragma;
    {
        const char * p = getenv("HTTP_CACHE_CONTROL");
        CacheControl = p ? p : "";
    }
    {
        const char * p = getenv("HTTP_PRAGMA");
        Pragma = p ? p : "";
    }
    if  (CacheControl.FindCi("no-cache") >= 0)
    {
        CacheControl = "Cache-Control: no-cache\r\n";
        Pragma.Empty();
    }
    else
    {
        CacheControl.Empty();
        if  (Pragma.FindCi("no-cache") >= 0)
        {
            CacheControl = "Pragma: no-cache\r\n";
        }
        Pragma.Empty();
    }
    CMaaString cc = Header.CgiGetValue("Cache-Control: ");
    CMaaString cp = Header.CgiGetValue("Pragma: ");
    CMaaString Connection;
    if  (Header.CgiGetValue("Connection: ").IsEmpty())
    {
        const char * p = getenv("HTTP_CONNECTION");
        if  (!p || *p == '\0')
        {
            Connection = "Connection: close\r\n";
            //Connection = "Connection: keep-alive\r\n";
        }
    }
    if  (CacheControl.IsNotEmpty())
    {
        if  (cc.IsNotEmpty() || cp.Find("no-cache") >= 0)
        {
        }
        else
        {
            Header = CacheControl + Header;
        }
    }

    Header = CMaaString("Accept-Ranges: bytes\r\n") + Header;

    _qword Start = 0, End = -1;
    Start = m_Start, End = m_End;
    /*
    {
	const char * p = getenv("HTTP_RANGE");
	if  (p)
	{
	    CMaaString txt(p);
	    if	(txt.IsLeftCi("bytes=", 6))
	    {
		txt = txt.Mid(6);
		int StopPos = -1;
		if (txt[0] != '-')
		{
		    mysscanf64(txt, &Start, -1, &StopPos, "-");
		}
		else
		{
		    StopPos = 0;
		}
		if (StopPos >= 0)
		{
		    mysscanf64(StopPos + 1 + txt, &End);
		}
	    }
	}
    }
    */

    CMaaString Method = m_Method;

    if  (Method == "HEAD")
    {
        Start = 0;
        End = -1;
    }

    /*
    if   (ErrorText.IsNotEmpty())
    {
        Data = ErrorText;
        ContentType = "text/plain";
        FileName.Empty();
        //Start = 0;
        //End = Data.Length() - 1;
        Start = 0;
        End = -1;
    }
    */

    fEnd = fEnd < 0 ? f.Length() : fEnd;
    if  (fStart > fEnd || fStart < 0)
    {
        fStart = fEnd = 0;
    }

    _qword len = fEnd - fStart; // Data.Length();
    //_qword pos = 0;

    f.Seek(fStart);

    if  (Method != "HEAD" && Method != "GET" && Method != "POST")
    {
        CMaaString txt;
        txt.Format(
            "Status: 405 Method Not Allowed\r\n"
            "Content-Length: 0\r\n"
            // "Connection: close\r\n"
            // "Connection: keep-alive\r\n"
            "\r\n"
           );
        if  (m_SubstOut)
        {
            s_Output += txt;
        }
        else
        {
#ifdef FAST_CGI_SUPP
            if (m_pFastCgiRequest)
            {
                SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
            }
            else
#endif
            {
                m_fStdOut.Write(txt);
            }
        }
        return Ret;
        //return 1;
    }

    if  (End == -1)
    {
        End = len - 1;
    }

    if  (End == len - 1)
    {
        // bStat = true;
    }

    if  (FileName.IsNotEmpty())
    {
        if  (ContentType.IsEmpty())
        {
            ContentType = GetContentTypeByFileName(FileName, "application/octet-stream");
        }
        if  (!bInline)
        {
            FileName.Format("Content-Disposition: attachment; filename=\"%S\"\r\n", &FileName);
        }
        else
        {
            // https://tools.ietf.org/html/rfc1806
            FileName.Format("Content-Disposition: inline; filename=\"%S\"\r\n", &FileName);
        }
    }
    if  (ContentType.IsEmpty())
    {
        ContentType = "application/octet-stream";
    }

    CMaaString LastModified;
    if  (t && t != (time_t)-1)
    {
        CMaaString dt = GetDateWebFormat(t);
        LastModified.Format("Last-Modified: %S\r\n", &dt);
    }

    /*{
    CMaaFile log("/var/ram/maa/log1.txt", CMaaFile::eWC_SrSw, eNoExcept);
        log.fprintf("Start = %D, End = %D, len = %D\n", (_qword)Start, (_qword)End, (_qword)len);
        log.fprintf("fStart = %D, fEnd = %D\n", (_qword)fStart, (_qword)fEnd);
    }*/

    CMaaString rs(getenv("REQUEST_SCHEME") ? getenv("REQUEST_SCHEME") : "");
    CMaaString txt, Data;
    if  (t > 0 && t <= tims)
    {
        txt.Format(
            "Status: 304 Not Modified\r\n"
            // "Connection: close\r\n"
            // "Connection: keep-alive\r\n"
            "\r\n"
           );
        if  (m_SubstOut)
        {
            s_Output += txt;
        }
        else
        {
#ifdef FAST_CGI_SUPP
            if (m_pFastCgiRequest)
            {
                SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
            }
            else
#endif
            {
                m_fStdOut.Write(txt);
            }
        }
        return Ret;
    }
    else if ((Start == 0 && End == len - 1))// || ErrorText.IsNotEmpty())
    {
        m_ReplySentStatus = 7;

        if  (End >= 0)
        {
            txt.Format(
                //"Status: 200 OK\r\n"
                //"Content-Type: application/octet-stream\r\n"
                "Content-Type: %S\r\n"
                "%s"
                "%S"
                "%S"
                "Content-Length: %D\r\n"
//                "Content-Range: bytes %D-%D/%D\r\n"
    //            "Cache-control: no-cache\r\n"
                "%S"
                "%S"
                "\r\n",
                &ContentType, rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER, &FileName, &LastModified,
                len, //Start, End, len,
                &Connection, &Header);
        }
        else
        {
            txt.Format(
                //"Status: 200 OK\r\n"
                //"Content-Type: application/octet-stream\r\n"
                "Content-Type: %S\r\n"
                "%s"
                "%S"
                "%S"
                "Content-Length: %D\r\n"
    //            "Content-Range: bytes %D-%D/%D\r\n"
    //            "Cache-control: no-cache\r\n"
                "%S"
                "%S"
                "\r\n",
                &ContentType, rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER, &FileName, &LastModified,
                len, //Start, End, len,
                &Connection, &Header);
        }
    }
    else if (Start >= 0 && End >= Start && End < len)
    {
        m_ReplySentStatus = 2 | (Start == 0 ? 1 : 0) | (End == len - 1 ? 4 : 0);

        f.Seek(fStart + Start);
        _qword _len = End + 1 - Start;
        //Data = Data.RefMid(Start, (int)_len);
        txt.Format(
            "Status: 206 Partial content\r\n"
            //"Content-Type: application/octet-stream\r\n"
            "Content-Type: %S\r\n"
            "%s"
            "%S"
            "%S"
            "Content-Length: %D\r\n"
            "Content-Range: bytes %D-%D/%D\r\n"
//            "Cache-control: no-cache\r\n"
            "%S"
            "%S"
            "\r\n",
            &ContentType, rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER, &FileName, &LastModified,
            _len, Start, End, len,
            &Connection, &Header);
        len = _len;
    }
    else
    {
        m_ReplySentStatus = 0x100000;
        ContentType = "text/plain";
        t = time(nullptr);
        Data.Format("unknown error at point 1");

        ContentType = "text/html";
        Data =
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
        "<html><head>\n"
        "<title>416 Requested Range Not Satisfiable</title>\n"
        "</head><body>\n"
        "<h1>Requested Range Not Satisfiable</h1>\n"
        "<p>None of the range-specifier values in the Range\n"
        "request-header field overlap the current extent\n"
        "of the selected resource.</p>\n"
        "<hr>\n"
        "From ToolsLib CCGIHelper::SendReply(CMaaFile f, ...)\n"
        "</body></html>\n";
        txt.Format(
            "Status: 416\r\n"
            "Content-Type: %S\r\n"
            "%s"
            //"Last-Modified: %s\r\n"
            "Content-Length: %D\r\n"
            //"Content-Range: bytes %D-%D/%D\r\n"
            "Cache-control: no-cache\r\n"
            "%S"
            "\r\n",
            &ContentType,
            rs == "https" ? szHTTPS_ADD_HEADER : szHTTP_ADD_HEADER,
            // &FileName,
            //(const char *)GetDateWebFormat(t),
            (_qword)Data.Length(),
            &Connection);
        len = 0;
    }
    if  (Method == "HEAD")
    {
        m_ReplySentStatus |= 0x10;
    }
#ifdef FAST_CGI_SUPP
    if (m_pFastCgiRequest)
    {
        SendFCgiHeaderEx(txt); //FCGX_PutStr(txt, txt.Length(), m_pFastCgiRequest->out);
        FCGX_PutStr(Data, Data.Length(), m_pFastCgiRequest->out);
    }
    else
#endif
    {
        m_fStdOut.Write(txt);
        m_fStdOut.Write(Data);
    }
    if  (!len && f.Length() == 0)
    {
        m_EndFileSent = m_Method != "HEAD" && true;
    }
    while(len > 0)
    {
        char Buffer[100 * 1024];
        _dword xx = len < (_qword)sizeof(Buffer) ? (_dword)len : (_dword)sizeof(Buffer);
        _dword x = f.Read(Buffer, xx);
        if  (x == 0)
        {
            break;
        }
        if  (x == xx)
        {
        }
        _dword nn;
        if  (m_SubstOut)
        {
            s_Output += CMaaString(Buffer, x);
            nn = (_dword)x;
        }
        else
        {
#ifdef FAST_CGI_SUPP
            if (m_pFastCgiRequest)
            {
                nn = FCGX_PutStr(Buffer, x, m_pFastCgiRequest->out);
            }
            else
#endif
            {
                nn = m_fStdOut.Write(Buffer, x);
            }
        }
        if  (nn == 0)
        {
            break;
        }
        len -= nn;
        if  (!len && f.GetCurPos() == f.Length())
        {
            m_EndFileSent = m_Method != "HEAD" && true;
        }
    }
    return len ? 1 : 0;
}

int CCGIHelper::GetReplySentStatus() const noexcept
{
    return m_ReplySentStatus;
}

bool CCGIHelper::IsEndFileSent() const noexcept
{
    return m_EndFileSent;
}

int CCGIHelper::SendIndependentReply(CMaaString /*Data*/, CMaaString /*Header*/) // with no if-mofified-since, partial responce and so on conversions
{
    return 1; // error
}

int CCGIHelper::SendIndependentReply(CMaaFile /*f*/, CMaaString /*Header*/)//, _qword Start = 0, _qword End = -1);
{
    return 1; // error
}
int CCGIHelper::Send100Continue() //(CMaaFile fStdOut)
{
    int r = 0;
    if   (/*!IsSubsted() &&*/ m_Last100Cont && time(nullptr) - m_Last100Cont >= 120)
    {
        CMaaString s = CMaaStringRO(
            m_n100Count == 0 ?
                "Status: 100 Continue\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 0\r\n"
                "Connection: Keep-Alive\r\n"
                "Cache-control: no-cache\r\n"
                "Cache-control: no-transform\r\n"
                "\r\n"
                    :
                "HTTP/1.1 100 Continue\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: 0\r\n"
                "Connection: Keep-Alive\r\n"
                "Cache-control: no-cache\r\n"
                "Cache-control: no-transform\r\n"
                "\r\n"
            );
        if (m_fStdOut.Write(s) == (_dword)s.Length() && s.IsNotEmpty())
        {
            m_n100Count++;
        }
        else if (s.IsNotEmpty())
        {
            r = 1;
        }
        //m_fStdOut.Flush(); // error: fdatasync() Error code=22 (Invalid argument)
        m_Last100Cont = time(nullptr);
    }
    return r;

}
static const char* spBotsSubStr[9] =
{
    "bot", "spider", "stackrambler", "yandex/", "yahoo! slurp", "webmoney advisor", "smartlinksaddon", "internetseer.com", "crawler"
};
static CMaaString::sFindNCalcData<9> sfBots(spBotsSubStr);
bool CCGIHelper::ItIsABot(const char * p)
{
    CMaaString tmp(p ? p : (g_imp ? g_imp->getenv("HTTP_USER_AGENT") : ::getenv("HTTP_USER_AGENT")));
    const bool bItIsABot = tmp.FindNCi<9>(0, spBotsSubStr, sfBots.SubStrLen) >= 0
         || tmp.IsLeftCi("curl/", (StrInt)5) || tmp.IsLeftCi("java/", (StrInt)5) || tmp.IsLeftCi("python-", (StrInt)7)
         // || tmp.IsLeftCi("python-urllib/", (StrInt)14) || tmp.IsLeftCi("python-requests/", (StrInt)16)
        ;
    //CMaaFile log("/var/ram/maa/uag.txt", CMaaFile::eAC_SrSw, eNoExcept);
    //log.fprintf("%s - %S\n", bItIsASearchMachine ? "true" : "false", &tmp0);
    return bItIsABot;
}
thread_local_ bool CCGIHelper::s_SubstIn = false;
thread_local_ bool CCGIHelper::s_SubstOut = false;
thread_local_ CMaaString CCGIHelper::s_Method, CCGIHelper::s_Fn, CCGIHelper::s_Qs, CCGIHelper::s_Output;

void CCGIHelper::SetSubst(bool bIn, bool bOut, const CMaaString &Qs, CMaaString Fn, const CMaaString &Method)
{
    /*
            putenv("REQUEST_METHOD=GET");
            putenv("QUERY_STRING=".$qs);
            putenv("REQUEST_URI=".$ru);
            putenv("SCRIPT_NAME=".substr($fn, 1));
            putenv("SCRIPT_FILENAME=".getcwd().substr($fn, 1));
*/
    s_SubstIn = bIn;
    s_SubstOut = bOut;
    s_Method = Method;
    s_Qs = Qs;
    s_Fn = Fn;
    if  (s_SubstIn)
    {
        //        CMaaFile ll(s_SubstIn ? "/var/ram/maa/ssi.txt" : nullptr, CMaaFile::eAC_SrSw, eNoExcept);
        CMaaString e;
//#ifdef _WIN32
//#define setenv(a,b,c) { CMaaString aa = Utf8ToUnicode(a); CMaaString bb = Utf8ToUnicode(b); SetEnvironmentVariableW((_WC_*)(const char *)aa, (_WC_*)(const char *)bb); }
//#define unsetenv(a) { CMaaString aa = Utf8ToUnicode(a); SetEnvironmentVariableW((_WC_*)(const char *)aa, nullptr); }
//#endif
        setenv("REQUEST_METHOD", Method, 1);
        setenv("QUERY_STRING", Qs, 1);
        if  (Fn[0] == '/')
        {
            Fn = Fn.RefMid(1);
        }
        e.Format(Qs.IsNotEmpty() ? "/%S?%S" : "/%S", &Fn, &Qs);
        setenv("REQUEST_URI", e, 1);
        setenv("SCRIPT_NAME", Fn, 1);
        unsetenv("HTTP_RANGE");
    }
    if  (s_SubstOut)
    {
        s_Output.Empty();
    }
}

/*
static CMaaString CgiGetValue(const CMaaString& Hdr0, const CMaaString& Name0, int* pos, int* nextpos)
{
    CMaaString Hdr = CMaaStringCrLf + Hdr0;
    CMaaString Name = CMaaStringCrLf + Name0;

    int StartPos = -2;

    pos = pos ? pos : &StartPos;

    CMaaString Val;
    int n = (warning_int)Hdr.FindCi(*pos + 2, Name);
    int n2 = *pos + 2;
    if  (n >= 0)
    {
        if  (pos)
        {
            *pos = n >= 2 ? n - 2 : 0;// + 2;
        }
        int n3 = (warning_int)Hdr.Find(n + Name.Length(), CMaaStringCrLf);
        if  (n3 > 0)
        {
            n2 = n3;
            Val = Hdr.Mid(n + Name.Length(), n2 - n - Name.Length());

            int n0 = n2 + 2;
            while(Hdr[n0] == '\t' || Hdr[n0] == ' ')
            {
                int n3 = (warning_int)Hdr.Find(n0, CMaaStringCrLf);
                if  (n3 < 0)
                {
                    break;
                }
                Val.AddMidOf(Hdr, n0 + 1, n2 - n0 - 1);
                n2 = n3;
                n0 = n3 + 2;
            }

            int i;
            for (i = 0; i < Val.Length() && Val[i] == ' '; i++) ;
            Val = Val.Mid(i);
            for (i = (warning_int)Val.Length(); i > 0 && Val[i - 1] == ' '; i--) ;
            Val = Val.Left(i);
        }
    }
    if  (nextpos)
    {
        *nextpos = n2 - 2;
    }
    return Val;
}
*/

CCGIHelper::iterator::iterator()
:   m_h(nullptr)
{
}
CCGIHelper::iterator::iterator(const CCGIHelper &h, CMaaString Mask, bool bFilesOnly)
:   m_h(&h),
    m_Mask(Mask),
    m_bFilesOnly(bFilesOnly)
{
    Next(true);
}
bool CCGIHelper::iterator::Next(bool bInit)
{
    if  (!IsValid())
    {
        return false;
    }
    if  (bInit)
    {
        m_p2 = m_h->m_Attr2List.LookAtFront();
        m_ParamNum = 0;
    }
    while(1)
    {
        if  (m_h->m_Attr2List.LookAtFront())
        {
            if  (!m_p2)
            {
                break;
            }
            m_p = m_p2; m_p2 = m_h->m_Attr2List.Next(m_p2);
        }
        else
        {
            m_p = &m_p_temp;
            if  (!m_h->GetParamByNum(m_ParamNum++, m_p->m_Name, m_p->m_ContentData))
            {
                break;
            }
        }
        if  (m_bFilesOnly && m_p->m_FileName.IsEmpty())
        {
            continue;
        }
        if  (!m_p->m_Name.IsMatchMask(m_Mask))
        {
            continue;
        }
        return true;
    }
    m_p = nullptr;
    return false;
}
CCGIHelper::iterator::iterator(const CCGIHelper::iterator &That) noexcept
:   m_h(nullptr)
{
    *this = That;
}
CCGIHelper::iterator::~iterator()
{
}
CCGIHelper::iterator & CCGIHelper::iterator::operator=(const CCGIHelper::iterator &that) noexcept
{
    iterator &That = (iterator &)that;
    if  (this != &That)
    {
        m_h = That.m_h;
        m_Mask = That.m_Mask;
        m_p = That.m_p;
        m_p2 = That.m_p2;
        m_bFilesOnly = That.m_bFilesOnly;
        if  (m_p == &That.m_p_temp)
        {
            m_p = &m_p_temp;
        }
        m_p_temp.m_FileName = That.m_p_temp.m_FileName;
        m_p_temp.m_Name = That.m_p_temp.m_Name;
        m_p_temp.m_ContentData = That.m_p_temp.m_ContentData;
        m_ParamNum = That.m_ParamNum;
    }
    return *this;
}
bool CCGIHelper::iterator::IsValid() const noexcept
{
    return m_h != nullptr;
}
CCGIHelper::iterator::operator bool () const noexcept
{
    return IsValid() && m_p != nullptr;
}
CMaaString CCGIHelper::iterator::GetFile(CMaaString * pFileName, CMaaString * pName, bool bFsAllowed) const
{
    if  (!IsValid())
    {
        throw 1;
    }
    if  (!m_p)
    {
        throw 2;
    }
    if  (pFileName)
    {
        *pFileName = m_p->m_FileName;
        if  (bFsAllowed)
        {
            *pFileName = CMaaFile::GetFileNameAny(pFileName->ToFsAllowed());
        }
    }
    if  (pName)
    {
        *pName = m_p->m_Name;
    }
    return m_p->m_ContentData;
}
CCGIHelper::iterator CCGIHelper::iterator::operator++(int)
{
    CCGIHelper::iterator tmp(*this);
    ++(*this);
    return tmp;
}
CCGIHelper::iterator CCGIHelper::iterator::operator++()
{
    if  (m_h)
    {
        Next();
    }
    return *this;
}

DEF_ALLOCATOR_CMaaUnivHash(int, CCGIHelper::AttrVal*)
DEF_ALLOCATOR_CMaaUnivHash(int, CMaaString)
