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

/**
 *
 * @param mode
 * @return
 */
extern OCR new_ocr(const char *config_dir);

/**
 *
 * @param ocr
 * @param image
 * @return
 */
extern void run(OCR ocr, const char *image);

/**
 *
 * @param ocr
 */
extern void free_ocr(OCR ocr);


#ifdef __cplusplus
}
#endif
#endif //FFI_OCR_LIB_H
