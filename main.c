#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define NUM_FRAMES 20 // Number of steps/frames in the transition

// Struct to represent a vertex (for lines and move commands)
typedef struct {
    float x, y;
} Vertex;

// Function to scale a vertex by a factor (1 + t)
Vertex scale_vertex(Vertex point, float t, Vertex origin) {
    float scale_factor = 1 + t * 4;  // Scale from 1x to 5x for t ranging from 0 to 1
    Vertex scaled;
    scaled.x = origin.x + (point.x - origin.x) * scale_factor;
    scaled.y = origin.y + (point.y - origin.y) * scale_factor;
    return scaled;
}

// Helper function to extract a float from a string
float parse_float(const char** str) {
    char* end;
    float value = strtof(*str, &end);  // Convert the string to a float
    *str = end;  // Update the pointer to the next part of the string
    return value;
}

// Function to load an SVG file into a string
char* load_svg(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filepath);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory for the file content
    char* svg_data = (char*)malloc((file_size + 1) * sizeof(char));
    if (svg_data == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    fread(svg_data, sizeof(char), file_size, file);
    svg_data[file_size] = '\0';  // Null-terminate the string

    fclose(file);
    return svg_data;
}

// Function to extract all SVG shapes (paths, circles, rectangles, etc.)
char** extract_all_shapes(const char* svg_data, int* num_shapes) {
    const char* shape_start;
    const char* start_ptr = svg_data;
    char** shape_data = NULL;
    int count = 0;

    const char* tags[] = {"<path ", "<circle ", "<rect ", "<polygon ", "<polyline ", "<ellipse "};  // Tags we will handle
    const int num_tags = sizeof(tags) / sizeof(tags[0]);

    while (*start_ptr != '\0') {
        int found_tag = 0;
        for (int i = 0; i < num_tags; i++) {
            if ((shape_start = strstr(start_ptr, tags[i])) != NULL) {
                const char* end = strchr(shape_start, '>');  // Find the end of the shape tag
                if (end != NULL) {
                    size_t length = end - shape_start + 1;
                    shape_data = (char**)realloc(shape_data, (count + 1) * sizeof(char*));
                    shape_data[count] = (char*)malloc((length + 1) * sizeof(char));
                    strncpy(shape_data[count], shape_start, length);
                    shape_data[count][length] = '\0';
                    count++;
                    start_ptr = end + 1;
                    found_tag = 1;
                    break;
                }
            }
        }
        if (!found_tag) {
            start_ptr++;
        }
    }

    *num_shapes = count;
    return shape_data;
}

// Function to transform SVG paths (scaling)
void scale_path(const char* path_str, char* output, float t, Vertex origin) {
    const char* ptr = path_str;
    char command[3];
    float x, y;

    // Copy the command letters and scale the points
    while (*ptr != '\0') {
        if (sscanf(ptr, "%2s", command) == 1) {
            strcat(output, command);  // Copy the command (e.g., 'M', 'L', 'C')
            ptr += strlen(command);
            if (command[0] == 'M' || command[0] == 'L' || command[0] == 'C') {
                // For commands with coordinates, parse and scale them
                while (*ptr == ' ') ptr++;  // Skip spaces
                while (sscanf(ptr, "%f,%f", &x, &y) == 2) {
                    Vertex point = {x, y};
                    Vertex scaled_point = scale_vertex(point, t, origin);
                    sprintf(output + strlen(output), " %f,%f", scaled_point.x, scaled_point.y);
                    ptr = strchr(ptr, ' ') + 1;  // Move to the next coordinate pair
                }
            }
        }
        // Move to the next command
        while (*ptr != '\0' && !strchr("MLCZHV", *ptr)) ptr++;
    }
}

// Function to scale SVG shapes (scaling)
void transform_shape(const char* shape_data, char* transformed_data, float t, Vertex origin) {
    if (strstr(shape_data, "<path") != NULL) {
        // Extract the d attribute and scale it
        char* d_str = strstr(shape_data, "d=\"") + 3;
        char* d_end = strchr(d_str, '\"');
        char d[5000];
        strncpy(d, d_str, d_end - d_str);
        d[d_end - d_str] = '\0';

        // Scale the path commands
        strcpy(transformed_data, "<path d=\"");
        scale_path(d, transformed_data + strlen(transformed_data), t, origin);
        strcat(transformed_data, "\" />");

    } else if (strstr(shape_data, "<circle") != NULL) {
        // Extract the center and radius of the circle
        const char* ptr = shape_data;
        char* cx_str = strstr(ptr, "cx=\"");
        char* cy_str = strstr(ptr, "cy=\"");
        char* r_str = strstr(ptr, "r=\"");
        float cx = parse_float((const char**)&cx_str);
        float cy = parse_float((const char**)&cy_str);
        float r = parse_float((const char**)&r_str);

        // Scale the center and radius
        Vertex center = {cx, cy};
        Vertex scaled_center = scale_vertex(center, t, origin);
        float scaled_r = r * (1 + t * 4);

        // Store the transformed circle
        sprintf(transformed_data, "<circle cx=\"%f\" cy=\"%f\" r=\"%f\" />", scaled_center.x, scaled_center.y, scaled_r);
    } else if (strstr(shape_data, "<rect") != NULL) {
        // Extract x, y, width, height, and scale them
        const char* ptr = shape_data;
        char* x_str = strstr(ptr, "x=\"");
        char* y_str = strstr(ptr, "y=\"");
        char* width_str = strstr(ptr, "width=\"");
        char* height_str = strstr(ptr, "height=\"");
        float x = parse_float((const char**)&x_str);
        float y = parse_float((const char**)&y_str);
        float width = parse_float((const char**)&width_str);
        float height = parse_float((const char**)&height_str);

        // Scale the position and size
        Vertex pos = {x, y};
        Vertex scaled_pos = scale_vertex(pos, t, origin);
        float scaled_width = width * (1 + t * 4);
        float scaled_height = height * (1 + t * 4);

        // Store the transformed rectangle
        sprintf(transformed_data, "<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" />", scaled_pos.x, scaled_pos.y, scaled_width, scaled_height);
    } else {
        // For other shapes, just copy them directly
        strcpy(transformed_data, shape_data);
    }
}

// Function to save the SVG data to a file
void save_svg(char** transformed_shapes, int num_shapes, int frame_number) {
    char filename[50];
    sprintf(filename, "morph_frame_%d.svg", frame_number);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Start SVG file with a proper viewBox
    fprintf(file, "<svg width='800' height='800' viewBox='0 0 1024 1024' xmlns='http://www.w3.org/2000/svg'>\n");

    // Write all transformed shape data
    for (int i = 0; i < num_shapes; i++) {
        fprintf(file, "%s\n", transformed_shapes[i]);
    }

    // Close SVG file
    fprintf(file, "</svg>\n");
    fclose(file);
    printf("File %s created successfully.\n", filename);
}

int main() {
    // Load the SVG content from file
    char* svg_content = load_svg("accelerate-svgrepo-com.svg");
    if (svg_content == NULL) {
        return 1;
    }

    // Extract all shapes from the loaded SVG
    int num_shapes = 0;
    char** shape_data = extract_all_shapes(svg_content, &num_shapes);

    if (num_shapes == 0) {
        free(svg_content);
        return 1;
    }

    // Buffer to store the transformed shape data for each shape
    char transformed_data[10000];  // Ensure the buffer is large enough to hold transformed shape data
    char** transformed_shapes = (char**)malloc(num_shapes * sizeof(char*));

    // Define origin (center point for scaling)
    Vertex origin = {512.0, 512.0};  // Center of the SVG

    // Perform transition and save each step into SVG files
    for (int frame = 0; frame <= NUM_FRAMES; frame++) {
        float t = (float)frame / NUM_FRAMES;  // Incremental step between 0 and 1

        // Transform each shape and store the result
        for (int i = 0; i < num_shapes; i++) {
            transformed_shapes[i] = (char*)malloc(10000 * sizeof(char));  // Allocate buffer for each shape
            transform_shape(shape_data[i], transformed_shapes[i], t, origin);  // Apply scaling transformation
        }

        // Save the current frame as an SVG file
        save_svg(transformed_shapes, num_shapes, frame);

        // Free each transformed shape buffer after saving
        for (int i = 0; i < num_shapes; i++) {
            free(transformed_shapes[i]);
        }
    }

    // Free dynamically allocated memory
    free(svg_content);
    for (int i = 0; i < num_shapes; i++) {
        free(shape_data[i]);
    }
    free(shape_data);
    free(transformed_shapes);

    return 0;
}
