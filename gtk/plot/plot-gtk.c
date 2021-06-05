/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	plot-gtk.c
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

#ifndef DEBUG
#define DEBUG 0
#endif

static void draw_grid(cairo_t* cr, struct plt_chart * chart, struct plt_figure * figure)
{
	struct plt_rect rect = chart->rect;
	static const double dashed[] = {2.0};
	double * vx;
	double * vy;
	double x0;
	double x1;
	double y0;
	double y1;
	double ky;
	double kx;
	double oy;
	double ox;
	int n;
	int i;

	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_set_line_width(cr, 1.0);
	cairo_set_dash(cr, dashed, 1, 0);

	y0 = rect.y + rect.height;
	y1 = rect.y;
	kx = chart->scale.x;
	ox = chart->offs.x;
	vx = figure->axis.x.ticks.pos;
	n = figure->axis.x.ticks.len;

	for(i = 1; i < n; ++i) {
		double x = vx[i] * kx + ox;
		cairo_move_to(cr, x, y0);
		cairo_line_to(cr, x, y1);
		cairo_stroke(cr);
	}

	x0 = rect.x;
	x1 = rect.x + rect.width;
	ky = chart->scale.y;
	oy = chart->offs.y;
	vy = figure->axis.y.ticks.pos;
	n = figure->axis.y.ticks.len;

	for(i = 1; i < n; ++i) {
		double y = vy[i] * ky + oy;
		cairo_move_to(cr, x0, y);
		cairo_line_to(cr, x1, y);
		cairo_stroke(cr);
	}

	cairo_restore(cr);
}

static void draw_ticks(cairo_t* cr, struct plt_chart * chart, struct plt_figure * figure)
{
	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.05, 0.05, 0.05);
	cairo_set_line_width(cr, 3.0);

	cairo_restore(cr);
}

static void draw_axis(cairo_t* cr, struct plt_chart * chart, struct plt_figure * figure)
{
	cairo_text_extents_t extents;
	struct plt_rect rect = chart->rect;
	double text_length;
	struct plt_axis * xaxis;
	struct plt_axis * yaxis;
	struct plt_font * font;

	xaxis = &figure->axis.x;
	yaxis = &figure->axis.y;

	cairo_save(cr);
	cairo_set_line_width(cr, 2);
	cairo_set_source_rgba(cr, 0.05, 0.05, 0.05, 1.0);
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_stroke(cr);

	font = &yaxis->font;
	cairo_select_font_face(cr, font->face, font->slant, font->weight);
	cairo_set_font_size(cr, font->size);

	cairo_text_extents(cr, yaxis->label, &extents);
	text_length=extents.width/2.0+extents.x_bearing;
	cairo_move_to(cr, rect.x-10.0, rect.y+text_length+(rect.height)/2.0);

	cairo_save(cr);

	font = &xaxis->font;
	cairo_select_font_face(cr, font->face, font->slant, font->weight);
	cairo_set_font_size(cr, font->size);

	cairo_rotate(cr, -1.57);
	cairo_show_text(cr, yaxis->label);
	cairo_restore(cr);

	cairo_text_extents(cr, xaxis->label, &extents);
	text_length=extents.width/2.0+extents.x_bearing;
	cairo_move_to(cr, rect.x-text_length+(rect.width)/2.0, rect.y+rect.height+20.0);
	cairo_show_text(cr, xaxis->label);

	cairo_restore(cr);
}

static void draw_background(cairo_t* cr, struct plt_rect rect, struct plt_figure * figure)
{
	cairo_pattern_t* pat;

	cairo_save(cr);
	pat=cairo_pattern_create_linear(rect.width/2-5,5, rect.width/2-5, rect.height-5);
	cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.95, 0.95, 0.95, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.85, 0.85, 0.85, 1.0);
	cairo_set_source(cr, pat);
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_fill(cr);
	cairo_restore(cr);
}

static void draw_border(cairo_t* cr, struct plt_rect rect, struct plt_figure * figure) 
{
	cairo_save(cr);

	cairo_set_line_width(cr, 2.5);
	cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_stroke(cr);

	cairo_restore(cr);
}

static void draw_title(cairo_t* cr, struct plt_rect rect, struct plt_figure * figure) 
{
	struct plt_font * font;
	cairo_text_extents_t extents;
	double text_length;

	cairo_save(cr);

	font = &figure->title.font;
	cairo_select_font_face(cr, font->face, font->slant, font->weight);
	cairo_set_font_size(cr, font->size);

	cairo_text_extents(cr, figure->title.text, &extents);
	text_length = extents.width / 2.0 + extents.x_bearing;

	cairo_move_to(cr, rect.x - text_length+(rect.width)/2.0, rect.y + 20.0);

	cairo_show_text(cr, figure->title.text);


	cairo_restore(cr);
}


static void draw_line(cairo_t* cr, struct plt_chart * chart, struct plt_point * point, 
					   struct plt_line_info * info)
{
	struct plt_point p0;
	struct plt_point p1;
	double ky;
	double kx;
	double oy;
	double ox;
	int n;
	int i;

	kx = chart->scale.x;
	ox = chart->offs.x;

	ky = chart->scale.y;
	oy = chart->offs.y;

	n = info->nseg;

	p0 = point[0];
	p0.x = p0.x * kx + ox;
	p0.y = p0.y * ky + oy;
	for(i = 0; i < n; ++i) {
		p1 = p0;
		p0 = point[i];
		p0.x = p0.x * kx + ox;
		p0.y = p0.y * ky + oy;
		cairo_move_to(cr, p0.x, p0.y);
		cairo_line_to(cr, p1.x, p1.y);
	}

	cairo_stroke(cr);
}

static void draw_series(cairo_t* cr, struct plt_chart * chart, struct plt_figure * figure)
{
	/* get the bounding rectangle (box) */
	struct plt_rect rect = chart->rect;
	unsigned int nseries;
	unsigned int j;

	nseries = figure->series.cnt;

	cairo_save(cr);
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_clip(cr);

	for (j = 0; j < nseries; ++j) {
		struct plt_series * series;
		struct plt_line_info * info;
		struct plt_point * point;
		struct plt_color color;

		series = figure->series.entry[j];

		info = &series->info.line;
		point = (struct plt_point *)series->data;
		cairo_new_path(cr);
		color = series->attr.line.color;
		cairo_set_source_rgba(cr, color.r, color.g, color.b, color.a);
		cairo_set_line_width(cr, series->attr.line.width);

#if (DEBUG)
		fprintf(stderr, "series=\"%s\" %d points.\n", series->label, info->nseg);
#endif

		draw_line(cr, &figure->chart, point, info);

		cairo_stroke(cr);
	}

	cairo_restore(cr);
}


static void draw_legend(cairo_t* cr, struct plt_rect rect, PlotFigure * figure)
{
	cairo_text_extents_t extents;
	double text_length, text_height;
	static const char* legend1="V:Red";

	cairo_save(cr);
	cairo_set_line_width(cr, 2.0);
	cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
	cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 10.0);
	cairo_text_extents(cr, (const char*)legend1, &extents);
	text_length=extents.width/2.0+extents.x_bearing;
	(void)text_length;

	text_height=extents.height/2.0+extents.y_bearing;
	cairo_move_to(cr, rect.x+5, rect.y+10.0+text_height/2.0);
	cairo_line_to(cr, rect.x+10.0, rect.y+10.0+text_height/2.0);
	cairo_stroke(cr);
	cairo_move_to(cr, rect.x+15.0, rect.y+10.0);
	cairo_show_text(cr, (const char*)legend1);
	cairo_restore(cr);
}


gboolean on_draw_event(GtkWidget* widget, cairo_t* cr, PlotFigure * figure)
{
	struct plt_rect viewport;
	struct plt_chart * chart;

#if (DEBUG)
	fprintf(stderr, "on_draw(figure=%p).\n", figure);
#endif

	viewport = figure->viewport;
	chart = &figure->chart;

	if (figure->opt.background) {
		draw_background(cr, viewport, figure);
	}

	if (figure->opt.border) {
		draw_border(cr, viewport, figure);
	}

	draw_title(cr, viewport, figure);

	draw_axis(cr, chart, figure);

	draw_ticks(cr, chart, figure);

	if (figure->opt.grid) {
		draw_grid(cr, chart, figure);
	}

	draw_series(cr, chart, figure);

	draw_legend(cr, viewport, figure);

	return FALSE;
}

void on_realize(GtkWidget* widget, PlotFigure * figure)
{
	fprintf(stderr, "on_realize(figure=%p).\n", figure);

}

void on_size_allocate(GtkWidget* widget, GtkAllocation *allocation, PlotFigure * figure)
{
	struct plt_rect disp;

#if (DEBUG)
	fprintf(stderr, "on_size_allocate(figure=%p).\n", figure);

	fprintf(stderr, "   (%d, %d) (%d, %d).\n", allocation->x, allocation->y,
			allocation->width, allocation->height);
#endif

	disp.x = allocation->x;
	disp.y = allocation->y;
	disp.width = allocation->width;
	disp.height = allocation->height;

	plt_figure_viewport_set(figure, disp);
}

int plt_figure_span_set(struct plt_figure * figure, struct plt_point span);

void plt_figure_widget_connect(struct plt_figure * figure, GtkWidget * darea)
{
	g_signal_connect(darea, "draw", G_CALLBACK(on_draw_event), figure);
	g_signal_connect(darea, "realize", G_CALLBACK(on_realize), figure);
	g_signal_connect(darea, "size-allocate", G_CALLBACK(on_size_allocate), figure);
}

