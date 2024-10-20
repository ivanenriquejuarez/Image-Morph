import os
import xml.etree.ElementTree as ET
import math

# Function to extract circle attributes from an SVG file
def extract_circle_info(svg_file):
    # Parse the SVG file
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Extract circle attributes
    for elem in root.iter():
        if elem.tag.endswith('circle'):
            cx = elem.attrib.get('cx')
            cy = elem.attrib.get('cy')
            r = elem.attrib.get('r')
            if cx and cy and r:
                return float(cx), float(cy), float(r)
    
    return None

# Function to extract triangle points from an SVG file
def extract_triangle_info(svg_file):
    # Parse the SVG file
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Extract triangle points (from <polygon> element)
    for elem in root.iter():
        if elem.tag.endswith('polygon'):
            points = elem.attrib.get('points')
            if points:
                # Split the points string into tuples of (x, y)
                points_list = [tuple(map(float, p.split(','))) for p in points.split()]
                if len(points_list) == 3:
                    return points_list

    return None

# Function to perform linear interpolation between two values
def interpolate(start, end, t):
    return start + t * (end - start)

# Function to write the interpolated shape to an SVG file
def write_svg(interpolated_points, frame_number):
    # Ensure the output directory exists
    output_dir = "../../frames_generated"
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
    # Extract circle data from the input SVG file
    small_circle = extract_circle_info('../../svg/small_circle.svg')
    triangle_points = extract_triangle_info('../../svg/triangle.svg')

    # Ensure both shapes were successfully read
    if not small_circle or not triangle_points:
        print("Error: Could not read shape data from SVG files.")
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
