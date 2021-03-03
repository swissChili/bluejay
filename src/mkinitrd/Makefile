OBJECTS = main.o
JAYROOT = ../../
CFLAGS = -O2 -I$(JAYROOT)/include -Wall

mkinitrd: $(OBJECTS)
	$(CC) -o $@ $^

clean:
	rm -f *.o mkinitrd

.PHONY: clean
