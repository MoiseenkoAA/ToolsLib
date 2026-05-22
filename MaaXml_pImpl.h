
// ToolsLib Project

/* CryptLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko and MaaSoftware (JSK, Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: –ú–æ–∏—Å–µ–µ–Ω–∫–æ –ê–Ω–¥—Ä–µ–π –ê–ª–µ–∫—Å–µ–µ–≤–∏—á)
 */

// ToolsLib/MaaXml.h

/* Copyright (C) 2002-2013 Andrey A. Moiseenko (support@maasoftware.ru)
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
 * Copyright remains Andrey A. Moiseenko's, and as such any Copyright notices in
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

extern void * gV;

/*
class test1
{
    int xx;
public:
    test1();
    ~test1();
    int f(int x);
};

#define gp1 (*p1)
extern test1 * p1;
*/



//-------------------------------------------------------------------------------------------------
template <typename T> class CMaaRefDLink
{
public:
    class Impl : public CMaaDLink
    {
    public:
        T * m_Ptr;
#ifdef MTX_INTERLOCKS
        long m_Refs;
#else
        std::atomic<long> m_Refs;
#endif
        Impl * m_pParent;

        Impl(T * pp)
        :   m_Ptr(pp),
            m_Refs(1),
            m_pParent(nullptr)
        {
        }
        ~Impl()
        {
            delete m_Ptr;
        }
        ADD_ALLOCATOR(Impl)
    };
private:
public:
    Impl * m_Imp;
public:
    int GetRefCount()
    {
        return m_Imp ? (int)m_Imp->m_Refs : 0;
    }
    explicit CMaaRefDLink(T * p)
    :   m_Imp(new Impl(p))
    {
    }
    ~CMaaRefDLink()
    {
        Decrement();
    }
    CMaaRefDLink(const CMaaRefDLink<T> & That)
    :   m_Imp(That.m_Imp)
    {
        Increment();
    }
    CMaaRefDLink(Impl * I)
    :   m_Imp(I ? I : new Impl(nullptr))
    {
        if  (I)
        {
            Increment();
        }
    }
    CMaaRefDLink & operator=(const CMaaRefDLink<T> & That)
    {
        if  (m_Imp != That.m_Imp)
        {
            CMaaRefDLink<T> t(*this);
            //tmp.Increment();
            ((CMaaRefDLink<T> &)That).Increment();
            //Decrement();
            m_Imp = That.m_Imp;
            t.Decrement();
        }
        return *this;
    }
    CMaaRefDLink & operator=(Impl * I)
    {
        if  (m_Imp != I)
        {
            CMaaRefDLink<T> n(I);
            *this = n;
        }
        return *this;
    }
    T * operator->() const
    {
        return m_Imp->m_Ptr;
    }
    T & operator*() const
    {
        return *(m_Imp->m_Ptr);
    }
    bool IsNull() const
    {
        return m_Imp->m_Ptr == nullptr;
    }
    operator Impl * ()
    {
        return m_Imp;
    }
    //private:
    void Decrement()
    {
#ifdef MTX_INTERLOCKS
#ifdef _WIN32
        long r = ::InterlockedDecrement(&m_Imp->m_Refs);
#else
        AtomicLock();
        long r = --(m_Imp->m_Refs);
        AtomicUnLock();
#endif
#else
        long r = --(m_Imp->m_Refs);
#endif
        if  (r == 0)
        {
            delete m_Imp;
            m_Imp = nullptr;
            //return 0;
        }
        //return r;
        /*
          if   (gV == m_Imp)
          {
               char txt[128];
               sprintf(txt, "Decrement to %d", m_Imp->m_Refs - 1);
               //MessageBox(nullptr, txt, "", MB_OK);
          }
		*/
        /*
          if	(--(m_Imp->m_Refs) == 0)
		{
			delete m_Imp;
			m_Imp = nullptr;
		}
          */
    }
    void Increment()
    {
#ifdef MTX_INTERLOCKS
#ifdef _WIN32
        /*return*/ ::InterlockedIncrement(&m_Imp->m_Refs);
#else
        AtomicLock();
        /*long r =*/ ++(m_Imp->m_Refs);
        AtomicUnLock();
        //return r;
#endif
#else
        ++(m_Imp->m_Refs);
#endif

        /*
          if   (gV == (m_Imp + 100000))
          {
               gV = m_Imp;
          }

          if   (gV == m_Imp)
          {
               char txt[128];
               sprintf(txt, "Increment to %d", m_Imp->m_Refs + 1);
               //MessageBox(nullptr, txt, "", MB_OK);
          }
          */
        //++(m_Imp->m_Refs);
    }
    ADD_ALLOCATOR(CMaaRefDLink<T>)
};
//-------------------------------------------------------------------------------------------------
class CMaaXmlDocument;
class CMaaXmlProcessingInstruction;
class CMaaXmlElement;
class CMaaXmlNode;
class CMaaXmlNodeImpl;
//-------------------------------------------------------------------------------------------------
typedef CMaaRefDLink<CMaaXmlNodeImpl>::Impl * CMaaXmlHandle;
class CMaaXmlNode
{
    friend class CMaaXmlNodeImpl;
    friend class CMaaXmlDocument;
    CMaaRefDLink<CMaaXmlNodeImpl> m_Imp;
protected:
    bool AddNode(CMaaXmlNode &Node);
    bool RemNode(CMaaXmlNode &Node);
public:
    bool AddAttribute(CMaaString Name, CMaaString Value);
    bool AddAttributeVerifyName(CMaaString Name, CMaaString Value);
    bool RemoveAttribute(CMaaString Name);
    int RemoveAllAttributes(); // returns number of attributes that was removed
    bool AddString(CMaaString Name, CMaaString Value);
    bool RemoveString(CMaaString Name);
    int RemoveAllStrings(); // returns number of attributes that was removed
    int CopyAttributes(CMaaXmlNode From, bool bHiddenPtrs = false);
    bool SetString(CMaaString Value);
    CMaaString GetString(bool *pIsFound = nullptr);
public:
    CMaaXmlNode(CMaaRefDLink<CMaaXmlNodeImpl>::Impl * I);
    CMaaXmlNode(int Type, CMaaString Name);
    CMaaXmlNode(const CMaaXmlNode & That);
    CMaaXmlNode & operator=(const CMaaXmlNode & That);
    virtual ~CMaaXmlNode();
    CMaaXmlHandle GetHandle();
    //operator CMaaXmlHandle ();
    CMaaXmlNode GetParent();
    operator bool()
    {
        return !IsNull();
    }
    CMaaXmlNode operator++()
    {
        return (*this = FindNext());
    }
    CMaaXmlNode operator--()
    {
        return (*this = FindPrev());
    }
    bool IsProcessingInstruction();
    bool IsElement();
    CMaaXmlNode FirstChild();
    CMaaXmlNode NextSibling();
    CMaaXmlNode PrevSibling();
    bool IsNull();
    bool IsValid()
    {
        return !IsNull();
    }
    bool IsString()
    {
        return GetString().Length() != 0;
    }
    bool AddProcessingInstruction(CMaaXmlNode & Node);
    bool AppendChild(CMaaXmlNode & Node);
    bool RemoveChild(CMaaXmlNode & Node); // Detach child from parent(this)
    bool Remove(); // Detach child from parent(this)
    static CMaaString EncodeAttr(CMaaString s, int iFeatures = 0);
    static void EncodeAttr(CMaaConcatString &c, CMaaString s, int iFeatures = 0);
    static CMaaString DecodeAttr(CMaaString s, int cp /*= e_utf8*/);
    static CMaaString EncodeString(CMaaString s, int iFeatures = 0, CMaaString Name = ""); // {return ((iFeatures & CMaaXmlDocument::eRssCDataStrings) && Name != "lastBuildDate") ? CMaaString("<![CDATA[") + s + "]]>" : EncodeAttr(s, iFeatures);}
    static void EncodeString(CMaaConcatString &c, CMaaString s, int iFeatures, CMaaString Name = ""); // chk for Rss {return EncodeAttr(c, s, iFeatures);}
    static CMaaString DecodeString(CMaaString s, int cp /*= e_utf8*/) {return DecodeAttr(s, cp);}
    CMaaString ToString(CMaaString Sp0 = "", CMaaString DeltaSp = " ", CMaaString EndOfLine = "\n", int iFeatures = 0);
    void ToString(CMaaConcatString &s, CMaaString Sp0 = "", CMaaString DeltaSp = " ", CMaaString EndOfLine = "\n", int iFeatures = 0);
    //CMaaXmlProcessingInstruction toProcessingInstruction();
    //CMaaXmlElement toElement();
    CMaaString GetName();
    void ChangeName(CMaaString Name);
    CMaaString FindAttribute(CMaaString Name, bool *pIsFound = nullptr);
    CMaaString FindString(CMaaString Name, bool *pIsFound = nullptr);
    const void * FirstStringIterator();
    const void * FirstAttributeIterator();
    bool GetString(const void * Iterator, CMaaString *pName, CMaaString *pValue);
    bool GetAttribute(const void * Iterator, CMaaString *pName, CMaaString *pValue);
    const void * NextStringIterator(const void * Iterator);
    const void * NextAttributeIterator(const void * Iterator);
    CMaaXmlNode FindNode(CMaaString Name, int *pN = nullptr);
    CMaaXmlNode FindNext();
    CMaaXmlNode FindPrev();
    CMaaXmlNode FindNodeWithAttr(CMaaString Name, CMaaString AttrName, CMaaString AttrVal, CMaaXmlNode SkipNode = CMaaXmlNode(nullptr));
    CMaaXmlNode FindNodeWithAttrRO(CMaaMutex * mtx, CMaaString Name, CMaaString AttrName, CMaaString AttrVal, CMaaXmlNode SkipNode = CMaaXmlNode(nullptr), bool bClean = false); // for RO xml docs
    CMaaXmlNode FindNodeWithString(CMaaString Name, CMaaString StringName, CMaaString StringVal, CMaaXmlNode SkipNode = CMaaXmlNode(nullptr));
    int ChangeNodesAttr(CMaaString Name, CMaaString AttrName, CMaaString OldVal, CMaaString NewVal);
    int RemoveNodesAttr(CMaaString Name, CMaaString AttrName, CMaaString Val);
    int RemoveNodesWithAttr(CMaaString Name, CMaaString AttrName, CMaaString Val);
    int ChangeNodesString(CMaaString Name, CMaaString AttrName, CMaaString OldVal, CMaaString NewVal);
    int RemoveNodesString(CMaaString Name, CMaaString AttrName, CMaaString Val);
    int RemoveNodesWithString(CMaaString Name, CMaaString AttrName, CMaaString Val);
    int AddHiddenPtr(CMaaString Name, void * ptr, void (*deref)(CMaaString, void *, void *) = nullptr, void * obj = nullptr, bool bDerefOld = true);
    int AddHiddenInt(CMaaString Name, int x); // Hidden ptrs and ints are mixed in the same scope
    int AddHiddenString(CMaaString Name, CMaaString Str);
    int RemoveHiddenPtr(CMaaString Name, bool bDeref = false);
    int RemoveHiddenInt(CMaaString Name, bool bDeref = false)
    {
        // Hidden ptrs and strings and ints are mixed in the same scope
        return RemoveHiddenPtr(Name, bDeref);
    }
    int RemoveHiddenString(CMaaString Name)
    {
        // Hidden string ptrs and ptrs and ints are mixed in the same scope
        return RemoveHiddenPtr(Name, true);
    }
    void * GetHiddenPtr(CMaaString Name, bool *bGot = nullptr);
    CMaaString GetHiddenString(CMaaString Name, bool* bGot = nullptr);
    int GetHiddenInt(CMaaString Name, int Default = 0, bool *bGot = nullptr); // Hidden ptrs and ints are mixed in the same scope
    int RemoveAllHiddenPtrs(bool bDeref = false);
    int RemoveAllHiddenInts(bool bDeref = false)
    {
        // Hidden ptrs and ints are mixed in the same scope
        return RemoveAllHiddenPtrs(bDeref);
    }
    bool MoveUp();
    bool MoveDown();
    bool InsertBefore(CMaaXmlNode p); // in: *this is free, out: this will be before 'p'
    bool InsertAfter(CMaaXmlNode p); // in: *this is free, out: this will be after 'p'
    CMaaXmlNode CopyNode(CMaaXmlDocument &doc, bool bChilds = true, bool bHiddenPtrs = false/*not implemented*/);
    void ConvertToUtf8(bool bChilds = true, _dword cp =
#ifdef _WIN32
         CP_ACP
#else
         0
#endif
         );
    typedef void (* EnumAttributesFn)(CMaaString AttrName, CMaaString AttrValue, CMaaXmlNode &Node, void *Param);
    typedef void (* EnumStringsFn)(CMaaString AttrName, CMaaString AttrValue, CMaaXmlNode &Node, void *Param);

    class XX
    {
        //char m_ImpBuffer[sizeof(CMaaRefDLink<CMaaXmlNodeImpl>)];
        CMaaRefDLink<CMaaXmlNodeImpl> m_Impl;
    protected:
        CMaaRefDLink<CMaaXmlNodeImpl> & Impl() const
        {
            return (CMaaRefDLink<CMaaXmlNodeImpl> &)m_Impl;//*(CMaaRefDLink<CMaaXmlNodeImpl> *)(char *)m_ImpBuffer;
        }
    public:
        CMaaXmlNode n() const
        {
            return CMaaXmlNode(Impl());
        }
        CMaaString szTxt;
        bool bConst;

        XX(CMaaXmlNode &x, CMaaString Txt);
        XX(const CMaaXmlNode &x, CMaaString Txt);
        XX(const XX &That);
        ~XX();
        operator CMaaString () const;
        //operator const char * () const;
        operator const CMaaXmlNode() const;
        operator CMaaXmlNode();
        bool operator=(CMaaString txt);
        bool operator=(const XX & That);
        bool operator==(CMaaString s) const
        {
            return CMaaString(*this) == s;
        }
        bool operator!=(CMaaString s) const
        {
            return CMaaString(*this) != s;
        }
        CMaaString operator+(CMaaString s) const
        {
            return CMaaString(*this) + s;
        }
    };
    /*
     // maaxml.h(288) : error C2027: ËÒÔÓÎ¸ÁÓ‚‡ÌËÂ ÌÂÓÔÂ‰ÂÎÂÌÌÓ„Ó ÚËÔ‡ "CMaaXmlNode"
     // maaxml.h(173): ÒÏ. Ó·˙ˇ‚ÎÂÌËÂ 'CMaaXmlNode'
     struct XX
     {
     	char m_ImpBuffer[sizeof(CMaaXmlNode)];
     public:
          CMaaXmlNode &n()
          {
               return *(CMaaXmlNode *)m_ImpBuffer;
          }
          CMaaString szTxt;
          bool bConst;

          XX(CMaaXmlNode &x, CMaaString Txt);
          XX(const CMaaXmlNode &x, CMaaString Txt);
          XX(const XX &That);
          ~XX();
          operator CMaaString () const;
          operator const CMaaXmlNode() const;
          operator CMaaXmlNode();
          bool operator=(CMaaString txt);
          bool operator=(const XX & That);
     };
     */


    XX operator[](CMaaString txt)
    {
        return XX(*this, txt);
    }

    XX operator[](const char *txt)
    {
        return XX(*this, CMaaString(txt));
    }

    XX operator[](CMaaString txt) const
    {
        return XX(*this, txt);
    }

    XX operator[](const char *txt) const
    {
        return XX(*this, CMaaString(txt));
    }

    bool operator<<(CMaaXmlNode &x)
    {
        return AppendChild(x);
    }
protected:
    static void stEnumAttrFn(CMaaString AttrName, CMaaString AttrValue, void *Param);
public:
    int EnumAttributes(EnumAttributesFn Fn, void * Param = nullptr);
    int EnumStrings(EnumStringsFn Fn, void * Param = nullptr);
    int SortAttributes();
    int SortStrings();
    enum
    {
        eAttributes = 0x01,
        eStrings = 0x02,
        eRecursive = 0x100,
        eAll = eAttributes | eStrings,
        eAllRecursive = eAll | eRecursive
    };
    void Sort(int Flags = eAllRecursive);

    int GetRefCount()
    {
        return m_Imp.GetRefCount();
    }
protected:
    bool AddBefore(CMaaXmlNode n);
    bool AddAfter(CMaaXmlNode n);
};
//-------------------------------------------------------------------------------------------------
class CMaaXmlProcessingInstruction : public CMaaXmlNode
{
public:
    CMaaXmlProcessingInstruction(CMaaString Name);
    CMaaXmlProcessingInstruction(CMaaXmlNode &Node);
    enum
    {
        eVersion = 1,
        eEncoding
    };
    bool AddInstruction(int Type, CMaaString Value = CMaaString(""));
    bool AddInstruction(CMaaString Name, CMaaString Value);
};
//-------------------------------------------------------------------------------------------------
class CMaaXmlElement : public CMaaXmlNode
{
public:
    CMaaXmlElement(CMaaString Name);
    CMaaXmlElement(CMaaXmlNode &Node);
    bool SetAttribute(CMaaString Name, CMaaString Value);
    bool AppendChild(CMaaXmlNode &Node);
};
//-------------------------------------------------------------------------------------------------
class CMaaXmlDocument
{
    CMaaString m_Name;
    CMaaXmlNode m_Root;
public:
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
        eCachedAll = eCachedNames | eCachedValues | eCachedStrings,
        eCachedOpts = eCachedNames,
        eCrLfAreInTheSimplePresentation = 0x100,
        eDoNotAddDoctypeKey = 0x200,
        eDefault = eTreatEmptyNodeAsAString * 0 + eAcceptStrings + eCachedOpts + eComplexStrings * 0 + eRssCDataStrings * 0
    };
    CMaaXmlDocument(CMaaString Name);
    CMaaXmlDocument(CMaaString Name, CMaaString RootName);
    CMaaXmlDocument(CMaaXmlDocument & That, bool bCopy = false);
    ~CMaaXmlDocument();
    CMaaXmlProcessingInstruction CreateProcessingInstruction(CMaaString xml = "xml");
    CMaaXmlElement CreateElement(CMaaString Name);
    bool AddProcessingInstruction(CMaaXmlNode &Node);
    void AppendChild(CMaaXmlNode &Node);
    //bool SetContent(CMaaFile File, CMaaString *errorMsg, int *errorLine, int *errorColumn);
    bool SetContent(CMaaString Text, CMaaString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr, int iFeatures = eDefault);
    bool SetContent(CMaaString Text, CMaaUnivHash<CMaaString, CMaaString> & hStringsCache, CMaaString* errorMsg = nullptr, int* errorLine = nullptr, int* errorColumn = nullptr, int iFeatures = eDefault);
    CMaaXmlNode FirstChild();
    CMaaXmlElement DocumentElement();
    CMaaString ToString(int ApproxLength = -1, CMaaString Sp0 = "", CMaaString DeltaSp = " ", CMaaString EndOfLine = "\n", int iFeatures = 0 /*eCrLfAreInTheSimplePresentation|eDoNotAddDoctypeKey|eRssCDataStrings*/);
    CMaaString ToString(CMaaString Sp0, CMaaString DeltaSp = " ", CMaaString EndOfLine = "\n", int iFeatures = 0);
    CMaaXmlDocument & operator=(const CMaaXmlDocument & That);
    bool LoadFromFile(CMaaString FileName, bool bThrow = true, CMaaString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr, int iFeatures = eTreatEmptyNodeAsAString * 0 + eAcceptStrings + eCachedOpts);
    bool SaveToFile(CMaaString FileName, bool bThrow = true, CMaaString Sp0 = "", CMaaString DeltaSp = " ", CMaaString EndOfLine = "\r\n", int iFeatures = 0 /*eCrLfAreInTheSimplePresentation|eDoNotAddDoctypeKey|eRssCDataStrings*/);
    void AddReplaceProcessingInstruction(CMaaString Encoding = "utf-8", CMaaString Version = "1.0", _dword ToUtf8Cp = -1);
    void ConvertToUtf8(_dword cp = -1); // CP_ACP /*0*/ / 1251
};
//-------------------------------------------------------------------------------------------------

#endif // __TOOLS_MAAXML_H
