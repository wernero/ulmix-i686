SUBDIRS := stdlib init hexdump

all: $(SUBDIRS)
	$(MAKE) -C ../diskimage

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

