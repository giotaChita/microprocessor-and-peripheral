	.global hash_func1
	.p2align 2
	.type	hash_func1, %function

hash_func1:
		MOV		r3, #0 		//hash stored in r3 and initialized to zero.
		start:
		LDRB  r2, [r0]  // Load byte into r2 from memory pointed to by r0 (src pointer)
		ADD  	r0, r0, #1    // Increment src pointer
		CMP   r2, #0    // Was the byte 0?
		BEQ 	fend				//If it is EXIT LOOP
		CMP		r2, #'0'	//if it's less that 0, loop
		BLT		start
		CMP 	r2, #'9'	//if it's greater than 9 go to cap_letter.
		BHI		cap_letter
		SUB		r2, r2, #48		//if it's less than 9 and greater than 0, substract ascii difference.
		SUB		r3, r3, r2		//And substract number's value from hash.
		cap_letter:
		CMP 	r2, #'A'		//if it's less than A and greater than 9, loop
		BLT 	start
		CMP 	r2, #'Z'	 //if it's greater than Z, loop
		BHI 	start
	//Add to hash letter's corresponded value - start
		SUB		r2, r2,#'A'  //Substract ascii code to find letter's index
		LSL   r2,r2,#2		//Multiply by 4 to load from the right address
		LDR		r2,[r1,r2]		//Load the letter's corresponded value - [r1] address of the beggining of the second argument
		ADD		r3, r3, r2		//Add value to hash
	//end
		B 		start			//Loop
	fend:
		MOV 	r0, r3			//Move returned value to r0
		LDR		r2, =0x20000770	//Load the memory address 0x2001FFFC to r2 - last available memory address to write
		STR		r0, [r2]				//Store r3 content to the memory address found in r2
		BX    lr        // Else return from subroutine
		NOP
		
