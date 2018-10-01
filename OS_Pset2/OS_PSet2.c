#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <langinfo.h>

int i, removedSlash;
int firstFile = 1;
char buf[1024];
struct tm *mtim;

char fileType(char* fname, struct stat infoBuf)
{
  if (S_ISLNK(infoBuf.st_mode))
    return 'l';
  else if (S_ISDIR(infoBuf.st_mode))
    return 'd';
  else if (S_ISCHR(infoBuf.st_mode))
    return 'c';
  else if (S_ISBLK(infoBuf.st_mode))
    return 'b';
  else
    return '-';
}

void printFileInfo(char *filePath, struct stat infoBuf)
{
  char *sympath;
  i = lstat(filePath, &infoBuf);
  printf("  %lu  %*ld ", infoBuf.st_ino, 5, infoBuf.st_blocks/2);
  printf("%c", fileType(filePath, infoBuf));
  printf( (infoBuf.st_mode & S_IRUSR) ? "r" : "-");
  printf( (infoBuf.st_mode & S_IWUSR) ? "w" : "-");
  printf( (infoBuf.st_mode & S_IXUSR) ? "x" : "-");
  printf( (infoBuf.st_mode & S_IRGRP) ? "r" : "-");
  printf( (infoBuf.st_mode & S_IWGRP) ? "w" : "-");
  printf( (infoBuf.st_mode & S_IXGRP) ? "x" : "-");
  printf( (infoBuf.st_mode & S_IROTH) ? "r" : "-");
  printf( (infoBuf.st_mode & S_IWOTH) ? "w" : "-");
  printf( (infoBuf.st_mode & S_IXOTH) ? "x" : "-");
  printf("%*ld ", 4, infoBuf.st_nlink);
  printf(" %s", getgrgid(infoBuf.st_gid)->gr_name);
  printf(" %s", getpwuid(infoBuf.st_uid)->pw_name);
  printf("%*ld", 9, infoBuf.st_size);
  mtim = localtime(&infoBuf.st_mtime);
  strftime(buf, sizeof(buf)-1, "%b %d %H:%M", mtim);
  printf(" %s", buf);
  if (S_ISLNK(infoBuf.st_mode))
  {
    readlink(filePath, buf, sizeof(buf)-1);
    printf(" %s -> %s\n", filePath, buf);
  }
  else
    printf(" %s\n", filePath);
}

void listdir(char *name)
{
  DIR *dir;
  if (!(dir = opendir(name)))
  {
    fprintf(stderr, "Error: Cannot open directory '%s': %s\n", name, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    struct dirent *entry;
    entry = readdir(dir);
    const char *d_name;
    struct stat statbuf;

    if(firstFile)
    {
      char *firstfileName = malloc(strlen(name) + 2);
      strcpy(firstfileName, name);
      if (removedSlash)
        strcat(firstfileName, "/");
      printFileInfo(firstfileName, statbuf);
      firstFile = 0;
    }

    if(!entry)
      break;
    d_name = entry->d_name;

    if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0)
    {
      char *result = malloc(strlen(name) + strlen(d_name) + 2);
      strcpy(result, name);
      strcat(result, "/");
      strcat(result, d_name);
      printFileInfo(result, statbuf);
    }
    if (entry->d_type & DT_DIR)
    {
      if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0)
      {
        //int path_length;
        char path[PATH_MAX];
        /*path_length = */snprintf(path, PATH_MAX, "%s/%s", name, d_name);
        //if (path_length >= PATH_MAX)
      //  {
        //  fprintf(stderr, "Error: Path length is too long\n");
        //  exit(EXIT_FAILURE);
        //}
        listdir(path);
      }
    }
  }
}
int main(int argc, char const *argv[])
{
  char adjustedName[strlen(argv[1])];
  strcpy(adjustedName, argv[1]);
  if(adjustedName[strlen(adjustedName)-1] == '/')
  {
    adjustedName[strlen(adjustedName)-1] = 0;
    removedSlash = 1;
  }
  listdir(adjustedName);
  return 0;
}
