#include <math.h>
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

float variance(float *d, int n);
float standard_dev(float *d, int n);

template <class T>
void swap(T s[], int n1, int n2)
{
	T t = s[n1];
	s[n1] = s[n2];
	s[n2] = t;
}

// from small (left) to large (right)
template <class T>
void quicksort(T v[], int left, int right)
{
	int i, last;

	if (left >= right) return;

	swap(v, left, (left+right)/2);

	last = left;
	for (i = left+1; i <= right; i++)
		if (v[i] < v[left])
			swap(v, ++last, i);

	swap(v, left, last);

	quicksort(v, left, last-1);
	quicksort(v, last+1, right);
}

template <class T>
void swap2(T s[][2], int n1, int n2)
{
	T t0 = s[n1][0];
	T t1 = s[n1][1];
	s[n1][0] = s[n2][0];
	s[n1][1] = s[n2][1];
	s[n2][0] = t0;
	s[n2][1] = t1;
}


template <class T>
void quicksort2(T v[][2], int left, int right)
{
	int i, last;

	if (left >= right) return;

	swap2(v, left, (left+right)/2);

	last = left;
	for (i = left+1; i <= right; i++)
		if (v[i] < v[left])
			swap2(v, ++last, i);

	swap2(v, left, last);

	quicksort2(v, left, last-1);
	quicksort2(v, last+1, right);
}


char ** dim2 (int height, int width, unsigned size);
void free2(char **array);
void reset2(char **array, int height, int width, unsigned size);
int round_m(double x);
int clip(int low, int high, int x);
int sign(int x);

template <class T>
T median3(T A[])  
{
	T d[3]; 
	for(int i=0; i<3; i++) d[i]=A[i];
	quicksort(d, 0, 2);
	return d[1];
}

template <class T>
T median5(T A[])  
{
	T d[5]; 
	for(int i=0; i<5; i++) d[i]=A[i];
	quicksort(d, 0, 4);
	return d[1];
}

#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#define max3(a, b, c) (max((a), max((b),(c))))
#define max4(a, b, c, d) max(max((a), (b)), max((c),(d)))

#endif