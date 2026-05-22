/*
ToolsLib CMaaUnivHash is 3.5x - 15x faster than unordered_map

git clone git://git.maasoftware.ru/ToolsLib.git

output (Visual C++ 2010, 32 bits, Windows XP, Core2Duo E7400, 2800 MHz):

unordered_map<int, int> :
 359,772 hmap.insert()
 282,048 hmap.erase(iter)
 357,760 hmap.insert()
 307,778 hmap.erase(val)

CMaaUnivHash<int, int> :
  94,252 h.Add()
  38,232 it.Remove()
  45,917 h.Add()
  20,542 h.Remove()

$ g++ -o a2.out -std=c++11 -lstdc++ -lpthread main2.cpp ../ToolsLib/ToolsLib.a
output (on Linux Debian 7.6 x64 VDS, Intel Xeon E5, 2000 MHz):

unordered_map<int, int> :
 625,677 hmap.insert()
 285,488 hmap.erase(iter)
 595,525 hmap.insert()
 252,544 hmap.erase(val)

CMaaUnivHash<int, int> :
 277,266 h.Add()
 244,400 it.Remove()
 129,552 h.Add()
  74,735 h.Remove()

*/

#ifdef __unix__
#include "../ToolsLib/ToolsLib.h"
#else
#include "..\ToolsLib\ToolsLib.h"
#endif

#include <iostream>
//#include <algorithm>
//#include <hash_map>
#include <unordered_map>

int main_TestToolsLib1(int nn = 1000000)
{
    typedef std::pair <int, int> ipair;
    int i;

    CHrtMultiTimer t0, t1;
    CMaaString s0, s1;
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
        for (ihmap = hmap.begin(); ihmap != hmap.end(); ihmap = ihmap2)
        {
            ihmap2 = ihmap;
            ++ihmap2;
            hmap.erase(ihmap);
        }
        t0.Stop("hmap.erase(iter)");

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
        CMaaUnivHash<int, int>::iterator it(h);
        for (; it; ++it)
        {
            it.Remove();
        }
        t1.Stop("it.Remove()");

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
