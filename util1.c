#include "defs.h"
char memo[100];

/* parameter load and dump, read lexicon, and display routines */

load(infile)
char *infile;
{
	FILE *iop;
	register int i;
	if ( (iop = fopen(infile,"r")) == NULL)
		{
		sprintf(out,"Can't open %s.",infile);
		errprint(out);
		BACKTOTTY
		return(0);
		}
	fscanf(iop,"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&max,&min,&imax,&fscale,
		&wbase,&wchange,&wgain,&wramp,&sscale);
	for(i=0; i<NCONTINS; i++)
		fscanf(iop,"%lf ",&fweight[i]);
	fscanf(iop,"%lf %lf %lf",&rest[F],&rest[P],&rest[W]);
	fscanf(iop,"%lf %lf %lf",&decay[F],&decay[P],&decay[W]);
	fscanf(iop,"%lf %lf %lf",&pthresh[F],&pthresh[P],&pthresh[W]);
	for (i=0; i<NPARAMS; i++) {
		fscanf(iop,"%lf ",&alpha[i]);
	}
	for (i=0; i<NLEVS; i++) {
		fscanf(iop,"%lf ",&ga[i]);
	}
	fclose(iop);
	return(1);
}

dump(iop)
FILE *iop;
{
	register int i;

	fprintf(iop,"%f %f %f %f %f %f %f %f %f\n",max,min,imax,fscale,
		wbase,wchange,wgain,wramp,sscale);
	for(i=0; i<NCONTINS; i++)
		fprintf(iop,"%f ",fweight[i]);
	fprintf(iop,"\n");
	fprintf(iop,"%f %f %f\n",rest[F],rest[P],rest[W]);
	fprintf(iop,"%f %f %f\n",decay[F],decay[P],decay[W]);
	fprintf(iop,"%f %f %f\n",pthresh[F],pthresh[P],pthresh[W]);
	for (i=0; i<NPARAMS; i++) {
	    fprintf(iop, "%f ",alpha[i]);
	}
	fprintf(iop,"\n");
	for (i=0; i<NLEVS; i++) {
	    fprintf(iop, "%f ",ga[i]);
	}
	fprintf(iop, "\n");
}

lexicon(filename)
char *filename;
{
	FILE *iop;
	char sp[40];
	if ( (iop = fopen(filename,"r")) == NULL)
		{
		sprintf(out,"Can't open %s.",filename);
		errprint(out);
		BACKTOTTY
		return(0);
		}
	nwords = 0;
	while (fgets(in,BUFSIZ,iop) != NULL)
		{
		sscanf(in,"%s %d %s",wordsound[nwords],&wordfreq[nwords],sp);
		nwords++;
		}
	fclose(iop);
	return(1);
}

printphonemes(pmin,pmax,pstep,iop)
int pmin,pmax,pstep; FILE *iop; 
{
	PHONEME *pp;
	register int cy;
	if (pmax ==0) pmax = PSLICES*FPP-1;
	if (pstep == 2) pstep = FPP*2;
	else pstep = FPP;
	fprintf(iop,"\tTRACE ACTIVATIONS at the PHONEME LEVEL (INPUT at %d)\n",
		cycleno);
	fprintf(iop,"Slice->\t");
	for (cy = pmin; cy <= pmax; cy += pstep) {
		fprintf(iop," %2d",cy);
	}
	fprintf(iop,"\n");
	for ALLPHONEMES {
	    for (cy = pmin; cy <= pmax; cy += pstep) {
		if (pp->ex[cy/FPP] > pthresh[P]) {
			break;
		}
	    }
	    if (cy > pmax) continue;
	    fprintf(iop,"%c:\t",pp->sound);
	    for (cy = pmin; cy <= pmax; cy += pstep) {
		if (pp->ex[cy/FPP] > pthresh[P]) {
			fprintf(iop,"%3d",(int) (pp->ex[cy/FPP]*100) );
		}
		else fprintf(iop,"   ");
		if (sigflag) {sigflag = 0; return;}
	    }
	    fprintf(iop,"\n");
	}
}

printwords(pmin,pmax,pstep,iop)
int pmin,pmax,pstep; 
FILE * iop;
{
	WORD **wptr,*wp;
	register int cy;
	if (pmax ==0) pmax = PSLICES*FPP-1;
	if (pstep == 2) pstep = 2*FPP;
	else pstep = FPP;
	fprintf(iop,"ACTIVATIONS of WORD TOKENS ");
	fprintf(iop,"BY STARTING POSITION (INPUT at %d)\n", cycleno);
	fprintf(iop,"Slice->\t");
	for (cy = pmin; cy <= pmax; cy += pstep) {
		fprintf(iop," %2d",cy);
	}
	fprintf(iop,"\n");
	for ALLWORDPTRS {
	    wp = *wptr;
	    for (cy = pmin; cy <= pmax; cy += pstep) {
		if (wp->ex[cy/FPP] > pthresh[W]) {
			break;
		}
	    }
	    if (cy > pmax) continue;
	    if (strlen(wp->sound) < 8) {
	        fprintf(iop,"%s\t",wp->sound);
	    }
	    else if (strlen(wp->sound) == 8) {
	        fprintf(iop,"%s",wp->sound);
	    }
	    else {
		fprintf(iop,"%s\n\t",wp->sound);
	    }
	    for (cy = pmin; cy <= pmax; cy += pstep) {
		if (wp->ex[cy/FPP] > pthresh[W]) {
			fprintf(iop,"%3d",(int) (wp->ex[cy/FPP]*100) );
		}
		else fprintf(iop,"   ");
		if (sigflag) {sigflag = 0; return;}
	    }
	    fprintf(iop,"\n");
	}
}

sfeatacts(infile)
char *infile;
{
	register int i,j,f,cy;
	FILE *iop;
	if ( (iop = fopen(infile,"w")) == NULL)
		{
		sprintf(out,"Can't open %s.",infile);
		errprint(out);
		BACKTOTTY
		return(0);
		}
	for (j = 0; j < NCONTINS; j++) {
		for (f = 0; f < NFVALS; f++) {
		    fprintf (iop,"%7.4f",feature[j][f].ex[fpcyc]);
		}
		fprintf(iop,"\n");
	}
	fclose(iop);
}

topdown() 
{
	WORD **wptr;
	for ALLWORDPTRS {
		(*wptr)->prime = 0;
	}
	while(1) {
getprime:
		getstring("next: ");
		if (in[0] == 'q') return;
		for ALLWORDPTRS {
			if (strcmp((*wptr)->sound,in) == 0)  break;
		}
		if (!*wptr) {
			sprintf(out,"%s not in lexicon.",in);
			errprint(out);
			BACKTOTTY
			goto getprime;
		}
getpval:
		getstring("pval: ");
		if (sscanf(in,"%lf",&((*wptr)->prime)) == 0) {
			sprintf(out,"Illegal prime value for %s.",in);
			errprint(out);
			BACKTOTTY
			goto getpval;
		}
	}
}

setout() 
{
	WORD **wptr;
	PHONEME *pp;

	for allitems(npouts) {
		outphoneme[npouts] = NULL;
getphoneme:
		getstring("phoneme (q to exit): ");
		if (in[0] == 'q') break;
		for ALLPHONEMES {
			if (pp->sound == in[0]) {
			outphoneme[npouts] = pp;
			break;
			}
		}
		if (!outphoneme[npouts]) {
			sprintf(out,"%s not a phoneme.",in);
			errprint(out);
			BACKTOTTY
			goto getphoneme;
		}
		getstring("slice: ");
		sscanf(in,"%d",&outslice[npouts]);
		outslice[npouts] /= FPP;
	}
	for allitems(nwouts) {
		outword[nwouts] = NULL;
getword:
		getstring("word (end with q): ");
		if (in[0] == 'q') break;
		for ALLWORDPTRS {
			if (strcmp((*wptr)->sound,in) == 0) {
				outword[nwouts] = *wptr;
				break;
			}
		}
		if (outword[nwouts] == NULL) {
			sprintf(out,"%s not in lexicon .",in);
			errprint(out);
			BACKTOTTY
			goto getword;
		}
		getstring("slice: ");
		sscanf(in,"%d",&woutslice[nwouts]);
		woutslice[nwouts] /= FPP;
	}
}

summarypr(min,max,step,iop) 
int min,max,step;
FILE *iop;
{
	int cy,c;
	PHONEME *pp;
	WORD *wp, **wptr;
	int last;

	if (min < 0) min = 0;
	max += 1; 
	if (max > FSLICES) max = FSLICES;
	if (step == 0) step = 1;
	if (step != 1) step = 3;
	fprintf(iop,"\t\t\t\tCYCLE %2d\n",cycleno);
	fprintf(iop,"          ");
	for (cy = min; cy < max; cy+=step) {
		fprintf(iop,"%3d",cy);
	}
	fprintf(iop,"\n");
	for ALLWORDPTRS {
	    wp = *wptr;
	    for (cy = min; cy < max; cy += step) {
		if (wp->ex[cy/FPP] > pthresh[W]) {
			break;
		}
	    }
	    if (cy >= max) continue;
	    if (strcmp(wp->sound,"-") == 0) {
		fprintf(iop,"--        ");
	    }
	    else {
	        fprintf(iop,"%-10s",wp->sound);
	    }
	    for (cy = min; cy < max; cy += step) {
		if (wp->ex[cy/FPP] > pthresh[W]) {
			fprintf(iop,"%3d",(int) (wp->ex[cy/FPP]*100) );
		}
		else fprintf(iop,"   ");
	    }
	    fprintf(iop,"\n");
	}
	for ALLPHONEMES {
	    last = 0;
	    for (cy = min; cy < max; cy += step) {
		if (pp->ex[cy/FPP] > pthresh[P]) {
			last = cy;
		}
	    }
	    if (!last) continue;
	    fprintf(iop,"%c         ",pp->sound);
	    for (cy = min; cy <= last; cy += step) {
		if (pp->ex[cy/FPP] > pthresh[P]) {
			fprintf(iop,"%3d",(int) (pp->ex[cy/FPP]*100) );
		}
		else fprintf(iop,"   ");
	    }
	    fprintf(iop,"\n");
	}

	/* DON'T PRINT FEATURE JUNK - JLE 1/15/83
	for (c = 0; c < NCONTINS; c++) {
	  last = 0;
	  for (cy = min; cy < max; cy+=step) {
	    if (fcount[c][cy]) last = cy;
	  }
	  if (last) {
	    fprintf(iop,"%s ct:\t",contname[c]);
	    for (cy = min; cy <= last; cy+=step) {
		    if (!fcount[c][cy]) fprintf(iop,"   ");
		    else fprintf(iop,"%3d",fcount[c][cy]);
	    }
	    fprintf(iop,"\n");
	    fprintf(iop,"%s sm:\t",contname[c]);
	    for (cy = min; cy <= last; cy+=step) {
		    if (!fsum[c][cy]) fprintf(iop,"   ");
		    else fprintf(iop,"%3d",(int) (fsum[c][cy]*100) );
	    }
	    fprintf(iop,"\n");
	  }
	}
	END OF COMMENTED OUT CODE*/
}

/* ROUTINE COMMENTED OUT 10/3/82 -- JLM 
wordsubset() 
{
	WORD *wp, **wptr;
	char *error;
	getstring("subset specification: ");
	if (strcmp(in,"*") == 0) {
		wptr = wordptr;
		for ALLWORDS *wptr++ =  wp;
		*wptr = '\0';
		return;
	}
	if ( ( error = re_comp(in)) != 0) {
		sprintf(out,"%s",error);
		errprint(out);
		BACKTOTTY
		return;
	}
	wptr = wordptr;
	nwptrs = 0;
	for ALLWORDS {
		if (re_exec(wp->sound)) {
			*wptr++ = wp;
			nwptrs++;
		}
	}
	*wptr = '\0';
}
*/

outprob() {
	PHONEME *pp;
	register int p, w,a,b;
	double t, str, denom,nt,ndenom,nstr;
	char tstr[40];
	if (fcflag ==1) {/*standard, luce version*/
	    denom = 0;
	    for (p = 0; p < npouts; p++) {
		    t = rex[p];
		    denom += exp(sscale*t);
	    }
	    if (outfile) fprintf(outfile,"%s",memo);
	    for (p = 0; p < npouts; p++) {
		t = rex[p];
		str = exp(sscale*t)/denom;
		sprintf(tstr," %f %f",str,t);
		strcat(out,tstr);
		if (outfile) fprintf(outfile," %f",str);
	    }
	    if (outfile) fprintf(outfile,"\n");
	}
	else if (fcflag ==2) {/*pick the most active based on running ave*/
		if (rex[0] > rex[1]) {
			a = 1; b = 0;
		}
		else {a=0;b=1;}
		sprintf(tstr," %d %d",a,b);
		strcat(out,tstr);
		if (outfile) fprintf(outfile,"%s %d %d\n",memo,a,b);
	}
	else {
	    for (p = 0; p < npouts; p++) {
		denom = 0;
		for ALLPHONEMES {
			denom += exp(sscale*pp->ex[outslice[p]]);
		}
		str =
		exp(sscale*outphoneme[p]->ex[outslice[p]])/denom;
		sprintf(tstr," %f",str);
		strcat(out,tstr);
		if (outfile) fprintf(outfile,"resp %c %d %d %f\n",
			outphoneme[p]->sound, outslice[p], cycleno, str);
	}   }
}

woutprob() {
	WORD **wptr,*wp;
	register int p, w;
	double str, denom, os;
	char tstr[30];
	for (w = 0; w < nwouts; w++) {
		denom = 0;
		for ALLWORDPTRS {
			wp = *wptr;
			denom += exp(sscale*wp->ex[woutslice[w]]);
		}
		os = outword[w]->ex[woutslice[w]];
		str =
		exp(sscale*outword[w]->ex[woutslice[w]])/denom;
		sprintf(tstr," %f %f",str,os);
		strcat(out,tstr);
		if (outfile) fprintf(outfile,"resp %s %d %d %f %f\n",
			outword[w]->sound, woutslice[w], cycleno,str, os);
	}
}
