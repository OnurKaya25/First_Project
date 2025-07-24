# My Advanced Calculator in C
I made an advanced level calculator in c. There are 10 different numerical analysis mode in program. Also you can write to calculate whatever you want.
 ### How does it work?
 * First of all you have to choose mode.
 * If you choose 1,2,3 or 11; The program will ask you about equation. So you will write your equation. But you should be careful about syntax. you should write like "2*x" instead of "2x" if you are going to use equation solver mode.
and about function syntax, you can find how to write in code. The program can solve logarithmic,trigonometric,inverse trigonometric functions.After that you will have define edges precisely if you have chosen mode 1 or 2. 
You should write it like [2,5] . So you have to use square brackets and comma while you define edges. (I added that because I wanted to waste my teacher's time but they didn't check themself :(  )
* If you choose 4,5,6 ; program will ask you about matrix's size. after entering that, you will enter numbers to coefficients matrix. Then to result matrix. After that you can solve it.
* If you choose 7,8,9 ;  the beginning is same as 1,2,3,11. You will enter your equation. Then the program will ask you about h or n; after you enter that, you should enter your number ( If you are using mode 8 or 9, then you will have to 
define edges like mode 1,2)
* If you choose mode 10; then you will enter how many point you will enter the program, then you will enter x and y coordinates with using blank. For example 2 5 
 ### How does code work?
* For equation entry, the code has dynamic input reader that I designed. So program can understand your equation even if it is about 1000 char. After that it creates an array that includes your array.
* After that, the program uses non-recursive descent parser (LL(1)) to parse your equation to tokens.
* The program keep it in linked list.
* After parsing, the program will calculate your equation.
* For matrix entry, I used double pointer to create dynamic result matrix and pointer to create result matrix.   
### How does calculating works after parsing?
* I use Shunting-Yard Algorithm. 
### Which operators can you use in Shunting Yard Algorithm?
* Firstly e is for euler number and pi is for pi number.
* +: for insertion
* -: for substraction
* *: for multiplication
* /: for division
* ^: for as a pow function ( in c)
* ( and ) for process prioty
* sin,cos,tan,cot,sec,csc for trigonometric functions
* arccos,arcsin,arctan for inverse trigonometric functions ( I forgot to add arccot :D )
* log_ and ln for logarithmic functions
* so if you choose mod 11,you can write something like e^(log_7(45+4-7*6)-ln(e/2))+sin(pi/2) and result will be 3 ( you can do math yourself.)
* Also if you choose mode 1,2 or 3 ; you can write something like e^sin(x/2)-log_5(3) then you enter edges like [-2,0] and the answer will be -0.783567


