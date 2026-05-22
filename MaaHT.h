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

//#ifndef __MAARF_MAATL_H
//#define __MAARF_MAATL_H

//for shared allocator use static allocator object definition like:
//DEF_ALLOCATOR_CMaaUnivHash(CMaaString, char)

//#define TOOLSLIB_CHECK_HT_USAGE

#ifndef _RR
#define std_move std::move
#endif

#ifdef TOOLSLIB_CHECK_HT_USAGE
#define TOOLSLIB_CHECK_HT_USAGE_ARG1 , eTOOLSLIB_CHECK_HT_USAGE
#define TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1 , eTOOLSLIB_CHECK_HT_USAGE_
#define TOOLSLIB_CHECK_HT_USAGE_ARG2 , TOOLSLIB_HT_DEF_FLAG TOOLSLIB_CHECK_HT_USAGE_ARG1
#else
#define TOOLSLIB_CHECK_HT_USAGE_ARG1
#define TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#define TOOLSLIB_CHECK_HT_USAGE_ARG2
#endif

//#define TOOLSLIB_HT64
#ifndef TOOLSLIB_HT64
#define TOOLSLIB_HT_HASH _dword
#define CMaaUnivHash_New CMaaUnivHash
#define CMaaUnivHash_New_KeyIsClass CMaaUnivHash_KeyIsClass
#define CMaaUnivHash_NewClassPtr CMaaUnivHash_ClassPtr
#define CMaaUnivHash_NewA CMaaUnivHashA
#else
#define TOOLSLIB_HT_HASH _uqword
#define CMaaUnivHash_New CMaaUnivHash64
#define CMaaUnivHash_New_KeyIsClass CMaaUnivHash64_KeyIsClass
#define CMaaUnivHash_NewClassPtr CMaaUnivHash64_ClassPtr
#define CMaaUnivHash_NewA CMaaUnivHashA64
#endif

//#define TOOLSLIB_FAST_IT_REMOVE // This is v2 of it.Remove(). It was faster than v1, but scan was slower. v3 of Remove(Record *) is better
//#define TOOLSLIB_HT_OVER_EXPERIMENTAL

// test_ht1()
#if 1 // 0.021543+0.013032+0.010176+0.007746=0.052497 
#define TOOLSLIB_HT_AllocPtr &m_pAllocator
#define TOOLSLIB_HT_AllocPtr2 , TOOLSLIB_HT_AllocPtr
#else // 0.021233+0.013346+0.010152+0.008037=0.052768 (+0.5%)
#define TOOLSLIB_HT_AllocPtr 
#define TOOLSLIB_HT_AllocPtr2
#endif

//int gCMaaToolLib_crt_Initializer() noexcept;

template < class Key, class Data = TOOLSLIB_HT_UNUSED_DATA_TYPE, bool bStandartHash = true, int AllocatorType =
#ifdef TOOLSLIB_SHARED_ALLOCATOR
    1
#else
    -1
#endif
> class CMaaUnivHash_New
{
public:
//protected:
    struct Record
    {
        Record * pNext;
        Key  K;
        Data D;
        Record(const Key &k, const Data &d) noexcept( noexcept(Key(k)) && noexcept(Data(d)) )
        :   K(k),
            D(d)
        {
        }
        Data & data() const
        {
            return (Data&)D;
        }
#ifdef TOOLSLIB_SHARED_ALLOCATOR
        //ADD_UL_ALLOCATOR(Record)
        ADD_ALLOCATOR(Record)
#ifndef TOOLSLIB_HT64
//#define DEF4_ALLOCATOR_CMaaUnivHash(A,B,F,X) template<> CMaaFixedAllocator< CMaaUnivHash< A, B, F, X >::Record >* CMaaUnivHash< A, B, F, X >::Record::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaUnivHash< A, B, F, X >::Record > CMaaUnivHash< A, B, F, X >::Record::s_AllocatorCreator(&CMaaUnivHash< A, B, F, X >::Record::s_pAllocator);
#define DEF4_ALLOCATOR_CMaaUnivHash(A,B,F,X) template<> CMaaFixedAllocator< CMaaUnivHash< A, B, F, X >::Record, 1 > & CMaaUnivHash< A, B, F, X >::Record::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaUnivHash< A, B, F, X >::Record, 1 > a;  return (CMaaFixedAllocator< CMaaUnivHash< A, B, F, X >::Record, 1 >  &)a; }
//#define DEF3_ALLOCATOR_CMaaUnivHash(A,B,F) DEF4_ALLOCATOR_CMaaUnivHash(A,B,F,1)
//#define DEF3_ALLOCATOR_CMaaUnivHash(A,B,F) DEF4_ALLOCATOR_CMaaUnivHash(A,B,F,-1)
#define DEF_ALLOCATOR_CMaaUnivHash(A,B) DEF4_ALLOCATOR_CMaaUnivHash(A,B,true,1)
#else
//#define DEF4_ALLOCATOR_CMaaUnivHash64(A,B,F,X) template<> CMaaFixedAllocator< CMaaUnivHash64< A, B, F, X >::Record >* CMaaUnivHash64< A, B, F, X >::Record::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaUnivHash64< A, B, F, X >::Record > CMaaUnivHash64< A, B, F, X >::Record::s_AllocatorCreator(&CMaaUnivHash64< A, B, F, X >::Record::s_pAllocator);
#define DEF4_ALLOCATOR_CMaaUnivHash64(A,B,F,X) template<> CMaaFixedAllocator< CMaaUnivHash64< A, B, F, X >::Record, 1 > & CMaaUnivHash64< A, B, F, X >::Record::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaUnivHash64< A, B, F, X >::Record, 1 > a;  return (CMaaFixedAllocator< CMaaUnivHash64< A, B, F, X >::Record, 1 >  &)a; }
//#define DEF3_ALLOCATOR_CMaaUnivHash64(A,B,F) DEF4_ALLOCATOR_CMaaUnivHash64(A,B,F,1)
//#define DEF3_ALLOCATOR_CMaaUnivHash64(A,B,F) DEF4_ALLOCATOR_CMaaUnivHash64(A,B,F,-1)
#define DEF_ALLOCATOR_CMaaUnivHash64(A,B) DEF4_ALLOCATOR_CMaaUnivHash64(A,B,true,1)
#endif

#else

#ifndef TOOLSLIB_HT64
#define DEF4_ALLOCATOR_CMaaUnivHash(A,B,F,X)
//#define DEF3_ALLOCATOR_CMaaUnivHash(A,B,F)
#define DEF_ALLOCATOR_CMaaUnivHash(A,B)
#else
#define DEF4_ALLOCATOR_CMaaUnivHash64(A,B,F,X)
//#define DEF3_ALLOCATOR_CMaaUnivHash64(A,B,F)
#define DEF_ALLOCATOR_CMaaUnivHash64(A,B)
#endif

#endif
    };

protected:
#ifndef TOOLSLIB_SHARED_ALLOCATOR
    CMaaFixedAllocator<Record, AllocatorType>* m_pAllocator;
#endif

    size_t m_TableSize, m_ItemsCount;   // size of item table, number of added items
    size_t m_HashSize;    // size of hash table
    size_t m_RemoveSomeItemsIndex; // opt some remove index

    Record ** m_Hash;   // hash table
    //int m_Err; // 0 - ok, -1 - init by 0 size (recreate table on Add), 1 - error

    int m_Flags;
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
    int m_AutoSqueezeLock;
#endif

public:
    enum // m_Flags
    {
        eErr1           = 0x01,
        eOk0            = 0x02,
        eNothingMask    = eErr1 | eOk0,
        eAutoSqueeze    = 0x04,
        eDefaultArgs    = 0x80000000 // Default preallocate and allocator_type
    };

protected:
    // hash function of key
#ifndef TOOLSLIB_HT64
//#define HashFunction___ CMaaHashFunction
#define HashFunction___(K) (bStandartHash ? CMaaHashFunction(K) : HashFunction(K))
#else
//#define HashFunction___ CMaaHashFunction64
#define HashFunction___(K) (bStandartHash ? CMaaHashFunction64(K) : HashFunction(K))
#endif
    virtual TOOLSLIB_HT_HASH HashFunction(const Key &K) const noexcept
    {
#ifndef TOOLSLIB_HT64
        return CMaaHashFunction(K);
#else
        return CMaaHashFunction64(K);
#endif
    }

public:

    class iterator : public CMaaDLink
    {
        friend class CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>;
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> * h;
#ifdef TOOLSLIB_FAST_IT_REMOVE
        typename CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::Record* r, ** p;
#else
        typename CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::Record* r;
#endif
        size_t index;
        bool bInc;
    public:
        iterator() noexcept
        :   h(nullptr)
        {
            //Init();
        }
        iterator(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &ht) noexcept
        :   h(nullptr)
        {
            //CMaaDLink::Init();
            *this = ht.it();
        }
        iterator(const iterator &That) noexcept
        :   CMaaDLink(), // linux g++ warn avoid
            h(nullptr)
        {
            //CMaaDLink::Init();
            *this = That;
        }
        /*
          iterator(iterator &That)
          :    h(nullptr)
          {
               Init();
               *this = That;
          }
        */
        ~iterator() noexcept
        {
            Release();
        }
        void Release() noexcept
        {
            if  (h)
            {
                h->m_Iterators.Release(this);
                h->UnLockAutoSqueeze();
                h = nullptr;
            }
        }
        iterator & operator=(const iterator &that) noexcept
        {
            iterator &That = (iterator &)that;
            if  (this != &That)
            {
                Release();
                if  ((h = That.h))
                {
                    h->LockAutoSqueeze();
                    r = That.r;
#ifdef TOOLSLIB_FAST_IT_REMOVE
                    p = That.p;
#endif
                    index = That.index;
                    bInc = That.bInc;
                    h->m_Iterators.AddAtFront(this);
                }
            }
            return *this;
        }
        iterator & operator=(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &ht) noexcept
        {
            Release();
            *this = ht.it();
            return *this;
        }
        bool IsValid() const noexcept
        {
            return h && !bInc && r; // && p;
        }
        operator bool () const noexcept
        {
            return IsValid();
        }
        // fast, noexcept functions w/o of validity check
        const Key& ckey() const noexcept
        {
            return r->K;
        }
        Key key() noexcept(noexcept(Key(r->K))) //const
        {
            return r->K;
        }
        Data & rdata() noexcept
        {
            return r->D;
        }
        Data data() noexcept(noexcept(Data(r->D))) //const
        {
            return r->D;
        }

        const Key & ckeyEx() const
        {
            if  (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Key keyEx() //const
        {
            if  (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Data & rdataEx()
        {
            if  (!IsValid())
            {
                throw 1;
            }
            return r->D;
        }
        Data dataEx() //const
        {
            if  (!IsValid())
            {
                throw 1;
            }
            return r->D;
        }

        /*
        operator Key ()
        {
            if   (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Data & operator()()
        {
            if   (!IsValid())
            {
                throw 1;
            }
            return r->D;
        }
        */
        iterator operator++(int) noexcept
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        iterator operator++() noexcept
        {
            if  (h) // remark
            {
                if  (!bInc)
                {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                    Inc(true);
#else
                    Inc();
#endif
                }
                if  (bInc)
                {
                    bInc = false;
                }
                else
                {
                    Release();
                }
            }
            return *this;
        }
#ifdef TOOLSLIB_FAST_IT_REMOVE
        // bMoveP_OnCurrIndex = true for ++it
        // bMoveP_OnCurrIndex = false in it.Remove(/*&d*/)
        bool Inc(bool bMoveP_OnCurrIndex = true) noexcept
#else
        bool Inc() noexcept
#endif
        {
            if  (h && r)
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                if (bMoveP_OnCurrIndex)
                {
                    p = &r->pNext;
                }
#endif
                r = r->pNext;
                if  (!r)
                {
                    while(++index < h->m_HashSize)
                    {
                        if  ((r = h->m_Hash[index]))
                        {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                            p = h->m_Hash + index;
#endif
                            break;
                        }
                    }
                }
                if  (r)
                {
                    bInc = true;
                    return true;
                }
            }
            Release();
            return false;
        }
//#ifdef TOOLSLIB_FAST_IT_REMOVE
        int Remove(Data * d) noexcept(noexcept(*d = *d)) //noexcept( noexcept(h->Remove(r, p, d)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
//#else    
//        int Remove(Data* d) noexcept(noexcept(r->K == r->K) && noexcept(*d = *d)) //noexcept( noexcept(h->Remove(r->K, d)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
//#endif
        {
            if  (IsValid())
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                return h->Remove(r, p, d);
#else
                //return h->Remove(r->K, d);
                return h->Remove(r, d);
#endif
            }
            return 2;
        }
//#ifdef TOOLSLIB_FAST_IT_REMOVE
        int Remove() noexcept //noexcept( noexcept(h->Remove(r, p)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
//#else    
//        int Remove() noexcept(noexcept(r->K == r->K)) //noexcept( noexcept(h->Remove(r->K)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
//#endif
        {
            if  (IsValid())
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                return h->Remove(r, p);
#else    
                //return h->Remove(r->K);
                return h->Remove(r);
#endif
            }
            return 2;
        }
    };

    class AutoSqueezeLocker
    {
        //friend class CMaaUnivHash_New<Key, Data>;
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &h;
    public:
        AutoSqueezeLocker(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &ht) noexcept
        :   h(ht)
        {
            Lock();
        }
        AutoSqueezeLocker (const AutoSqueezeLocker& That) = delete;
        AutoSqueezeLocker& operator=(const AutoSqueezeLocker& That) = delete;
        ~AutoSqueezeLocker() noexcept
        {
            UnLock();
        }
        void Lock() noexcept
        {
            h.LockAutoSqueeze();
        }
        void UnLock() noexcept
        {
            h.UnLockAutoSqueeze();
        }
    };

protected:
    CMaaDList<iterator> m_Iterators;
public:
    iterator it() noexcept
    {
        for (size_t i = 0; i < m_HashSize; i++)
        {
            if  (m_Hash[i])
            {
                iterator it;
                LockAutoSqueeze();
                it.h = this;
                it.index = i;
                it.r = m_Hash[i];
#ifdef TOOLSLIB_FAST_IT_REMOVE
                it.p = m_Hash + i;
#endif
                it.bInc = false;
                m_Iterators.AddAtFront(&it);
                return it;
            }
        }
        return iterator();
    }
    void InvalidateIterators() noexcept
    {
        iterator * p;
        while((p = m_Iterators.GetFromFront()))
        {
            if (p->h)
            {
                UnLockAutoSqueeze();
                p->h = nullptr;
            }
        }
    }
#ifdef TOOLSLIB_FAST_IT_REMOVE
    void MoveIterators(const Record* r, Record** prev) noexcept // from Remove
#else
    void MoveIterators(const Record * r) noexcept // from Remove
#endif
    {
        iterator * n;
        for (iterator * p = m_Iterators.LookAtFront(); p; p = n)
        {
            n = m_Iterators.Next(p);
            if (p->h)
            {
                if (p->r == r)
                {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                    p->Inc(false);
#else
                    p->Inc();
#endif
                }
#ifdef TOOLSLIB_FAST_IT_REMOVE
                else if (r->pNext == p->r)
                {
                    p->p = prev;
                }
#endif
            }
        }
    }
    void ChangeIterators(const Record * pOld, Record * pNew) noexcept
    {
        iterator * n;
        for (iterator * p = m_Iterators.LookAtFront(); p; p = n)
        {
            n = m_Iterators.Next(p);
            if  (p->h && p->r == pOld)
            {
                p->r = pNew;
            }
        }
    }

    virtual void Swap(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &That) noexcept
    {
        CMaaSwap(m_TableSize,  That.m_TableSize);
        CMaaSwap(m_HashSize,   That.m_HashSize);
        CMaaSwap(m_ItemsCount, That.m_ItemsCount);
        CMaaSwap(m_RemoveSomeItemsIndex, That.m_RemoveSomeItemsIndex);
        CMaaSwap(m_Hash,       That.m_Hash);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        CMaaSwap(m_pAllocator, That.m_pAllocator);
#endif
        //CMaaSwap(m_Err,        That.m_Err);
        CMaaSwap(m_Flags,      That.m_Flags);
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //!!!//CMaaSwap(m_AutoSqueezeLock, That.m_AutoSqueezeLock);
#endif

        CMaaDList<iterator> l1, l2;
        l1.MoveThatToTheFront(m_Iterators);
        l2.MoveThatToTheFront(That.m_Iterators);

        iterator * p;
        while((p = l1.GetFromFront()))
        {
            if  (p->h)
            {
                That.LockAutoSqueeze();
                p->h->UnLockAutoSqueeze();
                p->h = &That;
                That.m_Iterators.AddAtBack(p);
            }
        }
        while((p = l2.GetFromFront()))
        {
            if  (p->h)
            {
                LockAutoSqueeze();
                p->h->UnLockAutoSqueeze();
                p->h = this;
                m_Iterators.AddAtBack(p);
            }
        }
    }

    CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& operator=(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>&& That) noexcept
    {
        Swap(That);
        That.Cleanup();
        return *this;
    }

#if 0
    virtual CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> * MakeNewTable(size_t Size, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> * ptr, int Flags) noexcept(noexcept_new) //, int preallocate = 0, int allocator_type = -1)
    {
        return TL_NEW CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>(Size, ptr, 0, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1);
    }
#endif

protected:
    virtual int ResizeMoveSwap(bool bMove, size_t Size) noexcept(noexcept_new) //, int preallocate = 0, int allocator_type = -1)
    {
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> tmp(Size, this, 0, m_Flags TOOLSLIB_CHECK_HT_USAGE_ARG1);
        if (bMove)
        {
            return MoveSwap(tmp);
        }
        Swap(tmp);
        return 0;
    }

private:
    void Init(size_t Size, int Flags) noexcept(noexcept_new)
    {
        //gCMaaToolLib_crt_Initializer();
        if  ( Size < 2 )
        {
            Size = 0;
        }
        Flags &= ~ eNothingMask;
        const size_t MaxSz = ((size_t)-1) / sizeof(Record *) / 2;
        m_HashSize     = (Size + Size / 10 <= MaxSz ? Size + Size / 10 : MaxSz);
        m_TableSize    = Size;
        m_ItemsCount   = 0;
        m_Hash         = Size && Size < SIZE_MAX / sizeof(void *) / 11 * 10 ? TL_NEW Record * [ m_HashSize ] : nullptr;
        if  (m_Hash)
        {
            Memzero(m_Hash, m_HashSize * sizeof(*m_Hash));
            m_Flags = Flags;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
            gHashTablesAllocSize += (_qword)(m_HashSize * sizeof(*m_Hash));
#endif
        }
        else
        {
            m_Flags = Flags | (Size ? eErr1 : eOk0);
        }
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        m_AutoSqueezeLock = 0;
#endif
    }

public:
    CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> & operator = (const CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &That) noexcept(noexcept(Add(*(const Key *)nullptr, *(const Data*)nullptr)))
    {
        if  (this != &That)
        {
            iterator it((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &)That);
            Cleanup();
            for (; it; ++it)
            {
                Add(it.key(), it.data());
            }
        }
        return *this;
    }
    CMaaUnivHash_New() noexcept(noexcept_new)
    {
        GenericCtor(15, nullptr, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2);
    }
    CMaaUnivHash_New(size_t Size) noexcept(noexcept_new)
    {
        GenericCtor(Size, nullptr, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2);
    }
    CMaaUnivHash_New(size_t Size, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>* ptr, int iPreallocate, int Flags
#ifndef TOOLSLIB_CHECK_HT_USAGE
            = TOOLSLIB_HT_DEF_FLAG
#else
            TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#endif
        ) noexcept(noexcept_new)
    {
        GenericCtor(Size, ptr, iPreallocate, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1);
    }
    bool PreAllocate(size_t CountTotal, size_t FreeCountRequired = 0, size_t CountAdd = 0) // noexcept(noexcept_new)
    {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        bool b = false;
        if (m_pAllocator)
        {
#ifdef _RR
            if constexpr (noexcept_new)
            {
                if (!(b = m_pAllocator->PreAllocate(CountTotal, FreeCountRequired, CountAdd)))
                {
                    //m_Flags |= eErr1;
                }
            }
            else
#endif
            {
                if (!(b = m_pAllocator->PreAllocate(CountTotal, FreeCountRequired, CountAdd)))
                {
                    //m_Flags |= eErr1;
                }
            }
        }
        else
        {
            //m_Flags |= eErr1;
        }
        return b;
#else
        return true;// false;
#endif
    }
protected:
    void GenericCtor(size_t Size, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>* ptr, int iPreallocate, int Flags
#ifndef TOOLSLIB_CHECK_HT_USAGE
            = TOOLSLIB_HT_DEF_FLAG
#else
            TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#endif
        ) noexcept(noexcept_new)
    {
        if (Flags & eDefaultArgs)
        {
            iPreallocate = 0;
            //allocator_type = -1;
            Flags &= ~ eDefaultArgs;
        }

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        m_pAllocator = nullptr;
#endif
        Init(Size, Flags);
        m_RemoveSomeItemsIndex = 0;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if  (ptr)
        {
            m_pAllocator = ptr->m_pAllocator;
        }
        if  (!m_pAllocator)
        {
            m_pAllocator = TL_NEW CMaaFixedAllocator<Record, AllocatorType>(eNotInit);
        }
        if  (m_pAllocator)
        {
            m_pAllocator->AddRefEx();
            if  ((!ptr && iPreallocate >= 0) || iPreallocate > 0)
            {
#ifdef _RR
                if constexpr (noexcept_new)
                {
                    if (!m_pAllocator->PreAllocate(Size))
                    {
                        m_Flags |= eErr1;
                    }
                }
                else
#endif
                {
                    try
                    {
                        if (!m_pAllocator->PreAllocate(Size))
                        {
                            m_Flags |= eErr1;
                        }
                    }
                    catch (...)
                    {
                        m_pAllocator->DelRefEx();
                        m_pAllocator = nullptr; // 24.09.2024
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                        if (m_Hash)
                        {
                            gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
                        }
#endif
                        delete[] m_Hash;  m_Hash = nullptr;
                        m_Flags |= eErr1;
#ifndef _RR
                        if constexpr (!noexcept_new)
                        {
                            throw;
                        }
#else
                        throw;
#endif
                    }
                }
            }
        }
        else
        {
            m_Flags |= eErr1;
        }
#else
        //if (!CMaaFixedAllocator<CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::Record, AllocatorType>::CreateAddRefUl(&Record::s_pAllocator))
        if (!CMaaFixedAllocator<CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::Record, AllocatorType>::CreateAddRef(&Record::s_pAllocator))
        {
            m_Flags |= eErr1;
        }
#endif
    }
public:
    void SetAutoSqueeze(bool b) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        m_Flags = b ? (m_Flags | eAutoSqueeze) : (m_Flags & ~ eAutoSqueeze);
#endif
    }
    void LockAutoSqueeze() noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        ++m_AutoSqueezeLock;
#endif
    }
    void UnLockAutoSqueeze() noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        --m_AutoSqueezeLock;
#endif
    }
    bool IsNeedToSqueeze() const noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        if ((m_Flags & eAutoSqueeze) && m_AutoSqueezeLock <= 0 && m_ItemsCount <= m_TableSize / 4 && m_TableSize > 16)
        {
            return true;
        }
#else
        if (!m_Iterators.LookAtFront() && m_ItemsCount <= m_TableSize / 4 && m_TableSize > 16)
        {
            return true;
        }
#endif
        return false;
    }

    CMaaUnivHash_New(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>&& That) noexcept(noexcept_new)
    {
        size_t Size = 0;
        int iPreallocate = 0;
        //int allocator_type = -1;
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>* ptr = /*nullptr; // =*/ (CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> *) &That;

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        m_pAllocator = nullptr;
#endif
        Init(Size, 0);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if  (ptr)
        {
            m_pAllocator = ptr->m_pAllocator;
        }
        if  (!m_pAllocator)
        {
            m_pAllocator = TL_NEW CMaaFixedAllocator<Record, AllocatorType>(eNotInit);
        }
        if  (m_pAllocator)
        {
            m_pAllocator->AddRefEx();
            if  ((!ptr && iPreallocate >= 0) || iPreallocate > 0)
            {
#ifdef _RR
                if constexpr (noexcept_new)
                {
                    if (!m_pAllocator->PreAllocate(Size))
                    {
                        m_Flags |= eErr1;
                    }
                }
                else
#endif
                {
                    try
                    {
                        if (!m_pAllocator->PreAllocate(Size))
                        {
                            m_Flags |= eErr1;
                        }
                    }
                    catch (...)
                    {
                        m_pAllocator->DelRefEx();
                        m_pAllocator = nullptr; // 24.09.2024
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                        if (m_Hash)
                        {
                            gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
                        }
#endif
                        delete[] m_Hash;  m_Hash = nullptr;
                        m_Flags |= eErr1;
#ifndef _RR
                        if constexpr (!noexcept_new)
                        {
                            throw;
                        }
#else
                        throw;
#endif
                    }
                }
            }
        }
        else
        {
            m_Flags |= eErr1;
        }
#endif
        Swap(That);
    }

    CMaaUnivHash_New(const CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &That) noexcept(noexcept_new)
    {
        size_t Size = 16;
        int iPreallocate = 0;
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> * ptr = /*nullptr; // =*/ (CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> *)&That;

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        m_pAllocator = nullptr;
#endif
        Init(Size, That.m_Flags);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if  (ptr)
        {
            m_pAllocator = ptr->m_pAllocator;
        }
        if  (!m_pAllocator)
        {
            m_pAllocator = TL_NEW CMaaFixedAllocator<Record, AllocatorType>(eNotInit);
        }
        if  (m_pAllocator)
        {
            m_pAllocator->AddRefEx();
            if  ((!ptr && iPreallocate >= 0) || iPreallocate > 0)
            {
#ifdef _RR
                if constexpr (noexcept_new)
                {
                    if (!m_pAllocator->PreAllocate(Size))
                    {
                        m_Flags |= eErr1;
                    }
                }
                else
#endif
                {
                    try
                    {
                        if (!m_pAllocator->PreAllocate(Size))
                        {
                            m_Flags |= eErr1;
                        }
                    }
                    catch (...)
                    {
                        m_pAllocator->DelRefEx();
                        m_pAllocator = nullptr; // 24.09.2024
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                        if (m_Hash)
                        {
                            gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
                        }
#endif
                        delete[] m_Hash;  m_Hash = nullptr;
                        m_Flags |= eErr1;
#ifndef _RR
                        if constexpr (!noexcept_new)
                        {
                            throw;
                        }
#else
                        throw;
#endif
                    }
                }
            }
            *this = That;
        }
        else
        {
            m_Flags |= eErr1;
        }
#endif
    }

    void unite(const CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> & OtherTable)
    {
        iterator it((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType> &)OtherTable);
        for (; it; ++it)
        {
            Add(it.key(), it.data());
        }
    }
    const Data value(const Key &key, const Data &defaultValue) const noexcept(noexcept(key == key) && noexcept((Data&)defaultValue = defaultValue))
    {
        //Data d = defaultValue;
        //((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>&)*this).Find(key, &d);
        //return d;
        Data* p = (*this)[key];
        if (p)
        {
            return *p;
        }
        return defaultValue;
    }
    bool isEmpty() const noexcept
    {
        return GetItemCount() == 0;
    }

    bool IsOK() const noexcept
    {
        return !(m_Flags & eErr1);
    }

private:
    void destr () noexcept
    {
        InvalidateIterators();

        if  (m_ItemsCount > 0 && m_Hash)
        {
            //printf("deleting %d objects...", (int)m_ItemsCount);
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                Record * n;
                for (Record* r = m_Hash[i]; r; r = n)
                {
                    n = r->pNext;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                    gHashTablesAllocSize -= (_qword)(sizeof(*r));
#endif
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                    r->~Record();
                    m_pAllocator->DeleteEx(r TOOLSLIB_HT_AllocPtr2);
#else
                    delete r;
#endif
                }
            }
        }
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        if (m_Hash)
        {
            gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
        }
#endif
        delete [] m_Hash;  //m_Hash  = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if  (m_pAllocator)
        {
            m_pAllocator->DelRefEx(TOOLSLIB_HT_AllocPtr);
            //m_pAllocator = nullptr; // --24.09.2024
        }
#else
        // 2023+:
        Record::s_pAllocator->DelRefEx(&Record::s_pAllocator);
#endif
        m_Flags |= eErr1;
    }

public:
    virtual ~CMaaUnivHash_New () noexcept
    {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        /*if (m_pAllocator->GetAllocatorType() == 0x200)
        {
            CMaaWin32Locker_a2Locker_gLock_Atomic2;
            a2Locker_Lock;
            destr();
            a2Locker_UnLock;
            return;
        }*/
#endif
        destr();
    }

    int RemoveSomeItems(int N) noexcept
    {
        if (N <= 0 || (m_Flags & eNothingMask) || !m_ItemsCount)
        {
            return 0;
        }
        InvalidateIterators();
        N = (size_t)N <= m_ItemsCount ? N : (int)m_ItemsCount;
        const int Ret = N;
        m_ItemsCount -= N;
        while (1)
        {
            m_RemoveSomeItemsIndex = m_RemoveSomeItemsIndex < m_HashSize ? m_RemoveSomeItemsIndex : 0;
            if (m_Hash[m_RemoveSomeItemsIndex])
            {
                break;
            }
            m_RemoveSomeItemsIndex++;
        }
        while (N > 0)
        {
            Record* n;
            for (Record* r = m_Hash[m_RemoveSomeItemsIndex]; r; r = n)
            {
                n = r->pNext;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                gHashTablesAllocSize -= (_qword)(sizeof(*r));
#endif
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                r->~Record();
                m_pAllocator->DeleteEx(r TOOLSLIB_HT_AllocPtr2);
#else
                delete r;
#endif
                N--;
            }
            m_Hash[m_RemoveSomeItemsIndex] = nullptr;
            while (1)
            {
                m_RemoveSomeItemsIndex++;
                m_RemoveSomeItemsIndex = m_RemoveSomeItemsIndex < m_HashSize ? m_RemoveSomeItemsIndex : 0;
                if (m_Hash[m_RemoveSomeItemsIndex] || N <= 0)
                {
                    break;
                }
            }
        }
        m_ItemsCount += N;
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //if () Squeeze();
#endif
        return Ret - N;
    }

    void Cleanup() noexcept
    {
        InvalidateIterators();
        if (m_ItemsCount > 0)
        {
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                Record* n;
                for (Record* r = m_Hash[i]; r; r = n)
                {
                    n = r->pNext;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                    gHashTablesAllocSize -= (_qword)(sizeof(*r));
#endif
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                    r->~Record();
                    m_pAllocator->DeleteEx(r TOOLSLIB_HT_AllocPtr2);
#else
                    delete r;
#endif
                }
                //m_Hash[i] = nullptr;
            }
            Memzero(m_Hash, m_HashSize * sizeof(*m_Hash));
            m_ItemsCount = 0;
        }
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //Squeeze();
#endif
    }

    void Cleanup(int NewTableSize) noexcept(noexcept_new)
    {
        if (NewTableSize <= 0)
        {
            Cleanup();
        }
        else
        {
            InvalidateIterators();
            ResizeMoveSwap(false, NewTableSize);
        }
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //Squeeze();
#endif
    }

    size_t GetMaxIndex () const noexcept
    {
        return m_HashSize;
    }

    // returns 0 for success
    /*
    int FindByIndex(size_t Index, Key* K = nullptr, Data* D = nullptr) const noexcept(noexcept(*K = *K) && noexcept(*D = *D)) // obsolete
    {
        InvalidateIterators();
        if   (//Index >= 0 &&
              Index < m_HashSize)
        {
            if   (m_Hash[Index])
            {
                if   (K)
                {
                    *K = m_Hash[Index]->K;
                }
                if   ( D )
                {
                    *D = m_Hash[Index]->D;
                }
                return 0;
            }
        }
        return 1;
    }
    */

#if 0
#ifndef TOOLSLIB_SHARED_ALLOCATOR
    struct RR // return record
    {
        Record*& m_pr;
        CMaaFixedAllocator<Record, AllocatorType>* m_pAllocator;

        RR(Record*& pr, CMaaFixedAllocator<Record, AllocatorType>* pAllocator) noexcept
        :   m_pr(pr),
            m_pAllocator(pAllocator)
        {
        }
        ~RR() noexcept
        {
            if (m_pr)
            {
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                gHashTablesAllocSize -= (_qword)(sizeof(*m_pr));
#endif
                //CMaaFixedAllocator<Record, AllocatorType>::DeleteEx(m_pr TOOLSLIB_HT_AllocPtr2);
                m_pAllocator->DeleteEx(m_pr TOOLSLIB_HT_AllocPtr2);
            }
        }
    };
#else
    struct RR // return record
    {
        Record*& m_pr;

        RR(Record*& pr) noexcept
        :   m_pr(pr)
        {
        }
        ~RR() noexcept
        {
            if (m_pr)
            {
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                gHashTablesAllocSize -= (_qword)(sizeof(*m_pr));
#endif
                delete m_pr;
            }
        }
    };
#endif
#endif

    virtual void test__(void * /*a*/ = nullptr, void * /*b*/ = nullptr, void * /*c*/ = nullptr, int /*x1*/ = 0, int /*x2*/ = 0)
    {
    }

protected:
    void FastAddRec(Record* rec) noexcept
    {
        Record** Hash = m_Hash + HashFunction___(rec->K) % m_HashSize;
        rec->pNext = *Hash;
        *Hash = rec;
        m_ItemsCount++;
    }

public:
#if 0
    int NewMoveSwap(size_t sz) noexcept(noexcept_new)
    {
        if (sz < m_ItemsCount)
        {
            return 3; // param err
        }
        // expand table or first create on m_Err == -1
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>* NewTable = MakeNewTable(sz, this, m_Flags);
        if (!NewTable || !NewTable->IsOK())
        {
            return 2; // No free elements
        }
        else
        {
            if (m_Hash)
            {
                for (size_t i = 0; i < m_HashSize; ++i)
                {
                    Record * n;
                    for (Record *r = m_Hash[i]; r; r = n)
                    {
                        n = r->pNext;
                        m_ItemsCount--;
                        NewTable->FastAddRec(r);
                    }
                    m_Hash[i] = nullptr;
                }
            }
            Swap(*NewTable);
        }
        delete NewTable;
        return 0; // ok
    }
#endif
protected:
    int ResizeMoveSwap_(size_t sz) noexcept(noexcept_new)
    {
        if (sz < m_ItemsCount)
        {
            return 3; // param err
        }
        return ResizeMoveSwap(true, sz);
    }

    int MoveSwap(CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& NewTable) noexcept(noexcept_new)
    {
        if (!NewTable.IsOK())
        {
            return 2; // No free elements
        }
        if (m_Hash)
        {
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                Record* n;
                for (Record* r = m_Hash[i]; r; r = n)
                {
                    n = r->pNext;
                    //m_ItemsCount--;
                    NewTable.FastAddRec(r);
                }
                //m_Hash[i] = nullptr;
            }
            m_ItemsCount = 0;
        }
        Swap(NewTable);
        return 0; // ok
    }

    inline void Delete0(Record* rec) noexcept
    {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        rec->~Record();
        m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
#else
        delete rec;
#endif
    }
    inline void Delete(Record* rec) noexcept
    {
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize -= (_qword)(sizeof(*rec));
#endif
        Delete0(rec);
    }

public:

    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key &K, const Data &D/*, int fOverwrite = 0*/) noexcept(noexcept_new && noexcept(Record(K, D)) && noexcept(K == K) && noexcept(Data(D)))
    {
        InvalidateIterators();

        /*
        Record * rec = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        RR RR_rec(rec, m_pAllocator);
#else
        RR RR_rec(rec);
#endif
        */

        if  ((m_Flags & eErr1))
        {
            return 2;
        }

        Record** Prev = nullptr; // "= nullptr" can be omiited, but g++ warn

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
            Record * This = *Prev;
            for (; This; This = This->pNext)
            {
#ifndef TOOLSLIB_MAX_SPEED
                if  (!ItemsCount--) // integrity check failure
                {
                    CMaaUnivHash_THROW;
                }
#endif
                if  (This->K == K)
                {
                    return 1; // Exists
                }

#ifndef TOOLSLIB_MAX_SPEED
                if  (This == This->pNext)
                {
                    //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                    CMaaUnivHash_THROW;
                }
#endif
                Prev = &This->pNext;
            }
        }
        if  (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            // expand table or first create on m_Err == -1
            const int Ret = ResizeMoveSwap_(m_TableSize >= 3 ? 2 * m_TableSize + 1 : 3);
            if (Ret)
            {
                return Ret;
            }
            //return Add(K, D, fOverwrite);
            // key K is not exist
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
        }
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        Record* rec = m_pAllocator->NewEx(TOOLSLIB_HT_AllocPtr);
        if  (!rec)
        {
            return 2;
        }
        if constexpr (noexcept(Record(K, D)))
        {
            ::new_(rec, Record(K, D));
            //new(rec)Record(K, D);
        }
        else
        {
            try
            {
                ::new_(rec, Record(K, D));
            }
            catch (...)
            {
                m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
                throw;
            }
        }
#else
        Record* rec = TL_NEW Record(K, D);
        if (!rec)
        {
            return 2;
        }
#endif
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
        rec->pNext = *Prev;
        *Prev = rec;
        //rec = nullptr;
        m_ItemsCount++;
        return 0;
    }

    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    int AddOver(const Key& K, const Data& D)  noexcept(noexcept_new && noexcept(Record(K, D)) && noexcept(K == K) && noexcept(Data(D)) && noexcept((Data&)D = D)) //noexcept(noexcept(Add(K, D, 1)))
    {
        InvalidateIterators();

        if ((m_Flags & eErr1))
        {
            return 2;
        }

        Record** Prev = nullptr; // "= nullptr" can be omiited, but g++ warn

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
            Record* This = *Prev;
            for (; This; This = This->pNext)
            {
#ifndef TOOLSLIB_MAX_SPEED
                if (!ItemsCount--) // integrity check failure
                {
                    CMaaUnivHash_THROW;
                }
#endif
                if (This->K == K)
                {
                    This->D = D;
                    return 0; // ok
                }

#ifndef TOOLSLIB_MAX_SPEED
                if (This == This->pNext)
                {
                    //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                    CMaaUnivHash_THROW;
                }
#endif
                Prev = &This->pNext;
            }
        }
        if (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            // expand table or first create on m_Err == -1
            const int Ret = ResizeMoveSwap_(m_TableSize >= 3 ? 2 * m_TableSize + 1 : 3);
            if (Ret)
            {
                return Ret;
            }
            //return Add(K, D, fOverwrite);
            // key K is not exist
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
        }
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        Record* rec = m_pAllocator->NewEx(TOOLSLIB_HT_AllocPtr);
        if (!rec)
        {
            return 2;
        }
        if constexpr (noexcept(Record(K, D)))
        {
            ::new_(rec, Record(K, D));
            //new(rec)Record(K, D);
        }
        else
        {
            try
            {
                ::new_(rec, Record(K, D));
            }
            catch (...)
            {
                m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
                throw;
            }
        }
#else
        Record* rec = TL_NEW Record(K, D);
        if (!rec)
        {
            return 2;
        }
#endif
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
        rec->pNext = *Prev;
        *Prev = rec;
        //rec = nullptr;
        m_ItemsCount++;
        return 0;
    }

#if 0
    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key& K, const Data& D, int fOverwrite = 0) noexcept(noexcept_new && noexcept(Record(K, D)) && noexcept(K == K) && noexcept(Data(D)))
    {
        InvalidateIterators();

        /*
        Record * rec = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        RR RR_rec(rec, m_pAllocator);
#else
        RR RR_rec(rec);
#endif
        */

        if ((m_Flags & eErr1))
        {
            return 2;
        }

        Record** Prev = nullptr; // "= nullptr" can be omiited, but g++ warn

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
            Record* This = *Prev;
            for (; This; This = This->pNext)
            {
#ifndef TOOLSLIB_MAX_SPEED
                if (!ItemsCount--) // integrity check failure
                {
                    CMaaUnivHash_THROW;
                }
#endif
                if (This->K == K)
                {
                    if (!fOverwrite)
                    {
                        return 1; // Exists
                    }
#ifndef TOOLSLIB_HT_OVER_EXPERIMENTAL
                    This->D = D;
                    return 0; // ok
#else
                    switch (fOverwrite)
                    {
                    case 2:
                    {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                        Record* rec = m_pAllocator->NewEx(TOOLSLIB_HT_AllocPtr);
                        if (!rec)
                        {
                            return 2;
                        }
                        if constexpr (noexcept(Record(K, D)))
                        {
                            ::new_(rec, Record(K, D));
                        }
                        else
                        {
                            try
                            {
                                ::new_(rec, Record(K, D));
                            }
                            catch (...)
                            {
                                m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
                                throw;
                            }
                        }
#else
                        Record* rec = TL_NEW Record(K, D);
                        if (!rec)
                        {
                            return 2;
                        }
#endif
                        rec->pNext = This->pNext;
                        *Prev = rec;
                        Delete0(This);
                        //ChangeIterators(This, rec);
                    }
                    break;
                    case 200:
                    {
                        This->D.~Data();
                        if constexpr (noexcept(Data(D)))
                        {
                            ::new_(&This->D, Data(D));
                        }
                        else
                        {
                            try
                            {
                                ::new_(&This->D, Data(D)); // unsafe
                            }
                            catch (...)
                            {
                                throw;
                            }
                        }
                    }
                    break;
                    default:
                        This->D = D;
                    }
                    return 0; // ok
#endif
                }

#ifndef TOOLSLIB_MAX_SPEED
                if (This == This->pNext)
                {
                    //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                    CMaaUnivHash_THROW;
                }
#endif
                Prev = &This->pNext;
            }
        }
        if (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            // expand table or first create on m_Err == -1
            const int Ret = ResizeMoveSwap_(m_TableSize >= 3 ? 2 * m_TableSize + 1 : 3);
            if (Ret)
            {
                return Ret;
            }
            //return Add(K, D, fOverwrite);
            // key K is not exist
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
        }
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        Record* rec = m_pAllocator->NewEx(TOOLSLIB_HT_AllocPtr);
        if (!rec)
        {
            return 2;
        }
        if constexpr (noexcept(Record(K, D)))
        {
            ::new_(rec, Record(K, D));
            //new(rec)Record(K, D);
        }
        else
        {
            try
            {
                ::new_(rec, Record(K, D));
            }
            catch (...)
            {
                m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
                throw;
            }
        }
#else
        Record* rec = TL_NEW Record(K, D);
        if (!rec)
        {
            return 2;
        }
#endif
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
        rec->pNext = *Prev;
        *Prev = rec;
        //rec = nullptr;
        m_ItemsCount++;
        return 0;
    }
#endif

    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    const Record * AddEx(const Key& K, const Data& D, int fOverwrite = 0) noexcept(noexcept_new && noexcept(Record(K, D)) && noexcept(K == K) && noexcept(Data(D)))
    {
        InvalidateIterators();

        /*
        Record * rec = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        RR RR_rec(rec, m_pAllocator);
#else
        RR RR_rec(rec);
#endif
        */

        if ((m_Flags & eErr1))
        {
            return nullptr;// 2;
        }

        Record** Prev = nullptr; // "= nullptr" can be omiited, but g++ warn

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
            Record* This = *Prev;
            for (; This; This = This->pNext)
            {
#ifndef TOOLSLIB_MAX_SPEED
                if (!ItemsCount--) // integrity check failure
                {
                    CMaaUnivHash_THROW;
                }
#endif
                if (This->K == K)
                {
                    if (!fOverwrite)
                    {
                        return nullptr;// 1; // Exists
                    }
#ifndef TOOLSLIB_HT_OVER_EXPERIMENTAL
                    This->D = D;
                    return This;// 0; // ok
#else
                    switch (fOverwrite)
                    {
                    case 2:
                    {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                        Record* rec = m_pAllocator->NewEx(TOOLSLIB_HT_AllocPtr);
                        if (!rec)
                        {
                            return 2;
                        }
                        if constexpr (noexcept(Record(K, D)))
                        {
                            ::new_(rec, Record(K, D));
                        }
                        else
                        {
                            try
                            {
                                ::new_(rec, Record(K, D));
                            }
                            catch (...)
                            {
                                m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
                                throw;
                            }
                        }
#else
                        Record* rec = TL_NEW Record(K, D);
                        if (!rec)
                        {
                            return 2;
                        }
#endif
                        rec->pNext = This->pNext;
                        *Prev = rec;
                        Delete0(This);
                        //ChangeIterators(This, rec);
                    }
                    break;
                    case 200:
                    {
                        This->D.~Data();
                        if constexpr (noexcept(Data(D)))
                        {
                            ::new_(&This->D, Data(D));
                        }
                        else
                        {
                            try
                            {
                                ::new_(&This->D, Data(D)); // unsafe
                            }
                            catch (...)
                            {
                                throw;
                            }
                        }
                    }
                    break;
                    default:
                        This->D = D;
                    }
                    return 0; // ok
#endif
                }

#ifndef TOOLSLIB_MAX_SPEED
                if (This == This->pNext)
                {
                    //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                    CMaaUnivHash_THROW;
                }
#endif
                Prev = &This->pNext;
            }
        }
        if (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            // expand table or first create on m_Err == -1
            const int Ret = ResizeMoveSwap_(m_TableSize >= 3 ? 2 * m_TableSize + 1 : 3);
            if (Ret)
            {
                return nullptr;// Ret;
            }
            //return Add(K, D, fOverwrite);
            // key K is not exist
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
        }
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        Record* rec = m_pAllocator->NewEx(TOOLSLIB_HT_AllocPtr);
        if (!rec)
        {
            return nullptr;// 2;
        }
        if constexpr (noexcept(Record(K, D)))
        {
            ::new_(rec, Record(K, D));
            //new(rec)Record(K, D);
        }
        else
        {
            try
            {
                ::new_(rec, Record(K, D));
            }
            catch (...)
            {
                m_pAllocator->DeleteEx(rec TOOLSLIB_HT_AllocPtr2);
                throw;
            }
        }
#else
        Record* rec = TL_NEW Record(K, D);
        if (!rec)
        {
            return 2;
        }
#endif
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
        rec->pNext = *Prev;
        *Prev = rec;
        //rec = nullptr;
        m_ItemsCount++;
        return rec;// 0;
    }

protected:
    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int AddRec(Record* rec, int fOverwrite) noexcept(noexcept_new && noexcept(*(const Key *)nullptr == *(const Key*)nullptr))
    {
        if (!rec)
        {
            return 3;
        }

        InvalidateIterators();

/*
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        RR RR_rec(rec, m_pAllocator);
#else
        RR RR_rec(rec);
#endif
*/
        const Key &K = rec->K;

        if ((m_Flags & eErr1))
        {
            Delete(rec);
            return 2;
        }

        Record** Prev = nullptr; // "= nullptr" can be omiited, but g++ warn

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
            Record *This = *Prev;
            if constexpr (noexcept(This->K == K))
            {
                for (; This; This = This->pNext)
                {
#ifndef TOOLSLIB_MAX_SPEED
                    if (!ItemsCount--) // integrity check failure
                    {
                        CMaaUnivHash_THROW;
                    }
#endif
                    if (This->K == K)
                    {
                        if (!fOverwrite)
                        {
                            Delete(rec);
                            return 1; // Exists
                        }
                        rec->pNext = This->pNext;
                        *Prev = rec;
                        Delete(This);
                        //ChangeIterators(This, rec);
                        return 0;
                    }
#ifndef TOOLSLIB_MAX_SPEED
                    if (This == This->pNext)
                    {
                        //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                        CMaaUnivHash_THROW;
                    }
#endif
                    Prev = &This->pNext;
                }
            }
            else
            {
                try
                {
                    for (; This; This = This->pNext)
                    {
#ifndef TOOLSLIB_MAX_SPEED
                        if (!ItemsCount--) // integrity check failure
                        {
                            CMaaUnivHash_THROW;
                        }
#endif
                        if (This->K == K)
                        {
                            if (!fOverwrite)
                            {
                                Delete(rec);
                                return 1; // Exists
                            }
                            rec->pNext = This->pNext;
                            *Prev = rec;
                            Delete(This);
                            //ChangeIterators(This, rec);
                            return 0;
                        }
#ifndef TOOLSLIB_MAX_SPEED
                        if (This == This->pNext)
                        {
                            //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                            CMaaUnivHash_THROW;
                        }
#endif
                        Prev = &This->pNext;
                    }
                }
                catch (...)
                {
                    Delete(rec);
                    throw;
                }
            }
        }
        if (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            int Ret;
            try
            {
                if ((Ret = ResizeMoveSwap_(m_TableSize >= 3 ? 2 * m_TableSize + 1 : 3)))
                {
                    Delete(rec);
                    return Ret;
                }
            }
            catch (...)
            {
                Delete(rec);
                throw;
            }
            //Record* tmp = rec;
            //rec = nullptr;
            //Ret = AddRec(tmp, fOverwrite);
            //return Ret;
            // key K is not exist
            Prev = m_Hash + HashFunction___(K) % m_HashSize;
        }
        rec->pNext = *Prev;
        *Prev = rec;
        //rec = nullptr;
        m_ItemsCount++;
        return 0;
    }

public:
    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    //int AddOver(const Key &K, const Data &D) noexcept(noexcept(Add(K, D, 1)))
    //{
    //    return Add(K, D, 1);
    //}

    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory, 200 - Table failure in Data::Data() or other exception
    //int Add(const Key &K, const Data &D) noexcept(noexcept(Add(K, D, 0)))
    //{
    //    return Add(K, D, 0);
    //}

    void insert(const Key &K, const Data &D) noexcept(noexcept(Add(K, D)))
    {
        Add(K, D);
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find (const Key &K, Data *D /*= nullptr*/) const noexcept( noexcept(K==K) && noexcept(*D=*D) )
    {
        if  ((m_Flags & eNothingMask))//!m_Hash)
        {
            return 1;
        }

        Record ** Hash = m_Hash + HashFunction___(K) % m_HashSize;

        // searching for Key
        for (Record* This = *Hash; This; This = This->pNext)
        {
            if  (This->K == K)
            {
                if  (D)
                {
                    *D = This->D;
                }
                return 0; // 0: Key was found
            }
        }
        return 1; // 1: Key was not found
    }
    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find (const Key &K) const noexcept( noexcept(K==K) )
    {
        if  ((m_Flags & eNothingMask))//!m_Hash)
        {
            return 1;
        }

        Record** Hash = m_Hash + HashFunction___(K) % m_HashSize;

        // searching for Key
        for (Record* This = *Hash; This; This = This->pNext)
        {
            if (This->K == K)
            {
                return 0; // 0: Key was found
            }
        }
        return 1; // 1: Key was not found
    }
    bool contains(const Key &K) const noexcept(noexcept(Find(K)))
    {
        return !Find(K);
    }
    Data value(const Key &K) const noexcept(noexcept(K == K) && noexcept(Data(*(Data*)nullptr)))
    {
        //Data d;
        //Find(K, &d);
        //return d;
        return *((*this)[K]);
    }
    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to rerurn deleted context of Data
    int Remove(const Key &K, Data * D /*= nullptr*/) 
//#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept( noexcept(K==K) && noexcept(*D=*D) )
//#endif
    {
        if  ((m_Flags & eNothingMask))
        {
            return 1;
        }

        Record ** Prev = m_Hash + HashFunction___(K) % m_HashSize;
        // searching for Key
        for (Record *This = *Prev; This; This = This->pNext)
        {
            if  (This->K == K)
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                MoveIterators(This, Prev);
#else
                MoveIterators(This);
#endif
                if  (D)
                {
                    *D = This->D;
                }
                *Prev = This->pNext;
                m_ItemsCount--;
                Delete(This);

#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
                Squeeze();
#endif
                return 0; // 0: Key was found
            }
            Prev = &This->pNext;
        }
        return 1; // 1: Key was not found
    }

    // Removes element.
    // Returns 0 if ok.
    int Remove(const Key &K)
//#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept( noexcept(K==K) )
//#endif
    {
        if  ((m_Flags & eNothingMask))
        {
            return 1;
        }

        Record ** Prev = m_Hash + HashFunction___(K) % m_HashSize;
        // searching for Key
        for (Record* This = *Prev; This; This = This->pNext)
        {
            if  (This->K == K)
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                MoveIterators(This, Prev);
#else
                MoveIterators(This);
#endif
                * Prev = This->pNext;
                m_ItemsCount--;
                Delete(This);

#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
                Squeeze();
#endif
                return 0; // 0: Key was found
            }
            Prev = &This->pNext;
        }
        return 1; // 1: Key was not found
    }

    int Remove(Record* r, Record** prev, Data* D /*= nullptr*/)
//#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(*D = *D))
//#endif
    {
        if ((m_Flags & eNothingMask))
        {
            return 1;
        }
#ifdef TOOLSLIB_FAST_IT_REMOVE
        MoveIterators(r, prev);
#else
        MoveIterators(r);
#endif
        if (D)
        {
            *D = r->D;
        }
        *prev = r->pNext;
        m_ItemsCount--;
        Delete(r);

#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        Squeeze();
#endif
        return 0; // 0: Key was found
    }

    int Remove(Record* r, Record** prev)
//#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept
//#endif
    {
        if ((m_Flags & eNothingMask))
        {
            return 1;
        }
#ifdef TOOLSLIB_FAST_IT_REMOVE
        MoveIterators(r, prev);
#else
        MoveIterators(r);
#endif
        *prev = r->pNext;
        m_ItemsCount--;
        Delete(r);

#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        Squeeze();
#endif
        return 0;
    }

    //-==================================================
    int Remove(Record* r, Data* D /*= nullptr*/)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(*D = *D))
        //#endif
    {
        if ((m_Flags & eNothingMask))
        {
            return 1;
        }
        
        Record** Prev = m_Hash + HashFunction___(r->K) % m_HashSize;
        // searching for Key
        for (Record* This = *Prev; This; This = This->pNext)
        {
            if (This == r)
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                MoveIterators(This, Prev);
#else
                MoveIterators(This);
#endif
                if (D)
                {
                    *D = This->D;
                }
                *Prev = This->pNext;
                m_ItemsCount--;
                Delete(This);
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
                Squeeze();
#endif
                return 0; // 0: Key was found
            }
            Prev = &This->pNext;
        }
        return 1; // 1: Key was not found
    }

    int Remove(const Record* r)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept
        //#endif
    {
        if ((m_Flags & eNothingMask))
        {
            return 1;
        }

        Record** Prev = m_Hash + HashFunction___(r->K) % m_HashSize;
        // searching for Key
        for (Record* This = *Prev; This; This = This->pNext)
        {
            if (r == This)
            {
#ifdef TOOLSLIB_FAST_IT_REMOVE
                MoveIterators(This, Prev);
#else
                MoveIterators(This);
#endif
                *Prev = This->pNext;
                m_ItemsCount--;
                Delete(This);

#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
                Squeeze();
#endif
                return 0; // 0: Key was found
            }
            Prev = &This->pNext;
        }
        return 1; // 1: Key was not found
    }

    int Squeeze() noexcept //noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        if (!IsNeedToSqueeze())
        {
            return -1;
        }
        const size_t NewSize = m_ItemsCount >= 1 ? 2 * m_ItemsCount + 1 : 3;
        if (NewSize >= m_TableSize)
        {
            return -1;
        }

        // squeeze table

        try
        {
            return ResizeMoveSwap_(NewSize);
        }
        catch(...)
        {
            // No free elements, noexcept
        }
        return 2;
    }

    Data operator [] (const Key& K, const Data &DefaultValue) const noexcept(noexcept(K == K) && noexcept(Data(DefaultValue)))
    {
        if (!(m_Flags & eNothingMask))
        {
            Record** Hash = m_Hash + HashFunction___(K) % m_HashSize;
            // searching for Key
            for (Record* This = *Hash; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    return This->D;
                }
            }
        }
        return DefaultValue;
    }

    Data* operator [] (const Key& K) const noexcept(noexcept(K == K))
    {
        if (!(m_Flags & eNothingMask))
        {
            Record** Hash = m_Hash + HashFunction___(K) % m_HashSize;
            // searching for Key
            for (Record* This = *Hash; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    return &This->D;
                }
            }
        }
        return nullptr;
    }

    const Data& operator [] (const Key& K, const Data& DefaultValue, bool) const noexcept(noexcept(K == K))
    {
        if (!(m_Flags & eNothingMask))
        {
            Record** Hash = m_Hash + HashFunction___(K) % m_HashSize;
            // searching for Key
            for (Record* This = *Hash; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    return This->D;
                }
            }
        }
        return DefaultValue;
    }

    Data& operator [] (const Key& K, Data& DefaultValue, bool) noexcept(noexcept(K == K))
    {
        if (!(m_Flags & eNothingMask))
        {
            Record** Hash = m_Hash + HashFunction___(K) % m_HashSize;
            // searching for Key
            for (Record* This = *Hash; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    return This->D;
                }
            }
        }
        return DefaultValue;
    }

    const Record* FindRec(const Key& K) const noexcept(noexcept(K == K))
    {
        if (!(m_Flags & eNothingMask))
        {
            Record** Hash = m_Hash + HashFunction___(K) % m_HashSize;
            // searching for Key
            for (Record* This = *Hash; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    return This;
                }
            }
        }
        return nullptr;
    }

    /*
    Data Get(const Key& K, const Data& DefaultValue) const noexcept(noexcept(K == K) && noexcept(Data(DefaultValue)))
    {
        Data* p = (*this)[K];
        return p ? *p : DefaultValue;
    }
    */

protected:
    // returns GetItemCount()
    int EnumeratePtrs(Key ** Keys, Data ** Datas) const noexcept
    {
        Key ** p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record * This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = &This->K;
                *Datas++ = &This->D;
            }
        }
        return (int)(p - Keys);
    }
    int EnumerateKeysPtrs(Key ** Keys) const noexcept
    {
        Key ** p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record * This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = &This->K;
            }
        }
        return (int)(p - Keys);
    }
    int EnumerateDatasPtrs(Data ** Datas) const noexcept
    {
        Data ** p = Datas;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record * This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = &This->D;
            }
        }
        return (int)(p - Datas);
    }

public:
    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumeratePtrs(int Size, Key ** Keys, Data ** Datas) const noexcept
    {
        if  (Size >= 0)
        {
            //InvalidateIterators();
#ifdef TOOLSLIB_MAX_SPEED
            if  (Size >= GetItemCount())
            {
                // super opt, unsafe
                return Keys && !Datas ? EnumerateKeysPtrs(Keys) : !Keys && Datas ? EnumerateDatasPtrs(Datas) : Keys && Datas ? EnumeratePtrs(Keys, Datas) : GetItemCount();
            }
#endif
            int j = 0;
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                // scaning all Keys
                for (Record * This = m_Hash[i]; This; This = This->pNext)
                {
                    if  (j >= Size)
                    {
                        return -2;
                    }
                    if  (Keys)
                    {
                        Keys[j] = &This->K;
                    }
                    if  (Datas)
                    {
                        Datas[j] = &This->D;
                    }
                    j++;
                }
            }
            return j;
        }
        return -1;
    }

protected:
    // returns GetItemCount()
    int EnumerateKeys(Key * Keys) const noexcept( noexcept(*Keys=*Keys) )
    {
        Key * p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record * This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = This->K;
            }
        }
        return (int)(p - Keys);
    }
    int EnumerateDatas(Data * Datas) const noexcept( noexcept(*Datas=*Datas) )
    {
        Data * p = Datas;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record * This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = This->D;
            }
        }
        return (int)(p - Datas);
    }
    int EnumerateItems(Key * Keys, Data * Datas) const noexcept( noexcept(*Keys=*Keys) && noexcept(*Datas=*Datas) )
    {
        Key * p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record * This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = This->K;
                *Datas++ = This->D;
            }
        }
        return (int)(p - Keys);
    }

public:
    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumerateItems(int Size, Key * Keys, Data * Datas) const noexcept( noexcept(*Keys=*Keys) && noexcept(*Datas=*Datas) )
    {
        if  (Size >= 0)
        {
#ifdef TOOLSLIB_MAX_SPEED
            if  (Size >= GetItemCount())
            {
                // super opt, unsafe
                return Keys && !Datas ? EnumerateKeys(Keys) : !Keys && Datas ? EnumerateDatas(Datas) : Keys && Datas ? EnumerateItems(Keys, Datas) : GetItemCount();
            }
#endif
            int j = 0;
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                // scaning all Keys
                for (Record * This = m_Hash[i]; This; This = This->pNext)
                {
                    if  (Keys)
                    {
                        if  (j >= Size)
                        {
                            return -2;
                        }
                        *Keys = This->K;
                        Keys++;
                    }
                    if  (Datas)
                    {
                        if  (j >= Size)
                        {
                            return -2;
                        }
                        *Datas = This->D;
                        Datas++;
                    }
                    j++;
                }
            }
            return j;
        }
        return -1;
    }

    int GetItemCount () const noexcept
    {
        return (int)m_ItemsCount;
        //return EnumerateItems ( 0x7fffffff, nullptr, nullptr );
    }

    typedef void (*EnumerateProc)(const Key& K, Data& D, void* Param);
    typedef void (*EnumerateProcEx)(const Key& K, Data& D, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& ht, void* Param);
    typedef void (*EnumerateProcNE)(const Key& K, Data& D, void* Param) noexcept;
    typedef void (*EnumerateProcExNE)(const Key& K, Data& D, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& ht, void* Param) noexcept;

    // true to Remove() item
    typedef bool (*DelEnumerateProc)(const Key& K, Data& D, void* Param);
    typedef bool (*DelEnumerateProcEx)(const Key& K, Data& D, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& ht, void* Param);
    typedef bool (*DelEnumerateProcNE)(const Key& K, Data& D, void* Param) noexcept;
    typedef bool (*DelEnumerateProcExNE)(const Key& K, Data& D, CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>& ht, void* Param) noexcept;

    void EnumerateByProc(EnumerateProc Proc, void * Param)
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        Record * Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            for (Record * This = m_Hash[i]; This; This = Next)
            {
                Next = This->pNext;
                Proc(This->K, This->D, Param);
            }
        }
    }

    void EnumerateByProc(EnumerateProcEx Proc, void * Param)
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        Record * Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            for (Record * This = m_Hash[i]; This; This = Next)
            {
                Next = This->pNext;
                Proc(This->K, This->D, *this, Param);
            }
        }
    }

    void EnumerateByProc(EnumerateProcNE Proc, void* Param) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            for (Record* This = m_Hash[i]; This; This = Next)
            {
                Next = This->pNext;
                Proc(This->K, This->D, Param);
            }
        }
    }

    void EnumerateByProc(EnumerateProcExNE Proc, void* Param) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            for (Record* This = m_Hash[i]; This; This = Next)
            {
                Next = This->pNext;
                Proc(This->K, This->D, *this, Param);
            }
        }
    }

    int DelByEnumerateByProc(DelEnumerateProc Proc, void* Param)
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        int N = 0;
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            Record** Prev = m_Hash + i;
            for (Record* This = *Prev; This; This = Next)
            {
                Next = This->pNext;
                if (Proc(This->K, This->D, Param))
                {
                    Remove(This, Prev);
                    N++;
                }
                else
                {
                    Prev = &This->pNext;
                }
            }
        }
        return N;
    }

    int DelByEnumerateByProc(DelEnumerateProcEx Proc, void* Param)
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        int N = 0;
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            Record** Prev = m_Hash + i;
            for (Record* This = *Prev; This; This = Next)
            {
                Next = This->pNext;
                if (Proc(This->K, This->D, *this, Param))
                {
                    Remove(This, Prev);
                    N++;
                }
                else
                {
                    Prev = &This->pNext;
                }
            }
        }
        return N;
    }

    int DelByEnumerateByProc(DelEnumerateProcNE Proc, void* Param) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        int N = 0;
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            Record** Prev = m_Hash + i;
            for (Record* This = *Prev; This; This = Next)
            {
                Next = This->pNext;
                if (Proc(This->K, This->D, Param))
                {
                    Remove(This, Prev);
                    N++;
                }
                else
                {
                    Prev = &This->pNext;
                }
            }
        }
        return N;
    }

    int DelByEnumerateByProc(EnumerateProcExNE Proc, void* Param) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        int N = 0;
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            Record** Prev = m_Hash + i;
            for (Record* This = *Prev; This; This = Next)
            {
                Next = This->pNext;
                if (Proc(This->K, This->D, *this, Param))
                {
                    Remove(This, Prev);
                    N++;
                }
                else
                {
                    Prev = &This->pNext;
                }
            }
        }
        return N;
    }

    template<typename Fn> void EnumerateByProc(Fn Proc)
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        Record * Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            for (Record * This = m_Hash[i]; This; This = Next)
            {
                Next = This->pNext;
                Proc((const Key &)(This->K), This->D);
            }
        }
    }

    template<typename Fn> int DelByEnumerateByProc(Fn Proc)
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        AutoSqueezeLocker l(*this);
#endif
        int N = 0;
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            Record** Prev = m_Hash + i;
            for (Record* This = *Prev; This; This = Next)
            {
                Next = This->pNext;
                if (Proc((const Key&)(This->K), This->D))
                {
                    Remove(This, Prev);
                    N++;
                }
                else
                {
                    Prev = &This->pNext;
                }
            }
        }
        return N;
    }

    static _qword GetHashTablesAllocSize() noexcept
    {
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        return gHashTablesAllocSize;
#else
        return -1;
#endif
    }
};
//---------------------------------------------------------------------------
template<class Key, class Data> class CMaaUnivHash_New_KeyIsClass : public CMaaUnivHash_New<Key, Data, false>
{
    TOOLSLIB_HT_HASH HashFunction(const Key &K) const noexcept override
    {
#ifndef TOOLSLIB_HT64
        return K.Hash();
#else
        return K.Hash64();
#endif
    }
public:
    CMaaUnivHash_New_KeyIsClass() noexcept(noexcept_new)
    {
    }
    CMaaUnivHash_New_KeyIsClass(size_t Size) noexcept(noexcept_new)
        : CMaaUnivHash_New<Key, Data, false>(Size)
    {
    }

    CMaaUnivHash_New_KeyIsClass(size_t Size, CMaaUnivHash_New_KeyIsClass<Key, Data>* ptr, int preallocate, int Flags
#ifndef TOOLSLIB_CHECK_HT_USAGE
        = TOOLSLIB_HT_DEF_FLAG
#else
        TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#endif
    ) noexcept(noexcept_new)
        : CMaaUnivHash_New<Key, Data, false>(Size, ptr, preallocate, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1)
    {
    }

    int ResizeMoveSwap(bool bMove, size_t Size) noexcept(noexcept_new) override //, int preallocate = 0, int allocator_type = -1)
    {
        CMaaUnivHash_New_KeyIsClass<Key, Data> tmp(Size, this, 0, this->m_Flags TOOLSLIB_CHECK_HT_USAGE_ARG1); // preallocate is ignored by (Flags & 0x80000000);
        if (bMove)
        {
            return CMaaUnivHash_New<Key, Data, false>::MoveSwap(tmp);
        }
        CMaaUnivHash_New<Key, Data, false>::Swap(tmp);
        return 0;
    }

    CMaaUnivHash_New_KeyIsClass(CMaaUnivHash_New_KeyIsClass<Key, Data>&& That) noexcept(noexcept_new)
    :   CMaaUnivHash_New<Key, Data, false>(std_move((CMaaUnivHash_New<Key, Data, false>&&)That))
    {
    }

    CMaaUnivHash_New_KeyIsClass<Key, Data> & operator=(CMaaUnivHash_New_KeyIsClass<Key, Data>&& That) noexcept(noexcept_new)
    {
        CMaaUnivHash_New<Key, Data, false>::operator=(std_move((CMaaUnivHash_New<Key, Data, false>&&)That));
        return *this;
    }
};
//---------------------------------------------------------------------------
template<class Key, class Data, bool bStandartHash> class CMaaUnivHash_NewClassPtr : public CMaaUnivHash_New<Key, Data, bStandartHash>
{
public:
    CMaaUnivHash_NewClassPtr() noexcept(noexcept_new)
    {
    }
    CMaaUnivHash_NewClassPtr(size_t Size) noexcept(noexcept_new)
        : CMaaUnivHash_New<Key, Data, bStandartHash>(Size)
    {
    }

    CMaaUnivHash_NewClassPtr(size_t Size, CMaaUnivHash_NewClassPtr<Key, Data, bStandartHash>* ptr, int preallocate, int Flags
#ifndef TOOLSLIB_CHECK_HT_USAGE
        = TOOLSLIB_HT_DEF_FLAG
#else
        TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#endif
    ) noexcept(noexcept_new)
        : CMaaUnivHash_New<Key, Data, bStandartHash>(Size, ptr, preallocate, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1)
    {
    }

    int ResizeMoveSwap(bool bMove, size_t Size) noexcept(noexcept_new) override //, int preallocate = 0, int allocator_type = -1)
    {
        CMaaUnivHash_NewClassPtr<Key, Data, bStandartHash> tmp(Size, this, 0, this->m_Flags TOOLSLIB_CHECK_HT_USAGE_ARG1); // preallocate is ignored by (Flags & 0x80000000);
        if (bMove)
        {
            return CMaaUnivHash_New<Key, Data, bStandartHash>::MoveSwap(tmp);
        }
        CMaaUnivHash_New<Key, Data, bStandartHash>::Swap(tmp);
        return 0;
    }

    static void ObjectDeleter(Key &/*K*/, Data &D, CMaaUnivHash_New<Key, Data, bStandartHash> &/*ht*/, void * /*Param*/) noexcept
    {
        delete D;
    }

    ~CMaaUnivHash_NewClassPtr() noexcept
    {
        CMaaUnivHash_New<Key, Data, bStandartHash>::InvalidateIterators();
        EnumerateByProc(ObjectDeleter);
    }

    CMaaUnivHash_NewClassPtr(CMaaUnivHash_NewClassPtr<Key, Data, bStandartHash>&& That) noexcept(noexcept_new)
    :   CMaaUnivHash_New<Key, Data, bStandartHash>(std_move((CMaaUnivHash_New<Key, Data, bStandartHash>&&)That))
    {
    }

    CMaaUnivHash_NewClassPtr<Key, Data, bStandartHash>& operator=(CMaaUnivHash_NewClassPtr<Key, Data, bStandartHash>&& That) noexcept(noexcept_new)
    {
        CMaaUnivHash_New<Key, Data, bStandartHash>::operator=(std_move((CMaaUnivHash_New<Key, Data, bStandartHash>&&)That));
        return *this;
    }
};
//---------------------------------------------------------------------------
template<class Key, class Data, bool bStandartHash = true, int AllocatorType =
#ifdef TOOLSLIB_SHARED_ALLOCATOR
    1
#else
    - 1
#endif
> class CMaaUnivHash_NewA : public CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>
{
    /*
    TOOLSLIB_HT_HASH HashFunction(const Key& K) const noexcept override
    {
#ifndef TOOLSLIB_HT64
        return K.Hash();
#else
        return K.Hash64();
#endif
    }
    */
public:
    CMaaUnivHash_NewA() noexcept(noexcept_new)
    {
    }
    CMaaUnivHash_NewA(size_t Size) noexcept(noexcept_new)
    :   CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>(Size)
    {
    }

    CMaaUnivHash_NewA(size_t Size, CMaaUnivHash_NewA<Key, Data, bStandartHash, AllocatorType>* ptr, int preallocate, int Flags
#ifndef TOOLSLIB_CHECK_HT_USAGE
        = TOOLSLIB_HT_DEF_FLAG
#else
        TOOLSLIB_CHECK_HT_USAGE_ADD_ARG1
#endif
    ) noexcept(noexcept_new)
    :   CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>(Size, ptr, preallocate, Flags TOOLSLIB_CHECK_HT_USAGE_ARG1)
    {
    }

protected:
    int ResizeMoveSwap(bool bMove, size_t Size) noexcept(noexcept_new) override //, int preallocate = 0, int allocator_type = -1)
    {
        CMaaUnivHash_NewA<Key, Data, bStandartHash, AllocatorType> tmp(Size, this, 0, this->m_Flags TOOLSLIB_CHECK_HT_USAGE_ARG1); // preallocate is ignored by (Flags & 0x80000000);
        if (bMove)
        {
            return CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::MoveSwap(tmp);
        }
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::Swap(tmp);
        return 0;
    }

public:
    CMaaUnivHash_NewA(CMaaUnivHash_NewA<Key, Data, bStandartHash, AllocatorType>&& That) noexcept(noexcept_new)
    :   CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>(std_move((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>&&)That))
    {
    }

    CMaaUnivHash_NewA<Key, Data, bStandartHash, AllocatorType>& operator=(CMaaUnivHash_NewA<Key, Data, bStandartHash, AllocatorType>&& That) noexcept(noexcept_new)
    {
        CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>::operator=(std_move((CMaaUnivHash_New<Key, Data, bStandartHash, AllocatorType>&&)That));
        return *this;
    }
    typedef CMaaUnivHash_NewA<Key, Data, bStandartHash, AllocatorType> HtType;
    ADD_ALLOCATOR(HtType)
};


//---------------------------------------------------------------------------
#undef HashFunction___
//---------------------------------------------------------------------------

#undef TOOLSLIB_HT_HASH
#undef CMaaUnivHash_New
#undef CMaaUnivHash_New_KeyIsClass
#undef CMaaUnivHash_NewClassPtr
#undef CMaaUnivHash_NewA

//---------------------------------------------------------------------------
// CMaaUnivHash_New2 has the same speed as CMaaUnivHash_New
//---------------------------------------------------------------------------
//#include "Hash_New2.h"
//---------------------------------------------------------------------------


//#endif // __MAARF_MAATL_H
