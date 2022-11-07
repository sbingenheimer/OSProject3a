#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    int key;
    char *block;
} key_block;


int compare(const void *first, const void *second){

    return(*(int*)first - *(int*)second);

}

int main(int argc, char* argv[]) {

int fd = open(argv[1], O_RDWR);
if (fd < 0){
    printf("Error opening file");
    exit(1);
}

struct stat fdstat;
/*int statret = fstat(fd, &fdstat);
if (statret < 0){
    printf("Error getting file size");
    return -1;
}*/

int statret = stat(argv[1], &fdstat);
if (statret < 0){
    printf("Error getting file size");
    exit(1);
}

char * input = mmap(NULL, fdstat.st_size, PROT_READ || PROT_WRITE, MAP_SHARED, fd, 0);
if (input == MAP_FAILED){
    printf("Error in mmap");
    exit(1);
}
close(fd);

uint size = fdstat.st_size;

key_block *keyblockarr = malloc(size * sizeof(key_block));
key_block *keys = keyblockarr;

for (char* i = input; i < input + size; i += 100){
    keys->key = *(int*)i;
    keys->block = i;
    keys++;
}

//right now we have an array of structs keyblockarr, wasn't working with qs figured doesnt matter were changing it.
//qsort(keyblockarr->key, (sizeof(keyblockarr) / sizeof(keyblockarr[0].key)) , sizeof(int), compare);

for (uint p = 0; p < size; p+=100){
    printf("%d ", keyblockarr[p/100].key);
}

free(keyblockarr);

//write. Assuming we sort the array of structs can change later if not


int fp = open(argv[2], O_CREAT | O_RDWR );
if (fp < 0) {
    printf("error opening output");
    exit(1);
}

//tried a bunch of shit getting really weird seg faults. Should be a quick fix general format is done.
/*
for (uint q = 0; q < size; q+=100){
    char * nxt = keyblockarr[q/100].block;
        int wr = write(fp, &nxt, sizeof(nxt));
        if (wr < 0){
            printf("err");
            exit(1);
        }
    
    }
*/

close(fp);




}

