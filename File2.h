
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/File.h

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

#ifndef __TOOLS_FILE2_H
#define __TOOLS_FILE2_H

//#define MANUAL_CMaaFileImp_REFS false
#define MANUAL_CMaaFileImp_REFS true
#define ALLOW_NZT_FILENAMES false
//#define ALLOW_NZT_FILENAMES true // unsafe for user code
//#ifdef _WIN32

#ifdef CMAASTRING_HAS_ADD_REF_DATA
#define CMAASTRING_HAS_ADD_REF_DATA_F
#endif


#ifdef getcwd
#undef getcwd
#endif

#ifndef __unix__
#ifndef FILESYSTEM_SLASH
#define FILESYSTEM_SLASH '\\'
#endif
#ifndef szFILESYSTEM_SLASH
#define szFILESYSTEM_SLASH "\\"
#endif
#ifndef OTHER_FILESYSTEM_SLASH
#define OTHER_FILESYSTEM_SLASH '/'
#endif
#ifndef szOTHER_FILESYSTEM_SLASH
#define szOTHER_FILESYSTEM_SLASH "/"
#endif
#else
#ifndef FILESYSTEM_SLASH
#define FILESYSTEM_SLASH '/'
#endif
#ifndef szFILESYSTEM_SLASH
#define szFILESYSTEM_SLASH "/"
#endif
#ifndef OTHER_FILESYSTEM_SLASH
#define OTHER_FILESYSTEM_SLASH '\\'
#endif
#ifndef szOTHER_FILESYSTEM_SLASH
#define szOTHER_FILESYSTEM_SLASH "\\"
#endif
#endif

#define TOOLSLIB_SKIP_LN_PROC 0x0001
extern int gToolsLib_FileFlags;// = TOOLSLIB_SKIP_LN_PROC;

//extern XTOOFile2Error _XTOOFile2_true;

#ifdef _WIN32
#define MYFHANDLE HANDLE
#else
#define MYFHANDLE int
#endif

//---------------------------------------------------------------------------
//#define pszFileName2strFileName(pszFileName) CMaaString(pszFileName)
#define pszFileName2strFileName(pszFileName) CMaaString(pszFileName, CMaaString::eROStrlenMemString)
//---------------------------------------------------------------------------

#ifdef _WIN32
void TimetAndUsecToFileTime(time_t t, int usec, FILETIME *pft) noexcept;
void FileTimeToTimetAndUsec(const FILETIME * pft, time_t * pt, int * pusec) noexcept;
#endif

_qword TimetAndUsecTo_FileTime64(time_t t, int usec) noexcept;
void FileTime64_ToTimetAndUsec(_qword ft, time_t * pt, int * pusec) noexcept;

_qword GetCurrentTime_us(time_t * pt = nullptr, int * pusec = nullptr) noexcept;

//int main(int argn, char * args[]);

//extern const CMaa256Bits gAnySeparatorBits;
//constexpr const CMaa256Bits gAnySeparatorBits{ szFILESYSTEM_SLASH szOTHER_FILESYSTEM_SLASH, 2, false };

//------------------------------------------------------------------------------
// class CMaaFile global defs
//------------------------------------------------------------------------------
#define CMaaFileStdin ((const CMaaString&)CMaaFile::C().sStdin)
#define CMaaFileStdout ((const CMaaString&)CMaaFile::C().sStdout)
#define CMaaFileStderr ((const CMaaString&)CMaaFile::C().sStderr)
#define CMaaFileNul ((const CMaaString&)CMaaFile::C().sNul)
#define CMaaFileNull ((const CMaaString&)CMaaFile::C().sNull)
#define CMaaFileDevNull ((const CMaaString&)CMaaFile::C().sDevNull)
//------------------------------------------------------------------------------
// class CMaaFile
//------------------------------------------------------------------------------
#define CTOOFile2 CMaaFile
class ToolsExport CMaaFile : public XTOOFile2Error
{
    //friend int main(int argn, char * args[]);
protected:
    struct CMaaFileImp :
#if MANUAL_CMaaFileImp_REFS
        public CMaaManualReference
#else
        public CMaaReference
#endif
    {
        //friend int main(int argn, char * args[]);
#ifdef _WIN32
        HANDLE pFile;
        bool m_bStdHandle;
        CMaaFileImp(HANDLE hFile, bool bStdHandle = false) noexcept;
#endif
#ifdef __unix__
        int fd;
        bool m_bStdHandle;
        CMaaFileImp(int d, bool bStdHandle = false) noexcept;
#endif
        //CMaaAutoInitObject<int, 0> m_SrcFormat, m_DstFormat;
        int m_SrcFormat = 0, m_DstFormat = 0;
        void SetClosingType(int ClosingType) noexcept;
        ~CMaaFileImp();

        ADD_ALLOCATOR(CMaaFileImp)
    };

    CMaaFileImp* m_pImp;
    bool m_bIsThrow;

protected:
    static bool CreateDir_Internal(CMaaString& path, int Len, const CMaaString& DirMode = gsCMaaStringZ);
    static bool MkDir_Internal(const CMaaString& dir, bool bIgnoreExist = false, bool bThrow = true);
    static bool RmDir_Internal(const CMaaString& dir, bool bThrow = true);
    static bool Remove_Internal(const CMaaString& FileName, bool bThrow = true);
    static bool IsExists_Internal(const CMaaString& Name) noexcept;
#ifdef _WIN32
    static DWORD GetFileAttributes_Internal(const CMaaString& FileName, bool bThrow = false);
    static bool Attributes_Internal(const CMaaString& FileName, _dword Attr, bool bThrow = true);
#endif
public:

    struct CE
    {
        ceCMaaStringImp snStdin, snStdout, snStderr, snNul, snNull, snDevNull;
        ceCMaaStringImp snPfxUtf8, snUnicode, snUnicodeBigEndian;

        CMaaString::CE::S sStdin, sStdout, sStderr,
            sNul, sNull, sDevNull;
        CMaaString::CE::S sPfx[4];

        constexpr_ CE() noexcept;
    };
    struct CE2 // rarely used objects
    {
        ceCMaaStringImp snft[6 * 5];
        CMaaString::CE::S s_ft[6 * 5]; //s_ft[6][5];

        constexpr CE2() noexcept;
    };
    static const CE& C() noexcept;
    static const CE2& C2() noexcept;

    bool IsThrow() const noexcept
    {
        return m_bIsThrow;
    }
    bool SetThrow(bool bThrow = true) noexcept
    {
        const bool b = m_bIsThrow;
        m_bIsThrow = bThrow;
        return b;
    }

    enum ePrefix
    {
        ePfxRaw = 0,
        ePfxUtf8 = 1,
        ePfxUnicode = 2,
        ePfxUnicodeBigEndian = 3,
        ePfxMax
    };
    static const CMaaString & GetPrefix(int n) noexcept;
    static constexpr int GetMaxPrefixLength() noexcept             { return 3; }
    static const CMaaString & GetUtf8Prefix() noexcept             { return GetPrefix(ePfxUtf8); }
    static const CMaaString & GetUnicodePrefix() noexcept          { return GetPrefix(ePfxUnicode); }
    static const CMaaString & GetUnicodeBigEndianPrefix() noexcept { return GetPrefix(ePfxUnicodeBigEndian); }

    static CMaaString _FileNameToUnicode(CMaaString txt)
    {
#if defined(_WIN32) && defined(_UNICODE)
        return Utf8ToUnicode(txt);
#else
        return AnsiToUnicode(txt);
#endif
    }
    static int _FileNameCompare(CMaaString &a, CMaaString &b, bool bqSortOptimmizationWithStringInjection = false)
    {
        //#if defined(_WIN32) && defined(_UNICODE)
#if defined(_UNICODE)
        return CMaaString::Compare(a, b, 
//#ifdef CMAASTRING_HAS_ADD_DATA
            !bqSortOptimmizationWithStringInjection ? 2 : 3
//#else
//            2
//#endif
        );
#else
        return CMaaString::Compare(a, b);
#endif
    }
    static CMaaString _FileName(const TCHAR * path) {return _utf8_path(path);}
    static CMaaString _FileNameFromAnsi(const char * path) {return _utf8_path_from_ansi(path);}
    static int _FileNameLength(const CMaaString &txt) noexcept
    {
        //#if defined(_WIN32) && defined(_UNICODE)
#if defined(_UNICODE)
        return (warning_int)txt.Utf8CharCount();
#else
        return (warning_int)txt.Length();
#endif
    }

#if defined(_WIN32) && defined(_UNICODE)
#define MkFsCompatible(x) CMaaFile::MkCompatible(x)
#ifdef CMAASTRING_HAS_ADD_REF_DATA_F
#define MkFsNative(x) CMaaFile::MkNative(x) //Utf8ToUnicode(x)
#else
#define MkFsNative(x) Utf8ToUnicode(x)
#endif
#define MkFsNativeCompatible(x) CMaaFile::MkNativeCompatible(x)
#define MkFs_cast(x) (_WC_*)(const char *)(x)
//#define _uc_MkCompatible_str(x) Utf8ToUnicode(MkCompatible(x))
//    //#define _uc_MkCompatible(x) (_WC_*)(const char *)Utf8ToUnicode(CMaaFile::MkCompatible(x))
//#define _uc_MkCompatible(x) Utf8ToUnicode(CMaaFile::MkCompatible(x))
//#define _uc_MkCompatible_cast(x) (_WC_*)(const char *)(x)
#else
#define MkFsCompatible(x) CMaaFile::MkCompatible(x)
#define MkFsNative(x) (x)
#define MkFsNativeCompatible(x) CMaaFile::MkNativeCompatible(x)
#define MkFs_cast(x) (x)
//#define _uc_MkCompatible_str MkCompatible
//#define _uc_MkCompatible CMaaFile::MkCompatible
//#define _uc_MkCompatible_cast(x) (x) // static_cast<const char *>(x)
#endif
//#define _uc_MkCompatible_str(x) MkFsNative(MkFsCompatible(x))
//#define _uc_MkCompatible(x) MkFsNative(MkFsCompatible(x))
//#define _uc_MkCompatible_cast(x) MkFs_cast(x)

    static CMaaString _utf8_path(const TCHAR * path)
    {
        //#if defined(_WIN32) && defined(_UNICODE)
#if defined(_UNICODE) && defined(_WIN32)
        return UnicodeToUtf8((_WC_*)(TCHAR *)path);
#else
        return CMaaString(path, (int)strlen((const char *)path));
#endif
    }

    static CMaaString _utf8_path_from_ansi(const char * path)
    {
        //#if defined(_WIN32) && defined(_UNICODE)
#if defined(_UNICODE)
        return UnicodeToUtf8(AnsiToUnicode(CMaaString(path), false));
#else
        return CMaaString(path, (int)strlen(path));
#endif
    }

    static CMaaString GetFileName(const CMaaString &FullPath) noexcept; // RefMid CMaaString
    static CMaaString GetFolderName(const CMaaString &FullPath) // with ending slash
#if (ALLOW_NZT_FILENAMES)
        noexcept
#endif
        ;
    static CMaaString GetFileNameAny(const CMaaString &FullPath) noexcept; // GetFileName() with any '/' or '\\' separator
    static CMaaString GetFolderNameAny(const CMaaString &FullPath) // GetFolderName() with any '/' or '\\' separator
#if (ALLOW_NZT_FILENAMES)
        noexcept
#endif
        ;
    static CMaaString GetFolderName0(const CMaaString &FullPath); // with ending slash, 0-terminating CMaaString
    static CMaaString GetFolderName2(const CMaaString &FullPath) noexcept; // with ending slash, optimized NZT RefLeft CMaaString
    static CMaaString GetFolderName0Any(const CMaaString &FullPath); // GetFolderName0() with any '/' or '\\' separator
    static CMaaString GetFolderName2Any(const CMaaString &FullPath) noexcept; // GetFolderName2() with any '/' or '\\' separator

    static CMaaString ToUppercase(CMaaString FileName, int iCodePage = 1251 /**/);
    static CMaaString ToLowercase(CMaaString FileName, int iCodePage = 1251 /**/);

    static CMaaString MkCompatible(const CMaaString &Name1);
#ifdef CMaaString_TEST1
    //static CMaaString MkCompatible(const char * pszName) { return MkCompatible(CMaaString(pszName)); }
#endif
#ifdef _WIN32
    static CMaaString MkNative(const CMaaString& Name1) noexcept; // Utf8ToUnicode(x)
#endif
    static CMaaString MkNativeCompatible(const CMaaString& Name1);
    //---------------------------------------------------------------------------
#if 0
    CMaaString MkNativeCompatible(const CMaaString& FileName) // MkFsNativeCompatible(FileName)
    {
        CMaaString fn = MkFsCompatible(FileName);
        const bool bEq = fn.StrictEq(FileName);
        if (bEq)
        {
            fn.Empty();
        }
        return MkFsNative(bEq ? FileName : fn);
    }
#endif
    //---------------------------------------------------------------------------
#ifdef _WIN32
    HANDLE GetHandle(bool bExtract = false) noexcept; // nullptr if closed or not single owner
#else
    int GetHandle(bool bExtract = false) noexcept; // nullptr if closed or not single owner
#endif
    static bool CreateDir(const CMaaString &path, const CMaaString &DirMode = gsCMaaStringZ);
    enum
    {
        eFailIfExist = 0x01,
        eCreateFolder = 0x02
    };
    static bool CopyFile(const char* szSource, const char* szDestination)
    {
        return CopyFile(pszFileName2strFileName(szSource), pszFileName2strFileName(szDestination));
    }
    static bool CopyFile(const char* szSource, const char* szDestination, int Flags, bool bThrow = true)
    {
        return CopyFile(pszFileName2strFileName(szSource), pszFileName2strFileName(szDestination), Flags, bThrow);
    }
    static bool CopyFile(const char* szSource, const char* szDestination, bool fFailIfExists, bool bThrow = true)
    {
        return CopyFile(pszFileName2strFileName(szSource), pszFileName2strFileName(szDestination), fFailIfExists, bThrow);
    }
    static bool MoveFile(const char* szSource, const char* szDestination, bool bThrow = true)
    {
        return MoveFile(pszFileName2strFileName(szSource), pszFileName2strFileName(szDestination), bThrow);
    }
    static bool CopyFile(const CMaaString& Source, const CMaaString& Destination)
    {
        const bool fFailIfExists = false;
        return CopyFile(Source, Destination, fFailIfExists, true);
    }
    static bool CopyFile(const CMaaString& Source, const CMaaString& Destination, bool fFailIfExists, bool bThrow = true)
    {
        return CopyFile(Source, Destination, fFailIfExists ? eFailIfExist : 0, bThrow);
    }
    static bool CopyFile(const CMaaString& Source, const CMaaString& Destination, int Flags, bool bThrow = true);
    static bool MoveFile(const CMaaString& Source, const CMaaString& Destination, bool bThrow = true);
    static bool IsADir(const CMaaString &FileName, bool bLinkInfo = false) noexcept(noexcept_new);
    static bool IsAFile(const CMaaString &FileName, bool bLinkInfo = false) noexcept(noexcept_new);
    static bool Rename(const CMaaString &src, const CMaaString &dst, bool bThrow = true);
    static bool MkDir(const CMaaString &dir, bool bIgnoreExist = false, bool bThrow = true);
    static bool RmDir(const CMaaString &dir, bool bThrow = true);
    static bool RemoveDirRecursively(const CMaaString &path, bool bThrow = false, bool RemoveDirFiles = true);
    static bool CopyDirRecursively(CMaaString SrcPath, CMaaString DstPath, int Deph = -1, bool bThrow = false);
    static time_t GetDateTime(const CMaaString &FileName, int * pusec = nullptr, bool bThrow = true);
    static _qword GetFileTime64(const CMaaString &FileName, bool bThrow = true);
    static bool SetFileTime64(const CMaaString &FileName, _qword ft, bool bThrow = true);
    static bool SetFileTime64Ex(const CMaaString& FileName, _qword TimeNeed, bool bThrow = true);
    static bool SetDateTime(const CMaaString &FileName, time_t t, int usec = 0, bool bThrow = true);
    static bool SetDateTimeEx(const CMaaString &FileName, time_t t, int usec = 0, bool bThrow = true);
    static bool mkdir(const CMaaString &Path, bool bThrow = false);
    static bool chdir(const CMaaString &Path, bool bThrow = false);
    static CMaaString getcwd(bool bThrow = false);
#ifdef __unix__
    bool dup2(int FileNo = STDERR_FILENO); // 0 - STDIN_FILENO, 1 - STDOUT_FILENO, 2 - STDERR_FILENO
    bool SetCloseOnExec(bool bSet = true); // 0 - STDIN_FILENO, 1 - STDOUT_FILENO, 2 - STDERR_FILENO
    static bool StaticSetCloseOnExec(int fd, bool bSet = true, bool bThrow = true);
#else
    bool dup2(int FileNo = 2) {return false;} // 0 - STDIN_FILENO, 1 - STDOUT_FILENO, 2 - STDERR_FILENO
    bool SetCloseOnExec(bool bSet = true) {return true;} // stub
#endif
    static CMaaString GetCanonicalPath(CMaaString Path);
#ifdef _WIN32
    static CMaaString GetSystemDirectory(bool bThrow = false);
    static CMaaString GetWindowsDirectory(bool bThrow = false);
    static CMaaString GetExeFileName(int bFullPath = 1, bool bThrow = false);
    static CMaaString GetExePath(bool bRelease = false, bool bThrow = false); // returns dir name with ending '\\'
    static DWORD GetFileAttributes(const CMaaString &FileName, bool bThrow = false);
    static _dword Attributes(const CMaaString &FileName, bool bThrow = true);
    static _dword Attributes(const char* szFileName, bool bThrow = true);
    static bool Attributes(const CMaaString &FileName, _dword Attr, bool bThrow = true);
    static bool Attributes(const char* szFileName, _dword Attr, bool bThrow = true);
#endif
    static CMaaString GetCurrentDirectory(bool bThrow = false) {return getcwd(bThrow);}
    static bool SetCurrentDirectory(CMaaString Path, bool bThrow = false) {return chdir(Path, bThrow);}

    static bool chroot(const CMaaString &Path, bool bThrow = false);
    static CMaaString AppendPath(CMaaString src, CMaaString append, bool * pError = nullptr);
    static CMaaString GetRelativePath(CMaaString AbsPath, CMaaString RelatedTo, /*out*/ bool *pbReleted = nullptr);
    static CMaaString GetAbsolutePath(CMaaString RelOrAbsPath, CMaaString RelatedTo, /*out*/ bool *pbWasReleted = nullptr);

    //static _qword GetLastWriteTime(const char * szFileName, bool bThrow = true);
    static _qword Length(const CMaaString& szFileName, bool bThrow = true);
    static _qword Length(const char* szFileName, bool bThrow = true);
    static bool Remove(const CMaaString &szFileName, bool bThrow = true);
    static bool RemoveEx(const CMaaString& szFileName, bool bThrow = true);
    static bool RemoveAtReboot(const CMaaString& szFileName, bool bThrow = true);
    static bool RemoveEx(const char * szFileName, bool bThrow = true);
    static bool RemoveAtReboot(const char * szFileName, bool bThrow = true);

    // IsExists - if exists the File
    static bool IsExists(const CMaaString& Name) noexcept;
    static bool IsExists(const char * pszName) noexcept;

    //
    // ClosingType
    // -1 - not closing in any cases (i.e. for stdin/stdout/stderr/etc)
    //  1 - generic (close on destroy, constructor error, exception throwing in constructor), i.e.
    //      handle is fully controlled by passing them to a
    //      FromHandle() and object returned)
    //  0 - handle is not closed on any exception generated to be
    //      controlled by a calling function exception handler,
    //      and is closing generally on object destroy
#ifdef _WIN32
    static CMaaFile FromHandle(HANDLE hFile, bool bThrow = true, int ClosingType = 1);
#else //__unix__
    static CMaaFile FromHandle(int fd, bool bThrow = true, int ClosingType = 1);
#endif

protected:
    struct sfp {};
#ifdef _WIN32
    CMaaFile(HANDLE hFile, bool bThrow, sfp * psfp);
#else //__unix__
    CMaaFile(int fd, bool bThrow, sfp * psfp);
#endif

    void SetClosingType(int ClosingType) noexcept;

public:

    // Mode: "R", "W", "A", "R+"  + "C" - create, + "N" - new,  + "SrSw" - share read/write + "D" - create directory
    // Reserved File Names: "stdin", "stdout", "stderr"
    // To crete file skipping using std handle use "|notstdhandle" in pszMode
    // "+convert_to_utf8" and "+convert_to_ansi" modes are allowed
    enum eMode
    {
        eNoMode = 0,

    //protected: // flags
        eMMR = 0x0001,
        eMMW = 0x0002,
        eMMC = 0x0004,
        eMMN = 0x0008,
        eMMD = 0x0010,
        eMMA = 0x0020,
        eMMp = 0x0040,
        eMMRp = eMMR | eMMp,
        eMMSr = 0x0100,
        eMMSw = 0x0200,
        eMMSd = 0x0400, // Win32: Примечание Доступ к удалению позволяет выполнять операции удаления и переименования.
        eMMI  = 0x0800, // Open "Internal"
        eMMMask = eMMR | eMMW | eMMA, // really open mask
        //
        eMMSrSw = eMMSr | eMMSw,
        eMMSrSd = eMMSr | eMMSd,
        eMMSwSd = eMMSw | eMMSd,
        eMMSrSwSd = eMMSr | eMMSw | eMMSd,

        eMMWC = eMMW | eMMC,
        eMMWCN = eMMW | eMMC | eMMN, // 0
        eMMWCD = eMMW | eMMC | eMMD,
        eMMWCDN = eMMW | eMMC | eMMD | eMMN, // 0
        //eA,
        eMMAC = eMMA | eMMC,
        eMMACD = eMMA | eMMC | eMMD,
        eMMRpC = eMMRp | eMMC,
        eMMRpCN = eMMRp | eMMC | eMMN,
        eMMRpCDN = eMMRp | eMMC | eMMD | eMMN,
        eMMRWCD = eMMR | eMMW | eMMC | eMMD,
        eMMRWC = eMMR | eMMW | eMMC,
        eMMRW = eMMR | eMMW,

    //public: // modes
        
        eR = eMMR,
        eW = eMMW,
        eRp = eMMRp,
        eWC = eMMWC,
        eWCN = eMMWCN, // 0
        eWCD = eMMWCD,
        eWCDN = eMMWCDN, // 0
        eA = eMMA,
        eAC = eMMAC,
        eACD = eMMACD,
        eRpC = eMMRpC,
        eRpCN = eMMRpCN,
        eRpCDN = eMMRpCDN,
        eRWCD = eMMRWCD,
        eRWC = eMMRWC,
        eRW = eMMRW,

        eR_Sd = eR | eMMSd,
        eW_Sd = eW | eMMSd,
        eRp_Sd = eRp | eMMSd,
        eWC_Sd = eWC | eMMSd,
        eWCN_Sd = eWCN | eMMSd,
        eWCD_Sd = eWCD | eMMSd,
        eWCDN_Sd = eWCDN | eMMSd,
        eA_Sd = eA | eMMSd,
        eAC_Sd = eAC | eMMSd,
        eACD_Sd = eACD | eMMSd,
        eRpC_Sd = eRpC | eMMSd,
        eRpCN_Sd = eRpCN | eMMSd,
        eRpCDN_Sd = eRpCDN | eMMSd,
        eRWCD_Sd = eRWCD | eMMSd,
        eRWC_Sd = eRWC | eMMSd,
        eRW_Sd = eRW | eMMSd,

        eR_SrSw = eR | eMMSrSw,
        eW_SrSw = eW | eMMSrSw,
        eRp_SrSw = eRp | eMMSrSw,
        eWC_SrSw = eWC | eMMSrSw,
        eWCN_SrSw = eWCN | eMMSrSw,
        eWCD_SrSw = eWCD | eMMSrSw,
        eWCDN_SrSw = eWCDN | eMMSrSw,
        eA_SrSw = eA | eMMSrSw,
        eAC_SrSw = eAC | eMMSrSw, // == "a+b"
        eACD_SrSw = eACD | eMMSrSw,
        eRpC_SrSw = eRpC | eMMSrSw,
        eRpCN_SrSw = eRpCN | eMMSrSw,
        eRpCDN_SrSw = eRpCDN | eMMSrSw,
        eRWCD_SrSw = eRWCD | eMMSrSw,
        eRWC_SrSw = eRWC | eMMSrSw,
        eRW_SrSw = eRW | eMMSrSw,

        eR_SrSwSd = eR | eMMSrSwSd,
        eW_SrSwSd = eW | eMMSrSwSd,
        eRp_SrSwSd = eRp | eMMSrSwSd,
        eWC_SrSwSd = eWC | eMMSrSwSd,
        eWCN_SrSwSd = eWCN | eMMSrSwSd,
        eWCD_SrSwSd = eWCD | eMMSrSwSd,
        eWCDN_SrSwSd = eWCDN | eMMSrSwSd,
        eA_SrSwSd = eA | eMMSrSwSd,
        eAC_SrSwSd = eAC | eMMSrSwSd, // == "a+b"
        eACD_SrSwSd = eACD | eMMSrSwSd,
        eRpC_SrSwSd = eRpC | eMMSrSwSd,
        eRpCN_SrSwSd = eRpCN | eMMSrSwSd,
        eRpCDN_SrSwSd = eRpCDN | eMMSrSwSd,
        eRWCD_SrSwSd = eRWCD | eMMSrSwSd,
        eRWC_SrSwSd = eRWC | eMMSrSwSd,
        eRW_SrSwSd = eRW | eMMSrSwSd,

        eR_Sr = eR | eMMSr,
        eW_Sr = eW | eMMSr, // 0
        eRp_Sr = eRp | eMMSr,
        eWC_Sr = eWC | eMMSr,
        eWCN_Sr = eWCN | eMMSr, // 0
        eWCD_Sr = eWCD | eMMSr,
        eWCDN_Sr = eWCDN | eMMSr, // 0
        eA_Sr = eA | eMMSr,
        eAC_Sr = eAC | eMMSr,
        eACD_Sr = eACD | eMMSr,
        eRpC_Sr = eRpC | eMMSr, // 0
        eRpCN_Sr = eRpCN | eMMSr,
        eRpCDN_Sr = eRpCDN | eMMSr,
        eRWCD_Sr = eRWCD | eMMSr,
        eRWC_Sr = eRWC | eMMSr,
        eRW_Sr = eRW | eMMSr,

        eR_SrSd = eR | eMMSrSd,
        eW_SrSd = eW | eMMSrSd,
        eRp_SrSd = eRp | eMMSrSd,
        eWC_SrSd = eWC | eMMSrSd,
        eWCN_SrSd = eWCN | eMMSrSd,
        eWCD_SrSd = eWCD | eMMSrSd,
        eWCDN_SrSd = eWCDN | eMMSrSd,
        eA_SrSd = eA | eMMSrSd,
        eAC_SrSd = eAC | eMMSrSd,
        eACD_SrSd = eACD | eMMSrSd,
        eRpC_SrSd = eRpC | eMMSrSd,
        eRpCN_SrSd = eRpCN | eMMSrSd,
        eRpCDN_SrSd = eRpCDN | eMMSrSd,
        eRWCD_SrSd = eRWCD | eMMSrSd,
        eRWC_SrSd = eRWC | eMMSrSd,
        eRW_SrSd = eRW | eMMSrSd,

        eR_Sw = eR | eMMSw,
        eW_Sw = eW | eMMSw,
        eRp_Sw = eRp | eMMSw,
        eWC_Sw = eWC | eMMSw,
        eWCN_Sw = eWCN | eMMSw,
        eWCD_Sw = eWCD | eMMSw,
        eWCDN_Sw = eWCDN | eMMSw,
        eA_Sw = eA | eMMSw,
        eAC_Sw = eAC | eMMSw,
        eACD_Sw = eACD | eMMSw,
        eRpC_Sw = eRpC | eMMSw,
        eRpCN_Sw = eRpCN | eMMSw,
        eRpCDN_Sw = eRpCDN | eMMSw,
        eRWCD_Sw = eRWCD | eMMSw,
        eRWC_Sw = eRWC | eMMSw,
        eRW_Sw = eRW | eMMSw,

        eR_SwSd = eR | eMMSwSd,
        eW_SwSd = eW | eMMSwSd,
        eRp_SwSd = eRp | eMMSwSd,
        eWC_SwSd = eWC | eMMSwSd,
        eWCN_SwSd = eWCN | eMMSwSd,
        eWCD_SwSd = eWCD | eMMSwSd,
        eWCDN_SwSd = eWCDN | eMMSwSd,
        eA_SwSd = eA | eMMSwSd,
        eAC_SwSd = eAC | eMMSwSd,
        eACD_SwSd = eACD | eMMSwSd,
        eRpC_SwSd = eRpC | eMMSwSd,
        eRpCN_SwSd = eRpCN | eMMSwSd,
        eRpCDN_SwSd = eRpCDN | eMMSwSd,
        eRWCD_SwSd = eRWCD | eMMSwSd,
        eRWC_SwSd = eRWC | eMMSwSd,
        eRW_SwSd = eRW | eMMSwSd,

        // +I ------------------------
        eRI = eR | eMMI,
        eWI = eW | eMMI,
        eRpI = eRp | eMMI,
        eWCI = eWC | eMMI,
        eWCNI = eWCN | eMMI,
        eWCDI = eWCD | eMMI,
        eWCDNI = eWCDN | eMMI,
        eAI = eA | eMMI,
        eACI = eAC | eMMI,
        eACDI = eACD | eMMI,
        eRpCI = eRpC | eMMI,
        eRpCNI = eRpCN | eMMI,
        eRpCDNI = eRpCDN | eMMI,
        eRWCDI = eRWCD | eMMI,
        eRWCI = eRWC | eMMI,
        eRWI = eRW | eMMI,

        eRI_Sd = eR_Sd | eMMI,
        eWI_Sd = eW_Sd | eMMI,
        eRpI_Sd = eRp_Sd | eMMI,
        eWCI_Sd = eWC_Sd | eMMI,
        eWCNI_Sd = eWCN_Sd | eMMI,
        eWCDI_Sd = eWCD_Sd | eMMI,
        eWCDNI_Sd = eWCDN_Sd | eMMI,
        eAI_Sd = eA_Sd | eMMI,
        eACI_Sd = eAC_Sd | eMMI,
        eACDI_Sd = eACD_Sd | eMMI,
        eRpCI_Sd = eRpC_Sd | eMMI,
        eRpCNI_Sd = eRpCN_Sd | eMMI,
        eRpCDNI_Sd = eRpCDN_Sd | eMMI,
        eRWCDI_Sd = eRWCD_Sd | eMMI,
        eRWCI_Sd = eRWC_Sd | eMMI,
        eRWI_Sd = eRW_Sd | eMMI,

        eRI_SrSw = eR_SrSw | eMMI,
        eWI_SrSw = eW_SrSw | eMMI,
        eRpI_SrSw = eRp_SrSw | eMMI,
        eWCI_SrSw = eWC_SrSw | eMMI,
        eWCNI_SrSw = eWCN_SrSw | eMMI,
        eWCDI_SrSw = eWCD_SrSw | eMMI,
        eWCDNI_SrSw = eWCDN_SrSw | eMMI,
        eAI_SrSw = eA_SrSw | eMMI,
        eACI_SrSw = eAC_SrSw | eMMI,
        eACDI_SrSw = eACD_SrSw | eMMI,
        eRpCI_SrSw = eRpC_SrSw | eMMI,
        eRpCNI_SrSw = eRpCN_SrSw | eMMI,
        eRpCDNI_SrSw = eRpCDN_SrSw | eMMI,
        eRWCDI_SrSw = eRWCD_SrSw | eMMI,
        eRWCI_SrSw = eRWC_SrSw | eMMI,
        eRWI_SrSw = eRW_SrSw | eMMI,

        eRI_SrSwSd = eR_SrSwSd | eMMI,
        eWI_SrSwSd = eW_SrSwSd | eMMI,
        eRpI_SrSwSd = eRp_SrSwSd | eMMI,
        eWCI_SrSwSd = eWC_SrSwSd | eMMI,
        eWCNI_SrSwSd = eWCN_SrSwSd | eMMI,
        eWCDI_SrSwSd = eWCD_SrSwSd | eMMI,
        eWCDNI_SrSwSd = eWCDN_SrSwSd | eMMI,
        eAI_SrSwSd = eA_SrSwSd | eMMI,
        eACI_SrSwSd = eAC_SrSwSd | eMMI,
        eACDI_SrSwSd = eACD_SrSwSd | eMMI,
        eRpCI_SrSwSd = eRpC_SrSwSd | eMMI,
        eRpCNI_SrSwSd = eRpCN_SrSwSd | eMMI,
        eRpCDNI_SrSwSd = eRpCDN_SrSwSd | eMMI,
        eRWCDI_SrSwSd = eRWCD_SrSwSd | eMMI,
        eRWCI_SrSwSd = eRWC_SrSwSd | eMMI,
        eRWI_SrSwSd = eRW_SrSwSd | eMMI,

        eRI_Sr = eR_Sr | eMMI,
        eWI_Sr = eW_Sr | eMMI,
        eRpI_Sr = eRp_Sr | eMMI,
        eWCI_Sr = eWC_Sr | eMMI,
        eWCNI_Sr = eWCN_Sr | eMMI,
        eWCDI_Sr = eWCD_Sr | eMMI,
        eWCDNI_Sr = eWCDN_Sr | eMMI,
        eAI_Sr = eA_Sr | eMMI,
        eACI_Sr = eAC_Sr | eMMI,
        eACDI_Sr = eACD_Sr | eMMI,
        eRpCI_Sr = eRpC_Sr | eMMI,
        eRpCNI_Sr = eRpCN_Sr | eMMI,
        eRpCDNI_Sr = eRpCDN_Sr | eMMI,
        eRWCDI_Sr = eRWCD_Sr | eMMI,
        eRWCI_Sr = eRWC_Sr | eMMI,
        eRWI_Sr = eRW_Sr | eMMI,

        eRI_SrSd = eR_SrSd | eMMI,
        eWI_SrSd = eW_SrSd | eMMI,
        eRpI_SrSd = eRp_SrSd | eMMI,
        eWCI_SrSd = eWC_SrSd | eMMI,
        eWCNI_SrSd = eWCN_SrSd | eMMI,
        eWCDI_SrSd = eWCD_SrSd | eMMI,
        eWCDNI_SrSd = eWCDN_SrSd | eMMI,
        eAI_SrSd = eA_SrSd | eMMI,
        eACI_SrSd = eAC_SrSd | eMMI,
        eACDI_SrSd = eACD_SrSd | eMMI,
        eRpCI_SrSd = eRpC_SrSd | eMMI,
        eRpCNI_SrSd = eRpCN_SrSd | eMMI,
        eRpCDNI_SrSd = eRpCDN_SrSd | eMMI,
        eRWCDI_SrSd = eRWCD_SrSd | eMMI,
        eRWCI_SrSd = eRWC_SrSd | eMMI,
        eRWI_SrSd = eRW_SrSd | eMMI,

        eRI_Sw = eR_Sw | eMMI,
        eWI_Sw = eW_Sw | eMMI,
        eRpI_Sw = eRp_Sw | eMMI,
        eWCI_Sw = eWC_Sw | eMMI,
        eWCNI_Sw = eWCN_Sw | eMMI,
        eWCDI_Sw = eWCD_Sw | eMMI,
        eWCDNI_Sw = eWCDN_Sw | eMMI,
        eAI_Sw = eA_Sw | eMMI,
        eACI_Sw = eAC_Sw | eMMI,
        eACDI_Sw = eACD_Sw | eMMI,
        eRpCI_Sw = eRpC_Sw | eMMI,
        eRpCNI_Sw = eRpCN_Sw | eMMI,
        eRpCDNI_Sw = eRpCDN_Sw | eMMI,
        eRWCDI_Sw = eRWCD_Sw | eMMI,
        eRWCI_Sw = eRWC_Sw | eMMI,
        eRWI_Sw = eRW_Sw | eMMI,

        eRI_SwSd = eR_SwSd | eMMI,
        eWI_SwSd = eW_SwSd | eMMI,
        eRpI_SwSd = eRp_SwSd | eMMI,
        eWCI_SwSd = eWC_SwSd | eMMI,
        eWCNI_SwSd = eWCN_SwSd | eMMI,
        eWCDI_SwSd = eWCD_SwSd | eMMI,
        eWCDNI_SwSd = eWCDN_SwSd | eMMI,
        eAI_SwSd = eA_SwSd | eMMI,
        eACI_SwSd = eAC_SwSd | eMMI,
        eACDI_SwSd = eACD_SwSd | eMMI,
        eRpCI_SwSd = eRpC_SwSd | eMMI,
        eRpCNI_SwSd = eRpCN_SwSd | eMMI,
        eRpCDNI_SwSd = eRpCDN_SwSd | eMMI,
        eRWCDI_SwSd = eRWCD_SwSd | eMMI,
        eRWCI_SwSd = eRWC_SwSd | eMMI,
        eRWI_SwSd = eRW_SwSd | eMMI
        //----------------------------

        //eWN_SrSw,
        //eWN_Sw,
        //eWN,

        //eModeMax
    };
#ifdef __unix__
    enum eUnixStartProcessFlags
    {
        eCloseFds    = 0x10, // close fd= [3-...] 
        eCloseAllFds = 0x20
    };
#endif
    static CMaaString GetModeByModeMask(int mm);
    static CMaaString GetMode(int Mode) noexcept;
    static int GetModeMask(const CMaaString &Mode) noexcept;
    //
    CMaaFile() noexcept; // file is closed, XTOOFile2Error(eXTOONoError), m_bIsThrow = false
    //CMaaFile(const char* pszName, const char * pszMode = "R+C|SrSw", bool fMakeExeption = true); // closed on (!pszName || !pszMode)
    CMaaFile(const CMaaString& Name, const char* pszMode /*= "R+C|SrSw"*/, eNoExcept_) noexcept; // closed on (Name.IsEmpty() || !pszMode)
    CMaaFile(const CMaaString& Name, const char* pszMode /*= "R+C|SrSw"*/, bool fMakeExeption = true); // closed on (Name.IsEmpty() || !pszMode)
    CMaaFile(const CMaaString& Name, const CMaaString& Mode, eNoExcept_) noexcept; // closed on (Name.IsEmpty() || Mode.IsEmpty())
    CMaaFile(const CMaaString& Name, const CMaaString& Mode, bool fMakeExeption = true); // closed on (Name.IsEmpty() || Mode.IsEmpty())
    //
    //CMaaFile(const char* pszName, eMode Mode, bool fMakeExeption = true); // closed on (!pszName || Mode == eNoMode)
    CMaaFile(const CMaaString& Name, eMode Mode, eNoExcept_) noexcept; // closed on (Name.IsEmpty() || Mode == eNoMode)
    CMaaFile(const CMaaString& Name, eMode Mode = eRpC_SrSw, bool fMakeExeption = true); // closed on (Name.IsEmpty() || Mode == eNoMode)
    //
    //CMaaFile(const char* pszName, eMode fMode, const char* pszMode, bool fMakeExeption = true); // for Open(...)
    //CMaaFile(const CMaaString &Name, eMode fMode, const char* pszMode, bool fMakeExeption = true); // for Open(...)

    CMaaFile(const CMaaString& Name, eMode fMode, const CMaaString& strMode, eNoExcept_) noexcept; // for Open(...)
    CMaaFile(const CMaaString& Name, eMode fMode, const CMaaString& strMode, bool fMakeExeption = true); // for Open(...)
    //

    bool Open(const CMaaString& Name, int fMode = eNoMode, const CMaaString& Mode = gsCMaaStringZ);

public:
    CMaaFile(const CMaaFile &That) noexcept;
    CMaaFile(CMaaFile &&That) noexcept;
    CMaaFile & operator= (const CMaaFile & That) noexcept;
    CMaaFile & operator= (CMaaFile &&That) noexcept;

    // True if file was opened
    bool IsOpen() const noexcept;
    bool IsOpened() const noexcept
    {
        return IsOpen();
    }

    // returns bytes actually read
    _dword Read(void * Buffer, _dword Size);
    _dword ReadExact(void * Buffer, _dword Size)
    {
        const _dword x = Read(Buffer, Size);
        if  (x != Size)
        {
            CMaaString msg;
            msg.Format("File read exact data length error, %,D of %,D bytes of data is read in the last read call", (_qword)x, (_qword)Size);
            SetError(msg, 0);
        }
        return x;
    }

    // returns bytes actually written
    _dword Write(const void * Buffer, _dword Size);

    CMaaString ReadAll();
    CMaaString ReadAllSysFile() noexcept;
    CMaaString Read(_dword MaxLen = 30 * 1024 * 1024, int _Convert = 0 /*some unicode compatibility flag can be added later*/);
    _dword Write(const CMaaString &Txt);

    /*
     void SetError(int Reason, int Error = -1)
     {
          XTOOFile2Error err(Error);
          (XTOOFile2Error &)*this = err;
          if   (IsThrow())
          {
               throw err;
          }
     }
     */
    void SetError(const char * Msg = nullptr, int Error = -1)
    {
        XTOOFile2Error err(Msg, Error);
        (XTOOFile2Error &)*this = err;
        if  (IsThrow())
        {
            throw err;
        }
    }

    bool SetError(int Error /* = -1*/ , const char * Format, ...);
    bool SetError(int Error /* = -1*/ , CMaaString Format, ...);

    static void StaticError(const char * Msg = nullptr, int Error = -1)
    {
        XTOOFile2Error err(Msg, Error);
        //(XTOOFile2Error &)*this = err;
        //if   (IsThrow())
        {
            throw err;
        }
    }
    static bool StaticError(int Error /* = -1*/, bool bThrow, const char * Format, ...);
    static bool StaticError(int Error /* = -1*/, bool bThrow, CMaaString Format, ...);
    /*
    void Clear()
    {
        const char * pmsg;
        if  (this->GetErrorCode() || ((pmsg = GetMsg()) && *pmsg != '\0'))
        {
            //XTOOFile2Error tmp(nullptr, 0); // ok, v1
            XTOOFile2Error tmp(eXTOONoError);     // ok, v2
            (XTOOFile2Error &)*this = tmp;
        }
    }
    */

    _dword WriteVerify(const void * Buffer, _dword Size)
    {
        const _dword x = Write(Buffer, Size);
        if  (x != Size)
        {
            //SetError(e_FILE_READ_ERROR);
            CMaaString msg;
            msg.Format("File write error, %,D of %,D bytes of data is written in the last write call", (_qword)x, (_qword)Size);
            //SetError("File read error");
            SetError(msg, 0);
        }
        return x;
    }

    _dword WriteVerify(const CMaaString &Txt)
    {
        return WriteVerify(Txt, (warning_dword)Txt.Length());
    }

    /*
     _dword Write(int Digit)
     {
          return Write(&Digit, sizeof(int));
     }

     CMaaFile & operator<< (long x)
     {
          Write(&x, sizeof(x));
          return *this;
     }

     CMaaFile & operator>> (long &x)
     {
          _dword pos = CurPos();
          if   ( Read(&x, sizeof(x)) == sizeof(x))
          {
               return *this;
          }
          //SetErrMessEx( pszName );
          SetError(e_FILE_READ_ERROR, errno);
          x = 0;
          return *this;
     }

     CMaaFile & operator<< (unsigned long x)
     {
          Write(&x, sizeof(x));
          return *this;
     }

     CMaaFile & operator>> (unsigned long &x)
     {
          //_dword pos = CurPos ();
          if   (Read(&x, sizeof(x)) == sizeof(x))
          {
               return *this;
          }
          //SetErrMessEx( pszName );
          SetError(e_FILE_READ_ERROR, errno);
          x = 0;
          return *this;
     }

     CMaaFile & operator<< (int x)
     {
          Write(&x, sizeof(x));
          return *this;
     }

     CMaaFile & operator>> (int &x)
     {
          //_dword pos = CurPos ();
          if   (Read(&x, sizeof(x)) == sizeof(x))
          {
               return *this;
          }
          //SetErrMessEx( pszName );
          SetError(e_FILE_READ_ERROR, errno);
          x = 0;
          return *this;
     }

     CMaaFile & operator<< (unsigned int x)
     {
          Write(&x, sizeof(x));
          return *this;
     }

     CMaaFile & operator>> (unsigned int &x)
     {
          //_dword pos = CurPos();
          if   (Read(&x, sizeof(x)) == sizeof(x))
          {
               return *this;
          }
          //SetErrMessEx( pszName );
          SetError(e_FILE_READ_ERROR, errno);
          x = 0;
          return *this;
     }

     operator FILE * ()
     {
          if   ( m_pImp )
          {
               return m_pImp->pFile;
          }
          XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION );
          return nullptr;
     }

#ifndef __BORLANDC__
     CMaaFile & operator << ( __int64 x )
     {
          Write ( &x, sizeof ( x ) );
          return *this;
     }
     CMaaFile & operator << ( __uint64 x )
     {
          return operator << ( ( __int64 ) x );
     }

     CMaaFile & operator >> ( __int64 &x )
     {
          _dword pos = CurPos ();
          if   ( Read ( &x, sizeof ( x ) ) == sizeof ( x ) )
          {
               return *this;
          }
          //SetErrMessEx( pszName );
          SetError( e_FILE_READ_ERROR, errno );
          x = 0;
          return *this;
     }
     CMaaFile & operator >> ( __uint64 &x )
     {
          return operator >> ( ( __int64 & ) x );
     }
     #endif // __BORLANDC__

     CMaaFile & operator << ( const char * txt )
     {
          Write ( txt, ( _dword )( strlen ( txt ) + 1 ) );
          return *this;
     }

     CMaaFile & operator >> ( CMaaString &s )
     {
          _dword pos = CurPos ();
          char txt [ 1025 ];
          int Bytes = Read ( txt, sizeof ( txt ) - 1 );
          if   ( Bytes == 0 )
          {
               //SetErrMessEx( pszName );
               SetError( e_FILE_READ_ERROR, errno );
          }
          txt [ Bytes ] = 0;
          int len = ( int ) strlen ( txt );
          s = txt;
          if   ( Bytes )
          {
               Seek ( pos + ( _dword )( len + 1 ) );
          }
          return *this;
     }
*/

    // gets current position
    _qword CurPos();
    _qword GetCurPos() {return CurPos();}

    // sets end of file
    bool Truncate();

    // move file ptr FromWhere = { SEEK_SET, SEEK_END, SEEK_CUR }
    bool Seek(_qword Offset, int FromWhere = SEEK_SET);

    // returns _true if end of file
    bool IsEOF();

    // close handle if need ( no other references present )
    void Close() noexcept;

    void Flush(int Flags = 0); // 0 - fdatasync (data only, faster), 1 - fsync (data and metadata, slower)

    // get file length
    _qword Length();

    //CMaaDateTime LastWriteTime ();
    //_bool LastWriteTime ( const CMaaDateTime & DateTime );

    char * fgets(char * pszBuffer, int Size, bool bRemoveCutCrLf = true, int * pStrLen = nullptr, int Flags = 0/*0x100 - ignore zero chars as a string terminator*/);
    CMaaString fgets(int Size = TOOLSLIB_FGETS_64K, bool bRemoveCutCrLf = true, int Flags = 1); // (&0xff) == 0xff - throw, 0 - nothing to do, 1 - close file, | 0x100 - ignore zero chars as a string terminator
    char * rfgets(char * pszBuffer, int Size, bool bRemoveCutCrLf = true, int * pStrLen = nullptr, int Flags = 0/*0x100 - ignore zero chars as a string terminator*/);
    CMaaString rfgets(int Size = TOOLSLIB_FGETS_64K, bool bRemoveCutCrLf = true, int Flags = 1); // (&0xff) == 0xff - throw, 0 - nothing to do, 1 - close file, | 0x100 - ignore zero chars as a string terminator

    // analog of close()
    virtual ~CMaaFile();

    _dword fprintf(const char* format, ...);
    _dword fprintf2(const char* Format, CMaaString Text, ...);
#ifdef TOOLSLIB_CHAR8T_SUPPORT
    _dword fprintf(const char8_t* format, ...);
    _dword fprintf2(const char8_t* Format, CMaaString Text, ...);
#endif

    bool LockFile(DWORD TimeOut);

protected:
    static bool _RemoveDirRecursively(const CMaaString &path, bool bThrow, bool RemoveDirFiles); // if !RemoveDirFiles then remove only skel empty folders
};
//------------------------------------------------------------------------------
//extern CMaaFile f_glog;
CMaaFile * ToolsLib_OpenLog(bool bForcedInit = false);
#define f_glog (*ToolsLib_OpenLog(true))
//------------------------------------------------------------------------------

class CMaaFindFile2
{
protected:
    CMaaString m_Dir, m_Mask;
    int m_iRecursiveDepth;
#ifdef _WIN32
#ifdef _UNICODE
    struct _wfinddata64_t m_ff;
#else
    struct __finddata64_t m_ff;
#endif
    intptr_t m_h;
    bool m_b1st;
    CMaaFindFile2 * m_pCurr;
#endif

#ifdef __unix__
    FTS * m_fts;
    FTSENT * m_entry;
    dev_t m_dev;
    int m_iGetKeepDev;
    CMaaUnivHash<ino_t, int> m_hDirInodes;
#endif

public:
    struct sFind : public CMaaDLink // just for user lists
    {
        enum
        {
            eDot    = 0x01,
            eDotDot = 0x02,
            eDir    = 0x03,
            eFile   = 0x04,
            eSl     = 0x05,
            eUnknown = 0xff
        };
        const char* GetTypeName(int Align = -2) const noexcept; // -1 - left, -2 - file/dir type len based, 0 - name with out of spaces, 1 - right, 2 - file/dir type len based
        const CMaaString & GetTypeNameStr(int Align = -2) const noexcept; // -1 - left, -2 - file/dir type len based, 0 - name with out of spaces, 1 - right, 2 - file/dir type len based
        static int GetMaxTypeNameLength(int FileOrDirOnly = 1) noexcept;
        int m_Type;
        CMaaString m_FileName;
        _qword m_Size;
        time_t m_mTime;
#ifdef _WIN32
#ifdef _UNICODE
        struct _wfinddata64_t
#else
        struct __finddata64_t
#endif
        * m_px;
#endif
#ifdef __unix__
        FTSENT * m_px;
#endif
        _qword m_mft;

        CMaaString GetFileName() const noexcept; // RefMid CMaaString

        CMaaString GetDirName() const // with out if ending slash
#if (ALLOW_NZT_FILENAMES)
            noexcept
#endif
            ;
        CMaaString GetDirName0() const noexcept(noexcept_new); // with out if ending slash, 0-terminating CMaaString
        CMaaString GetDirName2() const noexcept; // with out if ending slash, optimized NZT RefLeft CMaaString
    };
    CMaaFindFile2(CMaaString Dir, CMaaString Mask, int iRecursiveDepth = 1) noexcept(noexcept_new);
    CMaaFindFile2(CMaaString DirWithMask, int iRecursiveDepth = 1) noexcept(noexcept_new);
    ~CMaaFindFile2();
    bool IsDirOpened() const noexcept; // optional. is dir opened
    bool Get(sFind &f) noexcept(noexcept_new);
    enum eFlags
    {
        eFt = 0x01,
        eForcedFt = 0x02,
        eDoNotFollowLinks = 0x04,
        eGetKeepDev = 0x08
    };
    enum
    {
        eDotMask = (1 << CMaaFindFile2::sFind::eDot),
        eDotDotMask = (1 << CMaaFindFile2::sFind::eDotDot),
        eDots = eDotMask|eDotDotMask,
        eDirMask = (1 << CMaaFindFile2::sFind::eDir),
        eFileMask = (1 << CMaaFindFile2::sFind::eFile),
        eSl = (1 << CMaaFindFile2::sFind::eSl),
        eSpecialMask = (1 << 31)
    };
    int SetOptFlags(int Flags = eFt|eForcedFt) noexcept; // |eGetKeepDev
    int SetFileTypeMasks(int Masks = eDirMask|eFileMask) noexcept;
protected:
    //CMaaAutoInitObject<int, eFt> m_Flags;
    //CMaaAutoInitObject<int, -1> m_FileTypeMask;
    int m_Flags = eFt;
    int m_FileTypeMask = -1;
    bool InternalGet(sFind &f) noexcept(noexcept_new);
};

#ifdef __unix__
// 0 - ok
; // pargs[0] is reserved for exec file name ("/usr/sbin/sendmail")
int SendEmail(CMaaString txt, const char ** pargs, bool bThrow = true, bool * pRet = nullptr);
#endif

#ifndef TOOLSLIB_SINGLE_THREAD
#ifdef __unix__
bool unix_StartProcess(const char * ExecFileName, const char ** args = nullptr, const char ** envs = nullptr, bool bThrow = true, int Flags = 0 /*C_os_StartProcess::eCloseFds, C_os_StartProcess::eCloseAllFds*/, bool bEJudge = false, int MemLimit = -1, int TimeLimitMs = 30000);

class C_os_StartProcess
{
    int m_Flags;
    int m_Errno;

    pid_t m_ChildProcessId;
    int m_Status, m_StatusEx, m_Options;
    rusage m_rusage;
    int m_ExitCode;
    timeval m_rtv, m_tv0, m_tv1;

    class Writer : public CMaaThread
    {
        CMaaString m_Data;
        CMaaFile m_File;
    public:
        Writer(CMaaString Data, int fd);
        ~Writer();
        unsigned Run();

        ADD_ALLOCATOR(Writer)
    };
    class Reader : public CMaaThread
    {
        CMaaString &m_Data;
        CMaaFile m_File;
        CMaaConcatString1 m_strData;
        CMaaPtr_<char, 1> m_Buffer;
    public:
        Reader(CMaaString &Data, int fd);
        ~Reader();
        unsigned Run();

        ADD_ALLOCATOR(Reader)
    };

    CMaaThread * m_pRW[3];
public:
    //CMaaAutoInitObject<int, -2> m_fdStdIn, m_fdStdOut, m_fdStdErr;
    int m_fdStdIn = -2, m_fdStdOut = -2, m_fdStdErr = -2;
    CMaaString m_StdInSend, m_StdOutRecv, m_StdErrRecv; // for handles with a number of -3

    bool m_bEJudge;
    int m_MemLimit;
    int m_TimeLimitMs;

    pid_t GetChildProcessId()
    {
        return m_ChildProcessId;
    }
    pid_t GetChild_pid()
    {
        return m_ChildProcessId;
    }
    int GetExitCode()
    {
        return m_ExitCode;
    }
    int GetStatus()
    {
        return m_Status;
    }
    int GetStatusEx()
    {
        return m_StatusEx;
        // with ejudge bits
        //
        /*
    if ((status & 0x10000))
    {
        printf("Memory Limit Exceeded\n");
    }
    if ((status & 0x20000))
    {
        printf("Security Violation\n");
    }
    if ((status & 0x40000))
    {
        printf("Time-limit exceeded\n");
    }
*/
    }
    void GetUsage(rusage &rusage)
    {
        memcpy(&rusage, &m_rusage, sizeof(rusage));
    }
    void GetUsage(rusage &rusage, timeval &tv)
    {
        GetUsage(rusage);
        GetRealTime(tv);
    }
    void GetRealTime(timeval &tv)
    {
        memcpy(&tv, &m_rtv, sizeof(m_rtv));
    }
    struct Pipe
    {
        int fd[2];

        int m_sigignorecount;
        sighandler_t m_sig_pipe;
        sighandler_t m_sig_bus;

        Pipe(bool bCreate = true);
        ~Pipe();
        bool Create();
        void Close();
        void CloseAt_vfork(); // preserve data in child
        bool SetHandleIsInherited(int Num, bool bSet = true, bool bThrow = false);
        bool ItIsASide(int SideNum);
        int GetReadHandle(bool bExtract = false)
        {
            return GetHandle(0, bExtract);
        }
        int GetWriteHandle(bool bExtract = false)
        {
            return GetHandle(1, bExtract);
        }
        int GetHandle(int Num, bool bExtract = false);
        void IgnoreSignals();
        void RestoreSignals();
    };

    Pipe m_p;
    Pipe _i, _o, _e;
    Pipe * _ioe[3];// = {&_i, &_o, &_e};
    bool m_bFdPipe[3];

    C_os_StartProcess(bool bEJudge = false, int MemLimit = -1 /*(((1 << 30) - 1) << 1) + 1*/, int TimeLimitMs = 30000)
    :   m_p(true), _i(false), _o(false), _e(false)
    {
        m_pRW[0] = m_pRW[1] = m_pRW[2] = nullptr;
        m_Flags = 0;
        m_ChildProcessId = -1;
        m_Status = 0;
        m_StatusEx = 0;
        m_Options = 0;
        m_Errno = 0;
        memset(&m_rusage, 0, sizeof(m_rusage));
        memset(&m_rtv, 0, sizeof(m_rtv));
        memset(&m_tv0, 0, sizeof(m_tv0));
        memset(&m_tv1, 0, sizeof(m_tv1));
        m_ExitCode = -1;
        m_bEJudge = bEJudge;
        m_MemLimit = MemLimit;
        m_TimeLimitMs = TimeLimitMs;
        //m_fdStdIn = m_fdStdOut = m_fdStdErr = -2; // -2 - inherit, -1 - closed
        m_p.IgnoreSignals();
    }
    ~C_os_StartProcess()
    {
        for (int i = 0; i < 3; i++)
        {
            if  (m_pRW[i])
            {
                delete m_pRW[i];
            }
        }
        m_p.RestoreSignals();
    }

    enum eFlags
    {
        eWaitChild   = 0x01,
        eGetUsage    = 0x02,
        eProtectExecFileIsNotWritable = 0x04,
        eProtect     = eProtectExecFileIsNotWritable,
        eSearchPath  = 0x08,
        eCloseFds    = CMaaFile::eCloseFds,   // 0x10, // close fd= [3-...] 
        eCloseAllFds = CMaaFile::eCloseAllFds // 0x20
    };
    bool fork_and_StartProcess_old(int Flags, const char * ExecFileName, const char ** args = nullptr, const char ** envs = nullptr, bool bThrow = true); // before 06.01.2019
    bool fork_and_StartProcess(int Flags, const char * ExecFileName, const char ** args = nullptr, const char ** envs = nullptr, bool bThrow = true);
};
bool unix_fork_and_StartProcess(int Flags /*= C_os_StartProcess::eWaitChild|C_os_StartProcess::eSearchPath*/, int * pExitCode, const char * ExecFileName, const char ** args = nullptr, const char ** envs = nullptr, bool bThrow = true, int fdStdIn = -2, int fdStdOut = -2, int fdStdErr = -2);
#endif
#endif



//#endif // _WIN32


#if 0
class CFileTimedLock
{
    int m_fd;
    int m_err;
public:
    CFileTimedLock(const char * LockFileName = "lock.txt", int Seconds = 60);
    ~CFileTimedLock();
    bool IsLocked();
};
#endif

class CFileTimedLock
{
    CMaaFile m_file;
protected:
    void ctor_(int Seconds, bool bThrow);
public:
    CFileTimedLock(CMaaString LockFileName = "lock.txt", int Seconds = 60, bool bThrow = false);
    CFileTimedLock(CMaaString LockFileName, int Seconds, eNoExcept_);
    ~CFileTimedLock();
    bool IsLocked() noexcept;
    void Close() noexcept;
};

int __utf8_printf(const char* format, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
int __utf8_printf(const char8_t* format, ...) noexcept;
#endif
int __utf8_printf(CMaaString format, ...) noexcept;
int __log(CMaaString format, ...);
int __utf8_printf2(const char* Format, const char* pText, ...) noexcept;
int __utf8_printf2(const char* Format, CMaaString Text, ...) noexcept;
int __utf8_printf2(CMaaString Format, CMaaString Text, ...) noexcept;
int __utf8_printf2(CMaaString Format, const char* pText, ...) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
int __utf8_printf2(const char* Format, const char8_t* pText, ...) noexcept;
int __utf8_printf2(CMaaString Format, const char8_t* pText, ...) noexcept;
#endif
int __utf8_fprintf(CMaaFile file, CMaaString format, ...);

int __utf8_printfV(const char* format, va_list list) noexcept;
#ifdef TOOLSLIB_CHAR8T_SUPPORT
int __utf8_printfV(const char8_t* format, va_list list) noexcept;
#endif
int __utf8_printfV(CMaaString format, va_list list) noexcept;
int __utf8_printfV2(CMaaString Format, CMaaString Text, va_list list) noexcept;

int __utf8_printf_out(CMaaString txt) noexcept;
int __utf8_printf_flush() noexcept;
int __utf8_printf_isatty() noexcept;

#ifdef _WIN32
class CMaaDllProc
{
    int f;
    HMODULE h = NULL;
    FARPROC p = NULL;
public:
    CMaaDllProc();
    ~CMaaDllProc();
    FARPROC Get(const wchar_t* Dll, const char* Proc) noexcept;
};
#endif

bool MaaSetConsoleLucidaFont();
bool MaaSetConsoleUtf8AndLocale();

class CMaaSetConsoleUtf8AndLocale
{
    bool m_bPreserveCp;
    const char* m_pPresLocale;
#ifdef _WIN32
    UINT m_ConsoleCp, m_ConsoleOutputCp;
    CMaaString m_uConsoleFont;
    CONSOLE_FONT_INFOEX m_cfi;
#else
    locale_t loc;
#endif
public:
    CMaaSetConsoleUtf8AndLocale(bool bPreserveCp = true);
    ~CMaaSetConsoleUtf8AndLocale();
    void SetUtf8();
    void Restore();
};
CMaaString MaaReadUtf8ConsoleLine1(CMaaString term_ch, int max_chars = -1, bool bEcho = true);

int __lib_printf(CMaaString format, ...);
int __lib_printf2(const char *Format, CMaaString Text, ...);
int __lib_printf_flush();
extern int (*g__lib_printf)(CMaaString txt);
extern int (*g__lib_printf_flush)();
extern int __lib_utf8_printf(CMaaString txt); // the sample: g__lib_printf = &__lib_utf8_printf;

#endif // __TOOLS_FILE2_H

//==============================================================================
