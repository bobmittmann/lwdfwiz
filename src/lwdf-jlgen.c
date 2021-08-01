/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	lwdf-jlgen.c
 * Project:	lwdfwiz
 * Comment: Generate filter for Julia language
 * Author:	Robinson Mittmann (bobmittmann@gmail.com)
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
#include "lwdf.h"

#define LWDF_NMAX (LWDF_ORDER_MAX) 

static double alpha(double g)
{
	double a;

	if (g > 0.5) {
		a = 1.0 - g;
	} else if (g > 0.0) {
		a = g;
	} else if (g > -0.5) {
		a = -g;
	} else {
		a = 1.0 + g;
	}

	return a;
}

static void adaptor(int i, int k, int t, double g, char *in1, char *in2,
	     char *out1, char *out2, FILE * f, int nbits)
{
	char pm = '+';
	int si = 0;
	int so = 0;
	int Q = 1 << nbits;
	int b;
	int aQ;
	double a;

	fprintf(f, "\n");
	fprintf(f, "\t# adaptor %d: g = %.8f\n ", i, g);

	if (g == 0.0) {
		fprintf(f, "\t%2s%-3s=%-7s;%28s%-3s=%-7s\n", "", out1, in2, "",
			out2, in1);
		return;
	}

	if (g > 0.5) {
		a = 1.0 - g;
	} else if (g > 0.0) {
		a = g;
		si = so = 1;
	} else if (g > -0.5) {
		a = -g;
		so = 1;
		pm = '-';
	} else {
		a = 1.0 + g;
		si = 1;
		pm = '-';
	}

	aQ = (int)(a * (double)Q);
	for (b = 0; (b < nbits) && (aQ != (1 << b)); b++);
	b = nbits - b;

	if (si)
		fprintf(f, "\tt%d = %s - %s\n", t, in2, in1);
	else
		fprintf(f, "\tt%d = %s - %s\n", t, in1, in2);

	if (nbits == 0) {
		if (so)
			fprintf(f, "\t%s = a[%d] * t%d %c %s\n", out1, k, t, pm, in2);
		else
			fprintf(f, "\t%s = a[%d] * t%d %c %s\n", out2, k, t, pm, in2);
	} else {
		if (b) {
			if (so)
				fprintf(f, "\t%s = %7s(t%d>>%d) %c %s\n", 
						out1, "", t, b, pm, in2);
			else
				fprintf(f, "\t%s = %7s(t%d>>%d) %c %s\n", 
						out2, "", t, b, pm, in2);
		} else {
			if (so)
				fprintf(f, "\t%s = ((%d*t%d)>>%d) %c %s\n", 
						out1, aQ, t, nbits, pm, in2);
			else
				fprintf(f, "\t%s = ((%d*t%d)>>%d) %c %s\n", 
						out2, aQ, t, nbits, pm, in2);
		}
	}
	if (so)
		fprintf(f, "\t%s = %s - t%d\n", out2, out1, t);
	else
		fprintf(f, "\t%s = %s - t%d\n", out1, out2, t);
	return;
}

int lwdf_jlgen(FILE *fout, const char * prefix, 
			   const struct lwdfwiz_param * wiz, 
			   const struct lwdf_info * inf)
{
	char in1[16], in2[16], out1[16], out2[16];
	const char *dtype;
	int N, nbits;
	int ftype;
	double F;
	bool bi;
	bool id;
	int rx;
	int i, j, k;
	int rl[LWDF_NMAX];

	N = wiz->order;
	F = wiz->samplerate;
	ftype = wiz->ftype;

	/* bireciprocal filter (0=no 1=yes)? */
	bi = wiz->bi;
	if (bi) {
		/* bireciprocal for decimation/interpolation (0=no, 1=yes)? */
		id = wiz->id;
	} else {
		id = false;
	}
	/* bits (not including sign bit, 0=no quantization)? */
	nbits = wiz->nbits;
	/* reuse and minimize temporary variables (0=no, 1=yes)?  */
	rx = wiz->rx ? 1 : 0;

	if (nbits == 0)
		dtype = "Float32";
	else
		dtype = "Int32";

	fprintf(fout, "# -----------------------------------------------------\n");
	fprintf(fout, "#   This file was automatically generated, do not edit!\n");
	fprintf(fout, "#");

	switch (ftype) {
	case LWDF_BUTTW:
		fprintf(fout, "#  Buttworth order %d\n", N);
		break;
	case LWDF_CHEB1:
		fprintf(fout, "#  Chebyshev I order %d\n", N);
		break;
	case LWDF_ELLIP:
		fprintf(fout, "# Elliptic/Cauer order %d\n", N);
		break;
	}

	if (bi) {
		fprintf(fout, "# - bireciprocal");
		if (id)
			fprintf(fout, ", for interpolation/decimation");
		fprintf(fout, "\n");
	} 
	fprintf(fout,
		"# - stopband min attenuation as=%.0f dB at fs=%.2f%% (100%%=F/2)\n",
		wiz->as, wiz->fs / (F / 2.0));
	fprintf(fout,
		"# - passband attenuation spread ap=%.2f dB at fp=%.2f%% (100%%=F/2)\n",
		wiz->ap, wiz->fp / (F / 2.0));

	fprintf(fout, "#\n");
	fprintf(fout, "# -----------------------------------------------------\n");
	fprintf(fout, "\n");

	fprintf(fout, "function %sfilter(st::Vector{%s}, i1::%s, i2::%s)\n", 
			prefix, dtype, dtype, dtype);

	k = 1;
	fprintf(fout, "\ta = %s[\n", dtype);
	for (i = 0; i < N; i++) {
		double a;

		if (inf->gamma[i] != 0.0) {
			a = alpha(inf->gamma[i]);
			fprintf(fout, "\t\t%12.9f\n", a);
			rl[i] = k++;
		}
	}
	fprintf(fout, "\t\t]\n");
	fprintf(fout, "\n");

	if (id && (N <= 3))
		fprintf(fout, "\to2 = i1\n");

	j = 0;
	if (!id) {
		if (N <= 3) {
			adaptor(0, rl[0], 0, inf->gamma[0], "i2 ", "st[1] ", "o2", "st[1] ",
				fout, nbits);
		} else {
			j = rx;
			adaptor(0, rl[0], 0, inf->gamma[0], "i2 ", "st[1] ",
				rx ? "x1 " : "x0 ", "st[1] ", fout, nbits);
		};
	}

	for (i = 3; i < N; i += 4) {
		if (!id) {
			k = rl[i];

			sprintf(in1, "st[%d]", i + 1);
			sprintf(in2, "st[%d]", i + 2);
			sprintf(out1, "x%d", rx ? (i + 1) % 2 : i + 1);
			sprintf(out2, "st[%d]", i + 2);
			adaptor(i + 1, rl[i + 1], rx ? 0 : i + 1, inf->gamma[i + 1], 
					in1, in2,
				out1, out2, fout, nbits);
			sprintf(in1, "x%d", rx ? j % 2 : j);
			sprintf(in2, "x%d", rx ? (i + 1) % 2 : i + 1);
			if ((i + 4) >= N)
				sprintf(out1, "o2");
			else
				sprintf(out1, "x%d", rx ? i % 2 : i);
			sprintf(out2, "st[%d]", i + 1);
			adaptor(i, rl[i], rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		} else {
			k = rl[i];

			if (i == 3)
				sprintf(in1, "i1 ");
			else
				sprintf(in1, "x%d", rx ? 0 : j);
			sprintf(in2, "st[%d]", k);
			if ((i + 4) >= N)
				sprintf(out1, "o2");
			else
				sprintf(out1, "x%d", rx ? 0 : k);
			sprintf(out2, "st[%d]", k);
			adaptor(i, k, rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		}
		j = i;
	}
	if (N == 1) {
		if (!id)
			fprintf(fout, " o1=i1\n");
		else
			fprintf(fout, " o1=i2\n");
	}
	for (i = 1; i < N; i += 4) {
		if (!id) {
			sprintf(in1, "st[%d]", i + 1);
			sprintf(in2, "st[%d]", i + 2);
			sprintf(out1, "x%d", rx ? (i + 1) % 2 : i + 1);
			sprintf(out2, "st[%d]", i + 2);
			adaptor(i + 1, rl[i + 1], rx ? 0 : i + 1, inf->gamma[i + 1], 
					in1, in2, out1, out2, fout, nbits);
			if (i == 1)
				sprintf(in1, "i1 ");
			else
				sprintf(in1, "x%d", rx ? j % 2 : j);
			sprintf(in2, "x%d", rx ? (i + 1) % 2 : i + 1);
			if ((i + 4) >= N)
				sprintf(out1, "o1");
			else
				sprintf(out1, "x%d", rx ? i % 2 : i);
			sprintf(out2, "st[%d]", i + 1);
			adaptor(i, rl[i], rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		} else {
			k = rl[i];

			if (i == 1)
				sprintf(in1, "i2 ");
			else
				sprintf(in1, "x%d", rx ? 0 : j);
			sprintf(in2, "st[%d]", k);
			if ((i + 4) >= N)
				sprintf(out1, "o1");
			else
				sprintf(out1, "x%d", rx ? 0 : k);
			sprintf(out2, "st[%d]", k);
			adaptor(i, k, rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		}
		j = i;
	}
	if (id)
		fprintf(fout,
			"\n"
			"# decimator output: sample(n)=(o1+/-o2)>>1 for lowpass/highpass */\n"
			"# interpolator output: sample(n)=o1, sample(n+1)=+/-o2 for lowpass/highpass */\n");
	else
		fprintf(fout,
			"\n"
			"# filter output: sample(n)=(o1+/-o2)>>1 for lowpass/highpass */\n");
	fprintf(fout, "\n");
	fprintf(fout, "\treturn (o1, o2)\n");
	fprintf(fout, "end\n\n");
	
	if (!id) {
		fprintf(fout, "function %slp_filter(st::Vector{%s}, x::Vector{%s})\n", 
				prefix, dtype, dtype);
		fprintf(fout, "\tn = length(x)\n");
		fprintf(fout, "\ty = zeros(%s, n)\n", dtype);
		fprintf(fout, "\n");
		fprintf(fout, "\tfor i in 1:n\n");
		fprintf(fout, "\t\t(o1, o2) = %sfilter(st, x[i], x[i])\n", prefix);
		fprintf(fout, "\t\ty[i] = (o1 + o2) / 2\n");
		fprintf(fout, "\tend\n");
		fprintf(fout, "\n");
		fprintf(fout, "\treturn y\n");
		fprintf(fout, "end\n\n");
	 } else {
		fprintf(fout, "function %slp_filter(st::Vector{%s}, x::Vector{%s})\n", 
				prefix, dtype, dtype);
		fprintf(fout, "\tn = div(length(x), 2)\n");
		fprintf(fout, "\ty = zeros(%s, n * 2)\n", dtype);
		fprintf(fout, "\n");
		fprintf(fout, "\tfor i in 1:n\n");
		fprintf(fout, "\t\t(o1, o2) = %sfilter(st, x[i], x[i])\n", prefix);
		fprintf(fout, "\t\ty[i * 2 - 1] = o1 / 2\n");
		fprintf(fout, "\t\ty[i * 2] = o2 / 2\n");
		fprintf(fout, "\tend\n");
		fprintf(fout, "\n");
		fprintf(fout, "\treturn y\n");
		fprintf(fout, "end\n\n");
	}
	return 0;
}
