#!/bin/bash
set -e

COMPILER=gcc

$COMPILER -o build -Wall -Wextra build.c
./build compiler:$COMPILER run
