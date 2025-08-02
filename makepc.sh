#!/bin/sh
clear
rm ./bin_pc/simplenewsfeed.app
cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=Linux
make
./bin_pc/simplenewsfeed.app
