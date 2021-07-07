//
// Created by meshell on 2021/7/6.
//

#include "glog/logging.h"

#ifndef __APPLE__
#include "omp.h"
#endif

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <vector>

#include <cstring>
#include <fstream>
#include <numeric>

#include "config.h"
#include "ocr_det.h"
#include "ocr_rec.h"

#include "lib.h"

using namespace std;
using namespace cv;
using namespace PaddleOCR;

extern "C" OCR new_ocr(const char *configStr) {
    OCRConfig *config;
    DBDetector *dbDetector;
    CRNNRecognizer *recognizer;
    Classifier *cls = nullptr;
    config = new OCRConfig(configStr);
    dbDetector = new DBDetector(
        config->det_model_dir, config->use_gpu, config->gpu_id,
        config->gpu_mem, config->cpu_math_library_num_threads,
        config->use_mkldnn, config->max_side_len, config->det_db_thresh,
        config->det_db_box_thresh, config->det_db_unclip_ratio,
        config->use_polygon_score, config->visualize,
        config->use_tensorrt, config->use_fp16
    );
    recognizer = new CRNNRecognizer(
        config->rec_model_dir, config->use_gpu, config->gpu_id,
        config->gpu_mem, config->cpu_math_library_num_threads,
        config->use_mkldnn, config->char_list_file,
        config->use_tensorrt, config->use_fp16
    );
    if (config->use_angle_cls) {
        cls = new Classifier(
            config->cls_model_dir, config->use_gpu, config->gpu_id,
            config->gpu_mem, config->cpu_math_library_num_threads,
            config->use_mkldnn, config->cls_thresh,
            config->use_tensorrt, config->use_fp16
        );
    }
    return (PaddleOcr*) new PaddleOcr{config, dbDetector, recognizer, cls};
}

extern "C" void free_ocr(OCR *ocr) {
    auto paddleOcr = (PaddleOcr *) ocr;
    delete paddleOcr->recognizer;
    delete paddleOcr->config;
    delete paddleOcr->dbDetector;
    delete paddleOcr;
}

extern "C" void free_result(OCRResult *result) {
    delete result->text;
    delete result->score;
    delete result;
}

extern "C" OCRResult * run(OCR *ocr, const char *image) {
    auto paddleOcr = (PaddleOcr *) ocr;
    cv::Mat src_img = cv::imread(image, cv::IMREAD_COLOR);
    if (!src_img.data) {
        std::cerr << "[ERROR] image read failed! image path: " << image << "\n";
        return nullptr;
    }
    auto start = std::chrono::system_clock::now();
    std::vector<std::vector<std::vector<int>>> boxes;
    paddleOcr->dbDetector->Run(src_img, boxes);
    auto results = paddleOcr->recognizer->Run(boxes, src_img, paddleOcr->classifier);
    if (results.size() < 2) {
        return nullptr;
    }
    auto len = int(results.size() / 2);
    char** ocr_str = new char*[len];
    char** ocr_score = new char*[len];
    int i = 1;
    int score_index = 0;
    int str_index = 0;
    for (const auto &item : results) {
        if (i % 2 == 0) {
            ocr_score[score_index] = strdup(const_cast<char*>(item.c_str()));
            std::cout << item.c_str() << std::endl;
            score_index++;
        } else {
            ocr_str[str_index] = strdup(const_cast<char*>(item.c_str()));
            std::cout << item.c_str();
            str_index++;
        }
        i++;
    }
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    auto exec_time = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    return new OCRResult{
        ocr_str,
        ocr_score,
        len,
        exec_time
    };
}
