#!/bin/bash
set -e

if [ ! -d "limine" ]; then
    git clone https://codeberg.org/Limine/Limine.git limine --branch=v10.x-binary --depth=1
    make -C limine
fi
