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

_Pragma ("GCC optimize (\"Ofast\")")

#include "glwdf.h"
#include <complex.h>
#include <assert.h>
#include <pthread.h>
#include <pcm.h>


extern PlotFigure * dbg_figure; 

void plt_dbg(double y[], size_t npts)
{
	const struct plt_color * color;
	struct plt_series * series;
	PlotFigure * figure = dbg_figure; 
	double dx = 1.0;
	double * x;
	unsigned int i;

	assert(npts > 1);

	series = plt_line_series_new(figure, "Tone", npts);
	if (series == NULL) {
		return;
	}
	/* Add some traces (series) */
	x = calloc(npts, sizeof(double));
	for (i = 0; i < npts; ++i) {
		x[i] = dx * i;
	}

	color = plt_stock_color_get();
	plt_series_line_color_set(series, color);
	plt_series_line_width_set(series, 0.5);
	plt_line_series_set(series, x, y, npts);
	
	plt_figure_xspan_auto(figure);
	plt_figure_yspan_auto(figure);

	plt_stock_color_next();
	free(x);
}

void freq_plt_update(struct lwdf_fp64_freq * ffr,
					struct lwdf_fp64 * flt, 
					struct plt_series * series)
{
	unsigned int n;
	unsigned int i;
	double * db;
	double * ph;
	double * w;
	double * f;
	complex double * z;
	double samplerate;

	samplerate = lwdf_fp64_samplerate_get(flt);

	n = lwdf_fp64_lowwpass_freq_resp(ffr, flt, &w, &z);

	db = calloc(n, sizeof(double));
	ph = calloc(n, sizeof(double));
	f = calloc(n, sizeof(double));

#if (DEBUG > 0)
	fprintf(stderr, "n=%d\n", n);
#endif

	for (i = 0; i < n; ++i) {
		db[i] = 20*log10(cabs(z[i]));
		ph[i] = carg(z[i]) / (2 * M_PI);
	
		/* convert frequencies to Hz */
		f[i] = w[i] * samplerate; 
		
#if (DEBUG > 1)
		fprintf(stderr, "f[%-2d] = %g[Hz] -> %g[dB]\n", i, f[i], db[i]);
#endif
	}

	plt_line_series_set(series, f, db, n);

	free(db);
	free(ph);
	free(f);
}

void glwdf_freq_tool_update(GlwdfFreqTool * ftool)
{
	assert(ftool != NULL);

	freq_plt_update(ftool->ffr, ftool->flt, ftool->ampl.series);
}

void on_gamma_change(GtkSpinButton *btn, GlwdfFreqTool * ftool) 
{
	int idx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "idx"));
	struct lwdf_fp64 * flt;
	double val;

	assert(ftool != NULL);

	flt = ftool->flt; 
	idx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(btn), "idx"));
    val = gtk_spin_button_get_value(btn);

	lwdf_fp64_coeff_set(flt, idx, val);

	freq_plt_update(ftool->ffr, ftool->flt, ftool->ampl.series);

    printf("Gamma[%d]=%g\n", idx, val);

	gtk_widget_queue_draw(ftool->ampl.drawing);
}

static GtkWidget * create_fixpt_spin_button(GtkGrid * grid, int row, 
											double value)
{
	GtkAdjustment *adjustment;
	GtkWidget * button;
	GtkWidget * label;
	char text[32];

	sprintf(text, "%d:", row);
	label = gtk_label_new(text);

	adjustment = gtk_adjustment_new(value, -1, 1, 0.0001, 0.01, 0.0);
	/* creates the spinbutton, with six decimal places */
	button = gtk_spin_button_new(adjustment, 0.01, 5);
	
	gtk_grid_insert_row(grid, row);
	gtk_grid_attach(grid, label, 1, row, 1, 1);
	gtk_grid_attach(grid, button, 2, row, 1, 1);

	return button;
}

static void update_grid_gamma(GlwdfFreqTool * ftool, GtkGrid* grid, 
							  double gamma[], int n)
{
	int i;

	for (i = 0; i < n; ++i) {
		int row = i;
		double val = gamma[row];
		GtkWidget * button;

		button = create_fixpt_spin_button(grid, row, val);
		(void)button;

		g_object_set_data(G_OBJECT(button), "idx", GINT_TO_POINTER(i));

		g_signal_connect(button, "value-changed", 
						 G_CALLBACK(on_gamma_change), ftool);
	}

    gtk_widget_show_all(GTK_WIDGET(grid));

}

static PlotFigure * freq_response_plot_init(GtkWidget * drawing, 
											double samplerate)
{
	struct plt_figure * figure;
	char title[128];
	double pos[128];
	char * lbl[128];
	char buf[128 * 16];
	int n;
	int i;

	sprintf(title, "LWDF Response: %.1f", samplerate);
	figure = plt_figure_new(title, "Frequency [Hz]", "Magnitude [dB]");

	/* set the string pointers */
	n = 23;
	for(i = 0; i < n; ++i) {
		double w;
		double f;

		lbl[i] = &buf[i * 16];

		w = (i * 1.0) / (n - 1);
		f = w * samplerate / 2;
		pos[i] = f;
		if (f < 1000) 
			sprintf(lbl[i], "%3.0f", f); 
		else if (f < 1000000) 
			sprintf(lbl[i], "%3.0fK", f / 1000); 
	}

	plt_figure_xspan_set(figure, 0, samplerate / 2);
	plt_figure_xticks_set(figure, pos, lbl, n);

	/* set the string pointers */
	n = 21;
	for(i = 0; i < n; ++i)
		lbl[i] = &buf[i * 16];

	plt_figure_yspan_set(figure, -114.0, 6.0);
	plt_mk_lin_ticks(pos, lbl, -114.0, 6.0, n);
	plt_figure_yticks_set(figure, pos, lbl, n);
	
	plt_figure_widget_connect(figure, drawing);

	return figure;
}

GlwdfFreqTool * glwdf_freq_tool_new(GtkBuilder * builder, GtkWidget * parent,
									struct lwdf_fp64 * flt)
{
	GlwdfFreqTool * ftool;
	GtkWidget * window;
	GtkWidget * drawing_plot;
	GtkWidget * grid_gamma;
	PlotFigure * figure; 
	PlotSeries * series;
	struct lwdf_fp64_freq * ffr;
	const struct plt_color * color;
	unsigned int npts = 512;
	double samplerate;
	double gamma[128];
	size_t cnt;

	ftool = g_new(GlwdfFreqTool, 1);

	if (ftool == NULL) {
		return NULL;
	}

	samplerate = lwdf_fp64_samplerate_get(flt);
	ftool->flt = flt;

	/* New frequency analysis object */
	ffr = lwdf_fp64_freq_new(flt, 8 * 1024);

	lwdf_fp64_freq_log_set(ffr, 0.5/256, 0.5, 256);
	ftool->ffr = ffr;

	window = GTK_WIDGET(gtk_builder_get_object(builder, "wnd_filter"));
	grid_gamma = GTK_WIDGET(gtk_builder_get_object(builder, "grid_gamma"));
	drawing_plot = GTK_WIDGET(gtk_builder_get_object(builder, "freq_plot"));

	ftool->window = window;

	/* update the gamma coefficients */
	cnt = lwdf_fp64_gamma_get(flt, gamma, sizeof(gamma) / sizeof(double));
	update_grid_gamma(ftool, GTK_GRID(grid_gamma), gamma, cnt);
	ftool->grid_gamma = grid_gamma;

	/* initialize the amplitude plot */
	figure = freq_response_plot_init(drawing_plot, samplerate);

	series = plt_line_series_new(figure, "Amp", npts);
	color = plt_stock_color_get();
	plt_series_line_color_set(series, color);
	plt_series_line_width_set(series, 2.0);

	ftool->ampl.figure = figure;
	ftool->ampl.series = series;
	ftool->ampl.drawing = drawing_plot;

	freq_plt_update(ftool->ffr, ftool->flt, ftool->ampl.series);

	return ftool;
}

void glwdf_freq_tool_free(GlwdfFreqTool * ftool)
{
	assert(ftool != NULL);

	lwdf_fp64_freq_free(ftool->ffr);

	g_free(ftool);

}

