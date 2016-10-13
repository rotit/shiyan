SRCS:=a.c b.c c.c

a: .depend b.o c.o 
	gcc -o $@ b.o c.o a.c
	echo XXXX
.depend :
	@for f in $(SRCS); do \
	g=`basename $$f | sed -e 's/\(.*\)\.\w/\1.o/'`; \
	$(CC) -M $(CPPFLAGS) -MQ $(obj)$$g $$f >> $@ ; \
	done

sinclude .depend

