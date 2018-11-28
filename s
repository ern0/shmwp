#!/bin/bash
clear

PID=`ps -e | grep changer | cut -d" " -f 1`
./watcher $PID
