/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	plot-i.h
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


#ifndef __PLOT_I_H__
#define __PLOT_I_H__

#include "plot.h"

#ifndef __PLOT_CORE__
#error "Never use <plot_i.h> directly; include <plot.h> instead."
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct plt_color {
	double r;
	double g;
	double b;
	double a;
};

struct plt_point {
	double x;
	double y;
};

struct plt_rect {
	double x;
	double y;
	double width;
	double height;
};

#define PLT_FONT_FACE_MAX 127

struct plt_font {
	char face[PLT_FONT_FACE_MAX + 1];
	uint8_t slant;
	uint8_t weight;
	float size;
};

#define PLT_TITLE_TEXT_MAX 127

struct plt_title {
  	char text[PLT_TITLE_TEXT_MAX + 1];
	struct plt_point location;
	struct plt_font font;
	struct plt_color color;
};

#define PLT_AXIS_LABEL_MAX 127

struct plt_axis {
  	char label[PLT_AXIS_LABEL_MAX + 1];
	struct plt_point location;
	struct plt_font font;
	struct plt_color color;
};

/* line segments metadata */
struct plt_line_info {
	uint32_t nseg; /* number of segments */
	uint32_t nmax; /* maximum number of allocated segments */
};

union plt_data_info {
	union {
		struct plt_line_info line;
	};
};

struct plt_figure;

struct plt_series_op {
	int (*draw)(struct plt_series * series, const void * data, const void * meta);
};

#define PLT_SERIES_LABEL_MAX 127
struct plt_series {
	struct plt_figure * figure;
  	char label[PLT_SERIES_LABEL_MAX + 1];
	struct {
		struct line {
			struct plt_color color;
			double width;
		} line;
	} attr;
	enum plt_series_type type;
	struct plt_series_op op;
	union plt_data_info info;
	void * data;
};

#define PLT_SERIES_MAX 64

struct plt_series_lst {
	uint32_t cnt;
	struct plt_series * entry[PLT_SERIES_MAX];
};

#define PLT_LEGEND_LABEL_MAX 127
struct plt_legend {
	struct plt_figure * figure;
	struct plt_point location;
	struct plt_font font;
	struct plt_color color;
};

struct plt_figure {
  	char label[PLT_AXIS_LABEL_MAX + 1];
	struct plt_title title;
	struct plt_point pos;
	struct plt_legend legend;
	struct {
		struct plt_axis x;
		struct plt_axis y;
	} axis;
	struct plt_series_lst series;
	/* FIXME mutual exclusion */
	int mutex;
	/* FIXME temporary */
	struct {
		struct plt_rect rect;
		struct plt_rect disp;
		struct plt_rect axis;
	} chart;
};

#ifdef __cplusplus
}
#endif

#endif /* __PLOT_I_H__ */

