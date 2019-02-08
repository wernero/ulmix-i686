SUBDIRS := stdlib init hexdump ushell coreutils netutils

all: $(SUBDIRS)
	$(MAKE) -C ../diskimage

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

