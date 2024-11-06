import numpy as np
from scipy.spatial import Delaunay
from svgpathtools import svg2paths2, wsvg
import os

# Function to extract critical points from an SVG
def extract_points_from_svg(svg_filename, num_points=20):
    """Extract meaningful points from an SVG by sampling along the path."""
    paths, _, _ = svg2paths2(svg_filename)
    
    points = []
    for path in paths:
        # Get equally spaced points along the path
        length = path.length()
        for i in range(num_points):
            point = path.point(i / num_points)
            points.append([point.real, point.imag])
    
    return np.array(points)

def match_points(source_points, target_points):
    """Match the number of points between source and target by downsampling or adding points."""
    source_len = len(source_points)
    target_len = len(target_points)
    
    if source_len > target_len:
        # Downsample source to match target
        source_points = downsample_points(source_points, target_len)
    elif target_len > source_len:
        # Downsample target to match source
        target_points = downsample_points(target_points, source_len)
    
    return source_points, target_points

def downsample_points(points, target_length):
    """Reduce the number of points to match target_length."""
    indices = np.round(np.linspace(0, len(points) - 1, target_length)).astype(int)
    return points[indices]

# Interpolation function between two sets of points
def interpolate_points(source_points, target_points, alpha):
    """Interpolate the points linearly based on alpha (between 0 and 1)."""
    return (1 - alpha) * source_points + alpha * target_points

# Generate intermediate SVG frames
def generate_intermediate_svg(alpha, frame_number, output_dir, source_points, target_points, triangles):
    """Generate and save intermediate SVG for a given alpha (0 to 1)."""
    interpolated_points = interpolate_points(source_points, target_points, alpha)
    
    # Open an SVG file for writing
    frame_filename = os.path.join(output_dir, f'frame_{frame_number:03d}.svg')
    with open(frame_filename, 'w') as f:
        # Write the SVG header
        f.write('<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n')
        f.write('<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="800" height="800">\n')
        
        # Draw the triangulation lines
        for triangle in triangles:
            points = interpolated_points[triangle]
            f.write(f'<polygon points="{points[0][0]},{points[0][1]} {points[1][0]},{points[1][1]} {points[2][0]},{points[2][1]}" ')
            f.write('style="fill:none;stroke:black;stroke-width:1"/>\n')
        
        # Write the SVG footer
        f.write('</svg>\n')
    
    print(f"Saved frame {frame_number} as {frame_filename}")

# Morph between chef and donut images
def morph_images(chef_svg_file, donut_svg_file, steps=30, output_dir='./morph_frames_svg'):
    """Morph from chef to donut by generating and saving intermediate SVG files."""
    # Extract points from both SVGs
    chef_points = extract_points_from_svg(chef_svg_file, num_points=50)
    donut_points = extract_points_from_svg(donut_svg_file, num_points=50)
    
    # Ensure the number of points is the same in both
    chef_points, donut_points = match_points(chef_points, donut_points)
    
    # Create a Delaunay triangulation based on the chef points
    tri = Delaunay(chef_points)
    
    # Ensure output directory exists
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Generate and save frames
    for i, alpha in enumerate(np.linspace(0, 1, num=steps)):
        generate_intermediate_svg(alpha, i, output_dir, chef_points, donut_points, tri.simplices)

# Paths to SVG files
chef_svg_file = '../../svg/chef.svg'
donut_svg_file = '../../svg/donut.svg'

# Run the morphing process
morph_images(chef_svg_file, donut_svg_file, steps=30, output_dir='./morph_frames_svg')
