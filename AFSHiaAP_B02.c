/*
revisi:
no2:
- join isi file
- hide file asli
no5:
-semua
*/

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
#include <sys/stat.h>
#include  <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <pthread.h>
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
int getLastCharPos(char *str, char chr){
	char *posChar = NULL;
	char *tempPosChar = strchr(str, chr);

 	while(tempPosChar != NULL){
		posChar = tempPosChar;

 		tempPosChar = strchr(tempPosChar+1, chr);
	}
	if(posChar==NULL)
		return 0;

 	return (int) (posChar-str);
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

		// printf(">>>>>>>>>>>>>>>fname %s - %s; owner: %s; gr: %s \n", fname, de->d_name, euid->pw_name, egid->gr_name);
		// printf("{%s} >>>>>>>stat: owner %s, group %s\n", de->d_name, euid->pw_name, egid->gr_name);
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

    // printf("\t\toldname: %s\n", de->d_name);
		if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") !=0){
			Decrypt(de->d_name);
            if(strcmp(path, "/") == 0){
                char *p = strtok(de->d_name, ".");
                int filter = 0;
                while (p != NULL){
                    if (strcmp(p, "mp4") == 0 || strcmp(p, "mkv") == 0){
                        filter = 1;
                        break;
                    }else{
                        p = strtok(NULL, ".");
                    }
                }
                if (filter)
                    continue;
            }
		}
    // printf("\t\tname: %s\n", de->d_name);
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
	char fpath[1000], temp[1000];
	strcpy(temp, path);
	Encrypt(temp);

	if(strcmp(temp,"/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);

	int res, isFile, status;
	//-----------------------SOAL NO 5

	isFile = access(fpath, F_OK);

	//printf("====================UNLINK=======%s  ====  %s\n", path, fpath);
	if(isFile<0)				//JIKA BUKAN FILE
		return 0;
	printf("====================UNLINK=======%s  ====  %s\n", path, fpath);
	char backup[] = "Backup", pathBackup[1000];
	char command[1000], timestamp[100], namaFileZip[1000], namaFile[1000], ext[1000],
			namaFileWithoutExt[1000], pathNow[1000], namaRecycleBin[]="RecycleBin";
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(timestamp, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	strncpy(pathBackup, path, getLastCharPos(path, '/'));
	pathBackup[getLastCharPos(path, '/')] = '\0';
	sprintf(temp, "%s/%s", pathBackup, backup);
	strcpy(pathBackup, temp);
	//sprintf(temp, "%s%s", dirpath, pathBackup);
	
	//decrypt(path);
	int posSlash = getLastCharPos(path, '/');
	int posDot = getLastCharPos(path, '.');
	
	if (posDot==0)
		posDot = strlen(path);
	else{
		strcpy(ext, path+posDot);
		if (strcmp(ext, ".swp")==0)		//PREVENT .swp file to load
		{
			res = unlink(fpath);
			
			if (res == -1)
				return -errno;
			return 0;
		}
	}
	strcpy(namaFile, path+posSlash+1);
	strncpy(namaFileWithoutExt, path+posSlash+1, posDot-(posSlash+1));
	namaFileWithoutExt[posDot-(posSlash+1)] = '\0';

	strncpy(temp, path, getLastCharPos(path, '/'));
	temp[getLastCharPos(path, '/')] = '\0';
	sprintf(pathNow, "%s%s", dirpath, temp);


	sprintf(namaFileZip, "%s_deleted_%s.zip", namaFileWithoutExt, timestamp);
	Encrypt(namaFileZip);
	Encrypt(namaRecycleBin);
	Encrypt(namaFile);
	Encrypt(namaFileWithoutExt);
	sprintf(command, "cd %s && mkdir -p '%s' && zip '%s/%s' '%s' '%s/%s'* && rm -f '%s/%s'*", pathNow, namaRecycleBin,namaRecycleBin, namaFileZip, namaFile, backup, namaFileWithoutExt, backup, namaFileWithoutExt);

	printf("=================COMMAND=======%s\n", command);
	if (fork()==0){
		res = execl("/bin/sh","/bin/sh", "-c", command, NULL);	
        if (res == -1)
		    return -errno;
    }

	while((wait(&status))>0);
	//-----------------------SOAL NO 5

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
	char fpath[1000], temp[1000];
	
	if(strstr(path,"/YOUTUBER") != 0){
		strcat(path,".iz1");
	}
	strcpy(temp, path);	
	Encrypt(temp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,temp);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

	Encrypt(path);
	sprintf(temp, "%s/%s", dirpath,path);
	if(access(temp, R_OK)<0)				//JIKA FILE TIDAK ADA
		return res;

	printf("===============FILEPATH========%s\n", path);
	char backup[] = "Backup", pathBackup[1000];
	Encrypt(backup);
	strncpy(pathBackup, path, getLastCharPos(path, '/'));
	pathBackup[getLastCharPos(path, '/')] = '\0';
	sprintf(temp, "%s/%s", pathBackup, backup);
	strcpy(pathBackup, temp);
	sprintf(temp, "%s%s", dirpath, pathBackup);
	printf("==================PATH BACKUP ===========%s\n", pathBackup);
	mkdir(temp, 0777);


	Decrypt(path);
	char filePathWithoutExt[1000], ext[100], timestamp[1000], fileNameBackup[1000], ch;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(timestamp, "%04d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	int posSlash = getLastCharPos(path, '/');
	int posDot = getLastCharPos(path, '.');
	
	if (posDot==0) {
		posDot = strlen(path);
		ext[0] = '\0';
	}
	else{
		strcpy(ext, path+posDot);
		if (strcmp(ext, ".swp")==0)		//PREVENT .swp file to load
			return res;
	}
	strncpy(filePathWithoutExt, path+posSlash+1, posDot-(posSlash+1));
	filePathWithoutExt[posDot-(posSlash+1)] = '\0';
	
	sprintf(fileNameBackup,"%s_%s%s", filePathWithoutExt, timestamp, ext);
	printf("===============PATH========%s\n", path);
	printf("===============PATH========%d=====%d\n", posSlash, posDot);
	printf("===============FILEPATH EDIT=======%s\n", fileNameBackup);
	Encrypt(fileNameBackup);
	Encrypt(path);
	sprintf(temp, "%s%s", dirpath, path);
	printf("==========DIR SOURCE========%s\n", temp);
	FILE *source = fopen(temp, "r");

	sprintf(temp, "%s%s/%s", dirpath, pathBackup, fileNameBackup);
	printf("==========DIR TARGET========%s\n", temp);
	FILE *target = fopen(temp, "w");

	while ((ch = fgetc(source)) != EOF)
		fprintf(target, "%c", ch);

	fclose(target);
	fclose(source);
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
		chmod(tmp, 0640);
		remove(fpath);
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
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.mkdir		= xmp_mkdir,
	.create		= xmp_create,
	.chmod		= xmp_chmod,
	.utimens	= xmp_utimens,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.write		= xmp_write,
	.truncate	= xmp_truncate,
};

int main(int argc, char *argv[]){
	pthread_t tid2;
	pthread_create(&tid2, NULL, &vidJoiner, NULL);
	umask(0);

    fuse_main(argc, argv, &xmp_oper, NULL);
    
    pthread_join(tid2, NULL);

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
    char d[256];
    char folder[] = "Videos";
    Encrypt(folder);
    sprintf(d, "%s/%s", dirpath, folder);
    mkdir(d, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    struct dirent *de;

    DIR *dp = opendir(dirpath);
    if (dp != NULL){
        //Reading all video files in `dirpath` that have the same prefix
        struct vidFinfo v[10];
        int c = 0;
        while ((de = readdir(dp)) != NULL){
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;
            char *p;
            Decrypt(de->d_name);
            p = strtok(de->d_name, ".");
            char first[256];
            sprintf(first, "%s", p);
            while (p != NULL){
                if (strcmp(p, "mp4") == 0 || strcmp(p, "mkv") == 0){
                    char fname[1000];
                    sprintf(fname, "%s.%s", first, p);
                    int x = 0;
                    for (int i = 0; i < 10; i++){
                        if (strcmp(v[i].filename, fname) == 0){
                            v[i].max++;
                            x = 1;
                            break;
                        }
                    }
                    if (x == 0){
                        sprintf(v[c].filename, "%s", fname);
                        v[c].max = 0;
                        c++;
                    }
                    break;
                }
                else{
                    p = strtok(NULL, ".");
                }
            }
        }
        closedir(dp);

        for (int i = 0; i < 10; i++){
            if (strcmp(v[i].filename, "") == 0){
                continue;
            }
            else{
                char fname[1000];
                printf("v[atas]filename: %s\n", v[i].filename);
                char filename[1000];
                strcpy(filename, v[i].filename);
                Encrypt(filename);
                sprintf(fname, "%s/%s/%s", dirpath, folder, filename);
                printf("Merging file %s...\n", fname);
                FILE *p = fopen(fname, "w");
                if (p != NULL){
                    for (int j = 0; j <= v[i].max; j++){
                        char benc[500];
                        char num[100];
                        sprintf(num, "%03d", j);
                        Encrypt(num);
                        // sprintf(benc, "%s`%s", v[i].filename, num);
                        printf("v[]filename: %s\n", v[i].filename);
                        sprintf(benc, "%s.%03d", v[i].filename, j);
                        Encrypt(benc);
                        char sname[1000];
                        sprintf(sname, "%s/%s", dirpath, benc);
                        printf("  from file %s...\n", sname);
                        FILE *q = fopen(sname, "r");
                        if (q != NULL){
                            do{
                                int c = fgetc(q);
                                if (feof(q))
                                    break;
                                fputc(c, p);
                            } while (1);
                            fclose(q);
                        }
                        else
                        {
                            printf("q == NULL\n");
                            break;
                        }
                    }
                    fclose(p);
                }
                else{
                    printf(">>>>>>>>>>>>BUAT VIDEO GAGAL\n");
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
