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
#include <math.h>

#ifndef DEBUG
#define DEBUG 0
#endif

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

	if (title != NULL) {
		strncpy(figure->title.text , title, PLT_TITLE_TEXT_MAX); 
	} else {
		figure->title.text[0] = '\0';
	}

	figure->chart.scale.x = 1.0;
	figure->chart.scale.y = -1.0;

	figure->chart.rect.x = 0.0;
	figure->chart.rect.y = 0.0;
	figure->chart.rect.width = 0.0;
	figure->chart.rect.height = 0.0;

	figure->viewport.x = 0.0;
	figure->viewport.y = 0.0;
	figure->viewport.width = 0.0;
	figure->viewport.height = 0.0;

	figure->margin.left = 5.5;
	figure->margin.right = 5.5;
	figure->margin.top = 5.5;
	figure->margin.bottom = 5.5;

	figure->padding.left = 30.5;
	figure->padding.right = 10.5;
	figure->padding.top = 40.5;
	figure->padding.bottom = 30.5;

	figure->series.cnt = 0;
	legend = &figure->legend;
	font = &figure->title.font;

	/* Default font */
	strncpy(font->face, "Sans Serif", PLT_FONT_FACE_MAX); 
	font->slant = CAIRO_FONT_SLANT_NORMAL;
	font->weight = CAIRO_FONT_WEIGHT_BOLD;
	font->size = 14;

	figure->title.location.x = 0.0;
	figure->title.location.y = 0.0;
	figure->title.font = *font;


	legend->location.x = 0.0;
	legend->location.y = 0.0;
	font = & legend->font;
	font->slant = CAIRO_FONT_SLANT_NORMAL;
	font->weight = CAIRO_FONT_WEIGHT_NORMAL;
	font->size = 12;

	xaxis = &figure->axis.x;
	yaxis = &figure->axis.y;

	xaxis->font = *font;
	if (title != NULL) {
		strncpy(xaxis->label, xlabel, PLT_AXIS_LABEL_MAX); 
	} else {
		xaxis->label[0] = '\0';
	}

	xaxis->ticks.max = PLT_TICKS_MAX;
	xaxis->ticks.len = 0;
	xaxis->span = 1.0;
	xaxis->offs = 0.0;

	yaxis->font = *font;
	if (title != NULL) {
		strncpy(yaxis->label, ylabel, PLT_AXIS_LABEL_MAX); 
	} else {
		yaxis->label[0] = '\0';
	}

	yaxis->ticks.max = PLT_TICKS_MAX;
	yaxis->ticks.len = 0;
	yaxis->span = 1.0;
	yaxis->offs = 0.0;


// fprintf(stderr, "1.\n");
#if (DEBUG)
	fprintf(stderr, "figure=%p.\n", figure);
#endif

	figure->opt.xauto = true;
	figure->opt.yauto = true;
	figure->opt.border = true;
	figure->opt.background = false;
	figure->opt.grid = true;

	return figure;
}

/*
 * adjust the point to host's coordinate system
 */
void plt_point_map(struct plt_figure * figure, struct plt_point * pt)
{
//	pt->x = (pt->x - figure->offset.x) * figure->scale.x + figure->chart.rect.x;
//	pt->y = (pt->y - figure->offset.y) * figure->scale.y + figure->chart.rect.y;
}

static int _plt_chart_scale_fixup(struct plt_figure * figure)
{
	struct plt_rect rect = figure->chart.rect;
	double kx;
	double ox;
	double ky;
	double oy;


	/*
	 * adjust the chart scale 
	 */
	kx = rect.width / figure->axis.x.span;
	ky = -1.0 * (rect.height / figure->axis.y.span);

	figure->chart.scale.x = kx;
	figure->chart.scale.y = ky;


	/*
	 * adjust the chart offset (zero)
	 */

	ox = rect.x - kx * figure->axis.x.offs;
	oy = rect.y + rect.height - ky * figure->axis.y.offs;

	figure->chart.offs.x = ox;
	figure->chart.offs.y = oy;

#if (DEBUG)
	fprintf(stderr, "scale: x=%g y=%g\n", figure->chart.scale.x, figure->chart.scale.y);
#endif

	return 0;
}

/*
 * adjust the viewport using host's coordinate system
 */
int plt_figure_viewport_set(struct plt_figure * figure, struct plt_rect rect)
{
	assert(figure != NULL);

	rect.x += figure->margin.left;
	rect.y +=  figure->margin.bottom;
	rect.width -=  (figure->margin.left + figure->margin.right);
	rect.height -=  (figure->margin.top + figure->margin.bottom);

	figure->viewport = rect;

	rect.x += figure->padding.left;
	rect.y += figure->padding.top;

	rect.width -= (figure->padding.left + figure->padding.right);
	rect.height -= (figure->padding.bottom + figure->padding.top);

	figure->chart.rect = rect;

	return _plt_chart_scale_fixup(figure);
}


int plt_figure_xspan_set(struct plt_figure * figure, double x0, double x1)
{
	assert(figure != NULL);

	figure->axis.x.span = x1 - x0;
	figure->axis.x.offs = x0;

	return _plt_chart_scale_fixup(figure);
}

int plt_figure_yspan_set(struct plt_figure * figure, double y0, double y1)
{
	assert(figure != NULL);

	figure->axis.y.span = y1 - y0;
	figure->axis.y.offs = y0;

	return _plt_chart_scale_fixup(figure);
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

int plt_figure_xticks_set(struct plt_figure * figure, const double x[], 
						  char * const label[], unsigned int cnt)
{
	struct plt_ticks * ticks;
	unsigned int i;

	assert(figure != NULL);
	assert(x != NULL);
	assert(label != NULL);

	ticks = &figure->axis.x.ticks;

	if (cnt > ticks->max)
		cnt = ticks->max;

	for (i = 0; i < cnt; ++i) {
		ticks->pos[i] = x[i];
		if (label[i] != NULL)
			strncpy(ticks->label[i], label[i], PLT_TICKS_LABEL_MAX); 
		else
			ticks->label[i][0] = '\0';
	}

	ticks->len = cnt;

	return cnt;
}

int plt_figure_yticks_set(struct plt_figure * figure, const double y[], 
						  char * const label[], unsigned int cnt)
{
	struct plt_ticks * ticks;
	unsigned int i;

	assert(figure != NULL);
	assert(y != NULL);
	assert(label != NULL);

	ticks = &figure->axis.y.ticks;

	if (cnt > ticks->max)
		cnt = ticks->max;

	for (i = 0; i < cnt; ++i) {
		ticks->pos[i] = y[i];
		if (label[i] != NULL)
			strncpy(ticks->label[i], label[i], PLT_TICKS_LABEL_MAX); 
		else
			ticks->label[i][0] = '\0';

	}

	ticks->len = cnt;

	return cnt;
}

void plt_mk_lin_ticks(double pos[], char * label[], double from, double to, unsigned int cnt)
{
	double dv;
	double v;
	int i;

	assert(pos != NULL);
	assert(label != NULL);

	dv = (to - from) / (cnt - 1);

	for(i = 0, v = from; i < cnt; ++i, v += dv) {
		char * s;
		pos[i] = v;
		s = label[i];

		assert(s != NULL);
		sprintf(label[i], "%g", v); 
	}
}

int plt_mk_log10_ticks(double pos[], char * label[], double from, double to, unsigned int cnt)
{
//	double dv;
	double v;
	double v0;
	int m; 
	int n; 
	int n0; 
	int n1; 
	int i;

	assert(pos != NULL);
	assert(label != NULL);
	assert(from > 0.0);
	assert(to > from);

	n0 = floor(log10(from));
	n1 = ceil(log10(to));
	n = n1 - n0;


	fprintf(stderr, " -------------------------------------\n");

	fprintf(stderr, " - n0=%d n1=%d\n", n0, n1);
	v0 = pow(10.0, n0);
	v = powf(10.0, n1);
	fprintf(stderr, "v=%g v0=%g\n", v, v0);

	//fprintf(stderr, " - number of decades = %d\n", n);

	m = cnt / n;
	//fprintf(stderr, " - points per decade = %d\n", m);

	cnt = m * n;

	v0 = pow(n0, 10.0);

	fprintf(stderr, "v0=%g from=%g\n", v0, from);
//	v1 = log10(to);
	v = v0;
	for(i = 0; i < n; ++i) {
		int j;
//		dv = (to - from) / m;
		for(j = 0; j < m; ++j) {
			char * s;

			pos[i + j * m] = log10(v);
			s = label[i + j * m];

			assert(s != NULL);
			sprintf(label[i], "%g", v); 
		}
		v += 10.0;
	}

	return cnt;
}


