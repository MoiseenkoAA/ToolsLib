#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Цифровое TRIE дерево v.12
// Author: Andrey Moiseenko (Андрей Моисеенко)
// Copyright (C) 2017 Andrey Moiseenko
// Licensed under GNU GPL v3
// www: http://maasoftware.ru
// e-mail: support@maasoftware.ru
// Can be commented: #define TRIE10_DEBUG
// Tested with compilers:
// gcc version 6.3.0
// Visual Studio 2010
// TODO: Make template

//#define TRIE10_DEBUG
//#define TRIE10_DEBUG2

#ifdef TRIE10_DEBUG
extern bool gbDbgTrie10Nodes; // debug nodes
#endif

class CMaaTrie10
{
protected:
    struct Node
    {
        Node * m_Parent; // родительская вершина
        int m_Value; // значение >=0, -1 - не вершина  TODO: флаг для не вершин вместо m_Value = -1
        int m_N; // число букв (вершин)
        char * m_Letters; // отсортированные буквы
        Node ** m_Nodes; // вершины, соответствующие буквам
        char m_Letter; // буква
        bool m_bValue; // эта вершина - слово, m_Value имеет смысл

        Node(Node * Parent, char Letter);
        ~Node();
        // Универсальная функция поиска/вставки/поиска следующей/предыдущей позиции по букве
        int Find(char c, bool bInsert = false, int iNextPrev = 0, int *pPosNextPrev = NULL);
        // Удаление по индексу позиции
        bool Remove(int pos);
        // Удаление с поиском по букве
        bool Remove(char c);
        // Поиск первой вершины относительно позиции
        Node * FirstAtPos(int pos) const;
        // Поиск последней вершины относительно позиции
        Node * LastAtPos(int pos) const;
        // Поиск следующей вершины через родителя
        Node * NextAtTop() const;
        // Поиск предыдущей вершины через родителя
        Node * PrevAtTop() const;
        // Получение имени ключа
        bool GetKey(char *p, int Size, bool bAdd0 = true, int *pSizeRequired = NULL, int *pLen = NULL) const;
    private:
        // Защита копирования
        Node(const Node &);
        Node & operator = (const Node &);
    };

    Node * m_Root;

#ifdef TRIE10_DEBUG
    bool m_bDbg;
#endif

private:
    // Защита копирования
    CMaaTrie10(const CMaaTrie10 &);
    CMaaTrie10 & operator = (const CMaaTrie10 &);
public:
    // Тип хандлера для пользователей
    typedef Node * Handle;

    class iterator
    {
        CMaaTrie10 * m_trie;
        Handle m_Handle;
        bool m_bDirect;
    public:
        iterator(CMaaTrie10 &t, Handle h, bool dir)
        :   m_trie(&t),
            m_Handle(h),
            m_bDirect(dir)
        {
        }
        iterator(const iterator &That)
        {
            *this = That;
        }
        ~iterator()
        {
        }
        iterator & operator = (const iterator &That)
        {
            m_trie = That.m_trie;
            m_Handle = That.m_Handle;
            m_bDirect = That.m_bDirect;
            return *this;
        }
        void Swap(iterator &That)
        {
            iterator tmp(*this);
            *this = That;
            That = tmp;
        }
        iterator operator++(int)
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        iterator operator++()
        {
            m_Handle = m_bDirect ? m_trie->Next(m_Handle) : m_trie->Prev(m_Handle);
            return *this;
        }
        iterator operator--(int)
        {
            iterator tmp(*this);
            --(*this);
            return tmp;
        }
        iterator operator--()
        {
            m_Handle = m_bDirect ? m_trie->Prev(m_Handle) : m_trie->Next(m_Handle);
            return *this;
        }
        bool operator==(const iterator &That) const
        {
            return m_Handle == That.m_Handle && m_trie == That.m_trie && m_bDirect == That.m_bDirect;
        }
        bool operator!=(const iterator &That) const
        {
            return !(*this == That);
        }
        operator Handle()
        {
            return m_Handle;
        }
        bool GetKey(char *p, int Size, bool bAdd0 = true, int *pSizeRequired = NULL, int *pLen = NULL) const
        {
            return m_trie->GetKey(m_Handle, p, Size, bAdd0, pSizeRequired, pLen);
        }
        /*
        const Key & ckey()
        {
            if   (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        Key key()
        {
            if   (!IsValid())
            {
                throw 1;
            }
            return r->K;
        }
        */
        int & rdata()
        {
            return m_trie->Value(m_Handle);
        }
        int data()
        {
            return m_trie->GetValue(m_Handle);
        }
        bool Remove(int * pval = NULL)
        {
            return m_trie->Remove(m_Handle, pval);
        }
    };

    // Конструктор
    CMaaTrie10();
    // Деструктор
    virtual ~CMaaTrie10();

    // inline functions
    // Добавить ключ str со значением val
    bool Add(const char * str, int val)
    {
#ifdef TRIE10_DEBUG
        m_bDbg && printf("Add(%s, %d): ", str, val);
#endif
        return Add(str, (int)strlen(str), val);
    }

    // Удалить ключ str, вернув значение в *pval
    bool Remove(const char * str, int *pval = NULL)
    {
#ifdef TRIE10_DEBUG
        m_bDbg && printf("Remove(%s): ", str);
#endif
        return Remove(str, (int)strlen(str), pval);
    }

    // Не рекомендуем в api, упрощённый интерфейс для задания
    int Find(const char * str) const
    {
        return Find(str, (int)strlen(str));
    }

    // Не рекомендуем в api, упрощённый интерфейс для задания
    int Find(const char * str, int len) const
    {
        int v = -1; // возвращаем -1, если слово не найдено
        Find(str, len, &v);
        return v;
    }

    // Рекомендуем в api, более удобная функция
    bool Find(const char * str, int * pval) const
    {
#ifdef TRIE10_DEBUG
        if  (m_bDbg)
        {
            printf("Find(%s): ", str);
            int v = -1;
            pval = pval ? pval : &v;
            bool b = Find(str, (int)strlen(str), pval);
            printf(b ? "%d " : "false ", *pval);
            return b;
        }
#endif
        return Find(str, (int)strlen(str), pval);
    }

    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQ(const char * str, bool bEqual = false) const
    {
#ifdef TRIE10_DEBUG
        m_bDbg && printf("FindGQ(%s, %s): ", str, bEqual ? "true" : "false");
#endif
        return FindGQ(str, (int)strlen(str), bEqual);
    }

    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQ(const char * str, bool bEqual = false) const
    {
#ifdef TRIE10_DEBUG
        m_bDbg && printf("FindLQ(%s, %s): ", str, bEqual ? "true" : "false");
#endif
        return FindLQ(str, (int)strlen(str), bEqual);
    }

    // Поиск первого слова
    Handle First() const
    {
        Node * x = m_Root;
        if  (x && x->m_bValue == false)
        {
            x = x->FirstAtPos(0);
        }
        return x;
    }

    // Поиск последнего слова
    Handle Last() const
    {
        Node * x = m_Root;
        if  (x && x->m_N > 0)
        {
            x = x->LastAtPos(x->m_N - 1);
        }
        return x;
    }

    iterator begin()
    {
        iterator it(*this, First(), true);
        return it;
    }

    iterator end()
    {
        iterator it(*this, NULL, true);
        return it;
    }

    iterator rbegin()
    {
        iterator it(*this, Last(), false);
        return it;
    }

    iterator rend()
    {
        iterator it(*this, NULL, false);
        return it;
    }

    iterator it(Handle h, bool dir = true)
    {
        iterator it(*this, h, dir);
        return it;
    }

    // Не рекомендуем в api, для задания
    // Ссылка на значение по хандлеру
    int & Value(Handle h)
    {
        // не рекомендуется
        static int v0;
        return h ? h->m_Value : (v0 = -1);
    }

    // Не рекомендуем в api, для задания
    // Получение значение по хандлеру
    int GetValue(Handle h)
    {
        return Value(h);
    }

    // Рекомендуем в api, для задания
    // Получение значения по хандлеру, может вернуть false если h не слово
    bool GetValue(Handle h, int *pval)
    {
        if  (h && h->m_bValue)
        {
            if  (pval)
            {
                *pval = h->m_Value;
            }
            return true;
        }
        return false;
    }

    // Установка значения по хандлеру, может вернуть false если h не слово
    bool SetValue(Handle h, int v)
    {
        if  (h && h->m_bValue)
        {
            h->m_Value = v;
            return true;
        }
        return false;
    }

    // Поиск узла по ключу, ключ может быть не словом
    Handle FindHandle(const char * str) const
    {
        return FindHandle(str, (int)strlen(str));
    }

    // Получить ключ без '\0' в конце
    bool GetKey(Handle h, char *p, int Size, int *pSizeRequired, int *pLen) const
    {
        return GetKey(h, p, Size, false, pSizeRequired, pLen);
    }

    void SetDbg(bool bSet)
    {
#ifdef TRIE10_DEBUG
        m_bDbg = bSet;
#endif
    }

    // Основные функции для не null-terminated строк
    bool Add(const char * str, int len, int val);
    bool Remove(const char * str, int len, int *pval = NULL);
    bool Remove(Node * p, int *pval = NULL);
    Handle FindHandle(const char * str, int len) const;
    bool Find(const char * str, int len, int *pval) const;
    Handle FindGQ(const char * str, int len, bool bEqual = false) const;
    Handle FindLQ(const char * str, int len, bool bEqual = false) const;
    Handle MaxPath(const char * str, int len, int *plen, int *pincompleteLen = NULL, int *pval = NULL) const;

    // Следующее слово
    Handle Next(Handle h) const;
    // Предшествующее слово
    Handle Prev(Handle h) const;
    // Удалить вершины-не слова без дочерних, поднимаясь вверх
    Node * RemoveUnused(Node * p);

    // Тестовая печать дерева, вызывается без параметров
    void Print(char * str = NULL, Node * x = NULL) const;
    // Тестовая печать слов дерева, вызывается без параметров
    int PrintWords(char * str = NULL, Node * x = NULL) const;

    // Получить имя ключа, универсально - с/без '\0' в конце
    bool GetKey(Handle h, char *p, int Size, bool bAdd0 = true, int *pSizeRequired = NULL, int *pLen = NULL) const;
};

// struct CMaaTrie10::Node implementation
CMaaTrie10::Node::Node(Node * Parent, char Letter)
:   m_Parent(Parent),
    //m_Value(-1),
    m_N(0),
    m_Letters(NULL),
    m_Nodes(NULL),
    m_Letter(Letter),
    m_bValue(false)
{
}

CMaaTrie10::Node::~Node()
{
    for (int i = 0; i < m_N; i++)
    {
        delete m_Nodes[i];
    }
    delete [] m_Letters;
    delete [] m_Nodes;
}

// Универсальная функция поиска/вставки/поиска следующей/предыдущей позиции по букве
int CMaaTrie10::Node::Find(char c, bool bInsert, int iNextPrev, int *pPosNextPrev)
{
    // бинарный поиск позиции символа c, возвращает -1 если символа нет и bInsert == false && iNextPrev == 0
    // bInsert == true - вставка символа c, которого нет
    // iNextPrev > 0 - позиция следующего символа записывается в *pPosNextPrev, c - нет
    // iNextPrev < 0 - позиция предыдущего символа записывается в *pPosNextPrev, c - нет

#ifdef TRIE10_DEBUG
    gbDbgTrie10Nodes && printf("Find(%c, %s, %d), %d  %d\n", c, bInsert ? "true" : "false", iNextPrev, m_N, m_Value);
#endif

    // бинарный поиск
    int b = 0, e = m_N - 1;
    while(b < e)
    {
        int m = (b + e) / 2;

        if  (m_Letters[m] < c)
        {
            b = m + 1;
        }
        else
        {
            e = m;
        }
    }
#ifdef TRIE10_DEBUG
    gbDbgTrie10Nodes && printf("%d %d / %d\n", b, e, m_N);
#endif
    if  (b < m_N)
    {
        if  (m_Letters[b] == c)
        {
            return b;
        }
        if  (m_Letters[b] < c && (bInsert || iNextPrev > 0))
        {
            // корректировка позиции вставки и следующей буквы
            b++;
        }
        else if (m_Letters[b] > c && (!bInsert && iNextPrev < 0))
        {
            // корректировка позиции предыдущей буквы
            b--;
        }
    }
    else if (!bInsert && iNextPrev < 0)
    {
        // m_N == 0 - корректировка позиции предыдущей буквы
        b = -1;
    }
    if  (bInsert)
    {
        // вставка c,NULL в позицию b

        char * l = NULL;
        Node ** n = NULL;
        try
        {
            l = new char[m_N + 1];
            n = new Handle[m_N + 1];
        }
        catch(...)
        {
            delete [] l;
            throw; // TODO: eat new[] exceptions
        }
        if  (!l || !n)
        {
            delete [] l;
            delete [] n;
            return -1;
        }
        memcpy(l, m_Letters, b);
        l[b] = c;
        memcpy(l + b + 1, m_Letters + b, m_N - b);

        memcpy(n, m_Nodes, b * sizeof(Node *));
        n[b] = NULL;
        memcpy(n + b + 1, m_Nodes + b, (m_N - b) * sizeof(Node *));

        delete [] m_Letters; m_Letters = l;
        delete [] m_Nodes; m_Nodes = n;
        m_N++;
        return b;
    }
    if  (iNextPrev)
    {
        // если нет такой позиции, возвращаем -1
        *pPosNextPrev = iNextPrev < 0 || (iNextPrev > 0 && b < m_N) ? b : -1;
    }
    return -1; // не найден при обычном поиске
}

// Удаление по индексу позиции
bool CMaaTrie10::Node::Remove(int pos)
{
    if  (pos >= 0)
    {
        // обходимся без reallocation, pos < m_N
        memmove(m_Letters + pos, m_Letters + pos + 1, m_N - (pos + 1));
        memmove(m_Nodes + pos, m_Nodes + pos + 1, (m_N - (pos + 1)) * sizeof(Node *));
        m_N--;
        return true;
    }
    return false;
}

// Удаление с поиском по букве
bool CMaaTrie10::Node::Remove(char c)
{
    return Remove(Find(c));
}

// Поиск первой вершины относительно позиции
CMaaTrie10::Node * CMaaTrie10::Node::FirstAtPos(int pos) const
{
    Node * p = m_Nodes[pos];
    while(!p->m_bValue && p->m_N > 0)
    {
        p = p->m_Nodes[0];
    }
    return p; // p->m_Value >= 0 по построению
}

// Поиск последней вершины относительно позиции
CMaaTrie10::Node * CMaaTrie10::Node::LastAtPos(int pos) const
{
    Node * p = m_Nodes[pos];
    while(p->m_N > 0)
    {
        p = p->m_Nodes[p->m_N - 1];
    }
    return p; // p->m_Value >= 0 по построению
}

// Поиск следующей вершины через родителя
CMaaTrie10::Node * CMaaTrie10::Node::NextAtTop() const
{
    Node * p = (Node *)this;
    while(p)
    {
        Node * n = p;
        p = p->m_Parent;
        if  (!p)
        {
            break;
        }
        int pos = p->Find(n->m_Letter);
        if  (pos + 1 < p->m_N)
        {
            return p->FirstAtPos(pos + 1);
        }
    }
    return p;
}

// Поиск предыдущей вершины через родителя
CMaaTrie10::Node * CMaaTrie10::Node::PrevAtTop() const
{
    Node * p = (Node *)this;
    while(p)
    {
        Node * n = p;
        p = p->m_Parent;
        if  (!p)
        {
            break;
        }
        int pos = p->Find(n->m_Letter);
        if  (pos > 0)
        {
            return p->LastAtPos(pos - 1);
        }
        if  (p->m_bValue)
        {
            break;
        }
    }
    return p;
}

// Получение имени ключа
bool CMaaTrie10::Node::GetKey(char *p, int Size, bool bAdd0, int *pSizeRequired, int *pLen) const
{
    const Node * n = this;
    int len = 0;
    while(n->m_Parent)
    {
        //int pos = n->m_Parent->Find(n); // slow, use when there is no n->m_Letter; pos >= 0 && pos < n->m_Parent->m_N по построению
        if  (len < Size)
        {
            //p[len] = n->m_Parent->m_Letters[pos];
            p[len] = n->m_Letter;
        }
        len++;
        n = n->m_Parent;
    }
    int SizeRequired = len + bAdd0 ? 1 : 0;
    if  (pSizeRequired)
    {
        *pSizeRequired = SizeRequired;
    }
    if  (pLen)
    {
        *pLen = len;
    }
    if  (Size < SizeRequired)
    {
        if  (Size > 0 && bAdd0)
        {
            *p = 0;
        }
        return false;
    }
    for (int i = len / 2; --i >= 0;)
    {
        char c = p[i];
        p[i] = p[len - 1 - i];
        p[len - 1 - i] = c;
    }
    if  (bAdd0)
    {
        p[len] = 0;
    }
    return true;
}

// class CMaaTrie10 implementation
// Конструктор
CMaaTrie10::CMaaTrie10()
{
    m_Root = NULL;
#ifdef TRIE10_DEBUG
    m_bDbg = false;
#endif
}

// Деструктор
CMaaTrie10::~CMaaTrie10()
{
    delete m_Root;
}

// Добавить слово str длиной len со значением val
bool CMaaTrie10::Add(const char * str, int len, int val)
{
    if  (/*val < 0 ||*/ len <= 0)
    {
        return false; // недопустимое значение value или len
    }
    if  (!m_Root)
    {
        m_Root = new Node(NULL, 0);
        if  (!m_Root)
        {
            return false;
        }
    }
    Node * p = m_Root;
    int i = 0, pos0 = -1;
    try
    {
        for (; i < len; i++)
        {
            char c = str[i];
            int pos = p->Find(c, true);
            if  (pos < 0)
            {
                break; // new [] error
            }
            if  (!p->m_Nodes[pos])
            {
                pos0 = pos; // запоминаем позицию буквы с нулевым указателем
                p->m_Nodes[pos] = new Node(p, c);
                if  (!p->m_Nodes[pos])
                {
                    //p->Remove(pos); // удалим [pos0==pos] вне цикла
                    break;
                }
                pos0 = -1; // забываем позицию
            }
            p = p->m_Nodes[pos];
        }
    }
    catch(...)
    {
        // new [] error
        if  (pos0 >= 0)
        {
            p->Remove(pos0); // более быстро, чем p->Remove(NULL);
        }
        RemoveUnused(p);
        throw;
    }
    if  (i < len)
    {
        // new [] error
        if  (pos0 >= 0)
        {
            p->Remove(pos0); // более быстро, чем p->Remove(NULL);
        }
        RemoveUnused(p);
        return false;
    }
    p->m_Value = val;
    p->m_bValue = true;
    return true;
}

// Удалить слово str длиной len, вернув значение в *pval
bool CMaaTrie10::Remove(const char * str, int len, int *pval)
{
    Node * p = FindHandle(str, len);
    return Remove(p, pval);
}

// Удалить слово вершины p, вернув значение в *pval
bool CMaaTrie10::Remove(Node * p, int *pval)
{
    if  (p && p->m_bValue)
    {
        if  (pval)
        {
            *pval = p->m_Value;
        }
        p->m_bValue = false;
        RemoveUnused(p);
        return true;
    }
    return false;
}

// Удалить вершины-не слова без дочерних, поднимаясь вверх
CMaaTrie10::Node * CMaaTrie10::RemoveUnused(Node * p)
{
    while(p && p->m_bValue == false && p->m_N == 0)
    {
        // эту вершину можно удалить
        Node * x = p;
        p = p->m_Parent;
        if  (!p)
        {
            // делаем дерево пустым
            m_Root = NULL;
        }
        else
        {
            p->Remove(x->m_Letter); // более быстро, чем p->Remove(x);
        }
        delete x;
    }
    return p;
}

// Поиск вершины по ключу, может вернуть вершину - не слово
CMaaTrie10::Handle CMaaTrie10::FindHandle(const char * str, int len) const
{
    Node * p = m_Root;
    for (int i = 0; i < len && p; i++)
    {
        char c = str[i];
        int pos = p->Find(c);
        if  (pos < 0)
        {
            p = NULL;
            break;
        }
        p = p->m_Nodes[pos];
    }
    return p;
}

CMaaTrie10::Handle CMaaTrie10::MaxPath(const char * str, int len, int *plen, int *pincompleteLen /*= NULL*/, int *pval /*= NULL*/) const
{
    int rlen = 0, inclen = 0;
    Handle r = NULL;

    Node * p = m_Root;
    for (int i = 0; i < len && p; i++)
    {
        char c = str[i];
        int pos = p->Find(c);
        if  (pos < 0)
        {
            p = NULL;
            break;
        }
        p = p->m_Nodes[pos];
        inclen++;
        if  (p->m_bValue)
        {
            rlen = inclen;
            r = p;
        }
    }
    if  (plen)
    {
        *plen = rlen;
    }
    if  (pincompleteLen)
    {
        *pincompleteLen = inclen;
    }
    if  (r && pval)
    {
        *pval = r->m_Value;
    }
    return r;
}

// Поиск слова, возврат значения в *pval, return true on success
bool CMaaTrie10::Find(const char * str, int len, int *pval) const
{
    Node * x = FindHandle(str, len);
    if  (x && x->m_bValue == false)
    {
        // не слово
        return false;
    }
    if  (x && pval)
    {
        *pval = x->m_Value;
    }
    return x ? true : false;
}

// Поиск последующего слова > str или >= str, если bEqual = true
CMaaTrie10::Handle CMaaTrie10::FindGQ(const char * str, int len, bool bEqual) const
{
    Node * p = m_Root;
    for (int i = 0; i < len && p; i++)
    {
        char c = str[i];
        int pos_next = -1; // позиция для следующего символа, если c не найден
        int pos = p->Find(c, false, 1, &pos_next);
#ifdef TRIE10_DEBUG2
        m_bDbg && printf("%d\n", pos);
#endif
        if  (pos < 0)
        {
#ifdef TRIE10_DEBUG2
            m_bDbg && printf("%d\n", pos_next);
#endif
            if  (pos_next >= 0)
            {
                return p->FirstAtPos(pos_next);
            }
            return p->NextAtTop();
        }
        p = p->m_Nodes[pos];
    }
    if  (p && (!bEqual || p->m_bValue == false))
    {
        p = Next(p);
    }
    return p;
}

// Поиск предшествующего слова < str или <= str, если bEqual = true
CMaaTrie10::Handle CMaaTrie10::FindLQ(const char * str, int len, bool bEqual) const
{
    Node * p = m_Root;
    for (int i = 0; i < len && p; i++)
    {
        char c = str[i];
        int pos_prev = -1; // позиция для предыдущего символа, если c не найден
        int pos = p->Find(c, false, -1, &pos_prev);
#ifdef TRIE10_DEBUG2
        m_bDbg && printf("%d\n", pos);
#endif
        if  (pos < 0)
        {
#ifdef TRIE10_DEBUG2
            m_bDbg && printf("%d\n", pos_prev);
#endif
            if  (pos_prev >= 0)
            {
                return p->LastAtPos(pos_prev);
            }
            return p->PrevAtTop();
        }
        p = p->m_Nodes[pos];
    }
    if  (p && (!bEqual || p->m_bValue == false))
    {
        p = Prev(p);
    }
    return p;
}

// Следующее слово
CMaaTrie10::Handle CMaaTrie10::Next(Handle p) const
{
    if  (p)
    {
        return p->m_N > 0 ? p->FirstAtPos(0) : p->NextAtTop();
    }
    return NULL;
}

// Предшествующее слово
CMaaTrie10::Handle CMaaTrie10::Prev(Handle p) const
{
    if  (p)
    {
        return p->PrevAtTop();
    }
    return NULL;
}

// Получить имя ключа, универсально - с/без '\0' в конце
bool CMaaTrie10::GetKey(Handle h, char *p, int Size, bool bAdd0, int *pSizeRequired, int *pLen) const
{
    if  (h)
    {
        return h->GetKey(p, Size, bAdd0, pSizeRequired, pLen);
    }
    if  (Size > 0 && bAdd0)
    {
        *p = 0;
    }
    if  (pSizeRequired)
    {
        *pSizeRequired = 0; // or 1
    }
    if  (pLen)
    {
        *pLen = 0;
    }
    return false;
}

// Тестовая печать дерева, вызывается без параметров
// TODO: убрать из релиза
void CMaaTrie10::Print(char * str, Node * x) const
{
    if  (!x)
    {
        if  (str)
        {
            printf("%s null\n", str);
            return;
        }
        printf("tree:\n"); fflush(stdout);
        static char buff[1024]; buff[0] = 0; str = buff;
        x = m_Root;
        if  (!x)
        {
            printf("empty\n"); fflush(stdout);
            return;
        }
    }
    if  (x->m_bValue)
    {
        printf("%p %-15s v=%4d n=%4d  ", x, str, x->m_Value, x->m_N);
    }
    else
    {
        printf("%p %-15s v=%4s n=%4d  ", x, str, "***", x->m_N);
    }
    for (int i = 0; i < x->m_N; i++)
    {
        printf("%c(%02x) ", x->m_Letters[i], (unsigned char)x->m_Letters[i]);
    }
    printf("\n");
    char * str2 = str + strlen(str);
    if  (str2 - str2 < 1024 - 1)
    {
        for (int i = 0; i < x->m_N; i++)
        {
            str2[0] = x->m_Letters[i];
            str2[1] = 0;
            Print(str, x->m_Nodes[i]);
            str2[0] = 0;
        }
    }
}

// Тестовая печать слов дерева, вызывается без параметров
// TODO: убрать из релиза
int CMaaTrie10::PrintWords(char * str, Node * x) const
{
    int V = 0;
    if  (!x)
    {
        if  (str)
        {
#ifdef TRIE10_DEBUG
            m_bDbg && printf("%s null\n", str);
#endif
            return -1;
        }
        //printf("tree:\n");
        static char buff[1024 + 1]; buff[0] = 0; str = buff;
        x = m_Root;
        if  (!x)
        {
            //printf("empty\n");
            return V;
        }
    }
    if  (x->m_bValue)
    {
        V += x->m_Value;
        printf("%05d %-15s\n", x->m_Value, str);
    }
    char * str2 = str + strlen(str);
    if  (str2 - str < 1024)
    {
        for (int i = 0; i < x->m_N; i++)
        {
            str2[0] = x->m_Letters[i];
            str2[1] = 0;
            V += PrintWords(str, x->m_Nodes[i]);
            str2[0] = 0;
        }
    }
    return V;
}

// CMaaTrie10 test function
int main_test_CMaaTrie10(int argn, char * args[])
{
    printf("Tests:\n");
    {
        printf("Test1: empty tree: "); fflush(stdout);
        CMaaTrie10 a;
        printf("."); fflush(stdout);
        printf("%s", a.Find("a", 1) < 0 ? "." : " fail "); fflush(stdout);
        printf("%s", a.Find("abc", 1) < 0 ? "." : " fail "); fflush(stdout);
        CMaaTrie10::Handle h = a.FindHandle("ab");
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        printf("%s", !a.Remove("a") ? "." : " fail "); fflush(stdout);
        printf("."); fflush(stdout);
        h = a.FindGQ("ab");
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        h = a.Next(h);
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        h = a.First();
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        h = a.Next(h);
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        h = a.Last();
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        h = a.Prev(h);
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        printf("\n"); fflush(stdout);
        a.Print(); fflush(stdout);
        printf("~"); fflush(stdout);
    }
    printf(".\n"); fflush(stdout);
    printf("\n"); fflush(stdout);
    {
        printf("Test2: задание:\n");
        CMaaTrie10 a;
        //a.SetDbg(true);
        /*
        a.Add("abc", 100);
        a.Add("abx", 10);
        a.Add("def", 2);
        a.Print();
        printf("%d ", a.Find("abc"));
        printf("%d ", a.Find("abx"));
        printf("%d ", a.Find("def"));
        printf("%d ", a.Find("klm"));
        printf("%d\n", a.Find("abd"));
        printf("%s\n", a.Remove("def") ? "true" : "false");
        printf("%d ", a.Find("abc"));
        printf("%d ", a.Find("def"));
        printf("%d ", a.Find("klm"));
        printf("%d\n", a.Find("abd"));
        a.Print();
        */
        const char * s[] = {"альфа", "бета", "гамма", "дельта", "эпсилон", "одна", "однаждыы", NULL};
        printf("Find(not existed): %d\n", a.Find("бета"));
        printf("Remove(not existed): %s\n", a.Remove("бета") ? "true" : "false");
        int i;
        for (i = 0; s[i]; i++)
        {
            bool b = a.Add(s[i], i + 1);
            printf("Add %s %d %s  ", s[i], i + 1, b ? "true" : "false");
            printf("//Find(\"бета\") = %d\n", a.Find("бета"));
        }
        {
            CMaaString s = "однажды";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%d %d ('%s', '%s') %d\n", len, ilen, (const char *)p, (const char *)ip, v);
        }
        {
            CMaaString s = "однаждыыхх";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%d %d ('%s', '%s') %d\n", len, ilen, (const char *)p, (const char *)ip, v);
            return 0;
        }
        if  (0)
        {
            a.Print();
            printf("LQ:\n");
            CMaaTrie10::Handle hh = a.FindGQ("бета");
            printf("%p\n", hh);
        }
        bool bEqual = false;
        //bEqual = true;
        printf("Test2: values for key %s \"бета\": ", bEqual ? ">=" : ">");
        for (CMaaTrie10::Handle h = a.FindGQ("бета", bEqual); h; h = a.Next(h))
        {
            //printf("%p %d\n", h, a.GetValue(h));
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
    }
    printf("\n");
    {
        printf("Test3: similar words: ");
        CMaaTrie10 a;
        a.SetDbg(true);
        a.Add("zabc", 101);
        a.Add("zabcd", 102);
        a.Add("zabcde", 103);
        a.Add("zabcf", 104);
        a.Add("zabcfe", 105);
        a.Add("ax", 99);
        a.Remove("zabcf");
        printf("\n");
        a.Print();
        //gbDbgTrie10Nodes=true;
        for (CMaaTrie10::Handle h = a.FindGQ("zabc"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindGQ("za"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindGQ("AB"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindGQ("{"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindLQ("zabc"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindLQ("zabcd"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindLQ("zabce"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindLQ("zabcfg"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindLQ("Z"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie10::Handle h = a.FindLQ("{"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        a.SetDbg(false);
        //for (CMaaTrie10::iterator it = a.begin(); it; ++it)
        for (CMaaTrie10::iterator it = a.begin(); it != a.end(); ++it)
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
        }
        printf("\n");
        for (CMaaTrie10::iterator it = a.rbegin(); it != a.rend(); ++it)
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
        }
        printf("\n");
        for (CMaaTrie10::iterator it = a.begin(); it != a.end(); )
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
            CMaaTrie10::iterator r = it++;
            r.Remove();
        }
        printf("\n");
        for (CMaaTrie10::Handle p = a.First(); p; )
        {
            char key[1024 + 1];
            a.GetKey(p, key, (int)sizeof(key), true);
            printf("[%s] ", key);
            CMaaTrie10::Handle n = a.Next(p);
            a.Remove(p);
            p = n;
        }
        printf("\n");
        a.Print();
    }
    printf("\n");
    return 0;
}


bool gbDbgTrie10Nodes = false;
int main(int argn, char * args[])
{
    return main_test_CMaaTrie10(argn, args);
}
