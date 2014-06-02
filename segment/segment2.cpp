#include <stdlib.h>
#include <stdio.h>

#include "segment.h"
#include "utilities.h"
#include "image.h"

// print out the data elements of a segment
void print_seg(segment ** SEG, int n)
{
	printf("\n\nSEG[%d]:\n", n);
	printf("num_of_pixels = %d\n", SEG[n]->num_of_pixels);
	printf("pixels: "); 
	for(int i=0; i<SEG[n]->num_of_pixels; i++) 
	{
		if(i%16==0) printf("\n");
		printf("(%4d,%4d) ", SEG[n]->pixel_x[i], SEG[n]->pixel_y[i]);
	}
	printf("\n");
	printf("mean = ( "); for(int b=0; b<SEG[n]->num_of_bands; b++) printf("%.2f ", SEG[n]->mean[b]);  printf(")\n"); 
	printf("std = ( ");  for(int b=0; b<SEG[n]->num_of_bands; b++) printf("%.2f ", SEG[n]->std[b]);   printf(")\n"); 
	printf("num_of_neighboring_segs = %d\n", SEG[n]->num_of_neighboring_segs);
	printf("neighboring segs: "); 
	for(int i=0; i<SEG[n]->num_of_neighboring_segs; i++) printf("%d ", SEG[n]->neighboring_segs[i]);
	printf("\n");
	printf("min_dist = %d\n", SEG[n]->min_dist);
	printf("min_dist_seg = %d\n", SEG[n]->min_dist_seg);
	printf("deserted = %d\n", SEG[n]->deserted);
	printf("\n\n");
}

void free_segment(segment ** SEG, uint32 s)
{
	free(SEG[s]->mean);
	free(SEG[s]->std);
	free(SEG[s]->neighboring_segs);
	free(SEG[s]->pixel_x);
	free(SEG[s]->pixel_y);
	// free(SEG[s]);
}


// merge two segments, with s1 is the host, and s2 to be deserted
int merge_two_segments(image *I, segment ** SEG, uint32 s1, uint32 s2)
{
	int32 b, m, n, i, j, k;
	uint32 s3, s4; 
	float dist;
	int n_bands = I->num_of_bands;

	// step 1: merging pixels of s1 and s2, re-calculate mean and std of s1
	m = SEG[s1]->num_of_pixels + SEG[s2]->num_of_pixels;
	SEG[s1]->pixel_x = (uint16 *) realloc (SEG[s1]->pixel_x, m * sizeof(uint16));
	SEG[s1]->pixel_y = (uint16 *) realloc (SEG[s1]->pixel_y, m * sizeof(uint16));
	if(SEG[s1]->pixel_x==NULL || SEG[s2]->pixel_y==NULL)
	{
		printf("failed to allocate memory: (merge_two_segments, SEG[[s1]->pixel_x, SEG[[s1]->pixel_y)\n");
		return -1;
	}
	memcpy(SEG[s1]->pixel_x+SEG[s1]->num_of_pixels, SEG[s2]->pixel_x, SEG[s2]->num_of_pixels*sizeof(uint16));
	memcpy(SEG[s1]->pixel_y+SEG[s1]->num_of_pixels, SEG[s2]->pixel_y, SEG[s2]->num_of_pixels*sizeof(uint16));
	for(b=0; b<n_bands; b++) SEG[s1]->mean[b] = (SEG[s1]->mean[b]*SEG[s1]->num_of_pixels + SEG[s2]->mean[b]*SEG[s2]->num_of_pixels) / m ;
	SEG[s1]->num_of_pixels = m;
	imgpel *pix = (imgpel *) malloc ( m * sizeof(imgpel));
	if(pix == NULL)
	{
		printf("failed to allocate memory: (segment2, pix)\n");
		return -1;
	}
	for(b=0; b<n_bands; b++)
	{
		for(k=0; k<m; k++) 
		{
			int xx = SEG[s1]->pixel_x[k];
			int yy = SEG[s1]->pixel_y[k];
			pix[k] = I->IMG[b][yy][xx];
		}
		SEG[s1]->std[b] = standard_dev(pix, m);
	}
	free(pix);

	// step 2: merging the neighbors of s2 into the neighbors of s1
	uint32* temp = (uint32 *) malloc ( SEG[s2]->num_of_neighboring_segs * sizeof(uint32));
	if(temp == NULL)
	{
		printf("failed to allocate memory: (segment2, temp)\n");
		return -1;
	}
	for(n=i=0; i<SEG[s2]->num_of_neighboring_segs; i++)
	{
		s3 = SEG[s2]->neighboring_segs[i];
		int appeared = 0; // check if s3 appears in the neighbor list of s1, or s3 is s1 itself
		for(j=0; j<SEG[s1]->num_of_neighboring_segs; j++)
		{
			s4 = SEG[s1]->neighboring_segs[j];
			if ((s4 == s3) || (s3 == s1))
			{
				appeared = 1; 
				break;
			}
		}
		if(appeared==0)	temp[n++] = s3;
	}
	for(i=0; SEG[s1]->neighboring_segs[i]!=s2; i++); // remove s2 from the neighbor list of s1
	memmove(SEG[s1]->neighboring_segs+i, SEG[s1]->neighboring_segs+i+1, (SEG[s1]->num_of_neighboring_segs-1-i) * sizeof(uint32));
	SEG[s1]->neighboring_segs = (uint32 *) realloc (SEG[s1]->neighboring_segs, (SEG[s1]->num_of_neighboring_segs + n - 1) * sizeof(uint32));
	if(SEG[s1]->neighboring_segs == NULL)
	{
		printf("failed to allocate memory: (segment2, SEG[s1]->neighboring_segs)\n");
		return -1;
	}
	memcpy(SEG[s1]->neighboring_segs + SEG[s1]->num_of_neighboring_segs - 1, temp, n*sizeof(uint32));
	free(temp);
	SEG[s1]->num_of_neighboring_segs += (n-1);

	// step 3: re-caluate min_dist for s1
	SEG[s1]->min_dist = (float) LARGE_FLOAT;
	SEG[s1]->min_dist_seg = 0;
	for(k=0; k<SEG[s1]->num_of_neighboring_segs; k++)
	{
		s3 = SEG[s1]->neighboring_segs[k];
		double dist_d = 0.0;
		for(b=0; b<n_bands; b++) dist_d += (SEG[s1]->mean[b]-SEG[s3]->mean[b]) * (SEG[s1]->mean[b]-SEG[s3]->mean[b]);
		dist = (float) sqrt(dist_d);
		if(dist < SEG[s1]->min_dist)
		{
			SEG[s1]->min_dist = dist;
			SEG[s1]->min_dist_seg = s3;
		}
	}

	// step 4: update the neighboring segments
	for(k=0; k<SEG[s1]->num_of_neighboring_segs; k++)
	{
		s3 = SEG[s1]->neighboring_segs[k];
		int pos_s1 = -1; int pos_s2 = -1;	// find the positions of s1 and s2, if they appear in the neighbor list of s3
		for(i=0; i<SEG[s3]->num_of_neighboring_segs; i++)
		{
			if(SEG[s3]->neighboring_segs[i] == s1) pos_s1 = i;
			if(SEG[s3]->neighboring_segs[i] == s2) pos_s2 = i;
		}
		if       (pos_s1==-1) SEG[s3]->neighboring_segs[pos_s2] = s1; // if s1 does not appear, then s2 must appear in the neighbor list of s3
		else if  (pos_s1!=-1 && pos_s2!=-1) // both may appear, if so, remove s2
		{
			memmove(SEG[s3]->neighboring_segs + pos_s2, SEG[s3]->neighboring_segs + pos_s2 + 1, (SEG[s3]->num_of_neighboring_segs-pos_s2-1)*sizeof(uint32));
			SEG[s3]->num_of_neighboring_segs -= 1;
		}
		SEG[s3]->min_dist = (float) LARGE_FLOAT;
		SEG[s3]->min_dist_seg = 0;
		for(j=0; j<SEG[s3]->num_of_neighboring_segs; j++)
		{
			s4 = SEG[s3]->neighboring_segs[j];
			double dist_d = 0.0;
			for(b=0; b<n_bands; b++) dist_d += (SEG[s3]->mean[b]-SEG[s4]->mean[b]) * (SEG[s3]->mean[b]-SEG[s4]->mean[b]);
			dist = (float) sqrt(dist_d);
			if(dist < SEG[s3]->min_dist)
			{
				SEG[s3]->min_dist = dist;
				SEG[s3]->min_dist_seg = s4;
			}
		}
	}

	// step 5: desert s2
	SEG[s2]->deserted = 1;
	free_segment(SEG, s2);

	return 0;
}

int segment2(image *I)
{
	int i, j, k, n, x, y, b;

	int n_bands = I->num_of_bands;
	int n_lines = I->num_of_lines;
	int n_cols = I->num_of_samples;

	uint32 s1, s2;
	int max_num_of_segs = n_lines * n_cols;		// initially, each pixel is a segment
	int num_of_segs = n_lines * n_cols;			// initially, each pixel is a segment
	float dist, min_min_dist;	// minimum of the min_dist
	int min_min_dist_seg;		// which segment has this min_min_dist

	// inilialization of SEG
	segment **SEG = (segment **) malloc (num_of_segs * sizeof(segment *));
	if(SEG == NULL) 
	{
		printf("failed to allocate memory for SEG.\n");
		return -1;
	}
	for(i=0; i<max_num_of_segs; i++)
	{
		SEG[i] = (segment *) malloc (sizeof(segment));
		if(SEG[i]==NULL)
		{
			printf("failed to allocate memory for SEG[n].\n");
			return -1;
		}
	}
	for (n=y=0; y<n_lines; y++)
	{
		for(x=0; x<n_cols; x++, n++)
		{
			SEG[n]->num_of_bands = n_bands;
			SEG[n]->num_of_pixels = 1;
			SEG[n]->pixel_x = (uint16 *) malloc (1 * sizeof(uint16));
			SEG[n]->pixel_y = (uint16 *) malloc (1 * sizeof(uint16));
			if(SEG[n]->pixel_x==NULL || SEG[n]->pixel_y==NULL)
			{
				printf("failed to allocate memory: (segment2, SEG[n]->pixel_x, SEG[n]->pixel_y)\n");
				return -1;
			}
			SEG[n]->pixel_x[0] = (uint16) x;
			SEG[n]->pixel_y[0] = (uint16) y;
			SEG[n]->mean = (float *) malloc (n_bands*sizeof(float));
			SEG[n]->std = (float *) malloc (n_bands*sizeof(float));
			if ((SEG[n]->mean == NULL) || (SEG[n]->std == NULL))
			{
				printf("failed to allocate memory: (segment2, SEG[n]->mean, SEG[n]->std)\n");
				return -1;
			}
			for(b=0; b<n_bands; b++) 
			{
				SEG[n]->mean[b] = I->IMG[b][y][x];
				SEG[n]->std[b] = 0;
			}
			if	(  (x==0 && y==0)          || (x==0 && y==(n_lines-1)) 
				|| (x==(n_cols-1) && y==0) || (x==(n_cols-1) && y==(n_lines-1)))	SEG[n]->num_of_neighboring_segs = 3;
			else if (x==0 || y==0 || x==(n_cols-1) || y==(n_lines-1))				SEG[n]->num_of_neighboring_segs = 5;
			else																	SEG[n]->num_of_neighboring_segs = 8;
			SEG[n]->neighboring_segs = (uint32 *) malloc (SEG[n]->num_of_neighboring_segs * sizeof(uint32));
			if(SEG[n]->neighboring_segs == NULL)
			{
				printf("failed to allocate memory: (segment2, SEG[n]->neighboring_segs)\n");
				return -1;
			}
			SEG[n]->min_dist = (float)LARGE_FLOAT;
			SEG[n]->min_dist_seg = 0;
			for(k=0,j=-1; j<=1; j++) for(i=-1; i<=1; i++)
			{
				if(y+j>=0 && y+j<n_lines && x+i>=0 && x+i<n_cols && (j!=0 || i!=0))
				{
					SEG[n]->neighboring_segs[k] = (y+j)*n_cols+x+i; 
					double dist_d = 0.0;
					for(b=0; b<n_bands; b++) dist_d += (I->IMG[b][y][x]-I->IMG[b][y+j][x+i])*(I->IMG[b][y][x]-I->IMG[b][y+j][x+i]);
					dist = (float) sqrt(dist_d);
					if(dist < SEG[n]->min_dist)
					{
						SEG[n]->min_dist = dist;
						SEG[n]->min_dist_seg = SEG[n]->neighboring_segs[k];
					}
					k += 1;
				}
			}
			SEG[n]->deserted = 0;
		}
	}

	// iterative merging: 
	// during each loop, a pair of segments are identified according minimum (optionally variance) conditions
	// then, these two segments are merged into one segment.
	while(1)
	{
		printf("n_segs = %d\n", num_of_segs);

		// step 1: find the minimum distance among all segments, variance condition may apply as well depending on user input.
		min_min_dist = (float) LARGE_FLOAT;
		min_min_dist_seg = 0;
		for(k=0; k<max_num_of_segs; k++)	// can be improved later by "condensing" SEG, i.e., deserted segments can be removed.
		{
			if( SEG[k]->deserted == 0 ) 
			{
				s1 = k;
				s2 = SEG[k]->min_dist_seg;
				int size_s1 = SEG[s1]->num_of_pixels;
				int size_s2 = SEG[s2]->num_of_pixels;
				if (size_s1>I->seg_size_th && size_s2>I->seg_size_th)	// to merge large segments, we (optionally) need to check their variances.
				{
					int sim_flag = 1;
					for(int b=0; b<n_bands; b++)
					{
						float mean_s1 = SEG[s1]->mean[b];
						float mean_s2 = SEG[s2]->mean[b];
						float mean_s = (size_s1*mean_s1+size_s2*mean_s2)/(size_s1+size_s2);
						float var_s1 = SEG[s1]->std[b] * SEG[s1]->std[b];
						float var_s2 = SEG[s2]->std[b] * SEG[s2]->std[b];
						float var_s = (var_s1+mean_s1*mean_s1)*size_s1/(size_s1+size_s2) + (var_s2+mean_s2*mean_s2)*size_s2/(size_s1+size_s2) - mean_s*mean_s;
						if(var_s >=I->var_ratio_th*max(var_s1, var_s2)) // unless user specified, var_ratio_th = LARGE_FLOAT and thus nullify this condition
						{
							sim_flag = 0;
							break;
						}
					}
					if (sim_flag == 1)
					{
						if (SEG[s1]->min_dist < min_min_dist) 
						{
							min_min_dist = SEG[k]->min_dist;
							min_min_dist_seg = s1;
						}
					}
				}
				else 
				{
					if (SEG[s1]->min_dist < min_min_dist) 
					{
						min_min_dist = SEG[s1]->min_dist;
						min_min_dist_seg = s1;
					}
				}
			}
		}
		if (min_min_dist > I->min_dist_th)	break;	//  if the dist between any two neighboring segments is larger than this threshold, terminate the iterative merging

		// step 2: merge two segments with min_min_dist
		s1 = min_min_dist_seg;
		s2 = SEG[s1]->min_dist_seg;
		merge_two_segments(I, SEG, s1, s2);
		num_of_segs -= 1;
	}

	// for an existing segment, if its size is less than a threshold, merge it with its nearst segment
	printf("num of segs before pruning: %d\n", num_of_segs);
	while(1)
	{
		int merging_flag = 0;
		for(i=0; i<n_lines*n_cols; i++)
		{
			if ( (SEG[i]->deserted == 0) && (SEG[i]->num_of_pixels<I->seg_size_th))
			{
				s1 = i;
				s2 = SEG[i]->min_dist_seg;
				merge_two_segments(I, SEG, s1, s2);
				num_of_segs -= 1;
				merging_flag = 1;
			}
		}
		if(merging_flag==0) break;
	}

	printf("num of segs after pruning: %d\n", num_of_segs);

	// create the map according to the segmentation results, 
	// i.e., the pixels in the kth segment are assigned the value k (k starts from 1).
	n = 1;
	for(i=0; i<n_lines*n_cols; i++)
	{
		if(SEG[i]->deserted==0)
		{
			for(j=0; j<SEG[i]->num_of_pixels; j++)
			{
				int yy = SEG[i]->pixel_y[j];
				int xx = SEG[i]->pixel_x[j];
				I->map[yy][xx] = n;	
			}
			n ++;
		}
	}

	for(i=0; i<max_num_of_segs; i++) 
	{
		if(SEG[i]->deserted==0) free_segment(SEG, i);
		free(SEG[i]);
	}
	free(SEG);

	return num_of_segs;
}