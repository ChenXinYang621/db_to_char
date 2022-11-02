#include <cstring>
#include <iostream>

using namespace std;

int is_string = 0;

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
                if ( *(pos_str_end + 1) == '.') {
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
        for (int i = 0; i < strlen(result); i++) {
            cout << *result;
            result++;
        }
        cout << endl;
    }
}

int main() {
    char *result;
    char *source = "123.123";
    convert_str(source, result);

    return 0;
}