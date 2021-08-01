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
#include <gtk/gtk.h>

typedef struct {
    GtkWidget *w_txtvw_main;            // Pointer to text view object
    GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
    GtkTextBuffer *textbuffer_main;     // Pointer to text buffer
    GtkWidget *w_freq_resp;     
    GtkWidget *w_time_plot;     
} app_widgets;




/* Singleton application */
struct glwdf_app app;

/* 
 * Filter example 
 *
 */
const double gamma_buttw_7[] = {
	+0.03128,
	-0.05307,
	+0.0625,
	-0.23284,
	+0.0625,
	-0.63655,
	+0.0625
};

const double gamma_buttw_11[] = {
	+0.061270655,
	-0.024424396,
	+0.122083000,
	-0.089941500,
	+0.122083000,
	-0.212148903,
	+0.122083000,
	-0.416122032,
	+0.122083000,
	-0.752463930,
	+0.122083000
};

const double gamma_buttw_13[] = {
	+0.029073955,
	-0.015588399,
	+0.058098800,
	-0.061593469,
	+0.058098800,
	-0.144658251,
	+0.058098800,
	-0.276238512,
	+0.058098800,
	-0.477098436,
	+0.058098800,
	-0.785183394,
	+0.058098800
};

const double gamma_chevy_13[] = {
	+0.808033373,
	-0.672777907,
	+0.897543917,
	-0.723572635,
	+0.705945503,
	-0.787822539,
	+0.493335904,
	-0.852634509,
	+0.318830299,
	-0.913384199,
	+0.202350068,
	-0.971110265,
	+0.145233719
};

const double gamma_chevy_17[] = {
	+0.658343804,
	-0.449935230,
	+0.873306611,
	-0.495775766,
	+0.754714991,
	-0.561951779,
	+0.601067772,
	-0.638677794,
	+0.448227121,
	-0.718752061,
	+0.317975384,
	-0.798595909,
	+0.219382482,
	-0.877783605,
	+0.154391522,
	-0.958222256,
	+0.122316824,
};

const double gamma_ellip_3[] = {
	+0.131607254,
	-0.517958541,
	+0.336639897
};

const double gamma_ellip_5[] = {
	+0.6338,
	-0.5372,
	+0.6605,
	-0.8260,
	+0.3755
};

const double gamma_ellip_9[] = {
	+0.236748440,
	-0.223099826,
	+0.340033608,
	-0.540013478,
	+0.200248543,
	-0.786454344,
	+0.129369806,
	-0.939166552,
	+0.103203749
};

const double gamma_ellip_11[] = {
	+0.321202733,
	-0.217182336,
	+0.476301722,
	-0.461759711,
	+0.305436126,
	-0.689444514,
	+0.193133254,
	-0.847842336,
	+0.136398210,
	-0.954437193,
	+0.113602321
};

const double gamma_ellip_15[] = {
 +0.711568558,
 -0.559092392,
 +0.942869698,
 -0.675537504,
 +0.939101444,
 -0.790115387,
 +0.935885661,
 -0.874544369,
 +0.933795606,
 -0.929674149,
 +0.932581871,
 -0.964888727,
 +0.931935648,
 -0.989365749,
 +0.931658009
};

int main(int argc, char *argv[])
{
    GtkBuilder   *builder; 
    GtkWidget   * wnd_main;
	GlwdfFreqTool * ftool;
	struct lwdf_fp64 * filt;

 //   GtkWidget   * wnd_time;
//    GtkWidget   * wnd_freq;
    app_widgets     *widgets = g_slice_new(app_widgets);
    GError * error = NULL;

    gtk_init(&argc, &argv);

  	builder = gtk_builder_new();
	if (!gtk_builder_add_from_file(builder, "glade/glwdf-app.glade", 
								   &error)) {
		g_warning("%s", error->message);
		g_free(error);
		return 1;
	}

	/* FIXME: 
	 *
	 * stantiating a new filter just for testing ... */
	filt = lwdf_fp64_new(88200);
	app.filter =  filt;
	lwdf_fp64_gamma_set(filt, gamma_buttw_7, 
						sizeof(gamma_buttw_7) / sizeof(double));
	lwdf_fp64_gamma_set(filt, gamma_ellip_15, 
						sizeof(gamma_ellip_15) / sizeof(double));

    // Get pointers to widgets
    wnd_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    widgets->w_txtvw_main = wnd_main;
    widgets->w_dlg_file_choose = 
		GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->textbuffer_main = 
		GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer_main"));
	widgets->w_time_plot = glwdf_time_plot_init(builder, filt);

	ftool = glwdf_freq_tool_new(builder, wnd_main, filt);
	widgets->w_freq_resp = ftool->window;

    gtk_builder_connect_signals(builder, widgets);
    g_object_unref(builder);

	glwdf_freq_tool_update(ftool);

    gtk_widget_show(wnd_main);
    gtk_widget_show(widgets->w_freq_resp);
    gtk_widget_show(widgets->w_time_plot);

    gtk_main();

    g_slice_free(app_widgets, widgets);

    return 0;
}

// File --> Open
void on_menuitem_file_open_activate(GtkMenuItem *menuitem, 
									app_widgets *app_wdgts)
{
    gchar *file_name = NULL;        // Name of file to open from dialog box
    gchar *file_contents = NULL;    // For reading contents of file
    gboolean file_success = FALSE;  // File read status
    
    // Show the "Open Text File" dialog box
    gtk_widget_show(app_wdgts->w_dlg_file_choose);
    
    // Check return value from Open Text File dialog box to see if user clicked the Open button
    if (gtk_dialog_run(GTK_DIALOG (app_wdgts->w_dlg_file_choose)) == 
		GTK_RESPONSE_OK) {
        // Get the file name from the dialog box
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_wdgts->w_dlg_file_choose));
        if (file_name != NULL) {
            // Copy the contents of the file to dynamically allocated memory
            file_success = g_file_get_contents(file_name, &file_contents, 
											   NULL, NULL);
            if (file_success) {
                // Put the contents of the file into the GtkTextBuffer
                gtk_text_buffer_set_text(app_wdgts->textbuffer_main, 
										 file_contents, -1);
            }
            g_free(file_contents);
        }
        g_free(file_name);
    }

    // Finished with the "Open Text File" dialog box, so hide it
    gtk_widget_hide(app_wdgts->w_dlg_file_choose);
}

// File --> Save as
void on_menuitem_file_save_as_activate(GtkMenuItem *menuitem, 
									   app_widgets *app_wdgts)
{
}

// File --> Save
void on_menuitem_file_save_activate(GtkMenuItem *menuitem, 
									app_widgets *app_wdgts)
{
}

// File --> New
void on_menuitem_file_new_activate(GtkMenuItem *menuitem, 
								   app_widgets *app_wdgts)
{
    // Clear the text from window "Close the file"
    gtk_text_buffer_set_text(app_wdgts->textbuffer_main, "", -1);
}


// File --> Close
void on_menuitem_file_close_activate(GtkMenuItem *menuitem, 
									 app_widgets *app_wdgts)
{
    // Clear the text from window "Close the file"
    gtk_text_buffer_set_text(app_wdgts->textbuffer_main, "", -1);
}

// File --> Quit
void on_menuitem_file_quit_activate(GtkMenuItem *menuitem, 
									app_widgets *app_wdgts)
{
    gtk_main_quit();
}

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

