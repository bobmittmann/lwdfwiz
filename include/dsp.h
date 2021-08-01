/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	dsp.h
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

#ifndef __DSP_H__
#define __DSP_H__

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <complex.h>

enum dsp_filt_topology {
	DSP_FILT_LWDF;
	DSP_FILT_LWDF_BI; /* Bireciprocal LWDF */
	DSP_FILT_IIR_BQ_CASCADE;
	DSP_FILT_IIR_BQ_PARALLEL;
	DSP_FILT_FIR;
};

enum dsp_core_topology {
	DSP_CORE_FLOAT32,
	DSP_CORE_FLOAT64,
	DSP_CORE_INT32,
	DSP_CORE_INT16
};

/* DSP filter */
struct dsp_filt;

#ifdef __cplusplus
extern "C" {
#endif

struct dsp_filt * dsp_filt_new(enum dsp_filt_topology, double samplerate);

int dsp_filt_free(struct dsp_filt *flt);

int dsp_filt_init(struct dsp_filt *flt, double samplerate);


#ifdef  __cplusplus
}
#endif

#endif	/* __DSP_H__ */

