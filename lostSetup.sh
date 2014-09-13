#!/bin/bash
cat $1 | while read node
do
  unode="ucalgary_nashad2@${node}"
  ssh -n -i id_rsa $unode 'rm -rf ~/LoST'
  res=`ssh -n -i id_rsa $unode "uname -m"`
  if [ $res == 'x86_64' ]; 
  then 
    scp -i id_rsa -r LoST64 "$unode:~/LoST"
  else 
    scp -i id_rsa -r LoST32 "$unode:~/LoST"
  fi
  echo "processing of node ${node} $res is completed"
done

