#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>

int i;

void listdir(const char *name)
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
    const char *d_name;
    entry = readdir(dir);

    if(!entry)
      break;

    d_name = entry->d_name;
    if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0)
    {
      char *result = malloc(strlen(name) + strlen(d_name) + 2);
      strcpy(result, name);
      strcat(result, "/");
      strcat(result, d_name);
      struct stat statbuf;

      i = lstat(result, &statbuf);

      printf("  %lu  %*ld ", statbuf.st_ino, 5, statbuf.st_blocks/2);
      printf( (S_ISDIR(statbuf.st_mode)) ? "d" : "-");
      printf( (statbuf.st_mode & S_IRUSR) ? "r" : "-");
      printf( (statbuf.st_mode & S_IWUSR) ? "w" : "-");
      printf( (statbuf.st_mode & S_IXUSR) ? "x" : "-");
      printf( (statbuf.st_mode & S_IRGRP) ? "r" : "-");
      printf( (statbuf.st_mode & S_IWGRP) ? "w" : "-");
      printf( (statbuf.st_mode & S_IXGRP) ? "x" : "-");
      printf( (statbuf.st_mode & S_IROTH) ? "r" : "-");
      printf( (statbuf.st_mode & S_IWOTH) ? "w" : "-");
      printf( (statbuf.st_mode & S_IXOTH) ? "x" : "-");
      printf("%*ld ", 4, statbuf.st_nlink);
      printf(" %s", getgrgid(statbuf.st_gid)->gr_name);
      printf(" %s", getpwuid(statbuf.st_uid)->pw_name);
      printf("%*ld", 9, statbuf.st_size);

      printf("     %s/%s\n", name, d_name);
    }

    if (entry->d_type & DT_DIR)
    {
      if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0)
      {
        int path_length;
        char path[PATH_MAX];

        path_length = snprintf(path, PATH_MAX, "%s/%s", name, d_name);
        if (path_length >= PATH_MAX)
        {
          fprintf(stderr, "Error: Path length is too long\n");
          exit(EXIT_FAILURE);
        }
        listdir(path);
      }
    }
  }
}
int main(int argc, char const *argv[])
{
  listdir("./OS");
  return 0;
}
