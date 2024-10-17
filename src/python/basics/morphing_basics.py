import os

# Define a class to represent a Circle with its center and radius
class Circle:
    def __init__(self, cx, cy, r):
        self.cx = cx  # x-coordinate of the center
        self.cy = cy  # y-coordinate of the center
        self.r = r    # radius of the circle

# Function to perform linear interpolation between two values
def interpolate(start, end, t):
    return start + t * (end - start)

# Function to morph from one circle to another based on the interpolation factor 't'
def morph(start, end, t):
    cx = interpolate(start.cx, end.cx, t)
    cy = interpolate(start.cy, end.cy, t)
    r = interpolate(start.r, end.r, t)
    return Circle(cx, cy, r)

# Function to generate an SVG file for the given circle without overwriting if the file exists
def write_svg(circle, frame_number):
    # Update the naming convention for output files
    if frame_number == 0:
        filename = "small_circle.svg"
    elif frame_number == 100:
        filename = "big_circle.svg"
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
            # Write the SVG content for the circle
            file.write("<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n")
            file.write(f"  <circle cx='{circle.cx}' cy='{circle.cy}' r='{circle.r}' fill='blue' />\n")
            file.write("</svg>\n")
        print(f"File {filename} created successfully.")
    except IOError:
        print(f"Error: Could not open file {filename} for writing")

# Main function to perform the morphing and generate SVG frames
def main():
    # Define the small and big circles (starting and ending points)
    small_circle = Circle(50, 50, 30)    # Small circle: center (50, 50), radius 30
    big_circle = Circle(250, 250, 200)   # Big circle: center (250, 250), radius 200

    # Loop to generate frames from t = 0 to t = 1, with 0.1 increments for smooth transitions
    t = 0.0
    while t <= 1.0:
        # Morph the small circle into the big circle based on the interpolation factor 't'
        result = morph(small_circle, big_circle, t)
        # Generate the corresponding SVG file for the current frame
        write_svg(result, int(t * 100))
        t += 0.1

main()
