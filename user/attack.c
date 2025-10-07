#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "../user/user.h"
#include "../kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // got this address by memory inspection -- not sure how this value came
  char *secret = sbrk(PGSIZE*9);
  secret = secret + 8 * PGSIZE + 16;

  write(2, secret, 8);
}
