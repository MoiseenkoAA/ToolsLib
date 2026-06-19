
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/rbtree.h

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

// Red-Black balanced tree
// Keys can be duplicated

// fixed algo from book
// Т. Кормен, Ч. Лейзерсон, Р. Ривест, К. Штайн
// "Алгоритмы: построение и анализ. 3-е издание"
// and from 1st or second edition

#define CMAA_RB_DEBUG false
//#define CMAA_RB_DEBUG true

//#define BOOK_EX_CODE

//#define TOOLSLIB_RB_SHARED_ALLOCATOR

template <class Key, class Data> class CMaaRBTree
{
public:
    static inline constexpr bool eBlue = false, eRed = true;
protected:
    class Node
    {
    public:
        Node * p, * left, * right;
        Key k;
        Data d;
        bool color;

        Node(const Key &_k, const Data &_d, bool _c) noexcept(noexcept(Key(_k)) && noexcept(Data(_d)))
        :   p(nullptr),
            left(nullptr),
            right(nullptr),
            k(_k),
            d(_d),
            color(_c)
        {
        }
        Key key() noexcept(noexcept(Key(k)))
        {
            return k;
        }
        Data & data() noexcept
        {
            return d;
        }
        const Key & c_key() const noexcept
        {
            return k;
        }
        const Data & c_data() const noexcept
        {
            return d;
        }
        char ColorCh() const noexcept
        {
            return color == eBlue ? 'B' : 'R';
        }
#ifdef TOOLSLIB_RB_SHARED_ALLOCATOR
        ADD_UL_ALLOCATOR(Node)
//#define DEF_ALLOCATOR_CMaaRBTree(A,B) template<> CMaaFixedAllocator< CMaaRBTree< A, B >::Node,  1 >* CMaaRBTree< A, B >::Node::s_pAllocator = nullptr; template<>   CMaaFixedAllocatorCreator< CMaaRBTree< A, B >::Node > CMaaRBTree< A, B >::Node::s_AllocatorCreator(&CMaaRBTree< A, B >::Node::s_pAllocator);
//  #define DEF_ALLOCATOR_CMaaRBTree(A,B) template<> CMaaFixedAllocator< CMaaRBTree< A, B >::Node, -1 >* CMaaRBTree< A, B >::Node::s_pAllocator = nullptr; template<> CMaaFixedAllocatorCreatorUl< CMaaRBTree< A, B >::Node > CMaaRBTree< A, B >::Node::s_AllocatorCreator(&CMaaRBTree< A, B >::Node::s_pAllocator);

//#define DEF_ALLOCATOR_CMaaRBTree(A,B) template<> CMaaFixedAllocator< CMaaRBTree< A, B >::Node, 1 >  & CMaaRBTree< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaRBTree< A, B >::Rec, 1 >  a;  return (CMaaFixedAllocator< CMaaRBTree< A, B >::Rec, 1 >  &)a; }
  #define DEF_ALLOCATOR_CMaaRBTree(A,B) template<> CMaaFixedAllocator< CMaaRBTree< A, B >::Node, -1 > & CMaaRBTree< A, B >::Node::GetAllocator() noexcept { static constexpr CMaaFixedAllocator< CMaaRBTree< A, B >::Rec, -1 > a;  return (CMaaFixedAllocator< CMaaRBTree< A, B >::Rec, -1 > &)a; }

#else
  #define DEF_ALLOCATOR_CMaaRBTree(A,B)
#endif
    };
public:
    typedef Node * Handle;
    static Key key(Handle x) noexcept(noexcept(x->key()))
    {
        return x->key();
    }
    static Data & data(Handle x) noexcept
    {
        return x->data();
    }
    static const Key & c_key(Handle x) noexcept
    {
        return x->c_key();
    }
    static const Data & c_data(Handle x) noexcept
    {
        return x->c_data();
    }
    Key k(Handle x) noexcept(noexcept(x->key()))
    {
        return x->key();
    }
    Data & d(Handle x) noexcept
    {
        return x->data();
    }
    /*
    Handle GetHandle(const Key &k) const
    {
        return FindNode(k);
    }
    */

protected:
    Node* _nil, * root;
    size_t N;
    bool m_bMulti;
#ifndef TOOLSLIB_RB_SHARED_ALLOCATOR
    CMaaFixedAllocator<Node, -1>* m_pAllocator;
#endif

public:
    CMaaRBTree(bool Multi = true) //noexcept(noexcept_new)
    :   m_bMulti(Multi)
    {
        N = 0;
#ifdef TOOLSLIB_RB_SHARED_ALLOCATOR
        _nil = (Node*)TL_NEW char[sizeof(Node)]; // for a lock-free constructor, UL allocator
        //_nil = (Node*)Node::operator new(sizeof(Node));
        if  (!_nil)
        {
            throw "CMaaRBTree(): allocation error";
        }
#else
        m_pAllocator = TL_NEW CMaaFixedAllocator<Node, -1>(eNotInit);
        if (!m_pAllocator)
        {
            throw "CMaaRBTree(): allocation error";
        }
        try
        {
            _nil = m_pAllocator->NewEx(&m_pAllocator);
        }
        catch (...)
        {
            _nil = nullptr;
        }
        if (!_nil)
        {
            throw "CMaaRBTree(): allocation error";
        }
#endif
        memset(_nil, 0, sizeof(Node));
        _nil->color = eBlue;
        root = _nil;
    }
    ~CMaaRBTree()
    {
        SimpleFree(root);
#ifdef TOOLSLIB_RB_SHARED_ALLOCATOR
        delete [] (char *)_nil;
        //Node::operator delete(_nil);
#else
        m_pAllocator->DeleteEx(_nil, &m_pAllocator);
        //m_pAllocator = nullptr; // 24.09.2024
#endif
    }
    void RemoveAll() noexcept
    {
        SimpleFree(root);
        root = _nil;
        N = 0;
        _nil->color = eBlue;
    }
    size_t GetCount() const noexcept
    {
        return N;
    }

protected:

    void DeleteNode(Node* x) noexcept
    {
        //if (x)
        {
#ifdef TOOLSLIB_RB_SHARED_ALLOCATOR
            delete x;
#else
            x->~Node();
            m_pAllocator->DeleteEx(x, &m_pAllocator);
#endif
        }
    }

    // delete subtree
    void SimpleFree(Node* x) noexcept
    {
        if (x != _nil)
        {
            if (x->left != _nil)
            {
                SimpleFree(x->left);
            }
            if (x->right != _nil)
            {
                SimpleFree(x->right);
            }
            DeleteNode(x);
        }
    }

    //
    //       |        <--LeftRotate(x)--         |
    //       Y         --RightRotate(y)-->       X
    //      / \                                 / \
    //     X   g                               a   Y
    //    / \                                     / \
    //   a   b                                   b   g
    //
    void LeftRotate(Node * x) noexcept(!CMAA_RB_DEBUG)
    {
#if CMAA_RB_DEBUG
        if  (x->right == _nil)
        {
            throw "CMaaRBTree::LeftRotate(): x->right == _nil";
        }
#endif
        // x->right != nullptr
        Node * y = x->right;
        x->right = y->left;
        if  (y->left != _nil)
        {
            y->left->p = x;
        }
        y->p = x->p;
        if  (x->p == _nil)
        {
            root = y;
        }
        else if (x == x->p->left)
        {
            x->p->left = y;
        }
        else
        {
            x->p->right = y;
        }
        y->left = x;
        x->p = y;
    }
    void RightRotate(Node * y) noexcept(!CMAA_RB_DEBUG)
    {
#if CMAA_RB_DEBUG
        if  (y->left == _nil)
        {
            throw "CMaaRBTree::RightRotate(): y->left == _nil";
        }
#endif
        // y->left != nullptr
        Node * x = y->left;
        y->left = x->right;
        if  (x->right != _nil)
        {
            x->right->p = y;
        }
        x->p = y->p;
        if  (y->p == _nil)
        {
            root = x;
        }
        else if (y == y->p->right)
        {
            y->p->right = x;
        }
        else
        {
            y->p->left = x;
        }
        x->right = y;
        y->p = x;
    }
public:
    // 0 - ok
    int Add(const Key& k, const Data& d) noexcept(noexcept(RBInsert(k, d, m_bMulti)))
    {
        return RBInsert(k, d, m_bMulti) ? 0 : 1;
    }
#if 0
    int Add_(const Key& k, const Data& d) noexcept(noexcept(RBInsertWiki(k, d, m_bMulti)))
    {
        return RBInsertWiki(k, d, m_bMulti) ? 0 : 1; // ok too
    }
#endif
    // Handle != nullptr - ok
    Handle AddNode(const Key& k, const Data& d) noexcept(noexcept(RBInsert(k, d, m_bMulti)))
    {
        return RBInsert(k, d, m_bMulti);
    }
    // Handle != nullptr - ok
    Node* Insert(const Key& k, const Data& d) noexcept(noexcept(RBInsert(k, d, m_bMulti)))
    {
        return RBInsert(k, d, m_bMulti);
    }

    // specific multi
    // 0 - ok
    int Add(const Key& k, const Data& d, bool bMulti) noexcept(noexcept(RBInsert(k, d, bMulti)))
    {
        return RBInsert(k, d, bMulti) ? 0 : 1;
    }
    // Handle != nullptr - ok
    Handle AddNode(const Key& k, const Data& d, bool bMulti) noexcept(noexcept(RBInsert(k, d, bMulti)))
    {
        return RBInsert(k, d, bMulti);
    }
    // Handle != nullptr - ok
    Node * Insert(const Key &k, const Data &d, bool bMulti) noexcept(noexcept(RBInsert(k, d, bMulti)))
    {
        return RBInsert(k, d, bMulti);
    }

    int GetHeightSlow(Node* x = nullptr) const noexcept
    {
        if  (!x)
        {
            x = root;
        }
        if  (x == _nil)
        {
            return 0;
        }
        const int l = GetHeightSlow(x->left);
        const int r = GetHeightSlow(x->right);
        return 1 + (l > r ? l : r);
    }
    // RB-Insert(T,z)
    // both book & wiki are ok
    Node * RBInsert(const Key &k, const Data &d, bool bMulti) //noexcept(noexcept(Node(k, d, eRed)) && noexcept_new && !CMAA_RB_DEBUG)
    {
        /*
        if (!bMulti)
        {
            Handle r = FindNode(k);
            if (r)
            {
                return nullptr;
            }
        }
        */
        Node * y = _nil;
        Node * x = root;
        if  (!bMulti)
        {
            while(x != _nil)
            {
                y = x;
                if  (k < x->k)
                {
                    x = x->left;
                }
                else if (x->k == k)
                {
                    return nullptr;
                }
                else
                {
                    x = x->right;
                }
            }
        }
        else
        {
            while(x != _nil)
            {
                y = x;
                if  (k < x->k)
                {
                    x = x->left;
                }
                else
                {
                    x = x->right;
                }
            }
        }
#ifdef TOOLSLIB_RB_SHARED_ALLOCATOR
        //Node * z = TL_NEW Node(k, d, eRed);
        Node* z = TL_NEW Node(k, d, eRed);
#else
        Node* z = m_pAllocator->NewEx(&m_pAllocator);
        if (!z)
        {
            throw "CMaaRBTree(): allocation error";
        }
        try
        {
            ::new_(z, Node(k, d, eRed));
        }
        catch (...)
        {
            m_pAllocator->DeleteEx(z, &m_pAllocator);
            throw;
        }
#endif
        //z->p = z->left = z->right =_nil;
        z->p = y;
        if  (y == _nil)
        {
            root = z;
        }
        else if (z->k < y->k)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
        z->left = _nil;
        z->right = _nil;
        z->color = eRed;
        N++;
        //Print();
        RBInsertFixup(z);
        return z;
    }
protected:
    void RBInsertFixup(Node *z) noexcept(!CMAA_RB_DEBUG)
    {
        while(z->p->color == eRed)
        {
            if  (z->p == root)
            {
                //throw "CMaaRBTree::RBInsertFixup(): (z->p == root)";
                break;
            }
            if  (z->p == z->p->p->left)
            {
                Node * y = z->p->p->right;
                if  (y->color == eRed) // wiki: if (y != nullptr && y->color == eRed)
                {
                    z->p->color = eBlue;
                    y->color = eBlue;
                    z->p->p->color = eRed;
                    z = z->p->p;
                    //__utf8_printf(">1<\n");
                    //Print();
                    continue;
                }
                if  (z == z->p->right) // wiki: if (z == z->p->right && z->p == z->p->p->left)
                {
                    z = z->p;
                    LeftRotate(z);
                    //__utf8_printf(">2<\n");
                    //Print();
                }
                z->p->color = eBlue;
                z->p->p->color = eRed;
                // wiki: if (z == z->p->left && z->p == z->p->p->left)
                RightRotate(z->p->p);
                // wiki: else
                // wiki: LeftRotate(z->p->p);

                //__utf8_printf(">3<\n");
                //Print();
                //break;
            }
            else //if (z->p == z->p->p->right)
            {
                Node * y = z->p->p->left;
                if  (y->color == eRed)
                {
                    z->p->color = eBlue;
                    y->color = eBlue;
                    z->p->p->color = eRed;
                    z = z->p->p;
                    //__utf8_printf(">1<\n");
                    //Print();
                    continue;
                }
                if  (y->color == eBlue && z == z->p->left)
                {
                    z = z->p;
                    RightRotate(z);
                    //__utf8_printf(">2<\n");
                    //Print();
                }
                z->p->color = eBlue;
                z->p->p->color = eRed;
                LeftRotate(z->p->p);
                //__utf8_printf(">3<\n");
                //Print();
                //break;
            }
            //else
            //{
            //throw "CMaaRBTree::RBInsertFixup(): (z->p != z->p->p->left) && (z->p != z->p->p->right)";
            //}
            break;
        }
        root->color = eBlue;
    }
protected:
    Node * grandparent(Node *n) noexcept
    {
        return (n != _nil && n->p != _nil) ? n->p->p : _nil;
    }
    Node * uncle(Node *n) noexcept
    {
        Node * g = grandparent(n);
        return g == _nil ? _nil // No grandparent means no uncle
               : n->p == g->left ? g->right : g->left;
    }
    void rotate_left(Node *n) noexcept
    {
        Node *pivot = n->right;

        pivot->p = n->p; // при этом, возможно, pivot становится корнем дерева
        if  (n->p != _nil)
        {
            if  (n->p->left == n)
            {
                n->p->left = pivot;
            }
            else
            {
                n->p->right = pivot;
            }
        }
        else
        {
            root = pivot;
        }

        n->right = pivot->left;
        if  (pivot->left != _nil)
        {
            pivot->left->p = n;
        }

        n->p = pivot;
        pivot->left = n;
    }

    void rotate_right(Node *n) noexcept
    {
        Node *pivot = n->left;

        pivot->p = n->p; // при этом, возможно, pivot становится корнем дерева
        if  (n->p != _nil)
        {
            if  (n->p->left == n)
            {
                n->p->left = pivot;
            }
            else
            {
                n->p->right = pivot;
            }
        }
        else
        {
            root = pivot;
        }

        n->left = pivot->right;
        if  (pivot->right != _nil)
        {
            pivot->right->p = n;
        }
        n->p = pivot;
        pivot->right = n;
    }
    void insert_case1(Node *n) noexcept
    {
        if  (n->p == _nil)
        {
            n->color = eBlue;
        }
        else
        {
            insert_case2(n);
        }
    }
    void insert_case2(Node *n) noexcept
    {
        if  (n->p->color == eBlue)
        {
            return; // Tree is still valid
        }
        else
        {
            insert_case3(n);
        }
    }
    void insert_case3(Node *n) noexcept
    {
        Node *u = uncle(n), * g;

        if  (u != _nil && u->color == eRed)
        {
            // && (n->parent->color == RED) Второе условие проверяется в insert_case2, то есть родитель уже является красным.
            n->p->color = eBlue;
            u->color = eBlue;
            g = grandparent(n);
            g->color = eRed;
            insert_case1(g);
        }
        else
        {
            insert_case4(n);
        }
    }
    void insert_case4(Node *n) noexcept
    {
        Node *g = grandparent(n);

        if  (n == n->p->right && n->p == g->left)
        {
            rotate_left(n->p);
            n = n->left;
        }
        else if (n == n->p->left && n->p == g->right)
        {
            rotate_right(n->p);
            n = n->right;
        }
        insert_case5(n);
    }

    void insert_case5(Node *n) noexcept
    {
        Node *g = grandparent(n);

        n->p->color = eBlue;
        g->color = eRed;
        if  (n == n->p->left && n->p == g->left)
        {
            rotate_right(g);
        }
        else
        {
            // (n == n->p->right) && (n->p == g->right)
            rotate_left(g);
        }
    }

    // ins_fixup() is iterative version of insert_case1() - insert_case5(), written by Moiseenko A.A.
    void ins_fixup(Node *n) noexcept
    {
        while(1)
        {
            if  (n->p == _nil)
            {
                n->color = eBlue;
                break;
            }
            if  (n->p->color == eBlue)
            {
                break; // Tree is still valid
            }

            Node *u = uncle(n), * g;

            if  (u != _nil && u->color == eRed)
            {
                // && (n->parent->color == RED) Второе условие проверяется в insert_case2, то есть родитель уже является красным.
                n->p->color = eBlue;
                u->color = eBlue;
                g = grandparent(n);
                g->color = eRed;
                n = g;
                continue;
            }
            g = grandparent(n);

            if  (n == n->p->right && n->p == g->left)
            {
                rotate_left(n->p);
                n = n->left;
            }
            else if (n == n->p->left && n->p == g->right)
            {
                rotate_right(n->p);
                n = n->right;
            }
            g = grandparent(n);

            n->p->color = eBlue;
            g->color = eRed;
            if  (n == n->p->left && n->p == g->left)
            {
                rotate_right(g);
            }
            else
            {
                // (n == n->p->right) && (n->p == g->right)
                rotate_left(g);
            }
            break;
        }
    }

public:
    Node * RBInsertWiki(const Key &k, const Data &d, bool bMulti) //noexcept(noexcept(Node(k, d, eRed)) && noexcept_new)
    {
        /*
        if (!bMulti)
        {
            Handle r = FindNode(k);
            if (r)
            {
                return nullptr;
            }
        }
        */
        Node * y = _nil;
        Node * x = root;
        if  (!bMulti)
        {
            while(x != _nil)
            {
                y = x;
                if  (k < x->k)
                {
                    x = x->left;
                }
                else if (x->k == k)
                {
                    return nullptr;
                }
                else
                {
                    x = x->right;
                }
            }
        }
        else
        {
            while(x != _nil)
            {
                y = x;
                if  (k < x->k)
                {
                    x = x->left;
                }
                else
                {
                    x = x->right;
                }
            }
        }
#ifdef TOOLSLIB_RB_SHARED_ALLOCATOR
        //Node * z = TL_NEW Node(k, d, eRed);
        Node* z = TL_NEW Node(k, d, eRed);
#else
        Node* z = m_pAllocator->NewEx(&m_pAllocator);
        if (!z)
        {
            throw "CMaaRBTree(): allocation error";
        }
        try
        {
            ::new_(z, Node(k, d, eRed));
        }
        catch (...)
        {
            m_pAllocator->DeleteEx(z, &m_pAllocator);
            throw;
        }
#endif

        //z->p = z->left = z->right =_nil;
        z->p = y;
        if  (y == _nil)
        {
            root = z;
        }
        else if (z->k < y->k)
        {
            y->left = z;
        }
        else
        {
            y->right = z;
        }
        z->left = _nil;
        z->right = _nil;
        z->color = eRed;
        N++;
        //RBInsertFixup(z);
        //insert_case1(z);
        ins_fixup(z);
        return z;
    }

protected:
#ifdef BOOK_EX_CODE
    void RBTransplant(Node * u, Node * v) noexcept
    {
        if  (u->p == _nil)
        {
            root = v;
        }
        else if (u == u->p->left)
        {
            u->p->left = v;
        }
        else
        {
            u->p->right = v;
        }
        v->p = u->p;
    }
#endif
public:
    /*
    Node* Root() const noexcept
    {
        return root;
    }
    */
    Node * begin() const noexcept
    {
        return root != _nil ? TreeMinimum(root) : _nil;
    }
    // next:  p = rb.TreeSuccessor(p);
    // for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreeSuccessor(p))
    Node * end() const noexcept
    {
        return _nil;
    }
    Node * rbegin() const noexcept
    {
        return root != _nil ? TreeMaximum(root) : _nil;
    }
    // prev:  p = rb.TreePredecessor(p);
    // for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreePredecessor(p))
    Node * rend() const noexcept
    {
        return _nil;
    }
    Node * TreeMinimum(Node * x) const noexcept
    {
        while(x->left != _nil)
        {
            x = x->left;
        }
        return x;
    }
    Node * TreeMaximum(Node * x) const noexcept
    {
        while(x->right != _nil)
        {
            x = x->right;
        }
        return x;
    }
    Node * TreeSuccessor(Node * x) const noexcept
    {
        if  (x->right != _nil)
        {
            return TreeMinimum(x->right);
        }
        Node * y = x->p;
        while(y != _nil && x == y->right)
        {
            x = y;
            y = y->p;
        }
        return y;
    }
    Node * TreePredecessor(Node * x) const noexcept
    {
        if  (x->left != _nil)
        {
            return TreeMaximum(x->left);
        }
        Node * y = x->p;
        while(y != _nil && x == y->left)
        {
            x = y;
            y = y->p;
        }
        return y;
    }
    Node* TreeSuccessor(Node* x, Node* p) const noexcept
    {
        if  (x->right != _nil)
        {
            return TreeMinimum(x->right);
        }
        if  (x == p)
        {
            return _nil;
        }
        Node* y = x->p;
        while(y != _nil && x == y->right)
        {
            if  (y == p)
            {
                return _nil;
            }
            x = y;
            y = y->p;
        }
        return y;
    }
    Node* TreePredecessor(Node* x, Node* p) const noexcept
    {
        if  (x->left != _nil)
        {
            return TreeMaximum(x->left);
        }
        if  (x == p)
        {
            return _nil;
        }
        Node* y = x->p;
        while(y != _nil && x == y->left)
        {
            if  (y == p)
            {
                return _nil;
            }
            x = y;
            y = y->p;
        }
        return y;
    }
    Node * RecursiveTreeSearch(Node * x, const Key &k) const noexcept(noexcept(k == k) && noexcept(k < k))
    {
        if  (x == _nil || k == x->k)
        {
            return x;
        }
        if  (k < x->k)
        {
            return RecursiveTreeSearch(x->left, k);
        }
        else
        {
            return RecursiveTreeSearch(x->right, k);
        }
    }
    Node * IteractiveTreeSearch(Node * x, const Key &k) const noexcept(noexcept(k == k) && noexcept(k < k))
    {
        while(x != _nil && !(k == x->k))
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
    Node * TreeSearchLQ(const Key &k, bool bRetEq = true, Node * x = nullptr) const noexcept(noexcept(k == k) && noexcept(k < k))
    {
        x = x ? x : root;
        Node * y;
        if  (bRetEq)
        {
            y = IteractiveTreeSearch(x, k);
            if  (y != _nil)
            {
                if  (m_bMulti)
                {
                    while(1)
                    {
                        Node* z = TreeSuccessor(y, x);
                        if  (z != _nil && k == z->k)
                        {
                            y = z;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                return y;
            }
        }
        y = _nil;

        while(x != _nil)
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
    Node * TreeSearchGQ(const Key &k, bool bRetEq = true, Node * x = nullptr) const noexcept(noexcept(k == k) && noexcept(k < k))
    {
        x = x ? x : root;
        Node * y;
        if  (bRetEq)
        {
            y = IteractiveTreeSearch(x, k);
            if  (y != _nil)
            {
                if  (m_bMulti)
                {
                    while(1)
                    {
                        Node* z = TreePredecessor(y, x);
                        if  (z != _nil && k == z->k)
                        {
                            y = z;
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                return y;
            }
        }
        y = _nil;

        while(x != _nil)
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
    Node * TreeSearchLQ1(const Key &k, bool bRetEq = true)
    {
        Node * x = root, * y = _nil;

        while(x != _nil)
        {
            if   (k == x->k)
            {
                if   (bRetEq)
                {
                    return x;
                }
                while(x != _nil && x->k == k)
                {
                    x = TreePredecessor(x);
                }
                return x;
            }
            if   (k < x->k)
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
    Node * TreeSearchGQ1(const Key &k, bool bRetEq = true)
    {
        Node * x = root, * y = _nil;

        while(x != _nil)
        {
            if   (k == x->k)
            {
                if   (bRetEq)
                {
                    return x;
                }
                while(x != _nil && x->k == k)
                {
                    x = TreeSuccessor(x);
                }
                return x;
            }
            if   (k < x->k)
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
    */
    int Find(const Key &k, Data *d = nullptr) noexcept(noexcept(IteractiveTreeSearch(root, k)) && noexcept(*d = *d))
    {
        Node * x = IteractiveTreeSearch(root, k);
        if  (x != _nil)
        {
            if  (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }
    Handle FindNode(const Key &k) const noexcept(noexcept(IteractiveTreeSearch(root, k))) //noexcept(noexcept(Key::operator==) && noexcept(Key::operator<))
    {
        Node * x = IteractiveTreeSearch(root, k);
        return x != _nil ? x : nullptr;
    }
    int Remove(const Key &k, Data *d = nullptr) noexcept(noexcept(IteractiveTreeSearch(root, k)) && !CMAA_RB_DEBUG)
    {
        Node * x = IteractiveTreeSearch(root, k);
        if  (x != _nil)
        {
            if  (d)
            {
                *d = x->d;
            }
            RBDelete(x);
            return 0;
        }
        return 1;
    }
    int FindMin(Key *k = nullptr, Data *d = nullptr) const noexcept
    {
        Node * x = TreeMinimum(root);
        if  (x != _nil)
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
    int FindMax(Key *k = nullptr, Data *d = nullptr) const noexcept
    {
        Node * x = TreeMaximum(root);
        if  (x != _nil)
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
    int FindSucc(const Key &_k, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(IteractiveTreeSearch(root, _k)) && noexcept(*k = *k) && noexcept(*d = *d))
    {
        Node * x = IteractiveTreeSearch(root, _k);
        if  (x != _nil)
        {
            x = TreeSuccessor(x);
            if  (x != _nil)
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
    int FindPred(const Key &_k, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(IteractiveTreeSearch(root, _k)) && noexcept(*k = *k) && noexcept(*d = *d))
    {
        Node * x = IteractiveTreeSearch(root, _k);
        if  (x != _nil)
        {
            x = TreePredecessor(x);
            if  (x != _nil)
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
    Handle FindSucc(Handle x) const noexcept
    {
        if  (x && x != _nil)
        {
            x = TreeSuccessor(x);
            return x != _nil ? x : nullptr;
        }
        return nullptr;
    }
    Handle FindPred(Handle x) const noexcept
    {
        if  (x && x != _nil)
        {
            x = TreePredecessor(x);
            return x != _nil ? x : nullptr;
        }
        return nullptr;
    }
    // 0 - ok
    int FindLQ(const Key &_k, bool bRetEq, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(TreeSearchLQ(_k, bRetEq, root) && noexcept(*k = *k) && noexcept(*d = *d)))
    {
        Node * x = TreeSearchLQ(_k, bRetEq, root);
        if  (x != _nil)
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
    // 0 - ok
    int FindGQ(const Key &_k, bool bRetEq, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(TreeSearchGQ(_k, bRetEq, root) && noexcept(*k = *k) && noexcept(*d = *d)))
    {
        Node * x = TreeSearchGQ(_k, bRetEq, root);
        if  (x != _nil)
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
    // 0 - ok
    int FindLQ(const Key &_k, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(FindLQ(_k, true, k, d)))
    {
        return FindLQ(_k, true, k, d);
    }
    // 0 - ok
    int FindGQ(const Key &_k, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(FindGQ(_k, true, k, d)))
    {
        return FindGQ(_k, true, k, d);
    }
    // 0 - ok
    int FindLnQ(const Key &_k, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(FindLQ(_k, true, k, d)))
    {
        return FindLQ(_k, false, k, d);
    }
    // 0 - ok
    int FindGnQ(const Key &_k, Key *k = nullptr, Data *d = nullptr) const noexcept(noexcept(FindGQ(_k, true, k, d)))
    {
        return FindGQ(_k, false, k, d);
    }
protected:
public:
    void RBDelete(Node * z) noexcept(!CMAA_RB_DEBUG)
    {
        //RBDelete1(z);
        RBDeleteWiki(z);
    }
#ifdef BOOK_EX_CODE
    // 3rd edition, bad
    void RBDelete3(Node * z)
    {
        Node * y = z;
        Node * x;
        char y_original_color = y->color;
        if  (z->left == _nil)
        {
            x = z->right;
            RBTransplant(z, z->right);
            /*
            if (y_original_color == eBlue && x == _nil && z->p != _nil && z->p->p != _nil)
            {
                LeftRotate(z->p->p);
                root->color = eBlue;
            }
            */
        }
        else if (z->right == _nil)
        {
            x = z->left;
            RBTransplant(z, z->left);
            /*
            if (y_original_color == eBlue && x == _nil && z->p != _nil && z->p->p != _nil)
            {
                RightRotate(z->p->p);
                root->color = eBlue;
            }
            */
        }
        else
        {
            y = TreeMinimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if  (y->p == z)
            {
                x->p = y;
            }
            else
            {
                RBTransplant(y, y->right);
                y->right = z->right;
                y->right->p = y;
            }
            RBTransplant(z, y);
            y->left = z->left;
            y->left->p = y;
            y->color = z->color;
        }
        if  (y_original_color == eBlue && x != _nil)
        {
            RBDeleteFixup(x);
        }
        N--;
        DeleteNode(z);
    }
    // 1st edition, bad
    void RBDelete1(Node * z)
    {
        Node * y;
        if  (z->left == _nil || z->right == _nil)
        {
            y = z;
        }
        else
        {
            y = TreeSuccessor(z);
        }
        Node * x;
        if  (y->left != _nil)
        {
            x = y->left;
        }
        else
        {
            x = y->right;
        }
        x->p = y->p;
        if  (y->p == _nil)
        {
            root = x;
        }
        else if (y == y->p->left)
        {
            y->p->left = x;
        }
        else
        {
            y->p->right = x;
        }
        char y_orig_color = y->color;
        if  (y != z)
        {
            y->p = z->p;
            y->left = z->left;
            y->right = z->right;
            y->color = z->color;
            y->left->p = y;
            y->right->p = y;
            if  (z == root)
            {
                root = y;
                root->color = eBlue;
            }
            else if (z->p->left == z)
            {
                z->p->left = y;
            }
            else
            {
                z->p->right = y;
            }
        }
        if  (y_orig_color == eBlue && x != _nil)
        {
            //__utf8_printf("fixup...\n");
            RBDeleteFixup(x);
        }
        N--;
        DeleteNode(z);
    }
#endif
    // Wiki 26-27.12.2017
    // https://ru.wikipedia.org/wiki/%D0%9A%D1%80%D0%B0%D1%81%D0%BD%D0%BE-%D1%87%D1%91%D1%80%D0%BD%D0%BE%D0%B5_%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE
    Node * Sibling(Node * n) noexcept
    {
        if  (n == n->p->left)
        {
            return n->p->right;
        }
        else
        {
            return n->p->left;
        }
    }
    void RBDeleteWiki(Node * z) noexcept(!CMAA_RB_DEBUG)
    {
        if  (z->left != _nil && z->right != _nil)
        {
            Node * y = TreeSuccessor(z);
            //swap(*y, *z);
            CMaaSwap(y->p, z->p);
            if  (y->p == _nil)
            {
                root = y;
            }
            else
            {
                if  (y->p->left == z)
                {
                    y->p->left = y;
                }
                else
                {
                    y->p->right = y;
                }
            }
            if  (z->p->left == y)
            {
                z->p->left = z;
            }
            else
            {
                z->p->right = z;
            }
            CMaaSwap(y->left, z->left);
            CMaaSwap(y->right, z->right);
            CMaaSwap(y->color, z->color);
            y->left->p = y;
            y->right->p = y;
            z->left->p = z;
            z->right->p = z;
            //root->color = eBlue;
        }
#if CMAA_RB_DEBUG
        if  (z->left != _nil && z->right != _nil)
        {
            throw "if (z->left != _nil && z->right != _nil)";
        }
#endif
        //rm0(z); // recursive fixup
        rm_fixup(z); // iterative fixup
        N--;
        DeleteNode(z);
    }
protected:
    // rm0() - rm6() - wiki source
    void rm0(Node *n) noexcept(!CMAA_RB_DEBUG)
    {
#if CMAA_RB_DEBUG
        if  (n->left != _nil && n->right != _nil)
        {
            throw "if (n->left != _nil && n->right != _nil)";
        }
#endif
        Node * c = n->left != _nil ? n->left : n->right;
        //replace_node(n, c); // n <== c
        c->p = n->p;
        if  (c->p == _nil)
        {
            root = c;
        }
        else
        {
            if  (c->p->left == n)
            {
                c->p->left = c;
            }
            else
            {
                c->p->right = c;
            }
        }
        n->left = n->right = n->p = _nil;
        if  (n->color == eBlue)
        {
            if  (c->color == eRed)
            {
                c->color = eBlue;
            }
            else
            {
                rm1(c);
            }
        }
        // delete n;
    }
    void rm1(Node * n) noexcept(!CMAA_RB_DEBUG)
    {
        if  (n->p != _nil)
        {
            rm2(n);
        }
    }
    void rm2(Node * n) noexcept(!CMAA_RB_DEBUG)
    {
        Node * s = Sibling(n);
        if  (s->color == eRed)
        {
            n->p->color = eRed;
            s->color = eBlue;
            if  (n == n->p->left)
            {
                LeftRotate(n->p);
            }
            else
            {
                RightRotate(n->p);
            }
        }
        rm3(n);
    }
    void rm3(Node * n) noexcept(!CMAA_RB_DEBUG)
    {
        Node *s = Sibling(n);

        if  (n->p->color == eBlue && s->color == eBlue &&
            s->left->color == eBlue && s->right->color == eBlue)
        {
            s->color = eRed;
            rm1(n->p);
        }
        else
        {
            rm4(n);
        }
    }
    void rm4(Node *n) noexcept(!CMAA_RB_DEBUG)
    {
        Node *s = Sibling(n);

        if  (n->p->color == eRed && s->color == eBlue &&
            s->left->color == eBlue && s->right->color == eBlue)
        {
            s->color = eRed;
            n->p->color = eBlue;
        }
        else
        {
            rm5(n);
        }
    }
    void rm5(Node *n) noexcept(!CMAA_RB_DEBUG)
    {
        Node *s = Sibling(n);

        if  (s->color == eBlue)
        {
            /* this if statement is trivial,
               due to case 2 (even though case 2 changed the sibling to a sibling's child,
               the sibling's child can't be red, since no red parent can have a red child). */
            /* the following statements just force the red to be on the left of the left of the parent,
               or right of the right, so case six will rotate correctly. */
            if  (n == n->p->left && s->right->color == eBlue && s->left->color == eRed)
            {
                /* this last test is trivial too due to cases 2-4. */
                s->color = eRed;
                s->left->color = eBlue;
                RightRotate(s);
            }
            else if (n == n->p->right && s->left->color == eBlue && s->right->color == eRed)
            {
                /* this last test is trivial too due to cases 2-4. */
                s->color = eRed;
                s->right->color = eBlue;
                LeftRotate(s);
            }
        }
        rm6(n);
    }
    void rm6(Node *n) noexcept(!CMAA_RB_DEBUG)
    {
        Node *s = Sibling(n);

        s->color = n->p->color;
        n->p->color = eBlue;

        if  (n == n->p->left)
        {
            s->right->color = eBlue;
            LeftRotate(n->p);
        }
        else
        {
            s->left->color = eBlue;
            RightRotate(n->p);
        }
    }

    // rm_fixup() is iterative version of rm0() - rm6(), written by Moiseenko A.A.
    void rm_fixup(Node *n) noexcept(!CMAA_RB_DEBUG)
    {
#if CMAA_RB_DEBUG
        if  (n->left != _nil && n->right != _nil) // assert
        {
            throw "if (n->left != _nil && n->right != _nil)";
        }
#endif
        Node * c = n->left != _nil ? n->left : n->right;
        //replace_node(n, c); // n <== c
        c->p = n->p;
        if  (c->p == _nil)
        {
            root = c;
        }
        else
        {
            if  (c->p->left == n)
            {
                c->p->left = c;
            }
            else
            {
                c->p->right = c;
            }
        }
        n->left = n->right = n->p = _nil;
        if  (n->color == eBlue)
        {
            if  (c->color == eRed)
            {
                c->color = eBlue;
            }
            else
            {
                //rm1(c);
                n = c;
                while(n->p != _nil)
                {
                    //rm2(n);

                    Node * s = Sibling(n);
                    if  (s->color == eRed)
                    {
                        n->p->color = eRed;
                        s->color = eBlue;
                        if  (n == n->p->left)
                        {
                            LeftRotate(n->p);
                        }
                        else
                        {
                            RightRotate(n->p);
                        }
                    }
                    //rm3(n);
                    s = Sibling(n);

                    if  (n->p->color == eBlue && s->color == eBlue && s->left->color == eBlue && s->right->color == eBlue)
                    {
                        s->color = eRed;
                        //rm1(n->p);
                        n = n->p;
                        continue;
                    }
                    //rm4(n);
                    //s = Sibling(n);

                    if  (n->p->color == eRed && s->color == eBlue && s->left->color == eBlue && s->right->color == eBlue)
                    {
                        s->color = eRed;
                        n->p->color = eBlue;
                    }
                    else
                    {
                        //rm5(n);
                        //Node *s = Sibling(n);

                        if  (s->color == eBlue)
                        {
                            /* this if statement is trivial,
                               due to case 2 (even though case 2 changed the sibling to a sibling's child,
                               the sibling's child can't be red, since no red parent can have a red child). */
                            /* the following statements just force the red to be on the left of the left of the parent,
                               or right of the right, so case six will rotate correctly. */
                            if  (n == n->p->left && s->right->color == eBlue && s->left->color == eRed)
                            {
                                /* this last test is trivial too due to cases 2-4. */
                                s->color = eRed;
                                s->left->color = eBlue;
                                RightRotate(s);
                            }
                            else if (n == n->p->right && s->left->color == eBlue && s->right->color == eRed)
                            {
                                /* this last test is trivial too due to cases 2-4. */
                                s->color = eRed;
                                s->right->color = eBlue;
                                LeftRotate(s);
                            }
                        }
                        //rm6(n);
                        s = Sibling(n);

                        s->color = n->p->color;
                        n->p->color = eBlue;

                        if  (n == n->p->left)
                        {
                            s->right->color = eBlue;
                            LeftRotate(n->p);
                        }
                        else
                        {
                            s->left->color = eBlue;
                            RightRotate(n->p);
                        }
                    }
                    break;
                }
            }
        }
        // delete orig n in called fn;
    }
#ifdef BOOK_EX_CODE
    // bad
    void RBDeleteFixup(Node * x) noexcept(!CMAA_RB_DEBUG)
    {
        while(x != root && x->color == eBlue)
        {
            if  (x == x->p->left)
            {
                Node * w = x->p->right;
                if  (w->color == eRed)
                {
                    w->color = eBlue;             // 1
                    x->p->color = eRed;          // 1
                    LeftRotate(x->p);           // 1
                    w = x->p->right;            // 1
                }
                if  (w == _nil)
                {
                    break; // wiki: none
                }
                // wiki: if (x->p->color == eBlue && w->color == eBlue && w->left->color == eBlue && w->right->color == eBlue)
                if  (w->left->color == eBlue && w->right->color == eBlue)
                {
                    w->color = eRed;             // 2
                    x = x->p;                   // 2
                }
                else
                {
                    // далее несоответствие
                    if  (w->right->color == eBlue)
                    {
                        w->left->color = eBlue; // n  // 3
                        // nw.color = eRed
                        w->color = eRed;             // 3
                        RightRotate(w);             // 3
                        w = x->p->right;            // 3
                    }
                    w->color = x->p->color;     // 4
                    x->p->color = eBlue;          // 4
                    w->right->color = eBlue;      // 4
                    LeftRotate(x->p);           // 4
                    x = root;                   // 4
                }
            }
            else if (x == x->p->right)
            {
                Node * w = x->p->left;
                if  (w->color == eRed)
                {
                    w->color = eBlue;             // 1
                    x->p->color = eRed;          // 1
                    RightRotate(x->p);          // 1
                    w = x->p->left;             // 1
                }
                if  (w == _nil)
                {
                    break;
                }
                if  (w->right->color == eBlue && w->left->color == eBlue)
                {
                    w->color = eRed;             // 2
                    x = x->p;                   // 2
                }
                else
                {
                    if  (w->left->color == eBlue)
                    {
                        w->right->color = eBlue; // n // 3
                        // nw.color = eRed
                        w->color = eRed;             // 3
                        LeftRotate(w);              // 3
                        w = x->p->left;             // 3
                    }
                    w->color = x->p->color;     // 4
                    x->p->color = eBlue;          // 4
                    w->left->color = eBlue;       // 4
                    RightRotate(x->p);          // 4
                    x = root;                   // 4
                }
            }
        }
        x->color = eBlue;
    }
#endif
public:
    void Swap(CMaaRBTree<Key, Data> &That) noexcept
    {
        CMaaSwap(N, That.N);
        CMaaSwap(_nil, That._nil);
        CMaaSwap(root, That.root);
#ifndef TOOLSLIB_RB_SHARED_ALLOCATOR
        CMaaSwap(m_pAllocator, That.m_pAllocator);
#endif
        CMaaSwap(m_bMulti, That.m_bMulti);
    }
    CMaaString Node2Text(Node* x) const;
    //{
    //   return CMaaString::sFormat("%d%c", x->k, x->ColorCh());
    //}
    void Print(Node * x = nullptr, int w = 79, int ll = 10) const
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
    CMaaString Print(Node * x, int w = 79, int l = 0, bool *e = nullptr) const
    {
        if  (e)
        {
            *e = false;
        }
        if  (x == _nil)
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
            //Ret.Format("%d%c", x->k, x->ColorCh());
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
        if  (x->left != _nil && x->left->p != x)
        {
            static char err[512];
            sprintf(err, "chk: %d->left error", x->k);
            throw err;
        }
        if  (x->right != _nil && x->right->p != x)
        {
            static char err[512];
            sprintf(err, "chk: %d->right error", x->k);
            throw err;
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
};

//DEF_ALLOCATOR_CMaaRBTree(int, int)
//opt:
//template<> CMaaString CMaaRBTree<int, int>::Node2Text(CMaaRBTree<int, int>::Node* x) const
//{
//    return CMaaString::sFormat("%d%c", x->k, x->ColorCh());
//}

//---------------
//#define CMAARANGES_PRINT
template<class T> class CMaaRanges
{
    CMaaRBTree<T, T> m_Tree;
    typedef CMaaRBTree<T, T>::Handle H;
    T m_Max, m_Zero;
public:
    CMaaRanges()
    :   m_Tree(false),
        m_Max{},
        m_Zero{}
    {
    }
    ~CMaaRanges()
    {
    }
    // 0 - ok
    int Add(const T& a_, const T& b_)
    {
#ifdef CMAARANGES_PRINT
        __utf8_printf("%d,%d:\n", a_, b_);
#endif
        if (a_ > b_)
        {
            return 1;
        }
        T a = a_, b = b_;
        ++b;
        if (b < b_)
        {
            m_Max = b = b_;
            if (a == b)
            {
                return 0;
            }
        }
        H n = m_Tree.TreeSearchLQ(a, true);
        if (n != m_Tree.rend())
        {
            if (n->data() >= a)
            {
                if (n->data() >= b)
                {
#ifdef CMAARANGES_PRINT
                    m_Tree.Print();
#endif
                    return 0;
                }
                a = n->key();
                /*
                if (n->data() > b)
                {
                    b = n->data();
                }
                */
            }
        }
        n = m_Tree.TreeSearchGQ(b, true);
        if (n == m_Tree.end())
        {
            n = m_Tree.rbegin();
            /*
            if (n == m_Tree.rend())
            {
                n = m_Tree.end();
            }
            */
        }
        if (n != m_Tree.end())
        {
            if (n->key() <= b && b < n->data())
            {
                b = n->data();
                if (n->key() < a)
                {
                    a = n->key();
                }
            }
        }
        while (1)
        {
            n = m_Tree.TreeSearchLQ(b, false);
            if (n != m_Tree.rend() && n->key() >= a)
            {
                m_Tree.RBDelete(n);
                continue;
            }
            break;
        }
        while (1)
        {
            n = m_Tree.TreeSearchGQ(a, true);
            if (n != m_Tree.end() && n->key() <= b)
            {
                m_Tree.RBDelete(n);
                continue;
            }
            break;
        }
        m_Tree.AddNode(a, b, false);
#ifdef CMAARANGES_PRINT
        m_Tree.Print();
#endif
        return 0;
    }
    // 0 - ok
    int Remove(T a, T b)
    {
#ifdef CMAARANGES_PRINT
        __utf8_printf("- %d,%d:\n", a, b);
#endif
        if (a > b)
        {
            return 1;
        }
        const T b_ = b;
        ++b;
        if (b < b_)
        {
            b = b_;
            m_Max = m_Zero;
            if (a == b)
            {
                return 0;
            }
        }
        H n = m_Tree.TreeSearchLQ(a, false);
        if (n != m_Tree.rend() && n->data() > a)
        {
            if (n->data() <= b)
            {
                if (n->key() < a)
                {
                    n->data() = a;
                }
                else
                {
                    m_Tree.RBDelete(n);
                }
            }
            else
            {
                T x = n->data();
                if (n->key() < a)
                {
                    n->data() = a;
                }
                else
                {
                    m_Tree.RBDelete(n);
                }
                m_Tree.AddNode(b, x);
            }
        }
        while (1)
        {
            n = m_Tree.TreeSearchLQ(b, false);
            if (n != m_Tree.rend() && n->key() >= a)
            {
                if (n->data() <= b)
                {
                    m_Tree.RBDelete(n);
                    continue;
                }
                T x = n->data();
                m_Tree.RBDelete(n);
                m_Tree.AddNode(b, x);
            }
            break;
        }
#ifdef CMAARANGES_PRINT
        m_Tree.Print();
#endif
        return 0;
    }
    // 0 - found
    int Find(const T& a) const noexcept
    {
        T a1 = a;
        ++a1;
        H n = a1 < a ? m_Tree.TreeSearchLQ(a, true) : m_Tree.TreeSearchLQ(a1, false);
        return (n != m_Tree.rend() && n->key() <= a && a < n->data()) || (a == m_Max && m_Max != m_Zero) ? 0 : 1;
    }
    const CMaaRBTree<T, T> & Tree() const noexcept
    {
        return m_Tree;
    }
    void RemoveAll() noexcept
    {
        m_Tree.RemoveAll();
        m_Max = m_Zero;
    }
    size_t GetCount() const noexcept
    {
        return m_Tree.GetCount();
    }
    void Print() const
    {
        m_Tree.Print();
    }
};

/*
template<> CMaaString CMaaRBTree<int, int>::Node2Text(CMaaRBTree<int, int>::Node* x) const
{
    return CMaaString::sFormat("[%d,%d)%c", x->k, x->d, x->ColorCh());
}
*/
//---------------
//#define CMAACOLORRANGES_PRINT
template<class T, class C> class CMaaColorRanges
{
public:
    struct Data
    {
        T d;
        C c;
        Data() noexcept
        //:   d(0), c(0)
        {
        }
        Data(T t_, C c_) noexcept
        :   d(t_), c(c_)
        {
        }
        Data(const Data& That) noexcept
        :   d(That.d), c(That.c)
        {
        }
    };
protected:
    CMaaRBTree<T, Data> m_Tree;
    typedef CMaaRBTree<T, Data>::Handle H;
    T m_Max, m_Zero;
    C m_c0, m_cErr, m_cMax;
public:
    CMaaColorRanges(C c0, C cErr)
    :   m_Tree(false),
        m_Max{},
        m_Zero{},
        m_c0(c0),
        m_cErr(cErr),
        m_cMax(c0)
    {
    }
    ~CMaaColorRanges()
    {
    }
    // 0 - ok
    int Add(const T& a_, const T& b_, C c)
    {
#ifdef CMAACOLORRANGES_PRINT
        __utf8_printf("%d,%d/%c:\n", a_, b_, c);
#endif
        if (a_ > b_)
        {
            return 1;
        }
        T a = a_, b = b_;
        ++b;
        if (b < b_)
        {
            m_Max = b = b_;
            m_cMax = m_cMax == m_c0 || m_cMax == c ? c : m_cErr;
            if (a == b)
            {
                return 0;
            }
        }
        while (a < b)
        {
            H n = m_Tree.TreeSearchLQ(a, true);
            if (n != m_Tree.rend() && n->data().d >= a)
            {
                H n2 = m_Tree.TreeSearchGQ(a, false);
                if (n2 == m_Tree.end() || n2->key() > b)
                {
                    if (n->data().c == c)
                    {
                        if (n->data().d < b)
                        {
                            n->data().d = b;
                        }
                    }
                    else
                    {
                        if (n->key() == a)
                        {
                            if (n->data().d <= b)
                            {
                                n->data().d = b;
                                n->data().c = m_cErr;
                                Merge_cErr(a);
                            }
                            else
                            {
                                T b2 = n->data().d;
                                n->data().d = b;
                                n->data().c = m_cErr;
                                m_Tree.AddNode(b, Data(b2, n->data().c), false);
                                Merge_cErr(a);
                            }
                        }
                        else
                        {
                            // n->key() < a
                            // n->data().d >= a
                            // n->data().c != c
                            if (n->data().d <= a)
                            {
                                m_Tree.AddNode(a, Data(b, c), false);
                            }
                            else if (n->data().d <= b)
                            {
                                T b2 = n->data().d;
                                n->data().d = a;
                                m_Tree.AddNode(a, Data(b2, m_cErr), false);
                                if (b2 < b)
                                {
                                    m_Tree.AddNode(b2, Data(b, c), false);
                                }
                                Merge_cErr(a);
                            }
                            else
                            {
                                T b2 = n->data().d;
                                n->data().d = a;
                                m_Tree.AddNode(b, Data(b2, n->data().c), false);
                                m_Tree.AddNode(a, Data(b, m_cErr), false);
                                Merge_cErr(a);
                            }
                        }
                    }
                    break;
                }
                else
                {
                    // n->key() <= a
                    // n->data().d >= a
                    // n2->key() > a
                    // n2->key() <= b

                    if (n->data().c == c)
                    {
                        n->data().d = a = n2->key();
                        if (n2->data().c == c)
                        {
                            n->data().d = n2->data().d;
                            m_Tree.RBDelete(n2);
                        }
                    }
                    else
                    {
                        if (n->data().d > a)
                        {
                            T b2 = n->data().d;
                            if (n->key() < a)
                            {
                                n->data().d = a;
                                m_Tree.AddNode(a, Data(b2, m_cErr), false);
                            }
                            else
                            {
                                n->data().c = m_cErr;
                            }
                            Merge_cErr(a);
                            a = b2;
                        }
                        else
                        {
                            if (n2->data().c == c)
                            {
                                T b2 = n2->data().d;
                                m_Tree.RBDelete(n2);
                                m_Tree.AddNode(a, Data(b2, c), false);
                                a = b2;
                            }
                            else
                            {
                                T a2 = n2->key();
                                m_Tree.AddNode(a, Data(a2, c), false);
                                a = a2;
                            }
                        }
                    }
                    // continue;
                }
            }
            else
            {
                n = m_Tree.TreeSearchGQ(a, false);
                if (n == m_Tree.end() || n->key() > b)
                {
                    m_Tree.AddNode(a, Data(b, c), false);
                    break;
                }
                // n->key() <= b
                if (n->data().c == c)
                {
                    T b2 = n->data().d;
                    m_Tree.RBDelete(n);
                    m_Tree.AddNode(a, Data(b2, c), false);
                    a = b2;
                }
                else
                {
                    m_Tree.AddNode(a, Data(n->key(), c), false);
                    a = n->data().d;
                }
            }
        }

#ifdef CMAACOLORRANGES_PRINT
        m_Tree.Print();
#endif
        return 0;
    }

    void Merge_cErr(const T& a_)
    {
        T a = a_;
        while (1)
        {
            H n = m_Tree.FindNode(a);
            if (n && n->data().c == m_cErr)
            {
                H n2 = m_Tree.TreeSearchLQ(a, false);
                if (n2 != m_Tree.rend() && n2->data().c == m_cErr && n2->data().d == n->key())
                {
                    n2->data().d = n->data().d;
                    a = n2->key();
                    m_Tree.RBDelete(n);
                    continue;
                }
            }
            break;
        }
        while (1)
        {
            H n = m_Tree.FindNode(a);
            if (n && n->data().c == m_cErr)
            {
                H n2 = m_Tree.TreeSearchGQ(a, false);
                if (n2 != m_Tree.end() && n2->data().c == m_cErr && n2->key() == n->data().d)
                {
                    n->data().d = n2->data().d;
                    m_Tree.RBDelete(n2);
                    continue;
                }
            }
            break;
        }
    }
    // 0 - ok
    int Remove(const T &a_, const T &b_)
    {
#ifdef CMAACOLORRANGES_PRINT
        __utf8_printf("- %d,%d:\n", a_, b_);
#endif
        if (a_ > b_)
        {
            return 1;
        }
        T a = a_, b = b_;
        ++b;
        if (b < b_)
        {
            b = b_;
            m_Max = m_Zero;
            m_cMax = m_c0;
            if (a == b)
            {
                return 0;
            }
        }
        H n = m_Tree.TreeSearchLQ(a, false);
        if (n != m_Tree.rend() && n->data().d > a)
        {
            if (n->data().d <= b)
            {
                if (n->key() < a)
                {
                    n->data().d = a;
                }
                else
                {
                    m_Tree.RBDelete(n);
                }
            }
            else
            {
                Data x = n->data();
                if (n->key() < a)
                {
                    n->data().d = a;
                }
                else
                {
                    m_Tree.RBDelete(n);
                }
                m_Tree.AddNode(b, x);
            }
        }
        while (1)
        {
            n = m_Tree.TreeSearchLQ(b, false);
            if (n != m_Tree.rend() && n->key() >= a)
            {
                if (n->data().d <= b)
                {
                    m_Tree.RBDelete(n);
                    continue;
                }
                Data x = n->data();
                m_Tree.RBDelete(n);
                m_Tree.AddNode(b, x);
            }
            break;
        }
#ifdef CMAACOLORRANGES_PRINT
        m_Tree.Print();
#endif
        return 0;
    }
    // ret color code, m_c0 - not found
    int Find(const T& a) const noexcept
    {
        T a1 = a;
        ++a1;
        H n = a1 < a ? m_Tree.TreeSearchLQ(a, true) : m_Tree.TreeSearchLQ(a1, false);
        return (n != m_Tree.rend() && n->key() <= a && a < n->data().d) ? n->data().c : (a == m_Max && m_Max != m_Zero) ? m_cMax : m_c0;
    }
    const CMaaRBTree<T, Data>& Tree() const noexcept
    {
        return m_Tree;
    }
    void RemoveAll() noexcept
    {
        m_Tree.RemoveAll();
        m_Max = m_Zero;
    }
    size_t GetCount() const noexcept
    {
        return m_Tree.GetCount();
    }
    void Print() const
    {
        m_Tree.Print();
    }
};
/*
template<> CMaaString CMaaRBTree<int, CMaaColorRanges<int>::Data>::Node2Text(CMaaRBTree<int, CMaaColorRanges<int>::Data>::Node* x) const
{
    return CMaaString::sFormat("[%d,%d)%c%c", x->k, x->d.d, x->d.c, x->ColorCh());
}
template<> CMaaString CMaaRBTree<_IP, CMaaColorRanges<_IP>::Data>::Node2Text(CMaaRBTree<_IP, CMaaColorRanges<_IP>::Data>::Node* x) const
{
    return CMaaString::sFormat("[%I,%I)%d%c", x->k, x->d.d, x->d.c, x->ColorCh());
}
*/

#if 0
DEF_ALLOCATOR_CMaaRBTree(int, int)

int gii = 0;
int test2()
{
    try
    {
        for (int c = 0; c < 1; c++)
        {
            //__utf8_printf("%79s\rc=%d  ", "", c);
            __utf8_printf("\rc=%d  ", c);
            const int N = 10000;
            const int VMax = 2 * N - N + N / 2;
            const int Dups = 1000;
            CMaaPtr_<int, 1> x(N);
            CMaaUnivHash<int, int> h;
            int i, j;
            __utf8_printf("gen ");
            for (i = 0; i < N; i++)
            {
                while(1)
                {
                    unsigned r = -1;
                    GetRnd(&r, sizeof(r));
                    //x[i] = rand() % VMax;
                    x[i] = i;//r % VMax;
                    //if (!h.Add(x[i], i))
                    {
                        break;
                    }
                }
            }
            __utf8_printf("shuff ");
            for (i = 0; i < N; i++)
            {
                unsigned r = 0;
                GetRnd(&r, sizeof(r));
                r %= N;
                int tmp = x[i];
                x[i] = x[(int)r];
                x[(int)r] = tmp;
            }
            CMaaRBTree<int, int> t;
            __utf8_printf("ins ");
            for (j = 0; j < Dups; j++)
            for (i = 0; i < N; i++)
            {
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\ri[%d]=%d:", i, x[i]);
                }
                /*
                 if (i >= 4)
                 {
                    gii++;
                 }
                 */
                t.Insert(x[i],i);

                if  (i % 10 == 0 && i > 0 && false)
                {
                    int xx = t.Remove(x[i - 10]);
                    x[i - 10] = -1;
                    //__utf8_printf("\ri[%d]=%d:", i, x[i]);
                }
                //t.Print();
                //getch();
            }

            __utf8_printf("LQ ");

            for (i = -1; i <= N; i++)
            {
                /*if (x[i] == -1)
                 {
                     continue;
                 }*/
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int k = -1, d = -1, r = -1;
                if  ((r = t.FindLQ(i, &k, &d)) || k != i)
                {
                    if  (i == -1 && r == 1)
                    {
                    }
                    else if (i == N && r == 0 && k == N - 1)
                    {
                    }
                    else
                    {
                        __utf8_printf("fLQ[%d]:%d %d ", i, r, k);
                    }
                    //__utf8_printf("-err\n");
                }
            }

            __utf8_printf("GQ ");

            for (i = -1; i <= N; i++)
            {
                /*if (x[i] == -1)
                 {
                     continue;
                 }*/
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int k = -1, d = -1, r = -1;
                if  ((r = t.FindGQ(i, &k, &d)) || k != i)
                {
                    if  (i == N && r == 1)
                    {
                    }
                    else if (i == -1 && r == 0 && k == 0)
                    {
                    }
                    else
                    {
                        __utf8_printf("fGQ[%d]:%d %d ", i, r, k);
                    }
                }
            }

            __utf8_printf("LnQ ");

            for (i = -1; i <= N + 1; i++)
            {
                /*if (x[i] == -1)
                 {
                     continue;
                 }*/
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int k = -1, d = -1, r = -1;
                if  ((r = t.FindLnQ(i, &k, &d)) || k != i - 1)
                {
                    if  ((i == -1 || i == 0) && r == 1)
                    {
                    }
                    else if (i == N + 1 && r == 0 && k == N - 1)
                    {
                    }
                    else
                    {
                        __utf8_printf("fLnQ[%d]:%d %d ", i, r, k);
                    }
                }
            }

            __utf8_printf("GnQ ");

            for (i = -2; i <= N; i++)
            {
                /*if (x[i] == -1)
                 {
                     continue;
                 }*/
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int k = -1, d = -1, r = -1;
                if  ((r = t.FindGnQ(i, &k, &d)) || k != i + 1)
                {
                    if  ((i == N -1 || i == N) && r == 1)
                    {
                    }
                    else if (i == -2 && r == 0 && k == 0)
                    {
                    }
                    else
                    {
                        __utf8_printf("fGnQ[%d]:%d %d ", i, r, k);
                    }
                }
            }

            __utf8_printf("Succ ");

            for (i = -1; i <= N; i++)
            {
                /*if (x[i] == -1)
                 {
                     continue;
                 }*/
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int k = -1, d = -1, r = -1;
                if  ((r = t.FindSucc(i, &k, &d)) || (k != i + 1 && Dups == 1) || (k != i + 1 && k != i && Dups > 1))
                {
                    if  ((i == -1 || i == N) && r == 1)
                    {
                    }
                    else if (i == N - 1 && r == 2)
                    {
                    }
                    else
                    {
                        __utf8_printf("Succ[%d]:%d ", i, r);
                    }
                }
            }

            __utf8_printf("Pred ");

            for (i = -1; i <= N; i++)
            {
                /*if (x[i] == -1)
                 {
                     continue;
                 }*/
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int k = -1, d = -1, r = -1;
                if  ((r = t.FindPred(i, &k, &d)) || (k != i - 1 && Dups == 1) || (k != i - 1 && k != i && Dups > 1))
                {
                    if  ((i == -1 || i == N) && r == 1)
                    {
                    }
                    else if (i == 0 && r == 2)
                    {
                    }
                    else
                    {
                        __utf8_printf("Pred[%d]:%d ", i, r);
                    }
                }
            }

            if  (0)
            {
                __utf8_printf("\n");
                int a = N / 2;
                // a = 0;
                // a = N - 1;
                CMaaRBTree<int, int>::Handle h = t.FindNode(a), h1, h2;
                __utf8_printf("%p %d %d\n", h, t.k(h), t.d(h));
                h1 = h2 = h;
                for (i = 0; i < Dups + 1; i++)
                {
                    h1 = t.FindSucc(h1);
                    __utf8_printf("Succ %p %d %d\n", h1, h1 ? t.k(h1) : -9999, h1 ? t.d(h1) : -8888);
                }
                for (i = 0; i < Dups + 1; i++)
                {
                    h2 = t.FindPred(h2);
                    __utf8_printf("Pred %p %d %d\n", h2, h2 ? t.k(h2) : -9999, h2 ? t.d(h2) : -8888);
                }
            }

            //__utf8_printf("\n");
            //t.Print();
            __utf8_printf("find ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int ii = -1;
                if  (t.Find(x[i], &ii))// || ii != i)
                {
                    __utf8_printf("\rf[%d]=%d:", i, x[i]);
                    __utf8_printf("-err\n");
                }
            }
            //__utf8_printf("\n");
            __utf8_printf("shuff ");
            for (i = 0; i < N; i++)
            {
                unsigned r = 0;
                GetRnd(&r, sizeof(r));
                r %= N;
                int tmp = x[i];
                x[i] = x[(int)r];
                x[(int)r] = tmp;
            }
            __utf8_printf("find ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int ii = -1;
                if  (t.Find(x[i], &ii))// || ii != i)
                {
                    __utf8_printf("\rf[%d]=%d:", i, x[i]);
                    __utf8_printf("-err\n");
                }
            }
            __utf8_printf("rem ");
            for (j = 0; j < Dups; j++)
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                //__utf8_printf("r %d: ", i);
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rr[%d]=%d:", i, x[i]);
                }
                if  (i >= 3)
                {
                    gii++;
                }
                int xx = t.Remove(x[i]);
                //__utf8_printf("%d\n", x);
                if  (xx)
                {
                    __utf8_printf("\rr[%d]=%d:", i, x[i]);
                    __utf8_printf("-err\n");
                }
                else
                {
                    //__utf8_printf("\n");
                }

                //t.Print();
                //getch();
            }
            //__utf8_printf("\n");
            //__utf8_printf("delete array\n");
            //delete [] x;
            __utf8_printf("Empty:\r");
            t.Print();
            //__utf8_printf("\n");
        }
        __utf8_printf("\n");
        return 0;
    }
    catch(CMaaString e)
    {
        __utf8_printf("Error: %s\n", (const char *)e);
    }
    catch(const char * e)
    {
        __utf8_printf("Error: %s\n", e);
    }
    catch(...)
    {
        __utf8_printf("catch(...)\n");
    }
    return 1;
}

int test1()
{
    try
    {
        for (int c = 0; c < 10000; c++)
        {
            __utf8_printf("%79s\rc=%d  ", "", c);
            const int N = 100;
            const int VMax = 2 * N - N + N / 2;
            CMaaPtr_<int, 1> x(N);
            CMaaUnivHash<int, int> h;
            int i;
            __utf8_printf("gen rand  ");
            for (i = 0; i < N; i++)
            {
                while(1)
                {
                    unsigned r = -1;
                    GetRnd(&r, sizeof(r));
                    //x[i] = rand() % VMax;
                    x[i] = r % VMax;
                    //if (!h.Add(x[i], i))
                    {
                        break;
                    }
                }
            }
            CMaaRBTree<int, int> t;
            __utf8_printf("insert/remove ");
            for (i = 0; i < N; i++)
            {
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\ri[%d]=%d:", i, x[i]);
                }
                /*
                 if (i >= 4)
                 {
                    gii++;
                 }
                 */
                t.Insert(x[i],i);

                if  (i % 10 == 0 && i > 0)
                {
                    int xx = t.Remove(x[i - 10]);
                    x[i - 10] = -1;
                    //__utf8_printf("\ri[%d]=%d:", i, x[i]);
                }
                //t.Print();
                //getch();
            }
            //__utf8_printf("\n");
            //t.Print();
            __utf8_printf("find ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int ii = -1;
                if  (t.Find(x[i], &ii))// || ii != i)
                {
                    __utf8_printf("\rf[%d]=%d:", i, x[i]);
                    __utf8_printf("-err\n");
                }
            }
            //__utf8_printf("\n");
            __utf8_printf("shuffle ");
            for (i = 0; i < N; i++)
            {
                unsigned r = 0;
                GetRnd(&r, sizeof(r));
                r %= N;
                int tmp = x[i];
                x[i] = x[(int)r];
                x[(int)r] = tmp;
            }
            __utf8_printf("find ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rf[%d]=%d:", i, x[i]);
                }
                int ii = -1;
                if  (t.Find(x[i], &ii))// || ii != i)
                {
                    __utf8_printf("\rf[%d]=%d:", i, x[i]);
                    __utf8_printf("-err\n");
                }
            }
            __utf8_printf("remove ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                //__utf8_printf("r %d: ", i);
                if  (i % 1000 == 0)
                {
                    //__utf8_printf("\rr[%d]=%d:", i, x[i]);
                }
                if  (i >= 3)
                {
                    gii++;
                }
                int xx = t.Remove(x[i]);
                //__utf8_printf("%d\n", x);
                if  (xx)
                {
                    __utf8_printf("\rr[%d]=%d:", i, x[i]);
                    __utf8_printf("-err\n");
                }
                else
                {
                    //__utf8_printf("\n");
                }

                //t.Print();
                //getch();
            }
            //__utf8_printf("\n");
            //__utf8_printf("delete array\n");
            //delete [] x;
            __utf8_printf("Empty:\r");
            t.Print();
            //__utf8_printf("\n");
        }
        __utf8_printf("\n");
        return 0;
    }
    catch(CMaaString e)
    {
        __utf8_printf("Error: %s\n", (const char *)e);
    }
    catch(const char * e)
    {
        __utf8_printf("Error: %s\n", e);
    }
    catch(...)
    {
        __utf8_printf("catch(...)\n");
    }
    return 1;
}

int fix_12_03_2022()
{
    CMaaRBTree<int, int> t;
    t.Add(5, 51);
    t.Add(5, 52);
    t.Add(5, 53);
    t.Add(5, 54);
    t.Add(5, 55);
    t.Add(1, 11);
    t.Add(10, 101);
    t.Print();
    //int l = 6, g = 4;
    int l = 5, g = 5;
    CMaaRBTree<int, int>::Handle h = t.TreeSearchLQ(l);
    if  (h != t.begin() && h->c_key() != l)
    {
        __utf8_printf("%d ", h->c_data());
    }
    //while (h != t.begin() && h->k == l)
    while(h != t.begin() && h->c_key() == l)
    {
        __utf8_printf("%d ", h->c_data());
        h = t.TreePredecessor(h);
    }
    __utf8_printf("\n");
    h = t.TreeSearchGQ(g);
    if  (h != t.end() && h->c_key() != g)
    {
        __utf8_printf("%d ", h->c_data());
    }
    //while (h != t.end() && h->k == g)
    while(h != t.end() && h->c_key() == g)
    {
        __utf8_printf("%d ", h->c_data());
        h = t.TreeSuccessor(h);
    }
    __utf8_printf("\n");
    return 0;
}

#endif

//DEF_ALLOCATOR_CMaaRBTree(int, int)
