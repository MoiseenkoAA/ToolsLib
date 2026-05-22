// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/avl_tree2.h (refs)

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

// AVL Tree (balanced tree)  (v2 - refs version, v2 is slower than v1)
// Keys can not be duplicated
// https://ru.wikipedia.org/wiki/%D0%90%D0%92%D0%9B-%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE
//

template <class Key, class Data> class CMaaAVLTree2
{
protected:
    class Node
    {
    public:
        Node * left, * right;
        Key k;
        Data d;
        unsigned char height;
        Node(const Key& _k, const Data& _d)
        :   left(nullptr),
            right(nullptr),
            k(_k),
            d(_d),
            height(1)
        {
        }
        int UpdateHeightGetBalance()
        {
            unsigned char l = left ? left->height : 0;
            unsigned char r = right ? right->height : 0;
            height = 1 + (l > r ? l : r);
            return (int)r - (int)l;
        }
        /*
        Key key()
        {
            return k;
        }
        Data& data()
        {
            return d;
        }
        const Key& c_key() const
        {
            return k;
        }
        const Data& c_data() const
        {
            return d;
        }
        */
        ADD_UL_ALLOCATOR(Node)
#ifndef TOOLSLIB_DONT_USE_ALLOCATORS
//#define DEF_ALLOCATOR_CMaaAVLTree2(A,B) template<> CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node,  1 >* CMaaAVLTree2< A, B >::Node::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaAVLTree2< A, B >::Node > CMaaAVLTree2< A, B >::Node::s_AllocatorCreator(&CMaaAVLTree2< A, B >::Node::s_pAllocator);
//  #define DEF_ALLOCATOR_CMaaAVLTree2(A,B) template<> CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, -1 >* CMaaAVLTree2< A, B >::Node::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaAVLTree2< A, B >::Node > CMaaAVLTree2< A, B >::Node::s_AllocatorCreator(&CMaaAVLTree2< A, B >::Node::s_pAllocator);
//#define DEF_ALLOCATOR_CMaaAVLTree2(A,B) template<> CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, 1 >  & CMaaAVLTree2< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, 1 >  a;  return (CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, 1 >  &)a; }
  #define DEF_ALLOCATOR_CMaaAVLTree2(A,B) template<> CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, -1 > & CMaaAVLTree2< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, -1 > a;  return (CMaaFixedAllocator< CMaaAVLTree2< A, B >::Node, -1 > &)a; }
#else
#define DEF_ALLOCATOR_CMaaAVLTree2(A,B)
#endif
    };

    Node* root;
    size_t N;

    struct AllocErr {};

public:

    CMaaAVLTree2()
    :   root(nullptr),
        N(0)
    {
    }

    ~CMaaAVLTree2()
    {
        DeleteNode(root);
    }

    size_t GetCount() const
    {
        return N;
    }

    int GetHeight() const
    {
        return height(root);
    }

    // Adds  element. ( Copy element to table )
    // Returns 0 if success. 1 - Key alredy exists. 2 - Not enought free memory
    int Add(const Key& K, const Data& D, int fOverwrite = 0)
    {
        try
        {
            const size_t N0 = N;
            insert(root, K, D, fOverwrite);
            return fOverwrite || N > N0 ? 0 : 1;
        }
        catch(AllocErr)
        {
        }
        return 2;
    }

    // Adds  element. ( Owerwrites it if exists )
    // Returns 0 if success. 2 - Not enought free memory
    int AddOver(const Key& K, const Data& D)
    {
        return Add(K, D, 1);
    }

    // Finds element.
    // If ok: returns 0 and copy Data ( if Data != nullptr )
    int Find(const Key& K, Data* D = nullptr) const
    {
        Node* n = search(root, K);
        if  (n)
        {
            if  (D)
            {
                *D = n->d;
            }
            return 0;
        }
        return 1;
    }

    // Removes element.
    // Returns 0 if ok.
    // it is sutable to add param void * Data ( witch is nullptr by default ) where to return deleted context of Data
    int Remove(const Key& K, Data* D = nullptr)
    {
        const size_t N0 = N;
        remove(root, K, D);
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
            int w2 = w - Ret.Length();
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

    void DeleteNode(Node* t)
    {
        if  (t)
        {
            DeleteNode(t->left);
            DeleteNode(t->right);
            delete t;
        }
    }

    unsigned char height(Node* x) const
    {
        return x ? x->height : 0;
    }

    //      a                          b
    //     / \                        / \
    //    L   b          -->         a   R
    //       / \                    / \
    //      C   R                  L   C
    Node* small_l_rotate(Node* a)
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
    Node* big_l_rotate(Node* a)
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
    Node* small_r_rotate(Node* a)
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
    Node* big_r_rotate(Node* a)
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

    void inorder(Node* head)
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

    void insert(Node*& head, const Key& _k, const Data& _d, int fOverwrite)
    {
        if  (!head)
        {
            N++;
            head = TL_NEW Node(_k, _d);
            if  (!head)
            {
                AllocErr e;
                throw e;
            }
            return;
        }

        if  (_k < head->k)
        {
            insert(head->left, _k, _d, fOverwrite);
        }
        else if (_k > head->k)
        {
            insert(head->right, _k, _d, fOverwrite);
        }
        else
        {
            // dup k is not inserted
            if  (fOverwrite)
            {
                head->d = _d;
            }
        }
        int bal = head->UpdateHeightGetBalance();
        if  (bal > 1)
        {
            if  (height(head->right->left) <= height(head->right->right))
            {
                head = small_l_rotate(head);
            }
            else
            {
                head = big_l_rotate(head);
            }
        }
        else if (bal < -1)
        {
            if  (height(head->left->right) <= height(head->left->left))
            {
                head = small_r_rotate(head);
            }
            else
            {
                head = big_r_rotate(head);
            }
        }
    }

    void remove(Node*& head, const Key& _k, Data* _d)
    {
        if  (!head)
        {
            return;
        }

        if  (_k < head->k)
        {
            remove(head->left, _k, _d);
        }
        else if (_k > head->k)
        {
            remove(head->right, _k, _d);
        }
        else
        {
            if  (_d)
            {
                *_d = head->d;
            }
            Node* l = head->left;
            Node* r = head->right;
            if  (!r)
            {
                delete head;
                head = l;
                N--;
            }
            else if (!l)
            {
                delete head;
                head = r;
                N--;
            }
            else
            {
                if  (height(head->right) >= height(head->left))
                {
                    while(r->left)
                    {
                        r = r->left;
                    }
                    head->k = r->k;
                    head->d = r->d;
                    remove(head->right, r->k, nullptr);
                }
                else
                {
                    while(l->right)
                    {
                        l = l->right;
                    }
                    head->k = l->k;
                    head->d = l->d;
                    remove(head->left, l->k, nullptr);
                }
            }
        }

        if  (!head)
        {
            return;
        }

        int bal = head->UpdateHeightGetBalance();
        if  (bal > 1)
        {
            if  (height(head->right->left) <= height(head->right->right))
            {
                head = small_l_rotate(head);
            }
            else
            {
                head = big_l_rotate(head);
            }
        }
        else if (bal < -1)
        {
            if  (height(head->left->right) <= height(head->left->left))
            {
                head = small_r_rotate(head);
            }
            else
            {
                head = big_r_rotate(head);
            }
        }
    }

    Node* search(Node* head, const Key& _k) const
    {
        if  (!head)
        {
            return nullptr;
        }
        if  (head->k > _k)
        {
            return search(head->left, _k);
        }
        if  (head->k < _k)
        {
            return search(head->right, _k);
        }
        //if (head->k == x)
        //{
        return head;
        //}
        //return nullptr;
    }
};

#if 0
#ifdef __unix__
int getch() { return 0; }
#endif
DEF_ALLOCATOR_RBTree(int, int)
DEF_ALLOCATOR_CMaaAVLTree2(int, int)
DEF_ALLOCATOR_CMaaAVLTree2(float, int)
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
template<> CMaaFixedAllocator<CMaaAVLTree2<int, int>::Node>* CMaaAVLTree2<int, int>::Node::s_pAllocator = nullptr;
template<> CMaaFixedAllocator<CMaaAVLTree2<float, int>::Node>* CMaaAVLTree2<float, int>::Node::s_pAllocator = nullptr;
template<> CMaaString CMaaAVLTree2<int, int>::Node2Text(CMaaAVLTree2<int, int>::Node* n)
{
    // cout << head->k;
    //__utf8_printf("%d", n->k);
    return CMaaString(n->k);
}
template<> CMaaString CMaaAVLTree2<float, int>::Node2Text(CMaaAVLTree2<float, int>::Node* n)
{
    // cout << head->k;
    //__utf8_printf("%d", n->k);
    return CMaaString::sFormat("[%d]=%.1lf", n->d, n->k);
}

int main2222()
{
    try
    {
        _qword t00 = GetUsTime();
        if  (0)
        {
            {
                CMaaAVLTree2<int, int> t;
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
                CMaaAVLTree2<int, int> t;
                //t.inorder();
                t.Print();

                int m[] = { 20, 25, 15, 10, 30, 5, 35, 67, 43, 21, 10, 89, 38, 69, -5, -35, -65, -89, -43, -88, -20, -38, 0 };
                int i;
                for (i = 0; m[i]; i++)
                {
                    if  (m[i] > 0)
                    {
                        __utf8_printf("%D i%d\n", (_qword)t.GetCount(), m[i]);
                        t.Add(m[i], m[i]) && __utf8_printf("!");
                        t.Print();
                        getch();
                    }
                    else
                    {
                        if  (m[i] == -5)
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
                if  (h.Add(x, 0))
                {
                    i--;
                }
            }

            _qword t0 = GetUsTime();

            CMaaRBTree<int, int> r;
            //CMaaAVLTree<int, int> tt;

            for (i = 0; i < N; i++)
            {
                r.Add(m[i], i, false) && __utf8_printf("0i%d[%d]! ", m[i], i);
                //tt.Add(m[i], i) && __utf8_printf("0i%d[%d]! ", m[i], i);
            }

            _qword t01 = GetUsTime();
            for (i = 0; i < N; i++)
            {
                r.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
                //tt.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
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

            CMaaAVLTree2<int, int> _t2;

            for (i = 0; i < N; i++)
            {
                //r.Add(m[i], i) || __utf8_printf("0i%d[%d]! ", m[i], i);
                _t2.Add(m[i], i) && __utf8_printf("0i%d[%d]! ", m[i], i);
            }
            int H2 = _t2.GetHeight();

            _qword t21 = GetUsTime();
            for (i = 0; i < N; i++)
            {
                //r.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
                _t2.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
            }

            _qword t3 = GetUsTime();


            __utf8_printf("---\n");
            r.Print();
            __utf8_printf("===\n");
            t.Print();
            __utf8_printf("---\n");
            _t2.Print();

            double hm = 1.45 * log(N + 2) / log(2);
            __utf8_printf("%,D\t%,D\n", t01 - t0, t1 - t01);
            __utf8_printf("%,D\t%,D\th=%d\tN=%d\t%.1lf\n", t11 - t1, t2 - t11, H, N, hm);
            __utf8_printf("%,D\t%,D\th=%d\tN=%d\t%.1lf\n", t21 - t2, t3 - t21, H2, N, hm);
        }
    }
    /*
    catch (int x)
    {
        __utf8_printf("catch(%d)\n", x);
    }
    */
    catch(...)
    {
        __utf8_printf("catch(...)\n");
    }
    //return 0;

    {
        CMaaAVLTree2<float, int> t;
        t.Add(1.3f, 1);
        t.Add(2.4f, 2);
        t.Add(3.5f, 3);
        t.Add(4.6f, 4);
        t.Add(5.7f, 5);
        t.Add(6.8f, 6);
        t.Add(7.9f, 7);
        t.Print();
        t.Remove(5.7f);
        t.Remove(6.8f);
        t.Remove(7.9f);
        t.Print();
    }
    return 0;
}

int main()
{
    main2222();
    __utf8_printf("==--==--==\n");
    return main2222();
}

#endif

//DEF_ALLOCATOR_CMaaAVLTree2(int, int)
/*
// opt:
template<> CMaaString CMaaAVLTree2<int, int>::Node2Text(CMaaAVLTree2<int, int>::Node* n)
{
    return CMaaString(n->k);
}
*/
