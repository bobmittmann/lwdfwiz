#include "config.h"

#include "glwdf.h"

#include <gtk/gtk.h>

#include "glwdf-application_window.h"
#include "glwdf-frequency_control.h"
#include "glwdf-filter_control.h"
#include "glwdf-filter_view.h"
#include "glwdf-assistant.h"

	struct _GlwdfApplicationWindow {
		GtkApplicationWindow parent_instance;

		GtkWidget *view_stack;
		GtkWidget *ctrl_stack;

		GtkWidget *filter_ctrl;
		GtkWidget *filter_view;


		GtkWidget *freq_ctrl;
		GtkWidget *freq_view;

		GtkWidget *time_ctrl;
		GtkWidget *time_view;

		GtkWidget *code_ctrl;
		GtkWidget *code_view;

		GtkWindow *assistant;

		GtkWidget *filtertoolbar;

		GtkWidget *toolbar;
		GtkWidget *status;
		GtkTextBuffer *buffer;

		int width;
		int height;
		gboolean maximized;
		gboolean fullscreen;

		struct lwdfwiz_param lwdfwiz_param;

	} ;

G_DEFINE_TYPE (GlwdfApplicationWindow, glwdf_application_window, GTK_TYPE_APPLICATION_WINDOW)

void
show_action_dialog (GSimpleAction *action)
{
	const char *name;
	GtkWidget *dialog;

	name = g_action_get_name (G_ACTION (action));

	dialog = gtk_message_dialog_new (NULL,
									 GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_CLOSE,
									 "You activated action: \"%s\"",
									 name);

	g_signal_connect (dialog, "response",
					  G_CALLBACK (gtk_window_destroy), NULL);

	gtk_widget_show (dialog);
}

static void activate_assistant (GSimpleAction *action,
								GVariant      *parameter,
								gpointer       user_data)
{
	GlwdfApplicationWindow *window = user_data;
	GtkWindow *assistant;

	assistant = glwdf_assistant_open (GTK_WINDOW(window), &window->lwdfwiz_param);


	window->assistant = assistant;
}

	static void
activate_action (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	show_action_dialog (action);
}


	static void
activate_toggle (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	GVariant *state;

	state = g_action_get_state (G_ACTION (action));
	g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));
	g_variant_unref (state);
}

	static void
activate_about (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	GtkWidget *window = user_data;

	glwdf_about_dialog_show(GTK_WIDGET(window));
}

void update_statusbar (GtkTextBuffer         *buffer,
				  GlwdfApplicationWindow *window)
{
	char *msg;
	int row, col;
	int count;
	GtkTextIter iter;

	/* clear any previous message, underflow is allowed */
	gtk_statusbar_pop (GTK_STATUSBAR (window->status), 0);

	count = gtk_text_buffer_get_char_count (buffer);

	gtk_text_buffer_get_iter_at_mark (buffer,
									  &iter,
									  gtk_text_buffer_get_insert (buffer));

	row = gtk_text_iter_get_line (&iter);
	col = gtk_text_iter_get_line_offset (&iter);

	msg = g_strdup_printf ("Cursor at row %d column %d - %d chars in document",
						   row, col, count);

	gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, msg);

	g_free (msg);
}

	static void
mark_set_callback (GtkTextBuffer         *buffer,
				   const GtkTextIter     *new_location,
				   GtkTextMark           *mark,
				   GlwdfApplicationWindow *window)
{
	update_statusbar (buffer, window);
}

	static void
change_radio_state (GSimpleAction *action,
					GVariant      *state,
					gpointer       user_data)
{
	g_simple_action_set_state (action, state);
}

	static void
activate_radio (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	g_action_change_state (G_ACTION (action), parameter);
}


static void glwdf_application_window_store_state (GlwdfApplicationWindow *win)
{
	GSettings *settings;

	settings = g_settings_new ("org.gtk.Demo4");
	g_settings_set (settings, "window-size", "(ii)", win->width, win->height);
	g_settings_set_boolean (settings, "maximized", win->maximized);
	g_settings_set_boolean (settings, "fullscreen", win->fullscreen);
	g_object_unref (settings);
}

static void glwdf_application_window_load_state (GlwdfApplicationWindow *win)
{
	GSettings *settings;

	settings = g_settings_new ("org.gtk.Demo4");
	g_settings_get (settings, "window-size", "(ii)", &win->width, &win->height);
	win->maximized = g_settings_get_boolean (settings, "maximized");
	win->fullscreen = g_settings_get_boolean (settings, "fullscreen");
	g_object_unref (settings);
}

//
// Workbench view change
//
static void selection_changed (GtkSelectionModel * view_pages, guint position, guint n_items,
							   GlwdfApplicationWindow *window)
{
	GtkSelectionModel *ctrl_pages;
	GtkBitset * selected;
	int i;

	g_message("selection_changed, position=%d n_itmes=%d", position, n_items);

	ctrl_pages = gtk_stack_get_pages(GTK_STACK(window->ctrl_stack));

//	pages = gtk_stack_get_pages(GTK_STACK(window->view_stack));

	g_message("pages=%p\n", ctrl_pages);

	selected = gtk_selection_model_get_selection(view_pages);

	for (i = position; i < n_items; i++)
	{
		if (gtk_bitset_contains (selected, i))
			position = i;
	}

	gtk_selection_model_select_item (ctrl_pages, position, true);

//	gtk_selection_model_set_selection(ctrl_pages, selection, selection);
}

	static void
window_close(GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	GtkWindow *window = user_data;

	gtk_window_close(window);
}


static GActionEntry win_entries[] = {
	{ "close", window_close, NULL, NULL, NULL },
	{ "assistant", activate_assistant, NULL, NULL, NULL },

	{ "shape", activate_radio, "s", "'oval'", change_radio_state },
	{ "bold", activate_toggle, NULL, "false", NULL },
	{ "about", activate_about, NULL, NULL, NULL },
	{ "file1", activate_action, NULL, NULL, NULL },
	{ "logo", activate_action, NULL, NULL, NULL }
};


	static void
glwdf_application_window_init (GlwdfApplicationWindow *window)
{
	GtkSelectionModel *pages;

	window->width = -1;
	window->height = -1;
	window->maximized = FALSE;
	window->fullscreen = FALSE;

	g_message("4.\n");

	gtk_widget_init_template (GTK_WIDGET (window));

	g_message("5.\n");

	g_action_map_add_action_entries (G_ACTION_MAP (window),
									 win_entries, G_N_ELEMENTS (win_entries),
									 window);

	g_message("6.\n");

	pages = gtk_stack_get_pages(GTK_STACK(window->view_stack));

	g_signal_connect (pages,
					  "selection-changed",
					  G_CALLBACK (selection_changed),
					  window);

	g_message("7.\n");

}

	static void
glwdf_application_window_constructed (GObject *object)
{
	GlwdfApplicationWindow *window = (GlwdfApplicationWindow *)object;

	glwdf_application_window_load_state (window);

	gtk_window_set_default_size (GTK_WINDOW (window), window->width, window->height);

	if (window->maximized)
		gtk_window_maximize (GTK_WINDOW (window));

	if (window->fullscreen)
		gtk_window_fullscreen (GTK_WINDOW (window));

	G_OBJECT_CLASS (glwdf_application_window_parent_class)->constructed (object);
}

	static void
glwdf_application_window_size_allocate (GtkWidget *widget,
										int        width,
										int        height,
										int        baseline)
{
	GlwdfApplicationWindow *window = (GlwdfApplicationWindow *)widget;

	GTK_WIDGET_CLASS (glwdf_application_window_parent_class)->size_allocate (widget,
																			 width,
																			 height,
																			 baseline);

	if (!window->maximized && !window->fullscreen)
		gtk_window_get_default_size (GTK_WINDOW (window), &window->width, &window->height);
}

	static void
surface_state_changed (GtkWidget *widget)
{
	GlwdfApplicationWindow *window = (GlwdfApplicationWindow *)widget;
	GdkToplevelState new_state;

	new_state = gdk_toplevel_get_state (GDK_TOPLEVEL (gtk_native_get_surface (GTK_NATIVE (widget))));
	window->maximized = (new_state & GDK_TOPLEVEL_STATE_MAXIMIZED) != 0;
	window->fullscreen = (new_state & GDK_TOPLEVEL_STATE_FULLSCREEN) != 0;
}

	static void
glwdf_application_window_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (glwdf_application_window_parent_class)->realize (widget);

	g_signal_connect_swapped (gtk_native_get_surface (GTK_NATIVE (widget)), "notify::state",
							  G_CALLBACK (surface_state_changed), widget);
}

static void glwdf_application_window_unrealize (GtkWidget *widget)
{
	g_signal_handlers_disconnect_by_func (gtk_native_get_surface (GTK_NATIVE (widget)),
										  surface_state_changed, widget);

	GTK_WIDGET_CLASS (glwdf_application_window_parent_class)->unrealize (widget);
}

static void glwdf_application_window_dispose (GObject *object)
{
	GlwdfApplicationWindow *window = (GlwdfApplicationWindow *)object;

	glwdf_application_window_store_state (window);

	G_OBJECT_CLASS (glwdf_application_window_parent_class)->dispose (object);
}

static void glwdf_application_window_class_init (GlwdfApplicationWindowClass *class)
{
	GtkIconTheme *icon_theme;
	GdkDisplay* display;

	display = gdk_display_get_default();
	icon_theme = gtk_icon_theme_get_for_display(display);
	gtk_icon_theme_add_resource_path(icon_theme, "/glwdf_application/icons");

	if (!gtk_icon_theme_has_icon(icon_theme, "glwdf-app")) { 
		fprintf(stderr, "Application icon not found...!\n");
	}

	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

	object_class->constructed = glwdf_application_window_constructed;
	object_class->dispose = glwdf_application_window_dispose;

	widget_class->size_allocate = glwdf_application_window_size_allocate;
	widget_class->realize = glwdf_application_window_realize;
	widget_class->unrealize = glwdf_application_window_unrealize;

	g_message("1.\n");

	gtk_widget_class_set_template_from_resource(widget_class, "/glwdf_application/ui/glwdf-application_window.ui");
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, toolbar);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, status);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, buffer);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, view_stack);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, ctrl_stack);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, filter_ctrl);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, filter_view);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, freq_ctrl);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, freq_view);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, time_ctrl);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, time_view);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, code_ctrl);
	gtk_widget_class_bind_template_child(widget_class, GlwdfApplicationWindow, code_view);

	g_message("2.\n");

	gtk_widget_class_bind_template_callback(widget_class, update_statusbar);
	gtk_widget_class_bind_template_callback(widget_class, mark_set_callback);

	g_message("3.\n");
}

