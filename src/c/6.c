#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>  // For creating directories
#include <sys/types.h>

//Rectangle to Triangle with Coordinates
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

// Function to generate an SVG file for the given polygon
void write_svg(Polygon* polygon, int frame_number) {
    char filename[100];

    // Generate filenames for each frame in the output_6 folder
    sprintf(filename, "output_6/frame_%d.svg", frame_number);

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
    // Create the output_6 folder if it does not exist
    struct stat st = {0};
    if (stat("output_6", &st) == -1) {
        mkdir("output_6", 0700);  // The 0700 permissions mean read/write/execute for the owner
    }

    // Define the rectangle and triangle polygons
    Point rectangle_vertices[] = { {50, 50}, {150, 50}, {150, 150}, {50, 150} }; // A rectangle
    Point triangle_vertices[] = { {100, 50}, {150, 150}, {50, 150}, {100, 50} }; // A triangle (duplicate the first vertex to match 4 vertices)

    // Set up the rectangle and triangle polygons
    Polygon rectangle = { rectangle_vertices, 4 };  // 4 vertices for the rectangle
    Polygon triangle = { triangle_vertices, 4 };    // 4 vertices for the triangle (using duplicate points)

    // Allocate memory for the result polygon (with the same number of vertices)
    Polygon result_polygon;
    result_polygon.num_vertices = rectangle.num_vertices;  // Both shapes have 4 vertices
    result_polygon.vertices = (Point*) malloc(result_polygon.num_vertices * sizeof(Point));

    // Loop to generate frames from t = 0 to t = 1, with 0.01 increments for smooth transitions
    for (float t = 0.0; t <= 1.0; t += 0.01) {
        // Morph the rectangle into the triangle based on the interpolation factor 't'
        morph(&rectangle, &triangle, t, &result_polygon);

        // Generate the corresponding SVG file for the current frame
        write_svg(&result_polygon, (int)(t * 100));
    }

    // Free allocated memory
    free(result_polygon.vertices);

    return 0;
}
