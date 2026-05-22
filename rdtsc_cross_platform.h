//so that the implementation compiles. 
//If you are using this file as .h remove this
#define MODULE_IMPL_ALL

//rdtsc.h
#ifndef MODULE_TSC
#define MODULE_TSC

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
    static inline uint64_t rdtsc();         //current value of the time step counter (TSC)
    static inline uint64_t rdtsc_freq();    //best guess at frequency of the TSC. This might be better correspond with real time then what the cpu lists.
    static inline void     rdtsc_barrier(); //can be used to make sure all instructions happened before or after calling rdtsc()

    static inline uint64_t os_counter();    //Wrapper around QueryPerformanceCounter or equivalent
    static inline uint64_t os_counter_freq(); //Wrapper around QueryPerformanceFrequency or equivalent

    //Some things about TSC:
    // 1 - TSC does not report "real" cycles: 
    //     After the early 2000s the TSC does no longer report real cycles and instead ticks at constant rate. 
    //     This information can be queried by checking cpuid TscInvariant flag. On the internet you will find people claiming
    //     that relying on constant rate TSC is dangerous, but by now its so widespread its safe to do so.
    //     One can also use cpuid to query the nominal (see below) TSC frequency. This, however, only works on some chips and 
    //     others require a lot of edge case handling. To see how to query these: 
    //     https://gist.github.com/Boostibot/483d36a34bc8092f207a456e5d4217dc
    //    
    // 2 - TSC nominal frequency is not fully accurate:
    //     If you query the nominal TSC frequency from cpuid and use it to compare time obtained from rdtsc/frequency against system clock, 
    //     you will find that they start to diverge over time. The speed of this divergenece will depend on your CPU, OS etc. 
    //     but it will probably be there (for me it is). To try it for yourself (on x86-64 chip any OS) run the code at: 
    //     https://gist.github.com/Boostibot/483d36a34bc8092f207a456e5d4217dc.
    //
    //     Instead of querying the TSC rate from cpuid, we ask the OS, which provides a number that is already adjusted for this
    //     time divergence. It is the best estimate towards the "real" tsc rate and spares us handling all the edge cases from cpuid. 
    //     
    // 3 - How does the OS clock work:
    //     If the system clock diverges from the TSC over time, how does QueryPerformanceCounter/clock_gettime (QPC) work? 
    //     Simply put every once in a while the OS obtains the real time from some other, more accurate, clock and only uses the 
    //     TSC to interpolate between these points. How it works under the hood is that there is a special memory page which gets 
    //     periodically updated by the kernel. It contains the TSC rate, real time offset and the TSC of the last kernel update. 
    //     When we call QueryPerformanceCounter/clock_gettime we get the current TSC, calculate the delta from the last kernel update, 
    //     convert it to some time unit using the rate and offset it. essentially: 
    //            real_time = (rdtsc() - last_kernel_tsc)/last_kernel_rate + last_kernel_real_time_offset. 
    //     On Linux the kernel also periodically updates the last_kernel_rate to best approximate the real tsc rate, other than that
    //     the mechanism on Linux/Windows/Mac is near identical. For more details and an example custom recreation of how this works on linux see: 
    //     https://gist.github.com/Boostibot/3058caba7e5009bc7dfd180195cf6fcd.
    //
    // 4 - When to use the TSC instead of OS clock:
    //     Doing raw rdtsc is about 2x faster than QPC from my testing. As such it will provide more
    //     accurate results for timing short pieces of code. Besides, the rate at which the kernel provides the real time is quite long
    //     (is said to be 4ms on linux) so its very unlikely it will happen inside the timed region - thus both methods, ignoring overhead, 
    //     should report exactly the same value. 
    //     
    //     On the other hand TSC is poor fit for long term timestamps. If we assume TSC is completely inaccurate and always returns 0, 
    //     QPC will be accurate to the kernel update interval. This gives us a guarantee that we will always be withing 
    //     roughly 4ms of the real time. With TSC we have no such guarantee.
    //
    //     So TLDR use rdtsc for small deltas and QPC for time stamps/big deltas. If you are making a profiler
    //     it might be best to use rdtsc for time keeping but every once in a while QPC, then use the TSC values 
    //     to effectively interpolate between the QPC values. 
    ///    This also spares you the need to calculate the TSC frequency and thus to use this ugly file ;) 


    //Inline implementation=====================
#if defined(_WIN32) || defined(_WIN64)
    typedef union _LARGE_INTEGER LARGE_INTEGER;
    typedef int BOOL;
    __declspec(dllimport) BOOL __stdcall QueryPerformanceCounter(LARGE_INTEGER* out);
    __declspec(dllimport) BOOL __stdcall QueryPerformanceFrequency(LARGE_INTEGER* out);

    static inline uint64_t os_counter()
    {
        uint64_t out = 0;
        QueryPerformanceCounter((LARGE_INTEGER*)(void*)&out);
        return out;
    }

    static inline uint64_t os_counter_freq()
    {
        static uint64_t freq = 0;
        if (freq == 0) QueryPerformanceFrequency((LARGE_INTEGER*)(void*)&freq);
        return freq;
    }

#elif defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
#include <time.h>
    static inline uint64_t os_counter()
    {
        struct timespec ts = { 0 };
        (void)clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
        return (uint64_t)ts.tv_nsec + ts.tv_sec * 1000000000LL;
    }

    static inline uint64_t os_counter_freq()
    {
        return (uint64_t)1000000000LL;
    }
#else
#error unsupported platform!
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__) && !defined(_M_ARM64EC) || defined(_M_CEE_PURE) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#ifdef _MSC_VER
#include <intrin.h>
    static inline uint64_t rdtsc() {
        _ReadWriteBarrier();
        return __rdtsc();
    }
    static inline void rdtsc_barrier() {
        _ReadWriteBarrier();
        _mm_lfence();
    }
#else
#include <x86intrin.h>
    static inline uint64_t rdtsc() {
        __asm__ __volatile__("":::"memory");
        return __rdtsc();
    }
    static inline void rdtsc_barrier() {
        __asm__ __volatile__("":::"memory");
        _mm_lfence();
    }
#endif
#elif defined(_M_ARM64) || defined(_M_ARM64EC) || defined(__aarch64__) || defined(__ARM_ARCH_ISA_A64)
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#endif

//msvc version taken from: https://gist.github.com/mmozeiko/98bb947fb5a9d5b8a695adf503308a58#file-armv8_tsc-h-L19-L45
//inline assembly Adapted from: https://github.com/cloudius-systems/osv/blob/master/arch/aarch64/arm-clock.cc
    static inline uint64_t rdtsc() {
        //Please note we read CNTVCT cpu system register which provides
        //the accross-system consistent value of the virtual system counter.
        uint64_t cntvct;
#if defined(_MSC_VER) && !defined(__clang__)
        // "Accessing CNTVCT_EL0" in https://developer.arm.com/documentation/ddi0601/latest/AArch64-Registers/CNTVCT-EL0--Counter-timer-Virtual-Count-Register
        cntvct = _ReadStatusReg(ARM64_SYSREG(3, 3, 14, 0, 2));
#else
        asm volatile ("mrs %0, cntvct_el0; " : "=r"(cntvct) :: "memory");
#endif
        return cntvct;
    }

    static inline void rdtsc_barrier() {
#if defined(_MSC_VER) && !defined(__clang__)
        __isb(_ARM64_BARRIER_SY);
#else
        asm volatile ("isb;" ::: "memory");
#endif
    }

#define RDTSC_FREQ_ASSEMBLY
    static inline uint64_t _rdtsc_freq_assembly() {
        uint64_t freq_hz;
#if defined(_MSC_VER) && !defined(__clang__)
        // "Accessing CNTFRQ_EL0" in https://developer.arm.com/documentation/ddi0601/latest/AArch64-Registers/CNTFRQ-EL0--Counter-timer-Frequency-Register
        freq_hz = _ReadStatusReg(ARM64_SYSREG(3, 3, 14, 0, 0));
#else
        asm volatile ("mrs %0, cntfrq_el0; isb; " : "=r"(freq_hz) :: "memory");
#endif
        return freq_hz;
    }
#else
#define RDTSC_IMPL_FALLBACK	
    static inline uint64_t rdtsc() { return os_counter(); }
    static inline uint64_t rdtsc_freq() { return os_counter_freq(); }
    static inline void     rdtsc_barrier() {}
#endif


#ifndef RDTSC_IMPL_FALLBACK
    extern uint64_t g_rdtsc_freq;
    uint64_t query_rdtsc_freq();
    static inline uint64_t rdtsc_freq()
    {
        if (g_rdtsc_freq == 0)
            g_rdtsc_freq = query_rdtsc_freq();
        return g_rdtsc_freq;
    }
#endif

#if (defined(MODULE_IMPL_ALL) || defined(MODULE_IMPL_TIME)) && !defined(MODULE_HAS_IMPL_TIME) && !defined(RDTSC_IMPL_FALLBACK)
#define MODULE_HAS_IMPL_TIME

    //Combination of the following:
    // original gist: https://gist.github.com/pmttavara/6f06fc5c7679c07375483b06bb77430c
    // discussion here: https://hero.handmade.network/forums/code-discussion/t/7485-queryperformancefrequency_returning_10mhz_bug/2#23567
    // dump_vdso_data.c: https://gist.github.com/mildsunrise/c63505931534bd3c0e143c0db8cad3f3
    // https://linux.die.net/man/2/perf_event_open
    // https://stackoverflow.com/a/57835630
    uint64_t g_rdtsc_freq = 0;
#if defined(RDTSC_FREQ_ASSEMBLY)
    uint64_t query_rdtsc_freq() {
        return _rdtsc_freq_assembly();
    }
#elif defined(_WIN32) || defined(_WIN64)

#include <stdint.h>
#include <stdio.h>
    uint64_t query_rdtsc_freq() {
        //predeclare stuff from windows.h
        typedef struct HINSTANCE__* HINSTANCE;
        typedef HINSTANCE HMODULE;
        typedef int BOOL;
        typedef unsigned long DWORD;
        typedef long long(__stdcall* FARPROC)();

        __declspec(dllimport) HMODULE __stdcall LoadLibraryA(const char* lpLibFileName);
        __declspec(dllimport) BOOL __stdcall FreeLibrary(HMODULE hLibModule);
        __declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE hModule, const char* lpProcName);
        __declspec(dllimport) void __stdcall Sleep(DWORD dwMilliseconds);

        uint64_t tsc_freq = 0;
        HMODULE ntdll = LoadLibraryA("ntdll.dll");
        if (ntdll) {

            int (*NtQuerySystemInformation)(int, void*, unsigned int, unsigned int*) =
                (int (*)(int, void*, unsigned int, unsigned int*))GetProcAddress(ntdll, "NtQuerySystemInformation");
            if (NtQuerySystemInformation) {

                volatile uint64_t* hypervisor_shared_page = NULL;
                unsigned int size = 0;

                // SystemHypervisorSharedPageInformation == 0xc5
                int result = (NtQuerySystemInformation)(0xc5, (void*)&hypervisor_shared_page, sizeof(hypervisor_shared_page), &size);

                // success
                if (size == sizeof(hypervisor_shared_page) && result >= 0) {
#if defined(_MSC_VER) && !defined(__clang__)
#if defined(_WIN32)
                    tsc_freq = (10000000ull << 32) / (hypervisor_shared_page[1] >> 32);
#else
                    uint64_t rem = 0;
                    tsc_freq = _udiv128(10000000ull, 0, hypervisor_shared_page[1], &rem);
#endif
#else
                    tsc_freq = ((unsigned __int128)10000000ull << (unsigned __int128)64ull) / hypervisor_shared_page[1];
#endif
                }
            }
            FreeLibrary(ntdll);
        }

        // Slow path
        if (!tsc_freq) {
            fprintf(stderr, "query_rdtsc_freq(): all other modes failed timing sleep\n");

            // Get time before sleep
            uint64_t qpc_begin = 0; QueryPerformanceCounter((LARGE_INTEGER*)&qpc_begin);
            uint64_t tsc_begin = rdtsc();

            Sleep(2);

            // Get time after sleep
            uint64_t qpc_end = qpc_begin + 1; QueryPerformanceCounter((LARGE_INTEGER*)&qpc_end);
            uint64_t tsc_end = rdtsc();

            // Do the math to extrapolate the RDTSC ticks elapsed in 1 second
            uint64_t qpc_freq = 0; QueryPerformanceFrequency((LARGE_INTEGER*)&qpc_freq);
            tsc_freq = (tsc_end - tsc_begin) * qpc_freq / (qpc_end - qpc_begin);
        }

        // Failure case
        if (!tsc_freq)
            tsc_freq = 1000000000;

        return tsc_freq;
    }
#elif defined(__linux__)

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <linux/perf_event.h>
#include <time.h>
#include <unistd.h>
    uint64_t query_rdtsc_freq(void) {

        // Cache the answer so that multiple calls never take the slow path more than once
        uint64_t tsc_freq = 0;

        //just the start of the vdso_data structure (since thats all we need)
        typedef struct vdso_data_start_t {
            uint32_t seq;
            int32_t clock_mode;
            uint64_t cycle_last;
            uint64_t mask;
            uint32_t mult;
            uint32_t shift;
        } vdso_data_start_t;

        // Fast path: Get the frequency through vdso. This assumes a concrete layout of the vdso data
        // in the linux kernel. This is complete hack using undocumented internal api and is likely to break in the future.
        // Still it is a best guess we have on linux (see below for the other method and its problems).
        // It was reconstructed from the clock_gettime implementation and various other things
        uint8_t* vvar_addr = NULL;
        {
            // quickly parse /proc/self/maps to find [vvar] mapping
            char mmaps[4096 * 4] = { 0 };
            FILE* mmapsfile = fopen("/proc/self/maps", "r");
            if (!mmapsfile)
                fprintf(stderr, "query_rdtsc_freq(): could not access own maps\n");
            else
            {
                size_t nread = fread(mmaps, 1, sizeof(mmaps) - 1, mmapsfile);
                fclose(mmapsfile);
                if (nread > 0)
                {
                    for (char* line = mmaps; line != NULL;) {
                        char* next_line = strchr(line, '\n');
                        if (next_line != NULL)
                            *(next_line++) = 0;

                        if (strstr(line, "[vvar]")) {
                            vvar_addr = (uint8_t*)(void*)strtoull(line, NULL, 16);
                            break;
                        }
                        line = next_line;
                    }
                }

                if (vvar_addr == NULL)
                    fprintf(stderr, "query_rdtsc_freq(): could not find [vvar] mapping\n");
            }
        }

        if (vvar_addr) {
            //+ 128 because thats where the vdso_data starts
            vdso_data_start_t* vdso_data = (vdso_data_start_t*)(vvar_addr + 128);
            if (vdso_data->shift < 64 && vdso_data->mult != 0)
                tsc_freq = (uint64_t)(((__uint128_t)1000000000ull << vdso_data->shift) / vdso_data->mult);

            //its more probable this method failed then your cpu being this slow
            if ((int64_t)tsc_freq < 20000)
                tsc_freq = 0;
        }

        // Fast path: Load kernel-mapped memory page (this may require higher priveledges and might fail)
        if (!tsc_freq) {
            fprintf(stderr, "query_rdtsc_freq(): get_vvar_address() failed attempting to get perf event info\n");

            struct perf_event_attr pe = { 0 };
            pe.type = PERF_TYPE_HARDWARE;
            pe.size = sizeof(struct perf_event_attr);
            pe.config = PERF_COUNT_HW_INSTRUCTIONS;
            pe.disabled = 1;
            pe.exclude_kernel = 1;
            pe.exclude_hv = 1;

            // __NR_perf_event_open == 298 (on x86_64)
            int fd = syscall(298, &pe, 0, -1, -1, 0);
            if (fd != -1) {

                struct perf_event_mmap_page* pc = (struct perf_event_mmap_page*)mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0);
                if (pc) {

                    // success
                    if (pc->cap_user_time == 1) {
                        // docs say nanoseconds = (tsc * time_mult) >> time_shift
                        //      set nanoseconds = 1000000000 = 1 second in nanoseconds, solve for tsc
                        //       =>         tsc = (1000000000 << time_shift) / time_mult
                        tsc_freq = (uint64_t)(((__uint128_t)1000000000ull << pc->time_shift) / pc->time_mult);
                    }

                    munmap(pc, 4096);
                }
                close(fd);
            }
        }

        // Slow path
        if (!tsc_freq) {
            fprintf(stderr, "query_rdtsc_freq(): all other modes failed timing sleep\n");

            // Get time before sleep
            struct timespec t;
            clock_gettime(CLOCK_MONOTONIC_RAW, &t);
            uint64_t nsc_begin = (uint64_t)t.tv_sec * 1000000000ull + t.tv_nsec;
            uint64_t tsc_begin = rdtsc();

            usleep(10000); // 10ms gives ~4.5 digits of precision - the longer you sleep, the more precise you get

            // Get time after sleep
            clock_gettime(CLOCK_MONOTONIC_RAW, &t);
            uint64_t nsc_end = (uint64_t)t.tv_sec * 1000000000ull + t.tv_nsec;
            uint64_t tsc_end = rdtsc();

            // Do the math to extrapolate the RDTSC ticks elapsed in 1 second
            tsc_freq = (tsc_end - tsc_begin) * 1000000000 / (nsc_end - nsc_begin);
        }

        // Failure case
        if (!tsc_freq)
            tsc_freq = 1000000000;

        return tsc_freq;
    }
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/types.h>
#include <sys/sysctl.h>

    uint64_t query_rdtsc_freq(void) {
        uint64_t freq = 1000000000;
        size_t size = sizeof(freq);
        sysctlbyname("machdep.tsc.frequency", &freq, &size, NULL, 0);
        return freq;
    }
#else
#error unsupported platform!
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

#include <stdio.h>
#include <math.h>
int main_rdtsc_freq() {
#ifdef RDTSC_IMPL_FALLBACK
    printf("Using fallback!\n");
#endif
    printf("Timing 10 ms...\n");
    uint64_t osc_begin = os_counter();
    uint64_t tsc_start = rdtsc();

    uint64_t wait_end = osc_begin + (uint64_t)(0.01 * os_counter_freq());
    while (os_counter() < wait_end);

    uint64_t osc_end = os_counter();
    uint64_t tsc_end = rdtsc();

    double tsc_diff = (double)(tsc_end - tsc_start) / rdtsc_freq();
    double osc_diff = (double)(osc_end - osc_begin) / os_counter_freq();

    printf("...took %f ms (rdtsc)\n", tsc_diff * 1e3);
    printf("...took %f ms (os)\n", osc_diff * 1e3);
    printf("=> error:%lf ns (keep in mind the two started at different times)\n",
        fabs(osc_diff - tsc_diff) * 1e9
    );

    printf("RDTSC frequency is %lu (%.4f GHz).\n", (unsigned long)rdtsc_freq(), rdtsc_freq() * 1e-9);
    printf("OS    frequency is %lu (%.4f GHz).\n", (unsigned long)os_counter_freq(), os_counter_freq() * 1e-9);
    return 0;
}