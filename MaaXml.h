
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/MaaXml.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * xml format using pimpl model from Herb Sutter "Exceptional C++"
 * (Герб Саттер "Решение сложных задач на С++"). First dated 18.11.2006.
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

#ifndef __TOOLS_MAAXML_H
#define __TOOLS_MAAXML_H

//#define TOOLSLIB_XML_CCHAR_PTR
#define TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
//#define TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__ // obsolate iterator == 1 and "@" key overload
//#define TOOLSLIB_ADD_HIDDEN_BY_STRING // obsolate cuz can be slower then int key

void Init_CMaaXmlNodeImpl_statics();

#define CMaa_JSON_HEX_QUOT  0x01
#define CMaa_JSON_HEX_TAG   0x02
#define CMaa_JSON_HEX_AMP   0x04
#define CMaa_JSON_HEX_APOS  0x08

CMaaString StrToJson(const CMaaString &x, int MaxLen = 0x7fffffff, int Flags = 0) noexcept(noexcept_new);
int JsonCharLen(const char* p, int i, int MaxLen, bool bWeak = true) noexcept;
CMaaString JsonToStr(const CMaaString &x) noexcept(noexcept_new);

//void InitsCMaaXml256Bits() noexcept;

//extern void * gV;
//-------------------------------------------------------------------------------------------------
class CMaaXmlDocument;
class CMaaXmlProcessingInstruction;
class CMaaXmlElement;
class CMaaXmlNode;
class CMaaXmlNodeImpl;
//-------------------------------------------------------------------------------------------------
typedef CMaaXmlNodeImpl* CMaaXmlHandle;
/*
//#define TL_TRY_FAST_XML_NODE
#ifdef TL_TRY_FAST_XML_NODE
class CMaaFastXmlNode;
#else
#define CMaaFastXmlNode CMaaXmlNode
#define gsCMaaFastXmlNodeZ gsCMaaXmlNodeZ
#define CMaaFastXmlNodeZ CMaaXmlNodeZ
#endif
*/
//-------------------------------------------------------------------------------------------------
#define gsCMaaXmlNodeZ ((const CMaaXmlNode&)CMaaXmlNode::C().sCMaaXmlNodeZ)
#define CMaaXmlNodeZ CMaaXmlNode{} // !! avoid to use the address of temporarily
//#define CMaaXmlNodeZ (*(const CMaaXmlNode*)&CMaaXmlNode::C::sCMaaXmlNodeZ)
class CMaaXmlNode
{
    //friend class CMaaXmlNodeImpl;
    //friend class CMaaXmlDocument;
    //CMaaRefDLink<CMaaXmlNodeImpl> m_Imp;
    CMaaXmlNodeImpl * m_Imp;
protected:
    bool AddNode(CMaaXmlNode &Node) noexcept(noexcept_new);
    bool RemNode(CMaaXmlNode &Node) noexcept(noexcept_new);
public:
    struct CE
    {
        struct S
        {
            const CMaaXmlNodeImpl* m_pImp;
            constexpr S() noexcept : m_pImp(nullptr) {}
            /*virtual*/ constexpr ~S() {}
            //constexpr S(const CMaaXmlNodeImpl* p) noexcept : m_pImp(p) {}
            //constexpr S & operator = (const CMaaXmlNodeImpl* p) noexcept { m_pImp = p; return *this; }
        };
        S sCMaaXmlNodeZ;
        constexpr CE() noexcept;
    };
    //static constexpr CE C;
    static const CE& C() noexcept;

    bool AddAttribute(const CMaaString& Name, const CMaaString& Value) noexcept(noexcept_new);
    /*
    template<std::size_t N1, std::size_t N2> bool AddAttribute(const char(&Name)[N1], const char(&txt)[N2]) noexcept
    {
        return AddAttribute(CMaaString(Name), CMaaString(txt));
    }
    template<std::size_t N> bool AddAttribute(const CMaaString& Name, const char(&txt)[N]) noexcept
    {
        return AddAttribute(Name, CMaaString(txt));
    }
    //bool AddAttribute(const CMaaString& Name, const char * pszValue) noexcept(noexcept_new);
    template<typename T> bool AddAttribute(const CMaaString& Name, const T& x) noexcept(noexcept_new)
    {
        return AddAttribute(Name, CMaaString(x));
    }
    */
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template <int N> bool AddAttribute(const char (&Name)[N], const CMaaString &Value)
    {
        return AddAttribute(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), Value);
    }
    template <int N> bool AddAttribute(const CMaaString &Name, const char (&Value)[N])
    {
        return AddAttribute(Name, CMaaString(Value, GET_CHECK_TXT_N1_N(Value, N), CMaaString::eROMemString0));
    }
    template <int N> bool AddAttribute(char (&Name)[N], const CMaaString &Value) = delete;
    template <int N> bool AddAttribute(const CMaaString &Name, char (&Value)[N]) = delete;
    template <int N1, int N2> bool AddAttribute(const char (&Name)[N1], const char (&Value)[N2])
    {
            return AddAttribute(
                CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N1), CMaaString::eROMemString0),
                CMaaString(Value, GET_CHECK_TXT_N1_N(Value, N2), CMaaString::eROMemString0)
                );
    }
    template <int N1, int N2> bool AddAttribute(char (&Name)[N1], const char (&Value)[N2]) = delete;
    template <int N1, int N2> bool AddAttribute(const char (&Name)[N1], char (&Value)[N2]) = delete;
    template <int N1, int N2> bool AddAttribute(char (&Name)[N1], char (&Value)[N2]) = delete;
#endif
    bool AddAttributeVerifyName(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new);
    bool RemoveAttribute(const CMaaString &Name) noexcept;
#ifdef TOOLSLIB_XML_CCHAR_PTR
    bool RemoveAttribute(const char * pszName) noexcept;
#endif
    int RemoveAllAttributes() noexcept; // returns number of attributes that was removed
    bool AddString(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new);
    bool RemoveString(const CMaaString &Name) noexcept;
#ifdef TOOLSLIB_XML_CCHAR_PTR
    bool RemoveString(const char * pszName) noexcept;
#endif
    int RemoveAllStrings() noexcept; // returns number of attributes that was removed
    int CopyAttributes(const CMaaXmlNode From, bool bHiddenPtrs = false, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept) = nullptr) noexcept(noexcept_new);
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    int CopyAttributes(const CMaaXmlNode From, bool bHiddenPtrs/* = false*/, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)/* = nullptr*/, void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)/* = nullptr*/) noexcept(noexcept_new);
#endif
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    bool SetString(const CMaaString &Value) noexcept;
    const CMaaString& GetString(bool *pIsFound = nullptr) const noexcept;
#endif
public:
    constexpr CMaaXmlNode() noexcept : m_Imp(nullptr) {}
    CMaaXmlNode(CMaaXmlNodeImpl* I) noexcept;
#ifdef TL_TRY_FAST_XML_NODE
    CMaaXmlNode(const CMaaFastXmlNode& x) noexcept;
#endif
    CMaaXmlNode(int Type, const CMaaString &Name) noexcept(noexcept_new);
    CMaaXmlNode(const CMaaXmlNode& That) noexcept;
    CMaaXmlNode(CMaaXmlNode&& That) noexcept { m_Imp = That.m_Imp; That.m_Imp = nullptr; }
    CMaaXmlNode& operator=(const CMaaXmlNode& That) noexcept;
    CMaaXmlNode& operator=(CMaaXmlNode&& That) noexcept;
    /*virtual*/ ~CMaaXmlNode();
    constexpr CMaaXmlHandle GetHandle() const noexcept { return m_Imp; }
    //operator CMaaXmlHandle ();
    CMaaXmlNode GetParent() const noexcept;
    void Empty() noexcept;
    operator bool() const noexcept
    {
        return !IsNull();
    }
    CMaaXmlNode operator++() noexcept
    {
        return (*this = FindNext());
    }
    CMaaXmlNode operator--() noexcept
    {
        return (*this = FindPrev());
    }
    bool IsProcessingInstruction() const noexcept;
    bool IsElement() const noexcept;
    CMaaXmlNode FirstChild() const noexcept;
    CMaaXmlNode LastChild() const noexcept;
    CMaaXmlNode NextSibling() const noexcept;
    CMaaXmlNode PrevSibling() const noexcept;
    bool IsNull() const noexcept
    {
        return !m_Imp;
    }
    bool IsNotNull() const noexcept
    {
        return m_Imp;
    }
    bool IsValid() const noexcept
    {
        return m_Imp; // !IsNull();
    }
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    bool IsString() const noexcept
    {
        return GetString().IsNotEmpty();
    }
#endif
    bool AddProcessingInstruction(CMaaXmlNode & Node) noexcept(noexcept_new);
    bool AppendChild(CMaaXmlNode & Node) noexcept(noexcept_new);
    bool RemoveChild(CMaaXmlNode & Node) noexcept(noexcept_new); // Detach child from parent(this)
    bool Remove() noexcept(noexcept_new); // Detach child from parent(this)
    static CMaaString EncodeAttr(const CMaaString &s, int iFeatures = 0) noexcept(noexcept_new);
    static void EncodeAttr(CMaaConcatString &c, const CMaaString &s, int iFeatures = 0) noexcept(noexcept_new);
    static CMaaString DecodeAttr(const CMaaString &s, int cp /*= e_utf8*/, CMaaStringsContinedBuffer * pContBuf = nullptr, /*bool*/ int bOptSimpleStrings = 0) noexcept(noexcept_new);
    static CMaaString EncodeString(const CMaaString &s, int iFeatures = 0, const CMaaString &Name = gsCMaaStringZ) noexcept(noexcept_new); // {return ((iFeatures & CMaaXmlDocument::eRssCDataStrings) && Name != "lastBuildDate") ? CMaaString("<![CDATA[") + s + "]]>" : EncodeAttr(s, iFeatures);}
    static void EncodeString(CMaaConcatString &c, const CMaaString &s, int iFeatures, const CMaaString &Name = gsCMaaStringZ) noexcept(noexcept_new); // chk for Rss {return EncodeAttr(c, s, iFeatures);}
    static CMaaString DecodeString(const CMaaString &s, int cp /*= e_utf8*/) noexcept(noexcept_new) {return DecodeAttr(s, cp);}
    CMaaString ToString(const CMaaString &Sp0 = gsCMaaStringZ, const CMaaString &DeltaSp = CMaaStringSp, const CMaaString &EndOfLine = CMaaStringLf, int iFeatures = 0) const noexcept(noexcept_new);
    void ToString(CMaaConcatString& s, const CMaaString& Sp0 = gsCMaaStringZ, const CMaaString& DeltaSp = CMaaStringSp, const CMaaString& EndOfLine = CMaaStringLf, int iFeatures = 0) const;
    void ToJson(CMaaConcatString& s, const CMaaString& Sp0 = gsCMaaStringZ, const CMaaString& DeltaSp = CMaaStringSp, const CMaaString& EndOfLine = CMaaStringLf, int Flags = 0 /*CMaa_JSON_HEX_APOS | ...*/, int MaxNameLen = 128, int MaxAttrLen = 128) const;
    //CMaaXmlProcessingInstruction toProcessingInstruction();
    //CMaaXmlElement toElement();
    const CMaaString& GetName() const noexcept;
    void ChangeName(const CMaaString &Name) noexcept(noexcept_new);
    const CMaaString& FindAttribute(const CMaaString& Name) const noexcept;
    const CMaaString& FindAttribute(const CMaaString& Name, bool* pIsFound /*= nullptr*/) const noexcept;
    const CMaaString& FindAttribute(const CMaaString& Name, bool& bIsFound) const noexcept;
#ifdef TOOLSLIB_XML_CCHAR_PTR
    const CMaaString& FindAttribute(const char* pszName) const noexcept; // noexcept
    const CMaaString& FindAttribute(const char* pszName, bool* pIsFound /*= nullptr*/) const noexcept; // noexcept
    const CMaaString& FindAttribute(const char* pszName, bool& bIsFound) const noexcept; // noexcept
#endif
    const CMaaString& FindString(const CMaaString& Name) const noexcept;
    const CMaaString& FindString(const CMaaString& Name, bool* pIsFound /*= nullptr*/) const noexcept;
    const CMaaString& FindString(const CMaaString& Name, bool& bIsFound) const noexcept;
#ifdef TOOLSLIB_XML_CCHAR_PTR
    const CMaaString& FindString(const char* pszName) const noexcept; // noexcept
    const CMaaString& FindString(const char* pszName, bool* pIsFound /*= nullptr*/) const noexcept; // noexcept
    const CMaaString& FindString(const char* pszName, bool& bIsFound) const noexcept; // noexcept
#endif

    const void * FirstStringIterator() const noexcept;
    const void * FirstAttributeIterator() const noexcept;
    bool GetString(const void * Iterator, CMaaString *pName, CMaaString *pValue) const noexcept;
    bool GetAttribute(const void * Iterator, CMaaString *pName, CMaaString *pValue) const noexcept;
    const void * NextStringIterator(const void * Iterator) const noexcept;
    const void * NextAttributeIterator(const void * Iterator) const noexcept;
    CMaaXmlNode FindNode(const CMaaString& Name) const noexcept;
    CMaaXmlNode FindNode(const CMaaString& Name, int& N) const noexcept;
    CMaaXmlNode FindNode(const CMaaString& Name, int* pN /*= nullptr*/) const noexcept;
#ifdef TOOLSLIB_XML_CCHAR_PTR
    CMaaXmlNode FindNode(const char* pszName) const noexcept; // noexcept
    CMaaXmlNode FindNode(const char* pszName, int &N) const noexcept; // noexcept
    CMaaXmlNode FindNode(const char* pszName, int* pN /*= nullptr*/) const noexcept; // noexcept
#endif
    CMaaXmlNode FindNext() const noexcept;
    CMaaXmlNode FindPrev() const noexcept;
    CMaaXmlNode FindNodeWithAttr(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ) const noexcept;
#ifdef TOOLSLIB_XML_CCHAR_PTR
    CMaaXmlNode FindNodeWithAttr(const CMaaString& Name, const char* pszAttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ) const noexcept;
    CMaaXmlNode FindNodeWithAttr(const char* pszName, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ) const noexcept;
    CMaaXmlNode FindNodeWithAttr(const char* pszName, const char* pszAttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ) const noexcept;
#endif
    // Flags: 1 - cleanup cache, 2 - no negative cache storing
    CMaaXmlNode FindNodeWithAttrRO(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode /*= gsCMaaXmlNodeZ*/, int Flags = 0) const noexcept(noexcept_new); // for RO xml docs. Flags: 1 - cleanup cache, 2 - no negative cache storing
    CMaaXmlNode FindNodeWithAttrRO(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal) const noexcept(noexcept_new); // for RO xml docs. Flags: 1 - cleanup cache, 2 - no negative cache storing
#ifdef TOOLSLIB_XML_CCHAR_PTR
    CMaaXmlNode FindNodeWithAttrRO(const CMaaString& Name, const char* pszAttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ, int Flags = 0) const noexcept(noexcept_new); // for RO xml docs. Flags: 1 - cleanup cache, 2 - no negative cache storing
    CMaaXmlNode FindNodeWithAttrRO(const char* pszName, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ, int Flags = 0) const noexcept(noexcept_new); // for RO xml docs. Flags: 1 - cleanup cache, 2 - no negative cache storing
    CMaaXmlNode FindNodeWithAttrRO(const char* pszName, const char* pszAttrName, const CMaaString& AttrVal, const CMaaXmlNode &SkipNode = gsCMaaXmlNodeZ, int Flags = 0) const noexcept(noexcept_new); // for RO xml docs. Flags: 1 - cleanup cache, 2 - no negative cache storing
#endif
    CMaaXmlNode FindNodeWithString(const CMaaString& Name, const CMaaString& StringName, const CMaaString& StringVal, const CMaaXmlNode& SkipNode = gsCMaaXmlNodeZ) const noexcept;
    int ChangeNodesAttr(const CMaaString &Name, const CMaaString &AttrName, CMaaString OldVal, const CMaaString &NewVal) noexcept(noexcept_new);
    int RemoveNodesAttr(const CMaaString &Name, CMaaString AttrName, CMaaString Val) noexcept;
    int RemoveNodesWithAttr(const CMaaString &Name, CMaaString AttrName, CMaaString Val) noexcept(noexcept_new);
    int ChangeNodesString(const CMaaString &Name, const CMaaString &AttrName, CMaaString OldVal, const CMaaString &NewVal) noexcept(noexcept_new);
    int RemoveNodesString(const CMaaString &Name, CMaaString AttrName, CMaaString Val) noexcept;
    int RemoveNodesWithString(const CMaaString &Name, CMaaString AttrName, CMaaString Val) noexcept(noexcept_new);

    int AddHiddenPtr(int Name, void* ptr, void (*deref)(int, void*, void*) noexcept /*= nullptr*/, void* obj = nullptr, bool bDerefOld = true) noexcept(noexcept_new);
    int AddHiddenPtr(int Name, void* ptr /*, bool bDerefOld = true*/) noexcept(noexcept_new);
    int AddHiddenPtr_mutable(int Name, void* ptr, void (*deref)(int, void*, void*) noexcept /*= nullptr*/, void* obj = nullptr, bool bDerefOld = true) const noexcept(noexcept_new);
    int AddHiddenPtr_mutable(int Name, void* ptr/*, bool bDerefOld = true*/) const noexcept(noexcept_new);
    int AddHiddenInt(int Name, int x) noexcept(noexcept_new); // Hidden ptrs and ints are mixed in the same scope
    int AddHiddenInt_mutable(int Name, int x) const noexcept(noexcept_new); // Hidden ptrs and ints are mixed in the same scope
    int AddHiddenStringImp(int Name, const CMaaString& Str) noexcept(noexcept_new);
    int AddHiddenString(int Name, const CMaaString& Str) noexcept(noexcept_new);
    int RemoveHiddenPtr(int Name, bool bDeref /*= false*/) noexcept;
    int RemoveHiddenPtr(int Name/*, bool bDeref = false*/) noexcept;
    int RemoveHiddenInt(int Name, bool bDeref /*= false*/) noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr(Name, bDeref);
    }
    int RemoveHiddenInt(int Name/*, bool bDeref = false*/) noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr(Name);
    }
    int RemoveHiddenPtr_mutable(int Name, bool bDeref /*= false*/) const noexcept;
    int RemoveHiddenPtr_mutable(int Name/*, bool bDeref = false*/) const noexcept;
    int RemoveHiddenInt_mutable(int Name, bool bDeref /*= false*/) const noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr_mutable(Name, bDeref);
    }
    int RemoveHiddenInt_mutable(int Name/*, bool bDeref = false*/) const noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr_mutable(Name);
    }
    int RemoveHiddenString(int Name) noexcept
    {
        // Hidden string ptrs and ptrs and ints are mixed in the same scope
        return RemoveHiddenPtr(Name, true);
    }
    void* GetHiddenPtr(int Name) const noexcept;
    void* GetHiddenPtr(int Name, bool& bGot) const noexcept;
    void* GetHiddenPtr(int Name, bool* bGot /*= nullptr*/) const noexcept;
    const CMaaString::CMaaStringImp* GetHiddenStringImp(int Name) const noexcept;
    const CMaaString::CMaaStringImp* GetHiddenStringImp(int Name, bool &bGot) const noexcept;
    const CMaaString::CMaaStringImp* GetHiddenStringImp(int Name, bool *bGot /*= nullptr*/) const noexcept;
    const CMaaString& GetHiddenString(int Name) const noexcept;
    const CMaaString& GetHiddenString(int Name, bool& bGot) const noexcept;
    const CMaaString& GetHiddenString(int Name, bool* bGot /*= nullptr*/) const noexcept;
    int GetHiddenInt(int Name, int Default = 0) const noexcept; // Hidden ptrs and ints are mixed in the same scope
    int GetHiddenInt(int Name, int Default, bool& bGot) const noexcept; // Hidden ptrs and ints are mixed in the same scope
    int GetHiddenInt(int Name, int Default, bool* bGot /*= nullptr*/) const noexcept; // Hidden ptrs and ints are mixed in the same scope
    int RemoveAllHiddenPtrs(bool bDeref /*= false*/) noexcept;
    int RemoveAllHiddenPtrs(/*bool bDeref = false*/) noexcept;
    int RemoveAllHiddenInts(bool bDeref /*= false*/) noexcept
    {
        // Hidden ptrs and ints are mixed in the same scope
        return RemoveAllHiddenPtrs(bDeref);
    }
    int RemoveAllHiddenInts(/*bool bDeref = false*/) noexcept
    {
        // Hidden ptrs and ints are mixed in the same scope
        return RemoveAllHiddenPtrs();
    }

#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    int AddHiddenPtr(const CMaaString& Name, void* ptr, void (*deref)(const CMaaString&, void*, void*) noexcept = nullptr, void* obj = nullptr, bool bDerefOld = true) noexcept(noexcept_new);
    int AddHiddenPtr_mutable(const CMaaString& Name, void* ptr, void (*deref)(const CMaaString&, void*, void*) noexcept = nullptr, void* obj = nullptr, bool bDerefOld = true) const noexcept(noexcept_new);
    int AddHiddenInt(const CMaaString &Name, int x) noexcept(noexcept_new); // Hidden ptrs and ints are mixed in the same scope
    int AddHiddenInt_mutable(const CMaaString& Name, int x) const noexcept(noexcept_new); // Hidden ptrs and ints are mixed in the same scope
    int AddHiddenString(const CMaaString &Name, const CMaaString &Str) noexcept(noexcept_new);
    int RemoveHiddenPtr(const CMaaString& Name, bool bDeref = false) noexcept;
    int RemoveHiddenInt(const CMaaString& Name, bool bDeref = false) noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr(Name, bDeref);
    }
    int RemoveHiddenPtr_mutable(const CMaaString& Name, bool bDeref = false) const noexcept;
    int RemoveHiddenInt_mutable(const CMaaString& Name, bool bDeref = false) const noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr_mutable(Name, bDeref);
    }
    int RemoveHiddenString(const CMaaString &Name) noexcept
    {
        // Hidden string ptrs and ptrs and ints are mixed in the same scope
        return RemoveHiddenPtr(Name, true);
    }
    void* GetHiddenPtr(const CMaaString& Name) const noexcept;
    void* GetHiddenPtr(const CMaaString& Name, bool& bGot) const noexcept;
    void* GetHiddenPtr(const CMaaString& Name, bool* bGot /*= nullptr*/) const noexcept;
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template <int N> void* GetHiddenPtr(const char (&pszName)[N], bool* bGot = nullptr) const noexcept
    {
        return GetHiddenPtr(CMaaString(pszName, GET_CHECK_TXT_N1_N(pszName, N), CMaaString::eROMemString0), bGot);
    }
#endif
    const CMaaString& GetHiddenString(const CMaaString& Name) const noexcept;
    const CMaaString& GetHiddenString(const CMaaString& Name, bool& bGot) const noexcept;
    const CMaaString& GetHiddenString(const CMaaString &Name, bool* bGot /*= nullptr*/) const noexcept;
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template <int N> const CMaaString& GetHiddenString(const char (&pszName)[N], bool* bGot = nullptr) const noexcept
    {
        return GetHiddenString(CMaaString(pszName, GET_CHECK_TXT_N1_N(pszName, N), CMaaString::eROMemString0), bGot);
    }
#endif
    int GetHiddenInt(const CMaaString& Name, int Default = 0) const noexcept; // Hidden ptrs and ints are mixed in the same scope
    int GetHiddenInt(const CMaaString& Name, int Default, bool& bGot) const noexcept; // Hidden ptrs and ints are mixed in the same scope
    int GetHiddenInt(const CMaaString& Name, int Default, bool* bGot /*= nullptr*/) const noexcept; // Hidden ptrs and ints are mixed in the same scope
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template <int N> int GetHiddenInt(const char(&pszName)[N], int Default = 0) const noexcept // Hidden ptrs and ints are mixed in the same scope
    {
        return GetHiddenInt(CMaaString(pszName, GET_CHECK_TXT_N1_N(pszName, N), CMaaString::eROMemString0), Default);
    }
    template <int N> int GetHiddenInt(const char (&pszName)[N], int Default, bool &bGot) const noexcept // Hidden ptrs and ints are mixed in the same scope
    {
        return GetHiddenInt(CMaaString(pszName, GET_CHECK_TXT_N1_N(pszName, N), CMaaString::eROMemString0), Default, bGot);
    }
    template <int N> int GetHiddenInt(const char(&pszName)[N], int Default, bool* bGot /*= nullptr*/) const noexcept // Hidden ptrs and ints are mixed in the same scope
    {
        return GetHiddenInt(CMaaString(pszName, GET_CHECK_TXT_N1_N(pszName, N), CMaaString::eROMemString0), Default, bGot);
    }
#endif
    /*
    int RemoveAllHiddenPtrs(bool bDeref = false) noexcept;
    int RemoveAllHiddenInts(bool bDeref = false) noexcept
    {
        // Hidden ptrs and ints are mixed in the same scope
        return RemoveAllHiddenPtrs(bDeref);
    }
    */

#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template <int N> int AddHiddenPtr(const char (&Name)[N], void* ptr, void (*deref)(CMaaString, void*, void*) noexcept = nullptr, void* obj = nullptr, bool bDerefOld = true) noexcept(noexcept_new)
    {
        return AddHiddenPtr(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), ptr, deref, obj, bDerefOld);
    }
    template <int N> int AddHiddenInt(const char (&Name)[N], int x) noexcept(noexcept_new) // Hidden ptrs and ints are mixed in the same scope
    {
        return AddHiddenInt(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), x);
    }
    template <int N> int AddHiddenString(const char (&Name)[N], const CMaaString &Str) noexcept(noexcept_new)
    {
        return AddHiddenString(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), Str);
    }
    template <int N> int RemoveHiddenPtr(const char (&Name)[N], bool bDeref = false) noexcept
    {
        return RemoveHiddenPtr(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), bDeref);
    }
    template <int N> int RemoveHiddenInt(const char (&Name)[N], bool bDeref = false) noexcept
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), bDeref);
    }
    template <int N> int RemoveHiddenString(const char (&Name)[N]) noexcept
    {
        // Hidden string ptrs and ptrs and ints are mixed in the same scope
        return RemoveHiddenPtr(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), true);
    }

    //template <int N> int c_tst(char (&txt)[N]);
    template <int N> int AddHiddenPtr(char (&Name)[N], void* ptr, void (*deref)(CMaaString, void*, void*) noexcept = nullptr, void* obj = nullptr, bool bDerefOld = true) = delete;
    template <int N> int AddHiddenInt(char (&Name)[N], int x) = delete; // Hidden ptrs and ints are mixed in the same scope
    template <int N> int AddHiddenString(char (&Name)[N], CMaaString Str) = delete;
    template <int N> int RemoveHiddenPtr(char (&Name)[N], bool bDeref = false) = delete;
    template <int N> int RemoveHiddenInt(char (&Name)[N], bool bDeref = false) = delete;
    template <int N> int RemoveHiddenString(char (&Name)[N]) = delete;
    template <int N> void* GetHiddenPtr(char (&pszName)[N], bool* bGot = nullptr) const noexcept = delete;
    template <int N> const CMaaString& GetHiddenString(char (&pszName)[N], bool* bGot = nullptr) const noexcept = delete;
    template <int N> void* GetHiddenInt(char (&pszName)[N], int Default = 0, bool* bGot = nullptr) const noexcept = delete;
#endif
#endif // TOOLSLIB_ADD_HIDDEN_BY_STRING

    bool MoveUp() noexcept(noexcept_new);
    bool MoveDown() noexcept(noexcept_new);
    bool MoveUpSibling() noexcept(noexcept_new);
    bool MoveDownSibling() noexcept(noexcept_new);
    bool InsertBefore(CMaaXmlNode p) noexcept(noexcept_new); // in: *this is free, out: this will be before 'p'
    bool InsertAfter(CMaaXmlNode p) noexcept(noexcept_new); // in: *this is free, out: this will be after 'p'
    CMaaXmlNode CopyNode(CMaaXmlDocument& doc, bool bChilds = true, bool bHiddenPtrs = false, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept) = nullptr) const noexcept(noexcept_new);
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    CMaaXmlNode CopyNode(CMaaXmlDocument &doc, bool bChilds/* = true*/, bool bHiddenPtrs/* = false*/, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)/* = nullptr*/, void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)/* = nullptr*/) const noexcept(noexcept_new);
#endif
    void ConvertToUtf8(bool bChilds = true, _dword cp =
#ifdef _WIN32
         CP_ACP
#else
         0
#endif
         ) noexcept(noexcept_new);
    typedef void (* EnumAttributesFn)(const CMaaString &AttrName, CMaaString &AttrValue, CMaaXmlNode &Node, void *Param);
    typedef void (* EnumStringsFn)(const CMaaString &AttrName, CMaaString &AttrValue, CMaaXmlNode &Node, void *Param);

    class XX
    {
        CMaaXmlNodeImpl * m_Impl;
    protected:
        /*
          CMaaXmlNodeImpl& Impl() const
          {
               return *m_Impl;
          }
          */
    public:
        CMaaXmlNode n() const noexcept
        {
            return m_Impl;
        }
        CMaaString szTxt;
        bool bConst;

        XX(CMaaXmlNode &x, const CMaaString &Txt) noexcept;
        XX(const CMaaXmlNode &x, const CMaaString &Txt) noexcept;
        XX(const XX &That) noexcept;
        ~XX() noexcept;
        operator CMaaString() const noexcept;
        const CMaaString &crString() const noexcept;
        //operator const char * () const;
        operator const CMaaXmlNode() const noexcept;
        operator CMaaXmlNode() noexcept;
        bool operator=(const CMaaString &txt) noexcept(noexcept_new);
        bool operator=(const XX & That) noexcept(noexcept_new);
        bool operator==(const CMaaString &s) const noexcept
        {
            //return CMaaString(*this) == s;
            return crString() == s;
        }
        bool operator!=(const CMaaString &s) const noexcept
        {
            //return CMaaString(*this) != s;
            return crString() != s;
        }
        CMaaString operator+(const CMaaString &s) const noexcept(noexcept_new)
        {
            //return CMaaString(*this) + s;
            return crString() + s;
        }
    };

    XX operator[](const CMaaString &txt) noexcept
    {
        return XX(*this, txt);
    }
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr1
    template <int N> XX operator[](char (&pszText)[N]) = delete;
    template <int N> XX operator[](const char (&pszText)[N]) noexcept
    {
        return XX(*this, CMaaString(pszText, GET_CHECK_TXT_N1_N(pszText, N), CMaaString::eROMemString0));
    }
#endif

    const XX operator[](const CMaaString &txt) const noexcept
    {
        return XX(*this, txt);
    }
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr1
    template <int N> const XX operator[](char (&pszText)[N]) const = delete;
    template <int N> const XX operator[](const char (&pszText)[N]) const noexcept
    {
        return XX(*this, CMaaString(pszText, GET_CHECK_TXT_N1_N(pszText, N), CMaaString::eROMemString0));
    }
#endif

    bool operator<<(CMaaXmlNode &x) noexcept(noexcept_new)
    {
        return AppendChild(x);
    }
protected:
    static void stEnumAttrFn(const CMaaString &AttrName, CMaaString &AttrValue, void *Param);
public:
    int EnumAttributes(EnumAttributesFn Fn, void* Param = nullptr);
    int EnumStrings(EnumStringsFn Fn, void * Param = nullptr);
    int SortAttributes() noexcept;
    int SortStrings() noexcept;
    enum
    {
        eAttributes = 0x01,
        eStrings = 0x02,
        eRecursive = 0x100,
        eAll = eAttributes | eStrings,
        eAllRecursive = eAll | eRecursive
    };
    void Sort(int Flags = eAllRecursive) noexcept;

    int GetRefCount() const noexcept;
protected:
    bool AddBefore(CMaaXmlNode n) noexcept(noexcept_new);
    bool AddAfter(CMaaXmlNode n) noexcept(noexcept_new);
};
//-------------------------------------------------------------------------------------------------
class CMaaXmlProcessingInstruction : public CMaaXmlNode
{
public:
    CMaaXmlProcessingInstruction(const CMaaString &Name) noexcept(noexcept_new);
    CMaaXmlProcessingInstruction(const CMaaXmlNode &Node) noexcept;
    enum
    {
        eVersion = 1,
        eEncoding
    };
    bool AddInstruction(int Type, CMaaString Value = gsCMaaStringZ) noexcept(noexcept_new);
    bool AddInstruction(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new);
    ADD_NO_ALLOCATOR
};
//-------------------------------------------------------------------------------------------------
class CMaaXmlElement : public CMaaXmlNode
{
public:
    CMaaXmlElement(const CMaaString &Name) noexcept(noexcept_new);
    CMaaXmlElement(const CMaaXmlNode &Node) noexcept;
    bool SetAttribute(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new);
    bool AppendChild(CMaaXmlNode& Node) noexcept(noexcept_new);
#ifdef TL_TRY_FAST_XML_NODE
    bool AppendChild(CMaaFastXmlNode& Node) noexcept(noexcept_new);
#endif
    ADD_NO_ALLOCATOR
};
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
class CMaaXmlDocument
{
    CMaaString m_Name;
    CMaaXmlNode m_Root;
public:
    static int s_PaddingBytes;
    enum
    {
        eAcceptStrings = 0x01,
        eTreatEmptyNodeAsAString = 0x03,
        eComplexStrings = 0x04,
        // all strings are nodes
        eRssCDataStrings = 0x08,
        // use <![CDATA[text]]> for strings except <lastBuildDate>
        eCachedNames = 0x10,
        eCachedValues = 0x20,
        eCachedStrings = 0x40,
        eContinedBuffer = 0x80,
        eCachedRONames = 0x100,
        eCachedROValues = 0x200,
        eCachedROStrings = 0x400,
        eMainRefString = 0x800,
        eMainRefStringZT = 0x1000,
        eMainRefString2 = eMainRefString | eMainRefStringZT,
        eCachedAll = eCachedNames | eCachedValues | eCachedStrings,
        eCachedROAll = eCachedRONames | eCachedROValues | eCachedROStrings,
        eCachedOpts = eCachedNames,
        eCrLfAreInTheSimplePresentation = 0x100,
        eDoNotAddDoctypeKey = 0x200,
        eEncodeAttrAsIsIfCan = 0x400,
        eDefault9 = eTreatEmptyNodeAsAString * 0 | eAcceptStrings | eCachedOpts | eComplexStrings * 0 | eRssCDataStrings * 0 | eContinedBuffer,
        eDefault1 = eAcceptStrings | eCachedOpts | eMainRefString2,
        eDefault1WithRO = eDefault1 | eCachedROAll,
        eDefaultRO = eAcceptStrings | eCachedROAll,
        eDefault2 = eAcceptStrings | eContinedBuffer,
        eDefault = eAcceptStrings | eCachedOpts * 0 | eContinedBuffer * 0 | eMainRefString2
    };
    CMaaXmlDocument(const CMaaString &Name = CMaaStringDoc) noexcept(noexcept_new);
    CMaaXmlDocument(const CMaaString &Name, const CMaaString &RootName) noexcept(noexcept_new);
    CMaaXmlDocument(CMaaXmlDocument& That, bool bCopy = false, bool bCopyHiddenPtrs = false, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept) = nullptr) noexcept(noexcept_new);
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    CMaaXmlDocument(CMaaXmlDocument & That, bool bCopy/* = false*/, bool bCopyHiddenPtrs/* = false*/, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)/* = nullptr*/, void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)/* = nullptr*/) noexcept(noexcept_new);
#endif
    ~CMaaXmlDocument();
    CMaaXmlProcessingInstruction CreateProcessingInstruction(CMaaString xml = "xml") noexcept(noexcept_new);
    CMaaXmlElement CreateElement(const CMaaString &Name) noexcept(noexcept_new);
#ifndef TOOLSLIB_FORCE_TRY_AND_CHECK_ConstStr
    template <int N> CMaaXmlElement CreateElement(char (&Name)[N]) = delete;
    template <int N> CMaaXmlElement CreateElement(const char (&Name)[N])
    {
        return CreateElement(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0));
    }
#endif
    bool AddProcessingInstruction(CMaaXmlNode &Node) noexcept(noexcept_new);
    void AppendChild(CMaaXmlNode &Node) noexcept(noexcept_new);
    //bool SetContent(CMaaFile File, CMaaString *errorMsg, int *errorLine, int *errorColumn);
    bool SetContent(CMaaString Text, CMaaString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr, int iFeatures = eDefault) noexcept(noexcept_new);
    bool SetContent(CMaaString Text, const CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr, int iFeatures = eDefaultRO) noexcept(noexcept_new);
    bool SetContent(CMaaString Text, CMaaUnivHash<CMaaString, CMaaString> & hStringsCache, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr, int iFeatures = eDefault) noexcept(noexcept_new);
    bool SetJson(const CMaaString &Text, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr) noexcept(noexcept_new);
    CMaaXmlNode FirstChild() const noexcept;
    CMaaXmlElement DocumentElement() const noexcept;
    CMaaString ToString(int ApproxLength = -1, const CMaaString &Sp0 = gsCMaaStringZ, const CMaaString &DeltaSp = CMaaStringSp, const CMaaString &EndOfLine = CMaaStringLf, int iFeatures = 0 /*eCrLfAreInTheSimplePresentation|eDoNotAddDoctypeKey|eRssCDataStrings*/) const noexcept(noexcept_new);
    CMaaString ToString(const CMaaString &Sp0, const CMaaString &DeltaSp = CMaaStringSp, const CMaaString &EndOfLine = CMaaStringLf, int iFeatures = 0) const noexcept(noexcept_new);
    CMaaString ToJson(int ApproxLength = -1, const CMaaString &Sp0 = gsCMaaStringZ, const CMaaString &DeltaSp = CMaaStringSpSp, const CMaaString &EndOfLine = CMaaStringLf, int Flags = 0 /*CMaa_JSON_HEX_APOS | ...*/, int MaxNameLen = 128, int MaxAttrLen = 128) const noexcept(noexcept_new);
    CMaaXmlDocument & operator=(const CMaaXmlDocument & That) noexcept;
    bool LoadFromFile(const CMaaString &FileName, bool bThrow = true, CMaaString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr, int iFeatures = /*eTreatEmptyNodeAsAString * 0 + eAcceptStrings + eCachedOpts*/ eDefault1);
    bool SaveToFile(const CMaaString &FileName, bool bThrow = true, const CMaaString &Sp0 = gsCMaaStringZ, const CMaaString &DeltaSp = CMaaStringSp, const CMaaString &EndOfLine = CMaaStringCrLf, int iFeatures = 0 /*eCrLfAreInTheSimplePresentation|eDoNotAddDoctypeKey|eRssCDataStrings*/) const;
    int UpdateFromFile(const CMaaString& FileName, CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, int ms = 1000, bool bThrow = false, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr, int iFeatures = /*eAcceptStrings | eCachedOpts * 0 | eContinedBuffer * 0 | eMainRefString2*/ eDefault);
    int UpdateFromFile(const CMaaString& FileName, const CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, int ms = 1000, bool bThrow = false, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr, int iFeatures = /*eAcceptStrings | eCachedROAll*/ eDefaultRO);
    int UpdateFromFile(const CMaaString& FileName, int ms = 1000, bool bThrow = false, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr, int iFeatures = /*eTreatEmptyNodeAsAString * 0 + eAcceptStrings + eCachedOpts*/ eDefault1);
    void AddReplaceProcessingInstruction(CMaaString Encoding = "utf-8", CMaaString Version = "1.0", _dword ToUtf8Cp = -1) noexcept(noexcept_new); // -2 - не перекодировать строки
    void ConvertToUtf8(_dword cp = -1) noexcept(noexcept_new); // CP_ACP /*0*/ / 1251
};
//-------------------------------------------------------------------------------------------------

#endif // __TOOLS_MAAXML_H
