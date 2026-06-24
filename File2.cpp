
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: ��������� ������ ����������)
 */

// ToolsLib/File.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains cross-platform classes for working with
 * files in utf8, folders, finding files.
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
#ifdef _WIN32
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif
#else
#include "temp.h"
       #include <sys/ipc.h>
       #include <sys/shm.h>
       #include <sys/ptrace.h>
#endif

int gToolsLib_FileFlags = TOOLSLIB_SKIP_LN_PROC;

//constexpr const CMaa256Bits gAnySeparatorBits{ szFILESYSTEM_SLASH szOTHER_FILESYSTEM_SLASH, 2, false };

//#define __USE_LOG

//---------------------------------------------------------------------------
////#define pszFileName2strFileName(pszFileName) CMaaString(pszFileName)
//#define pszFileName2strFileName(pszFileName) CMaaString(pszFileName, CMaaString::eROStrlenMemString)
//---------------------------------------------------------------------------

constexpr_ CMaaFile::CE::CE() noexcept
:
    snStdin{ "stdin" }, snStdout{ "stdout" }, snStderr{ "stderr" }, snNul{ "nul" }, snNull{ "null" }, snDevNull{ "/dev/null" }, snSlash(szFILESYSTEM_SLASH), snOtherSlash(szOTHER_FILESYSTEM_SLASH),
    snPfxUtf8{ "\xEF\xBB\xBF" }, snUnicode{ "\xFF\xFE" }, snUnicodeBigEndian{ "\xFE\xFF" },

    sStdin{ &snStdin }, sStdout{ &snStdout }, sStderr{ &snStderr },
    sNul{ &snNul }, sNull{ &snNull }, sDevNull{ &snDevNull }, sSlash{ &snSlash }, sOtherSlash{ &snOtherSlash },

    sPfx
    {
#ifdef CMaaStringImp_NOT_NULL_MODE
        {&CMaaString::C().snZ},
#else
        {},
#endif
        {&snPfxUtf8}, {&snUnicode}, {&snUnicodeBigEndian}
    }
{
}

constexpr CMaaFile::CE2::CE2() noexcept
:
    snft
    {
        "dot ",    "dot    ", "dot",     "    dot", "dot ",
        "dotdot",  "dotdot ", "dotdot",  " dotdot", "dotdot",
        "dir ",    "dir    ", "dir",     "    dir", " dir",
        "file",    "file   ", "file",    "   file", "file",
        "symlink", "symlink", "symlink", "symlink", "symlink",
        "unknown", "unknown", "unknown", "unknown", "unknown"
    }
    //, s_ft{}
{
    for (int i = 0; i < 6 * 5; i++)
    {
        s_ft[i] = &snft[i];
    }
}

static constexpr_ CMaaFile::CE sCMaaStringC;
static constexpr CMaaFile::CE2 sCMaaStringC2;
const CMaaFile::CE& CMaaFile::C() noexcept
{
    return sCMaaStringC;
}
const CMaaFile::CE2& CMaaFile::C2() noexcept
{
    return sCMaaStringC2;
}

//---------------------------------------------------------------------------
// struct CMaaFile::CMaaFileImp
//---------------------------------------------------------------------------
DEF_ALLOCATOR(CMaaFile::CMaaFileImp)
//---------------------------------------------------------------------------
#ifdef _WIN32
//---------------------------------------------------------------------------
CMaaFile::CMaaFileImp::CMaaFileImp(HANDLE hFile, bool bStdHandle) noexcept
{
    pFile = hFile;
    m_bStdHandle = bStdHandle;
}
//---------------------------------------------------------------------------
CMaaFile::CMaaFileImp::~CMaaFileImp()
{
    if (!m_bStdHandle)
    {
        if (pFile && pFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(pFile);
        }
    }
}
//---------------------------------------------------------------------------
#endif // _WIN32

//---------------------------------------------------------------------------
#ifdef __unix__
//---------------------------------------------------------------------------
CMaaFile::CMaaFileImp::CMaaFileImp(int d, bool bStdHandle) noexcept
{
    fd = d;
    m_bStdHandle = bStdHandle;
}
//---------------------------------------------------------------------------
CMaaFile::CMaaFileImp::~CMaaFileImp()
{
    if (!m_bStdHandle)
    {
        if (fd != -1)
        {
            /*
            if (fd == 5)
            {
                ::fprintf(stderr, "~CMaaFileImp(): %d %d\n", fd, close(fd));
                ::fprintf(stderr, "~CMaaFileImp(): %d\n", errno);
            }
            else
            */
            {
                close(fd);
            }
        }
    }
}
//---------------------------------------------------------------------------
#endif // __unix__
//---------------------------------------------------------------------------
void CMaaFile::CMaaFileImp::SetClosingType(int ClosingType) noexcept
{
    m_bStdHandle = ClosingType < 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef __unix__
//#include <signal.h>
//#include <sys/wait.h>

#if 0
static void OnMySIGPIPE(int /*x*/)
{
    signal(SIGPIPE, OnMySIGPIPE);

    //printf ( "OnMySIGPIPE\n" );
    //fflush ( stdout );

    return;
}

static void OnMySIGBUS(int /*x*/)
{
    signal(SIGBUS, OnMySIGBUS);

    //printf ( "OnMySIGBUS\n" );
    //fflush ( stdout );

    return;
}
#endif

#endif
//---------------------------------------------------------------------------

#ifndef _WIN32
//---------------------------------------------------------------------------
_qword GetCurrentTime_us(time_t * pt, int * pusec) noexcept
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    if  (pt)
    {
        *pt = tv.tv_sec;
    }
    if  (pusec)
    {
        *pusec = tv.tv_usec;
    }
    return (_qword)tv.tv_sec * 1000000 + (_qword)tv.tv_usec;
}
//---------------------------------------------------------------------------
#endif

#ifdef _WIN32
//---------------------------------------------------------------------------
_qword GetCurrentTime_us(time_t * pt, int * pusec) noexcept
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    time_t t;
    int usec;
    FileTimeToTimetAndUsec(&ft, &t, &usec);
    if  (pt)
    {
        *pt = t;
    }
    if  (pusec)
    {
        *pusec = usec;
    }
    return (_qword)t * 1000000 + (_qword)usec;
}
//---------------------------------------------------------------------------
void SetProgramNameForDebugOutput(const TCHAR * txt)
{
#ifdef _WIN32
#ifdef _DEBUG
    static HANDLE hLogFile[3] = {nullptr, nullptr, nullptr};
    //static CMaaFile hLogFile[3] = {CMaaFile(nullptr), CMaaFile(nullptr), CMaaFile(nullptr)};

    int Types[3] = {_CRT_WARN, _CRT_ERROR, _CRT_ASSERT};
    int i;
    for (i = 0; i < 3; i++)
    {
        if  (hLogFile[i])
        {
            _CrtSetReportMode(Types[i], _CRTDBG_MODE_DEBUG);
            CloseHandle(hLogFile[i]);
        }
        hLogFile[i] = nullptr;
    }
    if  (txt)
    {
        const TCHAR * filenames[2][3] =
        {
            {_T("w.txt"),   _T("e.txt"),   _T("a.txt")},
            {_T("wp.txt"),  _T("ep.txt"),  _T("ap.txt")}
        };

        for (i = 0; i < 3; i++)
        {
            CMaaString f1 = CMaaFile::_utf8_path(_T("crtlogs\\")) + CMaaFile::_utf8_path(txt) + CMaaFile::_utf8_path(filenames[0][i]);
            CMaaString f2 = CMaaFile::_utf8_path(_T("crtlogs\\")) + CMaaFile::_utf8_path(txt) + CMaaFile::_utf8_path(filenames[1][i]);
            CMaaFile::Remove(f2, false);
            if  (CMaaFile::IsAFile(f1))
            {
                CMaaFile::MoveFile(f1, f2, false);
            }
            CMaaFile f(f1, CMaaFile::eWCD_SrSw, eNoExcept);
            hLogFile[i] = f.GetHandle(true);
        }
    }
    int n = 0;
    for (i = 0; i < 3; i++)
    {
        if  (hLogFile[i])
        {
            n++;
            _CrtSetReportMode(Types[i], _CRTDBG_MODE_FILE);
            _CrtSetReportFile(Types[i], hLogFile[i]);
        }
        else
        {
            _CrtSetReportMode(Types[i], _CRTDBG_MODE_DEBUG);
        }
    }
    //if   (n > 0)
    {
        _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
    }
    //_RPT0(_CRT_WARN,"warning file message\n");
    //_RPT0(_CRT_ERROR,"error file message\n");
    //_RPT0(_CRT_ASSERT,"assert file message\n");
#endif
#endif
}
//---------------------------------------------------------------------------
// _WIN32
HANDLE CMaaFile::GetHandle(bool bExtract) noexcept
{
    HANDLE h = nullptr;
    if  (m_pImp != nullptr)
    {
        if  (bExtract)
        {
            if  (m_pImp->IsSingleOwner())
            {
                h = m_pImp->pFile; m_pImp->pFile = nullptr;
                delete m_pImp; m_pImp = nullptr;
            }
        }
        else
        {
            h = m_pImp->pFile;
        }
    }
    return h;
}
//---------------------------------------------------------------------------
DWORD CMaaFile::GetFileAttributes(const CMaaString& FileName, bool bThrow)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString fn = MkFsCompatible(FileName);
    const bool bEq = fn.StrictEq(FileName);
    if (bEq)
    {
        fn.Empty();
    }
    return GetFileAttributes_Internal(bEq ? FileName : fn, bThrow);
#else
    return GetFileAttributes_Internal(MkFsCompatible(FileName), bThrow);
#endif
}
_dword CMaaFile::Attributes(const CMaaString &FileName, bool bThrow)
{
    return GetFileAttributes_Internal(MkFsCompatible(FileName), bThrow);
}
_dword CMaaFile::Attributes(const char* szFileName, bool bThrow)
{
    return GetFileAttributes_Internal(MkFsCompatible(pszFileName2strFileName(szFileName)), bThrow);
}
//---------------------------------------------------------------------------
DWORD CMaaFile::GetFileAttributes_Internal(const CMaaString &FileName, bool bThrow)
{
    CMaaString tmp = MkFsNative(FileName);
    const _dword Attrib = ::GetFileAttributes(MkFs_cast(tmp));
    if (Attrib == 0xffffffff)
    {
        if (bThrow)
        {
            XTOOFile2Error err(CMaaString::sFormat("CMaaFile::GetFileAttributes(\"%S\")", &FileName));
            throw err;
        }
    }
    return Attrib;
}
//---------------------------------------------------------------------------
bool CMaaFile::Attributes(const CMaaString& FileName, _dword Attr, bool bThrow)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString fn = MkFsCompatible(FileName);
    const bool bEq = fn.StrictEq(FileName);
    if (bEq)
    {
        fn.Empty();
    }
    return Attributes_Internal(bEq ? FileName : fn, Attr, bThrow);
#else
    return Attributes_Internal(MkFsCompatible(FileName), Attr, bThrow);
#endif
}
bool CMaaFile::Attributes(const char* szFileName, _dword Attr, bool bThrow)
{
    return Attributes_Internal(MkFsCompatible(pszFileName2strFileName(szFileName)), Attr, bThrow);
}
//---------------------------------------------------------------------------
bool CMaaFile::Attributes_Internal(const CMaaString &FileName, _dword Attr, bool bThrow)
{
    CMaaString tmp = MkFsNative(FileName);
    if (!SetFileAttributes(MkFs_cast(tmp), Attr))
    {
        if (bThrow)
        {
            XTOOFile2Error err(CMaaString::sFormat("CMaaFile::Attributes(%S)", &FileName));
            throw err;
        }
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
#endif // _WIN32

//---------------------------------------------------------------------------
#ifdef _WIN32
CMaaString CMaaFile::MkNative(const CMaaString& Name1) noexcept // Utf8ToUnicode(x)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    if (Name1.HasAddRefData1())
    {
        CMaaString Name = Name1.GetAddRefData(1);
        if (Name.IsNotEmpty())
        {
            return Name;
        }
    }
    CMaaString Name = Utf8ToUnicode(Name1);
    ((CMaaString&)Name1).SetAddRefData(Name, 1);
    return Name;
#else
    return Utf8ToUnicode(Name1);
#endif
}
#endif
CMaaString CMaaFile::MkCompatible(const CMaaString &Name1)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    if (Name1.HasAddRefData0())
    {
        CMaaString Name = Name1.GetAddRefData(0);
        if (Name.IsNotEmpty())
        {
            return Name;
        }
    }
#endif

    CMaaString Name = Name1;
    Name.ReplaceNN(OTHER_FILESYSTEM_SLASH, FILESYSTEM_SLASH);

#if 0
    // Handling "\\..\\" and "\\.."
    CMaaString Name2 = Name;
    {
        int pos = 0;
        while(pos >= 0)
        {
            int pos1 = Name2.Find(pos, szFILESYSTEM_SLASH ".." szFILESYSTEM_SLASH);
            int pos2 = Name2.Find(pos, szFILESYSTEM_SLASH "..");
            if  (pos2 + 3 != Name2.Length())
            {
                pos2 = -1;
            }
            pos = (pos1 < 0 || (pos2 >= 0 && pos2 < pos1)) ? pos2 : pos1;
            if  (pos >= 0)
            {
                int pos_1 = pos - 1;
                while(pos_1 >= 0)
                {
                    pos_1 = Name2.ReverseFind(pos_1, szFILESYSTEM_SLASH, 1, -1);
                    if  (pos_1 >= 0 && Name2[pos_1 + 1] == FILESYSTEM_SLASH)
                    {
                        pos_1--;
                    }
                    else
                    {
                        break;
                    }
                }
                if  (pos_1 >= 0)
                {
                    if  (pos_1 > 0 || Name2[pos + 3] == FILESYSTEM_SLASH)
                    {
                        Name2.SetLeftMid(pos_1, pos + 3);
                        pos = pos_1;
                    }
                    else
                    {
                        Name2 = Name;
                        break;
                    }
                }
                else
                {
                    Name2 = Name;
                    break;
                }
            }
        }
        Name = Name2;
    }
#endif

#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (ALLOW_NZT_FILENAMES)
    {
        //return Name;
    }
    else
#endif
    {
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
        Name = Name.Str0Copy();
#else
        return Name.Str0Copy();
#endif
    }

#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    ((CMaaString&)Name1).SetAddRefData(Name, 0);
#endif
    return Name;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::MkNativeCompatible(const CMaaString& FileName)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString fn = MkFsCompatible(FileName);
    const bool bEq = fn.StrictEq(FileName);
    if (bEq)
    {
        fn.Empty();
    }
    return MkFsNative(bEq ? FileName : fn);
#else
    return MkFsNative(MkFsCompatible(FileName));
#endif
}
//---------------------------------------------------------------------------

#ifndef _WIN32
//---------------------------------------------------------------------------
int CMaaFile::GetHandle(bool bExtract) noexcept
{
    int h = -1;
    if  (m_pImp != nullptr)
    {
        if  (bExtract)
        {
            if  (m_pImp->IsSingleOwner())
            {
                h = m_pImp->fd; m_pImp->fd = -1;
                delete m_pImp; m_pImp = nullptr;
            }
        }
        else
        {
            h = m_pImp->fd;
        }
    }
    return h;
}
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
bool CMaaFile::CopyFile(const CMaaString& src, const CMaaString& dst, int Flags, bool bThrow)
{
    //#ifdef _WIN32
    if  ((Flags & eFailIfExist) && IsExists(dst))
    {
        return StaticError(0, bThrow, "CopyFile from %S to %S error: destination file is exist", &src, &dst);
    }
    char Buffer[100 * 1024];
    CMaaFile fsrc(src, CMaaFile::eR_Sr, bThrow);
    if  (!fsrc.IsOpen())
    {
        return false;
    }
    CMaaFile fdst(dst, (Flags & eCreateFolder) ? eWCD : eWC, bThrow);
    if  (!fdst.IsOpen())
    {
        return false;
    }
    _qword TotLen = 0;
    while(1)
    {
        _dword x = fsrc.Read(Buffer, sizeof(Buffer));
        if  (x == 0 /*|| x == (_dword)-1*/)
        {
            break;
        }
        _dword y = fdst.Write(Buffer, x);
        TotLen += y;
        if  (y != x)
        {
            fdst.Close();
            try
            {
                Remove(dst);
            }
            catch(...)
            {
            }
            return StaticError(0, bThrow,
               TotLen != 0 ?
               "CopyFile from %S to %S failed: can't write all data to a file" :
               "CopyFile from %S to %S failed: can't write any data to a file",
               &src, &dst);
        }
    }
    fdst.Close();
    fsrc.Close();
    {
        int usec = 0;
        time_t t = GetDateTime(src, &usec, false);
        if  (t != (time_t)-1)
        {
            SetDateTime(dst, t, usec, false);
        }
    }
    return true;
    /*
     if   (!::CopyFile(_uc_MkCompatible(szSource), _uc_MkCompatible(szDestination), fFailIfExists))
     {
          if   (bThrow)
          {
               CMaaString msg;
               msg.Format("CopyFile from %s to %s error:", szSource, szDestination);
               XTOOFile2Error err(msg);
               throw err;
          }
          return false;
     }
     return true;
     */
    //#endif
#if 000
    //CMaaString src = MkCompatible(szSource);
    //CMaaString dst = MkCompatible(szDestination);

    if  ((Flags & eFailIfExist) && IsExists(szDestination))
    //if   (fFailIfExists && IsExists(szDestination))
    {
        if  (bThrow)
        {
            CMaaString msg;
            msg.Format("CopyFile from %s to %s error: destination file is exist", szSource, szDestination);
            XTOOFile2Error err(msg, 0);
            throw err;
        }
        return false;
    }
    char Buffer[100 * 1024];
    CMaaFile src(szSource, CMaaFile::eR_Sr, bThrow);
    CMaaFile dst(szDestination, CMaaFile::eW, bThrow);
    if  (!src.IsOpen() || !dst.IsOpen())
    {
        return false;
    }
    while(1)
    {
        _dword x = src.Read(Buffer, sizeof(Buffer));
        if  (x == 0 /*|| x == (_dword)-1*/)
        {
            break;
        }
        if  (dst.Write(Buffer, x) != x)
        {
            return false;
        }
    }
    return true;
#endif
}

//---------------------------------------------------------------------------
// ����������� �����
//---------------------------------------------------------------------------
#ifdef __USE_LARGEFILE64
//int abc=def;
#endif

//---------------------------------------------------------------------------
bool CMaaFile::MoveFile(const CMaaString& Source, const CMaaString& Destination, bool bThrow)
{
#ifdef _WIN32
    CMaaString tmps = MkFsNativeCompatible(pszFileName2strFileName(Source));
    CMaaString tmpd = MkFsNativeCompatible(pszFileName2strFileName(Destination));
    if  (!::MoveFile(MkFs_cast(tmps), MkFs_cast(tmpd)))
    {
        if  (bThrow)
        {
            CMaaString msg;
            msg.Format("MoveFile from %S to %S error:", &Source, &Destination);
            XTOOFile2Error err(msg);
            throw err;
        }
        return false;
    }
    return true;
#endif
#ifdef __unix__
    if  (rename(MkFs_cast(MkFsNativeCompatible(Source)), MkFs_cast(MkFsNativeCompatible(Destination))) == -1)
    {
        if  (bThrow)
        {
            CMaaString msg;
            msg.Format("MoveFile from %S to %S error:", &Source, &Destination);
            XTOOFile2Error err(msg);
            throw err;
        }
        return false;
    }
    return true;
#endif
}
//---------------------------------------------------------------------------
#define FT_MAGIC 116444736000000000LL
//---------------------------------------------------------------------------
_qword TimetAndUsecTo_FileTime64(time_t t, int usec) noexcept
{
    return (_qword)t * 10000000LL + (_qword)usec * 10LL + FT_MAGIC;
}
//---------------------------------------------------------------------------
_qword TimetAndNsecTo_FileTime64(time_t t, _qword nsec) noexcept
{
    return (_qword)t * 10000000LL + nsec / 100LL + FT_MAGIC;
}
//---------------------------------------------------------------------------
_qword TimetAndNsecTo_FileTimeMy__Nsec(time_t t, _qword nsec) noexcept
{
    return (_qword)t * 1000000LL * 1000LL + nsec;
}
//---------------------------------------------------------------------------
void FileTime64_ToTimetAndUsec(_qword ft, time_t * pt, int * pusec) noexcept
{
    ft -= FT_MAGIC;
    ft /= 10;
    if  (pusec)
    {
        *pusec = ft % 1000000;
    }
    if  (pt)
    {
        *pt = ft / 1000000;
    }
}
//---------------------------------------------------------------------------
void FileTime64_ToTimetAndNsec(_qword ft, time_t * pt, _qword * pnsec) noexcept
{
    ft -= FT_MAGIC;
    if  (pnsec)
    {
        *pnsec = (ft % 10000000) * 100;
    }
    if  (pt)
    {
        *pt = ft / 10000000;
    }
}
//---------------------------------------------------------------------------
void FileTimeMy__Nsec_ToTimetAndNsec(_qword ft, time_t * pt, _qword * pnsec) noexcept
{
    if  (pnsec)
    {
        *pnsec = ft % 1000000000;
    }
    if  (pt)
    {
        *pt = ft / 1000000000;
    }
}
//---------------------------------------------------------------------------

#ifdef _WIN32
//---------------------------------------------------------------------------
void TimetAndUsecToFileTime(time_t t, int usec, FILETIME *pft) noexcept
{
    LONGLONG ll = Int32x32To64(t, 10000000) + Int32x32To64(usec, 10) + FT_MAGIC;
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = ll >> 32;
}
//---------------------------------------------------------------------------
void FileTimeToTimetAndUsec(const FILETIME * pft, time_t * pt, int * pusec) noexcept
{
    LONGLONG ll = (((LONGLONG)pft->dwHighDateTime) << 32) + (LONGLONG)pft->dwLowDateTime;
    ll -= FT_MAGIC;
    ll /= 10;
    if  (pusec)
    {
        *pusec = ll % 1000000;
    }
    if  (pt)
    {
        *pt = ll / 1000000;
    }
}
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
_qword CMaaFile::GetFileTime64(const CMaaString &FileName, bool bThrow)
{
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA d;
    memset(&d, 0, sizeof(d));
    CMaaString tmp = MkFsNativeCompatible(FileName);
    if  (GetFileAttributesEx(MkFs_cast(tmp), GetFileExInfoStandard, &d))
    {
        return ((_qword)d.ftLastWriteTime.dwHighDateTime << 32) | (_qword)d.ftLastWriteTime.dwLowDateTime;
    }
    else
    {
        if  (bThrow)
        {
            StaticError(-1, bThrow, "CMaaFile::GetFileTime64(\"%S\")", &FileName);
        }
    }
    return -1;
#endif
#ifdef __unix__
#ifdef __USE_LARGEFILE64
    struct stat64 s;
#else
    struct stat s;
#endif
    //timeval __utimes[2];
    //if [set] (utimes(fn, __utimes))
#ifdef __USE_LARGEFILE64
    if  (stat64(MkFs_cast(MkFsNativeCompatible(FileName)), &s))
#else
    if  (stat(MkFs_cast(MkFsNativeCompatible(FileName)), &s))
#endif
    {
        StaticError(-1, bThrow, "CMaaFile::GetFileTime64(\"%S\") - stat64()", &FileName);
        return -1;
    }
#ifdef __USE_LARGEFILE64
#ifdef STAT_HAVE_NSEC
    //__utf8_printf("st_mtim.tv_nsec = %,D\n", (_qword)s.st_mtim.tv_nsec);
#endif
    return TimetAndNsecTo_FileTime64(s.st_mtim.tv_sec, s.st_mtim.tv_nsec);
    //    return TimetAndUsecTo_FileTime64(s.st_mtim.tv_sec, s.st_mtim.tv_nsec / 1000);
#else
    return TimetAndUsecTo_FileTime64(s.st_mtime, 0); // temporarily
#endif
    //return TimetAndUsecTo_FileTime64(__utimes[1].tv_sec, __utimes[1].tv_usec); // [0] - access time, [1] - modification time
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::SetFileTime64(const CMaaString &FileName, _qword ft, bool bThrow)
{
#ifdef _WIN32
    CMaaFile f(FileName, CMaaFile::eRW_SrSw, bThrow);
    if  (!f.IsOpen())
    {
        return false;
    }
    if  (!f.m_pImp)
    {
        return StaticError(0, bThrow, "CMaaFile::SetFileTime64(\"%S\") - file can not be opened (temporarily)", &FileName);
    }
    FILETIME LastWriteTime;
    LastWriteTime.dwHighDateTime = (ft >> 32);
    LastWriteTime.dwLowDateTime = (DWORD)ft;
    if  (SetFileTime(f.m_pImp->pFile, nullptr, nullptr, &LastWriteTime))
    {
        return true;
    }
    return false;
#endif
#ifdef __unix__
    time_t t;
    int usec;
    FileTime64_ToTimetAndUsec(ft, &t, &usec);

    timeval tv[2]; // atime, mtime
    tv[0].tv_sec = tv[1].tv_sec = t;
    tv[0].tv_usec = tv[1].tv_usec = usec;
    if  (utimes(MkFs_cast(MkFsNativeCompatible(FileName)), tv))
    {
        return StaticError(-1, bThrow, "CMaaFile::SetFileTime64(\"%S\")", &FileName);
    }
    return true;
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::SetFileTime64Ex(const CMaaString& FileName, _qword TimeNeed, bool bThrow)
{
    if (SetFileTime64(FileName, TimeNeed, bThrow))
    {
        _qword TimeCurr = GetFileTime64(FileName, bThrow);
        if (TimeCurr == TimeNeed)
        {
            return true;
        }
        if (SetFileTime64(FileName, TimeNeed + (TimeNeed - TimeCurr), bThrow))
        {
            TimeCurr = GetFileTime64(FileName, bThrow);
            if (TimeCurr == TimeNeed)
            {
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
time_t CMaaFile::GetDateTime(const CMaaString &FileName, int * pusec, bool bThrow)
{
    if  (pusec)
    {
        *pusec = 0;
    }
    const _qword ft = GetFileTime64(FileName, bThrow);
    if  (ft != -1)
    {
        time_t t;
        int usec;
        FileTime64_ToTimetAndUsec(ft, &t, &usec);
        if  (pusec)
        {
            *pusec = usec;
        }
        return t;
    }
    return (time_t)-1;
}
//---------------------------------------------------------------------------
bool CMaaFile::SetDateTime(const CMaaString &FileName, time_t t, int usec, bool bThrow)
{
    return SetFileTime64(FileName, TimetAndUsecTo_FileTime64(t, usec), bThrow);
}
//---------------------------------------------------------------------------

#if 0
time_t CMaaFile::GetDateTime(CMaaString FileName, int * pusec, bool bThrow)
{
#ifdef _WIN32
    if  (pusec)
    {
        *pusec = 0;
    }
    time_t t = (time_t)-1;

    WIN32_FILE_ATTRIBUTE_DATA d;
    memset(&d, 0, sizeof(d));
    if  (GetFileAttributesEx(_uc_MkCompatible(FileName), GetFileExInfoStandard, &d))
    {
        FileTimeToTimetAndUsec(&d.ftLastWriteTime, &t, pusec);
    }
    else
    {
        if  (bThrow)
        {
            XTOOFile2Error err("CMaaFile::GetDateTime():");
            throw err;
        }
    }
    /*
     CMaaFile f(FileName, CMaaFile::eR_SrSw, bThrow);
     FILETIME CreationTime, LastAccessTime, LastWriteTime;
     if   (f.IsOpen() && f.m_pImp && GetFileTime(f.m_pImp->pFile, &CreationTime, &LastAccessTime, &LastWriteTime))
     {
          FileTimeToTimetAndUsec(&LastWriteTime, &t, pusec);
     }
     */
    return t;

    /*
     struct _stat s;
     if   (_stat(_uc_MkCompatible(FileName), &s))
     {
          if   (bThrow)
          {
//               XTOOFile2Error err("CMaaFile::GetDateTime");
//               throw err;
               SetError("CMaaFile::GetDateTime()");
          }
          return (time_t)-1;
     }
     return s.st_mtime;
     */
#endif
#ifdef __unix__
    if  (pusec)
    {
        *pusec = 0;
    }
    struct stat s;
    if  (stat(_uc_MkCompatible(FileName), &s))
    {
        if  (bThrow)
        {
            //               XTOOFile2Error err("CMaaFile::GetDateTime");
            //               throw err;
            SetError("CMaaFile::GetDateTime()");
        }
        return (time_t)-1;
    }
    return s.st_mtime;
#endif
}

bool CMaaFile::SetDateTime(CMaaString FileName, time_t t, int usec, bool bThrow)
{
#ifdef _WIN32
    CMaaFile f(FileName, CMaaFile::eRW_SrSw, bThrow);
    FILETIME LastWriteTime;
    TimetAndUsecToFileTime(t, usec, &LastWriteTime);
    //FILETIME CreationTime, LastAccessTime, LastWriteTime;
    if  (!f.IsOpen())
    {
        return false;
    }
    if  (!f.m_pImp)
    {
        if  (bThrow)
        {
            SetError("CMaaFile::SetDateTime() - bad implementation", 0);
            //XTOOFile2Error err("CMaaFile::SetDateTime() - bad implementation");
            //throw err;
        }
        return false;
    }
    if  (SetFileTime(f.m_pImp->pFile, nullptr, nullptr, &LastWriteTime))
    {
        return true;
    }
    return false;

    /*
     struct _utimbuf u;
     u.actime = u.modtime = t;

     if   (_utime(_uc_MkCompatible(FileName), &u))
     {
          if   (bThrow)
          {
//               XTOOFile2Error err("CMaaFile::SetDateTime");
//               throw err;
               SetError("CMaaFile::SetDateTime()");
          }
          return false;
     }
     return true;
     */
#endif
#ifdef __unix__
    timeval tv[2]; // atime, mtime
    tv[0].tv_sec = tv[1].tv_sec = t;
    tv[0].tv_usec = tv[1].tv_usec = usec;
    if  (utimes(_uc_MkCompatible(FileName), tv))
    {
        if  (bThrow)
        {
            //XTOOFile2Error err("CMaaFile::SetDateTime");
            //throw err;
            SetError("CMaaFile::SetDateTime()");
        }
        return false;
    }
    return true;
#endif
}
#endif

//---------------------------------------------------------------------------
bool CMaaFile::SetDateTimeEx(const CMaaString &FileName, time_t TimeNeed, int usec, bool bThrow)
{
    if  (SetDateTime(FileName, TimeNeed, usec, bThrow))
    {
        int us2;
        time_t TimeCurr = GetDateTime(FileName, &us2, bThrow);
        if  (TimeCurr == TimeNeed)
        {
            return true;
        }
        if  (SetDateTime(FileName, TimeNeed+(TimeNeed-TimeCurr), usec, bThrow))
        {
            TimeCurr = GetDateTime(FileName, &us2, bThrow);
            if  (TimeCurr == TimeNeed)
            {
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool CMaaFile::mkdir(const CMaaString &Path, bool bThrow)
{
#ifdef _WIN32
    int err = 0;
    if  (CreateDirectory(MkFs_cast(MkFsNativeCompatible(Path)), nullptr))
    {
        err = (int)GetLastError();
        if  (IsADir(Path)) // if  (IsADir_Internal(p))
        {
            return true;
        }
    }
#else
    int err = 0;
    if  (::mkdir(MkFs_cast(MkFsNativeCompatible(Path)), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) == 0)
    {
        err = errno;
        if  (IsADir(Path)) // if  (IsADir_Internal(p))
        {
            return true;
        }
    }
#endif
    return StaticError(err, bThrow, "CMaaFile::mkdir(\"%S\")", &Path);
}
//---------------------------------------------------------------------------
bool CMaaFile::chdir(const CMaaString &Path, bool bThrow)
{
#ifdef _WIN32
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString p = MkFsCompatible((Path.LastChar() == '\\' && !Path.IsRight(":\\", 2)) || (Path.LastChar() == '//' && !Path.IsRight(":/", 2)) ? Path.RemoveFromRight(1) : Path);
    const bool bEq = p.StrictEq(Path);
    if (bEq)
    {
        p.Empty();
    }
    CMaaString tmp = MkFsNative(bEq ? Path : p);
#else
    CMaaString tmp = MkFsNativeCompatible((Path.LastChar() == '\\' && !Path.IsRight(":\\", 2)) || (Path.LastChar() == '//' && !Path.IsRight(":/", 2)) ? Path.RemoveFromRight(1) : Path);
#endif
    if  (::SetCurrentDirectory(MkFs_cast(tmp)))
    {
        return true;
    }
    return StaticError(-1, bThrow, "CMaaFile::chdir(\"%S\")", &Path);
#else
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString p = MkFsCompatible((Path.LastChar() == '\\' && Path.Length() > 1) || (Path.LastChar() == '/' && Path.Length() > 1) ? Path.RemoveFromRight(1) : Path);
    const bool bEq = p.StrictEq(Path);
    if (bEq)
    {
        p.Empty();
    }
    CMaaString tmp = MkFsNative(bEq ? Path : p);
#else
    CMaaString tmp = MkFsNativeCompatible((Path.LastChar() == '\\' && Path.Length() > 1) || (Path.LastChar() == '/' && Path.Length() > 1) ? Path.RemoveFromRight(1) : Path);
#endif
    if  (::chdir(MkFs_cast(tmp)) == 0)
    {
        return true;
    }
    return StaticError(-1, bThrow, "CMaaFile::chdir(\"%S\")", &Path);
#endif
    return false;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetCanonicalPath(CMaaString Path)
{
#ifdef __unix__
    CMaaUnivHash<CMaaString, int> h;
    char Buffer[20 * 1024 + 1];
    if  (Path[0] != '/')
    {
        Path = GetCurrentDirectory() + "/" + Path;
    }
    for (int n = 1; n < Path.Length(); n++)
    {
        if  (Path[n] == '.')
        {
            if  (Path[n + 1] == '/' || Path[n + 1] == '\0')
            {
                Path.SetLeftMid(n - 1, n + 1);
                n--;
                continue;
            }
            if  (Path[n + 1] == '.' && (Path[n + 2] == '/' || Path[n + 2] == '\0') && n > 1)
            {
                int n2 = Path.ReverseFind(n - 2, '/');
                if  (n2 >= 0)
                {
                    Path.SetLeftMid(n2, n + 2);
                    n = n2;
                    continue;
                }
            }
        }

        if  (Path.Length() > 20 * 1024)
        {
            return ""; // recursive
        }
        int n2 = Path.Find(n, '/');
        if  (n2 < 0)
        {
            n2 = Path.Length();
            //break;
        }
        CMaaString tmp = Path.RefLeft(n2).Str0Copy();
        ssize_t x = readlink(tmp, Buffer, sizeof(Buffer) - 1);
        if  (x <= 0)
        {
            n = n2;
            continue;
        }
        else
        {
            if  (h.Add(Path, 1) != 0)
            {
                return "";
            }
            if  (Buffer[0] == '/')
            {
                Path = CMaaString(Buffer, x) + Path.RefMid(n2);
                n = 0;
            }
            else
            {
                if  (Buffer[x - 1] == '/' && (x >= 2 || Buffer[x - 2] != '/'))
                {
                    if  (IsADir(Path.RefLeft(n) + CMaaString(Buffer, x - 1)))
                    {
                        x--;
                    }
                }
                Path.SetLeftInsMid(n, CMaaString(Buffer, x), n2); n--;
            }
        }
    }
    //return Path;
#else
    //return Path;
#endif
//#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (ALLOW_NZT_FILENAMES)
    {
        return Path;
    }
    else
//#endif
    {
        return Path.Str0Copy(); // Path.Is0Behind() ? Path : Path.NewCopy();
    }
}
//---------------------------------------------------------------------------
#ifdef _WIN32
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetSystemDirectory(bool bThrow)
{
    TCHAR path[1024 + 2 * MAX_PATH];
    path[0] = 0;
    UINT n = ::GetSystemDirectory(path, sizeof(path)/ sizeof(TCHAR));
    if  (n == 0 || n > sizeof(path)/ sizeof(TCHAR))
    {
        if  (bThrow)
        {
            XTOOFile2Error err(n == 0 ? "CMaaFile::GetSystemDirectory()" : "CMaaFile::GetSystemDirectory() - too small buffer");
            throw err;
        }
        path[0] = 0;
    }
    return _utf8_path(path);
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetWindowsDirectory(bool bThrow)
{
    TCHAR path[1024 + 2 * MAX_PATH];
    path[0] = 0;
    UINT n = ::GetWindowsDirectory(path, sizeof(path)/ sizeof(TCHAR));
    if  (n == 0 || n > sizeof(path)/ sizeof(TCHAR))
    {
        if  (bThrow)
        {
            XTOOFile2Error err(n == 0 ? "CMaaFile::GetWindowsDirectory()" : "CMaaFile::GetWindowsDirectory() - too small buffer");
            throw err;
        }
        path[0] = 0;
    }
    return _utf8_path(path);
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetExeFileName(int bFullPath, bool bThrow)
{
    TCHAR path[1024 + 2 * MAX_PATH];
    path[0] = 0;
    const DWORD n = GetModuleFileName(nullptr, path, sizeof(path) / sizeof(TCHAR));
    if  (n == 0 || n > sizeof(path)/ sizeof(TCHAR))
    {
        if  (bThrow)
        {
            XTOOFile2Error err(n == 0 ? "CMaaFile::GetExeFileName()" : "CMaaFile::GetExeFileName() - too small buffer");
            throw err;
        }
        path[0] = 0;
    }
    CMaaString txt = _utf8_path(path);
    if  (bFullPath <= 0)
    {
        int n = (warning_int)txt.ReverseFind(FILESYSTEM_SLASH);
        if  (n >= 0)
        {
            txt = txt.RefMid(n + 1);
        }
        if  (bFullPath < 0)
        {
            n = bFullPath == -1 ? (warning_int)txt.ReverseFind('.') : (warning_int)txt.Find('.');
            if  (n >= 0)
            {
                txt = txt.Left(n);
            }
        }
    }
    return txt;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetExePath(bool bRelease, bool bThrow)
{
#ifndef TOOLSLIB_SINGLE_THREAD
    static constexpr CMaaLiteMutex s_mtx; // mutex
    CMaaAtomicFastMutexLocker Locker((CMaaLiteMutex&)s_mtx); // automatic scope locker
#endif
    static CMaaString v[2];
    if (v[bRelease].IsEmpty())
    {
        CMaaString Dir = GetExeFileName(1, bThrow);
        const int n = (warning_int)Dir.ReverseFind(FILESYSTEM_SLASH);
        if (n > 0)
        {
            Dir = Dir.Left(n + 1);
#ifdef _DEBUG
            if (bRelease && Dir.IsRightCi("\\debug\\", 7, 0))
            {
                Dir = Dir - 7 + "\\Release\\";
            }
#endif
        }
        v[bRelease] = Dir;
    }
    return v[bRelease];
}
//---------------------------------------------------------------------------
#endif // _WIN32

//---------------------------------------------------------------------------
CMaaString CMaaFile::getcwd(bool bThrow)
{
    CMaaString Path;
#ifdef _WIN32
    TCHAR Buffer[10 * 1024 + MAX_PATH];
    DWORD x = ::GetCurrentDirectory(sizeof(Buffer) / sizeof(TCHAR), Buffer);
    if  (x == 0)
    {
        if  (bThrow)
        {
            XTOOFile2Error err("CMaaFile::getcwd()");
            throw err;
        }
    }
    else if (x <= sizeof(Buffer) / sizeof(TCHAR))
    {
        Path = _utf8_path(Buffer);
    }
    else
    {
        CMaaPtr_<TCHAR, -1> Buffer((int)x + 10 * 1024 + MAX_PATH);
        if  (!Buffer.IsValid())
        {
            if  (bThrow)
            {
                XTOOFile2Error err("CMaaFile::getcwd() - new[] error", 0);
                throw err;
            }
        }
        else
        {
            x = ::GetCurrentDirectory((DWORD)Buffer.MaxIndex(), Buffer);
            if  (x == 0)
            {
                if  (bThrow)
                {
                    XTOOFile2Error err("CMaaFile::getcwd()");
                    throw err;
                }
            }
            else if (x <= Buffer.Size())
            {
                Path = _utf8_path(Buffer);
            }
            else
            {
                if  (bThrow)
                {
                    XTOOFile2Error err("CMaaFile::getcwd()");
                    throw err;
                }
            }
        }
    }
#else
    char Buffer[10 * 1024 + MAXPATHLEN];
    if  (::getcwd(Buffer, sizeof(Buffer)))
    {
        Path = _utf8_path(Buffer);
    }
    else
    {
        StaticError(-1, bThrow, "CMaaFile::getcwd()");
    }
#endif
    return Path;
}
//---------------------------------------------------------------------------
bool CMaaFile::chroot(const CMaaString &Path, bool bThrow)
{
#ifdef _WIN32
    return StaticError(0, bThrow, "CMaaFile::chroot() is not supported on WIN32");
#else
    if  (::chroot(MkFs_cast(MkFsNativeCompatible(Path))) == 0)
    {
        return true;
    }
    return StaticError(-1, bThrow, "CMaaFile::chroot(\"%S\")", &Path);
#endif
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::AppendPath(CMaaString src, CMaaString append, bool * pError)
{
    bool bError = false;
    src = MkFsCompatible(src);
    append = MkFsCompatible(append);
    CMaaString Drive;
#ifdef _WIN32
    if  (src[1] == ':')
    {
        Drive = src.RefLeft(3);
        src = src.RefMid(3);
    }
    else if (src[0] == FILESYSTEM_SLASH && src[1] == FILESYSTEM_SLASH)
    {
        int n = (warning_int)src.Find(2, FILESYSTEM_SLASH);
        if  (n > 0)
        {
            Drive = src.RefLeft(n + 1);
            src = src.RefMid(n + 1);
        }
        else
        {
            Drive = src;
            src.Empty();
        }
    }
#else
    if  (src[0] == FILESYSTEM_SLASH)
    {
        Drive = src.RefLeft(1);
        src = src.RefMid(1);
    }
#endif

    while(append.IsNotEmpty())
    {
        int n = (warning_int)append.Find(FILESYSTEM_SLASH);
        CMaaString Dir;
        if  (n < 0)
        {
            Dir = append;
            append.Empty();
        }
        else
        {
            Dir = append.RefLeft(n);
            append = append.RefMid(n + 1);
        }
        if  (Dir.IsNotEmpty() && Dir != ".")
        {
            if  (Dir == "..")
            {
                n = (warning_int)src.ReverseFind(FILESYSTEM_SLASH);
                if  (n < 0)
                {
                    if  (src.IsNotEmpty())
                    {
                        src.Empty();
                    }
                    else
                    {
                        bError = true;
                        break;
                    }
                }
                else
                {
                    src = src.RefLeft(n);
                }
            }
            else
            {
                if  (src.IsNotEmpty())
                {
                    src = src + szFILESYSTEM_SLASH + Dir;
                }
                else
                {
                    src = Dir;
                }
            }
        }
    }

    if  (src.IsNotEmpty())
    {
        if  (Drive.LastChar() == FILESYSTEM_SLASH)
        {
            src = Drive + src;
        }
        else
        {
            src = Drive + szFILESYSTEM_SLASH + src;
        }
    }
    else
    {
        if  (Drive.Length() > 2 && Drive.LastChar() == FILESYSTEM_SLASH)
        {
            src = Drive.RemoveFromRight(1);
        }
        else
        {
            src = Drive;
        }
    }
    if  (pError)
    {
        *pError = bError;
    }
//#if defined(_WIN32) && defined(_UNICODE)
    if constexpr (ALLOW_NZT_FILENAMES)
    {
        return src;
    }
    else
//#endif
    {
        return src.Str0Copy(); // src.Is0Behind() ? src : src.NewCopy();
    }
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetRelativePath(CMaaString AbsPath, CMaaString RelatedTo, /*out*/ bool *pbReleted /*= nullptr*/)
{
    CMaaString Ret;
    AbsPath = MkFsCompatible(AbsPath);
    RelatedTo = MkFsCompatible(RelatedTo);

    if  (pbReleted)
    {
        *pbReleted = false;
    }
    if  (AbsPath == "")
    {
        return RelatedTo;
    }

#ifdef _WIN32
    if  ((RelatedTo[1] == ':' && RelatedTo.Length() > 3) ||
         (RelatedTo[0] == FILESYSTEM_SLASH && RelatedTo[1] == FILESYSTEM_SLASH && RelatedTo.Length() > 3))
#else
    // __unix__
    if  (RelatedTo.Length() > 1 || RelatedTo[0] != FILESYSTEM_SLASH)
#endif
    {
        if  (RelatedTo.LastChar() != FILESYSTEM_SLASH)
        {
            RelatedTo += szFILESYSTEM_SLASH;
        }
    }
    else
    {
#ifdef _WIN32
        if  (RelatedTo[1] != ':')
#else
#endif
        if  (RelatedTo.LastChar() != FILESYSTEM_SLASH)
        {
            RelatedTo += szFILESYSTEM_SLASH;
        }
    }

    Ret = AbsPath;

#ifdef _WIN32
    if  (AbsPath.IsLeftCi(RelatedTo))
    {
        Ret = AbsPath.RefMid(RelatedTo.Length());
        if  (pbReleted)
        {
            *pbReleted = true;
        }
    }
#else
    if  (AbsPath.IsLeft(RelatedTo))
    {
        Ret = AbsPath.RefMid(RelatedTo.Length());
        if  (pbReleted)
        {
            *pbReleted = true;
        }
    }
#endif
    return Ret;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetAbsolutePath(CMaaString RelOrAbsPath, CMaaString RelatedTo, /*out*/ bool *pbWasReleted /*= nullptr*/)
{
    CMaaString Ret;
    RelOrAbsPath = MkFsCompatible(RelOrAbsPath);
    RelatedTo = MkFsCompatible(RelatedTo);

    if  (pbWasReleted)
    {
        *pbWasReleted = false;
    }
    if  (RelatedTo == "")
    {
        return RelOrAbsPath;
    }

#ifdef _WIN32
    if  ((RelatedTo[1] == ':' && RelatedTo.Length() > 3) ||
         (RelatedTo[0] == FILESYSTEM_SLASH && RelatedTo[1] == FILESYSTEM_SLASH && RelatedTo.Length() > 3))
#else
    // __unix__
    if  (RelatedTo.Length() > 1 || RelatedTo[0] != FILESYSTEM_SLASH)
#endif
    {
        if  (RelatedTo.LastChar() != FILESYSTEM_SLASH)
        {
            RelatedTo += szFILESYSTEM_SLASH;
        }
    }
    else
    {
#ifdef _WIN32
        if  (RelatedTo[1] != ':')
#else
#endif
        if  (RelatedTo.LastChar() != FILESYSTEM_SLASH)
        {
            RelatedTo += szFILESYSTEM_SLASH;
        }
    }

#ifdef _WIN32
    if  (RelOrAbsPath[1] == ':' || (RelOrAbsPath[0] == FILESYSTEM_SLASH && RelOrAbsPath[1] == FILESYSTEM_SLASH))
    {
        return RelOrAbsPath;
    }
#else
    if  (RelOrAbsPath[0] == FILESYSTEM_SLASH)
    {
        return RelOrAbsPath;
    }
#endif
    Ret = RelatedTo + RelOrAbsPath;
    if  (pbWasReleted)
    {
        *pbWasReleted = true;
    }
    return Ret;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifdef __unix__
#include "os_sig.h"
static void sig_alrm_handler(int /*sig*/)
{
    //sig = sig;
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool CMaaFile::LockFile(_dword TimeOut)
{
    if  (IsOpen())
    {
#ifdef _WIN32
        _qword l = Length();
        _qword o = GetCurPos();
        l -= o;
        OVERLAPPED ov;
        memset(&ov, 0, sizeof(ov));
        ov.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if  (!ov.hEvent)
        {
            SetError("Create event error", 0);
            return false;
        }
        ov.Offset = (DWORD)o;
        ov.InternalHigh = (DWORD)(o >> 32);
        bool Ret = false;
        if  (!LockFileEx(GetHandle(), LOCKFILE_EXCLUSIVE_LOCK /*| LOCKFILE_FAIL_IMMEDIATELY*/ /*? or 0*/, 0, (DWORD)l, (DWORD)(l >> 32), &ov))
        {
            DWORD err = GetLastError();
            if  (err == ERROR_IO_PENDING)// || err == ERROR_LOCK_VIOLATION)
            {
                if  (WaitForSingleObject(ov.hEvent, TimeOut) == WAIT_OBJECT_0)
                {
                    Ret = true;
                    DWORD x = 0;
                    GetOverlappedResult(GetHandle(), &ov, &x, TRUE);
                }
                else
                {
                    //GetOverlappedResult(GetHandle(), &ov, &x, TRUE);
                    CancelIo(GetHandle());
                }
            }
        }
        else
        {
            Ret = true;
        }
        CloseHandle(ov.hEvent);
        return Ret;
        // UnlockFileEx(HANDLE hFile, 0/*dwReserved*/, DWORD nNumberOfBytesToUnlockLow, DWORD nNumberOfBytesToUnlockHigh, LPOVERLAPPED lpOverlapped);
#else
        //m_fd = open(LockFileName, O_RDWR|O_TRUNC|O_CREAT/*|O_EXCL*/, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
        os_non_restarting_signal(SIGALRM, sig_alrm_handler);
        int Seconds = TimeOut / 1000;
        alarm(Seconds);
        //m_err = 0;

#ifdef ___ToolsLibQT___
        SetError("Bad implementation", 0);
#else
        if  (
#ifdef __USE_LARGEFILE64
      lockf64
#else
      lockf
#endif
       (GetHandle(), F_LOCK, 0) != 0)
        {
            int err = errno;
            alarm(0);
            //close(m_fd);
            //m_fd = -1;
            return false;
        }
        else
        {
            //m_err = 0;//errno;
            alarm(0);
            //errno = m_err;
            //printf("Lock - ok, errno = %d\n", errno);
            return true;
        }
#endif
#endif
    }
    else
    {
        SetError("Bad implementation", 0);
    }
    return false;
}
//---------------------------------------------------------------------------

#if 0
CFileTimedLock::CFileTimedLock(CMaaString LockFileName, int Seconds)
{
    m_fd = open(LockFileName, O_RDWR|O_TRUNC|O_CREAT/*|O_EXCL*/, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if  (m_fd < 0)
    {
        // error
        m_err = errno;
    }
    else
    {
        os_non_restarting_signal(SIGALRM, sig_alrm_handler);

        alarm(Seconds);

        m_err = 0;
        //if    (lockf(fd, F_TLOCK, 0) != 0)
        //if    (flock(fd, LOCK_EX) != 0)
        if  (
#ifdef __USE_LARGEFILE64
      lockf64
#else
      lockf
#endif
       (m_fd, F_LOCK, 0) != 0)
        {
            m_err = errno;
            alarm(0);
            close(m_fd);
            m_fd = -1;
            errno = m_err;
            //fprintf(stderr, "�� ���� ���������� ���������� �� ���� \"lock.txt\" errno = %d\n", errno);
        }
        else
        {
            m_err = 0;//errno;
            alarm(0);
            errno = m_err;
            //printf("Lock - ok, errno = %d\n", errno);
        }
    }
}

CFileTimedLock::~CFileTimedLock()
{
    if  (m_fd >= 0)
    {
        close(m_fd);
    }
}

bool CFileTimedLock::IsLocked() noexcept
{
    return m_fd >= 0;
}
#endif

//---------------------------------------------------------------------------
void CFileTimedLock::ctor_(int Seconds, bool bThrow)
{
#ifdef _WIN32
    if (m_file.IsOpen())
    {
        DWORD NumberOfBytesWritten = 0;
        OVERLAPPED ov;
        memset(&ov, 0, sizeof(ov));
        ov.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!ov.hEvent)
        {
            CMaaFile::StaticError(-1, bThrow, "Can't create event object");
            //return;
        }
        else
        {
            DWORD x = 0, e;
            BOOL Ret = WriteFile(m_file.GetHandle(false), "", 1, &NumberOfBytesWritten, &ov);
            if (!Ret && (e = GetLastError()) == ERROR_IO_PENDING)
            {
                Ret = GetOverlappedResult(m_file.GetHandle(false), &ov, &x, TRUE);
                /* or
                    if   ( WaitForSingleObject ( ov.hEvent, 10000 ) == WAIT_OBJECT_0 )
                    {
                         GetOverlappedResult ( m_hCom, &ov, &x, FALSE );
                    }
                    else
                    {
                         // ��������� ������
                    }
                    */
                if (!Ret)
                {
                    e = GetLastError();
                }
            }
            CloseHandle(ov.hEvent);
            if (!Ret)
            {
                CMaaFile::StaticError(e, bThrow, "WriteFile()");
            }
        }
        //m_file.Write("", 1);
        m_file.Seek(0);
    }
#endif
    if (!m_file.LockFile(Seconds * 1000))
    {
        m_file.Close();
        if (bThrow)
        {
            m_file.SetError(0, "LockFile()");
        }
    }
}
//---------------------------------------------------------------------------
CFileTimedLock::CFileTimedLock(CMaaString LockFileName, int Seconds, eNoExcept_)
    : m_file(LockFileName, "RWCD|SrSw|overlapped", false)
    //:    m_file(LockFileName, "R+CD|SrSw|overlapped", false)
{
    ctor_(Seconds, false);
}
//---------------------------------------------------------------------------
CFileTimedLock::CFileTimedLock(CMaaString LockFileName, int Seconds, bool bThrow)
    : m_file(LockFileName, "RWCD|SrSw|overlapped", bThrow)
    //:    m_file(LockFileName, "R+CD|SrSw|overlapped", bThrow)
{
    ctor_(Seconds, bThrow);
}
//---------------------------------------------------------------------------
CFileTimedLock::~CFileTimedLock()
{
    //m_file.Close();
}
//---------------------------------------------------------------------------
bool CFileTimedLock::IsLocked() noexcept
{
    return m_file.IsOpen();
}
//---------------------------------------------------------------------------
void CFileTimedLock::Close() noexcept
{
    m_file.Close();
}
//---------------------------------------------------------------------------

#if 0

//---------------------------------------------------------------------------
_dword CMaaFile::Attributes ( const char * szFileName, XTOOFile & Error )
{
    Error.Clear ();
    _dword Attrib = GetFileAttributes ( szFileName );
    if  ( Attrib == 0xffffffff )
    {
        // GetLastError ()
        //ror.XTOOStatus::SetError ( eTOOGetAttrib );
        Error.SetErrMessEx( szFileName );
        Error.XTOOStatus::SetError( e_ATTR_ERROR/*, errno*/ );
    }
    return Attrib;
}
//---------------------------------------------------------------------------
_bool CMaaFile::Attributes ( const char * szFileName, _dword Attr, XTOOFile & Error )
{
    Error.Clear ();
    if  ( SetFileAttributes ( szFileName, Attr ) == _false )
    {
        // GetLastError ()
        //Error.XTOOStatus::SetError ( eTOOSetAttrib );
        Error.SetErrMessEx( szFileName );
        Error.XTOOStatus::SetError( e_ATTR_ERROR/*, errno*/ );
    }
    return !Error.IsError ();
}
//---------------------------------------------------------------------------
_bool CMaaFile::LastWriteTime ( const char * szFileName, const CMaaDateTime & DateTime, XTOOFile & Error )
{
    struct _utimbuf TimeBuff;
    struct tm Tm;

    Error.Clear ();

    time_t tmp  = time ( nullptr );
    Tm.tm_isdst = localtime ( & tmp ) -> tm_isdst;
    // Positive if daylight saving time is in effect;
    // 0 if daylight saving time is not in effect;
    // negative if status of daylight saving time is unknown.

    Tm.tm_year = DateTime.Year  () - 1900;
    Tm.tm_mon  = DateTime.Month () - 1;
    Tm.tm_mday = DateTime.Day   ();

    Tm.tm_hour = DateTime.Hour   ();
    Tm.tm_min  = DateTime.Minute ();
    Tm.tm_sec  = DateTime.Second ();

    TimeBuff.actime = TimeBuff.modtime = mktime ( &Tm );

    if  ( _utime ( szFileName, &TimeBuff ) )
    {
        //Error.XTOOFile2Error::SetError ( errno );
        //          Error.SetErrMessEx( szFileName );
        //          Error.SetError ( e_SET_FILE_TIME, errno );
        ---- SetError(CMaaString::sFormat("CMaaFile::chroot(\"%S\")", &Path));
    }
    return !Error.IsError ();
}
//---------------------------------------------------------------------------
CMaaDateTime CMaaFile::LastWriteTime ( const char * szFileName, XTOOFile & Error )
{
    struct _stat StatInfo;
    struct tm Time;

    Error.Clear ();

    if  ( _stat ( szFileName, &StatInfo ) )
    {
        //Error.XTOOFile2Error::SetError ( errno );
        //          Error.SetErrMessEx( szFileName );
        //          Error.SetError ( e_GET_FILE_TIME, errno );
        ---               SetError(CMaaString::sFormat("CMaaFile::chroot(\"%S\")", &Path));
    }
    memcpy ( & Time, localtime ( &StatInfo.st_mtime ), sizeof ( Time ) );

    return CMaaDateTime ( Time.tm_year + 1900, Time.tm_mon + 1, Time.tm_mday,
         Time.tm_hour,        Time.tm_min,     Time.tm_sec,  0 );
}
//---------------------------------------------------------------------------
_dword CMaaFile::Length ( const char * szFileName, XTOOFile & Error )
{
    int Handle = _open ( szFileName, _O_RDONLY | _O_BINARY );

    Error.Clear ();

    if  ( Handle  == -1 )
    {
        //Error.XTOOFile2Error::SetError ( errno );
        //          Error.SetErrMessEx( szFileName );
        //          Error.SetError ( e_FILE_OPEN_ERROR, errno );
        ---               SetError(CMaaString::sFormat("CMaaFile::chroot(\"%S\")", &Path));
    }
    _dword Ret = _filelength ( Handle );
    _close ( Handle );

    if  ( Ret == -1 )
    {
        //ror.XTOOFile2Error::SetError ( errno );
        //          Error.SetErrMessEx( szFileName );
        //          Error.SetError ( e_FILE_SIZE, errno );
        ---               SetError(CMaaString::sFormat("CMaaFile::chroot(\"%S\")", &Path));
    }
    return Ret;
}
//---------------------------------------------------------------------------
CMaaDateTime CMaaFile::LastWriteTime ()
{
    if  ( IsOpen () )
    {
        struct _stat Info;
        struct tm Time;

        Clear ();
        if  ( !_fstat( _fileno ( m_pImp -> pFile ), & Info ) )
        {
            memcpy ( & Time, localtime ( & Info.st_mtime ), sizeof ( Time ) );
            return CMaaDateTime (
                 Time.tm_year + 1900, Time.tm_mon + 1, Time.tm_mday,
                 Time.tm_hour,        Time.tm_min,     Time.tm_sec,  0 );
        }
        //XTOOFile2Error::SetError ( errno );
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        SetError ( e_GET_FILE_TIME, errno );
    }
    return CMaaDateTime ( 0, 0, 0, 0, 0, 0, 0 );
}
//---------------------------------------------------------------------------
_bool CMaaFile::LastWriteTime ( const CMaaDateTime & DateTime )
{
    WORD wFatDate, wFatTime;

    FILETIME FileTime, LocalTime;

    if  ( IsOpen () )
    {
        wFatDate = DateTime.Day   () +
        ( ( DateTime.Month () - 1 ) << 5 ) +
        (( DateTime.Year  () - 1980 ) << 9 );

        wFatTime = DateTime.Second () /  2 +
        ( DateTime.Minute () << 5 ) +
        ( DateTime.Hour   () << 11 );

        DosDateTimeToFileTime ( wFatDate, wFatTime, &FileTime );

        LocalFileTimeToFileTime( & FileTime, & LocalTime );

        if  ( SetFileTime (
             ( HANDLE ) _get_osfhandle ( _fileno ( m_pImp -> pFile ) ),
             nullptr,        // time the file was created
             & LocalTime,   // time the file was last accessed
             & LocalTime )  // time the file was last written
             )
        {
            return _true;
        }
    }
    //XTOOStatus::SetError ( eTOOSetFTime );
    char buf[255];
    sprintf( buf, mess_for_search, __FILE__, __LINE__ );
    SetErrMessEx( buf );
    XTOOStatus::SetError ( e_SET_FILE_TIME );
    return _false;
}
//---------------------------------------------------------------------------

/*
#include <stdio.h>
#include <string.h>
//#include <sys/types.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define _ITS_FREEBSD_

#ifndef _ITS_FREEBSD_
#include "io4.h"
#else
#include <sys/stat.h>
#endif


#include "os_sig.h"

#include "uf.h"
*/

#endif // 0

//---------------------------------------------------------------------------
_dword CMaaFile::fprintf2(const char *Format, CMaaString Text, ...)
{
    va_list list;
    va_start(list, Text);
    CMaaString txt;
    txt.FormatV2(Format, Text, list);
    va_end(list);
    return Write(txt);
}
//---------------------------------------------------------------------------
_dword CMaaFile::fprintf(const char * format, ...)
{
    if (!IsOpen())
    {
        if (IsThrow())
        {
            SetError("Bad implementation", 0);
        }
        return 0;
    }
    CMaaString txt;
    va_list list;
    va_start(list, format);
    txt.FormatV(format, list);
    va_end(list);
    return Write(txt);
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_CHAR8T_SUPPORT
_dword CMaaFile::fprintf2(const char8_t* Format, CMaaString Text, ...)
{
    va_list list;
    va_start(list, Text);
    CMaaString txt;
    txt.FormatV2(Format, Text, list);
    va_end(list);
    return Write(txt);
}
//---------------------------------------------------------------------------
_dword CMaaFile::fprintf(const char8_t* format, ...)
{
    if (!IsOpen())
    {
        if (IsThrow())
        {
            SetError("Bad implementation", 0);
        }
        return 0;
    }
    CMaaString txt;
    va_list list;
    va_start(list, format);
    txt.FormatV(format, list);
    va_end(list);
    return Write(txt);
}
#endif
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFileName(const CMaaString &FullPath) noexcept
{
    const int n = (warning_int)FullPath.ReverseFind(FILESYSTEM_SLASH);
    return n >= 0 ? FullPath.RefMid(n + 1) : FullPath;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFolderName(const CMaaString &FullPath) // with ending slash
#if (ALLOW_NZT_FILENAMES)
    noexcept
#endif
{
    const int n = (warning_int)FullPath.ReverseFind(FILESYSTEM_SLASH);
    if constexpr (ALLOW_NZT_FILENAMES)
    {
        return (n >= 0) ? FullPath.RefLeft(n + 1) : CMaaStringZ;
    }
    else
    {
        return (n >= 0) ? FullPath.Left(n + 1).Str0Copy() : CMaaStringZ;
    }
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFileNameAny(const CMaaString &FullPath) noexcept
{
    const int n = (warning_int)FullPath.ReverseFind(gAnySeparatorBits); // any '/' or '\\' separator
    return n >= 0 ? FullPath.RefMid(n + 1) : FullPath;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFolderNameAny(const CMaaString &FullPath) // with ending slash
#if (ALLOW_NZT_FILENAMES)
    noexcept
#endif
{
    const int n = (warning_int)FullPath.ReverseFind(gAnySeparatorBits); // any '/' or '\\' separator
    if constexpr (ALLOW_NZT_FILENAMES)
    {
        return (n >= 0) ? FullPath.RefLeft(n + 1) : CMaaStringZ;
    }
    else
    {
        return (n >= 0) ? FullPath.Left(n + 1).Str0Copy() : CMaaStringZ;
    }
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFolderName0(const CMaaString &FullPath) // with ending slash, 0-terminating CMaaString
{
    const int n = (warning_int)FullPath.ReverseFind(FILESYSTEM_SLASH);
    return (n >= 0) ? FullPath.Left(n + 1).Str0Copy() : CMaaStringZ;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFolderName2(const CMaaString &FullPath) noexcept // with ending slash, optimized NZT RefLeft CMaaString
{
    const int n = (warning_int)FullPath.ReverseFind(FILESYSTEM_SLASH);
    return (n >= 0) ? FullPath.RefLeft(n + 1) : CMaaStringZ;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFolderName0Any(const CMaaString &FullPath) // with ending slash, 0-terminating CMaaString
{
    const int n = (warning_int)FullPath.ReverseFind(gAnySeparatorBits);
    return (n >= 0) ? FullPath.Left(n + 1).Str0Copy() : CMaaStringZ;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::GetFolderName2Any(const CMaaString &FullPath) noexcept // with ending slash, optimized NZT RefLeft CMaaString
{
    const int n = (warning_int)FullPath.ReverseFind(gAnySeparatorBits);
    return (n >= 0) ? FullPath.RefLeft(n + 1) : CMaaStringZ;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::ToUppercase(CMaaString FileName, int iCodePage)
{
#ifdef _UNICODE
    return iCodePage <= 0 ? FileName.ToUpper() : FileName.ToUpper(e_utf8_rus);
#else
    return iCodePage <= 0 ? FileName.ToUpper() : FileName.ToUpper(1251);
#endif
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::ToLowercase(CMaaString FileName, int iCodePage)
{
#ifdef _UNICODE
    return iCodePage <= 0 ? FileName.ToLower() : FileName.ToLower(e_utf8_rus);
#else
    return iCodePage <= 0 ? FileName.ToLower() : FileName.ToLower(1251);
#endif
}
//------------------------------------------------------------------------------
// class CMaaFile
//------------------------------------------------------------------------------
bool CMaaFile::CreateDir(const CMaaString& path, const CMaaString& DirMode)
{
    CMaaString PathBuffer = MkCompatible(path);
    return CreateDir_Internal(PathBuffer, (warning_int)PathBuffer.Length(), DirMode);
}
//------------------------------------------------------------------------------
bool CMaaFile::CreateDir_Internal(CMaaString &path, int Len, const CMaaString &DirMode)
{
#ifdef _WIN32
    if (MkDir_Internal(path.RefLeft(Len), true, false))
    {
        return true;
    }
    {
        //path = MkCompatible(path);
        const int pos = (warning_int)path.ReverseFind(Len - 1, FILESYSTEM_SLASH);
        if (pos > 0)
        {
            path[pos] = '\0';
            if (CreateDir_Internal(path, pos))
            {
                path[pos] = FILESYSTEM_SLASH;
                if (MkDir_Internal(path.RefLeft(Len), true, false))
                {
                    return true;
                }
            }
        }
    }
#if 0
    DWORD x = GetFileAttributes(_uc_MkCompatible(path));
    if (x != INVALID_FILE_ATTRIBUTES && (x & FILE_ATTRIBUTE_DIRECTORY))
    {
        return true;
    }
    if (CreateDirectory(_uc_MkCompatible(path), nullptr))
    {
        return true;
    }
    DWORD dw = GetLastError();
    if (dw == ERROR_ALREADY_EXISTS/*)
     {
          return true;
     }
     if   (errno == EEXIST */ && (GetFileAttributes(_uc_MkCompatible(path)) & FILE_ATTRIBUTE_DIRECTORY))
    {
        return true;
    }
    //ERROR_PATH_NOT_FOUND
    if (dw)//errno == ENOENT)
    {
        path = MkCompatible(path);
        const int pos = path.ReverseFind(FILESYSTEM_SLASH);
        if (pos > 0)
        {
            if (CreateDir(path.RefLeft(pos)))
            {
                if (CreateDirectory(_uc_MkCompatible(path), nullptr))
                {
                    return true;
                }
            }
        }
    }
#endif
    return false;
#else
    if (IsADir(path.RefLeft(Len)))
    {
        return true;
    }
    if (mkdir(path.RefLeft(Len), false))
    {
        int mode = 0755;
        if (DirMode.IsNotEmpty() && DirMode.dsscanf("%o", &mode) == 1)
        {
            chmod(MkFs_cast(MkFsNativeCompatible(path.RefLeft(Len))), mode);
        }
        return true;
    }
    if (errno == ENOENT)
    {
        //path = MkCompatible(path);
        const int pos = path.ReverseFind(FILESYSTEM_SLASH);
        if (pos > 0)
        {
            path[pos] = '\0';
            if (CreateDir_Internal(path, pos))
            {
                path[pos] = FILESYSTEM_SLASH;
                if (mkdir(path.RefLeft(Len), false))
                {
                    int mode = 0755;
                    if (DirMode.IsNotEmpty() && DirMode.dsscanf("%o", &mode) == 1)
                    {
                        chmod(MkFs_cast(MkFsNativeCompatible(path.RefLeft(Len))), mode);
                    }
                    return true;
                }
            }
        }
    }
    return false;
#endif
}
//------------------------------------------------------------------------------
/*
_dword CMaaFile::fprintf(const char * fmt, ...)
{
     va_list list(
     va_start(list, fmt);
     CMaaString txt;
     txt.FormatV(fmt, list);
     va_end(list);
     return Write(txt);
}
*/
//------------------------------------------------------------------------------
#ifdef __USE_LOG
#endif
//------------------------------------------------------------------------------
CMaaFile * ToolsLib_OpenLog(bool bForcedInit)
{
    static CMaaFile * pLog = nullptr;
    if (bForcedInit || !pLog)
    {
        gCMaaToolLib_crt_Initializer();

        CMaaString FileName;
#ifdef __unix__
        FileName.Format("/var/ram/common/f_glog_%t_%d.txt", time(nullptr), getpid());
#else
        FileName.Format("/var/ram/common/f_glog_%t.txt", time(nullptr));
#endif
        FileName.ReplaceNN(':', '_');
        if (!pLog)
        {
#ifdef TOOLSLIB_KEEP_GLOBALS2_IN_MEMORY
            pLog = TL_NEW
#ifdef __USE_LOG
                CMaaFile(FileName, CMaaFile::eWC_SrSw, eNoExcept);
#else
                CMaaFile;
#endif
#else
        static
#ifdef __USE_LOG
                CMaaFile g(FileName, CMaaFile::eWC_SrSw, eNoExcept);
#else
                CMaaFile g;
#endif
            pLog = &g;
#endif
        }
        else
        {
            *pLog =
#ifdef __USE_LOG
                CMaaFile(FileName, CMaaFile::eWC_SrSw, eNoExcept);
#else
                CMaaFile();
#endif
        }
    }
    return pLog;
}
//------------------------------------------------------------------------------
//CMaaFile f_glog;
static CMaaFile * pTmpLogFile = ToolsLib_OpenLog();
//------------------------------------------------------------------------------
#ifdef _WIN32
CMaaFile CMaaFile::FromHandle(HANDLE h, bool bThrow, int ClosingType)
#else //__unix__
CMaaFile CMaaFile::FromHandle(int h, bool bThrow, int ClosingType)
#endif
{
    f_glog.fprintf("FromHandle(%D, %s, %d)\n", (_qword)h, bThrow ? "true" : "false", ClosingType);
    try
    {
        CMaaFile f(h, bThrow, (sfp *)nullptr);
        if  (ClosingType >= 0 && !f.IsOpen())
        {
            f_glog.fprintf("FromHandle - closing\n");
#ifdef _WIN32
            CloseHandle(h);
#else
            close(h);
#endif
        }
        else
        {
            f_glog.fprintf("FromHandle - setting\n");
            f.SetClosingType(ClosingType);
        }
        f_glog.fprintf("returning\n");
        f_glog.fprintf("f.GetMsg()=%s\n", f.GetMsg());
        return f;
    }
    catch(...)
    {
        f_glog.fprintf("catch(...)\n");
        if  (ClosingType > 0)
        {
            f_glog.fprintf("FromHandle - closing (2)\n");
#ifdef _WIN32
            CloseHandle(h);
#else
            close(h);
#endif
        }
        throw;
    }
}
//------------------------------------------------------------------------------
#ifdef _WIN32
CMaaFile::CMaaFile(HANDLE h, bool bThrow, sfp *)
#else //__unix__
CMaaFile::CMaaFile(int h, bool bThrow, sfp *)
#endif
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(bThrow)
{
    //Clear();
    f_glog.fprintf("CMaaFile::CMaaFile(%D, %s)\n", (_qword)h, bThrow ? "true" : "false");
#ifdef _WIN32
    if  (h == nullptr || h == INVALID_HANDLE_VALUE)
    {
        SetError(0, "CMaaFile::CMaaFile(HANDLE=%p) for not a descriptor handle", (const void *)h);
    }
    else
    {
        m_pImp = TL_NEW CMaaFileImp(h, true);
        if  (!m_pImp)
        {
            SetError(0, "CMaaFile::CMaaFile(HANDLE) : bad alloc of m_pImp");
        }
    }
#else //__unix__
    if  (h == -1) // || h < 0
    {
        f_glog.fprintf("SetError  1\n");
        SetError(0, "CMaaFile::CMaaFile(fd=%d) for not a file descriptor", (int)h);
    }
    else
    {
        m_pImp = TL_NEW CMaaFileImp(h, true);
        f_glog.fprintf("m_pImp=%p\n", m_pImp);
        if  (!m_pImp)
        {
            f_glog.fprintf("m_pImp==nullptr\n");
            SetError(0, "CMaaFile::CMaaFile(fd) : bad alloc of m_pImp");
        }
    }
#endif
    f_glog.fprintf("current error: %s\n", GetMsg());
}
//------------------------------------------------------------------------------
void CMaaFile::SetClosingType(int ClosingType) noexcept
{
    if  (m_pImp)
    {
        m_pImp->SetClosingType(ClosingType);
    }
}
//------------------------------------------------------------------------------
bool CMaaFile::SetError(int Error, const char * Format, ...)
{
    Error = Error == -1 ?
#ifdef _WIN32
        (int)GetLastError()
#else
        errno
#endif
    :   Error;

    CMaaString Msg;
    va_list list;
    va_start(list, Format);
    Msg.FormatV(Format, list);
    va_end(list);

    XTOOFile2Error err(Msg, Error);
    (XTOOFile2Error &)*this = err;
    if  (IsThrow())
    {
        throw err;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CMaaFile::SetError(int Error /* = -1*/ , CMaaString Format, ...)
{
    CMaaString Msg;
    va_list list;
    va_start(list, Format);
    Msg.FormatV(Format, list);
    va_end(list);

    XTOOFile2Error err(Msg, Error);
    (XTOOFile2Error &)*this = err;
    if  (IsThrow())
    {
        throw err;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CMaaFile::StaticError(int Error, bool bThrow, const char * Format, ...)
{
    if  (!bThrow)
    {
        return false;
    }
    Error = Error == -1 ?
#ifdef _WIN32
        (int)GetLastError()
#else
        errno
#endif
    :   Error;

    CMaaString Msg;
    va_list list;
    va_start(list, Format);
    Msg.FormatV(Format, list);
    va_end(list);

    XTOOFile2Error err(Msg, Error);
    throw err;
    return false;
}
//------------------------------------------------------------------------------
bool CMaaFile::StaticError(int Error, bool bThrow, CMaaString Format, ...)
{
    if  (!bThrow)
    {
        return false;
    }
    CMaaString Msg;
    va_list list;
    va_start(list, Format);
    Msg.FormatV(Format, list);
    va_end(list);

    XTOOFile2Error err(Msg, Error);
    throw err;
    return false;
}
//------------------------------------------------------------------------------
CMaaString CMaaFile::GetMode(int Mode) noexcept
{
    try
    {
        return GetModeByModeMask(Mode);
    }
    catch (...)
    {
    }
    return CMaaStringZ;
}
//------------------------------------------------------------------------------
CMaaString CMaaFile::GetModeByModeMask(int mm)
{
    char txt[16];
    char *p = txt;
    if ((mm & eMMR))
    {
        *p++ = 'R';
        if ((mm & eMMp))
        {
            *p++ = '+';
        }
    }
    if ((mm & eMMW)) // && (mm & eMMRp) != eMMRp)
    {
        *p++ = 'W';
    }
    if ((mm & eMMA))
    {
        *p++ = 'A';
    }
    if ((mm & eMMC))
    {
        *p++ = 'C';
    }
    if ((mm & eMMD))
    {
        *p++ = 'D';
    }
    if ((mm & eMMN))
    {
        *p++ = 'N';
    }
    if ((mm & eMMI))
    {
        *p++ = 'I';
    }
    if ((mm & eMMSrSwSd))
    {
        *p++ = '|';
        if ((mm & eMMSr))
        {
            *p++ = 'S';
            *p++ = 'r';
        }
        if ((mm & eMMSw))
        {
            *p++ = 'S';
            *p++ = 'w';
        }
        if ((mm & eMMSd))
        {
            *p++ = 'S';
            *p++ = 'd';
        }
    }
    *p = 0;
    return p == txt ? CMaaStringZ : CMaaString(txt, (p - txt));
}
/*
void TestGMM()
{
        CMaaString m1 = CMaaFile::GetModeByModeMask(-1);
        CMaaString m2 = CMaaFile::GetModeByModeMask(0);
        CMaaString m3 = CMaaFile::GetModeByModeMask(CMaaFile::eMMR | CMaaFile::eMMSrSw);
        __utf8_printf("\"%S\", \"%S\", \"%S\"\n", &m1, &m2, &m3); // output: "R+WACDN|SrSw", "", "R|SrSw"
}
*/
//------------------------------------------------------------------------------
const CMaaString & CMaaFile::GetPrefix(int n) noexcept
{
    return (n > 0 && n < ePfxMax) ? ((const CMaaString&)CMaaFile::C().sPfx[n]) : gsCMaaStringZ;
}
//------------------------------------------------------------------------------
int CMaaFile::GetModeMask(const CMaaString &Mode) noexcept
{
    int fMode = 0;

    const char* p = Mode;
    const char* e = p + Mode.Length();
    while(p < e)
    {
        char c = *p++;
        switch(c)
        {
        case 'R':
            fMode |= eMMR;
            if (p < e && *p == '+')
            {
                fMode |= eMMRp;
                p++;
            }
            break;
        case 'W':
            fMode |= eMMW;
            break;
        case 'C':
            fMode |= eMMC;
            break;
        case 'N':
            fMode |= eMMN;
            break;
        case 'D':
            fMode |= eMMD;
            break;
        case 'A':
            fMode |= eMMA;
            break;
        case 'S':
            if (p < e)
            {
                c = *p;
                if (c == 'r')
                {
                    fMode |= eMMSr;
                    p++;
                }
                else if (c == 'w')
                {
                    fMode |= eMMSw;
                    p++;
                }
                else if (c == 'd')
                {
                    fMode |= eMMSd;
                    p++;
                }
            }
            break;
        case 'I':
            fMode |= eMMI;
            break;
        }
    }
    return fMode;
}
//------------------------------------------------------------------------------
#if 0
CMaaFile::CMaaFile(const char* pszName, eMode Mode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    if (!pszName)
    {
        return;
    }
    if (Mode == eNoMode) // || Mode >= eModeMax)
    {
        return;
    }
    Open(pszFileName2strFileName(pszName), Mode, CMaaStringZ);
}
#endif
//------------------------------------------------------------------------------
CMaaFile::CMaaFile() noexcept // closed, no exceptions
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(false)
{
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString& Name, eMode Mode, eNoExcept_) noexcept
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(false)
{
    try
    {
        Open(Name, Mode, CMaaStringZ);
    }
    catch (...)
    {
        // alloc error
    }
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString& Name, eMode Mode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    /*
    if (Name.IsEmpty())
    {
        return;
    }
    if (Mode == eNoMode)
    {
        return;
    }
    */
    Open(Name, Mode, CMaaStringZ);
}
//------------------------------------------------------------------------------
// Mode: "R", "W", "A", "R+"  + "C" - create  + "SrSw" - share read/write + "D" - create directory
// Reserved File Names: "stdin", "stdout", "stderr"
// To create file skipping using std handle use "|notstdhandle" in pszMode
#if 0
CMaaFile::CMaaFile(const char* pszName, const char* pszMode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    if (!pszName || !pszMode)
    {
        return;
    }
    //Open(pszName, eNoMode, pszMode);
    Open(pszFileName2strFileName(pszName), eNoMode, CMaaString(pszMode, CMaaString::eROStrlenMemString));
}
#endif
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString& Name, const char* pszMode, eNoExcept_) noexcept
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(false)
{
    if (!pszMode)
    {
        return;
    }
    try
    {
        Open(Name, eNoMode, CMaaString(pszMode, CMaaString::eROStrlenMemString));
    }
    catch (...)
    {
        // alloc error
    }
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString& Name, const CMaaString& Mode, eNoExcept_) noexcept
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(false)
{
    try
    {
        Open(Name, eNoMode, Mode);
    }
    catch (...)
    {
        // alloc error
    }
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString &Name, const char* pszMode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    if (!pszMode)
    {
        return;
    }
    Open(Name, eNoMode, CMaaString(pszMode, CMaaString::eROStrlenMemString));
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString &Name, const CMaaString &Mode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    Open(Name, eNoMode, Mode);
}
//------------------------------------------------------------------------------
#if 0
CMaaFile::CMaaFile(const char* pszName, eMode fMode, const char* pszMode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    if (!pszName)
    {
        return;
    }
    Open(pszFileName2strFileName(pszName), fMode, pszMode ? CMaaString(pszMode, CMaaString::eROStrlenMemString) : CMaaStringZ);
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString &Name, eMode fMode, const char* pszMode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    Open(Name, fMode, pszMode ? CMaaString(pszMode, CMaaString::eROStrlenMemString) : CMaaStringZ);
}
#endif
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString& Name, eMode fMode, const CMaaString& strMode, eNoExcept_) noexcept
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(false)
{
    try
    {
        Open(Name, fMode, strMode);
    }
    catch (...)
    {
        // alloc error
    }
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaString& Name, eMode fMode, const CMaaString& strMode, bool fMakeExeption)
:   XTOOFile2Error(eXTOONoError), // cleared err
    m_pImp(nullptr),
    m_bIsThrow(fMakeExeption)
{
    Open(Name, fMode, strMode);
}
//------------------------------------------------------------------------------
bool CMaaFile::Open(const CMaaString &Name1, int /*eMode*/ fMode0, const CMaaString& Mode)
{
    // Name can be memory string // pszFileName2strFileName(psz)
    gCMaaToolLib_crt_Initializer();

    Close();
    Clear();

    if (Name1.IsEmpty())
    {
        return false;
    }
    const int fMode = fMode0 | GetModeMask(Mode);
    if (!(fMode & eMMMask))
    {
        return false;
    }
    CMaaString Name2 = (fMode & eMMI) ? Name1
#ifdef __unix__
        .Str0Copy()
#endif
        : MkFsCompatible(Name1);

#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    const bool bEq = Name2.StrictEq(Name1);
    if (bEq)
    {
        Name2.Empty();
    }
    const CMaaString& Name = bEq ? Name1 : Name2;
#else
    const CMaaString& Name = Name2;
#endif

#ifdef _WIN32
    HANDLE hFile;
#endif
#ifdef __unix__
    int fd;
#endif

    if  ((fMode & eMMN)) // (Mode.Find('N') >= 0)
    {
        if  (IsExists_Internal(Name))
        {
            CMaaString tmp = GetMode(fMode0);
            tmp.Format("Create new file(\"%S\"), mode \"%S%S\": file exists", &Name1, &tmp, &Mode);
            SetError(tmp);
            return false;
        }
    }

    if  ((fMode & eMMD)) // (Mode.Find('D') >= 0)
    {
        const int SlashPos = (warning_int)Name.ReverseFind(FILESYSTEM_SLASH);
        if  (SlashPos > 0)
        {
            CMaaString DirMode;
            const int dm = (warning_int)Mode.Find(0, "DirMode=", 8);
            if  (dm >= 0)
            {
                DirMode = Mode.RefMid(dm + 8);
                DirMode = DirMode.GetWord(",|");
            }
            CMaaString PathBuffer = Name.RefLeft(SlashPos).NewCopy();
            CreateDir_Internal(PathBuffer, (warning_int)PathBuffer.Length(), DirMode);
        }
    }

#ifdef _WIN32
    DWORD dwDesiredAccess = 0;
#endif
#ifdef __unix__
    int flags = 0;
#endif
    if  ((fMode & eMMR)) // (Mode.Find('R') >= 0)
    {
#ifdef _WIN32
        dwDesiredAccess |= GENERIC_READ;
#endif
#ifdef __unix__
        flags |= O_RDONLY;
#endif
    }
    if  ((fMode & (eMMW|eMMp))) //(Mode.Find('W') >= 0 || Mode.Find(0, "R+", 2) >= 0))
    {
#ifdef _WIN32
        dwDesiredAccess |= GENERIC_WRITE;
#endif
#ifdef __unix__
        flags &= ~ O_RDONLY;
        if  ((fMode & eMMR)) //(Mode.Find(0, "R+", 2) >= 0 || Mode.Find('R') >= 0)
        {
            flags |= O_RDWR;
        }
        else
        {
            flags |= O_WRONLY;
        }
#endif
    }
    if  ((fMode & eMMA)) // (Mode.Find('A') >= 0)
    {
#ifdef _WIN32
        dwDesiredAccess |= GENERIC_READ | GENERIC_WRITE;
#endif
#ifdef __unix__
        flags &= ~ (O_RDONLY|O_WRONLY);
        flags |= O_RDWR;
#endif
    }
#ifdef _WIN32
    DWORD dwShareMode = 0;
#endif
#ifdef __unix__
#endif
    if  ((fMode & eMMSr)) // (Mode.Find(0, "Sr", 2) >= 0)
    {
#ifdef _WIN32
        dwShareMode |= FILE_SHARE_READ;
#endif
    }
    if  ((fMode & eMMSw)) // (Mode.Find(0, "Sw", 2) >= 0)
    {
#ifdef _WIN32
        dwShareMode |= FILE_SHARE_WRITE; // | FILE_SHARE_DELETE;
#endif
    }
    if ((fMode & eMMSd)) // (Mode.Find(0, "Sd", 2) >= 0)
    {
#ifdef _WIN32
        dwShareMode |= FILE_SHARE_DELETE;
#endif
    }
#ifdef _WIN32
    DWORD dwCreationDisposition = 0;
#endif
    bool bTruncate = false;
    if  ((fMode & (eMMC|eMMN))) // (Mode.Find('C') >= 0 || Mode.Find('N') >= 0)
    {
        if  ((fMode & (eMMR|eMMW)) == eMMW) // (Mode.Find('R') < 0 && Mode.Find('W') >= 0)
        {
#ifdef _WIN32
            if  ((fMode & eMMN)) // (Mode.Find('N') >= 0)
            {
                dwCreationDisposition = CREATE_NEW;
            }
            else
            {
                dwCreationDisposition = CREATE_ALWAYS;
            }
            const CMaaString n_ = Name.RefRight(CMaaFileNul.Length() + 1);
            if  (n_.Length() != CMaaFileNul.Length() + 1 || (*(const char *)n_ != '\\' && *(const char*)n_ != ':') || !n_.IsMidCi(CMaaFileNul, 1, 0)) // "nul"
#endif
#ifdef __unix__
            flags |= O_CREAT;// | O_TRUNC; //!!! "N" feature
            if  (Name != CMaaFileDevNull) // "/dev/null"
#endif
            {
                bTruncate = true;
            }
        }
        else
        {
#ifdef _WIN32
            dwCreationDisposition = OPEN_ALWAYS;
#endif
#ifdef __unix__
            flags |= O_CREAT;
#endif
        }
    }
    else
    {
#ifdef _WIN32
        dwCreationDisposition = OPEN_EXISTING;
#endif
    }

    CMaaString AutoFileFormatInfo;

#ifdef _WIN32
    // Reserved File Names: "stdin", "stdout", "stderr"
    // To crete file skipping using std handle use "|notstdhandle" in pszMode
    bool bStdHandle = false;
    if  (Mode.Find(0, "|notstdhandle", 13) < 0)
    {
        if  (Name == CMaaFileStdin)
        {
            hFile = GetStdHandle(STD_INPUT_HANDLE);
            bStdHandle = true;
        }
        else if (Name == CMaaFileStdout)
        {
            hFile = GetStdHandle(STD_OUTPUT_HANDLE);
            if  (Mode.Find(0, "+autofileformatinfo", 19) >= 0)
            {
#ifdef _UNICODE
                AutoFileFormatInfo = GetUtf8Prefix();
#endif
            }
            bStdHandle = true;
        }
        else if (Name == CMaaFileStderr)
        {
            hFile = GetStdHandle(STD_ERROR_HANDLE);
            bStdHandle = true;
        }
    }
    if  (!bStdHandle)
    {
        DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL;
        if  (Mode.Find(0, "overlapped", 10) >= 0)
        {
            dwAttributes = dwAttributes | FILE_FLAG_OVERLAPPED;
        }
        CMaaString tmp = MkFsNative(Name);
        hFile = CreateFile(MkFs_cast(tmp), dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, dwAttributes, nullptr);
        if  ((hFile == INVALID_HANDLE_VALUE || !hFile) && (fMode & (eMMA|eMMC)) == (eMMA|eMMC)) // (Mode.Find('A') >= 0 && Mode.Find('C') >= 0)
        {
            hFile = CreateFile(MkFs_cast(tmp), dwDesiredAccess, dwShareMode, nullptr, CREATE_ALWAYS, dwAttributes, nullptr);
        }
    }
    if  (hFile != INVALID_HANDLE_VALUE && hFile)
    {
        try
        {
            m_pImp = TL_NEW CMaaFileImp(hFile, bStdHandle);
        }
        catch (...)
        {
            CloseHandle(hFile);
            throw;
        }
        if (!m_pImp) // alloc error
        {
            CloseHandle(hFile);
            return false;
        }
        // Format C++ }
#endif
#ifdef __unix__
    // Reserved File Names: "stdin", "stdout", "stderr"
    // To crete file skipping using std handle use "|notstdhandle" in pszMode
    bool bStdHandle = false;
    if  (Mode.Find(0, "|notstdhandle", 13) < 0)
    {
        if  (Name == CMaaFileStdin)
        {
            //fd = open("/dev/stdin", flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            //fd = open("/dev/stdin", flags, S_IRUSR|S_IRGRP|S_IROTH);
            fd = stdin ? dup(fileno(stdin)) : -1;
            bStdHandle = true;
        }
        else if (Name == CMaaFileStdout)
        {
            //printf("opening file /dev/stdout mode %d(0x%0x)\n", flags);
            //fd = open("/dev/stdout", flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            //fd = open("/dev/stdout", flags, S_IWUSR|S_IWGRP|S_IWOTH);
            //fd = open("/dev/stdout", O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            //printf("fileno = %d\n", fd);
            fd = stdout ? dup(fileno(stdout)) : -1;
            bStdHandle = true;
        }
        else if (Name == CMaaFileStderr)
        {
            //fd = open("/dev/stderr", flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            //fd = open("/dev/stderr", flags, S_IWUSR|S_IWGRP|S_IWOTH);
            fd = stderr ? dup(fileno(stderr)) : -1;
            bStdHandle = true;
        }
        else if (Name == CMaaFileNull) // "null"
        {
            //fd = open("/dev/stderr", flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            //fd = open("/dev/stderr", flags, S_IWUSR|S_IWGRP|S_IWOTH);
            //fd = stderr ? fileno(stderr) : -1;
            fd = open(/*_uc_MkCompatible(*/ "/dev/null" /*)*/, O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            //flags = O_WRONLY;
            //Name = "/dev/null";
            bStdHandle = true;
        }
    }
    if  (!bStdHandle)
    {
        int mMode, m;
        mMode = m = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        int x = Mode.Find(0, "mode=", 5);
        if (x >= 0)
        {
            x += 5;
        }
        else
        {
            x = Mode.Find(0, "mode", 4);
            if (x >= 0)
            {
                x += 4;
            }
        }
        int x2 = Mode.Find(0, "mode:=", 6);
        if (x2 >= 0)
        {
            x2 += 6;
        }
        if  (x >= 0)
        {
            int nmode = 0;
            // r=4,w=2,e=1
            if  (dsscanf(x + (const char *)Mode, "%o", &nmode) == 1)
            {
                m = nmode & m;
            }
        }
        CMaaString tmp = MkFsNative(Name);
#ifdef __USE_LARGEFILE64
        fd = open64(MkFs_cast(tmp), flags, m);
#else
        fd = open(MkFs_cast(tmp), flags, m);
#endif
        //printf("open(%s, %x, %x) - %d\n", (const char *)_uc_MkCompatible(Name), flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, fd);
        //if  (fd == -1 && ((fMode & eMMA))) // (Mode.Find('A') >= 0)
        //{
        //    fd = open(tmp, flags | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
        //}
        if  (fd >= 0 && x2 >= 0)
        {
            int nmode = 0;
            // r=4,w=2,e=1
            if  (dsscanf(x2 + (const char *)Mode, "%o", &nmode) == 1) // +'2'
            {
                m = nmode & mMode;
            }
            fchmod(fd, m);
        }
    }
    if  (fd != -1)
    {
        try
        {
            m_pImp = TL_NEW CMaaFileImp(fd);
        }
        catch (...)
        {
            close(fd);
            throw;
        }
        if (!m_pImp) // alloc error
        {
            close(fd);
            return false;
        }
#endif
        if  (!bStdHandle)
        {
            if  (bTruncate)
            {
                //printf("Truncating at position %d...\n", (int)GetCurPos());
                Truncate();
                //printf("Length = %d...\n", (int)Length());
            }
            if  ((fMode & eMMA)) // (Mode.Find('A') >= 0)
            {
                try
                {
                    Seek(0, SEEK_END);
                }
                catch(...)
                {
                    if  (!m_pImp->Release())
                    {
#if MANUAL_CMaaFileImp_REFS
                        delete m_pImp;
#endif
                    }
                    m_pImp = nullptr;
                    throw;
                }
            }
        }
        if  (AutoFileFormatInfo.IsNotEmpty() && GetCurPos() == 0)
        {
            try
            {
                Write(AutoFileFormatInfo);
            }
            catch (...)
            {
                Close();
                throw;
            }
        }
        int dstfmt = -1;
        if  (Mode.Find(0, "+convert_to_ansi", 16) >= 0)
        {
            dstfmt = 0;
        }
        else if (Mode.Find(0, "+convert_to_utf8", 16) >= 0)
        {
            dstfmt = ePfxUtf8;
        }
        if  (dstfmt >= 0)
        {
            if  ((fMode & (eMMR|eMMA))) // (Mode.Find('R') >= 0 || Mode.Find('A') >= 0)
            {
                try
                {
                    _qword pos = GetCurPos();
                    Seek(0);
                    CMaaString txt = Read(GetMaxPrefixLength());
                    for (int i = 1; i < ePfxMax; i++)
                    {
                        CMaaString pfx = GetPrefix(i);
                        if  (pfx.IsNotEmpty() && txt.IsLeft(pfx))
                        {
                            m_pImp->m_SrcFormat = i;
                            if  (pos == 0)
                            {
                                pos = pfx.Length();
                            }
                            break;
                        }
                    }
                    Seek(pos);
                    m_pImp->m_DstFormat = dstfmt;
                }
                catch(...)
                {
                    if  (!m_pImp->Release())
                    {
#if MANUAL_CMaaFileImp_REFS
                        delete m_pImp;
#endif
                    }
                    m_pImp = nullptr;
                    throw;
                }
            }
        }
        return true;
    }
    else
    {
        CMaaString tmp = GetMode(fMode0);
        tmp.Format("CreateFile(\"%S\"), mode \"%S%S\"", &Name1, &tmp, &Mode);
        SetError(tmp);
    }
    return false;
}
//------------------------------------------------------------------------------
#ifdef __unix__
//------------------------------------------------------------------------------
bool CMaaFile::dup2(int FileNo)
{
    // 0 - STDIN_FILENO, 1 - STDOUT_FILENO, 2 - STDERR_FILENO
    if  (IsOpened())
    {
        Clear();
        int file_no__ = ::dup2(GetHandle(), FileNo);
        if  (file_no__ < 0)
        {
            CMaaString tmp;
            tmp.Format("dup2()");
            SetError(tmp);
            return false;
        }
        return true;
    }
    SetError("CMaaFile::dup2(): not opened", 0);
    return false;
}
//------------------------------------------------------------------------------
bool CMaaFile::SetCloseOnExec(bool bSet)
{
    if  (IsOpened())
    {
        Clear();
        int fd_flags = fcntl(GetHandle(), F_GETFD);
        if  (fd_flags < 0)
        {
            CMaaString tmp;
            tmp.Format("CMaaFile::SetCloseOnExec() - fcntl(fd, F_GETFD)");
            SetError(tmp);
            f_glog.fprintf("----- SetCloseOnExec(): %s\n", GetMsg());
            return false;
        }
        int new_fd_flags = bSet ? fd_flags | FD_CLOEXEC: fd_flags & ~ FD_CLOEXEC;
        //        new_fd_flags ^= FD_CLOEXEC;
        if  (new_fd_flags != fd_flags)
        {
            if  (fcntl(GetHandle(), F_SETFD, new_fd_flags) < 0)
            {
                CMaaString tmp;
                tmp.Format("CMaaFile::SetCloseOnExec() - fcntl(fd, F_SETFD, 0x%x (%s))", (int)new_fd_flags, new_fd_flags & FD_CLOEXEC ? "+FD_CLOEXEC" : "-FD_CLOEXEC");
                SetError(tmp);
                f_glog.fprintf("----- SetCloseOnExec(): %s\n", GetMsg());
                return false;
            }
            // verification
            int fd_flags_vrf = fcntl(GetHandle(), F_GETFD);
            if  (fd_flags_vrf < 0)
            {
                CMaaString tmp;
                tmp.Format("CMaaFile::SetCloseOnExec() - fcntl(fd, F_GETFD) verification error");
                SetError(tmp);
                f_glog.fprintf("----- SetCloseOnExec(): %s\n", GetMsg());
                return false;
            }
            if  (fd_flags_vrf != new_fd_flags)
            {
                CMaaString tmp;
                tmp.Format("CMaaFile::SetCloseOnExec() - fcntl(fd, F_GETFD) verification failed (%x != %x)", fd_flags_vrf, new_fd_flags);
                SetError(tmp);
                f_glog.fprintf("----- SetCloseOnExec(): %s\n", GetMsg());
                return false;
            }
            f_glog.fprintf("----- SetCloseOnExec(): fd_flags are verified\n");
            // end of verification

            f_glog.fprintf("----- SetCloseOnExec() - %sok\n", (fd_flags & FD_CLOEXEC) ? "++" : "--");
        }
        f_glog.fprintf("----- SetCloseOnExec() - %cok\n", (fd_flags & FD_CLOEXEC) ? '+' : '-');
        return true;
    }
    SetError("CMaaFile::SetCloseOnExec(): not opened", 0);
    f_glog.fprintf("----- SetCloseOnExec(): %s\n", GetMsg());
    return false;
}
//------------------------------------------------------------------------------
bool unix_StartProcess(const char * ExecFileName, const char ** args, const char ** envs, bool bThrow, int Flags, bool bEJudge, int MemLimit, int TimeLimitMs)
{
    // extern char ** environ;
    const char * pDefArgs[2] = {ExecFileName, nullptr};
    args = args ? args : pDefArgs;
    envs = envs ? envs : (const char **)environ;

    struct rlimit lim;
    int p1, res1;
    if  (MemLimit > 0)
    {
        lim.rlim_cur = MemLimit; // устанавливаем ограничение mtime миллисекунд
        lim.rlim_max = MemLimit; // устанавливаем ограничение mtime миллисекунд
        res1 = setrlimit(p1 = RLIMIT_AS, &lim);
        //printf("setrlimit(%d) returns %d\n", p1, res1);
    }
    if  (bEJudge && TimeLimitMs > 0)
    {
        lim.rlim_cur = TimeLimitMs; // устанавливаем ограничение mtime миллисекунд
        lim.rlim_max = TimeLimitMs; // устанавливаем ограничение mtime миллисекунд
        res1 = setrlimit(p1 = 19, &lim);
        printf("setrlimit(%d) returns %d\n", p1, res1);
    }

    if  (bEJudge)
    {
#ifndef ___ToolsLibQT___
        ptrace((__ptrace_request)0x4280, 0, 0, 0); // Для включения диагностики ошибки Memory Limit Exceeded
        ptrace((__ptrace_request)0x4282, 0, 0, 0); // Для включения диагностики ошибки time-limit exceeded
        int n1 = ptrace((__ptrace_request)0x4281, 0, 0, 0);
        //int n1=0;
        if  (n1 == 0)
        {
            //printf("ptrace(0x4281, 0, 0, 0) - ok (safe mode - on)\n", nullptr);
        }
        else
        {
            //printf("ptrace(0x4281, 0, 0, 0) returns %d\n", n1);
        }
#endif
    }

    //void CloseAllFds()
    if ((Flags & (CMaaFile::eCloseFds | CMaaFile::eCloseAllFds)))
    {
        long x = sysconf(_SC_OPEN_MAX);
        if ((Flags & CMaaFile::eCloseAllFds))
        {
            while (--x >= 0)
            {
                close(x);
            }
        }
        else
        {
            while (--x > 2)
            {
                close(x);
            }
        }
    }

    if  (execve(ExecFileName, (char **)args, (char **)envs) != -1)
    {
        return true;
    }
    if  (bThrow)
    {
        return CMaaFile::StaticError(-1, bThrow, "execve(%s, , )", ExecFileName);
    }
    return false;
}
//------------------------------------------------------------------------------
bool CMaaFile::StaticSetCloseOnExec(int fd, bool bSet, bool bThrow)
{
    if  (fd >= 0)
    {
        int fd_flags = fcntl(fd, F_GETFD);
        if  (fd_flags < 0)
        {
            f_glog.fprintf("----- StaticSetCloseOnExec() - err1\n");
            return CMaaFile::StaticError(-1, bThrow, "CMaaFile::StaticSetCloseOnExec() - fcntl(fd, F_GETFD)");
        }
        int new_fd_flags = bSet ? fd_flags | FD_CLOEXEC: fd_flags & ~ FD_CLOEXEC;
        //        new_fd_flags ^= FD_CLOEXEC;
        if  (new_fd_flags != fd_flags)
        {
            if  (fcntl(fd, F_SETFD, new_fd_flags) < 0)
            {
                f_glog.fprintf("----- StaticSetCloseOnExec() - err2\n");
                return CMaaFile::StaticError(-1, bThrow, "CMaaFile::StaticSetCloseOnExec() - fcntl(fd, F_SETFD, 0x%x (%s))", (int)new_fd_flags, new_fd_flags & FD_CLOEXEC ? "+FD_CLOEXEC" : "-FD_CLOEXEC");
            }
            // verification
            int fd_flags_vrf = fcntl(fd, F_GETFD);
            if  (fd_flags_vrf < 0)
            {
                f_glog.fprintf("----- StaticSetCloseOnExec() - fcntl(fd, F_GETFD) verification error\n");
            }
            if  (fd_flags_vrf != new_fd_flags)
            {
                f_glog.fprintf("----- StaticSetCloseOnExec() - fcntl(fd, F_GETFD) verification failed (%x != %x)", fd_flags_vrf, new_fd_flags);
            }
            f_glog.fprintf("----- StaticSetCloseOnExec(): fd_flags are verified\n");
            // end of verification

            f_glog.fprintf("----- StaticSetCloseOnExec() - %sok\n", (fd_flags & FD_CLOEXEC) ? "++" : "--");
        }
        f_glog.fprintf("----- StaticSetCloseOnExec() - %cok\n", (fd_flags & FD_CLOEXEC) ? '+' : '-');
        return true;
    }
    f_glog.fprintf("----- StaticSetCloseOnExec() - err3\n");
    return CMaaFile::StaticError(0, bThrow, "CMaaFile::StaticSetCloseOnExec(): fd not opened");
}
//------------------------------------------------------------------------------
#ifdef TOOLSLIB_SINGLE_THREAD
//------------------------------------------------------------------------------
int SendEmail(CMaaString txt, const char ** pargs, bool bThrow, bool * pRet) // pargs[0] is reserved for exec file name ("/usr/sbin/sendmail")
{
    int ExitCode = 1;
    if (bThrow)
    {
        XTOOFile2Error err("UNIMPLEMENTED", 1);
        throw err;
    }
    return ExitCode;
}
//------------------------------------------------------------------------------
#else // TOOLSLIB_SINGLE_THREAD
//------------------------------------------------------------------------------
DEF_ALLOCATOR(C_os_StartProcess::Writer)
DEF_ALLOCATOR(C_os_StartProcess::Reader)
C_os_StartProcess::Pipe::Pipe(bool bCreate)
{
    fd[0] = fd[1] = -1;
    m_sigignorecount = 0;
    if  (bCreate)
    {
        Create();
    }
}
//------------------------------------------------------------------------------
bool C_os_StartProcess::Pipe::Create()
{
    if  (fd[0] != -1 || fd[1] != -1)
    {
        return false;
    }
    fd[0] = fd[1] = -1;
    if  (pipe(fd) < 0)
    {
        fd[0] = fd[1] = -1;
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
C_os_StartProcess::Pipe::~Pipe()
{
    Close();
}
//------------------------------------------------------------------------------
void C_os_StartProcess::Pipe::CloseAt_vfork() // preserve data in child
{
    if  (fd[0] >= 0)
    {
        close(fd[0]);
    }
    //fd[0] = -1;
    if  (fd[1] >= 0)
    {
        close(fd[1]);
    }
    //fd[1] = -1;
    //if  (m_sigignorecount > 0)
    //{
    //    RestoreSignals();
    //}
}
//------------------------------------------------------------------------------
void C_os_StartProcess::Pipe::Close()
{
    if  (fd[0] >= 0)
    {
        close(fd[0]);
    }
    fd[0] = -1;
    if  (fd[1] >= 0)
    {
        close(fd[1]);
    }
    fd[1] = -1;
    if  (m_sigignorecount > 0)
    {
        RestoreSignals();
    }
}
//------------------------------------------------------------------------------
bool C_os_StartProcess::Pipe::SetHandleIsInherited(int Num, bool bSet, bool bThrow)
{
    return CMaaFile::StaticSetCloseOnExec(fd[Num & 1], !bSet, bThrow);
}
//------------------------------------------------------------------------------
bool C_os_StartProcess::Pipe::ItIsASide(int Num)
{
    Num &= 1;
    if  (fd[Num] == -1)
    {
        return false;
    }
    Num ^= 1;
    if  (fd[Num] != -1)
    {
        close(fd[Num]);
        fd[Num] = -1;
    }
    return true;
}
//------------------------------------------------------------------------------
int C_os_StartProcess::Pipe::GetHandle(int Num, bool bExtract)
{
    Num &= 1;
    int _fd = fd[Num];
    if  (bExtract)
    {
        fd[Num] = -1;
    }
    return _fd;
}
//------------------------------------------------------------------------------
static void _pipe_OnMySIGPIPE(int /*x*/)
{
    signal(SIGPIPE, _pipe_OnMySIGPIPE);
    return;
}
//------------------------------------------------------------------------------
static void _pipe_OnMySIGBUS(int /*x*/)
{
    signal(SIGBUS, _pipe_OnMySIGBUS);
    return;
}
//------------------------------------------------------------------------------
void C_os_StartProcess::Pipe::IgnoreSignals()
{
    if  (++m_sigignorecount == 1)
    {
        m_sig_pipe = signal(SIGPIPE, _pipe_OnMySIGPIPE);
        m_sig_bus = signal(SIGBUS, _pipe_OnMySIGBUS);
    }
}
//------------------------------------------------------------------------------
void C_os_StartProcess::Pipe::RestoreSignals()
{
    if  (--m_sigignorecount == 0)
    {
        signal(SIGPIPE, m_sig_pipe);
        signal(SIGBUS, m_sig_bus);
    }
    else
    {
        //  ++m_sigignorecount;
    }
}
//------------------------------------------------------------------------------
C_os_StartProcess::Writer::Writer(CMaaString Data, int fd)
:   m_Data(Data),
    m_File(CMaaFile::FromHandle(fd, true, 1))
{
}
//------------------------------------------------------------------------------
C_os_StartProcess::Writer::~Writer()
{
    Join();
}
//------------------------------------------------------------------------------
unsigned C_os_StartProcess::Writer::Run()
{
    try
    {
        C_os_StartProcess::Pipe p(false);
        p.IgnoreSignals();
        while(m_Data.IsNotEmpty())
        {
            _dword x = m_File.Write(m_Data, m_Data.Length());
            //f_glog.fprintf("f.Write %d bytes\n", (int)x);
            if  (x > 0)
            {
                m_Data = m_Data.RefMid((int)x);
            }
        }
        m_File.Close();
        return 0;
    }
    catch(...)
    {
    }
    m_File.Close();
    return 1;
}
//------------------------------------------------------------------------------
C_os_StartProcess::Reader::Reader(CMaaString &Data, int fd)
:   m_Data(Data),
    m_File(CMaaFile::FromHandle(fd, true, 1)),
    m_Buffer(128 * 1024)
{
}
//------------------------------------------------------------------------------
C_os_StartProcess::Reader::~Reader()
{
    Join();
}
//------------------------------------------------------------------------------
unsigned C_os_StartProcess::Reader::Run()
{
    try
    {
        C_os_StartProcess::Pipe p(false);
        p.IgnoreSignals();
        while(1)
        {
            int x = m_File.Read(m_Buffer, (int)m_Buffer.Size());
            if  (x > 0)
            {
                m_strData.Add(m_Buffer, (int)x);
            }
            else
            {
                break;
            }
        }
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        m_Data = (CMaaString)m_strData;
        Locker_UnLock;
        m_File.Close();
        m_Buffer.Resize(0);
        return 0;
    }
    catch(...)
    {
    }
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    try
    {
        m_Data = (CMaaString)m_strData;
    }
    catch(...)
    {
    }
    Locker_UnLock;
    m_File.Close();
    m_Buffer.Resize(0);
    return 1;
}
//------------------------------------------------------------------------------
int SendEmail(CMaaString txt, const char ** pargs, bool bThrow, bool * pRet) // pargs[0] is reserved for exec file name ("/usr/sbin/sendmail")
{
    const char * p0 = pargs[0];
    pargs[0] = "/usr/sbin/sendmail";
    int ExitCode = -1;
    if  (pRet)
    {
        *pRet = false;
    }

    bool Ret = false;

    try
    {
        C_os_StartProcess proc;
        proc.m_StdInSend = txt;
        proc.m_fdStdIn = -3;
        proc.m_fdStdOut = -1;
        //proc.m_fdStdErr = fdStdErr;

        Ret = proc.fork_and_StartProcess(C_os_StartProcess::eWaitChild/*|C_os_StartProcess::eGetUsage*/, pargs[0], pargs, nullptr, bThrow); //envs, bThrow);
        ExitCode = Ret ? proc.GetExitCode() : -1;
        pargs[0] = p0;
        if  (pRet)
        {
            *pRet = Ret;
        }
        if  (Ret && bThrow && ExitCode)
        {
            CMaaFile::StaticError(ExitCode, bThrow, "SendEmail()");
            //XTOOFile2Error err("SendEmail() failed: ", ExitCode);
            //throw err;
        }
        if  (!Ret && bThrow)
        {
            CMaaFile::StaticError(0, bThrow, "SendEmail() error");
            //XTOOFile2Error err("SendEmail() failed: ", ExitCode);
            //throw err;
        }
    }
    catch(XTOOFile2Error err)
    {
        ExitCode = err.GetErrorCode() ? err.GetErrorCode() : -1;
        pargs[0] = p0;
        if  (bThrow)
        {
            throw;
        }
    }
    catch(...)
    {
        ExitCode = -1;
        pargs[0] = p0;
        if  (bThrow)
        {
            throw;
        }
    }
    return ExitCode;
}
//------------------------------------------------------------------------------
#define USE_VFORK
#ifdef ___ToolsLibQT___
bool C_os_StartProcess::fork_and_StartProcess(int Flags, const char * /*ExecFileName*/, const char ** /*args*/, const char ** /*envs*/, bool bThrow)
#else
bool C_os_StartProcess::fork_and_StartProcess(int Flags, const char * ExecFileName, const char ** args, const char ** envs, bool bThrow)
#endif
{
    m_Flags = Flags;

    m_ChildProcessId = -1;
    m_Status = 0;
    m_Options = 0;
    memset(&m_rusage, 0, sizeof(m_rusage));
    m_ExitCode = -1;
    m_Errno = 0;

#ifdef ___ToolsLibQT___
    m_Errno = 1;
    return CMaaFile::StaticError(1, bThrow, "fork_and_StartProcess()");
#else

    CMaaString ExecFileNameWithPath;
    if  (Flags & eSearchPath)
    {
        //const char * ExecFileName = "ffmpeg";
        CMaaString cmd(ExecFileName);
        if  (cmd[0] != '.' && cmd[0] != '/' && cmd.Find('/') < 0)
        {
            const char * path = getenv("PATH");
            CMaaString Path(path ? path : "");
            while(Path.IsNotEmpty())
            {
                CMaaString p = Path.GetWord(':', true, false);
                CMaaString e = p + "/" + cmd;
                if  (CMaaFile::IsAFile(e))
                {
                    struct stat s;
                    if  (!stat(e, &s))
                    {
                        if  (s.st_mode & 0111)
                        {
                            ExecFileNameWithPath = e;
                            ExecFileName = ExecFileNameWithPath;
                            break;
                        }
                    }
                }
            }
        }
    }

#define log f_glog

    if  (Flags & eProtect)
    {
        struct stat s;
        if  (stat(ExecFileName/*_uc_MkCompatible(FileName)*/, &s) == 0)
        {
            //int ModeMask = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH;
            int ModeMask = S_IWGRP|S_IWOTH;
            if  (s.st_mode & ModeMask)
            {
                CMaaString TextMode = "mode=";
                int Bits[] =
                {
                    S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP,S_IXGRP,S_IROTH,S_IWOTH,S_IXOTH, 0
                };
                const char * chars = "rwxrwxrwx";
                int i;
                for (i = 0; Bits[i]; i++)
                {
                    char ch2[2] = {'-', 0};
                    if  ((s.st_mode & Bits[i]))
                    {
                        ch2[0] = chars[i];
                    }
                    TextMode += ch2;
                }

                TextMode += ", mask=";
                for (i = 0; Bits[i]; i++)
                {
                    char ch2[2] = {'-', 0};
                    if  ((ModeMask & Bits[i]))
                    {
                        ch2[0] = chars[i];
                    }
                    TextMode += ch2;
                }

                TextMode += ", disallowed:";
                for (i = 0; Bits[i]; i++)
                {
                    char ch2[2] = {'-', 0};
                    if  ((s.st_mode & ModeMask & Bits[i]))
                    {
                        ch2[0] = chars[i];
                    }
                    TextMode += ch2;
                }
                return CMaaFile::StaticError(0, bThrow, "fork_and_StartProcess(%s), %S", ExecFileName, &TextMode);
            }
        }
    }

    char pszExecFileName[MAXPATHLEN + 1];
    if (strlen(ExecFileName) >= sizeof(pszExecFileName))
    {
        return CMaaFile::StaticError(0, bThrow, "fork_and_StartProcess(%s), too long filename", ExecFileName);
    }
    strcpy(pszExecFileName, ExecFileName);

    int fd[3] =
    {
        m_fdStdIn  == -2 && stdin  ? dup(fileno(stdin))  : m_fdStdIn  < -3 || m_fdStdIn  >= 0 ? dup(m_fdStdIn)  : (int)m_fdStdIn,
        m_fdStdOut == -2 && stdout ? dup(fileno(stdout)) : m_fdStdOut < -3 || m_fdStdOut >= 0 ? dup(m_fdStdOut) : (int)m_fdStdOut,
        m_fdStdErr == -2 && stderr ? dup(fileno(stderr)) : m_fdStdErr < -3 || m_fdStdErr >= 0 ? dup(m_fdStdErr) : (int)m_fdStdErr
    };
    //static const char * fdnames[3] =
    //{
    //    "stdin", "stdout", "stderr"
    //};
    {
        for (int i = 0; i < 3; i++)
        {
            if  (m_pRW[i])
            {
                delete m_pRW[i];
                m_pRW[i] = nullptr;
            }
        }
    }
    //Pipe _i(false), _o(false), _e(false);
    //Pipe * _ioe[3] = {&_i, &_o, &_e};
    _ioe[0] = &_i;
    _ioe[1] = &_o;
    _ioe[2] = &_e;
    m_bFdPipe[0] = m_bFdPipe[1] = m_bFdPipe[2] = false;
    //m_pRW[0] = m_pRW[1] = m_pRW[2] = nullptr;

    int i;
    for (i = 0; i < 3; i++)
    {
        if  (fd[i] == -3)
        {
            _ioe[i]->Create();
            //            _ioe[i]->SetHandleIsInherited(0, true, false);
            //            _ioe[i]->SetHandleIsInherited(1, true, false);
            log.fprintf("%d. (-3 fd) %d %d\n", i, _ioe[i]->GetHandle(0, false), _ioe[i]->GetHandle(1, false));
            fd[i] = _ioe[i]->GetHandle(i == 0 ? 0 : 1, true);
            m_bFdPipe[i] = true;
        }
        else if (fd[i] >= 0)
        {
            try
            {
                CMaaFile::StaticSetCloseOnExec(fd[i], false, true);
            }
            catch(XTOOFile2Error err)
            {
                CMaaString tmp;
                tmp.Format("fork_and_StartProcess() - %s", (const char *)err.GetMsg());
                err.SetMsg(tmp);
                f_glog.fprintf("%s\n", (const char *)tmp);
                //throw;
            }
        }
        else
        {
            fd[i] = fd[i] >= -10 ? -1 : fd[i];
        }
        log.fprintf("fd[%d] = %d\n", i, fd[i]);
    }

    m_Flags = Flags;

    m_ChildProcessId = -1;
    m_Status = 0;
    memset(&m_rusage, 0, sizeof(m_rusage));
    m_ExitCode = -1;

    f_glog.SetCloseOnExec(false);
    f_glog.fprintf("test 2\n");

    {
        for (int i = 0; i < 3; i++)
        {
            if  (fd[i] >= 0)
            {
                CMaaFile::StaticSetCloseOnExec(fd[i], false, false);
            }
        }
    }
    gettimeofday(&m_tv0, nullptr);

    int err = 0;

#ifdef USE_VFORK
    pid_t pid = vfork();
#else
    pid_t pid = fork();
    if  (pid == 0)
    {
        ToolsLib_OpenLog(true);
    }
    f_glog.fprintf("pid=%d 1\n", (int)pid);
#endif
    m_ChildProcessId = pid;

    if  (pid)
    {
        err = errno;
        for (i = 0; i < 3; i++)
        {
            if  (fd[i] >= 0)// && _ioe[i]->GetHandle(0, false) < 0)
            {
                close(fd[i]);
                //fd[i] = -1;
            }
        }
    }

    if  (pid > 0)
    {
        // parent process
        err = 0;
        int errcode = 0;

        if  (m_bFdPipe[0])
        {
            _ioe[0]->ItIsASide(1);
            //_ioe[0]->IgnoreSignals();
            m_pRW[0] = TL_NEW Writer(m_StdInSend, _ioe[0]->GetHandle(1, true));
            _ioe[0]->Close();
            m_pRW[0] && m_pRW[0]->Create();
        }
        if  (m_bFdPipe[1])
        {
            _ioe[1]->ItIsASide(0);
            //_ioe[1]->IgnoreSignals();
            m_pRW[1] = TL_NEW Reader(m_StdOutRecv, _ioe[1]->GetHandle(0, true));
            _ioe[1]->Close();
            m_pRW[1] && m_pRW[1]->Create();
        }
        if  (m_bFdPipe[2])
        {
            _ioe[2]->ItIsASide(0);
            //_ioe[2]->IgnoreSignals();
            m_pRW[2] = TL_NEW Reader(m_StdErrRecv, _ioe[2]->GetHandle(0, true));
            _ioe[2]->Close();
            m_pRW[2] && m_pRW[2]->Create();
        }
        //_ioe[i]->RestoreSignals();

        if  (Flags & eWaitChild)
        {
            int Status = 0;
            int options = 0;
            m_Options = options;
            pid_t x;
            do
            {
                if  (Flags & eGetUsage)
                {
                    x = wait4(pid, &Status, options, &m_rusage);
                }
                else
                {
                    x = waitpid(pid, &Status, options);
                }
            } while(x==-1 && errno == EINTR);
            int eeee = err = x == -1 ? errno : 0;

            gettimeofday(&m_tv1, nullptr);
            m_rtv.tv_sec = m_tv1.tv_sec - m_tv0.tv_sec;
            m_rtv.tv_usec = m_tv1.tv_usec - m_tv0.tv_usec;
            if  (m_rtv.tv_usec < 0)
            {
                m_rtv.tv_sec--;
                m_rtv.tv_usec += 1000000;
            }

            int Status_simple = Status & 0xffff; //Status;// 07.11.2015// & 0xffff;
            m_Status = Status_simple;
            m_StatusEx = Status;
            //!!2019 f_glog.fprintf("\n\n\nwait() returns %d, WEXITSTATUS(%d) = %d\n\n", x, Status, WEXITSTATUS(Status_simple));
            f_glog.fprintf("\n\n\nwait() returns %d, WEXITSTATUS(%d) = %d\n\n", x, Status, WEXITSTATUS(Status));
            /*if   (x != -1)
            {
                m_ExitCode = WEXITSTATUS(Status_simple);
                //printf("m_ExitCode = %d\n", m_ExitCode);
            }*/
            if  (x == pid && WIFEXITED(Status))
            {
                err = m_ExitCode = WEXITSTATUS(Status);
                f_glog.fprintf("m_ExitCode = %d\n", m_ExitCode);
            }
            else //if  (x != pid || !WIFEXITED(Status))
            {
                if  (WIFSIGNALED(Status))
                {
                    //"child died with signal %u", WTERMSIG(Status);
                    int s = WTERMSIG(Status);
                    f_glog.fprintf("process %d is died with %d signal\n", (int)pid, (int)s);
                    m_ExitCode = s != 9 ? 124 : 128+9; // like: timeout 0.3 ./abcd.sh ; a=$?; echo $a
                    // s + 256;
                    if  (WCOREDUMP(Status))
                    {
                        f_glog.fprintf("Core dumped\n");
                    }
                    //if (m_StdOutRecv.IsEmpty() && m_StdErrRecv.IsEmpty())
                    {
                        err = m_ExitCode;
                    }
                }
                else if (WEXITSTATUS(Status) != 0)
                {
                    //"child exited with non-zero exit status %u", WEXITSTATUS(Status));
                    m_ExitCode = WEXITSTATUS(Status);
                    f_glog.fprintf("m_ExitCode = %d\n", m_ExitCode);
                    //if (m_ExitCode && m_StdOutRecv.IsEmpty() && m_StdErrRecv.IsEmpty())
                    {
                        err = m_ExitCode;
                    }
                }
                else if (WIFSTOPPED(Status))
                {
                    int s = WSTOPSIG(Status);
                    f_glog.fprintf("process %d is stopped by %d signal\n", (int)pid, (int)s);
                    err = m_ExitCode = -1;
                }
                else if (WIFCONTINUED(Status))
                {
                    f_glog.fprintf("process %d is continued\n", (int)pid);
                    err = m_ExitCode = -1;
                }
                else
                {
                    //waitpid() failed errno
                    err = eeee;
                    //m_ExitCode = eeee;
                }
            }
#if 0
            if  (x > 0)
            {
                if  (WIFEXITED(Status))
                {
                    //m_ExitCode = WEXITSTATUS(Status_simple);
                    m_ExitCode = WEXITSTATUS(Status);
                    f_glog.fprintf("m_ExitCode = %d\n", m_ExitCode);
                }
                else if (WIFSIGNALED(Status))
                {
                    int s = WTERMSIG(Status);
                    f_glog.fprintf("process %d is killed by %d signal\n", (int)pid, (int)s);
                    m_ExitCode = s != 9 ? 124 : 128+9; // like: timeout 0.3 ./abcd.sh ; a=$?; echo $a
                    // s + 256;
                    if  (WCOREDUMP(Status))
                    {
                        f_glog.fprintf("Core dumped\n");
                    }
                }
                else if (WIFSTOPPED(Status))
                {
                    int s = WSTOPSIG(Status);
                    f_glog.fprintf("process %d is stopped by %d signal\n", (int)pid, (int)s);
                }
                else if (WIFCONTINUED(Status))
                {
                    f_glog.fprintf("process %d is continued\n", (int)pid);
                }
                else
                {
                    f_glog.fprintf("x = %d, status = 0x%08X\n", (int)x, (int)Status);
                }
            }
            else
            {
                f_glog.fprintf("x = %d (<= 0?)\n", (int)x);
            }
#endif // 0
            for (int i = 0; i < 3; i++)
            {
                if  (m_bFdPipe[i])
                {
                    delete m_pRW[i];
                    m_pRW[i] = nullptr;
                }
            }
            /*if (bThrow && m_ExitCode > 0)
            {
                return CMaaFile::StaticError(m_ExitCode, bThrow, "fork_and_StartProcess(%s)", pszExecFileName);
            }*/
        }
    }
    else if (pid == 0)
    {
        //CMaaFile f_log("/var/ram/maa/log___1.txt", CMaaFile::eWCD_SrSw, eNoExcept);
        //CMaaFile f_log;
        int f_log;
        int std_fileno[3] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
        int child_fd[3];
        for (int i = 0; i < 3; i++)
        {
            //f_log.fprintf("fd[%d]=%d\n", i, fd[i]);
            child_fd[i] = -1;
            if  (fd[i] < -1)
            {
                close(std_fileno[i]);
                //f_log.fprintf("close(%d)\n", std_fileno[i]);
            }
            else if (fd[i] >= 0)
            {
                child_fd[i] = ::dup2(fd[i], std_fileno[i]);
                close(fd[i]);
            }
            else
            {
                close(std_fileno[i]);
                //child_fd[i] = std_fileno[i];
            }
            //log.fprintf("%d. child_fd[i] = %d\n", i, child_fd[i]);
#ifdef USE_VFORK
            _ioe[i]->CloseAt_vfork();
#else
            _ioe[i]->Close();
#endif
        }

        int err = -1; // unknown error
        if  (!unix_StartProcess(pszExecFileName, args, envs, false, Flags, m_bEJudge, m_MemLimit, m_TimeLimitMs))
        {
            err = errno;
            //printf("child: err = %d\n", err);
        }
        if  (err == ENOENT)
        {
        }
        _exit(err);
    }

    m_Errno = err;
    if  (err)
    {
        return CMaaFile::StaticError(err, bThrow, "fork_and_StartProcess(%s)", pszExecFileName);
    }
    return true;
#endif // ___ToolsLibQT___
}
//------------------------------------------------------------------------------
#ifdef ___ToolsLibQT___
bool C_os_StartProcess::fork_and_StartProcess_old(int Flags, const char * /*ExecFileName*/, const char ** /*args*/, const char ** /*envs*/, bool bThrow)
#else
bool C_os_StartProcess::fork_and_StartProcess_old(int Flags, const char * ExecFileName, const char ** args, const char ** envs, bool bThrow)
#endif
{
    m_Flags = Flags;

    m_ChildProcessId = -1;
    m_Status = 0;
    m_Options = 0;
    memset(&m_rusage, 0, sizeof(m_rusage));
    m_ExitCode = -1;
    m_Errno = 0;

#ifdef ___ToolsLibQT___
    m_Errno = 1;
    return CMaaFile::StaticError(1, bThrow, "fork_and_StartProcess()");
#else

    CMaaString ExecFileNameWithPath;
    if  (Flags & eSearchPath)
    {
        //const char * ExecFileName = "ffmpeg";
        CMaaString cmd(ExecFileName);
        if  (cmd[0] != '.' && cmd[0] != '/' && cmd.Find('/') < 0)
        {
            const char * path = getenv("PATH");
            CMaaString Path(path ? path : "");
            while(Path.IsNotEmpty())
            {
                CMaaString p = Path.GetWord(':', true, false);
                CMaaString e = p + "/" + cmd;
                if  (CMaaFile::IsAFile(e))
                {
                    struct stat s;
                    if  (!stat(e, &s))
                    {
                        if  (s.st_mode & 0111)
                        {
                            ExecFileNameWithPath = e;
                            ExecFileName = ExecFileNameWithPath;
                            break;
                        }
                    }
                }
            }
        }
    }

#define log f_glog

    if  (Flags & eProtect)
    {
        struct stat s;
        if  (stat(ExecFileName/*_uc_MkCompatible(FileName)*/, &s) == 0)
        {
            //int ModeMask = S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH;
            int ModeMask = S_IWGRP|S_IWOTH;
            if  (s.st_mode & ModeMask)
            {
                CMaaString TextMode = "mode=";
                int Bits[] =
                {
                    S_IRUSR,S_IWUSR,S_IXUSR,S_IRGRP,S_IWGRP,S_IXGRP,S_IROTH,S_IWOTH,S_IXOTH, 0
                };
                const char * chars = "rwxrwxrwx";
                int i;
                for (i = 0; Bits[i]; i++)
                {
                    char ch2[2] = {'-', 0};
                    if  ((s.st_mode & Bits[i]))
                    {
                        ch2[0] = chars[i];
                    }
                    TextMode += ch2;
                }

                TextMode += ", mask=";
                for (i = 0; Bits[i]; i++)
                {
                    char ch2[2] = {'-', 0};
                    if  ((ModeMask & Bits[i]))
                    {
                        ch2[0] = chars[i];
                    }
                    TextMode += ch2;
                }

                TextMode += ", disallowed:";
                for (i = 0; Bits[i]; i++)
                {
                    char ch2[2] = {'-', 0};
                    if  ((s.st_mode & ModeMask & Bits[i]))
                    {
                        ch2[0] = chars[i];
                    }
                    TextMode += ch2;
                }
                return CMaaFile::StaticError(0, bThrow, "fork_and_StartProcess(%s), %S", ExecFileName, &TextMode);
            }
        }
    }

    int fd[3] =
    {
        m_fdStdIn  == -2 && stdin  ? dup(fileno(stdin))  : m_fdStdIn  < -3 || m_fdStdIn  >= 0 ? dup(m_fdStdIn)  : (int)m_fdStdIn,
        m_fdStdOut == -2 && stdout ? dup(fileno(stdout)) : m_fdStdOut < -3 || m_fdStdOut >= 0 ? dup(m_fdStdOut) : (int)m_fdStdOut,
        m_fdStdErr == -2 && stderr ? dup(fileno(stderr)) : m_fdStdErr < -3 || m_fdStdErr >= 0 ? dup(m_fdStdErr) : (int)m_fdStdErr
    };
    //static const char * fdnames[3] =
    //{
    //    "stdin", "stdout", "stderr"
    //};

    Pipe _i(false), _o(false), _e(false);
    Pipe * _ioe[3] = {&_i, &_o, &_e};

    int i;
    for (i = 0; i < 3; i++)
    {
        if  (fd[i] == -3)
        {
            _ioe[i]->Create();
            //            _ioe[i]->SetHandleIsInherited(0, true, false);
            //            _ioe[i]->SetHandleIsInherited(1, true, false);
            log.fprintf("%d. (-3 fd) %d %d\n", i, _ioe[i]->GetHandle(0, false), _ioe[i]->GetHandle(1, false));
            fd[i] = _ioe[i]->GetHandle(i == 0 ? 0 : 1, true);
        }
        else if (fd[i] >= 0)
        {
            try
            {
                CMaaFile::StaticSetCloseOnExec(fd[i], false, true);
            }
            catch(XTOOFile2Error err)
            {
                CMaaString tmp;
                tmp.Format("fork_and_StartProcess() - %s", (const char *)err.GetMsg());
                err.SetMsg(tmp);
                f_glog.fprintf("%s\n", (const char *)tmp);
                //throw;
            }
        }
        else
        {
            fd[i] = fd[i] >= -10 ? -1 : fd[i];
        }
        log.fprintf("fd[%d] = %d\n", i, fd[i]);
    }

    /*
    for (i = 0; i < 3; i++)
    {
        if (_ioe[i]->GetHandle(0, false) >= 0)
        {
            CMaaFile f = CMaaFile::FromHandle(_ioe[i]->GetHandle(0, false), false, -1);
            int x = f.Write("12345", 5);
            log.fprintf("test -2 (5): %d, fd=%d\n", x, _ioe[i]->GetHandle(0, false));
        }
    }
    */
    /*
    {
        CMaaFile f1 = CMaaFile("/var/ram/file1__.txt", CMaaFile::eWC_SrSw, eNoExcept);
        CMaaFile f2 = CMaaFile::FromHandle(f1.GetHandle(false), false, -1);
        int x = f1.Write("12345", 5);
        log.fprintf("test1 (5): %d, fd=%d, %s\n", x, f1.GetHandle(false), f1.GetMsg());
        x = f2.Write("12345", 5);
        log.fprintf("test2 (5): %d, fd=%d, %s\n", x, f2.GetHandle(false), f2.GetMsg());
    }
    {
        Pipe temp(false);
        bool b = temp.Create();
        log.fprintf("b = %s, temp.fd[0]=%d, temp.fd[1]=%d, %d\n", b ? "true" : "false", temp.fd[0], temp.fd[1], temp.GetHandle(0, false));
        CMaaFile f1 = CMaaFile::FromHandle(temp.GetHandle(0, false), false, -1);
        log.fprintf("notest: %s\n", f1.GetMsg());
        CMaaFile f2 = CMaaFile::FromHandle(temp.GetHandle(1, false), false, -1);
        int x = f1.Write("12345", 5);
        log.fprintf("test (temp) (5): %d, fd=%d, %s\n", x, f1.GetHandle(false), f1.GetMsg());
        log.fprintf("fd=%d, %s\n", f2.GetHandle(false), f2.GetMsg());
    }
    {
        Pipe temp;
        CMaaFile f1 = CMaaFile::FromHandle(temp.GetHandle(0, false), false, -1);
        CMaaFile f2 = CMaaFile::FromHandle(temp.GetHandle(1, false), false, -1);
        int x = f1.Write(".12345.", 7);
        log.fprintf("test (temp) _ (7): %d, fd=%d\n", x, f1.GetHandle(false));
        log.fprintf("fd=%d\n", f2.GetHandle(false));
    }
    */

    Pipe _pipe;

    if  (_pipe.GetHandle(0, false) >= 0)
    {
        //_pipe.SetHandleIsInherited(1, true, false);
        try
        {
            //            _pipe.SetHandleIsInherited(1, true, true);
        }
        catch(XTOOFile2Error err)
        {
            CMaaString tmp;
            tmp.Format("fork_and_StartProcess() for pipe_ctrl_fd[1] - %s", (const char *)err.GetMsg());
            err.SetMsg(tmp);
            f_glog.fprintf("%s\n", (const char *)tmp);
            //throw;
        }
    }

    m_Flags = Flags;

    m_ChildProcessId = -1;
    m_Status = 0;
    memset(&m_rusage, 0, sizeof(m_rusage));
    m_ExitCode = -1;

    /*
    for (int i = 0; i < 3; i++)
    {
        if (_ioe[i]->GetHandle(0, false) >= 0)
        {
            CMaaFile f = CMaaFile::FromHandle(_ioe[i]->GetHandle(0, false), false, -1);
            int x = f.Write("12345", 5);
            log.fprintf("test -1 (5): %d, fd=%d\n", x, _ioe[i]->GetHandle(0, false));
        }
    }
    */

    f_glog.SetCloseOnExec(false);
    f_glog.fprintf("test 1\n");

    {
        for (int i = 0; i < 3; i++)
        {
            if  (fd[i] >= 0)
            {
                CMaaFile::StaticSetCloseOnExec(fd[i], false, false);
            }
            /*
        if (_ioe[i]->GetHandle(0, false) >= 0)
        {
            if (!_ioe[i]->SetHandleIsInherited(0, true, false))
            {
                f_glog.fprintf("!_ioe[i]->SetHandleIsInherited(0, true, false)\n");
            }
        }
        if (_ioe[i]->GetHandle(1, false) >= 0)
        {
            if (!_ioe[i]->SetHandleIsInherited(1, true, false))
            {
                f_glog.fprintf("!_ioe[i]->SetHandleIsInherited(1, true, false)\n");
            }
        }
        */
        }
    }
    gettimeofday(&m_tv0, nullptr);

    pid_t pid = fork();
    if  (pid == 0)
    {
        ToolsLib_OpenLog();
    }
    f_glog.fprintf("pid=%d 1\n", (int)pid);
    m_ChildProcessId = pid;

    int err = errno;
    if  (pid)
    {
        for (i = 0; i < 3; i++)
        {
            if  (fd[i] >= 0)// && _ioe[i]->GetHandle(0, false) < 0)
            {
                close(fd[i]);
                fd[i] = -1;
            }
        }
    }

    if  (pid > 0)
    {
        // parent process
        err = 0;
        int errcode = 0;
        _pipe.ItIsASide(0);
        CMaaFile fPipe = CMaaFile::FromHandle(_pipe.GetHandle(0, true), false, 1);
        f_glog.fprintf("pid > 0, fPipe=\n");
        if  (fPipe.IsOpen())
        {
            f_glog.fprintf("pid > 0, fPipe is opened\n");
            _pipe.IgnoreSignals();
            _dword x;
            f_glog.fprintf("pid > 0, fPipe reading...\n");
            if  ((x = fPipe.Read(&errcode, (_dword)sizeof(errcode))) == (_dword)sizeof(errcode))
            {
                //printf("errcode = %d\n", errcode);
                f_glog.fprintf("read retcode %d\n", (int)errcode);
                err = errcode;
            }
            else
            {
                f_glog.fprintf("read %d pipe data, err %s\n", (int)x, fPipe.GetMsg());
            }
            fPipe.Close();
            _pipe.RestoreSignals();
        }
        else
        {
            f_glog.fprintf("fPipe - is closed\n");
        }

        for (i = 0; i < 3; i++)
        {
            /*
            if   (_ioe[i]->GetHandle(0, false) >= 0)
            {
                _ioe[i]->ItIsASide(0);
                _ioe[i]->SetHandleIsInherited(0, true, false);
            }
            */
        }

        pid_t pids[3];
        int errnums[3];
        CMaaFile f__2 = f_glog;//("/var/ram/f__2.txt", CMaaFile::eWC_SrSw, eNoExcept);
        //            int initial_keys[3] = {0x40000001, 0x40000002, 0x40000003};
        int keys[3] = {-1, -1, -1};
        //CMaaFile log1("/var/ram/log1.txt", CMaaFile::eAC_SrSw, eNoExcept);
        for (i = 0; i < 3; i++)
        {
            f__2.fprintf("%d. %d\n", i, (int)_ioe[i]->GetHandle(i == 0 ? 1 : 0, false));

            if  (_ioe[i]->GetHandle(i == 0 ? 1 : 0, false) >= 0)
            {
                CMaaFile f = CMaaFile::FromHandle(_ioe[i]->GetHandle(i == 0 ? 1 : 0, false), false, -1);
                f_glog.fprintf("___ %d %d %s\n", _ioe[i]->GetHandle(i == 0 ? 1 : 0, false), f.GetHandle(false), f.GetMsg());
                //int x = f.Write("12345", 5);
                //log.fprintf("test (5): %d\n", x);
            }

            f_glog.SetCloseOnExec(false);
            f_glog.fprintf("i=%d\n", (int)i);

            {
                for (int i = 0; i < 3; i++)
                {
                    if  (fd[i] >= 0)
                    {
                        CMaaFile::StaticSetCloseOnExec(fd[i], false, false);
                    }
                    /*
        if (_ioe[i]->GetHandle(0, false) >= 0)
        {
            _ioe[i]->SetHandleIsInherited(0, true, false);
        }
        if (_ioe[i]->GetHandle(1, false) >= 0)
        {
            _ioe[i]->SetHandleIsInherited(1, true, false);
        }
        */
                }
            }
            if  (_ioe[i]->GetHandle(i == 0 ? 1 : 0, false) >= 0)
            {
                f_glog.fprintf("fork() for data s/r, i=%d\n", i);
            }

            if  (_ioe[i]->GetHandle(i == 0 ? 1 : 0, false) >= 0)
            {
                int _length = (int)(sizeof(_qword) + sizeof(_qword));
                _length = (_length + getpagesize() - 1) / getpagesize() * 100;
                _length *= getpagesize();
                keys[i] = shmget(
//            		initial_keys[i],
              IPC_PRIVATE,
              _length, //sizeof(_qword), /*SHM_PAGEABLE|*/
              IPC_CREAT|S_IWUSR|S_IRUSR);
                char * ptr = (char *)shmat(keys[i],  0, 0);
                //log1.fprintf("%d 0x%x %p\n", keys[i], keys[i], ptr);
                //--- SHM_PAGEABLE
                if  ((ptr + 1))
                {
                    *(_qword *)ptr = (_qword)-1;
                    *(1 + (_qword *)ptr) = (_qword)-1;
                    shmdt(ptr);
                }
                pids[i] = fork();
            }
            else
            {
                pids[i] = -1;
            }
            errnums[i] = errno;
            if  (pids[i] == 0)
            {
                ToolsLib_OpenLog();
            }
            f_glog.fprintf("pid[%d]=%d, errno=%d\n", i, (int)pids[i], errnums[i]);
            //f__2.fprintf("pid=%d, errno=%d\n", (int)pids[i], errnums[i]);
            if  (pids[i] == 0)
            {
                //                _ioe[i]->ItIsASide(0);
                for (int j = 0; j < 3; j++)
                {
                    if  (j != i)
                    {
                        _ioe[j]->Close();
                    }
                }
                CMaaString &Data = i == 0 ? m_StdInSend : i == 1 ? m_StdOutRecv : m_StdErrRecv;
                CMaaConcatString strData;
                int errcode = 0;
                //CMaaFile f__3("/var/ram/f__3.txt", CMaaFile::eWC_SrSw, eNoExcept);
                CMaaFile f__3;
                try
                {
                    _ioe[i]->IgnoreSignals();
                    f_glog.fprintf("\n\nData.Length() = %d\n", Data.Length());
                    _ioe[i]->ItIsASide(i == 0 ? 1 : 0);
                    CMaaFile f = CMaaFile::FromHandle(_ioe[i]->GetHandle(i == 0 ? 1 : 0, true), true, 1);
                    if  (i == 0)
                    {
                        while(Data.IsNotEmpty())
                        {
                            _dword x = f.Write(Data, Data.Length());
                            f_glog.fprintf("f.Write %d bytes\n", (int)x);
                            if  (x > 0)
                            {
                                f__3.Write(Data, (int)x);
                                Data = Data.RefMid((int)x);
                            }
                        }
                        f.Close();
                        f__3.Close();
                    }
                    else
                    {
                        CMaaPtr_<char, 1> Buffer(1024 * 1024);
                        while(1)
                        {
                            int x = f.Read(Buffer, (int)Buffer.Size());
                            if  (x > 0)
                            {
                                strData.Add(Buffer, (int)x);
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
                catch(XTOOFile2Error err)
                {
                    errcode = err.GetErrorCode();
                    f__3.fprintf("err: %s\n", err.GetMsg());
                }
                catch(...)
                {
                    f__3.fprintf("catch(...)\n");
                    errcode = 0x10000;
                }
                //CMaaFile log2("/var/ram/log2.txt", CMaaFile::eAC_SrSw, eNoExcept);
                //log2.fprintf("[%d] %d\n", i, i >= 0 && i < 3 ? keys[i] : -123);
                if  (i > 0)
                {
                    Data = (CMaaString)strData;
                    if  (keys[i] != -1)
                    {
                        //log2.fprintf("_2: [%d 0x%x] %d\n", i, keys[i], keys[i]);
                        int nret1 = -1234;
                        //                        	int nret1 = shmctl(keys[i], IPC_RMID, 0);
                        //            			char * ptr = (char *)shmat(keys[i], 0, 0);
                        int new_length = 2 * sizeof(_qword) + Data.Length();
                        new_length = (new_length + getpagesize() - 1) / getpagesize();
                        new_length *= getpagesize();

                        shmid_ds shm_info__;
                        memset(&shm_info__, 0, sizeof(shm_info__));
                        shmctl(keys[i], IPC_STAT, &shm_info__);
                        int seg_size = shm_info__.shm_segsz;
                        char * ptr = (char *)shmat(keys[i],0, 0);
                        if  (seg_size >= new_length)
                        {
                        }
                        else if ((ptr + 1) && seg_size >= (int)(2 * sizeof(_qword)))
                        {
                            //shmctl(keys[i], IPC_RMID, 0);
                            int key = shmget(IPC_PRIVATE, new_length, IPC_CREAT|S_IWUSR|S_IRUSR);
                            char *ptr2 = (char *)shmat(key, 0, 0);
                            *(_qword *)ptr = 0;//(_qword)(2 * sizeof(_qword));
                            *(1 + (_qword *)ptr) = (_qword)key;
                            shmdt(ptr);
                            ptr = ptr2;
                            if  ((ptr + 1))
                            {
                                *(_qword *)ptr = 0;
                                *(1 + (_qword *)ptr) = -1;
                            }
                            keys[i] = key;
                        }
                        //log2.fprintf("_3: nret1 = %d, [%d] %d\n", nret1, i, keys[i]);
                        //log2.fprintf("_4: [%d] %d %p\n", i, keys[i], ptr);
                        if  ((ptr + 1))
                        {
                            *(_qword *)ptr = (_qword)Data.Length();
                            memcpy(ptr + 2 * sizeof(_qword), Data, Data.Length());
                            shmdt(ptr);
                        }
                        //log2.fprintf("_5: [%d] %d %p %d\n", i, keys[i], ptr, Data.Length());
                        //log2.Close();
                    }
                }
                _ioe[i]->RestoreSignals();
                _exit(errcode);
            }
            else
            {
                if  ((_ioe[i]->GetHandle(0, false) >= 0 || _ioe[i]->GetHandle(1, false) >= 0) && fd[i] >= 0)
                {
                    close(fd[i]); fd[i] = -1;
                }
                _ioe[i]->Close();
            }
        }

        for (i = 0; i < 3; i++)
        {
            if  (pids[i] > 0 && (Flags & eWaitChild))
            {
                // parent process
                int errcode = errnums[i];
                {
                    int Status = 0;
                    int options = 0;
                    pid_t x;
                    /*
            if (Flags & eGetUsage)
            {
                x = wait4(pids[i], &Status, options, &m_rusage);
            }
            else
            */
                    x = waitpid(pids[i], &Status, options);
                    //m_Status = Status;
                    //if   (x != -1)
                    if  (x > 0 && WIFEXITED(Status))
                    {
                        int ExitCode = WEXITSTATUS(Status);
                        //printf("m_ExitCode = %d\n", m_ExitCode);
                    }
                    if  (i > 0 && keys[i] != -1)
                    {
                        shmid_ds shm_info__;
                        memset(&shm_info__, 0, sizeof(shm_info__));
                        shmctl(keys[i], IPC_STAT, &shm_info__);
                        int seg_size = shm_info__.shm_segsz;
                        char * ptr = (char *)shmat(keys[i], 0, 0);
                        _qword length = -1, key = -1;
                        //log1.fprintf("   [%d] %d %p (%d)", i, keys[i], ptr, (int)seg_size);
                        if  ((ptr + 1) && seg_size >= (int)(2 * sizeof(_qword)) &&
                     ((length = *(_qword *)ptr) >= 0) &&
                     length + (_qword)(2 * sizeof(_qword)) <= (_qword)seg_size)
                        {
                            CMaaString &Data = i == 0 ? m_StdInSend : i == 1 ? m_StdOutRecv : m_StdErrRecv;
                            key = *(1 + (_qword *)ptr);
                            if  (key != -1)
                            {
                                shmctl(keys[i], IPC_RMID, 0);
                                keys[i] = (int)key;
                                shmdt(ptr); // can be later than shmctl(keys[i], IPC_RMID, 0);
                                //shmid_ds shm_info__;
                                memset(&shm_info__, 0, sizeof(shm_info__));
                                shmctl(keys[i], IPC_STAT, &shm_info__);
                                seg_size = shm_info__.shm_segsz;
                                ptr = (char *)shmat(keys[i], 0, 0);
                                length = -1;//, key = -1;
                                //log1.fprintf("   [%d] %d %p (%d)", i, keys[i], ptr, (int)seg_size);
                                if  ((ptr + 1) && seg_size >= (int)(2 * sizeof(_qword)) &&
                             ((length = *(_qword *)ptr) >= 0) &&
                             length + (_qword)(2 * sizeof(_qword)) <= (_qword)seg_size)
                                {
                                    Data = CMaaString(ptr + 2 * sizeof(_qword), (int)length);
                                }
                                //log1.fprintf(" %D\n", length);
                            }
                            else
                            {
                                Data = CMaaString(ptr + 2 * sizeof(_qword), (int)length);
                            }
                        }
                        else
                        {
                            //log1.fprintf(" %D\n", length);
                            //log1.fprintf("\n");
                        }
                        if  ((ptr + 1))
                        {
                            shmdt(ptr);
                        }
                    }
                    f_glog.fprintf("\n\n\ntemp: wait() returns %d, WEXITSTATUS(%d) = %d\n\n", x, Status, WEXITSTATUS(Status));
                }
                _ioe[i]->Close();
                if  (i > 0 && keys[i] != -1)
                {
                    shmctl(keys[i], IPC_RMID, 0);
                    keys[i] = -1;
                }
            }
        }
        //f__2.Close();
    }

    if  (pid > 0)
    {
        if  (Flags & eWaitChild)
        {
            int Status = 0;
            int options = 0;
            m_Options = options;
            pid_t x;
            //do
            //{
            if  (Flags & eGetUsage)
            {
                x = wait4(pid, &Status, options, &m_rusage);
            }
            else
            {
                x = waitpid(pid, &Status, options);
            }
            //}while(x==0);
            gettimeofday(&m_tv1, nullptr);
            m_rtv.tv_sec = m_tv1.tv_sec - m_tv0.tv_sec;
            m_rtv.tv_usec = m_tv1.tv_usec - m_tv0.tv_usec;
            if  (m_rtv.tv_usec < 0)
            {
                m_rtv.tv_sec--;
                m_rtv.tv_usec += 1000000;
            }

            int Status_simple = Status & 0xffff; //Status;// 07.11.2015// & 0xffff;
            m_Status = Status_simple;
            m_StatusEx = Status;
            //!!2019 f_glog.fprintf("\n\n\nwait() returns %d, WEXITSTATUS(%d) = %d\n\n", x, Status, WEXITSTATUS(Status_simple));
            f_glog.fprintf("\n\n\nwait() returns %d, WEXITSTATUS(%d) = %d\n\n", x, Status, WEXITSTATUS(Status));
            /*if   (x != -1)
            {
                m_ExitCode = WEXITSTATUS(Status_simple);
                //printf("m_ExitCode = %d\n", m_ExitCode);
            }*/
            if  (x > 0)
            {
                if  (WIFEXITED(Status))
                {
                    //m_ExitCode = WEXITSTATUS(Status_simple);
                    m_ExitCode = WEXITSTATUS(Status);
                    f_glog.fprintf("m_ExitCode = %d\n", m_ExitCode);
                }
                else if (WIFSIGNALED(Status))
                {
                    int s = WTERMSIG(Status);
                    f_glog.fprintf("process %d is killed by %d signal\n", (int)pid, (int)s);
                    m_ExitCode = s != 9 ? 124 : 128+9; // like: timeout 0.3 ./abcd.sh ; a=$?; echo $a
                    // s + 256;
                    if  (WCOREDUMP(Status))
                    {
                        f_glog.fprintf("Core dumped\n");
                    }
                }
                else if (WIFSTOPPED(Status))
                {
                    int s = WSTOPSIG(Status);
                    f_glog.fprintf("process %d is stopped by %d signal\n", (int)pid, (int)s);
                }
                else if (WIFCONTINUED(Status))
                {
                    f_glog.fprintf("process %d is continued\n", (int)pid);
                }
                else
                {
                    f_glog.fprintf("x = %d, status = 0x%08X\n", (int)x, (int)Status);
                }
            }
            else
            {
                f_glog.fprintf("x = %d (<= 0?)\n", (int)x);
            }
        }
    }
    else if (pid == 0)
    {
        //CMaaFile f_log("/var/ram/maa/log___1.txt", CMaaFile::eWCD_SrSw, eNoExcept);
        CMaaFile f_log;
        int std_fileno[3] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
        int child_fd[3];
        for (int i = 0; i < 3; i++)
        {
            f_log.fprintf("fd[%d]=%d\n", i, fd[i]);
            child_fd[i] = -1;
            if  (fd[i] < -1)
            {
                close(std_fileno[i]);
                f_log.fprintf("close(%d)\n", std_fileno[i]);
            }
            else if (fd[i] >= 0)
            {
                child_fd[i] = ::dup2(fd[i], std_fileno[i]);
                close(fd[i]);
            }
            else
            {
                //child_fd[i] = std_fileno[i];
            }
            log.fprintf("%d. child_fd[i] = %d\n", i, child_fd[i]);
            _ioe[i]->Close();
        }
        _pipe.ItIsASide(1);
        f_glog.fprintf("point 1\n");
        if  (_pipe.GetHandle(1, false) >= 0)
        {
            //_pipe.SetHandleIsInherited(1, false, false);
            try
            {
                _pipe.SetHandleIsInherited(1, false, true);
            }
            catch(XTOOFile2Error err)
            {
                CMaaString tmp;
                tmp.Format("fork_and_StartProcess(), child process, for pipe_ctrl_fd[1] - %s", (const char *)err.GetMsg());
                err.SetMsg(tmp);
                f_glog.fprintf("child: %s\n", (const char *)err.GetMsg());
                //throw;
            }
        }
        f_glog.fprintf("point 2 (StartProcess)\n");

        err = -1; // unknown error
        if  (!unix_StartProcess(ExecFileName, args, envs, false, Flags, m_bEJudge, m_MemLimit, m_TimeLimitMs))
        {
            err = errno;
            //printf("child: err = %d\n", err);
        }
        if  (err == ENOENT)
        {
        }

        CMaaFile fPipe = CMaaFile::FromHandle(_pipe.GetHandle(1, true), false, 1);
        if  (fPipe.IsOpen())
        {
            _pipe.IgnoreSignals();
            _dword x;
            if  ((x = fPipe.Write(&err, (_dword)sizeof(err))) == (_dword)sizeof(err))
            {
            }
            f_glog.fprintf("write %d pipe data, err %s\n", (int)x, fPipe.GetMsg());
            fPipe.Close();
            _pipe.RestoreSignals();
        }
        _exit(err);
    }
    _pipe.Close();

    m_Errno = err;
    if  (err)
    {
        return CMaaFile::StaticError(err, bThrow, "fork_and_StartProcess()");
    }
    return true;
#endif // ___ToolsLibQT___
}
//------------------------------------------------------------------------------
bool unix_fork_and_StartProcess(int Flags, int * pExitCode, const char * ExecFileName, const char ** args, const char ** envs, bool bThrow, int fdStdIn, int fdStdOut, int fdStdErr)
{
    C_os_StartProcess proc;
    proc.m_fdStdIn = fdStdIn;
    proc.m_fdStdOut = fdStdOut;
    proc.m_fdStdErr = fdStdErr;

    bool Ret;
    try
    {
        Ret = proc.fork_and_StartProcess(Flags, ExecFileName, args, envs, bThrow);
        if  (pExitCode)
        {
            *pExitCode = proc.GetExitCode();
        }
    }
    catch(...)
    {
        if  (pExitCode)
        {
            *pExitCode = proc.GetExitCode();
        }
        throw;
    }
    return Ret;
}
//------------------------------------------------------------------------------
#endif // TOOLSLIB_SINGLE_THREAD
#endif // __unix__
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(const CMaaFile & That) noexcept
:   XTOOFile2Error((const XTOOFile2Error&)That),
    m_pImp(nullptr),
    m_bIsThrow(That.m_bIsThrow)
{
    *this = That;
}
//------------------------------------------------------------------------------
CMaaFile::CMaaFile(CMaaFile && That) noexcept
:   XTOOFile2Error((const XTOOFile2Error&)That),
    m_pImp(That.m_pImp),
    m_bIsThrow(That.m_bIsThrow)
{
    That.m_pImp = nullptr;
}
//------------------------------------------------------------------------------
CMaaFile::~CMaaFile()
{
    if  (IsOpen())
    {
        if  (!m_pImp->Release())
        {
#if MANUAL_CMaaFileImp_REFS
            delete m_pImp;
#endif
        }
    }
}
//------------------------------------------------------------------------------
bool CMaaFile::IsOpen() const noexcept
{
    return m_pImp != nullptr;
}
//------------------------------------------------------------------------------
bool CMaaFile::IsExists(const char* pszName) noexcept
{
    return IsExists(pszFileName2strFileName(pszName));
}
//------------------------------------------------------------------------------
bool CMaaFile::IsExists_Internal(const CMaaString& Name) noexcept
{
    CMaaFile Test(Name, CMaaFile::eRI_SrSw, eNoExcept);
    return Test.IsOpen();
}
//------------------------------------------------------------------------------
bool CMaaFile::IsExists(const CMaaString& Name) noexcept
{
    CMaaFile Test(Name, CMaaFile::eR_SrSw, eNoExcept);
    return Test.IsOpen();
    /*
#ifdef __unix__
    // Warning: different action
    FILE * f = fopen ( _uc_MkCompatible ( pszName ), "rb" );
    if  ( f )
    {
        fclose ( f );
    }
    return f ? true : false;
#else
     return Access ( _uc_MkCompatible ( pszName ), 0 );
#endif
*/
}
//------------------------------------------------------------------------------
CMaaFile & CMaaFile::operator= (const CMaaFile & That) noexcept
{
#ifndef TOOLSLIB_SINGLE_THREAD
    gCMaaSyncAssignLockUpperLock(true); // the  same as in Errors.cpp operator =
#endif
    CMaaSyncAssign(m_pImp, That.m_pImp, true, MANUAL_CMaaFileImp_REFS, false);
#if 0
    if  (That.IsOpen())
    {
        That.m_pImp->AddRef();
    }
    CMaaFileImp * tmp = m_pImp;
    if  (IsOpen())
    {
        m_pImp = That.m_pImp;
        if  (!tmp->Release())
        {
#if MANUAL_CMaaFileImp_REFS
            delete tmp;
#endif
        }
    }
    else
    {
        m_pImp = That.m_pImp;
    }
#endif
    m_bIsThrow = That.m_bIsThrow;
    ((XTOOFile2Error*)this)->Assign_locked(That);
#ifndef TOOLSLIB_SINGLE_THREAD
    gCMaaSyncAssignLockUpperUnLock();
#endif
    return *this;
}
//------------------------------------------------------------------------------
CMaaFile& CMaaFile::operator= (CMaaFile&& That) noexcept
{
    if (this != &That)
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        gCMaaSyncAssignLockUpperLock(true);
#endif
        CMaaSyncAssign(m_pImp, That.m_pImp, false, MANUAL_CMaaFileImp_REFS, false);
        That.m_pImp = nullptr;
#if 0
        // v1
        CMaaFileImp* tmp = m_pImp;
        if (IsOpen())
        {
            m_pImp = That.m_pImp;
            if (!tmp->Release())
            {
#if MANUAL_CMaaFileImp_REFS
                delete tmp;
#endif
            }
        }
        else
        {
            m_pImp = That.m_pImp;
        }
#endif
        m_bIsThrow = That.m_bIsThrow;
        ((XTOOFile2Error*)this)->Assign_locked(That);
#ifndef TOOLSLIB_SINGLE_THREAD
        gCMaaSyncAssignLockUpperUnLock();
#endif
    }
    return *this;
}
//------------------------------------------------------------------------------
_dword CMaaFile::Read(void *Buffer, _dword Size)
{
    if  (IsOpen())
    {
        Clear();

#ifdef _WIN32
        DWORD NumberOfBytesRead = 0;
        if  (!ReadFile(m_pImp->pFile, Buffer, Size, &NumberOfBytesRead, nullptr))
        {
            SetError("ReadFile()");
        }
#endif
#ifdef __unix__
        _dword NumberOfBytesRead;
        while(1)
        {
            NumberOfBytesRead = read(m_pImp->fd, Buffer, Size);
            if  (NumberOfBytesRead == (_dword)-1)
            {
                if  (errno == EINTR)
                {
                    continue;
                }
                NumberOfBytesRead = 0;
                SetError("ReadFile()");
            }
            break;
        }
#endif
        return NumberOfBytesRead;
    }
    else
    {
        //char buf[255];
        //sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        //SetErrMessEx( buf );
        //XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
        SetError("Bad implementation", 0);
    }
    return 0;
}
//------------------------------------------------------------------------------
_dword CMaaFile::Write(const void *Buffer, _dword Size)
{
    if  (IsOpen())
    {
        if  (Size)
        {
            Clear ();
#ifdef _WIN32
            DWORD NumberOfBytesWritten = 0;
            WriteFile(m_pImp->pFile, Buffer, Size, &NumberOfBytesWritten, nullptr);
#endif
#ifdef __unix__
            _dword NumberOfBytesWritten;
            while(1)
            {
                NumberOfBytesWritten = write(m_pImp->fd, Buffer, Size);
                if  (NumberOfBytesWritten == (_dword)-1)
                {
                    if  (errno == EINTR)
                    {
                        continue;
                    }
                    NumberOfBytesWritten = 0;
                }
                break;
            }
#endif

            if  (NumberOfBytesWritten != Size)
            {
                //char buf[255];
                //sprintf( buf, mess_for_search, __FILE__, __LINE__ );
                //SetErrMessEx( buf );
                //XTOOStatus::SetError ( e_FILE_WRITE_ERROR/*eTOOWriteLess*/ );
                SetError("WriteFile()");
            }
            return NumberOfBytesWritten;
        }
    }
    else
    {
        //char buf[255];
        //sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        //SetErrMessEx( buf );
        //XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
        SetError("Bad implementation", 0);
    }
    return 0;
}
//------------------------------------------------------------------------------
void CMaaFile::Close() noexcept
{
    if  (IsOpen())
    {
        CMaaFileImp * tmp = m_pImp;
        m_pImp = nullptr;
        if  (!tmp->Release())
        {
#if MANUAL_CMaaFileImp_REFS
            delete tmp;
#endif
        }
    }
    /*
    else
    {
        if  (m_pImp)
        {
               char buf[255];
               sprintf( buf, mess_for_search, __FILE__, __LINE__ );
               SetErrMessEx( buf );
               XTOOStatus::SetError(e_INVALID_IMPLEMENTATION); //eInvalidImplementation
        }
    }
    */
}
//------------------------------------------------------------------------------
_qword CMaaFile::Length(const CMaaString& FileName, bool bThrow)
{
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA d;
    memset(&d, 0, sizeof(d));
    CMaaString tmp = MkFsNativeCompatible(FileName);
    if (GetFileAttributesEx(MkFs_cast(tmp), GetFileExInfoStandard, &d))
    {
        return ((_qword)d.nFileSizeHigh << 32) | (_qword)d.nFileSizeLow;
    }
    if (bThrow)
    {
        XTOOFile2Error err("CMaaFile::Length");
        throw err;
    }
    return -1;
#endif
#ifdef __unix__
    struct stat st;
    memset(&st, 0, sizeof(st));
    if (stat(MkFs_cast(MkFsNativeCompatible(FileName)), &st) != -1)
    {
        return st.st_size;
    }
    if (bThrow)
    {
        XTOOFile2Error err("CMaaFile::Length");
        throw err;
    }
    return -1;
#endif
}
//------------------------------------------------------------------------------
_qword CMaaFile::Length(const char * szFileName, bool bThrow)
{
    return Length(pszFileName2strFileName(szFileName));
}
//------------------------------------------------------------------------------
#if 0
_qword CMaaFile::GetLastWriteTime(const char * szFileName, bool bThrow = true);
{
    /*
     BOOL GetFileTime(
  HANDLE hFile,
  LPFILETIME lpCreationTime,
  LPFILETIME lpLastAccessTime,
  LPFILETIME lpLastWriteTime
);
     */
    WIN32_FILE_ATTRIBUTE_DATA d;
    memset(&d, 0, sizeof(d));
    if  (GetFileAttributesEx(szFileName, GetFileExInfoStandard, &d))
    {
        FILETIME ft = d.ftLastWriteTime;
        return ((_qword)ft.dwHighDateTime  << 32) | (_qword)ft.dwLowDateTime;
    }
    if  (bThrow)
    {
        XTOOFile2Error err;
        throw err;
    }
    return -1;
}
#endif
//------------------------------------------------------------------------------
_qword CMaaFile::Length()
{
    _qword Length = (_qword)-1;

    if  (IsOpen())
    {
        Clear();
        //#ifdef __unix__
        _qword pos = CurPos();
        Seek(0, SEEK_END);
        Length = CurPos();
        Seek(pos);
/*
#else
    #ifdef _WIN32
        Length = _filelength ( _fileno ( m_pImp -> pFile ) );
    #else
        Length = filelength ( fileno ( m_pImp -> pFile ) );
    #endif
#endif
*/
        //curpos = ftell ( m_pImp -> pFile ); fseek ( m_pImp -> pFile, 0L, SEEK_END );
        //Length = ftell ( m_pImp -> pFile ); fseek ( m_pImp -> pFile, curpos, SEEK_SET );

        if  (Length == (_qword)-1)
        {
            //char buf[255];
            //sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            //SetErrMessEx( buf );
            ////XTOOFile2Error::SetError ( errno );
            SetError("CMaaFile::Length()");
        }
    }
    else
    {
        //XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
        SetError("Bad implementation", 0);
    }
    return Length;
}
//------------------------------------------------------------------------------
bool CMaaFile::Seek(_qword Offset, int FromWhere)
{
    if  (IsOpen())
    {
        Clear();
#ifdef _WIN32
        LARGE_INTEGER i;
        i.QuadPart = Offset;
        DWORD dwMoveMethod = FILE_BEGIN;
        if  (FromWhere == SEEK_END)
        {
            dwMoveMethod = FILE_END;
        }
        else if (FromWhere == SEEK_CUR)
        {
            dwMoveMethod = FILE_CURRENT;
        }
#ifndef VC60
        if  (SetFilePointerEx(m_pImp->pFile, i, /*lpNewFilePointer*/nullptr, dwMoveMethod))
#else
        if  (SetFilePointer(m_pImp->pFile, *(LONG *)&i, 1 + (LONG *)&i, dwMoveMethod))
#endif
        {
            // We are here when inside and behind the end of file for SEEK_SET
            return true;
        }
        SetError("SetFilePointerEx()");
        return false;
#endif
#ifdef __unix__

        //if  (lseek(m_pImp->fd, Offset, FromWhere) != -1)

        // off_t xx;
        if  (( //xx = 
#ifdef __USE_LARGEFILE64
//extern __off64_t lseek64 (int __fd, __off64_t __offset, int __whence) __THROW;
            lseek64
#else
            lseek
#endif

            (m_pImp->fd, Offset, FromWhere)) != (off_t)-1)
        {
            // We are here when inside and behind the end of file for SEEK_SET
            return true;
        }
        SetError("lseek()");
        return false;
#endif
    }
    else
    {
        SetError("Bad implementation", 0);
    }
    return false;
}
//------------------------------------------------------------------------------
_qword CMaaFile::CurPos()
{
    _qword Ret = (_qword)-1;
    if  (IsOpen())
    {
        Clear();
#ifdef _WIN32
#ifdef VC60
        LONG x[2] = {0, 0};
        LONG y;
        if  ((y = SetFilePointer(m_pImp->pFile, x[0], x + 1, FILE_CURRENT)) != (DWORD)(-1))
        {
            Ret = (_qword)y;
        }
        else
        {
            SetError("SetFilePointer()", 0);
        }
#else
        LARGE_INTEGER i, i0;
        i.QuadPart = i0.QuadPart = 0;
        if  (SetFilePointerEx(m_pImp->pFile, i0, &i, FILE_CURRENT))
        {
            Ret = i.QuadPart;
        }
        else
        {
            SetError("SetFilePointerEx()", 0);
        }
#endif
#endif
#ifdef __unix__
#ifdef __USE_LARGEFILE64
        __off64_t x = lseek64(m_pImp->fd, 0, SEEK_CUR);
        __off64_t _1 = (__off64_t)-1;
#else
        off_t x = lseek(m_pImp->fd, 0, SEEK_CUR);
        off_t _1 = (off_t)-1;
#endif
        if  (x != _1)
        {
            Ret = x;
        }
        else
        {
            SetError("lseek()");
        }
#endif
    }
    else
    {
        SetError("Bad implementation", 0);
    }
    return Ret;
}
//------------------------------------------------------------------------------
bool CMaaFile::IsEOF()
{
    if  (CurPos() >= Length())
    {
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
bool CMaaFile::Truncate()
{
    if  (IsOpen())
    {
        Clear ();
#ifdef _WIN32
        if  (SetEndOfFile(m_pImp->pFile))
        {
            return true;
        }
        else
        {
            SetError("SetEndOfFile()");
        }
#endif
#ifdef __unix__

        //            if   (ftruncate(m_pImp->fd, GetCurPos()) != -1)
        if  (
#ifdef __USE_LARGEFILE64
       ftruncate64
#else
       ftruncate
#endif
                       (m_pImp->fd, GetCurPos()) != -1)
        {
            return true;
        }
        else
        {
            SetError("ftruncate()");
        }
#endif
    }
    else
    {
        SetError("Bad implementation", 0);
    }
    return false;
}
//------------------------------------------------------------------------------
void CMaaFile::Flush(int Flags)
{
    if  (IsOpen())
    {
        Clear();
#ifdef _WIN32
        if  (!FlushFileBuffers(m_pImp->pFile))
        {
            SetError("Flush()");
        }
#else
        if  (Flags == 0)
        {
            if  (fdatasync(m_pImp->fd))
            {
                SetError("fdatasync()");
            }
        }
        else
        {
            if  (fsync(m_pImp->fd))
            {
                SetError("fsync()");
            }
        }
#endif
    }
    else
    {
        SetError("Bad implementation", 0);
    }
}
//---------------------------------------------------------------------------
bool CMaaFile::Remove(const CMaaString& FileName, bool bThrow)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString fn = MkFsCompatible(FileName);
    const bool bEq = fn.StrictEq(FileName);
    if (bEq)
    {
        fn.Empty();
    }
    return Remove_Internal(bEq ? FileName : fn, bThrow);
#else
    return Remove_Internal(MkFsCompatible(FileName), bThrow);
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::Remove_Internal(const CMaaString& FileName, bool bThrow)
{
#ifdef _WIN32
    CMaaString tmp = MkFsNative(FileName);
    if (!DeleteFile(MkFs_cast(tmp)))
    {
        if (bThrow)
        {
            CMaaString msg;
            msg.Format("DeleteFile %S error:", &FileName);
            XTOOFile2Error err(msg);
            throw err;
        }
        return false;
    }
#endif
#ifdef __unix__
    if (remove(MkFs_cast(MkFsNative(FileName))) < 0)
    {
        return StaticError(-1, bThrow, "remove() %S error:", &FileName);
    }
#endif
    return true;
}
//---------------------------------------------------------------------------
bool CMaaFile::RemoveAtReboot(const CMaaString& FileName,  bool bThrow)
{
#ifdef _WIN32
    CMaaString tmp = MkFsNativeCompatible(FileName);
    if  (!MoveFileEx(MkFs_cast(tmp), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT))
    {
        if  (bThrow)
        {
            CMaaString msg;
            msg.Format("MoveFileEx(%S, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT) error:", &FileName);
            XTOOFile2Error err(msg);
            throw err;
        }
        return false;
    }
#endif
#ifdef __unix__
    /*
          if   (!remove(_uc_MkCompatible(szFileName)))
          {
               if   (bThrow)
               {
                    CMaaString msg;
                    msg.Format("remove() %s error:", szFileName);
                    XTOOFile2Error err(msg);
                    throw err;
               }
               return false;
          }
          */
    if  (bThrow)
    {
        CMaaString msg;
        msg.Format("RemoveAtReboot() %S - not implemented", &FileName);
        XTOOFile2Error err(msg, 0);
        throw err;
    }
    return false;
#endif
    return true;
}
//---------------------------------------------------------------------------
bool CMaaFile::RemoveAtReboot(const char* szFileName, bool bThrow)
{
    return RemoveAtReboot(pszFileName2strFileName(szFileName), bThrow);
}
//---------------------------------------------------------------------------
bool CMaaFile::RemoveEx(const CMaaString& FileName, bool bThrow)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString fn = MkFsCompatible(FileName);
    const bool bEq = fn.StrictEq(FileName);
    if (bEq)
    {
        fn.Empty();
    }
#ifndef __unix__
    CMaaString tmp = MkFsNative(bEq ? FileName : fn);
    SetFileAttributes(MkFs_cast(tmp), 0x20);
#endif
    return Remove_Internal(bEq ? FileName : fn, bThrow);
#else
    CMaaString fn = MkFsCompatible(FileName);
#ifndef __unix__
    CMaaString tmp = MkFsNative(fn);
    SetFileAttributes(MkFs_cast(tmp), 0x20);
#endif
    return Remove_Internal(fn, bThrow);
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::RemoveEx(const char* szFileName, bool bThrow)
{
    return RemoveEx(pszFileName2strFileName(szFileName), bThrow);
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::rfgets(int Size, bool bRemoveCutCrLf, int Flags)
// (&0xff) == 0xff - throw, 0 - nothing to do, 1 - close file, | 0x100 - ignore zero chars as a string terminator
{
    int Len;
    const char* p;
    if (Size <= TOOLSLIB_FGETS_64K)
    {
        char Buffer[TOOLSLIB_FGETS_64K];
        p = rfgets(Buffer, Size, bRemoveCutCrLf, &Len, Flags != -1 ? Flags & ~0xff : 0);
        if (p)
        {
            return CMaaString(p, Len);
        }
    }
    else
    {
        CMaaPtr_<char, 1> Buffer(Size);
        if (Buffer.IsValid())
        {
            p = rfgets(Buffer, Size, bRemoveCutCrLf, &Len, Flags != -1 ? Flags & ~0xff : 0);
            if (p)
            {
                return CMaaString(p, Len);
            }
        }
    }
    switch(Flags & 0xff)
    {
    case 0xff:
        {
            SetError("EOF", 0);
            XTOOFile2Error err("EOF", 0);
            throw err;
        }
    case 0: // use with caution for bad unicode files
        break;
    case 1:
        Close();
    }
    return CMaaString {};
}
//---------------------------------------------------------------------------
char * CMaaFile::rfgets(char * pszBuffer, int Size, bool bRemoveCutCrLf, int * pStrLen, int Flags)
{
    if  (!IsOpen())
    {
        return nullptr;
    }
    _qword FilePos = GetCurPos();
    if  (FilePos <= 0)
    {
        /*
        // excessive ?
        if (Size > 0)
        {
            pszBuffer[0] = 0; // 11.06.2017
        }
        if (Size > 1)
        {
            pszBuffer[1] = 0; // 11.06.2017
        }
        */
        return nullptr;
    }

    char * pszRet = nullptr;
    if  (pStrLen)
    {
        *pStrLen = 0;
    }
    int OriginalSize = Size;
    int StrLen = 0;
    int fmt = IsOpen() ? (int)m_pImp->m_SrcFormat : 0;
    int dstfmt = IsOpen() ? (int)m_pImp->m_DstFormat : 0;
    if  (fmt == ePfxUnicode || fmt == ePfxUnicodeBigEndian)
    {
        Size--;
    }
    if  (IsOpen() && Size >= 1)
    {
        Clear();

        _qword pos = FilePos - (Size - 1);//CurPos();
        if  (pos < 0)
        {
            pos = 0;
            Size = (int)(FilePos + 1);
        }
        Seek(pos);
        int x = (int)Read(pszBuffer, Size - 1);
        if  (x == Size - 1)//> 0)
        {
            pszRet = pszBuffer;
            if  (x < Size)
            {
                pszBuffer[x] = 0;
            }
            else
            {
                x--;
            }

            int x2 = x;

            _WC_ wc2[2] = {0, 0};

            int i;

            switch(fmt)
            {
            case ePfxUnicode:
                for (i = x - 2; i >= 1 && i >= x - 4; i--)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i] + (((_WC_)(unsigned char)pszBuffer[i + 1]) << 8);
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        if  (wch == 0)
                        {
                            x2 = i;
                            break;
                        }
                        if  (wch != wc2[1])
                        {
                            wc2[(i - (x - 4)) / 2] = wch;
                            x2 = i;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                break;
            case ePfxUnicodeBigEndian:
                //fmt = ePfxUnicode;
                for (i = x - 2; i >= 1 && i >= x - 4; i--)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i+1] + (((_WC_)(unsigned char)pszBuffer[i]) << 8);
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        if  (wch == 0)
                        {
                            x2 = i;
                            break;
                        }
                        if  (wch != wc2[1])
                        {
                            wc2[(i - (x - 4)) / 2] = wch;
                            x2 = i;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                break;
            case ePfxUtf8:
            default:
                for (i = x - 1; i >= 0 && i >= x - 2; i--)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i];
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        if  (wch == 0)
                        {
                            x2 = i;
                            break;
                        }
                        if  (wch != wc2[1])
                        {
                            wc2[i - (x - 2)] = wch;
                            x2 = i;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }

            wc2[0] = wc2[1] = 0;

            int x0_ = (fmt == ePfxUnicode || fmt == ePfxUnicodeBigEndian) ? (x & 1) : 0;

            switch(fmt)
            {
            case ePfxUnicode:
                for (i = x2 - 2; i >= 0; i--)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i] + (((_WC_)(unsigned char)pszBuffer[i + 1]) << 8);
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        x0_ = i + 2;
                        break;
                    }
                }
                break;
            case ePfxUnicodeBigEndian:
                for (i = x2 - 2; i >= 0; i--)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i+1] + (((_WC_)(unsigned char)pszBuffer[i]) << 8);
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        x0_ = i + 2;
                        break;
                    }
                }
                break;
            case ePfxUtf8:
            default:
                for (i = x2 - 1; i >= 0; i--)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i];
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        x0_ = i + 1;
                        break;
                    }
                }
            }

            switch(fmt)
            {
            case ePfxUnicode:
                break;
            case ePfxUnicodeBigEndian:
                fmt = ePfxUnicode;
                for (i = x0_; i < x; i += 2)
                {
                    char ch2[2] = {pszBuffer[i + 1], pszBuffer[i]};
                    pszBuffer[i] = ch2[0];
                    pszBuffer[i + 1] = ch2[1];
                    pszBuffer[i] = ch2[0];
                    pszBuffer[i+1] = ch2[1];
                }
                break;
                //case ePfxUtf8:
                //default:
            }
            memmove(pszBuffer, pszBuffer + x0_, x -= x0_);
            x2 -= x0_;
            int tmp = fmt == ePfxUnicode ? 1 : 0;
            if  (bRemoveCutCrLf)
            {
                StrLen = x2;
                if  (tmp)
                {
                    pszBuffer[x2] = pszBuffer[x2 + 1] = 0;
                }
                else
                {
                    pszBuffer[x2] = 0;
                }
            }
            else
            {
                StrLen = x;
                if  (tmp)
                {
                    pszBuffer[x] = pszBuffer[x + 1] = 0;
                }
                else
                {
                    pszBuffer[x] = 0;
                }
            }

            Seek(FilePos - x);
        }
    }
    else
    {
        //char buf[255];
        //sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        //SetErrMessEx( buf );
        //XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
        SetError("Bad implementation", 0);
    }

    if  (StrLen > 0 && fmt != dstfmt)
    {
        CMaaString Unicode = fmt == ePfxUnicode ? CMaaString(pszBuffer, StrLen) : fmt == ePfxUtf8 ? Utf8ToUnicode(CMaaString(pszBuffer, StrLen)) : AnsiToUnicode(CMaaString(pszBuffer, StrLen));
        CMaaString Dst = dstfmt == ePfxUtf8 ? UnicodeToUtf8(Unicode) : dstfmt == ePfxUnicode ? Unicode : dstfmt == ePfxUnicodeBigEndian ? UnicodeToUnicodeBigEndian(Unicode) : UnicodeToAnsi(Unicode);
        if  (dstfmt == ePfxUnicode || dstfmt == ePfxUnicodeBigEndian)
        {
            OriginalSize = (OriginalSize & ~1) - 2;
        }
        else
        {
            OriginalSize = OriginalSize - 1;
        }
        if  (Dst.Length() > OriginalSize)
        {
            /// overflow...
            StrLen = OriginalSize;
        }
        else
        {
            StrLen = (warning_int)Dst.Length();
        }
        memcpy(pszBuffer, (const char *)Dst, StrLen + ((dstfmt == ePfxUnicode || dstfmt == ePfxUnicodeBigEndian) ? 2 : 1));
    }
    if  (pStrLen)
    {
        *pStrLen = StrLen;
    }
    if  (StrLen == 0 && (fmt == ePfxUnicode || fmt == ePfxUnicodeBigEndian) && GetCurPos() <= 0)
    {
        return nullptr;
        //Close();
    }
    return pszRet;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::fgets(int Size, bool bRemoveCutCrLf, int Flags)
// (&0xff) == 0xff - throw, 0 - nothing to do, 1 - close file, | 0x100 - ignore zero chars as a string terminator
{
    int Len;
    const char* p;
    if (Size <= TOOLSLIB_FGETS_64K)
    {
        char Buffer[TOOLSLIB_FGETS_64K];
        p = fgets(Buffer, Size, bRemoveCutCrLf, &Len, Flags != -1 ? Flags & ~0xff : 0);
        if (p)
        {
            return CMaaString(p, Len);
        }
    }
    else
    {
        CMaaPtr_<char, 1> Buffer(Size);
        if (Buffer.IsValid())
        {
            p = fgets(Buffer, Size, bRemoveCutCrLf, &Len, Flags != -1 ? Flags & ~0xff : 0);
            if (p)
            {
                return CMaaString(p, Len);
            }
        }
    }
    switch(Flags & 0xff)
    {
    case 0xff:
        {
            SetError("EOF", 0);
            XTOOFile2Error err("EOF", 0);
            throw err;
        }
    case 0: // use with caution for bad unicode files
        break;
    case 1:
        Close();
    }
    return CMaaString {};
}
//---------------------------------------------------------------------------
char * CMaaFile::fgets(char * pszBuffer, int Size, bool bRemoveCutCrLf, int * pStrLen, int Flags)
{
    char * pszRet = nullptr;
    if  (pStrLen)
    {
        *pStrLen = 0;
    }
    int OriginalSize = Size;
    int StrLen = 0;
    int fmt = IsOpen() ? (int)m_pImp->m_SrcFormat : 0;
    int dstfmt = IsOpen() ? (int)m_pImp->m_DstFormat : 0;
    if  (fmt == ePfxUnicode || fmt == ePfxUnicodeBigEndian)
    {
        Size--;
    }
    if  (IsOpen() && Size >= 1)
    {
        Clear();

        _qword pos = CurPos();
        int x = (int)Read(pszBuffer, Size - 1);
        if  (x > 0)
        {
            pszRet = pszBuffer;
            if  (x < Size)
            {
                pszBuffer[x] = 0;
            }
            else
            {
                x--;
            }

            int i = 0;
            switch(fmt)
            {
            case ePfxUnicode:
                for (i = 0; i < x - 1; i += 2)
                {
                    _WC_ wch = (_WC_)(unsigned char)pszBuffer[i] + (((_WC_)(unsigned char)pszBuffer[i + 1]) << 8);
                    if  ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                    {
                        break;
                    }
                }
                break;
            case ePfxUnicodeBigEndian:
                fmt = ePfxUnicode;
                for (i = 0; i < x - 1; i += 2)
                {
                    char ch2[2] = {pszBuffer[i + 1], pszBuffer[i]};
                    pszBuffer[i] = ch2[0];
                    pszBuffer[i + 1] = ch2[1];
                    _WC_ wch = (_WC_)(unsigned char)ch2[0] + (((_WC_)(unsigned char)ch2[1]) << 8);
                    if  (wch == 0 && !(Flags & 0x100))
                    {
                        break;
                    }
                    if  (wch == '\r' || wch == '\n')
                    {
                        if  (i + 2 < x - 1)
                        {
                            char ch2[2] = {pszBuffer[i + 2 + 1], pszBuffer[i + 2]};
                            pszBuffer[i + 2] = ch2[0];
                            pszBuffer[i + 2 + 1] = ch2[1];
                        }
                        break;
                    }
                }
                break;
            case ePfxUtf8:
            default:
                for (i = 0; i < x; i++)
                {
                    if  ((pszBuffer[i] == 0 && !(Flags & 0x100)) || pszBuffer[i] == '\r' || pszBuffer[i] == '\n')
                    {
                        break;
                    }
                }
            }
            int tmp = fmt == ePfxUnicode ? 1 : 0;
            if  (i < x - tmp)
            {
                _WC_ wch, wch2;
                switch(fmt)
                {
                case ePfxUnicode:
                    //case ePfxUnicodeBigEndian:
                    wch = (_WC_)(unsigned char)pszBuffer[i] + (((_WC_)(unsigned char)pszBuffer[i + 1]) << 8);
                    if  (wch == '\r' || wch == '\n')
                    {
                        if  (i + 3 < x)
                        {
                            wch2 = (_WC_)(unsigned char)pszBuffer[i + 2] + (((_WC_)(unsigned char)pszBuffer[i + 2 + 1]) << 8);
                            if  ((wch2 == '\r' || wch2 == '\n') && wch != wch2)
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = pszBuffer[i + 1] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 2*2] = pszBuffer[i + 2*2 + 1] = 0;
                                }
                                Seek(pos + i + 2*2);
                            }
                            else
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = pszBuffer[i + 1] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 2] = pszBuffer[i + 3] = 0;
                                }
                                Seek(pos + i + 1*2);
                            }
                        }
                        else
                        {
                            wch2 = (_WC_)(unsigned char)pszBuffer[i + 2] + (((_WC_)(unsigned char)pszBuffer[i + 2 + 1]) << 8);
                            if  ((wch2 == '\r' || wch2 == '\n') && wch != wch2)
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = pszBuffer[i + 1] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 2] = pszBuffer[i + 2 + 1] = 0;
                                }
                                Seek(pos + i + 2*2);
                            }
                            else
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = pszBuffer[i + 1] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 1] = pszBuffer[i + 2 + 1] = 0;
                                }
                                Seek(pos + i + 1*2);
                            }
                        }
                    }
                    else
                    {
                        StrLen = i;
                        //pszBuffer[i] = 0;
                        Seek(pos + i + 1);
                    }
                    break;
                case ePfxUtf8:
                default:
                    if  (pszBuffer[i] == '\r' || pszBuffer[i] == '\n')
                    {
                        if  (i < x - 1)
                        {
                            if  ((pszBuffer[i + 1] == '\r' || pszBuffer[i + 1] == '\n') && pszBuffer[i] != pszBuffer[i + 1])
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 2] = 0;
                                }
                                Seek(pos + i + 2);
                            }
                            else
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 1] = 0;
                                }
                                Seek(pos + i + 1);
                            }
                        }
                        else
                        {
                            if  ((pszBuffer[i + 1] == '\r' || pszBuffer[i + 1] == '\n') && pszBuffer[i] != pszBuffer[i + 1])
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 1] = 0;
                                }
                                Seek(pos + i + 2);
                            }
                            else
                            {
                                if  (bRemoveCutCrLf)
                                {
                                    pszBuffer[StrLen = i] = 0;
                                }
                                else
                                {
                                    pszBuffer[StrLen = i + 1] = 0;
                                }
                                Seek(pos + i + 1);
                            }
                        }
                    }
                    else
                    {
                        StrLen = i;
                        //pszBuffer[i] = 0;
                        Seek(pos + i + 1);
                    }
                }
            }
            else
            {
                if  (fmt == ePfxUnicode)
                {
                    pszBuffer[StrLen = i] = pszBuffer[i + 1] = 0;
                }
                else
                {
                    pszBuffer[StrLen = i] = 0;
                }
                Seek(pos + i);
            }
        }
        /*
          if   ( pszRet == nullptr && ferror ( m_pImp -> pFile ) )
          {
               //XTOOFile2Error::SetError ( errno );
               char buf[255];
               sprintf( buf, mess_for_search, __FILE__, __LINE__ );
               SetErrMessEx( buf );
               SetError( e_FILE_READ_ERROR, errno );
          }
          */
    }
    else
    {
        //char buf[255];
        //sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        //SetErrMessEx( buf );
        //XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
        SetError("Bad implementation", 0);
    }

    if  (StrLen > 0 && fmt != dstfmt)
    {
        CMaaString Unicode = fmt == ePfxUnicode ? CMaaString(pszBuffer, StrLen) : fmt == ePfxUtf8 ? Utf8ToUnicode(CMaaString(pszBuffer, StrLen)) : AnsiToUnicode(CMaaString(pszBuffer, StrLen));
        CMaaString Dst = dstfmt == ePfxUtf8 ? UnicodeToUtf8(Unicode) : dstfmt == ePfxUnicode ? Unicode : dstfmt == ePfxUnicodeBigEndian ? UnicodeToUnicodeBigEndian(Unicode) : UnicodeToAnsi(Unicode);
        if  (dstfmt == ePfxUnicode || dstfmt == ePfxUnicodeBigEndian)
        {
            OriginalSize = (OriginalSize & ~1) - 2;
        }
        else
        {
            OriginalSize = OriginalSize - 1;
        }
        if  (Dst.Length() > OriginalSize)
        {
            /// overflow...
            StrLen = OriginalSize;
        }
        else
        {
            StrLen = (warning_int)Dst.Length();
        }
        memcpy(pszBuffer, (const char *)Dst, StrLen + ((dstfmt == ePfxUnicode || dstfmt == ePfxUnicodeBigEndian) ? 2 : 1));
    }
    if  (pStrLen)
    {
        *pStrLen = StrLen;
    }
    if  (StrLen == 0 && (fmt == ePfxUnicode || fmt == ePfxUnicodeBigEndian) && GetCurPos() + 1 >= Length())
    {
        return nullptr;
        //Close();
    }
    return pszRet;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool CMaaFile::IsADir(const CMaaString &FileName, bool bLinkInfo) noexcept(noexcept_new)
{
#ifdef _WIN32
    const DWORD dw = GetFileAttributes(FileName);
    return dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY);
#endif
#ifdef __unix__
    struct stat st;
    memset(&st, 0, sizeof(st));
    if  (bLinkInfo ? lstat(MkFs_cast(MkFsNativeCompatible(FileName)), &st) == 0 : stat(MkFs_cast(MkFsNativeCompatible(FileName)), &st) == 0)
    {
        if  (S_ISDIR(st.st_mode))
        {
            return true;
        }
    }
    return false;
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::IsAFile(const CMaaString &FileName, bool bLinkInfo) noexcept(noexcept_new)
{
#ifdef _WIN32
    const DWORD dw = GetFileAttributes(FileName);
    return dw != INVALID_FILE_ATTRIBUTES && !(dw & FILE_ATTRIBUTE_DIRECTORY);
#endif
#ifdef __unix__
    struct stat st;
    memset(&st, 0, sizeof(st));
    if  (bLinkInfo ? lstat(MkFs_cast(MkFsNativeCompatible(FileName)), &st) == 0 : stat(MkFs_cast(MkFsNativeCompatible(FileName)), &st) == 0)
    {
        if  (S_ISREG(st.st_mode))
        {
            return true;
        }
    }
    return false;
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::Rename(const CMaaString &src, const CMaaString &dst, bool bThrow)
{
#ifdef _WIN32
    CMaaString tmps = MkFsNativeCompatible(src), tmpd = MkFsNativeCompatible(dst);
    if  (::MoveFile(MkFs_cast(tmps), MkFs_cast(tmpd)) != 0)
    {
        return true;
    }
#endif
#ifdef __unix__
    if  (rename(MkFs_cast(MkFsNativeCompatible(src)), MkFs_cast(MkFsNativeCompatible(dst))) == 0)
    {
        return true;
    }
#endif
    if  (bThrow)
    {
        XTOOFile2Error err("Rename");
        throw err;
    }
    return false;
}
//---------------------------------------------------------------------------
bool CMaaFile::MkDir(const CMaaString& dir1, bool bIgnoreExist, bool bThrow)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString dir = MkFsCompatible(dir1);
    const bool bEq = dir.StrictEq(dir1);
    if (bEq)
    {
        dir.Empty();
    }
    return MkDir_Internal(bEq ? dir1 : dir, bIgnoreExist, bThrow);
#else
    return MkDir_Internal(MkFsCompatible(dir1), bIgnoreExist, bThrow);
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::MkDir_Internal(const CMaaString &dir, bool bIgnoreExist, bool bThrow)
{
#ifdef _WIN32
    if (bIgnoreExist)
    {
        const DWORD x = GetFileAttributes_Internal(dir);
        if (x != INVALID_FILE_ATTRIBUTES && (x & FILE_ATTRIBUTE_DIRECTORY))
        {
            return true;
        }
    }
    CMaaString tmp = MkFsNative(dir);
    if (CreateDirectory(MkFs_cast(tmp), nullptr))
    {
        return true;
    }
    const DWORD dw = GetLastError();
    if (dw == ERROR_ALREADY_EXISTS/*)
     {
          return true;
     }
     if   (errno == EEXIST */ && bIgnoreExist && (GetFileAttributes_Internal(dir) & FILE_ATTRIBUTE_DIRECTORY))
    {
        return true;
    }
#endif
#ifdef __unix__
    CMaaString tmp = MkFsNative(dir);
    int res = ::mkdir(MkFs_cast(tmp), 0755);
    const int e = errno;
    if (res == 0 || (e == EEXIST && bIgnoreExist))
    {
        return true;
    }
#endif
    if (bThrow)
    {
        XTOOFile2Error err("CMaaFile::MkDir()");
        throw err;
        //SetError(CMaaString::sFormat("CMaaFile::chroot(\"%S\")", &Path));
    }
    return false;
}
//---------------------------------------------------------------------------
bool CMaaFile::RmDir(const CMaaString& dir1, bool bThrow)
{
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
    CMaaString dir = MkFsCompatible(dir1);
    const bool bEq = dir.StrictEq(dir1);
    if (bEq)
    {
        dir.Empty();
    }
    return RmDir_Internal(bEq ? dir1 : dir, bThrow);
#else
    return RmDir_Internal(MkFsCompatible(dir1), bThrow);
#endif
}
//---------------------------------------------------------------------------
bool CMaaFile::RmDir_Internal(const CMaaString &dir, bool bThrow)
{
#ifdef _WIN32
    CMaaString tmp = MkFsNative(dir);
    if (RemoveDirectory(MkFs_cast(tmp)))
    {
        return true;
    }
#endif
#ifdef __unix__
    if (::rmdir(MkFs_cast(MkFsNative(dir))) == 0)
    {
        return true;
    }
#endif
    if (bThrow)
    {
        XTOOFile2Error err("RmDir");
        throw err;
    }
    return false;
}
//---------------------------------------------------------------------------
/*
static void FileDelErrorLog(const char * path, bool bIsDir = false)
{
     DWORD le = GetLastError();
     int er = errno;

     FILE * f = fopen("logs\\FileDelErrorLog.txt", "a+b");
     if   (f)
     {
          //CMaaDateTime dt;
          time_t dt = time(nullptr);
          fprintf(f, "[%s %s]: Error deleting %s %s: GetLastError() = %d, errno = %d\r\n", (const char *)GetTextDate(dt), (const char *)GetTextTime(dt), bIsDir ? "directory" : "file", path, (int)le, er);
          fclose(f);
     }
}
*/
//---------------------------------------------------------------------------
bool CMaaFile::RemoveDirRecursively(const CMaaString& path, bool bThrow, bool RemoveDirFiles)
{
    return _RemoveDirRecursively(path, bThrow, RemoveDirFiles);
}
//---------------------------------------------------------------------------
bool CMaaFile::_RemoveDirRecursively(const CMaaString &path, bool bThrow, bool RemoveDirFiles)
{
    //int x = path.Length();
    /*
     if      (x + (int)strlen("\\*.*") + 1 >= MAX_PATH)
     {
          return false;
     }
    */

    CMaaFindFile2 ff(path + "\\*.*", 1);
    CMaaFindFile2::sFind f;

    while(ff.Get(f))
    {
        if  (f.m_Type == CMaaFindFile2::sFind::eDot || f.m_Type == CMaaFindFile2::sFind::eDotDot)
        {
            //continue;
        }
        else if (f.m_Type == CMaaFindFile2::sFind::eFile)
        {
            if  (RemoveDirFiles)
            {
                if  (!RemoveEx(f.m_FileName, bThrow))
                {
                    //FileDelErrorLog(f.m_FileName, false);
                }
            }
        }
        else if (f.m_Type == CMaaFindFile2::sFind::eDir)
        {
            _RemoveDirRecursively(f.m_FileName, bThrow, RemoveDirFiles);
        }
        else
        {
            if  (RemoveDirFiles)
            {
                if  (!RemoveEx(f.m_FileName, false))
                {
                    //FileDelErrorLog(f.m_FileName);
                }
            }
        }
    }
    bool bOK = RmDir(path, bThrow);
    if  (!bOK)
    {
#ifdef _WIN32
        DWORD err = GetLastError();
        //FileDelErrorLog(path, true);
        if  (err == 32)    // busy by other process
        {
            Sleep(300);
            bOK = RmDir(path, false);
            if  (!bOK)
            {
                DWORD err = GetLastError();
                //FileDelErrorLog(path, true);
            }
        }
#endif
    }
    return bOK;
}
//---------------------------------------------------------------------------
bool CMaaFile::CopyDirRecursively(CMaaString SrcPath, CMaaString DstPath, int Deph, bool bThrow)
{
    bool bRet = true;
    SrcPath = MkFsCompatible(SrcPath);
    DstPath = MkFsCompatible(DstPath);
    if  (SrcPath.LastChar() != FILESYSTEM_SLASH)
    {
        SrcPath += szFILESYSTEM_SLASH;
    }
    if  (DstPath.LastChar() != FILESYSTEM_SLASH)
    {
        DstPath += szFILESYSTEM_SLASH;
    }
    if  (SrcPath.ToLower(e_string_depending_all) == DstPath.ToLower(e_string_depending_all)) // if (SrcPath.IsCi(DstPath, e_string_depending_all))
    {
        // except...
        return false;
    }
    CMaaFindFile2 ff(SrcPath + "*", Deph);
    ff.SetFileTypeMasks(CMaaFindFile2::eDirMask|CMaaFindFile2::eFileMask);
    CMaaFindFile2::sFind f;
    while(ff.Get(f))
    {
        CMaaString FileName = f.m_FileName;
        if  (FileName.RefLeft(SrcPath.Length()).ToLower(e_string_depending_all) == SrcPath.ToLower(e_string_depending_all)) // if  (FileName.IsLeftCi(SrcPath, e_string_depending_all))
        {
            CMaaString RelativeFileName = FileName.RefMid(SrcPath.Length());
            if  (IsADir(FileName))
            {
                MkDir(DstPath + RelativeFileName, true, bThrow);
            }
            else
            {
                bRet = CopyFile(FileName, DstPath + RelativeFileName, eCreateFolder, bThrow) || bRet;
            }
        }
    }
    return bRet;
}
//---------------------------------------------------------------------------
CMaaFile * g_log = nullptr;
//---------------------------------------------------------------------------
#ifdef _WIN32
DEF_ALLOCATOR(CMaaFindFile2::CMaaFindFile2Imp)
#endif
//---------------------------------------------------------------------------
#ifdef _WIN32
CMaaFindFile2::CMaaFindFile2(const CMaaString& Dir, const CMaaString& Mask, int iRecursiveDepth) noexcept(noexcept_new)
:   m_pImp{ TL_NEW CMaaFindFile2Imp(Dir, Mask, iRecursiveDepth) }
{
    if (m_pImp)
    {
        m_Stack.AddAtFront(m_pImp);
    }
}
CMaaFindFile2::CMaaFindFile2(const CMaaString& DirWithMask, int iRecursiveDepth) noexcept(noexcept_new)
:   m_pImp{ TL_NEW CMaaFindFile2Imp(DirWithMask, iRecursiveDepth) }
{
    if (m_pImp)
    {
        m_Stack.AddAtFront(m_pImp);
    }
}
#else
CMaaFindFile2::CMaaFindFile2(const CMaaString& Dir, const CMaaString& Mask, int iRecursiveDepth) noexcept(noexcept_new)
:   m_Imp(Dir, Mask, iRecursiveDepth)
{
}
CMaaFindFile2::CMaaFindFile2(const CMaaString& DirWithMask, int iRecursiveDepth) noexcept(noexcept_new)
:   m_Imp(DirWithMask, iRecursiveDepth)
{
}
#endif

CMaaFindFile2::~CMaaFindFile2()
{
}
CMaaFindFile2::CMaaFindFile2Imp::CMaaFindFile2Imp(const CMaaString& Dir, CMaaString Mask, int iRecursiveDepth) noexcept(noexcept_new)
{
    m_Dir = Dir;
    if  (iRecursiveDepth > 1 || iRecursiveDepth < 0)
    {
        m_Mask = Mask;
    }
    else
    {
#ifdef _WIN32
        if  (Mask[0] == FILESYSTEM_SLASH || Mask[1] == ':')
        {
            m_Mask = Mask;
            Mask = "*.*";
        }
        else
        {
            m_Mask = CMaaString("*") + szFILESYSTEM_SLASH + Mask;
        }
#endif
#ifdef __unix__
        if  (Mask[0] == FILESYSTEM_SLASH)
        {
            m_Mask = Mask;
        }
        else
        {
            m_Mask = CMaaString("*") + szFILESYSTEM_SLASH + Mask;
        }
#endif
    }
    m_Dir = m_Dir.Str0Copy();
    m_Mask = m_Mask.Str0Copy();
    m_iRecursiveDepth = iRecursiveDepth;
#ifdef _WIN32
    //m_h = _findfirst64(CMaaFile::_uc_MkCompatible(Dir + szFILESYSTEM_SLASH + Mask), &m_ff);
    CMaaString MyPath = MkFsNativeCompatible(Dir + szFILESYSTEM_SLASH + CMaaString((iRecursiveDepth > 1 || iRecursiveDepth < 0) ? CMaaString("*.*") : Mask));
#ifdef _UNICODE
    m_h = _wfindfirst64(MkFs_cast(MyPath), &m_ff);
#else
    m_h = _findfirst64(MkFs_cast(MyPath), &m_ff);
#endif
#endif
#ifdef __unix__
    m_iGetKeepDev = 0;
    CMaaString DirEx = MkFsNativeCompatible(Dir);
    char * Paths[2] = {(char *)(const char *)DirEx, nullptr};
    //int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR | FTS_COMFOLLOW | FTS_PHYSICAL;
    //int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR | ((m_Flags & eDoNotFollowLinks) ? FTS_PHYSICAL : FTS_COMFOLLOW | FTS_LOGICAL);
    int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR | FTS_COMFOLLOW | FTS_LOGICAL;
    // FTS_COMFOLLOW - follow symlinks in the Paths
    // FTS_LOGICAL - deep into symlinks
    // FTS_PHYSICAL - return sl type instead of target fs entry

    m_fts = fts_open(Paths, ftsoptions, nullptr);
    m_entry = nullptr;
#endif
}
//---------------------------------------------------------------------------
CMaaFindFile2::CMaaFindFile2Imp::CMaaFindFile2Imp(const CMaaString& DirWithMask, int iRecursiveDepth) noexcept(noexcept_new)
{
    m_iRecursiveDepth = iRecursiveDepth;
    CMaaString Dir = CMaaFile::MkCompatible(DirWithMask);
    const int n = (warning_int)Dir.ReverseFind(FILESYSTEM_SLASH);
    if  (n < 0)
    {
        //char pfx[3] = {'.', FILESYSTEM_SLASH, 0};
        //Dir = CMaaString(pfx) + Dir;
        m_Dir = ".";
        m_Mask = CMaaString("*" szFILESYSTEM_SLASH) + DirWithMask;
    }
    else
    {
        m_Dir = DirWithMask.RefLeft(n == 0 ? 1 : n);
        m_Mask = DirWithMask;
    }
    m_Dir = CMaaFile::MkCompatible(m_Dir);
    m_Mask = CMaaFile::MkCompatible(m_Mask);
    m_Dir = m_Dir.Str0Copy();
    m_Mask = m_Mask.Str0Copy();
#ifdef _WIN32
    CMaaString tmp = (iRecursiveDepth > 1 || iRecursiveDepth < 0) ? MkFsNativeCompatible((m_Dir + szFILESYSTEM_SLASH + "*.*")) : MkFsNativeCompatible(Dir);
#ifdef _UNICODE
    m_h = _wfindfirst64(MkFs_cast(tmp), &m_ff);
#else
    m_h = _findfirst64(MkFs_cast(tmp), &m_ff);
#endif
#endif
#ifdef __unix__
    m_iGetKeepDev = 0;
    CMaaString DirEx = MkFsNativeCompatible(m_Dir);
    char * Paths[2] = { (char *)(const char *)DirEx, nullptr };
    //        int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR;
    //int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR | ((m_Flags & eDoNotFollowLinks) ? FTS_PHYSICAL : FTS_COMFOLLOW | FTS_LOGICAL);
    int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR | FTS_COMFOLLOW | FTS_LOGICAL;
    //int ftsoptions = FTS_SEEDOT | FTS_NOCHDIR | FTS_COMFOLLOW | FTS_PHYSICAL;// | FTS_WHITEOUT;
    // FTS_COMFOLLOW - follow symlinks in the Paths
    // FTS_LOGICAL - deep into symlinks
    // FTS_PHYSICAL - return sl type instead of target fs entry
    m_fts = fts_open(Paths, ftsoptions, nullptr);
    //printf("fts_open(%s): ret %d\n", (const char *)DirEx, m_fts);
    //g_log && g_log->fprintf("fts_open(%s): ret %d\n", (const char *)DirEx, m_fts);
    //printf("m_Dir=%s, m_Mask=%s, depth=%d\n", (const char *)m_Dir, (const char *)m_Mask, m_iRecursiveDepth);
    m_entry = nullptr;
#endif
}
//---------------------------------------------------------------------------
CMaaFindFile2::CMaaFindFile2Imp::~CMaaFindFile2Imp()
{
#ifdef _WIN32
    if (m_h != -1)
    {
        _findclose(m_h);
    }
#endif
#ifdef __unix__
    if (m_fts)
    {
        fts_close(m_fts);
    }
#endif
}
//---------------------------------------------------------------------------
bool CMaaFindFile2::IsDirOpened() const noexcept
{
#ifdef _WIN32
    return m_pImp && m_pImp->m_h != -1;
#endif
#ifdef __unix__
    return m_Imp.m_fts;
#endif
}
//---------------------------------------------------------------------------
int CMaaFindFile2::SetOptFlags(int Flags) noexcept
{
    if  (Flags == -1)
    {
        return m_Flags;
    }
#ifdef __unix__
    if  ((Flags & eGetKeepDev))
    {
        m_Imp.m_iGetKeepDev = 1;
    }
#endif
    return m_Flags = Flags;
}
//---------------------------------------------------------------------------
int CMaaFindFile2::SetFileTypeMasks(int Masks) noexcept
{
    if  (Masks == -1)
    {
        return m_FileTypeMask;
    }
    return m_FileTypeMask = Masks;
}
//---------------------------------------------------------------------------
bool CMaaFindFile2::CMaaFindFile2Imp::Get(sFind& f, CMaaFindFile2& ff) noexcept(noexcept_new)
{
#ifdef _WIN32
    if  (m_h == -1)
    {
        return false;
    }
    if  (m_b1st)
    {
        m_b1st = false;
    }
    else
    {
        if  (
#ifdef _UNICODE
             _wfindnext64
#else
             _findnext64
#endif
             (m_h, &m_ff) == -1)
        {
            _findclose(m_h);
            m_h = -1;
            return false;
        }
    }
    f.m_Fn = CMaaFile::_utf8_path(m_ff.name);
    f.m_Size = m_ff.size;
    f.m_mTime = m_ff.time_write;
    f.m_Type = CMaaFindFile2::sFind::eUnknown;
    f.m_mft = 0;
    if  ((m_ff.attrib != INVALID_FILE_ATTRIBUTES) && (m_ff.attrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        f.m_Type = CMaaFindFile2::sFind::eDir;
        if  (f.m_Fn == "." || f.m_Fn.IsRight("\\.", 2))
        {
            f.m_Type = CMaaFindFile2::sFind::eDot;
        }
        if  (f.m_Fn == ".." || f.m_Fn.IsRight("\\..", 3))
        {
            f.m_Type = CMaaFindFile2::sFind::eDotDot;
        }
    }
    if  ((m_ff.attrib != INVALID_FILE_ATTRIBUTES) && !(m_ff.attrib & FILE_ATTRIBUTE_DIRECTORY))
    {
        f.m_Type = CMaaFindFile2::sFind::eFile;
    }
    f.m_Dir = m_Dir;
    f.m_FileName = m_Dir + szFILESYSTEM_SLASH + f.m_Fn;
    f.m_px = &m_ff;
    if  (f.m_Type == CMaaFindFile2::sFind::eDir && (m_iRecursiveDepth > 1 || m_iRecursiveDepth < 0))
    {
        CMaaFindFile2Imp * pNext = TL_NEW CMaaFindFile2Imp(f.m_FileName, m_Mask, m_iRecursiveDepth - 1);
        if (pNext)
        {
            ff.m_Stack.AddAtFront(pNext);
        }
    }
#endif
#ifdef __unix__
    if  (!m_fts)
    {
        return false;
    }
    int Type = CMaaFindFile2::sFind::eUnknown;
    while(Type == CMaaFindFile2::sFind::eUnknown)
    {
        m_entry = fts_read(m_fts);
        if  (!m_entry)
        {
            return false;
        }
        //g_log && g_log->fprintf("m_entry->fts_path = %s, m_entry->fts_info = %d\n", m_entry->fts_path, (int)m_entry->fts_info);

        CMaaString fn;
        fn.Format("%s", m_entry->fts_path);
        switch(m_entry->fts_info)
        {
        case FTS_F:
            Type = CMaaFindFile2::sFind::eFile;
            if  (m_entry->fts_statp)
            {
                if  (m_iGetKeepDev == 1)
                {
                    m_dev = m_entry->fts_statp->st_dev;
                    m_iGetKeepDev++;
                }
            }
            break;
        case FTS_D:
            if  (gToolsLib_FileFlags & TOOLSLIB_SKIP_LN_PROC)
            {
                CMaaString p = m_entry->fts_path;
                struct stat buf;
                memset(&buf, 0, sizeof(buf));
                if  ((p == "/proc" || p.IsLeft("/proc/", 6) || p == "/run" || p.IsLeft("/run/", 5) || p == "/dev" || p.IsLeft("/dev/", 5))
//                       && !lstat(m_entry->fts_path, &buf) && (buf.st_mode & S_IFLNK)
                     )
                {
                    fts_set(m_fts, m_entry, FTS_SKIP);
                    break;
                }
            }
            if  (m_entry->fts_statp)
            {
                if  (m_iGetKeepDev == 1)
                {
                    m_dev = m_entry->fts_statp->st_dev;
                    m_iGetKeepDev++;
                }
                if  (m_iGetKeepDev == 2 && m_entry->fts_statp->st_dev != m_dev)
                {
                    fts_set(m_fts, m_entry, FTS_SKIP);
                }
                else if (m_hDirInodes.Add(m_entry->fts_statp->st_ino, 1)) //ino_t
                {
                    fts_set(m_fts, m_entry, FTS_SKIP);
                }
            }
            struct stat buf;
            memset(&buf, 0, sizeof(buf));
            lstat(m_entry->fts_path, &buf);
            if  (ff.m_Flags & eDoNotFollowLinks)
            {
                //f.fprintf(" (m_Flags & eDoNotFollowLinks), S_IFLNK=%o ", S_IFLNK);
            }

            if  ((ff.m_Flags & eDoNotFollowLinks) && !lstat(m_entry->fts_path, &buf) && (buf.st_mode & S_IFLNK))
            {
                fts_set(m_fts, m_entry, FTS_SKIP);
                m_hDirInodes.Remove(m_entry->fts_statp->st_ino);
            }
            if  (m_entry->fts_level <= 0)
            {
                break;
            }
            //printf("DirName: %s\n", entry->fts_path);
            if  (m_entry->fts_level >= m_iRecursiveDepth && m_iRecursiveDepth >= 1)
            {
                fts_set(m_fts, m_entry, FTS_SKIP);
                m_hDirInodes.Remove(m_entry->fts_statp->st_ino);
            }
            Type = CMaaFindFile2::sFind::eDir;
            break;
        case FTS_DOT:
            Type = CMaaFindFile2::sFind::eDot;
            break;
        case FTS_SL:
            if  (ff.m_Flags & eDoNotFollowLinks)
            {
                fts_set(m_fts, m_entry, FTS_SKIP);
                m_hDirInodes.Remove(m_entry->fts_statp->st_ino);
            }
            Type = CMaaFindFile2::sFind::eSl;
            break;
        case FTS_DP:
            //printf ( "%s - postorder directory\n", entry->fts_path );
            //break;
        case FTS_DNR:
        case FTS_ERR:
        case FTS_NS:
            //(void)fflush(stdout);
            //printf ( "%s: %s", entry->fts_path, strerror(entry->fts_errno));
            //rval = 1;
            break;
        }
    }
    f.m_FileName = m_entry->fts_path;
    const int n = (warning_int)f.m_FileName.ReverseFind(FILESYSTEM_SLASH);
    if (n < 0)
    {
        f.m_Fn = f.m_FileName;
        f.m_Dir.Empty();
    }
    else if (!n)
    {
        f.m_Fn = f.m_FileName.RefMid(1);
        f.m_Dir = CMaaFileSystemSlash; // szFILESYSTEM_SLASH;
    }
    else
    {
        f.m_Fn = f.m_FileName.RefMid(n + 1);
        f.m_Dir = f.m_FileName.RefLeft(n); // optimized NZT RefLeft CMaaString
    }
    f.m_Size = m_entry->fts_statp ? m_entry->fts_statp->st_size : 0;
    f.m_mTime = m_entry->fts_statp ? m_entry->fts_statp->st_mtime : 0;
    f.m_Type = Type;
    f.m_mft = 0;
    if  (Type == CMaaFindFile2::sFind::eDot)
    {
        if  (f.m_FileName.IsRight("..", 2))
        {
            f.m_Type = CMaaFindFile2::sFind::eDotDot;
        }
    }
    //g_log && g_log->fprintf("f.m_Type = %d (%d)\n", f.m_Type, CMaaFindFile2::sFind::eFile);
    f.m_px = m_entry;
    //f.m_px->fts_statp->st_dev
    //f.m_px->fts_statp->st_ino
#endif
    if  (ff.m_Flags & (eFt|eForcedFt))
    {
        int usec = 0;
        const time_t t = CMaaFile::GetDateTime(f.m_FileName, &usec, false);
        f.m_mft = t != (time_t)-1 ? TimetAndUsecTo_FileTime64(t, usec) : -1;
        if  (t != (time_t)-1)
        {
            f.m_mTime = t;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool CMaaFindFile2::Get(sFind &f) noexcept(noexcept_new)
{
    while(1)
    {
#ifdef _WIN32
        CMaaFindFile2Imp * p = m_Stack.LookAtFront();
        if (!p)
        {
            return false;
        }
        if  (!p->Get(f, *this))
        {
            delete m_Stack.GetFromFront();
            if (p != m_pImp)
            {
                continue;
            }
            m_pImp = nullptr;
            return false;
        }
#else
        if (!m_Imp.Get(f, *this))
        {
            return false;
        }
#endif
        int Type = f.m_Type;
        Type = Type > eSl ? 31 : Type;
        if  (!(m_FileTypeMask & (1 << Type)))
        {
            continue;
        }
#ifdef _WIN32
        if  (f.m_FileName.IsMatchFileMask(m_pImp->m_Mask, 0, &m_pm))
#else
        if (f.m_FileName.IsMatchFileMask(m_Imp.m_Mask, 0, &m_pm))
#endif
        {
            break;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
#if 0
CMaaString CMaaFindFile2::sFind::GetFileName() const noexcept
{
#ifdef _WIN32
    return m_Fn;
#else
    const int n = (warning_int)m_FileName.ReverseFind(FILESYSTEM_SLASH);
    return (n >= 0) ? m_FileName.RefMid(n + 1) : m_FileName;
#endif
}
//---------------------------------------------------------------------------
CMaaString CMaaFindFile2::sFind::GetDirName() const // with out if ending slash
#if (ALLOW_NZT_FILENAMES)
    noexcept
#endif
{
#ifdef _WIN32
    return m_Dir;
#else
    const int n = (warning_int)m_FileName.ReverseFind(FILESYSTEM_SLASH);
    if constexpr (ALLOW_NZT_FILENAMES)
    {
        return (n >= 0) ? m_FileName.RefLeft(n > 0 ? n : 1) : CMaaStringZ;
    }
    else
    {
        return (n >= 0) ? m_FileName.Left(n > 0 ? n : 1).Str0Copy() : CMaaStringZ;
    }
#endif
}
//---------------------------------------------------------------------------
CMaaString CMaaFindFile2::sFind::GetDirName0() const noexcept(noexcept_new) // with out if ending slash, 0-terminating CMaaString
{
#ifdef _WIN32
    return m_Dir;
#else
    const int n = (warning_int)m_FileName.ReverseFind(FILESYSTEM_SLASH);
    return (n >= 0) ? m_FileName.Left(n > 0 ? n : 1).Str0Copy() : CMaaStringZ;
#endif
}
//---------------------------------------------------------------------------
CMaaString CMaaFindFile2::sFind::GetDirName2() const noexcept // with out if ending slash, optimized NZT RefLeft CMaaString
{
#ifdef _WIN32
    return m_Dir;
#else
    const int n = (warning_int)m_FileName.ReverseFind(FILESYSTEM_SLASH);
    return (n >= 0) ? m_FileName.RefLeft(n > 0 ? n : 1) : CMaaStringZ;
#endif
}
#endif // 0
//---------------------------------------------------------------------------
#if 0
static constexpr const char * StaticStrFileTypeNames[6][5] =
{
    {"dot ",       "dot    ",     "dot",        "    dot",     "dot "},
    {"dotdot",     "dotdot ",     "dotdot",     " dotdot",     "dotdot"},
    {"dir ",       "dir    ",     "dir",        "    dir",     " dir"},
    {"file",       "file   ",     "file",       "   file",     "file"},
    {"symlink",    "symlink",     "symlink",    "symlink",     "symlink"},
    {"unknown",    "unknown",     "unknown",    "unknown",     "unknown"}
};
//---------------------------------------------------------------------------
const char* CMaaFindFile2::sFind::GetTypeName(int Align) const noexcept
{
    Align = Align <= -2 ? 0 : Align == -1 ? 1 : Align == 0 ? 2 : Align == 1 ? 3 : 4;
    int t = m_Type;
    t = t == CMaaFindFile2::sFind::eFile ? 3 :
        t == CMaaFindFile2::sFind::eDir ? 2 :
        t == CMaaFindFile2::sFind::eDot ? 0 :
        t == CMaaFindFile2::sFind::eDotDot ? 1 :
        t == CMaaFindFile2::sFind::eSl ? 4 : 5;
    return StaticStrFileTypeNames[t][Align];
}
const CMaaString & CMaaFindFile2::sFind::GetTypeNameStr(int Align) const noexcept
{
    Align = Align <= -2 ? 0 : Align == -1 ? 1 : Align == 0 ? 2 : Align == 1 ? 3 : 4;
    int t = m_Type;
    t = t == CMaaFindFile2::sFind::eFile ? 3 :
        t == CMaaFindFile2::sFind::eDir ? 2 :
        t == CMaaFindFile2::sFind::eDot ? 0 :
        t == CMaaFindFile2::sFind::eDotDot ? 1 :
        t == CMaaFindFile2::sFind::eSl ? 4 : 5;

    CMaaWin32Locker_aLocker_gLock_Atomic;
    aLocker_Lock;
    static CMaaString strStaticStrFileTypeNames[6][5];
    if (strStaticStrFileTypeNames[t][Align].IsEmpty())
    {
        strStaticStrFileTypeNames[t][Align] = CMaaString(StaticStrFileTypeNames[t][Align], CMaaString::eROStrlenMemString);
    }
    aLocker_UnLock;
    return strStaticStrFileTypeNames[t][Align];
}
#else

//---------------------------------------------------------------------------
const char* CMaaFindFile2::sFind::GetTypeName(int Align) const noexcept
{
    Align = Align <= -2 ? 0 : Align == -1 ? 1 : Align == 0 ? 2 : Align == 1 ? 3 : 4;
    int t = m_Type;
    t = t == CMaaFindFile2::sFind::eFile ? 3 :
        t == CMaaFindFile2::sFind::eDir ? 2 :
        t == CMaaFindFile2::sFind::eDot ? 0 :
        t == CMaaFindFile2::sFind::eDotDot ? 1 :
        t == CMaaFindFile2::sFind::eSl ? 4 : 5;
    return CMaaFile::C2().s_ft[t * 5 + Align].m_pImp->m_pszStr;
}
const CMaaString & CMaaFindFile2::sFind::GetTypeNameStr(int Align) const noexcept
{
    Align = Align <= -2 ? 0 : Align == -1 ? 1 : Align == 0 ? 2 : Align == 1 ? 3 : 4;
    int t = m_Type;
    t = t == CMaaFindFile2::sFind::eFile ? 3 :
        t == CMaaFindFile2::sFind::eDir ? 2 :
        t == CMaaFindFile2::sFind::eDot ? 0 :
        t == CMaaFindFile2::sFind::eDotDot ? 1 :
        t == CMaaFindFile2::sFind::eSl ? 4 : 5;

    return ((const CMaaString&)CMaaFile::C2().s_ft[t * 5 + Align]);
}
#endif
//---------------------------------------------------------------------------
int CMaaFindFile2::sFind::GetMaxTypeNameLength(int FileOrDirOnly) noexcept
{
    return FileOrDirOnly ? 4 : 7;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::ReadAll()
{
    if  (IsOpen())
    {
        _qword pos = GetCurPos();
        _qword len = Length();
        if  (pos >= 0 && len >= pos)
        {
            len -= pos;
            if  (len < 0x7fffffff)
            {
                CMaaString Buf(nullptr, (int)len, CMaaString::eNotInitMem);
                if  (Buf.Length() == len)
                {
                    pos = Read(Buf.GetBuffer(), (int)len);
                    if  (pos == len)
                    {
                        return Buf;
                    }
                    if  (pos >= 0)
                    {
                        return Buf.Left((int)pos);
                    }
                }
                else
                {
                    SetError("Bad implementation: alloc", 0);
                }
            }
            else
            {
                SetError("Bad implementation: >2GB", 0);
            }
        }
    }
    return CMaaStringZ;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::ReadAllSysFile() noexcept
{
    CMaaString r;
    try
    {
        if (IsOpen())
        {
            while (1)
            {
                char Buffer[TOOLSLIB_CS_64K];
                int len = (int)Read(Buffer, TOOLSLIB_CS_64K);
                if (!len)
                {
                    break;
                }
                r.Add(Buffer, len);
            }
        }
    }
    catch (...)
    {
    }
    return r;
}
//---------------------------------------------------------------------------
CMaaString CMaaFile::Read(_dword MaxLen, int _Convert)
{
    CMaaString Txt;
    int fmt = _Convert && IsOpen() ? (int)m_pImp->m_SrcFormat : 0;
    int dstfmt = _Convert && IsOpen() ? (int)m_pImp->m_DstFormat : 0;
    _dword t = 0;
    try
    {
        {
            CMaaConcatString1 str(TOOLSLIB_CS_64K /*MaxLen*/);
            while(t < MaxLen)
            {
                char Buffer[TOOLSLIB_CS_64K];
                _dword l0 = MaxLen - t;
                if  (l0 > sizeof(Buffer))
                {
                    l0 = sizeof(Buffer);
                }
                _dword l = Read(Buffer, l0);
                t += l;
                str.Add(Buffer, (int)l);
                if  (l != l0)
                {
                    break;
                }
            }
            if  ((_Convert & 3) == 1 && (t & 1) && (fmt == ePfxUnicode || fmt == ePfxUnicodeBigEndian))
            {
                t++;
                str += '\0';
            }
            Txt = (CMaaString)str;
        }

        if  (_Convert && t > 0)
        {
            if  (t != (_dword)Txt.GetLength())
            {
                throw 1;
            }
            const _dword x = t;
            _dword i = 0;
            char * pszBuffer = Txt.GetBuffer();
            switch(fmt)
            {
                //case ePfxUnicode:
                /*
                    for  (i = 0; i < x - 1; i += 2)
                    {
                         _WC_ wch = (_WC_)(unsigned char)pszBuffer[i] + (((_WC_)(unsigned char)pszBuffer[i + 1]) << 8);
                         if   ((wch == 0 && !(Flags & 0x100)) || wch == '\r' || wch == '\n')
                         {
                              break;
                         }
                    }
                    */
                //     break;
            case ePfxUnicodeBigEndian:
                fmt = ePfxUnicode;
                for (i = 0; i < x - 1; i += 2)
                {
                    char ch2[2] = {pszBuffer[i + 1], pszBuffer[i]};
                    pszBuffer[i] = ch2[0];
                    pszBuffer[i + 1] = ch2[1];
                }
                break;
                //case ePfxUtf8:
                //default:
            }
            if  (Txt.IsNotEmpty() && fmt != dstfmt)
            {
                CMaaString Unicode = fmt == ePfxUnicode ? Txt : fmt == ePfxUtf8 ? Utf8ToUnicode(Txt) : AnsiToUnicode(Txt);
                Txt = dstfmt == ePfxUtf8 ? UnicodeToUtf8(Unicode) : dstfmt == ePfxUnicode ? Unicode : dstfmt == ePfxUnicodeBigEndian ? UnicodeToUnicodeBigEndian(Unicode) : UnicodeToAnsi(Unicode);
            }
        }
    }
    catch(XTOOFile2Error err)
    {
        throw;
    }
    catch(...)
    {
        SetError("Alloc error", 0);
        Txt.Empty();
    }
    return Txt;
}
//---------------------------------------------------------------------------
_dword CMaaFile::Write(const CMaaString &Txt)
{
    return Write(Txt, (warning_dword)Txt.Length());
}
//---------------------------------------------------------------------------




//-----------------------------------------------------------------------------
#ifdef _WIN32
HANDLE h__utf8_printf_StdOutHandle = nullptr;
//---------------------------------------------------------------------------
int __utf8_printf_isatty() noexcept
{
    return 0;
}
//---------------------------------------------------------------------------
CMaaDllProc::CMaaDllProc()
:   f(12345)
{
}
//---------------------------------------------------------------------------
CMaaDllProc::~CMaaDllProc()
{
    if  (f == 54321)
    {
        f = 12345;
        p = NULL;
        if  (h)
        {
            FreeLibrary(h);
            h = NULL;
        }
    }
}
//---------------------------------------------------------------------------
FARPROC CMaaDllProc::Get(const wchar_t* Dll, const char* Proc) noexcept
{
    if  (f == 12345)
    {
#ifdef _UNICODE
        h = LoadLibrary(Dll);
#else
        h = LoadLibraryW(Dll);
#endif
        p = h ? GetProcAddress(h, Proc) : NULL;
        f = 54321;
    }
    return f == 54321 ? p : NULL;
}
//---------------------------------------------------------------------------
typedef BOOL (WINAPI *My_t_SetCurrentConsoleFontEx)(
    _In_ HANDLE hConsoleOutput,
    _In_ BOOL bMaximumWindow,
    _In_ PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx
);
//---------------------------------------------------------------------------
BOOL WINAPI MySetCurrentConsoleFontEx(
    _In_ HANDLE               hConsoleOutput,
    _In_ BOOL                 bMaximumWindow,
    _In_ PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx
)
{
    static CMaaDllProc dp;
    My_t_SetCurrentConsoleFontEx My_SetCurrentConsoleFontEx = (My_t_SetCurrentConsoleFontEx)dp.Get(L"Kernel32.dll", "SetCurrentConsoleFontEx");
    if  (My_SetCurrentConsoleFontEx)
    {
        return My_SetCurrentConsoleFontEx(hConsoleOutput, bMaximumWindow, lpConsoleCurrentFontEx);
    }
    return FALSE;
}
//---------------------------------------------------------------------------
typedef BOOL(WINAPI* My_t_GetCurrentConsoleFontEx)(
    _In_  HANDLE               hConsoleOutput,
    _In_  BOOL                 bMaximumWindow,
    _Out_ PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx
);
//---------------------------------------------------------------------------
BOOL WINAPI MyGetCurrentConsoleFontEx(
    _In_  HANDLE               hConsoleOutput,
    _In_  BOOL                 bMaximumWindow,
    _Out_ PCONSOLE_FONT_INFOEX lpConsoleCurrentFontEx
)
{
    static CMaaDllProc dp;
    My_t_GetCurrentConsoleFontEx My_GetCurrentConsoleFontEx = (My_t_GetCurrentConsoleFontEx)dp.Get(L"Kernel32.dll", "GetCurrentConsoleFontEx");
    if  (My_GetCurrentConsoleFontEx)
    {
        return My_GetCurrentConsoleFontEx(hConsoleOutput, bMaximumWindow, lpConsoleCurrentFontEx);
    }
    return FALSE;
}
//---------------------------------------------------------------------------
bool MaaSetConsoleLucidaFont()
{
    HANDLE StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX info;
    memset(&info, 0, sizeof(CONSOLE_FONT_INFOEX));
    info.cbSize = sizeof(CONSOLE_FONT_INFOEX);              // prevents err=87 below
    if  (MyGetCurrentConsoleFontEx(StdOut, FALSE, &info))
    {
        info.FontFamily = FF_DONTCARE;
        info.dwFontSize.X = 0;  // leave X as zero
        info.dwFontSize.Y = 14;
        info.FontWeight = 400;
        //_tcscpy_s
        wcscpy_s(info.FaceName, L"Lucida Console");
        if  (MySetCurrentConsoleFontEx(StdOut, FALSE, &info))
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool MaaSetConsoleUtf8AndLocale()
{
    //char* a = setlocale(LC_ALL, ".1251");
    //SetConsoleOutputCP(CP_UTF8);
    //SetConsoleCP(CP_UTF8);
    //char* a = setlocale(LC_ALL, ".1200"); // UTF-16 little-endian locale.
    SetConsoleOutputCP(CP_UTF8);
    //SetConsoleCP(CP_ACP);
    SetConsoleCP(CP_UTF8); // Don't use gets(). Use getch() or ReadConsoleInput() Windows; getchar() linux

    MaaSetConsoleLucidaFont();
    return false;
}
//---------------------------------------------------------------------------
CMaaSetConsoleUtf8AndLocale::CMaaSetConsoleUtf8AndLocale(bool bPreserveCp)
:   m_bPreserveCp(bPreserveCp)
{
    m_pPresLocale = nullptr;// setlocale(LC_ALL, nullptr);
    m_ConsoleCp = GetConsoleCP();
    m_ConsoleOutputCp = GetConsoleOutputCP();
    SetUtf8();
}
//---------------------------------------------------------------------------
CMaaSetConsoleUtf8AndLocale::~CMaaSetConsoleUtf8AndLocale()
{
    if  (m_bPreserveCp)
    {
        Restore();
    }
}
//---------------------------------------------------------------------------
void CMaaSetConsoleUtf8AndLocale::SetUtf8()
{
    //char* a = setlocale(LC_ALL, ".1251");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); // Don't use gets(). Use getchar(), ReadConsoleInput()
    //MaaSetConsoleLucidaFont();
    {
        HANDLE StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_FONT_INFOEX info;
        memset(&m_cfi, 0, sizeof(CONSOLE_FONT_INFOEX));
        m_cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);              // prevents err=87 below
        if  (MyGetCurrentConsoleFontEx(StdOut, FALSE, &m_cfi))
        {
            info = m_cfi;
            info.FontFamily = FF_DONTCARE;
            //info.FontFamily = FF_ROMAN;// TMPF_TRUETYPE;
            info.nFont = 0;
            //info.dwFontSize.X = 0;  // leave X as zero
            //info.dwFontSize.Y = 14;
            info.dwFontSize.X = 8;  // leave X as zero
            info.dwFontSize.Y = 16;
            info.FontWeight = FW_NORMAL;// 400
            info.FontWeight = FW_DONTCARE;// 0
            m_uConsoleFont = UnicodeString(m_cfi.FaceName);
            //_tcscpy_s
            wcscpy_s(info.FaceName, L"Consolas");
            //wcscpy_s(info.FaceName, L"Lucida Console");
            //wcscpy_s(info.FaceName, L"Times New Roman");
            //wcscpy_s(info.FaceName, L"MS Gothic");
            //wcscpy_s(info.FaceName, L"Arial Unicode MS");
            //wcscpy_s(info.FaceName, L"Courier New");
            //wcscpy_s(info.FaceName, L"GNU Unifont"); // -
            if  (MySetCurrentConsoleFontEx(StdOut, FALSE, &info))
            {
                //__utf8_printf("\nSet font - ok\n");
                //return true;
            }
            else
            {
                __utf8_printf("Error set font\n");
            }
        }
        //return false;
    }
}
//---------------------------------------------------------------------------
void CMaaSetConsoleUtf8AndLocale::Restore()
{
    if  (m_uConsoleFont.IsNotEmpty())
    {
        HANDLE StdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        //CONSOLE_FONT_INFOEX info;
        //memset(&info, 0, sizeof(CONSOLE_FONT_INFOEX));
        //info.cbSize = sizeof(CONSOLE_FONT_INFOEX);              // prevents err=87 below
        //if (MyGetCurrentConsoleFontEx(StdOut, FALSE, &info))
        {
            //info.FontFamily = FF_DONTCARE;
            //info.dwFontSize.X = 0;  // leave X as zero
            //info.dwFontSize.Y = 14;
            //info.FontWeight = 400;
            //_tcscpy_s
            //wcscpy_s(info.FaceName, (const wchar_t *)(const char *)m_uConsoleFont);
            if  (MySetCurrentConsoleFontEx(StdOut, FALSE, &m_cfi))
            {
                //__utf8_printf("\nRestored\n");
                //return true;
            }
            else
            {
                __utf8_printf("\nError restore font\n");
            }
        }
    }
    SetConsoleOutputCP(m_ConsoleOutputCp);
    SetConsoleCP(m_ConsoleCp);
    //setlocale(LC_ALL, m_pPresLocale);
}
//---------------------------------------------------------------------------
#else // _WIN32
//---------------------------------------------------------------------------
int h__utf8_printf_StdOutHandle = -1;
//---------------------------------------------------------------------------
int __utf8_printf_isatty() noexcept
{
    return h__utf8_printf_StdOutHandle != -1 ? isatty(h__utf8_printf_StdOutHandle) : 0;
}
//---------------------------------------------------------------------------
bool MaaSetConsoleLucidaFont()
{
    return false;
}
//---------------------------------------------------------------------------
bool MaaSetConsoleUtf8AndLocale()
{
    //MaaSetConsoleLucidaFont();
    return false;
}
//---------------------------------------------------------------------------
CMaaSetConsoleUtf8AndLocale::CMaaSetConsoleUtf8AndLocale(bool bPreserveCp)
:   m_bPreserveCp(bPreserveCp)
{
    m_pPresLocale = nullptr;// setlocale(LC_ALL, nullptr);
    loc = 0;
    SetUtf8();
}
//---------------------------------------------------------------------------
CMaaSetConsoleUtf8AndLocale::~CMaaSetConsoleUtf8AndLocale()
{
    if  (m_bPreserveCp)
    {
        Restore();
    }
}
//---------------------------------------------------------------------------
void CMaaSetConsoleUtf8AndLocale::SetUtf8()
{
    /*
    loc = newlocale(LC_ALL_MASK, "en_US.utf8", NULL); // 0 on error
    if (loc)
    {
        uselocale(loc); // 0 on error
    }
    */
    //setlocale(LC_ALL, "en_US.utf8");
    //setlocale(LC_ALL, "ru_RU.utf8");
    MaaSetConsoleLucidaFont();
}
//---------------------------------------------------------------------------
void CMaaSetConsoleUtf8AndLocale::Restore()
{
    //setlocale(LC_ALL, m_pPresLocale);
    /*
    if (loc)
    {
        freelocale(loc);
        loc = 0;
    }
    */
}
//---------------------------------------------------------------------------
#endif // _WIN32

//---------------------------------------------------------------------------
constexpr int Utf8CharLen(char u0) noexcept
{
    unsigned char c = (unsigned char)u0;
    if  ((c & 0xE0) == 0xC0)
    {
        return 2;
    }
    if  ((c & 0xf0) == 0xe0)
    {
        return 3;
    }
    if  ((c & 0xf8) == 0xf0)
    {
        return 4;
    }
    return 1;
}
//---------------------------------------------------------------------------
CMaaString MaaReadUtf8ConsoleLine1(CMaaString term_ch, int max_chars, bool bEcho)
{
    char Buffer[TOOLSLIB_CS_64K];
    CMaaConcatString s(Buffer, sizeof(Buffer));
    while(1)
    {
        bool bUtf8CharNotEnded = false;
        int n = 0;
        {
            CMaaString tmp = s.GetMemoryString();
            n = (warning_int)tmp.Utf8CharCount(true);
            if  (n > 0)
            {
                int pos = (warning_int)tmp.Utf8Pos(--n, true);
                int l = Utf8CharLen(tmp[pos]);
                int t = (warning_int)tmp.Length() - pos;
                if  (t >= l)
                {
                    n++;
                }
                else
                {
                    bUtf8CharNotEnded = true;
                }
            }
        }
        if  (max_chars >= 0 && n >= max_chars)
        {
            break;
        }
#ifdef _WIN32
        int ch = getch();
        //__utf8_printf("(%02x)   ", ch);
        if  (!ch)
        {
            ch = 0x100 + getch();
            //__utf8_printf("(00 %02x)   ", ch);
        }
        else
        {
            if (ch == 0xe0) // in linux can be different // && bUtf8CharNotEnded)
            {
                ch = (ch << 8) + getch();
                if (ch == 0xe04b) // left
                {
                    ch = 0x08;
                }
            }
#if 0
        }
#endif
#else
        int ch = getchar();
        if  (ch == EOF)
        {
            break;
        }
        else
        {
            if  (0 && ch == 0x1b) // in linux can be different // && bUtf8CharNotEnded)
            {
                ch = 0x100;
                char ch2 = getchar();
                if  (ch2 == '[')
                {
                    ch2 = getchar();
                    if  (ch2 == 'D') // left
                    {
                        ch = 0x08;
                    }
                }
            }
#endif
            else
            {
                if  (term_ch.Find((char)ch) >= 0)
                {
                    break;
                }
            }
            //if (ch < 0x20)
            //{
            //__utf8_printf("(%02x)   ", ch);
            //}
            if  (ch < 0x100)
            {
                if  (ch != 0x08)
                {
                    int l = Utf8CharLen((char)ch);
                    s += (char)ch;
                    for (int i = 1; i < l; i++)
                    {
#ifdef _WIN32
                        int ch2 = getch();
#else
                        int ch2 = getchar();
                        //if (ch == EOF)
                        //{
                        //    break;
                        //}
#endif
                        s += (char)ch2;
                    }
                    if  (bEcho)
                    {
                        CMaaString tmp = s.GetMemoryString();
                        tmp = tmp.RefRight(l);
                        __utf8_printf("%S", &tmp);
                    }
                }
                else // if (ch == 0x08) // Backspace
                {
                    CMaaString tmp = s.GetMemoryString();
                    int l = (warning_int)tmp.Utf8CharCount();
                    if  (l > 0)
                    {
                        l = (warning_int)tmp.Utf8Pos(l - 1);
                        s.SetNewLengthValue(l);
                        bEcho&& __utf8_printf("%c %c", ch, ch);
                    }
                }
            }
        }
    }
    return s;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int __utf8_printf(const char* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    CMaaString txt;
    txt.FormatV(format, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
int __utf8_printf(const char8_t* format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    CMaaString txt;
    txt.FormatV(format, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
#endif
//---------------------------------------------------------------------------
int __utf8_printf(CMaaString format, ...) noexcept
{
    va_list list;
    va_start(list, format);
    CMaaString txt;
    txt.FormatV(format, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int __utf8_printfV(const char* format, va_list list) noexcept
{
    CMaaString txt;
    txt.FormatV(format, list);
    return __utf8_printf_out(txt);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
int __utf8_printfV(const char8_t* format, va_list list) noexcept
{
    CMaaString txt;
    txt.FormatV(format, list);
    return __utf8_printf_out(txt);
}
#endif
//---------------------------------------------------------------------------
int __utf8_printfV(CMaaString format, va_list list) noexcept
{
    CMaaString txt;
    txt.FormatV(format, list);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int __utf8_printfV2(CMaaString Format, CMaaString Text, va_list list) noexcept
{
    CMaaString txt;
    txt.FormatV2(Format, Text, list);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int __utf8_printf2(const char *Format, const char * pText, ...) noexcept
{
    va_list list;
    va_start(list, pText);
    pText = pText ? pText : "(null text2)";
    CMaaString txt, Text((const void*)pText, CMaaString::eROStrlenMemString); // RO?
    txt.FormatV2(Format, Text, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int __utf8_printf2(const char* Format, CMaaString Text, ...) noexcept
{
    va_list list;
    va_start(list, Text);
    CMaaString txt;
    txt.FormatV2(Format, Text, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int __utf8_printf2(CMaaString Format, CMaaString Text, ...) noexcept
{
    va_list list;
    va_start(list, Text);
    CMaaString txt;
    txt.FormatV2(Format, Text, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int __utf8_printf2(CMaaString Format, const char * pText, ...) noexcept
{
    va_list list;
    va_start(list, pText);
    CMaaString txt;
    txt.FormatV2(Format, pText, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
#ifdef TOOLSLIB_CHAR8T_SUPPORT
int __utf8_printf2(const char* Format, const char8_t* pText, ...) noexcept
{
    va_list list;
    va_start(list, pText);
    pText = pText ? pText : u8"(null text2)";
    CMaaString txt, Text((const void*)pText, CMaaString::eROStrlenMemString); // RO?
    txt.FormatV2(Format, Text, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
int __utf8_printf2(CMaaString Format, const char8_t* pText, ...) noexcept
{
    va_list list;
    va_start(list, pText);
    CMaaString txt;
    txt.FormatV2(Format, pText, list);
    va_end(list);
    return __utf8_printf_out(txt);
}
#endif
//---------------------------------------------------------------------------
#define __utf8_printf_UTF8_OUTPUT

//---------------------------------------------------------------------------
//CMaaFile fstderr(CMaaFileStderr, CMaaFile::eW, eNoExcept);
//---------------------------------------------------------------------------
int __utf8_fprintf(CMaaFile file, CMaaString format, ...)
{
    va_list list;
    va_start(list, format);
    CMaaString txt;
    txt.FormatV(format, list);
    va_end(list);
    txt = UnicodeToAnsi(Utf8ToUnicode(txt)).CharToOem(); // ???
    return (int)file.Write(txt);
}
//---------------------------------------------------------------------------
int __utf8_printf_out(CMaaString txt) noexcept
{
#ifdef _WIN32
#ifndef __utf8_printf_UTF8_OUTPUT
    txt = UnicodeToAnsi(Utf8ToUnicode(txt)).CharToOem();
#endif
    if (h__utf8_printf_StdOutHandle == nullptr)
    {
        CMaaFile fstdout(CMaaFileStdout, CMaaFile::eW, false);
        h__utf8_printf_StdOutHandle = fstdout.GetHandle(true);
    }
    if (h__utf8_printf_StdOutHandle)
    {
        DWORD NumberOfBytesWritten = 0;
        WriteFile(h__utf8_printf_StdOutHandle, (const char*)txt, (warning_dword)txt.Length(), &NumberOfBytesWritten, nullptr);
        return (int)NumberOfBytesWritten;
    }
#else
    if (h__utf8_printf_StdOutHandle == -1)
    {
        CMaaFile fstdout(CMaaFileStdout, CMaaFile::eW, false);
        h__utf8_printf_StdOutHandle = fstdout.GetHandle(true);
    }
    if (h__utf8_printf_StdOutHandle != -1)
    {
        _dword NumberOfBytesWritten = write(h__utf8_printf_StdOutHandle, (const char*)txt, txt.Length());
        if (NumberOfBytesWritten == (_dword)-1)
        {
            NumberOfBytesWritten = 0;
        }
        return (int)NumberOfBytesWritten;
    }
#endif
    return 0;
}
//---------------------------------------------------------------------------
int __utf8_printf_flush() noexcept
{
#ifdef _WIN32
    if  (h__utf8_printf_StdOutHandle)
    {
        return FlushFileBuffers(h__utf8_printf_StdOutHandle) ? 1 : 0;
    }
#else
    if  (h__utf8_printf_StdOutHandle != -1)
    {
        return fsync(h__utf8_printf_StdOutHandle) == 0 ? 1 : 0;
    }
#endif
    return 0;
}
//---------------------------------------------------------------------------
int __log(CMaaString format, ...)
{
#define __log_flush //__utf8_printf_flush()
    va_list list;
    va_start(list, format);
    CMaaString txt;
    txt.FormatV(format, list);
    va_end(list);
    static CMaaTime _t;
    _qword t = _t.GetTime();
    txt.Format("%4D.%06D %S", t / 1000000, t % 1000000, &txt);
#ifdef _WIN32
    txt = UnicodeToAnsi(Utf8ToUnicode(txt)).CharToOem();
    if  (h__utf8_printf_StdOutHandle == nullptr)
    {
        CMaaFile fstdout(CMaaFileStdout, CMaaFile::eW, false);
        h__utf8_printf_StdOutHandle = fstdout.GetHandle(true);
    }
    if  (h__utf8_printf_StdOutHandle)
    {
        DWORD NumberOfBytesWritten = 0;
        WriteFile(h__utf8_printf_StdOutHandle, (const char *)txt, (warning_dword)txt.Length(), &NumberOfBytesWritten, nullptr);
        __log_flush;
        return (int)NumberOfBytesWritten;
    }
#else
    if  (h__utf8_printf_StdOutHandle == -1)
    {
        CMaaFile fstdout(CMaaFileStdout, CMaaFile::eW, false);
        h__utf8_printf_StdOutHandle = fstdout.GetHandle(true);
    }
    if  (h__utf8_printf_StdOutHandle != -1)
    {
        _dword NumberOfBytesWritten = write(h__utf8_printf_StdOutHandle, (const char *)txt, txt.Length());
        if  (NumberOfBytesWritten == (_dword)-1)
        {
            NumberOfBytesWritten = 0;
        }
        __log_flush;
        return (int)NumberOfBytesWritten;
    }
#endif
    return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int __lib_utf8_printf(CMaaString txt)
{
    //return __utf8_printf("%S", &txt);
    return __utf8_printf_out(txt);
}
//---------------------------------------------------------------------------
int (*g__lib_printf)(CMaaString txt) = nullptr;
int (*g__lib_printf_flush)() = nullptr;
//---------------------------------------------------------------------------
int __lib_printf(CMaaString format, ...)
{
    if  (g__lib_printf)
    {
        va_list list;
        va_start(list, format);
        CMaaString txt;
        txt.FormatV(format, list);
        va_end(list);
        return (g__lib_printf)(txt);
    }
    return 0;
}
//---------------------------------------------------------------------------
int __lib_printf2(const char *Format, CMaaString Text, ...)
{
    if  (g__lib_printf)
    {
        va_list list;
        va_start(list, Text);
        CMaaString txt;
        txt.FormatV2(Format, Text, list);
        va_end(list);
        return (g__lib_printf)(txt);
    }
    return 0;
}
//---------------------------------------------------------------------------
int __lib_printf_flush()
{
    return g__lib_printf_flush ? g__lib_printf_flush() : 0;
}
//---------------------------------------------------------------------------
#ifdef __unix__
DEF_ALLOCATOR_CMaaUnivHash(ino_t, int)
DEF_ALLOCATOR_CMaaUnivHash(CMaaString, int)
#endif
//-----------------------------------------------------------------------------

/*
void md_truncfile(int fh, U32B len)
{
#ifdef _WIN32
    HANDLE h = (HANDLE)_get_osfhandle(fh);
    CMaaFile f = CMaaFile::FromHandle(h, false, -1);
    const _qword q = f.GetCurPos();
    f.Seek(len);
    f.Truncate();
    f.Seek(q);
    f.Close();
#else
    ftruncate(fh,len);
#endif
}
*/
