/*
ToolsLib CMaaUnivHash is 3.5x - 15x faster than unordered_map

git clone git://git.maasoftware.ru/ToolsLib.git

output (Visual C++ 2010, 32 bits, Windows XP, Core2Duo E7400, 2800 MHz):

q = 499999500000
q = 0
unordered_map<int, int> :
 314,784 hmap.insert()
  72,753 hmap.find(existed)
 255,102 hmap.erase(iter)
  63,864 hmap.find(not existed)
 294,268 hmap.insert()
 296,055 hmap.erase(val)

q = 499999500000
q = 0
CMaaUnivHash<int, int> :
  91,024 h.Add()
   8,833 h.Find(existed)
  52,243 it.Remove()
   7,840 h.Find(not existed)
  44,055 h.Add()
  18,900 h.Remove()

$ g++ -o a3.out -std=c++11 -lstdc++ -lpthread main3.cpp ../ToolsLib/ToolsLib.a
output (on Linux Debian 7.6 x64 VDS, Intel Xeon E5, 2000 MHz):

q = 499999500000
q = 0
unordered_map<int, int> :
 577,276 hmap.insert()
 170,402 hmap.find(existed)
 291,785 hmap.erase(iter)
 115,816 hmap.find(not existed)
 590,268 hmap.insert()
 213,276 hmap.erase(val)

q = 499999500000
q = 0
CMaaUnivHash<int, int> :
 254,875 h.Add()
  36,704 h.Find(existed)
 207,689 it.Remove()
  30,051 h.Find(not existed)
 117,712 h.Add()
  89,317 h.Remove()

*/

//#include "stdafx.h"

#ifdef __unix__
#include "../ToolsLib/ToolsLib.h"
#else
#include "..\ToolsLib\ToolsLib.h"
#endif

#include <iostream>
//#include <algorithm>
//#include <hash_map>
#include <unordered_map>

int main_TestToolsLib2(int nn = 1000000)
{
    typedef std::pair <int, int> ipair;
    int i;

    CHrtMultiTimer t0, t1;
    CMaaString s0, s1;
    _qword q;
    int n;
    {
        t0.Start(); t0.Stop("begin");
        t0.Start();
        std::unordered_map<int, int> hmap;
        std::unordered_map<int, int>::iterator ihmap, ihmap2;
        t0.Stop("hmap::hmap()");

        t0.Start();
        for (i = 0; i < nn; i++)
        {
            hmap.insert(ipair(i, i));
        }
        t0.Stop("hmap.insert()");

        t0.Start();
        q = 0;
        n = 0;
        for (i = 0; i < nn; i++)
        {
            ihmap = hmap.find(i);
            if  (ihmap != hmap.end())
            {
                q += ihmap->first;
                n++;
            }
        }
        t0.Stop("hmap.find(existed)");
        __utf8_printf("q = %D\n", q);

        t0.Start();
        for (ihmap = hmap.begin(); ihmap != hmap.end(); ihmap = ihmap2)
        {
            ihmap2 = ihmap;
            ++ihmap2;
            hmap.erase(ihmap);
        }
        t0.Stop("hmap.erase(iter)");

        t0.Start();
        q = 0;
        n = 0;
        for (i = 0; i < nn; i++)
        {
            ihmap = hmap.find(i+nn);
            if  (ihmap != hmap.end())
            {
                q += ihmap->first;
                n++;
            }
        }
        t0.Stop("hmap.find(not existed)");
        __utf8_printf("q = %D\n", q);

        t0.Start();
        for (i = 0; i < nn; i++)
        {
            hmap.insert(ipair(i, i));
        }
        t0.Stop("hmap.insert()");

        t0.Start();
        for (i = 0; i < nn; i++)
        {
            hmap.erase(i);
        }
        t0.Stop("hmap.erase(val)");
        for (ihmap = hmap.begin(); ihmap != hmap.end(); ++ihmap)
        {
            std::cout << "hmap[" << ihmap->first << "]=" << ihmap->second << " ";
        }
        std::cout << '\n';
        t0.Start();
    }
    t0.Stop("hmap::~hmap()");
    s0 = t0.GetResult("unordered_map<int, int> :\n");
    __utf8_printf("%S\n", &s0);
    //nn=-nn;
    {
        t1.Start(); t1.Stop("begin");
        t1.Start();
        CMaaUnivHash<int, int> h;
        t1.Stop("h::h()");

        t1.Start();
        for (i = 0; i < nn; i++)
        {
            h.Add(i, i);
        }
        t1.Stop("h.Add()");

        t1.Start();
        q = 0;
        n = 0;
        for (i = 0; i < nn; i++)
        {
            int x = 0;
            if  (!h.Find(i, &x))
            {
                q += x;
                n++;
            }
        }
        t1.Stop("h.Find(existed)");
        __utf8_printf("q = %D\n", q);

        t1.Start();
        CMaaUnivHash<int, int>::iterator it(h);
        for (; it; ++it)
        {
            it.Remove();
        }
        t1.Stop("it.Remove()");

        t1.Start();
        q = 0;
        n = 0;
        for (i = 0; i < nn; i++)
        {
            int x = 0;
            if  (!h.Find(i + nn, &x))
            {
                q += x;
                n++;
            }
        }
        t1.Stop("h.Find(not existed)");
        __utf8_printf("q = %D\n", q);

        t1.Start();
        for (i = 0; i < nn; i++)
        {
            h.Add(i, i);
        }
        t1.Stop("h.Add()");

        t1.Start();
        for (i = 0; i < nn; i++)
        {
            h.Remove(i);
        }
        t1.Stop("h.Remove()");
        {
            CMaaUnivHash<int, int>::iterator it(h);
            for (; it; ++it)
            {
                __utf8_printf("h[%d]=%d ", it.key(), it.data());
            }
            __utf8_printf("\n");
        }
        t1.Start();
    }
    t1.Stop("h::~h()");
    s1 = t1.GetResult("CMaaUnivHash<int, int> :\n");
    __utf8_printf("%S\n", &s1);

    CHrtMultiTimer t_e;
    CMaaString s_e;
    t_e.Start(); t_e.Stop("end");
    s_e = t_e.GetResult("finish : ");
    __utf8_printf("%S\n", &s_e);
    return 0;
}
