import os
import xml.etree.ElementTree as ET
import math
import webcolors

# Function to get the project root directory
def get_project_root():
    return os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def color_to_hex(color):
    if color.startswith('#'):
        return color.upper()  # It's already a hex value, just ensure it's uppercase
    try:
        return webcolors.name_to_hex(color.lower())
    except ValueError:
        print(f"Warning: Unknown color name '{color}'. Keeping as is.")
        return color

# def extract_line_info(svg_content):
#     # Parse the SVG content
#     root = ET.fromstring(svg_content)
    
#     # Initialize an empty list to store line information
#     lines = []
    
#     # Extract line attributes
#     for elem in root.iter():
#         if elem.tag.endswith('line'):
#             x1 = elem.attrib.get('x1')
#             y1 = elem.attrib.get('y1')
#             x2 = elem.attrib.get('x2')
#             y2 = elem.attrib.get('y2')
            
#             if x1 and y1 and x2 and y2:
#                 lines.append({
#                     'x1': float(x1),
#                     'y1': float(y1),
#                     'x2': float(x2),
#                     'y2': float(y2)
#                 })
    
#     return lines


def extract_line_info(svg_source):
    # Check if the input is a file path or SVG content
    if os.path.isfile(svg_source):
        # If it's a file, read its content
        with open(svg_source, 'r') as file:
            svg_content = file.read()
        root = ET.fromstring(svg_content)
    else:
        # If it's not a file, assume it's SVG content
        root = ET.fromstring(svg_source)
    
    # Initialize an empty list to store line information
    lines = []
    
    # Extract line attributes
    for elem in root.iter():
        if elem.tag.endswith('line'):
            x1 = elem.attrib.get('x1')
            y1 = elem.attrib.get('y1')
            x2 = elem.attrib.get('x2')
            y2 = elem.attrib.get('y2')
            stroke = elem.attrib.get('stroke')
            strokeWidth = elem.attrib.get('stroke-width')

            
            if x1 and y1 and x2 and y2:
                lines.append({
                    'x1': float(x1),
                    'y1': float(y1),
                    'x2': float(x2),
                    'y2': float(y2),
                    'stroke': color_to_hex(elem.attrib.get('stroke', '#000000')),
                    'strokeWidth': strokeWidth
                })
    
    return lines

def equalize_line_counts(startsvg, finalsvg):
    start_count = len(startsvg)
    final_count = len(finalsvg)
    
    if start_count == final_count:
        return startsvg, finalsvg
    
    if start_count > final_count:
        # We need to split lines in finalsvg
        ratio = start_count / final_count
        new_finalsvg = []
        for line in finalsvg:
            split_lines = split_line(line, ratio)
            new_finalsvg.extend(split_lines)
        return startsvg, new_finalsvg
    else:
        # We need to split lines in startsvg
        ratio = final_count / start_count
        new_startsvg = []
        for line in startsvg:
            split_lines = split_line(line, ratio)
            new_startsvg.extend(split_lines)
        return new_startsvg, finalsvg

def split_line(line, ratio):
    num_parts = int(ratio)
    split_lines = []
    for i in range(num_parts):
        t1 = i / num_parts
        t2 = (i + 1) / num_parts
        new_line = {
            'x1': line['x1'] + (line['x2'] - line['x1']) * t1,
            'y1': line['y1'] + (line['y2'] - line['y1']) * t1,
            'x2': line['x1'] + (line['x2'] - line['x1']) * t2,
            'y2': line['y1'] + (line['y2'] - line['y1']) * t2,
            'stroke': line['stroke'],
            'strokeWidth': line['strokeWidth']
        }
        split_lines.append(new_line)
    return split_lines


def hex_to_rgb(hex_color):
    hex_color = hex_color.lstrip('#')
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

def rgb_to_hex(rgb):
    return '#{:02x}{:02x}{:02x}'.format(int(rgb[0]), int(rgb[1]), int(rgb[2]))

def interpolate_color(color1, color2, t):
    rgb1 = hex_to_rgb(color1)
    rgb2 = hex_to_rgb(color2)
    new_rgb = tuple(int(rgb1[i] + (rgb2[i] - rgb1[i]) * t) for i in range(3))
    return rgb_to_hex(new_rgb)


def generate_interpolated_svgs(startsvg, finalsvg, number_of_steps=100, output_file_name_base="frame", output_directory=None):
    if output_directory is None:
        # Use the default ../frames_generated relative to the script location
        script_dir = os.path.dirname(os.path.abspath(__file__))
        output_directory = os.path.abspath(os.path.join(script_dir, "..", "frames_generated"))
    
    # Ensure the output directory exists
    os.makedirs(output_directory, exist_ok=True)
    
    for step in range(number_of_steps + 1):  # +1 to include the final state
        t = step / number_of_steps  # Interpolation factor (0.0 to 1.0)
        
        # Interpolate between startsvg and finalsvg
        interpolated_svg = []
        for start_line, final_line in zip(startsvg, finalsvg):
            # Calculate the stroke width
            start_width = int(start_line['strokeWidth'])
            final_width = int(final_line['strokeWidth'])
            if start_width == final_width:
                current_width = start_width
            else:
                current_width = round(start_width + (final_width - start_width) * t)

            # Handle stroke color
            start_color = start_line['stroke']
            final_color = final_line['stroke']
            if start_color == final_color:
                current_color = start_color
            else:
                current_color = interpolate_color(start_color, final_color, t)

            interpolated_line = {
                'x1': start_line['x1'] + (final_line['x1'] - start_line['x1']) * t,
                'y1': start_line['y1'] + (final_line['y1'] - start_line['y1']) * t,
                'x2': start_line['x2'] + (final_line['x2'] - start_line['x2']) * t,
                'y2': start_line['y2'] + (final_line['y2'] - start_line['y2']) * t,
                'stroke': current_color,
                'strokeWidth': current_width
            }
            interpolated_svg.append(interpolated_line)
        
        # Generate SVG content
        svg_content = generate_svg_content(interpolated_svg)
        
        # Write SVG file
        filename = f"{output_file_name_base}_{step:03d}.svg"
        filepath = os.path.join(output_directory, filename)
        with open(filepath, 'w') as f:
            f.write(svg_content)
        
        #print(f"Generated: {filepath}")

def generate_svg_content(lines):
    svg = ET.Element('svg', {
        'xmlns': 'http://www.w3.org/2000/svg',
        'version': '1.1',
        'width': '200',
        'height': '200'
    })
    
    for line in lines:
        ET.SubElement(svg, 'line', {
            'x1': str(round(line['x1'])),
            'y1': str(round(line['y1'])),
            'x2': str(round(line['x2'])),
            'y2': str(round(line['y2'])),
            'stroke': line['stroke'],
            'stroke-width': str(line['strokeWidth'])
        })
    
    return ET.tostring(svg, encoding='unicode')

# Function to perform linear interpolation between two values
def interpolate(start, end, t):
    return start + t * (end - start)

# Function to write the interpolated shape to an SVG file
def write_svg(interpolated_points, frame_number):
    # Get the project root directory
    project_root = get_project_root()
    
    # Ensure the output directory exists
    output_dir = os.path.join(project_root, "frames_generated")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    # Create the filename for the SVG file
    filename = os.path.join(output_dir, f"frame_{frame_number:03d}.svg")
    
    # Write the SVG content for the interpolated shape
    with open(filename, "w") as file:
        file.write(f"<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n")
        file.write(f"  <polygon points='{interpolated_points}' fill='blue' />\n")
        file.write("</svg>\n")
    print(f"File {filename} created successfully.")

# Main function to perform the morphing and generate SVG frames
def main():
    # Get the project root directory
    project_root = get_project_root()
    print(project_root)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.abspath(os.path.join(script_dir, "..", "..", "frames_generated"))
    # Extract circle data from the input SVG file
    
    startsvg = extract_line_info(os.path.join(project_root, 'svg', 'tc_2_start.svg'))
    #startsvg = extract_line_info(os.path.join(project_root, 'svg', 'tc_x.svg'))
    print(f"Number of lines: {len(startsvg)}")
    for i, line in enumerate(startsvg, 1):
        print(f"Line {i}: ({line['x1']}, {line['y1']}) to ({line['x2']}, {line['y2']}), stroke ({line['stroke']}), stroke-width ({line['strokeWidth']})")
    
    finalsvg = extract_line_info(os.path.join(project_root, 'svg', 'tc_2_final.svg'))
    #finalsvg = extract_line_info(os.path.join(project_root, 'svg', 'tc_x.svg'))
    print(f"Number of lines: {len(finalsvg)}")
    for i, line in enumerate(finalsvg, 1):
        print(f"Line {i}: ({line['x1']}, {line['y1']}) to ({line['x2']}, {line['y2']}), stroke ({line['stroke']}), stroke-width ({line['strokeWidth']})")
    #triangle_points = extract_triangle_info(os.path.join(project_root, 'svg', 'tc_1_final.svg'))
    
##wjr
    ##return
    # Ensure both shapes were successfully read
    if not startsvg or not finalsvg:
        print("Error: Could not read shape data from SVG files.")
        return
    
    if startsvg == finalsvg:
        print("The start and final SVGs have identical contents.")
    else:
        print("The start and final SVGs have different contents.")
    
    
    start_line_count = len(startsvg)
    final_line_count = len(finalsvg)

    print(f"Number of lines in start SVG: {start_line_count}")
    print(f"Number of lines in final SVG: {final_line_count}")

    if start_line_count != final_line_count:
        print("The number of lines in start and final SVGs are not equal.")
        print("Equalizing line counts...")
        startsvg, finalsvg = equalize_line_counts(startsvg, finalsvg)
        print(f"After equalization: Start SVG has {len(startsvg)} lines, Final SVG has {len(finalsvg)} lines.")

    # Print contents of startsvg and finalsvg for verification
    print("\nContents of startsvg:")
    for i, line in enumerate(startsvg, 1):
        print(f"Line {i}: {line}")

    print("\nContents of finalsvg:")
    for i, line in enumerate(finalsvg, 1):
        print(f"Line {i}: {line}")




    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.abspath(os.path.join(script_dir, "..", "..", "frames_generated"))
    generate_interpolated_svgs(startsvg, finalsvg, 100, "tc2", output_dir)

    return




    # Circle center and radius
    cx, cy, r = small_circle
    
    # Extract triangle vertices
    triangle_vertices = triangle_points
    
    # Number of points to start with on the circle
    num_circle_points = 30
    
    # Loop to generate frames from t = 0 to t = 1, with 0.01 increments for smooth transitions
    for frame in range(101):
        t = frame / 100.0  # Interpolation factor (0.00 to 1.00)
        
        # Calculate interpolated points between circle and triangle vertices
        interpolated_points = []
        for i in range(num_circle_points):
            # Calculate the initial point on the circle's circumference
            angle = (2 * math.pi / num_circle_points) * i
            circle_x = cx + r * math.cos(angle)
            circle_y = cy + r * math.sin(angle)
            
            # Determine which triangle vertex this point will move towards
            triangle_vertex = triangle_vertices[i % 3]
            tx, ty = triangle_vertex
            
            # Interpolate between the circle point and the triangle vertex
            interp_x = interpolate(circle_x, tx, t)
            interp_y = interpolate(circle_y, ty, t)
            interpolated_points.append(f"{interp_x},{interp_y}")
        
        # Create the interpolated points string for the polygon
        interpolated_points_str = " ".join(interpolated_points)
        
        # Generate the corresponding SVG file for the current frame
        write_svg(interpolated_points_str, frame)

if __name__ == "__main__":
    main()