# lisp-interpreter

A lisp interpreter in C. Still work in progress.



To build project: 

make

Run source file:

	./prog file.lisp

Run repl:

	./prog

To safely exit the repl:
    
    abort



Parser:

	-Only supports single line comments with semi-colon.
  
	-Don't chain multiple symbols together. Ex: don't (+1 2).



Eval:

	-Supports pretty much same syntax as common lisp

    -Support recursion
   
    -Quoted symbols and lists will not be evaluated


Garbage Collection:

	-I didn't include manual memory management, instead it only deallocates at the end (can be quite the memory hog).
  
	-Again this is still a work in progress, please do not ever use this in your main projects.

Built-in primitive functions: +,-,/,%,cons,car,cdr,eq,numberp,null,quote,eval,defun,setf,length,print,if,<,>,and,or,progn,aref,array-dimensions,lambda,make-array,list,sleep

Supports:
    -Vectors
    -Lambdas / anonymous functions
    -Closures
    -Higher order functions
    -Lists
    -Strings
    -Positive integers

Examples:


  	(cons 1 '(2 3)) -> (1 2 3)
  
  	(setf b (+ 2 3)) -> b
  
  	(* 3 (+ 6 b)) -> 33


	(defun double (x) (* x 2)) -> double
  
	(double b) -> 10


	(if (numberp b) 5 10) -> 5

	(length '((1 2 3) w (3 f))) -> 3

 	(print "haha") -> "haha" "haha"
  
 	(null nil) -> T
	
	(defun factorial (x) (if (< x 2) 1 (* x (factorial (- x 1))))
  
  	(print (factorial 10)) -> 3628800


    (defun make-adder (x) (lambda (y) (+ x y)))

    (setf adder (make-adder 5))

    (print (adder 10)) -> 15
 
Features to implement:
  
	-Macros
  
    -Optional Arguments

	-String manipulation
  
	-Garbage collection
  
	-Pretty much everything else

