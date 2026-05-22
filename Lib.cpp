
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Lib.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains cross-platform classes for working with
 * files in utf8, folders, finding files.
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
#ifdef _WIN32
#ifndef ___ToolsLibQT___
#include "ToolsLib\temp.h"
#else
#include "temp.h"
#endif
#else
#include "temp.h"
//       #include <sys/ipc.h>
//       #include <sys/shm.h>
//       #include <sys/ptrace.h>
#endif

//-----------------------------------------------------------------------------
void* ToolsLib_No_Allocator()
{
    throw std::bad_alloc();
    return nullptr;
}
//-----------------------------------------------------------------------------
CMaaGeo1::CMaaGeo1(const CMaaString& Path1, const CMaaString& Path2, const CMaaString& Rus)
:   r(1, 2),
    r6(1, 2),
    m_hCache4(s_CacheSize),
    m_hCache6(s_CacheSize),
    m_c(256),
    m_doc("geo", "Geo"),
    m_Rus(Rus)
{
    C4::GetAllocator().PreAllocate(s_CacheSize);
    C6::GetAllocator().PreAllocate(s_CacheSize);
    m_Path = CMaaFile::IsAFile(Path1 + "Geo.xml") || !CMaaFile::IsAFile(Path2 + "Geo.xml") ? Path1 : Path2;
    Load();
}
//-----------------------------------------------------------------------------
CMaaGeo1::~CMaaGeo1()
{
}
//-----------------------------------------------------------------------------
bool CMaaGeo1::Load()
{
    CMaaString err;
    int Line = -1, Column = -1;
    if (m_doc.UpdateFromFile(m_Path + "Geo.xml", 0, false, &err, &Line, &Column) <= 0)
    {
        //__utf8_printf("err %S, l=%d, c=%d\n", &err, Line, Column);
        return false;
    }

    m_MaxId = m_MaxA = m_N4 = m_N6 = 0;
    m_hCode2IdMap.Cleanup();
    m_hCode2AMap.Cleanup();

    //m_DefaultLevel = m_doc.DocumentElement().FindAttribute("DefaultLevel").ToInt(0x101);
    LoadShortcuts();
    LoadCountries();
    LoadIPv4();
    LoadIPv6();
    LoadAliases();
    return true;
}
//-----------------------------------------------------------------------------
bool CMaaGeo1::LoadShortcuts()
{
    CMaaXmlElement ee = m_doc.DocumentElement();
    CMaaXmlNode e = ee.FindNode("Shortcuts");
    CMaaXmlNode n = e.FindNode("Shortcut");
    for (; n; ++n)
    {
        int Id = n.FindAttribute("Id").ToInt(-1);
        if (Id >= 0 && Id <= 255)
        {
            m_c[Id].code = n.FindAttribute("code");
            m_c[Id].Code = n.FindAttribute("Code");
            m_c[Id].Name = n.FindAttribute("Name");
            m_c[Id].Rus = n.FindAttribute(m_Rus);
            m_hCode2IdMap.Add(m_c[Id].n = GetCode(m_c[Id].code), Id);
            m_hCode2IdMap.Add(m_c[Id].N = GetCode(m_c[Id].Code), Id);
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CMaaGeo1::LoadCountries()
{
    CMaaXmlElement ee = m_doc.DocumentElement();
    CMaaXmlNode e = ee.FindNode("Countries");
    CMaaXmlNode n = e.FindNode("Country");
    int Id = 3;
    for (; n && Id <= 255; ++n, Id++)
    {
        m_c[Id].code = n.FindAttribute("code");
        m_c[Id].Code = n.FindAttribute("Code");
        m_c[Id].Name = n.FindAttribute("Name");
        m_c[Id].Rus = n.FindAttribute(m_Rus);
        m_hCode2IdMap.Add(m_c[Id].n = GetCode(m_c[Id].code), Id);
        m_hCode2IdMap.Add(m_c[Id].N = GetCode(m_c[Id].Code), Id);
    }
    m_MaxId = Id;
    return true;
}
//-----------------------------------------------------------------------------
bool CMaaGeo1::LoadIPv4()
{
    m_hCache4.Cleanup();
    m_lCache4.RemoveAll();
    r.RemoveAll();
    CMaaString src = m_doc.DocumentElement().FindNode("Countries").FindAttribute("IPv4");
    if (src[0] == '\\' || src.Find("..") >= 0 || src.Find(':') >= 0)
    {
        src = "%1-ipv4.txt";
    }
    for (int Id = 3; Id < m_MaxId; Id++)
    {
        CMaaString fn;
        fn.Format2("%S%S", src, &m_c[Id].code, &m_c[Id].Code);
        CMaaString txt = CMaaFile(m_Path + fn, CMaaFile::eR_SrSw, false).ReadAll();
        if (txt.IsLeft(CMaaFile::GetUtf8Prefix()))
        {
            txt = txt.RefMid(CMaaFile::GetUtf8Prefix().Length());
        }
        int N = 0;
        while (txt.IsNotEmpty())
        {
            CMaaString Line = txt.GetLine0();
            _IP Ip[2];
            int posx, msk;
            if ((posx = CMaaIpToLongEx(Line, Ip, "/-")) >= 0)
            {
                msk = 32;
                if (Line[posx] == '/')
                {
                    if (dsscanf(posx + 1 + (const char*)Line, "%d", &msk) == 1 && msk >= 0 && msk < 32)
                    {
                        Ip[1] = CMaaGetMaskByNum(msk);
                        Ip[0] &= Ip[1];
                        Ip[1] = Ip[0] | ~Ip[1];
                        if (!r.Add(Ip[0], Ip[1], Id))
                        {
                            N++;
                        }
                    }
                }
                else if (Line[posx] == '-')
                {
                    if (!CMaaIpToLong(posx + 1 + (const char*)Line, Ip + 1))
                    {
                        if (Ip[0] < Ip[1])
                        {
                            if (!r.Add(Ip[0], Ip[1], Id))
                            {
                                N++;
                            }
                            msk = 0;
                        }
                        else if (Ip[0] != Ip[1])
                        {
                            msk = -1;
                        }
                    }
                    else
                    {
                        msk = -1;
                    }
                }
                if (msk == 32)
                {
                    if (!r.Add(Ip[0], Ip[0], Id))
                    {
                        N++;
                    }
                }
            }
        }
        //__utf8_printf("%d\n", N);
        m_N4 += N;
    }
    //__utf8_printf("m_N4 = %d\n", m_N4);
    m_Coll4 = 0;
    const CMaaRBTree<_IP, CMaaColorRanges<_IP, unsigned char>::Data>& t = r.Tree();
    for (CMaaRBTree<_IP, CMaaColorRanges<_IP, unsigned char>::Data>::Handle h = t.begin(); h != t.end(); h = t.TreeSuccessor(h))
    {
        //RR++;
        if (h->data().c == 2)
        {
            m_Coll4++;
            //__utf8_printf("[%I,%I)%d ", h->key(), h->data().d, h->data().c);
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CMaaGeo1::LoadIPv6()
{
    m_hCache6.Cleanup();
    m_lCache6.RemoveAll();
    r6.RemoveAll();
    CMaaString src = m_doc.DocumentElement().FindNode("Countries").FindAttribute("IPv6");
    if (src[0] == '\\' || src.Find("..") >= 0 || src.Find(':') >= 0)
    {
        src = "%1-ipv6.txt";
    }
    for (int Id = 3; Id < m_MaxId; Id++)
    {
        CMaaString fn;
        fn.Format2("%S%S", src, &m_c[Id].code, &m_c[Id].Code);
        CMaaString txt = CMaaFile(m_Path + fn, CMaaFile::eR_SrSw, false).ReadAll();
        if (txt.IsLeft(CMaaFile::GetUtf8Prefix()))
        {
            txt = txt.RefMid(CMaaFile::GetUtf8Prefix().Length());
        }
        int N = 0;
        while (txt.IsNotEmpty())
        {
            CMaaString Line = txt.GetLine0();
            _IP6 Ip[2] = { eNotInit, eNotInit };
            int posx, msk;
            if ((posx = CMaaIpToLongEx(Line, Ip[0].ptr(), "/-")) >= 0)
            {
                msk = 128;
                if (Line[posx] == '/')
                {
                    if (dsscanf(posx + 1 + (const char*)Line, "%d", &msk) == 1 && msk >= 0 && msk < 128)
                    {
                        Ip[1].GetMask(msk);
                        Ip[0] &= Ip[1];
                        Ip[1].Invert();
                        Ip[1] |= Ip[0];
                        if (!r6.Add(Ip[0], Ip[1], Id))
                        {
                            N++;
                        }
                    }
                }
                else if (Line[posx] == '-')
                {
                    if (!CMaaIpToLong6(posx + 1 + (const char*)Line, Ip[1].ptr()))
                    {
                        if (Ip[0] < Ip[1])
                        {
                            if (!r6.Add(Ip[0], Ip[1], Id))
                            {
                                N++;
                            }
                            msk = 0;
                        }
                        else if (Ip[0] != Ip[1])
                        {
                            msk = -1;
                        }
                    }
                    else
                    {
                        msk = -1;
                    }
                }
                if (msk == 128)
                {
                    if (!r6.Add(Ip[0], Ip[0], Id))
                    {
                        N++;
                    }
                }
            }
        }
        //__utf8_printf("%d\n", N);
        m_N6 += N;
    }
    //__utf8_printf("m_N6 = %d\n", m_N4);
    m_Coll6 = 0;
    const CMaaRBTree<_IP6, CMaaColorRanges<_IP6, unsigned char>::Data>& t = r6.Tree();
    for (CMaaRBTree<_IP6, CMaaColorRanges<_IP6, unsigned char>::Data>::Handle h = t.begin(); h != t.end(); h = t.TreeSuccessor(h))
    {
        //RR++;
        if (h->data().c == 2)
        {
            m_Coll6++;
            //__utf8_printf("[%J,%J)%d ", h->key().const_ptr(), h->data().d.const_ptr(), h->data().c);
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool CMaaGeo1::LoadAliases()
{
    CMaaXmlElement ee = m_doc.DocumentElement();
    CMaaXmlNode e = ee.FindNode("Aliases");
    CMaaXmlNode n = e.FindNode("Alias");
    int x = 0;
    for (; n; ++n, x++)
    {
        m_a[x].code = n.FindAttribute("code");
        m_a[x].Code = n.FindAttribute("Code");
        m_a[x].Name = n.FindAttribute("Name");
        m_a[x].Rus = n.FindAttribute(m_Rus);
        CMaaString txt = m_a[x].Countries = n.FindAttribute("Countries");
        m_a[x].Map.Clear();
        while (txt.IsNotEmpty())
        {
            const int Id = m_hCode2IdMap[GetCode(txt.GetWord(' ')), -1];
            if (Id >= 0)
            {
                m_a[x].Map.Set((unsigned char)Id);
            }
        }
        m_hCode2AMap.Add(m_a[x].n = GetCode(m_a[x].code), x);
        m_hCode2AMap.Add(m_a[x].N = GetCode(m_a[x].Code), x);
    }
    m_MaxA = x;
    return true;
}
//-----------------------------------------------------------------------------
void CMaaGeo1::GetMask(CMaaString Countries, CMaa256Bits& Map) const noexcept
{
    Map.Clear();
    while (Countries.IsNotEmpty())
    {
        const int code = GetCode(Countries.GetWord(' '));
        const int Id = m_hCode2IdMap[code, -1];
        if (Id >= 0)
        {
            Map.Set((unsigned char)Id);
        }
        else
        {
            const int a = m_hCode2AMap[code, -1];
            if (a >= 0)
            {
                Map |= m_a[a].Map;
            }
        }
    }
}
//-----------------------------------------------------------------------------
unsigned char CMaaGeo1::GetId(const CMaaString &Code) const noexcept
{
    return m_hCode2IdMap[GetCode(Code), 0];
}
//-----------------------------------------------------------------------------
unsigned char CMaaGeo1::GetId(_IP Ip) //const noexcept
{
    //return r.Find(Ip);
    C4* p = m_hCache4[Ip, nullptr];
    if (p)
    {
        m_lCache4.Release(p);
        m_lCache4.AddAtFront(p);
        return p->c;
    }
    if (m_hCache4.GetItemCount() >= s_CacheSize)
    {
        p = m_lCache4.GetFromBack();
        m_hCache4.Remove(p->Ip);
    }
    else
    {
        p = TL_NEW C4;
        if (!p)
        {
            return r.Find(Ip);
        }
    }
    p->Ip = Ip;
    p->c = r.Find(Ip);
    m_hCache4.Add(Ip, p);
    m_lCache4.AddAtFront(p);
    return p->c;
}
//-----------------------------------------------------------------------------
unsigned char CMaaGeo1::GetId(const _IP6& Ip) //const noexcept
{
    return r6.Find(Ip);
    C6* p = m_hCache6[Ip, nullptr];
    if (p)
    {
        m_lCache6.Release(p);
        m_lCache6.AddAtFront(p);
        return p->c;
    }
    if (m_hCache6.GetItemCount() >= s_CacheSize)
    {
        p = m_lCache6.GetFromBack();
        m_hCache6.Remove(p->Ip);
    }
    else
    {
        p = TL_NEW C6;
        if (!p)
        {
            return r6.Find(Ip);
        }
    }
    p->Ip = Ip;
    p->c = r6.Find(Ip);
    m_hCache6.Add(Ip, p);
    m_lCache6.AddAtFront(p);
    return p->c;
}
//-----------------------------------------------------------------------------
CMaaString CMaaGeo1::GetEx(_IP Ip, int level) //const noexcept
{
    level = level < 0 ? m_DefaultLevel : level;
    return level > 0 ? Get(GetId(Ip), level) : CMaaStringZ;
}
CMaaString CMaaGeo1::GetEx(const _IP6 &Ip, int level) //const noexcept
{
    level = level < 0 ? m_DefaultLevel : level;
    return level > 0 ? Get(GetId(Ip), level) : CMaaStringZ;
}
CMaaString CMaaGeo1::Get(int Id, int level) const noexcept
{
    level = level < 0 ? m_DefaultLevel : level;
    if (Id < 0 || Id >= m_MaxId || level <= 0)
    {
        return CMaaStringZ;
    }
    const C& c = m_c[Id];
    CMaaString Ret, tmp;
    if ((level & 0xff))
    {
        Ret = c.Code;
    }
    for (int i = 0; i < m_MaxA; i++)
    {
        if (m_a[i].Map.Test((unsigned char)Id))
        {
            if ((level & 0xff) > 1)
            {
                Ret.Format("%S, %S", &Ret, &m_a[i].Code);
            }
            if ((level >> 8) > 1)
            {
                tmp.Format("%S, %S", &tmp, m_Rus.IsNotEmpty() && m_a[i].Rus.IsNotEmpty() ? &m_a[i].Rus : &m_a[i].Name);
            }
        }
    }
    if ((level >> 8))
    {
        if (m_Rus.IsNotEmpty() && c.Rus.IsNotEmpty())
        {
            Ret.Format("%S%s%S (%S)%S", &Ret, Ret.IsNotEmpty() ? " " : "", &c.Rus, &c.Name, &tmp);
        }
        else
        {
            Ret.Format("%S%s%S%S", &Ret, Ret.IsNotEmpty() ? " " : "", &c.Name, &tmp);
        }
    }
    return Ret;
}
//-----------------------------------------------------------------------------
CMaaString CMaaGeo1::GetStrStat() const noexcept
{
    return CMaaString::sFormat("MaxId = %d, MaxA = %d, Countries = %d\n"
        "IPv4: %d/%d, %d collisions\n"
        "IPv6: %d/%d, %d collisions\n",
        m_MaxId, m_MaxA, m_MaxId - 3,
        (int)r.GetCount(), m_N4, m_Coll4,
        (int)r6.GetCount(), m_N6, m_Coll6
    );
}
//-----------------------------------------------------------------------------
void CMaaGeo1::PrintStat() const noexcept
{
    CMaaString s = GetStrStat();
    printf("%s", (const char *)s);
}
//-----------------------------------------------------------------------------
DEF_UL_ALLOCATOR(CMaaGeo1::C4)
DEF_UL_ALLOCATOR(CMaaGeo1::C6)
//-----------------------------------------------------------------------------
//DEF_ALLOCATOR_CMaaRBTree(_IP, CMaaColorRanges<_IP, unsigned char>::Data)
//DEF_ALLOCATOR_CMaaRBTree(_IP6, CMaaColorRanges<_IP6, unsigned char>::Data)
//-----------------------------------------------------------------------------
