#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int p[2];
  pipe(p);

  int cpid = fork();

  if (cpid < 0)
  {
    fprintf(2, "Failed to create child process\n");
    exit(1);
  } else if (cpid == 0) {
    char buf;
    read(p[0], &buf, 1);
    fprintf(2, "%d: received ping\n", (int) getpid());
    close(p[0]);
    write(p[1], &buf, 1);
    close(p[1]);
    exit(0);
  } else {
    char buf;
    write(p[1], "a", 1);
    close(p[1]);
    wait(0);
    read(p[0], &buf, 1);
    fprintf(2, "%d: received pong\n", (int)getpid());
    close(p[0]);
    exit(0);
  }
}