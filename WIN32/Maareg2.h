//---------------------------------------------------------------------------
//
// PRODUCT: ToolsLib
//
// AUTHOR:  Andrey A. Moisseenko
//
// Creation date:  17.01.2018
//
//---------------------------------------------------------------------------

//===========================================================================
// CMaaReg* - utf8 params & ret vals
//---------------------------------------------------------------------------
class ToolsExport CMaaReg
{
public:
    // Access mode
    enum AccessMode
    {
        Read = 0,
        Write = 1,
        CreateThisKey = 2,
        ReadAndCreateThisKey = 2,
        WriteAndCreateThisKey = 3
    };

protected:
    HKEY m_hKey, m_hRootKey;
    CMaaString m_RegKeyName;

private:
    void Init(CMaaString RegKeyName, AccessMode Access);

public:
    CMaaReg(HKEY hRootKey, CMaaString RegKeyName, AccessMode Access);
    CMaaReg(const CMaaReg * Reg, CMaaString RegKeyName, AccessMode Access);
    virtual ~CMaaReg();

    CMaaReg & operator = (const CMaaReg & That);
    CMaaReg & operator << (const CMaaReg & That);

    BOOL GetValue(CMaaString Name, CMaaString &val, int ApproxMaxLen = 10 * 1024) const;
    BOOL GetValue(CMaaString Name, char * p, size_t sz) const;
    BOOL GetValue(CMaaString Name, DWORD *pValue) const;
    BOOL SetValue(CMaaString Name, CMaaString Buffer);
    BOOL SetValue(const _WC_ * Name, const _WC_ * Buffer);
    BOOL SetValue(CMaaString Name, DWORD Value);
    BOOL SetValue(CMaaString, const void * Buffer, int Len); // Set binary value
    BOOL DelValue(CMaaString Name);

    int GetSubKeysCount() const;
    int GetValuesCount() const;
    void GetSubKeyName(int Index, CMaaString &SubKeyName, int ApproxMaxLen = 10 * 1024)  const;
    void DeleteAll(CMaaString SubKeyName); // delete subkey with subkeys & vals
    void DeleteAll_FullPath(CMaaString FullSubKeyName);
    void CopyClean(const CMaaReg & From);
#if 0
    void CleanNotExisted(const CMaaReg & From);
    void Copy(const CMaaReg & From);
#endif
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class ToolsExport CMaaRegHLM : public CMaaReg
{
    //   Local machine
public:
    CMaaRegHLM(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHLM & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaRegHCU : public CMaaReg
{
    // Current user
public:
    CMaaRegHCU(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHCU & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaRegHCR : public CMaaReg
{
public:
    CMaaRegHCR(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHCR & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaRegHUS : public CMaaReg
{
public:
    CMaaRegHUS(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHUS & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaRegHCC : public CMaaReg
{
public:
    CMaaRegHCC(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHCC & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaRegHPD : public CMaaReg
{
public:
    CMaaRegHPD(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHPD & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
class ToolsExport CMaaRegHDD : public CMaaReg
{
public:
    CMaaRegHDD(CMaaString RegKeyName, AccessMode Access = CMaaReg::Write);
    CMaaRegHDD & operator = (const CMaaReg & That);
};
//---------------------------------------------------------------------------
//===========================================================================
