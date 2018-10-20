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
char *token;
char cwd[PATH_MAX];
char *argv[100];
size_t bufsize = 4096;
ssize_t i;
int j,k, n, fd, pid;
char *file;
char *iored;
struct rusage usage;
double usertime, systime;

int main()
{
  buf = (char *)malloc(bufsize * sizeof(char));
  while(1)
  {
    //if (getcwd(cwd, sizeof(cwd)) != NULL)
    //  printf("$%s ", cwd);
    //else
    printf("$ ");
    j = 0;
    if((i = getline(&buf, &bufsize, stdin)) > 0)
    {
      if(buf[0] == '#')
        continue;
      //if(buf[i-1] == '\n')
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
              if ((fd = open(iored, O_WRONLY, 0666)) < 0)
                fprintf(stderr, "Error: Unable to open file, %s for writing. %s\n", file, strerror(errno));
              dup2(fd, 0);
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] != '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 1;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              dup2(fd, 1);
              close(fd);
              break;
            }
            if(argv[k][0] == '2' && argv[k][1] == '>' && argv[k][2] != '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 2;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              dup2(fd, 2);
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] == '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 2;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              dup2(fd, 1);
              close(fd);
              break;
            }
            if(argv[k][0] == '2' && argv[k][1] == '>' && argv[k][2] == '>')
            {
              iored = argv[k];
              argv[k] = NULL;
              file = iored + 3;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
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
          n = wait3(NULL, 0, &usage);
          usertime = (double)usage.ru_utime.tv_usec / 1000000;
          systime = (double)usage.ru_stime.tv_usec / 1000000;
          printf("Real time: %f, User time: %f, System Time: %f\n",usertime + systime, usertime, systime);
          break;
      }
    }
  }
  return 0;
}
