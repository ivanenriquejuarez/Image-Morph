import os
import xml.etree.ElementTree as ET

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

# Function to perform linear interpolation between two values
def interpolate(start, end, t):
    return start + t * (end - start)

# Function to write the interpolated circle to an SVG file
def write_svg(circle, frame_number):
    # Ensure the output directory exists
    output_dir = "../../frames_generated"
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Create the filename for the SVG file
    filename = os.path.join(output_dir, f"frame_{frame_number:03d}.svg")

    # Write the SVG content for the interpolated circle
    with open(filename, "w") as file:
        file.write(f"<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n")
        file.write(f"  <circle cx='{circle[0]}' cy='{circle[1]}' r='{circle[2]}' fill='blue' />\n")
        file.write("</svg>\n")

    print(f"File {filename} created successfully.")

# Main function to perform the morphing and generate SVG frames
def main():
    # Extract circle data from the input SVG files
    small_circle = extract_circle_info('../../svg/small_circle.svg')
    big_circle = extract_circle_info('../../svg/big_circle.svg')

    # Ensure both circles were successfully read
    if not small_circle or not big_circle:
        print("Error: Could not read circle data from SVG files.")
        return

    # Loop to generate frames from t = 0 to t = 1, with 0.01 increments for smooth transitions
    for frame in range(101):
        t = frame / 100.0  # Interpolation factor (0.00 to 1.00)

        # Interpolate between the small and big circles
        interpolated_circle = (
            interpolate(small_circle[0], big_circle[0], t),  # cx
            interpolate(small_circle[1], big_circle[1], t),  # cy
            interpolate(small_circle[2], big_circle[2], t)   # r
        )

        # Generate the corresponding SVG file for the current frame
        write_svg(interpolated_circle, frame)

# Directly call the main function
main()
