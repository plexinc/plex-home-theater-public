include(ExternalProject)

if(TARGET_LINUX)
  set(OS linux)
elseif(TARGET_DARWIN)
  set(OS darwin)
endif()


if(NOT TARGET_RPI)
  set(CONFIG_FLAGS --target-os=${OS} --sysroot="${CMAKE_PREFIX_PATH}" --sysinclude="${CMAKE_PREFIX_PATH}/usr/include" --disable-altivec --disable-amd3dnow --disable-avisynth --disable-crystalhd --disable-debug --disable-decoder=mpeg_xvmc --disable-devices --disable-doc --disable-dxva2 --disable-encoders --disable-extra-warnings --disable-ffmpeg --disable-ffplay --disable-ffprobe --disable-ffserver --disable-frei0r --disable-gray --disable-hardcoded-tables --disable-indevs --disable-iwmmxt --disable-libdc1394 --disable-libdirac --disable-libfaac --disable-libfreetype --disable-libgsm --disable-libmp3lame --disable-libnut --disable-libopencore-amrnb --disable-libopencore-amrwb --disable-libopencv --disable-libopenjpeg --disable-librtmp --disable-libschroedinger --disable-libspeex --disable-libtheora --disable-libvo-aacenc --disable-libvo-amrwbenc --disable-libvorbis --disable-libvpx --disable-libx264 --disable-libxavs --disable-libxvid --disable-memalign-hack --disable-mlib --disable-mmi --disable-muxers --disable-nonfree --disable-outdevs --disable-small --disable-sram --disable-static --disable-symver --disable-vda --disable-version3 --disable-vis --disable-w32threads --disable-x11grab --enable-aandct --enable-asm --enable-avcodec --enable-avdevice --enable-avfilter --enable-avformat --enable-bsfs --enable-bzlib --enable-dct --enable-demuxers --enable-fastdiv --enable-fft --enable-filters --enable-golomb --enable-gpl --enable-huffman --enable-hwaccels --enable-libvorbis --enable-logging --enable-lpc --enable-mdct --enable-muxer=adts --enable-muxer=ogg --enable-muxer=spdif --enable-network --enable-optimizations --enable-parsers --enable-pic --enable-postproc --enable-protocol=http --enable-pthreads --enable-rdft --enable-runtime-cpudetect --enable-shared --enable-stripping --enable-swscale --enable-swscale-alpha --enable-vaapi --enable-vdpau --enable-yasm --enable-zlib --host-libs=-lm)
else()
  set(CONFIG_FLAGS --target-os=${OS} --disable-muxers --disable-encoders --disable-devices --disable-doc --disable-ffplay --disable-ffmpeg --disable-ffprobe --disable-ffserver --disable-vda --disable-crystalhd --disable-decoder=mpeg_xvmc --disable-debug --disable-amd3dnow --disable-libvorbis --enable-gpl --enable-postproc --enable-shared --disable-static --enable-pthreads --enable-muxer=spdif --enable-muxer=adts --enable-encoder=ac3 --enable-encoder=aac --enable-protocol=http --enable-runtime-cpudetect)

  list(APPEND CONFIG_FLAGS --enable-hardcoded-tables --enable-pic
  --cpu=arm1176jzf-s --arch=arm --enable-cross-compile
  --cross-prefix=${TARGET_PREFIX} --sysroot=${SYSROOT_PREFIX}
  --sysinclude="${SYSROOT_PREFIX}/usr/include" )

endif()

ExternalProject_Add(
  ffmpeg
  URL .
  PREFIX ffmpeg
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> ${CONFIG_FLAGS}
  BUILD_COMMAND make -j 4
  INSTALL_COMMAND make install
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libavformat.so.53.32.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME avformat-53-${ARCH}.so)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libavutil.so.51.35.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME avutil-51-${ARCH}.so)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libpostproc.so.52.0.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME postproc-52-${ARCH}.so)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libswscale.so.2.1.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME swscale-2-${ARCH}.so)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libavfilter.so.2.61.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME avfilter-2-${ARCH}.so)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libswresample.so.0.6.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME swresample-0-${ARCH}.so)


OPTION(ENABLE_UBUNTU_FFMPEG_HACK "Work around for getting the internal ffmpeg to work with deb-helper" OFF)
if(ENABLE_UBUNTU_FFMPEG_HACK)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libavformat.so.53.32.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME libavformat.so.53)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libavutil.so.51.35.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME libavutil.so.51)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libpostproc.so.52.0.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME libpostproc.so.52)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libswscale.so.2.1.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME libswscale.so.2)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libavfilter.so.2.61.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME libavfilter.so.2)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ffmpeg/lib/libswresample.so.0.6.100 DESTINATION ${BINPATH}/system/players/dvdplayer RENAME libswresample.so.0)
endif(ENABLE_UBUNTU_FFMPEG_HACK)
