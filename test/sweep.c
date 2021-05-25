/*
 * sweep(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	sweep.c
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

#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include "pcm.h"

#define APP_NAME "sweep"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

static char *progname;

void do_filter(float y[], unsigned int ny, float x[], unsigned int nx);


int do_pcmfloat_sweep(const char *prefix, float samplerate,
		      int oversample, int decimation, int interleave,
		      float f0, float f1,
		      float ampl, float duration, float silence)
{
	bool enable_png = true;
	char name[128];
	struct pcmfloat *x;
	struct pcmfloat *y;
	struct pcmfloat *xi;
	uint32_t nsamples;
	uint32_t xsamples;
	uint32_t ysamples;
	unsigned int j;
	unsigned int i;
	int ret = 0;

	if (oversample <= 1)
		oversample = 1;
	if (oversample > 1024)
		oversample = 1024;

	if (interleave <= 1)
		interleave = 1;

	if (decimation <= 1)
		decimation = 1;

	/* number of samples in a sweep period */
	nsamples = samplerate * (duration + silence);
	xsamples = nsamples * decimation;
	ysamples = nsamples * oversample;

	/* create PCM buffers */

	/* input */
	x = pcmfloat_create(xsamples, samplerate);
	/* interleaved input */
	xi = pcmfloat_create(xsamples * interleave, samplerate);
	/* output */
	y = pcmfloat_create(ysamples, samplerate * oversample / decimation);

	printf("\n");
	printf("Sweep: %.1f .. %.1f Hz in %.2f sec.\n", f0, f1, duration);
	printf("Input: %d samples at %.1f SPS.\n", x->len, x->samplerate);
	printf("Output: %d samples at %.1f SPS.\n", y->len, y->samplerate);
	fflush(stdout);

	if (duration > 120) {
		uint32_t sec;
		uint32_t min;
		uint32_t hour;
		uint32_t tmp;

		tmp = duration;
		min = tmp / 60;
		sec = tmp - (min * 60);

		tmp = min;
		hour = tmp / 60;
		min = tmp - (hour * 60);

		printf("Duration: %d:%02d:%02d.\n", hour, min, sec);
		fflush(stdout);
	}

	printf("Samplerate: %.2f sps\n"
	       "Amplitude: %.2f\n"
	       "Oversample: %d\n"
	       "Decimation: %d\n"
	       "Interleave: %d\n",
	       samplerate, ampl, oversample, decimation, interleave);
	fflush(stdout);

	printf("Creating a sweep waveform...\n");
	fflush(stdout);
	pcmfloat_sweep(x, f0, f1, ampl);

	sprintf(name, "%sx", prefix);
	printf("Writing %s files...\n", name);
	fflush(stdout);
	pcmfloat_plot(name, x, "lc rgb '#108010' pt 0 lt 1 lw 1", enable_png);

	/* Interleave */
	for (i = 0; i < x->len; ++i) {
		for (j = 0; j < interleave; ++j) {
			xi->sample[(i * interleave) + j] = x->sample[i];
		}
/*		xi->sample[(i * interleave)] = x->sample[i];
		for (j = 1; j < interleave; ++j) {
			xi->sample[(i * interleave) + j] = 0;
		} */
	}

	printf("Applying filter ...\n");
	fflush(stdout);

	do_filter(y->sample, y->len, xi->sample, xi->len);

	sprintf(name, "%sy", prefix);
	printf("Writing %s files...\n", name);
	fflush(stdout);

	pcmfloat_plot(name, y, "lc rgb '#c01010' pt 0 lt 1 lw 1", enable_png);

	pcmfloat_destroy(xi);
	pcmfloat_destroy(x);
	pcmfloat_destroy(y);

	return ret;
}

void system_cleanup(void)
{
}

static void show_usage(void)
{
	fprintf(stderr, "Usage: %s [OPTION...]\n", progname);
	fprintf(stderr, "  -h  \tShow this help message\n");
	fprintf(stderr, "  -v  \tShow version\n");
	fprintf(stderr, "  -p 'PREFIX'\tprefix output files\n");
	fprintf(stderr, "  -o 'FILE'\tOutput to a file\n");
	fprintf(stderr, "  -f 'FREQ'\tStart frequency (Hz)."
		" Default to 200Hz.\n");
	fprintf(stderr, "  -F 'FREQ'\tStop frequency (Hz)."
		" Default to 4KHz.\n");
	fprintf(stderr, "  -s 'SAMPLERATE'\tsamplerate (samples/seconds)\n");
	fprintf(stderr, "  -t 'DURATION'\tSweep playing time (seconds)\n");
	fprintf(stderr, "  -q 'SILENCE'\tquiet (seconds)\n");
	fprintf(stderr, "  -x 'OVERSAMPLE'\toversampling factor\n");
	fprintf(stderr, "  -d 'DECIMATE'\tdecimation factor\n");
	fprintf(stderr, "  -i 'INTERLEAVE'\tinterleaving factor\n");
	fprintf(stderr, "  -a 'VAL'\tAmplitude (0.0 <= a <= 1.0) \n");
	fprintf(stderr, "\n");
}

static void show_version(void)
{
	fprintf(stderr, "%s %d.%d\n", APP_NAME, VERSION_MAJOR, VERSION_MINOR);
}

/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */

#define FNAME_MAX_LEN 128
#define PREFIX_MAX_LEN 32

int main(int argc, char *argv[])
{
	char out_fname[FNAME_MAX_LEN + 1] = "";
	char prefix[PREFIX_MAX_LEN + 1] = "";
	float ampl = 0.5;
	float f0 = 200;
	float f1 = 4000;
	float duration = 2.0;
	float silence = 0.0;
	float samplerate = 11025;
	int decimate = 1;
	int oversample = 1;
	int interleave = 1;
	int c;

	/* the program name start just after the last slash */
	if ((progname = (char *)strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "vhd:f:F:a:d:i:o:p:q:s:t:x:")) > 0) {
		switch (c) {
		case 'v':
			show_version();
			return 0;
		case 'h':
			show_usage();
			return 1;
		case 'f':
			f0 = strtof(optarg, NULL);
			break;
		case 'F':
			f1 = strtof(optarg, NULL);
			break;
		case 's':
			samplerate = strtof(optarg, NULL);
			break;
		case 'o':
			strncpy(out_fname, optarg, FNAME_MAX_LEN);
			break;
		case 'p':
			strncpy(prefix, optarg, PREFIX_MAX_LEN);
			break;
		case 'a':
			ampl = strtof(optarg, NULL);
			break;
		case 't':
			duration = strtof(optarg, NULL);
			if (duration < 0) {
				fprintf(stderr, "Invalid duration.\n");
				return 1;
			}
			break;
		case 'q':
			silence = strtof(optarg, NULL);
			if (silence < 0) {
				silence = 0;
			}
			break;
		case 'x':
			oversample = strtoul(optarg, NULL, 10);
			break;
		case 'd':
			decimate = strtoul(optarg, NULL, 10);
			break;
		case 'i':
			interleave = strtoul(optarg, NULL, 10);
			break;
		default:
			show_usage();
			return 2;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Unexpected positional argument\n");
		return 3;
	}

	printf("\n");
	printf("TDM Sweep Generator. %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
	printf("(C) Copyright 2018, Bob Mittmann.\n");

	do_pcmfloat_sweep(prefix, samplerate, oversample, decimate,
			  interleave, f0, f1, ampl, duration, silence);

	system_cleanup();

	return 0;
}

