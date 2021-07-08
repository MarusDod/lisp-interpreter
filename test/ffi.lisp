(setf handle (dll-open "./libtest.so"))

(setf foo (dll-load handle "foo"))

(defun b (x y) (dll-apply foo x y))

(print (b 1 2))

(dll-close handle)