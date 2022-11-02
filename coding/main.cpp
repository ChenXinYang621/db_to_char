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
// 第二个参数整数部分
int number_int2 = 0;
// 小数部分的位数
int number_double = 0;
// 判断负数情况
int flag = 0;
// 判断小数点（第一个参数）
int dot_position = -1;
// 判断小数点（第二个参数）
int dot_position2 = -1;
// 判断 $
int dollar_flag = 0;

// 判断为数字
bool is_number(const char *str) { return *str >= '0' && *str <= '9'; }

int is_num = 0;
// 原数据是否为 string 类型
int is_string = 0;
int args_count = 0;

void string_lower(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') str[i] += 32;
    }
}

void convert_str(char *str, char *result) {
    if (is_string) {
        strcpy(result, str);
    } else {
        int length = strlen(str);

        char *pos_str = str;
        char *pos_result = result;
        // str 最后一个位置
        char *pos_str_end = str + length - 1;

        for (int i = 0; i < length; i++) {
            if (str[i] == '.') {
                dot_position = i;
                break;
            }
        }

        // 对浮点数去除末尾 0/' '
        if (dot_position != -1) {
            // 去除末尾的 0/' ' 一直到非零数字或者'.'
            while (*pos_str_end == '.' || *pos_str_end == '0') {
                pos_str_end--;
                if (*(pos_str_end + 1) == '.') {
                    break;
                }
            }
        }
        pos_str_end++;

        // 有 '-' 需要额外赋值
        if (*pos_str == '-') {
            *pos_result = *pos_str;
            flag = 1;
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

        int result_length = strlen(result);
        // 在去除多于数字之后如果没有数字，用 0 表示
        if (!result_length) {
            strcpy(result, "0");
            return;
        }

        // 开始判断位数
        if (dot_position == -1) {
            number_int = result_length;
        } else {
            number_int = dot_position;
            number_double = result_length - dot_position;
        }
        if (flag == 1) {
            number_int--;
        }
    }
}

void convert_str(char *str1, char *str2, char *result) {}

// 检查是否存在非法条件数
bool str_check(string str) {}

bool fmt_check(string str) {
    // 判断格式化输出格式是否符合标准
    // 转为小写，方便进行判断
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    int dollar_flag = count(str.begin(), str.end(), '$');
    int cnt_s = count(str.begin(), str.end(), 's');
    int cnt_d = count(str.begin(), str.end(), 'd');
    int cnt_dot = count(str.begin(), str.end(), '.');
    int cnt_0 = count(str.begin(), str.end(), '0');
    int cnt_9 = count(str.begin(), str.end(), '9');

    // 通过 find 方法统计 "mi" 和 "pr"
    int i = 0;
    int cnt_mi = 0;
    int length = str.length();
    while (str.find("mi", i) != string::npos) {
        cnt_mi++;
        i = str.find("mi", i) + 1;
    }
    int cnt_pr = 0;
    while (str.find("pr", i) != string::npos) {
        cnt_pr++;
        i = str.find("pr", i) + 1;
    }

    if (cnt_dot) {
        dot_position2 = str.find(".");
    } else if (cnt_d) {
        dot_position2 = str.find("d");
    }

    // 各种限制条件的判断
    if (cnt_s > 1 || cnt_mi > 1 || cnt_pr > 1 || dollar_flag > 1 || cnt_d > 1 ||
        cnt_dot > 1 || (cnt_dot == cnt_d == 1) || (cnt_s == cnt_mi == 1) ||
        (cnt_s == cnt_pr == 1) || (cnt_mi == cnt_pr == 1)) {
        return false;
    }

    // 统计整数部分的数量
    for (int i = 0; str[i] != 'd' || str[i] != '.'; i++) {
        if (str[i] == '9' || str[i] == '0') {
            number_int2++;
        }
    }

    // 正确的字母
    char right_letter[5] = {'0', '9', '$', '.', 'd'};

    // for (int i = 0; i < str.length(); i++) {
    //     if (i != 0) {
    //         if (str[i] == '$') {
    //             continue;
    //         }
    //     } else if (i == 0) {
    //         if (str[i] == 'S') {
    //         }
    //     }
    // }

    // if (cnt_s == 1) {
    // }
    int start = 0;
    int end = length;
    if (cnt_s == 1) {
        if (str.find("s") == 0) {
            start++;
        } else if (str.find("s") == length - 1) {
            end--;
        } else {
            return false;
        }
    }
    if (cnt_pr == 1) {
        if (str.find("pr") == length - 2) {
            end -= 2;
        } else {
            return false;
        }
    }
    if (cnt_mi == 1) {
        if (str.find("mi") == length - 2) {
            end -= 2;
        } else {
            return false;
        }
    }
    // for (int i = start; i < end; i++) {
    //     int j = 0;
    //     for (j = 0; j <)
    // }

    return true;
}

// 初始化函数作为参数类型和内容的判断
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    args_count = args->arg_count;

    if (args_count > 2 || args_count < 1) {
        strcpy(message,
               "ERROR: wrong number of the args, to_char() requires one or two "
               "arguments");
        return true;
    }

    // string 或 num
    if (args->arg_type[0] == Item_result::STRING_RESULT) {
        is_string = 1;
    } else {
        is_num = 1;
    }
    if (args_count == 2) {
        string_lower(args->args[1]);
        if (args->arg_type[1] != Item_result::STRING_RESULT) {
            strcpy(
                message,
                "ERROR: wrong type of the args, the second need to be string");
            return true;
        }
        if (!fmt_check(string(args->args[1]))) {
            strcpy(message, "ERROR: wrong content of the fmt");
            return true;
        }
    }
    // 一定要强转，否则会出现只能读取一个的问题
    args->arg_type[0] = Item_result::STRING_RESULT;
    return false;
}

// 需要指定输出的 length
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