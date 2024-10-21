#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265358979323846
#define NUM_FRAMES 40 // Number of steps/frames in the transition

// Struct to represent a vertex (for lines and move commands)
typedef struct {
    float x, y;
} Vertex;

// Function to interpolate between two values
float interpolate(float start, float end, float t) {
    return start + t * (end - start);
}

// Function to rotate a vertex by t * 90 degrees clockwise (incremental rotation)
Vertex rotate_vertex(Vertex point, float t) {
    float angle = t * PI / 2;  // t * 90 degrees in radians
    Vertex rotated;
    rotated.x = point.x * cos(angle) + point.y * sin(angle);
    rotated.y = -point.x * sin(angle) + point.y * cos(angle);
    return rotated;
}

// Function to move (translate) a vertex to the right by t * dx units (incremental translation)
Vertex translate_vertex(Vertex point, float t, float dx) {
    point.x += t * dx;               // Incrementally move x to the right
    return point;
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

// Function to extract all <path> elements from the SVG file
char** extract_all_paths(const char* svg_data, int* num_paths) {
    const char* start_ptr = svg_data;
    const char* path_start;
    char** path_data = NULL;
    int count = 0;

    while ((path_start = strstr(start_ptr, "<path d=\"")) != NULL) {
        start_ptr = path_start + strlen("<path d=\"");
        const char* path_end = strchr(start_ptr, '\"');

        if (path_end == NULL) {
            break;
        }

        size_t length = path_end - start_ptr;
        path_data = (char**)realloc(path_data, (count + 1) * sizeof(char*));
        path_data[count] = (char*)malloc((length + 1) * sizeof(char));
        strncpy(path_data[count], start_ptr, length);
        path_data[count][length] = '\0';

        count++;
        start_ptr = path_end + 1;
    }

    *num_paths = count;
    return path_data;
}

// Function to transform SVG path data and store it into the buffer
void transform_svg_path(const char* svg_path_data, char* transformed_data, float t, float dx) {
    const char* ptr = svg_path_data;
    char* output = transformed_data;

    Vertex current_pos = {0, 0};  // Track current position

    while (*ptr != '\0') {
        if (*ptr == 'M') {
            // Move command (Move to new position)
            *output++ = 'M';
            ptr++;
            current_pos.x = parse_float(&ptr);
            current_pos.y = parse_float(&ptr);
            Vertex transformed = translate_vertex(rotate_vertex(current_pos, t), t, dx);
            output += sprintf(output, " %f,%f", transformed.x, transformed.y);
        } else if (*ptr == 'L') {
            // Line command (Draw a line from current position to a new position)
            *output++ = 'L';
            ptr++;
            Vertex end;
            end.x = parse_float(&ptr);
            end.y = parse_float(&ptr);
            Vertex transformed = translate_vertex(rotate_vertex(end, t), t, dx);
            output += sprintf(output, " %f,%f", transformed.x, transformed.y);
        } else if (*ptr == 'C') {
            // Cubic Bezier curve command
            *output++ = 'C';
            ptr++;
            Vertex control1, control2, end;
            control1.x = parse_float(&ptr);
            control1.y = parse_float(&ptr);
            control2.x = parse_float(&ptr);
            control2.y = parse_float(&ptr);
            end.x = parse_float(&ptr);
            end.y = parse_float(&ptr);

            Vertex transformed_control1 = translate_vertex(rotate_vertex(control1, t), t, dx);
            Vertex transformed_control2 = translate_vertex(rotate_vertex(control2, t), t, dx);
            Vertex transformed_end = translate_vertex(rotate_vertex(end, t), t, dx);

            output += sprintf(output, " %f,%f %f,%f %f,%f", transformed_control1.x, transformed_control1.y, transformed_control2.x, transformed_control2.y, transformed_end.x, transformed_end.y);
        } else if (*ptr == 'Z') {
            // Close path command
            *output++ = 'Z';
            ptr++;
        } else {
            // Copy any other character (like spaces)
            *output++ = *ptr++;
        }
    }
    *output = '\0';  // Null-terminate the transformed data
}

// Function to save the SVG data to a file
void save_svg(char** transformed_paths, int num_paths, int frame_number) {
    char filename[50];
    sprintf(filename, "morph_frame_%d.svg", frame_number);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Start SVG file with a proper viewBox
    fprintf(file, "<svg width='800' height='800' viewBox='0 0 1024 1024' xmlns='http://www.w3.org/2000/svg'>\n");

    // Write all transformed path data
    for (int i = 0; i < num_paths; i++) {
        fprintf(file, "<path d=\"%s\" fill=\"none\" stroke=\"black\" stroke-width=\"2\" />\n", transformed_paths[i]);
    }

    // Close SVG file
    fprintf(file, "</svg>\n");
    fclose(file);
    printf("File %s created successfully.\n", filename);
}

int main() {
    // Load the SVG content from file
    char* svg_content = load_svg("accelerate-svg-repo-com.svg");
    if (svg_content == NULL) {
        return 1;
    }

    // Extract all paths from the loaded SVG
    int num_paths = 0;
    char** path_data = extract_all_paths(svg_content, &num_paths);

    if (num_paths == 0) {
        free(svg_content);
        return 1;
    }

    // Buffer to store the transformed path data for each path
    char transformed_data[10000];  // Ensure the buffer is large enough to hold transformed path data
    char** transformed_paths = (char**)malloc(num_paths * sizeof(char*));

    // Perform transition and save each step into SVG files
    for (int frame = 0; frame <= NUM_FRAMES; frame++) {
        float t = (float)frame / NUM_FRAMES;  // Incremental step between 0 and 1

        // Transform each path and store the result
        for (int i = 0; i < num_paths; i++) {
            transformed_paths[i] = (char*)malloc(10000 * sizeof(char));  // Allocate buffer for each path
            transform_svg_path(path_data[i], transformed_paths[i], t, 100.0);  // Apply rotation and translation
        }

        // Save the current frame as an SVG file
        save_svg(transformed_paths, num_paths, frame);

        // Free each transformed path buffer after saving
        for (int i = 0; i < num_paths; i++) {
            free(transformed_paths[i]);
        }
    }

    // Free dynamically allocated memory
    free(svg_content);
    for (int i = 0; i < num_paths; i++) {
        free(path_data[i]);
    }
    free(path_data);
    free(transformed_paths);

    return 0;
}
