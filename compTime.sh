#!/bin/bash
if [ $# -lt 3 ] 
then
  echo "Need 3 arguments but provided $#"
  echo "./lostExp.sh numOfBlocks numOfChallenges serverlist"
  exit 0
fi

echo "Computation times" > "time" 
port=9191
cat $3 | while read node
do
  echo ${node}
  echo -e "${node} \t \c" >> "time"
  unode="ucalgary_nashad2@${node}"
ssh -i id_rsa $unode >> "time" << ENDCMD
  cd /home/ucalgary_nashad2/LoST/server/ > /dev/null
  killall ./pserver &> /dev/null
  ./pserver $port | grep Average | tr -d "Average Computation Time:" & 
  cd ../
  ./pclient $1 $2 "localhost" $port &> /dev/null 
  cd server
  killall ./pserver &> /dev/null 
ENDCMD

done

