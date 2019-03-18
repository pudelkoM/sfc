#!/bin/bash

set -e
$SFC --pw "" e < input0.bin > output0.bin
if $SFC --pw "bar" d < output0.bin > back0.bin; then
    echo "Decryption with different pw should fail"
    exit 1;
fi
