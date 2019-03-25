#!/bin/bash

set -e
$SFC --pw "asdf" e < input0.bin > output0.bin

# modify byte
head -c 100 output0.bin > temp.bin
printf "\x123" >> temp.bin
tail -c +101 output0.bin >> temp.bin
mv temp.bin output0.bin

if $SFC --pw "asdf" d < output0.bin > back0.bin; then
    echo "Did not detect corruption"
    exit 1
fi
