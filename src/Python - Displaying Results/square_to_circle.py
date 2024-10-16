import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Define a function to interpolate between two points (x, y)
def interpolate_point(start, end, t):
    start_x, start_y = start
    end_x, end_y = end
    result_x = start_x + t * (end_x - start_x)
    result_y = start_y + t * (end_y - start_y)
    return (result_x, result_y)

# Function to generate points on a circle
def generate_circle_points(radius, num_points):
    return [(radius * np.cos(2 * np.pi * i / num_points), radius * np.sin(2 * np.pi * i / num_points)) for i in range(num_points)]

# Function to generate points on a square (equally spaced)
def generate_square_points(side_length, num_points_per_side):
    half = side_length / 2
    points = []
    
    # Bottom side (from left to right)
    for i in range(num_points_per_side):
        points.append((-half + i * side_length / num_points_per_side, -half))
    
    # Right side (from bottom to top)
    for i in range(num_points_per_side):
        points.append((half, -half + i * side_length / num_points_per_side))
    
    # Top side (from right to left)
    for i in range(num_points_per_side):
        points.append((half - i * side_length / num_points_per_side, half))
    
    # Left side (from top to bottom)
    for i in range(num_points_per_side):
        points.append((-half, half - i * side_length / num_points_per_side))
    
    return points

# Define a function to interpolate between two shapes (list of points)
def interpolate_shape(shape1, shape2, t):
    return [interpolate_point(p1, p2, t) for p1, p2 in zip(shape1, shape2)]

# Define a function to plot the shape
def plot_shape(shape, color):
    x_values = [p[0] for p in shape] + [shape[0][0]]  # Close the shape by connecting last point to first
    y_values = [p[1] for p in shape] + [shape[0][1]]
    return x_values, y_values

# Number of points to use for both square and circle
num_points = 40

# Generate points for the square and the circle
square_points = generate_square_points(2.0, num_points // 4)  # 4 sides, so divide by 4
circle_points = generate_circle_points(1.5, num_points)       # Use 40 points for the circle

# Set up the figure and axis
fig, ax = plt.subplots()
ax.set_aspect('equal', adjustable='box')
ax.grid(True)

# Plot the square and circle initially
square_x_vals, square_y_vals = plot_shape(square_points, 'blue')
circle_x_vals, circle_y_vals = plot_shape(circle_points, 'green')
ax.plot(square_x_vals, square_y_vals, 'blue', label='Square', linewidth=2)
ax.plot(circle_x_vals, circle_y_vals, 'green', label='Circle', linewidth=2)

# Initialize the plot for the interpolated shape (which will update)
interpolated_line, = ax.plot([], [], 'red', label='Interpolated Shape', linewidth=2)

# Animation function that updates the plot for each frame
def update(t):
    interpolated_shape = interpolate_shape(square_points, circle_points, t)
    x_vals, y_vals = plot_shape(interpolated_shape, 'red')
    interpolated_line.set_data(x_vals, y_vals)
    return interpolated_line,

# Create an animation that moves t from 0 to 1
ani = animation.FuncAnimation(fig, update, frames=[i / 100 for i in range(101)], interval=50)

# Add labels and legend
plt.title('Morphing Square into Circle')
plt.legend()
plt.show()
