# SoalShift_modul4_B02

## Soal 1
Semua nama file dan folder harus terenkripsi

Enkripsi yang Atta inginkan sangat sederhana, yaitu `Caesar cipher`. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:

##### qE1~ YMUR2"'hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0

Misalkan ada file bernama `“halo”` di dalam folder `“INI_FOLDER”`, dan key yang dipakai adalah `17`, maka:

`“INI_FOLDER/halo”` saat belum di-mount maka akan bernama `“n,nsbZ]wio/QBE#”`, saat telah di-mount maka akan otomatis terdekripsi kembali menjadi `“INI_FOLDER/halo”` .

Perhatian: Karakter `‘/’ `adalah karakter ilegal dalam penamaan file atau folder dalam *NIX, maka dari itu dapat diabaikan

### Jawaban:
> Check : [Full SourceCode](https://github.com/rizanw/SoalShift_modul4_B02/blob/master/AFSHiaAP.c)
### Penjelasan :
1. Untuk mendeklarasikan custom alphabet 

```sh 
static const char worldlist[] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
```

2. Karena yang diminta di dalam soal key nya adalah `17`

```sh
static const int KEYchiper = 17;
```

3. Membuat fungsi untuk `decrypt Caesar Cipher`

```sh
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
```
##### Note :

Permintaan soal jika bertemu huruf `/` maka diabaikan 

```sh
if (s[i] == '/')
		continue;
```

Ketika fungsi `strchr` ini dipanggil, maka fungsi ini akan mengambil nilai yang disimpan oleh variabel cari (s[i]) dan menjadikannya sebagai acuan data yang ingin dicari. Kemudian, fungsi ini akan memulai mencari dan memeriksa apakah karakter tersebut terdapat dalam variabel kalimat. 

```sh
ptr = strchr(worldlist, s[i]);
```

Untuk mendapatkan `index ke-` dari suatu huruf

```sh
if(ptr){
						idx = ptr - worldlist - KEYchiper;
						if (idx < 0){
								idx = idx + strlen(worldlist);
						}
						s[i] = worldlist[idx];
				}
```
4. Membuat fungsi `encrypt Caesar Cipher`

```sh
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
```
#### Note
Mencari data yang akan dimasukkan kedalam array berdasarkan `index`,`key`,dan `panjang data`

```sh
if(ptr){
						idx = ptr - worldlist;
						s[i] = worldlist[(idx + KEYchiper) % strlen(worldlist)];
				}
 ```
 
 5. Untuk membandingkan dua buah string pada `de->d_name` menggunakan `strcmp` dan me-`decrypt` nama file
 
 ```sh
     printf("\t\toldname: %s\n", de->d_name);
		if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") !=0){
			Decrypt(de->d_name);
		}
```

6. Me-`encrypt` nama dari setiap path dengan menambahkan dalah setiap fungsi
```sh
strcpy(enpath, path);
Encrypt(enpath);
sprintf(fpath,"%s%s",dirpath,enpath);
```
## Soal 2
Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”

Urutan operasi dari kebutuhan ini adalah:

  a. Tepat saat sebelum file system di-mount
  
    i. Secara otomatis folder “Videos” terbuat di root directory file system
    
    ii. Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
    
    Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file. File pecahan tersebut dijamin terletak di root folder fuse

    iii. Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut

    iv. Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
    
    v. Jangan tampilkan file pecahan di direktori manapun
    
  b. Tepat saat file system akan di-unmount
  
    i. Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
    
    ii. Hapus folder “Videos” 
    
### Jawaban:
> Check : [Full SourceCode](https://github.com/rizanw/SoalShift_modul4_B02/blob/master/AFSHiaAP.c)
### Penjelasan :


## Soal 3
Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP_RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:

    - Owner Name 	: ‘chipset’ atau ‘ic_controller’
    - Group Name	: ‘rusak’
    - Tidak dapat dibaca
    
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file `“filemiris.txt”` (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.

### Jawaban:
> Check : [Full SourceCode](https://github.com/rizanw/SoalShift_modul4_B02/blob/master/AFSHiaAP.c)
### Penjelasan :


## Soal 4
Pada folder `YOUTUBER`, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah `“.iz1”`. File berekstensi `“.iz1”` tidak bisa diubah permissionnya dan memunculkan error bertuliskan `“File ekstensi iz1 tidak boleh diubah permissionnya.”`

### Jawaban:
> Check : [Full SourceCode](https://github.com/rizanw/SoalShift_modul4_B02/blob/master/AFSHiaAP.c)
### Penjelasan :


## Soal 5
Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama `Backup` kemudian hasil dari save tersebut akan disimpan pada backup dengan nama `namafile_[timestamp].ekstensi`. Dan ketika file asli dihapus, maka akan dibuat folder bernama `RecycleBin`, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama `namafile_deleted_[timestamp].zip` dan ditaruh kedalam folder RecycleBin (file asli dan backup terhapus). Dengan format `[timestamp]` adalah `yyyy-MM-dd_HH:mm:ss`

### Jawaban:
> Check : [Full SourceCode](https://github.com/rizanw/SoalShift_modul4_B02/blob/master/AFSHiaAP.c)
### Penjelasan :

