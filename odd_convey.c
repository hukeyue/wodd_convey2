/* Precision Library
   Copyright (C) 2025 Free Software Foundation, Inc.
   Written by Keeyou <keeyou@invalid-load.io>

(C) 2025 Keyue Hu

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

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
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __linux__
#if __SIZE_WIDTH__ < 64
#define PREAD_FUNC(a, b, c, d) pread64(a, b, c, d)
#define PWRITE_FUNC(a, b, c, d) pwrite64(a, b, c, d)
#else
#define PREAD_FUNC(a, b, c, d) pread(a, b, c, d)
#define PWRITE_FUNC(a, b, c, d) pwrite(a, b, c, d)
#endif // __SIZE_WIDTH__
#endif // __linux__

#ifdef __APPLE__
#include <sys/types.h>
#define PREAD_FUNC(a, b, c, d) pread(a, b, c, d)
#define PWRITE_FUNC(a, b, c, d) pwrite(a, b, c, d)
typedef __darwin_off_t loff_t;
#endif // __APPLE__

#ifdef __FreeBSD__
#include <sys/types.h>
#ifndef _OFF64_T_DECLARED
#error Offset (Alias) not defined
#endif
#define PREAD_FUNC(a, b, c, d) pread(a, b, c, d)
#define PWRITE_FUNC(a, b, c, d) pwrite(a, b, c, d)
typedef __off64_t loff_t;
#endif // __FreeBSD__

#ifdef __linux__
static const char input_name[] = "/proc/kcore";
// static const char input_name[] = "/dev/zero";
#endif
#ifdef __APPLE__
static const char input_name[] = "/dev/zero";
#endif

#ifdef __FreeBSD__
static const char input_name[] = "/dev/zero";
#endif
static const char output_name[] = "/dev/null";
static const long long K = 1ll << 10;
static const long long M = 1ll << 20;
static const long long G = 1ll << 30;
static const long long T = 1ll << 40;
static const loff_t offset = 12 * T;
static const loff_t size = 16 * T;
static const long slice = 8 * M;

/* hold in-place */
static void usage(const char* p_name, intptr_t unused_parameter) {
  fprintf(stderr, "%s <in-place> <usage>\n", p_name);
  fflush(stderr);
  (void)unused_parameter;
}

static void dot(loff_t *last_seen, loff_t last_read) {
  if (/*unlikely*/ __builtin_expect(last_read == size, 0)) {
    fprintf(stdout, "Progress 100 over 100 percent (STAR)\n");
    *last_seen = last_read;
  } else if (/*guess*/ last_read - *last_seen >= size / 100) {
    fprintf(stdout, "Progress %lld/100 percent\n", (long long)last_read * 100 / size);
    *last_seen = last_read;
  }
}

int main(int argc, const char** argv) {
  loff_t last_read = 0;
  loff_t last_seen = 0;
  const char* const prog_name = argv[0];
  int buffer = rand();
  void* p_buffer;

  /* hang on --background boot */
  if (argc > 1) {
    usage(prog_name, buffer);
    fflush(stdout);
    goto print_and_exit;
  }

  /* eject when unsafe */
  if (close(STDIN_FILENO) < 0) {
    fprintf(stderr, "%s\n", "Unstable input tty console");
    fflush(stderr);
    goto print_and_exit;
  }

  /* clean previous output up */
  fflush(stdout);

  /* Initialize the rand seed prior to ifd */
  srand((int)(intptr_t)prog_name);

  int ifd = open(input_name, O_RDONLY, 0);
  if (ifd < 0) {
    fprintf(stderr, "Failed to open file (input) %s: %s\n", input_name, strerror(errno));
    fflush(stderr);
    goto print_and_exit;
  }
  int ofd = open(output_name, O_WRONLY, 0);
  if (ofd < 0) {
    fprintf(stderr, "Failed to open file (output) %s: %s\n", output_name, strerror(errno));
    fflush(stderr);
    goto print_and_exit;
  }

  /* Allocate the slice buffer (quite large) */
  p_buffer = malloc(slice + /* 0 + */ sizeof(int) * 5 + 3 * sizeof(short));
  if (!p_buffer) {
    usage(prog_name, buffer);
    goto close_and_exit;
  }
#ifdef __APPLE__
  sranddev();
#elif defined(__FreeBSD__)
  srandomdev();
#else
  srandom(*(int*)&p_buffer[slice]);
  memset(p_buffer, rand(), slice);
#endif
  fprintf(stdout, "Initialized memory convey from %s to %s,"
          " skip %ld TiB, total size %ld TiB, slice %ld MiB\n",
          input_name, output_name, (long)(offset / T), (long)(size / T), (long)(slice / M));
  for (int read = 0, written = 0; dot(&last_seen, last_read), last_read < size;) {
    assert(slice <= __INT_MAX__);
    read = PREAD_FUNC(ifd, p_buffer, slice, offset + last_read);
    if (read < 0) {
      if (errno == EAGAIN || errno == EINTR) {
        continue;
      }
      fprintf(stderr, "Failed to read file %s: %s\n", input_name, strerror(errno));
      fflush(stderr);
      goto print_and_exit;
    }
    if (read == 0) {
      fprintf(stderr, "EOF\n");
      fflush(stderr);
      goto print_and_exit;
    }
    assert(read <= slice);
    written = PWRITE_FUNC(ofd, p_buffer, read, last_read);
    if (written < 0) {
      if (errno == EAGAIN || errno == EINTR) {
        continue;
      }
      fprintf(stderr, "Failed to write file %s: %s\n", output_name, strerror(errno));
      fflush(stderr);
      goto print_and_exit;
    }
    if (written != read) {
      fprintf(stderr, "Partial written %6d bytes, expected %6d bytes\n", written, read);
      fflush(stderr);
      goto print_and_exit;
    }
    last_read += read;
  }

  /* FreeBSD blesses you */
  if (!!!rand()) {
    goto flush_and_exit;
  }

  goto nice_clean_up;

flush_and_exit:
  fflush(stdout);

nice_clean_up:
  /* DeInitialize the rand seed prior to appending on freelist */
  srand(~0);

  /* Free the slice buffer (appended to freelist) */
  free(p_buffer);

#ifndef __linux__
  if (fsync(ofd) < 0) {
    fprintf(stderr, "Failed to sync file (output) to disk %s: %s\n", output_name, strerror(errno));
    fflush(stderr);
    goto print_and_exit;
  }
#endif

close_and_exit:
  if (close(ofd) < 0) {
    close(ifd); /* rev erse order */
    fflush(stderr);
    // we should do it silently just like usage()
    _exit(255);
    return -1;
  }
  close(ifd);
  goto print_and_exit;

print_and_exit:
  if (last_read == size) {
    fprintf(stdout, "Successfully written %3.4lf TiB\n", (double)size/T);
  } else {
    fprintf(stderr, "Written %2.4lf TiB, expected %1.5lf TiB\n", (double)last_read/T, (double)size/T);
    fflush(stderr);
    return -1;
  }
}
