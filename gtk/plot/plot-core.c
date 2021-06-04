/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	plot-core.c
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

struct plt_figure * plt_figure_new(const char * title, const char * xlabel, const char * ylabel)
{
	struct plt_figure * figure;
	struct plt_legend * legend;
	struct plt_axis * xaxis;
	struct plt_axis * yaxis;
	struct plt_font * font;

	figure = g_new(struct plt_figure, 1);

	if (figure == NULL) {
		return NULL;
	}

	fprintf(stderr, "1.\n");

	if (title != NULL) {
		strncpy(figure->title.text , title, PLT_TITLE_TEXT_MAX); 
	} else {
		figure->title.text[0] = '\0';
	}

	figure->pos.x = 0.0;
	figure->pos.y = 0.0;
	figure->series.cnt = 0;
	legend = &figure->legend;
	font = &figure->title.font;

	/* Default font */
	strncpy(font->face, "Sans Serif", PLT_FONT_FACE_MAX); 
	font->slant = CAIRO_FONT_SLANT_NORMAL;
	font->weight = CAIRO_FONT_WEIGHT_NORMAL;
	font->size = 12;

	legend->location.x = 0.0;
	legend->location.y = 0.0;
	legend->font = *font;

	xaxis = &figure->axis.x;
	yaxis = &figure->axis.y;

	xaxis->font = *font;
	if (title != NULL) {
		strncpy(xaxis->label, xlabel, PLT_AXIS_LABEL_MAX); 
	} else {
		xaxis->label[0] = '\0';
	}

	yaxis->font = *font;
	if (title != NULL) {
		strncpy(yaxis->label, ylabel, PLT_AXIS_LABEL_MAX); 
	} else {
		yaxis->label[0] = '\0';
	}

	fprintf(stderr, "figure=%p.\n", figure);

	return figure;
}


struct plt_series * plt_line_series_new(struct plt_figure * figure, const char * label, 
											 unsigned int max)
{
	struct plt_series * series;
	struct plt_point * points;
	unsigned int pos;

	assert(figure != NULL);

	if ((pos = figure->series.cnt) == PLT_SERIES_MAX)
		return NULL;


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

int plt_series_line_color_set(struct plt_series * series, double r, double g, 
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
						   const double y[], unsigned int cnt)
{
	struct plt_line_info * info;
	struct plt_point * point;
	unsigned int pos;
	unsigned int i;
	unsigned int rem;

	assert(series != NULL);
	assert(x != NULL);
	assert(y != NULL);

	info = &series->info.line;
	point = (struct plt_point *)series->data;

	pos = info->nseg;
	rem = (info->nmax - pos);
	if (cnt > rem)
		cnt = rem;


	for (i = 0; i < cnt; ++i) {
		point[pos + i].x = x[i];
		point[pos + i].y = y[i];
	}

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
						const double y[], unsigned int cnt)
{
	plt_line_series_clear(series);
	return plt_line_series_insert(series, x, y, cnt);
}


int plt_line_series_draw(struct plt_figure * figure)
{

	return 0;	
}

