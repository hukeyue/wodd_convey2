#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>

void usage(const char* exec_name) {
  fprintf(stderr, "%s Usage: access <file path>\n", exec_name);
  fflush(stderr);
  _exit(-2);
}

int main(int argc, const char** argv) {
  int tmp;
  if (argc != 2) {
    usage(argv[0]);
  }
  if (strlen(argv[1]) == 0) {
    usage(argv[0]);
  }

  tmp = access(argv[1], R_OK);
  (void)&tmp;
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

  return -1;
}
