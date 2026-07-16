
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/SysInfo.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform class for simple reference count
 * implementation.
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

//------------------------------------------------------------------------------
// CPU
//------------------------------------------------------------------------------

#ifdef _WIN32
typedef BOOL (WINAPI *LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

int MaaGetActiveProcessorCoresCount()
{
    int procCoreCount = 0;

    try
    {
        CMaaPtr_<char, 1> buffer(1024);
        static CMaaDllProc dp;
        LPFN_GLPI Glpi = (LPFN_GLPI)dp.Get(L"Kernel32.dll", "GetLogicalProcessorInformation");
        if  (!Glpi)
        {
            //_tprintf(TEXT("GetLogicalProcessorInformation is not supported.\n"));
            throw 1;
        }

        DWORD returnLength = 0;

        while(1)
        {
            returnLength = (DWORD)buffer.Size();
            if  (Glpi((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)(char *)buffer, &returnLength))
            {
                break;
            }
            if  (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                buffer.Resize((int)returnLength);
            }
            else
            {
                //_tprintf(TEXT("Error %d\n"), GetLastError());
                throw 3;
            }
        }

        DWORD byteOffset = 0;
        PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)(char *)buffer;

        while((byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION)) <= returnLength)
        {
            switch(ptr->Relationship)
            {
            case RelationProcessorCore:
                procCoreCount++;
                break;
            default:
                break;
            }
            //byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ptr++;
        }
    }
    catch(...)
    {
    }
    //_tprintf(TEXT("Number of active processor cores: %d\n"), procCoreCount);

    return procCoreCount > 0 ? procCoreCount : 0;
}
#endif

int gCpuMHz = 0;
bool gbHypervisor = false;
int CMaaGetCpuCount() noexcept
{
    static int numProcessors{0};
    if  (!numProcessors)
    {
#ifdef _WIN32
        try
        {
            numProcessors = MaaGetActiveProcessorCoresCount();
        }
        catch(...)
        {
        }
#endif
#ifdef __linux__
        CMaaString All = CMaaFile::ReadAllSysFile("/proc/cpuinfo");
        int n = 0;
        while(All.Length())
        {
            CMaaString Line = All.GetLine0();
            if  (Line.IsLeft("processor", 9))
            {
                n++;
            }
            if  (!gbHypervisor && Line.IsLeft("flags", 5))
            {
                gbHypervisor = Line.Find(" hypervisor ") >= 0 || Line.IsRight(" hypervisor", 11);
            }
            if  (!gCpuMHz && Line.IsLeft("cpu MHz", 7))
            {
                double x = 0.0;
                if (dsscanf(7 + (const char *)Line, " : %lf", &x) == 1)
                {
                    gCpuMHz = (int)(x + 0.5);
                }
            }
        }
        numProcessors = n ? n : 1;
#endif
        if  (!numProcessors)
        {
            numProcessors = 1;
        }
    }
    return numProcessors;
}

int CMaaGetHardwareConcurrency() noexcept
{
    return std::thread::hardware_concurrency();
}

#ifdef _WIN32
CMaaCpuUsage::CMaaCpuUsage(bool _100p_is_one_core) noexcept
{
    m_100p_is_one_core = _100p_is_one_core;
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    //numProcessors = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

    self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}

double CMaaCpuUsage::Get(double *pSys, double *pUsr, int *pCores) noexcept
{
    double All;

    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    if  (now.QuadPart <= lastCPU.QuadPart)
    {
        //Overflow detection. Just skip this value.
        All = -1.0;
    }
    else
    {
        double k = 100.0 / (now.QuadPart - lastCPU.QuadPart); // / c;
        if (!m_100p_is_one_core)
        {
            k /= CMaaGetCpuCount();
        }
        All = k * ((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
        if  (pSys)
        {
            *pSys = k * (sys.QuadPart - lastSysCPU.QuadPart);
        }
        if  (pUsr)
        {
            *pUsr = k * (user.QuadPart - lastUserCPU.QuadPart);
        }
        if  (pCores)
        {
            *pCores = CMaaGetCpuCount();
        }
    }
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;

    return All;
}

CMaaCpuSystemUsage::CMaaCpuSystemUsage(bool _100p_is_one_core) noexcept
{
    m_100p_is_one_core = _100p_is_one_core;
    GetSystemTimeAsFileTime((FILETIME*)&lastCPU);
    GetSystemTimes((FILETIME*)&lastIdle, (FILETIME*)&lastKernel, (FILETIME*)&lastUser);
}

double CMaaCpuSystemUsage::Get(double* pKernelOnly, double* pUsr, double* pIdle, int* pCores) noexcept
{
    double All;

    ULARGE_INTEGER now, fIdle, fKernel, fUser, fAll, lastAll;
    GetSystemTimeAsFileTime((FILETIME*)&now);
    GetSystemTimes((FILETIME*)&fIdle, (FILETIME*)&fKernel, (FILETIME*)&fUser);
    
    fAll.QuadPart = fKernel.QuadPart + fUser.QuadPart;
    lastAll.QuadPart = lastKernel.QuadPart + lastUser.QuadPart;
    
    if (now.QuadPart <= lastCPU.QuadPart)
    //if (fAll.QuadPart < lastAll.QuadPart)
    {
        //Overflow detection. Just skip this value.
        All = -1.0;
    }
    else
    {
        //int c = CMaaGetCpuCount();
        double k = 100.0 / (now.QuadPart - lastCPU.QuadPart);
        if (!m_100p_is_one_core)
        {
            k /= CMaaGetCpuCount();
        }
        if (pKernelOnly)
        {
            *pKernelOnly = k * ((fKernel.QuadPart - fIdle.QuadPart) - (lastKernel.QuadPart - lastIdle.QuadPart));// / (fAll.QuadPart - lastAll.QuadPart);
        }
        if (pUsr)
        {
            *pUsr = k * (fUser.QuadPart - lastUser.QuadPart);// / (fAll.QuadPart - lastAll.QuadPart);
        }
        if (pIdle)
        {
            *pIdle = k * (fIdle.QuadPart - lastIdle.QuadPart);// / (fAll.QuadPart - lastAll.QuadPart);
        }
        if (pCores)
        {
            *pCores = CMaaGetCpuCount();
        }
        All = k * ((fKernel.QuadPart - fIdle.QuadPart + fUser.QuadPart) - (lastKernel.QuadPart - lastIdle.QuadPart + lastUser.QuadPart));
        lastCPU = now;
        lastIdle = fIdle;
        lastKernel = fKernel;
        lastUser = fUser;
    }

    return All;
}
#endif

#ifdef __linux__
CMaaCpuUsage::CMaaCpuUsage(bool _100p_is_one_core) noexcept
{
    m_100p_is_one_core = _100p_is_one_core;
    tms timeSample;
    lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;
}

double CMaaCpuUsage::Get(double *pSys, double *pUsr, int *pCores) noexcept
{
    double All;

    tms timeSample;
    clock_t now;

    now = times(&timeSample);
    if  (now <= lastCPU || timeSample.tms_stime < lastSysCPU || timeSample.tms_utime < lastUserCPU)
    {
        //Overflow detection. Just skip this value.
        All = -1.0;
    }
    else
    {
        double k = 100.0 / (now - lastCPU);
        if (!m_100p_is_one_core)
        {
            k /= CMaaGetCpuCount();
        }
        All = k * ((timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime - lastUserCPU));
        if  (pSys)
        {
            *pSys = k * (timeSample.tms_stime - lastSysCPU);
        }
        if  (pUsr)
        {
            *pUsr = k * (timeSample.tms_utime - lastUserCPU);
        }
        if  (pCores)
        {
            *pCores = CMaaGetCpuCount();
        }
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;

    return All;
}

// /proc/stat
//cpu  29832305 556 11240914 1331584285 258337 0 2696391 599532 0 0
/*
man proc

    user   (1) Time spent in user mode.
    nice   (2) Time spent in user mode with low priority (nice).
    system (3) Time spent in system mode.
    idle   (4) Time spent in the idle task.  This value should be USER_HZ times the second entry in the /proc/uptime pseudo-file.
    iowait (since Linux 2.5.41)
        (5) Time waiting for I/O to complete.  This value is not reliable, for the following reasons:
            •  The  CPU  will not wait for I/O to complete; iowait is the time that a task is waiting for I/O to complete.  When a CPU goes into idle state for outstanding task I/O, another task will be scheduled on
           this CPU.
            •  On a multi-core CPU, the task waiting for I/O to complete is not running on any CPU, so the iowait of each CPU is difficult to calculate.
            •  The value in this field may decrease in certain conditions.
    irq (since Linux 2.6.0)
        (6) Time servicing interrupts.
    softirq (since Linux 2.6.0)
        (7) Time servicing softirqs.
    steal (since Linux 2.6.11)
        (8) Stolen time, which is the time spent in other operating systems when running in a virtualized environment
    guest (since Linux 2.6.24)
        (9) Time spent running a virtual CPU for guest operating systems under the control of the Linux kernel.
    guest_nice (since Linux 2.6.33)
        (10) Time spent running a niced guest (virtual CPU for guest operating systems under the control of the Linux kernel).
*/
CMaaCpuSystemUsage::CMaaCpuSystemUsage(bool _100p_is_one_core) noexcept
{
    m_100p_is_one_core = _100p_is_one_core;
    clk_tck = sysconf(_SC_CLK_TCK);
    memset(v, 0, sizeof(v));
    FILE* f = fopen("/proc/stat", "r");
    if  (f)
    {
        char line[512];
        line[511] = 0;
        if (fgets(line, 512, f))
        {
            dsscanf(line, -1, "cpu %D%D%D%D%D%D%D%D%D%D", v, v + 1, v + 2, v + 3, v + 4, v + 5, v + 6, v + 7, v + 8, v + 9);
        }
        fclose(f);
    }
    a = 0;
    for (int i = 0; i < 10; i++)
    {
        a += v[i];
    }

    tms timeSample;
    t = times(&timeSample);
}

double CMaaCpuSystemUsage::Get(double* pKernelOnly, double* pUsr, double* pIdle, int* pCores) noexcept
{
    double All;

    _qword aa = 0, vv[10] = {};
    FILE* f = fopen("/proc/stat", "r");
    if  (f)
    {
        char line[512];
        line[511] = 0;
        if (fgets(line, 512, f))
        {
            dsscanf(line, -1, "cpu %D%D%D%D%D%D%D%D%D%D", vv, vv + 1, vv + 2, vv + 3, vv + 4, vv + 5, vv + 6, vv + 7, vv + 8, vv + 9);
        }
        fclose(f);
    }
    for (int i = 0; i < 10; i++)
    {
        aa += vv[i];
    }

    tms timeSample;
    clock_t now;
    now = times(&timeSample);
    if  (now <= t || (aa - a) <= 0)
    {
        //Overflow detection. Just skip this value.
        All = -1.0;
    }
    else
    {
        double k = 100.0 / (now - t);
        if (!m_100p_is_one_core)
        {
            k /= CMaaGetCpuCount();
        }
        if (pKernelOnly)
        {
            *pKernelOnly = k * ((vv[2]) - (v[2]));
        }
        if (pUsr)
        {
            *pUsr = k * (vv[0] - v[0]);
        }
        if (pIdle)
        {
            *pIdle = k * (vv[3] - v[3]);
        }
        if (pCores)
        {
            *pCores = CMaaGetCpuCount();
        }
        All = k * ((aa - vv[3]) - (a - v[3]));
        t = now;
        a = aa;
        memcpy(v, vv, sizeof(v));
    }

    return All;
}
#endif

int CMaaCpuUsage::Print() noexcept
{
    double All, Sys, Usr;
    int c;
    All = Get(&Sys, &Usr, &c);
    if  (All >= 0)
    {
        return __utf8_printf("cpu usage: sys=%.1lf, usr=%.1lf, sum=%.1lf, %d cores\n", Sys, Usr, All, c);
    }
    return 0;
}

int CMaaCpuSystemUsage::Print() noexcept
{
    double All, Kernel, Usr, Idle;
    int c;
    All = Get(&Kernel, &Usr, &Idle, &c);
    if  (All >= 0)
    {
        return __utf8_printf("PC cpu usage: kern=%.1lf, usr=%.1lf, idle=%.1lf, sum=%.1lf, %d cores\n", Kernel, Usr, Idle, All, c);
    }
    return 0;
}

//------------------------------------------------------------------------------
// Memory
//------------------------------------------------------------------------------

#ifdef __unix__
#include <malloc.h>
static _qword CTOOGetHeapAllocated()
{
    struct mallinfo2 m;
    m = mallinfo2();
    return m.arena;
}
static _qword CTOOGetMaxRss()
{
    rusage ru;
    if  (!getrusage(RUSAGE_SELF, &ru))
    {
        return ru.ru_maxrss * 1024;
    }
    return 0;
}
#else
#if 0
#include <malloc.h>
static _qword CTOOGetHeapAllocated()
{
    // slow, is not thread safe
    _HEAPINFO hinfo;
    int heapstatus;
    int numLoops;
    hinfo._pentry = nullptr;
    numLoops = 0;
    _qword s = 0;
    while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK && numLoops < 0x7ffffff)
    {
        if  (hinfo._useflag == _USEDENTRY)
        {
            s += hinfo._size;
        }
        //printf("%6s block at %Fp of size %4.4X\n", (hinfo._useflag == _USEDENTRY ? "USED" : "FREE"), hinfo._pentry, hinfo._size);
        numLoops++;
    }

    /*
    switch(heapstatus)
    {
    case _HEAPEMPTY:
        printf("OK - empty heap\n");
        break;
    case _HEAPEND:
        printf("OK - end of heap\n");
        break;
    case _HEAPBADPTR:
        printf("ERROR - bad pointer to heap\n");
        break;
    case _HEAPBADBEGIN:
        printf("ERROR - bad start of heap\n");
        break;
    case _HEAPBADNODE:
        printf("ERROR - bad node in heap\n");
        break;
    }
    */
    return s;
}
#if 0
#include <psapi.h>
#pragma comment(lib,"Kernel32.lib")
#pragma comment(lib,"Psapi.lib")
static _qword CTOOGetPagefileUsage()
{
    PROCESS_MEMORY_COUNTERS x;
    memset(&x, 0, sizeof(x));
    x.cb = sizeof(x);
    GetProcessMemoryInfo(GetCurrentProcess(), &x, sizeof(x));
    return x.PagefileUsage;
}
#endif
#endif
#endif

#ifdef _WIN32
typedef BOOL (WINAPI * My_t_GetProcessMemoryInfo)(
    HANDLE Process,
    PPROCESS_MEMORY_COUNTERS ppsmemCounters,
    DWORD cb
);
#endif

_qword MaaGetMemoryUsage(_qword* pPeak) noexcept
{
    _qword x = -1;
    if  (pPeak)
    {
        *pPeak = -1;
    }
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    memset(&pmc, 0, sizeof(pmc));
    pmc.cb = sizeof(pmc);
    static CMaaDllProc dp1, dp2;
    My_t_GetProcessMemoryInfo My_GetProcessMemoryInfo = (My_t_GetProcessMemoryInfo)dp1.Get(L"Kernel32.dll", "GetProcessMemoryInfo");
    My_GetProcessMemoryInfo = My_GetProcessMemoryInfo ? My_GetProcessMemoryInfo : (My_t_GetProcessMemoryInfo)dp2.Get(L"Psapi.dll", "GetProcessMemoryInfo");
    if  (My_GetProcessMemoryInfo)
    {
        My_GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        x = pmc.WorkingSetSize;
        if  (pPeak)
        {
            *pPeak = pmc.PeakWorkingSetSize;
        }
    }
#endif
#ifdef __linux__
#if 1
    FILE* f = fopen("/proc/self/status", "r");
    if  (f)
    {
        char line[132];
        line[128] = 0;
        int flag = 0;
        while(fgets(line, 128, f) && flag != 3)
        {
            if  (CMaaIsMemEq(line, "VmSize:", 7))
            {
                // "VmSize:\t   19896 kB"
                flag |= 1;
                char* p = line + 7;
                while(*p != 0 && (*p < '0' || *p > '9')) p++;
                int pos = 0;
                if  (mysscanf64(p, &x, -1, &pos, " \t"))
                {
                    p += pos;
                    while(*p == ' ' || *p == '\t') p++;
                    if  (CMaaIsMemEq(p, "kB", 2))
                    {
                        x *= 1024;
                    }
                }
            }
            else if (CMaaIsMemEq(line, "VmPeak:", 7))
            {
                // "VmPeak:\t   19896 kB"
                flag |= 2;
                if  (pPeak)
                {
                    char* p = line + 7;
                    while(*p != 0 && (*p < '0' || *p > '9')) p++;
                    int pos = 0;
                    if  (mysscanf64(p, pPeak, -1, &pos, " \t"))
                    {
                        p += pos;
                        while(*p == ' ' || *p == '\t') p++;
                        if  (CMaaIsMemEq(p, "kB", 2))
                        {
                            (*pPeak) *= 1024;
                        }
                    }
                }
            }
        }
        fclose(f);
    }
#else
    CMaaString All = CMaaFile::ReadAllSysFile("/proc/self/status");

    int flag = 0;
    while(All.IsNotEmpty() && flag != 3)
    {
        CMaaString Line = All.GetLine0();
        if  (Line.IsLeft("VmSize:", 7))
        {
            // "VmSize:\t   19896 kB"
            flag |= 1;
            const char * p = Line;
            const char * e = p + Line.Length();
            p += 7;
            while(p < e && (*p < '0' || *p > '9')) p++;
            int pos = 0;
            if  (mysscanf64(p, &x, -1, &pos, " \t\r\n"))
            {
                p += pos;
                while(*p == ' ' || *p == '\t') p++;
                if  (CMaaIsMemEq(p, "kB", 2))
                {
                    x *= 1024;
                }
            }
        }
        else if (Line.IsLeft("VmPeak:", 7))
        {
            // "VmPeak:\t   19896 kB"
            flag |= 2;
            if  (pPeak)
            {
                const char * p = Line;
                const char * e = p + Line.Length();
                p += 7;
                while(p < e && (*p < '0' || *p > '9')) p++;
                int pos = 0;
                if  (mysscanf64(p, pPeak, -1, &pos, " \t\r\n"))
                {
                    p += pos;
                    while(*p == ' ' || *p == '\t') p++;
                    if  (CMaaIsMemEq(p, "kB", 2))
                    {
                        (*pPeak) *= 1024;
                    }
                }
            }
        }
    }
#endif
#endif
#if 0 // MAC
#include<mach/mach.h>

    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if  (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count) == KERN_SUCCESS)
    {
        // resident size is in t_info.resident_size;
        // virtual size is in t_info.virtual_size;
        x = t_info.virtual_size;
    }
#endif
    return x;
}


_qword CTOOGetMemoryAllocated()
{
    //return MaaGetMemoryUsage();
#ifdef __unix__
    return CTOOGetHeapAllocated();
    //return CTOOGetMaxRss();
#else
    return 0;
    //return CTOOGetHeapAllocated(); // slow, is not thread safe

    //return CTOOGetPagefileUsage();
#endif
}
//------------------------------------------------------------------------------

unsigned char* fswap_bits(const unsigned char* m, int N) // N bits
{
    constexpr int qw_bits = sizeof(_uqword) * 8;
    const int b = (N + 7) / 8;
    unsigned char* r = new unsigned char[b];
    if (r && N)
    {
        const sFswapBitsMap& Map = gfnFswapBitsMap();
        const int Shift = (8 - N % 8) & 7;
        if (!Shift)
        {
            for (int i = 0; i < b; i++)
            {
                r[i] = Map.m[m[b - 1 - i]];
            }
        }
        else
        {
            const int Shift2 = 8 - Shift;
            unsigned char y = Map.m[m[b - 1]] >> Shift, z;
            int i;
            for (i = 0; i < b - 1; i++)
            {
                r[i] = y | ((z = Map.m[m[b - 2 - i]]) << Shift2);
                y = z >> Shift;
            }
            r[i] = y;
        }
    }
    return r;
}

void fswap_bits1(unsigned char* m, int N) noexcept // N bits
{
    if (m && N)
    {
        const sFswapBitsMap& Map = gfnFswapBitsMap();
        const int b = (N + 7) / 8;
        const int b2 = b / 2;
        for (int i = 0; i < b2; i++)
        {
            unsigned char mm = Map.m[m[i]];
            m[i] = Map.m[m[b - 1 - i]];
            m[b - 1 - i] = mm;
        }
        if ((b & 1))
        {
            m[b2] = Map.m[m[b2]];
        }
        const int Shift = (8 - N % 8) & 7;
        if (Shift)
        {
            const int Shift2 = 8 - Shift;
            unsigned char y = m[0] >> Shift, z;
            int i;
            for (i = 0; i < b - 1; i++)
            {
                m[i] = y | ((z = m[i + 1]) << Shift2);
                y = z >> Shift;
            }
            m[i] = y;
        }
    }
}

void fprint_bits(const unsigned char* m, int N) noexcept
{
    for (int i = 0; i < N; i++)
    {
        __utf8_printf("%c", m[i / 8] & (1 << (i % 8)) ? '1' : '0');
    }
    __utf8_printf("\n");
}

void fprint_bits2(const unsigned char* m, int N)
{
    CMaaString s = BitsToString(m, N);
    __utf8_printf("%S\n", &s);
#if 0 // check
    CMaaPtr_<unsigned char, 1> r((N + 7) / 8);
    if (StringToBits(s, r) != N)
    {
        __utf8_printf("StringToBits() ret error\n");
        return;
    }
    if (memcmp(m, r, N / 8))
    {
        __utf8_printf("StringToBits() gen error 1\n");
        return;
    }
    if ((N % 8) && (m[N / 8] & ~(-1 << (N % 8))) != r[N / 8])
    {
        __utf8_printf("StringToBits() gen error 2\n");
        return;
    }
#endif
}

CMaaString BitsToString(const unsigned char* m, int N)
{
    CMaaString s(nullptr, N, CMaaString::eNotInitMem);
    if (s.IsNotEmpty())
    {
        char* p = s.GetBuffer();
        for (int i = 0; i < N; i++)
        {
            *p++ = m[i / 8] & (1 << (i % 8)) ? '1' : '0';
        }
    }
    return s;
}

int StringToBits(const CMaaString &s, unsigned char* m) noexcept
{
    const int N = s.Length();
    const char* p = s;
    const int N0 = N / 8;
    int i;
    for (i = 0; i < N0; i++)
    {
        unsigned char c = 0;
        for (int j = 0; j < 8; j++)
        {
            char b = *p++;
            if (b == '1')
            {
                c |= (1 << j);
            }
            else if (b != '0')
            {
                return -1;
            }
        }
        *m++ = c;
    }
    if ((i = N % 8))
    {
        unsigned char c = 0;
        for (int j = 0; j < i; j++)
        {
            char b = *p++;
            if (b == '1')
            {
                c |= (1 << j);
            }
            else if (b != '0')
            {
                return -1;
            }
        }
        *m++ = c;
    }
    return N;
}
