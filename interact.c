#include "defs.h"
/*
 *act_features():
 *	handles input-to-feature excitation
 */	
act_features() 
{
    register double *iptr,*smptr,*fexptr,*nexptr;
    int c;
    FEATURE *fp;

    for (c = 0; c < NCONTINS; c++) {
      for (iptr = ffi[c], smptr = fsum[c]; iptr < ffi[c] + FSLICES;) {
	*iptr++ = (*smptr++)*ga[F];
    } }

    if (cycleno < FSLICES) {
	for (c = 0; c < NCONTINS; c++) {
          for (fp = &feature[c][0],
	       iptr = &input[cycleno][c][0];
	       fp < &feature[c][0] + NFVALS; fp++,iptr++) {
		if ( *iptr ) {
		    fp->nex[cycleno] += fweight[c]*(*iptr);
    }  }  }     }

    for (c = 0; c < NCONTINS; c++) {
       for (fp = &feature[c][0]; fp < &feature[c][0] + NFVALS; fp++) {
	    for (fexptr = fp->fex, nexptr = fp->nex, iptr = ffi[c];
	         fexptr < fp->fex + FSLICES; ) {
		 if (*iptr)
		    *nexptr++ -= (*iptr++ - *fexptr++);
		else
		    nexptr++,iptr++,fexptr++;
    }   }   }
}


ppinteract()
{
	register double *iptr, *ppexptr, *pnexptr, *sumptr,*netptr;
	register int ppeak;
	PHONEME *pp;
	int pmax, pmin, halfdur;
	double *pmaxptr;

	for (iptr = ppi, sumptr = pisum; iptr < ppi + PSLICES; ) {
		*iptr++ = ga[P]*(*sumptr++);
	}

	for ALLPHONEMES {
	    halfdur = pp->pdur/2;
	    for (ppeak=0, pnexptr=pp->nex, ppexptr=pp->pex, netptr=pp->net;
	         ppeak < PSLICES;
		 ppeak++, pnexptr++, ppexptr++, netptr++) {
		*netptr = *pnexptr;
		pmax = ppeak+halfdur;
		if (pmax >= PSLICES) {
		    pmax = PSLICES-1;
		    pmin = ppeak - halfdur;
		}
		else if ( (pmin = ppeak - halfdur) < 0) {
			pmin = 0;
		}
		pmaxptr = &ppi[pmax];
	        for (iptr = (&ppi[pmin]); iptr < pmaxptr;) {
		    *pnexptr -= *iptr++;
		}
		if (*ppexptr) {
			*pnexptr += (pmax-pmin)*(*ppexptr);
		}
	}   }
}

fpinteract()
{
	register double *fpexptr, *wtptr, *pnexptr, *pfetptr, *pendptr;
	register int ftick;
	FEATURE *fp;
	PHONEME *pp, **plptr;
	int pstart, pend;
	int winstart, pfetind;
	int fspr, fmax;
	int c;
	double fpextot, fpexsum;
	double t;

	for (pfetind = 0, fp = &feature[0][0];
	     pfetind < NCONTINS*NFVALS; fp++,pfetind++ ) {
	    fpextot = fp->pextot;
	    for (fpexsum = 0, ftick = 0, fpexptr = &fp->pex[0];
		 fpexsum < fpextot; ftick++, fpexsum += *fpexptr++){
		if (*fpexptr) {
		    for (plptr = fp->phonlist; *plptr; ) {
			pp = *plptr++;
			fspr = fp->spread*pp->wscale;
			fmax = FSLICES - fspr;
			if (ftick < fspr) {
			    pstart = 0;
			    pend = (ftick + fspr - 1)/FPP;
			    /* the divide truncates downward to the
			       preceeding peak except in the case where
			       we are actually right on a point divisible
			       by FPP. subtracting 1 insures that we
			       will always be at the last tick that counts*/
			}
			else {
			    if (ftick > fmax) pend = PSLICES - 1; 
			    else pend = (ftick + fspr -1)/FPP;
			    pstart = (ftick - fspr)/FPP + 1;
			    /* the divide truncates downward to the preceeding
			       tick, or if no remainder to the tick at the 
			       disappearing edge of the window -- adding 1
			       makes certain that we are inside the window */
			}
			winstart = fspr - (ftick - FPP*pstart);
			pendptr = &pp->nex[pend];
			t = pp->feature[pfetind]*(*fpexptr);
			c = pfetind/NFVALS;
			for (pnexptr = &pp->nex[pstart],
			     wtptr = &pp->fpw[c][winstart];
			     pnexptr <= pendptr;
			     wtptr += 3 ) {
			    *pnexptr++ += *wtptr*t;
			}
	}   }   }   }
}

pfinteract() 
{
	register double *fnexptr, *wtptr;
	register double *pfexptr, *pfetptr;
	register double *fendptr;
	register int ppeak, fpeak;
	FEATURE *fp;
	PHONEME *pp;
	int fspr,fend,fbegin,c;
	int winstart;
	double pfextot, pfexsum;
	double t;


	for ALLPHONEMES {
	    pfextot = pp->fextot;
	    for (pfexsum = 0, ppeak=0, pfexptr=pp->fex; pfexsum < pfextot;
		 ppeak++, pfexsum += *pfexptr++) {
		fpeak = ppeak*FPP;
		if (*pfexptr) {
		  for (c = 0; c < NCONTINS; c++) {
		    for (pfetptr = &pp->feature[c*NFVALS], fp = &feature[c][0];
		         fp < &feature[c][0] + NFVALS; fp++,pfetptr++ ) {
		    if (*pfetptr) {
			fspr = fp->spread*pp->wscale;
			fbegin = 1+fpeak-fspr;
			if (fbegin < 0) {
			    winstart = 1 - fbegin;
			    fbegin = 0;
			    fend = fpeak+fspr;
			}
			else {
			    if ( (fend = fpeak + fspr) > FSLICES-1) {
				fend = FSLICES-1;
			    }
			    winstart = 1;
			}
			fendptr = &fp->nex[fend];
			t = (*pfexptr)*(*pfetptr);
			for (wtptr = &pp->pfw[c][winstart],
			     fnexptr= &fp->nex[fbegin];
			     fnexptr < fendptr;) {
			       *fnexptr++ += (*wtptr++)*t;
			}
	} } } } }   }
}

wwinteract()
{
	register double *iptr, *sumptr, *wnexptr, *wwexptr;
	register int wstart; 
	WORD *wp, **wptr;
	int wmin, wmax;
	double *wmaxptr;

	for (iptr = wwi, sumptr = wisum; iptr < wwi + PSLICES; ) {
	    if (*sumptr > imax) *sumptr = imax;
	    *iptr++ = ga[W]*(*sumptr++);
	}

	for ALLWORDPTRS {
	    wp = *wptr;
	    for (wstart = 0, wnexptr = wp->nex, wwexptr = wp->wex;
	         wstart < PSLICES; 
		 wstart++, wnexptr++, wwexptr++) {
	        wmin = wstart + wp->start;
	        if (wmin < 0) wmin = 0;
	        wmax = wstart + wp->end;
	        if (wmax > PSLICES-1) wmax = PSLICES-1;
	        wmaxptr = &wwi[wmax];
	        for (iptr = &wwi[wmin]; iptr < wmaxptr; ) {
		    *wnexptr -= *iptr++;
	        }
	        if (*wwexptr) {
		    *wnexptr += (wmax - wmin)*(*wwexptr);
	        }
	    }
	}
}

pwinteract() 
{
	register double *pwexptr, *wnexptr, *wtptr;
	WORD *wp, **wptr;
	PHONEME *pp;
	register int wpeak,pslice;
	int pdur,winstart,wmin, wmax;
	int *ofstptr;
	double *wmaxptr;
	double pwextot, pwexsum;
	double t;

	for ALLPHONEMES {
	    pwextot = pp->wextot;
	    pdur = pp->pdur;
	    for (pwexsum = 0, pslice = 0, pwexptr = pp->wex;
		 pwexsum < pwextot; pwexsum += *pwexptr++, pslice++) {
		if (*pwexptr) {
		  for (wptr = pp->wordlist, ofstptr = pp->ofstlist; *wptr; ) {
		    wp = *wptr++;
		    wpeak = pslice - *ofstptr++;
		    if (wpeak < -pdur ) continue;
		    if ( (wmin = 1 + wpeak - pdur) < 0) {
			winstart = 1 - wmin;
			wmin = 0;
			wmax = wpeak + pdur;
		    }	
		    else {
			if ( (wmax = wpeak + pdur) > PSLICES-1) {
			    wmax = PSLICES-1;
			}
			winstart = 1;
		    }
		    wmaxptr = &wp->nex[wmax];
		    t = wp->scale*(*pwexptr);
		    for ( wnexptr = &wp->nex[wmin],
			  wtptr = &pp->pww[winstart];
			  wnexptr < wmaxptr;) {
			    *wnexptr++ += *wtptr++*t;
	}   }   } } }
}

wpinteract()
{
	register double *pnexptr, *wpexptr, *wtptr;
	WORD *wp, **wptr;
	PHONEME *pp;
	register int wstart, wslot; 
	register char *t_c_p;
	register int *t_o_p;
	int pdur,pwin,pmin, pmax;
	double *pmaxptr;
	double wpextot, wpexsum;

	for ALLWORDPTRS {
	    wp = *wptr;
	    wpextot = wp->pextot;
	    for (wpexsum = 0, wstart = 0, wpexptr = wp->pex;
		 wpexsum < wpextot; wstart++, wpexsum += *wpexptr++) {
	      if (*wpexptr) {
		for (t_c_p = wp->sound, t_o_p = wp->offset; *t_c_p;) {
		    wslot = wstart+*t_o_p++;
		    pp = phonptr[*t_c_p++];
		    pdur = pp->pdur;
		    pmin = 1 + wslot - pdur;
		    if (pmin >= PSLICES) break;
		    if (pmin < 0) {
			pwin = 1 - pmin;
			pmin = 0;
			pmax = wslot + pdur;
			pmaxptr = &pp->nex[pmax];
		    }
		    else {
			if ( (pmax = wslot + pdur) > PSLICES-1) {
			    pmax = PSLICES-1;
			}
			pwin = 1;
			pmaxptr = &pp->nex[pmax];
		    }
		    for ( pnexptr = &pp->nex[pmin],
			  wtptr = &pp->wpw[pwin];
			  pnexptr < pmaxptr;) {
			    *pnexptr++ += (*wtptr++)*(*wpexptr);
		    }
    }   }    }  }
}

fupdate()
{
	register double *exptr, *nexptr, *pexptr, *fexptr;
	register double *sumptr;
	register int *cntptr;
	FEATURE *fp;
	int c,f;
	double t, tt;
	double *fpextotptr;

	for (sumptr = fsum[0], cntptr = fcount[0];
	     sumptr < fsum[0] + NCONTINS*FSLICES;) {
		*sumptr++ = *cntptr++ = 0;
	}

	for (c = 0; c < NCONTINS; c++)
	    for (f = 0; f < NFVALS; f++) {
		fp = &feature[c][f];
		fpextotptr = &fp->pextot;
		*fpextotptr = 0;
		for (sumptr = fsum[c], cntptr = fcount[c], exptr = fp->ex,
		     nexptr = fp->nex, pexptr = fp->pex, fexptr = fp->fex; 
		     sumptr < fsum[c] + FSLICES;) {
		    t = *exptr;
		    if (inoise != 0.0) *nexptr += inoise*gnoise();
		    if ( *nexptr > 0) {
		        t += (max - t)*(*nexptr);
		    }
		    else if (*nexptr < 0) {
		        t += (t - min)*(*nexptr);
		    }
		    if ( (tt = *exptr - rest[F]) ) {
			t -= decay[F]*tt;
		    }
		    if (t > 0) {
			    if (t > max) t = max;
			    *sumptr++ += t;
			    *cntptr++ += 1;
			    *fpextotptr += *pexptr++ = t*alpha[FP];
			    *fexptr++ = t*ga[F];
		    }
		    else {
			    if (t < min) t = min;
			    *pexptr++ = 0;
			    *fexptr++ = 0;
			    cntptr++; sumptr++;
		    }
		    *exptr++ = t;
		    *nexptr++ = 0;
	    }
	}
}

pupdate() 
{
	register double *exptr, *nexptr, *fexptr, *pexptr, *wexptr;
	register double *sumptr, *isumptr;
	register int *countptr;
	register int ppeak;
	PHONEME *pp;
	int halfdur;
	int pmin, pmax;
	double *pmaxptr;
	double *pfextotptr;
	double *pwextotptr;
	double t,tt;

	for (sumptr = psum, isumptr = pisum, countptr = pcount;
		sumptr < psum + PSLICES;) {
	    *sumptr++ = *isumptr++ = *countptr = 0;
	}

	for ALLPHONEMES {
		halfdur = (pp->pdur)/2;
		pfextotptr = &pp->fextot;
		pwextotptr = &pp->wextot;
		*pfextotptr = 0;
		*pwextotptr = 0;
		for (ppeak = 0,
		     exptr = pp->ex, nexptr = pp->nex, fexptr = pp->fex,
		     pexptr = pp->pex, wexptr = pp->wex, sumptr = psum,
		     countptr = pcount;
		     ppeak < PSLICES; ppeak++) {
			t = *exptr;
		 	if (inoise != 0.0) *nexptr += inoise*gnoise();
			if (*nexptr > 0) {
			    t += (max - t)*(*nexptr);
			}
			else if (*nexptr < 0) {
			    t += (t - min)*(*nexptr);
			}
			if ( (tt = *exptr - rest[P]) ) {
			    t -= decay[P]*tt;
			}
			if (t > 0) {
			    if (t > max) t = max;
			    *pwextotptr += *wexptr++ = alpha[PW]*(t);
			    *pexptr++ = ga[P]*(t);
			    *pfextotptr += *fexptr++ = alpha[PF]*(t);
			    *sumptr++ += t;
			    *countptr++ += 1;
			    if (ppeak < halfdur) pmin = 0;
			    else pmin = ppeak - halfdur;
			    pmax = ppeak+halfdur;
			    if (pmax > PSLICES-1) pmax = PSLICES-1;
			    pmaxptr = &pisum[pmax];
			    for (isumptr = &pisum[pmin]; isumptr<pmaxptr;) {
				*isumptr++ += t;
			    }
			}
			else {
				if (t < min) t = min;
				*wexptr++ = *pexptr++ = *fexptr++ = 0;
				sumptr++; countptr++;
			}
			*exptr++ = t;
			*nexptr++ = 0;
		}
	}
}

wupdate() 
{
	register double *exptr, *nexptr, *pexptr, *wexptr;
	register double *sumptr, *isumptr;
	register int *countptr;
	register int wstart;
	WORD *wp, **wptr;
	int wmin, wmax;
	double *wpextotptr;
	double *wmaxptr;
	double t, tt;

	for (sumptr = wsum, isumptr = wisum, countptr = wcount;
		sumptr < wsum + PSLICES; ) {
	    *sumptr++ = *isumptr++ = *countptr++ = 0;
	}

	for ALLWORDPTRS {
	    wp = *wptr;
	    wpextotptr = &wp->pextot;
	    *wpextotptr = 0;
	    for (wstart = 0,
		 exptr = wp->ex, nexptr = wp->nex, pexptr = wp->pex,
		 wexptr = wp->wex, sumptr = wsum, countptr = wcount; 
		 wstart < PSLICES; wstart++) {
		t = *exptr;
		if (inoise != 0.0) *nexptr += inoise*gnoise();
		if ( *nexptr > 0) {
		    t +=  (max - t)*(*nexptr);
		}
		else if (*nexptr < 0) {
		    t +=  (t - min)*(*nexptr);
		}
		if ( (tt = *exptr - wp->rest) ) {
		    t -= decay[W]*tt;
		}
		if (t > 0) {
		    if (t > max) t = max;
		    *wpextotptr += *pexptr++ = t*alpha[WP];
		    tt = t * t;
		    *wexptr++ = tt*ga[W];
		    *countptr++ += 1;
		    *sumptr++ += t;
		    wmin = nonneg(wstart + wp->start);
		    wmax = wp->end + wstart;
		    if (wmax > PSLICES-1) wmax = PSLICES-1;
		    wmaxptr = &wisum[wmax];
		    for (isumptr = &wisum[wmin]; isumptr < wmaxptr;) {
			*isumptr++ += tt;
		    }
		}
		else {
		    if (t < min) t = min;
		    *wexptr++ = *pexptr++ = 0;
		    countptr++;  sumptr++;
		}
		*exptr++ = t;
		*nexptr++ = 0;
	}
    }
}
