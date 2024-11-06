#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

// Define a structure to represent an Ellipse with center and two radii
typedef struct {
    float cx;  // x-coordinate of the center
    float cy;  // y-coordinate of the center
    float rx;  // horizontal radius (x-axis)
    float ry;  // vertical radius (y-axis)
} Ellipse;

// Function to perform linear interpolation between two values
float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

// Function to morph from a circle to an ellipse based on interpolation factor 't'
void morph_circle_to_ellipse(Ellipse* start, Ellipse* end, float t, Ellipse* result) {
    result->cx = interpolate(start->cx, end->cx, t);
    result->cy = interpolate(start->cy, end->cy, t);
    result->rx = interpolate(start->rx, end->rx, t);  // Horizontal radius
    result->ry = interpolate(start->ry, end->ry, t);  // Vertical radius
}

// Function to generate an SVG file for the ellipse
void write_svg_ellipse(Ellipse* ellipse, int frame_number) {
    char filename[100];
    sprintf(filename, "circle_to_ellipse/frame_%d.svg", frame_number);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <ellipse cx='%f' cy='%f' rx='%f' ry='%f' fill='blue' />\n", ellipse->cx, ellipse->cy, ellipse->rx, ellipse->ry);
    fprintf(file, "</svg>\n");

    fclose(file);
    printf("File %s created successfully.\n", filename);
}

// Function to load an SVG file and extract ellipse or circle data
int load_svg(const char* filename, Ellipse* ellipse) {
    printf("Loading SVG file: %s\n", filename);
    xmlDoc *doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", filename);
        return -1;
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE) {
            if (xmlStrcmp(node->name, (const xmlChar *)"circle") == 0) {
                // Extract the 'cx', 'cy', and 'r' attributes for circle
                xmlChar* cx_str = xmlGetProp(node, (const xmlChar *)"cx");
                xmlChar* cy_str = xmlGetProp(node, (const xmlChar *)"cy");
                xmlChar* r_str = xmlGetProp(node, (const xmlChar *)"r");

                ellipse->cx = atof((const char *)cx_str);
                ellipse->cy = atof((const char *)cy_str);
                ellipse->rx = atof((const char *)r_str);
                ellipse->ry = ellipse->rx;  // Circle has equal rx and ry

                xmlFree(cx_str);
                xmlFree(cy_str);
                xmlFree(r_str);
            } else if (xmlStrcmp(node->name, (const xmlChar *)"ellipse") == 0) {
                // Extract the 'cx', 'cy', 'rx', and 'ry' attributes for ellipse
                xmlChar* cx_str = xmlGetProp(node, (const xmlChar *)"cx");
                xmlChar* cy_str = xmlGetProp(node, (const xmlChar *)"cy");
                xmlChar* rx_str = xmlGetProp(node, (const xmlChar *)"rx");
                xmlChar* ry_str = xmlGetProp(node, (const xmlChar *)"ry");

                ellipse->cx = atof((const char *)cx_str);
                ellipse->cy = atof((const char *)cy_str);
                ellipse->rx = atof((const char *)rx_str);
                ellipse->ry = atof((const char *)ry_str);

                xmlFree(cx_str);
                xmlFree(cy_str);
                xmlFree(rx_str);
                xmlFree(ry_str);
            }
        }
        node = node->next;
    }

    xmlFreeDoc(doc);
    return 0;
}

int main() {
    // Create output directory if it doesn't exist
    struct stat st = {0};
    if (stat("circle_to_ellipse", &st) == -1) {
        mkdir("circle_to_ellipse", 0700);
    }

    Ellipse circle, ellipse, result;

    // Load the SVG files for the circle and ellipse
    if (load_svg("../../svg/small_circle.svg", &circle) == -1) {
        printf("Failed to load ../../svg/small_circle.svg\n");
        return -1;
    }
    if (load_svg("../../svg/large_ellipse.svg", &ellipse) == -1) {
        printf("Failed to load ../../svg/large_ellipse.svg\n");
        return -1;
    }

    // Morph from circle to ellipse over 100 frames
    for (float t = 0.0; t <= 1.0; t += 0.01) {
        morph_circle_to_ellipse(&circle, &ellipse, t, &result);
        write_svg_ellipse(&result, (int)(t * 100));
    }

    return 0;
}
