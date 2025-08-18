#!/bin/sh
clear
rm ./bin_pc/PBSimpleNewsFeed.app
cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=Linux
make
./bin_pc/PBSimpleNewsFeed.app
