
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/FileTable.h

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


//extern CMaaString g_dbg;

#define e_INFINITE_LOOP__FILE_INT_ERR 1234
#define e_ERROR_CREATE_FILE 1234 + 1
#define e_FILE_INTEGRITY_ERROR 1234 + 2
#define e_FILE_KEY_DATA_FORMAT_ERROR 1234 + 3

/*
template < class T > void CMaaSwap ( T & a, T & b )
{
     T tmp ( a );
     a = b;
     b = tmp;
}
*/

bool SwapFiles ( CMaaString f1, CMaaString f2 );

//---------------------------------------------------------------------------
#define MAA_HT_NULL  -1
#define MAA_HT_OFFSET0 16
#define MAA_HT_OFFSET1 ( MAA_HT_OFFSET0 + 16 )
#define MAA_HT_OFFSET2 ( MAA_HT_OFFSET1 + 16 )
#define pszHeader "MAA's Hash Table"
#define pszIndex  "<<< HT Index >>>"
#define pszItems  "<<< HT Items >>>"
//---------------------------------------------------------------------------

#ifdef __unix__
//#define __STRUCT__PACKED __attribute__((packed))
#else
//#define __STRUCT__PACKED
#endif

struct CMaaUnivFileHash_Header
{
    _qword  m_TableSize;   // size of item table
    _qword  m_HashSize;    // size of hash table
    _qword  m_KeySize;
    _qword  m_DataSize;
    _qword  m_Flags;
    _qword  m_ItemCount;
    _qword  m_FreeList;
    _qword  m_Reserved0;
} ; //__attribute__((packed));

template < class Key, class Data > class CMaaUnivFileHash
{
    struct Record
    {
        _qword Next;
        Key  K;
        Data D;
    }
#ifdef __unix__
    //      __attribute__((packed))
#endif
    ;
#if 1
    typedef CMaaUnivFileHash_Header Header;
#else
    struct Header
    {
        _qword  m_TableSize;   // size of item table
        _qword  m_HashSize;    // size of hash table
        _qword  m_KeySize;
        _qword  m_DataSize;
        _qword  m_Flags;
        _qword  m_ItemCount;
        _qword  m_FreeList;
        _qword  m_Reserved0;
    } ; //__attribute__((packed));
#endif

    Header h;

    BOOL m_fIsOK, m_fDeleteOnClose;
    int m_Status;

protected:
    // hash function of key
    virtual _uqword HashFunction ( const Key & K ) const
    {
        if constexpr (sizeof(Key) == sizeof(_dword))
        {
            return (_uqword)*(_udword *)&K;
        }
        else if constexpr (sizeof(Key) == sizeof(_qword))
        {
            return *(_uqword *)&K;
        }
        else
        {
            return CMaaStandardHashFunction64CLK<(int)sizeof(Key)>((const unsigned char*)&K);
        }
    }
    virtual _bool IsEqual ( const Key & K1, const Key & K2 )
    {
        return K1 == K2;
    }

    CMaaString m_FileName;
    CMaaFile m_File;

public:
    int IsValidRecN ( _qword n ) const noexcept
    {
        return ( n >= 0 && n < h.m_TableSize ) ? 1 : 0;
    }

    // Эти интициализационные данные будут записаны
    // в только что созданную таблицу и будут находиться
    // в удалённых элементах таблицы
    virtual void InitKeyAndData ( Key & K, Data & D )
    {
        memset ( &K, -1, sizeof ( K ) );
        memset ( &D, -1, sizeof ( D ) );
    }
    virtual bool IsLooksValid ( Key & K, Data & D )
    {
        Key K2;
        Data D2;
        InitKeyAndData ( K2, D2 );

        return K != K2 || D != D2;
    }

    BOOL IsNeedRepair ()
    {
        if  ( m_File.IsOpen () && ( h.m_Flags & 2 ) )
        {
            return TRUE;
        }
        return FALSE;
    }

    // -1 == can't repair
    // n - n items recovered
    _qword Repair ()
    {
        Flush ();
        Header h;
        memset ( &h, 0, sizeof ( h ) );
        CMaaFile f ( m_FileName, CMaaFile::eR_SrSw );
        f.Seek ( MAA_HT_OFFSET0 );
        f.Read ( &h, sizeof ( h ) );
        if  ( h.m_TableSize < 2 || h.m_TableSize > 0x7fffffff / ( sizeof ( Record ) + sizeof ( _qword ) ) ||
             h.m_HashSize < h.m_TableSize || h.m_HashSize > 0x7fffffff / ( sizeof ( Record ) + sizeof ( _qword ) ) )
        {
            return -1;
        }
        if  ( h.m_KeySize != sizeof ( Key ) || h.m_DataSize != sizeof ( Data ) )
        {
            return -1;
        }
        CMaaString tmpfn = m_FileName + ".repair";
        CMaaFile::Remove ( tmpfn, false );
        CMaaUnivFileHash < Key, Data > * NewTable = MakeNewTable ( tmpfn, 4, eCreate | eDeleteOnClose );
        if  ( !NewTable )
        {
            CMaaFile::Remove ( tmpfn, false );
            return -1;
        }

        _qword n = 0;
        if  ( !NewTable->IsOK () )
        {
            //NewTable->SetDeleteOnClose ();
            delete NewTable;
            CMaaFile::Remove ( tmpfn, false );
            return -1;
        }
        try
        {
            f.Seek ( sizeof ( h ) + MAA_HT_OFFSET2 + h.m_HashSize * sizeof ( _qword ) );
            Record r;
            while( f.Read ( &r, sizeof ( r ) ) == sizeof ( r ) )
            {
                if  ( IsLooksValid ( r.K, r.D ) )
                {
                    if  ( !NewTable->Add ( r.K, r.D ) )
                    {
                        n++;
                    }
                }
            }
            BOOL fDel = m_fDeleteOnClose;
            Swap ( *NewTable );
            SetDeleteOnClose ( fDel );
            NewTable->SetDeleteOnClose ();
        }
        catch(...)
        {
            delete NewTable;
            CMaaFile::Remove ( tmpfn, false );
            return -1;
        }
        delete NewTable;
        CMaaFile::Remove ( tmpfn, false );
        return n;
    }

    static void SqeezeProc ( Key & K, Data & D, void * p )
    {
        CMaaUnivFileHash < Key, Data > * t = ( CMaaUnivFileHash < Key, Data > * ) p;
        t->Add ( K, D );
    }
    static void CountProc ( Key & /*K*/, Data & /*D*/, void * p )
    {
        (*(_qword *)p)++;
    }
    _qword Sqeeze ()
    {
        Flush ();
        _qword n = 0;
        EnumerateByProc ( &CountProc, &n );

        CMaaString tmpfn = m_FileName + ".sqeeze";
        CMaaFile::Remove ( tmpfn, false );
        CMaaUnivFileHash < Key, Data > * NewTable = MakeNewTable ( tmpfn, n, eCreate | eDeleteOnClose );
        if  ( !NewTable || !NewTable->IsOK () )
        {
            delete NewTable;
            CMaaFile::Remove ( tmpfn, false );
            return -1;
        }
        try
        {
            EnumerateByProc ( &SqeezeProc, NewTable );
            BOOL fDel = m_fDeleteOnClose;
            Swap ( *NewTable );
            SetDeleteOnClose ( fDel );
            NewTable->SetDeleteOnClose ();
        }
        catch(...)
        {
            delete NewTable;
            CMaaFile::Remove ( tmpfn, false );
            return -1;
        }
        delete NewTable;
        CMaaFile::Remove ( tmpfn, false );
        return h.m_ItemCount;
    }

    enum eStatus
    {
        eFileCreateError = 0,
        eFileInitError,
        eFileIntegrityError,
        eFileFormatKeyDataError,
        eFileOpened,
        eFileCreated,

        eStatusMax
    };

    void ThrowError ( int n )
    {
        //g_dbg.Append ( "ThrowError(%d) for file object %s\n", n, ( const char * ) m_FileName );
        switch( GetStatus () )
        {
        case eFileCreateError:
        case eFileInitError:
            //m_File.SetError ( e_ERROR_CREATE_FILE, 0 );
            m_File.SetError(0, "Create file error, file name is %S", &m_FileName);
            break;
        case eFileIntegrityError:
            //m_File.SetError ( e_FILE_INTEGRITY_ERROR, 0 );
            m_File.SetError(0, "File integrity error, file name is %S", &m_FileName);
            break;
        case eFileFormatKeyDataError:
            //m_File.SetError ( e_FILE_KEY_DATA_FORMAT_ERROR, 0 );
            m_File.SetError(0, "File key data format error, file name is %S", &m_FileName);
            break;
        }
        throw n + __LINE__ * 1000;
    }
    int GetStatus () const noexcept
    {
        return m_Status;
    }
    const char * GetStatusMsg ()
    {
        if  ( m_Status < 0 || m_Status >= eStatusMax )
        {
            return "Unknown status";
        }
        static const char * m [ 6 ] = { "eFileCreateError", "eFileInitError", "eFileIntegrityError", "eFileFormatKeyDataError", "File was opened", "File was created" };
        return  m [ m_Status ];
    }
    void Flush ()
    {
        if  ( m_File.IsOpen () )
        {
            m_File.Flush ();
        }
    }

    virtual void Swap ( CMaaUnivFileHash < Key, Data > & That )
    {
        m_File.Close ();
        That.m_File.Close ();
        SwapFiles ( m_FileName, That.m_FileName );
        CMaaSwap ( h,         That.h );
        CMaaSwap ( m_fIsOK,   That.m_fIsOK );
        CMaaSwap ( m_Status,  That.m_Status );
        m_File = CMaaFile ( m_FileName, CMaaFile::eRp_SrSw );
        That.m_File = CMaaFile ( That.m_FileName, CMaaFile::eRp_SrSw );
    }

    virtual CMaaUnivFileHash < Key, Data > * MakeNewTable ( const CMaaString &FileName, _qword Size, int Flags )
    {
        return TL_NEW CMaaUnivFileHash < Key, Data > ( FileName, Size, Flags );
    }

    void SetDeleteOnClose ( BOOL fDelete = TRUE ) noexcept
    {
        m_fDeleteOnClose = fDelete;
    }

    enum eFlags
    {
        eCreate            = 0x01,
        eDeleteOnClose     = 0x02
    };

    static bool RemoveFiles(const CMaaString &FileName)
    {
        bool bRes = true;
        try
        {
            CMaaUnivFileHash tmp(FileName, 16, eCreate | eDeleteOnClose);
        }
        catch(...)
        {
            bRes = false;
        }
        return bRes;
    }

    CMaaUnivFileHash ( const CMaaString &FileName = "hashtable.dat", _qword Size = 16, int Flags = 0 )
    //:   m_File ( ( const char * ) nullptr )
    {
        m_FileName = FileName;
        m_fDeleteOnClose = ( Flags & eDeleteOnClose ) != 0;
        try
        {
            if  ( Flags & eCreate )
            {
                CMaaFile::Remove ( /*(const char*)*/m_FileName.z_str(), false);
            }
            else
            {
                m_File = CMaaFile ( FileName, CMaaFile::eRp_SrSw );
                m_Status = eFileOpened;
            }
        }
        catch( XTOOFile2Error err)
        {
            //__utf8_printf("step1: %s\n", err.GetMsg());
        }
        try
        {
            if  ( !m_File.IsOpen () )
            {
                //__utf8_printf("step2\n");
                m_File = CMaaFile ( FileName, CMaaFile::eRWCD_SrSw );
                //__utf8_printf("step3: opened\n");
                m_Status = eFileCreated;
                //__utf8_printf("step4: initializing\n");
                InitHTFile ( Size );
            }
        }
        catch( XTOOFile2Error err)
        {
            //__utf8_printf("step5: %s\n", err.GetMsg());
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
            //__utf8_printf("step6: closed\n");
            m_fIsOK = FALSE;
            m_Status = eFileCreateError;
            return;
        }

        char txt [ MAA_HT_OFFSET0 ];
        m_File.Seek ( 0 );
        //__utf8_printf("step7:\n");
        if  ( m_File.Read ( txt, MAA_HT_OFFSET0 ) != MAA_HT_OFFSET0 || !CMaaIsMemEq( txt, pszHeader, MAA_HT_OFFSET0 ) )
        {
            int nb = 0;
            for (nb = 0; nb < MAA_HT_OFFSET0 && txt[nb] == pszHeader[nb]; ) nb++;
            nb = nb < MAA_HT_OFFSET0 ? nb : -1;
            //__utf8_printf("step8: error at byte %d\n", nb);
            m_fIsOK = FALSE;
            m_Status = eFileIntegrityError;
            return;
        }
        //m_File.Seek ( MAA_HT_OFFSET0 );
        if  ( m_File.Read ( &h, sizeof ( h ) ) != sizeof ( h ) )
        {
            //__utf8_printf("step9:\n");
            m_fIsOK = FALSE;
            m_Status = eFileIntegrityError;
            return;
        }
        if  ( h.m_TableSize < 2 || h.m_TableSize > (_qword)(0x7ffffffffffffffLL / ( sizeof ( Record ) + sizeof ( _qword ) )) ||
             h.m_HashSize < h.m_TableSize || h.m_HashSize > (_qword)(0x7ffffffffffffffLL / ( sizeof ( Record ) + sizeof ( _qword ) )) ||
             ( h.m_Flags & 1 ) ||
             ( h.m_FreeList != MAA_HT_NULL && !IsValidRecN ( h.m_FreeList ) ) )
        {
            //__utf8_printf("step10:\n");
            m_fIsOK = FALSE;
            h.m_Flags |= 2;
            m_Status = eFileIntegrityError;
            return;
        }
        if  ( h.m_KeySize != sizeof ( Key ) || h.m_DataSize != sizeof ( Data ) )
        {
            //__utf8_printf("step11:\n");
            m_fIsOK = FALSE;
            h.m_Flags |= 2;
            //UpdateHeader ();
            m_Status = eFileFormatKeyDataError;
            return;
        }
        if  ( m_File.Length() != (_qword)(sizeof ( h ) + MAA_HT_OFFSET2 + h.m_HashSize * sizeof ( _qword ) + h.m_TableSize * sizeof ( Record )) )
        {
            //__utf8_printf("step12: %D != %D\n", (_qword)m_File.Length(), (_qword)(sizeof ( h ) + MAA_HT_OFFSET2 + h.m_HashSize * sizeof ( _qword ) + h.m_TableSize * sizeof ( Record )));
            //__utf8_printf("(sizeof(h) = %D, MAA_HT_OFFSET2=%d, h.m_HashSize(%D) * sizeof(_qword) (%D) + h.m_TableSize(%D) * sizeof(Record)(%D) (%D)))\n",
            // (_qword)sizeof(h), (int)MAA_HT_OFFSET2, (_qword)h.m_HashSize, (_qword)(h.m_HashSize * sizeof(_qword)), (_qword)h.m_TableSize, (_qword)(sizeof(Record)), (_qword)(h.m_TableSize * sizeof(Record)));
            m_fIsOK = FALSE;
            h.m_Flags |= 2;
            //UpdateHeader ();
            m_Status = eFileIntegrityError;
            return;
        }
        //__utf8_printf("___ possible ok\n");
        // other (data structute) integrity is leaving unchecked
        m_fIsOK = TRUE;
    }

protected:
    void InitHTFile ( _qword Size )
    {
        if  ( Size < 2 || Size > (0x7ffffffffffffffLL / (_qword)(sizeof(_qword) + sizeof(Record))) )
        {
            Size = 2;
        }
        memset ( &h, 0, sizeof ( h ) );
        h.m_HashSize   = Size + Size / 10;
        h.m_TableSize  = Size;
        h.m_KeySize    = sizeof ( Key );
        h.m_DataSize   = sizeof ( Data );
        h.m_FreeList   = 0;
        h.m_Flags      = 1;
        h.m_ItemCount  = 0;
        m_File.Seek ( 0 );
        m_File.WriteVerify ( pszHeader, 16 );
        m_File.WriteVerify ( &h, sizeof ( h ) );
        m_File.WriteVerify ( pszIndex, 16 );

        const _qword x = MAA_HT_NULL;
        _qword i;
        for ( i = 0; i < h.m_HashSize; i++ )
        {
            m_File.WriteVerify ( &x, sizeof ( x ) );
        }
        m_File.WriteVerify ( pszItems, 16 );

        Record r;
        InitKeyAndData ( r.K, r.D );
        for ( i = 1; i < h.m_TableSize; i++ )
        {
            r.Next = i;
            m_File.WriteVerify ( &r, sizeof ( r ) );
        }
        r.Next = MAA_HT_NULL;
        m_File.WriteVerify ( &r, sizeof ( r ) );
        m_File.Truncate ();

        h.m_Flags = 0;
        UpdateHeader ();
        m_File.Flush ();
    }

public:
    BOOL IsOK () const noexcept
    {
        return m_fIsOK;
    }

    virtual ~CMaaUnivFileHash () noexcept
    {
        // g_dbg += "~CMaaUnivFileHash () for "; g_dbg += m_FileName; g_dbg += "\n";
        if  ( m_fDeleteOnClose )
        {
            // g_dbg += "Deleting on close "; g_dbg += m_FileName; g_dbg += "\n";
            try
            {
                m_File.Close ();
            }
            catch(...)
            {
            }
            try
            {
                CMaaFile::Remove ( m_FileName, false );
            }
            catch(...)
            {
                // g_dbg += "catch(...)\n";
            }
            // g_dbg += "end delete on close\n";
        }
    }

    void Cleanup ( _qword NewTableSize = 0 )
    {
        if  ( !IsOK () )
        {
            ThrowError ( 5 );
        }
        CMaaString tmpfn = m_FileName + ".tmp";
        CMaaFile::Remove ( tmpfn, false );
        CMaaUnivFileHash < Key, Data > * NewTable = MakeNewTable ( tmpfn, NewTableSize ? NewTableSize : h.m_TableSize, eCreate | eDeleteOnClose );
        if  ( NewTable )
        {
            try
            {
                BOOL fDel = m_fDeleteOnClose;
                Swap ( *NewTable );
                SetDeleteOnClose ( fDel );
                NewTable->SetDeleteOnClose ();
            }
            catch(...)
            {
                delete NewTable;
                CMaaFile::Remove ( tmpfn, false );
                throw;
            }
            delete NewTable;
        }
        CMaaFile::Remove ( tmpfn, false );
    }

    _qword GetMaxIndex () const
    {
        return h.m_HashSize;
    }

    // returns 0 for success
    /*
     int FindByIndex ( int Index, Key * K = nullptr, Data * D = nullptr ) const
     {
          if   ( Index >= 0 && Index < m_HashSize )
          {
               if   ( m_Hash [ Index ] )
               {
                    if   ( K )
                    {
                         *K = m_Hash [ Index ] -> K;
                    }
                    if   ( D )
                    {
                         *D = m_Hash [ Index ] -> D;
                    }
                    return 0;
               }
          }
          return 1;
     }
     */

protected:
    void SeekToRec ( _qword n )
    {
        if  ( IsValidRecN ( n ) )
        {
            m_File.Seek ( sizeof ( h ) + MAA_HT_OFFSET2 + h.m_HashSize * sizeof ( _qword ) + n * sizeof ( Record ) );
        }
        else
        {
            m_Status = eFileIntegrityError;
            h.m_Flags |= 2;
            UpdateHeader ();
            //m_File.SetError ( e_FILE_INTEGRITY_ERROR, 0 );
            m_File.SetError(0, "File integrity error");
            throw 1 + __LINE__ * 1000;
        }
    }
    void GetRec ( _qword n, Record &r )
    {
        SeekToRec ( n );
        m_File.Read ( &r, sizeof ( r ) );
    }
    void SetRec ( _qword n, const Record &r )
    {
        SeekToRec ( n );
        m_File.Write ( &r, sizeof ( r ) );
    }

    _qword GetIndex ( _qword n )
    {
        _qword i = MAA_HT_NULL;
        if  ( n >= 0 && n < h.m_HashSize )
        {
            m_File.Seek ( sizeof ( h ) + MAA_HT_OFFSET1 + n * sizeof ( _qword ) );
            m_File.Read ( &i, sizeof ( i ) );
        }
        else
        {
            m_Status = eFileIntegrityError;
            h.m_Flags |= 2;
            UpdateHeader ();
            //m_File.SetError ( e_FILE_INTEGRITY_ERROR, 0 );
            m_File.SetError(0, "File integrity error");
            throw 2 + __LINE__ * 1000;
        }
        return i;
    }
    void SetIndex ( _qword n, _qword i )
    {
        if  ( n >= 0 && n < h.m_HashSize )
        {
            m_File.Seek ( sizeof ( h ) + MAA_HT_OFFSET1 + n * sizeof ( _qword ) );
            m_File.Write ( &i, sizeof ( i ) );
        }
        else
        {
            m_Status = eFileIntegrityError;
            h.m_Flags |= 2;
            UpdateHeader ();
            //m_File.SetError ( e_FILE_INTEGRITY_ERROR, 0 );
            m_File.SetError(0, "File integrity error");
            throw 2 + __LINE__ * 1000;
        }
    }
    void UpdateHeader ()
    {
        m_File.Seek ( MAA_HT_OFFSET0 );
        m_File.Write ( &h, sizeof ( h ) );
    }

public:
    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add    ( const Key & K, const Data & D, int fOverwrite = 0 )
    {
        if  ( !IsOK () )
        {
            ThrowError ( 5 );
        }
        int Ret = 0; // 0: IsOk, 1,2: IsFail,  3 - file error
        const _qword lFree = h.m_FreeList;
        if  ( lFree == MAA_HT_NULL )
        {
            // Expand the table
            const BOOL fDel = m_fDeleteOnClose;
            CMaaString tmpfn = m_FileName + ".tmp";
            CMaaFile::Remove ( tmpfn, false );
            CMaaUnivFileHash < Key, Data > * NewTable = MakeNewTable ( tmpfn, 2 * h.m_TableSize, eCreate | eDeleteOnClose );
            if  ( !NewTable || !NewTable->IsOK () )
            {
                Ret = 2; // No free elements
            }
            else
            {
                try
                {
                    SeekToRec ( 0 );
                    Record r;
                    for ( _qword i = 0; i < h.m_TableSize; i++ )
                    {
                        m_File.Read ( &r, sizeof ( r ) );
                        NewTable->Add ( r.K, r.D );
                    }
                    Ret = NewTable->Add ( K, D, fOverwrite );
                    Swap ( *NewTable );
                    SetDeleteOnClose ( fDel ); // to make sure
                    NewTable->SetDeleteOnClose ();
                }
                catch(...)
                {
                    Ret = 3;
                    //delete NewTable;
                    //throw;
                }
                delete NewTable;
            }
            CMaaFile::Remove ( tmpfn, false );
        }
        else
        {
            const _qword n = (_qword)(HashFunction(K) % (_uqword)h.m_HashSize);
            const _qword idx = GetIndex ( n );
            if  ( idx == MAA_HT_NULL )
            {
                // Creating TL_NEW node
                SetIndex ( n, lFree );
                Record r;
                GetRec ( lFree, r );
                h.m_FreeList = r.Next;
                h.m_ItemCount++;
                r.Next = MAA_HT_NULL;
                r.K = K;
                r.D = D;
                SetRec ( lFree, r );
                UpdateHeader ();
            }
            else
            {
                Record r;
                _qword i = 0;
                _qword idx2 = idx;
                // searching if Key alredy exists
                while( idx2 != MAA_HT_NULL )
                {
                    if  ( i++ > h.m_TableSize )
                    {
                        m_Status = eFileIntegrityError;
                        h.m_Flags |= 2;
                        UpdateHeader ();
                        //m_File.SetError( e_INFINITE_LOOP__FILE_INT_ERR, 0 );
                        m_File.SetError(0, "Infinite loop file int.error");
                        throw 4 + __LINE__ * 1000; // infinite loop
                    }
                    GetRec ( idx2, r );
                    if  ( IsEqual ( r.K, K ) )
                    {
                        if  ( ! fOverwrite )
                        {
                            Ret = 1; // Key alredy exists
                            break;
                        }
                        r.D = D;
                        SetRec ( idx2, r );
                        return 0;
                    }
                    idx2 = r.Next;
                }
                if  ( ! Ret )
                {
                    GetRec ( lFree, r );
                    h.m_FreeList = r.Next;
                    h.m_ItemCount++;
                    r.K = K;
                    r.D = D;
                    r.Next = idx;
                    SetRec ( lFree, r );
                    SetIndex ( n, lFree );
                    UpdateHeader ();
                }
            }
        }
        return Ret;

    }

    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    int AddOver ( const Key & K, const Data & D )
    {
        return Add ( K, D, 1 );
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find   ( const Key & K, Data * D = nullptr )
    {
        //__utf8_printf("Find() point1\n");
        if  ( !IsOK () )
        {
            //__utf8_printf("Find() point2\n");
            ThrowError ( 5 );
        }
        int Ret = 1; // 0: IsOk, 1: Key was not found

        //__utf8_printf("Find() point3\n");
        const _qword n = (_qword)(HashFunction(K) % (_uqword)h.m_HashSize);
        //__utf8_printf("Find() point3\n");
        _qword idx = GetIndex ( n );
        Record r;
        _qword i = 0;
        while( idx != MAA_HT_NULL )
        {
            //__utf8_printf("Find() point3: %D %D\n", i, idx);
            if  ( i++ > h.m_TableSize )
            {
                //__utf8_printf("Find() point3.2: %D %D %D\n", i, idx, (_qword)h.m_TableSize);
                m_Status = eFileIntegrityError;
                h.m_Flags |= 2;
                UpdateHeader ();
                //m_File.SetError ( e_INFINITE_LOOP__FILE_INT_ERR, 0 );
                m_File.SetError(0, "Infinite loop file int.error");
                throw 4 + __LINE__ * 1000; // infinite loop
            }
            //__utf8_printf("Find() point4\n");
            GetRec ( idx, r );
            //__utf8_printf("Find() point5\n");
            if  ( IsEqual ( r.K, K ) )
            {
                if  ( D )
                {
                    *D = r.D;
                }
                Ret = 0; // Key was found
                break;
            }
            idx = r.Next;
        }
        return Ret;
    }

    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to rerurn deleted context of Data
    int Remove ( const Key & K, Data * D = nullptr )
    {
        if  ( !IsOK () )
        {
            ThrowError ( 5 );
        }
        int Ret = 1; // 0: IsOk, 1: Key was not found

        const _qword n = (_qword)(HashFunction(K) % (_uqword)h.m_HashSize);
        _qword idx = GetIndex ( n );
        _qword idx_p = 0/*just for uninitialized var is used warning removing*/;
        Record r, p;
        _qword i = 0;
        while( idx != MAA_HT_NULL )
        {
            if  ( i++ > h.m_TableSize )
            {
                m_Status = eFileIntegrityError;
                h.m_Flags |= 2;
                UpdateHeader ();
                //m_File.SetError ( e_INFINITE_LOOP__FILE_INT_ERR, 0 );
                m_File.SetError(0, "Infinite loop file int.error");
                throw 4 + __LINE__ * 1000; // infinite loop
            }
            GetRec ( idx, r );
            if  ( IsEqual ( r.K, K ) )
            {
                if  ( D )
                {
                    *D = r.D;
                }
                if  ( i > 1 )
                {
                    p.Next = r.Next;
                    SetRec ( idx_p, p );
                }
                else
                {
                    SetIndex ( n, r.Next );
                }
                r.Next = h.m_FreeList;
                InitKeyAndData ( r.K, r.D );
                h.m_FreeList = idx;
                SetRec ( idx, r );
                h.m_ItemCount--;
                UpdateHeader ();
                Ret = 0; // Key was found
                break;
            }
            p = r;
            idx_p = idx;
            idx = r.Next;
        }
        return Ret;
    }

    /*
     Data * operator [] ( const Key & K )
     {
          Record ** Hash = m_Hash + (_qword)(HashFunction(K) % (_uqword)h.m_HashSize);
          Record  * This = * Hash;
          if   ( This )
          {
               // searching for Key
               for  ( ; This; This = This -> pNext )
               {
                    if   ( This -> K == Key )
                    {
                         return & This -> D;
                    }
               }
          }
          return nullptr;
     }
     */

    /*
     // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
     int EnumeratePtrs ( int Size, Key ** Keys, Data ** Datas ) const
     {
          if   ( Size >= 0 )
          {
               int j = 0;
               for  ( int i = 0; i < m_HashSize; i++ )
               {
                    Record * This = m_Hash [ i ];
                    if   ( This )
                    {
                         // scaning all Keys
                         for  ( ; This; This = This -> pNext )
                         {
                              if   ( j >= Size )
                              {
                                   return -2;
                              }
                              if   ( Keys )
                              {
                                   Keys  [ j ] = & This -> K;
                              }
                              if   ( Datas )
                              {
                                   Datas [ j ] = & This -> D;
                              }
                              j++;
                         }
                    }
               }
               return j;
          }
          return -1;
     }

     // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
     int EnumerateItems ( int Size, Key * Keys, Data * Datas ) const
     {
          if   ( Size >= 0 )
          {
               int j = 0;
               for  ( int i = 0; i < m_HashSize; i++ )
               {
                    Record * This = m_Hash [ i ];
                    if   ( This )
                    {
                         // scaning all Keys
                         for  ( ; This; This = This -> pNext )
                         {
                              if   ( Keys )
                              {
                                   if   ( j >= Size )
                                   {
                                        return -2;
                                   }
                                   * Keys = This -> K;
                                   Keys++;
                              }
                              if   ( Datas )
                              {
                                   if   ( j >= Size )
                                   {
                                        return -2;
                                   }
                                   * Datas = This -> D;
                                   Datas++;
                              }
                              j++;
                         }
                    }
               }
               return j;
          }
          return -1;
     }
     */

    _qword GetItemCount () const
    {
        //return EnumerateItems ( 0x7fffffff, nullptr, nullptr );
        return h.m_ItemCount;
    }

    _qword GetDBSize ()
    {
        return m_File.Length(); //ul
    }

    _qword GetItemCountEx ()
    {
        _qword n = 0;
        EnumerateByProc ( &CountProc, &n );
        return n;
    }

    typedef void ( *EnumerateProc )( Key & K, Data & D, void * Param );
    typedef void ( *EnumerateProcEx )( Key & K, Data & D, CMaaUnivFileHash < Key, Data > & ht, void * Param );

    void EnumerateByProc ( EnumerateProc Proc, void * Param = nullptr )
    {
        Record r;
        for (_qword i = 0; i < h.m_HashSize; i++)
        {
            _qword n = GetIndex ( i );
            _qword c = 0;
            while( n != MAA_HT_NULL )
            {
                if  ( c++ > h.m_TableSize )
                {
                    //m_File.SetError ( e_INFINITE_LOOP__FILE_INT_ERR, 0 );
                    m_File.SetError(0, "Infinite loop file int.error");
                    throw 4 + __LINE__ * 1000;
                }
                GetRec ( n, r );
                n = r.Next;
                Proc ( r.K, r.D, Param );
            }
        }
    }

    void EnumerateByProc ( EnumerateProcEx Proc, void * Param = nullptr )
    {
        Record r;
        for (_qword i = 0; i < h.m_HashSize; i++)
        {
            _qword n = GetIndex ( i );
            _qword c = 0;
            while( n != MAA_HT_NULL )
            {
                if  ( c++ > h.m_TableSize )
                {
                    //m_File.SetError ( e_INFINITE_LOOP__FILE_INT_ERR, 0 );
                    m_File.SetError(0, "Infinite loop file int.error");
                    throw 4 + __LINE__ * 1000;
                }
                GetRec ( n, r );
                n = r.Next;
                Proc ( r.K, r.D, *this, Param );
            }
        }
    }
};
//---------------------------------------------------------------------------
#undef pszItems
#undef pszIndex
#undef pszHeader
#undef MAA_HT_OFFSET2
#undef MAA_HT_OFFSET1
#undef MAA_HT_OFFSET0
#undef MAA_HT_NULL  // -2
//---------------------------------------------------------------------------

class CMaaMessageDB
{
    struct SqueezeStruct
    {
        char * m_Buffer;
        _dword m_BufferSize;
        CMaaMessageDB * m_pGetFrom, * m_pAddTo;
    };
    static void SqeezeProc ( _qword &i, _qword &o, void * p );
public:
    struct Header
    {
        _qword m_Size;
        _qword m_FilledSize;
        _qword m_Reserved [ 2 ];
    } ; //__attribute__((packed));

    Header h;

    struct MHeader // MessageHeader
    {
        _qword m_ReservedSize;
        _qword m_Size;
        _qword m_ReservedIndex;
        _qword m_Signature;

        enum eSignature
        {
            //eValid   = 0x12345678,
            eValid   = 'm' + ( 's' << 8 ) + ( 'g' << 16 ) + ( '>' << 24 ), // "msg>"
            eDeleted = 'd' + ( 'e' << 8 ) + ( 'l' << 16 ) + ( '>' << 24 )  // "del>"
        };
    } ; //__attribute__((packed));

    CMaaString m_FileName;
    _qword m_MaxSize;
    CMaaFile m_File;
    BOOL m_fIsOK;
    BOOL m_fAllowMsgOverlap;
    BOOL m_fDeleteOnClose;
    _dword m_MaxMsgSize;

    enum eStatus
    {
        eFileCreateError = 0,
        eFileInitError,
        eFileIntegrityError,
        eFileFormatKeyDataError, // not used in this class
        eFileOpened,
        eFileCreated,

        eStatusMax
    };
    int m_Status;

    enum eFlags
    {
        eCreate            = 0x01,
        eDeleteOnClose     = 0x02,
        eAllowMsgOverlap   = 0x04
    };

    class HashTable : public CMaaUnivFileHash < _qword, _qword >
    {
        CMaaMessageDB * m_pmdb;
        int m_Type;
    public:
        HashTable ( CMaaMessageDB * pMessageDB, const CMaaString &FileName, int Type = 0, _qword Size = 4, int Flags = 0 )
        :   CMaaUnivFileHash < _qword, _qword > ( FileName, Size, Flags ),
            m_pmdb ( pMessageDB ),
            m_Type ( Type )
        {
        }
        void InitKeyAndData ( _qword & K, _qword & D )
        {
            K = -1;
            D = -1;
        }
        bool IsLooksValid ( _qword & K, _qword & D )
        {
            if  ( K == -1 && D == -1 )
            {
                return false;
            }
            if  ( m_pmdb )
            {
                return m_pmdb->IsValidOffset ( !m_Type ? D : K );
            }
            return true;
        }
    };

    HashTable m_Index;

    void UpdateHeader ();
    void ReadHeader (); // re-reads header
    bool IsValidOffset ( _qword o );
    bool IsValidOffset ( _qword o, MHeader &mh );

    //-----------------------------------------
    //                 Methods
    //-----------------------------------------

    CMaaMessageDB ( const CMaaString &FileName, _dword MaxMsgSize, int Flags = CMaaMessageDB::eAllowMsgOverlap, _qword MaxSize = -1 );
    virtual ~CMaaMessageDB ();
    static bool RemoveFiles(const CMaaString &FileName);
    void SetDeleteOnClose ( BOOL fDelete = TRUE );
    void Flush (); // Flush 2 files
    bool IsNeedSqeeze (); // is sqeeze will be effective

    // 0 - ok, 1 - database size limit, != 0 - error(s)
    int Add(_qword Index, const void * ptr, _dword Size);
    int Add(_qword Index, const CMaaString &txt)
    {
        return Add(Index, txt, (warning_dword)txt.Length());
    }

    // 0 - ok, 1 - not found, 2 - offset is not valid, 3 - file error, (!= 0 - error(s))
    int Remove ( _qword Index );

    // 0 - ok, 1 - not found, 2 - offset is not valid, 3 - file error, -1 - buffer is too small, (!= 0 - error(s))
    int Get(_qword Index, void * Buffer, _dword * BufferSize);
    int Get(_qword Index, CMaaString &txt, _dword MaxDataSize = 10 * 1024); // -1 for defaut max size

    // -1 - error, N >= 0 - N bytes released
    _qword Squeeze ();

    void Swap ( CMaaMessageDB & That );

    _qword GetDBSize ()
    {
        return (m_File.Length () + m_Index.GetDBSize ()); // ul
    }

};


//#if
template <class Data> class CMaaStr2FixedDataHashOneTable : protected CMaaUnivFileHash<_qword, Data>
{
    typedef CMaaString Key;
    // Data: struct Data {_qword index, SomeData d};
    typedef void (*EnumerateProcEx)(_qword &K, Data &D, void * Param);

    CMaaMessageDB * m_db;

    Key m_Key1;
    _qword m_index0;

    _uqword HashFunction(const _qword & K) const
    {
        CMaaString txt;
        if  (K == -1)
        {
            txt = m_Key1;
        }
        else if (!m_db->Get(K, txt))
        {
        }
        //return txt.ToUpper().Hash64();
        return txt.Hash64();
    }

    _bool IsEqual(const _qword &K1, const _qword &K2)
    {
        Key k1, k2;

        if  (K1 == -1)
        {
            k1 = m_Key1;
        }
        else
        {
            if  (m_db->Get(K1, k1))
            {
                return _false;
            }
        }

        if  (K2 == -1)
        {
            k2 = m_Key1;
        }
        else
        {
            if  (m_db->Get(K2, k2))
            {
                return _false;
            }
        }

        //return k1.ToUpper() == k2.ToUpper();
        return k1 == k2;
    }

    CMaaUnivFileHash <_qword, Data> * MakeNewTable(const CMaaString &FileName, _qword Size, int Flags) override
    {
        CMaaStr2FixedDataHashOneTable<Data> * p = TL_NEW CMaaStr2FixedDataHashOneTable<Data>(m_db, FileName, Flags, Size);
        if  (p)
        {
            p->m_Key1 = m_Key1;
        }
        return p;
    }

    struct sParam
    {
        //CMaaStringFileHash<Data> &ht;
        EnumerateProcEx Proc;
        void * Param;
        sParam(/*CMaaStringFileHash<Data> &h,*/ EnumerateProcEx ProcEx, void * ptr)
        :   //ht(h),
            Proc(ProcEx),
            Param(ptr)
        {
        }
    };

    static void EnumerateProc1(_qword &K, Data &D, void * Param)
    {
        sParam &p = *(sParam *)Param;
        p.Proc(K, D, p.Param);
    }

public:
    CMaaStr2FixedDataHashOneTable(CMaaMessageDB * db, const CMaaString &FileName, int Flags = CMaaMessageDB::eAllowMsgOverlap, _qword MaxSize = -1)
    :   CMaaUnivFileHash<_qword, Data>(FileName + ".ht", MaxSize, Flags),
        m_db(db)
    {
        m_index0 = 0;
    }

    ~CMaaStr2FixedDataHashOneTable()
    {
        CMaaUnivFileHash<_qword, Data>::Flush();
        m_db->Flush();
    }

    void EnumerateByProc(EnumerateProcEx Proc, void * Param = nullptr)
    {
        sParam param(Proc, Param);
        CMaaUnivFileHash<_qword, Data>::EnumerateByProc(EnumerateProc1, &param);
    }

    int Add(const Key &K, const Data &D, int fOverwrite = 0)
    {
        Data d2;
        if  (Find(K, &d2) == 0)
        {
            if  (d2 == D)
            {
                return fOverwrite ? 0 : 1;
            }
            else
            {
                if  (!fOverwrite)
                {
                    return 1;
                }
                //D.index = d2.index;
                _qword tmp = d2.index;
                d2 = D;
                d2.index = tmp;
                //                    if   (d2.index == D.index)
                {
                    return CMaaUnivFileHash<_qword, Data>::Add(d2.index, D, fOverwrite);
                }
                /*
                    else
                    {
                         Remove(K);
                         //CMaaUnivFileHash<_qword, Data>::Remove(d2.index);
                         //m_db.Remove(d2.index);
                    }
*/
            }
        }
        int r = -1;
        while(1)
        {
            m_index0++;
            CMaaString temp;
            if  (m_db->Get(m_index0, temp, (_dword)-1))
            {
                r = m_db->Add(m_index0, K);
                //	       logfile.fprintf("CMaaStringFileHash_2::Add(): key->Add(%D,%S) returns %d\n", m_index0, &K, r);
                if  (!r)
                {
                    d2 = D;
                    d2.index = m_index0;
                    break;
                }
            }
        }
        //int r = m_db->Add(D.index, K);
        if  (!r)
        {
            r = CMaaUnivFileHash<_qword, Data>::Add(d2.index, d2, fOverwrite);
            if  (r)
            {
                m_db->Remove(d2.index);
            }
        }
        return r;
    }

    int AddOver(const Key &K, const Data &D)
    {
        return Add(K, D, 1);
    }

    int Find(const Key &K, Data *D = nullptr)
    {
        m_Key1 = K;
        return CMaaUnivFileHash<_qword, Data>::Find(-1, D);
    }

    int Remove(const Key &K, Data *D = nullptr)
    {
        m_Key1 = K;
        Data d2;
        int r = CMaaUnivFileHash<_qword, Data>::Remove(-1, &d2);
        if  (!r)
        {
            m_db->Remove(d2.index);
            if  (D)
            {
                *D = d2;
            }
        }
        return r;
    }

    static bool RemoveFiles(const CMaaString &FileName)
    {
        bool bRes = true;
        try
        {
            CMaaStr2FixedDataHashOneTable tmp(FileName, 10 * 1024, CMaaUnivFileHash<_qword, Data>::eCreate | CMaaUnivFileHash<_qword, Data>::eDeleteOnClose);
        }
        catch(...)
        {
            bRes = false;
        }
        return bRes;
    }
};

template <class Data> class CMaaStr2FixedDataHash
{
public:
    typedef CMaaString Key;
    struct DiskData
    {
        _qword index;
        Data d;
        DiskData()
        {
            //uninitialized
        }
        DiskData(const DiskData &That)
        {
            *this = That;
        }
        DiskData & operator=(const DiskData &That)
        {
            index = That.index;
            d = That.d;
            return *this;
        }
        bool operator == (const DiskData &That) const
        {
            return d == That.d;
        }
        bool operator != (const DiskData &That) const
        {
            return d != That.d;
        }
    } ; //__attribute__((packed));

    typedef void(*EnumerateProcEx)(Key &K, Data &D, CMaaStr2FixedDataHash<Data> &ht, void * Param);

private:
    /*
     struct Data2
     {
          _qword index;
          Data d;
     };
     */

    CMaaMessageDB m_db;
    CMaaStr2FixedDataHashOneTable<DiskData> m_h;

    struct sParam
    {
        CMaaStr2FixedDataHash<Data> &ht;
        EnumerateProcEx Proc;
        void * Param;
        sParam(CMaaStr2FixedDataHash<Data> &h, EnumerateProcEx ProcEx, void * ptr)
        :   ht(h),
            Proc(ProcEx),
            Param(ptr)
        {
        }
    };

    static void EnumerateProc1(_qword &/*K*/, DiskData &D, void * Param)
    {
        sParam &p = *(sParam *)Param;
        CMaaString txt;
        if  (!p.ht.m_db.Get(D.index, txt))//, 10 * 1024);
        {
            p.Proc(txt, D.d, p.ht, p.Param);
        }
    }

public:
    CMaaStr2FixedDataHash(const CMaaString &FileName, _dword MaxMsgSize = 10 * 1024, int Flags = CMaaMessageDB::eAllowMsgOverlap, _qword MaxSize = -1)
    :   m_db(FileName, MaxMsgSize, Flags, MaxSize),
        m_h(&m_db, FileName + ".ht", Flags, MaxSize)
    {
    }

    void EnumerateByProc(EnumerateProcEx Proc, void * Param = nullptr)
    {
        sParam param(*this, Proc, Param);
        m_h.EnumerateByProc(EnumerateProc1, &param);
    }

    int Add(const Key &K, const Data &D, int fOverwrite = 0)
    {
        DiskData _data;
        _data.index = 0;
        _data.d = D;
        return m_h.Add(K, _data, fOverwrite);
    }

    int AddOver(const Key &K, const Data &D)
    {
        return Add(K, D, 1);
    }

    int Find(const Key &K, Data *D = nullptr)
    {
        DiskData _data;
        int r = m_h.Find(K, D ? &_data : nullptr);
        if  (!r && D)
        {
            *D = _data.d;
        }
        return r;
    }

    int Remove(const Key &K, Data *D = nullptr)
    {
        DiskData _data;
        int r = m_h.Remove(K, D ? &_data : nullptr);
        if  (!r && D)
        {
            *D = _data.d;
        }
        return r;
    }

    int Get(_qword Index, CMaaString &txt, _dword MaxDataSize = (_dword)-1)
    {
        /*
          if   (MaxDataSize == (_dword)-1)
          {
               MaxDataSize = m_db.m_MaxMsgSize;
          }
          */
        return m_db.Get(Index, txt, MaxDataSize);
    }

    static bool RemoveFiles(const CMaaString &FileName)
    {
        bool bRes = true;
        try
        {
            CMaaStr2FixedDataHash<Data> tmp(FileName, 10 * 1024, CMaaMessageDB::eCreate | CMaaMessageDB::eDeleteOnClose);
        }
        catch(...)
        {
            bRes = false;
        }
        return bRes;
    }
};
//#endif

class CMaaStringFileHash_2 : protected CMaaUnivFileHash<_qword, _qword>
{
    typedef CMaaString Key;
    typedef CMaaString Data;
    typedef _qword Index;
    typedef void (*EnumerateProcEx)(CMaaString &K, Data &D, void * Param);

    CMaaMessageDB * m_key_db, * m_data_db;

    Key m_Key1;

    Index m_index0;

    _uqword HashFunction(const _qword & K) const
    {
        CMaaString txt;
        if  (K == -1)
        {
            txt = m_Key1;
        }
        else if (!m_key_db->Get(K, txt, (_dword)-1))
        {
        }
        //          return txt.ToUpper().Hash64();
        return txt.Hash64();
    }

    _bool IsEqual(const _qword &K1, const _qword &K2)
    {
        Key k1, k2;

        if  (K1 == -1)
        {
            k1 = m_Key1;
        }
        else
        {
            if  (m_key_db->Get(K1, k1, (_dword)-1))
            {
                return _false;
            }
        }

        if  (K2 == -1)
        {
            k2 = m_Key1;
        }
        else
        {
            if  (m_key_db->Get(K2, k2, (_dword)-1))
            {
                return _false;
            }
        }

        //          return k1.ToUpper() == k2.ToUpper();
        return k1 == k2;
    }

    CMaaUnivFileHash <_qword, _qword> * MakeNewTable(const CMaaString &FileName, _qword Size, int Flags) override
    {
        CMaaStringFileHash_2 * p = TL_NEW CMaaStringFileHash_2(m_key_db, m_data_db, FileName, Flags, Size);
        if  (p)
        {
            p->m_Key1 = m_Key1;
        }
        return p;
    }

    struct sParam
    {
        //CMaaStringFileHash<Data> &ht;
        EnumerateProcEx Proc;
        void * Param;
        CMaaStringFileHash_2 * m_pHash;
        sParam(/*CMaaStringFileHash<Data> &h,*/ EnumerateProcEx ProcEx, void * ptr, CMaaStringFileHash_2 * pHash) noexcept
        :   //ht(h),
            Proc(ProcEx),
            Param(ptr),
            m_pHash(pHash)
        {
        }
    };

    static void EnumerateProc1(_qword &K, _qword &/*D*/, void * Param)
    {
        sParam &p = *(sParam *)Param;
        Key k;
        Data d;
        p.m_pHash->m_key_db->Get(K, k, (_dword)-1);
        p.m_pHash->m_data_db->Get(K, d, (_dword)-1);
        p.Proc(k, d, p.Param);
    }

public:
    CMaaStringFileHash_2(CMaaMessageDB * key_db, CMaaMessageDB * data_db, const CMaaString &FileName, int Flags = CMaaMessageDB::eAllowMsgOverlap, _qword MaxSize = -1)
    :   CMaaUnivFileHash<_qword, _qword>(FileName + ".ht", MaxSize, Flags),
        m_key_db(key_db),
        m_data_db(data_db),
        m_index0(0)
    {
    }

    ~CMaaStringFileHash_2() noexcept
    {
        try
        {
            Flush();
            //m_key_db->Flush();
        }
        catch (...)
        {
        }
    }

    void EnumerateByProc(EnumerateProcEx Proc, void * Param = nullptr)
    {
        sParam param(Proc, Param, this);
        CMaaUnivFileHash<_qword, _qword>::EnumerateByProc(EnumerateProc1, &param);
    }

    int Add(const Key &K, const Data &D, int fOverwrite = 0)
    {
        //CMaaFile logfile("/temp/logfile.txt", CMaaFile::eACD_SrSw, eNoExcept);
        CMaaFile logfile;
        //logfile.fprintf("CMaaStringFileHash_2::Add(): %d %d\n", x1, x2);
        Index index;
        if  (Find(K, &index) == 0)
        {
            logfile.fprintf("CMaaStringFileHash_2::Add(): Find(%S,%D)\n", &K, index);

            CMaaString old_data;
            m_data_db->Get(index, old_data, (_dword)-1);
            logfile.fprintf("CMaaStringFileHash_2::Add(): data->Get(%D) %S\n", index, &old_data);

            if  (old_data == D)
            {
                return fOverwrite ? 0 : 1;
            }
            else
            {
                if  (!fOverwrite)
                {
                    return 1;
                }
                const int x1 = m_data_db->Remove(index);
                logfile.fprintf("CMaaStringFileHash_2::Add(): data->Remove(%D) returns %d\n", index, x1);
                /*int x2 =*/ m_data_db->Add(index, D);
                logfile.fprintf("CMaaStringFileHash_2::Add(): data->Add(%D, %S) returns %d\n", index, &D, x1);
                return CMaaUnivFileHash<_qword, _qword>::Add(index, index, fOverwrite);
            }
        }
        int r = -1;
        while(1)
        {
            m_index0++;
            CMaaString temp;
            if  (m_key_db->Get(m_index0, temp, (_dword)-1) && m_data_db->Get(m_index0, temp, (_dword)-1))
            {
                r = m_key_db->Add(m_index0, K);
                logfile.fprintf("CMaaStringFileHash_2::Add(): key->Add(%D,%S) returns %d\n", m_index0, &K, r);
                if  (!r)
                {
                    r = m_data_db->Add(m_index0, D);
                    logfile.fprintf("CMaaStringFileHash_2::Add(): data->Add(%D,%S) returns %d\n", m_index0, &D, r);
                    if  (!r)
                    {
                        break;
                    }
                    const int xx = m_key_db->Remove(m_index0);
                    logfile.fprintf("CMaaStringFileHash_2::Add(): key->Remove(%D) returns %d\n", m_index0, xx);
                }
            }
        }
        if  (!r)
        {
            r = CMaaUnivFileHash<_qword, _qword>::Add(m_index0, m_index0, fOverwrite);
            logfile.fprintf("CMaaStringFileHash_2::Add(): ::Add(%D,%D,%d) returns %d\n", m_index0, m_index0, fOverwrite, r);
            if  (r)
            {
                m_key_db->Remove(m_index0);
                m_data_db->Remove(m_index0);
            }
        }
        return r;
    }

    int AddOver(const Key &K, const Data &D)
    {
        return Add(K, D, 1);
    }

    int Find(const Key &K, Index *index = nullptr)
    {
        m_Key1 = K;
        //          Index index;
        const int r = CMaaUnivFileHash<_qword, _qword>::Find(-1, index);
        /*
          if (!r && D)
          {
              m_data_db->Get(index, *D, (_dword)-1);
          }
*/
        return r;
    }

    int Find(const Key &K, Data *D = nullptr)
    {
        //CMaaFile logfile("/temp/logfile.txt", CMaaFile::eACD_SrSw, eNoExcept);
        CMaaFile logfile;
        Index index = -1;
        const int r = Find(K, &index);
        if  (!r && D)
        {
            logfile.fprintf("CMaaStringFileHash_2::Find(): %d, %D\n", r, index);
            const int xx = m_data_db->Get(index, *D, (_dword)-1);
            logfile.fprintf("CMaaStringFileHash_2::Find__ Get(): %d, %S\n", xx, D);
        }
        else
        {
            logfile.fprintf("CMaaStringFileHash_2::Find(): %d, %D\n", r, index);
        }
        return r;
    }

    int Remove(const Key &K, Data *D = nullptr)
    {
        m_Key1 = K;
        Index index;
        Data d2;
        const int r = CMaaUnivFileHash<_qword, _qword>::Remove(-1, &index);
        if  (!r)
        {
            const int x1 = m_key_db->Remove(index);
            D && m_data_db->Get(index, *D, (_dword)-1);
            const int x2 = m_data_db->Remove(index);
            //CMaaFile logfile("/temp/logfile.txt", CMaaFile::eACD_SrSw, eNoExcept);
            CMaaFile logfile;
            logfile.fprintf("Remove(): %d %d\n", x1, x2);
        }
        return r;
    }

    static bool RemoveFiles(CMaaString FileName)
    {
        bool bRes = true;
        try
        {
            CMaaUnivFileHash<_qword, _qword> tmp(FileName, 10 * 1024, eCreate | eDeleteOnClose);
        }
        catch(...)
        {
            bRes = false;
        }
        return bRes;
    }
};


class CMaaStr2StrHash
{
public:
    typedef CMaaString Key;
    typedef CMaaString Data;
    typedef void(*EnumerateProcEx)(Key &K, Data &D, CMaaStr2StrHash &ht, void * Param);

private:
    /*
     struct Data2
     {
          _qword index;
          Data d;
     };
     */

    CMaaMessageDB m_key_db, m_data_db;
    CMaaStringFileHash_2 m_h;

    struct sParam
    {
        CMaaStr2StrHash &ht;
        EnumerateProcEx Proc;
        void * Param;
        sParam(CMaaStr2StrHash &h, EnumerateProcEx ProcEx, void * ptr) noexcept
        :   ht(h),
            Proc(ProcEx),
            Param(ptr)
        {
        }
    };

    static void EnumerateProc1(CMaaString &K, Data &D, void * Param)
    {
        sParam &p = *(sParam *)Param;
        //          CMaaString txt;
        //          if   (!p.ht.m_db.Get(D.index, txt))//, 10 * 1024);
        {
            p.Proc(K, D, p.ht, p.Param);
        }
    }

public:
    CMaaStr2StrHash(const CMaaString &FileName, _dword MaxMsgSize = 10 * 1024, int Flags = CMaaMessageDB::eAllowMsgOverlap, _qword MaxSize = -1)
    :   m_key_db(FileName + ".key", MaxMsgSize, Flags, MaxSize),
        m_data_db(FileName + ".data", MaxMsgSize, Flags, MaxSize),
        m_h(&m_key_db, &m_data_db, FileName + ".ht", Flags, MaxSize)
    {
    }

    void EnumerateByProc(EnumerateProcEx Proc, void * Param = nullptr)
    {
        sParam param(*this, Proc, Param);
        m_h.EnumerateByProc(EnumerateProc1, &param);
    }

    int Add(const Key &K, const Data &D, int fOverwrite = 0)
    {
        return m_h.Add(K, D, fOverwrite);
    }

    int AddOver(const Key &K, const Data &D)
    {
        return Add(K, D, 1);
    }

    int Find(const Key &K, Data *D = nullptr)
    {
        const int x = m_h.Find(K, D);
        //CMaaFile logfile("/temp/logfile.txt", CMaaFile::eACD_SrSw, eNoExcept);
        CMaaFile logfile;
        logfile.fprintf("CMaaStr2StrHash::Remove(): %d\n", x);
        return x;
    }

    int Remove(const Key &K, Data *D = nullptr)
    {
        return m_h.Remove(K, D);
    }

    int Get(_qword Index, CMaaString &txt, _dword MaxDataSize = (_dword)-1)
    {
        /*
          if   (MaxDataSize == (_dword)-1)
          {
               MaxDataSize = m_db.m_MaxMsgSize;
          }
          */
        return m_data_db.Get(Index, txt, MaxDataSize);
    }

    static bool RemoveFiles(CMaaString FileName)
    {
        bool bRes = true;
        try
        {
            CMaaStr2StrHash tmp(FileName, 10 * 1024, CMaaMessageDB::eCreate | CMaaMessageDB::eDeleteOnClose);
        }
        catch(...)
        {
            bRes = false;
        }
        return bRes;
    }
};
