/*
 * glwdf-filter_control.h
 * This file is part of glwdf
 *
 * Copyright (C) 2005 - Paolo Borelli
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLWDF_FILTER_CONTROL_H
#define GLWDF_FILTER_CONTROL_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLWDF_TYPE_FILTER_CONTROL (glwdf_filter_control_get_type ())

G_DECLARE_FINAL_TYPE(GlwdfFilterControl, glwdf_filter_control, GLWDF, FILTER_CONTROL, GtkBox)

GtkWidget *glwdf_filter_control_new(void);

G_END_DECLS

#endif

