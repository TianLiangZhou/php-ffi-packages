<?php
namespace FastFFI\OCR;

use FFI;
use RuntimeException;

class OCR
{
    /**
     * @var ?OCR
     */
    private static ?OCR $ocr = null;

    /**
     * @var FFI
     */
    protected FFI $ffi;

    /**
     * @var FFI\CData
     */
    private $paddleOcr;


    /**
     * @var array
     */
    private $options = [
        'use_gpu' => 0,
        'gpu_id' => 0,
        'gpu_mem' => 4000,
        'cpu_math_library_num_threads' => 10,
        'use_mkldnn' => 0,
        'max_side_len' => 960,
        'det_db_thresh' => 0.3,
        'det_db_box_thresh' => 0.5,
        'det_db_unclip_ratio' => 1.6,
        'use_polygon_score' => 1,
        'det_model_dir' => __DIR__ . '/../inference/ch_ppocr_server_v2.0_det_infer',
        'use_angle_cls' => 0,
        'cls_model_dir' => __DIR__ . '/../inference/ch_ppocr_mobile_v2.0_cls_infer',
        'cls_thresh' => 0.1,
        'rec_model_dir' => __DIR__ . '/../inference/ch_ppocr_server_v2.0_rec_infer',
        'char_list_file' => __DIR__ . '/../ppocr_keys_v1.txt',
        'visualize' => 0,
        'use_tensorrt' => 0,
        'use_fp16' => 0,
    ];

    /**
     * OpenCC constructor.
     * @param array $options [
     * 'use_gpu' => 0,
     * 'gpu_id' =>  0,
     * 'gpu_mem' =>  4000,
     * 'cpu_math_library_num_threads' =>  10,
     * 'use_mkldnn' =>  0,
     * 'max_side_len' =>  960,
     * 'det_db_thresh' =>  0.3,
     * 'det_db_box_thresh' =>  0.5,
     * 'det_db_unclip_ratio' =>  1.6,
     * 'use_polygon_score' => 1,
     * 'det_model_dir' =>  __DIR__ . '/../inference/ch_ppocr_server_v2.0_det_infer',
     * 'use_angle_cls' => 0,
     * 'cls_model_dir' =>  __DIR__ . '/../inference/ch_ppocr_mobile_v2.0_cls_infer',
     * 'cls_thresh' => 0.9,
     * 'rec_model_dir' =>  __DIR__ . '/../inference/ch_ppocr_server_v2.0_rec_infer',
     * 'char_list_file' => __DIR__ . '/../inference/ppocr_keys_v1.txt',
     * 'visualize' => 1,
     * 'use_tensorrt' => 0,
     * 'use_fp16' =>   0,
     * ]
     */
    private function __construct(array $options = [])
    {
        if (ini_get('ffi.enable') == false) {
            throw new RuntimeException("请设置php.ini中的ffi.enable参数");
        }
        $this->ffi = $this->makeFFI();
        foreach ($options as $name => $value) {
            if (isset($this->options[$name])) {
                $this->options[$name] = $value;
            }
        }
        $this->paddleOcr = $this->ffi->new_ocr($this->getConfig());
    }

    /**
     *
     */
    public function __destruct()
    {
        // TODO: Implement __destruct() method.
        if ($this->ffi) {
            $this->ffi->free_ocr($this->paddleOcr);
        }
    }

    /**
     * 简体转繁体
     *
     * @param string $image
     * @return array
     */
    public function run(string $image): array
    {
        if (!file_exists($image)) {
            throw new RuntimeException("The file not exist!");
        }
        $result = $this->ffi->run($this->paddleOcr, $image);
        if ($result == null) {
            throw new RuntimeException("Failed!");
        }
        $text = [];
        for ($i = 0; $i < $result->len; $i++) {
            $text[] = FFI::string($result->text[$i]);
        }
        $this->freeResult($result);
        return $text;
    }

    /**
     * @param $result
     */
    private function freeResult($result)
    {
        $this->ffi->free_result($result);
    }

    /**
     * @param array $options
     * @return static
     */
    public static function new(array $options = []): OCR
    {
        if (self::$ocr == null) {
            self::$ocr = new static($options);
        }
        return self::$ocr;
    }

    /**
     *
     */
    private function __clone()
    {

    }

    /**
     * @return FFI
     */
    private function makeFFI(): FFI
    {
        return FFI::cdef(
            file_get_contents(__DIR__ . '/../lib/libocrffi.h'),
            $this->defaultLibraryPath()
        );
    }

    /**
     * @return string
     */
    private function defaultLibraryPath(): string
    {
        if (PHP_INT_SIZE !== 8) {
            throw new RuntimeException('不支持32位系统，请自行编译lib文件');
        }
        $suffix = PHP_SHLIB_SUFFIX;
        if (PHP_OS == 'Darwin') {
            $suffix = 'dylib';
        }
        $filepath = __DIR__ . '/../lib/libocrffi.' . $suffix;
        if (file_exists($filepath)) {
            return realpath($filepath);
        }
        throw new RuntimeException('不支持的系统，请自行编译lib文件');
    }

    private function getConfig()
    {
        return <<<EOF
# model load config
use_gpu {$this->options['use_gpu']}
gpu_id  {$this->options['gpu_id']}
gpu_mem  {$this->options['gpu_mem']}
cpu_math_library_num_threads  {$this->options['cpu_math_library_num_threads']}
use_mkldnn {$this->options['use_mkldnn']}
# det config
max_side_len  {$this->options['max_side_len']}
det_db_thresh  {$this->options['det_db_thresh']}
det_db_box_thresh  {$this->options['det_db_box_thresh']}
det_db_unclip_ratio  {$this->options['det_db_unclip_ratio']}
use_polygon_score {$this->options['use_polygon_score']}
det_model_dir  {$this->options['det_model_dir']}
# cls config
use_angle_cls {$this->options['use_angle_cls']}
cls_model_dir  {$this->options['cls_model_dir']}
cls_thresh  {$this->options['cls_thresh']}
# rec config
rec_model_dir {$this->options['rec_model_dir']}  
char_list_file {$this->options['char_list_file']}  
# show the detection results
visualize {$this->options['visualize']}  
# use_tensorrt
use_tensorrt {$this->options['use_tensorrt']}  
use_fp16   {$this->options['use_fp16']}  
EOF;
    }
}
