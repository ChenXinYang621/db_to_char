#include <math.h>
#include <mysql.h>
#include <mysql_com.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>
using namespace std;

extern "C" {
double to_number(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
                 unsigned char *);
bool to_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
}

bool to_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 2 && args->arg_count != 1) {
        strcpy(message,
               "wrong number of arguments: to_number() requires one or two "
               "arguments");
        return true;
    }
    if (args->arg_count == 1) {
        const char *str = args->args[0];
        int cnt = 0;
        while (*str == ' ')  //允许第一个参数前有空格
        {
            str++;
        }
        if (*str != '.' && !(*str >= '0' && *str <= '9') && *str != '+' &&
            *str != '-') {
            strcpy(message, "ERROR");
            return true;
        } else {
            if (*str == '.') cnt++;
            if (!(*str >= '0' && *str <= '9')) str++;
        }
        while (*str) {
            if (*str != ',' && *str != '.' && !(*str >= '0' && *str <= '9')) {
                strcpy(message, "ERROR");
                return true;
            }
            if (*str == '.') {
                cnt++;
                if (cnt == 2) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }
            if (*str == ',' && cnt != 0) {
                strcpy(message, "ERROR");
                return true;
            }
            str++;
        }
    }
    if (args->arg_count == 2) {
        const char *str0 = args->args[0];
        const char *str1 = args->args[1];
        int cnt1 = 0, cnt2 = 0;
        //报错处理
        while (*str0 == ' ')  //允许第一个参数前有空格
        {
            str0++;
        }
        if (*str0 != '.' && !(*str0 >= '0' && *str0 <= '9') && *str0 != '+' &&
            *str0 != '-') {
            strcpy(message, "ERROR");
            return true;
        } else {
            if (*str0 == '.') cnt1++;
            if (!(*str0 >= '0' && *str0 <= '9')) str0++;
        }
        if (*str1 != '.' && *str1 != '9' && *str1 != '+' && *str1 != '-' &&
            *str1 != '0' && *str1 != 'D') {
            strcpy(message, "ERROR");
            return true;
        } else {
            if (*str1 == '.' || *str1 == 'D') cnt2++;
            if (*str1 != '9') str1++;
        }
        while (*str0 && *str1) {
            if (*str0 != ',' && *str0 != '.' &&
                !(*str0 >= '0' && *str0 <= '9')) {
                strcpy(message, "ERROR");
                return true;
            }
            if (*str0 == '.') {
                cnt1++;
                if (cnt1 == 2) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }
            if (*str0 == ',' && cnt1 != 0) {
                strcpy(message, "ERROR");
                return true;
            }
            if (*str1 != '.' && *str1 != '9' && *str1 != 'D' && *str1 != 'G' &&
                *str1 != ',' && *str1 != '0') {
                strcpy(message, "ERROR");
                return true;
            }
            if (*str1 == '.' || *str1 == 'D') {
                cnt2++;
                if (cnt2 == 2) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }
            if ((*str1 == ',' || *str1 == 'G') && cnt2 != 0) {
                strcpy(message, "ERROR");
                return true;
            }
            if (*str1 == '0' && (str1 + 1) != nullptr) {
                strcpy(message, "ERROR");
                return true;
            }
            if (*str0 >= '0' && *str0 <= '9' && *str1 != '9' && cnt1 == 0) {
                strcpy(message, "ERROR");
                return true;
            }
            str1++;
            str0++;
        }
        if (str0 != nullptr && str1 == nullptr) {
            strcpy(message, "ERROR");
            return true;
        }
        if (str1 != nullptr && str0 == nullptr) {
            if (!(*str1 == '0' && (str1 + 1) == nullptr)) {
                strcpy(message, "ERROR");
                return true;
            }
        }
    }
    return false;
}

double to_number(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
                 unsigned char *) {
    if (!args->args[0]) {
        *is_null = 1;
        return 0.0;
    }
    // has one parse
    if (args->arg_count == 1 || args->arg_count == 2) {
        const char *str = args->args[0];
        double sum = 0, fac = 10.0;
        int negflag = 0;

        while (*str == ' ') {
            str++;
        }

        //判断是否为负数，是就指针往前走
        if (*str == '-') {
            //为负数
            negflag = 1;
            str++;
        }

        //判断是否为正数，是就指针往前走
        if (*str == '+') {
            negflag = 0;
            str++;
        }

        //把数字以前的0走完
        while (*str == '0') {
            str++;
        }
        while (*str) {
            //判断是否为','
            if (*str == ',') {
                str++;
                continue;
            }
            //退出循环，进行小数处理
            if (*str == '.') {
                str++;
                break;
            }
            if (*str >= '0' && *str <= '9') {
                sum = sum * 10 + *str - '0';
                str++;
            }
        }

        while (*str) {
            if (*str >= '0' && *str <= '9') {
                sum = sum + (*str - '0') / fac;
                fac = fac * 10;
            } else {
                break;
            }
            str++;
        }

        if (negflag == 1) {
            sum = 0 - sum;
        }
        *is_null = 0;
        return sum;
    }

    return 0.0;
}