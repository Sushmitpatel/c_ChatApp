#include "common.h"
volatile int should_terminate=0;
const char *download_dir = "/mnt/c/Users/Sushmit Patel/Desktop/";
int main(){
    int choice=0;
    printf("                welcome to distributed chat application\n\n\n");
    printf("IMP: ");
    printf("1. Type your message or use /sendfile <filepath> to send a file.\n");
    printf("2. first send message to each other for connection and first message will not be shown to each other \n");
    while(1){
    printf("1: start server\n");
    printf("2: connect to peer\n");
    scanf("%d",&choice);
    if(choice==1){
        start_server();
    }
    else if(choice==2){
        connect_to_peer();
    }

    }
   
    return 0;
}