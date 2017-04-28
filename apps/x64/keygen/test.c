#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define X 9857
#define w0 71
//#define bytes (X / 32) + 1
#define bytes 10

#define SetBit(P,i)     ( (P)[( (i) / 32)] |=  (1 << ( (i) % 32)) )
#define ClearBit(P,i)   ( (P)[( (i) / 32)] &= ~(1 << ( (i) % 32)) )
#define TestBit(P,i)  !!( (P)[( (i) / 32)] &   (1 << ( (i) % 32)) )



int getDegree(int euclid[]) {
    int i, j;

    for(i = bytes - 1; i >= 0; i--) {;
        if(euclid[i]) {
            for(j = 32 -1; j >= 0; j--) {
                if(TestBit(&(euclid[i]),j)) {
                    return j + 32*(i) + 1;
                }
            }
        }
    }
}

int OgetDegree(int euclid[]) {
    int i, j;

    for(i = bytes - 1; i >= 0; i--) {
        if(euclid[i]) {
            for(j = 32 -1; j >= 0; j--) {
                if(TestBit(&(euclid[i]),j)) {
                    return j + 32*(i) + 1;
                }
            }
        }
    }

    return 0;
}


void printBits(int pol[]) {
	int i;

	for(int i = 0; i < bytes * 32; i++) {
		printf("%d", TestBit(pol,i));
		if(i % 31 == 0 && i != 0)
			printf(" ");
	}
	printf("\n");
}

void shiftPolynomial(int originPoly[], int shiftedPoly[], int shift) {
    //test 16bit integer, then 8bit etc to speed it up
    int i, j;

    for(i = 0; i < bytes; i++) {
        if(originPoly[i]) {
            for(j = 0; j < 32; j++) {
                if(TestBit(originPoly + i,j))
                    SetBit(shiftedPoly, j + shift);
            }
        }
    }
}

void addingPolynomials(int augend[], int addend[], int total[]) {
    int i;

    for(i = 0; i < bytes; i++) {
        total[i] = augend[i] ^ addend[i];
    }
}

void multiplePolynomials(int multiplier[], int multiplicant[], int product[]) {
  //TODO Russian Peasant Multiplication algorithm
  //TODO the same as in dividePolynomials, shift no needed, just add with offset
  int i, j, tempMultiplier[bytes] = {0};
    for(i = 0; i < bytes; i++) {
        if(multiplicant[i]) {
            for(j = 0; j < 32; j++) {
                if(TestBit(multiplicant + i,j)) {
                  shiftPolynomial(multiplier, tempMultiplier, (i+1)*j);
                  addingPolynomials(product, tempMultiplier, product);
                  memset(tempMultiplier, 0, sizeof(tempMultiplier));
                }
            }
        }
    }
}

void test(int *a) {
    printf("EQW: %d\n", a[0]);
}

int main(void)
{
    int arr[10] = {0};
    int sec[10] = {0};
    int vys[10] = {0};
    int q[10] = {0};

    //SetBit(arr, 0);
    //sec[0] = 3;
    //memcpy(vys, sec, sizeof(vys));

    //printBits(vys);
	//shiftPolynomial(sec, vys, getDegree(arr) - getDegree(sec));
	//printBits(vys);
	//SetBit(q, getDegree(arr) - getDegree(sec) );
	//printBits(q);
	//printf("%d %d %d %d %d %d %d %d %d %d \n", arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8],arr[9]);
	//printBits(arr);

    arr[0] = 53;
    sec[0] = 3;
    multiplePolynomials(arr, sec, vys);
    printBits(arr);
    printf("%s\n", "ads");
    printBits(sec);
    printf("%s\n", "ads");
    printBits(vys);

    q[5] = 6;
    q[0] = 2;
    test(q+5);

    return 0;
}

/*
  x8+x4+x3+x+1(f1) : x6+x4+x+1(f2) = x2 + 1
-[x8+x6+x3+x2]
  x6+x4+x2+x+1
-[x6+x4+x+1]
  x2(f3)


   0    1    2    3    4    5    6    7    8
N: 1    1    0    1    1    0    0    0    1
D: 1    1    0    0    1    0    1    0    0

degreeOfDivident = 9
degreeOfDivisor = 7
shift bude o 2

   0    1    2    3    4    5    6    7    8
N: 1    1    0    1    1    0    0    0    1
d: 0    0    1    1    0    0    1    0    1


   0    1    2
q  0    0    1

teraz N = N-d

   0    1    2    3    4    5    6    7    8
N: 1    1    0    1    1    0    0    0    1
d: 0    0    1    1    0    0    1    0    1
N: 1    1    1    0    1    0    1    0    0

LOOOOOOP

   0    1    2    3    4    5    6
N: 1    1    1    0    1    0    1
D: 1    1    0    0    1    0    1

shift netreba

   0    1    2
q  1    0    1

   0    1    2    3    4    5    6
N: 1    1    1    0    1    0    1
D: 1    1    0    0    1    0    1
N: 0    0    1    0    0    0    0

LOOOOOOOOP
   0    1    2    3    4    5    6
N: 0    0    1    0    0    0    0
D: 1    1    0    0    1    0    1

r = N
   0    1    2
r: 0    0    1

q: x2 + 1
r x2





*/