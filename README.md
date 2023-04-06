# microprocessor and peripheral

## lab 1:

This project covers the assembly programming of an ARM micro-controller with
using the Keil tools. Within the assignment we write a routine, in ARM assembly, which will create the
hash from an alphanumeric string as follows :'

α) For each lowercase Latin letter it will add to the hash the number shown in
the following table
b) for each numeric digit it will subtract the value of the numeric digit from its hash
alphanumeric
c) the hash of the alphanumeric is not affected by any element of
the hash of the alphanumeric that is not a lowercase Latin letter or a numeric digit
d) add up the numbers in the hash until you arrive at a single digit number and
calculate its factor.

The final hash will be stored in a memory location of your choice.
![image](https://user-images.githubusercontent.com/69098448/230342479-2fe2781a-022b-434c-ba2c-2003edce35e5.png)


1. A basic C language main routine in which you dynamically provide the alphanumeric to be controlled using UART

2. An assembly language routine that will compute the hash of the alphanumeric, store its value in a memory location and return it to main

3. One routine in assembly that will compute the single digit from the hash and then its factorial, then store its value in a memory location and return it to main

5. Print, using printf(), the results with different inputs to your program that are based on different combinations.
