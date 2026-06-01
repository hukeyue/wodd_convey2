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
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BIT
#define _FILE_OFFSET_BIT 64
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE
#endif

#include <fcntl.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <signal.h>
#include <stdio.h>
#ifndef _WIN32
#include <errno.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#ifndef _WIN32
#define INVALID_HANDLE_VALUE (-1)
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
  (__extension__({ \
    long int __result; \
    do __result = (long int)(expression); \
    while(__result == -1L && errno == EINTR); \
    __result; \
  }))
#endif
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
    fprintf(stderr, format ": %s\n", ##__VA_ARGS__, strerror(errno)); \
  }))
#define CALL_READ(a, b, c) TEMP_FAILURE_RETRY(read(a, b, c))
#define CALL_WRITE(a, b, c) TEMP_FAILURE_RETRY(write(a, b, c))
#define CALL_FLUSH(a) TEMP_FAILURE_RETRY(fsync(a))
#define CALL_CLOSE(a) TEMP_FAILURE_RETRY(close(a))
#endif

#ifdef __linux__
#if __TOTAL_SIZE_WIDTH__ < 64
#define CALL_LSEEK(a, b, c) TEMP_FAILURE_RETRY(lseek64(a, b, c))
#else
#define CALL_LSEEK(a, b, c) TEMP_FAILURE_RETRY(lseek(a, b, c))
#endif // __TOTAL_SIZE_WIDTH__
#endif // __linux__

#ifdef __APPLE__
#include <sys/types.h>
#define CALL_LSEEK(a, b, c) TEMP_FAILURE_RETRY(lseek(a, b, c))
typedef __darwin_off_t loff_t;
#endif // __APPLE__

#ifdef __FreeBSD__
#include <sys/types.h>
#ifndef _OFF64_T_DECLARED
#error Offset (Alias) not defined
#endif
#define CALL_LSEEK(a, b, c) TEMP_FAILURE_RETRY(lseek(a, b, c))
typedef __off64_t loff_t;
#endif // __FreeBSD__

#ifdef _WIN32
typedef ULONGLONG loff_t; /* Same with QuadPart under LARGE_INTEGER */
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
    int len = _snwprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), L ## format L": %s\n", ##__VA_ARGS__, wGetLastErrorMessage()); \
    WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), buffer, len, NULL, NULL); \
  }))
#define CALL_LSEEK(a, b, c) \
  (__extension__({ \
    LARGE_INTEGER B; \
    B.QuadPart = b; \
    lseekInt(a, B, c); \
  }))
#define CALL_READ(a, b, c) \
  (__extension__({ \
    DWORD bytesRead; \
    if (!ReadFile(a, b, c, &bytesRead, NULL)) \
      bytesRead = -1; \
    bytesRead; \
  }))
#define CALL_WRITE(a, b, c) \
  (__extension__({ \
    DWORD bytesWritten; \
    if (!WriteFile(a, b, c, &bytesWritten, NULL)) \
      bytesWritten = -1; \
    bytesWritten; \
  }))
#define CALL_FLUSH(a) \
  (__extension__({ \
    FlushFileBuffers(a) ? 0 : -1; \
  }))
#define CALL_CLOSE(a) \
  (__extension__({ \
    CloseHandle(a) ? 0 : -1; \
  }))
#endif

#ifdef __linux__
#define INPUT_NAME "/proc/kcore"
// #define INPUT_NAME "/dev/zero"
#endif
#ifdef __APPLE__
#define INPUT_NAME "/dev/zero"
#endif

#ifdef __FreeBSD__
#define INPUT_NAME "/dev/zero"
#endif

#ifdef _WIN32
#define INPUT_NAME L"NUL"
#endif

#ifdef _WIN32
#define OUTPUT_NAME L"NUL"
#else
#define OUTPUT_NAME "/dev/null"
#endif

#define K               (1ull << 10)
#define M               (1ull << 20)
#define G               (1ull << 30)
#define T               (1ull << 40)
#define OFFSET          (12ull * T)
#define TOTAL_SIZE      (16ull * T)
#define SLICE           (8ul * M)
#define SLICE_ALIGNMENT (4ul * K)

#if defined(_MSC_VER) && !defined(__clang__)
#define likely(x)     (x)
#define unlikely(x)   (x)
#else
#define likely(x)     __builtin_expect(!!(x), 1)
#define unlikely(x)   __builtin_expect(!!(x), 0)
#endif

/* hold in-place */
#ifdef _WIN32
static void usage(const wchar_t* p_name) {
#else
static void usage(const char* p_name) {
#endif
  CALL_STDERR_PRINTLN("%s <in-place> <usage>", p_name);
}

static void dot(loff_t *last_seen, loff_t last_read) {
  if (unlikely(last_read == TOTAL_SIZE)) {
    CALL_STDOUT_PRINTLN("Progress 100 over 100 percent (STAR)");
    *last_seen = last_read;
    fflush(stdout); /* fsync on last dot */
  } else if (/*guess*/ last_read - *last_seen >= TOTAL_SIZE / 100) {
    CALL_STDOUT_PRINTLN("Progress %lld/100 percent", (long long)last_read * 100 / TOTAL_SIZE);
    *last_seen = last_read;
  }
}

#ifdef _WIN32
static const wchar_t* wGetLastErrorMessage() {
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
static int lseekInt(HANDLE hFile, LARGE_INTEGER offset, int whence) {
  DWORD dwMoveMethod;
  switch (whence) {
    case SEEK_SET:
      dwMoveMethod = FILE_BEGIN;
      break;
    case SEEK_CUR:
      dwMoveMethod = FILE_CURRENT;
      break;
    case SEEK_END:
      dwMoveMethod = FILE_END;
      break;
    default:
      return -1;
  }
  if (!SetFilePointerEx(hFile, offset, NULL, dwMoveMethod)) {
    return -1;
  }
  return 0;
}
#endif

static int32_t odd_random() {
#if defined(__APPLE__) || defined(__FreeBSD__)
  return arc4random(); // always succeeds
#elif defined(__linux__)
  int32_t result;
  int ret = getentropy(&result, sizeof(result));
  assert(ret == 0 && "getentropy failure");
  return result;
#elif defined(_WIN32)
  int32_t result;
  ULONG output_bytes_this_pass = sizeof(result);
  NTSTATUS ret = BCryptGenRandom(/*hAlgorithm=*/NULL, (uint8_t*)&result, output_bytes_this_pass,
                                 BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  assert(BCRYPT_SUCCESS(ret) && "BCryptGenRandom failure");
  return result;
#endif
}

static int exit_code = 0;
static void signal_handler(int sig) {
  if (sig == SIGINT)
    exit_code = 1;
}

#define CHECK_EXIT_CODE_AND_LEAVE() \
  do { \
    if (unlikely(exit_code)) \
      goto close_and_exit; \
  } while (0)

#ifdef _WIN32
int wmain(int argc, const wchar_t* argv[]) {
#else
int main(int argc, const char* argv[]) {
#endif
  loff_t last_read = 0;
  loff_t last_seen = 0;
#ifdef _WIN32
  const wchar_t* const prog_name = argv[0];
#else
  const char* const prog_name = argv[0];
#endif
  void* p_buffer = NULL;
#ifdef _WIN32
  HANDLE ifd = INVALID_HANDLE_VALUE, ofd = INVALID_HANDLE_VALUE;
#else
  int ifd = INVALID_HANDLE_VALUE, ofd = INVALID_HANDLE_VALUE;
#endif

  /* hang on --background boot */
  if (argc > 1) {
    usage(prog_name);
    fflush(stdout);
    goto close_and_exit;
  }

  /* deal with SIGINT signal */
  if (signal(SIGINT, signal_handler) != 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Unstable signal handler");
    goto close_and_exit;
  }

  /* check point 0: close console input steam */
  CHECK_EXIT_CODE_AND_LEAVE();

#ifdef _WIN32
  fclose(stdin);
#else
  /* eject when unsafe */
  if (close(STDIN_FILENO) < 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Unstable input tty console");
    goto close_and_exit;
  }
#endif

  /* clean previous output up */
  fflush(stdout);

  /* check point 1: open input stream */
  CHECK_EXIT_CODE_AND_LEAVE();

#ifdef _WIN32
  ifd = CreateFileW(INPUT_NAME, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
  ifd = open(INPUT_NAME, O_RDONLY, 0);
#endif
  if (ifd == INVALID_HANDLE_VALUE) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to open file (input) %s", INPUT_NAME);
    goto close_and_exit;
  }

  /* check point 2: open close stream */
  CHECK_EXIT_CODE_AND_LEAVE();

#ifdef _WIN32
  ofd = CreateFileW(OUTPUT_NAME, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
  ofd = open(OUTPUT_NAME, O_WRONLY, 0);
#endif
  if (ofd == INVALID_HANDLE_VALUE) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to open file (output) %s", OUTPUT_NAME);
    goto close_and_exit;
  }

  /* check point 3: allocate temporary buffer */
  CHECK_EXIT_CODE_AND_LEAVE();

  /* Allocate the slice buffer (quite large) */
#ifdef _WIN32
  p_buffer = _aligned_malloc(SLICE, SLICE_ALIGNMENT);
  if (!p_buffer) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to allocate memory");
    goto close_and_exit;
  }
#else
  int ret = posix_memalign(&p_buffer, SLICE_ALIGNMENT, SLICE);
  if (ret != 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to allocate memory");
    goto close_and_exit;
  }
  assert(p_buffer != NULL);
#endif
  for (int i = 0; i < SLICE / sizeof(int32_t); ++i) {
    int32_t r = odd_random();
    memcpy(p_buffer + sizeof(r) * i, &r, sizeof(r));
  }
  CALL_STDOUT_PRINTLN("Initialized memory convey from %s to %s,"
          " skip %ld TiB, total size %ld TiB, slice %ld MiB",
          INPUT_NAME, OUTPUT_NAME, (long)(OFFSET / T), (long)(TOTAL_SIZE / T), (long)(SLICE / M));

  /* check point 4: seek input file position */
  CHECK_EXIT_CODE_AND_LEAVE();

  if (CALL_LSEEK(ifd, OFFSET, SEEK_SET) < 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to read file %s", INPUT_NAME);
    goto close_and_exit;
  }
  for (int num_read = 0, num_written = 0; dot(&last_seen, last_read), last_read < TOTAL_SIZE;) {

    /* check point 5: do input file read operation */
    CHECK_EXIT_CODE_AND_LEAVE();

    assert(SLICE <= INT_MAX);
    num_read = CALL_READ(ifd, p_buffer, SLICE);
    if (num_read < 0) {
      CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to read file %s", INPUT_NAME);
      goto close_and_exit;
    }
    if (num_read == 0) {
      CALL_STDERR_PRINTLN("EOF");
      goto close_and_exit;
    }

    /* check point 6: do output file write operation */
    CHECK_EXIT_CODE_AND_LEAVE();

    assert(num_read <= SLICE);
    num_written = CALL_WRITE(ofd, p_buffer, num_read);
    if (num_written < 0) {
      CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to write file %s", OUTPUT_NAME);
      goto close_and_exit;
    }
    if (num_written != num_read) {
      CALL_STDERR_PRINTLN("Partial written %6d bytes, expected %6d bytes", num_written, num_read);
      goto close_and_exit;
    }
    last_read += num_read;
  }

  goto nice_clean_up;

flush_and_exit:
  fflush(stdout);

nice_clean_up:

  /* Free the slice buffer (appended to freelist) */
  if (p_buffer) {
#ifdef _WIN32
    _aligned_free(p_buffer);
#else
    free(p_buffer);
#endif
  }

  /* check point 7: flush output stream */
  CHECK_EXIT_CODE_AND_LEAVE();

#ifndef __linux__
  if (ofd != INVALID_HANDLE_VALUE && CALL_FLUSH(ofd) < 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to flush output file's buffer to disk %s", OUTPUT_NAME);
    goto close_and_exit;
  }
#endif

close_and_exit:
  if (ofd != INVALID_HANDLE_VALUE && CALL_CLOSE(ofd) < 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to close output file %s", OUTPUT_NAME);
  }
  if (ifd != INVALID_HANDLE_VALUE && CALL_CLOSE(ifd) < 0) {
    CALL_STDERR_PRINTLN_WITH_ERRORS("Failed to close input file %s", INPUT_NAME);
  }

  if (last_read == TOTAL_SIZE) {
    CALL_STDOUT_PRINTLN("Successfully written %3.4lf TiB", (double)TOTAL_SIZE/T);
  } else {
    CALL_STDERR_PRINTLN("Written %2.4lf TiB, expected %1.5lf TiB", (double)last_read/T, (double)TOTAL_SIZE/T);
    return -1;
  }
}
