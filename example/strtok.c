#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT "a/bbb///cc;xxx:yyy:"

int main(int argc, char *argv[]) {
    char *str1, *str2;
    char *tok1, *tok2;
    char *token, *subtoken;
    char *saveptr1 = NULL, *saveptr2;
    int j;

    str1 = malloc(strlen(TEXT));
    memcpy(str1, TEXT, strlen(TEXT));
    tok1 = ":;";
    tok2 = "/";

    for (j = 1; ; j++, str1 = NULL) {
        token = strtok_r(str1, tok1, &saveptr1);
        if (token == NULL) {
            break;
        }
        printf("%d: %s\n", j, token);

        for (str2 = token; ; str2 = NULL) {
            subtoken = strtok_r(str2, tok2, &saveptr2);
            if (subtoken == NULL) {
                break;
            }
            printf(" --> %s\n", subtoken);
        }
    }

    exit(EXIT_SUCCESS);
}
