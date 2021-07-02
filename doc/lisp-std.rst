Lisp Standard Library
=====================

This provides documentation for every built-in function in the Lisp standard
library. It is not auto-generated, please update this documentation if you
change the API in any way.

In general every user-facing API in the standard library should be documented
here.

- ``(x ...)`` represents a list ``x``.
- ``& body`` means that the rest of the list is represented by ``body``.
- ``[something]`` means that ``something`` is optional.

Top-level primitives
--------------------

These are "functions" that can only appear at the top-level of the program. This
means they can't be nested in any other expressions.

.. function:: (defun function-name (args ...) & body)

    Defines a function ``function-name`` that takes ``args`` and evaluates
    ``body``. ``function-name`` is quoted, not evaluated.

    .. code-block:: lisp

        (defun say-hi (name)
          (print "Hi, ")
          (print name))

        (say-hi "Joe")
        ; "Hi,"
        ; "Joe"

.. function:: (defmacro macro-name (args ...) & body)

    ``defmacro`` is to macros as ``defun`` is to functions. When ``macro-name``
    is called, whatever it evaluates to will be compiled.

    Note that internally this compiles a function the same way all other
    functions are compiled, meaning you can call **any** lisp function from a
    macro definition and it will work as expected.

    .. code-block:: Lisp

        (defun double (n)
          (+ n n))
        
        (defmacro call-with-4 (whatever)
          (print "this was run at **compile time**")
          (print whatever)
          ;; ``whatever`` expands to the form passed to this macro, in this case
          ;; ``double``.
          (list whatever 4))

        (print (call-with-4 double))
        ; "this was run at **compile time**"
        ; 'double
        ; 8

Functions
---------

.. function:: (if condition true-condition [false-condition])

	Evaluates ``condition``, if it is truthy (non-``nil``) ``true-condition`` is
	evaluated. Otherwise ``false-condition`` is evaluated. If
	``false-condition`` is not provided, ``if`` will evaluate to ``nil``.

.. function:: (let1 (variable binding) & body)

	Evaluates ``binding`` and binds it to ``variable``, then evaluates ``body``.
	After ``body`` is evaluated ``variable`` is unbound.

	.. code-block:: lisp

		(let1 (greeting (greet "John"))
		  (do-something greeting)
		  (print greeting))
		; greeting is no longer bound

.. function:: (gc)

	Force the garbage collector (GC) to run.

.. function:: (car pair)

	Return the first item in ``pair``.

    .. code-block:: lisp

        (car (cons 'a 'b)) ;=> 'a

.. function:: (cdr pair)

	Return the second (last) item in ``pair``.

    .. code-block:: lisp

        (cdr (cons 'a 'b)) ;=> 'b

.. function:: (cons a b)

	Return a cons-pair containing ``a`` and ``b``.

.. function:: (print val)

	Print out ``val`` to standard output. This will not be formatted as an
	s-expression, but in a manner more similar to the internal representation.

.. function:: (list & items)

    Returns a cons-list of items.

    .. code-block:: lisp

        (list 1 2 3)
        ; is the same as
        (cons 1 (cons 2 (cons 3 nil)))

.. function:: (quote form)

    Returns form without evaluating it.

    .. code-block:: lisp
     
        '(cons a b)
        ; or
        (quote cons a b)
        ; is the same as
        (list 'cons 'a 'b)
