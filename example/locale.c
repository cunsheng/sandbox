#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>

#define PRINT_LCONV_STR(l, member) printf("%s: %s\n", #member, l->member)
#define PRINT_LCONV_CHAR(l, member) printf("%s: %c\n", #member, l->member)
#define PRINT_SETLOCALE(category) printf("%s: %s\n", #category, setlocale(category, NULL))
#define PRINT_NL_LANGINFO(item) printf("%s: %s\n", #item, nl_langinfo(item))

void print_lconv() {
    struct lconv *l;
    l = localeconv();

    printf("localeconv:\n");
    PRINT_LCONV_STR(l, decimal_point);
    PRINT_LCONV_STR(l, thousands_sep);
    PRINT_LCONV_STR(l, grouping);
    PRINT_LCONV_STR(l, int_curr_symbol);
    PRINT_LCONV_STR(l, currency_symbol);
    PRINT_LCONV_STR(l, mon_decimal_point);
    PRINT_LCONV_STR(l, mon_thousands_sep);
    PRINT_LCONV_STR(l, mon_grouping);
    PRINT_LCONV_STR(l, positive_sign);
    PRINT_LCONV_STR(l, negative_sign);

    PRINT_LCONV_CHAR(l, int_frac_digits);
    PRINT_LCONV_CHAR(l, frac_digits);
    PRINT_LCONV_CHAR(l, p_cs_precedes);
    PRINT_LCONV_CHAR(l, n_cs_precedes);
    PRINT_LCONV_CHAR(l, p_sep_by_space);
    PRINT_LCONV_CHAR(l, n_sep_by_space);
    PRINT_LCONV_CHAR(l, p_sign_posn);
    PRINT_LCONV_CHAR(l, n_sign_posn);
}

void print_setlocale() {
    printf("setlocale:\n");
    PRINT_SETLOCALE(LC_ALL);
    PRINT_SETLOCALE(LC_COLLATE);
    PRINT_SETLOCALE(LC_CTYPE);
    PRINT_SETLOCALE(LC_MESSAGES);
    PRINT_SETLOCALE(LC_MONETARY);
    PRINT_SETLOCALE(LC_NUMERIC);
    PRINT_SETLOCALE(LC_TIME);
}

void print_time_format() {
    time_t raw;
    struct tm *tminfo;
    char buf[80];

    time(&raw);
    tminfo = localtime(&raw);

    strftime(buf, 80, "%c", tminfo);
    printf("Date is: %s\n", buf);
}

void print_nl_langinfo() {
    printf("nl_langinfo:\n");

    PRINT_NL_LANGINFO(CODESET);
    PRINT_NL_LANGINFO(D_T_FMT);
    PRINT_NL_LANGINFO(D_FMT);
    PRINT_NL_LANGINFO(T_FMT);

    PRINT_NL_LANGINFO(DAY_1);
    PRINT_NL_LANGINFO(DAY_2);
    PRINT_NL_LANGINFO(DAY_3);
    PRINT_NL_LANGINFO(DAY_4);
    PRINT_NL_LANGINFO(DAY_5);
    PRINT_NL_LANGINFO(DAY_6);
    PRINT_NL_LANGINFO(DAY_7);

    PRINT_NL_LANGINFO(ABDAY_1);
    PRINT_NL_LANGINFO(ABDAY_2);
    PRINT_NL_LANGINFO(ABDAY_3);
    PRINT_NL_LANGINFO(ABDAY_4);
    PRINT_NL_LANGINFO(ABDAY_5);
    PRINT_NL_LANGINFO(ABDAY_6);
    PRINT_NL_LANGINFO(ABDAY_7);

    PRINT_NL_LANGINFO(MON_1);
    PRINT_NL_LANGINFO(MON_2);
    PRINT_NL_LANGINFO(MON_3);
    PRINT_NL_LANGINFO(MON_4);
    PRINT_NL_LANGINFO(MON_5);
    PRINT_NL_LANGINFO(MON_6);
    PRINT_NL_LANGINFO(MON_7);
    PRINT_NL_LANGINFO(MON_8);
    PRINT_NL_LANGINFO(MON_9);
    PRINT_NL_LANGINFO(MON_10);
    PRINT_NL_LANGINFO(MON_11);
    PRINT_NL_LANGINFO(MON_12);

    PRINT_NL_LANGINFO(RADIXCHAR);
    PRINT_NL_LANGINFO(THOUSEP);
    PRINT_NL_LANGINFO(YESEXPR);
    PRINT_NL_LANGINFO(NOEXPR);
    PRINT_NL_LANGINFO(CRNCYSTR);
}

int main(void) {
    printf("################\n");
    printf("#### current system setting  ####\n");
    printf("################\n");
    print_lconv();
    printf("----------------\n");
    print_setlocale();
    printf("----------------\n");
    print_time_format();
    printf("----------------\n");
    print_nl_langinfo();

    setlocale(LC_ALL, "zh_CN.UTF8");

    printf("################\n");
    printf("#### setlocale to zh_CN.UTF8 ####\n");
    printf("################\n");
    print_lconv();
    printf("----------------\n");
    print_setlocale();
    printf("----------------\n");
    print_time_format();
    printf("----------------\n");
    print_nl_langinfo();

    setlocale(LC_ALL, "");

    printf("################\n");
    printf("#### setlocale to empty ####\n");
    printf("################\n");
    print_lconv();
    printf("----------------\n");
    print_setlocale();
    printf("----------------\n");
    print_time_format();
    printf("----------------\n");
    print_nl_langinfo();
}
