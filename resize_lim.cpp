
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/resize_lim.cpp

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

#ifdef ___ToolsLibQT___

#include "temp.h"
#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ptrace.h>
#endif

#else

#ifdef _WIN32
#include "ToolsLib\temp.h"
#else
#include "temp.h"
       #include <sys/ipc.h>
       #include <sys/shm.h>
       #include <sys/ptrace.h>
#endif

#endif

CResizeLimit::CResizeLimit(int SemMax, int to, CMaaString LockFileName, int Pid)
{
    m_SemMax = SemMax;
    m_to = to;
    m_LockFileName = LockFileName;
    m_Pid = Pid != -1 ? Pid :
#ifdef __unix__
    (int)getpid()
#else
    (int)GetCurrentProcessId()
#endif
    ;
    ts0 = CMaaTimeStamp();
    Line.Format("%D %u", ts0, m_Pid);
}
CResizeLimit::~CResizeLimit()
{
    try
    {
        CMaaFile f(m_LockFileName, CMaaFile::eRWCD_SrSw, "|overlapped|mode:=666", true);
        if  (!f.LockFile(500))
        {
            return;
        }
        CMaaString l, txt, txt0 = f.ReadAll();
        _qword ts = CMaaTimeStamp();
        int n = 0;
        while(txt0.IsNotEmpty())
        {
            l = txt0.GetLine();
            if  (l != Line)
            {
                _qword q = 0;
                mysscanf64(l, &q, -1, nullptr, " ");
                if  (ts - m_to * 1000000 <= q && q < ts + 1000000)
                {
                    txt += l + CMaaStringLf;
                    n++;
                }
            }
        }
        f.Seek(0);
        f.Write(txt);
        f.Truncate();
    }
    catch(...)
    {
    }
}
bool CResizeLimit::Set()
{
    while((_qword)(CMaaTimeStamp() - ts0) < (_qword)(m_to * 1000000))
    {
        CMaaFile f(m_LockFileName, CMaaFile::eRWCD_SrSw, "|overlapped|mode=666", true);
        if  (!f.LockFile(100))
        {
#ifdef _WIN32
            Sleep(10);
#else
            usleep(10000);
#endif
            continue;
        }
        CMaaString l, txt, txt0 = f.ReadAll();
        _qword ts = CMaaTimeStamp();
        int n = 0;
        while(txt0.IsNotEmpty())
        {
            l = txt0.GetLine();
            _qword q = 0;
            mysscanf64(l, &q, -1, nullptr, " ");
            if  (ts - m_to * 1000000 <= q && q < ts + 1000000)
            {
                txt += l + CMaaStringLf;
                n++;
            }
        }
        txt += Line + CMaaStringLf;
        f.Seek(0);
        f.Write(txt);
        f.Truncate();
        return n < m_SemMax;
    }
    return false;
}
bool CResizeLimit::Wait()
{
    while((_qword)(CMaaTimeStamp() - ts0) < (_qword)(m_to * 1000000))
    {
        CMaaFile f(m_LockFileName, CMaaFile::eRWCD_SrSw, "|overlapped|mode=666", true);
        if  (!f.LockFile(100))
        {
#ifdef _WIN32
            Sleep(10);
#else
            usleep(10000);
#endif
            continue;
        }
        CMaaString l, txt, txt0 = f.ReadAll();
        CMaaString txt2 = txt0;
        _qword ts = CMaaTimeStamp();
        int n = 0, x = -1;
        while(txt0.IsNotEmpty())
        {
            l = txt0.GetLine();
            if  (l == Line)
            {
                x = n;
            }
            _qword q = 0;
            mysscanf64(l, &q, -1, nullptr, " ");
            if  (ts - m_to * 1000000 <= q && q < ts + 1000000)
            {
                txt += l + CMaaStringLf;
                n++;
            }
        }
        if  (txt != txt2)
        {
            f.Seek(0);
            f.Write(txt);
            f.Truncate();
        }
        if  (x < m_SemMax)
        {
            return true;
        }
        if  (x < 0)
        {
            break;
        }
    }
    return false;
}

#if 0
int main(int argn, char * args[])
{
    CResizeLimit lim(5);
    if  (!lim.Set() && !lim.Wait())
    {
        printf("error\n");
    }
    printf("W..."); fflush(stdout);
    usleep(10000000);
    printf("done\n");
    return 0;
}
#endif
