#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <string>

#include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysql.h"
#include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysql_com.h"
using namespace std;

extern "C" {
long long test_number(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
                      unsigned char *);
bool test_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
}

bool test_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 1) {
        strcpy(message, "ERROR: args number not right");
        return true;
    }
    if (args->arg_type[0] == Item_result::STRING_RESULT) {
        strcpy(message, "ERROR: String argument");
        return true;
    }
    if (args->arg_type[0] == Item_result::INT_RESULT) {
        strcpy(message, "ERROR: real argument");
        return true;
    }
    if (args->arg_type[0] == Item_result::REAL_RESULT) {
        strcpy(message, "ERROR: int argument");
        return true;
    }
    if (args->arg_type[0] == Item_result::ROW_RESULT) {
        strcpy(message, "ERROR: row argument");
        return true;
    }
    if (args->arg_type[0] == Item_result::INVALID_RESULT) {
        strcpy(message, "ERROR: String argument");
        return true;
    }
    return false;
}

long long test_number(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
                      unsigned char *) {
    const char *str = args->args[0];
    string temp = str;
    int value = count(temp.begin(), temp.end(), '.');
    return *args->arg_type;
}