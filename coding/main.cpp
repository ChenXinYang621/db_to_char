// 在 Path 中引入 MySQL 相关库
#include <mysql.h>
#include <mysql_com.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
using namespace std;

// MySQL UDF 必须使用 C/C++ 实现
// 可以通过 UDF 扩充 MySQL 的功能，加入一个新的 SQL 函数类似于内置的函数
extern "C" {
// 主函数，char * 对应 SQL STRING
char *to_char(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
              unsigned char *);
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

bool right_number(const char *str) {

    return true;
}

// 初始化函数作为参数类型和内容的判断
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    // 限制参数为 1 或 2 个
    if (args->arg_count != 2 && args->arg_count != 1) {
        strcpy(message,
               "wrong number of the args: to_char() requires one or two "
               "arguments");
        return true;
    }

    // 参数为 1 个的情况
    if (args->arg_count == 1) {
        // 如果参数在 MySQL 输入时就有错误，会返回 INVALID_RESULT
        // 对 string 类型不做约束，在 string 和 int 之外抛出错误
        if (*args->arg_type == Item_result::DECIMAL_RESULT) {
            return false;
        }
        const char *str = args->args[0];
        string temp = str;

        int cnt = count(temp.begin(), temp.end(), '.');
        // 判断第一个字符
        if (!is_number(str) && *str != '+' && *str != '-') {
            strcpy(message, "ERROR: Invalid First Char");
            return true;
        }
        str++;
        while (*str) {
            if (!is_number(str)) {
                strcpy(message, "ERROR: Invalid Content");
                return true;
            }
        }

        if (*str != '.' && !is_number(str) && *str != '+' && *str != '-') {
            strcpy(message, "ERROR: Invalid Content");
            return true;
        }
        str++;
        while (*str) {
            if (*str != '.' && !is_number(str)) {
                strcpy(message, "ERROR: Invalid Content");
                return true;
            }
        }
        return false;
    }

    // 参数为 2 个的情况
    if (args->arg_count == 2) {
        // 第一个参数为 int/double 类型，第二个参数为 string 类型
        if ((*args->arg_type != Item_result::INT_RESULT &&
             *args->arg_type != Item_result::REAL_RESULT) ||
            *(args->arg_type + 1) != Item_result::STRING_RESULT) {
            strcpy(message, "ERROR: Wrong Type");
            return true;
        }

        const char *str0 = args->args[0];
        const char *str1 = args->args[1];
        string temp0 = str0, temp1 = str1;

        // 判断格式化输出格式是否符合标准
        // temp1 转为小写，方便进行判断
        transform(temp1.begin(), temp1.end(), temp1.begin(), ::tolower);
        int cnt_dollar = count(temp1.begin(), temp1.end(), "$");
        int cnt_pr = count(temp1.begin(), temp1.end(), "pr");
        int cnt_s = count(temp1.begin(), temp1.end(), "s");
        int cnt_mi = count(temp1.begin(), temp1.end(), "mi");
        int cnt_d = count(temp1.begin(), temp1.end(), "d");
        int cnt_dot = count(temp1.begin(), temp1.end(), ".");
        int cnt_0 = count(temp1.begin(), temp1.end(), "0");
        int cnt_9 = count(temp1.begin(), temp1.end(), "9");

        int length1 = strlen(str1);
        int i = 0;

        // 各种限制条件的判断
        if (cnt_s > 1 || cnt_mi > 1 || cnt_pr > 1 || cnt_dollar > 1 ||
            cnt_d > 1 || cnt_dot > 1) {
            strcpy(message, "ERROR");
            return true;
        }
        if (cnt_dot == cnt_d == 1) {
            strcpy(message, "ERROR");
            return true;
        }
        if ((cnt_s == cnt_mi == 1) || (cnt_s == cnt_pr == 1) ||
            (cnt_mi == cnt_pr == 1)) {
            strcpy(message, "ERROR");
            return true;
        }

        if (cnt_s == 1) {
        }

        if (*args->arg_type == Item_result::INT_RESULT) {
            // 判断第一个字符
            if (!is_number(str0) && *str0 != '+' && *str0 != '-') {
                strcpy(message, "ERROR");
                return true;
            }
            str0++;
            while (*str0) {
                if (!is_number(str0)) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }

        } else if (*args->arg_type == Item_result::REAL_RESULT) {
            if (*str0 != '.' && !is_number(str0) && *str0 != '+' &&
                *str0 != '-') {
                strcpy(message, "ERROR");
                return true;
            }
            str0++;
            while (*str0) {
                if (*str0 != '.' && !is_number(str0)) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }
        }

        // todo 比较第一个参数和第二个参数的位数
        if (cnt0 == 0) {
            // 判断第一个字符
            if (!is_number(str0) && *str0 != '+' && *str0 != '-') {
                strcpy(message, "ERROR");
                return true;
            }
            str0++;

            while (*str0) {
                if (!is_number(str0)) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }
        } else {
            if (*str0 != '.' && !is_number(str0) && *str0 != '+' &&
                *str0 != '-') {
                strcpy(message, "ERROR");
                return true;
            }
            str0++;

            while (*str0) {
                if (*str0 != '.' && !is_number(str0)) {
                    strcpy(message, "ERROR");
                    return true;
                }
            }
        }
    }

    return false;
}

char *to_char(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
              unsigned char *) {
    if (!args->args[0]) {
        *is_null = 1;
        return NULL;
    }

    return NULL;
}