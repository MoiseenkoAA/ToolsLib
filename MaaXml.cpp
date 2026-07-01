
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: ÃÓËÒÂÂÌÍÓ ¿Ì‰ÂÈ ¿ÎÂÍÒÂÂ‚Ë˜)
 */

// ToolsLib/MaaXml.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * xml format using pimpl model from Herb Sutter "Exceptional C++"
 * (–ì–µ—Ä–± –°–∞—Ç—Ç–µ—Ä "–†–µ—à–µ–Ω–∏–µ —Å–ª–æ–∂–Ω—ã—Ö –∑–∞–¥–∞—á –Ω–∞ –°++"). First dated 18.11.2006.
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

//#define MANUAL_XML_REFS false
#define MANUAL_XML_REFS true
//#define CMaaSyncAssign CMaaXmlNodeImpl::AssignFast

//#define GLOCK_LIB2 gLock_lib2

//#define DYNAMIC_HiddenDataPtrs // slower

//#include "MaaXml.h"
//void * gV = nullptr;
//-------------------------------------------------------------------------------------------------
int CMaaXmlDocument::s_PaddingBytes = 1;
//-------------------------------------------------------------------------------------------------
//const CMaaXmlNode CMaaXmlNode::sCMaaXmlNodeZ{};
//-------------------------------------------------------------------------------------------------
class CMaaXmlNodeImpl :
#if MANUAL_XML_REFS
    public CMaaManualReferenceDLink
#else
    public CMaaReference, public CMaaDLink
#endif
{
    //friend class CMaaXmlNode;
    //friend static int SortFn1(const void* a1, const void* a2);
public:
    CMaaXmlNodeImpl* m_pParent;
    int m_Type;
    CMaaString m_Name;
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    CMaaString m_String; // 2018
#endif

    struct Attr : public CMaaDLink
    {
        CMaaString m_Name, m_Value;

        constexpr_ Attr(bool) noexcept {}
        Attr(const CMaaString& Name, const CMaaString& Value) noexcept
        :   m_Name(Name),
            m_Value(Value)
        {
        }
        ADD_ALLOCATOR(Attr)
    };
    struct NPtr
    {
        CMaaXmlNodeImpl* m_First;
        int N;
        //NPtr() noexcept {}
        constexpr NPtr(CMaaXmlNodeImpl * p, int n) noexcept : m_First(p), N(n) {}
        constexpr NPtr(bool) noexcept : m_First(nullptr), N(0) {}
    };
    struct HiddenPtrsI
    {
        void* m_Ptr;
        void (*deref)(int, void*, void*) noexcept; // Name, ptr, obj
        void* obj;
        HiddenPtrsI(eNotInit_) noexcept {}
        constexpr HiddenPtrsI(void* Ptr, void (*d)(int, void*, void*) noexcept, void* o) noexcept : m_Ptr(Ptr), deref(d), obj(o) {}
    };
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    struct HiddenPtrs
    {
        void* m_Ptr;
        void (*deref)(const CMaaString&, void*, void*) noexcept; // Name, ptr, obj
        void* obj;
    };
#endif
    struct sFindNodeWithAttrKey
    {
        CMaaString Name, AttrName, AttrVal;
        //sFindNodeWithAttrKey() noexcept {}
        sFindNodeWithAttrKey(const sFindNodeWithAttrKey &That) noexcept
        :   Name(That.Name),
            AttrName(That.AttrName),
            AttrVal(That.AttrVal)
        {
        }
        sFindNodeWithAttrKey(const sFindNodeWithAttrKey &&That) noexcept
        :   Name(That.Name),
            AttrName(That.AttrName),
            AttrVal(That.AttrVal)
        {
        }
        sFindNodeWithAttrKey(const CMaaString &n, const CMaaString &a, const CMaaString &v) noexcept
        :   Name(n),
            AttrName(a),
            AttrVal(v)
        {
        }
        bool operator == (const sFindNodeWithAttrKey& That) const noexcept
        {
            return AttrVal == That.AttrVal && AttrName == That.AttrName && Name == That.Name;
        }
        _dword Hash() const noexcept
        {
            return CMaaRol14dw(Name.Hash()) ^ CMaaRol7dw(AttrName.Hash()) ^ AttrVal.Hash();
        }
        _uqword Hash64() const noexcept
        {
            return CMaaRol14qw(Name.Hash64()) ^ CMaaRol7qw(AttrName.Hash64()) ^ AttrVal.Hash64();
        }
    };
    CMaaDList<CMaaXmlNodeImpl> m_ChildsList;
    //friend class CMaaDList<Attr>;
    CMaaDListAD<Attr> m_AttrsList, m_StringsList;

    static CMaaUnivHash<CMaaString, Attr*, true, 1>* GetStaticAttrs(int x)
    {
#if 1
        static CMaaUnivHash<CMaaString, Attr*, true, 1> s_hAttrs(0), s_hStrings(0);
        return !x ? &s_hAttrs : &s_hStrings;
#else
        static CMaaUnivHash<CMaaString, Attr*, true, 1> 
            *p_hAttrs = TL_NEW CMaaUnivHash<CMaaString, Attr*, true, 1>(0),
            *p_hStrings = TL_NEW CMaaUnivHash<CMaaString, Attr*, true, 1>(0);
        return !x ? p_hAttrs : p_hStrings;
#endif
    }
    static CMaaUnivHash<CMaaString, NPtr, true, 1>* GetStaticNPtr()
    {
#if 1
        static CMaaUnivHash<CMaaString, NPtr, true, 1> s_hNode(0);
        return &s_hNode;
#else
        static CMaaUnivHash<CMaaString, NPtr, true, 1> *p_hNode = TL_NEW CMaaUnivHash<CMaaString, NPtr, true, 1>(0);
        return p_hNode;
#endif
    }
#ifdef DYNAMIC_HiddenDataPtrs
    static CMaaUnivHashA<int, HiddenPtrsI, true, 1>* GetStaticHiddenDataPtrsI()
    {
#if 1
        static CMaaUnivHashA<int, HiddenPtrsI, true, 1> s_hHiddenDataPtrs(0);
        return &s_hHiddenDataPtrs;
#else
        static CMaaUnivHashA<int, HiddenPtrsI, true, 1>* p_hHiddenDataPtrs = TL_NEW CMaaUnivHashA<int, HiddenPtrsI, true, 1>(0);
        return p_hHiddenDataPtrs;
#endif
    }
#else
    static CMaaUnivHash<int, HiddenPtrsI, true, 1>* GetStaticHiddenDataPtrsI()
    {
#if 1
        static CMaaUnivHash<int, HiddenPtrsI, true, 1> s_hHiddenDataPtrs(0);
        return &s_hHiddenDataPtrs;
#else
        static CMaaUnivHash<int, HiddenPtrsI, true, 1>* p_hHiddenDataPtrs = TL_NEW CMaaUnivHash<int, HiddenPtrsI, true, 1>(0);
        return p_hHiddenDataPtrs;
#endif
    }
#endif
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    static CMaaUnivHash<CMaaString, HiddenPtrs, true, 1>* GetStaticHiddenDataPtrs()
    {
#if 1
        static CMaaUnivHash<CMaaString, HiddenPtrs, true, 1> s_hHiddenDataPtrs(0);
        return &s_hHiddenDataPtrs;
#else
        static CMaaUnivHash<CMaaString, HiddenPtrs, true, 1> *p_hHiddenDataPtrs = TL_NEW CMaaUnivHash<CMaaString, HiddenPtrs, true, 1>(0);
        return p_hHiddenDataPtrs;
#endif
    }
#endif
    static CMaaUnivHash<sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1>* GetStaticFindNodeWithAttr()
    {
#if 1
        static CMaaUnivHash<sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1> s_hFindNodeWithAttr(0);
        return &s_hFindNodeWithAttr;
#else
        static CMaaUnivHash<sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1> *p_hFindNodeWithAttr = TL_NEW CMaaUnivHash<sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1>(0);
        return p_hFindNodeWithAttr;
#endif
    }

    CMaaUnivHash<CMaaString, Attr*, true, 1> m_hAttrs, m_hStrings;
    CMaaUnivHash<CMaaString, NPtr, true, 1> m_hNode;
#ifdef DYNAMIC_HiddenDataPtrs
    CMaaUnivHashA<int, HiddenPtrsI, true, 1>* m_phHiddenDataPtrs;
#else
    CMaaUnivHash<int, HiddenPtrsI, true, 1> m_hHiddenDataPtrsI;
#endif
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    CMaaUnivHash<CMaaString, HiddenPtrs, true, 1> m_hHiddenDataPtrs;
#endif
    CMaaUnivHash<sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1>* m_phFindNodeWithAttr;

    static void RemHiddenPtrProcI(const int &Name, HiddenPtrsI& p, void* Param) noexcept;
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    static void RemHiddenPtrProc(const CMaaString& Name, HiddenPtrs& p, void* Param) noexcept;
#endif
public:
    enum
    {
        eRoot = 0,
        eProcInst,
        eDoctype,
        eElm
    };
    //bool IsProcessingInstruction();
    //bool IsElement();
    static CMaaXmlNodeImpl* FirstChild(const CMaaXmlNodeImpl* Node) noexcept;
    static CMaaXmlNodeImpl* LastChild(const CMaaXmlNodeImpl* Node) noexcept;
    static CMaaXmlNodeImpl* NextSibling(const CMaaXmlNodeImpl* Node) noexcept;
    static CMaaXmlNodeImpl* PrevSibling(const CMaaXmlNodeImpl* Node) noexcept;
    CMaaXmlNodeImpl* FindNext() const noexcept;
    CMaaXmlNodeImpl* FindPrev() const noexcept;
    /*
    static bool IsNull(const CMaaXmlNode& Node) noexcept
    {
        return !Node.m_Imp;
    }
    */
    CMaaXmlNodeImpl(int Type, const CMaaString &Name) noexcept(noexcept_new)
    :   m_pParent(nullptr),
        m_Type(Type),
        m_Name(Name),
        m_hAttrs(0, GetStaticAttrs(0), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
        m_hStrings(0, GetStaticAttrs(1), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
        m_hNode(0, GetStaticNPtr(), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
#ifdef DYNAMIC_HiddenDataPtrs
        m_phHiddenDataPtrs(nullptr),
#else
        m_hHiddenDataPtrsI(0, GetStaticHiddenDataPtrsI(), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
#endif
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
        m_hHiddenDataPtrs(0, GetStaticHiddenDataPtrs(), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
#endif
        //m_hFindNodeWithAttr(0, GetStaticFindNodeWithAttr(), 0)
        m_phFindNodeWithAttr(nullptr)
    {
#ifdef DYNAMIC_HiddenDataPtrs
        GetStaticHiddenDataPtrsI();
#endif
        GetStaticFindNodeWithAttr();
    }
    ~CMaaXmlNodeImpl();
    bool AddAttribute(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new);
    bool RemoveAttribute(const CMaaString &Name) noexcept;
    bool AddString(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new);
    bool RemoveString(const CMaaString &Name) noexcept;
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    bool SetString(const CMaaString &Value) noexcept; // @
    bool GetString(CMaaString &Value) const noexcept; // @
    const CMaaString& GetString(bool* pIsFound) const noexcept;
#endif
    int RemoveAllAttributes() noexcept; // returns number of attributes that was removed
    int RemoveAllStrings() noexcept; // returns number of strings that was removed
    bool AddProcessingInstruction(CMaaXmlNodeImpl * Node) noexcept(noexcept_new);
    bool AddNode(CMaaXmlNodeImpl* Node) noexcept(noexcept_new);
    bool RemNode(CMaaXmlNodeImpl* Node) noexcept(noexcept_new);
    void ChangeChildName(CMaaXmlNodeImpl* pThat, const CMaaString &Name) noexcept(noexcept_new);
    //CMaaString toText(int sp);
    const CMaaString& FindAttribute(const CMaaString& Name) const noexcept;
    const CMaaString& FindAttribute(const CMaaString& Name, bool* pIsFound /*= nullptr*/) const noexcept;
    const CMaaString& FindAttribute(const CMaaString& Name, bool& bIsFound) const noexcept;
    const CMaaString& FindString(const CMaaString& Name) const noexcept;
    const CMaaString& FindString(const CMaaString& Name, bool* pIsFound /*= nullptr*/) const noexcept;
    const CMaaString& FindString(const CMaaString& Name, bool& bIsFound) const noexcept;
    CMaaXmlNodeImpl * FindNode(const CMaaString& Name) const noexcept;
    CMaaXmlNodeImpl * FindNode(const CMaaString& Name, int& N) const noexcept;
    CMaaXmlNodeImpl * FindNode(const CMaaString& Name, int* pN /*= nullptr*/) const noexcept;
    CMaaXmlNodeImpl * FindNodeWithAttr(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNodeImpl* SkipNode = nullptr) const noexcept;
    CMaaXmlNodeImpl * FindNodeWithString(const CMaaString& Name, const CMaaString& StringName, const CMaaString& StringVal, const CMaaXmlNodeImpl* SkipNode = nullptr) const noexcept;
    int ChangeNodesAttr(const CMaaString& Name, const CMaaString& AttrName, CMaaString OldVal, const CMaaString& NewVal) noexcept(noexcept_new);
    int ChangeNodesString(const CMaaString& Name, const CMaaString& StringName, CMaaString OldVal, const CMaaString& NewVal) noexcept(noexcept_new);
    int RemoveNodesAttr(const CMaaString& Name, CMaaString AttrName, CMaaString Val) noexcept;
    int RemoveNodesString(const CMaaString& Name, CMaaString StringName, CMaaString Val) noexcept;
    int RemoveNodesWithAttr(const CMaaString& Name, CMaaString AttrName, CMaaString Val) noexcept(noexcept_new);
    int RemoveNodesWithString(const CMaaString& Name, CMaaString StringName, CMaaString Val) noexcept(noexcept_new);
    CMaaXmlNodeImpl * FindNodeWithAttrRO(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNodeImpl* SkipNode = nullptr, int Flags = 0) noexcept(noexcept_new); // for RO xml docs. Flags: 1 - cleanup cache, 2 - no negative cache storing
    int AddHiddenPtr(int Name, void* ptr, void (*deref)(int, void*, void*) noexcept /*= nullptr*/, void* obj = nullptr, bool bDerefOld = true) noexcept(noexcept_new);
    int AddHiddenPtr(int Name, void* ptr/*, bool bDerefOld = true*/) noexcept(noexcept_new);
    int RemoveHiddenPtr(int Name, bool bDeref /*= false*/) noexcept;
    int RemoveHiddenPtr(int Name/*, bool bDeref = false*/) noexcept;
    void* GetHiddenPtr(int Name) const noexcept;
    void* GetHiddenPtr(int Name, bool& bGot) const noexcept;
    void* GetHiddenPtr(int Name, bool* bGot /*= nullptr*/) const noexcept;
    int RemoveAllHiddenPtrs(bool bDeref /*= false*/) noexcept;
    int RemoveAllHiddenPtrs(/*bool bDeref = false*/) noexcept;
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    int AddHiddenPtr(const CMaaString &Name, void* ptr, void (*deref)(const CMaaString&, void*, void*) noexcept = nullptr, void* obj = nullptr, bool bDerefOld = true) noexcept(noexcept_new);
    int RemoveHiddenPtr(const CMaaString &Name, bool bDeref = false) noexcept;
    void* GetHiddenPtr(const CMaaString& Name) const noexcept;
    void* GetHiddenPtr(const CMaaString& Name, bool& bGot) const noexcept;
    void* GetHiddenPtr(const CMaaString& Name, bool* bGot /*= nullptr*/) const noexcept;
    //int RemoveAllHiddenPtrs(bool bDeref = false) noexcept;
#endif
    static bool AddBefore(CMaaXmlNodeImpl * n, CMaaXmlNodeImpl * x) noexcept(noexcept_new);
    static bool AddAfter(CMaaXmlNodeImpl * n, CMaaXmlNodeImpl * x) noexcept(noexcept_new);
#ifndef TOOLSLIB_ADD_HIDDEN_BY_STRING
    static int CopyAttributes(CMaaXmlNodeImpl *Dst, const CMaaXmlNodeImpl *Src, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept) = nullptr) noexcept(noexcept_new);
#else
    static int CopyAttributes(CMaaXmlNodeImpl *Dst, const CMaaXmlNodeImpl *Src, bool bHiddenPtrs = false, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept) = nullptr, void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept) = nullptr) noexcept(noexcept_new);
#endif
    static int CopyStrings(CMaaXmlNodeImpl *Dst, const CMaaXmlNodeImpl *Src) noexcept(noexcept_new);
    void ConvertToUtf8(_dword cp) noexcept(noexcept_new);
    static int EnumAttributes(CMaaXmlNodeImpl * Node, void (*stEnumAttrFn)(const CMaaString& AttrName, CMaaString& AttrValue, void* Param), void* Param);
    static int EnumStrings(CMaaXmlNodeImpl * Node, void (*stEnumAttrFn)(const CMaaString& AttrName, CMaaString& AttrValue, void* Param), void* Param);
    static int SortAttributes(CMaaXmlNodeImpl* Node) noexcept;
    static int SortStrings(CMaaXmlNodeImpl* Node) noexcept;
    const void* FirstStringIterator() const noexcept
    {
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
        return m_String.IsNotEmpty() ? (const void*)1 : m_StringsList.LookAtFront();
#else
        return m_StringsList.LookAtFront();
#endif
    }
    const void* FirstAttributeIterator() const noexcept
    {
        return m_AttrsList.LookAtFront();
    }
    bool GetString(const void* Iterator, CMaaString* pName, CMaaString* pValue) const noexcept
    {
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
        if  (Iterator == (const void*)1)
        {
            if  (m_String.IsNotEmpty())
            {
                if  (pName)
                {
                    *pName = CMaaStringEta;
                }
                if  (pValue)
                {
                    *pValue = m_String;
                }
                return true;
            }
            return false;
        }
#endif
        const Attr* ptr = (Attr*)Iterator;
        if  (ptr)
        {
            if  (pName)
            {
                *pName = ptr->m_Name;
            }
            if  (pValue)
            {
                *pValue = ptr->m_Value;
            }
            return true;
        }
        return false;
    }
    bool GetAttribute(const void* Iterator, CMaaString* pName, CMaaString* pValue) const noexcept
    {
        const Attr* ptr = (Attr*)Iterator;
        if  (ptr)
        {
            if  (pName)
            {
                *pName = ptr->m_Name;
            }
            if  (pValue)
            {
                *pValue = ptr->m_Value;
            }
            return true;
        }
        return false;
    }
    const void* NextStringIterator(const void* Iterator) const noexcept
    {
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
        if  (Iterator == (const void*)1)
        {
            return m_StringsList.LookAtFront();
        }
#endif
        return Iterator ? m_StringsList.Next((Attr*)Iterator) : nullptr;
    }
    const void* NextAttributeIterator(const void* Iterator) const noexcept
    {
        return Iterator ? m_AttrsList.Next((Attr*)Iterator) : nullptr;
    }

    static void AssignFast(CMaaXmlNodeImpl*& a, CMaaXmlNodeImpl*& b, bool bInc, bool bDel = MANUAL_XML_REFS) noexcept
    {
        //gCMaaSyncAssignLock.Lock();
        if (bInc)
        {
#ifdef TOOLSLIB_A_A_ASSIGN_OPT
            if (b == a)
            {
                return; // 03.11.2024
            }
#endif
            b && b->AddRef();
        }
        CMaaXmlNodeImpl* tmp = a;
        a = b;
        if  (tmp)
        {
            if  (!tmp->Release() && bDel)
            {
                delete tmp;
            }
        }
        //gCMaaSyncAssignLock.UnLock();
    }
    bool IsProcessingInstruction() const noexcept
    {
        return m_Type == CMaaXmlNodeImpl::eProcInst;
    }
    bool IsElement() const noexcept
    {
        return m_Type == CMaaXmlNodeImpl::eElm;
    }
private:
    ADD_ALLOCATOR(CMaaXmlNodeImpl)
};
//-------------------------------------------------------------------------------------------------
static constexpr_ CMaaXmlNodeImpl::Attr sEmptyAttr(false);
static constexpr CMaaXmlNodeImpl::NPtr sEmptyNPtr(false);
constexpr CMaaXmlNode::CE::CE() noexcept
{
}
static constexpr CMaaXmlNode::CE sCMaaXmlNodeC;
const CMaaXmlNode::CE& CMaaXmlNode::C() noexcept
{
    return sCMaaXmlNodeC;
}
//-------------------------------------------------------------------------------------------------
void Init_CMaaXmlNodeImpl_statics()
{
    //gCMaaToolLib_crt_Initializer();
    CMaaXmlNodeImpl::GetStaticAttrs(0);
    CMaaXmlNodeImpl::GetStaticAttrs(1);
    CMaaXmlNodeImpl::GetStaticNPtr();
    CMaaXmlNodeImpl::GetStaticHiddenDataPtrsI();
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    CMaaXmlNodeImpl::GetStaticHiddenDataPtrs();
#endif
    CMaaXmlNodeImpl::GetStaticFindNodeWithAttr();
}
//-------------------------------------------------------------------------------------------------
template <> _dword CMaaHashFunction<CMaaXmlNodeImpl::sFindNodeWithAttrKey>(const CMaaXmlNodeImpl::sFindNodeWithAttrKey& x) noexcept { return x.Hash(); }
template <> _uqword CMaaHashFunction64<CMaaXmlNodeImpl::sFindNodeWithAttrKey>(const CMaaXmlNodeImpl::sFindNodeWithAttrKey& x) noexcept { return x.Hash64(); }
class CMaaXmlNodeImpl;
static int SortFn1(const void * a1, const void * a2);

DEF_ALLOCATOR(CMaaXmlNodeImpl::Attr)
DEF_ALLOCATOR(CMaaXmlNodeImpl)

/*
CMaaUnivHash<CMaaString, CMaaXmlNodeImpl::Attr*, true, 1> CMaaXmlNodeImpl::s_hAttrs(0), CMaaXmlNodeImpl::s_hStrings(0);
CMaaUnivHash<CMaaString, CMaaXmlNodeImpl::NPtr, true, 1> CMaaXmlNodeImpl::s_hNode(0);
CMaaUnivHash<CMaaString, CMaaXmlNodeImpl::HiddenPtrsI, true, 1> CMaaXmlNodeImpl::s_hHiddenDataPtrsI(0);
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
CMaaUnivHash<CMaaString, CMaaXmlNodeImpl::HiddenPtrs, true, 1> CMaaXmlNodeImpl::s_hHiddenDataPtrs(0);
#endif
CMaaUnivHash<CMaaXmlNodeImpl::sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1> CMaaXmlNodeImpl::s_hFindNodeWithAttr(0);
*/

//-------------------------------------------------------------------------------------------------
// CMaaXmlNodeImpl
//-------------------------------------------------------------------------------------------------
CMaaXmlNodeImpl::~CMaaXmlNodeImpl()
{
    /*
     {
          FILE * f = fopen("c:\\out.txt", "a+b");
          if   (f)
          {
               fprintf(f, "Removing node %s\n", (const char *)m_Name);
               fclose(f);
          }
     }
     */
    //printf("deleting the node '%s'\n", (const char *)m_Name);
    RemoveAllHiddenPtrs(true);
    CMaaXmlNodeImpl *p;
    while((p = m_ChildsList.GetFromFront()))
    {
        p->m_pParent = nullptr;
        if  (!p->Release())
        {
#if MANUAL_XML_REFS
            delete p;
#endif
        }
    }
#ifdef DYNAMIC_HiddenDataPtrs
    delete m_phHiddenDataPtrs;
#endif
    delete m_phFindNodeWithAttr;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::FirstChild(const CMaaXmlNodeImpl* Node) noexcept
{
    return Node ? Node->m_ChildsList.LookAtFront() : nullptr;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::LastChild(const CMaaXmlNodeImpl* Node) noexcept
{
    return Node ? Node->m_ChildsList.LookAtBack() : nullptr;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::NextSibling(const CMaaXmlNodeImpl* Node) noexcept
{
    CMaaXmlNodeImpl* p;
    return Node && (p = Node->m_pParent) ? p->m_ChildsList.Next((CMaaXmlNodeImpl*)Node) : nullptr;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::PrevSibling(const CMaaXmlNodeImpl* Node) noexcept
{
    CMaaXmlNodeImpl* p;
    return Node && (p = Node->m_pParent) ? p->m_ChildsList.Prev((CMaaXmlNodeImpl*)Node) : nullptr;
}

/*
bool CMaaXmlNodeImpl::IsNull(const CMaaXmlNode &Node) noexcept
{
    return !Node.m_Imp;
}
*/

#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
bool CMaaXmlNodeImpl::SetString(const CMaaString &Value) noexcept
{
    if  ((m_ChildsList.IsNotEmpty() || m_StringsList.IsNotEmpty()) && Value.IsNotEmpty())
    {
        return false;
    }
    m_String = Value;
    return true;
}
bool CMaaXmlNodeImpl::GetString(CMaaString &Value) const noexcept
{
    Value = m_String;
    return m_ChildsList.IsEmpty() && m_StringsList.IsEmpty();
}
const CMaaString& CMaaXmlNodeImpl::GetString(bool* pIsFound) const noexcept
{
    if (pIsFound)
    {
        *pIsFound = m_String.IsNotEmpty();
    }
    return m_String;
}
#endif

bool CMaaXmlNodeImpl::AddAttribute(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
    Attr * a;
    if  ((a = m_hAttrs[Name, nullptr]))
    {
        a->m_Value = Value;
        m_AttrsList.Release(a);
    }
    else
    {
        a = TL_NEW Attr(Name, Value);
        if  (!a)
        {
            return false;
        }
        CMaaClassPtr<Attr> p(a);
        if (m_hAttrs.Add(Name, a))
        {
            //delete a; // ~p
            return false;
        }
        p.Reset();
    }
    m_AttrsList.AddAtBack(a);
    return true;
}

bool CMaaXmlNodeImpl::AddString(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
    if  (Name == CMaaStringEta)
    {
        if  ((m_ChildsList.IsNotEmpty() || m_StringsList.IsNotEmpty()) && Value.IsNotEmpty())
        {
            return false;
        }
        m_String = Value;
        return true;
    }
#endif
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    if  (m_String.IsNotEmpty())
    {
        return false;
    }
#endif

    Attr* a;
    if  ((a = m_hStrings[Name, nullptr]))
    {
        a->m_Value = Value;
        return true;
    }

    a = TL_NEW Attr(Name, Value);
    if  (!a)
    {
        return false;
    }
    CMaaClassPtr<Attr> p(a);
    if (m_hStrings.AddOver(Name, a))
    {
        //delete a; // ~p
        return false;
    }
    p.Reset();
    m_StringsList.AddAtBack(a);
    return true;
}

bool CMaaXmlNodeImpl::RemoveAttribute(const CMaaString &Name) noexcept
{
    Attr * a;
    if  (m_hAttrs.Remove(Name, &a))
    {
        return false;
    }
    m_AttrsList.Release(a);
    delete a;
    return true;
}

bool CMaaXmlNodeImpl::RemoveString(const CMaaString &Name) noexcept
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
    if  (Name == CMaaStringEta)
    {
        const bool r = m_String.IsNotEmpty();
        m_String.Empty();
        return r;
    }
#endif
    Attr * a;
    if  (m_hStrings.Remove(Name, &a))
    {
        return false;
    }
    m_StringsList.Release(a);
    delete a;
    return true;
}

int CMaaXmlNodeImpl::RemoveAllAttributes() noexcept
{
    int n = 0;
    m_hAttrs.Cleanup();
    Attr * a;
    while((a = m_AttrsList.GetFromFront()))
    {
        delete a;
        n++;
    }
    return n;
}

int CMaaXmlNodeImpl::RemoveAllStrings() noexcept
{
    int n = 0;
    m_hStrings.Cleanup();
    Attr * a;
    while((a = m_StringsList.GetFromFront()))
    {
        delete a;
        n++;
    }
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    if  (m_String.IsNotEmpty())
    {
        m_String.Empty();
        n++;
    }
#endif
    return n;
}

int CMaaXmlNodeImpl::AddHiddenPtr(int Name, void* ptr, void (*deref)(int, void*, void*) noexcept, void* obj, bool bDeref) noexcept(noexcept_new)
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        m_phHiddenDataPtrs = TL_NEW CMaaUnivHashA<int, HiddenPtrsI, true, 1>(3, GetStaticHiddenDataPtrsI(), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2);
        if (!m_phHiddenDataPtrs)
        {
            return 2; // 2 - Not enought free memory
        }
    }
    else
#endif
    {
#ifdef DYNAMIC_HiddenDataPtrs
        HiddenPtrsI* p = (*m_phHiddenDataPtrs)[Name];
#else
        HiddenPtrsI* p = m_hHiddenDataPtrsI[Name];
#endif
        if (p)
        {
            //if (p->m_Ptr == ptr && p->deref == deref && p->obj == obj)
            /*
            if (p->m_Ptr == ptr && !p->deref && !p->obj)
            {
                return 0;
            }
            */
            if (bDeref && p->deref)
            {
                p->deref(Name, p->m_Ptr, p->obj);
            }
            p->m_Ptr = ptr;
            p->deref = deref;
            p->obj = obj;
            return 0;
        }
    }
    const HiddenPtrsI p(ptr, deref, obj);
#ifdef DYNAMIC_HiddenDataPtrs
    return m_phHiddenDataPtrs->Add(Name, p);
#else
    return m_hHiddenDataPtrsI.Add(Name, p);
#endif
}
int CMaaXmlNodeImpl::AddHiddenPtr(int Name, void* ptr) noexcept(noexcept_new)
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        m_phHiddenDataPtrs = TL_NEW CMaaUnivHashA<int, HiddenPtrsI, true, 1>(3, GetStaticHiddenDataPtrsI(), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2);
        if (!m_phHiddenDataPtrs)
        {
            return 2; // 2 - Not enought free memory
        }
    }
    else
#endif
    {
#ifdef DYNAMIC_HiddenDataPtrs
        HiddenPtrsI* p = (*m_phHiddenDataPtrs)[Name];
#else
        HiddenPtrsI* p = m_hHiddenDataPtrsI[Name];
#endif
        if (p)
        {
            /*
            if (p->m_Ptr == ptr && !p->deref && !p->obj)
            {
                return 0;
            }
            */
            if (/*bDeref &&*/ p->deref)
            {
                p->deref(Name, p->m_Ptr, p->obj);
            }
            p->m_Ptr = ptr;
            p->deref = nullptr;
            p->obj = nullptr;
            return 0;
        }
    }
    const HiddenPtrsI p(ptr, nullptr, nullptr);
#ifdef DYNAMIC_HiddenDataPtrs
    return m_phHiddenDataPtrs->Add(Name, p);
#else
    return m_hHiddenDataPtrsI.Add(Name, p);
#endif
}
int CMaaXmlNodeImpl::RemoveHiddenPtr(int Name, bool bDeref) noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        return 1;
    }
    if (!bDeref)
    {
        return m_phHiddenDataPtrs->Remove(Name);
    }
    int Ret;
    HiddenPtrsI p;
    //int tmp = Name;
    if (!(Ret = m_phHiddenDataPtrs->Remove(Name, &p)) && p.deref)
    {
        p.deref(Name/*tmp*/, p.m_Ptr, p.obj);
    }
    return Ret;
#else
    if (!bDeref)
    {
        return m_hHiddenDataPtrsI.Remove(Name);
    }
    int Ret;
    HiddenPtrsI p(eNotInit);
    //int tmp = Name;
    if (!(Ret = m_hHiddenDataPtrsI.Remove(Name, &p)) && p.deref)
    {
        p.deref(Name/*tmp*/, p.m_Ptr, p.obj);
    }
    return Ret;
#endif
}
int CMaaXmlNodeImpl::RemoveHiddenPtr(int Name) noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        return 1;
    }
    return m_phHiddenDataPtrs->Remove(Name);
#else
    return m_hHiddenDataPtrsI.Remove(Name);
#endif
}
void* CMaaXmlNodeImpl::GetHiddenPtr(int Name) const noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        return nullptr;
    }
    HiddenPtrsI* p = (*m_phHiddenDataPtrs)[Name];
    return p ? p->m_Ptr : nullptr;
#else
    HiddenPtrsI* p = m_hHiddenDataPtrsI[Name];
    return p ? p->m_Ptr : nullptr;
#endif
}
void* CMaaXmlNodeImpl::GetHiddenPtr(int Name, bool& bGot) const noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        bGot = false;
        return nullptr;
    }
    HiddenPtrsI* p = (*m_phHiddenDataPtrs)[Name];
    if (p)
    {
        bGot = true;
        return p->m_Ptr;
    }
    bGot = false;
    return nullptr;
#else
    HiddenPtrsI* p = m_hHiddenDataPtrsI[Name];
    if (p)
    {
        bGot = true;
        return p->m_Ptr;
    }
    bGot = false;
    return nullptr;
#endif
}
void* CMaaXmlNodeImpl::GetHiddenPtr(int Name, bool* bGot) const noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    if (!m_phHiddenDataPtrs)
    {
        if (bGot)
        {
            *bGot = false;
        }
        return nullptr;
    }
    HiddenPtrsI* p = (*m_phHiddenDataPtrs)[Name];
    if (p)
    {
        if (bGot)
        {
            *bGot = true;
        }
        return p->m_Ptr;
    }
    if (bGot)
    {
        *bGot = false;
    }
    return nullptr;
#else
    HiddenPtrsI* p = m_hHiddenDataPtrsI[Name];
    if (p)
    {
        if (bGot)
        {
            *bGot = true;
        }
        return p->m_Ptr;
    }
    if (bGot)
    {
        *bGot = false;
    }
    return nullptr;
#endif
}
void CMaaXmlNodeImpl::RemHiddenPtrProcI(const int &Name, HiddenPtrsI&/*p*/, void* Param) noexcept
{
    CMaaXmlNodeImpl* x = (CMaaXmlNodeImpl*)Param;
    x->RemoveHiddenPtr(Name, true);
}
int CMaaXmlNodeImpl::RemoveAllHiddenPtrs(bool bDeref) noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    int n1;
    if (!m_phHiddenDataPtrs)
    {
        n1 = 0;
    }
    else
    {
        n1 = m_phHiddenDataPtrs->GetItemCount();
        if (!bDeref)
        {
            m_phHiddenDataPtrs->Cleanup();
        }
        else
        {
            m_phHiddenDataPtrs->EnumerateByProc(&RemHiddenPtrProcI, this);
        }
    }
#else
    const int n1 = m_hHiddenDataPtrsI.GetItemCount();
    if (!bDeref)
    {
        m_hHiddenDataPtrsI.Cleanup();
    }
    else
    {
        m_hHiddenDataPtrsI.EnumerateByProc(&RemHiddenPtrProcI, this);
    }
#endif
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    const int n2 = m_hHiddenDataPtrs.GetItemCount();
    if (!bDeref)
    {
        m_hHiddenDataPtrs.Cleanup();
    }
    else
    {
        m_hHiddenDataPtrs.EnumerateByProc(&RemHiddenPtrProc, this);
    }
    return n1 + n2;
#else
    return n1;
#endif
}
int CMaaXmlNodeImpl::RemoveAllHiddenPtrs() noexcept
{
#ifdef DYNAMIC_HiddenDataPtrs
    int n1;
    if (!m_phHiddenDataPtrs)
    {
        n1 = 0;
    }
    else
    {
        n1 = m_phHiddenDataPtrs->GetItemCount();
        m_phHiddenDataPtrs->Cleanup();
    }
#else
    const int n1 = m_hHiddenDataPtrsI.GetItemCount();
    m_hHiddenDataPtrsI.Cleanup();
#endif
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
    const int n2 = m_hHiddenDataPtrs.GetItemCount();
    m_hHiddenDataPtrs.Cleanup();
    return n1 + n2;
#else
    return n1;
#endif
}

#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
int CMaaXmlNodeImpl::AddHiddenPtr(const CMaaString &Name, void * ptr, void (*deref)(const CMaaString&, void *, void *) noexcept, void * obj, bool bDeref) noexcept(noexcept_new)
{
    HiddenPtrs* p = m_hHiddenDataPtrs[Name];
    if (p)
    {
        if (p->m_Ptr == ptr && p->deref == deref && p->obj == obj)
        {
            return 0;
        }
        if (bDeref && p->deref)
        {
            p->deref(Name, p->m_Ptr, p->obj);
        }
        p->m_Ptr = ptr;
        p->deref = deref;
        p->obj = obj;
        return 0;
    }
    else
    {
        HiddenPtrs p;
        p.m_Ptr = ptr;
        p.deref = deref;
        p.obj = obj;
        return m_hHiddenDataPtrs.Add(Name, p);
    }
}
int CMaaXmlNodeImpl::RemoveHiddenPtr(const CMaaString &Name, bool bDeref) noexcept
{
    if (!bDeref)
    {
        return m_hHiddenDataPtrs.Remove(Name);
    }
    int Ret;
    HiddenPtrs p;
    CMaaString tmp = Name;
    if  (!(Ret = m_hHiddenDataPtrs.Remove(Name, &p)) && p.deref)
    {
        p.deref(tmp, p.m_Ptr, p.obj);
    }
    return Ret;
}
void * CMaaXmlNodeImpl::GetHiddenPtr(const CMaaString &Name) const noexcept
{
    HiddenPtrs* p = m_hHiddenDataPtrs[Name];
    return p ? p->m_Ptr : nullptr;
}
void* CMaaXmlNodeImpl::GetHiddenPtr(const CMaaString& Name, bool& bGot) const noexcept
{
    HiddenPtrs* p = m_hHiddenDataPtrs[Name];
    if (p)
    {
        bGot = true;
        return p->m_Ptr;
    }
    bGot = false;
    return nullptr;
}
void* CMaaXmlNodeImpl::GetHiddenPtr(const CMaaString& Name, bool* bGot) const noexcept
{
    HiddenPtrs* p = m_hHiddenDataPtrs[Name];
    if (p)
    {
        if (bGot)
        {
            *bGot = true;
        }
        return p->m_Ptr;
    }
    if (bGot)
    {
        *bGot = false;
    }
    return nullptr;
}
void CMaaXmlNodeImpl::RemHiddenPtrProc(const CMaaString &Name, HiddenPtrs &/*p*/, void *Param) noexcept
{
    CMaaXmlNodeImpl * x = (CMaaXmlNodeImpl *)Param;
    x->RemoveHiddenPtr(Name, true);
}
/*
int CMaaXmlNodeImpl::RemoveAllHiddenPtrs(bool bDeref) noexcept
{
    const int n = m_hHiddenDataPtrs.GetItemCount();
    if  (!bDeref)
    {
        m_hHiddenDataPtrs.Cleanup();
    }
    else
    {
        m_hHiddenDataPtrs.EnumerateByProc(&RemHiddenPtrProc, this);
    }
    return n;
}
*/
#endif

bool CMaaXmlNodeImpl::AddProcessingInstruction(CMaaXmlNodeImpl* Node) noexcept(noexcept_new)
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    if  (m_String.IsNotEmpty())
    {
        return false;
    }
#endif
    if  (!Node // empty node
        || Node == this // can't be added to itself
        || Node->m_pParent // already added
        )
    {
        return false;
    }
    NPtr p{ Node, 0 };
    m_hNode.Find(Node->m_Name, &p);
    p.m_First = Node;
    p.N++;
    m_hNode.AddOver(Node->m_Name, p);

    Node->AddRef();
    m_ChildsList.AddAtFront(Node);
    Node->m_pParent = this;

    return true;
}

bool CMaaXmlNodeImpl::AddNode(CMaaXmlNodeImpl *Node) noexcept(noexcept_new)
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    if  (m_String.IsNotEmpty())
    {
        return false;
    }
#endif
    if  (!Node // empty node
        || Node == this // can't be added to itself
        || Node->m_pParent // already added
        )
    {
        return false;
    }

    NPtr *p = m_hNode[Node->m_Name];
    if (p)
    {
        p->N++;
    }
    else
    {
        const NPtr p{ Node, 1 };
        m_hNode.AddOver(Node->m_Name, p);
    }
    Node->AddRef();
    m_ChildsList.AddAtBack(Node);
    Node->m_pParent = this;
    return true;
}

static void deref_string_imp(int, void* ptr, void* /*obj*/) noexcept
{
    CMaaString::Release((CMaaString::CMaaStringImp*)ptr);
}

static void* copy_string_imp(int, void* ptr, void** /*&obj*/)
{
    CMaaString::AddRef((CMaaString::CMaaStringImp*)ptr);
    return ptr;
}

static void deref_string(int, void* ptr, void* /*obj*/) noexcept
{
    delete (CMaaString*)ptr;
}

static void* copy_string(int, void* ptr, void** /*&obj*/)
{
    return TL_NEW CMaaString(*(CMaaString*)ptr);
}

#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
static void deref_string(const CMaaString&, void* ptr, void* /*obj*/) noexcept
{
    delete (CMaaString*)ptr;
}

static void* copy_string(const CMaaString&, void* ptr, void** /*&obj*/)
{
    return TL_NEW CMaaString(*(CMaaString*)ptr);
}
#endif

#ifndef TOOLSLIB_ADD_HIDDEN_BY_STRING
int CMaaXmlNodeImpl::CopyAttributes(CMaaXmlNodeImpl *Dst, const CMaaXmlNodeImpl *Src, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)) noexcept(noexcept_new)
{
    int Count = 0;
    if (Dst && Src)
    {
        CMaaXmlNodeImpl* s = (CMaaXmlNodeImpl*)Src;
        //if (s)
        {
            for (Attr* a = s->m_AttrsList.LookAtFront(); a; a = s->m_AttrsList.Next(a))
            {
                Count++;
                Dst->AddAttribute(a->m_Name, a->m_Value);
            }
        }
        if (bHiddenPtrs
#ifdef DYNAMIC_HiddenDataPtrs
            && s->m_phHiddenDataPtrs
#endif
            )
        {
#ifdef DYNAMIC_HiddenDataPtrs
            CMaaUnivHash<int, HiddenPtrsI, true, 1>::iterator it(*s->m_phHiddenDataPtrs);
#else
            CMaaUnivHash<int, HiddenPtrsI, true, 1>::iterator it(s->m_hHiddenDataPtrsI);
#endif
            for (; it; ++it)
            {
                void* ptr, * obj;
                if (!it.rdata().deref)
                {
                    //obj = it.rdata().obj;
                    obj = nullptr;
                    ptr = it.rdata().m_Ptr;
                }
                else if (it.rdata().deref == deref_string_imp)
                {
                    obj = it.rdata().obj;
                    ptr = copy_string_imp(it.ckey(), it.rdata().m_Ptr, &obj);
                }
                else if (it.rdata().deref == deref_string)
                {
                    obj = it.rdata().obj;
                    ptr = copy_string(it.ckey(), it.rdata().m_Ptr, &obj);
                }
                else if (!copyref)
                {
                    continue;
                }
                else
                {
                    obj = it.rdata().obj;
                    ptr = copyref(it.ckey(), it.rdata().m_Ptr, &obj, it.rdata().deref);
                }
                Dst->AddHiddenPtr(it.ckey(), ptr, it.rdata().deref, obj, true);
            }
        }
    }
    return Count;
}
#else
int CMaaXmlNodeImpl::CopyAttributes(CMaaXmlNode &Dst, const CMaaXmlNode &Src, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept), void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)) noexcept(noexcept_new)
{
    int Count = 0;
    CMaaXmlNodeImpl * s = Src.m_Imp;
    if  (s)
    {
        for (Attr* a = s->m_AttrsList.LookAtFront(); a; a = s->m_AttrsList.Next(a))
        {
            Count++;
            Dst.AddAttribute(a->m_Name, a->m_Value);
        }
    }
    if (bHiddenPtrs 
#ifdef DYNAMIC_HiddenDataPtrs
        && s->m_phHiddenDataPtrs
#endif
        )
    {
#ifdef DYNAMIC_HiddenDataPtrs
        CMaaUnivHash<int, HiddenPtrsI, true, 1>::iterator it(*s->m_phHiddenDataPtrs);
#else
        CMaaUnivHash<int, HiddenPtrsI, true, 1>::iterator it(*s->m_hHiddenDataPtrsI);
#endif
        for (; it; ++it)
        {
            void* ptr, * obj;
            if (!it.rdata().deref)
            {
                //obj = it.rdata().obj;
                obj = nullptr;
                ptr = it.rdata().m_Ptr;
            }
            else if (it.rdata().deref == deref_string_imp)
            {
                obj = it.rdata().obj;
                ptr = copy_string_imp(it.ckey(), it.rdata().m_Ptr, &obj);
            }
            else if (it.rdata().deref == deref_string)
            {
                obj = it.rdata().obj;
                ptr = copy_string(it.ckey(), it.rdata().m_Ptr, &obj);
            }
            else if (!copyref)
            {
                continue;
            }
            else
            {
                obj = it.rdata().obj;
                ptr = copyref(it.ckey(), it.rdata().m_Ptr, &obj, it.rdata().deref);
            }
            Dst.AddHiddenPtr(it.ckey(), ptr, it.rdata().deref, obj, true);
        }
    }
    if (bHiddenPtrs)
    {
        CMaaUnivHash<CMaaString, HiddenPtrs, true, 1>::iterator it(s->m_hHiddenDataPtrs);
        for (; it; ++it)
        {
            void * ptr, * obj;
            if (!it.rdata().deref)
            {
                //obj = it.rdata().obj;
                obj = nullptr;
                ptr = it.rdata().m_Ptr;
            }
            else if (it.rdata().deref == deref_string)
            {
                obj = it.rdata().obj;
                ptr = copy_string(it.ckey(), it.rdata().m_Ptr, &obj);
            }
            else if (!copyref)
            {
                continue;
            }
            else
            {
                obj = it.rdata().obj;
                ptr = copyref_(it.ckey(), it.rdata().m_Ptr, &obj, it.rdata().deref);
            }
            Dst.AddHiddenPtr(it.ckey(), ptr, it.rdata().deref, obj, true);
        }
    }
    return Count;
}
#endif

int CMaaXmlNodeImpl::CopyStrings(CMaaXmlNodeImpl *Dst, const CMaaXmlNodeImpl *Src) noexcept(noexcept_new)
{
    int Count = 0;
    if (Dst && Src)
    {
        const CMaaXmlNodeImpl* s = Src;
        //if (s)
        {
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
            Dst->SetString(s->m_String);
#endif
            for (Attr* a = s->m_StringsList.LookAtFront(); a; a = s->m_StringsList.Next(a))
            {
                Count++;
                Dst->AddString(a->m_Name, a->m_Value);
            }
        }
    }
    return Count;
}

int CMaaXmlNodeImpl::EnumAttributes(CMaaXmlNodeImpl *Node, void (*stEnumAttrFn)(const CMaaString &AttrName, CMaaString &AttrValue, void *Param), void * Param)
{
    int Count = 0;
    const CMaaXmlNodeImpl* s = Node;
    if  (s)
    {
        Attr* nxt;
        for (Attr* a = s->m_AttrsList.LookAtFront(); a; a = nxt)
        {
            nxt = s->m_AttrsList.Next(a);
            Count++;
            if  (stEnumAttrFn)
            {
                stEnumAttrFn(a->m_Name, a->m_Value, Param);
            }
        }
    }
    return Count;
}

int CMaaXmlNodeImpl::EnumStrings(CMaaXmlNodeImpl *Node, void (*stEnumAttrFn)(const CMaaString &AttrName, CMaaString &AttrValue, void *Param), void * Param)
{
    int Count = 0;
    CMaaXmlNodeImpl* s = Node;
    if  (s)
    {
        Attr* nxt;
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
        if  (s->m_String.IsNotEmpty())
        {
            Count++;
            if  (stEnumAttrFn)
            {
                stEnumAttrFn(CMaaStringEta, s->m_String, Param);
            }
        }
#endif
        for (Attr* a = s->m_StringsList.LookAtFront(); a; a = nxt)
        {
            nxt = s->m_StringsList.Next(a);
            Count++;
            if  (stEnumAttrFn)
            {
                stEnumAttrFn(a->m_Name, a->m_Value, Param);
            }
        }
    }
    return Count;
}

//-------------------------------------------------------------------------------------------------
static int SortFn1(const void * a1, const void * a2)
{
    CMaaXmlNodeImpl::Attr *a = *(CMaaXmlNodeImpl::Attr **)a1;
    CMaaXmlNodeImpl::Attr *b = *(CMaaXmlNodeImpl::Attr **)a2;
    //return CMaaString::Compare(a->m_Name, b->m_Name);
    return CMaaString::Compare(a->m_Name, b->m_Name, 2);
}
//-------------------------------------------------------------------------------------------------
int CMaaXmlNodeImpl::SortAttributes(CMaaXmlNodeImpl* Node) noexcept
{
    CMaaXmlNodeImpl* s = Node;
    if  (s)
    {
        const int Count = s->m_AttrsList.GetCount();
        CMaaPtr_<Attr*, -1> m(Count + 1);
        if  (m.IsValid())
        {
            int i;
            Attr* a = s->m_AttrsList.LookAtFront();
            for (i = 0; i < Count && a; i++, a = s->m_AttrsList.Next(a))
            {
                m[i] = a;
            }
            qsort(&m[0], i, sizeof(Attr*), SortFn1);
            while (i--)
            {
                a = m[i];
                s->m_AttrsList.Release(a);
                s->m_AttrsList.AddAtFront(a);
            }
            return Count;
        }
        return -1;
    }
    return 0;
}
/*
CMaaXmlDocument Diff(CMaaXmlDocument Src, CMaaXmlDocument Dst)
{
     CMaaXmlDocument diff("diff");
     CMaaXmlElement s = Src.DocumentElement();
     CMaaXmlElement d = Dst.DocumentElement();
     CMaaXmlElement e = diff.CreateElement("diff");
     diff.AppendChild(e);
     CMaaXmlElement Add = diff.CreateElement("Add");
     CMaaXmlElement AddVal = diff.CreateElement("AddVal");
     CMaaXmlElement RemVal = diff.CreateElement("RemVal");
     CMaaXmlElement Rem = diff.CreateElement("Rem");
     Diff(diff, s, d, Add, AddVal, RemVal, Rem);
     if   (Add.FirstChild
     return diff;
}

CMaaXmlDocument Diff(CMaaXmlDocument Src, CMaaXmlDocument Dst)
{
     CMaaXmlDocument diff("diff");
     CMaaXmlElement s = Src.DocumentElement();
     CMaaXmlElement d = Dst.DocumentElement();
     Diff(diff, s, d);
     return diff;
}

CMaaXmlDocument Patch(CMaaXmlDocument Src, CMaaXmlDocument Diff)
{
}
*/
int CMaaXmlNodeImpl::SortStrings(CMaaXmlNodeImpl* Node) noexcept
{
    CMaaXmlNodeImpl* s = Node;
    if  (s)
    {
        const int Count = s->m_StringsList.GetCount();
        CMaaPtr_<Attr*, -1> m(Count + 1);
        if  (m.IsValid())
        {
            int i;
            Attr* a = s->m_StringsList.LookAtFront();
            for (i = 0; i < Count && a; i++, a = s->m_StringsList.Next(a))
            {
                m[i] = a;
            }
            qsort(&m[0], i, sizeof(Attr*), SortFn1);
            while (i--)
            {
                a = m[i];
                s->m_StringsList.Release(a);
                s->m_StringsList.AddAtFront(a);
            }
            return Count;
        }
        return -1;
    }
    return 0;
}


bool CMaaXmlNodeImpl::AddBefore(CMaaXmlNodeImpl * N, CMaaXmlNodeImpl * X) noexcept(noexcept_new)
{
    if  (N && X && !X->m_pParent && N->m_pParent && X->m_Name == N->m_Name)
    {
        X->AddRef();
        X->InsertBefore(N);

        CMaaXmlNodeImpl* P = X->m_pParent = N->m_pParent;
        NPtr * p = P->m_hNode[N->m_Name];
        if (p)
        {
            p->N++;
            if  (p->m_First == N)
            {
                p->m_First = X;
            }
        }
        return true;
    }
    return false;
}

bool CMaaXmlNodeImpl::AddAfter(CMaaXmlNodeImpl * N, CMaaXmlNodeImpl * X) noexcept(noexcept_new)
{
    if  (N && X && !X->m_pParent && N->m_pParent && X->m_Name == N->m_Name)
    {
        X->AddRef();
        X->InsertAfter(N);

        CMaaXmlNodeImpl* P = X->m_pParent = N->m_pParent;
        NPtr * p = P->m_hNode[N->m_Name];
        if (p)
        {
            p->N++;
        }
        return true;
    }
    return false;
}

void CMaaXmlNodeImpl::ChangeChildName(CMaaXmlNodeImpl * pThat, const CMaaString &Name) noexcept(noexcept_new)
{
    if  (!pThat || pThat->m_Name == Name || pThat->m_pParent != this)
    {
        return;
    }
    NPtr* p = m_hNode[pThat->m_Name];
    if  (p)
    {
        if  (!--p->N)
        {
            m_hNode.Remove(pThat->m_Name);
        }
        else if (p->m_First == pThat)
        {
            CMaaXmlNodeImpl * x = m_ChildsList.Next(pThat);
            while(x)
            {
                if  (x->m_Name == pThat->m_Name)
                {
                    break;
                }
                x = m_ChildsList.Next(x);
            }
            if  (!x)
            {
                // error
                m_hNode.Remove(pThat->m_Name);
            }
            else
            {
                p->m_First = x;
            }
        }
    }

    p = m_hNode[(pThat->m_Name = Name)];
    if  (!p)
    {
        const NPtr p2{ pThat, 1 }; // p2.m_First = pThat; p2.N = 1;
        m_hNode.AddOver(Name, p2);
    }
    else
    {
        CMaaXmlNodeImpl * x = p->m_First;
        while(x && x != pThat)
        {
            x = m_ChildsList.Prev(x);
        }
        p->N++;
        if  (x)
        {
            p->m_First = pThat;
        }
    }
}

bool CMaaXmlNodeImpl::RemNode(CMaaXmlNodeImpl* pNode) noexcept(noexcept_new)
{
    if  (!pNode || pNode->m_pParent != this)
    {
        return false;
    }

    NPtr * p = m_hNode[pNode->m_Name];
    if (p)
    {
        if (!--p->N)
        {
            m_hNode.Remove(pNode->m_Name);
        }
        else
        {
            if (p->m_First == pNode)
            {
                p->m_First = pNode->FindNext();
            }
        }
    }
    m_ChildsList.Release(pNode);

    pNode->m_pParent = nullptr;
    if  (!pNode->Release())
    {
#if MANUAL_XML_REFS
        delete pNode;
#endif
    }
    return true;
}

CMaaXmlNodeImpl * CMaaXmlNodeImpl::FindNext() const noexcept
{
    CMaaXmlNodeImpl * Ret = NextSibling(this);
    while (Ret && Ret->m_Name != m_Name) //while (Ret && !Name.EqEx(Ret->m_Name))
    {
        Ret = NextSibling(Ret);
    }
    return Ret;
}

CMaaXmlNodeImpl * CMaaXmlNodeImpl::FindPrev() const noexcept
{
    CMaaXmlNodeImpl * Ret = PrevSibling(this);
    while (Ret && Ret->m_Name != m_Name) //while (Ret && !Name.EqEx(Ret->m_Name))
    {
        Ret = PrevSibling(Ret);
    }
    return Ret;
}

const CMaaString& CMaaXmlNodeImpl::FindAttribute(const CMaaString& Name) const noexcept
{
    return m_hAttrs[Name, (Attr*)&sEmptyAttr]->m_Value;
}

const CMaaString& CMaaXmlNodeImpl::FindAttribute(const CMaaString &Name, bool *pIsFound) const noexcept
{
    Attr* a;
    if  ((a = m_hAttrs[Name, nullptr]))
    {
        if (pIsFound)
        {
            *pIsFound = true;
        }
        return a->m_Value;
    }
    if (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}

const CMaaString& CMaaXmlNodeImpl::FindAttribute(const CMaaString& Name, bool& bIsFound) const noexcept
{
    Attr* a;
    if ((a = m_hAttrs[Name, nullptr]))
    {
        bIsFound = true;
        return a->m_Value;
    }
    bIsFound = false;
    return gsCMaaStringZ;
}

const CMaaString& CMaaXmlNodeImpl::FindString(const CMaaString& Name) const noexcept
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
    if (Name == CMaaStringEta)
    {
        return m_String;
    }
#endif
    return m_hStrings[Name, (Attr*)&sEmptyAttr]->m_Value;
}

const CMaaString& CMaaXmlNodeImpl::FindString(const CMaaString &Name, bool *pIsFound) const noexcept
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
    if  (Name == CMaaStringEta)
    {
        if  (pIsFound)
        {
            *pIsFound = m_String.IsNotEmpty();
        }
        return m_String;
    }
#endif
    Attr* a;
    if  ((a = m_hStrings[Name, nullptr]))
    {
        if (pIsFound)
        {
            *pIsFound = true;
        }
        return a->m_Value;
    }
    if  (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}

const CMaaString& CMaaXmlNodeImpl::FindString(const CMaaString& Name, bool& bIsFound) const noexcept
{
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT__
    if (Name == CMaaStringEta)
    {
        bIsFound = m_String.IsNotEmpty();
        return m_String;
    }
#endif
    Attr* a;
    if ((a = m_hStrings[Name, nullptr]))
    {
        bIsFound = true;
        return a->m_Value;
    }
    bIsFound = false;
    return gsCMaaStringZ;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::FindNode(const CMaaString &Name) const noexcept
{
    return m_hNode[Name, sEmptyNPtr, false].m_First;
}
CMaaXmlNodeImpl* CMaaXmlNodeImpl::FindNode(const CMaaString& Name, int &N) const noexcept
{
    const NPtr &p = m_hNode[Name, sEmptyNPtr, false];
    N = p.N;
    return p.m_First;
}
CMaaXmlNodeImpl* CMaaXmlNodeImpl::FindNode(const CMaaString& Name, int* pN) const noexcept
{
    const NPtr &p = m_hNode[Name, sEmptyNPtr, false];
    if (pN)
    {
        *pN = p.N;
    }
    return p.m_First;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::FindNodeWithAttr(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNodeImpl* SkipNode) const noexcept
{
    CMaaXmlNodeImpl* n = FindNode(Name);
    for (; n; n = n->FindNext())
    {
        if (n != SkipNode)
        {
            bool IsFound;
            if (n->FindAttribute(AttrName, IsFound) == AttrVal && IsFound)
            {
                break;
            }
        }
    }
    return n;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::FindNodeWithString(const CMaaString& Name, const CMaaString& StringName, const CMaaString& StringVal, const CMaaXmlNodeImpl* SkipNode) const noexcept
{
    CMaaXmlNodeImpl* n = FindNode(Name);
    for (; n; n = n->FindNext())
    {
        if (n != SkipNode)
        {
            bool IsFound;
            if (n->FindString(StringName, IsFound) == StringVal && IsFound)
            {
                break;
            }
        }
    }
    return n;
}

int CMaaXmlNodeImpl::ChangeNodesAttr(const CMaaString& Name, const CMaaString& AttrName, CMaaString OldVal, const CMaaString& NewVal) noexcept(noexcept_new)
{
    int N = 0;
    for (CMaaXmlNodeImpl* n = FindNode(Name); n; n = n->FindNext())
    {
        bool IsFound;
        if (n->FindAttribute(AttrName, IsFound) == OldVal && IsFound)
        {
            n->AddAttribute(AttrName, NewVal);
            N++;
        }
    }
    return N;
}

int CMaaXmlNodeImpl::ChangeNodesString(const CMaaString& Name, const CMaaString& StringName, CMaaString OldVal, const CMaaString& NewVal) noexcept(noexcept_new)
{
    int N = 0;
    for (CMaaXmlNodeImpl* n = FindNode(Name); n; n = n->FindNext())
    {
        bool IsFound;
        if (n->FindString(StringName, IsFound) == OldVal && IsFound)
        {
            n->AddString(StringName, NewVal);
            N++;
        }
    }
    return N;
}

int CMaaXmlNodeImpl::RemoveNodesAttr(const CMaaString& Name, CMaaString AttrName, CMaaString Val) noexcept
{
    int N = 0;
    for (CMaaXmlNodeImpl* n = FindNode(Name); n; n = n->FindNext())
    {
        bool IsFound;
        if (n->FindAttribute(AttrName, IsFound) == Val && IsFound)
        {
            n->RemoveAttribute(AttrName);
            N++;
        }
    }
    return N;
}

int CMaaXmlNodeImpl::RemoveNodesString(const CMaaString& Name, CMaaString StringName, CMaaString Val) noexcept
{
    int N = 0;
    for (CMaaXmlNodeImpl* n = FindNode(Name); n; n = n->FindNext())
    {
        bool IsFound;
        if (n->FindString(StringName, IsFound) == Val && IsFound)
        {
            n->RemoveString(StringName);
            N++;
        }
    }
    return N;
}

int CMaaXmlNodeImpl::RemoveNodesWithAttr(const CMaaString& Name, CMaaString AttrName, CMaaString Val) noexcept(noexcept_new)
{
    int N = 0;
    CMaaXmlNodeImpl* n = FindNode(Name), * nn;
    for (; n; n = nn)
    {
        nn = n->FindNext();
        bool IsFound;
        if (n->FindAttribute(AttrName, IsFound) == Val && IsFound)
        {
            RemNode(n);
            N++;
        }
    }
    return N;
}

int CMaaXmlNodeImpl::RemoveNodesWithString(const CMaaString& Name, CMaaString StringName, CMaaString Val) noexcept(noexcept_new)
{
    int N = 0;
    CMaaXmlNodeImpl* n = FindNode(Name), * nn;
    for (; n; n = nn)
    {
        nn = n->FindNext();
        bool IsFound;
        if (n->FindString(StringName, IsFound) == Val && IsFound)
        {
            RemNode(n);
            N++;
        }
    }
    return N;
}

CMaaXmlNodeImpl* CMaaXmlNodeImpl::FindNodeWithAttrRO(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNodeImpl* SkipNode, int Flags) noexcept(noexcept_new)
{
    static constexpr CMaaAtomicFastMutex0 sMutex;

    CMaaManualAtomicFastMutexLocker Locker((CMaaAtomicFastMutex0&)sMutex);
    Locker.Lock();
    if (!m_phFindNodeWithAttr)
    {
        try
        {
            m_phFindNodeWithAttr = TL_NEW CMaaUnivHash<CMaaXmlNodeImpl::sFindNodeWithAttrKey, CMaaXmlNodeImpl*, true, 1>(0, CMaaXmlNodeImpl::GetStaticFindNodeWithAttr(), 0 TOOLSLIB_CHECK_HT_USAGE_ARG2);
        }
        catch (...)
        {
        }
        if (!m_phFindNodeWithAttr)
        {
            Locker.UnLock();
            return nullptr;
        }
    }
    if ((Flags & 1))
    {
        m_phFindNodeWithAttr->Cleanup();
        Locker.UnLock();
        return nullptr;
    }
    CMaaXmlNodeImpl::sFindNodeWithAttrKey k(Name, AttrName, AttrVal);
    CMaaXmlNodeImpl* p;
    if (!m_phFindNodeWithAttr->Find(k, &p))
    {
        Locker.UnLock();
        if (p && p == SkipNode)
        {
            for (p = p->FindNext(); p; p = p->FindNext())
            {
                bool IsFound;
                if (p->FindAttribute(AttrName, IsFound) == AttrVal && IsFound)
                {
                    break;
                }
            }
        }
    }
    else
    {
        Locker.UnLock();
        bool b1st = true;
        for (p = FindNode(Name); p; p = p->FindNext())
        {
            bool IsFound;
            if (p->FindAttribute(AttrName, IsFound) == AttrVal && IsFound)
            {
                if (b1st)
                {
                    b1st = false;
                    Locker.Lock();
                    m_phFindNodeWithAttr->Add(k, p);
                    Locker.UnLock();
                }
                if (p != SkipNode)
                {
                    return p;
                }
            }
        }
        if (b1st && !(Flags & 2))
        {
            Locker.Lock();
            m_phFindNodeWithAttr->Add(k, nullptr);
            Locker.UnLock();
        }
    }
    return p;
}

//-------------------------------------------------------------------------------------------------
struct strEnum
{
    strEnum(CMaaXmlNode::EnumAttributesFn _Fn, CMaaXmlNode _Node, void* _Param) noexcept
    :   Fn(_Fn),
        Node(_Node),
        Param(_Param)
    {
    }
    CMaaXmlNode::EnumAttributesFn Fn;
    CMaaXmlNode Node;
    void* Param;
};
void CMaaXmlNode::stEnumAttrFn(const CMaaString &AttrName, CMaaString &AttrValue, void* Param)
{
    strEnum* e = (strEnum*)Param;
    e->Fn(AttrName, AttrValue, e->Node, e->Param);
}
//-------------------------------------------------------------------------------------------------
// CMaaXmlNode
//-------------------------------------------------------------------------------------------------
CMaaXmlNode::CMaaXmlNode(int Type, const CMaaString &Name) noexcept(noexcept_new)
:   m_Imp(TL_NEW CMaaXmlNodeImpl(Type, Name))
{
}
CMaaXmlNode::CMaaXmlNode(const CMaaXmlNode & That) noexcept
{
    if  ((m_Imp = (CMaaXmlNodeImpl*)That.m_Imp))
    {
        m_Imp->AddRef();
    }
}
/*
CMaaXmlNode::CMaaXmlNode(CMaaXmlNode&& That) noexcept
{
    m_Imp = That.m_Imp;
    That.m_Imp = nullptr;
}
*/
CMaaXmlNode & CMaaXmlNode::operator=(const CMaaXmlNode & That) noexcept
{
    CMaaXmlNodeImpl::AssignFast(m_Imp, (CMaaXmlNodeImpl *&)That.m_Imp, true, MANUAL_XML_REFS);
    return *this;
}
CMaaXmlNode& CMaaXmlNode::operator=(CMaaXmlNode&& That) noexcept
{
    if  (m_Imp != That.m_Imp)
    {
        CMaaXmlNodeImpl* pImp = m_Imp;
        m_Imp = That.m_Imp;
        That.m_Imp = nullptr;
        if  (pImp)
        {
            if  (!pImp->Release())
            {
#if MANUAL_XML_REFS
                delete pImp;
#endif
            }
        }
    }
    return *this;
}

bool CMaaXmlNode::AddProcessingInstruction(CMaaXmlNode & Node) noexcept(noexcept_new)
{
    return m_Imp && m_Imp->AddProcessingInstruction(Node.m_Imp);
}
bool CMaaXmlNode::AddNode(CMaaXmlNode &Node) noexcept(noexcept_new)
{
    return m_Imp && m_Imp->AddNode(Node.m_Imp);
}
bool CMaaXmlNode::RemNode(CMaaXmlNode &Node) noexcept(noexcept_new)
{
    return m_Imp && m_Imp->RemNode(Node.m_Imp);
}

bool CMaaXmlNode::AddAttribute(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
    return m_Imp && m_Imp->AddAttribute(Name, Value);
}
#if 0
template <int N> bool CMaaXmlNode::AddAttribute(const char (&Name)[N], const CMaaString &Value)
{
    return m_Imp ? m_Imp->AddAttribute(CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N), CMaaString::eROMemString0), Value) : false;
}
template <int N> bool CMaaXmlNode::AddAttribute(const CMaaString &Name, const char (&Value)[N])
{
    return m_Imp ? m_Imp->AddAttribute(Name, CMaaString(Value, GET_CHECK_TXT_N1_N(Value, N), CMaaString::eROMemString0)) : false;
}
template <int N1, int N2> bool CMaaXmlNode::AddAttribute(const char (&Name)[N1], const char (&Value)[N2])
{
    return m_Imp ? m_Imp->AddAttribute(
        CMaaString(Name, GET_CHECK_TXT_N1_N(Name, N1), CMaaString::eROMemString0),
        CMaaString(Value, GET_CHECK_TXT_N1_N(Value, N2), CMaaString::eROMemString0)
        ) : false;
}
#endif
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
bool CMaaXmlNode::SetString(const CMaaString& Value) noexcept
{
    return m_Imp && m_Imp->SetString(Value);
}
#endif
bool CMaaXmlNode::AddString(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
    return m_Imp && m_Imp->AddString(Name, Value);
}
bool CMaaXmlNode::RemoveAttribute(const CMaaString &Name) noexcept
{
    return m_Imp && m_Imp->RemoveAttribute(Name);
}
#ifdef TOOLSLIB_XML_CCHAR_PTR
bool CMaaXmlNode::RemoveAttribute(const char* pszName) noexcept
{
    return m_Imp && m_Imp->RemoveAttribute(CMaaStringRO(pszName));
}
#endif
bool CMaaXmlNode::RemoveString(const CMaaString& Name) noexcept
{
    return m_Imp && m_Imp->RemoveString(Name);
}
#ifdef TOOLSLIB_XML_CCHAR_PTR
bool CMaaXmlNode::RemoveString(const char * pszName) noexcept
{
    return m_Imp && m_Imp->RemoveString(CMaaStringRO(pszName));
}
#endif
int CMaaXmlNode::RemoveAllAttributes() noexcept
{
    return m_Imp ? m_Imp->RemoveAllAttributes() : 0;
}
int CMaaXmlNode::RemoveAllStrings() noexcept
{
    return m_Imp ? m_Imp->RemoveAllStrings() : 0;
}
CMaaXmlNode::CMaaXmlNode(CMaaXmlNodeImpl * I) noexcept
{
    if ((m_Imp = I))
    {
        I->AddRef();
    }
}
/*
CMaaXmlHandle CMaaXmlNode::GetHandle() const noexcept
{
    return m_Imp;
}
*/
/*
CMaaXmlNode::operator CMaaXmlHandle ()
{
     return GetHandle();
}
*/
CMaaXmlNode::~CMaaXmlNode()
{
    if  (m_Imp)
    {
        if  (!m_Imp->Release())
        {
#if MANUAL_XML_REFS
            delete m_Imp;
#endif
        }
    }
}
void CMaaXmlNode::Empty() noexcept
{
    CMaaXmlNodeImpl* p;
    if ((p = m_Imp))
    {
        m_Imp = nullptr;
        if (!p->Release())
        {
#if MANUAL_XML_REFS
            delete p;
#endif
        }
    }
}
bool CMaaXmlNode::IsProcessingInstruction() const noexcept
{
    return m_Imp && m_Imp->IsProcessingInstruction();
}
bool CMaaXmlNode::IsElement() const noexcept
{
    return m_Imp && m_Imp->IsElement();
}
CMaaXmlNode CMaaXmlNode::FirstChild() const noexcept
{
    return CMaaXmlNodeImpl::FirstChild(m_Imp);
}
CMaaXmlNode CMaaXmlNode::LastChild() const noexcept
{
    return CMaaXmlNodeImpl::LastChild(m_Imp);
}

CMaaXmlNode CMaaXmlNode::NextSibling() const noexcept
{
    return CMaaXmlNodeImpl::NextSibling(m_Imp);
}

CMaaXmlNode CMaaXmlNode::PrevSibling() const noexcept
{
    return CMaaXmlNodeImpl::PrevSibling(m_Imp);
}

CMaaXmlNode CMaaXmlNode::FindNext() const noexcept
{
    return m_Imp ? m_Imp->FindNext() : nullptr;
}

CMaaXmlNode CMaaXmlNode::FindPrev() const noexcept
{
    return m_Imp ? m_Imp->FindPrev() : nullptr;
}

const CMaaString& CMaaXmlNode::FindAttribute(const CMaaString& Name) const noexcept
{
    return m_Imp ? m_Imp->FindAttribute(Name) : gsCMaaStringZ;
}

const CMaaString& CMaaXmlNode::FindAttribute(const CMaaString &Name, bool *pIsFound) const noexcept
{
    if  (m_Imp)
    {
        return m_Imp->FindAttribute(Name, pIsFound);
    }
    if (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}

const CMaaString& CMaaXmlNode::FindAttribute(const CMaaString& Name, bool& bIsFound) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindAttribute(Name, bIsFound);
    }
    bIsFound = false;
    return gsCMaaStringZ;
}

#ifdef TOOLSLIB_XML_CCHAR_PTR
const CMaaString& CMaaXmlNode::FindAttribute(const char* pszName) const noexcept
{
    return m_Imp ? m_Imp->FindAttribute(CMaaStringRO(pszName)) : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::FindAttribute(const char * pszName, bool *pIsFound) const noexcept
{
    if  (m_Imp)
    {
        return m_Imp->FindAttribute(CMaaStringRO(pszName), pIsFound);
    }
    if (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::FindAttribute(const char* pszName, bool& bIsFound) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindAttribute(CMaaStringRO(pszName), bIsFound);
    }
    bIsFound = false;
    return gsCMaaStringZ;
}
#endif

const CMaaString& CMaaXmlNode::FindString(const CMaaString& Name) const noexcept
{
    return m_Imp ? m_Imp->FindString(Name) : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::FindString(const CMaaString &Name, bool *pIsFound) const noexcept
{
    if  (m_Imp)
    {
        return m_Imp->FindString(Name, pIsFound);
    }
    if (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::FindString(const CMaaString& Name, bool& bIsFound) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindString(Name, bIsFound);
    }
    bIsFound = false;
    return gsCMaaStringZ;
}
#ifdef TOOLSLIB_XML_CCHAR_PTR
const CMaaString& CMaaXmlNode::FindString(const char* pszName) const noexcept
{
    return m_Imp ? m_Imp->FindString(CMaaStringRO(pszName)) : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::FindString(const char * pszName, bool *pIsFound) const noexcept
{
    if  (m_Imp)
    {
        return m_Imp->FindString(CMaaStringRO(pszName), pIsFound);
    }
    if (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::FindString(const char* pszName, bool& bIsFound) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindString(CMaaStringRO(pszName), bIsFound);
    }
    bIsFound = false;
    return gsCMaaStringZ;
}
#endif

#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
const CMaaString& CMaaXmlNode::GetString(bool* pIsFound) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->GetString(pIsFound);
    }
    if (pIsFound)
    {
        *pIsFound = false;
    }
    return gsCMaaStringZ;
}
#endif

const void * CMaaXmlNode::FirstStringIterator() const noexcept
{
    return m_Imp ? m_Imp->FirstStringIterator() : nullptr;
}
const void * CMaaXmlNode::FirstAttributeIterator() const noexcept
{
    return m_Imp ? m_Imp->FirstAttributeIterator() : nullptr;
}
bool CMaaXmlNode::GetString(const void * Iterator, CMaaString *pName, CMaaString *pValue) const noexcept
{
    return m_Imp ? m_Imp->GetString(Iterator, pName, pValue) : false;
}
bool CMaaXmlNode::GetAttribute(const void * Iterator, CMaaString *pName, CMaaString *pValue) const noexcept
{
    return m_Imp ? m_Imp->GetAttribute(Iterator, pName, pValue) : false;
}
const void * CMaaXmlNode::NextStringIterator(const void * Iterator) const noexcept
{
    return m_Imp ? m_Imp->NextStringIterator(Iterator) : nullptr;
}
const void * CMaaXmlNode::NextAttributeIterator(const void * Iterator) const noexcept
{
    return m_Imp ? m_Imp->NextAttributeIterator(Iterator) : nullptr;
}

CMaaXmlNode CMaaXmlNode::FindNode(const CMaaString& Name) const noexcept
{
    return m_Imp ? m_Imp->FindNode(Name) : nullptr;
}
CMaaXmlNode CMaaXmlNode::FindNode(const CMaaString& Name, int &N) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindNode(Name, N);
    }
    N = 0;
    return CMaaXmlNode();
}
CMaaXmlNode CMaaXmlNode::FindNode(const CMaaString& Name, int* pN) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindNode(Name, pN);
    }
    if (pN)
    {
        *pN = 0;
    }
    return CMaaXmlNode();
}

#ifdef TOOLSLIB_XML_CCHAR_PTR
CMaaXmlNode CMaaXmlNode::FindNode(const char* pszName) const noexcept // noexcept
{
    return m_Imp ? m_Imp->FindNode(CMaaStringRO(pszName)) : CMaaXmlNode();
}
CMaaXmlNode CMaaXmlNode::FindNode(const char* pszName, int &N) const noexcept // noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindNode(CMaaStringRO(pszName), N);
    }
    N = 0;
    return CMaaXmlNode();
}
CMaaXmlNode CMaaXmlNode::FindNode(const char* pszName, int* pN) const noexcept // noexcept
{
    if (m_Imp)
    {
        return m_Imp->FindNode(CMaaStringRO(pszName), pN);
    }
    if (pN)
    {
        *pN = 0;
    }
    return CMaaXmlNode();
}
#endif

bool CMaaXmlNode::AppendChild(CMaaXmlNode & Node) noexcept(noexcept_new)
{
    return AddNode(Node);
}
bool CMaaXmlNode::RemoveChild(CMaaXmlNode & Node) noexcept(noexcept_new)
{
    return RemNode(Node);
}
bool CMaaXmlNode::Remove() noexcept(noexcept_new)
{
    return GetParent().RemNode(*this);
}
const CMaaString& CMaaXmlNode::GetName() const noexcept
{
    return m_Imp ? m_Imp->m_Name : gsCMaaStringZ;
}
CMaaXmlNode CMaaXmlNode::GetParent() const noexcept
{
    return m_Imp ? CMaaXmlNode(m_Imp->m_pParent) : *this;
}
void CMaaXmlNode::ChangeName(const CMaaString &Name) noexcept(noexcept_new)
{
    if  (m_Imp)
    {
        if  (!m_Imp->m_pParent)
        {
            //if  (m_Imp->m_Name != Name)
            {
                m_Imp->m_Name = Name;
            }
        }
        else
        {
            m_Imp->m_pParent->ChangeChildName(m_Imp, Name);
        }
    }
}

int CMaaXmlNode::AddHiddenPtr(int Name, void* ptr, void (*deref)(int, void*, void*) noexcept, void* obj, bool bDerefOld) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->AddHiddenPtr(Name, ptr, deref, obj, bDerefOld) : -1;
}
int CMaaXmlNode::AddHiddenPtr(int Name, void* ptr) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->AddHiddenPtr(Name, ptr) : -1;
}
int CMaaXmlNode::AddHiddenPtr_mutable(int Name, void* ptr, void (*deref)(int, void*, void*) noexcept, void* obj, bool bDerefOld) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->AddHiddenPtr(Name, ptr, deref, obj, bDerefOld) : -1;
}
int CMaaXmlNode::AddHiddenPtr_mutable(int Name, void* ptr) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->AddHiddenPtr(Name, ptr) : -1;
}
int CMaaXmlNode::RemoveHiddenPtr(int Name, bool bDeref) noexcept
{
    return m_Imp ? m_Imp->RemoveHiddenPtr(Name, bDeref) : -1;
}
int CMaaXmlNode::RemoveHiddenPtr(int Name) noexcept
{
    return m_Imp ? m_Imp->RemoveHiddenPtr(Name) : -1;
}
int CMaaXmlNode::RemoveHiddenPtr_mutable(int Name, bool bDeref) const noexcept
{
    return m_Imp ? m_Imp->RemoveHiddenPtr(Name, bDeref) : -1;
}
int CMaaXmlNode::RemoveHiddenPtr_mutable(int Name) const noexcept
{
    return m_Imp ? m_Imp->RemoveHiddenPtr(Name) : -1;
}
void* CMaaXmlNode::GetHiddenPtr(int Name) const noexcept
{
    return m_Imp ? m_Imp->GetHiddenPtr(Name) : nullptr;
}
void* CMaaXmlNode::GetHiddenPtr(int Name, bool& bGot) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->GetHiddenPtr(Name, bGot);
    }
    bGot = false;
    return nullptr;
}
void* CMaaXmlNode::GetHiddenPtr(int Name, bool* bGot) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->GetHiddenPtr(Name, bGot);
    }
    if (bGot)
    {
        *bGot = false;
    }
    return nullptr;
}
int CMaaXmlNode::RemoveAllHiddenPtrs(bool bDeref) noexcept
{
    return m_Imp ? m_Imp->RemoveAllHiddenPtrs(bDeref) : 0;
}
int CMaaXmlNode::RemoveAllHiddenPtrs() noexcept
{
    return m_Imp ? m_Imp->RemoveAllHiddenPtrs() : 0;
}
int CMaaXmlNode::AddHiddenInt(int Name, int x) noexcept(noexcept_new)
{
    return AddHiddenPtr(Name, (void*)(size_t)x);
}
int CMaaXmlNode::AddHiddenInt_mutable(int Name, int x) const noexcept(noexcept_new)
{
    return AddHiddenPtr_mutable(Name, (void*)(size_t)x);
}
int CMaaXmlNode::GetHiddenInt(int Name, int Default) const noexcept
{
    bool bGot;
    void * ptr = GetHiddenPtr(Name, bGot);
    return bGot ? (int)(size_t)ptr : Default;
}
int CMaaXmlNode::GetHiddenInt(int Name, int Default, bool& bGot) const noexcept
{
    void* ptr = GetHiddenPtr(Name, bGot);
    return bGot ? (int)(size_t)ptr : Default;
}
int CMaaXmlNode::GetHiddenInt(int Name, int Default, bool* bGot) const noexcept
{
    bool b;
    if (!bGot)
    {
        bGot = &b;
    }
    void* ptr = GetHiddenPtr(Name, bGot);
    return bGot ? (int)(size_t)ptr : Default;
}
int CMaaXmlNode::AddHiddenStringImp(int Name, const CMaaString& Str) noexcept(noexcept_new)
{
    CMaaString::CMaaStringImp* ptr = Str.Get_pImp();
    if (ptr)
    {
        try
        {
            return AddHiddenPtr_mutable(Name, ptr, deref_string_imp);
        }
        catch (...)
        {
        }
        CMaaString::Release(ptr);
        return -1;
    }
    RemoveHiddenPtr_mutable(Name, true);
    return 0;
}
int CMaaXmlNode::AddHiddenString(int Name, const CMaaString& Str) noexcept(noexcept_new)
{
    CMaaString* ptr = nullptr;
    int R = -1;
    try
    {
        ptr = TL_NEW CMaaString(Str);
        if (ptr && !(R = AddHiddenPtr_mutable(Name, ptr, deref_string)))
        {
            return R;
        }
    }
    catch (...)
    {
    }
    delete ptr;
    return R;
}
const CMaaString::CMaaStringImp* CMaaXmlNode::GetHiddenStringImp(int Name) const noexcept
{
    return (const CMaaString::CMaaStringImp*)GetHiddenPtr(Name);
}
const CMaaString::CMaaStringImp* CMaaXmlNode::GetHiddenStringImp(int Name, bool &bGot) const noexcept
{
    return (const CMaaString::CMaaStringImp*)GetHiddenPtr(Name, bGot);
}
const CMaaString::CMaaStringImp* CMaaXmlNode::GetHiddenStringImp(int Name, bool *bGot) const noexcept
{
    return (const CMaaString::CMaaStringImp*)GetHiddenPtr(Name, bGot);
}
const CMaaString& CMaaXmlNode::GetHiddenString(int Name) const noexcept
{
    bool bGot;
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    return bGot ? *ptr : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::GetHiddenString(int Name, bool& bGot) const noexcept
{
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    return bGot ? *ptr : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::GetHiddenString(int Name, bool* bGot) const noexcept
{
    bool b;
    bGot = bGot ? bGot : &b;
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    return *bGot ? *ptr : gsCMaaStringZ;
}

#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
int CMaaXmlNode::AddHiddenPtr(const CMaaString& Name, void* ptr, void (*deref)(const CMaaString&, void*, void*) noexcept, void* obj, bool bDerefOld) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->AddHiddenPtr(Name, ptr, deref, obj, bDerefOld) : -1;
}
int CMaaXmlNode::AddHiddenPtr_mutable(const CMaaString& Name, void* ptr, void (*deref)(const CMaaString&, void*, void*) noexcept, void* obj, bool bDerefOld) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->AddHiddenPtr(Name, ptr, deref, obj, bDerefOld) : -1;
}
int CMaaXmlNode::RemoveHiddenPtr(const CMaaString& Name, bool bDeref) noexcept
{
    return m_Imp ? m_Imp->RemoveHiddenPtr(Name, bDeref) : -1;
}
int CMaaXmlNode::RemoveHiddenPtr_mutable(const CMaaString& Name, bool bDeref) const noexcept
{
    return m_Imp ? m_Imp->RemoveHiddenPtr(Name, bDeref) : -1;
}
void* CMaaXmlNode::GetHiddenPtr(const CMaaString& Name) const noexcept
{
    return m_Imp ? m_Imp->GetHiddenPtr(Name) : nullptr;
}
void* CMaaXmlNode::GetHiddenPtr(const CMaaString& Name, bool& bGot) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->GetHiddenPtr(Name, bGot);
    }
    bGot = false;
    return nullptr;
}
void* CMaaXmlNode::GetHiddenPtr(const CMaaString& Name, bool* bGot) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->GetHiddenPtr(Name, bGot);
    }
    if (bGot)
    {
        *bGot = false;
    }
    return nullptr;
}
#if 0
void* CMaaXmlNode::GetHiddenPtr(const char * const& pszName, bool* bGot) const noexcept
{
    if (m_Imp)
    {
        return m_Imp->GetHiddenPtr(CMaaStringRO(pszName), bGot);
    }
    if (bGot)
    {
        *bGot = false;
    }
    return nullptr;
}
#endif
/*
int CMaaXmlNode::RemoveAllHiddenPtrs(bool bDeref) noexcept
{
    return m_Imp ? m_Imp->RemoveAllHiddenPtrs(bDeref) : 0;
}
*/
int CMaaXmlNode::AddHiddenInt(const CMaaString& Name, int x) noexcept(noexcept_new)
{
    return AddHiddenPtr(Name, (void*)(size_t)x);
}
int CMaaXmlNode::AddHiddenInt_mutable(const CMaaString& Name, int x) const noexcept(noexcept_new)
{
    return AddHiddenPtr_mutable(Name, (void*)(size_t)x);
}
int CMaaXmlNode::GetHiddenInt(const CMaaString &Name, int Default) const noexcept
{
    bool bGot;
    const int r = (int)(size_t)GetHiddenPtr(Name, bGot);
    return bGot ? r : Default;
}
int CMaaXmlNode::GetHiddenInt(const CMaaString& Name, int Default, bool& bGot) const noexcept
{
    const int r = (int)(size_t)GetHiddenPtr(Name, bGot);
    return bGot ? r : Default;
}
int CMaaXmlNode::GetHiddenInt(const CMaaString& Name, int Default, bool* bGot) const noexcept
{
    bool b;
    if (!bGot)
    {
        bGot = &b;
    }
    const int r = (int)(size_t)GetHiddenPtr(Name, bGot);
    return bGot ? r : Default;
}
#if 0
int CMaaXmlNode::GetHiddenInt(const char * const& pszName, int Default, bool* bGot) const noexcept
{
    bool b;
    if (!bGot)
    {
        bGot = &b;
    }
    int r = (int)(size_t)GetHiddenPtr(CMaaStringRO(pszName), bGot);
    return *bGot ? r : Default;
}
#endif
int CMaaXmlNode::AddHiddenString(const CMaaString &Name, const CMaaString &Str) noexcept(noexcept_new)
{
    CMaaString* ptr = nullptr;
    int R = -1;
    try
    {
        ptr = TL_NEW CMaaString(Str);
        if  (ptr && !(R = AddHiddenPtr_mutable(Name, ptr, deref_string)))
        {
            return R;
        }
    }
    catch(...)
    {
    }
    delete ptr;
    return R;
}
const CMaaString& CMaaXmlNode::GetHiddenString(const CMaaString &Name) const noexcept
{
    bool bGot;
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    return bGot ? *ptr : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::GetHiddenString(const CMaaString& Name, bool& bGot) const noexcept
{
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    return bGot ? *ptr : gsCMaaStringZ;
}
const CMaaString& CMaaXmlNode::GetHiddenString(const CMaaString& Name, bool* bGot) const noexcept
{
    bool b;
    bGot = bGot ? bGot : &b;
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    if (*bGot)
    {
        return *ptr;
    }
    return gsCMaaStringZ;
}
#if 0
const CMaaString& CMaaXmlNode::GetHiddenString(const char * const& pszName, bool* bGot) const noexcept
{
    bool b;
    bGot = bGot ? bGot : &b;
    const CMaaString* ptr = (CMaaString*)GetHiddenPtr(CMaaStringRO(pszName), bGot);
    if (*bGot)
    {
        return *ptr;
    }
    return CMaaString::sCMaaStringZ;
}
#endif
#endif

static constexpr CMaaConstStr gTL_EncodeAttr_r[7] =
{
    "&lt;",
    "&quot;",
    "&amp;",
    "&gt;",
    "&#13;",
    "&#10;",
    "&#0;"
};
void CMaaXmlNode::EncodeAttr(CMaaConcatString &c, const CMaaString &s, int iFeatures) noexcept(noexcept_new)
{
    int i0 = 0;
    const int l = (warning_int)s.Length();
    const char * p0 = s;

    for (int i = 0; i < l; i++)
    {
        int j = -1;
        switch(p0[i])
        {
        case '<':
            j = 0;
            break;
        case '"':
            j = 1;
            break;
        case '&':
            j = 2;
            break;
        /*
        case '>': // ??? (unused)
            j = 3;
            break;
        case '>': // ??? (unused)
            if  ((int)(p - p0) - i0 >= 2) // ??? (unused)
            {
                if  (p[i-1] == ']' && p[i-2] == ']') // ??? (unused)
                {
                    j = 3;
                }
            }
            break;
        */
        case '\r':
            if  (!(iFeatures & CMaaXmlDocument::eCrLfAreInTheSimplePresentation))
            {
                j = 4;
            }
            break;
        case '\n':
            if  (!(iFeatures & CMaaXmlDocument::eCrLfAreInTheSimplePresentation))
            {
                j = 5;
            }
            break;
        case 0:
            j = 6;
            break;
        }
        if  (j >= 0)
        {
            c.Add(i0 + p0, i - i0);
            c += gTL_EncodeAttr_r[j];
            i0 = i + 1;
        }
    }
    c.Add(i0 + p0, l - i0);
}

CMaaString CMaaXmlNode::EncodeAttr(const CMaaString &s, int iFeatures) noexcept(noexcept_new)
{
    char Buffer[TOOLSLIB_CS_16K];
    CMaaConcatString c(Buffer, sizeof(Buffer), TOOLSLIB_CS_16K);
    //EncodeAttr(c, s, iFeatures);
    //return c;

    int i0 = 0;
    const int l = (warning_int)s.Length();
    const char* p0 = s;

    for (int i = 0; i < l; i++)
    {
        int j = -1;
        switch (p0[i])
        {
        case '<':
            j = 0;
            break;
        case '"':
            j = 1;
            break;
        case '&':
            j = 2;
            break;
        /*
        case '>': // ??? (unused)
            j = 3;
            break;
        case '>': // ??? (unused)
            if  ((int)(p - p0) - i0 >= 2) // ??? (unused)
            {
                if  (p[i-1] == ']' && p[i-2] == ']') // ??? (unused)
                {
                    j = 3;
                }
            }
            break;
        */
        case '\r':
            if (!(iFeatures & CMaaXmlDocument::eCrLfAreInTheSimplePresentation))
            {
                j = 4;
            }
            break;
        case '\n':
            if (!(iFeatures & CMaaXmlDocument::eCrLfAreInTheSimplePresentation))
            {
                j = 5;
            }
            break;
        case 0:
            j = 6;
            break;
        }
        if (j >= 0)
        {
            c.Add(i0 + p0, i - i0);
            c += gTL_EncodeAttr_r[j];
            i0 = i + 1;
        }
    }
    if (!i0)
    {
        return (iFeatures & CMaaXmlDocument::eEncodeAttrAsIsIfCan) ? s : s.NewCopy();
    }
    c.Add(i0 + p0, l - i0);
    return c;
}

static constexpr CMaaConstStr gTL_DecodeAttr_r[4] =
{
    "lt;",
    "quot;",
    "amp;",
    "gt;"
        //,
    //"#x",
    //"#"
};
static constexpr char gTL_DecodeAttr_ch[4] = { '<', '"', '&', '>' }; // , '\0', '\0' };
CMaaString CMaaXmlNode::DecodeAttr(const CMaaString &s, int cp, CMaaStringsContinedBuffer * pContBuf, /*bool*/ int bOptSimpleStrings) noexcept(noexcept_new) // 2020: cp = 0/1251 || e_utf8 for Unicode &#xHEX, &#DEC
{
    char Buffer[TOOLSLIB_CS_16K];
    int i0 = 0;
    const int l = (warning_int)s.Length();
    const char * p0 = s;
    CMaaConcatString Ret(Buffer, sizeof(Buffer), l);
    for (int i = 0; i < l; )
    {
        if  (p0[i] == '&')
        {
            int j;
            for (j = 0; j < 4; j++)
            {
                if (s.IsMid(gTL_DecodeAttr_r[j].p, i + 1, gTL_DecodeAttr_r[j].len))
                {
                    Ret.Add(i0 + p0, i - i0);
                    Ret += gTL_DecodeAttr_ch[j];
                    i0 = (i += (int)gTL_DecodeAttr_r[j].len + 1);
                    break;
                }
            }
            if (j < 4)
            {
                continue;
            }
            if  (s.IsMid("#x", i + 1, 2))
            {
                _WC_ n = 0;
                int k;
                for (k = 0; k < 4; k++)
                {
                    const int kk = i + 3 + k;
                    if (kk >= l)
                    {
                        break;
                    }
                    const char ch = p0[kk];
                    if  (ch >= '0' && ch <= '9')
                    {
                        n = n * 0x10 + (int)(ch - '0');
                    }
                    else if (ch >= 'a' && ch <= 'f')
                    {
                        n = n * 0x10 + (int)(ch - 'a' + 10);
                    }
                    else if (ch >= 'A' && ch <= 'F')
                    {
                        n = n * 0x10 + (int)(ch - 'A' + 10);
                    }
                    else
                    {
                        break;
                    }
                }
                const int kk = i + 3 + k;
                if  (k > 0 && kk < l && p0[kk] == ';')
                {
                    Ret.Add(i0 + p0, i - i0);
                    if  (!cp || cp == 1251)
                    {
                        const char c = n < 0x100 ? (char)n : '?';
                        Ret += c;
                    }
                    else
                    {
                        Ret += UnicodeToUtf8(&n, 1, 0);
                    }
                    i0 = i = kk + 1;
                    continue;
                }
            }
            if (i + 1 < l && p0[i + 1] == '#')
            {
                _WC_ n = 0;
                int k;
                for (k = 0; k < 5; k++)
                {
                    const int kk = i + 2 + k;
                    if (kk >= l)
                    {
                        break;
                    }
                    const char ch = p0[kk];
                    if  (ch >= '0' && ch <= '9')
                    {
                        n = n * 10 + (int)(ch - '0');
                    }
                    else
                    {
                        break;
                    }
                }
                const int kk = i + 2 + k;
                if  (k > 0 && kk < l && p0[kk] == ';')
                {
                    Ret.Add(i0 + p0, i - i0);
                    if  (!cp || cp == 1251)
                    {
                        const char c = n < 0x100 ? (char)n : '?';
                        Ret += c;
                    }
                    else
                    {
                        Ret += UnicodeToUtf8(&n, 1, 0);
                    }
                    i0 = i = kk + 1;
                    continue;
                }
            }
        }
        i++;
    }
    if (!i0)
    {
        return bOptSimpleStrings ? s : pContBuf ? pContBuf->Put(s, true) : s.NewCopy();
    }
    Ret.Add(i0 + p0, l - i0);
    return pContBuf ? pContBuf->Put(Ret.GetMemoryString(), true) : Ret;
}

CMaaString CMaaXmlNode::ToString(const CMaaString &Sp0, const CMaaString &DeltaSp, const CMaaString &EndOfLine, int iFeatures) const noexcept(noexcept_new)
{
    if  (!m_Imp)
    {
        return CMaaStringZ;
    }
    CMaaString Sp1 = Sp0 + DeltaSp;
    CMaaString s, e;
    s = Sp0;
    if  (IsProcessingInstruction())
    {
        s = "<?xml";
        if  (m_Imp->m_Name.IsNotEmpty())
        {
            s = CMaaString("<?") + m_Imp->m_Name;
        }
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            s.Append(" %S=\"%S\"", &p->m_Name, &p->m_Value);
        }
        s += "?>";
    }
    else
    {
        s.Format("%S<%S", &Sp0, &m_Imp->m_Name);
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            CMaaString v = EncodeAttr(p->m_Value, iFeatures);
            s.Append(" %S=\"%S\"", &p->m_Name, &v);
        }
        CMaaXmlNode n = FirstChild();
        if  (n.IsNull() && m_Imp->m_StringsList.IsEmpty() 
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
            && m_Imp->m_String.IsEmpty()
#endif
            )
        {
            if  (m_Imp->m_AttrsList.IsNotEmpty())
            {
                s += " />";
            }
            else
            {
                s += "/>";
            }
        }
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
        else if (m_Imp->m_String.IsNotEmpty())
        {
            if  (m_Imp->m_AttrsList.IsNotEmpty())
            {
                s += CMaaString(" >");
            }
            else
            {
                s += CMaaString(">");
            }
            CMaaString v = EncodeString(m_Imp->m_String, iFeatures, m_Imp->m_Name);
            s += v;
            s.Append("</%S>", &m_Imp->m_Name);
        }
#endif
        else
        {
            if  (m_Imp->m_AttrsList.IsNotEmpty())
            {
                s += CMaaString(" >") + EndOfLine;
            }
            else
            {
                s += CMaaString(">") + EndOfLine;
            }
            for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_StringsList.LookAtFront(); p; p = m_Imp->m_StringsList.Next(p))
            {
                if  (p->m_Value.IsEmpty())
                {
                    /*
                    if (p->m_Name == "")
                    {
                        static int aa = 0;
                        aa++;
                        aa++;
                        aa++;
                    }
                    */
                    s += Sp1 + "<" + p->m_Name + "/>" + EndOfLine;
                }
                else
                {
                    CMaaString v = EncodeString(p->m_Value, iFeatures, p->m_Name);
                    s += Sp1 + "<" + p->m_Name + ">" + v + "</" + p->m_Name + ">" + EndOfLine;
                }
            }
            while(n.IsNotNull())
            {
                e = n.ToString(Sp1, DeltaSp, EndOfLine, iFeatures);
                s += e + EndOfLine;
                n = n.NextSibling();
            }
            s.Append("%S</%S>", &Sp0, &m_Imp->m_Name);
        }
    }
    return s;
}

void CMaaXmlNode::ToString(CMaaConcatString &s, const CMaaString &Sp0, const CMaaString &DeltaSp, const CMaaString &EndOfLine, int iFeatures) const
{
    if  (!m_Imp)
    {
        return;
    }
    CMaaString Sp1 = Sp0 + DeltaSp;
    s += Sp0;
    if  (IsProcessingInstruction())
    {
        if  (m_Imp->m_Name.IsNotEmpty())
        {
            s += "<?";
            s += m_Imp->m_Name;
        }
        else
        {
            s += "<?xml";
        }
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            s.Append(" %S=\"%S\"", &p->m_Name, &p->m_Value);
        }
        s += "?>";
    }
    else
    {
        s += "<";
        s += m_Imp->m_Name;
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            s += " ";
            s += p->m_Name;
            s += "=\"";
            EncodeAttr(s, p->m_Value, iFeatures);
            s += "\"";
        }
        CMaaXmlNode n = FirstChild();
        if  (n.IsNull() && m_Imp->m_StringsList.IsEmpty()
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
            && m_Imp->m_String.IsEmpty()
#endif
            )
        {
            if  (m_Imp->m_AttrsList.IsNotEmpty())
            {
                s += " />";
            }
            else
            {
                s += "/>";
            }
        }
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
        else if (m_Imp->m_String.IsNotEmpty())
        {
            if  (m_Imp->m_AttrsList.IsNotEmpty())
            {
                s += " >";
            }
            else
            {
                s += ">";
            }
            EncodeString(s, m_Imp->m_String, iFeatures, m_Imp->m_Name);
            s += "</";
            s += m_Imp->m_Name;
            s += ">";
        }
#endif
        else
        {
            if  (m_Imp->m_AttrsList.IsNotEmpty())
            {
                s += " >";
                s += EndOfLine;
            }
            else
            {
                s += ">";
                s += EndOfLine;
            }
            for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_StringsList.LookAtFront(); p; p = m_Imp->m_StringsList.Next(p))
            {
                if  (p->m_Value.IsEmpty())
                {
                    s += Sp1;
                    s += "<";
                    s += p->m_Name;
                    s += "/>";
                    s += EndOfLine;
                }
                else
                {
                    s += Sp1;
                    s += "<";
                    s += p->m_Name;
                    s += ">";
                    EncodeString(s, p->m_Value, iFeatures, p->m_Name);
                    s += "</";
                    s += p->m_Name;
                    s += ">";
                    s += EndOfLine;
                }
            }
            while(n.IsNotNull())
            {
                n.ToString(s, Sp1, DeltaSp, EndOfLine, iFeatures);
                s += EndOfLine;
                n = n.NextSibling();
            }
            s += Sp0;
            s += "</";
            s += m_Imp->m_Name;
            s += ">";
        }
    }
}

CMaaString StrToJson(const CMaaString &x, int MaxLen, int Flags) noexcept(noexcept_new) // = 0x7fffffff, 0)
{
    //try
    //{
        char Buffer[TOOLSLIB_CS_4K];
        CMaaConcatString s(Buffer, sizeof(Buffer), TOOLSLIB_CS_4K);
        const char * p0 = x;
        const char * p = p0, * e = p0 + x.Length();
        for (; p < e && MaxLen > 0; MaxLen--, p++)
        {
            char c = *p;
            switch(c)
            {
            case '\\':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\\\", 2);
                }
                break;
            case '/':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\/", 2);
                }
                break;
            case '\b':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\b", 2);
                }
                break;
            case '\f':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\f", 2);
                }
                break;
            case '\n':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\n", 2);
                }
                break;
            case '\r':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\r", 2);
                }
                break;
            case '\t':
                //if ((MaxLen -= 2) >= 0)
                {
                    s.Add("\\t", 2);
                }
                break;
            case '"':
                if (!(Flags & CMaa_JSON_HEX_QUOT))
                {
                    //if ((MaxLen -= 2) >= 0)
                    {
                        s.Add("\\\"", 2);
                    }
                    break;
                }
                CMaa_fallthrough;
            default:
                if (
                    (c >= 0 && c <= 31) ||
                    c == '"' ||
                    (c == '\'' && (Flags & CMaa_JSON_HEX_APOS)) ||
                    ((c == '<' || c == '>') && (Flags & CMaa_JSON_HEX_TAG)) ||
                    (c == '&' && (Flags & CMaa_JSON_HEX_AMP))
                   )
                {
                    char tmp[8];
                    //sprintf(tmp, "\\u%04x", (int)c);
                    tmp[0] = '\\';
                    tmp[1] = 'u';
                    tmp[2] = '0';
                    tmp[3] = '0';
                    Byte2Hex_nzt_L(&tmp[4], c);
                    //if ((MaxLen -= 6) >= 0)
                    {
                        s.Add(tmp, 6);
                    }
                }
                else
                {
                    int l = (warning_int)x.Utf8CharsLenFromPos0(1, (StrInt)(p - p0), true, false);
                    if (l == -4) // utf32 char
                    {
                        p += 3; // skip
                        MaxLen++;
                    }
                    else if (l < 0) // truncated utf char by end of string
                    {
                        p += 2;
                        MaxLen++;
                    }
                    else
                    {
                        //if ((MaxLen -= l) >= 0)
                        {
                            s.Add(p, l);
                        }
                        p += l - 1;
                    }
                }
            }
        }
        //return CMaaString(s);
        return s.str2();
    //}
    //catch(...)
    //{
    //}
    //return CMaaStringZ;
}

int JsonCharLen(const char * p, int i, int MaxLen, bool bWeak) noexcept
{
    if (i >= MaxLen)
    {
        return 0;
    }
    if (p[i] == '\\')
    {
        if (i + 1 >= MaxLen)
        {
            return bWeak ? 1 : -1;
        }
        char c = p[i + 1];
        switch(c)
        {
        case '"':
        case '\'':
        case '\\':
        case '/':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
            return 2;
        case 'U':
            if (!bWeak)
            {
                //return -1;
            }
            CMaa_fallthrough;
        case 'u':
            if (i + 2 + 4 <= MaxLen)
            {
                int f = 0;
                for (int j = 0; j < 4; j++)
                {
                    c = p[i + 2 + j];
                    if (c >= '0' && c <= '9')
                    {
                        continue;
                    }
                    if (c >= 'a' && c <= 'f' && !(f & 2))
                    {
                        f |= 1;
                        continue;
                    }
                    if (c >= 'A' && c <= 'F' && !(f & 1))
                    {
                        f |= 2;
                        continue;
                    }
                    return bWeak ? 1 : -1;
                }
            }
            return 6;
        }
    }
    return 1;
}

CMaaString JsonToStr(const CMaaString& x) noexcept(noexcept_new)
{
    //try
    //{
        char Buffer[TOOLSLIB_CS_4K];
        CMaaConcatString s(Buffer, sizeof(Buffer), x.Length() + 1);
        const char * p = x;
        const char * e = p + x.Length();
        for (; p < e; p++)
        {
            char c = *p;
            if (c == '\\' && p + 1 < e)
            {
                switch(*(++p))
                {
                case '"':
                    s += '"';
                    break;
                case '\'':
                    s += '\'';
                    break;
                case '\\':
                    s += '\\';
                    break;
                case '/':
                    s += '/';
                    break;
                case 'b':
                    s += '\b';
                    break;
                case 'f':
                    s += '\f';
                    break;
                case 'n':
                    s += '\n';
                    break;
                case 'r':
                    s += '\r';
                    break;
                case 't':
                    s += '\t';
                    break;
                case 'U':
                case 'u':
                    if (p + 4 < e && JsonCharLen(p - 1, 0, 6) == 6)
                    {
                        _WC_ w = 0;
                        for (int j = 0; j < 4; j++)
                        {
                            c = *(++p);
                            if (c >= '0' && c <= '9')
                            {
                                c -= '0';
                            }
                            else if (c >= 'a' && c <= 'f')
                            {
                                c -= 'a' + 10;
                            }
                            else
                            {
                                c -= 'A' + 10;
                            }
                            w = (w << 4) + (_WC_)c;
                        }
                        s += UnicodeToUtf8(&w, 1, 0);
                        break;
                    }
                    CMaa_fallthrough;
                default:
                    s += '\\';
                    p--;
                }
            }
            else
            {
                s += c;
            }
        }
        //return CMaaString(s);
        return s.str2();
    //}
    //catch(...)
    //{
    //}
    //return CMaaStringZ;
}

//#define JsonToStr(x) x.ReplaceNN("&quot;", "\""); x.ReplaceNN("&#92;", "\\");
//#define StrToJson(x) x.ReplaceNN('\\', "&#92;"); x.ReplaceNN('"', "&quot;");

void CMaaXmlNode::ToJson(CMaaConcatString& s, const CMaaString& Sp0, const CMaaString& DeltaSp, const CMaaString& EndOfLine, int Flags, int MaxNameLen, int MaxAttrLen) const
{
    const bool bArray = GetName() == CMaaStringJsonArray;
    MaxNameLen = MaxNameLen >= 0 ? MaxNameLen : 0x7fffffff;
    MaxAttrLen = MaxAttrLen >= 0 ? MaxAttrLen : 0x7fffffff;
    CMaaXmlNode nn;
    for (CMaaXmlNode n = FirstChild(); n; n = nn)
    {
        nn = n.NextSibling();
        const CMaaString& NodeName = n.GetName();
        const CMaaString& JsonName = n.FindAttribute(CMaaStringJsonName);
        if (NodeName == CMaaStringJsonVal)
        {
            CMaaString Name = StrToJson(JsonName, MaxNameLen, Flags);
            const CMaaString &v1 = n.FindAttribute(CMaaStringJsonValue);
            const CMaaString &t = n.FindAttribute(CMaaStringJsonType);
            if (t.IsEmpty() || t == CMaaStringJsonString)
            {
                CMaaString v = StrToJson(v1, MaxAttrLen, Flags);
                s += Sp0;
                if (Name.IsNotEmpty() || !bArray)
                {
                    s += '"';
                    s += Name;
                    s.Add("\": \"", 4);
                }
                else
                {
                    s += '"';
                }
                s += v;
                s += '"';
                if (nn)
                {
                    s += ',';
                }
                s += EndOfLine;
            }
            else
            {
                //v = MaxAttrLen <= 0 ? v1 : v1.RefLeft(MaxAttrLen);
                CMaaString v = StrToJson(v1, MaxAttrLen, Flags);
                s += Sp0;
                if (Name.IsNotEmpty() || !bArray)
                {
                    s += '"';
                    s += Name;
                    s.Add("\": ", 3);
                }
                s += v;
                if (nn)
                {
                    s += ',';
                }
                s += EndOfLine;
            }
        }
        else if (NodeName == CMaaStringJsonNode)
        {
            CMaaString Name = StrToJson(JsonName, MaxNameLen, Flags);
            s += Sp0;
            if (Name.IsNotEmpty())
            {
                s += '"';
                s += Name;
                s.Add("\": ", 3);
            }
            s += '{';
            s += EndOfLine;

            n.ToJson(s, Sp0 + DeltaSp, DeltaSp, EndOfLine, Flags, MaxNameLen, MaxAttrLen);

            s += Sp0;
            s += '}';
            if (nn)
            {
                s += ',';
            }
            s += EndOfLine;
        }
        else if (NodeName == CMaaStringJsonArray)
        {
            CMaaString Name = StrToJson(JsonName, MaxNameLen, Flags);
            s += Sp0;
            if (Name.IsNotEmpty())
            {
                s += '"';
                s += Name;
                s.Add("\": ", 3);
            }
            s += '[';
            s += EndOfLine;

            n.ToJson(s, Sp0 + DeltaSp, DeltaSp, EndOfLine, Flags, MaxNameLen, MaxAttrLen);

            s += Sp0;
            s += ']';
            if (nn)
            {
                s += ',';
            }
            s += EndOfLine;
        }
    }
}

CMaaXmlNode CMaaXmlNode::FindNodeWithAttr(const CMaaString &Name, const CMaaString &AttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode) const noexcept
{
    return m_Imp ? m_Imp->FindNodeWithAttr(Name, AttrName, AttrVal, SkipNode.GetHandle()) : nullptr;
}
#ifdef TOOLSLIB_XML_CCHAR_PTR
CMaaXmlNode CMaaXmlNode::FindNodeWithAttr(const CMaaString& Name, const char * pszAttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode) const noexcept
{
    return m_Imp ? FindNodeWithAttr(Name, CMaaStringRO(pszAttrName), AttrVal, SkipNode.GetHandle()) : nullptr;
}
CMaaXmlNode CMaaXmlNode::FindNodeWithAttr(const char* pszName, const CMaaString& AttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode) const noexcept
{
    return m_Imp ? FindNodeWithAttr(CMaaStringRO(pszName), AttrName, AttrVal, SkipNode.GetHandle()) : nullptr;
}
CMaaXmlNode CMaaXmlNode::FindNodeWithAttr(const char* pszName, const char* pszAttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode) const noexcept
{
    return m_Imp ? FindNodeWithAttr(CMaaStringRO(pszName), CMaaStringRO(pszAttrName), AttrVal, SkipNode.GetHandle()) : nullptr;
}
#endif

CMaaXmlNode CMaaXmlNode::FindNodeWithAttrRO(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal, const CMaaXmlNode& SkipNode, int Flags) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->FindNodeWithAttrRO(Name, AttrName, AttrVal, SkipNode.GetHandle(), Flags) : nullptr;
}
CMaaXmlNode CMaaXmlNode::FindNodeWithAttrRO(const CMaaString& Name, const CMaaString& AttrName, const CMaaString& AttrVal) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->FindNodeWithAttrRO(Name, AttrName, AttrVal/*, nullptr, 0*/) : nullptr;
}
#ifdef TOOLSLIB_XML_CCHAR_PTR
CMaaXmlNode CMaaXmlNode::FindNodeWithAttrRO(const CMaaString& Name, const char* pszAttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode, int Flags) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->FindNodeWithAttrRO(Name, CMaaStringRO(pszAttrName), AttrVal, SkipNode.GetHandle(), Flags) : nullptr;
}
CMaaXmlNode CMaaXmlNode::FindNodeWithAttrRO(const char* pszName, const CMaaString& AttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode, int Flags) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->FindNodeWithAttrRO(CMaaStringRO(pszName), AttrName, AttrVal, SkipNode.GetHandle(), Flags) : nullptr;
}
CMaaXmlNode CMaaXmlNode::FindNodeWithAttrRO(const char* pszName, const char* pszAttrName, const CMaaString &AttrVal, const CMaaXmlNode &SkipNode, int Flags) const noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->FindNodeWithAttrRO(CMaaStringRO(pszName), CMaaStringRO(pszAttrName), AttrVal, SkipNode.GetHandle(), Flags) : nullptr;
}
#endif

CMaaXmlNode CMaaXmlNode::FindNodeWithString(const CMaaString& Name, const CMaaString& StringName, const CMaaString& StringVal, const CMaaXmlNode& SkipNode) const noexcept
{
    return m_Imp ? m_Imp->FindNodeWithString(Name, StringName, StringVal, SkipNode.GetHandle()) : nullptr;
}

int CMaaXmlNode::ChangeNodesAttr(const CMaaString &Name, const CMaaString &AttrName, CMaaString OldVal, const CMaaString &NewVal) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->ChangeNodesAttr(Name, AttrName, std::move(OldVal), NewVal) : 0;
}

int CMaaXmlNode::ChangeNodesString(const CMaaString &Name, const CMaaString &StringName, CMaaString OldVal, const CMaaString &NewVal) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->ChangeNodesString(Name, StringName, std::move(OldVal), NewVal) : 0;
}

int CMaaXmlNode::RemoveNodesAttr(const CMaaString &Name, CMaaString AttrName, CMaaString Val) noexcept
{
    return m_Imp ? m_Imp->RemoveNodesAttr(Name, std::move(AttrName), std::move(Val)) : 0;
}

int CMaaXmlNode::RemoveNodesString(const CMaaString &Name, CMaaString StringName, CMaaString Val) noexcept
{
    return m_Imp ? m_Imp->RemoveNodesString(Name, std::move(StringName), std::move(Val)) : 0;
}

int CMaaXmlNode::RemoveNodesWithAttr(const CMaaString &Name, CMaaString AttrName, CMaaString Val) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->RemoveNodesWithAttr(Name, std::move(AttrName), std::move(Val)) : 0;
}

int CMaaXmlNode::RemoveNodesWithString(const CMaaString &Name, CMaaString StringName, CMaaString Val) noexcept(noexcept_new)
{
    return m_Imp ? m_Imp->RemoveNodesWithString(Name, std::move(StringName), std::move(Val)) : 0;
}

bool CMaaXmlNode::InsertBefore(CMaaXmlNode p) noexcept(noexcept_new)
{
    if  (p.IsNull() || IsNull() /*|| p.GetParent().IsNull()!!2018!!*/ || GetHandle() == p.GetHandle() /*!!2018!!*/ || p.GetName() != GetName())
    {
        return false;
    }
    Remove();
    return AddBefore(p);
}

bool CMaaXmlNode::InsertAfter(CMaaXmlNode p) noexcept(noexcept_new)
{
    if  (p.IsNull() || IsNull() || GetHandle() == p.GetHandle() || p.GetName() != GetName())
    {
        return false;
    }
    Remove();
    return AddAfter(p);
}

bool CMaaXmlNode::MoveUp() noexcept(noexcept_new)
{
    CMaaXmlNode p = FindPrev();
    if (p.IsNotNull() && IsNotNull() && p.GetName() == GetName())
    {
        Remove();
        return AddBefore(p);
    }
    return false;
}

bool CMaaXmlNode::MoveDown() noexcept(noexcept_new)
{
    CMaaXmlNode p = FindNext();
    if  (p.IsNotNull() && IsNotNull() && p.GetName() == GetName())
    {
        Remove();
        return AddAfter(p);
    }
    return false;
}

bool CMaaXmlNode::MoveUpSibling() noexcept(noexcept_new)
{
    CMaaXmlNode p = PrevSibling();
    CMaaString Name = GetName();
    ChangeName(p.GetName());
    const bool b = MoveUp();
    ChangeName(Name);
    return b;
}

bool CMaaXmlNode::MoveDownSibling() noexcept(noexcept_new)
{
    CMaaXmlNode n = NextSibling();
    CMaaString Name = GetName();
    ChangeName(n.GetName());
    const bool b = MoveDown();
    ChangeName(Name);
    return b;
}

bool CMaaXmlNode::AddBefore(CMaaXmlNode n) noexcept(noexcept_new)
{
    return CMaaXmlNodeImpl::AddBefore(n.m_Imp, m_Imp);
}

bool CMaaXmlNode::AddAfter(CMaaXmlNode n) noexcept(noexcept_new)
{
    return CMaaXmlNodeImpl::AddAfter(n.m_Imp, m_Imp);
}

void CMaaXmlNodeImpl::ConvertToUtf8(_dword cp) noexcept(noexcept_new)
{
    int aCount = 0;
    for (Attr *a = m_AttrsList.LookAtFront(); a; a = m_AttrsList.Next(a))
    {
        aCount++;
        a->m_Value = UnicodeToUtf8(AnsiToUnicode(a->m_Value, false, cp));
    }
    int sCount = 0;
    for (Attr *a = m_StringsList.LookAtFront(); a; a = m_StringsList.Next(a))
    {
        sCount++;
        a->m_Value = UnicodeToUtf8(AnsiToUnicode(a->m_Value, false, cp));
    }
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
    if  (m_String.IsNotEmpty())
    {
        m_String = UnicodeToUtf8(AnsiToUnicode(m_String, false, cp));
    }
#endif
}

CMaaXmlNode CMaaXmlNode::CopyNode(CMaaXmlDocument& doc, bool bChilds, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)) const noexcept(noexcept_new)
{
    if (!m_Imp)
    {
        return CMaaXmlNode();
    }

    CMaaXmlNode n = CMaaXmlNode(m_Imp->m_Type, GetName());
    CMaaXmlNodeImpl::CopyAttributes(n.m_Imp, m_Imp, bHiddenPtrs, copyref);
    CMaaXmlNodeImpl::CopyStrings(n.m_Imp, m_Imp);
    if (bChilds)
    {
        CMaaXmlNode c = FirstChild();
        for (; c.IsNotNull(); c = c.NextSibling())
        {
            CMaaXmlNode cc = c.CopyNode(doc, bChilds, bHiddenPtrs, copyref);
            n.AppendChild(cc);
        }
    }
    return n;
}

int CMaaXmlNode::CopyAttributes(const CMaaXmlNode From, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)) noexcept(noexcept_new)
{
    return m_Imp != From.m_Imp ? CMaaXmlNodeImpl::CopyAttributes(m_Imp, From.m_Imp, bHiddenPtrs, copyref) : 0;
    //CMaaXmlNodeImpl::CopyStrings(n, *this);
}

#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
CMaaXmlNode CMaaXmlNode::CopyNode(CMaaXmlDocument &doc, bool bChilds, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept), void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)) const noexcept(noexcept_new)
{
    if  (!m_Imp)
    {
        return CMaaXmlNode();
    }

    CMaaXmlNode n = CMaaXmlNode(m_Imp->m_Type, GetName());
    CMaaXmlNodeImpl::CopyAttributes(n, *this, bHiddenPtrs, copyref, copyref_);
    CMaaXmlNodeImpl::CopyStrings(n, *this);
    if  (bChilds)
    {
        CMaaXmlNode c = FirstChild();
        for (; c.IsNotNull(); c = c.NextSibling())
        {
            CMaaXmlNode cc = c.CopyNode(doc, bChilds, bHiddenPtrs, copyref, copyref_);
            n.AppendChild(cc);
        }
    }
    return n;
}

int CMaaXmlNode::CopyAttributes(const CMaaXmlNode From, bool bHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept), void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)) noexcept(noexcept_new)
{
    return m_Imp != From.m_Imp ? CMaaXmlNodeImpl::CopyAttributes(*this, From, bHiddenPtrs, copyref, copyref_) : 0;
    //CMaaXmlNodeImpl::CopyStrings(n, *this);
}
#endif

int CMaaXmlNode::EnumAttributes(EnumAttributesFn Fn, void * Param)
{
    if  (m_Imp)
    {
        strEnum e(Fn, *this, Param);
        return CMaaXmlNodeImpl::EnumAttributes(m_Imp, stEnumAttrFn, &e);
    }
    return 0;
}

int CMaaXmlNode::EnumStrings(EnumStringsFn Fn, void * Param)
{
    if  (m_Imp)
    {
        strEnum e(Fn, *this, Param);
        return CMaaXmlNodeImpl::EnumStrings(m_Imp, stEnumAttrFn, &e);
    }
    return 0;
}

int CMaaXmlNode::SortAttributes() noexcept
{
    return m_Imp ? CMaaXmlNodeImpl::SortAttributes(m_Imp) : 0;
}

int CMaaXmlNode::SortStrings() noexcept
{
    return m_Imp ? CMaaXmlNodeImpl::SortStrings(m_Imp) : 0;
}

void CMaaXmlNode::Sort(int Flags) noexcept
{
    if  (!m_Imp)
    {
        return;
    }
    if  (Flags & eAttributes)
    {
        SortAttributes();
    }
    if  (Flags & eStrings)
    {
        SortStrings();
    }
    if  (Flags & eRecursive)
    {
        CMaaXmlNode c = FirstChild();
        for (; c.IsNotNull(); c = c.NextSibling())
        {
            c.Sort(Flags);
        }
    }
}


//-------------------------------------------------------------------------------------------------
// CMaaXmlProcessingInstruction
//-------------------------------------------------------------------------------------------------
CMaaXmlProcessingInstruction::CMaaXmlProcessingInstruction(const CMaaString &Name) noexcept(noexcept_new)
:   CMaaXmlNode(CMaaXmlNodeImpl::eProcInst, Name)
{
}

CMaaXmlProcessingInstruction::CMaaXmlProcessingInstruction(const CMaaXmlNode &Node) noexcept
:   CMaaXmlNode(Node)
{

}

bool CMaaXmlProcessingInstruction::AddInstruction(int Type, CMaaString Value) noexcept(noexcept_new)
{
    CMaaString Name;
    if  (Type == eVersion)
    {
        Name = "version";
        if  (Value.IsEmpty())
        {
            Value = "1.0";
        }
    }
    else if (Type == eEncoding)
    {
        Name = "encoding";
        if  (Value.IsEmpty())
        {
            Value = "utf-8";
        }
    }
    else
    {
        return false;
    }
    return AddInstruction(Name, Value);
}

bool CMaaXmlProcessingInstruction::AddInstruction(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
    return CMaaXmlNode::AddAttribute(Name, Value);
}

//-------------------------------------------------------------------------------------------------
// CMaaXmlElement
//-------------------------------------------------------------------------------------------------
CMaaXmlElement::CMaaXmlElement(const CMaaString &Name) noexcept(noexcept_new)
:   CMaaXmlNode(CMaaXmlNodeImpl::eElm, Name)
{
}

CMaaXmlElement::CMaaXmlElement(const CMaaXmlNode &Node) noexcept
:   CMaaXmlNode(Node)
{
}

bool CMaaXmlElement::SetAttribute(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
    return CMaaXmlNode::AddAttribute(Name, Value);
}

bool CMaaXmlElement::AppendChild(CMaaXmlNode &Node) noexcept(noexcept_new)
{
    return CMaaXmlNode::AddNode(Node);
}

//-------------------------------------------------------------------------------------------------
// CMaaXmlElement
//-------------------------------------------------------------------------------------------------
CMaaXmlDocument::CMaaXmlDocument(const CMaaString &Name) noexcept(noexcept_new)
:   m_Name(/*gCMaaToolLib_crt_Initializer() ? Name :*/ Name),
    m_Root(CMaaXmlNodeImpl::eRoot, gsCMaaStringZ)
{
}

CMaaXmlDocument::CMaaXmlDocument(const CMaaString &Name, const CMaaString &RootName) noexcept(noexcept_new)
:   m_Name(/*gCMaaToolLib_crt_Initializer() ? Name :*/ Name),
    m_Root(CMaaXmlNodeImpl::eRoot, gsCMaaStringZ)
{
    CMaaXmlElement Root = CreateElement(RootName);
    AppendChild(Root);
}

//-------------------------------------------------------------------------------------------------
CMaaXmlDocument::CMaaXmlDocument(CMaaXmlDocument& That, bool bCopy, bool bCopyHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept)) noexcept(noexcept_new)
:   m_Name(gsCMaaStringZ),
    m_Root(CMaaXmlNodeImpl::eRoot, gsCMaaStringZ)
{
    if (!bCopy)
    {
        *this = That;
    }
    else
    {
        //CMaaWin32Locker Locker(GLOCK_LIB2, CMaaStringZ);
        CMaaWin32Locker_a2Locker_gLock_Atomic2;
        a2Locker_Lock;
        CMaaXmlNode Root = That.m_Root;
        m_Name = That.m_Name;
        a2Locker_UnLock;

        // move op by default here
        m_Root = Root.CopyNode(*this, true, bCopyHiddenPtrs, copyref);
    }
}
#ifdef TOOLSLIB_ADD_HIDDEN_BY_STRING
CMaaXmlDocument::CMaaXmlDocument(CMaaXmlDocument &That, bool bCopy, bool bCopyHiddenPtrs, void* (*copyref)(int, void*, void**, void (*deref)(int, void*, void*) noexcept), void* (*copyref_)(const CMaaString&, void*, void**, void (*deref_)(const CMaaString&, void*, void*) noexcept)) noexcept(noexcept_new)
:   m_Name(gsCMaaStringZ),
    m_Root(CMaaXmlNodeImpl::eRoot, gsCMaaStringZ)
{
    if  (!bCopy)
    {
        *this = That;
    }
    else
    {
        //CMaaWin32Locker Locker(GLOCK_LIB2, CMaaStringZ);
        CMaaWin32Locker_a2Locker_gLock_Atomic2;
        a2Locker_Lock;
        CMaaXmlNode Root = That.m_Root;
        m_Name = That.m_Name;
        a2Locker_UnLock;

        // move op by default here
        m_Root = Root.CopyNode(*this, true, bCopyHiddenPtrs, copyref, copyref_);
    }
}
#endif

CMaaXmlDocument::~CMaaXmlDocument()
{
}

CMaaXmlElement CMaaXmlDocument::CreateElement(const CMaaString &Name) noexcept(noexcept_new)
{
    return CMaaXmlElement(Name);
}
#if 0
CMaaXmlElement CMaaXmlDocument::CreateElement(const char * const& Name)
{
    return CMaaXmlElement(CMaaStringRO(Name));
}
#endif
bool CMaaXmlDocument::AddProcessingInstruction(CMaaXmlNode &Node) noexcept(noexcept_new)
{
    CMaaXmlNode pi = FirstChild();
    for (; pi.IsNotNull(); )
    {
        CMaaXmlNode tmp = pi;
        pi = tmp.NextSibling();

        if  (tmp.IsProcessingInstruction())
        {
            tmp.Remove();
        }
    }
    return m_Root.AddProcessingInstruction(Node);
}

void CMaaXmlDocument::AppendChild(CMaaXmlNode &Node) noexcept(noexcept_new)
{
    m_Root.AppendChild(Node);
}

CMaaXmlNode CMaaXmlDocument::FirstChild() const noexcept
{
    CMaaWin32Locker_a2Locker_gLock_Atomic2;
    a2Locker_Lock;
    CMaaXmlNode Root = m_Root; // mthr safe
    a2Locker_UnLock;

    return Root.FirstChild();
}

CMaaXmlElement CMaaXmlDocument::DocumentElement() const noexcept
{
    CMaaWin32Locker_a2Locker_gLock_Atomic2;
    a2Locker_Lock;
    CMaaXmlNode Root = m_Root; // mthr safe
    a2Locker_UnLock;

    CMaaXmlNode n = Root.FirstChild();
    while(n.IsNotNull())
    {
        if  (n.IsElement())
        {
            break;
        }
        n = n.NextSibling();
    }
    return n;
}

CMaaXmlProcessingInstruction CMaaXmlDocument::CreateProcessingInstruction(CMaaString xml) noexcept(noexcept_new)
{
    return CMaaXmlProcessingInstruction(xml);
}

CMaaString CMaaXmlDocument::ToString(const CMaaString &Sp0, const CMaaString &DeltaSp, const CMaaString &EndOfLine, int iFeatures) const noexcept(noexcept_new)
{
    //CMaaString Sp0;
    //CMaaString DeltaSp = " ";

    CMaaString s, e;
#ifdef TL_TRY_FAST_XML_NODE
    CMaaXmlNode f = FirstChild();
    CMaaFastXmlNode n = f;
#else
    CMaaXmlNode n = FirstChild();
#endif
    if  (n.IsNull() || !n.IsProcessingInstruction())
    {
        s += CMaaString("<?xml?>") + EndOfLine;
    }
    else
    {
        e = n.ToString(Sp0, DeltaSp, EndOfLine, iFeatures);
        s += e + EndOfLine;
        n = n.NextSibling();
    }
    if  (!(iFeatures & eDoNotAddDoctypeKey))
    {
        e.Format("<!DOCTYPE %S>%S", &m_Name, &EndOfLine);
    }
    s += e;
    for (; n.IsNotNull(); n = n.NextSibling())
    {
        e = n.ToString(Sp0, DeltaSp, EndOfLine, iFeatures);
        s += e + EndOfLine;
    }
    return s;
}

CMaaString CMaaXmlDocument::ToString(int ApproxLength, const CMaaString &Sp0, const CMaaString &DeltaSp, const CMaaString &EndOfLine, int iFeatures) const noexcept(noexcept_new)
{
    try
    {
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString s(Buffer, sizeof(Buffer), ApproxLength < 0 ? TOOLSLIB_CS_64K : ApproxLength);
        //CMaaConcatString s(ApproxLength < 0 ? 256 * 1024 : ApproxLength, 1);
        //CMaaString Sp0;
        //CMaaString DeltaSp = " ";

        CMaaString e;
#ifdef TL_TRY_FAST_XML_NODE
        CMaaXmlNode f = FirstChild();
        CMaaFastXmlNode n = f;
#else
        CMaaXmlNode n = FirstChild();
#endif
        if  (n.IsNull() || !n.IsProcessingInstruction())
        {
            s += CMaaString("<?xml?>") + EndOfLine;
        }
        else
        {
            n.ToString(s, Sp0, DeltaSp, EndOfLine, iFeatures);
            s += EndOfLine;
            n = n.NextSibling();
        }
        if  (!(iFeatures & eDoNotAddDoctypeKey))
        {
            e.Format("<!DOCTYPE %S>%S", &m_Name, &EndOfLine);
        }
        s += e;
        for (; n.IsNotNull(); n = n.NextSibling())
        {
            n.ToString(s, Sp0, DeltaSp, EndOfLine, iFeatures);
            s += EndOfLine;
        }
        return s;
    }
    catch(...)
    {
    }
    return CMaaStringZ;
}

CMaaString CMaaXmlDocument::ToJson(int ApproxLength, const CMaaString &Sp0, const CMaaString &DeltaSp, const CMaaString &EndOfLine, int Flags, int MaxNameLen, int MaxAttrLen) const noexcept(noexcept_new)
{
    try
    {
        char Buffer[TOOLSLIB_CS_64K];
        CMaaConcatString s(Buffer, sizeof(Buffer), ApproxLength < 0 ? TOOLSLIB_CS_64K : ApproxLength);
        //CMaaConcatString s(ApproxLength < 0 ? 256 * 1024 : ApproxLength, 1);
        //CMaaString Sp0;
        //CMaaString DeltaSp = " ";

        CMaaXmlNode e = DocumentElement();
        if (e.GetName() == CMaaStringJsonNode || e.GetName() == CMaaStringJsonArray)
        {
            s += Sp0;
            CMaaString Name = StrToJson(e.FindAttribute(CMaaStringJsonName), MaxNameLen, Flags);
            if (Name.IsNotEmpty())// || !bArray)
            {
                s += '"';
                s += Name;
                s.Add("\": ", 3);
            }
            s += e.GetName() == CMaaStringJsonNode ? '{' : '[';
            s += EndOfLine;
            CMaaString Sp = Sp0 + DeltaSp;

            e.ToJson(s, Sp0 + DeltaSp, DeltaSp, EndOfLine, Flags, MaxNameLen, MaxAttrLen);
            s += e.GetName() == CMaaStringJsonNode ? '}' : ']';
            s += EndOfLine;

            return s;
        }
    }
    catch (...)
    {
    }
    return CMaaStringZ;
}

/*
bool CMaaXmlDocument::SetContent(CMaaFile File, CMaaString *errorMsg, int *errorLine, int *errorColumn)
{
     return false;
}
*/

CMaaXmlDocument & CMaaXmlDocument::operator=(const CMaaXmlDocument & That) noexcept
{
    CMaaWin32Locker_a2Locker_gLock_Atomic2;
    a2Locker_Lock;
    CMaaString n = m_Name;
    CMaaXmlNode r = m_Root; // fix non-recursive Atomic2 deadlock
    m_Name = That.m_Name; // mthr
    m_Root = That.m_Root; // safe
    a2Locker_UnLock;
    return *this;
}

/*
struct Lnk1 : public CMaaDLink
{
    CMaaXmlNode n;
    Lnk1(CMaaXmlNode &Node) noexcept
    :   n(Node)
    {
    }
    struct Ref
    {
        Ref() noexcept
        {
            GetAllocator().AddRef();
        }
        ~Ref()
        {
            GetAllocator().DelRef();
        }
    };
    ADD_ALLOCATOR(Lnk1)
};
DEF_ALLOCATOR(Lnk1)
static Lnk1::Ref sLnk1Ref;
*/

static bool constexpr IsAlphaXml(char c, int idx) noexcept
{
    if  (idx > 0 && ((c >= '0' && c <= '9') || c == ':'))
    {
        return true;
    }
    if  (c >= 'a' && c <= 'z')
    {
        return true;
    }
    if  (c >= 'A' && c <= 'Z')
    {
        return true;
    }
    if  ((c >= '‡' && c <= 'ˇ') || c == '∏')
    {
        return true;
    }
    if  ((c >= '¿' && c <= 'ﬂ') || c == '®')
    {
        return true;
    }
    return c == '_' || c == '-';
}

static bool constexpr IsSpace(char c) noexcept
{
    return c == ' ' || c == '\t' || c == 0;
}

static bool constexpr IsEOW(char c) noexcept
{
    return c == ' ' || c == '"' || c == '\n' || c == '\r' || c == '\'' || c == '\t' || c == '>' || c == '<' || c == '/' || c == ']' || c == 0 || c == '?';
}

#if 0

// unused
static constexpr CMaa256Bits
    sAlphaXml0("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz¿¡¬√ƒ≈®∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ‡·‚„‰Â∏ÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘˙˚¸˝˛ˇ_-", 120, false),
    sAlphaXmlN("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz¿¡¬√ƒ≈®∆«»… ÀÃÕŒœ–—“”‘’÷◊ÿŸ⁄€‹›ﬁﬂ‡·‚„‰Â∏ÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘˙˚¸˝˛ˇ_-0123456789:", 131, false),
    sSpace(" \t\0", 3, false),
    sEOW(" \"\n\r'\t></]?\0", 12, false);

static bool constexpr IsAlphaXml_(char c, int idx) noexcept
{
    return idx > 0 ? sAlphaXmlN.Test(c) : sAlphaXml0.Test(c);
}

static bool constexpr IsSpace_(char c) noexcept
{
    return sSpace.Test(c);
}

static bool constexpr IsEOW_(char c) noexcept
{
    return sEOW.Test(c);
}

#else

#define IsAlphaXml_ IsAlphaXml
#define IsSpace_ IsSpace
#define IsEOW_ IsEOW

#endif

bool CMaaXmlNode::AddAttributeVerifyName(const CMaaString &Name, const CMaaString &Value) noexcept(noexcept_new)
{
    //CMaaString AllowedSymbols1 = "";
    /*
    if (Name.Length() < 1)
    {
        return false;
    }
    if (Name[0] >= '0' && Name[0] <= '9')
    {
        return false;
    }
    */
    const int Len = (warning_int)Name.Length();
    const char* p = Name;
    for (int i = 0; i < Len; i++)
    {
        if  (!IsAlphaXml(p[i], i))
        {
            return false;
        }
    }
    return AddAttribute(Name, Value);
}
//-------------------------------------------------------------------------------------------------
#if 0
// No strings support in this commented code
bool CMaaXmlDocument::SetContent_Prev_Strict(CMaaString Text, CMaaString *errorMsg, int *errorLine, int *errorColumn)
{
    bool Ret = false;
    int cp = e_utf8;

    try
    {
        CMaaXmlDocument doc(m_Name);

        Text = CMaaString((const char *)Text);

        {
            CMaaString Line = Text.GetLine(false, true);
            int n = Line.FindCi("encoding=");
            if  (n > 0)
            {
                Line = Line.Mid(n + 9).GetWord(" '\"");
                if  (Line.FindCi("utf") < 0)
                {
                    cp = 1251;
                }
            }
        }

        CMaaDListAD<Lnk1> List;
        CMaaXmlNode c = doc.m_Root;

        char * p = (char *)(const char *)Text;
        *errorLine = 0;
        while(*p)
        {
            (*errorLine)++;
            *errorColumn = 0;
            char * p0 = p - 1;
            char * e = strchr(p, '\r');
            char * e2 = strchr(p, '\n');
            if  (!e)
            {
                e = p + strlen(p);
            }
            if  (e2 && e2 < e)
            {
                e = e2;
            }
            char *ee = e;
            char eChar = *ee;
            *ee = 0;
            while(*p == ' ') p++;
            while(e > p && e[-1] == ' ')
            {
                e--;
                *e = 0;
            }
            if  (*p != 0)
            {
                if  (*p != '<')
                {
                    *errorColumn = (int)(p - p0);
                    *errorMsg = "'<' is expected";
                    throw 1;
                }
                p++;
                if  (p >= e)
                {
                    *errorColumn = (int)(p - p0);
                    *errorMsg = "line is too short - there is no closing '>'";
                    throw 2;
                }
                if  (e[-1] != '>')
                {
                    *errorColumn = (int)(e - p0);
                    *errorMsg = "'>' is expected";
                    throw 3;
                }
                e--;
                if  (e > p && e[-1] == ' ') e--;
                if  (*p == '/')
                {
                    Lnk1 * l = List.GetFromBack();
                    if  (!l)
                    {
                        *errorColumn = (int)(p - p0);
                        *errorMsg = "element is not opened";
                        throw 4;
                    }
                    p++;
                    *e = 0;
                    CMaaString Name = l->n.GetName();
                    delete l;
                    if  (Name != p)
                    {
                        *errorColumn = (int)(p - p0);
                        *errorMsg = "element name to be closed mismatch";
                        throw 5;
                    }
                    l = List.LookAtBack();
                    c = l ? l->n : doc.m_Root;
                }
                else
                {
                    bool bClosed = false;
                    int Type = CMaaXmlNodeImpl::eElm;
                    if  (*p == '?')
                    {
                        p++;
                        if  (e <= p || e[-1] != '?')
                        {
                            *errorColumn = (int)(e - 1 - p0);
                            *errorMsg = "missing '?' in processing instruction";
                            throw 6;
                        }
                        e--;
                        Type = CMaaXmlNodeImpl::eProcInst;
                        bClosed = true;
                    }
                    else if (*p == '!')
                    {
                        p++;
                        if  ((int)(e - p) < (int)strlen("DOCTYPE"))
                        {
                            *errorColumn = (int)(p - p0);
                            *errorMsg = "\"DOCTYPE\" is expected";
                            throw 7;
                        }
                        if  (!CMaaIsMemEq(p, "DOCTYPE", 7))
                        {
                            *errorColumn = (int)(p - p0);
                            *errorMsg = "\"DOCTYPE\" is expected";
                            throw 8;
                        }
                        p += strlen("DOCTYPE");
                        while(*p == ' ' && p < e) p++;
                        *e = 0;
                        Type = CMaaXmlNodeImpl::eDoctype;
                        bClosed = true;
                        //CMaaXmlNode n = CMaaXmlNode(Type = CMaaXmlNodeImpl::eDoctype, p);
                    }
                    else if (e[-1] == '/')
                    {
                        bClosed = true;
                        e--;
                        while(e > p && e[-1] == ' ') e--;
                    }
                    CMaaString Name;
                    *e = 0;
                    char * p2 = p;
                    char * p3 = strchr(p, ' ');
                    if  (p3)
                    {
                        *p3 = 0;
                        p = p3 + 1;
                    }
                    else
                    {
                        p = e;
                    }
                    Name = p2;
                    CMaaXmlNode n = CMaaXmlNode(Type, Name);
                    // get attributes
                    while(p < e)
                    {
                        if  (*p == ' ')
                        {
                            p++;
                            continue;
                        }
                        char * p2 = strchr(p, '=');
                        if  (!p2)
                        {
                            *errorColumn = (int)(e - p0);
                            *errorMsg = "'=' not found";
                            throw 9;
                        }
                        *p2 = 0;
                        CMaaString Name = p;
                        p = p2 + 1;
                        if  (*p != '\"')
                        {
                            *errorColumn = (int)(p - p0);
                            *errorMsg = "'\"' is expected";
                            throw 10;
                        }
                        *p = 0;
                        p++;
                        p2 = p;
                        //char * pp = p;
                        while(p < e && *p != '\"')
                        {
                            //*pp++ = *p++;
                            p++;
                        }
                        if  (*p != '\"')
                        {
                            *errorColumn = (int)(p - p0);
                            *errorMsg = "'\"' is expected";
                            throw 12;
                        }
                        *p = 0;
                        //*pp = 0;

                        CMaaString Value = p2;
                        Value = CMaaXmlNode::DecodeAttr(Value, cp);
                        n.AddAttribute(Name, Value);
                        //printf("attr: %s, value: %s\n", (const char *)Name, (const char *)Value);
                        p++;
                    }
                    if  (Type == CMaaXmlNodeImpl::eDoctype)
                    {
                        doc.m_Name = n.GetName();
                    }
                    else
                    {
                        c.AppendChild(n);
                        if  (!bClosed)
                        {
                            Lnk1 * l = TL_NEW Lnk1(n);
                            if  (!l)
                            {
                                *errorColumn = (int)(p - p0);
                                *errorMsg = "there is not enough free memory";
                                throw 13;
                            }
                            List.AddAtBack(l);
                            c = n;
                        }
                    }
                    /*
                         {
                              CMaaString s = doc.ToString();
                              printf("\n\n%s\n", (const char *)s);
                         }
                         */
                }
            }
            *ee = eChar;
            if  (eChar && ee[1] != eChar && (ee[1] == '\r' || ee[1] == '\n'))
            {
                p = ee + 2;
            }
            else
            {
                p = ee + 1;
            }
        }
        (*errorLine)++;
        *errorColumn = 0;
        if  (List.IsNotEmpty())
        {
            *errorMsg = "not all elements are closed";
            throw 14;
        }
        *this = doc;
        Ret = true;
    }
    catch(...)
    {
    }
    return Ret;
}
#endif
//-------------------------------------------------------------------------------------------------

#define SkipCrLf if (p[i] == '\r') { if (p[i+1] == '\n') i++; (*errorLine)++; StartLine = i+1; continue; }
#define SkipLf if (p[i] == '\n') { (*errorLine)++; StartLine = i+1; continue; }

#define SkipCrLfOrLf SkipCrLf SkipLf

static CMaaUnivHash<CMaaString, CMaaString> * sphStringsCacheNull = nullptr;
bool CMaaXmlDocument::SetContent(CMaaString Text1, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures) noexcept(noexcept_new)
{
    if (iFeatures & eCachedAll)
    {
        CMaaUnivHash<CMaaString, CMaaString> hStringsCache;
        return SetContent(Text1, hStringsCache, errorMsg, errorLine, errorColumn, iFeatures);
    }
    return SetContent(Text1, *sphStringsCacheNull, errorMsg, errorLine, errorColumn, iFeatures);
}

bool CMaaXmlDocument::SetContent(CMaaString Text1, const CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures) noexcept(noexcept_new)
{
    return SetContent(Text1, (CMaaUnivHash<CMaaString, CMaaString>&)hStringsCache, errorMsg, errorLine, errorColumn, iFeatures & ~eCachedAll);
}

bool CMaaXmlDocument::SetContent(CMaaString Text1, CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures) noexcept(noexcept_new)
{
    CMaaStringsContinedBuffer m_ContBuf(Text1.Length(), s_PaddingBytes /*= 1*/); // does not alloc buffer if unused
    CMaaStringsContinedBuffer* pContBuf = iFeatures & eContinedBuffer ? &m_ContBuf : nullptr;

    //InitsCMaaXml256Bits();

    const bool bMainRefStringZT = (iFeatures & eMainRefString2) == eMainRefString2;

    CMaaString _errorMsg;
    int _errorLine, _errorColumn;
    if  (!errorMsg)
    {
        errorMsg = &_errorMsg;
    }
    if  (!errorLine)
    {
        errorLine = &_errorLine;
    }
    if  (!errorColumn)
    {
        errorColumn = &_errorColumn;
    }
    int StartLine = 0;

    bool Ret = false;
    int cp = e_utf8;

    try
    {
        CMaaWin32Locker_a2Locker_gLock_Atomic2;
        a2Locker_Lock;
        CMaaString OldName = m_Name; // mthr safe
        a2Locker_UnLock;

        CMaaXmlDocument doc(OldName);

        CMaaString Text = bMainRefStringZT ? Text1.NewCopy() : Text1.Str0Copy();
        for (int i = 0/*CMaaFile::ePfxUtf8*/; i < CMaaFile::ePfxMax; i++)
        {
            CMaaString prefix = CMaaFile::GetPrefix(i);
            if  (prefix.IsNotEmpty() && Text.IsLeft(prefix))
            {
                Text = Text.RefMid(prefix.Length());
                Text1 = Text;
                break;
            }
        }
        {
            CMaaString Line = Text.GetLine(false, true);
            const int n = (warning_int)Line.FindCi(0, "encoding=", 9);
            if  (n > 0)
            {
                Line = Line.RefMid(n + 9).GetWord(" '\"");
                if  (Line.FindCi("utf") < 0)
                {
                    cp = 1251;
                }
            }
        }

        //CMaaDListAD<Lnk1> List;
        CMaaPtr_<CMaaXmlNode, -1> Stack;
        int StackTop = 0;

        CMaaXmlNode c = doc.m_Root;

        const char * p = (const char*)Text;
        char * P = (char*)p;
        int Pos = 0;
        const int Len = (warning_int)Text.Length();
        *errorLine = 0;
        *errorColumn = 0;
        int zline, zcolumn, zstartline = 0;
        zline = zcolumn = -1;
        bool bIsEmpty = true;
        while(Pos < Len)
        {
            int x = (warning_int)Text.Find(Pos, '<');
            if  (x < 0)
            {
                x = Len;
            }
            int i;
            for (i = Pos; i < x; i++)
            {
                SkipCrLfOrLf;

                if  (!IsSpace_(p[i]))
                {
                    *errorMsg = "'<' is expected";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
            }
            if  (zline + zcolumn == -2)
            {
                zstartline = StartLine;
                zline = *errorLine;
                zcolumn = x - StartLine;
            }

            Pos = i + 1;
            if  (Pos >= Len)
            {
                break;
            }
            int el = *errorLine;
            int esl = StartLine;
            int epos = -1;
            for (i = Pos; i < Len; i++)
            {
                SkipCrLfOrLf;
                if  (!IsSpace_(p[i]))
                {
                    break;
                }
            }
            if  (i >= Len)
            {
                StartLine = esl;
                *errorLine = el;
                *errorColumn = Pos - esl;
                *errorMsg = "missed element name after '<'";
                throw 2;
            }
            bIsEmpty = false;
            Pos = i;
            char Type = 0;
            if  (p[i] == '/' || p[i] == '?' || p[i] == '!')
            {
                Type = p[i];
                Pos++;
            }
            for (i = Pos; i < Len; i++)
            {
                if  (!IsAlphaXml_(p[i], i - Pos))
                {
                    break;
                }
            }
            if  (i == Pos)
            {
                //*errorLine = el;
                *errorColumn = Pos - StartLine;
                *errorMsg = "element name is expected after '<'";
                throw 3;
            }
            int i0 = i;
            CMaaString ElementName = (iFeatures & eMainRefString) ? Text.RefMid(Pos, i - Pos) : pContBuf ? pContBuf->Put(CMaaString(p + Pos, i - Pos, CMaaString::eMemString), true) : CMaaString(p + Pos, i - Pos);
            (iFeatures& (eCachedNames| eCachedRONames)) && hStringsCache.Find(ElementName, &ElementName) && (iFeatures& eCachedNames) && hStringsCache.Add(ElementName, ElementName);

            el = *errorLine;
            esl = StartLine;
            epos = Pos;
            Pos = i;
            for (i = Pos; i < Len; i++)
            {
                SkipCrLfOrLf;
                if  (!IsSpace_(p[i]))
                {
                    break;
                }
            }
            if  (Type == '/' && p[i] != '>')
            {
                *errorColumn = i - StartLine;
                *errorMsg = "expected '>' in closing element name";
                throw 4;
            }
            if  (Type == '/' && p[i] == '>')
            {
                //Lnk1 * l = List.GetFromBack();
                if  (!StackTop)
                {
                    StartLine = esl;
                    *errorLine = el;
                    *errorColumn = epos - esl;
                    *errorMsg = "element is not opened";
                    throw 5;
                }
                CMaaString Name = Stack[--StackTop].GetName();
                Stack[StackTop].Empty();
                //delete l;
                if  (Name != ElementName)
                {
                    StartLine = esl;
                    *errorLine = el;
                    *errorColumn = epos - esl;
                    *errorMsg = "element name to be closed mismatch";
                    throw 6;
                }
                //l = List.LookAtBack();
                c = StackTop ? Stack[StackTop - 1] : doc.m_Root;

                zstartline = StartLine;
                zline = zcolumn = -1;

                Pos = i + 1;
                if (bMainRefStringZT && IsEOW_(P[i0]))
                {
                    P[i0] = 0;
                }
                continue;
            }


            if  (i == Pos && p[i] != '/')
            {
                if  (Type == '?' && p[i] == '?')
                {
                    // ok
                }
                else if (p[i] == '>')
                {
                    // ok
                }
                else
                {
                    *errorColumn = Pos - StartLine;
                    *errorMsg = "unexpected character in element name";
                    throw 7;
                }
            }

            bool bClosed = false;
            int tType = CMaaXmlNodeImpl::eElm;
            if  (Type == '?')
            {
                tType = CMaaXmlNodeImpl::eProcInst;
                //bClosed = true;
                Pos = i;
            }
            else if (Type == '!')
            {
                if  (ElementName != "DOCTYPE")
                {
                    StartLine = esl;
                    *errorLine = el;
                    *errorColumn = epos - esl;
                    *errorMsg = "\"DOCTYPE\" is expected";
                    throw 8;
                }
                /*
                    p += strlen("DOCTYPE");
                    while(*p == ' ' && p < e) p++;
                    *e = 0;
                    */
                tType = CMaaXmlNodeImpl::eDoctype;
                //bClosed = true;
                ////CMaaXmlNode n = CMaaXmlNode(Type = CMaaXmlNodeImpl::eDoctype, p);
                Pos = i;

                for (i = Pos; i < Len; i++)
                {
                    if  (!IsAlphaXml_(p[i], i - Pos))
                    {
                        break;
                    }
                }
                if  (i == Pos)
                {
                    //*errorLine = el;
                    *errorColumn = Pos - StartLine;
                    *errorMsg = "name of \"DOCTYPE\" is expected";
                    throw 9;
                }
                if (bMainRefStringZT && IsEOW_(P[i0]))
                {
                    P[i0] = 0;
                }
                i0 = i;
                ElementName = (iFeatures & eMainRefString) ? Text.RefMid(Pos, i - Pos) : pContBuf ? pContBuf->Put(CMaaString(p + Pos, i - Pos, CMaaString::eMemString), true) : CMaaString(p + Pos, i - Pos);
                (iFeatures& (eCachedNames| eCachedRONames)) && hStringsCache.Find(ElementName, &ElementName) && (iFeatures& eCachedNames) && hStringsCache.Add(ElementName, ElementName);

                Pos = i;
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (!IsSpace_(p[i]))
                    {
                        break;
                    }
                }
                if  (p[i] != '>')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'>' is expected";
                    throw 10;
                }
                Pos = i + 1;
                // Create DOCTYPE
                doc.m_Name = ElementName; //n.GetName();
                if (bMainRefStringZT && IsEOW_(P[i0]))
                {
                    P[i0] = 0;
                }
                continue;
            }
            else
            {
                Pos = i;
            }
#ifdef CMaaStringReplace
            CMaaString Name = ElementName;
#else
            CMaaString Name = (const char *)ElementName;
#endif
            (iFeatures& (eCachedNames| eCachedRONames)) && hStringsCache.Find(Name, &Name) && (iFeatures& eCachedNames) && hStringsCache.Add(Name, Name);
            CMaaXmlNode n = CMaaXmlNode(tType, Name);
            int attr_count = 0;
            // get attributes
            while(1)
            {
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (!IsSpace_(p[i]))
                    {
                        break;
                    }
                }
                if  (i >= Len || p[i] == '/' || p[i] == '>' || p[i] == '?')
                {
                    break;
                }
                Pos = i;
                for (; i < Len; i++)
                {
                    if  (!IsAlphaXml_(p[i], i - Pos))
                    {
                        break;
                    }
                }
                if  (i == Pos)
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "attribute name is expected";
                    throw 11;
                }
                int i00 = i;
                CMaaString aName = (iFeatures & eMainRefString) ? Text.RefMid(Pos, i - Pos) : pContBuf ? pContBuf->Put(CMaaString(p + Pos, i - Pos, CMaaString::eMemString), true) : CMaaString(p + Pos, i - Pos);
                if  (p[i] != '=')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'=' is expected";
                    throw 12;
                }
                if (bMainRefStringZT && IsEOW_(P[i00]))
                {
                    P[i00] = 0;
                }
                i++;
                const char cquote = p[i];
                if  (p[i] != '"' && p[i] != '\'')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'\"' or '\\'' is expected";
                    throw 13;
                }
                Pos = i + 1;
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (p[i] == cquote)//'"')
                    {
                        break;
                    }
                }
                if  (p[i] != cquote)//'"')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'\"' or '\\'' not found";
                    throw 14;
                }
                //CMaaString aValue = CMaaString(p + Pos, i - Pos);
                i00 = i;
                CMaaString aValue = Text.RefMid(Pos, i - Pos);
                Pos = ++i;
                if  (!IsSpace_(p[i]) && p[i] != '\r' && p[i] != '\n' &&
                     p[i] != '/' && p[i] != '>'  && p[i] != '?')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "space or newline or '/' or '>' is expected";
                    throw 15;
                }
#ifdef CMaaStringReplace
                CMaaString Name = aName;
                CMaaString Value = CMaaXmlNode::DecodeAttr(aValue, cp, pContBuf, (iFeatures & eMainRefString));
#else
                CMaaString Name = (const char *)aName;
                CMaaString Value = CMaaXmlNode::DecodeAttr((const char *)aValue, cp, pContBuf);
#endif
                (iFeatures& (eCachedNames| eCachedRONames)) && hStringsCache.Find(Name, &Name) && (iFeatures& eCachedNames) && hStringsCache.Add(Name, Name);
                (iFeatures& (eCachedValues| eCachedROValues)) && hStringsCache.Find(Value, &Value) && (iFeatures& eCachedValues) && hStringsCache.Add(Value, Value);
                n.AddAttribute(Name, Value);
                attr_count++;
                if (bMainRefStringZT && IsEOW_(P[i00]))
                {
                    P[i00] = 0;
                }
            }
            Pos = i;
            if  (Type == '?')
            {
                if  (p[i] != '?')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'?' is expected";
                    throw 20;
                }
                if (bMainRefStringZT && IsEOW_(P[i0]))
                {
                    P[i0] = 0;
                }
                Pos = ++i;
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (!IsSpace_(p[i]))
                    {
                        break;
                    }
                }
                if  (p[i] != '>')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'>' is expected";
                    throw 21;
                }
                bClosed = true;
            }
            else if (p[i] == '?')
            {
                *errorColumn = i - StartLine;
                *errorMsg = "'?' is unexpected";
                throw 22;
            }
            if  (p[i] == '/')
            {
                bClosed = true;
                Pos = i + 1;
            }

            for (i = Pos; i < Len; i++)
            {
                SkipCrLfOrLf;
                if  (!IsSpace_(p[i]))
                {
                    break;
                }
            }
            if  (p[i] != '>')
            {
                *errorColumn = i - StartLine;
                *errorMsg = "'>' is expected";
                throw 16;
            }
            if (bMainRefStringZT && IsEOW_(P[i0])) // && P[i0] != '?')
            {
                P[i0] = 0;
            }

            Pos = ++i;

            if  (attr_count >= 0 && !bClosed && (iFeatures & eAcceptStrings))
            // == -> >= 2018
            {
                bool bRssCData = false;
                int x, xr = 0;
                if  ((iFeatures & eRssCDataStrings) && Text.IsMid("<![CDATA[", Pos, 9))
                {
                    bRssCData = true;
                    x = (warning_int)Text.Find(Pos + 9, "]]>");
                    if  (x >= 0)
                    {
                        xr = x;
                        x += 3;
                        while(x < Text.Length() && IsSpace_(Text[x]))
                        {
                            x++;
                        }
                        if  (Text[x] != '<')
                        {
                            x = -1;
                        }
                    }
                }
                else
                {
                    x = (warning_int)Text.Find(Pos, '<');
                }
                if  (x >= 0)
                {
                    CMaaString Name = n.GetName();
                    const int y = (warning_int)Text.Find(bRssCData ? x + 1 : Pos, '>');
                    if  (y >= 0)
                    {
                        CMaaString ClosingTag = Text.RefMid(x + 1, y - x - 1);
                        int ii, j;
                        for (ii = j = 0; ii < ClosingTag.Length(); ii++)
                        {
                            if  (ClosingTag[ii] == '\r' || ClosingTag[ii] == '\n')
                            {
                                continue;
                            }
                            if  (ClosingTag[ii] == '/')
                            {
                                if (j != ii)
                                {
                                    ClosingTag[j] = '/';
                                }
                                j++;
                                continue;
                            }
                            if  (IsSpace_(ClosingTag[ii]))
                            {
                                continue;
                            }
                            break;
                        }
                        ClosingTag.SetLeftMid(j, ii);
                        for (ii = (warning_int)ClosingTag.Length() - 1; ii >= 0; ii--)
                        {
                            if  (ClosingTag[ii] == '\r' || ClosingTag[ii] == '\n')
                            {
                                continue;
                            }
                            if  (IsSpace_(ClosingTag[ii]))
                            {
                                continue;
                            }
                            break;
                        }
                        ClosingTag = ClosingTag.RefLeft(ii + 1);

                        if  (ClosingTag[0] == '/' && ClosingTag.IsMid(Name, 1))
                        {
                            if  (attr_count == 0 && !(iFeatures & eComplexStrings))
                            {
                                // it's a string, not node
                                i0 = x;
                                CMaaString RawVal = Text.RefMid(Pos, x - Pos);
                                //CMaaString Value = bRssCData ? RawVal.Mid(9, xr - Pos - 9) : CMaaXmlNode::DecodeAttr(RawVal, cp);
                                CMaaString Value = bRssCData ? (pContBuf ? pContBuf->Put(RawVal.RefMid(9, xr - Pos - 9), true) : (iFeatures & eMainRefString) ? RawVal.RefMid(9, (i0 = xr) - Pos - 9) : RawVal.Mid(9, xr - Pos - 9)) : CMaaXmlNode::DecodeAttr(RawVal, cp, pContBuf, (iFeatures & eMainRefString));
                                if (bMainRefStringZT && IsEOW_(P[i0]))
                                {
                                    P[i0] = 0;
                                }

                                //Lnk1 * l = List.LookAtBack();
                                if  (!StackTop)
                                {
                                    StartLine = zstartline;
                                    *errorLine = zline;
                                    *errorColumn = zcolumn;
                                    *errorMsg = "element for new string is not opened";
                                    throw 18;
                                }
                                (iFeatures & (eCachedNames| eCachedRONames)) && hStringsCache.Find(Name, &Name) && (iFeatures& eCachedNames) && hStringsCache.Add(Name, Name);
                                (iFeatures & (eCachedValues| eCachedROValues)) && hStringsCache.Find(Value, &Value) && (iFeatures& eCachedValues) && hStringsCache.Add(Value, Value);
                                Stack[StackTop - 1].AddString(Name, Value);

                                for (i = Pos; i <= y; i++)
                                {
                                    SkipCrLfOrLf;
                                }
                                Pos = y + 1;
                                continue;
                            }
                            else // 2018
                            {
#ifdef TOOLSLIB_XML_COMPLEX_STRINGS_SUPPORT
                                i0 = x;
                                CMaaString RawVal = Text.RefMid(Pos, x - Pos);
                                //CMaaString Value = bRssCData ? RawVal.Mid(9, xr - Pos - 9) : CMaaXmlNode::DecodeAttr(RawVal, cp);
                                CMaaString Value = bRssCData ? (pContBuf ? pContBuf->Put(RawVal.RefMid(9, xr - Pos - 9), true) : (iFeatures & eMainRefString) ? RawVal.RefMid(9, (i0 = xr) - Pos - 9) : RawVal.Mid(9, xr - Pos - 9)) : CMaaXmlNode::DecodeAttr(RawVal, cp, pContBuf, (iFeatures & eMainRefString));
                                if (bMainRefStringZT && IsEOW_(P[i0]))
                                {
                                    P[i0] = 0;
                                }
                                (iFeatures & (eCachedStrings| eCachedROStrings)) && hStringsCache.Find(Value, &Value) && (iFeatures & eCachedStrings) && hStringsCache.Add(Value, Value);
                                n.SetString(Value);
                                Pos = y + 1;
                                bClosed = true;
#else
                                *errorColumn = Pos - StartLine;
                                *errorMsg = "complex string with attributes";
                                throw 23;
#endif
                            }
                        }
                    }
                }
            }
            if  (bClosed && attr_count == 0 && (iFeatures & (eTreatEmptyNodeAsAString | eComplexStrings)) == eTreatEmptyNodeAsAString)
            {
                // empty node or string?

                //making string
                //Lnk1 * l = List.LookAtBack();
                if  (!StackTop)
                {
                    StartLine = zstartline;
                    *errorLine = zline;
                    *errorColumn = zcolumn;
                    *errorMsg = "element for new string is not opened";
                    throw 18;
                }
                Stack[StackTop - 1].AddString(n.GetName(), CMaaStringZ);
                continue;
            }

            c.AppendChild(n);
            if  (!bClosed)
            {
                //Lnk1* l = TL_NEW Lnk1(n);
                //if  (!l)
                if (!Stack.CheckExpand(StackTop + 1))
                {
                    *errorColumn = Pos - StartLine;
                    *errorMsg = "there is not enough free memory";
                    throw 17;
                }
                //List.AddAtBack(l);
                c = Stack[StackTop++] = n;
            }
        }
        if  (bIsEmpty)
        {
            StartLine = 0;
            *errorMsg = "document is empty: '<' is expected";
            *errorColumn = 0;
            *errorLine = 0;
            throw 19;
        }
        //(*errorLine)++;
        *errorColumn = Pos - StartLine;
        //if  (List.IsNotEmpty())
        if (StackTop)
        {
            *errorMsg = "not all elements are closed";
            throw 18;
        }
        //CMaaString tmp = doc.ToString();
        *this = doc;
        Ret = true;
    }
    catch(int /*x*/)
    {
        //static int err = x;
        //(*errorLine)++;
        //(*errorColumn)++;
        //throw;
    }
    catch(...)
    {
        //(*errorLine)++;
        //(*errorColumn)++;
        //throw;
    }
    if  (!Ret)
    {
        //static int aa = 0;
        ////const char * msg = *errorMsg;
        //aa++;
        (*errorLine)++;
        if (cp == e_utf8)
        {
            *errorColumn = (warning_int)Text1.RefMid(StartLine, *errorColumn).Utf8CharCount() + 1;
        }
        else
        {
            (*errorColumn)++;
        }
    }
    return Ret;
}

bool CMaaXmlDocument::SetJson(const CMaaString &Text, CMaaString* errorMsg, int* errorLine, int* errorColumn) noexcept(noexcept_new)
{
    bool Ret = false;
    //int cp = e_utf8;
    const bool bWeak = false;

    CMaaString _errorMsg;
    int _errorLine, _errorColumn;
    if  (!errorMsg)
    {
        errorMsg = &_errorMsg;
    }
    if  (!errorLine)
    {
        errorLine = &_errorLine;
    }
    if  (!errorColumn)
    {
        errorColumn = &_errorColumn;
    }
    errorMsg->Empty();
    *errorLine = 0;
    *errorColumn = 0;
    int StartLine = 0;

    try
    {
        CMaaWin32Locker_a2Locker_gLock_Atomic2;
        a2Locker_Lock;
        CMaaString OldName = m_Name; // mthr safe
        a2Locker_UnLock;

        CMaaXmlDocument doc(OldName);

        CMaaXmlNode pi = doc.CreateProcessingInstruction();
        doc.AddProcessingInstruction(pi);
        CMaaXmlNode r = doc.m_Root;
        //CMaaXmlNode e = r;
        //bool bRoot = true;
        int iHasNext = -1;

        //CMaaDListAD<Lnk1> List;
        CMaaXmlNode c = r;// doc.m_Root;

        const char* p = (const char*)Text;
        int Pos = 0;
        const int Len = (warning_int)Text.Length();
        //int zline, zcolumn;
        //zline = zcolumn = -1;
        //bool bIsEmpty = true;
        while (Pos < Len)
        {
            int i, l;
            for (i = Pos; i < Len; i++)
            {
                SkipCrLfOrLf;
                break;
            }
            while (i < Len && IsSpace(p[i]))
            {
                i++;
            }
            if (i > Pos)
            {
                Pos = i;
                continue;
            }
            CMaaString Name;
            if (i < Len && c.GetName() == CMaaStringJsonArray)// || c.GetHandle() == r.GetHandle())
            {
                // value of array //or root
            }
            else if (i < Len && (p[i] == '"' || IsAlphaXml(p[i], 0)))
            {
                if (iHasNext == 0)
                {
                    *errorMsg = "':' is missed";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
                int p1;
                if (p[i] == '"')
                {
                    p1 = ++i;
                    while (i < Len && IsAlphaXml(p[i], i - p1) && p[i] != '"')
                    {
                        l = JsonCharLen(p, i, Len, bWeak);
                        if (l < 0)
                        {
                            *errorMsg = "\name\" syntax error";
                            *errorColumn = i - StartLine;
                            throw 1;
                        }
                        i += l;
                    }
                    if (i == p1 || i >= Len || p[i] != '"')
                    {
                        *errorMsg = "\"name\" is expected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    Name = Text.Mid(p1, i - p1);
                    i++;
                }
                else
                {
                    p1 = i;
                    while (i < Len && IsAlphaXml(p[i], i - p1) && p[i] != '"' && p[i] != ':')
                    {
                        l = JsonCharLen(p, i, Len, bWeak);
                        if (l < 0)
                        {
                            *errorMsg = "name syntax error";
                            *errorColumn = i - StartLine;
                            throw 1;
                        }
                        i += l;
                    }
                    if (i == p1 || i >= Len || p[i] == '"')
                    {
                        *errorMsg = "\"name\" is expected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    Name = Text.Mid(p1, i - p1);
                }
                Name = JsonToStr(Name);
                while (i < Len && IsSpace(p[i]))
                {
                    i++;
                }
                if (i >= Len || p[i] != ':')
                {
                    *errorMsg = "':' is expected";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
                i++;
                while (i < Len && IsSpace(p[i]))
                {
                    i++;
                }
                while (1)
                {
                    Pos = i;
                    for (; i < Len; i++)
                    {
                        SkipCrLfOrLf;
                        break;
                    }
                    while (i < Len && IsSpace(p[i]))
                    {
                        i++;
                    }
                    if (i > Pos)
                    {
                        Pos = i;
                        continue;
                    }
                    break;
                }
            }
            if (i < Len && (p[i] == '{' || p[i] == '['))
            {
                CMaaXmlElement n = doc.CreateElement(p[i] == '{' ? CMaaStringJsonNode : CMaaStringJsonArray);
                n.AddAttribute(CMaaStringJsonName, Name);
                c.AppendChild(n);

                /*
                Lnk1* l = TL_NEW Lnk1(n);
                if (!l)
                {
                    *errorColumn = Pos - StartLine;
                    *errorMsg = "there is not enough free memory";
                    throw 17;
                }
                List.AddAtBack(l);
                */
                c = n;
                Pos = ++i;
                iHasNext = -1;
                continue;
            }
            if (i < Len && (p[i] == '}' || p[i] == ']'))
            {
                if (iHasNext == 1)
                {
                    if (p[i] == '}')
                    {
                        *errorMsg = "'}' is unexpected: not last";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    else
                    {
                        *errorMsg = "']' is unexpected: not last";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                }
                if (p[i] == '}')
                {
                    if (c.GetName() != CMaaStringJsonNode)
                    {
                        *errorMsg = "'}' is unexpected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                }
                else
                {
                    if (c.GetName() != CMaaStringJsonArray)
                    {
                        *errorMsg = "']' is unexpected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                }
                /*
                if (c.GetParent().GetHandle() == r.GetHandle())
                {
                    if (p[i] == '}')
                    {
                        *errorMsg = "'}' is unexpected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    else
                    {
                        *errorMsg = "']' is unexpected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                }
                */
                c = c.GetParent();
                iHasNext = 0;
                i++;
                while (1)
                {
                    Pos = i;
                    for (; i < Len; i++)
                    {
                        SkipCrLfOrLf;
                        break;
                    }
                    while (i < Len && IsSpace(p[i]))
                    {
                        i++;
                    }
                    if (i > Pos)
                    {
                        Pos = i;
                        continue;
                    }
                    break;
                }
                if (i < Len && p[i] == ',')
                {
                    iHasNext = 1;
                    i++;
                    if (c.GetHandle() == r.GetHandle())
                    {
                        *errorMsg = "',' is unexpected";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    while (1)
                    {
                        Pos = i;
                        for (; i < Len; i++)
                        {
                            SkipCrLfOrLf;
                            break;
                        }
                        while (i < Len && IsSpace(p[i]))
                        {
                            i++;
                        }
                        if (i > Pos)
                        {
                            Pos = i;
                            continue;
                        }
                        break;
                    }
                }
                if (i < Len && c.GetHandle() == r.GetHandle())
                {
                    *errorMsg = "end of document is expected";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
                continue;
            }
            CMaaString Val;
            int p1 = i;
            if (i < Len && p[i] == '"')
            {
                p1 = ++i;
                for (; i < Len && p[i] != '"'; i++)
                {
                    SkipCrLfOrLf;
                    l = JsonCharLen(p, i, Len, bWeak);
                    if (l < 0)
                    {
                        *errorMsg = "\"val\" syntax error";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    i += (l - 1);
                }
                if (i >= Len || p[i] != '"')
                {
                    *errorMsg = "'\"' is expected";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
                Val = Text.Mid(p1, i - p1);
                Val = JsonToStr(Val);
                i++;
                CMaaXmlElement n = doc.CreateElement(CMaaStringJsonVal);
                n.AddAttribute(CMaaStringJsonName, Name);
                //n.AddAttribute(CMaaStringJsonType, CMaaStringJsonString);
                n.AddAttribute(CMaaStringJsonValue, Val);
                c.AppendChild(n);
            }
            else
            {
                // int or float
                // double atof(const char *string); // ÔÂÓ·‡ÁÓ‚‡ÌËÂ ÒÚÓÍË ‚ ˜ËÒÎÓ ÚËÔ‡ double
                // [ ][{+|-}][ˆËÙ˚][.ˆËÙ˚][{e|E}[{+|-}]ˆËÙ˚]

                if (i + 1 < Len && (p[i] == '-' || p[i] == '+') && ((p[i + 1] >= '0' && p[i + 1] <= '9') || p[i + 1] == '.'))
                {
                    i++;
                }
                int dots = 0, nums = 0;
                for (; i < Len && ((p[i] >= '0' && p[i] <= '9') || p[i] == '.'); i++)
                {
                    if (p[i] == '.')
                    {
                        if (++dots > 1)
                        {
                            *errorMsg = "'.' is unexpected";
                            *errorColumn = i - StartLine;
                            throw 1;
                        }
                    }
                    else
                    {
                        nums++;
                    }
                }
                int E = -1;
                if (nums)
                {
                    E = dots; // 0 - int candidate, 1 - float candidate
                }
                if (nums && i < Len && (p[i] == 'E' || p[i] == 'e'))
                {
                    E = 1;
                    i++;
                    if (i < Len)
                    {
                        if (p[i] == '+' || p[i] == '-')
                        {
                            i++;
                        }
                        if (i < Len && p[i] >= '0' && p[i] <= '9')
                        {
                            i++;
                            while (i < Len && p[i] >= '0' && p[i] <= '9') i++;
                        }
                        else
                        {
                            E = -1;
                        }
                    }
                    else
                    {
                        E = -1;
                    }
                }
                if (i == p1 && Text.IsMid("true", p1, 4))
                {
                    i += 4; dots = -1;
                }
                else if (i == p1 && Text.IsMid("false", p1, 5))
                {
                    i += 5; dots = -1;
                }
                else if (i == p1 && Text.IsMid("null", p1, 4))
                {
                    i += 4; dots = -2;
                }
                if (i == p1)
                {
                    if (Name.IsNotEmpty())
                    {
                        *errorMsg = "missed value";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                    else
                    {
                        *errorMsg = "missed field/node/array";
                        *errorColumn = i - StartLine;
                        throw 1;
                    }
                }
                if (E < 0 && dots >= 0)
                {
                    *errorMsg = "number is expected";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
                Val = Text.Mid(p1, i - p1);
                //Val = JsonToStr(Val);
                CMaaXmlElement n = doc.CreateElement(CMaaStringJsonVal);
                n.AddAttribute(CMaaStringJsonName, Name);
                n.AddAttribute(CMaaStringJsonType, !dots && E != 1 ? CMaaStringJsonInt : dots < 0 ? (dots == -2 ? CMaaStringJsonNull : CMaaStringJsonBool) : CMaaStringJsonFloat);
                n.AddAttribute(CMaaStringJsonValue, Val);
                c.AppendChild(n);
            }
            iHasNext = 0;
            while (1)
            {
                Pos = i;
                for (; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    break;
                }
                while (i < Len && IsSpace(p[i]))
                {
                    i++;
                }
                if (i > Pos)
                {
                    Pos = i;
                    continue;
                }
                break;
            }
            if (i < Len && p[i] == ',')
            {
                iHasNext = 1;
                i++;
                while (1)
                {
                    Pos = i;
                    for (; i < Len; i++)
                    {
                        SkipCrLfOrLf;
                        break;
                    }
                    while (i < Len && IsSpace(p[i]))
                    {
                        i++;
                    }
                    if (i > Pos)
                    {
                        Pos = i;
                        continue;
                    }
                    break;
                }
            }
            continue;
        }
        //(*errorLine)++;
        *errorColumn = Pos - StartLine;
        if (c.GetHandle() != r.GetHandle())
        {
            *errorMsg = "not all elements are closed";
            throw 18;
        }
        if (!doc.DocumentElement())
        {
            *errorMsg = "empty document";
            *errorColumn = Pos - StartLine;
            throw 1;
        }
        //CMaaString tmp = doc.ToString();
        *this = doc;
        Ret = true;
    }
    catch (int /*x*/)
    {
        //static int err = x;
        //(*errorLine)++;
        //(*errorColumn)++;
        //throw;
    }
    catch (...)
    {
        //(*errorLine)++;
        //(*errorColumn)++;
        //throw;
    }
    if (!Ret)
    {
        //static int aa = 0;
        ////const char * msg = *errorMsg;
        //aa++;
        (*errorLine)++;
        //(*errorColumn)++;
        *errorColumn = (warning_int)Text.RefMid(StartLine, *errorColumn).Utf8CharCount() + 1;
    }
    return Ret;
}

//CMaaRefDLink<CMaaXmlNodeImpl> m_Imp;
//CMaaString szTxt;
//bool bConst;

/*
CMaaXmlNode::XX::XX(CMaaXmlNode &x, CMaaString Txt)
:       m_Imp(*(CMaaRefDLink<CMaaXmlNodeImpl> *)m_ImpBuffer), //x.m_Imp),
     szTxt(Txt),
     bConst(false)
{
     //new_(m_Imp(*(CMaaRefDLink<CMaaXmlNodeImpl> *)m_ImpBuffer), //x.m_Imp),
     new_(&m_Imp, CMaaRefDLink<CMaaXmlNodeImpl>(x.m_Imp));
}
CMaaXmlNode::XX::XX(const XX &That)
:       m_Imp(*(CMaaRefDLink<CMaaXmlNodeImpl> *)m_ImpBuffer), //x.m_Imp),
     szTxt(That.szTxt),
     bConst(That.bConst)
{
     new_(&m_Imp, CMaaRefDLink<CMaaXmlNodeImpl>(That.m_Imp));
}
CMaaXmlNode::XX::~XX()
{
     m_Imp.~CMaaRefDLink();
}
CMaaXmlNode::XX::XX(const CMaaXmlNode &x, CMaaString Txt)
:       m_Imp(*(CMaaRefDLink<CMaaXmlNodeImpl> *)m_ImpBuffer), //x.m_Imp),
//:     m_Imp(x.m_Imp),
     szTxt(Txt),
     bConst(true)
{
     new_(&m_Imp, CMaaRefDLink<CMaaXmlNodeImpl>(x.m_Imp));
}
bool CMaaXmlNode::XX::operator=(CMaaString txt)
{
     if   (bConst)
     {
          return false;
     }
     CMaaXmlNode n(m_Imp.IsNotNull() ? m_Imp.m_Imp : nullptr);
     return n.AddAttribute(szTxt, txt);
}
bool CMaaXmlNode::XX::operator=(const XX & That)
{
     if   (bConst)
     {
          return false;
     }
     CMaaXmlNode n1(m_Imp.IsNotNull() ? m_Imp.m_Imp : nullptr);
     CMaaXmlNode n2(That.m_Imp.IsNotNull() ? That.m_Imp.m_Imp : nullptr);
     return n1.AddAttribute(szTxt, n2.FindAttribute(That.szTxt));
}
*/

CMaaXmlNode::XX::XX(CMaaXmlNode &x, const CMaaString &Txt) noexcept
:   m_Impl(x.m_Imp),
    szTxt(Txt),
    bConst(false)
{
    if  (m_Impl)
    {
        m_Impl->AddRef();
    }
}
CMaaXmlNode::XX::XX(const CMaaXmlNode &x, const CMaaString &Txt) noexcept
:   m_Impl((CMaaXmlNodeImpl *)x.m_Imp),
    szTxt(Txt),
    bConst(true)
{
    if  (m_Impl)
    {
        m_Impl->AddRef();
    }
}
CMaaXmlNode::XX::XX(const XX &That) noexcept
:   m_Impl(That.m_Impl),
    szTxt(That.szTxt),
    bConst(That.bConst)
{
}
CMaaXmlNode::XX::~XX() noexcept
{
    if  (m_Impl)
    {
        if  (!m_Impl->Release())
        {
#if MANUAL_XML_REFS
            delete m_Impl;
#endif
        }
    }
}

/*
CMaaXmlNode::XX::XX(CMaaXmlNode &x, CMaaString Txt)
:    szTxt(Txt),
     bConst(false)
{
     new_(&n(), CMaaXmlNode(x));
}
CMaaXmlNode::XX::XX(const CMaaXmlNode &x, CMaaString Txt)
:    szTxt(Txt),
     bConst(true)
{
     new_(&n(), CMaaXmlNode(x));
}
CMaaXmlNode::XX::XX(const XX &That)
:    szTxt(That.szTxt),
     bConst(That.bConst)
{
     new_(&n(), CMaaXmlNode(That.n()));
}
CMaaXmlNode::XX::~XX()
{
     n().~CMaaRefDLink();
}
*/

const CMaaString& CMaaXmlNode::XX::crString() const noexcept
{
    return m_Impl ? m_Impl->FindAttribute(szTxt) : gsCMaaStringZ;
}

CMaaXmlNode::XX::operator CMaaString() const noexcept
{
    return m_Impl ? m_Impl->FindAttribute(szTxt) : gsCMaaStringZ;
}
/*
CMaaXmlNode::XX::operator const char * () const
{
     return n().FindAttribute(szTxt);
}
*/
CMaaXmlNode::XX::operator const CMaaXmlNode() const noexcept
{
    return m_Impl ? m_Impl->FindNode(szTxt) : nullptr;
}
CMaaXmlNode::XX::operator CMaaXmlNode() noexcept
{
    return m_Impl ? m_Impl->FindNode(szTxt) : nullptr;
}
bool CMaaXmlNode::XX::operator=(const CMaaString &txt) noexcept(noexcept_new)
{
    if  (bConst)
    {
        return false;
    }
    return m_Impl && m_Impl->AddAttribute(szTxt, txt);
}
bool CMaaXmlNode::XX::operator=(const XX & That) noexcept(noexcept_new)
{
    if  (bConst)
    {
        return false;
    }
    return m_Impl && m_Impl->AddAttribute(szTxt, That.m_Impl ? That.m_Impl->FindAttribute(That.szTxt) : gsCMaaStringZ);
}

bool CMaaXmlDocument::LoadFromFile(const CMaaString &FileName, bool bThrow, CMaaString *errorMsg, int *errorLine, int *errorColumn, int iFeatures)
{
    CMaaFile f(FileName, CMaaFile::eR_SrSw, bThrow);
    if  (f.IsOpen())
    {
        CMaaString txt = f.ReadAll();
        CMaaXmlDocument doc(CMaaStringDoc);
        if  (doc.SetContent(txt, errorMsg, errorLine, errorColumn, iFeatures))
        {
            *this = doc;
            return true;
        }
    }
    return false;
}

int CMaaXmlDocument::UpdateFromFile(const CMaaString& FileName, int ms, bool bThrow, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures)
{
    if (iFeatures & eCachedAll)
    {
        CMaaUnivHash<CMaaString, CMaaString> hStringsCache;
        return UpdateFromFile(FileName, hStringsCache, ms, bThrow, errorMsg, errorLine, errorColumn, iFeatures);
    }
    return UpdateFromFile(FileName, *sphStringsCacheNull, ms, bThrow, errorMsg, errorLine, errorColumn, iFeatures);
}

int CMaaXmlDocument::UpdateFromFile(const CMaaString& FileName, const CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, int ms, bool bThrow, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures)
{
    return UpdateFromFile(FileName, (CMaaUnivHash<CMaaString, CMaaString>&)hStringsCache, ms, bThrow, errorMsg, errorLine, errorColumn, iFeatures);
}

int CMaaXmlDocument::UpdateFromFile(const CMaaString& FileName, CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, int ms, bool bThrow, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures)
{
    CMaaXmlElement e = DocumentElement();
    bool bGot;
    const void* p = e.GetHiddenPtr(-'t', bGot);
    const time_t t = bGot ? (time_t)p : (time_t)-1;
    const int u = e.GetHiddenInt(-'u', 0);
    p = e.GetHiddenPtr(-'s', bGot);
    const _qword sz = bGot ? (_qword)p : -1;

    int fu;
    const time_t ft = CMaaFile::GetDateTime(FileName, &fu, bThrow);
    const _qword fsz = CMaaFile::Length(FileName, bThrow);

    int uu;
    const time_t tt = CMaaGetTimeOfDay(&uu);// time(nullptr);
    //const int uu = fu;
    const double diff = (double)(tt - ft) + (uu - fu) / 1000000.0;
    const double diffu = diff >= 0 ? diff : -diff;

    if (ft != (time_t)-1 && ((ft != t || fu != u || fsz != sz || ms == -1) && diffu >= (double)(ms / 1000.0)))
    {
        CMaaFile f(FileName, CMaaFile::eR_SrSw, bThrow);
        if (f.IsOpen())
        {
            CMaaString txt = f.ReadAll();
            CMaaXmlDocument doc(CMaaStringDoc);
            if (doc.SetContent(txt, hStringsCache, errorMsg, errorLine, errorColumn, iFeatures))
            {
                e = doc.DocumentElement();
                e.AddHiddenPtr(-'t', (void*)ft);
                e.AddHiddenInt(-'u', fu);
                e.AddHiddenPtr(-'s', (void*)fsz);
                *this = doc;
                return 1;
            }
            return 0;
        }
    }
    return -1;
}

bool CMaaXmlDocument::SaveToFile(const CMaaString &FileName, bool bThrow, const CMaaString &Sp0, const CMaaString &DeltaSp, const CMaaString &EndOfLine, int iFeatures) const
{
    CMaaString txt = ToString(-1, Sp0, DeltaSp, EndOfLine, iFeatures);
    CMaaString prefix;

    CMaaXmlNode n = FirstChild();
    while(n)
    {
        if  (n.IsProcessingInstruction())
        {
            if  (n.FindAttribute("encoding") == "utf-8")
            {
                prefix = CMaaFile::GetPrefix(CMaaFile::ePfxUtf8);
            }
            break;
        }
        n = n.NextSibling();
    }

    CMaaFile f(FileName, CMaaFile::eWCD_SrSw, bThrow);
    if  (f.IsOpen())
    {
        if  (f.Write(prefix) == (_dword)prefix.Length() && f.Write(txt) == (_dword)txt.Length())
        {
            f.Close();
            return true;
        }
    }
    return false;
}

void CMaaXmlNode::ConvertToUtf8(bool /*bChilds*/, _dword cp) noexcept(noexcept_new)
{
    if  (m_Imp)
    {
        m_Imp->ConvertToUtf8(cp);

        CMaaXmlNode c = FirstChild();
        for (; c.IsNotNull(); c = c.NextSibling())
        {
            c.ConvertToUtf8(true, cp);
        }
    }
}

void CMaaXmlDocument::ConvertToUtf8(_dword cp) noexcept(noexcept_new)
{
    int N = 0;
    CMaaXmlNode n = FirstChild();
    while(n)
    {
        if  (n.IsProcessingInstruction())
        {
            CMaaString txt = n["encoding"];
            if  (txt != "utf-8")
            {
                if  (txt == "windows-1251" && cp == (_dword)-1)
                {
                    cp = 1251;
                }
                n["encoding"] = "utf-8";
                N++;
                //break;
            }
            else if (N == 0)
            {
                return;// false;
            }
        }
        n = n.NextSibling();
    }

    n = FirstChild();
    while(n)
    {
        if  (n.IsElement())
        {
            n.ConvertToUtf8(true, cp);
        }
        n = n.NextSibling();
    }
}

void CMaaXmlDocument::AddReplaceProcessingInstruction(CMaaString Encoding, CMaaString Version, _dword ToUtf8Cp) noexcept(noexcept_new)
{
    if  ((int)ToUtf8Cp != -2 && Encoding == "utf-8")
    {
        ConvertToUtf8(ToUtf8Cp);
    }

    CMaaXmlNode pi = FirstChild();
    for (; pi.IsNotNull(); pi = pi.NextSibling())
    {
        if  (pi.IsProcessingInstruction())
        {
            CMaaXmlProcessingInstruction p = pi;
            if  (p.FindAttribute("encoding") != Encoding || p.FindAttribute("encoding") != Version)
            {
                p.AddInstruction(CMaaXmlProcessingInstruction::eVersion, Version);
                p.AddInstruction(CMaaXmlProcessingInstruction::eEncoding, Encoding);
            }
            //pi.Remove();
            break;
        }
    }
    if  (pi.IsNull())
    {
        CMaaXmlProcessingInstruction pi = CreateProcessingInstruction("xml");
        if  (pi.IsNotNull())
        {
            pi.AddInstruction(CMaaXmlProcessingInstruction::eVersion, Version);
            pi.AddInstruction(CMaaXmlProcessingInstruction::eEncoding, Encoding);
            AddProcessingInstruction(pi); //AppendChild(pi);
        }
    }
    //
    CMaaXmlNode FirstNode = FirstChild();

    if  (FirstNode.IsNull() || !pi.InsertBefore(FirstNode))
    {
        AppendChild(pi);
    }
}
//-------------------------------------------------------------------------------------------------
CMaaString CMaaXmlNode::EncodeString(const CMaaString &s, int iFeatures, const CMaaString &Name) noexcept(noexcept_new)
{
    return ((iFeatures & CMaaXmlDocument::eRssCDataStrings) && s.Find("]]>") < 0 && Name != "lastBuildDate") ? (CMaaString("<![CDATA[") + s + "]]>").s() : EncodeAttr(s, iFeatures);
}
//-------------------------------------------------------------------------------------------------
void CMaaXmlNode::EncodeString(CMaaConcatString &c, const CMaaString &s, int iFeatures, const CMaaString &Name) noexcept(noexcept_new)
{
    if  ((iFeatures & CMaaXmlDocument::eRssCDataStrings) && s.Find("]]>") < 0 && Name != "lastBuildDate")
    {
        c += "<![CDATA[";
        c += s;
        c += "]]>";
    }
    else
    {
        EncodeAttr(c, s, iFeatures);
    }
}

int CMaaXmlNode::GetRefCount() const noexcept
{
    return m_Imp ? m_Imp->GetRefsCount() : 0;
}

