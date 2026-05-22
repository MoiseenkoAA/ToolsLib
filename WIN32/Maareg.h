//---------------------------------------------------------------------------
//
// PRODUCT: ToolsLib
//
// AUTHOR:  Andrey A. Moisseenko
//
// Creation date:  28.07.98
//
//---------------------------------------------------------------------------

#ifndef __TOOLSLIB_REMOVE_OLD_CODE

//extern char ErrMsg [ 1024 ];
//---------------------------------------------------------------------------
typedef char CTooRegValue [ _MAX_PATH + 1 ];
//---------------------------------------------------------------------------
class ToolsExport CTooReg
{
public:
    // Access mode
    enum AccessMode
    {
        Read          = 0,
        Write         = 1,
        CreateThisKey = 2,
        ReadAndCreateThisKey = 2,
        WriteAndCreateThisKey = 3
    };

protected:
    HKEY m_hKey, m_hRootKey;
    CTooRegValue m_RegKeyName;

private:
    void Init ( const char * RegKeyName, AccessMode Access );

public:
    CTooReg ( HKEY hRootKey, const char * RegKeyName, AccessMode Access );
    CTooReg ( const CTooReg * Reg, const char * RegKeyName, AccessMode Access );
    virtual ~CTooReg ();

    CTooReg & operator = ( const CTooReg & That );
    CTooReg & operator << ( const CTooReg & That );

    BOOL GetValue ( const char * Name, char * Buffer, int MaxLen ) const;
    BOOL SetValue ( const char * Name, const char * Buffer );
    BOOL SetValueW(_WC_ * Name, _WC_ * Buffer);
#ifdef _UNICODE
    BOOL SetValueAW(_WC_ * Name, _WC_ * Buffer) {return SetValueW(Name, Buffer);}
#else
    BOOL SetValueAW(const char * Name, const char * Buffer) {return SetValue(Name, Buffer);}
#endif
    BOOL SetValue ( const char * Name, DWORD Value );
    BOOL SetValue ( const char * Name, const void * Buffer, int Len ); // Set binary value
    BOOL DelValue ( const char * Name );

    int  GetSubKeysCount () const;
    int GetValuesCount () const;
    void GetSubKeyName ( int Index, char * SubKeyName, int len )  const;
    void DeleteAll ( const char * SubKeyName );
    void DeleteAll ( char * FullSubKeyName, int len );
    void CopyClean ( const CTooReg & From );
    void CleanNotExisted ( const CTooReg & From );
    void Copy ( const CTooReg & From );

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class ToolsExport CTooRegHLM : public CTooReg
{
    //   Local machine
public:
    CTooRegHLM ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHLM & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
class ToolsExport CTooRegHCU : public CTooReg
{
    // Current user
public:
    CTooRegHCU ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHCU & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
class ToolsExport CTooRegHCR : public CTooReg
{
public:
    CTooRegHCR ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHCR & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
class ToolsExport CTooRegHUS : public CTooReg
{
public:
    CTooRegHUS ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHUS & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
class ToolsExport CTooRegHCC : public CTooReg
{
public:
    CTooRegHCC ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHCC & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
class ToolsExport CTooRegHPD : public CTooReg
{
public:
    CTooRegHPD ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHPD & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
class ToolsExport CTooRegHDD : public CTooReg
{
public:
    CTooRegHDD ( const char * RegKeyName, AccessMode Access = CTooReg::Write );
    CTooRegHDD & operator = ( const CTooReg & That );
};
//---------------------------------------------------------------------------
#endif // #ifndef __TOOLSLIB_REMOVE_OLD_CODE
