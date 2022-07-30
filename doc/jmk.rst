JMK 2 Build System
==================

JMK2 is a rewrite of the JMK build system. I am slowly porting
Bluejay to JMK2 instead of the legacy M4-based JMK build system.

JMK2 is used to generate makefiles for each project in Bluejay. A
project is a directory with a ``Jmk2`` file (case sensitive). Each
project produces a single output based on some sources.

The script ``bin/jmk2`` looks in the source tree for ``Jmk2`` files,
and process each one into the corresponding Makefile. It accepts
option definitions with the ``-D`` flag, eg ``./bin/jmk2
-DSOME_OPTION=123``. You can also specify the C compiler, assembler,
and linker to use with the ``-c``, ``-a``, and ``-l`` flags,
respectively.

Here is an example Jmk2 file:

.. code-block:: tcl

    init hello # hello is the name of the project
    srcs hello.c world.c # the source files this project uses
    type executable # the preset type of project this is

Each line consists of a command (``init``, ``srcs``, ``type``) and its
arguments. The commands are documented here:

.. function:: init name [target]

    Initializes the project with a given name. The ``name`` is currently
    unused, but should be set to a descriptive identifier.

    ``target`` is the name of the target that the project
    generates. By default it is the same as ``name``. For an
    executable, this could be ``hello`` or ``hello.exe``. For a shared
    library, ``libhello.so``, etc.

.. function:: preset preset_name

    Applies the preset ``preset_name``. A preset is a function defined
    in the ``::presets`` namespace which makes some changes to the
    project state.

    These are the default presets:

    * ``freestanding`` Changes the ``cflags`` to build a freestanding binary (without linking the standard library).
    * ``optimize`` Changes the ``cflags`` and ``asmflags`` to enable \
      compile-time optimizations.
    * ``32`` Tells the compilers to produce a 32 bit build.
    * ``debug`` Tells the compilers to enable debug information in the \
      resulting builds (enables DWARF symbols).
    * ``warn`` Enables useful warnings and ``-Werror``.
    * ``nasm`` Sets ``nasm`` as the default assembler.

.. function:: presets preset_a [preset_b]...

    Applies all the given presets in order. Identical to calling
    ``preset`` once for each argument.

.. function:: cflag string

    Adds ``string`` to the ``::cflags`` variable, which will be passed
    to the C compiler.

.. function:: cflags string_a [string_b]...

    Adds multiple strings to the ``::cflags`` variable, the same as
    calling ``cflags`` repeatedly.

.. function:: asmflag, asmflags

    Same as ``cflag``, ``cflags`` but for the ``::asmflags`` variable.

.. function:: option name default_value

    If the option ``name`` has not been specified when invoking
    ``bin/jmk2``, sets the value of the option to
    ``default_value``. Options can be read with
    ``::options(option_name)``.

TODO: finish!
