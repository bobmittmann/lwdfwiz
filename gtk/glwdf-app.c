/*
 * glwdf(1)  GTK Lattice Wave Digital Filters
 * 
 * This file is part of gLWDF.
 *
 * File:	glwdf-app.c
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

#include "plot.h"
#include "glwdf.h"


float grab_float_value(GtkSpinButton *button, gpointer user_data)
{
  return gtk_spin_button_get_value (button);
}

void on_gamma_change(GtkButton *b) 
{
    printf("You selected: %s\n", gtk_button_get_label (b));
}

GtkWidget * create_fixpt_spin_button(GtkGrid * grid, int row, double value)
{
	GtkAdjustment *adjustment;
	GtkWidget * button;
	GtkWidget * label;
	char text[32];

	sprintf(text, "%d:", row);
	label = gtk_label_new(text);

	adjustment = gtk_adjustment_new(value, -2, 2, 0.000001, 0.001, 0.0);
	/* creates the spinbutton, with six decimal places */
	button = gtk_spin_button_new (adjustment, 0.000001, 6);
	g_signal_connect(button, "value-changed", G_CALLBACK(on_gamma_change), NULL);

	gtk_grid_insert_row(grid, row);
	gtk_grid_attach(grid, label, 1, row, 1, 1);
	gtk_grid_attach(grid, button, 2, row, 1, 1);

	return button;
}


void update_grid_gamma(GtkGrid* grid, double gamma[], int n)
{
	int i;

	for (i = 0; i < n; ++i) {
		int row = i;
		double val = gamma[row];
		GtkWidget * button;

		button = create_fixpt_spin_button(grid, row, val);
		(void)button;
	}

}


void init_grid_gamma(GtkWidget * grid)
{
	double gamma[127];
	int n = 10;
	int i;

	for (i = 0; i < n; ++i) {
		 gamma[i] = 0.0;
	 }

	 update_grid_gamma(GTK_GRID(grid), gamma, n);
}

/* called when window is closed */
void on_window_main_destroy()
{
    gtk_main_quit();
}

/* Singleton application */
struct glwdf_app app;

int main(int argc, char *argv[])
{
    GtkBuilder * builder; 
    GtkWidget * window;
    GtkWidget * drawing_plot;
	GtkWidget * grid_gamma;
	PlotFigure * figure; 
    GError * error = NULL;

    gtk_init(&argc, &argv);

  	builder = gtk_builder_new();
	if (!gtk_builder_add_from_file(builder, "glade/glwdf-app-wnd.glade", &error)) {
		g_warning("%s", error->message);
		g_free(error);
	}


    window = GTK_WIDGET(gtk_builder_get_object(builder, "wnd_glwdf_app"));

	g_signal_connect(window, "destroy", G_CALLBACK(on_window_main_destroy), NULL);

    gtk_builder_connect_signals(builder, NULL);

	grid_gamma = GTK_WIDGET(gtk_builder_get_object(builder, "grid_gamma"));
	init_grid_gamma(grid_gamma);
	app.grid_gamma = grid_gamma;

  	drawing_plot = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_plot"));
	figure = glwdf_freq_response_plot_init(drawing_plot);
	app.figure = figure;

    g_object_unref(builder);

    gtk_widget_show_all(window);                
    gtk_main();

    return 0;
}

