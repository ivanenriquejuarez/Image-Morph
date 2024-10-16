#Image Morphing

This project demonstrates a simple **image tweening algorithm** written in C that morphs a small circle into a large circle by generating intermediate frames using **linear interpolation**. The project creates a series of SVG files to visualize the morphing process.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Usage](#usage)
- [Future Improvements](#future-improvements)
- [License](#license)

## Overview
This is a basic project that showcases how to perform image morphing between two geometric shapes (a small circle and a big circle). The project generates a series of SVG files representing each step of the morphing process.

You can run this project to see a smooth transition from a small circle to a larger one, with all the intermediate shapes saved as separate SVG files.

## Features
- Generates SVG files for each frame of the morphing animation
- Basic linear interpolation between two shapes
- Avoids overwriting files if they already exist
- Written in C for high performance
