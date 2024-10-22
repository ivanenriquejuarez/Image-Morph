#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

//Extracting small_circle.svg to large_circle.svg and morphing
// Define a structure to represent a Circle with its center and radius
typedef struct 
{
    float cx;  // x-coordinate of the center
    float cy;  // y-coordinate of the center
    float r;   // radius of the circle
} Circle;

// Function to perform linear interpolation between two values
float interpolate(float start, float end, float t) 
{
    return start + t * (end - start);
}

// Function to morph from one circle to another based on the interpolation factor 't'
void morph(Circle* start, Circle* end, float t, Circle* result) 
{
    result->cx = interpolate(start->cx, end->cx, t);
    result->cy = interpolate(start->cy, end->cy, t);
    result->r = interpolate(start->r, end->r, t);
}

// Function to generate an SVG file for the given circle, saving to cir folder
void write_svg(Circle* circle, int frame_number) 
{
    char filename[100];
    // Generate filenames for each frame in the output_5 folder
    sprintf(filename, "circle_to_circle/frame_%d.svg", frame_number);

    // Open the file for writing
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

// Function to load an SVG file and extract the first <circle> element's cx, cy, and r
int load_svg(const char* filename, Circle* circle) 
{
    printf("Loading SVG file: %s\n", filename);  // Print file loading info

    // Load the SVG XML file
    xmlDoc *doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", filename);
        return -1;
    }

    // Get the root element (should be <svg>)
    xmlNode *root_element = xmlDocGetRootElement(doc);
    if (root_element == NULL) {
        printf("Error: Could not find root element in %s\n", filename);
        xmlFreeDoc(doc);
        return -1;
    }

    // Traverse the children of <svg> to find the <circle> element
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"circle") == 0) {
            // Extract the 'cx', 'cy', and 'r' attributes
            xmlChar* cx_str = xmlGetProp(node, (const xmlChar *)"cx");
            xmlChar* cy_str = xmlGetProp(node, (const xmlChar *)"cy");
            xmlChar* r_str = xmlGetProp(node, (const xmlChar *)"r");

            // Check if the attributes were found
            if (cx_str == NULL || cy_str == NULL || r_str == NULL) {
                printf("Error: Missing 'cx', 'cy', or 'r' attributes in the circle element\n");
                xmlFreeDoc(doc);  // Free the document before returning
                return -1;
            }

            // Convert the attributes to floats and store them in the Circle struct
            circle->cx = atof((const char *)cx_str);
            circle->cy = atof((const char *)cy_str);
            circle->r = atof((const char *)r_str);

            // Free the XML strings after use
            xmlFree(cx_str);
            xmlFree(cy_str);
            xmlFree(r_str);

            // Print debug information
            printf("Loaded circle from %s: cx=%f, cy=%f, r=%f\n", filename, circle->cx, circle->cy, circle->r);
            break;  // Stop after finding the first circle
        }
        node = node->next;
    }

    // Free the document after parsing
    xmlFreeDoc(doc);

    return 0;
}

int main() 
{
    // Create the output_5 folder if it does not exist
    struct stat st = {0};
    if (stat("circle_to_circle", &st) == -1) {
        mkdir("circle_to_circle", 0700);  // Create the directory with rwx permissions for the owner
    }

    Circle small_circle, big_circle, result;

    // Load the SVG files for small and big circles from two directories up
    if (load_svg("../../svg/small_circle.svg", &small_circle) == -1) {
        printf("Failed to load ../../svg/small_circle.svg\n");
        return -1;
    }

    if (load_svg("../../svg/big_circle.svg", &big_circle) == -1) {
        printf("Failed to load ../../svg/big_circle.svg\n");
        return -1;
    }

    // Print loaded values for small_circle and big_circle
    printf("Small Circle - cx: %f, cy: %f, r: %f\n", small_circle.cx, small_circle.cy, small_circle.r);
    printf("Big Circle - cx: %f, cy: %f, r: %f\n", big_circle.cx, big_circle.cy, big_circle.r);

    // Loop to generate frames from t = 0 to t = 1, with 0.01 increments
    for (float t = 0.0; t <= 1.0; t += 0.01) {
        // Morph the small circle into the big circle based on the interpolation factor 't'
        morph(&small_circle, &big_circle, t, &result);

        // Generate the corresponding SVG file for the current frame
        write_svg(&result, (int)(t * 100));
    }

    return 0;
}
