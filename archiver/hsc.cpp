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

#include "perm.h"
#ifdef _WIN32
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifndef NO_TOOLSLIB_ARCHIVERS

CHAm_HSC::CHAm_HSC(CHAm_IO &i, CHAm_IO &o, bool bPack)
:   m_i(i),
    m_o(o),
    m_file(!bPack ? o : i),
    m_ac(bPack ? o : i)
{
    // model data
    memset(curcon, 0, sizeof(curcon)); // current context
    ht=nullptr;            // hash table
    hp=nullptr;            // hash list pointer array
    con=nullptr;           // context array
    cl=nullptr;            // context length array
    cc=nullptr;            // character counts
    ft=nullptr;            // total frequency of context
    fe=nullptr;            // frequencys under ESCTH in context
    elp=nullptr;           // expire list previous pointer array
    eln=nullptr;           // expire list next pointer array
    elf = ell = 0;      // first and last of expire list
    rfm=nullptr;           // refresh counter array
    fa=nullptr;            // frequency array
    fc=nullptr;            // character for frequency array
    nb=nullptr;            // next pointer for frequency array
    fcfbl = 0;          // pointer to free frequency blocks
    nrel = 0;           // context for frequency block release

    // frequency mask system
    memset(cmask, 0, sizeof(cmask));     // masked characters
    memset(cmstack, 0, sizeof(cmstack)); // stack of cmask[] entries to clear 
    cmsp = 0;                            // pointer to cmstack

    // escape propability modifying system variables
    nec = 0;                      // counter for no escape expected
    memset(iec, 0, sizeof(iec));  // initial escape counters

    // update stack variables
    usp = 0;                     // stack pointer
    memset(cps, 0, sizeof(cps)); // context pointers
    memset(as, 0, sizeof(as));   // indexes to frequency array

    // miscalneous
    dropcnt = 0;                 // counter for context len drop
    maxclen = 0;                 // current maximum length for context 
    memset(hrt, 0, sizeof(hrt)); // semi random data for hashing
    memset(hs, 0, sizeof(hs));   // hash stack for context search 
    cslen = 0;                   // length of context to search

    if (bPack)
    {
        hsc_pack();
    }
    else
    {
        hsc_unpack();
    }
}

CHAm_HSC::~CHAm_HSC()
{
    hsc_cleanup();
}

#define IECLIM		32	       // initial escape counter upper limit
#define NECLIM		5	       // no escape expected counter limit
#define NECTLIM		4	       //
#define NECMAX		10	       // no escape expected counter maximum
#define MAXCLEN		4	       // assumed to be 4 in several places
#define NUMCON		10000	       // number of contexts to remember 
#define NUMCFB		32760	       // number of frequencies to remember
#define ESCTH		3	       // threshold for escape calculation
#define MAXTVAL		8000	       // maximum frequency value
#define RFMINI		4	       // initial refresh counter value
#define HTLEN	        16384	       // length of hash table
#define NIL		0xffff	       // NIL pointer in lists
#define ESC		256	       // escape symbol

/*
typedef unsigned char Context[4];

// model data
static Context curcon;		      // current context
static _word *ht=nullptr;		      // hash table
static _word *hp=nullptr;		      // hash list pointer array
static Context *con=nullptr;	      // context array
static unsigned char *cl=nullptr;	      // context length array
static unsigned char *cc=nullptr;	      // character counts
static _word *ft=nullptr;		      // total frequency of context
static unsigned char *fe=nullptr;	      // frequencys under ESCTH in context
static _word *elp=nullptr;		      // expire list previous pointer array
static _word *eln=nullptr;		      // expire list next pointer array
static _word elf,ell;		      // first and last of expire list
static unsigned char *rfm=nullptr;	      // refresh counter array
static _word *fa=nullptr;		      // frequency array
static unsigned char *fc=nullptr;	      // character for frequency array
static _word *nb=nullptr;		      // next pointer for frequency array
static _word fcfbl;		      // pointer to free frequency blocks
static _word nrel;		      // context for frequency block release

// frequency mask system
static unsigned char cmask[256];      // masked characters
static unsigned char cmstack[256];    // stack of cmask[] entries to clear 
static _sword cmsp;		      // pointer to cmstack

// escape propability modifying system variables
static unsigned char nec;	      // counter for no escape expected
static unsigned char iec[MAXCLEN+1];  // initial escape counters

// update stack variables
static _word usp;		      // stack pointer
static _word cps[MAXCLEN+1]; 	      // context pointers
static _word as[MAXCLEN+1];	      // indexes to frequency array

// miscalneous
static _sword dropcnt;		      // counter for context len drop
static unsigned char maxclen;	      // current maximum length for context 
static _word hrt[HTLEN];		      // semi random data for hashing
static _word hs[MAXCLEN+1]; 	      // hash stack for context search 
static _sword cslen;		      // length of context to search
*/
 
/***********************************************************************
	Cleanup routine
***********************************************************************/

void CHAm_HSC::hsc_cleanup()
{
    if (ht!=nullptr) delete [] ht,ht=nullptr;
    if (fc!=nullptr) delete [] fc,fc=nullptr;
    if (fa!=nullptr) delete [] fa,fa=nullptr;
    if (ft!=nullptr) delete [] ft,ft=nullptr;
    if (fe!=nullptr) delete [] fe,fe=nullptr;
    if (nb!=nullptr) delete [] nb,nb=nullptr;
    if (hp!=nullptr) delete [] hp,hp=nullptr;
    if (elp!=nullptr) delete [] elp,elp=nullptr;
    if (eln!=nullptr) delete [] eln,eln=nullptr;
    if (cl!=nullptr) delete [] cl,cl=nullptr;
    if (cc!=nullptr) delete [] cc,cc=nullptr;
    if (rfm!=nullptr) delete [] rfm,rfm=nullptr;
    if (con!=nullptr) delete [] con,con=nullptr;
}


/***********************************************************************
	System initialization
***********************************************************************/

//static  _word make_context(unsigned char cl, _sword c);

void CHAm_HSC::init_model()
{
    register _sword i;
    _sdword z,l,h,t;
    
    ht = TL_NEW _word[HTLEN];
    hp = TL_NEW _word[NUMCON];
    elp = TL_NEW _word[NUMCON];
    eln = TL_NEW _word[NUMCON];
    cl = TL_NEW unsigned char[NUMCON];
    cc = TL_NEW unsigned char[NUMCON];
    ft = TL_NEW _word[NUMCON];
    fe = TL_NEW unsigned char[NUMCON];
    rfm = TL_NEW unsigned char[NUMCON];
    con = TL_NEW CHAm_HSC_Context[NUMCON*1-0*sizeof(CHAm_HSC_Context)];
    fc = TL_NEW unsigned char[NUMCFB];
    fa = TL_NEW _word[NUMCFB];
    nb = TL_NEW _word[NUMCFB];
    if (hp==nullptr || elp==nullptr || eln==nullptr ||
	cl==nullptr || rfm==nullptr || con==nullptr ||
	cc==nullptr || ft==nullptr || fe==nullptr ||
	fc==nullptr || fa==nullptr || nb==nullptr || ht==nullptr) {
	hsc_cleanup();
	//error(1,ERR_MEM,"init_model()");
	throw "init_model()";
    }
    maxclen=MAXCLEN;		
    iec[0]=(IECLIM>>1);
    for (i=1;i<=MAXCLEN;++i) iec[i]=(IECLIM>>1)-1;
    dropcnt=NUMCON/4;
    nec=0;
    nrel=0;
    hs[0]=0;
    for (i=0;i<HTLEN;++i) ht[i]=NIL;	
    for (i=0;i<NUMCON;++i) {
	eln[i]=i+1;
	elp[i]=i-1;
	cl[i]=0xff;
	nb[i]=NIL;
    }
    elf=0;
    ell=NUMCON-1;
    for (i=NUMCON;i<NUMCFB-1;++i) nb[i]=i+1;
    nb[i]=NIL;
    fcfbl=NUMCON;
    curcon[3]=curcon[2]=curcon[1]=curcon[0]=0;
    cmsp=0;
    for (i=0;i<256;++i) cmask[i]=0;		
    for (z=10,i=0;i<HTLEN;++i) {
	h=z/(2147483647L/16807L);
	l=z%(2147483647L/16807L);		
	if ((t=16807L*l-(2147483647L%16807L)*h)>0) z=t;
	else z=t+2147483647L;
	hrt[i]=(_word)z&(HTLEN-1);
    }
}

void CHAm_HSC::init_pack()
{
    init_model();
    m_ac.ac_init_encode();
}

void CHAm_HSC::init_unpack()
{
    init_model();
    m_ac.ac_init_decode();
}


/***********************************************************************
	Finite context model
***********************************************************************/

#define HASH(s,l,h)	{				          \
			  h=0;                                    \
			  if (l) h=hrt[s[0]];                     \
			  if (l>1) h=hrt[(s[1]+h)&(HTLEN-1)];     \
			  if (l>2) h=hrt[(s[2]+h)&(HTLEN-1)];     \
			  if (l>3) h=hrt[(s[3]+h)&(HTLEN-1)];     \
			}                                                     

#define move_context(c) curcon[3]=curcon[2],curcon[2]=curcon[1], \
			curcon[1]=curcon[0],curcon[0]=c

void CHAm_HSC::release_cfblocks()
{
	
    register _word i,j,d;
    
    do {
	do if (++nrel==NUMCON) nrel=0; while (nb[nrel]==NIL);
	for (i=0;i<=usp;++i) if ((cps[i]&0x7fff)==nrel) break;
    } while (i<=usp);	
    for (i=nb[nrel],d=fa[nrel];i!=NIL;i=nb[i]) if (fa[i]<d) d=fa[i];
    ++d;
    if (fa[nrel]<d) {
	for (i=nb[nrel];fa[i]<d && nb[i]!=NIL;i=nb[i]);
	fa[nrel]=fa[i];
	fc[nrel]=fc[i];
	j=nb[i];
	nb[i]=fcfbl;
	fcfbl=nb[nrel];
	if ((nb[nrel]=j)==NIL) {
	    cc[nrel]=0;
	    fe[nrel]=(ft[nrel]=fa[nrel])<ESCTH?1:0;
	    return;
	}
    }
    fe[nrel]=(ft[nrel]=fa[nrel]/=d)<ESCTH?1:0;
    cc[nrel]=0;
    for (j=nrel,i=nb[j];i!=NIL;) {
	if (fa[i]<d) {
	    nb[j]=nb[i];
	    nb[i]=fcfbl;
	    fcfbl=i;
	    i=nb[j];
	}
	else {
	    ++cc[nrel];
	    ft[nrel]+=fa[i]/=d;
	    if (fa[i]<ESCTH) fe[nrel]++;
	    j=i;
	    i=nb[i];
	}
    }
}

_word CHAm_HSC::make_context(unsigned char conlen, _sword c)
{

    register _sword i;
    register _word nc,fp;
    
    nc=ell;
    ell=elp[nc];
    elp[elf]=nc;
    eln[nc]=elf;
    elf=nc;
    if (cl[nc]!=0xff) {
	if (cl[nc]==MAXCLEN && --dropcnt==0) maxclen=MAXCLEN-1;
	HASH(con[nc],cl[nc],i);
	if (ht[i]==nc) ht[i]=hp[nc];
	else {
	    for (i=ht[i];hp[i]!=nc;i=hp[i]);
	    hp[i]=hp[nc];
	}
	if (nb[nc]!=NIL) {
	    for (fp=nb[nc];nb[fp]!=NIL;fp=nb[fp]);
	    nb[fp]=fcfbl;
	    fcfbl=nb[nc];
	}
    }
    nb[nc]=NIL;
    fe[nc]=ft[nc]=fa[nc]=1;
    fc[nc]=c;
    rfm[nc]=RFMINI;
    cc[nc]=0;
    cl[nc]=conlen;
    con[nc][0]=curcon[0];
    con[nc][1]=curcon[1];
    con[nc][2]=curcon[2];
    con[nc][3]=curcon[3];
    HASH(curcon,conlen,i);
    hp[nc]=ht[i];
    ht[i]=nc;
    return nc;
}

void CHAm_HSC::el_movefront(_word cp)
{
    if (cp==elf) return;
    if (cp==ell) ell=elp[cp];
    else {
	elp[eln[cp]]=elp[cp];
	eln[elp[cp]]=eln[cp];
    }	
    elp[elf]=cp;
    eln[cp]=elf;
    elf=cp;
}

void CHAm_HSC::add_model(_sword c)
{
    register _word i;
    register _sword cp;
    
    while (usp!=0) {
	i=as[--usp];
	cp=cps[usp];
	if (cp&0x8000) {
	    cp&=0x7fff;
	    if (fcfbl==NIL) release_cfblocks();
	    nb[i]=fcfbl;
	    i=nb[i];
	    fcfbl=nb[fcfbl];
	    nb[i]=NIL;
	    fa[i]=1;			
	    fc[i]=c;
	    ++cc[cp];
	    ++fe[cp];
	}
	else if (++fa[i]==ESCTH) --fe[cp];
	if ((fa[i]<<1)<++ft[cp]/(cc[cp]+1)) --rfm[cp];
	else if (rfm[cp]<RFMINI) ++rfm[cp];
	if (!rfm[cp] || ft[cp]>=MAXTVAL) {
	    ++rfm[cp];
	    fe[cp]=ft[cp]=0;
	    for (i=cp;i!=NIL;i=nb[i]) {
		if (fa[i]>1) {
		    ft[cp]+=fa[i]>>=1;
		    if (fa[i]<ESCTH) ++fe[cp];
		}
		else {
		    ++ft[cp];
		    ++fe[cp];
		}
	    }
	}
    }
}

_word CHAm_HSC::find_next()
{
    register _sword i,k;
    register _word cp;
    
    for (i=cslen-1;i>=0;--i) {
	k=hs[i];
	for (cp=ht[k];cp!=NIL;cp=hp[cp]) {
	    if (cl[cp]==i) {
		switch (i) {
		  case 4:
		    if (curcon[3]!=con[cp][3]) break;
		  case 3:
		    if (curcon[2]!=con[cp][2]) break;
		  case 2:
		    if (curcon[1]!=con[cp][1]) break;
		  case 1:
		    if (curcon[0]!=con[cp][0]) break;
		  case 0:
		    cslen=i;
		    return cp;
		}
	    }	
	}
    }
    return NIL;
}

_word CHAm_HSC::find_longest()
{
    hs[1]=hrt[curcon[0]];	
    hs[2]=hrt[(curcon[1]+hs[1])&(HTLEN-1)]; 
    hs[3]=hrt[(curcon[2]+hs[2])&(HTLEN-1)]; 
    hs[4]=hrt[(curcon[3]+hs[3])&(HTLEN-1)]; 
    usp=0;
    while(cmsp) cmask[cmstack[--cmsp]]=0;
    cslen=MAXCLEN+1;
    return find_next();
}

_word CHAm_HSC::adj_escape_prob(_word esc, _word cp)
{
    if (ft[cp]==1) return iec[cl[cp]]>=(IECLIM>>1)?2:1;
    if (cc[cp]==255) return 1;
    if (cc[cp] && ((cc[cp]+1)<<1)>=ft[cp]) {
	esc=(_sword)((_sdword)esc*((cc[cp]+1)<<1)/ft[cp]);
	if (cc[cp]+1==ft[cp]) esc+=(cc[cp]+1)>>1;
    }
    return esc?esc:1;
}


_sword CHAm_HSC::code_first(_word cp, _sword c)
{
    register _word i;
    register _sword sum,cf,tot,esc;
    
    sum=cf=0;
    for (i=cp;i!=NIL;i=nb[i]) {
	if (fc[i]==c) {
	    cf=fa[i];
	    as[0]=i;
	    break;
	}
	sum+=fa[i];
    }	
    tot=ft[cp];
    esc=adj_escape_prob(fe[cp],cp);
    if (nec>=NECLIM) {
	if (tot<=NECTLIM && nec==NECMAX) {
	    tot<<=2;
	    sum<<=2;
	    cf<<=2;
	}
	else {
	    tot<<=1;
	    sum<<=1;
	    cf<<=1;
	}
    }
    usp=1;
    if (cf==0) {
	m_ac.ac_out(tot,tot+esc,tot+esc);
	for (i=cp;i!=NIL;sum=i,i=nb[i]) {
	    cmstack[cmsp++]=fc[i];
	    cmask[fc[i]]=1;
	}
	as[0]=sum;  /* sum holds last i ! */
	nec=0;
	if (ft[cp]==1 && iec[cl[cp]]<IECLIM) ++iec[cl[cp]];
	cps[0]=0x8000|cp; 
	return 0;
    }
    m_ac.ac_out(sum,sum+cf,tot+esc);
    if (nec<NECMAX) ++nec;
    if (ft[cp]==1 && iec[cl[cp]]) --iec[cl[cp]];
    cps[0]=cp;
    return 1;
}


_sword CHAm_HSC::code_rest(_word cp, _sword c)
{
    register _word i;
    register _sword sum,cf,tot,esc;
    
    tot=sum=cf=esc=0;
    for (i=cp;i!=NIL;i=nb[i]) {
	if (!cmask[fc[i]]) {
	    if (fa[i]<ESCTH) ++esc;
	    if (cf==0 && fc[i]==c) {
		sum=tot;
		cf=fa[i];
		as[usp]=i;
	    }
	    tot+=fa[i];
	}
    }	
    esc=adj_escape_prob(esc,cp);
    if (cf==0) {
	m_ac.ac_out(tot,tot+esc,tot+esc);
	for (i=cp;i!=NIL;sum=i,i=nb[i]) {
	    if (!cmask[fc[i]]) {
		cmstack[cmsp++]=fc[i];
		cmask[fc[i]]=1;
	    }
	}
	as[usp]=sum;  /* sum holds last i ! */
	if (ft[cp]==1 && iec[cl[cp]]<IECLIM) ++iec[cl[cp]];
	cps[usp++]=0x8000|cp; 
	return 0;
    }
    m_ac.ac_out(sum,sum+cf,tot+esc);
    ++nec;   /* must add test used in code_first() if NECMAX<5 ! */
    if (ft[cp]==1 && iec[cl[cp]]) --iec[cl[cp]];
    cps[usp++]=cp;
    return 1;
}

void CHAm_HSC::code_new(_sword c)
{
    register _sword i;
    register _word sum,tot;
    
    sum=0;
    tot=257-cmsp;
    for (i=0;i<c;++i) sum+=1-cmask[i];
    m_ac.ac_out(sum,sum+1,tot);
}

_sword CHAm_HSC::decode_first(_word cp)
{
    register _word c;
    register _word tv;
    register _word i;
    register _sword sum,tot,esc,cf;
    register unsigned char sv;

    esc=adj_escape_prob(fe[cp],cp);
    tot=ft[cp];
    if (nec>=NECLIM) {
	if (tot<=NECTLIM && nec==NECMAX) sv=2;
	else sv=1;
	tot<<=sv;
	tv=m_ac.ac_threshold_val(tot+esc)>>sv;
	for (c=cp,sum=0;;c=nb[c]) {
	    if (c==NIL) break;
	    if (sum+fa[c]<=tv) sum+=fa[c];
	    else {
		cf=fa[c]<<sv;
		break;
	    }
	}
	sum<<=sv;
    }
    else {
	tv=m_ac.ac_threshold_val(tot+esc);
	for (c=cp,sum=0;;c=nb[c]) {
	    if (c==NIL) break;
	    if (sum+fa[c]<=tv) sum+=fa[c];
	    else {
		cf=fa[c];
		break;
	    }
	}
    }
    usp=1;
    if (c!=NIL) {
	m_ac.ac_in(sum,sum+cf,tot+esc);
	if (ft[cp]==1 && iec[cl[cp]]) --iec[cl[cp]];
	as[0]=c;
	cps[0]=cp;
	c=fc[c];
	if (nec<NECMAX) ++nec;
    }
    else {
	m_ac.ac_in(tot,tot+esc,tot+esc);
	if (ft[cp]==1 && iec[cl[cp]]<IECLIM) ++iec[cl[cp]];
	for (i=cp;i!=NIL;sum=i,i=nb[i]) {
	    cmstack[cmsp++]=fc[i];
	    cmask[fc[i]]=1;
	}
	cps[0]=0x8000|cp;
	as[0]=sum;
	c=ESC;
	nec=0;
    }
    return c;
}

_sword CHAm_HSC::decode_rest(_word cp)
{
    register _word c;
    register _word tv;
    register _word i;
    register _sword sum,tot,esc,cf;
    
    esc=tot=0;
    for (i=cp;i!=NIL;i=nb[i]) {
	if (!cmask[fc[i]]) {
	    tot+=fa[i];
	    if (fa[i]<ESCTH) ++esc;
	}
    }
    esc=adj_escape_prob(esc,cp);
    tv=m_ac.ac_threshold_val(tot+esc);
    for (c=cp,sum=0;;c=nb[c]) {
	if (c==NIL) break;
	if (!cmask[fc[c]]) {
	    if (sum+fa[c]<=tv) sum+=fa[c];
	    else {
		cf=fa[c];
		break;
	    }
	}
    }
    if (c!=NIL) {
	m_ac.ac_in(sum,sum+cf,tot+esc);
	if (ft[cp]==1 && iec[cl[cp]]) --iec[cl[cp]];
	as[usp]=c;
	cps[usp++]=cp;
	c=fc[c];
	++nec;  /* must add test used in code_first() if NECMAX<5 ! */
    }
    else {
	m_ac.ac_in(tot,tot+esc,tot+esc);
	if (ft[cp]==1 && iec[cl[cp]]<IECLIM) ++iec[cl[cp]];
	for (i=cp;i!=NIL;sum=i,i=nb[i]) {
	    if (!cmask[fc[i]]) {
		cmstack[cmsp++]=fc[i];
		cmask[fc[i]]=1;
	    }
	}
	cps[usp]=0x8000|cp;
	as[usp++]=sum;		/* sum holds last i !! */
	c=ESC;
    }
    return c;
}

_sword CHAm_HSC::decode_new()
{
    register _sword c;
    register _word tv,sum,tot;
    
    tot=257-cmsp;
    tv=m_ac.ac_threshold_val(tot);
    for (c=sum=0;c<256;++c) {
	if (cmask[c]) continue;
	if (sum+1<=tv) ++sum;
	else break;
    }
    m_ac.ac_in(sum,sum+1,tot);	
    return c;
}

#define code_byte(cp,c) (cmsp?code_rest(cp,c):code_first(cp,c))
#define decode_byte(cp) (cmsp?decode_rest(cp):decode_first(cp))

/***********************************************************************
	Encoding
***********************************************************************/

void CHAm_HSC::hsc_pack()
{
    _sword c;
    _word cp;
    unsigned char ncmax,ncmin;
    
    init_pack();
    for (;(c=m_file.getbyte())>=0;) {
        //__utf8_printf(".");
	cp=find_longest();
	ncmin=cp==NIL?0:cl[cp]+1;
	ncmax=maxclen+1;
	for(;;) {
	    if (cp==NIL) {
	        //__utf8_printf("c");
		code_new(c);
		break;
	    }			
	    //__utf8_printf("C");
	    if (code_byte(cp,c)) {
	        //__utf8_printf(" el_mf ");
		el_movefront(cp);
		break;
	    }		
	    cp=find_next();
	}
	add_model(c);
	while (ncmax>ncmin) make_context(--ncmax,c);
	move_context(c);
    }
    cp=find_longest();
    while (cp!=NIL) {
	code_byte(cp,ESC);
	cp=find_next();
    }
    code_new(ESC);
    m_ac.ac_end_encode();
    hsc_cleanup();
}

/***********************************************************************
	Decoding
***********************************************************************/

void CHAm_HSC::hsc_unpack()
{
    _sword c;
    _word cp;
    unsigned char ncmax,ncmin;
    
    init_unpack();
    for (;;) {
	cp=find_longest(); 
	ncmin=cp==NIL?0:cl[cp]+1;
	ncmax=maxclen+1;
	for(;;) {
	    if (cp==NIL) {
		c=decode_new();
		break;
	    }			
	    if ((c=decode_byte(cp))!=ESC) {
		el_movefront(cp);
		break;
	    }		
	    cp=find_next();
	}
	if (c==ESC) break;
	add_model(c);
	while (ncmax>ncmin) make_context(--ncmax,c);
	m_file.putbyte(c);
	move_context(c);
    }
    m_file.flush();
    hsc_cleanup();
}

#endif
