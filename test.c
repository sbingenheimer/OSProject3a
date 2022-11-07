#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef unsigned int uint;

char usage[] =
    "Record Checker (binary files may differ but are still sorted correctly)\n\n\
usage: rcheck file1 file2\n\n\
positional arguments:\n\
  file1\t\trecord file\n\
  file2\t\tsorted record file\n";

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define MAG "\x001b[45;1m"
#define RESET "\x1B[0m"

typedef struct {
  int key;
  char *pointer;
} key_pointer;

void swap(key_pointer *a, key_pointer *b) {
  key_pointer t = *a;
  *a = *b;
  *b = t;
}

key_pointer *part(key_pointer *lower, key_pointer *upper) {
  key_pointer *l = lower;
  for (key_pointer *c = lower; c < upper; c++)
    if (c->key < upper->key)
      swap(c, l++);
  swap(l, upper);
  return l;
}

void qs(key_pointer *lower, key_pointer *upper) {
  if (lower < upper) {
    key_pointer *pivot = part(lower, upper);
    qs(lower, pivot - 1);
    qs(pivot + 1, upper);
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "%s", usage);
    exit(EXIT_FAILURE);
  }

  struct {
    int fd;
    char *map;
    char *fn;
  } rec, sort;
  rec.fn = argv[1];
  sort.fn = argv[2];

  if ((rec.fd = open(rec.fn, O_RDONLY)) == -1 ||
      (sort.fd = open(sort.fn, O_RDONLY)) == -1) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  struct stat st;
  stat(rec.fn, &st);
  uint recsize = st.st_size;
  stat(sort.fn, &st);
  uint sortsize = st.st_size;

  if (sortsize != recsize) {
    fprintf(stderr, "ERROR record file and sorted file don't have the same "
                    "number of records\n");
    exit(EXIT_FAILURE);
  }

  if ((rec.map = mmap(0, recsize, PROT_READ, MAP_SHARED, rec.fd, 0)) ==
          MAP_FAILED ||
      (sort.map = mmap(0, sortsize, PROT_READ, MAP_SHARED, sort.fd, 0)) ==
          MAP_FAILED)
    exit(EXIT_FAILURE);

  assert(sortsize == recsize);
  uint size = sortsize = recsize;
  size = size / 100;
  key_pointer *key_pointer_map =
      (key_pointer *)malloc(size * sizeof(key_pointer));
  key_pointer *c = key_pointer_map;

  for (char *r = rec.map; r < rec.map + size * 100; r += 100) {
    c->key = *(int *)r;
    c->pointer = r;
    c++;
  }

  qs(key_pointer_map, key_pointer_map + size - 1);

  for (uint c = 0; c < size * 100; c += 100)
    if (key_pointer_map[c / 100].key != *(int *)&sort.map[c])
      printf(MAG "[%08u]" RESET GRN "   %d" RESET " != " RED " %d\n" RESET,
             c / 100, key_pointer_map[c / 100].key, *(int *)&sort.map[c]);

  free(key_pointer_map);

  if (munmap(rec.map, size) == -1 || munmap(sort.map, size) == -1)
    exit(EXIT_FAILURE);

  close(sort.fd);
  close(rec.fd);

  exit(0);
}