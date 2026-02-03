#!/bin/sh

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 [debug|release]"
    exit 1
fi

case "$1" in
    debug)
        BUILD_TYPE=Debug
        ;;
    release)
        BUILD_TYPE=Release
        ;;
    *)
        echo "Invalid build type: $1"
        echo "Expected: debug or release"
        exit 1
        ;;
esac

EXE="build/${BUILD_TYPE}/main"

ASAN_OPTIONS="detect_leaks=1" LSAN_OPTIONS="suppressions=asan.supp" $EXE
