当前目录下的ffmpeg source code 需要单独编译，因为在android Q 的编译环境下，编译ffmpeg会提示类似下面的错误：


ld.lld: error: can't create dynamic relocation R_ARM_V4BX against local symbol in readonly segment; recompile object files with -fPIC or pass '-Wl,-z,notext' to allow text relocations in the output
>>> defined in out/target/product/sdm710/obj_arm/SHARED_LIBRARIES/libavutil_vendor_intermediates/arm/float_dsp_neon.o
>>> referenced by out/target/product/sdm710/obj_arm/SHARED_LIBRARIES/libavutil_vendor_intermediates/arm/float_dsp_neon.o:(ff_vector_fmul_reverse_neon)

可以参考这个网站的说明：
https://android.googlesource.com/platform/bionic/+/master/android-changes-for-ndk-developers.md#Text-Relocations-Enforced-for-API-level-23

按照提示 编译的时候使用 -Wl,-z,notext 后可以编译通过 ，但是在dlopen 相关的so 的时候 会失败 ，提示so 里面有 relocations text，出现 relocations text
的代码 的函数 ：ff_vector_fmul_reverse_neon 这个函数 是一个汇编文件 无法把它修改为不带relocations text 的代码。

Note: it is technically possible to have a shared object with the TEXTREL entry/flag but without any actual text relocations. This doesn‘t happen with the NDK,
=>按照这个提示 可以使用NDK 的编译环境 试试。

目前使用NDK 的编译环境编译出来的so 可以正常运行。

所以当前目录的source 需要配置NDK 的编译环境，NDK的编译环境可以请配置组帮忙配置，或者到多媒体SVN 中去下载：多媒体SVN\NDK\ndk.tar.gz

解压后，配好NDK 环境后 需要在build_ffmpeg_android.sh 文件中修改你的NDK 环境变量

NDK=/work/mm_dashuang.zhang/ndk/android-ndk-r10e

修改为你自己的配置。

