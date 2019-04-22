#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home";

static struct fuse_operations xmp_oper = {
  .getattr = xmp_getattr,
  .readdir = xmp_readdir,
  .read = xmp_read,
};

int int main(int argc, char const *argv[]) {
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}
