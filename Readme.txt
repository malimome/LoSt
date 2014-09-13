1. First put the hostname of the new planet lab nodes in nodelistsetup.txt file one in each line without space.
2. Run "./keySetup.sh nodelistsetup.txt" and prompt yes for the ssh authentication handshake. This ensures that during the setup phase no prompts are showed by the new planet lab nodes.
3. Run "./lostSetup.sh nodelistsetup.txt". This uploads necessary files and setup execution environment in the planet lab nodes. Make sure that lostServer and lostClient folders and location.cpp and clientSetup.sh files are in the current directory.
4. Step 2 and 3 have to be done only once for each planet lab node. 
5. After the setup is completed, put the hostname of the nodes on which to perform the experiment in "nodelist.txt" file each in one line without space.
6. Run "./lostExp.sh nodelist.txt" to start the experiment. Make sure that there is a directory named "output" in the current directory.
7. After the experiment is completed, all the output files will be found in the "output" directory.
