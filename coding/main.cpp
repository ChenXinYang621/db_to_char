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

void to_char_deinit(UDF_INIT *initid);
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

int is_num = 0;
// 原数据是否为 string 类型
int is_string = 0;
int args_count = 0;

int cnt_s = 0;
int cnt_d = 0;
int cnt_dot = 0;
int cnt_mi = 0;
int cnt_pr = 0;

char *tmp;

char *standard(char *str) {
    // 保留
    tmp[0] = ' ';
    int len = 1;

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ',' || str[i] == '9' || str[i] == '0' || str[i] == '.') {
            tmp[len++] = str[i];
        }
    }

    tmp[len] = '\0';
    return tmp;
}

void string_lower(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 32;
        }
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

        // 去处末尾 ' '
        while (*pos_str_end == ' ') {
            pos_str_end--;
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

bool is_number(char *str) { return *str >= '0' && *str <= '9'; }

bool number_check(char *str) {
    // 去除前面的空格
    while (*str == ' ') {
        str++;
    }
    string temp = str;
    int length = temp.length();
    char *pos_str = str;
    char *pos_str_end = str + length - 1;

    while (*pos_str_end == ' ') {
        pos_str_end--;
    }

    // 统计 '.' 个数
    int cnt = count(temp.begin(), temp.end(), '.');
    if (cnt > 1) {
        return false;
    }

    // 判断第一个字符
    if (!is_number(pos_str) && *pos_str != '+' && *pos_str != '-' &&
        *pos_str != '.') {
        return false;
    }
    pos_str++;

    while (pos_str != pos_str_end) {
        if (!is_number(pos_str) && *pos_str != '.') {
            return false;
        }
        pos_str++;
    }

    return true;
}

bool match(char *str1, char *str2) {
    char *dot_str = 0;
    char *dot_fmt = 0;
    char *pos_str;
    char *pos_fmt;
    // 四舍五入位
    int up_flag = 0;
    int is_dot = 1;
    int is_error = 0;
    char *str_start;
    if (str1[0] == '-') {
        str_start = str1;
    } else {
        str_start = str1 - 1;
    }

    for (int i = 1; i < strlen(str1); i++) {
        if (str1[i] == '-') {
            return false;
        }
    }
    // 查找小数点的位置
    for (int i = 0; i < strlen(str1); i++) {
        if (str1[i] == '.') {
            dot_str = str1 + i;
            break;
        }
    }
    // 如果没有小数，dot_str 在末尾处
    if (!dot_str) {
        dot_str = str1 + strlen(str1);
    }
    // 查找第二个参数的小数点位置
    for (int i = 0; i < strlen(str2); i++) {
        if (str2[i] == '.') {
            dot_fmt = str2 + i;
            break;
        }
    }
    // 如果没有小数，dot_fmt 在末尾处
    if (!dot_fmt) {
        dot_fmt = str2 + strlen(str2);
    }

    int len_str = strlen(str1);
    int len_fmt = strlen(str2);

    // 小数右边部分
    pos_fmt = dot_fmt;
    pos_str = dot_str;
    while (pos_fmt != str2 + len_fmt && pos_str != str1 + len_str) {
        // 小数部分出现 ','
        if (*pos_fmt == ',') {
            return false;
        }
        *pos_fmt = *pos_str;
        pos_fmt++;
        pos_str++;
    }

    // str2.length() > str1.length()
    // 多出的部分用 0 补全
    while (pos_fmt != str2 + len_fmt) {
        if (pos_fmt == dot_fmt) {
            pos_fmt++;
            continue;
        }
        if (*pos_fmt == ',') {
            return false;
        }
        *pos_fmt = '0';
        pos_fmt++;
    }

    // str1.length() > str2.length()
    // 截断的情况需要考虑四舍五入
    if (pos_str != str1 + len_str) {
        if (pos_str == dot_str) {
            pos_str++;
            pos_fmt++;
        }
        if (*pos_str >= '5') {
            up_flag = 1;
            pos_fmt--;
        }
    }
    while (up_flag && pos_fmt != dot_fmt) {
        // '9' 的情况需要额外考虑进位问题
        if (*pos_fmt == '9') {
            *pos_fmt = '0';
        } else {
            *pos_fmt = *pos_fmt + 1;
            up_flag = 0;
        }
        pos_fmt--;
    }

    // 整数部分
    pos_fmt = dot_fmt - 1;
    pos_str = dot_str - 1;

    // 进行整数部分的进位
    while (up_flag && pos_str != str_start) {
        if (*pos_str == '9') {
            *pos_str = '0';
        } else {
            *pos_str = *pos_str + 1;
            up_flag = 0;
        }
        pos_str--;
    }
    if (up_flag) {
        // 存在 '.8' 的情况
        int k;
        if (str1[0] == '-') {
            k = 1;
        } else {
            k = 0;
        }
        for (int i = len_str; i > k; i--) {
            str1[i] = str1[i - 1];
        }
        str1[k] = '1';
        len_str++;
        dot_str++;
    }

    // 回到整数部分的判断，解决 ',' 问题
    pos_str = dot_str - 1;
    while (pos_str != str_start && *pos_fmt != ' ') {
        if (*pos_fmt == ',') {
            pos_fmt--;
        } else {
            // 如果非 ',' 添加进 fmt
            *pos_fmt = *pos_str;
            pos_fmt--;
            pos_str--;
        }
    }
    if (*pos_fmt != ' ') {
        pos_fmt++;
        char *a = str2 + 1;
        char *b;
        // 判断前面多余的匹配的值，'9' 补 ' '，其余一律为 '0'
        while (*a != '0' && a != pos_fmt) {
            a++;
        }
        b = a - 1;
        // '9' 情况需要补 ' '
        while (*b) {
            *b = ' ';
            b--;
        }
        b = a;
        // 空余补 '0'
        while (b != pos_fmt) {
            if (*b != ',') {
                *b = '0';
            }
            b++;
        }
    }

    // 出现多余字符
    if (pos_str != str_start) {
        is_error = 1;
    }

    pos_fmt = str2;
    // 去除前面的 ' '
    while (*pos_fmt == ' ') {
        pos_fmt++;
    }
    pos_fmt--;

    // 修改过后的 str2 长度
    len_fmt = strlen(str2);
    // s 需要显示符号
    if (cnt_s) {
        char k;
        if (str1[0] == '-') {
            k = '-';
        } else {
            k = '+';
        }
        if (cnt_s == 1) {
            *pos_fmt = k;
        } else {
            *(str2 + len_fmt) = k;
            *(str2 + len_fmt + 1) = '\0';
            for (int i = 0; i < strlen(str2); i++) {
                str2[i] = str2[i + 1];
            }
        }
    } else if (cnt_mi) {
        // 显示 '-'
        if (str1[0] == '-') {
            *(str2 + len_fmt) = '-';
            *(str2 + len_fmt + 1) = '\0';
        }
        for (int i = 0; i < strlen(str2); i++) {
            str2[i] = str2[i + 1];
        }
    } else if (cnt_pr) {
        // 添加 "<>"
        if (str1[0] == '-') {
            *pos_fmt = '<';
            *(str2 + len_fmt) = '>';
            *(str2 + len_fmt + 1) = '\0';
        }
    } else if (str1[0] == '-')
        *pos_fmt = '-';

    len_fmt = strlen(str2);
    // 如果有 $_flag，在结果添加 $
    if (dollar_flag) {
        pos_fmt = str2;
        while (*pos_fmt < '0' || *pos_fmt > '9') pos_fmt++;
        char *a = str2 + len_fmt + 1;
        while (a != pos_fmt) {
            *a = *(a - 1);
            a--;
        }
        *a = '$';
    }

    // 参数过少问题
    if (is_error) {
        for (int i = 0; i < strlen(str2); i++) {
            str2[i] = '#';
        }
    }
    return true;
}

void convert_str(char *str1, char *str2, char *result) {
    string_lower(str2);
    if (!is_string) {
        convert_str(str1, result);
    } else {
        if (!number_check(str1)) {
            strcpy(result, "ERROR: please input a number");
            return;
        } else {
            is_string = 0;
            convert_str(str1, result);
        }
    }

    str2 = standard(str2);
    if (match(str1, str2)) {
        strcpy(result, str2);
    } else {
        strcpy(result, "ERROR: fmt wrong");
    }
}

// 检查是否存在非法条件数
// bool str_check(string str) {}

// 检查格式化字符串
bool fmt_check(char *str) {
    char letter[] = {'.', ',', 'd', 'm', 'i', 'p', 'r', 's', '$', '0', '9'};
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        int letter_flag = 0;
        if (str[i] == '.' || str[i] == 'd') {
            cnt_dot++;
            str[i] = '.';
        }
        if (str[i] == '$') {
            dollar_flag++;
        }
        for (int j = 0; j < strlen(letter); j++) {
            if (str[i] == letter[j]) {
                letter_flag = 1;
                break;
            }
        }
        if (!letter_flag) {
            return false;
        }
    }

    if (cnt_dot > 1 || dollar_flag > 1) {
        return false;
    }
    if (str[0] == 'm' || str[0] == 'i' || str[0] == 'p' || str[0] == 'r') {
        return false;
    }

    if (str[0] == 's') {
        cnt_s = 1;
    }

    for (int i = 1; i < len; i++) {
        if (str[i] == 'm') {
            if (str[i + 1] == 'i' && str[i + 2] == '\0') {
                cnt_mi = 1;
            } else {
                return false;
            }
        }
        if (str[i] == 'p') {
            if (str[i + 1] == 'r' && str[i + 2] == '\0') {
                cnt_pr = 1;
            } else {
                return false;
            }
        }
        if (str[i] == 's') {
            if (!cnt_s && str[i + 1] == '\0') {
                cnt_s = 2;
            } else {
                return 0;
            }
        }
        if ((str[i] == 'i' && str[i - 1] != 'm') ||
            str[i] == 'r' && str[i - 1] != 'p') {
            return false;
        }
    }
    if ((cnt_mi && cnt_pr) || (cnt_pr && cnt_s) || (cnt_s && cnt_mi)) {
        return false;
    }
    return true;
}

// 初始化函数作为参数类型和内容的判断
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    args_count = args->arg_count;

    if (args_count > 2 || args_count < 1) {
        strcpy(message,
               "ERROR: wrong number of the args, to_char() requires one or "
               "two "
               "arguments");
        return true;
    }

    // string 或 num
    if (args->arg_type[0] == STRING_RESULT)
        is_string = 1;
    else if (args->arg_type[0] == INT_RESULT ||
             args->arg_type[1] == DECIMAL_RESULT) {
        is_num = 1;
    }
    if (args_count == 2) {
        // string_lower(args->args[1]);
        if (args->arg_type[1] != Item_result::STRING_RESULT) {
            strcpy(message,
                   "ERROR: wrong type of the args, the second need to be "
                   "string");
            return true;
        }
        if (!fmt_check(args->args[1])) {
            strcpy(message, "ERROR: wrong content of the fmt");
            return true;
        }
    }
    // 一定要强转，否则会出现只能读取一个的问题
    args->arg_type[0] = Item_result::STRING_RESULT;
    tmp = (char *)malloc(255);
    return false;
}

// 需要指定输出的 length
char *to_char(UDF_INIT *initid, UDF_ARGS *args, char *result,
              unsigned long *length, char *is_null, char *error) {
    // 如果为空，返回 NULL
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
    cnt_dot = 0;
    dollar_flag = 0;
    cnt_s = 0;
    cnt_mi = 0;
    cnt_pr = 0;

    return result;
}

void to_char_deinit(UDF_INIT *initid) { free(tmp); }