import matplotlib.pyplot as plt
import matplotlib.animation as animation
import cairosvg
from PIL import Image
import io

# Directory where the SVG files are stored
svg_directory = './'  # Assuming the files are in the same directory
svg_filenames = [f"octagon_frame_{i}.svg" for i in range(11)]  # List of SVG file names

# Function to convert SVG to PNG using cairosvg and load it as an image
def load_svg_as_image(filename):
    # Convert SVG to PNG in memory using cairosvg
    png_data = cairosvg.svg2png(url=filename)
    
    # Open the PNG data as an image using PIL
    image = Image.open(io.BytesIO(png_data))
    
    # Convert the PIL image to a format that matplotlib can use
    return image

# Function to animate the sequence of PNG images
def animate(i, img, ax):
    ax.clear()  # Clear the previous frame
    svg_image = load_svg_as_image(svg_filenames[i])  # Load PNG image from SVG
    ax.imshow(svg_image)  # Display the PNG image
    ax.set_title(f"Frame {i}")
    return img,

# Create the figure and axis for animation
fig, ax = plt.subplots(figsize=(6, 6))
ax.set_axis_off()  # Turn off axis

# Load the first SVG image by converting it to PNG
svg_image = load_svg_as_image(svg_filenames[0])
img = ax.imshow(svg_image)

# Create an animation from the sequence of PNG images
ani = animation.FuncAnimation(
    fig, animate, frames=len(svg_filenames), fargs=(img, ax), interval=500, repeat=True
)

# Show the animation
plt.show()
