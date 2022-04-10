TIMEFORMAT=%3lR
SCRIPTPATH="$( cd "$(dirname "$0")" ; pwd -P )"
pushd $SCRIPTPATH > /dev/null

clang++ -g -o tests ../xml.cpp ../net.cpp ../utf-8.cpp tests.cpp -lcurl || exit 1
time ./tests
