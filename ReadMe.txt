Polynomial Calculator
Authored by Yazan Daefi
323139923

==Description==

The program is a polynomial calculator, it runs in unlimited loop and receives a polynomial from the user and value to replace in the polynomial until the user enters “done”.
It contains two parts: ex4a – that calculate the result using threads according to the polynomial degree.
ex4b – that calculate the result using a shared memory.

Program DATABASE:
1.power – array of integers which save the degrees in the polynomial.
2.parameters – array of integers that saves all the factors.
functions:
six main functions:
1.PolynomialsSolve: the main function that contains the main loop of the program, receiving polynomials from the user until identify the word "done".
2.parsePolynomial: function that cut each part of the polynomial and send to parsePart function, receives the equation.
3.parsePart: function that parse a part of the polynomial into the global arrays, receives the part by parsePolynomial function.
4.isValid: function that checks the polynomial and decided if the input is valid, checks the spaces and if contains a degree that larger than 3.
5.calculatePart: in ex4a - void* function to be run by the thread, it calculates a polynomial part, in ex4b - function that calculate the value of a specific part from the polynomial, receives index that
6.calculatEquation: function that calculate the polynomial, in ex4a – using threads, in ex4b – using shared memory.
==Program Files==
ex4a.c - the file contains all the functions of the first part that using threads.
ex4b.c - the file contains all the functions of the first part that using shared memory.
==How to compile?==
compile: gcc ex4a.c -o ex4 -pthread
compile: gcc ex4b.c -o ex4
run: ./ex4

==Input:==
an equation from the user in the format “POLYNOMIAL, VALUE” or “done” to exit.

==Output:==
the result of the equation.
