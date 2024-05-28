#!/bin/bash
make
if [[ $? == 0 ]]; then
    ./run
fi
