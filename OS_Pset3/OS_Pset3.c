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

//char buf[4096];
char *buf;
char* token;
char *argv[100];
size_t bufsize = 4096;
ssize_t i;
int j;

int main()
{
  buf = (char *)malloc(bufsize * sizeof(char));
  while(1)
  {
    j = 0;
    if((i = getline(&buf, &bufsize, stdin)) > 0)
    {
      buf[i] = '\0';
      token = strtok(buf, " ");
      while(token)
      {
        argv[j] = token;
        fprintf(stderr, "argv[%d] = %s \n", j, argv[j]);
        j++;
        token = strtok(NULL, " ");
      }
      argv[j] = NULL;

      if (!strcmp("cd", argv[0]))
        fprintf(stderr, "cd\n");
    }
  }
  return 0;
}
