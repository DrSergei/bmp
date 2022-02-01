#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "bmp.h"
#include "stego.h"

int main(int argc, char** argv) {
    if ((strcmp("crop-rotate", argv[1]) == 0) && (argc == 8)) {
        crop_rotate(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
        return 0;
    }
    if ((strcmp("insert", argv[1]) == 0) && (argc == 6)) {
        insert_info(argv[2], argv[3], argv[4], argv[5]);
        return 0;
    }
    if ((strcmp("extract", argv[1]) == 0) && (argc == 5)) {
        get_info(argv[2], argv[3], argv[4]);
        return 0;
    }
    fprintf(stderr, "Invalid arguments\n");
    assert(NULL);
}