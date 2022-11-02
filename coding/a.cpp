#include <cstring>
#include <iostream>
#include <string>

#include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysql.h"
extern "C" {
char *to_char(UDF_INIT *initid, UDF_ARGS *args, char *result,
              unsigned long *length, char *is_null, char *error);
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void xxx_deinit(UDF_INIT *initid);
}
int is_string = 0, is_num = 0;
char letter[] = {'.', ',', 'd', 'm', 'i', 'p', 'r', 's', '$', '0', '9'};
// char *ans = (char*)malloc(256);
void stringlower(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') str[i] += 32;
    }
}
bool check_str(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        int letter_flag = 0;
        for (int j = 0; j < strlen(letter); j++) {
            if (str[i] == letter[j]) {
                letter_flag = 1;
                break;
            }
        }
        if (!letter_flag) return 0;
    }
    return 1;
}
void change1(char *str, char *ans) {
    if (is_string)
        strcpy(ans, str);
    else {
        int len = strlen(str);
        int flag = 0;
        char *pos_str = str;
        char *pos_ans = ans;
        char *pos_rstr = str + len - 1;
        for (int i = 0; i < len; i++)
            if (str[i] == '.') {
                flag = 1;
                break;
            }
        if (flag) {
            while (*pos_rstr == '.' || *pos_rstr == '0') {
                pos_rstr--;
            }
        }
        pos_rstr++;
        if (*pos_str == '-') {
            *pos_ans = *pos_str;
            pos_str++;
            pos_ans++;
        }
        while (*pos_str == '0') pos_str++;
        while (pos_str != pos_rstr) {
            *pos_ans = *pos_str;
            pos_str++;
            pos_ans++;
        }
        if (!strlen(ans)) strcpy(ans, "0");
    }
    return;
}
void change2(char *str, char *fmt, char *ans) {
    stringlower(fmt);
    if (!check_str(fmt))
        strcpy(ans, "the fmt is not correct");
    else
        strcpy(ans, "2");
    return;
}
char *to_char(UDF_INIT *initid, UDF_ARGS *args, char *result,
              unsigned long *length, char *is_null, char *error) {
    unsigned int num = args->arg_count;
    switch (num) {
        case 0: {
            strcpy(result, "no parameter");
            break;
        }
        case 1: {
            change1(args->args[0], result);
            //  strcpy(result,ans);
            break;
        }
        case 2: {
            change2(args->args[0], args->args[1], result);
            //  strcpy(result,ans);
            break;
        }
        default: {
            strcpy(result, "too much parameter");
            break;
        }
    }
    *length = strlen(result);
    is_string = 0;
    is_num = 0;
    return result;
}
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_type[0] == STRING_RESULT)
        is_string = 1;
    else if (args->arg_type[0] == INT_RESULT ||
             args->arg_type[1] == DECIMAL_RESULT)
        is_num = 1;
    args->arg_type[0] = STRING_RESULT;
    return 0;
}
