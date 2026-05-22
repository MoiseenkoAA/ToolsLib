
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/FileTable.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains classes for working with hash tables on disk.
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
#ifdef _WIN32
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif
#else
#include "temp.h"
#endif

bool SwapFiles ( CMaaString f1, CMaaString f2 )
{
    CMaaString bak1 = f1 + ".bak";
    CMaaString bak2 = f2 + ".bak";
    CMaaFile::CopyFile ( f1, bak1 );
    CMaaFile::CopyFile ( f2, bak2 );
    try
    {
        CMaaFile::CopyFile ( f2, f1 );
        CMaaFile::CopyFile ( bak1, f2 );
        CMaaFile::Remove ( bak1, false );
        CMaaFile::Remove ( bak2, false );
    }
    catch(...)
    {
        CMaaFile::CopyFile ( bak1, f1 );
        CMaaFile::Remove ( bak1, false );
        CMaaFile::CopyFile ( bak2, f2 );
        CMaaFile::Remove ( bak2, false );
        throw;
    }
    return true;
}

//-----------------------------------------------------------------------------------------------------------
// class CMaaMessageDB
//-----------------------------------------------------------------------------------------------------------
CMaaMessageDB::~CMaaMessageDB ()
{
    if  ( m_fDeleteOnClose )
    {
        if  ( m_File.IsOpen () )
        {
            try
            {
                m_File.Close ();
            }
            catch(...)
            {
            }
            CMaaFile::Remove ( m_FileName, false );
        }
    }
}

void CMaaMessageDB::SetDeleteOnClose ( BOOL fDelete )
{
    m_Index.SetDeleteOnClose ( m_fDeleteOnClose = fDelete );
}

bool CMaaMessageDB::IsNeedSqeeze ()
{
    if  ((_qword)(h.m_FilledSize + h.m_FilledSize / 2 + 16 + sizeof ( h )) > h.m_Size)
    {
        return false;
    }
    return true;
}

void CMaaMessageDB::Flush ()
{
    if  ( m_File.IsOpen () )
    {
        m_File.Flush ();
    }
    m_Index.Flush ();
}

static const char * pszMessageDBHeader = "MAA's Message_DB";

bool CMaaMessageDB::RemoveFiles(const CMaaString &FileName)
{
    bool bRes = true;
    try
    {
        CMaaMessageDB tmp(FileName, 1024 * 1024, eCreate | eDeleteOnClose);
    }
    catch(...)
    {
        bRes = false;
    }
    return bRes;
}

CMaaMessageDB::CMaaMessageDB ( const CMaaString &FileName, _dword MaxMsgSize, int Flags, _qword MaxSize )
:   m_FileName ( FileName ),
    m_MaxSize ( MaxSize ),
    //m_File ( ( const char * ) nullptr ),
    m_MaxMsgSize ( MaxMsgSize ),
#ifdef _WIN32
#pragma warning(disable:4355) // suppress this used in base member initialization list warnin for the next line
#endif
    m_Index ( this, CMaaString ( FileName ) + ".idx", 0, 4, Flags & ( eCreate | eDeleteOnClose ) )
#ifdef _WIN32
#pragma warning(default:4355) // this used in base member initialization list
#endif
{
    m_fDeleteOnClose = ( Flags & eDeleteOnClose ) != 0;
    m_fAllowMsgOverlap = ( Flags & eAllowMsgOverlap ) != 0;
    memset ( &h, 0, sizeof ( h ) );
    try
    {
        if  ( Flags & eCreate )
        {
            CMaaFile::Remove (pszFileName2strFileName(FileName), false );
        }
        else
        {
            m_File = CMaaFile ( FileName, CMaaFile::eRp_SrSw );
            m_Status = eFileOpened;
        }
    }
    catch( XTOOFile2Error )
    {
    }
    try
    {
        if  ( !m_File.IsOpen () )
        {
            m_File = CMaaFile ( FileName, CMaaFile::eRWC_SrSw );
            m_Status = eFileCreated;
            //InitFile ();
            m_File.WriteVerify ( pszMessageDBHeader, 16 );
            h.m_FilledSize = 0;
            h.m_Size = 16 + sizeof ( h );
            m_File.WriteVerify ( &h, sizeof ( h ) );
        }
    }
    catch( XTOOFile2Error )
    {
        if  ( m_File.IsOpen () )
        {
            m_File.Close ();
            CMaaFile::Remove (pszFileName2strFileName(FileName), false );
            m_fIsOK = FALSE;
            m_Status = eFileInitError;
            return;
        }
    }
    if  ( !m_File.IsOpen () )
    {
        m_fIsOK = FALSE;
        m_Status = eFileCreateError;
        return;
    }

    char txt [ 16 ];
    m_File.Seek ( 0 );
    if  ( m_File.Read ( txt, 16 ) != 16 || !CMaaIsMemEq( txt, pszMessageDBHeader, 16 ) )
    {
        m_fIsOK = FALSE;
        m_Status = eFileIntegrityError;
        return;
    }
    //m_File.Seek ( MAA_HT_OFFSET0 );
    if  ( m_File.Read ( &h, sizeof ( h ) ) != sizeof ( h ) )
    {
        m_fIsOK = FALSE;
        m_Status = eFileIntegrityError;
        return;
    }
    if  ( h.m_Size < 0 || ( _dword ) h.m_Size > m_File.Length () )
    {
        h.m_Size = m_File.Length (); //ul
    }
    /*
     if   ( h.m_TableSize < 2 || h.m_TableSize > 0x7fffffff / ( sizeof ( Record ) + sizeof ( _qword ) ) ||
            h.m_HashSize < h.m_TableSize || h.m_HashSize > 0x7fffffff / ( sizeof ( Record ) + sizeof ( _qword ) ) ||
            ( h.m_Flags & 1 ) ||
            !IsValidRecN ( h.m_FreeList ) )
     {
          m_fIsOK = FALSE;
          h.m_Flags |= 2;
          m_Status = eFileIntegrityError;
          return;
     }
     if   ( m_File.Length () != sizeof ( h ) + MAA_HT_OFFSET2 + h.m_HashSize * sizeof ( _qword ) + h.m_TableSize * sizeof ( Record ) )
     {
          m_fIsOK = FALSE;
          h.m_Flags |= 2;
          UpdateHeader ();
          m_Status = eFileIntegrityError;
          return;
     }
     if   ( h.m_KeySize != sizeof ( Key ) || h.m_DataSize != sizeof ( Data ) )
     {
          m_fIsOK = FALSE;
          h.m_Flags |= 2;
          UpdateHeader ();
          m_Status = eFileFormatKeyDataError;
          return;
     }
     */
    // other (data structute) integrity is leaving unchecked
    m_fIsOK = TRUE;
}

bool CMaaMessageDB::IsValidOffset ( _qword o )
{
    if  ( o < 0 || o >= h.m_Size )
    {
        return false;
    }
    if  ( !m_File.Seek ( o ) )
    {
        return false;
    }
    MHeader mh;
    memset ( &mh, 0, sizeof ( mh ) );
    mh.m_Signature = MHeader::eDeleted;
    m_File.Read ( &mh, sizeof ( mh ) );
    return mh.m_Signature == MHeader::eValid && mh.m_Size >= (_qword)sizeof ( mh ) && mh.m_ReservedSize >= (_qword)sizeof ( mh ) && mh.m_Size <= mh.m_ReservedSize;
}

bool CMaaMessageDB::IsValidOffset ( _qword o, MHeader &mh )
{
    if  ( o < 0 || o >= h.m_Size )
    {
        return false;
    }
    if  ( !m_File.Seek ( o ) )
    {
        return false;
    }

    memset ( &mh, 0, sizeof ( mh ) );
    mh.m_Signature = MHeader::eDeleted;
    m_File.Read ( &mh, sizeof ( mh ) );
    return mh.m_Signature == MHeader::eValid && o + mh.m_ReservedSize <= h.m_Size && mh.m_Size >= (_qword)sizeof ( mh ) && mh.m_ReservedSize >= (_qword)sizeof ( mh ) && mh.m_Size <= mh.m_ReservedSize;
}

void CMaaMessageDB::UpdateHeader ()
{
    m_File.Seek ( 16 );
    m_File.Write ( &h, sizeof ( h ) );
}

// 0 - ok, 1 - database size limit, 2 - MessageSizeLimit, 3 - file error, (!= 0 - error(s))
int CMaaMessageDB::Add ( _qword Index, const void * ptr, _dword Size )
{
    if  (/*Size < 0 ||*/ Size > m_MaxMsgSize)
    {
        return 2;
    }
    _qword o = 0;
    MHeader mh;
    int f = m_Index.Find ( Index, &o );
    if  ( !f )
    {
        if  ( IsValidOffset ( o, mh ) )
        {
            if  ( m_fAllowMsgOverlap )
            {
                bool f = ( o + mh.m_ReservedSize >= h.m_Size );
                if  ( f )
                {
                    if  ( (_qword)(h.m_Size - mh.m_ReservedSize + sizeof ( mh ) + Size) > m_MaxSize && m_MaxSize > 0 )
                    {
                        return 1;
                    }
                }
                if  ( f || mh.m_ReservedSize >= (_qword)(sizeof ( mh ) + Size) )
                {
                    if  ( !m_File.Seek ( o ) )
                    {
                        return 3;
                    }
                    h.m_FilledSize += sizeof ( mh ) + Size - mh.m_Size;
                    if  ( f )
                    {
                        h.m_Size += -mh.m_ReservedSize + sizeof ( mh ) + Size;
                        mh.m_ReservedSize = sizeof ( mh ) + Size;
                    }
                    mh.m_Size = sizeof ( mh ) + Size;
                    m_File.Write ( &mh, sizeof ( mh ) );
                    m_File.Write ( ptr, Size );
                    if  ( f )
                    {
                        m_File.Truncate ();
                    }
                    UpdateHeader ();
                    return 0;
                }
            }
            if  ( (_qword)(h.m_Size + sizeof ( mh ) + Size) > m_MaxSize && m_MaxSize > 0 )
            {
                return 1;
            }
            if  ( !m_File.Seek ( o ) )
            {
                return 3;
            }
            mh.m_Signature = MHeader::eDeleted;
            m_File.Write ( &mh, sizeof ( mh ) );
            h.m_FilledSize -= mh.m_Size;
            UpdateHeader ();
        }
    }
    if  ( (_qword)(h.m_Size + sizeof ( mh ) + Size) > m_MaxSize && m_MaxSize > 0 )
    {
        return 1;
    }
    _qword o2 = h.m_Size;
    MHeader mh2;
    if  ( !m_File.Seek ( o2 ) )
    {
        if  ( f )
        {
            // restore (Index, o) or remove it finally
            if  ( !m_File.Seek ( o ) )
            {
                m_Index.Remove ( Index );
                return 1;
            }
            mh.m_Signature = MHeader::eValid;
            m_File.Write ( &mh, sizeof ( mh ) );
            h.m_FilledSize += mh.m_Size;
            UpdateHeader ();
            return 1;
        }
    }
    memset ( &mh2, 0, sizeof ( mh2 ) );
    mh2.m_ReservedSize = mh2.m_Size = sizeof ( mh2 ) + Size;
    mh2.m_ReservedIndex = Index;
    mh2.m_Signature = MHeader::eValid;
    m_File.WriteVerify ( &mh2, sizeof ( mh2 ) );
    m_File.WriteVerify ( ptr, Size );
    h.m_Size += mh2.m_ReservedSize;
    h.m_FilledSize += mh2.m_ReservedSize;
    UpdateHeader ();
    m_Index.AddOver ( Index, o2 );
    return 0;
}

// 0 - ok, 1 - not found, 2 - offset is not valid, 3 - file error, (!= 0 - error(s))
int CMaaMessageDB::Remove ( _qword Index )
{
    _qword o = 0;
    MHeader mh;
    int f = m_Index.Remove ( Index, &o );
    if  ( !f )
    {
        if  ( !IsValidOffset ( o, mh ) )
        {
            return 2;
        }
        if  ( !m_File.Seek ( o ) )
        {
            return 3;
        }
        mh.m_Signature = MHeader::eDeleted;
        m_File.Write ( &mh, sizeof ( mh ) );
        h.m_FilledSize -= mh.m_Size;
        if  ( o + mh.m_Size == h.m_Size && m_fAllowMsgOverlap )
        {
            // the last message
            h.m_Size = o;
            UpdateHeader ();
            m_File.Seek ( o );
            m_File.Truncate ();
        }
        else
        {
            UpdateHeader ();
        }
        return 0;
    }
    return 1;
}

// 0 - ok, 1 - not found, 2 - offset is not valid, 3 - file error, -1 - buffer is too small, (!= 0 - error(s))
int CMaaMessageDB::Get ( _qword Index, void * Buffer, _dword * BufferSize )
{
    _qword o = 0;
    MHeader mh;
    int f = m_Index.Find ( Index, &o );
    if  ( f )
    {
        return 1;
    }
    if  ( !IsValidOffset ( o, mh ) )
    {
        return 2;
    }
    _dword Sz = (_dword)(mh.m_Size - sizeof ( mh ));
    if  ( Sz > *BufferSize )
    {
        *BufferSize = Sz;
        return -1;
    }
    *BufferSize = Sz;
    if  ( !m_File.Seek ( o + sizeof ( mh ) ) )
    {
        return 3;
    }
    if  ( m_File.Read ( Buffer, Sz ) != Sz )
    {
        return 3;
    }
    return 0;
}

int CMaaMessageDB::Get(_qword Index, CMaaString &txt, _dword MaxDataSize)
{
    if  (MaxDataSize == (_dword)-1)
    {
        MaxDataSize = m_MaxMsgSize;
    }
    _dword ItemSize = 0;
    int r = -10;
    r = Get(Index, nullptr, &ItemSize);
    if  (!r)
    {
        txt.Empty();
    }
    else if (r == -1 && ItemSize <= MaxDataSize)
    {
        CMaaPtr_<char, -1> Buffer(ItemSize);
        r = -10;
        if  (Buffer.IsValid())
        {
            r = Get(Index, Buffer, &ItemSize);
            if  (!r)
            {
                txt = CMaaString(Buffer, ItemSize);
            }
        }
    }
    return r;
}

void CMaaMessageDB::SqeezeProc ( _qword &i, _qword &/*o*/, void * p )
{
    SqueezeStruct * s = ( SqueezeStruct * ) p;
    _dword Sz = s->m_BufferSize;
    if  ( !s->m_pGetFrom->Get ( i, s->m_Buffer, &Sz ) )
    {
        s->m_pAddTo->Add ( i, s->m_Buffer, Sz );
    }
}
// -1 - error, N - N bytes released
_qword CMaaMessageDB::Squeeze ()
{
    _qword N = h.m_Size;
    CMaaString fn2 = m_FileName + ".squeeze";
    try
    {
        BOOL fDel = m_fDeleteOnClose;
        CMaaPtr_<char, 1> Buffer ( m_MaxMsgSize );
        CMaaMessageDB mdb2 ( fn2, m_MaxMsgSize, eAllowMsgOverlap | eCreate, m_MaxSize );
        mdb2.SetDeleteOnClose ();
        SqueezeStruct s;
        s.m_Buffer = Buffer;
        s.m_BufferSize = m_MaxMsgSize;
        s.m_pGetFrom = this;
        s.m_pAddTo = &mdb2;
        m_Index.EnumerateByProc ( SqeezeProc, &s );
        Swap ( mdb2 );
        SetDeleteOnClose ( fDel ); // to make sure
        mdb2.SetDeleteOnClose ();
        m_Index.Sqeeze ();
    }
    catch(...)
    {
    }
    return N - h.m_Size;
}

void CMaaMessageDB::Swap ( CMaaMessageDB & That )
{
    m_File.Close ();
    That.m_File.Close ();
    m_Index.Swap ( That.m_Index );
    SwapFiles ( m_FileName, That.m_FileName );
    m_File = CMaaFile ( m_FileName, CMaaFile::eRp_SrSw );
    That.m_File = CMaaFile ( That.m_FileName, CMaaFile::eRp_SrSw );
    ReadHeader ();
    That.ReadHeader ();
}

void CMaaMessageDB::ReadHeader ()
{
    m_File.Seek ( 16 );
    memset ( &h, 0, sizeof ( h ) );
    m_File.Read ( &h, sizeof ( h ) );
}
