#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct {
    int key;
    char *block;
} key_block;

struct thread_args {
    key_block* bucket;
    int count;
};


//compare function for c lib qsort
int compare(const void *first, const void *second){

    key_block *A = (key_block *)first;
    key_block *B = (key_block *)second;
    return(A->key - B->key);

}

//sort function to be called by multiple threads
key_block* sorti(key_block* bucketofKeys, int size){


    qsort(bucketofKeys, size , sizeof(key_block), compare);

    return bucketofKeys;
}

void* myThreadSort(void *args){
   
   struct thread_args *currargs = (struct thread_args*)args;
    sorti(currargs->bucket, currargs->count);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
char error_message[30] = "An error has occurred\n";
int fd = open(argv[1], O_RDWR);
if (fd < 0){
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(0);
}

struct stat fdstat;

int statret = stat(argv[1], &fdstat);
if (statret < 0){
    exit(0);
}

char * input = mmap(NULL, fdstat.st_size, PROT_READ || PROT_WRITE, MAP_SHARED, fd, 0);
if (input == MAP_FAILED){
    exit(0);
}
close(fd);

uint size = fdstat.st_size;

/*key_block *keyblockarr;
keyblockarr = malloc(size * sizeof(key_block));
key_block *keys = keyblockarr;*/

//CHANGED THIS UP!!!!!!!!
key_block keys[size/100];
int count = 0;

for (char* i = input; i < input + size; i += 100){
    keys[count].key = *(int*)i;
    keys[count].block = i;
    count++;
}


//set up buckets before threading, each int array holds an array keys
//each bucket holds values between each max value. THIS WORKS FOR CERTAIN
key_block bucket1keys[size/100] ;
key_block bucket2keys[size/100] ;
key_block bucket3keys[size/100] ;
key_block bucket4keys[size/100] ;

//max meaning bucket 1 holds values between -2147483648 and -2147483648/2
// bucket 2 holds -2147483648/2 to 0 exc.
long bucket1max = -2147483648/2;
long bucket2max = 0;
long bucket3max = 2147483648/2;
int count1, count2, count3, count4 = 0;
for (uint p = 0; p < size; p+=100){
    if (keys[p/100].key < bucket1max){
        bucket1keys[count1].key = keys[p/100].key;
        bucket1keys[count1].block = keys[p/100].block;
        count1++;
        continue;
    }
    if (keys[p/100].key < bucket2max){
        bucket2keys[count2].key = keys[p/100].key;
        bucket2keys[count2].block = keys[p/100].block;
        count2++;
        continue;
    }
    if (keys[p/100].key < bucket3max){
        bucket3keys[count3].key = keys[p/100].key;
        bucket3keys[count3].block = keys[p/100].block;
        count3++;
        continue;
    }
    
        bucket4keys[count4].key = keys[p/100].key;
        bucket4keys[count4].block = keys[p/100].block;
        count4++;
    
}

//Uncomment this if you want to see the buckets before sorting

/*
int i =0;
printf("Bucket 1: \n");
while(bucket1keys[i].key != NULL){
   printf("%d\n", bucket1keys[i].key);
   i++;
}
i=0;
printf("Bucket 2: \n");
while(bucket2keys[i].key != NULL){
   printf("%d\n", bucket2keys[i].key);
    i++;
}
i=0;
printf("Bucket 3: \n");
while(bucket3keys[i].key != NULL){
   printf("%d\n", bucket3keys[i].key);
    i++;
}
i=0;
printf("Bucket 4: \n");
while(bucket4keys[i].key != NULL){
   printf("%d\n", bucket4keys[i].key);
    i++;
}
*/

// This returns the num processors, which is how many threads we should use, and how many buckets
// For me this returned 4, which is why I set up 4 buckets, hopefully it does for you as wel
printf("This system has %d processors configured and "
            "%d processors available.\n",
            get_nprocs_conf(), get_nprocs());

//This is where threading should happen, right now I just have non-parallel calls
//to sort for proof that it works before threading

pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;


struct thread_args args1;
args1.bucket = bucket1keys;
args1.count = count1;
struct thread_args args2;
args2.bucket = bucket2keys;
args2.count = count2;
struct thread_args args3;
args3.bucket = bucket3keys;
args3.count = count3;
struct thread_args args4;
args4.bucket = bucket4keys;
args4.count = count4;



pthread_create(&thread1, NULL, myThreadSort, (void *)&args1);
pthread_create(&thread2, NULL, myThreadSort, (void *)&args2);
pthread_create(&thread3, NULL, myThreadSort, (void *)&args3);
pthread_create(&thread4, NULL, myThreadSort, (void *)&args4);

pthread_join(thread1, NULL);
pthread_join(thread2, NULL);
pthread_join(thread3, NULL);
pthread_join(thread4, NULL);







//We need these 4 functions threaded so they all happen at the same time
/*
sorti(bucket1keys, count1);
sorti(bucket2keys, count2);
sorti(bucket3keys, count3);
sorti(bucket4keys, count4);
*/

//Temporary output for sorting proof
/*
int i=0;
while(bucket1keys[i].key != '\0' && count1!=0){
   printf("%d\n", bucket1keys[i].key);
   i++;
}
i=0;
while(bucket2keys[i].key  != '\0' && count2!=0){
   printf("%d\n", bucket2keys[i].key);
   i++;
}
i=0;
while(bucket3keys[i].key != '\0' && count3!=0){
   printf("%d\n", bucket3keys[i].key);
   i++;
}
i=0;
while(bucket4keys[i].key != '\0' && count4!=0){
   printf("%d\n", bucket4keys[i].key);
   i++;
}
*/
//write. Assuming we sort the array of structs can change later if not
int fp = open(argv[2], O_RDWR | O_CREAT | S_IWUSR);
if (fp < 0) {
    exit(0);
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

int t = 0;
int y = 0;
while (bucket1keys[y].key != '\0' && count1 != 0){
    char * wri = bucket3keys[y].block;
    while(t != 100){
        int wr = write(fp, &wri+t, sizeof(wri));
        if (wr < 0){
            printf("errr");
            exit(0);
        }
        t++;
    }
    y++;
}

t = 0;
y = 0;
while (bucket2keys[y].key != '\0' && count2 != 0){
    char * wri = bucket2keys[y].block;
    while(t != 100){
        int wr = write(fp, &wri+t, sizeof(wri));
         if (wr < 0){
            printf("errr");
            exit(0);
        }
        t++;
    }
    y++;
}

t = 0;
y = 0;
while (bucket3keys[y].key != '\0' && count3 != 0){
    char * wri = bucket3keys[y].block;
    while(t != 100){
        int wr = write(fp, &wri+t, sizeof(wri));
         if (wr < 0){
            printf("errr");
            exit(0);
        }
        t++;
    }
    y++;
}
t = 0;
y = 0;
while (bucket4keys[y].key != '\0' && count4 != 0){
    char * wri = bucket4keys[y].block;
    while(t != 100){
        int wr = write(fp, &wri+t, sizeof(wri));
         if (wr < 0){
            printf("errr");
            exit(0);
        }
        t++;
    }
    y++;
}

fsync(fp);


}



