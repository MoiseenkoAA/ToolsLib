
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/a_trie.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains strings and string's functions implementation.
 * This CMaaString does not throws throws exception out of boundaries.
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

#include "perm.h"
#include "temp.h"


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// Цифровое TRIE дерево v.13
// Author: Andrey Moiseenko (Андрей Моисеенко)
// Copyright (C) 2017 Andrey Moiseenko
// Licensed under GNU GPL v3
// www: http://maasoftware.ru
// e-mail: support@maasoftware.ru
// Can be commented: #define TRIE10_DEBUG
// Tested with compilers:
// g++ version 6.3.0
// Visual Studio 2017

//#define TRIE10_DEBUG
//#define TRIE10_DEBUG2

#ifdef TRIE10_DEBUG
extern bool gbDbgTrie10Nodes; // debug nodes
#endif

// Тестовая печать дерева, вызывается без параметров
template <> void CMaaTrie<int>::Print(char* str, Node* x, int pos) const
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
        printf("%p %-15s v=%4d n=%4d  ", x, str, *(int*)x->m_Value, x->m_N);
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
    char* str2 = str + pos;
    if (str2 - str2 < 10 * 1024)
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
template <> int CMaaTrie<int>::PrintWords(char* str, Node* x, int pos) const
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
        __utf8_printf("%05d %-15s\n", *(int*)x->m_Value, str);
        //CMaaString s2 = UnicodeToUtf8(AnsiToUnicode(str));
        //__utf8_printf("%05d %-15S\n", *(int*)x->m_Value, &s2);
    }
    char* str2 = str + pos;
    if (str2 - str < 10 * 1024)
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

// CMaaTrie<int> test function
int main_test_CMaaTrie(int argn, const char* args[])
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
        const char* s[] = { "альфа", "бета", "гамма", "дельта", "эпсилон", "одна", "однаждыы", nullptr };
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
            a.MaxPath(s, (warning_int)s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char*)s, len, ilen, (const char*)p, (const char*)ip, v);
        }
        {
            CMaaString s = "однаждыыхх";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, (warning_int)s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char*)s, len, ilen, (const char*)p, (const char*)ip, v);
            //return 0;
        }
        if (0)
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


// Тестовая печать дерева, вызывается без параметров
template <> void CMaaTrie2<int, '\0', 256>::Print(char * str, Node * x, int pos) const
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
    char * str2 = str + pos;
    if  (str2 - str2 < 10 * 1024)
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

// CMaaTrie2<int> test function
int main_test_CMaaTrie2(int argn, const char * args[])
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
        const char * s[] = {"альфа", "бета", "гамма", "дельта", "эпсилон", "одна", "однаждыы", nullptr};
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
            a.MaxPath(s, (warning_int)s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char *)s, len, ilen, (const char *)p, (const char *)ip, v);
        }
        {
            CMaaString s = "однаждыыхх";
            int v = -1, len = -1, ilen = -1;
            a.MaxPath(s, (warning_int)s.Length(), &len, &ilen, &v);
            CMaaString p = s.Left(len);
            CMaaString ip = s.Left(ilen);
            printf("%s: %d %d ('%s', '%s') %d\n", (const char *)s, len, ilen, (const char *)p, (const char *)ip, v);
            //return 0;
        }
#if 1
        if  (1)
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

#if 0

struct xyz
{
    int v;
    xyz()
    {
        v = -1; __utf8_printf("() ");
    }
    xyz(const int vv)
    {
        v = vv; __utf8_printf("(int %d) ", vv);
    }
    xyz(const xyz &vv)
    {
        v = vv.v; __utf8_printf("(xyz %d) ", vv.v);
    }
    ~xyz()
    {
        __utf8_printf("~(%d) ", v);
    }
    operator int () const
    {
        __utf8_printf("int(%d) ", v);
        return v;
    }
    const xyz & operator =(int vv)
    {
        __utf8_printf("%d=%d ", v, vv);
        v = vv;
        return *this;
    }
    const xyz & operator =(const xyz &vv)
    {
        __utf8_printf("%d=xyz|%d ", v, vv.v);
        v = vv.v;
        return *this;
    }
};

int main_trie0_test(int argn, char * args[])
{
    if  (argn <= 1)
    {
        CMaaTrie<xyz> a;
        CMaaString aa("aa");
        int vvv = a.FindS(aa);
        __utf8_printf("!!! f.%d\n", vvv);
        {
            xyz _(10);
            a.AddS(aa, _);
        }
        vvv = a.FindS(aa);
        __utf8_printf("f.%d\n", vvv);
        {
            xyz _ = 20;
            a.AddS(aa, _);
        }
        vvv = a.FindS(aa);
        __utf8_printf("f.%d\n", vvv);

        a.AddS(aa, 30);
        vvv = a.FindS(aa);
        __utf8_printf("f.%d\n", vvv);
        return 0;//main_test_CMaaTrie(argn, args);
    }
}

bool gbDbgTrie10Nodes = false;
int main(int argn, char * args[])
{
    return main_test_CMaaTrie(argn, args);
}
#endif

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
        /*
        CMaaString sp(nullptr, 256);
        for (int ch = 0; ch < 256; ch++)
        {
            sp[ch] = RussianAlphabet.Find((char)ch) < 0 ? (char)ch : ' ';
        }
        */
        CMaa256Bits sp(RussianAlphabet, (int)RussianAlphabet.Length());
        /*
        for (int ch = 0; ch < 256; ch++)
        {
            sp.Set((char)ch);
        }
        */
        sp.Invert();
        //sp += " \t\r\n";
        CMaaString ut = "игр";
        //__utf8_printf("%S ", &ut);
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
            CMaaString w = txt.GetWord(sp).ToLower(1251);
            CMaaString u = UnicodeToUtf8(AnsiToUnicode(w));
            int v = 0;
            a.Find(w, (warning_int)w.Length(), &v);
            v++;
            if (v <= 0)
            {
                __utf8_printf("%-15S == %d\n", &u, v);
            }
            a.Add(w, (warning_int)w.Length(), v);
            int v2 = a.Find(w, (warning_int)w.Length());
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
            v = a.Find(t, (warning_int)t.Length());
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

        CMaaPtrAE_<CMaaTrie<int>::Handle, 1> m(N);
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

/*
000002282CD53E50 24468 99526 >
000002282ECCE720 24468 99526 <
CMaaTrie<int>:
  0.199247 us      12,890,112  Add/Find
  0.001942 us      12,894,208  Next
  0.001626 us      12,894,208  Prev
*/

/*
000002C837FA08B0 24468 99526 >
000002C83D227080 24468 99526 <
CMaaTrie2<int>:
  0.220467 us     135,675,904  Add/Find
  0.018996 us     135,680,000  Next
  0.015777 us     135,680,000  Prev
*/
