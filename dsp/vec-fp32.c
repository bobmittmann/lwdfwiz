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

_Pragma ("GCC optimize (\"Ofast\")")

#include <limits.h>
#include <math.h>

#include "vector.h"

ssize_t vec_fp32_cosine(float y[], size_t len, float w0)
{
	unsigned int i;
	double dw;

	dw = (double)(2.0 * M_PI) * w0;

	for (i = 0; i < len; ++i) {
		y[i] = cos(dw * i);
	}

	return len;
}

ssize_t vec_fp64_wnd_blackman(float y[], size_t len, float alpha)
{
	unsigned int i;
	double sum;
	double a0;
	double a1;
	double a2;
	double g;

	a0 = (1.0 - alpha) / 2.0;
	a1 = 1.0 / 2.0;
	a2 = alpha / 2.0;

	sum = 0;
	for (i = 0; i < len; ++i) {
		y[i] = a0 - a1*cos((2.0*M_PI*i)/len) + a2*cos((4.0*M_PI*i) / len);
		sum += y[i];
	}
	g = (double)len / sum;

	for (i = 0; i < len; ++i) {
		y[i] *= g;
	}

	return len;
}

complex float vec_fp64_gortzel_dft(const float x[], size_t len, float w)
{
	unsigned int i;
	double coeff;
	double scale;
	double omega;
	double s1;
	double s2;

	scale = (double)(2.0) / len;
	omega = (double)(2.0 * M_PI) * w;
	coeff = (double)(2.0) * cos(omega); 

	s1 = 0;
	s2 = 0;

	for (i = 0; i < len; ++i) {
		double s;

		s = x[i] + (coeff * s1) - s2;
		s2 = s1;
		s1 = s;
	}

	return (s1 - cexp(-I * omega) * s2) * scale;
}


