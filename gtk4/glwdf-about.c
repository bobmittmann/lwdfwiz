#include "config.h"

#include "glwdf.h"

#include <gtk/gtk.h>

void glwdf_about_dialog_show(GtkWidget *window)
{
  const char *authors[] = {
    "Bob Mittmann <bobmittmann@gmail.com>",
    NULL
  };

  const char *documentors[] = {
    "Bob Mittmann <bobmittmann@gmail.com>",
    NULL
  };

  gtk_show_about_dialog (GTK_WINDOW (window),
                         "program-name", "GTK WLDF Wizard",
                         "version", g_strdup_printf ("%s", PACKAGE_VERSION),
                         "copyright", "(C) 2021 Bob Mittmann",
                         "license-type", GTK_LICENSE_GPL_3_0,
                         "website", "http://github.com/bobmittmann/lwdfwiz",
                         "comments", "Lattice Wave Digital Filters Wizard.",
                         "authors", authors,
                         "documenters", documentors,
                         "logo-icon-name", "glwdf-app",
                         "title", "About GTK Code Demos",
                         NULL);
}

