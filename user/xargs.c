#include "../kernel/types.h"
#include "../user/user.h"

int readline(char *buf) {
  while (read(0, buf, sizeof(char)) != 0) {
    if (*buf == '\n') {
      *buf = 0;
      return 1;
    }
    buf++;
  }
  return 0;
}

void xargs(int argc, char *argv[]) {
  char cmd[100];
  strcpy(cmd, argv[1]);

  for (int i = 1; i < argc; i++) {
    strcpy(argv[i - 1], argv[i]);
  }

  for (;;) {
    char arg[512];
    int cont = readline(arg);
    if (cont == 0)
      break;

    strcpy(argv[argc - 1], arg);
    if (fork() == 0) {
      exec(cmd, argv);
      return;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    exit(1);
  }

  xargs(argc, argv);

  wait(0);
  exit(0);
}
