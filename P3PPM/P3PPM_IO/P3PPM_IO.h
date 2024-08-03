#ifndef __P3PPM_IO
#define __P3PPM_IO
#include <malloc.h>
#include <stdio.h>
#include <string.h>



/*error wrapper*/
/*@error*/
#define __P3PPM_IO_NOERRORS 0
#define __P3PPM_IO_MALLOCERROR 1
/*error wrapper*/
/*controll masks*/
/*@ctrl_mask*/
#define READING     (long)0b00000001
#define SIGNATURE   (long)0b00000010
#define WIDTH       (long)0b00000100
#define HEIGHT      (long)0b00001000
#define MAX_COLOR   (long)0b00010000
/*controll masks*/



/*color space wrapper*/
/*@rgb*/
typedef unsigned char byte;

typedef struct _pixel_rgb_named{
    byte r;
    byte g;
    byte b;
}_prgb_named;

typedef struct _pixel_rgb_unnamed{
    long component[3];
}_prgb_unnamed;

typedef union _pixel_rgb{
    _prgb_named named_data;
    _prgb_unnamed unnamed_data;
}_prgb;
/*color space wrapper*/
/*image wrapper*/
/*@img*/
typedef struct _RAW_DATA_rgb{
    _prgb *image_data;
    long width;
    long height;
    long max_value;
}_rawimg;
/*image wrapper*/

int extract_P3PPM(_rawimg *img_struct, const char *filename);
int pack_P3PPM(_rawimg *img_struct, const char *filename);

#endif