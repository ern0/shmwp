#!/bin/bash

g++ -g -O0 changer.cpp -o changer
gcc -std=c99 -g -O0 -D _SVID_SOURCE watcher.c -o watcher
