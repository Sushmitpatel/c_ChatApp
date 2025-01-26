#ifndef Chat_h
#define Chat_h
extern volatile int should_terminate;
void getinput(char *buffer,int buffer_size);
void send_message(int socket,const char* msg);
void receive_message(int socket,char *buffer,int buffer_size);
#endif //chat_h