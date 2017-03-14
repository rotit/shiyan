	
sources:=$(wildcard *.c)
objects:=$(sources:.c=.o)
dependence:=$(sources:.c=.d)

CC=gcc

all: $(objects)
	$(CC) $^ -o $@

include $(dependence)
%.d: %.c
	set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean
clean:
	rm -f all $(objects) $(dependence)
