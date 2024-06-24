## ffi-ocr

`ffi-ocr`是一个`PHP FFI`图片取词库，基于百度开源的[paddleOCR](https://github.com/PaddlePaddle/PaddleOCR) 项目，使用`C++`导出`C`函数构建动态链接库来给`php`调用。
项目中大部分源码都可以从 `https://github.com/PaddlePaddle/PaddleOCR/tree/release/2.1/deploy/cpp_infer` 找到。

### 环境

需要`php >= 7.4` 以上的版本并且开启了`FFI`扩展。

还需要设置`php.ini` 中的`ffi.enable`为`On`。

### paddlepaddle

推理预测库版本: [2.5](https://www.paddlepaddle.org.cn/inference/v2.5/guides/introduction/index_intro.html)

项目依赖: __paddle推理预测库__([点击去下载](https://www.paddlepaddle.org.cn/inference/v2.5/guides/install/download_lib.html) )，__预测模型库__([点击去下载](https://github.com/baidu/lac/releases/tag/v2.1.0) )。

### Linux

将下载的推理预测库存放到: `/opt/paddle_inference`目录。

可以通过`ldd lib/libocrffi.so`显示库的依赖, 再根据情况建立软链。

```shell 
[meshell@/] cd /lib64
[meshell@/lib64] ln -s paddle_inference/paddle/lib/libpaddle_inference.so libpaddle_inference.so
[meshell@/lib64] ln -s paddle_inference/third_party/install/mklml/lib/libmklml_intel.so libmklml_intel.so
[meshell@/lib64] ln -s paddle_inference/third_party/install/mklml/lib/libiomp5.so libiomp5.so
[meshell@/lib64] ln -s paddle_inference/third_party/install/mkldnn/lib/libmkldnn.so.0 libdnnl.so.2
```


> 该库不支持`window`环境，如果你需要请自行编译。
