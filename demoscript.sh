#!/bin/bash

# clear the screen
clear 

# run server
./server &

# run retreiver tests 
./retreiver www.google.com
./retreiver 127.0.0.1/SecretFile.html
./retreiver 127.0.0.1/../../../etc/passwd
./retreiver 127.0.0.1/
./retreiver 127.0.0.1/validFile.txt

# the server process
killall server
