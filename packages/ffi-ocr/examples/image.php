<?php

include __DIR__ . '/../src/OCR.php';

$ocr = FastFFI\OCR\OCR::new();

var_dump($ocr->run(__DIR__ . '/img.jpg'));
