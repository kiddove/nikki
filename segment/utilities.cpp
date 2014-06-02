#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include "utilities.h"

char ** dim2 (int height, int width, unsigned size)
{
  int i;
  char **prow;
  char *pdata; 
  
  pdata = (char *) calloc (height * width, size);
  if (pdata == (char *) NULL)
  {
    fprintf (stderr, "No heap space for data\n");
    exit (1);
  }
  prow = (char **) malloc (height * sizeof (char *));
  if (prow == (char **) NULL)
  {
    fprintf (stderr, "No heap space for row pointers\n");
    exit (1);
  }
  for (i = 0; i < height; i++)
  {
    prow[i] = pdata;		     
    pdata += size * width;	 
  }
  return prow;			         
}

void free2(char **array)
{
  if (array == (char **) NULL) return;
  free(*array);
  free(array);
}

void reset2(char **array, int height, int width, unsigned size)
{
	for(int i=0; i<height; i++)
		memset( array[i], 0, width*size);
}

int round_m(double x)
{
  if(x>0) return ((int) (x+0.5));
  else    return ((int) (x-0.5));
}

int clip(int low, int high, int x)
{
  if(x<low) x=low;
  else if(x>high) x=high;
  return x;
}

int sign(int x)
{
	if		(x>0)	return 1;
	else if	(x<0)	return -1;
	else			return 0;
}


float variance(float *d, int n)
{
	float m = 0.0, v=0.0;
	for(int i=0; i<n; i++) m += d[i];
	m = m/n;
	for(int j=0; j<n; j++) v += (d[j]-m)*(d[j]-m);
	v = v/n;
	return v;
}

float standard_dev(float *d, int n)
{
	float m = 0.0, v=0.0;
	for(int i=0; i<n; i++) m += d[i];
	m = m/n;
	for(int j=0; j<n; j++) v += (d[j]-m)*(d[j]-m);
	v = v/n;
	v = sqrt(v);
	return v;
}