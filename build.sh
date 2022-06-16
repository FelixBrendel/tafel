#!/bin/bash
#
TIMEFORMAT=%3lR
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
pushd $SCRIPTPATH > /dev/null

time clang++ -g -o tafel src/*.cpp -lcurl || exit 1

# time valgrind --leak-check=full ./tafel
time ./tafel

popd > /dev/null
unset TIMEFORMAT
