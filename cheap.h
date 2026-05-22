
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/cheap.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform templates for working heap
 * sutable for using them in sockets' timers.
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

//------------------------------------------------
// Min heap's template class (priority queue)
//------------------------------------------------
// m[Parent(i)] <= m[i]
//------------------------------------------------
template <class Key, class Data> class CMaaHeap
{
public:
    friend int main2(int argn, char* args[]);
    friend class CMaaHaffman;

    class Rec
    {
    public:
        Key k;
        int idx;
        Data d;

        Rec(const Key &_k, const Data &_d, int _idx) noexcept(noexcept(Key(_k)) && noexcept(Data(_d)))
        :   k(_k),
            idx(_idx),
            d(_d)
        {
        }
        //ADD_UL_ALLOCATOR(Rec)
        ADD_ALLOCATOR(Rec)
#ifndef TOOLSLIB_DONT_USE_ALLOCATORS
//#define DEF_ALLOCATOR_CMaaHeap(A,B) template<> CMaaFixedAllocator< CMaaHeap< A, B >::Rec, 1 >* CMaaHeap< A, B >::Rec::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaHeap< A, B >::Rec > CMaaHeap< A, B >::Rec::s_AllocatorCreator(&CMaaHeap< A, B >::Rec::s_pAllocator);
//#define DEF_ALLOCATOR_CMaaHeap(A,B) template<> CMaaFixedAllocator< CMaaHeap< A, B >::Rec, -1 >* CMaaHeap< A, B >::Rec::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaHeap< A, B >::Rec > CMaaHeap< A, B >::Rec::s_AllocatorCreator(&CMaaHeap< A, B >::Rec::s_pAllocator);

#define DEF_ALLOCATOR_CMaaHeap(A,B) template<> CMaaFixedAllocator< CMaaHeap< A, B >::Rec, 1 > & CMaaHeap< A, B >::Rec::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaHeap< A, B >::Rec, 1 > a;  return (CMaaFixedAllocator< CMaaHeap< A, B >::Rec, 1 >  &)a; }
//#define DEF_ALLOCATOR_CMaaHeap(A,B) template<> CMaaFixedAllocator< CMaaHeap< A, B >::Rec, -1 > & CMaaHeap< A, B >::Rec::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaHeap< A, B >::Rec, -1 > a;  return (CMaaFixedAllocator< CMaaHeap< A, B >::Rec, -1 >  &)a; }
#else
#define DEF_ALLOCATOR_CMaaHeap(A,B)
#endif

    };
public:
    typedef Rec* Handle;

protected:
    int m_Size;
    CMaaPtr_<Rec *, -1> m;

    CMaaHeap(const CMaaHeap& That) = delete;
    CMaaHeap & operator=(const CMaaHeap& That) = delete;
public:
    CMaaHeap(int sz = 0) noexcept
    :   m_Size(0),
        m(sz)
    {
        if (!m.IsValid())
        {
            m.Empty();
        }
    }

    ~CMaaHeap()
    {
        for (int i = m_Size; i--; )
        {
            if (m[i])
            {
                delete m[i];
            }
        }
    }

    int GetCount() const noexcept
    {
        return m_Size;
    }

    static constexpr int Parent(int i) noexcept
    {
        return (i - 1) / 2;
    }
    static constexpr int Left(int i) noexcept
    {
        return 2 * i + 1;
    }
    static constexpr int Right(int i) noexcept
    {
        return 2 * i + 2;
    }
    void Swap(int a, int b) noexcept
    {
        Rec * tmp = m[a];
        m[a] = m[b];
        m[b] = tmp;
        m[a]->idx = a;
        m[b]->idx = b;
    }
protected:
    // m[Parent(i)] <= m[i]
    // !(m[i] < m[Parent(i)])
    void MoveDown_(int i) noexcept(noexcept(Key{} < Key{})) // && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        Rec* tmp = m[i];
        while (1)
        {
            int l = Left(i);
            if (l >= m_Size)
            {
                break;
            }
            const int r = l + 1;
            // find minimal child
            l = r < m_Size && m[r]->k < m[l]->k ? r : l;
            /*
            if (tmp->k <= m[l]->k)
            {
                break;
            }
            */
            if (!(m[l]->k < tmp->k))
            {
                break;
            }
            //Swap(i, l);
            m[i] = m[l];
            m[i]->idx = i;
            //// MoveDown_(l); //Heapify(l);
            i = l;
        }
        m[i] = tmp;
        m[i]->idx = i;
    }
public:
    // void Heapify(int i) noexcept(noexcept(Key{} < Key{}))
    void MoveDown(int i) noexcept(noexcept(Key{} < Key{})) // && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if (i >= 0 && i < m_Size)
        {
            MoveDown_(i);
        }
    }
    void MoveUp(int i) noexcept(noexcept(Key{} < Key{}))
    {
        Rec* tmp = m[i];
        int p;
        while (i > 0 && tmp->k < m[(p = Parent(i))]->k)
        {
            //Swap(i, p);
            m[i] = m[p];
            m[i]->idx = i;
            i = p;
        }
        m[i] = tmp;
        m[i]->idx = i;
    }
    void BuildHeap() noexcept(noexcept(MoveDown_(0)))
    {
        if (m_Size > 1)
        {
            for (int i = Parent(m_Size - 1); i >= 0; i--)
            {
                MoveDown_(i);
            }
        }
    }
    bool Look(Key* k = nullptr, Data* d = nullptr) const noexcept(noexcept(*k = *k) && noexcept(*d = *d)) // look min
    {
        if (m_Size <= 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[0]->k;
        }
        if (d)
        {
            *d = m[0]->d;
        }
        return true;
    }
    bool Look(Handle ptr, Key* k, Data* d) const noexcept(noexcept(*k = *k) && noexcept(*d = *d)) // look handle
    {
        if (ptr)
        {
            if (k)
            {
                *k = ptr->k;
            }
            if (d)
            {
                *d = ptr->d;
            }
            return true;
        }
        return false;
    }
    bool Del(Key* k = nullptr, Data* d = nullptr) noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0] = m[0]) && noexcept(MoveDown(0))) // del & return min
    {
        if (m_Size <= 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[0]->k;
        }
        if (d)
        {
            *d = m[0]->d;
        }
        Swap(0, --m_Size);
        delete m[m_Size];
        //m[m_Size] = nullptr;
        MoveDown(0);
        return true;
    }
    bool Del(const Handle ptr, Key* k = nullptr, Data* d = nullptr) noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0] = m[0]) && noexcept(MoveDown(0))) // del & return by handle
    {
        if (m_Size <= 0)
        {
            return false;
        }
        const int i = ptr->idx;
        if (k)
        {
            *k = m[i]->k;
        }
        if (d)
        {
            *d = m[i]->d;
        }
        Swap(i, --m_Size);
        delete m[m_Size];
        MoveDown(i);
        return true;
    }
    int SetKeyByHandle(const Handle ptr, const Key &k) noexcept(noexcept(k < k) && noexcept(k == k) && noexcept((Key&)k = k))
    {
        return SetKey(ptr->idx, k, false);
    }
    int SetKey(int i, const Key &k, bool bInsert = false) noexcept(noexcept(k < k) && noexcept(k == k) && noexcept((Key&)k = k)) // && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if (i < 0 || i >= m_Size)
        {
            return 1; // out of range
        }
        if (m[i]->k < k)
        {
            // Increase: Move down
            m[i]->k = k;
            MoveDown_(i); // Heapify(i);
        }
        else if (bInsert || !(k == m[i]->k))
        {
            // Decrease: Move up
            m[i]->k = k;
            MoveUp(i);
        }
        return 0;
    }
    Handle Add(const Key &k, const Data &d) noexcept(noexcept((Key&)k = k) && noexcept(SetKey(m_Size, k, true)))
    {
        if ((int)m.MaxIndex() <= m_Size)
        {
            CMaaPtr_<Rec*, -1> m2(2 * m_Size + 1);
            if (!m2.IsValid())
            {
                return nullptr;
            }
            //memcpy((Rec**)m2, (Rec**)m, m.Size());
            memcpy(m2, m, m.Size());
            m.Swap(m2);
        }
        Handle h;
        try
        {
            h = TL_NEW Rec(k, d, m_Size);
        }
        catch (...)
        {
            h = nullptr;
        }
        if ((m[m_Size] = h))
        {
            SetKey(++m_Size - 1, k, true);
            return h;
        }
        return nullptr;
    }
#if 0
    void Print() noexcept // simple
    {
        __utf8_printf("ch:   ");
        for (int i = 0; i < m_Size; i++)
        {
            __utf8_printf("%d ", (int)m[i]->k);
        }
        __utf8_printf("\n");
    }
#endif
};
//------------------------------------------------
//------------------------------------------------
#if 0
template <class Key, class Data> class CMaaHeapOld
{
    friend int main2(int argn, char * args[]);
    friend class CMaaHaffman;

    class Rec
    {
    public:
        Key k;
        int l;
        ssize_t i;
        Data d;

        Rec(Key _k, Data _d, int _l = -1, ssize_t _i = -1) noexcept( noexcept(Key(_k)) && noexcept(Data(_d)) )
        :   k(_k),
            l(_l),
            i(_i),
            d(_d)
        {
        }
        ADD_UL_ALLOCATOR(Rec)
//#define DEF_ALLOCATOR_CMaaHeapOld(A,B) template<> CMaaFixedAllocator< CMaaHeapOld< A, B >::Rec >* CMaaHeapOld< A, B >::Rec::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaHeapOld< A, B >::Rec > CMaaHeapOld< A, B >::Rec::s_AllocatorCreator(&CMaaHeapOld< A, B >::Rec::s_pAllocator);
  #define DEF_ALLOCATOR_CMaaHeapOld(A,B) template<> CMaaFixedAllocator< CMaaHeapOld< A, B >::Rec, -1 >* CMaaHeapOld< A, B >::Rec::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaHeapOld< A, B >::Rec > CMaaHeapOld< A, B >::Rec::s_AllocatorCreator(&CMaaHeapOld< A, B >::Rec::s_pAllocator);
    };
public:
    typedef Rec * Handle;

protected:
    size_t N;
    Handle * l[64];
    int pos_l;
    ssize_t pos_i;

public:
    CMaaHeapOld() noexcept
    {
        memset(l, 0, sizeof(l));
        N = 0;
        pos_l = 0;
        pos_i = 0;
    }
    ~CMaaHeapOld() noexcept
    {
        //printf("1. N = %d\n", N);
        for (int i = 64; i--;)
        {
            if  (l[i])
            {
                for (size_t j = (((size_t)1) << i); j--; )
                {
                    if  (l[i][j])
                    {
                        delete l[i][j]; l[i][j] = nullptr;
                        --N;
                    }
                }
                delete [] l[i]; l[i] = nullptr;
            }
        }
        //printf("2. N = %d\n", N);
    }
    /*
     0 - [0][0]
     1 - [1][0]
     2 - [1][1]
     3 - [2][0]
     */
    static void Left(int &l, ssize_t &i) noexcept
    {
        l++;
        i *= 2;
    }
    static void Right(int &l, ssize_t &i) noexcept
    {
        l++;
        i = 2 * i + 1;
    }
    static void Up(int &l, ssize_t &i) noexcept
    {
        l--;
        i /= 2;
    }
    Handle Add(const Key &k, const Data &d)
    {
        if  (!l[pos_l])
        {
            l[pos_l] = TL_NEW Handle[((size_t)1) << pos_l];
            if  (!l[pos_l])
            {
                throw "TL_NEW [] error";
            }
            memset(l[pos_l], 0, (((size_t)1) << pos_l) * sizeof(Handle));
        }
        Rec * pObj = TL_NEW Rec(k, d, pos_l, pos_i);
        l[pos_l][pos_i] = pObj;
        int a = pos_l;
        size_t b = pos_i;
        while(a > 0 && k < l[a - 1][b / 2]->k)
        {
            Swap(l[a][b], l[a - 1][b / 2]);
            a--;
            b /= 2;
        }
        ++N;
        if  (++pos_i >= (((ssize_t)1) << pos_l))
        {
            pos_l++;
            pos_i = 0;
        }
        Check();
        return pObj;
    }
    bool Look(Key *k = nullptr, Data *d = nullptr) const noexcept( noexcept(*k=*k) && noexcept(*d=*d) ) // Look at top
    {
        if  (pos_l == 0 && pos_i == 0)
        {
            return false;
        }
        if  (k)
        {
            *k = l[0][0]->k;
        }
        if  (d)
        {
            *d = l[0][0]->d;
        }
        return true;
    }
    bool Del(Key *k = nullptr, Data *d = nullptr) noexcept( noexcept(*k=*k) && noexcept(*d=*d) )  // Del top
    {
        if  (pos_l == 0 && pos_i == 0)
        {
            return false;
        }
        if  (--pos_i < 0)
        {
            pos_l--;
            pos_i = (((ssize_t)1) << pos_l) - 1;
        }
        if  (k)
        {
            *k = l[0][0]->k;
        }
        if  (d)
        {
            *d = l[0][0]->d;
        }
        Swap(l[0][0], l[pos_l][pos_i]);
        delete l[pos_l][pos_i];
        l[pos_l][pos_i] = nullptr;
        --N;
        int a = 0, b = 0;
        while(a + 1 <= pos_l)
        {
            if  (HaveLeft(a, b) && HaveRight(a, b) && (l[a + 1][b * 2]->k < l[a][b]->k || l[a + 1][b * 2 + 1]->k < l[a][b]->k))
            {
                if  (l[a + 1][b * 2]->k < l[a + 1][b * 2 + 1]->k)
                {
                    Swap(l[a][b], l[a + 1][b * 2]);
                    ++a, b *= 2;
                }
                else
                {
                    Swap(l[a][b], l[a + 1][b * 2 + 1]);
                    ++a, b = b * 2 + 1;
                }
            }
            else if (HaveLeft(a, b) && l[a + 1][b * 2]->k < l[a][b]->k)
            {
                Swap(l[a][b], l[a + 1][b * 2]);
                ++a, b *= 2;
            }
            else
            {
                break;
            }
        }
        Check();
        return true;
    }
    size_t GetCount() const noexcept
    {
        return N;
    }
    bool Exists(int a, ssize_t b) const noexcept
    {
        return (a < pos_l || (a == pos_l && b < pos_i));
    }
    bool HaveLeft(int a, ssize_t b) const noexcept
    {
        Left(a, b);
        return Exists(a, b);
    }
    bool HaveRight(int a, ssize_t b) const noexcept
    {
        Right(a, b);
        return Exists(a, b);
    }
    Handle GetHandle(int a, ssize_t b) const noexcept
    {
        if  (a >= 0 && Exists(a, b))
        {
            return l[a][b];
        }
        return nullptr;
    }
    Handle GetHandle(size_t Index) const noexcept
    {
        int a;
        for (a = 0; (Index >> a); a++) ;
        return GetHandle(a, (ssize_t)Index - (((ssize_t)1) << (a - 1)));
    }
    bool Look(Handle ptr, Key *k, Data *d) const noexcept( noexcept(*k=*k) && noexcept(*d=*d) )
    {
        if  (ptr)
        {
            if  (k)
            {
                *k = ptr->k;
            }
            if  (d)
            {
                *d = ptr->d;
            }
            return true;
        }
        return false;
    }
    bool Del(Handle ptr, Key *k = nullptr, Data *d = nullptr) noexcept( noexcept(*k=*k) && noexcept(*d=*d) )
    {
        if  (!ptr)
        {
            return false;
        }
        int pl = ptr->l;
        ssize_t pi = ptr->i;
        if  (pl < 0 || pi < 0 || pi >= (((ssize_t)1) << pl) || pl > pos_l || (pl == pos_l && pi >= pos_i))
        {
            return false;
        }
        if  (k)
        {
            *k = l[pl][pi]->k;
        }
        if  (d)
        {
            *d = l[pl][pi]->d;
        }

        if  (--pos_i < 0)
        {
            pos_l--;
            pos_i = (((size_t)1) << pos_l) - 1;
        }

        Swap(l[pl][pi], l[pos_l][pos_i]);
        delete l[pos_l][pos_i];
        l[pos_l][pos_i] = nullptr;
        --N;

        if  (pl != pos_l || pi != pos_i) // or l[pl][pi] != nullptr
        {
            int a = pl;
            ssize_t b = pi;
            while(a > 0 && l[a][b]->k < l[a - 1][b / 2]->k)
            {
                Swap(l[a][b], l[a - 1][b / 2]);
                a--;
                b /= 2;
            }
            while(a + 1 <= pos_l)
            {
                if  (HaveLeft(a, b) && HaveRight(a, b) && (l[a + 1][b * 2]->k < l[a][b]->k || l[a + 1][b * 2 + 1]->k < l[a][b]->k))
                {
                    if  (l[a + 1][b * 2]->k < l[a + 1][b * 2 + 1]->k)
                    {
                        Swap(l[a][b], l[a + 1][b * 2]);
                        ++a, b *= 2;
                    }
                    else
                    {
                        Swap(l[a][b], l[a + 1][b * 2 + 1]);
                        ++a, b = b * 2 + 1;
                    }
                }
                else if (HaveLeft(a, b) && l[a + 1][b * 2]->k < l[a][b]->k)
                {
                    Swap(l[a][b], l[a + 1][b * 2]);
                    ++a, b *= 2;
                }
                else
                {
                    break;
                }
            }
        }
        Check();
        return true;
    }
    void Swap(Handle &a, Handle &b) noexcept
    {
        CMaaSwap(a->l, b->l);
        CMaaSwap(a->i, b->i);
        CMaaSwap(a, b);
    }
#if 1
    bool Check() noexcept {return true;}
#else
    void Print()
    {
        printf("\n");
        for (int a = 0; a <= (pos_i ? pos_l : pos_l - 1); a++)
        {
            for (int b = 0; b < (a == pos_l ? pos_i : (1 << a)); b++)
            {
                if  (b)
                {
                    printf("\t");
                }
                printf("%d", (int)l[a][b]->k);
            }
            printf("\n");
        }
    }
    bool Check() noexcept { return true; }
#if 0
    bool Check() noexcept
    {
        for (int a = 1; a <= (pos_i ? pos_l : pos_l - 1); a++)
        {
            for (ssize_t b = 0; b < (a == pos_l ? pos_i : (((ssize_t)1) << a)); b++)
            {
                if  (l[a - 1][b / 2]->k <= l[a][b]->k)
                {
                    //ok
                }
                else
                {
                    printf("Checked error: [%d][%d]\n", a, (int)b);
                    return false;
                }
            }
        }
        return true;
    }
#endif
#endif
};
#endif

//DEF_ALLOCATOR_CMaaHeap(int, int)


//------------------------------------------------
// Simple Max heap's class
//------------------------------------------------
// m[Parent(i)] >= m[i]
//------------------------------------------------
template <class Key, class Data> class CMaaMaxHeap
{
public:
    struct Rec
    {
        Key k;
        Data d;
    };
    int m_Size;
    CMaaPtr_<Rec, -1> m;
public:
    CMaaMaxHeap(int sz = 0) noexcept
    :   m_Size(sz),
        m(sz)
    {
    }

    static constexpr int Parent(int i) noexcept
    {
        return (i - 1) / 2;
    }
    static constexpr int Left(int i) noexcept
    {
        return 2 * i + 1;
    }
    static constexpr int Right(int i) noexcept
    {
        return 2 * i + 2;
    }
protected:
    void MoveDown_(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        /*
        while (1)
        {
            const int l = Left(i);
            const int r = Right(i);
            int largest = (l < m_Size && m[i].k < m[l].k) ? l : i;
            if (r < m_Size && m[largest].k < m[r].k)
            {
                largest = r;
            }
            if (largest == i)
            {
                break;
            }
            Rec tmp = m[i];
            m[i] = m[largest];
            m[largest] = tmp;
            //MoveDown_(largest); //Heapify(largest);
            i = largest;
        }
        */
        Rec tmp = m[i];
        while (1)
        {
            int l = Left(i);
            if (l >= m_Size)
            {
                break;
            }
            const int r = l + 1;
            // find maximal child
            l = r < m_Size && m[l].k < m[r].k ? r : l;
            /*
            if (tmp.k >= m[l].k)
            {
                break;
            }
            */
            if (!(tmp.k < m[l].k))
            {
                break;
            }
            //Swap(i, l);
            m[i] = m[l];
            //m[i].idx = i;
            //// MoveDown_(l); //Heapify(l);
            i = l;
        }
        m[i] = tmp;
        //m[i].idx = i;
    }
public:
    //void Heapify(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    void MoveDown(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if (i >= 0 && i < m_Size)
        {
            MoveDown_(i);
        }
    }
    void MoveUp(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        /*
        int p;
        while (i > 0 && m[(p = Parent(i))].k < m[i].k)
        {
            Rec tmp = m[i];
            m[i] = m[p];
            m[p] = tmp;
            i = p;
        }
        */
        Rec tmp = m[i];
        int p;
        while (i > 0 && m[(p = Parent(i))].k < tmp.k)
        {
            //Swap(i, p);
            m[i] = m[p];
            //m[i].idx = i;
            i = p;
        }
        m[i] = tmp;
        //m[i].idx = i;
    }
    void BuildHeap() noexcept(noexcept(MoveDown_(0)))
    {
        if (m_Size > 1)
        {
            for (int i = Parent(m_Size - 1); i >= 0; i--)
            {
                MoveDown_(i);
            }
        }
    }
    bool Look(Key* k = nullptr, Data* d = nullptr) const noexcept(noexcept(*k = *k) && noexcept(*d = *d)) // look min
    {
        if (m_Size <= 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[0].k;
        }
        if (d)
        {
            *d = m[0].d;
        }
        return true;
    }
    bool Del(Key* k = nullptr, Data* d = nullptr) noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0] = m[0]) && noexcept(MoveDown(0))) // del & return min
    {
        if (m_Size <= 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[0].k;
        }
        if (d)
        {
            *d = m[0].d;
        }
        m[0] = m[--m_Size];
        MoveDown(0);
        return true;
    }
    bool Del(int i, Key* k = nullptr, Data* d = nullptr) noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0] = m[0]) && noexcept(MoveDown(0))) // del & return [i]
    {
        if (m_Size <= 0 || i >= m_Size || i < 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[i].k;
        }
        if (d)
        {
            *d = m[i].d;
        }
        m[i] = m[--m_Size];
        MoveDown(i);
        return true;
    }
    int SetKey(int i, Key k, bool bInsert = false) noexcept(noexcept(k < k) && noexcept(k = k) && noexcept(k == k) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if (i < 0 || i >= m_Size)
        {
            return 1; // out of range
        }
        if (k < m[i].k)
        {
            // Decrease: Move down
            m[i].k = k;
            MoveDown_(i); //Heapify(i);
        }
        else if (bInsert || !(k == m[i].k))
        {
            // Increase: Move up
            m[i].k = k;
            MoveUp(i);
        }
        return 0;
    }
    int Add(Key k, Data d) noexcept(noexcept(k = k) && noexcept(d = d) && noexcept(SetKey(m_Size, k, true)))
    {
        if (m.CheckExpand(m_Size + 1))
        {
            m[m_Size].k = k;
            m[m_Size++].d = d;
            return SetKey(m_Size - 1, k, true);
        }
        return -1;
    }
    void Print() noexcept // simple
    {
        __utf8_printf("maxh: ");
        for (int i = 0; i < m_Size; i++)
        {
            __utf8_printf("%d ", (int)m[i].k);
        }
        __utf8_printf("\n");
    }
};
//------------------------------------------------

//------------------------------------------------
// Simple Max heap's class
//------------------------------------------------
// m[Parent(i)] >= m[i]
//------------------------------------------------
#if 0
template <class Key, class Data> class CMaaMaxHeapOld
{
public:
    struct Rec
    {
        Key k;
        Data d;
    };
    int m_Size;
    CMaaPtrAE<Rec> m;
public:
    CMaaMaxHeapOld(int sz = 0)
    :   m_Size(sz),
        m(sz)
    {
    }

    static constexpr int Parent(int i) noexcept
    {
        return i / 2;
    }
    static constexpr int Left(int i) noexcept
    {
        return 2 * i;
    }
    static constexpr int Right(int i) noexcept
    {
        return 2 * i + 1;
    }
    void Heapify(int i) //noexcept(noexcept(Key::operator >(const Key &)const) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        const int l = Left(i);
        const int r = Right(i);
        int largest;
        if  (l <= m_Size && m[l - 1].k > m[i - 1].k)
        {
            largest = l;
        }
        else
        {
            largest = i;
        }
        if  (r <= m_Size && m[r - 1].k > m[largest - 1].k)
        {
            largest = r;
        }
        if  (largest != i)
        {
            Rec tmp = m[i - 1];
            m[i - 1] = m[largest - 1];
            m[largest - 1] = tmp;
            Heapify(largest);
        }
    }
    void BuildHeap() //noexcept(noexcept(CMaaMaxHeap<Key, Data>::Heapify(1)))
    {
        for (int i = m_Size / 2; i >= 1; i--)
        {
            Heapify(i);
        }
    }
    bool Look(Key *k = nullptr, Data *d = nullptr) const noexcept( noexcept(*k=*k) && noexcept(*d=*d) ) // look max
    {
        if  (m_Size <= 0)
        {
            return false;
        }
        if  (k)
        {
            *k = m[0].k;
        }
        if  (d)
        {
            *d = m[0].d;
        }
        return true;
    }
    bool Del(Key *k = nullptr, Data *d = nullptr) //noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0] = m[0]) && noexcept(Heapify(1))) // del & return max
    {
        if  (m_Size <= 0)
        {
            return false;
        }
        if  (k)
        {
            *k = m[0].k;
        }
        if  (d)
        {
            *d = m[0].d;
        }
        m[0] = m[--m_Size];
        Heapify(1);
        return true;
    }
    int SetKey(int i, Key k, bool bInsert = false) noexcept(noexcept(k < k) && noexcept(k = k) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if  (i <= 0 || i > m_Size)
        {
            return 1; // out of range
        }
        if  (k < m[i - 1].k)
        {
            // Increase only !!!
            return 2;
        }
        if  (!bInsert && k == m[i - 1].k)
        {
            return 0; // skip the same
        }
        m[i - 1].k = k;
        int p;
        while(i > 1 && m[(p = Parent(i)) - 1].k < m[i - 1].k)
        {
            Rec tmp = m[i - 1];
            m[i - 1] = m[p - 1];
            m[p - 1] = tmp;
            i = p;
        }
        return 0;
    }
    int Add(Key k, Data d) // noexcept(noexcept(k = k) && noexcept(d = d) && noexcept(SetKey(m_Size, k, true)))
    {
        m[m_Size].k = k;
        m[m_Size++].d = d;
        return SetKey(m_Size, k, true);
    }
    void Print() // simple
    {
        for (int i = 0; i < m.m_Size; i++)
        {
            __utf8_printf("%d ", (int)m[i].k);
        }
        __utf8_printf("\n");
    }
};
#endif
//------------------------------------------------
// Simple Min heap's class (priority queue)
//------------------------------------------------
// m[Parent(i)] <= m[i]
//------------------------------------------------
template <class Key, class Data> class CMaaMinHeap
{
public:
    struct Rec
    {
        Key k;
        Data d;
    };
    int m_Size;
    CMaaPtr_<Rec, -1> m;
public:
    CMaaMinHeap(int sz = 0) noexcept
    :   m_Size(sz),
        m(sz)
    {
    }

    static constexpr int Parent(int i) noexcept
    {
        return (i - 1) / 2;
    }
    static constexpr int Left(int i) noexcept
    {
        return 2 * i + 1;
    }
    static constexpr int Right(int i) noexcept
    {
        return 2 * i + 2;
    }
protected:
    void MoveDown_(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        /*
        while (1)
        {
            const int l = Left(i);
            const int r = Right(i);
            int largest = (l < m_Size && m[l].k < m[i].k) ? l : i;
            if (r < m_Size && m[r].k < m[largest].k)
            {
                largest = r;
            }
            if (largest == i)
            {
                break;
            }
            Rec tmp = m[i];
            m[i] = m[largest];
            m[largest] = tmp;
            //MoveDown_(largest); //Heapify(largest);
            i = largest;
        }
        */
        Rec tmp = m[i];
        while (1)
        {
            int l = Left(i);
            if (l >= m_Size)
            {
                break;
            }
            const int r = l + 1;
            // find minimal child
            l = r < m_Size && m[r].k < m[l].k ? r : l;
            /*
            if (tmp.k <= m[l].k)
            {
                break;
            }
            */
            if (!(m[l].k < tmp.k))
            {
                break;
            }
            //Swap(i, l);
            m[i] = m[l];
            //m[i].idx = i;
            //// MoveDown_(l); //Heapify(l);
            i = l;
        }
        m[i] = tmp;
        //m[i].idx = i;
    }
public:
    //void Heapify(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    void MoveDown(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if (i >= 0 && i < m_Size)
        {
            MoveDown_(i);
        }
    }
    void MoveUp(int i) noexcept(noexcept(Key{} < Key{}) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        /*
        int p;
        while (i > 0 && m[i].k < m[(p = Parent(i))].k)
        {
            Rec tmp = m[i];
            m[i] = m[p];
            m[p] = tmp;
            i = p;
        }
        */
        Rec tmp = m[i];
        int p;
        while (i > 0 && tmp.k < m[(p = Parent(i))].k)
        {
            //Swap(i, p);
            m[i] = m[p];
            //m[i].idx = i;
            i = p;
        }
        m[i] = tmp;
        //m[i].idx = i;
    }
    void BuildHeap() noexcept(noexcept(MoveDown_(0)))
    {
        if (m_Size > 1)
        {
            for (int i = Parent(m_Size - 1); i >= 0; i--)
            {
                MoveDown_(i);
            }
        }
    }
    bool Look(Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(*k = *k) && noexcept(*d = *d)) // look min
    {
        if  (m_Size <= 0)
        {
            return false;
        }
        if  (k)
        {
            *k = m[0].k;
        }
        if  (d)
        {
            *d = m[0].d;
        }
        return true;
    }
    bool Del(Key *k = nullptr, Data *d = nullptr) noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0]=m[0]) && noexcept(MoveDown(0))) // del & return min
    {
        if  (m_Size <= 0)
        {
            return false;
        }
        if  (k)
        {
            *k = m[0].k;
        }
        if  (d)
        {
            *d = m[0].d;
        }
        m[0] = m[--m_Size];
        MoveDown(0);
        return true;
    }
    bool Del(int i, Key* k = nullptr, Data* d = nullptr) noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0]=m[0]) && noexcept(MoveDown(0))) // del & return [i]
    {
        if (m_Size <= 0 || i >= m_Size || i < 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[i].k;
        }
        if (d)
        {
            *d = m[i].d;
        }
        m[i] = m[--m_Size];
        MoveDown(i);
        return true;
    }
    int SetKey(int i, Key k, bool bInsert = false) noexcept(noexcept(k < k) && noexcept(k = k) && noexcept(k == k) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if  (i < 0 || i >= m_Size)
        {
            return 1; // out of range
        }
        if (m[i].k < k)
        {
            // Increase: Move down
            m[i].k = k;
            MoveDown_(i); //Heapify(i);
        }
        else if (bInsert || !(k == m[i].k))
        {
            // Decrease: Move up
            m[i].k = k;
            MoveUp(i);
        }
        return 0;
    }
    int Add(Key k, Data d) noexcept(noexcept(k = k) && noexcept(d = d) && noexcept(SetKey(m_Size, k, true)))
    {
        if (m.CheckExpand(m_Size + 1))
        {
            m[m_Size].k = k;
            m[m_Size++].d = d;
            return SetKey(m_Size - 1, k, true);
        }
        return -1;
    }
    void Print() noexcept // simple
    {
        __utf8_printf("minh: ");
        for (int i = 0; i < m_Size; i++)
        {
            __utf8_printf("%d ", (int)m[i].k);
        }
        __utf8_printf("\n");
    }
};
//------------------------------------------------

//------------------------------------------------
// Simple Min heap's class (priority queue)
//------------------------------------------------
// m[Parent(i)] >= m[i]
//------------------------------------------------
#if 0
template <class Key, class Data> class CMaaMinHeapOld
{
public:
    struct Rec
    {
        Key k;
        Data d;
    };
    int m_Size;
    CMaaPtrAE<Rec> m;
public:
    CMaaMinHeapOld(int sz = 0)
    :   m_Size(sz),
        m(sz)
    {
    }

    static constexpr int Parent(int i) noexcept
    {
        return i / 2;
    }
    static constexpr int Left(int i) noexcept
    {
        return 2 * i;
    }
    static constexpr int Right(int i) noexcept
    {
        return 2 * i + 1;
    }
    void Heapify(int i) //noexcept(noexcept(Key::operator <(const Key&) const) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        const int l = Left(i);
        const int r = Right(i);
        int largest;
        if (l <= m_Size && m[l - 1].k < m[i - 1].k)
        {
            largest = l;
        }
        else
        {
            largest = i;
        }
        if (r <= m_Size && m[r - 1].k < m[largest - 1].k)
        {
            largest = r;
        }
        if (largest != i)
        {
            Rec tmp = m[i - 1];
            m[i - 1] = m[largest - 1];
            m[largest - 1] = tmp;
            Heapify(largest);
        }
    }
    void BuildHeap() //noexcept(noexcept(Heapify(1)))
    {
        for (int i = m_Size / 2; i >= 1; i--)
        {
            Heapify(i);
        }
    }
    bool Look(Key* k = nullptr, Data* d = nullptr) const noexcept(noexcept(*k = *k) && noexcept(*d = *d)) // look max
    {
        if (m_Size <= 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[0].k;
        }
        if (d)
        {
            *d = m[0].d;
        }
        return true;
    }
    bool Del(Key* k = nullptr, Data* d = nullptr) //noexcept(noexcept(*k = *k) && noexcept(*d = *d) && noexcept(m[0]=m[0]) && noexcept(Heapify(1))) // del & return max
    {
        if (m_Size <= 0)
        {
            return false;
        }
        if (k)
        {
            *k = m[0].k;
        }
        if (d)
        {
            *d = m[0].d;
        }
        m[0] = m[--m_Size];
        Heapify(1);
        return true;
    }
    int SetKey(int i, Key k, bool bInsert = false) noexcept(noexcept(k > k) && noexcept(k = k) && std::is_nothrow_move_constructible<Rec>::value&& std::is_nothrow_move_assignable<Rec>::value)
    {
        if (i <= 0 || i > m_Size)
        {
            return 1; // out of range
        }
        if (k > m[i - 1].k)
        {
            // Decrease only !!!
            return 2;
        }
        if (!bInsert && k == m[i - 1].k)
        {
            return 0; // skip the same
        }
        m[i - 1].k = k;
        int p;
        while (i > 1 && m[(p = Parent(i)) - 1].k > m[i - 1].k)
        {
            Rec tmp = m[i - 1];
            m[i - 1] = m[p - 1];
            m[p - 1] = tmp;
            i = p;
        }
        return 0;
    }
    int Add(Key k, Data d) // noexcept(noexcept(k = k) && noexcept(d = d) && noexcept(SetKey(m_Size, k, true)))
    {
        m[m_Size].k = k;
        m[m_Size++].d = d;
        return SetKey(m_Size, k, true);
    }
    void Print() // simple
    {
        for (int i = 0; i < m.m_Size; i++)
        {
            __utf8_printf("%d ", (int)m[i].k);
        }
        __utf8_printf("\n");
    }
};
#endif
//------------------------------------------------


#if 0
// MinHeaps and RBTree tests/compare
int main3333()
{
    _qword t00 = GetUsTime();
    {
        const int N = 10000000;// / 50000;
        CMaaPtr_<int, 1> m(N);
        CMaaUnivHash<int, char> ht;
        __utf8_printf("Get unique random ints...\n");
        int i;
        for (i = 0; i < N; i++)
        {
            int x = 0;
            GetRnd(&x, (int)sizeof(x));
            /*
            if (i == 0)
            {
                x = 1287757060;
            }
            if (i == 1)
            {
                x = -1885024578;
            }
            if (i == 2)
            {
                x = -452717411;
            }
            */
            //x = i;
            m[i] = x;
            if  (ht.Add(x, 0))
            {
                i--;
            }
        }
        _qword t001 = GetUsTime();
        CMaaHeap<int, int> ch;
        for (i = 0; i < N; i++)
        {
            ch.Add(m[i], m[i]);
        }
        t001 = GetUsTime() - t001;

        _qword t003 = GetUsTime();
        //CMaaMinHeap<int, int> cminh(N);
        CMaaMinHeap<int, int> cminh;
        for (i = 0; i < N; i++)
        {
            //cminh.m[i].k = m[i];
            //cminh.m[i].d = m[i];
            cminh.Add(m[i], m[i]);
        }
        //cminh.BuildHeap();
        t003 = GetUsTime() - t003;

        _qword t002 = GetUsTime();
        CMaaRBTree<int, int> cr;
        for (i = 0; i < N; i++)
        {
            cr.Add(m[i], m[i]);
        }
        t002 = GetUsTime() - t002;

        int mk[2], md[2];
        mk[0] = mk[1] = md[0] = md[1] = -1;
        _qword t011 = GetUsTime();
        ch.Del(&mk[0], &md[0]);
        for (i = 1; i < N; i++)
        {
            ch.Del(&mk[1], &md[1]);
        }
        t011 = GetUsTime() - t011;
        __utf8_printf("mk0=%d\tmd0=%d\tmk1=%d\tmd1=%d\n", mk[0], md[0], mk[1], md[1]);

        mk[0] = mk[1] = md[0] = md[1] = -1;
        _qword t013 = GetUsTime();
        cminh.Del(&mk[0], &md[0]);
        for (i = 1; i < N; i++)
        {
            cminh.Del(&mk[1], &md[1]);
        }
        t013 = GetUsTime() - t013;
        __utf8_printf("mk0=%d\tmd0=%d\tmk1=%d\tmd1=%d\n", mk[0], md[0], mk[1], md[1]);

        mk[0] = mk[1] = md[0] = md[1] = -1;
        _qword t012 = GetUsTime();
        CMaaRBTree<int, int>::Handle h = cr.begin();
        mk[0] = h->c_key();
        md[0] = h->c_data();
        cr.RBDelete(h);
        for (i = 1; i < N; i++)
        {
            CMaaRBTree<int, int>::Handle h = cr.begin();
            mk[1] = h->c_key();
            md[1] = h->c_data();
            cr.RBDelete(h);
        }
        t012 = GetUsTime() - t012;
        __utf8_printf("mk0=%d\tmd0=%d\tmk1=%d\tmd1=%d\n", mk[0], md[0], mk[1], md[1]);

        __utf8_printf("ch: %,D\t%,D\t\n", t001, t011);
        __utf8_printf("mi: %,D\t%,D\t\n", t003, t013);
        __utf8_printf("cr: %,D\t%,D\t\n", t002, t012);
        __utf8_printf(" /: %.1lf\t%.1lf\t+=/ %.3lf\t\n", (double)t001 / t002, (double)t011 / t012, (double)(t001 + t011) / (t002 + t012));
        __utf8_printf(" /: %.1lf\t%.1lf\t+=/ %.3lf\t\n", (double)t003 / t002, (double)t013 / t012, (double)(t003 + t013) / (t002 + t012));

    }
    return 0;
}

DEF_ALLOCATOR_CMaaHeap(int, int)
DEF_ALLOCATOR_CMaaRBTree(int, int)

/*
Get unique random ints...
mk0=-2147483260 md0=-2147483260 mk1=2147483088  md1=2147483088
mk0=-2147483260 md0=-2147483260 mk1=2147483088  md1=2147483088
mk0=-2147483260 md0=-2147483260 mk1=2147483088  md1=2147483088
ch: 244,741     10,457,618
mi: 382,469     8,736,675
cr: 7,364,559   691,708
 /: 0.0 15.1    +=/ 1.328
 /: 0.1 12.6    +=/ 1.132

// ~ CMaaMinHeap is faster than CMaaHeap
// CMaaMinHeap and CMaaHeap ::Add() is mach faster CMaaRBTree
// CMaaRBTree::Del() is mach faster then CMaaMinHeap and CMaaHeap
// Heap Look() (get min) is O(1), CMaaRBTree::begin() is O(log(N)),
// Heap Look() should be more good then CMaaRBTree::begin() for sockets timers,
// because it can be called on each network socket i/o in the pool.

*/

#endif
