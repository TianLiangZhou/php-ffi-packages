#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
//
// Created by meshell on 2021/4/18.
//


typedef void * OCR;

typedef struct {
    char *str;
    float score;
} ResultItem;

typedef struct {
    char** text;
    char** score;
    int len;
    double exec_time;
} OCRResult;


OCR new_ocr(const char *config_str);
void free_ocr(OCR *ocr);
void free_result(OCRResult *result);
OCRResult * run(OCR *ocr, const char *image);
