#! /bin/bash

if [ -f ball_and_plate ] && [ -f kohonen.nn ] && [ -f ase.nn ]; then
	./ball_and_plate -k kohonen.nn -s ase.nn -r
else
	echo "Some files don't exist"
fi
