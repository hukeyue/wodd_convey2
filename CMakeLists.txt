cmake_minimum_required(VERSION 4.0)

if (CMAKE_CURRENT_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "Dummy build is not support")
endif()

## ESCALASTION
## ESCAPE
## KEY INVALID -- SERIAL NUMBER CALLED -- STALL

project(wodd_convey VERSION 2.02 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

### SANITIY CHECK AFTER BOOT (AMI)

if (MSVC)
  set(CMAKE_GENERATOR_CC cl)
endif()

### INITIALIZATION (COFF)

include(CheckLibraryExists)

### WIN32 Platform Specific (preprocessor)

if (WIN32)
  add_definitions(-D_WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
  add_definitions(-DWINVER=0x0501)
  add_definitions(-DNOMINMAX)
  add_definitions(-D_UNICODE)
  add_definitions(-DUNICODE)

  if (MSVC)
    add_link_options(/DYNAMICBASE /NXCOMPAT)
  endif()

  set(SUBSYSTEM_VERSION_SUFFIX ",5.01")
  if (MSVC)
    set(CMAKE_C_CREATE_WIN32_EXE "/subsystem:windows${SUBSYSTEM_VERSION_SUFFIX}")
    set(CMAKE_CXX_CREATE_WIN32_EXE "/subsystem:windows${SUBSYSTEM_VERSION_SUFFIX}")
    set(CMAKE_C_CREATE_CONSOLE_EXE "/subsystem:console${SUBSYSTEM_VERSION_SUFFIX}")
    set(CMAKE_CXX_CREATE_CONSOLE_EXE "/subsystem:console${SUBSYSTEM_VERSION_SUFFIX}")
  else()
    set(CMAKE_C_CREATE_WIN32_EXE "-mwindows -Xlinker --subsystem -Xlinker windows${SUBSYSTEM_VERSION_SUFFIX}")
    set(CMAKE_CXX_CREATE_WIN32_EXE "-mwindows -Xlinker --subsystem -Xlinker windows${SUBSYSTEM_VERSION_SUFFIX}")
    set(CMAKE_C_CREATE_CONSOLE_EXE "-mconsole -Xlinker --subsystem -Xlinker console${SUBSYSTEM_VERSION_SUFFIX}")
    set(CMAKE_CXX_CREATE_CONSOLE_EXE "-mconsole -Xlinker --subsystem -Xlinker console${SUBSYSTEM_VERSION_SUFFIX}")
  endif()
endif()

if (MSVC)
  # Some of our files are bigger than the regular limits.
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/bigobj>)

  ## Set Source and Excutable character sets to UTF-8
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>)

  # lld's uac manifest is outdated and incompatible with windows xp
  add_link_options(/MANIFESTUAC:NO)
endif()

if (MINGW)
  add_link_options(-municode)
endif()

if (NOT MSVC)
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:-fdata-sections>)
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:-ffunction-sections>)
  add_link_options(-fdata-sections -ffunction-sections)
  if (APPLE)
    add_link_options(-Wl,-dead_strip)
  else()
    add_link_options(-Wl,--gc-sections)
  endif()
endif()

### INITIALIZATION (COFF)

include(CheckLibraryExists)
if (WIN32)
  check_library_exists(shell32 CloseHandle "" HAVE_WIN32_CLOSE_HANDLE)
  check_library_exists(shell32 ReadFile "" HAVE_WIN32_READ_FILE)
  check_library_exists(shell32 WriteFile "" HAVE_WIN32_WRITE_FILE)
  check_library_exists(shell32 CreateFileW "" HAVE_WIN32_CREATE_FILE_W)
  check_library_exists(shell32 SetFilePointerEx "" HAVE_WIN32_SET_FILE_POINTER_EX)
  check_library_exists(shell32 FlushFileBuffers "" HAVE_WIN32_FLUSH_FILE_BUFFERS)
else()
  check_library_exists(c lseek "" HAVE_LSEEK)
  check_library_exists(c lseek64 "" HAVE_LSEEK64)
  check_library_exists(c read "" HAVE_READ)
  check_library_exists(c write "" HAVE_WRTIE)
  check_library_exists(c fsync "" HAVE_FSYNC)
endif()

### INBED EXECUTOR (odd_convey)
add_executable(access access.c)
add_executable(knr_access knr_access.c)
add_executable(odd_convey odd_convey.c)
if (WIN32)
  link_libraries(shell32 kernel32)
endif()

### POST STAGE (Linkage)
# stale
