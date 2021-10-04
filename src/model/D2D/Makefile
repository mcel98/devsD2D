ROOT := $(shell pwd)
SUBDIRS := $(wildcard $(ROOT)/atomics/*/.)

all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)


