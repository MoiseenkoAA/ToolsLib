/* MD4.H - header file for MD4C.C
 * $FreeBSD: src/sys/sys/md4.h,v 1.2 2005/01/07 02:29:23 imp Exp $
 */

/*-
   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD4 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.
   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD4 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

//#ifndef _MD4_H_
//#define _MD4_H_
#ifndef __MD4_INCLUDE__

/* MD4 context. */
/* typedef a 32-bit type */
typedef unsigned int UINT4;

typedef struct MD4Context {
    UINT4 state[4];   /* state (ABCD) */
    UINT4 count[2];   /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];     /* input buffer */
} MD4_CTX;

void   MD4Init(MD4_CTX *);
void   MD4Update(MD4_CTX *, const unsigned char *, unsigned int);
void   MD4Pad(MD4_CTX *);
void   MD4Final(unsigned char [16], MD4_CTX *);
char * MD4End(MD4_CTX *, char *);
char * MD4File(const char *, char *);
char * MD4Data(const unsigned char *, unsigned int, char *);


//void MD5Init ();
//void MD5Update ();
//void MD5Final ();

class CMD4Cacl
{
    MD4_CTX m_ctx;
public:
    CMD4Cacl();
    ~CMD4Cacl();
    void Update(const void * ptr, unsigned int Len) noexcept;
    void GetHash(void * ptr) noexcept; // 16 bytes;
    CMaaString GetHash() // 16 bytes string
    {
        CMaaString digest(nullptr, 16);
        if  (digest.Length() == 16)
        {
            GetHash((char *)(const char *)digest);
        }
        return digest;
    }
};

#define __MD4_INCLUDE__
#endif /* __MD4_INCLUDE__ */


//#endif /* _MD4_H_ */
