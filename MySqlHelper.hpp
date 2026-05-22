
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/MySqlHelper.h

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

#ifndef MYSQL_PORT
#ifdef MARIADB_PORT
#define MYSQL_PORT MARIADB_PORT
#else
#define MYSQL_PORT 3306
#endif
#endif

class CMySqlHelper
{
    bool bOk;
    MYSQL * myData;
    bool bConnected;
#ifndef __unix__
    HINSTANCE hLib;
#endif

    CMaaString m_Err;

    CMaaUnivHash<CMaaString, int> m_hFieldName2Idx; // for local allocator sharing
    CMaaUnivHash<int, CMaaString> m_hFieldIdx2Name; // for local allocator sharing

    //static CMaaUnivHash<CMaaString, int, true, 1> s_hFieldName2Idx; // for global allocator
    //static CMaaUnivHash<int, CMaaString, true, 1> s_hFieldIdx2Name; // for global allocator

#ifndef __unix__
    MYSQL * (STDCALL *mysql_init)(MYSQL *mysql);
    MYSQL * (STDCALL *mysql_real_connect)(MYSQL *mysql, const char *host,
         const char *user,
         const char *passwd,
         const char *db,
         unsigned int port,
         const char *unix_socket,
         unsigned long clientflag);
    int (STDCALL *mysql_query)(MYSQL *mysql, const char *q);
    void (STDCALL *mysql_close)(MYSQL *sock);
    unsigned int (STDCALL *mysql_errno)(MYSQL *mysql);
    const char * (STDCALL *mysql_error)(MYSQL *mysql);
    MYSQL_RES * (STDCALL *mysql_store_result)(MYSQL *mysql);
    MYSQL_ROW   (STDCALL *mysql_fetch_row)(MYSQL_RES *result);
    my_ulonglong (STDCALL *mysql_num_rows)(MYSQL_RES *res);
    void (STDCALL *mysql_data_seek)(MYSQL_RES *result, my_ulonglong offset);
    unsigned int (STDCALL *mysql_num_fields)(MYSQL_RES *res);
    MYSQL_FIELD * (STDCALL *mysql_fetch_field)(MYSQL_RES *result);
    MYSQL_FIELD * (STDCALL *mysql_fetch_fields)(MYSQL_RES *result);
    void (STDCALL *mysql_free_result)(MYSQL_RES *result);
    unsigned long * (STDCALL *mysql_fetch_lengths)(MYSQL_RES *result);
#endif
    enum eNullCursor_
    {
        eNullCursor
    };
public:
    CMySqlHelper(const char * Server = nullptr, const char * UserName = nullptr, const char * Password = nullptr, int Port = MYSQL_PORT);
    ~CMySqlHelper();
    void Close() noexcept;
    int Query(const char * szSQL, bool bThrow = false);
    int Query(const char * format, CMaaString Query, ...);
    void QueryExc(const char * format, CMaaString Query, ...);
    class Cursor;
    Cursor QueryCursor(const char * szSQL);
    Cursor QueryCursorEx(const char * format, CMaaString Query, ...);
    bool IsOk() const noexcept;
    CMaaString GetError(const CMaaString &strSql = CMaaStringZ, bool bSetVar = true) noexcept;

    Cursor GetCursor();
    MYSQL_RES * StoreResult() noexcept;
    MYSQL_ROW	FetchRow(MYSQL_RES *result) noexcept;
    //unsigned int NumFields(MYSQL_RES *result);
    unsigned long * FetchLengths(MYSQL_RES *result) noexcept;
    _uqword NumRows(MYSQL_RES *res) noexcept;
    void Seek(MYSQL_RES *res, _uqword pos /*offset*/) noexcept;
    unsigned int NumFields(MYSQL_RES *res) noexcept;
    MYSQL_FIELD * FetchField(MYSQL_RES *result) noexcept;
    MYSQL_FIELD * FetchFields(MYSQL_RES *result) noexcept;
    void FreeResult(MYSQL_RES *result) noexcept;
    static CMaaString WildcardToSqlLike(CMaaString txt);
    static CMaaString ToSqlText(const CMaaString &txt);
    static CMaaString FromSqlText(const CMaaString &txt) noexcept;
    static CMaaString ToSqlText_old(const CMaaString &txt);
    static CMaaString FromSqlText_old(const CMaaString &txt);
    static time_t sscanf_DateTime(const CMaaString &Date, const CMaaString &Time) noexcept
    {
        time_t t = ConvertTextDateTime(Date, Time, '-', true);
        return t != (time_t)-1 ? t : 0;
    }
    static bool sprintf_DateTime(time_t t, CMaaString *pDate = nullptr, CMaaString *pTime = nullptr) noexcept;
    static time_t FromTs(const CMaaString &sql_timestamp) noexcept
    {
        time_t t = ConvertTextDateTime(sql_timestamp, '-', true);
        return t != (time_t)-1 ? t : 0;
    }
    static CMaaString ToTs(time_t t) noexcept;

    class Cursor
    {
        CMySqlHelper * m_pHelper;
        MYSQL_RES * m_res;
        _uqword m_N;
        MYSQL_ROW m_row;
        unsigned long * m_lengths;
        unsigned int m_num_fields;
        bool m_Associated;
        CMaaUnivHash<CMaaString, int> m_hFieldName2Idx;
        CMaaUnivHash<int, CMaaString> m_hFieldIdx2Name;
    public:
        Cursor();
        Cursor(const Cursor& That);
        Cursor(Cursor&& That);
        void operator=(const Cursor& That) noexcept;
        void operator=(Cursor&& That) noexcept;
        Cursor(CMySqlHelper* h, bool bAssoc = false);
        Cursor(CMySqlHelper* h, eNullCursor_ e);
        _uqword GetCount() noexcept;
        bool Seek(_uqword pos) noexcept;
        bool Fetch() noexcept;
        CMaaString Get__(int idx);
        CMaaString GetMem__(int idx);
        //void GetFL(int idx, char ** pp, int * pl); // CMySqlHelper debug
        CMaaString Get(int idx);
        CMaaString GetMem(int idx);
        CMaaString Get_(const CMaaString &FieldName);
        CMaaString Get__(int idx, bool * pIsColumnExist /*= nullptr*/);
        CMaaString Get(int idx, bool * pIsColumnExist /*= nullptr*/);
        CMaaString GetMem__(int idx, bool * pIsColumnExist /*= nullptr*/);
        CMaaString GetMem(int idx, bool * pIsColumnExist /*= nullptr*/);
        CMaaString Get_(const CMaaString &FieldName, bool * pIsColumnExist /*= nullptr*/);
        // GetXXX(,) the same as XXX(,)
        int GetInt(int idx, int def);
        unsigned int GetUInt(int idx, unsigned int def);
        _qword GetQword(int idx, _qword def);
        _uqword GetUQword(int idx, _uqword def);
        // GetXXX(,) the same as XXX(,)
        int Int(int idx, int def = 0);
        unsigned int UInt(int idx, unsigned int def = 0);
        _qword Qword(int idx, _qword def = 0);
        _uqword UQword(int idx, _uqword def = 0);
        CMaaString GetFieldName(int idx);
        int GetFieldIdx(const CMaaString &name);
        int NumFields() noexcept;
        void Associate();
        ~Cursor();
    };
    _qword GetLastInsertId();
    _qword GetRowCount();
};

#endif
