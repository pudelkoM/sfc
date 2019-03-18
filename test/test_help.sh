#!/bin/bash

$SFC --help > /dev/null
if [[ "$?" != 1 ]]; then
    echo "Should return exit code 1"
    exit 1
fi