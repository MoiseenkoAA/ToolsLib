// HA based (modified by Moiseenko A.A.) IO routines, 2016

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

#ifndef NO_TOOLSLIB_ARCHIVERS

/*
#define BLOCKLEN 	8192

extern unsigned char ib[],ob[];
extern int ibl,ibf,obl;
extern U32B icnt,ocnt,totalsize;

#define getbyte() (ibl>0?(--ibl,ib[ibf++]):(bread(),(ibl>0?--ibl,ib[ibf++]:-1)))
#define putbyte(c) {ob[obl++]=(c);if(obl==BLOCKLEN)bwrite();}
#define flush() bwrite()

#define CRCCALC		1	// flag to setinput/setoutput
#define PROGDISP	2	// flog to setinput/setoutput

extern void (*outspecial)(unsigned char *obuf, unsigned oblen);
extern unsigned (*inspecial)(unsigned char *ibuf, unsigned iblen);

void setoutput(int fh, int mode, char *name);
void setinput(int fh, int mode, char *name);
U32B getcrc(void);
void clearcrc(void);
void bread(void);
void bwrite(void);
*/

const int CHAm_BLOCKLEN = 8192;

class CHAm_IO
{
protected:
    bool m_bOutput;
    unsigned char ib[CHAm_BLOCKLEN], ob[CHAm_BLOCKLEN];
    int ibl, ibf, obl;
    _dword icnt, ocnt, totalsize;
public:
    CHAm_IO(bool bOutput);
    virtual ~CHAm_IO();
    virtual void bread() = 0;
    virtual void bwrite() = 0;
    int getbyte()
    {
        return (ibl > 0 ? (--ibl, ib[ibf++]) : (bread(), (ibl > 0 ? --ibl, ib[ibf++] : -1)));
    }
    void putbyte(unsigned char c)
    {
        ob[obl++] = c;
        //__utf8_printf("o");
        if   (obl == CHAm_BLOCKLEN)
        {
            bwrite();
        }
    }
    void flush()
    {
        bwrite();
    }
    bool IsOutput()
    {
        return m_bOutput;
    }
    virtual _qword Length()
    {
        return -1; // not supported
    }
};

class CHAm_IO_File : public CHAm_IO
{
protected:
    CMaaFile m_File;
public:
    CHAm_IO_File(bool bOutput, CMaaFile f);
    ~CHAm_IO_File();
    void bread();
    void bwrite();
    _qword Length()
    {
        return m_File.Length();
    }
};

#endif
