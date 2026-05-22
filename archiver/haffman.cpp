// Haffman archiver originally made by Moiseenko A.A., 2016

// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/archiver/haffman.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains wide used definitions for ToolsLib and other projects.
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
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifndef NO_TOOLSLIB_ARCHIVERS

CMaaHaffman::CMaaHaffman(CHAm_IO &ii, CHAm_IO &oo, bool bPack, int MaxChars, int BufferSize, int UpdatePeriod)
:   m_i(ii),
    m_o(oo),
    m_file(!bPack ? oo : ii),
    m_MaxChars(MaxChars),
    m_BufferSize(BufferSize),
    m_UpdatePeriod(UpdatePeriod),
    m_Buffer(BufferSize),
    m_pos(0),
    m_bBufferFilled(false),
    m_Codes(MaxChars),
    m_CLen(MaxChars),
    m_Freq(MaxChars),
    ppat(1), gpat(0),
    m_Handles(MaxChars)
{
    bool dbg = false;

    int i;
    for (i = 0; i < MaxChars; i++)
    {
        m_Freq[i] = 1;
    }

    int n = 0;
    for (i = 0; i < MaxChars; i++)
    {
        if (m_Freq[i])
        {
            n++;
        }
    }
    CMaaHeap<int, CMaaHaffman_Data> h;
    n = 0;
    CMaaMinHeap<int, CMaaHaffman_Data>::Rec z;
    for (i = 0; i < MaxChars; i++)
    {
        if (m_Freq[i])
        {
            z.k = m_Freq[i];
            z.d.ch = i;
            z.d.l = z.d.r = nullptr;
            m_Handles[i] = h.Add(z.k, z.d);
            n++;
        }
    }

    if (bPack)
    {
        _uqword L = 0;

        _qword len = m_i.Length();
        for (i = 8 * (int)sizeof(len); i--; )
        {
            putbit((int)(len >> i)&1);
        }
        int ch;
        while((ch = m_file.getbyte()) >= 0)
        {
            if (m_pos % m_UpdatePeriod == 0)
            {
                CMaaMinHeap<int, CMaaHaffman_Data> hq(n = (int)h.GetCount());
                int nn = 0;
                for (i = 0; i < 64; i++)
                {
                    if (h.l[i])
                    {
                        for (int j = 0; j < (1 << i) && nn < n; j++)
                        {
                            if (!h.l[i][j])
                            {
                                CMaaString err;
                                err.Format("h.l[%d][%d] == nullptr error", i, j);
                                throw err;
                            }
                            hq.m[nn].k = h.l[i][j]->k;
                            hq.m[nn].d = h.l[i][j]->d;
                            nn++;
                        }
                    }
                }
                if (n != nn || n != m_MaxChars)
                {
                    __utf8_printf("n != nn  %d %d\n", n, nn);
                    CMaaString err;
                    err.Format("n != nn || n != m_MaxChars // %d %d %d", n, nn, m_MaxChars);
                    throw err;
                }

                for (i = 1; i <= n - 1; i++)
                {
                    CMaaMinHeap<int, CMaaHaffman_Data>::Rec x, y;
                    hq.Del(&x.k, &x.d);
                    hq.Del(&y.k, &y.d);
                    z.k = x.k + y.k;
                    z.d.l = TL_NEW CMaaMinHeap<int, CMaaHaffman_Data>::Rec;
                    z.d.r = TL_NEW CMaaMinHeap<int, CMaaHaffman_Data>::Rec;
                    z.d.ch = -1;
                    *z.d.l = x;
                    *z.d.r = y;
                    hq.Add(z.k, z.d);
                }

                if (hq.Del(&z.k, &z.d))
                {
                    UpdateCodes(&z);
                }
                else
                {
                    throw "get tree error";
                }
            }

            z.k = 0;
            int ch2 = m_Buffer[m_pos];
            L += m_CLen[ch];
            h.Del(m_Handles[ch], &z.k, &z.d);
            z.k++;
            m_Handles[ch] = h.Add(z.k, z.d);
            m_Buffer[m_pos] = ch;
            if (m_bBufferFilled)
            {
                h.Del(m_Handles[ch2], &z.k, &z.d);
                z.k--;
                m_Handles[ch2] = h.Add(z.k, z.d);
            }
            if (++m_pos >= m_BufferSize)
            {
                m_pos = 0;
                m_bBufferFilled = true;
            }
            int clen = m_CLen[ch];
            _uqword c = m_Codes[ch];
            if (dbg && L < 80)
            {
                __utf8_printf("%D,%d ", c, clen);
            }
            while(--clen >= 0)
            {
                if (dbg && L < 80) { __utf8_printf("%d", (int)(c >> clen)&1); }
                putbit(((int)(c >> clen)&1));// ^ 1);
            }
            if (dbg && L < 80) { __utf8_printf(" "); }
        }
        //__utf8_printf("archive size: %D bytes + %d bytes header = %D bytes\n", (L + 7) / 8, (int)sizeof(len), (L + 7) / 8 + (int)sizeof(len));
        Flush();
    }
    else
    {
        // UnPack

        _qword L = 0;
        _qword len = 0;
        {
            for (i = 0; i < 8 * (int)sizeof(len); i++)
            {
                len <<= 1;
                len |= getbit()&1;
            }
        }
        CMaaMinHeap<int, CMaaHaffman_Data>::Rec zz, *pzz;
        zz.k = -2;

        try
        {
            int ch;
            while(gpat >= 0)
            {
                if (m_pos % m_UpdatePeriod == 0)
                {
                    CMaaMinHeap<int, CMaaHaffman_Data> hq(n = (int)h.GetCount());
                    int nn = 0;
                    for (i = 0; i < 64; i++)
                    {
                        if (h.l[i])
                        {
                            for (int j = 0; j < (1 << i) && nn < n; j++)
                            {
                                if (!h.l[i][j])
                                {
                                    CMaaString err;
                                    err.Format("h.l[%d][%d] == nullptr error", i, j);
                                    throw err;
                                }
                                hq.m[nn].k = h.l[i][j]->k;
                                hq.m[nn].d = h.l[i][j]->d;
                                nn++;
                            }
                        }
                    }
                    if (n != nn || n != m_MaxChars)
                    {
                        __utf8_printf("n != nn  %d %d\n", n, nn);
                        CMaaString err;
                        err.Format("n != nn || n != m_MaxChars // %d %d %d", n, nn, m_MaxChars);
                        throw err;
                    }

                    for (i = 1; i <= n - 1; i++)
                    {
                        CMaaMinHeap<int, CMaaHaffman_Data>::Rec x, y;
                        hq.Del(&x.k, &x.d);
                        hq.Del(&y.k, &y.d);
                        z.k = x.k + y.k;
                        z.d.l = TL_NEW CMaaMinHeap<int, CMaaHaffman_Data>::Rec;
                        z.d.r = TL_NEW CMaaMinHeap<int, CMaaHaffman_Data>::Rec;
                        z.d.ch = -1;
                        *z.d.l = x;
                        *z.d.r = y;
                        hq.Add(z.k, z.d);
                    }

                    if (zz.k != -2)
                    {
                        UpdateCodes(&zz); // free
                    }
                    if (hq.Del(&zz.k, &zz.d))
                    {
                        //UpdateCodes(&z); at the next loop
                    }
                    else
                    {
                        throw "get tree error";
                    }
                }
                pzz = &zz;
                int ll = 0;
                while(pzz->d.ch < 0)
                {
                    int b = getbit();// ^ 1;
                    if (dbg && L < 10)
                    {
                        __utf8_printf("%d", b);
                    }
                    if (gpat < 0)
                    {
                        break;
                    }
                    ll++;
                    if (b == 0)
                    {
                        if (!pzz->d.l)
                        {
                            throw "error";
                        }
                        pzz = pzz->d.l;
                    }
                    else if (b == 1)
                    {
                        if (!pzz->d.r)
                        {
                            throw "error";
                        }
                        pzz = pzz->d.r;
                    }
                    else
                    {
                        throw "error";
                    }
                }
                if (dbg && L < 10)
                {
                    __utf8_printf(",%d ", ll);
                }
                if (gpat < 0)
                {
                    break;
                }
                ch = pzz->d.ch;

                z.k = 0;
                int ch2 = m_Buffer[m_pos];
                //L += m_CLen[ch];
                L++;
                h.Del(m_Handles[ch], &z.k, &z.d);
                z.k++;
                m_Handles[ch] = h.Add(z.k, z.d);
                m_Buffer[m_pos] = ch;
                if (m_bBufferFilled)
                {
                    h.Del(m_Handles[ch2], &z.k, &z.d);
                    z.k--;
                    m_Handles[ch2] = h.Add(z.k, z.d);
                }
                if (++m_pos >= m_BufferSize)
                {
                    m_pos = 0;
                    m_bBufferFilled = true;
                }
                m_o.putbyte(ch);
                if (L >= len)
                {
                    //__utf8_printf("overflow is fixed on L > len check before m_o.putbyte(ch);\n");
                    break;
                }
            }
            //__utf8_printf("source size: %D bytes\n", L);
            m_o.flush();
        }
        catch(...)
        {
            if (zz.k != -2)
            {
                UpdateCodes(&zz); // free
            }
            try
            {
                m_o.flush();
            }
            catch(...)
            {
            }
            throw;
        }
        if (zz.k != -2)
        {
            UpdateCodes(&zz); // free
            //UpdateCodes2(&zz); // free
        }
    }
    //__utf8_printf("(end) heap size: %d\n", h.m_Size); // 0
}

CMaaHaffman::~CMaaHaffman()
{
}

void CMaaHaffman::UpdateCodes(CMaaMinHeap<int, CMaaHaffman_Data>::Rec *z, _uqword pfx, int l)
{
    if (z->d.ch >= 0)
    {
        m_CLen[z->d.ch] = l;
        m_Codes[z->d.ch] = pfx;
    }
    else
    {
        //__utf8_printf("  %S %d\n", &pfx, z->k);
    }
    if (z->d.l)
    {
        UpdateCodes(z->d.l, pfx << 1, l + 1);
        delete z->d.l;
    }
    if (z->d.r)
    {
        UpdateCodes(z->d.r, (pfx << 1) + 1, l + 1);
        delete z->d.r;
    }
}

void CMaaHaffman::UpdateCodes2(CMaaMinHeap<int, CMaaHaffman_Data>::Rec *z, _uqword pfx, int l)
{
    if (z->d.ch >= 0)
    {
        m_CLen[z->d.ch] = l;
        m_Codes[z->d.ch] = pfx;
        __utf8_printf("%02x %c %D %d %d\n", z->d.ch, (char)z->d.ch, pfx, l, z->k);
    }
    else
    {
        __utf8_printf("->%D %d %d\n", pfx, l, z->k);
    }
    if (z->d.l)
    {
        UpdateCodes2(z->d.l, pfx << 1, l + 1);
        delete z->d.l;
    }
    if (z->d.r)
    {
        UpdateCodes2(z->d.r, (pfx << 1) + 1, l + 1);
        delete z->d.r;
    }
}

DEF_ALLOCATOR_CMaaHeap(int, CMaaHaffman_Data)
#endif

#if 0
int main(int argn, char * args[])
{
    if (argn == 4)
    {
        if (args[1][0] == 'a')
        {
            try
            {
                CHAm_IO_File i(false, CMaaFile(args[2], CMaaFile::eR_SrSw));
                CHAm_IO_File o(true, CMaaFile(args[3], CMaaFile::eWC_SrSw));
                CMaaHaffman a(i, o, true);
                __utf8_printf("done\n");
            }
            catch(...)
            {
                __utf8_printf("unknown error\n");
                return 1;
            }
            return 0;
        }
        if (args[1][0] == 'x')
        {
            try
            {
                CHAm_IO_File i(false, CMaaFile(args[2], CMaaFile::eR_SrSw));
                CHAm_IO_File o(true, CMaaFile(args[3], CMaaFile::eWC_SrSw));
                CMaaHaffman a(i, o, false);
                __utf8_printf("done\n");
            }
            catch(...)
            {
                __utf8_printf("unknown error\n");
                return 1;
            }
            return 0;
        }
    }
    //if (argn != 4)
    {
        __utf8_printf("Haffman pack/unpack:\n");
        __utf8_printf("%s a srcfn dstfn\n", args[0]);
        __utf8_printf("%s x srcfn dstfn\n", args[0]);
        return 1;
    }
}
#endif
