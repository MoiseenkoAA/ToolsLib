
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/curl_wrp.cpp

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

#ifndef NO_CURL_WRP

//#include "https.c"

// Copyright (C) 2020 Moiseenko A.A., based on C code
// of 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Simple HTTPS GET
 * </DESC>
 */
//#include <stdio.h>
#include <curl/curl.h>

//-----------------------------------------
CMaaCURLWrapperInit gMaaCURLWrapperInit;
//-----------------------------------------
CMaaCURLWrapperInit::CMaaCURLWrapperInit()
{
    ptr = TL_NEW CMaaReference;
}
CMaaCURLWrapperInit::~CMaaCURLWrapperInit()
{
    long r;
    if  ((r = ptr->Release()) == 1)
    {
        r = ptr->Release();
        //__utf8_printf("curl_global_cleanup()\n");
        curl_global_cleanup();
    }
    //__utf8_printf("CMaaCURLWrapperInit::~CMaaCURLWrapperInit(): %d\n", (int)r);
}
void CMaaCURLWrapperInit::AddRef()
{
    long r;
    if  ((r = ptr->AddRef()) == 2)
    {
        int x = curl_global_init(CURL_GLOBAL_DEFAULT);
        //__utf8_printf("curl_global_init(): %d\n", x);
        // curl_global_init(CURL_GLOBAL_ALL); // In windows, this will init the winsock stuff
        r = ptr->AddRef();
    }
    //__utf8_printf("CMaaCURLWrapperInit::AddRef(): %d\n", (int)r);
}
void CMaaCURLWrapperInit::Release()
{
    ptr->Release();
}
//-----------------------------------------
/*
struct str_for_fnMaacURLWriter
{
    CMaaConcatString *s;
    int maxlen;
    bool bOk;
};
*/
int fnMaacURLWriter(char *data, size_t size, size_t nmemb, str_for_fnMaacURLWriter *wt)
{
    if  (!wt || wt->bOk == false)
    {
        return 0;
    }
    if  ((size_t)wt->s.Length() + size*nmemb > (size_t)wt->maxlen)
    {
        nmemb = (size_t)(wt->maxlen - wt->s.Length()) / size;
        wt->bOk = false;
    }
    (wt->s).Add((const char *)data, (int)size*nmemb);
    return wt->s.IsValid() ? size * nmemb : 0;
}

int fnMaacURLWriter2(char *data, size_t size, size_t nmemb, str_for_fnMaacURLWriter *wt)
{
    if  (!wt || wt->bOk == false)
    {
        return 0;
    }
    if  ((size_t)wt->s.Length() > (size_t)wt->maxlen)
    {
        nmemb = 0;
        wt->bOk = false;
    }
    else if  ((size_t)wt->s.Length() + (size_t)wt->FilePos + size*nmemb > (size_t)wt->maxlen + (size_t)wt->MaxFileLen)
    {
        _qword q = (_qword)((size_t)wt->maxlen + (size_t)wt->MaxFileLen - (size_t)wt->s.Length());
        nmemb = q >= 0 ? ((size_t)q) / size : 0;
        wt->bOk = false;
    }
    size_t nmemb0 = nmemb;
    nmemb = 0;
    while(nmemb0 > 0 && wt->bOk)
    {
        size_t nmemb1 = size * nmemb0 <= 0x7fffffff ? nmemb0 : 0x7fffffff / size;
        if ((wt->s).GetMemoryString().IsRight("\r\n\r\n", 4))
        {
            //__utf8_printf("%,D + %,D = %,D\n", (_qword)(size * nmemb1), (_qword)wt->FilePos, (_qword)(size * nmemb1 + wt->FilePos));
            if (size * nmemb1 + (size_t)wt->FilePos > (size_t)wt->MaxFileLen)
            {
                wt->bOk = false;
                break;
            }
            try
            {
                _dword x = wt->f->Write((const char *)data, (int)size*nmemb1);
                wt->FilePos += x;
                if (x != size*nmemb1)
                {
                    wt->bOk = false;
                }
                else
                {
                    nmemb += nmemb1;
                    nmemb0 -= nmemb1;
                }
            }
            catch(...)
            {
                wt->bOk = false;
            }
        }
        else
        {
            nmemb1 = (size*nmemb1 > (size_t)(0x7fffffff - wt->s.Length())) ? (0x7fffffff - wt->s.Length()) / size : nmemb1;
            if (!nmemb1)
            {
                wt->bOk = false;
                break;
            }
            (wt->s).Add((const char *)data, (int)size*nmemb1);
            if (!wt->s.IsValid())
            {
                wt->bOk = false;
                break;
            }
            nmemb += nmemb1;
            nmemb0 -= nmemb1;
            CMaaString s = (wt->s).GetMemoryString();
            int pos = s.Find("\r\n\r\n");
            if (pos + 4 > wt->maxlen)
            {
                wt->bOk = false;
            }
            else if (pos >= 0)
            {
                pos += 4;
                try
                {
                    _dword x = wt->f->Write(s.RefMid(pos));
                    if (x != (wt->FilePos = (_dword)(s.Length() - pos)))
                    {
                        wt->bOk = false;
                    }
                }
                catch(...)
                {
                    wt->bOk = false;
                }
                (wt->s).SetNewLengthValue(pos);

                CMaaString tmp = wt->s;
                wt->ContentLength = tmp.CgiGetValue("Content-Length:").ToQword(-1); // or tmp.HttpGetValue()?
            }
        }
    }
    if (wt->bOk && wt->fnProgress)
    {
        if ((wt->fnProgress)(wt))
        {
            wt->bOk = false;
        }
    }
    return wt->s.IsValid() && wt->bOk ? size * nmemb : 0;
}

size_t fnMaacURLReader(void *ptr, size_t size, size_t nmemb, CMaaString *rt)
{
    if  (rt == nullptr)
    {
        return 0;
    }
    size_t len = size * nmemb;
    if  (len > (size_t)(*rt).Length())
    {
        len = (*rt).Length() / size;
        len *= size;
    }
    memcpy(ptr, (const char *)(*rt), len);
    *rt = (*rt).RefMid(len);
    return len;
}

CMaaCURLWrapper::CMaaCURLWrapper(long TimeOutMs, int MaxLen, int Flags, _qword MaxFileLen)
:   curl(nullptr),
    res(CURLE_FAILED_INIT),
    m_Flags(Flags),
    m_TimeOutMs(TimeOutMs)
{
    m_wt.maxlen = MaxLen > 0 ? MaxLen : 0x7fffffff;
    m_wt.MaxFileLen = MaxFileLen >= 0 ? MaxFileLen : 128 * (_qword)(1024 * 1024 * 1024);
    m_wt.FilePos = 0;
    m_wt.f = nullptr;
    SetProgress(nullptr, nullptr);
    gMaaCURLWrapperInit.AddRef();
    curl = curl_easy_init();
}

CMaaCURLWrapper::~CMaaCURLWrapper()
{
    // always cleanup
    curl_easy_cleanup(curl);
    gMaaCURLWrapperInit.Release();
}

bool CMaaCURLWrapper::SetUrl(CMaaString url)
{
    m_wt.ContentLength = -1;
    //__utf8_printf("url: >>%S<<\n", &url);
    res = curl_easy_setopt(curl, CURLOPT_URL, (const char *)url);
    if  (res != CURLE_OK)
    {
        CMaaString err;
        err.Format("set url in curl_easy_setopt failed: %s", curl_easy_strerror(res));
        throw err;
    }
    if  (!(m_Flags & 1) && url.IsLeftCi("https://", 8, 0))
    {
        // SKIP_PEER_VERIFICATION
        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    }

    if  (!(m_Flags & 2) && url.IsLeftCi("https://", 8, 0))
    {
        // SKIP_HOSTNAME_VERIFICATION
        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    if  (m_TimeOutMs > 0)
    {
        res = curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, m_TimeOutMs);
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("set CURLOPT_TIMEOUT_MS failed: %s", curl_easy_strerror(res));
            throw err;
        }
    }
    return true;
}

CMaaString CMaaCURLWrapper::Get(CMaaString url, bool * pOk)
{
    m_wt.FilePos = 0;
    m_wt.f = nullptr;
    m_wt.bOk = true;
    //m_wt.maxlen = m_MaxLen;
    m_wt.s.Empty();
    bool b;
    pOk = pOk ? pOk : &b;
    *pOk = false;
    if  (curl)
    {
        SetUrl(url);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnMaacURLWriter);
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("set CURLOPT_WRITEFUNCTION failed: %s", curl_easy_strerror(res));
            throw err;
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_wt);
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("set CURLOPT_WRITEDATA failed: %s", curl_easy_strerror(res));
            throw err;
        }
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("set curl_easy_setopt failed: %s", curl_easy_strerror(res));
            throw err;
        }

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            throw err;
        }
        *pOk = true;
    }
    if  (!m_wt.bOk)
    {
        *pOk = false;
        m_wt.s.Empty();
    }
    return m_wt.s;
}

void CMaaCURLWrapper::SetProgress(fnProgressMaacURLWriter fnProgress, void * ProgressParam) noexcept
{
    m_wt.ProgressParam = ProgressParam;
    m_wt.fnProgress = fnProgress;
}
CMaaString CMaaCURLWrapper::GetFile(CMaaString url, CMaaFile File, CMaaString * pstrHeaders, bool * pOk)
{
    File.Clear();
    m_wt.FilePos = 0;
    m_wt.bOk = true;
    //m_wt.maxlen = m_MaxLen;
    m_wt.s.Empty();
    m_wt.f = nullptr;
    m_wt.f = &File;
    bool b;
    pOk = pOk ? pOk : &b;
    *pOk = false;
    int pq = url.Find('?');
    pq = pq >= 0 ? pq : url.Length();
    int pb = url.ReverseFind(pq - 1, '/');
    CMaaString FileName = UrlWithMixedUtf8ToUtf8(url.Mid(pb + 1, pq - (pb + 1)));
    
    if  (curl)
    {
        SetUrl(url);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnMaacURLWriter2);
        if  (res != CURLE_OK)
        {
            m_wt.f = nullptr;
            CMaaString err;
            err.Format("set CURLOPT_WRITEFUNCTION failed: %s", curl_easy_strerror(res));
            throw err;
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_wt);
        if  (res != CURLE_OK)
        {
            m_wt.f = nullptr;
            CMaaString err;
            err.Format("set CURLOPT_WRITEDATA failed: %s", curl_easy_strerror(res));
            throw err;
        }
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        if  (res != CURLE_OK)
        {
            m_wt.f = nullptr;
            CMaaString err;
            err.Format("set curl_easy_setopt failed: %s", curl_easy_strerror(res));
            throw err;
        }

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        m_wt.f = nullptr;
        // Check for errors
        if  (res != CURLE_OK)
        {
            CMaaString err(File.GetMsg());
            if (err.IsNotEmpty())
            {
                XTOOFile2Error e((DWORD)0);
                e = (XTOOFile2Error &)File;
                if  (File.IsThrow())
                {
                    throw e;
                }
            }
            else
            {
                err.Format("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            }
            throw err;
        }
        *pOk = true;
    }
    if  (!m_wt.bOk)
    {
        *pOk = false;
        m_wt.s.Empty();
    }
    CMaaString strHeaders = m_wt.s;
    CMaaString tmp = strHeaders;
    while(tmp.IsNotEmpty())
    {
        CMaaString Line = tmp.GetLine();
        while(Line[0] == ' ')
        {
            Line = Line.RefMid(1);
        }
        if (Line.IsLeftCi("Content-Disposition", (int)strlen("Content-Disposition"), 0))
        {
            Line = Line.RefMid((int)strlen("Content-Disposition"));
            while(Line[0] == ' ')
            {
                Line = Line.RefMid(1);
            }
            if (Line[0] == ':')
            {
                Line = Line.RefMid(1);
                while(Line[0] == ' ')
                {
                    Line = Line.RefMid(1);
                }
                if (Line.IsLeftCi("attachment", (int)strlen("attachment"), 0))
                {
                    Line = Line.RefMid((int)strlen("attachment"));
                    while(Line[0] == ' ')
                    {
                        Line = Line.RefMid(1);
                    }
                    if (Line[0] == ';')
                    {
                        Line = Line.RefMid(1);
                        while(Line[0] == ' ')
                        {
                            Line = Line.RefMid(1);
                        }
                        if (Line.IsLeftCi("filename", (int)strlen("filename"), 0))
                        {
                            Line = Line.RefMid((int)strlen("filename"));
                            while(Line[0] == ' ')
                            {
                                Line = Line.RefMid(1);
                            }
                            if (Line[0] == '=')
                            {
                                Line.GetWord("\"");
                                CMaaString fn = GetMailSubject(Line.GetWord("\""));
                                if (fn.IsNotEmpty())
                                {
                                    FileName = fn;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (pstrHeaders)
    {
        *pstrHeaders = strHeaders;
    }
    return FileName;
}

CMaaString CMaaCURLWrapper::Put(CMaaString url, CMaaString Data, bool * pOk)
{
    m_wt.FilePos = 0;
    m_wt.f = nullptr;
    m_wt.bOk = true;
    //m_wt.maxlen = m_MaxLen;
    m_wt.s.Empty();
    bool b;
    pOk = pOk ? pOk : &b;
    *pOk = false;
    if  (curl)
    {
        SetUrl(url);

        //curl_easy_setopt(curl, CURLOPT_RETURNTRANSFER, 1);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);

        curl_easy_setopt(curl, CURLOPT_POST, 1);

        //curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

        //CMaaString wt;
        ////curl_easy_setopt(curl, CURLOPT_READDATA, &wt);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (const char *)Data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)Data.Length());

        //res = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnMaacURLWriter);
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("set CURLOPT_WRITEFUNCTION failed: %s", curl_easy_strerror(res));
            throw err;
        }
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_wt);
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("set CURLOPT_WRITEDATA failed: %s", curl_easy_strerror(res));
            throw err;
        }
        //curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1);
        //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors
        if  (res != CURLE_OK)
        {
            CMaaString err;
            err.Format("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            throw err;
        }
        *pOk = true;
    }
    if  (!m_wt.bOk)
    {
        *pOk = false;
        m_wt.s.Empty();
    }
    return m_wt.s;
}

CMaaString CMaaCURLWrapper::GetHdr(CMaaString url, CMaaString Hdr, bool * pOk)
{
    m_wt.FilePos = 0;
    m_wt.f = nullptr;
    m_wt.bOk = true;
    //m_wt.maxlen = m_MaxLen;
    m_wt.s.Empty();
    bool b;
    pOk = pOk ? pOk : &b;
    *pOk = false;
    CMaaPtr_<CMaaString, 1> hs;
    //try
    {
        if  (curl)
        {
            SetUrl(url);

            while(Hdr.IsNotEmpty())
            {
                CMaaString h = Hdr.GetLine(true, true, false);
                if (h.IsNotEmpty())
                {
                    hs += h;
                }
            }

            //curl_easy_setopt(curl, CURLOPT_RETURNTRANSFER, 1); //--++
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); //++
            curl_easy_setopt(curl, CURLOPT_HEADER, 0);
            struct curl_slist *chunk = NULL;
            for (int i = 0; i < (int)hs.MaxIndex(); i++)
            {
                /* Remove a header curl would otherwise add by itself */
                /* Add a custom header */
                chunk = curl_slist_append(chunk, (const char *)hs[i]);
            }
            /* set our custom set of headers */
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk); //++

            //curl_easy_setopt(curl, CURLOPT_POST, 0);
            res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnMaacURLWriter);
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("set CURLOPT_WRITEFUNCTION failed: %s", curl_easy_strerror(res));
                throw err;
            }
            res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_wt);
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("set CURLOPT_WRITEDATA failed: %s", curl_easy_strerror(res));
                throw err;
            }
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("set curl_easy_setopt failed: %s", curl_easy_strerror(res));
                throw err;
            }

            // Perform the request, res will get the return code
            res = curl_easy_perform(curl);
            // Check for errors
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("curl_easy_perform() failed: %s", curl_easy_strerror(res));
                throw err;
            }
            *pOk = true;
        }
    }
    //catch(...)
    //{
    //    throw;
    //}
    if  (!m_wt.bOk)
    {
        *pOk = false;
        m_wt.s.Empty();
    }
    return m_wt.s;
}

CMaaString CMaaCURLWrapper::PutHdr(CMaaString url, CMaaString Hdr, CMaaString Data, bool * pOk)
{
    m_wt.FilePos = 0;
    m_wt.f = nullptr;
    m_wt.bOk = true;
    //m_wt.maxlen = m_MaxLen;
    m_wt.s.Empty();
    bool b;
    pOk = pOk ? pOk : &b;
    *pOk = false;
    CMaaPtr_<CMaaString, 1> hs;
    //try
    {
        if  (curl)
        {
            SetUrl(url);

            while(Hdr.IsNotEmpty())
            {
                CMaaString h = Hdr.GetLine(true, true, false);
                if (h.IsNotEmpty())
                {
                    hs += h;
                }
            }

            res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            //curl_easy_setopt(curl, CURLOPT_RETURNTRANSFER, 1); //--++
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); //++
            curl_easy_setopt(curl, CURLOPT_HEADER, 0);
            struct curl_slist *chunk = NULL;
            for (int i = 0; i < (int)hs.MaxIndex(); i++)
            {
                /* Remove a header curl would otherwise add by itself */
                /* Add a custom header */
                chunk = curl_slist_append(chunk, (const char *)hs[i]);
            }
            /* set our custom set of headers */
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk); //++

            curl_easy_setopt(curl, CURLOPT_POST, 1);

            //curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

            //CMaaString wt;
            ////curl_easy_setopt(curl, CURLOPT_READDATA, &wt);

            curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (const char *)Data);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)Data.Length());

            //res = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
            res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnMaacURLWriter);
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("set CURLOPT_WRITEFUNCTION failed: %s", curl_easy_strerror(res));
                throw err;
            }
            res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_wt);
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("set CURLOPT_WRITEDATA failed: %s", curl_easy_strerror(res));
                throw err;
            }
            //curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1);
            //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            // Perform the request, res will get the return code
            res = curl_easy_perform(curl);
            // Check for errors
            if  (res != CURLE_OK)
            {
                CMaaString err;
                err.Format("curl_easy_perform() failed: %s", curl_easy_strerror(res));
                throw err;
            }
            *pOk = true;
        }
    }
    //catch(...)
    //{
    //    throw;
    //}
    if  (!m_wt.bOk)
    {
        *pOk = false;
        m_wt.s.Empty();
    }
    return m_wt.s;
}

#endif // NO_CURL_WRP
