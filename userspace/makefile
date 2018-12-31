SUBDIRS := stdlib init hexdump ushell

all: $(SUBDIRS)
	$(MAKE) -C ../diskimage

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

