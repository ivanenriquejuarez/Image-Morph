#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <math.h>
#include <sys/stat.h>
#include <errno.h>

// Function to interpolate between two float values (Ease-In/Ease-Out)
float ease_in_out(float t) {
    return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

// Linear interpolation between two points
void linear_interpolation(float x1, float y1, float x2, float y2, float t, float* result_x, float* result_y) {
    *result_x = (1 - t) * x1 + t * x2;
    *result_y = (1 - t) * y1 + t * y2;
}

// Function to load the path data (d attribute) from an SVG file
char* load_svg_path(const char* filename) {
    xmlDoc *doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", filename);
        return NULL;
    }

    xmlNode *root_element = xmlDocGetRootElement(doc);
    xmlNode *node = root_element->children;

    // Find the <path> element and get the "d" attribute (path data)
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"path") == 0) {
            xmlChar* path_data = xmlGetProp(node, (const xmlChar *)"d");
            char *path_copy = strdup((char*)path_data); // Make a copy of the path data
            xmlFree(path_data);
            xmlFreeDoc(doc);
            return path_copy;
        }
        node = node->next;
    }

    xmlFreeDoc(doc);
    return NULL;
}

// Function to write the interpolated path to an SVG file
void write_svg_path(const char* interpolated_path, int frame_number) {
    char filename[100];
    sprintf(filename, "chef_to_donut/frame_%03d.svg", frame_number);  // Zero-padded to 3 digits

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing: %s\n", filename, strerror(errno));
        return;
    }

    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <path d='%s' fill='blue' />\n", interpolated_path);
    fprintf(file, "</svg>\n");

    fclose(file);
    printf("Frame %d written successfully to %s\n", frame_number, filename);  // Debug message
}

// Function to interpolate between two SVG paths
char* morph_paths(const char* chef_path, const char* donut_path, float t) {
    // TODO: Parse the paths and normalize them (i.e., make sure both paths have the same number of points)
    // Then apply interpolation between corresponding points.

    // For now, return the chef path for simplicity
    if (t == 0.0f) {
        return strdup(chef_path);  // Return chef path at t=0
    } else if (t == 1.0f) {
        return strdup(donut_path);  // Return donut path at t=1
    } else {
        // Linear interpolation between chef_path and donut_path (actual implementation needed here)
        return strdup(chef_path);  // Temporary: just returning chef_path
    }
}

int main() {
    // Load the path data from chef.svg and donut.svg
    char* chef_path = load_svg_path("../../svg/chef.svg");
    char* donut_path = load_svg_path("../../svg/donut.svg");

    if (chef_path == NULL || donut_path == NULL) {
        printf("Error: Failed to load SVG paths.\n");
        return -1;
    }

    // Create the output directory if it doesn't exist
    struct stat st = {0};
    if (stat("chef_to_donut", &st) == -1) {
        if (mkdir("chef_to_donut", 0700) != 0) {
            printf("Error creating directory 'chef_to_donut': %s\n", strerror(errno));
            return -1;
        }
    }

    // Generate 101 frames (from frame 0 to frame 100)
    for (int frame = 0; frame <= 100; frame++) {
        float t = frame / 100.0f;  // Interpolation factor between 0 and 1
        char* interpolated_path = morph_paths(chef_path, donut_path, t);  // Morph using linear interpolation
        write_svg_path(interpolated_path, frame);  // Write the result to a new SVG file
        free(interpolated_path);
    }

    // Clean up
    free(chef_path);
    free(donut_path);

    return 0;
}
