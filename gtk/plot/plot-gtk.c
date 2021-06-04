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

static void draw_dashed_grid(cairo_t* cr, struct plt_rect axis, PlotFigure * figure)
{
	static const double dashed[] = {2.0};
	int i;

	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	cairo_set_line_width(cr, 1.0);
	cairo_set_dash(cr, dashed, 1, 0);

	for(i = 1; ; ++i) {
		if((axis.x+i*50) >= (axis.x+axis.width)) 
			break;
		cairo_move_to(cr, axis.x+i*50, axis.y);
		cairo_line_to(cr, axis.x+i*50, axis.y+axis.height);
		cairo_stroke(cr);
	}

	for(i = 1; ; ++i) {
		if((axis.y+i*40) >= (axis.y+axis.height)) 
			break;
		cairo_move_to(cr, axis.x, axis.y+i*40);
		cairo_line_to(cr, axis.x+axis.width, axis.y+i*40);
		cairo_stroke(cr);
	}
	cairo_restore(cr);
}

static void draw_ticks(cairo_t* cr, struct plt_rect axis, PlotFigure * figure)
{
	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.05, 0.05, 0.05);
	cairo_set_line_width(cr, 3.0);

	cairo_restore(cr);
}

static void draw_axis(cairo_t* cr, struct plt_rect axis, PlotFigure * figure)
{
	cairo_text_extents_t extents;
	double text_length;
	struct plt_axis * xaxis;
	struct plt_axis * yaxis;
	struct plt_font * font;

	xaxis = &figure->axis.x;
	yaxis = &figure->axis.y;

	cairo_save(cr);
	cairo_set_line_width(cr, 0.15);
	cairo_set_source_rgb(cr, 0.05, 0.05, 0.05);
	cairo_rectangle(cr, axis.x, axis.y, axis.width, axis.height);
	cairo_stroke(cr);

	font = &yaxis->font;
	cairo_select_font_face(cr, font->face, font->slant, font->weight);
	cairo_set_font_size(cr, font->size);

	cairo_text_extents(cr, yaxis->label, &extents);
	text_length=extents.width/2.0+extents.x_bearing;
	cairo_move_to(cr, axis.x-10.0, axis.y+text_length+(axis.height)/2.0);

	cairo_save(cr);

	font = &xaxis->font;
	cairo_select_font_face(cr, font->face, font->slant, font->weight);
	cairo_set_font_size(cr, font->size);

	cairo_rotate(cr, -1.57);
	cairo_show_text(cr, yaxis->label);
	cairo_restore(cr);

	cairo_text_extents(cr, xaxis->label, &extents);
	text_length=extents.width/2.0+extents.x_bearing;
	cairo_move_to(cr, axis.x-text_length+(axis.width)/2.0, axis.y+axis.height+20.0);
	cairo_show_text(cr, xaxis->label);

	cairo_restore(cr);
}

static void draw_background(cairo_t* cr, struct plt_rect rect)
{
	cairo_pattern_t* pat;

	cairo_save(cr);
	pat=cairo_pattern_create_linear(rect.width/2-5,5, rect.width/2-5, rect.height-5);
	cairo_pattern_add_color_stop_rgba(pat, 0.0, 0.0, 0.07, 0.09, 0.5);
	cairo_pattern_add_color_stop_rgba(pat, 1.0, 0.0, 0.0, 0.0, 0.9);
	cairo_set_source(cr, pat);
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_fill(cr);
	cairo_restore(cr);
}

static void draw_line(cairo_t* cr, struct plt_rect axis, struct plt_point * point, 
					   struct plt_line_info * info)
{
	struct plt_point p0;
	struct plt_point p1;
	unsigned int i;
	double y_offs;

	y_offs = axis.y + axis.height;

	p0 = point[0];
	p0.y = y_offs - p0.y; 
	for(i = 0; i < info->nseg; ++i) {
		p1 = p0;
		p0 = point[i];
		p0.y = y_offs - p0.y; 
		cairo_move_to(cr, p0.x, p0.y);
		cairo_line_to(cr, p1.x, p1.y);
	}
}

static void draw_series(cairo_t* cr, struct plt_rect axis, PlotFigure * figure)
{
	unsigned int nseries;
	unsigned int j;

	nseries = figure->series.cnt;

	cairo_save(cr);
	cairo_rectangle(cr, axis.x, axis.y, axis.width, axis.height);
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

		fprintf(stderr, "series=\"%s\" %d points.\n", series->label, info->nseg);

		draw_line(cr, axis, point, info);

		cairo_stroke(cr);
	}

	cairo_restore(cr);
}


static void draw_legend(cairo_t* cr, struct plt_rect axis, PlotFigure * figure)
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
	cairo_move_to(cr, axis.x+5, axis.y+10.0+text_height/2.0);
	cairo_line_to(cr, axis.x+10.0, axis.y+10.0+text_height/2.0);
	cairo_stroke(cr);
	cairo_move_to(cr, axis.x+15.0, axis.y+10.0);
	cairo_show_text(cr, (const char*)legend1);
	cairo_restore(cr);
}


gboolean on_draw_event(GtkWidget* widget, cairo_t* cr, PlotFigure * figure)
{
	struct plt_rect rect;
	struct plt_rect disp;
	struct plt_rect axis;

	fprintf(stderr, "on_draw(figure=%p).\n", figure);

	rect = figure->chart.rect;
	disp = figure->chart.disp;
	axis = figure->chart.axis;
	(void)disp;

	draw_background(cr, rect);

	draw_axis(cr, axis, figure);

	draw_ticks(cr, axis, figure);

	draw_dashed_grid(cr, axis, figure);

	draw_series(cr, axis, figure);

	draw_legend(cr, axis, figure);

	return FALSE;
}

void on_realize(GtkWidget* widget, PlotFigure * figure)
{
	fprintf(stderr, "on_realize(figure=%p).\n", figure);

}

void on_size_allocate(GtkWidget* widget, GtkAllocation *allocation, PlotFigure * figure)
{
	struct plt_rect rect;
	struct plt_rect disp;
	struct plt_rect axis;

	fprintf(stderr, "on_size_allocate(figure=%p).\n", figure);

	rect.x = 5.5;
	rect.y = 5.5;
	rect.width = allocation->width-10.5;
	rect.height = allocation->height-10.5;

	axis.x = 60.5;
	axis.y = 40.5;
	axis.width = rect.width-80.5;
	axis.height = rect.height-75.5;

	disp.x = allocation->x;
	disp.y = allocation->y;
	disp.width = allocation->width;
	disp.height = allocation->height;

	figure->chart.rect = rect;
	figure->chart.disp = disp;
	figure->chart.axis = axis;
}

void plt_figure_widget_connect(struct plt_figure * figure, GtkWidget * darea)
{
	g_signal_connect(darea, "draw", G_CALLBACK(on_draw_event), figure);
	g_signal_connect(darea, "realize", G_CALLBACK(on_realize), figure);
	g_signal_connect(darea, "size-allocate", G_CALLBACK(on_size_allocate), figure);
}

