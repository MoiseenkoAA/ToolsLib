
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/SysInfo.h

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

#ifndef __TOOLS_SYSINFO_H
#define __TOOLS_SYSINFO_H

//------------------------------------------------------------------------------
// class CMaaCpuUsage, CMaaGetCpuCount(), MaaGetMemoryUsage(*)
// // win32 MaaGetActiveProcessorCoresCount()
//------------------------------------------------------------------------------

#ifdef _WIN32
// win32 only: ret 4 phys cores on cpu with 4 phys and 8 logical cores
int MaaGetActiveProcessorCoresCount();
#endif

// win32: ret 4 phys cores on cpu with 4 phys and 8 logical cores (MaaGetActiveProcessorCoresCount())
// linux: ret logical cores count
// first call init
int CMaaGetCpuCount() noexcept;
int CMaaGetHardwareConcurrency() noexcept;

class CMaaCpuUsage
{
protected:
#ifdef _WIN32
    ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
    //static int numProcessors;
    HANDLE self;
#endif
#ifdef __linux__
    clock_t lastCPU, lastSysCPU, lastUserCPU;
#endif
    bool m_100p_is_one_core;
public:
    CMaaCpuUsage(bool _100p_is_one_core = true) noexcept;
    double Get(double* pSys = nullptr, double* pUsr = nullptr, int* pCores = nullptr) noexcept;
    int Print() noexcept;
};

class CMaaCpuSystemUsage
{
protected:
#ifdef _WIN32
    ULARGE_INTEGER lastCPU, lastIdle, lastKernel, lastUser;
    //static int numProcessors;
#endif
#ifdef __linux__
    long clk_tck;
    clock_t t;
    _qword a, v[10];
#endif
    bool m_100p_is_one_core;
public:
    CMaaCpuSystemUsage(bool _100p_is_one_core = true) noexcept;
    double Get(double* pKernelOnly = nullptr, double* pUsr = nullptr, double* pIdle = nullptr, int* pCores = nullptr) noexcept;
    int Print() noexcept;
};

// returns current process virtual memory used and peak
_qword MaaGetMemoryUsage(_qword* pPeak = nullptr) noexcept;

// do not use
// win32 CTOOGetHeapAllocated() is slow, is not thread safe
//_qword CTOOGetMemoryAllocated();

unsigned char* fswap_bits(const unsigned char* m, int N); // N bits
void fswap_bits1(unsigned char* m, int N) noexcept; // N bits
void fprint_bits(const unsigned char* m, int N) noexcept;
void fprint_bits2(const unsigned char* m, int N);
CMaaString BitsToString(const unsigned char* m, int N); // temp func
int StringToBits(const CMaaString& s, unsigned char* m) noexcept; // temp func

#endif // __TOOLS_SYSINFO_H

//==============================================================================
