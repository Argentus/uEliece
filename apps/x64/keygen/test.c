#include <stdio.h>
#include <stdlib.h>

#define SetBit(P,i)     ( (P)[( (i) / 32)] |=  (1 << ( (i) % 32)) )
#define ClearBit(P,i)   ( (P)[( (i) / 32)] &= ~(1 << ( (i) % 32)) )
#define TestBit(P,i)  !!( (P)[( (i) / 32)] &   (1 << ( (i) % 32)) )

int main(void)
{
    int arr[2] = {0};

    //SetBit(arr, 10);
    arr[0] = 5;
    printf("%d %d \n", arr[0], arr[1]);

    int i;

    for(i = 32 - 1; i >= 0; i--) {
    	printf("%d\n", i);
    	if(TestBit(arr, i)) {
    		printf("%s %d\n", "ads", i);
    	}
    }
    return 0;
}