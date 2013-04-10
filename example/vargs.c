#include <stdio.h>
#include <stdarg.h>

#define _TO_STRING(m) #m
#define TO_STRING(m) _TO_STRING(m)

#define KWDT_LOG(level, format, args...) kwdt_log(level, #level, format, ## args)
#define KWDT_LOG2(level, format, args...) kwdt_log(level, #level, format, place, ##args)
#define KWDT_LOG3(level, format, args...) kwdt_log(level, #level, format##args)

typedef enum _kwdt_log_level {
    KWDT_LOG_ERROR = 1,
    KWDT_LOG_INFO,
    KWDT_LOG_DEBUG
} KWDT_LOG_LEVEL_t;

void kwdt_log(KWDT_LOG_LEVEL_t level, char *level_str, char *format, ...) {
    char buf[512];
    va_list args;
    int len;

    va_start(args, format);
    len = vsnprintf(buf, 512, format, args);
    va_end(args);

    fprintf(stdout, "%s\n", buf);
}

int main(void) {
    printf("%s\n", TO_STRING(KWDT_LOG(KWDT_LOG_INFO, "format")));
    printf("%s\n", TO_STRING(KWDT_LOG(KWDT_LOG_INFO, "format", "holdplace")));
    printf("%s\n", TO_STRING(KWDT_LOG2(KWDT_LOG_INFO, "format")));
    printf("%s\n", TO_STRING(KWDT_LOG2(KWDT_LOG_INFO, "format", "holdplace")));
    printf("%s\n", TO_STRING(KWDT_LOG3(KWDT_LOG_INFO, format, holdplace)));
    KWDT_LOG(KWDT_LOG_ERROR, "output: %d, %s", 1, "blabla");
}
