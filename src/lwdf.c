/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	lwdf.c
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

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include "lwdf.h"

#define APP_NAME "sweep"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

static char *progname;

#define MAXSTR 128	
#define FNAMECODE "test/filter.c"
#define FNAMEGAMMA "lwdf_gamma.txt"


/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */

#define FNAME_MAX_LEN 128
#define PREFIX_MAX_LEN 32


void system_cleanup(void)
{
}

static void show_usage(void)
{
	fprintf(stderr, "Usage: %s [OPTION...]\n", progname);
	fprintf(stderr, "  -h  \tShow this help message\n");
	fprintf(stderr, "  -v  \tShow version\n");
	fprintf(stderr, "  -o \t'FILE'\tOutput to a file\n");
	fprintf(stderr, "  -g \tgenerate a gamma coefficients file\n");
	fprintf(stderr, "  -F \t'FREQ'\tSamplerate frequency [Hz]"
			" Default to 44100KHz.\n");
	fprintf(stderr, "  -c \t'FREQ'\tCutoff frequency [Hz]"
		" Default to 200Hz.\n");
	fprintf(stderr, "  -a \t'ATTENUATION'\tpassband attenuation [dB]\n");
	fprintf(stderr, "  -x \t'OVERSAMPLE'\toversampling factor\n");
	fprintf(stderr, "  -d \t'DECIMATE'\tdecimation factor\n");
	fprintf(stderr, "  -i \t'INTERLEAVE'\tinterleaving factor\n");
	fprintf(stderr, "\n");
}

static void show_version(void)
{
	fprintf(stderr, "%s %d.%d\n", APP_NAME, VERSION_MAJOR, VERSION_MINOR);
}
int main(int argc, char *argv[])
{
	char outname[FNAME_MAX_LEN + 1] = "";
	double samplerate = 11025;
	bool jlgen = false;
	bool gmgen = false;
	bool cgen = false;
	bool hgen = false;
	bool outname_set = false;
	int decimate = 0;
	int oversample = 1;
	int interleave = 1;
	int verbose = 1;
	int quiet = 0;
	struct lwdf_info inf;
	FILE *fout;
	int c;
	int i;
	int N;

	/* the program name start just after the last slash */
	if ((progname = (char *)strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "VH?hvqgjcdho:F:xdi")) > 0) {
		switch (c) {
		case 'V':
			show_version();
			return 0;
		case '?':
		case 'H':
			show_usage();
			return 1;
		case 'v':
			verbose++;
			break;
		case 'q':
			quiet++;
			break;
		case 'g':
			gmgen = true;
			break;
		case 'j':
			jlgen = true;
			break;
		case 'c':
			cgen = true;
			break;
		case 'h':
			hgen = true;
			break;
		case 'o':
			strcpy(outname, optarg);
			outname_set = true;
			break;
		case 'F':
			samplerate = strtof(optarg, NULL);
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

	(void)samplerate;
	(void)decimate;
	(void)oversample;
	(void)interleave;

	if (optind < argc) {
		fprintf(stderr, "Unexpected positional argument\n");
		return 3;
	}

	if (quiet) 
		verbose = 0;

	/* command line arguments sanity check */
	if (cgen && hgen) {
		fprintf(stderr, "incompatible options: -o -c -h\n");
		return 1;
	}

	if (cgen && jlgen) {
		fprintf(stderr, "incompatible options: -o -c -j\n");
		return 1;
	}

	if (cgen && gmgen) {
		fprintf(stderr, "incompatible options: -o -c -g\n");
		return 1;
	}

	if (outname_set) {
		if ((fout = fopen(outname, "w")) == NULL) {
			fprintf(stderr, "ERROR: creating file \"%s\": %s\n", 
					outname, strerror(errno));  
			return 1;
		}
	} else { 
		fout = stdout;
	}

	if (verbose) {
		fprintf(stderr, "\n");
		fprintf(stderr, "LWDF (Lattice Wave Digital Filters) Generator. %d.%d\n", 
				VERSION_MAJOR, VERSION_MINOR);
		fprintf(stderr, "(C) Copyright 2021, Bob Mittmann.\n");
		fprintf(stderr, "\n");
	}

	if (lwdf_wiz(&inf) < 0) {
		fprintf(stderr, "!Error!1\n");
		return 1;
	}

	if (gmgen) {
		N = inf.order;

		for (i = 0; i < N; i++) {
			fprintf(fout, " gamma[%2d] = ", i);
			if (inf.gamma[i] == 0.0)
				fprintf(fout, " 0.000000000\n");
			else
				fprintf(fout, "%+12.9f\n", inf.gamma[i]);
		}

		if (outname_set) {
			if (verbose) {
				fprintf(stderr, "gamma coefficients saved to file %s\n", outname);
			}
			fclose(fout);
		}

		return 0;
	}

	lwdf_cgen(fout, &inf);

	if (outname_set) {
		if (verbose) {
			fprintf(stderr, "C code saved to file %s\n", outname);
		}
		fclose(fout);
	}

	return 0;
}

