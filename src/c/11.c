#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <math.h>
#include <string.h>

// Function to extract circle to triangle attributes from an SVG file and morph
int extract_circle_info(const char* svg_file, float* cx, float* cy, float* r) {
    // Parse the SVG XML file
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;
    }

    // Get the root element (should be <svg>)
    xmlNode *root_element = xmlDocGetRootElement(doc);

    // Traverse the children of <svg> to find the <circle> element
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"circle") == 0) {
            // Extract 'cx', 'cy', and 'r' attributes
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
    // Parse the SVG XML file
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;
    }

    // Get the root element (should be <svg>)
    xmlNode *root_element = xmlDocGetRootElement(doc);

    // Traverse the children of <svg> to find the <polygon> element
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"polygon") == 0) {
            // Extract 'points' attribute
            xmlChar* points_str = xmlGetProp(node, (const xmlChar *)"points");

            if (points_str) {
                // Split points string into coordinates
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

// Function to calculate a quadratic Bézier curve point
float bezier_point(float p0, float p1, float p2, float t) {
    return pow(1 - t, 2) * p0 + 2 * (1 - t) * t * p1 + t * t * p2;
}

// Function to write the interpolated shape to an SVG file
void write_svg(char* interpolated_points, int frame_number) {
    char filename[256];
    sprintf(filename, "../../frames_generated/frame_%03d.svg", frame_number);

    // Ensure the output directory exists
    struct stat st = {0};
    if (stat("../../frames_generated", &st) == -1) {
        mkdir("../../frames_generated", 0700);
    }

    // Write the SVG content to the file
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <polygon points='%s' fill='blue' />\n", interpolated_points);
    fprintf(file, "</svg>\n");

    fclose(file);
    printf("File %s created successfully.\n", filename);
}

// Main function to perform the morphing and generate SVG frames
int main() {
    float cx, cy, r;
    float triangle_vertices[3][2];

    // Extract circle data from the input SVG file
    if (extract_circle_info("../../svg/small_circle.svg", &cx, &cy, &r) == -1) {
        printf("Error: Could not extract circle info.\n");
        return -1;
    }

    // Extract triangle data from the input SVG file
    if (extract_triangle_info("../../svg/triangle.svg", triangle_vertices) == -1) {
        printf("Error: Could not extract triangle info.\n");
        return -1;
    }

    // Number of points on the circle
    int num_circle_points = 30;

    // Define control points (you can adjust these for smoother curves)
    float control_points[3][2] = {
        {cx + 0.5 * r, cy},  // control point for 1st triangle vertex
        {cx, cy - 0.5 * r},  // control point for 2nd triangle vertex
        {cx - 0.5 * r, cy}   // control point for 3rd triangle vertex
    };

    // Loop to generate frames from t = 0 to t = 1
    for (int frame = 0; frame <= 100; frame++) {
        float t = frame / 100.0f;  // Interpolation factor
        char interpolated_points[1024] = "";
        char point[50];

        // Calculate interpolated points between circle and triangle vertices using Bézier curves
        for (int i = 0; i < num_circle_points; i++) {
            // Calculate the initial point on the circle's circumference
            float angle = (2 * M_PI / num_circle_points) * i;
            float circle_x = cx + r * cos(angle);
            float circle_y = cy + r * sin(angle);

            // Determine which triangle vertex this point moves towards
            float* triangle_vertex = triangle_vertices[i % 3];
            float tx = triangle_vertex[0];
            float ty = triangle_vertex[1];

            // Use the corresponding control point for the Bézier curve
            float* control_point = control_points[i % 3];

            // Calculate the Bézier point at time t
            float interp_x = bezier_point(circle_x, control_point[0], tx, t);
            float interp_y = bezier_point(circle_y, control_point[1], ty, t);

            // Add the point to the interpolated points string
            sprintf(point, "%f,%f ", interp_x, interp_y);
            strcat(interpolated_points, point);
        }

        // Remove trailing space
        interpolated_points[strlen(interpolated_points) - 1] = '\0';

        // Generate the corresponding SVG file for the current frame
        write_svg(interpolated_points, frame);
    }

    return 0;
}
