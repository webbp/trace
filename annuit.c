#include <math.h>

main(argc,argv) int argc; char **argv; {

	double rate, payout, infl,left,thispay,raise,invest;
	int y,ay,sy;
	sscanf(argv[1],"%lf",&rate);
	sscanf(argv[2],"%lf",&invest);
	sscanf(argv[3],"%lf",&infl);
	sscanf(argv[4],"%lf",&raise);
	sscanf(argv[5],"%d",&ay);
	
	left = 0;
	for (sy = 1; sy <= ay; sy++) {
		left = left + left*rate + 
			invest*pow((1.0+infl+raise),(double)(sy-1));
	}
	for (y = 1; ; sy++,y++) {
		thispay = 10*pow((1.0+infl),(double)sy);
		left = left*(1+rate)-thispay;
		if (left < thispay*(1.0+infl)) break;
	}
	printf("Payout will last %d years with remainder of %f\n",
						y,left/thispay*(1.0+infl));
}
