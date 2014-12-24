######### Shared configuration between POSIX platforms
############ Special Clang cflags
option(CLANG_COLOR "Show CLang color during compile" ON)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
  if(CLANG_COLOR)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -fcolor-diagnostics")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fcolor-diagnostics")
  endif(CLANG_COLOR)
endif(CMAKE_C_COMPILER_ID STREQUAL "Clang")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(DISABLED_WARNINGS "-Wno-reorder -Wno-sign-compare -Wno-unused-variable -Wno-format")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(DISABLED_WARNINGS "-Wno-parentheses-equality -Wno-self-assign-field")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe ${DISABLED_WARNINGS}")
endif(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")  

############ Set global CFlags with the information from the subroutines
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS}")

############ Disable optimization when building for Debug
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

############ Generate debug symbols even when we build for Release
if(TARGET_RPI)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O3")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")
else(TARGET_RPI)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -Os")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Os")
endif(TARGET_RPI)

############ Check for pthread_setname -> HAVE_PTHREAD_SETNAME_NP
if(TARGET_FREEBSD)
  set(HAVE_PTHREAD_SET_NAME_NP 1)
else()
  set(HAVE_PTHREAD_SETNAME_NP 1)
endif()

if(NOT TARGET_RPI)
  set(HAS_SDL_JOYSTICK 1)
endif()
set(HAS_LIBRTMP 1)

include(CheckCXXSymbolExists)
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
CHECK_CXX_SYMBOL_EXISTS("pthread_condattr_setclock" "pthread.h" HAVE_PTHREAD_CONDATTR_SETCLOCK)

if(NOT USE_INTERNAL_FFMPEG)
  add_definitions(-DUSE_EXTERNAL_FFMPEG)
else()
  add_definitions(-DUSE_INTERNAL_FFMPEG)
endif()

add_definitions(
  -DTARGET_POSIX
  -D_REENTRANT
  -D_FILE_DEFINED
  -D__STDC_CONSTANT_MACROS
  -DHAVE_CONFIG_H
  -D_FILE_OFFSET_BITS=64
  -D_LARGEFILE64_SOURCE
  -D_LINUX
  -D__STDC_LIMIT_MACROS
)
