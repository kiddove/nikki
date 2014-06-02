#ifndef _IMAGE_H_
#define _IMAGE_H_

#define imgpel   float
#define uint8    unsigned char
#define uint16   unsigned short int
#define int16    short int
#define int32    int
#define uint32   unsigned int
#define float32  float
#define double64 double

#define FILE_NAME_SIZE 100

typedef struct {
    char *TokenName;
    void *Place;
    int Type;         //! 0: int, 1: char, 2: double
    double Default;
    int param_limits; //! 0: no limits, 1: both min and max, 2: only min (i.e. no negatives)
    double min_limit;
    double max_limit;
} Mapping;


typedef struct
{
	char img_file[FILE_NAME_SIZE];
	char hdr_file[FILE_NAME_SIZE];
	char map_file[FILE_NAME_SIZE];
	int num_of_bands;
	int num_of_lines;
	int num_of_samples;
	int data_type;
	int header_offset;

	int min_dist_th;
	int seg_size_th;
	float var_ratio_th;
	
	imgpel ***IMG;
	uint16 **map;

} image;

void	allocate_image(image *img);
void	free_image(image *img);
void	read_image(image *img);
void	write_map(image *img);

#endif
