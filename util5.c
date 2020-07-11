# include "defs.h"

# define NG_XSCALE 0.1
# define NG_PSCALE 4.0
# define NG_WSCALE 6.0
# define NG_SSCALE 6.0
# define NG_PBASE  1.0
# define NG_WBASE  4.0
# define NG_SBASE  1.0
# define NG_XAXIS   .4
# define NG_YMIN   0.0
# define NG_XMIN   0.0
# define NG_YRANGE 2.8
# define NG_XMAX   10.0
# define NG_YMAX   7.0
# define NG_SMAX   6.0
# define NG_XFUDGE  1
# define NG_RISE   .15
# define NG_XMARGIN .4
# define NG_HATY   .8
# define NG_FMARG  (1.00*NG_XSCALE)
# define NG_XTIC   (0.50*NG_XSCALE)

FILE *ngiop = NULL;

double ng_pscale = NG_PSCALE;
double ng_wscale = NG_WSCALE;
double ng_sscale = NG_SSCALE;
double ng_xmax = NG_XMAX;
int ng_max   = (PSLICES*FPP);

double
ngyval(act,scale,offset) double act, scale, offset; {
	double yup;
	yup = scale*act + offset;
	if (yup > NG_YRANGE + offset) yup = NG_YRANGE + offset;
	return (yup);
}

double
ngxval(cycle,pstep) int cycle, pstep; {
	double xleft;
	xleft = NG_XMARGIN + NG_XSCALE*cycle/FPP;
	return(xleft);
}

newgraph(min,max,step) 
int min,max,step;
{
	getstring("Filename: ");
	if ( (ngiop = fopen(in,"w")) == NULL) {
		return;
	}
	ng_box(max,1,FPP);
	ng_words(0,max,FPP,1,"MAX");
	ng_phonemes(0,max,FPP,1);
	ng_xaxis(0,max,FPP);
	fclose(ngiop);
}


wgraph(min,max,step) 
int min,max,step;
{
	getstring("Filename: ");
	if ( (ngiop = fopen(in,"w")) == NULL) {
		return;
	}
	ng_box(max,0,FPP);
	ng_words(0,max,FPP,0,"MAX");
	ng_xaxis(0,max,FPP);
	fclose(ngiop);
}
ng_phonemes(pmin,pmax,pstep,split)
int pmin,pmax,pstep,split;
{
	PHONEME *pp;
	register int cy;
	double ngy;
	double ngx;
	for ALLPHONEMES {
	    for (cy = pmin; cy <= pmax; cy += FPP*2) {
		if (pp->ex[cy/FPP] > .05) {
			ngy = ngyval(pp->ex[cy/FPP],ng_pscale,NG_PBASE);
			ngx = ngxval(cy,FPP);
			fprintf(ngiop,".to (%f,%f)\n%c\n",ngx, ngy, pp->sound);
		}
	    }
	}
}

ng_words(pmin,pmax,pstep,split,word)
int pmin,pmax,pstep,split; char *word;
{
	WORD **wptr,*wp;
	register int cy;
	int max,one;
	int line, i;
	double ngx, ngy;
	double ngxl, ngxr;
	if (pmax ==0) pmax = PSLICES*FPP-1;
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
	for ALLWORDPTRS {
	    wp = *wptr;
	    if (one && strcmp(wp->sound,word) != 0) {
		continue;
	    }
	    if (max) getmax(wp);

	    for (cy = pmin; cy <= pmax; cy += FPP) {
		if (wp->ex[cy/FPP] > .05 &&
				(!max || wp->ex[cy/FPP]  == wp->max)) {
			if (split) {
			  ngy = ngyval(wp->ex[cy/FPP],ng_wscale,NG_WBASE);
			}
			else {
			  ngy = ngyval(wp->ex[cy/FPP],ng_sscale,NG_SBASE);
			}
			ngx = ngxval(cy,FPP);
			ngxl = ngx - NG_FMARG;
			ngxr = ngx + (strlen(wp->sound)-1)*2*NG_XSCALE 
			       + NG_FMARG;
			fprintf(ngiop,".c %s\n",wp->sound);
			fprintf(ngiop,".set y  = %f\n",ngy);
			fprintf(ngiop,
			    ".frame (%f, y - .75*size) (%f, y + .75*size)\n",
			    ngxl,ngxr);
			fprintf(ngiop,".draw (%f, y) (%f, y)\n",
			    ngxl - NG_XTIC,ngxl);
			fprintf(ngiop,".draw (%f, y) (%f, y)\n",
			    ngxr,ngxr + NG_XTIC);
			for (i = 0; i < strlen(wp->sound);i++) {
				ngx = ngxval(cy +2*i*FPP, FPP);
				fprintf(ngiop,".to (%f,%f)\n%c\n",ngx, ngy,
					wp->sound[i]);
			}
			/*
			ngx = ngxval((cy + (2*i-1)*FPP + NG_XFUDGE),FPP);
			fprintf(ngiop,".to (%f,%f)\n%c\n",ngx, ngy,'<');
			fprintf(ngiop,".to (%f,%f)\n%c\n",ngx, ngy,'>');
			*/
		}
	    }
	}
}

ng_xaxis(pmin,pmax,pstep) int pmin, pmax, pstep; {
	int cy, ph; double ngx;

	for (ph = 0; ph < ninputs; ph++ ) {
		ngx = ngxval(inpeak[ph],FPP);
		if (inspec[ph][0] == 'B') {
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS+NG_RISE,'b');
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS-NG_RISE,'p');
		}
		else if (inspec[ph][0] == 'D') {
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS+NG_RISE,'t');
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS-NG_RISE,'d');
		}
		else if (inspec[ph][0] == 'L') {
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS+NG_RISE,'l');
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS-NG_RISE,'r');
		}
		else if (inspec[ph][0] == 'T') {
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS+NG_RISE,'t');
		   fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS-NG_RISE,'p');
		}
		else fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_XAXIS,inspec[ph][0]);
	}
	if (ngxval(cycleno,FPP) > pmax ) {
		fprintf(ngiop, ".to (%f,%f)\n%c\n",NG_XMAX,
			ngxval(pmax,FPP),'>');
		return;
	}
	ngx = ngxval(cycleno,FPP);
	fprintf(ngiop,".to (%f,%f)\n%c\n",ngx,NG_HATY,'^');
}

ng_box(max,split,step) int max; int step;  {
    ng_xmax = NG_XMARGIN + ngxval(max,FPP);
    if (split) {
      fprintf(ngiop,".frame (%f, %f) (%f, %f)\n",0.0,NG_PBASE,ng_xmax, NG_YMAX);
      fprintf(ngiop,".draw (%f,%f) (%f,%f)\n",0.0,NG_WBASE,ng_xmax,NG_WBASE);
    }
    else {
      fprintf(ngiop,".frame (%f, %f) (%f, %f)\n",0.0,NG_SBASE,ng_xmax, NG_SMAX);
    }
}
