#!/bin/bash

cat $1 | grep "Phase" | awk '{print $1" "$2 $7 $9 $11}' > phase.csv
cat $1 | grep "Data: sync_mark" | awk '{print $1" "$2 $10 "," $13 "," $16}' > sync_mark.csv

TITLE=`basename $1 .csv`
DIRNAME=`dirname $1`
echo $DIRNAME

gnuplot -e "set term png size 1024,786" \
    -e "set output '$DIRNAME/$TITLE.png'" \
    -e "trace_title='$1'" plot.gp

rm phase.csv sync_mark.csv
