#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

static const char *dirpath = "/home/rizk/shift4";
static const char worldlist[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
static const int KEYchiper = 17;
static const int LenPath = 1000;

int getIndex(char c);
char Encrypt(char *s);
char Decrypt(char *s);
void DecryptDir();

static int xmp_getattr(const char *path, struct stat *stbuf){
  int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){

  char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);

	if(strcmp(enpath, "/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s", dirpath, enpath);

	int res = 0;
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

    printf("\t\toldname: %s\n", de->d_name);
		if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") !=0){
			Decrypt(de->d_name);
		}
    printf("\t\tname: %s\n", de->d_name);
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
  struct fuse_file_info *fi){
	int res = 0;
  int fd = 0 ;
  char fpath[LenPath];

	// if(strcmp(path,"/") == 0){
	// 	path=dirpath;
	// 	sprintf(fpath,"%s",path);
	// }
	// else sprintf(fpath, "%s%s",dirpath,path);

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
  .getattr = xmp_getattr,
  .readdir = xmp_readdir,
  .mkdir = xmp_mkdir,
  .read = xmp_read,
  // .create = xmp_create,
};


int main(int argc, char *argv[]){
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}

void DecryptDir(){
  DIR *dp;
  struct dirent *de;

  char fpath[1000];
  sprintf(fpath, "%s", dirpath);
  dp = opendir(fpath);
  while ((de = readdir(dp)) != NULL) {
    if(strcmp(de->d_name, ".") != 0 || strcmp(de->d_name, "..") != 0){
      rename(de->d_name, Decrypt(de->d_name));
      printf("%s\n", de->d_name);
    }
  }
  closedir(dp);
}

// int getIndex(char c){
// 	for(int i = 0;i < strlen(wordlist);i++){
// 		if(c == wordlist[i])
// 			return i;
// 	}
//   return -1;
// }

char Encrypt(char *s){
		int idx;
		char *ptr;

		for (int i = 0; i < strlen(s); i++){
				if (s[i] == '/')
					continue;

				ptr = strchr(worldlist, s[i]);
				if(ptr){
						idx = ptr - worldlist;
						s[i] = worldlist[(idx + KEYchiper) % strlen(worldlist)];
				}
		}
		return *s;
}

char Decrypt(char *s){
		int idx;
		char *ptr;

		for (int i = 0; i < strlen(s); i++){
				if (s[i] == '/')
					continue;

				ptr = strchr(worldlist, s[i]);
				if(ptr){
						idx = ptr - worldlist - KEYchiper;
						if (idx < 0){
								idx = idx + strlen(worldlist);
						}
						s[i] = worldlist[idx];
				}
		}
		return *s;
}
