# include "defs.h"

char field[4];

scr_phonemes(pmin,pmax,pstep,split)
int pmin,pmax,pstep,split;
{
	PHONEME *pp;
	register int cy;
	int line;
	if (pmax ==0) pmax = PSLICES*FPP-1;
	if (pstep == 2) pstep = FPP*2;
	else pstep = FPP;
	if (!split) {
		clear();
		xaxis(pmin,pmax,pstep);
	}
	for ALLPHONEMES {
	    for (cy = pmin; cy <= pmax; cy += pstep) {
		if (pp->ex[cy/FPP] > pthresh[P]) {
			if (split) {
			   line = yval(pp->ex[cy/FPP],20*yscale,10,0);
			}
			else {
			   line = yval(pp->ex[cy/FPP],20*yscale,20,0);
			}
			sprintf(field,"  %c",pp->sound);
			printfield(line,cy/pstep,field);
		}
	    }
	}
}

scr_words(pmin,pmax,pstep,split,word)
int pmin,pmax,pstep,split; char *word;
{
	WORD **wptr,*wp;
	register int cy;
	int max,one;
	int line, i;
	if (pmax ==0) pmax = PSLICES*FPP-1;
	if (pstep == 2) pstep = 2*FPP;
	else pstep = FPP;
	if (strcmp(word,"MAX") == 0) {
		max = 1;
		one = 0;
	}
	else if (strcmp(word,"ALL") == 0) {
		max = 0;
		one = 0;
	}
	else {
		max = 0;
		one =  1;
	}
	clear();
	xaxis(pmin,pmax,pstep);
	for ALLWORDPTRS {
	    wp = *wptr;
	    if (one && strcmp(wp->sound,word) != 0) {
		continue;
	    }
	    if (max) getmax(wp);

	    for (cy = pmin; cy <= pmax; cy += pstep) {
		if (wp->ex[cy/FPP] > pthresh[W] &&
				(!max || wp->ex[cy/FPP]  == wp->max)) {
			if (split) line = 
			   yval(wp->ex[cy/FPP],30*yscale,10,10);
			else line = yval(wp->ex[cy/FPP],30*yscale,20,0);
			sprintf(field,"  <");
			printfield(line,(cy - FPP)/pstep,field);
			for (i = 0; i < strlen(wp->sound);i++) {
				sprintf(field,"  %c",wp->sound[i]);
				printfield(line,(cy + 2*i*FPP)/pstep,field);
			}
			sprintf(field,"  >");
			printfield(line,(cy + (2*i -1)*FPP)/pstep,field);
		}
	    }
	}
}

/*
printfeatures(pmin,pmax,pstep,iop)
int pmin, pmax,pstep; 
FILE *iop;
{
	register int i,j,f,cy;
	if (pmax == 0) pmax = cycleno;
	if (pstep == 0) pstep = 1;
	fprintf(iop,"THE TRACE AT THE FEATURE LEVEL (INPUT AT %d)\n",cycleno);
	fprintf(iop,"Slice->\t");
	for (cy = pmin; cy <= pmax; cy += pstep) {
		fprintf(iop,"%3d",cy);
	}
	fprintf(iop,"\n");
	for (j = 0; j < NCONTINS; j++) {
	    for (f = 0; f < NFVALS; f++) {
		for (cy = pmin; cy <= pmax; cy += pstep) {
		    if (feature[j][f].ex[cy] > pthresh[F]) {
			break;
		    }
		}
		if (cy == pmax+pstep) continue;
		fprintf (iop,"%s.%d\t",contname[j],f);
		for (cy = pmin; cy <= pmax; cy += pstep) {
		    if (feature[j][f].ex[cy] <= pthresh[F]) 
			fprintf(iop,"   ");
		    else
			fprintf (iop,"%3d",(int) (feature[j][f].ex[cy]*100));
		}
		fprintf(iop,"\n");
	    }
	    if (sigflag) {sigflag = 0; return;}
	}
}
*/


scr_sum(min,max,step) 
int min,max,step;
{
	scr_words(min,max,step,1,"MAX");
	scr_phonemes(min,max,step,1);
}

xaxis(pmin,pmax,pstep) int pmin, pmax, pstep; {
	int cy, ph;

	for (cy = pmin; cy < pmax; cy += pstep) {
		sprintf(field," %2d",cy);
		printfield(XAXIS,cy/pstep,field);
	}
	for (ph = 0; ph < ninputs; ph++ ) {
		sprintf(field,"%3s",inspec[ph]);
		printfield(XAXIS,inpeak[ph]/pstep,field);
	}
	if ((cycleno/pstep)*VCOLWIDTH > 75) {
		sprintf(field,"%d>",cycleno);
		printfield(XAXIS,75,field);
		return;
	}
	move(XAXIS,(cycleno/pstep)*VCOLWIDTH);
	printw("*");
	move(XAXIS,(1+(cycleno/pstep))*VCOLWIDTH);
	printw("*");
}


printfield(line,vcol,str) int line, vcol; char *str; {
	int col;

	col = vcol*VCOLWIDTH;
	move(line,col);
	printw(str);
}

yval(act,gain,ymax,offset) double act; int gain, ymax, offset; {
	int yup, line;

	yup = gain*act;
	if (yup >= ymax) yup = ymax-1;
	yup = yup + offset;
	line = (XAXIS-1)-yup;
	if (line < 0) return(0);
	return(line);
}

getmax(wp) struct word *wp; {
	register int i;
	wp->mloc = 0;
	wp->max = 0;

	for (i = 0; i < PSLICES; i++) {
		if (wp->ex[i] > wp->max) {
			wp->mloc = i;
			wp->max = wp->ex[i];
		}
	}
}

scr_features()
{
	register int v,c;
	char tstr[30];
	clear();

	for (c = 0; c < NCONTINS; c++) {
		for (v = 0; v < NFVALS;v++) {
			sprintf(tstr,"%7.4f",feature[c][v].ex[fpcyc]);
			move(c,8*v);
			printw(tstr);
		}
	}
}

