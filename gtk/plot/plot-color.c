/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	plot-color.c
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

#define COLOR_STOCK_CNT 10

const struct plt_color color_stock[COLOR_STOCK_CNT] = {
	{ .r = 0.0588, .g = 0.4667, .b = 0.7059, .a = 1.0 },
	{ .r = 1.0000, .g = 0.4980, .b = 0.0549, .a = 1.0 },
	{ .r = 0.1725, .g = 0.6275, .b = 0.1725, .a = 1.0 },
	{ .r = 0.8392, .g = 0.1529, .b = 0.1569, .a = 1.0 },
	{ .r = 0.5804, .g = 0.4039, .b = 0.7412, .a = 1.0 },

	{ .r = 0.5490, .g = 0.3373, .b = 0.2941, .a = 1.0 },
	{ .r = 0.8902, .g = 0.4667, .b = 0.7608, .a = 1.0 },
	{ .r = 0.4980, .g = 0.4980, .b = 0.4980, .a = 1.0 },
	{ .r = 0.7373, .g = 0.7412, .b = 0.1333, .a = 1.0 },
	{ .r = 0.0902, .g = 0.7451, .b = 0.8118, .a = 1.0 },
};

unsigned int color_stock_idx = 0;

const struct plt_color * plt_stock_color_next(void)
{
	const struct plt_color  * color;
	unsigned int idx;

	idx = color_stock_idx + 1;
	if (idx == COLOR_STOCK_CNT)
		idx = 0;

	color =  &color_stock[idx];

	color_stock_idx = idx;

	return color;
}

const struct plt_color * plt_stock_color_get(void)
{
	const struct plt_color  * color;
	unsigned int idx;

	idx = color_stock_idx;
	color =  &color_stock[idx];

	return color;
}

struct plt_color * plt_color_new(double r, double g, 
								 unsigned int b, double a)
{
	struct plt_color  * color;

	color = g_new(struct plt_color, 1);

	if (color == NULL) {
		return NULL;
	}

	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;

	return 0;
}
 
