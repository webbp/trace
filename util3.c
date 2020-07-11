# include "defs.h"
extern char *re_comp();

/* low-level subroutines -- plan is that they should not need to be
   changed too often  
   all calls directly to curses go thorugh these routines */

stop()
{
	if (signal(2,stop) & 01) signal(2,stop);
	sigflag = 1;
}

quit()
{
	register int i;
	FILE *iop;

	getstring("w to save params and quit, q to quit without saving: ");

	if (in[0] == 'q' || in[0] == 'y') {
		if (screenflag) end_window();
		exit();
	}
	if (in[0] == 'w')
		{
		getstring ("File name: ");
		if ( (iop = fopen(in,"w")) == NULL)
			{
			sprintf(out,"Can't open %s.",in);
			errprint(out);
			BACKTOTTY
			return;
			}
		dump(iop);
		if (screenflag) end_window();
		exit();
		}
}

getval(fp)
double *fp;
{
	sprintf(out,"%7.4f -- change? ",*fp);
	getstring(out);
	if (in[0] == 'n') return;
	sscanf(in,"%lf",fp);
}

getint(ip)
int *ip;
{
	sprintf(out,"%4d -- change? ",*ip);
	getstring(out);
	if (in[0] == 'n') return;
	sscanf(in,"%d",ip);
}


index(s,t)
char s[], t[];
{
	register int i,j,k;
	for (i = 0; s[i] != '\0'; i++)
		{
		for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++,k++);
		if (t[k] == '\0')
			return(i);
		}
	return(-1);
}

startsame(s1,s2) /* does s1 start the same as s2? s2 is the full version, s1
				 its possible beginning */
char *s1, *s2; 
{
	while (*s1) {
		if (*s1++ != *s2++)
		return(0);
	}
	return(1);
}


setex() 
{
	char *error;
getreloop:
	getstring("Regular expression: ");
	if (in[0] == '-' || in[0] == 'q')  {
		echeck = 0;
		return;
	}
	if ( (error = re_comp(in)) != NULL) {
		sprintf(out,"%s",error);
		errprint(out);
		BACKTOTTY
		goto getreloop;
	}
	echeck = 1;
}

int fin_count;

fileinput(type,str)
int type; char *str; {
	char name[100];
	if (type == STD) {
		if (altiop)  {
			fclose(altiop);
			altiop = NULL;
		}
		infp = stdin;
		screenflag = screenorig;
		return;
	}
	else if (type == EOF) {
	    if (altiop) {
		if (--fin_count>0) {
			rewind(altiop);
			return;
		}
		else  {
			fclose(altiop);
			altiop = NULL;
			if (screenflag) end_window();
			exit();
		}
	    }
	    infp = stdin;
	    screenflag = screenorig;
	    return;
	}
	else if (type == NONSTD)  {
		getstring ("Take input from file: ");
		strcpy(name,in);
		getstring("how many reps? " );
		if (sscanf(in,"%d",&fin_count) != 1) {
			sprintf(out,"Must give count for finput.");
			errprint(out);
			BACKTOTTY
			return;
		}
		if (altiop)  {
			fclose(altiop);
			altiop = NULL;
		}
		if ( (altiop = fopen(name,"r") ) == NULL) {
			sprintf(out,"Can't open %s.",in);
			errprint(out);
			BACKTOTTY
			return;
		}
		screenflag = 0;
		infp = altiop;
	}
	else if (type == ARG)  {
		if (altiop)  {
			fclose(altiop);
			altiop = NULL;
		}
		if ( (altiop = fopen(str,"r") ) == NULL) {
			sprintf(out,"Can't open %s.",str);
			errprint(out);
			BACKTOTTY
			return;
		}
		screenflag = 0;
		fin_count = 1;
		infp = altiop;
	}
}

ecomp(wp1,wp2)
WORD **wp1, **wp2; {
	double t;

	t = (*wp2)->ex - (*wp1)->ex;
	if (t > 0) return (1);
	if (t < 0) return (-1);
	return (0);
}


setoutfile(fname) char *fname;
{
	if (fname[0] == '-') {
		if (outfile != NULL) fclose(outfile);
		outfile = NULL;
		outflag = 0;
	}
	else {
		strcpy(outname,fname);
		if (outfile != NULL) fclose(outfile);
		if ( (outfile = fopen(outname,"a") ) == NULL) {
			sprintf(out,"cannot open %s.",outname);
			errprint(out);
			BACKTOTTY
			return;
		}
		outflag = 1;
	}
}

getlex()
{
    getstring("lexicon file name: ");
    if (lexicon(in))	{
	    initialize();
    }
}

getpars() 
{
    WORD *wp;
    register int i;

    getstring("parameter file name: ");
    if (load(in))	{
	    i = 0;
	    for ALLWORDS {
		    wp->base = rest[W] + fscale*log(1. + wordfreq[i]);
	    }
    }
}



end_window() {
	mcm(ILINE,0);
	refresh();
	nocrmode();
	echo();
	endwin();
}

chelp() {
        int line;
        for (line = 0; line < 21 ; line++) {
		move(line,0);
                printw("%*s",HELPWIDTH," ");
        }
}

help() {
        char **op;
        int line;
	if (!screenflag) return;
	chelp();
        for (op = optname, line = 0; *op != NULL ; line++) {
		move(line,0);
                printw("%-12s",*op++);
                if (*op != NULL) printw("%-12s",*op++);
                if (*op != NULL) printw("%-12s",*op++);
                if (*op != NULL) printw("%-12s",*op++);
        }
	refresh();
}

init_window() {
	initscr();
	crmode();
	noecho();
	clear();
	refresh();
}

mcm(line,col) int line; int col; {
	move(line,col);
	clrtoeol();
	move(line,col);
}

getstring(string)
char *string; {
	int eofck;
	if (screenflag) {
		screenprint(string,ILINE);
		getchars();
		clrtoeol();
		return(1);
	}
        else eofck = fscanf(infp,"%s",in);
	return(eofck);
}

getchars() {
	register char c;
	register char *p;

	p = in;

	while ( (c = getch()) != '\012' && c != '\015' && c != ' ') {
		if (c == '' || c == '') {
			addch('');
			refresh();
			*p--;
			continue;
		}
		*p++ = c;
		addch(c);
		refresh();
	}
	addch(' ');
	refresh();
	*p = '\0';
}

ttyprint(string) char *string; {
	screenprint(string,ELINE);
}

errprint(string) char *string; {
	if (screenflag) screenprint(string,ELINE);
	else {
		fprintf(stderr,string);
		fprintf(stderr,"\n");
	}
}

screenprint(string,line) char *string; int line; {
	if (!screenflag) return;
	mcm(line,0);
	printw(string);
	refresh();
}

dinspec() {
        int line;
	if (!screenflag) return;
	chelp();
        for (line = 0; line < ninputs; line++) {
		move(line,0);
                printw("%3s %3d %7.4f",
			inspec[line],inpeak[line],instrength[line]);
        }
	refresh();
}

getdouble(fptr,n,args)
register double *fptr;
int n;
char **args;
{
	register int i;
	
	if (screenflag) {
		chelp();
		for (i = 0; i < n; i++) {
			move(i,0);
			printw("%7s",args[i]);
			printw("%7.4f",*(fptr + i));
		}
	}
	getstring("changes? ");
	if (in[0] == ' ' || in[0] == 'n' || in[0] == 'q') return;
	for (i = 0; i < n; i++)
		if (strcmp(in,args[i]) == 0)  break;
	if (i == n) 
		{
		errprint("Unrecognized label.");
		BACKTOTTY
		return;
		}
	getstring("value: ");
	sscanf(in,"%lf",(fptr + i));
}

