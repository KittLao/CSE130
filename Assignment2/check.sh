#!/bin/bash

#######################################################################
#
# Copyright (C) 2020 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

#> `who am i | awk '{print $1}'`

#speedup=2/3
speedup=2/5

echo "" 
tests=0
passes=0

cnt=64
> test.out
for f in {1..10}
do
  ./sort -m $cnt > sort.out
  if (( $? != 0 )) 
  then
    echo "    Functional: FAIL   (program error)" | tee sort.out
  else
    cat sort.out
  fi
  cat sort.out >> test.out
done
echo "" 
pass=`grep PASS test.out | wc -l`
fail=`grep FAIL test.out | wc -l`
(( total = 10 ))
(( tests += total ))
(( passes += pass ))
pct=0;
if (( total > 0 ))
then
  pct=`echo "scale=2; $pass / $total * 100.0" | bc -l`
fi
printf "%20s:  %2d/%2d  %5.1f%%\n" "Functional" $pass $total $pct
echo "" 

cnt=1000
> test.out
for f in {1..10}
do
  (time ./sort -s $cnt) &> sort.out
  fail=`grep FAIL sort.out | wc -l`
  if (( fail > 0 ))
  then
    head -1 sort.out
  else 
    ssecs=`tail -3 sort.out | head -1 | tr "m" "\n" | tail -1 | sed 's/.$//'`
    ./sort -m 128 > /dev/null
    if (( $? != 0 )) 
    then
        echo "       Speedup: FAIL   (program error)" | tee sort.out
    else 
        (time ./sort -m $cnt) &> sort.out
        fail=`grep FAIL sort.out | wc -l`
        if (( fail > 0 ))
        then
          head -1 sort.out
        else
          msecs=`tail -3 sort.out | head -1 | tr "m" "\n" | tail -1 | sed 's/.$//'`
          bar=`echo "scale=2; $ssecs * $speedup" | bc -l`
          spd=`echo "scale=2; $ssecs / $msecs" | bc -l`
          if [ 1 -eq "$(echo "${msecs} > ${bar}" | bc)" ]
          then
           echo "       Speedup: FAIL   ($spd times faster)" | tee sort.out
         else
           echo "       Speedup: PASS   ($spd times faster)" | tee sort.out
         fi
       fi
    fi
  fi
  cat sort.out >> test.out
done
echo "" 
pass=`grep PASS test.out | wc -l`
fail=`grep FAIL test.out | wc -l`
(( total = 10 ))
(( tests += total ))
(( passes += pass ))
pct=0;
if (( total > 0 ))
then
  pct=`echo "scale=2; $pass / $total * 100.0" | bc -l`
fi
printf "%20s:  %2d/%2d  %5.1f%%\n" "Non-Functional" $pass $total $pct
echo "" 


rm test.out
rm sort.out

pct=0
if (( tests > 0 ))
then
  pct=`echo "scale=2; $passes / $tests * 100.0" | bc -l`
fi
printf "%20s:  %2d/%2d  %5.1f%% of 90%%\n" "Tests" $passes $tests $pct
