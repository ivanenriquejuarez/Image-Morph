import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Define a function to interpolate between two points (x, y)
def interpolate_point(start, end, t):
    start_x, start_y = start
    end_x, end_y = end
    result_x = start_x + t * (end_x - start_x)
    result_y = start_y + t * (end_y - start_y)
    return (result_x, result_y)

# Define a function to interpolate between two shapes (list of points)
def interpolate_shape(shape1, shape2, t):
    return [interpolate_point(p1, p2, t) for p1, p2 in zip(shape1, shape2)]

# Define a function to plot the shape
def plot_shape(shape, color):
    x_values = [p[0] for p in shape] + [shape[0][0]]  # Close the shape by connecting last point to first
    y_values = [p[1] for p in shape] + [shape[0][1]]
    return x_values, y_values

# Define two shapes (small square and large square)
small_square = [(0.0, 0.0), (1.0, 0.0), (1.0, 1.0), (0.0, 1.0)]  # Small square
large_square = [(-1.0, -1.0), (2.0, -1.0), (2.0, 2.0), (-1.0, 2.0)]  # Large square

# Set up the figure and axis
fig, ax = plt.subplots()
ax.set_aspect('equal', adjustable='box')
ax.grid(True)

# Plot the small square and large square once (they won't change)
small_x_vals, small_y_vals = plot_shape(small_square, 'blue')
large_x_vals, large_y_vals = plot_shape(large_square, 'green')
ax.plot(small_x_vals, small_y_vals, 'blue', label='Small Square', linewidth=2)
ax.plot(large_x_vals, large_y_vals, 'green', label='Large Square', linewidth=2)

# Initialize the plot for the interpolated shape (which will update)
interpolated_line, = ax.plot([], [], 'red', label='Interpolated Shape', linewidth=2)

# Animation function that updates the plot for each frame
def update(t):
    interpolated_shape = interpolate_shape(small_square, large_square, t)
    x_vals, y_vals = plot_shape(interpolated_shape, 'red')
    interpolated_line.set_data(x_vals, y_vals)
    
    return interpolated_line,

# Create an animation that moves t from 0 to 1
ani = animation.FuncAnimation(fig, update, frames=[i / 100 for i in range(101)], interval=50)

# Add labels and legend
plt.title('Animating Shape Interpolation with Small and Large Square')
plt.legend()
plt.show()
