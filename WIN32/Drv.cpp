//==============================================================================
// PRODUCT: ToolsLib
//==============================================================================
// FILE:       Drv.cpp    24.04.2003
//
// AUTHOR:     Andrey A. Moisseenko  <support@maasoftware.ru>
//==============================================================================

#include "perm.h"
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifdef _WIN32
void (* gptr_error_dbg_printf_function)(const char * text) = nullptr;

void error_dbg_printf(const char * text)
{
    if  (gptr_error_dbg_printf_function)
    {
        gptr_error_dbg_printf_function(text);
    }
    else
    {
        //printf("%s\n", text);
    }
}

/*
static CMaaAtomicFastMutex sLogLock;
#ifdef _DEBUG_MTX
static CMaaAtomicFastMutex* spLogLockDeleted = (CMaaAtomicFastMutex*)1;
*/

static CMaa_gLock_lib_Mutex sLogLock;
#ifdef _DEBUG_MTX
static CMaa_gLock_lib_Mutex * spLogLockDeleted = (CMaa_gLock_lib_Mutex *)1;
#endif

//const char * gLockMsg[10] = {"<>", "<>", "<>", "<>", "<>", "<>", "<>", "<>", "<>", "<no lock>"};

int CMaaWin32Lock::GetDepthEx() noexcept
{
    //LockF(__FILE__, __LINE__);
    Lock();
    const int n = xx - 1;
    UnLock();
    return n;
}

bool CMaaWin32Lock::GetLockHolder(int x, char * txt, int buffer_len) const noexcept
{
    if  (x < 0 
#ifdef _DEBUG_MTX
        || x >= m_nSrc
#else
        || x >= xx
#endif
        || buffer_len < 100)
    {
        return false;
    }

    sprintf(txt, "[%d]: ", x);
    int n0 = int_strlen(txt);
#ifndef _DEBUG_MTX
    const char * FileName = "release";
    int LineNum = 0;
#else
    const char * FileName = m_SrcFile[x];
    int LineNum = m_SrcLine[x];
    FileName = FileName ? FileName : "null";
#endif
    int n = (int)strlen(FileName);
    n = n0 + n < buffer_len - 30 ? n : buffer_len - 30 - n0;
    n = n >= 0 ? n : 0;
    memcpy(n0 + txt, FileName, n);
    sprintf(n0 + n + txt, "(%d)", LineNum);
    return true;
}

DWORD CMaaWin32Lock::LockF(const char * SrcFile, int SrcLine) noexcept
{
#ifdef _DEBUG_MTX
    if  (this == spLogLockDeleted)
    {
        return WAIT_OBJECT_0;
    }
#endif
#ifndef _DEBUG_MTX
    DWORD x = Lock(INFINITE);
#else
    DWORD DLTO = 60000;
    DWORD x = Lock(DLTO);
    static int additional_wait_for_log_saved = 0;
    if  (x == WAIT_TIMEOUT)
    {
        x = Lock((++additional_wait_for_log_saved % 32) * 1000);
        if  (x == WAIT_TIMEOUT)
        {

            time_t t = time(nullptr);
            tm t0;
            tm * p = localtime(&t);
            if  (p)
            {
                memcpy(&t0, p, sizeof(t0));
            }
            else
            {
                memset(&t0, 0, sizeof(t0));
            }

            FILE * f = fopen("c:\\maa\\CMaaWin32Locker.txt", "a+b");
            if  (f)
            {
                char txt[256];
                /*
               time_t t = time(nullptr);
               tm t0;
               tm * p = localtime(&t);
               if   (p)
               {
                    memcpy(&t0, p, sizeof(t0));
               }
               else
               {
                    memset(&t0, 0, sizeof(t0));
               }
               */
                fprintf(f, "\r\n%2d.%02d.%d %02d:%02d:%02d: Possible deadlock (%d ms timeout): call from %s(%d):\r\n",
                     t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec,
                     (int)DLTO, SrcFile ? SrcFile : "null", SrcLine);
                for (int i = 0; ; i++)
                {
                    if  (!GetLockHolder(i, txt, (int)(sizeof(txt))))
                    {
                        break;
                    }
                    fprintf(f, "%s\r\n", txt);
                }
                fclose(f);
            }

            x = Lock();
        }
    }
    int n = m_nSrc - 1;
    if  (x == WAIT_OBJECT_0 && n >= 0 && n < 20)
    {
        m_SrcFile[n] = SrcFile;
        m_SrcLine[n] = SrcLine;
        m_SrcThreadId[n] = GetCurrentThreadId();
    }
#endif
    return x;
}

void CMaaWin32Lock::StillLocked(const char* SrcFile, int SrcLine) noexcept
{
#ifdef _DEBUG_MTX
    int n = m_nSrc - 1;
    if (n >= 0 && n < 20 && m_SrcThreadId[n] == GetCurrentThreadId())
    {
        m_SrcFile[n] = SrcFile;
        m_SrcLine[n] = SrcLine;
        //m_SrcThreadId[n] = GetCurrentThreadId();
    }
#endif
}

CMaaWin32Lock::CMaaWin32Lock(BOOL fInitiallyLocked) noexcept
{
    xx = 0;
    m_RefCount = 1;
#ifdef _DEBUG_MTX
    m_nSrc = 0;
#endif
    m_hMutex = CreateMutex(nullptr, fInitiallyLocked, nullptr);
    if  (fInitiallyLocked)
    {
        ++xx;
        ++m_RefCount;
#ifdef _DEBUG_MTX
        int n = m_nSrc++;
        if  (n >= 0 && n < 20)
        {
            m_SrcFile[n] = "initially locked";
            m_SrcLine[n] = -1;
            m_SrcThreadId[n] = GetCurrentThreadId();
        }
#endif
    }
}
CMaaWin32Lock::~CMaaWin32Lock() noexcept
{
#ifdef _DEBUG_MTX
    if  (this == &sLogLock)
    {
        spLogLockDeleted = this;
    }
    FlushLog(true);
#endif
    CloseHandle(m_hMutex);
}

void CMaaWin32Lock::SetLogFile(CMaaString LogFileName) noexcept
{
#ifdef _DEBUG_MTX
    sLogLock.Lock();
    FlushLog(true);
    m_LogFileName = LogFileName;
    sLogLock.UnLock();
#endif
}

void CMaaWin32Lock::FlushLog(bool bForced) noexcept
{
#ifdef _DEBUG_MTX
    sLogLock.Lock();
    if  (m_LogFileName.IsValid() && m_Log.IsNotEmpty() && (bForced || m_Log.Length() > 10 * 1024))
    {
        try
        {
            CMaaFile f(m_LogFileName, CMaaFile::eAC);
            if  (f.CurPos() >= 100000000)
            {
                f.Seek(0);
                f.Truncate();
            }
            f.Write(m_Log, m_Log.Length());
        }
        catch(...)
        {
        }
        m_Log.Empty();
    }
    sLogLock.UnLock();
#endif
}

DWORD CMaaWin32Lock::Lock(const char * txt) noexcept
{
#ifdef _DEBUG_MTX
    if  (this == spLogLockDeleted)
    {
        return WAIT_OBJECT_0;
    }
#endif
    const DWORD dwTimeToWait = INFINITE;
    DWORD x = ::WaitForSingleObject ( m_hMutex, dwTimeToWait );
#ifdef _DEBUG_MTX
    if  (m_LogFileName.IsValid())
    {
        sLogLock.Lock();
        const char * szStatus =
        x == WAIT_ABANDONED ? "WAIT_ABANDONED" :
        x == WAIT_OBJECT_0 ? "WAIT_OBJECT_0" :
        x == WAIT_TIMEOUT ? "WAIT_TIMEOUT" :
        x == WAIT_FAILED ? "WAIT_FAILED" : "?";
        m_Log.Append("Lock(%s): x == %d (%s), e = %d\r\n", txt, x, szStatus, GetLastError());
        FlushLog();
        sLogLock.UnLock();
    }
#endif
    if  (x == WAIT_ABANDONED)
    {
        x = WAIT_OBJECT_0;
        m_RefCount -= xx;
        xx = 0;
#ifdef _DEBUG_MTX
        m_nSrc = 0;
#endif
    }
    if  (x == WAIT_OBJECT_0)
    {
        ++xx;
        ++m_RefCount;
#ifdef _DEBUG_MTX
        int n = m_nSrc++;
        if (n >= 0 && n < 20)
        {
            m_SrcFile[n] = txt;
            m_SrcLine[n] = -1;
            m_SrcThreadId[n] = GetCurrentThreadId();
        }
#endif
    }
    /*
    else
    {
        // error
    }
    */
    return x;
}
DWORD CMaaWin32Lock::Lock(DWORD dwTimeToWait) noexcept
{
#ifdef _DEBUG_MTX
    if  (this == spLogLockDeleted)
    {
        return WAIT_OBJECT_0;
    }
#endif
    DWORD x = ::WaitForSingleObject(m_hMutex, dwTimeToWait);
#ifdef _DEBUG_MTX
    if  (m_LogFileName.IsValid())
    {
        sLogLock.Lock();
        const char * szStatus =
        x == WAIT_ABANDONED ? "WAIT_ABANDONED" :
        x == WAIT_OBJECT_0 ? "WAIT_OBJECT_0" :
        x == WAIT_TIMEOUT ? "WAIT_TIMEOUT" :
        x == WAIT_FAILED ? "WAIT_FAILED" : "?";
        m_Log.Append("Lock(%d): x == %d (%s), e = %d\r\n", dwTimeToWait, x, szStatus, GetLastError());
        FlushLog();
        sLogLock.UnLock();
    }
#endif
    if  (x == WAIT_ABANDONED)
    {
        x = WAIT_OBJECT_0;
        m_RefCount -= xx;
        xx = 0;
#ifdef _DEBUG_MTX
        m_nSrc = 0;
#endif
    }
    if (x == WAIT_OBJECT_0)
    {
        ++xx;
        m_RefCount++;
#ifdef _DEBUG_MTX
        int n = m_nSrc++;
        if (n >= 0 && n < 20)
        {
            m_SrcFile[n] = "unnamed";
            m_SrcLine[n] = -1;
            m_SrcThreadId[n] = GetCurrentThreadId();
        }
#endif
    }
    /*
    else
    {
        // error
        if (x == WAIT_ABANDONED)
        {
            //static int aa = 0;
            //aa++;
        }
        else if (x == WAIT_TIMEOUT)
        {
            //static int aa = 0;
            //aa++;
        }
        else if (x == WAIT_FAILED)
        {
            //static int aa = 0;
            //int e = GetLastError();
            //aa++;
        }
        else
        {
            //static int aa = 0;
            //int e = GetLastError();
            //aa++;
        }
    }
    */
    return x;
}
DWORD CMaaWin32Lock::TryLock() noexcept
{
    return Lock((DWORD)0);
}
int CMaaWin32Lock::UnLock() noexcept
{
#ifdef _DEBUG_MTX
    return UnLockF("unknown", -2);
#else
    const int n = --xx;
    const int nRefCount = --m_RefCount;
    if (!::ReleaseMutex(m_hMutex))
    {
        // error !!!
        ++xx;
        /*nRefCount =*/ ++m_RefCount;
        return -1;
    }
    else
    {
        if (!nRefCount)
        {
            delete this;
            return -1;
        }
    }
    return n;
#endif
}
int CMaaWin32Lock::UnLockF(const char * file, int line) noexcept
{
#ifdef _DEBUG_MTX
    if  (this == spLogLockDeleted)
    {
        return WAIT_OBJECT_0;
    }
    if  (m_LogFileName.IsValid())
    {
        sLogLock.Lock();
        m_Log.Append("Unlocking...\r\n");
        FlushLog();
        sLogLock.UnLock();
    }
#endif
    if  (xx <= 0
#ifndef _DEBUG_MTX
        && gptr_error_dbg_printf_function
#endif
        )
    {
        if  (line != -2)
        {
            // pre handling error
            char txt[256];
            int buffer_len = (int)sizeof(txt);

            sprintf(txt, "CMaaWin32Lock::UnLockF(");
            int n0 = int_strlen(txt);
            file = file ? file : "null";

            int n = (int)strlen(file);
            n = n0 + n < buffer_len - 80 ? n : buffer_len - 80 - n0;
            n = n >= 0 ? n : 0;
            memcpy(n0 + txt, file, n);
            sprintf(n0 + n + txt, ", %d) error : xx = %d (lock count less then 0)", line, xx);

#ifdef _DEBUG_MTX
            time_t t = time(nullptr);
            tm t0;
            tm* p = localtime(&t);
            if  (p)
            {
                memcpy(&t0, p, sizeof(t0));
            }
            else
            {
                memset(&t0, 0, sizeof(t0));
            }

            FILE* f = fopen("c:\\maa\\CMaaWin32Locker.txt", "a+b");
            if  (f)
            {
                fprintf(f, "\r\n%2d.%02d.%d %02d:%02d:%02d: %s\r\n",
                    t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec, txt);
                fclose(f);
            }
#endif
            error_dbg_printf(txt);
        }
    }

    const int n = --xx;

#ifdef _DEBUG_MTX
    --m_nSrc;
#endif

    const int nRefCount = --m_RefCount;
    if  (!::ReleaseMutex ( m_hMutex ))
    {
        // error !!!
        ++xx;
        /*nRefCount =*/ ++m_RefCount;

#ifdef _DEBUG_MTX
        m_nSrc++;
        file = file ? file : "null";
        if  (m_LogFileName.IsValid())
        {
            sLogLock.Lock();
            m_Log.Append("ReleaseMutex() failed: e = %d, file = %s, line = %d\r\n", GetLastError(), file, line);
            FlushLog();
            sLogLock.UnLock();
        }
#endif
        return -1;
    }
    else
    {
#ifdef _DEBUG_MTX
        if  (m_LogFileName.IsValid())
        {
            sLogLock.Lock();
            m_Log.Append("UnLock() - ok\r\n");
            FlushLog();
            sLogLock.UnLock();
        }
#endif
        if  (!nRefCount)
        {
            delete this;
            return -1;
        }
    }
    return n;
}

void CMaaWin32Lock::AddRef() noexcept
{
    Lock();
    ++m_RefCount;
    UnLock();
}

int CMaaWin32Lock::UnRef() noexcept
{
    Lock();
    --m_RefCount;
    return UnLock();
}
int CMaaWin32Lock::GetRefCount() noexcept
{
    /*
    Lock();
    const int n = m_RefCount;
    UnLock();
    return n - 1;
    */
    return m_RefCount;
}

CMaaWin32Lock::operator HANDLE() const noexcept
{
    return m_hMutex;
}

//---------------------------------------------------------------------------------------------
#if 0
CMaaWin32Locker::CMaaWin32Locker(CMaaMutex &Lock, CMaaString
#ifndef _DEBUG_MTX
    &
#endif
    Name) noexcept
:   m_Lock(Lock)
#ifdef _DEBUG_MTX
    , m_Name(Name)
#endif
{
    m_x = 0;
    m_bUnLockerMode = false;
}
CMaaWin32Locker::CMaaWin32Locker(CMaaMutex & Lock, const char* Name) noexcept
:   m_Lock(Lock)
    //, m_Name(Name)
{
    m_x = 0;
    m_bUnLockerMode = false;
}
CMaaWin32Locker::CMaaWin32Locker(CMaaMutex & Lock) noexcept
:   m_Lock(Lock)
{
    m_x = 0;
    m_bUnLockerMode = false;
}
CMaaWin32Locker::~CMaaWin32Locker() noexcept
{
    if  ((m_x > 0 && !m_bUnLockerMode) || m_x != 0)
    {
        char txt[256];

#ifdef _DEBUG_MTX
        FILE * f = fopen("c:\\maa\\CMaaWin32Locker.txt", "a+b");
        if  (f)
        {
            time_t t = time(nullptr);
            tm t0;
            tm * p = localtime(&t);
            if  (p)
            {
                memcpy(&t0, p, sizeof(t0));
            }
            else
            {
                memset(&t0, 0, sizeof(t0));
            }
            fprintf(f, "%2d.%02d.%d %02d:%02d:%02d CMaaWin32Locker::~CMaaWin32Locker(): error: m_x=%d, m_Name=%s\r\n",
                 t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec,
                 m_x, (const char *)m_Name);
            for (int i = 0; ; i++)
            {
                if  (!m_Lock.GetLockHolder(i, txt, (int)(sizeof(txt))))
                {
                    break;
                }
                fprintf(f, "%s\r\n", txt);
            }
            fclose(f);
        }
#endif
        strcpy(txt, "CMaaWin32Locker::~CMaaWin32Locker() name \"");
        int n0 = int_strlen(txt);
        int n = m_Name.Length();
        n = n0 + n < (int)sizeof(txt) - 30 ? n : (int)sizeof(txt) - 30 - n0;
        n = n >= 0 ? n : 0;
        memcpy(n0 + txt, (const char *)m_Name, n);
        //txt[(n0 += n)] = 0;
        sprintf(n0 + n + txt, "\" m_x == %d", m_x);
        error_dbg_printf(txt);
        for (int i = 0; ; i++)
        {
            if  (!m_Lock.GetLockHolder(i, txt, (int)(sizeof(txt))))
            {
                break;
            }
            error_dbg_printf(txt);
        }
        while(m_x > 0)
        {
            m_Lock.UnLock();
            --m_x;
        }
        while(m_x < 0)
        {
            m_Lock.Lock();
            ++m_x;
        }
    }
}
DWORD CMaaWin32Locker::Lock(const char * txt) noexcept
{
    DWORD x = m_Lock.Lock(txt);
    if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    {
        ++m_x;
    }
    return x;
}
#ifdef TOOLSLIB_MAX_SPEED111
DWORD CMaaWin32Locker::Lock(DWORD dwTimeToWait) noexcept
{
    DWORD x = m_Lock.Lock(dwTimeToWait);
    if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    {
        ++m_x;
    }
    return x;
}
#else
DWORD CMaaWin32Locker::Lock(DWORD dwTimeToWait) noexcept
{
    DWORD x = m_Lock.Lock(dwTimeToWait);
    if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    {
        ++m_x;
    }
    return x;
}
#endif
DWORD CMaaWin32Locker::LockF(const char * file, int line) noexcept
{
    DWORD x = m_Lock.LockF(file, line);
    if  (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
    {
        ++m_x;
    }
    return x;
}
int CMaaWin32Locker::UnLock() noexcept
{
    return UnLockF("unknown", -2);
}
int CMaaWin32Locker::UnLockF(const char * file, int line) noexcept
{
    if  (m_x <= 0 && !m_bUnLockerMode
#ifndef _DEBUG_MTX
        && gptr_error_dbg_printf_function
#endif
        )
    {
        if  (line != -2)
        {
            char txt[256];

            file = file ? file : "null";
#ifdef _DEBUG_MTX
            FILE* f = fopen("c:\\maa\\CMaaWin32Locker.txt", "a+b");
            if  (f)
            {
                time_t t = time(nullptr);
                tm t0;
                tm* p = localtime(&t);
                if  (p)
                {
                    memcpy(&t0, p, sizeof(t0));
                }
                else
                {
                    memset(&t0, 0, sizeof(t0));
                }
                fprintf(f, "%2d.%02d.%d %02d:%02d:%02d CMaaWin32Locker::UnLock(%s, %d): error: m_x=%d, m_Name=%s\r\n",
                    t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec,
                    file, line, m_x, (const char*)m_Name);
                fclose(f);
            }
#endif
            strcpy(txt, "CMaaWin32Locker::UnLock(");
            int n0 = int_strlen(txt);
            int n = int_strlen(file);
            n = n0 + n < (int)sizeof(txt) - 80 ? n : (int)sizeof(txt) - 80 - n0;
            n = n >= 0 ? n : 0;
            memcpy(n0 + txt, file, n);
            sprintf(n0 + n + txt, ", %d) name \"", line);

            n0 = int_strlen(txt);
            n = m_Name.Length();
            n = n0 + n < (int)sizeof(txt) - 30 ? n : (int)sizeof(txt) - 30 - n0;
            n = n >= 0 ? n : 0;
            memcpy(n0 + txt, (const char*)m_Name, n);
            sprintf(n0 + n + txt, "\" m_x == %d", m_x);
            error_dbg_printf(txt);
        }
        return -1;
    }
    --m_x;
    return m_Lock.UnLock();
}
void CMaaWin32Locker::SetUnLockerMode(bool bSet) noexcept
{
    m_bUnLockerMode = bSet;
}
#endif
//---------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------
CMaaWin32Semaphore::CMaaWin32Semaphore(int MaxCount, int InitialCount) noexcept
{
    m_hSemaphore = CreateSemaphore(nullptr, InitialCount < 0 ? MaxCount : InitialCount, MaxCount, nullptr);
    m_RefCount = 1;
}
CMaaWin32Semaphore::~CMaaWin32Semaphore() noexcept
{
    CloseHandle(m_hSemaphore);
}

/*
DWORD CMaaWin32Semaphore::Lock(const char * txt)
{
     DWORD dwTimeToWait = INFINITE;
     DWORD x = ::WaitForSingleObject ( m_hSemaphore, dwTimeToWait );
     if   (x == WAIT_ABANDONED)
     {
          static int aa = 0;
          aa++;
          x = WAIT_OBJECT_0;
     }
     if   ( x != WAIT_OBJECT_0 )
     {
          // error
     }
     else
     {
          if   (this == &gLock)
          {
               for  (int i = 0; i < 9; i++)
               {
                    gLockMsg[i] = gLockMsg[i+1];
               }
               gLockMsg[9] = txt;
          }
          ++xx;
          if   (m_bLog && 0)
          {
               FILE * f = fopen("c:\\log2.txt", "a+b");
               if   (f)
               {
                    fprintf(f, "lock %d %s\n", xx, txt);
                    fclose(f);
               }
          }
     }
     return x;
}
*/
DWORD CMaaWin32Semaphore::Lock(DWORD dwTimeToWait) noexcept
{
    DWORD x = ::WaitForSingleObject(m_hSemaphore, dwTimeToWait);
    if  (x == WAIT_ABANDONED)
    {
        //static int aa = 0;
        //aa++;
        x = WAIT_OBJECT_0;
        m_RefCount = 1;
    }
    if  ( x != WAIT_OBJECT_0 )
    {
        // error
        /*
          if (x == WAIT_ABANDONED)
          {
               //static int aa = 0;
               //aa++;
          }
          else if (x == WAIT_TIMEOUT)
          {
               //static int aa = 0;
               //aa++;
          }
          else if (x == WAIT_FAILED)
          {
               //static int aa = 0;
               //int e = GetLastError();
               //aa++;
          }
          else
          {
               //static int aa = 0;
               //int e = GetLastError();
               //aa++;
          }
          */
    }
    else
    {
        ++m_RefCount;
    }
    return x;
}
DWORD CMaaWin32Semaphore::TryLock() noexcept
{
    return Lock((DWORD)0);
}
void CMaaWin32Semaphore::UnLock(int Count) noexcept
{
    LONG PreviousCount = 0;
    const int n = m_RefCount -= Count;
    if  (!::ReleaseSemaphore(m_hSemaphore, Count, &PreviousCount))
    {
        m_RefCount += Count;
        //static int aa = 0;
        //aa++;
    }
    else
    {
        if  (n <= 0)
        {
            delete this;
        }
    }
}
//---------------------------------------------------------------------------------------------

CMaaAppOwner::CMaaAppOwner ()
:   m_fOwner ( TRUE ),
    m_OwnEvent ( nullptr ),
    m_PopupEvent ( nullptr )
{
}

void CMaaAppOwner::SetAppName ( const char * AppName )
{
    CMaaAppOwner ow ( AppName );
    BOOL f;
    HANDLE h;

    f = m_fOwner;
    m_fOwner = ow.m_fOwner;
    ow.m_fOwner = f;

    h = m_OwnEvent;
    m_OwnEvent = ow.m_OwnEvent;
    ow.m_OwnEvent = h;

    h = m_PopupEvent;
    m_PopupEvent = ow.m_PopupEvent;
    ow.m_PopupEvent = h;
}

CMaaAppOwner::CMaaAppOwner ( const char * AppName )
:   m_fOwner ( TRUE ),
    m_OwnEvent ( nullptr ),
    m_PopupEvent ( nullptr )
{
    char txt [ 512 ];
    if  ( strlen ( AppName ) > 100 )
    {
        return;
    }
    sprintf ( txt, "%s_Mutex", AppName );
    HANDLE hMutex = CreateMutexA ( nullptr, TRUE, txt );
    if  ( hMutex && ::WaitForSingleObject ( hMutex, 10000 ) != WAIT_OBJECT_0 )
    {
        m_fOwner = FALSE;
    }
    else
    {
        sprintf ( txt, "%s_OwnEvent", AppName );
        m_OwnEvent = CreateEventA ( nullptr, TRUE, FALSE, txt );
        if  ( /**/!m_OwnEvent ||/**/ ::WaitForSingleObject ( m_OwnEvent, 0 ) == WAIT_OBJECT_0 )
        {
            m_fOwner = FALSE;
        }
        else
        {
            SetEvent ( m_OwnEvent );
        }
        sprintf ( txt, "%s_PopupEvent", AppName );
        m_PopupEvent = CreateEventA ( nullptr, TRUE, FALSE, txt );
    }
    if  ( hMutex )
    {
        ::ReleaseMutex ( hMutex );
        CloseHandle ( hMutex );
    }
}

CMaaAppOwner::~CMaaAppOwner ()
{
    if  ( m_OwnEvent )
    {
        if  ( m_fOwner )
        {
            ResetEvent ( m_OwnEvent );
        }
        CloseHandle ( m_OwnEvent );
    }
    if  ( m_PopupEvent )
    {
        CloseHandle ( m_PopupEvent );
    }
}

BOOL CMaaAppOwner::IsOwner ()
{
    return m_fOwner;
}

void CMaaAppOwner::Popup ()
{
    if  ( m_PopupEvent )
    {
        SetEvent ( m_PopupEvent );
    }
}

CMaaAppLocker::CMaaAppLocker(const char * AppName)
{
    m_LockCount = 0;
    m_hMutex = INVALID_HANDLE_VALUE;
    if  (strlen(AppName) > 100)
    {
        return;
    }
    char txt[256];
    sprintf(txt, "%s_CMaaAppLocker", AppName);
    m_hMutex = CreateMutexA(nullptr, FALSE, txt);
}
CMaaAppLocker::~CMaaAppLocker()
{
    if  (m_hMutex != INVALID_HANDLE_VALUE)
    {
        for (; m_LockCount > 0; m_LockCount--)
        {
            ::ReleaseMutex(m_hMutex);
        }
        CloseHandle(m_hMutex);
    }
}
bool CMaaAppLocker::IsLocked()
{
    return m_LockCount > 0;
}
bool CMaaAppLocker::Lock(DWORD TimeOut)
{
    DWORD w = ::WaitForSingleObject(m_hMutex, TimeOut);
    if  (w == WAIT_OBJECT_0 || w == WAIT_ABANDONED)
    {
        m_LockCount++;
        return true;
    }
    if  (w != WAIT_TIMEOUT)
    {
        //printf("w = 0x%x=%d\n", w, w);
    }
    return false;
}
void CMaaAppLocker::UnLock()
{
    if  (IsLocked())
    {
        ::ReleaseMutex(m_hMutex);
        m_LockCount--;
    }
}


#ifndef __TOOLSLIB_REMOVE_OLD_CODE
BOOL InstallVxD ( const char * pFullVxDFileName1, BOOL fInstall )
{
    CMaaString buf(nullptr, 1024);
    char * b = (char *)(const char *)buf;
    if  ( !GetShortPathName ( pFullVxDFileName1, b, buf.Length() ) )
    {
        // File does not exist
        if  ( fInstall )
        {
            return FALSE;
        }
        else
        {
            strcpy ( b, pFullVxDFileName1 );
        }
    }
    char * pFullVxDFileName = b;

    if  ( strlen ( pFullVxDFileName ) <= strlen ( ".vxd" ) )
    {
        return FALSE;
    }
    char * p = pFullVxDFileName + strlen ( pFullVxDFileName ) - strlen ( ".vxd" );
    if  ( stricmp ( p, ".vxd" ) && strcmp ( p, ".386" ) )
    {
        return FALSE;
    }
    p = strrchr ( pFullVxDFileName, '\\' );
    p = p ? p + 1 : pFullVxDFileName;
    char Name [ 8 + 5 ];
    if  ( strlen ( p ) > 8 + strlen ( ".vxd" ) )
    {
        return FALSE;
    }
    strcpy ( Name, p );
    Name [ strlen ( Name ) - strlen ( ".vxd" ) ] = 0;
    CMaaString RegPath ( "System\\CurrentControlSet\\Services\\VxD" );
    if  ( fInstall )
    {
        // Install
        RegPath += "\\";
        RegPath += Name;

        CTooRegHLM r ( RegPath, CTooReg::WriteAndCreateThisKey );
        r.SetValue ( "StaticVxD", pFullVxDFileName );
        r.SetValue ( "Start", "", 1 );
    }
    else
    {
        // Uninstall
        try
        {
            CTooRegHLM r ( RegPath );
            r.DeleteAll ( Name );
        }
        catch( const char * )
        {
        }
        // Deleting device=Path\\File.vxd from [386Enh] section of system.ini
        CMaaBuffer bf;
        bf [ 0 ] = 0;
        ::GetWindowsDirectory ( bf, bf.Size () );
        if  ( !bf [ 0 ] || bf [ ( int ) strlen ( bf ) - 1 ] != '\\' )
        {
            strcat ( bf, "\\" );
        }
        CMaaString SysDir ( bf );
        CMaaString ini ( SysDir ), tmp ( SysDir ), bak ( SysDir );
        ini += "system.ini";
        tmp += "system.$$$";
        bak += "system.bak";
        FILE * f = fopen ( ini, "rb" );
        FILE * ft = fopen ( tmp, "wb" );
        if  ( f && ft )
        {
            BOOL f386Enh = FALSE, fExists = FALSE, fError = FALSE;
            while( fgets ( bf, bf.Size () - 2, f ) )
            {
                while( bf [ 0 ] && ( bf [ ( int ) strlen ( bf ) - 1 ] == '\r' || bf [ ( int ) strlen ( bf ) - 1 ] == '\n' ) )
                {
                    bf [ ( int ) strlen ( bf ) - 1 ] = 0;
                }
                if  ( bf [ 0 ] == '[' )
                {
                    f386Enh = FALSE;
                }
                if  ( !strcmp ( bf, "[386Enh]" ) )
                {
                    f386Enh = TRUE;
                }
                if  ( f386Enh )
                {
                    if  ( !memicmp ( bf, "device=", strlen ( "device=" ) ) && strlen ( bf ) >= strlen ( "device=" ) + strlen ( p ) )
                    {
                        if  ( !memicmp ( bf + strlen ( bf ) - strlen ( p ), p, strlen ( p ) ) &&
                             ( bf [ ( int ) strlen ( bf ) - ( int ) strlen ( p ) - 1 ] == '\\' || bf [ ( int ) strlen ( bf ) - ( int ) strlen ( p ) - 1 ] == '=' ) )
                        {
                            fExists = TRUE;
                            continue;
                        }
                    }
                }
                strcat ( bf, "\r\n" );
                if  ( fwrite ( bf, strlen ( bf ), 1, ft ) != 1 )
                {
                    fError = TRUE;
                    break;
                }
            }
            fclose ( ft );
            fclose ( f );
            if  ( fError || !fExists )
            {
                CMaaFile::Remove ( tmp, XTOOFile(_false) );
            }
            else
            {
                CMaaFile::Remove ( bak, XTOOFile(_false) );
                CMaaFile::MoveFile ( ini, bak, XTOOFile(_false) );
                CMaaFile::MoveFile ( tmp, ini, XTOOFile(_false) );
            }
        }
        else
        {
            if  ( ft )
            {
                fclose ( ft );
                CMaaFile::Remove ( tmp, XTOOFile(_false) );
            }
            if  ( f )
            {
                fclose ( f );
            }
        }
    }
    return TRUE;
}
#endif //#ifndef __TOOLSLIB_REMOVE_OLD_CODE

// HRESULT SHGetFolderPath ( HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath );

//---------------------------------------------------------------------------
// class CMaaDrvApi
//---------------------------------------------------------------------------

CMaaDrvApi::CMaaDrvApi ( const char * pszFileName ) noexcept
{
#ifndef __unix__
    OSVERSIONINFO Info;
    Info.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );
    BOOL bV = GetVersionEx ( & Info );
    m_IsWinNT = bV ? ( Info.dwPlatformId == VER_PLATFORM_WIN32_NT ? true : false ) : false;

    char txt [ 300 ];
    if  (strlen(pszFileName) + strlen("\\\\.\\Globals\\") >= sizeof(txt))
    {
        m_IsLoadSuccessfully = FALSE;
        m_hVxD = INVALID_HANDLE_VALUE;
    }
    else
    {
        strcpy ( txt, "\\\\.\\" );
        strcat ( txt, pszFileName );
        m_IsLoadSuccessfully = FALSE;
        //m_hVxD   = CreateFile ( txt, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, 0 );
        //m_hVxD   = CreateFile ( txt, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE, 0 );
        m_hVxD   = CreateFileA ( txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE, 0 );
        if  ( m_hVxD == INVALID_HANDLE_VALUE )
        {
            m_hVxD   = CreateFileA ( txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0 );
        }
        if  ( m_hVxD == INVALID_HANDLE_VALUE )
        {
            strcpy ( txt, "\\\\.\\Globals\\" );
            strcat ( txt, pszFileName );
            m_hVxD   = CreateFileA ( txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE, 0 );
            if  ( m_hVxD == INVALID_HANDLE_VALUE )
            {
                m_hVxD   = CreateFileA ( txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0 );
            }
        }
    }
    if  ( m_hVxD == INVALID_HANDLE_VALUE )
    {
        DWORD err = GetLastError ();
        m_ovlp.hEvent = 0;
    }
    else
    {
        m_ovlp.hEvent = 0;
        m_ovlp.hEvent = CreateEvent ( 0, TRUE, 0, nullptr );
        if  ( m_ovlp.hEvent )
        {
            m_IsLoadSuccessfully = TRUE;
        }
    }
#else
    m_IsLoadSuccessfully = TRUE;
#endif
    //!__unix__
}

CMaaDrvApi::CMaaDrvApi(const wchar_t* pszFileName) noexcept
{
#ifndef __unix__
    OSVERSIONINFO Info;
    Info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    BOOL bV = GetVersionEx(&Info);
    m_IsWinNT = bV ? (Info.dwPlatformId == VER_PLATFORM_WIN32_NT ? true : false) : false;

    wchar_t txt[300];
    if (maa_wcslen(pszFileName) + maa_wcslen(L"\\\\.\\Globals\\") >= sizeof(txt) / sizeof(txt[0]))
    {
        m_IsLoadSuccessfully = FALSE;
        m_hVxD = INVALID_HANDLE_VALUE;
    }
    else
    {
        wcscpy(txt, L"\\\\.\\");
        wcscat(txt, pszFileName);
        m_IsLoadSuccessfully = FALSE;
        //m_hVxD   = CreateFile ( txt, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, 0 );
        //m_hVxD   = CreateFile ( txt, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE, 0 );
        m_hVxD = CreateFileW(txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE, 0);
        if (m_hVxD == INVALID_HANDLE_VALUE)
        {
            m_hVxD = CreateFileW(txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
        }
        if (m_hVxD == INVALID_HANDLE_VALUE)
        {
            wcscpy(txt, L"\\\\.\\Globals\\");
            wcscat(txt, pszFileName);
            m_hVxD = CreateFileW(txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_DELETE_ON_CLOSE, 0);
            if (m_hVxD == INVALID_HANDLE_VALUE)
            {
                m_hVxD = CreateFileW(txt, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
            }
        }
    }
    if (m_hVxD == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        m_ovlp.hEvent = 0;
    }
    else
    {
        m_ovlp.hEvent = 0;
        m_ovlp.hEvent = CreateEvent(0, TRUE, 0, nullptr);
        if (m_ovlp.hEvent)
        {
            m_IsLoadSuccessfully = TRUE;
        }
    }
#else
    m_IsLoadSuccessfully = TRUE;
#endif
    //!__unix__
}

CMaaDrvApi::~CMaaDrvApi ()
{
#ifndef __unix__
    if  ( m_hVxD != INVALID_HANDLE_VALUE )
    {
        CloseHandle ( m_hVxD );
    }
    if  ( m_ovlp.hEvent )
    {
        CloseHandle ( m_ovlp.hEvent );
    }
#endif  //!__unix__
}

//---------------------------------------------------------------------------
BOOL CMaaDrvApi::VxdIoControl ( DWORD dwService, const void * BuffIn,  int SizeIn, void * BuffOut, int SizeOut, int * SizeRet ) const noexcept
{
    DWORD l_SizeRet = -1;
    //m_SizeRet = l_SizeRet;
    if  ( ! m_IsLoadSuccessfully )
    {
        return 1;
    }
    BOOL Ret = FALSE;
#ifndef __unix__
    //SetLastError ( 0 );
    //OVERLAPPED    l_ovlp; // = {0,0,0,0,0};
    if  ( m_hVxD != INVALID_HANDLE_VALUE )
    {
        Ret = DeviceIoControl(m_hVxD, dwService, (void *)BuffIn, SizeIn, BuffOut, SizeOut, &l_SizeRet, nullptr /*&m_ovlp*/);
    }
    //m_SizeRet = l_SizeRet;

    if  ( Ret && SizeRet )
    {
        * SizeRet = l_SizeRet;
    }

    /*
     if (!Ret)
     {
          //printf ( "Error code = %d\n", GetLastError() );
          if   ( GetLastError() == ERROR_IO_PENDING )
          {
               //printf("VxD correctly returned operation incomplete.\n");
          }
          else
          {
               //ERROR_OUT("VxD does not support the requested API!!!\n");
          }
     }
     else
     {
          //ERROR_OUT("VxD processed the call synchronously!!!\n");
     }
     */

    //
    // DeviceIoControl call will have returned without completing
    // requested function. GetOverlappedResult at this point
    // should return ERROR_IO_INCOMPLETE if called w/ fWait=FALSE.
    //
    /*
     if   ( ! GetOverlappedResult ( hVxD, &ovlp, &cbRet, FALSE ) )
     {
          if ( GetLastError() == ERROR_IO_INCOMPLETE )
          {
               printf("GetOverlappedResult returned expected value.\n");
          }
          else
          {
               ERROR_OUT("GetOverlappedResult returned unexpected error.\n");
          }
     }
     else
     {
          ERROR_OUT("GetOverlappedResult unexpectedly returned success.\n");
     }
     */

    /*
     nTicks = GetTickCount();

     //
     // This call to GetOverlappedResult will suspend this thread
     // until the operation is completed by the VxD. I.e. until the
     // VxD calls DIOC_VWIN32CompletionRoutine.
     //
     GetOverlappedResult(hVxD, &ovlp, &cbRet, TRUE);

     nTicks = GetTickCount() - nTicks; // This will wrap after 47 days

     printf("DevIoctl Call elapsed time: %d ms\n", nTicks);
     printf("DevIoctl returned: %d bytes\n", cbRet);
     printf("DevIoctl output: %s\n", OutBuff);
     */
#else
    /*
     struct io_buffer p;

     if ( SizeIn > MAX_IO_IN_SIZE || SizeOut > MAX_IO_OUT_SIZE )
     {
         printf ( "drvapi: too big in or out block!\n" );
         return FALSE;
     }

     memcpy ( p.InBuffer, BuffIn, SizeIn );
     p.InBufferSize = SizeIn;
     p.IES_IO = dwService;
     p.OutBufferSize = SizeOut;

     Ret = myioctl ( DRVIO_IES_IOCTL, & p );

     memcpy ( BuffOut, p.OutBuffer, p.OutBufferSize );

     if ( SizeRet ) * SizeRet = p.OutBufferSize;
*/
#endif //!__unix__
    return Ret;
}
//---------------------------------------------------------------------------
BOOL CMaaDrvApi::IsLoadSuccessfully () const noexcept
{
    return m_IsLoadSuccessfully;
}
//---------------------------------------------------------------------------
typedef DWORD ( WINAPI *pfOpenVxDHandle )( HANDLE hSource );
//---------------------------------------------------------------------------
DWORD CMaaDrvApi::OpenVxDHandle ( HANDLE hWin32Source )
{
    DWORD vh = 0;
    if  ( IsLoadSuccessfully () )
    {
        HMODULE hm = GetModuleHandle ( _T("kernel32.dll") );
        if  ( hm )
        {
            pfOpenVxDHandle pf = ( pfOpenVxDHandle ) GetProcAddress ( hm, "OpenVxDHandle" );
            if  ( pf )
            {
                vh = pf ( hWin32Source );
            }
        }
    }
    return vh;
}
//---------------------------------------------------------------------------


//
// Usage: SetThreadName (-1, "MainThread");
//
//#include <windows.h>
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, const char* threadName) noexcept
{
//#ifdef _DEBUG // worked under vs2022 debugger started, both in the debug and in the release
    THREADNAME_INFO info;
    memset(&info, 0, sizeof(info)); // rem
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID ? dwThreadID : (DWORD)-1; // -1 for current thread
    //info.dwThreadID = dwThreadID ? dwThreadID : GetCurrentThreadId(); // rem
    info.dwFlags = 0;

    __try
    {
#ifndef VC60
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
#else
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG *), (ULONG_PTR*)&info);
#endif
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
//#endif
}

#endif // _WIN32
