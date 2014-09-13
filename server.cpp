#include "stdio.h"
#include "socket.h"
#include "ecc.h"
#include "utils.h"
#include <signal.h>

extern const int sec_size = (p_size)-1; 
unsigned long *indexSet;
mpz_t *coeff;
extern mpz_t sigma,meu[s];

void sendResponse(int clisock, uint64_t *diffNano)
{
	char lind[1024]={0};
	receiveFile("stmp", clisock);
  getTimeElapsed();

	FILE* fp = fopen("stmp","rb");
	fgets(lind, sizeof(lind), fp);
	unsigned long l, index;

	l = atol(lind);
	indexSet = (unsigned long*)malloc(l*sizeof(unsigned long));

	for (int i = 0; i<l; i++)
	{
		bzero(lind, 1024);
		fgets(lind, sizeof(lind), fp);
		indexSet[i] = atol(lind);
	}

	coeff = (mpz_t*)malloc(l*sizeof(mpz_t));
    for(int i=0; i<l; i++)
    {
    	mpz_init(coeff[i]);
		bzero(lind, 1024);
		fgets(lind, sizeof(lind), fp);
    	mpz_set_str(coeff[i], lind, 62);
    }
    fclose(fp);

    server(l);

    fp = fopen("stmpr","wb");

    for(int i=0; i<s; i++)
    {
    	mpz_get_str(lind, 62, meu[i]);
    	fprintf(fp, "%s\n", lind);
    	bzero(lind, 1024);
    }

	mpz_get_str(lind, 62, sigma);
	fprintf(fp, "%s\n", lind);
	fclose(fp);

	*diffNano = getTimeElapsed();
  cout<<"Computation Time:  "<< *diffNano <<endl << endl;
	sendFile("stmpr", clisock);
	free(indexSet);
	free(coeff);
}

int servsock;
void sig_handler(int sig)
{
  shutdown(servsock, SHUT_RDWR);
	closeSocket(servsock);
  exit(0);
}

int main(int argc, char *argv[])
{
	long port = PORT;
  int fa = 0, flag, numOfChallenges = 0;
	if (argc >= 2)
    fa = atoi(argv[1]);
	
  if (fa > 1024)
		port = atoi(argv[1]);

  signal(SIGINT, sig_handler);

	servsock = serverConnect(port);
	int clisock;
  char buff[10] = {0};
  while(1)
  {
    clisock = serverListen(servsock);

    // HandShake
    bzero(buff, 10);
    receiveMsg(buff, 6, clisock);
    if (strncmp(buff, "HELLO", 5))
      continue;
    sendMsg("GOAHEAD", 8, clisock);
    receiveObj(&numOfChallenges, sizeof(int), clisock);
    if (numOfChallenges <=0 || numOfChallenges > 100000)
      continue;
    receiveMsg(buff, 2, clisock);
    if (buff[0] == 'R')
      flag = 0;
    else if (buff[0] == 'P')
      flag = 1;
    else
      continue;
    sendMsg("GOAHEAD", 8, clisock);
    //End HandShake
	  uint64_t elapsed = 0;
	  long double average = 0;

	  for (int i=0; i<numOfChallenges; i++)
    {
      elapsed = 0;
      if (flag)
		    sendResponse(clisock, &elapsed);
      else
      {
        receiveFile("tempfile", clisock);
        sendFile("tempfile", clisock);
      }
      average += elapsed/(float)numOfChallenges;
    }
    cout<<"Average Computation Time:  "<< average <<endl;

	  closeSocket(clisock);
  }
  shutdown(servsock, SHUT_RDWR);
	closeSocket(servsock);

	return 0;
}


