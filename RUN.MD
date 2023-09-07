   
1. 源码编译安装 gstream

源码编译 https://gstreamer.freedesktop.org/documentation/installing/building-from-source-using-cerbero.html?gi-language=c

2. 安装libmpg123

官网 http://mpg123.orgis.org/

源码 http://mpg123.orgis.org/download/?V=1&O=D


3. GStreamer实现MP3音频播放

https://poemdear.com/2019/08/12/gstreamer%E5%AE%9E%E7%8E%B0mp3%E9%9F%B3%E9%A2%91%E6%92%AD%E6%94%BE/


```sh
meson builddir
ninja -C builddir
meson install -C builddir


```