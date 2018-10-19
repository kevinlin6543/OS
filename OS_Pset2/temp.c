#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

int i, lastCharSlash, defFile, path_length;
int firstFile = 1;
char buf[1024];
struct tm mtim, cTime;
struct passwd *fuid;
struct group *fgid;
time_t curTime;

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
  curTime = time(0);
  localtime_r(&curTime, &cTime);
  char *sympath;
  if ((lstat(filePath, &infoBuf)) < 0)
  {
    fprintf(stderr, "Error: File, %s, stats could not be obtained. %s\n", filePath, strerror(errno));
    exit(EXIT_FAILURE);
  }
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
  if ((fuid = getpwuid(infoBuf.st_uid)) == NULL)
  {
    fprintf(stderr, "Error: UID could not be retrieved from file directory, %s. %s\n", filePath, strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf("%-8s", fuid->pw_name);
  if ((fgid = getgrgid(infoBuf.st_uid)) == NULL)
  {
    fprintf(stderr, "Error: GID could not be retrieved from file directory, %s. %s\n", filePath, strerror(errno));
    exit(EXIT_FAILURE);
  }
  printf(" %-8s", fgid->gr_name);
  printf("%*ld", 9, infoBuf.st_size);
  localtime_r(&infoBuf.st_mtime, &mtim);
  if ((cTime.tm_yday - mtim.tm_yday) > 30 || (cTime.tm_year - mtim.tm_year) >= 1)
    strftime(buf, sizeof(buf)-1, "%b %-2d  %Y", &mtim);
  else
    strftime(buf, sizeof(buf)-1, "%b %-2d %H:%M", &mtim);
  printf(" %s", buf);
  if (S_ISLNK(infoBuf.st_mode))
  {
    if ((readlink(filePath, buf, sizeof(buf)-1)) < 0)
    {
      fprintf(stderr, "Error: The symbolic link file, %s, could not be read for target location. %s\n", filePath, strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf(" %s -> %s\n", filePath, buf);
  }
  else
    printf(" %s\n", filePath);
}
void listdir(char *name)
{
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(name)))
  {
    fprintf(stderr, "Error: Directory, %s, could not be opened. %s\n", name, strerror(errno));
    exit(EXIT_FAILURE);
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (entry < 0)
    {
      fprintf(stderr, "Error: Directory could not be read. %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    const char *d_name = entry->d_name;
    char path[PATH_MAX];
    struct stat statbuf;
    if (firstFile)
    {
      if (defFile)
        printFileInfo("./", statbuf);
      else
        printFileInfo(name, statbuf);
      firstFile = 0;
      if (lastCharSlash)
        name[strlen(name)-1] = 0;
    }
    path_length = snprintf(path, PATH_MAX, "%s/%s", name, d_name);
    if (path_length >= PATH_MAX)
    {
      fprintf(stderr, "Error: Path length is too long\n");
      exit(EXIT_FAILURE);
    }
    if (entry->d_type & DT_REG)
      printFileInfo(path, statbuf);
    if (entry->d_type & DT_DIR)
    {
      if (strcmp(d_name, "..") == 0 || strcmp(d_name, ".") == 0)
        continue;
      printFileInfo(path, statbuf);
      listdir(path);
    }
  }
}
int main(int argc, char **argv)
{
  if (argc == 1)
  {
    char *def = ".";
    defFile = 1;
    listdir(def);
    return 0;
  }
  else if (argc > 2)
  {
    fprintf(stderr, "Error: Too many Arguements listed. Double check file directory name.\n");
    exit(EXIT_FAILURE);
  }
  if (argv[1][strlen(argv[1])-1] == '/')
    lastCharSlash = 1;
  listdir(argv[1]);
  return 0;
}
