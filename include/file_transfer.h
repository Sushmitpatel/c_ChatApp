#ifndef file_transfer_h
#define file_transfer_h

void send_file(int sockfd,char* file_name);
void receive_file(int sockfd);
int is_file_accessible( char * file_path);
#endif //file_transfer_h