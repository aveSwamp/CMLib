#include "P3PPM_IO.h"
#define TOSTR()
#define DEBUG(n) printf("")

int extract_P3PPM(_rawimg *img_struct, const char *filename){
    FILE *ppm_file = fopen(filename, "rb");
    long width = (long)0L;//in pixels
    long height = (long)0L;
    long max_value = (long)0L;
    long flags = (long)0L;//reading flags
    //0: if 1 stop reading
    //1: if 1 signature read
    //2: if 1 width was read
    //3: if 1 height was read
    //4: if 1 max color value was read

    byte *signature = (byte *)malloc(sizeof(byte) * 3);
    if((void *)signature == NULL){
        return __P3PPM_IO_MALLOCERROR;
    }
    fread((void *)signature, sizeof(byte), 2, ppm_file);//reading signature
    signature[2] = '\0';
    //printf("PPM signature: %s\n", signature);
    flags = flags | (1 << 1);
    free(signature);
    byte *read_unit = (byte *)malloc(sizeof(byte) * 1);
    while((flags & (long)(1 << 0)) != (long)(1 << 0)){
        fread((void *)read_unit, sizeof(byte), 1, ppm_file);
        if(((flags & (1 << 4)) != (1 << 4)) && ((flags & (1 << 3)) == (1 << 3)) && ((flags & (1 << 2)) == (1 << 2))){//max_value
            if((((*read_unit) == (byte)10) || ((*read_unit) == (byte)13) || ((*read_unit) == (byte)32)) && (max_value != 0)){//'\n'=10, '\r'=13, [SPACE](' ')=32
                flags = flags | (1 << 4);
                flags = flags | (1 << 0);
            }
            if(((*read_unit) >= (byte)48) && ((*read_unit) <= (byte)57)){
                max_value = (max_value * 10) + ((*read_unit) - (byte)48);
            }
        } else if(((flags & (1 << 3)) != (1 << 3)) && ((flags & (1 << 2)) == (1 << 2))){//height
            if((((*read_unit) == (byte)10) || ((*read_unit) == (byte)13) || ((*read_unit) == (byte)32)) && (height != 0)){//'\n'=10, '\r'=13, [SPACE](' ')=32
                flags = flags | (1 << 3);
            }
            if(((*read_unit) >= (byte)48) && ((*read_unit) <= (byte)57)){
                height = (height * 10) + ((*read_unit) - (byte)48);
            }
        } else if((flags & (1 << 2)) != (1 << 2)){//width
            if((((*read_unit) == (byte)10) || ((*read_unit) == (byte)13) || ((*read_unit) == (byte)32)) && (width != 0)){//'\n'=10, '\r'=13, [SPACE](' ')=32
                flags = flags | (1 << 2);
            }
            if(((*read_unit) >= (byte)48) && ((*read_unit) <= (byte)57)){
                width = (width * 10) + ((*read_unit) - (byte)48);
            }
        }
    }
    //printf("IMAGE size: %ld x %ld\n", width, height);
    //printf("MAX_VALUE: %ld\n", max_value);
    (*img_struct).width = width;
    (*img_struct).height = height;
    (*img_struct).max_value = max_value;
    long length = width * height;
    (*img_struct).image_data = (_prgb *)malloc(sizeof(_prgb) * length);
    for(long i = 0; i < length; i++){
        byte red_channel = 0;
        byte green_channel = 0;
        byte blue_channel = 0;
        flags = 0L;
        //0: if eq 1 stop reading
        //1: if eq 1 start reading for red
        //2: if eq 1 stop reading for red
        //3: if eq 1 start reading for green
        //4: if eq 1 stop reading for green
        //5: if eq 1 start reading for blue
        //6: if eq 1 stop reading for blue
        while((flags & (long)(1 << 0)) != (long)(1 << 0)){
            fread((void *)read_unit, sizeof(byte), 1, ppm_file);
            if(feof(ppm_file) != 0){
                flags = flags | (1 << 0);
            }else if(((flags & (long)(1 << 6)) != (long)(1 << 6)) && ((flags & (long)(1 << 4)) == (long)(1 << 4)) && ((flags & (long)(1 << 2)) == (long)(1 << 2))){
                if(((*read_unit) >= (byte)48) && ((*read_unit) <= (byte)57)){
                    if((flags & (long)(1 << 5)) != (long)(1 << 5)){
                        flags = flags | (1 << 5);
                    }
                    blue_channel = (blue_channel * 10) + ((*read_unit) - (byte)48);
                    //printf("b");
                }else if(((flags & (long)(1 << 5)) == (long)(1 << 5))){
                    flags = flags | (long)(1 << 6);
                    flags = flags | (long)(1 << 0);
                }
            }else if(((flags & (long)(1 << 4)) != (long)(1 << 4)) && ((flags & (long)(1 << 2)) == (long)(1 << 2))){
                if(((*read_unit) >= (byte)48) && ((*read_unit) <= (byte)57)){
                    if((flags & (long)(1 << 3)) != (long)(1 << 3)){
                        flags = flags | (1 << 3);
                    }
                    green_channel = (green_channel * 10) + ((*read_unit) - (byte)48);
                    //printf("g");
                }else if(((flags & (long)(1 << 3)) == (long)(1 << 3))){
                    flags = flags | (long)(1 << 4);
                }
            }else if((flags & (long)(1 << 2)) != (long)(1 << 2)){
                if(((*read_unit) >= (byte)48) && ((*read_unit) <= (byte)57)){
                    if((flags & (long)(1 << 1)) != (long)(1 << 1)){
                        flags = flags | (1 << 1);
                    }
                    red_channel = (red_channel * 10) + ((*read_unit) - (byte)48);
                    //printf("r");
                }else if(((flags & (long)(1 << 1)) == (long)(1 << 1))){
                    flags = flags | (long)(1 << 2);
                }
            }
        }//rgb parsing end
        //printf("%d %d %d\n", red_channel, green_channel, blue_channel);
        (*img_struct).image_data[i].named_data.r = (red_channel & 0xff);
        (*img_struct).image_data[i].named_data.g = (green_channel & 0xff);
        (*img_struct).image_data[i].named_data.b = (blue_channel & 0xff);
    }
    free(read_unit);
    fclose(ppm_file);
    return __P3PPM_IO_NOERRORS;
}

char *FormSize(long width, long height){
    char *output_buffer;
    long output_length = 0;
    long width_length = 0;
    long height_length = 0;
    long copy_width = width;
    long copy_height = height;
    while(copy_width != 0){
        copy_width = copy_width / 10;
        ++width_length;
    }
    while(copy_height != 0){
        copy_height = copy_height / 10;
        ++height_length;
    }
    output_length = width_length + 1 + height_length + 3;
    output_buffer = (char *)malloc(sizeof(char) * output_length);
    for(long i = 0; i < width_length; i++){
        output_buffer[i] = width / (10 * (width_length - 1 - i));
    }
    for(long i = width_length + 2; i < height_length; i++){
        output_buffer[i] = height / (10 * (height_length - 1 - i));
    }
    output_buffer[output_length-3] = '\r';
    output_buffer[output_length-2] = '\n';
    output_buffer[output_length-1] = '\0';
    return output_buffer;

}

//char *FormMaxValue(long max_value){
//
//}

int pack_P3PPM(_rawimg *img_struct, const char *filename){// CR LF in windows
    FILE *ppm_file = fopen(filename, "wb");
    
    return __P3PPM_IO_NOERRORS;
}
