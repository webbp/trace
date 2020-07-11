# include "defs.h"

FILE *altiop = NULL;
FILE *infp = stdin;
FILE *outfile = NULL;
char outname[30];
int ecomp();

struct word *wordptr[NWORDS + 1];
struct word *wtord[NWORDS + 1];
double *exptr[PSLICES];
int nwptrs;
int printmin, printmax;
double windowcent = 78;		/* make double for getval routine */
int fpcyc;

char wordsound[NWORDS][MAXLEN];
int wordfreq[NWORDS];
struct phoneme phoneme[NPHONS];
struct phoneme *phonptr[128]; /* pointers to phoneme structures indexed 
			   by the character corresponding to the phoneme */

struct feature feature[NCONTINS][NFVALS];

int nwords = 0;

char in[BUFSIZ];
char out[BUFSIZ];

double max = { 1.};
double min = {-.3};
double wbase = {.3};
double wchange = {.1};
double wgain = {1.};
double wramp = {1.};
double rate = {.5};

int   yscale = 1;
double inoise = 0.0;
double rarate = 1.0;
double pthresh[NLEVS]    = {.00,.00,.05};
double linthresh = .2; 
double linthrsq = .04;
double decay[NLEVS]      = {.02,.02,.02}; 
double alpha[NPARAMS] = 
/*   IF   FF    FP  PP  PW  WW  WP  PF  PFC  */
    {1.0,.00,.020,.00,.10,.00,.15,.00,.00};

double ga[NLEVS] = 
/*   FF    PP     WW */
   {.100, .100, .100};

double sscale = 10;
double nsscale = 10;

double rest[NLEVS]   = {-.1, -.1, -.01};

double fscale = {.00};
double imax = {3.};

double psum[PSLICES];
double pisum[PSLICES];
int pcount[PSLICES];

double fsum[NCONTINS][FSLICES];
int fcount[NCONTINS][FSLICES];

double wisum[PSLICES];
double wsum[PSLICES];
int wcount[PSLICES];

double ppi[PSLICES];
double wwi[PSLICES];
double ffi[NCONTINS][FSLICES];
int cycleno;

int nreps = 6;
int sigflag;
int echeck;
int outflag;
int sumflag = 1;
int fcflag = 1;
int echoflag = 0;
int grace = 0;
int screenflag = 1;
int screenorig;
int coartflag = 1;
int printfreq = 6;

int stop();

struct phoneme 	*inphone[MAXITEMS];
char 	inspec[MAXITEMS][60];
double 	infet[MAXITEMS][NCONTINS][NFVALS];

double 	instrength[MAXITEMS];
int 	inpeak[MAXITEMS];
int  	indur[MAXITEMS];
double	inrate[MAXITEMS];
double	insus[MAXITEMS];

struct word 	*outword[MAXITEMS];
struct phoneme 	*outphoneme[MAXITEMS];
int outslice[MAXITEMS];
int woutslice[MAXITEMS];
double rex[MAXITEMS];

int ninputs, nwouts, npouts;

double input[FSLICES][NCONTINS][NFVALS];

main(argc,argv)
int argc; char **argv;
{
	int specflag = 0;
	int scriptflag = 0;
	char scriptfile[100];
	srandom(time(0));
	while (--argc > 0 && (*++argv) [0] == '-') {
		switch (argv[0][1]) {
			case 'p':
				load(*++argv);
				argc--;
				break;
			case 'l':
				lexicon(*++argv);
				argc--;
				break;
			case 'i':
				inspecs(*++argv);
				specflag = 1;
				argc--;
				break;
			case 'f':
				infeats(*++argv);
				argc--;
				break;
			case 'a':	/* to make it easy for sdb */
				abort();
				break;
			case 'b':
				screenflag = 0;
				break;
			case 's':
				scriptflag = 1;
				strcpy(scriptfile,*++argv);
				argc--;
				break;
		}
	}
	if (signal(2,stop) & 01) signal(2,stop);
	initialize();
	if (screenflag) init_window();
	screenorig = screenflag;
	if (specflag) inputcomp();
	if (scriptflag) fileinput(ARG,scriptfile);
	command();
}

cycle() 
{
	int tick,pmin,pmax;
	for (tick = 0; tick < nreps; tick++) {
	    act_features();
	    fpinteract();
	    pfinteract();
	    pwinteract();
	    wpinteract();
	    ppinteract();
	    wwinteract();
	    fupdate();
	    pupdate();
	    wupdate();
	    cycleno++;
	    if (fcflag) prave();
	    sprintf(out,"%d",cycleno);
	    if ( !(cycleno%printfreq) ) {
			if (sumflag == 1) {
				if (screenflag) {
				    scr_sum(printmin,printmax,FPP,stdout);
				}
				if (outfile) {
				    summarypr(printmin,printmax,FPP,outfile);
				}
			}
			if (sumflag == 2) {
				if (screenflag) {
				    scr_words(printmin,printmax,3,0,"MAX");
				}
			}
			if (sumflag == 3) {
			   if (npouts) outprob();
			   if (nwouts) woutprob();
			}
	    }
	    if (sumflag == 1) {
	      if (npouts) outprob();
	      if (nwouts) woutprob();
	    }
	    if (screenflag) {
		ttyprint(out);
	    }
	    if (sigflag) {sigflag = 0;break;}
	}
	if (outfile) fflush(outfile);
}

zarrays() 
{
	WORD *wp, **wptr;
	PHONEME *pp;
	register int i,f,c,p;

	cycleno = 0;
	printmin = 0;
	printmax = (FSLICES < 66 ? FSLICES : 66);
	for (i = 0; i < PSLICES; i++) {
		wwi[i] =0; wsum[i] =0; wisum[i] =0; wcount[i] = 0;
		ppi[i] =0; psum[i] =0; pisum[i] = 0; pcount[i] = 0;
	}

	for (i = 0; i < FSLICES; i++) {
	    for (c = 0; c < NCONTINS; c++) {
		ffi[c][i] =0; fsum[c][i] =0; fcount[c][i] = 0;
	}   }

	for ALLWORDPTRS {
	    wp = *wptr;
		wp->pextot = 0;
	    wp->rest = wp->prime + wp->base;
	    for (i = 0; i < PSLICES; i++) {
		wp->ex[i] = wp->rest;
		wp->nex[i] =0;
		wp->pex[i] = 0;
		wp->wex[i] = 0;
	}   }

	for ALLPHONEMES {
		pp->fextot = 0;
		pp->wextot= 0;
	    for (i = 0; i < PSLICES; i++) {
		pp->ex[i] = rest[P];
		pp->wex[i] = 0; pp->fex[i] = 0; pp->nex[i] = 0;
		pp->pex[i] = 0;
	}   }

	for (c = 0; c < NCONTINS; c++) {
	    for (f=0; f < NFVALS; f++) {
		feature[c][f].pextot = 0;
		for (i = 0; i < FSLICES; i++) {
		    feature[c][f].ex[i] = rest[F];
		    feature[c][f].pex[i] = 0;
		    feature[c][f].fex[i] = 0;
		    feature[c][f].nex[i] = 0;
	}   }   }
	for (p = 0; p < npouts; p++) {
		rex[p] = outphoneme[p]->ex[outslice[p]];
	}
}

prave() {
	int p;
	for (p = 0; p < npouts; p++) {
		rex[p] = (rarate)*outphoneme[p]->ex[outslice[p]]
			+ (1-rarate)*rex[p];
	}
}
