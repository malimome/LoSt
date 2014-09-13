#!/bin/bash
cat $1 | while read node
do
  echo ${node}
  unode="ucalgary_nashad2@${node}"
  ssh -i id_rsa $unode 'ls'
done


