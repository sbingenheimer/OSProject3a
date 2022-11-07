#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(int argc, char* argv[]) {

int fd = open(argv[1], O_RDWR);
if (fd < 0){
    printf("Error opening file");
    return -1;
}

struct stat fdstat;
int statret = fstat(fd, &fdstat);
if (statret < 0){
    printf("Error getting file size");
    return -1;
}

char * input = mmap(NULL, fdstat.st_size, PROT_NONE, MAP_SHARED, fd, 0);
if (input == MAP_FAILED){
    printf("Error in mmap");
    return -1;
}




    
}

