#include <mysql.h>
#include <string.h>

#include <cstring>
#include <iostream>
extern "C" {
char *to_char(UDF_INIT *initid, UDF_ARGS *args, char *result,
              unsigned long *length, char *is_null, char *error);
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void xxx_deinit(UDF_INIT *initid);
}
int is_string, is_num;
int $_num, d_num, s_num, mi_num, pr_num, e_num;
char letter[] = {'.', ',', 'd', 'm', 'i', 'p', 'r', 's', '$', '0', '9', 'e'};
// char *ans = (char*)malloc(256);
void str_lwr(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') str[i] += 32;
    }
}
char *tmp = 0;
bool check_str(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        int letter_flag = 0;
        if (str[i] == '.' || str[i] == 'd') {
            d_num++;
            str[i] = '.';
        }
        if (str[i] == '$') $_num++;
        for (int j = 0; j < strlen(letter); j++) {
            if (str[i] == letter[j]) {
                letter_flag = 1;
                break;
            }
        }
        if (!letter_flag) return 0;
    }

    if (d_num > 1 || $_num > 1) return 0;
    if (str[0] == 'm' || str[0] == 'i' || str[0] == 'p' || str[0] == 'r')
        return 0;

    if (str[0] == 's') s_num = 1;

    for (int i = 1; i < len; i++) {
        if (str[i] == 'm') {
            if (str[i + 1] == 'i' && str[i + 2] == '\0')
                mi_num = 1;
            else
                return 0;
        }
        if (str[i] == 'p') {
            if (str[i + 1] == 'r' && str[i + 2] == '\0')
                pr_num = 1;
            else
                return 0;
        }
        if (str[i] == 's') {
            if (!s_num && str[i + 1] == '\0')
                s_num = 2;
            else
                return 0;
            ;
        }
        if (str[i] == 'e') {
            if (str[i + 1] != 'e' || str[i + 2] != 'e' || str[i + 3] != 'e')
                return 0;
            for (int j = i + 4; j < len; j++) {
                if (str[j] <= '9' && str[j] >= '0') return 0;
            }
            e_num++;
            i += 3;
        }
        if (str[i] == 'i' && str[i - 1] != 'm') return 0;
        ;
        if (str[i] == 'r' && str[i - 1] != 'p') return 0;
        ;
    }
    if (mi_num && pr_num) return 0;
    if (pr_num && s_num) return 0;
    if (s_num && mi_num) return 0;
    if (e_num > 1) return 0;
    return 1;
}
char *strcut(char *str) {
    int len = 1;
    tmp[0] = ' ';

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ',' || str[i] == '9' || str[i] == '0' || str[i] == '.') {
            tmp[len] = str[i];
            len++;
        }
    }

    tmp[len] = '\0';

    return tmp;
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
        if (*pos_str == '-' || *pos_str == '+') {
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
bool doit(char *str, char *fmt) {
    char *dot_str = 0;
    char *dot_fmt = 0;
    char *pos_str;
    char *pos_fmt;
    int up = 0;
    int is_dot = 1;
    int is_error = 0;
    char *str_start;
    if (str[0] == '-')
        str_start = str;
    else
        str_start = str - 1;
    for (int i = 1; i < strlen(str); i++) {
        if (str[i] == '-') return 0;
    }
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            dot_str = str + i;
            break;
        }
    }
    if (!dot_str) {
        dot_str = str + strlen(str);
    }
    for (int i = 0; i < strlen(fmt); i++) {
        if (fmt[i] == '.') {
            dot_fmt = fmt + i;
            break;
        }
    }
    if (!dot_fmt) {
        dot_fmt = fmt + strlen(fmt);
    }

    int len_str = strlen(str);
    int len_fmt = strlen(fmt);

    // dot's right
    pos_fmt = dot_fmt;
    pos_str = dot_str;
    while (pos_fmt != fmt + len_fmt && pos_str != str + len_str) {
        if (*pos_fmt == ',') return 0;
        *pos_fmt = *pos_str;
        pos_fmt++;
        pos_str++;
    }

    // fmt > str
    while (pos_fmt != fmt + len_fmt) {
        if (pos_fmt == dot_fmt) {
            pos_fmt++;
            continue;
        }
        if (*pos_fmt == ',') return 0;
        *pos_fmt = '0';
        pos_fmt++;
    }

    // str > fmt
    if (pos_str != str + len_str) {
        if (pos_str == dot_str) {
            pos_str++;
            pos_fmt++;
        }
        if (*pos_str >= '5') {
            up = 1;
            pos_fmt--;
        }
    }
    while (up && pos_fmt != dot_fmt) {
        if (*pos_fmt == '9') {
            *pos_fmt = '0';
        } else {
            *pos_fmt = *pos_fmt + 1;
            up = 0;
        }
        pos_fmt--;
    }

    // dot's left
    pos_fmt = dot_fmt - 1;
    pos_str = dot_str - 1;

    // continue up
    while (up && pos_str != str_start) {
        if (*pos_str == '9') {
            *pos_str = '0';
        } else {
            *pos_str = *pos_str + 1;
            up = 0;
        }
        pos_str--;
    }
    if (up) {
        int k;
        if (str[0] == '-')
            k = 1;
        else
            k = 0;
        for (int i = len_str; i > k; i--) {
            str[i] = str[i - 1];
        }
        str[k] = '1';
        len_str++;
        dot_str++;
    }

    pos_str = dot_str - 1;
    while (pos_str != str_start && *pos_fmt != ' ') {
        if (*pos_fmt == ',') {
            pos_fmt--;
        } else {
            *pos_fmt = *pos_str;
            pos_fmt--;
            pos_str--;
        }
    }
    if (*pos_fmt != ' ') {
        pos_fmt++;
        char *a = fmt + 1;
        char *b;
        while (*a != '0' && a != pos_fmt) a++;
        b = a - 1;
        while (*b) {
            *b = ' ';
            b--;
        }
        b = a;
        while (b != pos_fmt) {
            if (*b != ',') *b = '0';
            b++;
        }
    }

    if (pos_str != str_start) {
        is_error = 1;
    }

    pos_fmt = fmt;
    while (*pos_fmt == ' ') pos_fmt++;
    pos_fmt--;

    len_fmt = strlen(fmt);
    if (s_num) {
        char k;
        if (str[0] == '-')
            k = '-';
        else
            k = '+';
        if (s_num == 1) {
            *pos_fmt = k;
        } else {
            *(fmt + len_fmt) = k;
            *(fmt + len_fmt + 1) = '\0';
            for (int i = 0; i < strlen(fmt); i++) fmt[i] = fmt[i + 1];
        }
    } else if (mi_num) {
        if (str[0] == '-') {
            *(fmt + len_fmt) = '-';
            *(fmt + len_fmt + 1) = '\0';
        }
        for (int i = 0; i < strlen(fmt); i++) fmt[i] = fmt[i + 1];
    } else if (pr_num) {
        if (str[0] == '-') {
            *pos_fmt = '<';
            *(fmt + len_fmt) = '>';
            *(fmt + len_fmt + 1) = '\0';
        }
    } else if (str[0] == '-')
        *pos_fmt = '-';

    len_fmt = strlen(fmt);
    if ($_num) {
        pos_fmt = fmt;
        while (*pos_fmt < '0' || *pos_fmt > '9') pos_fmt++;
        char *a = fmt + len_fmt + 1;
        while (a != pos_fmt) {
            *a = *(a - 1);
            a--;
        }
        *a = '$';
    }

    if (is_error) {
        for (int i = 0; i < strlen(fmt); i++) fmt[i] = '#';
    }
    return 1;
}
void do_eeee(char *str, char *fmt) {
    char *dot_str;
    char *dot_fmt;
    int eps;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            dot_str = str + i;
            break;
        }
    }
    if (!dot_str) {
        dot_str = str + strlen(str);
    }
    for (int i = 0; i < strlen(fmt); i++) {
        if (fmt[i] == '.') {
            dot_fmt = fmt + i;
            break;
        }
    }
    if (!dot_fmt) {
        dot_fmt = fmt + strlen(fmt);
    }
    eps = fmt + strlen(fmt) - dot_fmt;  // wei shu
}
void change2(char *str, char *fmt, char *ans) {
    str_lwr(fmt);
    if (!check_str(fmt))
        strcpy(ans, "the fmt is not correct");
    else {
        fmt = strcut(fmt);
        if (e_num)
            do_eeee(str, fmt);
        else if (doit(str, fmt))
            strcpy(ans, fmt);
        else
            strcpy(ans, "wrong fmt");
    }
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
    $_num = 0;
    d_num = 0;
    s_num = 0;
    mi_num = 0;
    pr_num = 0;
    e_num = 0;
    return result;
}
bool to_char_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_type[0] == STRING_RESULT)
        is_string = 1;
    else if (args->arg_type[0] == INT_RESULT ||
             args->arg_type[1] == DECIMAL_RESULT)
        is_num = 1;
    args->arg_type[0] = STRING_RESULT;
    tmp = (char *)malloc(256);
    return 0;
}
void str_reverse_deinit(UDF_INIT *initid) { free(tmp); }