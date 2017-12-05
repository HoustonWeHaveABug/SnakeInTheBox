SITB_C_FLAGS=-O2 -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

sitb: sitb.o
	gcc -o sitb sitb.o

sitb.o: sitb.c sitb.make
	gcc -c ${SITB_C_FLAGS} -o sitb.o sitb.c

clean:
	rm -f sitb sitb.o
