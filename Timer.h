
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Timer.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform high resolution time calculations.
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

//---------------------------------------------------------------------------
#ifdef _WIN32
ULONG MaaAdd128(ULARGE_INTEGER* x, const ULARGE_INTEGER* y) noexcept;
void MaaMul64x64(const ULARGE_INTEGER& xx, const ULARGE_INTEGER& yy, ULARGE_INTEGER* res) noexcept;
_uqword MaaMulDiv64(_uqword x, _uqword y, _uqword z, _uqword *pRest = nullptr, _uqword *pHighPart = nullptr) noexcept;
#endif
#ifdef _WIN64
extern "C"
{
    _uqword MaaAsmMulDiv64_win(_uqword x, _uqword y, _uqword z, _uqword* pRest = nullptr, _uqword* pHighPart = nullptr) noexcept;
}
#define MaaMulDiv64 MaaAsmMulDiv64_win
#endif
#ifdef __linux__
extern "C"
{
    _uqword MaaAsmMulDiv64_unix(_uqword x, _uqword y, _uqword z, _uqword* pRest = nullptr, _uqword* pHighPart = nullptr) noexcept;
}
#define MaaMulDiv64 MaaAsmMulDiv64_unix
#endif
//---------------------------------------------------------------------------

#if defined(_WIN32) || defined(__linux__) // linux can be commented because web app are delayed by 0.1 ms to calibrate

#ifdef _WIN32
#if defined (_M_ARM)
//#include <armintr.h>
//#include <arm_neon.h>
#endif

#if defined (_M_ARM64)
//#include <arm64intr.h>
//#include <arm64_neon.h>
#endif
#else
#include <x86intrin.h>
#endif

class CMaaTime
{
    std::atomic<_uqword> m_StartTime__rdtsc, m_LastTime__rdtsc;
    std::atomic<_uqword> m_LastUsTime;
    _uqword m_Freq, m_Accuraciry, m_IAccuraciry;
    std::atomic<_uqword> m_LastTime, m_LastTimeFactor, m_NextTime__rdtsc;
public:
    _uqword GetCounter() const noexcept
    {
#ifdef _WIN32
#ifdef __KERNEL
#ifndef _IA64_
        return __rdtsc();
#else
        return KeQueryInterruptTime();
#endif
#else

#if defined (_M_ARM) || defined (_M_ARM64)
        LARGE_INTEGER l;
        //l.QuadPart = 0;
        QueryPerformanceCounter(&l);
        return (_uqword)l.QuadPart;
#else
        return __rdtsc();
#endif

#endif
#else
        // linux
        return __rdtsc();
#endif
    }
    _uqword GetAccuracity() const noexcept
    {
        return m_Accuraciry;
    }
    _uqword GetIAccuracity() const noexcept
    {
        return m_IAccuraciry;
    }
    _uqword GetFrequency() const noexcept
    {
        return m_Freq;
    }
    CMaaTime() noexcept;
    CMaaTime(CMaaTime &That) noexcept;
    _uqword GetTime(_uqword Factor = 1000000) noexcept; // in seconds * Factor
    _uqword GetDeltaTime(_uqword t, _uqword Factor = 1000000) const noexcept;
    _uqword GetNextTime(_uqword t, _uqword Factor = 1000000) const noexcept
    {
        return GetCounter() + GetDeltaTime(t, Factor);
    }
    _uqword GetTimeEx(_uqword Factor = 1000000) noexcept; // more fast call by using time caching
    _uqword StartWatch() noexcept; // returns 0 us
    double GetTime_fp() noexcept
    {
        return GetTime() / 1000000.0;
    }
};
#else
// unix
class CMaaTime
{
    timeval m_StartTime;
public:
    _uqword GetCounter() const noexcept
    {
        timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec * 1000000 + tv.tv_usec;
    }
    _uqword GetAccuracity() const noexcept
    {
        return 1;
    }
    _uqword GetIAccuracity() const noexcept
    {
        return 1;
    }
    _uqword GetFrequency() const noexcept
    {
        return 1000000;
    }
    CMaaTime(int CalibrateMs = 0) noexcept;
    CMaaTime(CMaaTime &That) noexcept;
    _uqword GetTime(_uqword Factor = 1000000) noexcept; // in seconds * Factor
    _uqword GetDeltaTime(_uqword t, _uqword Factor = 1000000) const noexcept;
    _uqword GetNextTime(_uqword t, _uqword Factor) const noexcept
    {
        return GetCounter() + GetDeltaTime(t, Factor);
    }
    _uqword GetNextTime(_uqword t) const noexcept // Factor = 1000000
    {
        return GetCounter() + t;
    }
    _uqword GetTimeEx(_uqword Factor = 1000000) noexcept
    {
        return GetTime(Factor);
    }
    _uqword StartWatch() noexcept; // returns 0 us
    double GetTime_fp() noexcept
    {
        return GetTime() / 1000000.0;
    }
};

#endif

inline CMaaTime & GetHRTime(bool bInit = false) noexcept
{
    //static CMaaLiteMutex* gptr = nullptr;
    static CMaaTime* phrt = nullptr;
    if (bInit && !phrt)
    {
        //gptr = TL_NEW_ CMaaLiteMutex;
        //gptr->AddRef();
#ifdef TOOLSLIB_KEEP_GLOBALS2_IN_MEMORY
        phrt = TL_NEW_ CMaaTime;
#else
        static CMaaTime g;
        phrt = &g;
#endif
    }
    return *phrt;
}

#define gHRTime GetHRTime()

inline CMaaTime& GetUsTimer() noexcept
{
    static CMaaTime t(GetHRTime(true));
    return t;
}
inline CMaaTime& GetMsTimer() noexcept
{
    static CMaaTime t(GetHRTime(true));
    return t;
}

inline _uqword GetUsTime() noexcept
{
    return GetUsTimer().GetTimeEx(1000000);
}
inline _uqword GetMsTime() noexcept
{
    return GetMsTimer().GetTimeEx(1000);
}

// MyGetTickCount() is 5x slower then GetTickCount()
inline _dword MyGetTickCount() noexcept
{
#ifdef __unix__000
    // ~2.7x slower then GetMsTime();
    timeval tv;
    gettimeofday(&tv, nullptr);
    return (_dword)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#else
    return (_dword)GetMsTime();
#endif
}

// MyGetTickCount64() is 5x slower then GetTickCount64()
inline _uqword MyGetTickCount64() noexcept
{
#ifdef __unix__000
    // 2.7x slower then GetUsTime();
    timeval tv;
    gettimeofday(&tv, nullptr);
    return (_uqword)tv.tv_sec * 1000000 + tv.tv_usec;
#else
    return GetUsTime();
#endif
}

_uqword CMaaTimeStamp() noexcept; // time(nullptr) * 1000000 + usec
time_t CMaaTimeStampToTime(_uqword ts, int * pusec = nullptr) noexcept;
time_t CMaaGetTimeOfDay(int* pMicroseconds = nullptr) noexcept;

#ifdef __unix__
#define GetTickCount MyGetTickCount
#define GetTickCount64 MyGetTickCount64
#endif

//---------------------------------------------------------------------------
