/*
 * glwdf-filterctl.c
 * This file is part of glwdf
 *
 * Copyright (C) 2005 - Bob Mittmann
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


#include "config.h"

#include "glwdf-filter_control.h"

#include <string.h>
#include <glib/gi18n.h>

struct _GlwdfFilterControl
{
	GtkBox parent;

	GSettings *settings;
	GtkWidget *font;
	GtkWidget *transition;
};

G_DEFINE_TYPE (GlwdfFilterControl, glwdf_filter_control, GTK_TYPE_BOX)

	static void
glwdf_filter_control_init (GlwdfFilterControl *filterctl)
{
	gtk_widget_init_template (GTK_WIDGET (filterctl));
#if 0
	filterctl->settings = g_settings_new ("org.gtk.Demo4");

	g_settings_bind (filterctl->settings, "font",
					 filterctl->font, "font",
					 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (filterctl->settings, "transition",
					 filterctl->transition, "active-id",
					 G_SETTINGS_BIND_DEFAULT);
#endif
}

	static void
glwdf_filter_control_dispose (GObject *object)
{
	GlwdfFilterControl *filterctl = GLWDF_FILTER_CONTROL (object);

	g_clear_object (&filterctl->settings);

	G_OBJECT_CLASS (glwdf_filter_control_parent_class)->dispose (object);
}

	static void
glwdf_filter_control_class_init (GlwdfFilterControlClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

	object_class->dispose = glwdf_filter_control_dispose;

	gtk_widget_class_set_template_from_resource(widget_class, "/glwdf_application/ui/glwdf-filter_control.ui");

	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), GlwdfFilterControl, font);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), GlwdfFilterControl, transition);
}

/**
 * glwdf_filter_control_new:
 *
 * Creates a new #GlwdfFilterControl.
 *
 * Return value: the new #GlwdfFilterControl object
 **/
	GtkWidget *
glwdf_filter_control_new (void)
{
	return GTK_WIDGET (g_object_new (GLWDF_TYPE_FILTER_CONTROL, NULL));
	//  return g_object_new (GLWDF_TYPE_FILTER_CONTROL, "transient-for", win, "use-header-bar", TRUE, NULL);
}

