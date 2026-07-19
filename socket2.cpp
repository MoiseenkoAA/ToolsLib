
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Ìîèñååíêî Àíäðåé Àëåêñååâè÷)
 */

// ToolsLib/socket2.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * sockets (TCP, UDP), IPv4 addresses, shapers, connections and
 * servers templates, perfect timers, htonq/ntohq functions for qword.
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

//#include "stdafx.h"
#include "perm.h"
#ifdef _WIN32
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif
static int DebugLevel = 0;
#else
#include "temp.h"
static int DebugLevel = 0;
#endif
//#define IfDebug(x) if ( x <= DebugLevel )
#define IfDebug(x) if ( false )

#define LOG1(x) //m_C == 1 && log1(x)
#define LOG1_(x) //m_pFdSockets->m_C == 1 && m_pFdSockets->log1(x)
#define DP(x) //x
#define DP1(x) x
#define DP2(x) //x

//extern SOCKET g_____socket;

//int gInt = 0;

//SOCKET gH1 = -1;

const _IP6 gIp6_0/*(_IP6::eInitByZero)*/, gIp6_1(_IP6::eInitByOne), gIp6_v4Supp(_IP6::eV4Supp);

#ifdef __SHAPERS_PROFILING
CMaaString gShaperPerformanceInfo;
CMaaString gShaperTempInfo;
//int iSockThread_pt = 0;
#endif

CMaaTime * gpShaper_hrt = nullptr;
CMaaFile * gpm_logfile = nullptr;

#define m_hrt (*gpShaper_hrt)
#define m_logfile (*gpm_logfile)

_qword SockThread_pt[100];

#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)
#ifndef TOOLSLIB_SINGLE_THREAD

DEF_ALLOCATOR(CMaaSockThread::SCmd)
DEF_ALLOCATOR(CMaaSockThread)
DEF_ALLOCATOR(CMaaFdSockets)

#ifdef __SOCK_NEW_TIMERS
DEF_ALLOCATOR_CMaaHeap(_qword, CMaaSocketTimer *)
#endif

CMaaFdSockets * gCMaaFdSockets = nullptr;
CMaaSockStartup * gpSockStartup = nullptr;


//#define __FORCED_CLOSE_NOTIFICATION

void ExtLog(const char * msg);

//void Log(const char * txt);

/*
void pppp(const char *fmt, ...)
{
     va_list list;
     va_start(list, fmt);
     char txt[1024];
        _vsnprintf(txt, sizeof(txt), fmt, list);
     Log(txt);
     va_end ( list );
}

#define printf pppp
*/

void Trace(const char * msg, int s)
{
    printf(msg, s);
    printf("\n");
    fflush(stdout);
}

void Trace(const char * msg, int s, const char * str)
{
    printf(msg, s, str);
    printf("\n");
    fflush(stdout);
}

//----------------------------------------------------------------------------------------------------------

DEF_ALLOCATOR(CMaaConnectionInTimeCharacteristics::DataBlock)

CMaaConnectionInTimeCharacteristics::CMaaConnectionInTimeCharacteristics(int MaxPoints, _qword InitialValue, _qword Units)
:   m_bptr(((m_max_points = (MaxPoints >= 0 ? MaxPoints : -1) + 1) + 7) / 8)
{
    m_Units = Units;
    m_bptr.Zero();
    Reset(InitialValue);
}

CMaaConnectionInTimeCharacteristics::~CMaaConnectionInTimeCharacteristics()
{
    for (int i = (int)m_bptr.MaxIndex(); i--; )
    {
        delete m_bptr[i];
    }
}

void CMaaConnectionInTimeCharacteristics::Reset(_qword InitialValue, bool bToLastData)
{
    if  (bToLastData)
    {
        if  (m_pos > 0)
        {
            int RealPos = (m_pos - 1) % m_max_points;
            InitialValue = m_bptr[RealPos / 8]->Data[RealPos % 8];
            m_pos = 0;
            Set_Inc(m_InitialValue = InitialValue);
            return;
        }
    }
    m_pos = 0;
    Set_Inc(m_Accumulator = m_InitialValue = InitialValue);
}

void CMaaConnectionInTimeCharacteristics::Set_Inc(_qword x)
{
    if  (m_max_points <= 0)
    {
        return;
    }
    if  (m_pos < m_max_points && m_bptr[m_pos / 8] == nullptr)
    {
        m_bptr[m_pos / 8] = TL_NEW DataBlock;
        if  (!m_bptr[m_pos / 8])
        {
            return;
        }
    }
    const int RealPos = m_pos % m_max_points;
    m_bptr[RealPos / 8]->Data[RealPos % 8] = x;
    m_pos++;
}

void CMaaConnectionInTimeCharacteristics::Accumulate(_qword x, bool bSet_and_Inc)
{
    m_Accumulator += x;
    if  (bSet_and_Inc)
    {
        Set_Inc(m_Accumulator);
    }
}

_qword CMaaConnectionInTimeCharacteristics::GetDataDifference(int Period) const noexcept
{
    if  (m_pos <= 1 || Period <= 0)
    {
        if  (Period == -1 && m_pos > 0)
        {
            const int RealPos = (m_pos - 1) % m_max_points;
            return m_bptr[RealPos / 8]->Data[RealPos % 8] - m_InitialValue;
        }
        return 0;
    }

    if  (Period >= m_pos - 1 && m_pos < m_max_points)
    {
        const int RealPos = (m_pos - 1) % m_max_points;
        return m_bptr[RealPos / 8]->Data[RealPos % 8] - m_bptr[0]->Data[0];
    }

    if  (Period >= m_max_points)
    {
        Period = m_max_points - 1;
    }

    const int StartPos = (m_pos - 1 - Period) % m_max_points;
    const int EndPos = (m_pos - 1) % m_max_points;
    return m_bptr[EndPos / 8]->Data[EndPos % 8] - m_bptr[StartPos / 8]->Data[StartPos % 8];
}

_qword CMaaConnectionInTimeCharacteristics::GetAverageDataDifference(int Period) const noexcept
{
    if  (m_pos <= 1 || Period <= 0)
    {
        if  (Period == -1 && m_pos > 0)
        {
            const int RealPos = (m_pos - 1) % m_max_points;
            return (m_bptr[RealPos / 8]->Data[RealPos % 8] - m_InitialValue) / m_pos / m_Units;
        }
        return 0;
    }

    if  (Period >= m_pos - 1 && m_pos < m_max_points)
    {
        const int RealPos = (m_pos - 1) % m_max_points;
        return (m_bptr[RealPos / 8]->Data[RealPos % 8] - m_bptr[0]->Data[0]) / (m_pos - 1) / m_Units;
    }

    if  (Period >= m_max_points)
    {
        Period = m_max_points - 1;
    }

    const int StartPos = (m_pos - 1 - Period) % m_max_points;
    const int EndPos = (m_pos - 1) % m_max_points;
    return (m_bptr[EndPos / 8]->Data[EndPos % 8] - m_bptr[StartPos / 8]->Data[StartPos % 8]) / Period / m_Units;
}

int CMaaConnectionInTimeCharacteristics::GetCurPos() const noexcept
{
    return m_pos > 0 ? m_pos - 1 : 0;
}

int CMaaConnectionInTimeCharacteristics::GetCurMaxPeriod() const noexcept
{
    const int x = GetCurPos();
    return x < m_max_points || x == 0 ? x : m_max_points - 1;
}
//----------------------------------------------------------------------------------------------------------
#endif
static constexpr void TwoBytes2Hex4_nzt(char* pp, const _byte* p) noexcept
{
    Byte2Hex_nzt_L(pp, p[0]);
    Byte2Hex_nzt_L(pp + 2, p[1]);
}
static constexpr char * Byte2Hex_nzt1(char* txt, _byte x, int zcnt) noexcept
{
    const _byte x1 = x >> 4;
    if  (x1 || zcnt >= 2)
    {
        *txt++ = x1 < 10 ? '0' + x1 : 'a' + x1 - 10;
    }
    const _byte x2 = x & 0xf;
    if  (x || zcnt >= 1)
    {
        *txt++ = x2 < 10 ? '0' + x2 : 'a' + x2 - 10;
    }
    return txt;
}
static constexpr char* TwoBytes2Hex_nzt(char* txt, const _byte* p) noexcept
{
    txt = Byte2Hex_nzt1(txt, p[0], 0);
    return Byte2Hex_nzt1(txt, p[1], p[0] ? 2 : 1);
}
#if 0
void MacToText(char* txt, const _byte* mac, bool bZTerm) noexcept
{
    Byte2Hex_nzt_LT(txt + 0 * 3, mac[0], ':');
    Byte2Hex_nzt_LT(txt + 1 * 3, mac[1], ':');
    Byte2Hex_nzt_LT(txt + 2 * 3, mac[2], ':');
    Byte2Hex_nzt_LT(txt + 3 * 3, mac[3], ':');
    Byte2Hex_nzt_LT(txt + 4 * 3, mac[4], ':');
    if (bZTerm)
    {
        Byte2Hex_nzt_LT(txt + 5 * 3, mac[5], 0);
    }
    else
    {
        Byte2Hex_nzt_L(txt + 5 * 3, mac[5]);
    }
    /*
    for (int i = 0; i < 5; i++)
    {
        Byte2Hex_nzt_L(txt, *mac++);
        txt += 2;
        *txt++ = ':';
    }
    Byte2Hex_nzt_L(txt, *mac);
    txt += 2;
    *txt = 0;
    */
    //return txt;
}
#endif
//---------------------------------------------------------------------------
bool TextToMac(void* pMac, const CMaaString &m) noexcept
{
    if (m.Length() != 3 * 6 - 1)
    {
        return false;
    }
    _byte m6[6];
    const char* p = m;
    int i = 0;
    for (; i < 5; i++, p += 3)
    {
        const int Nibble1 = To0F_LH(p[0]);
        const int Nibble2 = To0F_LH(p[1]);
        if (Nibble1 < 0 || Nibble2 < 0 || p[2] != ':')
        {
            return false;
        }
        m6[i] = (_byte)((Nibble1 << 4) | Nibble2);
    }
    {
        const int Nibble1 = To0F_LH(p[0]);
        const int Nibble2 = To0F_LH(p[1]);
        if (Nibble1 < 0 || Nibble2 < 0)
        {
            return false;
        }
        m6[i] = (_byte)((Nibble1 << 4) | Nibble2);
    }
    memcpy(pMac, m6, 6);
    return true;
}
//---------------------------------------------------------------------------
bool TextToMac(void* pMac, const char* p) noexcept
{
    _byte m6[6];
    int i = 0;
    for (; i < 5; i++, p += 3)
    {
        if (p[0] == 0 || p[1] == 0 || p[2] == 0)
        {
            return false;
        }
        const int Nibble1 = To0F_LH(p[0]);
        const int Nibble2 = To0F_LH(p[1]);
        if (Nibble1 < 0 || Nibble2 < 0 || p[2] != ':')
        {
            return false;
        }
        m6[i] = (_byte)((Nibble1 << 4) | Nibble2);
    }
    {
        if (p[0] == 0 || p[1] == 0 || p[2] != 0)
        {
            return false;
        }
        const int Nibble1 = To0F_LH(p[0]);
        const int Nibble2 = To0F_LH(p[1]);
        if (Nibble1 < 0 || Nibble2 < 0)
        {
            return false;
        }
        m6[i] = (_byte)((Nibble1 << 4) | Nibble2);
    }
    memcpy(pMac, m6, 6);
    return true;
}
//---------------------------------------------------------------------------
char * CMaaIpToTextEx(char * txt, const _byte * ip6, int Mode) noexcept
{
    // min txt size is 40 bytes
    if  ((Mode & 4) && CMaaIsMemEq(ip6, gIp6_v4Supp.const_ptr(), 12))
    {
        //_IP v4 = ntohl(*(_IP *)(ip6 + 12));
        strcpy(txt, "::ffff:");
        CMaaIpToTextEx(txt + 7, ntohl(*(_IP *)(ip6 + 12)), Mode & ~(4 | 16) & 0xf);
        return txt;
    }
    char* p = txt;
    if  (Mode & 16)
    {
        for (int i = 0; i < 16; i += 2)
        {
            TwoBytes2Hex4_nzt(p, ip6 + i);
            p += 4;
            *p++ = ':';
        }
        p--;
    }
    else
    {
        // the first longest 0000:*:0000 seed is replaced by ::
        int z0 = -1, z0_len = 0, z = -1, z_len = 0;
        int i, x;
        for (i = 0; i < 16; i += 2)
        {
            x = (((int)ip6[i]) << 8) | ip6[i + 1];
            if  (!x)
            {
                if  (z < 0)
                {
                    z = i;
                    z_len = 0;
                }
                z_len += 2;
                if  (z_len > 2 && z0_len < z_len)
                {
                    z0 = z;
                    z0_len = z_len;
                }
            }
            else
            {
                z = -1;
            }
        }
        if  (z0 >= 0)
        {
            if  (!z0)
            {
                *p++ = ':';
            }
            for (i = 0; i < z0; i += 2)
            {
                p = TwoBytes2Hex_nzt(p, ip6 + i);
                *p++ = ':';
            }
            for (i = z0 + z0_len; i < 16; i += 2)
            {
                *p++ = ':';
                p = TwoBytes2Hex_nzt(p, ip6 + i);
            }
            if  (z0 + z0_len == 16)
            {
                *p++ = ':';
            }
        }
        else
        {
            for (i = 0; i < 16; i += 2)
            {
                p = TwoBytes2Hex_nzt(p, ip6 + i);
                *p++ = ':';
            }
            p--;
        }
    }
    *p = 0;
#if 0 // old, ok
    // the first 0000:*:0000 seed is replaced by ::
    int prev0 = 0, haveZ = 0;
    for (int i = 0; i < 16; i += 2)
    {
        int x = (((int)ip6[i]) << 8) | ip6[i + 1];
        if  (x || haveZ)
        {
            if  (i)
            {
                *p++ = ':';
            }
            char tmp[10];
            sprintf(tmp, "%x", x);
            char *pp = tmp;
            while(*pp == '0') pp++;
            if  (!*pp)
            {
                pp--;
            }
            strcpy(p, pp);
            p += strlen(p);
            haveZ |= prev0;
            prev0 = 0;
        }
        else
        {
            if  (prev0++ == 0)
            {
                *p++ = ':';
            }
        }
    }
    if  (prev0)
    {
        *p++ = prev0 == 1 ? '0' : ':';
        // ":0" end one Zero / "::" end multiply Zeros
    }
    *p = 0;
#endif
    return txt;
}
//---------------------------------------------------------------------------
#ifndef TOOLSLIB_DELETE_UNSAFE
const char* CMaaIpToText(const _byte* ip6, int Mode) noexcept
{
    static char txt [ 64 ];
    return CMaaIpToTextEx(txt, ip6, Mode);
}
#endif
//---------------------------------------------------------------------------
char * CMaaIpToText(char * txt, const _byte * ip6) noexcept
{
    if  (txt)
    {
        CMaaIpToTextEx(txt, ip6);
    }
    return txt;
}
//---------------------------------------------------------------------------
// returns len of ip text, -1 on error, * RetIp = converted ipv6
// example: for x86 platform "[::1]-::2", ret=5
int CMaaIpToLongEx(const char *txt, _byte *RetIp, const char *AvailableTrailers, int Flags) noexcept
{
    return CMaaIpToLongEx(txt, -1, RetIp, AvailableTrailers, Flags);
}
int CMaaIpToLongEx(const char *txt, int len, _byte *RetIp, const char *AvailableTrailers, int Flags) noexcept
{
#define RET_ERR(x) return -1
    _byte ip[16];
    len = len >= 0 ? len : (int)strlen(txt);
    memset(ip, 0, 16);
    const char *p = txt, *e = txt + len, *pp;
    char ce = 0;
    if  (len && *p == '[')
    {
        p++;
        //~~ e = strchr(p, ']');
        for (pp = p; pp < e; pp++)
        {
            if  (*pp == ']')
            {
                break;
            }
        }
        if  (pp == e)
        {
            // ']' not found
            RET_ERR(-2);
        }
        e = pp;
        ce = ']';
    }
    int b = 0;
    for (; b < 8; b++)
    {
        if  (p < e && *p == ':')
        {
            break;
        }
        if  (b == 6 && (Flags & 4))
        {
            //_byte z[16];
            //memset(z, 0, 16);
            //z[10] = z[11] = 0xff;
            //if (!memcmp(ip, z, 12))
            if  (CMaaIsMemEq(ip, gIp6_v4Supp.const_ptr(), 12))
            {
                _IP v4 = 0;
                int r = CMaaIpToLongEx(p, len - (int)(p - txt), &v4, !ce ? AvailableTrailers : "]");
                if  (r >= 0)
                {
                    v4 = htonl(v4);
                    memcpy(ip + 12, &v4, 4);
                    b += 2;
                    p += r;
                    e = p;
                    break;
                }
            }
        }
        {
            int x = 0;
            pp = p;
            int f = 0;
            while(pp < e && ((*pp >= '0' && *pp <= '9') || (*pp >= 'a' && *pp <= 'f' && f != 2) || (*pp >= 'A' && *pp <= 'F' && f != 1)))
            {
                if  (*pp >= '0' && *pp <= '9')
                {
                    x = (x << 4) + (*pp - '0');
                }
                else if (*pp >= 'a' && *pp <= 'f')
                {
                    x = (x << 4) + (*pp - 'a' + 10);
                    f |= 1;
                }
                else if (*pp >= 'A' && *pp <= 'F')
                {
                    x = (x << 4) + (*pp - 'A' + 10);
                    f |= 2;
                }
                pp++;
            }
            if  (p == pp || pp > p + 4 || (b < 7 && (pp >= e || *pp != ':')))
            {
                RET_ERR(-3);
            }
            if  (b == 7 && pp != e)
            {
                if  (ce)
                {
                    RET_ERR(-33);
                }
                e = pp;
            }
            ip[2 * b] = (_byte)(x >> 8);
            ip[2 * b + 1] = (_byte)x;
            p = pp + 1;
        }
    }
    if  (p < e && *p == ':')
    {
        if  (b >= 7)
        {
            RET_ERR(-20);
        }
        ip[2 * b] = ip[2 * b + 1] = 0;
        b++;
        p++;
        if  (p > e)
        {
            RET_ERR(-4);
        }
        if  (b == 1)
        {
            if  (p < e && *p == ':')
            {
                p++;
            }
            else
            {
                RET_ERR(-5);
            }
        }
    }
    if  (b != 8)
    {
        int b2 = b + 1;
        if  (b2 >= 8)
        {
            if  (p < e)
            {
                if  (ce)
                {
                    RET_ERR(-11);
                }
                e = p;
            }
        }
        for (; b2 < 8 && p <= e; b2++)
        {
            //if (p < e && *p == ':')
            //{
            //RET_ERR(-6);
            //}
            if  (b2 >= b + 2 && b2 <= 6 && (Flags & 4))
            {
                if  (CMaaIsMemEq(ip, 12 - 2 * b2 + gIp6_v4Supp.const_ptr(), 2 * b2))
                {
                    _IP v4 = 0;
                    int r = CMaaIpToLongEx(p, len - (int)(p - txt), &v4, !ce ? AvailableTrailers : "]");
                    if  (r >= 0)
                    {
                        v4 = htonl(v4);
                        memcpy(ip + 2 * b2, &v4, 4);
                        b2 += 2;
                        e = p + r;
                        break;
                    }
                }
            }
            {
                int x = 0;
                pp = p;
                int f = 0;
                while(pp < e && ((*pp >= '0' && *pp <= '9') || (*pp >= 'a' && *pp <= 'f' && f != 2) || (*pp >= 'A' && *pp <= 'F' && f != 1)))
                {
                    if  (*pp >= '0' && *pp <= '9')
                    {
                        x = (x << 4) + (*pp - '0');
                    }
                    else if (*pp >= 'a' && *pp <= 'f')
                    {
                        x = (x << 4) + (*pp - 'a' + 10);
                        f |= 1;
                    }
                    else if (*pp >= 'A' && *pp <= 'F')
                    {
                        x = (x << 4) + (*pp - 'A' + 10);
                        f |= 2;
                    }
                    pp++;
                }
                if  (p == pp && !ce)
                {
                    e = b2 == b + 1 ? pp : pp - 1;
                    break;
                }
                if  (p == pp && ce && b2 == b + 1 && p >= e)
                {
                    break;
                }
                if  (p == pp || pp > p + 4)
                {
                    RET_ERR(-7);
                }
                ip[2 * b2] = (_byte)(x >> 8);
                ip[2 * b2 + 1] = (_byte)x;
                if  ((b2 < 7 && pp < e && *pp != ':') || (b2 == 7 && pp != e))
                {
                    if  (ce)
                    {
                        RET_ERR(-8);
                    }
                    e = pp;
                    b2++;
                    break;
                }
                p = pp + 1;
            }
        }
        memmove(ip + 16 - 2 * (b2 - b - 1), ip + 2 * (b + 1), 2 * (b2 - b - 1));
        memset(ip + 2 * b, 0, 2 * (8 - b2 + 1));
    }
    e = ce ? e + 1 : e;
    if  (e < txt + len && !strchr(AvailableTrailers, *e))
    {
        RET_ERR(-10);
    }
    memcpy(RetIp, ip, 16);
    return (int)(size_t)(e - txt);         // All Ok
#undef RET_ERR
}
//---------------------------------------------------------------------------
// returns 0 -- means: Ok, * RetIp = converted ip
int CMaaIpToLong6( const char * txt, _byte * RetIp ) noexcept
{
    return CMaaIpToLongEx(txt, RetIp, "") >= 0 ? 0 : -1;
}
//---------------------------------------------------------------------------
// returns 0 -- means: Ok, * RetIp = converted IPv4 or IPv6 address
// for IPv4, * RetIp is ::ffff:xx.xx.xx.xx
int CMaaIpToLong46(const char * txt, _byte * RetIp) noexcept
{
    return CMaaIpToLongEx46(txt, RetIp, "") >= 0 ? 0 : -1;
}
// returns len of ip text, -1 on error, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4-1.2.3.55" is converted to 0x01020304, ret=7
// for IPv4, * RetIp is ::ffff:xx.xx.xx.xx
int CMaaIpToLongEx46(const char * txt, _byte * RetIp, const char * AvailableTrailers) noexcept
{
    _IP v4 = 0;
    const int r = CMaaIpToLongEx(txt, &v4, AvailableTrailers);
    if  (r >= 0)
    {
        //_byte ip[16];
        //memset(ip, 0, 10);
        //ip[10] = ip[11] = 0xff;
        const _IP6 ip(v4);
        ip.Store(RetIp);
        //_IP6 ip(gIp6_v4Supp);
        //v4 = htonl(v4);
        //memcpy(ip.ptr() + 12, &v4, 4);
        //memcpy(RetIp, ip.ptr(), 16);
        return r;
    }
    return CMaaIpToLongEx(txt, RetIp, AvailableTrailers);
}

//---------------------------------------------------------------------------
//#define ib( x ) ( ( int )( ip >> x ) & 0xff )
#define byte_b(x) ((_byte)(ip >> x))
//---------------------------------------------------------------------------
static char* Ipv4ByteToText(char* txt, _byte x, char e) noexcept
{
    if  (x >= 100)
    {
        *txt++ = (x / 100) + '0';
    }
    if  (x >= 10)
    {
        *txt++ = ((x / 10) % 10) + '0';
    }
    *txt++ = (x % 10) + '0';
    *txt++ = e;
    return txt;
}
//---------------------------------------------------------------------------
static char* Ipv4ByteToText000(char* txt, _byte x, char e) noexcept
{
    *txt++ = (x / 100) + '0';
    *txt++ = ((x / 10) % 10) + '0';
    *txt++ = (x % 10) + '0';
    *txt++ = e;
    return txt;
}
char * CMaaIpToTextEx(char * txt, _IP ip, int Mode) noexcept
{
    //static char txt [ 20 ];
    char* txt0 = txt;
    if  (!Mode)
    {
        //sprintf ( txt, "%d.%d.%d.%d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
        txt = Ipv4ByteToText(txt, byte_b(24), '.');
        txt = Ipv4ByteToText(txt, byte_b(16), '.');
        txt = Ipv4ByteToText(txt, byte_b(8), '.');
        /*txt =*/ Ipv4ByteToText(txt, byte_b(0), 0);
    }
    else
    {
        //sprintf ( txt, "%03d.%03d.%03d.%03d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
        txt = Ipv4ByteToText000(txt, byte_b(24), '.');
        txt = Ipv4ByteToText000(txt, byte_b(16), '.');
        txt = Ipv4ByteToText000(txt, byte_b(8), '.');
        /*txt =*/ Ipv4ByteToText000(txt, byte_b(0), 0);
    }
    return txt0;
}
//---------------------------------------------------------------------------
#ifndef TOOLSLIB_DELETE_UNSAFE
const char* CMaaIpToText(_IP ip, int Mode) noexcept
{
    static char txt[20];
    return CMaaIpToTextEx(txt, ip, Mode);
}
#endif
//---------------------------------------------------------------------------
char * CMaaIpToText( char * txt, _IP ip, int Mode) noexcept
{
    char* txt0 = txt;
    if  ( txt )
    {
        if  ( ! Mode )
        {
            //sprintf ( txt, "%d.%d.%d.%d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
            txt = Ipv4ByteToText(txt, byte_b(24), '.');
            txt = Ipv4ByteToText(txt, byte_b(16), '.');
            txt = Ipv4ByteToText(txt, byte_b(8), '.');
            /*txt =*/ Ipv4ByteToText(txt, byte_b(0), 0);
        }
        else
        {
            //sprintf ( txt, "%03d.%03d.%03d.%03d", ib ( 24 ), ib ( 16 ), ib ( 8 ), ib ( 0 ) );
            txt = Ipv4ByteToText000(txt, byte_b(24), '.');
            txt = Ipv4ByteToText000(txt, byte_b(16), '.');
            txt = Ipv4ByteToText000(txt, byte_b(8), '.');
            /*txt =*/ Ipv4ByteToText000(txt, byte_b(0), 0);
        }
    }
    return txt0;
}
//#undef ib
#undef byte_b
//---------------------------------------------------------------------------
// returns 0 -- means: Ok, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4" is converted to 0x01020304
int CMaaIpToLong(const char * txt, _IP * RetIp) noexcept
{
    _IP ip = 0, tmp;
    int i;

    for (i = 0; i < 4; i++)
    {
        tmp = 0;
        if  (txt[0] < '0' || txt[0] > '9')
        {
            return -1;     // …££®£
        }
        while(txt[0] >= '0' && txt[0] <= '9')
        {
            tmp = tmp * 10 + (_dword)(txt[0] - '0');
            if  (tmp > 0xff)
            {
                return -1;
            }
            ++txt;
        }
        if  (txt[0] != '.' && i < 3)
        {
            return -1;
        }
        if  (txt[0] != 0 && i == 3)
        {
            return -1;
        }
        // Which byte-ordering do You need ?
        //ip = ( ip >> 8 ) + ( tmp << 24 ); // Network byte order // for Intel-like processors with low-to-hight byte ordering
        ip = (ip << 8) + tmp;
        ++txt;
    }
    * RetIp = ip;
    return 0;         // All Ok
}
//---------------------------------------------------------------------------
/*
_IP CMaaIpAddress(_byte a, _byte b, _byte c, _byte d) noexcept
{
    return ((_IP)(a) << 24) | ((_IP)(b) << 16) | ((_IP)(c) << 8) | (_IP)(d);
}
*/
//---------------------------------------------------------------------------
// returns len of ip text, -1 on error, * RetIp = converted ip ( machine-depended byte order )
// example: for x86 platform "1.2.3.4-1.2.3.55" is converted to 0x01020304, ret=7
int CMaaIpToLongEx(const char *txt, _IP *RetIp, const char *AvailableTrailers) noexcept
{
    return CMaaIpToLongEx(txt, -1, RetIp, AvailableTrailers);
}
int CMaaIpToLongEx(const char *txt, int len, _IP *RetIp, const char *AvailableTrailers) noexcept
{
    _IP ip = 0, tmp;
    const char *txt0 = txt;

    for (int i = 0; ; i++)
    {
        if  (len >= 0 && txt >= txt0 + len)
        {
            return -1;
        }
        tmp = 0;
        char c = *txt;
        if  (c < '0' || c > '9')
        {
            return -1;     // …££®£
        }
        while(len < 0 || txt < txt0 + len)
        {
            c = *txt;
            if  (c < '0' || c > '9')
            {
                break;
            }
            tmp = tmp * 10 + (_IP)(c - '0');
            if  (tmp > 0xff)
            {
                return -1;
            }
            ++txt;
        }
        ip = (ip << 8) + tmp;
        if  (i < 3)
        {
            if  (len >= 0 && txt >= txt0 + len)
            {
                return -1;
            }
            if  (txt[0] != '.')
            {
                return -1;
            }
            ++txt;
        }
        if  (i == 3)
        {
            if  (len >= 0 && txt < txt0 + len && !strchr(AvailableTrailers, txt[0]))
            {
                return -1;
            }
            if  (len < 0 && txt[0] && !strchr(AvailableTrailers, txt[0]))
            {
                return -1;
            }
            break;
        }
    }
    *RetIp = ip;
    return (int)(txt - txt0); // All Ok
}
//---------------------------------------------------------------------------
_IP CMaaGetMaskByNum(int Num) noexcept
{
    return Num && (unsigned)Num <= 32U ? ((_IP)-1) << (32 - Num) : 0;
    /*
    if (!Num)
    {
        return 0;
    }
    return ~ ((1 << (32 - Num)) - 1);
    */
}
//---------------------------------------------------------------------------
int CMaaGetNumByMask(_IP Mask) noexcept
{
    _IP m = 0;
    for (int i = 0; i <= 32; i++)
    {
        if  (m == Mask)
        {
            return i;
        }
        m = (m >> 1) | 0x80000000;
    }
    return -1;
}
//---------------------------------------------------------------------------

_IP6::_IP6() noexcept
{
    Zero();
}
_IP6::_IP6(eTmpB e) noexcept
{
    if  (e >= eInitByZero && e <= eInitBy255)
    {
        //Zero();
        //m[15] = (_byte)e;
        *this = (_byte)e;
    }
    /*else if (e == eV4Supp)
    {
        Zero();
        m[10] = m[11] = 0xff;
    }
    else if (e == eAllNodesMc)
    {
        Zero();
        m[0] = 0xff;
        m[1] = 0x02;
        m[15] = 0x01;
    }
    else if (e == eAllRoutersMc)
    {
        Zero();
        m[0] = 0xff;
        m[1] = 0x02;
        m[15] = 0x02;
    }
    else if (e == eInitByFaked1111)
    {
        Zero();
        m[0] = 0x20;
        m[14] = m[15] = 0x11;
    }*/
}
_IP6::_IP6(eTmp4) noexcept
{
    Zero();
    m[10] = m[11] = 0xff;
}
_IP6::_IP6(eTmp1111) noexcept
{
    Zero();
    m[0] = 0x20;
    m[14] = m[15] = 0x11;
}
_IP6::_IP6(eTmpAllNMc) noexcept
{
    Zero();
    m[0] = 0xff;
    m[1] = 0x02;
    m[15] = 0x01;
}
_IP6::_IP6(eTmpAllRMc) noexcept
{
    Zero();
    m[0] = 0xff;
    m[1] = 0x02;
    m[15] = 0x02;
}
_IP6::_IP6(bool, int MaskNum) noexcept
{
    GetMask(MaskNum);
}
_IP6::_IP6(const _byte *ip) noexcept
{
    *(_qword*)m = *(_qword*)ip;
    *(1 + (_qword*)m) = *(1 + (_qword*)ip);
    //memcpy(m, ip, sizeof(m));
}
void _IP6::Store(_byte *ip) const noexcept
{
    *(_qword*)ip = *(_qword*)m;
    *(1 + (_qword*)ip) = *(1 + (_qword*)m);
    //memcpy(ip, m, sizeof(m));
    //return ip;
}
_IP6::_IP6(_IP ip) noexcept
{
    //memset(m, 0, 10);
    //m[10] = m[11] = 0xff;
    *this = gIp6_v4Supp;
    *(_IP*)(m + 12) = htonl(ip);
}
_IP6 & _IP6::Zero() noexcept
{
    *(1 + (_qword *)m) = *(_qword*)m = 0;
    //memset(m, 0, sizeof(m));
    return *this;
}
_byte * _IP6::ptr() noexcept
{
    return m;
}
const _byte * _IP6::const_ptr() const noexcept
{
    return m;
}
_IP _IP6::v4() const noexcept
{
    return ntohl(*(_IP *)&m[12]);
}
_IP _IP6::v4(bool * pIsOk) const noexcept
{
    if  (pIsOk)
    {
        //_byte z[12];
        //memset(z, 0, 10);
        //z[10] = z[11] = 0xff;
        //*pIsOk = !memcmp(m, z, 12);
        *pIsOk = CMaaIsMemEq(m, gIp6_v4Supp.const_ptr(), 12);
    }
    return v4();
}
_IP6::_IP6(const _IP6 &That) noexcept
{
    *(_qword *)m = *(_qword *)That.m;
    *(1 + (_qword *)m) = *(1 + (_qword *)That.m);
    //memcpy(m, That.m, sizeof(m));
}
_IP6 & _IP6::operator =(const _IP6 &That) noexcept
{
    *(_qword *)m = *(_qword *)That.m;
    *(1 + (_qword *)m) = *(1 + (_qword *)That.m);
    //memcpy(m, That.m, sizeof(m));
    return *this;
}
bool _IP6::operator ==(const _IP6 &That) const noexcept
{
    return *(_qword *)m == *(_qword *)That.m && *(1 + (_qword *)m) == *(1 + (_qword *)That.m);
    //return !memcmp(m, That.m, sizeof(m));
}
bool _IP6::operator !=(const _IP6 &That) const noexcept
{
    return *(_qword *)m != *(_qword *)That.m || *(1 + (_qword *)m) != *(1 + (_qword *)That.m);
    //return !(*this  == That);
    //return memcmp(m, That.m, sizeof(m));
}
/*
    if   (!Num)
    {
        return 0;
    }
    return ~ ((1 << (32 - Num)) - 1);
*/
_IP6 & _IP6::GetMask(int Mask) noexcept
{
    if  (Mask >= (int)(sizeof(m) * 8))
    {
        _qword _1 = -1;
        *(_qword *)m = *(1 + (_qword *)m) = _1;
        //memset(m, -1, sizeof(m));
        return *this;
    }
    if  (Mask < 0)
    {
        Zero();
        return *this;
    }
    const int byte = Mask / 8;
    const int bit = Mask % 8;
    const _byte m10 = ~ (0xff >> bit);
    memset(m, -1, byte);
    if  (!bit)
    {
        memset(m + byte, 0, (int)sizeof(m) - byte);
    }
    else
    {
        m[byte] = m10;
        byte + 1 < (int)sizeof(m) && memset(m + byte + 1, 0, (int)sizeof(m) - byte - 1);
    }
    return *this;
}
_IP6 & _IP6::Invert() noexcept
{
    const _qword _1 = -1;
    *(_qword *)m ^= _1;
    *(1 + (_qword *)m) ^= _1;
    /*
    for (int i = 0; i < (int)sizeof(m); i++)
    {
        m[i] ^= 0xff;
    }
    */
    return *this;
}
_IP6 & _IP6::operator &=(const _IP6 &That) noexcept
{
    *(_qword *)m &= *(_qword *)That.m;
    *(1 + (_qword *)m) &= *(1 + (_qword *)That.m);
    /*
    for (int i = 0; i < (int)sizeof(m); i++)
    {
        m[i] &= That.m[i];
    }
    */
    return *this;
}
_IP6 & _IP6::operator |=(const _IP6 &That) noexcept
{
    *(_qword *)m |= *(_qword *)That.m;
    *(1 + (_qword *)m) |= *(1 + (_qword *)That.m);
    /*
    for (int i = 0; i < (int)sizeof(m); i++)
    {
        m[i] |= That.m[i];
    }
    */
    return *this;
}
/*
_IP6 & _IP6::SetMask(int Mask)
{
    int maxmask = (int)(sizeof(m) * 8);
    if (Mask >= maxmask || Mask < 0)
    {
        return *this;
    }
    //Mask = Mask % 128;
    int byte = Mask / 8;
    int bit = (maxmask - Mask) % 8;
    int m01 = ((1 << bit) - 1);
    m[byte] |= m01;
    for (int i = byte + 1; i < (int)sizeof(m); i++)
    {
        m[i] = 0xff;
    }
    return *this;
}
_IP6 & _IP6::ClearMask(int Mask)
{
    int maxmask = (int)(sizeof(m) * 8);
    if (Mask >= maxmask || Mask < 0)
    {
        return Zero();
    }
    //Mask = Mask % 128;
    int byte = Mask / 8;
    int bit = (maxmask - Mask) % 8;
    int m01 = ((1 << bit) - 1);
    m[byte] &= ~ m01;
    for (int i = byte + 1; i < (int)sizeof(m); i++)
    {
        m[i] = 0;
    }
    return *this;
}
*/
int _IP6::GetMaskNum() const noexcept
{
    int b1, b2;
    for (b1 = 0; b1 < (int)sizeof(m); b1++)
    {
        if  (m[b1] != 0xff)
        {
            break;
        }
    }
    for (b2 = (int)sizeof(m) - 1; b2 >= 0; b2--)
    {
        if  (m[b2] != 0)
        {
            break;
        }
    }
    if  (b2 < b1)
    {
        return b1 * 8;
    }
    if  (b1 == b2)
    {
        for (int b = 1; b < 8; b++)
        {
            const _byte m10 = ~ (0xff >> b);
            if  (m[b1] == m10)
            {
                return b1 * 8 + b;
            }
        }
    }
    return -1;
}
int _IP6::Compare(const _IP6 &That) const noexcept
{
    return memcmp(m, That.m, sizeof(m));
}
bool _IP6::operator < (const _IP6 &That) const noexcept
{
    return memcmp(m, That.m, sizeof(m)) < 0;
    //return Compare(That) < 0;
}
bool _IP6::operator <= (const _IP6 &That) const noexcept
{
    return memcmp(m, That.m, sizeof(m)) <= 0;
    //return Compare(That) <= 0;
}
bool _IP6::operator > (const _IP6 &That) const noexcept
{
    return memcmp(m, That.m, sizeof(m)) > 0;
    //return Compare(That) > 0;
}
bool _IP6::operator >= (const _IP6 &That) const noexcept
{
    return memcmp(m, That.m, sizeof(m)) >= 0;
    //return Compare(That) >= 0;
}
_IP6& _IP6::operator =(_byte x) noexcept
{
    Zero();
    m[15] = x;
    return *this;
}
/*_IP6& _IP6::operator =(_IP x) noexcept
{
    return (*this = _IP6(x));
}*/
_IP6& _IP6::operator =(const _byte* ip) noexcept
{
    *(_qword*)m = *(_qword*)ip;
    *(1 + (_qword*)m) = *(1 + (_qword*)ip);
    //memcpy(m, ip, sizeof(m));
    return *this;
}
int _IP6::Compare(const _byte * ip) const noexcept
{
    return memcmp(m, ip, sizeof(m));
}
bool _IP6::operator ==(const _byte * ip) const noexcept
{
    return *(_qword *)m == *(_qword *)ip && *(1 + (_qword *)m) == *(1 + (_qword *)ip);
    //return !memcmp(m, ip, sizeof(m));
}
bool _IP6::operator !=(const _byte * ip) const noexcept
{
    return *(_qword *)m != *(_qword *)ip || *(1 + (_qword *)m) != *(1 + (_qword *)ip);
    //return memcmp(m, ip, sizeof(m));
}
bool _IP6::operator < (const _byte * ip) const noexcept
{
    return memcmp(m, ip, sizeof(m)) < 0;
    //return Compare(ip) < 0;
}
bool _IP6::operator <= (const _byte * ip) const noexcept
{
    return memcmp(m, ip, sizeof(m)) <= 0;
    //return Compare(ip) <= 0;
}
bool _IP6::operator > (const _byte * ip) const noexcept
{
    return memcmp(m, ip, sizeof(m)) > 0;
    //return Compare(ip) > 0;
}
bool _IP6::operator >= (const _byte * ip) const noexcept
{
    return memcmp(m, ip, sizeof(m)) >= 0;
    //return Compare(ip) >= 0;
}
_IP6 & _IP6::operator &= (const _byte * ip) noexcept
{
    *(_qword *)m &= *(_qword *)ip;
    *(1 + (_qword *)m) &= *(1 + (_qword *)ip);
    /*
    for (int i = 0; i < (int)sizeof(m); i++)
    {
        m[i] &= ip[i];
    }
    */
    return *this;
}
_IP6 & _IP6::operator |= (const _byte * ip) noexcept
{
    *(_qword *)m |= *(_qword *)ip;
    *(1 + (_qword *)m) |= *(1 + (_qword *)ip);
    /*
    for (int i = 0; i < (int)sizeof(m); i++)
    {
        m[i] |= ip[i];
    }
    */
    return *this;
}
_IP6 & _IP6::operator++() noexcept
{
    for (int i = 15; i >= 0; i--)
    {
        if (++m[i])
        {
            break;
        }
    }
    return *this;
}
_IP6 & _IP6::operator--() noexcept
{
    for (int i = 15; i >= 0; i--)
    {
        if (--m[i] < (_byte)255)
        {
            break;
        }
    }
    return *this;
}
_dword _IP6::Hash() const noexcept
{
    const _dword* p = (const _dword*)m;
    return p[0] ^ p[1] ^ p[2] ^ p[3];
}
_uqword _IP6::Hash64() const noexcept
{
    const _uqword* p = (const _uqword*)m;
    return p[0] ^ p[1];
}
void _IP6::Swap(_IP6 &That) noexcept
{
    _IP6 tmp(*this);
    *this = That;
    That = tmp;
}
#if 0
enum eType
{
    eUndefined = 0, //eUnspecified = eUndefined, // ::0
    eLoopback = 1, // ::1
    eMulticast = 2, // group addr // FF00::/8
    eLinkLocalUnicast = 3, // FE80::/10
    eGlobalUnicast = 4 // other
};
#endif
_IP6::eType _IP6::GetType() const noexcept
{
    if  (IsMulticast())
    {
        return eMulticast;
    }
    if  (IsLinkLocalUnicast())
    {
        return eLinkLocalUnicast;
    }
    if  (!m[0])
    {
        if  (IsUndefined())
        {
            return eUndefined;
        }
        if  (IsLoopback())
        {
            return eLoopback;
        }
    }
    return eGlobalUnicast;
}
bool _IP6::IsUndefined() const noexcept
{
    return *this == gIp6_0;
}
bool _IP6::IsLoopback() const noexcept
{
    return *this == gIp6_1;
}
bool _IP6::IsMulticast() const noexcept
{
    return m[0] == 0xff;
}

bool _IP6::IsLinkLocalUnicast() const noexcept
{
    return ((((_word)(m[0]) << 8) | (_word)m[1]) & 0xFFC0) == 0xFE80;
}
bool _IP6::IsGlobalUnicast() const noexcept
{
    return !IsMulticast() && !IsLinkLocalUnicast() && (m[0] || (!IsUndefined() && !IsLoopback()));
}
bool _IP6::Isv4() const noexcept
{
    //_byte z[12];
    //memset(z, 0, 10);
    //z[10] = z[11] = 0xff;
    //return !memcmp(m, z, 12);
    return CMaaIsMemEq(m, gIp6_v4Supp.const_ptr(), 12);
}
bool _IP6::Isv4(_IP *pRetIp) const noexcept
{
    /*
    if (Isv4())
    {
        if (pRetIp)
        {
            *pRetIp = ntohl(*(_IP*)&m[12]);
        }
        return true;
    }
    return false;
    */
    if  (pRetIp)
    {
        *pRetIp = ntohl(*(_IP*)&m[12]);
    }
    return Isv4();
}
bool _IP6::GenMulticastMac(void *pMac) const noexcept
{
    if  (IsMulticast())
    {
        _byte* p = (_byte*)pMac;
        p[0] = p[1] = 0x33;
        *(_IP *)(p + 2) = *(_IP *)(m + 12);
        return true;
    }
    return false;
}
char * _IP6::ToString(char *pstr) const noexcept
{
    return CMaaIpToTextEx(pstr, (_byte *)m);
}
CMaaString _IP6::ToString() const
{
    char txt[48]; // 40 is enough
    CMaaIpToTextEx(txt, (_byte *)m);
    return txt;
}
bool _IP6::FromString(const char *pstr) noexcept
{
    //return !CMaaIpToLong6(pstr, m);
    return CMaaIpToLongEx(pstr, m, "") >= 0;
}
bool _IP6::FromString(const CMaaString &str) noexcept
{
    return CMaaIpToLongEx(str, (warning_int)str.Length(), m, "") >= 0;
}
int _IP6::FromStringEx(const char *pstr, const char * AvailableTrailers) noexcept // "-/ "
{
    return CMaaIpToLongEx(pstr, m, AvailableTrailers);
}
int _IP6::FromStringEx(const CMaaString &str, const char * AvailableTrailers) noexcept // "-/ "
{
    return CMaaIpToLongEx(str, (warning_int)str.Length(), m, AvailableTrailers);
}
template<> int CMaaXSign(const _IP6& a, const _IP6& b) noexcept
{
    return a.Compare(b);
}

#if defined(__TOOLS_USE_WINSOCK2) || defined(__unix__)

//---------------------------------------------------------------------------
// Name of system service
//---------------------------------------------------------------------------
#ifdef __unix1__
_PCSTR GetServByProtoAndPort ( const _word PortValue, _PCSTR IpProtoName )
{
    _PSTR pNameServ = nullptr;
    int Port = ( int ) htons (  ( u_short ) PortValue );
    servent * pServent = getservbyport ( Port, IpProtoName );
    if  ( pServent )
    {
        if  ( pServent -> s_name )
        {
            pNameServ = pServent -> s_name;
        }
    }

    return pNameServ;
}
#endif // #ifdef __unix1__

static _qword __GetTime0AndResetHrt(CMaaTime &hrTimer) noexcept
{
    hrTimer.StartWatch();
    return GetCurrentTime_us();
}
//---------------------------------------------------------------------------
#ifndef TOOLSLIB_SINGLE_THREAD
_qword CMaaFdSocket::UpdateHrTime(int index) noexcept
{
    static CMaaTime hrTimer(GetHRTime());
    static _qword t0 = __GetTime0AndResetHrt(hrTimer);
    if  ((unsigned)index < e_fds_max_time)
    {
        return m_hrTime[index] = hrTimer.GetTime(1000000) + t0;
    }
    if (index == e_fds_current_time)
    {
        return hrTimer.GetTime(1000000) + t0;
    }
    return -1;
}
_qword CMaaFdSocket::SetHrTime(int index, _qword t) noexcept
{
    if  ((unsigned)index < e_fds_max_time)
    {
        return m_hrTime[index] = t;
    }
    return -1;
}
_qword CMaaFdSocket::GetHrTime_(int index) noexcept
{
    if  ((unsigned)index < e_fds_max_time)
    {
        return m_hrTime[index];
    }
    //if (index == e_fds_current_time)
    //{
        return UpdateHrTime(e_fds_current_time);
    //}
    //return -1;
}
_qword CMaaFdSocket::GetHrTime(int index, int RelatedTo, bool /*bAbsoluteTime*/, int factor) noexcept
{
    const _qword t2 = GetHrTime_(RelatedTo);
    if  (t2 <= 0)
    {
        return -1;
    }
    _qword t = GetHrTime_(index);
    if  ((t -= t2) < 0)
    {
        return -1;
    }
    /*
     if   (bAbsoluteTime)
     {
          t += m_AbsTimeCreated;
     }
    */
    if  (factor == 1000000)
    {
    }
    else if (factor == 1000)
    {
        t /= 1000;
    }
    else if (factor == 1)
    {
        t /= 1000000;
    }
    else
    {
        t = MaaMulDiv64(t, factor, 1000000);
    }
    return t;
}
CMaaFdSocket::CMaaFdSocket(CMaaFdSockets * pFdSockets) noexcept
:   m_pFdSockets (pFdSockets)
{
    //m_Act1 = m_Mode = 0;
    for (int i = 0; i < e_fds_max_time; i++)
    {
        m_hrTime[i] = -1;
    }
#ifdef _WIN32
    //m_LastMode = -1;
    m_Ev = WSACreateEvent();
#endif
    //m_AcceptFlag = false;
    m_AbsTimeCreated = GetCurrentTime_us();
    UpdateHrTime(e_fds_created_time);
}
//---------------------------------------------------------------------------
CMaaFdSocket::~CMaaFdSocket()
{
#ifdef _WIN32
    WSACloseEvent(m_Ev);
#endif
}
//---------------------------------------------------------------------------
void CMaaFdSocket::SetFdSockets(CMaaFdSockets * pFdSockets) noexcept
{
    m_pFdSockets = pFdSockets;
}
//---------------------------------------------------------------------------
void CMaaFdSocket::SetFdSockets(CMaaFdSocket * FdSocket) noexcept
{
    m_pFdSockets = FdSocket->m_pFdSockets;
}
//---------------------------------------------------------------------------
void CMaaFdSocket::ChangeFdMode(int Act) noexcept
{
    DP(printf("ChangeFdMode(%x) / %d\n", Mode, ((CMaaSocket *)this)->m_Socket);)
    m_Act1 = CMaaFdSockets::ActAct(m_Act1, Act);
    if (m_Act1 && m_pFdSockets)
    {
        m_pFdSockets->OnModeChangeDelayed(this);
    }
}
//---------------------------------------------------------------------------
int CMaaFdSocket::GetFdMode() const noexcept
{
    if  (m_pFdSockets)
    {
        return m_pFdSockets->GetMode((CMaaSocket *)this);
    }
    return 0;
}
//---------------------------------------------------------------------------
void CMaaFdSocket::ChangeFdModeEx(int Act, bool bForced) noexcept
{
    DP(printf("ChangeFdMode(%x) / %d\n", Mode, ((CMaaSocket*)this)->m_Socket);)
    const int Act1 = m_Act1;
    m_Act1 = 0;
    if (m_pFdSockets)
    {
        m_pFdSockets->ChangeMode((CMaaSocket*)this, Act1, Act, bForced);
    }
    else
    {
        DP(printf("!m_pFdSockets\n");)
    }
}
//---------------------------------------------------------------------------
CMaaSocket::CMaaSocket(CMaaFdSockets * pFdSockets, int af, int type, int protocol )
:   CMaaFdSocket(pFdSockets)
    //, m_Socket(INVALID_SOCKET)
    //, m_domain(-1)
#if defined(TL_EPOLL) || defined(_WIN32)
    //, m_RealState(0)
#endif
{
#ifdef TL_EPOLL
    //m_ActiveState = 0;
#endif
    m_ModeChangeItem.m_pItem = m_ListItem.m_pItem = this;
#ifdef TL_EPOLL
    m_ActiveStateListItem.m_pItem = this;
#endif
    //m_bIsClosing = false;
    if  ((m_Socket = socket(m_domain = af, type, protocol)) == INVALID_SOCKET)
    {
        Error("socket()");
    }
    //m_domain = af; //AF_INET6;
}
//---------------------------------------------------------------------------
CMaaSocket::~CMaaSocket()
{
    //m_pTcpSocketCast = nullptr;
    m_TcpSocketFlag = false;
    DP(printf("Closing socket(~) %d %p\n", m_Socket, this);)
    RemoveFdSocket(true);
    if  (m_Socket != INVALID_SOCKET)
    {
        //shutdown(m_Socket, 0);
        closesocket(m_Socket);
    }
}
//---------------------------------------------------------------------------
void CMaaSocket::Error(const char * msg)
{
    //CMaaString Msg;
    //int Error = WSAGetLastError ();
    //
    //Msg.Format(IDS_XTOOSockError, msg, Error);
    //throw XTOOSocketError(Msg, Error, DelObject);
    //throw msg;
    throw XTOOSockErr(msg, nullptr, GetConnectionName());
    //CloseByException((char *)msg);
}
//---------------------------------------------------------------------------
void CMaaSocket::StaticError(const char * msg)
{
    throw XTOOSockErr(msg, nullptr, nullptr);
}
//---------------------------------------------------------------------------
int CMaaSocket::GetDomainSock() const noexcept
{
    return m_domain;
}
//---------------------------------------------------------------------------
SOCKET CMaaSocket::GetSocket() const noexcept
{
    return m_Socket;
}
//---------------------------------------------------------------------------
void CMaaSocket::SetSocket(SOCKET s, int domain) noexcept
{
    ChangeFdModeEx(eDisableAll);
    if  (m_Socket != INVALID_SOCKET)
    {
        closesocket(m_Socket);
    }
    m_Socket = s;
    if  (domain != -1)
    {
        m_domain = domain;
    }
#ifdef _WIN32
    m_LastMode = -1;
#endif
}
//---------------------------------------------------------------------------
void CMaaSocket::SwapSockets(SOCKET &s, int &d) noexcept // domain
{
    SOCKET s1 = m_Socket;
    m_Socket = s;
    s = s1;

    int d1 = m_domain;
    m_domain = d;
    d = d1;

#ifdef _WIN32
    m_LastMode = -1;
#endif
}
//---------------------------------------------------------------------------
#ifndef TOOLSLIB_USE_IPV6
void CMaaSocket::SwapSockets(SOCKET &s) noexcept // domain
{
    int d = m_domain;
    SwapSockets(s, d);
}
#endif // #ifndef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaSocket::CloseByException(const char * msg)
{
    //printf("CMaaSocket::CloseByExeption('%s') %s\n", msg, (const char *)GetConnectionName()); fflush(stdout);

    Notify_PreClosingByException(msg);
    // ending connection
    //DeleteSocket Err ( msg, this );
    //throw Err;
    throw XTOOSockErr(msg, this, GetConnectionName());
    return eDisableAll;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_PreDeletingByException()
{
    return 0;
}
//---------------------------------------------------------------------------
void CMaaSocket::AddFdSocket()
{
    //    if (dynamic_cast<CMaaTcpSocket *>(this))
#ifdef TL_EPOLL
#ifndef TL_EPOLL_SOCKETPAIR
    if  (dynamic_cast<CMaaWakeUpPair *>(this))
    {
        //m_pFdSockets->AddSocket(this);
        m_pFdSockets->AddSocket(this, eAll, EPOLLIN);
    }
    else
#endif
    if  (dynamic_cast<CMaaUdpServer *>(this)) //dyn/static not tested EPOLL // CONTINUE DEV
    {
#ifdef TL_UDP_EPOLLET
        m_pFdSockets->AddSocket(this, eAll/*|eNoLET*/, EPOLLIN|EPOLLOUT/*!!!2022!!!*/|EPOLLERR);
#else
        m_pFdSockets->AddSocket(this, eAll|eNoLET, EPOLLIN|EPOLLOUT/*!!!2020!!!*/|EPOLLERR);
#endif
    }
    else if(dynamic_cast<CMaaUdpSocket *>(this)) //dyn/static not tested EPOLL // CONTINUE DEV
    {
#ifdef TL_UDP_EPOLLET
        m_pFdSockets->AddSocket(this, eExcept/*|eNoLET*/, /*EPOLLIN|*/EPOLLERR);
#else
        m_pFdSockets->AddSocket(this, eExcept|eNoLET, /*EPOLLIN|*/EPOLLERR);
#endif
    }
    else
#endif
    {
        m_pFdSockets->AddSocket(this);
    }
}
//---------------------------------------------------------------------------
void CMaaSocket::RemoveFdSocket(bool bFromDestructor) noexcept
{
    m_pFdSockets->RemoveSocket(this, bFromDestructor);
}
//---------------------------------------------------------------------------
void CMaaSocket::MoveToFront() noexcept
{
    m_pFdSockets&& m_pFdSockets->MoveToFront(this);
}
//---------------------------------------------------------------------------
void CMaaSocket::MoveToTail() noexcept
{
    m_pFdSockets&& m_pFdSockets->MoveToTail(this);
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Read()
{
    return eDisableRead;
}
//---------------------------------------------------------------------------
void CMaaSocket::Notify_Act() noexcept
{
    ChangeFdModeEx(0);
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Write()
{
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Connected(_IP /*Ip*/, _Port /*Port*/, const char * /*DnsName*/)
{
    return eAll;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Connected6(_byte * /*Ip6*/, _Port /*Port*/, const char * /*DnsName*/)
{
    return eAll;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Error()
{
    return CloseByException("Some socket error occured");
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Accepted(_IP /*IpFrom*/, _Port /*Port*/)
{
    return 0;
}
//---------------------------------------------------------------------------
int CMaaSocket::Notify_Accepted6(_byte * /*Ip6From*/, _Port /*Port*/)
{
    return 0;
}
//---------------------------------------------------------------------------
CMaaString CMaaSocket::GetConnectionName() noexcept
{
    return CMaaStringZ;
}
//---------------------------------------------------------------------------
_IP CMaaSocket::GetHostByName(const char * DnsName, _Port * pPort, _IP * pAllIPs, int * pAllIPsCount)
{
    int xAI = 10;
    if  (!pAllIPsCount)
    {
        pAllIPsCount = &xAI;
    }
    if  (!pAllIPs)
    {
        *pAllIPsCount = 0;
    }
    _IP Ip = 0;
    CMaaString txt(DnsName);
    int x = CMaaIpToLongEx(txt, &Ip, ":");
    if  (x >= 0)
    {
        if  (txt[x] == ':' && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
        }
        if  (*pAllIPsCount > 1)
        {
            pAllIPs[1] = 0;
        }
        if  (*pAllIPsCount > 0)
        {
            pAllIPs[0] = Ip;
            *pAllIPsCount = 1;
        }
    }
    else
    {
        x = (warning_int)txt.ReverseFind(':');
        if  (x >= 0 && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
            txt = txt.Left(x);
        }
        // It is not numerical address as name given
        struct hostent * h = gethostbyname(txt);
        if  (!h)
        {
            txt.Format("gethostbyname(\"%S\")", &txt);
            throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
            //Error ( txt );
            return 0;
        }
        Ip = ntohl(*(_IP *)h->h_addr_list[0]);
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            pAllIPs[i] = ntohl(*(_IP *)h->h_addr_list[i]);
        }
        if  (i < *pAllIPsCount)
        {
            pAllIPs[i] = 0;
        }
        *pAllIPsCount = i;
    }
    return Ip;
}
//---------------------------------------------------------------------------
char * CMaaSocket::GetHostByAddr( _IP Ip, char * DnsName)
{
    _IP ip = htonl(Ip);
    struct hostent * h = gethostbyaddr((char*)&ip, sizeof(ip), AF_INET);
    if  ( !h )
    {
        CMaaString txt;
        txt.Format("gethostbyaddrname (\"%I\")", Ip);
        throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
        //Error ( txt );
        return 0;
    }
    strcpy(DnsName, h -> h_name);
    return DnsName;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
_byte * CMaaSocket::GetHostByName6(_byte * RetIp, const char * DnsName, _Port * pPort, _byte ** pAllIPs, int * pAllIPsCount)
{
    int xAI = 10;
    if  (!pAllIPsCount)
    {
        pAllIPsCount = &xAI;
    }
    if  (!pAllIPs)
    {
        *pAllIPsCount = 0;
    }
    CMaaString txt(DnsName);
    _byte Ip[16];
    memset(Ip, 0, sizeof(Ip));
    int x = CMaaIpToLongEx(txt, Ip, ":");
    if  (x >= 0)
    {
        if  (txt[x] == ':' && pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
        }
        if  (*pAllIPsCount > 1)
        {
            memset(pAllIPs[1], 0, 16);
        }
        if  (*pAllIPsCount > 0)
        {
            memcpy(pAllIPs[0], Ip, 16);
            *pAllIPsCount = 1;
        }
    }
    else
    {
        x = (warning_int)txt.ReverseFind(':');
        if  (x >= 0 &&  pPort)
        {
            int Port = 0;
            dsscanf(x + 1 + (const char*)txt, "%d", &Port);
            *pPort = Port;
            txt = txt.Left(x);
        }
#ifdef _WIN32
        CMaaString buffer;
        CMaaHe1 * h = CMaa_gethostbyname2(txt, buffer);
        if  (!h)
        {
            txt.Format("ipv6 CMaa_gethostbyname2(\"%S\")", &txt);
            throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
            //Error ( txt );
            return 0;
        }
        memcpy(Ip, h->h_addr_list[0], 16);
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            memcpy(pAllIPs[i], h->h_addr_list[i], 16);
        }
        if  (i < *pAllIPsCount)
        {
            memset(pAllIPs[i], 0, 16);
        }
        *pAllIPsCount = i;
#else
        // It is not numerical address as name given
        struct hostent * h = gethostbyname2(txt, AF_INET6);
        if  (!h)
        {
            txt.Format("ipv6 gethostbyname2(\"%S\")", &txt);
            throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
            //Error ( txt );
            return 0;
        }
        //int h_length; // Length of address.
        memcpy(Ip, h->h_addr_list[0], 16);
        int i;
        for (i = 0; h->h_addr_list[i] && i < *pAllIPsCount; i++)
        {
            memcpy(pAllIPs[i], h->h_addr_list[i], 16);
        }
        if  (i < *pAllIPsCount)
        {
            memset(pAllIPs[i], 0, 16);
        }
        *pAllIPsCount = i;
#endif
    }
    memcpy(RetIp, Ip, 16);
    return RetIp;
}
//---------------------------------------------------------------------------
char * CMaaSocket::GetHostByAddr6(_byte * Ip6, char * DnsName)
{
    _byte Ip[16];
    memcpy(Ip, Ip6, 16);
    struct hostent * h = gethostbyaddr((char*)Ip, sizeof(Ip), AF_INET6);
    if  ( !h )
    {
        CMaaString txt;
        txt.Format("gethostbyaddrname (\"%J\")", Ip6);
        throw XTOOSockDnsErr(txt, nullptr, GetConnectionName());
        //Error ( txt );
        return 0;
    }
    strcpy(DnsName, h->h_name);
    return DnsName;
}
#endif // #ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaSocket::Close()
{
    //shutdown(m_Socket, 0);
    throw XTOOSockClose(this);
    //return CloseByException("Closing connection");
}
//---------------------------------------------------------------------------

int gDisableNagleAlgoritm = -1;
//---------------------------------------------------------------------------
CMaaTcpSocket::CMaaTcpSocket(CMaaFdSockets * pFdSockets, int domain) // AF_INET
//:    CMaaSocket(pFdSockets, AF_INET, SOCK_STREAM, 0 )
:   CMaaSocket(pFdSockets, domain, SOCK_STREAM, IPPROTO_TCP )
#ifdef __SHAPERS
    ,
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_SndLLShaper(this),
    m_RcvLLShaper(this)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
#endif
{
    //m_pTcpSocketCast = this;
    m_TcpSocketFlag = true;
#ifdef __SHAPERS
    m_ShaperConditionLastCheckTime = 0;
#endif
#ifdef TOOLSLIB_USE_CMaaTcpSocket_ConnectionInTimeCharacteristics
    memset(m_pConnectionInTimeCharacteristics, 0, sizeof(m_pConnectionInTimeCharacteristics));
#endif

    InitCounters(false);
    memset(&m_s, 0, sizeof(m_s));

    // Variable to set the SO_KEEPALIVE option with setsockopt ().
    BOOL fKeepAlive = 1;//TRUE;

    if  (SOCKET_ERROR == setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&fKeepAlive, sizeof(BOOL)))
    {
        Error ( "setsockopt ()" );
    }

    if  (gDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gDisableNagleAlgoritm != 0);
    }

    /*
     char txt[1024];
     sprintf(txt, "CMaaTcpSocket::CMaaTcpSocket(): this = %p\n", this);
     ExtLog(txt);
    */
}
CMaaTcpSocket::~CMaaTcpSocket()
{
#ifdef TOOLSLIB_USE_CMaaTcpSocket_ConnectionInTimeCharacteristics
    DeleteConnectionInTimeCharacteristics();
#endif
}
//---------------------------------------------------------------------------
bool CMaaTcpSocket::DisableNagleAlgoritm(bool bDisable, SOCKET s) const noexcept
{
#ifdef __unix__
    int x = bDisable ? 1 : 0;
#else
    BOOL x = bDisable ? TRUE : FALSE;
#endif
    return setsockopt(s != INVALID_SOCKET ? s : m_Socket, IPPROTO_TCP, TCP_NODELAY, (char *)&x, sizeof(x)) == 0;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_CMaaTcpSocket_ConnectionInTimeCharacteristics
void CMaaTcpSocket::DeleteConnectionInTimeCharacteristics() noexcept
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            delete m_pConnectionInTimeCharacteristics[i][j];
            m_pConnectionInTimeCharacteristics[i][j] = nullptr;
        }
    }
}
void CMaaTcpSocket::CreateConnectionInTimeCharacteristics(int s, int m, int h, int d)
{
    DeleteConnectionInTimeCharacteristics();
    int i;
    for (i = 0; i < 4; i++)
    {
        _qword InitialValue = !(i & 1) ? m_BytesSend : m_BytesRecv;
        m_pConnectionInTimeCharacteristics[i][0] = TL_NEW CMaaConnectionInTimeCharacteristics(s, InitialValue, 1);
        m_pConnectionInTimeCharacteristics[i][1] = TL_NEW CMaaConnectionInTimeCharacteristics(m, InitialValue, 60);
        m_pConnectionInTimeCharacteristics[i][2] = TL_NEW CMaaConnectionInTimeCharacteristics(h, InitialValue, 3600);
        m_pConnectionInTimeCharacteristics[i][3] = TL_NEW CMaaConnectionInTimeCharacteristics(d, InitialValue, 24 * 3600);
    }
    int null_ptrs = 0;
    for (i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if  (!m_pConnectionInTimeCharacteristics[i][j])
            {
                null_ptrs++;
            }
        }
    }
    if  (null_ptrs)
    {
        DeleteConnectionInTimeCharacteristics();
    }
}
void CMaaTcpSocket::ResetConnectionInTimeCharacteristics(bool bSecondPartOnly)
{
    if  (m_pConnectionInTimeCharacteristics[0][0])
    {
        for (int i = !bSecondPartOnly ? 0 : 2; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                _qword value = !(i & 1) ? m_BytesSend : m_BytesRecv;
                m_pConnectionInTimeCharacteristics[i][j]->Reset(value, true);
            }
        }
    }
}
#endif
void CMaaTcpSocket::InitCounters(bool bConnected)
{
    m_BytesSend = m_BytesRecv = m_BytesSend0 = m_BytesRecv0 = 0;
    m_SendSpeed = m_RecvSpeed = bConnected ? 0 : -1;
#ifdef TOOLSLIB_USE_CMaaTcpSocket_ConnectionInTimeCharacteristics
    ResetConnectionInTimeCharacteristics(false);
#endif
}
//---------------------------------------------------------------------------
void CMaaTcpSocket::Timer1sSpeed()
{
    m_SendSpeed = m_BytesSend != (_qword)0 ? (int)(m_BytesSend - m_BytesSend0) : m_SendSpeed;
    m_BytesSend0 = m_BytesSend;

    m_RecvSpeed = m_BytesRecv != (_qword)0 ? (int)(m_BytesRecv - m_BytesRecv0) : m_RecvSpeed;
    m_BytesRecv0 = m_BytesRecv;

#ifdef TOOLSLIB_USE_CMaaTcpSocket_ConnectionInTimeCharacteristics
    if  (m_pConnectionInTimeCharacteristics[0][0])
    {
        for (int i = 0; i < 4; i++)
        {
            _qword value = !(i & 1) ? m_BytesSend : m_BytesRecv;
            //_qword value = !(i & 1) ? m_SendSpeed : m_RecvSpeed;
            m_pConnectionInTimeCharacteristics[i][0]->Set_Inc(value);
            if  (m_pConnectionInTimeCharacteristics[i][0]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[i][1]->Set_Inc(value);
                if  (m_pConnectionInTimeCharacteristics[i][1]->GetCurPos() % 60 == 0)
                {
                    m_pConnectionInTimeCharacteristics[i][2]->Set_Inc(value);
                    if  (m_pConnectionInTimeCharacteristics[i][2]->GetCurPos() % 24 == 0)
                    {
                        m_pConnectionInTimeCharacteristics[i][3]->Set_Inc(value);
                        //Accumulate(value, true);
                    }
                }
            }
        }
    }
#endif
}
//---------------------------------------------------------------------------
void CMaaTcpSocket::GetStatistics(_qword * pSentBytes, _qword * pRecvBytes, int * pSendSpeed, int * pRecvSpeed, bool bShapersSpeed) const noexcept
{
    if  (pSentBytes)
    {
        *pSentBytes = m_BytesSend;
    }
    if  (pRecvBytes)
    {
        *pRecvBytes = m_BytesRecv;
    }
    if  (pSendSpeed)
    {
        *pSendSpeed = m_SendSpeed;
    }
    if  (pRecvSpeed)
    {
        *pRecvSpeed = m_RecvSpeed;
    }
    if  (bShapersSpeed)
    {
#ifdef __SHAPERS
        //CMaaWin32Locker_Locker_gLock;
        //Locker_Lock;
        gpSockStartup && gpSockStartup->m_UsrShaperMutex.LockM();
        if  (pSendSpeed && m_SndLLShaper.m_Shaper && m_BytesSend != (_qword)0)
        {
            *pSendSpeed = m_SndLLShaper.m_VAverageLast;
        }
        if  (pRecvSpeed && m_RcvLLShaper.m_Shaper && m_BytesRecv != (_qword)0)
        {
            *pRecvSpeed = m_RcvLLShaper.m_VAverageLast;
        }
        gpSockStartup && gpSockStartup->m_UsrShaperMutex.UnLockM();
        //Locker_UnLock;
#else
        if  (pSendSpeed)
        {
            *pSendSpeed = 0;
        }
        if  (pRecvSpeed)
        {
            *pRecvSpeed = 0;
        }
#endif // __SHAPERS
    }
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::SendData(const void * Buffer, int Size)
{
    int Total = 0;
    int ret = 0;

    int rr;
#ifdef __SHAPERS
    //CMaaWin32Locker_Locker_gLock;
#endif
    while(Size > 0)
    {
        rr = -1;
#ifdef __SHAPERS
        //Locker_Lock;
        gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
        if  (m_SndLLShaper.m_Shaper)
        {
            rr = m_SndLLShaper.GetLenReserved(Size);
        }
        gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
        //Locker_UnLock;
#endif
        if  (rr == 0)
        {
            DP2(
                __utf8_printf("rr == 0\n", rr);
                if  (Size)
                {
                    //--- m_pFdSockets->ResetAState(this, eWrite);
                }
            )
            break;
        }
        int req = rr > 0 ? rr : Size;
        ret = send(m_Socket, (char *) Buffer, req, 0);
#ifdef _WIN32
        const DWORD dw = WSAGetLastError();
        /*
        if (ret > 0)
        {
            CMaaString str((char*)Buffer, ret, 0);
            __utf8_printf("send(%D, '%S')\n", (_qword)m_Socket, &str);
        }
        */
        DP2(__utf8_printf("ret=%d, req=%d, Size=%d, m_RealState=%x\n", ret, req, Size, m_RealState);)
        if  (ret >= 0 && ret < req)
        {
            m_RealState &= ~eWrite;
        }
        /*
        if (ret > 0)
        {
            if  (ret == req && req < Size)
            {
                //gH1 = m_Socket;
                //__utf8_printf("ResetAState\n");
                m_pFdSockets->ResetAState(this, eWrite);
            }
            else
            {
                m_pFdSockets->ResetAState(this, eWrite);
            }
        }
        */
#endif
#ifdef TL_EPOLL
        int e = ret < 0 ? errno : 0;
#ifndef TL_EPOLLET
        m_pFdSockets->ResetRAState(this, eWrite);
#else
        if  (ret >= 0 && ret < req)
        {
            m_pFdSockets->ResetRAState(this, eWrite);
        }
#endif
#else
#ifdef __linux__
        int e = ret < 0 ? errno : 0;
#endif
#endif
        DP(printf("send(%d) returns %d\n", m_Socket, ret); fflush(stdout);)

        if  (ret == SOCKET_ERROR)
        {
#ifdef _WIN32
            if  ( dw != WSAEWOULDBLOCK )
#else
            // unix
            if  (e != EAGAIN)
#endif
            {
                CloseByException("send()");
            }
#ifdef TL_EPOLLET
            m_pFdSockets->ResetRAState(this, eWrite); // e == EAGAIN
            //break;
#endif
#ifdef _WIN32
            m_RealState &= ~eWrite;
#endif

            //Total = -1 - Total; // i.e Total === -1 , windows (one cycle) // 15.12.2016
            //Total = -1;

            // 27.07.2012 one cycle, break
            Total = Total > 0 ? Total : -1;
            // 27.07.2012

            break;
        }
        else
        {
            UpdateHrTime(e_fds_last_sent_time);
            Total  += ret;
            Size   -= ret;
            Buffer = ret + (char *)Buffer;
            if  (m_BytesSend == (_qword)-1)
            {
                m_BytesSend = 0;
            }
            m_BytesSend += ret;
        }
#ifdef __SHAPERS
        //Locker_Lock;
        if (gpSockStartup)
        {
            CMaaWin32Locker lk(gpSockStartup->m_UsrShaperMutex);
            if (m_SndLLShaper.m_Shaper)
            {
                m_SndLLShaper.SubLenReserved(ret);
            }
        }
        else
        {
            if (m_SndLLShaper.m_Shaper)
            {
                m_SndLLShaper.SubLenReserved(ret);
            }
        }
        //Locker_UnLock;
#endif
        // 12.12.2016-2020
#ifndef TL_EPOLLET_MULTISEND
        break; // one portion read only
#else
        if  (ret < req)
        {
            break; // egde
        }
#endif
    }
    int Mode;
    //if   ((Total > 0 || Total <= -1) && !m_pFdSockets->m_SockHash.Find(this, &Mode))
    //if  ((Total > 0 || Total <= -1) && !m_pFdSockets->SockHashFind(this, &Mode))
    if ((Total > 0 || Total <= -1))
    {
        Mode = m_pFdSockets->SockHashFindMode(this);
        if  (!(m_RealState & ~eWrite))
        //if (Total != -1)
        {
            Mode &= ~eEventWrite;
            //m_pFdSockets->m_SockHash.AddOver(this, Mode);
            m_pFdSockets->SockHashAddOver(this, Mode);
        }
        Total = Total > 0 ? Total : 0;
    }
    //printf("SendData() for %d: Total = %d", m_Socket, Total); fflush(stdout);
    return Total >= -1 ? Total : -1;
}
//---------------------------------------------------------------------------
bool gCMaaTcpSocketsCompatibleRead = false; //only with out of shapers
//---------------------------------------------------------------------------
bool CMaaTcpSocket::IsClosed(int LastReceivedLen) const noexcept
{
#ifdef __SHAPERS
    if  (LastReceivedLen < 0 || (!m_RcvLLShaper.m_Shaper && gCMaaTcpSocketsCompatibleRead && LastReceivedLen == 0))
    {
        return true;
    }
#else
    if  (LastReceivedLen < 0 || (gCMaaTcpSocketsCompatibleRead && LastReceivedLen == 0))
    // or if   (LastReceivedLen <= 0)
    {
        return true;
    }
#endif
    return false;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::RecvData(void * Buffer, int Size)
{
    int Total = 0, Total2 = 0;
    int ret = 0;

    bool bCompatibleRead = false;
    //#ifndef _WIN32
    bool bRAStateReset = false;
    //#endif

    int rr;
#ifdef __SHAPERS
    //CMaaWin32Locker_Locker_gLock;
#endif
    while(Size > 0)
    {
        rr = -1;
#ifdef __SHAPERS
        //Locker_Lock;
        gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
        if  (m_RcvLLShaper.m_Shaper)
        {
            rr = m_RcvLLShaper.GetLenReserved(Size);
        }
        else
#endif
        {
            bCompatibleRead = gCMaaTcpSocketsCompatibleRead;
        }
#ifdef __SHAPERS
        gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
        //Locker_UnLock;
#endif
#if defined(TL_EPOLL) && defined(__SHAPERS)
        /*
        if   (rr == 0)
        {
            rr++;
        }
        */
#else
        if  (rr == 0)
        {
            break;
        }
#endif
        //printf("recv(%d, len=%d, rr=%d, Size=%d)...\n", m_Socket, rr > 0 ? rr : Size, rr, Size); fflush(stdout);
        const int req = rr > 0 ? rr : Size;
#ifdef __unix__
        if  (m_Socket == fileno(stdin))
        {
            ret = read(m_Socket, (char *)Buffer, req);
            if  (ret < 0 && errno == EAGAIN)
            {
                ret = 0;
            }
        }
        else
#endif
        ret = recv(m_Socket, (char *)Buffer, req, 0);
#ifdef _WIN32
        DWORD e = WSAGetLastError();
        if  (ret >= 0 && ret < req)
        {
            m_RealState &= ~eRead;
            bRAStateReset = true;
        }
        /*
        if (ret > 0)
        {
            CMaaString str((char*)Buffer, ret, 0);
            __utf8_printf("recv(%D, '%S')\n", (_qword)m_Socket, &str);
        }
        */
        m_pFdSockets->ResetRAState(this, eRead);
        bRAStateReset = true;
#endif
#ifdef TL_EPOLL
        int e = ret < 0 ? errno : 0;
#ifndef TL_EPOLLET
        //printf("r.rastate\n");
        m_pFdSockets->ResetRAState(this, eRead);
        bRAStateReset = true;
#else
#ifdef TL_EPOLLET_MULTIREAD
        if  (ret >= 0 && ret < req && !(ret == 0 && Total > 0))
#else
        if  (ret >= 0 && ret < req)
#endif
        {
            //printf("r.rastate\n");
            m_pFdSockets->ResetRAState(this, eRead);
            bRAStateReset = true;
        }
#endif
#endif
        DP(printf("recv(%d) returns %d\n", m_Socket, ret); fflush(stdout);)
        //DP1(printf("[%d] ret=%d e=%d\n", m_Socket, ret, e); fflush(stdout);)
        if  (ret == SOCKET_ERROR)
        {
            //DP1(printf("[%d] ret=%d e=%d\n", m_Socket, ret, e); fflush(stdout);)
#ifdef _WIN32
            if  (e == WSAEWOULDBLOCK)
            {
                m_RealState &= ~eRead;
                m_pFdSockets->ResetRAState(this, eRead);
                bRAStateReset = true;
                break;
            }
#endif
#ifdef TL_EPOLL
            if  (e == EAGAIN)
            {
                //printf("eagain r.rastate\n");
                m_pFdSockets->ResetRAState(this, eRead);
                bRAStateReset = true;
                break;
            }
#endif
            //XTOOSockErr err("recv", nullptr);
            //printf("%s\n", (const char *)err.GetMsg()); fflush(stdout);

#ifdef TL_EPOLLET_MULTIREAD
            if  (Total <= 0)
#endif
            //if   ( WSAGetLastError () != WSAEWOULDBLOCK )
            {
                CloseByException("recv()");
                //vAsyncMsg ( m_Socket, FD_READ, err );
            }
            break;
        }
        else if (!ret)
        {
            Total2 = -1; // on close
            //printf("[%d] ret=0 (Total=%d) --> Total2 = -1; // on close\n", m_Socket, Total);
            break;
        }
        else
        {
            UpdateHrTime(e_fds_last_receive_time);
            Total  += ret;
            Size   -= ret;
            Buffer = ret + (char *)Buffer;
            if  (m_BytesRecv == (_qword)-1)
            {
                m_BytesRecv = 0;
            }
            m_BytesRecv += ret;
        }
        if  (ret > 0)
        {
#ifdef __SHAPERS
            //Locker_Lock;
            if (gpSockStartup)
            {
                CMaaWin32Locker lk(gpSockStartup->m_UsrShaperMutex);
                if (m_RcvLLShaper.m_Shaper)
                {
                    m_RcvLLShaper.SubLenReserved(ret);
                }
            }
            else
            {
                if (m_RcvLLShaper.m_Shaper)
                {
                    m_RcvLLShaper.SubLenReserved(ret);
                }
            }
            //Locker_UnLock;
#if defined(TL_EPOLL) && defined(__SHAPERS)
            if  (ret >= req)
            {
                break;
            }
#endif
#endif
        }
        //12.12.2016-2020
#ifndef TL_EPOLLET
        break;
#endif
#ifdef TL_EPOLLET_MULTIREAD
        if  (ret >= 0 && ret < req)
#endif
        {
            break;
        }
    }
    int Mode;
#ifdef __unix__
    //if   ((Total > 0 || m_Socket == fileno(stdin)) && !m_pFdSockets->m_SockHash.Find(this, &Mode))
    //if  (((Total > 0 && Total2 >= 0) || m_Socket == fileno(stdin)) && !m_pFdSockets->SockHashFind(this, &Mode))
    if (((Total > 0 && Total2 >= 0) || m_Socket == fileno(stdin)))
#else
    //if   (Total > 0 && !m_pFdSockets->m_SockHash.Find(this, &Mode))
    //if  (Total > 0 && !m_pFdSockets->SockHashFind(this, &Mode))
    if (Total > 0)
#endif
    {
        Mode = m_pFdSockets->SockHashFindMode(this);
#ifdef __unix__
        if  (m_Socket == fileno(stdin))
        {
            //printf("Mode = 0x%x -->", Mode);
            Mode &= ~eEventRead;
            //printf(" 0x%x\n", Mode); fflush(stdout);
        }
        else
        {
#endif
#ifdef __unix__
            const int l = bRAStateReset ? -1 : 1;
#else
            //char c = 0;
            ////printf("recv(%d, len=1, MSG_PEEK)...\n", m_Socket); fflush(stdout);
            //const int l = recv(m_Socket, &c, 1, MSG_PEEK);
            const int l = bRAStateReset ? -1 : 1;
#endif
            //printf("recv(%d) returns %d\n", m_Socket, l); fflush(stdout);
            if  (l >= 0)
            {
                //printf("m |= er\n");
                //Mode |= eEventRead;
            }
            else
            {
                Mode &= ~eEventRead;
            }
#ifdef __unix__
        }
#endif
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
    }


    //printf("RecvData for %d: Total = %d\n", m_Socket, Total); fflush(stdout);

    if  (Total2 < 0 && Total >= 0)
    {
        //#ifndef TL_EPOLLET_MULTIREAD
        //Total = Total > 0 ? Total : -1;// - Total;
        ////Total = -1 - Total;
        //#else
        if  (Total == 0)
        {
            Total = -1;
        }
        //#endif
    }
    if  (bCompatibleRead && Total < 0)
    {
        Total = 0;
    }
    //char txt [ 256 ];
    //sprintf ( txt, "RecvData() %d bytes", Total );
    //Trace ( txt );
    return Total;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::ConnectAgain(int fCallNotify)
{
    // printf("%p: CMaaTcpSocket::ConnectAgain (%d)\n", this, fCallNotify);

    IfDebug(200) { DP(printf("CMaaTcpSocket::ConnectAgain ()\n");) }
    const int domain = GetDomainSock();
    const _IP Ip = htonl(m_s.m_sin.sin_addr.s_addr);
    _byte * Ip6 = (_byte *)m_s.m_sin6.sin6_addr.s6_addr;
    const _Port Port = domain == AF_INET6 ? htons(m_s.m_sin6.sin6_port) : htons(m_s.m_sin.sin_port);
    char pip[256];
#ifdef _WIN32
    if  (fCallNotify == 100)
    {
        ChangeFdModeEx(eDisableConnect);
#ifdef __unix__
        //ChangeFdModeEx(eConnect | eExept);
        SetNonBlockingMode();
#endif
        UpdateHrTime(e_fds_connected_at_time);
        return domain == AF_INET6 ? Notify_Connected6(Ip6, Port, CMaaIpToTextEx(pip, Ip6, 0)) : Notify_Connected(Ip, Port, CMaaIpToTextEx(pip, Ip, 0));
    }
#endif
    int x;
    if  ( (x=(domain == AF_INET6 ? connect(m_Socket, (struct sockaddr *)&m_s.m_sin6, sizeof(m_s.m_sin6)) : connect(m_Socket, (struct sockaddr *)&m_s.m_sin, sizeof(m_s.m_sin)))) )
    {
#ifdef __unix__
#define __CA_ERRNO       errno
#define __CA_EISCONN     EISCONN      // FreeBSD
#define __CA_EINPROGRESS EINPROGRESS  // FreeBSD
#define __CA_EALREADY    EALREADY     // FreeBSD

        //#define __CA_EISCONN     -123456
        //#define __CA_EINPROGRESS 115
        //#define __CA_EALREADY    -123457
#else
#define __CA_ERRNO       WSAGetLastError ()
#define __CA_EINPROGRESS WSAEWOULDBLOCK
#define __CA_EALREADY    WSAEALREADY
#define __CA_EISCONN     WSAEISCONN
#endif
        if  (0)
        {
            DP(printf ( "connect returns %d errno=%d", x, errno );
            XTOOSockErr Err("error=", nullptr);
            fflush(stdout);)
        }
        switch( __CA_ERRNO )
        {
        case __CA_EINPROGRESS:
        case __CA_EALREADY:
            return fCallNotify ? 0 : 2;

        case __CA_EISCONN:
            if  ( fCallNotify )
            {
                ChangeFdModeEx(eDisableConnect);
#ifdef __unix__
                //printf("SetNonBlockingMode()\n"); fflush(stdout);
                SetNonBlockingMode();
#endif
                UpdateHrTime(e_fds_connected_at_time);
                return domain == AF_INET6 ? Notify_Connected6(Ip6, Port, CMaaIpToTextEx(pip, Ip6, 0)) : Notify_Connected(Ip, Port, CMaaIpToTextEx(pip, Ip, 0));
            }
            return fCallNotify ? 0 : 3;
        default:
            {
                IfDebug(2) { DP(printf("\n\n_____connect returns %d errno=%d", x, errno);) }
                {XTOOSockErr Err ( "error=", nullptr );}
                IfDebug(2) { DP(printf("_____\n"); fflush(stdout);) }
            }
        }

        CMaaString txt;
        if (domain == AF_INET6)
        {
            txt.Format("connect (%J, %d)", Ip6, Port);
        }
        else
        {
            txt.Format("connect (%I, %d)", Ip, Port);
        }
        Error(txt);
        return fCallNotify ? 0 : 4;
    }
    else
    {
        //printf ( "connect returns %d\n", x );
        //fflush(stdout);
    }
    if  (fCallNotify)
    {
        ChangeFdModeEx(eDisableConnect);
#ifdef __unix__
        //ChangeFdModeEx(eConnect | eExept);
        SetNonBlockingMode();
#endif
        UpdateHrTime(e_fds_connected_at_time);
        return domain == AF_INET6 ? Notify_Connected6(Ip6, Port, CMaaIpToTextEx(pip, Ip6, 0)) : Notify_Connected(Ip, Port, CMaaIpToTextEx(pip, Ip, 0));
        // TEST: return 0;
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect(_IP Ip, _Port Port)
{
    if  (GetDomainSock() == AF_INET6)
    {
        Error("Connect() on IPv6 socket called");
    }
    IfDebug(100) { DP(printf("CMaaTcpSocket::Connect(%s, %d)\n", CMaaIpToText(Ip), Port);) }
    m_s.m_sin.sin_family      = AF_INET;
    m_s.m_sin.sin_port        = htons(Port);
    m_s.m_sin.sin_addr.s_addr = htonl(Ip);
    ChangeFdModeEx(eConnect);
    //printf("\n\nMode: %d\n", GetFdMode());
    int Ret = ConnectAgain(0);
    IfDebug(1)
    {
        DP(printf("Connect for %d returns %d\n", (int)m_Socket, Ret); fflush(stdout);)
    }
    if  ( Ret >= 3 )
    {
        CMaaString txt;
        txt.Format("connect (%I, %d)", Ip, Port);
        Error(txt);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect(const char * DnsName, _Port Port)
{
    IfDebug(100) { DP(printf("CMaaTcpSocket::Connect(\"%s\", %d)\n", DnsName, Port);) }
    const _IP Ip = GetHostByName(DnsName, &Port);
    return Connect(Ip, Port);
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect(const CMaaString& DnsName, _Port Port)
{
    IfDebug(100) { DP(printf("CMaaTcpSocket::Connect(\"%s\", %d)\n", DnsName, Port);) }
    const _IP Ip = GetHostByName((const char *)DnsName.Str0Copy(), &Port);
    return Connect(Ip, Port);
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect(_IP Ip, _Port Port)
{
    //RemoveFdSocket ();
    ChangeFdModeEx(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect(Ip, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Detect64(const CMaaString &DnsName_Port, CMaaString *pDnsName_Port) noexcept
{
    if  (pDnsName_Port)
    {
        *pDnsName_Port = DnsName_Port;
    }
#ifndef TOOLSLIB_USE_IPV6
    return AF_INET;
#else
    {
        _IP6 tmp;
        if  (CMaaIpToLongEx46(DnsName_Port, tmp.ptr(), ":") >= 0)
        {
            bool b;
            tmp.v4(&b);
            return b ? AF_INET : AF_INET6;
        }
        else
        {
            _Port Port = 0;
            _IP Ip = 0;
            try
            {
                /*const _byte* Ip6 =*/ GetHostByName6(tmp.ptr(), DnsName_Port, &Port);
                bool b;
                Ip = tmp.v4(&b);
                if  (!b)
                {
                    if  (pDnsName_Port)
                    {
                        pDnsName_Port->Format("[%J]:%d", tmp.ptr(), (int)Port);
                    }
                    return AF_INET6;
                }
                if  (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch(...)
            {
            }
            try
            {
                Ip = GetHostByName ( DnsName_Port, &Port );
                if  (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch(...)
            {
            }
        }
    }
    return -1;
#endif
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Detect46(const CMaaString &DnsName_Port, CMaaString *pDnsName_Port) noexcept
{
    if  (pDnsName_Port)
    {
        *pDnsName_Port = DnsName_Port;
    }
#ifndef TOOLSLIB_USE_IPV6
    return AF_INET;
#else
    {
        _IP6 tmp;
        if  (CMaaIpToLongEx46(DnsName_Port, tmp.ptr(), ":") >= 0)
        {
            bool b;
            tmp.v4(&b);
            return b ? AF_INET : AF_INET6;
        }
        else
        {
            _Port Port = 0;
            _IP Ip = 0;
            try
            {
                Ip = GetHostByName(DnsName_Port, &Port);
                if  (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch(...)
            {
            }
            try
            {
                /*const _byte* Ip6 =*/ GetHostByName6(tmp.ptr(), DnsName_Port, &Port);
                bool b;
                Ip = tmp.v4(&b);
                if  (!b)
                {
                    if  (pDnsName_Port)
                    {
                        pDnsName_Port->Format("[%J]:%d", tmp.ptr(), (int)Port);
                    }
                    return AF_INET6;
                }
                if  (pDnsName_Port)
                {
                    pDnsName_Port->Format("[%I]:%d", Ip, (int)Port);
                }
                return AF_INET;
            }
            catch(...)
            {
            }
        }
    }
    return -1;
#endif
}
#ifdef TOOLSLIB_USE_IPV6
int CMaaTcpSocket::Connect6(const _byte * Ip6, _Port Port)
{
    if  (GetDomainSock() != AF_INET6)
    {
        Error("Connect() on not IPv6 socket called");
    }
    ///////IfDebug ( 100 ) printf ( "CMaaTcpSocket::Connect6 ( %s, %d )\n", CMaaIpToText ( Ip ), Port );
    m_s.m_sin6.sin6_family     = AF_INET6;
    m_s.m_sin6.sin6_port       = htons(Port);
    memcpy(m_s.m_sin6.sin6_addr.s6_addr, Ip6, 16);
    ChangeFdModeEx(eConnect);
    //printf("\n\nMode: %d\n", GetFdMode());
    int Ret = ConnectAgain(0);
    IfDebug(1)
    {
        DP(printf("Connect for %d returns %d\n", (int)m_Socket, Ret); fflush(stdout);)
    }
    if  ( Ret >= 3 )
    {
        CMaaString txt;
        txt.Format("connect (%J, %d)", Ip6, Port);
        Error(txt);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect6(const char* DnsName, _Port Port)
{
    IfDebug(100) { DP(printf("CMaaTcpSocket::Connect ( \"%s\", %d )\n", DnsName, Port);) }
    _byte b[16];
    _byte* Ip6 = GetHostByName6(b, DnsName, &Port);
    return Connect6(Ip6, Port);
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Connect6(const CMaaString& DnsName, _Port Port)
{
    IfDebug(100) { DP(printf("CMaaTcpSocket::Connect ( \"%s\", %d )\n", DnsName, Port);) }
    _byte b[16];
    _byte* Ip6 = GetHostByName6(b, (const char *)DnsName.Str0Copy(), &Port);
    return Connect6(Ip6, Port);
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect6(const _byte * Ip6, _Port Port)
{
    ChangeFdModeEx(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect6(Ip6, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect6(const char * DnsName, _Port Port)
{
    ChangeFdModeEx(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect6(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect6(const CMaaString& DnsName, _Port Port)
{
    ChangeFdModeEx(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect6(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
#endif //#ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect(const char* DnsName, _Port Port)
{
    ChangeFdModeEx(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::AsyncConnect(const CMaaString& DnsName, _Port Port)
{
    ChangeFdModeEx(eConnect | eExept);
#ifdef __unix__
    SetNonBlockingMode();
#endif
    Connect(DnsName, Port);
    return 1;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Bind(_Port Port, _IP Ip, bool bReuse)
{
    if  (GetDomainSock() == AF_INET6)
    {
        Error("Bind() on IPv6 socket called");
    }
    IfDebug(100) { DP(printf("CMaaTcpSocket::Bind ( %d )\n", Port);) }
    m_s.m_sin.sin_family      = AF_INET;
    m_s.m_sin.sin_port        = htons(Port);
    m_s.m_sin.sin_addr.s_addr = htonl(Ip);
    if  (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
#define P_PARAM (const char *)
#else
        int On = 1;
#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof(On));
    }
    if  (::bind(m_Socket, (struct sockaddr *)&m_s.m_sin, sizeof(m_s.m_sin)) == SOCKET_ERROR)
    {
        Error("bind ()");
        return 2;
    }
    socklen_t len = sizeof(m_s.m_sin);
    if  (getsockname(m_Socket, (struct sockaddr *)&m_s.m_sin, &len))
    {
        Error("getsockname()");
        return 22;
    }
    SetAcceptFlag();
    return 0;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
int CMaaTcpSocket::Bind6(_Port Port, const _byte * Ip6, bool bReuse)
{
    if  (GetDomainSock() != AF_INET6)
    {
        Error("Bind() on not IPv6 socket called");
    }

    //__utf8_printf("Bind6(%d, %J, %s)\n", Port, Ip6, bReuse ? "true" : "false");

    IfDebug(100) { DP(printf("CMaaTcpSocket::Bind ( %d )\n", Port);) }
    m_s.m_sin6.sin6_family     = AF_INET6;
    m_s.m_sin6.sin6_port       = htons(Port);
    if  (Ip6)
    {
        memcpy(m_s.m_sin6.sin6_addr.s6_addr, Ip6, 16);
    }
    else
    {
        memset(m_s.m_sin6.sin6_addr.s6_addr, 0, 16);
    }
    if  (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
#define P_PARAM (const char *)
#else
        int On = 1;
#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof(On));
    }
    if  (::bind(m_Socket, (struct sockaddr *)&m_s.m_sin6, sizeof(m_s.m_sin6)) == SOCKET_ERROR)
    {
        Error("bind ()");
        return 2;
    }
    socklen_t len = sizeof(m_s.m_sin6);
    if  (getsockname(m_Socket, (struct sockaddr *)&m_s.m_sin6, &len))
    {
        Error("getsockname()");
        return 22;
    }
    SetAcceptFlag();
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Bind6(CMaaString strIpPort, bool bReuse)
{
    _byte BindIp[16];
    memset(BindIp, 0, sizeof(BindIp));
    int Port = 0;
    int trailer_pos = CMaaIpToLongEx(strIpPort, BindIp, ":");

    //__utf8_printf("%S %d %J\n", &strIpPort, trailer_pos, BindIp);

    strIpPort = strIpPort.Mid(trailer_pos > 0 ? trailer_pos + 1: 1);
    strIpPort.sscanfInt(&Port);

    //__utf8_printf("%S %d\n", &strIpPort, Port);

    return Bind6(Port, BindIp, strIpPort.LastChar() == 'r');
}
#endif //#ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaTcpSocket::Bind(CMaaString strIpPort, bool bReuse)
{
    _IP BindIp = 0;
    int Port = 0;
    int trailer_pos = CMaaIpToLongEx(strIpPort, &BindIp, ":");

    strIpPort = strIpPort.Mid(trailer_pos > 0 ? trailer_pos + 1: 1);
    strIpPort.sscanfInt(&Port);
    return Bind(Port, BindIp, strIpPort.LastChar() == 'r');
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::GetBindedPort()
{
    if  (GetDomainSock() == AF_INET6)
    {
        socklen_t len = sizeof(m_s.m_sin6);
        if  (getsockname(m_Socket, (struct sockaddr *)&m_s.m_sin6, &len))
        {
            Error("getsockname()");
            return 22;
        }
        return ntohs(m_s.m_sin6.sin6_port);
    }

    socklen_t len = sizeof(m_s.m_sin);
    if  (getsockname(m_Socket, (struct sockaddr *)&m_s.m_sin, &len))
    {
        Error("getsockname()");
        return 22;
    }
    return ntohs(m_s.m_sin.sin_port);
}
//---------------------------------------------------------------------------
bool CMaaTcpSocket::GetConnInfo(_IP * pLocalIp, _Port * pLocalPort, _IP * pRemoteIp, _Port * pRemotePort, bool bThrow)
{
    bool Ret = (pLocalIp || pLocalPort || pRemoteIp || pRemotePort);
    struct sockaddr_in s;
    socklen_t len;
    if  (pLocalIp || pLocalPort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if  (getsockname(m_Socket, (struct sockaddr *)&s, &len))
        {
            if (bThrow)
            {
                Error("getsockname()");
            }
            Ret = false;
        }
        else
        {
            if (pLocalIp)
            {
                *pLocalIp = ntohl(s.sin_addr.s_addr);
            }
            if (pLocalPort)
            {
                *pLocalPort = ntohs(s.sin_port);
            }
        }
    }
    if  (pRemoteIp || pRemotePort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if  (getpeername(m_Socket, (struct sockaddr *)&s, &len))
        {
            if (bThrow)
            {
                Error("getpeername()");
            }
            Ret = false;
        }
        else
        {
            if (pRemoteIp)
            {
                *pRemoteIp = ntohl(s.sin_addr.s_addr);
            }
            if (pRemotePort)
            {
                *pRemotePort = ntohs(s.sin_port);
            }
        }
    }
    return Ret;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
bool CMaaTcpSocket::GetConnInfo6(_byte * pLocalIp, _Port * pLocalPort, _byte * pRemoteIp, _Port * pRemotePort, bool bThrow)
{
    bool Ret = (pLocalIp || pLocalPort || pRemoteIp || pRemotePort);
    struct sockaddr_in6 s;
    socklen_t len;
    if  (pLocalIp || pLocalPort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if  (getsockname(m_Socket, (struct sockaddr *)&s, &len))
        {
            if (bThrow)
            {
                Error("getsockname()");
            }
            Ret = false;
        }
        else
        {
            if (pLocalIp)
            {
                memcpy(pLocalIp, s.sin6_addr.s6_addr, 16);
            }
            if (pLocalPort)
            {
                *pLocalPort = ntohs(s.sin6_port);
            }
        }
    }
    if  (pRemoteIp || pRemotePort)
    {
        memset(&s, 0, sizeof(s));
        len = sizeof(s);
        if  (getpeername(m_Socket, (struct sockaddr *)&s, &len))
        {
            if (bThrow)
            {
                Error("getpeername()");
            }
            Ret = false;
        }
        else
        {
            if (pRemoteIp)
            {
                memcpy(pRemoteIp, s.sin6_addr.s6_addr, 16);
            }
            if (pRemotePort)
            {
                *pRemotePort = ntohs(s.sin6_port);
            }
        }
    }
    return Ret;
}
//---------------------------------------------------------------------------
_byte * CMaaSocket::GetLocalHostIp6(_byte * RetIp)
{
    char txt[256];
    txt[0] = 0;
    gethostname(txt, sizeof(txt));
#ifdef _WIN32
    CMaaString buffer;
    CMaaHe1 * h = CMaa_gethostbyname2(txt, buffer);
    if  (*txt == 0)
    {
        strcpy(txt, "<unknown>");
    }
    if  (!h)
    {
        CMaaString msg;
        msg.Format("gethostbyname(\"%s\")", txt);
        throw XTOOSockDnsErr(msg, nullptr, "CMaaSocket::GetLocalHostIp()");
        //StaticError("gethostbyname() in CMaaSocket::GetLocalHostIp()");
        //memset(RetIp, 0, sizeof(RetIp);
        return nullptr;//RetIp; // 1? // !!!!!!!!!2017
    }
    /*
     printf("Host name: %s", txt);
     if   ( h -> h_name )
     {
          printf ( " (%s)", h -> h_name );
     }
     printf ( "\n" );
     int i;
     for   ( i = 0; h -> h_aliases [ i ] != nullptr; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != nullptr; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    memcpy(RetIp, h->h_addr_list[0], 16);
#else
    struct hostent * h = gethostbyname2(txt, AF_INET6);
    if  (*txt == 0)
    {
        strcpy(txt, "<unknown>");
    }
    if  (!h)
    {
        CMaaString msg;
        msg.Format("gethostbyname(\"%s\")", txt);
        throw XTOOSockDnsErr(msg, nullptr, "CMaaSocket::GetLocalHostIp()");
        //StaticError("gethostbyname() in CMaaSocket::GetLocalHostIp ()");
        //memset(RetIp, 0, sizeof(RetIp);
        return nullptr;//RetIp; // 1? // !!!!!!!!!2017
    }
    /*
     printf("Host name: %s", txt);
     if   ( h -> h_name )
     {
          printf ( " (%s)", h -> h_name );
     }
     printf ( "\n" );
     int i;
     for   ( i = 0; h -> h_aliases [ i ] != nullptr; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != nullptr; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    memcpy(RetIp, h->h_addr_list[0], 16);
#endif
    return RetIp;
}
#endif // #ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
_IP CMaaSocket::GetLocalHostIp()
{
    char txt[256];
    txt[0] = 0;
    gethostname(txt, sizeof(txt));
    struct hostent * h = gethostbyname(txt);
    if  (*txt == 0)
    {
        strcpy(txt, "<unknown>");
    }
    if  (!h)
    {
        CMaaString msg;
        msg.Format("gethostbyname(\"%s\")", txt);
        throw XTOOSockDnsErr(msg, nullptr, "CMaaSocket::GetLocalHostIp()");
        //StaticError("gethostbyname () in CMaaSocket::GetLocalHostIp()" );
        return 1; // 1?
    }
    /*
     printf("Host name: %s", txt);
     if   ( h -> h_name )
     {
          printf ( " (%s)", h -> h_name );
     }
     printf ( "\n" );
     int i;
     for   ( i = 0; h -> h_aliases [ i ] != nullptr; i++ )
     {
          printf ( "Alias: %s\n", h -> h_aliases [ i ] );
     }
     for   ( i = 0; h -> h_addr_list [ i ] != nullptr; i++ )
     {
          printf ( "IP address: %s\n", CMaaIpToText ( htonl ( * ( _IP * ) h -> h_addr_list [ i ] ) ) );
     }
     */
    return htonl(*(_IP *)h->h_addr_list[0]);
}
//---------------------------------------------------------------------------
const char * CMaaSocket::GetLocalHostName() noexcept
{
    static char txt[1024];
    txt[0] = 0;
    gethostname(txt, sizeof(txt));
    //struct hostent * h = gethostbyname ( txt );
    if  (*txt == 0)
    {
        strcpy(txt, "<unknown>");
    }
    return txt;
}
//---------------------------------------------------------------------------
void CMaaSocket::SetNonBlockingMode()
{
#ifdef __unix__
    int flags = fcntl(m_Socket, F_GETFL);
    if  (flags != -1)
    {
        if  (fcntl(m_Socket, F_SETFL, flags | O_NONBLOCK))
        {
            Error("CMaaSocket::SetNonBlockingMode() fcntl F_SETFL");
            //Error("fcntl F_SETFL O_NONBLOCK failed");
        }
    }
    else
    {
        Error("CMaaSocket::SetNonBlockingMode() fcntl F_GETFL");
    }
#else
    long cmd = FIONBIO;
    u_long IsNonBlocking = 1;     // zero for disabling
    if  (ioctlsocket(m_Socket, cmd, &IsNonBlocking))
    {
        Error("CMaaSocket::SetNonBlockingMode()");
    }
#endif
    IfDebug(100)
    {
        DP(Trace("SetNonBlockingMode() for %d - done", (int)m_Socket);)
    }
}
//---------------------------------------------------------------------------
int CMaaTcpSocket::Listen(int QueenDepth)
{
    IfDebug(100) { DP(printf("CMaaTcpSocket::Listen()\n");) }
    if  (SOCKET_ERROR == listen(m_Socket, QueenDepth))
    {
        Error("listen ()");
        return 3;
    }
    return 0;
}
//---------------------------------------------------------------------------
SOCKET CMaaTcpSocket::Accept(_IP * IpFrom, _Port * Port)
{
    //printf("Accept()\n");
    IfDebug(100) { DP(printf("CMaaTcpSocket::Accept() ");) }
    struct sockaddr_in si;
    socklen_t len;
    memset(&si, 0, len = sizeof(si));
    SOCKET s = accept(GetSocket(), (struct sockaddr *)&si, &len);
    DP(printf("accepted socket %d\n", s);)
    int Mode;
    //if   (!m_pFdSockets->m_SockHash.Find(this, &Mode))
    if  (!m_pFdSockets->SockHashFind(this, &Mode))
    //Mode = m_pFdSockets->SockHashFindMode(this);
    {
        Mode &= ~eEventRead;
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
#ifdef TL_EPOLL
        m_pFdSockets->ResetRAState(this, eRead);
#endif
    }

    if  (s == SOCKET_ERROR)
    {
        //printf("accept error %d\n", errno);
        Error("accept()");
    }
    if  (IpFrom)
    {
        *IpFrom = htonl(si.sin_addr.s_addr);
    }
    if  (Port)
    {
        *Port = htons(si.sin_port);
    }
    IfDebug ( 100 )
    {
        DP(printf("accept from %s:%d, socket = %d\n", CMaaIpToText(htonl(si.sin_addr.s_addr)), htons(si.sin_port), (int)s);
        fflush(stdout);)
    }

    if  (gDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gDisableNagleAlgoritm != 0, s);
    }

    return s;
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
SOCKET CMaaTcpSocket::Accept6(_byte * Ip6From, _Port * Port)
{
    //printf("Accept()\n");
    IfDebug(100) { DP(printf("CMaaTcpSocket::Accept() ");) }
    struct sockaddr_in6 si;
    socklen_t len;
    memset(&si, 0, len = sizeof(si));
    SOCKET s = accept(GetSocket(), (struct sockaddr *)&si, &len);
    DP(printf("accepted socket %d\n", s);)
    int Mode;
    //if   (!m_pFdSockets->m_SockHash.Find(this, &Mode))
    if  (!m_pFdSockets->SockHashFind(this, &Mode))
    //Mode = m_pFdSockets->SockHashFindMode(this);
    {
        Mode &= ~eEventRead;
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
#ifdef TL_EPOLL
        m_pFdSockets->ResetRAState(this, eRead);
#endif
    }

    if  (s == SOCKET_ERROR)
    {
        //printf("accept error %d\n", errno);
        Error("accept()");
    }
    if  (Ip6From)
    {
        memcpy(Ip6From, si.sin6_addr.s6_addr, 16);
    }
    if  (Port)
    {
        *Port = htons(si.sin6_port);
    }
    IfDebug ( 100 )
    {
        DP(printf("accept from %s:%d, socket = %d\n", CMaaIpToText(si.sin6_addr.s6_addr), htons(si.sin_port), (int)s);
        fflush(stdout);)
    }

    if  (gDisableNagleAlgoritm >= 0)
    {
        DisableNagleAlgoritm(gDisableNagleAlgoritm != 0, s);
    }

    return s;
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpSocket::CMaaUdpSocket(CMaaFdSockets * pFdSockets, int domain)
:   CMaaSocket(pFdSockets, domain, SOCK_DGRAM, IPPROTO_UDP)
{
    memset(&m_s, 0, sizeof(m_s));
#ifdef __unix__
    SetNonBlockingMode(); // exp !!!2020!!!
#endif
}
//---------------------------------------------------------------------------
void CMaaUdpSocket::Send(const void * pBuffer, int BufferSize, _IP Ip, _Port Port)
{
    m_s.m_sin.sin_family         = AF_INET;
    m_s.m_sin.sin_port           = htons(Port);
    m_s.m_sin.sin_addr.s_addr    = htonl(Ip);
    const int Ret = sendto(m_Socket, (char *) pBuffer, BufferSize, 0, (struct sockaddr *)&m_s.m_sin, sizeof(m_s.m_sin));

#ifdef TL_EPOLL
    int e = Ret < 0 ? errno : 0;
#ifndef TL_UDP_EPOLLET
    m_pFdSockets->ResetRAState(this, eWrite);
#endif
#endif
    if  (Ret == SOCKET_ERROR)
    {
#ifdef TL_UDP_EPOLLET
        if  (e == EAGAIN)
        {
            m_pFdSockets->ResetRAState(this, eWrite);
        }
#endif
        Error("sendto()");
    }
    else
    {
        UpdateHrTime(e_fds_last_sent_time);
    }
}
//---------------------------------------------------------------------------
#ifdef TOOLSLIB_USE_IPV6
void CMaaUdpSocket::Send6(const void * pBuffer, int BufferSize, const _byte *Ip6, _Port Port)
{
    m_s.m_sin6.sin6_family  = AF_INET6;
    m_s.m_sin6.sin6_port    = htons(Port);
    memcpy(m_s.m_sin6.sin6_addr.s6_addr, Ip6, 16);
    const int Ret = sendto(m_Socket, (char *) pBuffer, BufferSize, 0, (struct sockaddr *)&m_s.m_sin6, sizeof(m_s.m_sin6));

#ifdef TL_EPOLL
    int e = Ret < 0 ? errno : 0;
#ifndef TL_UDP_EPOLLET
    m_pFdSockets->ResetRAState(this, eWrite);
#endif
#endif
    if  (Ret == SOCKET_ERROR)
    {
#ifdef TL_UDP_EPOLLET
        if  (e == EAGAIN)
        {
            m_pFdSockets->ResetRAState(this, eWrite);
        }
#endif
        Error("sendto()");
    }
    else
    {
        UpdateHrTime(e_fds_last_sent_time);
    }
}
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient(CMaaFdSockets * pFdSockets, _Port Port, _IP Ip)
:   CMaaUdpSocket(pFdSockets),
    m_Port (Port),
    m_Ip   (Ip)
{
}
//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient(CMaaFdSockets * pFdSockets, _Port Port, const char * DnsName)
:   CMaaUdpSocket(pFdSockets),
    m_Port ( Port )
{
    m_Ip = GetHostByName ( DnsName );
}
//---------------------------------------------------------------------------
CMaaUdpClient::CMaaUdpClient(CMaaFdSockets * pFdSockets)
:   CMaaUdpSocket(pFdSockets),
    m_Port (0),
    m_Ip   (0)
{
}
//---------------------------------------------------------------------------
void CMaaUdpClient::Send(const void * pBuffer, int BufferSize )
{
    CMaaUdpSocket::Send(pBuffer, BufferSize, m_Ip, m_Port);
}
//---------------------------------------------------------------------------
void CMaaUdpClient::Send(const void * pBuffer, int BufferSize, _IP Ip)
{
    CMaaUdpSocket::Send(pBuffer, BufferSize, Ip, m_Port);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpBroadcast::CMaaUdpBroadcast(CMaaFdSockets * pFdSockets, _Port Port)
:   CMaaUdpClient(pFdSockets, Port, (_IP)0)
{
    // Variable to set the broadcast option with setsockopt ().
    BOOL fBroadcast = 1;//TRUE;

    if  (SOCKET_ERROR == setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *)&fBroadcast, sizeof(BOOL)))
    {
        Error("setsockopt()");
    }
}
//---------------------------------------------------------------------------
void CMaaUdpBroadcast::Send(const void * pBuffer, int BufferSize)
{
    CMaaUdpClient::Send(pBuffer, BufferSize, INADDR_BROADCAST);
}
//---------------------------------------------------------------------------
void CMaaUdpBroadcast::Send(const void * pBuffer, int BufferSize, _Port Port)
{
    CMaaUdpSocket::Send(pBuffer, BufferSize, INADDR_BROADCAST, Port);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaUdpServer::CMaaUdpServer(CMaaFdSockets * pFdSockets, _Port Port, _IP BindIp, bool bReuse)
:   CMaaUdpSocket(pFdSockets)
{
    IfDebug(100) { DP(printf("CMaaUdpServer::CMaaUdpServer(%d)\n", Port);) }
    m_s.m_sin.sin_family = AF_INET;
    m_s.m_sin.sin_port   = htons(Port);
    m_s.m_sin.sin_addr.s_addr = htonl(BindIp);

    if  (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
        //#define P_PARAM (const char *)
#else
        int On = 1;
        //#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof(On));
    }

    if  (SOCKET_ERROR == ::bind(m_Socket, (struct sockaddr *)&m_s.m_sin, sizeof(m_s.m_sin)))
    {
        Error("bind()");
    }
    AddFdSocket();
}
//---------------------------------------------------------------------------
CMaaUdpServer::CMaaUdpServer(CMaaFdSockets * pFdSockets, _Port Port, _IP6 BindIp, bool bReuse)
:   CMaaUdpSocket(pFdSockets, AF_INET6)
{
    IfDebug(100) { DP(printf("CMaaUdpServer::CMaaUdpServer ( %d )\n", Port);) }
    m_s.m_sin6.sin6_family = AF_INET6;
    m_s.m_sin6.sin6_port   = htons(Port);
    memcpy(m_s.m_sin6.sin6_addr.s6_addr, BindIp.const_ptr(), 16);

    if  (bReuse)
    {
#ifndef __unix__
        BOOL On = true;
        //#define P_PARAM (const char *)
#else
        int On = 1;
        //#define P_PARAM
#endif
        setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, P_PARAM &On, sizeof(On));
    }

    if  (SOCKET_ERROR == ::bind(m_Socket, (struct sockaddr *)&m_s.m_sin6, sizeof(m_s.m_sin6)))
    {
        Error("bind()");
    }
    AddFdSocket();
}
void CMaaUdpServer::AddMembership(_IP MulticastAddr /*= CMaaIpAddress(224,0,0,0)*/, _IP InterfaceIpOrIndex /*= 0*/)
{
#ifdef _WIN32
    struct ip_mreq mreq;   // Used in adding or dropping multicasting addresses
    mreq.imr_multiaddr.s_addr = htonl(MulticastAddr);
    mreq.imr_interface.s_addr = htonl(InterfaceIpOrIndex);

    if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR*) & mreq, sizeof(mreq)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
    }
#else
    //StaticError("Not implemented");
    //https://www.tenouk.com/Module41c.html
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = htonl(MulticastAddr);
    mreq.imr_interface.s_addr = htonl(InterfaceIpOrIndex);
    if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
    {
        StaticError("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
    }
#endif
}
void CMaaUdpServer::DropMembership(_IP MulticastAddr /*= CMaaIpAddress(224,0,0,0)*/, _IP InterfaceIpOrIndex /*= 0*/)
{
#ifdef _WIN32
    struct ip_mreq mreq;   // Used in adding or dropping multicasting addresses
    mreq.imr_multiaddr.s_addr = htonl(MulticastAddr);
    mreq.imr_interface.s_addr = htonl(InterfaceIpOrIndex);

    if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char FAR*) & mreq, sizeof(mreq)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_DROP_MEMBERSHIP) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_DROP_MEMBERSHIP) failed");
    }
#else
    //StaticError("Not implemented");
    //https://www.tenouk.com/Module41c.html
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = htonl(MulticastAddr);
    mreq.imr_interface.s_addr = htonl(InterfaceIpOrIndex);
    if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
    {
        StaticError("setsockopt(for IPPROTO_IP, IP_DROP_MEMBERSHIP) failed");
    }
#endif
}
void CMaaUdpServer::JoinGroup6(_IP6 MulticastAddr, unsigned int InterfaceIndex)
{
#ifdef _WIN32
    struct ipv6_mreq mreq;   // Used in adding or dropping multicasting addresses
    memcpy(mreq.ipv6mr_multiaddr.s6_addr, MulticastAddr.ptr(), 16);
    mreq.ipv6mr_interface = InterfaceIndex;

    if (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char FAR*) & mreq, sizeof(mreq)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_JOIN_GROUP) failed");
    }
#else
    //InterfaceIndex = if_nametoindex("enp3s0"); // 0 on error +errno set
    // TODO: IPv6
    //StaticError("Not implemented");
    //https://www.tenouk.com/Module41c.html
    struct ipv6_mreq mreq;   // Used in adding or dropping multicasting addresses
    memcpy(mreq.ipv6mr_multiaddr.s6_addr, MulticastAddr.ptr(), 16);
    mreq.ipv6mr_interface = InterfaceIndex;
    if (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&mreq, sizeof(mreq)) < 0)
    {
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_JOIN_GROUP) failed");
    }
#endif
}
void CMaaUdpServer::LeaveGroup6(_IP6 MulticastAddr, unsigned int InterfaceIndex)
{
#ifdef _WIN32
    struct ipv6_mreq mreq;   // Used in adding or dropping multicasting addresses
    memcpy(mreq.ipv6mr_multiaddr.s6_addr, MulticastAddr.ptr(), 16);
    mreq.ipv6mr_interface = InterfaceIndex;

    if (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char FAR*) & mreq, sizeof(mreq)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_ADD_MEMBERSHIP) failed");
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_LEAVE_GROUP) failed");
    }
#else
    //InterfaceIndex = if_nametoindex("enp3s0"); // 0 on error +errno set
    // TODO: IPv6
    //StaticError("Not implemented");
    //https://www.tenouk.com/Module41c.html
    struct ipv6_mreq mreq;   // Used in adding or dropping multicasting addresses
    memcpy(mreq.ipv6mr_multiaddr.s6_addr, MulticastAddr.ptr(), 16);
    mreq.ipv6mr_interface = InterfaceIndex;
    if (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char*)&mreq, sizeof(mreq)) < 0)
    {
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_LEAVE_GROUP) failed");
    }
#endif
}
void CMaaUdpServer::SetIpMulticastIfaceAndTTL(_IP IfaceIpOrIfIndex, int ttl) // use Sent//sendto internally// to send to multicast IP and port later
{
#ifdef _WIN32
    DWORD dwIfaceIpOrIfIndex = htonl(IfaceIpOrIfIndex);
    if  (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_IF, (char FAR *)&dwIfaceIpOrIfIndex, sizeof (DWORD)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_MULTICAST_IF) failed");
    }
    int iTTL = ttl;
    if  (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char FAR *)&iTTL, sizeof (int)) == SOCKET_ERROR)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
    }
#else
    //StaticError("SetIpMulticastTTL for unix -- Not implemented");
    _IP nIfaceIp = htonl(IfaceIpOrIfIndex);
    if  (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&nIfaceIp, sizeof (_IP)) < 0)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_MULTICAST_IF) failed");
    }
    int iTTL = ttl;
    if  (setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&iTTL, sizeof (int)) < 0)
    {
        //Error("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
        StaticError("setsockopt(for IPPROTO_IP, IP_MULTICAST_TTL) failed");
    }
#endif
}
void CMaaUdpServer::SetIpMulticastIfaceAndHop6(unsigned int InterfaceIndex, int ttl) // use Sent//sendto internally// to send to multicast IP and port later
{
#ifdef _WIN32
    unsigned int dwInterfaceIndex = InterfaceIndex;
    if  (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char FAR*)&dwInterfaceIndex, sizeof(dwInterfaceIndex)) == SOCKET_ERROR)
    {
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_MULTICAST_IF) failed");
    }
    int iTTL = ttl;
    if  (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char FAR*)&iTTL, sizeof(int)) == SOCKET_ERROR)
    {
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_MULTICAST_HOPS) failed");
    }
#else
    //InterfaceIndex = if_nametoindex("enp3s0"); // 0 on error +errno set
    // TODO: IPv6
    //StaticError("SetIpMulticastTTL for unix -- Not implemented");
    unsigned int nIfaceIp = InterfaceIndex;
    if  (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char*)&nIfaceIp, sizeof(_IP)) < 0)
    {
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_MULTICAST_IF) failed");
    }
    int iTTL = ttl;
    if  (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&iTTL, sizeof(int)) < 0)
    {
        StaticError("setsockopt(for IPPROTO_IPV6, IPV6_MULTICAST_HOPS) failed");
    }
#endif
}
//---------------------------------------------------------------------------
int CMaaUdpSocket::Recv(void * pBuffer, int BufferSize)
{
    socklen_t len = sizeof(m_t.m_sin);

    memset(&m_t.m_sin, 0, sizeof(m_t.m_sin));
    const int Ret = recvfrom(m_Socket, (char *)pBuffer, BufferSize, 0, (struct sockaddr *)&m_t.m_sin, &len);
#ifdef TL_EPOLL
    int e = Ret < 0 ? errno : 0;
#ifndef TL_UDP_EPOLLET
    m_pFdSockets->ResetRAState(this, eRead);
#endif
#endif
#ifdef _WIN32
    m_pFdSockets->ResetRAState(this, eRead);
#endif
    if  (Ret == SOCKET_ERROR)
    {
#ifdef TL_UDP_EPOLLET
        if  (e == EAGAIN)
        {
            m_pFdSockets->ResetRAState(this, eRead);
        }
#endif
        Error("recvfrom()");
    }
    else
    {
        UpdateHrTime(e_fds_last_receive_time);
    }
    int Mode;
    //if   (!m_pFdSockets->m_SockHash.Find(this, &Mode))
    //if  (!m_pFdSockets->SockHashFind(this, &Mode))
    Mode = m_pFdSockets->SockHashFindMode(this);
    {
        Mode &= ~eEventRead;
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaUdpSocket::Recv(void * pBuffer, int BufferSize, _IP * pIp, _Port * pPort)
{
    const int len = Recv(pBuffer, BufferSize);
    if  (pIp)
    {
        *pIp = GetSourceIp();
    }
    if  (pPort)
    {
        *pPort = GetSourcePort();
    }
    return len;
}
//---------------------------------------------------------------------------
_IP CMaaUdpSocket::GetSourceIp() const noexcept
{
    return htonl(m_t.m_sin.sin_addr.s_addr);
}
//---------------------------------------------------------------------------
_Port CMaaUdpSocket::GetSourcePort() const noexcept
{
    return htons(m_t.m_sin.sin_port);
}
//---------------------------------------------------------------------------
_Port CMaaUdpSocket::GetBindedPort() const noexcept
{
    return htons(m_s.m_sin.sin_port);
}
//---------------------------------------------------------------------------

// >> IPv6 UDP
#ifdef TOOLSLIB_USE_IPV6
//---------------------------------------------------------------------------
int CMaaUdpSocket::Recv6(void * pBuffer, int BufferSize)
{
    socklen_t len = sizeof(m_t.m_sin6);

    memset(&m_t.m_sin6, 0, sizeof(m_t.m_sin6));
    const int Ret = recvfrom(m_Socket, (char *)pBuffer, BufferSize, 0, (struct sockaddr *)&m_t.m_sin6, &len);
#ifdef TL_EPOLL
    int e = Ret < 0 ? errno : 0;
#ifndef TL_UDP_EPOLLET
    m_pFdSockets->ResetRAState(this, eRead);
#endif
#endif
    if  (Ret == SOCKET_ERROR)
    {
#ifdef TL_UDP_EPOLLET
        if  (e == EAGAIN)
        {
            m_pFdSockets->ResetRAState(this, eRead);
        }
#endif
        Error("recvfrom()");
    }
    else
    {
        UpdateHrTime(e_fds_last_receive_time);
    }
    int Mode;
    //if   (!m_pFdSockets->m_SockHash.Find(this, &Mode))
    //if  (!m_pFdSockets->SockHashFind(this, &Mode))
    Mode = m_pFdSockets->SockHashFindMode(this);
    {
        Mode &= ~eEventRead;
        //m_pFdSockets->m_SockHash.AddOver(this, Mode);
        m_pFdSockets->SockHashAddOver(this, Mode);
    }
    return Ret;
}
//---------------------------------------------------------------------------
int CMaaUdpSocket::Recv6(void * pBuffer, int BufferSize, _byte* pIp6, _Port * pPort)
{
    const int len = Recv6(pBuffer, BufferSize);
    if  (pIp6)
    {
        GetSourceIp6(pIp6);
    }
    if  (pPort)
    {
        *pPort = GetSourcePort6();
    }
    return len;
}
//---------------------------------------------------------------------------
void CMaaUdpSocket::GetSourceIp6(_byte * pIp6) const noexcept
{
    memcpy(pIp6, m_t.m_sin6.sin6_addr.s6_addr, 16);
}
//---------------------------------------------------------------------------
_Port CMaaUdpSocket::GetSourcePort6() const noexcept
{
    return htons(m_t.m_sin6.sin6_port);
}
//---------------------------------------------------------------------------
_Port CMaaUdpSocket::GetBindedPort6() const noexcept
{
    return htons(m_s.m_sin6.sin6_port);
}
//---------------------------------------------------------------------------
#endif
// << IPv6 UDP

//---------------------------------------------------------------------------
CMaaFdSockets::CMaaFdSockets(CMaaSockThread * pThread)
:   m_SockHash(FD_SETSIZE),
#ifdef _WIN32
    m_SelectSockets(CMaa_MAX_SOCKS),
    m_SelectModes(CMaa_MAX_SOCKS),
    m_Select_hh(CMaa_MAX_SOCKS_EVENTS),
#else
#ifdef TL_EPOLL
    m_SelectSockets(TL_EPOLL_EVENTS),
    m_SelectModes(TL_EPOLL_EVENTS),
    m_Select_rev(TL_EPOLL_EVENTS + 16),
    m_Select_wev(TL_EPOLL_EVENTS + 16),
#else
    m_SelectSockets(CMaa_MAX_SOCKS),
    m_SelectModes(CMaa_MAX_SOCKS),
    m_Select_rev(CMaa_MAX_SOCKS + 16),
    m_Select_wev(CMaa_MAX_SOCKS + 16),
#endif
#endif
    m_pThread(pThread),
#ifdef __SOCK_NEW_TIMERS
    m_Time(!gpSockStartup ? GetHRTime(true) : gpSockStartup->m_Time)
#endif
{
    m_bLook = false;
    m_bForceMode = false;
    //1111111
    //FD_ZERO(&m_Read);
    //FD_ZERO(&m_Write);
    //FD_ZERO(&m_Exept);
    m_Timeval.tv_sec = 20;
    m_Timeval.tv_usec = 0;
#ifdef _WIN32
    if  (m_SelectSockets.MaxIndex() < CMaa_MAX_SOCKS || m_SelectModes.MaxIndex() < CMaa_MAX_SOCKS || m_Select_hh.MaxIndex() < CMaa_MAX_SOCKS_EVENTS)
#else
#ifdef TL_EPOLL
    if  (m_SelectSockets.MaxIndex() < TL_EPOLL_EVENTS || m_SelectModes.MaxIndex() < TL_EPOLL_EVENTS || m_Select_rev.MaxIndex() < TL_EPOLL_EVENTS + 16 || m_Select_wev.MaxIndex() < TL_EPOLL_EVENTS + 16)
#else
    if  (m_SelectSockets.MaxIndex() < CMaa_MAX_SOCKS || m_SelectModes.MaxIndex() < CMaa_MAX_SOCKS || m_Select_rev.MaxIndex() < CMaa_MAX_SOCKS + 16 || m_Select_wev.MaxIndex() < CMaa_MAX_SOCKS + 16)
#endif
#endif
    {
        CMaaSocket::StaticError("Data alloc in CMaaFdSockets::CMaaFdSockets()");
    }
#ifdef TL_EPOLL
    m_epollfd = epoll_create(TL_EPOLL_EVENTS);
    if  (m_epollfd < 0)
    {
        CMaaSocket::StaticError("epoll_create");
    }
#endif
}
//---------------------------------------------------------------------------
CMaaFdSockets::~CMaaFdSockets ()
{
    while(m_Timers.LookAtFront())
    {
        m_Timers.LookAtFront()->Detach();
    }
#ifdef TL_EPOLL
    if  (m_epollfd != -1)
    {
        if  (close(m_epollfd))
        {
            DP(printf("CMaaFdSockets::~CMaaFdSockets () - close() error\n");)
        }
    }
#endif
}
#ifdef __SOCK_NEW_TIMERS
_qword CMaaFdSockets::GetTime() noexcept
{
    //m_TimerMutex.LockM();
    //const _qword t = m_Time.GetTime(1000000);
    //m_TimerMutex.UnLockM();
    //return t;

    //return (_qword)m_Time.GetTimeEx(1000000);
    return (_qword)m_Time.GetTime(1000000);
}
#endif
//---------------------------------------------------------------------------
void CMaaFdSockets::close_epoll_fd() noexcept //FXX_epollfd() // on fork()
{
#ifdef TL_EPOLL
    if  (m_epollfd != -1)
    {
        if  (close(m_epollfd))
        {
            DP(printf("FXX_epollfd() - close() error\n");)
        }
    }
    m_epollfd = -1;
    //    printf("FXX_epollfd(): m_epollfd = %d\n", m_epollfd);
#endif
}
//---------------------------------------------------------------------------
void CMaaFdSockets::SetCloseOnExec_epool_fd(bool bSet, bool bThrow) // for forks
{
#ifdef TL_EPOLL
    if  (m_epollfd != -1)
    {
        CMaaFile::StaticSetCloseOnExec(m_epollfd, bSet, bThrow);
    }
#endif
}
//---------------------------------------------------------------------------
void CMaaFdSockets::LookEp(bool bLook) noexcept
{
    m_bLook = bLook;
    /*
    if (bLook)
    {
        printf("LoopEp()...\n");
    }
    */
}
//---------------------------------------------------------------------------
void CMaaFdSockets::OnModeChangeDelayed(CMaaFdSocket* pSock) noexcept
{
    if (!pSock->m_ModeChangeItem.Next)
    {
        m_ModeChangeList.AddAtBack(&pSock->m_ModeChangeItem);
    }
}
//---------------------------------------------------------------------------
bool CMaaFdSockets::AddSocket(CMaaSocket *pSock, int Mode, int epoll_ev)
{
    if (!pSock)
    {
        return false;
    }
    //printf ( "%p: AddSocket %p %d\n", this, pSock, (int)pSock->GetSocket() ); fflush(stdout);
    IfDebug(201)
    {
        DP(printf("%p: AddSocket(%p, %x, %x)\n", this, pSock, Mode, epoll_ev); fflush(stdout);)
    }
    if  (!m_SockHash.Find(pSock))
    {
        RemoveSocket(pSock);
    }
    //if (!m_SockHash.AddOver(pSock, Mode))
    pSock->m_Mode = 0; // Mode;
    pSock->m_Act1 = 0; /*~~~*/
    if (!m_SockHash.AddOver(pSock, 0))
    {
        m_SockList.AddAtBack(&pSock->m_ListItem);

        IfDebug(201)
        {
            DP(printf("AddSocket %p - ok\n", pSock); fflush(stdout);)
        }
        const SOCKET s = pSock->GetSocket();
#if defined(TL_EPOLL) || defined(_WIN32)
        m_SockMap.AddOver(s, pSock);
        pSock->m_RealState = 0;
#endif
        if  (IsWinStdioSock(s))
        {
            //pSock->m_Mode = Mode;
            return true;
        }
#ifdef _WIN32
        ChangeMode(pSock, 0, Mode, true);
#else
        /*
111111111

          if   ( ( Mode & eRead ) )
          {
               FD_SET( s, & m_Read );
          }
          if   ( ( Mode & eWrite ) )
          {
               FD_SET( s, & m_Write );
          }
          if   ( ( Mode & eExept ) )
          {
               FD_SET( s, & m_Exept );
          }
*/
#endif
        //pSock->m_Mode = 0;
#ifdef TL_EPOLL
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.data.fd = s;

        if  (epoll_ev != -1)
        {
            ev.events = epoll_ev;
        }
        else
        {
#ifdef TL_EPOLLET
            ev.events = !pSock->m_AcceptFlag ? (int)EPOLLET : 0; // CONTINUE DEV
#else
            ev.events = 0;
#endif
            //ev.events |= EPOLLIN;
            ev.events |= EPOLLIN | EPOLLRDHUP | EPOLLHUP;
            if  (!pSock->m_AcceptFlag)// && !(Mode & eNoLET))
            {
                ev.events |= EPOLLOUT;
            }
            ev.events |= EPOLLERR; // always is set by epoll
        }
        DP(printf("ADD: %d %x\n", s, (int)ev.events);)
        if  (m_epollfd != -1 && epoll_ctl(m_epollfd, EPOLL_CTL_ADD, s, &ev) == -1)
        {
            DP(perror("epoll_ctl: ADD");)
            return false;
        }
        else if (m_epollfd != -1)
        {
            //DP1(printf("epoll_ctl: ADD: (%d, %x) - ok, pid:%d, %s\n", s, ev.events, getpid(), (const char *)pSock->GetConnectionName());)
        }
        ChangeMode(pSock, 0, Mode, true);
#endif
    }
    return true;
}
//---------------------------------------------------------------------------
bool CMaaFdSockets::MoveToFront(CMaaSocket* pSock) noexcept
{
    if (!m_SockHash.Find(pSock) && pSock->m_ListItem.Next)
    {
        m_SockList.Release(&pSock->m_ListItem);
        m_SockList.AddAtFront(&pSock->m_ListItem);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool CMaaFdSockets::MoveToTail(CMaaSocket* pSock) noexcept
{
    if (!m_SockHash.Find(pSock) && pSock->m_ListItem.Next)
    {
        m_SockList.Release(&pSock->m_ListItem);
        m_SockList.AddAtBack(&pSock->m_ListItem);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool CMaaFdSockets::RemoveSocket(CMaaSocket * pSock, bool bFromDestructor) noexcept
{
    //printf ( "%p: RemoveSocket %p%s %d\n", this, pSock, bFromDestructor ? " from destructor" : "", (int)pSock->GetSocket() ); fflush(stdout);
    IfDebug(201)
    {
        DP(printf("%p: RemoveSocket %p%s\n", this, pSock, bFromDestructor ? " from destructor" : ""); fflush(stdout);)
    }

    //int Mode = 0;
    //if (!m_SockHash.Remove(pSock, &Mode) && pSock)
    if  (pSock && !m_SockHash.Remove(pSock))
    {
        int Mode = pSock->m_Mode;
        m_SockList.Release(&pSock->m_ListItem);
        if (pSock->m_ModeChangeItem.Next)
        {
            m_ModeChangeList.Release(&pSock->m_ModeChangeItem);
        }
#if defined(TL_EPOLL) && defined(__SHAPERS)
        m_SockShRWHash.Remove(pSock);
#endif
#if defined(_WIN32) && defined(__SHAPERS)
        //m_SockShRWHash.Remove(pSock);
        if  (pSock->m_TcpSocketFlag)
        {
            ((CMaaTcpSocket*)pSock)->m_ModeShRW = 0;
        }
#endif
        const SOCKET s = pSock->GetSocket();
        IfDebug(201)
        {
            DP(printf("SOCKET=%d \n", (int)s); fflush(stdout);)
        }
        m_SockMap.Remove(s);
#if defined(TL_EPOLL) || defined(_WIN32)
        pSock->m_RealState = 0;
#endif
#ifdef TL_EPOLL
        //m_hActiveState.Remove(s);
        if (pSock->m_ActiveStateListItem.Next)
        {
            m_ActiveStateList.Release(&pSock->m_ActiveStateListItem);
        }
#endif
        if  (!IsWinStdioSock(s) && s != (SOCKET)-1) // -1 : 11.08.2017
        {
            /*111111
               FD_CLR( s, & m_Read );
               FD_CLR( s, & m_Write );
               FD_CLR( s, & m_Exept );
            */
#ifdef TL_EPOLL
            epoll_event ev;
            memset(&ev, 0, sizeof(ev));
            ev.data.fd = s;
            ev.events = 0;
            if  (m_epollfd != -1 && epoll_ctl(m_epollfd, EPOLL_CTL_DEL, s, &ev) == -1)
            {
                DP(CMaaString txt;
                txt.Format("epoll_ctl: EPOLL_CTL_DEL, %d %d", (int)m_epollfd, (int)s);
                perror(txt);)
                return false;
            }
            else if (m_epollfd != -1)
            {
                //DP1(printf("EPOLL_CTL_DEL %d = ok, pid:%d, %s\n", s, getpid(), (const char *)pSock->GetConnectionName());)
            }
#endif
        }
        return true;
    }
    if  (!bFromDestructor)
    {
        //DP1(printf("RemoveSocket(): m_SockHash not found for %p\n", pSock); fflush(stdout);)
        DP(printf("RemoveSocket(): m_SockHash not found for %p\n", pSock); fflush(stdout);)
    }
    return false;
}
//---------------------------------------------------------------------------
int CMaaFdSockets::IsWinStdioSock(SOCKET /*sock*/) const noexcept
{
#ifdef __unix__
    return 0;
#else
    return 0;
    /*
     int s = sock;
     return ( s == _fileno ( stdin ) || s == _fileno ( stdout ) || s == _fileno ( stderr ) ) ? 1 : 0;
     */
#endif
}
//---------------------------------------------------------------------------
void CMaaFdSockets::SetTimeOut(int Sec, int MicroSec) noexcept
{
    m_Timeval.tv_sec  = Sec;
    m_Timeval.tv_usec = MicroSec;
}
//---------------------------------------------------------------------------
int CMaaFdSockets::GetSockCount() const noexcept
{
    return m_SockHash.GetItemCount();
}
//---------------------------------------------------------------------------
int CMaaFdSockets::GetSockets(CMaaSocket ** m, int * pModes, int k) const noexcept
{
    CMaaDLink_pItem<CMaaSocket> * p = m_SockList.LookAtFront();
    int i = 0;
    for (; p && i < k; i++)
    {
        pModes[i] = (m[i] = p->m_pItem)->m_Mode;
        p = m_SockList.Next(p);
    }
    return i;
}
//---------------------------------------------------------------------------
#ifdef _WIN32_000000000
void pr3e(const char * ttt, HANDLE * h, HANDLE h0, HANDLE h1, CMaaSocket *s)
{
    char txt[512];
    txt[0] = 0;
    for (int i = 0; i < 3; i++)
    {
        if  (*h == h0)
        {
            strcat(txt, "0");
        }
        else if (*h == h1)
        {
            strcat(txt, "1");
        }
        else if (*h == s->m_Ev)
        {
            strcat(txt, "e");
        }
        else
        {
#ifdef __SHAPERS
            if (s->m_TcpSocketFlag)
            {
                CMaaTcpSocket* t = (CMaaTcpSocket*)s; //dynamic_cast<CMaaTcpSocket*>(s);
                if  (*h == t->m_SndLLShaper.m_hEv)
                {
                    strcat(txt, "s");
                }
                else if (*h == t->m_RcvLLShaper.m_hEv)
                {
                    strcat(txt, "r");
                }
                else
                {
                    strcat(txt, "?");
                }
            }
            else
            {
                strcat(txt, "?");
            }
#endif
        }
        if  (WaitForSingleObject(*h, 0) == WAIT_OBJECT_0)
        {
            strcat(txt, "+");
        }
        else
        {
            strcat(txt, "-");
        }
        h++;
    }
    printf(ttt, txt);
}
//---------------------------------------------------------------------------
bool CMaaFdSockets::log1(int point)
{
    /*
     CMaaFile f("c:\\maa\\timerlog.txt", CMaaFile::eAC_SrSw, eNoExcept);
     f.fprintf("%t log1: %d\n", time(nullptr), point);
     */
    return false;
}
#endif
#ifdef _WIN32
//static HANDLE hEv0 = nullptr;
//static HANDLE hEv1 = nullptr;
static WSAEVENT hEv0 = nullptr;
static WSAEVENT hEv1 = nullptr;
int CMaaFdSockets::Select()
{
    struct timeval Timeval;
    Timeval = m_Timeval;

    int k = (warning_int)m_SockHash.GetMaxIndex();

    CMaaSocket ** Sockets = m_SelectSockets;
    int * Modes = m_SelectModes;

    //try
    {
        //k = m_SockHash.EnumerateItems(k, Sockets, Modes);
        k = GetSockets(Sockets, Modes, k);

        WSASetLastError(0);
        {
            int us = GetWaitForTime(Timeval.tv_sec * 1000000 + Timeval.tv_usec);
            if  (m_bForceMode)
            {
                us = 0;
                m_bForceMode = false;
            }
            Timeval.tv_sec = us / 1000000;
            Timeval.tv_usec = us % 1000000;
        }

        //HANDLE* hh = m_Select_hh;
        WSAEVENT* hh = m_Select_hh;
        //hh[0] = hh[1] = m_pThread->m_hWakeup; // one of [0]/[1] was reserved for thread exit event
        hh[0] = m_pThread->m_hWakeup;

        int i;
        for (i = 0; i < k; i++)
        {
#ifdef __SHAPERS
            const int idx = 1 + 3 * i;
#else
            const int idx = 1 + i;
#endif

            hh[idx] = Sockets[i]->m_Ev;
#ifdef __SHAPERS
            hh[idx + 1] = hEv0;
            hh[idx + 2] = hEv0;
#endif
            //Sockets[i]->SetTestValue(-1);
            //Sockets[i]->SetTestMessage(CMaaString::sFormat("%p: to = %d\n", Sockets[i], Timeval.tv_sec * 1000000 + Timeval.tv_usec));
            if  (Sockets[i]->GetFdMode() & CMaaEnum::eHold)
            {
                hh[idx] = hEv0;
            }
            else if (Sockets[i]->m_TcpSocketFlag)
            {
#ifdef __SHAPERS
                CMaaTcpSocket* t = (CMaaTcpSocket*)(Sockets[i]);// dynamic_cast<CMaaTcpSocket*>(Sockets[i]);
                int wr = -1, rr = -1;

                //CMaaWin32Locker_Locker_gLock;
                //Locker_Lock;
                gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
                if  (t->m_SndLLShaper.m_Shaper)
                {
                    wr = t->m_SndLLShaper.GetLenReserved(1000000);
                }
                if  (t->m_RcvLLShaper.m_Shaper)
                {
                    rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                }
                gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
                //Locker_UnLock;
                if  (rr == 0 && wr == 0)
                {
                    hh[idx] = hEv0;
                    hh[idx + 1] = t->m_SndLLShaper.m_hEv;
                    hh[idx + 2] = t->m_RcvLLShaper.m_hEv;
                }
                else if (rr && wr)
                {
                    //hh[idx + 1] = hh[idx + 2] = m_hEv0;
                }
                else if (rr == 0 && wr)
                {
                    hh[idx + 2] = t->m_RcvLLShaper.m_hEv;
                    if  ((Modes[i] & (eEventWrite | eWrite)) != eWrite)
                    {
                        hh[idx] = hEv0;
                    }
                }
                else if (rr && wr == 0)
                {
                    hh[idx + 1] = t->m_SndLLShaper.m_hEv;
                    //if   ((Modes[i] & (eEventRead|eRead)) != eRead)
                    if  (!(Modes[i] & (eRead | eExept)) || (Modes[i] & eEventRead))
                    {
                        hh[idx] = hEv0;
                    }
                }
                if ( (rr && ((Modes[i] & (eEventRead | eRead)) == (eEventRead | eRead))) || (wr && ((Modes[i] & (eEventWrite | eWrite)) == (eEventWrite | eWrite))) )
#else
                if ( ((Modes[i] & (eEventRead | eRead)) == (eEventRead | eRead)) || ((Modes[i] & (eEventWrite | eWrite)) == (eEventWrite | eWrite)) )
#endif
                {
                    hh[idx] = hEv1;
                }
            }
        }

        DP2(
        //CMaaWin32Locker_Locker_gLock;
        //Locker_Lock;
        {
#ifdef __SHAPERS
            for (int i = 1; i < 1 + 3 * k; i++)
#else
            for (int i = 1; i < 1 + k; i++)
#endif
            {
                void* p = hh[i];
                if  (hh[i] == hEv0)
                {
                    p = nullptr;
                }
                if  (hh[i] == hEv1)
                {
                    p = (void *)1;
                }
                __utf8_printf("%p ", p);
            }
            __utf8_printf("\n");
        }
        //Locker_UnLock;
        )

#ifdef __SHAPERS
        //DWORD w = WaitForMultipleObjects(1 + 3 * k, hh, FALSE, Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000);
        DWORD w = WSAWaitForMultipleEvents(1 + 3 * k, hh, FALSE, Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000, FALSE);
#else
        //DWORD w = WaitForMultipleObjects(1 + k, hh, FALSE, Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000);
        DWORD w = WSAWaitForMultipleEvents(1 + k, hh, FALSE, Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000, FALSE);
#endif

        //DP2(__utf8_printf("w = %d, a = %d\n", (int)w, m_hActiveState.GetItemCount());)
        DP2(__utf8_printf("w = %d, a = %d\n", (int)w, m_ActiveStateList.GetItemCount());)
        if  (w == WSA_WAIT_TIMEOUT) //WAIT_TIMEOUT)
        {
            w = WAIT_OBJECT_0;
        }
        //w -= WAIT_OBJECT_0;
        w -= WSA_WAIT_EVENT_0;
#ifdef __SHAPERS
        if  ((int)w < 0 || (int)w >= 1 + 3 * k)
#else
        if  ((int)w < 0 || (int)w >= 1 + k)
#endif
        {
            return -1;
        }
#ifdef __SHAPERS
        // win32 shapers wakeups performs rw events set, m_pThread->m_WakeUpFlags is unused
        ////bool bWakedUp = w < 2; //// && (m_pThread->m_WakeUpFlags & 1) : false;
        ////m_pThread->m_WakeUpFlags = 0; // reset wake up by shapers thread flag (0x01)
        ///* unsigned char c = */ m_pThread->m_WakeUpFlags.fetch_and(~0x01);
#endif
        if  (!w) // (w < 1)
        {
            //ResetEvent(m_pThread->m_hWakeup);
            WSAResetEvent(m_pThread->m_hWakeup);
        }
        /*
        else
        {
            __utf8_printf("(w=%d)", w);
        }
        */

        for (i = 0; i < k; i++)
        {
            int Mode;
            CMaaSocket* p;
            //if (!m_SockHash.Find(p = Sockets[i], &Mode))
            if (!m_SockHash.Find(p = Sockets[i]))
            {
                Mode = p->m_Mode;
                if  (Mode & CMaaEnum::eHold)
                {
                    continue;
                }
                WSANETWORKEVENTS ev;
                memset(&ev, 0, sizeof(ev));
                ev.lNetworkEvents = 0;
                //if (/*w == 1 + 3 * i && */ WaitForSingleObject(p->m_Ev, 0) == WAIT_OBJECT_0)
                if  (WSAWaitForMultipleEvents(1, &p->m_Ev, TRUE, 0, FALSE) == WSA_WAIT_EVENT_0)
                {
                    //WSAResetEvent(p->m_Ev);
                    WSAEnumNetworkEvents(p->GetSocket(), p->m_Ev, &ev);
                    //CMaaFile f("c:\\temp\\ev.txt", CMaaFile::eAC_SrSw, eNoExcept);
                    //f.fprintf("%p %02x\n", (void *)p->GetSocket(), (int)ev.lNetworkEvents);
                }
                /*
                if (w >= 1 && (w - 1) / 3 != i)
                {
                    //continue;
                }
                */
                if  (ev.lNetworkEvents)
                {
                    //__utf8_printf("%p %D lnetev=%x\n", p, (_qword)p->GetSocket(), (int)ev.lNetworkEvents);
                }
                int x = p->m_RealState;
                int Err = 0;
                if  ((ev.lNetworkEvents & FD_CONNECT) && ev.iErrorCode[FD_CONNECT_BIT])
                {
                    if  (!Err && ev.iErrorCode[FD_CONNECT_BIT] != WSAEWOULDBLOCK)
                    {
                        Err = ev.iErrorCode[FD_CONNECT_BIT];
                    }
                    ev.lNetworkEvents &= ~FD_CONNECT;
                }
                if  ((ev.lNetworkEvents & FD_READ) && ev.iErrorCode[FD_READ_BIT])
                {
                    if  (!Err && ev.iErrorCode[FD_READ_BIT] != WSAEWOULDBLOCK)
                    {
                        Err = ev.iErrorCode[FD_READ_BIT];
                    }
                    ev.lNetworkEvents &= ~FD_READ;
                }
                if  ((ev.lNetworkEvents & FD_WRITE) && ev.iErrorCode[FD_WRITE_BIT])
                {
                    if  (!Err && ev.iErrorCode[FD_WRITE_BIT] != WSAEWOULDBLOCK)
                    {
                        Err = ev.iErrorCode[FD_WRITE_BIT];
                    }
                    ev.lNetworkEvents &= ~FD_WRITE;
                }
                if  ((ev.lNetworkEvents & FD_CLOSE) && ev.iErrorCode[FD_CLOSE_BIT])
                {
                    if  (!Err && ev.iErrorCode[FD_CLOSE_BIT] != WSAEWOULDBLOCK)
                    {
                        Err = ev.iErrorCode[FD_CLOSE_BIT];
                    }
                    ev.lNetworkEvents &= ~FD_CLOSE;
                }
                if  ((ev.lNetworkEvents & FD_ACCEPT) && ev.iErrorCode[FD_ACCEPT_BIT])
                {
                    if  (!Err && ev.iErrorCode[FD_ACCEPT_BIT] != WSAEWOULDBLOCK)
                    {
                        Err = ev.iErrorCode[FD_ACCEPT_BIT];
                    }
                    ev.lNetworkEvents &= ~FD_ACCEPT;
                }
                /*
                if (ev.lNetworkEvents & FD_ACCEPT)
                {
                    x |= eRead;
                    Mode |= eEventRead;
                }
                */
                if  (ev.lNetworkEvents & FD_READ)
                {
                    x |= eRead;
                    Mode |= eEventRead;
                    //Info.Append("R");
                }
                /*
                if (((int)w) == 1 + 3 * i + 2)
                {
                    x |= eRead;
                    Mode |= eEventRead;
                    //Info.Append("R");
                }
                */
                /*
                if (ev.lNetworkEvents & FD_CLOSE)
                {
                    //x |= eRead;
                    Mode |= eEventClose;
                    //Info.Append("R");
                }
                */
                if  (ev.lNetworkEvents & FD_WRITE)
                {
                    x |= eWrite;
                    Mode |= eEventWrite;
                    //Info.Append("W");
                }
                /*
                if (((int)w) == 1 + 3 * i + 1)
                {
                    x |= eWrite;
                    Mode |= eEventWrite;
                    //Info.Append("W");
                }
                */
                p->m_wsa_ev |= ev.lNetworkEvents;
                /*
                if (Err)
                {
                    p->m_Err = Err;
                    if (p->m_TcpSocketFlag)
                    {
                        CMaaTcpSocket* t = (CMaaTcpSocket*)p;// dynamic_cast<CMaaTcpSocket*>(p);
                        if (!t->m_CloseTimer.IsStarted())// && p->m_CloseTimer.GetPeriod() > 2000)
                        {
                            t->m_CloseTimer.OnClose(t, false);
                        }
                    }
                    //Info.Append("H");
                }
                */
                if  (Err)
                {
                    p->m_Err = Err;
                    x |= eExept;
                    Mode |= eEventExept;
                    //Info.Append("E");
                }
#ifdef __SHAPERS000000000
                if  (0 && p/*Sockets[i]*/->m_TcpSocketFlag)
                {
                    CMaaTcpSocket* t = (CMaaTcpSocket*)p; // dynamic_cast<CMaaTcpSocket*>(Sockets[i]);
                    /*
                    int wr = -1, rr = -1;
                    if (!(Mode & eConnect))
                    {
                        //CMaaWin32Locker_Locker_gLock;
                        //Locker_Lock;
                        gpSockStartup && gpSockStartup->m_UsrShaperMutex.LockM();
                        if (t->m_SndLLShaper.m_Shaper)
                        {
                            wr = t->m_SndLLShaper.GetLenReserved(1000000);
                        }
                        if (t->m_RcvLLShaper.m_Shaper)
                        {
                            rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                        }
                        gpSockStartup && gpSockStartup->m_UsrShaperMutex.UnLockM();
                        //Locker_UnLock;
                    }
                    */
                    //t->m_SndLLShaper.m_hEv
                    if  (WaitForSingleObject(hh[1 + 3 * i + 2], 0) == WAIT_OBJECT_0)
                    {
                        x |= eRead;
                        Mode |= eEventRead;
                    }
                    if  (WaitForSingleObject(hh[1 + 3 * i + 1], 0) == WAIT_OBJECT_0)
                    {
                        x |= eWrite;
                        Mode |= eEventWrite;
                    }
                }
#endif
                DP2(
                    /*if (gH1 == p->m_Socket)
                    {
                        __utf8_printf("p->m_RealState=%x = %x, a.x = %x\n", p->m_RealState, x, x & Mode & eAll);
                        gH1 = -1;
                    }*/
                    ;
                )
                p->m_RealState = x;
                x &= Mode & eAll;
                if  (x)// || (ev.lNetworkEvents & (FD_CLOSE | FD_CONNECT | FD_ACCEPT)))
                {
                    ////m_hActiveState.AddOver(p->m_Socket, x);
                }
                const int a = x;
                if  (p)
                {
                    //m_SockHash.AddOver(p, Mode);
                    p->m_Mode = Mode; // 30.02.2024

                    x = p->GetFdMode();
                    x = x & ~(eEventRead | eEventWrite);

                    x = x & ~(eShNoRead | eShNoWrite);

                    x |= (a & eAll) << eEventShift;
                    ChangeModeRaw(p, x);

                    // shaper21
#ifdef __SHAPERS
                    if (p/*Sockets[i]*/->m_TcpSocketFlag)
                    {
                        CMaaTcpSocket* t = (CMaaTcpSocket*)p; // dynamic_cast<CMaaTcpSocket*>(Sockets[i]);
                        //t->m_ModeShRW = 0;
                        int wr = -1, rr = -1;
                        if  (!(x & eConnect))
                        {
                            //CMaaWin32Locker_Locker_gLock;
                            //Locker_Lock;
                            gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
                            if  (t->m_SndLLShaper.m_Shaper)
                            {
                                wr = t->m_SndLLShaper.GetLenReserved(1000000);
                            }
                            if  (t->m_RcvLLShaper.m_Shaper)
                            {
                                rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                            }
                            gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
                            //Locker_UnLock;
                        }
                        int mm = 0;
                        if  (!rr)
                        {
                            x |= eShNoRead;
                        }
                        else
                        {
                            mm |= t->m_ModeShRW & eRead;
                            t->m_ModeShRW &= ~eRead;
                        }
                        if  (!wr)
                        {
                            x |= eShNoWrite;
                        }
                        else
                        {
                            mm |= t->m_ModeShRW & eWrite;
                            t->m_ModeShRW &= ~eWrite;
                        }
                        if  (mm)
                        {
                            //__utf8_printf("[ChangeMode(p, x | mm  %x);]", x | mm);
                            ChangeMode(p, 0, x | mm, true);
                        }
                        else if (!rr || !wr)
                        {
                            ChangeModeRaw(p, x);
                        }
                    }
#endif // __SHAPERS
                    // shaper21
                }
            }
        }

        bool bWakeUp = false;
        for (i = 0; i < k; i++)
        {
            // cycle expand
            CMaaSocket* pSock = Sockets[i];
            try
            {
                while (1)
                {
                    // 0 on ERROR
                    int Mode;
                    //if  (m_SockHash.Find(Sockets[i], &Mode) || (Mode & eHold))
                    if (m_SockHash.Find(pSock) || ((Mode = pSock->m_Mode) & eHold))
                    {
                        break;
                    }
                    try
                    {
                        //unsigned int Act = 0;
                        //CMaaSocket* pSock = Sockets[i];
                        SOCKET s = pSock->GetSocket();

                        if ((Mode & eEventExept))
                        {
                            if (pSock->m_Err != WSAEWOULDBLOCK)
                            {
                                int error = pSock->m_Err;
                                WSASetLastError(error);
                                pSock->ChangeFdMode(pSock->Notify_Error());
                            }
                        }
#ifdef __FORCED_CLOSE_NOTIFICATION
                        else if ((Mode & eEventRead) && (Mode & (eRead | eConnect | eExcept)) == eExcept)
                        {
                            if (pSock->m_TcpSocketFlag) // p
                            {
                                char c = 0;
                                int l = recv(pSock->GetSocket(), &c, 1, MSG_PEEK);

                                if (l == 0)
                                {
                                    //int ErrnoBackup = errno;
                                    WSASetLastError(CMaa_CONN_CLOSED_ERROR);
                                    pSock->ChangeFdMode(pSock->Notify_Error());
                                    //errno = ErrnoBackup;
                                }
                                else if (l < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
                                {
                                    //int ErrnoBackup = errno;
                                    pSock->ChangeFdMode(pSock->Notify_Error());
                                    //errno = ErrnoBackup;
                                }
                            }
                        }
#endif
                        //if  (Act)
                        //{
                            //ChangeModeEx(pSock, Act);
                        //}
                    }
                    catch (XTOOSockClose&)
                    {
                    }
                    catch (XTOOSockErr& /*SockErr*/)
                    {
                    }

                    // 1.1 FD_CONNECT
                    //if (m_SockHash.Find(Sockets[i], &Mode) || (Mode & eHold))
                    if (m_SockHash.Find(pSock) || ((Mode = pSock->m_Mode) & eHold))
                    {
                        break;
                    }
                    if (Sockets[i]->m_wsa_ev & FD_CONNECT)
                    {
                        try
                        {
                            //unsigned int Act = 0;
                            //CMaaSocket* pSock = Sockets[i]; // 30.04.2024
                            //SOCKET s = pSock->GetSocket();
                            //if (pSock->m_wsa_ev & FD_CONNECT)
                            {
                                pSock->m_wsa_ev &= ~FD_CONNECT;

                                SOCKET s = pSock->GetSocket();
                                try
                                {
                                    pSock->ChangeFdModeEx(((CMaaTcpSocket*)pSock)->ConnectAgain(100));
                                }
                                catch (XTOOSockErr& SockErr)
                                {
                                    //int xx;
                                    //if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                    if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock))
                                    {
                                        WSASetLastError(SockErr.GetErrorCode());
                                        pSock->ChangeFdMode(pSock->Notify_Error());
                                        //if  (Act)
                                        //{
                                            //ChangeModeEx(pSock, Act);
                                        //}
                                    }
                                    else
                                    {
                                        throw;
                                    }
                                }
                            }
                            //if  (Act)
                            //{
                                //__utf8_printf("R: ChangeMode(%p, 0x%x);\n", pSock, Act);
                                //ChangeModeEx(pSock, Act);
                            //}
                        }
                        catch (XTOOSockClose&)
                        {
                        }
                        catch (XTOOSockErr& /*SockErr*/)
                        {
                        }
                    }

                    // 1.2 FD_ACCEPT
                    //if (m_SockHash.Find(Sockets[i], &Mode) || (Mode & eHold))
                    if (m_SockHash.Find(pSock) || ((Mode = pSock->m_Mode) & eHold))
                    {
                        break;
                    }
                    if (pSock->m_wsa_ev & FD_ACCEPT)
                    {
                        try
                        {
                            //unsigned int Act = 0;
                            //CMaaSocket* pSock = Sockets[i]; // 30.04.2024
                            //SOCKET s = pSock->GetSocket();
                            //if (pSock->m_wsa_ev & FD_ACCEPT)
                            {
                                pSock->m_wsa_ev &= ~FD_ACCEPT;

                                SOCKET s = pSock->GetSocket();
                                try
                                {
                                    //Act = ((CMaaTcpSocket*)pSock)->Notify_Read();
                                    pSock->ChangeFdMode(pSock->Notify_Read());
                                }
                                catch (XTOOSockErr& SockErr)
                                {
                                    //int xx;
                                    //if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                    if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock))
                                    {
                                        WSASetLastError(SockErr.GetErrorCode());
                                        pSock->ChangeFdMode(pSock->Notify_Error());
                                        //if  (Act)
                                        //{
                                        //    ChangeModeEx(pSock, Act);
                                        //}
                                    }
                                    else
                                    {
                                        throw;
                                    }
                                }
                            }
                            //if  (Act)
                            //{
                                //__utf8_printf("R: ChangeMode(%p, 0x%x);\n", pSock, Act);
                                //ChangeModeEx(pSock, Act);
                            //}
                        }
                        catch (XTOOSockClose&)
                        {
                        }
                        catch (XTOOSockErr& /*SockErr*/)
                        {
                        }
                    }

                    // 1 FD_READ
                    bool bReadIsCalled = false;
                    //if (m_SockHash.Find(Sockets[i], &Mode) || (Mode & eHold))
                    if (m_SockHash.Find(pSock) || ((Mode = pSock->m_Mode) & eHold))
                    {
                        break;
                    }
                    try
                    {
                        //unsigned int Act = 0;
                        //CMaaSocket* pSock = Sockets[i]; // 30.04.2024
                        SOCKET s = pSock->GetSocket();
                        if ((Mode & eEventRead) && (Mode & (eRead | eConnect)))
                        {
                            //if ((Mode & eConnect) && (pSock->m_wsa_ev & FD_CONNECT))
                            /*
                            if (0)//(Mode & eConnect)
                            {
                                //bConnect = true;
                                try
                                {
                                    Act = ((CMaaTcpSocket*)pSock)->ConnectAgain(100);
                                }
                                catch (XTOOSockErr& SockErr)
                                {
                                    int xx;
                                    if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                    {
                                        Act = pSock->Notify_Error();
                                        if (Act)
                                        {
                                            ChangeMode(pSock, Act);
                                        }
                                    }
                                    else
                                    {
                                        throw;
                                    }
                                }
                            }
                            */
                            if (!(Mode & eConnect))
                            {
                                // shaper21
#ifdef __SHAPERS
                                if ((Mode & eShNoRead))
                                {
                                    pSock->ChangeFdMode(eDisableRead);
                                    //Act = pSock -> Notify_Read ();
                                    //Act = (Act &~ eRead) | eDisableRead;
                                    /*
                                    int m = 0;
                                    m_SockShRWHash.Find(pSock, &m);
                                    m |= eRead;
                                    m_SockShRWHash.AddOver(pSock, m);
                                    */
                                    if (pSock->m_TcpSocketFlag)
                                    {
                                        ((CMaaTcpSocket*)pSock)->m_ModeShRW |= eRead;
                                    }
                                }
                                else
#endif
                                {
                                    bReadIsCalled = true; //2022
                                    pSock->ChangeFdMode(pSock->Notify_Read());
                                    //29.04.2024+Ex//Act = Act & ~(Mode & (eRead | eWrite));
                                }
                                // shaper21

                                if (pSock->m_TcpSocketFlag)
                                {
                                    CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                    if (p->m_CloseTimer.IsStarted() && p->m_CloseTimer.GetPeriod() > 2000)
                                    {
                                        p->m_CloseTimer.Start1(2000);
                                    }
                                }
                                // bReadIsCalled = true;
                            }
                        }

                        //if  (Act)
                        //{
                            //__utf8_printf("R: ChangeMode(%p, 0x%x);\n", pSock, Act);
                            //ChangeModeEx(pSock, Act);
                        //}
                    }
                    catch (XTOOSockClose&)
                    {
                    }
                    catch (XTOOSockErr& /*SockErr*/)
                    {
                    }

                    // 2.1 on FD_CLOSE
                    //if (m_SockHash.Find(Sockets[i], &Mode) || (Mode & eHold))
                    if (m_SockHash.Find(pSock) || ((Mode = pSock->m_Mode) & eHold))
                    {
                        break;
                    }
                    //if ((Mode & eRead)) // read
                    if (pSock->m_wsa_ev & FD_CLOSE)
                    {
                        try
                        {
                            //unsigned int Act = 0;
                            //CMaaSocket* pSock = Sockets[i]; // 30.04.2024
                            //SOCKET s = pSock->GetSocket();
                            //if (pSock->m_wsa_ev & FD_CLOSE)
                            {
                                pSock->m_wsa_ev &= ~FD_CLOSE;
                                if (pSock->m_TcpSocketFlag)
                                {
                                    CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                    p->m_CloseTimer.OnClose(p, bReadIsCalled);
                                }
                                //Sockets [ i ]->m_bIsClosing = true;
                                //Act = pSock -> Notify_Read ();
                            }
                            /*
                            if (Act)
                            {
                                ChangeMode(pSock, Act);
                            }
                            */
                        }
                        catch (XTOOSockClose&)
                        {
                        }
                        catch (XTOOSockErr&)
                        {
                        }
                    }

                    // 2 FD_WRITE
                    //if (m_SockHash.Find(Sockets[i], &Mode) || (Mode & eHold))
                    if (m_SockHash.Find(pSock) || ((Mode = pSock->m_Mode) & eHold))
                    {
                        break;
                    }
                    try
                    {
                        //unsigned int Act = 0;
                        //CMaaSocket* pSock = Sockets[i]; // 30.04.2024
                        SOCKET s = pSock->GetSocket();

                        if ((Mode & eEventWrite) && (Mode & (eWrite | eConnect)))
                        {
                            /*
                            if (0)//(Mode & eConnect)
                            {
                                try
                                {
                                    Act = ((CMaaTcpSocket*)pSock)->ConnectAgain(100);
                                }
                                catch (XTOOSockErr& SockErr)
                                {
                                    int xx;
                                    if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                    {
                                        Act = pSock->Notify_Error();
                                        if (Act)
                                        {
                                            ChangeMode(pSock, Act);
                                        }
                                    }
                                    else
                                    {
                                        throw;
                                    }
                                }
                            }
                            */
                            if (!(Mode & eConnect))
                            {
                                const _qword x = pSock->m_TcpSocketFlag ? ((CMaaTcpSocket*)pSock)->GetDataSentSize() : 0;
                                // shaper21
                                int Act0 = 0;
#ifdef __SHAPERS
                                if (Mode & eShNoWrite)
                                {
                                    pSock->ChangeFdMode(eDisableWrite);
                                    //Act = pSock -> Notify_Write ();
                                    //Act = (Act &~ eWrite) | eDisableWrite;
                                    /*
                                    int m = 0;
                                    m_SockShRWHash.Find(pSock, &m);
                                    m |= eWrite;
                                    m_SockShRWHash.AddOver(pSock, m);
                                    */
                                    //__utf8_printf("/dw\\");
                                    if (pSock->m_TcpSocketFlag)
                                    {
                                        CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                        p->m_ModeShRW |= eWrite;
                                        //__utf8_printf("(|=: p->m_ModeShRW=%x)", p->m_ModeShRW);
                                    }
                                }
                                else
#endif
                                    // shaper21
                                {
                                    //pSock->ChangeFdMode(Act0 = (pSock->Notify_Write() & ~(Mode & (eRead))));
                                    pSock->ChangeFdMode(Act0 = pSock->Notify_Write());
                                    //Act = Act & ~(Mode & (eRead | eWrite)); // not correct if not all data sent at one notify call (SendData() called not for all data or data is appended after SendData()) and eWrite is returned
                                    //29.04.2024+Ex//Act = Act & ~(Mode & (eRead)); // ok111
                                    //__utf8_printf("%D", p->GetDataSentSize() - x);
                                }
                                if ((Act0 & eWrite) && pSock->m_TcpSocketFlag && ((CMaaTcpSocket*)pSock)->GetDataSentSize() != x)
                                {
                                    bWakeUp = true; // WakeUp();
                                }
                                //29.04.2024+Ex//Act = Act & ~(Mode & (eRead | eWrite)); // ok111
                            }
                        }

                        //if  (Act)
                        //{
                            //__utf8_printf("W: ChangeMode(%p, 0x%x);\n", pSock, Act);
                            //ChangeModeEx(pSock, Act);
                        //}
                    }
                    catch (XTOOSockClose&)
                    {
                    }
                    catch (XTOOSockErr& /*SockErr*/)
                    {
                    }
                    break;
                } // while(1)
            }
            catch(...)
            {
                for (i = 0; i < k; i++)
                {
                    CMaaSocket* pSock = Sockets[i];
                    if  (!m_SockHash.Find(pSock))
                    {
                        pSock->Notify_Act();
                    }
                }
                if (bWakeUp) // && (Act & eWrite))
                {
                    WakeUp();
                }
                throw;
            }
        }
        for (i = 0; i < k; i++)
        {
            CMaaSocket* pSock = Sockets[i];
            if  (!m_SockHash.Find(pSock))
            {
                pSock->Notify_Act();
            }
        }
        CMaaDLink_pItem<CMaaFdSocket>* p;
        while ((p = m_ModeChangeList.GetFromFront()))
        {
            CMaaSocket* pSock = (CMaaSocket*)p->m_pItem;
            pSock->Notify_Act();
        }
        if (bWakeUp) // && (Act & eWrite))
        {
            WakeUp();
        }
#ifdef __SHAPERS
        /*
        //if (bWakedUp) //m_pThread->m_WakeupPair && m_pThread->m_WakeupPair->m_Bytes > 0
        {
            // On waked up
            for (auto it(m_SockShRWHash.it()); it; ++it)
            {
                if (!m_SockHash.Find(it.key()) && it.data())
                {
                    //printf("ChangeMode(%p,eRead)\n", it.key()); fflush(stdout);
                    ChangeMode(it.key(), it.data());
                }
                it.Remove();
            }
        }
        */
        /*
        for (i = 0; 0 && i < k; i++)
        {
            int Mode;
            if (!m_SockHash.Find(Sockets[i], &Mode))
            {
                CMaaTcpSocket* p;
                if (Sockets[i]->m_TcpSocketFlag && (p = (CMaaTcpSocket*)Sockets[i])->m_ModeShRW)
                {
                    //__utf8_printf("(pmsrw=%x)", p->m_ModeShRW);
                    //DP1(if (p->m_ModeShRW & eWrite) __utf8_printf("\\ew/");)
                    ChangeMode(p, p->m_ModeShRW);
                }
            }
        }
        */
#endif
        if  (w >= 1)
        {
            return 1;
        }

    }
    //catch(...)
    //{
    //    throw;
    //}
    return 0;
}

#else // _WIN32
#ifdef TL_EPOLL

int CMaaFdSockets::Select()
{
    int i;
    struct timeval Timeval;
    int fStdin = 0;

    Timeval = m_Timeval;

    int k = TL_EPOLL_EVENTS;

    /*
    if  (!(m_SelectSockets.MaxIndex() >= (size_t)k && m_SelectModes.MaxIndex() >= (size_t)k &&
           m_Select_rev.MaxIndex() >= (size_t)(k + 16) &&
           m_Select_wev.MaxIndex() >= (size_t)(k + 16)
        ))
    {
        CMaaPtr<CMaaSocket *> Sockets(k, 1);
        CMaaPtr<int> Modes(k, 1);
        CMaaPtr<bool> rev(k + 16, 1), wev(k + 16, 1);
        m_SelectSockets.Swap(Sockets);
        m_SelectModes.Swap(Modes);
        m_Select_rev.Swap(rev);
        m_Select_wev.Swap(wev);
    }
    */

    CMaaSocket ** Sockets = m_SelectSockets;
    int * Modes = m_SelectModes;
    bool * rev = m_Select_rev;
    bool * wev = m_Select_wev;

    int Ret = -1;

    //try
    {
        int fSet = 0;
        bool ev = false;

        errno = 0;
        int us;
        {
            us = GetWaitForTime(Timeval.tv_sec * 1000000 + Timeval.tv_usec);
            if  (ev)
            {
                us = 0;
            }
            Timeval.tv_sec = us / 1000000;
            Timeval.tv_usec = us % 1000000;
        }
        //CMaaString Info;
        if  (m_epollfd == -1)
        {
            Ret = -2;
            fSet = 0;
            m_Timeval.tv_sec = 0;
            m_Timeval.tv_usec = 100000;
        }
        else
        {
            //Ret = epoll_wait(m_epollfd, m_e2v, TL_EPOLL_EVENTS, m_hActiveState.GetItemCount() > 0 ? 0 : Timeval.tv_sec < 0x7fffffff / 1000 - 1 ? Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000 : 0x7fffffff);
            Ret = epoll_wait(m_epollfd, m_e2v, TL_EPOLL_EVENTS, m_ActiveStateList.LookAtFront() ? 0 : Timeval.tv_sec < 0x7fffffff / 1000 - 1 ? Timeval.tv_sec * 1000 + Timeval.tv_usec / 1000 : 0x7fffffff);
            fSet = 1; //!!!!
            //Info.Format("epoll_wait(): %d", Ret);
        }
        for (i = 0; i < Ret; i++)
        {
            int sk = m_e2v[i].data.fd;
            int e = m_e2v[i].events;

            CMaaSocket *p = nullptr;
            m_SockMap.Find(sk, &p);
            int Mode = 0;
            //Info.Append(" sk=%d:", sk);
            //if (p && !m_SockHash.Find(p, &Mode))
            if (/*p &&*/ !m_SockHash.Find(p))
            {
                Mode = p->m_Mode;
                int x = p->m_RealState;
                if  (e & EPOLLIN)
                {
                    x |= eRead;
                    Mode |= eEventRead;
                    //Info.Append("R");
                }
                if  (e & EPOLLOUT)
                {
                    x |= eWrite;
                    Mode |= eEventWrite;
                    //Info.Append("W");
                }
                if  (e & (EPOLLRDHUP | EPOLLHUP))
                {
                    CMaaTcpSocket* t;
                    if  (p->m_TcpSocketFlag && !(t = (CMaaTcpSocket*)p)->m_CloseTimer.IsStarted())// && p->m_CloseTimer.GetPeriod() > 2000)
                    {
                        t->m_CloseTimer.OnClose(t, false);
                    }
                    //Info.Append("H");
                }
                if  (e & EPOLLERR)
                //if (e & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
                {
                    x |= eExept;
                    Mode |= eEventExept;
                    //Info.Append("E");
                }
                p->m_RealState = x;
                x &= Mode & eAll;
                if  (x)
                {
                    //m_hActiveState.AddOver(sk, x);
                    p->m_ActiveState = x;
                    if (!p->m_ActiveStateListItem.Next)
                    {
                        m_ActiveStateList.AddAtBack(&p->m_ActiveStateListItem);
                    }
                }
                //static CMaaStringG ggg;
                //static time_t ttt = 0;
                //if (ggg != Info || ttt != time(nullptr))
                //{
                //    ggg = Info;
                //    ttt = time(nullptr);
                //    __utf8_printf("%S\n", &Info);
                //}
            }
            if  (p)
            {
                //m_SockHash.AddOver(p, Mode);
                p->m_Mode = Mode;
            }
        }
        if  (Ret >= 0)
        {
            //Ret = m_hActiveState.GetItemCount();
            Ret = m_ActiveStateList.LookAtFront() ? 1 : 0;
        }

        if  ( Ret < 0 && !fSet )
        {
#ifndef __unix__
            int s = m_Timeval.tv_sec * 1000 + m_Timeval.tv_usec / 1000; s = s ? s : 1;
            Sleep ( s );
#else
            struct timespec req, rem;
            req.tv_sec  = m_Timeval.tv_sec;
            req.tv_nsec = m_Timeval.tv_usec * 1000;
            nanosleep ( &req, &rem );
#endif
            Ret = Ret == -2/*case EPOLL*/ ? -2 : 0;
        }
        if  ( fStdin )
        {
#ifndef __unix__
            fStdin = kbhit ();
            if  ( Ret >= 0 && fStdin )
            {
                Ret++;
                FD_SET( _fileno ( stdin ), & Read );
            }
#endif
        }
        if  ( Ret > 0 || ev )
        {
            int Errno = errno;

#ifdef __SHAPERS
            //printf("Wakeup Bytes = %d\n", m_pThread->m_WakeupPair->m_Bytes); fflush(stdout);
            bool bWakedUp = false;
            if  (m_pThread->m_WakeupPair)
            {
                if  (m_pThread->m_WakeupPair->m_Bytes.load() > 0)
                {
                    bWakedUp = (m_pThread->m_WakeUpFlags.fetch_and(~ 0x01) & 0x01);
                }
            }
            //bool bWakedUp = (m_pThread->m_WakeUpFlags & 1);
#endif

            //Ret = m_hActiveState.GetItemCount();
            k = 0;
            //CMaaPtr<CMaaSocket *> ptrs(k);
            {
                //CMaaUnivHash <int, int>::iterator it(m_hActiveState);
                CMaaDLink_pItem<CMaaSocket> * pp = m_ActiveStateList.LookAtFront();
                //for (i = 0; it; i++, ++it)
                for (; pp; pp = m_ActiveStateList.Next(pp))
                {
                    //int sk = it.key();
                    CMaaSocket *p = pp->m_pItem;
                    //if  (!m_SockMap.Find(sk, &p))
                    if  (!m_SockHash.Find(p))
                    {
                        //ptrs[k++] = p;
                        Sockets[k++] = p;
                        //int r = p->m_RealState;
                        int a = p->m_ActiveState; //it.data();
                        //{ DP(printf("+ %d %p r/a = %x/%x, Mode = %x\n", sk, p, r, a, p->GetFdMode());) }
                        int x = p->GetFdMode();
                        x = x & ~(eEventRead|eEventWrite);

                        x = x & ~(eShNoRead|eShNoWrite);

                        x |= (a & eAll) << eEventShift;
                        ChangeModeRaw(p, x);

                        //p->ChangeFdMode(x);
                        //}
                        //else
                        //{
                        //    //DP(printf("!!! m_SockMap.Find(%d) - not found\n", sk);)
                        //}
                        // shaper21
#ifdef __SHAPERS
                        //rev[i] = wev[i] = false;
                        if  (p/*Sockets[i]*/->m_TcpSocketFlag)
                        {
                            CMaaTcpSocket* t = (CMaaTcpSocket*)(p/*Sockets[i]*/);
                            int wr = -1, rr = -1;
                            if  (!(x & eConnect))
                            {
                                //CMaaWin32Locker_Locker_gLock;
                                //Locker_Lock;
                                gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
                                if  (t->m_SndLLShaper.m_Shaper)
                                {
                                    wr = t->m_SndLLShaper.GetLenReserved(1000000);
                                }
                                if  (t->m_RcvLLShaper.m_Shaper)
                                {
                                    rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                                }
                                gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
                                //Locker_UnLock;
                            }
                            if  (!rr)
                            {
                                x |= eShNoRead;
                            }
                            if  (!wr)
                            {
                                x |= eShNoWrite;
                            }
                            if  (!rr || !wr)
                            {
                                ChangeModeRaw(p, x);
                            }
                        }
#endif // __SHAPERS
                        // shaper21
                    }
                    else
                    {
                        //DP(printf("!!! m_SockMap.Find(%d) - not found\n", sk);)
                    }
                }
                Ret = k;
            }

            for ( int i = 0; i < k; i++ )
            {
#if 1
                // cycle expand
                CMaaSocket* pSock = Sockets[i];
                try
                {
                    while (1)
                    {
                        // 0
                        //if (m_SockHash.Find(Sockets[i], &Mode))
                        if (m_SockHash.Find(pSock))
                        {
                            break;
                        }
                        int Mode = pSock->m_Mode;
                        try
                        {
                            //unsigned int Act = 0;
                            //CMaaSocket * pSock = Sockets [ i ];
                            SOCKET s = pSock->GetSocket();

                            if (Mode & eEventExept)
                            {
                                int error = Errno;
                                socklen_t errlen = sizeof(error);
                                if (getsockopt(s, SOL_SOCKET, SO_ERROR, (void*)&error, &errlen) == 0)
                                {
                                    errno = error;
                                }
                                else
                                {
                                    errno = Errno;
                                }
                                pSock->ChangeFdMode(pSock->Notify_Error());
                            }
#ifdef __FORCED_CLOSE_NOTIFICATION
                            else if ((Mode & eEventRead) && (Mode & (eRead | eConnect | eExcept)) == eExcept)
                            {
                                if (pSock->m_TcpSocketFlag)
                                {
                                    char c = 0;
                                    int l = recv(pSock->GetSocket(), &c, 1, MSG_PEEK);

                                    if (l == 0)
                                    {
                                        int ErrnoBackup = errno;
                                        errno = CMaa_CONN_CLOSED_ERROR;
                                        pSock->ChangeFdMode(pSock->Notify_Error());
                                        errno = ErrnoBackup;
                                    }
                                    else if (l < 0)
                                    {
                                        int ErrnoBackup = errno;
                                        pSock->ChangeFdMode(pSock->Notify_Error());
                                        errno = ErrnoBackup;
                                    }
                                }
                            }
#endif
                            //if  (Act)
                            //{
                            //    ChangeModeEx(pSock, Act);
                            //}
                        }
                        catch (XTOOSockClose&)
                        {
                            //IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                        }
                        catch (XTOOSockErr& SockErr)
                        {
                            //IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                            /*
                            int xx;
                            if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                            {
                                delete Sockets[i];
                            }
                            */
                        }

                        // 1
                        //if (m_SockHash.Find(Sockets[i], &Mode))
                        if (m_SockHash.Find(pSock))
                        {
                            break;
                        }
                        Mode = pSock->m_Mode;
                        try
                        {
                            //unsigned int Act = 0;
                            //CMaaSocket * pSock = Sockets [ i ];
                            SOCKET s = pSock->GetSocket();
                            if ((Mode & eEventRead) && (Mode & (eRead | eConnect)))
                            {
                                if (Mode & eConnect)
                                {
                                    //bConnect = true;
                                    try
                                    {
                                        pSock->ChangeFdModeEx(((CMaaTcpSocket*)pSock)->ConnectAgain(1));
                                    }
                                    catch (XTOOSockErr& SockErr)
                                    {
                                        //int xx;
                                        //if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                        if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock))
                                        {
                                            pSock->ChangeFdMode(pSock->Notify_Error());
                                        }
                                        else
                                        {
                                            throw;
                                        }
                                    }
                                }
                                else
                                    //if   ( FD_ISSET( s, &Read ) )
                                {
                                    // shaper21
#ifdef __SHAPERS
                                    if ((Mode & eShNoRead))
                                    {
                                        pSock->ChangeFdMode(eDisableRead);
                                        //Act = pSock -> Notify_Read ();
                                        //Act = (Act &~ eRead) | eDisableRead;
                                        int m = 0;
                                        m_SockShRWHash.Find(pSock, &m);
                                        m |= eRead;
                                        m_SockShRWHash.AddOver(pSock, m);
                                    }
                                    else
#endif
                                    {
                                        pSock->ChangeFdMode(pSock->Notify_Read());
                                        //Act = Act & ~(Mode & (eRead|eWrite));
                                    }
                                    // shaper21

                                    if (pSock->m_TcpSocketFlag)
                                    {
                                        CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                        if (p->m_CloseTimer.IsStarted() && p->m_CloseTimer.GetPeriod() > 2000)
                                        {
                                            p->m_CloseTimer.Start1(2000);
                                        }
                                    }
                                    // bReadIsCalled = true;
                                }
                            }

                            //if  (Act)
                            //{
                            //    ChangeModeEx(pSock, Act);
                            //}
                        }
                        catch (XTOOSockClose&)
                        {
                            //printf ( "catch ( XTOOSockClose & )\n" );
                        }
                        catch (XTOOSockErr& SockErr)
                        {
                            //printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p, %s\n", SockErr.GetSock(), (const char *)SockErr.GetMsg() );
                            /*
                            int xx;
                            if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                            {
                                delete Sockets[i];
                            }
                            */
                        }

                        // 2
                        //if (m_SockHash.Find(Sockets[i], &Mode))
                        if (m_SockHash.Find(pSock))
                        {
                            break;
                        }
                        Mode = pSock->m_Mode;
                        try
                        {
                            //unsigned int Act = 0;
                            //CMaaSocket * pSock = Sockets [ i ];
                            SOCKET s = pSock->GetSocket();

                            if ((Mode & eEventWrite) && (Mode & (eWrite | eConnect)))
                                //if   ( FD_ISSET( s, &Write ) )
                            {
                                if (Mode & eConnect)
                                {
                                    try
                                    {
                                        pSock->ChangeFdModeEx(((CMaaTcpSocket*)pSock)->ConnectAgain(1));
                                    }
                                    catch (XTOOSockErr& SockErr)
                                    {
                                        //int xx;
                                        //if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                        if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock))
                                        {
                                            pSock->ChangeFdMode(pSock->Notify_Error());
                                        }
                                        else
                                        {
                                            throw;
                                        }
                                    }
                                }
                                else
                                {
                                    //CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                    //_qword x = p ? p->GetDataSentSize() : 0;
                                    // shaper21
#ifdef __SHAPERS
                                    if (Mode & eShNoWrite)
                                    {
                                        pSock->ChangeFdMode(eDisableWrite);
                                        //Act = pSock -> Notify_Write ();
                                        //Act = (Act &~ eWrite) | eDisableWrite;
                                        int m = 0;
                                        m_SockShRWHash.Find(pSock, &m);
                                        m |= eWrite;
                                        m_SockShRWHash.AddOver(pSock, m);
                                    }
                                    else
#endif
                                    {
                                        pSock->ChangeFdMode(pSock->Notify_Write());
                                        //Act = Act & ~(Mode & (eRead|eWrite));
                                    }
                                    // shaper21
                                    //if   ((Act & eWrite) && p && p->GetDataSentSize() != x)
                                    //{
                                    //     WakeUp();
                                    //}
                                }
                            }

                            //if  (Act)
                            //{
                            //    ChangeModeEx(pSock, Act);
                            //}
                        }
                        catch (XTOOSockClose&)
                        {
                            //IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                        }
                        catch (XTOOSockErr& SockErr)
                        {
                            //IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                            /*
                            int xx;
                            if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                            {
                                delete Sockets[i];
                            }
                            */
                        }
                        break;
                    } // while(1)
                }
                catch (...)
                {
                    for (i = 0; i < k; i++)
                    {
                        CMaaSocket* pSock = Sockets[i];
                        if  (!m_SockHash.Find(pSock))
                        {
                            pSock->Notify_Act();
                        }
                    }
                    /*
                    if (bWakeUp) // && (Act & eWrite))
                    {
                        WakeUp();
                    }
                    */
                    throw;
                }

#else
                // cycle mode
                CMaaSocket* pSock = Sockets[i];
                try
                {
                    for (int j = 0; j < 3; j++)
                    {
                        //int Mode;
                        //if  ( !m_SockHash.Find ( Sockets [ i ], &Mode ) )
                        if (!m_SockHash.Find(pSock))
                        {
                            int Mode = pSock->m_Mode;
                            //bool bConnect = false;
                            try
                            {
                                //unsigned int Act = 0;
                                //CMaaSocket * pSock = Sockets [ i ];
                                SOCKET s = pSock->GetSocket();

                                switch (j)
                                {
                                case 1:
                                    if ((Mode & eEventRead) && (Mode & (eRead | eConnect)))
                                    {
                                        if (Mode & eConnect)
                                        {
                                            //bConnect = true;
                                            try
                                            {
                                                pSock->ChangeFdModeEx(((CMaaTcpSocket*)pSock)->ConnectAgain(1));
                                            }
                                            catch (XTOOSockErr& SockErr)
                                            {
                                                //int xx;
                                                //if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                                if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock))
                                                {
                                                    pSock->ChangeFdMode(pSock->Notify_Error());
                                                }
                                                else
                                                {
                                                    throw;
                                                }
                                            }
                                        }
                                        else
                                            //if   ( FD_ISSET( s, &Read ) )
                                        {
                                            pSock->ChangeFdMode(pSock->Notify_Read());

                                            if (pSock->m_TcpSocketFlag)
                                            {
                                                CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                                if (p->m_CloseTimer.IsStarted() && p->m_CloseTimer.GetPeriod() > 2000)
                                                {
                                                    p->m_CloseTimer.Start1(2000);
                                                }
                                            }
                                            // bReadIsCalled = true;
                                        }
                                    }
                                    break;
                                case 2:
                                    if ((Mode & eEventWrite) && (Mode & (eWrite | eConnect)))
                                        //if   ( FD_ISSET( s, &Write ) )
                                    {
                                        if (Mode & eConnect)
                                        {
                                            try
                                            {
                                                pSock->ChangeFdModeEx(((CMaaTcpSocket*)pSock)->ConnectAgain(1));
                                            }
                                            catch (XTOOSockErr& SockErr)
                                            {
                                                //int xx;
                                                //if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                                if (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock))
                                                {
                                                    pSock->ChangeFdMode(pSock->Notify_Error());
                                                }
                                                else
                                                {
                                                    throw;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            //CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                            //_qword x = p ? p->GetDataSentSize() : 0;
                                            pSock->ChangeFdMode(pSock->Notify_Write());
                                            //if   ((Act & eWrite) && p && p->GetDataSentSize() != x)
                                            //{
                                            //     WakeUp();
                                            //}
                                        }
                                    }
                                    break;
                                case 0:
                                    if (Mode & eEventExept)
                                    {
                                        errno = Errno;
                                        pSock->ChangeFdMode(pSock->Notify_Error());
                                    }
#ifdef __FORCED_CLOSE_NOTIFICATION
                                    else if ((Mode & eEventRead) && (Mode & (eRead | eConnect | eExcept)) == eExcept)
                                    {
                                        if (pSock->m_TcpSocketFlag)
                                        {
                                            char c = 0;
                                            int l = recv(pSock->GetSocket(), &c, 1, MSG_PEEK);

                                            if (l == 0)
                                            {
                                                int ErrnoBackup = errno;
                                                errno = CMaa_CONN_CLOSED_ERROR;
                                                pSock->ChangeFdMode(pSock->Notify_Error());
                                                errno = ErrnoBackup;
                                            }
                                            else if (l < 0)
                                            {
                                                int ErrnoBackup = errno;
                                                pSock->ChangeFdMode(pSock->Notify_Error());
                                                errno = ErrnoBackup;
                                            }
                                        }
                                    }
#endif
                                    break;
                                }
                                //if  ( Act )
                                //{
                                //    ChangeModeEx( pSock, Act );
                                //}
                            }
                            catch (XTOOSockClose&)
                            {
                                //IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                            }
                            catch (XTOOSockErr& SockErr)
                            {
                                //IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                                /*
                                  int xx;
                                  if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                                  {
                                      delete Sockets[i];
                                  }
                                  */
                            }
                        }
                    }
                }
                catch (...)
                {
                    for (i = 0; i < k; i++)
                    {
                        CMaaSocket* pSock = Sockets[i];
                        if  (!m_SockHash.Find(pSock))
                        {
                            pSock->Notify_Act();
                        }
                    }
                    /*
                    if (bWakeUp) // && (Act & eWrite))
                    {
                        WakeUp();
                    }
                    */
                    throw;
                }
#endif
            }
            for (i = 0; i < k; i++)
            {
                CMaaSocket* pSock = Sockets[i];
                if  (!m_SockHash.Find(pSock))
                {
                    pSock->Notify_Act();
                }
            }
            CMaaDLink_pItem<CMaaFdSocket>* p;
            while ((p = m_ModeChangeList.GetFromFront()))
            {
                CMaaSocket* pSock = (CMaaSocket*)p->m_pItem;
                pSock->Notify_Act();
            }
            /*
            if (bWakeUp) // && (Act & eWrite))
            {
                WakeUp();
            }
            */
#ifdef __SHAPERS
            if  (bWakedUp) //m_pThread->m_WakeupPair && m_pThread->m_WakeupPair->m_Bytes > 0
            {
                // On waked up
                for (auto it(m_SockShRWHash.it()); it; ++it)
                {
                    if  (!m_SockHash.Find(it.key()) && it.data())
                    {
                        //printf("ChangeMode(%p,eRead)\n", it.key()); fflush(stdout);
                        //ChangeMode(it.key(), it.data(), false); // --2025
                        ChangeMode(it.key(), 0, it.data(), true); // 2025
                    }
                    it.Remove();
                }
            }
#endif
        }
    }
    /*
    catch (int)
    {
        //printf("catch(%d), throw;\n", x); fflush(stdout);
        throw;
    }
    catch(...)
    {
        //printf("catch(...), throw;\n"); fflush(stdout);
        throw;
    }
    */
    return Ret;
}

#else // !_WIN32 && !TL_EPOLL // select() version

#error "select() version is broken // 24.09.2021"
// select() version is a bad for net sockets

int CMaaFdSockets::Select ()
{
    int i;
    fd_set Read, Write, Exept;
    struct timeval Timeval;
    int fStdin = 0;

    Timeval = m_Timeval;
    //printf("tv: %d %d\r\n", Timeval.tv_sec, Timeval.tv_usec);
    FD_ZERO ( &Read );
    FD_ZERO ( &Write );
    FD_ZERO ( &Exept );

    int k = m_SockHash.GetMaxIndex ();

    /*
    if  (!(m_SelectSockets.MaxIndex() >= (size_t)k && m_SelectModes.MaxIndex() >= (size_t)k &&
           m_Select_rev.MaxIndex() >= (size_t)(k + 16) &&
           m_Select_wev.MaxIndex() >= (size_t)(k + 16)
        ))
    {
        CMaaPtr<CMaaSocket *> Sockets(k, 1);
        CMaaPtr<int> Modes(k, 1);
        CMaaPtr<bool> rev(k + 16, 1), wev(k + 16, 1);
        m_SelectSockets.Swap(Sockets);
        m_SelectModes.Swap(Modes);
        m_Select_rev.Swap(rev);
        m_Select_wev.Swap(wev);
    }
    */

    CMaaSocket ** Sockets = m_SelectSockets;
    int * Modes = m_SelectModes;
    bool * rev = m_Select_rev;
    bool * wev = m_Select_wev;

    int Ret = -1;

    try
    {
        int fSet = 0;
        bool ev = false;

        k = m_SockHash.EnumerateItems ( k, Sockets, Modes );

        bool brwev = false;
        //int fSet = 0;
        for ( int i = 0; i < k; i++ )
        {
            {
                int Mode;
                if  (Sockets[i] && (Sockets[i]->GetFdMode() & CMaaEnum::eHold))
                {
                    if  (--k == 0)
                    {
                        int us;
                        {
                            us = GetWaitForTime(Timeval.tv_sec * 1000000 + Timeval.tv_usec);
                            Timeval.tv_sec = us / 1000000;
                            Timeval.tv_usec = us % 1000000;
                        }
                        usleep(us);
                        return 0;
                    }
                    for (int j = i; j < k; j++)
                    {
                        Sockets[j] = Sockets[j + 1];
                        Modes[j] = Modes[j + 1];
                    }
                    i--;
                    continue;
                }
            }
            if  ( Sockets [ i ] )
            {
                SOCKET s = Sockets [ i ] -> GetSocket ();
                bool bIsTcpSock = Sockets[i]->m_TcpSocketFlag;

                int Mode = Modes [ i ];
                if  ( IsWinStdioSock ( s ) )
                {
                    if  ( ( Mode & eRead ) )
                    {
                        fStdin++;
                    }
                    continue;
                }
                if  ( Mode & eConnect )
                {
                    Mode = ( Mode | eWrite ) & ~eRead;
                    Mode = Mode|eWrite|eRead;
                }
#ifndef __FORCED_CLOSE_NOTIFICATION
                if  ((Mode & (eRead|eEventRead)) == eRead)
#else
                if  ((Mode & (eRead|eEventRead)) == eRead || (bIsTcpSock && (Mode & (eExcept|eEventRead)) == eExcept))
#endif
                {
                    FD_SET( s, & Read );     fSet++;
                }
                if  ((Mode & (eWrite|eEventWrite)) == eWrite)
                {
                    FD_SET( s, & Write );    fSet++;
                }
                if  ( ( Mode & eExept ) )
                {
                    FD_SET( s, & Exept );    fSet++;
                }

                rev[i] = wev[i] = false;
                if  (Sockets[i]->m_TcpSocketFlag)
                {
                    CMaaTcpSocket* t = (CMaaTcpSocket*)(Sockets[i]);
                    int wr = -1, rr = -1;
#ifdef __SHAPERS
                    if  (!(Mode & eConnect))
                    {
                        //CMaaWin32Locker_Locker_gLock;
                        //Locker_Lock;
                        gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
                        if  (t->m_SndLLShaper.m_Shaper)
                        {
                            wr = t->m_SndLLShaper.GetLenReserved(1000000);
                        }
                        if  (t->m_RcvLLShaper.m_Shaper)
                        {
                            rr = t->m_RcvLLShaper.GetLenReserved(1000000);
                        }
                        gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
                        //Locker_UnLock;
                    }
                    if  (rr == 0 && wr == 0)
                    {
#ifndef __FORCED_CLOSE_NOTIFICATION
                        if  ((Mode & eRead))
#else
                        if  ((Mode & eRead) || (bIsTcpSock && (Mode & eRead|eExcept)))
#endif
                        {
                            FD_CLR(s, &Read);     fSet--;
                        }
                        if  ((Mode & eWrite))
                        {
                            FD_CLR(s, &Write);    fSet--;
                        }
                        // hh[2 + 3 * i] = hEv0;
                        rev[i] = t->m_RcvLLShaper.m_hEv;
                        wev[i] = t->m_SndLLShaper.m_hEv;
                    }
                    else if (rr && wr)
                    {
                        //hh[2 + 3 * i + 1] = hh[2 + 3 * i + 2] = m_hEv0;
                    }
                    else if (rr == 0 && wr)
                    {
                        rev[i] = t->m_RcvLLShaper.m_hEv;
                        if  ((Modes[i] & (eEventWrite|eWrite)) != eWrite)
                        {
                            if  (FD_ISSET( s, & Read ))
                            {
                                FD_CLR( s, & Read );     fSet--;
                            }
                            if  (FD_ISSET( s, & Write ))
                            {
                                FD_CLR( s, & Write );     fSet--;
                            }
                        }
                    }
                    else if (rr && wr == 0)
                    {
                        wev[i] = t->m_SndLLShaper.m_hEv;
#ifndef __FORCED_CLOSE_NOTIFICATION
                        if  ((Modes[i] & (eEventRead|eRead)) != eRead)
#else
                        if  ((Modes[i] & (eEventRead|eRead)) != eRead && (!bIsTcpSock || (Modes[i] & (eEventRead|eExcept)) != eExcept))
#endif
                        {
                            if  (FD_ISSET( s, & Read ))
                            {
                                FD_CLR( s, & Read );     fSet--;
                            }
                            if  (FD_ISSET( s, & Write ))
                            {
                                FD_CLR( s, & Write );     fSet--;
                            }
                        }
                    }
#endif // __SHAPERS
                    if  (rr)
                    {
#ifndef __FORCED_CLOSE_NOTIFICATION
                        if  ((Modes[i] & (eEventRead|eRead)) == (eEventRead|eRead))
#else
                        if  ((Modes[i] & (eEventRead|eRead)) == (eEventRead|eRead) || (bIsTcpSock && (Modes[i] & (eEventRead|eExcept)) == (eEventRead|eExcept)))
#endif
                        {
                            ev = true;
                            if  (FD_ISSET( s, & Read ))
                            {
                                FD_CLR( s, & Read );     fSet--;
                            }
                        }
                    }
                    if  (wr)
                    {
                        if  ((Modes[i] & (eEventWrite|eWrite)) == (eEventWrite|eWrite))
                        {
                            ev = true;
                            if  (FD_ISSET( s, & Write ))
                            {
                                FD_CLR( s, & Write );     fSet--;
                            }
                        }
                    }
                }
                ev = ev || rev[i] || wev[i];
            }
        }

        errno = 0;
        int us;
        {
            us = GetWaitForTime(Timeval.tv_sec * 1000000 + Timeval.tv_usec);
            if  (ev)
            {
                us = 0;
            }
            Timeval.tv_sec = us / 1000000;
            Timeval.tv_usec = us % 1000000;
        }

        Ret = select ( FD_SETSIZE, &Read, &Write, &Exept, &Timeval );

        // 20.11.2008
        if  (Ret > 0)
        {
            for ( int i = 0; i < k; i++ )
            {
                int Mode;
                if  (!m_SockHash.Find(Sockets[i], &Mode))
                {
                    //if   ( Sockets [ i ] )
                    SOCKET s = Sockets [ i ] -> GetSocket ();
                    /*
                    if   ( IsWinStdioSock ( s ) )
                    {
                         continue;
                    }
                    */
                    if  (FD_ISSET(s, &Read))
                    {
                        Mode |= eEventRead;
                    }
                    if  (FD_ISSET(s, &Write))
                    {
                        Mode |= eEventWrite;
                    }
                    /*
                    if   (FD_ISSET(s, &Exept))
                    {
                         strcat ( txt, "E" );
                    }
                    */
                    m_SockHash.AddOver(Sockets[i], Mode);
                }
            }
        }

        if  ( Ret < 0 && !fSet )
        {
            FD_ZERO ( &Read );
            FD_ZERO ( &Write );
            FD_ZERO ( &Exept );
        }

        if  ( Ret < 0 && !fSet )
        {
#ifndef __unix__
            int s = m_Timeval.tv_sec * 1000 + m_Timeval.tv_usec / 1000; s = s ? s : 1;
            Sleep ( s );
#else
            struct timespec req, rem;
            req.tv_sec  = m_Timeval.tv_sec;
            req.tv_nsec = m_Timeval.tv_usec * 1000;
            nanosleep ( &req, &rem );
#endif
            Ret = Ret == -2/*case EPOLL*/ ? -2 : 0;
        }
        if  ( fStdin )
        {
#ifndef __unix__
            fStdin = kbhit ();
            if  ( Ret >= 0 && fStdin )
            {
                Ret++;
                FD_SET( _fileno ( stdin ), & Read );
            }
#endif
        }
        if  ( Ret > 0 || ev )
        {
            int Errno = errno;

            for ( int i = 0; i < k; i++ )
            {
#if 1
                // cycle expand
                while(1)
                {
                    // 0
                    int Mode;
                    if  (m_SockHash.Find(Sockets[i], &Mode))
                    {
                        break;
                    }
                    try
                    {
                        unsigned int Act = 0;
                        CMaaSocket * pSock = Sockets [ i ];
                        SOCKET s = pSock -> GetSocket ();

                        if  (FD_ISSET(s, &Exept))
                        {
                            errno = Errno;
                            Act = pSock -> Notify_Error ();
                        }
#ifdef __FORCED_CLOSE_NOTIFICATION
                        else if ((Mode & eEventRead) && (Mode & (eRead|eConnect|eExcept)) == eExcept)
                        {
                            if  (pSock->m_TcpSocketFlag)
                            {
                                char c = 0;
                                int l = recv(pSock->GetSocket(), &c, 1, MSG_PEEK);

                                if  (l == 0)
                                {
                                    int ErrnoBackup = errno;
                                    errno = CMaa_CONN_CLOSED_ERROR;
                                    Act = pSock -> Notify_Error ();
                                    errno = ErrnoBackup;
                                }
                                else if (l < 0)
                                {
                                    int ErrnoBackup = errno;
                                    Act = pSock -> Notify_Error ();
                                    errno = ErrnoBackup;
                                }
                            }
                        }
#endif
                        if  (Act)
                        {
                            ChangeMode(pSock, Act);
                        }
                    }
                    catch(XTOOSockClose&)
                    {
                        //IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                    }
                    catch( XTOOSockErr & SockErr )
                    {
                        //IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                        /*
                        int xx;
                        if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                        {
                            delete Sockets[i];
                        }
                        */
                    }

                    // 1
                    if  (m_SockHash.Find(Sockets[i], &Mode))
                    {
                        break;
                    }
                    try
                    {
                        unsigned int Act = 0;
                        CMaaSocket * pSock = Sockets [ i ];
                        SOCKET s = pSock -> GetSocket ();
                        if  ((Mode & eEventRead) && (Mode & (eRead|eConnect)))
                        {
                            if  (Mode & eConnect)
                            {
                                //bConnect = true;
                                try
                                {
                                    Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                }
                                catch(XTOOSockErr & SockErr)
                                {
                                    int xx;
                                    if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                    {
                                        Act = pSock->Notify_Error();
                                    }
                                    else
                                    {
                                        throw;
                                    }
                                }
                            }
                            else
                            //if   ( FD_ISSET( s, &Read ) )
                            {
                                // shaper21
#ifdef __SHAPERS
#if 0
                                if  ((Mode & eShNoRead))
                                {
                                    Act = eDisableRead;
                                    //Act = pSock -> Notify_Read ();
                                    //Act = (Act &~ eRead) | eDisableRead;
                                    int m = 0;
                                    m_SockShRWHash.Find(pSock, &m);
                                    m |= eRead;
                                    m_SockShRWHash.AddOver(pSock, m);
                                }
                                else
#endif
#endif
                                Act = pSock -> Notify_Read ();
                                // shaper21

                                if (pSock->m_TcpSocketFlag)
                                {
                                    CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                    if (p->m_CloseTimer.IsStarted() && p->m_CloseTimer.GetPeriod() > 2000)
                                    {
                                        p->m_CloseTimer.Start1(2000);
                                    }
                                }
                                // bReadIsCalled = true;
                            }
                        }

                        if  (Act)
                        {
                            ChangeMode(pSock, Act);
                        }
                    }
                    catch(XTOOSockClose&)
                    {
                        //printf ( "catch ( XTOOSockClose & )\n" );
                    }
                    catch( XTOOSockErr & SockErr )
                    {
                        //printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p, %s\n", SockErr.GetSock(), (const char *)SockErr.GetMsg() );
                        /*
                        int xx;
                        if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                        {
                            delete Sockets[i];
                        }
                        */
                    }

                    // 2
                    if  (m_SockHash.Find(Sockets[i], &Mode))
                    {
                        break;
                    }
                    try
                    {
                        unsigned int Act = 0;
                        CMaaSocket * pSock = Sockets [ i ];
                        SOCKET s = pSock -> GetSocket ();

                        if  ((Mode & eEventWrite) && (Mode & (eWrite|eConnect)))
                        //if   ( FD_ISSET( s, &Write ) )
                        {
                            if  ( Mode & eConnect )
                            {
                                try
                                {
                                    Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                }
                                catch(XTOOSockErr & SockErr)
                                {
                                    int xx;
                                    if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                    {
                                        Act = pSock->Notify_Error();
                                    }
                                    else
                                    {
                                        throw;
                                    }
                                }
                            }
                            else
                            {
                                //CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                //_qword x = p ? p->GetDataSentSize() : 0;
                                // shaper21
#ifdef __SHAPERS
#if 0
                                if  (Mode & eShNoWrite)
                                {
                                    Act = eDisableWrite;
                                    //Act = pSock -> Notify_Write ();
                                    //Act = (Act &~ eWrite) | eDisableWrite;
                                    int m = 0;
                                    m_SockShRWHash.Find(pSock, &m);
                                    m |= eWrite;
                                    m_SockShRWHash.AddOver(pSock, m);
                                }
                                else
#endif
#endif
                                Act = pSock->Notify_Write();
                                // shaper21
                                //if   ((Act & eWrite) && p && p->GetDataSentSize() != x)
                                //{
                                //     WakeUp();
                                //}
                            }
                        }

                        if  (Act)
                        {
                            ChangeMode(pSock, Act);
                        }
                    }
                    catch(XTOOSockClose&)
                    {
                        //IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                    }
                    catch( XTOOSockErr & SockErr )
                    {
                        //IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                        /*
                        int xx;
                        if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                        {
                            delete Sockets[i];
                        }
                        */
                    }
                    break;
                } // while(1)

#else
                // cycle mode
                for ( int j = 0; j < 3; j++ )
                {
                    int Mode;
                    if  ( !m_SockHash.Find ( Sockets [ i ], &Mode ) )
                    {
                        //bool bConnect = false;
                        try
                        {
                            unsigned int Act = 0;
                            CMaaSocket * pSock = Sockets [ i ];
                            SOCKET s = pSock -> GetSocket ();

                            switch( j )
                            {
                            case 1:
                                if  ((Mode & eEventRead) && (Mode & (eRead|eConnect)))
                                {
                                    if  (Mode & eConnect)
                                    {
                                        //bConnect = true;
                                        try
                                        {
                                            Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                        }
                                        catch(XTOOSockErr & SockErr)
                                        {
                                            int xx;
                                            if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                            {
                                                Act = pSock->Notify_Error();
                                            }
                                            else
                                            {
                                                throw;
                                            }
                                        }
                                    }
                                    else
                                    //if   ( FD_ISSET( s, &Read ) )
                                    {
                                        Act = pSock -> Notify_Read ();

                                        if (pSock->m_TcpSocketFlag)
                                        {
                                            CMaaTcpSocket* p = (CMaaTcpSocket*)pSock;
                                            if (p->m_CloseTimer.IsStarted() && p->m_CloseTimer.GetPeriod() > 2000)
                                            {
                                                p->m_CloseTimer.Start1(2000);
                                            }
                                        }
                                        // bReadIsCalled = true;
                                    }
                                }
                                break;
                            case 2:
                                if  ((Mode & eEventWrite) && (Mode & (eWrite|eConnect)))
                                //if   ( FD_ISSET( s, &Write ) )
                                {
                                    if  ( Mode & eConnect )
                                    {
                                        try
                                        {
                                            Act = ( ( CMaaTcpSocket * ) pSock ) -> ConnectAgain ( 1 );
                                        }
                                        catch(XTOOSockErr & SockErr)
                                        {
                                            int xx;
                                            if  (SockErr.GetSock() == nullptr && !m_SockHash.Find(pSock, &xx))
                                            {
                                                Act = pSock->Notify_Error();
                                            }
                                            else
                                            {
                                                throw;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        //CMaaTcpSocket * p = dynamic_cast<CMaaTcpSocket *>(pSock);
                                        //_qword x = p ? p->GetDataSentSize() : 0;
                                        Act = pSock->Notify_Write();
                                        //if   ((Act & eWrite) && p && p->GetDataSentSize() != x)
                                        //{
                                        //     WakeUp();
                                        //}
                                    }
                                }
                                break;
                            case 0:
                                if  ( FD_ISSET( s, &Exept ) )
                                {
                                    errno = Errno;
                                    Act = pSock -> Notify_Error ();
                                }
#ifdef __FORCED_CLOSE_NOTIFICATION
                                else if ((Mode & eEventRead) && (Mode & (eRead|eConnect|eExcept)) == eExcept)
                                {
                                    if  (pSock->m_TcpSocketFlag)
                                    {
                                        char c = 0;
                                        int l = recv(pSock->GetSocket(), &c, 1, MSG_PEEK);

                                        if  (l == 0)
                                        {
                                            int ErrnoBackup = errno;
                                            errno = CMaa_CONN_CLOSED_ERROR;
                                            Act = pSock -> Notify_Error ();
                                            errno = ErrnoBackup;
                                        }
                                        else if (l < 0)
                                        {
                                            int ErrnoBackup = errno;
                                            Act = pSock -> Notify_Error ();
                                            errno = ErrnoBackup;
                                        }
                                    }
                                }
#endif
                                break;
                            }
                            if  ( Act )
                            {
                                ChangeMode ( pSock, Act );
                            }
                        }
                        catch( XTOOSockClose & )
                        {
                            //IfDebug(2) printf ( "catch ( XTOOSockClose & )\n" );
                        }
                        catch( XTOOSockErr & SockErr )
                        {
                            //IfDebug(2) printf ( "catch ( XTOOSockErr & SockErr ). SockErr.GetSock () == %p\n", SockErr.GetSock () );
                            /*
                              int xx;
                              if (bConnect && SockErr.GetSock() == nullptr && !m_SockHash.Find(Sockets[i], &xx))
                              {
                                  delete Sockets[i];
                              }
                              */
                        }
                    }
                }
#endif
            }
#if defined(TL_EPOLL) && defined(__SHAPERS)
            //if (bWakedUp)
#endif
        }
    }
    catch(int)
    {
        //printf("catch(%d), throw;\n", x); fflush(stdout);
        throw;
    }
    catch(...)
    {
        //printf("catch(...), throw;\n"); fflush(stdout);
        throw;
    }
    return Ret;
}

#endif // !_WIN32 && !TL_EPOLL
#endif // !_WIN32
//---------------------------------------------------------------------------
/*
int CMaaFdSockets::GetMode(CMaaSocket* pSock)
{
    return m_SockHash[pSock, 0]; // Mode
}
*/
//---------------------------------------------------------------------------
void CMaaFdSockets::ChangeModeRaw(CMaaSocket * pSock, int Mode) noexcept
{
    /*
    if (!m_SockHash.Find(pSock) && pSock)
    {
        m_SockHash.AddOver(pSock, Mode);
    }
    */
    if  (pSock)
    {
        pSock->m_Mode = Mode;
        /*
        int* pMode = m_SockHash[pSock];
        if  (pMode)
        {
            *pMode = Mode;
        }
        */
    }
}
//---------------------------------------------------------------------------
/*
bool CMaaFdSockets::ChangeModeEx(CMaaSocket* pSock, int Act1, int Act2) noexcept
{
    return ChangeMode(pSock, Act1, Act2, false);
}
*/
//---------------------------------------------------------------------------
bool CMaaFdSockets::ChangeMode(CMaaSocket* pSock, int Act1, int Act2, bool bForced) noexcept
{
    //__utf8_printf("CMaaFdSockets::ChangeMode(%p, 0x%x)\n", pSock, Act);
    //int* pMode;
    //if (Act && pSock && (pMode = m_SockHash[pSock]))
    /*
    if (!bForced && !(Act1 | Act2))
    {
        return true;
    }
    */
    if (!m_SockHash.Find(pSock))
    {
        if (pSock->m_ModeChangeItem.Next)
        {
            m_ModeChangeList.Release(&pSock->m_ModeChangeItem);
        }
        //const int PrevMode = *pMode;
        const int PrevMode = pSock->m_Mode;
        const int Mode = ActMode(ActMode(PrevMode, Act1), Act2);
        if (!bForced && Mode == PrevMode)
        {
            return true;
        }
#ifdef DEBUG_SET_TEST_MESSAGE
        pSock->SetTestMessage(CMaaString::sFormat("Changing mode, Act1=0x%x, Act2=0x%x\n", Act1, Act2), Act1 == 0x100 ? 0x100 : -0x100);
#endif
#if 0
        if  (g_____socket == pSock->GetSocket() && (Mode & (eRead|eConnect)) == eRead)
        {
            static int aa = 0;
            aa++;
            aa++;
            aa++;
#ifdef DEBUG_SET_TEST_MESSAGE
            pSock->SetTestMessage(CMaaString::sFormat("Changing mode, Act=0x%x\n", Act), 0x12345678);
#endif
        }
#endif
        //m_SockHash.AddOver ( pSock, Mode );
        //*pMode = Mode;
        pSock->m_Mode = Mode;
        const SOCKET s = pSock -> GetSocket ();
        if  ( IsWinStdioSock ( s ) )
        {
            return true;
        }
#ifdef _WIN32
        //int a = (((Act & (eRead|eWrite|eConnect)) & ~((Act & (eDisableRead|eDisableWrite|eDisableConnect)) >> eDisableShift)));
        const int a = Mode;

#ifdef DEBUG_SET_TEST_MESSAGE
        pSock->SetTestMessage(CMaaString::sFormat("ChangeMode(from 0x%04x to 0x%04x, Act=0x%x)\n", PrevMode, Mode, Act));
#endif
        long m = 0;
        if  ((a & (eRead|eExept)) && (a & (eRead|eEventRead)) != eEventRead)
        {
            if  (pSock->m_AcceptFlag && (a & eRead))
            {
                m |= FD_ACCEPT;
            }
            else if (!pSock->m_AcceptFlag)
            {
                m |= FD_READ;
                if  (pSock->m_TcpSocketFlag)
                {
                    m |= FD_CLOSE;
                }
            }
        }
        if  (a & eWrite)
        {
            m |= FD_WRITE;
        }
        //if (!pSock->m_AcceptFlag)
        //{
        //m |= FD_WRITE; //2022
        //}
        if  (a & eConnect)
        {
            m |= FD_CONNECT;
            m &= ~(FD_READ|FD_WRITE|FD_CLOSE);
        }
        if  (Mode & eHold)
        {
            m = 0;
        }

        if  (pSock->m_LastMode == -1 || pSock->m_LastMode != a)
        {
            if  (pSock->m_LastMode == -1)
            {
                pSock->m_pm = -1;
                m_bForceMode = true;
            }
            if  ((pSock->m_LastMode ^ a) & eSensitiveEvents)
            {
                m_bForceMode = true;
            }
            pSock->m_LastMode = a;
        }

        if  (m != pSock->m_pm)
        {
            //int lastm = pSock->m_pm != -1 ? pSock->m_pm : 0;
            pSock->m_pm = m;

            //CMaaFile f("c:\\temp\\ev.txt", CMaaFile::eAC_SrSw, eNoExcept);
            //f.fprintf("%p+%02x\n", (void*)pSock->GetSocket(), (int)m);

            //__utf8_printf("WSAEventSelect(%D, pSock->m_Ev, 0x%x)\n", (_qword)s, m);
            if  (WSAEventSelect(s, pSock->m_Ev, m))
            {
                //const DWORD dw = WSAGetLastError();
                //__utf8_printf("WSAEventSelect() error %d\n", (int)dw);
            }
            /*
            if (!(m & FD_ACCEPT))
            {
                if (m & FD_WRITE)
                {
                    pSock->m_RealState |= eWrite;
                }
                if (m & FD_READ)
                {
                    pSock->m_RealState |= eRead;
                }
            }
            */

            if  ((m & (FD_READ|FD_WRITE|FD_CLOSE))) //10.01.2008
            //if (((m & (FD_READ | FD_WRITE | FD_CLOSE)) & ~ lastm)) // 29.04.2024
            {
#if 0
                if  (pSock->m_bDebug)
                {
#ifdef DEBUG_SET_TEST_MESSAGE
                    pSock->SetTestMessage(CMaaString::sFormat("WSAEventSelect(, %x); WSASetEvent()\n", m));
#endif
                }
#endif
                WakeUp();
            }
#if 0
            {
                int x = pSock->m_RealState;
                {
                    x &= Mode & eAll;
                    //x |= (Mode & eWrite);
                    /*
                    if ((Act & eDisableEventRead) && s == fileno(stdin))
                    {
                        x &= ~eRead;
                    }
                    */
                    //__utf8_printf("m_hActiveState[%D]=0x%x\n", (_qword)s, x);
                    if  (!x)
                    {
                        m_hActiveState.Remove(s);
                    }
                    else
                    {
                        m_hActiveState.AddOver(s, x);
                    }
                }
            }
#endif
            return true;
        }
#else
        /*
          if   ( ( Act & eRead ) )
          {
               FD_SET( s, & m_Read );
          }
          if   ( ( Act & eWrite ) )
          {
               FD_SET( s, & m_Write );
          }
          if   ( ( Act & eExept ) )
          {
               FD_SET( s, & m_Exept );
          }
          Act = Act >> eDisableShift;
          if   ( ( Act & eRead ) )
          {
               FD_CLR( s, & m_Read );
          }
          if   ( ( Act & eWrite ) )
          {
               FD_CLR( s, & m_Write );
          }
          if   ( ( Act & eExept ) )
          {
               FD_CLR( s, & m_Exept );
          }
        */
#ifdef TL_EPOLL
        epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.data.fd = s;
#ifdef TL_EPOLLET
        ev.events = !(Mode & eNoLET) && !pSock->m_AcceptFlag ? (int)EPOLLET : 0; // CONTINUE DEV
#else
        ev.events = 0;
#endif
        bool bUdp = false, bMod = false; // bUdpMod, bTcpMod - 2022
        if  (dynamic_cast<CMaaUdpServer *>(pSock))
        {
            //ev.events &= ~ EPOLLET; // 2022
            if  (Mode & eRead)
            {
                ev.events |= EPOLLIN;
            }
            if  (Mode & eWrite)
            {
                ev.events |= EPOLLOUT; // !!!2020!!!
            }
            if  (Mode & eExept)
            {
                ev.events |= EPOLLERR; // ?? is always set by epoll
            }
            bMod = bUdp = true;
        }
        else if (dynamic_cast<CMaaUdpSocket *>(pSock))
        {
            //ev.events &= ~ EPOLLET; // 2022
            if  (Mode & eRead)
            {
                //ev.events |= EPOLLIN;
            }
            if  (Mode & eWrite)
            {
                ev.events |= EPOLLOUT; // !!!2020!!!
            }
            if  (Mode & eExept)
            {
                ev.events |= EPOLLERR; // ?? is always set by epoll
            }
            bMod = true;
        }
        else
        {
            if  (dynamic_cast<CMaaTcpSocket *>(pSock))
            {
#ifndef __SHAPERS
                bMod = true;
#endif
            }
            //if (Mode & eRead)
#if defined(TL_EPOLL) && defined(__SHAPERS)
            /*
              int shm = 0;
              if (!m_SockShRWHash.Find(pSock, &shm) && (shm & eRead))//eShNoRead))
              {
                    ev.events &= ~ EPOLLIN;
              }
              else
            */
#endif
            {
                //ev.events |= EPOLLIN;
                ev.events |= EPOLLIN | EPOLLRDHUP | EPOLLHUP;
            }
            //if (Mode & eWrite)
#if defined(TL_EPOLL) && defined(__SHAPERS)
            /*
              if (shm & eWrite)
              {
                   ev.events &= ~EPOLLOUT;
              }
              else
              */
#endif
            if  (!pSock->m_AcceptFlag)
            {
                ev.events |= EPOLLOUT;
            }
            //if (Mode & eExept)
            {
                ev.events |= EPOLLERR; // is always set by epoll
            }
        }
#ifdef TL_UDP_EPOLLET
        if  (bUdp)
        {
            int x = pSock->m_RealState;
            {
                x &= Mode & eAll;
                if  (((Act1 | Act2) & eDisableEventRead))
                {
                    x &= ~ eRead;
                }
                if  (((Act1 | Act2) & eDisableEventWrite))
                {
                    x &= ~ eWrite;
                }
                if  (!x)
                {
                    //m_hActiveState.Remove(s);
                    if (pSock->m_ActiveStateListItem.Next)
                    {
                        m_ActiveStateList.Release(&pSock->m_ActiveStateListItem);
                    }
                }
                else
                {
                    //m_hActiveState.AddOver(s, x);
                    pSock->m_ActiveState = x;
                    if (!pSock->m_ActiveStateListItem.Next)
                    {
                        m_ActiveStateList.AddAtBack(&pSock->m_ActiveStateListItem);
                    }
                }
            }
        }
        else
#endif
        {
            int x = pSock->m_RealState;
            {
                x &= Mode & eAll;
                if  (((Act1 | Act2) & eDisableEventRead) && s == fileno(stdin))
                {
                    x &= ~ eRead;
                }
                if  (!x)
                {
                    //m_hActiveState.Remove(s);
                    if (pSock->m_ActiveStateListItem.Next)
                    {
                        m_ActiveStateList.Release(&pSock->m_ActiveStateListItem);
                    }
                }
                else
                {
                    //m_hActiveState.AddOver(s, x);
                    pSock->m_ActiveState = x;
                    if (!pSock->m_ActiveStateListItem.Next)
                    {
                        m_ActiveStateList.AddAtBack(&pSock->m_ActiveStateListItem);
                    }
                }
            }
        }
        // printf("MOD: %d %x\n", s, (int)ev.events);
        //if (m_epollfd == -1)
        //{
        //   __utf8_printf("m_epollfd == -1\n"); // REM 2022
        //}

        if  (
#if defined(TL_EPOLL) //&& defined(__SHAPERS)
            bMod //false //true
#else
            false
#endif

         && m_epollfd != -1)
        {
            //__utf8_printf("EPOLL_CTL_MOD tcp or udp %d\n", s);
        }
        /*
          if (
#if defined(TL_EPOLL) && defined(__SHAPERS)
              bTcpMod //false //true
#else
              false
#endif
          
           && m_epollfd != -1)
           {
               __utf8_printf("EPOLL_CTL_MOD tcp %d\n", s);
           }
*/
        if  (
#if defined(TL_EPOLL) //&& defined(__SHAPERS)
            bMod //false //true
#else
            false
#endif
         && m_epollfd != -1 && epoll_ctl(m_epollfd, EPOLL_CTL_MOD, s, &ev) == -1)
        //if (1 && m_epollfd != -1 && epoll_ctl(m_epollfd, EPOLL_CTL_MOD, s, &ev) == -1) // !!!2020!!! - test (should be exhausted)
        {
            perror("epoll_ctl: mod");
            return false;
        }
#endif
        return true;
#endif
    }
    return false;
}
//---------------------------------------------------------------------------
void CMaaFdSockets::DeleteAllSockets() noexcept
{
    IfDebug(100) { DP(printf("CMaaFdSockets::DeleteAllSockets()\n");) }
#if 0
    int k = m_SockHash.GetItemCount() + 1; // (warning_int)m_SockHash.GetMaxIndex();
    CMaaPtr<CMaaSocket *> Sockets(k);
    //CMaaPtr<int> Modes(k);
    //k = m_SockHash.EnumerateItems(k, Sockets, Modes);
    k = m_SockHash.EnumerateItems(k, Sockets, nullptr);

    for (int i = 0; i < k; i++)
    {
        CMaaSocket *pSock = Sockets[i];
        if  (/*pSock &&*/ !m_SockHash.Find(pSock))
        {
            IfDebug(100)
            {
                DP(CMaaString p = pSock->GetConnectionName();
                p = p.IsNotEmpty() ? p : CMaaString("<unknown>", CMaaString::eROStrlenMemString);
                printf("delete socket %p %s\n", pSock, (const char *)p);)
            }
            delete pSock;
        }
    }
#else
    CMaaDLink_pItem<CMaaSocket>* p;
    while ((p = m_SockList.LookAtFront()))
    {
        delete p->m_pItem;
    }
#endif
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
CMaaSockStartup::CMaaSockStartup(BOOL fThrow, int Flags)
#ifdef __SOCK_NEW_TIMERS
:   m_Time(GetHRTime(true))
#endif
{
#ifndef __SOCK_NEW_TIMERS
    GetHRTime(true);
#endif
#ifdef _WIN32
    if  (!hEv0)
    {
        //hEv0 = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        //hEv1 = CreateEvent(nullptr, TRUE, TRUE, nullptr);
        hEv0 = WSACreateEvent();
        hEv1 = WSACreateEvent();
        WSASetEvent(hEv1);
    }
#endif

    m_IsOk = false;
    gCMaaFdSockets = nullptr; //TL_NEW CMaaFdSockets();
    m_pTcpFileMainThread = nullptr;
#ifndef __unix__
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(2, 2);
    if  (WSAStartup(wVersionRequested, &m_WsaData) == SOCKET_ERROR)
    {
        //LastWsaError = WSAGetLastError ();
        if  (fThrow)
        {
            throw XTOOSockErr("WSAStartup failed", nullptr);
        }
        else
        {
            return;
        }
    }
#endif
    //gCMaaConnectTasks = TL_NEW CMaaTasks;
#ifdef _WIN32
    m_pTcpFileMainThread = TL_NEW CMaaTcpFileMainThread;
    m_IsOk = m_pTcpFileMainThread ? true : false;
    //m_IsOk = StartTcpFileSockets();
    //m_IsOk = true;
#else
    if  ((Flags & 1))
    {
#ifndef TL_EPOLL00000000000000 // TL_EPOLL00000000000000 to enable TcpFileSockets !! 2019: TcpFileSockets works with TL_EPOLL !
        StartTcpFileSockets();
        m_IsOk = m_pTcpFileMainThread ? true : false;
#else
        // 29.09.2014 !!!!!!!!!! //  StartTcpFileSockets(); // TODO!!
        m_IsOk = m_pTcpFileMainThread || true ? true : false; // always
#endif
    }
    else
    {
        m_IsOk = true;
    }
#endif
}
//---------------------------------------------------------------------------
#ifdef __SOCK_NEW_TIMERS
/*
_qword CMaaSockStartup::GetTime() noexcept
{
    //m_TimerMutex.LockM();
    const _qword t = m_Time.GetTime(1000000);
    //m_TimerMutex.UnLockM();
    return t;
}
*/
#endif
//---------------------------------------------------------------------------
bool CMaaSockStartup::StopTcpFileSockets() noexcept
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    CMaaTcpFileMainThread * p = m_pTcpFileMainThread;
    m_pTcpFileMainThread = nullptr;
    Locker_UnLock;
    if (p)
    {
        p->Exit();
        delete p;
    }
    //m_pTcpFileMainThread = nullptr;
    return p != nullptr;
}

bool CMaaSockStartup::StartTcpFileSockets()
{
    bool bRetCode = true;
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (!m_pTcpFileMainThread)
    {
        m_pTcpFileMainThread = TL_NEW CMaaTcpFileMainThread(nullptr, false);
        Locker_UnLock;
        if  (m_pTcpFileMainThread)
        {
            if  (m_pTcpFileMainThread->Create())
            {
                return true;
            }
            delete m_pTcpFileMainThread;
            m_pTcpFileMainThread = nullptr;
            //StopTcpFileSockets();
        }
        bRetCode = false;
    }
    else
    {
        Locker_UnLock;
    }
    return bRetCode;
}
//---------------------------------------------------------------------------
CMaaSockStartup::~CMaaSockStartup ()
{
    //fprintf(stderr, "CMaaSockStartup::~CMaaSockStartup() - SetExitFileSockThreads()\n"); fflush(stdout);
    SetExitFileSockThreads();

    //fprintf(stderr, "CMaaSockStartup::~CMaaSockStartup() - 1 %p\n", m_pTcpFileMainThread); fflush(stderr);
    //delete m_pTcpFileMainThread;
    StopTcpFileSockets();

    //fprintf(stderr, "CMaaSockStartup::~CMaaSockStartup() - 2\n"); fflush(stderr);
    delete gCMaaFdSockets;
    gCMaaFdSockets = nullptr;
    //fprintf(stderr, "CMaaSockStartup::~CMaaSockStartup() - 3\n"); fflush(stderr);

#ifdef __SHAPERS
    //fprintf(stderr, "CMaaSockStartup::~CMaaSockStartup() - 4\n"); fflush(stderr);
    m_ShaperThread.Exit();
#endif

#ifndef __unix__
    //fprintf(stderr, "CMaaSockStartup::~CMaaSockStartup() - 5\n"); fflush(stderr);
    WSACleanup();
#endif
    //fprintf(stderr, "Exit from CMaaSockStartup::~CMaaSockStartup()\n"); fflush(stderr);
}
//---------------------------------------------------------------------------
bool CMaaSockStartup::IsOk() noexcept
{
    return m_IsOk;
}
//---------------------------------------------------------------------------
int CMaaSockConsole::EnterString(const char * str)
{
    if  (!strcmp(str, "exit") || !strcmp(str, "quit"))
    {
        printf("Exiting...");
        throw "User break";
    }
    return 0;
}
//---------------------------------------------------------------------------
void CMaaSockConsole::Escape()
{
    // for windows only
    printf("Exiting...");
    throw "User break";
}
//---------------------------------------------------------------------------
int CMaaSockConsole::Notify_Read()
{
#ifndef __unix__
    int fRet = 1;
    while(kbhit() && m_Pos < sizeof(m_Buff) - 1)
    {
        char c = getch();
        if  (c == 0x1b)
        {
            Escape();
            continue;
        }
        if  (c != '\r' && c != '\n')
        {
            printf("%c", c);
            m_Buff[m_Pos++] = c;
            if  (m_Pos >= sizeof(m_Buff) - 1)
            {
                m_Buff[m_Pos] = 0;
                fRet = 0;
                break;
            }
        }
        if  (c == '\r' || c == '\n')
        {
            //printf ( "\n", c );
            printf("\n");
            m_Buff[m_Pos] = 0;
            fRet = 0;
            break;
        }
    }
    if  (fRet)
    {
        return 0;
    }
    m_Pos = 0;
#else
    if  ( !fgets ( m_Buff, sizeof ( m_Buff ), stdin ) )
    {
        return 0;
    }
    if  ( m_Buff [ strlen ( m_Buff ) - 1 ] == '\n' )
    {
        m_Buff [ strlen ( m_Buff ) - 1 ] = 0;
    }
#endif
    return EnterString(m_Buff);
}
//---------------------------------------------------------------------------
int CMaaSockConsole::Notify_Write()
{
    return eDisableWrite;
}
//---------------------------------------------------------------------------
CMaaString CMaaSockConsole::GetConnectionName() noexcept
{
#ifdef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    //return CMaaString{ "stdin console" };
    return "stdin console";
#else
    return CMaaString("stdin console", CMaaString::eROStrlenMemString);
#endif
}
//---------------------------------------------------------------------------
CMaaSockConsole::CMaaSockConsole(CMaaFdSockets * pFdSockets)
:   CMaaTcpSocket(pFdSockets),
    m_Pos(0)
{
#ifdef __unix__
    SetSocket(0);
#else
    SetSocket(_fileno(stdin));
#endif
    AddFdSocket();
    ChangeFdModeEx(eRead);
}
//---------------------------------------------------------------------------
CMaaSockConsole::~CMaaSockConsole()
{
    RemoveFdSocket();
    m_Socket = INVALID_SOCKET;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Read()
{
    if  (m_len < (int)sizeof(m_Buffer))
    {
        int r = RecvData(&m_Buffer[m_len], (int)sizeof(m_Buffer) - m_len);
        if  (IsClosed(r))
        {
            // ending connection
            //printf ( "\nReceiving 0 bytes of data - closing connection.\n" );
            return CloseByException ( "Receiving 0 bytes of data - closing connection.");
        }
        fwrite(&m_Buffer[m_pos], 1, r, stdout);
        m_len += r;
        return eWrite;
    }
    return eDisableRead;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Write()
{
    if  (m_pos < m_len)
    {
        int w = SendData(&m_Buffer[m_pos], m_len - m_pos);
        m_pos += w;
        if  (w > 0)
        {
            memmove(m_Buffer, m_Buffer + m_pos, m_len - m_pos);
            m_len -= m_pos;
            m_pos = 0;
        }
        return (m_len < (int)sizeof(m_Buffer)) ? eRead : 0;
    }
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Error()
{
    //printf("\nSome error in Echo server...\n");
    CloseByException("Some error in Echo server...");
    return 0;
}
//---------------------------------------------------------------------------
int CMaaEchoConnection::Notify_Accepted(_IP /*IpFrom*/, _Port /*Port*/)
{
    //printf ( "\nAccepted connection from %s:%d\n", CMaaIpToText ( IpFrom ), Port );
    //SendData ( "", 0 );
    return 0;
}
//---------------------------------------------------------------------------
CMaaEchoConnection::CMaaEchoConnection(CMaaFdSockets * pFdSockets, CMaaEchoServer * /*pServer*/)
:   CMaaTcpSocket(pFdSockets)
{
    m_pos = m_len = 0;
}
//---------------------------------------------------------------------------
CMaaEchoConnection::~CMaaEchoConnection()
{
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// Servers
//---------------------------------------------------------------------------
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets * pFdSockets, int Port, const char * ServerName)
:   CMaaTcpSocket(pFdSockets) //, AF_INET)
{
    ServerName = ServerName ? ServerName : "CMaaUniversalServer";
    m_ServerName.Format("%s on port %d", ServerName, Port);
    Bind ( Port );
    Listen ();
    AddFdSocket ();
}
//---------------------------------------------------------------------------
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets * pFdSockets, int Port, int domain, const char * ServerName)
:   CMaaTcpSocket(pFdSockets, domain)
{
    ServerName = ServerName ? ServerName : domain == AF_INET6 ? "CMaaUniversalServer (ipv6)" : "CMaaUniversalServer";
    m_ServerName.Format("%s on port %d", ServerName, Port);
    if  (domain == AF_INET6)
    {
        Bind6(Port);
    }
    else
    {
        Bind(Port);
    }
    Listen();
    AddFdSocket();
}
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets * pFdSockets, CMaaString Port, const char * ServerName)
:   CMaaTcpSocket(pFdSockets, Port[0] == '[' ? AF_INET6 : AF_INET)
{
    ServerName = ServerName ? ServerName : "CMaaUniversalServer";
    m_ServerName.Format("%s on %S", ServerName, &Port);
    if  (Port[0] == '[')
    {
        //__utf8_printf("Bind6(%S)...\n", &Port);
        Bind6(Port);
    }
    else
    {
        Bind(Port);
    }
    Listen();
    AddFdSocket();
}
//---------------------------------------------------------------------------
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets* pFdSockets, int Port, const CMaaString& ServerName)
:   CMaaTcpSocket(pFdSockets) //, AF_INET)
{
    if (ServerName.IsEmpty())
    {
        m_ServerName.Format("%s on port %d", "CMaaUniversalServer", Port);
    }
    else
    {
        m_ServerName.Format("%S on port %d", &ServerName, Port);
    }
    Bind(Port);
    Listen();
    AddFdSocket();
}
//---------------------------------------------------------------------------
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets* pFdSockets, int Port, int domain, const CMaaString& ServerName)
:   CMaaTcpSocket(pFdSockets, domain)
{
    if (ServerName.IsEmpty())
    {
        m_ServerName.Format("%s on port %d", domain == AF_INET6 ? "CMaaUniversalServer (ipv6)" : "CMaaUniversalServer", Port);
    }
    else
    {
        m_ServerName.Format("%S on port %d", &ServerName, Port);
    }
    if (domain == AF_INET6)
    {
        Bind6(Port);
    }
    else
    {
        Bind(Port);
    }
    Listen();
    AddFdSocket();
}
CMaaUnivServer::CMaaUnivServer(CMaaFdSockets* pFdSockets, CMaaString Port, const CMaaString& ServerName)
:   CMaaTcpSocket(pFdSockets, Port[0] == '[' ? AF_INET6 : AF_INET)
{
    if (ServerName.IsEmpty())
    {
        m_ServerName.Format("%s on %S", "CMaaUniversalServer", &Port);
    }
    else
    {
        m_ServerName.Format("%S on %S", &ServerName, &Port);
    }
    if (Port[0] == '[')
    {
        //__utf8_printf("Bind6(%S)...\n", &Port);
        Bind6(Port);
    }
    else
    {
        Bind(Port);
    }
    Listen();
    AddFdSocket();
}
//---------------------------------------------------------------------------
CMaaUnivServer::~CMaaUnivServer()
{
    //RemoveFdSocket();
}
//---------------------------------------------------------------------------
//CMaaTcpSocket * CMaaUnivServer::CreateNewConnection(CMaaFdSockets * pFdSockets)
//{
//     return nullptr;
//}
//---------------------------------------------------------------------------
extern int gAcceptCatchErrors;
int gAcceptCatchErrors = 0;
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Read()
{
    // TODO: Create thread, and CMaaFdSockets
    //
    // CMaaFdSockets * pFdSockets

    CMaaSockThread * Thr = nullptr;

    _IP IpFrom = 0;
    _byte Ip6From[16]; memset(Ip6From, 0, sizeof(Ip6From));
    _Port Port = 0;
    SOCKET s = INVALID_SOCKET;
    const int domain = GetDomainSock();

    CMaaTcpSocket * pConnection = nullptr;
    try
    {
#ifdef TOOLSLIB_USE_IPV6
        s = domain == AF_INET6 ? Accept6(Ip6From, &Port) : Accept(&IpFrom, &Port);
#else
        s = Accept(&IpFrom, &Port);
#endif
        //closesocket ( s );
        //m_pFdSockets->SetTimeOut(20);
        //return 0;
        Thr = m_pFdSockets->m_pThread->CreateNewThread();
        if  (Thr)
        {
            //printf("Thr->m_pFdSockets = %p\n", Thr->m_pFdSockets);
            pConnection = CreateNewConnection(Thr->m_pFdSockets);
            //printf("pConnection = %p\n", pConnection);
        }
    }
    catch(...)
    {
        if  (s != INVALID_SOCKET)
        {
            closesocket(s);
        }
        if  (Thr)
        {
            delete Thr;
        }
        throw;
    }
    if  (!pConnection)
    {
        closesocket(s);
        if  (Thr)
        {
            delete Thr;
        }
    }
    else
    {

        //printf("1.  Thr->m_pFdSockets = %p, pConnection = %p\n", Thr->m_pFdSockets, pConnection);
        pConnection->RemoveFdSocket(); // !!2020!!
        pConnection->SetSocket(s, domain);
        pConnection->AddFdSocket();
        // :::TEST:::
#ifdef __unix__
        IfDebug(1)
        {
            //printf("");
            //Trace("12345: Accepted socket %d, setting nonblocking mode", s);
        }
        pConnection->SetNonBlockingMode();
#endif

        //printf("2.  Thr->m_pFdSockets = %p, pConnection = %p\n", Thr->m_pFdSockets, pConnection);
        bool bError = false;
        try
        {
            pConnection->UpdateHrTime(e_fds_connected_at_time);
            pConnection->ChangeFdModeEx(domain == AF_INET6 ? pConnection->Notify_Accepted6(Ip6From, Port) : pConnection->Notify_Accepted(IpFrom, Port));
            //printf("after Notify_Accepted() and ChangeFdMode() pConnection->GetFdMode() returns %x\n", pConnection->GetFdMode());

            //printf("3.  Thr->m_pFdSockets = %p, pConnection = %p\n", Thr->m_pFdSockets, pConnection);
            if  (!Thr->Create())
            {
                //printf("!Thr->Create()\n");
                delete Thr;
                Thr = nullptr;
                StaticError("Can't create thread");
            }
        }
        catch(...)
        {
            bError = true;
            gAcceptCatchErrors++;
        }
        if  (bError)
        {
            delete Thr;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Write()
{
    // server does not perform write operations
    return eDisableWrite;
}
//---------------------------------------------------------------------------
int CMaaUnivServer::Notify_Error()
{
    CloseByException("Some error in listened socket...\n");
    return 0;
}
//---------------------------------------------------------------------------
CMaaString CMaaUnivServer::GetConnectionName() noexcept
{
    return m_ServerName;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// class CMaaTcpConnection
//---------------------------------------------------------------------------
CMaaTcpConnection::Buffer::Buffer(int Sz, const void * Buff)
:   m_BuffSize(0),
    m_Pos(0)
{
    m_pBuffer = TL_NEW char[Sz + 1];
    m_pBuffer[0] = 0;
    if  (Buff && m_pBuffer)
    {
        memcpy(m_pBuffer, Buff, m_BuffSize = Sz);
        m_pBuffer[Sz] = 0;
    }
}
//---------------------------------------------------------------------------
CMaaTcpConnection::Buffer::~Buffer()
{
    delete [] m_pBuffer;
}
//---------------------------------------------------------------------------
CMaaString CMaaTcpConnection::GetConnectionName() noexcept
{
    return m_ConnName;
}
//---------------------------------------------------------------------------
void CMaaTcpConnection::Send(const char * Buff, int Size)
{
    Buffer * pb = TL_NEW Buffer(Size, Buff);
    m_SendList.AddAtBack(pb);
    //printf("Send(): GetFdMode() for %d is 0x%x\n", m_Socket, GetFdMode());
    if  (GetFdMode() & eConnect)
    {
        return;
    }
    ChangeFdMode(eAll);
    Notify_Write();
}
//---------------------------------------------------------------------------
void CMaaTcpConnection::SetName(const char * ConnName)
{
    m_ConnName.Format("%s %s", m_ClassName, (const char *)ConnName);
}
//---------------------------------------------------------------------------
CMaaTcpConnection::CMaaTcpConnection(CMaaFdSockets * pFdSockets, const char * ClassName)
:   CMaaTcpSocket(pFdSockets),
    m_ClassName(ClassName)
{
}
//---------------------------------------------------------------------------
CMaaTcpConnection::~CMaaTcpConnection()
{
    Buffer * pb;
    while( (pb = m_SendList.GetFromFront()) )
    {
        delete pb;
    }
}
//---------------------------------------------------------------------------
int CMaaTcpConnection::Notify_Accepted(_IP IpFrom, _Port Port)
{
    CMaaString ConnName;
    ConnName.Format("%I:%d->%d", IpFrom, Port, GetBindedPort());
    SetName(ConnName);
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpConnection::Notify_Accepted6(_byte * IpFrom, _Port Port)
{
    CMaaString ConnName;
    ConnName.Format("[%J]:%d->%d", IpFrom, Port, GetBindedPort());
    SetName(ConnName);
    return 0;
}
//---------------------------------------------------------------------------
int CMaaTcpConnection::SendBuffer()
{
    Buffer * b = m_SendList.GetFromFront();
    if  (!b)
    {
        return eEmptyList;
    }
    b->m_Pos += SendData(b->m_pBuffer + b->m_Pos, b->m_BuffSize - b->m_Pos);
    //printf("%d send %d/%d bytes\n", fIsClientSide, b.m_Pos, b.m_BuffSize);
    if  (b->m_BuffSize > b->m_Pos)
    {
        IfDebug(200) { DP(printf("SendBuffer () %d/%d\n%s\n", b->m_Pos, b->m_BuffSize, b->m_pBuffer);) }
        // not all buffer was sent immediately
        m_SendList.AddAtFront(b);
        return eToBeContinued;
    }
    //IfDebug(200) printf ( "SendBuffer () %d\n%s\n", b.m_BuffSize, b.m_pBuffer );
    delete b;
    return (m_SendList.LookAtFront()) ? eBufferSent : eLastBuffer;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// class CMaaTcpProxy
//---------------------------------------------------------------------------
CMaaTcpProxy::CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaString &TcpServName)
:   CMaaTcpConnection(pFdSockets, szClassName),
    fIsClientSide(1),
    pOtherSide(nullptr),
    m_TcpServName(TcpServName),
    fCloseByRead0(1),
    fNeedToClose(0),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    pOtherSide = TL_NEW CMaaTcpProxy(this->m_pFdSockets, this, m_TcpServName);
    AddFdSocket();
    m_Timer0.Attach(m_pFdSockets);
}
//---------------------------------------------------------------------------
CMaaTcpProxy::CMaaTcpProxy(CMaaFdSockets * pFdSockets, CMaaTcpProxy * pClientSide, CMaaString &TcpServName)
:   CMaaTcpConnection(pFdSockets, szClassName),
    fIsClientSide(0),
    pOtherSide(nullptr),
    m_TcpServName(TcpServName),
    fCloseByRead0(1),
    fNeedToClose(0),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    CMaaString tmp;
    tmp.Format("C ....->%S", &TcpServName);
    SetName(tmp);
    AddFdSocket();
#ifdef __SHAPERS
    //CMaaWin32Locker_Locker_gLock;
    //Locker_Lock;
    gpSockStartup&& gpSockStartup->m_SysShaperMutex.LockM();
    if  (gpSockStartup)
    {
        /*
          IfDebug(100)
          {
               printf("Adding shapers...\n"); fflush(stdout);
          }
          gpSockStartup->m_ShaperThread.m_Snd1000->Add(m_SndLLShaper);
          gpSockStartup->m_ShaperThread.m_Rcv1000->Add(m_RcvLLShaper);
        */
    }
    gpSockStartup&& gpSockStartup->m_SysShaperMutex.UnLockM();
    //Locker_UnLock;
#endif
    const int x = AsyncConnect(m_TcpServName);
    //     IfDebug(2) printf ( "Connect() retunts %d, socket = %d\n", x, GetSocket () );
    IfDebug(2) { DP(printf("AsyncConnect() retunts %d, socket = %d\n", x, (int)GetSocket());) }
    tmp.Format("%d->%S", GetBindedPort (), &TcpServName);
    SetName(tmp);
    pOtherSide = pClientSide;
    //     AddFdSocket ();
    m_Timer0.Attach(m_pFdSockets);
    /*
     if (!x)
     {
        ChangeFdMode ( eDisableConnect|eAll );
#ifdef __unix__
        SetNonBlockingMode();
#endif
     }
     else
     {
        StaticError("Connect");
        //throw 1;
     }
*/
}
//---------------------------------------------------------------------------
void CMaaTcpProxy::OnTimer(int f)
{
    switch(f)
    {
    case 0:
        CloseByException("Close");
        break;
    }
}
//---------------------------------------------------------------------------
int CMaaTcpProxy::Notify_Read()
{
    char Buff[4096 + 1];
    IfDebug(100)
    {
        DP(printf("CMaaTcpProxy::Notify_Read() for socket %d,  Recving Data...\n", (int)m_Socket); fflush(stdout);)
    }
    int r = RecvData(Buff, sizeof(Buff) - 1);
    IfDebug(100)
    {
        DP(printf("RecvData: %d bytes !!!!!!!!\n", r); fflush(stdout);)
    }

    if  (IsClosed(r))
    {
        // ending connection
        //.....
        if  ( pOtherSide )
        {
            pOtherSide -> pOtherSide = nullptr;
            pOtherSide -> fNeedToClose = 1;
            pOtherSide->m_Timer0.Start1(1);
            pOtherSide = nullptr;
        }
        return CloseByException ( "closing connection" );
    }
    /*
     if   ( ! pOtherSide )
     {
          try
          {
               pOtherSide = TL_NEW CMaaTcpProxy(this->m_pFdSockets, this, m_TcpServName);
          }
          catch ( XTOOSockErr & )
          {
          }
          if   ( !pOtherSide )
          {
               //SendHttpErrorResponse ( "Can't connect to server\n\r\n\r" );
               return CloseByException ( "Can't create connection to server" );
          }
     }
    */
    if  (pOtherSide && r > 0)
    {
        IfDebug(100)
        {
            DP(printf("CMaaTcpProxy::Notify_Read() for socket %d,  writing %d bytes data to socket %d...\n", (int)m_Socket, r, (int)pOtherSide->m_Socket); fflush(stdout);)
        }
        pOtherSide->Send(Buff, r);
        IfDebug(100)
        {
            DP(printf("CMaaTcpProxy::Notify_Read() for socket %d,  writing %d bytes data to socket %d - return from Send()\n", (int)m_Socket, r, (int)pOtherSide->m_Socket); fflush(stdout);)
        }
    }
    return eRead;
}
//---------------------------------------------------------------------------
int CMaaTcpProxy::Notify_Write()
{
    //printf("GetFdMode() for %d is 0x%x\n", m_Socket, GetFdMode());
    IfDebug(100)
    {
        DP(printf("CMaaTcpProxy::Notify_Write() for socket %d\n", (int)m_Socket); fflush(stdout);)
    }
    if  (GetFdMode() & eConnect)
    {
        IfDebug(100)
        {
            DP(Trace("CMaaTcpProxy::Notify_Write(): return 0 for socket %d: (GetFdMode() & eConnect)\n", (int)m_Socket);)
        }
        return 0;
    }
    Buffer * pb;
    int fBuff = 0;
    while( (pb = m_SendList.GetFromFront()) )
    {
        fBuff++;
        Buffer &b = *pb;
        IfDebug(100)
        {
            DP(printf("CMaaTcpProxy::Notify_Write(): sending %d bytes to socket %d...\n", b.m_BuffSize - b.m_Pos, (int)m_Socket); fflush(stdout);)
        }
        int xx = 0;
        b.m_Pos += (xx = SendData(b.m_pBuffer + b.m_Pos, b.m_BuffSize - b.m_Pos));
        IfDebug(100)
        {
            DP(printf("CMaaTcpProxy::Notify_Write(): %d bytes are sent to socket %d...\n", xx, (int)m_Socket); fflush(stdout);)
        }

        //printf("b.m_Pos = %d, b.m_BuffSize = %d\n", b.m_Pos, b.m_BuffSize);
        if  (b.m_BuffSize > b.m_Pos)
        {
            //printf ( "attach again buffer\n" );
            // not all buffer was sent immediately
            m_SendList.AddAtFront(pb);
            return eRead;
        }
        delete pb;
    }
    // nothing to be send
    if  (!fBuff)
    {
        if  (fNeedToClose)
        {
            //printf("point 1: fNeedToClose\n");
            return CloseByException("closing connection (after write all data)");
        }
        IfDebug(100)
        {
            DP(Trace("CMaaTcpProxy::Notify_Write(): return (eAll|eDisableWrite) for socket %d\n", (int)m_Socket);)
        }
        return eDisableWrite;
    }
    IfDebug(100)
    {
        DP(Trace("CMaaTcpProxy::Notify_Write(): return eAll for socket %d\n", (int)m_Socket);)
    }
    return eWrite;
}
//---------------------------------------------------------------------------
CMaaTcpProxy::~CMaaTcpProxy()
{
    Buffer * pb;
    while( (pb = m_SendList.GetFromFront()) )
    {
        delete pb;
    }
    if  (pOtherSide)
    {
        pOtherSide->pOtherSide = nullptr;
        IfDebug(100) { DP(printf("delete        %p %s\n", pOtherSide, (const char *)pOtherSide->GetConnectionName());) }
        delete pOtherSide;
    }
}
//---------------------------------------------------------------------------
const char CMaaTcpProxy::szClassName [] = "TcpProxy";
//---------------------------------------------------------------------------
CMaaTcpSocket * CMaaTcpProxyServer::CreateNewConnection(CMaaFdSockets * pFdSockets)
{
    return TL_NEW CMaaTcpProxy(pFdSockets, m_ServName);
}
//---------------------------------------------------------------------------
CMaaTcpProxyServer::CMaaTcpProxyServer(CMaaFdSockets * pFdSockets, int Port, const char * Server)
:   CMaaUnivServer(pFdSockets, Port, "TcpProxyServer"),
    m_ServName ( Server )
{
}
//---------------------------------------------------------------------------


//------------
//---------------------------------------------------------------------------
// class CMaaTcpStdoutConnection
//---------------------------------------------------------------------------
CMaaTcpStdoutConnection::CMaaTcpStdoutConnection(CMaaFdSockets * pFdSockets, CMaaString &TcpServName)
:   CMaaTcpConnection(pFdSockets, szClassName),
    m_TcpServName ( TcpServName ),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Timer0(this, 0)
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    //printf("::()\n");
    AddFdSocket();
    m_Timer0.Attach(m_pFdSockets);
}
//---------------------------------------------------------------------------
void CMaaTcpStdoutConnection::OnTimer(int f)
{
    switch(f)
    {
    case 0:
        CloseByException("Close");
        break;
    }
}
//---------------------------------------------------------------------------
int CMaaTcpStdoutConnection::Notify_Read()
{
    //printf("r");
    char Buff[4096 + 1];
    int r = RecvData(Buff, sizeof ( Buff ) - 1);
    if  (IsClosed(r))
    {
        return CloseByException("closing connection");
    }
    if  (r > 0)
    {
        fwrite(Buff, 1, r, stdout);
    }
    return eRead;
}
//---------------------------------------------------------------------------
int CMaaTcpStdoutConnection::Notify_Write()
{
    //printf("w");
    /*
     if (GetFdMode() & eConnect)
     {
        return 0;
     }
     */
    return eDisableWrite;
}
//---------------------------------------------------------------------------
CMaaTcpStdoutConnection::~CMaaTcpStdoutConnection()
{
}
//---------------------------------------------------------------------------
const char CMaaTcpStdoutConnection::szClassName [] = "TcpStdoutConnection";
//---------------------------------------------------------------------------
CMaaTcpSocket * CMaaTcpStdoutServer::CreateNewConnection(CMaaFdSockets * pFdSockets)
{
    //printf("create\n");
    return TL_NEW CMaaTcpStdoutConnection(pFdSockets, m_ServName);
}
//---------------------------------------------------------------------------
CMaaTcpStdoutServer::CMaaTcpStdoutServer(CMaaFdSockets * pFdSockets, int Port)
:   CMaaUnivServer(pFdSockets, Port, "TcpStdoutServer")
{
    //printf("constructor\n");
}
//---------------------------------------------------------------------------

//------------



#ifdef DebugLevel
#undef DebugLevel
#endif
//int DebugLevel = 0;

#endif //#ifndef TOOLSLIB_SINGLE_THREAD
#endif // __TOOLS_USE_WINSOCK || __TOOLS_USE_WINSOCK2



#ifndef __unix__

#else

//typedef long long __int64;

#include <signal.h>

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

int gSockThreads = 0;

#ifndef TOOLSLIB_SINGLE_THREAD
CMaaSockThread::CMaaSockThread(CMaaSockThread * pParent)
:   m_Refs(0),
    m_WakeUpFlags(0),
    m_pParent(pParent)
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    gSockThreads++;
    static int _Id = 0;
    Id = ++_Id;
    Locker_UnLock;
    DP(printf("::CMaaSockThread() %d\n", Id);)
    //m_Refs = 0;
    m_bExiting = false;
    m_pFdSockets = TL_NEW CMaaFdSockets(this);
    //m_pFdSockets->SetTimeOut(0, 10 * 1000);
#ifdef __unix__
    m_WakeupPair = nullptr;
    if  (m_pFdSockets)
    {
        m_WakeupPair = TL_NEW CMaaWakeUpPair(m_pFdSockets);
    }
#else
    //m_hWakeup = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_hWakeup = WSACreateEvent();
#endif
#ifdef __unix__
    if  (!m_pFdSockets || !m_WakeupPair)
#else
    if  (!m_pFdSockets)
#endif
    {
#ifdef __unix__
        delete m_WakeupPair;
#endif
        delete m_pFdSockets;
        //CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        gSockThreads--;
        Locker_UnLock;
        throw 0;
    }
    //CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    if  (pParent)
    {
        pParent->m_ChildThreads.AddAtBack(this);
    }
    Locker_UnLock;
}

int CMaaSockThread::AddRef() noexcept
{
    return ++m_Refs;
}

int CMaaSockThread::DelRef() noexcept
{
    return --m_Refs;
}

void CMaaSockThread::AddCommand(int Cmd, CMaaSockThread * Par1, void * Par2)
{
    AddCommand(TL_NEW SCmd(Cmd, Par1, Par2));
}

void CMaaSockThread::AddCommand(SCmd * c) noexcept
{
    if  (c)
    {
        m_Mutex.LockM();
        m_List.AddAtBack(c);
        m_Mutex.UnLockM();

#ifndef _WIN32
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
#endif
        WakeUp();
#ifndef _WIN32
        Locker_UnLock;
#endif
    }
}
CMaaSockThread::~CMaaSockThread()
{
    //printf("--1--\n"); fflush(stdout);

    DP(printf("::~CMaaSockThread() %d\n", Id);)

    try
    {
        AddCommand(-1);
    }
    catch(...)
    {
    }

    //printf("--2--\n"); fflush(stdout);

    Join();

    //printf("--2.1--\n"); fflush(stdout);


    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;

    //printf("--2.2--\n"); fflush(stdout);

#ifdef __unix__

    delete m_WakeupPair;
    m_WakeupPair = nullptr;
#else
    //CloseHandle(m_hWakeup); m_hWakeup = nullptr;
    WSACloseEvent(m_hWakeup); m_hWakeup = nullptr;
#endif

    gSockThreads--;


    //printf("--3--\n"); fflush(stdout);

    // +2016
    if  (m_pParent)
    {
        m_pParent->m_ChildThreads.Release(this);
        m_pParent = nullptr;
    }
    Locker_UnLock;

    //try
    //{
        m_pFdSockets->DeleteAllSockets(); // let sockets be in unlocked mode in ~   (2016)
    //}
    //catch(...)
    //{
    //}

    Locker_Lock; //??

    //printf("--4--\n"); fflush(stdout);

    delete m_pFdSockets; // sync need?

    //printf("--5--\n"); fflush(stdout);

    Locker_UnLock; //?? //27.03.2025

    for (;;)
    {

        //printf("--6--\n"); fflush(stdout);

        m_Mutex.LockM();
        SCmd * p = m_List.GetFromFront();
        m_Mutex.UnLockM();

        if  (!p)
        {
            break;
        }
        delete p;
    }

    //if  (m_pParent)
    //{

        //printf("--7--\n"); fflush(stdout);

        //m_pParent->m_ChildThreads.Release(this);
    //}

    //Locker_UnLock;
}

void CMaaSockThread::WakeUpChildThreads() noexcept
{
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;

    for (CMaaSockThread * t = m_ChildThreads.LookAtFront(); t; t = m_ChildThreads.Next(t))
    {
        t->WakeUp();
    }

    Locker_UnLock;
}

//int gggggggg = 0;

void CMaaSockThread::RemoveWakeUpPair() noexcept
{
#ifdef __unix__
    delete m_WakeupPair; m_WakeupPair = nullptr;
#endif
}
void CMaaSockThread::WakeUp(unsigned char flags) noexcept
{
    m_WakeUpFlags |= flags;
#ifdef __unix__
    //CMaaWin32Locker_Locker_gLock;
    //Locker_Lock;

    if  (m_WakeupPair)
    {
        m_WakeupPair->WakeUp();
    }

    //Locker_UnLock;
#else
    //SetEvent(m_hWakeup);
    WSASetEvent(m_hWakeup);
#endif
}

unsigned CMaaSockThread::Run()
{
    SetThreadName(-1, "CMaaSockThread");
    //    printf("CMaaSockThread::Run(), m_pFdSockets=%p\n", m_pFdSockets);
    int Ret = 0;
#ifndef __unix__

#else
    signal(SIGPIPE, OnMySIGPIPE);
    signal(SIGBUS, OnMySIGBUS);
#endif

    try
    {
        Process();
    }
    catch(const char * Msg)
    {
        //printf("catch(\"%s\");\n", Msg);
        XTOOWsaErr  WsaErr;
        XTOOErrno   Errno;
        XTOOSockErr ErrMsg(Msg, nullptr);
        Ret = 1;
    }
    catch(XTOOError &/*x*/)
    {
        //printf("catch(XTOOError &);\n", x.GetMsg());
        XTOOErrMsg  ErrMsg("\n");
        Ret = 2;
    }
    catch(eExitThread &)
    {
        //printf("catch(eExitThread &);\n");
        //printf("catch(eExitThread &)\r\n");
        //fflush(stdout);
        Ret = 4;
    }
    catch(...)
    {
        //printf("catch(...)\r\n");
        //fflush(stdout);
        try
        {
            XTOOWsaErr  WsaErr;
            XTOOErrno   Errno;
            XTOOErrMsg  ErrMsg("\nUnknown error.");
        }
        catch(...)
        {
            char * p = strerror ( errno );
            printf("\nUnknown error. Errno = %d %s\n", errno, p ? p : ""); //!!! printf
        }
        Ret = 3;
    }
    //printf("exiting...\r\n");
    //fflush(stdout);
    //    printf("CMaaSockThread::Run() - if (m_pParent) m_pParent->AddCommand(1, this);\n");

    /*--------------------------------*/
    //m_pFdSockets->DeleteAllSockets();
    /*--------------------------------*/
    if  (m_pParent)
    {
        m_pParent->AddCommand(1, this);
    }
    //printf("exit threadwith code=%d\r\n", Ret);
    //fflush(stdout);
    //    printf("CMaaSockThread::Run() - return\n");
    return (unsigned)Ret;
}

CMaaSockThread * CMaaSockThread::CreateNewThread()
{
    DP(printf("CMaaSockThread::CreateNewThread()...\n");)
    return TL_NEW CMaaSockThread(this);
}

#ifdef __unix__
#ifdef TL_EPOLL111x
// true if pair is got, s1 - (EPOOL:non)blocking socket, s2 - blocking socket
bool CMaaSocketPair(CMaaSocket &s1, CMaaSocket &s2)
{
    bool Ret = false;
    //printf("p1 = %p, p2 = %p\r\n", p1, p2);
    //fflush(stdout);
    if  (!s1.m_TcpSocketFlag || !s2.m_TcpSocketFlag)
    {
        return Ret;
    }
    //CMaaTcpSocket* p1 = (CMaaTcpSocket*)&s1;
    //CMaaTcpSocket* p2 = (CMaaTcpSocket*)&s2;

    CMaaUnivHash<int, int> hAcceptedSockets;

    struct epoll_event ev, events[2];
    int listen_sock, connect_sock = -1, accept_sock = -1, epollfd = -1;
    struct sockaddr_in sin_ls, sin_cs, sin_a;
    int Port = -1;
    bool bConnected = false;

    // Set up listening socket, 'listen_sock' (socket(), bind(), listen())

    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    connect_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    while(listen_sock != -1 && connect_sock != -1)
    {
        if  (fcntl(connect_sock, F_SETFL, O_NONBLOCK))
        {
            break;
        }
        for (int i = 65535; i >= 64000; i--)
        {
            memset(&sin_ls, 0, sizeof(sin_ls));
            sin_ls.sin_family      = AF_INET;
            sin_ls.sin_port        = htons(i);
            sin_ls.sin_addr.s_addr = htonl(0x7f000001);
            memcpy(&sin_cs, &sin_ls, sizeof(sin_ls));
            if  (!bind(listen_sock, (struct sockaddr *) &sin_ls, sizeof(sin_ls)))
            {
                if  (listen(listen_sock, 5) >= 0)
                {
                    Port = i;
                }
                break;
            }
        }
        if  (Port < 0)
        {
            break;
        }
        epollfd = epoll_create(2);
        if  (epollfd == -1)
        {
            //perror("epoll_create");
            break;
        }

        ev.events = EPOLLIN;
        ev.data.fd = listen_sock;
        if  (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1)
        {
            //perror("epoll_ctl: listen_sock");
            break;
        }

        {
            ev.events = EPOLLIN|EPOLLOUT;//|EPOLLET;
            ev.data.fd = connect_sock;
            if  (epoll_ctl(epollfd, EPOLL_CTL_ADD, connect_sock, &ev) == -1)
            {
                //perror("epoll_ctl: connect_sock");
                break;
            }
        }
        int x;
        if  ( (x=connect(connect_sock, (struct sockaddr *)&sin_cs, sizeof(sin_cs))) )
        {
            switch( __CA_ERRNO )
            {
            case __CA_EISCONN:
            case __CA_EALREADY:
                x = 0;
                bConnected = true;
                break;
            case __CA_EINPROGRESS:
            case EAGAIN:
                x = -1;
                break;
            default:
                x = -2; //error
            }
        }
        if  (x == -2)
        {
            //perror("connect");
            break;
        }
        if  (x == -1 && 0)
        {
            ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
            ev.data.fd = connect_sock;
            if  (epoll_ctl(epollfd, EPOLL_CTL_ADD, connect_sock, &ev) == -1)
            {
                //perror("epoll_ctl: connect_sock");
                break;
            }
        }
        bool bLoop = true;
        while(bLoop)
        {
            //printf("epoll_wait();\n");
            int nfds = epoll_wait(epollfd, events, 2, -1);
            //printf("epoll_wait() : %d\n", nfds);
            if  (nfds == -1)
            {
                //perror("epoll_pwait");
                bLoop = false;
                break;
            }

            for (int n = 0; n < nfds; ++n)
            {
                //printf("%d %x\n", events[n].data.fd, (int)events[n].events);
                if  (events[n].data.fd == listen_sock)
                {
                    //printf("accept sk...\n");
                    socklen_t len;
                    memset(&sin_a, 0, len = sizeof(sin_a));
                    int a = accept(listen_sock, (struct sockaddr *)&sin_a, &len);
                    if  (a == -1)
                    {
                        //perror("accept");
                        bLoop = false;
                        break;
                    }
                    //_IP IpFrom = htonl(sin_a.sin_addr.s_addr);
                    int p = htons(sin_a.sin_port);
                    hAcceptedSockets.AddOver(p, a);
                    if  (bConnected)
                    {
                        struct sockaddr_in s;
                        socklen_t len;
                        memset(&s, 0, len = sizeof(s));
                        if  (getsockname(connect_sock, (struct sockaddr *)&s, &len))
                        {
                            //perror("cs1 - getsockname");
                            bLoop = false;
                            break;
                        }
                        //printf("p = %d, l.c.p = %d\n", p, ntohs(s.sin_port));
                        if  (p == ntohs(s.sin_port))
                        {
                            accept_sock = a;
                            hAcceptedSockets.Remove(p);
                            bLoop = false;
                            break;
                        }
                        closesocket(a);
                        hAcceptedSockets.Remove(p);
                    }
                }
                if  (events[n].data.fd == connect_sock)
                {
                    //printf("connect sk...\n");
                    if  (events[n].events & EPOLLERR)
                    {
                        //perror("cs - EPOLLERR");
                        closesocket(connect_sock); connect_sock = -1;
                        bLoop = false;
                        break;
                    }
                    bConnected = true;
                    {
                        struct sockaddr_in s;
                        socklen_t len;
                        memset(&s, 0, len = sizeof(s));
                        if  (getsockname(connect_sock, (struct sockaddr *)&s, &len))
                        {
                            //perror("cs2 - getsockname");
                            bLoop = false;
                            break;
                        }
                        //printf("l.c.p = %d\n", ntohs(s.sin_port));
                        if  (!hAcceptedSockets.Remove(ntohs(s.sin_port), &accept_sock))
                        {
                            bLoop = false;
                            break;
                        }
                        CMaaUnivHash <int, int>::iterator it(hAcceptedSockets);
                        for (; it; ++it)
                        {
                            closesocket(it.data());
                            it.Remove();
                        }
                    }
                }
            }
        }
        break;
    }
    if  (accept_sock != -1 && connect_sock != -1)
    {
        // got pair
        s1.SetSocket(connect_sock);
        s2.SetSocket(accept_sock);
        DP(printf("pair: %d %d\n", connect_sock, accept_sock);)
        Ret = true;
    }
    else
    {
        DP(printf("pair: error\n");)
        if  (accept_sock != -1)
        {
            closesocket(accept_sock); accept_sock = -1;
        }
        if  (connect_sock != -1)
        {
            closesocket(connect_sock); connect_sock = -1;
        }
    }
    closesocket(listen_sock); listen_sock = -1;
    CMaaUnivHash <int, int>::iterator it(hAcceptedSockets);
    for (; it; ++it)
    {
        closesocket(it.data());
    }
    if  (epollfd != -1)
    {
        close(epollfd);
    }
    return Ret;
}
#else
bool CMaaSocketPair(CMaaSocket &s1, CMaaSocket &s2)
{
    int d[2];
    d[0] = d[1] = -1;
    //CMaaTcpSocket* p1 = s1.m_pTcpSocketCast;// dynamic_cast<CMaaTcpSocket*>(&s1);
    //CMaaTcpSocket* p2 = s2.m_pTcpSocketCast;// dynamic_cast<CMaaTcpSocket*>(&s2);
    //printf("p1 = %p, p2 = %p\r\n", p1, p2);
    //fflush(stdout);
    int r;
    if  (s1.m_TcpSocketFlag && s2.m_TcpSocketFlag)
    {
        r = socketpair(AF_UNIX, SOCK_STREAM/*|SOCK_NONBLOCK*/, 0, d);
        // r = pipe(d);
    }
    else
    {
        r = socketpair(AF_UNIX, SOCK_DGRAM/*|SOCK_NONBLOCK*/, 0, d);
    }
    if  (r)
    {
        //      printf("error %d\r\n\n\n", errno);
        return false;
    }
    //printf("d[0] = %d, d[1] = %d\r\n", d[0], d[1]);
    //fflush(stdout);
    s1.SetSocket(d[0]);
    s2.SetSocket(d[1]);
    return true;
}
#endif
#endif


CMaaSocketTimer::CMaaSocketTimer(int PeriodUs) noexcept
{
    //m_bActive = false;
    m_PeriodUs = PeriodUs;
    //m_pFdSockets = nullptr;

#if defined(__unix__) && !defined(__SOCK_NEW_TIMERS)
    memset(&m_Next, 0, sizeof(m_Next));
#endif

//#ifdef __SOCK_NEW_TIMERS
    //m_Handle = nullptr;
//#endif
}
CMaaSocketTimer::~CMaaSocketTimer()
{
    Detach();
}

_qword CMaaSocketTimer::GetPeriod() const noexcept
{
    _qword r;
    if  (m_pFdSockets)
    {
        m_pFdSockets->m_TimerMutex.LockM();
        r = m_PeriodUs;
        m_pFdSockets->m_TimerMutex.UnLockM();
    }
    else
    {
        r = m_PeriodUs;
    }
    return r;
}
bool CMaaSocketTimer::IsStarted() const noexcept
{
    return m_bActive;
}

bool CMaaSocketTimer::Start2(_qword PeriodUs, int iWakeUp) noexcept
{
    if  (m_pFdSockets)
    {
        m_pFdSockets->m_TimerMutex.LockM();
    }

    const bool bActiveOld = m_bActive;

    if  (PeriodUs)
    {
        m_PeriodUs = PeriodUs;
    }
    if  (m_PeriodUs <= 0)
    {
        m_bActive = false;
        if  (m_pFdSockets)
        {
            m_pFdSockets->m_TimerMutex.UnLockM();
        }
        return false;
    }
    
    bool bRet = true;

#ifndef __SOCK_NEW_TIMERS

#ifdef __unix__
    const _qword NextOld = 1000000 * m_Next.tv_sec + m_Next.tv_usec;
    gettimeofday(&m_Next, nullptr);
    const _qword Next = 1000000 * m_Next.tv_sec + m_Next.tv_usec + m_PeriodUs;
    m_Next.tv_usec = Next % 1000000;
    m_Next.tv_sec = Next / 1000000;
#else
    const _qword NextOld = m_Next;
    m_Next = GetTickCount() + (_dword)(m_PeriodUs / 1000);
    const _qword Next = m_Next;
    //if  (m_PeriodUs / 1000 == 0)
    //{
    //    m_Next++;
    ///}
#endif
    m_bActive = true;

#else

    _qword NextOld, Next;
    if  (m_pFdSockets)
    {
        CMaaSocketTimer* t;
        if (!m_pFdSockets->HeapLook(&NextOld, &t) || !t->IsStarted())
        {
            NextOld = m_Next;
        }

        Next = m_Next = m_pFdSockets->GetTime() + m_PeriodUs;
        if  (gpSockStartup)
        {
            //gpSockStartup->m_TimerMutex.LockM();
            if  (m_Next != -1)
            {
                if (m_Handle)
                {
                    m_pFdSockets->SetKeyByHandle(m_Handle, m_Next);
                    m_bActive = true;
                }
                else
                {
                    m_Handle = m_pFdSockets->HeapAdd(m_Next, this);
                    if (!m_Handle)
                    {
                        m_bActive = bRet = false;
                    }
                    else
                    {
                        m_bActive = true;
                    }
                }
            }
            else
            {
                if (m_Handle)
                {
                    m_pFdSockets->HeapDel(m_Handle);  m_Handle = nullptr;
                }
                m_bActive = bRet = false;
            }
            //gpSockStartup->m_TimerMutex.UnLockM();
        }
    }
    else
    {
        //NextOld = Next = 0;
        m_Next = -1;
        m_bActive = true;
        iWakeUp = 0;
    }

#endif

    if  (m_pFdSockets)
    {
        m_pFdSockets->m_TimerMutex.UnLockM();
    }

    if  (iWakeUp > 1 || (iWakeUp == 1 && (!bActiveOld || Next < NextOld)))
    {
        if  (m_pFdSockets && m_pFdSockets->m_pThread)
        {
            m_pFdSockets->m_pThread->WakeUp();
        }
    }

    return bRet; 
}

void CMaaSocketTimer::Stop() noexcept
{
    m_bActive = false;
}

void CMaaSocketTimer::Attach(CMaaFdSockets * pFdSockets) noexcept
{
    Detach();
    m_pFdSockets = pFdSockets;
    //m_pFdSockets->m_Timers.AddAtBack(this);
    m_pFdSockets->AddToList(this);
#ifdef __SOCK_NEW_TIMERS
    if  (m_bActive) // = never: Detach() --> Stop()
    {
        if  (1) // (gpSockStartup)
        {
            const _qword t0 = m_pFdSockets->GetTime();
            if  (m_Next == -1 || m_Next < t0)
            {
                m_Next = t0 + m_PeriodUs;
            }
            //if  (m_pFdSockets)
            {
                m_pFdSockets->m_TimerMutex.LockM();
                if  (m_Next != -1)
                {
                    m_Handle = m_pFdSockets->HeapAdd(m_Next, this);
                    if (!m_Handle)
                    {
                        m_bActive = false;
                    }
                }
                m_pFdSockets->m_TimerMutex.UnLockM();
            }
        }
        else
        {
            m_Next = -1;
            m_bActive = false;
        }
    }
#endif
}

void CMaaSocketTimer::Detach() noexcept
{
    //printf("void CMaaSocketTimer::Detach()\r\n");
    //fflush(stdout);
    if  (m_pFdSockets)
    {
        //printf("void CMaaSocketTimer::Detach() - Release\r\n");
        //fflush(stdout);
        //m_pFdSockets->m_Timers.Release(this);
        m_pFdSockets->ReleaseFromList(this);
        //printf("void CMaaSocketTimer::Detach() - Release - ok\r\n");
        //fflush(stdout);
#ifdef __SOCK_NEW_TIMERS
        if  (m_pFdSockets)
        {
            m_pFdSockets->m_TimerMutex.LockM();
            if  (m_Handle)
            {
                m_pFdSockets->HeapDel(m_Handle);  m_Handle = nullptr;
            }
            m_pFdSockets->m_TimerMutex.UnLockM();
        }
#endif
        m_pFdSockets = nullptr;
    }
    Stop();
}

#ifndef __SOCK_NEW_TIMERS

#ifdef __unix__
void CMaaSocketTimer::PreTimer(timeval * t)
{
    if  (!m_bActive)
    {
        return;
    }
    timeval tv;
    if  (!t)
    {
        gettimeofday(t = &tv, nullptr);
    }
    if  ((t->tv_sec > m_Next.tv_sec) || (t->tv_sec == m_Next.tv_sec && t->tv_usec >= m_Next.tv_usec))
    {
        m_Next.tv_usec += m_PeriodUs;
        if  (m_Next.tv_usec > 1000000)
        {
            m_Next.tv_sec += m_Next.tv_usec / 1000000;
            m_Next.tv_usec %= 1000000;
        }
        if  ((t->tv_sec > m_Next.tv_sec) || (t->tv_sec == m_Next.tv_sec && t->tv_usec >= m_Next.tv_usec))
        {
            m_Next = *t; // Some timer ticks will be skipped
        }
        OnTimer();
    }
}

int CMaaSocketTimer::GetWaitForTime(int us, timeval *t)
{
    if  (!m_bActive)
    {
        return us;
    }
    timeval tv;
    if  (!t)
    {
        gettimeofday(t = &tv, nullptr);
    }
    _qword q = (_qword)(m_Next.tv_sec - t->tv_sec) * 1000000 + (m_Next.tv_usec - t->tv_usec);
    //_qword max_dword = (((1 << 30) - 1) << 1) + 1;
    static constexpr _qword max_dword = ((1 << 30) - 1);
    const int u = (int)(q < max_dword ? q : max_dword);
    return us <= u ? us : u < 0 ? 0 : u;
}
#else
void CMaaSocketTimer::PreTimer(DWORD * t)
{
    if  (!m_bActive)
    {
        return;
    }
    DWORD tv;
    if  (!t)
    {
        t = &tv;
        *t = GetTickCount();
    }
    if  ((int)(*t - m_Next) >= 0)
    {
        m_Next += (_dword)(m_PeriodUs / 1000);
        if  ((int)(*t - m_Next) >= 0)
        {
            m_Next = *t; // Some timer ticks will be skipped
        }
        OnTimer();
    }
}

int CMaaSocketTimer::GetWaitForTime(int us, DWORD *t) noexcept
{
    if  (!m_bActive)
    {
        return us;
    }
    DWORD tv;
    if  (!t)
    {
        t = &tv;
        tv = GetTickCount();
    }
    _qword q = (_qword)(m_Next - *t) * 1000;
    //_qword max_dword = (((1 << 30) - 1) << 1) + 1;
    static constexpr _qword max_dword = ((1 << 30) - 1);
    const int u = (int)(q < max_dword ? q : max_dword);
    return us <= u ? us : u < 0 ? 0 : u;
}
#endif

#else

void CMaaSocketTimer::PreTimer(_qword t)
{
    if  (!m_bActive)
    {
        return;
    }
    /*
     _qword tv;
     if   (!t)
     {
          t = &tv;
          *t = gpSockStartup ? gpSockStartup->GetTime() : 0;
     }
     */
    if  (m_pFdSockets)
    {
        m_pFdSockets->m_TimerMutex.LockM();
    }
    if  ((_qword)(t - m_Next) >= 0)
    {
        m_Next += m_PeriodUs;
        if  ((_qword)(t - m_Next) >= 0)
        {
            m_Next = t + 1; // Some timer ticks have been skipped
        }
        if  (m_pFdSockets && gpSockStartup)
        {
            //gpSockStartup->m_TimerMutex.LockM();
            if  (m_Next != -1)
            {
                if (m_Handle)
                {
                    m_pFdSockets->SetKeyByHandle(m_Handle, m_Next);
                }
                else
                {
                    m_Handle = m_pFdSockets->HeapAdd(m_Next, this);
                    if (!m_Handle)
                    {
                        m_bActive = false;
                    }
                }
            }
            else
            {
                m_pFdSockets->HeapDel(m_Handle);  m_Handle = nullptr;
                m_bActive = false;
            }
            //gpSockStartup->m_TimerMutex.UnLockM();
        }
        if  (m_pFdSockets)
        {
            m_pFdSockets->m_TimerMutex.UnLockM();
        }
        OnTimer();
        return;
    }
    if  (m_pFdSockets)
    {
        m_pFdSockets->m_TimerMutex.UnLockM();
    }
}

int CMaaSocketTimer::GetWaitForTime(int us, _qword t) const noexcept
{
    if  (!m_bActive)
    {
        return us;
    }
    /*
    _qword tv;
    if   (!t)
    {
        t = &tv;
        tv = gpSockStartup ? gpSockStartup->GetTime() : 0;
    }
    */
    _qword q;
    if  (m_pFdSockets)
    {
        m_pFdSockets->m_TimerMutex.LockM();
        q = (_qword)(m_Next - t);
        m_pFdSockets->m_TimerMutex.UnLockM();
    }
    else
    {
        q = (_qword)(m_Next - t);
    }
    //_qword max_dword = (((1 << 30) - 1) << 1) + 1;
    static constexpr _qword max_dword = ((1 << 30) - 1);
    const int u = (int)(q < max_dword ? q : max_dword);
    return us <= u ? us : u < 0 ? 0 : u;
}
#endif

void CMaaSocketTimer::OnTimer()
{
}

//---------------------------------------------------------------------------

/*
void CMaaFdSockets::Attach(CMaaSocketTimer * t)
{
    t->Attach(this);
}

void CMaaFdSockets::Detach(CMaaSocketTimer * t)
{
    t->Detach();
}
*/

void CMaaFdSockets::PreTimer() noexcept
{
#ifndef __SOCK_NEW_TIMERS
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, nullptr);
#else // __unix__
    DWORD tv = GetTickCount();
#endif
    CMaaSocketTimer * t, * n;
    for (t = m_Timers.LookAtFront(); t; t = m_Timers.Next(t))
    {
        t->m_TmpFlg = false;
    }
    bool bRepeat = true;
    while(bRepeat)
    {
        bRepeat = false;
        for (t = m_Timers.LookAtFront(); t; t = n)
        {
            n = m_Timers.Next(t);
            if  (!t->m_TmpFlg)
            {
                t->m_TmpFlg = true;
                try
                {
                    t->PreTimer(&tv);
                }
                catch(...)
                {
                    n = nullptr;
                    bRepeat = true;
                }
            }
        }
    }
#else
    if  (!gpSockStartup)
    {
        return;
    }
    const _qword tv = GetTime() + 1;
    CMaaSocketTimer * t;
    m_TimerMutex.LockM();
    bool bHandled = false;
    while(1)
    {
        _qword tx;
        if  (!m_TimerHeap.Look(&tx, &t) || tx > tv)
        {
            break;
        }
        m_TimerHeap.Del();
        t->ResetHandle();
        if  (t->IsStarted())
        {
            bHandled = true;
            m_TimerMutex.UnLockM();
            try
            {
                t->PreTimer(tv);
            }
            catch(...)
            {
            }
            m_TimerMutex.LockM();
        }
    }
    m_TimerMutex.UnLockM();
    if (bHandled)
    {
#if 0
        CMaaDLink_pItem<CMaaSocket> * p = m_SockList.LookAtFront();
        while(p)
        {
            for (p = m_SockList.LookAtFront(); p; p = m_SockList.Next(p))
            {
                CMaaSocket* pSock = p->m_pItem;
                pSock->Notify_Act();
                if (m_SockHash.Find(pSock))
                {
                    break;
                }
            }
        }
#else
        CMaaDLink_pItem<CMaaFdSocket>* p;
        while ((p = m_ModeChangeList.GetFromFront()))
        {
            CMaaSocket* pSock = (CMaaSocket*)p->m_pItem;
            pSock->Notify_Act();
        }
#endif
    }
#endif
}

int CMaaFdSockets::GetWaitForTime(int us) noexcept
{
#ifndef __SOCK_NEW_TIMERS
#ifdef __unix__
    timeval tv;
    gettimeofday(&tv, nullptr);
#else
    DWORD tv = GetTickCount();
#endif
    for (CMaaSocketTimer * t = m_Timers.LookAtFront(); t; t = m_Timers.Next(t))
    {
        us = t->GetWaitForTime(us, &tv);
    }
#else
    // __utf8_printf("us: %,D\n", (_qword)*us);
    DP(printf("us: %D\n", (_qword)us);)
    if  (!gpSockStartup)
    {
        return us;
    }
    const _qword tv = GetTime();
    CMaaSocketTimer * t;
    m_TimerMutex.LockM();
    while(1)
    {
        _qword tx;
        if  (!m_TimerHeap.Look(&tx, &t))
        {
            t = nullptr;
            break;
        }
        if  (!t->IsStarted())
        {
            m_TimerHeap.Del();
            t->ResetHandle();
        }
        else
        {
            break;
        }
    }
    m_TimerMutex.UnLockM();
    if  (t)
    {
        int us2 = us;
#ifdef _WIN32
        if  ((us2 -= 998) < 0) // 1 ms round next
        {
            us2 = 0;
        }
#endif
        us2 = t->GetWaitForTime(us2, tv);
#ifdef _WIN32
        us2 += 998; // 1 ms (-1 us) round next
#endif
        // printf("us2 < us: %,D < %,D ?\n", (_qword)us2, (_qword)*us);
        us = us2 < us ? us2 : us;
    }
#endif
    return us;
}

void CMaaSockThread::Process()
{
    LOG1_(99);
    while(Process1())
    {
        LOG1_(100);
    }
}

void CMaaSockThread::OnExitCommand(SCmd *) noexcept
{
    m_bExiting = true;
    m_pFdSockets->DeleteAllSockets();
}

bool CMaaSockThread::Process1()
{
    for (;;)
    {
        CMaaWin32Locker_Locker_gLock;
        for (;;)
        {
            LOG1_(101);
            //Locker_Lock;
            m_Mutex.LockM();
            SCmd * c = m_List.GetFromFront();
            m_Mutex.UnLockM();
            //Locker_UnLock;
            //printf("Got command: %p\r\n", c); fflush(stdout);

            if  (!c)
            {
                break;
            }
            //printf("Got command: %d, %p, %p\r\n", c->Cmd, c->m_Par1, c->m_Par2); fflush(stdout);

            switch(c->Cmd)
            {
            case 1:
                //printf("c->Cmd == 1 - Join()\r\n");fflush(stdout);//P
                c->m_Par1->Join();
                //printf("c->Cmd == 1 - Join() - ok\r\n");fflush(stdout);//P
                delete c->m_Par1;
                //printf("c->Cmd == 1 - delete - ok\r\n");fflush(stdout);//P
                //delete c;
                //c = nullptr;
                break;
            case -1:
                //printf("OnExitCommand();\n");
                OnExitCommand(c);
                //delete c;
                //c = nullptr;
            }
            delete c;
        }

        LOG1_(102);
        if  (!m_pParent)
        {
            //printf("000: point\r\n");fflush(stdout);//P
        }
        else
        {
            //printf("111: point\r\n");fflush(stdout);//P
        }

        if  (m_ChildThreads.LookAtFront() && m_pFdSockets->GetSockCount() <= 0)
        {
            //Log("if (m_ChildThreads.LookAtFront() && m_pFdSockets->GetSockCount() <= 0) { }");
        }
        Locker_Lock;
        if  (m_Refs <= 0 && !m_ChildThreads.LookAtFront())
        {
            Locker_UnLock;
            if  (m_bExiting)
            {
                //printf("Point n2\n");
                break;
            }
            //printf("Point n2.5\n");
#ifdef __unix__
            if  (m_pFdSockets->GetSockCount() <= 1)
#else
            if  (m_pFdSockets->GetSockCount() <= 0)
#endif
            {
                //printf("Point n3\n");
                //if (m_pParent)
                {
                    break;
                }
            }
            //if (m_pParent || gSockThreads <= 1)
            {
                CheckPoint(0);
            }
            /*
            if (bExitSockThread)
            {
                if (m_pParent || gSockThreads <= 1)
                {
                    break;
                }
            }
            */
        }
        else
        {
            Locker_UnLock;
        }
        //WDTimerFlags[2] = 1;
        LOG1_(105);
        if  (m_pFdSockets->Select() < 0)
        {
            //2022
            m_Mutex.LockM();
            const bool b_c = m_List.LookAtFront() ? true : false;
            m_Mutex.UnLockM();
            Locker_Lock;
            const bool b_t = m_ChildThreads.LookAtFront() ? true : false;
            Locker_UnLock;
            if  (b_c)
            {
                return true;
            }
            if  (b_t)
            {
#ifdef _WIN32
                if  (!SwitchToThread())
                {
                    Sleep(1);
                }
#else
                if  (sched_yield())
                {
                    usleep(1000);
                }
#endif
                return true;
            }
            CMaaSocket::StaticError("Select()");
            //Console.Error("select()");
            break;
        }
        LOG1_(106);
        m_pFdSockets->PreTimer();
        LOG1_(107);
        return true;
    }
    //printf("Point n5\n");
    LOG1_(108);
    return false;
}
//---------------------------------------------------------------------------
#ifdef __unix__
//bool CMaaSocketPair(CMaaSocket &s1, CMaaSocket &s2);
CMaaWakeUpPair::CMaaWakeUpPair(CMaaFdSockets *pFdSockets)
:   TL_WA_PAIR_SK(pFdSockets),
    m_s2(pFdSockets),
    m_Bytes(0)
{
    //RemoveFdSocket();
    //m_s2.RemoveFdSocket();
    CMaaSocketPair(*this, m_s2);
    //FILE * f = fopen("TEST.txt", "a+b");
    //DP1(printf("CMaaWakeUpPair: %d %d\n", GetSocket(), m_s2.GetSocket());)
    try
    {
        SetNonBlockingMode();
        //      fprintf(f, "CMaaWakeUpPair::CMaaWakeUpPair(): SetNonBlockingMode() - ok\n");
    }
    catch(XTOOSockErr e)
    {
        //      fprintf(f, "CMaaWakeUpPair::CMaaWakeUpPair(): catch %s\n", (const char *)e.GetMsg());
        e.Eat();
    }
    catch(...)
    {
        //      fprintf(f, "CMaaWakeUpPair::CMaaWakeUpPair(): catch(...)\n");
    }
    try
    {
#ifdef TL_EPOLL_SOCKETPAIR
        // defined: TL_WA_PAIR_SK CMaaTcpSocket
        //m_s2.SetNonBlockingMode(); // max ~277 successive wakeups until block if tcp socket is nonblocking !!!2020!!!
#endif
    }
    catch(XTOOSockErr e)
    {
        e.Eat();
    }
    catch(...)
    {
    }
    //fclose(f);
    AddFdSocket();
    ChangeFdModeEx(eRead|eExcept|eDisableWrite);
}

CMaaWakeUpPair::~CMaaWakeUpPair()
{
#ifdef __unix__
    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    m_pFdSockets->m_pThread->m_WakeupPair = nullptr;
    Locker_UnLock;
#endif
    //printf("CMaaWakeUpPair::~CMaaWakeUpPair()\r\n");
    //fflush(stdout);
    //abort();
}

/*
void CMaaFdSocket::ResetEvent(int ev)
{
    if  (m_pFdSockets)
    {
        m_pFdSockets->ResetEvent(ev);
    }
}
*/

//static CMaaMutex sWakeUpMutex;
//#define sWakeUpMutex gLock
int CMaaWakeUpPair::Notify_Read()
{
    char Buffer[10*1024];
#ifndef TL_EPOLL_SOCKETPAIR
    m_WakeUpMutex.LockM();
    bool bLocked = true;
#else
    bool bLocked = false;
#endif
    try
    {
        if  (m_Bytes.load() > 0 || 1)
        {
#ifdef TL_EPOLL_SOCKETPAIR
            int n = RecvData(Buffer, sizeof(Buffer));
            //int n = recv(m_Socket, Buffer, sizeof(Buffer), 0);
            //int n = read(m_Socket, Buffer, sizeof(Buffer));
            m_WakeUpMutex.LockM();
            bLocked = true;
#else
            //          socklen_t len = sizeof(m_t.m_sin);
            //          sockadd_in m_t.m_sin;
            //          memset(&m_t.m_sin, 0, sizeof(m_t.m_sin));
            //          int n = recvfrom(m_Socket, Buffer, sizeof(BufferSize), 0, (struct sockaddr *)&m_t.m_sin, &len);

            int n = recv(m_Socket, Buffer, sizeof(Buffer), 0);
            //__utf8_printf("CMaaWakeUpPair::Notify_Read(): m_Bytes = %d, n = %d\n", m_Bytes, n);
#endif
            if  (n > 0)
            {
                if  ((m_Bytes -= n) < 0)
                {
                    m_Bytes = 0;
                }
            }
#ifdef TL_EPOLLmmmm
            if  (n < 0 && errno == EAGAIN)
            {
                int x = m_RealState;
                {
                    x &= ~eRead;
                    m_RealState = x;
                    x &= GetFdMode() & eAll;
                    if (!x)
                    {
                        //m_pFdSockets->m_hActiveState.Remove(m_Socket);
                        if (m_ActiveStateListItem.Next)
                        {
                            m_pFdSockets->m_ActiveStateList.Release(&m_ActiveStateListItem);
                        }
                    }
                    else
                    {
                        //m_pFdSockets->m_hActiveState.AddOver(m_Socket, x);
                        m_ActiveState = x;
                        if (!m_ActiveStateListItem.Next)
                        {
                            m_pFdSockets->m_ActiveStateList.AddAtBack(&m_ActiveStateListItem);
                        }
                    }
                }
            }
            int Mode;
            //if   (n > 0 && !m_pFdSockets->m_SockHash.Find(this, &Mode))
            //if  (n > 0 && !m_pFdSockets->SockHashFind(this, &Mode))
            if (n > 0)
            {
                Mode = m_pFdSockets->SockHashFindMode(this);
                //char c = 0;
                //printf("recv(%d, len=1, MSG_PEEK)...\n", m_Socket); fflush(stdout);
                //int l = read(m_Socket, &c, 1);
                //if   (n == sizeof(Buffer)) //l >= 0)
                {
                    Mode |= eEventRead;
                }
                //else
                {
                    //Mode &= ~eEventRead;
                }
                //m_pFdSockets->m_SockHash.AddOver(this, Mode);
                m_pFdSockets->SockHashAddOver(this, Mode);
            }
#endif
            //RecvData(Buffer, sizeof(Buffer));
        }
        if (bLocked)
        {
            m_WakeUpMutex.UnLockM();
            //bLocked = false;
        }
#ifndef TL_EPOLL
        ChangeFdMode(eDisableEventRead);
#endif
    }
    catch(XTOOSockErr &e)
    {
        if  (bLocked)
        {
            m_WakeUpMutex.UnLockM();
        }
#ifndef TL_EPOLL
        ChangeFdMode(eDisableEventRead);
#endif
        //Trace("CMaaWakeUpPair::Notify_Read(): catch(XTOOSockErr &e), errno=%d", errno);
        CMaaWin32Locker_Locker_gLock;
        Locker_Lock;
        delete this;
        Locker_UnLock;
        e.Eat();
        throw;
    }
    catch(...)
    {
        if  (bLocked)
        {
            m_WakeUpMutex.UnLockM();
        }
#ifndef TL_EPOLL
        ChangeFdMode(eDisableEventRead);
#endif
        //Trace("CMaaWakeUpPair::Notify_Read(): catch(...)", 0);
        throw;
    }
    return 0;
}

int CMaaWakeUpPair::Notify_Write()
{
    return eDisableWrite;
}

//int Notify_Error ();

CMaaString CMaaWakeUpPair::GetConnectionName() noexcept {return CMaaString("CMaaWakeUpPair", CMaaString::eROStrlenMemString); }

void CMaaWakeUpPair::WakeUp() noexcept
{
    char c = '1';
    m_WakeUpMutex.LockM();
    if  (m_Bytes.load() < 100) // max ~277 Wakeups until block !!!2020!!!
    {
        m_Bytes++;
        try
        {
            int x = send(m_s2.m_Socket, &c, 1, 0);
            //int x = write(m_s2.m_Socket, &c, 1);
            if  (x <= 0)
            {
                m_Bytes--;
            }
            m_WakeUpMutex.UnLockM();
        }
        catch(XTOOSockErr &e)
        {
            m_WakeUpMutex.UnLockM();
            //printf("catch(XTOOSockErr &e) in CMaaWakeUpPair::WakeUp()\r\n");fflush(stdout);
            e.Eat();
            // ignoring error
        }
        catch(...)
        {
            m_WakeUpMutex.UnLockM();
            //printf("catch(...) in CMaaWakeUpPair::WakeUp()\r\n"); fflush(stdout);
        }
    }
    else
    {
        m_WakeUpMutex.UnLockM();
    }
}
#endif

CMaaTcpSocket::CCloseTimer::CCloseTimer(int PeriodUs)
:   CMaaSocketTimer(PeriodUs),
    m_pSocket(nullptr)
{
}

CMaaTcpSocket::CCloseTimer::~CCloseTimer()
{
    //Stop();
    //Detach();
}

void CMaaTcpSocket::CCloseTimer::OnTimer()
{
    // only for windows implementation
    if  (m_pSocket)
    {
        if  (m_pSocket->GetFdMode() & CMaaEnum::eHold)
        {
            return;
        }
        char c = 0;
        int l = recv(m_pSocket->GetSocket(), &c, 1, MSG_PEEK);
#ifdef _WIN32
        DWORD e = l < 0 ? WSAGetLastError() : 0;
#else
        int e = l < 0 ? errno : 0;
#endif
        if (GetPeriod() == 2)
        {
            Start2(2000, 0);
        }
        if  (l == 0) // 28.04.2024
        {
#ifdef _WIN32
            if (m_pSocket->GetFdMode() & eRead)
            {
                //Stop();
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Read());
            }
#if 0
            else
            {
                //Stop();
                WSASetLastError(CMaa_CONN_CLOSED_ERROR);
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Error());
            }
#endif
#else
            if  (m_pSocket->GetFdMode() & eRead)
            {
                //Stop();
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Read());
            }
#if 0
            else
            {
                //Stop();
                int ErrnoBackup = errno;
                errno = CMaa_CONN_CLOSED_ERROR;
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Error());
                errno = ErrnoBackup;
            }
#endif
#endif
            //ev.lNetworkEvents |= FD_READ;
            //ev.iErrorCode[FD_READ_BIT] = CMaa_CONN_CLOSED_ERROR;//WSAENOTCONN;
        }
        else if (l < 0)
        {
#ifdef _WIN32
            if  (e != WSAEWOULDBLOCK)
#else
            if  (e != EAGAIN)
#endif
            {
                Stop();
                //ev.lNetworkEvents |= FD_READ;
                //ev.iErrorCode[FD_READ_BIT] = x;
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Error());
            }
        }
        else
        {
            if  (m_pSocket->GetFdMode() & eRead)
            {
                m_pSocket->ChangeFdMode(m_pSocket->Notify_Read());
            }
        }
    }
    else
    {
        Detach();
    }
}

void CMaaTcpSocket::CCloseTimer::OnClose(CMaaTcpSocket * pSocket, bool bShortPeriod)
{
    m_pSocket = pSocket;
    Attach(pSocket->m_pFdSockets);
    Start1(2); // bShortPeriod ? 2000 : 2 /*200000/100*/); //pSocket->GetFdMode());
}

void CMaaFdSockets::WakeUp(unsigned char flags) noexcept
{
    if  (m_pThread)
    {
        m_pThread->WakeUp(flags);
    }
}

void CMaaFdSocket::WakeUp(unsigned char flags) noexcept
{
    if  (m_pFdSockets)
    {
        m_pFdSockets->WakeUp(flags);
    }
}

//---------------------------------------------------------------------------------
// IPv6 support for Windows
//---------------------------------------------------------------------------------
/*
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
*/

#ifdef _WIN32
/*
struct CMaaIpv6_1
{
     _byte a[16];
};
struct CMaaHe1
{
     CMaaIpv6_1 * h_addr_list[1];
};
*/
CMaaHe1 * CMaa_gethostbyname2(const char * name, CMaaString &buffer)
{
    buffer.Empty();
    //int iResult;
    //INT iRetval;

    DWORD dwRetval;

    int i = 1;

    struct addrinfo *result = nullptr;
    struct addrinfo *ptr = nullptr;
    struct addrinfo hints;

    //struct sockaddr_in  *sockaddr_ipv4;
    //    struct sockaddr_in6 *sockaddr_ipv6;
    struct sockaddr_in6 * p_sockaddr_ip6;
    //struct sockaddr_in6_old * p_sockaddr_ip6_old;
    /*
struct sockaddr_in6 {
        short   sin6_family;
        u_short sin6_port;
        u_long  sin6_flowinfo;
        struct  in6_addr sin6_addr;
        u_long  sin6_scope_id;
};

typedef struct sockaddr_in6 SOCKADDR_IN6;
typedef struct sockaddr_in6 *PSOCKADDR_IN6;
typedef struct sockaddr_in6 FAR *LPSOCKADDR_IN6;


struct sockaddr_in6_old {
        short   sin6_family;        
        u_short sin6_port;          
        u_long  sin6_flowinfo;      
        struct  in6_addr sin6_addr;  
};
*/

    //char ipstringbuffer[46];
    //DWORD ipbufferlength = 46;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;//AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //--------------------------------
    // Call getaddrinfo(). If the call succeeds,
    // the result variable will hold a linked list
    // of addrinfo structures containing response
    // information
    dwRetval = getaddrinfo(name, "HTTP", &hints, &result);
    if  (dwRetval != 0)
    {
        return nullptr;
    }

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != nullptr ; ptr=ptr->ai_next)
    {
        //printf("\tFlags: 0x%x\n", ptr->ai_flags);
        switch(ptr->ai_family)
        {
            /*
            case AF_UNSPEC:
                printf("Unspecified\n");
                break;
            case AF_INET:
                printf("AF_INET (IPv4)\n");
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
                printf("\tIPv4 address %s\n",
                    inet_ntoa(sockaddr_ipv4->sin_addr) );
                break;
            */
        case AF_INET6:
            //printf("AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n",
            //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            p_sockaddr_ip6 = (LPSOCKADDR_IN6) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            /*
                ipbufferlength = 46;
                iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, nullptr, 
                    ipstringbuffer, &ipbufferlength );
                if (iRetval)
                    printf("WSAAddressToString failed with %u\n", WSAGetLastError() );
                else    
                    printf("\tIPv6 address %s\n", ipstringbuffer);
                */
            CMaaIpv6_1 ip_1;
            memset(&ip_1, 0, sizeof(ip_1));
            memcpy(ip_1.a, p_sockaddr_ip6->sin6_addr.u.Byte, sizeof(ip_1.a) < sizeof(p_sockaddr_ip6->sin6_addr) ? sizeof(ip_1.a) : sizeof(p_sockaddr_ip6->sin6_addr)); // 16
            buffer += CMaaString(&ip_1, (int)sizeof(ip_1));
            break;
            /*
            case AF_NETBIOS:
                printf("AF_NETBIOS (NetBIOS)\n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_family);
                break;
            */
        }
        /*
        printf("\tSocket type: ");
        switch (ptr->ai_socktype) {
            case 0:
                printf("Unspecified\n");
                break;
            case SOCK_STREAM:
                printf("SOCK_STREAM (stream)\n");
                break;
            case SOCK_DGRAM:
                printf("SOCK_DGRAM (datagram) \n");
                break;
            case SOCK_RAW:
                printf("SOCK_RAW (raw) \n");
                break;
            case SOCK_RDM:
                printf("SOCK_RDM (reliable message datagram)\n");
                break;
            case SOCK_SEQPACKET:
                printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_socktype);
                break;
        }
        printf("\tProtocol: ");
        switch (ptr->ai_protocol) {
            case 0:
                printf("Unspecified\n");
                break;
            case IPPROTO_TCP:
                printf("IPPROTO_TCP (TCP)\n");
                break;
            case IPPROTO_UDP:
                printf("IPPROTO_UDP (UDP) \n");
                break;
            default:
                printf("Other %ld\n", ptr->ai_protocol);
                break;
        }
        printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
        */
    }

    freeaddrinfo(result);

    const int N = (warning_int)(buffer.Length() / (int)sizeof(CMaaIpv6_1));
    int L0 = (warning_int)buffer.Length();
    const int L = (int)(buffer.Length() + sizeof(CMaaHe1) + N * sizeof(CMaaIpv6_1 *));
    buffer += CMaaString(nullptr, (int)(sizeof(CMaaHe1) + N * sizeof(CMaaIpv6_1 *)));
    if  (!L0 || buffer.Length() < L)
    {
        return nullptr;
    }
    CMaaHe1 * pRet = (CMaaHe1 *)(L0 + (const char *)buffer);
    for (i = 0; i < N; i++)
    {
        pRet->h_addr_list[i] = i + (CMaaIpv6_1 *)(const char *)buffer;
    }
    pRet->h_addr_list[i] = nullptr;
    return pRet;
}
#endif // #ifndef TOOLSLIB_SINGLE_THREAD
#if 0
int __cdecl main(int argc, char **argv)
{

    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;
    INT iRetval;

    DWORD dwRetval;

    int i = 1;

    struct addrinfo *result = nullptr;
    struct addrinfo *ptr = nullptr;
    struct addrinfo hints;

    struct sockaddr_in  *sockaddr_ipv4;
    //    struct sockaddr_in6 *sockaddr_ipv6;
    LPSOCKADDR sockaddr_ip;

    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;

    // Validate the parameters
    if  (argc != 3) {
        printf("usage: %s <hostname> <servicename>\n", argv[0]);
        printf("getaddrinfo provides protocol-independent translation\n");
        printf("   from an ANSI host name to an IP address\n");
        printf("%s example usage\n", argv[0]);
        printf("   %s www.contoso.com 0\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if  (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    //--------------------------------
    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    printf("Calling getaddrinfo with following parameters:\n");
    printf("\tnodename = %s\n", argv[1]);
    printf("\tservname (or port) = %s\n\n", argv[2]);

    //--------------------------------
    // Call getaddrinfo(). If the call succeeds,
    // the result variable will hold a linked list
    // of addrinfo structures containing response
    // information
    dwRetval = getaddrinfo(argv[1], argv[2], &hints, &result);
    if  ( dwRetval != 0 ) {
        printf("getaddrinfo failed with error: %d\n", dwRetval);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo returned success\n");

    // Retrieve each address and print out the hex bytes
    for (ptr=result; ptr != nullptr ;ptr=ptr->ai_next) {

        printf("getaddrinfo response %d\n", i++);
        printf("\tFlags: 0x%x\n", ptr->ai_flags);
        printf("\tFamily: ");
        switch(ptr->ai_family) {
        case AF_UNSPEC:
            printf("Unspecified\n");
            break;
        case AF_INET:
            printf("AF_INET (IPv4)\n");
            sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
            printf("\tIPv4 address %s\n",
                inet_ntoa(sockaddr_ipv4->sin_addr) );
            break;
        case AF_INET6:
            printf("AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n",
            //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            iRetval = WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, nullptr,
                ipstringbuffer, &ipbufferlength );
            if  (iRetval)
            printf("WSAAddressToString failed with %u\n", WSAGetLastError() );
            else
            printf("\tIPv6 address %s\n", ipstringbuffer);
            break;
        case AF_NETBIOS:
            printf("AF_NETBIOS (NetBIOS)\n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_family);
            break;
        }
        printf("\tSocket type: ");
        switch(ptr->ai_socktype) {
        case 0:
            printf("Unspecified\n");
            break;
        case SOCK_STREAM:
            printf("SOCK_STREAM (stream)\n");
            break;
        case SOCK_DGRAM:
            printf("SOCK_DGRAM (datagram) \n");
            break;
        case SOCK_RAW:
            printf("SOCK_RAW (raw) \n");
            break;
        case SOCK_RDM:
            printf("SOCK_RDM (reliable message datagram)\n");
            break;
        case SOCK_SEQPACKET:
            printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_socktype);
            break;
        }
        printf("\tProtocol: ");
        switch(ptr->ai_protocol) {
        case 0:
            printf("Unspecified\n");
            break;
        case IPPROTO_TCP:
            printf("IPPROTO_TCP (TCP)\n");
            break;
        case IPPROTO_UDP:
            printf("IPPROTO_UDP (UDP) \n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_protocol);
            break;
        }
        printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
        printf("\tCanonical name: %s\n", ptr->ai_canonname);
    }

    freeaddrinfo(result);
    WSACleanup();

    return 0;
}
#endif
#endif
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
// Shapers
//---------------------------------------------------------------------------------
#ifndef TOOLSLIB_SINGLE_THREAD
#ifdef __SHAPERS
//--------------------------------------------------------------------------
int CMaaTcpLLShaper::GetLenReserved(int Len) 
#ifdef _WIN32
const
#endif
 noexcept
{
    const int Len0 = Len;
    if  (m_MaxSpeedLimit >= 0 && Len > m_MaxSpeedLimit - m_V)
    {
        Len = m_MaxSpeedLimit - m_V;
    }
    const int V0 = 1;
    if  (V0 > m_Vsrd)
    {
        if  (Len > V0 - m_V)
        {
            Len = V0 - m_V;
        }
    }
    else
    {
        if  (Len > m_Vsrd - m_V)
        {
            Len = m_Vsrd - m_V;
        }
    }
    int Len2 = Len;
    if  (m_Shaper)
    {
        Len2 = m_Shaper->GetLenReserved(Len);
    }

    // test
    // not strict shaper limits support (gStrictTcpShaperLimits <= 0, m_Vsr >= 0)
    if  (Len2 < m_Vsr - m_V)
    {
        Len2 = m_Vsr - m_V;
    }
    /*
     if   (Len2 > m_Vsr - m_V && m_V == 0)
     {
          Len2 = m_Vsr - m_V;
     }
     */
    /*
     if   (Len2 < m_Vsrd - m_V)
     {
          Len2 = m_Vsrd - m_V;
     }
     */
    if  (Len < Len2)
    {
        Len2 = Len;
    }
    Len2 = Len2 >= 0 ? Len2 : 0;
    if  (Len0 > 0)
    {
        if  (Len2 > 0)
        {
#ifdef _WIN32
            //SetEvent(m_hEv);
            WSASetEvent(m_hEv);
#else
            m_hEv = true;
            /*
               if (m_pSocket)
               {
                   m_pSocket->WakeUp();
               }
            */
#endif
        }
        else
        {
#ifdef _WIN32
            //ResetEvent(m_hEv);
            WSAResetEvent(m_hEv);
#else
            m_hEv = false;
#endif
        }
    }
    return Len2;
}
//--------------------------------------------------------------------------
void CMaaTcpLLShaper::SubLenReserved(int Len)
{
    m_V += Len;
    if  (m_Shaper)
    {
        m_Shaper->SubLenReserved(Len);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpLLShaper::OnTimer(int i)
{
    //printf("CMaaTcpLLShaper::OnTimer(%d)\n", i); fflush(stdout); //111111111
    m_VTempSum += m_V;
    m_V = 0;
    if  (i)
    {
        m_VAverageLast = m_VTempSum;
        m_VTempSum = 0;
        if  (m_Shaper && m_pSocket && m_Shaper->m_Conditions.LookAtFront() && m_pSocket->m_ShaperConditionLastCheckTime != i)
        {
            m_pSocket->m_ShaperConditionLastCheckTime = i;
            m_pSocket->CheckConditions(m_Shaper);//, m_Shaper->m_Conditions);
        }
    }
}

void CMaaTcpLLShaper::SetConnectionLimits(int MaxSpeed, int bHardLimit) noexcept
{
    if  (m_bHardLimit && bHardLimit < 0)
    {
        return;// false;
    }
    if  (MaxSpeed == -2)
    {
        return;
    }
    if  (MaxSpeed < 0)
    {
        MaxSpeed = -1;
    }
    else if ((MaxSpeed /= _SHAPER_TICKS_PER_SECOND) < 1)
    {
        MaxSpeed = 1;
    }
    m_MaxSpeedLimit = MaxSpeed;
    if  (bHardLimit >= 0)
    {
        m_bHardLimit = bHardLimit > 0;
    }
}
//--------------------------------------------------------------------------
CMaaTcpShaper::CMaaTcpShaper(int Type, const CMaaString &Name, double W, int MaxSpeed, int UserId)
{
    Init(Type, Name, W, MaxSpeed, UserId);
}
//--------------------------------------------------------------------------
CMaaTcpShaper::CMaaTcpShaper(const CMaaString &GeneralShaperName, double W, int MaxSpeed) // Type = eGeneral
{
    Init(eGeneral, GeneralShaperName, W, MaxSpeed);
}
//--------------------------------------------------------------------------
CMaaTcpShaper::CMaaTcpShaper(const CMaaString &UserName, int UserId, double W, int MaxSpeed) // Type = eUserSubshaper
{
    Init(eUserSubshaper, UserName, W, MaxSpeed, UserId);
}
//--------------------------------------------------------------------------
/*
CMaaTcpShaper::CMaaTcpShaper(double W, int MaxSpeed) //  // Type = AutoUserSubShaper
{
     Init(AutoUserSubShaper, gsCMaaStringZ, W, MasSpeed, -2);
}
*/
//--------------------------------------------------------------------------
CMaaTcpShaper::~CMaaTcpShaper()
{
    if  (m_Shaper)
    {
        m_Shaper->Del(*this);
    }
    DeleteConnectionInTimeCharacteristics();
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SetConnectionLimits(int MaxSpeed, bool bRecursive) noexcept
{
    if  (MaxSpeed == -2)
    {
        return;
    }
    if  (m_ChildsLL.LookAtFront())
    {
        int max_speed_limit;
        if  (MaxSpeed < 0)
        {
            max_speed_limit = -1;
        }
        else
        {
            if  ((max_speed_limit = (MaxSpeed /= _SHAPER_TICKS_PER_SECOND)) < 1)
            {
                max_speed_limit = 1;
            }
        }
        for (CMaaTcpLLShaper * p = m_ChildsLL.LookAtFront(); p; p = m_ChildsLL.Next(p))
        {
            if  (!p->m_bHardLimit)
            {
                p->m_MaxSpeedLimit = max_speed_limit;
            }
        }
    }
    if  (bRecursive)
    {
        for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            l->p->SetConnectionLimits(MaxSpeed, true);
        }
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SetConditions(CMaaDListAD<Condition> &List, bool bAllowToMoveAllListData)
{
    m_Conditions.RemoveAll();
    if  (bAllowToMoveAllListData)
    {
        m_Conditions.MoveThatToTheFront(List);
    }
    else
    {
        CopyList(m_Conditions, List);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::CopyList(CMaaDList<Condition> &dst, const CMaaDList<Condition> &src)
{
    for (Condition * p = src.LookAtFront(); p; p = src.Next(p))
    {
        Condition * n = p->Copy();
        if  (!n)
        {
            break;
        }
        dst.AddAtBack(n);
    }
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetNotEmptyChildrenShapersCount() const noexcept
{
    return m_ConnectionsCount[2];
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetConnectionsCount(bool bRecursive) const noexcept
{
    return m_ConnectionsCount[bRecursive ? 1 : 0];
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::Init(int Type, const CMaaString &Name, double W, int MaxSpeed, int UserId)
{
    memset(m_pConnectionInTimeCharacteristics, 0, sizeof(m_pConnectionInTimeCharacteristics));
    memset(m_ConnectionsCount, 0, sizeof(m_ConnectionsCount));

    m_L.p = this;

    m_Type = Type;
    m_Name = Name;
    if  (MaxSpeed < 0)
    {
        MaxSpeed = 0x7fffffff;
    }
    m_MaxSpeed = MaxSpeed / _SHAPER_TICKS_PER_SECOND;
    if  (m_MaxSpeed < 1)
    {
        m_MaxSpeed = 1;
    }
    m_UserId = UserId;
    //m_MaxSpeed = 50000 / 5;
    //m_MaxSpeed = 100000 / 5;
    //m_MaxSpeed = 5 / 5;
    m_Bank = m_MaxSpeed;
    m_Shaper = nullptr;
    m_Weight = W;
    m_V0 = m_Vsr = m_Bank;
    m_V = m_VTempSum = m_VAverageLast = 0;
    m_fVsr = (double)m_Vsr;
    if  (Type == eGeneral)
    {
        CreateConnectionInTimeCharacteristics();
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::DeleteConnectionInTimeCharacteristics() noexcept
{
    for (int j = 0; j < 4; j++)
    {
        delete m_pConnectionInTimeCharacteristics[j];
        m_pConnectionInTimeCharacteristics[j] = nullptr;
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::CreateConnectionInTimeCharacteristics(int s, int m, int h, int d)
{
    DeleteConnectionInTimeCharacteristics();
    //int i;
    //for  (i = 0; i < 2; i++)
    {
        _qword InitialValue = 0;//!(i & 1) ? m_BytesSend : m_BytesRecv;
        m_pConnectionInTimeCharacteristics[0] = TL_NEW CMaaConnectionInTimeCharacteristics(s, InitialValue, 1);
        m_pConnectionInTimeCharacteristics[1] = TL_NEW CMaaConnectionInTimeCharacteristics(m, InitialValue, 60);
        m_pConnectionInTimeCharacteristics[2] = TL_NEW CMaaConnectionInTimeCharacteristics(h, InitialValue, 3600);
        m_pConnectionInTimeCharacteristics[3] = TL_NEW CMaaConnectionInTimeCharacteristics(d, InitialValue, 24 * 3600);
    }
    int null_ptrs = 0;
    //for  (i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if  (!m_pConnectionInTimeCharacteristics[j])
            {
                null_ptrs++;
            }
        }
    }
    if  (null_ptrs)
    {
        DeleteConnectionInTimeCharacteristics();
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::ResetConnectionInTimeCharacteristics()
{
    if  (m_pConnectionInTimeCharacteristics[0])
    {
        //for  (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                const _qword value = 0;//!(i & 1) ? m_BytesSend : m_BytesRecv;
                m_pConnectionInTimeCharacteristics[j]->Reset(value, true);
            }
        }
    }
}
//--------------------------------------------------------------------------
CMaaString CMaaTcpShaper::GetDispName()
{
    switch(m_Type)
    {
    case eGeneral:
        return m_Name;
    case eUserSubshaper:
        return CMaaString::sFormat("u:%s(%d)", (const char *)m_Name, m_UserId);
    case eAutoUserSubshaper:
        return CMaaString::sFormat("a:%s(%d)", (const char *)m_Name, m_UserId);
    }
    return CMaaStringZ;
}
//--------------------------------------------------------------------------
CMaaString CMaaTcpShaper::GetShaperNameRawRecursiveLibName()
{
    CMaaString Name;
    {
        switch(m_Type)
        {
        case eGeneral:
            Name = m_Name;
            break;
        case eUserSubshaper:
            Name.Format("user(%d)", m_UserId);
            break;
        case eAutoUserSubshaper:
            //Name = "AutoUserSubshaper";
            Name.Format("auto(%d)", m_UserId);
            break;
        }
        if  (m_Shaper)
        {
            CMaaString ParentShaperName = m_Shaper->GetShaperNameRawRecursiveLibName();
            if  (ParentShaperName.IsNotEmpty())
            {
                Name = ParentShaperName + "." + Name;
            }
        }
    }
    return Name;
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetLenReserved(int Len) const noexcept
{
    const int Len2 = GetLenReserved2(Len);
    /*
     if   (Len2 < m_Vsr - m_V)
     {
          Len2 = m_Vsr - m_V;
     }
     if   (Len < Len2)
     {
          Len2 = Len;
     }
     */
    return Len2;
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::GetLenReserved2(int Len) const noexcept
{
    if  (m_Bank < Len)
    {
        Len = m_Bank;
    }
    // test
    if  (Len < 0)
    {
        Len = 0;
    }
    if  (m_Shaper)
    {
        Len = m_Shaper->GetLenReserved(Len);
    }
    // test
    if  (Len < 0)
    {
        Len = 0;
    }
    return Len;
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SubLenReserved(int Len)
{
    m_Bank -= Len;
    m_V += Len;
    if  (m_pConnectionInTimeCharacteristics[0])
    {
        const _qword value = Len;
        m_pConnectionInTimeCharacteristics[0]->Accumulate(value);
        //if   (m_pConnectionInTimeCharacteristics[0]->GetCurPos() % 60 == 0)
        {
            m_pConnectionInTimeCharacteristics[1]->Accumulate(value);
            //if   (m_pConnectionInTimeCharacteristics[1]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[2]->Accumulate(value);
                //if   (m_pConnectionInTimeCharacteristics[2]->GetCurPos() % 24 == 0)
                {
                    m_pConnectionInTimeCharacteristics[3]->Accumulate(value);
                }
            }
        }
    }
    if  (m_Shaper)
    {
        m_Shaper->SubLenReserved(Len);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::OnTimerUpdate() noexcept
{
    if  (m_Shaper && !m_Shaper->m_Shaper)
    {
        //printf("CMaaTcpShaper::OnTimerUpdate(): Update()\n"); fflush(stdout);
        Update();
    }
}
//--------------------------------------------------------------------------
const double dThreshold = 0.0001;
//--------------------------------------------------------------------------
void CMaaTcpShaper::Update() noexcept
{
    if  (m_Shaper && !m_Shaper->m_Shaper)
    {
        m_Shaper->m_Reserved = 0;
        m_Shaper->m_v0 = 0;//m_Shaper->m_v1 = m_Shaper->m_v2 = 0;
        m_Shaper->m_Max = 0x7fffffff;
        m_Shaper->m_bFullFilled = m_Shaper->m_bFinished = false;

        int Result = 0;
        RecCalc(0, Result); // Init
        m_Reserved = m_Max;

        int r;
        do
        {
            r = 0;
            do
            {
                Result = 0;
                RecCalc(1, Result); // Ðàñïðåäåëåíèå ñîãëàñíî âåñàì âåðøèí âåëè÷èíû m_Reserved, correct m_Reserver on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
                r |= Result;

            }    while(Result);

            Result = 0;
            RecCalc(2, Result); // Popup m_Reserved

        }    while(Result || (m_Reserved > dThreshold && r));

        Result = 0;
        RecCalc(3, Result); // Store m_v1 = m_v0 (== Vsr)

        Result = 0;
        RecCalc(4, Result); // Compare m_V == 0 --> add parent->m_Reserved, mark m_bFullFilled
        do
        {
            r = 0;
            do
            {
                Result = 0;
                RecCalc(1, Result); // Ðàñïðåäåëåíèå ñîãëàñíî âåñàì âåðøèí âåëè÷èíû m_Reserved, correct m_Reserver on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
                r |= Result;

            }    while(Result);

            Result = 0;
            RecCalc(2, Result); // Popup m_Reserved

        }    while(Result || (m_Reserved > dThreshold && r));

        Result = 0;
        RecCalc(5, Result); // Store m_v2 = m_v0 (== Vsr2)

        Result = 0;
        RecCalc(6, Result); // Compare m_V < m_v0 (m_Vsr2) --> add parent->m_Reserved, mark m_bFullFilled
        do
        {
            r = 0;
            do
            {
                Result = 0;
                RecCalc(1, Result); // Ðàñïðåäåëåíèå ñîãëàñíî âåñàì âåðøèí âåëè÷èíû m_Reserved, correct m_Reserver on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
                r |= Result;

            }    while(Result);

            Result = 0;
            RecCalc(2, Result); // Popup m_Reserved

        }    while(Result || (m_Reserved > dThreshold && r));

        Result = 0;
        RecCalc(7, Result); // GetResults (Vsr = m_v2, Vsrd = m_v0)
    }
}
//--------------------------------------------------------------------------
int gStrictTcpShaperLimits = 0; // -1 - no, 0 - default, 1 - yes

#ifdef __SHAPERS_PROFILING
static int s_depth = -1;
#endif

void CMaaTcpShaper::RecCalc_1(/*int Step,*/int& Result)
{
#ifdef __SHAPERS_PROFILING
    const int Step = -1;
    _qword t0;
    if  (++s_depth <= 1)
    {
        t0 = m_hrt.GetTime();
    }
    //iSockThread_pt = depth;
#endif
    L* l;
    //CMaaTcpLLShaper* x;
    //double tmp, ww, t;
    //switch(Step)
    {
        //case -1: // Update VAve
        if  (Result != 0 && m_pConnectionInTimeCharacteristics[0])
        {
            const _qword value = 0;
            m_pConnectionInTimeCharacteristics[0]->Accumulate(value, true);
            if  (m_pConnectionInTimeCharacteristics[0]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[1]->Accumulate(value, true);
                if  (m_pConnectionInTimeCharacteristics[1]->GetCurPos() % 60 == 0)
                {
                    m_pConnectionInTimeCharacteristics[2]->Accumulate(value, true);
                    if  (m_pConnectionInTimeCharacteristics[2]->GetCurPos() % 24 == 0)
                    {
                        m_pConnectionInTimeCharacteristics[3]->Accumulate(value, true);
                    }
                }
            }
        }
        OnTimer(Result);
        L* next; // is not very need to be in the currecnt implementation (10.01.2010)
        for (l = m_Childs.LookAtFront(); l; l = next)
        {
            next = m_Childs.Next(l);
            CMaaTcpShaper* p = l->p;
            p->RecCalc_1(Result);
        }
        //break;
    }
#ifdef __SHAPERS_PROFILING
    if  ((s_depth == 0 && Step == -1) || (s_depth == 1 && Step != -1))
    {
        _qword t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SRecCalc(%d, %d) = %,D\n", &gShaperTempInfo, Step, Result, t - t0);
        int index = Step >= 0 ? Step : 99;
        SockThread_pt[index] += t - t0;
    }
    --s_depth;
#endif
}

void CMaaTcpShaper::RecCalc(int Step, int &Result) noexcept
{
#ifdef __SHAPERS_PROFILING
    _qword t0;
    if  (++s_depth <= 1)
    {
        t0 = m_hrt.GetTime();
    }
    //iSockThread_pt = depth;
#endif
    L * l;
    CMaaTcpLLShaper * x;
    double tmp, ww, t;
    switch(Step)
    {
#if 0
    case -1: // Update VAve
        if  (Result != 0 && m_pConnectionInTimeCharacteristics[0])
        {
            const _qword value = 0;
            m_pConnectionInTimeCharacteristics[0]->Accumulate(value, true);
            if  (m_pConnectionInTimeCharacteristics[0]->GetCurPos() % 60 == 0)
            {
                m_pConnectionInTimeCharacteristics[1]->Accumulate(value, true);
                if  (m_pConnectionInTimeCharacteristics[1]->GetCurPos() % 60 == 0)
                {
                    m_pConnectionInTimeCharacteristics[2]->Accumulate(value, true);
                    if  (m_pConnectionInTimeCharacteristics[2]->GetCurPos() % 24 == 0)
                    {
                        m_pConnectionInTimeCharacteristics[3]->Accumulate(value, true);
                    }
                }
            }
        }
        OnTimer(Result);
        L * next; // is not very need to be in the currecnt implementation (10.01.2010)
        for (l = m_Childs.LookAtFront(); l; l = next)
        {
            next = m_Childs.Next(l);
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        break;
#endif
    case 0: // Init
        m_Reserved = 0;
        m_v0 = 0; //m_v1 = m_v2 = 0;
        m_Max = m_MaxSpeed > 0 ? (double)m_MaxSpeed : 0x7fffffff;
        m_bFullFilled = m_bFinished = false;
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            //x->RecCalc(Step, Result);
            x->m_Reserved = 0; //unused
            x->m_v0 = x->m_v1 = x->m_v2 = 0;
            x->m_Max = x->m_MaxSpeedLimit > 0 ? (double)x->m_MaxSpeedLimit : 0x7fffffff;
            x->m_bFullFilled = x->m_bFinished = false;
        }
        break;
    case 1: // Ðàñïðåäåëåíèå ñîãëàñíî âåñàì âåðøèí âåëè÷èíû m_Reserved, correct m_Reserved on fly for SL->m_V < SL->m_v0 | SL->m_v0 = SL->m_Max
        if  (m_Reserved > dThreshold)
        {
            ww = 0;
            for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
            {
                CMaaTcpShaper * p = l->p;
                if  (!p->m_bFullFilled)
                {
                    ww += p->m_Weight;
                }
                //p->RecCalc(Step, Result);
            }
            for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
            {
                if  (!x->m_bFullFilled)
                {
                    ww += x->m_Weight;
                }
                //x->RecCalc(Step, Result);
            }
            if  (ww > 0)
            {
                tmp = m_Reserved;
                for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
                {
                    CMaaTcpShaper * p = l->p;
                    if  (!p->m_bFullFilled)
                    {
                        t = tmp * p->m_Weight / ww;
                        if  (t + p->m_Reserved + p->m_v0 >= p->m_Max)
                        {
                            t = p->m_Max - (p->m_v0 + p->m_Reserved);
                            p->m_Reserved = p->m_Max - p->m_v0;
                            p->m_bFullFilled = true;
                            Result++;
                        }
                        else
                        {
                            p->m_Reserved += t;
                        }
                        if  (gStrictTcpShaperLimits > 0)
                        {
                            p->m_v0 += t; // can add to version > 1.2.9
                        }
                        m_Reserved -= t;
                    }
                    //p->RecCalc(Step, Result);
                }
                for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
                {
                    if  (!x->m_bFullFilled)
                    {
                        t = tmp * x->m_Weight / ww;
                        if  (t + x->m_v0 >= x->m_Max)
                        {
                            t = x->m_Max - x->m_v0;
                            if  (x->m_V < x->m_Max)
                            {
                                //t -= x->m_Max - x->m_V;
                                t = x->m_V - x->m_v0;
                            }
                            x->m_v0 = x->m_Max;
                            x->m_bFullFilled = true;
                            Result++;
                        }
                        else
                        {
                            x->m_v0 += t;
                        }
                        m_Reserved -= t;
                    }
                    //x->RecCalc(Step, Result);
                }
            }
        }

        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        /*
          for  (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
          {
               x->RecCalc(Step, Result);
          }
          */
        break;
    case 2: // Popup m_Reserved
        if  (m_Shaper->m_Shaper && m_Reserved > dThreshold)
        {
            m_Shaper->m_Reserved += m_Reserved;
            /// ++++++++ m_v0 -= m_Reserved; // can add to version > 1.2.9
            m_Reserved = 0;
            m_bFullFilled = true;
            m_bFinished = true;
            Result++;
        }
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        /*
          for  (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
          {
               x->RecCalc(Step, Result);
          }
          */
        break;
    case 3: // Store m_v1 = m_v0 (== Vsr)
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            x->m_v1 = x->m_v0;
        }
        break;
    case 4: // Compare m_V == 0 --> add parent->m_Reserved, mark m_bFullFilled
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            if  (!x->m_bFullFilled)
            {
                if  (x->m_V == 0) //< x->m_v0)
                {
                    x->m_bFullFilled = true;
                    m_Reserved += x->m_v0 - x->m_V;
                    Result++;
                }
            }
        }
        break;
    case 5: // Store m_v2 = m_v0 (== Vsr2)
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            x->m_v2 = x->m_v0;
        }
        break;
    case 6: // Compare m_V < m_v0 (m_Vsr2) --> add parent->m_Reserved, mark m_bFullFilled
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            if  (!x->m_bFullFilled)
            {
                if  ((double)x->m_V < x->m_v0)
                {
                    x->m_bFullFilled = true;
                    m_Reserved += x->m_v0 - x->m_V;
                    Result++;
                }
            }
        }
        break;
    case 7: // GetResults (Vsr = m_v2, Vsrd = m_v0)
        for (l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
        {
            CMaaTcpShaper * p = l->p;
            p->RecCalc(Step, Result);
        }
        for (x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
        {
            x->m_Vsr = (int)(x->m_v2 + 0.5);
            x->m_Vsrd = (int)(x->m_v0 + 0.5);
        }
        break;
    }
#ifdef __SHAPERS_PROFILING
    if  ((s_depth == 0 && Step == -1) || (s_depth == 1 && Step != -1))
    {
        _qword t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SRecCalc(%d, %d) = %,D\n", &gShaperTempInfo, Step, Result, t - t0);
        int index = Step >= 0 ? Step : 99;
        SockThread_pt[index] += t - t0;
    }
    --s_depth;
#endif
}
//--------------------------------------------------------------------------
#ifdef _WIN32
//--------------------------------------------------------------------------
//int CMaaSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep, const char * FuncName);
//--------------------------------------------------------------------------
/*
STATUS_ACCESS_VIOLATION Reading or writing to an inaccessible memory location.
STATUS_BREAKPOINT Encountering a hardware-defined breakpoint; used only by debuggers.
STATUS_DATATYPE_MISALIGNMENT Reading or writing to data at an address that is not properly aligned; for example, 16-bit entities must be aligned on 2-byte boundaries. (Not applicable to Intel 80x86 processors.)
STATUS_FLOATING_DIVIDE_BY_ZERO Dividing floating-point type by 0.0.
STATUS_FLOATING_OVERFLOW Exceeding maximum positive exponent of floating-point type.
STATUS_FLOATING_UNDERFLOW Exceeding magnitude of lowest negative exponent of floating-point type.
STATUS_FLOATING_RESEVERED_OPERAND Using a reserved floating-point format (invalid use of format).
STATUS_ILLEGAL_INSTRUCTION Attempting to execute an instruction code not defined by the processor.
STATUS_PRIVILEGED_INSTRUCTION Executing an instruction not allowed in current machine mode.
STATUS_INTEGER_DIVIDE_BY_ZERO Dividing an integer type by 0.
STATUS_INTEGER_OVERFLOW Attempting an operation that exceeds the range of the integer.
STATUS_SINGLE_STEP Executing one instruction in single-step mode; used only by debuggers.
*/
int CMaaSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep, const char * FuncName)
{
    char txt1[256], txt[256];
    sprintf(txt1, "Exception code = %x, Address = %p", code, ep->ExceptionRecord->ExceptionAddress);
    txt[0] = 0;

    int Ret = EXCEPTION_CONTINUE_SEARCH;

    switch(code)
    {
    case EXCEPTION_ACCESS_VIOLATION:
        /*
          for  (int i = 0; i < (int)ep->ExceptionRecord->NumberParameters; i++)
          {
               printf("ei[%d] = %08x\t", i, ep->ExceptionRecord->ExceptionInformation[i]);
          }
          printf("\n");
          */
        if  (ep->ExceptionRecord->NumberParameters >= 2)
        {
            sprintf(txt, "EXCEPTION_ACCESS_VIOLATION for data at %p can not be %s", (void *)ep->ExceptionRecord->ExceptionInformation[1], ep->ExceptionRecord->ExceptionInformation[0] == 0 ? "read" : "written");
        }
        else
        {
            sprintf(txt, "EXCEPTION_ACCESS_VIOLATION, data pointer is unknown");
        }
        Ret = EXCEPTION_EXECUTE_HANDLER;
        break;

    case EXCEPTION_BREAKPOINT:
        sprintf(txt, "EXCEPTION_BREAKPOINT");
        break;

    case EXCEPTION_DATATYPE_MISALIGNMENT:
        sprintf(txt, "EXCEPTION_DATATYPE_MISALIGNMENT");
        break;

    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        sprintf(txt, "EXCEPTION_FLT_DIVIDE_BY_ZERO");
        break;

    case EXCEPTION_FLT_OVERFLOW:
        sprintf(txt, "EXCEPTION_FLT_OVERFLOW");
        break;

    case EXCEPTION_FLT_UNDERFLOW:
        sprintf(txt, "EXCEPTION_FLT_UNDERFLOW");
        break;

        /*
     case STATUS_FLOATING_RESEVERED_OPERAND:
          sprintf(txt, "STATUS_FLOATING_RESEVERED_OPERAND");
          break;
     */

    case EXCEPTION_ILLEGAL_INSTRUCTION:
        sprintf(txt, "EXCEPTION_ILLEGAL_INSTRUCTION");
        break;

    case EXCEPTION_PRIV_INSTRUCTION:
        sprintf(txt, "EXCEPTION_PRIV_INSTRUCTION");
        break;

    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        sprintf(txt, "EXCEPTION_INT_DIVIDE_BY_ZERO");
        break;

    case EXCEPTION_INT_OVERFLOW:
        sprintf(txt, "EXCEPTION_INT_OVERFLOW");
        break;

    case EXCEPTION_SINGLE_STEP:
        sprintf(txt, "EXCEPTION_SINGLE_STEP");
        break;
    }

#ifdef __unix__
    mkdir("logs");
#else
    _mkdir("logs");
#endif
    FILE * f = fopen("logs\\exceptions.txt", "a+b");
    f = f ? f : fopen("c:\\maa\\exceptions.txt", "a+b"); // do not change reserved folder
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
        fprintf(f, "%2d.%02d.%d %02d:%02d:%02d: %s: %s\r\n%s\r\n\r\n",
             t0.tm_mday, t0.tm_mon + 1, t0.tm_year + 1900, t0.tm_hour, t0.tm_min, t0.tm_sec,
             FuncName, txt1, txt);
        fclose(f);
    }

    //return EXCEPTION_CONTINUE_EXECUTION;
    return Ret; // EXCEPTION_CONTINUE_SEARCH;
    //return EXCEPTION_EXECUTE_HANDLER;
}
//--------------------------------------------------------------------------
static int CMaaShaperThread_RRSEFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
    return CMaaSEFilter(code, ep, "CMyShaperThread_RRSEFilter()");
}
#endif
//--------------------------------------------------------------------------
unsigned CMaaShaperThread::Run()
{
    unsigned Ret = (unsigned)-1;
    //__try
    {
        Ret = Run1();
    }
    //__except(CMaaShaperThread_RRSEFilter(GetExceptionCode(), GetExceptionInformation()))
    {
    }
    return Ret;
}
//--------------------------------------------------------------------------
unsigned CMaaShaperThread::Run1()
{
    //try
    {
        return Run2();
    }
    /*
     catch(eExitThread)
     {
          throw;
     }
     catch(const char * msg)
     {
          RRCatchHandler("CMaaShaperThread::Run1()", m_Point, msg);
     }
     catch(...)
     {
          RRCatchHandler("CMaaShaperThread::Run1()", m_Point);
          throw;
     }
     */
}
//--------------------------------------------------------------------------
unsigned CMaaShaperThread::Run2()
{
    SetThreadName(-1, "CMaaShaperThread");
    /*
     {
                CProtocolListColor c(CProtocolListColor::eGreen);
                printf("CMaaTcpShaper thread id = %x\n", GetCurrentThreadId());
        }
        */
#ifdef _WIN32
    DWORD t0 = GetTickCount();
#else
    timeval t0;
    gettimeofday(&t0, nullptr);
#endif
    //CMaaSockStartup* pS = nullptr;
    try
    {
#ifdef __SHAPERS_PROFILING
        m_hrt.StartWatch();
#endif

        while(1)
        {
#ifdef _WIN32
            const int t = (int)(t0 + 1000 / _SHAPER_TICKS_PER_SECOND - GetTickCount());
            CheckPoint(t > 0 ? (DWORD)t : 0);
            t0 += 1000 / _SHAPER_TICKS_PER_SECOND;
#else
            timeval tt;
            gettimeofday(&tt, nullptr);
            const int t = 1000 / _SHAPER_TICKS_PER_SECOND - ((tt.tv_sec - t0.tv_sec) * 1000 + (tt.tv_usec - t0.tv_usec) / 1000);
            CheckPoint(t > 0 ? t : 0);
            t0.tv_usec += 1000000 / _SHAPER_TICKS_PER_SECOND;
            if  (t0.tv_usec >= 1000000)
            {
                t0.tv_sec += t0.tv_usec / 1000000;
                t0.tv_usec %= 1000000;
            }
#endif
#ifdef __SHAPERS_PROFILING
            memset(SockThread_pt, 0, sizeof(SockThread_pt));
#endif
            /*
            pS = gpSockStartup;
            if (pS)
            {
                pS->m_ShapersMutex.LockM();
            }
            */

            OnTimerUpdate();

            /*
            if (pS)
            {
                pS->m_ShapersMutex.UnLockM();
            }
            pS = nullptr;
            */
        }
    }
    catch(...)
    {
        /*
        if (pS)
        {
            pS->m_ShapersMutex.UnLockM();
        }
        */
    }
    return 0;
}
//--------------------------------------------------------------------------
void CMaaShaperThread::OnTimerUpdate()
{
    //printf("CMaaShaperThread::OnTimerUpdate() - 1\n"); fflush(stdout);
    //CMaaWin32Locker_Locker_gLock;
    //Locker_Lock;
    gpSockStartup&& gpSockStartup->m_SysShaperMutex.LockM();
    //printf("CMaaShaperThread::OnTimerUpdate() - 2\n"); fflush(stdout);

#ifdef __SHAPERS_PROFILING
    _qword t0 = m_hrt.GetTime();
    int nnn1 = 0;
#endif
    gpSockStartup&& gpSockStartup->m_UsrShaperMutex.LockM();
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
    {
        for (CMaaTcpShaper::L * l = p->m_Childs.LookAtFront(); l; l = p->m_Childs.Next(l))
        {
            l->p->OnTimerUpdate(/*this*/);
#ifdef __SHAPERS_PROFILING
            nnn1++;
#endif
        }
        //p->OnTimerUpdate();
    }
    gpSockStartup&& gpSockStartup->m_UsrShaperMutex.UnLockM();
#ifdef __SHAPERS_PROFILING
    //gShaperTempInfo.Format("%Sm_ShaperList items count = %d\n", &gShaperTempInfo, nnn1);
    SockThread_pt[20] = nnn1;
    _qword t = m_hrt.GetTime();
    //gShaperTempInfo.Format("%Stime1 = %,D\n", &gShaperTempInfo, t - t0);
    SockThread_pt[21] = t - t0;

    t0 = m_hrt.GetTime();
#endif

    int i = m_AveIndex + 1;
    i = i < _SHAPER_TICKS_PER_SECOND ? i : 0;
    m_AveIndex = i;
    int Result = i ? 0 : ++m_ShaperConditionLastCheckTime;
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
    {
        //p->RecCalc(-1, Result);
        p->RecCalc_1(Result);
    }
#ifdef __SHAPERS_PROFILING
    t = m_hrt.GetTime();
    //gShaperTempInfo.Format("%SRecCalc(-1) = %,D\n", &gShaperTempInfo, t - t0);
    SockThread_pt[22] = t - t0;
#endif
    try
    {
#ifdef __SHAPERS_PROFILING
        t0 = m_hrt.GetTime();
#endif
        int M = 0;
        //CMaaPtrAE_<CMaaTcpLLShaper *, 1> Ev(1024);
        try
        {
            for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
            {
                p->FillEvents(m_hEvs, M);
            }
        }
        catch(int)
        {
        }
        catch(...)
        {
        }
#ifdef __SHAPERS_PROFILING
        t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SFillEvents() = %,D\n", &gShaperTempInfo, t - t0);
        SockThread_pt[23] = t - t0;
        // Randomizing events positions
        t0 = m_hrt.GetTime();
#endif
#if 0
        if  (M > 1)
        {
            for (int i = 0; i < M; i++)
            {
                const int x = M * rand() / RAND_MAX;
                if  (x >= 0 && x < M)
                {
                    CMaaTcpLLShaper * hi = m_hEvs[i];
                    m_hEvs[i] = m_hEvs[x];
                    m_hEvs[x] = hi;
                }
            }
        }
#endif
#ifdef __SHAPERS_PROFILING
        t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%Srand() = %,D\n", &gShaperTempInfo, t - t0);
        SockThread_pt[24] = t - t0;
        t0 = m_hrt.GetTime();
#endif
        for (int i = 0; i < M; i++)
        {
#ifdef _WIN32
            DP(__utf8_printf("shaper SetEvent(%p)\n", m_hEvs[i]->m_hEv);)
            //SetEvent(m_hEvs[i]->m_hEv);
            WSASetEvent(m_hEvs[i]->m_hEv);
#else
            if  (!m_hEvs[i]->m_hEv)
            {
                m_hEvs[i]->m_hEv = true;
                if  (m_hEvs[i]->m_pSocket)
                {
                    //printf("w(%d=%d)", Ev[i]->m_pSocket->GetSocket(), Ev[i]->GetLenReserved(1000000));
                    m_hEvs[i]->m_pSocket->WakeUp(0x01);
                }
            }
#endif
        }
        //printf("((ww))\n");  fflush(stdout);
#ifdef __SHAPERS_PROFILING
        t = m_hrt.GetTime();
        //gShaperTempInfo.Format("%SSetEvent() = %,D\n", &gShaperTempInfo, t - t0);
        SockThread_pt[25] = t - t0;
#endif
    }
    catch(...)
    {
    }
    //printf("CMaaShaperThread::OnTimerUpdate() - 3\n"); fflush(stdout);
#ifdef __SHAPERS_PROFILING
    gShaperPerformanceInfo = gShaperTempInfo;
#endif
    gpSockStartup&& gpSockStartup->m_SysShaperMutex.UnLockM();
    //Locker_UnLock;
    //printf("CMaaShaperThread::OnTimerUpdate() - 4\n"); fflush(stdout);
#ifdef __SHAPERS_PROFILING
    t = m_hrt.GetTime();
    if  (t >= 90000000 && t < (90 + 4)*1000000)
    {
        //printf("CMaaShaperThread::OnTimerUpdate():\n%s", (const char *)gShaperPerformanceInfo);
        //m_logfile.fprintf("CMaaShaperThread::OnTimerUpdate():\n%S", &gShaperPerformanceInfo);
        static int nn = 0;
        if  (!nn++)
        {
            CMaaConcatString text;
            for (int i = 0; i < 100; i++)
            {
                if  (SockThread_pt[i])
                {
                    CMaaString txt;
                    txt.Format("[%d] = %,D\n", i, SockThread_pt[i]);
                    text += txt;
                }
            }
            m_logfile.fprintf("CMaaShaperThread::OnTimerUpdate():\n");
            m_logfile.Write(text);
        }
    }
#endif
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::FillEvents(CMaaPtrAE_<CMaaTcpLLShaper *, 1> &Ev, int &pos)
{
    for (CMaaTcpLLShaper * x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
    {
        Ev[pos] = x;
        if  (pos >= (int)Ev.MaxIndex())
        {
            throw 1;
        }
        pos++;
    }
    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        l->p->FillEvents(Ev, pos);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::SetNewShaperSpeedLimit(int MaxSpeed, double W) noexcept
{
    if  (MaxSpeed < 0)
    {
        MaxSpeed = 0x7fffffff;
    }
    m_MaxSpeed = MaxSpeed / _SHAPER_TICKS_PER_SECOND;
    if  (m_MaxSpeed < 1)
    {
        m_MaxSpeed = 1;
    }
    if  (W > 0.0)
    {
        m_Weight = W;
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::ReInit(double W, int MaxSpeed) // old name: ReInit1
{
    if  (MaxSpeed < 0)
    {
        MaxSpeed = 0x7fffffff;
    }
    m_MaxSpeed = MaxSpeed / _SHAPER_TICKS_PER_SECOND;
    if  (m_MaxSpeed < 1)
    {
        m_MaxSpeed = 1;
    }
    m_Weight = W;
    if  (m_Type == eAutoUserSubshaper)
    {
        m_Type = eUserSubshaper;
    }
    if  (m_Type == eUserSubshaper && m_UserId == -2 && m_Shaper)
    {
        for (CMaaTcpShaper::L * l = m_Shaper->m_Childs.LookAtFront(); l; l = m_Shaper->m_Childs.Next(l))
        {
            if  ((l->p->m_Type == eAutoUserSubshaper || (l->p->m_Type == eUserSubshaper)) && l->p != this)// && l->p->m_UserId == -1))
            {
                ReInitAutoUserSubshaper(l->p);
            }
        }
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::ReInitAutoUserSubshaper(CMaaTcpShaper * s)
{
    // this - template shaper
    // s - shaper to modify
    if  (s->m_Type != eUserSubshaper)
    {
        s->m_MaxSpeed = m_MaxSpeed;
        s->m_Weight = m_Weight;
    }
    s->SetConditions(m_Conditions, false);

    /*
     __lib_printf("src: %S\ndst: %S: type=%d, userid=%d, weight=%d, maxspeed=%d\n",
          &GetShaperNameRawRecursiveLibName(),
          &s->GetShaperNameRawRecursiveLibName(),
          s->m_Type, s->m_UserId, s->m_Weight, _SHAPER_TICKS_PER_SECOND * s->m_MaxSpeed);
     if   (m_Conditions.GetCount() || s->m_Conditions.GetCount())
     {
          __lib_printf("src conditions count = %d, dst conditions count=%d\n", m_Conditions.GetCount(), s->m_Conditions.GetCount());
     }
     */

    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        CMaaTcpShaper *p = nullptr;
        if  (l->p->m_Type == eGeneral)
        {
            s->m_GeneralShapers.Find(l->p->m_Name, &p);
        }
        else
        {
            s->m_UserShapers.Find(l->p->m_UserId, &p);
        }
        if  (!p)
        {
            p = TL_NEW CMaaTcpShaper(l->p->m_Type, l->p->m_Name, l->p->m_Weight, l->p->m_MaxSpeed * _SHAPER_TICKS_PER_SECOND, l->p->m_UserId);
            if  (!p)
            {
                continue;
            }
            //if   (!l->p->Add(*p))
            if  (!s->Add(*p)) // 3.12.2009
            {
                delete p;
                continue;
            }
        }
        l->p->ReInitAutoUserSubshaper(p);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::DumpAll()
{
    CMaaString __GetShaperNameRawRecursiveLibName = GetShaperNameRawRecursiveLibName();
    __lib_printf("%S type=%d, userid=%d, weight=%d, maxspeed=%d, conditions count = %d, %d childs, %d connections, (instant): bank=%d, m_V=%d\n",
         &__GetShaperNameRawRecursiveLibName,
         m_Type, m_UserId, m_Weight, _SHAPER_TICKS_PER_SECOND * m_MaxSpeed,
         m_Conditions.GetCount(),
         m_Childs.GetCount(),
         m_ChildsLL.GetCount(),
         m_Bank * _SHAPER_TICKS_PER_SECOND,
         m_V * _SHAPER_TICKS_PER_SECOND
         );

    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        CMaaTcpShaper *p = l->p;
        if  (p)
        {
            p->DumpAll();
        }
    }
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Add(CMaaTcpShaper &s)
{
    if  (!s.m_Shaper)
    {
        s.m_Shaper = this;
        m_Childs.AddAtBack(&s.m_L);
        bool b = true;
        if  (s.m_Type != eGeneral)
        {
            b = m_UserShapers.Add(s.m_UserId, &s) == 0;
        }
        else
        {
            b = m_GeneralShapers.Add(s.m_Name, &s) == 0;
        }
        if  (!b)
        {
            m_Childs.Release(&s.m_L);
            s.m_Shaper = nullptr;
        }
        else
        {
            if  (m_Shaper)
            {
                m_Shaper->AddRecursiveConnectionCounter(m_ConnectionsCount[0]);
                /*
                    if   (m_UserId != -2)
                    {
                         ++m_Shaper->m_ConnectionsCount[2];
                    }
                    */
            }
            //m_UseCount++;
        }
        return b;
    }
    return false;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Add(CMaaTcpLLShaper &l) noexcept
{
    if  (!l.m_Shaper)
    {
        int N = 0;
        CMaaTcpLLShaper * pl;
        int V0 = 0, Vsr = 0, Vsrd = 0;
        for (pl = m_ChildsLL.LookAtFront(); pl; pl = m_ChildsLL.Next(pl))
        {
            V0 += pl->m_V0;
            Vsr += pl->m_Vsr;
            Vsrd += pl->m_Vsrd;
            N++;
        }
        if  (N > 0)
        {
            const double k = (double)N / (double)(N + 1);

            for (pl = m_ChildsLL.LookAtFront(); pl; pl = m_ChildsLL.Next(pl))
            {
                pl->m_V0 = (int)(pl->m_V0 * k); pl->m_V0 = pl->m_V0 > 0 ? pl->m_V0 : 1;
                pl->m_Vsr = (int)(pl->m_Vsr * k); pl->m_Vsr = pl->m_Vsr > 0 ? pl->m_Vsr : 1;
                pl->m_Vsrd = (int)(pl->m_Vsrd * k); pl->m_Vsrd = pl->m_Vsrd > 0 ? pl->m_Vsrd : 1;
            }
        }
        V0 /= (N + 1); V0 = V0 > 0 ? V0 : 1;
        Vsr /= (N + 1); Vsr = Vsr > 0 ? Vsr : 1;
        Vsrd /= (N + 1); Vsrd = Vsrd > 0 ? Vsrd : 1;

        l.m_V0 = V0;
        l.m_Vsr = Vsr;
        l.m_Vsrd = Vsrd;
        l.m_Shaper = this;
        m_ChildsLL.AddAtBack(&l);
        ++m_ConnectionsCount[0];
        /*
          if   (++m_ConnectionsCount[0] == 1 && m_ConnectionsCount[1] == 0)
          {
               if   (m_Shaper)
               {
                    m_Shaper->m_ConnectionsCount[2]++;
               }
          }
          */
        AddRecursiveConnectionCounter(1);
        //m_UseCount++;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Del(CMaaTcpShaper &s) noexcept
{
    if  (s.m_Shaper == this)
    {
        m_Childs.Release(&s.m_L);
        if  (s.m_Type != eGeneral)
        {
            m_UserShapers.Remove(s.m_UserId);
        }
        else
        {
            m_GeneralShapers.Remove(s.m_Name);
        }
        s.m_Shaper = nullptr;
        if  (m_Shaper)
        {
            m_Shaper->AddRecursiveConnectionCounter(-m_ConnectionsCount[0]);
            /*
               if   (m_UserId != -2)
               {
                    --m_Shaper->m_ConnectionsCount[2];
               }
               */
        }
        //m_UseCount--;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::Del(CMaaTcpLLShaper &l) noexcept
{
    if  (l.m_Shaper == this)
    {
        m_ChildsLL.Release(&l);
        l.m_Shaper = nullptr;
        --m_ConnectionsCount[0];
        AddRecursiveConnectionCounter(-1);
        //m_UseCount--;
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::AddRecursiveConnectionCounter(int val) noexcept
{
    m_ConnectionsCount[1] += val;
    if  (val > 0 && m_ConnectionsCount[1] == val && m_Shaper)
    {
        m_Shaper->m_ConnectionsCount[2]++;
    }
    if  (val < 0 && m_ConnectionsCount[1] == 0 && m_Shaper)
    {
        m_Shaper->m_ConnectionsCount[2]--;
    }
    if  (m_Shaper)
    {
        m_Shaper->AddRecursiveConnectionCounter(val);
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::CopyFrom(CMaaTcpShaper *n, CMaaShaperThread *r)
{
    SetConditions(n->m_Conditions, false);

    for (L * l = n->m_Childs.LookAtFront(); l; l = n->m_Childs.Next(l))
    {
        CMaaTcpShaper *p = TL_NEW CMaaTcpShaper(l->p->m_Type, l->p->m_Name, l->p->m_Weight, l->p->m_MaxSpeed * _SHAPER_TICKS_PER_SECOND, l->p->m_UserId);
        if  (p)
        {
            if  (Add(*p))
            {
                //r->m_ShaperList.AddAtBack(p);
                p->CopyFrom(l->p, r);
            }
            else
            {
                delete p;
            }
        }
    }
}
//--------------------------------------------------------------------------
void CMaaTcpShaper::AddClone(int UserId, CMaaTcpShaper *n, CMaaShaperThread *r)
{
    CMaaTcpShaper* p = TL_NEW CMaaTcpShaper(eAutoUserSubshaper, gsCMaaStringZ, n->m_Weight, n->m_MaxSpeed * _SHAPER_TICKS_PER_SECOND, UserId);
    if  (p)
    {
        if  (Add(*p))
        {
            //r->m_ShaperList.AddAtBack(p);
            p->CopyFrom(n, r);
            p->m_UserId = UserId;
        }
        else
        {
            delete p;
        }
    }
}
//--------------------------------------------------------------------------
int CMaaTcpShaper::DeleteUnused() noexcept
{
    int Ret = 0;
    L * np;
    for (L * p = m_Childs.LookAtFront(); p; p = np)
    {
        np = m_Childs.Next(p);
        CMaaTcpShaper &s = *(p->p);
        const int n = s.DeleteUnused();
        if  (n == 0)
        {
            Del(s);
            delete &s;
        }
        Ret += n;
    }
    Ret += m_ChildsLL.GetCount();
    return Ret;
}
//--------------------------------------------------------------------------
bool CMaaTcpShaper::HasAttachedSockets() const noexcept
{
    if  (m_ChildsLL.LookAtFront())
    {
        return true;
    }
    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        if  (l->p->HasAttachedSockets())
        {
            return true;
        }
    }
    return false;
}
//--------------------------------------------------------------------------
void CMaaShaperThread::DeleteUnused() noexcept
{
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = m_ShaperList.Next(p))
    {
        p->DeleteUnused();
    }
}
//--------------------------------------------------------------------------
void CMaaShaperThread::CleanupShapers() noexcept
{
    CMaaTcpShaper * n;
    for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = n)
    {
        n = m_ShaperList.Next(p);
        if  (p->DeleteUnused() == 0)
        {
            m_ShaperList.Release(p);
            if  (p == m_SndSh)
            {
                m_SndSh = nullptr;
            }
            if  (p == m_RcvSh)
            {
                m_RcvSh = nullptr;
            }
            delete p;
        }
    }
}
//--------------------------------------------------------------------------
/*
void CMaaShaperThread::SetShapers(CMaaXmlNode n0)
{
        for     (CMaaTcpShaper * s = m_ShaperList.LookAtFront(); s; s = m_ShaperList.Next(s))
        {
                s->m_bMarkedForDeletion = true;
        }
        SetShapers(n0, m_SndSh, false);
        SetShapers(n0, m_RcvSh, true);
        m_SndSh->m_bMarkedForDeletion = false;
        m_RcvSh->m_bMarkedForDeletion = false;
        DeleteUnused(m_SndSh);
        DeleteUnused(m_RcvSh);
        CalcWeights();

        if(0)
        {
                CMaaTcpShaper *s, *r;
          FindShaper("Test\x01\x02\x01http", 8, &s, &r);
                CalcWeights();
        }
}

void CMaaShaperThread::SetShapers(CMaaXmlNode n0, CMaaTcpShaper *s, bool IsRecv)
{
        CMaaXmlNode n = n0.FindNode(gCon[CCon::e_ShapersListsElm2]);
        for     (; !n.IsNull(); n = n.FindNext())
        {
                double w = 1.0;
                int MaxSpeed = -1;
                if      (IsRecv)
                {
                        n.FindAttribute(gCon[CCon::e_ShapersListsElm2_aRWeight]).dsscanf("%lg", &w);
                        n.FindAttribute(gCon[CCon::e_ShapersListsElm2_aRMaxSpeed]).dsscanf("%d", &MaxSpeed);
                }
                else
                {
                        n.FindAttribute(gCon[CCon::e_ShapersListsElm2_aSWeight]).dsscanf("%lg", &w);
                        n.FindAttribute(gCon[CCon::e_ShapersListsElm2_aSMaxSpeed]).dsscanf("%d", &MaxSpeed);
                }
                if      (n.FindAttribute(gCon[CCon::e_ShapersListsElm2_aType]) == gCon[CCon::e_ShapersListsElm2_aType_vGeneral])
                {
                        CMaaString Name = n.FindAttribute(gCon[CCon::e_Name]);
                        CMaaTcpShaper * p = s->m_GeneralShapers[Name, nullptr];
                        if      (!p)
                        {
                                p = TL_NEW CMaaTcpShaper(CMaaTcpShaper::eGeneral, Name, w, MaxSpeed);
                                if      (p)
                                {
                                        if      (!s->Add(*p))
                                        {
                                                delete p;
                                                continue;
                                        }
                                        else
                                        {
                                                m_ShaperList.AddAtBack(p);
                                        }
                                }
                        }
                        else
                        {
                                p->ReInit1(w, MaxSpeed);
                        }
                        SetShapers(n, p, IsRecv);
                }
                else
                {
                        int Id = n.FindAttribute(gCon[CCon::e_ShapersListsElm2_aUserId]).ToInt(-3);
                        if  (Id == -3)
                        {
                            continue;
                        }
                        CMaaTcpShaper * p = s->m_UserShapers[Id, nullptr];
                        if      (!p)
                        {
                                p = TL_NEW CMaaTcpShaper(CMaaTcpShaper::eUserSubshaper, gsCMaaStringZ, w, MaxSpeed, Id);
                                if      (p)
                                {
                                        if      (!s->Add(*p))
                                        {
                                                delete p;
                                                continue;
                                        }
                                        else
                                        {
                                                m_ShaperList.AddAtBack(p);
                                        }
                                }
                        }
                        else
                        {
                                p->m_Type = CMaaTcpShaper::eUserSubshaper;
                                p->ReInit1(w, MaxSpeed);
                        }
                        if      (Id == -2)
                        {
                    SetShapers(n, p, IsRecv);
                                for     (CMaaTcpShaper::L * l = s->m_Childs.LookAtFront(); l; l = s->m_Childs.Next(l))
                                {
                                        p = l->p;
                                        if      (p->m_Type == CMaaTcpShaper::eAutoUserSubshaper && p->HasAttachedSockets())
                                        {
                                                p->ReInit1(w, MaxSpeed);
                                                SetShapers(n, p, IsRecv);
                                        }
                                        else if (p->m_Type == CMaaTcpShaper::eUserSubshaper && p->HasAttachedSockets())
                                        {
                                                p->ReInit1(w, MaxSpeed);
                                                SetShapers(n, p, IsRecv);
                                        }
                                }
                        }
                }
        }
}
*/
void CMaaShaperThread::FindShaper(const CMaaString &ShaperNameEx, int UserId, CMaaTcpShaper *&s, CMaaTcpShaper *&r)
{
    s = FindShaper(ShaperNameEx, 0, UserId, m_SndSh);
    r = FindShaper(ShaperNameEx, 0, UserId, m_RcvSh);
}

CMaaTcpShaper * CMaaShaperThread::FindShaper(const CMaaString &NameEx, int pos, int UserId, CMaaTcpShaper *s, bool bAutoCopy)
{
    const int End = (warning_int)NameEx.Length();

    if  (pos >= End || !s)
    {
        return nullptr;
    }
    while(pos < End)
    {
        if  (NameEx[pos] == '\x02')
        {
            CMaaTcpShaper * n = s->m_UserShapers[UserId, nullptr];
            if  (!n)
            {
                if  (bAutoCopy)
                {
                    s->m_UserShapers.Find(-2, &n);
                    if  (n)
                    {
                        s->AddClone(UserId, n, this);
                        // CalcWeights(); will be called from user function
                    }
                    if  (!(n = s->m_UserShapers[UserId, nullptr]))
                    {
                        return nullptr;
                    }
                }
                else
                {
                    return nullptr;
                }
            }
            else// if (UserId != -2)
            {
                if  (!n->m_Childs.LookAtFront() && bAutoCopy)
                {
                    CMaaTcpShaper * nn = s->m_UserShapers[-2, nullptr];
                    if  (nn)
                    {
                        n->CopyFrom(nn, this);
                    }
                }
            }
            pos++;

            s = n;
            int x = (warning_int)NameEx.Find(pos, '\x01');
            pos = x >= 0 ? x + 1 : End;
        }
        else
        {
            int x = (warning_int)NameEx.Find(pos, '\x01');
            x = x >= 0 ? x : End;
            CMaaString Name;
            Name = NameEx.RefMid(pos, x - pos);
            pos = x;
            CMaaTcpShaper * n = s->m_GeneralShapers[Name, nullptr];
            if  (!n)
            {
                return nullptr;
            }
            s = n;
            pos++;
        }
    }
    return s;
}

void CMaaTcpShaper::OnTimer(int i)
{
    //static DWORD dw0 = GetTickCount();
    //DWORD dw = GetTickCount();
    //int dwt = (int)(dw - dw0);
    //static CMaaFile f("c:\\temp\\shapers.txt", CMaaFile::eWCD_SrSw, eNoExcept);
    m_VTempSum += m_V;
    if  (i)
    {
        m_VAverageLast = m_VTempSum;
        m_VTempSum = 0;
    }
    //if   (m_MaxSpeed == 1)
    //{
    //    static int aa = 0;
    //    aa++;
    //}
    //CMaaString txt;
    //if   (dwt <= 100000)
    //{
    //     txt.Format("%4d.%03d %S: m_Bank=%d, m_MaxSpeed=%d, m_V=%d", dwt / 1000, dwt % 1000, &GetShaperNameRawRecursiveLibName(), m_Bank, m_MaxSpeed, m_V);
    //}
    if  (m_Bank >= 0)
    {
        m_Bank = m_MaxSpeed;
    }
    else
    {
        m_Bank += m_MaxSpeed;
        if  (m_Bank < 0)
        {
            m_Bank = 0;
        }
    }
    //if   (dwt <= 100000)
    //{
    //     txt.Format("%S, m_Bank=%d\r\n", &txt, m_Bank);
    //     if   (f.IsOpen())
    //     {
    //          f.Write(txt);
    //     }
    //}
    //else
    //{
    //     f.Close();
    //}
    m_V = 0;

    {
#ifdef __SHAPERS_PROFILING
        _qword t0 = m_hrt.GetTime();
#endif
        CMaaTcpLLShaper * next;
        for (CMaaTcpLLShaper * x = m_ChildsLL.LookAtFront(); x; x = next)
        {
            next = m_ChildsLL.Next(x);
            x->OnTimer(i);
        }
#ifdef __SHAPERS_PROFILING
        _qword t = m_hrt.GetTime();
        SockThread_pt[32] += t -t0;
#endif
    }
}

CMaaString CMaaTcpLLShaper::Dump()
{
    CMaaString txt;
    txt.Format("CMaaTcpLLShaper: this=%p, m_Shaper=%p, m_V=%d, m_Vsrd = %d, m_Vsr=%d, m_VTempSum=%d, m_VAverageLast=%d, m_V0=%d, m_Weight=%.4lf, m_MaxSpeedLimit=%d, "
         "m_v0=%.4lf, m_v1=%.4lf, m_v2=%.4lf, m_Reserved=%.4lf, m_Max=%.4lf, m_bFullFilled=%s, m_bFinished=%s"
         "\r\n",
         this, m_Shaper, _SHAPER_TICKS_PER_SECOND*m_V, _SHAPER_TICKS_PER_SECOND*m_Vsrd, _SHAPER_TICKS_PER_SECOND*m_Vsr, m_VTempSum, m_VAverageLast, _SHAPER_TICKS_PER_SECOND*m_V0, m_Weight, _SHAPER_TICKS_PER_SECOND*m_MaxSpeedLimit,
         m_v0, m_v1, m_v2, m_Reserved, m_Max, m_bFullFilled ? "true" : "false", m_bFinished ? "true" : "false"
         );
    return txt;
}

CMaaString CMaaTcpShaper::Dump()
{
    CMaaString txt;
    txt.Format("CMaaTcpShaper: this=%p, m_Shaper(parent)=%p, m_Type=%d, m_UserId=%d, m_Name=%s, m_MaxSpeed=%d, m_Bank=%d, "
         "m_Weight=%.4lf, m_V0=%d, m_Vsrd=%d, m_Vsr=%d, m_V=%d"
         "m_v0=%.4lf, m_Reserved=%.4lf, m_Max=%.4lf, m_bFullFilled=%s, m_bFinished=%s"
         "\r\n",
         this, m_Shaper, m_Type, m_UserId, (const char *)m_Name, _SHAPER_TICKS_PER_SECOND*m_MaxSpeed, _SHAPER_TICKS_PER_SECOND*m_Bank,
         m_Weight, _SHAPER_TICKS_PER_SECOND*m_V0, _SHAPER_TICKS_PER_SECOND*m_Vsrd, _SHAPER_TICKS_PER_SECOND*m_Vsr, _SHAPER_TICKS_PER_SECOND*m_V,
         m_v0, m_Reserved, m_Max, m_bFullFilled ? "true" : "false", m_bFinished ? "true" : "false"
         );
    CMaaString txt2;
    txt2.Format("m_Childs: %d, m_ChildsLL: %d\r\n", m_Childs.GetCount(), m_ChildsLL.GetCount());
    txt += txt2;
    txt2.Format("%p: L: ", this);
    for (L * l = m_Childs.LookAtFront(); l; l = m_Childs.Next(l))
    {
        txt += txt2 + l->p->Dump();
    }
    txt2.Format("%p: LL: ", this);
    for (CMaaTcpLLShaper * x = m_ChildsLL.LookAtFront(); x; x = m_ChildsLL.Next(x))
    {
        txt += txt2 + x->Dump();
    }
    txt += "\r\n\r\n";
    return txt;
}

//---------------------------------------------------------------------------------

CMaaTcpLLShaper::~CMaaTcpLLShaper()
{
    if  (m_Shaper)
    {
        //CMaaWin32Locker_Locker_gLock;
        //Locker_Lock;
        gpSockStartup&& gpSockStartup->m_SysShaperMutex.LockM();
        if  (m_Shaper)
        {
            m_Shaper->Del(*this);
        }
        gpSockStartup&& gpSockStartup->m_SysShaperMutex.UnLockM();
        //Locker_UnLock;
    }
#ifndef __unix__
    //CloseHandle(m_hEv);
    WSACloseEvent(m_hEv);
    m_hEv = nullptr;
#else
    m_hEv = false;
#endif
}

#endif // __SHAPERS
#endif // #ifndef TOOLSLIB_SINGLE_THREAD

#endif // #if defined(__TOOLS_USE_WINSOCK2)

/*
#ifndef ___ToolsLibQTAndroid___
_qword htonq(_qword x) noexcept
{
    x = (_qword)htonl(x >> 32) + ((_qword)htonl(x & 0xffffffff) << 32);
    return x;
}

_qword ntohq(_qword x) noexcept
{
    x = (_qword)htonl(x >> 32) + ((_qword)htonl(x & 0xffffffff) << 32);
    return x;
}
#endif
*/

#ifndef TOOLSLIB_SINGLE_THREAD
#ifdef __SHAPERS

DEF_ALLOCATOR(CMaaShaperThread)

CMaaShaperThread::CMaaShaperThread()
:   m_hEvs(1024)
#ifdef __SHAPERS_PROFILING
    ,
    m_logfile___("c:\\maa\\logfile.txt", CMaaFile::eWC_SrSw, false)
#endif
{
    //gpShaper_hrt = &m_hrt___;
    //gpm_logfile = &m_logfile___;

    m_pUserObject = nullptr;

    CMaaWin32Locker_Locker_gLock;
    Locker_Lock;
    gLock.AddRef();
    Locker_UnLock;

    m_AveIndex = m_ShaperConditionLastCheckTime = 0;
    m_ShaperList.AddAtBack(m_SndSh = TL_NEW CMaaTcpShaper("send"));
    m_ShaperList.AddAtBack(m_RcvSh = TL_NEW CMaaTcpShaper("recv"));

    if  (
#ifdef _WIN32
        0
#else
        1
#endif
    )
    {
        //1024000 B/s (1000 KB/s) speed limit
        m_Snd1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 1024000);
        m_Rcv1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 1024000);
    }
    else
    {
        // 10 B/s (0.01 KB/s) speed limit by default
        m_Snd1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 10);
        m_Rcv1000 = TL_NEW CMaaTcpShaper("102400", 10.0, 10);
    }
    if  (m_Snd1000)
    {
        m_SndSh->Add(*m_Snd1000);
    }
    if  (m_Rcv1000)
    {
        m_RcvSh->Add(*m_Rcv1000);
    }

    Create();
}

CMaaShaperThread::~CMaaShaperThread()
{
    Exit();
    //_RPT0(_CRT_WARN,"CMaaShaperThread::~CMaaShaperThread()\n");
    CMaaWin32Locker_Locker_gLock;
    //Locker_Lock;
    gpSockStartup&& gpSockStartup->m_SysShaperMutex.LockM();
    {
        CMaaTcpShaper * n;
        for (CMaaTcpShaper * p = m_ShaperList.LookAtFront(); p; p = n)
        {
            n = m_ShaperList.Next(p);
            if  (p->m_Shaper)
            {
                p->m_Shaper->Del(*p);
            }
        }
    }
    CleanupShapers();
    gpSockStartup&& gpSockStartup->m_SysShaperMutex.UnLockM();
    delete m_pUserObject; m_pUserObject = nullptr;
    Locker_Lock;
    gLock.UnRef();
    Locker_UnLock;
    //if (<0)
    //{
    //gpLock = nullptr;
    //}
}
#endif // __SHAPERS
#endif //#ifndef TOOLSLIB_SINGLE_THREAD

template <> _dword CMaaHashFunction<_IP6>(const _IP6& k) noexcept
{
    return k.Hash();
}

template <> _uqword CMaaHashFunction64<_IP6>(const _IP6& k) noexcept
{
    return k.Hash64();
}

#ifndef TOOLSLIB_SINGLE_THREAD // NEEDS_MULTITHREAD
#ifdef __SHAPERS
DEF_ALLOCATOR_CMaaUnivHash(int, CMaaTcpShaper*)
DEF_ALLOCATOR_CMaaUnivHash(CMaaString, CMaaTcpShaper*)
#endif
DEF_ALLOCATOR_CMaaUnivHash(CMaaSocket*, char)
DEF_ALLOCATOR_CMaaUnivHash(CMaaSocket*, int)
//DEF_ALLOCATOR_CMaaUnivHash(unsigned __int64, class CMaaSocket*)
#ifdef _WIN32
DEF_ALLOCATOR_CMaaUnivHash(SOCKET, CMaaSocket*)
#else
DEF_ALLOCATOR_CMaaUnivHash(int, CMaaSocket*)
#endif
#endif

#if 0
volatile int fext = 0;
class CUdpSrv : public CMaaUdpServer
{
    int N;
public:
    CUdpSrv(CMaaFdSockets * pFdSockets, _Port Port)
    :   CMaaUdpServer(pFdSockets, Port, 0, true),
        N(0)
    {
    }
    int Notify_Read()
    {
        char Buffer[64 * 1024 + 1024];
        _IP Ip = 0;
        _Port Port;
        int x = Recv(Buffer, 64 * 1024, &Ip, &Port);
        if  (x > 0)
        {
            ++N;
            //__utf8_printf("\rrecvfrom %I:%d %d %d    ", Ip, Port, x, N);
        }
        else
        {
            __utf8_printf("\nrecvfrom %I:%d %d %d\n", Ip, Port, x, N);
        }
        return 0;
    }
    int Notify_Error()
    {
        __utf8_printf("\nErr in srv\n");
        return CloseByException("");
    }
    ~CUdpSrv()
    {
        __utf8_printf("\n%d dg received\n", N);
    }
};
class CUdpCli : public CMaaUdpClient
{
    int N;
    _IP m_Ip;
    _Port m_Port;
    CMaaSockTimerT<CUdpCli> m_Timer1;
public:
    CUdpCli(CMaaFdSockets * pFdSockets, _IP Ip, _Port Port)
    :   CMaaUdpClient(pFdSockets, Port, Ip),
        N(0),
        m_Ip(Ip),
        m_Port(Port),
        m_Timer1(this, 1)
    {
        AddFdSocket();
        __utf8_printf("%p %p\n", m_pFdSockets, pFdSockets);
        m_Timer1.Attach(m_pFdSockets);
        m_Timer1.Start(1000000);
    }
    void OnTimer(int f)
    {
        __utf8_printf("OnTimer(%d)\n", f);
        m_Timer1.Stop();
        for (int i = 0; i < 10000; i++)
        {
            Send("abcdefg", 8);
        }
        CloseByException("");
    }
    ~CUdpCli()
    {
        fext = 1;
    }
};
int main_udp()
{
    try
    {
        CMaaSockStartup st;
        gpSockStartup = &st;
        CMaaSockThread thr1(nullptr), thr2(nullptr);
        TL_NEW CUdpSrv(thr1.m_pFdSockets, 2100);
        thr1.Create();

        TL_NEW CUdpCli(thr2.m_pFdSockets, CMaaIpAddress(194,177,20,83), 2100);
        thr2.Create();

        while(fext == 0) ;
        __utf8_printf("pre-exiting...\n");
        usleep(1000000);
        __utf8_printf("exiting...\n");
    }
    catch(...)
    {
    }
    return 0;
}
#endif
