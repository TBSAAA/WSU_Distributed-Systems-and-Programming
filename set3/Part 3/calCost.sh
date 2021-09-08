#!/bin/bash

for ((i=1; i<=10; i++))
do
	for ((j=1; j<=10; j++))
	do
		./box_3_client 137.154.149.28 $j $i
		echo " "
	done
done