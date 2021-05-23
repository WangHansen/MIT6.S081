#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    fprintf(2, "xargs: missing argument(s)\n");
    exit(1);
  }
  
  char *newArgs[MAXARG], buf[512], *p;

  // Set buffer to empty.
  p = buf;
  memset(buf, '\0', strlen(buf));

  int i = 0;
  // Copy arguments given in xargs.
  for (; i < argc - 1; i++)
  {
    newArgs[i] = malloc(strlen(argv[i + 1]));
    strcpy(newArgs[i], argv[i + 1]);
  }

  // Read from stdin byte by byte until the end.
  while (read(0, p, 1))
  {
    if (*p == ' ') {
      *p = '\0';
      newArgs[i] = malloc(strlen(buf));
      strcpy(newArgs[i++], buf);
      p = buf;
      memset(buf, '\0', strlen(buf));
    } else if (*p == '\n') {
      *p = '\0';
      newArgs[i] = malloc(strlen(buf));
      strcpy(newArgs[i++], buf);
      p = buf;
      memset(buf, '\0', strlen(buf));

      // Exec the command.
      if (fork() == 0) {
        exec(argv[1], newArgs);
      } else {
        wait(0);
      }
      
      // Clear new args.
      for (int j = argc - 1; j < MAXARG; j++)
      {
        memset(newArgs[j], '\0', strlen(newArgs[j]));
      }
      i = argc;
    } else {
      p++;
    }
  }
  exit(0);
}
