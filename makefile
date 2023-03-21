all:
	$(MAKE) -C build-context
%:
	$(MAKE) -C build-context $@