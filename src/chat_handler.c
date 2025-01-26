#include "common.h"
extern volatile int should_terminate;
void getinput(char*buffer,int buffer_size){
    memset(buffer,0,buffer_size);
    if(!fgets(buffer,buffer_size,stdin)){
        error_handler("invalid/ error reading input\n");
    }
    int len=strlen(buffer);
    if(len>0 && buffer[len-1]=='\n'){
        buffer[len-1]='\0';
    }

}
void send_message(int sockfd,const char *msg){
    if(send(sockfd,msg,strlen(msg),0)<0){
        close(sockfd);
        error_handler("send failed");
    }
}

void receive_message(int sockfd, char *buffer,int buffer_size){
    int bytes_received=recv(sockfd,buffer,buffer_size,0);
    if(bytes_received<0){
        close(sockfd);
        error_handler("send failed");
    }
    if(bytes_received==0){
        printf("connection terminated\n");
        should_terminate=1;
    }
    buffer[bytes_received]='\0';
   
}