#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pcre.h>

#define PATT_BUF_SIZE 128

#define PCRE_INFO(re, extra, errno, what, ptr) errno = pcre_fullinfo(re, extra, what, &ptr);\
    if ((errno) < 0) {\
        print_pcre_fullinfo_error((errno));\
        exit(1);\
    }
#define PRINT_INFO_INT(re, extra, errno, what, ptr) PCRE_INFO(re, extra, errno, what, ptr)\
    printf("%s: %d\n", #what, ptr);
#define PRINT_INFO_CHAR(re, extra, errno, what, ptr) PCRE_INFO(re, extra, errno, what, ptr)\
    printf("%s: %s\n", #what, ptr);

#define PRINT_OPT(opt, OPT) if (opt & OPT) printf("%s\n", #OPT);

#define TO_STRING(x) _TO_STRING(x)
#define _TO_STRING(x) #x

void print_pcre_fullinfo_error(int errno) {
    if (errno == 0) {
        return;
    }

    switch (errno) {
        case PCRE_ERROR_NULL:
            printf("the argument code or where was NULL\n");
            break;
        case PCRE_ERROR_BADMAGIC:
            printf("the \"magic number\" was not found\n");
            break;
        case PCRE_ERROR_BADOPTION:
            printf("the value of what was invalid\n");
            break;
    }
}

void print_pcre_fullinfo(const pcre *re, pcre_extra *extra) {
    int errno;

    int int_val;
    unsigned char *uchar_val;
    int i, name_cnt, ne_size;
    char *name_tbl;
    unsigned int opt;

    PRINT_INFO_INT(re, extra, errno, PCRE_INFO_SIZE, int_val);
    PRINT_INFO_INT(re, extra, errno, PCRE_INFO_STUDYSIZE, int_val);

    printf("------\n");
    PRINT_INFO_INT(re, extra, errno, PCRE_INFO_CAPTURECOUNT, int_val);
    PRINT_INFO_INT(re, extra, errno, PCRE_INFO_BACKREFMAX, int_val);

    printf("------\n");
    PRINT_INFO_CHAR(re, extra, errno, PCRE_INFO_DEFAULT_TABLES, uchar_val);
    PRINT_INFO_CHAR(re, extra, errno, PCRE_INFO_FIRSTTABLE, uchar_val);

    printf("------\n");
    PCRE_INFO(re, extra, errno, PCRE_INFO_FIRSTBYTE, uchar_val);
    printf("PCRE_INFO_FIRSTBYTE: %hhu(%#hhx)", uchar_val, uchar_val);
    PCRE_INFO(re, extra, errno, PCRE_INFO_LASTLITERAL, int_val);
    printf(", PCRE_INFO_LASTLITERAL: %d(%#x)\n", int_val, int_val);

    printf("------\n");
    PCRE_INFO(re, extra, errno, PCRE_INFO_NAMECOUNT, name_cnt);
    if (name_cnt) {
        PCRE_INFO(re, extra, errno, PCRE_INFO_NAMEENTRYSIZE, ne_size);
        PCRE_INFO(re, extra, errno, PCRE_INFO_NAMETABLE, name_tbl);

        printf("PCRE_INFO_NAMECOUNT: %d\n", name_cnt);
        for (i = 0; i < name_cnt; i++) {
            printf("%hhu %hhu %s\n", name_tbl, name_tbl + 1, name_tbl + 2);
            name_tbl += ne_size;
        }
    }

    PCRE_INFO(re, extra, errno, PCRE_INFO_OPTIONS, opt);
    if (opt) {
        printf("------\n");

        PRINT_OPT(opt, PCRE_ANCHORED);
        PRINT_OPT(opt, PCRE_AUTO_CALLOUT);
        PRINT_OPT(opt, PCRE_CASELESS);
        PRINT_OPT(opt, PCRE_DOLLAR_ENDONLY);
        PRINT_OPT(opt, PCRE_DOTALL);
        PRINT_OPT(opt, PCRE_EXTENDED);
        PRINT_OPT(opt, PCRE_EXTRA);
        PRINT_OPT(opt, PCRE_FIRSTLINE);
        PRINT_OPT(opt, PCRE_MULTILINE);
        PRINT_OPT(opt, PCRE_NO_AUTO_CAPTURE);
        PRINT_OPT(opt, PCRE_UNGREEDY);
        PRINT_OPT(opt, PCRE_UTF8);
        PRINT_OPT(opt, PCRE_NO_UTF8_CHECK);
    }
}

int main(int argc, char **argv) {
    int fd, read_len;
    char patt[PATT_BUF_SIZE];

    const pcre *re;
    const char *errptr;
    int erroffset;


    if (argc < 2) {
        printf("Usage: %s <pattern file path>\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open patt file failure");
        exit(1);
    }

    read_len = read(fd, patt, PATT_BUF_SIZE);
    if (read_len < 0) {
        perror("read patt file failure");
        exit(1);
    }

    if (patt[read_len - 1] == '\n') {
        patt[read_len - 1] = '\0';
    } else {
        patt[read_len] = '\0';
    }
    printf("Pattern: %s\n", patt);
    printf("------\n");

    re = pcre_compile(patt, PCRE_ANCHORED|PCRE_DOTALL, &errptr, &erroffset, NULL);
    print_pcre_fullinfo(re, NULL);

    return 0;
}
