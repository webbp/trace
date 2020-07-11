#include "defs.h"

char *optname[] =	{
	"abort","alpha","clear","coart","cycle","decay","dinspec",
	"expr","fcoption","finput","fpcyc",
	"fweight","freqscale","gamma","grace",
	"help(?)","inoise","inspecs","infeatures","imax",
	"lexicon","min","max","memo","ngraph","ngmax",
	"ngwscale","ngpscale","ngsscale","nreps","nsscale","output",
	"ofile","owtacts","phonacts","pfreq",
	"params","pthresh","quit","rarate","rc","rest","sumpr","sumopt",
	"sinspec","sfeatures","sfeatacts", "sscale",
	"test","topdown","windowcent",
	"wordacts","wgraph","wtacts","wbase","wchange","wgain","wramp",
	NULL};

char *levlabs[3] = {"F", "P", "W"};
char *conlabs[9]= {"IF","FF","FP","PP","PW","WW","WP","PF","PFC"};

command()
{
double tval;
int i,echeck;
int pmin,pmax,pstep;

while (echeck = getstring(": ")) {
	if (echeck == EOF) {
		fileinput(EOF);
	}
	else if (in[0] == '\0') {
		errprint("");
	}
	else if (startsame(in,"cycle")) {
		cycle();
	}
	else if (startsame(in,"clear")) {
		clear();
	}
	else if (startsame(in,"coarticulation")) {
		getint(&coartflag);
	}
	else if (startsame(in,"rc")) {
		zarrays();
		cycle();
	}
	else if (startsame(in,"wordacts")) {
		scr_words(printmin,printmax,3,0,"MAX");
	}
	else if (startsame(in,"wtacts")) {
		scr_words(printmin,printmax,3,0,"ALL");
	}
	else if (startsame(in,"owtacts")) {
		getstring("word: ");
		scr_words(printmin,printmax,3,0,in);
	}
	else if (startsame(in,"phonacts")) {
		scr_phonemes(printmin,printmax,3,0);
	}
	else if (startsame(in,"featacts")) {
		scr_features();
	}
	else if (startsame(in,"sfeatacts")) {
		getstring("fname: ");
		sfeatacts(in);
	}
	else if (startsame(in,"memo")) {
		getstring("string: ");
		strcpy(memo,in);
	}
	else if (startsame(in,"expr")) {
		setex();
	}
	else if (startsame(in,"fcopt")) {
	    getint(&fcflag);
	}
	else if (startsame(in,"fpcyc")) {
	    getint(&fpcyc);
	}
	else if (startsame(in,"finput")) {
		fileinput(NONSTD);
	}
	else if (startsame(in,"inoise")) {
		getval(&inoise);
	}
	else if (startsame(in,"inspecs")) {
		getstring("File name (- = stdin): ");
		inspecs(in);
	}
	else if (startsame(in,"infeatures")) {
		getstring("File name: ");
		infeats(in);
	}
	/* NOT PRESENTLY OPERATIVE -- JLM 10-5-82
	else if (startsame(in,"wsubset")) {
		wordsubset();
	}
	*/
	else if (startsame(in,"test")) {
		getstring("test string: ");
		strcpy(memo,in);
		test(in);
	}
	else if (startsame(in,"topdown")) {
		topdown();
	}
	else if (startsame(in,"output")) {
		setout();
	}
	else if (startsame(in,"ofile")) {
		getstring("give filename (or - for none): ");
		setoutfile(in);
	}
	else if (in[0] == '?') {
		help();
	}
	else if (startsame(in,"help")) {
		help();
	}
	else if (startsame(in,"lexicon")) {
		getlex();
	}
	else if (startsame(in,"params")) {
		getpars();
	}
	else if (startsame(in,"quit")) {
		quit();
	}
	else if (startsame(in,"decay")) {
		getdouble(decay,NLEVS,levlabs);
	}
	else if (startsame(in,"alpha")) {
		getdouble(alpha,NPARAMS,conlabs);
	}
	else if (startsame(in,"gamma")) {
		getdouble(ga,NLEVS,levlabs);
	}
	else if (startsame(in,"grace")) {
		getint(&grace);
	}
	else if (startsame(in,"rest")) {
		tval = rest[W];
		getdouble(rest,NLEVS,levlabs);
		if (tval != rest[W]) {
			initialize();
		}
	}
	else if (startsame(in,"fweight")) {
		getdouble(fweight,NCONTINS,contname);
	}
	else if (startsame(in,"pthresh")) {
		getdouble(pthresh,NLEVS,levlabs);
	}
	else if (startsame(in,"ngraph")) {
		newgraph(pmin,ng_max,pstep);
	}
	else if (startsame(in,"ngmax")) {
		getint(&ng_max);
	}
	else if (startsame(in,"ngwscale")) {
		getval(&ng_wscale);
	}
	else if (startsame(in,"ngsscale")) {
		getval(&ng_sscale);
	}
	else if (startsame(in,"ngpscale")) {
		getval(&ng_pscale);
	}
	else if (startsame(in,"nreps")) {
		getint(&nreps);
	}
	else if (startsame(in,"pfreq")) {
		getint(&printfreq);
	}
	else if (startsame(in,"rarate")) {
		getval(&rarate);
	}
	else if (startsame(in,"sumpr")) {
		scr_sum(pmin,pmax,pstep);
	}
	else if (startsame(in,"sinspec")) {
		sinspec();
	}
	else if (startsame(in,"sfeatures")) {
		getstring("Filename: ");
		sfeatures(in);
	}
	else if (startsame(in,"dinspec")) {
		dinspec();
	}
	else if (startsame(in,"sumopt")) {
	    getint(&sumflag);
	}
	else if (startsame(in,"pmin")) {
		getint(&pmin);
	}
	else if (startsame(in,"pmax")) {
		getint(&pmax);
	}
	else if (startsame(in,"pstep")) {
		getint(&pstep);
	}
	else if (startsame(in,"min")) {
		getval(&min);
	}
	else if (startsame(in,"max")) {
		getval(&max);
	}
	else if (startsame(in,"windowcent")) {
		getval(&windowcent);
	}
	else if (startsame(in,"wbase")) {
		getval(&wbase);
	}
	else if (startsame(in,"wgraph")) {
		wgraph(pmin,ng_max,pstep);
	}
	else if (startsame(in,"wchange")) {
		getval(&wchange);
	}
	else if (startsame(in,"wgain")) {
		getval(&wgain);
	}
	else if (startsame(in,"wramp")) {
		getval(&wramp);
	}
	else if (startsame(in,"imax")) {
		getval(&imax);
	}
	else if (startsame(in,"sscale")) {
		getval(&sscale);
	}
	else if (startsame(in,"nsscale")) {
		getval(&nsscale);
	}
	else if (startsame(in,"freqscale")) {
		tval = fscale;
		getval(&fscale);
		if (tval != fscale) {
		    initialize();
		}
	}
	else if (startsame(in,"abort")) {
		abort();	/* to get a core dump for sdb */
	}
	else {
		errprint("Unrecognized request: For help type ?.");
		if (infp != stdin) fileinput(STD);
	}
	wait(0);
}
}

