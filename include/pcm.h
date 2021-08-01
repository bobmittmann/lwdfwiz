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


#ifndef __PCM_H__
#define __PCM_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <complex.h>


struct pcm16 {
	uint32_t len;
	uint32_t samplerate;
	int16_t sample[];
};

struct pcm_fp32 {
	uint32_t len;
	float samplerate;
	float sample[];
};

struct pcm_fp64 {
	struct  {
		uint32_t len;
		float samplerate;
	} hdr;
	double sample[];
};

#ifdef __cplusplus
extern "C" {
#endif

struct pcm_i16 *pcm_i16_create(unsigned int len, unsigned int samplerate);

int pcm_i16_destroy(struct pcm_i16 *pcm);

/* Add a sinusoidal waveform to the existing one */
int pcm_i16_sin(struct pcm_i16 *pcm, float freq, float amp);

/* Add a sinusoidal sweep */
int pcm_i16_sweep(struct pcm_i16 *pcm, float f0, float f1, float amp);

/* Add white noise to the the waveform */
int pcm_i16_white_noise(struct pcm_i16 *pcm, float level);

/* write the waveform into a file suitable for gnuplot */
int pcm_i16_dump(const char *fname, struct pcm_i16 *pcm);

int pcm_i16_plot(const char *name, struct pcm_i16 *pcm);

int pcm_i16_c_dump(const char *fname, struct pcm_i16 *pcm);

/* Floating point */

struct pcm_fp64 *pcm_fp64_create(unsigned int len, double samplerate);

int pcm_fp64_destroy(struct pcm_fp64 *pcm);

/* write the waveform into a file suitable for gnuplot */
int pcm_fp64_dump(const char *fname, struct pcm_fp64 *pcm);

/* write the waveform into a file suitable for gnuplot */
int pcm_fp64_plot(const char *name, struct pcm_fp64 *pcm,
				  const char *lstyle, bool png);

int pcm_fp64_sweep(struct pcm_fp64 *pcm, double f0, double f1, double a);

int pcm_fp64_logsweep(struct pcm_fp64 *pcm, double f0, double f1, double a);

ssize_t pcm_fp64_cosine(double y[], size_t size, double w0);

ssize_t pcm_fp64_wnd_blackman(double y[], size_t npts, double alpha);

complex double pcm_fp64_gortzel_dft(const double x[], size_t len, double w);



/* Single precision */
struct pcm_fp32 *pcm_fp32_create(unsigned int len, float samplerate);

int pcm_fp32_destroy(struct pcm_fp32 *pcm);

/* write the waveform into a file suitable for gnuplot */
int pcm_fp32_dump(const char *fname, struct pcm_fp32 *pcm);

/* write the waveform into a file suitable for gnuplot */
int pcm_fp32_plot(const char *name, struct pcm_fp32 *pcm,
				  const char *lstyle, bool png);

int pcm_fp32_sweep(struct pcm_fp32 *pcm, float f0, float f1, float a);

int pcm_fp32_logsweep(struct pcm_fp32 *pcm, float f0, float f1, float a);

#ifdef __cplusplus
}
#endif
#endif				/* __PCM_H__ */
