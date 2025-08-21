#!/bin/sh
clear
cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=Linux
make
./bin_pc/PBSimpleNewsFeed.app
