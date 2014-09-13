#!/bin/bash
if [ $# -ne 4 ] 
then
  echo "Need 4 arguments but provided $#"
  echo "./concatOutput.sh numOfBlocks numOfChallenges serverlist clientlist"
  exit 0
fi

numlines=`expr $2 - 1 `
dummyfile=""
for ((  num = 0 ;  num < numlines;  num++  ))
do
  dummyfile=$dummyfile"0\tdummy\n"
done
dummyfile=$dummyfile"0\tdummy"

cat $3 | while read servnode
do
  output="CAT$servnode.txt"
  echo -e "" > $output
  cat  $4 | while read clinode
  do
    dummy="Client: "$clinode"\nServer: "$servnode"\nBlocks: "$1"\nTime Elapsed\tData Verification\n"$dummyfile
    filename="output/$clinode-$servnode-$1"
    if ! [ -s "$filename" ]
    then
      echo -e $dummy >> $output
    else
      cat $filename >> $output
    fi
  done
done


