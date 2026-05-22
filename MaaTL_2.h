//---------------------------------------------------------------------------
// template<class X, X val> class CMaaAutoInitObject;
// template<class X, typename Y, Y val> class CMaaAutoInitObjectEx;
// template<class X> class CMaaForcedInitObject; // constructor with one argument
//
// Limitations: Y is a simple type, val is a integral type value
//
// Exampes of using:
//
#if 0
class IntEx
{
public:
    int x;
    IntEx(int tmp)
        : x(tmp)
    {
    }
};

int GlobalInt = 'GLOB';

int main()
{
    CMaaAutoInitObject<bool, false> val123;
    CMaaAutoInitObjectEx<bool, bool, false> val;
    val = val123;
    val123 = val;
    CMaaAutoInitObjectEx<IntEx, int, 12345> valEx;
    CMaaAutoInitObjectEx<int, int, 12345> val2;
    CMaaAutoInitObject<int, 15> val234;
    CMaaAutoInitObject<int*, &GlobalInt/*nullptr*/> pInt;
    CMaaForcedInitObject<int*> pFInt;//(&val234);
    printf("pInt = %p, pFInt = %p, *pFInt = %d\n", (int*)pInt, (int*)pFInt, *pFInt);
    pInt = &val2;
    val2 = val234;
    printf("pInt = %p, *pInt = %d\n", (int*)pInt, *pInt);
    valEx = val234;
    ((IntEx&)valEx).x = 4567;
    printf("val2 = %d, %d, %d\n", (int)val2, (int)val2, (int)val2);
    val2 = 789;
    printf("val2 = %d\n", (int)val2);
    int x = val2;
    val2++;
    printf("x = %d, val2 = %d\n", x, val2);

    printf("\n");
    int* p = (int*)&val2;
    printf("p = %p\n", p);
    p = &val2;
    printf("p = %p\n", p);
    *p = 10;
    printf("val2 = %d\n", (int)val2);

    //long * l = val2;
    //printf("l = %p\n", l);
}
#endif

template<class X, X val> class CMaaAutoInitObject
{
    X x;
public:
    CMaaAutoInitObject()
        : x(val)
    {
    }
    CMaaAutoInitObject(const X& _x)
        : x(_x)
    {
    }
    /*
     typedef typename *X XX;
     XX operator->()
     {
          return *x;
     }
     const XX operator->() const
     {
          return *x;
     }
     */
    operator X& () noexcept
    {
        return x;
    }
    operator const X& () const noexcept
    {
        return x;
    }
    X* operator &() noexcept
    {
        return &x;
    }
    const X* operator &() const noexcept
    {
        return &x;
    }
    template<class Z> X& operator = (Z& z)
    {
        x = z;
        return x;
    }
    template<class Z> X& operator = (const Z& z)
    {
        x = z;
        return x;
    }
};

template<class X> class CMaaAutoInitNullPointer
{
    X x;
public:
    CMaaAutoInitNullPointer() noexcept
        : x(nullptr)
    {
    }
    CMaaAutoInitNullPointer(const X& _x)
        : x(_x)
    {
    }
    /*
     typedef typename *X XX;
     XX operator->()
     {
          return *x;
     }
     const XX operator->() const
     {
          return *x;
     }
     */
    operator X& () noexcept
    {
        return x;
    }
    operator const X& () const noexcept
    {
        return x;
    }
    X* operator &() noexcept
    {
        return &x;
    }
    const X* operator &() const noexcept
    {
        return &x;
    }
    template<class Z> operator Z& ()
    {
        return static_cast<Z&>(x);
    }
    template<class Z> operator const Z& () const
    {
        return static_cast<const Z&>(x);
    }
    template<class Z> X& operator = (Z& z)
    {
        x = z;
        return x;
    }
    template<class Z> X& operator = (const Z& z)
    {
        x = z;
        return x;
    }
};

template<class X, typename Y, Y val> class CMaaAutoInitObjectEx
{
    X x;
public:
    CMaaAutoInitObjectEx()
        : x(val)
    {
    }
    operator X& () noexcept
    {
        return x;
    }
    operator const X& () const noexcept
    {
        return x;
    }
    X* operator-> ()
    {
        return *x;
    }
    const X* operator-> () const
    {
        return *x;
    }
    X* operator &() noexcept
    {
        return &x;
    }
    const X* operator &() const noexcept
    {
        return &x;
    }
    template<class Z> operator Z& ()
    {
        return static_cast<Z&>(x);
    }
    template<class Z> operator const Z& () const
    {
        return static_cast<const Z&>(x);
    }
    template<class Z> X& operator = (Z& z)
    {
        x = z;
        return x;
    }
    template<class Z> X& operator = (const Z& z)
    {
        x = z;
        return x;
    }
};

template<class X> class CMaaForcedInitObject
{
    X x;
public:
    template<class Z> CMaaForcedInitObject(Z& val)
        : x(val)
    {
    }
    template<class Z> CMaaForcedInitObject(const Z& val)
        : x(val)
    {
    }
    X* operator-> ()
    {
        return *x;
    }
    const X* operator-> () const
    {
        return *x;
    }
    operator X& () noexcept
    {
        return x;
    }
    operator const X& () const noexcept
    {
        return x;
    }
    X* operator &() noexcept
    {
        return &x;
    }
    const X* operator &() const noexcept
    {
        return &x;
    }
    template<class Z> operator Z& ()
    {
        return static_cast<Z&>(x);
    }
    template<class Z> operator const Z& () const
    {
        return static_cast<const Z&>(x);
    }
    template<class Z> X& operator = (Z& z)
    {
        x = z;
        return x;
    }
    template<class Z> X& operator = (const Z& z)
    {
        x = z;
        return x;
    }
};

//---------------------------------------------------------------------------
template < class Key, class Data > class CMaaUnivHash_Fast
{
    struct Record
    {
        Record* pNext;
        Key  K;
        Data D;
    };
    int  m_TableSize;   // size of item table
    int  m_HashSize;    // size of hash table

    Record* m_FreeList,   // list of free items
        * m_Table,      // table of items
        ** m_Hash;      // hash table
    bool fOK;

protected:
    // hash function of key
    virtual _dword HashFunction(const Key& K) const noexcept
    {
        if constexpr (sizeof(Key) == sizeof(_dword))
        {
            return *(_dword*)&K;
        }
#ifdef TRY_HF8_OPT
        else if constexpr (sizeof(Key) == 2 * sizeof(_dword))
        {
            //return CMaaRol7dw(*(_dword*)&k) ^ *(1 + (_dword*)&K);
            return *((_dword*)&K) ^ *(1 + (_dword*)&K);
            //return *(1 + (_dword*)&K) ^ *(_dword*)&K;
        }
#endif
        else
        {
            unsigned char* p = (unsigned char*)&K;
            _dword x = 0;
#ifdef TRY_HF_OPT
            for (int i = (int)(sizeof(Key)) >> _dword_shift; i--; )
            {
                x = CMaaRol7dwMacro(x) ^ *(_dword*)p;
                p += sizeof(_dword);
            }
            for (int i = ((int)(sizeof(Key)) & _dword_mask); i--; )
            {
                x = CMaaRol7dwMacro(x) ^ *p++;
            }
#else
            for (int i = (int)sizeof(Key); i--; )
            {
                x = CMaaRol7dwMacro(x) ^ *p++;
            }
#endif
            return x;
        }
    }

public:

    class iterator : public CMaaDLink
    {
        friend class CMaaUnivHash_Fast<Key, Data>;
        CMaaUnivHash_Fast<Key, Data>* h;
        typename CMaaUnivHash_Fast<Key, Data>::Record* r;
        int index;
        bool bInc;
    public:
        iterator()
            : h(nullptr)
        {
            Init();
        }
        iterator(CMaaUnivHash_Fast<Key, Data>& ht)
            : h(nullptr)
        {
            Init();
            *this = ht.it();
        }
        iterator(const iterator& That)
            : h(nullptr)
        {
            Init();
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
                h = nullptr;
            }
        }
        iterator& operator=(const iterator& that)
        {
            iterator& That = (iterator&)that;
            if (this != &That)
            {
                Release();
                if ((h = That.h))
                {
                    r = That.r;
                    index = That.index;
                    bInc = That.bInc;
                    h->m_Iterators.AddAtFront(this);
                }
            }
            return *this;
        }
        iterator& operator=(CMaaUnivHash_Fast<Key, Data>& ht)
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
        const Key& ckey() noexcept
        {
            if (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Key key()
        {
            if (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Data& rdata() noexcept
        {
            if (!IsValid())
            {
                throw 1;
            }
            return r->D;
        }
        Data data()
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
        iterator operator++(int)
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        iterator operator++()
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
        bool Inc()
        {
            if (h && r)
            {
                r = r->pNext;
                if (!r)
                {
                    while (index < h->m_HashSize - 1 && !(r = h->m_Hash[++index]))
                    {
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
        int Remove(Data* d = nullptr) // remove key, 0 - ok, 1 - not found, 2 - iterator is not valid
        {
            if (IsValid())
            {
                return h->Remove(r->K, d);
            }
            return 2;
        }
    };
protected:
    CMaaDList<iterator> m_Iterators;
public:
    iterator it() noexcept
    {
        for (int i = 0; i < m_HashSize; i++)
        {
            if (m_Hash[i])
            {
                iterator it;
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
            p->h = nullptr;
        }
    }
    void MoveIterator(Record* r) noexcept
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

    virtual void Swap(CMaaUnivHash_Fast < Key, Data >& That) noexcept
    {
        CMaaSwap(m_TableSize, That.m_TableSize);
        CMaaSwap(m_HashSize, That.m_HashSize);
        CMaaSwap(m_FreeList, That.m_FreeList);
        CMaaSwap(m_Table, That.m_Table);
        CMaaSwap(m_Hash, That.m_Hash);
        CMaaSwap(fOK, That.fOK);
        CMaaDList<iterator> l1, l2;
        l1.MoveThatToTheFront(m_Iterators);
        l2.MoveThatToTheFront(That.m_Iterators);
        iterator* p;
        while ((p = l1.GetFromFront()))
        {
            p->h = &That;
            That.m_Iterators.AddAtBack(p);
        }
        while ((p = l2.GetFromFront()))
        {
            p->h = this;
            m_Iterators.AddAtBack(p);
        }
    }

    virtual CMaaUnivHash_Fast < Key, Data >* MakeNewTable(size_t Size, CMaaUnivHash_Fast<Key, Data>* ptr = nullptr, int Flags = 0)
    {
        return TL_NEW CMaaUnivHash_Fast < Key, Data >((int)Size, ptr, Flags);
    }

    CMaaUnivHash_Fast(int Size = 16, CMaaUnivHash_Fast* /*ptr*/ = nullptr, int Flags = 0)
    {
        if (Size < 2)
        {
            Size = 2;
        }
        m_HashSize = Size + Size / 10;
        m_TableSize = Size;
        m_Table = Size < SIZE_MAX / sizeof(Record) ? TL_NEW Record[m_TableSize] : nullptr;
        m_Hash = Size < SIZE_MAX / sizeof(void*) / 11 * 10 ? TL_NEW Record * [m_HashSize] : nullptr;
        if (m_Table && m_Hash)
        {
            fOK = true;
            // Initializing free list
#if 0
            Record* m = m_Table;
            for (int i = m_TableSize - 1; i--; )
            {
                m->pNext = m + 1;
                m++;
            }
            m->pNext = nullptr;
            m_FreeList = m_Table;
#else
            m_FreeList = nullptr;
            CMaaMakeAddFreeList(m_Table, m_TableSize, m_FreeList);
#endif
            Memzero(m_Hash, m_HashSize * sizeof(*m_Hash));
        }
        else
        {
            delete[] m_Hash;  m_Hash = nullptr;
            delete[] m_Table; m_Table = nullptr;
            fOK = false;
        }
    }

    bool IsOK() noexcept
    {
        return fOK;
    }

    virtual ~CMaaUnivHash_Fast() noexcept
    {
        InvalidateIterators();
        delete[] m_Hash;  m_Hash = nullptr;
        delete[] m_Table; m_Table = nullptr;
    }

    void Cleanup(int NewTableSize = 0)
    {
        InvalidateIterators();
        CMaaUnivHash_Fast < Key, Data >* NewTable = MakeNewTable(NewTableSize ? NewTableSize : m_TableSize, this, 0);
        if (NewTable)
        {
            Swap(*NewTable);
            delete NewTable;
        }
    }

    int GetMaxIndex() const noexcept
    {
        return m_HashSize;
    }

    // returns 0 for success
    int FindByIndex(int Index, Key* K = nullptr, Data* D = nullptr) const noexcept(noexcept(*K = *K) && noexcept(*D = *D))
    {
        InvalidateIterators();
        if (Index >= 0 && Index < m_HashSize)
        {
            if (m_Hash[Index])
            {
                if (K)
                {
                    *K = m_Hash[Index]->K;
                }
                if (D)
                {
                    *D = m_Hash[Index]->D;
                }
                return 0;
            }
        }
        return 1;
    }

    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key& K, const Data& D, int fOverwrite = 0)
    {
        InvalidateIterators();

        int Ret = 0; // 0: IsOk, 1,2: IsFail
        Record* Free = m_FreeList;
        Record* This;
        if (!Free)
        {
            CMaaUnivHash_Fast < Key, Data >* NewTable = MakeNewTable(2 * m_TableSize, this, 0);
            if (!NewTable || !NewTable->IsOK())
            {
                Ret = 2; // No free elements
            }
            else
            {
                This = m_Table;
                for (int i = m_TableSize; i; i--)
                {
                    NewTable->Add(This->K, This->D);
                    This++;
                }
                Ret = NewTable->Add(K, D, fOverwrite);
                Swap(*NewTable);
                delete NewTable;
            }
        }
        else
        {
            Record** Hash = m_Hash + HashFunction(K) % (_dword)m_HashSize;
            This = *Hash;
            if (!This)
            {
                // Creating TL_NEW node
                *Hash = Free;
                m_FreeList = Free->pNext;
                Free->pNext = nullptr;
                Free->K = K;
                Free->D = D;
            }
            else
            {
                // searching if Key alredy exists
                for (; This; This = This->pNext)
                {
                    if (This->K == K)
                    {
                        if (!fOverwrite)
                        {
                            Ret = 1; // Key alredy exists
                            break;
                        }
                        This->D = D;
                        return 0;
                    }
                }
                if (!Ret)
                {
                    m_FreeList = Free->pNext;
                    Free->pNext = *Hash;
                    *Hash = Free;
                    Free->K = K;
                    Free->D = D;
                }
            }
        }
        return Ret;

    }

    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    int AddOver(const Key& K, const Data& D)
    {
        return Add(K, D, 1);
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find(const Key& K, Data* D /*= nullptr*/) const noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        int Ret = 1; // 0: IsOk, 1: Key was not found

        if (!m_Table || !m_Hash)
        {
            static int aa = 0;
            aa++;
            return 1;
        }
        Record** Hash = m_Hash + HashFunction(K) % (_dword)m_HashSize;
        Record* This = *Hash;
        if (This)
        {
            // searching for Key
            for (; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    if (D)
                    {
                        *D = This->D;
                    }
                    Ret = 0; // Key was found
                    break;
                }
            }
        }
        return Ret;
    }

    // Finds element.
    // If ok: returns 0
    int Find(const Key& K) const noexcept(noexcept(K == K))
    {
        int Ret = 1; // 0: IsOk, 1: Key was not found

        if (!m_Table || !m_Hash)
        {
            static int aa = 0;
            aa++;
            return 1;
        }
        Record** Hash = m_Hash + HashFunction(K) % (_dword)m_HashSize;
        Record* This = *Hash;
        if (This)
        {
            // searching for Key
            for (; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    Ret = 0; // Key was found
                    break;
                }
            }
        }
        return Ret;
    }

    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to rerurn deleted context of Data
    int Remove(const Key& K, Data* D /*= nullptr*/) noexcept(noexcept(K == K) && noexcept(*D = *D))
    {
        int Ret = 1; // 0: IsOk, 1: Key was not found

        Record** Hash = m_Hash + HashFunction(K) % (_dword)m_HashSize;
        Record* This = *Hash, * Prev = nullptr;
        if (This)
        {
            // searching for Key
            for (; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    MoveIterator(This);
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
                    This->pNext = m_FreeList;
                    m_FreeList = This;
                    Ret = 0; // Key was found
                    break;
                }
                Prev = This;
            }
        }
        return Ret;
    }

    // Removes element.
    // Returns 0 if ok.
    int Remove(const Key& K) noexcept(noexcept(K == K))
    {
        int Ret = 1; // 0: IsOk, 1: Key was not found

        Record** Hash = m_Hash + HashFunction(K) % (_dword)m_HashSize;
        Record* This = *Hash, * Prev = nullptr;
        if (This)
        {
            // searching for Key
            for (; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    MoveIterator(This);
                    if (Prev)
                    {
                        Prev->pNext = This->pNext;
                    }
                    else
                    {
                        *Hash = This->pNext;
                    }
                    This->pNext = m_FreeList;
                    m_FreeList = This;
                    Ret = 0; // Key was found
                    break;
                }
                Prev = This;
            }
        }
        return Ret;
    }

    Data* operator [] (const Key& K) noexcept
    {
        Record** Hash = m_Hash + HashFunction(K) % (_dword)m_HashSize;
        Record* This = *Hash;
        if (This)
        {
            // searching for Key
            for (; This; This = This->pNext)
            {
                if (This->K == K)
                {
                    return &This->D;
                }
            }
        }
        return nullptr;
    }

    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumeratePtrs(int Size, Key** Keys, Data** Datas) const noexcept
    {
        if (Size >= 0)
        {
            InvalidateIterators();

            int j = 0;
            for (int i = 0; i < m_HashSize; i++)
            {
                Record* This = m_Hash[i];
                if (This)
                {
                    // scaning all Keys
                    for (; This; This = This->pNext)
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
            }
            return j;
        }
        return -1;
    }

    // returns -1: invalid parameter Size, -2: Size is too small (too small arrays Keys/Datas )
    int EnumerateItems(int Size, Key* Keys, Data* Datas) const noexcept(noexcept(*Keys == *Keys) && noexcept(*Datas = *Datas))
    {
        if (Size >= 0)
        {
            int j = 0;
            for (int i = 0; i < m_HashSize; i++)
            {
                Record* This = m_Hash[i];
                if (This)
                {
                    // scaning all Keys
                    for (; This; This = This->pNext)
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
            }
            return j;
        }
        return -1;
    }

    int GetItemCount() const noexcept
    {
        return EnumeratePtrs(0x7fffffff, nullptr, nullptr);
    }

    typedef void (*EnumerateProc)(Key& K, Data& D, void* Param);
    typedef void (*EnumerateProcEx)(Key& K, Data& D, CMaaUnivHash_Fast < Key, Data >& ht, void* Param);

    void EnumerateByProc(EnumerateProc Proc, void* Param = nullptr)
    {
        for (int i = 0; i < m_HashSize; i++)
        {
            Record* This = m_Hash[i];
            if (This)
            {
                // scanning all Keys
                Record* Next;
                for (; This; This = Next)
                {
                    Next = This->pNext;
                    Proc(This->K, This->D, Param);
                }
            }
        }
    }

    void EnumerateByProc(EnumerateProcEx Proc, void* Param = nullptr)
    {
        for (int i = 0; i < m_HashSize; i++)
        {
            Record* This = m_Hash[i];
            if (This)
            {
                // scanning all Keys
                Record* Next;
                for (; This; This = Next)
                {
                    Next = This->pNext;
                    Proc(This->K, This->D, *this, Param);
                }
            }
        }
    }
};
//---------------------------------------------------------------------------
template < class Key, class Data > class CMaaUnivHash_Fast_KeyIsClass : public CMaaUnivHash_Fast < Key, Data >
{
    _dword HashFunction(const Key& K) const noexcept override
    {
        return K.Hash();
    }
public:
    CMaaUnivHash_Fast_KeyIsClass(int Size = 16, CMaaUnivHash_Fast<Key, Data>* ptr = nullptr, int Flags = 0)
        : CMaaUnivHash_Fast < Key, Data >(Size, ptr, Flags)
    {
    }
    CMaaUnivHash_Fast < Key, Data >* MakeNewTable(size_t Size, CMaaUnivHash_Fast<Key, Data>* ptr = nullptr, int Flags = 0) override
    {
        return TL_NEW CMaaUnivHash_Fast_KeyIsClass < Key, Data >((int)Size, ptr, Flags);
    }
};
//---------------------------------------------------------------------------
template < class Key, class Data > class CMaaUnivHash_FastClassPtr : public CMaaUnivHash_Fast < Key, Data >
{
public:
    CMaaUnivHash_FastClassPtr(int Size = 16, CMaaUnivHash_Fast<Key, Data>* ptr = nullptr, int Flags = 0)
        : CMaaUnivHash_Fast < Key, Data >(Size, ptr, Flags)
    {
    }

    static void ObjectDeleter(Key& /*K*/, Data& D, CMaaUnivHash_Fast < Key, Data >& /*ht*/, void* /*Param*/) noexcept
    {
        delete D;
    }

    ~CMaaUnivHash_FastClassPtr()
    {
        CMaaUnivHash_Fast<Key, Data>::InvalidateIterators();
        EnumerateByProc(ObjectDeleter);
    }
};
