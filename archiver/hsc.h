// HA based (modified by Moiseenko A.A.) HA HSC method, 2016

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
	HA HSC method
***********************************************************************/

/*	HSC method packing function
*/

//void hsc_pack(void);


/*	HSC method unpacking function
*/

//void hsc_unpack(void);

/*	Cleanup for HSC method
*/
//void hsc_cleanup(void);

/*	Cleanup for error conditions
*/

#ifndef NO_TOOLSLIB_ARCHIVERS

const int CHAm_HSC_HTLEN = 16384; // length of hash table
const int CHAm_HSC_MAXCLEN = 4;   // assumed to be 4 in several places

class CHAm_HSC
{
    CHAm_IO &m_i, &m_o, &m_file;
    CHAm_AC m_ac;
public:
    CHAm_HSC(CHAm_IO &i, CHAm_IO &o, bool bPack);
    ~CHAm_HSC();
protected:
    void hsc_pack();
    void hsc_unpack();
    void hsc_cleanup();
    void init_model();
    void init_pack();
    void init_unpack();
    void release_cfblocks();
    _word make_context(unsigned char conlen, _sword c);
    void el_movefront(_word cp);
    void add_model(_sword c);
    _word find_next();
    _word find_longest();
    _word adj_escape_prob(_word esc, _word cp);
    _sword code_first(_word cp, _sword c);
    _sword code_rest(_word cp, _sword c);
    void code_new(_sword c);
    _sword decode_first(_word cp);
    _sword decode_rest(_word cp);
    _sword decode_new();
    typedef unsigned char CHAm_HSC_Context[4];

    // model data
    CHAm_HSC_Context curcon;    // current context
    _word *ht;                  // hash table
    _word *hp;                  // hash list pointer array
    CHAm_HSC_Context *con;      // context array
    unsigned char *cl;          // context length array
    unsigned char *cc;          // character counts
    _word *ft;                  // total frequency of context
    unsigned char *fe;          // frequencys under ESCTH in context
    _word *elp;                 // expire list previous pointer array
    _word *eln;                 // expire list next pointer array
    _word elf, ell;             // first and last of expire list
    unsigned char *rfm;         // refresh counter array
    _word *fa;                  // frequency array
    unsigned char *fc;          // character for frequency array
    _word *nb;                  // next pointer for frequency array
    _word fcfbl;                // pointer to free frequency blocks
    _word nrel;                 // context for frequency block release

    // frequency mask system
    unsigned char cmask[256];   // masked characters
    unsigned char cmstack[256]; // stack of cmask[] entries to clear 
    _sword cmsp;                // pointer to cmstack

    // escape propability modifying system variables
    unsigned char nec;            // counter for no escape expected
    unsigned char iec[CHAm_HSC_MAXCLEN+1]; // initial escape counters

    // update stack variables
    _word usp;                  // stack pointer
    _word cps[CHAm_HSC_MAXCLEN+1];       // context pointers
    _word as[CHAm_HSC_MAXCLEN+1];        // indexes to frequency array

    // miscalneous
    _sword dropcnt;               // counter for context len drop
    unsigned char maxclen;        // current maximum length for context 
    _word hrt[CHAm_HSC_HTLEN];    // semi random data for hashing
    _word hs[CHAm_HSC_MAXCLEN+1]; // hash stack for context search 
    _sword cslen;                 // length of context to search
};

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
                CHAm_HSC a(i, o, true);
                __utf8_printf("done\n");
            }
            catch(...)
            {
                __utf8_printf("unknown error\n");
                return 1;
            }
            try
            {
                CHAm_IO_File i(false, CMaaFile(args[2], CMaaFile::eR_SrSw));
                CHAm_IO_File o(true, CMaaFile("testa", CMaaFile::eWC_SrSw));
                CHAm_HSC a(i, o, true);
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
                CHAm_HSC a(i, o, false);
                __utf8_printf("done\n");
            }
            catch(...)
            {
                __utf8_printf("unknown error\n");
                return 1;
            }
            try
            {
                CHAm_IO_File i(false, CMaaFile(args[2], CMaaFile::eR_SrSw));
                CHAm_IO_File o(true, CMaaFile("testx", CMaaFile::eWC_SrSw));
                CHAm_HSC a(i, o, false);
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
        __utf8_printf("%s a srcfn dstfn\n", args[0]);
        __utf8_printf("%s x srcfn dstfn\n", args[0]);
        return 1;
    }
}
*/
