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
     * @var string
     */
    protected string $configDir = __DIR__ . '/../config.txt';

    /**
     * @var FFI\CData
     */
    private $paddleOcr;

    /**
     * OpenCC constructor.
     * @param string|null $dictDir
     */
    private function __construct(string $dictDir = null)
    {
        if (ini_get('ffi.enable') == false) {
            throw new RuntimeException("请设置php.ini中的ffi.enable参数");
        }
        $this->ffi = $this->makeFFI();
        if ($configDir && file_exists($configDir)) {
            $this->configDir = $configDir;
        }
        $this->paddleOcr = $this->ffi->new_ocr($this->configDir);
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
     * @return array ['words' => '', 'tags' => '', 'weight' = > '']
     */
    public function run(string $image): void
    {
        $this->ffi->run($this->paddleOcr, $image);
    }

    /**
     * @param string|null $configFile
     * @return static
     */
    public static function new(string $configFile = null): OCR
    {
        if (self::$ocr == null) {
            self::$ocr = new static($configFile);
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
}
