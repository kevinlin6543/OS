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

size_t getFileSize(const char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}
int main(int argc, char const *argv[])
{
  char *r = argv[2];
  for (int i = 3; i < argc; i++)
  {
    size_t filesize = getFileSize(argv[i]);
    int fd = open(argv[i], O_RDWR);
    void* mmappedData = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
    s = mmappedData;
    while((s = memmem(s, filesize, argv[1], strlen(argv[1]))) != NULL)
    {
      memcpy(s, r, strlen(r));
      s++;
    }
    munmap(mmappedData, filesize);
    close(fd);
  }
  return 0;
}
