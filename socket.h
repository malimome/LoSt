#include <string.h>
#include <arpa/inet.h>

#define PORT 9973
#define HOST "localhost"
#define BUFFSIZE 1024

int sendFile(const char *filename, int sock);
int receiveFile(const char *filename, int sock);
int sendMsg(const char *msg, int, int sock);
int receiveMsg(char *buff, int, int sock);
int sendObj(const void *msg, int len, int sock);
int receiveObj(void *buff, int len, int sock);
int serverConnect(int port);
int serverListen(int sockfd);
int clientConnect(const char *hostname, int port);
void closeSocket(int sock);



