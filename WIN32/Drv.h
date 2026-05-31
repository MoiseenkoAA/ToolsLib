
#include "..\Defs.h"    // Added by ClassView

#ifdef DEBUG_MTX

#define LockM() LockF(__FILE__, __LINE__)
#ifdef _WIN32
#define UnLockM() UnLockF(__FILE__, __LINE__)
#else
#define UnLockM UnLock //later
#endif

#else

#define LockM Lock
#define UnLockM UnLock

#endif

class CMaaWin32Lock
{
    HANDLE m_hMutex;
    int xx;
    int m_RefCount;
#ifdef _DEBUG_MTX
    CMaaString m_Log;
    CMaaString m_LogFileName;
    const char * m_SrcFile[20];
    int m_SrcLine[20];
    DWORD m_SrcThreadId[20];
    int m_nSrc;
#endif
public:
    bool GetLockHolder(int x, char * buffer, int buffer_len) const noexcept;
    int GetRefCount() noexcept;
    CMaaWin32Lock(BOOL fInitiallyLocked = FALSE) noexcept;
    ~CMaaWin32Lock() noexcept;
    CMaaWin32Lock(const CMaaWin32Lock&) = delete;
    CMaaWin32Lock(const CMaaWin32Lock&&) = delete;
    CMaaWin32Lock& operator=(const CMaaWin32Lock&) = delete;
    CMaaWin32Lock& operator=(const CMaaWin32Lock&&) = delete;
    operator HANDLE() const noexcept;
    DWORD Lock(const char * txt) noexcept;
    DWORD Lock(DWORD dwTimeToWait = INFINITE) noexcept; // !!!! 2013 !!!!
    DWORD LockF(const char* SrcFile, int SrcLine) noexcept;
    DWORD TryLock() noexcept;
    int UnLockF(const char * file, int line) noexcept;
    int UnLock() noexcept;

    void StillLocked(const char* SrcFile, int SrcLine) noexcept;
    void SetLogFile(CMaaString LogFileName = CMaaString {}) noexcept;
    void FlushLog(bool bForced = false) noexcept;
    int GetDepthEx() noexcept;
    void AddRef() noexcept;
    int UnRef() noexcept;

    int IsLocked() noexcept { return xx; }

    void lock() noexcept { Lock(); }
    void unlock() noexcept { UnLock(); }
    bool try_lock() noexcept { return TryLock() == WAIT_OBJECT_0; }
};

void error_dbg_printf(const char* text);

template<class T> class CMaaWin32Locker
{
    T& m_Lock;
#ifdef _DEBUG_MTX
    CMaaString m_Name;
#else
#define m_Name CMaaStringZ
#endif
    int m_x;
    bool m_bUnLockerMode;
public:
    CMaaWin32Locker(T& Lock, const CMaaString
#ifndef _DEBUG_MTX
        &
#endif
        Name) noexcept
    :   m_Lock(Lock)
#ifdef _DEBUG_MTX
    ,   m_Name(Name)
#endif
    {
        m_x = 0;
        m_bUnLockerMode = false;
    }
    CMaaWin32Locker(T& Lock, const char* Name) noexcept
    :   m_Lock(Lock)
    //, m_Name(Name)
    {
        m_x = 0;
        m_bUnLockerMode = false;
    }
    CMaaWin32Locker(T& Lock) noexcept
    :   m_Lock(Lock)
    {
        m_x = 0;
        m_bUnLockerMode = false;
    }
    ~CMaaWin32Locker() noexcept
    {
        if (/*(m_x > 0 && !m_bUnLockerMode) ||*/ m_x)
        {
            char txt[256];

#ifdef _DEBUG_MTX
            FILE* f = fopen("c:\\maa\\CMaaWin32Locker.txt", "a+b");
            if (f)
            {
                time_t t = time(nullptr);
                tm t0;
                tm* p = localtime(&t);
                if (p)
                {
                    memcpy(&t0, p, sizeof(t0));
                }
                else
                {
                    memset(&t0, 0, sizeof(t0));
                }
                fprintf(f, "%2d.%02d.%d %02d:%02d:%02d CMaaWin32Locker::~CMaaWin32Locker(): error: m_x=%d, m_Name=%s\r\n",
                    t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec,
                    m_x, (const char*)m_Name);
                for (int i = 0; ; i++)
                {
                    if (!m_Lock.GetLockHolder(i, txt, (int)(sizeof(txt))))
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
            int n = (warning_int)m_Name.Length();
            n = n0 + n < (int)sizeof(txt) - 30 ? n : (int)sizeof(txt) - 30 - n0;
            n = n >= 0 ? n : 0;
            memcpy(n0 + txt, (const char*)m_Name, n);
            //txt[(n0 += n)] = 0;
            sprintf(n0 + n + txt, "\" m_x == %d", m_x);
            error_dbg_printf(txt);
            for (int i = 0; ; i++)
            {
                if (!m_Lock.GetLockHolder(i, txt, (int)(sizeof(txt))))
                {
                    break;
                }
                error_dbg_printf(txt);
            }
            while (m_x > 0)
            {
                m_Lock.UnLock();
                --m_x;
            }
            while (m_x < 0)
            {
                m_Lock.Lock();
                ++m_x;
            }
        }
    }
    DWORD Lock(const char* txt) noexcept
    {
        DWORD x = m_Lock.Lock(txt);
        if (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
    DWORD LockF(const char* file, int line) noexcept
    {
        DWORD x = m_Lock.LockF(file, line);
        if (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
    DWORD Lock() noexcept
    {
        DWORD x = m_Lock.Lock();
        if (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
    DWORD Lock(DWORD dwTimeToWait) noexcept
    {
        DWORD x = m_Lock.Lock(dwTimeToWait);
        if (x == WAIT_OBJECT_0 || x == WAIT_ABANDONED)
        {
            ++m_x;
        }
        return x;
    }
    int UnLockF(const char* file, int line) noexcept
    {
        if (m_x <= 0 && !m_bUnLockerMode)
        {
            if (
#ifndef _DEBUG_MTX
                gptr_error_dbg_printf_function &&
#endif
                line != -2)
            {
                char txt[256];

                file = file ? file : "null";
#ifdef _DEBUG_MTX
                FILE* f = fopen("c:\\maa\\CMaaWin32Locker.txt", "a+b");
                if (f)
                {
                    time_t t = time(nullptr);
                    tm t0;
                    tm* p = localtime(&t);
                    if (p)
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
                n = (warning_int)m_Name.Length();
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
    int UnLock() noexcept
    {
        //return UnLockF("unknown", -2);
        if (m_x <= 0 && !m_bUnLockerMode)
        {
            return -1;
        }
        --m_x;
        return m_Lock.UnLock();
    }
    void SetUnLockerMode(bool bSet = true) noexcept
    {
        m_bUnLockerMode = bSet;
    }
    void StillLocked(const char* SrcFile, int SrcLine) noexcept
    {
        m_Lock.StillLocked(SrcFile, SrcLine);
    }
    int IsLocked() const noexcept
    {
        return m_x ? m_Lock.IsLocked() : 0;
    }
#ifndef _DEBUG_MTX
#undef m_Name
#endif
};

class CMaaWin32Semaphore
{
    HANDLE m_hSemaphore;
    //bool m_bLog;
    //int xx;
    int m_RefCount;
public:
    CMaaWin32Semaphore(int MaxCount, int InitialCount = -1) noexcept; // -1 is substituted by MaxCount
    ~CMaaWin32Semaphore() noexcept;
    operator HANDLE() const noexcept
    {
        return m_hSemaphore;
    }
    //DWORD Lock(const char * txt);
    DWORD Lock(DWORD dwTimeToWait = INFINITE) noexcept;
    DWORD TryLock() noexcept;
    void UnLock(int Count = 1) noexcept;
};

class CMaaDoLockUnLock
{
    CMaaWin32Lock & m_Lock;
public:
    CMaaDoLockUnLock ( CMaaWin32Lock & Lock ) noexcept
    :   m_Lock ( Lock )
    {
        m_Lock.Lock ();
    }
    ~CMaaDoLockUnLock () noexcept
    {
        m_Lock.UnLock ();
    }
};

class CMaaAppOwner
{
    BOOL m_fOwner;
    HANDLE m_OwnEvent, m_PopupEvent;
public:
    CMaaAppOwner ( const char * AppName );
    ~CMaaAppOwner ();
    BOOL IsOwner ();
    void Popup ();
    CMaaAppOwner ();
    void SetAppName ( const char * AppName );
};

class CMaaAppLocker
{
    // Mutex for the application
    int m_LockCount;
    HANDLE m_hMutex;
public:
    CMaaAppLocker(const char * AppName);
    ~CMaaAppLocker();
    bool IsLocked();
    bool Lock(DWORD TimeOut = 10000);
    void UnLock();
};

BOOL ToolsExport InstallVxD ( const char * pFullVxDFileName, BOOL fInstall = TRUE );

#define DRIVER_CtlCode( x )        CTL_CODE( FILE_DEVICE_UNKNOWN, 0x800 + ( x ), METHOD_BUFFERED, FILE_ANY_ACCESS )
#define DRIVER_GET_VERSION         DRIVER_CtlCode ( 1 )

class ToolsExport CMaaDrvApi
{
    int           m_IsLoadSuccessfully;
#ifndef __unix__
public:
    int          m_IsWinNT; // bool          m_IsWinNT;
private:
    HANDLE        m_hVxD;
    OVERLAPPED    m_ovlp; // = {0,0,0,0,0};
#endif //!__unix__

public:
    //DWORD           m_SizeRet;

    CMaaDrvApi(const char* pszFileName) noexcept; //  = "VCOMMMAA"
    CMaaDrvApi(const wchar_t* pszFileName) noexcept; //  = "VCOMMMAA"
public:
    //// Dynamically Load driver

    virtual ~CMaaDrvApi ();

#ifdef __unix__111
    /*
     BOOL myioctl ( int ioreq, void * arg);

     int CreateAdapter ( char * Name );

     int RemoveAdapter ( int Num );

     int LockDriver ( void );

     int UnLockDriver ( void );

     int RemoveModule ( void );
*/
#endif //__unix__111

    virtual BOOL IsLoadSuccessfully () const noexcept;

    /*virtual*/ BOOL VxdIoControl(DWORD dwService, const void* BuffIn, int SizeIn, void* BuffOut, int SizeOut, int* SizeRet) const noexcept;
    DWORD OpenVxDHandle ( HANDLE hWin32Source );
};

void SetThreadName(DWORD dwThreadID, const char* threadName) noexcept;

