// 在 Path 中引入 MySQL 相关库
#include <mysql.h>

#include <algorithm>
#include <cstring>
#include <string>
using namespace std;

// MySQL UDF 必须使用 C/C++ 实现
// 可以通过 UDF 扩充 MySQL 的功能，加入一个新的 SQL 函数类似于内置的函数
extern "C" {
// 主函数，char * 对应 SQL STRING
char *to_char(UDF_INIT *initid, UDF_ARGS *args, char *result,
              unsigned long *length, char *is_null, char *error);
// 初始化函数
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
}

// 整数部分的位数
int number_int = 0;
// 小数部分的位数
int number_double = 0;
// 判断正数情况
bool positive = true;

// 判断为数字
bool is_number(const char *str) { return *str >= '0' && *str <= '9'; }

int is_num = 0;
int is_string = 0;
int args_count = 0;

// 初始化函数作为参数类型和内容的判断
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    // // 限制参数为 1 或 2 个
    // // 参数为 1 个的情况
    // if (args->arg_count == 1) {
    //     // 能使用的类型有 string, double, decimal
    //     if (*args->arg_type == Item_result::STRING_RESULT) {
    //         return false;
    //     }
    //     const char *str = args->args[0];
    //     string temp = str;

    //     // 小数点位
    //     int cnt = count(temp.begin(), temp.end(), '.');
    //     // 判断第一个字符
    //     if (!is_number(str) && *str != '.') {
    //         if (*str == '-') {
    //             positive = false;
    //             true_number.append("-");
    //         }
    //         str++;
    //         temp = str;
    //     }
    //     int first, end = 0;
    //     int length = temp.length();
    //     for (first = 0; first < length; first++) {
    //         if (temp[first] != 0 || temp[first] == '.') {
    //             break;
    //         }
    //     }
    //     // 表示没有对应数字，结果为 0
    //     if (first == length) {
    //         true_number = "0";
    //     }
    //     if (cnt == 1) {
    //     }
    //     while (*str) {
    //     }

    //     if (cnt == 0) {
    //     } else {
    //     }

    //     // 判断剩下的数字部分
    //     while (*str) {
    //         if (*str == '.') {
    //             cnt = 1;
    //         } else {
    //             if (cnt == 0) {
    //                 number_int++;
    //             } else {
    //                 number_double++;
    //             }
    //         }
    //     }
    //     args->arg_type[0] = Item_result::STRING_RESULT;
    //     return false;
    // }

    // 参数为 2 个的情况
    // if (args->arg_count == 2) {
    //     // 第一个参数为 int/double 类型，第二个参数为 string 类型
    //     if ((*args->arg_type != Item_result::INT_RESULT &&
    //          *args->arg_type != Item_result::REAL_RESULT) ||
    //         *(args->arg_type + 1) != Item_result::STRING_RESULT) {
    //         strcpy(message, "ERROR: Wrong Type");
    //         return true;
    //     }

    //     const char *str0 = args->args[0];
    //     const char *str1 = args->args[1];
    //     string temp0 = str0, temp1 = str1;

    //     // 判断格式化输出格式是否符合标准
    //     // temp1 转为小写，方便进行判断
    //     transform(temp1.begin(), temp1.end(), temp1.begin(), ::tolower);
    //     int cnt_dollar = count(temp1.begin(), temp1.end(), "$");
    //     int cnt_pr = count(temp1.begin(), temp1.end(), "pr");
    //     int cnt_s = count(temp1.begin(), temp1.end(), "s");
    //     int cnt_mi = count(temp1.begin(), temp1.end(), "mi");
    //     int cnt_d = count(temp1.begin(), temp1.end(), "d");
    //     int cnt_dot = count(temp1.begin(), temp1.end(), ".");
    //     int cnt_0 = count(temp1.begin(), temp1.end(), "0");
    //     int cnt_9 = count(temp1.begin(), temp1.end(), "9");

    //     int length1 = strlen(str1);
    //     int i = 0;

    //     // 各种限制条件的判断
    //     if (cnt_s > 1 || cnt_mi > 1 || cnt_pr > 1 || cnt_dollar > 1 ||
    //         cnt_d > 1 || cnt_dot > 1) {
    //         strcpy(message, "ERROR");
    //         return true;
    //     }
    //     if (cnt_dot == cnt_d == 1) {
    //         strcpy(message, "ERROR");
    //         return true;
    //     }
    //     if ((cnt_s == cnt_mi == 1) || (cnt_s == cnt_pr == 1) ||
    //         (cnt_mi == cnt_pr == 1)) {
    //         strcpy(message, "ERROR");
    //         return true;
    //     }

    //     if (cnt_s == 1) {
    //     }

    //     if (*args->arg_type == Item_result::INT_RESULT) {
    //         // 判断第一个字符
    //         if (!is_number(str0) && *str0 != '+' && *str0 != '-') {
    //             strcpy(message, "ERROR");
    //             return true;
    //         }
    //         str0++;
    //         while (*str0) {
    //             if (!is_number(str0)) {
    //                 strcpy(message, "ERROR");
    //                 return true;
    //             }
    //         }

    //     } else if (*args->arg_type == Item_result::REAL_RESULT) {
    //         if (*str0 != '.' && !is_number(str0) && *str0 != '+' &&
    //             *str0 != '-') {
    //             strcpy(message, "ERROR");
    //             return true;
    //         }
    //         str0++;
    //         while (*str0) {
    //             if (*str0 != '.' && !is_number(str0)) {
    //                 strcpy(message, "ERROR");
    //                 return true;
    //             }
    //         }
    //     }

    //     // todo 比较第一个参数和第二个参数的位数
    //     if (cnt0 == 0) {
    //         // 判断第一个字符
    //         if (!is_number(str0) && *str0 != '+' && *str0 != '-') {
    //             strcpy(message, "ERROR");
    //             return true;
    //         }
    //         str0++;

    //         while (*str0) {
    //             if (!is_number(str0)) {
    //                 strcpy(message, "ERROR");
    //                 return true;
    //             }
    //         }
    //     } else {
    //         if (*str0 != '.' && !is_number(str0) && *str0 != '+' &&
    //             *str0 != '-') {
    //             strcpy(message, "ERROR");
    //             return true;
    //         }
    //         str0++;

    //         while (*str0) {
    //             if (*str0 != '.' && !is_number(str0)) {
    //                 strcpy(message, "ERROR");
    //                 return true;
    //             }
    //         }
    //     }
    // }

    args_count = args->arg_count;

    if (args_count > 2 || args_count < 1) {
        strcpy(message,
               "wrong number of the args: to_char() requires one or two "
               "arguments");
        return true;
    }

    // string 或 num
    if (args->arg_type[0] == Item_result::STRING_RESULT) {
        is_string = 1;
    } else {
        is_num = 1;
    }
    if (args_count == 2 && args->arg_type[1] != Item_result::STRING_RESULT) {
        strcpy(message, "wrong type of the args: the second need to be string");
        return true;
    }
    args->arg_type[0] = Item_result::STRING_RESULT;
    return false;
}

void convert_str(char *str, char *result) {
    if (is_string) {
        strcpy(result, str);
    } else {
        int length = strlen(str);
        // 判断小数点
        int cnt = 0;

        char *pos_str = str;
        char *pos_result = result;
        // str 最后一个位置
        char *pos_str_end = str + length - 1;

        for (int i = 0; i < length; i++) {
            if (str[i] == '.') {
                cnt = 1;
                break;
            }
        }
        // 对浮点数去除末尾 0/' '
        if (cnt) {
            // 去除末尾的 0/' ' 一直到非零数字或者'.'
            while (*pos_str_end == ' ' || *pos_str_end == '.' ||
                   *pos_str_end == '0') {
                pos_str_end--;
                if (*(pos_str_end + 1) == '.') {
                    break;
                }
            }
        }
        pos_str_end++;

        // 去除前面的空格
        while (*pos_str == ' ') {
            pos_str++;
        }
        // 有 '-' 需要额外赋值
        if (*pos_str == '-') {
            *pos_result = *pos_str;
            pos_str++;
            pos_result++;
        }
        // 去除首部多余的 0
        while (*pos_str == '0') {
            pos_str++;
        }
        while (pos_str != pos_str_end) {
            *pos_result = *pos_str;
            pos_str++;
            pos_result++;
        }

        // 在去除多于数字之后如果没有数字，用 0 表示
        if (!strlen(result)) {
            strcpy(result, "0");
        }
    }
}

void convert_str(char *str1, char *str2, char *result) {}

char *to_char(UDF_INIT *initid, UDF_ARGS *args, char *result,
              unsigned long *length, char *is_null, char *error) {
    //如果为空，返回 NULL
    if (!args->args[0]) {
        *is_null = 1;
        return NULL;
    }

    switch (args_count) {
        case 1: {
            convert_str(args->args[0], result);
            break;
        }
        case 2: {
            convert_str(args->args[0], args->args[1], result);
            break;
        }
    }
    *length = strlen(result);
    is_string = 0;
    is_num = 0;
    return result;
}