#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "segment.h"
#include "image.h"

#define MAX_ITEMS_TO_PARSE  200

char *GetConfigFileContent (char *Filename);
void ParseContent (char *buf, int bufsize, image *I);
int ParameterNameToMapIndex (char *s);

Mapping Map[] = 
{
    //{"ENVI",                        },
    //{"description",                 },
    {"samples"					      },
    {"lines"					      },
    {"bands"                          },
    //{"header offset"                },
    //{"file type"                    },
    {"data type"                      },
    //{"interleave"                   },
    //{"sensor type"                  },
    //{"byte order"                   },
    //{"x start"                      },
    //{"y start"                      },
    //{"map info",                    },
	//{"wavelength units              },
    //{"pixel size"                   },
    //{"band names"                   },
    {NULL                             }
};


void RMHelpExit (char *comm)
{
    printf("\n================== segmentation =============================================================================================================\n\n");
    printf("usage: %s\n\n", comm);
    printf("		input_image    : input image file name (its header file must be in the same directory and named .hdr\n");
	printf("		output_map     : output map file name \n");
    printf("		min_dist_th    : segment distance threshold (the merging process ends when the minimum seg distance is greater than this threshold)\n");
    printf("		seg_size_th    : segment size threshold (segments smaller than this threshold are merged with its nearest neightboring segment)\n");
	printf("		[var_ratio_th] : (optional) if both segments are large than seg_size_th,	require merged_var < var_ratio_th * max(var1, var2) for each band\n");
    printf("\n\n");
    printf("Developed by:  Yunwei Jia   October 20, 2007\n");
    printf("==============================================================================================================================================\n\n");

    exit(-1);
}

void Configure (int argc, char *argv[], image *I)
{
    char *content;
    if ((argc<5) || (argc>6))
    {
        RMHelpExit(argv[0]);
    }

	strcpy(I->img_file, argv[1]);
	strcpy(I->hdr_file, argv[1]); 
	strcpy(I->hdr_file+strlen(argv[1])-3, "hdr"); 
	strcpy(I->map_file, argv[2]); 
    content = GetConfigFileContent (I->hdr_file);
	int len = strlen(content);
    ParseContent (content, len, I);
    free (content);
	I->min_dist_th = atoi(argv[3]);
	I->seg_size_th = atoi(argv[4]);
	if(argc==6) I->var_ratio_th = (float) atof(argv[5]);
	else  I->var_ratio_th = (float) LARGE_FLOAT;
}

char *GetConfigFileContent (char *Filename)
{
    long FileSize;
    FILE *f;
    char *buf;

    if (NULL == (f = fopen (Filename, "r")))
    {
        printf("Cannot open configuration file %s.\n", Filename);
        return NULL;
    }

    if (0 != fseek (f, 0, SEEK_END))
    {
        printf("Cannot fseek in configuration file %s.\n", Filename);
        return NULL;
    }

    FileSize = ftell (f);
    if (FileSize < 0 || FileSize > 60000)
    {
        printf("Unreasonable Filesize %ld reported by ftell for configuration file %s\n.", FileSize, Filename);
        return NULL;
    }
    if (0 != fseek (f, 0, SEEK_SET))
    {
        printf("Cannot fseek in configuration file %s.", Filename);
        return NULL;
    }

    if ((buf = (char *)malloc (FileSize + 1))==NULL) 
	{
		printf("failed to allocate memory in: GetConfigFileContent: buf\n");
		return NULL;
	}

    // Note that ftell() gives us the file size as the file system sees it.  The actual file size,
    // as reported by fread() below will be often smaller due to CR/LF to CR conversion and/or
    // control characters after the dos EOF marker in the file.

    FileSize = fread (buf, 1, FileSize, f);
    buf[FileSize] = '\0';

    fclose (f);
    return buf;
}


/*!
***********************************************************************
* \brief
*    Parses the character array buf and writes global variable input, which is defined in
*    configfile.h.  This hack will continue to be necessary to facilitate the addition of
*    new parameters through the Map[] mechanism (Need compiler-generated addresses in map[]).
* \param buf
*    buffer to be parsed
* \param bufsize
*    buffer size of buffer
***********************************************************************
*/
void ParseContent (char *buf, int bufsize, image *I)
{

    char *items[MAX_ITEMS_TO_PARSE];
    int item = 0;
    int InString = 0, InItem = 0;
    char *p = buf;
    char *bufend = &buf[bufsize];
    int IntContent;
    int i;

    // Stage one: Generate an argc/argv-type list in items[], without comments and whitespace.
    // This is context insensitive and could be done most easily with lex(1).

    while (p < bufend)
    {
        switch (*p)
        {
        case 13:
            p++;
            break;
        case '#':                 // Found comment
            *p = '\0';              // Replace '#' with '\0' in case of comment immediately following integer or string
            while (*p != '\n' && p < bufend)  // Skip till EOL or EOF, whichever comes first
                p++;
            InString = 0;
            InItem = 0;
            break;
        case '\n':
            InItem = 0;
            InString = 0;
            *p++='\0';
            break;
        case ' ':
        case '\t':              // Skip whitespace, leave state unchanged
            if (InString)
                p++;
            else
            {                     // Terminate non-strings once whitespace is found
                *p++ = '\0';
                InItem = 0;
            }
            break;

        case '"':               // Begin/End of String
            *p++ = '\0';
            if (!InString)
            {
                items[item++] = p;
                InItem = ~InItem;
            }
            else
                InItem = 0;
            InString = ~InString; // Toggle
            break;

        default:
            if (!InItem)
            {
                items[item++] = p;
                InItem = ~InItem;
            }
            p++;
        }
    }

    item--;

	for (i=0; i<item; i++)
	{
		if(_strcmpi("samples", items[i])==0 && _strcmpi("=", items[i+1])==0)
		{
			sscanf (items[i+2], "%d", &IntContent);
			I->num_of_samples = IntContent;
		}
		else if(_strcmpi("lines", items[i])==0 && _strcmpi("=", items[i+1])==0)
		{
			sscanf (items[i+2], "%d", &IntContent);
			I->num_of_lines = IntContent;
		}
		else if(_strcmpi("bands", items[i])==0 && _strcmpi("=", items[i+1])==0)
		{
			sscanf (items[i+2], "%d", &IntContent);
			I->num_of_bands = IntContent;
		}
		else if(_strcmpi("data", items[i])==0 && _strcmpi("type", items[i+1])==0 && _strcmpi("=", items[i+2])==0)
		{
			sscanf (items[i+3], "%d", &IntContent);
			I->data_type = IntContent;
		}
	}
}

/*!
***********************************************************************
* \brief
*    Returns the index number from Map[] for a given parameter name.
* \param s
*    parameter name string
* \return
*    the index number if the string is a valid parameter name,         \n
*    -1 for error
***********************************************************************
*/
static int ParameterNameToMapIndex (char *s)
{
    int i = 0;

    while (Map[i].TokenName != NULL)
        if (0==_strcmpi (Map[i].TokenName, s))
            return i;
        else
            i++;
    return -1;
}