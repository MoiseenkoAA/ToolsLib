
// First version of heap.

template <class T> class CHeap
{
    int N;
    T * l[64];
    int pos_l, pos_i;
public:
    CHeap()
    {
        memset(l, 0, sizeof(l));
        N = 0;
        pos_l = pos_i = 0;
    }
    ~CHeap()
    {
        for (int i = 63; i--;)
        {
            delete [] l[i];
        }
    }
    /*
     0 - [0][0]
     1 - [1][0]
     2 - [1][1]
     3 - [2][0]
     */
    void Left(int &l, int &i)
    {
        l++;
        i *= 2;
    }
    void Right(int &l, int &i)
    {
        l++;
        i = 2 * i + 1;
    }
    void Up(int &l, int &i)
    {
        l--;
        i /= 2;
    }
    bool Add(const T & x)
    {
        if  (!l[pos_l])
        {
            l[pos_l] = new T[1 << pos_l];
            if  (!l[pos_l])
            {
                throw "new [] error";
            }
        }
        l[pos_l][pos_i] = x;
        int a = pos_l, b = pos_i;
        while(a > 0 && x < l[a - 1][b / 2])
        {
            CMaaSwap(l[a][b], l[a - 1][b / 2]);
            a--;
            b /= 2;
        }
        if  (++pos_i >= (1 << pos_l))
        {
            pos_l++;
            pos_i = 0;
        }
        Check();
        return true;
    }
    bool Del(T & x)
    {
        if  (pos_l == 0 && pos_i == 0)
        {
            return false;
        }
        if  (--pos_i < 0)
        {
            pos_l--;
            pos_i = (1 << pos_l) - 1;
        }
        x = l[0][0];
        l[0][0] = l[pos_l][pos_i];
        int a = 0, b = 0;
        while(a + 1 <= pos_l)
        {
            if  (HaveLeft(a, b) && HaveRight(a, b) && (l[a + 1][b * 2] < l[a][b] || l[a + 1][b * 2 + 1] < l[a][b]))
            {
                if  (l[a + 1][b * 2] < l[a + 1][b * 2 + 1])
                {
                    CMaaSwap(l[a][b], l[a + 1][b * 2]);
                    ++a, b *= 2;
                }
                else
                {
                    CMaaSwap(l[a][b], l[a + 1][b * 2 + 1]);
                    ++a, b = b * 2 + 1;
                }
            }
            else if (HaveLeft(a, b) && l[a + 1][b * 2] < l[a][b])
            {
                CMaaSwap(l[a][b], l[a + 1][b * 2]);
                ++a, b *= 2;
            }
            else
            {
                break;
            }
        }
        Check();
        return true;
    }
    bool Exists(int a, int b)
    {
        return (a < pos_l || (a == pos_l && b < pos_i));
    }
    bool HaveLeft(int a, int b)
    {
        Left(a, b);
        return Exists(a, b);
    }
    bool HaveRight(int a, int b)
    {
        Right(a, b);
        return Exists(a, b);
    }
    bool Del(int pl, int pi, T &x)
    {
        if  (pl < 0 || pi < 0 || pi >= (1 << pl) || pl > pos_l || (pl == pos_l && pi >= pos_i))
        {
            return false;
        }
        x = l[pl][pi];

        if  (--pos_i < 0)
        {
            pos_l--;
            pos_i = (1 << pos_l) - 1;
        }

        l[pl][pi] = l[pos_l][pos_i];
        int a = pl, b = pi;
        while(a > 0 && l[a][b] < l[a - 1][b / 2])
        {
            CMaaSwap(l[a][b], l[a - 1][b / 2]);
            a--;
            b /= 2;
        }
        while(a + 1 <= pos_l)
        {
            if  (HaveLeft(a, b) && HaveRight(a, b) && (l[a + 1][b * 2] < l[a][b] || l[a + 1][b * 2 + 1] < l[a][b]))
            {
                if  (l[a + 1][b * 2] < l[a + 1][b * 2 + 1])
                {
                    CMaaSwap(l[a][b], l[a + 1][b * 2]);
                    ++a, b *= 2;
                }
                else
                {
                    CMaaSwap(l[a][b], l[a + 1][b * 2 + 1]);
                    ++a, b = b * 2 + 1;
                }
            }
            else if (HaveLeft(a, b) && l[a + 1][b * 2] < l[a][b])
            {
                CMaaSwap(l[a][b], l[a + 1][b * 2]);
                ++a, b *= 2;
            }
            else
            {
                break;
            }
        }
        Check();
        return true;
    }
    void Print()
    {
        printf("\n");
        for (int a = 0; a <= (pos_i ? pos_l : pos_l - 1); a++)
        {
            for (int b = 0; b < (a == pos_l ? pos_i : (1 << a)); b++)
            {
                if  (b)
                {
                    printf("\t");
                }
                printf("%d", l[a][b]);
            }
            printf("\n");
        }
    }
    bool Check()
    {
        for (int a = 1; a <= (pos_i ? pos_l : pos_l - 1); a++)
        {
            for (int b = 0; b < (a == pos_l ? pos_i : (1 << a)); b++)
            {
                if  (l[a - 1][b / 2] <= l[a][b])
                {
                    //ok
                }
                else
                {
                    printf("Checked error: [%d][%d]\n", a, b);
                    return false;
                }
            }
        }
        return true;
    }
};
