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

#include <gtk/gtk.h>


#ifdef __cplusplus
extern "C" {
#endif

GtkWidget* do_assistant (GtkWidget *do_widget);

void glwdf_about_dialog_show(GtkWidget *window);

#ifdef  __cplusplus
}
#endif

#endif	/* __GLWDF_H__ */

