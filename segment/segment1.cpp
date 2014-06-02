#include <stdlib.h>
#include <stdio.h>

#include "segment.h"
#include "utilities.h"
#include "image.h"

// out_map was initialized to 0 already
int segment1(image *in_image, image *out_map, int min_dist_th)
{
	int n_bands = in_image->num_of_bands;
	int n_lines = in_image->num_of_lines;
	int n_cols = in_image->num_of_samples;
	int n_seg = 0; // number of segments, first segment is named 1, second 2, and so on
	float pel_dist, min_dist;  // measurement of the distance between two pixels

	imgpel **map = (imgpel **) dim2 (n_lines, n_cols, sizeof(imgpel));

	for(int y=0; y<n_lines; y++)
	{
		for(int x=0; x<n_cols; x++)
		{
			if(map[y][x] == 0) // non-segmented pixel, check its eight neighbors for a potential merge; if no merge, start a new segment
			{
				min_dist = (float) LARGE_FLOAT;
				for(int j=y-1; j<=y+1; j++)
				{
					for(int i=x-1; i<=x+1; i++)
					{
						if(j>=0 && j<n_lines && i>=0 && i<n_cols && (j!=y || i!=x) && (map[j][i]>0))
						{
							pel_dist = 0;
							for(int k=0; k<n_bands; k++) 
								pel_dist += ((in_image->IMG[k][y][x] - in_image->IMG[k][j][i]) * (in_image->IMG[k][y][x] - in_image->IMG[k][j][i]));
							if(pel_dist < min_dist)
							{
								min_dist = pel_dist;
								map[y][x] = map[j][i];
							}
						}
					}
				}
				if (min_dist > min_dist_th * n_bands)
				{
					n_seg += 1;
					map[y][x] = (imgpel) n_seg;
				}
			}
		}
	}

	for(int y=0; y<n_lines; y++)
	{
		for(int x=0; x<n_cols; x++)
		{
			int boundary_flag = 0;
			for(int j=y-1; j<=y+1; j++)
			{
				for(int i=x-1; i<=x+1; i++)
				{
					int jj = clip(0, n_lines-1, j);
					int ii = clip(0, n_cols-1, i);
					if(map[jj][ii] != map[y][x])	boundary_flag = 1;		
				}
			}
			for(int k=0; k<n_bands; k++)
				out_map->IMG[k][y][x] = ((boundary_flag==1) ? 0xFFFF : in_image->IMG[k][y][x]);
		}
	}

	free2((char **)map);

	return n_seg;
}