if(HAVE_BREAKPAD)
  set(FIND_BREAKPAD_QUIET 1)
endif(HAVE_BREAKPAD)

if(HAVE_DUMP_SYMBOLS)
  set(FIND_DUMP_SYMBOLS_QUIET 1)
endif(HAVE_DUMP_SYMBOLS)

OPTION(ENABLE_DUMP_SYMBOLS "Create breakpad symbols" ON)

set(DO_DUMP_SYMBOLS 0)
find_program(DUMP_SYMS dump_syms HINTS ${dependdir}/bin ${BREAKPAD_PATH}/bin)
if(DUMP_SYMS MATCHES "-NOTFOUND")
  message(STATUS "dump_syms not found, will not create breakpad symbols")
else(DUMP_SYMS MATCHES "-NOTFOUND")
  if(NOT FIND_DUMP_SYMBOLS_QUIET)
    message(STATUS "dump_syms found ${DUMP_SYMS}")
  endif(NOT FIND_DUMP_SYMBOLS_QUIET)
  if(ENABLE_DUMP_SYMBOLS)
    set(DO_DUMP_SYMBOLS 1)
  endif(ENABLE_DUMP_SYMBOLS)
endif(DUMP_SYMS MATCHES "-NOTFOUND")

set(HAVE_BREAKPAD 0)

if(APPLE)
  set(PLAT "mac")
elseif(UNIX)
  set(PLAT "linux")
else(APPLE)
  set(PLAT "windows")
endif(APPLE)

if(NOT BREAKPAD_ROOT)
  set(BREAKPAD_ROOT ${root}/../google-breakpad-read-only/src)
endif(NOT BREAKPAD_ROOT)

find_path(BREAKPAD_INCLUDES exception_handler.h HINTS ${BREAKPAD_ROOT} ${dependdir}/include/breakpad ${BREAKPAD_PATH}/include PATH_SUFFIXES client/${PLAT}/handler)
if(BREAKPAD_INCLUDES MATCHES "-NOTFOUND")
  message(STATUS "Can't find exception_handler.h")
else(BREAKPAD_INCLUDES MATCHES "-NOTFOUND")
  STRING(REPLACE "/client/${PLAT}/handler" "" BREAKPAD_INC_DIR ${BREAKPAD_INCLUDES})

  if(WIN32)
    set(BREAKPAD_PLAT ${BREAKPAD_ROOT}/client/${PLAT})
    set(BREAKPAD_LIB_HINTS ${BREAKPAD_PLAT}/Debug/lib ${BREAKPAD_PLAT}/Retail/lib ${dependdir}/lib ${BREAKPAD_PATH}/lib)

    find_library(LIBBREAKPAD_EXCEPTION_HANDLER NAMES exception_handler exception_handler_d HINTS ${BREAKPAD_LIB_HINTS})
    if(NOT LIBBREAKPAD_EXCEPTION_HANDLER MATCHES "-NOTFOUND")
      if(NOT FIND_BREAKPAD_QUIET)
        message(STATUS "Found breakpad exception_handler ${LIBBREAKPAD_EXCEPTION_HANDLER}")
      endif(NOT FIND_BREAKPAD_QUIET)
    endif(NOT LIBBREAKPAD_EXCEPTION_HANDLER MATCHES "-NOTFOUND")

    find_library(LIBBREAKPAD_COMMON NAMES common common_d HINTS ${BREAKPAD_LIB_HINTS})
    if(NOT LIBBREAKPAD_COMMON MATCHES "-NOTFOUND")
      if(NOT FIND_BREAKPAD_QUIET)
        message(STATUS "Found breakpad common ${LIBBREAKPAD_COMMON}")
      endif(NOT FIND_BREAKPAD_QUIET)
    endif(NOT LIBBREAKPAD_COMMON MATCHES "-NOTFOUND")


    find_library(LIBBREAKPAD_CRASH_GEN_CLIENT NAMES crash_generation_client crash_generation_client_d HINTS ${BREAKPAD_LIB_HINTS})
    if(NOT LIBBREAKPAD_CRASH_GEN_CLIENT MATCHES "-NOTFOUND")
      if(NOT FIND_BREAKPAD_QUIET)
        message(STATUS "Found breakpad crash_generation_client ${LIBBREAKPAD_CRASH_GEN_CLIENT}")
      endif(NOT FIND_BREAKPAD_QUIET)
    endif(NOT LIBBREAKPAD_CRASH_GEN_CLIENT MATCHES "-NOTFOUND")

    if(LIBBREAKPAD_EXCEPTION_HANDLER AND LIBBREAKPAD_COMMON AND LIBBREAKPAD_CRASH_GEN_CLIENT)
      set(HAVE_BREAKPAD 1)
    else(LIBBREAKPAD_EXCEPTION_HANDLER AND LIBBREAKPAD_COMMON AND LIBBREAKPAD_CRASH_GEN_CLIENT)
      message(STATUS "No breakpad support")
    endif(LIBBREAKPAD_EXCEPTION_HANDLER AND LIBBREAKPAD_COMMON AND LIBBREAKPAD_CRASH_GEN_CLIENT)

  else(WIN32)
    find_library(LIBBREAKPAD_CLIENT NAMES breakpad_client breakpad breakpad-d HINTS ${dependdir}/lib ${BREAKPAD_PATH}/lib)
    if(LIBBREAKPAD_CLIENT MATCHES "-NOTFOUND")
      message(STATUS "No breakpad support")
    else(LIBBREAKPAD_CLIENT MATCHES "-NOTFOUND")
      if(NOT FIND_BREAKPAD_QUIET)
        message(STATUS "Found breakpad_client ${LIBBREAKPAD_CLIENT}")
      endif(NOT FIND_BREAKPAD_QUIET)
      set(HAVE_BREAKPAD 1)
    endif(LIBBREAKPAD_CLIENT MATCHES "-NOTFOUND")
  endif(WIN32)
endif(BREAKPAD_INCLUDES MATCHES "-NOTFOUND")