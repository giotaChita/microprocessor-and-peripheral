#include <stdio.h>
#include <string.h>
#include "uart.h"

extern int hash_func2(int b);
extern int hash_func1(char *a, int *b);

int main(void)
{	
	uart_init(115200);
	uart_enable();
	
	char input[100];
	int nums[] = {10,42,12,21,7,5,67,48,69,2,36,3,19,1,14,51,71,8,26,54,75,15,6,59,13,25};
	int hash;
	int result;
		
	while(1){
		uart_print("\nEnter a string: ");
		scanf("%s",input);
		printf("%s", input);
		uart_print(input);
		if (strcmp(input, "ok") == 0) {
				uart_print("the end!");
				break;
		}
		hash = hash_func1(input, nums);
		printf("\nHash: %d ", hash);
		result = hash_func2(hash);
		printf("\nresult = %d", result);

	}
    
	//char a[] = "Aa! _2hk9Bm";	//random
	//char a[] = "!&*$()#3L";		//0
	//char a[] = "1!&*$()#3N8";		//negative
	return 0;
}
