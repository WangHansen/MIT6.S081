#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void child_process(int lp[2])
{
  int prime;
  int buf;

  read(lp[0], &prime, 4);
  printf("prime %d\n", prime);

  close(lp[1]);

  if (read(lp[0], &buf, 4)) {
    int rp[2];
    pipe(rp);

    if (fork() == 0)
    {
      child_process(rp);
    }
    else
    {
      close(lp[1]);
      close(rp[0]);
      if (buf % prime)
      {
        write(rp[1], &buf, 4);
      }
      while (read(lp[0], &buf, 4) > 0)
      {
        if (buf % prime)
        {
          write(rp[1], &buf, 4);
        }
      }
      close(lp[0]);
      close(rp[1]);
      wait(0);
    }
  }
  exit(0);
}

int main(int argc, char *argv[])
{
  int lp[2];
  pipe(lp);

  int cpid = fork();
  if (cpid < 0) {
    printf("Failed to create child process\n");
    exit(1);
  }

  // child process
  if (cpid == 0) {
    child_process(lp);
  } else {
    close(lp[0]);
    for (int i = 2; i <= 35; i++) {
      if (write(lp[1], &i, 4) != 4)
      {
        printf("Failed to write %d into the pipe\n", i);
        exit(1);
      }
    }
    close(lp[1]);
    wait(0);
  }
  exit(0);
}