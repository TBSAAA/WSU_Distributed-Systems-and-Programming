#!/bin/bash

for ((i=1; i<=10; i++))
do
	for ((j=1; j<=10; j++))
	do
		for ((k=1; k<=10; k++))
		do
			((surface = 2*(i*j+j*k+k*i)))
			((volume = i*j*k))
			echo "length: "$i ", width: "$j ", height: "$k
			echo "reall surface: "$surface ", reall volume: "$volume
			./box_3_client 137.154.149.28 $i $j $k 1 | egrep 'surface|Volume'
			echo " "
		done
	done
done