#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "stego.h"
#include "bmp.h"

static uint8_t char_to_code(char x) {
    switch (x) {
        case ' ': return 26;
        case '.': return 27;
        case ',': return 28;
        default: return (uint8_t)x - (uint8_t)'A';
    }
}

static char code_to_char(uint8_t x) {
    switch (x) {
        case 26: return ' ';
        case 27: return '.';
        case 28: return ',';
        default: return (char)(x + (uint8_t)'A');
    }
}

static uint8_t click(uint8_t n, bool flag) {
    if (flag) {
        return n | 1;
    } else {
        return n & (~1);
    }
}

static void insert(BITMAPINFOHEADER* info_header, uint8_t* data, uint32_t x, uint32_t y, char rgb, bool bit) {
    assert((y < (uint32_t)info_header->biHeight) && (x < (uint32_t)info_header->biWidth));
    RGBTRIPLE* ptr = (RGBTRIPLE*)(data + offset(info_header->biWidth, x, y));
    switch (rgb) {
        case 'B': {
            ptr->rgbtBlue = click(ptr->rgbtBlue, bit);
            break;
        }
        case 'R': {
            ptr->rgbtRed = click(ptr->rgbtRed, bit);
            break;
        }
        case 'G': {
            ptr->rgbtGreen = click(ptr->rgbtGreen, bit);
            break;
        }
        default: {
            return;
        }
    }
}

void insert_info(char* input, char* output, char* key, char* message) {

    char* buffer = (char*)malloc(2048 * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr,"Error in memory allocation\n");
        exit(1);
    }
    FILE* message_text = fopen(message, "r");
    if (message_text == NULL) {
        fprintf(stderr,"Error in open message file\n");
        exit(1);
    }
    fscanf(message_text, "%[^\n]2047s", buffer);
    fclose(message_text);

    FILE* key_text = fopen(key, "r");
    if (key_text == NULL) {
        fprintf(stderr,"Error in open key file\n");
        exit(1);
    }

    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    uint8_t* data = load_bmp(input, &file_header, &info_header);

    char* it = buffer;
    uint32_t x, y;
    char rgb;
    while ((*it != '\0' ) && (*it != '\n')) {
        uint8_t code = char_to_code(*it);
        for (size_t i = 0; i < 5; i++) {
            fscanf(key_text, "%d %d %c", &x, &y, &rgb);
            insert(&info_header, data, x, info_header.biHeight - y - 1, rgb, code & 1);
            code /= 2;
        }
        it++;
    }
    fclose(key_text);
    free(buffer);

    save_bmp(output, &file_header, &info_header, data);
}

static bool get(BITMAPINFOHEADER* info_header, uint8_t* data, uint32_t x, uint32_t y, char rgb) {
    assert((y < (uint32_t)info_header->biHeight) && (x < (uint32_t)info_header->biWidth));
    RGBTRIPLE* ptr = (RGBTRIPLE*)(data + offset(info_header->biWidth, x, y));
    switch (rgb) {
        case 'B': {
            return (ptr->rgbtBlue & 1) ? (true) : (false);
        }
        case 'R': {
            return (ptr->rgbtRed  & 1) ? (true) : (false);
        }
        case 'G': {
            return (ptr->rgbtGreen & 1) ? (true) : (false);
        }
        default: {
            exit(1);
        }
    }
}

void get_info(char* input, char* key, char* message) {

    FILE* key_text = fopen(key, "r");
    if (key_text == NULL) {
        fprintf(stderr,"Error in open key file\n");
        assert(NULL);
    }

    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    uint8_t* data = load_bmp(input, &file_header, &info_header);

    FILE* message_text = fopen(message, "w");
    if (message_text == NULL) {
        fprintf(stderr,"Error in open message file\n");
        assert(NULL);
    }

    uint32_t x, y;
    char rgb;
    size_t counter = 0;
    uint32_t number = 0;
    while (fscanf(key_text, "%d %d %c", &x, &y, &rgb) > 0) {
        bool bit = get(&info_header, data, x, info_header.biHeight - y - 1, rgb);
        number += ((bit) ? (1) : 0) * (powl(2, counter % 5));
        counter++;
        if (counter % 5 == 0) {
            fprintf(message_text, "%c", code_to_char(number));
            number = 0;
        }
    }
    free(data);
    fprintf(message_text, "%s", "\n");
    fclose(key_text);
    fclose(message_text);
}
