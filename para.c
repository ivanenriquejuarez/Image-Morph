#include <stdio.h>
#include <math.h>
#include <omp.h>

#define PI 3.14159265358979323846
#define MESH_RES 4 // Subdivide each octagon edge into MESH_RES sections

// Struct to hold a vertex
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

// Generate a mesh between each edge of the octagon
void generate_mesh(Vertex* octagon, Vertex mesh[][MESH_RES + 1]) {
    for (int i = 0; i < 8; i++) {
        int next = (i + 1) % 8;
        for (int j = 0; j <= MESH_RES; j++) {
            float t = (float)j / MESH_RES;
            mesh[i][j] = interpolate_vertex(octagon[i], octagon[next], t);
        }
    }
}

// Function to morph the mesh from the small octagon to the big octagon
void morph_mesh(Vertex small_mesh[][MESH_RES + 1], Vertex big_mesh[][MESH_RES + 1], float t, Vertex result_mesh[][MESH_RES + 1]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= MESH_RES; j++) {
            result_mesh[i][j] = interpolate_vertex(small_mesh[i][j], big_mesh[i][j], t);
        }
    }
}

// Function to write the mesh as an SVG file with a grid background
void write_svg_mesh(Vertex mesh[][MESH_RES + 1], int frame_number) {
    char filename[50];
    sprintf(filename, "octagon_mesh_frame_%d.svg", frame_number);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Start SVG file
    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");

    // Add a grid background (horizontal and vertical lines)
    int grid_size = 50;  // Spacing between grid lines
    for (int x = 0; x <= 500; x += grid_size) {
        fprintf(file, "<line x1='%d' y1='0' x2='%d' y2='500' stroke='lightgray' stroke-width='0.5' />\n", x, x);
    }
    for (int y = 0; y <= 500; y += grid_size) {
        fprintf(file, "<line x1='0' y1='%d' x2='500' y2='%d' stroke='lightgray' stroke-width='0.5' />\n", y, y);
    }

    // Draw the mesh as lines between vertices
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < MESH_RES; j++) {
            fprintf(file, "<line x1='%f' y1='%f' x2='%f' y2='%f' stroke='blue' stroke-width='1' />\n",
                    mesh[i][j].x, mesh[i][j].y, mesh[i][j + 1].x, mesh[i][j + 1].y);
        }
    }

    // Close SVG
    fprintf(file, "</svg>\n");

    fclose(file);
    printf("File %s created successfully.\n", filename);
}


int main() {
    Vertex small_octagon[8], big_octagon[8];
    Vertex small_mesh[8][MESH_RES + 1], big_mesh[8][MESH_RES + 1], result_mesh[8][MESH_RES + 1];

    // Small octagon: center (100, 100), radius 50
    generate_octagon(100, 100, 50, small_octagon);
    generate_mesh(small_octagon, small_mesh);

    // Big octagon: center (250, 250), radius 150
    generate_octagon(250, 250, 150, big_octagon);
    generate_mesh(big_octagon, big_mesh);

    const int num_frames = 10;  // Number of frames for the morph

    // Parallel loop using OpenMP to generate frames
    #pragma omp parallel for private(result_mesh)
    for (int i = 0; i <= num_frames; i++) {
        float t = (float)i / num_frames;

        // Morph the small mesh into the big mesh
        morph_mesh(small_mesh, big_mesh, t, result_mesh);

        // Write the corresponding SVG file for this frame
        write_svg_mesh(result_mesh, i);
    }

    return 0;
}
