#include <stdio.h>
#include <stdlib.h>

// Define a structure to represent a Circle with its center and radius
typedef struct {
    float cx;  // x-coordinate of the center
    float cy;  // y-coordinate of the center
    float r;   // radius of the circle
} Circle;

// Function to perform linear interpolation between two values
float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

// Function to morph from one circle to another based on the interpolation factor 't'
void morph(Circle* start, Circle* end, float t, Circle* result) {
    result->cx = interpolate(start->cx, end->cx, t);
    result->cy = interpolate(start->cy, end->cy, t);
    result->r = interpolate(start->r, end->r, t);
}

// Function to generate an SVG file for the given circle without overwriting if the file exists
void write_svg(Circle* circle, int frame_number) {
    char filename[50];
    
    // Update the naming convention for output files
    if (frame_number == 0) {
        sprintf(filename, "small_circle.svg");
    } else if (frame_number == 100) {
        sprintf(filename, "big_circle.svg");
    } else {
        sprintf(filename, "frame_%d.svg", frame_number);
    }
    
    // Check if the file already exists
    FILE *check_file = fopen(filename, "r");
    if (check_file != NULL) {
        // File exists, so close and return without writing
        fclose(check_file);
        printf("File %s already exists. Skipping...\n", filename);
        return;
    }

    // File does not exist, proceed to write
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Write the SVG content for the circle
    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <circle cx='%f' cy='%f' r='%f' fill='blue' />\n", circle->cx, circle->cy, circle->r);
    fprintf(file, "</svg>\n");

    // Close the file after writing
    fclose(file);
    printf("File %s created successfully.\n", filename);
}

// Main function to perform the morphing and generate SVG frames
int main() {
    // Define the small and big circles (starting and ending points)
    Circle small_circle = {50, 50, 30};   // Small circle: center (50, 50), radius 30
    Circle big_circle = {250, 250, 200};  // Big circle: center (250, 250), radius 200
    Circle result;  // To hold the interpolated circle in each frame

    // Loop to generate frames from t = 0 to t = 1, with 0.01 increments for smooth transitions
    for (float t = 0.0; t <= 1.0; t += 0.01) {
        // Morph the small circle into the big circle based on the interpolation factor 't'
        morph(&small_circle, &big_circle, t, &result);
        // Generate the corresponding SVG file for the current frame
        write_svg(&result, (int)(t * 100));
    }

    return 0;
}
