CC = /opt/local/bin/gcc-mp-10
DEBUG = -g 
OPTIMIZE = -O
VC = vc
VFLAGS = -O2
CFLAGS = ${DEBUG}
LFLAGS = -L/opt/local/lib -lm -lcurses -lgnuregex
TOBJS = trace.o initialize.o interact.o command.o util1.o util2.o util3.o util4.o util5.o gfv.o gnoise.o
VTOBJS = trace.o initialize.o vinteract.o command.o util1.o util2.o util3.o util4.o util5.o gfv.o gnoise.o
CPOBJS = trace.o initialize.o interact.o command.o util1.o util2.o util3.o util4.o util5.o cpfv.o 
TSRCS = trace.c initialize.c interact.c command.c util1.c util2.c util3.c util4.c util5.c gfv.c gnoise.o
CPSRCS = trace.c initialize.c interact.c command.c util1.c util2.c util3.c util4.c util5.c cpfv.c

trace:	$(TOBJS)
	$(CC) $(CFLAGS) $(TOBJS) $(LFLAGS) -o trace

ttrace:	$(TOBJS)
	$(CC) $(CFLAGS) $(TOBJS) $(LFLAGS) -o ttrace

vtrace:	$(VTOBJS)
	$(CC) $(CFLAGS) $(VTOBJS) $(LFLAGS) -o vtrace

cptrace: $(CPOBJS)
	$(CC) $(CFLAGS) $(CPOBJS) $(LFLAGS) -o cptrace

trace.o:	defs.h trace.c
	$(CC) $(CFLAGS) -c trace.c

initialize.o:	defs.h initialize.c
	$(CC) $(CFLAGS) -c initialize.c

interact.o:	defs.h interact.c
	$(CC) $(CFLAGS) -c interact.c

vinteract.o:	defs.h vinteract.c
	$(VC) $(VFLAGS) -c vinteract.c

command.o:	defs.h command.c
	$(CC) $(CFLAGS) -c command.c

fetvals.o:	defs.h fetvals.c
	$(CC) $(CFLAGS) -c fetvals.c

util1.o:	defs.h util1.c
	$(CC) $(CFLAGS) -c util1.c -Wno-return-type

util2.o:	defs.h util2.c
	$(CC) $(CFLAGS) -c util2.c -Wno-return-type

util3.o:	defs.h util3.c
	$(CC) $(CFLAGS) -c util3.c -Wno-return-type

util4.o:	defs.h util4.c
	$(CC) $(CFLAGS) -c util4.c -Wno-return-type

util5.o:	defs.h util5.c
	$(CC) $(CFLAGS) -c util5.c -Wno-return-type
	
gnoise.o:	gnoise.c
	$(CC) $(CFLAGS) -c gnoise.c

clean:
	rm *.o trace cptrace

tags:
	ctags ${TSRCS} defs.h
