#! /bin/bash

if [ -f trainer ]
then
	if [ which time &> /dev/null ]; then
		./trainer -k kohonen.nn -c ace.nn -s ase.nn -r
	else
		time ./trainer -k kohonen.nn -c ace.nn -s ase.nn -r
	fi
else
	echo "The training program doesn't exist"
fi
