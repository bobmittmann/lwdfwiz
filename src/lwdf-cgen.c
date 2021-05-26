/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	lwdf-cgen.c
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
#include "lwdf.h"

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

static void adaptor(int i, int t, double g, char *in1, char *in2,
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
	fprintf(f, "\t/* adaptor %2d: g = %.8f */\n ", i, g);

	if (g == 0.0) {
		fprintf(f, "\t%2s%-3s=%-7s;%28s%-3s=%-7s;\n", "", out1, in2, "",
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
		fprintf(f, "\tt%d = %s - %s;\n", t, in2, in1);
	else
		fprintf(f, "\tt%d = %s - %s;\n", t, in1, in2);

	if (nbits == 0) {
		if (so)
			fprintf(f, "\t%s = a%d * t%d %c %s;\n", out1, i, t, pm, in2);
		else
			fprintf(f, "\t%s = a%d * t%d %c %s;\n", out2, i, t, pm, in2);
	} else {
		if (b) {
			if (so)
				fprintf(f, "\t%s = %7s(t%d>>%d) %c %s;\n", 
						out1, "", t, b, pm, in2);
			else
				fprintf(f, "\t%s = %7s(t%d>>%d) %c %s;\n", 
						out2, "", t, b, pm, in2);
		} else {
			if (so)
				fprintf(f, "\t%s = ((%d*t%d)>>%d) %c %s;\n", 
						out1, aQ, t, nbits, pm, in2);
			else
				fprintf(f, "\t%s = ((%d*t%d)>>%d) %c %s;\n", 
						out2, aQ, t, nbits, pm, in2);
		}
	}
	if (so)
		fprintf(f, "\t%s = %s - t%d;\n", out2, out1, t);
	else
		fprintf(f, "\t%s = %s - t%d;\n", out1, out2, t);
	return;
}

int lwdf_cgen(FILE *fout, const struct lwdfwiz_param * wiz, 
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
	/* bireciprocal for decimation/interpolation (0=no, 1=yes)? */
	id = wiz->id;
	/* bits (not including sign bit, 0=no quantization)? */
	nbits = wiz->nbits;
	/* reuse and minimize temporary variables (0=no, 1=yes)?  */
	rx = wiz->rx ? 1 : 0;

	if (nbits == 0)
		dtype = "float";
	else
		dtype = "int";

	fprintf(fout, "/* -----------------------------------------------------\n");
	fprintf(fout, " * This file was automatically generated, do not edit!\n");
	fprintf(fout, " *\n");

	switch (ftype) {
	case LWDF_BUTTW:
		fprintf(fout, " * Buttworth order %d\n", N);
		break;
	case LWDF_CHEB1:
		fprintf(fout, " * Chebyshev I order %d\n", N);
		break;
	case LWDF_ELLIP:
		fprintf(fout, " * Elliptic/Cauer order %d\n", N);
		break;
	}

	if (bi) {
		fprintf(fout, " * - bireciprocal");
		if (id)
			fprintf(fout, ", for interpolation/decimation");
		fprintf(fout, "\n");
	}
	fprintf(fout,
		" * - stopband min attenuation as=%.0f dB at fs=%.2f%% (100%%=F/2)\n",
		wiz->as, wiz->fs / (F / 2.0));
	fprintf(fout,
		" * - passband attenuation spread ap=%.2f dB at fp=%.2f%% (100%%=F/2)\n",
		wiz->ap, wiz->fp / (F / 2.0));

	fprintf(fout, " *\n");
	fprintf(fout, " * -----------------------------------------------------\n");
	fprintf(fout, " */\n");
	fprintf(fout, "\n");

	fprintf(fout, "void filter(%s st[], %s i1, %s i2, %s *o1, %s *o2)\n", 
			dtype, dtype, dtype, dtype, dtype);
	fprintf(fout, "{\n");

	k = 0;
	for (i = 0; i < N; i++) {
		double a;

		if (inf->gamma[i] != 0.0) {
			a = alpha(inf->gamma[i]);
			fprintf(fout, "\tconst float a%d = ", i);
			fprintf(fout, "%12.9f\n", a);
			rl[i] = k++;
		}
	}
	fprintf(fout, "\n");

	if (wiz->rx) {
		if ((!id) || (N > 1))
			fprintf(fout, "\t%s t0", dtype);
		if (id) {
			if (N >= 7)
				fprintf(fout, ",x0");
		} else {
			if (N >= 3)
				fprintf(fout, ",x0");
			if (N >= 5)
				fprintf(fout, ",x1");
		}
		if ((!id) || (N > 1))
			fprintf(fout, ";\n");
	} else {
		char sep;
		if ((!id) || (N > 1))
			fprintf(fout, "\t%s", dtype);
		sep = ' ';
		for (i = 0; i < N; i++)
			if (inf->gamma[i] != 0.0) {
				fprintf(fout, "%ct%d", sep, i);
				sep = ',';
			}
		if ((!id) || (N > 1))
			fprintf(fout, ";\n");
		if (!id) {
			if (N > 1) {
				fprintf(fout, "\t%s", dtype);
				sep = ' ';
			}
			for (i = 0; i <= (N - 5); i++) {
				fprintf(fout, "%cx%d", sep, i);
				sep = ',';
			}
			if ((N - 3) > 0) {
				fprintf(fout, "%cx%d", sep, N - 3);
				sep = ',';
			}
			if ((N - 1) > 0) {
				fprintf(fout, "%cx%d", sep, N - 1);
				sep = ',';
			}
			if (N > 1)
				fprintf(fout, ";\n");
		} else {
			if (N > 5) {
				fprintf(fout, "\t%s", dtype);
				sep = ' ';
			}
			for (i = 1; i <= (N - 5); i += 2) {
				fprintf(fout, "%cx%d", sep, i);
				sep = ',';
			}
			if (N > 5)
				fprintf(fout, ";\n");
		}
	}

	if (id)
		fprintf(fout, "\t/* interpolator input: i1=i2=sample(n) */\n "
			"\t/* decimator input: i1=sample(n), i2=sample(n+1) */\n");
	else
		fprintf(fout, "\t/* filter input: i1=i2=sample(n) */\n");

	fprintf(fout, "\n\t/* Upper arm ------------ */\n");
	if (id && (N <= 3))
		fprintf(fout, "\t*o2=i1;\n");

	j = 0;
	if (!id) {
		if (N <= 3) {
			adaptor(0, 0, inf->gamma[0], "i2 ", "st[0] ", "*o2", "st[0] ",
				fout, nbits);
		} else {
			j = rx;
			adaptor(0, 0, inf->gamma[0], "i2 ", "st[0] ",
				rx ? "x1 " : "x0 ", "st[0] ", fout, nbits);
		};
	}

	for (i = 3; i < N; i += 4) {
		if (!id) {
			sprintf(in1, "st[%2d]", i);
			sprintf(in2, "st[%2d]", i + 1);
			sprintf(out1, "x%-2d", rx ? (i + 1) % 2 : i + 1);
			sprintf(out2, "st[%2d]", i + 1);
			adaptor(i + 1, rx ? 0 : i + 1, inf->gamma[i + 1], in1, in2,
				out1, out2, fout, nbits);
			sprintf(in1, "x%-2d", rx ? j % 2 : j);
			sprintf(in2, "x%-2d", rx ? (i + 1) % 2 : i + 1);
			if ((i + 4) >= N)
				sprintf(out1, "*o2");
			else
				sprintf(out1, "x%-2d", rx ? i % 2 : i);
			sprintf(out2, "st[%2d]", i);
			adaptor(i, rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		} else {
			k = rl[i];

			if (i == 3)
				sprintf(in1, "i1 ");
			else
				sprintf(in1, "x%-2d", rx ? 0 : j);
			sprintf(in2, "st[%2d]", k);
			if ((i + 4) >= N)
				sprintf(out1, "*o2");
			else
				sprintf(out1, "x%-2d", rx ? 0 : k);
			sprintf(out2, "st[%2d]", k);
			adaptor(i, rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		}
		j = i;
	}
	fprintf(fout, "\n\t/* Lower arm  -----------------  */\n");
	if (N == 1) {
		if (!id)
			fprintf(fout, " *o1=i1;\n");
		else
			fprintf(fout, " *o1=i2;\n");
	}
	for (i = 1; i < N; i += 4) {
		if (!id) {
			sprintf(in1, "st[%2d]", i);
			sprintf(in2, "st[%2d]", i + 1);
			sprintf(out1, "x%-2d", rx ? (i + 1) % 2 : i + 1);
			sprintf(out2, "st[%2d]", i + 1);
			adaptor(i + 1, rx ? 0 : i + 1, inf->gamma[i + 1], in1, in2,
				out1, out2, fout, nbits);
			if (i == 1)
				sprintf(in1, "i1 ");
			else
				sprintf(in1, "x%-2d", rx ? j % 2 : j);
			sprintf(in2, "x%-2d", rx ? (i + 1) % 2 : i + 1);
			if ((i + 4) >= N)
				sprintf(out1, "*o1");
			else
				sprintf(out1, "x%-2d", rx ? i % 2 : i);
			sprintf(out2, "st[%2d]", i);
			adaptor(i, rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		} else {
			k = rl[i];

			if (i == 1)
				sprintf(in1, "i2 ");
			else
				sprintf(in1, "x%-2d", rx ? 0 : j);
			sprintf(in2, "st[%2d]", k);
			if ((i + 4) >= N)
				sprintf(out1, "*o1");
			else
				sprintf(out1, "x%-2d", rx ? 0 : k);
			sprintf(out2, "st[%2d]", k);
			adaptor(i, rx ? 0 : i, inf->gamma[i], in1, in2, out1, out2,
				fout, nbits);
		}
		j = i;
	}
	if (id)
		fprintf(fout,
			"\n"
			"\t/* decimator output: sample(n)=(o1+/-o2)>>1 for lowpass/highpass */\n"
			"\t/* interpolator output: sample(n)=o1, sample(n+1)=+/-o2 for lowpass/highpass */\n");
	else
		fprintf(fout,
			"\n"
			"\t/* filter output: \n"
			"\t     sample(n)=(o1+/-o2)>>1 for lowpass/highpass */\n");
	fprintf(fout, "}\n\n");
	
	fprintf(fout, "void hp_filter(%s st[], %s y[], %s x[], unsigned int len)\n", 
			dtype, dtype, dtype);
	fprintf(fout, "{\n");
	
	fprintf(fout, "}\n\n");

	fprintf(fout, "void lp_filter(%s st[], %s y[], %s x[], unsigned int len)\n", 
			dtype, dtype, dtype);
	fprintf(fout, "{\n");
	fprintf(fout, "\tint i;\n");
	fprintf(fout, "\t%s o1, o2;\n", dtype);
	fprintf(fout, "\n");
	fprintf(fout, "\tfor (i = 0; i < len; ++i) {\n");
	fprintf(fout, "\t\tfilter(st, x[i], x[i], &o1, &o2)\n");
	fprintf(fout, "\t\ty[i] = o1 + o2;\n");
	fprintf(fout, "\t}\n");
	fprintf(fout, "}\n\n");

	return 0;
}
