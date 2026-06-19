
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/avl_tree.h

/* Copyright (C) 2002-2025 Andrey A. Moiseenko (support@maasoftware.ru)
  * All rights reserved.
  *
  * This library contains cross-platform templates for working heap
  * sutable for using them in sockets' timers.
  * The library implementation written
  * by Andrey A. Moiseenko (support@maasoftware.ru).
  * This library and applications are
  * FREE FOR COMMERCIAL AND NON-COMMERCIAL USE
  * as long as the following conditions are adhered to.
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

// AVL Tree (balanced tree)
// Keys can not be duplicated
// https://ru.wikipedia.org/wiki/%D0%90%D0%92%D0%9B-%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE
//

//#define TOOLSLIB_AVL_SHARED_ALLOCATOR

template <class Key, class Data> class CMaaAVLTree
{
protected:
    class Node
    {
    public:
        Node * left, * right;
        Key k;
        Data d;
        unsigned char height;
        Node(const Key& _k, const Data& _d) noexcept(noexcept(Key(_k)) && noexcept(Data(_d)))
        :   left(nullptr),
            right(nullptr),
            k(_k),
            d(_d),
            height(1)
        {
        }
        int UpdateHeightGetBalance() noexcept
        {
            const unsigned char l = left ? left->height : 0;
            const unsigned char r = right ? right->height : 0;
            height = 1 + (l > r ? l : r);
            return (int)r - (int)l;
        }
        /*
        Key key() noexcept(noexcept(Key(k)))
        {
            return k;
        }
        Data& data() noexcept
        {
            return d;
        }
        const Key& c_key() const noexcept
        {
            return k;
        }
        const Data& c_data() const noexcept
        {
            return d;
        }
        */
#ifdef TOOLSLIB_AVL_SHARED_ALLOCATOR
        ADD_UL_ALLOCATOR(Node)
#ifndef TOOLSLIB_DONT_USE_ALLOCATORS
////#define DEF_ALLOCATOR_CMaaAVLTree(A,B) template<> CMaaFixedAllocator< CMaaAVLTree< A, B >::Node,  1 >* CMaaAVLTree< A, B >::Node::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaAVLTree< A, B >::Node > CMaaAVLTree< A, B >::Node::s_AllocatorCreator(&CMaaAVLTree< A, B >::Node::s_pAllocator);
//#define DEF_ALLOCATOR_CMaaAVLTree(A,B) template<> CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, -1 >* CMaaAVLTree< A, B >::Node::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaAVLTree< A, B >::Node > CMaaAVLTree< A, B >::Node::s_AllocatorCreator(&CMaaAVLTree< A, B >::Node::s_pAllocator);

//#define DEF_ALLOCATOR_CMaaAVLTree(A,B) template<> CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, 1 >  & CMaaAVLTree< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, 1 >  a;  return (CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, 1 >  &)a; }
  #define DEF_ALLOCATOR_CMaaAVLTree(A,B) template<> CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, -1 > & CMaaAVLTree< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, -1 > a;  return (CMaaFixedAllocator< CMaaAVLTree< A, B >::Node, -1 > &)a; }
#else
#define DEF_ALLOCATOR_CMaaAVLTree(A,B)
#endif

#else
  #define DEF_ALLOCATOR_CMaaAVLTree(A,B)
#endif
    };

    Node * root;
#ifndef TOOLSLIB_AVL_SHARED_ALLOCATOR
    CMaaFixedAllocator<Node, -1>* m_pAllocator;
#endif
    size_t N;

public:

    CMaaAVLTree() 
#ifdef TOOLSLIB_AVL_SHARED_ALLOCATOR
        noexcept
#endif
    :   root(nullptr),
        N(0)
    {
#ifndef TOOLSLIB_AVL_SHARED_ALLOCATOR
        m_pAllocator = TL_NEW CMaaFixedAllocator<Node, -1>(eNotInit);
        if (!m_pAllocator)
        {
            throw "CMaaRBTree(): allocation error";
        }
        m_pAllocator->AddRefEx();
#endif
    }

    ~CMaaAVLTree()
    {
        if (root)
        {
            SimpleFree(root);
        }
#ifndef TOOLSLIB_AVL_SHARED_ALLOCATOR
        m_pAllocator->DelRefEx(&m_pAllocator);
#endif
    }

    size_t GetCount() const noexcept
    {
        return N;
    }

    void Swap(CMaaAVLTree<Key, Data>& That) noexcept
    {
        CMaaSwap(root, That.root);
#ifndef TOOLSLIB_AVL_SHARED_ALLOCATOR
        CMaaSwap(m_pAllocator, That.m_pAllocator);
#endif
        CMaaSwap(N, That.N);
    }

    int GetHeight() const noexcept
    {
        return height(root);
    }

    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key& K, const Data& D, int fOverwrite) noexcept(noexcept(insert(root, K, D, fOverwrite)))
    {
        const size_t N0 = N;
        Node * z = insert(root, K, D, fOverwrite);
        if (!z)
        {
            return 2;
        }
        root = z;
        return fOverwrite || N > N0 ? 0 : 1;
    }
    int Add(const Key& K, const Data& D) noexcept(noexcept(insert(root, K, D, 0)))
    {
        const size_t N0 = N;
        Node* z = insert(root, K, D, 0);
        if (!z)
        {
            return 2;
        }
        root = z;
        return N > N0 ? 0 : 1;
    }
    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    /*
    int AddOver(const Key& K, const Data& D) noexcept(noexcept(Add(K, D, 1))) // noexcept(noexcept(insert(root, K, D, 1)))
    {
        return Add(K, D, 1);
    }
    */
    int AddOver(const Key& K, const Data& D) noexcept(noexcept(insert(root, K, D, 1)))
    {
        const size_t N0 = N;
        Node* z = insert(root, K, D, 1);
        if (!z)
        {
            return 2;
        }
        root = z;
        return 0;
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find(const Key& K, Data* D /*= nullptr*/) const noexcept(noexcept(K < K) && noexcept(*D = *D))
    {
        Node* n = search(root, K);
        if (n)
        {
            if (D)
            {
                *D = n->d;
            }
            return 0;
        }
        return 1;
    }
    int Find(const Key& K) const noexcept(noexcept(K < K))
    {
        return search(root, K) ? 0 : 1;
    }

    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to return deleted context of Data
    int Remove(const Key& K, Data* D /*= nullptr*/) noexcept(noexcept(K < K) && noexcept(*D = *D))
    {
        const size_t N0 = N;
        root = remove(root, K, D);
        return N < N0 ? 0 : 1;
    }
    int Remove(const Key& K) noexcept(noexcept(K < K))
    {
        const size_t N0 = N;
        root = remove(root, K);
        return N < N0 ? 0 : 1;
    }

    void inorder()
    {
        __utf8_printf("%D: ", (_qword)N);
        inorder(root);
        __utf8_printf("\n");
    }

    CMaaString Node2Text(Node* head);
    //{
    //   return CMaaString(x->k);
    //}

    void Print(Node* x = nullptr, int w = 79, int ll = 10)
    {
        x = x ? x : root;
        for (int l = 0; l < ll; l++)
        {
            bool e = false;
            CMaaString str = Print(x, w, l, &e);
            if  (e)
            {
                break;
            }
            __utf8_printf("%S\n", &str);
        }
    }
    CMaaString Print(Node* x, int w = 79, int l = 0, bool* e = nullptr)
    {
        if  (e)
        {
            *e = false;
        }
        if  (!x)
        {
            CMaaString sp(nullptr, w);
            sp.Fill(' ');
            if  (e)
            {
                *e = true;
            }
            return sp;
        }
        if  (l == 0)
        {
            CMaaString Ret = Node2Text(x);
            int w2 = w - (warning_int)Ret.Length();
            if  (w2 < 2)
            {
                w2 = 2;
            }
            int w1 = w2 / 2;
            w2 -= w1;
            CMaaString sp1(nullptr, w1), sp2(nullptr, w2);
            sp1.Fill(' ');
            sp2.Fill(' ');
            return sp1 + Ret + sp2;
        }
        bool ee[2];
        CMaaString Ret =
        Print(x->left, w / 2, l - 1, &ee[0]) +
        Print(x->right, w - w / 2, l - 1, &ee[1]);
        if  (e)
        {
            *e = ee[0] && ee[1];
        }
        return Ret;
    }

protected:

    void DeleteNode(Node* t) noexcept
    {
        //if (t)
        {
#ifdef TOOLSLIB_AVL_SHARED_ALLOCATOR
            delete t;
#else
            t->~Node();
            m_pAllocator->DeleteEx(t, &m_pAllocator);
#endif
        }
    }

    // delete subtree
    void SimpleFree(Node* t) noexcept
    {
        //if (t)
        {
            if (t->left)
            {
                SimpleFree(t->left);
            }
            if (t->right)
            {
                SimpleFree(t->right);
            }
            DeleteNode(t);
        }
    }

    static unsigned char height(const Node * x) noexcept
    {
        return x ? x->height : 0;
    }

    //      a                          b
    //     / \                        / \
    //    L   b          -->         a   R
    //       / \                    / \
    //      C   R                  L   C
    static Node* small_l_rotate(Node* a) noexcept
    {
        Node* b = a->right;
        a->right = b->left;
        b->left = a;
        a->UpdateHeightGetBalance();
        b->UpdateHeightGetBalance();
        return b;
    }

    //        a                              c
    //      /   \                          /   \
    //    L       b          -->         a       b
    //           / \                    / \     / \
    //          c   R                  L   M   N   R
    //         / \
    //        M   N
    static Node* big_l_rotate(Node* a) noexcept
    {
        Node* b = a->right;
        Node* c = b->left;
        a->right = c->left;
        b->left = c->right;
        c->left = a;
        c->right = b;
        a->UpdateHeightGetBalance();
        b->UpdateHeightGetBalance();
        c->UpdateHeightGetBalance();
        return c;
    }

    //      a                          b
    //     / \                        / \
    //    b   R          -->         L   a
    //   / \                            / \
    //  L   C                          C   R
    static Node* small_r_rotate(Node* a) noexcept
    {
        Node* b = a->left;
        a->left = b->right;
        b->right = a;
        a->UpdateHeightGetBalance();
        b->UpdateHeightGetBalance();
        return b;
    }

    //        a                              c
    //      /   \                          /   \
    //    b       R          -->         b       a
    //   / \                            / \     / \
    //  L   c                          L   M   N   R
    //     / \
    //    M   N
    static Node* big_r_rotate(Node* a) noexcept
    {
        Node* b = a->left;
        Node* c = b->right;
        a->left = c->right;
        b->right = c->left;
        c->right = a;
        c->left = b;
        a->UpdateHeightGetBalance();
        b->UpdateHeightGetBalance();
        c->UpdateHeightGetBalance();
        return c;
    }

    void inorder(Node * head)
    {
        if  (!head)
        {
            return;
        }
        inorder(head->left);
        //cout<<head->k<<" ";
        //__utf8_printf("%d ", head->k);
        CMaaString r = Node2Text(head);
        __utf8_printf("%S ", &r);
        inorder(head->right);
    }

    Node * insert(Node * x, const Key& _k, const Data& _d, int fOverwrite) noexcept(noexcept(Node(_k, _d)) && noexcept_new && noexcept(_k < _k))
    {
        if  (!x)
        {
#ifdef TOOLSLIB_AVL_SHARED_ALLOCATOR
            x = TL_NEW Node(_k, _d);
#else
            x = m_pAllocator->NewEx(&m_pAllocator);
            if (!x)
            {
                throw "CMaaAVLTree(): allocation error";
            }
            try
            {
                ::new_(x, Node(_k, _d));
            }
            catch (...)
            {
                m_pAllocator->DeleteEx(x, &m_pAllocator);
                throw;
            }
#endif
            if  (x)
            {
                N++;
            }
            return x;
        }

        if  (_k < x->k)
        {
            Node * z = insert(x->left, _k, _d, fOverwrite);
            if (!z)
            {
                return nullptr;
            }
            x->left = z;
        }
        else if (x->k < _k)
        {
            Node * z = insert(x->right, _k, _d, fOverwrite);
            if (!z)
            {
                return nullptr;
            }
            x->right = z;
        }
        else
        {
            // dup k is not inserted
            if  (fOverwrite)
            {
                x->d = _d;
                return x;
            }
        }
        const int bal = x->UpdateHeightGetBalance();
        return 
            bal > 1 ?
                (height(x->right->left) <= height(x->right->right) ? small_l_rotate(x) : big_l_rotate(x)) :
            bal < -1 ?
                (height(x->left->right) <= height(x->left->left) ? small_r_rotate(x) : big_r_rotate(x)) :
            x;
    }

    Node * remove(Node * x, const Key& _k, Data* _d) noexcept(noexcept(_k < x->k) && noexcept(*_d = x->d))
    {
        if  (!x)
        {
            return x;
        }

        if  (_k < x->k)
        {
            x->left = remove(x->left, _k, _d);
        }
        else if (x->k < _k)
        {
            x->right = remove(x->right, _k, _d);
        }
        else
        {
            if  (_d)
            {
                *_d = x->d;
            }
            Node * l = x->left;
            Node * r = x->right;
            if  (!r)
            {
                DeleteNode(x);
                x = l;
                N--;
            }
            else if (!l)
            {
                DeleteNode(x);
                x = r;
                N--;
            }
            else
            {
                if  (height(x->right) >= height(x->left))
                {
                    while(r->left)
                    {
                        r = r->left;
                    }
                    x->k = r->k;
                    x->d = r->d;
                    x->right = remove(x->right, r->k/*, nullptr*/);
                }
                else
                {
                    while(l->right)
                    {
                        l = l->right;
                    }
                    x->k = l->k;
                    x->d = l->d;
                    x->left = remove(x->left, l->k/*, nullptr*/);
                }
            }
        }

        if  (!x)
        {
            return x;
        }

        const int bal = x->UpdateHeightGetBalance();
        return 
            bal > 1 ?
                (height(x->right->left) <= height(x->right->right) ? small_l_rotate(x) : big_l_rotate(x)) :
            bal < -1 ?
                (height(x->left->right) <= height(x->left->left) ? small_r_rotate(x) : big_r_rotate(x)) :
            x;
    }

    Node* remove(Node* x, const Key& _k/*, Data* _d*/) noexcept(noexcept(_k < x->k) /*&& noexcept(*_d = x->d)*/)
    {
        if (!x)
        {
            return x;
        }

        if (_k < x->k)
        {
            x->left = remove(x->left, _k/*, _d*/);
        }
        else if (x->k < _k)
        {
            x->right = remove(x->right, _k/*, _d*/);
        }
        else
        {
            /*
            if (_d)
            {
                *_d = x->d;
            }
            */
            Node* l = x->left;
            Node* r = x->right;
            if (!r)
            {
                DeleteNode(x);
                x = l;
                N--;
            }
            else if (!l)
            {
                DeleteNode(x);
                x = r;
                N--;
            }
            else
            {
                if (height(x->right) >= height(x->left))
                {
                    while (r->left)
                    {
                        r = r->left;
                    }
                    x->k = r->k;
                    x->d = r->d;
                    x->right = remove(x->right, r->k/*, nullptr*/);
                }
                else
                {
                    while (l->right)
                    {
                        l = l->right;
                    }
                    x->k = l->k;
                    x->d = l->d;
                    x->left = remove(x->left, l->k/*, nullptr*/);
                }
            }
        }

        if (!x)
        {
            return x;
        }

        const int bal = x->UpdateHeightGetBalance();
        return
            bal > 1 ?
                (height(x->right->left) <= height(x->right->right) ? small_l_rotate(x) : big_l_rotate(x)) :
            bal < -1 ?
                (height(x->left->right) <= height(x->left->left) ? small_r_rotate(x) : big_r_rotate(x)) :
            x;
    }

    Node * search(Node * x, const Key &_k) const noexcept(noexcept(_k < x->k))
    {
        while(x)
        {
            if  (_k < x->k)
            {
                x = x->left;
            }
            else if (x->k < _k)
            {
                x = x->right;
            }
            else // if (x->k == x) return x;
            {
                break;
            }
        }
        return x;
    }
};

#if 0
#ifdef __unix__
int getch() {return 0;}
#endif
DEF_ALLOCATOR_CMaaRBTree(int, int)
DEF_ALLOCATOR_CMaaAVLTree(int, int)
DEF_ALLOCATOR_CMaaAVLTree(float, int)
template<> CMaaString CMaaAVLTree<int, int>::Node2Text(CMaaAVLTree<int, int>::Node* n)
{
    // cout << head->k;
    //__utf8_printf("%d", n->k);
    return CMaaString(n->k);
}
template<> CMaaString CMaaAVLTree<float, int>::Node2Text(CMaaAVLTree<float, int>::Node* n)
{
    // cout << head->k;
    //__utf8_printf("%d", n->k);
    return CMaaString::sFormat("[%d]=%.1lf", n->d, n->k);
}

int main_avl_test()
{
    try
    {
        _qword t00 = GetUsTime();
        if (0)
        {
            {
                CMaaAVLTree<int, int> t;
                //t.inorder();
                int i;
                for (i = 1; i <= 9; i++)
                {
                    t.Add(i, i);
                    __utf8_printf("%D:\n", (_qword)t.GetCount());
                    t.Print();
                    getch();
                }
                //t.inorder();
                //t.Print();
                t.Remove(4);
                //t.inorder();
                __utf8_printf("%D:\n", (_qword)t.GetCount());
                t.Print();
                getch();
            }
            __utf8_printf("---\n");
            getch();
            {
                CMaaAVLTree<int, int> t;
                //t.inorder();
                t.Print();

                int m[] = { 20, 25, 15, 10, 30, 5, 35, 67, 43, 21, 10, 89, 38, 69, -5, -35, -65, -89, -43, -88, -20, -38, 0 };
                int i;
                for (i = 0; m[i]; i++)
                {
                    if (m[i] > 0)
                    {
                        __utf8_printf("%D i%d\n", (_qword)t.GetCount(), m[i]);
                        t.Add(m[i], m[i], false) && __utf8_printf("!");
                        t.Print();
                        getch();
                    }
                    else
                    {
                        if (m[i] == -5)
                        {
                            //__utf8_printf("\n");
                            //t.inorder();
                            //t.Print();
                        }
                        //__utf8_printf(" r%d", -m[i]);
                        __utf8_printf("%D r%d\n", (_qword)t.GetCount(), -m[i]);
                        t.Remove(-m[i]) && __utf8_printf("!");
                        t.Print();
                        getch();
                    }
                }
                __utf8_printf("===\n");
                //t.inorder();
                //t.Print();
            }
        }
        //return 0;
        {
            const int N = 1000000;
            CMaaPtr_<int, 1> m(N);
            CMaaUnivHash<int, char> h;
            int i;
            for (i = 0; i < N; i++)
            {
                int x = 0;
                GetRnd(&x, (int)sizeof(x));
                m[i] = x;
                if (h.Add(x, 0))
                {
                    i--;
                }
            }

            _qword t0 = GetUsTime();

            CMaaRBTree<int, int> r;

            for (i = 0; i < N; i++)
            {
                r.Add(m[i], i) && __utf8_printf("0i%d[%d]! ", m[i], i);
            }

            _qword t01 = GetUsTime();
            for (i = 0; i < N; i++)
            {
                r.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
            }

            _qword t1 = GetUsTime();

            CMaaAVLTree<int, int> t;

            for (i = 0; i < N; i++)
            {
                t.Add(m[i], i) && __utf8_printf("1i%d[%d]! ", m[i], i);
            }
            int H = t.GetHeight();

            _qword t11 = GetUsTime();

            for (i = 0; i < N; i++)
            {
                t.Remove(m[i]) && __utf8_printf("1r%d[%d]! ", m[i], i);
            }

            _qword t2 = GetUsTime();

            __utf8_printf("---\n");
            r.Print();
            __utf8_printf("===\n");
            t.Print();

            double hm = 1.45 * log(N + 2) / log(2);
            __utf8_printf("rb  add=%,D\trem=%,D\n", t01 - t0, t1 - t01);
            __utf8_printf("avl add=%,D\trem=%,D\th=%d\tN=%d\thm=%.1lf\n", t11 - t1, t2 - t11, H, N, hm);
        }
    }
    /*
    catch (int x)
    {
        __utf8_printf("catch(%d)\n", x);
    }
    */
    catch (...)
    {
        __utf8_printf("catch(...)\n");
    }
    //return 0;

    {
        CMaaAVLTree<float, int> t;
        t.Add(1.3f, 1);
        t.Add(2.4f, 2);
        t.Add(3.5f, 3);
        t.Add(4.6f, 4);
        t.Add(5.7f, 5);
        t.Add(6.8f, 6);
        t.Add(7.9f, 7);
        t.Print();
        __utf8_printf("\n");
        t.Remove(5.7f);
        t.Remove(6.8f);
        t.Remove(7.9f);
        t.Print();
    }
    return 0;
}
#endif

/*
// opt:
template<> CMaaString CMaaAVLTree<int, int>::Node2Text(CMaaAVLTree<int, int>::Node* n)
{
    return CMaaString(n->k);
}
*/
