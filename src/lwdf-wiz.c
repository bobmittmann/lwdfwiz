/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	lwdf-wiz.c
 * Module:
 * Project:	lwdfwiz
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2021 Robinson Mittmann. All Rights Reserved.
 *
 * LWDFWiz is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "lwdf.h"

#define LWDF_NMAX (LWDF_ORDER_MAX) 

int readln(char * buf, unsigned int max);

int input_double(const char * prompt, double * pval)
{
	char s[32];
	double x;
	int ret;

	printf(prompt);
	fflush(stdout);
	x = *pval;
	sprintf(s, "%g", x);
	ret = readln(s, sizeof(s));

	if (ret < 0)
		return ret;

	if (ret == 0)
		return 0;

	if (s[0] == '\n' || s[0] == '\0')
		return -1;

	x = strtod(s, NULL);
	*pval = x;
	return 1;
}

int input_int(const char * prompt, int * pval)
{
	char s[32];
	int x;
	int ret;

	printf(prompt);
	fflush(stdout);
	sprintf(s, "%d", *pval);

	ret = readln(s, sizeof(s));

	if (ret < 0)
		return ret;

	if (ret == 0)
		return 0;

	if (s[0] == '\n' || s[0] == '\0')
		return -1;

	x = strtol(s, NULL, 0);
	*pval = x;
	return 1;
}

int lwdfwiz_term(struct lwdfwiz_param * wiz, struct lwdf_info * inf)
{
	char s[1024];
	int ftype;
	int i, i1, i2, bi = 0;
	int ff;
	int id = 0, rx, Nmin, N, nbits;

	/* filter specs and design */
	double F;
	double asmin;
	double as;
	double es, fs, phis, ap, ep, fp, phip, t, tt;	
	/* filter order computation */
	double k0, k1, k4, c1, c2, c3;	
	/* Butterworth coefficient computation */
	double ks, kp, g;	
	/* Chebyshev */
	double w, r, Ai, Bi;	
	double epmin;
	double epmax;
	/* Elliptic/Cauer */
	double fsmin;
	double fsmax;
	double q0, q1, q2, q3, q4, m3, m2, m1, m0; 
	/* filter coefficients */
	double gamma[LWDF_NMAX];	

	/* Get default values ... */
	ftype = wiz->ftype; 
	F = wiz->samplerate; 
	nbits = wiz->nbits;
	bi = wiz->bi ? 1 : 0; 
	id = wiz->id ? 1 : 0; 
	rx = wiz->rx ? 1 : 0; 
	as = wiz->as; 
	asmin = wiz->asmin; 
	es = wiz->es; 
	fs = wiz->fs; 
	ap = wiz->ap; 
	ep = wiz->ep; 
	fp = wiz->fp; 
	ff = wiz->ff; 

	printf("\n");
	printf("+------------------------------------------------------------+\n");
	printf("| LWDF Wizard                                                |\n");
	printf("|                                                            |\n");
    printf("|  Explicit Formulas for Lattice Wave Digital Filters,       |\n");
	printf("|                              Lajos Gazsi, 1985 IEEE        |\n");
	printf("|                                                            |\n");
	printf("+------------------------------------------------------------+\n");
	printf("\n");
	printf(" %d) Butterworth\n", 
		   LWDF_BUTTW);
	printf(" %d) Chebyshev I\n", 
		   LWDF_CHEB1);
	printf(" %d) Elliptic\n", 
		   LWDF_ELLIP);

	do {
		input_int(" - <ft> filter type? ", &ftype);
	} while ((ftype > 3) || (ftype < 1));

	if (ftype == LWDF_ELLIP) {
		do {
			input_int(" - <bi> bireciprocal filter (0=no 1=yes)? ", &bi);
		} while (bi > 1);
	}

	do {
		input_double(" - < F> sampling frequency [Hz]? ", &F);
	} while (F <= 0.0);

	do {
		input_double(" - <as> stopband min attenuation [dB]? ", &asmin);
	} while ((asmin > 200.0) || (asmin <= 0.0));

	as = asmin;
	/* as = 10.0*log(1.0+es*es) / log(10.0); */
	es = sqrt(exp(log(10.0) * (as / 10.0)) - 1.0);	
	
	do {
		input_double(" - <fs> stopband lower edge [Hz]? ", &fs);
	} while ((fs >= F/2.0) || (fs <= 0));

	/* stopband transformed frequency */
	phis = tan(M_PI * fs / F);
	if ((ftype == LWDF_ELLIP) && (bi)) {
		ep = 1.0 / es;
		phip = 1.0 / phis;

		if (phis < 1) {
			fprintf(stderr, "#error: phis=%f, invalid fs=%f\n", phis, fs);
			return -1;
		}

		/* ap set below after epmin <= ep is chosen */
		fp = (F / 2.0) - fs;	
		printf(" ... passband upper edge %.1f [Hz]\n", fp);
	} else {
		do {
			input_double(" - <ap> passband attenuation spread [dB]? ", &ap);
		} while (ap > 100.0);

		/* as = 10.0*log(1.0+es*es) / log(10.0); */
		ep = sqrt(exp(log(10.0) * (ap / 10.0)) - 1.0);	

		do {
			input_double(" - <fp> passband upper edge [Hz]? ", &fp);
		} while (fp >= F/2.0);

		/* passband transformed frequency */
		phip = tan(M_PI * fp / F);	
	}

	if (isnan(phis) || isnan(phip) || isnan(es)) {
		fprintf(stderr, "#error: phis=%f, phip=%f, es=%f\n", phis, phip, es);
		return -2;
	}

	k0 = sqrt(phis / phip);

	if (isnan(k0)) {
		fprintf(stderr, "#error: k0=%f = sqrt(phis=%f / phip=%f) !!!\n", 
				k0, phis, phip);
		return 1;
	}

	k1 = k0 * k0 + sqrt(k0 * k0 * k0 * k0 - 1.0);

	for (k4 = k1, i = 0; i < 3; i++)
		k4 = k4 * k4 + sqrt(k4 * k4 * k4 * k4 - 1.0);

	if (isnan(k0) || isnan(k1) || isnan(k4)) {
		fprintf(stderr, "#error: k0=%f, k1=%f, k4=%f\n", k0, k1, k4);
		return 1;
	}

	switch (ftype) {
	case LWDF_BUTTW:
	default:
		c1 = 1.0;
		c2 = 1.0;
		c3 = k0 * k0;
		break;
	case LWDF_CHEB1:
		c1 = 1.0;
		c2 = 2.0;
		c3 = k1;
		break;
	case LWDF_ELLIP:
		c1 = 8.0;
		c2 = 4.0;
		c3 = 2.0 * k4;
		break;
	}

	if (isnan(c1) || isnan(c2) || isnan(c3)) {
		fprintf(stderr, "#error: c1=%f, c2=%f, c3=%f\n", c1, c2, c3);
		return 1;
	}

	Nmin = (int)ceil(c1 * log(c2 * es / ep) / log(c3));

	if ((Nmin % 2) == 0)
		Nmin++;		/* odd min order */

	if (Nmin < 1) {
		fprintf(stderr, "#error: Nmin=%d\n", Nmin);
		return -1;
	}

	N = Nmin;
	do {
		sprintf(s, " - < N> order (min=%d, max=%d, must be odd)? ", 
				Nmin, LWDF_NMAX);
		input_int(s, &N);
	} while ((N > LWDF_NMAX) || (N < Nmin));
	if (N > LWDF_NMAX)
		N = LWDF_NMAX;
	if ((N % 2) == 0)
		return 0;	/* if even, code would not be correct */

	do {
		input_int(" - <ff> flip frequency response around F/4 (0=no 1=yes)? ",
				  &ff);
	} while ((ff > 1) || ((ff < 0) ));

	for (i = 0; i < LWDF_NMAX; i++)
		gamma[i] = 0.0;

	switch (ftype) {
	case LWDF_BUTTW:
		ks = (exp(log(es * es) * (1.0 / N)) -
		      phis * phis) / (exp(log(es * es) * (1.0 / N)) +
				      phis * phis);
		kp = (exp(log(ep * ep) * (1.0 / N)) -
		      phip * phip) / (exp(log(ep * ep) * (1.0 / N)) +
				      phip * phip);
		if ((ks <= 0.0) && (kp >= 0.0))
			printf(" ... set g = 0 (half coeffs 0, bireciprocal)\n");
		t = (log(fabs(ks)) / log(2.0));
		if (ks < 0.0)
			t = -t;
		i1 = ceil(t);
		t = (log(fabs(kp)) / log(2.0));
		if (kp < 0.0)
			t = -t;
		i2 = floor(t);

		g = ks + (kp - ks) / 2.0;

		for (i = i1; i <= i2; ++i) {
			double q = exp(log(2.0) * (double)i);
			printf(" ... set g = 2^%2d = %g (half coeffs, no mul)\n", i, q);
		}

		do {
			sprintf(s, " - < g> (%g better passband to %g better stopband)? ", ks, kp);
			input_double(s, &g);
		} while ((g < ks) || (g > kp));

		if (g == 0.0) {	
			/* Butterworth bireciprocal */
			bi = 1;
			gamma[0] = 0.0;
			for (i = 1; i <= ((N - 1) / 2); i++) {
				t = tan(M_PI * (double)i / 2.0 / (double)N);
				gamma[i * 2 - 1] = -(t * t);
				gamma[i * 2] = 0.0;
			}
		} else {	
			/* Butterworth */
			bi = 0;
			t = sqrt(1.0 - g * g);
			gamma[0] = (1.0 + g - t) / (1.0 + g + t);
			for (i = 1; i <= ((N - 1) / 2); i++) {
				tt = t * cos(M_PI * (double)i / (double)N);
				gamma[i * 2 - 1] = (tt - 1.0) / (tt + 1.0);
				gamma[i * 2] = g;
			}
		} 
		break;

	case LWDF_CHEB1:
		for (t = 1.0, i = 0; i < N; t = t * k1, i++);
		epmin = (2.0 * es) / t;
		epmax = ep;

		ep = epmin + (epmax - epmin) / 2.0;
		do {
			sprintf(s, " - <ep> (%f better passband to %f better stopband)? ", 
					epmin, epmax);
			input_double(s, &ep);
		} while ((ep < epmin) || (ep > epmax));

		ap = 10.0 * log(1.0 + ep * ep) / log(10.0);
		w = exp(log(((1.0 / ep) + sqrt(1.0 / (ep * ep) + 1.0))) *
			(1.0 / (double)N));
		r = (w - 1.0 / w) * phip;
		gamma[0] = (2.0 - r) / (2.0 + r);
		for (i = 1; i <= ((N - 1) / 2); i++) {
			t = M_PI * (double)i / (double)N;
			Ai = r * cos(t);
			Bi = (w * w + 1.0 / (w * w) -
			      2.0 * cos(2.0 * t)) * (phip * phip) / 4.0;
			gamma[i * 2 - 1] = (Ai - Bi - 1.0) / (Ai + Bi + 1.0);
			gamma[i * 2] = (1.0 - Bi) / (1.0 + Bi);
		} 
		break;

	case LWDF_ELLIP:
		if (bi)
			r = es;
		else
			r = sqrt(es / ep);
		r = r * r + sqrt(r * r * r * r - 1.0);
		r = r * r + sqrt(r * r * r * r - 1.0);
		t = (0.5) * exp(log(2.0 * r) * (4.0 / (double)N));
		for (i = 0; i < 4; i++)
			t = sqrt((0.5) * (t + 1 / t));
		if (bi)
			fsmin = (F / M_PI) * atan(t);
		else
			fsmin = (F / M_PI) * atan(phip * t * t);
		fsmax = fs;
		do {
			sprintf(s, " - <fs> (%.0f better transition to"
					" %.0f better pass&stopband)? ", 
					fsmin, fsmax);
			input_double(s, &fs);
		} while ((fs < fsmin) || (fs > fsmax));

		phis = tan(M_PI * fs / F);
		if (bi)
			q0 = phis;
		else
			q0 = sqrt(phis / phip);
		q1 = q0 * q0 + sqrt(q0 * q0 * q0 * q0 - 1.0);
		q2 = q1 * q1 + sqrt(q1 * q1 * q1 * q1 - 1.0);
		q3 = q2 * q2 + sqrt(q2 * q2 * q2 * q2 - 1.0);
		q4 = q3 * q3 + sqrt(q3 * q3 * q3 * q3 - 1.0);
		m3 = (0.5) * exp(log(2.0 * q4) * ((double)N / 2.0));
		m2 = sqrt((0.5) * (m3 + 1.0 / m3));
		m1 = sqrt((0.5) * (m2 + 1.0 / m2));
		m0 = sqrt((0.5) * (m1 + 1.0 / m1));

		if (isnan(m0) || isnan(m1) || isnan(m2) || isnan(m3)) {
			fprintf(stderr, "#error: m0=%f, m1=%f, m2=%f, m3=%f\n",
					m0, m1, m2, m3);
			return 1;
		}

		if (bi) {
			epmin = 1.0 / m0;
		} else {
			epmin = es / (m0 * m0);
		}

		if (isnan(epmin) || epmin == 0.0) {
			fprintf(stderr, "#error: epmin=%f\n", epmin);
			return -1;
		}

		epmax = ep;
		if (epmin > epmax) {
			fprintf(stderr, "#error: epmin=%f > epmax=%f !!! ", epmin, epmax);
			return -1;
		}

		ep = epmin + (epmax - epmin) / 2.0;
		do {
			sprintf(s, " - <ep> (%f better passband to %f better stopband)? ", 
					epmin, epmax);
			input_double(s, &ep);
		} while ((ep < epmin) || (ep > epmax));


		ap = 10.0 * log(1.0 + ep * ep) / log(10.0);
		if (bi)
			es = m0;
		else
			es = ep * m0 * m0;
		as = 10.0 * log(1.0 + es * es) / log(10.0);
		g = 1.0 / ep + sqrt(1.0 / (ep * ep) + 1.0);
		g = m1 * g + sqrt((m1 * g * m1 * g) + 1.0);
		g = m2 * g + sqrt((m2 * g * m2 * g) + 1.0);
		t = exp(log((m3 / g) + sqrt((m3 * m3 / g / g) + 1.0)) *
			(1.0 / (double)N));
		t = 1.0 / (2.0 * q4) * (t - 1.0 / t);
		t = 1.0 / (2.0 * q3) * (t - 1.0 / t);
		t = 1.0 / (2.0 * q2) * (t - 1.0 / t);
		t = 1.0 / (2.0 * q1) * (t - 1.0 / t);
		if (bi)
			w = -1.0;
		else
			w = 1.0 / (2.0 * q0) * (t - 1.0 / t);
		if (bi)
			gamma[0] = 0;
		else
			gamma[0] =
			    (1.0 + w * q0 * phip) / (1.0 - w * q0 * phip);
		for (i = 1; i <= ((N - 1) / 2); i++) {
			t = q4 / sin((double)i * M_PI / (double)N);
			t = 1.0 / (2.0 * q3) * (t + 1.0 / t);
			t = 1.0 / (2.0 * q2) * (t + 1.0 / t);
			t = 1.0 / (2.0 * q1) * (t + 1.0 / t);
			t = 1.0 / (2.0 * q0) * (t + 1.0 / t);
			tt = 1.0 / t;
			if (bi) {
				Bi = 1.0;
				Ai = 2.0 / (1.0 + tt * tt) * sqrt(1.0 -
								  (q0 * q0 +
								   1.0 / (q0 *
									  q0) -
								   tt * tt) *
								  tt * tt);
				gamma[i * 2 - 1] = (Ai - 2.0) / (Ai + 2.0);
				gamma[i * 2] = 0.0;
			} else {
				t = (1.0 + w * w * tt * tt);
				Bi = (w * w +
				      tt * tt) / t * (q0 * phip * q0 * phip);
				Ai = (-2.0 * w * q0 * phip) / t * sqrt(1.0 -
								       (q0 *
									q0 +
									1.0 /
									(q0 *
									 q0) -
									tt *
									tt) *
								       tt * tt);
				gamma[i * 2 - 1] =
				    (Ai - Bi - 1.0) / (Ai + Bi + 1.0);
				gamma[i * 2] = (1.0 - Bi) / (1.0 + Bi);
			}
		}
		break;
	}

	if (ff) {
		/* flip frequency response around F/4 */
		for (i = 0; i < N; i += 2)
			gamma[i] = -gamma[i];
	}

	if (bi) {
		do {
			input_int(" - <id> bireciprocal decimation/interpolation (0=no 1=yes)? ", 
					  &id);
		} while ((id > 1) || ((id < 0) ));
	}

	do {
		input_int(" - <nb> bits (not including sign bit, 0=no quantization)? ", 
				  &nbits);
	} while ((nbits > 31) || ((nbits < 0) ));

	do {
		input_int(" - <rx> reuse and minimize temporary variables (0=no, 1=yes)? ", 
				  &rx);
	} while ((rx > 1) || ((rx < 0) ));

	wiz->samplerate = F;
	wiz->ftype = ftype;
	wiz->order = N;
	wiz->nbits = nbits;
	wiz->bi = bi == 1 ? true : false;
	wiz->id = id == 1 ? true : false;
	wiz->rx = rx == 1 ? true : false;
	wiz->asmin = asmin; 
	wiz->as = as;
	wiz->es = es;
	wiz->fs = fs;
	wiz->ap = ap;
	wiz->ep = ep;
	wiz->fp = fp;
	wiz->ff = ff;

	for (i = 0; i < LWDF_NMAX; i++)
		inf->gamma[i] = gamma[i];

	return N;
}
