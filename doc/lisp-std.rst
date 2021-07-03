Lisp Standard Library
=====================

This provides documentation for every built-in function in the Lisp standard
library. It is not auto-generated, please update this documentation if you
change the API in any way.

In general every user-facing API in the standard library should be documented
here.

Built-in "functions"
--------------------

.. function:: (defun function-name (arg1 ... argN) & body)

    Define a function ``function-name`` that takes N arguments with names
    ``arg1`` ... ``argN``. ``body`` is evaluated in order, with the whole
    function evaluating to the last expression.

    .. code-block:: lisp
        
        (defun greet (name)
          (string-concat "Hello, " name))
        ; string-concat isn't even implemented yet, but you get the picture.

.. function:: (if condition true-condition [false-condition])

    Evaluates ``condition``, if it is truthy (non-``nil``) ``true-condition`` is
    evaluated. Otherwise ``false-condition`` is evaluated. If
    ``false-condition`` is not provided and ``condition`` is ``nil``, ``if``
    will evaluate to ``nil``.

    .. code-block:: lisp

        (print (if (= 2 3)
                  "2 = 3"
                  "2 /= 3"))
        ; 2 /= 3

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

Functions
---------

.. function:: (car pair)

    Return the first item in ``pair``.

.. function:: (cdr pair)

    Return the second (last) item in ``pair``.

.. function:: (cons a b)

    Return a cons-pair containing ``a`` and ``b``.

.. function:: (print val)

    Print out ``val`` to standard output. This will not be formatted as an
    s-expression, but in a manner more similar to the internal representation.

