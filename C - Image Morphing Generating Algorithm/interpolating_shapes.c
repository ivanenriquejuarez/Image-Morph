#include <stdio.h>

// Define a structure to hold 2D points
typedef struct {
    float x;
    float y;
} Point;

// Function to interpolate between two points
Point interpolate(Point start, Point end, float t) {
    Point result;
    result.x = start.x + t * (end.x - start.x);
    result.y = start.y + t * (end.y - start.y);
    return result;
}

// Function to interpolate between two shapes (arrays of points)
void interpolate_shape(Point* shape1, Point* shape2, Point* result, float t, int num_points) {
    for (int i = 0; i < num_points; i++) {
        result[i] = interpolate(shape1[i], shape2[i], t);  // Interpolate each point
    }
}

// Function to print a shape (array of points)
void print_shape(Point* shape, int num_points) {
    for (int i = 0; i < num_points; i++) {
        printf("Point %d: (%f, %f)\n", i, shape[i].x, shape[i].y);
    }
}

int main() {
    // Define two shapes (small square and large square)
	// Small square
    Point small_square[4] = {
		{0.0, 0.0}, // Bottom-left corner
		{1.0, 0.0}, // Bottom-right corner
		{1.0, 1.0}, // Top-right corner
		{0.0, 1.0}  // Top-left corner
	};  
	// Large square
    Point large_square[4] = {
		{-1.0, -1.0}, //Bottom-left corner
		{2.0, -1.0},  //Bottom-right corner
		{2.0, 2.0},   //Top-right corner
		{-1.0, 2.0}   //Top-left corner
	}; 

    // Array to hold the interpolated shape (4 points for a square)
    Point result[4];

    // Interpolation factor t (0.0 = small square, 1.0 = large square)
    float t = 0.5;  // Halfway between small and large square

    // Interpolate between the small square and large square
    interpolate_shape(small_square, large_square, result, t, 4);

    // Print the interpolated shape
    printf("Interpolated shape at t = %f:\n", t);
    print_shape(result, 4);

    return 0;
}