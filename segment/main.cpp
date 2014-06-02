#include <stdlib.h>
#include <stdio.h>
#include "segment.h"
#include "utilities.h"
#include "image.h"

char errortext[ET_SIZE];

int main(int argc, char *argv[])
{
	image in_image;
	Configure (argc, argv, &in_image);
	allocate_image(&in_image);
	read_image(&in_image);
	int num_of_segments = segment2(&in_image);
	printf("number of segments: %d\n", num_of_segments);
	write_map(&in_image);
	free_image(&in_image);
	printf("done\n");

	return 0;
}







