 /******************************************************************************
 * client.cpp
 *
 *
 * This file contains the various functions performed by the client in the POR
 * scheme. There are three important functions defined in this file. The first 
 * one is the function client(). It calls several other functions which in turn
 * are defined in another file "por.cpp". The functions called are: 
 *
 * 1. generate_prime() : Randomly generates a prime number of p_size bytes. 
 *
 * 2. keygen(): Randomly generates the two keys, 'Encryption key' and 'MAC key' 
 * required in the scheme.
 *
 * 3. encode(): Encodes the given input file.
 *
 * 4. gen_authenticators(): Creates the file tag, determines the blocks in the
 * encoded file and creates authenticators for each of them.
 *
 * The second important function in this module is verifier() which first verfies
 * the file tag and then calls challenge() to create a set of challenges for the 
 * prover (server). 
 *
 * The third function is verify() which verfies the server's response.
 *
 *******************************************************************************/


#include "ecc.h"

/* prime p. It is generated in the file "por.cpp" and external linkage
   allows it to be accessed in this file */
extern mpz_t prime ;		             			    

/* pointer to set of indices in the challenge set */
unsigned long *indices;    

/* pointer to the set of coefficients in the challenge set */     
mpz_t *coeff;

/* encryption and mac keys */
/* secret key present with client/verifier */
unsigned char k_e[32],k_m[32]; 			

/* Intialization vector for Encryption (AES in CBC mode) */
unsigned char iv[32];	     

/* PRF(AES) key extracted from file tag */
unsigned char key_prf[32];           

/* alpha also extracted from file tag */       			     
mpz_t al[s];   
                              			     

/* creates a set of challenge(index + coefficient) for the prover */
unsigned long challenge(unsigned long n, unsigned long numIndic)
{
  // n is the index set size
   unsigned long i,j,temp;

   unsigned long l= numIndic;

   mpz_t seed;
   mpz_init(seed);
    
   /* array 'sample[]' holds all the indices 1...n. */
   unsigned long *sample; 	             			    
   sample = new unsigned long[n];    
                                                                                                 
   for(i=0;i<n;i++)
    sample[i] = i;

   if(l>n || l<1)
   {
     cout<<"Wrong input....exiting...."<<endl;
     exit(0);
   }
  
   /* allocating space to store l random indices */
   indices = (unsigned long*)malloc(l*sizeof(unsigned long));       

   /* allocating space to store l random coefficients */
   coeff = (mpz_t*)malloc(l*sizeof(mpz_t));			     
  
   for(i=0;i<l;i++)  
    mpz_init(coeff[i]);

   //cout<<"Enter the seed for generating the indices and coefficients:  ";
   //cin>>sd;
   int sd; 
   FILE * fd = fopen("/dev/urandom", "r");
   if (fd < 0) {
     perror("Failed to open \"/dev/urandom\"");
     sd = (int)time(NULL);
   }
   else
     fread(&sd, sizeof(int), 1, fd);
   srand(sd);
   
   /* shuffling the array sample[] using modern version Fisher Yates shuffle algorithm */
   /* http://en.wikipedia.org/wiki/Fisher-Yates_shuffle */
   for(i=n-1;i>0;i--)                                            
   {
     j=rand()%(i+1);
     temp=sample[j];
     sample[j]=sample[i];
     sample[i]=temp; 
  }
   
   /* The indices present at first 'l' positions of the array now form the 
      set of indices in the challenge set */
   for(i=0;i<l;i++)
   {
     indices[i] = sample[i];
     // cout<<indices[i]<<endl;
   }

   /* Creating a gmp integer seed with the same value as user given seed */
   mpz_init_set_ui(seed,sd);                
 
   /* The variable 'state' holds an algorithm selection and a current state 
      and is used in generation of random gmp integer */                        
   gmp_randstate_t state;					     
                         
   /* Initializes 'state' for a Mersenne Twister algorithm */                                            
   gmp_randinit_default(state);					      
  
   /* Sets an intial seed value into 'state' */
   gmp_randseed(state,seed);                                       

   /* Generates a uniform random integer in the range 0 to n-1, inclusive 
      The generated coefficient is thus automatically in the field Zp */
   for(i=0;i<l;i++)
    mpz_urandomm(coeff[i],state,prime);				       						   

   //delete(sample);
 
   /* Frees all memory occupied by 'state' */
   gmp_randclear(state);				
  
   return l;
}

void init(FILE *fp)
{
	//char buffer[1024] = {0};
	//generate_prime();
	get_prime();
	keygen(k_e,k_m);

	//mpz_get_str(buffer, 10, prime);
	//fprintf(fp, "%s\n", buffer);
	fwrite(k_e, sizeof(unsigned char) ,32, fp);
	fwrite(k_m, sizeof(unsigned char) ,32, fp);
	/* Initialize lookup tables, polynomials, etc. required for RS
	      encoding of the file */
	initialize_ecc();
}

unsigned long getPrimeKeys()
{
	get_prime();
	FILE *fp = fopen("KEYS", "rb");
	char buffer[5] = {0};
	//fgets(buffer, 1024, fp);
	fread(k_e, sizeof(unsigned char) ,32, fp);
	fread(k_m, sizeof(unsigned char) ,32, fp);

	bzero(buffer, 5);
	fgets(buffer, 5, fp);
	fclose(fp);
	/* Initialize lookup tables, polynomials, etc. required for RS
	      encoding of the file */
	initialize_ecc();
	return (unsigned long)atoi(buffer);
}

unsigned long client(const char *filename)
{
   /* Input file size, encoded file size and tag size */
   unsigned long in_size,en_size;                            

   /* for storing file size of different files */
   struct stat results;	        
   
   /* Computing the size of input file */					   
   stat(filename,&results);
   in_size = results.st_size;
  
   encode(filename,in_size);

   /* Computing the size of encoded file */
   stat("Encoded", &results);
   en_size=results.st_size;

   unsigned long indexSize = gen_authenticators(en_size,k_e,k_m,iv);
   return indexSize;
}

unsigned long verifier(unsigned long numIndic)
{
     
   int i=0,j=0,k=0,r=0;

   /* no of blocks in the encoded file */
   unsigned long n; 					
   
   /* File tag */
   unsigned char *t0, *t;
   int t_size; 

   /* Computing the size of file tag which itself is stored in the file "tag" */
   struct stat results;	 
   stat("tag", &results);
   t_size=results.st_size;  

   t = (unsigned char*)malloc(t_size);
 
   fstream fin;
  
   fin.open("tag",ios::in|ios::binary);

   if (!fin)
   {
      cout<<"Cannot open file.\n";
      exit(0);
   }

   /* Reading and storing the file tag in 't[]' */
   for(i=0;i<t_size;i++)
    t[i] = fin.get();

   fin.close();
   /* to store the hash(MAC) obtained from file tag and the hash computed
      from a part of tag */ 
   unsigned char md1[20],md2[20];

   int t0_size = t_size-20;

   AES_KEY key_de;
  
   mpz_t n1;
   mpz_init(n1);

   for(i=0;i<s;i++)
    mpz_init(al[i]);

   /* parameter to HMAC function specifying the hash function as SHA1 */
   const EVP_MD *md;
   md = EVP_sha1();

   t0 = (unsigned char*)malloc(t0_size);
   
   for(i=0;i<t0_size;i++)
     t0[i] = t[i];

   /* store the MAC on t0 from file tag into md1 */
   for(j=i;j<t_size;j++)
     md1[j-i]=t[j];

   /* HMAC function is defined in openssl library  */
   /* MAC on t0 is computed and stored in md2 */
   HMAC(md,k_m,32,t0,t_size-20,md2,NULL);

   for(i=0;i<20;i++)
   {
      if(md1[i]!=md2[i])
      {
          cout<<"MAC on file tag invalid!!!"<<endl;
          exit(0); 
      }
   }

   unsigned char temp[20];

   /* input and output from the Decryption function(AES in CBC mode) to extract 
      the PRF key and the alphas */
   unsigned char *in, *out;

   /* for stoing an alpha extracted from tag */
   unsigned char alpha[p_size];

   /* Extracting n from tag. n is stored as string of digits in base 10.
      Fixed 20 bytes are reserved for it */
   for(i=0;i<20;i++)
    temp[i]=t0[i];

   /* creates a gmp integer from the string of digits in temp */
   mpz_set_str(n1,(char*)temp,10);
   n = mpz_get_ui(n1);

   int buflen = t0_size-20;

   /* allocating memory for the encrypted data in t0. t_size is the total length
      of tag in which the last 20 bytes (160 bits as the hash function is SHA1) 
      contains the MAC(t0). Out of the remaining bytes the first 20 are reserved 
      for n. The remaining bytes contain encrypted data */
   in = (unsigned char*)malloc(buflen);  
  
   for(i=20;i<t0_size;i++)
     in[i-20] = t0[i];
   memcpy(iv, "%^GJSDFWEFFGdiuew4", 16);
   out= (unsigned char*)malloc(buflen);
    
   /* Initializing the decryption key */
   AES_set_decrypt_key(k_e,256,&key_de);

   AES_cbc_encrypt(in,out,buflen,&key_de,iv,0);

   for(i=0;i<32;i++)
     key_prf[i] = out[i];

   k=32;
   for(i=0;i<s;i++)
   {
        for(j=0;j<p_size;j++)
          alpha[j] = out[k++];
        mpz_import(al[i],p_size,1,1,0,0,alpha);
   }

   free(in);
   free(out);

   unsigned long l = challenge(n, numIndic);
   return l;
}


int verify(mpz_t meu[], unsigned long l, mpz_t *sigma)
{
   mpz_t cph, temp1, temp2, si, index;

   mpz_init(cph);
   mpz_init(temp1);
   mpz_init(temp2);
   mpz_init(si);
   mpz_init(index);

   int i,j;

   char input[16];

   unsigned char cipher[16]; 

   for(i=0;i<l;i++)
   {
     mpz_set_ui(index,indices[i]);

     /* converting the index i into a 16 byte(128 bits) message for AES(PRF) input by
        converting the decimal integer into base 16(hexadecimal) number string.
        Index is an unsigned long integer, max size 64 bits = 16 hexadecimal
        characters */
     mpz_get_str(input,16,index);   

     int len=strlen(input);

     /* If the length of converted string is less than 16, pad it with zeros */
     for(int j=len;j<16;j++)
      input[j]='0';


     AES_KEY key_prf_en;

     AES_set_encrypt_key(key_prf,256,&key_prf_en);    

     AES_encrypt((unsigned char*)input,cipher,&key_prf_en);

     /* Imports the character array 'cipher' into a gmp integer 'cph' by reading 16 words
        of 1 byte each(characters) from the array */
     mpz_import(cph,16,1,1,0,0,cipher);     

     /* temp1 = temp1 + coeff[i]*cph */
     mpz_addmul(temp1,coeff[i],cph);

     /* temp1 = temp1 % prime */
     mpz_mod(temp1,temp1,prime);

   }

   for(j=0;j<s;j++)
   {
     /* temp2 = temp2 + al[j]*meu[j] */
     mpz_addmul(temp2,al[j],meu[j]);

     /* temp2 = temp2 % prime */
     mpz_mod(temp2,temp2,prime);
   }

   mpz_add(si,temp1,temp2);
   mpz_mod(si,si,prime);

   mpz_set(*sigma, si);
   return 0;
}

  

  
