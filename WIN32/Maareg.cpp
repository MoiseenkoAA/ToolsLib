//==============================================================================
// PROJECT: ToolsLib
//==============================================================================
// FILE:       MaaReg.cpp
//
// AUTHOR:     Andrey A. Moisseenko
//
// Creation date:  28.07.98
//
//---------------------------------------------------------------------------

#include "perm.h"
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif

#ifdef _WIN32
#ifndef __TOOLSLIB_REMOVE_OLD_CODE

char ErrMsg [ 1024 ];

//---------------------------------------------------------------------------
CTooReg::CTooReg ( HKEY hRootKey, const char * RegKeyName, AccessMode Access )
:   m_hRootKey ( hRootKey )
{
    Init ( RegKeyName, Access );
}
//---------------------------------------------------------------------------
CTooReg::CTooReg ( const CTooReg * Reg, const char * RegKeyName, AccessMode Access )
:   m_hRootKey ( Reg -> m_hRootKey )
{
    CTooRegValue KeyName;
    sprintf ( KeyName, "%s\\%s", Reg -> m_RegKeyName, RegKeyName );
    Init ( KeyName, Access );
}
//---------------------------------------------------------------------------
CTooRegHCR::CTooRegHCR ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_CLASSES_ROOT, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CTooRegHCU::CTooRegHCU ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_CURRENT_USER, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CTooRegHLM::CTooRegHLM ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_LOCAL_MACHINE, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CTooRegHUS::CTooRegHUS ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_USERS, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CTooRegHCC::CTooRegHCC ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_CURRENT_CONFIG, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CTooRegHPD::CTooRegHPD ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_PERFORMANCE_DATA, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CTooRegHDD::CTooRegHDD ( const char * RegKeyName, AccessMode Access )
:   CTooReg ( HKEY_DYN_DATA, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
void CTooReg::Init ( const char * RegKeyName, AccessMode Access )
{
    REGSAM rsAccess;
    DWORD dwDisposition;
    if  (strlen(RegKeyName) >= sizeof(m_RegKeyName))
    {
        strcat(ErrMsg, "Too long RegKeyName string");
        throw ErrMsg;
    }
    strcpy ( m_RegKeyName, RegKeyName );
    int error = 0;

    if  ( Access & CTooReg::Write )
    {
        rsAccess = KEY_ALL_ACCESS;
    }
    else
    {
        rsAccess = KEY_READ; //KEY_ENUMERATE_SUB_KEYS;
    }
    if  ( Access & CTooReg::CreateThisKey )
    {
        error = RegCreateKeyExA ( m_hRootKey, RegKeyName, nullptr, "", REG_OPTION_NON_VOLATILE, rsAccess, nullptr, & m_hKey, & dwDisposition );
        if  ( error == ERROR_SUCCESS )
        {
            return;
        }
    }
    int err2 = RegOpenKeyExA ( m_hRootKey, RegKeyName, nullptr, rsAccess, & m_hKey );
    if  (  err2 != ERROR_SUCCESS )
    {
        //CMaaString Msg;

        if  ( Access & CTooReg::CreateThisKey )
        {
            sprintf ( ErrMsg, "CreateRegError %s %d", m_RegKeyName, error );
            //Msg.Format ( IDS_XTOOCreateRegError, m_RegKeyName, error );
        }
        else
        {
            sprintf ( ErrMsg, "OpenRegError %s %d", m_RegKeyName, error = err2 );
            //Msg.Format ( IDS_XTOOOpenRegError, m_RegKeyName, error = err2 );
        }
        XTOOLastError le ( "", error );
        strcat ( ErrMsg, le.GetMsg () );
        throw ErrMsg;
        //throw XTOORegError ( Msg, error );
    }
}
//---------------------------------------------------------------------------
CTooReg::~CTooReg ()
{
    int error = RegCloseKey ( m_hKey );
    if  ( error != ERROR_SUCCESS )
    {
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOCloseRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
        sprintf ( ErrMsg, "CloseRegError %s %d", m_RegKeyName, error );
        //throw ErrMsg;
    }
}
//---------------------------------------------------------------------------
BOOL CTooReg::GetValue ( const char * Name, char * Buffer, int MaxLen ) const
{
    DWORD cbLen = MaxLen;
    DWORD type;
    //char _Buffer [ _MAX_PATH + 1 ];
    if  ( RegQueryValueExA ( m_hKey, Name, nullptr, &type, ( LPBYTE ) Buffer, & cbLen ) == ERROR_SUCCESS )
    {
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CTooReg::SetValue ( const char * Name, const char * Buffer )
{
    DWORD cbLen = (DWORD)strlen ( Buffer ) + 1;
    int error = RegSetValueExA ( m_hKey, Name, nullptr, REG_SZ, ( LPBYTE ) Buffer, cbLen );
    if  ( error == ERROR_SUCCESS )
    {
        return TRUE;
    }
    sprintf ( ErrMsg, "WriteRegError %s %d", m_RegKeyName, error );
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CTooReg::SetValueW(_WC_ * Name, _WC_ * Buffer)
{
    DWORD cbLen = (DWORD)(sizeof(_WC_) * (maa_wcslen(Buffer) + 1));
    int error = RegSetValueExW(m_hKey, Name, nullptr, REG_SZ, (LPBYTE)Buffer, cbLen);
    if  (error == ERROR_SUCCESS)
    {
        return TRUE;
    }
    sprintf(ErrMsg, "WriteRegError %s %d", m_RegKeyName, error);
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CTooReg::SetValue ( const char * Name, DWORD Value )
{
    int error = RegSetValueExA ( m_hKey, Name, nullptr, REG_DWORD, ( CONST BYTE *) &Value, sizeof ( Value ) );

    if  ( error == ERROR_SUCCESS )
    {
        return TRUE;
    }
    sprintf ( ErrMsg, "WriteRegError %s %d", m_RegKeyName, error );
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CTooReg::SetValue ( const char * Name, const void * Buffer, int Len )
{
    DWORD  cbLen = Len;
    int error = RegSetValueExA ( m_hKey, Name, nullptr, REG_BINARY, ( LPBYTE ) Buffer, cbLen );
    if  ( error == ERROR_SUCCESS )
    {
        return TRUE;
    }
    sprintf ( ErrMsg, "WriteRegError %s %d", m_RegKeyName, error );
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CTooReg::DelValue ( const char * Name )
{
    if  ( RegDeleteValueA ( m_hKey, Name ) == ERROR_SUCCESS )
    {
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
int CTooReg::GetSubKeysCount () const
{
    DWORD SubKeys;

    int error = RegQueryInfoKeyA ( m_hKey, nullptr, nullptr, nullptr, & SubKeys,  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr );
    if  ( error != ERROR_SUCCESS )
    {
        sprintf ( ErrMsg, "QueryRegError %s %d", m_RegKeyName, error );
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOQueryRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
    return SubKeys;
}
//---------------------------------------------------------------------------
int CTooReg::GetValuesCount () const
{
    DWORD Values;

    int error = RegQueryInfoKeyA ( m_hKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, & Values, nullptr, nullptr, nullptr, nullptr );
    if  ( error != ERROR_SUCCESS )
    {
        sprintf ( ErrMsg, "QueryRegError %s %d", m_RegKeyName, error );
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOQueryRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
    return Values;
}
//---------------------------------------------------------------------------
void CTooReg::GetSubKeyName ( int Index, char * SubKeyName, int len ) const
{
    int error = RegEnumKeyA ( m_hKey, Index, SubKeyName, len );
    if  ( error != ERROR_SUCCESS )
    {
        sprintf ( ErrMsg, "EnumRegError %s %d", m_RegKeyName, error );
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOEnumRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
}
//---------------------------------------------------------------------------
void CTooReg::DeleteAll ( char * FullSubKeyName, int len )
{
    CTooReg DelReg ( m_hRootKey, FullSubKeyName, CTooReg::Read );
    int SubKeys = DelReg.GetSubKeysCount ();
    int CurLen = (int)strlen ( FullSubKeyName );
    if  ( CurLen + 1 > len && SubKeys )
    {
        sprintf ( ErrMsg, "DelAllRegError %s", m_RegKeyName );
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOODelAllRegError, m_RegKeyName );
        //throw XTOORegError ( Msg, 0 );
    }
    FullSubKeyName [ CurLen ] = '\\';

    for ( ; SubKeys--; )
    {
        DelReg.GetSubKeyName ( SubKeys, FullSubKeyName + CurLen + 1, len - CurLen - 1 );
        DeleteAll ( FullSubKeyName, len );
    }
    FullSubKeyName [ CurLen ] = 0;
    int error = RegDeleteKeyA ( m_hRootKey, FullSubKeyName );
    if  ( error != ERROR_SUCCESS )
    {
        sprintf ( ErrMsg, "DeleteKeyRegError %s %d", m_RegKeyName, error );
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOODeleteKeyRegError, m_RegKeyName, FullSubKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
}
//---------------------------------------------------------------------------
void CTooReg::DeleteAll ( const char * SubKeyName )
{
    CTooRegValue SubKey;
    if  ( strlen ( m_RegKeyName ) + strlen ( SubKeyName ) + 1 > sizeof ( SubKey ) )
    {
        sprintf ( ErrMsg, "DeleteKeyRegError %s %s", SubKeyName, m_RegKeyName );
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOODeleteKeyRegError, SubKeyName, m_RegKeyName );
        //throw XTOORegError ( Msg, 0 );

    }
    sprintf ( SubKey, "%s\\%s", m_RegKeyName, SubKeyName );
    DeleteAll ( SubKey, sizeof ( SubKey ) );
}
//---------------------------------------------------------------------------
void CTooReg::CopyClean ( const CTooReg & From )
{
    CleanNotExisted ( From );
    Copy ( From );
}
//---------------------------------------------------------------------------
void CTooReg::CleanNotExisted ( const CTooReg & From )
{
    CTooRegValue Name, Data;
    DWORD NameSize, DataSize, Type;
    int i;
    for ( i = GetValuesCount (); i--; )
    {
        NameSize = sizeof ( Name );
        DataSize = sizeof ( Data );
        if  ( RegEnumValueA ( m_hKey, i, Name, & NameSize, nullptr, &Type, ( LPBYTE ) Data, &DataSize ) ==  ERROR_SUCCESS )
        {
            CTooRegValue Data2;
            DWORD Data2Size = sizeof ( Data2 );
            DWORD Type2;
            if  ( RegQueryValueExA ( From.m_hKey, Name, nullptr, &Type2, ( LPBYTE ) Data2, & Data2Size ) == ERROR_SUCCESS )
            {
                if  ( Type == Type2 )
                {
                    continue;
                }
            }
            DelValue ( Name );
        }
    }
    for ( i = GetSubKeysCount (); i--; )
    {
        CTooRegValue SubKeyName;
        GetSubKeyName ( i, SubKeyName, sizeof ( SubKeyName ) );

        try
        {
            CTooReg DelReg ( & From, SubKeyName, CTooReg::Read );
            continue;
        }
        //catch ( XTOORegError & )
        catch( char * )
        {
        }
        DeleteAll ( SubKeyName );
    }
}
//---------------------------------------------------------------------------
void CTooReg::Copy ( const CTooReg & From )
{
    CTooRegValue Name, Data;
    DWORD NameSize, DataSize, Type;
    int i;
    for ( i = From.GetValuesCount (); i--; )
    {
        NameSize = sizeof ( Name );
        DataSize = sizeof ( Data );
        if  ( RegEnumValueA ( From.m_hKey, i, Name, & NameSize, nullptr, &Type, ( LPBYTE ) Data, &DataSize ) ==  ERROR_SUCCESS )
        {
            CTooRegValue Data2;
            DWORD Data2Size = sizeof ( Data2 );
            DWORD Type2;
            if  ( RegQueryValueExA ( m_hKey, Name, nullptr, &Type2, ( LPBYTE ) Data2, & Data2Size ) == ERROR_SUCCESS )
            {
                if  ( Type == Type2 && DataSize == Data2Size )
                {
                    if  (CMaaIsMemEq( Data, Data2, DataSize ) )
                    {
                        continue;
                    }
                }
                DelValue ( Name );
            }
            int error = RegSetValueExA ( m_hKey, Name, nullptr, Type, ( LPBYTE ) Data, DataSize );
            if  ( error != ERROR_SUCCESS )
            {
                sprintf ( ErrMsg, "WriteRegError2 %s %s", Name, m_RegKeyName );
                throw ErrMsg;
                //CMaaString Msg;
                //Msg.Format ( IDS_XTOOWriteRegError2, Name, m_RegKeyName );
                //throw XTOORegError ( Msg, 0 );
            }
        }
    }
    Data [ 0 ] = 0;
    From.GetValue ( "", Data, sizeof ( Data ) );
    SetValue ( "", Data );
    for ( i = From.GetSubKeysCount (); i--; )
    {
        CTooRegValue SubKeyName;
        From.GetSubKeyName ( i, SubKeyName, sizeof ( SubKeyName ) );

        CTooReg NextTo ( this, SubKeyName, CTooReg::WriteAndCreateThisKey );
        CTooReg NextFrom ( & From, SubKeyName, CTooReg::Read );
        NextTo.Copy ( NextFrom );
    }
}
//---------------------------------------------------------------------------
CTooReg & CTooReg::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooReg & CTooReg::operator << ( const CTooReg & That )
{
    Copy ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHLM & CTooRegHLM::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHCU & CTooRegHCU::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHCR & CTooRegHCR::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHUS & CTooRegHUS::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHCC & CTooRegHCC::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHPD & CTooRegHPD::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CTooRegHDD & CTooRegHDD::operator = ( const CTooReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------

#endif // #ifndef __TOOLSLIB_REMOVE_OLD_CODE
#endif // #ifdef _WIN32
