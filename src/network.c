#include "common.h"
#include<sys/time.h>

#define Buffer_size 4194304
#define port 8082
#define ip "127.0.0.1"

void error_handler(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int create_socket(){
      int sockfd=socket(AF_INET,SOCK_STREAM,0);// (ip type , tcp, 0 for default protocal)
    if(sockfd<0){
       error_handler("socket creation failed\n");
    }
    return sockfd;
}

void setup_address(struct sockaddr_in* addr) {
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0) {
        error_handler("Invalid address / address not supported\n");
    }
}


void start_server(){
   // creation binding listen accept

    int server_sockfd=create_socket();
    
    struct sockaddr_in server_sockaddr;
    setup_address(&server_sockaddr);

    if(bind(server_sockfd,(struct sockaddr*)&server_sockaddr,sizeof(server_sockaddr))<0){
        error_handler("binding failed\n");
    }

    if(listen(server_sockfd,3)<0){
        error_handler("Listen failed\n");
    }
    printf("server started and waiting for connection\n");
    struct sockaddr client_sockaddr;
    int address_len=sizeof(client_sockaddr);
    int new_sockfd;
    if((new_sockfd=accept(server_sockfd,(struct sockaddr*)&client_sockaddr,(socklen_t*)&address_len))<0){
        error_handler("accept failed\n");
    }
    printf("connection established wiht peer\n");
    threaded_communication(new_sockfd);
    close(server_sockfd);
    close(new_sockfd);

}

void connect_to_peer(){
   int client_sockfd=create_socket();
   struct sockaddr_in server_address;
   setup_address(&server_address);
   
   if(connect(client_sockfd,(struct sockaddr*)&server_address,sizeof(server_address))<0){
    error_handler("connection failed\n");
   }
   printf("connection established to peer\n");
   threaded_communication(client_sockfd);
   close(client_sockfd);
}

void *send_thread(void *arg) {
    int sockfd = *((int *)arg);
    char buffer[Buffer_size];
    char command[Buffer_size];

    while (1) {
        getinput(buffer, Buffer_size);
        if (should_terminate) {
            break;
        }

        // Check if the input is a file send command
        if (strncmp(buffer, "/sendfile ", 10) == 0) {
            // Extract file path from the command
            char *file_path = buffer + 10;  // Skip the "/sendfile " part
            if (is_file_accessible(file_path)) {
                int type = 1;  // Indicates file transfer
                struct timeval start_time, end_time;
                gettimeofday(&start_time, NULL);

                // Send type as file transfer
                send(sockfd, &type, sizeof(type), 0);

                // Send the file
                send_file(sockfd, file_path);

                // Measure and display time taken for the transfer
                gettimeofday(&end_time, NULL);
                double time_taken = (end_time.tv_sec - start_time.tv_sec) + 
                                    (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
                printf("\nTime taken: %.2f seconds\n", time_taken);
            } else {
                printf("Error: File '%s' is not accessible.\n", file_path);
            }
        } else if (strcmp(buffer, "/exit") == 0) {
            // User requested to exit the chat
            printf("Exiting...\n");
            should_terminate = 1;
            break;
        } else {
            // Send a regular chat message
            int type = 0;  // Indicates text message
            send(sockfd, &type, sizeof(type), 0);
            send_message(sockfd, buffer);
        }

         printf("\033[0;34mYou\033[0m: ");
          fflush(stdout);
       
    }

    return NULL;
}


void *receive_thread(void *arg){
  int sockfd=*((int *)arg);
  char buffer[Buffer_size];
  int message_type=0;
  while(1){
    if(should_terminate){
      break;
    }
    recv(sockfd,&message_type,sizeof(message_type),0);
    memset(buffer,0,sizeof(buffer));
    if(message_type==0){
       printf("\n\033[0;31m sender:");
       fflush(stdout);
       receive_message(sockfd,buffer,Buffer_size);
       printf("\033[0;32m%s\033[0m\n", buffer);
       fflush(stdout);
    }
    else if(message_type==1){
      receive_file(sockfd);
    }
    fflush(stdout);
   
  }
  return NULL;
}
void threaded_communication(int sockfd){
   pthread_t send_tid,recv_tid;
   pthread_create(&send_tid,NULL,send_thread,&sockfd);
   pthread_create(&recv_tid,NULL,receive_thread,&sockfd);
   pthread_join(send_tid,NULL);
   pthread_join(recv_tid,NULL);
}