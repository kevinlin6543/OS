#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

size_t getFilesize(const char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

int main(int argc, char const *argv[])
{
  for (int i = 3; i <= argc; i++)
  {
    size_t filesize = getFilesize(argv[i]);
    int fd = open(argv[1], O_RDONLY);
    void* mmappedData = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    write(1, mmappedData, filesize)
    return 0;
  }
}
