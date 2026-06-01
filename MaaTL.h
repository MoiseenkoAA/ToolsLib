// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/MaaTL.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform templates for working with
 * linked lists, hash tables and so on. First dated at 16.10.2003.
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

#ifndef __MAARF_MAATL_H
#define __MAARF_MAATL_H

#ifdef _MaaRF_INTERNAL_BUILD
//---------------------------------------------------------------------------
struct sDbgTmp
{
};
#define TRY_HF_OPT

#if defined(_WIN32) && !defined(_WIN64)
typedef int int3264;
typedef unsigned int uint3264;
#else
typedef _qword int3264;
typedef _uqword uint3264;
#endif

/*
#ifndef SIZE_MAX
// SIZE_MAX definition must match exactly with limits.h for modules support.
#ifdef _WIN64
#define SIZE_MAX 0xffffffffffffffffui64
#else
#define SIZE_MAX 0xffffffffui32
#endif
#endif
*/

// FOR COMPATIBILITY:
#define TL_NO_CMaaAllocatorStaticLocker
#define TL_DbgAlloc
#define SIZE_MAX (((size_t)-1) >> 1)
#define TL_NEW new
#define noexcept_new true
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
#define TOOLSLIB_HT_DEF_FLAG 0x04 // eAutoSqueeze
#else
#define TOOLSLIB_HT_DEF_FLAG 0x00
#endif
#define new_(ptr, Type) new(ptr) Type
#define new__ new

template <class _Ty>
struct std_remove_reference {
    using type = _Ty;
    using _Const_thru_ref_type = const _Ty;
};
template <class _Ty> using std_remove_reference_t = typename std_remove_reference<_Ty>::type;
template <class _Ty> constexpr std_remove_reference_t<_Ty>&& std_move(_Ty&& _Arg) noexcept { return static_cast<std_remove_reference_t<_Ty>&&>(_Arg); }

//---------------------------------------------------------------------------
#endif

//#define TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE // auto squeeze CMaaUnivHash if h.SetAutoSqueeze(true) is called / TOOLSLIB_HT_DEF_FLAG == 1
//#define TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
//#define TOOLSLIB_SHARED_ALLOCATOR

#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
#define TOOLSLIB_HT_DEF_FLAG 0x04 // eAutoSqueeze
#else
#define TOOLSLIB_HT_DEF_FLAG 0x00
#endif

#if defined(TOOLSLIB_MAX_SPEED) && defined(TOOLSLIB_SINGLE_THREAD) && !defined(TOOLSLIB_SHARED_ALLOCATOR)
//#define TOOLSLIB_SHARED_ALLOCATOR
#endif

#define TOOLSLIB_HASH_USE_LEN
#define TRY_HF_OPT // TRY_HF_OPT is faster with -flto, and slower without of -flto

#ifdef _WIN32
#define TRY_HF8_OPT
#endif
#ifdef __linux__
// If post defined "template <> _dword CMaaHashFunction<CMaaString>(const CMaaString & k) noexcept;", then
// TRY_HF8_OPT is slower by >3.2% (overall cms, linux x64).
// Inline post-defined "inline _dword CMaaHashFunction(const CMaaString & k) noexcept;" is sometimes solving this problem
//#define TRY_HF8_OPT
#endif

#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
#ifdef TOOLSLIB_SINGLE_THREAD
extern _qword gHashTablesAllocSize;
#else
extern std::atomic<_qword> gHashTablesAllocSize;
#endif
#endif

//---------------------------------------------------------------------------
#ifdef _WIN32_DRV_ALIGN_8
#pragma pack( push, temp_pack_id_2, 8 )
#endif
//---------------------------------------------------------------------------

#ifndef TOOLSLIB_MAX_SPEED
#define __ALLOCATOR_FILL
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
//#undef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
#endif
#endif
//#define __ALLOCATOR_RESERVE_PTR
#define __ALLOCATOR_FILL_INIT_CHAR 0xaa
#define __ALLOCATOR_FILL_ALLOC_CHAR 0xdd
#define __ALLOCATOR_FILL_FREE_CHAR 0xee

#define T_NULL(T) static_cast<T>(nullptr)

extern bool gbCMaaPtrThrow;

#ifndef _MaaRF_INTERNAL_BUILD
//#define Malloc(x) malloc(x)
//#define Free(x,y) free(x)
#define Memcpy(x,y,z) memcpy(x,y,z)
#define Memcmp(x,y,z) memcmp(x,y,z)
#define Memzero(x,y) memset(x,0,y)

#define Strlen(x) strlen(x)
#define Strcmp(x,y) strcmp(x,y)

#ifndef INT3
#define INT3
#endif
#ifndef CMaaPtr_THROW1
#define CMaaPtr_THROW1 throw "Memory allocation error"
#define CMaaPtr_THROW1_BAD_IMP throw "Bad implementation"
#endif
#ifndef CMaaPtr_THROW
#define CMaaPtr_THROW {if (gbCMaaPtrThrow) CMaaPtr_THROW1;}
#define CMaaPtr_THROW_BAD_IMP {if (gbCMaaPtrThrow) CMaaPtr_THROW1_BAD_IMP;}
#endif
#ifndef OUT_DEBUG_STRING
#define OUT_DEBUG_STRING(x)
#endif

#define std_is_nothrow_move_constructible0(T) std::is_nothrow_move_constructible<T>::value
#define std_is_nothrow_move_constructible1(T, a) std::is_nothrow_move_constructible<T>::value
#define std_is_nothrow_move_assignable(T, eq) std::is_nothrow_move_assignable<T>::value

#else

#ifndef CMaaPtr_THROW1
#define CMaaPtr_THROW1
#define CMaaPtr_THROW1_BAD_IMP
#endif
#ifndef CMaaPtr_THROW
#define CMaaPtr_THROW
#define CMaaPtr_THROW_BAD_IMP
#endif

#define std_is_nothrow_move_constructible0(T) noexcept(T())
#define std_is_nothrow_move_constructible1(T, a) noexcept(T(a))
#define std_is_nothrow_move_assignable(T, eq) noexcept(eq)

#endif

#ifdef CMaaPtr_THROW
#define CMaaUnivHash_THROW CMaaPtr_THROW
#else
#define CMaaUnivHash_THROW {}
#endif

//---------------------------------------------------------------------------
#define Min(a,b) ((a)<(b)?(a):(b))
#define Max(a,b) ((a)>(b)?(a):(b))
template<class T> int CMaaXSign(T x) { return x < 0 ? -1 : x > 0 ? 1 : 0; }
//---------------------------------------------------------------------------
//template < class T > void CMaaSwap ( T & a, T & b ) noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value)
//template < class T > void CMaaSwap ( T & a, T & b ) noexcept( noexcept(T(a)) && noexcept(a=b) )
template < class T > void CMaaSwap(T& a, T& b) noexcept(std_is_nothrow_move_constructible1(T, a) && std_is_nothrow_move_assignable(T, a = b))
{
    T tmp ( a );
    a = b;
    b = tmp;
}
// noexcept( noexcept(CMaaSwap(a, a)) )
//---------------------------------------------------------------------------
struct CMaaDLink
{
    CMaaDLink * Next, * Prev; // list

    // it is not need to initialize links before caling AddAtXxxx ()
    constexpr void Init() noexcept
    {
        Prev = Next = nullptr;
    }
    constexpr void InsertAfter ( CMaaDLink * That ) noexcept  // .... That, this, ....
    {
        Next = That -> Next;
        Prev = That;
        Next -> Prev = this;
        That -> Next = this;
    }
    constexpr void InsertBefore ( CMaaDLink * That ) noexcept  // .... this, That, ....
    {
        Prev = That -> Prev;
        Next = That;
        Prev -> Next = this;
        That -> Prev = this;
    }
    constexpr CMaaDLink() noexcept
    {
        Init();
    }
    constexpr CMaaDLink(eNotInit_) noexcept
    {
        // not init
    }
} GCC_PACKED;
//---------------------------------------------------------------------------
template < class T > struct CMaaDLink_Item : public CMaaDLink
{
    T m_Item;
    operator T & () noexcept
    {
        return m_Item;
    }
};
//---------------------------------------------------------------------------
template < class T > struct CMaaDLink_pItem : public CMaaDLink
{
    T * m_pItem;
    operator T *& () noexcept
    {
        return m_pItem;
    }
    T * operator -> () noexcept
    {
        return m_pItem;
    }
    T * operator = ( T * ptr ) noexcept
    {
        return m_pItem = ptr;
    }
};
//---------------------------------------------------------------------------
template < class T > struct CMaaDLink_pItemAD : public CMaaDLink_pItem < T >
{
    CMaaDLink_pItemAD () noexcept
    {
        CMaaDLink_pItem < T >:: m_pItem = nullptr;
    }
    ~CMaaDLink_pItemAD () noexcept
    {
        delete CMaaDLink_pItem < T >:: m_pItem;
    }
    T * operator = ( T * ptr ) noexcept
    {
        return CMaaDLink_pItem < T >::operator = ( ptr );
    }
};
//---------------------------------------------------------------------------
template < class T > class CMaaDList
{
    CMaaDLink Head;
public:
    bool m_AutoDeleteItems, m_Padding[3];
    //----------------------------------------------------------------------
    constexpr CMaaDList(bool AutoDeleteItems = false) noexcept
    :   Head(eNotInit)
    {
        Init();
        m_AutoDeleteItems = AutoDeleteItems;
        m_Padding[2] = m_Padding[1] = m_Padding[0] = false;
    }
    //----------------------------------------------------------------------
    // Cut from that, move to empty this
    CMaaDList(T* pFirst, T* pLast, bool AutoDeleteItems = false) noexcept
    :   Head(eNotInit)
    {
        if (!pFirst || !pLast)
        {
            Init();
        }
        else
        {
            pFirst->Prev->Next = pLast->Next;
            pLast->Next->Prev = pFirst->Prev;
            Head.Next = pFirst;
            Head.Prev = pLast;
            pFirst->Prev = pLast->Next = &Head;
        }
        m_AutoDeleteItems = AutoDeleteItems;
    }
    //----------------------------------------------------------------------
    /*virtual*/ constexpr ~CMaaDList() noexcept
    {
        if (m_AutoDeleteItems)
        {
            RemoveAll();
        }
    }
    //----------------------------------------------------------------------
    constexpr void Init() noexcept
    {
        Head.Prev = Head.Next = &Head;
    }
    constexpr bool IsEmpty() const noexcept
    {
        return Head.Next == &Head;
    }
    constexpr bool IsNotEmpty() const noexcept
    {
        return Head.Next != &Head;
    }
    //----------------------------------------------------------------------
#ifndef __unix__
     //#define CMaaDLink__ CMaaDLink::
#define CMaaDLink__
#else
#define CMaaDLink__
#endif
    //----------------------------------------------------------------------
    //virtual void Delete ( T * p ) const noexcept
    //{
    //    delete p;
    //}
    //----------------------------------------------------------------------
    /*
    void Clear() noexcept
    {
        for (T* p; (p = GetFromFront()); )
        {
            Delete(p);
        }
    }
    */
    //----------------------------------------------------------------------
    constexpr void RemoveAll() noexcept
    {
        T* p;
        while ((p = GetFromFront()))
        {
            delete p;
        }
    }
    //----------------------------------------------------------------------
    constexpr void AddAtBack(T* That) noexcept
    {
        That->CMaaDLink__ Prev = Head.Prev;
        That->CMaaDLink__ Next = &Head;
        Head.Prev = That;
        That->CMaaDLink__ Prev->CMaaDLink__ Next = That;
    }
    //----------------------------------------------------------------------
    constexpr void AddAtFront(T* That) noexcept
    {
        That->CMaaDLink__ Next = Head.Next;
        That->CMaaDLink__ Prev = &Head;
        Head.Next = That;
        That->CMaaDLink__ Next->Prev = That;
    }
    //----------------------------------------------------------------------
    constexpr void PopToFront(T* That) noexcept
    {
        Release(That);
        AddAtFront(That);
    }
    //----------------------------------------------------------------------
    constexpr void PopToBack(T* That) noexcept
    {
        Release(That);
        AddAtBack(That);
    }
    //----------------------------------------------------------------------
    constexpr T* GetFromFront() noexcept
    {
        CMaaDLink* That;
        if ((That = Head.Next) == &Head)
        {
            That = nullptr;
        }
        else
        {
            That->CMaaDLink__ Next->Prev = &Head;
            Head.Next = That->CMaaDLink__ Next;
            That->CMaaDLink__ Prev = That->CMaaDLink__ Next = nullptr;
        }
        return (T*)That;
    }
    //----------------------------------------------------------------------
    constexpr T* GetFromBack() noexcept
    {
        CMaaDLink* That;
        if ((That = Head.Prev) == &Head)
        {
            That = nullptr;
        }
        else
        {
            That->CMaaDLink__ Prev->Next = &Head;
            Head.Prev = That->CMaaDLink__ Prev;
            That->CMaaDLink__ Next = That->CMaaDLink__ Prev = nullptr;
        }
        return (T*)That;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtFront() const noexcept
    {
        return (Head.Next == &Head) ? nullptr : (T*)Head.Next;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtBack() const noexcept
    {
        return (Head.Prev == &Head) ? nullptr : (T*)Head.Prev;
    }
    constexpr bool IsCount1or0() const noexcept
    {
        return (Head.Next == Head.Prev);
    }
    constexpr bool IsCount1() const noexcept
    {
        return (Head.Next == Head.Prev && Head.Next != &Head);
    }
    //----------------------------------------------------------------------
    constexpr T* Next(T* That) const noexcept
    {
        return That->Next == &Head ? nullptr : (T*)That->Next;
    }
    //----------------------------------------------------------------------
    constexpr const T* Next(const T* That) const noexcept
    {
        return That->Next == &Head ? nullptr : (const T*)That->Next;
    }
    //----------------------------------------------------------------------
    constexpr T* Prev(T* That) const noexcept
    {
        return That->Prev == &Head ? nullptr : (T*)That->Prev;
    }
    //----------------------------------------------------------------------
    constexpr const T* Prev(const T* That) const noexcept
    {
        return That->Prev == &Head ? nullptr : (const T*)That->Prev;
    }
    //----------------------------------------------------------------------
    int CopyChainToBack(T* pFirst, T* pSkip = nullptr) noexcept
    {
        int Ret = 0;
        while (pFirst)
        {
            CMaaDLink* pThis = pFirst;
            pFirst = (T*)pFirst->CMaaDLink__ Next;
            if (pThis != pSkip)
            {
                AddAtBack((T*)pThis);
            }
            else
            {
                pThis->Init();
                Ret++;
            }
        }
        return Ret;
    }
    //----------------------------------------------------------------------
    static constexpr void Release(T* That) noexcept
    {
        if (That->CMaaDLink__ Prev)
        {
            That->CMaaDLink__ Prev->Next = That->CMaaDLink__ Next;
        }
        else
        {
            INT3 // I never pass here free item
        }
        if (That->CMaaDLink__ Next)
        {
            That->CMaaDLink__ Next->Prev = That->CMaaDLink__ Prev;
        }
        else
        {
            INT3 // I never pass here free item
        }
        That->CMaaDLink__ Next = That->CMaaDLink__ Prev = nullptr; // but it is not nessesary to perform initializations
    }
    //----------------------------------------------------------------------
    static constexpr void ReleaseSafe(T* That) noexcept
    {
        if (That->CMaaDLink__ Prev)
        {
            That->CMaaDLink__ Prev->Next = That->CMaaDLink__ Next;
        }
        else
        {
            // safe
        }
        if (That->CMaaDLink__ Next)
        {
            That->CMaaDLink__ Next->Prev = That->CMaaDLink__ Prev;
        }
        else
        {
            // safe
        }
        That->CMaaDLink__ Next = That->CMaaDLink__ Prev = nullptr; // but it is not nessesary to perform initializations
    }
    //----------------------------------------------------------------------
#if 0
    void Remove(T* That) noexcept
    {
        Release(That);
    }
#endif
    //----------------------------------------------------------------------
    //static void InsertAfrer ( T * That, T * After )
    //{
    //     That -> CMaaDLink__ Next = After -> CMaaDLink__ Next;
    //     That -> CMaaDLink__ Prev = After;
    //     That -> CMaaDLink__ Next -> Prev = That;
    //}
    //----------------------------------------------------------------------
    int GetCount() const noexcept
    {
        int c = 0;
        T* p;
        for (p = LookAtFront(); p; p = this->Next(p))
        {
            c++;
        }
        return c;
    }
    int GetItemCount() const noexcept { return GetCount(); }
    constexpr void MoveThatToTheEnd(CMaaDList < T >& That) noexcept
    {
        if (!That.IsEmpty())
        {
            T* F = (T*)That.Head.Next;
            T* L = (T*)That.Head.Prev;
            T* m = (T*)Head.Prev;
            m->Next = F;
            F->Prev = m;
            L->Next = &Head;
            Head.Prev = L;
            That.Init();
        }
    }
    constexpr void MoveThatToTheFront(CMaaDList < T >& That) noexcept
    {
        if (!That.IsEmpty())
        {
            T* F = (T*)That.Head.Next;
            T* L = (T*)That.Head.Prev;
            T* m = (T*)Head.Next;
            m->Prev = L;
            L->Next = m;
            F->Prev = &Head;
            Head.Next = F;
            That.Init();
        }
    }
    constexpr void MoveAfter(T* pItem, T* pFirst, T* pLast) noexcept
    {
        if (!pItem)
        {
            CMaaDList<T> tmp(pFirst, pLast);
            MoveThatToTheEnd(tmp);
        }
        else
        {
            pFirst->Prev->Next = pLast->Next;
            pLast->Next->Prev = pFirst->Prev;
            pFirst->Prev = pItem;
            pLast->Next = pItem->Next;
            pItem->Next->Prev = pLast;
            pItem->Next = pFirst;
        }
    }
    constexpr void MoveBefore(T* pItem, T* pFirst, T* pLast) noexcept
    {
        if (!pItem)
        {
            CMaaDList<T> tmp(pFirst, pLast);
            MoveThatToTheFront(tmp);
        }
        else
        {
            pFirst->Prev->Next = pLast->Next;
            pLast->Next->Prev = pFirst->Prev;
            pLast->Next = pItem;
            pFirst->Prev = pItem->Prev;
            pItem->Prev->Next = pFirst;
            pItem->Prev = pLast;
        }
    }
    constexpr bool MoveUp(T* That) noexcept
    {
        T* Neighbor = Prev(That);
        if (!Neighbor)
        {
            return false;
        }
        Release(That);
        That->InsertBefore(Neighbor);
        return true;
    }
    constexpr bool MoveDown(T* That) noexcept
    {
        T* Neighbor = Next(That);
        if (!Neighbor)
        {
            return false;
        }
        Release(That);
        That->InsertAfter(Neighbor);
        return true;
    }
    constexpr void Swap(CMaaDList < T >& That, bool bAdSwap = false) noexcept
    {
        CMaaDList < T > tmp;
        tmp.MoveThatToTheFront(That);
        That.MoveThatToTheFront(*this);
        MoveThatToTheFront(tmp);
        if (bAdSwap)
        {
            CMaaSwap(m_AutoDeleteItems, That.m_AutoDeleteItems);
        }
    }
#undef CMaaDLink__
};
//---------------------------------------------------------------------------
template < class T > class CMaaDListAD : public CMaaDList < T >
{
public:
    constexpr CMaaDListAD() noexcept
    :   CMaaDList < T >(true) // Auto deleting list items from destructor by default
    {
    }
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct CMaaSLink
{
    CMaaSLink* Next; // s-list

    // it is not need to initialize links before caling AddAtXxxx ()
    constexpr void Init() noexcept
    {
        Next = nullptr;
    }
    /*
    void InsertAfter(CMaaSLink* That) noexcept  // .... That, this, ....
    {
        Next = That->Next;
        That->Next = this;
    }
    */
    /*
    --
    void InsertBefore(CMaaSLink* That) noexcept  // .... this, That, ....
    {
        //--// Next = That;
        //--// That->Next = this;
    }
    */
    constexpr CMaaSLink() noexcept
    {
        Init();
    }
} GCC_PACKED;
//---------------------------------------------------------------------------
template < class T > struct CMaaSLink_Item : public CMaaSLink
{
    T m_Item;
    constexpr operator T& () noexcept
    {
        return m_Item;
    }
};
//---------------------------------------------------------------------------
template < class T > struct CMaaSLink_pItem : public CMaaSLink
{
    T* m_pItem;
    constexpr operator T*& () noexcept
    {
        return m_pItem;
    }
    constexpr T* operator -> () noexcept
    {
        return m_pItem;
    }
    constexpr T* operator = (T* ptr) noexcept
    {
        return m_pItem = ptr;
    }
};
//---------------------------------------------------------------------------
template < class T > struct CMaaSLink_pItemAD : public CMaaSLink_pItem < T >
{
    constexpr CMaaSLink_pItemAD() noexcept
    {
        CMaaSLink_pItem < T >::m_pItem = nullptr;
    }
    constexpr ~CMaaSLink_pItemAD() noexcept
    {
        delete CMaaSLink_pItem < T >::m_pItem;
    }
    constexpr T* operator = (T* ptr) noexcept
    {
        return CMaaSLink_pItem < T >::operator = (ptr);
    }
};
//---------------------------------------------------------------------------
template < class T > class CMaaSList
{
    CMaaSLink Head;
public:
    bool m_AutoDeleteItems, m_Padding[3];
    //----------------------------------------------------------------------
    constexpr CMaaSList(bool AutoDeleteItems = false) noexcept
    :   m_AutoDeleteItems{ AutoDeleteItems },
        m_Padding{}
    {
    }
    //----------------------------------------------------------------------
    /*virtual*/ constexpr ~CMaaSList() noexcept
    {
        if (m_AutoDeleteItems)
        {
            RemoveAll();
        }
    }
    //----------------------------------------------------------------------
    constexpr void Init() noexcept
    {
        Head.Next = nullptr;
    }
    constexpr bool IsEmpty() const noexcept
    {
        return !Head.Next;
    }
    constexpr bool IsNotEmpty() const noexcept
    {
        return Head.Next;
    }
    //----------------------------------------------------------------------
#ifndef __unix__
//#define CMaaSLink__ CMaaSLink::
#define CMaaSLink__
#else
#define CMaaSLink__
#endif
    //----------------------------------------------------------------------
    constexpr void RemoveAll() noexcept
    {
        T* p;
        while ((p = GetFromFront()))
        {
            delete p;
        }
    }
    //----------------------------------------------------------------------
    constexpr void AddAtFront(T* That) noexcept
    {
        That->CMaaSLink__ Next = Head.Next;
        Head.Next = That;
    }
    //----------------------------------------------------------------------
    // That is free, After is in list
    static constexpr void InsertAfter(CMaaSLink* That, CMaaSLink* After) noexcept  // .... this, That, ....
    {
        That->CMaaSLink__ Next = After->CMaaSLink__ Next;
        After->CMaaSLink__ Next = That;
    }
    //----------------------------------------------------------------------
    constexpr T* GetFromFront() noexcept
    {
        CMaaSLink* That = Head.Next;
        if (That)
        {
            Head.Next = That->CMaaSLink__ Next;
            That->CMaaSLink__ Next = nullptr;
        }
        return (T*)That;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtFront() const noexcept
    {
        return (T*)Head.Next;
    }
    //----------------------------------------------------------------------
    static constexpr T* Next(T* That) noexcept
    {
        return (T*)That->CMaaSLink__ Next;
    }
    //----------------------------------------------------------------------
    static constexpr const T* Next(const T* That) noexcept
    {
        return (const T*)That->CMaaSLink__ Next;
    }
    //----------------------------------------------------------------------
    constexpr T* ExtractChain() noexcept
    {
        T* Ret = LookAtFront();
        Init();
        return Ret;
    }
    //----------------------------------------------------------------------
    int GetCount() const noexcept
    {
        int c = 0;
        T* p;
        for (p = LookAtFront(); p; p = this->Next(p))
        {
            c++;
        }
        return c;
    }
    int GetItemCount() const noexcept { return GetCount(); }
    constexpr void Swap(CMaaSList < T >& That, bool bAdSwap = false) noexcept
    {
        CMaaSwap(Head.Next, That.Head.Next);
        if (bAdSwap)
        {
            CMaaSwap(m_AutoDeleteItems, That.m_AutoDeleteItems);
        }
    }
#undef CMaaSLink__
};
//---------------------------------------------------------------------------
template < class T > class CMaaSListAD : public CMaaSList < T >
{
public:
    constexpr CMaaSListAD() noexcept
    :   CMaaSList < T >(true) // Auto deleting list items from destructor by default
    {
    }
};

//---------------------------------------------------------------------------
// CMaaSList2 with tail access and insert, 2 links
//---------------------------------------------------------------------------
template < class T > class CMaaSList2
{
    CMaaSLink Head, Tail;
public:
    bool m_AutoDeleteItems, m_Padding[3];
    //----------------------------------------------------------------------
    constexpr CMaaSList2(bool AutoDeleteItems = false) noexcept
    :   m_AutoDeleteItems{ AutoDeleteItems },
        m_Padding{}
    {
    }
    //----------------------------------------------------------------------
    /*virtual*/ constexpr ~CMaaSList2() noexcept
    {
        if (m_AutoDeleteItems)
        {
            RemoveAll();
        }
    }
    //----------------------------------------------------------------------
    constexpr void Init() noexcept
    {
        Tail.Next = Head.Next = nullptr;
    }
    constexpr bool IsEmpty() const noexcept
    {
        return !Head.Next;
    }
    constexpr bool IsNotEmpty() const noexcept
    {
        return Head.Next;
    }
    //----------------------------------------------------------------------
#ifndef __unix__
//#define CMaaSLink__ CMaaSLink::
#define CMaaSLink__
#else
#define CMaaSLink__
#endif
    //----------------------------------------------------------------------
    constexpr void RemoveAll() noexcept
    {
        T* p;
        while ((p = GetFromFront()))
        {
            delete p;
        }
    }
    //----------------------------------------------------------------------
    constexpr void AddAtBack(T* That) noexcept
    {
        if (Tail.Next)
        {
            // >= 1 elements exists
            Tail.Next = Tail.Next->Next = That;
            That->CMaaSLink__ Next = nullptr;
        }
        else
        {
            // empty list
            Tail.Next = Head.Next = That;
            That->CMaaSLink__ Next = nullptr;
        }
    }
    //----------------------------------------------------------------------
    constexpr void AddAtFront(T* That) noexcept
    {
        if (Head.Next)
        {
            That->CMaaSLink__ Next = Head.Next;
            Head.Next = That;
        }
        else
        {
            // empty list
            Tail.Next = Head.Next = That;
            That->CMaaSLink__ Next = nullptr;
        }
    }
    //----------------------------------------------------------------------
    // That is free, After is in list
    constexpr void InsertAfter(CMaaSLink* That, CMaaSLink* After) noexcept  // .... this, That, ....
    {
        That->CMaaSLink__ Next = After->CMaaSLink__ Next;
        After->CMaaSLink__ Next = That;
        if (Tail.Next == After)
        {
            Tail.Next = That;
        }
    }
    //----------------------------------------------------------------------
    constexpr T* GetFromFront() noexcept
    {
        CMaaSLink* That = Head.Next;
        if (That)
        {
            if (!(Head.Next = That->CMaaSLink__ Next))
            {
                Tail.Next = nullptr;
            }
            That->CMaaSLink__ Next = nullptr;
        }
        return (T*)That;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtFront() const noexcept
    {
        return (T*)Head.Next;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtBack() const noexcept
    {
        return (T*)Tail.Next;
    }
    //----------------------------------------------------------------------
    static constexpr T* Next(T* That) noexcept
    {
        return (T*)That->CMaaSLink__ Next;
    }
    //----------------------------------------------------------------------
    static constexpr const T* Next(const T* That) noexcept
    {
        return (const T*)That->CMaaSLink__ Next;
    }
    //----------------------------------------------------------------------
    constexpr T* ExtractChain() noexcept
    {
        T* Ret = LookAtFront();
        Init();
        return Ret;
    }
    //----------------------------------------------------------------------
    int CopyChainToBack(T* pFirst, T* pSkip = nullptr) noexcept
    {
        int Ret = 0;
        while (pFirst)
        {
            CMaaSLink* pThis = pFirst;
            pFirst = (T*)pFirst->CMaaSLink__ Next;
            if (pThis != pSkip)
            {
                AddAtBack((T*)pThis);
            }
            else
            {
                pThis->Init();
                Ret++;
            }
        }
        return Ret;
    }
    //----------------------------------------------------------------------
    void ReleaseSlow(T* pThat) noexcept
    {
        T* c = ExtractChain();
        CopyChainToBack(c, pThat);
    }
    //----------------------------------------------------------------------
    int GetCount() const noexcept
    {
        int c = 0;
        T* p;
        for (p = LookAtFront(); p; p = this->Next(p))
        {
            c++;
        }
        return c;
    }
    int GetItemCount() const noexcept { return GetCount(); }
    constexpr void MoveThatToTheEnd(CMaaSList2 < T >& That) noexcept
    {
        if (!That.IsEmpty())
        {
            if (IsEmpty())
            {
                Head.Next = That.Head.Next;
                Tail.Next = That.Tail.Next;
            }
            else
            {
                Tail.Next->CMaaSLink__ Next = That.Head.Next;
                Tail.Next = That.Tail.Next;
            }
            That.Init();
        }
    }
    constexpr void MoveThatToTheFront(CMaaSList2 < T >& That) noexcept
    {
        if (!That.IsEmpty())
        {
            if (IsEmpty())
            {
                Head.Next = That.Head.Next;
                Tail.Next = That.Tail.Next;
            }
            else
            {
                That.Tail.Next->CMaaSLink__ Next = Head.Next;
                Head.Next = That.Head.Next;
            }
            That.Init();
        }
    }
    constexpr void Swap(CMaaSList2 < T >& That, bool bAdSwap = false) noexcept
    {
        CMaaSList2 < T > tmp;
        tmp.MoveThatToTheFront(That);
        That.MoveThatToTheFront(*this);
        MoveThatToTheFront(tmp);
        if (bAdSwap)
        {
            CMaaSwap(m_AutoDeleteItems, That.m_AutoDeleteItems);
        }
    }
#undef CMaaSLink__
};
//---------------------------------------------------------------------------
template < class T > class CMaaSList2AD : public CMaaSList2 < T >
{
public:
    constexpr CMaaSList2AD() noexcept
    :   CMaaSList2 < T >(true) // Auto deleting list items from destructor by default
    {
    }
};

//---------------------------------------------------------------------------
// CMaaSList2 with tail access and insert, 1 link
//---------------------------------------------------------------------------
template < class T > class CMaaSList1
{
    CMaaSLink Tail;
public:
    bool m_AutoDeleteItems, m_Padding[3];
    //----------------------------------------------------------------------
    constexpr CMaaSList1(bool AutoDeleteItems = false) noexcept
    :   m_AutoDeleteItems{ AutoDeleteItems },
        m_Padding{}
    {
    }
    //----------------------------------------------------------------------
    /*virtual*/ constexpr ~CMaaSList1() noexcept
    {
        if (m_AutoDeleteItems)
        {
            RemoveAll();
        }
    }
    //----------------------------------------------------------------------
    constexpr void Init() noexcept
    {
        Tail.Next = nullptr;
    }
    constexpr bool IsEmpty() const noexcept
    {
        return !Tail.Next;
    }
    constexpr bool IsNotEmpty() const noexcept
    {
        return Tail.Next;
    }
    //----------------------------------------------------------------------
#ifndef __unix__
//#define CMaaSLink__ CMaaSLink::
#define CMaaSLink__
#else
#define CMaaSLink__
#endif
    //----------------------------------------------------------------------
    void RemoveAll() noexcept
    {
        T* p;
        while ((p = GetFromFront()))
        {
            delete p;
        }
    }
    //----------------------------------------------------------------------
    constexpr void AddAtBack(T* That) noexcept
    {
        if (Tail.Next)
        {
            // >= 1 elements exists
            That->CMaaSLink__ Next = Tail.Next->Next;
            Tail.Next = Tail.Next->Next = That;
        }
        else
        {
            // empty list
            Tail.Next = That;
            That->CMaaSLink__ Next = That;
        }
    }
    //----------------------------------------------------------------------
    constexpr void AddAtFront(T* That) noexcept
    {
        if (Tail.Next)
        {
            That->CMaaSLink__ Next = Tail.Next->Next;
            Tail.Next->Next = That;
        }
        else
        {
            // empty list
            Tail.Next = That;
            That->CMaaSLink__ Next = That;
        }
    }
    //----------------------------------------------------------------------
    // That is free, After is in list
    constexpr void InsertAfter(CMaaSLink* That, CMaaSLink* After) noexcept  // .... this, That, ....
    {
        That->CMaaSLink__ Next = After->CMaaSLink__ Next;
        After->CMaaSLink__ Next = That;
        if (Tail.Next == After)
        {
            Tail.Next = That;
        }
    }
    //----------------------------------------------------------------------
    constexpr T* GetFromFront() noexcept
    {
        if (Tail.Next)
        {
            CMaaSLink* That = Tail.Next->Next;
            if (That->CMaaSLink__ Next == That)
            {
                Tail.Next = nullptr;
            }
            else
            {
                Tail.Next->Next = That->CMaaSLink__ Next;
            }
            That->CMaaSLink__ Next = nullptr;
            return (T*)That;
        }
        return (T*)nullptr;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtFront() const noexcept
    {
        return Tail.Next ? (T*)Tail.Next->Next : nullptr;
    }
    //----------------------------------------------------------------------
    constexpr T* LookAtBack() const noexcept
    {
        return (T*)Tail.Next;
    }
    //----------------------------------------------------------------------
    constexpr T* Next(T* That) const noexcept
    {
        return That->CMaaSLink__ Next != Tail.Next->Next ? (T*)That->CMaaSLink__ Next : nullptr;
    }
    //----------------------------------------------------------------------
    constexpr const T* Next(const T* That) const noexcept
    {
        return That->CMaaSLink__ Next != Tail.Next->Next ? (const T*)That->CMaaSLink__ Next : nullptr;
    }
    //----------------------------------------------------------------------
    constexpr T* ExtractChain() noexcept
    {
        T* Ret = LookAtFront();
        if (Ret)
        {
            Tail.Next->CMaaSLink__ Next = nullptr;
        }
        Init();
        return Ret;
    }
    //----------------------------------------------------------------------
    int CopyChainToBack(T* pFirst, T* pSkip = nullptr) noexcept
    {
        int Ret = 0;
        while (pFirst)
        {
            CMaaSLink* pThis = pFirst;
            pFirst = (T*)pFirst->CMaaSLink__ Next;
            if (pThis != pSkip)
            {
                AddAtBack((T*)pThis);
            }
            else
            {
                pThis->Init();
                Ret++;
            }
        }
        return Ret;
    }
    //----------------------------------------------------------------------
    void ReleaseSlow(T* pThat) noexcept
    {
        T* c = ExtractChain();
        CopyChainToBack(c, pThat);
    }
    //----------------------------------------------------------------------
    int GetCount() const noexcept
    {
        int c = 0;
        T* p;
        for (p = LookAtFront(); p; p = this->Next(p))
        {
            c++;
        }
        return c;
    }
    int GetItemCount() const noexcept { return GetCount(); }
    constexpr void MoveThatToTheEnd(CMaaSList1 < T >& That) noexcept
    {
        if (!That.IsEmpty())
        {
            if (IsEmpty())
            {
                Tail.Next = That.Tail.Next;
            }
            else
            {
                CMaaSLink* F = Tail.Next->CMaaSLink__ Next;
                Tail.Next->CMaaSLink__ Next = That.Tail.Next->CMaaSLink__ Next;
                That.Tail.Next->CMaaSLink__ Next = F;
                Tail.Next = That.Tail.Next;
            }
            That.Init();
        }
    }
    constexpr void MoveThatToTheFront(CMaaSList1 < T >& That) noexcept
    {
        if (!That.IsEmpty())
        {
            if (IsEmpty())
            {
                Tail.Next = That.Tail.Next;
            }
            else
            {
                CMaaSLink* m = That.Tail.Next->CMaaSLink__ Next;
                That.Tail.Next->CMaaSLink__ Next = Tail.Next->CMaaSLink__ Next;
                Tail.Next->CMaaSLink__ Next = m;
            }
            That.Init();
        }
    }
    constexpr void Swap(CMaaSList1 < T >& That, bool bAdSwap = false) noexcept
    {
        CMaaSList1 < T > tmp;
        tmp.MoveThatToTheFront(That);
        That.MoveThatToTheFront(*this);
        MoveThatToTheFront(tmp);
        if (bAdSwap)
        {
            CMaaSwap(m_AutoDeleteItems, That.m_AutoDeleteItems);
        }
    }
#undef CMaaSLink__
};
//---------------------------------------------------------------------------
template < class T > class CMaaSList1AD : public CMaaSList1 < T >
{
public:
    constexpr CMaaSList1AD() noexcept
    :   CMaaSList1 < T >(true) // Auto deleting list items from destructor by default
    {
    }
};

//---------------------------------------------------------------------------
#if 1
//---------------------------------------------------------------------------
// old version of CMaaPtr, CMaaPtrAE is faster
//---------------------------------------------------------------------------
#ifndef _MaaRF_INTERNAL_BUILD
//#if 0
template < class T > class CMaaPtr
{
protected:
    T* m_Ptr;
    int3264 m_MaxIndex, m_TotalItems;
    int m_xThrow;

public:

    int GetThrowParameter() const noexcept
    {
        return m_xThrow;
    }
    void Swap(CMaaPtr& That) noexcept
    {
        CMaaSwap(m_Ptr, That.m_Ptr);
        CMaaSwap(m_MaxIndex, That.m_MaxIndex);
        CMaaSwap(m_xThrow, That.m_xThrow);
        CMaaSwap(m_TotalItems, That.m_TotalItems);
    }
    // This constructor creates MaxIndex numbers of elements of class T.
    // Under Windows makes exeption if no free memory available.
    CMaaPtr(CMaaPtr& x)
    :   m_Ptr(nullptr),
        m_MaxIndex(0),
        m_TotalItems(0),
        m_xThrow(x.m_xThrow)
    {
        CMaaPtr<T> tmp(x.m_MaxIndex, x.m_xThrow);
        if  (tmp.IsValid())
        {
            //memcpy(tmp, x, x.MaxIndex() * sizeof(T));
            int3264 m = x.m_MaxIndex;
            for (int3264 i = 0; i < m; i++)
            {
                tmp.m_Ptr[i] = x.m_Ptr[i];
            }
        }
        tmp.Swap(*this);
    }

    void operator += (const CMaaPtr& x)
    {
        CMaaPtr<T> tmp((size_t)m_MaxIndex + (size_t)x.m_MaxIndex, m_xThrow);
        if  (tmp.IsValid())
        {
            //          memcpy(tmp, *this, MaxIndex() * sizeof(T));
            int3264 i, m = m_MaxIndex;
            for (i = 0; i < m; i++)
            {
                tmp.m_Ptr[i] = m_Ptr[i];
            }
            //          memcpy(MaxIndex + tmp, x, x.MaxIndex() * sizeof(T));
            int3264 m2 = x.m_MaxIndex;
            for (i = 0; i < m2; i++)
            {
                tmp.m_Ptr[i + m] = x.m_Ptr[i];
            }
        }
        tmp.Swap(*this);
    }

    void operator += (const T& x)
    {
        int3264 m = m_MaxIndex;
        if (CheckExpand(m + 1))
        {
            // m_MaxIndex === m + 1;
            if constexpr (noexcept(m_Ptr[m] = x))
            {
                m_Ptr[m] = x;
            }
            else
            {
                m_MaxIndex = m;
                m_Ptr[m] = x; // throwable
                m_MaxIndex = m + 1;
            }
            //return true;
        }
        //return false;
    }

    void operator += (T& x)
    {
        int3264 m = m_MaxIndex;
        if (CheckExpand(m + 1))
        {
            // m_MaxIndex === m + 1;
            if constexpr (noexcept(m_Ptr[m] = x))
            {
                m_Ptr[m] = x;
            }
            else
            {
                m_MaxIndex = m;
                m_Ptr[m] = x; // throwable
                m_MaxIndex = m + 1;
            }
            //return true;
        }
        //return false;
    }

    CMaaPtr<T>& operator = (const CMaaPtr<T>& That)
    {
        CMaaPtr<T> tmp(That.m_MaxIndex, m_xThrow);
        if  (tmp.IsValid())
        {
            int3264 m = That.m_MaxIndex;
            for (int3264 i = 0; i < m; i++)
            {
                tmp.m_Ptr[i] = That.m_Ptr[i];
            }
        }
        tmp.Swap(*this);
        return *this;
    }

    void append(T x)
    {
        int3264 m = m_MaxIndex;
        if (CheckExpand(m + 1))
        {
            // m_MaxIndex === m + 1;
            if constexpr (noexcept(m_Ptr[m] = x))
            {
                m_Ptr[m] = x;
            }
            else
            {
                m_MaxIndex = m;
                m_Ptr[m] = x; // throwable
                m_MaxIndex = m + 1;
            }
            //return true;
        }
        //return false;
    }

    CMaaPtr(size_t MaxIndex = 0, int xThrow = 0)
    :   m_Ptr(nullptr),
        m_MaxIndex((int3264)MaxIndex),
        m_TotalItems((int3264)MaxIndex),
        m_xThrow(xThrow)
    {
        if  (MaxIndex)
        {
#ifdef _MaaRF_INTERNAL_BUILD
            m_Ptr = /*MaxIndex < INT_MAX &&*/ MaxIndex < SIZE_MAX / sizeof(T) ? TL_NEW T[MaxIndex] : nullptr;
#else
            try
            {
                m_Ptr = /*MaxIndex < INT_MAX &&*/ MaxIndex < SIZE_MAX / sizeof(T) ? TL_NEW T[MaxIndex] : nullptr;
            }
            catch(...)
            {
            }
#endif
            if  (!m_Ptr && MaxIndex)
            {
                INT3;
#ifndef __unix__
                OUT_DEBUG_STRING("MAARF:Memory allocation failed\r\n");
#endif
                m_MaxIndex = 0;
                m_TotalItems = m_TotalItems ? m_TotalItems : 1;
                if  (xThrow > 0)
                {
                    CMaaPtr_THROW1;
                }
                if  (xThrow == 0)
                {
                    CMaaPtr_THROW;
                }
            }
        }
    }

    /*
    CMaaPtr() noexcept
    :   m_Ptr(nullptr),
        m_MaxIndex(0),
        m_TotalItems(0),
        m_xThrow(-1)
    {
    }
    */

    CMaaPtr(size_t MaxIndex, eNoExcept_) noexcept
    :   m_Ptr(nullptr),
        m_MaxIndex((int3264)MaxIndex),
        m_TotalItems((int3264)MaxIndex),
        m_xThrow(-1)
    {
        if (MaxIndex)
        {
            try
            {
                m_Ptr = /*MaxIndex < INT_MAX &&*/ MaxIndex < SIZE_MAX / sizeof(T) ? TL_NEW T[MaxIndex] : nullptr;
            }
            catch (...)
            {
            }
            if (!m_Ptr && MaxIndex)
            {
                //INT3;
#ifndef __unix__
                //OUT_DEBUG_STRING("MAARF:Memory allocation failed\r\n");
#endif
                m_MaxIndex = 0;
                m_TotalItems = m_TotalItems ? m_TotalItems : 1;
            }
        }
    }

    virtual bool CheckResize(size_t MaxN) // MaxN - TL_NEW items count required
    {
        if (IsValid() && MaxN <= (size_t)m_TotalItems)
        {
            if ((int3264)MaxN > m_MaxIndex)
            {
                m_MaxIndex = (int3264)MaxN;
            }
            return true;
        }
        const size_t N = 2 * (size_t)m_TotalItems >= MaxN ? 2 * (size_t)m_TotalItems : MaxN;
        CMaaPtr<T> m2(N, m_xThrow);
        if (!m2.IsValid())
        {
            return false;
        }
        m2.m_MaxIndex = (int3264)MaxN;
        Swap(m2);
        return true;
    }

    virtual bool CheckExpand(size_t MaxN) // MaxN - TL_NEW items count required
    {
        if (IsValid() && MaxN <= (size_t)m_TotalItems)
        {
            if ((int3264)MaxN > m_MaxIndex)
            {
                m_MaxIndex = (int3264)MaxN;
            }
            return true;
        }
        const size_t N = 2 * (size_t)m_TotalItems >= MaxN ? 2 * (size_t)m_TotalItems : MaxN;
        CMaaPtr<T> m2(N, m_xThrow);
        if (!m2.IsValid())
        {
            return false;
        }
        int3264 m = m_MaxIndex;
        for (int3264 i = 0; i < m; i++)
        {
            m2.m_Ptr[i] = m_Ptr[i];
        }
        m2.m_MaxIndex = (int3264)MaxN;
        Swap(m2);
        return true;
    }

    virtual ~CMaaPtr() noexcept
    {
        delete[] m_Ptr;
    }

    virtual bool IsValid() const noexcept
    {
        return m_Ptr || !m_TotalItems;
    }

    void Empty() noexcept
    {
        m_MaxIndex = 0;

        if (!IsValid())
        {
            m_TotalItems = 0;
        }
    }

    operator T* () noexcept
    {
        return m_Ptr;
    }

    T& operator * () const noexcept
    {
        return *m_Ptr;
    }

    T& operator [] (int3264 Index) noexcept
    {
        return m_Ptr[Index];
    }
    const T& operator [] (int3264 Index) const noexcept
    {
        return m_Ptr[Index];
    }
    /*
    T& operator [] (size_t Index) noexcept
    {
        return m_Ptr[Index];
    }
    const T& operator [] (size_t Index) const noexcept
    {
        return m_Ptr[Index];
    }
    */
    T& at(int3264 Index) noexcept
    {
        return m_Ptr[Index];
    }
    const T& at(int3264 Index) const noexcept
    {
        return m_Ptr[Index];
    }
    size_t MaxIndex() const noexcept
    {
        return m_MaxIndex;
    }
    int3264 count() const noexcept
    {
        return m_MaxIndex;
    }
    int3264 size() const noexcept
    {
        return (int3264)MaxIndex();
    }
    size_t Size() const noexcept
    {
        return m_MaxIndex * sizeof(T);
    }
    void Zero() noexcept
    {
        if  (m_Ptr)
        {
            Memzero(m_Ptr, (size_t)sizeof(T) * m_MaxIndex);
        }
    }
    void fill(const T& value, int3264 size = -1)
    {
        if  (m_Ptr && size <= m_MaxIndex)
        {
            for (int3264 i = 0; i < size; i++)
            {
                m_Ptr[i] = value;
            }
        }
    }
    virtual bool Resize(int3264 n)
    {
        CMaaPtr < T > tmp(n, m_xThrow);
        if  (!tmp.IsValid())
        {
            return false;
        }
        const int3264 nn = (int)m_MaxIndex < n ? (int)m_MaxIndex : n;
        for (int3264 i = 0; i < nn; i++)
        {
            tmp[i] = (*this)[i];
        }
        Swap(tmp);
        return true;
    }
    int Get_xThrow() const noexcept
    {
        return m_xThrow;
    }
    bool resize(int3264 n)
    {
        CMaaPtr < T > tmp(n, m_xThrow);
        if  (!tmp.IsValid())
        {
            return false;
        }
        /*
        int nn = (int)m_MaxIndex < n ? (int)m_MaxIndex : n;
        for (int i = 0; i < nn; i++)
        {
            tmp[i] = (*this)[i];
        }
        */
        //tmp.MaxIndex = nn;
        Swap(tmp);
        return true;
    }
    static void Delete(T* ptr) noexcept
    {
        delete[] ptr;
    }
    T* Extract() noexcept
    {
        T* ptr = m_Ptr;
        m_Ptr = nullptr;
        return ptr;
    }
};
//---------------------------------------------------------------------------
// Automatic expandable CMaaPtr
template < class T > class CMaaPtrAE : public CMaaPtr < T >
{
public:
    CMaaPtrAE(size_t MaxIndex, int xThrow = 0)
    :   CMaaPtr < T >(MaxIndex, xThrow)
    {
    }
    CMaaPtrAE(size_t MaxIndex, eNoExcept_) noexcept
    :   CMaaPtr < T >(MaxIndex, eNoExcept)
    {
    }
    CMaaPtrAE(CMaaPtrAE<T>& That)
    :   CMaaPtr < T >(That.m_MaxIndex, That.m_xThrow)
    {
        if  (CMaaPtr < T >::IsValid() && That.IsValid())
        {
            T* pThis = (T*)*this;
            T* pThat = (T*)That;
            int3264 n = (int3264)CMaaPtr < T >::MaxIndex();
            for (int3264 i = 0; i < n; i++)
            {
                pThis[i] = pThat[i];
            }
        }
    }
    T& operator [] (int3264 Index)
    {
        if (Index < 0 || Index + 1 < 0 || !CMaaPtr < T >::CheckExpand(Index + 1))
        {
            static T t;
            return t;
        }
        return CMaaPtr < T >::m_Ptr[Index];
    }
    const T& operator [] (int3264 Index) const
    {
        int3264 m = (int3264)CMaaPtr < T >::MaxIndex();
        if  (Index < 0 || Index >= m)
        {
            ////CMaaPtr_THROW;
            static T t;
            return t;
        }
        return CMaaPtr < T >::m_Ptr[Index];
    }
    void Swap(CMaaPtrAE<T>& That) noexcept
    {
        CMaaPtr<T>::Swap(That);
    }
};
#endif //_MaaRF_INTERNAL_BUILD or 0
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// based on old version of CMaaPtr, CMaaPtrAE
// CMaaPtr_::CMaaPtr_(size_t MaxIndex) noexcept(xThrow < 0 || (!xThrow && noexcept(T())))
// bool CMaaPtr_::CheckExpand(size_t MaxN) noexcept((xThrow < 0 || (!xThrow && noexcept(T()))) && noexcept(m_Ptr[0] = m_Ptr[0]))
//---------------------------------------------------------------------------
template < class T, int xThrow > class CMaaPtr_
{
protected:
    T* m_Ptr;
    int3264 m_MaxIndex, m_TotalItems;

public:

    constexpr int GetThrowParameter() const noexcept
    {
        return xThrow;
    }
    void Swap(CMaaPtr_<T, xThrow>& That) noexcept
    {
        CMaaSwap(m_Ptr, That.m_Ptr);
        CMaaSwap(m_MaxIndex, That.m_MaxIndex);
        CMaaSwap(m_TotalItems, That.m_TotalItems);
    }

    // This constructor creates MaxIndex numbers of elements of class T.
    // Makes exeption if no free memory available.
    CMaaPtr_(size_t MaxIndex) noexcept(xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T)))
    :   m_Ptr(nullptr),
        m_MaxIndex((int3264)MaxIndex),
        m_TotalItems((int3264)MaxIndex)
    {
        if (MaxIndex)
        {
            // MaxIndex < INT_MAX &&
            if (MaxIndex < SIZE_MAX
#ifndef _WIN32
                / 2
#endif
                / sizeof(T))
            {
#ifdef _MaaRF_INTERNAL_BUILD
                m_Ptr = TL_NEW T[MaxIndex];
#else
                try
                {
                    m_Ptr = TL_NEW T[MaxIndex];
                }
                catch (const std::bad_alloc&)
                {
                    //std::cout << "Allocation failed: " << e.what() << '\n';
                    if constexpr (xThrow > 0)
                    {
                        CMaaPtr_THROW1;
                    }
                }
                catch (...)
                {
                    if constexpr (xThrow >= 0 && !std_is_nothrow_move_constructible0(T))
                    {
                        throw;
                    }
                }
#endif
            }
            if (!m_Ptr)
            {
                m_MaxIndex = 0;
                m_TotalItems = m_TotalItems ? m_TotalItems : 1;
                if constexpr (xThrow > 0)
                {
                    CMaaPtr_THROW1;
                }
            }
        }
    }

    // This constructor creates array of 0 elements of class T, noexcept
    constexpr CMaaPtr_() noexcept
    :   m_Ptr(nullptr),
        m_MaxIndex(0),
        m_TotalItems(0)
    {
    }

    CMaaPtr_(const CMaaPtr_<T, xThrow>& x) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, m_Ptr[0] = m_Ptr[0]))
    :   m_Ptr(nullptr),
        m_MaxIndex(0),
        m_TotalItems(0)
    {
        const int3264 m = x.m_MaxIndex;
        if (m)
        {
            if (x.IsValid())
            {
                CMaaPtr_<T, xThrow> tmp(m);
                if (tmp.IsValid())
                {
                    //memcpy(tmp, x, m * sizeof(T));
                    for (int3264 i = 0; i < m; i++)
                    {
                        tmp.m_Ptr[i] = x.m_Ptr[i];
                    }
                }
                tmp.Swap(*this);
            }
        }
    }

    void operator += (const CMaaPtr_<T, xThrow> &x) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, m_Ptr[0] = m_Ptr[0]))
    {
        if (x.IsValid())
        {
            CMaaPtr_<T, xThrow> tmp((size_t)m_MaxIndex + (size_t)x.m_MaxIndex);
            if (tmp.IsValid())
            {
                //          memcpy(tmp, *this, MaxIndex() * sizeof(T));
                int3264 i;
                const int3264 m = m_MaxIndex;
                for (i = 0; i < m; i++)
                {
                    tmp.m_Ptr[i] = m_Ptr[i];
                }
                //          memcpy(MaxIndex + tmp, x, x.MaxIndex() * sizeof(T));
                const int3264 m2 = x.m_MaxIndex;
                for (i = 0; i < m2; i++)
                {
                    tmp.m_Ptr[i + m] = x.m_Ptr[i];
                }
            }
            tmp.Swap(*this);
        }
    }

    void operator += (const T& x) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && noexcept(CheckExpand(m_MaxIndex + 1)) && std_is_nothrow_move_assignable(T, m_Ptr[0] = x))
    {
        const int3264 m = m_MaxIndex;
        if (CheckExpand(m + 1))
        {
            // m_MaxIndex === m + 1;
            if constexpr (noexcept(m_Ptr[m] = x))
            {
                m_Ptr[m] = x;
            }
            else
            {
                m_MaxIndex = m;
                m_Ptr[m] = x; // throwable
                m_MaxIndex = m + 1;
            }
            //return true;
        }
        //return false;
    }

    void operator += (T& x) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && noexcept(CheckExpand(m_MaxIndex + 1)) && std_is_nothrow_move_assignable(T, m_Ptr[0] = x))
    {
        const int3264 m = m_MaxIndex;
        if (CheckExpand(m + 1))
        {
            // m_MaxIndex === m + 1;
            if constexpr (noexcept(m_Ptr[m] = x))
            {
                m_Ptr[m] = x;
            }
            else
            {
                m_MaxIndex = m;
                m_Ptr[m] = x; // throwable
                m_MaxIndex = m + 1;
            }
            //return true;
        }
        //return false;
    }

    CMaaPtr_<T, xThrow>& operator = (const CMaaPtr_<T, xThrow>& That) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, m_Ptr[0] = m_Ptr[0]))
    {
        const int3264 m = That.m_MaxIndex;
        CMaaPtr_<T, xThrow> tmp(m);
        if (tmp.IsValid())
        {
            for (int3264 i = 0; i < m; i++)
            {
                tmp.m_Ptr[i] = That.m_Ptr[i];
            }
        }
        tmp.Swap(*this);
        return *this;
    }

    void append(T x) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && noexcept(CheckExpand(m_MaxIndex + 1)) && std_is_nothrow_move_assignable(T, m_Ptr[0] = x))
    {
        const int3264 m = m_MaxIndex;
        if (CheckExpand(m + 1))
        {
            // m_MaxIndex === m + 1;
            if constexpr (noexcept(m_Ptr[m] = x))
            {
                m_Ptr[m] = x;
            }
            else
            {
                m_MaxIndex = m;
                m_Ptr[m] = x; // throwable
                m_MaxIndex = m + 1;
            }
            //return true;
        }
        //return false;
    }

    /*virtual*/ bool CheckResize(size_t MaxN) noexcept(xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T)))
    {
        if (IsValid() && MaxN <= (size_t)m_TotalItems)
        {
            if ((int3264)MaxN > m_MaxIndex)
            {
                m_MaxIndex = (int3264)MaxN;
            }
            return true;
        }
        const size_t N = 2 * (size_t)m_TotalItems >= MaxN ? 2 * (size_t)m_TotalItems : MaxN;
        CMaaPtr_<T, xThrow> m2(N);
        if (!m2.IsValid())
        {
            return false;
        }
        m2.m_MaxIndex = (int3264)MaxN;
        Swap(m2);
        return true;
    }

    /*virtual*/ bool CheckExpand(size_t MaxN) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, m_Ptr[0] = m_Ptr[0]))
    {
        if (IsValid() && MaxN <= (size_t)m_TotalItems)
        {
            if ((int3264)MaxN > m_MaxIndex)
            {
                m_MaxIndex = (int3264)MaxN;
            }
            return true;
        }
        const size_t N = 2 * (size_t)m_TotalItems >= MaxN ? 2 * (size_t)m_TotalItems : MaxN;
        CMaaPtr_<T, xThrow> m2(N);
        if (!m2.IsValid())
        {
            return false;
        }
        const int3264 m = m_MaxIndex;
        for (int3264 i = 0; i < m; i++)
        {
            m2.m_Ptr[i] = m_Ptr[i];
        }
        m2.m_MaxIndex = (int3264)MaxN;
        Swap(m2);
        return true;
    }

    virtual constexpr ~CMaaPtr_() noexcept
    {
        delete[] m_Ptr;
    }

    /*virtual*/ bool IsValid() const noexcept
    {
        if constexpr (xThrow > 0)
        {
            return true;
        }
        else
        {
            return m_Ptr || !m_TotalItems;
        }
    }

    void Empty() noexcept
    {
        m_MaxIndex = 0;

        if (!IsValid())
        {
            m_TotalItems = 0;
        }
    }

    operator T* () noexcept
    {
        return m_Ptr;
    }

    T& operator * () const noexcept
    {
        return *m_Ptr;
    }

    T& operator [] (int3264 Index) noexcept
    {
        return m_Ptr[Index];
    }
    const T& operator [] (int3264 Index) const noexcept
    {
        return m_Ptr[Index];
    }
    /*
    T& operator [] (size_t Index) noexcept
    {
        return m_Ptr[Index];
    }
    const T& operator [] (size_t Index) const noexcept
    {
        return m_Ptr[Index];
    }
    */
#ifndef _MaaRF_INTERNAL_BUILD
    T& at(int3264 Index) //noexcept
    {
        if (Index < 0 || Index >= m_MaxIndex)
        {
            throw std::out_of_range("at");
        }
        return m_Ptr[Index];
    }
    const T& at(int3264 Index) const //noexcept
    {
        if (Index < 0 || Index >= m_MaxIndex)
        {
            throw std::out_of_range("at");
        }
        return m_Ptr[Index];
    }
#endif
    constexpr size_t MaxIndex() const noexcept
    {
        return m_MaxIndex;
    }
    constexpr int3264 count() const noexcept
    {
        return m_MaxIndex;
    }
    constexpr int3264 size() const noexcept
    {
        return (int3264)MaxIndex();
    }
    constexpr size_t Size() const noexcept
    {
        return m_MaxIndex * sizeof(T);
    }
    void Zero() noexcept
    {
        if (m_Ptr)
        {
            Memzero(m_Ptr, (size_t)sizeof(T) * m_MaxIndex);
        }
    }
    void fill(const T& value, int3264 size = -1) noexcept(noexcept(m_Ptr[0] = value))
    {
        if (m_Ptr && size <= m_MaxIndex)
        {
            for (int3264 i = 0; i < size; i++)
            {
                m_Ptr[i] = value;
            }
        }
    }
    /*virtual*/ bool Resize(int3264 n) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, m_Ptr[0] = m_Ptr[0]))
    {
        CMaaPtr_ < T, xThrow > tmp(n);
        if (!tmp.IsValid())
        {
            return false;
        }
        const int3264 nn = (int)m_MaxIndex < n ? (int)m_MaxIndex : n;
        for (int3264 i = 0; i < nn; i++)
        {
            //tmp[i] = (*this)[i];
            tmp.m_Ptr[i] = m_Ptr[i];
        }
        Swap(tmp);
        return true;
    }
    constexpr int Get_xThrow() const noexcept
    {
        return xThrow;
    }
    bool resize(int3264 n) noexcept(xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T)))
    {
        CMaaPtr_ < T, xThrow > tmp(n);
        if (!tmp.IsValid())
        {
            return false;
        }
        /*
        int nn = (int)m_MaxIndex < n ? (int)m_MaxIndex : n;
        for (int i = 0; i < nn; i++)
        {
            tmp[i] = (*this)[i];
        }
        */
        //tmp.MaxIndex = nn;
        Swap(tmp);
        return true;
    }
    static void Delete(T* ptr) noexcept
    {
        delete[] ptr;
    }
    T* Extract() noexcept
    {
        T* ptr = m_Ptr;
        m_Ptr = nullptr;
        m_MaxIndex = m_TotalItems = 0;
        return ptr;
    }
};
//---------------------------------------------------------------------------
// Automatic expandable CMaaPtr_
template < class T, int xThrow > class CMaaPtrAE_ : public CMaaPtr_<T, xThrow>
{
public:
    CMaaPtrAE_(size_t MaxIndex) noexcept(xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T)))
        : CMaaPtr_<T, xThrow>(MaxIndex)
    {
    }
    CMaaPtrAE_() noexcept
    //:   CMaaPtr_<T, xThrow>()
    {
    }
    CMaaPtrAE_(CMaaPtrAE_<T, xThrow>& That) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, this->m_Ptr[0] = this->m_Ptr[0]))
    :   CMaaPtr_<T, xThrow>(That.m_MaxIndex)
    {
        if (CMaaPtr_<T, xThrow>::IsValid() && That.IsValid())
        {
            T* pThis = this->m_Ptr;
            T* pThat = That.m_Ptr;
            const int3264 m = (int3264)CMaaPtr_ < T, xThrow >::MaxIndex();
            for (int3264 i = 0; i < m; i++)
            {
                pThis[i] = pThat[i];
            }
        }
    }
    T& operator [] (int3264 Index) noexcept((xThrow < 0 || (!xThrow && std_is_nothrow_move_constructible0(T))) && std_is_nothrow_move_assignable(T, this->m_Ptr[0] = this->m_Ptr[0]) && noexcept(CMaaPtr_<T, xThrow>::CheckExpand(Index + 1)))
    {
        if (Index < 0 || Index + 1 < 0 || !CMaaPtr_<T, xThrow>::CheckExpand(Index + 1))
        {
            static T t;
            return t;
        }
        return CMaaPtr_<T, xThrow>::m_Ptr[Index];
    }
    const T& operator [] (int3264 Index) const noexcept(std_is_nothrow_move_constructible0(T))
    {
        const int3264 m = (int3264)CMaaPtr_<T, xThrow>::MaxIndex();
        if (Index < 0 || Index >= m)
        {
            ////CMaaPtr_THROW;
            static T t;
            return t;
        }
        return CMaaPtr_<T, xThrow>::m_Ptr[Index];
    }
    void Swap(CMaaPtrAE_<T, xThrow>& That) noexcept
    {
        CMaaPtr_<T, xThrow>::Swap(That);
    }
};
//---------------------------------------------------------------------------

#else
//---------------------------------------------------------------------------
// New version of CMaaPtr, CMaaPtrAE is slower
//---------------------------------------------------------------------------
template < class T > class CMaaPtr
{
protected:
    T    * m_Ptr;
    size_t m_MaxIndex, m_TotalItems, m_InitializedItems;
    int m_xThrow;

public:

    int GetThrowParameter() const noexcept
    {
        return m_xThrow;
    }
    void Swap ( CMaaPtr & That ) noexcept
    {
        CMaaSwap ( m_Ptr, That.m_Ptr );
        CMaaSwap ( m_MaxIndex, That.m_MaxIndex );
        CMaaSwap ( m_xThrow, That.m_xThrow );
        CMaaSwap ( m_TotalItems, That.m_TotalItems );
        CMaaSwap ( m_InitializedItems, That.m_InitializedItems);
    }
    // This constructor creates MaxIndex numbers of elements of class T.
    // Under Windows makes exeption if no free memory available.
    CMaaPtr ( CMaaPtr &x )
    :   m_Ptr ( nullptr ),
        m_MaxIndex ( 0 ),
        m_TotalItems(0),
        m_InitializedItems(0),
        m_xThrow(x.m_xThrow)
    {
        CMaaPtr<T> tmp(x.MaxIndex(), x.m_xThrow);
        if  (tmp.IsValid())
        {
            //memcpy(tmp, x, x.MaxIndex() * sizeof(T));
            int m = (int)x.m_InitializedItems;
            for (int i = 0; i < m; i++)
            {
                new_(tmp.m_Ptr + i, T(x[i]));
                tmp.m_InitializedItems = (i + 1);
            }
        }
        tmp.Swap(*this);
    }

    void DefInit(size_t m)
    {
        if  (m <= m_TotalItems)
        {
            while(m_InitializedItems < m)
            {
                new_(m_Ptr + m_InitializedItems++, T{});
            }
        }
    }

    void operator += (const CMaaPtr &x)
    {
        CMaaPtr<T> tmp(MaxIndex() + x.MaxIndex(), m_xThrow);
        if  (tmp.IsValid())
        {
            //          memcpy(tmp, *this, MaxIndex() * sizeof(T));
            int i;
            int m = (int)m_InitializedItems;
            for (i = 0; i < m; i++)
            {
                new_(tmp.m_Ptr + i, T((*this)[i]));
                tmp.m_InitializedItems = (i + 1);
            }
            m = (int)MaxIndex();
            DefInit(m);
            //          memcpy(MaxIndex + tmp, x, x.MaxIndex() * sizeof(T));
            int m2 = (int)x.m_InitializedItems;
            for (i = 0; i < m2; i++)
            {
                new_(tmp.m_Ptr + i + m, T(x.m_Ptr[i]));
                //tmp[i + (int)MaxIndex()] = x[i];
                tmp.m_InitializedItems = (i + m + 1);
            }
        }
        tmp.Swap(*this);
    }

    /*
    void operator += (T x)
    {
        CMaaPtr<T> tmp(MaxIndex + 1, m_xThrow);
        if (tmp.IsValid())
        {
            //memcpy(tmp, *this, MaxIndex() * sizeof(T));
            for (int i = 0; i < (int)MaxIndex(); i++)
            {
                tmp[i] = x[i];
            }
            tmp[MaxIndex] = x;
        }
        tmp.Swap(*this);
    }
    */

    void operator += (const T &x)
    {
        if  (m_TotalItems <= m_MaxIndex + 1)
        {
            DefInit(m_MaxIndex);
            if  (m_InitializedItems > m_MaxIndex)
            {
                (*this)[m_MaxIndex + 1] = x;
                m_MaxIndex++;
            }
            else
            {
                new_(m_Ptr + m_MaxIndex + 1, T(x));
                m_InitializedItems = m_MaxIndex + 1;
            }
        }
        else
        {
            CMaaPtr<T> tmp(m_MaxIndex + 1, m_xThrow, 2 * (m_MaxIndex + 1));
            if  (tmp.IsValid())
            {
                int i;
                int m = (int)m_InitializedItems;
                for (i = 0; i < m; i++)
                {
                    new_(tmp.m_Ptr + i, T((*this)[i]));
                    tmp.m_InitializedItems = (i + 1);
                }
                m = (int)MaxIndex();
                tmp.DefInit(m);
                new_(tmp.m_Ptr + m, T(x));
                tmp.m_InitializedItems = (m + 1);
            }
            tmp.Swap(*this);
        }
    }

    void operator += (T &x)
    {
        if  (m_TotalItems <= m_MaxIndex + 1)
        {
            DefInit(m_MaxIndex);
            if  (m_InitializedItems > m_MaxIndex)
            {
                (*this)[m_MaxIndex + 1] = x;
                m_MaxIndex++;
            }
            else
            {
                new_(m_Ptr + m_MaxIndex + 1, T(x));
                m_InitializedItems = m_MaxIndex + 1;
            }
        }
        else
        {
            CMaaPtr<T> tmp(m_MaxIndex + 1, m_xThrow, 2 * (m_MaxIndex + 1));
            if  (tmp.IsValid())
            {
                int i;
                int m = (int)m_InitializedItems;
                for (i = 0; i < m; i++)
                {
                    new_(tmp.m_Ptr + i, T((*this)[i]));
                    tmp.m_InitializedItems = (i + 1);
                }
                m = (int)MaxIndex();
                tmp.DefInit(m);
                new_(tmp.m_Ptr + m, T(x));
                tmp.m_InitializedItems = (m + 1);
            }
            tmp.Swap(*this);
        }
    }

    CMaaPtr<T> & operator = (const CMaaPtr<T> &That)
    {
        CMaaPtr<T> tmp(That.m_MaxIndex, m_xThrow, That.m_TotalItems);
        if  (tmp.IsValid())
        {
            int m = (int)That.m_InitializedItems;
            for (int i = 0; i < m; i++)
            {
                new_(tmp.m_Ptr + i, T(That[i]));
                tmp.m_InitializedItems = (i + 1);
            }
        }
        tmp.Swap(*this);
        return *this;
    }

    void append (T x)
    {
        (*this) += x;
    }

    CMaaPtr ( size_t MaxIndex = 0, int xThrow = 0, size_t TotalItems = 0)
    :   m_Ptr(nullptr),
        m_MaxIndex(MaxIndex),
        m_TotalItems(TotalItems > MaxIndex ? TotalItems : MaxIndex),
        m_InitializedItems(0),
        m_xThrow(xThrow)
    {
        if  ((size_t)sizeof(T) * m_TotalItems >= m_TotalItems)
        {
            m_Ptr = nullptr;
            try
            {
                m_Ptr = (T *)(TL_NEW char[(size_t)sizeof(T) * m_TotalItems]);
            }
            catch(...)
            {
            }
            if  (!m_Ptr && m_TotalItems)
            {
                INT3;
#ifndef __unix__
                OUT_DEBUG_STRING ( "MAARF:Memory allocation failed\r\n" );
#endif
                m_MaxIndex = m_TotalItems = 0;
                if  (xThrow > 0)
                {
                    CMaaPtr_THROW1;
                }
                if  (xThrow == 0)
                {
                    CMaaPtr_THROW;
                }
            }
        }
    }

    ~CMaaPtr () noexcept
    {
        size_t m = m_InitializedItems;
        while(m)
        {
            (m_Ptr + --m)->~T();
        }
        delete [] (char *)m_Ptr;
    }

    virtual bool IsValid () const noexcept
    {
        return m_Ptr != nullptr || m_TotalItems == 0;
    }

    operator T * () noexcept
    {
        if  (m_Ptr)
        {
            DefInit(m_MaxIndex); // for compatibility
        }
        return m_Ptr;
    }

    T & operator * () const noexcept
    {
        if  (m_Ptr)
        {
            DefInit(1); // for compatibility
        }
        return * m_Ptr;
    }

    T & operator [] ( int Index ) noexcept
    {
        if  (m_Ptr)
        {
            DefInit(Index + 1);
        }
        return m_Ptr [ Index ];
    }
    const T & operator [] ( int Index ) const noexcept
    {
        if  (m_Ptr)
        {
            DefInit(Index + 1);
        }
        return m_Ptr [ Index ];
    }
    T & at(int Index) noexcept
    {
        if  (m_Ptr)
        {
            DefInit(Index + 1);
        }
        return m_Ptr[Index];
    }
    const T&at(int Index) const noexcept
    {
        if  (m_Ptr)
        {
            DefInit(Index + 1);
        }
        return m_Ptr[Index];
    }
    size_t MaxIndex() const noexcept
    {
        return m_MaxIndex;
    }
    int count() const noexcept
    {
        return (int)m_MaxIndex;
    }
    int size() const noexcept
    {
        return (int)MaxIndex();
    }
    size_t Size() const noexcept
    {
        return m_MaxIndex * sizeof(T);
    }
    void Zero () noexcept
    {
        if  ( m_Ptr )
        {
            Memzero ( m_Ptr, ( size_t ) sizeof ( T ) * m_MaxIndex );
            if  (m_InitializedItems < m_MaxIndex)
            {
                m_InitializedItems = m_MaxIndex;
            }
        }
    }
    void fill(const T & value, int size = -1)
    {
        if  (m_Ptr && size <= (int)m_MaxIndex && size > 0)
        {
            int m = (int)m_InitializedItems;
            if  (m > size)
            {
                m = size;
            }
            int i;
            for (i = 0; i < m; i++)
            {
                m_Ptr[i] = value;
            }
            for (; i < size; i++)
            {
                new_(m_Ptr + i, T(value));
                m_InitializedItems = (i + 1);
            }
        }
    }
    virtual bool Resize ( int n )
    {
        if  (n <= (int)m_TotalItems)
        {
            if  (n >= (int)m_MaxIndex)
            {
                m_MaxIndex = n;
                DefInit(n);
            }
            else
            {
                int m = (int)m_InitializedItems;
                for (int i = m; i > n; )
                {
                    i--;
                    (m_Ptr + i)->~T();
                    m_MaxIndex = m_InitializedItems = i;
                }
            }
        }
        else
        {
            CMaaPtr < T > tmp(n);
            if  (!tmp.IsValid())
            {
                return false;
            }
            /*
            int nn = (int)m_MaxIndex < n ? (int)m_MaxIndex : n;
            for (int i = 0; i < nn; i++)
            {
                tmp[i] = (*this)[i];
            }
            */
            int nn = (int)m_InitializedItems < n ? (int)m_InitializedItems : n;
            for (int i = 0; i < nn; i++)
            {
                new_(tmp.m_Ptr + i, T((*this)[i]));
                tmp.m_InitializedItems = (i + 1);
            }
            //tmp.DefInit(n);
            Swap(tmp);
        }
        return true;
    }
    int Get_xThrow() const noexcept
    {
        return m_xThrow;
    }
    bool resize(int n)
    {
        CMaaPtr < T > tmp(n);
        if  (!tmp.IsValid())
        {
            return false;
        }
        /*
        int nn = (int)m_MaxIndex < n ? (int)m_MaxIndex : n;
        for (int i = 0; i < nn; i++)
        {
            tmp[i] = (*this)[i];
        }
        */
        //tmp.MaxIndex = nn;
        tmp.DefInit(n);
        Swap(tmp);
        return true;
    }
    static void Delete(T * ptr) noexcept
    {
        char* p = (char*)ptr;
        delete [] p;
    }
    T * Extract() noexcept
    {
        T * ptr = m_Ptr;
        m_Ptr = nullptr;
        return ptr;
    }
};
//---------------------------------------------------------------------------
// Automatic expandable CMaaPtr
template < class T > class CMaaPtrAE : public CMaaPtr < T >
{
public:
    CMaaPtrAE(size_t MaxIndex, int xThrow = 0)
    :   CMaaPtr < T > (MaxIndex, xThrow)
    {
    }
    CMaaPtrAE(CMaaPtrAE<T> & That)
    :   CMaaPtr < T > (That.m_MaxIndex, That.m_xThrow, That.m_TotalItems)
    {
        if  (CMaaPtr < T >::IsValid() && That.IsValid())
        {
            T * pThis = (T *)*this;
            T * pThat = (T *)That;
            int n = (int) CMaaPtr < T >::m_InitializedItems;
            for (int i = 0; i < n; i++)
            {
                new_(pThis + i, T(pThat[i]));
            }
        }
    }
    T & operator [] (int Index)
    {
        int m = (int) CMaaPtr < T >::m_TotalItems;
        if  (Index >= m )
        {
            int N = 2 * m > Index ? 2 * m : Index + 1;
            CMaaPtrAE < T > Expand(N, CMaaPtr < T >::m_xThrow);
            if  (!Expand.IsValid())
            {
                //CMaaPtr_THROW;
                static T t{};
                return t;
            }
            m = (int)CMaaPtr < T >::m_InitializedItems;
            for (int i = 0; i < m; i++)
            {
                new_(Expand.m_Ptr + i, T(CMaaPtr < T >::m_Ptr[i]));
                Expand.m_InitializedItems = (i + 1);
            }
            //memcpy((T*)Expand, (T*)*this, CMaaPtr < T >::Size());
            Swap(Expand);
        }
        return CMaaPtr < T >::operator [] (Index);
    }
    void Swap(CMaaPtrAE<T> & That) noexcept
    {
        CMaaPtr<T>::Swap(That);
    }
};
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#ifndef TOOLSLIB_SINGLE_THREAD
void CMaaAllocatorBasicLock() noexcept;
void CMaaAllocatorBasicUnLock() noexcept;
#endif
//---------------------------------------------------------------------------
template<class T> void CMaaAllocatorLock(T *) noexcept
{
#ifndef TOOLSLIB_SINGLE_THREAD
    CMaaAllocatorBasicLock();
#endif
}
template<class T> void CMaaAllocatorUnLock(T *) noexcept
{
#ifndef TOOLSLIB_SINGLE_THREAD
    CMaaAllocatorBasicUnLock();
#endif
}
// to redefine:
// template<> void CMaaAllocatorLock(MyClass *);
// template<> void CMaaAllocatorUnLock(MyClass *);
//typedef void (*lf(T *)) __lf;
#define TL_NO_CMaaAllocatorStaticLocker
#ifndef TL_NO_CMaaAllocatorStaticLocker
template <class T> class CMaaAllocatorStaticLocker
{
public:
    static void CreateFn(T* obj) noexcept
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        __GLock__LockFn(obj, true);
#endif
    }
    static void InitLockFn(T* obj) noexcept
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        __GLock__LockFn(obj, true)->Lock();
#endif
    }
    static void LockFn(T * obj) noexcept
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        // CMaaAllocatorLock<T>(obj); // version before 19.10.2021+
        __GLock__LockFn(obj)->Lock();
#endif
    }
    static void UnLockFn(T * obj) noexcept
    {
#ifndef TOOLSLIB_SINGLE_THREAD
        //CMaaAllocatorUnLock<T>(obj); // version before 19.10.2021+
        __GLock__LockFn(obj)->UnLock();
#endif
    }
};
#endif
//---------------------------------------------------------------------------
template<class T> void CMaaMakeAddFreeList(char* m, size_t N, size_t sz, T*& l)  noexcept
{
    T* m0 = (T*)m;
    for (size_t i = 1; i < N; i++)
    {
        *(T**)m = (T*)(m + sz);
        m += sz;
    }
    *(T**)m = l;
    l = m0;
}
template<class T> void CMaaMakeAddFreeList(T* m, size_t N, T*& l)  noexcept
{
    T* m0 = m;
    for (size_t i = 1; i < N; i++)
    {
        *(T**)m = m + 1;
        m++;
    }
    *(T**)m = l;
    l = m0;
}
//---------------------------------------------------------------------------
//#define TL_ALLOCATOR_CHECK
#ifdef TL_ALLOCATOR_CHECK
//extern std::atomic<void*> g_AllocatorCheckPtr;
void g_AllocatorCheckPtr_Get(void* ptr);
void g_AllocatorCheckPtr_Set(void* ptr);
#endif
inline constexpr int ln2____(size_t x, int i = 0) { return x < (((size_t)1) << (i + 1)) ? i : ln2____(x, i + 1); }
template<class T, int AllocatorType = 1
#ifndef TOOLSLIB_SINGLE_THREAD
#ifndef TL_NO_CMaaAllocatorStaticLocker
#ifdef _WIN32
    //, typename Fn
    , class CLocker = CMaaAllocatorStaticLocker<T>
#else
    , class CLocker = CMaaAllocatorStaticLocker<T>
#endif
#endif
#endif
> class CMaaFixedAllocator
{
    mutable T* m_pSList;
    mutable size_t m_Ref;
    mutable size_t m_TotalCount, m_FreeCount;
    mutable int m_pMemIndex;
    //int m_AllocatorType; // 0 - unknown, -1 - w/o of internal syncronization (NewUl/DeleteUl/AddRefUl/DelRefUl), 1 - synchronized allocations (New/Delete/AddRef/DelRef)

    static constexpr size_t ItemSize() noexcept
    {
        const size_t s7 = sizeof(void*) - 1;
#ifdef __ALLOCATOR_RESERVE_PTR
        return ((sizeof(T) + sizeof(void*)) + s7) & ~s7;
#else
        return sizeof(T) > sizeof(void*) ? (sizeof(T) + s7) & ~s7 : sizeof(void*);
#endif
    }
    static constexpr int MaxIndex = sizeof(size_t) * 8 - 1 - 3 - ln2____(ItemSize());
    mutable char* pMem[MaxIndex];

protected:
//#ifndef TOOLSLIB_SINGLE_THREAD // noops are good optimized, #ifndef not very required
    class Locker
    {
    public:
#ifndef TOOLSLIB_SINGLE_THREAD
        Locker() noexcept
        {
#ifdef TL_NO_CMaaAllocatorStaticLocker
            __GLock__LockFn(T_NULL(T*))->Lock();
#else
            CLocker::LockFn(T_NULL(T*));
#endif
        }
        Locker(bool b) noexcept
        {
#ifdef TL_NO_CMaaAllocatorStaticLocker
            __GLock__LockFn(T_NULL(T*), b)->Lock();
#else
            CLocker::LockFn(T_NULL(T*));
#endif
        }
        ~Locker() noexcept
        {
#ifdef TL_NO_CMaaAllocatorStaticLocker
            __GLock__LockFn(T_NULL(T*))->UnLock();
#else
            CLocker::UnLockFn(T_NULL(T*));
#endif
        }
        static constexpr void CreateMtx() noexcept
        {
#if 0
#ifdef TL_NO_CMaaAllocatorStaticLocker
            __GLock__LockFn(T_NULL(T*), true);
#else
            CLocker::CreateFn(T_NULL(T*));
            //CMaaAllocatorStaticLocker<T>::CreateFn(T_NULL(T*));
#endif
#endif
        }

#else // TOOLSLIB_SINGLE_THREAD

        constexpr Locker() noexcept {}
        constexpr Locker(bool) noexcept {}
        constexpr ~Locker() noexcept {}
        static constexpr void CreateMtx() noexcept {}
#endif
    };
//#endif
public:
    constexpr CMaaFixedAllocator() noexcept
    {
        m_pSList = nullptr;
        m_Ref = 0;
        m_TotalCount = m_FreeCount = 0;
        m_pMemIndex = 0;
        for (int i = 0; i < MaxIndex; i++)
        {
            pMem[i] = nullptr;
        }
        //m_AllocatorType = AllocatorType;
        if constexpr (AllocatorType >= 0)
        {
//#ifndef TOOLSLIB_SINGLE_THREAD
            Locker::CreateMtx();
//#endif
        }
    }
    CMaaFixedAllocator(eNotInit_) noexcept
    {
        m_pSList = nullptr;
        m_Ref = 0;
        m_TotalCount = m_FreeCount = 0;
        m_pMemIndex = 0;
        /*
        for (int i = 0; i < MaxIndex; i++)
        {
            pMem[i] = nullptr;
        }
        */
        //m_AllocatorType = AllocatorType;
        if constexpr (AllocatorType >= 0)
        {
            //#ifndef TOOLSLIB_SINGLE_THREAD
            Locker::CreateMtx();
            //#endif
        }
    }
    constexpr ~CMaaFixedAllocator() noexcept
    {
        //FreeAll();
    }
    void destr2() noexcept
    {
        FreeAll();
        delete this;
    }
    void FreeAll() noexcept
    {
        for (int i = m_pMemIndex; i--; )
        {
            delete[] pMem[i];
        }
        m_pSList = nullptr;
        m_TotalCount = m_FreeCount = 0;
        m_pMemIndex = 0;
    }
    static inline constexpr int GetAllocatorType() noexcept
    {
        return AllocatorType;
    }
    bool PreAllocate(size_t CountTotal, size_t FreeCountRequired = 0, size_t CountAdd = 0) // noexcept(noexcept_new)
    {
        const size_t cnt1 = m_TotalCount;
        while(m_TotalCount < CountTotal || m_FreeCount < FreeCountRequired || m_TotalCount - cnt1 < CountAdd)
        {
            if  (Alloc() < 0)
            {
                return false;
            }
        }
        return true;
    }
    // Is mem ptr preallocated and aligned to object
    bool IsObjPtr(const T* ptr) const
    {
        const size_t sz_1 = ItemSize();
        for (int i = m_pMemIndex; i--; )
        {
            char* p = pMem[i];
            if ((char*)ptr >= p && (char*)ptr < p + sz_1 * (((size_t)1) << i))
            {
                return !((size_t)((char*)ptr - p) % sz_1);
            }
        }
        return false;
    }
    // Is mem ptr point to a preallocated memory
    bool HasMemPtr(const void* ptr) const
    {
        const size_t sz_1 = ItemSize();
        for (int i = m_pMemIndex; i--; )
        {
            char* p = pMem[i];
            if ((char*)ptr >= p && (char*)ptr < p + sz_1 * (((size_t)1) << i))
            {
                return true;
            }
        }
        return false;
    }
protected:
    int Alloc() //noexcept(noexcept_new)
    {
        TL_DbgAlloc;

        if (m_pMemIndex < MaxIndex)
        {
            const int i = m_pMemIndex;
            const size_t n = ((size_t)1) << i;
            size_t sz = ItemSize();
            //try
            //{
                /*
                switch (n * sz)
                {
                case 196608:
                    static int aa = 0;
                    aa++;
                }
                */
            pMem[i] = n < SIZE_MAX / sz ? TL_NEW char[n * sz] : nullptr;
#ifdef __ALLOCATOR_FILL
            if (pMem[i])
            {
                memset(pMem[i], __ALLOCATOR_FILL_INIT_CHAR, n * sz);
            }
#endif
            //}
            //catch(...)
            //{
                //throw;
            //}
            if (pMem[i])
            {
                CMaaMakeAddFreeList(pMem[i], n, sz, m_pSList);
                m_TotalCount += n;
                m_FreeCount += n;
                m_pMemIndex++;
                return i;
            }
        }
        return -1;
    }
public:
    T * New2(CMaaFixedAllocator<T, AllocatorType> ** pVar) noexcept(noexcept_new)
    {
        T * p = m_pSList;
        if  (!p)
        {
            Alloc();
            p = m_pSList;
        }
        if  (p)
        {
            ++m_Ref;
            --m_FreeCount;
            m_pSList = *(T **)p;
        }
        else if (!m_Ref)
        {
            *pVar = nullptr;
            destr2();
        }
#ifdef __ALLOCATOR_RESERVE_PTR
        if  (p)
        {
            memset(p, __ALLOCATOR_FILL_ALLOC_CHAR, sizeof(void *) + sizeof(T));
            p = (T *)(sizeof(void *) + (char *)p);
        }
#else
#ifdef __ALLOCATOR_FILL
        if  (p)
        {
            memset(p, __ALLOCATOR_FILL_ALLOC_CHAR, sizeof(T));
        }
#endif
#endif
        return p;
    }
    T* New2() noexcept(noexcept_new)
    {
        T* p = m_pSList;
        if (!p)
        {
            Alloc();
            p = m_pSList;
        }
        if (p)
        {
            ++m_Ref;
            --m_FreeCount;
            m_pSList = *(T**)p;
        }
        else if (!m_Ref)
        {
            FreeAll();
        }
#ifdef __ALLOCATOR_RESERVE_PTR
        if (p)
        {
            memset(p, __ALLOCATOR_FILL_ALLOC_CHAR, sizeof(void*) + sizeof(T));
            p = (T*)(sizeof(void*) + (char*)p);
        }
#else
#ifdef __ALLOCATOR_FILL
        if (p)
        {
            memset(p, __ALLOCATOR_FILL_ALLOC_CHAR, sizeof(T));
        }
#endif
#endif
        return p;
    }
    constexpr void Delete2(T * ptr, CMaaFixedAllocator<T, AllocatorType> ** pVar) noexcept
    {
        if  (ptr)
        {
#ifdef TL_ALLOCATOR_CHECK
            g_AllocatorCheckPtr_Get(ptr);
#endif
#ifdef __ALLOCATOR_RESERVE_PTR
            ptr = (T *)((char *)ptr - sizeof(void *));
            memset(ptr, __ALLOCATOR_FILL_FREE_CHAR, sizeof(T) + sizeof(void *));
#else
#ifdef __ALLOCATOR_FILL
            memset(ptr, __ALLOCATOR_FILL_FREE_CHAR, sizeof(T));
#endif
#endif
            *(T **)ptr = m_pSList;
            m_pSList = ptr;
            ++m_FreeCount;
            if  (!--m_Ref)
            {
                *pVar = nullptr;
                destr2();
            }
        }
    }
    constexpr void Delete2(T* ptr) noexcept
    {
        if (ptr)
        {
#ifdef TL_ALLOCATOR_CHECK
            g_AllocatorCheckPtr_Get(ptr);
#endif
#ifdef __ALLOCATOR_RESERVE_PTR
            ptr = (T*)((char*)ptr - sizeof(void*));
            memset(ptr, __ALLOCATOR_FILL_FREE_CHAR, sizeof(T) + sizeof(void*));
#else
#ifdef __ALLOCATOR_FILL
            memset(ptr, __ALLOCATOR_FILL_FREE_CHAR, sizeof(T));
#endif
#endif
            * (T**)ptr = m_pSList;
            m_pSList = ptr;
            ++m_FreeCount;
            if (!--m_Ref)
            {
                FreeAll();
            }
        }
    }
    static T * New(CMaaFixedAllocator<T, AllocatorType> ** pVar) noexcept(noexcept_new)
    {
//#ifndef TOOLSLIB_SINGLE_THREAD
        Locker lk(!*pVar);
//#endif

        //T * p = nullptr;
        CMaaFixedAllocator<T, AllocatorType>* a = *pVar;
#ifndef TOOLSLIB_SINGLE_THREAD
        //try
#endif
        //{
            if  (!a)
            {
                *pVar = a = TL_NEW CMaaFixedAllocator<T, AllocatorType>(eNotInit);
            }
            T * p = a ? a->New2(pVar) : nullptr;
        //}
#ifndef TOOLSLIB_SINGLE_THREAD
        //catch(...)
        //{
            //if (a && !a->m_Ref)
            //{
            //    *pVar = nullptr;
            //    delete a;
            //}
            //throw;
        //}
#endif
        return p;
    }
    T* New() noexcept(noexcept_new)
    {
        //#ifndef TOOLSLIB_SINGLE_THREAD
        Locker lk(false);
        //#endif

        return New2();
    }
    static constexpr void Delete(T* ptr, CMaaFixedAllocator<T, AllocatorType>** pVar) noexcept
    {
        if (ptr)
        {
            //#ifndef TOOLSLIB_SINGLE_THREAD
            Locker lk;
            //#endif
            CMaaFixedAllocator<T, AllocatorType>* a = *pVar;
            if (a)
            {
                a->Delete2(ptr, pVar);
            }
        }
    }
    constexpr void Delete(T* ptr) noexcept
    {
        if (ptr)
        {
            //#ifndef TOOLSLIB_SINGLE_THREAD
            Locker lk;
            //#endif
            Delete2(ptr);
        }
    }
    static T * NewUl(CMaaFixedAllocator<T, AllocatorType> ** pVar) noexcept(noexcept_new)
    {
        //T * p = nullptr;
        CMaaFixedAllocator<T, AllocatorType>* a = *pVar;
        //try
        //{
            if  (!a)
            {
                *pVar = a = TL_NEW CMaaFixedAllocator<T, AllocatorType>(eNotInit);
            }
            T * p = a ? a->New2(pVar) : nullptr;
        //}
        //catch(...)
        //{
        //    if (a && !a->m_Ref)
        //    {
        //        *pVar = nullptr;
        //        delete a;
        //    }
        //    throw;
        //}
        return p;
    }
    T* NewUl() noexcept(noexcept_new)
    {
        return New2();
    }
    static constexpr void DeleteUl(T* ptr, CMaaFixedAllocator<T, AllocatorType>** pVar) noexcept
    {
        if (ptr)
        {
            CMaaFixedAllocator<T, AllocatorType>* a = *pVar;
            if (a)
            {
                a->Delete2(ptr, pVar);
            }
        }
    }
    constexpr void DeleteUl(T* ptr) noexcept
    {
        if (ptr)
        {
            Delete2(ptr);
        }
    }
    size_t AddRef() noexcept
    {
//#ifndef TOOLSLIB_SINGLE_THREAD
        Locker lk;
//#endif
        return ++m_Ref;
    }
    size_t DelRef(CMaaFixedAllocator<T, AllocatorType> ** pVar /*= nullptr*/) noexcept
    {
        size_t Ref;
        {
//#ifndef TOOLSLIB_SINGLE_THREAD
            Locker lk;
//#endif
            Ref = --m_Ref;
            if (!Ref)
            {
                if (pVar)
                {
                    *pVar = nullptr;
                }
                destr2();
            }
        }
        return Ref;
    }
    size_t DelRef() noexcept
    {
        size_t Ref;
        {
            //#ifndef TOOLSLIB_SINGLE_THREAD
            Locker lk;
            //#endif
            Ref = --m_Ref;
            if (!Ref)
            {
                FreeAll();
            }
        }
        return Ref;
    }
    size_t AddRefUl() noexcept
    {
        return ++m_Ref;
    }
    size_t DelRefUl(CMaaFixedAllocator<T, AllocatorType>** pVar /*= nullptr*/) noexcept
    {
        const size_t Ref = --m_Ref;
        if (!Ref)
        {
            if (pVar)
            {
                *pVar = nullptr;
            }
            destr2();
        }
        return Ref;
    }
    size_t DelRefUl() noexcept
    {
        const size_t Ref = --m_Ref;
        if (!Ref)
        {
            FreeAll();
        }
        return Ref;
    }
    static bool CreateAddRef(CMaaFixedAllocator<T, AllocatorType>** pVar) noexcept(noexcept_new)
    {
//#ifndef TOOLSLIB_SINGLE_THREAD
        Locker lk(!*pVar);
//#endif

        bool b = false;
#ifndef TOOLSLIB_SINGLE_THREAD
        //try
#endif
        //{
            CMaaFixedAllocator<T, AllocatorType>* a = *pVar;
            if (!a)
            {
                *pVar = a = TL_NEW CMaaFixedAllocator<T, AllocatorType>(eNotInit);
                b = a && a->AddRefUl(); // stay-in-memory
            }
            else
            {
                a->AddRefUl(); // 2023+
                b = true;
            }
        //}
#ifndef TOOLSLIB_SINGLE_THREAD
        //catch (...)
        //{
        //    throw;
        //}
#endif
        return b;
    }
    static bool CreateAddRefUl(CMaaFixedAllocator<T, AllocatorType>** pVar) noexcept(noexcept_new)
    {
        bool b = false;
        //try
        //{
            CMaaFixedAllocator<T, AllocatorType>* a = *pVar;
            if (!a)
            {
                *pVar = a = TL_NEW CMaaFixedAllocator<T, AllocatorType>(eNotInit);
                b = a && a->AddRefUl(); // stay-in-memory
            }
            else
            {
                a->AddRefUl(); // 2023+
                b = true;
            }
        //}
        //catch (...)
        //{
        //    throw;
        //}
        return b;
    }

    //int m_AllocatorType; // 0 - unknown, -1 - w/o of internal syncronization (NewUl/DeleteUl/AddRefUl/DelRefUl/CreateAddRefUl), 1 - synchronized allocations (New/Delete/AddRef/DelRef/CreateAddRef)
    static T * NewEx_static(CMaaFixedAllocator<T, AllocatorType> ** pVar) noexcept(noexcept_new)
    {
        if constexpr (AllocatorType < 0)
        {
            return NewUl(pVar);
        }
        else
        {
            return New(pVar);
        }
    }

    static constexpr void DeleteEx_static(T * ptr, CMaaFixedAllocator<T, AllocatorType> ** pVar) noexcept
    {
        if constexpr (AllocatorType < 0)
        {
            DeleteUl(ptr, pVar);
        }
        else
        {
            Delete(ptr, pVar);
        }
    }

    T* NewEx(CMaaFixedAllocator<T, AllocatorType>** pVar) noexcept(noexcept_new)
    {
        if constexpr (AllocatorType < 0)
        {
            return NewUl(pVar);
        }
        else
        {
            return New(pVar);
        }
    }
    T* NewEx() noexcept(noexcept_new)
    {
        if constexpr (AllocatorType < 0)
        {
            return NewUl();
        }
        else
        {
            return New();
        }
    }

    void DeleteEx(T* ptr, CMaaFixedAllocator<T, AllocatorType>** pVar) noexcept
    {
        if constexpr (AllocatorType < 0)
        {
            DeleteUl(ptr, pVar);
        }
        else
        {
            Delete(ptr, pVar);
        }
    }
    void DeleteEx(T* ptr) noexcept
    {
        if constexpr (AllocatorType < 0)
        {
            DeleteUl(ptr);
        }
        else
        {
            Delete(ptr);
        }
    }

    size_t AddRefEx() noexcept
    {
        if constexpr (AllocatorType < 0)
        {
            return AddRefUl();
        }
        else
        {
            return AddRef();
        }
    }

    size_t DelRefEx(CMaaFixedAllocator<T, AllocatorType> ** pVar /*= nullptr*/) noexcept
    {
        if constexpr (AllocatorType < 0)
        {
            return DelRefUl(pVar);
        }
        else
        {
            return DelRef(pVar);
        }
    }
    size_t DelRefEx() noexcept
    {
        if constexpr (AllocatorType < 0)
        {
            return DelRefUl();
        }
        else
        {
            return DelRef();
        }
    }
    size_t GetFreeCount() const noexcept 
    {
        Locker lk;
        return m_FreeCount;
    }
    size_t GetTotalCount() const noexcept
    {
        Locker lk;
        return m_TotalCount;
    }
};
//---------------------------------------------------------------------------
template<class T> class CMaaFixedAllocatorCreator
{
    CMaaFixedAllocator<T, 1>** m_ptr;
    CMaaFixedAllocatorCreator() = delete;
public:
    CMaaFixedAllocatorCreator(CMaaFixedAllocator<T, 1>** ptr) noexcept(noexcept_new)
    {
        CMaaFixedAllocator<T, 1>::CreateAddRef(m_ptr = ptr);
    }
    ~CMaaFixedAllocatorCreator()
    {
        (*m_ptr)->DelRef(m_ptr);
    }
};
//---------------------------------------------------------------------------
template<class T> class CMaaFixedAllocatorCreatorUl
{
    CMaaFixedAllocator<T, -1>** m_ptr;
    CMaaFixedAllocatorCreatorUl() = delete;
public:
    CMaaFixedAllocatorCreatorUl(CMaaFixedAllocator<T, -1>** ptr) noexcept(noexcept_new)
    {
        CMaaFixedAllocator<T, -1>::CreateAddRefUl(m_ptr = ptr);
    }
    ~CMaaFixedAllocatorCreatorUl()
    {
        (*m_ptr)->DelRefUl(m_ptr);
    }
};
//---------------------------------------------------------------------------

#ifdef __unix__
#define __cdecl
#endif

void* ToolsLib_No_Allocator();

#if 0
#define ADD_ALLOCATOR(T) \
static CMaaFixedAllocator<T, 1> * s_pAllocator; \
static CMaaFixedAllocatorCreator<T> s_AllocatorCreator; \
public: \
void *__cdecl operator new__(size_t count) \
{ \
    return CMaaFixedAllocator<T, 1>::New(&s_pAllocator); \
} \
void* __cdecl operator new__(size_t /*count*/, const char * /*lpszFileName*/, int /*nLine*/, sDbgTmp *) \
{ \
    return CMaaFixedAllocator<T, 1>::New(&s_pAllocator); \
} \
constexpr void __cdecl operator delete(void * object) \
{ \
    CMaaFixedAllocator<T, 1>::Delete((T *)object, &s_pAllocator); \
} \
constexpr void __cdecl operator delete(void* object, const char * /* lpszFileName */, int /* nLine */, sDbgTmp*) \
{ \
    CMaaFixedAllocator<T, 1>::Delete((T *)object, &s_pAllocator); \
}

#define ADD_UL_ALLOCATOR(T) \
static CMaaFixedAllocator<T, -1> * s_pAllocator; \
static CMaaFixedAllocatorCreator<T> s_AllocatorCreatorUl; \
public: \
void *__cdecl operator new__(size_t /*count*/) \
{ \
    return CMaaFixedAllocator<T, -1>::NewUl(&s_pAllocator); \
} \
void* __cdecl operator new__(size_t /*count*/, const char * /*lpszFileName*/, int /*nLine*/, sDbgTmp *) \
{ \
    return CMaaFixedAllocator<T, -1>::NewUl(&s_pAllocator); \
} \
constexpr void __cdecl operator delete(void * object) \
{ \
    CMaaFixedAllocator<T, -1>::DeleteUl((T *)object, &s_pAllocator); \
} \
constexpr void __cdecl operator delete(void* object, const char * /* lpszFileName */, int /* nLine */, sDbgTmp*) \
{ \
    CMaaFixedAllocator<T, -1>::DeleteUl((T *)object, &s_pAllocator); \
}

#define ADD_SPECIFIC_ALLOCATOR(T, x) \
static CMaaFixedAllocator<T, x> * s_pAllocator; \
public: \
void *__cdecl operator new__(size_t count) \
{ \
    return CMaaFixedAllocator<T, x>::NewEx_static(&s_pAllocator); \
} \
void* __cdecl operator new__(size_t /*count*/, const char * /*lpszFileName*/, int /*nLine*/, sDbgTmp *) \
{ \
    return CMaaFixedAllocator<T, x>::NewEx_static(&s_pAllocator); \
} \
constexpr void __cdecl operator delete(void * object) \
{ \
    CMaaFixedAllocator<T, x>::DeleteEx_static((T *)object, &s_pAllocator); \
} \
constexpr void __cdecl operator delete(void* object, const char * /* lpszFileName */, int /* nLine */, sDbgTmp*) \
{ \
    CMaaFixedAllocator<T, x>::DeleteEx_static((T *)object, &s_pAllocator); \
}

#ifdef _WIN32
#define ADD_NO_ALLOCATOR \
private: \
    void* __cdecl operator new__(size_t count) noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; \
    void* __cdecl operator new__(size_t count, const char*, int, sDbgTmp*) noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; \
    constexpr void __cdecl operator delete(void* object) {} \
    constexpr void __cdecl operator delete(void* object, const char*, int, sDbgTmp*) {}
#else
#define ADD_NO_ALLOCATOR \
private: \
    void* __cdecl operator new__(size_t count)  noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; \
    constexpr void __cdecl operator delete(void* object) {}
#endif

#else

#ifndef TOOLSLIB_DONT_USE_ALLOCATORS
#ifdef _WIN32
//#define ADD_ALLOCATOR(T) static CMaaFixedAllocator<T, 1> * s_pAllocator; static CMaaFixedAllocatorCreator<T> s_AllocatorCreator; public: void *__cdecl operator new__(size_t count) noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, 1>::New(&s_pAllocator) : nullptr; } void* __cdecl operator new__(size_t count, const char *, int, sDbgTmp*) noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, 1>::New(&s_pAllocator) : nullptr; } constexpr void __cdecl operator delete(void * object) { CMaaFixedAllocator<T, 1>::Delete(static_cast<T *>(object), &s_pAllocator); } constexpr void __cdecl operator delete(void* object, const char *, int, sDbgTmp*) { CMaaFixedAllocator<T, 1>::Delete(static_cast<T *>(object), &s_pAllocator); }
//#define ADD_UL_ALLOCATOR(T) static CMaaFixedAllocator<T, -1> * s_pAllocator; static CMaaFixedAllocatorCreatorUl<T> s_AllocatorCreator; public: void *__cdecl operator new__(size_t count) noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, -1>::NewUl(&s_pAllocator) : nullptr; } void* __cdecl operator new__(size_t count, const char *, int, sDbgTmp *) noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, -1>::NewUl(&s_pAllocator) : nullptr; } constexpr void __cdecl operator delete(void * object) { CMaaFixedAllocator<T, -1>::DeleteUl(static_cast<T*>(object), &s_pAllocator); } constexpr void __cdecl operator delete(void* object, const char *, int, sDbgTmp*) { CMaaFixedAllocator<T, -1>::DeleteUl(static_cast<T*>(object), &s_pAllocator); }
//#define ADD_SPECIFIC_ALLOCATOR(T, x) static CMaaFixedAllocator<T, x> * s_pAllocator; public: void *__cdecl operator new__(size_t count) noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, x>::NewEx_static(&s_pAllocator) : nullptr; } void* __cdecl operator new__(size_t count, const char *, int, sDbgTmp*) noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, x>::NewEx_static(&s_pAllocator) : nullptr; } constexpr void __cdecl operator delete(void * object) { CMaaFixedAllocator<T, x>::DeleteEx_static(static_cast<T*>(object), &s_pAllocator); } constexpr void __cdecl operator delete(void* object, const char *, int, sDbgTmp*) { CMaaFixedAllocator<T, x>::DeleteEx_static(static_cast<T*>(object), &s_pAllocator); }

#define ADD_ALLOCATOR(T) static CMaaFixedAllocator<T, 1> & GetAllocator() noexcept; public: void *__cdecl operator new__(size_t count) noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().New() : nullptr; } void* __cdecl operator new__(size_t count, const char *, int, sDbgTmp*) noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().New() : nullptr; } constexpr void __cdecl operator delete(void * object) { GetAllocator().Delete(static_cast<T *>(object)); } constexpr void __cdecl operator delete(void* object, const char *, int, sDbgTmp*) { GetAllocator().Delete(static_cast<T *>(object)); }
#define ADD_UL_ALLOCATOR(T) static CMaaFixedAllocator<T, -1> & GetAllocator() noexcept; public: void *__cdecl operator new__(size_t count) noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().NewUl() : nullptr; } void* __cdecl operator new__(size_t count, const char *, int, sDbgTmp *) noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().NewUl() : nullptr; } constexpr void __cdecl operator delete(void * object) { GetAllocator().DeleteUl(static_cast<T*>(object)); } constexpr void __cdecl operator delete(void* object, const char *, int, sDbgTmp*) { GetAllocator().DeleteUl(static_cast<T*>(object)); }
#define ADD_SPECIFIC_ALLOCATOR(T, x) static CMaaFixedAllocator<T, x> & GetAllocator() noexcept; public: void *__cdecl operator new__(size_t count) noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().NewEx() : nullptr; } void* __cdecl operator new__(size_t count, const char *, int, sDbgTmp*) noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().NewEx() : nullptr; } constexpr void __cdecl operator delete(void * object) { GetAllocator().DeleteEx(static_cast<T*>(object)); } constexpr void __cdecl operator delete(void* object, const char *, int, sDbgTmp*) { GetAllocator().DeleteEx(static_cast<T*>(object)); }
#define ADD_NO_ALLOCATOR private: void* __cdecl operator new__(size_t count) noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; void* __cdecl operator new__(size_t count, const char*, int, sDbgTmp*) noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; constexpr void __cdecl operator delete(void* object) {} constexpr void __cdecl operator delete(void* object, const char*, int, sDbgTmp*) {}

#else
//#define ADD_ALLOCATOR(T) static CMaaFixedAllocator<T, 1> * s_pAllocator; static CMaaFixedAllocatorCreator<T> s_AllocatorCreator; public: void *__cdecl operator new__(size_t count)  noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, 1>::New(&s_pAllocator) : nullptr; } constexpr void __cdecl operator delete(void * object) { CMaaFixedAllocator<T, 1>::Delete(static_cast<T *>(object), &s_pAllocator); }
//#define ADD_UL_ALLOCATOR(T) static CMaaFixedAllocator<T, -1> * s_pAllocator; static CMaaFixedAllocatorCreatorUl<T> s_AllocatorCreator; public: void *__cdecl operator new__(size_t count)  noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, -1>::NewUl(&s_pAllocator) : nullptr; } constexpr void __cdecl operator delete(void * object) { CMaaFixedAllocator<T, -1>::DeleteUl(static_cast<T*>(object), &s_pAllocator); }
//#define ADD_SPECIFIC_ALLOCATOR(T, x) static CMaaFixedAllocator<T, x> * s_pAllocator; public: void *__cdecl operator new__(size_t count)  noexcept(noexcept_new) { return count <= sizeof(T) ? CMaaFixedAllocator<T, x>::NewEx_static(&s_pAllocator) : nullptr; } constexpr void __cdecl operator delete(void * object) { CMaaFixedAllocator<T, x>::DeleteEx_static(static_cast<T*>(object), &s_pAllocator); }

#define ADD_ALLOCATOR(T) static CMaaFixedAllocator<T, 1> & GetAllocator() noexcept; public: void *__cdecl operator new__(size_t count)  noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().New() : nullptr; } constexpr void __cdecl operator delete(void * object) { if (object) GetAllocator().Delete(static_cast<T *>(object)); }
#define ADD_UL_ALLOCATOR(T) static CMaaFixedAllocator<T, -1> & GetAllocator() noexcept; public: void *__cdecl operator new__(size_t count)  noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().NewUl() : nullptr; } constexpr void __cdecl operator delete(void * object) { if (object) GetAllocator().DeleteUl(static_cast<T*>(object)); }
#define ADD_SPECIFIC_ALLOCATOR(T, x) static CMaaFixedAllocator<T, x> & GetAllocator() noexcept; public: void *__cdecl operator new__(size_t count)  noexcept(noexcept_new) { return count <= sizeof(T) ? GetAllocator().NewEx() : nullptr; } constexpr void __cdecl operator delete(void * object) { if (object) GetAllocator().DeleteEx(static_cast<T*>(object)); }
#define ADD_NO_ALLOCATOR private: void* __cdecl operator new__(size_t count)  noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; constexpr void __cdecl operator delete(void* object) {}
#endif

#else // TOOLSLIB_DONT_USE_ALLOCATORS
struct SF1
{
    static constexpr int PreAllocate(int) noexcept { return 0; }
    static constexpr int AddRef() noexcept { return 0; }
    static constexpr int DelRef() noexcept { return 0; }
    static constexpr int GetFreeCount() noexcept { return 0; }
    static constexpr int GetTotalCount() noexcept { return 0; }
};

#define ADD_ALLOCATOR(T) static SF1 GetAllocator() noexcept { return SF1(); }
#define ADD_UL_ALLOCATOR(T) static SF1 GetAllocator() noexcept { return SF1(); }
#define ADD_SPECIFIC_ALLOCATOR(T, x) static SF1 GetAllocator() noexcept { return SF1(); }
#define ADD_NO_ALLOCATOR //private: void* __cdecl operator new__(size_t count) noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; void* __cdecl operator new__(size_t count, const char*, int, sDbgTmp*) noexcept(noexcept_new) { return ToolsLib_No_Allocator(); }; constexpr void __cdecl operator delete(void* object) {} constexpr void __cdecl operator delete(void* object, const char*, int, sDbgTmp*) {}
#endif // TOOLSLIB_DONT_USE_ALLOCATORS

#endif

#ifndef TOOLSLIB_DONT_USE_ALLOCATORS
//#define    DEF_ALLOCATOR(T) CMaaFixedAllocator< T,  1 >* T::s_pAllocator = nullptr;   CMaaFixedAllocatorCreator< T > T::s_AllocatorCreator(&T::s_pAllocator);
//#define DEF_UL_ALLOCATOR(T) CMaaFixedAllocator< T, -1 >* T::s_pAllocator = nullptr; CMaaFixedAllocatorCreatorUl< T > T::s_AllocatorCreator(&T::s_pAllocator);

//#define    DEF_ALLOCATOR_(T) template<> CMaaFixedAllocator< T,  1 >* T::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< T > T::s_AllocatorCreator(&T::s_pAllocator);
//#define DEF_UL_ALLOCATOR_(T) template<> CMaaFixedAllocator< T, -1 >* T::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< T > T::s_AllocatorCreator(&T::s_pAllocator);

#define    DEF_ALLOCATOR(T) CMaaFixedAllocator<T, 1>  & T::GetAllocator() noexcept { static constexpr CMaaFixedAllocator<T, 1> a;  return (CMaaFixedAllocator<T, 1>  &)a; }
#define DEF_UL_ALLOCATOR(T) CMaaFixedAllocator<T, -1> & T::GetAllocator() noexcept { static constexpr CMaaFixedAllocator<T, -1> a; return (CMaaFixedAllocator<T, -1> &)a; }

#define    DEF_ALLOCATOR_(T) template<> CMaaFixedAllocator<T, 1>  & T::GetAllocator() noexcept { static constexpr CMaaFixedAllocator<T, 1> a;  return (CMaaFixedAllocator<T, 1>  &)a; }
#define DEF_UL_ALLOCATOR_(T) template<> CMaaFixedAllocator<T, -1> & T::GetAllocator() noexcept { static constexpr CMaaFixedAllocator<T, -1> a; return (CMaaFixedAllocator<T, -1> &)a; }

#else // TOOLSLIB_DONT_USE_ALLOCATORS
#define    DEF_ALLOCATOR(T) 
#define DEF_UL_ALLOCATOR(T) 

#define    DEF_ALLOCATOR_(T) 
#define DEF_UL_ALLOCATOR_(T) 
#endif // TOOLSLIB_DONT_USE_ALLOCATORS

/*
class X
{
     void *__cdecl operator TL_NEW(size_t count);
     //void *__cdecl operator TL_NEW(size_t count, void * object) throw();
     //void *__cdecl operator TL_NEW(size_t count, const std::nothrow_t&) throw();
     void __cdecl operator delete(void * object);
     //void __cdecl operator delete(void * object, void * memory) throw();
     //void __cdecl operator delete(void * object, const std::nothrow_t&) throw();

     //void *operator TL_NEW[](std::size_t _Count) throw(std::bad_alloc);
     //void *operator TL_NEW[](std::size_t _Count, const std::nothrow_t&) throw();
     //void *operator TL_NEW[](std::size_t _Count, void* _Ptr) throw();
     //void operator delete[](void* _Ptr) throw();
     //void operator delete[](void *, void *) throw();
     //void operator delete[](void* _Ptr, const std::nothrow_t&) throw();

     static CMaaFixedAllocator<X> * s_pAllocator;
};
X::CMaaFixedAllocator<X> * s_pAllocator = nullptr;
*/
//---------------------------------------------------------------------------
inline _dword CMaaMem04ExpandDword(const unsigned char* p, int count) noexcept // for count == 0..4
{
    return count == 1 ? (_dword)*p : count == 2 ? (_dword) * (_word*)p : count == 3 ? ((_dword) * (_word*)p) | (((_dword)p[2]) << 16) : count == 4 ? *(_dword*)p : 0;
}
//---------------------------------------------------------------------------
template<int count> _dword CMaaMem04ExpandDwordCLK(const unsigned char* p) noexcept // for count == 0..4
{
    if constexpr (count == 1)
    {
        return (_dword)*p;
    }
    else if constexpr (count == 2)
    {
        return (_dword) * (_word*)p;
    }
    else if constexpr (count == 3)
    {
        return ((_dword) * (_word*)p) | (((_dword)p[2]) << 16);
    }
    else if constexpr (count == 4)
    {
        return *(_dword*)p;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline constexpr _dword my_word_ce(const char* p) noexcept
{
#ifdef TOOLSLIB_LITTLE_ENDIAN
    return (_dword)(unsigned char)p[0] | (((_dword)(unsigned char)p[1]) << 8);
#else
    return (_dword)(unsigned char)p[1] | (((_dword)(unsigned char)p[0]) << 8);
#endif
}
//---------------------------------------------------------------------------
inline constexpr _dword my_dword_ce(const char* p) noexcept
{
#ifdef TOOLSLIB_LITTLE_ENDIAN
    return (_dword)(unsigned char)p[0] | (((_dword)(unsigned char)p[1]) << 8) | (((_dword)(unsigned char)p[2]) << 16) | (((_dword)(unsigned char)p[3]) << 24);
#else
    return (_dword)(unsigned char)p[3] | (((_dword)(unsigned char)p[2]) << 8) | (((_dword)(unsigned char)p[1]) << 16) | (((_dword)(unsigned char)p[0]) << 24);
#endif
}
//---------------------------------------------------------------------------
inline constexpr _uqword my_uqword_ce(const char* p) noexcept
{
#ifdef TOOLSLIB_LITTLE_ENDIAN
    return (_uqword)my_dword_ce(p) | (((_uqword)(_uqword)my_dword_ce(p + 4)) << 32);
#else
    return (_uqword)my_dword_ce(p + 4) | (((_uqword)(_uqword)my_dword_ce(p)) << 32);
#endif
}
//---------------------------------------------------------------------------
inline constexpr _dword my_word_ce(const char8_t* p) noexcept
{
#ifdef TOOLSLIB_LITTLE_ENDIAN
    return (_dword)(unsigned char)p[0] | (((_dword)(unsigned char)p[1]) << 8);
#else
    return (_dword)(unsigned char)p[1] | (((_dword)(unsigned char)p[0]) << 8);
#endif
}
//---------------------------------------------------------------------------
inline constexpr _dword my_dword_ce(const char8_t* p) noexcept
{
#ifdef TOOLSLIB_LITTLE_ENDIAN
    return (_dword)(unsigned char)p[0] | (((_dword)(unsigned char)p[1]) << 8) | (((_dword)(unsigned char)p[2]) << 16) | (((_dword)(unsigned char)p[3]) << 24);
#else
    return (_dword)(unsigned char)p[3] | (((_dword)(unsigned char)p[2]) << 8) | (((_dword)(unsigned char)p[1]) << 16) | (((_dword)(unsigned char)p[0]) << 24);
#endif
}
//---------------------------------------------------------------------------
inline constexpr _uqword my_uqword_ce(const char8_t* p) noexcept
{
#ifdef TOOLSLIB_LITTLE_ENDIAN
    return (_uqword)my_dword_ce(p) | (((_uqword)(_uqword)my_dword_ce(p + 4)) << 32);
#else
    return (_uqword)my_dword_ce(p + 4) | (((_uqword)(_uqword)my_dword_ce(p)) << 32);
#endif
}
//---------------------------------------------------------------------------
inline constexpr _dword CMaaMem04ExpandDwordCLK_ce(const char* p, int count) noexcept // for count == 0..4
{
    if (count == 1)
    {
        return (_dword)(unsigned char)p[0];
    }
    else if (count == 2)
    {
        return my_word_ce(p);
    }
    else if (count == 3)
    {
        return my_word_ce(p) | (((_dword)(unsigned char)p[2]) << 16);
    }
    else if (count == 4)
    {
        return my_dword_ce(p);
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline constexpr _dword CMaaMem04ExpandDwordCLK_ce(const char8_t* p, int count) noexcept // for count == 0..4
{
    if (count == 1)
    {
        return (_dword)(unsigned char)p[0];
    }
    else if (count == 2)
    {
        return my_word_ce(p);
    }
    else if (count == 3)
    {
        return my_word_ce(p) | (((_dword)(unsigned char)p[2]) << 16);
    }
    else if (count == 4)
    {
        return my_dword_ce(p);
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
inline _uqword CMaaMem08ExpandUqword(const unsigned char* p, int count) noexcept // for count == 0..8
{
    switch (count)
    {
    case 0:
        return 0;
    case 1:
        return (_uqword)*p;
    case 2:
        return (_uqword) * (_word*)p;
    case 3:
        return ((_uqword) * (_word*)p) | (((_uqword)p[2]) << 16);
    case 4:
        return (_uqword) * (_dword*)p;
    case 5:
        return (_uqword) * (_dword*)p | ((_uqword)p[4] << 32);
    case 6:
        return (_uqword) * (_dword*)p | (((_uqword) * (_word*)(p + 4)) << 32);
    case 7:
        return (_uqword) * (_dword*)p | (((_uqword) * (_word*)(p + 4)) << 32) | (((_uqword)p[6]) << 48);
    }
    return *(_uqword*)p;
}
//---------------------------------------------------------------------------
template<int count> _uqword CMaaMem08ExpandUqwordCLK(const unsigned char* p) noexcept // for count == 0..8
{
    switch (count)
    {
    case 0:
        return 0;
    case 1:
        return (_uqword)*p;
    case 2:
        return (_uqword) * (_word*)p;
    case 3:
        return ((_uqword) * (_word*)p) | (((_uqword)p[2]) << 16);
    case 4:
        return (_uqword) * (_dword*)p;
    case 5:
        return (_uqword) * (_dword*)p | ((_uqword)p[4] << 32);
    case 6:
        return (_uqword) * (_dword*)p | (((_uqword) * (_word*)(p + 4)) << 32);
    case 7:
        return (_uqword) * (_dword*)p | (((_uqword) * (_word*)(p + 4)) << 32) | (((_uqword)p[6]) << 48);
    }
    return *(_uqword*)p;
}
//---------------------------------------------------------------------------
inline constexpr _uqword CMaaMem08ExpandUqwordCLK_ce(const char* p, int count) noexcept // for count == 0..8
{
    switch (count)
    {
    case 0:
        return 0;
    case 1:
        return (_uqword)(unsigned char)p[0];
    case 2:
        return (_uqword)my_word_ce(p);
    case 3:
        return ((_uqword)my_word_ce(p)) | (((_uqword)(unsigned char)p[2]) << 16);
    case 4:
        return (_uqword)my_dword_ce(p);
    case 5:
        return (_uqword)my_dword_ce(p) | ((_uqword)(unsigned char)p[4] << 32);
    case 6:
        return (_uqword)my_dword_ce(p) | (((_uqword)my_word_ce(p + 4)) << 32);
    case 7:
        return (_uqword)my_dword_ce(p) | (((_uqword)my_word_ce(p + 4)) << 32) | (((_uqword)(unsigned char)p[6]) << 48);
    }
    return my_uqword_ce(p);
}
//---------------------------------------------------------------------------
inline constexpr _uqword CMaaMem08ExpandUqwordCLK_ce(const char8_t* p, int count) noexcept // for count == 0..8
{
    switch (count)
    {
    case 0:
        return 0;
    case 1:
        return (_uqword)(unsigned char)p[0];
    case 2:
        return (_uqword)my_word_ce(p);
    case 3:
        return ((_uqword)my_word_ce(p)) | (((_uqword)(unsigned char)p[2]) << 16);
    case 4:
        return (_uqword)my_dword_ce(p);
    case 5:
        return (_uqword)my_dword_ce(p) | ((_uqword)(unsigned char)p[4] << 32);
    case 6:
        return (_uqword)my_dword_ce(p) | (((_uqword)my_word_ce(p + 4)) << 32);
    case 7:
        return (_uqword)my_dword_ce(p) | (((_uqword)my_word_ce(p + 4)) << 32) | (((_uqword)(unsigned char)p[6]) << 48);
    }
    return my_uqword_ce(p);
}
//---------------------------------------------------------------------------
#if 0
#define CMaaRol7dwMacro(x) (((x) << 7) | ((x) >> 25)) // ror eax,19h
#define CMaaRol7qwMacro(x) (((x) << 7) | ((x) >> 57)) // ror rax,39h
//#define CMaaRol7dwMacro(x) (((x) >> 25) | ((x) << 7)) // ror eax,19h
//#define CMaaRol7qwMacro(x) (((x) >> 57) | ((x) << 7)) // ror rax,39h
//inline constexpr _dword CMaaRor19h(_dword x) noexcept { return CMaaRol7dwMacro(x); }
//inline constexpr _uqword CMaaRor39h(_uqword x) noexcept { return CMaaRol7qwMacro(x); }
inline constexpr _dword CMaaRol7dw(_dword x) noexcept { return CMaaRol7dwMacro(x); }
inline constexpr _uqword CMaaRol7qw(_uqword x) noexcept { return CMaaRol7qwMacro(x); }

#define CMaaRol14dwMacro(x) (((x) << 14) | ((x) >> 18))
#define CMaaRol14qwMacro(x) (((x) << 14) | ((x) >> 50))
inline constexpr _dword CMaaRol14dw(_dword x) noexcept { return CMaaRol14dwMacro(x); }
inline constexpr _uqword CMaaRol14qw(_uqword x) noexcept { return CMaaRol14qwMacro(x); }
#else
#ifndef _MaaRF_INTERNAL_BUILD
#define CMaaRol7dwMacro(x) std::rotl(x, 7)
#define CMaaRol7qwMacro(x) std::rotl(x, 7)
#define CMaaRol7dw(x) std::rotl(x, 7)
#define CMaaRol7qw(x) std::rotl(x, 7)

#define CMaaRol14dwMacro(x) std::rotl(x, 14)
#define CMaaRol14qwMacro(x) std::rotl(x, 14)
#define CMaaRol14dw(x) std::rotl(x, 14)
#define CMaaRol14qw(x) std::rotl(x, 14)
#else
#define CMaaRol7dwMacro(x) _rotl(x, 7)
#define CMaaRol7qwMacro(x) _rotl64(x, 7)
//inline _dword CMaaRol7(_dword x) noexcept { return CMaaRol7dwMacro(x); }
//inline _uqword CMaaRol7(_uqword x) noexcept { return CMaaRol7qwMacro(x); }
#define CMaaRol7dw(x) CMaaRol7dwMacro(x)
#define CMaaRol7qw(x) CMaaRol7qwMacro(x)

#define CMaaRol14dwMacro(x) _rotl(x, 14)
#define CMaaRol14qwMacro(x) _rotl64(x, 14)
#define CMaaRol14dw(x) CMaaRol14dwMacro(x)
#define CMaaRol14qw(x) CMaaRol14qwMacro(x)
#endif
#endif
//---------------------------------------------------------------------------
inline _dword CMaaStandardHashFunction(const unsigned char* p, int len) noexcept
{
    if (len <= (int)sizeof(_dword))
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        return CMaaMem04ExpandDword(p, len) ^ (_dword)len;
#else
        return CMaaMem04ExpandDword(p, len);
#endif
    }
    _dword x;
#ifdef TRY_HF_OPT
#if defined(_WIN64) || defined(__linux__)
    if (len >= 2 * (int)sizeof(_uqword))
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        _uqword q = len;
#else
        _uqword q = 0;
#endif
        for (int i = len >> _qword_shift; i--; )
        {
            q = CMaaRol7qwMacro(q) ^ *(_uqword*)p;
            p += sizeof(_uqword);
        }
        x = (_dword)(q >> 32);
        x = CMaaRol7dwMacro(x) ^ (_dword)q;
        if ((len & (1 << _dword_shift)))
        {
            x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
            p += sizeof(_dword);

        }
        if ((len & _dword_mask))
        {
            x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDword(p, (len & _dword_mask));
        }
    }
    else
#endif
    {
#ifdef TOOLSLIB_HASH_USE_LEN
        x = len;
#else
        x = 0;
#endif
        for (int i = len >> _dword_shift; i--; )
        {
            x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
            p += sizeof(_dword);
        }
        if ((len & _dword_mask))
        {
            x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDword(p, (len & _dword_mask));
        }
    }
#else
#ifdef TOOLSLIB_HASH_USE_LEN
    x = len;
#else
    x = 0;
#endif
    for (int i = len; i--; )
    {
        x = CMaaRol7dwMacro(x) ^ *p++;
    }
#endif
    return x;
}
//---------------------------------------------------------------------------
inline _uqword CMaaStandardHashFunction64(const unsigned char* p, int len) noexcept
{
    _uqword x;
    if (len <= (int)sizeof(_uqword))
    {
        x = 0;
        memcpy(&x, p, len);
#ifdef TOOLSLIB_HASH_USE_LEN
        return x ^ (_dword)len;
#else
        return x;
#endif
    }
#ifdef TOOLSLIB_HASH_USE_LEN
    x = len;
#else
    x = 0;
#endif
#ifdef TRY_HF_OPT
    for (int i = len >> _qword_shift; i--; )
    {
        x = CMaaRol7qwMacro(x) ^ *(_uqword*)p;
        p += sizeof(_uqword);
    }
    if ((len & _qword_mask))
    {
        x = CMaaRol7qwMacro(x) ^ CMaaMem08ExpandUqword(p, (len & _qword_mask));
    }
#else
    for (int i = len; i--; )
    {
        x = CMaaRol7qwMacro(x) ^ *p++;
    }
#endif
    return x;
}
//---------------------------------------------------------------------------
// for const length key
//---------------------------------------------------------------------------
template<int len> _dword CMaaStandardHashFunctionCLK(const unsigned char* p) noexcept
{
    if constexpr (len < (int)sizeof(_dword))
    {
        return CMaaMem04ExpandDwordCLK<len>(p);
    }
    else
    {
        _dword x;
#ifdef TRY_HF_OPT
#if defined(_WIN64) || defined(__linux__)
        if constexpr (len >= 2 * (int)sizeof(_uqword))
        {
            _uqword q = CMaaRol7qwMacro(*(_uqword*)p) ^ *(_uqword*)(p + sizeof(_uqword));
            p += 2 * sizeof(_uqword);
            for (int i = (len >> _qword_shift) - 2; i--; )
            {
                q = CMaaRol7qwMacro(q) ^ *(_uqword*)p;
                p += sizeof(_uqword);
            }
            x = (_dword)(q >> 32);
            x = CMaaRol7dwMacro(x) ^ (_dword)q;
            if constexpr ((len & (1 << _dword_shift)))
            {
                x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
                p += sizeof(_dword);
            }
            if constexpr ((len & _dword_mask))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK<(len & _dword_mask)>(p);
            }
        }
        else
#endif
        {
            x = *(_dword*)p;
            p += sizeof(_dword);
            for (int i = len >> _dword_shift; --i; )
            {
                x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
                p += sizeof(_dword);
            }
            if constexpr ((len & _dword_mask))
            {
                x = CMaaRol7dwMacro(x) ^ CMaaMem04ExpandDwordCLK<(len & _dword_mask)>(p);
            }
        }
#else
        {
            x = 0;
            for (int i = len; i--; )
            {
                x = CMaaRol7dwMacro(x) ^ *p++;
            }
        }
#endif
        return x;
    }
}
//---------------------------------------------------------------------------
// for const length key
//---------------------------------------------------------------------------
template<int len> _uqword CMaaStandardHashFunction64CLK(const unsigned char* p) noexcept
{
    if constexpr (len < (int)sizeof(_uqword))
    {
        return CMaaMem08ExpandUqwordCLK<len>(p);;
    }
    else
    {
#ifdef TRY_HF_OPT
        _uqword x = *(_uqword*)p;
        p += sizeof(_uqword);
        for (int i = len >> _qword_shift; --i; )
        {
            x = CMaaRol7qwMacro(x) ^ *(_uqword*)p;
            p += sizeof(_uqword);
        }
        if constexpr ((len & _qword_mask))
        {
            x = CMaaRol7qwMacro(x) ^ CMaaMem08ExpandUqwordCLK<(len & _qword_mask)>(p);
        }
#else
        _uqword x = 0;
        for (int i = len; i--; )
        {
            x = CMaaRol7qwMacro(x) ^ *p++;
        }
#endif
        return x;
    }
}
//---------------------------------------------------------------------------
/*
#ifdef _WIN64
extern "C"
{
    _dword CMaaStandardHashFunction32_win(const void* p, _dword len) noexcept;
    _uqword CMaaStandardHashFunction64_win(const void* p, _dword len) noexcept;
}
//#define CMaaStandardHashFunction CMaaStandardHashFunction32_win // +2.1% speed if len=1000, -10% if len=8
//#define CMaaStandardHashFunction64 CMaaStandardHashFunction64_win // -12.5% speed
#endif
*/
//---------------------------------------------------------------------------
template <class Key> _dword CMaaHashFunction(const Key& k) noexcept
{
    if constexpr (sizeof(Key) == sizeof(_dword))
    {
        return *(_dword*)&k;
    }
#ifdef TRY_HF8_OPT
    else if constexpr (sizeof(Key) == 2 * sizeof(_dword))
    {
        return CMaaRol7dw(*(_dword*)&k) ^ *(1 + (_dword *)&k);
        //return *((_dword*)&k) ^ *(1 + (_dword*)&k);
        //return *(1 + (_dword *)&k) ^ *(_dword *)&k;
    }
    else if constexpr (sizeof(Key) == sizeof(_word))
    {
        return *(_word*)&k;
    }
#endif
    else
    {
        return CMaaStandardHashFunctionCLK<(int)sizeof(Key)>((const unsigned char*)&k);
    }
}
//---------------------------------------------------------------------------
template <class Key> _uqword CMaaHashFunction64(const Key& k) noexcept
{
    if constexpr (sizeof(Key) == sizeof(_dword))
    {
        return *(_dword*)&k;
    }
#ifdef TRY_HF8_OPT
    else if constexpr (sizeof(Key) == sizeof(_uqword))
    {
        return *((_uqword*)&k);
    }
    else if constexpr (sizeof(Key) == 2 * sizeof(_uqword))
    {
        return CMaaRol7qw(*((_uqword*)&k)) ^ *(1 + (_uqword*)&k);
    }
    else if constexpr (sizeof(Key) == sizeof(_word))
    {
        return *(_word*)&k;
    }
#endif
    else
    {
        return CMaaStandardHashFunction64CLK<(int)sizeof(Key)>((const unsigned char*)&k);
    }
}
//---------------------------------------------------------------------------
template <class Key> _dword CMaaHashFunction2(const Key k) noexcept
{
    if constexpr (sizeof(Key) <= sizeof(_dword))
    {
        return (_dword)k;
    }
#ifdef TRY_HF8_OPT
    else if constexpr (sizeof(Key) == 2 * sizeof(_dword))
    {
        return ((_dword)k) ^ (_dword)(k >> 32);
    }
#endif
    else
    {
        return CMaaStandardHashFunctionCLK<(int)sizeof(Key)>((const unsigned char*)&k);
    }
}
//---------------------------------------------------------------------------
template <class Key> _uqword CMaaHashFunction642(const Key k) noexcept
{
    if constexpr (sizeof(Key) <= sizeof(_dword))
    {
        return (_dword)k;
    }
#ifdef TRY_HF8_OPT
    else if constexpr (sizeof(Key) == sizeof(_uqword))
    {
        return ((_uqword)k);
    }
    /*
    else if constexpr (sizeof(Key) == 2 * sizeof(_uqword))
    {
        return ((_uqword)k) ^ (_uqword)(k >> 64);
    }
    */
#endif
    else
    {
        return CMaaStandardHashFunction64CLK<(int)sizeof(Key)>((const unsigned char*)&k);
    }
}
//---------------------------------------------------------------------------
//template <> _dword CMaaHashFunction<CMaaString>(const CMaaString & k) noexcept;
//template <> _dword CMaaHashFunction<CMaaString>(const CMaaString & k) noexcept
//{
//     ...
//}

// Use <int> or <_dword> version instead of <short>/<_word> for performance
template <> _dword CMaaHashFunction<short>(const short& k) noexcept;
template <> _uqword CMaaHashFunction64<short>(const short& k) noexcept;
//#ifndef __unix__
// Use <int> or <_dword> version instead of <short>/<_word> for performance
template <> _dword CMaaHashFunction<_word>(const _word& k) noexcept;
template <> _uqword CMaaHashFunction64<_word>(const _word& k) noexcept;
//#endif
template <> _dword CMaaHashFunction<int>(const int& k) noexcept;
template <> _uqword CMaaHashFunction64<int>(const int& k) noexcept;
//#ifndef __unix__
template <> _dword CMaaHashFunction<_dword>(const _dword& k) noexcept;
template <> _uqword CMaaHashFunction64<_dword>(const _dword& k) noexcept;
//#endif
template <> _uqword CMaaHashFunction64<_qword>(const _qword& k) noexcept;
template <> _uqword CMaaHashFunction64<_uqword>(const _uqword& k) noexcept;
template <> _dword CMaaHashFunction<unsigned>(const unsigned& k) noexcept;
template <> _uqword CMaaHashFunction64<unsigned>(const unsigned& k) noexcept;
//inline _dword CMaaHashFunction(const int &k) noexcept { return k; }
////inline _dword CMaaHashFunction(const _dword &k) noexcept { return k; }
//inline _dword CMaaHashFunction(const unsigned &k) noexcept { return k; }
class CMaaString;
template <> _dword CMaaHashFunction<CMaaString>(const CMaaString& k) noexcept;
inline _dword CMaaHashFunction(const CMaaString& k) noexcept;
template <> _uqword CMaaHashFunction64<CMaaString>(const CMaaString& k) noexcept;
inline _uqword CMaaHashFunction64(const CMaaString& k) noexcept;
//---------------------------------------------------------------------------
// Use (int&) or (_dword&) version instead of (short&)/(_word&) for performance
inline _dword CMaaHashFunction(const short& k) noexcept
{
    return (_dword)k;
}
//---------------------------------------------------------------------------
inline _uqword CMaaHashFunction64(const short& k) noexcept
{
    return (_dword)k;
}
//#ifndef __unix__
// Use (int&) or (_dword&) version instead of (short&)/(_word&) for performance
inline _dword CMaaHashFunction(const _word& k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction64(const _word& k) noexcept
{
    return (_dword)k;
}
//#endif
//---------------------------------------------------------------------------
inline _dword CMaaHashFunction(const int& k) noexcept
{
    return (_dword)k;
}
//---------------------------------------------------------------------------
inline _uqword CMaaHashFunction64(const int& k) noexcept
{
    return (_dword)k;
}
#ifndef __unix__
inline _dword CMaaHashFunction(const _dword& k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction64(const _dword& k) noexcept
{
    return (_dword)k;
}
#endif
inline _dword CMaaHashFunction(const unsigned& k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction64(const unsigned& k) noexcept
{
    return (_dword)k;
}
inline _dword CMaaHashFunction(const _qword& k) noexcept
{
    return ((_dword)k) ^ (_dword)(k >> 32);
}
inline _uqword CMaaHashFunction64(const _qword& k) noexcept
{
    return ((_uqword)k);
}
inline _dword CMaaHashFunction(const _uqword& k) noexcept
{
    return ((_dword)k) ^ (_dword)(k >> 32);
}
inline _uqword CMaaHashFunction64(const _uqword& k) noexcept
{
    return ((_uqword)k);
}
//---------------------------------------------------------------------------
inline _dword CMaaHashFunction2(int k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction642(int k) noexcept
{
    return (_dword)k;
}
#ifndef __unix__
inline _dword CMaaHashFunction2(_dword k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction642(_dword k) noexcept
{
    return (_dword)k;
}
#endif
inline _dword CMaaHashFunction2(unsigned k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction642(unsigned k) noexcept
{
    return (_dword)k;
}
inline _dword CMaaHashFunction2(_word k) noexcept
{
    return (_dword)k;
}
inline _uqword CMaaHashFunction642(_word k) noexcept
{
    return (_dword)k;
}
inline _dword CMaaHashFunction2(_qword k) noexcept
{
    return ((_dword)k) ^ (_dword)(k >> 32);
}
inline _uqword CMaaHashFunction642(_qword k) noexcept
{
    return (_uqword)k;
}
inline _dword CMaaHashFunction2(_uqword k) noexcept
{
    return ((_dword)k) ^ (_dword)(k >> 32);
}
inline _uqword CMaaHashFunction642(_uqword k) noexcept
{
    return (_uqword)k;
}
template <> _dword CMaaHashFunction2<int>(const int k) noexcept;
template <> _uqword CMaaHashFunction642<int>(const int k) noexcept;
//#ifndef __unix__
template <> _dword CMaaHashFunction2<_dword>(const _dword k) noexcept;
template <> _uqword CMaaHashFunction642<_dword>(const _dword k) noexcept;
//#endif
template <> _dword CMaaHashFunction2<unsigned>(const unsigned k) noexcept;
template <> _uqword CMaaHashFunction642<unsigned>(const unsigned k) noexcept;
template <> _dword CMaaHashFunction2<_word>(const _word k) noexcept;
template <> _uqword CMaaHashFunction642<_word>(const _word k) noexcept;
template <> _dword CMaaHashFunction2<_qword>(const _qword k) noexcept;
template <> _uqword CMaaHashFunction642<_qword>(const _qword k) noexcept;
template <> _dword CMaaHashFunction2<_uqword>(const _uqword k) noexcept;
template <> _uqword CMaaHashFunction642<_uqword>(const _uqword k) noexcept;
//---------------------------------------------------------------------------
#define CMaaUnivHash_IsNew
/*
#ifdef CMaaUnivHash_IsNew
#define CMaaUnivHash CMaaUnivHash_New
#define CMaaUnivHash_KeyIsClass CMaaUnivHash_New_KeyIsClass
#define CMaaUnivHash_ClassPtr CMaaUnivHash_NewClassPtr
#else
#define CMaaUnivHash CMaaUnivHash_Fast
#define CMaaUnivHash_KeyIsClass CMaaUnivHash_Fast_KeyIsClass
#define CMaaUnivHash_ClassPtr CMaaUnivHash_FastClassPtr
#endif
#define CMaaUnivHash2 CMaaUnivHash_New2
*/
//---------------------------------------------------------------------------

//for shared allocator use static allocator object definition like:
//DEF_ALLOCATOR_CMaaUnivHash(CMaaString, char);

//---------------------------------------------------------------------------
enum eTOOLSLIB_CHECK_HT_USAGE_
{
    eTOOLSLIB_CHECK_HT_USAGE
};
typedef int TOOLSLIB_HT_UNUSED_DATA_TYPE;
#define TOOLSLIB_HT_UNUSED_DATA0 0
//typedef eTOOLSLIB_CHECK_HT_USAGE_ TOOLSLIB_HT_UNUSED_DATA_TYPE;
//#define TOOLSLIB_HT_UNUSED_DATA0 eTOOLSLIB_CHECK_HT_USAGE
//---------------------------------------------------------------------------
#include "MaaHT.h"

#if !defined(_MaaRF_INTERNAL_BUILD) && (defined(_WIN64) || defined(__unix__))
#define TOOLSLIB_HT64
#include "MaaHT.h"
#else
#define DEF4_ALLOCATOR_CMaaUnivHash64(A,B,F,X)
#define DEF3_ALLOCATOR_CMaaUnivHash64(A,B,F)
#define DEF_ALLOCATOR_CMaaUnivHash64(A,B)
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// CMaaUnivHash_New2 has the same speed as CMaaUnivHash_New
//---------------------------------------------------------------------------
//#include "Hash_New2.h"
//---------------------------------------------------------------------------

#include "MaaTL_2.h"

//---------------------------------------------------------------------------
#ifdef _WIN32_DRV_ALIGN_8
#pragma pack( pop, temp_pack_id_2 )
#endif
//---------------------------------------------------------------------------

#if 1
//---------------------------------------------------------------------------
struct CMaa256Bits
{
protected:
    unsigned char m[256 / 8];
public:
    CMaa256Bits() noexcept
    {
        Clear();
    }
    CMaa256Bits(const CMaa256Bits& That) noexcept
    {
        *this = That;
    }
    CMaa256Bits(const char* ptr, int len) noexcept
    {
        Clear();
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
    }
    CMaa256Bits(const unsigned char* ptr, int len) noexcept
    {
        Clear();
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
    }
    constexpr CMaa256Bits(const char* ptr, int len, bool bInvert) noexcept
    {
        ceClear();
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
        if (bInvert)
        {
            ceInvert();
        }
    }
    constexpr CMaa256Bits(const unsigned char* ptr, int len, bool bInvert) noexcept
    {
        ceClear();
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
        if (bInvert)
        {
            ceInvert();
        }
    }
    CMaa256Bits(unsigned char c0, unsigned char c1) noexcept
    {
        Clear();
        Set(c0, c1);
    }
    constexpr CMaa256Bits(unsigned char c0, unsigned char c1, bool bInvert) noexcept
    {
        ceClear();
        Set(c0, c1);
        if (bInvert)
        {
            ceInvert();
        }
    }
    CMaa256Bits(char c0, char c1) noexcept
    {
        Clear();
        Set((unsigned char)c0, (unsigned char)c1);
    }
    constexpr CMaa256Bits(char c0, char c1, bool bInvert) noexcept
    {
        ceClear();
        Set((unsigned char)c0, (unsigned char)c1);
        if (bInvert)
        {
            ceInvert();
        }
    }
    constexpr ~CMaa256Bits()
    {
    }
    CMaa256Bits& operator=(const CMaa256Bits& That) noexcept
    {
        memcpy(m, That.m, sizeof(m));
        return *this;
    }
    bool operator==(const CMaa256Bits& That) noexcept
    {
        return !memcmp(m, That.m, sizeof(m));
    }
    bool operator!=(const CMaa256Bits& That) noexcept
    {
        return memcmp(m, That.m, sizeof(m));
    }
    void Set() noexcept
    {
        memset(m, -1, sizeof(m));
    }
    constexpr void Set(char c) noexcept
    {
        m[((unsigned char)c) >> 3] |= (1 << (c & 7));
    }
    constexpr void Set(unsigned char c) noexcept
    {
        m[c >> 3] |= (1 << (c & 7));
    }
    constexpr void Set(unsigned char c0, unsigned char c1) noexcept
    {
        if (c0 <= c1)
        {
            const int idx0 = (c0 >> 3);
            const int idx1 = (c1 >> 3);
            const unsigned char m0 = (0xff << (c0 & 7));
            const unsigned char m1 = (0xff >> (7 - (c1 & 7)));
            if (idx0 == idx1)
            {
                m[idx0] |= (m0 & m1);
            }
            else
            {
                m[idx0] |= m0;
                //memset(m + idx0 + 1, -1, idx1 - idx0 - 1);
                for (int i = idx0 + 1; i < idx1; i++)
                {
                    m[i] = 0xff;
                }
                m[idx1] |= m1;
            }
        }
    }
    constexpr void Set(char c0, char c1) noexcept
    {
        Set((unsigned char)c0, (unsigned char)c1);
    }
    constexpr bool Test(char c) const noexcept
    {
        return (m[((unsigned char)c) >> 3] & (1 << (c & 7)));
    }
    constexpr bool Test(unsigned char c) const noexcept
    {
        return (m[c >> 3] & (1 << (c & 7)));
    }
    void Clear() noexcept
    {
        memset(m, 0, sizeof(m));
    }
    constexpr void ceClear() noexcept
    {
        for (int i = 0; i < (int)sizeof(m); i++)
        {
            m[i] = 0;
        }
    }
    constexpr void Clear(char c) noexcept
    {
        m[((unsigned char)c) >> 3] &= ~(1 << (c & 7));
    }
    constexpr void Clear(unsigned char c) noexcept
    {
        m[c >> 3] &= ~(1 << (c & 7));
    }
#if 0
    void Clear(unsigned char c0, unsigned char c1) noexcept // v. invert
    {
        if (c0 <= c1)
        {
            const int idx0 = (c0 >> 3);
            const int idx1 = (c1 >> 3);
            const unsigned char m0 = (0xff << (c0 & 7));
            const unsigned char m1 = (0xff >> (7 - (c1 & 7)));
            if (idx0 == idx1)
            {
                m[idx0] &= ~(m0 & m1);
            }
            else
            {
                m[idx0] &= ~m0;
                memset(m + idx0 + 1, 0, idx1 - idx0 - 1);
                m[idx1] &= ~m1;
            }
        }
    }
#else
    void Clear(unsigned char c0, unsigned char c1) noexcept // v. direct
    {
        if (c0 <= c1)
        {
            const int idx0 = (c0 >> 3);
            const int idx1 = (c1 >> 3);
            const unsigned char m0 = (0xff >> (8 - (c0 & 7)));
            const unsigned char m1 = (0xff << (1 + (c1 & 7)));
            if (idx0 == idx1)
            {
                m[idx0] &= (m0 | m1);
            }
            else
            {
                m[idx0] &= m0;
                memset(m + idx0 + 1, 0, idx1 - idx0 - 1);
                m[idx1] &= m1;
            }
        }
    }
#endif
    void Clear(char c0, char c1) noexcept
    {
        Clear((unsigned char)c0, (unsigned char)c1);
    }
    constexpr void ceInvert() noexcept
    {
        for (int i = 0; i < (int)sizeof(m); i++)
        {
            m[i] ^= 0xff;
        }
    }
    void Invert() noexcept
    {
        for (int i = 0; i < (int)(sizeof(m) / sizeof(_qword)); i++)
        {
            *(i + (_qword*)m) ^= (_qword)-1;
        }
    }
    CMaa256Bits& operator|=(const CMaa256Bits& That) noexcept
    {
        for (int i = 0; i < (int)(sizeof(m) / sizeof(_qword)); i++)
        {
            *(i + (_qword*)m) |= *(i + (_qword*)That.m);
        }
        return *this;
    }
    CMaa256Bits& operator&=(const CMaa256Bits& That) noexcept
    {
        for (int i = 0; i < (int)(sizeof(m) / sizeof(_qword)); i++)
        {
            *(i + (_qword*)m) &= *(i + (_qword*)That.m);
        }
        return *this;
    }
    CMaa256Bits& operator^=(const CMaa256Bits& That) noexcept
    {
        for (int i = 0; i < (int)(sizeof(m) / sizeof(_qword)); i++)
        {
            *(i + (_qword*)m) ^= *(i + (_qword*)That.m);
        }
        return *this;
    }
    bool Test(const CMaa256Bits& That) const noexcept
    {
        for (int i = 0; i < (int)(sizeof(m) / sizeof(_qword)); i++)
        {
            if ((*(i + (_qword*)m) & *(i + (_qword*)That.m)))
            {
                return true;
            }
        }
        return false;
    }
    char* ToText(char* p257) const noexcept
    {
        for (int i = 0; i < 256; i++)
        {
            p257[i] = Test((unsigned char)i) ? '1' : '.';
        }
        p257[256] = '\0';
        return p257;
    }
};
//---------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------
struct CMaa256Bits
{
protected:
    union mm
    {
        unsigned char m[256 / 8];
        _uqword q[256 / 8 / 8]; // == 4
        mm() noexcept {}
        constexpr mm(bool bQword) noexcept
        {
            if (bQword)
            {
                q[3] = q[2] = q[1] = q[0] = 0;
            }
            else
            {
                for (int i = 0; i < (int)sizeof(m); i++)
                {
                    m[i] = (unsigned char)0;
                }
            }
        }
        constexpr mm(const _uqword* p) noexcept
        {
            q[0] = p[0];
            q[1] = p[1];
            q[2] = p[2];
            q[3] = p[3];
        }
        constexpr mm(const unsigned char * p) noexcept
        {
            for (int i = 0; i < (int)sizeof(m); i++)
            {
                m[i] = p[i];
            }
        }
    } m;
public:
    constexpr CMaa256Bits() noexcept
    :   m(true)
    {
    }
    constexpr CMaa256Bits(const CMaa256Bits &That) noexcept
    :   m(That.m.q)
    {
    }
    CMaa256Bits(const char* ptr, int len) noexcept
    {
        Clear();
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
    }
    CMaa256Bits(const unsigned char* ptr, int len) noexcept
    {
        Clear();
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
    }
    constexpr CMaa256Bits(const char* ptr, int len, bool bInvert) noexcept
    :   m(false)
    {
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
        if (bInvert)
        {
            ceBInvert();
        }
    }
    constexpr CMaa256Bits(const unsigned char* ptr, int len, bool bInvert) noexcept
    :   m(false)
    {
        for (int i = 0; i < len; i++)
        {
            Set(ptr[i]);
        }
        if (bInvert)
        {
            ceBInvert();
        }
    }
    CMaa256Bits(unsigned char c0, unsigned char c1) noexcept
    {
        Clear();
        Set(c0, c1);
    }
    constexpr CMaa256Bits(unsigned char c0, unsigned char c1, bool bInvert) noexcept
    :   m(true)
    {
        Set(c0, c1);
        if (bInvert)
        {
            ceQInvert();
        }
    }
    CMaa256Bits(char c0, char c1) noexcept
    {
        Clear();
        Set((unsigned char)c0, (unsigned char)c1);
    }
    constexpr CMaa256Bits(char c0, char c1, bool bInvert) noexcept
    :   m(true)
    {
        Set((unsigned char)c0, (unsigned char)c1);
        if (bInvert)
        {
            ceQInvert();
        }
    }
    constexpr ~CMaa256Bits()
    {
    }
    constexpr CMaa256Bits& operator=(const CMaa256Bits& That) noexcept
    {
        m.q[0] = That.m.q[0];
        m.q[1] = That.m.q[1];
        m.q[2] = That.m.q[2];
        m.q[3] = That.m.q[3];
        return *this;
    }
    constexpr bool operator==(const CMaa256Bits& That) noexcept
    {
        return m.q[0] == That.m.q[0] && m.q[1] == That.m.q[1] && m.q[2] == That.m.q[2] && m.q[3] == That.m.q[3];
    }
    constexpr bool operator!=(const CMaa256Bits& That) noexcept
    {
        return m.q[0] != That.m.q[0] || m.q[1] != That.m.q[1] || m.q[2] != That.m.q[2] || m.q[3] != That.m.q[3];
    }
    constexpr void Set() noexcept
    {
        m.q[3] = m.q[2] = m.q[1] = m.q[0] = (_uqword)-1;
    }
    constexpr void Set(char c) noexcept
    {
        m.m[((unsigned char)c) >> 3] |= (1 << (c & 7));
    }
    constexpr void Set(unsigned char c) noexcept
    {
        m.m[c >> 3] |= (1 << (c & 7));
    }
    constexpr void Set(unsigned char c0, unsigned char c1) noexcept
    {
        if (c0 <= c1)
        {
            const int idx0 = (c0 >> 6);
            const int idx1 = (c1 >> 6);
            const _uqword m0 = ((_uqword)-1) << (c0 & 63);
            const _uqword m1 = ((_uqword)-1) >> (63 - (c1 & 63));
            if (idx0 == idx1)
            {
                m.q[idx0] |= (m0 & m1);
            }
            else
            {
                m.q[idx0] |= m0;
                for (int i = idx0 + 1; i < idx1; i++)
                {
                    m.q[i] = (_uqword)-1;
                }
                m.q[idx1] |= m1;
            }
        }
    }
    constexpr void Set(char c0, char c1) noexcept
    {
        Set((unsigned char)c0, (unsigned char)c1);
    }
    constexpr bool Test(char c) const noexcept
    {
        return (m.m[((unsigned char)c) >> 3] & (1 << (c & 7)));
    }
    constexpr bool Test(unsigned char c) const noexcept
    {
        return (m.m[c >> 3] & (1 << (c & 7)));
    }
    constexpr void Clear() noexcept
    {
        m.q[3] = m.q[2] = m.q[1] = m.q[0] = 0;
    }
    constexpr void ceBClear() noexcept
    {
        for (int i = 0; i < (int)sizeof(m.m); i++)
        {
            m.m[i] = 0;
        }
    }
    constexpr void ceQClear() noexcept
    {
        m.q[3] = m.q[2] = m.q[1] = m.q[0] = 0;
    }
    constexpr void Clear(char c) noexcept
    {
        m.m[((unsigned char)c) >> 3] &= ~(1 << (c & 7));
    }
    constexpr void Clear(unsigned char c) noexcept
    {
        m.m[c >> 3] &= ~(1 << (c & 7));
    }
#if 0
    void Clear(unsigned char c0, unsigned char c1) noexcept // v. invert
    {
        if (c0 <= c1)
        {
            const int idx0 = (c0 >> 3);
            const int idx1 = (c1 >> 3);
            const unsigned char m0 = (0xff << (c0 & 7));
            const unsigned char m1 = (0xff >> (7 - (c1 & 7)));
            if (idx0 == idx1)
            {
                m[idx0] &= ~(m0 & m1);
            }
            else
            {
                m[idx0] &= ~ m0;
                memset(m + idx0 + 1, 0, idx1 - idx0 - 1);
                m[idx1] &= ~ m1;
            }
        }
    }
#else
    constexpr void Clear(unsigned char c0, unsigned char c1) noexcept // v. direct
    {
        if (c0 <= c1)
        {
            const int idx0 = (c0 >> 6);
            const int idx1 = (c1 >> 6);
            const _uqword m0 = ((_uqword)-1) >> (64 - (c0 & 63));
            const _uqword m1 = ((_uqword)-1) << (1 + (c1 & 63));
            if (idx0 == idx1)
            {
                m.q[idx0] &= (m0 | m1);
            }
            else
            {
                m.q[idx0] &= m0;
                for (int i = idx0 + 1; i < idx1; i++)
                {
                    m.q[i] = 0;
                }
                m.q[idx1] &= m1;
            }
        }
    }
#endif
    constexpr void Clear(char c0, char c1) noexcept
    {
        Clear((unsigned char)c0, (unsigned char)c1);
    }
    constexpr void ceBInvert() noexcept
    {
        for (int i = 0; i < (int)sizeof(m.m); i++)
        {
            m.m[i] ^= 0xff;
        }
    }
    constexpr void ceQInvert() noexcept
    {
        m.q[0] ^= (_uqword)-1;
        m.q[1] ^= (_uqword)-1;
        m.q[2] ^= (_uqword)-1;
        m.q[3] ^= (_uqword)-1;
    }
    constexpr void Invert() noexcept
    {
        m.q[0] ^= (_uqword)-1;
        m.q[1] ^= (_uqword)-1;
        m.q[2] ^= (_uqword)-1;
        m.q[3] ^= (_uqword)-1;
    }
    constexpr CMaa256Bits& operator|=(const CMaa256Bits& That) noexcept
    {
        m.q[0] |= That.m.q[0];
        m.q[1] |= That.m.q[1];
        m.q[2] |= That.m.q[2];
        m.q[3] |= That.m.q[3];
        return *this;
    }
    constexpr CMaa256Bits& operator&=(const CMaa256Bits& That) noexcept
    {
        m.q[0] &= That.m.q[0];
        m.q[1] &= That.m.q[1];
        m.q[2] &= That.m.q[2];
        m.q[3] &= That.m.q[3];
        return *this;
    }
    constexpr CMaa256Bits& operator^=(const CMaa256Bits& That) noexcept
    {
        m.q[0] ^= That.m.q[0];
        m.q[1] ^= That.m.q[1];
        m.q[2] ^= That.m.q[2];
        m.q[3] ^= That.m.q[3];
        return *this;
    }
    char* ToText(char* p257) const noexcept
    {
        for (int i = 0; i < 256; i++)
        {
            p257[i] = Test((unsigned char)i) ? '1' : '.';
        }
        p257[256] = '\0';
        return p257;
    }
};
#endif
//---------------------------------------------------------------------------

template<int N> class CMaaBuffer_char
{
public:
    char ptr[N];
    ADD_ALLOCATOR(CMaaBuffer_char<N>)
};
//DEF_ALLOCATOR_(CMaaBuffer_char<1024>)

template<int N> class CMaaBuffer_uchar
{
public:
    unsigned char ptr[N];
    ADD_ALLOCATOR(CMaaBuffer_uchar<N>)
};
//DEF_ALLOCATOR_(CMaaBuffer_uchar<1024>)

template<int N> class CMaaBuffer_int
{
public:
    int ptr[N];
    ADD_ALLOCATOR(CMaaBuffer_int<N>)
};
//DEF_ALLOCATOR_(CMaaBuffer_int<1024>)

#endif // __MAARF_MAATL_H
