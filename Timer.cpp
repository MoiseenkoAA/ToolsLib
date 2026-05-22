
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Timer.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform high resolution time calculations.
 * First dated 30.10.2009.
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

#ifdef __linux__
#include <stdbool.h>
#include <stdint.h>

#include <sys/mman.h>
#include <linux/perf_event.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#ifdef _WIN32
ULONG MaaAdd128(ULARGE_INTEGER *x, const ULARGE_INTEGER * y) noexcept
{
    ULARGE_INTEGER tmp;
    tmp.QuadPart = (unsigned long long)x[0].LowPart + (unsigned long long)y[0].LowPart;
    x[0].LowPart = tmp.LowPart;
    tmp.LowPart = tmp.HighPart;
    tmp.HighPart = 0;

    tmp.QuadPart += (unsigned long long)x[0].HighPart + (unsigned long long)y[0].HighPart;
    x[0].HighPart = tmp.LowPart;
    tmp.LowPart = tmp.HighPart;
    tmp.HighPart = 0;

    tmp.QuadPart += (unsigned long long)x[1].LowPart + (unsigned long long)y[1].LowPart;
    x[1].LowPart = tmp.LowPart;
    tmp.LowPart = tmp.HighPart;
    tmp.HighPart = 0;

    tmp.QuadPart += (unsigned long long)x[1].HighPart + (unsigned long long)y[1].HighPart;
    x[1].HighPart = tmp.LowPart;
    tmp.LowPart = tmp.HighPart;
    tmp.HighPart = 0;

    return tmp.LowPart;
}
void MaaMul64x64(const ULARGE_INTEGER &xx, const ULARGE_INTEGER &yy, ULARGE_INTEGER *res) noexcept
{
#ifdef _AMD64_
    res[0].QuadPart = _umul128(xx.QuadPart, yy.QuadPart, &res[1].QuadPart);
#else
    ULARGE_INTEGER nn[4][2];

    //(10a + b)*(10c + d) = (100ac + 10*(bc + ad) + bd);
    nn[0][0].QuadPart = 0;
    nn[0][1].QuadPart = (unsigned long long)xx.HighPart * (unsigned long long)yy.HighPart;

    nn[1][0].QuadPart = (unsigned long long)xx.LowPart * (unsigned long long)yy.LowPart;
    nn[1][1].QuadPart = 0;

    nn[2][0].QuadPart = (unsigned long long)xx.LowPart * (unsigned long long)yy.HighPart;
    nn[2][1].LowPart = nn[2][0].HighPart;
    nn[2][1].HighPart = 0;
    nn[2][0].HighPart = nn[2][0].LowPart;
    nn[2][0].LowPart = 0;

    nn[3][0].QuadPart = (unsigned long long)xx.HighPart * (unsigned long long)yy.LowPart;
    nn[3][1].LowPart = nn[3][0].HighPart;
    nn[3][1].HighPart = 0;
    nn[3][0].HighPart = nn[3][0].LowPart;
    nn[3][0].LowPart = 0;

    MaaAdd128(nn[0], nn[1]);
    MaaAdd128(nn[0], nn[2]);
    MaaAdd128(nn[0], nn[3]);
    res[0].QuadPart = nn[0][0].QuadPart;
    res[1].QuadPart = nn[0][1].QuadPart;
#endif
}
#endif

#if defined(_WIN32) || defined(__linux__) // linux can be commented because web app are delayed by 0.1 ms to calibrate
CMaaTime::CMaaTime(CMaaTime &That) noexcept
{
    m_LastTimeFactor = 0;
    m_LastUsTime = 0;
    m_Freq = That.m_Freq;
    m_Accuraciry = That.m_Accuraciry;
    m_IAccuraciry = That.m_IAccuraciry;
    //m_NextTime__rdtsc = That.m_NextTime__rdtsc;

    m_StartTime__rdtsc = m_LastTime__rdtsc = GetCounter();
    //StartWatch();
}

#ifdef __linux__
//#include <datapage_.h>
//typedef vdso_clock vdso_data;

#if 0
struct arch_vdso_data {};

struct vdso_timestamp {
    __u64       sec;
    __u64       nsec;
};

#define VDSO_BASES      (CLOCK_TAI + 1)
struct vdso_data {
    __u32                       seq;

    __s32                       clock_mode;
    __u64                       cycle_last;
    __u64                       mask;
    __u32                       mult;
    __u32                       shift;

    union {
        struct vdso_timestamp   basetime[VDSO_BASES];
        //struct timens_offset  offset[VDSO_BASES];
    };

    __s32                       tz_minuteswest;
    __s32                       tz_dsttime;
    __u32                       hrtimer_res;
    __u32                       __unused;

    struct arch_vdso_data       arch_data;
};
#else
extern int gCpuMHz;
extern bool gbHypervisor;
struct vdso_data_start_t
{
    uint32_t seq;
    int32_t clock_mode;
    uint64_t cycle_last;
    uint64_t mask;
    uint32_t mult;
    uint32_t shift;
};
typedef vdso_data_start_t vdso_data;
#endif

#endif

CMaaTime::CMaaTime() noexcept
{
    CMaaGetCpuCount(); // for KVM guest calibrating fix under linux
#ifdef _WIN32
    constexpr int CalibrateMs = 1; // 2;

    m_LastTimeFactor = 0;
#ifndef _IA64_
    LARGE_INTEGER tmp, tmp2, tmpf;
#if defined (_M_ARM) || defined (_M_ARM64)
    m_StartTime__rdtsc = m_LastTime__rdtsc = /*m_NextTime__rdtsc =*/ GetCounter();
#else
    m_StartTime__rdtsc = m_LastTime__rdtsc = /*m_NextTime__rdtsc =*/ __rdtsc();
#endif
    m_LastUsTime = 0;
    m_Accuraciry = 1;

#ifdef __KERNEL
    m_IAccuraciry = KeQueryTimeIncrement();
#else
    m_IAccuraciry = 1;
#endif

#ifdef __KERNEL
    KeQueryPerformanceCounter(&tmpf);
    tmp.QuadPart = KeQueryPerformanceCounter(nullptr).QuadPart;
    _uqword t = __rdtsc();
    NdisMSleep(CalibrateMs * 1000);
    tmp2.QuadPart = KeQueryPerformanceCounter(nullptr).QuadPart;
    _uqword t2 = __rdtsc();
    m_Freq = MaaMulDiv64(t2 - t, tmpf.QuadPart, tmp2.QuadPart - tmp.QuadPart);
#else
    m_Freq = 0;

#ifdef _WIN64
    // Fast path: Load kernel-mapped memory page
    HMODULE ntdll = LoadLibrary(L"ntdll.dll");
    if (ntdll)
    {
        int (*NtQuerySystemInformation)(int, void*, unsigned int, unsigned int*) =
            (int (*)(int, void*, unsigned int, unsigned int*))GetProcAddress(ntdll, "NtQuerySystemInformation");
        if (NtQuerySystemInformation)
        {
            volatile _uqword* hypervisor_shared_page = NULL;
            unsigned int size = 0;

            // SystemHypervisorSharedPageInformation == 0xc5
            const int result = (NtQuerySystemInformation)(0xc5/*SystemHypervisorSharedPageInformation*/, (void*)&hypervisor_shared_page, sizeof(hypervisor_shared_page), &size);

            // success
            if (size == sizeof(hypervisor_shared_page) && result >= 0 && hypervisor_shared_page && hypervisor_shared_page[1])
            {
                // docs say ReferenceTime = ((VirtualTsc * TscScale) >> 64)
                //      set ReferenceTime = 10000000 = 1 second @ 10MHz, solve for VirtualTsc
                //       =>    VirtualTsc = 10000000 / (TscScale >> 64)
                //m_Freq = (10000000ull << 32) / (hypervisor_shared_page[1] >> 32);
                // If your build configuration supports 128 bit arithmetic, do this:
                // m_Freq = ((unsigned __int128)10000000ull << (unsigned __int128)64ull) / hypervisor_shared_page[1];
                m_Freq = MaaMulDiv64(160000000ull, 0x1000000000000000ull, hypervisor_shared_page[1]);
            }
        }
        FreeLibrary(ntdll);
    }
#endif
    if (!m_Freq) // Slow path
    {
        QueryPerformanceFrequency(&tmpf);
        QueryPerformanceCounter(&tmp);
        const _uqword t = GetCounter();  // __rdtsc();
        Sleep(CalibrateMs);
        QueryPerformanceCounter(&tmp2);
        const _uqword t2 = GetCounter();  // __rdtsc();
        m_Freq = MaaMulDiv64(t2 - t, tmpf.QuadPart, tmp2.QuadPart - tmp.QuadPart);
    }
#endif

#if defined (_M_ARM) || defined (_M_ARM64)
    m_StartTime__rdtsc = m_LastTime__rdtsc = GetCounter();
#else
    m_StartTime__rdtsc = m_LastTime__rdtsc = __rdtsc();
#endif
    m_LastUsTime = 0;

#else
    // _IA64_

#ifdef __KERNEL
    LARGE_INTEGER tmpf;
    KeQueryPerformanceCounter(&tmpf);
    m_Freq = tmpf.QuadPart;
    m_Accuraciry = m_IAccuraciry = KeQueryTimeIncrement();
    m_StartTime__rdtsc = m_LastTime__rdtsc = KeQueryInterruptTime();
    m_LastUsTime = 0;
#else
    // not implemented
#endif
#endif

#endif

#ifdef __linux__

    constexpr int CalibrateUs = 100;

    m_LastTimeFactor = 0;

    m_LastUsTime = 0;
    m_Accuraciry = 1;

    m_IAccuraciry = 1;

    m_Freq = 0;

#if 1 // need divide by cpu frequency in GHz: 2,600,309 result of 1 sec on 2.6 GHz cpu

    // Cache the answer so that multiple calls never take the slow path more than once
    //uint64_t tsc_freq = 0;

    //just the start of the vdso_data structure (since thats all we need)
    // Fast path: Get the frequency through vdso. This assumes a concrete layout of the vdso data
    // in the linux kernel. This is complete hack using undocumented internal api and is likely to break in the future.
    // Still it is a best guess we have on linux (see below for the other method and its problems).
    // It was reconstructed from the clock_gettime implementation and various other things
    char* vvar_addr = nullptr;
    {
        // quickly parse /proc/self/maps to find [vvar] mapping
        char mmaps[4096 * 4];
        memset(mmaps, 0, sizeof(mmaps));
        FILE* mmapsfile = fopen("/proc/self/maps", "r");
        if (!mmapsfile)
        {
            //fprintf(stderr, "query_rdtsc_freq(): could not access own maps\n");
        }
        else
        {
            size_t nread = fread(mmaps, 1, sizeof(mmaps) - 1, mmapsfile);
            fclose(mmapsfile);
            if (nread > 0)
            {
                char* nxt;
                for (char* line = mmaps; line; line = nxt)
                {
                    nxt = strchr(line, '\n');
                    if (nxt)
                    {
                        *nxt++ = 0;
                    }
                    if (strstr(line, "[vvar]"))
                    {
                        //vvar_addr = (char*)(void*)strtoull(line, NULL, 16);
                        dsscanf(line, "%p", &vvar_addr);
                        break;
                    }
                }
            }

            if (!vvar_addr)
            {
                //fprintf(stderr, "query_rdtsc_freq(): could not find [vvar] mapping\n");
            }
        }
    }

    if (vvar_addr)
    {
        //+ 128 because thats where the vdso_data starts
        vdso_data *vd = (vdso_data *)(vvar_addr + 128);
//printf("seq=%lld, clock_mode=%lld, cycle_last=%lld, mask=%lld, mult=%lld, shift=%lld\n", (_qword)vd->seq, (_qword)vd->clock_mode, (_qword)vd->cycle_last, (_qword)vd->mask, (_qword)vd->mult, (_qword)vd->shift);
//printf("vd->shift=%d, vd->mult=%ld, gbHypervisor=%d, gCpuMHz=%d\n", (int)vd->shift, (long)vd->mult, gbHypervisor, (int)gCpuMHz);
        if (vd->shift < 64 && vd->mult != 0
#ifdef TL_KVM
         && gCpuMHz
#else
        && (!gbHypervisor || gCpuMHz)
#endif
         )
        {
//printf("gCpuMHz = %d\n", gCpuMHz);
//printf("seq=%lld, clock_mode=%lld, cycle_last=%lld, mask=%lld, mult=%lld, shift=%lld\n",
// (_qword)vd->seq, (_qword)vd->clock_mode, (_qword)vd->cycle_last, (_qword)vd->mask, (_qword)vd->mult, (_qword)vd->shift);
            //m_Freq = (uint64_t)(((__uint128_t)1000000000ull << vdso_data->shift) / vdso_data->mult);
            m_Freq = MaaMulDiv64(1000000000ull, 1ull << vd->shift, vd->mult);

#ifdef TL_KVM
            // KVM guest calibrating fix
            m_Freq = MaaMulDiv64(m_Freq, gCpuMHz, 1000); // x2.6 on vds. do not on home pc
#else
            // KVM guest calibrating fix
            if (gbHypervisor)
            {
                m_Freq = MaaMulDiv64(m_Freq, gCpuMHz, 1000); // x2.6 on vds. do not on home pc
            }
#endif
        }

        //its more probable this method failed then your cpu being this slow
        if (m_Freq < 20000)
        {
            m_Freq = 0;
        }
        //printf("1st method m_Freq=%lld\n", m_Freq);
//else printf("1st method m_Freq=%lld\n", m_Freq);
    }
#endif // need divide by cpu frequency in GHz: 2,600,309 result of 1 sec on 2.6 GHz cpu

    if (!m_Freq)
    {
        // Fast path: Load kernel-mapped memory page
        struct perf_event_attr pe;
        memset(&pe, 0, sizeof(pe));
        pe.type = PERF_TYPE_HARDWARE;
        pe.size = sizeof(struct perf_event_attr);
        pe.config = PERF_COUNT_HW_INSTRUCTIONS;
        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        // __NR_perf_event_open == 298 (on x86_64)
        int fd = syscall(/*298 or __NR_perf_event_open or */ SYS_perf_event_open, &pe, 0, -1, -1, 0);
        if (fd != -1)
        {
            struct perf_event_mmap_page* pc = (struct perf_event_mmap_page*)mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
            if (pc)
            {
                // success
    //printf("pc->version=%d, pc->compat_version=%d, pc->cap_user_time=%d\n", (int)pc->version, (int)pc->compat_version, (int)pc->cap_user_time);
    //printf("%lld * (1ull << (%lld - 1)) / %lld\n", 2000000000ull, pc->time_shift, (_uqword)pc->time_mult);
                if (pc->cap_user_time == 1)
                {
                    // docs say nanoseconds = (tsc * time_mult) >> time_shift
                    //      set nanoseconds = 1000000000 = 1 second in nanoseconds, solve for tsc
                    //       =>         tsc = 1000000000 / (time_mult >> time_shift)
                    //m_Freq = (1000000000ull << (pc->time_shift / 2)) / (pc->time_mult >> (pc->time_shift - pc->time_shift / 2));
                    // If your build configuration supports 128 bit arithmetic, do this:
                    // tsc_freq = ((__uint128_t)1000000000ull << (__uint128_t)pc->time_shift) / pc->time_mult;

                    //printf("5 %lld * %lld / %lld\n", 2000000000ull, 1ull << (pc->time_shift - 1), (_uqword)pc->time_mult);
                    m_Freq = MaaMulDiv64(2000000000ull, 1ull << (pc->time_shift - 1), pc->time_mult);
                }
                munmap(pc, 4096);
            }
            close(fd);
        }
    }

    // Slow path
    if (!m_Freq)
    {
        // Get time before sleep
        _uqword nsc_begin = 0;
        {
            struct timespec t;
            if (!clock_gettime(CLOCK_MONOTONIC_RAW, &t))
            {
                nsc_begin = (_uqword)t.tv_sec * 1000000000 + t.tv_nsec;
            }
        }
        _uqword tsc_begin = __rdtsc();

        usleep(CalibrateUs); // 10ms(10000us) gives ~4.5 digits of precision - the longer you sleep, the more precise you get

        // Get time after sleep
        _uqword nsc_end = nsc_begin + 1;
        {
            struct timespec t;
            if (!clock_gettime(CLOCK_MONOTONIC_RAW, &t))
            {
                nsc_end = (_uqword)t.tv_sec * 1000000000 + t.tv_nsec;
            }
        }
        _uqword tsc_end = __rdtsc();

        // Do the math to extrapolate the RDTSC ticks elapsed in 1 second
        //tsc_freq = (tsc_end - tsc_begin) * 1000000000 / (nsc_end - nsc_begin);

        //printf("12 %lld * %lld / %lld\n", tsc_end - tsc_begin, 1000000000, nsc_end - nsc_begin);
        m_Freq = MaaMulDiv64(tsc_end - tsc_begin, 1000000000, nsc_end - nsc_begin);
    }

    //printf("13 %lld\n", m_Freq);
    //__utf8_printf("13 %,D\n", m_Freq);
    m_StartTime__rdtsc = m_LastTime__rdtsc = __rdtsc();
    m_LastUsTime = 0;

#endif

#if defined(__APPLE__) && defined(__MACH__)

    // TODO...
    m_LastTimeFactor = 0;
    m_StartTime__rdtsc = m_LastTime__rdtsc = /*m_NextTime__rdtsc =*/ GetCounter();
    m_LastUsTime = 0;
    m_Accuraciry = 1;

    m_IAccuraciry = 1;

    m_Freq = 0;

    _uqword /*uint64_t*/ freq = 1000000000;
    size_t size = sizeof(freq);
    sysctlbyname("machdep.tsc.frequency", &freq, &size, NULL, 0);
    m_Freq = freq;
#endif

#if !defined(_WIN32) && !defined(__linux__) && !defined(_WIN32) && !defined(__APPLE__) && !defined(__MACH__)
#error "unsupported platform!"
#endif
}

_uqword CMaaTime::GetTime(_uqword Factor) noexcept
{
    /*
    std::atomic<int> x{0};
    int x0 = x;
    while (!x.compare_exchange_strong(x0, x0 + 1 TOOLSLIB_CE_WS_MO)) {}
    */

    _uqword x;
    while(1)
    {
        x = GetCounter();
        _uqword y = m_LastTime__rdtsc.load(std::TL_memory_order_acquire);
        if  ((_qword)(x - y) < 0)
        {
            x = y;
            break;
        }
        else
        {
            if  (m_LastTime__rdtsc.compare_exchange_weak(y, x, std::TL_memory_order_acq_rel, std::memory_order_relaxed))
            {
                break;
            }
        }
    }
#if defined(__KERNEL) && defined(_IA64_) && defined(_WIN32)
    x = MaaMulDiv64(x - m_StartTime__rdtsc, Factor, 10000000);
#else
    x = MaaMulDiv64(x - m_StartTime__rdtsc, Factor, m_Freq);
#endif

    m_LastUsTime.store(x, std::TL_memory_order_release);
    return x;
}
_uqword CMaaTime::GetDeltaTime(_uqword t, _uqword Factor) const noexcept
{
#if defined(__KERNEL) && defined(_IA64_) && defined(_WIN32)
    return MaaMulDiv64(t, 10000000, Factor);
#else
    return MaaMulDiv64(t, m_Freq, Factor);
#endif
}

_uqword CMaaTime::StartWatch() noexcept
{
    const _uqword x = GetCounter();
    m_LastTimeFactor = 0;
    if  ((_qword)(x - m_LastTime__rdtsc) >= 0)
    {
        m_StartTime__rdtsc = m_LastTime__rdtsc = x;
    }
    else
    {
        m_StartTime__rdtsc = m_LastTime__rdtsc.load(std::TL_memory_order_acquire);
    }
    m_LastUsTime = 0;
    return 0;
}

_uqword CMaaTime::GetTimeEx(_uqword Factor) noexcept
{
    // more fast call by using time caching
    if  (Factor != m_LastTimeFactor || (_qword)(GetCounter() - m_NextTime__rdtsc) >= 0)
    {
        const _uqword LastTime = GetTime(m_LastTimeFactor = Factor);
        m_LastTime = LastTime;
#if defined(__KERNEL) && defined(_IA64_) && defined(_WIN32)
        m_NextTime__rdtsc = m_StartTime__rdtsc + MaaMulDiv64(LastTime + 1, 10000000, Factor);
#else
        m_NextTime__rdtsc = m_StartTime__rdtsc + MaaMulDiv64(LastTime + 1, m_Freq, Factor);
#endif
        return LastTime;
    }
    return m_LastTime;
}


#else
//unix
CMaaTime::CMaaTime(int /*CalibrateMs*/) noexcept
{
    StartWatch();
}
CMaaTime::CMaaTime(CMaaTime & /*That*/) noexcept
{
    StartWatch();
}
_uqword CMaaTime::StartWatch() noexcept
{
    gettimeofday(&m_StartTime, nullptr);
    return 0;
}
_uqword CMaaTime::GetTime(_uqword Factor) noexcept
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    //int s1 = (tv.tv_usec - m_StartTime.tv_usec) / 1000000 - 1;
    //_qword x = (_qword)(tv.tv_sec - m_StartTime.tv_sec + s1) * Factor + (_qword)(tv.tv_usec - m_StartTime.tv_usec - s1 * 1000000) * Factor / 1000000;
    const _uqword x = (_uqword)(tv.tv_sec - m_StartTime.tv_sec - 1) * Factor + (_uqword)(1000000 + tv.tv_usec - m_StartTime.tv_usec) * Factor / 1000000;
    return x;
}
_uqword CMaaTime::GetDeltaTime(_uqword t, _uqword Factor) const noexcept
{
#ifdef MaaMulDiv64
    return MaaMulDiv64(t, 1000000, Factor);
#else
    return t * (1000000 / Factor);
#endif
}
#endif // _WIN32

_uqword CMaaTimeStamp() noexcept // time(nullptr) * 1000000 + usec
{
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, nullptr);
    return (_uqword)tv.tv_sec * 1000000 + tv.tv_usec;
#else
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    time_t t;
    int usec;
    FileTimeToTimetAndUsec(&ft, &t, &usec);
    return (_uqword)t * 1000000 + usec;
#endif
}

time_t CMaaTimeStampToTime(_uqword ts, int * pusec) noexcept
{
    if  (pusec)
    {
        *pusec = ts % 1000000;
    }
    return (time_t) ts / 1000000;
}

/*
template <class Key> _dword CMaaHashFunction(const Key & k)
{
     return CMaaStandardHashFunction(&k, (int)sizeof(k));
}
template <class Key> _uqword CMaaHashFunction64(const Key & k)
{
     return CMaaStandardHashFunction64(&k, (int)sizeof(k));
}
*/
template <> _dword CMaaHashFunction<short>(const short& k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction64<short>(const short& k) noexcept
{
    return (_dword)k;
}
//#ifndef __unix__
template <> _dword CMaaHashFunction<_word>(const _word& k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction64<_word>(const _word& k) noexcept
{
    return (_dword)k;
}
//#endif
template <> _dword CMaaHashFunction<int>(const int& k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction64<int>(const int& k) noexcept
{
    return (_dword)k;
}
#ifndef __unix__
template <> _dword CMaaHashFunction<_dword>(const _dword& k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction64<_dword>(const _dword& k) noexcept
{
    return (_dword)k;
}
#endif
template <> _dword CMaaHashFunction<unsigned>(const unsigned& k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction64<unsigned>(const unsigned& k) noexcept
{
    return (_dword)k;
}

template <> _dword CMaaHashFunction2<int>(const int k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction642<int>(const int k) noexcept
{
    return (_dword)k;
}
#ifndef __unix__
template <> _dword CMaaHashFunction2<_dword>(const _dword k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction642<_dword>(const _dword k) noexcept
{
    return (_dword)k;
}
#endif
template <> _dword CMaaHashFunction2<unsigned>(const unsigned k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction642<unsigned>(const unsigned k) noexcept
{
    return (_dword)k;
}
template <> _dword CMaaHashFunction2<_word>(const _word k) noexcept
{
    return (_dword)k;
}
template <> _uqword CMaaHashFunction642<_word>(const _word k) noexcept
{
    return (_dword)k;
}
template <> _dword CMaaHashFunction2<_qword>(const _qword k) noexcept
{
    return ((_dword)k) ^ (_dword)(k >> 32);
}
template <> _uqword CMaaHashFunction642<_qword>(const _qword k) noexcept
{
    return (_uqword)k;
}
template <> _dword CMaaHashFunction2<_uqword>(const _uqword k) noexcept
{
    return ((_dword)k) ^ (_dword)(k >> 32);
}
template <> _uqword CMaaHashFunction642<_uqword>(const _uqword k) noexcept
{
    return (_uqword)k;
}

time_t CMaaGetTimeOfDay(int* pMicroseconds) noexcept
{
    if  (!pMicroseconds)
    {
        return time(NULL);
    }
#ifdef _WIN32
    time_t t;
    while(1)
    {
        SYSTEMTIME st1, st2;
        memset(&st1, 0, sizeof(st1));
        memset(&st2, 0, sizeof(st2));
        t = time(nullptr);
        GetSystemTime(&st1);
        const time_t t2 = time(nullptr);
        GetSystemTime(&st2);
        if  (t2 == t && st2.wMilliseconds >= st1.wMilliseconds)
        {
            *pMicroseconds = st2.wMilliseconds * 1000;
            break;
        }
    }
    return t;
    /*
    SYSTEMTIME Time;
    memset(&Time, 0, sizeof(Time));
    tm tt;
    memset(&tt, 0, sizeof(tt));
    GetLocalTime(&Time);
    *pMilliseconds = Time.wMilliseconds;
    tt.tm_hour = Time.wHour;
    tt.tm_min = Time.wMinute;
    tt.tm_sec = Time.wSecond;
    tt.tm_mday = Time.wDay;
    tt.tm_mon = Time.wMonth - 1;
    tt.tm_year = Time.wYear - 1900;
    tt.tm_isdst = -1;
    return mktime(&tt);
    */
#else
    timeval tv;
    gettimeofday(&tv, nullptr);
    *pMicroseconds = tv.tv_usec;
    return tv.tv_sec;
#endif
}

/*
int main()
{
     LARGE_INTEGER tmp;
     QueryPerformanceFrequency(&tmp);
     _uqword q = tmp.QuadPart;
     __utf8_printf("q = %,D (0x%08x%08x)\n", q, (_dword)(q >> 32), (_dword)(q & 0xffffffff));
     return 0;
}
// C2Q 9550 2.83 GHz :
q = 2,764,179 (0x00000000002a2d93)
*/

/*
#ifdef _WIN32
int test_delta()
{
    CMaaTime& h = GetHRTime();
    DWORD t0 = GetTickCount();
    _uqword x00;
    //_uqword x0 = (x00 = h.GetCounter()) + h.GetDeltaTime(5000000, 1000000);
    //_uqword x0 = (x00 = h.GetCounter()) + h.GetDeltaTime(5000, 1000);
    _uqword x0 = (x00 = h.GetCounter()) + h.GetDeltaTime(5, 1);
    __utf8_printf("x00=%,D, x0=%,D, x0 - x00=%,D\n", (_uqword)x00, (_uqword)x0, (_uqword)(x0 - x00));
    while (h.GetCounter() < x0);
    DWORD t = GetTickCount();
    __utf8_printf("t - t0 = 5s = %d ms\n", (int)(t - t0));
    return 0;
}
#else
int test_delta()
{
    CMaaTime& h = GetHRTime();
    DWORD t0 = GetTickCount();
    _uqword x00;
    //_uqword x0 = (x00 = h.GetCounter()) + h.GetDeltaTime(5000000, 1000000);
    //_uqword x0 = (x00 = h.GetCounter()) + h.GetDeltaTime(5000, 1000);
    _uqword x0 = (x00 = h.GetCounter()) + h.GetDeltaTime(5, 1);
    __utf8_printf("x00=%,D, x0=%,D, x0 - x00=%,D\n", x00, x0, x0 - x00);
    while (h.GetCounter() < x0);
    DWORD t = GetTickCount();
    __utf8_printf("t - t0 = 5s = %d ms\n", (int)(t - t0));
    return 0;
}
#endif
*/

#ifdef _WIN32
#ifdef MaaMulDiv64
#undef MaaMulDiv64
#endif
_uqword MaaMulDiv64(_uqword x, _uqword y, _uqword z, _uqword* pRest /*= nullptr*/, _uqword* pHighPart /*= nullptr*/) noexcept
{
    if  (pHighPart)
    {
        *pHighPart = 0;
    }
    if  (pRest)
    {
        *pRest = 0;
    }

    if  (z == 0)
    {
        return 0;
    }

    ULARGE_INTEGER xx, yy, zz, m[2];
    xx.QuadPart = x;
    yy.QuadPart = y;
    zz.QuadPart = z;
    MaaMul64x64(xx, yy, m);

    if  (m[1].QuadPart == 0)
    {
        if  (pRest)
        {
            *pRest = (_uqword)(m[0].QuadPart % zz.QuadPart);
        }
        return (_uqword)(m[0].QuadPart / zz.QuadPart);
    }

    ULARGE_INTEGER res[2];
    res[0].QuadPart = 0;
    res[1].QuadPart = 0;
    while(1)
    {
        if  (m[1].QuadPart == 0)
        {
            ULARGE_INTEGER tmp[2];
            tmp[0].QuadPart = m[0].QuadPart / zz.QuadPart;
            m[0].QuadPart %= zz.QuadPart;
            tmp[1].QuadPart = 0;
            MaaAdd128(res, tmp);
            break;
        }
        int sn;
        for (sn = 0; sn < 64; sn++)
        {
            ULARGE_INTEGER tmp;
            tmp.QuadPart = (((ULONGLONG)1) << (63 - sn));
            if  (m[1].QuadPart & tmp.QuadPart)
            {
                break;
            }
        }
        ULARGE_INTEGER Dividend[2];
        Dividend[1].QuadPart = (m[1].QuadPart << sn) | (m[0].QuadPart >> (64 - sn));
        Dividend[0].QuadPart = (m[0].QuadPart << sn);

        int sz;
        for (sz = 0; sz < 64; sz++)
        {
            ULARGE_INTEGER tmp;
            tmp.QuadPart = (((ULONGLONG)1) << (63 - sz));
            if  (zz.QuadPart & tmp.QuadPart)
            {
                break;
            }
        }
        sz = 64 - sz;
        int s = 32 - sz;
        ULARGE_INTEGER Divider1;
        if  (s >= 0)
        {
            Divider1.QuadPart = zz.QuadPart << s;
        }
        else
        {
            Divider1.QuadPart = zz.QuadPart >> (-s);
        }
        ULARGE_INTEGER alpha;
        alpha.QuadPart = (Dividend[1].QuadPart - 1) / (Divider1.QuadPart + 1);
        int sh = s + 64 - sn;
        if  (sh < 0)
        {
            alpha.QuadPart >>= (-sh);
            sh = 0;
        }
        if  (alpha.QuadPart == 0)
        {
            while(m[1].QuadPart != 0 || m[0].QuadPart >= zz.QuadPart)
            {
                if  (m[0].QuadPart < zz.QuadPart)
                {
                    m[1].QuadPart--;
                }
                m[0].QuadPart -= zz.QuadPart;
                if  ((++res[0].QuadPart) == 0)
                {
                    res[1].QuadPart++;
                }
            }
            break;
        }
        ULARGE_INTEGER m2[2];
        MaaMul64x64(alpha, zz, m2);
        m2[1].QuadPart = (m2[1].QuadPart << sh) | (m2[0].QuadPart >> (64 - sh));
        m2[0].QuadPart <<= sh;
        if  (m[0].QuadPart < m2[0].QuadPart)
        {
            m[1].QuadPart--;
        }
        m[0].QuadPart -= m2[0].QuadPart;
        m[1].QuadPart -= m2[1].QuadPart;

        m2[1].QuadPart = alpha.QuadPart >> (64 - sh);
        m2[0].QuadPart = alpha.QuadPart << sh;
        MaaAdd128(res, m2);
    }
    if  (pHighPart)
    {
        *pHighPart = m[1].QuadPart;
    }
    if  (pRest)
    {
        *pRest = m[0].QuadPart;
    }
    return (_uqword)res[0].QuadPart;
}
#endif


/*
int hrt_test()
{
    CMaaTime hrt1(100), hrt2(100);
    _uqword t1 = hrt1.GetTime();
    _uqword t2 = hrt2.GetTime();
    Sleep(1000);
    t1 = hrt1.GetTime() - t1;
    t2 = hrt2.GetTime() - t2;
    __utf8_printf("%,D\n", t1);
    __utf8_printf("%,D\n", t2);
    return 0;
}
*/
