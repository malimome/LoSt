 /*******************************************************************************
 * ecc.h 
 *
 *
 * Written by Sanchit Agarwal (sanchit_ag@ismu.ac.in)
 * July 2011
 *
 * This header file includes all the C++ libraries required for the software. 
 * Other than the common C++ libraries, these include GMP libraries and OpenSSL 
 * libraries.
 *
 * The software uses Reed Solomon(RS) Error Correcting Code. The RS code is from 
 * the RSCODE project and is available here http://rscode.sourceforge.net/. The 
 * RSCODE project has convenient 'byte-sized' block coding and works in the 
 * Galois Field 2^8. The maximum size of the RS codeword (message + parity) can 
 * thus be 255 bytes (2^8-1). The code is optimal for small size of parity say,  
 * 10-20 bytes and can thus correct 5-10 bytes error.
 *
 * This file also contains all the parameters that are govern the POR scheme. 
 *
 * The parameters are:
 *
 * 1. WORD_SIZE: length of the dataword for encoding
 *
 * 2. NPAR: the number of parity bits in the codeword 
 *
 * The above two parameters fix the type of RS encoding used for encoding the 
 * file
 *
 * 3. LMBDA: security parameter
 *
 * 4. p_size: size of the prime in bytes. For the private scheme this is equal 
 * to LMBDA bits
 *
 * 5. s: the number of sectors in each block of encoded file
 *
 * Finally, the declaration of various routines required for POR scheme is 
 * contained in this file. 

 *******************************************************************************/


#include<iostream> 
#include<fstream>
#include<cmath>
#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<ctime>
#include<string.h>
#include "gmp.h"
#include "openssl/aes.h"
#include "openssl/hmac.h"
#include "openssl/evp.h"


using namespace std;

#define WORD_SIZE 223         

#define NPAR 32    	  

#define LMBDA 80            

/* p_size determines the number of blocks m1 m2.....mn of the encoded file */
#define p_size LMBDA/8

#define s 10   

#define PRIME "741566175304764773004631"
#define FILENAME "FILE.TXT"
#define BLOCKNUM 10
#define CHALLNUM 50
/*****************************************************************************/

#define TRUE 1
#define FALSE 0

typedef unsigned long BIT32;
typedef unsigned short BIT16;

/*****************************************************************************/

/* Maximum degree of various polynomials. */
#define MAXDEG (NPAR*2)

/*****************************************************************************/

/* Encoder parity bytes */
extern int pBytes[MAXDEG];

/* Decoder syndrome bytes */
extern int synBytes[MAXDEG];

/* print debugging info */
extern int DEBUG;

/* Reed Solomon encode/decode routines */
void initialize_ecc (void);
int check_syndrome (void);
void print_parity(void);
void decode_data (unsigned char data[], int nbytes);
void encode_data (unsigned char msg[], int nbytes, unsigned char dst[]);

/* CRC-CCITT checksum generator */
BIT16 crc_ccitt(unsigned char *msg, int len);

/* galois arithmetic tables */
extern int gexp[];
extern int glog[];

void init_galois_tables (void);
int ginv(int elt); 
int gmult(int a, int b);


/* Error location routines */
int correct_errors_erasures (unsigned char codeword[], int csize,int nerasures, int erasures[]);

/* polynomial arithmetic */
void add_polys(int dst[], int src[]) ;
void scale_poly(int k, int poly[]);
void mult_polys(int dst[], int p1[], int p2[]);

void copy_poly(int dst[], int src[]);
void zero_poly(int poly[]);

/*****************************************************************************/

/* server client POR protocol routines */

void encode(const char *str,unsigned long in_size);

void generate_prime();
void get_prime();

void keygen(unsigned char *k_e, unsigned char *k_m);

unsigned long gen_authenticators(unsigned long en_size, unsigned char *k_e, unsigned char *k_m, unsigned char *iv);

void decode(int en_size);

void init(FILE *fp);
unsigned long getPrimeKeys();
unsigned long client(const char *filename);

unsigned long verifier(unsigned long numIndic);

void server(unsigned long l);

int verify(mpz_t meu[], unsigned long l, mpz_t *ret);

