#!/bin/bash

set -e
LARGE_INPUT=$(mktemp)
trap "rm $LARGE_INPUT" EXIT
SIZE=$(expr 1024 \* 1024 \* 16) # 16M
head -c $SIZE /dev/urandom > $LARGE_INPUT
$SFC --pw "foo" e < $LARGE_INPUT > output0.bin
$SFC --pw "foo" d < output0.bin > back0.bin
cmp $LARGE_INPUT back0.bin
