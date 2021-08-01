#include "config.h"

#include "glwdf.h"

#include <gtk/gtk.h>

#include "glwdf-application.h"
#include "glwdf-application_window.h"

struct _GlwdfApplication {
	GtkApplication parent_instance;
	unsigned int app_window_count;

} ;

G_DEFINE_TYPE (GlwdfApplication, glwdf_application, GTK_TYPE_APPLICATION)


void show_action_dialog (GSimpleAction *action);



	static void
activate_action (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	show_action_dialog (action);
}

static void create_window (GApplication *app,
						   const char   *content)
{
	GlwdfApplication * glwdf_app = GLWDF_APPLICATION(app);
	GlwdfApplicationWindow *window;

	(void)glwdf_app;

	window = (GlwdfApplicationWindow *)g_object_new (glwdf_application_window_get_type (),
													 "application", app,
													 "show-menubar", TRUE,
													 NULL);
	//	if (content)
	//		gtk_text_buffer_set_text (window->buffer, content, -1);

	gtk_window_present (GTK_WINDOW (window));


	if (glwdf_app->app_window_count > 1) {
		/* */
		gtk_widget_activate_action(GTK_WIDGET(window), "win.assistant", NULL);
	} else {
		gtk_widget_activate_action(GTK_WIDGET(window), "win.assistant", NULL);
//		printf("Open existing...\n");
	}

	glwdf_app->app_window_count++;

}


	static void
activate_new (GSimpleAction *action,
			  GVariant      *parameter,
			  gpointer       user_data)
{
	GApplication *app = user_data;

	create_window (app, NULL);
}

	static void
open_response_cb (GtkNativeDialog *dialog,
				  int              response_id,
				  gpointer         user_data)
{
	GtkFileChooserNative *native = user_data;
	GApplication *app = g_object_get_data (G_OBJECT (native), "app");
	GtkWidget *message_dialog;
	GFile *file;
	char *contents;
	GError *error = NULL;

	if (response_id == GTK_RESPONSE_ACCEPT) {
		file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (native));

		if (g_file_load_contents (file, NULL, &contents, NULL, NULL, &error)) {
			create_window (app, contents);
			g_free (contents);
		} else {
			message_dialog = gtk_message_dialog_new (NULL,
													 GTK_DIALOG_DESTROY_WITH_PARENT,
													 GTK_MESSAGE_ERROR,
													 GTK_BUTTONS_CLOSE,
													 "Error loading file: \"%s\"",
													 error->message);
			g_signal_connect (message_dialog, "response",
							  G_CALLBACK (gtk_window_destroy), NULL);
			gtk_widget_show (message_dialog);
			g_error_free (error);
		}
	}

	gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (native));
	g_object_unref (native);
}


	static void
activate_open (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	GApplication *app = user_data;
	GtkFileChooserNative *native;

	native = gtk_file_chooser_native_new ("Open File",
										  NULL,
										  GTK_FILE_CHOOSER_ACTION_OPEN,
										  "_Open",
										  "_Cancel");

	g_object_set_data_full (G_OBJECT (native), "app", g_object_ref (app), g_object_unref);
	g_signal_connect (native,
					  "response",
					  G_CALLBACK (open_response_cb),
					  native);

	gtk_native_dialog_show (GTK_NATIVE_DIALOG (native));
}

	static void
activate_quit (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	GtkApplication *app = user_data;
	GtkWidget *win;
	GList *list, *next;

	list = gtk_application_get_windows (app);
	while (list)
	{
		win = list->data;
		next = list->next;

		gtk_window_destroy (GTK_WINDOW (win));

		list = next;
	}
}

static GActionEntry app_entries[] = {
	{ "new", activate_new, NULL, NULL, NULL },
	{ "open", activate_open, NULL, NULL, NULL },
	{ "save", activate_action, NULL, NULL, NULL },
	{ "save-as", activate_action, NULL, NULL, NULL },
	{ "quit", activate_quit, NULL, NULL, NULL }
};

static void startup (GApplication *app)
{
	GtkBuilder *builder;
	GtkWidget *filtertoolbar;

	G_APPLICATION_CLASS (glwdf_application_parent_class)->startup(app);

	builder = gtk_builder_new();
	gtk_builder_add_from_resource(builder, "/glwdf_application/ui/menus.ui", NULL);
	gtk_builder_add_from_resource(builder, "/glwdf_application/ui/toolbars.ui", NULL);


	gtk_application_set_menubar(GTK_APPLICATION(app), 
								G_MENU_MODEL(gtk_builder_get_object(builder, "menubar")));

	filtertoolbar = GTK_WIDGET(gtk_builder_get_object(builder, "filtertoolbar"));
	if (filtertoolbar == NULL) {
		printf("ERROR!!!!!!!!!!!!!!!!\n");
	}

	g_object_unref (builder);
}

static void activate (GApplication *app)
{
	create_window (app, NULL);
}

static void glwdf_application_init (GlwdfApplication *app)
{
	GSettings *settings;
	GAction *action;

	settings = g_settings_new ("org.gtk.Demo4");

	g_action_map_add_action_entries (G_ACTION_MAP (app),
									 app_entries, G_N_ELEMENTS (app_entries),
									 app);

	action = g_settings_create_action (settings, "color");

	g_action_map_add_action (G_ACTION_MAP (app), action);

	g_object_unref (settings);

	app->app_window_count = 1;
}

static void glwdf_application_class_init(GlwdfApplicationClass *class)
{
	GApplicationClass *app_class = G_APPLICATION_CLASS (class);

	app_class->startup = startup;
	app_class->activate = activate;
}

int main(int argc, char *argv[])
{
	GtkApplication *app;


	app = GTK_APPLICATION (g_object_new (glwdf_application_get_type (),
										 "application-id", "glwdf.app",
										 "flags", G_APPLICATION_HANDLES_OPEN,
										 NULL));

	return g_application_run (G_APPLICATION (app), 0, NULL);
}

