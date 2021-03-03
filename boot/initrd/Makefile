SOURCES = config
JAYROOT = ../../

initrd.img: $(SOURCES) | $(JAYROOT)/bin/mkinitrd
	$(JAYROOT)/bin/mkinitrd $@ $^

$(JAYROOT)/bin/mkinitrd:
	$(MAKE) -C $(JAYROOT)/src/mkinitrd install

.PHONY: $(JAYROOT)/bin/mkinitrd
