
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/a_trie.h

/* Copyright (C) 2002-2025 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains strings and string's functions implementation.
 * This CMaaString does not throws throws exception out of boundaries.
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

//#include "perm.h"
//#include "temp.h"


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// Цифровое TRIE дерево v.14
// Author: Andrey Moiseenko (Андрей Моисеенко)
// Copyright (C) 2017 Andrey Moiseenko
// Licensed under GNU GPL v3
// www: http://maasoftware.ru
// e-mail: support@maasoftware.ru
// Can be commented: #define TRIE10_DEBUG
// Tested with compilers:
// g++ version 12.2.0
// Visual Studio 2022

//#define TRIE10_DEBUG
//#define TRIE10_DEBUG2

#ifdef TRIE10_DEBUG
extern bool gbDbgTrie10Nodes; // debug nodes
#endif

//#define TRIE10_OPT_BITMAP

#if 1

template < class Data > class CMaaTrie
{
protected:
    struct Node
    {
        Node* m_Parent; // родительская вершина
        char m_Value[sizeof(Data)]; // значение
        int m_N; // число букв (вершин)
        char* m_Letters; // отсортированные буквы
        Node** m_Nodes; // вершины, соответствующие буквам
#ifdef TRIE10_OPT_BITMAP
        unsigned char m_BitMap[256 / 8];
#endif
        char m_Letter; // буква
        bool m_bValue; // эта вершина - слово, m_Value имеет смысл

        Node(Node* Parent, char Letter) noexcept
            : m_Parent(Parent),
            //m_Value(-1),
            m_N(0),
            m_Letters(nullptr),
            m_Nodes(nullptr),
            m_Letter(Letter),
            m_bValue(false)
        {
#ifdef TRIE10_OPT_BITMAP
            memset(m_BitMap, 0, sizeof(m_BitMap));
#endif
            //memset(m_Value, 0, sizeof(m_Value));
#ifdef __ALLOCATOR_FILL
            memset(m_Value, __ALLOCATOR_FILL_INIT_CHAR, sizeof(m_Value));
#endif
        }

        ~Node()
        {
            for (int i = 0; i < m_N; i++)
            {
                delete m_Nodes[i];
            }
            delete[] m_Letters;
            delete[] m_Nodes;
            if (m_bValue)
            {
                Data* d = (Data*)m_Value;
                d->~Data();
#ifdef __ALLOCATOR_FILL
                memset(m_Value, __ALLOCATOR_FILL_FREE_CHAR, sizeof(m_Value));
#endif
            }
        }

        // Универсальная функция поиска/вставки/поиска следующей/предыдущей позиции по букве
        template <bool bInsert /* = false */ > int Find (char c, int iNextPrev = 0, int* pPosNextPrev = nullptr) noexcept(!bInsert || noexcept_new)
        {
            // бинарный поиск позиции символа c, возвращает -1 если символа нет и bInsert == false && iNextPrev == 0
            // bInsert == true - вставка символа c, которого нет
            // iNextPrev > 0 - позиция следующего символа записывается в *pPosNextPrev, c - нет
            // iNextPrev < 0 - позиция предыдущего символа записывается в *pPosNextPrev, c - нет

#ifdef TRIE10_DEBUG
            gbDbgTrie10Nodes&& printf("Find(%c, %s, %d), %d  %d\n", c, bInsert ? "true" : "false", iNextPrev, m_N, m_Value);
#endif
#ifdef TRIE10_OPT_BITMAP
            if (!bInsert && !iNextPrev && !(m_BitMap[((unsigned char)c) / 8] & (1 << (((unsigned char)c) & 0x7))))
            {
                return -1;
            }
#endif

            // бинарный поиск
            int b = 0, e = m_N - 1;
            while (b < e)
            {
                const int m = (b + e) / 2;

                if (m_Letters[m] < c)
                {
                    b = m + 1;
                }
                else
                {
                    e = m;
                }
            }
#ifdef TRIE10_DEBUG
            gbDbgTrie10Nodes&& printf("%d %d / %d\n", b, e, m_N);
#endif
            if (b < m_N)
            {
                if (m_Letters[b] == c)
                {
                    return b;
                }
                if (m_Letters[b] < c && (bInsert || iNextPrev > 0))
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
            if constexpr (bInsert)
            {
                // вставка c,nullptr в позицию b

                char* l = nullptr;
                Node** n = nullptr;
                try
                {
                    l = TL_NEW char[m_N + 1];
                    n = TL_NEW Handle[m_N + 1];
                }
                catch (...)
                {
                    if constexpr (!noexcept_new)
                    {
                        delete[] l;
                        throw; // TODO: eat New[] exceptions
                    }
                }
                if (!l || !n)
                {
                    delete[] l;
                    delete[] n;
                    return -1;
                }
                memcpy(l, m_Letters, b);
                l[b] = c;
                memcpy(l + b + 1, m_Letters + b, m_N - b);

                memcpy(n, m_Nodes, b * sizeof(Node*));
                n[b] = nullptr;
                memcpy(n + b + 1, m_Nodes + b, (m_N - b) * sizeof(Node*));

                delete[] m_Letters; m_Letters = l;
                delete[] m_Nodes; m_Nodes = n;
                m_N++;
#ifdef TRIE10_OPT_BITMAP
                m_BitMap[((unsigned char)c) / 8] |= (1 << (((unsigned char)c) & 0x7));
#endif
                return b;
            }
            if (iNextPrev)
            {
                // если нет такой позиции, возвращаем -1
                *pPosNextPrev = (iNextPrev < 0 || (iNextPrev > 0 && b < m_N)) ? b : -1;
            }
            return -1; // не найден при обычном поиске
        }

        // Удаление по индексу позиции
        bool Remove(int pos) noexcept
        {
            if (pos >= 0)
            {
#ifdef TRIE10_OPT_BITMAP
                char c = m_Letters[pos];
                m_BitMap[((unsigned char)c) / 8] &= ~(1 << (((unsigned char)c) & 0x7));
#endif
                // обходимся без reallocation, pos < m_N
                memmove(m_Letters + pos, m_Letters + pos + 1, m_N - (pos + 1));
                memmove(m_Nodes + pos, m_Nodes + pos + 1, (m_N - (pos + 1)) * sizeof(Node*));
                m_N--;
                return true;
            }
            return false;
        }

        // Удаление с поиском по букве
        bool Remove(char c) noexcept
        {
            return Remove(Find<false>(c));
        }

        // Поиск первой вершины относительно позиции
        Node* FirstAtPos(int pos) const noexcept
        {
            Node* p = m_Nodes[pos];
            while (!p->m_bValue && p->m_N > 0)
            {
                p = p->m_Nodes[0];
            }
            return p; // p->m_bValue == true по построению
        }

        // Поиск последней вершины относительно позиции
        Node* LastAtPos(int pos) const noexcept
        {
            Node* p = m_Nodes[pos];
            while (p->m_N > 0)
            {
                p = p->m_Nodes[p->m_N - 1];
            }
            return p; // p->m_bValue == true по построению
        }

        // Поиск следующей вершины через родителя
        Node* NextAtTop() const noexcept
        {
            Node* p = (Node*)this;
            while (p)
            {
                const Node* n = p;
                p = p->m_Parent;
                if (!p)
                {
                    break;
                }
                const int pos = p->Find<false>(n->m_Letter);
                if (pos + 1 < p->m_N)
                {
                    return p->FirstAtPos(pos + 1);
                }
            }
            return p;
        }

        // Поиск предыдущей вершины через родителя
        Node* PrevAtTop() const noexcept
        {
            Node* p = (Node*)this;
            while (p)
            {
                const Node* n = p;
                p = p->m_Parent;
                if (!p)
                {
                    break;
                }
                const int pos = p->Find<false>(n->m_Letter);
                if (pos > 0)
                {
                    return p->LastAtPos(pos - 1);
                }
                if (p->m_bValue)
                {
                    break;
                }
            }
            return p;
        }

        // Получение имени ключа
        bool GetKey(char* p, int Size, bool bAdd0 = true, int* pSizeRequired = nullptr, int* pLen = nullptr) const noexcept
        {
            const Node* n = this;
            int len = 0;
            while (n->m_Parent)
            {
                //int pos = n->m_Parent->Find<false>(n); // slow, use when there is no n->m_Letter; pos >= 0 && pos < n->m_Parent->m_N по построению
                if (len < Size)
                {
                    //p[len] = n->m_Parent->m_Letters[pos];
                    p[len] = n->m_Letter;
                }
                len++;
                n = n->m_Parent;
            }
            int SizeRequired = len + (bAdd0 ? 1 : 0);
            if (pSizeRequired)
            {
                *pSizeRequired = SizeRequired;
            }
            if (pLen)
            {
                *pLen = len;
            }
            if (Size < SizeRequired)
            {
                if (Size > 0 && bAdd0)
                {
                    *p = 0;
                }
                return false;
            }
            for (int i = len / 2; i--;)
            {
                const char c = p[i];
                p[i] = p[len - 1 - i];
                p[len - 1 - i] = c;
            }
            if (bAdd0)
            {
                p[len] = 0;
            }
            return true;
        }

        // Получение имени ключа
        CMaaString GetKey() const noexcept(noexcept_new)
        {
            char Buffer[TOOLSLIB_CS_4K];
            CMaaConcatString c(Buffer, sizeof(Buffer));

            const Node* n = this;
            int len = 0;
            while (n->m_Parent)
            {
                c += n->m_Letter;
                len++;
                n = n->m_Parent;
            }
            CMaaString r = c;
            if (r.Length() != len)
            {
                r.Empty();
                return r;
            }
            char* p = r.GetBuffer();
            for (int i = len / 2; i--;)
            {
                char c = p[i];
                p[i] = p[len - 1 - i];
                p[len - 1 - i] = c;
            }
            return r;
        }

    private:
        // Защита копирования
        Node(const Node&);
        Node& operator = (const Node&);
    };

    Node* m_Root;

#ifdef TRIE10_DEBUG
    bool m_bDbg;
#endif

private:
    // Защита копирования
    CMaaTrie(const CMaaTrie&) = delete;
    CMaaTrie& operator = (const CMaaTrie&) = delete;
public:
    // Тип хандлера для пользователей
    typedef Node* Handle;

    class iterator
    {
        CMaaTrie* m_trie;
        Handle m_Handle;
        bool m_bDirect;
    public:
        iterator(CMaaTrie& t, Handle h, bool dir) noexcept
            : m_trie(&t),
            m_Handle(h),
            m_bDirect(dir)
        {
        }
        iterator(const iterator& That) noexcept
        {
            *this = That;
        }
        ~iterator() noexcept
        {
        }
        iterator& operator = (const iterator& That) noexcept
        {
            m_trie = That.m_trie;
            m_Handle = That.m_Handle;
            m_bDirect = That.m_bDirect;
            return *this;
        }
        void Swap(iterator& That) noexcept
        {
            iterator tmp(*this);
            *this = That;
            That = tmp;
        }
        iterator operator++(int) noexcept
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        iterator operator++() noexcept
        {
            m_Handle = m_bDirect ? m_trie->Next(m_Handle) : m_trie->Prev(m_Handle);
            return *this;
        }
        iterator operator--(int) noexcept
        {
            iterator tmp(*this);
            --(*this);
            return tmp;
        }
        iterator operator--() noexcept
        {
            m_Handle = m_bDirect ? m_trie->Prev(m_Handle) : m_trie->Next(m_Handle);
            return *this;
        }
        bool operator==(const iterator& That) const noexcept
        {
            return m_Handle == That.m_Handle && m_trie == That.m_trie && m_bDirect == That.m_bDirect;
        }
        bool operator!=(const iterator& That) const noexcept
        {
            return !(*this == That);
        }
        operator Handle() noexcept
        {
            return m_Handle;
        }
        bool GetKey(char* p, int Size, bool bAdd0 = true, int* pSizeRequired = nullptr, int* pLen = nullptr) const noexcept(noexcept_new)
        {
            return m_trie->GetKey(m_Handle, p, Size, bAdd0, pSizeRequired, pLen);
        }
        CMaaString GetKey() const noexcept(noexcept_new)
        {
            return m_trie->GetKey(m_Handle);
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
        Data& rdata() noexcept(std::is_nothrow_move_constructible<Data>::value)
        {
            return m_trie->Value(m_Handle);
        }
        Data data() noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
        {
            return m_trie->GetValue(m_Handle);
        }
        bool Remove(Data* pval = nullptr) noexcept(noexcept(*pval = *pval))
        {
            return m_trie->Remove(m_Handle, pval);
        }
    };

    // Конструктор
    CMaaTrie() noexcept
    {
        m_Root = nullptr;
#ifdef TRIE10_DEBUG
        m_bDbg = false;
#endif
    }

    // Деструктор
    virtual ~CMaaTrie()
    {
        delete m_Root;
    }

    // inline functions
    // Добавить ключ str со значением val
    bool Add(const char* str, const Data& val) noexcept(noexcept(Add(str, (int)strlen(str), val)))
    {
#ifdef TRIE10_DEBUG
        m_bDbg&& printf("Add(%s, %d): ", str, val);
#endif
        return Add(str, (int)strlen(str), val);
    }

    // Удалить ключ str, вернув значение в *pval
    bool Remove(const char* str, Data* pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
#ifdef TRIE10_DEBUG
        m_bDbg&& printf("Remove(%s): ", str);
#endif
        return Remove(str, (int)strlen(str), pval);
    }

    // Не рекомендуем в api, упрощённый интерфейс для задания
    Data Find(const char* str) const noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Find(str, (int)strlen(str));
    }

    // Не рекомендуем в api, упрощённый интерфейс для задания
    Data Find(const char* str, int len) const noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        Data v{};// = -1; // возвращаем -1, если слово не найдено
        Find(str, len, &v);
        return v;
    }

    // Рекомендуем в api, более удобная функция
    bool Find(const char* str, Data* pval) const noexcept(noexcept(*pval=*pval))
    {
#ifdef TRIE10_DEBUG
        if (m_bDbg)
        {
            printf("Find(%s): ", str);
            int v{};// = -1;
            pval = pval ? pval : &v;
            bool b = Find(str, (int)strlen(str), pval);
            printf(b ? "%d " : "false ", *pval);
            return b;
        }
#endif
        return Find(str, (int)strlen(str), pval);
    }

    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQ(const char* str, bool bEqual = false) const noexcept
    {
#ifdef TRIE10_DEBUG
        m_bDbg&& printf("FindGQ(%s, %s): ", str, bEqual ? "true" : "false");
#endif
        return FindGQ(str, (int)strlen(str), bEqual);
    }

    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQ(const char* str, bool bEqual = false) const noexcept
    {
#ifdef TRIE10_DEBUG
        m_bDbg&& printf("FindLQ(%s, %s): ", str, bEqual ? "true" : "false");
#endif
        return FindLQ(str, (int)strlen(str), bEqual);
    }

    // CMaaString - functions
    //////////
    // Добавить ключ str со значением val
    bool AddS(CMaaString str, const Data& val) noexcept(noexcept(Add((const char*)str, str.Length(), val)))
    {
        return Add((const char*)str, str.Length(), val);
    }
    // Удалить ключ str, вернув значение в *pval
    bool RemoveS(CMaaString str, Data* pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        return Remove((const char*)str, str.Length(), pval);
    }
    // Не рекомендуем в api, упрощённый интерфейс для задания
    Data FindS(CMaaString str) const noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Find((const char*)str, str.Length());
    }
    // Рекомендуем в api, более удобная функция
    bool FindS(CMaaString str, Data* pval) const noexcept(noexcept(*pval=*pval))
    {
        return Find((const char*)str, str.Length(), pval);
    }
    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQS(CMaaString str, bool bEqual = false) const noexcept
    {
        return FindGQ((const char*)str, str.Length(), bEqual);
    }
    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQS(CMaaString str, bool bEqual = false) const noexcept
    {
        return FindLQ((const char*)str, str.Length(), bEqual);
    }
    //////////

    // Поиск первого слова
    Handle First() const noexcept
    {
        Node* x = m_Root;
        if (x && x->m_bValue == false)
        {
            x = x->FirstAtPos(0);
        }
        return x;
    }

    // Поиск последнего слова
    Handle Last() const noexcept
    {
        Node* x = m_Root;
        if (x && x->m_N > 0)
        {
            x = x->LastAtPos(x->m_N - 1);
        }
        return x;
    }

    iterator begin() noexcept
    {
        iterator it(*this, First(), true);
        return it;
    }

    iterator end() noexcept
    {
        iterator it(*this, nullptr, true);
        return it;
    }

    iterator rbegin() noexcept
    {
        iterator it(*this, Last(), false);
        return it;
    }

    iterator rend() noexcept
    {
        iterator it(*this, nullptr, false);
        return it;
    }

    iterator it(Handle h, bool dir = true) noexcept
    {
        iterator it(*this, h, dir);
        return it;
    }

    // Не рекомендуем в api, для задания
    // Ссылка на значение по хандлеру
    Data& Value(Handle h) noexcept(std::is_nothrow_move_constructible<Data>::value)
    {
        // не рекомендуется
        static Data v0{};
        return h ? *(Data*)h->m_Value : (v0);// = -1);
    }

    // Не рекомендуем в api, для задания
    // Получение значение по хандлеру
    Data GetValue(Handle h) noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Value(h);
    }

    // Рекомендуем в api, для задания
    // Получение значения по хандлеру, может вернуть false если h не слово
    bool GetValue(Handle h, Data* pval) noexcept(noexcept(*pval=*pval))
    {
        if (h && h->m_bValue)
        {
            if (pval)
            {
                *pval = *(Data*)h->m_Value;
            }
            return true;
        }
        return false;
    }

    // Установка значения по хандлеру, может вернуть false если h не слово
    bool SetValue(Handle h, const Data& v) noexcept(std::is_nothrow_move_assignable<Data>::value)
    {
        if (h && h->m_bValue)
        {
            *(Data*)h->m_Value = v;
            return true;
        }
        return false;
    }

    // Поиск узла по ключу, ключ может быть не словом
    Handle FindHandle(const char* str) const noexcept
    {
        return FindHandle(str, (int)strlen(str));
    }

    // Получить ключ без '\0' в конце
    bool GetKey(Handle h, char* p, int Size, int* pSizeRequired, int* pLen) const noexcept
    {
        return GetKey(h, p, Size, false, pSizeRequired, pLen);
    }

#ifdef TRIE10_DEBUG
    void SetDbg(bool bSet) noexcept
    {
        m_bDbg = bSet;
    }
#else
    void SetDbg(bool) noexcept
    {
    }
#endif

    // Основные функции для не null-terminated строк

    // Добавить слово str длиной len со значением val
    bool Add(const char* str, int len, const Data& val) noexcept(noexcept_new && noexcept(Data(val)) && noexcept((Data &)val = val))
    {
        if (/*val < 0 ||*/ len <= 0)
        {
            return false; // недопустимое значение value или len
        }
        if (!m_Root)
        {
            m_Root = TL_NEW Node(nullptr, 0);
            if (!m_Root)
            {
                return false;
            }
        }
        Node* p = m_Root;
        int i = 0, pos0 = -1;
        try
        {
            for (; i < len; i++)
            {
                const char c = str[i];
                int pos = p->template Find<true>(c);
                if (pos < 0)
                {
                    break; // New [] error
                }
                if (!p->m_Nodes[pos])
                {
                    pos0 = pos; // запоминаем позицию буквы с нулевым указателем
                    p->m_Nodes[pos] = TL_NEW Node(p, c);
                    if (!p->m_Nodes[pos])
                    {
                        //p->Remove(pos); // удалим [pos0==pos] вне цикла
                        break;
                    }
                    pos0 = -1; // забываем позицию
                }
                p = p->m_Nodes[pos];
            }
            if (i >= len)
            {
                Data* d = (Data*)p->m_Value;
                if (!p->m_bValue)
                {
#ifdef __ALLOCATOR_FILL
                    memset(p->m_Value, __ALLOCATOR_FILL_ALLOC_CHAR, sizeof(p->m_Value));
#endif
                    new_(d, Data(val));
                }
                else
                {
                    *d = val;
                }
                p->m_bValue = true;
                //return true;
            }
        }
        catch (...)
        {
            // New [] error
            if (pos0 >= 0)
            {
                p->Remove(pos0); // более быстро, чем p->Remove(nullptr);
            }
            RemoveUnused(p);
            if constexpr (!noexcept(noexcept_new && noexcept(Data(val)) && noexcept((Data&)val = val)))
            {
                throw;
            }
        }
        if (i < len)
        {
            // New [] error
            if (pos0 >= 0)
            {
                p->Remove(pos0); // более быстро, чем p->Remove(nullptr);
            }
            RemoveUnused(p);
            return false;
        }
        //p->m_Value = val;
        //p->m_bValue = true;
        return true;
    }

    // Удалить слово str длиной len, вернув значение в *pval
    bool Remove(const char* str, int len, Data* pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        Node* p = FindHandle(str, len);
        return Remove(p, pval);
    }

    // Удалить слово вершины p, вернув значение в *pval
    bool Remove(Node* p, Data* pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        if (p && p->m_bValue)
        {
            Data* d = (Data*)p->m_Value;
            if (pval)
            {
                *pval = *d;
            }
            d->~Data();
#ifdef __ALLOCATOR_FILL
            memset(p->m_Value, __ALLOCATOR_FILL_FREE_CHAR, sizeof(p->m_Value));
#endif
            p->m_bValue = false;
            RemoveUnused(p);
            return true;
        }
        return false;
    }

    // Поиск вершины по ключу, может вернуть вершину - не слово
    Handle FindHandle(const char* str, int len) const noexcept
    {
        Node* p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const char c = str[i];
            int pos = p->template Find<false>(c);
            if (pos < 0)
            {
                p = nullptr;
                break;
            }
            p = p->m_Nodes[pos];
        }
        return p;
    }

    // Поиск слова, возврат значения в *pval, return true on success
    bool Find(const char* str, int len, Data* pval) const noexcept(noexcept(*pval=*pval))
    {
        Node* x = FindHandle(str, len);
        if (x && x->m_bValue == false)
        {
            // не слово
            return false;
        }
        if (x && pval)
        {
            *pval = *(Data*)x->m_Value;
        }
        return x ? true : false;
    }

    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQ(const char* str, int len, bool bEqual = false) const noexcept
    {
        Node* p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const char c = str[i];
            int pos_next = -1; // позиция для следующего символа, если c не найден
            int pos = p->template Find<false>(c, 1, &pos_next);
#ifdef TRIE10_DEBUG2
            m_bDbg&& printf("%d\n", pos);
#endif
            if (pos < 0)
            {
#ifdef TRIE10_DEBUG2
                m_bDbg&& printf("%d\n", pos_next);
#endif
                if (pos_next >= 0)
                {
                    return p->FirstAtPos(pos_next);
                }
                return p->NextAtTop();
            }
            p = p->m_Nodes[pos];
        }
        if (p && (!bEqual || p->m_bValue == false))
        {
            p = Next(p);
        }
        return p;
    }

    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQ(const char* str, int len, bool bEqual = false) const noexcept
    {
        Node* p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const char c = str[i];
            int pos_prev = -1; // позиция для предыдущего символа, если c не найден
            int pos = p->template Find<false>(c, -1, &pos_prev);
#ifdef TRIE10_DEBUG2
            m_bDbg&& printf("%d\n", pos);
#endif
            if (pos < 0)
            {
#ifdef TRIE10_DEBUG2
                m_bDbg&& printf("%d\n", pos_prev);
#endif
                if (pos_prev >= 0)
                {
                    return p->LastAtPos(pos_prev);
                }
                return p->PrevAtTop();
            }
            p = p->m_Nodes[pos];
        }
        if (p && (!bEqual || p->m_bValue == false))
        {
            p = Prev(p);
        }
        return p;
    }

    // Определение максимального пути (слова/части слова)
    Handle MaxPath(const char* str, int len, int* plen, int* pincompleteLen = nullptr, Data* pval = nullptr) const noexcept(noexcept(*pval=*pval))
    {
        int rlen = 0, inclen = 0;
        Handle r = nullptr;

        Node* p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const char c = str[i];
            int pos = p->template Find<false>(c);
            if (pos < 0)
            {
                p = nullptr;
                break;
            }
            p = p->m_Nodes[pos];
            inclen++;
            if (p->m_bValue)
            {
                rlen = inclen;
                r = p;
            }
        }
        if (plen)
        {
            *plen = rlen;
        }
        if (pincompleteLen)
        {
            *pincompleteLen = inclen;
        }
        if (r && pval)
        {
            *pval = *(Data*)r->m_Value;
        }
        return r;
    }

    // Определение первого пути (слова/части слова)
    Handle FirstPath(const char* str, int len, int* plen, int* pincompleteLen = nullptr, Data* pval = nullptr) const noexcept(noexcept(*pval=*pval))
    {
        int rlen = 0, inclen = 0;
        Handle r = nullptr;

        Node* p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            char c = str[i];
            int pos = p->template Find<false>(c);
            if (pos < 0)
            {
                p = nullptr;
                break;
            }
            p = p->m_Nodes[pos];
            inclen++;
            if (p->m_bValue)
            {
                rlen = inclen;
                r = p;
                break; // первое существующее слово
            }
        }
        if (plen)
        {
            *plen = rlen;
        }
        if (pincompleteLen)
        {
            *pincompleteLen = inclen;
        }
        if (r && pval)
        {
            *pval = *(Data*)r->m_Value;
        }
        return r;
    }

    // Следующее слово
    Handle Next(Handle p) const noexcept
    {
        if (p)
        {
            return p->m_N > 0 ? p->FirstAtPos(0) : p->NextAtTop();
        }
        return nullptr;
    }

    // Предшествующее слово
    Handle Prev(Handle p) const noexcept
    {
        if (p)
        {
            return p->PrevAtTop();
        }
        return nullptr;
    }

    // Удалить вершины-не слова без дочерних, поднимаясь вверх
    Node* RemoveUnused(Node* p) noexcept
    {
        while (p && p->m_bValue == false && p->m_N == 0)
        {
            // эту вершину можно удалить
            Node* x = p;
            p = p->m_Parent;
            if (!p)
            {
                // делаем дерево пустым
                m_Root = nullptr;
            }
            else
            {
                p->Remove(x->m_Letter); // более быстро, чем p->Remove(x);
            }
            delete x;
        }
        return p;
    }

    // Тестовая печать дерева, вызывается без параметров
    void Print(char* str = nullptr, Node* x = nullptr, int pos = 0) const;
    // Тестовая печать слов дерева, вызывается без параметров
    int PrintWords(char* str = nullptr, Node* x = nullptr, int pos = 0) const;

    // Получить имя ключа, универсально - с/без '\0' в конце
    bool GetKey(Handle h, char* p, int Size, bool bAdd0 = true, int* pSizeRequired = nullptr, int* pLen = nullptr) const noexcept
    {
        if (h)
        {
            return h->GetKey(p, Size, bAdd0, pSizeRequired, pLen);
        }
        if (Size > 0 && bAdd0)
        {
            *p = 0;
        }
        if (pSizeRequired)
        {
            *pSizeRequired = 0; // or 1
        }
        if (pLen)
        {
            *pLen = 0;
        }
        return false;
    }

    // Получить имя ключа в CMaaString
    CMaaString GetKey(Handle h) const noexcept(noexcept(h->GetKey()))
    {
        if (h)
        {
            return h->GetKey();
        }
        CMaaString tmp;
        return tmp;
    }
};

#endif

#if 2

template < class Data, char ch0 = '\0', int AlphabetSize = 256 > class CMaaTrie2
{
protected:
    struct Node
    {
        Node * m_Parent; // родительская вершина
        char m_Value[sizeof(Data)]; // значение
        Node * m_Nodes[AlphabetSize]; // вершины, соответствующие буквам
        char m_Letter; // буква
        bool m_bValue; // эта вершина - слово, m_Value имеет смысл

        Node(Node * Parent, char Letter) noexcept
        :   m_Parent(Parent),
            m_Letter(Letter),
            m_bValue(false)
        {
            memset(m_Nodes, 0, sizeof(m_Nodes));
#ifdef __ALLOCATOR_FILL
            memset(m_Value, __ALLOCATOR_FILL_INIT_CHAR, sizeof(m_Value));
#endif
        }

        ~Node() noexcept
        {
            for (int i = 0; i < AlphabetSize; i++)
            {
                delete m_Nodes[i];
            }
            if  (m_bValue)
            {
                Data * d = (Data *)m_Value;
                d->~Data();
#ifdef __ALLOCATOR_FILL
                memset(m_Value, __ALLOCATOR_FILL_FREE_CHAR, sizeof(m_Value));
#endif
            }
        }

        // Удаление с поиском по букве
        /*
        bool Remove(char c)
        {
            return Remove(Find<false>(c));
        }
        */

        // slow
        int GetCount() const noexcept // slow
        {
            int N = 0;
            for (int idx = 0; idx < AlphabetSize; idx++)
            {
                if (m_Nodes[idx])
                {
                    N++;
                }
            }
            return N;
        }

        Node* FindN1(int idx) const noexcept
        {
            while (++idx < AlphabetSize)
            {
                if (m_Nodes[idx])
                {
                    return m_Nodes[idx];
                }
            }
            return nullptr;
        }

        Node* FindP1(int idx) const noexcept
        {
            while (--idx >= 0)
            {
                if (m_Nodes[idx])
                {
                    return m_Nodes[idx];
                }
            }
            return nullptr;
        }

        // Поиск первой вершины относительно позиции
        static Node* FirstAtPos(Node* p) noexcept
        {
            while (!p->m_bValue)
            {
                Node* n = p->FindN1(-1);
                if (!n)
                {
                    break;
                }
                p = n;
            }
            return p; // p->m_bValue == true по построению
        }

        // Поиск первой вершины относительно позиции
        Node * FirstAtPos(int pos) const noexcept
        {
            return FirstAtPos(m_Nodes[pos]);
        }

        // Поиск последней вершины относительно позиции
        static Node* LastAtPos(Node* p) noexcept
        {
            while (1)
            {
                Node* n = p->FindP1(AlphabetSize);
                if (!n)
                {
                    break;
                }
                p = n;
            }
            return p; // p->m_bValue == true по построению
        }

        // Поиск последней вершины относительно позиции
        Node * LastAtPos(int pos) const noexcept
        {
            return LastAtPos(m_Nodes[pos]);
        }

        // Поиск следующей вершины через родителя
        Node * NextAtTop() const noexcept
        {
            Node * p = (Node *)this;
            while(p)
            {
                const int pos = (int)(unsigned char)p->m_Letter;
                p = p->m_Parent;
                if  (!p)
                {
                    break;
                }
                Node* n = p->FindN1(pos);
                if (n)
                {
                    return FirstAtPos(n);
                }
            }
            return p;
        }

        // Поиск предыдущей вершины через родителя
        Node* PrevAtTop() const noexcept
        {
            Node* p = (Node*)this;
            while (p)
            {
                const int pos = (int)(unsigned char)p->m_Letter;
                p = p->m_Parent;
                if (!p)
                {
                    break;
                }
                Node* n = p->FindP1(pos);
                if (n)
                {
                    return LastAtPos(n);
                }
                if (p->m_bValue)
                {
                    break;
                }
            }
            return p;
        }

        // Получение имени ключа
        bool GetKey(char *p, int Size, bool bAdd0 = true, int *pSizeRequired = nullptr, int *pLen = nullptr) const noexcept
        {
            const Node * n = this;
            int len = 0;
            while(n->m_Parent)
            {
                //int pos = n->m_Parent->Find(n); // slow, use when there is no n->m_Letter; pos >= 0 && pos < n->m_Parent->m_N по построению
                if  (len < Size)
                {
                    //p[len] = n->m_Parent->m_Letters[pos];
                    p[len] = n->m_Letter + ch0;
                }
                len++;
                n = n->m_Parent;
            }
            int SizeRequired = len + (bAdd0 ? 1 : 0);
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
            for (int i = len / 2; i--;)
            {
                const char c = p[i];
                p[i] = p[len - 1 - i];
                p[len - 1 - i] = c;
            }
            if  (bAdd0)
            {
                p[len] = 0;
            }
            return true;
        }

        // Получение имени ключа
        CMaaString GetKey() const noexcept(noexcept_new)
        {
            char Buffer[TOOLSLIB_CS_4K];
            CMaaConcatString c(Buffer, sizeof(Buffer));

            const Node * n = this;
            int len = 0;
            while(n->m_Parent)
            {
                c += (char)(n->m_Letter + ch0);
                len++;
                n = n->m_Parent;
            }
            CMaaString r = c;
            if  (r.Length() != len)
            {
                r.Empty();
                return r;
            }
            char *p = r.GetBuffer();
            for (int i = len / 2; i--;)
            {
                char c = p[i];
                p[i] = p[len - 1 - i];
                p[len - 1 - i] = c;
            }
            return r;
        }

    private:
        // Защита копирования
        Node(const Node &);
        Node & operator = (const Node &);
    };

    Node * m_Root;

private:
    // Защита копирования
    CMaaTrie2(const CMaaTrie2 &);
    CMaaTrie2 & operator = (const CMaaTrie2 &);
public:
    // Тип хандлера для пользователей
    typedef Node * Handle;

    class iterator
    {
        CMaaTrie2 * m_trie;
        Handle m_Handle;
        bool m_bDirect;
    public:
        iterator(CMaaTrie2 &t, Handle h, bool dir) noexcept
        :   m_trie(&t),
            m_Handle(h),
            m_bDirect(dir)
        {
        }
        iterator(const iterator &That) noexcept
        {
            *this = That;
        }
        ~iterator() noexcept
        {
        }
        iterator & operator = (const iterator &That) noexcept
        {
            m_trie = That.m_trie;
            m_Handle = That.m_Handle;
            m_bDirect = That.m_bDirect;
            return *this;
        }
        void Swap(iterator &That) noexcept
        {
            iterator tmp(*this);
            *this = That;
            That = tmp;
        }
        iterator operator++(int) noexcept
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        iterator operator++() noexcept
        {
            m_Handle = m_bDirect ? m_trie->Next(m_Handle) : m_trie->Prev(m_Handle);
            return *this;
        }
        iterator operator--(int) noexcept
        {
            iterator tmp(*this);
            --(*this);
            return tmp;
        }
        iterator operator--() noexcept
        {
            m_Handle = m_bDirect ? m_trie->Prev(m_Handle) : m_trie->Next(m_Handle);
            return *this;
        }
        bool operator==(const iterator &That) const noexcept
        {
            return m_Handle == That.m_Handle && m_trie == That.m_trie && m_bDirect == That.m_bDirect;
        }
        bool operator!=(const iterator &That) const noexcept
        {
            return !(*this == That);
        }
        operator Handle() noexcept
        {
            return m_Handle;
        }
        bool GetKey(char *p, int Size, bool bAdd0 = true, int *pSizeRequired = nullptr, int *pLen = nullptr) const noexcept
        {
            return m_trie->GetKey(m_Handle, p, Size, bAdd0, pSizeRequired, pLen);
        }
        CMaaString GetKey() const
        {
            return m_trie->GetKey(m_Handle);
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
        Data & rdata() noexcept(std::is_nothrow_move_constructible<Data>::value)
        {
            return m_trie->Value(m_Handle);
        }
        Data data() noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
        {
            return m_trie->GetValue(m_Handle);
        }
        bool Remove(Data * pval = nullptr) noexcept(noexcept(*pval=*pval))
        {
            return m_trie->Remove(m_Handle, pval);
        }
    };

    // Конструктор
    CMaaTrie2() noexcept
    {
        m_Root = nullptr;
    }

    // Деструктор
    virtual ~CMaaTrie2()
    {
        delete m_Root;
    }

    // inline functions
    // Добавить ключ str со значением val
    bool Add(const char * str, const Data &val) noexcept(noexcept(Add(str, (int)strlen(str), val)))
    {
        return Add(str, (int)strlen(str), val);
    }

    // Удалить ключ str, вернув значение в *pval
    bool Remove(const char * str, Data *pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        return Remove(str, (int)strlen(str), pval);
    }

    // Не рекомендуем в api, упрощённый интерфейс для задания
    Data Find(const char * str) const noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Find(str, (int)strlen(str));
    }

    // Не рекомендуем в api, упрощённый интерфейс для задания
    Data Find(const char * str, int len) const noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        Data v{};// = -1; // возвращаем -1, если слово не найдено
        Find(str, len, &v);
        return v;
    }

    // Рекомендуем в api, более удобная функция
    bool Find(const char * str, Data * pval) const noexcept(noexcept(*pval=*pval))
    {
        return Find(str, (int)strlen(str), pval);
    }

    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQ(const char * str, bool bEqual = false) const noexcept
    {
        return FindGQ(str, (int)strlen(str), bEqual);
    }

    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQ(const char * str, bool bEqual = false) const noexcept
    {
        return FindLQ(str, (int)strlen(str), bEqual);
    }

    // CMaaString - functions
    //////////
    // Добавить ключ str со значением val
    bool AddS(CMaaString str, const Data &val) noexcept(noexcept(Add((const char*)str, str.Length(), val))) // noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Add((const char *)str, str.Length(), val);
    }
    // Удалить ключ str, вернув значение в *pval
    bool RemoveS(CMaaString str, Data *pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        return Remove((const char *)str, str.Length(), pval);
    }
    // Не рекомендуем в api, упрощённый интерфейс для задания
    Data FindS(CMaaString str) const noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Find((const char *)str, str.Length());
    }
    // Рекомендуем в api, более удобная функция
    bool FindS(CMaaString str, Data * pval) const noexcept(noexcept(*pval=*pval))
    {
        return Find((const char *)str, str.Length(), pval);
    }
    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQS(CMaaString str, bool bEqual = false) const noexcept
    {
        return FindGQ((const char *)str, str.Length(), bEqual);
    }
    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQS(CMaaString str, bool bEqual = false) const noexcept
    {
        return FindLQ((const char *)str, str.Length(), bEqual);
    }
    //////////

    // Поиск первого слова
    Handle First() const noexcept
    {
        Node * x = m_Root;
        if  (x && x->m_bValue == false)
        {
            x = x->FindN1(-1);
            x = Node::FirstAtPos(x);
        }
        return x;
    }

    // Поиск последнего слова
    Handle Last() const noexcept
    {
        Node * x = m_Root;
        if  (x)
        {
            x = x->FindP1(AlphabetSize);
            x = Node::LastAtPos(x);
        }
        return x;
    }

    iterator begin() noexcept
    {
        iterator it(*this, First(), true);
        return it;
    }

    iterator end() noexcept
    {
        iterator it(*this, nullptr, true);
        return it;
    }

    iterator rbegin() noexcept
    {
        iterator it(*this, Last(), false);
        return it;
    }

    iterator rend() noexcept
    {
        iterator it(*this, nullptr, false);
        return it;
    }

    iterator it(Handle h, bool dir = true) noexcept
    {
        iterator it(*this, h, dir);
        return it;
    }

    // Не рекомендуем в api, для задания
    // Ссылка на значение по хандлеру
    Data & Value(Handle h) noexcept(std::is_nothrow_move_constructible<Data>::value)
    {
        // не рекомендуется
        static Data v0{};
        return h ? *(Data *)h->m_Value : (v0);// = -1);
    }

    // Не рекомендуем в api, для задания
    // Получение значение по хандлеру
    Data GetValue(Handle h) noexcept(std::is_nothrow_move_constructible<Data>::value&& std::is_nothrow_move_assignable<Data>::value)
    {
        return Value(h);
    }

    // Рекомендуем в api, для задания
    // Получение значения по хандлеру, может вернуть false если h не слово
    bool GetValue(Handle h, Data *pval) noexcept(noexcept(*pval=*pval))
    {
        if  (h && h->m_bValue)
        {
            if  (pval)
            {
                *pval = *(Data *)h->m_Value;
            }
            return true;
        }
        return false;
    }

    // Установка значения по хандлеру, может вернуть false если h не слово
    bool SetValue(Handle h, const Data &v) noexcept(std::is_nothrow_move_assignable<Data>::value)
    {
        if  (h && h->m_bValue)
        {
            *(Data *)h->m_Value = v;
            return true;
        }
        return false;
    }

    // Поиск узла по ключу, ключ может быть не словом
    Handle FindHandle(const char * str) const noexcept
    {
        return FindHandle(str, (int)strlen(str));
    }

    // Получить ключ без '\0' в конце
    bool GetKey(Handle h, char *p, int Size, int *pSizeRequired, int *pLen) const noexcept
    {
        return GetKey(h, p, Size, false, pSizeRequired, pLen);
    }

    // Основные функции для не null-terminated строк

    // Добавить слово str длиной len со значением val
    bool Add(const char * str, int len, const Data &val) noexcept(noexcept_new && noexcept(Data(val) && noexcept((Data&)val = val)))
    {
        if  (/*val < 0 ||*/ len <= 0)
        {
            return false; // недопустимое значение value или len
        }
        if  (!m_Root)
        {
            m_Root = TL_NEW Node(nullptr, '\0');
            if  (!m_Root)
            {
                return false;
            }
        }
        Node * p = m_Root;
        int i = 0;
        try
        {
            for (; i < len; i++)
            {
                const int c = (int)(unsigned char)str[i] - (int)(unsigned char)ch0;
                if (c < 0 || c >= AlphabetSize)
                {
                    // break;
                    throw 1;
                }
                Node* n = p->m_Nodes[c];
                if (!n)
                {
                    n = p->m_Nodes[c] = TL_NEW Node(p, (char)c);
                    if (!n)
                    {
                        // break;
                        throw 1;
                    }
                }
                p = n;
            }
            if  (i >= len)
            {
                Data * d = (Data *)p->m_Value;
                if  (!p->m_bValue)
                {
#ifdef __ALLOCATOR_FILL
                    memset(p->m_Value, __ALLOCATOR_FILL_ALLOC_CHAR, sizeof(p->m_Value));
#endif
                    new_(d, Data(val));
                }
                else
                {
                    *d = val;
                }
                p->m_bValue = true;
                return true;
            }
        }
        catch(...)
        {
            RemoveUnused(p);
            if constexpr (!noexcept(noexcept_new && noexcept(Data(val) && noexcept((Data&)val = val))))
            {
                throw;
            }
            return false;
        }
        if  (i < len)
        {
            RemoveUnused(p);
            return false;
        }
        return true;
    }

    // Удалить слово str длиной len, вернув значение в *pval
    bool Remove(const char * str, int len, Data *pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        Node * p = FindHandle(str, len);
        return Remove(p, pval);
    }

    // Удалить слово вершины p, вернув значение в *pval
    bool Remove(Node * p, Data *pval = nullptr) noexcept(noexcept(*pval=*pval))
    {
        if  (p && p->m_bValue)
        {
            Data * d = (Data *)p->m_Value;
            if  (pval)
            {
                *pval = *d;
            }
            d->~Data();
#ifdef __ALLOCATOR_FILL
            memset(p->m_Value, __ALLOCATOR_FILL_FREE_CHAR, sizeof(p->m_Value));
#endif
            p->m_bValue = false;
            RemoveUnused(p);
            return true;
        }
        return false;
    }

    // Поиск вершины по ключу, может вернуть вершину - не слово
    Handle FindHandle(const char * str, int len) const noexcept
    {
        Node * p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const int c = (int)(unsigned char)str[i] - (int)(unsigned char)ch0;
            if (c < 0 || c >= AlphabetSize)
            {
                return nullptr;
            }
            p = p->m_Nodes[c];
        }
        return p;
    }

    // Поиск слова, возврат значения в *pval, return true on success
    bool Find(const char * str, int len, Data *pval) const noexcept(noexcept(*pval=*pval))
    {
        Node * x = FindHandle(str, len);
        if  (x && x->m_bValue == false)
        {
            // не слово
            return false;
        }
        if  (x && pval)
        {
            *pval = *(Data *)x->m_Value;
        }
        return x ? true : false;
    }

    // Поиск последующего слова > str или >= str, если bEqual = true
    Handle FindGQ(const char * str, int len, bool bEqual = false) const noexcept
    {
        Node * p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const int c = (int)(unsigned char)str[i] - (int)(unsigned char)ch0;
            if ((c < 0 || c >= AlphabetSize) || !p->m_Nodes[c])
            {
                Node* n = c < 0 ? p->FindN1(-1) : c >= AlphabetSize ? nullptr : p->FindN1(c);
                if  (n)
                {
                    return Node::FirstAtPos(n);
                }
                return p->NextAtTop();
            }
            p = p->m_Nodes[c];
        }
        if  (p && (!bEqual || p->m_bValue == false))
        {
            p = Next(p);
        }
        return p;
    }

    // Поиск предшествующего слова < str или <= str, если bEqual = true
    Handle FindLQ(const char * str, int len, bool bEqual = false) const noexcept
    {
        Node * p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const int c = (int)(unsigned char)str[i] - (int)(unsigned char)ch0;
            if ((c < 0 || c >= AlphabetSize) || !p->m_Nodes[c])
            {
                Node* n = c >= AlphabetSize ? p->FindP1(AlphabetSize) : c < 0 ? nullptr : p->FindP1(c);
                if (n)
                {
                    return Node::LastAtPos(n);
                }
                return p->PrevAtTop();
            }
            p = p->m_Nodes[c];
        }
        if  (p && (!bEqual || p->m_bValue == false))
        {
            p = Prev(p);
        }
        return p;
    }

    // Определение максимального пути (слова/части слова)
    Handle MaxPath(const char * str, int len, int *plen, int *pincompleteLen = nullptr, Data *pval = nullptr) const noexcept(noexcept(*pval=*pval))
    {
        int rlen = 0, inclen = 0;
        Handle r = nullptr;

        Node * p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const int c = (int)(unsigned char)str[i] - (int)(unsigned char)ch0;
            if (c < 0 || c >= AlphabetSize)
            {
                break;
                // throw 1;
            }
            p = p->m_Nodes[c];
            if  (!p)
            {
                break;
            }
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
            *pval = *(Data *)r->m_Value;
        }
        return r;
    }

    // Определение первого пути (слова/части слова)
    Handle FirstPath(const char * str, int len, int *plen, int *pincompleteLen = nullptr, Data *pval = nullptr) const noexcept(noexcept(*pval=*pval))
    {
        int rlen = 0, inclen = 0;
        Handle r = nullptr;

        Node * p = m_Root;
        for (int i = 0; i < len && p; i++)
        {
            const int c = (int)(unsigned char)str[i] - (int)(unsigned char)ch0;
            if (c < 0 || c >= AlphabetSize)
            {
                break;
                //throw 1;
            }
            p = p->m_Nodes[c];
            if  (!p)
            {
                break;
            }
            inclen++;
            if  (p->m_bValue)
            {
                rlen = inclen;
                r = p;
                break; // первое существующее слово
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
            *pval = *(Data *)r->m_Value;
        }
        return r;
    }

    // Следующее слово
    Handle Next(Handle p) const noexcept
    {
        if (p)
        {
            Node* n = p->FindN1(-1);
            if (n)
            {
                return Node::FirstAtPos(n);
            }
            return p->NextAtTop();
        }
        return nullptr;
    }

    // Предшествующее слово
    Handle Prev(Handle p) const noexcept
    {
        if  (p)
        {
            return p->PrevAtTop();
        }
        return nullptr;
    }

    // Удалить вершины-не слова без дочерних, поднимаясь вверх
    Node * RemoveUnused(Node * p) noexcept
    {
        while (p && p->m_bValue == false)
        {
            if (p->FindN1(-1))
            {
                return p;
            }

            // эту вершину можно удалить
            Node * x = p;
            p = p->m_Parent;
            if  (!p)
            {
                // делаем дерево пустым
                m_Root = nullptr;
            }
            else
            {
                const int c = (int)(unsigned char)x->m_Letter;
                p->m_Nodes[c] = nullptr;
            }
            delete x;
        }
        return p;
    }

    // Тестовая печать дерева, вызывается без параметров
    void Print(char * str = nullptr, Node * x = nullptr, int pos = 0) const;
    // Тестовая печать слов дерева, вызывается без параметров
    int PrintWords(char * str = nullptr, Node * x = nullptr, int pos = 0) const;

    // Получить имя ключа, универсально - с/без '\0' в конце
    bool GetKey(Handle h, char *p, int Size, bool bAdd0 = true, int *pSizeRequired = nullptr, int *pLen = nullptr) const noexcept
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

    // Получить имя ключа в CMaaString
    CMaaString GetKey(Handle h) const
    {
        if  (h)
        {
            return h->GetKey();
        }
        CMaaString tmp;
        return tmp;
    }
};

#endif

#if 0
// Тестовая печать дерева, вызывается без параметров
template <> void CMaaTrie<int>::Print(char * str, Node * x, int pos) const
{
    if  (!x)
    {
        if  (str)
        {
            printf("%s !null\n", str);
            return;
        }
        printf("tree:\n"); fflush(stdout);
        static CMaaString buff(nullptr, 10 * 1024 + 1);
        str = (char *)(const char *)buff; *str = 0; pos = 0;
        x = m_Root;
        if  (!x)
        {
            printf("empty\n"); fflush(stdout);
            return;
        }
    }
    if  (x->m_bValue)
    {
        printf("%p %-15s v=%4d n=%4d  ", x, str, *(int *)x->m_Value, x->m_N);
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
    char * str2 = str + pos;
    if  (str2 - str2 < 10 * 1024)
    {
        for (int i = 0; i < x->m_N; i++)
        {
            str2[0] = x->m_Letters[i];
            str2[1] = 0;
            Print(str, x->m_Nodes[i], pos + 1);
            str2[0] = 0;
        }
    }
}

// Тестовая печать слов дерева, вызывается без параметров
template <> int CMaaTrie<int>::PrintWords(char * str, Node * x, int pos) const
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
        static CMaaString buff(nullptr, 10 * 1024 + 1);
        str = (char *)(const char *)buff; *str = 0; pos = 0;
        x = m_Root;
        if  (!x)
        {
            //printf("empty\n");
            return V;
        }
    }
    if  (x->m_bValue)
    {
        V += *(int *)x->m_Value;
        printf("%05d %-15s\n", *(int *)x->m_Value, str);
    }
    char * str2 = str + pos;
    if  (str2 - str < 10 * 1024)
    {
        for (int i = 0; i < x->m_N; i++)
        {
            str2[0] = x->m_Letters[i];
            str2[1] = 0;
            V += PrintWords(str, x->m_Nodes[i], pos + 1);
            str2[0] = 0;
        }
    }
    return V;
}
#endif

#if 0

// Тестовая печать дерева, вызывается без параметров
template <> void CMaaTrie2<int, '\0', 256>::Print(char* str, Node* x, int pos) const
{
    if (!x)
    {
        if (str)
        {
            printf("%s !null\n", str);
            return;
        }
        printf("tree:\n"); fflush(stdout);
        static CMaaString buff(nullptr, 10 * 1024 + 1);
        str = (char*)(const char*)buff; *str = 0; pos = 0;
        x = m_Root;
        if (!x)
        {
            printf("empty\n"); fflush(stdout);
            return;
        }
    }
    if (x->m_bValue)
    {
        printf("%p %-15s v=%4d n=%4d  ", x, str, *(int*)x->m_Value, x->GetCount());
    }
    else
    {
        printf("%p %-15s v=%4s n=%4d  ", x, str, "***", x->GetCount());
    }
    //for (int i = 0; i < x->m_N; i++)
    //{
    //    printf("%c(%02x) ", x->m_Letters[i], (unsigned char)x->m_Letters[i]);
    //}
    for (int i = 0; i < 256; i++)
    {
        if (x->m_Nodes[i])
        {
            printf("%c(%02x) ", (char)(i + 0), (i + 0));
        }
    }
    printf("\n");
    char* str2 = str + pos;
    if (str2 - str2 < 10 * 1024)
    {
        /*
        for (int i = 0; i < x->m_N; i++)
        {
            str2[0] = x->m_Letters[i];
            str2[1] = 0;
            Print(str, x->m_Nodes[i], pos + 1);
            str2[0] = 0;
        }
        */
        for (int i = 0; i < 256; i++)
        {
            if (x->m_Nodes[i])
            {
                str2[0] = (char)(i + 0);
                str2[1] = 0;
                Print(str, x->m_Nodes[i], pos + 1);
                str2[0] = 0;
            }
        }
    }
}

// Тестовая печать слов дерева, вызывается без параметров
template <> int CMaaTrie2<int, '\0', 256>::PrintWords(char* str, Node* x, int pos) const
{
    int V = 0;
    if (!x)
    {
        if (str)
        {
#ifdef TRIE10_DEBUG
            m_bDbg&& printf("%s null\n", str);
#endif
            return -1;
        }
        //printf("tree:\n");
        static CMaaString buff(nullptr, 10 * 1024 + 1);
        str = (char*)(const char*)buff; *str = 0; pos = 0;
        x = m_Root;
        if (!x)
        {
            //printf("empty\n");
            return V;
        }
    }
    if (x->m_bValue)
    {
        V += *(int*)x->m_Value;
        printf("%05d %-15s\n", *(int*)x->m_Value, str);
    }
    char* str2 = str + pos;
    if (str2 - str < 10 * 1024)
    {
        /*
        for (int i = 0; i < x->m_N; i++)
        {
            str2[0] = x->m_Letters[i];
            str2[1] = 0;
            V += PrintWords(str, x->m_Nodes[i], pos + 1);
            str2[0] = 0;
        }
        */
        for (int i = 0; i < 256; i++)
        {
            if (x->m_Nodes[i])
            {
                str2[0] = (char)(i + 0);
                str2[1] = 0;
                V += PrintWords(str, x->m_Nodes[i], pos + 1);
                str2[0] = 0;
            }
        }
    }
    return V;
}

#endif


#if 0
// CMaaTrie<int> test function
int main_test_CMaaTrie(int argn, const char * args[])
{
    printf("Tests:\n");
    {
        printf("Test1: empty tree: "); fflush(stdout);
        CMaaTrie<int> a;
        printf("."); fflush(stdout);
        printf("%s", a.Find("a", 1) < 0 ? "." : " fail "); fflush(stdout);
        printf("%s", a.Find("abc", 1) < 0 ? "." : " fail "); fflush(stdout);
        CMaaTrie<int>::Handle h = a.FindHandle("ab");
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
        CMaaTrie<int> a;
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
        const char * s[] = {"альфа", "бета", "гамма", "дельта", "эпсилон", "одна", "однаждыы", nullptr};
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
            printf("%s: %d %d ('%s', '%s') %d\n", (const char *)s, len, ilen, (const char *)p, (const char *)ip, v);
        }
        {
            CMaaString s = "однаждыыхх";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char *)s, len, ilen, (const char *)p, (const char *)ip, v);
            //return 0;
        }
        if  (0)
        {
            a.Print();
            printf("LQ:\n");
            CMaaTrie<int>::Handle hh = a.FindGQ("бета");
            printf("%p\n", hh);
        }
        bool bEqual = false;
        //bEqual = true;
        printf("Test2: values for key %s \"бета\": ", bEqual ? ">=" : ">");
        for (CMaaTrie<int>::Handle h = a.FindGQ("бета", bEqual); h; h = a.Next(h))
        {
            //printf("%p %d\n", h, a.GetValue(h));
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
    }
    printf("\n");
    {
        printf("Test3: similar words: ");
        CMaaTrie<int> a;
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
        for (CMaaTrie<int>::Handle h = a.FindGQ("zabc"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindGQ("za"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindGQ("AB"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindGQ("{"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindLQ("zabc"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindLQ("zabcd"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindLQ("zabce"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindLQ("zabcfg"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindLQ("Z"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie<int>::Handle h = a.FindLQ("{"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        a.SetDbg(false);
        //for (CMaaTrie<int>::iterator it = a.begin(); it; ++it)
        for (CMaaTrie<int>::iterator it = a.begin(); it != a.end(); ++it)
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
        }
        printf("\n");
        for (CMaaTrie<int>::iterator it = a.rbegin(); it != a.rend(); ++it)
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
        }
        printf("\n");
        for (CMaaTrie<int>::iterator it = a.begin(); it != a.end(); )
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
            CMaaTrie<int>::iterator r = it++;
            r.Remove();
        }
        printf("\n");
        for (CMaaTrie<int>::Handle p = a.First(); p; )
        {
            char key[1024 + 1];
            a.GetKey(p, key, (int)sizeof(key), true);
            printf("[%s] ", key);
            CMaaTrie<int>::Handle n = a.Next(p);
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
int main(int argn, const char * args[])
{
    return main_test_CMaaTrie(argn, args);
    //return main_test_CMaaTrie2(argn, args);
}
#endif

#if 0

// CMaaTrie2<int> test function
int main_test_CMaaTrie2(int argn, const char* args[])
{
    printf("Tests:\n");
    {
        printf("Test1: empty tree: "); fflush(stdout);
        CMaaTrie2<int> a;
        printf("."); fflush(stdout);
        printf("%s", a.Find("a", 1) < 0 ? "." : " fail "); fflush(stdout);
        printf("%s", a.Find("abc", 1) < 0 ? "." : " fail "); fflush(stdout);
        CMaaTrie2<int>::Handle h = a.FindHandle("ab");
        printf("%s", !h ? "." : " fail "); fflush(stdout);
        printf("%s", !a.Remove("a") ? "." : " fail "); fflush(stdout);
        printf("."); fflush(stdout);
#if 1
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
#endif
        printf("\n"); fflush(stdout);
        a.Print(); fflush(stdout);
        printf("~"); fflush(stdout);
    }
    printf(".\n"); fflush(stdout);
    printf("\n"); fflush(stdout);
    {
        printf("Test2: задание:\n");
        CMaaTrie2<int> a;
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
        const char* s[] = { "альфа", "бета", "гамма", "дельта", "эпсилон", "одна", "однаждыы", nullptr };
        printf("Find(not existed): %d\n", a.Find("бета"));
        printf("Remove(not existed): %s\n", a.Remove("бета") ? "true" : "false");
        int i;
        for (i = 0; s[i]; i++)
        {
            const bool b = a.Add(s[i], i + 1);
            printf("Add %s %d %s  ", s[i], i + 1, b ? "true" : "false");
            printf("//Find(\"бета\") = %d\n", a.Find("бета"));
        }
        {
            CMaaString s = "однажды";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char*)s, len, ilen, (const char*)p, (const char*)ip, v);
        }
        {
            CMaaString s = "однаждыыхх";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char*)s, len, ilen, (const char*)p, (const char*)ip, v);
            //return 0;
        }
#if 1
        if (1)
        {
            a.Print();
            printf("LQ:\n");
            CMaaTrie2<int>::Handle hh = a.FindGQ("бета");
            printf("%p\n", hh);
            a.PrintWords();
        }
        bool bEqual = false;
        //bEqual = true;
        printf("Test2: values for key %s \"бета\": ", bEqual ? ">=" : ">");
        for (CMaaTrie2<int>::Handle h = a.FindGQ("бета", bEqual); h; h = a.Next(h))
        {
            //printf("%p %d\n", h, a.GetValue(h));
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
#endif
    }
    printf("\n");
    {
        printf("Test3: similar words: ");
        CMaaTrie2<int> a;
        //a.SetDbg(true);
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
#if 1
        for (CMaaTrie2<int>::Handle h = a.FindGQ("zabc"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindGQ("za"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindGQ("AB"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindGQ("{"); h; h = a.Next(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindLQ("zabc"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindLQ("zabcd"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindLQ("zabce"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindLQ("zabcfg"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindLQ("Z"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle h = a.FindLQ("{"); h; h = a.Prev(h))
        {
            printf("%d ", a.GetValue(h));
        }
        printf("\n");
#endif
        //a.SetDbg(false);
        //for (CMaaTrie2<int>::iterator it = a.begin(); it; ++it)
        for (CMaaTrie2<int>::iterator it = a.begin(); it != a.end(); ++it)
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
        }
        printf("\n");
        for (CMaaTrie2<int>::iterator it = a.rbegin(); it != a.rend(); ++it)
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
        }
        printf("\n");
        for (CMaaTrie2<int>::iterator it = a.begin(); it != a.end(); )
        {
            char key[1024 + 1];
            it.GetKey(key, (int)sizeof(key), true);
            printf("[%s]:%d ", key, it.data());
            CMaaTrie2<int>::iterator r = it++;
            r.Remove();
        }
        printf("\n");
        for (CMaaTrie2<int>::Handle p = a.First(); p; )
        {
            char key[1024 + 1];
            a.GetKey(p, key, (int)sizeof(key), true);
            printf("[%s] ", key);
            CMaaTrie2<int>::Handle n = a.Next(p);
            a.Remove(p);
            p = n;
        }
        printf("\n");
        a.Print();
    }
    printf("\n");
    return 0;
}

int main(int argn, const char* args[])
{
    //return main_test_CMaaTrie(argn, args);
    return main_test_CMaaTrie2(argn, args);
}
#endif

#if 0

#define CMaaTrie_ver 1 // v.1 is the faster

#if CMaaTrie_ver == 1
#else
#define CMaaTrie CMaaTrie2
#endif

int main_pushkin(int argn, const char* args[])
{
    //main_test_CMaaTrie(argn, args);
    if (argn > 1)
    {
        CMaaTrie<int> a;
        CMaaUnivHash<CMaaString, int> ht;
        CMaaFile f(args[1], CMaaFile::eR_SrSw, eNoExcept); // "pushkin_stihi.txt"
        CMaaString txt;
        CMaaString RussianAlphabet = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюя";
        RussianAlphabet = Utf8ToAnsi(RussianAlphabet);
        CMaaString sp(nullptr, 256);
        for (int ch = 0; ch < 256; ch++)
        {
            sp[ch] = RussianAlphabet.Find((char)ch) < 0 ? (char)ch : ' ';
        }
        //sp += " \t\r\n";
        CMaaString ut = "игр";
        __utf8_printf("%S ", &ut);
        CMaaString t = Utf8ToAnsi(ut);
        int t0 = 0;// -1;
        CHrtMultiTimer tmr;
        tmr.Start();
        while (txt.IsNotEmpty() || f.IsOpen())
        {
            if (txt.Length() < 1 * 1024 && f.IsOpen())
            {
                txt += f.Read(4 * 1024);
                if (f.IsEOF())
                {
                    f.Close();
                }
            }
            CMaaString w = txt.GetWord(true, true, sp).ToLower(1251);
            CMaaString u = UnicodeToUtf8(AnsiToUnicode(w));
            int v = 0;
            a.Find(w, w.Length(), &v);
            v++;
            if (v <= 0)
            {
                __utf8_printf("%-15S == %d\n", &u, v);
            }
            a.Add(w, w.Length(), v);
            int v2 = a.Find(w, w.Length());
            if (v2 != v)
            {
                __utf8_printf("%-15S %d <> %d\n", &u, v2, v);
            }
            else
            {
                //__utf8_printf("%-15S %d\n", &u, v2);
            }
            int vt = 0;
            ht.Find(w, &vt);
            vt++;
            ht.AddOver(w, vt);
            if (vt != v)
            {
                __utf8_printf("%-15S %d <> %d(ht)\n", &u, v, vt);
            }
            if (w == t)
            {
                if (!++t0)
                {
                    t0++;
                }
            }
            v = a.Find(t, t.Length());
            if (t0 != v)
            {
                __utf8_printf("after %S #%d: %S = %d <> %d\n", &u, v2, &ut, v, t0);
            }
        }
        tmr.Stop("Add/Find");
        int V = a.PrintWords();
        fflush(stdout);
        __utf8_printf("   %d\n", V);

        V = 0;
#if CMaaTrie_ver == 1
        const char* p_min_1 = "\x80";
        const char* p_max_12 = "\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f";
#else
        const char* p_min_1 = "\x00";
        const char* p_max_12 = "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";
#endif
        const int p_max_len = (int)strlen(p_max_12);

        int N = a.Find(p_min_1, 1, &V) ? 1 : 0;
        //gbDbgTrie10Nodes = true;
        CMaaTrie<int>::Handle h0 = NULL;
        tmr.Start();
        for (CMaaTrie<int>::Handle h = a.FindGQ(p_min_1, 1); h; h = a.Next(h))
        {
            if (!h0)
            {
                h0 = h;
                __utf8_printf("%p ", h);
            }
            N++;
            V += a.GetValue(h);
            //__utf8_printf("%d\n", N);
        }
        tmr.Stop("Next");
        __utf8_printf("%d %d >\n", N, V);

        V = 0;
        N = a.Find(p_max_12, p_max_len, &V) ? 1 : 0;
        //gbDbgTrie10Nodes = true;
        h0 = NULL;
        tmr.Start();
        for (CMaaTrie<int>::Handle h = a.FindLQ(p_max_12, p_max_len); h; h = a.Prev(h))
        {
            if (!h0)
            {
                h0 = h;
                __utf8_printf("%p ", h);
            }
            N++;
            V += a.GetValue(h);
            //__utf8_printf("%d\n", N);
        }
        tmr.Stop("Prev");
        __utf8_printf("%d %d <\n", N, V);
        //__utf8_printf("%d\n", a.Find(t, t.Length()));

        CMaaString res = tmr.GetResult("CMaaTrie<int>:\n");
        __utf8_printf("%S\n", &res);

        V = 0; N = 0;
        CMaaUnivHash<CMaaString, int>::iterator it(ht);
        for (; it; ++it)
        {
            N++;
            V += it.data();
        }
        __utf8_printf("%d %d ht\n", N, V);

        h0 = a.First(); __utf8_printf("%p ", h0);
        h0 = a.Last(); __utf8_printf("%p\n", h0);

        CMaaPtrAE<CMaaTrie<int>::Handle> m(N);
        int idx = 0;
        for (CMaaTrie<int>::Handle h = a.First(); h; h = a.Next(h))
        {
            m[idx] = h;
            a.SetValue(h, idx);
            idx++;
        }
        CMaaString w1, w2;
        for (CMaaTrie<int>::Handle h = a.Last(); h; h = a.Prev(h))
        {
            --idx;
            if (m[idx] != h)
            {
                __utf8_printf("%4d %p:%d <> %p:%d\n", idx, h, a.GetValue(h), (CMaaTrie<int>::Handle)m[idx], a.GetValue(m[idx]));
            }
            char k[256];
            *k = 0;
            a.GetKey(h, k, 256);
            w2 = k;
            if (w1 < w2)
            {
                __utf8_printf("%4d %p %4d %S < %S\n", idx, h, a.GetValue(h), &w1, &w2);
            }
            w1 = w2;
        }

        return 0;
    }
    return 0;
}

#endif