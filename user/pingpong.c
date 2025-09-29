#include "../kernel/stat.h"
#include "../kernel/types.h"
#include "../user/user.h"

int main(int argc, char *argv[]) {
  int ptc[2];
  int ctp[2];
  char buf[1];

  pipe(ptc);
  pipe(ctp);
  int pid = fork();
  if (pid == 0) {
    // parent process
    write(ptc[1], "hello", 1);
    read(ctp[0], buf, 1);
    int ppid = getpid();
    printf("%d: received pong\n", ppid);
    exit(0);
  } else if (pid > 0) {
    // child process
    read(ptc[0], buf, 1);
    int cpid = getpid();
    printf("%d: received ping\n", cpid);
    write(ctp[1], "bye", 1);
    exit(0);
  } else {
    // fork error
    exit(1);
  }
}
