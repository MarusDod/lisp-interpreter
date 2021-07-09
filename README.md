# Lisp Interpreter

A lisp interpreter in C. Still work in progress.



### Build project: 

```bash
    git clone https://github.com/MarusDod/lisp-interpreter
    #cd into directory
    mkdir build
    cd build
    cmake ..
    make
```

### Install:

```bash
    sudo make install
```

### Uninstall:

```bash
    sudo xargs rm < install_manifest.txt
```



### Run source file:

```
    lisp file.lisp
```

### Run repl:

```
    lisp
```

### Safely exit the repl:
    
```
    abort
```

### Parser:

    -Supports quotes, symbols, strings, integers, list and arryas
    -Single line comments with semi-colon.


### Eval:

    -Pretty much same syntax as common lisp
    -No need for funcall


### Memory management:

    -None (as of yet)

### Features:
    - Vectors
    - Higher order functions
    - Closures
    - Lists
    - Strings
    - Positive integers
    - C FFI

### Examples:

```lisp
      (cons 1 '(2 3)) ; (1 2 3)
  
      (setf b (+ 2 3)) ; b
  
      (* 3 (+ 6 b)) ; 33
```


```lisp
    (defun double (x) (* x 2)) ; double
  
    (double b) ; 10
```


```lisp
    (if (numberp b) 5 10) ; 5
    (length '((1 2 3) w (3 f))) ; 3

     (print "haha") ; "haha" "haha"
     (null nil) ; T
    (defun factorial (x) (if (< x 2) 1 (* x (factorial (- x 1))))
      (print (factorial 10)) ; 3628800

    (defun make-adder (x) (lambda (y) (+ x y)))

    (setf adder (make-adder 5))

    (print (adder 10)) ; 15
```

```lisp
    (setf handle (dll-open "./libtest.so"))

    (setf foo (dll-load handle "foo"))
    (defun b (x y) (dll-apply foo x y))
    (print (b 1 2))

    (dll-close handle)
```
 
## Features to implement:
  
    - Macros
    - Optional Arguments
    - String manipulation
    - Garbage collection
    - Just in Time Compilation

