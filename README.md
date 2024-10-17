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

## Features
- Two versions C and Python
    - Both contain the very basic implementation of the interpolating methods.
- Python is the only one that can do the following:
    - Open/close SVG files
    - Extract the path details
    - Generates SVG files for each frame of the morphing animation based on the path details.
    - Basic linear interpolation between two shapes.
    - Avoids overwriting files if they already exist.
    - Display results (testing).


## Project Structure
1) src - contains all source code in C and python
    1a) C contains the very basics of interpolating methods.
    1b) Python contains the actual image morph that extracts the svg files and uses interpolation method to morph a circle shape into a triangle.

## Current Issues
1) morph2.py cannot do complex shapes yet.
2) displaying results is a mock up and has nothing to do with the svg files.
3) parallelization hasn't been implemented.
4) I tried to use xml library in C but was going into a lot of issues in regards to extracting path details from svg which is why I changed to python for the actual morphing (morph2.py).
5) We only have less than 2 weeks to finish :(

