OBJECTS = main.o
JAYROOT = ../../
CFLAGS = -O2 -I$(JAYROOT)/include -Wall

mkinitrd: $(OBJECTS)
	$(CC) -o $@ $^

install: mkinitrd
	cp mkinitrd $(JAYROOT)/bin

clean:
	rm -f *.o mkinitrd

.PHONY: clean install
