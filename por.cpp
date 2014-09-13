 /*******************************************************************************
 * por.cpp
 *
 *
 * This file contains four major functions performed by the client. These are
 * seperately defined in this file for better modularity of the software.
 * All these functions are called by the client() in the file "client.cpp".  
 * 
 * The functions defined here are: 
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
 *
 *******************************************************************************/

#include "ecc.h"     


/* global variable accessible throughout this file 
   Other files also access this variable through 
   external linakge */
mpz_t prime;               		 

/* to store dataword and codeword for encoding */
unsigned char msg[WORD_SIZE];
unsigned char codeword[WORD_SIZE + NPAR];
  

/* 'str' is the input file name and 'in_size' is the input file size */
void encode(const char *str, unsigned long in_size)
{  

  int count;

  /* loop variable */
  int i;
 
  unsigned long size = in_size;
 
  fstream fin,fout;
  
  fin.open(str,ios::in|ios::binary);

  if (!fin)
  {
      cout<<"Cannot open file.\n";
      exit(0);
  }
 
  fout.open("Encoded",ios::out|ios::binary); 

  char ch=fin.get();
  
  /* loop runs until whole of the file is read */
  while(size!=0)
  {
    count=0;
    for(i=0;i<WORD_SIZE && size!=0;i++)
    {      
          msg[i]=ch;
          count++;
          ch=fin.get();
          size--;
    }
 
    encode_data(msg, count, codeword);

    for(i=0;i<count+NPAR;i++)
    { 
       fout.put(codeword[i]);
    }
  }
  
  fout.close();
  fin.close();

}

void get_prime()
{
	const char *strprime = PRIME;
    mpz_init(prime);
    mpz_set_str(prime, strprime, 10);
}

void generate_prime()
{
   int i;
   char mpzBuff[1024] = {0};
 
   /* p is a large (LMBDA bits) prime no generated randomly on execution */
   mpz_t p; 

   char a[LMBDA + 1];
    
   mpz_init(p);
     
   srand(time(NULL));

   /* creating a random string of 0 and 1 of length LMBDA */
   for(i=0;i<LMBDA;i++)
   {     
       int t1=(rand()%2);

       if(t1==0)
         a[i]='0';
        else
         a[i]='1';   
   }

    a[LMBDA]='\0';

    /*converts the string of 0 and 1 in 'a' to an integer in base 10 and stores it in p */
    mpz_set_str(p,a,2); 

    /* sets p to next possible prime greater than p */
    mpz_nextprime(p,p);  

    mpz_get_str(mpzBuff, 10, p);
    cout<<"Prime generated: "<< mpzBuff<<endl<<endl;

    mpz_init(prime);
    mpz_set(prime,p);

}
  

void keygen(unsigned char key_enc[32], unsigned char key_mac[32])
{

    int i;
    srand(time(NULL));
    mpz_t k_enc, k_mac;
    mpz_t k,temp;

    mpz_init(k);
    mpz_init(k_enc);
    mpz_init(k_mac);

    mpz_init_set_ui(temp,rand());

    
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed(state,temp);

    /* GMP in-built random function. */
    /* Generates a random number between 0 and 2^256-1 */
    mpz_urandomb(k_enc,state,256);  
    mpz_urandomb(k_mac,state,256);  

    size_t count;

    /* converts the gmp integer 'k_enc' into an unsigned char array 'key_enc' by reading 
       words of 1 byte each from the integer */
    mpz_export(key_enc,&count,1,1,0,0,k_enc);

   /* if length of key<32 bytes, then pad it with some random bytes */
    while(count<32)                 
     key_enc[count++] = rand()%256;

    mpz_export(key_mac,&count,1,1,0,0,k_mac);

    while(count<32)
     key_mac[count++] = rand()%256;
}

unsigned long gen_authenticators(unsigned long en_size, unsigned char *k_e, unsigned char *k_m, unsigned char *iv_de)
{
    unsigned long no_of_blocks;

    mpz_t n;
    mpz_init(n);

    /* defined in main() in main.cpp */
    extern const int sec_size;
    //cout<<"sec size: "<<sec_size;
    //cout<<"\t s: "<<s;
    //cout<<"\t en_size: "<<en_size<<endl;
    	
    /* Calculating the number of blocks in the Encoded file */						
    if(en_size%(sec_size*s)==0)
     no_of_blocks=en_size/(sec_size*s);
    else 
     no_of_blocks=(en_size/(sec_size*s)) + 1;
   
    mpz_set_ui(n,no_of_blocks);


    int i,j,k;
    int v,w;

    mpz_t key,temp,iv,al[s];
	
    /* key for PRF (32 bytes = 256 bits) */
    unsigned char userkey[32];      
               	  
    unsigned char alpha[p_size];

    /* input to Encryption function (AES in CBC mode);s alphas(s*p_size) + PRF key(32 bytes) */
    unsigned char in[s*p_size + 32];            

    /* intialization vector for Encryption function*/
    unsigned char ivec_en[16];                         

    size_t size;
    
    int l=0, len_in=0, b=0;
    
    for(i=0;i<s;i++)
      mpz_init(al[i]);

    mpz_init(key);
    mpz_init(iv);
    mpz_init_set_ui(temp,rand());
  
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed(state,temp);

    /* randomly choosing key for PRF */
    mpz_urandomb(key,state,256);                 
    mpz_export(userkey,&size,1,1,0,0,key);

    while(size<32)
      userkey[size++] = rand()%256;

    for(i=0;i<32;i++)
      in[l++]=userkey[i];

    /* randomly generating 's' alpha. If the size of chosen alpha is less
       than p_size bytes then pad it with zeros at the beginning to obtain 
       the fixed size */
    for(i=0;i<s;i++)                           
    {
      mpz_urandomm(al[i],state,prime);
      mpz_export(alpha,&size,1,1,0,0,al[i]);
   
      if(size<p_size)
      {
        for(v=size-1,w=p_size-1;v>=0;v--,w--)
         alpha[w] = alpha[v];
        while(w>=0)
        {
         alpha[w]= 0;
         w--;
        }
      }

      for(j=0;j<p_size;j++)
       in[l++] = alpha[j];
  
    }
 
    //MOHSEN
    /* randomly generating an iv (16 bytes) for Enc function */
    //mpz_urandomb(iv,state,128);

    //mpz_export(ivec_en,&size,1,1,0,0,iv);

    /* if length of iv is less than 16 bytes then pad it with random bytes */
    //if(size<16)
    //  ivec_en[size++] = rand()%256;
 
    /* iv for decryption funtion which is same as the one for encryption as is
       passed to the client */
    //for(i=0;i<16;i++)
    //  iv_de[i]= ivec_en[i];
    memcpy(ivec_en, "%^GJSDFWEFFGdiuew4", 16);
    memcpy(iv_de, "%^GJSDFWEFFGdiuew4", 16);
    cout << iv_de << endl;
    //MOHSEN
          
    len_in = l;

    /* determining the length of cipher text that will be produced from Enc function 
       when input size is 'len_in' bytes */
    int rem = len_in % 16;

    if(rem!=0)
      b = len_in + (16-rem);
    else 
      b = len_in;

    long buflen = b;

    unsigned char *out, *t0, *tag;

    out = new unsigned char[buflen];

    /* 20 bytes for n, the no. of blocks stored as a string of decimal numbers
       (n being unsigned long has max size 64 bits = 20 decimal number )and 
       buflen bytes for encrypted message */
    t0 = new unsigned char[20 + buflen];          

    /* tag t = t0||MAC(t0). MAC used is HMAC with SHA1, which has message digest of
       length 160 bits = 20 bytes */
    tag = new unsigned char [20 + buflen + 20];  

    
    int t0_size = 20 + buflen;
    int t_size = 20 + buflen + 20;

    AES_KEY key_en;

    /* setting the encrption key */
    AES_set_encrypt_key(k_e,256,&key_en);
      
    AES_cbc_encrypt(in,out,buflen,&key_en,ivec_en,1);

    /* coverting 'n' into a string of digits in base 10 */  
    mpz_get_str((char*)t0,10,n);
    int z = strlen((char*)t0);

    /* if the length created string is less than 20 then fill
       it with NULL ('\0') characters */
    while(z<20)
     t0[z++] = '\0';
       
    for(i=0;i<buflen;i++)
     t0[20+i] = out[i];
 
    for(i=0;i<20+buflen;i++)
      tag[i]=t0[i];

    /* parameter to HMAC function specifying the hash function as SHA1 */
    const EVP_MD *md;
    md = EVP_sha1();

    /* Message digest size = 20 bytes (160 bits) for SHA1 */
    unsigned char md_value[20];  

    unsigned int md_len;

    HMAC(md,k_m,32,t0,t0_size,md_value,&md_len);

    for(i=0;i<md_len;i++)
     tag[t0_size+i] = md_value[i];

    fstream fout;
  
    fout.open("tag",ios::out|ios::binary);

    if (!fout)
    {
         cout<<"Cannot open file to store tag\n";
         exit(0);
    }
    
    /* write the tag in the file "tag" */
    for(i=0;i<40 + buflen;i++)
        fout.put(tag[i]);

    fout.close();

    FILE *fp; 				
    /* to store a message sector of length 'sec_size' bytes from the Encoded file */
    unsigned char msg[sec_size];        

    AES_KEY key_prf_en;
  
    AES_set_encrypt_key(userkey,256,&key_prf_en);

    /* Input to AES which should be 16 bytes (128 bits) */
    char input[16];  						

    fstream fin("Encoded",ios::binary|ios::in);
    if(!fin)
    {
        cout<<"Error reading file!!!!!\n";
        exit(0);
    }
        
    if ((fp = fopen("Authenticators","wb"))==NULL) 
    { 
       cout<<"Cannot open file.\n";
       exit(0);
    }     
    

    unsigned long counter=0;   
    mpz_t msg_sect,au; 
    mpz_t c,cph;

    unsigned char buf[p_size];
    size_t cnt=0;
  
       
    mpz_init(msg_sect);
    mpz_init(au);
    mpz_init(c);
    mpz_init(cph);
    
    int flag = en_size;
   
    for(counter=0;counter<no_of_blocks;counter++)
    {
      
        mpz_set_ui(temp,0);
        for(j=0;j<s;j++)
        {
           /* Initializing the msg sector with 0 */
           for(k=0;k<sec_size;k++)
             msg[k] = 0;
           for(k=0;k<sec_size && flag!=0;k++)
           { 
              msg[k] = fin.get();
              flag--;
           }

           /* convert the message into an gmp integer */
           mpz_import(msg_sect,sec_size,1,1,0,0,msg); 

           /* temp = temp + (al[j] * msg_sect) */
           mpz_addmul(temp,al[j],msg_sect);  
       }
      
       mpz_set_ui(c,counter);   

       /* converting the index i into a 16 byte(128 bits) message for AES(PRF) input by
          converting the decimal integer into base 16(hexadecimal) number string.
          Index is an unsigned long integer, max size 64 bits = 16 hexadecimal
          characters */
       /* mpz_import can be used as a substitute*/
       mpz_get_str(input,16,c);   

       int len=strlen(input);

       /* If the length of converted string is less than 16, pad it with zeros */
       for(i=len;i<16;i++)        
         input[i]='0';
        
       unsigned char cipher[16];    

       AES_encrypt((unsigned char*)input,cipher,&key_prf_en);

       /* converting the cipher text (16 bytes) into an gmp integer */
       mpz_import(cph,16,1,1,0,0,cipher); 

       mpz_add(au,temp,cph);
       mpz_mod(au,au,prime);

       /* convert au from gmp integer into an unsigned char array for the 
          ease of storing it in file */
       mpz_export(buf,&cnt,1,1,0,0,au);

       if(cnt<p_size)
       {
           for(v=cnt-1,w=p_size-1;v>=0;v--,w--)
             buf[w] = buf[v];
              while(w>=0)
              {
                buf[w]= 0;
                w--;
              }
        }

        fwrite(buf,1,p_size,fp);                    
       
      }

      fclose(fp);
      fin.close();

      return no_of_blocks;
}

/* RS Decoding routine. It can be used when extract() is implemented */

/*
void decode(unsigned long en_size)
{
  int i;
  int erasures[16];
  int nerasures = 0;

  int ML;
  unsigned long x = en_size;

  unsigned char c;

  fstream fin,fout;

  fin.open("Encoded",ios::in|ios::binary); 
  
  if(!fin)
  {
    cout<<"Cannot open file!!!\n";
    exit(0);
  }

  fout.open("Output",ios::out|ios::binary);
  if(!fout)
  {
    cout<<"Cannot open file!!!\n";
    exit(0); 
  }

   while(x!=0)
   {
     ML=0;
     for(int j=0;j<(WORD_SIZE + NPAR) && x!=0;j++)  
     {
       c=fin.get();
       codeword[j]=c;
       ML++;
       x--;
    }
 
    decode_data(codeword, ML);


    int flag=check_syndrome();

    if(flag==0)
    {
   //  cout<<"No error occured...";
     for(i=0;i<ML-NPAR;i++)
      fout.put(codeword[i]);
    }
    else
    {
    // cout<<"Error occured!!!"<<endl;
     correct_errors_erasures (codeword, ML, nerasures, erasures);
   
     for(i=0;i<ML-NPAR;i++)
      fout.put(codeword[i]);
    }
 }
 fin.close();
 fout.close();

}
*/

