/*
 * glwdf-filter_view.c
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

#include "glwdf-filter_view.h"

#include <string.h>
#include <glib/gi18n.h>

struct _GlwdfFilterView
{
    GtkBox parent;

    GSettings *settings;
};

G_DEFINE_TYPE (GlwdfFilterView, glwdf_filter_view, GTK_TYPE_BOX)

    static void
glwdf_filter_view_init (GlwdfFilterView *filter_view)
{
    gtk_widget_init_template (GTK_WIDGET (filter_view));
}

    static void
glwdf_filter_view_dispose (GObject *object)
{
    GlwdfFilterView *filter_view = GLWDF_FILTER_VIEW (object);

    g_clear_object (&filter_view->settings);

    G_OBJECT_CLASS (glwdf_filter_view_parent_class)->dispose (object);
}

    static void
glwdf_filter_view_class_init (GlwdfFilterViewClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

    object_class->dispose = glwdf_filter_view_dispose;

	gtk_widget_class_set_template_from_resource(widget_class, "/glwdf_application/ui/glwdf-filter_view.ui");
}

/**
 * glwdf_filter_view_new:
 *
 * Creates a new #GlwdfFilterView.
 *
 * Return value: the new #GlwdfFilterView object
 **/
    GtkWidget *
glwdf_filter_view_new (void)
{
    return GTK_WIDGET (g_object_new (GLWDF_TYPE_FILTER_VIEW, NULL));
}

