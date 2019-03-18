#!/bin/bash

set -e
PWFILE=pw.txt
printf "foo" > $PWFILE
$SFC --pw-file $PWFILE e < input0.bin > output0.bin
$SFC --pw-file $PWFILE d < output0.bin > back0.bin
cmp input0.bin back0.bin
