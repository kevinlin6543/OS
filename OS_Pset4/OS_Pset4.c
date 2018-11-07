#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

char buf[1024];
int fd, numFiles, bytesRead, totalBytes;

void handler(int sig)
{
  fprintf(stderr, "Files read: %d\nBytes read: %d\n", numFiles, totalBytes);
  exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[])
{
  signal(SIGINT, handler);
  for (int i = 2; i < argc; i++)
  {
    int mPipe[2], grepPipe[2];
    pid_t mPID, grepPID;
    pipe2(mPipe, O_CLOEXEC);
    pipe2(grepPipe, O_CLOEXEC);
    switch (mPID = fork())
    {
      case 0:
      dup2(mPipe[0], STDIN_FILENO);
      execvp("more", NULL);
      default:
      switch (grepPID = fork())
      {
        case 0:
        dup2(grepPipe[0], STDIN_FILENO);
        dup2(mPipe[1], STDOUT_FILENO);
        execlp("grep", "grep", argv[1], NULL);
        break;
        default:
        fd = open(argv[i], O_RDONLY);
        numFiles++;
        close(grepPipe[0]);
        close(mPipe[0]);
        close(mPipe[1]);
        while ((bytesRead = read(fd, buf, 1024)) != 0)
        {
          write(grepPipe[1], buf, bytesRead);
          totalBytes += bytesRead;
        }
        close(fd);
        close(grepPipe[1]);
        wait(0);
        break;
      }
      wait(0);
      break;
    }
  }

  return 0;
}
