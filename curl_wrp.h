
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/curl_wrp.h

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

#ifndef NO_CURL_WRP

//#include <curl/curl.h>

class CMaaCURLWrapperInit
{
    CMaaReference * ptr;
public:
    CMaaCURLWrapperInit();
    ~CMaaCURLWrapperInit();
    void AddRef();
    void Release();
};

struct str_for_fnMaacURLWriter;
typedef int (*fnProgressMaacURLWriter)(str_for_fnMaacURLWriter *wt);
struct str_for_fnMaacURLWriter
{
    CMaaConcatString s;
    int maxlen; // file header / html data len
    CMaaFile *f;
    _qword FilePos, MaxFileLen, ContentLength;
    void * ProgressParam;
    fnProgressMaacURLWriter fnProgress;
    bool bOk;
};

extern "C"
{
    int fnMaacURLWriter(char *data, size_t size, size_t nmemb, str_for_fnMaacURLWriter *wt);
    int fnMaacURLWriter2(char *data, size_t size, size_t nmemb, str_for_fnMaacURLWriter *wt);
    size_t fnMaacURLReader(void *ptr, size_t size, size_t nmemb, CMaaString *rt);
};

class CMaaCURLWrapper
{
protected:
    CURL *curl;
    CURLcode res;
    int m_Flags;
    long m_TimeOutMs;

    bool SetUrl(CMaaString url);

    str_for_fnMaacURLWriter m_wt;

public:
    CMaaCURLWrapper(long TimeOutMs = -1, int MaxLen = -1, int Flags = 0, _qword MaxFileLen = -1);
    ~CMaaCURLWrapper();
    CMaaString Get(CMaaString url, bool * pOk);
    // GetFile() returns a file name from headers, file is appended to a File, optional *pstrHeaders returns the headers
    CMaaString GetFile(CMaaString url, CMaaFile File, CMaaString * pstrHeaders, bool * pOk);
    CMaaString Put(CMaaString url, CMaaString Data, bool * pOk);
    CMaaString GetHdr(CMaaString url, CMaaString Hdr, bool * pOk);
    CMaaString PutHdr(CMaaString url, CMaaString Hdr, CMaaString Data, bool * pOk);
    void SetProgress(fnProgressMaacURLWriter fnProgress, void * ProgressParam) noexcept;
};

#endif // NO_CURL_WRP
