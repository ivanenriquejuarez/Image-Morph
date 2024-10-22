In order to create an exe file you will need to download the libxml2 library

Linux:
sudo apt-get install libxml2
After the installation make sure you have the file located in:
/usr/include/

Command to create file:
gcc -o morph_svg morph.c -I/usr/include/libxml2 -lxml2 -lm -fopenmp

morph_svg will create the frames from 0-99 (total of 100) with the name: Frame_##.svg where the ## indicates the frame number
There will be an output message in the terminal when the files are created

When you run it you will notice some files will be created first than others
For example you might the see the following:

Created . . . . Frame_00.svg
Created . . . . Frame_32.svg
Created . . . . Frame_04.svg

Pragma OMP will automatically assign avaiable threads to the for loop inside.
For the sake of this project and learning we maunally did it ourselves.

We have a start point and an end point for each thread.
We also included an if statement to check if we have reached the end and no longer need to assign anything to a thread.

The parallelization works by dividing up the frames that each thread will work on:
For example,

If the only avaiable threads are two then it will divide the frames into those given frames. from 0-49 thread 1 and from 50-99 thread 2. It 
will then peform the interpolation.

Currently looking into different ways of running this in parallel to save even more time when it comes to more complex images. 
In theory if there is a lot involved for one particular image then we can divide that up into available thread. We
can try to accomplish this by reserving one thread for example in our main loop we can use maxThreads()-1 so that we know that one thread is not being used
at any time in the for loop and we can use that thread to divide up the calculations being done.
What does this mean?
Well say we have a complex shape and one of the threads job is to make a curved lines into straight lines. Well we can cut
the curved line in half and assign those vertices to two threads. Then wait and join the information before any of those threads
can be used again.

______Morph_2.c_____
In this version of parallelization we can take it a set further to do some calcuations in parallel as well. We see that here in  number of circle points

#pragma omp parallel for
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
                #pragma omp critical
                {
                    sprintf(point, "%f,%f ", interp_x, interp_y);
                    strcat(interpolated_points, point);
                }
            }
For simplicity we let omp parallel decide which threads do the work.

                #pragma omp critical
                {
                    sprintf(point, "%f,%f ", interp_x, interp_y);
                    strcat(interpolated_points, point);
                }

This portion of the code ensures that only one thread at a time to prevent data corruption.


At first I thought there would be some sort of conflict when assigning threads because we a parallel block and a parallel for block within it.
Fortunately OpenMP allows for nesting.

#pragma omp parallel creates a region where all threads execute the code within the block. Each thread
gets its own copy.

#pragma omp parallel for automatically divides the iterations of the loop among the available threads.
