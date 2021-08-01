/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	glwdf-app.c
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

_Pragma ("GCC optimize (\"Ofast\")")

#include <complex.h>
#include <assert.h>
#include <pthread.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#include "lwdf.h"
#include "vector.h"

#ifndef LWDF_FREQ_TASK_MAX
#define LWDF_FREQ_TASK_MAX 1
#endif

struct lwdf_fp64_freq {
	bool log;
	struct lwdf_fp64 * flt;

	size_t max_len; /* allocated vector length */

	size_t len; /* length of w and z vectors */
	double * w; /* normalized frequency vector */
	complex double * z; /* frequency response vector */

	size_t max_dftn; /* allocated DFT length */
	size_t dftn; /* dft points */ 
	struct {
		double * x;
		double * y;
		double * wnd;
	} task [LWDF_FREQ_TASK_MAX]; /* worker task */ 
	unsigned int ntasks; /* number of worker tasks */
};

/*
 * Create a linearly spaced frequency vector for 
 * single term DFT calcultation. 
 *
 * The frequencies are normalized to the samplerate:
 *
 * w[cyc/samp] = f[cyc/sec] / samplerate[samp/sec]
 *
 * w : output frequency vector;
 * dftn : number of terms for the input sequence of the dft; 
 * w0: initial frequncy
 * w1: final frequncy
 * cnt: number of intervals between w0 and w1
 *
 * return:  the actual size of the vector w;
 */
ssize_t dft_linspace_freq_vec(double w[], size_t size,
							  double w0, double w1, 
							  size_t dftn)
{
	unsigned int n;
	unsigned int j;
	unsigned int i;
	double dw;
	double wp;
	double wr;
	
	dw = (w1 - w0) / (size);
	wp = -1.0;
	j = 0; 

	for (j = 0, wr = 0.5; (j < size) && (wr >= w0); wr -= dw) {
		double ri;
		double wi;
		double mi;

		mi = round(dftn * wr / 2) * 2;
		if (mi < 2)
			break;
		ri = dftn / mi;
		wi = 1.0 / ri;

		if ((ri >= 2.0) && (wi != wp)) {
			wp = wi;
			w[j++] = wi;
#if (DEBUG > 3)
			fprintf(stderr, "%4d mi=%5g q=%7.3f wr=%8.6f w=%8.6f\n", 
					j, mi, ri, wr, wi);
#endif
		}
	}

	n = j;
	for (i = 0; i < (n / 2); ++i) {
		double t;
		--j;
		t = w[j];
		w[j] = w[i];
		w[i] = t;
	}

	return n;
}

ssize_t dft_logspace_freq_vec(double w[], size_t size,
							  double w0, double w1, 
							  size_t dftn)
{
	unsigned int n;
	unsigned int j;
	unsigned int i;
	double kw;
	double wp;
	double wr;


	kw = 1/exp(log(w1/w0)/(size));

#if (DEBUG > 3)
	fprintf(stderr, "size=%d kw=%g w0=%g w1=%g\n", (int)size, kw, w0, w1);
#endif

	wp = -1.0;
	j = 0; 

	for (j = 0, wr = 0.5; (j < size) && (wr >= w0); wr *= kw) {
		double ri;
		double wi;
		double mi;

		mi = round(dftn * wr / 2) * 2;
		if (mi < 2)
			break;
		ri = dftn / mi;
		wi = 1.0 / ri;

		if ((ri >= 2.0) && (wi != wp)) {
#if (DEBUG > 3)
			fprintf(stderr, "%4d mi=%5g q=%7.3f wr=%8.6f w=%8.6f\n", 
					j, mi, ri, wr, wi);
#endif
			wp = wi;
			w[j++] = wi;
		}
	}

	n = j;
	for (i = 0; i < (n / 2); ++i) {
		double t;
		--j;
		t = w[j];
		w[j] = w[i];
		w[i] = t;
	}

	return n;
}

static ssize_t __lwdf_fp64_vec_realloc(struct lwdf_fp64_freq * ffr, 
									   ssize_t npts)
{
	assert(ffr != NULL);
	assert(npts >= 2);

	if (npts > ffr->max_len) {
		complex double * z;
		double * w;

		if ((w = calloc(npts, sizeof(double))) == NULL) {
			fprintf(stderr, "%s: calloc() failed: %s", __func__,
					strerror(errno));
			return -1;
		};
		if ((z = calloc(npts, sizeof(complex double))) == NULL) {
			fprintf(stderr, "%s: calloc() failed: %s", __func__,
					strerror(errno));
			free(w);
			return -1;
		};
		free(ffr->w);
		free(ffr->z);
		ffr->w = w;
		ffr->z = z;
		ffr->max_len = npts;
	}

	return npts;
}

ssize_t lwdf_fp64_freq_lin_set(struct lwdf_fp64_freq * ffr, 
						   double w0, double w1, ssize_t npts)
{
	ssize_t cnt;

	assert(ffr != NULL);
	assert(npts >= 2);

	__lwdf_fp64_vec_realloc(ffr, npts);

	cnt = dft_linspace_freq_vec(ffr->w, npts, w0, w1, ffr->dftn);

	return ffr->len = cnt;
}

ssize_t lwdf_fp64_freq_log_set(struct lwdf_fp64_freq * ffr, 
						   double w0, double w1, ssize_t npts)
{
	ssize_t cnt;

	assert(ffr != NULL);
	assert(npts >= 2);

	__lwdf_fp64_vec_realloc(ffr, npts);

	cnt = dft_logspace_freq_vec(ffr->w, npts, w0, w1, ffr->dftn);

	return ffr->len = cnt;
}

/*
 * Create a new LWDF frequency analysis object
 */
struct lwdf_fp64_freq * lwdf_fp64_freq_new(struct lwdf_fp64 * flt, 
										   size_t dftn)
{
	struct lwdf_fp64_freq * ffr;
	complex double * z;
	size_t npts;
	double * wnd;
	double * w;
	double * p;
	ssize_t cnt;
	double w0;
	double w1;
	int nw; /* number of workers */
	int i;

	assert(flt != NULL);
	assert(dftn > 8);

	if ((ffr = calloc(1, sizeof(struct lwdf_fp64_freq))) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		return NULL;
	};

	/* default to linear space */
	npts = dftn / 16;
	w0 = 0.0;
	w1 = 0.5;

	if ((w = calloc(dftn, sizeof(double))) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		free(ffr);
		return NULL;
	};

	if ((z = calloc(dftn, sizeof(complex double))) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		free(w);
		free(ffr);
		return NULL;
	};

	/* default to linear space */
	cnt = dft_linspace_freq_vec(w, npts, w0, w1, dftn);

	/* window */
	if ((wnd = calloc(dftn, sizeof(double))) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		free(z);
		free(w);
		free(ffr);
		return NULL;
	};

	if ((p = calloc(dftn, LWDF_FREQ_TASK_MAX * 2 * sizeof(double))) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		free(wnd);
		free(z);
		free(w);
		free(ffr);
		return NULL;
	};

	ffr->flt = flt;
	ffr->max_len = dftn;
	ffr->len = cnt;
	ffr->dftn = dftn;
	ffr->max_dftn = dftn;
	ffr->w = w;
	ffr->z = z;
#if (LWDF_FREQ_TASK_MAX) > 1
	/* Multithread */
#endif
	nw = LWDF_FREQ_TASK_MAX;

	/* set worker's vector pointers */
	for (i = 0; i < nw; ++i) {
		ffr->task[i].x = p;
		p += dftn;
		ffr->task[i].y = p;
		p += dftn;
	}

	return ffr;
}

void lwdf_fp64_freq_free(struct lwdf_fp64_freq * ffr)
{
	assert(ffr != NULL);
	assert(ffr->w != NULL);
	assert(ffr->z != NULL);

	free(ffr->z);
	free(ffr->w);
	free(ffr);
}

void plt_dbg(double y[], size_t npts);

ssize_t lwdf_fp64_lowwpass_freq_resp(struct lwdf_fp64_freq * ffr,
									 struct lwdf_fp64 * flt, 
									 double * pw[],
									 complex double * pz[])
{
#if FREQ_RESP_WND 
	double * wnd;
#endif
	unsigned int dftn;
	unsigned int k;
	unsigned int n;
	complex double * z;
	double * w;
	double * x;
	double * y;

	assert(ffr != NULL);
	assert(ffr->w != NULL);
	assert(ffr->z != NULL);

	assert(ffr->task[0].x != NULL);
	assert(ffr->task[0].y != NULL);

	/* FIXME: multithread */
	w = ffr->w;
	z = ffr->z;
	x = ffr->task[0].x;
	y = ffr->task[0].y;
	n = ffr->len;
	dftn = ffr->dftn;

	for (k = 0; k < n; ++k) {
#if FREQ_RESP_WND 
		unsigned int i;
#endif
		/* apply window */
		vec_fp64_cosine(x, dftn, w[k]);

#if FREQ_RESP_WND 
		/* apply window */
		for (i = 0; i < dftn; ++i)
			x *= wnd[i];
#endif

		/* apply filter */
		lwdf_fp64_reset(flt);

		lwdf_fp64_lowpass(flt, y, x, dftn);

		/* single point DFT */
		z[k] = vec_fp64_gortzel_dft(y, dftn, w[k]);

#if (DEBUG > 3)
		fprintf(stderr, "k=%d w=%g z=%g%+g\n", k, w[k],
				creal(z[k]), cimag(z[k]));
#endif

	}

	if (pz != NULL)
		*pz = z;

	if (pw != NULL)
		*pw = w;

	return n;
}

