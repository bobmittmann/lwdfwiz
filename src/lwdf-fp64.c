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

_Pragma ("GCC optimize (\"Ofast\")")

#include "lwdf.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

/* Number of coefficients is the same as order in most cases */
#define LWDF_COEFF_MAX (LWDF_ORDER_MAX)

/* Number of states is the same as order in most cases */
#define LWDF_STATE_MAX (LWDF_ORDER_MAX)

/* Double precision filter */
struct lwdf_fp64 {
	float samplerate;
	/* Coefficients */
	struct {
		uint16_t max;
		uint16_t cnt;
		double gamma[LWDF_COEFF_MAX];
	} coeff;

	/* Internal states */
	struct {
		uint16_t max;
		uint16_t cnt;
		double t[LWDF_STATE_MAX];
	} state;
};


static inline void lwd_adaptor0(double g, double in1, double in2, 
								double *out1, double *out2)
{
	*out1 = in2;
	*out2 = in1;
}	

/* g > 0.5 */
static inline void lwd_adaptor1(double g, double in1, double in2, 
								double *out1, double *out2)
{
	double a = 1.0 - g;
	double x1;
	double x2;

	x1 = in1 - in2;
	x2 = a * x1 + in2;
	*out1 = x2 - x1;
	*out2 = x2;
}

/* 0.5 >= g > 1e-9  */
static inline void lwd_adaptor2(double g, double in1, double in2, 
								double *out1, double *out2)
{
	double a = g;
	double x1;
	double x2;

	x1 = in2 - in1;
	x2 = a * x1 + in2;
	*out2 = x2 - x1;
	*out1 = x2;
}

/* -1e-9 > g > -0.5  */
static inline void lwd_adaptor3(double g, double in1, double in2, 
								double *out1, double *out2)
{
	double a = -g;
	double x1;
	double x2;

	x1 = in1 - in2;
	x2 = a * x1 - in2;
	*out2 = x2 - x1;
	*out1 = x2;
}

/* g <= -0.5  */
static inline void lwd_adaptor4(double g, double in1, double in2, 
								double *out1, double *out2)
{
	double a = 1.0 + g;
	double x1;
	double x2;

	x1 = in2 - in1;
	x2 = a * x1 - in2;
	*out1 = x2 - x1;
	*out2 = x2;
}

static inline void lwd_adaptor(double g, double in1, double in2, 
						double *out1, double *out2)
{
	if (g > 0.5) {
		lwd_adaptor1(g, in1, in2, out1, out2);
	} else if (g > 1e-9) {
		lwd_adaptor2(g, in1, in2, out1, out2);
	} else if (g > -1e-9) {
		lwd_adaptor0(0.0, in1, in2, out1, out2);
	} else if (g > -0.5) {
		lwd_adaptor3(g, in1, in2, out1, out2);
	} else  {
		lwd_adaptor4(g, in1, in2, out1, out2);
	}
}

static inline void __lwd_adaptor(double g, double in1, double in2, 
							   double *out1, double *out2)
{
	lwd_adaptor4(g, in1, in2, out1, out2);
}

static double lwdf_fa_1(double g[], double st[], double i2)
{
	double o2;

	lwd_adaptor(g[0], i2, st[0], &o2, &st[0]);

	return o2;
}

static double lwdf_fb_0(double g[], double st[], double i1)
{
	double o1;

	o1 = i1;

	return o1;
}

static double lwdf_fb_2(double g[], double st[], double i1)
{
	double out;
    double x2;

    lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
    lwd_adaptor(g[1], i1, x2, &out, &st[1]);

	return out;
}

static double lwdf_fa_3(double g[], double st[], double in)
{
    double x0, x4;
	double out;

    lwd_adaptor(g[0], in, st[0], &x0, &st[0]);

    lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);

    lwd_adaptor(g[3], x0, x4, &out, &st[3]);


	return out;
}

static double lwdf_fb_4(double g[], double st[], double i1)
{
    double x1,x2,x6;
	double o1;

    lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
    lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
    lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]);
    lwd_adaptor(g[5], x1, x6, &o1, &st[5]);

	return o1;
}
 
static double lwdf_fa_5(double g[], double st[], double i2)
{
	double x0, x3, x4, x8;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &o2, &st[7]);

	return o2;
}

static double lwdf_fb_6(double g[], double st[], double i1)
{
	double x1, x2, x5, x6, x10;
	double o1;

 	lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
 	lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
 	lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]);
 	lwd_adaptor(g[5], x1, x6, &x5, &st[5]);
 	lwd_adaptor(g[10], st[9], st[10], &x10, &st[10]);
 	lwd_adaptor(g[9], x5, x10, &o1, &st[9]);

	return o1;
}

static double lwdf_fa_7(double g[], double st[], double i2)
{
	double x0, x3, x4, x7, x8, x12;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &x7, &st[7]);
 	lwd_adaptor(g[12], st[11], st[12], &x12, &st[12]);
 	lwd_adaptor(g[11], x7, x12, &o2, &st[11]); 

	return o2;
}

static double lwdf_fb_8(double g[], double st[], double i1)
{
	double x1, x2, x5, x6, x9, x10, x14;
	double o1;

 	lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
 	lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
 	lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]); 
 	lwd_adaptor(g[5], x1, x6, &x5, &st[5]);
 	lwd_adaptor(g[10], st[9], st[10], &x10, &st[10]);
 	lwd_adaptor(g[9], x5, x10, &x9, &st[9]);
 	lwd_adaptor(g[14], st[13], st[14], &x14, &st[14]);
 	lwd_adaptor(g[13], x9, x14, &o1, &st[13]);

	return o1;
}


static double lwdf_fa_9(double g[], double st[], double i2)
{
	double x0, x3, x4, x7, x8, x11, x12, x16;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &x7, &st[7]);
 	lwd_adaptor(g[12], st[11], st[12], &x12, &st[12]);
 	lwd_adaptor(g[11], x7, x12, &x11, &st[11]); 
 	lwd_adaptor(g[16], st[15], st[16], &x16, &st[16]);
 	lwd_adaptor(g[15], x11, x16, &o2, &st[15]);

	return o2;
}

static double lwdf_fb_10(double g[], double st[], double i1)
{
	double x1, x2, x5, x6, x9, x10, x13, x14, x18;
	double o1;

 	lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
 	lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
 	lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]); 
 	lwd_adaptor(g[5], x1, x6, &x5, &st[5]);
 	lwd_adaptor(g[10], st[9], st[10], &x10, &st[10]);
 	lwd_adaptor(g[9], x5, x10, &x9, &st[9]);
 	lwd_adaptor(g[14], st[13], st[14], &x14, &st[14]);
 	lwd_adaptor(g[13], x9, x14, &x13, &st[13]);
 	lwd_adaptor(g[18], st[17], st[18], &x18, &st[18]);
 	lwd_adaptor(g[17], x13, x18, &o1, &st[17]);

	return o1;
}

static double lwdf_fa_11(double g[], double st[], double i2)
{
	double x0, x3, x4, x7, x8, x11, x12, x15, x16, x20;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &x7, &st[7]);
 	lwd_adaptor(g[12], st[11], st[12], &x12, &st[12]);
 	lwd_adaptor(g[11], x7, x12, &x11, &st[11]); 
 	lwd_adaptor(g[16], st[15], st[16], &x16, &st[16]);
 	lwd_adaptor(g[15], x11, x16, &x15, &st[15]);
 	lwd_adaptor(g[20], st[19], st[20], &x20, &st[20]);
 	lwd_adaptor(g[19], x15, x20, &o2, &st[19]);

	return o2;
}

static double lwdf_fb_12(double g[], double st[], double i1)
{
	double x1, x2, x5, x6, x9, x10, x13, x14, x17, x18, x22;
	double o1;

 	lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
 	lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
 	lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]); 
 	lwd_adaptor(g[5], x1, x6, &x5, &st[5]);
 	lwd_adaptor(g[10], st[9], st[10], &x10, &st[10]);
 	lwd_adaptor(g[9], x5, x10, &x9, &st[9]);
 	lwd_adaptor(g[14], st[13], st[14], &x14, &st[14]);
 	lwd_adaptor(g[13], x9, x14, &x13, &st[13]);
 	lwd_adaptor(g[18], st[17], st[18], &x18, &st[18]);
 	lwd_adaptor(g[17], x13, x18, &x17, &st[17]);
 	lwd_adaptor(g[22], st[21], st[22], &x22, &st[22]);
 	lwd_adaptor(g[21], x17, x22, &o1, &st[21]);

	return o1;
}

static double lwdf_fa_13(double g[], double st[], double i2)
{
	double x0, x3, x4, x7, x8, x11, x12, x15, x16, x19, x20, x24;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &x7, &st[7]);
 	lwd_adaptor(g[12], st[11], st[12], &x12, &st[12]);
 	lwd_adaptor(g[11], x7, x12, &x11, &st[11]); 
 	lwd_adaptor(g[16], st[15], st[16], &x16, &st[16]);
 	lwd_adaptor(g[15], x11, x16, &x15, &st[15]);
 	lwd_adaptor(g[20], st[19], st[20], &x20, &st[20]);
 	lwd_adaptor(g[19], x15, x20, &x19, &st[19]);
 	lwd_adaptor(g[24], st[23], st[24], &x24, &st[24]); 
 	lwd_adaptor(g[23], x19, x24, &o2, &st[23]); 

	return o2;
}

static double lwdf_fb_14(double g[], double st[], double i1)
{
	double x1, x2, x5, x6, x9, x10, x13, x14, x17, x18, x21;
	double x22, x26;
	double o1;

 	lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
 	lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
 	lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]);
 	lwd_adaptor(g[5], x1, x6, &x5, &st[5]);
 	lwd_adaptor(g[10], st[9], st[10], &x10, &st[10]);
 	lwd_adaptor(g[9], x5, x10, &x9, &st[9]);
 	lwd_adaptor(g[14], st[13], st[14], &x14, &st[14]);
 	lwd_adaptor(g[13], x9, x14, &x13, &st[13]);
 	lwd_adaptor(g[18], st[17], st[18], &x18, &st[18]);
 	lwd_adaptor(g[17], x13, x18, &x17, &st[17]);
 	lwd_adaptor(g[22], st[21], st[22], &x22, &st[22]);
 	lwd_adaptor(g[21], x17, x22, &x21, &st[21]);
 	lwd_adaptor(g[26], st[25], st[26], &x26, &st[26]);
 	lwd_adaptor(g[25], x21, x26, &o1, &st[25]);

	return o1;
}

static double lwdf_fa_15(double g[], double st[], double i2)
{
	double x0, x3, x4, x7, x8, x11, x12, x15, x16, x19, x20;
	double x23, x24, x28;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &x7, &st[7]);
 	lwd_adaptor(g[12], st[11], st[12], &x12, &st[12]);
 	lwd_adaptor(g[11], x7, x12, &x11, &st[11]);
 	lwd_adaptor(g[16], st[15], st[16], &x16, &st[16]);
 	lwd_adaptor(g[15], x11, x16, &x15, &st[15]);
 	lwd_adaptor(g[20], st[19], st[20], &x20, &st[20]);
 	lwd_adaptor(g[19], x15, x20, &x19, &st[19]);
 	lwd_adaptor(g[24], st[23], st[24], &x24, &st[24]);
 	lwd_adaptor(g[23], x19, x24, &x23, &st[23]);
 	lwd_adaptor(g[28], st[27], st[28], &x28, &st[28]);
 	lwd_adaptor(g[27], x23, x28, &o2, &st[27]);

	return o2;
}

static double lwdf_fa_21(double g[], double st[], double i2)
{
	double x0, x3, x4, x7, x8, x11, x12, x15, x16, x19, x20;
	double x23, x24, x27, x28, x31, x32, x35, x36, x40;
	double o2;

 	lwd_adaptor(g[0], i2, st[0], &x0, &st[0]);
 	lwd_adaptor(g[4], st[3], st[4], &x4, &st[4]);
 	lwd_adaptor(g[3], x0, x4, &x3, &st[3]);
 	lwd_adaptor(g[8], st[7], st[8], &x8, &st[8]);
 	lwd_adaptor(g[7], x3, x8, &x7, &st[7]);
 	lwd_adaptor(g[12], st[11], st[12], &x12, &st[12]);
 	lwd_adaptor(g[11], x7, x12, &x11, &st[11]);
 	lwd_adaptor(g[16], st[15], st[16], &x16, &st[16]);
 	lwd_adaptor(g[15], x11, x16, &x15, &st[15]);
 	lwd_adaptor(g[20], st[19], st[20], &x20, &st[20]);
 	lwd_adaptor(g[19], x15, x20, &x19, &st[19]);
 	lwd_adaptor(g[24], st[23], st[24], &x24, &st[24]);
 	lwd_adaptor(g[23], x19, x24, &x23, &st[23]);
 	lwd_adaptor(g[28], st[27], st[28], &x28, &st[28]);
 	lwd_adaptor(g[27], x23, x28, &x27, &st[27]);
 	lwd_adaptor(g[32], st[31], st[32], &x32, &st[32]);
 	lwd_adaptor(g[31], x27, x32, &x31, &st[31]);
 	lwd_adaptor(g[36], st[35], st[36], &x36, &st[36]);
 	lwd_adaptor(g[35], x31, x36, &x35, &st[35]);
 	lwd_adaptor(g[40], st[39], st[40], &x40, &st[40]);
 	lwd_adaptor(g[39], x35, x40, &o2, &st[39]);

	return o2;
}


static double lwdf_fb_22(double g[], double st[], double i1)
{
	double x1, x2, x5, x6, x9, x10, x13, x14, x17, x18, x21;
	double x22, x25, x26, x29, x30, x33, x34, x37, x38, x42;
	double o1;

 	lwd_adaptor(g[2], st[1], st[2], &x2, &st[2]);
 	lwd_adaptor(g[1], i1, x2, &x1, &st[1]);
 	lwd_adaptor(g[6], st[5], st[6], &x6, &st[6]);
 	lwd_adaptor(g[5], x1, x6, &x5, &st[5]);
 	lwd_adaptor(g[10], st[9], st[10], &x10, &st[10]);
 	lwd_adaptor(g[9], x5, x10, &x9, &st[9]);
 	lwd_adaptor(g[14], st[13], st[14], &x14, &st[14]);
 	lwd_adaptor(g[13], x9, x14, &x13, &st[13]);
 	lwd_adaptor(g[18], st[17], st[18], &x18, &st[18]);
 	lwd_adaptor(g[17], x13, x18, &x17, &st[17]);
 	lwd_adaptor(g[22], st[21], st[22], &x22, &st[22]);
 	lwd_adaptor(g[21], x17, x22, &x21, &st[21]);
 	lwd_adaptor(g[26], st[25], st[26], &x26, &st[26]);
 	lwd_adaptor(g[25], x21, x26, &x25, &st[25]);
 	lwd_adaptor(g[30], st[29], st[30], &x30, &st[30]);
 	lwd_adaptor(g[29], x25, x30, &x29, &st[29]);
 	lwd_adaptor(g[34], st[33], st[34], &x34, &st[34]);
 	lwd_adaptor(g[33], x29, x34, &x33, &st[33]);
 	lwd_adaptor(g[38], st[37], st[38], &x38, &st[38]);
 	lwd_adaptor(g[37], x33, x38, &x37, &st[37]);
 	lwd_adaptor(g[42], st[41], st[42], &x42, &st[42]);
 	lwd_adaptor(g[41], x37, x42, &o1, &st[41]);

	return o1;
}

struct lwdf_sub {
	double (* fa)(double *, double *, double);
	double (* fb)(double *, double *, double);
};

const struct lwdf_sub sub_lut[] = {
	{ lwdf_fa_1, lwdf_fb_0 }, /* 1 */
	{ lwdf_fa_1, lwdf_fb_2 }, /* 3 */
	{ lwdf_fa_3, lwdf_fb_2 }, /* 5 */
	{ lwdf_fa_3, lwdf_fb_4 }, /* 7 */
	{ lwdf_fa_5, lwdf_fb_4 },
	{ lwdf_fa_5, lwdf_fb_6 },
	{ lwdf_fa_7, lwdf_fb_6 },
	{ lwdf_fa_7, lwdf_fb_8 },
	{ lwdf_fa_9, lwdf_fb_8 },
	{ lwdf_fa_9, lwdf_fb_10 },
	{ lwdf_fa_11, lwdf_fb_10 },
	{ lwdf_fa_11, lwdf_fb_12 },
	{ lwdf_fa_13, lwdf_fb_12 },
	{ lwdf_fa_13, lwdf_fb_14 },
	{ lwdf_fa_15, lwdf_fb_14 },
	{ lwdf_fa_15, lwdf_fb_14 },
	{ lwdf_fa_21, lwdf_fb_22 }
};


ssize_t lwdf_fp64_lowpass(struct lwdf_fp64 * flt, 
						  double y[], const double x[], size_t len)
{
	double (* fa)(double *, double *, double);
	double (* fb)(double *, double *, double);
	unsigned int n;
	unsigned int i;
	double * t;
	double * g;

	/* Filter order */
	n = flt->coeff.cnt;
	/* Coefficients */
	g = flt->coeff.gamma;
	/* State */
	t = flt->state.t;

#if (DEBUG > 0)
//	fprintf(stderr, "lwdf_lowpass: n=%2d idx=%d\n", n, n / 2);
#endif

	fa = sub_lut[n / 2].fa;
	fb = sub_lut[n / 2].fb;

	for (i = 0; i < len; ++i) {
		double ya = fa(g, t, x[i]);
		double yb = fb(g, t, x[i]);

		y[i] = (ya + yb) / 2;
	}

	return len;
}

/* High Pass */
ssize_t lwdf_fp64_higpass(struct lwdf_fp64 * flt, double y[], const double x[], size_t len)
{
	double (* fa)(double *, double *, double);
	double (* fb)(double *, double *, double);
	unsigned int n;
	unsigned int i;
	double * t;
	double * g;

	assert(flt != NULL);
	assert(y != NULL);
	assert(x != NULL);

	/* Filter order */
	n = flt->coeff.cnt;
	/* Coefficients */
	g = flt->coeff.gamma;
	/* State */
	t = flt->state.t;

	fa = sub_lut[n / 2].fa;
	fb = sub_lut[n / 2].fb;

	for (i = 0; i < len; ++i) {
		double y0 = fa(g, t, x[i]);
		double y1 = fb(g, t, x[i]);

		y[i] = (y0 - y1) / 2;
	}

	return len;
}


struct lwdf_fp64 *lwdf_fp64_new(double samplerate)
{
	struct lwdf_fp64 * flt;
	unsigned int size = sizeof(struct lwdf_fp64);

	assert(samplerate >= 0);

	if ((flt = calloc(1, size)) == NULL) {
		fprintf(stderr, "%s: calloc() failed: %s", __func__,
			strerror(errno));
		return NULL;
	};

	flt->coeff.max = LWDF_COEFF_MAX;
	flt->state.max = LWDF_STATE_MAX;
	flt->samplerate = samplerate;

	return flt;
}

int lwdf_fp64_free(struct lwdf_fp64 *flt)
{
	if (flt == NULL) {
		fprintf(stderr, "%s: NULL pointer.", __func__);
		return -1;
	};

	free(flt);

	return 0;
}

/* g == 0 */
int lwdf_fp64_init(struct lwdf_fp64 * flt, double samplerate)
{
	unsigned int i;

	assert(flt != NULL);
	assert(samplerate >= 0);

	flt->coeff.cnt = 0;
	/* Filter order (number of coefficients) */
	for (i = 0; i < LWDF_COEFF_MAX; ++i) {
		flt->coeff.gamma[i] = 0.0;
	}

	flt->state.cnt = 0;
	/* Filter internal state variables (delays) */
	for (i = 0; i < LWDF_STATE_MAX; ++i) {
		flt->state.t[i] = 0.0;
	}

	flt->coeff.max = LWDF_COEFF_MAX;
	flt->state.max = LWDF_STATE_MAX;
	flt->samplerate = samplerate;

	return 0;
}

static void __lwdf_fp64_reset(struct lwdf_fp64 * flt)
{
	unsigned int i;

	/* Clear internal state */
	for (i = 0; i < flt->state.cnt; ++i) {
		flt->state.t[i] = 0.0;
	}

}

int lwdf_fp64_reset(struct lwdf_fp64 * flt)
{
	assert(flt != NULL);

	/* Clear internal state */
	__lwdf_fp64_reset(flt);

	return 0;
}

ssize_t lwdf_fp64_gamma_set(struct lwdf_fp64 * flt, const double gamma[], 
							size_t cnt)
{
	unsigned int i;

	assert(flt != NULL);
	assert(gamma != NULL);
	assert(cnt < LWDF_COEFF_MAX);

	/* Set the coefficients */
	for (i = 0; i < LWDF_COEFF_MAX; ++i) {
		flt->coeff.gamma[i] = gamma[i];
	}

	/* Adjust filter state and order */
	flt->coeff.cnt = cnt;
	flt->state.cnt = cnt;

	/* Clear internal state */
	__lwdf_fp64_reset(flt);

	return cnt;
}


ssize_t lwdf_fp64_gamma_get(struct lwdf_fp64 * flt, double gamma[], size_t max)
{
	unsigned int i;
	unsigned int n;

	assert(flt != NULL);

	n = flt->coeff.cnt;

	if (gamma != NULL) {
		/* read up to max coefficients */
		if (max < n)
			n = max;

		for (i = 0; i < n; ++i) {
			gamma[i] = flt->coeff.gamma[i];
		}
	}

	return n;
}

double lwdf_fp64_samplerate_get(struct lwdf_fp64 * flt)
{
	assert(flt != NULL);

	return flt->samplerate;
}

double lwdf_fp64_coeff_get(struct lwdf_fp64 * flt, unsigned int idx)
{
	assert(flt != NULL);
	assert(idx < flt->coeff.cnt);

	return flt->coeff.gamma[idx];
}

void lwdf_fp64_coeff_set(struct lwdf_fp64 * flt, 
						unsigned int idx, double coeff)
{
	assert(flt != NULL);
	assert(idx < flt->coeff.cnt);

	if (flt->coeff.gamma[idx] != coeff) {
		flt->coeff.gamma[idx] = coeff;
	}
}


