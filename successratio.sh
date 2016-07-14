#!/bin/sh
: > count.txt
size=100
tmp=0
for i in `seq 1 $size`; do
  tmp=$(($tmp+1))
  if [ $tmp -gt 4 ]; then
    wait
    tmp=1
    # echo `cat count.txt | grep -o 0 | wc -l` '/' `cat count.txt | wc -l`
  fi
  (./cMonteCalro2048 $1 $2; echo $? >> count.txt) &
done
wait
echo $1 $2 `cat count.txt | grep -o 0 | wc -l` '/' $size
rm -rf count.txt
