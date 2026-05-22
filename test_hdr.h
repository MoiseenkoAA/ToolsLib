static void f_test1111()
{
    {
        stat_print();
#define TEST_CS_STR_A "abcdefg" "NX"
        static const CMaaConstStr cs_a = TEST_CS_STR_A;
        CMaaString a = cs_a, b = TEST_CS_STR_A;
        __utf8_printf("%S, %08lw %016llw\n%S, %08lw %016llw\n", &a, a.Hash(), a.Hash64(), &b, b.Hash(), b.Hash64());
        stat_print();
    }
}
