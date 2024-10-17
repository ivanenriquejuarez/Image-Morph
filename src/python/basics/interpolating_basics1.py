# File: interpolating_basics1.py

def interpolate(start, end, t):
    # The interpolation formula: difference between end and start value with respect to 't'
    return start + t * (end - start)

# Starting point(source) and ending point(target)
start = 10
end = 20

# Interpolation factor t (0.0 means 100% start, 1.0 means 100% end)
t = 0.5

# Calculate result
result = interpolate(start, end, t)

# Print result
print(f"The interpolated value is: {result}")
