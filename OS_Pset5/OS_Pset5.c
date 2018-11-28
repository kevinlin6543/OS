#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

char *s;
int fd;

int main(int argc, char const *argv[])
{
  if (argc < 4)
  {
    fprintf(stderr, "Error: not enough arguements. FORMAT: smear TARGET REPLACEMENT file1 {file2 ...}\n");
    exit(EXIT_FAILURE);
  }
  //char *r = argv[2];
  for (int i = 3; i < argc; i++)
  {
    struct stat st;
    if (stat(argv[i], &st) < 0)
    {
      fprintf(stderr, "Error: Unable to get stat for file, %s. Will skip file. %s\n", argv[i], strerror(errno));
      continue;
    }
    size_t filesize = st.st_size;
    if ((fd = open(argv[i], O_RDWR)) < 0)
    {
      fprintf(stderr, "Error: Unable to open file, %s, for read and write. Will skip file. %s\n", argv[i], strerror(errno));
      continue;
    }
    void* mmappedData = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
    if (mmappedData == MAP_FAILED)
    {
      fprintf(stderr, "Error: Unable to mmap file, %s, for read and write. Will skip file. %s\n", argv[i], strerror(errno));
      continue;
    }
    s = mmappedData;
    while((s = memmem(s, filesize, argv[1], strlen(argv[1]))) != NULL)
    {
      memcpy(s, argv[2], strlen(argv[2]));
      s++;
    }
    if (munmap(mmappedData, filesize) < 0)
    {
      fprintf(stderr, "Error: Unable to munmap file, %s. Will move onto next file %s\n", argv[i], strerror(errno));
      continue;
    }
    if (close(fd) < 0)
    {
      fprintf(stderr, "Error: Unable to close file, %s. Will move onto next file. %s\n", argv[i], strerror(errno));
      continue;
    }
  }
  return 0;
}
