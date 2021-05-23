#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}

void find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  // If path is a file, then just check whether
  // it's name is the same as target.
  case T_FILE:
    if (strcmp(fmtname(path), target) == 0)
    {
      write(1, path, sizeof(path));
      write(1, "\n", 1);
    }
  // If path is a directory, then loop for each file under it
  // to check their names; for subdirectories, invoke find() again.
  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    // Put a slash at the end of buf.
    p = buf + strlen(buf);
    *p++ = '/';
    // Loop for each entry under current directory;
    // note that "a directory is a sequence of dirent struct"(see fs.h).
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      // Omit '.' and '..'
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      // Construct complete path to current file.
      memmove(p, de.name, DIRSIZ); // memcpy is equal to memmove here(see ulib.c).
      p[DIRSIZ] = '\0';
      if (strcmp(de.name, target) == 0)
      {
        printf("%s\n", buf);
      }
      if (stat(buf, &st) < 0)
      {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      // Recurse for subdirectories
      if (st.type == T_DIR)
      {
        find(buf, target);
      }
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if(argc != 3) {
    printf("Usage: find [path] [name]\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}