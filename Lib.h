
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/Lib.h

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

class CMaaGeo1
{
    CMaaColorRanges<_IP, unsigned char> r;
    CMaaColorRanges<_IP6, unsigned char> r6;
public:
    struct C
    {
        int n, N;
        CMaaString code, Code, Name, Rus;
    };
    struct A : public C
    {
        CMaaString Countries;
        CMaa256Bits Map;
    };
    struct C4 : public CMaaDLink
    {
        _IP Ip;
        unsigned char c;
        ADD_UL_ALLOCATOR(C4)
    };
    struct C6 : public CMaaDLink
    {
        _IP6 Ip;
        unsigned char c;
        C6() noexcept
        :   Ip(eNotInit)
        {
        }
        ADD_UL_ALLOCATOR(C6)
    };
private:
    static constexpr int s_CacheSize = 1023;
    CMaaUnivHash<_IP, C4*> m_hCache4;
    CMaaUnivHash<_IP6, C6*> m_hCache6;
    CMaaDListAD<C4> m_lCache4;
    CMaaDListAD<C6> m_lCache6;

    int m_MaxId = 0, m_MaxA = 0, m_N4 = 0, m_N6 = 0, m_Coll4 = 0, m_Coll6 = 0;
    CMaaPtr_<C, 1> m_c;
    CMaaPtrAE_<A, 1> m_a;
    CMaaUnivHash<int, int> m_hCode2IdMap, m_hCode2AMap;
    CMaaString m_Path, m_Rus;
    CMaaXmlDocument m_doc;
    bool LoadShortcuts();
    bool LoadCountries();
    bool LoadIPv4();
    bool LoadIPv6();
    bool LoadAliases();
public:
    int m_DefaultLevel = 0x101;
    static constexpr int s_PresetLevelsCount = 9;
    static constexpr int s_PresetLevels[s_PresetLevelsCount] = { 0, 1, 2, 0x100, 0x101, 0x102, 0x200, 0x201, 0x202 };
    CMaaGeo1(const CMaaString& Path1 = ".\\", const CMaaString& Path2 = ".\\", const CMaaString& Rus = gsCMaaStringZ);
    ~CMaaGeo1();
    bool Load();
    void GetMask(CMaaString txt, CMaa256Bits& Map) const noexcept;
    int MaxC() const noexcept { return m_MaxId; }
    int MaxA() const noexcept { return m_MaxA; }
    const CMaaPtr_<C, 1>& GetC() const noexcept { return m_c; }
    const CMaaPtrAE_<A, 1>& GetA() const noexcept { return m_a; }
    const CMaaString& GetRus() const noexcept { return m_Rus; }
    const CMaaColorRanges<_IP, unsigned char>& GetR() const noexcept { return r; }
    const CMaaColorRanges<_IP6, unsigned char>& GetR6() const noexcept { return r6; }

    static int GetCode(const CMaaString& c2) noexcept
    {
        return c2.Length() == 2 ? *(unsigned short*)(const char*)c2 : -1;
    }
    unsigned char GetId(const CMaaString& Code) const noexcept;
    unsigned char GetId(_IP Ip);// const noexcept;
    unsigned char GetId(const _IP6& Ip);// const noexcept;
    CMaaString Get(int Id, int level = -1) const noexcept; // level = { 0, 1, 2, 0x101, 0x102, 0x201, 0x202 }
    CMaaString GetEx(_IP Ip, int level = -1);// const noexcept;
    CMaaString GetEx(const _IP6& Ip, int level = -1);// const noexcept;
    CMaaString GetStrStat() const noexcept;
    void PrintStat() const noexcept;
};
