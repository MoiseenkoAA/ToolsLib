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
//#ifdef __TOOLSLIB_REMOVE_OLD_CODE

//---------------------------------------------------------------------------
CMaaReg::CMaaReg ( HKEY hRootKey, CMaaString RegKeyName, AccessMode Access )
:   m_hRootKey ( hRootKey )
{
    Init ( RegKeyName, Access );
}
//---------------------------------------------------------------------------
CMaaReg::CMaaReg ( const CMaaReg * Reg, CMaaString RegKeyName, AccessMode Access )
:   m_hRootKey ( Reg -> m_hRootKey )
{
    CMaaString KeyName = (CMaaString)Reg->m_RegKeyName + "\\" + RegKeyName;
    //sprintf ( KeyName, "%s\\%s", Reg -> m_RegKeyName, RegKeyName );
    Init ( KeyName, Access );
}
//---------------------------------------------------------------------------
CMaaRegHCR::CMaaRegHCR (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_CLASSES_ROOT, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CMaaRegHCU::CMaaRegHCU (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_CURRENT_USER, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CMaaRegHLM::CMaaRegHLM (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_LOCAL_MACHINE, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CMaaRegHUS::CMaaRegHUS (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_USERS, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CMaaRegHCC::CMaaRegHCC (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_CURRENT_CONFIG, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CMaaRegHPD::CMaaRegHPD (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_PERFORMANCE_DATA, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
CMaaRegHDD::CMaaRegHDD (CMaaString RegKeyName, AccessMode Access )
:   CMaaReg ( HKEY_DYN_DATA, RegKeyName, Access )
{
}
//---------------------------------------------------------------------------
void CMaaReg::Init (CMaaString RegKeyName, AccessMode Access )
{
    REGSAM rsAccess;
    DWORD dwDisposition;
    if  (RegKeyName.Utf8CharCount() >= _MAX_PATH)
    {
        CMaaString ErrMsg = "Too long RegKeyName string";
        throw ErrMsg;
    }
    m_RegKeyName = RegKeyName;

    int error = 0;
    CMaaString n = Utf8ToUnicode(m_RegKeyName, 1);

    if  ( Access & CMaaReg::Write )
    {
        rsAccess = KEY_ALL_ACCESS;
    }
    else
    {
        rsAccess = KEY_READ; //KEY_ENUMERATE_SUB_KEYS;
    }
    if  ( Access & CMaaReg::CreateThisKey )
    {
        error = RegCreateKeyExW(m_hRootKey, (LPCWSTR)(const char *)n, 0, (LPWSTR)L"", REG_OPTION_NON_VOLATILE, rsAccess, nullptr, &m_hKey, &dwDisposition);
        if  ( error == ERROR_SUCCESS )
        {
            return;
        }
    }
    int err2 = RegOpenKeyExW(m_hRootKey, (LPCWSTR)(const char *)n, 0, rsAccess, &m_hKey);
    if  (  err2 != ERROR_SUCCESS )
    {
        //CMaaString Msg;

        CMaaString ErrMsg;
        if  ( Access & CMaaReg::CreateThisKey )
        {
            ErrMsg.Format("CreateRegError %S %d", &m_RegKeyName, error);
            //Msg.Format ( IDS_XTOOCreateRegError, m_RegKeyName, error );
        }
        else
        {
            ErrMsg.Format("OpenRegError %S %d", &m_RegKeyName, error = err2);
            //Msg.Format ( IDS_XTOOOpenRegError, m_RegKeyName, error = err2 );
        }
        XTOOLastError le ( "", error );
        ErrMsg += le.GetMsg();
        throw ErrMsg;
        //throw XTOORegError ( Msg, error );
    }
}
//---------------------------------------------------------------------------
CMaaReg::~CMaaReg ()
{
    int error = RegCloseKey ( m_hKey );
    if  ( error != ERROR_SUCCESS )
    {
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOCloseRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
        CMaaString ErrMsg;
        ErrMsg.Format("CloseRegError %S %d", &m_RegKeyName, error);
        //throw ErrMsg;
    }
}
//---------------------------------------------------------------------------
BOOL CMaaReg::GetValue(CMaaString Name, CMaaString &val, int ApproxMaxLen) const
{
    CMaaPtr_<char, 1> Buffer(ApproxMaxLen * 4 + 2);
    DWORD cbLen = (DWORD)Buffer.Size();
    DWORD type;
    Name = Utf8ToUnicode(Name, 1);
    if  (RegQueryValueExW(m_hKey, (LPCWSTR)(const char *)Name, nullptr, &type, (LPBYTE)(char *)Buffer, &cbLen) == ERROR_SUCCESS)
    {
        CMaaString v(Buffer, (int)cbLen);
        val = UnicodeToUtf8(v, -1);
        return TRUE;
    }
    return FALSE;
}
BOOL CMaaReg::GetValue(CMaaString Name, char * p, size_t sz) const
{
    if  (!sz)
    {
        return FALSE;
    }
    CMaaString v;
    if  (GetValue(Name, v, (int)sz))
    {
        v = v.Left((int)sz - 1);
        memcpy(p, v, v.Length() + 1);
        return TRUE;
    }
    *p = 0;
    return FALSE;
}
BOOL CMaaReg::GetValue(CMaaString Name, DWORD *pValue) const
{
    DWORD dw = pValue ? *pValue : 0;
    DWORD cbLen = sizeof(dw);
    DWORD type;
    Name = Utf8ToUnicode(Name, 1);
    if  (RegQueryValueExW(m_hKey, (LPCWSTR)(const char *)Name, nullptr, &type, (LPBYTE)&dw, &cbLen) == ERROR_SUCCESS)
    {
        if  (type == REG_DWORD)
        {
            if  (pValue)
            {
                *pValue = dw;
            }
            return TRUE;
        }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CMaaReg::SetValue(CMaaString Name, CMaaString Buffer)
{
    Name = Utf8ToUnicode(Name, 1);
    Buffer = Utf8ToUnicode(Buffer, 1);
    DWORD cbLen = (DWORD)Buffer.Length();
    int error = RegSetValueExW ( m_hKey, (LPCWSTR)(const char *)Name, 0, REG_SZ, (LPBYTE)(const char *)Buffer, cbLen);
    if  ( error == ERROR_SUCCESS )
    {
        return TRUE;
    }
    CMaaString ErrMsg;
    ErrMsg.Format("WriteRegError %S %d", &m_RegKeyName, error);
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CMaaReg::SetValue(const _WC_ * Name, const _WC_ * Buffer)
{
    DWORD cbLen = (DWORD)(sizeof(_WC_) * (maa_wcslen(Buffer) + 1));
    int error = RegSetValueExW(m_hKey, Name, 0, REG_SZ, (LPBYTE)Buffer, cbLen);
    if  (error == ERROR_SUCCESS)
    {
        return TRUE;
    }
    CMaaString ErrMsg;
    ErrMsg.Format("WriteRegError %S %d", &m_RegKeyName, error);
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CMaaReg::SetValue(CMaaString Name, DWORD Value)
{
    Name = Utf8ToUnicode(Name, 1);
    int error = RegSetValueExW(m_hKey, (LPCWSTR)(const char *)Name, 0, REG_DWORD, ( CONST BYTE *) &Value, sizeof ( Value ) );

    if  ( error == ERROR_SUCCESS )
    {
        return TRUE;
    }
    CMaaString ErrMsg;
    ErrMsg.Format("WriteRegError %S %d", &m_RegKeyName, error);
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CMaaReg::SetValue(CMaaString Name, const void * Buffer, int Len)
{
    Name = Utf8ToUnicode(Name, 1);
    DWORD  cbLen = Len;
    int error = RegSetValueExW(m_hKey, (LPCWSTR)(const char *)Name, 0, REG_BINARY, (LPBYTE)Buffer, cbLen);
    if  (error == ERROR_SUCCESS)
    {
        return TRUE;
    }
    CMaaString ErrMsg;
    ErrMsg.Format("WriteRegError %S %d", &m_RegKeyName, error);
    throw ErrMsg;
    //CMaaString Msg;
    //Msg.Format ( IDS_XTOOWriteRegError, Name, m_RegKeyName, error );
    //throw XTOORegError ( Msg, error );
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL CMaaReg::DelValue(CMaaString Name)
{
    Name = Utf8ToUnicode(Name, 1);
    if  (RegDeleteValueW(m_hKey, (LPCWSTR)(const char *)Name) == ERROR_SUCCESS)
    {
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
int CMaaReg::GetSubKeysCount() const
{
    DWORD SubKeys;

    int error = RegQueryInfoKey ( m_hKey, nullptr, nullptr, nullptr, & SubKeys,  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr );
    if  ( error != ERROR_SUCCESS )
    {
        CMaaString ErrMsg;
        ErrMsg.Format("QueryRegError %S %d", &m_RegKeyName, error);
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOQueryRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
    return SubKeys;
}
//---------------------------------------------------------------------------
int CMaaReg::GetValuesCount () const
{
    DWORD Values;

    int error = RegQueryInfoKey ( m_hKey, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, & Values, nullptr, nullptr, nullptr, nullptr );
    if  ( error != ERROR_SUCCESS )
    {
        CMaaString ErrMsg;
        ErrMsg.Format("QueryRegError %S %d", &m_RegKeyName, error);
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOQueryRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
    return Values;
}
//---------------------------------------------------------------------------
void CMaaReg::GetSubKeyName(int Index, CMaaString &SubKeyName, int ApproxMaxLen) const
{
    CMaaPtr_<char, 1> Buffer(ApproxMaxLen * 4 + 2);
    ApproxMaxLen = (int)Buffer.Size();
    int error = RegEnumKeyW(m_hKey, Index, (LPWSTR)(char *)Buffer, ApproxMaxLen);
    if  ( error != ERROR_SUCCESS )
    {
        CMaaString ErrMsg;
        ErrMsg.Format("EnumRegError %S %d", &m_RegKeyName, error);
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOOEnumRegError, m_RegKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
    CMaaString v(Buffer, 2 * (int)maa_wcslen((LPCWSTR)(char *)Buffer));
    SubKeyName = UnicodeToUtf8(v);
}
//---------------------------------------------------------------------------
void CMaaReg::DeleteAll_FullPath(CMaaString FullSubKeyName)
{
    CMaaReg DelReg(m_hRootKey, FullSubKeyName, CMaaReg::Read);
    int SubKeys = DelReg.GetSubKeysCount();
    const int CurLen = (warning_int)FullSubKeyName.Utf8CharCount();
    if  (CurLen + 0*1 > _MAX_PATH && SubKeys)
    {
        CMaaString ErrMsg;
        ErrMsg.Format("DelAllRegError %S", &m_RegKeyName);
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOODelAllRegError, m_RegKeyName );
        //throw XTOORegError ( Msg, 0 );
    }
    //FullSubKeyName [ CurLen ] = '\\';

    for ( ; SubKeys--; )
    {
        CMaaString s;
        DelReg.GetSubKeyName(SubKeys, s);
        DeleteAll_FullPath(FullSubKeyName + "\\" + s);
    }
    FullSubKeyName = Utf8ToUnicode(FullSubKeyName, 1);
    int error = RegDeleteKeyW(m_hRootKey, (LPCWSTR)(const char *)FullSubKeyName);
    if  ( error != ERROR_SUCCESS )
    {
        CMaaString ErrMsg;
        ErrMsg.Format("DeleteKeyRegError %S %d", &m_RegKeyName, error);
        throw ErrMsg;
        //CMaaString Msg;
        //Msg.Format ( IDS_XTOODeleteKeyRegError, m_RegKeyName, FullSubKeyName, error );
        //throw XTOORegError ( Msg, error );
    }
}
//---------------------------------------------------------------------------
void CMaaReg::DeleteAll(CMaaString SubKeyName)
{
    DeleteAll_FullPath(m_RegKeyName + "\\" + SubKeyName);
}
//---------------------------------------------------------------------------
void CMaaReg::CopyClean ( const CMaaReg & From )
{
    CMaaString ErrMsg;
    ErrMsg.Format("CMaaReg::CopyClean() not implemented");
    throw ErrMsg;
#if 0
    CleanNotExisted ( From );
    Copy ( From );
#endif
}
#if 0
//---------------------------------------------------------------------------
void CMaaReg::CleanNotExisted ( const CMaaReg & From )
{
    CMaaRegValue Name, Data;
    DWORD NameSize, DataSize, Type;
    int i;
    for ( i = GetValuesCount (); i--; )
    {
        NameSize = sizeof ( Name );
        DataSize = sizeof ( Data );
        if  ( RegEnumValueA ( m_hKey, i, Name, & NameSize, nullptr, &Type, ( LPBYTE ) Data, &DataSize ) ==  ERROR_SUCCESS )
        {
            CMaaRegValue Data2;
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
        CMaaRegValue SubKeyName;
        GetSubKeyName ( i, SubKeyName, sizeof ( SubKeyName ) );

        try
        {
            CMaaReg DelReg ( & From, SubKeyName, CMaaReg::Read );
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
void CMaaReg::Copy ( const CMaaReg & From )
{
    CMaaRegValue Name, Data;
    DWORD NameSize, DataSize, Type;
    int i;
    for ( i = From.GetValuesCount (); i--; )
    {
        NameSize = sizeof ( Name );
        DataSize = sizeof ( Data );
        if  ( RegEnumValueA ( From.m_hKey, i, Name, & NameSize, nullptr, &Type, ( LPBYTE ) Data, &DataSize ) ==  ERROR_SUCCESS )
        {
            CMaaRegValue Data2;
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
        CMaaRegValue SubKeyName;
        From.GetSubKeyName ( i, SubKeyName, sizeof ( SubKeyName ) );

        CMaaReg NextTo ( this, SubKeyName, CMaaReg::WriteAndCreateThisKey );
        CMaaReg NextFrom ( & From, SubKeyName, CMaaReg::Read );
        NextTo.Copy ( NextFrom );
    }
}
#endif
//---------------------------------------------------------------------------
#if 0
CMaaReg & CMaaReg::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaReg & CMaaReg::operator << ( const CMaaReg & That )
{
    Copy ( That );
    return * this;
}
#endif
//---------------------------------------------------------------------------
CMaaRegHLM & CMaaRegHLM::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaRegHCU & CMaaRegHCU::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaRegHCR & CMaaRegHCR::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaRegHUS & CMaaRegHUS::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaRegHCC & CMaaRegHCC::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaRegHPD & CMaaRegHPD::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------
CMaaRegHDD & CMaaRegHDD::operator = ( const CMaaReg & That )
{
    CopyClean ( That );
    return * this;
}
//---------------------------------------------------------------------------

//#endif // #ifdef __TOOLSLIB_REMOVE_OLD_CODE
#endif // _WIN32
