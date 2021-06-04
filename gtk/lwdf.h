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

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

enum lwdf_type {
	LWDF_BUTTW = 1, /* Butterworth */
	LWDF_CHEB1 = 2, /* Chebyshev */
	LWDF_ELLIP = 3  /* Elliptic/Cauer */
};

/* max filter order */
#define LWDF_NMAX 127

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
	double gamma[LWDF_NMAX];	
};

struct lwdfwiz_cfg {
	/* Store parameters from the past... */
	struct lwdfwiz_param wiz;
	char prefix[32];
	char cfname[256];
	char jlfname[256];
};

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

#ifdef  __cplusplus
}
#endif

#endif	/* __LWDF_H__ */

