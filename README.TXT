
HOW TO USE

INIT Phase:
1- run the following
./keySetup.sh nodelist.txt
This is a one time process for every new node.

SETUP Phase:
1- run ./publish
./publish will create Authenticators, Encoded, KEYS, tag file based on a file FILE.TXT
Just put a file with name FILE.TXT there and run ./publish

2- run ./lostSetup.sh nodelist.txt

This phase is one time process unless you change the FILE.TXT or you change the client server code. If you changed FILE.TXT run both steps but if you just changed client server code, just run second step.

RUN Phase:
1- run
./lostExp numOfBlocks numOfChallenges serverlist clientlist [port]

numOfBlock is the number of blocks to be tested in file. If numOfBlocks is less than or equal to 0 then a small file (tempfile) will be read and sent by server to client.

numOfChallenges is the number of times that client sends a challenge to the server.

serverlist is the name of the file containing list of all servers in the system.

clientlist is the name of the file containing list of all clients in the system.
Note that these lists can be the same.

[port] is an optional port value which must be greater than 1024. By default, it is 9973. You don't need to change this unless you get a "Try to Connect" error.

To run the experiment:
1- For getting one line file 10 times: ./lostExp.sh -1 10 nodelist.txt nodelist.txt
2- To challenge the server 10 time asking for 10 blocks: ./lostExp.sh 10 10 nodelist.txt nodelist.txt
3- The same for 30 blocks: ./lostExp.sh 30 10 nodelist.txt nodelist.txt
4- The same for 50 blocks: ./lostExp.sh 50 10 nodelist.txt nodelist.txt
