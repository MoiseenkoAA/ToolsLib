
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Errors.h

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

#ifndef __TOOLS_ERRORS_H
#define __TOOLS_ERRORS_H

#define TOOLSLIB_ERRORS_IMP_ALLOCATORS


#ifdef _WIN32
#define CMaa_CONN_CLOSED_ERROR (WSABASEERR - 1)
#else
#define CMaa_CONN_CLOSED_ERROR 4000
#endif

#define TOO_DECLARE_THROW virtual void Raise() const override { throw *this; }

/*
#ifdef _WIN32
     #define TOO_DECLARE_THROW() virtual void Raise () const { throw *this; }
#else
     #define TOO_DECLARE_THROW() virtual void Raise () const {}
#endif
*/

enum _eXTOONoError
{
    eXTOONoError
};

//------------------------------------------------------------------------------
// struct XTOOBaseMsg
//------------------------------------------------------------------------------
struct ToolsExport XTOOBaseMsg
{
};

//------------------------------------------------------------------------------
// class XTOOMsg
//------------------------------------------------------------------------------
class ToolsExport XTOOMsg : public XTOOBaseMsg
{
private:
    // new version of error handling
    CMaaString message;
    //int        error;
protected:
    bool m_bIsThrow, m_Padding[3];
public:
    XTOOMsg(bool bIsThrow = true) noexcept;
    virtual ~XTOOMsg();

    virtual bool IsError() const noexcept = 0;
    virtual CMaaString ErrorMessage() const = 0;
    virtual void Clear() noexcept = 0;

    //protected:
    void SetError();

    // TL_NEW version of error handling
    void SetErrMessEx(const char* error_message) { message = error_message; }
    void SetErrMessEx(CMaaString error_message) noexcept { message = error_message; }
    CMaaString GetErrMessEx() const noexcept { return message; }
    //int GetErrorEx()          {     return error;      }
    //void SetErrorEx( int error_ ) { error = error_; }

private:
    virtual void Raise() const = 0;
};

//------------------------------------------------------------------------------
const char* GetWsaErrorMessage(int Code) noexcept;
//------------------------------------------------------------------------------
// struct XCSRWSAError
//------------------------------------------------------------------------------
struct XTOOWSAError : public XTOOBaseMsg
{
    int m_ErrorCode;

    XTOOWSAError(int ErrorCode) noexcept
    {
        m_ErrorCode = ErrorCode;
    }
};

//------------------------------------------------------------------------------
// struct XCSRThread
//------------------------------------------------------------------------------
struct XTOOThread : public XTOOBaseMsg
{
    int m_ErrorCode;

    XTOOThread(int ErrorCode) noexcept
    {
        m_ErrorCode = ErrorCode;
    }
};

//------------------------------------------------------------------------------
// class XTOOStatus
//------------------------------------------------------------------------------
#ifndef __TOOLSLIB_REMOVE_OLD_CODE_SKELETONS
class ToolsExport XTOOStatus : public XTOOMsg
{
    //ETOOStatus m_Status;
    int m_Status;

public:
    /*
     enum ETOOStatus
     {
          eSuccess               = 0,
          eMemoryError,
          eTOOCopyFile,
          eTOOMoveFile,
          eTOOGetAttrib,
          eTOOSetAttrib,
          eTOOSetFTime,
          eTOOWriteLess,
          eInvalidImplementation,
          eTOOFindFileComplete,
          eKeyNotFound,
          eKeyDecryptionError,
          eShifrFileError,
          eDeShifrFileError,
          eSignAlreadyExists,
          eUse_GetLastError,
          eNotImplemented
     };

     XTOOStatus ( ETOOStatus Status, _bool bIsThrow = _true );
    */
    XTOOStatus(int Status, bool bIsThrow = true) noexcept;

    bool IsError() const noexcept override;

    //ETOOStatus Status () const;
    int Status() const noexcept;
    CMaaString ErrorMessage() const override;

    void Clear() noexcept override;

    //protected:
    //void SetError ( ETOOStatus Status );
    void SetError(int Status);
    void SetErrorSilent(int Status) noexcept
    {
        m_Status = Status;
    }

private:
    TOO_DECLARE_THROW
};

//------------------------------------------------------------------------------
// class XTOOErrno
// MAA: 29.11.2002 renamed to XTOOErrnoOld
//------------------------------------------------------------------------------
class ToolsExport XTOOErrnoOld : public XTOOStatus
{
    int m_iErrno;

public:
    XTOOErrnoOld(int iErrno, bool bIsThrow = true) noexcept;

    bool IsError() const noexcept override;

    int Errno() const noexcept;
    CMaaString ErrorMessage() const override;

    void Clear() noexcept override;

    //protected:
    void SetError(int iErrno);

private:
    TOO_DECLARE_THROW
};
#endif

//------------------------------------------------------------------------------
// class XTOOFile
//------------------------------------------------------------------------------
#ifndef __TOOLSLIB_REMOVE_OLD_CODE
class ToolsExport XTOOFile : public XTOOErrnoOld
{
public:
    XTOOFile(bool bIsThrow = true) noexcept;
    //XTOOFile(ETOOStatus Status, int iErrno, bool bIsThrow = true) noexcept;

    void Clear() noexcept override;

    bool IsError() const noexcept override;
    CMaaString ErrorMessage() const override;

    //protected:
    //void SetError(ETOOStatus Status, int iErrno);
    void SetError(int Status, int iErrno);

private:
    TOO_DECLARE_THROW
};
#endif // __TOOLSLIB_REMOVE_OLD_CODE

//==============================================================================
// MAA: Added 29.11.2002
//------------------------------------------------------------------------------
#ifdef NOT_USE_WINSOCK
class CMaaSocket
{
public:
    int Notify_PreDeletingByExeption() { return 0; }
    int Notify_PreDeletingByException() { return 0; }
};
#endif

//------------------------------------------------------------------------------
// struct XTOOBaseError
//------------------------------------------------------------------------------
struct ToolsExport XTOOBaseError
{
};


class ToolsExport XTOOError : public XTOOBaseError
{
public:
    struct ToolsExport XTOOBaseErrorImp : public CMaaManualReference // CMaaReference
    {
        XTOOBaseErrorImp() noexcept {}
        virtual constexpr ~XTOOBaseErrorImp() {}
        constexpr XTOOBaseErrorImp(_eXTOONoError) noexcept /* : CMaaManualReference(CMaaReferenceBase + 1) */ {}
        virtual const char * GetMsg() noexcept { return ""; }
        virtual XTOOBaseErrorImp * Clear() noexcept = 0;
    };

protected:
    XTOOBaseErrorImp * m_pImp;

public:
    XTOOError(XTOOBaseErrorImp * pImp) noexcept;
    virtual ~XTOOError();
    XTOOError(const XTOOError &That) noexcept;
    XTOOError & operator=(const XTOOError &That) noexcept;
    void Assign_locked(const XTOOError& That) noexcept;
    const char * GetMsg() noexcept
    {
        return m_pImp ? m_pImp->GetMsg() : "";
    }
    CMaaString GetOemMsg()
    {
        if  (!m_pImp)
        {
            return CMaaStringZ;
        }
        CMaaString s(m_pImp->GetMsg());
#ifdef _WIN32
        CharToOemA(s, (char *)(const char *)s);
#endif
        return s;
    }
    bool SetMsg(const char* Msg) noexcept;
    template<class S> bool SetMsg(const S& s) noexcept
    {
        return SetMsg((const char*)s.c());
    }
    bool MoveMsg(char * Msg) noexcept;
    void Clear() noexcept;
};
//------------------------------------------------------------------------------
struct XTOOErrMsgImp : public XTOOError::XTOOBaseErrorImp
{
private:
    /*mutable*/ char* m_Msg;
public:
    XTOOErrMsgImp() noexcept;
    constexpr ~XTOOErrMsgImp() { if (m_Msg) { delete[] m_Msg; } }
    constexpr XTOOErrMsgImp(_eXTOONoError) noexcept
    :   XTOOError::XTOOBaseErrorImp(eXTOONoError),
        m_Msg(nullptr)
    {
    }
    XTOOErrMsgImp(const char* Msg /*= nullptr*/) noexcept;
    XTOOErrMsgImp(char* Msg, bool bMove) noexcept;
    void SetMsg(const char * Msg) noexcept;
    void MoveMsg(char * Msg) noexcept;
    void InsertInMsg(const char * Msg, char Space = 0) noexcept;
    const char * GetMsg() noexcept override;
    bool FindSetSubstError(int n) noexcept;
    XTOOError::XTOOBaseErrorImp * Clear() noexcept override;
    bool HasMsg() const noexcept { return m_Msg && *m_Msg; }
#ifdef TOOLSLIB_ERRORS_IMP_ALLOCATORS
    ADD_ALLOCATOR(XTOOErrMsgImp)
#endif
};
//------------------------------------------------------------------------------
struct ToolsExport XTOOErrMsg : public XTOOError
{
    XTOOErrMsg(const char * Msg);
};

struct ToolsExport XTOOErrno : public XTOOError
{
    XTOOErrno();
    XTOOErrno(int Errno);
    XTOOErrno(const char * Msg);
};

struct ToolsExport XTOOWsaErr : public XTOOError
{
    XTOOWsaErr();
    XTOOWsaErr(int Err, int _IsDnsError = -1);
};

class CMaaSocket;

#ifdef __unix__
/*
class CMaaSocket
{
public:
    CMaaSocket()
    {
    }
    ~CMaaSocket()
    {
    }
};
*/
#endif
struct ToolsExport XTOOSockErr : public XTOOError
{
    XTOOSockErr(const char * Msg, CMaaSocket * pSock, const char * pConnName = nullptr, int Err = -1, int _IsDnsError = 0);
    CMaaSocket * GetSock() const noexcept;
    void Eat() noexcept;

    //     XTOOSockErr ( const XTOOSockErr & That );
    //     XTOOSockErr & operator = ( const XTOOSockErr & That );
    int IsDnsError() const noexcept;
    int GetErrorCode() const noexcept;
};

struct ToolsExport XTOOSockDnsErr : public XTOOSockErr
{
    XTOOSockDnsErr(const char * Msg, CMaaSocket * pSock, const char * pConnName = nullptr);
};

struct ToolsExport XTOOSockClose : public XTOOError
{
    XTOOSockClose(CMaaSocket * pSock);
    CMaaSocket* GetSock() const noexcept;
};

struct ToolsExport XTOOLastError : public XTOOError
{
    XTOOLastError();
    XTOOLastError(_eXTOONoError) noexcept;
    XTOOLastError(DWORD LastError);
    XTOOLastError(int LastError);
    XTOOLastError(const char * Msg, DWORD LastError = (DWORD)-1);
    template<class S> XTOOLastError(const S& s, DWORD LastError = (DWORD)-1)
    :   XTOOError(nullptr)
    {
        XTOOLastError tmp((const char *)s.c(), LastError);
        *this = tmp;
    }
    DWORD GetErrorCode() const noexcept;
};

typedef XTOOLastError XTOOFile2Error;

/*
struct ToolsExport XTOOLastErrorEx : public XTOOLastError
{
     XTOOLastErrorEx(const char * Format, ...);
     XTOOLastErrorEx(const char * Format, va_list Args);
};
*/

bool AddSubstError(XTOOError &err, int n, bool bAnyThread = false) noexcept;
bool RemoveSubstError(int n) noexcept;

class XTOOSubstError
{
    int m_n;
    //bool m_bAnyThread;
    bool m_bAdded;
public:
    XTOOSubstError(XTOOError &err, int n = -2, bool bAnyThread = false) noexcept
    {
        if  (!(m_bAdded = AddSubstError(err, m_n = n, bAnyThread)))
        {
            m_n = 0;
        }
    }
    int GetErrNum() const noexcept
    {
        return m_n;
    }
    virtual ~XTOOSubstError() noexcept
    {
        if  (m_bAdded)
        {
            RemoveSubstError(m_n);
        }
    }
};

class XTOOSubstSockErrorEx : public XTOOSubstError
{
public:
    XTOOSubstSockErrorEx(XTOOError &err, int n = -2, bool bAnyThread = false) noexcept
    :   XTOOSubstError(err, n, bAnyThread)
    {
        if  ((n = GetErrNum()) != 0)
        {
#ifdef _WIN32
            WSASetLastError(n);
#else
#ifdef __unix__
            errno = n;
#endif
#endif
        }
    }
    ~XTOOSubstSockErrorEx() noexcept {}
};

//==============================================================================

#endif // __TOOLS_ERRORS_H
