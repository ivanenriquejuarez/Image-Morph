class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

def interpolate(start, end, t):
    # Interpolate between two points
    return Point(
        start.x + t * (end.x - start.x),
        start.y + t * (end.y - start.y)
    )

def interpolate_shape(shape1, shape2, t):
    # Interpolate between two shapes (arrays of points)
    return [interpolate(shape1[i], shape2[i], t) for i in range(len(shape1))]

def print_shape(shape):
    # Print a shape (array of points)
    for i, point in enumerate(shape):
        print(f"Point {i}: ({point.x}, {point.y})")

# Define two shapes (small square and large square)
small_square = [
    Point(0.0, 0.0),  # Bottom-left corner
    Point(1.0, 0.0),  # Bottom-right corner
    Point(1.0, 1.0),  # Top-right corner
    Point(0.0, 1.0)   # Top-left corner
]

large_square = [
    Point(-1.0, -1.0),  # Bottom-left corner
    Point(2.0, -1.0),   # Bottom-right corner
    Point(2.0, 2.0),    # Top-right corner
    Point(-1.0, 2.0)    # Top-left corner
]

# Interpolation factor t (0.0 = small square, 1.0 = large square)
t = 0.5  # Halfway between small and large square

# Interpolate between the small square and large square
result = interpolate_shape(small_square, large_square, t)

# Print the interpolated shape
print(f"Interpolated shape at t = {t}:")
print_shape(result)
