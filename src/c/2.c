#include <stdio.h>

// Define a structure to hold 2D points (x, y)
typedef struct 
{
    float x;
    float y;
} Point;

// Function to interpolate between two points
Point interpolate(Point start, Point end, float t) 
{
    Point result;
    result.x = start.x + t * (end.x - start.x);
    result.y = start.y + t * (end.y - start.y);
    return result;
}

int main() 
{
    // Define two points in 2D space
    Point start = {0.0, 0.0};  // Starting point (0, 0)
    Point end = {1.0, 1.0};    // Ending point (1, 1)

    // Interpolation factor t (0.0 = start, 1.0 = end)
    float t = 0.9;  // Change this value to see different interpolation steps

    // Perform interpolation
    Point result = interpolate(start, end, t);

    // Print the interpolated point
    printf("Interpolated Point: (%f, %f)\n", result.x, result.y);

    return 0;
}

