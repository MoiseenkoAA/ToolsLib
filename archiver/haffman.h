// Haffman archiver originally made by Moiseenko A.A., 2016

// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/archiver/haffman.h

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

#ifndef NO_TOOLSLIB_ARCHIVERS

struct CMaaHaffman_Data
{
    int ch;
    CMaaMinHeap<int, CMaaHaffman_Data>::Rec *l, *r;
    CMaaHaffman_Data()
    {
        ch = -1;
        l = r = nullptr;
    }
};

class CMaaHaffman
{
public:
protected:
    CHAm_IO &m_i, &m_o, &m_file;
    int m_MaxChars, m_BufferSize, m_UpdatePeriod;
    CMaaPtr_<int, 1> m_Buffer;
    int m_pos;
    bool m_bBufferFilled;
    CMaaPtr_<_uqword, 1> m_Codes;
    CMaaPtr_<int, 1> m_CLen;
    CMaaPtr_<int, 1> m_Freq;
    int ppat, gpat;
public:
protected:
    CMaaPtr_<CMaaHeap<int, CMaaHaffman_Data>::Handle, 1> m_Handles;

    void putbit(int b)
    {
        ppat <<= 1;
        if (b) ppat|=1;
        if (ppat&0x100)
        {
            m_o.putbyte(ppat&0xff);
            ppat=1;
        }
    }
    void Flush()
    {
        if (ppat != 1)
        {
            while(!(ppat&0x100)) ppat<<=1;
            m_o.putbyte(ppat&0xff);
            ppat = 1;
        }
        m_o.flush();
    }
    int getbit()
    {
        gpat <<= 1;
        if (!(gpat&0xff))
        {
            gpat=m_i.getbyte();
            if (gpat == -1)
            {
                return -1;
            }
            else
            {
                gpat<<=1;
                gpat|=1;
            }
            /*if (gpat&0x100)
                gpat=0x100;
            else
            {
                gpat<<=1;
                gpat|=1;
            }*/
        }
        return (gpat&0x100) >> 8;
    }
public:
    CMaaHaffman(CHAm_IO &ii, CHAm_IO &oo, bool bPack, int MaxChars = 256, int BufferSize = 16 * 1024, int UpdatePeriod = 128);
    ~CMaaHaffman();
    void UpdateCodes(CMaaMinHeap<int, CMaaHaffman_Data>::Rec *z, _uqword pfx = 0, int l = 0);
    void UpdateCodes2(CMaaMinHeap<int, CMaaHaffman_Data>::Rec *z, _uqword pfx = 0, int l = 0);
};

//DEF_ALLOCATOR_CMaaHeap(int, CMaaHaffman_Data)
#endif

/*
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
*/
