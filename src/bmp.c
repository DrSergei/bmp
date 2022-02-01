#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "bmp.h"

uint32_t padding(uint32_t w) {
    const size_t ALIGNMENT = 4;
    return (sizeof(uint32_t) - ((sizeof(RGBTRIPLE) * w) % ALIGNMENT)) % ALIGNMENT;
}

size_t offset(uint32_t width, uint32_t x, uint32_t y) {
    return y * (width * sizeof(RGBTRIPLE) + padding(width)) + x * sizeof(RGBTRIPLE);
}

uint8_t* load_bmp(char* input, BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header) {
    FILE* input_file = fopen(input, "rb");
    if (input_file == NULL) {
        fprintf(stderr,"Error in open input file\n");
        exit(1);
    }

    fread(file_header, sizeof(BITMAPFILEHEADER), 1, input_file);
    fread(info_header, sizeof(BITMAPINFOHEADER), 1, input_file);

    if ((file_header->bfOffBits != 54) || // https://coderoad.ru/25713117/%D0%B2-%D1%87%D0%B5%D0%BC-%D1%80%D0%B0%D0%B7%D0%BD%D0%B8%D1%86%D0%B0-%D0%BC%D0%B5%D0%B6%D0%B4%D1%83-bisizeimage-bisize-%D0%B8-bfsize
        (info_header->biSize != 40) || // проверка размера
        (info_header->biBitCount != 24) || // число бит на цвет
        (info_header->biCompression != 0)) // нет сжатия https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-wmf/4e588f70-bd92-4a6f-b77f-35d0feaf7a57
    {
        fclose(input_file);
        fprintf(stderr, "Error file\n");
        exit(1);
    }

    uint8_t* res = (uint8_t*)malloc(file_header->bfSize);
    if (res == NULL) {
        fprintf(stderr, "Error in memory allocation\n");
        assert(NULL);
    }
    fread(res, info_header->biSizeImage, 1, input_file);
    fclose(input_file);
    return res;
}

static uint8_t* crop(BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header, uint8_t* data, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    if ((x + w > (uint32_t)(info_header->biWidth)) || (y + h > (uint32_t)(info_header->biHeight))) {
        fprintf(stderr, "Error in crop size\n");
        free(data);
        exit(1);
    }

    uint32_t size = (w * sizeof(RGBTRIPLE) + padding(w)) * h;

    uint8_t* res = (uint8_t*)calloc(size, 1);
    if (res == NULL) {
        fprintf(stderr, "Error in memory allocation\n");
        free(data);
        exit(1);
    }

    for (size_t oy = 0; oy < h; oy++) {
        for (size_t ox = 0; ox < w; ox++) {
            RGBTRIPLE* newptr = (RGBTRIPLE*)(res + offset(w, ox, oy));
            RGBTRIPLE* oldptr = (RGBTRIPLE*)(data + offset(info_header->biWidth, ox + x, y + oy));
            *newptr = *oldptr;
        }
    }

    file_header->bfSize = size + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    info_header->biSizeImage = size;
    info_header->biWidth = w;
    info_header->biHeight = h;
    free(data);
    return res;
}

static uint8_t* rotate(BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header, uint8_t* data) {

    uint32_t size = (info_header->biHeight * sizeof(RGBTRIPLE) + padding(info_header->biHeight)) * info_header->biWidth;
    uint8_t* res = (uint8_t*)calloc(size, 1);
    if (res == NULL) {
        fprintf(stderr, "Error in memory allocation\n");
        free(data);
        exit(1);
    }

    for (size_t oy = 0; oy < (size_t)info_header->biWidth; oy++) {
        for (size_t ox = 0; ox < (size_t)info_header->biHeight; ox++) {
            RGBTRIPLE* newptr = (RGBTRIPLE*)(res + offset(info_header->biHeight, ox, (info_header->biWidth - oy - 1)));
            RGBTRIPLE* oldptr = (RGBTRIPLE*)(data + offset(info_header->biWidth, oy, ox));
            *newptr = *oldptr;
        }
    }

    int32_t tmp = info_header->biHeight;
    info_header->biHeight = info_header->biWidth;
    info_header->biWidth = tmp;
    file_header->bfSize = size + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    info_header->biSizeImage = size;
    free(data);
    return res;
}

void save_bmp(char* output, BITMAPFILEHEADER* file_header, BITMAPINFOHEADER* info_header, uint8_t* data) {
    FILE* output_file = fopen(output, "wb");
    if (output_file == NULL) {
        fprintf(stderr,"Error in open output file\n");
        free(data);
        exit(1);
    }

    fwrite(file_header, sizeof(BITMAPFILEHEADER), 1, output_file);
    fwrite(info_header, sizeof(BITMAPINFOHEADER), 1, output_file);
    fwrite(data, info_header->biSizeImage, 1, output_file);
    fclose(output_file);
    free(data);
}

void crop_rotate(char *input, char* output, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    uint8_t* buf1 = load_bmp(input, &file_header, &info_header);
    uint8_t* buf2 = crop(&file_header, &info_header, buf1, x, info_header.biHeight - y - h, w, h);
    uint8_t* buf3 = rotate(&file_header, &info_header, buf2);
    save_bmp(output, &file_header, &info_header, buf3);
}
