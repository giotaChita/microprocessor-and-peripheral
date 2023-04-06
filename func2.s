	.global hash_func2
	.p2align 2
	.type	hash_func2, %function
	
hash_func2:
	PUSH	{r4,r5,r7} // push low registers 1,4,7 onto the stack
	CMP		r0, #0 // subtract from r0 the 0, r0 = argument input 
	BLT		fend_negative //only negative ,if N = 1
	BEQ		fend_zero		// if flag Z = 1 (one digit), then go to fend_zero
	hash_result:
	MOV	 	r3, #0 // r3 stores the sum, initialize it to 0
	
	hash_sum:
	// for mod process
	MOV 	r2, #10		// initialize r2 = 10 
	UDIV    r5, r0, r2     // div to get quotient     
	MUL     r4, r5, r2      // need for computing remainder
	SUB     r1, r0, r4      // r1 = the mod, the remainder
	
	ADD r3, r3, r1 // calculate the sum and stores it to r3
	UDIV r0, r0, r2 // div to get quotient of our argument
	CMP r0, #0 // checks if the process of adding the digits is done (one digit//10 = 0) 
	BNE hash_sum // if not repeat
	UDIV r7, r3, r2 // to checks if the sum is one digit 
	CMP r7, #0 // the check 
	MOV r0, r3 // store the value of r3 to  r0
	BEQ factorial // if the sum was one digit go to factorial
	MOV r1, r3 // if not store the value to r1 as our new argument
	B hash_result // and go back to hash_result
	
	
	factorial:
	MOV r1, r3 // final result 
	factor_loop:
	CMP r3, #1 // r3 contains hash_sum 
	BLE fend // if 1 then go to end its BLE and not BEQ for the case of 0 string given.
	SUB r3, r3, #1 // subtract from r3--
	MUL r1, r1, r3 // int result = fact(n-1)*n
	B factor_loop //go back to factorial
	
	fend_zero:
	MOV 	r1, #1 //if zero then 0!= 1
	B 		fend
	fend_negative:
	MOV 	r1, #0 //if negative put zero and end
	fend:
	MOV 	r0, r1			//Move returned value to r0
	LDR		r2, =0x20000774	//Load the memory address 0x20000774 to r2 
	STR		r0, [r2]	//Store r0 content to the memory address found in r2
	POP		{r4,r5, r7} // pop the low registers off the stack
	BX  	lr        // Else return from subroutine, the return address is loaded from the LR back into the PC
	NOP
