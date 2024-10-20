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

// Function to resample a polygon with fewer vertices to match the number of vertices
void resample_polygon(Polygon* poly, int new_num_vertices) {
    Point* new_vertices = (Point*) malloc(new_num_vertices * sizeof(Point));
    int current_num_vertices = poly->num_vertices;

    // For each new vertex, calculate where it should be along the existing edges
    for (int i = 0; i < new_num_vertices; i++) {
        float t = (float)i / new_num_vertices;  // Parameter along the polygon's length
        int seg = (int)(t * current_num_vertices);  // Find the segment this t corresponds to
        float local_t = (t * current_num_vertices) - seg;

        // Wrap around to the first vertex if we're at the last segment
        int next = (seg + 1) % current_num_vertices;

        // Linearly interpolate between vertices to find the new point
        new_vertices[i].x = (1 - local_t) * poly->vertices[seg].x + local_t * poly->vertices[next].x;
        new_vertices[i].y = (1 - local_t) * poly->vertices[seg].y + local_t * poly->vertices[next].y;
    }

    // Replace the old vertices with the new resampled vertices
    free(poly->vertices);
    poly->vertices = new_vertices;
    poly->num_vertices = new_num_vertices;
}

// Function to extract polygon points from an SVG file
int extract_polygon_points(const char* svg_file, Polygon* polygon) {
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;
    }

    // Get the root element of the SVG file
    xmlNode *root_element = xmlDocGetRootElement(doc);

    // Traverse to find the <polygon> element and extract the 'points' attribute
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"polygon") == 0) {
            // Extract the 'points' attribute
            xmlChar* points_str = xmlGetProp(node, (const xmlChar *)"points");
            if (points_str) {
                // Count the number of vertices (by counting spaces)
                int num_vertices = 0;
                for (int i = 0; points_str[i] != '\0'; i++) {
                    if (points_str[i] == ' ') {
                        num_vertices++;
                    }
                }
                num_vertices++; // One more vertex than spaces

                // Allocate memory for the polygon's vertices
                polygon->vertices = (Point*) malloc(num_vertices * sizeof(Point));
                polygon->num_vertices = num_vertices;

                // Parse the points into the vertices array
                char* token = strtok((char*) points_str, " ");
                int i = 0;
                while (token != NULL && i < num_vertices) {
                    sscanf(token, "%f,%f", &polygon->vertices[i].x, &polygon->vertices[i].y);
                    token = strtok(NULL, " ");
                    i++;
                }

                xmlFree(points_str);
                xmlFreeDoc(doc);
                return 0;
            }
        }
        node = node->next;
    }

    xmlFreeDoc(doc);
    return -1;
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

    // Extract the rectangle polygon points from the SVG file
    if (extract_polygon_points("../../svg/rectangle.svg", &rectangle) == -1) {
        printf("Error: Could not extract rectangle points.\n");
        return -1;
    }

    // Extract the triangle polygon points from the SVG file
    if (extract_polygon_points("../../svg/triangle.svg", &triangle) == -1) {
        printf("Error: Could not extract triangle points.\n");
        return -1;
    }

    // Resample the polygon with fewer vertices to match the one with more vertices
    if (rectangle.num_vertices > triangle.num_vertices) {
        resample_polygon(&triangle, rectangle.num_vertices);
    } else if (triangle.num_vertices > rectangle.num_vertices) {
        resample_polygon(&rectangle, triangle.num_vertices);
    }

    // Allocate memory for the result polygon (with the same number of vertices)
    Polygon result_polygon;
    result_polygon.num_vertices = rectangle.num_vertices;  // Both polygons now have the same number of vertices
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
