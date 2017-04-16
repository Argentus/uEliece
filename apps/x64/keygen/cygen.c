#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define X 9857
#define w0 71
#define bytes (X / 32) + 1

#define SetBit(P,i)     ( (P)[( (i) / 32)] |=  (1 << ( (i) % 32)) )
#define ClearBit(P,i)   ( (P)[( (i) / 32)] &= ~(1 << ( (i) % 32)) )
#define TestBit(P,i)  !!( (P)[( (i) / 32)] &   (1 << ( (i) % 32)) )

int count = 0;


void printArray(int arr[], int size) {
    int i;

    for(i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    putc('\n', stdout);
}

void generateIndexes(int indexes[]) {
    int i, num;
    unsigned int mask[bytes] = {0};

    for(i = 0; i < w0; i++) {
        do {
            num = rand() % X;
        } while(TestBit(mask, num));
        SetBit(mask, num);
        indexes[i] = num;
    }
}

void createPolynomial(int indexes[], int arr[]) {
    int i;

    for(i = 0; i < w0; i++) {
        SetBit(arr, indexes[i]);
    }
}

void freeMemory(int **euclid, int size) {
	int i;

    for (i = 0; i < size; i++)
        free(euclid[i]);
    free(euclid);
}

void allocMemory(int ***euclid) {
    static int max = 0;

    if (count == max) {
        int newmax = (max + 2) * 2;   /* 4, 12, 28, 60, ... */
        int **newptr = (int **)realloc(*euclid, newmax * sizeof(*euclid));

        if (newptr == NULL) {
            freeMemory(*euclid, count);
            exit(EXIT_FAILURE);
        }
        max = newmax;
        *euclid = newptr;
    }

    (*euclid)[count] = (int *)calloc(bytes, sizeof(int));

    if ((*euclid)[count] == 0) {
        freeMemory(*euclid, count);
        exit(EXIT_FAILURE);
    }

    count++;
}

int getDegree(int euclid[]) {
    int i, j;

    for(i = bytes - 1; i >= 0; i--) {
        printf("I:%d:[%d] ", i, euclid[i]);
        if(euclid[i]) {
            for(j = 32 -1; j >= 0; j--) {
                printf("J:%d:[%d] ", j, euclid[i]);
                if(TestBit(&(euclid[i]),j)) {
                    return j + 32*(i) + 1;
                }
            }
        }
    }
}

void euclidAlgorithm(int ***euclid) {
    int indexes[w0], i;

    srand(time(NULL));

    generateIndexes(indexes);
    printArray(indexes, w0);
    allocMemory(euclid);
    createPolynomial(indexes, (*euclid)[0]);
    putc('\n', stdout);
    printArray((*euclid)[0], bytes);
    putc('\n', stdout);
    allocMemory(euclid);
    SetBit((*euclid)[1], X);
    SetBit((*euclid)[1], 0);   
    printArray((*euclid)[1], bytes);

    allocMemory(euclid);
    (*euclid)[2][0] = 3;

    printf("TOTO : %d \n", getDegree((*euclid)[2]));
}



int main() {
    int **euclid = 0;

    euclidAlgorithm(&euclid);

    freeMemory(euclid, count);

    
    return 0;
}

