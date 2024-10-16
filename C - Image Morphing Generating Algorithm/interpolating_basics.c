/*
Interpolating basics: interpolation of two numbers, 
the idea is to find an in-between value depending on 
a factor of 't' where it is (0 and 1).
*/

#include <stdio.h>

int main()
{
	//Starting point(source) and ending point(target)
	int start = 10;
	int end = 20;
	
	//Interpolation factor t (0.0 means 100% start, 1.0 means 100% end)
	float t = 0.5;
	
	//The interpolation formula: difference between end and start value with respect to 't'
	int result = start + t * (end - start);
	
	//Print result
	printf("The interpolated value is: %d\n", result);
	
	return 0;
}

