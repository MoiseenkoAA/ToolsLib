
// HA based (modified by Moiseenko A.A.) arithmetic coder, 2016

/***********************************************************************
  This file is part of HA, a general purpose file archiver.
  Copyright (C) 1995 Harri Hirvola

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
************************************************************************
	HA arithmetic coder
***********************************************************************/

/***********************************************************************
This file contains some small changes made by Nico de Vries (AIP-NL)
allowing it to be compiled with Borland C++ 3.1.
***********************************************************************/

#include "perm.h"
#ifdef _WIN32
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifndef NO_TOOLSLIB_ARCHIVERS

CHAm_AC::CHAm_AC(CHAm_IO &f)
:   m_io(f)
{
    Init1st();
}

void CHAm_AC::Out(_word low, _word high, _word tot)
{
    ac_out(low, high, tot);
}

void CHAm_AC::In(_word low, _word high, _word tot)
{
    ac_in(low, high, tot);
}

void CHAm_AC::Flush()
{
    if (m_io.IsOutput())
    {
        //ac_end_encode();
    }
}

CHAm_AC::~CHAm_AC()
{
}

void CHAm_AC::Init1st()
{
    // "static"
    h = l = v = 0;
    s = 0;
    gpat = ppat = 0;
    // our
    /*buffer_pos = 0;
    if (m_io.IsOutput())
    {
        ac_init_encode();
    }
    else
    {
        ac_init_decode();
    }*/
}
/***********************************************************************
  Bit I/O
***********************************************************************/

#define putbit(b) { \
                     ppat <<= 1; \
                    if (b) ppat|=1; \
                    if (ppat&0x100) \
                    { \
                        m_io.putbyte(ppat&0xff); \
                        ppat=1; \
                    } \
                  }

#define getbit(b) { \
                    gpat <<= 1; \
                    if (!(gpat&0xff)) \
                    { \
                        gpat=m_io.getbyte(); \
                        if (gpat&0x100) \
                            gpat=0x100; \
                        else \
                        { \
                            gpat<<=1; \
                            gpat|=1; \
                        } \
                    } \
                    b |= (gpat&0x100) >> 8; \
                  }

/***********************************************************************
  Arithmetic encoding
***********************************************************************/

void CHAm_AC::ac_out(_word low, _word high, _word tot)
{
    //__utf8_printf("ac_out(%d %d %d)\n", low, high, tot);
    _dword r;

    r=(_dword)(h-l)+1;
    h=(_word)(r*high/tot-1)+l;
    l+=(_word)(r*low/tot);
    if (!((h^l)&0x8000)) 
    {
        putbit(l&0x8000);
        while(s) 
        {
            --s;
            putbit(~l&0x8000);
        }
        l<<=1;
        h<<=1;
        h|=1;
        while (!((h^l)&0x8000))
        {
            putbit(l&0x8000);
            l<<=1;
            h<<=1;
            h|=1;
        }
    }
    while ((l&0x4000)&&!(h&0x4000))
    {
        ++s;
        l<<=1;
        l&=0x7fff;
        h<<=1;
        h|=0x8001;
    }
}
void CHAm_AC::ac_init_encode()
{
    h=0xffff;
    l=s=0;
    ppat=1;
}

void CHAm_AC::ac_end_encode(void)
{
    ++s;
    putbit(l&0x4000);
    while (s--)
    {
        putbit(~l&0x4000);
    }
    if (ppat==1)
    {
        m_io.flush();
        return;
    }
    while(!(ppat&0x100)) ppat<<=1;
    m_io.putbyte(ppat&0xff);
    m_io.flush();
}


/***********************************************************************
  Arithmetic decoding
***********************************************************************/

void CHAm_AC::ac_in(_word low, _word high, _word tot)
{
    _dword r;

    r=(_dword)(h-l)+1;
    h=(_word)(r*high/tot-1)+l;
    l+=(_word)(r*low/tot);
    while (!((h^l)&0x8000))
    {
        l<<=1;
        h<<=1;
        h|=1;
        v<<=1;
        getbit(v);
    }
    while ((l&0x4000)&&!(h&0x4000))
    {
        l<<=1;
        l&=0x7fff;
        h<<=1;
        h|=0x8001;
        v<<=1;
        v^=0x8000;
        getbit(v);
    }
}

_word CHAm_AC::ac_threshold_val(_word tot)
{
    _dword r;

    r=(_dword)(h-l)+1;
    return (_word)((((_dword)(v-l)+1)*tot-1)/r);
}

void CHAm_AC::ac_init_decode(void)
{
    h=0xffff;
    l=0;
    gpat=0;
    v=m_io.getbyte() << 8;
    v|=0xff & m_io.getbyte();
}

#endif
