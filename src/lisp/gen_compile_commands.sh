#!/bin/sh

make clean
bear make

sed -i 's/compiler.c/compiler.dasc/g' compile_commands.json
