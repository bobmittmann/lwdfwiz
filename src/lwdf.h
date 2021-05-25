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

struct lwdf_info {
	double samplerate;
	uint8_t ftype;
	uint8_t order;
	uint8_t nbits;
	bool bi;
	bool id;
	bool rx;
	double as; 
	double es; 
	double fs;
	double ap;
	double fp;
	double gamma[LWDF_NMAX];	
	bool ccode;
};

#ifdef __cplusplus
extern "C" {
#endif

int lwdf_cgen(FILE *fout, const struct lwdf_info * inf);

int lwdf_wiz(struct lwdf_info * inf);

#ifdef  __cplusplus
}
#endif

#endif	/* __LWDF_H__ */
