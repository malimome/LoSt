#!/bin/bash
if [ $# -lt 4 ] 
then
  echo "Need 4 arguments but provided $#"
  echo "./lostExp.sh numOfBlocks numOfChallenges serverlist clientlist [port]"
  exit 0
fi

if [ $# -lt 5 ]
then
  port=$5
else
  port=""
fi
nodes=( `cat "$3"` )
count=${#nodes[@]}
clnodes=( `cat "$4"` )
clcount=${#clnodes[@]}
echo "Total Server Nodes: $count, Total client nodes: $clcount"

echo "Data cleaning on clients starting ..."
clindex=0
while [ $clindex -lt $clcount ] 
do
  echo "Data cleaned on ${clnodes[$clindex]}"
  ssh -n -l ucalgary_nashad2 -i id_rsa ${clnodes[$clindex]} "rm ~/LoST/output/*"
  let clindex=$clindex+1
done
echo "Data cleaned #####"

destIndex=0
while [ $destIndex -lt $count ]
do
  echo "Processing for server ${nodes[$destindex]} Running ..."
  ssh -n -l ucalgary_nashad2 -i id_rsa ${nodes[$destIndex]} 'su -; killall pserver;'
  ssh -n -f -l ucalgary_nashad2 -i id_rsa ${nodes[$destIndex]} "cd /home/ucalgary_nashad2/LoST/server/; ./pserver $port &"
  clindex=0
  while [ $clindex -lt $clcount ] 
  do
    if [ "${nodes[$destIndex]}" !=  "${clnodes[$clindex]}" ]
    then
      params="cd /home/ucalgary_nashad2/LoST/; ./pclient $1 $2 ${nodes[$destIndex]} $port"
      echo $params
      ssh -n -l ucalgary_nashad2 -i id_rsa ${clnodes[$clindex]} "${params}"
      echo "Experiment completed for client ${clnodes[$clindex]}"
    fi
    let clindex=$clindex+1
  done
  echo "Stop Server for node ${nodes[$destIndex]} "
  ssh -n -l ucalgary_nashad2 -i id_rsa ${nodes[$destIndex]} 'su -; killall pserver;'
  let destIndex=$destIndex+1
done
echo "Experiment finished, collecting data back ..."

clindex=0
while [ $clindex -lt $clcount ] 
do
  echo "Data collecting from ${clnodes[$clindex]}"
  unode="ucalgary_nashad2@${clnodes[$clindex]}:~/LoST/output/* output/"
  echo "dest: $unode"
  scp -i id_rsa $unode
  let clindex=$clindex+1
done
echo "DATA Collection finished #####"

