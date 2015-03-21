#!/bin/sh
i=1
while [ $i -le 30 ];
do
  echo "round $i" >&2
  ./run.sh
  i=`expr $i + 1`
done

