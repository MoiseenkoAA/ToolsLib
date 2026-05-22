
// Old code. Reary used.

//==============================================================================
// Created:     11.03.98
//
// Author:      Andrey A. Moisseyenko
//
// Description: Smart pointer realization
//
//==============================================================================
//
// Contains templates: CMaaPtr, CMaaClassPtr
//

/*
template < class T > class CMaaPtr
{
protected:
     T    * m_Ptr;

public:

     // This constructor creates MaxIndex numbers of elements of class T.
     // Under Windows makes exeption if no free memory available.
     CMaaPtr ( size_t MaxIndex, int * SetErrorFlag = nullptr )
     :    m_Ptr ( nullptr )
     {
          // Checking total size of array to be less than 64K (Dos) of 4G (Win32)
          if   ( ( size_t ) sizeof ( T ) * MaxIndex >= MaxIndex )
          {
               m_Ptr = TL_NEW T [ MaxIndex ];
          }
          if   ( SetErrorFlag && ! m_Ptr )
          {
               * SetErrorFlag |= 1;
          }
          #ifdef _WIN32
          if   ( ! m_Ptr )
          {
               //throw ( XTOOError ( MemoryError ) );
               throw "MemoryError";
          }
          #endif
     }

     // Deleting elements if class T
     ~CMaaPtr ()
     {
          delete [] m_Ptr;
     }

     // Is not enought memory
     _bool IsValid () const
     {
          return m_Ptr == nullptr;
     }

     // приведение к типу 'указатель на T'
     operator T * ()
     {
          return m_Ptr;
     }

     T & operator * () const
     {
          return * m_Ptr;
     }
     // You can access each element using indexing mode
     T & operator [] ( int Index )
     {
          return m_Ptr [ Index ];
     }
};
*/
//---------------------------------------------------------------------------
template < class T > class CMaaClassPtr
{
protected:
    T   * m_Ptr;

public:
    // This constructor set element of class T
    CMaaClassPtr(T * ptr) noexcept
    {
        m_Ptr = ptr;
    }
    // This constructor creates element of class T
    // Under Windows makes exeption if no free memory available
    CMaaClassPtr()
    {
        Init();
    }
    // This constructor creates element of class T
    // Set flag |= 1; if no free memory available && SetErrorFlag
    // throw "MemoryError" if no free memory available && !SetErrorFlag
    CMaaClassPtr(int &Flag) noexcept
    {
        Init(Flag);
    }
    CMaaClassPtr(const CMaaClassPtr<T>& That) noexcept = delete;
    CMaaClassPtr(CMaaClassPtr<T>&& That) noexcept
    {
        if (&That != this)
        {
            m_Ptr = That.m_Ptr;
            That.m_Ptr = nullptr;
        }
    }
    ~CMaaClassPtr()
    {
        delete m_Ptr;
    }
    // Is enought memory
    bool IsValid() const noexcept
    {
        return m_Ptr;
    }
    CMaaClassPtr<T>& operator=(const CMaaClassPtr<T>& That) noexcept = delete;
    CMaaClassPtr<T>& operator=(CMaaClassPtr<T>&& That) noexcept
    {
        if (&That != this)
        {
            delete m_Ptr;
            m_Ptr = That.m_Ptr;
            That.m_Ptr = nullptr;
        }
        return *this;
    }
    CMaaClassPtr<T>& operator=(T * ptr) noexcept
    {
        delete m_Ptr;
        m_Ptr = ptr;
        return *this;
    }
    void Swap(CMaaClassPtr<T>& That) noexcept
    {
        CMaaSwap(m_Ptr, That.m_Ptr);
    }
    T* Extract() noexcept
    {
        T* p = m_Ptr;
        m_Ptr = nullptr;
        return p;
    }
    void Reset() noexcept
    {
        m_Ptr = nullptr;
    }
    // type cast to 'pointer to T'
    operator T * () noexcept
    {
        return m_Ptr;
    }
    T * operator -> () const noexcept
    {
        return m_Ptr;
    }
    T & operator * () const noexcept
    {
        return * m_Ptr;
    }
    void Init(int &Flag, T * p) noexcept
    {
        delete m_Ptr;
        m_Ptr = p;
        if  (!m_Ptr)
        {
            Flag |= 1;
        }
    }
    void Init(int* pFlag, T* p) noexcept
    {
        delete m_Ptr;
        m_Ptr = p;
        if (!m_Ptr && pFlag)
        {
            *pFlag |= 1;
        }
    }
protected:
    void Init()
    {
        m_Ptr = TL_NEW T;
        if (!m_Ptr)
        {
            //throw ( XTOOError ( MemoryError ) );
            throw "MemoryError";
        }
    }
    void Init(int &Flag) noexcept
    {
        try
        {
            m_Ptr = TL_NEW T;
        }
        catch (...)
        {
            m_Ptr = nullptr;
        }
        if (!m_Ptr)
        {
            Flag |= 1;
        }
    }
};
//---------------------------------------------------------------------------
template < class T > class CMaaRefPtr0
{
protected:
    T* m_Ptr;

public:
    // This constructor set element of class T
    CMaaRefPtr0(T* ptr) noexcept
    {
        if ((m_Ptr = ptr))
        {
            ptr->AddRef();
        }
    }
    // -- This constructor creates element of class T
    // -- Under Windows makes exeption if no free memory available
    // init by nullptr, not allocates ptr by default
    constexpr CMaaRefPtr0() noexcept
    {
        m_Ptr = nullptr;
        //Init();
    }
    // This constructor creates element of class T
    // Set flag |= 1; if no free memory available && SetErrorFlag
    // throw "MemoryError" if no free memory available && !SetErrorFlag
    CMaaRefPtr0(int& Flag) noexcept
    {
        Init(Flag);
    }
    CMaaRefPtr0(const CMaaRefPtr0<T>& That) noexcept
    {
        if (That.m_Ptr)
        {
            That.m_Ptr->AddRef();
        }
        m_Ptr = That.m_Ptr;
    }
    CMaaRefPtr0(CMaaRefPtr0<T>&& That) noexcept
    {
        if (That.m_Ptr != m_Ptr)
        {
            m_Ptr = That.m_Ptr;
            That.m_Ptr = nullptr;
        }
    }
    constexpr ~CMaaRefPtr0()
    {
        if (m_Ptr)
        {
            m_Ptr->Release();
        }
    }
    void Reset() noexcept
    {
        if (m_Ptr)
        {
            m_Ptr->Release();
            m_Ptr = nullptr;
        }
    }
    // Is enought memory
    bool IsValid() const noexcept
    {
        return m_Ptr;
    }
    CMaaRefPtr0<T>& operator=(const CMaaRefPtr0<T>& That) noexcept
    {
        if (m_Ptr != That.m_Ptr)
        {
            if (That.m_Ptr)
            {
                That.m_Ptr->AddRef();
            }
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            m_Ptr = That.m_Ptr;
        }
        return *this;
    }
    CMaaRefPtr0<T>& operator=(CMaaRefPtr0<T>&& That) noexcept
    {
        if (m_Ptr != That.m_Ptr)
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            m_Ptr = That.m_Ptr;
            That.m_Ptr = nullptr;
        }
        return *this;
    }
    CMaaRefPtr0<T>& operator=(T* ptr) noexcept
    {
        if (m_Ptr != ptr)
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            if ((m_Ptr = ptr))
            {
                ptr->AddRef();
            }
        }
        return *this;
    }
    /*
    void SetPtr0(T* ptr, bool bInc) noexcept
    {
        if (m_Ptr != ptr)
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            if ((m_Ptr = ptr) && bInc)
            {
                ptr->AddRef();
            }
        }
        //return *this;
    }
    */
    /*
    CMaaRefPtr0<T>& operator=(T* ptr) noexcept
    {
        if (m_Ptr != ptr)
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            m_Ptr = ptr;
        }
        return *this;
    }
    */
    void Swap(CMaaRefPtr0<T>& That) noexcept
    {
        CMaaSwap(m_Ptr, That.m_Ptr);
    }
    T* Extract() noexcept
    {
        T* p = m_Ptr;
        m_Ptr = nullptr;
        return p;
    }
    // type cast to 'pointer to T'
    operator T* () noexcept
    {
        return m_Ptr;
    }
    T* operator -> () const noexcept
    {
        return m_Ptr;
    }
    T& operator * () const noexcept
    {
        return *m_Ptr;
    }
    void Init(int& Flag, T* p) noexcept
    {
        if (m_Ptr != p)
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            if ((m_Ptr = p))
            {
                p->AddRef();
            }
        }
        if (!m_Ptr)
        {
            Flag |= 1;
        }
    }
    void Init(int* pFlag, T* p) noexcept
    {
        if (m_Ptr != p)
        {
            if (m_Ptr)
            {
                m_Ptr->Release();
            }
            if ((m_Ptr = p))
            {
                p->AddRef();
            }
        }
        if (!m_Ptr && pFlag)
        {
            *pFlag |= 1;
        }
    }
protected:
    void Init()
    {
        m_Ptr = TL_NEW T;
        if (!m_Ptr)
        {
            //throw ( XTOOError ( MemoryError ) );
            throw "MemoryError";
        }
    }
    void Init(int& Flag) noexcept
    {
        try
        {
            m_Ptr = TL_NEW T;
        }
        catch (...)
        {
            m_Ptr = nullptr;
        }
        if (!m_Ptr)
        {
            Flag |= 1;
        }
    }
};
//---------------------------------------------------------------------------
template < class T > class CMaaRefPtr
{
protected:
    T* m_Ptr;

public:
    // This constructor set element of class T
    CMaaRefPtr(T* ptr) noexcept
    {
        m_Ptr = ptr;
    }
    // This constructor creates element of class T
    // Under Windows makes exeption if no free memory available
    CMaaRefPtr()
    {
        Init();
    }
    // This constructor creates element of class T
    // Set flag |= 1; if no free memory available && SetErrorFlag
    // throw "MemoryError" if no free memory available && !SetErrorFlag
    CMaaRefPtr(int& Flag) noexcept
    {
        Init(Flag);
    }
    CMaaRefPtr(const CMaaRefPtr<T>& That) noexcept
    {
        That.m_Ptr&& That.m_Ptr->AddRef();
        m_Ptr = That.m_Ptr;
    }
    CMaaRefPtr(CMaaRefPtr<T>&& That) noexcept
    {
        if (That.m_Ptr != m_Ptr)
        {
            m_Ptr = That.m_Ptr;
            That.m_Ptr = nullptr;
        }
    }
    ~CMaaRefPtr()
    {
        if (m_Ptr && !m_Ptr->Release())
        {
            delete m_Ptr;
        }
    }
    // Is enought memory
    bool IsValid() const noexcept
    {
        return m_Ptr;
    }
    CMaaRefPtr<T>& operator=(const CMaaRefPtr<T>& That) noexcept
    {
        if (m_Ptr != That.m_Ptr)
        {
            That.m_Ptr&& That.m_Ptr->AddRef();
            if (m_Ptr && !m_Ptr->Release())
            {
                delete m_Ptr;
            }
            m_Ptr = That.m_Ptr;
        }
        return *this;
    }
    CMaaRefPtr<T>& operator=(CMaaRefPtr<T>&& That) noexcept
    {
        if (m_Ptr != That.m_Ptr)
        {
            if (m_Ptr && !m_Ptr->Release())
            {
                delete m_Ptr;
            }
            m_Ptr = That.m_Ptr;
            That.m_Ptr = nullptr;
        }
        return *this;
    }
    CMaaRefPtr<T>& operator=(T* ptr) noexcept
    {
        if (m_Ptr != ptr)
        {
            if (m_Ptr && !m_Ptr->Release())
            {
                delete m_Ptr;
            }
            m_Ptr = ptr;
        }
        return *this;
    }
    void Swap(CMaaRefPtr<T>& That) noexcept
    {
        CMaaSwap(m_Ptr, That.m_Ptr);
    }
    T* Extract() noexcept
    {
        T* p = m_Ptr;
        m_Ptr = nullptr;
        return p;
    }
    // type cast to 'pointer to T'
    operator T* () noexcept
    {
        return m_Ptr;
    }
    T* operator -> () const noexcept
    {
        return m_Ptr;
    }
    T& operator * () const noexcept
    {
        return *m_Ptr;
    }
    void Init(int& Flag, T* p) noexcept
    {
        if (m_Ptr != p)
        {
            if (m_Ptr && !m_Ptr->Release())
            {
                delete m_Ptr;
            }
            m_Ptr = p;
        }
        if (!m_Ptr)
        {
            Flag |= 1;
        }
    }
    void Init(int* pFlag, T* p) noexcept
    {
        if (m_Ptr != p)
        {
            if (m_Ptr && !m_Ptr->Release())
            {
                delete m_Ptr;
            }
            m_Ptr = p;
        }
        if (!m_Ptr && pFlag)
        {
            *pFlag |= 1;
        }
    }
protected:
    void Init()
    {
        m_Ptr = TL_NEW T;
        if (!m_Ptr)
        {
            //throw ( XTOOError ( MemoryError ) );
            throw "MemoryError";
        }
    }
    void Init(int& Flag) noexcept
    {
        try
        {
            m_Ptr = TL_NEW T;
        }
        catch (...)
        {
            m_Ptr = nullptr;
        }
        if (!m_Ptr)
        {
            Flag |= 1;
        }
    }
};
/*
struct CMaaRefPtrTest : public CMaaManualReference
{
    int x;
};
int CMaaRefPtr_test()
{
    CMaaRefPtr<CMaaRefPtrTest> r1;
    r1->x = 1;
    __utf8_printf("%d\n", r1->x);
    r1 = new CMaaRefPtrTest;
    if (r1)
    {
        r1->x = 2;
        __utf8_printf("%d\n", r1->x);
    }
    return 0;
}
*/
//---------------------------------------------------------------------------
#if 0
// CMaaClassPtr<> samples
template<int N> class CMaaBuffer1
{
public:
    char ptr[N];
    ADD_ALLOCATOR(CMaaBuffer1<N>)
};
DEF_ALLOCATOR_(CMaaBuffer1<64 * 1024 + 4000>)

int func_buffer_test1() noexcept
{
    CMaaBuffer1<64 * 1024 + 4000>::s_pAllocator->PreAllocate(15);

    int flag = 0;
    CMaaClassPtr< CMaaBuffer1<64 * 1024 + 4000> > B1(flag), B2(flag);
    if (flag)
    {
        return 1; // new error
    }
    // or:
    if (!B1 || !B2)
    {
        return 1; // new error
    }
    char* ptr1 = B1->ptr;
    char* ptr2 = B2->ptr;
    __utf8_printf("ptr1 = %p, not null here\n", ptr1);
    __utf8_printf("ptr2 = %p, not null here\n", ptr2);

    B1 = std::move(B2);
    ptr1 = B1->ptr;
    ptr2 = B2->ptr; // nullptr
    __utf8_printf("ptr1 = %p, not null here\n", ptr1);
    __utf8_printf("ptr2 = %p, nullptr here\n", ptr2);

    CMaaClassPtr< CMaaBuffer1<64 * 1024 + 4000> > B3 = std::move(B1);
    ptr1 = B1->ptr;
    char* ptr3 = B3->ptr;
    __utf8_printf("ptr1 = %p, nullptr here\n", ptr1);
    __utf8_printf("ptr3 = %p, not null here\n", ptr3);

    return 0;
}

int func_buffer_test2()
{
    CMaaBuffer1<64 * 1024 + 4000>::s_pAllocator->PreAllocate(15);

    CMaaClassPtr< CMaaBuffer1<64 * 1024 + 4000> > B1, B2; // throw "MemoryError"; on new error
    // never:
    if (!B1 || !B2)
    {
        return 1;
    }
    char* ptr1 = B1->ptr;
    char* ptr2 = B2->ptr;
    __utf8_printf("ptr1 = %p, not null here\n", ptr1);
    __utf8_printf("ptr2 = %p, not null here\n", ptr2);
    return 0;
}

int func_buffer_tests() noexcept
{
    func_buffer_test1();

    try
    {
        func_buffer_test2();
    }
    catch (const char* msg)
    {
        __utf8_printf("catch(%s)\n", msg);
    }
    return 0;
}
#endif
//---------------------------------------------------------------------------

/*
//----------------------------------
// Example of using CMaaPtr
//----------------------------------

// WINDOWS:
void main ()
{
     try
     {
          int len = 256;
          CMaaPtr < char > String ( len );
          CMaaPtr < int > a ( 10 );
          sprintf ( String, "Copying to string with length = %d", len );

          a [ 1 ] = 5;
          ( ( int * ) a ) [ 1 ] = 1;
          * ( a + 1 ) = 2;
          * a = 4;

          // Next line makes exeption
          CMaaPtr < int > TooManyMemoryAsking ( 0x80000000 );
          CMaaPtr < int > TooManyMemoryAsking ( 0x10000000 );
     }
     catch ( ... )
     {
          // Handling exeptions
     }
}

// DOS:
void main ()
{
     int len = 256;
     int IsError = _false;
     CMaaPtr < char > String ( len, & IsError );
     CMaaPtr < int > a ( 10, & IsError );
     if   ( IsError )
     {
          printf ( "Not enought free memory\n" );
     }
     else
     {
          sprintf ( String, "Copying to string with length = %d", len );

          a [ 1 ] = 5;
          ( ( int * ) a ) [ 1 ] = 1;
          * ( a + 1 ) = 2;
          * a = 4;
     }
     // Next line makes exeption
     CMaaPtr < int > TooManyMemoryAsking ( 32769 );
     if   ( ! TooManyMemoryAsking.IsValid () ) // or if ( ! TooManyMemoryAsking )
     {
          printf ( "Not enought free memory\n" );
     }
     else
     {
          // All Ok.
     }
}

//-------------------------
// The another example
//-------------------------
int TestExeptionSize = 0x1000;

class B
{
     struct DifficultCalculationsForB
     {
          int m_Size;
          DifficultCalculationsForB ( int x );
     };
     DifficultCalculationsForB m_RunTimeCalculationsOfSize;

     int m_DosErrFlag;
     CMaaPtr < char > m_String;
     CMaaPtr < int  > m_Ints;
     CMaaPtr < char > m_StrWithRunTimeSize;
public:
     B ()
     :    m_DosErrFlag ( 0 ),
          m_RunTimeCalculationsOfSize ( 5 ),
          m_String ( 256, & m_DosErrFlag ),
          m_Ints ( TestExeptionSize, & m_DosErrFlag ),
          m_StrWithRunTimeSize ( m_RunTimeCalculationsOfSize.m_Size, & m_DosErrFlag )
     {
          if   ( m_DosErrFlag )
          {
               // We can be here only under DOS
               // Error. Not enought free memory.
               return;
          }
          * m_String = 0;
     }
};

B::DifficultCalculationsForB::DifficultCalculationsForB ( int x )
{
     m_Size = x * x + x;
}

void main ()
{
     CMaaPtr < int > a ( 10 );

     B * b = nullptr;

     #ifdef _WIN32
          try
     #endif
     {
          B bb;
          TestExeptionSize = 0x10000000;
          b = TL_NEW B;
          delete b;
     }
     #ifdef _WIN32
          catch ( ... )
          {
               int aa = a [ 1 ];
          }
     #endif
     a [ 1 ] = 5;
     ((int *)a) [ 1 ] = 1;
     * ( a + 1 ) = 2;
     * a = 4;
}
*/
