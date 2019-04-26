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
#include <stdlib.h>
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
#include <pthread.h>
#include  <sys/types.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static const char *dirpath = "/home/rizk/shift4";
static const char worldlist[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
static const int KEYchiper = 17;
static const int LenPath = 1000;
static const char blOwner1[] = "chipset";
static const char blOwner2[] = "ic_controller";
static const char blGroup[] = "rusak";
static const char ytFolder[] = "/YOUTUBER/";

//var global soal4
int soal4v = 0;
char *soalsc;
char *soaldt;

struct vidFinfo{
	int max;
	char filename[256];
};

char Encrypt(char *s);
char Decrypt(char *s);
void *vidJoiner(void *argv);
int rmDir(const char *path);
void *soal4(){
	printf("VAR-SOAL4>>>> %d\n", soal4v);
	if(soal4v == 1){
		char *argv[4] = {"zenity", "--warning", "--text='File ekstensi iz1 tidak boleh diubah permissionnya.'", NULL};
		execv("/usr/bin/zenity", argv);
		printf("=============File ekstensi iz1 tidak boleh diubah permissionnya.\n");
		sleep(10);
		// char *argv[] = {"cp", soalsc, soaldt, NULL};
		// execv("/bin/cp", argv);
		soal4v = 0;
	}
}

void cpFile(char *sc, char *dt);

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

	printf("ADA FILE TERHAPUS!!?\n" );
	char RBFolder[] = "RecycleBin";
	Encrypt(RBFolder);
	char RBPath[LenPath];
	sprintf(RBPath, "%s/%s", dirpath, RBFolder);
	mkdir(RBPath, 0750);

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

	printf("%s | CHMOD >>>>>>>>>>>> %s\n", fpath, path);
	int len = strlen(fpath);
	// fpath[len-1] == '1' && fpath[len-2] == 'z' && fpath[len-3] == 'i' && fpath[len-4] == '.'
	if(strstr(fpath, ytFolder) == 0){
	printf("JANGAN UBAH FOLDER YUTUB BOY!!\n");
		printf("%d %d %d %d\n", fpath[len-1], fpath[len-2], fpath[len-3], fpath[len-4]);
		if(fpath[len-1] == 37 && fpath[len-2] == 83 && fpath[len-3] == 91 && fpath[len-4] == 96){
			// soal4v = 1;
			pid_t fk;
			fk = fork();
			if(fk == 0){
				char *argv[4] = {"zenity", "--warning", "--text=File ekstensi iz1 tidak boleh diubah permissionnya.", NULL};
				execv("/usr/bin/zenity", argv);
			}
		}
	}

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

	printf("ADA FILE TEREDIT!!?\n" );
	char BakFolder[] = "Backup";
	Encrypt(BakFolder);
	char BakPath[LenPath];
	sprintf(BakPath, "%s/%s", dirpath, BakFolder);
	mkdir(BakPath, 0750);



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
	char fpath[LenPath], enpath[LenPath], tmp[LenPath];

	strcpy(enpath, path);

	printf("7777777777777 CREATEFILE DISINI %s\n", fpath);
	printf(">>>>>>>>>>>>>>>>>> %s >>> %s >>> %d\n", path, ytFolder, mode);
	if(strstr(path, ytFolder) != NULL){
		mode = 0640;
		printf("FILEBERHASIL: %s -m %d\n", path, mode );
		// sprintf(enpath, "%s.iz1", path);
		// strcat(enpath, ".iz1");
	}

	Encrypt(enpath);
	sprintf(fpath,"%s%s",dirpath,enpath);

	(void) fi;

	int res;
	res = creat(fpath, mode);
	if(res == -1)
		return -errno;
	close(res);

	if(strstr(path, ytFolder) != NULL){
		strcpy(tmp, fpath);
		char extiz1[] = ".iz1";
		Encrypt(extiz1);
		strcat(tmp, extiz1);
		char cmd[LenPath*2];
		sprintf(cmd, "%s %s", fpath, tmp);
		printf("\t\tCMD>>>>>>>> %s\n", cmd);
		//
		// strcpy(soalsc, fpath);
		// strcpy(soaldt, tmp);
		cpFile(fpath, tmp);
		remove(fpath);
		chmod(tmp, 0640);
		// soal4v = 1;
		// char *argv[] = {"cp", fpath, tmp, NULL};
		// execv("/bin/cp", argv);
		// execlp("cp", "cp", cmd, NULL);
	}

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
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, soal4, NULL);
	pthread_create(&tid2, NULL, vidJoiner, NULL);
	umask(0);
	fuse_main(argc, argv, &xmp_oper, NULL);

	char vidDir[256];
	char vidFolder[] = "Videos";
	Encrypt(vidFolder);
	sprintf(vidDir, "%s/%s", dirpath, vidFolder);
	rmDir(vidDir);
	return 0;
}

//Fungsi remove ini diambil dari https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c/42978529
int rmDir(const char *path){
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;
   if (d){
      struct dirent *p;
      r = 0;
      while (!r && (p=readdir(d))){
          int r2 = -1;
          char *buf;
          size_t len;
          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")){
             continue;
          }
          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf){
             struct stat statbuf;
             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)){
                if (S_ISDIR(statbuf.st_mode)){
                   r2 = rmDir(buf);
                }else{
                   r2 = unlink(buf);
                }
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }
   if (!r){
      r = rmdir(path);
   }
   return r;
}

void *vidJoiner(void *argv){
		char vidFolder[] = "Videos";
		Encrypt(vidFolder);
    char dst[256];
		sprintf(dst, "%s/%s", dirpath, vidFolder);

    mkdir(dst, 0750);

		struct dirent *de;
    DIR *dp = opendir(dirpath);
    if (dp != NULL){
        struct vidFinfo vids[100];
        int counter = 0;

        while ((de = readdir(dp)) != NULL){
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;

						char viName[256];
						strcpy(viName, de->d_name);
						Decrypt(viName);

						char *viTok;
            viTok = strtok(viName, ".");

            char Tok1[256];
            sprintf(Tok1, "%s", viTok);

            while (viTok != NULL){
                if (strcmp(viTok, "mkv") == 0 || strcmp(viTok, "mp4") == 0 || strcmp(viTok, "flv") == 0){
                    char fname[LenPath];
                    sprintf(fname, "%s.%s", Tok1, viTok);

                    int x = 0;
                    for (int i = 0; i < 10; i++){
                        if (strcmp(vids[i].filename, fname) == 0){
                            vids[i].max++;
                            x = 1;
                            break;
                        }
                    }

										if (x == 0){
                        sprintf(vids[counter].filename, "%s", fname);
                        vids[counter].max = 0;
                        counter++;
                    }

                    break;
                }else
									viTok = strtok(NULL, ".");
            }
        }
        closedir(dp);

        for (int i = 0; i < 10; i++){
            if (strcmp(vids[i].filename, "") == 0){
                continue;
            }else{
								char *vidName = vids[i].filename;
								Encrypt(vidName);

								char fname[LenPath];
								sprintf(fname, "%s/%s/%s", dirpath, vidFolder, vidName);

                FILE *nvid = fopen(fname, "w");
                if (nvid != NULL){
                    for (int j = 0; j <= vids[i].max; j++){

                        char benc[LenPath];
                        sprintf(benc, "%s.%03d", vids[i].filename, j);
												Encrypt(benc);

                        char sname[LenPath];
                        sprintf(sname, "%s/%s", dirpath, benc);

                        FILE *rvid = fopen(sname, "r");
                        if (rvid != NULL){
                            do{
                                int c = fgetc(rvid);
                                if (feof(rvid))
                                    break;
                                fputc(c, nvid);
                            } while (1);
                            fclose(rvid);
                        }else
                            break;
                    }
                    fclose(nvid);
                }
            }
        }
    }
    return 0;
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

//fungsi cp ini diambil dari https://www.geeksforgeeks.org/c-program-copy-contents-one-file-another-file/
void cpFile(char *sc, char *dt){
    FILE *fptr1, *fptr2;
    char c;

    // Open one file for reading
    fptr1 = fopen(sc, "r");
    if (fptr1 == NULL){
        printf("Cannot open file %s \n", sc);
        exit(0);
    }

    // Open another file for writing
    fptr2 = fopen(dt, "w");
    if (fptr2 == NULL){
        printf("Cannot open file %s \n", dt);
        exit(0);
    }

    // Read contents from file
    c = fgetc(fptr1);
    while (c != EOF){
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }

    printf("\nContents copied to %s", dt);

    fclose(fptr1);
    fclose(fptr2);
}
