/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	glwdf-plot.c
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

#include "plot.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "glwdf.h"
#include "pcm.h"

static void _figure_freqresp_init(struct plt_figure * figure)
{
	double pos[128];
	char * lbl[128];
	char buf[128 * 16];
	int n;
	int i;

	/* initialize axis */

	/* set the string pointers */
	n = 11;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_xspan_set(figure, 0, 2 * M_PI);

	/* set the string pointers */
	n = 9;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_yspan_set(figure, -1.0, 1.0);
	plt_mk_lin_ticks(pos, lbl, -1.0, 1.0, n);
	plt_figure_yticks_set(figure, pos, lbl, n);

	plt_mk_log10_ticks(pos, lbl, 0.01 * M_PI, 2 * M_PI, n);
	plt_figure_xticks_set(figure, pos, lbl, n);
}

struct plt_figure * plt_figure_freq_resp_new(const char * title)
{
	struct plt_figure * figure;

	figure = plt_figure_new(title, "Frequency [Hz]", "Magnitude [dB]");

	_figure_freqresp_init(figure);

	return figure;
}

