import os
import svgpathtools
import xml.etree.ElementTree as ET
from svgpathtools import parse_path, Path, Line, CubicBezier, QuadraticBezier

# Define a class to represent a generic SVG Path Shape
class Shape:
    def __init__(self, path):
        self.path = path  # SVG path object representing the shape

# Function to perform linear interpolation between two values
def interpolate(start, end, t):
    return start + t * (end - start)

# Function to extract the path data from an SVG file
def extract_path_from_svg(svg_file):
    # Parse the SVG file
    tree = ET.parse(svg_file)
    root = tree.getroot()

    # Extract path data from the <path> element
    for elem in root.iter():
        if elem.tag.endswith('path'):
            d = elem.attrib.get('d')
            if d:
                return parse_path(d)
    
    raise ValueError(f"No <path> element found in {svg_file}")

# Function to normalize a path to have the same scale and be centered
def normalize_path(path, target_size=400, canvas_size=500):
    # Calculate the bounding box of the path
    xmin, xmax, ymin, ymax = path.bbox()
    width = xmax - xmin
    height = ymax - ymin

    # Calculate scale factor to fit the target size
    scale_factor = target_size / max(width, height)

    # Normalize the path by scaling and translating to center it in the canvas
    normalized_path = path.scaled(scale_factor)
    dx = (canvas_size / 2) - ((xmin + xmax) * scale_factor / 2)
    dy = (canvas_size / 2) - ((ymin + ymax) * scale_factor / 2)
    normalized_path = normalized_path.translated(dx + dy * 1j)

    return normalized_path

# Function to resample paths to have the same number of segments using parametric sampling
def resample_paths(path1, path2, num_segments=100):
    # Calculate the total length of each path
    length1 = path1.length()
    length2 = path2.length()

    # Generate evenly spaced points along each path based on their lengths
    path1_points = [path1.point(path1.ilength(length1 * t / num_segments)) for t in range(num_segments + 1)]
    path2_points = [path2.point(path2.ilength(length2 * t / num_segments)) for t in range(num_segments + 1)]

    # Create new paths with resampled points
    resampled_path1 = Path(*[Line(path1_points[i], path1_points[i + 1]) for i in range(num_segments)])
    resampled_path2 = Path(*[Line(path2_points[i], path2_points[i + 1]) for i in range(num_segments)])

    return resampled_path1, resampled_path2

# Function to morph from one path to another based on the interpolation factor 't'
def morph(start, end, t):
    if len(start.path) != len(end.path):
        raise ValueError("Shapes must have the same number of path segments to morph.")
    
    morphed_path = Path()
    for seg1, seg2 in zip(start.path, end.path):
        if isinstance(seg1, Line) and isinstance(seg2, Line):
            start_points = [seg1.start, seg1.end]
            end_points = [seg2.start, seg2.end]
            morphed_points = [
                interpolate(start_point, end_point, t)
                for start_point, end_point in zip(start_points, end_points)
            ]
            morphed_path.append(Line(*morphed_points))
        elif isinstance(seg1, (CubicBezier, QuadraticBezier)) and isinstance(seg2, (CubicBezier, QuadraticBezier)):
            # Handle morphing for Bezier curves
            start_points = seg1.bpoints()
            end_points = seg2.bpoints()
            morphed_points = [
                interpolate(start_point, end_point, t)
                for start_point, end_point in zip(start_points, end_points)
            ]
            if len(morphed_points) == 4:
                morphed_path.append(CubicBezier(*morphed_points))
            elif len(morphed_points) == 3:
                morphed_path.append(QuadraticBezier(*morphed_points))
        else:
            raise NotImplementedError("Morphing for this segment type is not yet implemented.")
    
    return Shape(morphed_path)

# Function to generate an SVG file for the given shape without overwriting if the file exists
def write_svg(shape, frame_number):
    # Update the naming convention for output files
    if frame_number == 0:
        filename = "start_shape.svg"
    elif frame_number == 100:
        filename = "end_shape.svg"
    else:
        filename = f"frame_{frame_number}.svg"

    # Check if the file already exists
    if os.path.exists(filename):
        # File exists, return without writing
        print(f"File {filename} already exists. Skipping...")
        return

    # File does not exist, proceed to write
    try:
        with open(filename, "w") as file:
            # Write the SVG content for the shape
            file.write("<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n")
            file.write(f"  <path d='{shape.path.d()}' fill='blue' />\n")
            file.write("</svg>\n")
        print(f"File {filename} created successfully.")
    except IOError:
        print(f"Error: Could not open file {filename} for writing")

# Main function to perform the morphing and generate SVG frames
def main():
    # Extract paths from the SVG files
    start_path = extract_path_from_svg("../../svg/chef.svg")  # Starting shape (chef)
    end_path = extract_path_from_svg("../../svg/donut.svg")  # Target shape (donut)

    # Normalize the paths to ensure they have the same size and are centered
    normalized_start_path = normalize_path(start_path)
    normalized_end_path = normalize_path(end_path)

    # Resample the paths to ensure they have the same number of segments
    resampled_start_path, resampled_end_path = resample_paths(normalized_start_path, normalized_end_path)

    start_shape = Shape(resampled_start_path)
    end_shape = Shape(resampled_end_path)

    # Loop to generate frames from t = 0 to t = 1, with 0.01 increments for smooth transitions
    for frame in range(101):
        t = frame / 100.0
        # Morph the start shape into the end shape based on the interpolation factor 't'
        result = morph(start_shape, end_shape, t)
        # Generate the corresponding SVG file for the current frame
        write_svg(result, int(t * 100))

if __name__ == "__main__":
    main()
