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
void usage(const wchar_t* exec_name) {
  fprintf(stderr, "%ws Usage: access <file path>\n", exec_name);
  fflush(stderr);
  _exit(-2);
}
#else
void usage(const char* exec_name) {
  fprintf(stderr, "%s Usage: access <file path>\n", exec_name);
  fflush(stderr);
  _exit(-2);
}
#endif

#ifdef _WIN32
const wchar_t* wGetLastErrorMessage() {
  DWORD errorCode = GetLastError();
  static WCHAR buffer[64 << 10];

  DWORD result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    buffer, sizeof(buffer)/sizeof(buffer[0]), NULL);

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
  tmp = access(argv[1], R_OK); // TODO: GetFileAttributeW
#endif
  (void)&tmp;
#ifdef _WIN32
  if (tmp == INVALID_FILE_ATTRIBUTES) {
    fprintf(stderr, "Not accessible '%ws' due to '%ws'\n", argv[1], wGetLastErrorMessage());
    fflush(stderr);
    goto done;
  } else if (tmp & FILE_ATTRIBUTE_DIRECTORY) {
    fprintf(stdout, "READ Permission OK on directory '%ws'\n", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_NORMAL) {
    fprintf(stdout, "READ Permission OK on normal file '%ws'\n", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_SYSTEM) {
    fprintf(stdout, "READ Permission OK on system file '%ws'\n", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_HIDDEN) {
    fprintf(stdout, "READ Permission OK on hidden file '%ws'\n", argv[1]);
  } else if (tmp & FILE_ATTRIBUTE_COMPRESSED) {
    fprintf(stdout, "READ Permission OK on compressed file '%ws'\n", argv[1]);
  } else {
    fprintf(stdout, "READ Permission OK on file '%ws'\n", argv[1]);
  }
  exit(0);
done:
#else
  switch(tmp) {
    case 0:
      fprintf(stdout, "READ Permission OK on file '%s'\n", argv[1]);
      exit(0);
    break;
    case -1:
      fprintf(stderr, "Not accessible '%s' due to '%s'\n", argv[1], strerror(errno));
      fflush(stderr);
    break;
    default:
      fprintf(stderr, "Encountered invisited error: '%s'\n", strerror(errno));
      fflush(stderr);
      usage(argv[1]);
    break;
  }
#endif

  return -1;
}
