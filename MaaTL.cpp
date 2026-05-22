//==========================================================================
// PRODUCT: MaaRF - Maa's routing firewall software driver
// (C) Copyright Maa, 2003. All Rights Reserved.
//==========================================================================
// FILE:       MaaTL.h
//
// AUTHOR:     Andrey A. Moisseyenko
//
// OVERVIEW    Maa's template library.
// ~~~~~~~~
// DATE:       26.10.2003
//==========================================================================

#include "perm.h"
#include "temp.h"

#if 0
_dword CMaaStandardHashFunction ( const void * ptr, int len )
{
    unsigned char * p = ( unsigned char * ) ptr;
    _dword x = 0;
    for ( int i = len; i--; )
    {
        x = ((x << 7) | (x >> 25)) ^ *p++;
    }
    return x;
}
#endif
