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
#include "conf.h"

#define PROG_NAME "lwdfwiz"
#define VERSION_MAJOR 1
#define VERSION_MINOR 0

static char *progname;


struct lwdfwiz_cfg conf = {
	.wiz = {
		.samplerate = 88200,
		.ftype = LWDF_ELLIP,
		.order = 13,
		.nbits = 0,
		.bi = false,
		.id = false,
		.rx = false,
		.ff = false,
		.asmin = 6.0,
		.as = 6.0,
		.es = 0.0,
		.fs = 22100.0,
		.ap = 0.0,
		.ep = 0.0,
		.fp = 0.0
	},
	.prefix = "lp",
	.cfname = "../test/filter.c",
	.jlfname = "test.jl"
};

/*
 * Configuration profile
 */
/* *INDENT-OFF* */
BEGIN_SECTION(wiz)
       DEFINE_FLOAT("samplerate", &conf.wiz.samplerate)
       DEFINE_INT("ftype", &conf.wiz.ftype)
       DEFINE_INT("nbits", &conf.wiz.nbits)
       DEFINE_FLOAT("asmin", &conf.wiz.asmin)
       DEFINE_FLOAT("as", &conf.wiz.as)
       DEFINE_FLOAT("es", &conf.wiz.es)
       DEFINE_FLOAT("fs", &conf.wiz.fs)
       DEFINE_FLOAT("ap", &conf.wiz.ap)
       DEFINE_FLOAT("ep", &conf.wiz.ep)
       DEFINE_FLOAT("fp", &conf.wiz.fp)
       DEFINE_BOOLEAN("bi", &conf.wiz.bi)
       DEFINE_BOOLEAN("id", &conf.wiz.id)
       DEFINE_BOOLEAN("rx", &conf.wiz.rx)
       DEFINE_BOOLEAN("ff", &conf.wiz.ff)
END_SECTION
/* *INDENT-ON* */

BEGIN_SECTION(conf_root)
       DEFINE_SECTION("wizard", &wiz)
       DEFINE_STRING("prefix", &conf.prefix)
       DEFINE_STRING("cfname", &conf.prefix)
       DEFINE_STRING("jlfname", &conf.jlfname)
END_SECTION

const char *confpath = "lwdwiz.conf";

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
	fprintf(stderr, "%s %d.%d\n", PROG_NAME, VERSION_MAJOR, VERSION_MINOR);
}


#define FNAME_MAX_LEN 128

/**
 * @brief	main entry
 * @param	command line argument
 * @return	error code
 */

int main(int argc, char *argv[])
{
	char outname[FNAME_MAX_LEN + 1] = "";
	double samplerate = 11025;
	bool jlgen = false;
	bool gmgen = false;
	bool cgen = false;
	bool hgen = false;
	bool do_magic = true;
	bool outname_set = false;
	int decimate = 0;
	int oversample = 1;
	int interleave = 1;
	int verbose = 1;
	int quiet = 0;
	struct lwdfwiz_param wiz;
	struct lwdf_info inf;
	FILE *fout;
	int c;
	int i;

	/* the program name start just after the last slash */
	if ((progname = (char *)strrchr(argv[0], '/')) == NULL)
		progname = argv[0];
	else
		progname++;

	/* Try loading configuration from file */
	if (load_conf(confpath, conf_root) <= 0) {
		/* Write back the defaults */
		save_conf(confpath, conf_root);
	}

	/* copy from configuration */
	wiz = conf.wiz;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "VH?hvqgjcdho:F:xdib:n:t:")) > 0) {
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
			wiz.samplerate = strtof(optarg, NULL);
			break;
		case 'a':
			wiz.as = strtof(optarg, NULL);
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
		case 'b':
			wiz.nbits = strtoul(optarg, NULL, 10);
			break;
		case 'N':
			wiz.order = strtoul(optarg, NULL, 10);
			break;
		case 't':
			wiz.ftype = strtoul(optarg, NULL, 10);
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
		fprintf(stderr, " LWDF Wizard. %d.%d\n", 
				VERSION_MAJOR, VERSION_MINOR);
		fprintf(stderr, " Lattice Wave Digital Filters Generator\n"); 
		fprintf(stderr, " (C) Copyright 2021, Bob Mittmann.\n");
		fprintf(stderr, "\n");
	}

	/* readback */

	if (do_magic) {
		if (lwdfwiz_term(&wiz, &inf) < 0) {
			fprintf(stderr, "!Error!1\n");
			return 1;
		}

		/* write back to configuration */
		conf.wiz = wiz;
		/* save configuration */
		save_conf(confpath, conf_root);
	}

	if (gmgen) {
		unsigned int N = wiz.order;

		for (i = 0; i < N; i++) {
			fprintf(fout, " gamma[%2d] = ", i);
			if (inf.gamma[i] == 0.0)
				fprintf(fout, " 0.000000000\n");
			else
				fprintf(fout, "%+12.9f\n", inf.gamma[i]);
		}

		if (outname_set) {
			if (verbose) {
				fprintf(stderr, "gamma coefficients saved to file %s\n", 
						outname);
			}
			fclose(fout);
		}

		return 0;
	}

	lwdf_cgen(fout, &wiz, &inf);

	if (outname_set) {
		if (verbose) {
			fprintf(stderr, "C code saved to file %s\n", outname);
		}
		fclose(fout);
	}

	return 0;
}

