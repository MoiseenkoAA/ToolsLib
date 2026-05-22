
int main_SkipListTest();
int main_SkipListTestSort(int N = 10000);

// Skip List - медленноватый список с пропусками
/*
  По тестам N=30000
tests:
  6.793618 us       3,977,216  skiplist
  0.007562 us       5,328,896  rbtree
  0.003666 us       5,570,560  qsort
*/

template<class Key, class Data> class CMaaSkipList
{
private:

    struct Node
    {
        Key k;
        Data d;
        // pointers to successor nodes
        CMaaPtr<Node *> next; // forward

        Node(Key _k, Data _d, int level)
        :   k(_k),
            d(_d),
            next(level)
        {
        }
        Node(Key _k, Data _d, int level, Node *ptr)
        :   k(_k),
            d(_d),
            next(level)
        {
            for (int i = 0; i < level; i++)
            {
                next[i] = ptr;
            }
        }
        int Level() const
        {
            return (int)next.MaxIndex();
        }
    };

    // data members
    float m_probability;
    int m_maxLevel;
    Node* head; // pointer to first node
    Node* NIL;  // last node

public:
    CMaaSkipList(float probability = 0.5, int maxLevel = 16)
    {
        m_probability = probability;
        m_maxLevel = maxLevel;

        Data tmp = Data();

        //Key nilKey = std::numeric_limits<Key>::max();
        NIL = NULL;//new Node(nilKey, tmp, maxLevel, NULL);

        Key headKey = Key(); // std::numeric_limits<Key>::min();
        head = new Node(headKey, tmp, maxLevel, NIL);
    }

    ~CMaaSkipList()
    {
        Node * n = head;
        while(n->next[0])
        {
            Node * tmp = n;
            n = n->next[0];
            delete tmp;
        }
        delete n;
    }

    void * begin()
    {
        return head->next[0];
    }
    void * end()
    {
        return NIL;
    }
    void * next(void * p)
    {
        return p != NIL ? ((Node *)p)->next[0] : NIL;
    }
    Key key(void *p) const
    {
        if  (p != NIL)
        {
            Node * n = (Node *)p;
            return n->k;
        }
        return Key();
    }
    Data & data(void *p) const
    {
        if  (p != NIL)
        {
            Node * n = (Node *)p;
            return n->d;
        }
        static Data d = Data();
        return d;
    }
    int Level(void *p) const
    {
        if  (p != NIL)
        {
            Node * n = (Node *)p;
            return n->Level();
        }
        return -1;
    }

    Node * predecessor(Key k, int level) const
    {
        Node* x = head;

        while(x->next[level] != NIL && x->next[level]->k < k)
        {
            x = x->next[level];
        }
        return x;
    }

    int randomLevel() const
    {
        int v = 1;
        while(((double)rand() / RAND_MAX) < m_probability && v < m_maxLevel)
        {
            v++;
        }
        return v;
    }

    int Add(Key k, Data d, bool bOver = true)
    {
        Node * p = predecessor(k, 0);
        Node * n = p->next[0];
        if  (n != NIL && n->k == k)
        {
            if  (!bOver)
            {
                return 1;
            }
            n->d = d;
            return 0; // overwritten
        }

        // create new node
        const int newNodeLevel = randomLevel();
        Node * x = new Node(k, d, newNodeLevel);

        // connect pointers of predecessors and new node to respective successors
        for (int i = 0; i < newNodeLevel; i++)
        {
            if  (i)
            {
                p = predecessor(k, i);
            }
            x->next[i] = p->next[i];
            p->next[i] = x;
        }
        return 0;
    }

    // Returns the first node for which !(n->k < k)
    Node * lower_bound(Key k) const
    {
        Node* x = head;

        for (int i = m_maxLevel; --i >= 0;)
        {
            while(x->next[i] != NIL && x->next[i]->k < k)
            {
                x = x->next[i];
            }
        }
        return x->next[0];
    }

    int Find(Key k, Data *d = NULL) const
    {
        Node * x = lower_bound(k);
        if  (x != NIL && x->k == k)
        {
            if  (d)
            {
                *d = x->d;
            }
            return 0;
        }
        return 1;
    }

    int Remove(Key k, Data * d = NULL)
    {
        Node * p = predecessor(k, 0);
        Node * n = p->next[0];
        if  (n == NIL || n->k != k)
        {
            return 1;
        }
        p->next[0] = n->next[0];
        for (int i = n->Level(); --i > 0; )
        {
            p = predecessor(k, i);
            p->next[i] = n->next[i];
        }
        if  (d)
        {
            *d = n->d;
        }
        delete n;
        return 0;
    }

    void Print() const;
#if 0
    void Print0() const
    {
        Node * list = head->next[0];
        int lineLen = 0;

        __utf8_printf("{");

        while(list != NIL)
        {
            __utf8_printf("d: %d, k: %d, l: %d", list->d, list->k, list->Level());

            list = list->next[0];

            if  (list != NIL)
            {
                __utf8_printf(" : ");
            }
            if  (++lineLen % 2 == 0)
            {
                __utf8_printf("\n");
            }
        }
        __utf8_printf("}\n");
    }
    void Print() const
    {
        Node * p;
        int l;
        for (l = m_maxLevel; --l >= 0; )
        {
            p = head->next[0];
            while(p != NIL)
            {
                if  (p->Level() >= l)
                {
                    __utf8_printf("%3d ", p->k);
                }
                else
                {
                    __utf8_printf("%3s ", "");
                }
                p = p->next[0];
            }
            __utf8_printf("\n");
        }
        p = head->next[0];
        while(p != NIL)
        {
            __utf8_printf("%3d ", p->d);
            p = p->next[0];
        }
        __utf8_printf("\n");
        __utf8_printf("\n");

#if 0
        for (l = m_maxLevel; --l >= 0; )
        {
            p = head->next[l];
            while(p != NIL)
            {
                __utf8_printf("%3d ", p->k);
                p = p->next[l];
            }
            __utf8_printf("\n");
        }
        __utf8_printf("\n");
        __utf8_printf("\n");
#endif
    }
#endif
};

#define SKIPLIST_TESTS

#ifdef SKIPLIST_TESTS
template<> void CMaaSkipList<int, int>::Print() const
{
    Node * p;
    int l;
    for (l = m_maxLevel; --l >= 0; )
    {
        p = head->next[0];
        while(p != NIL)
        {
            if  (p->Level() >= l)
            {
                __utf8_printf("%3d ", p->k);
            }
            else
            {
                __utf8_printf("%3s ", "");
            }
            p = p->next[0];
        }
        __utf8_printf("\n");
    }
    p = head->next[0];
    while(p != NIL)
    {
        __utf8_printf("%3d ", p->d);
        p = p->next[0];
    }
    __utf8_printf("\n");
    __utf8_printf("\n");

#if 0
    for (l = m_maxLevel; --l >= 0; )
    {
        p = head->next[l];
        while(p != NIL)
        {
            __utf8_printf("%3d ", p->k);
            p = p->next[l];
        }
        __utf8_printf("\n");
    }
    __utf8_printf("\n");
    __utf8_printf("\n");
#endif
}

//==================================================
int main_SkipListTest()
{
    srand(time(NULL));
    // 1.Initialize an empty CMaaSkipList object
    CMaaSkipList<int, int> s;

    // 2. insert()
    for (int i = 0; i < 100; ++i)
    {
        int v = rand() % 50;
        s.Add(v, -v);
    }

    for (int i = 0; i < 1000; ++i)
    {
        int v = rand() % 50;
        int t = rand() % 2;
        if  (t)
        {
            s.Add(v, -v);
            if  (s.Find(v))
            {
                __utf8_printf("err: [%d] Node NOT found!\n", v);
            }
        }
        else
        {
            s.Remove(v);
            int f = 0;
            if  (!s.Find(v, &f))
            {
                __utf8_printf("err: [%d]=%d Node found!\n\n", v, f);
            }
        }
    }

    // 2a. print()
    s.Print();

    // 3. find()
    int f = -9999;
    if  (!s.Find(10, &f))
    {
        __utf8_printf("[10] Node found!\nvalue: %d\n", f);
    }
    else
    {
        __utf8_printf("[10] Node NOT found!\n");
    }

    // 4. insert() - overwrite
    s.Add(20, 99);

    // 4a. print()
    s.Print();

    // 5. erase()
    s.Remove(20);

    // 5a. print();
    s.Print();

    __utf8_printf("\nDone!\n");
    return 0;
}

struct dd0
{
    int k;
    int d;
};

struct dd_
{
    int n;
    int *v;
    dd_(int x)
    {
        n = 1;
        v = new int[n];
        v[0] = x;
    }
    dd_(dd_ *a, int x)
    {
        n = a->n + 1;
        v = new int[n];
        memcpy(v, a->v, a->n * sizeof(int));
        v[a->n] = x;
    }
    ~dd_()
    {
        delete [] v;
    }
};
static int cmp1(const void *p1, const void *p2)
{
    return ((const dd0 *)p1)->k - ((const dd0 *)p2)->k;
}

int main_SkipListTestSort(int N)
{
    /*
  Так и не понял, зачем такая структура, т.к. по тестам N=30000
tests:
  6.793618 us       3,977,216  skiplist
  0.007562 us       5,328,896  rbtree
  0.003666 us       5,570,560  qsort
*/
    srand(time(NULL));

    CMaaSkipList<int, dd_ *> s;//(0.5, 32);
    CMaaRBTree<int, int> r;
    //const int N = 20;

    CMaaPtr<dd0> m(N), n(N), y(N);
    int i, v, f;
    for (i = 0; i < N; i++)
    {
        v = rand() % N;
        m[i].k = v;
        m[i].d = i;
    }

    CHrtMultiTimer t0, t1;

    t0.Start();
    for (i = 0; i < N; i++)
    {
        v = m[i].k;
        dd_ * p = NULL;
        if  (!s.Find(v, &p))
        {
            dd_ * pp = new dd_(p, m[i].d);
            s.Add(v, pp);
            delete p;
        }
        else
        {
            p = new dd_(m[i].d);
            s.Add(v, p);
        }
    }

    int idx = 0;
    for (void * p = s.begin(); p != s.end(); p = s.next(p))
    {
        v = s.key(p);
        dd_ * ptr = s.data(p);
        for (i = 0; i < ptr->n; i++)
        {
            n[idx].k = v;
            n[idx++].d = ptr->v[i];
        }
    }
    t0.Stop("skiplist");
    for (void * p = s.begin(); p != s.end(); p = s.next(p))
    {
        v = s.key(p);
        dd_ * ptr = s.data(p);
        delete ptr;
    }

    t0.Start();
    for (i = 0; i < N; i++)
    {
        r.Add(m[i].k, m[i].d);
    }
    idx = 0;
    for (CMaaRBTree<int, int>::Handle p = r.begin(); p != r.end(); p = r.TreeSuccessor(p))
    {
        v = r.key(p);
        int d = r.data(p);
        y[idx].k = v;
        y[idx++].d = d;
    }
    t0.Stop("rbtree");


    t0.Start();
    qsort((dd0 *)m, N, sizeof(dd0), cmp1);
    t0.Stop("qsort");

    CMaaString s0 = t0.GetResult("tests:\n");
    __utf8_printf("%S\n", &s0);

    int e = 0;
    if  (memcmp(m, n, N * sizeof(dd0)))
    {
        e++;
        __utf8_printf("Error: sl != qs\n");
    }
    if  (memcmp(m, y, N * sizeof(dd0)))
    {
        e++;
        __utf8_printf("Error: rb != qs\n");
    }
    if  (!e)
    {
        return 0;
    }

    for (i = 0; i < N; i++)
    {
        int v = m[i].k;
        int d = m[i].d;
        __utf8_printf("%d:%d ", v, d);
    }
    __utf8_printf("\n");

    for (i = 0; i < N; i++)
    {
        int v = n[i].k;
        int d = n[i].d;
        __utf8_printf("%d:%d ", v, d);
    }
    __utf8_printf("\n");

    for (i = 0; i < N; i++)
    {
        int v = y[i].k;
        int d = y[i].d;
        __utf8_printf("%d:%d ", v, d);
    }
    __utf8_printf("\n");

    /*
    for (void * p = s.begin(); p != s.end(); p = s.next(p))
    {
        v = s.key(p);
        f = 0;
        f = s.data(p);
        for (i = 0; i < f; i++)
        {
            __utf8_printf("%d ", v);
        }
    }
    __utf8_printf("\n");

    for (i = 0; i < N; i++)
    {
        int v = m[i];
        __utf8_printf("%d ", v);
    }
    __utf8_printf("\n");
    return 0;
*/
}
#endif //SKIPLIST_TESTS
