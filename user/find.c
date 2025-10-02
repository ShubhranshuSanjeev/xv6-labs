#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "../kernel/fs.h"
#include "../kernel/stat.h"
#include "../user/user.h"

/**

  - directory and filename is given
  - find the file in current directory if valid
  - if there are sub-directories, then recursively traverse them, repeat above
  step
*/

void find(char *dir, char *file) {
  int fd;
  char buf[512], *p;
  struct stat st;
  struct dirent de;

  // trying to open a directory, idk what this will give
  if ((fd = open(dir, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", dir);
    return;
  }

  // reading stats about the file descriptor
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  // whether the opened path is actually a directory
  if (st.type != T_DIR) {
    fprintf(2, "find: no such directory exsists %s\n", dir);
    close(fd);
    return;
  }

  // copying directory name to buffer - (self explainable, still adding a
  // comment)
  strcpy(buf, dir);
  p = buf + strlen(buf); // pointing to end of buf string
  *p++ = '/'; // putting '/' at the end of the string and moving pointer after '/'

  // trying to read the directory for all the dirent
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    // adding dirent name after '/'
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0; // adding null character at the end of the string
    if (stat(buf, &st) < 0) {
      printf("find: cannot stat %s\n", buf);
      continue;
    }

    switch (st.type) {
    case T_DEVICE:
    case T_FILE:
      if (strcmp(file, de.name) == 0) {
        printf("%s\n", buf);
      }
      break;
    case T_DIR:
      find(buf, file);
      break;
    }
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
