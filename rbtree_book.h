
// ToolsLib Project

/* CryptLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko and MaaSoftware (JSK, Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/rbtree.h

/* Copyright (C) 2002-2015 Andrey A. Moiseenko (support@maasoftware.ru)
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
 * Copyright remains Andrey A. Moiseenko's, and as such any Copyright notices in
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

template <class Key, class Data> class CMaaRBTree
{
protected:
    class Node
    {
    public:
        char color;
        Node * p, * left, * right;
        Key k;
        Data d;

        Node(const Key &_k, const Data &_d, int _c = 0)
        :   p(NULL),
            left(NULL),
            right(NULL),
            color(_c),
            k(_k),
            d(_d)
        {
        }
        Key key()
        {
            return k;
        }
        Data & data()
        {
            return d;
        }
        const Key & c_key() const
        {
            return k;
        }
        const Data & c_data() const
        {
            return d;
        }
        ADD_UL_ALLOCATOR(Node)
    };
public:
    typedef Node * Handle;
    static Key key(Handle x)
    {
        return x->key();
    }
    static Data & data(Handle x)
    {
        return x->data();
    }
    static const Key & c_key(Handle x)
    {
        return x->c_key();
    }
    static const Data & c_data(Handle x)
    {
        return x->c_data();
    }
    Key k(Handle x)
    {
        return x->key();
    }
    Data & d(Handle x)
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
    size_t N;
    Node * _nil, * root;

public:
    CMaaRBTree()
    {
        N = 0;
        _nil = (Node *)new char[sizeof(Node)];
        if  (!_nil)
        {
            throw "CMaaRBTree(): allocation error";
        }
        memset(_nil, 0, sizeof(Node));
        _nil->color = 'B';
        root = _nil;
    }
    void SimpleFree(Node * x)
    {
        if  (x != _nil)
        {
            if  (x->left != _nil)
            {
                SimpleFree(x->left);
            }
            if  (x->right != _nil)
            {
                SimpleFree(x->right);
            }
            delete x;
        }
    }
    ~CMaaRBTree()
    {
        SimpleFree(root);
        delete [] (char *)_nil;
    }
    size_t GetCount() const
    {
        return N;
    }
protected:
    //
    //       |        <--LeftRotate(x)--         |
    //       Y         --RightRotate(y)-->       X
    //      / \                                 / \
    //     X   g                               a   Y
    //    / \                                     / \
    //   a   b                                   b   g
    //
    void LeftRotate(Node * x)
    {
        if  (x->right == _nil)
        {
            throw "CMaaRBTree::LeftRotate(): x->right == _nil";
        }
        // x->right != NULL
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
    void RightRotate(Node * y)
    {
        if  (y->left == _nil)
        {
            throw "CMaaRBTree::RightRotate(): y->left == _nil";
        }
        // y->left != NULL
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
    Handle Add(const Key &k, const Data &d)
    {
        return RBInsert(k, d) ? 0 : 1;
    }
    Handle AddNode(const Key &k, const Data &d)
    {
        return RBInsert(k, d);
    }
    Node * Insert(const Key &k, const Data &d)
    {
        return RBInsert(k, d);
    }
    // RB-Insert(T,z)
    Node * RBInsert(const Key &k, const Data &d)
    {
        Node * z = new Node(k, d, 'R');
        //z->p = z->left = z->right =_nil;
        Node * y = _nil;
        Node * x = root;
        while(x != _nil)
        {
            y = x;
            if  (z->k < x->k)
            {
                x = x->left;
            }
            else
            {
                x = x->right;
            }
        }
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
        z->color = 'R';
        N++;
        //Print();
        RBInsertFixup(z);
        return z;
    }
protected:
    void RBInsertFixup(Node *z)
    {
        while(z->p->color == 'R')
        {
            if  (z->p == root)
            {
                throw "CMaaRBTree::RBInsertFixup(): (z->p == root)";
            }
            if  (z->p == z->p->p->left)
            {
                Node * y = z->p->p->right;
                if  (y->color == 'R')
                {
                    z->p->color = 'B';
                    y->color = 'B';
                    z->p->p->color = 'R';
                    z = z->p->p;
                    //__utf8_printf(">1<\n");
                    //Print();
                    continue;
                }
                if  (z == z->p->right)
                {
                    z = z->p;
                    LeftRotate(z);
                    //__utf8_printf(">2<\n");
                    //Print();
                }
                z->p->color = 'B';
                z->p->p->color = 'R';
                RightRotate(z->p->p);
                //__utf8_printf(">3<\n");
                //Print();
                break;
            }
            else if (z->p == z->p->p->right)
            {
                Node * y = z->p->p->left;
                if  (y->color == 'R')
                {
                    z->p->color = 'B';
                    y->color = 'B';
                    z->p->p->color = 'R';
                    z = z->p->p;
                    //__utf8_printf(">1<\n");
                    //Print();
                    continue;
                }
                if  (y->color == 'B' && z == z->p->left)
                {
                    z = z->p;
                    RightRotate(z);
                    //__utf8_printf(">2<\n");
                    //Print();
                }
                z->p->color = 'B';
                z->p->p->color = 'R';
                LeftRotate(z->p->p);
                //__utf8_printf(">3<\n");
                //Print();
                break;
            }
            else
            {
                throw "CMaaRBTree::RBInsertFixup(): (z->p != z->p->p->left) && (z->p != z->p->p->right)";
            }
        }
        root->color = 'B';
    }
protected:
    void RBTransplant(Node * u, Node * v)
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
public:
    Node * begin() const
    {
        return TreeMinimum(root);
    }
    // next:  p = rb.TreeSuccessor(p);
    // for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreeSuccessor(p))
    Node * end() const
    {
        return _nil;
    }
    Node * rbegin() const
    {
        return TreeMaximum(root);
    }
    // prev:  p = rb.TreePredecessor(p);
    // for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreePredecessor(p))
    Node * rend() const
    {
        return _nil;
    }
    Node * TreeMinimum(Node * x) const
    {
        while(x->left != _nil)
        {
            x = x->left;
        }
        return x;
    }
    Node * TreeMaximum(Node * x) const
    {
        while(x->right != _nil)
        {
            x = x->right;
        }
        return x;
    }
    Node * TreeSuccessor(Node * x) const
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
    Node * TreePredecessor(Node * x) const
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
    Node * RecursiveTreeSearch(Node * x, const Key &k) const
    {
        if  (x == _nil || k == x->k)
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
    Node * IteractiveTreeSearch(Node * x, const Key &k) const
    {
        while(x != _nil && k != x->k)
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
    Node * TreeSearchLQ(const Key &k, bool bRetEq = true, Node * x = NULL) const
    {
        x = x ? x : root;
        Node * y;
        if  (bRetEq)
        {
            y = IteractiveTreeSearch(x, k);
            if  (y != _nil)
            {
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
    Node * TreeSearchGQ(const Key &k, bool bRetEq = true, Node * x = NULL) const
    {
        x = x ? x : root;
        Node * y;
        if  (bRetEq)
        {
            y = IteractiveTreeSearch(x, k);
            if  (y != _nil)
            {
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
    int Find(const Key &k, Data *d = NULL)
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
    Handle FindNode(const Key &k) const
    {
        Node * x = IteractiveTreeSearch(root, k);
        return x != _nil ? x : NULL;
    }
    int Remove(const Key &k, Data *d = NULL)
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
    int FindMin(Key *k = NULL, Data *d = NULL) const
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
    int FindMax(Key *k = NULL, Data *d = NULL) const
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
    int FindSucc(const Key &_k, Key *k = NULL, Data *d = NULL) const
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
    int FindPred(const Key &_k, Key *k = NULL, Data *d = NULL) const
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
    Handle FindSucc(Handle x) const
    {
        if  (x && x != _nil)
        {
            x = TreeSuccessor(x);
            return x != _nil ? x : NULL;
        }
        return NULL;
    }
    Handle FindPred(Handle x) const
    {
        if  (x && x != _nil)
        {
            x = TreePredecessor(x);
            return x != _nil ? x : NULL;
        }
        return NULL;
    }
    int FindLQ(const Key &_k, bool bRetEq, Key *k = NULL, Data *d = NULL) const
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
    int FindGQ(const Key &_k, bool bRetEq, Key *k = NULL, Data *d = NULL) const
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
    int FindLQ(const Key &_k, Key *k = NULL, Data *d = NULL) const
    {
        return FindLQ(_k, true, k, d);
    }
    int FindGQ(const Key &_k, Key *k = NULL, Data *d = NULL) const
    {
        return FindGQ(_k, true, k, d);
    }
    int FindLnQ(const Key &_k, Key *k = NULL, Data *d = NULL) const
    {
        return FindLQ(_k, false, k, d);
    }
    int FindGnQ(const Key &_k, Key *k = NULL, Data *d = NULL) const
    {
        return FindGQ(_k, false, k, d);
    }
protected:
public:
    void RBDelete(Node * z)
    {
        RBDelete1(z);
    }
    // 3rd edition
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
            if (y_original_color == 'B' && x == _nil && z->p != _nil && z->p->p != _nil)
            {
                LeftRotate(z->p->p);
                root->color = 'B';
            }
            */
        }
        else if (z->right == _nil)
        {
            x = z->left;
            RBTransplant(z, z->left);
            /*
            if (y_original_color == 'B' && x == _nil && z->p != _nil && z->p->p != _nil)
            {
                RightRotate(z->p->p);
                root->color = 'B';
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
        if  (y_original_color == 'B' && x != _nil)
        {
            RBDeleteFixup(x);
        }
        N--;
        delete z;
    }
    // 1st edition
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
                root->color = 'B';
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
        if  (y_orig_color == 'B' && x != _nil)
        {
            //__utf8_printf("fixup...\n");
            RBDeleteFixup(x);
        }
        N--;
        delete z;
    }
protected:
    void RBDeleteFixup(Node * x)
    {
        while(x != root && x->color == 'B')
        {
            if  (x == x->p->left)
            {
                Node * w = x->p->right;
                if  (w->color == 'R')
                {
                    w->color = 'B';             // 1
                    x->p->color = 'R';          // 1
                    LeftRotate(x->p);           // 1
                    w = x->p->right;            // 1
                }
                if  (w == _nil)
                {
                    break;
                }
                if  (w->left->color == 'B' && w->right->color == 'B')
                {
                    w->color = 'R';             // 2
                    x = x->p;                   // 2
                }
                else
                {
                    if  (w->right->color == 'B')
                    {
                        w->left->color = 'B'; // n  // 3
                        // nw.color = 'R'
                        w->color = 'R';             // 3
                        RightRotate(w);             // 3
                        w = x->p->right;            // 3
                    }
                    w->color = x->p->color;     // 4
                    x->p->color = 'B';          // 4
                    w->right->color = 'B';      // 4
                    LeftRotate(x->p);           // 4
                    x = root;                   // 4
                }
            }
            else if (x == x->p->right)
            {
                Node * w = x->p->left;
                if  (w->color == 'R')
                {
                    w->color = 'B';             // 1
                    x->p->color = 'R';          // 1
                    RightRotate(x->p);          // 1
                    w = x->p->left;             // 1
                }
                if  (w == _nil)
                {
                    break;
                }
                if  (w->right->color == 'B' && w->left->color == 'B')
                {
                    w->color = 'R';             // 2
                    x = x->p;                   // 2
                }
                else
                {
                    if  (w->left->color == 'B')
                    {
                        w->right->color = 'B'; // n // 3
                        // nw.color = 'R'
                        w->color = 'R';             // 3
                        LeftRotate(w);              // 3
                        w = x->p->left;             // 3
                    }
                    w->color = x->p->color;     // 4
                    x->p->color = 'B';          // 4
                    w->left->color = 'B';       // 4
                    RightRotate(x->p);          // 4
                    x = root;                   // 4
                }
            }
        }
        x->color = 'B';
    }
public:
    void Swap(CMaaRBTree<Key, Data> &That)
    {
        CMaaSwap(N, That.N);
        CMaaSwap(_nil, That._nil);
        CMaaSwap(root, That.root);
    }
    void Print(Node * x = NULL, int w = 79, int ll = 10)
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
    CMaaString Print(Node * x, int w = 79, int l = 0, bool *e = NULL)
    {
        if  (e)
        {
            *e = false;
        }
        if  (x == _nil)
        {
            CMaaString sp(NULL, w);
            sp.Fill(' ');
            if  (e)
            {
                *e = true;
            }
            return sp;
        }
        if  (l == 0)
        {
            CMaaString Ret;
            Ret.Format("%d%c", x->k, (char)x->color);
            int w2 = w - Ret.Length();
            if  (w2 < 2)
            {
                w2 = 2;
            }
            int w1 = w2 / 2;
            w2 -= w1;
            CMaaString sp1(NULL, w1), sp2(NULL, w2);
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

//template<> CMaaFixedAllocator<CMaaRBTree<int, int>::Node> * CMaaRBTree<int, int>::Node::s_pAllocator = NULL;

#if 0
template<> CMaaFixedAllocator<CMaaRBTree<int, int>::Node> * CMaaRBTree<int, int>::Node::s_pAllocator = NULL;
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
            CMaaPtr<int> x(N);
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
            CMaaPtr<int> x(N);
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


#endif

//template<> CMaaFixedAllocator<CMaaRBTree<int, int>::Node> * CMaaRBTree<int, int>::Node::s_pAllocator = NULL;
