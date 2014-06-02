#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <memory.h>

#include "image.h"

#define ET_SIZE 300
#define LARGE_FLOAT (99999.99)

typedef struct
{
	int num_of_bands;					// number of bands
	int num_of_pixels;					// number of pixels in the segment
	uint16 *pixel_x;					// coordinates of the pixels in the segment, [0] --- lines, [1] --- cols
	uint16 *pixel_y;					// coordinates of the pixels in the segment, [0] --- lines, [1] --- cols
	float *mean;						// mean of the pixels in the segment
	float *std;							// variance of the pixels in the segment
	int num_of_neighboring_segs;		// number of neighboring segments
	uint32 *neighboring_segs;			// list of neighboring segments		
	float min_dist;					    // mimimum distance from any of its neighboring segments
	uint32 min_dist_seg;				// which neighboring segment has this min_dist
	int deserted;						// 0 -- valid, 1 -- merged with another segment and thus deserted
} segment;

void Configure (int argc, char *argv[], image *img);
int segment1(image *in_image, image *out_map, int min_dist_th);
int segment2(image *in_image);

#endif
