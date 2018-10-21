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

char *buf, *token, *file, *iored, *scriptpath;
char execpath[PATH_MAX], cwd[PATH_MAX], c;
char *argv[100];
size_t bufsize = 4096;
ssize_t i;
int j, k, n, fd, fds, pid, script, l, p;
struct rusage usage;
double usertime, systime;

int tokenize(char *tok, char *v[], char *buffer)
{
  p = 0;
  tok = strtok(buffer, " ");
  while(tok)
  {
    v[p] = tok;
    fprintf(stderr, "counter = %d; token: %s\n", p, tok);
    tok = strtok(NULL, " ");
    p++;
  }
  v[p] = NULL;
  return p;
}

int scriptread(int filed, char *buffer, int s, char **v, char *tok)
{
  k = 0;
  while ( (i = read(filed, &c, 1)) != 0)
  {
    if (c == '\0')
    {
      s = 0;
      break;
    }
    if (c == '\n')
    {
      buffer[k] = '\0';
      printf("buffer = %s\n", buffer);
      break;
    }
    buffer[k] = c;
    k++;
  }
  if (i < 0)
    fprintf(stderr, "Error: Unable to read file descriptor, %d. %s\n", filed, strerror(errno));
  tokenize(tok, v, buffer);
  return i;
}

int main()
{
  buf = (char *)malloc(bufsize * sizeof(char));
  if (buf == NULL)
  {
    fprintf(stderr, "Error: Unable to allocate memory for buffer using malloc. %s\n", strerror(errno));
    exit(EXIT_FAILURE); //Critical Error since all other parts of the shell can't run without a buffer.
  }
  while(1)
  {
    fprintf(stderr, "Loop!!!!\n");
    j = 0;
    if (!script)
    {
      printf("$ ");
      i = getline(&buf, &bufsize, stdin);
    }
    if(script || i > 0)
    {
      if(buf[0] == '#')
        continue;
      if (!script)
        buf[i-1] = '\0';

      j = tokenize(token, argv, buf);
      /*token = strtok(buf, " ");
      while(token)
      {
        argv[j] = token;
        fprintf(stderr, "token: %s\n", token);
        token = strtok(NULL, " ");
        j++;
      }
      argv[j] = NULL;
      */
      if(!script && argv[0][0] == '.' && argv[0][1] == '/')
      {
        script = 1;
        scriptpath = argv[0]+2;
        if ((fds = open(scriptpath, O_RDONLY)) < 0)
          fprintf(stderr, "Error: Unable to open file, %s, for reading. %s\n", scriptpath, strerror(errno));
        continue;
      }
      if(script)
      {
        if (scriptread(fds, buf, script, argv, token)==0)
        {
          script = 0;
          if (close(fds) < 0)
            fprintf(stderr, "Error: Unable to close File Descriptor, %d. %s\n", fds, strerror(errno));
        }
      }

      if(!strcmp("exit", argv[0]))
        _exit(atoi(argv[1]));

      if (!strcmp("cd", argv[0]))
      {
        if(chdir(argv[1]) < 0)
          fprintf(stderr, "Error: Unable to change directories to %s. %s\n", argv[1], strerror(errno));
        continue;
      }
      //printf("argv[0] = %s\n", argv[0]);
      i = snprintf(execpath, PATH_MAX, "/bin/%s", argv[0]);
      l = 0;
      if(argv[l] != NULL)
      {
        printf("argv[%d] = %s\n", l, argv[l]);
        l++;
      }

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
              if ((fd = open(iored, O_WRONLY, 0666)) < 0)
                fprintf(stderr, "Error: Unable to open file, %s for writing. %s\n", file, strerror(errno));
              dup2(fd, 0);
              close(fd);
              break;
            }
            if(argv[k][0] == '>' && argv[k][1] != '>')
            {
              iored = argv[k];
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
              file = iored + 3;
              if ((fd = open(file, O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
                fprintf(stderr, "Error: Unable to create or open file, %s for writing. %s\n", file, strerror(errno));
              dup2(fd, 2);
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
