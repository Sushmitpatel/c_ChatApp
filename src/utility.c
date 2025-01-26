#include "common.h"
void error_handler(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}