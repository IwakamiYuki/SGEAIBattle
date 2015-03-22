#!/bin/sh
i=1
while [ $i -le 100 ];
do
  echo "round $i" >&2
  # ./run.sh
  ./start.sh
  i=`expr $i + 1`
done

