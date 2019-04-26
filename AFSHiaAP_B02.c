#define FUSE_USE_VERSION 31
#define HAVE_SETXATTR

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
// #include "pthread.h"

static const char *dirpath = "/home/rizk/shift4";
static const char worldlist[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
static const int KEYchiper = 17;
static const int LenPath = 1000;
static const char blOwner1[] = "chipset";
static const char blOwner2[] = "ic_controller";
static const char blGroup[] = "rusak";
static const char ytFolder[] = "/YOUTUBER/";

char Encrypt(char *s);
char Decrypt(char *s);

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

static int xmp_access(const char *path, int mask){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = access(fpath, mask);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = readlink(fpath, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi){
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		struct stat fstat;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		char fname[LenPath];
		sprintf(fname, "%s/%s", fpath, de->d_name);
		stat(fname, &fstat);

		struct passwd *euid = getpwuid(fstat.st_uid);
		struct group *egid = getgrgid(fstat.st_gid);
		struct tm *fntm = localtime(&fstat.st_atime);

		printf(">>>>>>>>>>>>>>>fname %s - %s; owner: %s; gr: %s \n", fname, de->d_name, euid->pw_name, egid->gr_name);
		printf("{%s} >>>>>>>stat: owner %s, group %s\n", de->d_name, euid->pw_name, egid->gr_name);
		if(strcmp(egid->gr_name, blGroup) == 0){
			if(strcmp(euid->pw_name, blOwner1) == 0 || strcmp(euid->pw_name, blOwner2) == 0){
				printf("'%s' HAPUS FILE INI CEPATTT!!!!!!!!!!!!!!\n", de->d_name);
				FILE *nf;
				char scfile[] = "filemiris.txt";
				Encrypt(scfile);
				char scpath[LenPath];
				sprintf(scpath, "%s/%s", dirpath, scfile);
				char wordtext[LenPath*2];
				sprintf(wordtext, "%s || %s - %s || %02d-%02d-%04d %02d:%02d:%02d\n", fname, euid->pw_name, egid->gr_name, fntm->tm_mon, fntm->tm_mday, fntm->tm_year + 1900, fntm->tm_hour, fntm->tm_min, fntm->tm_sec);
				nf = fopen(scpath, "a");
				fputs(wordtext, nf);
				fclose(nf);
				remove(fname);
			}
		}

    printf("\t\toldname: %s\n", de->d_name);
		if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") !=0){
			Decrypt(de->d_name);
		}
    printf("\t\tname: %s\n", de->d_name);
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
	int res;
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	printf(">>>>> BUAT FOLDER DISINI!! %s\n", fpath);
	printf(">>>>>>>>>>>>>>>>>> %s >>> %s >>> %d\n", path, ytFolder, mode);
	if(strstr(path, ytFolder) != NULL){
		mode = 0750;
		printf("ini folderYutub bro~ %d!!!!\n", mode);
	}

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to){
	int res;
	char ffrom[LenPath], enfrom[LenPath], fto[LenPath], ento[LenPath];

	strcpy(enfrom, from);
	Encrypt(enfrom);
	strcpy(ento, to);
	Encrypt(ento);

	sprintf(ffrom, "%s%s", dirpath, enfrom);
	sprintf(fto, "%s%s", dirpath, ento);

	res = symlink(ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to){
	int res;
	char ffrom[LenPath], enfrom[LenPath], fto[LenPath], ento[LenPath];

	strcpy(enfrom, from);
	Encrypt(enfrom);
	strcpy(ento, to);
	Encrypt(ento);

	sprintf(ffrom, "%s%s", dirpath, enfrom);
	sprintf(fto, "%s%s", dirpath, ento);

	res = rename(ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to){
	int res;
	char ffrom[LenPath], enfrom[LenPath], fto[LenPath], ento[LenPath];

	strcpy(enfrom, from);
	Encrypt(enfrom);
	strcpy(ento, to);
	Encrypt(ento);

	sprintf(ffrom, "%s%s", dirpath, enfrom);
	sprintf(fto, "%s%s", dirpath, ento);

	res = link(ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size){
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi){
	int fd;
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	(void) fi;

	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;
	struct stat fstat;
	stat(fpath, &fstat);
	struct passwd *euid = getpwuid(fstat.st_uid);
	struct group *egid = getgrgid(fstat.st_gid);

	printf(">>>>>>>stat: owner %s, group %s\n", euid->pw_name, egid->gr_name);
	if(strcmp(egid->gr_name, blGroup) == 0){
		if(strcmp(euid->pw_name, blOwner1) == 0 || strcmp(euid->pw_name, blOwner2) == 0){
			printf("HAPUS FILE INI CEPATTT!!!!!!!!!!!!!!\n");
		}
	}

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi){
	int fd;
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	printf("Masuksiniga?\n" );

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	res = statvfs(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);

	printf("7777777777777 CREATEFILE DISINI %s\n", fpath);
	printf(">>>>>>>>>>>>>>>>>> %s >>> %s >>> %d\n", path, ytFolder, mode);
	if(strstr(path, ytFolder) != NULL){
		mode = 0640;
		printf("FILEBERHASIL: %s -m %d\n", path, mode );
		// sprintf(enpath, "%s.iz1", path);
		strcat(enpath, ".iz1");
	}

	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	(void) fi;

	int res;
	res = creat(fpath, mode);
	if(res == -1)
		return -errno;
	close(res);
	return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */
	// char fpath[LenPath];
	//
	// if(strstr(path, ytFolder) != NULL){
 	// 	printf("FILEBERHASIL di RELEASE: %s\n", path );
 	// 	sprintf(fpath, "%s.iz1", path);
 	// 	printf("FILEBERHASIL di RELEASE: %s\n", fpath );
 	// }else
	// 	strcpy(fpath, path);

	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
	size_t size, int flags){

	char fpath[LenPath], enpath[LenPath];
	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	int res = lsetxattr(fpath, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
	size_t size){

	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	int res = lgetxattr(fpath, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size){

	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	int res = llistxattr(fpath, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name){

	char fpath[LenPath], enpath[LenPath];

	strcpy(enpath, path);
	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	int res = lremovexattr(fpath, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create = xmp_create,
	.release = xmp_release,
	.fsync = xmp_fsync,
	#ifdef HAVE_SETXATTR
		.setxattr	= xmp_setxattr,
		.getxattr	= xmp_getxattr,
		.listxattr	= xmp_listxattr,
		.removexattr	= xmp_removexattr,
	#endif
};

int main(int argc, char *argv[]){
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

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
