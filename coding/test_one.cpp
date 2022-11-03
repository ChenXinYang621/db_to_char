#include <algorithm>
#include <cstring>
#include <iostream>

using namespace std;

int dollar_flag = 0;
int dot_position2 = -1;
int number_int2 = 0;

bool fmt_check(string str) {
    // 判断格式化输出格式是否符合标准
    // 转为小写，方便进行判断
    dollar_flag = count(str.begin(), str.end(), '$');
    int cnt_s = count(str.begin(), str.end(), 's');
    int cnt_d = count(str.begin(), str.end(), 'd');
    int cnt_dot = count(str.begin(), str.end(), '.');
    int cnt_0 = count(str.begin(), str.end(), '0');
    int cnt_9 = count(str.begin(), str.end(), '9');

    // 通过 find 方法统计 "mi" 和 "pr"
    int init_point = 0;
    int cnt_mi = 0;
    int length = str.length();
    while (str.find("mi", init_point) != string::npos) {
        cnt_mi++;
        init_point = str.find("mi", init_point) + 1;
    }
    int cnt_pr = 0;
    init_point = 0;
    while (str.find("pr", init_point) != string::npos) {
        cnt_pr++;
        init_point = str.find("pr", init_point) + 1;
    }
    // 查询小数位置
    if (cnt_dot) {
        dot_position2 = str.find(".");
    } else if (cnt_d) {
        dot_position2 = str.find("d");
    }

    // 各种限制条件的判断
    if (cnt_s > 1 || cnt_mi > 1 || cnt_pr > 1 || dollar_flag > 1 || cnt_d > 1 ||
        cnt_dot > 1 || (cnt_dot == 1 && cnt_d == 1) ||
        (cnt_s == 1 && cnt_mi == 1) || (cnt_s == 1 && cnt_pr == 1) ||
        (cnt_mi == 1 && cnt_pr == 1)) {
        return false;
    }

    // 统计整数部分的数量
    for (int i = 0; i < length && (str[i] != 'd' || str[i] != '.'); i++) {
        if (str[i] == '9' || str[i] == '0') {
            number_int2++;
        }
    }

    // 正确的字母
    char right_letter[6] = {'0', '9', '$', '.', 'd', ','};

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
    // char right_letter[6] = {'0', '9', '$', '.', 'd', ','};
    int number_flag = 0;
    for (int i = start; i < end; i++) {
        int j = 0;
        for (j = 0; j < 6; j++) {
            if (str[i] == right_letter[j]) {
                break;
            }
        }
        if (j == 6) {
            return false;
        }
        if (j == 0 || j == 1) {
            number_flag++;
        }
        if (j == 5) {
            if (!number_flag) {
                return false;
            }
            number_flag = 0;
        }
    }

    return true;
}

int main() {
    bool a = fmt_check(".,99");
    cout << a << endl;

    return 0;
}