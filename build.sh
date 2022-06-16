#!/bin/bash
#
TIMEFORMAT=%3lR
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
pushd $SCRIPTPATH > /dev/null

bash src/displays/waveshare_2.9inch_epaper/build.sh 

time clang++ -g -o tafel src/*.cpp src/displays/waveshare_2.9inch_epaper/bin/*.o -lbcm2835 -lm  -lcurl || exit 1

# time valgrind --leak-check=full ./tafel
time ./tafel

popd > /dev/null
unset TIMEFORMAT
