/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	lwdf.h
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

/*
 * Lattice Wave Digital Filters
 *
 * Based on: 
 *  Explicit Formulas for Lattice Wave Digital Filters, Lajos Gazsi, 1985 IEEE
 *
 */

#ifndef __LWDF_H__
#define __LWDF_H__

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>

enum lwdf_type {
	LWDF_BUTTW = 1, /* Butterworth */
	LWDF_CHEB1 = 2, /* Chebyshev */
	LWDF_ELLIP = 3  /* Elliptic/Cauer */
};

/* max filter order */
#define LWDF_ORDER_MAX 127

/* Input to the filter generator */
struct lwdfwiz_param {
	double samplerate;
	uint8_t ftype;
	uint8_t order;
	uint8_t nbits;
	/* bireciprocal filter */
	bool bi;
	/* bireciprocal for decimation/interpolation */
	bool id;
	bool rx;
	bool ff;
	double asmin; 
	double as; 
	double es; 
	double fs;
	double ap;
	double ep;
	double fp;
};

/* Output from the filter generator */
struct lwdf_info {
	double samplerate;
	unsigned int order;
	double gamma[LWDF_ORDER_MAX];	
};

struct lwdfwiz_cfg {
	/* Store parameters from the past... */
	struct lwdfwiz_param wiz;
	char prefix[32];
	char cfname[256];
	char jlfname[256];
};


/* Floating point single precision filter */
struct lwdf_fp32;

/* Floating point double precision filter */
struct lwdf_fp64;

/* Filter frequency response analysis */
struct lwdf_fp64_freq;

#ifdef __cplusplus
extern "C" {
#endif

/* Terminal/console mode wizard */
int lwdfwiz_term(struct lwdfwiz_param * wiz, struct lwdf_info * inf);

int lwdf_cgen(FILE *fout, const char * prefix, 
			   const struct lwdfwiz_param * wiz, 
			   const struct lwdf_info * inf);

int lwdf_jlgen(FILE *fout, const char * prefix, 
			   const struct lwdfwiz_param * wiz, 
			   const struct lwdf_info * inf);



/* 
 * Run time filters 
 * */

struct lwdf_fp64 *lwdf_fp64_new(double samplerate);

int lwdf_fp64_free(struct lwdf_fp64 *flt);

int lwdf_fp64_init(struct lwdf_fp64 * flt, double samplerate);

ssize_t lwdf_fp64_gamma_set(struct lwdf_fp64 * flt, const double gamma[], size_t cnt);

ssize_t lwdf_fp64_gamma_get(struct lwdf_fp64 * flt, double gamma[], size_t max);

double lwdf_fp64_coeff_get(struct lwdf_fp64 * flt, unsigned int idx);

void lwdf_fp64_coeff_set(struct lwdf_fp64 * flt, 
						unsigned int idx, double coeff);

double lwdf_fp64_samplerate_get(struct lwdf_fp64 * flt);

int lwdf_fp64_reset(struct lwdf_fp64 * flt);


struct lwdf_fp64_freq * lwdf_fp64_freq_new(struct lwdf_fp64 * flt, 
										   size_t dft_n);

void lwdf_fp64_freq_free(struct lwdf_fp64_freq * ffr);

ssize_t lwdf_fp64_lowwpass_freq_resp(struct lwdf_fp64_freq * ffr,
									 struct lwdf_fp64 * flt, 
									 double * pw[],
									 complex double * pz[]);

ssize_t lwdf_fp64_freq_log_set(struct lwdf_fp64_freq * ffr, 
							   double w0, double w1, ssize_t npts);

ssize_t lwdf_fp64_freq_lin_set(struct lwdf_fp64_freq * ffr, 
							   double w0, double w1, ssize_t npts);
/* 
 * Filtering 
 *  These functions performs filtering over a vector
 * */

/* Low Pass */
ssize_t lwdf_fp64_lowpass(struct lwdf_fp64 * flt, double y[], 
						  const double x[], size_t len);
/* High Pass */
ssize_t lwdf_fp64_higpass(struct lwdf_fp64 * flt, double y[], 
						  const double x[], size_t len);
/* Split Band */
ssize_t lwdf_fp64_splitband(struct lwdf_fp64 * flt, double y[], 
							const double x[], size_t len);

#ifdef  __cplusplus
}
#endif

#endif	/* __LWDF_H__ */
