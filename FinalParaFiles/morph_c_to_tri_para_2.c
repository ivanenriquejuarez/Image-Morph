#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <math.h>
#include <string.h>
#include <time.h>  // Include time.h for execution time measurement
#include <omp.h>   // Include OpenMP header for parallelization

// Function prototypes
int extract_circle_info(const char* svg_file, float* cx, float* cy, float* r);
int extract_triangle_info(const char* svg_file, float triangle[3][2]);
float bezier_point(float p0, float p1, float p2, float t);
void write_svg(char* interpolated_points, int frame_number);

// Function to extract circle attributes from an SVG file
int extract_circle_info(const char* svg_file, float* cx, float* cy, float* r) {
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"circle") == 0) {
            xmlChar* cx_str = xmlGetProp(node, (const xmlChar *)"cx");
            xmlChar* cy_str = xmlGetProp(node, (const xmlChar *)"cy");
            xmlChar* r_str = xmlGetProp(node, (const xmlChar *)"r");

            if (cx_str && cy_str && r_str) {
                *cx = atof((const char *)cx_str);
                *cy = atof((const char *)cy_str);
                *r = atof((const char *)r_str);

                xmlFree(cx_str);
                xmlFree(cy_str);
                xmlFree(r_str);
                xmlFreeDoc(doc);
                return 0;
            }
        }
        node = node->next;
    }

    xmlFreeDoc(doc);
    return -1;
}

// Function to extract triangle points from an SVG file
int extract_triangle_info(const char* svg_file, float triangle[3][2]) {
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"polygon") == 0) {
            xmlChar* points_str = xmlGetProp(node, (const xmlChar *)"points");

            if (points_str) {
                sscanf((const char *)points_str, "%f,%f %f,%f %f,%f",
                       &triangle[0][0], &triangle[0][1],
                       &triangle[1][0], &triangle[1][1],
                       &triangle[2][0], &triangle[2][1]);

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

// Function to calculate a quadratic Bézier curve point
float bezier_point(float p0, float p1, float p2, float t) {
    return pow(1 - t, 2) * p0 + 2 * (1 - t) * t * p1 + t * t * p2;
}

// Function to write the interpolated shape to an SVG file
void write_svg(char* interpolated_points, int frame_number) {
    char filename[256];
    sprintf(filename, "./circle_to_triangle/frame_%03d.svg", frame_number);

    struct stat st = {0};
    if (stat("./circle_to_triangle", &st) == -1) {
        mkdir("./circle_to_triangle", 0700);
    }

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <polygon points='%s' fill='blue' />\n", interpolated_points);
    fprintf(file, "</svg>\n");

    fclose(file);
}

// Main function to perform the morphing and generate SVG frames
int main() {
    // Start timing the execution
    clock_t start_time = clock();

    float cx, cy, r;
    float triangle_vertices[3][2];

    if (extract_circle_info("./small_circle.svg", &cx, &cy, &r) == -1) {
        printf("Error: Could not extract circle info.\n");
        return -1;
    }

    if (extract_triangle_info("./triangle.svg", triangle_vertices) == -1) {
        printf("Error: Could not extract triangle info.\n");
        return -1;
    }

    int num_circle_points = 30;

    float control_points[3][2] = {
        {cx + 0.5 * r, cy},
        {cx, cy - 0.5 * r},
        {cx - 0.5 * r, cy}
    };

    int total_frames = 10000;
    
    #pragma omp parallel for
    for (int frame = 0; frame < total_frames; frame++) {
        float t = (float)frame / (total_frames - 1);  // `t` smoothly ranges from 0 to 1

        // Each thread works on its own string for the interpolated points
        char thread_interpolated_points[1024] = "";
        char point[50];

        // Parallelize the loop for Bézier curve point calculation
        #pragma omp parallel for private(point)
        for (int i = 0; i < num_circle_points; i++) {
            float angle = (2 * M_PI / num_circle_points) * i;
            float circle_x = cx + r * cos(angle);
            float circle_y = cy + r * sin(angle);

            float* triangle_vertex = triangle_vertices[i % 3];
            float tx = triangle_vertex[0];
            float ty = triangle_vertex[1];

            float* control_point = control_points[i % 3];
            float interp_x = bezier_point(circle_x, control_point[0], tx, t);
            float interp_y = bezier_point(circle_y, control_point[1], ty, t);

            // Prepare the point string for this particular thread
            sprintf(point, "%f,%f ", interp_x, interp_y);

            // Append this thread's point to the result
            strcat(thread_interpolated_points, point);
        }

        // Write the frame to an SVG file
        write_svg(thread_interpolated_points, frame);
    }

    // Stop timing the execution
    clock_t end_time = clock();

    // Calculate the time taken in milliseconds
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000;
    printf("Execution Time: %.2f ms\n", time_taken);

    return 0;
}
