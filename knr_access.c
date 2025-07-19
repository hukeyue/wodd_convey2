#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE
#endif // __linux__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

void usage(exec_name)
  const char* exec_name;
{
  fprintf(stderr, "%s Usage: access <file path>", exec_name);
  fflush(stderr);
  _exit(-2);
}

char* strerror_g(errnum)
  int errnum;
{
  static char knr_buffer[ 128 << 10 ];
  memset(knr_buffer, 0, sizeof(knr_buffer));
#ifdef __linux__
  return strerror_r(errnum, knr_buffer, sizeof(knr_buffer));
#endif
#ifdef __APPLE__
  int p = strerror_r(errnum, knr_buffer, sizeof(knr_buffer));
  assert(p == 0);
  return knr_buffer;
#endif
}

int main(argc, argv)
  int argc;
  const char** argv;
{
  int tmp, d;
  d = strlen(argv[1]);
  if (argc != 2 || d == 0) {
    usage(argv[0]);
  }

  tmp = access(argv[1], R_OK);
  (void)&tmp;
  switch(tmp) {
    case 0:
      fprintf(stdout, "OK '%s'\n", argv[1]);
      exit(0);
    break;
    case -1:
      fprintf(stderr, "Not feasible '%s' due to the fact that"
              " GNU extensions encountered ERROR: '%s'\n",
              argv[1], strerror_g(errno));
    break;
    default:
      fprintf(stderr, "Unknown error: '%s'\n", strerror(errno));
    break;
  }

  return -1;
}
