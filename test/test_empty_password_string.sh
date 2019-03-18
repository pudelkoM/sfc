#!/bin/bash

set -e
$SFC --pw "" e < input0.bin > output0.bin
$SFC --pw "" d < output0.bin > back0.bin
cmp input0.bin back0.bin
