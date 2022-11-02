#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main() {
    string a = "3123.12412,51234.124.124123.123.12.312";
    cout << (long long)count(a.begin(), a.end(), '.');
    return 0;
}