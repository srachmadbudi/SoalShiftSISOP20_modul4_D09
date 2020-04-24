#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/boodboy/Documents";

//karakter untuk enkripsi
char list[100] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

//fungsi enkripsi, encrypt adalah string yg akan dienkripsi
void enc(char *encrypt)
{

    if (!strcmp(encrypt, ".") || !strcmp(encrypt, ".."))
        return;

    for (int i = 0; i < strlen(encrypt); i++)
    {
        for (int j = 0; j < strlen(list); j++)
        {
            if (encrypt[i] == list[j])
            {
                encrypt[i] = list[(j + 10) % strlen(list)];
                break;
            }
        }
    }
    printf("Hasil enkripsi : %s\n", encrypt);
}

//fungsi dekripsi, decrypt adalah string yg akan didekripsi
void dec(char *decrypt)
{

    if (!strcmp(decrypt, ".") || !strcmp(decrypt, ".."))
        return;

    for (int i = 0; i < strlen(decrypt); i++)
    {
        for (int j = 0; j < strlen(list); j++)
        {
            if (decrypt[i] == list[j])
            {
                decrypt[i] = list[(strlen(list) - 10) % strlen(list)];
                break;
            }
        }
    }
    printf("Hasil dekripsi : %s\n", decrypt);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{

    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", dirpath, path);
    int res = 0;
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0)
            break;
    }
    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{

    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", dirpath, path);
    int res = 0;
    int fd = 0;
    (void)fi;
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
    .read = xmp_read,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
