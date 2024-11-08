#!/bin/bash

# Compile and execute the sequential version
echo "Project 1: Team Image Tweening"
sleep 3
echo "Compiling and running the sequential version..."
sleep 5
gcc -o morph_animation_s circle-to-triangle.c $(xml2-config --cflags --libs) -lm
if [ $? -eq 0 ]; then
    echo "Sequential version compiled successfully. Running..."
	sleep 4
    ./morph_animation_s
    echo "Sequential version finished."
	echo "---------------------------------------------------------------"
	sleep 4
else
    echo "Failed to compile the sequential version."
    exit 1
fi

# Compile and execute the parallel version
echo "Compiling and running the parallel version..."
sleep 4
gcc -o morph_animation_p morph_c_to_tr_para_2.c -fopenmp $(xml2-config --cflags --libs) -lm
if [ $? -eq 0 ]; then
    echo "Parallel version compiled successfully. Running..."
	sleep 2
    ./morph_animation_p
    echo "Parallel version finished."
	echo "---------------------------------------------------------------"
	sleep 2 
else
    echo "Failed to compile the parallel version."
    exit 1
fi
