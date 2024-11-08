# Image Morphing

This project demonstrates a simple **image tweening algorithm** written in C that morphs a small circle into a large circle by generating intermediate frames using **linear interpolation**. The project creates a series of SVG files to visualize the morphing process.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Current Issues](#current-issues)

## Overview
This is a basic project that showcases how to perform image morphing between two geometric shapes. The project generates a series of SVG files representing each step of the morphing process.

You can run this project to see a smooth transition from a small circle to a larger one, with all the intermediate shapes saved as separate SVG files in c.

## How to run (based on the files of the presentation folder)
Used programs and Libraries to consider
1) C and Python
2) OpenMP (Parallelizing): sudo apt-get install libomp-dev    
3) LibXML (Extracting SVG coordinates): sudo apt-get install libxml2 libxml2-dev

## Features
- Two versions C and Python of Sequential Version
    - Both contain the very basic implementation of the interpolating methods.
    - Wayne's version was the two lines coming together (python).
    - We covered the basics of interpolating methods using basic fixed shape points- evolved to using complex fixed shape         polygon and eventually we were able to extract the SVG coordinates and use the bezier interpolation method to do the         circle to triangle morphing.  


## Project Structure
1) imagetweening/src/c: contains the understanding of the how the program works but doing small versions of the project.
2) imagetweening/src/presentation: contains the finished product of circle to triangle with extracting of points, read and      writing svg files, and calculating interpolating points.

## how It Works
1) 1.c: to understand how to generate a frame with respect to the target svg from source svg, we must understand the basics of interpolating. Interpolating is finding an in-between value depending on a factor of 't' where it is from 0 to 1 (think of 0 as the starting and 1 the ending). In 1.c soruce code, we start with the source and have an end point, with the help of the linear interpolation formula, we can calculate the result with respect to t (remember t is the value between 0 - 1).
2) 2.c: a more advance use of the interpolation method is creating an object, we used x and y coordinates. With the previous knowledge, we calculate the results for both x and y coordinates which gives us our t value.
3) 3.c: now we move onto 2d-coordinates of a small square and large square, same idea. We calculate their separate coordinates and it gives us their respective t value results.
4) 4.c: lets apply the knowledge from the previous intermediate methods but now we wish to generate an SVG file for the given results of t. We don't extract anything, we have prewritten circle(s) coordinates wihtin a method.
5) 5.c: we want to morph a small circle svg to a large circle svg, so we import libxml libraries to extract the path details of said svg files, traverse through the svg to find circle element and extract the details. We get those coordinates and place it into our morph method that we created, we then write (generate) the corresponding svg file for the current frame.
6) 6.c: a triangle to circle svg- instead of using a basic interpolation linear method we will use the bezier method which is defined by 3 points, starting, middle, and ending point.
7) presentation/all the files: is the finished product that has a lot of morphing mock up from source to target. 

## Current Issues
1) Complex shapes: to many points to manually program- almost impossible to program.
2) Display: in python, the gui is buggy and can't display the shapes properly but to view the shapes, you can manually visit the output folder and it will show case all the frames created during the sequential/parallel version.

## Code from 3rd Party
1) Ivan & Wayne: The Formula's with cos and sin to get the interpolating_points. We couldn't understand how to fully utuilize the bezier method without have a GPT generated formula to get those points.
2) Josue & Steven: Pragma: we didn't fully understand the approach to utilize parallel since we have a bottleneck issue of I/O reading and writing till GPT gave examples to use whether it was CUDA, pthreads, and openMP. We utilized the information gathered from GPT.

