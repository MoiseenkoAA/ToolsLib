
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Ìîèñååíêî Àíäðåé Àëåêñååâè÷)
 */

// ToolsLib/Errors.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains error handling classes.
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

#define static_constexpr_Imp static constexpr
//#define static_constexpr_Imp static

struct SubstMsgErr : public CMaaDLink
{
    XTOOError m_err;
    int m_nErr;
public:
    CMaaThreadIdType m_ThreadId;
    SubstMsgErr(XTOOError &err, int n, CMaaThreadIdType ThreadId) noexcept
    :   m_err(err),
        m_nErr(n),
        m_ThreadId(ThreadId)
    {
    }
};

static CMaaDList<SubstMsgErr> m_SubstMsgErrList;

static CMaaAtomicFastMutex0 sSubstMsgErrListLock;

bool AddSubstError(XTOOError &err, int n, bool bAnyThread) noexcept
{
    SubstMsgErr * p = nullptr;
    try
    {
        p = TL_NEW SubstMsgErr(err, n, bAnyThread ? CMaaInvalidThreadId() : CMaaGetCurrentThreadId());
    }
    catch(...)
    {
    }
    if  (p)
    {
        sSubstMsgErrListLock.LockM();
        m_SubstMsgErrList.AddAtFront(p);
        sSubstMsgErrListLock.UnLockM();
    }
    return p ? true : false;
}

bool RemoveSubstError(int n) noexcept
{
    SubstMsgErr * p;
    sSubstMsgErrListLock.LockM();
    const CMaaThreadIdType thr = CMaaGetCurrentThreadId();
    constexpr CMaaThreadIdType thr0 = CMaaInvalidThreadId();
    for (p = m_SubstMsgErrList.LookAtFront(); p; p = m_SubstMsgErrList.Next(p))
    {
        if  (p->m_nErr == n && (p->m_ThreadId == thr0 || CMaaThreadIdsEqual(p->m_ThreadId, thr)))
        {
            m_SubstMsgErrList.Release(p);
            delete p;
            break;
        }
    }
    sSubstMsgErrListLock.UnLockM();
    return p ? true : false;
}

bool FindSetSubstError(XTOOError &err, int n) noexcept
{
    bool bRet = false;
    const char * pMsg = nullptr;
    SubstMsgErr * p;
    sSubstMsgErrListLock.LockM();
    const CMaaThreadIdType thr = CMaaGetCurrentThreadId();
    constexpr CMaaThreadIdType thr0 = CMaaInvalidThreadId();
    for (p = m_SubstMsgErrList.LookAtFront(); p; p = m_SubstMsgErrList.Next(p))
    {
        if  (p->m_nErr == n && (p->m_ThreadId == thr0 || CMaaThreadIdsEqual(p->m_ThreadId, thr)))
        {
            if  ((pMsg = p->m_err.GetMsg()))
            {
                bRet = err.SetMsg(pMsg);
            }
            break;
        }
    }
    sSubstMsgErrListLock.UnLockM();
    return bRet;
}



struct sWsaErr
{
    int Code;
    const char * Msg;
};

#define ADD_WSA_ERR(a,b) {a, b}

//---------------------------------------------------------------------------
static constexpr sWsaErr WsaErrorMessages[] =
{
#if defined(__unix__) || defined(__BORLANDC__)
#ifdef __unix__
#include "unix/unixerrors.h"
#endif
#else
#include "win32\winerrors.h"
#endif

    ADD_WSA_ERR(CMaa_CONN_CLOSED_ERROR, "__CONNECTION_IS_CLOSED__"),
    ADD_WSA_ERR(-222, nullptr)
};
//---------------------------------------------------------------------------
const char* GetWsaErrorMessage(int Code) noexcept
{
#ifdef TOOLSLIB_KEEP_GLOBALS_IN_MEMORY
    // this object is longer exist and is not deleted
    static CMaaUnivHash<int, const char*> *ph = TL_NEW_ CMaaUnivHash<int, const char*>(sizeof(WsaErrorMessages) / sizeof(WsaErrorMessages[0]), [&]()
        {
            for (int i = 0; WsaErrorMessages[i].Msg; i++)
            {
                ph->Add(WsaErrorMessages[i].Code, WsaErrorMessages[i].Msg);
            }
        });
    return (*ph)[Code, nullptr];
#else
    // this object is shorter exist and is deleted at program exit
    static CMaaUnivHash<int, const char*> h(sizeof(WsaErrorMessages) / sizeof(WsaErrorMessages[0]), [&]()
        {
            for (int i = 0; WsaErrorMessages[i].Msg; i++)
            {
                h.Add(WsaErrorMessages[i].Code, WsaErrorMessages[i].Msg);
            }
        });
    return h[Code, nullptr];
    //CMaaUnivHash<int, const char*>* ph = &h;
#endif

    //const char* r = (*ph)[Code, nullptr];
    /*
    {
        for (int i = 0; WsaErrorMessages[i].Msg; i++)
        {
            if (Code == WsaErrorMessages[i].Code)
            {
                r = WsaErrorMessages[i].Msg;
                break;
            }
        }
    }
    */
    //return r;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_SHARED_ALLOCATOR
DEF_ALLOCATOR_CMaaUnivHash(int, const char*);
#endif
//---------------------------------------------------------------------------
// class XTOOMsg
//---------------------------------------------------------------------------
XTOOMsg::XTOOMsg(bool bIsThrow) noexcept
:   m_bIsThrow(bIsThrow)
{
}

XTOOMsg::~XTOOMsg()
{
}

// return description for this error
//CMaaString XTOOMsg::GetErrorMessage() const
//{
//     return CMaaString();
//}

// set error state and throw exception
void XTOOMsg::SetError()
{
    if  (m_bIsThrow)
    {
        Raise();
    }
}

//------------------------------------------------------------------------------
// class XTOOErrno
// MAA: 29.11.2002 renamed XTOOErrnoOld
//------------------------------------------------------------------------------
#ifndef __TOOLSLIB_REMOVE_OLD_CODE_SKELETONS
XTOOErrnoOld::XTOOErrnoOld(int iErrno, bool bIsThrow) noexcept
:   XTOOStatus (/*eSuccess*/e_NO_ERROR, bIsThrow),
    m_iErrno(iErrno)
{
}

bool XTOOErrnoOld::IsError() const noexcept
{
    return m_iErrno != 0;
}

int XTOOErrnoOld::Errno() const noexcept
{
    return m_iErrno;
}

CMaaString XTOOErrnoOld::ErrorMessage() const
{
#ifdef _WIN32
    CMaaString s;
    if  (!Status())
    {
        s.Format("Error %d (%s), File name is \"%s\"", m_iErrno, strerror(m_iErrno), (const char *)GetErrMessEx());
    }
    else
    {
        // branch is added 21.09.2005
        if  (XTOOErrnoOld::IsError())
        {
            s.Format("Error %d (%s), File name is \"%s\", %s", m_iErrno,
                 strerror(m_iErrno), (const char *)GetErrMessEx(), (const char *)XTOOStatus::ErrorMessage());
        }
        else
        {
            s.Format("File name is \"%s\", %s", (const char *)GetErrMessEx(), (const char *)XTOOStatus::ErrorMessage());
        }
    }
    return s;
    //return CMaaString ( strerror ( m_iErrno ) );
#else
    //  6.01.98 : strerror () "ªãè ¥â" >744 ¡ ©â ¢ DATA á¥£¬¥­â¥
    char msg[30];
    sprintf(msg, "Errno = %d", (int)m_iErrno);
    return CMaaString(msg);
#endif
}

void XTOOErrnoOld::Clear() noexcept
{
    m_iErrno = 0;
}

void XTOOErrnoOld::SetError(int iErrno)
{
    m_iErrno = iErrno;
    if  (XTOOErrnoOld::IsError())
    {
        XTOOMsg::SetError();
    }
}

//------------------------------------------------------------------------------
// class XTOOStatus
//------------------------------------------------------------------------------
XTOOStatus::XTOOStatus(int /*ETOOStatus*/ Status, bool bIsThrow) noexcept
:   XTOOMsg(bIsThrow),
    m_Status(Status)
{
}

bool XTOOStatus::IsError() const noexcept
{
    return m_Status != e_NO_ERROR/*eSuccess*/;
}

int/*XTOOStatus::ETOOStatus*/ XTOOStatus::Status() const noexcept
{
    return m_Status;
}

struct ErrDescr
{
    int Code;
    const char * Description;
};

#define REMOVE_OLD_MESSAGES
static const ErrDescr StatusErrorDescr[] =
{
    { e_NO_ERROR,            "There is no error" },
#ifdef e_INVALID_IMPLEMENTATION
    { e_INVALID_IMPLEMENTATION, "Function not implemented in some case (e_INVALID_IMPLEMENTATION)" },
#endif
    { -1, nullptr }
};
CMaaString XTOOStatus::ErrorMessage() const
{
    for (int i = 0; StatusErrorDescr[i].Description; i++)
    {
        if  (StatusErrorDescr[i].Code == m_Status)
        {
            CMaaString t;
            t.Format("Status = %d (%s)", (int)m_Status, StatusErrorDescr[i].Description);
            return t;
        }
    }
    CMaaString txt;
    txt.Format("Status = %d", (int)m_Status);
    return txt;
}

void XTOOStatus::Clear() noexcept
{
    m_Status = e_NO_ERROR/*eSuccess*/;
}

void XTOOStatus::SetError(int/*ETOOStatus*/ Status)
{
    m_Status = Status;
    if  (XTOOStatus::IsError())
    {
        XTOOMsg::SetError();
    }
}
#endif

//------------------------------------------------------------------------------
// class XTOOFile
//------------------------------------------------------------------------------
#ifndef __TOOLSLIB_REMOVE_OLD_CODE

XTOOFile::XTOOFile(bool bIsThrow) noexcept
:   XTOOErrnoOld(0, bIsThrow)
{
}

/*XTOOFile::XTOOFile(ETOOStatus Status, int iErrno, bool bIsThrow) noexcept
:    XTOOStatus(Status, bIsThrow),
     XTOOErrnoOld(iErrno, bIsThrow)
{
}*/

bool XTOOFile::IsError() const noexcept
{
    return XTOOStatus::IsError() || XTOOErrnoOld::IsError();
}

CMaaString XTOOFile::ErrorMessage() const
{
    return XTOOErrnoOld::ErrorMessage();
    /*
     if  (XTOOErrnoOld::IsError())
     {
          return XTOOErrnoOld::ErrorMessage();
     }
     return XTOOStatus::ErrorMessage();
     */
}

void XTOOFile::Clear() noexcept
{
    XTOOStatus::Clear();
    XTOOErrnoOld::Clear();
}

void XTOOFile::SetError(int/*ETOOStatus*/ Status, int iErrno)
{
    XTOOFile::Clear();

    // added 21.09.2005
    XTOOStatus::SetErrorSilent(Status);
    // next 2 lines was swapped 23.01.04
    XTOOErrnoOld::SetError(iErrno);
    XTOOStatus::SetError(Status);
}
#endif // __TOOLSLIB_REMOVE_OLD_CODE

/*
//------------------------------------------------------------------------------
// class XTOOInvalidImplementation
//------------------------------------------------------------------------------
XTOOInvalidImplementation::XTOOInvalidImplementation(bool bIsThrow)
:    m_bIsThrow(bIsThrow)
{
}

_bool XTOOFile::IsError() const noexcept
{
     return XTOOStatus::IsError() | XTOOErrnoOld::IsError();
}
*/

//==============================================================================
// MAA: Added 29.11.2002
//------------------------------------------------------------------------------
XTOOError::XTOOError(XTOOBaseErrorImp * pImp) noexcept
:   m_pImp(pImp)
{
}
XTOOError::~XTOOError()
{
    if  (m_pImp && !m_pImp->Release())
    {
        delete m_pImp;
    }
}
XTOOError::XTOOError(const XTOOError &That) noexcept
:   m_pImp(nullptr)
{
    CMaaSyncAssign(m_pImp, That.m_pImp, true, true, true, true);
}
XTOOError & XTOOError::operator=(const XTOOError &That) noexcept
{
    CMaaSyncAssign(m_pImp, That.m_pImp, true, true, true);
    return *this;
}
void XTOOError::Assign_locked(const XTOOError& That) noexcept
{
    CMaaSyncAssign(m_pImp, That.m_pImp, true, true, false);
}
void XTOOError::Clear() noexcept
{
    if (m_pImp)
    {
        XTOOError::XTOOBaseErrorImp * p = m_pImp->Clear(), * t;
        if (p)
        {
            t = m_pImp;
            m_pImp = p;
            if (!t->Release())
            {
                delete t;
            }
        }
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*
class XTOOErrMsgImp : public XTOOError::XTOOBaseErrorImp
{
     char * m_Msg;
public:
     XTOOErrMsgImp(const char * Msg = nullptr);
     ~XTOOErrMsgImp();
     void SetMsg(const char * Msg);
     void InsertInMsg(const char * Msg, char Space = 0);
     const char * GetMsg() noexcept;
     bool FindSetSubstError(int n) noexcept;
};
*/
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
DEF_ALLOCATOR(XTOOErrMsgImp)
#endif
//------------------------------------------------------------------------------
XTOOErrMsgImp::XTOOErrMsgImp() noexcept
:   m_Msg(nullptr)
{
}
//------------------------------------------------------------------------------
XTOOErrMsgImp::XTOOErrMsgImp(const char* Msg) noexcept
:   m_Msg(nullptr)
{
    SetMsg(Msg);
}
XTOOErrMsgImp::XTOOErrMsgImp(char * Msg, bool bMove) noexcept
:   m_Msg(nullptr)
{
    if (bMove)
    {
        MoveMsg(Msg);
    }
    else
    {
        SetMsg(Msg);
    }
}
/*
XTOOErrMsgImp::~XTOOErrMsgImp()
{
    if  (m_Msg)
    {
        delete [] m_Msg;
    }
}
*/
const char * XTOOErrMsgImp::GetMsg() noexcept
{
    return m_Msg;
}
void XTOOErrMsgImp::SetMsg(const char * Msg) noexcept
{
    if  (m_Msg)
    {
        delete [] m_Msg;
        m_Msg = nullptr;
    }
    if  (Msg)
    {
        try
        {
            const int len = int_strlen(Msg);
            m_Msg = TL_NEW char[len + 1];
            if  (m_Msg)
            {
                memcpy(m_Msg, Msg, len + 1);
            }
        }
        catch(...)
        {
        }
    }
}
void XTOOErrMsgImp::MoveMsg(char * Msg) noexcept
{
    if (m_Msg)
    {
        delete[] m_Msg;
    }
    m_Msg = Msg;
}

static XTOOErrMsgImp* GetStaticNoErrorXTOOErrMsgImp() noexcept
{
    static_constexpr_Imp XTOOErrMsgImp s{ eXTOONoError };
    ((XTOOErrMsgImp&)s).AddRef();
    return (XTOOErrMsgImp*)&s;
}

XTOOError::XTOOBaseErrorImp * XTOOErrMsgImp::Clear() noexcept
{
    if (HasMsg())
    {
        if (!IsSingleOwner())
        {
            //return TL_NEW XTOOErrMsgImp;
            return GetStaticNoErrorXTOOErrMsgImp();
        }
        MoveMsg(nullptr);
    }
    return nullptr;
}
void XTOOErrMsgImp::InsertInMsg(const char * Msg, char Space) noexcept
{
    const int len1 = Msg ? int_strlen(Msg) : 0;
    const int len2 = Space ? 1 : 0;
    const int len3 = m_Msg ? int_strlen(m_Msg) : 0;
    //if  (len1 + len2 + len3 + 1 > 512)
    {
        char* txt = nullptr;
        try
        {
            txt = TL_NEW char[len1 + len2 + len3 + 1];
        }
        catch(...)
        {
        }
        if  (txt)
        {
            memcpy(txt, Msg, len1);
            if (len2)
            {
                txt[len1] = ' ';
            }
            memcpy(txt + len1 + len2, m_Msg, len3);
            txt[len1 + len2 + len3] = 0;
            MoveMsg(txt);
        }
        else
        {
            SetMsg("error && XTOOErrMsgImp::InsertInMsg() alloc error");
        }
    }
    /*
    else
    {
        // we will work even if there is no memory
        char txt[512];
        memcpy(txt, Msg, len1);
        if (len2)
        {
            txt[len1] = ' ';
        }
        memcpy(txt + len1 + len2, m_Msg, len3);
        txt[len1 + len2 + len3] = 0;
        SetMsg(txt);
    }
    */
}
bool XTOOErrMsgImp::FindSetSubstError(int n) noexcept
{
    bool bRet = false;
    const char * pMsg = nullptr;
    SubstMsgErr * p;
    sSubstMsgErrListLock.LockM();
    const CMaaThreadIdType thr = CMaaGetCurrentThreadId();
    constexpr CMaaThreadIdType thr0 = CMaaInvalidThreadId();
    for (p = m_SubstMsgErrList.LookAtFront(); p; p = m_SubstMsgErrList.Next(p))
    {
        if  (p->m_nErr == n && (thr == thr0 || CMaaThreadIdsEqual(p->m_ThreadId, thr)))
        {
            if  ((pMsg = p->m_err.GetMsg()))
            {
                SetMsg(pMsg);
                bRet = true;
            }
            break;
        }
    }
    sSubstMsgErrListLock.UnLockM();
    if  (!p && n == CMaa_CONN_CLOSED_ERROR)
    {
        pMsg = "Connection is closed by remote host";
        SetMsg(pMsg);
        bRet = true;
    }
    return bRet;
}
//------------------------------------------------------------------------------
bool XTOOError::SetMsg(const char * Msg) noexcept
{
    XTOOErrMsgImp * x = dynamic_cast<XTOOErrMsgImp *>(m_pImp);
    if  (x)
    {
        x->SetMsg(Msg);
        return !Msg || x->GetMsg();
    }
    return false;
}
//------------------------------------------------------------------------------
bool XTOOError::MoveMsg(char * Msg) noexcept
{
    XTOOErrMsgImp* x = dynamic_cast<XTOOErrMsgImp*>(m_pImp);
    if (x)
    {
        x->MoveMsg(Msg);
        return !Msg || x->GetMsg();
    }
    else
    {
        delete[] Msg;
    }
    return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
XTOOErrMsg::XTOOErrMsg(const char * Msg)
:   XTOOError(TL_NEW XTOOErrMsgImp(Msg))
{
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
struct XTOOErrnoImp : public XTOOErrMsgImp
{
    int  m_errno, m_herrno, m_Flags;

    XTOOErrnoImp() noexcept;
    constexpr ~XTOOErrnoImp() {}
    XTOOErrnoImp(int Errno, int Flags = 0) noexcept;
    constexpr XTOOErrnoImp(_eXTOONoError) noexcept : XTOOErrMsgImp(eXTOONoError), m_errno(0), m_herrno(0), m_Flags(0) {}
    XTOOErrnoImp(const char * Msg, int Flags = 0) noexcept;
    void Init() noexcept;
    XTOOError::XTOOBaseErrorImp * Clear() noexcept;
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    ADD_ALLOCATOR(XTOOErrnoImp)
#endif
};
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
DEF_ALLOCATOR(XTOOErrnoImp)
#endif
//------------------------------------------------------------------------------
XTOOErrnoImp::XTOOErrnoImp() noexcept
{
    m_errno = errno;
    m_Flags = 0;
    Init();
}
XTOOErrnoImp::XTOOErrnoImp(int Errno, int Flags) noexcept
{
    m_errno = Errno != -1 ? Errno : errno;
    m_Flags = Flags;
    Init();
}
XTOOErrnoImp::XTOOErrnoImp(const char * Msg, int Flags) noexcept
{
    m_errno = errno;
    m_Flags = Flags;
    Init();
    if  (Msg)
    {
        InsertInMsg(Msg, ' ');
    }
    else if(!m_errno)
    {
        MoveMsg(nullptr);
    }
}

static XTOOErrnoImp* GetStaticNoErrorXTOOErrnoImp() noexcept
{
    static_constexpr_Imp XTOOErrnoImp s{ eXTOONoError };
    ((XTOOErrnoImp&)s).AddRef();
    return (XTOOErrnoImp*)&s;
}

XTOOError::XTOOBaseErrorImp* XTOOErrnoImp::Clear() noexcept
{
    if (m_errno || HasMsg())
    {
        if (!IsSingleOwner())
        {
            //return TL_NEW XTOOErrnoImp(0, 0);
            return GetStaticNoErrorXTOOErrnoImp();
        }
        m_errno = 0;
        m_Flags = 0;
        MoveMsg(nullptr);
    }
    return nullptr;
}
//------------------------------------------------------------------------------

#define ERRNO_USE__H_ERRNO 0x12340567

#ifdef __unix__
//static int h_errno = 0;
//static char * hstrerror ( int e )
//{
//    return nullptr;
//}
#endif

void XTOOErrnoImp::Init() noexcept
{
    char txtbuff[100];
    char * msg = nullptr;
    char * txt = txtbuff;
    int e;
    sprintf(txt, "errno = %d", e = m_errno);
    const char * p = m_errno ? strerror(m_errno) : nullptr;
    if  (m_Flags & 1)
    {
#ifdef __unix__
        const char* w = GetWsaErrorMessage(e);
        if  (w)
        {
            if  (p)
            {
                const int len1 = int_strlen(w);
                const int len2 = int_strlen(p);
                try
                {
                    msg = TL_NEW char[len1 + len2 + 3 + 1];
                }
                catch(...)
                {
                }
                if  (msg)
                {
                    //sprintf(msg, "%s - %s", w, p);
                    memcpy(msg, w, len1);
                    memcpy(msg + len1, " - ", 3);
                    memcpy(msg + len1 + 3, p, len2 + 1);
                    p = msg;
                }
                else
                {
                    p = w;
                }
            }
            else
            {
                p = w;
            }
        }
        if  (e == CMaa_CONN_CLOSED_ERROR)
        {
            p = "Connection is closed by remote host";
        }
#endif
    }
#ifdef __unix__
    if  (m_errno == ERRNO_USE__H_ERRNO)
    {
        e = m_herrno = h_errno;
        sprintf(txt, "h_errno = %d", e);
        p = e ? hstrerror(e) : nullptr;
    }
#endif
    p = p ? p : (e ? txt : "");
    if (p == msg)
    {
        MoveMsg(msg);
    }
    else
    {
        SetMsg(p);
        delete[] msg;
    }
}
//------------------------------------------------------------------------------
XTOOErrno::XTOOErrno()
:   XTOOError(TL_NEW XTOOErrnoImp)
{
}
XTOOErrno::XTOOErrno(int Errno)
:   XTOOError(TL_NEW XTOOErrnoImp(Errno))
{
}
XTOOErrno::XTOOErrno(const char * Msg)
:   XTOOError(TL_NEW XTOOErrnoImp(Msg))
{
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
struct XTOOWsaErrImp : public XTOOErrMsgImp
{
    int       m_err;
    int       m_IsDnsError;

    XTOOWsaErrImp() noexcept;
    constexpr ~XTOOWsaErrImp() {}
    XTOOWsaErrImp(int Err, int _IsDnsError) noexcept;
    constexpr XTOOWsaErrImp(_eXTOONoError) noexcept : XTOOErrMsgImp(eXTOONoError), m_err(0), m_IsDnsError(0) {}
    void Init() noexcept;
    XTOOError::XTOOBaseErrorImp * Clear() noexcept;
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    ADD_ALLOCATOR(XTOOWsaErrImp)
#endif
};
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
DEF_ALLOCATOR(XTOOWsaErrImp)
#endif
//------------------------------------------------------------------------------
XTOOWsaErrImp::XTOOWsaErrImp() noexcept
:   m_err(0),
    m_IsDnsError(-1)
{
#ifndef __unix__
#ifndef NOT_USE_WINSOCK
    m_err = WSAGetLastError();
#else
    m_err = 0;
#endif
#endif
    Init();
}

static XTOOWsaErrImp* GetStaticNoErrorXTOOWsaErrImp() noexcept
{
    static_constexpr_Imp XTOOWsaErrImp s{ eXTOONoError };
    ((XTOOWsaErrImp&)s).AddRef();
    return (XTOOWsaErrImp*)&s;
}

XTOOError::XTOOBaseErrorImp * XTOOWsaErrImp::Clear() noexcept
{
    if (m_err || HasMsg())
    {
        if (!IsSingleOwner())
        {
            //return TL_NEW XTOOWsaErrImp(0, 0);
            return GetStaticNoErrorXTOOWsaErrImp();
        }
        m_err = 0;
        m_IsDnsError = 0;
        MoveMsg(nullptr);
    }
    return nullptr;
}
XTOOWsaErrImp::XTOOWsaErrImp(int Err, int _IsDnsError) noexcept
{
    m_err = Err;
    m_IsDnsError = _IsDnsError;
#ifndef __unix__
#ifndef NOT_USE_WINSOCK
    if  (Err == -1)
    {
        m_err = WSAGetLastError();
    }
#endif
#endif
    Init();
}
void XTOOWsaErrImp::Init() noexcept
{
    if  (!FindSetSubstError(m_err))
    {
        char txt [ 100 ];
        //sprintf(txt, "Winsock error %d", m_err);
        const char* p = GetWsaErrorMessage(m_err);

#ifdef _WIN32
#ifdef WSANO_DATA
        if  (m_err == WSANO_DATA)
        {
            m_IsDnsError = 1;
        }
#endif
#ifdef NO_DATA
        if  (m_err == NO_DATA)
        {
            m_IsDnsError = 1;
        }
#endif
        /*
        if   (m_err == WSAHOST_NOT_FOUND || m_err == HOST_NOT_FOUND)
        {
             m_IsDnsError = 1;
        }
        */
        char * pWinMsg = nullptr;

        LPVOID lpMsgBuf = nullptr;
#ifdef _UNICODE
        if  (m_err)
        {
            FormatMessageW(
                 FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                 nullptr, m_err,
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                 (LPTSTR) &lpMsgBuf, 0, nullptr);
        }
        CMaaString tmp_utf8;
        try
        {
            tmp_utf8 = UnicodeToUtf8((const _WC_*)lpMsgBuf);
            //tmp_utf8 = UnicodeToUtf8(UnicodeString((const _WC_*)lpMsgBuf, -1, 0, false));
        }
        catch(...)
        {
        }
        char * pp = lpMsgBuf ? (char *)(const char *)tmp_utf8 : nullptr;
#else
        if  (m_err)
        {
            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, m_err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPSTR)&lpMsgBuf, 0, nullptr);
        }
        char * pp = (char *)lpMsgBuf;
#endif
        if  (!pp)
        {
            //pp = (char *)"Unknown error";
            if (m_err)
            {
                sprintf(txt, "Winsock error %d", m_err);
                p = txt;
            }
            else
            {
                p = "";
            }
        }
        else
        {
            char* e = pp + strlen(pp);
            while(pp < e && (e[-1] == 0x0a || e[-1] == 0x0d))
            {
                *(--e) = 0;
            }
            if  (p)
            {
                try
                {
                    const int len1 = int_strlen(p);
                    const int len2 = (int)(e - pp); // = int_strlen(pp);
                    pWinMsg = TL_NEW char[len1 + len2 + 20];
                    if  (pWinMsg)
                    {
                        //sprintf(pWinMsg, "%s - %s", p, pp);
                        memcpy(pWinMsg, p, len1);
                        memcpy(pWinMsg + len1, " - ", 3);
                        memcpy(pWinMsg + len1 + 3, pp, len2 + 1);
                    }
                }
                catch(...)
                {
                }
                p = pWinMsg;
            }
            if  (!p)
            {
                SetMsg(pp);
            }
        }
        if  (lpMsgBuf)
        {
            LocalFree(lpMsgBuf);
        }
        if  (p)
        {
            if (p == pWinMsg)
            {
                MoveMsg(pWinMsg);
            }
            else
            {
                SetMsg(p);
                delete[] pWinMsg;
            }
        }
#else
        //SetMsg(p ? p : m_err ? txt : "");
        if (!p && m_err)
        {
            sprintf(txt, "Winsock error %d", m_err);
            SetMsg(txt);
        }
        else
        {
            SetMsg(p ? p : "");
        }
#endif
    }
}
//------------------------------------------------------------------------------
XTOOWsaErr::XTOOWsaErr()
:   XTOOError(TL_NEW XTOOWsaErrImp)
{
}
XTOOWsaErr::XTOOWsaErr(int Err, int _IsDnsError)
:   XTOOError(TL_NEW XTOOWsaErrImp(Err, _IsDnsError))
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#ifndef __unix__
struct XTOOSockErrImp : public XTOOWsaErrImp
#else
struct XTOOSockErrImp : public XTOOErrnoImp
#endif
{
    mutable CMaaSocket * m_pSock;
#ifdef __unix__
    int m_IsDnsError;
#endif

    XTOOSockErrImp(const char * Msg, CMaaSocket * pSock, const char * pConnName = nullptr, int Err = -1, int _IsDnsError = 0);
    ~XTOOSockErrImp();
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    ADD_ALLOCATOR(XTOOSockErrImp)
#endif
};
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
DEF_ALLOCATOR(XTOOSockErrImp)
#endif
//------------------------------------------------------------------------------
XTOOSockErrImp::XTOOSockErrImp(const char * Msg, CMaaSocket * pSock, const char * pConnName, int Err, int _IsDnsError)
:
#ifndef __unix__
    XTOOWsaErrImp(Err, _IsDnsError),
    m_pSock(pSock)
#else
    XTOOErrnoImp(Err, 1),
    m_pSock(pSock),
    m_IsDnsError(_IsDnsError)
#endif
{
    Msg = Msg ? Msg : "";
    if  (pConnName)
    {
        const int l1 = (int)strlen(pConnName);
        const int l2 = (int)strlen(Msg);
        if  (l1 + l2 + 4 <= 512)
        {
            char txt[512];
            sprintf(txt, "%s: %s ", pConnName, Msg);
            InsertInMsg(txt);
        }
        else
        {
            char* txt = nullptr;
            try
            {
                txt = TL_NEW char[l1 + l2 + 4];
            }
            catch(...)
            {
            }
            if  (txt)
            {
                sprintf(txt, "%s: %s ", pConnName, Msg);
                InsertInMsg(txt);
                delete[] txt;
            }
            else
            {
                InsertInMsg(Msg, ' ');
            }
        }
    }
    else
    {
        InsertInMsg(Msg, ' ');
    }

    if  (pSock)
    {
        try
        {
#ifndef TOOLSLIB_SINGLE_THREAD
            pSock->Notify_PreDeletingByException();
#endif
        }
        catch(...)
        {
        }
    }
}
//------------------------------------------------------------------------------
XTOOSockErrImp::~XTOOSockErrImp()
{
    if  (m_pSock)
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        delete m_pSock;
#endif
    }
}
//------------------------------------------------------------------------------
XTOOSockErr::XTOOSockErr(const char * Msg, CMaaSocket * pSock, const char * pConnName, int Err, int _IsDnsError)
:   XTOOError(TL_NEW XTOOSockErrImp(Msg, pSock, pConnName, Err, _IsDnsError))
{
}
CMaaSocket * XTOOSockErr::GetSock() const noexcept
{
    return ((XTOOSockErrImp *)m_pImp)->m_pSock;
}
int XTOOSockErr::IsDnsError() const noexcept
{
    return ((XTOOSockErrImp *)m_pImp)->m_IsDnsError;
}

int XTOOSockErr::GetErrorCode() const noexcept
{
#ifdef _WIN32
    return ((XTOOSockErrImp *)m_pImp)->m_err;
#else
    return ((XTOOErrnoImp *)m_pImp)->m_errno;
#endif

}

void XTOOSockErr::Eat() noexcept
{
    ((XTOOSockErrImp *)m_pImp)->m_pSock = nullptr;
}
//------------------------------------------------------------------------------

/*
XTOOSockErr::XTOOSockErr(const XTOOSockErr &That)
:    XTOOError(That.m_pImp)
{
     printf("XTOOSockErr(That)\n");
}
XTOOSockErr & XTOOSockErr::operator=(const XTOOSockErr &That)
{
     return *this;
}
*/


XTOOSockDnsErr::XTOOSockDnsErr(const char * Msg, CMaaSocket * pSock, const char * pConnName)
#ifdef __unix__
:   XTOOSockErr((errno = ERRNO_USE__H_ERRNO) ? Msg : Msg, pSock, pConnName, -1, 1)
#else
:   XTOOSockErr(Msg, pSock, pConnName, -1, 1)
#endif
{
}



//------------------------------------------------------------------------------
struct XTOOSockCloseImp : public XTOOError::XTOOBaseErrorImp
{
    CMaaSocket * m_pSock;

    XTOOSockCloseImp(CMaaSocket* pSock);
    constexpr ~XTOOSockCloseImp()
    {
        if (m_pSock)
        {
#ifndef TOOLSLIB_SINGLE_THREAD
            delete m_pSock;
#endif
        }
    }
    constexpr XTOOSockCloseImp(_eXTOONoError) : XTOOError::XTOOBaseErrorImp(eXTOONoError), m_pSock(nullptr) {}
    XTOOError::XTOOBaseErrorImp * Clear() noexcept override;
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    ADD_ALLOCATOR(XTOOSockCloseImp)
#endif
};
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
DEF_ALLOCATOR(XTOOSockCloseImp)
#endif
//------------------------------------------------------------------------------
//void Log(const char * txt);
//------------------------------------------------------------------------------
XTOOSockCloseImp::XTOOSockCloseImp(CMaaSocket * pSock)
:   m_pSock(pSock)
{
    /*
    char txt [ 256 ];
    sprintf ( txt, "XTOOSockCloseImp::XTOOSockCloseImp ( CMaaSocket * pSock ), pSock=%p", pSock);
    Log(txt);
    */

    if  (pSock)
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        pSock->Notify_PreDeletingByException();
#endif
    }
}
//------------------------------------------------------------------------------
/*
XTOOSockCloseImp::~XTOOSockCloseImp()
{
    if (m_pSock)
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        delete m_pSock;
#endif
    }
}
*/
//------------------------------------------------------------------------------
static XTOOSockCloseImp* GetStaticNoErrorXTOOSockCloseImp() noexcept
{
    static_constexpr_Imp XTOOSockCloseImp s{ eXTOONoError };
    ((XTOOSockCloseImp&)s).AddRef();
    return (XTOOSockCloseImp*)&s;
}

XTOOError::XTOOBaseErrorImp * XTOOSockCloseImp::Clear() noexcept
{
    if (IsSingleOwner())
    {
        if (m_pSock)
        {
#ifndef TOOLSLIB_SINGLE_THREAD
            delete m_pSock;
#endif
            m_pSock = nullptr;
        }
        return nullptr;
    }
    //return TL_NEW XTOOSockCloseImp(nullptr);
    return GetStaticNoErrorXTOOSockCloseImp();
}
//------------------------------------------------------------------------------
XTOOSockClose::XTOOSockClose(CMaaSocket * pSock)
:   XTOOError(TL_NEW XTOOSockCloseImp(pSock))
{
}
//------------------------------------------------------------------------------
CMaaSocket* XTOOSockClose::GetSock() const noexcept
{
    return ((XTOOSockCloseImp*)m_pImp)->m_pSock;
}
//------------------------------------------------------------------------------

#ifdef __BORLANDC__
DWORD GetLastError()
{
    return errno;
}
#endif
//------------------------------------------------------------------------------
struct XTOOGetLastErrorImp : public XTOOErrMsgImp
{
    /*mutable*/ DWORD m_Error;

    XTOOGetLastErrorImp() noexcept;
    constexpr ~XTOOGetLastErrorImp() {}
    XTOOGetLastErrorImp(DWORD LastError) noexcept;
    constexpr XTOOGetLastErrorImp(_eXTOONoError) noexcept
    :   XTOOErrMsgImp(eXTOONoError),
        m_Error(0)
    {
        //Init(); do nothing
    }
    XTOOGetLastErrorImp(const char * Msg, DWORD LastError = -1) noexcept;
    void Init() noexcept;
    XTOOError::XTOOBaseErrorImp * Clear() noexcept;
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    ADD_ALLOCATOR(XTOOGetLastErrorImp)
#endif
};
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
DEF_ALLOCATOR(XTOOGetLastErrorImp)
#endif

#ifdef __unix__
static int GetLastError() noexcept
{
    return errno;
}
#endif
XTOOGetLastErrorImp::XTOOGetLastErrorImp() noexcept
{
    m_Error = GetLastError();
    Init();
}
XTOOGetLastErrorImp::XTOOGetLastErrorImp(DWORD LastError) noexcept
{
    m_Error = LastError == (DWORD)-1 ? GetLastError() : LastError;
    Init();
}
XTOOGetLastErrorImp::XTOOGetLastErrorImp(const char * Msg, DWORD LastError) noexcept
{
    m_Error = LastError == (DWORD)-1 ? GetLastError () : LastError;
    Init();
    if  (Msg)
    {
        InsertInMsg(Msg, ' ');
    }
    else if (!m_Error)
    {
        MoveMsg(nullptr);
    }
}
void XTOOGetLastErrorImp::Init() noexcept
{
    if  (m_Error)
    {
        LPVOID lpMsgBuf = nullptr;
#if defined(__BORLANDC__) || defined(__unix__)
        const char* str1 = strerror((int)m_Error);
        const char* pszShortName = nullptr; // GetWsaErrorMessage((int)m_Error);
        const int len1 = int_strlen(str1);
        const int len11 = pszShortName ? int_strlen(pszShortName) : 0;
        try
        {
            lpMsgBuf = TL_NEW char[len1 + len11 + 3 + 1];
        }
        catch(...)
        {
        }
        if  (!lpMsgBuf)
        {
            char b2[256];
            sprintf(b2, "Error code=%d (%s%s%s)", (int)m_Error, len1 + len11 + 40 < (int)sizeof(b2) ? pszShortName : "", len1 + len11 + 40 < (int)sizeof(b2) && pszShortName ? " - " : "", len1 + len11 + 40 < (int)sizeof(b2) ? str1 : "too long error message");
            SetMsg(b2);
            return;
        }
        //strcpy((char*)lpMsgBuf, strerror((int)m_Error));
        if (pszShortName)
        {
            memcpy((char*)lpMsgBuf, pszShortName, len11);
            memcpy(len11 + (char*)lpMsgBuf, " - ", 3);
            memcpy(len11 + 3 + (char*)lpMsgBuf, str1, len1 + 1);
        }
        else
        {
            memcpy((char*)lpMsgBuf, str1, len1 + 1);
        }
        //CMaaString tmp_utf8 = (const char *)lpMsgBuf;
        char* p = (char*)lpMsgBuf;
#else
#ifdef _UNICODE
        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, m_Error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR)&lpMsgBuf, 0, nullptr);
        CMaaString tmp_utf8;
        try
        {
            tmp_utf8 = UnicodeToUtf8((const _WC_*)lpMsgBuf);
        }
        catch(...)
        {
        }
        char* p = lpMsgBuf ? (char*)(const char*)tmp_utf8 : nullptr;
#else
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, m_Error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPSTR)&lpMsgBuf, 0, nullptr);
        char* p = (char*)lpMsgBuf;
#endif
#endif
        if  (!p)
        {
            p = (char *)"Unknown error";
        }
        else
        {
            char* e = p + strlen(p);
            while(p < e && (e[-1] == 0x0a || e[-1] == 0x0d))
            {
                *(--e) = 0;
            }
        }

        // For DOS console
        //CMaaString b(p);
        //CharToOem(p, (char*)(const char *)b);
        //p = b;

        char* b2 = nullptr;
        int len2 = int_strlen(p);
        try
        {
            b2 = TL_NEW char[len2 + 40];
        }
        catch(...)
        {
        }
        if  (b2)
        {
            sprintf(b2, "Error code=%d (%s)", m_Error, p);
            MoveMsg(b2);
            // Display the string.
            //MessageBox( nullptr, (LPCTSTR)b2, "Error", MB_OK | MB_ICONINFORMATION );
            //delete[] b2;
        }
        else
        {
            SetMsg(p);
            // Display the string.
            //MessageBox( nullptr, (LPCTSTR)p, "Error", MB_OK | MB_ICONINFORMATION );
        }
#if defined(__BORLANDC__) || defined(__unix__)
        delete[](char*) lpMsgBuf;
#else
        if  (lpMsgBuf)
        {
            LocalFree(lpMsgBuf);
        }
#endif
    }
}
//------------------------------------------------------------------------------
XTOOLastError::XTOOLastError()
:   XTOOError(TL_NEW XTOOGetLastErrorImp)
{
}
XTOOLastError::XTOOLastError(DWORD LastError)
    : XTOOError(TL_NEW XTOOGetLastErrorImp(LastError))
{
}
XTOOLastError::XTOOLastError(int LastError)
    : XTOOError(TL_NEW XTOOGetLastErrorImp((DWORD)LastError))
{
}

static XTOOGetLastErrorImp* GetStaticNoErrorXTOOGetLastErrorImp() noexcept
{
    static_constexpr_Imp XTOOGetLastErrorImp s{ eXTOONoError };
    ((XTOOGetLastErrorImp&)s).AddRef();
    return (XTOOGetLastErrorImp*)&s;
}

XTOOLastError::XTOOLastError(_eXTOONoError) noexcept
:   XTOOError(GetStaticNoErrorXTOOGetLastErrorImp())
{
}

//------------------------------------------------------------------------------
XTOOError::XTOOBaseErrorImp* XTOOGetLastErrorImp::Clear() noexcept
{
    if (m_Error || HasMsg())
    {
        if (!IsSingleOwner())
        {
            //return TL_NEW XTOOGetLastErrorImp((DWORD)0);
            return GetStaticNoErrorXTOOGetLastErrorImp();
        }
        m_Error = 0;
        MoveMsg(nullptr);
    }
    return nullptr;
}
//------------------------------------------------------------------------------

XTOOLastError::XTOOLastError(const char * Msg, DWORD LastError)
:   XTOOError(TL_NEW XTOOGetLastErrorImp(Msg, LastError))
{
}
DWORD XTOOLastError::GetErrorCode() const noexcept
{
    return m_pImp ? ((XTOOGetLastErrorImp *)m_pImp)->m_Error : 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/*
XTOOLastErrorEx::XTOOLastErrorEx(const char * Format, ...)
{
     CString s;
     va_list List;
     va_start(List, Format);
     s.FormatV(Format, List);
     ((XTOOErrMsgImp *)m_pImp)->InsertInMsg(s, ' ');
     va_end(List);
}
XTOOLastErrorEx::XTOOLastErrorEx(const char * Format, va_list Args)
{
     CString s;
     s.FormatV(Format, Args);
     ((XTOOErrMsgImp *)m_pImp)->InsertInMsg(s, ' ');
}
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class CMaaToolsLibClassImpRefKeeper to avoid delete/new on some zero count imps
//------------------------------------------------------------------------------
CMaaToolsLibClassImpRefKeeper::CMaaToolsLibClassImpRefKeeper() noexcept
{
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    XTOOErrMsgImp::GetAllocator().AddRefEx();
    XTOOErrnoImp::GetAllocator().AddRefEx();
    XTOOWsaErrImp::GetAllocator().AddRefEx();
    XTOOSockErrImp::GetAllocator().AddRefEx();
    XTOOSockCloseImp::GetAllocator().AddRefEx();
    XTOOGetLastErrorImp::GetAllocator().AddRefEx();
#endif
    CMaaFile::CMaaFileImp::GetAllocator().AddRefEx();
#ifdef _WIN32
    CMaaFindFile2::CMaaFindFile2Imp::GetAllocator().AddRefEx();
#else
#ifndef TOOLSLIB_SINGLE_THREAD
    //C_os_StartProcess::Reader::GetAllocator().AddRefEx();
    //C_os_StartProcess::Writer::GetAllocator().AddRefEx();
#endif
#endif

#if !defined(TOOLSLIB_SINGLE_THREAD) && (defined(__TOOLS_USE_WINSOCK2) || defined(__unix__))
    CMaaSockThread::SCmd::GetAllocator().AddRefEx();
#endif
}
CMaaToolsLibClassImpRefKeeper::~CMaaToolsLibClassImpRefKeeper()
{
#if !defined(TOOLSLIB_SINGLE_THREAD) && (defined(__TOOLS_USE_WINSOCK2) || defined(__unix__))
    CMaaSockThread::SCmd::GetAllocator().DelRefEx();
#endif

#ifndef _WIN32
#ifndef TOOLSLIB_SINGLE_THREAD
    //C_os_StartProcess::Writer::GetAllocator().DelRefEx();
    //C_os_StartProcess::Reader::GetAllocator().DelRefEx();
#endif
#else
    CMaaFindFile2::CMaaFindFile2Imp::GetAllocator().DelRefEx();
#endif
    CMaaFile::CMaaFileImp::GetAllocator().DelRefEx();
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    XTOOGetLastErrorImp::GetAllocator().DelRefEx();
    XTOOSockCloseImp::GetAllocator().DelRefEx();
    XTOOSockErrImp::GetAllocator().DelRefEx();
    XTOOWsaErrImp::GetAllocator().DelRefEx();
    XTOOErrnoImp::GetAllocator().DelRefEx();
    XTOOErrMsgImp::GetAllocator().DelRefEx();
#endif
}
//------------------------------------------------------------------------------

//==============================================================================
