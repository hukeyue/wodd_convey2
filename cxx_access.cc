// SPDX-License-Identifier: CDDL-1.0
/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or https://opensource.org/licenses/CDDL-1.0.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE
#endif // __linux__

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem>

#ifdef _WIN32
#define CALL_STDOUT_PRINTLN(format, ...) \
  (__extension__({ \
    wchar_t buffer[4096]; \
    int len = _snwprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), L ## format L"\n", ##__VA_ARGS__); \
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer, len, NULL, NULL); \
  }))
#define CALL_STDERR_PRINTLN(format, ...) \
  (__extension__({ \
    wchar_t buffer[4096]; \
    int len = _snwprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), L ## format L"\n", ##__VA_ARGS__); \
    WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), buffer, len, NULL, NULL); \
  }))
#define CALL_STDERR_PRINTLN_WITH_ERRORS(format, ...) \
  (__extension__({ \
    wchar_t buffer[4096]; \
    int len = _snwprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), L ## format L" due to '%s'\n", ##__VA_ARGS__, wGetLastErrorMessage()); \
    WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), buffer, len, NULL, NULL); \
  }))
#else
#define CALL_STDOUT_PRINTLN(format, ...) \
  (__extension__({ \
    fprintf(stdout, format "\n", ##__VA_ARGS__); \
  }))
#define CALL_STDERR_PRINTLN(format, ...) \
  (__extension__({ \
    fprintf(stderr, format "\n", ##__VA_ARGS__); \
  }))
#define CALL_STDERR_PRINTLN_WITH_GNU_ERRORS(format, ...) \
  (__extension__({ \
    fprintf(stderr, format " due to the fact that GNU extensions encountered ERROR: '%s'\n", ##__VA_ARGS__, strerror_g(errno)); \
  }))
#define CALL_STDERR_PRINTLN_WITH_ERRORS(format, ...) \
  (__extension__({ \
    fprintf(stderr, format " due to '%s'\n", ##__VA_ARGS__, strerror(errno)); \
  }))
#endif

void usage(
#ifdef _WIN32
  const wchar_t* exec_name
#else
  const char* exec_name
#endif
)
{
  CALL_STDERR_PRINTLN("%s Usage: access <file path>", exec_name);
  exit(-2);
}

#ifdef _WIN32
int wmain(int argc, const wchar_t* argv[]) {
#else
int main(int argc, const char* argv[]) {
#endif
  int d;
#ifdef _WIN32
  d = wcslen(argv[1]);
#else
  d = strlen(argv[1]);
#endif
  if (argc != 2 || d == 0) {
    usage(argv[0]);
  }
  std::error_code ec;
  auto Stat = std::filesystem::symlink_status(argv[1], ec);
  if (!ec) {
    if (std::filesystem::is_regular_file(Stat))
      CALL_STDOUT_PRINTLN("READ Permission OK on %s", argv[1]);
    else if (std::filesystem::is_directory(Stat))
      CALL_STDOUT_PRINTLN("DREAD Permission OK on %s", argv[1]);
    else if (std::filesystem::is_symlink(Stat))
      CALL_STDOUT_PRINTLN("LREAD Permission OK on %s", argv[1]);
    else if (std::filesystem::exists(Stat))
      CALL_STDOUT_PRINTLN("EREAD Permission OK on %s", argv[1]);
    else
      CALL_STDOUT_PRINTLN("Not available %s", argv[1]);
    fflush(stdout);
    return 0;
  }
  std::string message = ec.message();
#ifdef _WIN32
  const size_t sz = 4096;
  wchar_t buffer[sz];
  size_t real_sz;
  wchar_t *errMessageStr = buffer;
  errno_t ret = mbstowcs_s(&real_sz, errMessageStr, sz, message.c_str(), message.size());
  assert(!ret && "Error message converision failed");
  static_cast<void>(ret);
#else
  const char *errMessageStr = message.c_str();
#endif
  CALL_STDERR_PRINTLN("%s %s", errMessageStr, argv[1]);
  return -1;
}
