#pragma once

#include <stdint.h>

#pragma pack(push, 1)

struct BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

typedef struct BITMAPFILEHEADER BITMAPFILEHEADER;

struct BITMAPINFOHEADER {
    uint32_t  biSize;
    int32_t   biWidth;
    int32_t   biHeight;
    uint16_t   biPlanes;
    uint16_t   biBitCount;
    uint32_t  biCompression;
    uint32_t  biSizeImage;
    int32_t   biXPelsPerMeter;
    int32_t   biYPelsPerMeter;
    uint32_t  biClrUsed;
    uint32_t  biClrImportant;
};

typedef struct BITMAPINFOHEADER BITMAPINFOHEADER;

struct RGBTRIPLE {
    uint8_t rgbtBlue;
    uint8_t rgbtGreen;
    uint8_t rgbtRed;
};

typedef struct RGBTRIPLE RGBTRIPLE;
#pragma pack(pop)

uint32_t padding(uint32_t w);

size_t offset(uint32_t width, uint32_t x, uint32_t y);

uint8_t* load_bmp(char* input, BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header);

void save_bmp(char* output, BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header, uint8_t* data);

void crop_rotate(char *input, char* output, uint32_t x, uint32_t y, uint32_t w, uint32_t h);