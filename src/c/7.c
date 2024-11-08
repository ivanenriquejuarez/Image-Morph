#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // For creating directories
#include <libxml/parser.h>  // For parsing SVG files
#include <libxml/tree.h>    // For working with XML trees

// Define a structure to represent a point (x, y)
typedef struct {
    float x;
    float y;
} Point;

// Define a structure to represent a polygon
typedef struct {
    Point* vertices;  // Array of points (vertices)
    int num_vertices; // Number of vertices
} Polygon;

// Function to perform linear interpolation between two values
float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

// Function to morph between two polygons based on the interpolation factor 't'
void morph(Polygon* start, Polygon* end, float t, Polygon* result) {
    // Interpolate each vertex of the polygon
    for (int i = 0; i < start->num_vertices; i++) {
        result->vertices[i].x = interpolate(start->vertices[i].x, end->vertices[i].x, t);
        result->vertices[i].y = interpolate(start->vertices[i].y, end->vertices[i].y, t);
    }
}

// Function to manually define the triangle points
void define_triangle(Polygon* triangle) {
    // Allocating memory for 3 vertices
    triangle->num_vertices = 3;
    triangle->vertices = (Point*) malloc(triangle->num_vertices * sizeof(Point));

    // Define the 3 points of a standard triangle
    triangle->vertices[0].x = 250;  // Top vertex
    triangle->vertices[0].y = 50;

    triangle->vertices[1].x = 100;  // Bottom-left vertex
    triangle->vertices[1].y = 400;

    triangle->vertices[2].x = 400;  // Bottom-right vertex
    triangle->vertices[2].y = 400;
}

// Function to manually define the rectangle points
void define_rectangle(Polygon* rectangle) {
    // Allocating memory for 4 vertices
    rectangle->num_vertices = 4;
    rectangle->vertices = (Point*) malloc(rectangle->num_vertices * sizeof(Point));

    // Define the 4 points of a rectangle
    rectangle->vertices[0].x = 100;  // Top-left
    rectangle->vertices[0].y = 100;

    rectangle->vertices[1].x = 400;  // Top-right
    rectangle->vertices[1].y = 100;

    rectangle->vertices[2].x = 400;  // Bottom-right
    rectangle->vertices[2].y = 400;

    rectangle->vertices[3].x = 100;  // Bottom-left
    rectangle->vertices[3].y = 400;
}

// Function to generate an SVG file for the given polygon
void write_svg(Polygon* polygon, int frame_number) {
    char filename[100];

    // Generate filenames for each frame in the output_7 folder
    sprintf(filename, "output_7/frame_%d.svg", frame_number);

    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Write the SVG content for the polygon
    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <polygon points='");

    // Write each vertex of the polygon
    for (int i = 0; i < polygon->num_vertices; i++) {
        fprintf(file, "%f,%f ", polygon->vertices[i].x, polygon->vertices[i].y);
    }

    fprintf(file, "' fill='blue' />\n");
    fprintf(file, "</svg>\n");

    // Close the file after writing
    fclose(file);
    printf("File %s created successfully.\n", filename);
}

// Main function to perform the morphing and generate SVG frames
int main() {
    // Create the output_7 folder if it does not exist
    struct stat st = {0};
    if (stat("output_7", &st) == -1) {
        mkdir("output_7", 0700);  // The 0700 permissions mean read/write/execute for the owner
    }

    // Define the start and end polygons (rectangle and triangle)
    Polygon rectangle, triangle;

    // Manually define the rectangle and triangle points
    define_rectangle(&rectangle);
    define_triangle(&triangle);

    // Allocate memory for the result polygon (with the same number of vertices)
    int max_vertices = (rectangle.num_vertices > triangle.num_vertices) ? rectangle.num_vertices : triangle.num_vertices;
    Polygon result_polygon;
    result_polygon.num_vertices = max_vertices;
    result_polygon.vertices = (Point*) malloc(result_polygon.num_vertices * sizeof(Point));

    // Loop to generate frames from t = 0 to t = 1, with 0.01 increments for smooth transitions
    for (float t = 0.0; t <= 1.0; t += 0.01) {
        // Morph the rectangle into the triangle based on the interpolation factor 't'
        morph(&rectangle, &triangle, t, &result_polygon);

        // Generate the corresponding SVG file for the current frame
        write_svg(&result_polygon, (int)(t * 100));
    }

    // Free allocated memory
    free(rectangle.vertices);
    free(triangle.vertices);
    free(result_polygon.vertices);

    return 0;
}
