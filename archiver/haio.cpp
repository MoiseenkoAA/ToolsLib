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
       HA I/O routines
***********************************************************************/

#include "perm.h"
#ifdef _WIN32
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifndef NO_TOOLSLIB_ARCHIVERS

/*#define uppdcrc(_crc,_c) _crc=(crctab[((int)(_crc)^(_c))&0xff]^((_crc)>>8))
#define CRCMASK		0xffffffffUL
#define CRCP		0xEDB88320UL

int infile,outfile;
U32B crc;
U32B crctab[256];
unsigned char ib[BLOCKLEN],ob[BLOCKLEN];
int ibl,ibf,obl;
U32B icnt,ocnt,totalsize;
unsigned char r_crc,w_crc,r_progdisp,w_progdisp;
static int write_on,crctabok=0;
static char *outname=nullptr,*inname=nullptr;

void (*outspecial)(unsigned char *obuf, unsigned oblen);
unsigned (*inspecial)(unsigned char *ibuf, unsigned iblen);

static void makecrctab(void) {

    U16B i,j;
    U32B tv;
	
    for (i=0;i<256;i++) {
	tv=i;
	for (j=8;j>0;j--) {
	    if (tv&1) tv=(tv>>1)^CRCP;
	    else tv>>=1;
	}
	crctab[i]=tv;
    }
}

void setoutput(int fh, int mode, char *name) {

    outname=name;
    outspecial=nullptr;
    if (fh>=0) write_on=1;
    else write_on=0;
    obl=0;
    ocnt=0;
    outfile=fh;
    w_crc=mode&CRCCALC;
    if (w_crc) {
	if (!crctabok) makecrctab();
	crc=CRCMASK;
    }
    w_progdisp=mode&PROGDISP;
}


void setinput(int fh, int mode, char *name) {

    inname=name;
    inspecial=nullptr;
    ibl=0;
    icnt=0;
    infile=fh;
    r_crc=mode&CRCCALC;
    if (r_crc) {
	if (!crctabok) makecrctab();
	crc=CRCMASK;
    }
    r_progdisp=mode&PROGDISP;
}


U32B getcrc(void) {
	
    return crc^CRCMASK;
}

void clearcrc(void) {

    crc=CRCMASK;
}
*/

// HA based (modified by Moiseenko A.A.) IO routines, 2016

CHAm_IO::CHAm_IO(bool bOutput)
:   m_bOutput(bOutput)
{
    ibl = ibf = obl = 0;
    icnt = ocnt = totalsize = 0;
}
CHAm_IO::~CHAm_IO()
{
}

CHAm_IO_File::CHAm_IO_File(bool bOutput, CMaaFile f)
:   CHAm_IO(bOutput),
    m_File(f)
{
}

CHAm_IO_File::~CHAm_IO_File()
{
    if (m_bOutput)
    {
        try
        {
            flush();
        }
        catch(...)
        {
        }
    }
}

void CHAm_IO_File::bread()
{
    ibl = m_File.Read(ib, CHAm_BLOCKLEN);
    if (ibl < 0)
    {
        //error(1,ERR_READ,inname);
    }
    ibf=0;
}

void CHAm_IO_File::bwrite()
{
    //__utf8_printf("m_File.WriteVerify(ob, %d)\n", (int)obl);
    m_File.WriteVerify(ob, obl);
    ocnt+=obl;
    obl=0;
}

#endif
