/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	plot.h
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

#ifndef __PLOT_H__
#define __PLOT_H__

#include <sys/types.h>
#include <stdint.h>

#include <cairo/cairo.h>
#include <gtk/gtk.h>

typedef struct plt_color PlotColor;
typedef struct plt_rect PlotRect;
typedef struct plt_font PlotFont;
typedef struct plt_title PlotTitle;
typedef struct plt_axis PlotAxis;

enum plt_series_type {
	PLT_SERIES_LINE = 1,
	PLT_SERIES_VLINE = 2,
	PLT_SERIES_HLINE = 3
};

typedef struct plt_series PlotSeries;

typedef struct plt_legend PlotLegend;

typedef struct plt_figure PlotFigure;

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------- 
 * Figures
 * -------------------------------------------------------------------------- */

PlotFigure * plt_figure_new(const char * title, const char * xlabel, const char * ylabel);

int plt_figure_xticks_set(PlotFigure * fig, const double x[], char * const lbl[], size_t cnt);

int plt_figure_yticks_set(PlotFigure * fig, const double y[], char * const lbl[], size_t cnt);

int plt_figure_xspan_set(PlotFigure * figure, double x0, double x1);

int plt_figure_yspan_set(PlotFigure * figure, double y0, double y1);

int plt_figure_xspan_auto(struct plt_figure * figure);

int plt_figure_yspan_auto(struct plt_figure * figure);

int plt_offs_set(PlotFigure * figure, double x, double y);

/* -------------------------------------------------------------------------- 
 * Axis 
 * -------------------------------------------------------------------------- */

void plt_mk_lin_ticks(double pos[], char ** label, double from, 
					  double to, size_t cnt);

int plt_mk_log10_ticks(double pos[], char * label[], double from, 
					   double to, size_t cnt);

/* -------------------------------------------------------------------------- 
 * Series
 * -------------------------------------------------------------------------- */

PlotSeries * plt_line_series_new(PlotFigure * fig, const char * label, 
								 size_t max);

int plt_line_series_insert(PlotSeries * lin, const double x[], 
						   const double y[], size_t cnt);

int plt_line_series_set(PlotSeries * series, const double x[], 
						const double y[], size_t cnt);


int plt_line_series_clear(PlotSeries * fig);

int plt_series_line_color_rgba_set(PlotSeries * series, double r, double g, 
							  unsigned int b, double a);

int plt_series_line_color_set(PlotSeries * series, const PlotColor * c);

int plt_series_line_width_set(PlotSeries * series, double w);

/* -------------------------------------------------------------------------- 
 * GTK related functions 
 * -------------------------------------------------------------------------- */

void init_drawing_plot(GtkWidget * drawing);

void plt_figure_widget_connect(PlotFigure * figure, GtkWidget * darea);

/* -------------------------------------------------------------------------- 
 * Specialized figures 
 * -------------------------------------------------------------------------- */

/* Frequency response */
struct plt_figure * plt_figure_freq_resp_new(const char * title);

const struct plt_color * plt_stock_color_get(void);

const struct plt_color * plt_stock_color_next(void);

#ifdef __cplusplus
}
#endif

#endif /* __PLOT_H__ */

