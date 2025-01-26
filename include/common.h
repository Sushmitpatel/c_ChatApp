#ifndef common_h
#define common_h
extern const char *download_dir;
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<libgen.h>
#include<sys/stat.h>
// project specific header files

#include "chat_handler.h"
#include "network.h"
#include "utility.h"
#include "file_transfer.h"

#endif //common_h