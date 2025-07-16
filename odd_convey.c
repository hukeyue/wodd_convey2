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
#ifdef __linux__
#if __SIZE_WIDTH__ < 64
#define PREAD_FUNC(a, b, c, d) pread64(a, b, c, d)
#define PWRITE_FUNC(a, b, c, d) pwrite64(a, b, c, d)
#else
#define PREAD_FUNC(a, b, c, d) pread(a, b, c, d)
#define PWRITE_FUNC(a, b, c, d) pwrite(a, b, c, d)
#endif
#endif
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static const char input_name[] = "/proc/kcore";
// static const char input_name[] = "/dev/zero";
static const char output_name[] = "/dev/null";
static const long long K = 1ll << 10;
static const long long M = 1ll << 20;
static const long long G = 1ll << 30;
static const long long T = 1ll << 40;
static const loff_t offset = 12 * T;
static const loff_t size = 16 * T;
static const long slice = 8 * M;

int main() {
  loff_t last_read = 0;
  loff_t last_seen = 0;

  int ifd = open(input_name, O_RDONLY, 0);
  if (ifd < 0) {
    fprintf(stderr, "Failed to open file (input) %s: %s\n", input_name, strerror(errno));
    return -1;
  }
  int ofd = open(output_name, O_WRONLY, 0);
  if (ofd < 0) {
    fprintf(stderr, "Failed to open file (output) %s: %s\n", output_name, strerror(errno));
    return -1;
  }

  void* buffer = malloc(slice);
  if (!buffer) {
    fprintf(stderr, "No memory to allocate\n");
    return -1;
  }
  fprintf(stdout, "Initialized memory convey from %s to %s,"
          " skip %d TiB, total size %d TiB\n",
          input_name, output_name, (int)(offset / T), (int)(size / T));
  for (int read = 0, written = 0; last_read < size;) {
    assert(slice <= __INT_MAX__);
    read = PREAD_FUNC(ifd, buffer, slice, offset + last_read);
    if (read < 0) {
      if (errno == EAGAIN || errno == EINTR) {
        continue;
      }
      fprintf(stderr, "Failed to read file %s: %s\n", input_name, strerror(errno));
      fflush(stderr);
      break;
    }
    if (read == 0) {
      fprintf(stderr, "EOF\n");
      fflush(stderr);
      break;
    }
    assert(read <= slice);
    written = PWRITE_FUNC(ofd, buffer, read, last_read);
    if (written < 0) {
      if (errno == EAGAIN || errno == EINTR) {
        continue;
      }
      fprintf(stderr, "Failed to write file %s: %s\n", output_name, strerror(errno));
      fflush(stderr);
      break;
    }
    if (written != read) {
      fprintf(stderr, "Partial written %d bytes, expected %d bytes\n", written, read);
      fflush(stderr);
      break;
    }
    last_read += read;
    if (last_read - last_seen >= size / 100) {
      fprintf(stdout, "Progress %lld/100 percent\n", (long long)last_read * 100 / size);
      last_seen = last_read;
    }
  }

  free(buffer);
  close(ifd);
  close(ofd);

  if (last_read == size) {
    fprintf(stdout, "Successfully written %lf TiB\n", (double)size/T);
  } else {
    fprintf(stderr, "Written %lf TiB, expected %lf TiB\n", (double)last_read/T, (double)size/T);
    fflush(stderr);
    return -1;
  }
}
