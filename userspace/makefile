SUBDIRS := stdlib init hexdump ushell coreutils

all: $(SUBDIRS)
	$(MAKE) -C ../diskimage

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

