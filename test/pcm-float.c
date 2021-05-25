/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	
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

#include "pcm.h"

#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>

struct pcmfloat *pcmfloat_create(unsigned int len, float samplerate)
{
	struct pcmfloat *pcm;
	unsigned int size = sizeof(struct pcmfloat) + len * sizeof(float);

	assert(samplerate > 0);
	assert(len > 0);

	if ((pcm = calloc(1, size)) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		return NULL;
	};

	memset(pcm, 0, size);
	pcm->len = len;
	pcm->samplerate = samplerate;

	return pcm;
}

int pcmfloat_destroy(struct pcmfloat *pcm)
{
	if (pcm == NULL) {
		fprintf(stderr, "%s: NULL pointer.", __func__);
		return -1;
	};

	free(pcm);

	return 0;
}

/* write the waveform into a file suitable for gnuplot */
int pcmfloat_dump(const char *fname, struct pcmfloat *pcm)
{
	FILE *f;
	int i;

	if (fname == NULL) {
		f = stdout;
	} else {
		if ((f = fopen(fname, "w")) == NULL) {
			fprintf(stderr, "fopen(\"%s\") failed: %s!",
				fname, strerror(errno));
			return -1;
		}
	}

	for (i = 0; i < pcm->len; ++i) {
		fprintf(f, "%10.7f %10.7f\n",
			(float)i / pcm->samplerate, pcm->sample[i]);
	}

	fclose(f);

	return 0;
}

/* write the waveform into a file suitable for gnuplot */
int pcmfloat_plot(const char *name, struct pcmfloat *pcm,
		  const char *lstyle, bool png)
{
	char plt_path[PATH_MAX];
	char out_path[PATH_MAX];
	char dat_path[PATH_MAX];
	FILE *f;

	sprintf(plt_path, "%s.plt", name);
	sprintf(out_path, "%s.png", name);
	sprintf(dat_path, "%s.dat", name);

	if ((f = fopen(plt_path, "w")) == NULL) {
		fprintf(stderr, "fopen(\"%s\") failed: %s!",
			plt_path, strerror(errno));
		return -1;
	}
	fprintf(f, "set terminal wxt\n");
	if (png) {
		fprintf(f,
			"set terminal png size 2048,1024 font 'verdana,12'\n");
		fprintf(f, "set output '%s'\n", out_path);
	}
	fprintf(f, "set offset graph 0.0,0.0,0.0,0.0\n");
	fprintf(f, "# labels\n");	/* labels */
	fprintf(f, "set xlabel 'time(s)'\n");
	fprintf(f, "set ylabel '%s'\n", name);
	fprintf(f, "# define axis\n");	/* axis */
	fprintf(f, "set style line 11 lc rgb '#c0c0c0' lt 0.5\n");
	fprintf(f, "set border 3 back ls 11\n");
	fprintf(f, "set tics out nomirror\n");
	fprintf(f, "# define grid\n");	/* grid */
	fprintf(f, "set style line 12 lc rgb '#808080' lt 0 lw 1\n");
	fprintf(f, "set grid back ls 12\n");
	fprintf(f, "# line styles\n");	/* line styles */
	if (lstyle) {
		fprintf(f, "set style line 1 %s\n", lstyle);
	} else {
		fprintf(f,
			"set style line 1 lc rgb '#101010' pt 0 lt 1 lw 1\n");
	}
	fprintf(f, "set style line 2 lc rgb '#108010' pt 0 lt 0.5 lw 1\n");
	fprintf(f, "plot '%s' using 1:2 notitle with lp ls 1\n", dat_path);
	fprintf(f, "set output\n");
	fprintf(f, "quit\n");
	fclose(f);

	return pcmfloat_dump(dat_path, pcm);
}

int pcmfloat_sweep(struct pcmfloat *pcm, float f0, float f1, float a)
{
	double x;
	double z;
	double w0;
	double w1;
	double dw;
	double w;
	int i;

	if (pcm == NULL) {
		fprintf(stderr, "%s: NULL pointer.", __func__);
		return -1;
	};

	w0 = (2.0 * M_PI * f0) / pcm->samplerate;
	w1 = (2.0 * M_PI * f1) / pcm->samplerate;
	dw = (w1 - w0) / pcm->len;

	w = w0;
	for (i = 0; i < pcm->len; ++i) {
		x = sin(w);
		w += i * dw;

		/* saturate  */
		if (x > 1)
			x = 1;
		else if (x < -1)
			x = -1;

		/* add to the existing data in the buffer */
		z = pcm->sample[i];
		pcm->sample[i] = z + x * a;
	}

	return 0;
}

#if 0
int pcmfloat_logsweep(struct pcmfloat *pcm, float f0, float f1, float a)
{
	double x;
	double z;
	double w0;
	double w1;
	double dw;
	double p, p0;
	double q, q0;
	int i;
	double r[2][2];
	double d[2][2];

	if (pcm == NULL) {
		fprintf(stderr, "%s: NULL pointer.", __func__);
		return -1;
	};

	/* Normalized Start and stop frequencies */
	w0 = (2.0 * M_PI * f0) / pcm->samplerate;
	w1 = (2.0 * M_PI * f1) / pcm->samplerate;
	dw = (w1 - w0) / pcm->len;
	itvl = pcm->len / pcm->samplerate;

	x = ((int64_t) f1 * Q30(1.0)) / f0;
	y = q30log10(x);
	YAP("SweepGen: x=%.6f y=%.6f", Q30F(x), Q30F(y));

	/* Initial and end states */
	/* WARN: s = 2 * sin(w) */
	s0 = sin(w0);
	s1 = sin(w0);
	exp = log10(itvl);

	z = pcm->sample[0];
	pcm->sample[0] = z + x * a;

	for (i = 1; i < pcm->len; ++i) {
		float p1;
		float q1;

		p0 = p;
		q0 = q;

		p = p0 * r[0][0] + q0 * r[0][1];
		q = p0 * r[1][0] + q0 * r[1][1];

		x = p;

		/* saturate  */
		if (x > 1)
			x = 1;
		else if (x < -1)
			x = -1;

		/* Scale and store into the buffer */
		/* convert to Q15 */
		pcm[i] = p * 1;
		/* Compute the new values */
		q1 = q - s * p;
		p1 = s * q1 + p;
		s1 = s1 * exp;
		/* saturate  */
		q = Q30_SAT_ONE(q1);
		p = Q30_SAT_ONE(p1);
	}
	for (; i < len; ++i) {
		pcm[i] = 0;
	}

	/* add to the existing data in the buffer */
	z = pcm->sample[i];
	pcm->sample[i] = z + x * a;

	r[0][0] = r[0][0] * d[0][0] + r[0][1] * d[1][0];
	r[0][1] = r[0][0] * d[0][1] + r[0][1] * d[1][1];
	r[1][0] = r[1][0] * d[0][0] + r[1][1] * d[1][0];
	r[1][1] = r[1][0] * d[0][1] + r[1][1] * d[1][1];
}

return 0;
}
#endif

