Kernel Logging
==============

Drivers and other kernel components may write log messages to the default output
(currently only VGA since it is the only display target implemented) using
``kprintf`` in ``include/kernel/log.h``. Additional defines in the same file may
help differentiate different types of log messages (i.e. errors, debug
information, etc).

.. code-block:: c

    kprintf(OKAY "Something succeeded\n");
    kprintf(ERROR "Something failed :(\n");
    // etc, see log.h for details
