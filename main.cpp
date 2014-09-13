 /*******************************************************************************
 * main.cpp
 *
 *
 * This file contains the main() which carries out the communication betwwen 
 * the client and the server as per the POR protocol.
 *
 * First the function client() present in the client module(client.cpp) is 
 * called. The client() internally calls several other functions to generate 
 * keys, encode the file, generate the file tag and authenticators.
 *
 * The main() next calls the verifier() also defined in the client module (as 
 * the client is the verifier in private verification). The verifier() verifies
 * the file tag and then creates a challenge for the server.
 *
 * Next, the function server() in the server module(server.cpp) is invoked which 
 * generates a response to the client's challenge. 
 *
 * Finally, the function verify() defined in the client module is called which 
 * verifies the server's response. 
 *
 * Decoding function can further be used if one wants to extract the original 
 * file.

 *******************************************************************************/



#include "ecc.h"

/* size of sector is one byte less than that of prime thereby automatically 
   making each sector an element of Zp */
extern const int sec_size = (p_size)-1; 

main()
{
 
    /* no of challenges in the challenge set */
    unsigned l;                        		

    bool flag;

    cout<<"........................................................Client module running..........................................................."<<endl<<endl;

    client();

    cout<<endl;

    cout<<".........................................................Client as verifier............................................................."<<endl<<endl;

    l = verifier();

    cout<<endl;
 
    cout<<"....................................................... Server module running............................................................"<<endl<<endl;
    
    server(l);

    cout<<"..............................................Client verifying the response from the server..............................................."<<endl<<endl;

    flag=verify(l);

    if(flag)
     cout<<"Data verified...."<<endl<<endl;
    else
     cout<<"Data lost...."<<endl<<endl;

}   
