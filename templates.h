
// ToolsLib Project

/* ToolsLib library for RusRoute firewall and other projects of
 * Andrey A. Moiseenko / IE Moiseenko A.A. (Russia)
 * e-mail: support@maasoftware.ru, maa2002@mail.ru
 * web: http://maasoftware.ru, http://maasoftware.com, http://maasoft.ru, http://maasoft.org
 * Author's full name: Andrey Alekseevitch Moiseenko
 * (russian name: Моисеенко Андрей Алексеевич)
 */

// ToolsLib/templates.h

/* Copyright (C) 2002-2024 Andrey A. Moiseenko (support@maasoftware.ru)
 * All rights reserved.
 *
 * This file contains some templates for HRT.
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


//#ifndef __unix__
class CHrtMultiTimer : public CMaaTime
{
#define MaxN 64
    _qword m_Time[MaxN];
    _qword m_Mem[MaxN];
    const char * m_Msg[MaxN];
    int m_pos;
public:
    CHrtMultiTimer(CMaaTime * pTime = nullptr)
    :   CMaaTime(pTime ? *pTime : gHRTime)
    {
        m_pos = 0;
    }
    void Start() noexcept
    {
        StartWatch();
    }
    _qword Stop(const char * Msg) noexcept
    {
        const _qword x = GetTime();
        if  (m_pos < MaxN)
        {
            m_Time[m_pos] = x;
            m_Msg[m_pos] = Msg;
            //m_Mem[m_pos] = 0;
            m_Mem[m_pos] = MaaGetMemoryUsage(); //CTOOGetMemoryAllocated();// / 1024;
            m_pos++;
        }
        return x;
    }
    CMaaString GetResult(const char * Header = "")
    {
        CMaaString Ret(Header);
        for (int i = 0; i < m_pos; i++)
        {
            //Ret.Format("%S%10,D us  %9,D Kb  %s\n", &Ret, m_Time[i], m_Mem[i], m_Msg[i]);
            Ret.Format("%S%3,D.%06D us  %14,D  %s\n", &Ret, m_Time[i] / 1000000, m_Time[i] % 1000000, m_Mem[i], m_Msg[i]);
        }
        return Ret;
    }
#undef MaxN
};

//#endif

inline void void_null_func() {}

// CMaaPtInPolly algo is from Habr comment
template<class T = double> int CMaaPtInPollyN(const T& x, const T& y, int N, const T* xx, const T* yy) noexcept
{
    if (N < 3)
    {
        return -1; // N < 3 error
    }
    bool in_polygon = false;
    for (int i = 0; i < N; i++)
    {
        T x0 = xx[i];
        T y0 = yy[i];
        T x1 = xx[i + 1 < N ? i + 1 : 0];
        T y1 = yy[i + 1 < N ? i + 1 : 0];
        if (((y0 <= y && y < y1) || (y1 <= y && y < y0)) && (x > (x1 - x0) * (y - y0) / (y1 - y0) + x0))
        {
            in_polygon = !in_polygon;
        }
    }
    return in_polygon;
}

template<class T = double> int CMaaPtInPolly2N(const T& x, const T& y, int N, const T* xy) noexcept
{
    if (N < 3)
    {
        return -1; // N < 3 error
    }
    bool in_polygon = false;
    for (int i = 0; i < N; i++)
    {
        T x0 = xy[2 * i];
        T y0 = xy[2 * i + 1];
        T x1 = xy[i + 1 < N ? 2 * (i + 1) : 0];
        T y1 = xy[i + 1 < N ? 2 * (i + 1) + 1: 1];
        if (((y0 <= y && y < y1) || (y1 <= y && y < y0)) && (x > (x1 - x0) * (y - y0) / (y1 - y0) + x0))
        {
            in_polygon = !in_polygon;
        }
    }
    return in_polygon;
}

#if 0
void CMaaPtInPolly_Test()
{
    const int N = 5;
    double p[2 * N] = { 0, 0, 100, 0, 100, 100, 50, 0, 0, 100 };
    const int M = 5;
    double m[2 * N] = { 0, 0, 200, 50, 200, 200, 30, 30, 50, 20 };

    double xx[N];
    double yy[N];

    for (int i = 0; i < N; i++)
    {
        xx[i] = p[2 * i];
        yy[i] = p[2 * i + 1];
        __utf8_printf("(%lf, %lf) ", xx[i], yy[i]);
    }
    __utf8_printf("\n");
    const char* msg[3] = { "N < 3 error", "out of polly", "in polly" };
    for (int i = 0; i < M; i++)
    {
        double x = m[2 * i], y = m[2 * i + 1];
        __utf8_printf("(%lf, %lf) - %s - %s\n", x, y, msg[1 + CMaaPtInPollyN(x, y, N, xx, yy)], msg[1 + CMaaPtInPolly2N(x, y, N, p)]);
    }
}
#endif

//--------------------------------------------------------------------

//#define DEBUG_CMaaMergeQSort
//#define DEBUG_CMaaMergeQSort_pw
//#define DEBUG_CMaaMergeQSort_cv

#ifdef DEBUG_CMaaMergeQSort
static void pr(int* m, int n, CMaaString& s, bool bPrint)
{
    for (int i = 0; i < n; i++)
    {
        s.Append("%d ", m[i]);
    }
    if (bPrint)
    {
        __utf8_printf("%S\n", &s);
    }
}
#endif

template<class T, class P> using pfCMaaMergeQSortCompare = int (*)(T a, T b, P& p) noexcept;

template<class T, class P> class CMaaMergeQSort
{
public:
#ifdef DEBUG_CMaaMergeQSort
    static constexpr int NN = 4;
#else
    static constexpr int NN = 64;
    //static int NN;// = 64;
#endif
#ifdef DEBUG_CMaaMergeQSort_pw
    std::atomic<_qword> pw1 = 0, pw2 = 0, pw3 = 0;
#endif
    struct Task : public CMaaDLink
    {
        char Type, Status, chResult;
        T* m1, * m2;
        int a, b, c;
        Task* pResult;
    };
protected:
    /*
    N = 50,000,000, NN = 64
    CMaaMergeQSort
     0.000022 us       3,907,584  alloc
     2.766620 us     204,296,192  gen rnd
     2.371516 us     604,389,376  CMaaMergeQSort
     5.321778 us     604,389,376  qsort
     0.009706 us     604,389,376  check
    = 55.4% faster (I7-7700K, Win10 x64) = 2.2440x

    CMaaMergeQSort
     0.000054 us     406,269,952  alloc
    40.428274 us     406,269,952  gen rnd
    13.116061 us   1,176,715,264  CMaaMergeQSort
    21.633860 us   1,176,715,264  qsort
     0.086219 us   1,176,715,264  check
    = 39.4% faster (linux vds x64) = 1.65x
   */
#ifdef TOOLSLIB_SINGLE_THREAD
    CMaaAtomicFastMutexST m_Mutex;
#else
    CMaaAtomicFastMutex m_Mutex; // fastest
#endif
    //CMaaFastMutex m_Mutex;     // slower
    //std::mutex m_Mutex;        // fast
#ifdef DEBUG_CMaaMergeQSort_cv
    std::mutex mM2, mM3;
    std::condition_variable cvM2, cvM3;
#endif
    T* m_M;
    CMaaPtr_<T, 1> m_M2;
    CMaaPtr_<Task, 1> m_Alloc;
    CMaaDList<Task> m_S, m_MmC, m_Free;
    CMaaPtr_<std::thread, 1> m_Threads;
    bool m_ExitFlag = false;
    bool m_bThreadsCreated = false;
    pfCMaaMergeQSortCompare<const T&, P> m_fn;
    P& m_p;

    Task* Alloc() noexcept
    {
        Task* p;
        //bool b1st = true;
        while (1)
        {
#ifndef DEBUG_CMaaMergeQSort_cv
            m_Mutex.lock();
#endif
#ifdef DEBUG_CMaaMergeQSort_cv
            std::unique_lock lk(mM3);
#endif
            p = m_Free.GetFromFront();
            if (p)
            {
#ifndef DEBUG_CMaaMergeQSort_cv
                m_Mutex.unlock();
#endif
                break;
            }
#ifndef DEBUG_CMaaMergeQSort_cv
            m_Mutex.unlock();
#endif
#ifdef DEBUG_CMaaMergeQSort_pw
            ++pw3;
#endif

#ifdef DEBUG_CMaaMergeQSort_cv
            {
                cvM3.wait(lk, [this]
                    {
                        //m_Mutex.lock();
                        //const bool b = b1st || /*m_fc > 0; */ m_Free.LookAtFront();
                        //m_Mutex.unlock();
                        //b1st = false;
                        const bool b = m_Free.LookAtFront(); //m_fc > 0; // free count > 0
                        return b;
                    });
            }
#else
            std::this_thread::yield();
#endif
        }
        return p;
    }
    void CreateThreads()
    {
        m_Mutex.lock();
        const bool b = m_bThreadsCreated;
        m_bThreadsCreated = true;
        m_Mutex.unlock();

        if (!b)
        {
            for (int i = 0; i < (int)m_Threads.MaxIndex(); i++)
            {
                //m_Threads[i] = std::move(std::thread(ThreadFn, this)); // static fn with arg
                m_Threads[i] = std::move(std::thread(&CMaaMergeQSort<T, P>::ThreadFn, this));
                //m_Mutex.lock();
                //++m_NT;
                //m_Mutex.unlock();
            }
        }
    }
#if 0
    static void ThreadFn(CMaaMergeQSort<T, P>* pThis) noexcept
    {
        pThis->Fn();
    }
#endif
    void ThreadFn() noexcept
    {
        Task* p = nullptr;
        while (1)
        {
            m_Mutex.lock();
            if (p)
            {
                if (p->pResult)
                {
                    if ((p->pResult->Status |= p->chResult) == 3)
                    {
                        CMaaDList<Task>::Release(p->pResult);
                        m_S.AddAtFront(p->pResult);
                    }
                }
                {
#ifdef DEBUG_CMaaMergeQSort_cv
                    mM3.lock();
#endif
                    //std::unique_lock lk(mM3);
                    m_Free.AddAtFront(p);
#ifdef DEBUG_CMaaMergeQSort_cv
                    if (m_Free.LookAtBack() == p)
                    {
                        mM3.unlock();
                        cvM3.notify_one();
                    }
                    else
                    {
                        mM3.unlock();
                    }
#endif
                }
            }
            p = m_S.GetFromFront();
            if (!p)
            {
                if (m_ExitFlag && !m_MmC.LookAtFront())
                {
                    m_Mutex.unlock();
                    break;
                }
                m_Mutex.unlock();
#ifdef DEBUG_CMaaMergeQSort_pw
                ++pw1;
#endif
#ifdef DEBUG_CMaaMergeQSort_cv
                {
                    std::unique_lock lk(mM2);
                    cvM2.wait(lk, [this]
                        {
                            m_Mutex.lock();
                            const Task* p = m_S.LookAtFront();
                            const bool b = p || m_ExitFlag;
                            m_Mutex.unlock();
                            return b;
                        });
                }
#else
                {
                    std::this_thread::yield();
                }
#endif
                continue;
            }
#ifdef DEBUG_CMaaMergeQSort
            CMaaString s;
            if (p)
            {
                if (p->Type == 'S' || p->Type == 'C')
                {
                    s.Format("%c(%d,%d) ", p->Type, p->a, p->b);
                }
                else
                {
                    s.Format("%c(%d,%d,%d) ", p->Type, p->a, p->b, p->c);
                }
                __utf8_printf("%S\n", &s);
                s.Empty();
            }
#endif
            m_Mutex.unlock();

            switch (p->Type)
            {
            case 'S':
            {
                PopSort(p->m1 + p->a, p->b - p->a);
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m1 + p->a, p->b - p->a, s, true);
#endif
                break;
            }
            case 'M':
            {
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m1 + p->a, p->b - p->a, s, false);
                s += "+(M)";
                pr(p->m1 + p->b, p->c - p->b, s, false);
                s += "=";
#endif
                Merge(p->m1, p->m2, p->a, p->b, p->c);
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m2 + p->a, p->c - p->a, s, true);
#endif
                break;
            }
            case 'm':
            {
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m1 + p->a, p->b - p->a, s, false);
                s += "+(m)";
                pr(p->m2 + p->b, p->c - p->b, s, false);
                s += " =";
#endif
                Merge_(p->m1, p->m2, p->a, p->b, p->c);
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m2 + p->a, p->c - p->a, s, true);
#endif
                break;
            }
            case 'C':
            {
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m1, p->b, s, false);
                s += "=";
#endif
                Copy(p->m1, p->m2, p->a, p->b);
#ifdef DEBUG_CMaaMergeQSort
                pr(p->m2, p->b, s, true);
#endif
                break;
            }
            }
            /*
            else
            {
                ++pw2;
                if (1)
                {
                    std::unique_lock lk(mM2);
                    cvM2.wait(lk, [this]
                        {
                            m_Mutex.lock();
                            const Task* p = m_S.LookAtBack();
                            const bool b = p || m_ExitFlag;
                            m_Mutex.unlock();
                            return b;
                        });
                }
                else
                {
                    std::this_thread::yield();
                }
            }
            */
        }
    }
    void PopSort(T* m, int N) noexcept
    {
        for (int i = 0; i < N - 1; i++)
        {
            for (int j = i + 1; j < N; j++)
            {
                if (m_fn(m[i], m[j], m_p) > 0)
                {
                    CMaaSwap(m[i], m[j]);
                }
            }
        }
    }
    void Merge(T* m, T* r, int a, int b, int c) noexcept
    {
        const int b0 = b;
        for (int i = a; i < c; i++)
        {
            if (a >= b0)
            {
                r[i] = m[b++];
            }
            else if (b >= c)
            {
                r[i] = m[a++];
            }
            else
            {
                if (m_fn(m[a], m[b], m_p) > 0)
                {
                    r[i] = m[b++];
                }
                else
                {
                    r[i] = m[a++];
                }
            }
        }
    }
    void Merge_(T* m, T* r, int a, int b, int c) noexcept
    {
        const int b0 = b;
        for (int i = a; i < c; i++)
        {
            if (a >= b0)
            {
                //r[i] = m[b++];
                return;
            }
            else if (b >= c)
            {
                r[i] = m[a++];
            }
            else
            {
                if (m_fn(m[a], r[b], m_p) > 0)
                {
                    r[i] = r[b++];
                }
                else
                {
                    r[i] = m[a++];
                }
            }
        }
    }
    void Copy(T* m, T* r, int a, int b) noexcept
    {
        if (m != r)
        {
            for (int i = a; i < b; i++)
            {
                r[i] = m[i];
            }
        }
    }
    void Sort1(T*& m1, T*& m2, int a, int c) noexcept
    {
        const int b = (a + c) / 2;
        if (b - a <= NN && c - b <= NN)
        {
            PopSort(m_M + a, b - a);
            PopSort(m_M + b, c - b);
            Merge(m1 = m_M, m2 = m_M2, a, b, c);
        }
        else
        {
            T* p1, * p2;

            Sort1(m2, m1, a, b);
            Sort1(p1, p2, b, c);
            if (p1 != m2)
            {
                Merge_(m1, m2, a, b, c);
            }
            else
            {
                Merge(m1, m2, a, b, c);
            }
        }
    }
    void CreateTasks(T*& m1, T*& m2, int a, int c, Task* ResultTask, char chRes) noexcept
    {
        const int b = (a + c) / 2;
        if (b - a <= NN && c - b <= NN)
        {
            Task* p = Alloc();
            p->Type = 'M';
            p->Status = 0;
            m1 = p->m1 = m_M;
            m2 = p->m2 = m_M2;
            p->a = a;
            p->b = b;
            p->c = c;
            p->chResult = chRes;
            p->pResult = ResultTask;

#ifdef DEBUG_CMaaMergeQSort
            __utf8_printf("M(%d,%d,%d)\n", p->a, p->b, p->c);
#endif

            Task* s1 = Alloc();
            s1->Type = 'S';
            s1->Status = 3;
            s1->m1 = m_M;
            //s1->m2 = nullptr;
            s1->a = a;
            s1->b = b;
            s1->chResult = 1;
            s1->pResult = p;
            //m_S.AddAtFront(s1);
#ifdef DEBUG_CMaaMergeQSort
            __utf8_printf("S(%d,%d)\n", s1->a, s1->b);
#endif
            Task* s2 = Alloc();
            s2->Type = 'S';
            s2->Status = 3;
            s2->m1 = m_M;
            //s2->m2 = nullptr;
            s2->a = b;
            s2->b = c;
            s2->chResult = 2;
            s2->pResult = p;
            //m_S.AddAtFront(s2);
#ifdef DEBUG_CMaaMergeQSort
            __utf8_printf("S(%d,%d)\n", s2->a, s2->b);
#endif
            //m_MmC.AddAtFront(p);
#ifdef DEBUG_CMaaMergeQSort
            //__utf8_printf("M(%d,%d,%d)\n", p->a, p->b, p->c);
#endif
            m_Mutex.lock();

            m_S.AddAtBack(s1);
            m_S.AddAtBack(s2);
            m_MmC.AddAtBack(p);

            m_Mutex.unlock();

#ifdef DEBUG_CMaaMergeQSort_cv
            cvM2.notify_all();
#endif
        }
        else
        {
            Task* p = Alloc();
            p->Type = 'M';
            p->Status = 4;
            p->a = a;
            p->b = b;
            p->c = c;
            p->chResult = chRes;
            p->pResult = ResultTask;

            CreateTasks(m1, m2, a, b, p, 1);
            p->m1 = m2;
            p->m2 = m1;
            CreateTasks(m1, m2, b, c, p, 2);
            if (m1 != p->m2)
            {
                p->Type = 'm';
            }
            m1 = p->m1;
            m2 = p->m2;

#ifdef DEBUG_CMaaMergeQSort_cv
            bool f = false;
#endif
            m_Mutex.lock();
            if ((p->Status &= ~4) == 3)
            {
                m_S.AddAtBack(p);
#ifdef DEBUG_CMaaMergeQSort_cv
                f = true;
#endif
            }
            else
            {
                m_MmC.AddAtBack(p);
            }
            m_Mutex.unlock();

#ifdef DEBUG_CMaaMergeQSort_cv
            if (f)
            {
                cvM2.notify_one();
            }
#endif
#ifdef DEBUG_CMaaMergeQSort
            __utf8_printf("%c%s%s(%d,%d,%d)\n", p->Type, p->m1 == m_M ? "1" : "2", p->m2 == m_M ? "1" : "2", p->a, p->b, p->c);
#endif
        }
    }
public:
    CMaaMergeQSort(T* M, int N, P& p, pfCMaaMergeQSortCompare<const T&, P> fn, int nThreads = -1, bool bSync = true)
    :   m_M(M),
        m_fn(fn),
        m_p(p)
    {
        if (N <= NN)
        {
            PopSort(M, N);
        }
        else if (
#ifdef TOOLSLIB_SINGLE_THREAD
            nThreads == -2 ||
#endif
            nThreads == 1)
        {
            m_M2.CheckResize(N);
            T* m1, * m2;
            Sort1(m2, m1, 0, N);
            Copy(m1, M, 0, N);
        }
        else
        {
            m_M2.CheckResize(N);
            m_Alloc.CheckResize((CMaaGetHardwareConcurrency() + (int)sizeof(size_t) * 8) * 3 + 10);
            m_Threads.CheckResize(!nThreads ? 2 : nThreads < 0 ? CMaaGetHardwareConcurrency() : nThreads);

            for (int i = (int)m_Alloc.MaxIndex(); i--; )
            {
                m_Free.AddAtFront(&m_Alloc[i]);
            }
            try
            {
                CreateThreads();

                Task* p = Alloc();
                p->Type = 'C';
                p->Status = 4;
                p->a = 0;
                p->b = N;
                p->chResult = 0;
                p->pResult = nullptr;

                CreateTasks(p->m2, p->m1, 0, N, p, 3);
                p->m2 = M;
                m_Mutex.lock();
                if ((p->Status &= ~4) == 3)
                {
                    m_S.AddAtBack(p);
                }
                else
                {
                    m_MmC.AddAtBack(p);
                }
                m_ExitFlag = true;
                m_Mutex.unlock();

#ifdef DEBUG_CMaaMergeQSort_cv
                cvM2.notify_all();
#endif

#ifdef DEBUG_CMaaMergeQSort
                __utf8_printf("%c%s%s(%d,%d)\n\n", p->Type, p->m1 == m_M ? "1" : "2", p->m2 == m_M ? "1" : "2", p->a, p->b);
#endif
                //CreateThreads();
            }
            catch (...)
            {
                m_Mutex.lock();
                m_ExitFlag = true;
                m_Mutex.unlock();

#ifdef DEBUG_CMaaMergeQSort_cv
                cvM2.notify_all();
#endif
                JoinThreads();
                throw;
            }
            if (bSync)
            {
                JoinThreads();
            }
        }
    }
    ~CMaaMergeQSort()
    {
        JoinThreads();
#ifdef DEBUG_CMaaMergeQSort_pw
        __utf8_printf("pw1=%,D, pw2=%,D, pw3=%,D\n", (_qword)pw1, (_qword)pw2, (_qword)pw3);
#endif
    }
    void JoinThreads()
    {
        m_Mutex.lock();
        const bool b = m_bThreadsCreated;
        m_bThreadsCreated = false;
        m_Mutex.unlock();

        if (b)
        {
            for (int i = 0; i < (int)m_Threads.MaxIndex(); i++)
            {
                if (m_Threads[i].joinable())
                {
                    m_Threads[i].join();
                }
            }
        }
    }
};

#if 0

//int CMaaMergeQSort<int, const void*>::NN = 6 * 32;

static int fnIntCompare(const int& a, const int& b, const void*& p) noexcept
{
    return a - b;
}

int qsort_int(const void* a, const void* b)
{
    return (int)(*(int*)a) - (int)(*(int*)b);
}

int CMaaMergeQSort_test()
{
    CHrtMultiTimer t;
#ifdef DEBUG_CMaaMergeQSort
    constexpr int N = 100; // 20
#else
    constexpr int N = 50000000;// for 100000: time as qsort
#endif
    __utf8_printf("N = %,D, NN = %d\n", (_qword)N, (int)CMaaMergeQSort<int, const void*>::NN);
    t.Start();
    CMaaPtr_<int, 1> m(N), q(N);
    t.Stop("alloc");
    t.Start();
    GetRnd(m, (int)m.Size());
    for (int i = 0; i < N; i++)
    {
        m[i] = (int)((unsigned)m[i] % (unsigned)N);
#ifdef DEBUG_CMaaMergeQSort
        __utf8_printf("%d ", m[i]);
#endif
    }
    t.Stop("gen rnd");
    memcpy(q, m, m.Size());

    __utf8_printf("\nGenerated. Sorting...\n\n");

    /*
    CMaaFile::Remove("CMaaMergeQSort.txt", false);
    for (int NN = 30; NN < 550; NN++)
    {
        CMaaMergeQSort<int, const void*>::NN = NN;
        __utf8_printf("NN = %d ", NN);

        _uqword t0 = MyGetTickCount64();
        {
            const void* p = nullptr;
            CMaaMergeQSort<int, const void*>(m, N, p, fnIntCompare);
        }
        t0 = MyGetTickCount64() - t0;
        __utf8_printf("%,D\n", t0);
        CMaaFile("CMaaMergeQSort.txt", "AC|SrSw", false).fprintf("%4d;%D,%06D\r\n", NN, t0 / 1000000, t0 % 1000000);
        memcpy(m, q, m.Size());
    }
    */

    t.Start();
    const void* p = nullptr;
    CMaaMergeQSort<int, const void*> s(m, N, p, fnIntCompare, -1, false); // -1, false
    __utf8_printf("Some parallel work...\n");
    s.JoinThreads();
    t.Stop("CMaaMergeQSort");

    __utf8_printf("\nqsort...\n");
    t.Start();
    qsort(q, N, sizeof(int), qsort_int);
    t.Stop("qsort");

#ifdef DEBUG_CMaaMergeQSort
    for (int i = 0; i < N; i++)
    {
        __utf8_printf("%d ", m[i]);
    }
#endif
    __utf8_printf("\nCheck...");
    t.Start();
    int nErr = 0;
    for (int i = 0; i < N - 1; i++)
    {
        if (m[i] > m[i + 1])
        {
            nErr++;
        }
    }
    t.Stop("check");
    __utf8_printf("\nnErr = %d\n", nErr);

    if (memcmp(m, q, m.Size()))
    {
        __utf8_printf("Error: CMaaMergeQSort != qsort\n");
    }

    CMaaString str = t.GetResult("CMaaMergeQSort\n");
    __utf8_printf("\n%S\n", &str);
    return 0;
}

#endif
