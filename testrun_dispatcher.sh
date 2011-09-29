#!/bin/bash

echo "#############################################" >> testresult.txt
echo "Testing with 40 Nodes"                         >> testresult.txt
echo "#############################################" >> testresult.txt
for i in {1..50}
do
 echo "Testrun $i"

 sh testrun.sh 40
done


echo "#############################################" >> testresult.txt
echo "Testing with 60 Nodes"                         >> testresult.txt
echo "#############################################" >> testresult.txt

for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 60
done



echo "#############################################" >> testresult.txt
echo "Testing with 80 Nodes"                         >> testresult.txt
echo "#############################################" >> testresult.txt

for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 80
done


echo "#############################################" >> testresult.txt
echo "Testing with 100 Nodes"                        >> testresult.txt
echo "#############################################" >> testresult.txt

for i in {1..50}
do
 echo "Testrun $i"
 sh testrun.sh 100
done