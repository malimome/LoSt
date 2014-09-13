#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "socket.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int sendFile(const char *filename, int sock)
{
	FILE* fp = fopen(filename,"rb");
	int bytes, ns, fsize;
	char buffer[BUFFSIZE]={0};

  fseek (fp , 0 , SEEK_END);
  fsize = ftell (fp);
  rewind (fp);
  sendObj(&fsize, sizeof(int), sock);

	while((bytes = fread(buffer, sizeof(char), BUFFSIZE,fp))>0)
	{
		ns = send(sock, buffer, bytes, 0);
		if (ns < 0)
			error("ERROR writing to socket");
		bzero(buffer,BUFFSIZE);
 	}
	fclose(fp);
	return 0;
}

int receiveFile(const char *filename, int sock)
{
	FILE* fp = fopen(filename,"wb");
	char buffer[BUFFSIZE] = {0};
	int bytes, flag = 0, total = 0, fsize, recvsize;
  receiveObj(&fsize, sizeof(int), sock);
  if (fsize<0 || fsize>1000000)
  {
    fclose(fp);
    return 0;
  }
   	
	while(1)
	{
    if (fsize < BUFFSIZE)
      recvsize = fsize;
    else
      recvsize = BUFFSIZE;
    bytes = recv(sock, buffer, recvsize, flag);
    if (bytes <= 0)
      break;
 		fwrite(buffer, sizeof(char), bytes, fp);
    total += bytes;
    fsize -= bytes;
    if (fsize <= 0)
      break;
 		bzero(buffer, BUFFSIZE);
 		flag = MSG_DONTWAIT;
 	}
 	fclose(fp);
	return total;
}

int sendMsg(const char *msg, int len, int sock)
{
	int n;
    n = send(sock, msg, len, 0);
    if (n < 0) error("ERROR writing to socket");
    return 0;
}

int receiveMsg(char *buff, int len, int sock)
{
	int n;
    n = recv(sock, buff, len, 0);
    if (n < 0) error("ERROR reading from socket");
    return n;
}

int sendObj(const void *msg, int len, int sock)
{
	int n;
    n = send(sock, msg, len, 0);
    if (n < 0) error("ERROR writing to socket");
    return 0;
}

int receiveObj(void *buff, int len, int sock)
{
	int n;
    n = recv(sock, buff, len, 0);
    if (n < 0) error("ERROR reading from socket");
    return n;
}

int serverConnect(int port = PORT)
{
    int sockfd;
    struct sockaddr_in serv_addr;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(port);
     int y = 0;
     if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&y,sizeof(int)) == -1) {
         perror("setsockopt");
         exit(1);
     }
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
     {
              error("ERROR on binding");
              return -1;
     }
     return sockfd;
}

int serverListen(int sockfd)
{
    int newsockfd;
    struct sockaddr_in cli_addr;
	socklen_t clilen;

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
         error("ERROR on accept");
         return -1;
    }
    return newsockfd;
}

int clientConnect(const char *hostname, int port = PORT)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
        return -1;
    }

    return sockfd;
}

void closeSocket(int sock)
{
	close(sock);
}

