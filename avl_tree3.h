// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/avl_tree3.h (parents, iterators)

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

// AVL Tree (balanced tree)  (v3 - parent+, iterators version)
// Keys can not be duplicated
// https://ru.wikipedia.org/wiki/%D0%90%D0%92%D0%9B-%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE
//

template <class Key, class Data> class CMaaAVLTree3
{
protected:
    class Node
    {
    public:
        Node * p, * left, * right;
        Key k;
        Data d;
        unsigned char height;
        Node(const Key& _k, const Data& _d)
        :   p(nullptr),
            left(nullptr),
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
        ADD_UL_ALLOCATOR(Node)
#ifndef TOOLSLIB_DONT_USE_ALLOCATORS
//#define DEF_ALLOCATOR_CMaaAVLTree3(A,B) template<> CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node,  1 >* CMaaAVLTree3< A, B >::Node::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaAVLTree3< A, B >::Node > CMaaAVLTree3< A, B >::Node::s_AllocatorCreator(&CMaaAVLTree3< A, B >::Node::s_pAllocator);
//  #define DEF_ALLOCATOR_CMaaAVLTree3(A,B) template<> CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, -1 >* CMaaAVLTree3< A, B >::Node::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaAVLTree3< A, B >::Node > CMaaAVLTree3< A, B >::Node::s_AllocatorCreator(&CMaaAVLTree3< A, B >::Node::s_pAllocator);

//#define DEF_ALLOCATOR_CMaaAVLTree3(A,B) template<> CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, 1 >  & CMaaAVLTree3< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, 1 >  a;  return (CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, 1 >  &)a; }
  #define DEF_ALLOCATOR_CMaaAVLTree3(A,B) template<> CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, -1 > & CMaaAVLTree3< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, -1 > a;  return (CMaaFixedAllocator< CMaaAVLTree3< A, B >::Node, -1 > &)a; }
#else
#define DEF_ALLOCATOR_CMaaAVLTree3(A,B)
#endif
    };

public:
    typedef Node* Handle;

protected:
    Node* root;
    size_t N;

    struct AllocErr {};

    //Handle m_h[128], m_p[128];
    char m_t[128];

public:
    static Key key(Handle x)
    {
        return x->key();
    }
    static Data& data(Handle x)
    {
        return x->data();
    }
    static const Key& c_key(Handle x)
    {
        return x->c_key();
    }
    static const Data& c_data(Handle x)
    {
        return x->c_data();
    }
    Key k(Handle x)
    {
        return x->key();
    }
    Data& d(Handle x)
    {
        return x->data();
    }
    /*
    Handle GetHandle(const Key &k) const
    {
        return FindNode(k);
    }
    */

    CMaaAVLTree3()
    :   root(nullptr),
        N(0)
    {
    }

    ~CMaaAVLTree3()
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
            root = insert(root, nullptr, K, D, fOverwrite);
            if  (root)
            {
                //root->p = nullptr;
            }
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
        root = remove(root, nullptr, K, D);
        if  (root)
        {
            //root->p = nullptr;
        }
        return N < N0 ? 0 : 1;
    }

    void inorder()
    {
        __utf8_printf("%D: ", (_qword)N);
        inorder(root);
        __utf8_printf("\n");
    }

    CMaaString Node2Text(Node* x);
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

        Handle it;

        /*
        Key k;
        if (!FindMin(&k))
        {
            //CMaaString str = Node2Text(it);
            //__utf8_printf("(%S) ", &str);
            __utf8_printf("(%d) ", k);
            //__utf8_printf("(%f) ", k);
        }
        */

        for (it = begin(); it != end(); it = FindSucc(it))
        {
            CMaaString str = Node2Text(it);
            __utf8_printf("%S ", &str);
        }
        if  (begin() != end())
        {
            __utf8_printf("\n");
        }

        /*
        if (!FindMax(&k))
        {
            //CMaaString str = Node2Text(it);
            //__utf8_printf("(%S) ", &str);
            __utf8_printf("(%d) ", k);
            //__utf8_printf("(%f) ", k);
        }
        */

        for (it = rbegin(); it != rend(); it = FindPred(it))
        {
            CMaaString str = Node2Text(it);
            __utf8_printf("%S ", &str);
        }
        if  (begin() != end())
        {
            __utf8_printf("\n");
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
        /*
        // checks
        if (x->left && x->left->p != x)
        {
            static char err[512];
            sprintf(err, "chk: %d->left error", x->k);
            throw err;
        }
        if (x->right && x->right->p != x)
        {
            static char err[512];
            sprintf(err, "chk: %d->right error", x->k);
            throw err;
        }
        */
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
    bool Check(Node* x)
    {
        if  (x->p)
        {
            if  (x->p->left == x && x->p->right != x)
            {
            }
            else if (x->p->left != x && x->p->right == x)
            {
            }
            else
            {
                return false;
            }
        }
        if  (x->left && x->left->p != x)
        {
            return false;
        }
        if  (x->right && x->right->p != x)
        {
            return false;
        }
        return true;
    }
    bool CheckAll(Node* x = nullptr)
    {
        if  (!x)
        {
            x = root;
        }
        if  (!Check(x))
        {
            return false;
        }
        if  (x->left && !Check(x->left))
        {
            return false;
        }
        if  (x->right && !Check(x->right))
        {
            return false;
        }
        return true;
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
        if  (a->right)
        {
            a->right->p = a;
        }
        a->p = b;
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
        if  (a->right)
        {
            a->right->p = a;
        }
        if  (b->left)
        {
            b->left->p = b;
        }
        a->p = b->p = c;
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
        if  (a->left)
        {
            a->left->p = a;
        }
        a->p = b;
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
        if  (a->left)
        {
            a->left->p = a;
        }
        if  (b->right)
        {
            b->right->p = b;
        }
        a->p = b->p = c;
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

    Node* insert(Node* x, Node* parent, const Key& _k, const Data& _d, int fOverwrite)
    {
        if  (!x)
        {
            N++;
            x = TL_NEW Node(_k, _d);
            if  (!x)
            {
                AllocErr e;
                throw e;
            }
            x->p = parent;
            return x;
        }

        if  (_k < x->k)
        {
            x->left = insert(x->left, x, _k, _d, fOverwrite);
        }
        else if (_k > x->k)
        {
            x->right = insert(x->right, x, _k, _d, fOverwrite);
        }
        else
        {
            // dup k is not inserted
            if  (fOverwrite)
            {
                x->d = _d;
            }
        }
        int bal = x->UpdateHeightGetBalance();
        if  (bal > 1)
        {
            if  (height(x->right->left) <= height(x->right->right))
            {
                x = small_l_rotate(x);
            }
            else
            {
                x = big_l_rotate(x);
            }
            x->p = parent;
        }
        else if (bal < -1)
        {
            if  (height(x->left->right) <= height(x->left->left))
            {
                x = small_r_rotate(x);
            }
            else
            {
                x = big_r_rotate(x);
            }
            x->p = parent;
        }
        return x;
    }

    /*
    Node* insert_it(Node* x, Node* parent, const Key& _k, const Data& _d, int fOverwrite)
    {
        int idx = 0;
        m_h[idx] = x;
        m_p[idx] = parent;
        m_t[idx] = 0;
        for (; m_h[idx]; idx++)
        {
            if (_k < m_h[idx]->k)
            {
                m_h[idx + 1] = m_h[idx]->left;
                m_p[idx + 1] = m_h[idx];
                m_t[idx] = -1;
                //x->left = insert(x->left, x, _k, _d, fOverwrite);
            }
            else if (_k > m_h[idx]->k)
            {
                m_h[idx + 1] = m_h[idx]->right;
                m_p[idx + 1] = m_h[idx];
                m_t[idx] = 1;
                //x->right = insert(x->right, x, _k, _d, fOverwrite);
            }
            else
            {
                // dup k is not inserted
                if (fOverwrite)
                {
                    x->d = _d;
                }
                return m_h[0];
            }
        }
        if (!m_h[idx])
        {
            N++;
            x = TL_NEW Node(_k, _d);
            if (!x)
            {
                AllocErr e;
                throw e;
            }
            x->p = m_p[idx];
        }
        for (; --idx >= 0; )
        {
            if (m_t[idx] < 0)
            {
                m_p[idx + 1]->left = x;
            }
            else
            {
                m_p[idx + 1]->right = x;
            }
            x = m_p[idx + 1];
            int bal = x->UpdateHeightGetBalance();
            if (bal > 1)
            {
                if (height(x->right->left) <= height(x->right->right))
                {
                    x = small_l_rotate(x);
                }
                else
                {
                    x = big_l_rotate(x);
                }
                x->p = m_p[idx];
            }
            else if (bal < -1)
            {
                if (height(x->left->right) <= height(x->left->left))
                {
                    x = small_r_rotate(x);
                }
                else
                {
                    x = big_r_rotate(x);
                }
                x->p = m_p[idx];
            }
        }
        return x;
    }
    */
    Node* insert_it3(Node* x, Node* parent, const Key& _k, const Data& _d, int fOverwrite)
    {
        Node* x0 = x;
        int idx = 0;
        //m_h[idx] = x;
        //m_p[idx] = parent;
        m_t[idx] = 0;
        for (; x; idx++)
        {
            if  (_k < x->k)
            {
                //m_h[idx + 1] = m_h[idx]->left;
                //m_p[idx + 1] = m_h[idx];
                m_t[idx] = -1;
                //x->left = insert(x->left, x, _k, _d, fOverwrite);
                parent = x;
                x = x->left;
            }
            else if (_k > x->k)
            {
                //m_h[idx + 1] = m_h[idx]->right;
                //m_p[idx + 1] = m_h[idx];
                m_t[idx] = 1;
                //x->right = insert(x->right, x, _k, _d, fOverwrite);
                parent = x;
                x = x->right;
            }
            else
            {
                // dup k is not inserted
                if  (fOverwrite)
                {
                    x->d = _d;
                }
                return x0;
            }
        }
        if  (!x)
        {
            N++;
            x = TL_NEW Node(_k, _d);
            if  (!x)
            {
                AllocErr e;
                throw e;
            }
            x->p = parent;// m_p[idx];
        }
        for (; --idx >= 0; )
        {
            if  (m_t[idx] < 0)
            {
                //m_p[idx + 1]->left = x;
                parent->left = x;
            }
            else
            {
                //m_p[idx + 1]->right = x;
                parent->right = x;
            }
            //x = m_p[idx + 1];
            x = parent;
            parent = x->p;
            int bal = x->UpdateHeightGetBalance();
            if  (bal > 1)
            {
                if  (height(x->right->left) <= height(x->right->right))
                {
                    x = small_l_rotate(x);
                }
                else
                {
                    x = big_l_rotate(x);
                }
                x->p = parent;// m_p[idx];
            }
            else if (bal < -1)
            {
                if  (height(x->left->right) <= height(x->left->left))
                {
                    x = small_r_rotate(x);
                }
                else
                {
                    x = big_r_rotate(x);
                }
                x->p = parent;// m_p[idx];
            }
        }
        return x;
    }

    Node* remove(Node* x, Node* parent, const Key& _k, Data* _d)
    {
        if  (!x)
        {
            return x;
        }

        if  (_k < x->k)
        {
            x->left = remove(x->left, x, _k, _d);
        }
        else if (_k > x->k)
        {
            x->right = remove(x->right, x, _k, _d);
        }
        else
        {
            if  (_d)
            {
                *_d = x->d;
            }
            Node* l = x->left;
            Node* r = x->right;
            if  (!r)
            {
                delete x;
                x = l;
                if  (x)
                {
                    x->p = parent;
                }
                N--;
            }
            else if (!l)
            {
                delete x;
                x = r;
                //if (x)
                //{
                x->p = parent;
                //}
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
                    x->right = remove(x->right, x, r->k, nullptr);
                }
                else
                {
                    while(l->right)
                    {
                        l = l->right;
                    }
                    x->k = l->k;
                    x->d = l->d;
                    x->left = remove(x->left, x, l->k, nullptr);
                }
            }
        }

        if  (!x)
        {
            return x;
        }

        int bal = x->UpdateHeightGetBalance();
        if  (bal > 1)
        {
            if  (height(x->right->left) <= height(x->right->right))
            {
                x = small_l_rotate(x);
            }
            else
            {
                x = big_l_rotate(x);
            }
            x->p = parent;
        }
        else if (bal < -1)
        {
            if  (height(x->left->right) <= height(x->left->left))
            {
                x = small_r_rotate(x);
            }
            else
            {
                x = big_r_rotate(x);
            }
            x->p = parent;
        }
        return x;
    }

    Node* search(Node* x, const Key& _k) const
    {
        if  (x)
        {
            if  (x->k > _k)
            {
                return search(x->left, _k);
            }
            if  (x->k < _k)
            {
                return search(x->right, _k);
            }
            //if (x->k == x)
            //{
            //   return x;
            //}
        }
        return x;
        //return nullptr;
    }
public:
    Node* begin() const
    {
        return root ? TreeMinimum(root) : root;
    }
    // next:  p = rb.TreeSuccessor(p);
    // for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreeSuccessor(p))
    Node* end() const
    {
        return nullptr;
    }
    Node* rbegin() const
    {
        return root ? TreeMaximum(root) : root;
    }
    // prev:  p = rb.TreePredecessor(p);
    // for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreePredecessor(p))
    Node* rend() const
    {
        return nullptr;
    }
    Node* TreeMinimum(Node* x) const
    {
        while(x->left)
        {
            x = x->left;
        }
        return x;
    }
    Node* TreeMaximum(Node* x) const
    {
        while(x->right)
        {
            x = x->right;
        }
        return x;
    }
    Node* TreeSuccessor(Node* x) const
    {
        if  (x->right)
        {
            return TreeMinimum(x->right);
        }
        Node* y = x->p;
        while(y && x == y->right)
        {
            x = y;
            y = y->p;
        }
        return y;
    }
    Node* TreePredecessor(Node* x) const
    {
        if  (x->left)
        {
            return TreeMaximum(x->left);
        }
        Node* y = x->p;
        while(y && x == y->left)
        {
            x = y;
            y = y->p;
        }
        return y;
    }
    Node* RecursiveTreeSearch(Node* x, const Key& k) const
    {
        if  (!x || k == x->k)
        {
            return x;
        }
        if  (k < x->k)
        {
            return TreeSearch(x->left, k);
        }
        else
        {
            return TreeSearch(x->right, k);
        }
    }
    Node* IteractiveTreeSearch(Node* x, const Key& k) const
    {
        while(x && k != x->k)
        {
            if  (k < x->k)
            {
                x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        return x;
    }
    Node* TreeSearchLQ(const Key& k, bool bRetEq = true, Node* x = nullptr) const
    {
        x = x ? x : root;
        Node* y;
        if  (bRetEq)
        {
            y = IteractiveTreeSearch(x, k);
            if  (y)
            {
                return y;
            }
        }
        y = nullptr;

        while(x)
        {
            if  (k == x->k)
            {
                x = x->left;
            }
            else if (k < x->k)
            {
                x = x->left;
            }
            else
            {
                y = x;
                x = x->right;
            }
        }
        return y;
    }
    Node* TreeSearchGQ(const Key& k, bool bRetEq = true, Node* x = nullptr) const
    {
        x = x ? x : root;
        Node* y;
        if  (bRetEq)
        {
            y = IteractiveTreeSearch(x, k);
            if  (y)
            {
                return y;
            }
        }
        y = nullptr;

        while(x)
        {
            if  (k == x->k)
            {
                x = x->right;
            }
            else if (k < x->k)
            {
                y = x;
                x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
        return y;
    }
    /*
    int Find(const Key& k, Data* d = nullptr)
    {
        Node* x = IteractiveTreeSearch(root, k);
        if (x != _nil)
        {
            if (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }
    */
    Handle FindNode(const Key& k) const
    {
        Node* x = IteractiveTreeSearch(root, k);
        //return x ? x : nullptr;
        return x;
    }
    /*
    int Remove(const Key &k, Data *d = nullptr)
    {
        Node * x = IteractiveTreeSearch(root, k);
        if   (x != _nil)
        {
            if   (d)
            {
                *d = x->d;
            }
            RBDelete(x);
            return 0;
        }
        return 1;
    }
    */
    int FindMin(Key *k = nullptr, Data *d = nullptr) const
    {
        Node * x = root ? TreeMinimum(root) : root;
        if  (x)
        {
            if  (k)
            {
                *k = x->k;
            }
            if  (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }
    int FindMax(Key *k = nullptr, Data *d = nullptr) const
    {
        Node * x = root ? TreeMaximum(root) : root;
        if  (x)
        {
            if  (k)
            {
                *k = x->k;
            }
            if  (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }
    int FindSucc(const Key &_k, Key *k = nullptr, Data *d = nullptr) const
    {
        Node * x = root ? IteractiveTreeSearch(root, _k) : root;
        if  (x)
        {
            x = TreeSuccessor(x);
            if  (x)
            {
                if  (k)
                {
                    *k = x->k;
                }
                if  (d)
                {
                    *d = x->d;
                }
                return 0;
            }
            return 2;
        }
        return 1;
    }
    int FindPred(const Key &_k, Key *k = nullptr, Data *d = nullptr) const
    {
        Node * x = root ? IteractiveTreeSearch(root, _k) : root;
        if  (x)
        {
            x = TreePredecessor(x);
            if  (x)
            {
                if  (k)
                {
                    *k = x->k;
                }
                if  (d)
                {
                    *d = x->d;
                }
                return 0;
            }
            return 2;
        }
        return 1;
    }
    Handle FindSucc(Handle x) const
    {
        if  (x)// && x != _nil)
        {
            x = TreeSuccessor(x);
            //return x != _nil ? x : nullptr;
        }
        //return nullptr;
        return x;
    }
    Handle FindPred(Handle x) const
    {
        if  (x)// && x != _nil)
        {
            x = TreePredecessor(x);
            //return x != _nil ? x : nullptr;
        }
        //return nullptr;
        return x;
    }
    int FindLQ(const Key& _k, bool bRetEq, Key* k = nullptr, Data* d = nullptr) const
    {
        Node* x = TreeSearchLQ(_k, bRetEq, root);
        if  (x)
        {
            if  (k)
            {
                *k = x->k;
            }
            if  (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }
    int FindGQ(const Key& _k, bool bRetEq, Key* k = nullptr, Data* d = nullptr) const
    {
        Node* x = TreeSearchGQ(_k, bRetEq, root);
        if  (x)
        {
            if  (k)
            {
                *k = x->k;
            }
            if  (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }
    int FindLQ(const Key& _k, Key* k = nullptr, Data* d = nullptr) const
    {
        return FindLQ(_k, true, k, d);
    }
    int FindGQ(const Key& _k, Key* k = nullptr, Data* d = nullptr) const
    {
        return FindGQ(_k, true, k, d);
    }
    int FindLnQ(const Key& _k, Key* k = nullptr, Data* d = nullptr) const
    {
        return FindLQ(_k, false, k, d);
    }
    int FindGnQ(const Key& _k, Key* k = nullptr, Data* d = nullptr) const
    {
        return FindGQ(_k, false, k, d);
    }
};

#if 0
#ifdef __unix__
int getch() { return 0; }
#endif
DEF_ALLOCATOR_RBTree(int, int)
template<> CMaaString CMaaRBTree<int, int>::Node2Text(CMaaRBTree<int, int>::Node* x)
{
    return CMaaString::sFormat("%d%c", x->k, (char)x->color);
}
DEF_ALLOCATOR_CMaaAVLTree3(int, int)
DEF_ALLOCATOR_CMaaAVLTree3(float, int)
template<> CMaaString CMaaAVLTree3<int, int>::Node2Text(CMaaAVLTree3<int, int>::Node* n)
{
    return CMaaString(n->k);
}
template<> CMaaString CMaaAVLTree3<float, int>::Node2Text(CMaaAVLTree3<float, int>::Node* n)
{
    return CMaaString::sFormat("[%d]=%.1lf", n->d, n->k);
}
template<> CMaaFixedAllocator<CMaaAVLTree<int, int>::Node>* CMaaAVLTree<int, int>::Node::s_pAllocator = nullptr;
template<> CMaaFixedAllocator<CMaaAVLTree<float, int>::Node>* CMaaAVLTree<float, int>::Node::s_pAllocator = nullptr;
template<> CMaaString CMaaAVLTree<int, int>::Node2Text(CMaaAVLTree<int, int>::Node* n)
{
    return CMaaString(n->k);
}
template<> CMaaString CMaaAVLTree<float, int>::Node2Text(CMaaAVLTree<float, int>::Node* n)
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
                CMaaAVLTree3<int, int> t;
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
                CMaaAVLTree3<int, int> t;
                //t.inorder();
                t.Print();

                int m[] = { 20, 25, 15, 10, 30, 5, 35, 67, 43, 21, 10, 89, 38, 69, -5, -35, -65, -89, -43, -88, -20, -38, 0 };
                int i;
                for (i = 0; m[i]; i++)
                {
                    if  (m[i] > 0)
                    {
                        __utf8_printf("%D i%d", (_qword)t.GetCount(), m[i]);
                        t.Add(m[i], m[i]) && __utf8_printf("!");
                        __utf8_printf("\n");
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
                        __utf8_printf("%D r%d", (_qword)t.GetCount(), -m[i]);
                        t.Remove(-m[i]) && __utf8_printf("!");
                        __utf8_printf("\n");
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
            const int N = 1000000;// / 50000;
            CMaaPtr_<int, 1> m(N);
            CMaaUnivHash<int, char> h;
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
                if  (h.Add(x, 0))
                {
                    i--;
                }
            }

            _qword mt[8][10]; // [0] - rb, [1] - v3, [2] - v1; [][0] - ins, [][1] - remove
            __utf8_printf("Testing CMaaRBTree<int, int>...\n");
            int idx = 0;

            CMaaRBTree<int, int> r;
            mt[idx][0] = GetUsTime();

            for (i = 0; i < N; i++)
            {
                r.Add(m[i], i, false) && __utf8_printf("0i%d[%d]! ", m[i], i);
                /*
                if (i < 3)
                {
                    r.Add(m[i], i, false) || __utf8_printf("0i%d[%d]!\n", m[i], i);
                }
                */
                //tt.Add(m[i], i) && __utf8_printf("0i%d[%d]! ", m[i], i);
            }
            mt[idx][0] = GetUsTime() - mt[idx][0];
            __utf8_printf(">>>");
            mt[idx][2] = GetUsTime();
            int R = r.GetHeightSlow();
            mt[idx][2] = GetUsTime() - mt[idx][2];
            __utf8_printf("...");

            mt[idx][1] = GetUsTime();
            for (i = 0; i < N; i++)
            {
                r.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
                //tt.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
            }
            mt[idx][1] = GetUsTime() - mt[idx][1];
            __utf8_printf("<<<\n");
            r.Print();

            idx++;
            __utf8_printf("Testing CMaaAVLTree3<int, int>...\n");

            CMaaAVLTree3<int, int> t;

            mt[idx][0] = GetUsTime();
            for (i = 0; i < N; i++)
            {
                t.Add(m[i], i) && __utf8_printf("1i%d[%d]! ", m[i], i);
                /*
                if (i < 3)
                {
                    int y;
                    !t.Find(m[i], &y) && __utf8_printf("%d\n", y);
                }
                */
            }
            mt[idx][0] = GetUsTime() - mt[idx][0];

            mt[idx][2] = GetUsTime();
            int H = t.GetHeight();
            mt[idx][2] = GetUsTime() - mt[idx][2];

            mt[idx][3] = GetUsTime();

            CMaaAVLTree3<int, int>::Handle it;

            it = t.begin();
            int e = 0;
            int prev = t.key(it);
            for (it = t.FindSucc(it); it != t.end(); it = t.FindSucc(it))
            {
                if  (prev >= t.key(it))
                {
                    __utf8_printf("error prev=%d, key=%d\n", prev, t.key(it));
                    e++;
                }
                prev = t.key(it);
            }
            e&& __utf8_printf("\n");
            mt[idx][3] = GetUsTime() - mt[idx][3];

            mt[idx][4] = GetUsTime();
            it = t.rbegin();
            e = 0;
            int next = t.key(it);
            for (it = t.FindPred(it); it != t.rend(); it = t.FindPred(it))
            {
                if  (next <= t.key(it))
                {
                    __utf8_printf("error next=%d, key=%d\n", next, t.key(it));
                    e++;
                }
                next = t.key(it);
            }
            e&& __utf8_printf("\n");
            mt[idx][4] = GetUsTime() - mt[idx][4];

            mt[idx][5] = GetUsTime();
            for (it = t.begin(); it != t.end(); it = t.FindSucc(it))
            {
                if  (!t.Check(it))
                {
                    __utf8_printf("1. error chk, key=%d\n", t.key(it));
                }
            }
            for (it = t.rbegin(); it != t.rend(); it = t.FindPred(it))
            {
                if  (!t.Check(it))
                {
                    __utf8_printf("2. error chk, key=%d\n", t.key(it));
                }
            }
            mt[idx][5] = (GetUsTime() - mt[idx][5]) / 2;

            mt[idx][6] = GetUsTime();
            if  (!t.CheckAll())
            {
                __utf8_printf("3. error chk all\n");
            }
            mt[idx][6] = GetUsTime() - mt[idx][6];


            mt[idx][1] = GetUsTime();
            for (i = 0; i < N; i++)
            {
                t.Remove(m[i]) && __utf8_printf("1r%d[%d]! ", m[i], i);
            }
            mt[idx][1] = GetUsTime() - mt[idx][1];

            idx++;
            __utf8_printf("Testing CMaaAVLTree<int, int>...\n");
            CMaaAVLTree<int, int> _t2;

            mt[idx][0] = GetUsTime();
            for (i = 0; i < N; i++)
            {
                //r.Add(m[i], i) || __utf8_printf("0i%d[%d]! ", m[i], i);
                _t2.Add(m[i], i) && __utf8_printf("0i%d[%d]! ", m[i], i);
                /*
                if (i < 3)
                {
                    int y;
                    !_t2.Find(m[i], &y) && __utf8_printf("%d\n", y);
                }
                */
            }
            mt[idx][0] = GetUsTime() - mt[idx][0];

            mt[idx][2] = GetUsTime();
            int H2 = _t2.GetHeight();
            mt[idx][2] = GetUsTime() - mt[idx][2];

            mt[idx][1] = GetUsTime();
            for (i = 0; i < N; i++)
            {
                //r.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
                _t2.Remove(m[i]) && __utf8_printf("0r%d[%d]! ", m[i], i);
            }
            mt[idx][1] = GetUsTime() - mt[idx][1];

            __utf8_printf("--r--\n");
            r.Print();
            __utf8_printf("--3--\n");
            t.Print();
            __utf8_printf("--1--\n");
            _t2.Print();

            // mt: [0] - rb, [1] - v3, [2] - v1; [][0] - ins, [][1] - remove
            // hm: max theoretical avl height
            double hm = 1.45 * log(N + 2) / log(2), kir = mt[2][0] / (double)mt[0][0], krr = mt[2][1] / (double)mt[0][1], ki = mt[1][0] / (double)mt[2][0], kr = mt[1][1] / (double)mt[2][1];
            __utf8_printf("%,D\t%,D\th=%d\t%,D\n", mt[0][0], mt[0][1], R, mt[0][2]);
            __utf8_printf("%,D\t%,D\th=%d\tN=%d\t%.1lf\t%.3lf\t%.3lf\n", mt[2][0], mt[2][1], H2, N, hm, kir, krr);
            __utf8_printf("%,D\t%,D\th=%d\tN=%d\t%.1lf\t%.3lf\t%.3lf\n", mt[1][0], mt[1][1], H, N, hm, ki, kr);
        }
    }
    /*
    catch (int x)
    {
        __utf8_printf("catch(%d)\n", x);
    }
    */
    catch(const char* m)
    {
        __utf8_printf("catch(%s)\n", m);
    }
    catch(...)
    {
        __utf8_printf("catch(...)\n");
    }
    //return 0;

    {
        CMaaAVLTree3<float, int> t;
        t.Add(1.3f, 1);
        t.Add(2.4f, 2);
        t.Add(3.5f, 3);
        t.Add(4.6f, 4);
        t.Add(5.7f, 5);
        t.Add(6.8f, 6);
        t.Add(7.9f, 7);
        //t.Print();
        t.Remove(5.7f);
        t.Add(4.6f, 44, 1);
        t.Remove(6.8f);
        t.Remove(7.9f);
        //t.Print();
    }
    return 0;
}

int main()
{
    main2222();
    //__utf8_printf("==--==--==\n"); main2222();
    return 0;
}

#endif

//DEF_ALLOCATOR_CMaaAVLTree3(int, int)
/*
// opt:
template<> CMaaString CMaaAVLTree3<int, int>::Node2Text(CMaaAVLTree3<int, int>::Node* n)
{
    return CMaaString(n->k);
}
*/
