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

# Define two points in 2D space
start = Point(0.0, 0.0)  # Starting point (0, 0)
end = Point(1.0, 1.0)    # Ending point (1, 1)

# Interpolation factor t (0.0 = start, 1.0 = end)
t = 0.9  # Change this value to see different interpolation steps

# Perform interpolation
result = interpolate(start, end, t)

# Print the interpolated point
print(f"Interpolated Point: ({result.x}, {result.y})")
