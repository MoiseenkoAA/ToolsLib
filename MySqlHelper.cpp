
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/MySqlHelper.cpp

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This library contains cross-platform classes for working with
 * MySQL databases: queries, cursors, exceptions, string and date
 * converting from/to MySQL format.
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

#if 1
#ifdef __unix__
#include <mysql/mysql.h>
#else
#include <mysql.h>
#endif

//CMaaUnivHash<CMaaString, int, true, 1> CMySqlHelper::s_hFieldName2Idx(1, nullptr, 0);
//CMaaUnivHash<int, CMaaString, true, 1> CMySqlHelper::s_hFieldIdx2Name(1, nullptr, 0);

#ifndef __unix__
DEF_ALLOCATOR_CMaaUnivHash(CMaaString, int)
#endif
//DEF_ALLOCATOR_CMaaUnivHash(int, CMaaString)

CMySqlHelper::CMySqlHelper(const char * Server, const char * UserName, const char * Password, int Port)
:   m_hFieldName2Idx(0),
    m_hFieldIdx2Name(0)
{
    if  (Port <= 0)
    {
        Port = MYSQL_PORT;
    }
    myData = nullptr;
    bConnected = false;
    bOk = false;
    //char dir[MAX_PATH + 1];
    //::GetCurrentDirectory(sizeof(dir), dir);
#ifndef __unix__
    hLib = LoadLibrary(TEXT("libmySQL.dll"));
    if  (!hLib)
    {
        m_Err = "Error loading libmySQL.dll";
        return;
    }
    m_Err = "Error GetProcAddress for function in libmySQL.dll";
    (FARPROC &)mysql_init = GetProcAddress(hLib, "mysql_init");
    (FARPROC &)mysql_real_connect = GetProcAddress(hLib, "mysql_real_connect");
    (FARPROC &)mysql_query = GetProcAddress(hLib, "mysql_query");
    (FARPROC &)mysql_close = GetProcAddress(hLib, "mysql_close");
    (FARPROC &)mysql_errno = GetProcAddress(hLib, "mysql_errno");
    (FARPROC &)mysql_error = GetProcAddress(hLib, "mysql_error");
    (FARPROC &)mysql_store_result = GetProcAddress(hLib, "mysql_store_result");
    (FARPROC &)mysql_fetch_row = GetProcAddress(hLib, "mysql_fetch_row");
    (FARPROC &)mysql_num_rows = GetProcAddress(hLib, "mysql_num_rows");
    (FARPROC &)mysql_num_fields = GetProcAddress(hLib, "mysql_num_fields");
    (FARPROC &)mysql_fetch_field = GetProcAddress(hLib, "mysql_fetch_field");
    (FARPROC &)mysql_fetch_fields = GetProcAddress(hLib, "mysql_fetch_fields");
    (FARPROC &)mysql_free_result = GetProcAddress(hLib, "mysql_free_result");
    (FARPROC &)mysql_fetch_lengths = GetProcAddress(hLib, "mysql_fetch_lengths");

    if  (!mysql_init ||
         !mysql_real_connect ||
         !mysql_query ||
         !mysql_close ||
         !mysql_errno ||
         !mysql_error ||
         !mysql_store_result ||
         !mysql_fetch_row ||
         !mysql_num_rows ||
         !mysql_num_fields ||
         !mysql_fetch_field ||
         !mysql_fetch_fields ||
         !mysql_free_result
         )
    {
        return;
    }

#endif

    myData = mysql_init(nullptr);
    if  (!myData)
    {
        return;
    }
    if  (!mysql_real_connect(myData, Server, UserName, Password, nullptr, Port, nullptr, 0))
    {
        return;
    }
#ifndef MARIADB_PORT
    myData->reconnect = 1;
#endif
    bConnected = true;
    //Sleep(1000);
    /*
          if (mysql_query(myData, szCreateDb))
          {
                m_Err = GetError();
                mysql_close(myData);
                myData = nullptr;
                return;
          }
          if (mysql_query(myData, szUseDb))
          {
                m_Err = GetError();
                mysql_close(myData);
                myData = nullptr;
                return;
          }
          if (szCreateTbl && mysql_query(myData, szCreateTbl))
          {
                m_Err = GetError();
                mysql_close(myData);
                myData = nullptr;
                return;
          }
          */
    bOk = true;
}
CMySqlHelper::~CMySqlHelper()
{
    Close();
#ifndef __unix__
    if  (hLib)
    {
        FreeLibrary(hLib);
    }
#endif
}
void CMySqlHelper::Close() noexcept
{
    if  (/*bConnected &&*/ myData)
    {
        mysql_close(myData);
        myData = nullptr;
    }
}
/*
    int Query(const char * szSQL)
    {
        bool bThrow = false;
        //  char * szAddQuery1 = "INSERT DELAYED INTO priemka (date, time, dst, device, rawval, val)"
        //       " VALUES ('2008-09-15', '09:56:00', 's', 'S1', 0.0, 0.0)";
        if   (!bOk || !myData)
        {
            if (bThrow)
            {
                throw GetError();
            }
            return -1;
        }
        int r = mysql_query(myData, szSQL);
        if (r && bThrow)
        {
            throw GetError();
        }
        return r;
    }
    */
int CMySqlHelper::Query(const char * szSQL, bool bThrow /*= false*/)
{
    //  char * szAddQuery1 = "INSERT DELAYED INTO priemka (date, time, dst, device, rawval, val)"
    //       " VALUES ('2008-09-15', '09:56:00', 's', 'S1', 0.0, 0.0)";
    if  (!bOk || !myData)
    {
        if  (bThrow)
        {
            throw GetError();
        }
        return -1;
    }
    const int r = mysql_query(myData, szSQL);
    if  (r && bThrow)
    {
        throw GetError(CMaaString(szSQL));
    }
    return r;
}
int CMySqlHelper::Query(const char * format, CMaaString Query, ...)
{
    CMaaString txt;
    va_list list;
    va_start(list, Query);
    txt.FormatV2(format, Query, list);
    va_end(list);
    return this->Query(txt);
}
void CMySqlHelper::QueryExc(const char * format, CMaaString Query, ...)
{
    CMaaString txt;
    va_list list;
    va_start(list, Query);
    txt.FormatV2(format, Query, list);
    va_end(list);
    this->Query(txt, true);
}
//class Cursor;
CMySqlHelper::Cursor CMySqlHelper::QueryCursor(const char * szSQL)
{
    if  (bOk && myData)
    {
        if (!mysql_query(myData, szSQL))
        {
            return Cursor(this); //GetCursor();
        }
    }
    return Cursor(this, eNullCursor); // return Cursor();
}
CMySqlHelper::Cursor CMySqlHelper::QueryCursorEx(const char * format, CMaaString Query, ...)
{
    if  (bOk && myData)
    {
        CMaaString txt;
        va_list list;
        va_start(list, Query);
        txt.FormatV2(format, Query, list);
        va_end(list);
        this->Query(txt, true);
        //if (!r)
        {
            return Cursor(this); //GetCursor();
        }
    }
    return Cursor(this, eNullCursor); // return Cursor();
}
bool CMySqlHelper::IsOk() const noexcept
{
    return bOk;
}
CMaaString CMySqlHelper::GetError(const CMaaString &strSql, bool bSetVar) noexcept
{
    if  (myData && mysql_errno && mysql_error)
    {
        CMaaString txt;
#ifdef _WIN32
        //txt = mysql_error(myData);
        txt = UnicodeToUtf8(AnsiToUnicode(mysql_error(myData)));
        txt.Format("MySQL error: [%d] %S%s%S", mysql_errno(myData), &txt, strSql.IsNotEmpty() ? "; query: " : "", &strSql);
#else
        txt.Format("MySQL error: [%d] %s%s%S", mysql_errno(myData), mysql_error(myData), strSql.IsNotEmpty() ? "; query: " : "", &strSql);
#endif
        if  (bSetVar)
        {
            m_Err = txt;
        }
        return txt;
    }
    return m_Err;
}

CMySqlHelper::Cursor CMySqlHelper::GetCursor()
{
    return Cursor(this);
}
MYSQL_RES * CMySqlHelper::StoreResult() noexcept
{
    if  (!bOk || !myData || !mysql_store_result)
    {
        return nullptr;
    }
    return mysql_store_result(myData);
}
MYSQL_ROW CMySqlHelper::FetchRow(MYSQL_RES *result) noexcept
{
    if  (!bOk || !myData || !mysql_fetch_row)
    {
        return nullptr;
    }
    return mysql_fetch_row(result);
}
MYSQL_FIELD * CMySqlHelper::FetchFields(MYSQL_RES *result) noexcept
{
    if  (!bOk || !myData || !mysql_fetch_fields)
    {
        return nullptr;
    }
    return mysql_fetch_fields(result);
}
/*unsigned int CMySqlHelper::NumFields(MYSQL_RES *result)
    {
        if   (!bOk || !myData || !mysql_num_fields)
        {
            return 0;
        }
        return mysql_num_fields(result);
    }*/
unsigned long * CMySqlHelper::FetchLengths(MYSQL_RES *result) noexcept
{
    if  (!bOk || !myData || !mysql_fetch_lengths)
    {
        return nullptr;
    }
    return mysql_fetch_lengths(result);
}
_uqword CMySqlHelper::NumRows(MYSQL_RES *res) noexcept
{
    if  (!bOk || !myData || !mysql_num_rows)
    {
        return 0;
    }
    return (_uqword) /*my_ulonglong*/ mysql_num_rows(res);
}
void CMySqlHelper::Seek(MYSQL_RES *res, _uqword pos) noexcept
{
    if  (!bOk || !myData || !mysql_data_seek)
    {
        return;
    }
    mysql_data_seek(res, pos);
}
unsigned int CMySqlHelper::NumFields(MYSQL_RES *res) noexcept
{
    if  (!bOk || !myData || !mysql_num_fields)
    {
        return 0;
    }
    return mysql_num_fields(res);
}
MYSQL_FIELD * CMySqlHelper::FetchField(MYSQL_RES *result) noexcept
{
    if  (!bOk || !myData || !mysql_fetch_field)
    {
        return nullptr;
    }
    return mysql_fetch_field(result);
}
void CMySqlHelper::FreeResult(MYSQL_RES *result) noexcept
{
    if  (!bOk || !myData || !mysql_free_result)
    {
        return;
    }
    mysql_free_result(result);
}

struct sMySql_ToSqlText_old_Map
{
    char m[256];
    constexpr sMySql_ToSqlText_old_Map() noexcept
    {
        for (int_ i = 0; i < 256; i++)
        {
            m[i] = 0;
        }
        m['\\'] = '\\';
        m['\''] = '\'';
        m['"'] = '"';
        m['\b'] = 'b';
        m['\n'] = 'n';
        m['\r'] = 'r';
        m['\t'] = 't';
        m[0x1A] = 'Z';
        m['%'] = '%';
        m['_'] = '_';
        m[0] = '0';
    }
};

static constexpr sMySql_ToSqlText_old_Map sToOldMap;

CMaaString CMySqlHelper::ToSqlText_old(const CMaaString &txt)
{
#if 0
    txt.ReplaceNN('\\', "\\\\");
    txt.ReplaceNN('\'', "\\'");
    txt.ReplaceNN('"', "\\\"");
    txt.ReplaceNN('\b', "\\b");
    txt.ReplaceNN('\n', "\\n");
    txt.ReplaceNN('\r', "\\r");
    txt.ReplaceNN('\t', "\\t");
    txt.ReplaceNN('\x1A', "\\Z");
    txt.ReplaceNN('%', "\\%");
    txt.ReplaceNN('_', "\\_");
    txt.ReplaceNN(CMaaString("\0", 1), "\\0"); // unchecked
#else
    //const sMySql_ToSqlText_old_Map& sToOldMap = sMySql_ToSqlText_old();
    /*
    static char sMap[256];
    if (!sMap[0])
    {
        sMap['\\'] = '\\';
        sMap['\''] = '\'';
        sMap['"'] = '"';
        sMap['\b'] = 'b';
        sMap['\n'] = 'n';
        sMap['\r'] = 'r';
        sMap['\t'] = 't';
        sMap[0x1A] = 'Z';
        sMap['%'] = '%';
        sMap['_'] = '_';
        sMap[0] = '0';
    }
    */
    char csBuffer[TOOLSLIB_CS_64K];
    int pos0, pos;
    const int Len = (warning_int)txt.Length();
    CMaaConcatString res(csBuffer, sizeof(csBuffer), Len > 0 ? (Len << 1) : 1);
    const char* p = txt;
    char c2[2];
    c2[0] = '\\';
    for (pos = pos0 = 0; pos < Len; pos++)
    {
        c2[1] = sToOldMap.m[(unsigned char)p[pos]];
        if (c2[1])
        {
            res.Add(p + pos0, pos - pos0);
            res.Add(c2, 2);
            pos0 = pos + 1;
        }
    }
    if (pos0)
    {
        res.Add(p + pos0, Len - pos0);
        return res;
    }
#endif
    return txt;
}

struct sMySql_ToSqlText_Map
{
    char m[256];
    constexpr sMySql_ToSqlText_Map() noexcept
    {
        for (int_ i = 0; i < 256; i++)
        {
            m[i] = 0;
        }
        m['\\'] = '\\';
        m['\''] = '\'';
        m['"'] = '"';
        m['\b'] = 'b';
        m['\n'] = 'n';
        m['\r'] = 'r';
        m['\t'] = 't';
        m[0x1A] = 'Z';
        m[0] = '0';
    }
};

static constexpr sMySql_ToSqlText_Map sToMap;

CMaaString CMySqlHelper::ToSqlText(const CMaaString &txt)
{
#if 0
    txt.ReplaceNN('\\', "\\\\");
    txt.ReplaceNN('\'', "\\'");
    txt.ReplaceNN('"', "\\\"");
    txt.ReplaceNN('\b', "\\b");
    txt.ReplaceNN('\n', "\\n");
    txt.ReplaceNN('\r', "\\r");
    txt.ReplaceNN('\t', "\\t");
    txt.ReplaceNN('\x1A', "\\Z");
    //txt.ReplaceNN('%', "\\%");
    //txt.ReplaceNN('_', "\\_");
    txt.ReplaceNN(CMaaString("\0", 1), "\\0"); // unchecked
#else
    //const sMySql_ToSqlText_Map& sToMap = sMySql_ToSqlText();
    /*
    static char sMap[256];
    if (!sMap[0])
    {
        sMap['\\'] = '\\';
        sMap['\''] = '\'';
        sMap['"'] = '"';
        sMap['\b'] = 'b';
        sMap['\n'] = 'n';
        sMap['\r'] = 'r';
        sMap['\t'] = 't';
        sMap[0x1A] = 'Z';
        sMap[0] = '0';
    }
    */
    char csBuffer[TOOLSLIB_CS_64K];
    int pos0, pos;
    const int Len = (warning_int)txt.Length();
    CMaaConcatString res(csBuffer, sizeof(csBuffer), Len > 0 ? (Len << 1) : 1);
    const char* p = txt;
    char c2[2];
    c2[0] = '\\';
    for (pos = pos0 = 0; pos < Len; pos++)
    {
        c2[1] = sToMap.m[(unsigned char)p[pos]];
        if (c2[1])
        {
            res.Add(p + pos0, pos - pos0);
            res.Add(c2, 2);
            pos0 = pos + 1;
        }
    }
    if (pos0)
    {
        res.Add(p + pos0, Len - pos0);
        return res;
    }
#endif
    return txt;
}
CMaaString CMySqlHelper::WildcardToSqlLike(CMaaString txt)
{
    while(txt.ReplaceNN("**", "*") > 0) ;
    txt = CMySqlHelper::ToSqlText(txt);
    txt.ReplaceNN('%', "\\%");
    txt.ReplaceNN('_', "\\_");
    txt.ReplaceNN('*', "%");
    txt.ReplaceNN('?', "_");
    if  (txt == "%")
    {
        txt.Empty();
    }
    return txt;
}

struct sMySql_FromSqlText_old_Map
{
    char m[256];
    constexpr sMySql_FromSqlText_old_Map() noexcept
    {
        for (int_ i = 0; i < 256; i++)
        {
            m[i] = -1;
        }
        m['\\'] = '\\';
        m['\''] = '\'';
        m['"'] = '"';
        m['b'] = '\b';
        m['n'] = '\n';
        m['r'] = '\r';
        m['t'] = '\t';
        m['Z'] = 0x1A;
        m['%'] = '%';
        m['_'] = '_';
        m['0'] = '\0';
    }
};

static constexpr sMySql_FromSqlText_old_Map sFromOldMap;

CMaaString CMySqlHelper::FromSqlText_old(const CMaaString &txt)
{
    //const sMySql_FromSqlText_old_Map& sFromOldMap = sMySql_FromSqlText_old();
    /*
    static char sMap[256];
    if (!sMap['0'])
    {
        sMap['\\'] = '\\';
        sMap['\''] = '\'';
        sMap['"'] = '"';
        sMap['b'] = '\b';
        sMap['n'] = '\n';
        sMap['r'] = '\r';
        sMap['t'] = '\t';
        sMap['Z'] = 0x1A;
        sMap['%'] = '%';
        sMap['_'] = '_';
        sMap['0'] = (char)0x80;
    }
    */
    char csBuffer[TOOLSLIB_CS_64K];
    int pos0, pos;
    const int Len = (warning_int)txt.Length();
    CMaaConcatString res(csBuffer, sizeof(csBuffer), Len > 0 ? Len : 1);
    const char* p = txt;
    for (pos = pos0 = 0; pos < Len; pos++)
    {
        if  (p[pos] == '\\' && pos + 1 < Len)
        {
            const char c = sFromOldMap.m[(unsigned char)p[pos + 1]];
            if (c >= 0)
            {
                res.Add(p + pos0, pos - pos0);
                //res += (char)(c & 0x7f);
                res += (char)c;
                pos0 = ++pos + 1;
            }
        }
    }
    if (pos0)
    {
        res.Add(p + pos0, Len - pos0);
        return res;
    }
    return txt;
}
CMaaString CMySqlHelper::FromSqlText(const CMaaString &txt) noexcept
{
    return txt;
}
#if 0
time_t CMySqlHelper::sscanf_DateTime(CMaaString Date, CMaaString Time) noexcept
{
    time_t t = 0;
    tm tt;
    memset(&tt, 0, sizeof(tt));

    int n = Date.Find('-');
    int x;
    if  (n > 0)
    {
        if  (Date.dsscanf("%d", &x) == 1 && x >= 1971 && x <= 2100)
        {
            tt.tm_year = x - 1900;
            Date = Date.RefMid(n + 1);

            n = Date.Find('-');
            if  (n > 0)
            {
                if  (Date.dsscanf("%d", &x) == 1)
                {
                    tt.tm_mon = x - 1;
                    Date = Date.RefMid(n + 1);
                    if  (Date.dsscanf("%d", &x) == 1)
                    {
                        tt.tm_mday = x;

                        int hh, mm, ss;
                        if  (Time.dsscanf("%d:%d:%d", &hh, &mm, &ss) == 3)
                        {
                            tt.tm_hour = hh;
                            tt.tm_min = mm;
                            tt.tm_sec = ss;
                            tt.tm_isdst = -1;
                            t = mktime(&tt);
                            if  (t == (time_t)-1)
                            {
                                t = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return t;
}
time_t CMySqlHelper::FromTs(CMaaString ts) noexcept
{
    time_t t = 0;

    const char* p = ts;
    const char* e = p + ts.Length();
    while(p < e && *p == ' ') p++;

    int n = 0, x = 0;
    while(p < e && p[n] >= '0' && p[n] <= '9')
    {
        x = x * 10 + (p[n] - '0');
        n++;
    }
    if  (n > 0 && p[n] == '-')
    {
        tm tt;
        memset(&tt, 0, sizeof(tt));

        tt.tm_year = x - 1900;
        p += n + 1;

        n = 0; x = 0;
        while(p < e && p[n] >= '0' && p[n] <= '9')
        {
            x = x * 10 + (p[n] - '0');
            n++;
        }
        if  (n > 0 && p[n] == '-')
        {
            tt.tm_mon = x - 1;
            p += n + 1;

            n = 0; x = 0;
            while(p < e && p[n] >= '0' && p[n] <= '9')
            {
                x = x * 10 + (p[n] - '0');
                n++;
            }
            if  (n > 0 && p[n] == ' ')
            {
                tt.tm_mday = x;
                p += n + 1;

                n = 0; x = 0;
                while(p < e && p[n] >= '0' && p[n] <= '9')
                {
                    x = x * 10 + (p[n] - '0');
                    n++;
                }
                if  (n > 0 && p[n] == ':')
                {
                    tt.tm_hour = x;
                    p += n + 1;

                    n = 0; x = 0;
                    while(p < e && p[n] >= '0' && p[n] <= '9')
                    {
                        x = x * 10 + (p[n] - '0');
                        n++;
                    }
                    if  (n > 0 && p[n] == ':')
                    {
                        tt.tm_min = x;
                        p += n + 1;

                        n = 0; x = 0;
                        while(p < e && p[n] >= '0' && p[n] <= '9')
                        {
                            x = x * 10 + (p[n] - '0');
                            n++;
                        }
                        p += n;
                        if  (n > 0 && (p == e || *p == ' '))
                        {
                            tt.tm_sec = x;
                            while(p < e && *p == ' ') p++;

                            if  (p == e)
                            {
                                tt.tm_isdst = -1;
                                t = tt.tm_year >= 0 ? mktime(&tt) : (time_t)-1;
                                if  (t == (time_t)-1)
                                {
                                    t = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return t;
}
#endif
bool CMySqlHelper::sprintf_DateTime(time_t t, CMaaString *pDate /*= nullptr*/, CMaaString *pTime /*= nullptr*/) noexcept
{
    const tm * p = localtime(&t);
    if  (p)
    {
        tm tt;
        memcpy(&tt, p, sizeof(tt));
        if  (pDate)
        {
            pDate->Format("%04d-%02d-%02d", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday);
        }
        if  (pTime)
        {
            pTime->Format("%02d:%02d:%02d", tt.tm_hour, tt.tm_min, tt.tm_sec);
        }
        return true;
    }
    return false;
}

CMaaString CMySqlHelper::ToTs(time_t t) noexcept
{
    const tm * p = localtime(&t);
    return p ? CMaaString::sFormat("%04d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec) :
        CMaaString("0000-00-00 00:00:00\0\0\0\0", 19, CMaaString::eROMemString0000);
}

CMySqlHelper::Cursor::Cursor()
:   m_pHelper(nullptr),
    m_res(nullptr),
    m_N(0),
    m_row(nullptr),
    m_lengths(nullptr),
    m_num_fields(0),
    m_Associated(false),
    m_hFieldName2Idx(0),
    m_hFieldIdx2Name(0)
    //m_hFieldName2Idx(0, &s_hFieldName2Idx, 0),
    //m_hFieldIdx2Name(0, &s_hFieldIdx2Name, 0)
{
}
CMySqlHelper::Cursor::Cursor(const Cursor& That)
:   m_pHelper(nullptr),
    m_res(nullptr),
    m_N(0),
    m_row(nullptr),
    m_lengths(nullptr),
    m_num_fields(0),
    m_Associated(false),
    m_hFieldName2Idx(0, (CMaaUnivHash<CMaaString, int>*)&That.m_hFieldName2Idx, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
    m_hFieldIdx2Name(0, (CMaaUnivHash<int, CMaaString>*)&That.m_hFieldIdx2Name, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2)
{
    *this = That;
}
CMySqlHelper::Cursor::Cursor(Cursor&& That)
:   m_pHelper(nullptr),
    m_res(nullptr),
    m_N(0),
    m_row(nullptr),
    m_lengths(nullptr),
    m_num_fields(0),
    m_Associated(false),
    m_hFieldName2Idx(0, &That.m_hFieldName2Idx, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
    m_hFieldIdx2Name(0, &That.m_hFieldIdx2Name, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2)
{
    *this = That;
}
void CMySqlHelper::Cursor::operator=(const Cursor& That) noexcept
{
    if (this == &That)
    {
        return;
    }
    if (m_res && m_pHelper)
    {
        m_pHelper->FreeResult(m_res);
    }
    m_pHelper = That.m_pHelper;
    m_res = That.m_res;
    m_N = That.m_N;
    m_row = That.m_row;
    m_lengths = That.m_lengths;
    m_num_fields = That.m_num_fields;
    m_hFieldName2Idx.Swap((CMaaUnivHash<CMaaString, int>&)That.m_hFieldName2Idx);
    m_hFieldIdx2Name.Swap((CMaaUnivHash<int, CMaaString>&)That.m_hFieldIdx2Name);
    m_Associated = That.m_Associated;
    Cursor& That2 = (Cursor&)That;
    That2.m_res = nullptr;
    That2.m_N = 0;
    That2.m_row = nullptr;
    That2.m_lengths = nullptr;
    That2.m_num_fields = 0;
    That2.m_hFieldName2Idx.Cleanup();
    That2.m_hFieldIdx2Name.Cleanup();
    That2.m_Associated = false;
}
void CMySqlHelper::Cursor::operator=(Cursor&& That) noexcept
{
    if (this == &That)
    {
        return;
    }
    if (m_res && m_pHelper)
    {
        m_pHelper->FreeResult(m_res);
    }
    m_pHelper = That.m_pHelper;
    m_res = That.m_res;
    m_N = That.m_N;
    m_row = That.m_row;
    m_lengths = That.m_lengths;
    m_num_fields = That.m_num_fields;
    m_hFieldName2Idx.Swap(That.m_hFieldName2Idx);
    m_hFieldIdx2Name.Swap(That.m_hFieldIdx2Name);
    m_Associated = That.m_Associated;
    That.m_res = nullptr;
    That.m_N = 0;
    That.m_row = nullptr;
    That.m_lengths = nullptr;
    That.m_num_fields = 0;
    That.m_hFieldName2Idx.Cleanup();
    That.m_hFieldIdx2Name.Cleanup();
    That.m_Associated = false;
}
CMySqlHelper::Cursor::Cursor(CMySqlHelper *h, bool bAssoc)
:   m_pHelper(h),
    m_N(0),
    m_row(nullptr),
    m_lengths(nullptr),
    m_num_fields(0),
    m_Associated(false),
    m_hFieldName2Idx(0, &h->m_hFieldName2Idx, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
    m_hFieldIdx2Name(0, &h->m_hFieldIdx2Name, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2)
{
    m_res = h->StoreResult();
    if  (m_res)
    {
        m_N = h->NumRows(m_res);
        m_num_fields = h->NumFields(m_res);
        if  (bAssoc)
        {
            Associate();
        }
    }
}
CMySqlHelper::Cursor::Cursor(CMySqlHelper* h, eNullCursor_)
:   m_pHelper(h),
    m_N(0),
    m_row(nullptr),
    m_lengths(nullptr),
    m_num_fields(0),
    m_Associated(false),
    m_hFieldName2Idx(0, &h->m_hFieldName2Idx, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
    m_hFieldIdx2Name(0, &h->m_hFieldIdx2Name, 0 TOOLSLIB_CHECK_HT_USAGE_ARG2),
    m_res(nullptr)
{
}
_uqword CMySqlHelper::Cursor::GetCount() noexcept
{
    return m_N;
}
bool CMySqlHelper::Cursor::Seek(_uqword pos) noexcept
{
    if  (m_pHelper && pos < m_N)
    {
        m_pHelper->Seek(m_res, pos);
        return true;
    }
    return false;
}
bool CMySqlHelper::Cursor::Fetch() noexcept
{
    if  (m_res && m_pHelper)
    {
        m_row = m_pHelper->FetchRow(m_res);
        //m_num_fields = m_row ? m_pHelper->NumFields(m_res) : 0;
        m_lengths = m_row ? m_pHelper->FetchLengths(m_res) : nullptr;
        return m_row != nullptr;
    }
    return false;
}
CMaaString CMySqlHelper::Cursor::Get__(int idx)
{
    if  (idx >= 0 && (unsigned)idx < m_num_fields)
    {
        return CMaaString(m_row[idx], (int)m_lengths[idx]);
    }
    return CMaaStringZ;
}
CMaaString CMySqlHelper::Cursor::GetMem__(int idx)
{
    if  (idx >= 0 && (unsigned)idx < m_num_fields)
    {
        return CMaaString(m_row[idx], (int)m_lengths[idx], CMaaString::eROMemString);
    }
    return CMaaStringZ;
}
int CMySqlHelper::Cursor::Int(int idx, int def)
{
    return GetMem__(idx).ToInt(def);
}
unsigned int CMySqlHelper::Cursor::UInt(int idx, unsigned int def)
{
    return GetMem__(idx).ToUInt(def);
}
_qword CMySqlHelper::Cursor::Qword(int idx, _qword def)
{
    return GetMem__(idx).ToQword(def);
}
_uqword CMySqlHelper::Cursor::UQword(int idx, _uqword def)
{
    return GetMem__(idx).ToUQword(def);
}
int CMySqlHelper::Cursor::GetInt(int idx, int def)
{
    return GetMem__(idx).ToInt(def);
}
unsigned int CMySqlHelper::Cursor::GetUInt(int idx, unsigned int def)
{
    return GetMem__(idx).ToUInt(def);
}
_qword CMySqlHelper::Cursor::GetQword(int idx, _qword def)
{
    return GetMem__(idx).ToQword(def);
}
_uqword CMySqlHelper::Cursor::GetUQword(int idx, _uqword def)
{
    return GetMem__(idx).ToUQword(def);
}
#if 0 // CMySqlHelper debug
void CMySqlHelper::Cursor::GetFL(int idx, char ** pp, int * pl)
{
    if  (idx >= 0 && (unsigned)idx < m_num_fields)
    {
        *pp = (char *)m_row[idx];
        *pl = (int)m_lengths[idx];
    }
    else
    {
        *pp = (char *)1;
        *pl = (int)-1;
    }
}
#endif
CMaaString CMySqlHelper::Cursor::Get(int idx)
{
    return /*CMySqlHelper * / FromSqlText*/(this->Get__(idx));
}
CMaaString CMySqlHelper::Cursor::GetMem(int idx)
{
    return /*CMySqlHelper * / FromSqlText*/(this->GetMem__(idx));
}
CMaaString CMySqlHelper::Cursor::Get_(const CMaaString &FieldName)
{
    Associate();
    return this->Get(GetFieldIdx(FieldName));
}
CMaaString CMySqlHelper::Cursor::Get__(int idx, bool * pIsColumnExist)
{
    if  (idx >= 0 && (unsigned)idx < m_num_fields)
    {
        if  (pIsColumnExist)
        {
            *pIsColumnExist = true;
        }
        return CMaaString(m_row[idx], (int)m_lengths[idx]);
    }
    if  (pIsColumnExist)
    {
        *pIsColumnExist = false;
    }
    return CMaaStringZ;
}
CMaaString CMySqlHelper::Cursor::GetMem__(int idx, bool * pIsColumnExist)
{
    if  (idx >= 0 && (unsigned)idx < m_num_fields)
    {
        if  (pIsColumnExist)
        {
            *pIsColumnExist = true;
        }
        return CMaaString(m_row[idx], (int)m_lengths[idx], CMaaString::eROMemString);
    }
    if  (pIsColumnExist)
    {
        *pIsColumnExist = false;
    }
    return CMaaStringZ;
}
CMaaString CMySqlHelper::Cursor::Get(int idx, bool * pIsColumnExist)
{
    return /*CMySqlHelper * / FromSqlText*/(this->Get__(idx, pIsColumnExist));
}
CMaaString CMySqlHelper::Cursor::GetMem(int idx, bool * pIsColumnExist)
{
    return /*CMySqlHelper * / FromSqlText*/(this->GetMem__(idx, pIsColumnExist));
}
CMaaString CMySqlHelper::Cursor::Get_(const CMaaString &FieldName, bool * pIsColumnExist)
{
    Associate();
    return this->Get(GetFieldIdx(FieldName), pIsColumnExist);
}
CMaaString CMySqlHelper::Cursor::GetFieldName(int idx)
{
    Associate();
    return m_hFieldIdx2Name[idx, CMaaStringZ]; // name
}
int CMySqlHelper::Cursor::GetFieldIdx(const CMaaString &name)
{
    Associate();
    return m_hFieldName2Idx[name, -1]; // idx
}
int CMySqlHelper::Cursor::NumFields() noexcept
{
    return (int)m_num_fields;
}
void CMySqlHelper::Cursor::Associate()
{
    if  (!m_Associated)
    {
        MYSQL_FIELD * fields = m_pHelper->FetchFields(m_res);
        if  (fields)
        {
            const int N = (int)m_num_fields;
            m_hFieldName2Idx.Cleanup(N != 1 ? N : 2);
            m_hFieldIdx2Name.Cleanup(N != 1 ? N : 2);
            for (int i = 0; i < N; i++)
            {
                //name, org_name
                CMaaString name;
                //if (fields[i].name_length)
                {
                    name = CMaaString(fields[i].name, fields[i].name_length);
                }
                /*name = FromSqlText(name);*/
                m_hFieldName2Idx.Add(name, i);
                m_hFieldIdx2Name.Add(i, name);
            }
        }
        m_Associated = true;
    }
}
CMySqlHelper::Cursor::~Cursor()
{
    if  (m_res && m_pHelper)
    {
        m_pHelper->FreeResult(m_res);
    }
}

_qword CMySqlHelper::GetLastInsertId()
{
    Cursor cur = QueryCursor("SELECT LAST_INSERT_ID()");
    return cur.Fetch() ? cur.GetQword(0, -1) : -1;
}
_qword CMySqlHelper::GetRowCount()
{
    Cursor cur = QueryCursor("SELECT ROW_COUNT()");
    return cur.Fetch() ? cur.GetQword(0, -1) : -1;
}
#endif
