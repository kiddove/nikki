#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "utilities.h"
#include "image.h"

void allocate_image(image *I)
{
	I->IMG = (imgpel ***) malloc (I->num_of_bands*sizeof(imgpel**));
	if(I->IMG==NULL)
	{
		printf("failed to allocate memory: allocate_image: I\n");
		exit(0);
	}
	for(int i=0; i<I->num_of_bands; i++)
	{
		I->IMG[i] = (imgpel **) dim2 (I->num_of_lines,  I->num_of_samples,   sizeof(imgpel));
	}
	I->map = (uint16 **) dim2 (I->num_of_lines,   I->num_of_samples,   sizeof(uint16));
}

void free_image(image *I)
{
	for(int b=0; b<I->num_of_bands; b++)
	{
		if(I->IMG[b] != NULL)
			free2((char **) I->IMG[b]);
	}
	free2((char **) I->map);
}

void read_image(image *I)
{
	FILE *fp = fopen(I->img_file, "rb");
	if(fp == NULL)
	{
		printf("failed to open image for read: %s\n", I->img_file);
		exit(0);
	}

	for(int i=0; i<I->num_of_bands; i++)
	{
		for(int j=0; j<I->num_of_lines; j++) 
		{
			for(int k=0; k<I->num_of_samples; k++)
			{
				if		(I->data_type==1)  {uint8 d;   fread(&d, sizeof(uint8),   1, fp); I->IMG[i][j][k] = d;}
				else if (I->data_type==2)  {int16 d;   fread(&d, sizeof(int16),   1, fp); I->IMG[i][j][k] = d;}
				else if (I->data_type==4)  {float32 d; fread(&d, sizeof(float32), 1, fp); I->IMG[i][j][k] = d;}
				else if (I->data_type==12) {uint16 d;  fread(&d, sizeof(uint16),  1, fp); I->IMG[i][j][k] = d;}
				else 
				{
					printf("data type error. \n");
					exit(0);
				}
			}
		}
	}

	fclose(fp);
}

void write_map(image *I)
{	
	FILE *fp = fopen(I->map_file, "wb");
	if(fp == NULL)
	{
		printf("failed to open map file for write: %s.\n", I->map_file);
		return;
	}
	for(int j=0; j<I->num_of_lines; j++) 
		fwrite(I->map[j], sizeof(uint16), I->num_of_samples, fp);

	fclose(fp);
}


