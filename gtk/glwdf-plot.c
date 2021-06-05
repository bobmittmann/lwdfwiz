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

static void plt_series_sine(struct plt_series * series, double freq, unsigned int npts)
{
	double x[1024 + 8];
	double y[1024 + 8];
	double itv = 2 * M_PI;
	double dw = itv / npts;
	double w;
	unsigned int i;

	for (i = 0, w = 0; i < npts; ++i, w += dw) {
		x[i] = w;
	}

	for (i = 0; i < npts; ++i) {
		y[i] = sin(freq * x[i]);
	}

	plt_line_series_set(series, x, y, npts);
}


PlotFigure * glwdf_freq_response_plot_init(GtkWidget * darea)
{
	struct plt_figure * figure;
	struct plt_series * series;
	unsigned int npts = 1024;
	double pos[128];
	char * lbl[128];
	char buf[128 * 16];
	int n;
	int i;

	figure = plt_figure_new("LWDF Response", "Frequency [Hz]", "Magnitude [dB]");

#if (DEBUG)
	fprintf(stderr, "figure=%p\n", figure);
#endif

	series = plt_line_series_new(figure, "Test Data", npts);

	// fprintf(stderr, "1.\n");
	plt_series_line_color_set(series, 1.0, 0.0, 0.0, 1.0);
	plt_series_line_width_set(series, 2.0);

	/* set the string pointers */
	n = 11;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_xspan_set(figure, 0, 2 * M_PI);
	plt_mk_log10_ticks(pos, lbl, 0.01 * M_PI, 2 * M_PI, n);
	plt_figure_xticks_set(figure, pos, lbl, n);


	/* set the string pointers */
	n = 9;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_yspan_set(figure, -1.0, 1.0);
	plt_mk_lin_ticks(pos, lbl, -1.0, 1.0, n);
	plt_figure_yticks_set(figure, pos, lbl, n);
	
	plt_series_sine(series, 100, npts);

	plt_figure_widget_connect(figure, darea);

	return figure;
}


PlotFigure * glwdf_time_response_plot_init(GtkWidget * darea)
{
	struct plt_figure * figure;
	struct plt_series * series;
	unsigned int npts = 1024;
	double pos[128];
	char * lbl[128];
	char buf[128 * 16];
	int n;
	int i;

	figure = plt_figure_new("LWDF Response", "Frequency [Hz]", "Magnitude [dB]");

	fprintf(stderr, "figure=%p\n", figure);

	series = plt_line_series_new(figure, "Test Data", npts);

	// fprintf(stderr, "1.\n");
	plt_series_line_color_set(series, 1.0, 0.0, 0.0, 1.0);
	plt_series_line_width_set(series, 2.0);

	/* set the string pointers */
	n = 11;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_xspan_set(figure, 0, 2 * M_PI);
	plt_mk_lin_ticks(pos, lbl, 0.0, 2 * M_PI, n);
	plt_figure_xticks_set(figure, pos, lbl, n);


	/* set the string pointers */
	n = 9;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_yspan_set(figure, -1.0, 1.0);
	plt_mk_lin_ticks(pos, lbl, -1.0, 1.0, n);
	plt_figure_yticks_set(figure, pos, lbl, n);
	
	plt_series_sine(series, 2, npts);

	plt_figure_widget_connect(figure, darea);

	return figure;
}



