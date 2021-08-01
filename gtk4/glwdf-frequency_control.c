/*
 * glwdf-ctrl.c
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

#include "glwdf-frequency_control.h"

#include <string.h>
#include <glib/gi18n.h>

struct _GlwdfFrequencyControl
{
    GtkBox parent;

    GSettings *settings;
};

G_DEFINE_TYPE (GlwdfFrequencyControl, glwdf_frequency_control, GTK_TYPE_BOX)

    static void
glwdf_frequency_control_init (GlwdfFrequencyControl *ctrl)
{
    gtk_widget_init_template (GTK_WIDGET (ctrl));
}

    static void
glwdf_frequency_control_dispose (GObject *object)
{
    GlwdfFrequencyControl *ctrl = GLWDF_FREQUENCY_CONTROL (object);

    g_clear_object (&ctrl->settings);

    G_OBJECT_CLASS (glwdf_frequency_control_parent_class)->dispose (object);
}

    static void
glwdf_frequency_control_class_init (GlwdfFrequencyControlClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

	object_class->dispose = glwdf_frequency_control_dispose;


	gtk_widget_class_set_template_from_resource(widget_class, "/glwdf_application/ui/glwdf-frequency_control.ui");
}

/**
 * glwdf_frequency_control_new:
 *
 * Creates a new #GlwdfFrequencyControl.
 *
 * Return value: the new #GlwdfFrequencyControl object
 **/
    GtkWidget *
glwdf_frequency_control_new (void)
{
    return GTK_WIDGET (g_object_new (GLWDF_TYPE_FREQUENCY_CONTROL, NULL));
}

