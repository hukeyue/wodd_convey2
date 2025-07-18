#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void usage(exec_name)
  const char* exec_name;
{
  fprintf(stderr, "%s Usage: access <file path>", exec_name);
  fflush(stderr);
  _exit(-2);
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
      fprintf(stderr, "Not accessible '%s'\n", argv[1]);
    break;
    default:
      fprintf(stderr, "Unknown error: '%d'\n", errno);
    break;
  }

  return -1;
}
