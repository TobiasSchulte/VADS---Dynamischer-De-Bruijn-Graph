#!/bin/bash


for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 40
done

for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 60
done

for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 80
done

for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 100
done