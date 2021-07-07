//
// Created by meshell on 2021/7/6.
//

#ifndef FFI_OCR_LIB_H
#define FFI_OCR_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif
typedef void * OCR;
typedef struct {
    PaddleOCR::OCRConfig *config;
    PaddleOCR::DBDetector *dbDetector;
    PaddleOCR::CRNNRecognizer *recognizer;
    PaddleOCR::Classifier *classifier;
} PaddleOcr;

typedef struct {
    char** text;
    char** score;
    int len;
    double exec_time;
} OCRResult;



/**
 *
 * @param config_str
 * @return
 */
extern OCR new_ocr(const char *config_str);

/**
 *
 * @param ocr
 * @param image
 * @return
 */
extern OCRResult * run(OCR *ocr, const char *image);

/**
 *
 * @param ocr
 */
extern void free_ocr(OCR *ocr);


/**
 *
 * @param result
 */
extern void free_result(OCRResult *result);

#ifdef __cplusplus
}
#endif
#endif //FFI_OCR_LIB_H
