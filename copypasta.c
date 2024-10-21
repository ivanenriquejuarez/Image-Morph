#include <stdio.h>
#include <math.h>
#include <omp.h>

#define PI 3.14159265358979323846

// Struct to hold a vertex of the octagon
typedef struct {
    float x, y;
} Vertex;

// Function to interpolate between two vertices
Vertex interpolate_vertex(Vertex start, Vertex end, float t) {
    Vertex result;
    result.x = start.x + t * (end.x - start.x);
    result.y = start.y + t * (end.y - start.y);
    return result;
}

// Function to generate the vertices of a regular octagon
void generate_octagon(float cx, float cy, float radius, Vertex* octagon) {
    for (int i = 0; i < 8; i++) {
        float angle = (PI / 4) * i;  // 360 degrees / 8 = 45 degrees per vertex
        octagon[i].x = cx + radius * cos(angle);
        octagon[i].y = cy + radius * sin(angle);
    }
}

// Function to morph one octagon to another
void morph_octagon(Vertex* start, Vertex* end, float t, Vertex* result) {
    for (int i = 0; i < 8; i++) {
        result[i] = interpolate_vertex(start[i], end[i], t);
    }
}

// Function to write the octagon as an SVG file
void write_svg_octagon(Vertex* octagon, int frame_number) {
    char filename[50];
    sprintf(filename, "octagon_frame_%d.svg", frame_number);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Start SVG file
    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "<polygon points='");

    // Write the vertices of the octagon
    for (int i = 0; i < 8; i++) {
        fprintf(file, "%f,%f ", octagon[i].x, octagon[i].y);
    }

    // Close the polygon and SVG file
    fprintf(file, "' fill='none' stroke='blue' stroke-width='2' />\n");
    fprintf(file, "</svg>\n");

    fclose(file);
    printf("File %s created successfully.\n", filename);
}

int main() {
    // Define the small and big octagons
    Vertex small_octagon[8], big_octagon[8], result_octagon[8];

    // Small octagon: center (100, 100), radius 50
    generate_octagon(100, 100, 50, small_octagon);

    // Big octagon: center (250, 250), radius 150
    generate_octagon(250, 250, 150, big_octagon);

    const int num_frames = 10;  // Number of frames for the morph

    // Parallel loop using OpenMP to generate frames
    #pragma omp parallel for private(result_octagon)
    for (int i = 0; i <= num_frames; i++) {
        float t = (float)i / num_frames;

        // Morph the small octagon into the big octagon
        morph_octagon(small_octagon, big_octagon, t, result_octagon);

        // Write the corresponding SVG file for this frame
        write_svg_octagon(result_octagon, i);
    }

    return 0;
}
