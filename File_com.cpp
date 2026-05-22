
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: РњРѕРёСЃРµРµРЅРєРѕ РђРЅРґСЂРµР№ РђР»РµРєСЃРµРµРІРёС‡)
 */

// ToolsLib/File_com.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains cross-platform classes for working with
 * files in utf8. Common code for all platform.
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
#include "temp.h"

#ifndef __TOOLSLIB_REMOVE_OLD_CODE

#ifndef __unix__
char mess_for_search[] = "file - %s, line - %s";
#else
char mess_for_search[] = "file - %s, line - %d";
#endif

XTOOFile _XTOOFile_true ( _true );

//------------------------------------------------------------------------------
// struct CMaaFile::CMaaFileImp
//------------------------------------------------------------------------------
CMaaFile::CMaaFileImp::CMaaFileImp ( FILE * file )
{
    pFile = file;
}

//------------------------------------------------------------------------------
CMaaFile::CMaaFileImp::~CMaaFileImp ()
{
    fclose ( pFile );
}
//------------------------------------------------------------------------------
// class CMaaFile
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CMaaFile::CMaaFile ( _PCSTR pszName, _PCSTR pszMode, _bool fMakeExeption )
:   m_pImp ( nullptr ),
    XTOOFile        ( fMakeExeption )
    //XTOOMsg ( fMakeExeption )
{
    Clear ();
    if  ( !pszName )
    {
        return;
    }

    FILE * file;

    SetErrMessEx ( pszName );

    if  ( pszMode )
    {
        file = fopen ( MkCompatible ( pszName ), pszMode );
    }
    else
    {
        //   From Comm_Nik action of CRWfopen ()
        //    - если файла нет, то он создается и готов к чтению и записи,
        //    - если файл есть, он открывается для чтения и записи,
        //    - работаем в бинарном режиме.

        file = fopen ( MkCompatible ( pszName ), "r+b" );
        if  ( ! file )
        {
            file =  fopen ( MkCompatible ( pszName ), "w+b" );
        }
    }

    if  ( file )
    {
        m_pImp = TL_NEW CMaaFileImp ( file );
    }
    else
    {
        SetErrMessEx( pszName );
        //XTOOErrno::SetError ( errno );
        //XTOOErrno::SetError ( errno );
        SetError( e_FILE_NOT_FOUND, errno );
    }
}

//------------------------------------------------------------------------------
CMaaFile::CMaaFile ( const CMaaFile & That )
:   m_pImp ( nullptr ),
    XTOOFile ( That.m_bIsThrow )
    //XTOOMsg ( That.m_bIsThrow )
{
    *this = That;
}

//------------------------------------------------------------------------------
CMaaFile::~CMaaFile ()
{
    if  ( IsOpen () )
    {
        if  (!m_pImp->Release())
        {
#if MANUAL_CMaaFileImp_REFS
            delete m_pImp;
#endif
        }
    }
}

_bool CMaaFile::IsOpen () const
{
    return m_pImp != nullptr;
}

_bool CMaaFile::Access ( _PCSTR pszName, int Mode )
{
#ifdef __unix__
    // MkCompatible()
    return _false;
#else
     #ifdef _WIN32
    return _access ( pszName, Mode ) == 0;
     #else
    return access ( pszName, Mode ) == 0;
     #endif
#endif
}

_bool CMaaFile::IsExists ( _PCSTR pszName )
{
#ifdef __unix__
    // Warning: different action
    FILE * f = fopen ( MkCompatible ( pszName ), "rb" );
    if  ( f )
    {
        fclose ( f );
    }
    return f ? _true : _false;
#else
    return Access ( MkCompatible ( pszName ), 0 );
#endif
}

//------------------------------------------------------------------------------
CMaaFile & CMaaFile::operator = ( const CMaaFile & That )
{
    //CMaaSyncAssign(m_pImp, That.m_pImp, true);

    static CMaaAtomicFastMutex sLock;
    sLock.LockM();
    if  (That.IsOpen())
    {
        That.m_pImp->AddRef();
    }
    if  (IsOpen())
    {
        if  (!m_pImp->Release())
        {
#if MANUAL_CMaaFileImp_REFS
            delete m_pImp;
#endif
        }
    }
    m_pImp     = That.m_pImp;
    m_bIsThrow = That.m_bIsThrow;
    SetErrMessEx(That.GetErrMessEx());
    sLock.UnLock();

    return *this;
}

//------------------------------------------------------------------------------
_dword CMaaFile::Read ( void *Buffer, _dword Size )
{
    int BytesRead = 0;
    if  ( IsOpen () )
    {
        Clear ();
        BytesRead = fread ( Buffer, 1, ( size_t ) Size, m_pImp -> pFile );
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return BytesRead;
}

//------------------------------------------------------------------------------
_dword CMaaFile::Write ( const void *Buffer, _dword Size )
{
    int BytesWrite = 0;
    if  ( IsOpen () )
    {
        if  ( Size )
        {
            Clear ();
            BytesWrite = fwrite ( Buffer, 1, ( size_t ) Size, m_pImp -> pFile );

            if  ( BytesWrite != (int)Size )
            {
                char buf[255];
                sprintf( buf, mess_for_search, __FILE__, __LINE__ );
                SetErrMessEx( buf );
                XTOOStatus::SetError ( e_FILE_WRITE_ERROR/*eTOOWriteLess*/ );
            }
        }
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return BytesWrite;
}

//------------------------------------------------------------------------------
void CMaaFile::Close ()
{
    if  ( IsOpen () )
    {
        if  (!m_pImp->Release())
        {
#if MANUAL_CMaaFileImp_REFS
            delete m_pImp;
#endif
        }
        m_pImp = nullptr;
    }
    else
    {
        if  ( m_pImp )
        {
            char buf[255];
            sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            SetErrMessEx( buf );
            XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
        }
    }
}

//------------------------------------------------------------------------------
_dword CMaaFile::Length ()
{
    _dword Length = ( _dword ) -1;

    if  ( IsOpen () )
    {
        Clear ();
#ifdef __unix__
        _dword pos = CurPos ();
        Seek ( 0, SEEK_END );
        Length = CurPos ();
        Seek ( pos );
#else
          #ifdef _WIN32
        Length = _filelength ( _fileno ( m_pImp -> pFile ) );
                #else
        Length = filelength ( fileno ( m_pImp -> pFile ) );
          #endif
#endif

        //curpos = ftell ( m_pImp -> pFile ); fseek ( m_pImp -> pFile, 0L, SEEK_END );
        //Length = ftell ( m_pImp -> pFile ); fseek ( m_pImp -> pFile, curpos, SEEK_SET );

        if  ( (int)Length == -1L )
        {
            char buf[255];
            sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            SetErrMessEx( buf );
            //XTOOErrno::SetError ( errno );
            SetError( e_FILE_SIZE, errno );
        }
    }
    else
    {
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return Length;
}

//------------------------------------------------------------------------------
_ErrCode CMaaFile::Seek ( long Offset, int FromWhere )
{
    if  ( IsOpen () )
    {
        return fseek ( m_pImp -> pFile, Offset, FromWhere );
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return -1;
}

//------------------------------------------------------------------------------
_dword CMaaFile::CurPos ()
{
    _dword Ret = ( _dword ) -1;
    if  ( IsOpen () )
    {
        Clear ();
        Ret = ftell ( m_pImp -> pFile );
        if  ( (int)Ret == -1 )
        {
            //XTOOErrno::SetError ( errno );
            char buf[255];
            sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            SetErrMessEx( buf );
            SetError( e_CUR_POS_FILE, errno );
        }
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return Ret;
}

//------------------------------------------------------------------------------
_bool CMaaFile::IsEOF ()
{
    if  ( IsOpen () )
    {
        return feof ( m_pImp -> pFile ) != 0;
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return _true;
}

//------------------------------------------------------------------------------
_bool CMaaFile::Truncate ()
{
    if  ( IsOpen () )
    {
        Clear ();
#ifdef __unix__
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_NOT_IMPLEMENTED );
        return _false;
#else
        if  (
          #ifdef _WIN32
             _chsize ( _fileno ( m_pImp->pFile ), ftell ( m_pImp -> pFile ) )
          #else
             chsize ( fileno ( m_pImp -> pFile ), ftell ( m_pImp -> pFile ) )
          #endif
             )
        {
            char buf[255];
            sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            SetErrMessEx( buf );
            //XTOOErrno::SetError ( errno );
            SetError ( e_CHANGE_SIZE_FILE, errno );
        }
#endif
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return !IsError ();
}

//------------------------------------------------------------------------------
void CMaaFile::Flush ()
{
    if  ( IsOpen () )
    {
        Clear ();

        if  ( fflush ( m_pImp->pFile ) )
        {
            // XTOOStatus::SetError ();
            char buf[255];
            sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            SetErrMessEx( buf );
            SetError ( e_FLUSH_FILE, EOF );
        }
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
}

//---------------------------------------------------------------------------
_ErrCode CMaaFile::Remove ( const char * szFileName, XTOOFile & Error )
{
    Error.Clear ();

    if  ( remove ( MkCompatible ( szFileName ) ) )
    {
        //Error.XTOOErrno::SetError ( errno );
        Error.SetErrMessEx( szFileName );
        Error.SetError ( e_DELETE_FILE, errno );
        return Error.XTOOErrnoOld::Errno ();
    }
    return 0;
}

_ErrCode CMaaFile::RemoveEx ( const char * szFileName, XTOOFile & Error )
{
#ifndef __unix__
    Attributes ( MkCompatible ( szFileName ), 0x20, Error );
#endif
    return Remove ( szFileName, Error );
}

char * CMaaFile::fgets ( char * pszBuffer, int Size )
{
    char * pszRet = nullptr;

    if  ( IsOpen () )
    {
        Clear ();

        pszRet = ::fgets ( pszBuffer, Size, m_pImp -> pFile );

        if  ( pszRet == nullptr && ferror ( m_pImp -> pFile ) )
        {
            //XTOOErrno::SetError ( errno );
            char buf[255];
            sprintf( buf, mess_for_search, __FILE__, __LINE__ );
            SetErrMessEx( buf );
            SetError( e_FILE_READ_ERROR, errno );
        }
    }
    else
    {
        char buf[255];
        sprintf( buf, mess_for_search, __FILE__, __LINE__ );
        SetErrMessEx( buf );
        XTOOStatus::SetError ( e_INVALID_IMPLEMENTATION/*eInvalidImplementation*/ );
    }
    return pszRet;
}

#endif // #ifndef __TOOLSLIB_REMOVE_OLD_CODE

//------------------------------------------------------------------------------
