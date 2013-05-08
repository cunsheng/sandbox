#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pcre.h>
#include <string.h>
#include <sys/time.h>

#define PATT_BUF_SIZE 128
#define OVECTOR_SIZE 32
#define SUBJECT_BUF_SIZE 128

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
#define PRINT_CONFIG(ITEM, val) pcre_config(ITEM, &val); printf("%s: %d\n", #ITEM, val)
#define PRINT_ERROR(val, ERROR) if (val == ERROR) printf("%s\n", #ERROR)

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

void print_pcre_config() {
    int val;

    PRINT_CONFIG(PCRE_CONFIG_UTF8, val);
    PRINT_CONFIG(PCRE_CONFIG_UNICODE_PROPERTIES, val);
    PRINT_CONFIG(PCRE_CONFIG_NEWLINE, val);
    PRINT_CONFIG(PCRE_CONFIG_LINK_SIZE, val);
    PRINT_CONFIG(PCRE_CONFIG_POSIX_MALLOC_THRESHOLD, val);
    PRINT_CONFIG(PCRE_CONFIG_MATCH_LIMIT, val);
    PRINT_CONFIG(PCRE_CONFIG_MATCH_LIMIT_RECURSION, val);
    PRINT_CONFIG(PCRE_CONFIG_STACKRECURSE, val);
}

double get_second() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec*1e-6;
}

int main(int argc, char **argv) {
    int fd, read_len;
    char patt[PATT_BUF_SIZE];

    const pcre *re;
    int error;
    const char *errptr;
    int erroffset;

    char subject[SUBJECT_BUF_SIZE];
    int rc, ovector[OVECTOR_SIZE], i;
    char cap[SUBJECT_BUF_SIZE];
    char *cap_ptr, **cap_array;
    int cap_len;

    double ts;

    pcre_extra *extra;

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

    printf("Version: %s\n", pcre_version());
    printf("------\n");

    print_pcre_config();
    printf("------\n");

    re = pcre_compile2(patt, PCRE_ANCHORED|PCRE_DOTALL, &error, &errptr, &erroffset, NULL);
    if (!re) {
        printf("pattern compile failure, error code: %d\n", error);
        exit(1);
    }

    printf("Input text(Max 127): ");
    scanf("%127s", subject);
    printf("------\n");

    rc = pcre_exec(re, NULL, subject, strlen(subject), 0, 0, ovector, OVECTOR_SIZE);
    print_pcre_fullinfo(re, NULL);
    printf("------\n");
    if (rc < 1) {
        PRINT_ERROR(rc, PCRE_ERROR_NOMATCH);
        PRINT_ERROR(rc, PCRE_ERROR_NULL);
        PRINT_ERROR(rc, PCRE_ERROR_BADOPTION);
        PRINT_ERROR(rc, PCRE_ERROR_BADMAGIC);
        PRINT_ERROR(rc, PCRE_ERROR_UNKNOWN_NODE);
        PRINT_ERROR(rc, PCRE_ERROR_NOMEMORY);
        PRINT_ERROR(rc, PCRE_ERROR_NOSUBSTRING);
        PRINT_ERROR(rc, PCRE_ERROR_MATCHLIMIT);
        PRINT_ERROR(rc, PCRE_ERROR_RECURSIONLIMIT);
        PRINT_ERROR(rc, PCRE_ERROR_CALLOUT);
        PRINT_ERROR(rc, PCRE_ERROR_BADUTF8);
        PRINT_ERROR(rc, PCRE_ERROR_BADUTF8_OFFSET);
        PRINT_ERROR(rc, PCRE_ERROR_PARTIAL);
        PRINT_ERROR(rc, PCRE_ERROR_BADPARTIAL);
        PRINT_ERROR(rc, PCRE_ERROR_INTERNAL);
        PRINT_ERROR(rc, PCRE_ERROR_BADCOUNT);
        PRINT_ERROR(rc, PCRE_ERROR_PARTIAL);
        exit(0);
    }

    for (i = 0; i < rc; i++) {
        cap_len = ovector[i * 2 + 1] - ovector[i * 2];
        memcpy(cap, &subject[ovector[i * 2]], cap_len);
        cap[cap_len] = '\0';
        printf("%d, %s\n", i + 1, cap);
    }
    printf("------\n");

    // test performance
    ts = get_second();
    for (i = 0; i < 100000; i++) {
        rc = pcre_exec(re, NULL, subject, strlen(subject), 0, 0, ovector, OVECTOR_SIZE);
    }
    printf("%f\n", get_second() - ts);
    printf("------\n");

    // compare study with no study
    extra = pcre_study(re, 0, &errptr);
    if (extra == NULL ) {
        printf("no improve after study\n");
    } else {
        if (errptr == NULL) {
            printf("study failure, %s\n", errptr);
            exit(1);
        }

        ts = get_second();
        for (i = 0; i < 100000; i++) {
            rc = pcre_exec(re, extra, subject, strlen(subject), 0, 0, ovector, OVECTOR_SIZE);
        }
        printf("%f\n", get_second() - ts);
    }
    printf("------\n");

    // extract string with pcre func
    printf("by pcre_copy_substring\n");
    for (i = 0; i < rc; i++) {
        cap_len = pcre_copy_substring(subject, ovector, rc, i, cap, SUBJECT_BUF_SIZE);
        if (cap_len < 0) {
            PRINT_ERROR(cap_len, PCRE_ERROR_NOMEMORY);
            PRINT_ERROR(cap_len, PCRE_ERROR_NOSUBSTRING);
            continue;
        }
        printf("%d, %s(%d)\n", i + 1, cap, cap_len);
    }
    printf("------\n");

    printf("by pcre_get_substring\n");
    for (i = 0; i < rc; i++) {
        cap_len = pcre_get_substring(subject, ovector, rc, i, (const char **)&cap_ptr);
        if (cap_len < 0) {
            PRINT_ERROR(cap_len, PCRE_ERROR_NOMEMORY);
            PRINT_ERROR(cap_len, PCRE_ERROR_NOSUBSTRING);
        }
        printf("%d, %s(%d)\n", i + 1, cap_ptr, cap_len);
    }
    printf("------\n");

    printf("by pcre_get_substring_list\n");
    pcre_get_substring_list(subject, ovector, rc, (const char ***)&cap_array);
    for (i = 0; i < rc; i++) {
        printf("%d, %s\n", i + 1, cap_array[i]);
    }
    

    return 0;
}
