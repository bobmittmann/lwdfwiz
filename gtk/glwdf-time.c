/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	glwdf-app.c
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

int pcm_fp64_linspace(double y[], unsigned int npts, double from, double to)
{
	double itv = to - from;
	double dw = itv / npts;
	double w;
	double w0 = from;
	unsigned i;

	for (i = 0, w = w0; i < npts; ++i, w += dw) {
		y[i] = w;
	}

	return npts;
}

/* normalized tone generator */
int pcm_fp64_tone(double y[], unsigned int npts, double freq, 
				  double ampl, double phase)
{
	double itv = 2 * M_PI;
	double dw = itv / npts;
	double w;
	double w0 = 0.0;
	unsigned i;

	w0 = phase;

	for (i = 0, w = w0; i < npts; ++i, w += dw) {
		y[i] = ampl * sin(freq * w  + w0);
	}

	return npts;
}

PlotFigure * glwdf_time_response_plot_init(struct lwdf_fp64 * flt)
{
	struct plt_figure * figure;
	double pos[128];
	char * lbl[128];
	char buf[128 * 16];
	int n;
	int i;

	figure = plt_figure_new("LWDF Time Response", "Time [sec]", "Amplitude");

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

	plt_figure_yspan_set(figure, -2.0, 2.0);
	plt_mk_lin_ticks(pos, lbl, -2.0, 2.0, n);
	plt_figure_yticks_set(figure, pos, lbl, n);
	
	return figure;
}


double x[65536];
double y[65536];
double z[65536];

PlotFigure * dbg_figure; 

GtkWidget * glwdf_time_plot_init(GtkBuilder * builder, struct lwdf_fp64 * flt)
{
	unsigned int npts = 2048;
 //	struct plt_series * series;
	GtkWidget * window;
	GtkWidget * drawing;
	PlotFigure * figure; 

	window = GTK_WIDGET(gtk_builder_get_object(builder, "wnd_time"));
	drawing = GTK_WIDGET(gtk_builder_get_object(builder, "time_plot"));

	/* Create a new plot figure */
	figure = glwdf_time_response_plot_init(flt);
	dbg_figure = figure;

	/* Connect the figure to the widget */
	plt_figure_widget_connect(figure, drawing);

	/* Add some traces (series) */

	pcm_fp64_linspace(x, npts, 0, 2*M_PI);
	pcm_fp64_tone(y, npts, 500, 1.0, 0);

	lwdf_fp64_lowpass(flt, z, y, npts);

/*	series = plt_line_series_new(figure, "Tone", npts);
	plt_series_line_color_set(series, 1.0, 0.0, 0.0, 1.0);
	plt_series_line_width_set(series, 0.5);
	plt_line_series_set(series, x, y, npts);

	series = plt_line_series_new(figure, "Low pass", npts);
	plt_series_line_color_set(series, 0.2, 0.2, 1.0, 1.0);
	plt_series_line_width_set(series, 0.5);
	plt_line_series_set(series, x, z, npts); */

	return window;
}


