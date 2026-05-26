cmake_minimum_required(VERSION 3.13)

if (CMAKE_CURRENT_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "Dummy build is not support")
endif()

## ESCALASTION
# Select the MSVC runtime library for use by compilers targeting the MSVC ABI.
#
# Introduced by cmake 3.15
# https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html#variable:CMAKE_MSVC_RUNTIME_LIBRARY
# and https://cmake.org/cmake/help/latest/policy/CMP0091.html
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

## ESCAPE
## KEY INVALID -- SERIAL NUMBER CALLED -- STALL

project(wodd_convey VERSION 2.04 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

### SANITIY CHECK AFTER BOOT (AMI)

if (MSVC)
  set(CMAKE_GENERATOR_CC cl)
endif()

### INITIALIZATION (COFF)

include(CheckLibraryExists)

### WIN32 Platform Specific (preprocessor)

if (WIN32)
  ## definitions FOR CRT
  add_definitions(-D__STD_C)
  add_definitions(-D_CRT_RAND_S)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)

  ## definitions FOR Windows SDK
  add_definitions(-D_WIN32)
  add_definitions(-D_WIN32_WINNT=0x0A00)
  add_definitions(-DWINVER=0x0A00)
  add_definitions(-DNOMINMAX)
  add_definitions(-D_UNICODE)
  add_definitions(-DUNICODE)

  if (MSVC)
    add_link_options(/DYNAMICBASE /NXCOMPAT)
  endif()

  set(SUBSYSTEM_VERSION_SUFFIX ",6.00")
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
  # Enable function-level linking.
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/Gy>)

  # Preserve previous PDB behavior.
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/FS>)

  # Some of our files are bigger than the regular limits.
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/bigobj>)

  ## Set Source and Excutable character sets to UTF-8
  add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>)

  if (CMAKE_LINKER MATCHES "lld")
    # lld's uac manifest is outdated and incompatible with windows xp
    add_link_options(/MANIFESTUAC:NO)
  endif()

  ## not omit stack frame pointer
  add_compile_options($<$<AND:$<CONFIG:RELEASE>,$<COMPILE_LANGUAGE:C,CXX>>:/Oy->)

  ## Generate debug info, Debug information in the .obj files for clang.
  ## Produce PDB file for msvc, no edit and continue.
  if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Debug information in the .obj files.
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/Z7>)

    ## To generate PDB files for debugging with MSVC, you can use the /Z7 option.
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
      string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
      string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
      string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
      string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
      string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
      string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
    endif()
  endif()

  # Turn this on to use ghash feature of lld for faster debug link on Windows.
  # http://blog.llvm.org/2018/01/improving-link-time-on-windows-with.html
  if (CMAKE_LINKER MATCHES "lld")
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:-gcodeview-ghash>)
    add_link_options(/DEBUG:GHASH)
  else()
    add_link_options(/DEBUG)
  endif()
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

if (WIN32)
  include(CheckCSourceCompiles)

  check_c_source_compiles ("
#include <windows.h>
int main() {
  CloseHandle(INVALID_HANDLE_VALUE);
}
" HAVE_WIN32_CLOSE_HANDLE)

  check_c_source_compiles ("
#include <windows.h>
int main() {
  ReadFile(INVALID_HANDLE_VALUE, NULL, 0, NULL, NULL);
}
" HAVE_WIN32_READ_FILE)

  check_c_source_compiles ("
#include <windows.h>
int main() {
  WriteFile(INVALID_HANDLE_VALUE, NULL, 0, NULL, NULL);
}
" HAVE_WIN32_WRITE_FILE)

  check_c_source_compiles ("
#include <windows.h>
int main() {
  CreateFileW(L\"NUL\", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}
" HAVE_WIN32_CREATE_FILE_W)

  check_c_source_compiles ("
#include <windows.h>
int main() {
  LARGE_INTEGER d;
  d.QuadPart = 0;
  SetFilePointerEx(INVALID_HANDLE_VALUE, d, NULL, FILE_BEGIN);
}
" HAVE_WIN32_SET_FILE_POINTER_EX)

  check_c_source_compiles ("
#include <windows.h>
int main() {
  FlushFileBuffers(INVALID_HANDLE_VALUE);
}
" HAVE_WIN32_FLUSH_FILE_BUFFERS)
else()
  include(CheckLibraryExists)
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

### POST STAGE (Linkage)
if (MSVC)
  target_sources(access PRIVATE win32.manifest)
  target_sources(knr_access PRIVATE win32.manifest)
  target_sources(odd_convey PRIVATE win32.manifest)
endif()

if (MINGW)
  target_sources(access PRIVATE win32.rc)
  target_sources(knr_access PRIVATE win32.rc)
  target_sources(odd_convey PRIVATE win32.rc)
endif()

if (WIN32)
  target_sources(access PRIVATE win32_version.rc)
  target_sources(knr_access PRIVATE win32_version.rc)
  target_sources(odd_convey PRIVATE win32_version.rc)

  link_libraries(shell32 kernel32)
endif()
