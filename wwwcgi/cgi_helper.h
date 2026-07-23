
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

char * getenv8(const char * name);

#ifdef _WIN32
CMaaString my_getenv(const CMaaString& name); // returns utf8 env variable or nullptr
int my_setenv(const CMaaString& name, const CMaaString& value, int overwrite);
int my_unsetenv(const CMaaString& name);
#define getenv my_getenv
#define setenv my_setenv
#define unsetenv my_unsetenv
#endif

extern bool gAllow_PATH_INFO;
#ifdef FAST_CGI_SUPP
extern CMaaRWLockWp gFastCgiRWLock;
#endif

class CCGIHelper
{
public:
    CMaaString getenv(const CMaaString& name);
    void RLock() noexcept
    {
#ifdef FAST_CGI_SUPP
        if (m_bFastCgiRLockable && !m_bFastCgiRLocked)
        {
            gFastCgiRWLock.RLock(false);
            m_bFastCgiRLocked = true;
        }
#endif
    }
    void RWUnLock() noexcept
    {
#ifdef FAST_CGI_SUPP
        if (m_bFastCgiRLockable && m_bFastCgiRLocked)
        {
            gFastCgiRWLock.RUnLock();
            m_bFastCgiRLocked = false;
        }
        if (m_bFastCgiRLockable && m_bFastCgiWLocked)
        {
            gFastCgiRWLock.WUnLock();
            m_bFastCgiWLocked = false;
        }
#endif
    }
    void R2WLock() noexcept
    {
#ifdef FAST_CGI_SUPP
        if (m_bFastCgiRLockable && m_bFastCgiRLocked && !m_bFastCgiWLocked)
        {
            gFastCgiRWLock.R2WLock();
            m_bFastCgiRLocked = false;
            m_bFastCgiWLocked = true;
        }
#endif
    }
    void W2RLock() noexcept
    {
#ifdef FAST_CGI_SUPP
        if (m_bFastCgiRLockable && m_bFastCgiWLocked && !m_bFastCgiRLocked)
        {
            gFastCgiRWLock.W2RLock();
            m_bFastCgiWLocked = false;
            m_bFastCgiRLocked = true;
        }
#endif
    }
protected:
#ifdef FAST_CGI_SUPP
    bool m_bFastCgiRLockable = false, m_bFastCgiRLocked = false, m_bFastCgiWLocked = false, m_bFastCgiPadding = false;
    FCGX_Request * m_pFastCgiRequest;
#endif
    CMaaUnivHash<CMaaString, CMaaString> m_hSubstCgiParamOverride;
    CMaaUnivHash<CMaaString, CMaaString>* m_phCgiParamOverride = nullptr;
    CMaaString m_ReinitQS;
    bool m_bReinitQS = false;
public:
    CMaaFile m_fStdOut; // = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true) for CGI, = {} for FastCGI
protected:
    int m_Error;
    bool m_SubstOut;
    CCGIHelper * m_subst_imp;
    CMaaString m_envMethod, m_envContentType, m_envContentLength, m_envQS, m_envRange;
    CMaaString m_Method,    m_ContentType,    m_ContentLength,    m_QS,    m_Range;
    // "application/x-www-form-urlencoded"
public:
    CMaaString m_PostData;
    int m_cl, m_rl;
    //int m_IsHttp100ContinueHeaderPreceeded;

    struct AttrVal : public CMaaSLink
    {
        CMaaString m_Attr;
        CMaaString m_Val;

        AttrVal() noexcept;
        AttrVal(const AttrVal &That) noexcept;
        ~AttrVal();
    };

    struct Attr2Val : public CMaaSLink
    {
        int m_nLines; // starting from -1
        CMaaUnivHash<int, CMaaString> m_hLinesAttr;
        CMaaString m_ContentDisposition, m_ContentType;
        CMaaString m_ContentData;
        //
        CMaaString m_Name, m_FileName;

        Attr2Val();
        Attr2Val(const AttrVal &That);
        ~Attr2Val();
    };

    _qword m_Start, m_End; // 0,-1

protected:
    // application/x-www-form-urlencoded and ""
    int m_AttrCount;
    CMaaSList2AD<AttrVal> m_srcAttrList, m_AttrList;
    int m_IsHttp100ContinueHeaderPreceeded;
    CMaaUnivHash<CMaaString, CMaaString> m_hsrcAttr, m_hAttr;
    CMaaUnivHash<int, AttrVal*> m_hNum2AttrMap;

    int m_ReplySentStatus;
    bool m_EndFileSent;
    bool m_Send500 = false;
    //int m_n100Count = 0;
    time_t m_Last100Cont = 0;

public:
    // multipart/form-data
    CMaaSList2AD<Attr2Val> m_Attr2List;
protected:
    int m_n100Count = 0;
    // attributes from m_ContentDisposition
    // m_hAttr2 === m_hsrcAttr2 at the current version
    CMaaUnivHash<CMaaString, CMaaString> m_hsrcAttr2, m_hAttr2;

    CMaaString m_ProgressFn;

    void Initialize(_qword MaxContentLength = -1, 
#ifdef FAST_CGI_SUPP
    FCGX_Request *
#else
    void *
#endif
     pFastCgiRequest = nullptr, /*CMaaUnivHash<CMaaString, CMaaString> *phCgiParamOverride,*/ const CMaaString &ProgressFn = gsCMaaStringZ, CMaaString ProgressFmt = CMaaStringZ);

#ifdef FAST_CGI_SUPP
    int SendFCgiHeaderEx(CMaaString txt);
#endif

public:
    CMaaFile m_fProgress;
    static thread_local_ CCGIHelper * g_imp;
    CCGIHelper(_qword MaxContentLength = -1, 
#ifdef FAST_CGI_SUPP
    FCGX_Request *
#else
    void *
#endif
         pFastCgiRequest = nullptr,
     CMaaUnivHash<CMaaString, CMaaString> *phCgiParamOverride = nullptr,
     const CMaaString &ProgressFn = gsCMaaStringZ, const CMaaString &ProgressFmt = gsCMaaStringZ);
    ~CCGIHelper();
    int GetErrorCode() const noexcept { return m_Error; }
    void ReinitializeGetQS(bool bSetNewQS, const CMaaString& NewQS = CMaaStringZ);
    int GetParam(const CMaaString &Name, int defvalue = 0, bool *isexists = nullptr) const noexcept;
    int GetParam(const CMaaString &Name, CMaaString *val = nullptr) const noexcept;
    bool GetParamByNum(int num, CMaaString &Attr, CMaaString &Val) const noexcept;
    int SetParam(const CMaaString &Name, const CMaaString &Val); // for GetParam() only
    int SendReply(CMaaString Data, CMaaString ContentType, CMaaString FileName, CMaaString Header, const CMaaString& ErrorText, time_t t, bool bInline); // , CMaaFile fStdOut = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true));
    int SendReply(const CMaaString& Data, const CMaaString& ContentType = CMaaStringTextPlain, const CMaaString& FileName = gsCMaaStringZ, const CMaaString& Header = gsCMaaStringZ, const CMaaString& ErrorText = gsCMaaStringZ, time_t t = time(nullptr)); // , CMaaFile fStdOut = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true));
    //int SendReply(CMaaString Data, CMaaString Header = CMaaStringZ);
    int SendReply(CMaaFile f, CMaaString Header, CMaaString FileName, time_t t, CMaaString ContentType, _qword Start, _qword End, bool bInline); // , CMaaFile fStdOut = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true));
    int SendReply(CMaaFile f, const CMaaString& Header = gsCMaaStringZ, const CMaaString& FileName = gsCMaaStringZ, time_t t = time(nullptr), const CMaaString& ContentType = gsCMaaStringZ, _qword Start = 0, _qword End = -1); // , CMaaFile fStdOut = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true));
    int SendIndependentReply(CMaaString Data, CMaaString Header = CMaaStringZ); // with no if-mofified-since, partial responce and so on conversions
    int SendIndependentReply(CMaaFile f, CMaaString Header = CMaaStringZ);//, _qword Start = 0, _qword End = -1);
    int Send100Continue(); // (CMaaFile fStdOut = CMaaFile(CMaaFileStdout, CMaaFile::eW_SrSw, true));
    static CMaaString GetContentTypeByFileName(const CMaaString& FileName, const CMaaString& DefaultContentType = CMaaStringApplicationOctet_stream); // "application/octet-stream"
    static CMaaString GetContentTypeByFileExtention(const CMaaString &Ext, const CMaaString &DefaultContentType = CMaaStringApplicationOctet_stream);
    static CMaaString GetContentTypeByExtention(CMaaString Ext, const CMaaString &DefaultContentType = CMaaStringApplicationOctet_stream);
    int GetReplySentStatus() const noexcept;
    bool IsEndFileSent() const noexcept;
    static bool ItIsABot(const char * p = nullptr);
    static void SetSubst(bool bIn, bool bOut, const CMaaString& Qs = gsCMaaStringZ, CMaaString Fn = "script.cgi", const CMaaString& Method = CMaaStringGET); // "GET"
    static void ResetSubst() noexcept
    {
        s_SubstIn = s_SubstOut = false;
    }
    static bool IsSubsted() noexcept
    {
        return s_SubstIn || s_SubstOut;
    }
protected:
    static thread_local_ bool s_SubstIn, s_SubstOut;
    static thread_local_ CMaaString s_Method, s_Fn, s_Qs;
public:
    static thread_local_ CMaaString s_Output;

    CMaaString GetMethod() const noexcept;
    static const CMaaUnivHash<CMaaString, CMaaString>* MakeMimeTable();

public:

    class iterator
    {
        //friend class CCGIHelper;
        const CCGIHelper * m_h;
        CMaaString m_Mask;
        bool m_bFilesOnly;
        CCGIHelper::Attr2Val * m_p, * m_p2, m_p_temp;
        int m_ParamNum;

    public:
        iterator();
        iterator(const CCGIHelper &h, CMaaString Mask = CMaaStringAsterisk, bool bFilesOnly = true); // "*"
        bool Next(bool bInit = false);
        iterator(const iterator &That) noexcept;
        ~iterator();
        iterator & operator=(const iterator &that) noexcept;
        bool IsValid() const noexcept;
        operator bool () const noexcept;
        CMaaString GetFile(CMaaString * pFileName = nullptr, CMaaString * pName = nullptr, bool bFsAllowed = true) const;
        iterator operator++(int);
        iterator operator++();
    };
    iterator it(CMaaString Mask = CMaaStringAsterisk, bool bFilesOnly = true) // "*"
    {
        return iterator (*this, Mask, bFilesOnly);
    }
};

typedef CCGIHelper * PCCGIHelper;
