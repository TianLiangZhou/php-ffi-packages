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

extern "C" OCR new_ocr(const char *config_file) {
    ifstream file(config_file);
    if (!file.good()) {
        return nullptr;
    }
    OCRConfig *config;
    DBDetector *dbDetector;
    CRNNRecognizer *recognizer;
    Classifier *cls = nullptr;
    config = new OCRConfig(config_file);
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

extern "C" void free_ocr(OCR ocr) {
    auto paddleOcr = (PaddleOcr *) ocr;
    delete paddleOcr->recognizer;
    delete paddleOcr->config;
    delete paddleOcr->dbDetector;
    delete paddleOcr;
}

extern "C" void run(OCR ocr, const char *image) {
    auto paddleOcr = (PaddleOcr *) ocr;
    cv::Mat src_img = cv::imread(image, cv::IMREAD_COLOR);
    if (!src_img.data) {
        std::cerr << "[ERROR] image read failed! image path: " << image << "\n";
        return ;
    }
    std::vector<std::vector<std::vector<int>>> boxes;
    paddleOcr->dbDetector->Run(src_img, boxes);
    paddleOcr->recognizer->Run(boxes, src_img, paddleOcr->classifier);
}
