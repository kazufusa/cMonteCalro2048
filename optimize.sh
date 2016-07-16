#!/bin/sh
for i in `seq 5 15`; do
  for j in `seq 100 100 1000`; do
    sh ./successratio.sh $i $j
    wait
  done
done
