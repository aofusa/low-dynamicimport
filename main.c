#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const int BUF_SIZE = 1024;

int write_buffer(const char *path, char *buf, int limit) {
  int count = 0;
  int eof = 1;
  int fd = open(path, O_RDONLY);

  while (eof && count < limit) {
    eof = read(fd, (void *)(buf + count), 1);
    count++;
  }

  close(fd);

  return count;
}

void print_buffer(char *buf, int size) {
  int index;
  for (index = 0; index < size; index++) {
    printf("0x%02x ", buf[index] & 0x000000ff);
  }
}

void *allocate_buffer(int size) {
  /**
  * PROT_NONE 
  * PROT_READ
  * PROT_WRITE
  * PROT_EXEC  // EXEC属性を指定することで実行可能になる
  */
  int prot = PROT_READ|PROT_WRITE|PROT_EXEC;
  /**
  * MAP_PRIVATE
  * MAP_SHARED
  * MAP_ANONYMOUS  // 無名メモリを指定しファイルと関連付けしない
  */
  int flags = MAP_PRIVATE|MAP_ANONYMOUS;
  int page_size = sysconf(_SC_PAGE_SIZE);
  int map_size = (size / page_size + 1) * page_size;
  return mmap(NULL, map_size, prot, flags, -1, 0);
}

int free_buffer(void *addr, int size) {
  int page_size = sysconf(_SC_PAGE_SIZE);
  int map_size = (size / page_size + 1) * page_size;
  return munmap(addr, map_size);
}

int main(int argc, char *argv[]) {
  char buf[BUF_SIZE];
  char *mm;
  int size = 0;
  char *path;

  if (argc < 2) {
    printf("Usage: %s <object>\n", argv[0]);
    exit(0);
  }
  path = argv[1];

  // Stack
  memset(buf, '\0', BUF_SIZE);
  size = write_buffer(path, buf, BUF_SIZE);
  // print_buffer(buf, size);

  // Heap
  mm = allocate_buffer(BUF_SIZE);
  size = write_buffer(path, mm, BUF_SIZE);
  // print_buffer(mm, size);

  // Call dynamic import object
  // int (*f)(int, int) = (void*)(buf + 0x40);  // SIGSEGV
  int (*f)(int, int) = (void*)(mm + 0x40);  // 0x40 がelfのエントリポイント
  int n = f(1, 2);
  printf("%d\n", n);

  free_buffer(mm, BUF_SIZE);

  return 0;
}

