
// unused file

//==============================================================================
// PROJECT: ToolsLib
//==============================================================================
// FILE:       GetStr.cpp
//
// AUTHOR:     Andrey A. Moisseyenko
//
// OVERVIEW    Работа с текстовыми строками
// ~~~~~~~~
//==============================================================================
#include "perm.h"
#include "temp.h"

#ifndef __TOOLSLIB_REMOVE_OLD_CODE

char * CMaaGetString ( char * pBuffer, int iSize, int Id, TOOMsgHandle Handle )
{
    if  ( iSize == 0 )
    {
        return nullptr;
    }

    int  Len = 0;

     #ifdef  _WIN32

    char Msg [ 1024 ];
    // Для UNICODE - другая функция !
    Len = LoadString ( Handle, Id, Msg, sizeof ( Msg ) );

     #else

    const char * Msg = nullptr;

    for ( pTOOStringRecord p = Handle; p -> Id > 0; p++ )
    {
        if  ( p -> Id == Id )
        {
            Msg = p -> Msg;
            Len = strlen ( Msg );
            break;
        }
    }

     #endif

    if  ( iSize > Len && ( Len ) )
    {
        memcpy ( pBuffer, Msg, Len + 1 );
    }
    else
    {
        * pBuffer = 0;
    }
    return pBuffer;
}

#endif
