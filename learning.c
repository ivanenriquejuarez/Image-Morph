#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_PATHS 100  // Maximum number of paths
#define MAX_PATH_LENGTH 1000  // Maximum length of path data
#define INTERVALS 10  // Number of intervals for growth

// Struct to represent a vertex (coordinate point)
typedef struct {
    float x, y;
} Vertex;

// Struct to store a path's data
typedef struct {
    char path_data[MAX_PATH_LENGTH];  // Path's "d" attribute data
    char fill_color[20];  // Optional: fill color of the path
} SVGPath;

// Function to load an SVG file into a string
char* load_svg(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filepath);
        return NULL;
    }

    // Get the file size
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

// Function to extract paths and their data from the SVG content
int extract_paths(const char* svg_content, SVGPath* paths) {
    const char* path_start = svg_content;
    int path_count = 0;

    while ((path_start = strstr(path_start, "<path ")) != NULL) {
        const char* d_attr = strstr(path_start, "d=\"");
        if (d_attr) {
            d_attr += 3;  // Move past 'd="'

            const char* d_end = strchr(d_attr, '\"');  // Find end of the path data
            if (d_end) {
                size_t length = d_end - d_attr;
                if (length >= MAX_PATH_LENGTH) {
                    printf("Warning: Path data too long, truncating.\n");
                    length = MAX_PATH_LENGTH - 1;
                }
                strncpy(paths[path_count].path_data, d_attr, length);
                paths[path_count].path_data[length] = '\0';  // Null-terminate the path data

                // Optionally, extract the fill color
                const char* fill_attr = strstr(path_start, "fill=\"");
                if (fill_attr) {
                    fill_attr += 6;  // Move past 'fill="'
                    const char* fill_end = strchr(fill_attr, '\"');
                    if (fill_end) {
                        size_t fill_length = fill_end - fill_attr;
                        strncpy(paths[path_count].fill_color, fill_attr, fill_length);
                        paths[path_count].fill_color[fill_length] = '\0';
                    }
                } else {
                    strcpy(paths[path_count].fill_color, "none");  // Default fill color
                }

                path_count++;
                if (path_count >= MAX_PATHS) {
                    printf("Warning: Maximum path limit reached.\n");
                    break;
                }
            }
        }
        path_start++;
    }

    return path_count;
}

// Function to scale a vertex by a factor
Vertex scale_vertex(Vertex point, float scale_factor, Vertex origin) {
    Vertex scaled;
    scaled.x = origin.x + (point.x - origin.x) * scale_factor;
    scaled.y = origin.y + (point.y - origin.y) * scale_factor;
    return scaled;
}

// Helper function to parse a float from a string
float parse_float(const char** str) {
    char* end;
    float value = strtof(*str, &end);  // Convert string to float
    *str = end;  // Move pointer to the next part of the string
    return value;
}

// Function to grow the first path in 10 intervals
void grow_first_path(SVGPath* path, Vertex origin) {
    char transformed_path[MAX_PATH_LENGTH];
    const char* ptr = path->path_data;
    char command[3];
    float x, y;
    Vertex point;

    // Perform 10 intervals of growth
    for (int step = 1; step <= INTERVALS; step++) {
        float scale_factor = 1.0 + (float)step / INTERVALS;  // Scaling factor from 1x to 2x
        const char* path_ptr = ptr;  // Reset pointer to the start of the original path
        char* out_ptr = transformed_path;  // Pointer for the output path

        // Iterate through the original path commands and scale the coordinates
        while (*path_ptr != '\0') {
            if (sscanf(path_ptr, "%2s", command) == 1) {
                // Copy the command (e.g., 'M', 'L', 'C')
                out_ptr += sprintf(out_ptr, "%s", command);
                path_ptr += strlen(command);

                // Scale the coordinates based on the command
                if (command[0] == 'M' || command[0] == 'L' || command[0] == 'C') {
                    while (*path_ptr == ' ') path_ptr++;  // Skip spaces
                    while (sscanf(path_ptr, "%f,%f", &x, &y) == 2) {
                        point.x = x;
                        point.y = y;
                        Vertex scaled_point = scale_vertex(point, scale_factor, origin);
                        out_ptr += sprintf(out_ptr, " %f,%f", scaled_point.x, scaled_point.y);
                        path_ptr = strchr(path_ptr, ' ') + 1;  // Move to the next coordinate pair
                    }
                }
            }
            // Move to the next command or character
            while (*path_ptr != '\0' && !strchr("MLCZHV", *path_ptr)) path_ptr++;
        }
        *out_ptr = '\0';  // Null-terminate the transformed path

        // Print the transformed path for this interval
        printf("Interval %d Path: %s\n", step, transformed_path);
    }
}

int main() {
    const char* svg_file = "accelerate-svgrepo-com.svg";  // Replace with your SVG file
    char* svg_content = load_svg(svg_file);
    if (svg_content == NULL) {
        return 1;  // Exit if file couldn't be loaded
    }

    // Array to store the extracted paths
    SVGPath paths[MAX_PATHS];

    // Extract paths from the SVG content
    int num_paths = extract_paths(svg_content, paths);
    if (num_paths == 0) {
        printf("No paths found in the SVG file.\n");
        free(svg_content);
        return 1;
    }

    // Print the extracted paths
    printf("Extracted %d paths from the SVG:\n", num_paths);
    for (int i = 0; i < num_paths; i++) {
        printf("Path %d: %s, Fill Color: %s\n", i + 1, paths[i].path_data, paths[i].fill_color);
    }

    // Define origin for scaling (center of the SVG or any desired point)
    Vertex origin = {512.0, 512.0};

    // Grow the first path
    if (num_paths > 0) {
        grow_first_path(&paths[0], origin);
    }

    // Free allocated memory
    free(svg_content);

    return 0;
}
