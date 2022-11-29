#!/bin/sh
autoreconf -is
rm -rf interflop-stdlib
git submodule update --init --recursive
./install-stdlib.sh
