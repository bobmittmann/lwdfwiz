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

static const double buf_x[]= {100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 700.0};
static const double buf_y[]= {100.0, 114.0, 120.0, 180.0, 136.0, 210.0, 150.0};

void lwdf_freq_response_plot_init(GtkWidget * darea)
{
	struct plt_figure * figure;
	struct plt_series * series;
	double x[1024];
	double y[1024];
	int cnt;
	int i;

	figure = plt_figure_new("My Plot", "Frequency [Hz]", "Magnitude [dB]");

	fprintf(stderr, "figure=%p.\n", figure);


	series = plt_line_series_new(figure, "Test Data", 1024);
	cnt = sizeof(buf_x) / sizeof(double);
	for (i = 0; i < cnt; ++i) {
		x[i] = buf_x[i];
		y[i] = buf_y[i];
	}
	plt_line_series_insert(series, x, y, cnt);
	plt_series_line_color_set(series, 1.0, 0.0, 0.0, 1.0);
	plt_series_line_width_set(series, 4.0);

	plt_figure_widget_connect(figure, darea);

	fprintf(stderr, "3.\n");
}

