<?php

include __DIR__ . '/../src/OCR.php';

$ocr = FastFFI\OCR\OCR::new([
    'use_mkldnn' => (int) (PHP_OS !== 'Darwin'),
]);

var_dump($ocr->run(__DIR__ . '/img.png'));
