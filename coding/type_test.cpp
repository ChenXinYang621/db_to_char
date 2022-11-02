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
char *test_number(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
                      char *message);
bool test_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
}

bool test_number_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    return false;
}

char *test_number(UDF_INIT *initid, UDF_ARGS *args, unsigned char *is_null,
                      char *message) {
    // int str = args->arg_type[0];
    
    // strcpy(message, temp);
    strcpy(message, "Hello");
    return message;
}