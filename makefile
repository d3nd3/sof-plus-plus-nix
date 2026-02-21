JOBS ?= $(shell nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
all:
	$(MAKE) -C src -j$(JOBS)
%:
	$(MAKE) -C src -j$(JOBS) $@