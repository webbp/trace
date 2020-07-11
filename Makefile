DEBUG = -g 
OPTIMIZE = -O
VC = vc
VFLAGS = -O2
CFLAGS = ${DEBUG}
LFLAGS = -lm -lcurses -ltermlib
TOBJS = trace.o initialize.o interact.o command.o util1.o util2.o util3.o util4.o util5.o gfv.o gnoise.o
VTOBJS = trace.o initialize.o vinteract.o command.o util1.o util2.o util3.o util4.o util5.o gfv.o gnoise.o
CPOBJS = trace.o initialize.o interact.o command.o util1.o util2.o util3.o util4.o util5.o cpfv.o 
TSRCS = trace.c initialize.c interact.c command.c util1.c util2.c util3.c util4.c util5.c gfv.c gnoise.o
CPSRCS = trace.c initialize.c interact.c command.c util1.c util2.c util3.c util4.c util5.c cpfv.c

trace:	$(TOBJS)
	cc $(CFLAGS) $(TOBJS) $(LFLAGS) -o trace

ttrace:	$(TOBJS)
	cc $(CFLAGS) $(TOBJS) $(LFLAGS) -o ttrace

vtrace:	$(VTOBJS)
	cc $(CFLAGS) $(VTOBJS) $(LFLAGS) -o vtrace

cptrace: $(CPOBJS)
	cc $(CFLAGS) $(CPOBJS) $(LFLAGS) -o cptrace

trace.o:	defs.h trace.c
	cc $(CFLAGS) -c trace.c

initialize.o:	defs.h initialize.c
	cc $(CFLAGS) -c initialize.c

interact.o:	defs.h interact.c
	cc $(CFLAGS) -c interact.c

vinteract.o:	defs.h vinteract.c
	$(VC) $(VFLAGS) -c vinteract.c

command.o:	defs.h command.c
	cc $(CFLAGS) -c command.c

fetvals.o:	defs.h fetvals.c
	cc $(CFLAGS) -c fetvals.c

util1.o:	defs.h util1.c
	cc $(CFLAGS) -c util1.c

util2.o:	defs.h util2.c
	cc $(CFLAGS) -c util2.c

util3.o:	defs.h util3.c
	cc $(CFLAGS) -c util3.c

util4.o:	defs.h util4.c
	cc $(CFLAGS) -c util4.c

util5.o:	defs.h util5.c
	cc $(CFLAGS) -c util5.c
	
gnoise.o:	gnoise.c
	cc $(CFLAGS) -c gnoise.c	

clean:
	rm *.o trace cptrace

tags:
	ctags ${TSRCS} defs.h
