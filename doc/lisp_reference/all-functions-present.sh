#!/bin/sh

# Prints a list of the functions that are not yet documented.

../../src/lisp/lisp all-functions.lisp | while read -r fun
do
	if grep "$fun" lisp_reference.tex > /dev/null; then
		# nothing
		true
	else
		echo "$fun"
	fi
done
