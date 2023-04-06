
#include <stdio.h>

extern int hash_func1(char *a, int *b); // import the function for calculating the hash sum
extern int hash_func2(int b);  // import the function which calculates the factorial 


int main(void)
{
	printf("TEST");
	
	char a[] = "Aa! _2hk9Bm";	// random string as input
	//char a[] = "!&*$()#3L";		//  L = 3, so the function1 should return 3-3 = 0
	//char a[] = "1!&*$()#3N8";		//negative sum so it should return 0
	int nums[] = {10,42,12,21,7,5,67,48,69,2,36,3,19,1,14,51,71,8,26,54,75,15,6,59,13,25};
	int hash;
	hash = hash_func1(a,nums); 
	printf("Hash: %d\n", hash);
	printf("result = %d", hash_func2(hash));
	
	return 0;
}

