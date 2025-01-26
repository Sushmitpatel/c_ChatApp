#ifndef network_h
#define network_h

void start_server();
void connect_to_peer();
void *receive_thread(void *arg);
void *send_thread(void *arg);
void threaded_communication(int sockfd);
#endif //network_h