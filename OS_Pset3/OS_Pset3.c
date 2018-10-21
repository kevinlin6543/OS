#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

char *buf, *token, *file, *iored, *scriptpath;
char execpath[PATH_MAX], cwd[PATH_MAX], c;
char *argv[100];
size_t bufsize = 4096;
ssize_t i;
int j, k, n, fd, pid, script, p;
struct rusage usage;
double usertime, systime;
FILE *fds;

int tokenize(char *tok, char *v[], char *buffer)
{
  p = 0;
  tok = strtok(buffer, " \n");
  while(tok)
  {
    v[p] = tok;
    tok = strtok(NULL, " \n");
    p++;
  }
  v[p] = NULL;
  return p;
}

int main()
{
  buf = (char *)malloc(bufsize * sizeof(char));
  if (buf == NULL)
  {
    fprintf(stderr, "Error: Unable to allocate memory for buffer using malloc. %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  while(1)
  {
    j = 0;
    if (!script)
    {
      printf("$ ");
      i = getline(&buf, &bufsize, stdin);
    }
    else if (script)
    {
      if ((i = getline(&buf, &bufsize, fds)) < 0)
        {
          script = 0;
          fclose(fds);
          continue;
        }
    }
    if(script || i > 0)
    {
      if(buf[0] == '#')
        continue;
      if (!script)
        buf[i-1] = '\0';

      j = tokenize(token, argv, buf);
      if (argv[0][0] == '.' && argv[0][1] == '/')
      {
        script = 1;
        scriptpath = argv[0]+2;
        fds = fopen(scriptpath, "r");
        continue;
      }

      if(!strcmp("exit", argv[0]))
        _exit(atoi(argv[1]));

      if (!strcmp("cd", argv[0]))
      {
        if(chdir(argv[1]) < 0)
          fprintf(stderr, "Error: Unable to change directories to %s. %s\n", argv[1], strerror(errno));
        continue;
      }

      i = snprintf(execpath, PATH_MAX, "/bin/%s", argv[0]);

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
              if ((fd = open(iored, O_RDONLY, 0666)) < 0)
                fprintf(stderr, "Error: Unable to open file, %s for writing. %s\n", file, strerror(errno));
              if (dup2(fd, 0) < 0)
                fprintf(stderr, "Error: Unable to duplicate standard in %s\n", strerror(errno));
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] != '>')
            {
              iored = argv[k];
              file = iored + 1;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              if (dup2(fd, 1) < 0)
                fprintf(stderr, "Error: Unable to duplicate standard out %s\n", strerror(errno));
              close(fd);
              break;
            }
            if(argv[k][0] == '2' && argv[k][1] == '>' && argv[k][2] != '>')
            {
              iored = argv[k];
              file = iored + 2;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              if (dup2(fd, 2) < 0)
                fprintf(stderr, "Error: Unable to duplicate standard error %s\n", strerror(errno));
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] == '>')
            {
              iored = argv[k];
              file = iored + 2;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              if (dup2(fd, 1) < 0)
                fprintf(stderr, "Error: Unable to duplicate standard out %s\n", strerror(errno));
              close(fd);
              break;
            }
            if(argv[k][0] == '2' && argv[k][1] == '>' && argv[k][2] == '>')
            {
              iored = argv[k];
              file = iored + 3;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              if (dup2(fd, 2) < 0)
                fprintf(stderr, "Error: Unable to duplicate standard error %s\n", strerror(errno));
              close(fd);
              break;
            }
          }
          argv[k] = NULL;
          execvp(execpath, argv);
          fprintf(stderr, "Error: Child process could not execute\n");
          exit(0);
          break;

        case -1:
          fprintf(stderr, "Error: Child process could not be created\n");
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
