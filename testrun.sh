#!/bin/bash

sed "s/#define NODES 100/#define NODES $1/" <./src/start.cpp >./src/startTestrun.cpp

g++ include/sha1.cpp src/startTestrun.cpp

chmod 755 a.out

./a.out >> testresult.txt
echo ---------------------------------------------  >> testresult.txt
