
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

#include "perm.h"
#include "temp.h"

//#define gp1 (*p1)
//extern test1 * p1;
/*
test1 * p1 = reinterpret_cast<test1*>(2);

test1::test1()
{
    xx = 0;
}
test1::~test1()
{
}
int test1::f(int x)
{
    printf("this == %p, x = %d\n", this, x); fflush(stdout);
}
*/


//#include "MaaXml.h"

void * gV = nullptr;

//int gDbgXml = 0;
struct strEnum
{
    strEnum(CMaaXmlNode::EnumAttributesFn _Fn, CMaaXmlNode _Node, void * _Param)
    :   Fn(_Fn),
        Node(_Node),
        Param(_Param)
    {
    }
    CMaaXmlNode::EnumAttributesFn Fn;
    CMaaXmlNode Node;
    void * Param;
};
void CMaaXmlNode::stEnumAttrFn(CMaaString AttrName, CMaaString AttrValue, void *Param)
{
    strEnum * e = (strEnum *)Param;
    e->Fn(AttrName, AttrValue, e->Node, e->Param);
}
//-------------------------------------------------------------------------------------------------
class CMaaXmlNodeImpl;
static int SortFn1(const void * a1, const void * a2);
//-------------------------------------------------------------------------------------------------
class CMaaXmlNodeImpl
{
    friend class CMaaXmlNode;
    friend int SortFn1(const void * a1, const void * a2);
    int m_Type;
    CMaaString m_Name;
    CMaaString m_String; // 2018
public:
    struct Attr : public CMaaDLink
    {
        CMaaString m_Name, m_Value;

        Attr(CMaaString Name, CMaaString Value)
        :   m_Name(Name),
            m_Value(Value)
        {
        }
        void SetValue(CMaaString Value)
        {
            m_Value = Value;
        }
        ADD_ALLOCATOR(Attr)
    };
    struct NPtr
    {
        CMaaRefDLink<CMaaXmlNodeImpl>::Impl * m_1stNodeImp;
        int N;
    };
    struct HiddenPtrs
    {
        void * m_Ptr;
        void (*deref)(CMaaString, void *, void *); // Name, ptr, obj
        void * obj;
    };
    struct sFindNodeWithAttrKey
    {
        CMaaString Name, AttrName, AttrVal;
        bool operator == (const sFindNodeWithAttrKey& That) const
        {
            return Name == That.Name && AttrName == That.AttrName && AttrVal == That.AttrVal;
        }
        _dword Hash() const noexcept
        {
            return Name.Hash() ^ AttrName.Hash() ^ AttrVal.Hash();
        }
    };
    CMaaDList<CMaaRefDLink<CMaaXmlNodeImpl>::Impl> m_ChildsList;
    friend class CMaaDList<Attr>;
    CMaaDListAD<Attr> m_AttrsList, m_StringsList;
    static CMaaUnivHash_KeyIsClass<CMaaString, Attr*> s_hAttrs, s_hStrings;
    static CMaaUnivHash_KeyIsClass<CMaaString, NPtr> s_hNode;
    static CMaaUnivHash_KeyIsClass<CMaaString, HiddenPtrs> s_hHiddenDataPtrs;
    static CMaaUnivHash_KeyIsClass<sFindNodeWithAttrKey, CMaaRefDLink<CMaaXmlNodeImpl>::Impl*> s_hFindNodeWithAttr;

    CMaaUnivHash_KeyIsClass<CMaaString, Attr *> m_hAttrs, m_hStrings;
    CMaaUnivHash_KeyIsClass<CMaaString, NPtr> m_hNode;
    CMaaUnivHash_KeyIsClass<CMaaString, HiddenPtrs> m_hHiddenDataPtrs;
    CMaaUnivHash_KeyIsClass<sFindNodeWithAttrKey, CMaaRefDLink<CMaaXmlNodeImpl>::Impl *> * m_phFindNodeWithAttr;

    static void RemHiddenPtrProc(const CMaaString &Name, HiddenPtrs &p, void *Param);
public:
    enum
    {
        eRoot = 0,
        eProcInst,
        eDoctype,
        eElm
    };
    bool IsProcessingInstruction();
    bool IsElement();
    static CMaaXmlNode FirstChild(CMaaXmlNode & Node);
    static CMaaXmlNode NextSibling(CMaaXmlNode & Node);
    static CMaaXmlNode PrevSibling(CMaaXmlNode & Node);
    static bool IsNull(CMaaXmlNode & Node);
    CMaaXmlNodeImpl(int Type, CMaaString Name)
    :   m_Type(Type),
        m_Name(Name),
        m_hAttrs(0, &s_hAttrs, 0, 1),
        m_hStrings(0, &s_hStrings, 0, 1),
        m_hNode(0, &s_hNode, 0, 1),
        m_hHiddenDataPtrs(0, &s_hHiddenDataPtrs, 0, 1),
        //m_hFindNodeWithAttr(0, &s_hFindNodeWithAttr, 0, 1)
        m_phFindNodeWithAttr(nullptr)
    {
    }
    ~CMaaXmlNodeImpl();
    bool AddAttribute(CMaaString Name, CMaaString Value);
    bool RemoveAttribute(CMaaString Name);
    bool AddString(CMaaString Name, CMaaString Value);
    bool RemoveString(CMaaString Name);
    bool SetString(CMaaString Value); // @
    bool GetString(CMaaString &Value); // @
    int RemoveAllAttributes(); // returns number of attributes that was removed
    int RemoveAllStrings(); // returns number of strings that was removed
    bool AddProcessingInstruction(CMaaXmlNode &Node, CMaaXmlNode &Parent);
    bool AddNode(CMaaXmlNode &Node, CMaaXmlNode &Parent);
    bool RemNode(CMaaXmlNode &Node);
    void ChangeChildName(CMaaRefDLink<CMaaXmlNodeImpl>::Impl * pThat, CMaaString Name);
    CMaaString toText(int sp);
    CMaaString FindAttribute(CMaaString Name, bool *pIsFound = nullptr);
    CMaaString FindString(CMaaString Name, bool *pIsFound = nullptr);
    CMaaXmlNode FindNode(CMaaString Name, int *pN = nullptr);
    int AddHiddenPtr(CMaaString Name, void * ptr, void (*deref)(CMaaString, void *, void *) = nullptr, void * obj = nullptr, bool bDerefOld = true);
    int RemoveHiddenPtr(CMaaString Name, bool bDeref = false);
    void * GetHiddenPtr(CMaaString Name, bool *bGot = nullptr);
    int RemoveAllHiddenPtrs(bool bDeref = false);
    static bool AddBefore(CMaaXmlNode n, CMaaXmlNode x);
    static bool AddAfter(CMaaXmlNode n, CMaaXmlNode x);
    static int CopyAttributes(CMaaXmlNode &Dst, CMaaXmlNode &Src, bool bHiddenPtrs = false/*not implemented*/);
    static int CopyStrings(CMaaXmlNode &Dst, CMaaXmlNode &Src);
    void ConvertToUtf8(_dword cp);
    static int EnumAttributes(CMaaXmlNode &Node, void (*stEnumAttrFn)(CMaaString AttrName, CMaaString AttrValue, void *Param), void * Param);
    static int EnumStrings(CMaaXmlNode &Node, void (*stEnumAttrFn)(CMaaString AttrName, CMaaString AttrValue, void *Param), void * Param);
    static int SortAttributes(CMaaXmlNode &Node);
    static int SortStrings(CMaaXmlNode &Node);
    const void * FirstStringIterator()
    {
        return m_String.Length() ? (const void *)1 : m_StringsList.LookAtFront();
    }
    const void * FirstAttributeIterator()
    {
        return m_AttrsList.LookAtFront();
    }
    bool GetString(const void * Iterator, CMaaString *pName, CMaaString *pValue)
    {
        if  (Iterator == (const void *)1)
        {
            if  (m_String.Length())
            {
                if  (pName)
                {
                    *pName = "@";
                }
                if  (pValue)
                {
                    *pValue = m_String;
                }
                return true;
            }
            return false;
        }
        Attr * ptr = (Attr *)Iterator;
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
    bool GetAttribute(const void * Iterator, CMaaString *pName, CMaaString *pValue)
    {
        Attr * ptr = (Attr *)Iterator;
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
    const void * NextStringIterator(const void * Iterator)
    {
        if  (Iterator == (const void *)1)
        {
            return m_StringsList.LookAtFront();
        }
        Attr * ptr = (Attr *)Iterator;
        if  (ptr)
        {
            ptr = m_StringsList.Next(ptr);
        }
        return ptr;
    }
    const void * NextAttributeIterator(const void * Iterator)
    {
        Attr * ptr = (Attr *)Iterator;
        if  (ptr)
        {
            ptr = m_AttrsList.Next(ptr);
        }
        return ptr;
    }
private:
    ADD_ALLOCATOR(CMaaXmlNodeImpl)
};

CMaaFixedAllocator<CMaaXmlNodeImpl::Attr> * CMaaXmlNodeImpl::Attr::s_pAllocator = nullptr;
CMaaFixedAllocator<CMaaXmlNodeImpl> * CMaaXmlNodeImpl::s_pAllocator = nullptr;
template<> CMaaFixedAllocator< CMaaRefDLink<CMaaXmlNodeImpl> > * CMaaRefDLink<CMaaXmlNodeImpl>::s_pAllocator = nullptr;
template<> CMaaFixedAllocator< CMaaRefDLink<CMaaXmlNodeImpl>::Impl >* CMaaRefDLink<CMaaXmlNodeImpl>::Impl::s_pAllocator = nullptr;

CMaaUnivHash_KeyIsClass<CMaaString, CMaaXmlNodeImpl::Attr*> CMaaXmlNodeImpl::s_hAttrs(0, nullptr, 0, 1), CMaaXmlNodeImpl::s_hStrings(0, nullptr, 0, 1);
CMaaUnivHash_KeyIsClass<CMaaString, CMaaXmlNodeImpl::NPtr> CMaaXmlNodeImpl::s_hNode(0, nullptr, 0, 1);
CMaaUnivHash_KeyIsClass<CMaaString, CMaaXmlNodeImpl::HiddenPtrs> CMaaXmlNodeImpl::s_hHiddenDataPtrs(0, nullptr, 0, 1);
CMaaUnivHash_KeyIsClass<CMaaXmlNodeImpl::sFindNodeWithAttrKey, CMaaRefDLink<CMaaXmlNodeImpl>::Impl*> CMaaXmlNodeImpl::s_hFindNodeWithAttr(0, nullptr, 0, 1);

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
    CMaaRefDLink<CMaaXmlNodeImpl>::Impl* p;
    while((p = m_ChildsList.GetFromFront()))
    {
        p->m_pParent = nullptr; //-- return CMaaXmlNode(((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)m_Imp)->m_pParent);

        CMaaRefDLink<CMaaXmlNodeImpl> Ref(p);
        Ref.Decrement();
    }
    delete m_phFindNodeWithAttr;
    //printf("node '%s' is deleted\n", (const char *)m_Name);
}

CMaaXmlNode CMaaXmlNodeImpl::FirstChild(CMaaXmlNode & Node)
{
    if  (!IsNull(Node))
    {
        //CMaaXmlNodeImpl * p = /*(CMaaXmlNodeImpl *)*/ Node.m_Imp.m_Imp->m_Ptr;
        return CMaaXmlNode(Node.m_Imp->m_ChildsList.LookAtFront());
    }
    return Node;
}

CMaaXmlNode CMaaXmlNodeImpl::NextSibling(CMaaXmlNode &Node)
{
    if  (!IsNull(Node))
    {
        CMaaRefDLink<CMaaXmlNodeImpl>::Impl * p = ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent;
        if  (!p)
        {
            return nullptr;
        }
        /*
          CMaaString s;
                CMaaRefDLink<CMaaXmlNodeImpl>::Impl * x = Node.m_Imp;
          while(x && x->m_Ptr)
          {
               s += x->m_Ptr->m_Name + "\n";
               x = p->m_Ptr->m_ChildsList.Next(x);
          }
          MessageBox(nullptr, s, "", MB_OK);
          */

        //CMaaXmlNodeImpl * p = /*(CMaaXmlNodeImpl *)*/ Node.m_Imp.m_Imp->m_Ptr;
        //return CMaaXmlNode(p->m_Ptr->m_ChildsList.Next(Node.m_Imp.m_Imp));
        return CMaaXmlNode(p->m_Ptr->m_ChildsList.Next(Node.m_Imp));
    }
    return Node;
}

CMaaXmlNode CMaaXmlNodeImpl::PrevSibling(CMaaXmlNode & Node)
{
    if  (!IsNull(Node))
    {
        CMaaRefDLink<CMaaXmlNodeImpl>::Impl * p = ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent;
        if  (!p)
        {
            return nullptr;
        }
        return CMaaXmlNode(p->m_Ptr->m_ChildsList.Prev(Node.m_Imp));
    }
    return Node;
}

bool CMaaXmlNodeImpl::IsNull(CMaaXmlNode &Node)
{
    CMaaXmlNodeImpl * p = /*(CMaaXmlNodeImpl *)*/ Node.m_Imp.m_Imp->m_Ptr;
    return p == nullptr;
}



bool CMaaXmlNodeImpl::SetString(CMaaString Value)
{
    if  ((m_ChildsList.LookAtFront() || m_StringsList.LookAtFront()) && Value.Length())
    {
        return false;
    }
    m_String = Value;
    return true;
}
bool CMaaXmlNodeImpl::GetString(CMaaString &Value)
{
    Value = m_String;
    return !m_ChildsList.LookAtFront() && !m_StringsList.LookAtFront();
}

bool CMaaXmlNodeImpl::AddAttribute(CMaaString Name, CMaaString Value)
{
    Attr * a = nullptr;
    m_hAttrs.Find(Name, &a);
    if  (a)
    {
        a->SetValue(Value);
        m_AttrsList.Release(a);
        m_AttrsList.AddAtBack(a);
    }
    else
    {
        a = new Attr(Name, Value);
        if  (!a)
        {
            return false;
        }
        if  (m_hAttrs.Add(Name, a))
        {
            delete a;
            return false;
        }
        m_AttrsList.AddAtBack(a);
    }
    return true;
}

bool CMaaXmlNodeImpl::AddString(CMaaString Name, CMaaString Value)
{
    if  (Name == "@")
    {
        if  ((m_ChildsList.LookAtFront() || m_StringsList.LookAtFront()) && Value.Length())
        {
            return false;
        }
        m_String = Value;
        return true;
    }
    if  (m_String.Length())
    {
        return false;
    }

    Attr * a0 = nullptr;
    m_hStrings.Find(Name, &a0);

    if  (a0)
    {
        a0->m_Value = Value;
        return true;
    }

    Attr * a = new Attr(Name, Value);
    if  (!a)
    {
        return false;
    }
    if  (m_hStrings.AddOver(Name, a))
    {
        delete a;
        return false;
    }
    m_StringsList.AddAtBack(a);
    return true;
}

bool CMaaXmlNodeImpl::RemoveAttribute(CMaaString Name)
{
    Attr * a0 = nullptr;
    m_hAttrs.Remove(Name, &a0);
    if  (!a0)
    {
        return false;
    }
    m_AttrsList.Release(a0);
    delete a0;
    return true;
}

bool CMaaXmlNodeImpl::RemoveString(CMaaString Name)
{
    if  (Name == "@")
    {
        bool r = m_String.Length() != 0;
        m_String.Empty();
        return r;
    }
    Attr * a0 = nullptr;
    m_hStrings.Remove(Name, &a0);
    if  (!a0)
    {
        return false;
    }
    m_StringsList.Release(a0);
    delete a0;
    return true;
}

int CMaaXmlNodeImpl::RemoveAllAttributes()
{
    int n = 0;
    m_hAttrs.Cleanup();
    Attr * a0;
    while((a0 = m_AttrsList.GetFromFront()))
    {
        delete a0;
        n++;
    }
    return n;
}

int CMaaXmlNodeImpl::RemoveAllStrings()
{
    int n = 0;
    m_hStrings.Cleanup();
    Attr * a0;
    while((a0 = m_StringsList.GetFromFront()))
    {
        delete a0;
        n++;
    }
    if  (m_String.Length())
    {
        m_String.Empty();
        n++;
    }
    return n;
}

int CMaaXmlNodeImpl::AddHiddenPtr(CMaaString Name, void * ptr, void (*deref)(CMaaString, void *, void *), void * obj, bool bDeref)
{
    HiddenPtrs p;
    if  (!m_hHiddenDataPtrs.Find(Name, &p))
    {
        if  (p.m_Ptr == ptr && p.deref == deref && p.obj == obj)
        {
            return 0;
        }
    }
    RemoveHiddenPtr(Name, bDeref);
    p.m_Ptr = ptr;
    p.deref = deref;
    p.obj = obj;
    return m_hHiddenDataPtrs.Add(Name, p);
}
int CMaaXmlNodeImpl::RemoveHiddenPtr(CMaaString Name, bool bDeref)
{
    int Ret;
    HiddenPtrs p;
    if  (!(Ret = m_hHiddenDataPtrs.Remove(Name, &p)) && bDeref && p.deref)
    {
        p.deref(Name, p.m_Ptr, p.obj);
    }
    return Ret;
}
void * CMaaXmlNodeImpl::GetHiddenPtr(CMaaString Name, bool *bGot)
{
    bool b;
    HiddenPtrs p;
    if  (!m_hHiddenDataPtrs.Find(Name, &p))
    {
        b = true;
    }
    else
    {
        b = false;
        p.m_Ptr = nullptr;
    }
    if  (bGot)
    {
        *bGot = b;
    }
    return p.m_Ptr;
}
void CMaaXmlNodeImpl::RemHiddenPtrProc(const CMaaString &Name, HiddenPtrs &/*p*/, void *Param)
{
    CMaaXmlNodeImpl * x = (CMaaXmlNodeImpl *)Param;
    x->RemoveHiddenPtr(Name, true);
}
int CMaaXmlNodeImpl::RemoveAllHiddenPtrs(bool bDeref)
{
    int n = m_hHiddenDataPtrs.GetItemCount();
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

bool CMaaXmlNodeImpl::AddProcessingInstruction(CMaaXmlNode & Node, CMaaXmlNode &Parent)
{
    if  (m_String.Length())
    {
        return false;
    }
    // simple check...
    if  (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_Ptr == this)
    {
        // can't be added to itself
        return false;
    }
    // or if added...
    // if(){}

    if  (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent)
    {
        // already added
        return false;
    }
    CMaaRefDLink<CMaaXmlNodeImpl> I(Node.m_Imp);
    I.Increment();

    CMaaRefDLink<CMaaXmlNodeImpl> P(Parent.m_Imp);
    P->m_ChildsList.AddAtFront(Node.m_Imp.m_Imp);
    ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent = P;

    NPtr p;
    p.m_1stNodeImp = Node.m_Imp.m_Imp;
    p.N = 0;
    m_hNode.Find(Node.GetName(), &p);
    p.m_1stNodeImp = Node.m_Imp.m_Imp;
    p.N++;
    m_hNode.AddOver(Node.GetName(), p);

    return true;
}

bool CMaaXmlNodeImpl::AddNode(CMaaXmlNode &Node, CMaaXmlNode &Parent)
{
    if  (m_String.Length())
    {
        return false;
    }
    // simple check...
    if  (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_Ptr == this)
    {
        // can't be added to itself
        return false;
    }
    // or if added...
    // if(){}

    if  (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent)
    {
        // already added
        return false;
    }
    CMaaRefDLink<CMaaXmlNodeImpl> I(Node.m_Imp);
    I.Increment();

    CMaaRefDLink<CMaaXmlNodeImpl> P(Parent.m_Imp);
    P->m_ChildsList.AddAtBack(Node.m_Imp.m_Imp);
    ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent = P;

    NPtr p;
    p.m_1stNodeImp = Node.m_Imp.m_Imp;
    p.N = 0;
    m_hNode.Find(Node.GetName(), &p);
    p.N++;
    m_hNode.AddOver(Node.GetName(), p);

    return true;
}

int CMaaXmlNodeImpl::CopyAttributes(CMaaXmlNode &Dst, CMaaXmlNode &Src, bool /*bHiddenPtrs*/)
{
    int Count = 0;
    CMaaRefDLink<CMaaXmlNodeImpl> s(Src.m_Imp);
    for (Attr *a = s->m_AttrsList.LookAtFront(); a; a = s->m_AttrsList.Next(a))
    {
        Count++;
        Dst.AddAttribute(a->m_Name, a->m_Value);
    }
    return Count;
}

int CMaaXmlNodeImpl::CopyStrings(CMaaXmlNode &Dst, CMaaXmlNode &Src)
{
    int Count = 0;
    CMaaRefDLink<CMaaXmlNodeImpl> s(Src.m_Imp);
    Dst.SetString(s->m_String);
    for (Attr *a = s->m_StringsList.LookAtFront(); a; a = s->m_StringsList.Next(a))
    {
        Count++;
        Dst.AddString(a->m_Name, a->m_Value);
    }
    return Count;
}

int CMaaXmlNodeImpl::EnumAttributes(CMaaXmlNode &Node, void (*stEnumAttrFn)(CMaaString AttrName, CMaaString AttrValue, void *Param), void * Param)
{
    int Count = 0;
    CMaaRefDLink<CMaaXmlNodeImpl> s(Node.m_Imp);
    Attr *nxt;
    for (Attr *a = s->m_AttrsList.LookAtFront(); a; a = nxt)
    {
        nxt = s->m_AttrsList.Next(a);
        Count++;
        if  (stEnumAttrFn)
        {
            stEnumAttrFn(a->m_Name, a->m_Value, Param);
        }
    }
    return Count;
}

int CMaaXmlNodeImpl::EnumStrings(CMaaXmlNode &Node, void (*stEnumAttrFn)(CMaaString AttrName, CMaaString AttrValue, void *Param), void * Param)
{
    int Count = 0;
    CMaaRefDLink<CMaaXmlNodeImpl> s(Node.m_Imp);
    Attr *nxt;
    if  (s->m_String.Length())
    {
        Count++;
        if  (stEnumAttrFn)
        {
            stEnumAttrFn("@", s->m_String, Param);
        }
    }
    for (Attr *a = s->m_StringsList.LookAtFront(); a; a = nxt)
    {
        nxt = s->m_StringsList.Next(a);
        Count++;
        if  (stEnumAttrFn)
        {
            stEnumAttrFn(a->m_Name, a->m_Value, Param);
        }
    }
    return Count;
}

//-------------------------------------------------------------------------------------------------
static int SortFn1(const void * a1, const void * a2)
{
    CMaaXmlNodeImpl::Attr *a = *(CMaaXmlNodeImpl::Attr **)a1;
    CMaaXmlNodeImpl::Attr *b = *(CMaaXmlNodeImpl::Attr **)a2;
    return CMaaString::Compare(a->m_Name, b->m_Name);
}
//-------------------------------------------------------------------------------------------------
int CMaaXmlNodeImpl::SortAttributes(CMaaXmlNode &Node)
{
    CMaaRefDLink<CMaaXmlNodeImpl> s(Node.m_Imp);
    int Count = s->m_AttrsList.GetCount();
    CMaaPtr<Attr *> m(Count + 1, -1);
    if  (m.IsValid())
    {
        int i;
        Attr *nxt;
        Attr *a = s->m_AttrsList.LookAtFront();
        for (i = 0; i < Count && a; i++, a = nxt)
        {
            nxt = s->m_AttrsList.Next(a);
            m[i] = a;
        }
        qsort(&m[0], i, sizeof(Attr *), SortFn1);
        for (int j = i - 1; j >= 0; j--)
        {
            Attr *a = m[j];
            s->m_AttrsList.Release(a);
            s->m_AttrsList.AddAtFront(a);
        }
        return Count;
    }
    return -1;
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
int CMaaXmlNodeImpl::SortStrings(CMaaXmlNode &Node)
{
    CMaaRefDLink<CMaaXmlNodeImpl> s(Node.m_Imp);
    int Count = s->m_StringsList.GetCount();
    CMaaPtr<Attr *> m(Count + 1, -1);
    if  (m.IsValid())
    {
        int i;
        Attr *nxt;
        Attr *a = s->m_StringsList.LookAtFront();
        for (i = 0; i < Count && a; i++, a = nxt)
        {
            nxt = s->m_StringsList.Next(a);
            m[i] = a;
        }
        qsort(&m[0], i, sizeof(Attr *), SortFn1);
        for (int j = i - 1; j >= 0; j--)
        {
            Attr *a = m[j];
            s->m_StringsList.Release(a);
            s->m_StringsList.AddAtFront(a);
        }
        return Count;
    }
    return -1;
}


bool CMaaXmlNodeImpl::AddBefore(CMaaXmlNode n, CMaaXmlNode x)
{
    CMaaRefDLink<CMaaXmlNodeImpl> X(x.m_Imp);
    X.Increment();

    CMaaRefDLink<CMaaXmlNodeImpl> N(n.m_Imp);

    X.m_Imp->InsertBefore(N.m_Imp);

    CMaaRefDLink<CMaaXmlNodeImpl> P(n.GetParent().m_Imp);
    //P->m_ChildsList.AddAtBack(Node.m_Imp.m_Imp);
    ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)x.m_Imp)->m_pParent = P;

    NPtr p;
    p.m_1stNodeImp = x.m_Imp;
    p.N = 0;
    P->m_hNode.Find(n.GetName(), &p);
    p.N++;
    if  (p.m_1stNodeImp == n.m_Imp)
    {
        p.m_1stNodeImp = x.m_Imp;
    }
    P->m_hNode.AddOver(n.GetName(), p);

    return true;
}

bool CMaaXmlNodeImpl::AddAfter(CMaaXmlNode n, CMaaXmlNode x)
{
    CMaaRefDLink<CMaaXmlNodeImpl> X(x.m_Imp);
    X.Increment();

    CMaaRefDLink<CMaaXmlNodeImpl> N(n.m_Imp);

    X.m_Imp->InsertAfter(N.m_Imp);

    CMaaRefDLink<CMaaXmlNodeImpl> P(n.GetParent().m_Imp);
    //P->m_ChildsList.AddAtBack(Node.m_Imp.m_Imp);
    ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)x.m_Imp)->m_pParent = P;

    NPtr p;
    p.m_1stNodeImp = n.m_Imp;
    p.N = 0;
    P->m_hNode.Find(n.GetName(), &p);
    p.N++;
    /*
        if      (p.m_1stNodeImp == n.m_Imp.m_Imp)
        {
                p.m_1stNodeImp = x.m_Imp.m_Imp;
        }
        */
    P->m_hNode.AddOver(n.GetName(), p);

    return true;
}

void CMaaXmlNodeImpl::ChangeChildName(CMaaRefDLink<CMaaXmlNodeImpl>::Impl * pThat, CMaaString Name)
{
    if  (pThat->m_Ptr->m_Name == Name)
    {
        return;
    }
    NPtr p;
    if  (!m_hNode.Find(pThat->m_Ptr->m_Name, &p))
    {
        if  (--p.N == 0)
        {
            m_hNode.Remove(pThat->m_Ptr->m_Name);
        }
        else if (p.m_1stNodeImp != pThat)
        {
            m_hNode.AddOver(pThat->m_Ptr->m_Name, p);
        }
        else
        {
            CMaaRefDLink<CMaaXmlNodeImpl>::Impl * x = m_ChildsList.Next(pThat);
            while(x)
            {
                if  (x->m_Ptr->m_Name == pThat->m_Ptr->m_Name)
                {
                    break;
                }
                x = m_ChildsList.Next(x);
            }
            if  (!x)
            {
                // error
                m_hNode.Remove(pThat->m_Ptr->m_Name);
            }
            else
            {
                p.m_1stNodeImp = x;
                m_hNode.AddOver(pThat->m_Ptr->m_Name, p);
            }
        }
    }

    pThat->m_Ptr->m_Name = Name;
    if  (m_hNode.Find(pThat->m_Ptr->m_Name, &p))
    {
        p.N = 1;
        p.m_1stNodeImp = pThat;
        m_hNode.AddOver(pThat->m_Ptr->m_Name, p);
    }
    else
    {
        CMaaRefDLink<CMaaXmlNodeImpl>::Impl * x = p.m_1stNodeImp;
        while(x && x != pThat)
        {
            x = m_ChildsList.Next(x);
        }
        p.N++;
        if  (!x)
        {
            p.m_1stNodeImp = pThat;
        }
        m_hNode.AddOver(pThat->m_Ptr->m_Name, p);
    }
}

bool CMaaXmlNodeImpl::RemNode(CMaaXmlNode &Node)
{
    /*
     // simple check...
        if      (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_Ptr == this)
        {
                return false;
        }
     */
    // or if added...
    // if(){}

    /*
     CMaaRefDLink<CMaaXmlNodeImpl> P(Parent.m_Imp);
     //P->m_ChildsList.AddAtBack(Node.m_Imp.m_Imp);
     if   (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent != P)
     {
          return false; // wrong parent
     }
     */

    //if   (Node.m_Imp.m_Imp->m_Refs == 1)
    {
        NPtr p;
        p.m_1stNodeImp = Node.m_Imp.m_Imp;
        p.N = 0;
        m_hNode.Find(Node.GetName(), &p);
        if  (p.N <= 1)
        {
            m_hNode.Remove(Node.GetName());
        }
        else
        {
            p.N--;
            if  (p.m_1stNodeImp == Node.m_Imp.m_Imp)
            {
                CMaaXmlNode n = Node.FindNext();
                p.m_1stNodeImp = n.m_Imp.m_Imp;
            }
            m_hNode.AddOver(Node.GetName(), p);
        }
        m_ChildsList.Release(Node.m_Imp.m_Imp);

    }
    ((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent = nullptr;
    CMaaRefDLink<CMaaXmlNodeImpl> Ref(Node.m_Imp);
    Ref.Decrement();
    if  (!Ref.m_Imp)
    {
        // m_Refs == 0 && object deleted
    }
    return true;
}

CMaaString CMaaXmlNodeImpl::FindAttribute(CMaaString Name, bool *pIsFound)
{
    CMaaString Ret;
    Attr * a = nullptr;
    m_hAttrs.Find(Name, &a);
    bool f = false;
    if  (a)
    {
        f = true;
        Ret = a->m_Value;
    }
    if  (pIsFound)
    {
        *pIsFound = f;
    }
    return Ret;
}

CMaaString CMaaXmlNodeImpl::FindString(CMaaString Name, bool *pIsFound)
{
    if  (Name == "@")
    {
        if  (pIsFound)
        {
            *pIsFound = m_String.Length() != 0;
        }
        return m_String;
    }
    CMaaString Ret;
    Attr * a = nullptr;
    m_hStrings.Find(Name, &a);
    bool f = false;
    if  (a)
    {
        f = true;
        Ret = a->m_Value;
    }
    if  (pIsFound)
    {
        *pIsFound = f;
    }
    return Ret;
}

CMaaXmlNode CMaaXmlNodeImpl::FindNode(CMaaString Name, int *pN)
{
    NPtr p;
    if  (m_hNode.Find(Name, &p))
    {
        if  (pN)
        {
            *pN = 0;
        }
        return CMaaXmlNode(nullptr);
    }
    if  (pN)
    {
        *pN = p.N;
    }
    return CMaaXmlNode(p.m_1stNodeImp);
}

//-------------------------------------------------------------------------------------------------
// CMaaXmlNode
//-------------------------------------------------------------------------------------------------
CMaaXmlNode::CMaaXmlNode(int Type, CMaaString Name)
:   m_Imp(new CMaaXmlNodeImpl(Type, Name))
{
}
CMaaXmlNode::CMaaXmlNode(const CMaaXmlNode & That)
:   m_Imp(That.m_Imp)
{
}
CMaaXmlNode & CMaaXmlNode::operator=(const CMaaXmlNode & That)
{
    m_Imp = That.m_Imp;
    return *this;
}

bool CMaaXmlNode::AddProcessingInstruction(CMaaXmlNode & Node)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->AddProcessingInstruction(Node, *this);
}
bool CMaaXmlNode::AddNode(CMaaXmlNode &Node)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->AddNode(Node, *this);
}
bool CMaaXmlNode::RemNode(CMaaXmlNode &Node)
{
    if  (IsNull())
    {
        return false;
    }
    CMaaRefDLink<CMaaXmlNodeImpl> P(m_Imp);
    if  (((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)Node.m_Imp)->m_pParent != P)
    {
        return false; // wrong parent
    }
    return m_Imp->RemNode(Node);
}

bool CMaaXmlNode::AddAttribute(CMaaString Name, CMaaString Value)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->AddAttribute(Name, Value);
}


bool CMaaXmlNode::SetString(CMaaString Value)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->SetString(Value);
}
bool CMaaXmlNode::AddString(CMaaString Name, CMaaString Value)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->AddString(Name, Value);
}
bool CMaaXmlNode::RemoveAttribute(CMaaString Name)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->RemoveAttribute(Name);
}
bool CMaaXmlNode::RemoveString(CMaaString Name)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->RemoveString(Name);
}
int CMaaXmlNode::RemoveAllAttributes()
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->RemoveAllAttributes();
}
int CMaaXmlNode::RemoveAllStrings()
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->RemoveAllStrings();
}
CMaaXmlNode::CMaaXmlNode(CMaaRefDLink<CMaaXmlNodeImpl>::Impl * I)
:   m_Imp(I)
{
}
CMaaXmlHandle CMaaXmlNode::GetHandle()
{
    return (CMaaXmlHandle)m_Imp;
}
/*
CMaaXmlNode::operator CMaaXmlHandle ()
{
     return GetHandle();
}
*/
CMaaXmlNode::~CMaaXmlNode()
{
}
bool CMaaXmlNode::IsProcessingInstruction()
{
    // null?
    return m_Imp.m_Imp && m_Imp->m_Type == CMaaXmlNodeImpl::eProcInst;
}
bool CMaaXmlNode::IsElement()
{
    // null?
    return m_Imp.m_Imp && m_Imp->m_Type == CMaaXmlNodeImpl::eElm;
}
CMaaXmlNode CMaaXmlNode::FirstChild()
{
    return CMaaXmlNodeImpl::FirstChild(*this);
}
CMaaXmlNode CMaaXmlNode::NextSibling()
{
    return CMaaXmlNodeImpl::NextSibling(*this);
}
bool CMaaXmlNode::IsNull()
{
    return CMaaXmlNodeImpl::IsNull(*this);
}
CMaaXmlNode CMaaXmlNode::FindNext()
{
    if  (IsNull())
    {
        return *this;
    }
    CMaaString Name = GetName();
    CMaaXmlNode Ret = NextSibling();
    while(!Ret.IsNull() && Ret.GetName() != Name)
    {
        Ret = Ret.NextSibling();
    }
    return Ret;
}

CMaaString CMaaXmlNode::FindAttribute(CMaaString Name, bool *pIsFound)
{
    if  (IsNull())
    {
        if  (pIsFound)
        {
            *pIsFound = false;
        }
        return CMaaString();
    }
    return m_Imp->FindAttribute(Name, pIsFound);
}

CMaaString CMaaXmlNode::FindString(CMaaString Name, bool *pIsFound)
{
    if  (IsNull())
    {
        if  (pIsFound)
        {
            *pIsFound = false;
        }
        return CMaaString();
    }
    return m_Imp->FindString(Name, pIsFound);
}
CMaaString CMaaXmlNode::GetString(bool *pIsFound)
{
    return FindString("@", pIsFound);
}

const void * CMaaXmlNode::FirstStringIterator()
{
    if  (IsNull())
    {
        return nullptr;
    }
    return m_Imp->FirstStringIterator();
}
const void * CMaaXmlNode::FirstAttributeIterator()
{
    if  (IsNull())
    {
        return nullptr;
    }
    return m_Imp->FirstAttributeIterator();
}
bool CMaaXmlNode::GetString(const void * Iterator, CMaaString *pName, CMaaString *pValue)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->GetString(Iterator, pName, pValue);
}
bool CMaaXmlNode::GetAttribute(const void * Iterator, CMaaString *pName, CMaaString *pValue)
{
    if  (IsNull())
    {
        return false;
    }
    return m_Imp->GetAttribute(Iterator, pName, pValue);
}
const void * CMaaXmlNode::NextStringIterator(const void * Iterator)
{
    if  (IsNull())
    {
        return nullptr;
    }
    return m_Imp->NextStringIterator(Iterator);
}
const void * CMaaXmlNode::NextAttributeIterator(const void * Iterator)
{
    if  (IsNull())
    {
        return nullptr;
    }
    return m_Imp->NextAttributeIterator(Iterator);
}

CMaaXmlNode CMaaXmlNode::FindNode(CMaaString Name, int *pN)
{
    if  (IsNull())
    {
        if  (pN)
        {
            *pN = 0;
        }
        return *this;
    }
    return m_Imp->FindNode(Name, pN);
}

bool CMaaXmlNode::AppendChild(CMaaXmlNode & Node)
{
    return AddNode(Node);
}
bool CMaaXmlNode::RemoveChild(CMaaXmlNode & Node)
{
    return RemNode(Node);
}
bool CMaaXmlNode::Remove()
{
    CMaaXmlNode Parent = GetParent();
    if  (Parent.IsNull())
    {
        return false;
    }
    return Parent.RemNode(*this);
}
CMaaString CMaaXmlNode::GetName()
{
    if  (!IsNull())
    {
        return m_Imp->m_Name;
    }
    CMaaString Empty;
    return Empty;
}
CMaaXmlNode CMaaXmlNode::GetParent()
{
    if  (IsNull())
    {
        return *this;
    }
    return CMaaXmlNode(((CMaaRefDLink<CMaaXmlNodeImpl>::Impl *)m_Imp)->m_pParent);
}
void CMaaXmlNode::ChangeName(CMaaString Name)
{
    if  (IsNull() || GetName() == Name)
    {
        return;
    }
    CMaaXmlNode Parent = GetParent();
    if  (Parent.IsNull())
    {
        m_Imp->m_Name = Name;
        return;
    }
    Parent.m_Imp->ChangeChildName(m_Imp, Name);
}
int CMaaXmlNode::AddHiddenPtr(CMaaString Name, void * ptr, void (*deref)(CMaaString, void *, void *), void * obj, bool bDerefOld)
{
    if  (IsNull())
    {
        return -1;
    }
    return m_Imp->AddHiddenPtr(Name, ptr, deref, obj, bDerefOld);
}
int CMaaXmlNode::RemoveHiddenPtr(CMaaString Name, bool bDeref)
{
    if  (IsNull())
    {
        return -1;
    }
    return m_Imp->RemoveHiddenPtr(Name, bDeref);
}
void * CMaaXmlNode::GetHiddenPtr(CMaaString Name, bool *bGot)
{
    if  (IsNull())
    {
        if  (bGot)
        {
            *bGot = false;
        }
        return nullptr;
    }
    return m_Imp->GetHiddenPtr(Name, bGot);
}
int CMaaXmlNode::RemoveAllHiddenPtrs(bool bDeref)
{
    if  (IsNull())
    {
        return 0;
    }
    return m_Imp->RemoveAllHiddenPtrs(bDeref);
}
int CMaaXmlNode::AddHiddenInt(CMaaString Name, int x)
{
    //return AddHiddenPtr(Name, (void*)x);
    return AddHiddenPtr(Name, reinterpret_cast<void*>(static_cast<long long>(x)));
}
int CMaaXmlNode::GetHiddenInt(CMaaString Name, int Default, bool *bGot)
{
    bool b;
    if  (!bGot)
    {
        bGot = &b;
    }
    //int r = (int)(long)(GetHiddenPtr(Name, bGot);
    int r = static_cast<int>(reinterpret_cast<long long>(GetHiddenPtr(Name, bGot)));
    if  (!*bGot)
    {
        r = Default;
    }
    return r;
}
static void deref_string(CMaaString, void* ptr, void* /*obj*/)
{
    delete (CMaaString*)ptr;
}
int CMaaXmlNode::AddHiddenString(CMaaString Name, CMaaString Str)
{
    CMaaString* ptr = nullptr;
    int R = -1;
    try
    {
        ptr = new CMaaString(Str);
        if  (!(R = AddHiddenPtr(Name, ptr, deref_string)))
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
CMaaString CMaaXmlNode::GetHiddenString(CMaaString Name, bool* bGot)
{
    bool b;
    bGot = bGot ? bGot : &b;
    CMaaString* ptr = (CMaaString*)GetHiddenPtr(Name, bGot);
    if  (*bGot)
    {
        return *ptr;
    }
    return CMaaStringZ;
}

MAA_STATIC_FLAG sFlag_EncodeAttr;
void CMaaXmlNode::EncodeAttr(CMaaConcatString &c, CMaaString s, int iFeatures)
{
    int i0 = 0;
    int l = s.Length();
    const char * p0 = s;
    const char * p = p0;
    CMaaString Ret;
    CMaaSafeMakeStatic msms(&sFlag_EncodeAttr);
    msms.Make();
    static CMaaString r[7] =
    {
        gStaticStringsAllocator.Alloc("&lt;"),
        gStaticStringsAllocator.Alloc("&quot;"),
        gStaticStringsAllocator.Alloc("&amp;"),
        gStaticStringsAllocator.Alloc("&gt;"),
        gStaticStringsAllocator.Alloc("&#13;"),
        gStaticStringsAllocator.Alloc("&#10;"),
        gStaticStringsAllocator.Alloc("&#0;")
    };
    msms.Done();
    for (int i = 0; i < l; i++)
    {
        int j = -1;
        switch(p[i])
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
        case '>':
            if  ((int)(p - p0) - i0 >= 2)
            {
                if  (p[i-1] == ']' && p[i-2] == ']')
                {
                    j = 3;
                }
            }
            break;
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
            c.Add(i0 + (const char *)s, i - i0);
            c += r[j];
            i0 = i + 1;
        }
    }
    c.Add(i0 + (const char *)s, s.Length() - i0);
}

CMaaString CMaaXmlNode::EncodeAttr(CMaaString s, int iFeatures)
{
    CMaaConcatString c(1024, -1);
    EncodeAttr(c, s, iFeatures);
    return c;
}

MAA_STATIC_FLAG sFlag_DecodeAttr;
CMaaString CMaaXmlNode::DecodeAttr(CMaaString s, int cp) // 2020: cp = 0/1251 || e_utf8 for Unicode &#xHEX, &#DEC
{
    int i0 = 0;
    int l = s.Length();
    const char * p0 = s;
    const char * p = p0;
    CMaaConcatString Ret(l);
    //static CMaaString r[5] = { "&lt;", "&quot;", "&amp;", "&gt;", "&#" };
    CMaaSafeMakeStatic msms(&sFlag_DecodeAttr);
    msms.Make();
    static CMaaString r[6] =
    {
        gStaticStringsAllocator.Alloc("&lt;"),
        gStaticStringsAllocator.Alloc("&quot;"),
        gStaticStringsAllocator.Alloc("&amp;"),
        gStaticStringsAllocator.Alloc("&gt;"),
        gStaticStringsAllocator.Alloc("&#x"),
        gStaticStringsAllocator.Alloc("&#")
    };
    msms.Done();
    char ch[6] = { '<', '"', '&', '>', '\0', '\0' };
    for (int i = 0; i < l; )
    {
        if  (p[i] == '&')
        {
            int j;
            for (j = 0; j < 6; j++)
            {
                if  (s.Mid(i, r[j].Length()) == r[j])
                {
                    if  (j == 4)
                    {
                        _WC_ n = 0;
                        int k;
                        for (k = 0; k < 4; k++)
                        {
                            if  (p0[i + 3 + k] >= '0' && p0[i + 3 + k] <= '9')
                            {
                                n = n * 0x10 + (int)(p0[i + 3 + k] - '0');
                            }
                            else if (p0[i + 3 + k] >= 'a' && p0[i + 3 + k] <= 'f')
                            {
                                n = n * 0x10 + (int)(p0[i + 3 + k] - 'a' + 10);
                            }
                            else if (p0[i + 3 + k] >= 'A' && p0[i + 3 + k] <= 'F')
                            {
                                n = n * 0x10 + (int)(p0[i + 3 + k] - 'A' + 10);
                            }
                            else
                            {
                                break;
                            }
                        }
                        if  (k > 0 && p0[i + 3 + k] == ';')
                        {
                            Ret.Add(i0 + (const char *)s, i - i0);
                            if  (!cp || cp == 1251)
                            {
                                char c = n < 0x100 ? (char)n : '?';
                                Ret.Add(&c, 1);
                            }
                            else
                            {
                                CMaaString c = UnicodeToUtf8(CMaaString((const char *)&n, 2));
                                Ret += c;
                            }

                            i += 4 + k;
                            i0 = i;
                            break;
                        }
                    }
                    else if (j == 5)
                    {
                        _WC_ n = 0;
                        int k;
                        for (k = 0; k < 5; k++)
                        {
                            if  (p0[i + 2 + k] >= '0' && p0[i + 2 + k] <= '9')
                            {
                                n = n * 10 + (int)(p0[i + 2 + k] - '0');
                            }
                            else
                            {
                                break;
                            }
                        }
                        if  (k > 0 && p0[i + 2 + k] == ';')
                        {
                            Ret.Add(i0 + (const char *)s, i - i0);
                            if  (!cp || cp == 1251)
                            {
                                char c = n < 0x100 ? (char)n : '?';
                                Ret.Add(&c, 1);
                            }
                            else
                            {
                                CMaaString c = UnicodeToUtf8(CMaaString((const char *)&n, 2));
                                Ret += c;
                            }

                            i += 3 + k;
                            i0 = i;
                            break;
                        }
                    }
                    else
                    {
                        Ret.Add(i0 + (const char *)s, i - i0);
                        Ret.Add(&ch[j], 1);

                        i += r[j].Length();
                        i0 = i;
                        break;
                    }
                }
            }
            if  (j < 6)
            {
                continue;
            }
        }
        i++;
    }
    Ret.Add(i0 + (const char *)s, l - i0);
    return Ret;
}

CMaaString CMaaXmlNode::ToString(CMaaString Sp0, CMaaString DeltaSp, CMaaString EndOfLine, int iFeatures)
{
    if  (IsNull())
    {
        return "";
    }
    CMaaString Sp1 = Sp0 + DeltaSp;
    CMaaString s, e;
    s = Sp0;
    if  (IsProcessingInstruction())
    {
        s = "<?xml";
        if  (m_Imp->m_Name != "")
        {
            s = CMaaString("<?") + m_Imp->m_Name;
        }
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            s += CMaaString::sFormat(" %S=\"%S\"", &p->m_Name, &p->m_Value);
        }
        s += "?>";
    }
    else
    {
        s.Format("%S<%S", &Sp0, &m_Imp->m_Name);
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            CMaaString v = EncodeAttr(p->m_Value, iFeatures);
            s += CMaaString::sFormat(" %S=\"%S\"", &p->m_Name, &v);
        }
        CMaaXmlNode n = FirstChild();
        if  (n.IsNull() && !m_Imp->m_StringsList.LookAtFront() && !m_Imp->m_String.Length())
        {
            if  (m_Imp->m_AttrsList.LookAtFront())
            {
                s += " />";
            }
            else
            {
                s += "/>";
            }
        }
        else if (m_Imp->m_String.Length())
        {
            if  (m_Imp->m_AttrsList.LookAtFront())
            {
                s += CMaaString(" >");
            }
            else
            {
                s += CMaaString(">");
            }
            CMaaString v = EncodeString(m_Imp->m_String, iFeatures, m_Imp->m_Name);
            s += v;
            s += CMaaString::sFormat("</%S>", &m_Imp->m_Name);
        }
        else
        {
            if  (m_Imp->m_AttrsList.LookAtFront())
            {
                s += CMaaString(" >") + EndOfLine;
            }
            else
            {
                s += CMaaString(">") + EndOfLine;
            }
            for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_StringsList.LookAtFront(); p; p = m_Imp->m_StringsList.Next(p))
            {
                if  (p->m_Value.Length() == 0)
                {
                    if  (p->m_Name == "")
                    {
                        static int aa = 0;
                        aa++;
                        aa++;
                        aa++;
                    }
                    s += Sp1 + "<" + p->m_Name + "/>" + EndOfLine;
                }
                else
                {
                    CMaaString v = EncodeString(p->m_Value, iFeatures, p->m_Name);
                    s += Sp1 + "<" + p->m_Name + ">" + v + "</" + p->m_Name + ">" + EndOfLine;
                }
            }
            while(!n.IsNull())
            {
                e = n.ToString(Sp1, DeltaSp, EndOfLine, iFeatures);
                s += e + EndOfLine;
                n = n.NextSibling();
            }
            s += CMaaString::sFormat("%S</%S>", &Sp0, &m_Imp->m_Name);
        }
    }
    return s;
}

void CMaaXmlNode::ToString(CMaaConcatString &s, CMaaString Sp0, CMaaString DeltaSp, CMaaString EndOfLine, int iFeatures)
{
    if  (IsNull())
    {
        return;
    }
    CMaaString Sp1 = Sp0 + DeltaSp;
    CMaaString e;
    s += Sp0;
    if  (IsProcessingInstruction())
    {
        if  (m_Imp->m_Name != "")
        {
            s += CMaaString("<?") + m_Imp->m_Name;
        }
        else
        {
            s += "<?xml";
        }
        for (CMaaXmlNodeImpl::Attr * p = m_Imp->m_AttrsList.LookAtFront(); p; p = m_Imp->m_AttrsList.Next(p))
        {
            s += CMaaString::sFormat(" %S=\"%S\"", &p->m_Name, &p->m_Value);
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
        if  (n.IsNull() && !m_Imp->m_StringsList.LookAtFront() && !m_Imp->m_String.Length())
        {
            if  (m_Imp->m_AttrsList.LookAtFront())
            {
                s += " />";
            }
            else
            {
                s += "/>";
            }
        }
        else if (m_Imp->m_String.Length())
        {
            if  (m_Imp->m_AttrsList.LookAtFront())
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
        else
        {
            if  (m_Imp->m_AttrsList.LookAtFront())
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
                if  (p->m_Value.Length() == 0)
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
            while(!n.IsNull())
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

CMaaXmlNode CMaaXmlNode::FindNodeWithAttr(CMaaString Name, CMaaString AttrName, CMaaString AttrVal, CMaaXmlNode SkipNode)
{
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        if  (n.GetHandle() != SkipNode.GetHandle())
        {
            bool IsFound = false;
            if  (n.FindAttribute(AttrName, &IsFound) == AttrVal && IsFound)
            {
                return n;
            }
        }
    }
    return n;
}

CMaaXmlNode CMaaXmlNode::FindNodeWithAttrRO(CMaaMutex* mtx, CMaaString Name, CMaaString AttrName, CMaaString AttrVal, CMaaXmlNode SkipNode, bool bClean)
{
    if  (IsNull())
    {
        return *this;
    }
    if  (mtx)
    {
        mtx->LockM();
    }
    if  (!m_Imp->m_phFindNodeWithAttr)
    {
        try
        {
            m_Imp->m_phFindNodeWithAttr = new CMaaUnivHash_KeyIsClass<CMaaXmlNodeImpl::sFindNodeWithAttrKey, CMaaRefDLink<CMaaXmlNodeImpl>::Impl*>(0, &CMaaXmlNodeImpl::s_hFindNodeWithAttr, 0, 1);
        }
        catch(...)
        {
        }
        if  (!m_Imp->m_phFindNodeWithAttr)
        {
            if  (mtx)
            {
                mtx->UnLockM();
            }
            return CMaaXmlNode(nullptr);
        }
    }
    if  (bClean)
    {
        m_Imp->m_phFindNodeWithAttr->Cleanup();
        if  (mtx)
        {
            mtx->UnLockM();
        }
        return CMaaXmlNode(nullptr);
    }
    CMaaXmlNodeImpl::sFindNodeWithAttrKey k;
    k.Name = Name;
    k.AttrName = AttrName;
    k.AttrVal = AttrVal;
    CMaaRefDLink<CMaaXmlNodeImpl>::Impl * p = nullptr;
    if  (!m_Imp->m_phFindNodeWithAttr->Find(k, &p))
    {
        if  (mtx)
        {
            mtx->UnLockM();
        }
        CMaaXmlNode n = CMaaXmlNode(p);
        if  (n.GetHandle() == SkipNode.GetHandle())
        {
            for (; !n.IsNull(); n = n.FindNext())
            {
                //if (n.GetHandle() != SkipNode.GetHandle())
                {
                    bool IsFound = false;
                    if  (n.FindAttribute(AttrName, &IsFound) == AttrVal && IsFound)
                    {
                        return n;
                    }
                }
            }
        }
        return n;
    }
    if  (mtx)
    {
        mtx->UnLockM();
    }
    bool b1st = true;
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        bool IsFound = false;
        if  (n.FindAttribute(AttrName, &IsFound) == AttrVal && IsFound)
        {
            if  (b1st)
            {
                b1st = false;
                if  (mtx)
                {
                    mtx->LockM();
                }
                m_Imp->m_phFindNodeWithAttr->Add(k, n.GetHandle());
                if  (mtx)
                {
                    mtx->UnLockM();
                }
            }
            if  (n.GetHandle() != SkipNode.GetHandle())
            {
                return n;
            }
        }
    }
    if  (b1st)
    {
        if  (mtx)
        {
            mtx->LockM();
        }
        m_Imp->m_phFindNodeWithAttr->Add(k, nullptr);
        if  (mtx)
        {
            mtx->UnLockM();
        }
    }
    return n;
}

CMaaXmlNode CMaaXmlNode::FindNodeWithString(CMaaString Name, CMaaString StringName, CMaaString StringVal, CMaaXmlNode SkipNode)
{
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        if  (n.GetHandle() != SkipNode.GetHandle())
        {
            bool IsFound = false;
            if  (n.FindString(StringName, &IsFound) == StringVal && IsFound)
            {
                return n;
            }
        }
    }
    return n;
}

int CMaaXmlNode::ChangeNodesAttr(CMaaString Name, CMaaString AttrName, CMaaString OldVal, CMaaString NewVal)
{
    int N = 0;
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        bool IsFound = false;
        if  (n.FindAttribute(AttrName, &IsFound) == OldVal && IsFound)
        {
            n.AddAttribute(AttrName, NewVal);
            N++;
        }
    }
    return N;
}

int CMaaXmlNode::ChangeNodesString(CMaaString Name, CMaaString StringName, CMaaString OldVal, CMaaString NewVal)
{
    int N = 0;
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        bool IsFound = false;
        if  (n.FindAttribute(StringName, &IsFound) == OldVal && IsFound)
        {
            n.AddString(StringName, NewVal);
            N++;
        }
    }
    return N;
}

int CMaaXmlNode::RemoveNodesAttr(CMaaString Name, CMaaString AttrName, CMaaString Val)
{
    int N = 0;
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        bool IsFound = false;
        if  (n.FindAttribute(AttrName, &IsFound) == Val && IsFound)
        {
            n.RemoveAttribute(AttrName);
            N++;
        }
    }
    return N;
}

int CMaaXmlNode::RemoveNodesString(CMaaString Name, CMaaString StringName, CMaaString Val)
{
    int N = 0;
    CMaaXmlNode n = FindNode(Name);
    for (; !n.IsNull(); n = n.FindNext())
    {
        bool IsFound = false;
        if  (n.FindString(StringName, &IsFound) == Val && IsFound)
        {
            n.RemoveString(StringName);
            N++;
        }
    }
    return N;
}

int CMaaXmlNode::RemoveNodesWithAttr(CMaaString Name, CMaaString AttrName, CMaaString Val)
{
    int N = 0;
    CMaaXmlNode n = FindNode(Name);
    CMaaXmlNode nn = n;
    for (; !n.IsNull(); n = nn)
    {
        nn = n.FindNext();
        bool IsFound = false;
        if  (n.FindAttribute(AttrName, &IsFound) == Val && IsFound)
        {
            n.Remove();
            //n.RemoveAttribute(AttrName);
            N++;
        }
    }
    return N;
}

int CMaaXmlNode::RemoveNodesWithString(CMaaString Name, CMaaString StringName, CMaaString Val)
{
    int N = 0;
    CMaaXmlNode n = FindNode(Name);
    CMaaXmlNode nn = n;
    for (; !n.IsNull(); n = nn)
    {
        nn = n.FindNext();
        bool IsFound = false;
        if  (n.FindAttribute(StringName, &IsFound) == Val && IsFound)
        {
            n.Remove();
            //n.RemoveAttribute(AttrName);
            N++;
        }
    }
    return N;
}

CMaaXmlNode CMaaXmlNode::PrevSibling()
{
    return CMaaXmlNodeImpl::PrevSibling(*this);
}

CMaaXmlNode CMaaXmlNode::FindPrev()
{
    if  (IsNull())
    {
        return *this;
    }
    CMaaString Name = GetName();
    CMaaXmlNode Ret = PrevSibling();
    while(!Ret.IsNull() && Ret.GetName() != Name)
    {
        Ret = Ret.PrevSibling();
    }
    return Ret;
}

bool CMaaXmlNode::InsertBefore(CMaaXmlNode p)
{
    if  (p.IsNull() || IsNull() /*|| p.GetParent().IsNull()!!2018!!*/ || GetHandle() == p.GetHandle() /*!!2018!!*/ || p.GetName() != GetName())
    {
        return false;
    }
    Remove();
    return AddBefore(p);
}

bool CMaaXmlNode::InsertAfter(CMaaXmlNode p)
{
    if  (p.IsNull() || IsNull() || GetHandle() == p.GetHandle() || p.GetName() != GetName())
    {
        return false;
    }
    Remove();
    return AddAfter(p);
}

bool CMaaXmlNode::MoveUp()
{
    CMaaXmlNode p = FindPrev();
    if  (!p.IsNull() && !IsNull() && p.GetName() == GetName())
    {
        Remove();
        return AddBefore(p);
    }
    return false;
}

bool CMaaXmlNode::MoveDown()
{
    CMaaXmlNode p = FindNext();
    if  (!p.IsNull() && !IsNull() && p.GetName() == GetName())
    {
        Remove();
        return AddAfter(p);
    }
    return false;
}

bool CMaaXmlNode::AddBefore(CMaaXmlNode n)
{
    if  (GetParent().IsNull() && !n.IsNull() && GetName() == n.GetName())
    {
        return CMaaXmlNodeImpl::AddBefore(n, *this);
    }
    return false;
}

bool CMaaXmlNode::AddAfter(CMaaXmlNode n)
{
    if  (GetParent().IsNull() && !n.IsNull() && GetName() == n.GetName())
    {
        return CMaaXmlNodeImpl::AddAfter(n, *this);
    }
    return false;
}

void CMaaXmlNodeImpl::ConvertToUtf8(_dword cp)
{
    /*
     if   (!m_Imp)//.m_Imp)
     {
          return;// CMaaXmlNode(nullptr);
     }
     */

    int aCount = 0;
    for (Attr *a = /*s->*/m_AttrsList.LookAtFront(); a; a = /*s->*/m_AttrsList.Next(a))
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
    if  (m_String.Length())
    {
        m_String = UnicodeToUtf8(AnsiToUnicode(m_String, false, cp));
    }
}

CMaaXmlNode CMaaXmlNode::CopyNode(CMaaXmlDocument &doc, bool bChilds, bool bHiddenPtrs)
{
    if  (!m_Imp.m_Imp)
    {
        return CMaaXmlNode(nullptr);
    }

    CMaaXmlNode n = CMaaXmlNode(m_Imp->m_Type, GetName());
    CMaaXmlNodeImpl::CopyAttributes(n, *this, bHiddenPtrs);
    CMaaXmlNodeImpl::CopyStrings(n, *this);
    if  (bChilds)
    {
        CMaaXmlNode c = FirstChild();
        for (; !c.IsNull(); c = c.NextSibling())
        {
            CMaaXmlNode cc = c.CopyNode(doc, bChilds, bHiddenPtrs);
            n.AppendChild(cc);
        }
    }
    return n;
}

int CMaaXmlNode::CopyAttributes(CMaaXmlNode From, bool bHiddenPtrs)
{
    if  (GetHandle() != From.GetHandle())
    {
        return CMaaXmlNodeImpl::CopyAttributes(*this, From, bHiddenPtrs);
    }
    return 0;
    //CMaaXmlNodeImpl::CopyStrings(n, *this);
}

int CMaaXmlNode::EnumAttributes(EnumAttributesFn Fn, void * Param)
{
    if  (IsNull())
    {
        return 0;
    }
    strEnum e(Fn, *this, Param);
    return CMaaXmlNodeImpl::EnumAttributes(*this, stEnumAttrFn, &e);
}

int CMaaXmlNode::EnumStrings(EnumStringsFn Fn, void * Param)
{
    if  (IsNull())
    {
        return 0;
    }
    strEnum e(Fn, *this, Param);
    return CMaaXmlNodeImpl::EnumStrings(*this, stEnumAttrFn, &e);
}

int CMaaXmlNode::SortAttributes()
{
    if  (IsNull())
    {
        return 0;
    }
    return CMaaXmlNodeImpl::SortAttributes(*this);
}

int CMaaXmlNode::SortStrings()
{
    if  (IsNull())
    {
        return 0;
    }
    return CMaaXmlNodeImpl::SortStrings(*this);
}

void CMaaXmlNode::Sort(int Flags)
{
    if  (IsNull())
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
        for (; !c.IsNull(); c = c.NextSibling())
        {
            c.Sort(Flags);
        }
    }
}


//-------------------------------------------------------------------------------------------------
// CMaaXmlProcessingInstruction
//-------------------------------------------------------------------------------------------------
CMaaXmlProcessingInstruction::CMaaXmlProcessingInstruction(CMaaString Name)
:   CMaaXmlNode(CMaaXmlNodeImpl::eProcInst, Name)
{
}

CMaaXmlProcessingInstruction::CMaaXmlProcessingInstruction(CMaaXmlNode &Node)
:   CMaaXmlNode(Node)
{

}

bool CMaaXmlProcessingInstruction::AddInstruction(int Type, CMaaString Value)
{
    CMaaString Name;
    if  (Type == eVersion)
    {
        Name = "version";
        if  (Value == "")
        {
            Value = "1.0";
        }
    }
    else if (Type == eEncoding)
    {
        Name = "encoding";
        if  (Value == "")
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

bool CMaaXmlProcessingInstruction::AddInstruction(CMaaString Name, CMaaString Value)
{
    return CMaaXmlNode::AddAttribute(Name, Value);
}

//-------------------------------------------------------------------------------------------------
// CMaaXmlElement
//-------------------------------------------------------------------------------------------------
CMaaXmlElement::CMaaXmlElement(CMaaString Name)
:   CMaaXmlNode(CMaaXmlNodeImpl::eElm, Name)
{
}

CMaaXmlElement::CMaaXmlElement(CMaaXmlNode &Node)
:   CMaaXmlNode(Node)
{
}

bool CMaaXmlElement::SetAttribute(CMaaString Name, CMaaString Value)
{
    return CMaaXmlNode::AddAttribute(Name, Value);
}

bool CMaaXmlElement::AppendChild(CMaaXmlNode &Node)
{
    return CMaaXmlNode::AddNode(Node);
}

//-------------------------------------------------------------------------------------------------
// CMaaXmlElement
//-------------------------------------------------------------------------------------------------
CMaaXmlDocument::CMaaXmlDocument(CMaaString Name)
:   m_Name(Name),
    m_Root(0, "")
{
}

CMaaXmlDocument::CMaaXmlDocument(CMaaString Name, CMaaString RootName)
:   m_Name(Name),
    m_Root(0, "")
{
    CMaaXmlElement Root = CreateElement(RootName);
    AppendChild(Root);
}

//-------------------------------------------------------------------------------------------------
CMaaXmlDocument::CMaaXmlDocument(CMaaXmlDocument &That, bool bCopy)
:   m_Name(That.m_Name),
    m_Root(0, "")
{
    if  (!bCopy)
    {
        *this = That;
    }
    else
    {
        CMaaXmlNode NewRoot = That.m_Root.CopyNode(*this);
        m_Root = NewRoot;
    }
}

CMaaXmlDocument::~CMaaXmlDocument()
{
}

CMaaXmlElement CMaaXmlDocument::CreateElement(CMaaString Name)
{
    return CMaaXmlElement(Name);
}

bool CMaaXmlDocument::AddProcessingInstruction(CMaaXmlNode &Node)
{
    CMaaXmlNode pi = FirstChild();
    for (; !pi.IsNull(); )
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

void CMaaXmlDocument::AppendChild(CMaaXmlNode &Node)
{
    m_Root.AppendChild(Node);
}

CMaaXmlNode CMaaXmlDocument::FirstChild()
{
    return m_Root.FirstChild();
}

CMaaXmlElement CMaaXmlDocument::DocumentElement()
{
    CMaaXmlNode n = FirstChild();
    while(!n.IsNull())
    {
        if  (n.IsElement())
        {
            //return n;
            break;
        }
        n = n.NextSibling();
    }
    return n;
}

CMaaXmlProcessingInstruction CMaaXmlDocument::CreateProcessingInstruction(CMaaString xml)
{
    return CMaaXmlProcessingInstruction(xml);
}

CMaaString CMaaXmlDocument::ToString(CMaaString Sp0, CMaaString DeltaSp, CMaaString EndOfLine, int iFeatures)
{
    //CMaaString Sp0;
    //CMaaString DeltaSp = " ";

    CMaaString s, e;
    CMaaXmlNode n = FirstChild();
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
    for (; !n.IsNull(); n = n.NextSibling())
    {
        e = n.ToString(Sp0, DeltaSp, EndOfLine, iFeatures);
        s += e + EndOfLine;
    }
    return s;
}

CMaaString CMaaXmlDocument::ToString(int ApproxLength, CMaaString Sp0, CMaaString DeltaSp, CMaaString EndOfLine, int iFeatures)
{
    try
    {
        CMaaConcatString s(ApproxLength < 0 ? 128 * 1024 : ApproxLength, 1);
        //CMaaString Sp0;
        //CMaaString DeltaSp = " ";

        CMaaString e;
        CMaaXmlNode n = FirstChild();
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
        for (; !n.IsNull(); n = n.NextSibling())
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

/*
bool CMaaXmlDocument::SetContent(CMaaFile File, CMaaString *errorMsg, int *errorLine, int *errorColumn)
{
     return false;
}
*/

CMaaXmlDocument & CMaaXmlDocument::operator=(const CMaaXmlDocument & That)
{
    m_Name = That.m_Name;
    m_Root = That.m_Root;
    return *this;
}

struct Lnk1 : public CMaaDLink
{
    CMaaXmlNode n;
    Lnk1(CMaaXmlNode &Node)
    :   n(Node)
    {
    }
};

static bool IsAlphaXml(char c, int idx)
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
    if  ((c >= '�' && c <= '�') || c == '�')
    {
        return true;
    }
    if  ((c >= '�' && c <= '�') || c == '�')
    {
        return true;
    }
    return c == '_' || c == '-';
}

static bool IsSpace(char c)
{
    return c == ' ' || c == '\t' || c == 0;
}

bool CMaaXmlNode::AddAttributeVerifyName(CMaaString Name, CMaaString Value)
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
    for (int i = 0; i < Name.Length(); i++)
    {
        char c = Name[i];
        if  (IsAlphaXml(c, i))
        {
            ;
        }
        else
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
            CMaaString Line = Text.GetLine(false, true).ToLower(0);
            int n = Line.Find("encoding=");
            if  (n > 0)
            {
                Line = Line.Mid(n + 9).GetWord(" '\"");
                if  (Line.Find("utf") < 0)
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
                    *errorMsg = "'<' expected";
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
                    *errorMsg = "'>' expected";
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
                        if  (memcmp(p, "DOCTYPE", strlen("DOCTYPE")))
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
                            *errorMsg = "'\"' expected";
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
                            *errorMsg = "'\"' expected";
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
        if  (List.LookAtFront())
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
bool CMaaXmlDocument::SetContent(CMaaString Text1, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures)
{
    if  (iFeatures & eCachedAll)
    {
        CMaaUnivHash<CMaaString, CMaaString> hStringsCache;
        return SetContent(Text1, hStringsCache, errorMsg, errorLine, errorColumn, iFeatures);
    }
    return SetContent(Text1, *sphStringsCacheNull, errorMsg, errorLine, errorColumn, iFeatures);
}

bool CMaaXmlDocument::SetContent(CMaaString Text1, CMaaUnivHash<CMaaString, CMaaString>& hStringsCache, CMaaString* errorMsg, int* errorLine, int* errorColumn, int iFeatures)
{
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

    bool Ret = false;
    int cp = e_utf8;

    try
    {
        CMaaXmlDocument doc(m_Name);

        CMaaString Text = CMaaString((const char *)Text1, Text1.Length());
        for (int i = 0/*CMaaFile::ePfxUtf8*/; i < CMaaFile::ePfxMax; i++)
        {
            CMaaString prefix = CMaaFile::GetPrefix(i);
            if  (prefix.Length() && Text.Left(prefix.Length()) == prefix)
            {
                Text = Text.RefMid(prefix.Length());
                break;
            }
        }
        {
            CMaaString Line = Text.GetLine(false, true).ToLower(0);
            int n = Line.Find("encoding=");
            if  (n > 0)
            {
                Line = Line.Mid(n + 9).GetWord(" '\"");
                if  (Line.Find("utf") < 0)
                {
                    cp = 1251;
                }
            }
        }

        CMaaDListAD<Lnk1> List;
        CMaaXmlNode c = doc.m_Root;

        const char * p = (const char *)Text;
        int Pos = 0;
        int Len = Text.Length();
        int StartLine = 0;
        *errorLine = 0;
        *errorColumn = 0;
        int zline, zcolumn;
        zline = zcolumn = -1;
        bool bIsEmpty = true;
        while(Pos < Len)
        {
            int x = Text.Find(Pos, '<');
            if  (x < 0)
            {
                x = Len;
            }
            int i;
            for (i = Pos; i < x; i++)
            {
                SkipCrLfOrLf;

                if  (!IsSpace(p[i]))
                {
                    *errorMsg = "'<' expected";
                    *errorColumn = i - StartLine;
                    throw 1;
                }
            }
            if  (zline + zcolumn == -2)
            {
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
                if  (!IsSpace(p[i]))
                {
                    break;
                }
            }
            if  (i >= Len)
            {
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
                if  (!IsAlphaXml(p[i], i - Pos))
                {
                    break;
                }
            }
            if  (i == Pos)
            {
                //*errorLine = el;
                *errorColumn = Pos - StartLine;
                *errorMsg = "element name expected after '<'";
                throw 3;
            }
            CMaaString ElementName = CMaaString(p + Pos, i - Pos);
            (iFeatures& eCachedNames) && hStringsCache.Find(ElementName, &ElementName) && hStringsCache.Add(ElementName, ElementName);

            el = *errorLine;
            esl = StartLine;
            epos = Pos;
            Pos = i;
            for (i = Pos; i < Len; i++)
            {
                SkipCrLfOrLf;
                if  (!IsSpace(p[i]))
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
                Lnk1 * l = List.GetFromBack();
                if  (!l)
                {
                    *errorLine = el;
                    *errorColumn = epos -esl;
                    *errorMsg = "element is not opened";
                    throw 5;
                }
                CMaaString Name = l->n.GetName();
                delete l;
                if  (Name != ElementName)
                {
                    *errorLine = el;
                    *errorColumn = epos -esl;
                    *errorMsg = "element name to be closed mismatch";
                    throw 6;
                }
                l = List.LookAtBack();
                c = l ? l->n : doc.m_Root;

                zline = zcolumn = -1;

                Pos = i + 1;
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
                    *errorLine = el;
                    *errorColumn = epos -esl;
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
                    if  (!IsAlphaXml(p[i], i - Pos))
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
                ElementName = CMaaString(p + Pos, i - Pos);
                (iFeatures& eCachedNames) && hStringsCache.Find(ElementName, &ElementName) && hStringsCache.Add(ElementName, ElementName);

                Pos = i;
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (!IsSpace(p[i]))
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
            (iFeatures& eCachedNames) && hStringsCache.Find(Name, &Name) && hStringsCache.Add(Name, Name);
            CMaaXmlNode n = CMaaXmlNode(tType, Name);
            int attr_count = 0;
            // get attributes
            while(1)
            {
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (!IsSpace(p[i]))
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
                    if  (!IsAlphaXml(p[i], i - Pos))
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
                CMaaString aName = CMaaString(p + Pos, i - Pos);
                if  (p[i] != '=')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "'=' is expected";
                    throw 12;
                }
                i++;
                char cquote = p[i];
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
                CMaaString aValue = CMaaString(p + Pos, i - Pos);
                Pos = ++i;
                if  (!IsSpace(p[i]) && p[i] != '\r' && p[i] != '\n' &&
                     p[i] != '/' && p[i] != '>'  && p[i] != '?')
                {
                    *errorColumn = i - StartLine;
                    *errorMsg = "space or newline or '/' or '>' is expected";
                    throw 15;
                }
#ifdef CMaaStringReplace
                CMaaString Name = aName;
                CMaaString Value = CMaaXmlNode::DecodeAttr(aValue, cp);
#else
                CMaaString Name = (const char *)aName;
                CMaaString Value = CMaaXmlNode::DecodeAttr((const char *)aValue, cp);
#endif
                (iFeatures& eCachedNames) && hStringsCache.Find(Name, &Name) && hStringsCache.Add(Name, Name);
                (iFeatures& eCachedValues) && hStringsCache.Find(Value, &Value) && hStringsCache.Add(Value, Value);
                n.AddAttribute(Name, Value);
                attr_count++;
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
                Pos = ++i;
                for (i = Pos; i < Len; i++)
                {
                    SkipCrLfOrLf;
                    if  (!IsSpace(p[i]))
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
                if  (!IsSpace(p[i]))
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

            Pos = ++i;

            if  (attr_count >= 0 && !bClosed && (iFeatures & eAcceptStrings))
            // == -> >= 2018
            {
                bool bRssCData = false;
                int x, xr = 0;
                if  ((iFeatures & eRssCDataStrings) && Text.RefMid(Pos, 9) == "<![CDATA[")
                {
                    bRssCData = true;
                    x = Text.Find(Pos + 9, "]]>");
                    if  (x >= 0)
                    {
                        xr = x;
                        x += 3;
                        while(x < Text.Length() && IsSpace(Text[x]))
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
                    x = Text.Find(Pos, '<');
                }
                if  (x >= 0)
                {
                    CMaaString Name = n.GetName();
                    int y = Text.Find(bRssCData ? x + 1 : Pos, '>');
                    if  (y >= 0)
                    {
                        CMaaString ClosingTag = Text.Mid(x + 1, y - x - 1);
                        int ii, j;
                        for (ii = j = 0; ii < ClosingTag.Length(); ii++)
                        {
                            if  (ClosingTag[ii] == '\r' || ClosingTag[ii] == '\n')
                            {
                                continue;
                            }
                            if  (ClosingTag[ii] == '/')
                            {
                                ClosingTag[j++] = '/';
                                continue;
                            }
                            if  (IsSpace(ClosingTag[ii]))
                            {
                                continue;
                            }
                            break;
                        }
                        ClosingTag = ClosingTag.Left(j) + ClosingTag.Mid(ii);
                        for (ii = ClosingTag.Length() - 1; ii >= 0; ii--)
                        {
                            if  (ClosingTag[ii] == '\r' || ClosingTag[ii] == '\n')
                            {
                                continue;
                            }
                            if  (IsSpace(ClosingTag[ii]))
                            {
                                continue;
                            }
                            break;
                        }
                        ClosingTag = ClosingTag.Left(ii + 1);

                        if  (ClosingTag == CMaaString("/") + Name)
                        {
                            if  (attr_count == 0 && !(iFeatures & eComplexStrings))
                            {
                                // it's a string, not node
                                CMaaString RawVal = Text.Mid(Pos, x - Pos);
                                CMaaString Value = bRssCData ? RawVal.Mid(9, xr - Pos - 9) : CMaaXmlNode::DecodeAttr(RawVal, cp);

                                Lnk1 * l = List.LookAtBack();
                                if  (!l)
                                {
                                    *errorLine = zline;
                                    *errorColumn = zcolumn;
                                    *errorMsg = "element for new string is not opened";
                                    throw 18;
                                }
                                (iFeatures & eCachedNames) && hStringsCache.Find(Name, &Name) && hStringsCache.Add(Name, Name);
                                (iFeatures & eCachedValues) && hStringsCache.Find(Value, &Value) && hStringsCache.Add(Value, Value);
                                l->n.AddString(Name, Value);

                                for (i = Pos; i <= y; i++)
                                {
                                    SkipCrLfOrLf;
                                }
                                Pos = y + 1;
                                continue;
                            }
                            else // 2018
                            {
                                CMaaString RawVal = Text.Mid(Pos, x - Pos);
                                CMaaString Value = bRssCData ? RawVal.Mid(9, xr - Pos - 9) : CMaaXmlNode::DecodeAttr(RawVal, cp);
                                (iFeatures & eCachedStrings) && hStringsCache.Find(Value, &Value) && hStringsCache.Add(Value, Value);
                                n.SetString(Value);
                                Pos = y + 1;
                                bClosed = true;
                            }
                        }
                    }
                }
            }
            if  (bClosed && attr_count == 0 && (iFeatures & (eTreatEmptyNodeAsAString | eComplexStrings)) == eTreatEmptyNodeAsAString)
            {
                // empty node or string?

                //making string
                Lnk1 * l = List.LookAtBack();
                if  (!l)
                {
                    *errorLine = zline;
                    *errorColumn = zcolumn;
                    *errorMsg = "element for new string is not opened";
                    throw 18;
                }
                l->n.AddString(n.GetName(), "");
                continue;
            }

            c.AppendChild(n);
            if  (!bClosed)
            {
                Lnk1 * l = TL_NEW Lnk1(n);
                if  (!l)
                {
                    *errorColumn = Pos - StartLine;
                    *errorMsg = "there is not enough free memory";
                    throw 17;
                }
                List.AddAtBack(l);
                c = n;
            }
        }
        if  (bIsEmpty)
        {
            *errorMsg = "document is empty: '<' expected";
            *errorColumn = 0;
            *errorLine = 0;
            throw 19;
        }
        //(*errorLine)++;
        *errorColumn = Pos - StartLine;
        if  (List.LookAtFront())
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
        (*errorLine)++;
        (*errorColumn)++;
        //throw;
    }
    catch(...)
    {
        (*errorLine)++;
        (*errorColumn)++;
        //throw;
    }
    if  (!Ret)
    {
        static int aa = 0;
        //const char * msg = *errorMsg;
        aa++;
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
     //new (m_Imp(*(CMaaRefDLink<CMaaXmlNodeImpl> *)m_ImpBuffer), //x.m_Imp),
     new (&m_Imp)CMaaRefDLink<CMaaXmlNodeImpl>(x.m_Imp);
}
CMaaXmlNode::XX::XX(const XX &That)
:       m_Imp(*(CMaaRefDLink<CMaaXmlNodeImpl> *)m_ImpBuffer), //x.m_Imp),
     szTxt(That.szTxt),
     bConst(That.bConst)
{
     new (&m_Imp)CMaaRefDLink<CMaaXmlNodeImpl>(That.m_Imp);
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
     new (&m_Imp)CMaaRefDLink<CMaaXmlNodeImpl>(x.m_Imp);
}
bool CMaaXmlNode::XX::operator=(CMaaString txt)
{
     if   (bConst)
     {
          return false;
     }
     CMaaXmlNode n(!m_Imp.IsNull() ? m_Imp.m_Imp : nullptr);
     return n.AddAttribute(szTxt, txt);
}
bool CMaaXmlNode::XX::operator=(const XX & That)
{
     if   (bConst)
     {
          return false;
     }
     CMaaXmlNode n1(!m_Imp.IsNull() ? m_Imp.m_Imp : nullptr);
     CMaaXmlNode n2(!That.m_Imp.IsNull() ? That.m_Imp.m_Imp : nullptr);
     return n1.AddAttribute(szTxt, n2.FindAttribute(That.szTxt));
}
*/

CMaaXmlNode::XX::XX(CMaaXmlNode &x, CMaaString Txt)
:   m_Impl(x.m_Imp),
    szTxt(Txt),
    bConst(false)
{
    //new (&Impl())CMaaRefDLink<CMaaXmlNodeImpl>(x.m_Imp);
}
CMaaXmlNode::XX::XX(const CMaaXmlNode &x, CMaaString Txt)
:   m_Impl(x.m_Imp),
    szTxt(Txt),
    bConst(true)
{
    //new (&Impl())CMaaRefDLink<CMaaXmlNodeImpl>(x.m_Imp);
}
CMaaXmlNode::XX::XX(const XX &That)
:   m_Impl(That.Impl()),
    szTxt(That.szTxt),
    bConst(That.bConst)
{
    //new (&Impl())CMaaRefDLink<CMaaXmlNodeImpl>(That.Impl());
}
CMaaXmlNode::XX::~XX()
{
    //Impl().~CMaaRefDLink();
}

/*
CMaaXmlNode::XX::XX(CMaaXmlNode &x, CMaaString Txt)
:    szTxt(Txt),
     bConst(false)
{
     new (&n())CMaaXmlNode(x);
}
CMaaXmlNode::XX::XX(const CMaaXmlNode &x, CMaaString Txt)
:    szTxt(Txt),
     bConst(true)
{
     new (&n())CMaaXmlNode(x);
}
CMaaXmlNode::XX::XX(const XX &That)
:    szTxt(That.szTxt),
     bConst(That.bConst)
{
     new (&n())CMaaXmlNode(That.n());
}
CMaaXmlNode::XX::~XX()
{
     n().~CMaaRefDLink();
}
*/

CMaaXmlNode::XX::operator CMaaString () const
{
    return n().FindAttribute(szTxt);
}
/*
CMaaXmlNode::XX::operator const char * () const
{
     return n().FindAttribute(szTxt);
}
*/
CMaaXmlNode::XX::operator const CMaaXmlNode() const
{
    return n().FindNode(szTxt);
}
CMaaXmlNode::XX::operator CMaaXmlNode()
{
    return n().FindNode(szTxt);
}
bool CMaaXmlNode::XX::operator=(CMaaString txt)
{
    if  (bConst)
    {
        return false;
    }
    return n().AddAttribute(szTxt, txt);
}
bool CMaaXmlNode::XX::operator=(const XX & That)
{
    if  (bConst)
    {
        return false;
    }
    return n().AddAttribute(szTxt, That.n().FindAttribute(That.szTxt));
}

bool CMaaXmlDocument::LoadFromFile(CMaaString FileName, bool bThrow, CMaaString *errorMsg, int *errorLine, int *errorColumn, int iFeatures)
{
    CMaaFile f(FileName, "R|SrSw", bThrow);
    if  (f.IsOpen())
    {
        CMaaString txt = f.Read();
        CMaaXmlDocument doc("doc");
        if  (doc.SetContent(txt, errorMsg, errorLine, errorColumn, iFeatures))
        {
            *this = doc;
            return true;
        }
    }
    return false;
}

bool CMaaXmlDocument::SaveToFile(CMaaString FileName, bool bThrow, CMaaString Sp0, CMaaString DeltaSp, CMaaString EndOfLine, int iFeatures)
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
    CMaaString Text = prefix + txt;

    CMaaFile f(FileName, "WCD|SrSw", bThrow);
    if  (f.IsOpen())
    {
        if  (f.Write(Text) == (_dword)Text.Length())
        {
            f.Close();
            return true;
        }
    }
    return false;
}

void CMaaXmlNode::ConvertToUtf8(bool /*bChilds*/, _dword cp)
{
    if  (!IsNull())
    {
        m_Imp->ConvertToUtf8(cp);

        CMaaXmlNode c = FirstChild();
        for (; !c.IsNull(); c = c.NextSibling())
        {
            c.ConvertToUtf8(true, cp);
        }
    }
}

void CMaaXmlDocument::ConvertToUtf8(_dword cp)
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

void CMaaXmlDocument::AddReplaceProcessingInstruction(CMaaString Encoding, CMaaString Version, _dword ToUtf8Cp)
{
    if  ((int)ToUtf8Cp != -2 && Encoding == "utf-8")
    {
        ConvertToUtf8(ToUtf8Cp);
    }

    CMaaXmlNode pi = FirstChild();
    for (; !pi.IsNull(); pi = pi.NextSibling())
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
        if  (!pi.IsNull())
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
CMaaString CMaaXmlNode::EncodeString(CMaaString s, int iFeatures, CMaaString Name)
{
    return ((iFeatures & CMaaXmlDocument::eRssCDataStrings) && s.Find("]]>") < 0 && Name != "lastBuildDate") ? CMaaString("<![CDATA[") + s + "]]>" : EncodeAttr(s, iFeatures);
}
//-------------------------------------------------------------------------------------------------
void CMaaXmlNode::EncodeString(CMaaConcatString &c, CMaaString s, int iFeatures, CMaaString Name)
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


//-------------------------------------------------------------------------------------------------
#if 0
//-------------------------------------------------------------------------------------------------
int main(int argn, char * args[])
{
    int i;
    CMaaXmlDocument doc("CopierConfig");

    if  (0)
    {
        CMaaXmlNode n = doc.FirstChild();
        if  (n.IsNull())
        {
            printf("n : IsNull\n");
        }
    }

    CMaaXmlProcessingInstruction xmlNode = doc.CreateProcessingInstruction("xml");
    xmlNode.AddInstruction(CMaaXmlProcessingInstruction::eVersion, "1.0");
    xmlNode.AddInstruction(CMaaXmlProcessingInstruction::eEncoding, "utf-8");
    doc.AppendChild(xmlNode);

    CMaaXmlElement cfg = doc.CreateElement("CopierConfig");
    doc.AppendChild(cfg);

    CMaaXmlElement sett = doc.CreateElement("Settings");
    //sett.setAttribute("CaseSensitiveMasks", checkCase->checkState());
    sett.SetAttribute("CaseSensitiveMasks", "false");
    cfg.AppendChild(sett);

    CMaaXmlElement exts = doc.CreateElement("Extentions");
    cfg.AppendChild(exts);
    exts.SetAttribute("Test", "Temp");

    for (i = 0; i < 10; i++)
    {
        CMaaXmlElement extList = doc.CreateElement("ExtentionsList");
        extList.SetAttribute("number", CMaaString::sFormat("%d", i));
        extList.SetAttribute("name", "���");
        exts.AppendChild(extList);
    }
    /*
     n = TreeExt->topLevelItemCount ();
        for  (i = 0; i < n; i++)
     {
          QTreeWidgetItem * p = TreeExt->topLevelItem(i);

          CMaaXmlElement extList = doc.createElement("ExtentionsList");
          extList.setAttribute("number", i);
          extList.setAttribute("name", p->text(0));

          int nn = p->childCount ();
          for  (int j = 0; j < nn; j++)
          {
               QTreeWidgetItem * c = p->child(j);
               CMaaXmlElement ext = doc.createElement("ext");

               ext.setAttribute("action", c->data(1,Qt::UserRole).toInt());
               ext.setAttribute("value",  c->text(0));
               extList.appendChild(ext);
          }
          exts.appendChild(extList);
     }
        */
    CMaaXmlElement exepts = doc.CreateElement("Exeptions");
    cfg.AppendChild(exepts);
    /*
        n = TreeExept->topLevelItemCount ();
     for  (i = 0; i < n; i++)
     {
          QTreeWidgetItem * p = TreeExept->topLevelItem(i);

          CMaaXmlElement exeptList = doc.createElement("ExeptionsList");
          exeptList.setAttribute("number", i);
          exeptList.setAttribute("name", p->text(0));

          int nn = p->childCount ();
          for  (int j = 0; j < nn; j++)
          {
               QTreeWidgetItem * c = p->child(j);
               CMaaXmlElement exept = doc.createElement("exept");

               exept.setAttribute("action", c->data(1,Qt::UserRole).toInt());
               exept.setAttribute("value",  c->text(0));
               exeptList.appendChild(exept);
          }
          exepts.appendChild(exeptList);
     }
        */
    CMaaXmlElement tasks = doc.CreateElement("Tasks");
    cfg.AppendChild(tasks);
    /*
        n = TreeDirs->topLevelItemCount ();
     for  (i = 0; i < n; i++)
     {
          QTreeWidgetItem * p = TreeDirs->topLevelItem(i);

          CMaaXmlElement taskList = doc.createElement("TasksList");
          //taskList.setAttribute("number", i);
          taskList.setAttribute("name", p->text(0));

          int nn = p->childCount ();
          for  (int j = 0; j < nn; j++)
          {
               QTreeWidgetItem * c = p->child(j);
               CMaaXmlElement task = doc.createElement("task");

               task.setAttribute("from", c->text(0));
               task.setAttribute("to", c->text(1));
               task.setAttribute("ext",  c->data(2, Qt::UserRole).toInt());
               task.setAttribute("exept",  c->data(3, Qt::UserRole).toInt());
               taskList.appendChild(task);
          }
          tasks.appendChild(taskList);
     }
        */
    /*

     QFile file(ConfigFileName);
     if   (file.open(QIODevice::WriteOnly))
     {
          QTextStream out(&file);
          out.setCodec(Codec);
          out << doc.ToString ();
          file.close();
     }
     else
     {
          QString s = tr("Can't open file \"%1\" for writing");
          s.replace("%1", ConfigFileName);
          QMessageBox b("Info", s, QMessageBox::Information, 0,0,1, this);
          b.exec();
     }
        */

    CMaaString s = doc.ToString();
    //printf("%s\r\n\r\n", (const char *) s);

    //s += "   </123>";

    try
    {
        CMaaFile f("1.xml", "rb");
        int l = f.Length();
        CMaaPtr<char> Buf(l + 1);
        l = f.Read(Buf, l);
        if  (l >= 0)
        {
            Buf[l] = 0;
        }
        s = Buf;
        CMaaXmlDocument doc2("NoName");
        CMaaString errorMsg;
        int errorLine, errorColumn;

        if  (!doc2.SetContent(s, &errorMsg, &errorLine, &errorColumn))
        {
            printf("Line: %d, Column: %d, Error: %s\n", errorLine, errorColumn, (const char *)errorMsg);
        }

        CMaaString s2 = doc2.ToString();
        printf("%s\r\n\r\n", (const char *) s2);
        CMaaFile f3("3.xml", "wb");
        f3.Write((const char *) s2, s2.Length());
    }
    catch(XTOOFile e)
    {
        printf("Error: %s\n", (const char *)e.ErrorMessage());
    }
    catch(...)
    {
        printf("Unknown error\n");
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------
#endif // #if 0
