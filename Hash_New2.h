//---------------------------------------------------------------------------
// CMaaUnivHash_New2 has the same speed as CMaaUnivHash_New
//---------------------------------------------------------------------------
#if 0
template < class Key, class Data, bool bStandartHash = true > class CMaaUnivHash_New2
{
    //public:
    //protected:
    struct Record
    {
        Record* pNext;
        Key  K;
        Data D;
        Record(const Key k, const Data& d) noexcept(noexcept(Key(K)) && noexcept(Data(d)))
        :   K(k),
            D(d)
        {
        }
#ifdef TOOLSLIB_SHARED_ALLOCATOR
        //ADD_UL_ALLOCATOR(Record)
        ADD_ALLOCATOR(Record)
#define DEF3_ALLOCATOR_CMaaUnivHash2(A,B,F) template<> CMaaFixedAllocator< CMaaUnivHash_New2< A, B, F >::Record >* CMaaUnivHash_New2< A, B, F >::Record::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaUnivHash_New2< A, B, F >::Record > CMaaUnivHash_New2< A, B, F >::Record::s_AllocatorCreator(&CMaaUnivHash_New2< A, B, F >::Record::s_pAllocator);
            //#define DEF3_ALLOCATOR_CMaaUnivHash2(A,B,F) template<> CMaaFixedAllocator< CMaaUnivHash_New2< A, B, F >::Record >* CMaaUnivHash_New2< A, B, F >::Record::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaUnivHash_New2< A, B, F >::Record > CMaaUnivHash_New2< A, B, F >::Record::s_AllocatorCreator(&CMaaUnivHash_New2< A, B, F >::Record::s_pAllocator);
#define DEF_ALLOCATOR_CMaaUnivHash2(A,B) DEF3_ALLOCATOR_CMaaUnivHash2(A,B,true)
#else
#define DEF3_ALLOCATOR_CMaaUnivHash2(A,B,F)
#define DEF_ALLOCATOR_CMaaUnivHash2(A,B)
#endif
    };

#ifndef TOOLSLIB_SHARED_ALLOCATOR
    CMaaFixedAllocator<Record>* m_pAllocator;
#endif

    size_t m_TableSize, m_ItemsCount;   // size of item table, number of added items
    size_t m_HashSize;    // size of hash table

    Record** m_Hash;   // hash table
    //int m_Err; // 0 - ok, -1 - init by 0 size (recreate table on Add), 1 - error

    int m_Flags;
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
    int m_AutoSqueezeLock;
#endif

public:
    enum // m_Flags
    {
        eErr1 = 0x01,
        eOk0 = 0x02,
        eNothingMask = eErr1 | eOk0,
        eAutoSqueeze = 0x04,
        eDefaultArgs = 0x80000000 // Default preallocate and allocator_type
    };

protected:
    // hash function of key
    //#define ___HashFunction2 CMaaHashFunction
#define ___HashFunction2(K) (bStandartHash ? CMaaHashFunction2(K) : HashFunction(K))
    virtual _dword HashFunction(const Key K) const noexcept
    {
        /*
          if   (sizeof(Key) == sizeof(_dword))
          {
               return *(_dword*)&K;
          }
          else
          {
               unsigned char * p = (unsigned char *)&K;
               _dword x = 0;
               for  (int i = sizeof(Key); i--;)
               {
                    x = CMaaRol7dwMacro(x) ^ *p++;
               }
               return x;
          }
          */
        return CMaaHashFunction2(K);
    }

public:

    class iterator : public CMaaDLink
    {
        friend class CMaaUnivHash_New2<Key, Data, bStandartHash>;
        CMaaUnivHash_New2<Key, Data, bStandartHash>* h;
        typename CMaaUnivHash_New2<Key, Data, bStandartHash>::Record* r;
        size_t index;
        bool bInc;
    public:
        iterator() noexcept
            : h(nullptr)
        {
            Init();
        }
        iterator(CMaaUnivHash_New2<Key, Data, bStandartHash>& ht) noexcept
            : h(nullptr)
        {
            CMaaDLink::Init();
            *this = ht.it();
        }
        iterator(const iterator& That) noexcept
            : CMaaDLink(),
            h(nullptr)
        {
            CMaaDLink::Init();
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
            if (h)
            {
                h->m_Iterators.Release(this);
                h->UnLockAutoSqueeze();
                h = nullptr;
            }
        }
        iterator& operator=(const iterator& that) noexcept
        {
            iterator& That = (iterator&)that;
            if (this != &That)
            {
                Release();
                if ((h = That.h))
                {
                    h->LockAutoSqueeze();
                    r = That.r;
                    index = That.index;
                    bInc = That.bInc;
                    h->m_Iterators.AddAtFront(this);
                }
            }
            return *this;
        }
        iterator& operator=(CMaaUnivHash_New2<Key, Data, bStandartHash>& ht) noexcept
        {
            Release();
            *this = ht.it();
            return *this;
        }
        bool IsValid() const noexcept
        {
            return h && !bInc && r;
        }
        operator bool() const noexcept
        {
            return IsValid();
        }
        // fast, noexcept functions w/o of validity check
        const Key ckey() const noexcept
        {
            return r->K;
        }
        Key key() noexcept(noexcept(Key(r->K))) //const
        {
            return r->K;
        }
        Data& rdata() noexcept
        {
            return r->D;
        }
        Data data() noexcept(noexcept(Data(r->D))) //const
        {
            return r->D;
        }

        const Key ckeyEx() const
        {
            if (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Key keyEx() //const
        {
            if (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Data& rdataEx()
        {
            if (!IsValid())
            {
                throw 1;
            }
            return r->D;
        }
        Data dataEx() //const
        {
            if (!IsValid())
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
            if (h) // remark
            {
                if (!bInc)
                {
                    Inc();
                }
                if (bInc)
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
        bool Inc() noexcept
        {
            if (h && r)
            {
                r = r->pNext;
                if (!r)
                {
                    while (++index < h->m_HashSize)
                    {
                        if ((r = h->m_Hash[index]))
                        {
                            break;
                        }
                    }
                }
                if (r)
                {
                    bInc = true;
                    return true;
                }
            }
            Release();
            return false;
        }
        int Remove(Data* d) noexcept(noexcept(r->K == r->K) && noexcept(*d = *d)) //noexcept( noexcept(h->Remove(r->K, d)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
        {
            if (IsValid())
            {
                return h->Remove(r->K, d);
            }
            return 2;
        }
        int Remove() noexcept(noexcept(r->K == r->K)) //noexcept( noexcept(h->Remove(r->K)) ) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
        {
            if (IsValid())
            {
                return h->Remove(r->K);
            }
            return 2;
        }
    };

    class AutoSqueezeLocker
    {
        //friend class CMaaUnivHash_New2<Key, Data>;
        CMaaUnivHash_New2<Key, Data, bStandartHash>& h;
    public:
        AutoSqueezeLocker(CMaaUnivHash_New2<Key, Data, bStandartHash>& ht) noexcept
            : h(ht)
        {
            Lock();
        }
        AutoSqueezeLocker(const AutoSqueezeLocker& That) = delete;
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
            if (m_Hash[i])
            {
                iterator it;
                LockAutoSqueeze();
                it.h = this;
                it.index = i;
                it.r = m_Hash[i];
                it.bInc = false;
                m_Iterators.AddAtFront(&it);
                return it;
            }
        }
        return iterator();
    }
    void InvalidateIterators() noexcept
    {
        iterator* p;
        while ((p = m_Iterators.GetFromFront()))
        {
            if (p->h)
            {
                UnLockAutoSqueeze();
                p->h = nullptr;
            }
        }
    }
    void MoveIterators(const Record* r) noexcept
    {
        iterator* n;
        for (iterator* p = m_Iterators.LookAtFront(); p; p = n)
        {
            n = m_Iterators.Next(p);
            if (p->h && p->r == r)
            {
                p->Inc();
            }
        }
    }
    void ChangeIterators(const Record* pOld, Record* pNew) noexcept
    {
        iterator* n;
        for (iterator* p = m_Iterators.LookAtFront(); p; p = n)
        {
            n = m_Iterators.Next(p);
            if (p->h && p->r == pOld)
            {
                p->r = pNew;
            }
        }
    }

    virtual void Swap(CMaaUnivHash_New2<Key, Data, bStandartHash>& That) noexcept
    {
        CMaaSwap(m_TableSize, That.m_TableSize);
        CMaaSwap(m_HashSize, That.m_HashSize);
        CMaaSwap(m_ItemsCount, That.m_ItemsCount);
        CMaaSwap(m_Hash, That.m_Hash);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        CMaaSwap(m_pAllocator, That.m_pAllocator);
#endif
        //CMaaSwap(m_Err,        That.m_Err);
        CMaaSwap(m_Flags, That.m_Flags);
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //!!!//CMaaSwap(m_AutoSqueezeLock, That.m_AutoSqueezeLock);
#endif

        CMaaDList<iterator> l1, l2;
        l1.MoveThatToTheFront(m_Iterators);
        l2.MoveThatToTheFront(That.m_Iterators);

        iterator* p;
        while ((p = l1.GetFromFront()))
        {
            if (p->h)
            {
                That.LockAutoSqueeze();
                p->h->UnLockAutoSqueeze();
                p->h = &That;
                That.m_Iterators.AddAtBack(p);
            }
        }
        while ((p = l2.GetFromFront()))
        {
            if (p->h)
            {
                LockAutoSqueeze();
                p->h->UnLockAutoSqueeze();
                p->h = this;
                m_Iterators.AddAtBack(p);
            }
        }
    }

    CMaaUnivHash_New2<Key, Data, bStandartHash>& operator=(CMaaUnivHash_New2<Key, Data, bStandartHash>&& That) noexcept
    {
        Swap(That);
        That.Cleanup();
        return *this;
    }

    virtual CMaaUnivHash_New2<Key, Data, bStandartHash>* MakeNewTable(size_t Size, CMaaUnivHash_New2<Key, Data, bStandartHash>* ptr, int Flags)//, int preallocate = 0, int allocator_type = -1)
    {
        return TL_NEW CMaaUnivHash_New2<Key, Data, bStandartHash>(Size, ptr, 0, -1, Flags);//, preallocate, allocator_type, Flags);
    }

private:
    void Init(size_t Size, int Flags)
    {
        if (Size < 2)
        {
            Size = 0;
        }
        Flags &= ~eNothingMask;
        const size_t MaxSz = ((size_t)-1) / sizeof(Record*) / 2;
        m_HashSize = (Size + Size / 10 <= MaxSz ? Size + Size / 10 : MaxSz);
        m_TableSize = Size;
        m_ItemsCount = 0;
        m_Hash = Size && Size < SIZE_MAX / sizeof(void*) / 11 * 10 ? TL_NEW Record * [m_HashSize] : nullptr;
        if (m_Hash)
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
    CMaaUnivHash_New2<Key, Data, bStandartHash>& operator = (const CMaaUnivHash_New2<Key, Data, bStandartHash>& That)
    {
        if (this != &That)
        {
            iterator it((CMaaUnivHash_New2<Key, Data, bStandartHash> &)That);
            Cleanup();
            for (; it; ++it)
            {
                Add(it.key(), it.data());
            }
        }
        return *this;
    }
    CMaaUnivHash_New2(size_t Size = 16, CMaaUnivHash_New2<Key, Data, bStandartHash>* ptr = nullptr, int iPreallocate = 0, int allocator_type = -1, int Flags = TOOLSLIB_HT_DEF_FLAG)
    {
        if (Flags & eDefaultArgs)
        {
            iPreallocate = 0;
            allocator_type = -1;
            Flags &= ~eDefaultArgs;
        }

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        m_pAllocator = nullptr;
#endif
        Init(Size, Flags);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if (ptr)
        {
            m_pAllocator = ptr->m_pAllocator;
        }
        if (!m_pAllocator)
        {
            m_pAllocator = TL_NEW CMaaFixedAllocator<Record>(ptr && ptr->m_pAllocator ? ptr->m_pAllocator->GetAllocatorType() : allocator_type);
        }
        if (m_pAllocator)
        {
            m_pAllocator->AddRefEx();
            if ((!ptr && iPreallocate >= 0) || iPreallocate > 0)
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
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                    if (m_Hash)
                    {
                        gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
                    }
#endif
                    delete[] m_Hash;  m_Hash = nullptr;
                    throw;
                }
            }
        }
        else
        {
            m_Flags |= eErr1;
        }
#else
        //if (!CMaaFixedAllocator<CMaaUnivHash_New2<Key, Data>::Record>::CreateAddRefUl(&Record::s_pAllocator))
        if (!CMaaFixedAllocator<CMaaUnivHash_New2<Key, Data, bStandartHash>::Record>::CreateAddRef(&Record::s_pAllocator))
        {
            m_Flags |= eErr1;
        }
#endif
    }

    void SetAutoSqueeze(bool b) noexcept
    {
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        m_Flags = b ? (m_Flags | eAutoSqueeze) : (m_Flags & ~eAutoSqueeze);
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

    CMaaUnivHash_New2(CMaaUnivHash_New2<Key, Data, bStandartHash>&& That)
    {
        size_t Size = 0;
        int iPreallocate = 0;
        int allocator_type = -1;
        CMaaUnivHash_New2<Key, Data, bStandartHash>* ptr = /*nullptr; // =*/ (CMaaUnivHash_New2<Key, Data, bStandartHash> *) & That;

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        m_pAllocator = nullptr;
#endif
        Init(Size, 0);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if (ptr)
        {
            m_pAllocator = ptr->m_pAllocator;
        }
        if (!m_pAllocator)
        {
            m_pAllocator = TL_NEW CMaaFixedAllocator<Record>(ptr && ptr->m_pAllocator ? ptr->m_pAllocator->GetAllocatorType() : allocator_type);
        }
        if (m_pAllocator)
        {
            m_pAllocator->AddRefEx();
            if ((!ptr && iPreallocate >= 0) || iPreallocate > 0)
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
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                    if (m_Hash)
                    {
                        gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
                    }
#endif
                    delete[] m_Hash;  m_Hash = nullptr;
                    throw;
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

    CMaaUnivHash_New2(const CMaaUnivHash_New2<Key, Data, bStandartHash>& That)
    {
        size_t Size = 16;
        int iPreallocate = 0;
        int allocator_type = -1;
        CMaaUnivHash_New2<Key, Data, bStandartHash>* ptr = /*nullptr; // =*/ (CMaaUnivHash_New2<Key, Data, bStandartHash> *) & That;

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        m_pAllocator = nullptr;
#endif
        Init(Size, That.m_Flags);
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if (ptr)
        {
            m_pAllocator = ptr->m_pAllocator;
        }
        if (!m_pAllocator)
        {
            m_pAllocator = TL_NEW CMaaFixedAllocator<Record>(ptr && ptr->m_pAllocator ? ptr->m_pAllocator->GetAllocatorType() : allocator_type);
        }
        if (m_pAllocator)
        {
            m_pAllocator->AddRefEx();
            if ((!ptr && iPreallocate >= 0) || iPreallocate > 0)
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
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                    if (m_Hash)
                    {
                        gHashTablesAllocSize -= (_qword)(m_HashSize * sizeof(*m_Hash));
                    }
#endif
                    delete[] m_Hash;  m_Hash = nullptr;
                    throw;
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

    void unite(const CMaaUnivHash_New2<Key, Data, bStandartHash>& OtherTable)
    {
        iterator it((CMaaUnivHash_New2<Key, Data, bStandartHash> &)OtherTable);
        for (; it; ++it)
        {
            Add(it.key(), it.data());
        }
    }
    const Data value(const Key key, const Data& defaultValue) const
    {
        Data d = defaultValue;
        ((CMaaUnivHash_New2<Key, Data, bStandartHash>&) * this).Find(key, &d);
        return d;
    }
    bool isEmpty() const noexcept
    {
        return GetItemCount() == 0;
    }

    bool IsOK() const noexcept
    {
        return !(m_Flags & eErr1);
    }

    virtual ~CMaaUnivHash_New2() noexcept
    {
        InvalidateIterators();

        if (m_ItemsCount > 0 && m_Hash)
        {
            //printf("deleting %d objects...", (int)m_ItemsCount);
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
                    m_pAllocator->DeleteEx(r, &m_pAllocator);
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
        delete[] m_Hash;  m_Hash = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        if (m_pAllocator)
        {
            m_pAllocator->DelRefEx(&m_pAllocator);
        }
#else
        // 2023+:
        Record::s_pAllocator->DelRefEx(&Record::s_pAllocator);
#endif
        m_Flags |= eErr1;
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
                    m_pAllocator->DeleteEx(r, &m_pAllocator);
#else
                    delete r;
#endif
                }
                m_Hash[i] = nullptr;
            }
            m_ItemsCount = 0;
        }
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //Squeeze();
#endif
    }

    void Cleanup(int NewTableSize)
    {
        if (NewTableSize <= 0)
        {
            Cleanup();
        }
        else
        {
            InvalidateIterators();
            CMaaUnivHash_New2<Key, Data, bStandartHash>* NewTable = MakeNewTable(NewTableSize, this, m_Flags);//, 0, m_pAllocator ? m_pAllocator->GetAllocatorType() : 0);
            if (NewTable)
            {
                Swap(*NewTable);
                delete NewTable;
            }
        }
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        //Squeeze();
#endif
    }

    size_t GetMaxIndex() const noexcept
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

#ifndef TOOLSLIB_SHARED_ALLOCATOR
    struct RR // return record
    {
        Record*& m_pr;
        CMaaFixedAllocator<Record>* m_pAllocator;

        RR(Record*& pr, CMaaFixedAllocator<Record>* pAllocator) noexcept
            : m_pr(pr),
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
                //CMaaFixedAllocator<Record>::DeleteEx(m_pr, &m_pAllocator);
                m_pAllocator->DeleteEx(m_pr, &m_pAllocator);
            }
        }
    };
#else
    struct RR // return record
    {
        Record*& m_pr;

        RR(Record*& pr) noexcept
            : m_pr(pr)
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

    virtual void test__(void* /*a*/ = nullptr, void* /*b*/ = nullptr, void* /*c*/ = nullptr, int /*x1*/ = 0, int /*x2*/ = 0)
    {
    }

protected:
    void FastAddRec(Record* rec) noexcept
    {
        Record** Hash = m_Hash + ___HashFunction2(rec->K) % m_HashSize;
        rec->pNext = *Hash;
        *Hash = rec;
        m_ItemsCount++;
    }

public:
    int NewMoveSwap(size_t sz)
    {
        if (sz < m_ItemsCount)
        {
            return 3; // param err
        }
        // expand table or first create on m_Err == -1
        CMaaUnivHash_New2<Key, Data, bStandartHash>* NewTable = MakeNewTable(sz, this, m_Flags);//, 0, m_pAllocator ? m_pAllocator->GetAllocatorType() : 0);
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
                    Record* n;
                    for (Record* r = m_Hash[i]; r; r = n)
                    {
                        n = r->pNext;
                        m_ItemsCount--;
                        NewTable->FastAddRec(r);
                    }
                    m_Hash[i] = nullptr;
                    /*
                        Record ** pHash = &m_Hash[i];
                        while (*pHash)
                        {
                            Record * r = *pHash;
                            *pHash = r->pNext;
                            m_ItemsCount--;
                            NewTable->FastAddRec(r);
                        }
                    */
                }
            }
            Swap(*NewTable);
        }
        delete NewTable;
        return 0; // ok
    }

    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key K, const Data& D, int fOverwrite)
    {
        InvalidateIterators();

        Record* rec = nullptr;
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        RR RR_rec(rec, m_pAllocator);
#else
        RR RR_rec(rec);
#endif

#if 0
        if (m_ItemsCount >= m_TableSize)
        {
            int Ret;
            return (Ret = NewMoveSwap(2 * m_TableSize)) ? Ret : Add(K, D, fOverwrite);
        }
#endif
        if ((m_Flags & eErr1))
        {
            return 2;
        }

        Record** Hash = nullptr;

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
            Record* This = *Hash;
            if (This)
            {
                for (Record* prev = nullptr; This; This = This->pNext)
                {
#ifndef TOOLSLIB_MAX_SPEED
                    if (!ItemsCount--) // integrity check failure
                    {
                        CMaaUnivHash2_THROW;
                    }
#endif
                    if (This->K == K)
                    {
                        if (!fOverwrite)
                        {
                            return 1; // Exists
                        }
                        switch (fOverwrite)
                        {
                        case 2:
                        {
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                            rec = m_pAllocator->NewEx(&m_pAllocator);
                            if (!rec)
                            {
                                return 2;
                            }
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                            gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
                            new_(rec, Record(K, D));
#else
                            rec = TL_NEW Record(K, D);
                            if (!rec)
                            {
                                return 2;
                            }
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                            gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
#endif
                            rec->pNext = This->pNext;
                            if (prev)
                            {
                                prev->pNext = rec;
                            }
                            else
                            {
                                *Hash = rec;
                            }
                            Record* r = rec;
                            rec = This;
                            ChangeIterators(This, r);
                        }
                        break;
                        case 200:
                            This->D.~Data();
                            new_(&This->D, Data(D)); // unsafe
                            break;
                        default:
                            This->D = D;
                        }
                        return 0; // ok
                    }

#ifndef TOOLSLIB_MAX_SPEED
                    if (This == This->pNext)
                    {
                        //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                        CMaaUnivHash2_THROW;
                    }
#endif
                    prev = This;
                }
            }
        }
        if (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            // expand table or first create on m_Err == -1
            const int Ret = NewMoveSwap(m_TableSize >= 2 ? 2 * m_TableSize : 2);
            if (Ret)
            {
                return Ret;
            }
            //return Add(K, D, fOverwrite);
            // key K is not exist
            Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
        }
#ifndef TOOLSLIB_SHARED_ALLOCATOR
        rec = m_pAllocator->NewEx(&m_pAllocator);
        if (!rec)
        {
            return 2;
        }
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
        new_(rec, Record(K, D));
#else
        rec = TL_NEW Record(K, D);
        if (!rec)
        {
            return 2;
        }
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
        gHashTablesAllocSize += (_qword)(sizeof(*rec));
#endif
#endif
        rec->pNext = *Hash;
        *Hash = rec;
        rec = nullptr;
        m_ItemsCount++;
        return 0;
    }

protected:
    // Adds  element. ( Copy element to table )
    // fOverwrite: 0 - don't overwrite, > 0 - overwrite: 1 - by Data::operator=, 2 - create TL_NEW and delete old Record (exception safe), 200 - Data::~Data() and new_(ptr, Data()) (unsafe)
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int AddRec(Record* rec, int fOverwrite)
    {
        if (!rec)
        {
            return 3;
        }

        InvalidateIterators();

#ifndef TOOLSLIB_SHARED_ALLOCATOR
        RR RR_rec(rec, m_pAllocator);
#else
        RR RR_rec(rec);
#endif

        const Key K = rec->K;

#if 0
        if (m_ItemsCount >= m_TableSize)
        {
            int Ret;
            if (!(Ret = NewMoveSwap(m_TableSize >= 2 ? 2 * m_TableSize : 2)))
            {
                Record* tmp = rec;
                rec = nullptr;
                Ret = AddRec(tmp, fOverwrite);
            }
            return Ret;
        }
#endif
        if ((m_Flags & eErr1))
        {
            return 2;
        }

        Record** Hash = nullptr;

        if (!(m_Flags & eOk0))
        {
#ifndef TOOLSLIB_MAX_SPEED
            size_t ItemsCount = m_ItemsCount + 1;
#endif
            Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
            Record* This = *Hash;
            if (This)
            {
                for (Record* prev = nullptr; This; This = This->pNext)
                {
#ifndef TOOLSLIB_MAX_SPEED
                    if (!ItemsCount--) // integrity check failure
                    {
                        CMaaUnivHash2_THROW;
                    }
#endif
                    if (This->K == K)
                    {
                        if (!fOverwrite)
                        {
                            return 1; // Exists
                        }
                        Record* r = rec;
                        r->pNext = This->pNext;
                        if (prev)
                        {
                            prev->pNext = r;
                        }
                        else
                        {
                            *Hash = r;
                        }
                        rec = This;
                        ChangeIterators(This, r);
                        return 0;
                    }

#ifndef TOOLSLIB_MAX_SPEED
                    if (This == This->pNext)
                    {
                        //test__((void *)&K, (void *)&D, (void *)rec, fOverwrite);
                        CMaaUnivHash2_THROW;
                    }
#endif
                    prev = This;
                }
            }
        }
        if (m_ItemsCount >= m_TableSize) // Hash is uninitialized here, table has nothing (eOk0)
        {
            int Ret;
            if ((Ret = NewMoveSwap(m_TableSize >= 2 ? 2 * m_TableSize : 2)))
            {
                return Ret;
            }
            //Record* tmp = rec;
            //rec = nullptr;
            //Ret = AddRec(tmp, fOverwrite);
            //return Ret;
            // key K is not exist
            Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
        }
        rec->pNext = *Hash;
        *Hash = rec;
        rec = nullptr;
        m_ItemsCount++;
        return 0;
    }

public:
    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    int AddOver(const Key K, const Data& D)
    {
        return Add(K, D, 1);
    }

    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory, 200 - Table failure in Data::Data() or other exception
    int Add(const Key K, const Data& D)
    {
        return Add(K, D, 0);
    }

    void insert(const Key K, const Data& D)
    {
        Add(K, D);
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find(const Key K, Data* D /*= nullptr*/) const noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        if ((m_Flags & eNothingMask))//!m_Hash)
        {
            return 1;
        }

        Record** Hash = m_Hash + ___HashFunction2(K) % m_HashSize;

        // searching for Key
        for (Record* This = *Hash; This; This = This->pNext)
        {
            if (This->K == K)
            {
                if (D)
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
    int Find(const Key K) const noexcept(noexcept(K == K))
    {
        if ((m_Flags & eNothingMask))//!m_Hash)
        {
            return 1;
        }

        Record** Hash = m_Hash + ___HashFunction2(K) % m_HashSize;

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
    bool contains(const Key K) const
    {
        return Find(K) == 0;
    }
    Data value(const Key K) const
    {
        Data d;
        Find(K, &d);
        return d;
    }
    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to rerurn deleted context of Data
    int Remove(const Key K, Data* D /*= nullptr*/)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(K == K) && noexcept(*D = *D))
        //#endif
    {
        if ((m_Flags & eNothingMask))
        {
            return 1;
        }

        Record** Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
        Record* Prev = nullptr;
        // searching for Key
        for (Record* This = *Hash; This; This = This->pNext)
        {
            if (This->K == K)
            {
                MoveIterators(This);
                if (D)
                {
                    *D = This->D;
                }
                if (Prev)
                {
                    Prev->pNext = This->pNext;
                }
                else
                {
                    *Hash = This->pNext;
                }
                m_ItemsCount--;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                gHashTablesAllocSize -= (_qword)(sizeof(*This));
#endif
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                This->~Record();
                m_pAllocator->DeleteEx(This, &m_pAllocator);
#else
                delete This;
#endif
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
                Squeeze();
#endif
                return 0; // 0: Key was found
            }
            Prev = This;
        }
        return 1; // 1: Key was not found
    }

    // Removes element.
    // Returns 0 if ok.
    int Remove(const Key K)
        //#ifndef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
        noexcept(noexcept(K == K))
        //#endif
    {
        if ((m_Flags & eNothingMask))
        {
            return 1;
        }

        Record** Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
        Record* Prev = nullptr;
        // searching for Key
        for (Record* This = *Hash; This; This = This->pNext)
        {
            if (This->K == K)
            {
                MoveIterators(This);
                if (Prev)
                {
                    Prev->pNext = This->pNext;
                }
                else
                {
                    *Hash = This->pNext;
                }
                m_ItemsCount--;
#ifdef TOOLSLIB_COLLECT_HASH_TABLES_ALLOC_SIZE
                gHashTablesAllocSize -= (_qword)(sizeof(*This));
#endif
#ifndef TOOLSLIB_SHARED_ALLOCATOR
                This->~Record();
                m_pAllocator->DeleteEx(This, &m_pAllocator);
#else
                delete This;
#endif
#ifdef TOOLSLIB_AUTO_SQUEEZE_HASH_TABLE
                Squeeze();
#endif
                return 0; // 0: Key was found
            }
            Prev = This;
        }
        return 1; // 1: Key was not found
    }

    int Squeeze() noexcept //noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        if (!IsNeedToSqueeze())
        {
            return -1;
        }
        const size_t NewSize = m_ItemsCount >= 1 ? 2 * m_ItemsCount : 2;
        if (NewSize >= m_TableSize)
        {
            return -1;
        }

        // squeeze table

        try
        {
            return NewMoveSwap(NewSize);
        }
        catch (...)
        {
            // No free elements, noexcept
        }
        return 2;
    }

    Data* operator [] (const Key K) noexcept(noexcept(K == K))
    {
        if (!(m_Flags & eNothingMask))
        {
            Record** Hash = m_Hash + ___HashFunction2(K) % m_HashSize;
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

protected:
    // returns GetItemCount()
    int EnumeratePtrs(Key** Keys, Data** Datas) const noexcept
    {
        Key** p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record* This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = &This->K;
                *Datas++ = &This->D;
            }
        }
        return (int)(p - Keys);
    }
    int EnumerateKeysPtrs(Key** Keys) const noexcept
    {
        Key** p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record* This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = &This->K;
            }
        }
        return (int)(p - Keys);
    }
    int EnumerateDatasPtrs(Data** Datas) const noexcept
    {
        Data** p = Datas;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record* This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = &This->D;
            }
        }
        return (int)(p - Datas);
    }

public:
    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumeratePtrs(int Size, Key** Keys, Data** Datas) const noexcept
    {
        if (Size >= 0)
        {
            //InvalidateIterators();
#ifdef TOOLSLIB_MAX_SPEED
            if (Size >= GetItemCount())
            {
                // super opt, unsafe
                return Keys && !Datas ? EnumerateKeysPtrs(Keys) : !Keys && Datas ? EnumerateDatasPtrs(Datas) : Keys && Datas ? EnumeratePtrs(Keys, Datas) : GetItemCount();
            }
#endif
            int j = 0;
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                // scaning all Keys
                for (Record* This = m_Hash[i]; This; This = This->pNext)
                {
                    if (j >= Size)
                    {
                        return -2;
                    }
                    if (Keys)
                    {
                        Keys[j] = &This->K;
                    }
                    if (Datas)
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
    int EnumerateKeys(Key* Keys) const noexcept(noexcept(*Keys = *Keys))
    {
        Key* p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record* This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = This->K;
            }
        }
        return (int)(p - Keys);
    }
    int EnumerateDatas(Data* Datas) const noexcept(noexcept(*Datas = *Datas))
    {
        Data* p = Datas;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record* This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = This->D;
            }
        }
        return (int)(p - Datas);
    }
    int EnumerateItems(Key* Keys, Data* Datas) const noexcept(noexcept(*Keys = *Keys) && noexcept(*Datas = *Datas))
    {
        Key* p = Keys;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scaning all Keys
            for (Record* This = m_Hash[i]; This; This = This->pNext)
            {
                *p++ = This->K;
                *Datas++ = This->D;
            }
        }
        return (int)(p - Keys);
    }

public:
    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumerateItems(int Size, Key* Keys, Data* Datas) const noexcept(noexcept(*Keys = *Keys) && noexcept(*Datas = *Datas))
    {
        if (Size >= 0)
        {
#ifdef TOOLSLIB_MAX_SPEED
            if (Size >= GetItemCount())
            {
                // super opt, unsafe
                return Keys && !Datas ? EnumerateKeys(Keys) : !Keys && Datas ? EnumerateDatas(Datas) : Keys && Datas ? EnumerateItems(Keys, Datas) : GetItemCount();
            }
#endif
            int j = 0;
            for (size_t i = 0; i < m_HashSize; ++i)
            {
                // scaning all Keys
                for (Record* This = m_Hash[i]; This; This = This->pNext)
                {
                    if (Keys)
                    {
                        if (j >= Size)
                        {
                            return -2;
                        }
                        *Keys = This->K;
                        Keys++;
                    }
                    if (Datas)
                    {
                        if (j >= Size)
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

    int GetItemCount() const noexcept
    {
        return (int)m_ItemsCount;
        //return EnumerateItems ( 0x7fffffff, nullptr, nullptr );
    }

    typedef void (*EnumerateProc)(const Key& K, Data& D, void* Param);
    typedef void (*EnumerateProcEx)(const Key& K, Data& D, CMaaUnivHash_New2<Key, Data, bStandartHash>& ht, void* Param);

    void EnumerateByProc(EnumerateProc Proc, void* Param)
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

    void EnumerateByProc(EnumerateProcEx Proc, void* Param)
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

    template<typename Fn> void EnumerateByProc(Fn Proc)
    {
        Record* Next;
        for (size_t i = 0; i < m_HashSize; ++i)
        {
            // scanning all Keys
            for (Record* This = m_Hash[i]; This; This = Next)
            {
                Next = This->pNext;
                Proc((const Key&)(This->K), This->D);
            }
        }
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
//#undef ___HashFunction2
//---------------------------------------------------------------------------
#endif
