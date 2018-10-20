#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

//char buf[4096];
char *buf;
//char *path = "/bin/";
char execpath[PATH_MAX];
char* token;
char *argv[100];
size_t bufsize = 4096;
ssize_t i;
int j,k,fd, pid;
char *file;
char *iored;
struct rusage usage;

int main()
{
  buf = (char *)malloc(bufsize * sizeof(char));
  while(1)
  {
    printf("$ ");
    j = 0;
    if((i = getline(&buf, &bufsize, stdin)) > 0)
    {
      if(buf[i-1] == '\n')
        buf[i-1] = '\0';
      token = strtok(buf, " ");
      while(token)
      {
        argv[j] = token;
        //fprintf(stderr, "argv[%d] = %s \n", j, argv[j]);
        token = strtok(NULL, " ");
        j++;
      }
      argv[j] = NULL;

      if(!strcmp("exit", argv[0]))
        _exit(0);

      if (!strcmp("cd", argv[0]))
      {
        //fprintf(stderr, "cd\n");
        chdir(argv[1]);
        continue;
      }

      i = snprintf(execpath, PATH_MAX, "/bin/%s", argv[0]);
      //fprintf(stderr, "%s\n", execpath);

      pid = fork();
      switch (pid)
      {
        case 0:
          for(k = 0; k < j; k++)
          {
            if(argv[k][0] == '<')
            {
              iored = argv[k];
              file = iored + 1;
              argv[k] = NULL;
              fd = open(iored, O_WRONLY, 0666);
              dup2(fd, 0);
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] != '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 1;
              fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
              dup2(fd, 1);
              close(fd);
              break;
            }
            if(argv[k][0] == '2' && argv[k][1] == '>' && argv[k][2] != '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 2;
              fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
              dup2(fd, 2);
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] == '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 2;
              fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666);
              dup2(fd, 1);
              close(fd);
              break;
            }
            if(argv[k][0] == '2' && argv[k][1] == '>' && argv[k][2] == '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 3;
              fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666);
              dup2(fd, 2);
              close(fd);
              break;
            }
          }
          execvp(execpath, argv);
          fprintf(stderr, "Warning: Child process could not execute\n");
          exit(0);
          break;

        case -1:
          fprintf(stderr, "Warning: Child process could not be created\n");
          break;

        default:
          wait3(NULL, 0, &usage);
          printf("Real time: %ld, User time: %ld, System Time: %ld\n", 1, 2, 3);
          break;
      }



    }
  }
  return 0;
}
