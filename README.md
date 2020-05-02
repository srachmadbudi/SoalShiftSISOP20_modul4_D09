# SoalShiftSISOP20_modul4_D09
```
Khofifah Nur Laela      05111840000025
Rachmad Budi Santoso    05111840000122
```
## 1. Enkripsi versi 1:
a. Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.<br/>
b. Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.<br/>
c. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.<br/>
d. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.<br/>
e. Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.<br/>
```
9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO
```
Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10
“encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg
Note : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.<br/>
f. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya. <br/>

### Jawaban :
- Program membaca nama directori dam mencocokkan awalan directori dengan string "encv1_", jika telah dilakukan rename directori tersebut merupakan directory baru. <br/>
- Jika awalan string cocok, maka untuk setiap file di dalam directori tersebut akan dienkripsi. Program akan melakukan rekursi untuk mengecek apakah di dalam directory yang dienkripsi terdapat directory, jika iya, maka program akan mengambil nama file atau directory dan mengenkripsi nama file atau directory tersebut. <br/>
- Program mengambil nama file dan mengenkripsi sebelum digunakan untuk nama file baru dengan menggunakan caesar cipher dengan key 10. <br/>
- Jika directory direname dengan menghilangkan string "encv1", maka semua file akan didekripsi. Semua operasi rename akan dicatat di dalam log dengan level info. <br/>
Terdapat fungsi yang dideklarasikan dengan fungsi `fuse_operations` :
```
static struct fuse_operations xmp_oper = {
  .getattr  = xmp_getattr,
  .readdir  = xmp_readdir,
  .mknod    = xmp_mknod,
  .mkdir    = xmp_mkdir,
  .unlink   = xmp_unlink,
  .rmdir    = xmp_rmdir,
  .rename   = xmp_rename,
  .truncate = xmp_truncate,
  .open     = xmp_open,
  .read     = xmp_read,
  .write    = xmp_write,
};
```
Fungsi FUSE di atas digunakan untuk menampilkan isi folder dari `"/home/boodboy/Documents"`, sehingga didefinisikan 
```
char *dir_path = "/home/boodboy/Documents";
```
Fungsi enkripsi untuk mengenkripsi nama directory "encv1_" :
```
void encryptV1(char *src) 
{
  int len = strlen(src);
  int start = 0;

  for (int i = strlen(src); i >= 0; i--) 
  {
    if(src[i] == '/')
      break;

    if(src[i] == '.')
    {
      len = i - 1;
      break;
    }
  }

  for (int i = 1; i < len; i++)
    if (src[i] == '/')
      start = i;

  for (int i = start; i <= len; i++) 
  {
    if(src[i] == '/')
      continue;

    int caesar_index = 0;
    while(1)
    {
      if(src[i] == caesar[caesar_index])
      {
        src[i] = caesar[caesar_index + 10];
        break;
      }
      caesar_index++;
    }
  }
}
```

Fungsi dekripsi untuk mendekripsi nama isi dari directory "encv1_" jika dilakukan rename directory menjadi bukan "encv1_" 
```
void decryptV1(char *src) 
{
  int len = strlen(src); 
  int start = 0;
    
  for (int i = 1; i < len; i++)
  {
    if(src[i] == '/' || src[i + 1] == '\0') 
    {
      start = i + 1;
      break;
    }
  }

  for (int i = strlen(src); i >= 0; i--)
  {
    if (src[i] == '.') 
    {
      len = i - 1;
      break;
    }
  }

  for (int i = start; i <= len; i++) 
  {
    if(src[i] == '/')
      continue;

    int caesar_index = strlen(caesar) - 1;
    while(1)
    {
      if(src[i] == caesar[caesar_index])
      {
        src[i] = caesar[caesar_index - 10];
        break;
      }
      caesar_index--;
    }
  }
}
```
## 2. Enkripsi versi 2:
a. Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2. <br/>
b. Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2. <br/>
c. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekrip. <br/>
d. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file. <br/>
e. Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil yakni: File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004. <br/>
f. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada didalam direktori tersebut (rekursif). <br/>
``` 
Kendala : tidak mengerjakan soal nomor 2. 
```

## 3. Sinkronisasi direktori otomatis:
Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:<br/>
a. Kedua directory memiliki parent directory yang sama. <br/>
b. Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi: <br/>
- Nama dari setiap berkas di dalamnya sama. <br/>
- Modified time dari setiap berkas di dalamnya tidak berselisih lebih dari 0.1 detik. <br/>
c. Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.<br/>
d. Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory.<br/>

Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.<br/>
Implementasi dilarang menggunakan symbolic links dan thread.<br/>
```
Kendala : tidak mengerjakan soal nomor 3.
```

## 3. Log system:
Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan. Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING. Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.Sisanya, akan dicatat dengan level INFO.<br/>
Format untuk logging yaitu:<br/>
```
[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]
```
LEVEL    : Level logging <br/>
yy       : Tahun dua digit <br/>
mm       : Bulan dua digit <br/>
dd       : Hari dua digit <br/>
HH       : Jam dua digit <br/>
MM       : Menit dua digit <br/>
SS       : Detik dua digit <br/>
CMD      : System call yang terpanggil <br/>
DESC     : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::) <br/>

Contoh format logging nantinya seperti:
```
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
```
### Jawaban :
Nama file untuk menyimpan log file :
```
char *log_path = "/home/boodboy/fs.log";
```
Fungsi writeLog digunakan untuk melakukan pencatatan setiap perintah system call yang telah dijalankan. 
```
void writeLog(char *level, char *cmd_desc)
{
  FILE * fp;
  fp = fopen (log_path, "a+");

  time_t rawtime = time(NULL);
  
  struct tm *info = localtime(&rawtime);
  
  char time[100];
  strftime(time, 100, "%y%m%d-%H:%M:%S", info);

  char log[100];
  sprintf(log, "%s::%s::%s\n", level, time, cmd_desc);
  fputs(log, fp);

  fclose(fp);
}

```
