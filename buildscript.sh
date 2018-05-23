#!/bin/bash

#clear the screen
clear

#compile the server and retreiver files
g++ Server.cpp -o server -lpthread
g++ Retreiver.cpp -o retreiver
