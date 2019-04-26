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


## Soal 2
Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”

Urutan operasi dari kebutuhan ini adalah:
