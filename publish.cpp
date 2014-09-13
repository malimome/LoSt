
#include "stdio.h"
#include "socket.h"
#include "ecc.h"
#include <sys/time.h>

extern unsigned long *indices;
extern mpz_t *coeff;
extern const int sec_size = (p_size)-1;

int main(int argc, char *argv[])
{
	char *servhost = (char *)HOST;
	long port = PORT;

	if (argc >=2)
		servhost = argv[2];
	if (argc >= 3)
		if (atoi(argv[3]) > 1024)
			port = atoi(argv[3]);

	FILE *fp = fopen("KEYS", "wb");
    init(fp);
    unsigned long indexSize = client(FILENAME);
    fprintf(fp, "%d\n", (int)indexSize);
    fclose(fp);

	//int sock = clientConnect(servhost, port);
	//sendFile("Authenticators", sock);
	//sendFile("Encoded", sock);

	//closeSocket(sock);
	return 0;
}


