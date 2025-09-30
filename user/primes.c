#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"

void primes(int) __attribute__((noreturn));
int check(int l, int num) { return num % l != 0; }

void sieve(int fd) {
  int buf, prime;
  read(fd, &buf, sizeof(buf));
  prime = buf;

  if (prime == 0) {
    return;
  }
  printf("prime %d\n", prime);

  int p[2];
  pipe(p);
  if (fork() == 0) {
    close(fd);
    close(p[1]);
    sieve(p[0]);
  } else {
    close(p[0]);
    while (1) {
      if (read(fd, &buf, sizeof(buf)) == 0 || buf == 0)
        break;
      if (check(prime, buf)) {
        write(p[1], &buf, sizeof(buf));
      }
    }
    close(fd);
    close(p[1]);
    wait(0);
  }
}

int main(int argc, char *argv[]) {
  int p[2];
  pipe(p);

  if (fork() == 0) {
    close(p[1]);
    sieve(p[0]);
  } else {
    close(p[0]);
    for (int i = 2; i <= 280; i++) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);
    wait(0);
  }
  exit(0);
}
