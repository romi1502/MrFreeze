all:
	$(MAKE) -C Freeze

install: all
	$(MAKE) -C Freeze install

clean:
	$(MAKE) -C Freeze clean
	rm -r freeze_engine/*.o