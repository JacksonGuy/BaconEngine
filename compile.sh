#!/bin/bash
make
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if [[ $? == 0 ]]; then
        ./run
    fi
else 
    if [[ $? == 0 ]]; then
        ./bin/run.exe
    fi
fi
