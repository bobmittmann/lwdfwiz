/*
 * lwdfwiz(1)  Lattice Wave Digital Filters Wizard
 * 
 * This file is part of LWDFWiz.
 *
 * File:	glwdf.h
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

/*
 * Lattice Wave Digital Filters
 *
 * Based on: 
 *  Explicit Formulas for Lattice Wave Digital Filters, Lajos Gazsi, 1985 IEEE
 *
 */

#ifndef __GLWDF_H__
#define __GLWDF_H__

#include "lwdf.h"
#include "plot.h"
#include <gtk/gtk.h>

struct glwdf_app {
	PlotFigure * figure; 
	GtkWidget * grid_gamma;
	struct lwdf_fp64 * filter;
};

struct glwdf_freq_tool {
	GtkWidget * window;
	GtkWidget * grid_gamma;

	struct lwdf_fp64 * flt;
	struct lwdf_fp64_freq * ffr;

	struct {
		PlotFigure * figure; 
		PlotSeries * series;
		GtkWidget * drawing;
		bool show;
	} ampl;

	struct {
		PlotFigure * figure; 
		PlotSeries * series;
		GtkWidget * drawing;
		bool show;
	} phase;
};


typedef struct glwdf_freq_tool GlwdfFreqTool;

#ifdef __cplusplus
extern "C" {
#endif

GtkWidget * glwdf_time_plot_init(GtkBuilder * builder, struct lwdf_fp64 * filt);

GlwdfFreqTool * glwdf_freq_tool_new(GtkBuilder * builder, GtkWidget * parent,
									struct lwdf_fp64 * flt);

void glwdf_freq_tool_update(GlwdfFreqTool * ftool);

void glwdf_freq_tool_free(GlwdfFreqTool * ftool);

#ifdef  __cplusplus
}
#endif

#endif	/* __GLWDF_H__ */

