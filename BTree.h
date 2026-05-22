
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/BTree.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file is Memory and disk BTree templates.
 * Originally converted from Niklaus Wirth "Algorithms and data
 * structures", rus: 2001
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

#ifndef __TOOLS_BTREE_H
#define __TOOLS_BTREE_H

#if defined(_WIN64) || defined(_____AMD64_____)
//typedef _qword BTreeMemoryPageNum;
#else
//typedef _sdword BTreeMemoryPageNum;
#endif

template <class Item, class PageNum = char *> class CMaaBTreeAllocator
{
public:
    int m_n, m_nn;
    int m_DiskPageSize;
    int m_AllocatedBlocks;

    PageNum m_PagesCount;

    PageNum m_Num0, m_Root;

    CMaaUnivHash<PageNum, char *> m_hLoadedPages;

    char * m_pMemList;

    struct Page;

    CMaaBTreeAllocator(int n) // 2*n - max number of items on the page, if n < 0, then -n == sizeof(disk file page)
    {
        m_Num0 = 0;
        m_Root = m_Num0;
        m_DiskPageSize = 0;
        m_AllocatedBlocks = 0;
        m_pMemList = nullptr;
        if  (n < 0)
        {
            m_DiskPageSize = -n;
            n = (m_DiskPageSize - sizeof(_sdword) - sizeof(PageNum)) / (2 * (sizeof(Item) + sizeof(PageNum)));
            if  (n < 1)
            {
                n = 1;
                m_DiskPageSize = 0;
            }
        }
        m_n = n;
        m_nn = 2 * n;
        if  (m_DiskPageSize == 0)
        {
            m_DiskPageSize = sizeof(_sdword) + m_nn*sizeof(Item) + (m_nn+1)*sizeof(PageNum);
        }
        m_PagesCount = 0;
    }

    virtual ~CMaaBTreeAllocator()
    {
        /*int c =*/ DeleteAllPageMem();
        //printf("DeleteAllPageMem() returns %d\n", c);
        /*size_t cc =*/ m_hLoadedPages.GetMaxIndex();
        //printf("m_hLoadedPages.GetMaxIndex() returns %d\n", cc);
    }

    virtual PageNum GetRootPageNum()
    {
        return m_Root;
    }

    virtual void SetRootPageNum(PageNum Num)
    {
        m_Root = Num;
    }

    char * AllocPageMem()
    {
        char * p = m_pMemList;
        if  (p)
        {
            m_pMemList = *(char **)p;
        }
        else
        {
            int sz = sizeof(int) + sizeof(_sdword) + m_nn*sizeof(Item) + (m_nn+1)*sizeof(PageNum);
            p = TL_NEW char [sz];
        }
        return p;
    }

    void FreePageMem(char * p)
    {
        if  (p)
        {
            *(char **)p = m_pMemList;
            m_pMemList = p;
        }
    }

    int DeleteAllPageMem()
    {
        int c = 0;
        while(m_pMemList)
        {
            char * p = m_pMemList;
            m_pMemList = *(char **)p;
            delete []p;
            c++;
        }
        return c;
    }

    virtual char * Load(PageNum Num)
    {
        if  (Num == m_Num0)
        {
            return nullptr;
        }
        char * ptr = m_hLoadedPages[Num, nullptr];
        if  (!ptr)
        {
            // Read from disk
            ptr = (char *)Num;
            m_hLoadedPages.AddOver(ConvertPageNumToPageNum(Num), ptr);
        }
        (*(int *)ptr)++;
        return ptr;
    }

    virtual PageNum New()
    {
        m_PagesCount++;
        int sz = sizeof(int) + sizeof(_sdword) + m_nn*sizeof(Item) + (m_nn+1)*sizeof(PageNum);
        char * ptr = AllocPageMem(); //TL_NEW char[sz];
        if  (!ptr)
        {
            throw "Memory allocation error";
        }
        m_AllocatedBlocks++;
        memset(ptr, 0, sz);
        return (PageNum)ptr;
    }

    virtual void UnLoad(PageNum Num, char * ptr)
    {
        if  (ptr)
        {
            if  (--(*(int *)ptr) <= 0)
            {
                //Write to disk
                m_hLoadedPages.Remove(Num);
            }
        }
    }

    virtual void Dispose(PageNum Num, char * ptr)
    {
        m_PagesCount--;
        m_hLoadedPages.Remove(Num);
        //delete [] ptr;
        FreePageMem(ptr);
        m_AllocatedBlocks--;
    }

    virtual PageNum ConvertPageNumToPageNum(PageNum Num)
    {
        return Num;
    }

    virtual bool IsValidPageNum(PageNum /*Num*/)
    {
        return true;
    }

    struct Page
    {
        CMaaBTreeAllocator &m_a;
        PageNum m_Num;
        char    *m_ptr;
        _sdword &m;   // number of items in the page
        Item    *e;
        PageNum *p;

        Page(CMaaBTreeAllocator &a, PageNum Num)
        :   m_a(a),
            m_Num(a.ConvertPageNumToPageNum(Num)),
            m_ptr(a.Load(Num)),
            m(*(_sdword *)(m_ptr + sizeof(int)))
        {
            if  (m_ptr)
            {
                e = (Item *)(&m + 1);
                p = (PageNum *)(e + a.m_nn);
                e--;
                if  (m < 0 || m > m_a.m_nn)
                {
                    throw "Integrity error";
                }
                for (int i = 0; i <= (m > 0 ? m : -1); i++)
                {
                    if  (!m_a.IsValidPageNum(p[i]))
                    {
                        throw "Integrity error";
                    }
                }
            }
            else
            {
                e = nullptr;
                p = nullptr;
            }
        }

        ~Page()
        {
            m_a.UnLoad(m_Num, m_ptr);
        }
        operator PageNum()
        {
            return m_Num;
        }
        void Delete()
        {
            //printf ( "Delete page %p\n", pThis );
            m_a.Dispose(m_Num, m_ptr);
            m_Num = m_a.m_Num0;
            //
            m_ptr = nullptr;  //m = * ( int * ) pThis;
            e = nullptr;
            p = nullptr;
        }
        void Move(int k, int i, int cnt, const Page &That)
        {
            if  (cnt < 0 || k + cnt > m_a.m_nn + 1 || i + cnt > m_a.m_nn + 1)
            {
                // error
                return;
            }
            memmove(&e[k], &That.e[i], cnt * sizeof(*e));
            memmove(&p[k], &That.p[i], cnt * sizeof(*p));
        }
        void Move(int k, int i, int cnt)
        {
            Move(k, i, cnt, *this);
        }
    };
};

template <class Item, class PageNum = _sdword> class CMaaBTreeFileAllocator : public CMaaBTreeAllocator<Item, _sdword>
{
protected:
    CMaaString m_PagesFileName, m_IndexFileName;
public:
    CMaaFile m_fPages, m_fIndex;
    PageNum m_MaxPages;
    CMaaPtr_<char, 1> m_EmptyDiskPageTemplate, m_TempDiskPageTemplate;

    //typedef _sdword PageNum;

    CMaaBTreeFileAllocator(int n, CMaaString FileName = "CMaaBTree", const char * pszMode = "RWCD|SrSw") // 2*n - max number of items on the page, if n < 0, then -n == sizeof(disk file page)
    :   CMaaBTreeAllocator<Item, _sdword>(n),
        m_fPages(m_PagesFileName = FileName + ".pages", pszMode),
        m_fIndex(m_IndexFileName = FileName + ".index", pszMode),
        m_EmptyDiskPageTemplate(CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize),
        m_TempDiskPageTemplate(CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize)
    {
        m_MaxPages = 0;
        CMaaBTreeAllocator<Item, _sdword>::m_Num0 = -1;
        memset(CMaaBTreeAllocator<Item, _sdword>::m_EmptyDiskPageTemplate, 0, CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize);

        ScanIndex();
    }

    PageNum GetRootPageNum()
    {
        PageNum x = CMaaBTreeAllocator<Item, _sdword>::m_Num0;
        m_fPages.Seek(0);
        if  (m_fPages.Read(m_TempDiskPageTemplate, CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize) != CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize)
        {
            x = CMaaBTreeAllocator<Item, _sdword>::m_Num0;
        }
        else
        {
            x = *(PageNum *)(char *)CMaaBTreeAllocator<Item, _sdword>::m_TempDiskPageTemplate;
        }
        if  (!IsValidPageNum(x))
        {
            throw "Bad Root page number";
        }
        return x;
    }

    void SetRootPageNum(PageNum Num)
    {
        char * ptr = m_TempDiskPageTemplate;
        m_fPages.Seek(0);
        if  (m_fPages.Read(ptr, CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize) != CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize)
        {
            memset(ptr, 0, CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize);
            *(PageNum *)ptr = Num - 1;
        }
        if  (*(PageNum *)ptr != Num)
        {
            m_fPages.Seek(0);
            *(PageNum *)ptr = Num;
            if  (m_fPages.Write(ptr, CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize) != CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize)
            {
                throw "Write Root page error";
            }
        }
    }

    void RemoveFiles()
    {
        m_fPages.Close();
        m_fIndex.Close();
        CMaaFile::Remove(m_PagesFileName, false);
        CMaaFile::Remove(m_IndexFileName, false);
    }

    void ScanIndex()
    {
        unsigned char tmp[1024], c;
        m_fIndex.Seek(0);
        m_MaxPages = (int)(m_fPages.Length() / (_qword)CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize) - 1;
        m_fIndex.Seek(0);
        CMaaBTreeAllocator<Item, _sdword>::m_PagesCount = 0;
        PageNum Pages = 0;
        while(1)
        {
            int x = (int)m_fIndex.Read(tmp, sizeof(tmp));
            for (int i = 0; i < x; i++)
            {
                c = tmp[i];
                for (int j = 0; j < 8; j++)
                {
                    if  (Pages < m_MaxPages)
                    {
                        if  ((tmp[i] & (1 << j)))
                        {
                            CMaaBTreeAllocator<Item, _sdword>::m_PagesCount++;
                        }
                    }
                    else
                    {
                        c &= ~(1 << j);
                    }
                    Pages++;
                }
                if  (Pages >= m_MaxPages && c != tmp[i])
                {
                    m_fIndex.Seek(Pages / 8 - 1);
                    m_fIndex.Write(&c, 1);
                    m_fIndex.Truncate();
                    break;
                }
            }
            if  (x < (int)sizeof(tmp))
            {
                memset(tmp, 0, sizeof(tmp));
                int y = (m_MaxPages + 7 - Pages) / 8;
                while(y > 0)
                {
                    x = y < (int)sizeof(tmp) ? (int)sizeof(tmp) : y;
                    int z = m_fIndex.Write(tmp, x);
                    y -= z;
                    if  (z != x)
                    {
                        // throw error
                        break;
                    }
                }
                break;
            }
        }
        m_fIndex.Seek(0);
    }

    PageNum ConvertPageNumToPageNum(PageNum Num)
    {
        return Num >= 0 ? Num : Num == CMaaBTreeAllocator<Item, _sdword>::m_Num0 ? Num : -2 - Num;
    }

    _qword FilePageOffset(PageNum Num, PageNum Add = 0)
    {
        return ((_qword)Num + (_qword)Add + 1) * (_qword)CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize;
    }

    bool IsValidPageNum(PageNum Num)
    {
        return Num == CMaaBTreeAllocator<Item, _sdword>::m_Num0 || FilePageOffset(Num, 1) <= m_fPages.Length();
    }

    PageNum New()
    {
        int a = (int)m_fIndex.CurPos();
        int b = (int)m_fIndex.Length();
        unsigned char tmp[1024];
        PageNum Page = -1;
        int pos = a;
        while(pos < b)
        {
            int x = m_fIndex.Read(tmp, b - pos > (int)sizeof(tmp) ? (int)sizeof(tmp) : b - pos);
            for (int i = 0; i < x; i++)
            {
                if  (tmp[i] != 0xff)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if  ((tmp[i] & (1 << j)) == 0)
                        {
                            Page = (pos + i) * 8 + j;
                            i = x;
                            pos = b;
                            break;
                        }
                    }
                }
            }
            pos += x;
            if  (x <= 0)
            {
                break;
            }
        }
        if  (Page == -1)
        {
            m_fIndex.Seek(0);
            pos = 0;
            while(pos < a)
            {
                int x = m_fIndex.Read(tmp, a - pos > (int)sizeof(tmp) ? (int)sizeof(tmp) : a - pos);
                for (int i = 0; i < x; i++)
                {
                    if  (tmp[i] != 0xff)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            if  ((tmp[i] & (1 << j)) == 0)
                            {
                                Page = (pos + i) * 8 + j;
                                i = x;
                                pos = a;
                                break;
                            }
                        }
                    }
                }
                pos += x;
                if  (x <= 0)
                {
                    break;
                }
            }
        }
        if  (Page == -1)
        {
            m_fIndex.Seek(b);
            tmp[0] = 0;
            m_fIndex.Write(&tmp[0], 1);
            Page = b * 8;
            b++;
        }

        m_fPages.Seek(FilePageOffset(Page));
        m_fPages.Write(CMaaBTreeAllocator<Item, _sdword>::m_EmptyDiskPageTemplate, CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize);
        m_MaxPages = (PageNum)(m_fPages.Length() / (_qword)CMaaBTreeAllocator<Item, _sdword>::m_DiskPageSize) - 1;

        m_fIndex.Seek(Page / 8);
        m_fIndex.Read(&tmp[0], 1);
        tmp[0] |= (1 << (Page & 7));
        m_fIndex.Seek(Page / 8);
        m_fIndex.Write(&tmp[0], 1);

        Page = -2 - Page;
        CMaaBTreeAllocator<Item, _sdword>::m_PagesCount++;

        return Page;

        /*
          int sz = 2 * sizeof(int) + m_nn*sizeof(Item) + (m_nn+1)*sizeof(PageNum);
          char * ptr = TL_NEW char[sz];
          if   (!ptr)
          {
               throw "Memory allocation error";
          }
          memset(ptr, 0, sz);
          return (PageNum)ptr;
          */
    }

    char * Load(PageNum Num)
    {
        if  (Num == CMaaBTreeAllocator<Item, _sdword>::m_Num0)
        {
            return nullptr;
        }
        if  (m_fPages.Length() < FilePageOffset(ConvertPageNumToPageNum(Num), 1))
        {
            throw "Integrity error";
        }
        char * ptr = nullptr;
        CMaaBTreeAllocator<Item, _sdword>::m_hLoadedPages.Find(ConvertPageNumToPageNum(Num), &ptr);
        if  (!ptr)
        {
            // Read from disk
            int sz = sizeof(int) + sizeof(_sdword) + CMaaBTreeAllocator<Item, _sdword>::m_nn*sizeof(Item) + (CMaaBTreeAllocator<Item, _sdword>::m_nn+1)*sizeof(PageNum);
            ptr = CMaaBTreeAllocator<Item, _sdword>::AllocPageMem(); //TL_NEW char[sz];
            if  (!ptr)
            {
                throw "Memory allocation error";
            }
            CMaaBTreeAllocator<Item, _sdword>::m_AllocatedBlocks++;
            if  (Num < 0)
            {
                memset(ptr, 0, sz);
            }
            else
            {
                m_fPages.Seek(FilePageOffset(Num));
                *(int *)ptr = 0;
                if  (m_fPages.Read(ptr + sizeof(int), sz - sizeof(int)) != sz - sizeof(int))
                {
                    throw "CMaaBTreeFileAllocator::Load() file read error";
                }
            }
            CMaaBTreeAllocator<Item, _sdword>::m_hLoadedPages.AddOver(ConvertPageNumToPageNum(Num), ptr);
        }
        (*(int *)ptr)++;
        return ptr;
    }

    void UnLoad(PageNum Num, char * ptr)
    {
        if  (ptr && Num != CMaaBTreeAllocator<Item, _sdword>::m_Num0)
        {
            if  (--(*(int *)ptr) <= 0)
            {
                //Write to disk
                m_fPages.Seek(FilePageOffset(Num));
                int sz = sizeof(int) + sizeof(_sdword) + CMaaBTreeAllocator<Item, _sdword>::m_nn*sizeof(Item) + (CMaaBTreeAllocator<Item, _sdword>::m_nn+1)*sizeof(PageNum);
                char *ptr2 = CMaaBTreeAllocator<Item, _sdword>::AllocPageMem(); //TL_NEW char[sz];
                if  (ptr2)
                {
                    *(int *)ptr2 = 0;
                    if  (m_fPages.Read(ptr2 + sizeof(int), sz - sizeof(int)) != sz - sizeof(int))
                    {
                        throw "CMaaBTreeFileAllocator::UnLoad() file check read error";
                    }
                    m_fPages.Seek(FilePageOffset(Num));
                }
                if  (!ptr2 || !CMaaIsMemEq(ptr, ptr2, sz))
                {
                    if  (m_fPages.Write(ptr + sizeof(int), sz - sizeof(int)) != sz - sizeof(int))
                    {
                        throw "CMaaBTreeFileAllocator::UnLoad() file write error";
                    }
                }
                CMaaBTreeAllocator<Item, _sdword>::FreePageMem(ptr2);
                //delete [] ptr;
                CMaaBTreeAllocator<Item, _sdword>::FreePageMem(ptr);
                CMaaBTreeAllocator<Item, _sdword>::m_AllocatedBlocks--;
                CMaaBTreeAllocator<Item, _sdword>::m_hLoadedPages.Remove(Num);
            }
        }
    }

    void Dispose(PageNum Num, char * ptr)
    {
        if  (Num != CMaaBTreeAllocator<Item, _sdword>::m_Num0)
        {
            unsigned char c;
            m_fIndex.Seek(Num / 8);
            m_fIndex.Read(&c, 1);
            c &= ~(1 << (Num & 7));
            m_fIndex.Seek(Num / 8);
            m_fIndex.Write(&c, 1);
            CMaaBTreeAllocator<Item, _sdword>::m_hLoadedPages.Remove(Num);
            CMaaBTreeAllocator<Item, _sdword>::m_PagesCount--;
        }
        //delete [] ptr;
        CMaaBTreeAllocator<Item, _sdword>::FreePageMem(ptr);
        CMaaBTreeAllocator<Item, _sdword>::m_AllocatedBlocks--;
    }
};


template <class A, class Item, class PageNum = char *> class CMaaBTree
{
public:
    typedef typename A::Page Page;

    struct ItemEx
    {
        Item v;
        operator Item & ()
        {
            return (Item &)v;
        }
        PageNum p;
    };

private:
    PageNum Root;
    //int h;
    int ins;
    int fLQ_GQ;

    A &m_alloc;

    int k,l;
    //int m_fFound;

    int &n, &nn;

    bool bDeleteOnClose;


public:

    CMaaBTree(A &a, bool _bDeleteOnClose = false)
    :   m_alloc(a),
        n(m_alloc.m_n),
        nn(m_alloc.m_nn),  // nn = 2 * n;
        bDeleteOnClose(_bDeleteOnClose)
    {
        Root = m_alloc.GetRootPageNum();
    }

    // begin Functions like in CMaaUnivHash, CMaaRBTree
    // 0 - success, 1 - fail (key is already exists)
    int Add(const Item &x)
    {
        Item v;
        return Search(x, v, 1) == 0 ? 0 : 1;
    }
    // 0 - success add, 1 - fail (key is already exists, return v)
    int AddFind(const Item &x, Item &v)
    {
        return Search(x, v, 1) == 0 ? 0 : 1;
    }
    // 0 - success (return v), 1 - fail (key is not exists)
    int Find(const Item &x, Item *v = nullptr)
    {
        Item vv;
        v = v ? v : &vv;
        return Search(x, *v, 0) > 0 /* == 1 */ ? 0 : 1;
    }
    // 0 - success (return v - deleted item), 1 - fail (key is not exists)
    int Remove(const Item &x, Item *v = nullptr)
    {
        return (v ? Delete(x, *v) : Delete(x)) > 0 /* == 1 */ ? 0 : 1;
    }
    // 0 - success (maximum key v <= x), 1 - fail (key is not exists)
    int FindLQ(const Item &x, Item *v = nullptr)
    {
        Item vv;
        v = v ? v : &vv;
        return Search(x, *v, -1) > 0 /* == 1 */ ? 0 : 1;
    }
    // 0 - success (minimum key v >= x), 1 - fail (key is not exists)
    int FindGQ(const Item &x, Item *v = nullptr)
    {
        Item vv;
        v = v ? v : &vv;
        return Search(x, *v, 2) > 0 /* == 1 */ ? 0 : 1;
    }
    /*
     // (sample fn) 0 - success (maximum key v < x for int key), 1 - fail (key is not exists)
     int FindLnQ(const Item &x, Item *v = nullptr)
     {
          Item xx = x - 1;
          Item vv;
          v = v ? v : &vv;
          return Search(xx, *v, -1) > 0   / * == 1 * /   ? 0 : 1;
     }
     // (sample fn) 0 - success (minimum key v > x for int key), 1 - fail (key is not exists)
     int FindGnQ(const Item &x, Item *v = nullptr)
     {
          Item xx = x + 1;
          Item vv;
          v = v ? v : &vv;
          return Search(xx, *v, 2) > 0   / * == 1 * /   ? 0 : 1;
     }
     */
    // end of Functions like in CMaaUnivHash

    ~CMaaBTree()
    {
        if  (bDeleteOnClose)
        {
            int c = RemoveAll();
            if  (c)
            {
                //printf("CMaaBTree::~CMaaBTree() deleted %d items\n", c);
            }
            //printf("m_alloc.m_hLoadedPages.GetItemCount() == %d\n", m_alloc.m_hLoadedPages.GetItemCount());
            //printf("m_alloc.m_AllocatedBlocks == %d\n", m_alloc.m_AllocatedBlocks);
        }
    }

    int RemoveAll()
    {
        int c = 0;

        CMaaPtr_<Item, 1> k(nn); // ", 1 xThrow" 2025
        while(1)
        {
            int i, j;
            if  (Root == m_alloc.m_Num0)
            {
                break;
            }
            {
                Page p(m_alloc, Root);
                j = p.m;
                for (i = 0; i < j; i++)
                {
                    k[i] = p.e[i + 1];
                }
            }
            for (i = 0; i < j; i++)
            {
                Delete(k[i]);
                c++;
            }
        }
        return c;
    }

    int Search(const Item &x, Item &v, int Ins = 0)
    {
        ins    = Ins;
        fLQ_GQ = Ins == 1 ? 0 : Ins;
        //m_fFound = 0;
        int h = 0;
        ItemEx u;
        (Item &)u = v;

        Search(x, Root, h, u);

        if  ((h & 1) && ins == 1) //2009
        {
            // включение новой корневой страницы
            Page pg(m_alloc, m_alloc.New());
            PageNum q = Root;
            Root = pg;
            pg.m = 1;
            pg.p[0] = q;
            pg.e[1] = u;
            pg.p[1] = u.p;
            m_alloc.SetRootPageNum(Root);
        }

        // avoid v is purged on insert
        if  ((h & (2|4)))
        {
            v = u;
            return 1;
        }
        return 0;
        /*
          if ( ((h & (2|4)) && m_fFound) || (!(h & (2|4)) && !m_fFound) )
          {
               return m_fFound;
          }
          else
          {
               printf("h=%d m_fFound=%d error\n", h, m_fFound);
               return m_fFound;
          }
          */
    }

    int Delete(const Item &x)
    {
        Item v;
        return Delete(x, v);
    }

    int Delete(const Item &x, Item &v)
    {
        int h = 0, fnd = 0;
        //m_fFound = 0;
        Delete(x, Root, h, fnd, v);
        if  (h)
        {
            // уменьшен размер корневой страницы
            Page a(m_alloc, Root);
            if  (a.m == 0)
            {
                PageNum p = a.p[0];
                a.Delete();
                Root = p;
                m_alloc.SetRootPageNum(Root);
            }
        }
        return fnd;//m_fFound;
    }

    void PrintTree()
    {
        int l = 80 / 2 - n * 3;
        for (int h = 0, f = 1; f; h++)
        {
            f = -1;
            l = PrintTree(Root, l, 0, h, f);
            if  (l < 80)
            {
                for (int i = 0; i < (80 - l) / 2; i++)
                {
                    printf(" ");
                }
            }
            f = 0;
            PrintTree(Root, l, 0, h, f);
            l /= nn;
            if  (f == 1)
            {
                printf("\n");
            }
        }
    }

    // v is purged on insert
    int Search(const Item &x, PageNum p_a, int &h, ItemEx &v)
    {
        if  (p_a == m_alloc.m_Num0)
        {
            // Элемента с ключом x нет в дереве
            if  (ins)
            {
                h = 1;
                (Item &)v = x;
                v.p = m_alloc.m_Num0;
            }
            return h;
        }
        Page a(m_alloc, p_a);

        // Поиск ключа x на странице a; h-false
        int k,l,r;
        l = 1;
        r = a.m;
        do
        {
            // двоичный поиск в массиве
            k = (l + r) / 2;
            if  (a.e[k] >= x)
            {
                r = k - 1;
            }
            if  (a.e[k] <= x)
            {
                l = k + 1;
            }

        }    while(r >= l);

        if  (l - r > 1)
        {
            //h = 0; // old
            h = 4; // "found"
            //m_fFound++;
            //Found(a.e[k]);
            (Item &)v = a.e[k];   // return a.e [ k ] into v
            if  (fLQ_GQ)
            {
                h = 2;
            }
        }
        else
        {
            //ItemEx u;
            //Search(x, a.p[r], h, u);
            Search(x, a.p[r], h, v);
            if  (h)
            {
                if  (h == 4)
                {
                    // item found (fix 2015)
                    //v = u; // вытягиваем u наверх
                    return h;
                }

                if  (ins == 1)
                {
                    ItemEx u = v;
                    Insert(a, u, v, r, h); // ret: h is unchanged or h == 0
                    return h;
                }

                if  (h == 2)
                {
                    // h == 2 only if key is found and fLQ_GQ != 0 (ins != 0 && ins != 1)
                    //v = u; // вытягиваем u наверх
                    return h;
                }

                if  (fLQ_GQ < 0)
                {
                    if  (r > 0)
                    {
                        //m_fFound++;
                        //Found(a.e[r]);
                        (Item &)v = a.e[r];
                        //v.p = nullptr;
                        h = 2;
                    }
                    else
                    {
                        // not found
                        h = 8;
                    }
                }
                else if ( fLQ_GQ > 0 )
                {
                    if  (l <= a.m)
                    {
                        //m_fFound++;
                        //Found(a.e[l]);
                        (Item &)v = a.e[l];
                        //v.p = nullptr;
                        h = 2;
                    }
                    else
                    {
                        // upper element witch refered to page a
                        // h = 1 без изменений
                    }
                }
            }
        }
        return h;
    }

    /*
     void Found(Item &i)
     {
          //v.count++;
     }
     */

    void Insert(Page &a, const ItemEx &u, ItemEx &v, int r, int &h)
    {
        // Включение u справа от a.e [ r ]
        //int i;
        if  (a.m < nn)
        {
            a.m++;
            h = 0;
            a.Move(r + 2, r + 1, a.m - r - 1);
            a.e[r+1] = (const Item &)u;
            a.p[r+1] = u.p;
            return;
        }
        // Страница a заполнена, расщепляем ее и присваиваем полученный
        // элемент v
        Page b(m_alloc, m_alloc.New());
        if  (r <= n)
        {
            if  (r == n)
            {
                v = u;
            }
            else
            {
                (Item &)v = a.e[n];
                v.p = a.p[n];
                a.Move(r + 2, r + 1, n - (r + 1));
                a.e[r + 1] = (const Item &)u;
                a.p[r + 1] = u.p;
            }
            b.Move(1, 1 + n, n, a);
        }
        else
        {
            // r > n
            // включение в первую страницу
            r = r - n;
            (Item &)v = a.e[n + 1];
            v.p = a.p[n + 1];
            b.Move(1, n + 2, r - 1, a);
            b.e[r] = (const Item &)u;
            b.p[r] = u.p;
            b.Move(r + 1, r + 1 + n, n - r, a);
        }
        a.m = b.m = n;
        b.p[0] = v.p;
        v.p = b;
    }

    int Delete(const Item &x, PageNum p_a, int &h, int &fnd, Item &v)
    {
        if  (p_a == m_alloc.m_Num0)
        {
            // key is not in the tree
            h = 0;
            return h;
        }
        Page a(m_alloc, p_a);
        l = 1;
        int r = a.m;
        do
        {
            // двоичный поиск в массиве
            k = (l + r) / 2;
            if  (a.e[k] >= x)
            {
                r = k - 1;
            }
            if  (a.e[k] <= x)
            {
                l = k + 1;
            }

        }    while(r >= l);

        PageNum q = a.p[r];

        if  (l - r > 1)
        {
            // we found a.e[k] with key x
            //m_fFound++;
            fnd++;
            v = a.e[k];
            if  (q == m_alloc.m_Num0)
            {
                // a - терминальная страница
                a.m--;
                h = a.m < n ? 1 : 0;
                a.Move(k, k + 1, a.m - k + 1);
            }
            else
            {
                Del(q, a, h);
                if  (h)
                {
                    UnderFlow(a, q, r, h);
                }
            }
        }
        else
        {
            Delete(x, q, h, fnd, v);
            if  (h)
            {
                UnderFlow(a, q, r, h);
            }
        }
        return h;
    }

    int Del(PageNum p_p, PageNum p_a, int &h)
    {
        Page p(m_alloc, p_p), a(m_alloc, p_a);
        PageNum q = p.p[p.m];
        if  (q != m_alloc.m_Num0)
        {
            Del(q, a, h);
            if  (h)
            {
                UnderFlow(p, q, p.m, h);
            }
        }
        else
        {
            //p.p [ p.m ] = a.p [ k ];
            a.e[k] = p.e[p.m];
            p.m--;
            h = p.m < n ? 1 : 0;
        }
        return h;
    }

    void UnderFlow(PageNum p_c, PageNum p_a, int s, int &h)
    {
        Page c(m_alloc, p_c), a(m_alloc, p_a);
        // a - страница с нехваткой, c - страница-предок
        int /*i,*/ k, mb, mc;

        mc = c.m; // h = true, a.m = n - 1
        if  (s < mc)
        {
            // b - страница справа от a
            s++;
            Page b(m_alloc, c.p[s]);
            mb = b.m;
            k = (mb - n + 1) / 2;
            // k - число элементов на соседней странице b
            a.e[n] = c.e[s];
            a.p[n] = b.p[0];
            if  (k > 0)
            {
                // пересылка k элементов с b на a
                a.Move(n + 1, 1, k - 1, b);
                c.e[s] = b.e[k];
                c.p[s] = b;
                b.p[0] = b.p[k];
                mb -= k;
                b.Move(1, 1 + k, mb);
                b.m = mb;
                a.m = n - 1 + k;
                h = 0;
            }
            else
            {
                // слияние страниц a и b
                a.Move(1 + n, 1, n, b);
                c.Move(s, s + 1, mc - s);
                a.m = nn;
                c.m = mc - 1;
                b.Delete();
                h = c.m < n ? 1 : 0;
            }
        }
        else
        {
            // s >= mc
            // b - страница слева от a
            Page b(m_alloc, c.p[s - 1]);
            mb = b.m + 1;
            k = (mb - n) / 2;
            if  (k > 0)
            {
                // пересылка k элементов с b на a
                a.Move(1 + k, 1, n - 1); // a.m == n - 1
                a.e[k] = c.e[s];
                a.p[k] = a.p[0];
                mb -= k;
                a.Move(1, 1 + mb, k - 1, b);
                a.p[0] = b.p[mb];
                c.e[s] = b.e[mb];
                c.p[s] = a;
                b.m = mb - 1;
                a.m = n - 1 + k; // += k;
                h = 0;
            }
            else
            {
                // слияние страниц a и b
                b.e[mb] = c.e[s];
                b.p[mb] = a.p[0];
                b.Move(1 + mb, 1, n - 1, a);
                b.m = nn;
                c.m = mc - 1;
                a.Delete();
                h = c.m < n ? 1 : 0;
            }
        }
    }

    int PrintTree(PageNum p_p, int l, int h0, int h, int &f)
    {
        if  (p_p != m_alloc.m_Num0)
        {
            Page p(m_alloc, p_p);
            int i;
            int ll = 0;
            if  (!(h0 == h || h == -1))
            {
                for (i = 0; i <= p.m; i++)
                {
                    ll += PrintTree(p.p[i], l, h0 + 1, h, f);
                }
                return ll;
            }
            else
            {
                for (i = 0; i <= p.m; i++)
                {
                    int f = -1;
                    ll += PrintTree(p.p[i], l, h0 + 1, h + 1, f);
                }
                char txt [ 10 * 1024 ];
                * txt = 0;
                //sprintf ( strchr ( txt, 0), "%d", p.m );
                for ( i = 1; i <= p.m; i++ )
                {
                    if  (strlen(txt) >= (int)sizeof(txt) - 100)
                    {
                        strcat(txt, ".....");
                        break;
                    }
                    sprintf(strchr(txt, 0), "%d ", p.e[i]);
                    //sprintf ( strchr ( txt, 0), ".", p.e [ i ] );
                }
                strcat(txt, "  ");
                ll = ll > (int)strlen(txt) ? ll : (int)strlen(txt);
                if  (f >= 0)
                {
                    l = (ll - (int)strlen(txt)) / 2;
                    for (i = 0; i < l; i++)
                    {
                        printf(" ");
                    }
                    printf("%s", txt);
                    for (i = 0; i < ll - l - (int)strlen(txt); i++)
                    {
                        printf(" ");
                    }
                    f = 1;
                }
                return ll;
            }
        }
        return 0;
    }
};
//--------------------------------------------------------------------------


#if 0
int btree_test()
{
    try
    {
        for (int c = 0; c < 10000; c++)
        {
            //__utf8_printf("%79s\rc=%d  ", "", c);
            __utf8_printf("\rc=%d  ", c);
            const int N = 1000;
            const int VMax = 2 * N - N + N / 2;
            const int Dups = 1;
            CMaaPtr_<int, 1> x(N);
            //CMaaUnivHash<int, int> h;
            CMaaBTreeAllocator<int> aa(4);
            CMaaBTree<CMaaBTreeAllocator<int>, int> bt(aa);

            int i, j;
            __utf8_printf("gen ");
            for (i = 0; i < N; i++)
            {
                while(1)
                {
                    unsigned r = -1;
                    GetRnd(&r, sizeof(r));
                    //x[i] = rand() % VMax;
                    x[i] = 2 * i;//r % VMax;
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
            __utf8_printf("ins ");
            for (j = 0; j < Dups; j++)
            for (i = 0; i < N; i++)
            {
                int v = -1;
                int r = bt.Search(x[i], v, 1); // v is purged
                if  (r || v != -1)
                {
                    __utf8_printf("i[%d]=%d:%d -err\n", i, x[i], v);
                }
                else// if (v != x[i]) // v is purged
                {
                    //__utf8_printf("i[%d]=%d: !found,inserted %d\n", i, x[i], v);
                    /*
                      v = -1;
                      r = bt.Search(x[i], v, 1);
                      if (r != 1 || v != x[i])
                      {
                         __utf8_printf("i[%d]=%d: reinserted: %d %d\n", i, x[i], r, v);
                      }
                      */
                }
                {
                    int v = -1;
                    int xx = x[i] + 1 - ((N / 3) & ~1);
                    {
                        int r = bt.Search(xx, v, 0); // v is not purged
                        if  (r || v != -1)
                        {
                            printf("sneo: xx=%d, r=%d, v=%d\n", xx, r, v);
                        }
                    }
                    v = -1;
                    int r = bt.Search(xx, v, 1); // v is not purged
                    if  (r || v != -1)
                    {
                        printf("io: xx=%d, r=%d, //v=%d//\n", xx, r, v);
                    }
                    v = -1;
                    r = bt.Search(xx, v, 1); // v is filled
                    if  (!r || v != xx)
                    {
                        printf("ieo: xx=%d, r=%d, //v=%d//\n", xx, r, v);
                    }
                    {
                        int r = bt.Search(xx, v, 0); // v is filled
                        if  (!r || v != xx)
                        {
                            printf("seo: xx=%d, r=%d, v=%d\n", xx, r, v);
                        }
                    }
                }
                /*
                 if (i == 0)
                 {
                      v = -1;
                      r = bt.Search(x[i], v, 1);
                      if (r)
                      {
                           __utf8_printf("i[%d]=%d: found %d\n", i, x[i], v);
                      }
                 }
                 */

                /*if (i % 10 == 0 && i > 0 && false)
                 {
                      int xx = t.Remove(x[i - 10]);
                      x[i - 10] = -1;
                    //__utf8_printf("\ri[%d]=%d:", i, x[i]);
                 }*/
            }

            __utf8_printf("rem_odd ");
            for (j = 0; j < Dups; j++)
            for (i = 0; i < N; i++)
            {
                {
                    //int v = -1;
                    int xx = x[i] + 1 - ((N / 3) & ~1);
                    int r = bt.Remove(xx);
                    if  (r)
                    {
                        __utf8_printf("ro[%d]=%d: err\n", i, xx);
                    }
                }
            }


            __utf8_printf("LQ ");

            for (i = -1; i <= N + 1; i++)
            {
                int k = -10, d = -1, r = -1, ii = i * 2 + 1;
                if  (!(r = bt.Search(ii, k, -1)) || k != ii - 1)
                {
                    if  ((i == -1 || i == 0) && !r)
                    {
                    }
                    else if (i >= N && r && k == 2 *(N - 1))
                    {
                    }
                    else
                    {
                        __utf8_printf("fLQ[%d]:%d %d ", i, r, k);
                    }
                }
            }

            __utf8_printf("GQ ");

            for (i = -2; i <= N; i++)
            {
                int k = -10, d = -1, r = -1, ii = 2 * i + 1;
                if  (!(r = bt.Search(ii, k, 2)) || k != ii + 1)
                {
                    if  ((i == N - 1 || i == N) && !r)
                    {
                    }
                    else if (i == -2 && r && k == 0)
                    {
                    }
                    else
                    {
                        __utf8_printf("fGQ[%d]:%d %d ", i, r, k);
                    }
                }
            }

            __utf8_printf("find ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                int ii = -1;
                if  (!bt.Search(x[i], ii, 0) || ii != x[i])
                {
                    __utf8_printf("\rf[%d]=%d:%d", i, x[i], ii);
                    __utf8_printf("-err\n");
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
            __utf8_printf("find ");
            for (i = 0; i < N; i++)
            {
                if  (x[i] == -1)
                {
                    continue;
                }
                int ii = -1;
                if  (!bt.Search(x[i], ii) || ii != x[i])
                {
                    __utf8_printf("\rf[%d]=%d:%d", i, x[i], ii);
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
                int v = -1;
                //int xx = bt.Delete(x[i]);
                int xx = bt.Remove(x[i], &v);
                if  (xx || v != x[i])
                {
                    __utf8_printf("\rr[%d]=%d:%d", i, x[i], v);
                    __utf8_printf("-err\n");
                }
                else
                {
                    //__utf8_printf("\n");
                }
            }
            __utf8_printf("Empty:\r");
            if  (!bt.Find(x[0]))
            {
                bt.PrintTree();
            }
            //bt.PrintTree();
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

#endif // __TOOLS_BTREE_H

