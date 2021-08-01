/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	plot-figure.c
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

#define __PLOT_CORE__
#include "plot-i.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#ifndef DEBUG
#define DEBUG 0
#endif

struct plt_series * plt_line_series_new(struct plt_figure * figure, 
										const char * label, 
										size_t max)
{
	struct plt_series * series;
	struct plt_point * points;
	unsigned int pos;

	assert(figure != NULL);
	assert(max > 1);
	
	if ((pos = figure->series.cnt) == PLT_SERIES_MAX) {
		return NULL;
	}

	if ((points = g_new(struct plt_point, max + 1)) == NULL) {
		return NULL;
	}

	if ((series = g_new(struct plt_series , 1)) == NULL) {
		g_free(points);
		return NULL;
	}

	/* FIXME: mutual exclusion */
	figure->series.entry[pos] = series;
	figure->series.cnt = pos + 1;

	series->data = points;
	series->figure = figure;
	series->info.line.nmax = max;
	series->info.line.nseg = 0;
	series->info.line.min.x = INFINITY;
	series->info.line.min.y = INFINITY;
	series->info.line.max.x = -INFINITY;
	series->info.line.max.y = -INFINITY;


	if (label != NULL) {
		strncpy(series->label, label, PLT_SERIES_LABEL_MAX); 
	} else {
		sprintf(series->label, "%d", pos);
	}

	series->attr.line.color.r = 0.5;
	series->attr.line.color.g = 0.5;
	series->attr.line.color.b = 0.5;
	series->attr.line.color.a = 1.0;

	return series;
}

int plt_series_line_color_set(struct plt_series * series,
							  const struct plt_color * color)
{
	assert(series != NULL);
	assert(color != NULL);

	series->attr.line.color = *color;

	return 0;
}

int plt_series_line_color_rgba_set(struct plt_series * series, 
								   double r, double g, 
								   unsigned int b, double a)
{
	assert(series != NULL);

	series->attr.line.color.r = r;
	series->attr.line.color.g = g;
	series->attr.line.color.b = b;
	series->attr.line.color.a = a;

	return 0;
}

int plt_series_line_width_set(struct plt_series * series, double w)
{
	assert(series != NULL);

	series->attr.line.width = w;

	return 0;
}


int plt_line_series_insert(struct plt_series * series, const double x[], 
						   const double y[], size_t cnt)
{
	struct plt_line_info * info;
	struct plt_point * point;
	unsigned int pos;
	unsigned int i;
	unsigned int rem;
	double xmin;
	double ymin;
	double xmax;
	double ymax;

	assert(series != NULL);
	assert(x != NULL);
	assert(y != NULL);

	info = &series->info.line;
	point = (struct plt_point *)series->data;

	xmin = info->min.x;
	ymin = info->min.y;
	xmax = info->max.x;
	ymax = info->max.y;

	pos = info->nseg;
	rem = (info->nmax - pos);
	if (cnt > rem)
		cnt = rem;

	for (i = 0; i < cnt; ++i) {
		point[pos + i].x = x[i];
		point[pos + i].y = y[i];
		if (y[i] > ymax)
			ymax = y[i];
		if (y[i] < ymin)
			ymin = y[i];
		if (x[i] > xmax)
			xmax = x[i];
		if (x[i] < xmin)
			xmin = x[i];
	}

	info->min.x = xmin;
	info->min.y = ymin;
	info->max.x = xmax;
	info->max.y = ymax;

#if (DEBUG> 2)
	fprintf(stderr, "min=(%g,%g) max=(%g,%g)\n", xmin, ymin, xmax, ymax);
#endif

	info->nseg += cnt;
	return cnt;
}

int plt_line_series_clear(struct plt_series * series)
{
	struct plt_line_info * info;

	assert(series != NULL);

	info = &series->info.line;
	info->nseg = 0;

	return 0;
}

int plt_line_series_set(struct plt_series * series, const double x[], 
						const double y[], size_t cnt)
{
	plt_line_series_clear(series);
	return plt_line_series_insert(series, x, y, cnt);
}


int plt_line_series_draw(struct plt_figure * figure)
{

	return 0;	
}

