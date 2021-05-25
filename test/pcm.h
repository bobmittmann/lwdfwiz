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

struct pcm16 {
	uint32_t len;
	uint32_t samplerate;
	int16_t sample[];
};

struct pcmfloat {
	uint32_t len;
	float samplerate;
	float sample[];
};


#ifdef __cplusplus
extern "C" {
#endif

struct pcm16 *pcm16_create(unsigned int len, unsigned int samplerate);

int pcm16_destroy(struct pcm16 *pcm);

/* Add a sinusoidal waveform to the existing one */
int pcm16_sin(struct pcm16 *pcm, float freq, float amp);

/* Add a sinusoidal sweep */
int pcm16_sweep(struct pcm16 *pcm, float f0, float f1, float amp);

/* Add white noise to the the waveform */
int pcm16_white_noise(struct pcm16 *pcm, float level);

/* write the waveform into a file suitable for gnuplot */
int pcm16_dump(const char *fname, struct pcm16 *pcm);

int pcm16_plot(const char *name, struct pcm16 *pcm);

int pcm16_c_dump(const char *fname, struct pcm16 *pcm);

struct pcmfloat *pcmfloat_create(unsigned int len, float samplerate);

int pcmfloat_destroy(struct pcmfloat *pcm);

/* write the waveform into a file suitable for gnuplot */
int pcmfloat_dump(const char *fname, struct pcmfloat *pcm);

/* write the waveform into a file suitable for gnuplot */
int pcmfloat_plot(const char *name, struct pcmfloat *pcm,
				  const char *lstyle, bool png);

int pcmfloat_sweep(struct pcmfloat *pcm, float f0, float f1, float a);

int pcmfloat_logsweep(struct pcmfloat *pcm, float f0, float f1, float a);


#ifdef __cplusplus
}
#endif
#endif				/* __PCM_H__ */
