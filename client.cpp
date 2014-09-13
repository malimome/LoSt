
#include "stdio.h"
#include "socket.h"
#include "ecc.h"
//#include <sys/time.h>
//#include <sys/stat.h>
#include "utils.h"

#define TEMPFILE "ctempfile"

extern unsigned long *indices;
extern mpz_t *coeff;
extern const int sec_size = (p_size)-1; 

bool challengeResponse(int sock, unsigned long l, uint64_t *diffNano)
{
  verifier(l);

  FILE* fp = fopen("ctmp","wb");

	fprintf(fp, "%lu\n", (unsigned long)l);
	for (int i = 0; i<l; i++)
		fprintf(fp, "%lu\n", indices[i]);

  char strCff[1024] = {0};
  for(int i=0; i<l; i++)
  {
   	mpz_get_str(strCff, 62, coeff[i]);
   	fprintf(fp, "%s\n", strCff);
  	bzero(strCff, 1024);
  }
  fclose(fp);

	sendFile("ctmp", sock);
  getTimeElapsed();
    /* Buffers for response generated by server (prover) */
	receiveFile("ctmpr", sock);

	*diffNano = getTimeElapsed();
  cout<<"Time elapsed:  "<< *diffNano <<endl;

  mpz_t servSigma,meu[s];
  fp = fopen("ctmpr","rb");
  for(int i=0; i<s; i++)
  {
  	mpz_init(meu[i]);
   	fgets(strCff, sizeof(strCff), fp);
   	mpz_set_str(meu[i], strCff, 62);
   	bzero(strCff, 1024);
  }

	mpz_init(servSigma);
	fgets(strCff, sizeof(strCff), fp);
	mpz_set_str(servSigma, strCff, 62);
	fclose(fp);

  mpz_t cliSigma;
  mpz_init(cliSigma);

  verify(meu, l, &cliSigma);

  bzero(strCff, 1024);
  mpz_get_str(strCff, 10, cliSigma);
	cout << "Sigma at client: " << strCff<< endl;

	free(indices);

  if(mpz_cmp(cliSigma, servSigma)==0)
  {
    cout<<"@@@@@@@@@@@ Data verified........................"<< endl << endl;
    return true;
  }
  else
  {
    cout<<"########### Data lost............................"<< endl << endl;
    return false;
  }
}

int FileRet(int sock, uint64_t *elapsed)
{
  int res;
  sendFile(TEMPFILE, sock);
  getTimeElapsed();

  res=receiveFile(TEMPFILE, sock);
  *elapsed = getTimeElapsed();
  cout<<"Time elapsed:  "<< *elapsed << "\t res: " << res <<endl;

  return res;
}

void doHandshake(int numOfChallenges, int flag, int sock)
{
  char hn[256]={0};
  sendMsg("HELLO", 6, sock);
  receiveMsg(hn, 8, sock);
  if (strncmp(hn, "GOAHEAD", 7))
  {
    closeSocket(sock);
    exit(0);
  }
  sendObj(&numOfChallenges, sizeof(int), sock);
  if (flag)
    sendMsg("P", 2, sock);
  else
    sendMsg("R", 2, sock);
  receiveMsg(hn, 8, sock);
  if (strncmp(hn, "GOAHEAD", 7))
  {
    closeSocket(sock);
    exit(0);
  }
}

FILE * prepareOutput(const char* servhost, int l, int flag)
{
	char output[1024] = {0}, hn[256]={0};
	gethostname(hn, 256);
  mkdir("output", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (flag)
	  snprintf(output, 1024, "output/%s-%s-%d", hn, servhost, (int)l);
  else
	  snprintf(output, 1024, "output/%s-%s-FRet", hn, servhost);
	FILE *foutput = fopen(output, "w");
	fprintf(foutput, "Client: %s\n", hn);
	fprintf(foutput, "Server: %s\n", servhost);
  if (flag)
  	fprintf(foutput, "Blocks: %d\n", (int) l);
  else
	  fprintf(foutput, "File Retreival test\n");
	fprintf(foutput, "Time Elapsed \t\t\t Data Verification\n");

  return foutput;
}

int main(int argc, char *argv[])
{
	char *servhost = (char *)HOST;
	long port = PORT;
	unsigned long numOfBlocks = 0;
  int flag = 1;
	if (argc >= 2)
	{
		if (isalpha(argv[1][0]) || argv[1][0] == '?')
		{
			cout << "pclient [numOfBlocks = 10] [numOfChallenges=50] [Server Name = localhost] [Port = 8888]" << endl;
			exit(0);
		}
		else if(argv[1][0] == '-')
      flag= 0;
    else
			numOfBlocks = atoi(argv[1]);
	}

	int numOfChallenges = CHALLNUM;
  if (argc >=3)
    numOfChallenges = atoi(argv[2]); 
	if (argc >=4)
		servhost = argv[3];
	if (argc >= 5)
		if (atoi(argv[4]) > 1024)
			port = atoi(argv[4]);

	/* no of challenges in the challenge set */
    unsigned l;                        		

    unsigned long indexSize = getPrimeKeys();
    if (numOfBlocks > 0 && numOfBlocks<indexSize)
    	l = numOfBlocks;
    else
    	l = BLOCKNUM;

	int sock = clientConnect(servhost, port);
  doHandshake(numOfChallenges, flag, sock);
  FILE *foutput = prepareOutput(servhost, l, flag);

	char output[1024] = {0};
	bool res = false;
	uint64_t elapsed = 0;
	for (int i=0; i<numOfChallenges; i++)
	{
    elapsed = 0;
    if (flag)
		  res = challengeResponse(sock, l, &elapsed);
    else
      res=FileRet(sock, &elapsed);

		if (res)
			snprintf(output, 15, "%s", "Data Verified");
		else
			snprintf(output, 15, "%s", "Data lost");
		fprintf(foutput, "%llu \t\t\t %s \n", elapsed, output);
		//write the results
	}

	fclose(foutput);
	closeSocket(sock);
	return 0;	
}


