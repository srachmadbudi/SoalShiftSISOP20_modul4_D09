#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

char *dir_path = "/home/boodboy/Documents";
char *log_path = "/home/boodboy/fs.log";
char caesar[100] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO9(ku@AW1[L";

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  char temp[1000];

  strcpy(temp, path);

  if(strncmp(path, "/encv1_", 6) == 0)
    decryptV1(temp);

  char fpath[1000];
  sprintf(fpath, "%s%s", dir_path, temp);

  res = lstat(fpath, stbuf);

  if(res == -1)
    return -errno;

  return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
  char tmp[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else
  {
    strcpy(tmp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(tmp);

    sprintf(fpath, "%s%s", dir_path, tmp);
  }

  int res = 0;

  DIR *dp;
  struct dirent *de;

  (void)offset;
  (void)fi;

  dp = opendir(fpath);

  if(dp == NULL)
    return -errno;

  while ((de = readdir(dp)) != NULL)
  {
    if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
      continue;

    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;

    char temp[1000];
    strcpy(temp, de->d_name);

    if (strncmp(path, "/encv1_", 6) == 0)
      encryptV1(temp);

    res = (filler(buf, temp, &st, 0));

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
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

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

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
  int res;

  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

  if (S_ISREG(mode)) 
  {
    res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
    if (res >= 0)
      res = close(res);
  } 
  else if (S_ISFIFO(mode))
    res = mkfifo(fpath, mode);
  else
    res = mknod(fpath, mode, rdev);

  if (res == -1)
    return -errno;

  char desc[100];
  sprintf(desc, "CREATE::%s", fpath);
  writeLog("INFO", desc);

  return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
  int res;

  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

  res = mkdir(fpath, mode);
  if (res == -1)
    return -errno;

  char desc[100];
  sprintf(desc, "MKDIR::%s", fpath);
  writeLog("INFO", desc);

  return 0;
}

static int xmp_unlink(const char *path)
{
  int res;

  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

  res = unlink(fpath);
  if (res == -1)
    return -errno;

  char desc[100];
  sprintf(desc, "REMOVE::%s", fpath);
  writeLog("WARNING", desc);
  
  return 0;
}

static int xmp_rmdir(const char *path)
{
  int res;

  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }
  res = rmdir(fpath);
  if (res == -1)
    return -errno;

  char desc[100];
  sprintf(desc, "RMDIR::%s", fpath);
  writeLog("WARNING", desc);

  return 0;
}

static int xmp_rename(const char *from, const char *to)
{
  int res;

  char ffrom[1000];
  if (strcmp(from, "/") == 0)
  {
    from = dir_path;
    sprintf(ffrom, "%s", from);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, from);

    if(strncmp(from, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(ffrom, "%s%s", dir_path, temp);
  }

  char fto[1000];
  if (strcmp(to, "/") == 0)
  {
    to = dir_path;
    sprintf(fto, "%s", to);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, to);

    if(strncmp(to, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fto, "%s%s", dir_path, temp);
  }

  res = rename(ffrom, fto);
  if (res == -1)
    return -errno;

  char desc[100];
  sprintf(desc, "RENAME::%s::%s", ffrom, fto);
  writeLog("INFO", desc);

  return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
  int res;

  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

  res = truncate(fpath, size);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
  int res;

  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

  res = open(fpath, fi->flags);
  if (res == -1)
    return -errno;

  close(res);
  return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];

  if (strcmp(path, "/") == 0)
  {
    path = dir_path;
    sprintf(fpath, "%s", path);
  }
  else 
  {
    char temp[1000];
    strcpy(temp, path);

    if(strncmp(path, "/encv1_", 6) == 0)
      decryptV1(temp);

    sprintf(fpath, "%s%s", dir_path, temp);
  }

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

  char desc[100];
  sprintf(desc, "WRITE::%s", fpath);
  writeLog("INFO", desc);

  return res;
}

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

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
}
