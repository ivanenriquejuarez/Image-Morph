#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libxml/parser.h> //xml reads file library
#include <libxml/tree.h> // extracts the points from the svg
#include <math.h>
#include <string.h>
#include <time.h>  
#include <sys/time.h>  //time execution

// Function prototypes for functions defined later
int extract_circle_info(const char* svg_file, float* cx, float* cy, float* r);
int extract_triangle_info(const char* svg_file, float triangle[3][2]);
float bezier_point(float p0, float p1, float p2, float t);
void write_svg(char* interpolated_points, int frame_number);

// Function to extract circle attributes (cx, cy, r) from an SVG file
int extract_circle_info(const char* svg_file, float* cx, float* cy, float* r)
 {
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);  // Load the SVG file as an XML document
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;  // Return -1 if file loading fails
    }

    // Access the root element and iterate to find <circle>
    xmlNode *root_element = xmlDocGetRootElement(doc);
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"circle") == 0) {
            // Extract 'cx', 'cy', and 'r' attributes from <circle>
            xmlChar* cx_str = xmlGetProp(node, (const xmlChar *)"cx");
            xmlChar* cy_str = xmlGetProp(node, (const xmlChar *)"cy");
            xmlChar* r_str = xmlGetProp(node, (const xmlChar *)"r");

            if (cx_str && cy_str && r_str) {  // If all attributes are present
                *cx = atof((const char *)cx_str);  // Convert cx to float and assign
                *cy = atof((const char *)cy_str);  // Convert cy to float and assign
                *r = atof((const char *)r_str);    // Convert r to float and assign

                // Free allocated memory for attribute strings
                xmlFree(cx_str);
                xmlFree(cy_str);
                xmlFree(r_str);
                xmlFreeDoc(doc);  // Free the document after parsing
                return 0;  // Successfully extracted attributes
            }
        }
        node = node->next;  // Move to the next node
    }

    xmlFreeDoc(doc);  // Free the document if no <circle> is found
    return -1;  // Return -1 if circle information could not be extracted
}

// Function to extract triangle points from an SVG file's <polygon> element
int extract_triangle_info(const char* svg_file, float triangle[3][2]) 
{
    xmlDoc *doc = xmlReadFile(svg_file, NULL, 0);  // Load the SVG file as an XML document
    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", svg_file);
        return -1;
    }

    // Aaccess the root element and find <polygon> representing a triangle
    xmlNode *root_element = xmlDocGetRootElement(doc);
    xmlNode *node = root_element->children;
    while (node) {
        if (node->type == XML_ELEMENT_NODE && xmlStrcmp(node->name, (const xmlChar *)"polygon") == 0) {
            xmlChar* points_str = xmlGetProp(node, (const xmlChar *)"points");

            if (points_str) {  //Extract 'points' attribute containing triangle vertices
                sscanf((const char *)points_str, "%f,%f %f,%f %f,%f",
                       &triangle[0][0], &triangle[0][1],  // Vertex 1
                       &triangle[1][0], &triangle[1][1],  // Vertex 2
                       &triangle[2][0], &triangle[2][1]); // Vertex 3

                xmlFree(points_str);  // Free the string after use
                xmlFreeDoc(doc);  // 	 Free the document
                return 0;  // 			 Successfully extracted triangle points
            }
        }
        node = node->next;  // Move to the next node
    }

    xmlFreeDoc(doc);  // Free the document if no <polygon> is found
    return -1;  // Return -1 if triangle points could not be extracted
}

// Function to calculate a quadratic Bézier curve point given three points and a factor `t`
float bezier_point(float p0, float p1, float p2, float t) 
{
    // Quadratic Bézier formula to smoothly transition from p0 to p2, with p1 as control
    return pow(1 - t, 2) * p0 + 2 * (1 - t) * t * p1 + t * t * p2;
}

// Function to save the current interpolated frame to an SVG file
void write_svg(char* interpolated_points, int frame_number) 
{
    // Generate unique filename for each frame
    char filename[256];
    sprintf(filename, "./circle_to_triangle/frame_%03d.svg", frame_number);

	//Open the file for writing and write.
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }
	
	//We write the template to make this an svg and write in the polygon points within the child nodes
    fprintf(file, "<svg width='500' height='500' xmlns='http://www.w3.org/2000/svg'>\n");
    fprintf(file, "  <polygon points='%s' fill='blue' />\n", interpolated_points);
    fprintf(file, "</svg>\n");

    fclose(file);  // Close the file after writing
    printf("File %s created successfully.\n", filename);
}

// main func
int main() 
{
	//stop watch object
	struct timeval start, end;
    gettimeofday(&start, NULL);  // record the wall-clock start time

    // Variables for circle center and radius
    float cx, cy, r;
    float triangle_vertices[3][2];  // Array to store triangle vertices

    // Load circle information from the SVG file
    if (extract_circle_info("../../svg/small_circle.svg", &cx, &cy, &r) == -1) {
        printf("Error: Could not extract circle info.\n");
        return -1;
    }

    // Load triangle vertex coordinates from the SVG file
    if (extract_triangle_info("../../svg/triangle.svg", triangle_vertices) == -1) {
        printf("Error: Could not extract triangle info.\n");
        return -1;
    }

    int num_circle_points = 30;  // Number of points along the circle to morph
    float control_points[3][2] = {  // Control points for Bézier curve transition
        {cx + 0.5 * r, cy},         // Control point for 1st vertex
        {cx, cy - 0.5 * r},         // Control point for 2nd vertex
        {cx - 0.5 * r, cy}          // Control point for 3rd vertex
    };

    int total_frames = 100000;  // Total number of frames in the morphing animation
    for (int frame = 0; frame < total_frames; frame++) {
        float t = (float)frame / (total_frames - 1);  // `t` ranges from 0 to 1 smoothly
		
		// Couldn't have used the bezier curve method without these lines of code from 3rd source ( 159 - 175)
        char interpolated_points[1024] = "";  // String to hold interpolated points for SVG
        char point[50];  // Temporary buffer to store each interpolated point

        // Calculate each interpolated point on the circle-to-triangle morph
        for (int i = 0; i < num_circle_points; i++) {
            // Calculate initial point on the circle
            float angle = (2 * M_PI / num_circle_points) * i;
            float circle_x = cx + r * cos(angle);
            float circle_y = cy + r * sin(angle);

            // Determine target vertex for this point and corresponding control point
            float* triangle_vertex = triangle_vertices[i % 3];
            float* control_point = control_points[i % 3];

            // Interpolate position using Bézier curve
            float interp_x = bezier_point(circle_x, control_point[0], triangle_vertex[0], t);
            float interp_y = bezier_point(circle_y, control_point[1], triangle_vertex[1], t);

            sprintf(point, "%f,%f ", interp_x, interp_y);  // Format point as "x,y"
            strcat(interpolated_points, point);  // Append to SVG point string
        }
		// Remove the trailing space from the interpolated points string
        interpolated_points[strlen(interpolated_points) - 1] = '\0';

        // Write the current frame's interpolated points to an SVG file
        write_svg(interpolated_points, frame);
    }

    gettimeofday(&end, NULL);  // Record the wall-clock end time

    // Calculate the time difference in seconds with millisecond precision
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Execution Time: %.3f seconds\n", time_taken);

    return 0;
}
