#!/bin/bash
#
TIMEFORMAT=%3lR
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
pushd $SCRIPTPATH > /dev/null

# src="src/*.cpp"
# opt="-g -o tafel -lcurl"

# if [ $HOSTNAME != rapi ]
# then
# else
#     src="$src src/displays/waveshare_2.9inch_epaper/bin/*.o"
#     opt="$opt -D ON_RASPBERRY -lbcm2835 -lm"
# fi

# time clang++ $src $opt || exit 1


if [ $HOSTNAME != rapi ]
then
    echo "Not running on raspberry"
    make || exit 1
    time ./tafel
else
    echo "Running on raspberry"
    echo "building waveshare"
    bash src/displays/waveshare_2.9inch_epaper/build.sh
    echo "building tafel"
    make || exit 1
fi

popd > /dev/null
unset TIMEFORMAT
