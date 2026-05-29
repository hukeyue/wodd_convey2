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

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <errno.h>
#endif
#include <string.h>

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
#define CALL_STDERR_PRINTLN_WITH_ERRORS(format, ...) \
  (__extension__({ \
    fprintf(stderr, format " due to '%s'\n", ##__VA_ARGS__, strerror(errno)); \
  }))
#endif

#ifdef _WIN32
void usage(const wchar_t* exec_name) {
#else
void usage(const char* exec_name) {
#endif
  CALL_STDERR_PRINTLN("%s Usage: access <file path>", exec_name);
  _exit(-2);
}

#ifdef _WIN32
const wchar_t* wGetLastErrorMessage() {
  DWORD errorCode = GetLastError();
  static WCHAR buffer[64 << 10];

  DWORD result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    buffer, sizeof(buffer)/sizeof(buffer[0]), NULL);

  while (result && iswspace(buffer[result-1])) {
    buffer[--result] = L'\0';
  }

  if (result == 0) {
    return L"Failed to get error message";
  }

  return buffer;
}
#endif

#ifdef _WIN32
int wmain(int argc, const wchar_t* argv[]) {
  DWORD tmp;
#else
int main(int argc, const char* argv[]) {
  int tmp;
#endif
  if (argc != 2) {
    usage(argv[0]);
  }
#ifdef _WIN32
  if (wcslen(argv[1]) == 0) {
#else
  if (strlen(argv[1]) == 0) {
#endif
    usage(argv[0]);
  }

#ifdef _WIN32
  tmp = GetFileAttributesW(argv[1]);
#else
  tmp = access(argv[1], R_OK);
#endif
  (void)&tmp;
#ifdef _WIN32
  if (tmp == INVALID_FILE_ATTRIBUTES) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Not accessible '%s'", argv[1]);
    goto done;
  } else if (tmp & FILE_ATTRIBUTE_DIRECTORY) {
    CALL_STDOUT_PRINTLN("READ Permission OK on directory '%s'", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_NORMAL) {
    CALL_STDOUT_PRINTLN("READ Permission OK on normal file '%s'", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_SYSTEM) {
    CALL_STDOUT_PRINTLN("READ Permission OK on system file '%s'", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_HIDDEN) {
    CALL_STDOUT_PRINTLN("READ Permission OK on hidden file '%s'", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_COMPRESSED) {
    CALL_STDOUT_PRINTLN("READ Permission OK on compressed file '%s'", argv[1]);
  } else {
    CALL_STDOUT_PRINTLN("READ Permission OK on file '%s'", argv[1]);
  }
  exit(0);
done:
#else
  switch(tmp) {
    case 0:
      CALL_STDOUT_PRINTLN("READ Permission OK on file '%s'", argv[1]);
      exit(0);
    break;
    case -1:
      CALL_STDERR_PRINTLN_WITH_ERRORS("Not accessible '%s'", argv[1]);
    break;
    default:
      CALL_STDERR_PRINTLN_WITH_ERRORS("Encountered invisited error");
      usage(argv[1]);
    break;
  }
#endif

  return -1;
}
