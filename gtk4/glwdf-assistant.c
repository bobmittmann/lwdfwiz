/* Assistant
 *
 * Demonstrates a sample multi-step assistant with GtkAssistant. Assistants
 * are used to divide an operation into several simpler sequential steps,
 * and to guide the user through these steps.
 */

#include <gtk/gtk.h>
#include "glwdf-assistant.h"

G_BEGIN_DECLS

#define GLWDF_TYPE_ASSISTANT_BLK (glwdf_assistant_blk_get_type ())

G_DECLARE_FINAL_TYPE(GlwdfAssistantBlk, glwdf_assistant_blk, GLWDF, ASSISTANT_BLK, GObject)

//GlwdfAssistantBlk * glwdf_assistant_blk_new(void);
GlwdfAssistantBlk *  glwdf_assistant_blk_new(GtkWindow * parent, struct lwdfwiz_param * wiz);

G_END_DECLS

struct _GlwdfAssistantBlk {
	GtkAssistant *assistant;
	GtkWidget * butterworth_page1;
	struct lwdfwiz_param * wiz;
} ;

G_DEFINE_TYPE (GlwdfAssistantBlk, glwdf_assistant_blk, G_TYPE_OBJECT)


static void glwdf_assistant_blk_init (GlwdfAssistantBlk *blk)
{
}

static void glwdf_assistant_blk_class_init(GlwdfAssistantBlkClass *class)
{
	GlwdfAssistantBlkClass *blk_class = G_OBJECT (class);
}


static void assistant_on_prepare (GtkWidget *widget, GtkWidget *page, gpointer data)
{
	struct lwdfwiz_param * wiz;
	int current_page, n_pages;
	char *title;

	wiz = (struct lwdfwiz_param *)g_object_get_data(G_OBJECT(widget), "wiz-parm");

	g_message("glwdf_assistant_on_prepare::samplerate=%g", wiz->samplerate);

	current_page = gtk_assistant_get_current_page (GTK_ASSISTANT (widget));
	n_pages = gtk_assistant_get_n_pages (GTK_ASSISTANT (widget));

	title = g_strdup_printf ("Sample assistant (%d of %d)", current_page + 1, n_pages);
	gtk_window_set_title (GTK_WINDOW (widget), title);
	g_free (title);


#if 0
	/* The fourth page (counting from zero) is the progress page.  The
	 * user clicked Apply to get here so we tell the assistant to commit,
	 * which means the changes up to this point are permanent and cannot
	 * be cancelled or revisited. */
	if (current_page == 1)
		gtk_assistant_commit (GTK_ASSISTANT (widget));
#endif
}


static void assistant_on_apply (GtkWidget *widget, gpointer data)
{
	/* Close automatically once changes are fully applied. */
	gtk_window_destroy (GTK_WINDOW (data));
}

static void assistant_on_close (GtkWidget *widget, gpointer data)
{
	gtk_window_destroy (GTK_WINDOW (widget));
}

static void assistant_on_cancel (GtkWidget *widget, gpointer data)
{
	gtk_window_destroy (GTK_WINDOW (widget));
}

static void assistant_on_escape (GtkWidget *widget, gpointer data)
{
	gtk_window_destroy (GTK_WINDOW (widget));
}

static void samplerate_on_changed(GtkAssistant *assistant)
{
	struct lwdfwiz_param * wiz;

	gtk_assistant_update_buttons_state(assistant);

	wiz = (struct lwdfwiz_param *)g_object_get_data(G_OBJECT(assistant), "wiz-parm");
	g_message("samplerate=%g", wiz->samplerate);
}

static gboolean samplerate_on_validate(GtkAssistant *assistant,
									   GParamSpec *pspec,
									   GtkEntry *entry)
{
	GtkWidget *current_page;
	struct lwdfwiz_param * wiz;
	int page_number;
	const char * txt;
	double val;
	bool valid; 

	page_number = gtk_assistant_get_current_page (assistant);
	current_page = gtk_assistant_get_nth_page (assistant, page_number);

	txt = gtk_editable_get_text (GTK_EDITABLE (entry));

    val = strtod(txt, NULL);
	if (val == (double)0.0) {
		valid = false;
		g_message("on_samplerate_validate: invalid input=\"%s\"", txt);
	} else {
		valid = true;
		g_message("on_samplerate_validate: samplerate=\"%g\"", val);
		wiz = (struct lwdfwiz_param *)g_object_get_data(G_OBJECT(assistant), "wiz-parm");
		wiz->samplerate = val;
	}

	gtk_assistant_set_page_complete (assistant, current_page, valid);

	return valid;
}


#if 0
	static void
on_entry_changed (GtkWidget *widget, gpointer data)
{
	GtkAssistant *assistant = GTK_ASSISTANT (data);
	GtkWidget *current_page;
	int page_number;
	const char *text;

	page_number = gtk_assistant_get_current_page (assistant);
	current_page = gtk_assistant_get_nth_page (assistant, page_number);
	text = gtk_editable_get_text (GTK_EDITABLE (widget));

	if (text && *text)
		gtk_assistant_set_page_complete (assistant, current_page, TRUE);
	else
		gtk_assistant_set_page_complete (assistant, current_page, FALSE);
}
#endif

/* Design method radio button actions */
static void design_method_set_state (GSimpleAction *action,
									  GVariant      *state,
									  gpointer       user_data)
{
	struct lwdfwiz_param * wiz;

	wiz = (struct lwdfwiz_param *)user_data;
	
	g_message("design.method.set_state state=%s user_data=%p ftype=%d", 
			  g_variant_print(state, false), user_data, wiz->ftype);

	g_simple_action_set_state (action, state);
}

static void design_method_change_state (GSimpleAction *action,
										GVariant      *state,
										gpointer       user_data)
{
	g_message("design.method.change_state: state=%s user_data= %p", 
			  g_variant_print(state, false), user_data);

	g_action_change_state (G_ACTION (action), state);
}

/* Design response radio button actions */
static void design_response_set_state (GSimpleAction *action,
									  GVariant      *state,
									  gpointer       user_data)
{
	struct lwdfwiz_param * wiz;

	wiz = (struct lwdfwiz_param *)user_data;
	
	g_message("design.response.set_state state=%s user_data=%p ftype=%d", 
			  g_variant_print(state, false), user_data, wiz->ftype);

	g_simple_action_set_state (action, state);
}

static void design_response_change_state (GSimpleAction *action,
										GVariant      *state,
										gpointer       user_data)
{
	g_message("design.response.change_state: state=%s user_data= %p", 
			  g_variant_print(state, false), user_data);

	g_action_change_state (G_ACTION (action), state);
}

static GActionEntry design_entries[] = {
	{ "method", design_method_set_state, "i", "1", design_method_change_state},
	{ "response", design_response_set_state, "i", "1", design_response_change_state}
};


GtkWindow * glwdf_assistant_open (GtkWindow * parent, struct lwdfwiz_param * wiz)
{
	GlwdfAssistantBlk * blk;

	if ((blk = glwdf_assistant_blk_new(parent, wiz)) == NULL)
		return NULL;

	return GTK_WINDOW(blk->assistant);
}




GlwdfAssistantBlk *  glwdf_assistant_blk_new(GtkWindow * parent, struct lwdfwiz_param * wiz)
{
	GlwdfAssistantBlk * blk;
	GtkAssistant *assistant;
	GtkBuilder *builder; 
	GtkBuilderScope *scope;
	GtkWidget * butterworth_page1;
	GError * error = NULL;


	blk = GLWDF_ASSISTANT_BLK(g_object_new (glwdf_assistant_blk_get_type (), NULL));

	/* create a new builder */
	builder = gtk_builder_new ();

	/* create a new cscope for local (static) functions */
	scope = gtk_builder_cscope_new ();
	gtk_builder_cscope_add_callback_symbols (GTK_BUILDER_CSCOPE (scope),
		"samplerate_on_changed", G_CALLBACK (samplerate_on_changed),
		"samplerate_on_validate", G_CALLBACK (samplerate_on_validate),
		"assistant_on_apply", G_CALLBACK (assistant_on_apply),
		"assistant_on_close", G_CALLBACK (assistant_on_close),
		"assistant_on_cancel", G_CALLBACK (assistant_on_cancel),
		"assistant_on_escape", G_CALLBACK (assistant_on_escape),
		"assistant_on_prepare", G_CALLBACK (assistant_on_prepare),
		NULL);

	gtk_builder_set_scope (builder, scope);
	gtk_builder_expose_object (builder, "parent", G_OBJECT (parent));

	if (!gtk_builder_add_from_resource (builder, 
		"/glwdf_application/ui/glwdf-assistant.ui", &error)) {
		assistant = NULL;
		g_error ("failed to add UI: %s", error->message);
	} else if (!gtk_builder_add_from_resource (builder, 
		"/glwdf_application/ui/glwdf-assistant_butterworth.ui", &error)) {
		assistant = NULL;
		g_error ("failed to add UI: %s", error->message);
	} else {
		GActionGroup *actions;

		assistant = GTK_ASSISTANT(gtk_builder_get_object(builder, "assistant"));
		if (assistant == NULL) {
			g_error("gtk_builder_get_object(assistant) failed!");
		} else {
			blk->assistant = assistant;
			blk->butterworth_page1 = GTK_WIDGET(gtk_builder_get_object(builder, "butterworth_page1"));

			gtk_assistant_insert_page (assistant, butterworth_page1, 3);
			gtk_assistant_set_page_type (assistant, butterworth_page1, GTK_ASSISTANT_PAGE_CONTENT);
  			gtk_assistant_set_page_title (assistant, butterworth_page1, "Butterworth Page 1");

			g_object_set_data(G_OBJECT(assistant), "wiz-parm", wiz);

			actions = G_ACTION_GROUP (g_simple_action_group_new ());
			g_action_map_add_action_entries (G_ACTION_MAP (actions), 
											 design_entries, 
											 G_N_ELEMENTS (design_entries), wiz);
			gtk_widget_insert_action_group (GTK_WIDGET (assistant), 
											"design", G_ACTION_GROUP (actions));

			gtk_widget_show (GTK_WIDGET(assistant));

			//	GtkWidget *samplerate;
			//	samplerate = GTK_WIDGET(gtk_builder_get_object(builder, "samplerate_entry"));
			//	(void)samplerate;
			//		GtkWidget *btn1;
			//		btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "design_method"));

			//gtk_window_set_default_size (assistant, -1, 300);

			//	gtk_window_set_display (assistant, gtk_widget_get_display (GTK_WIDGET(partent)));
			//
			//		gtk_window_set_modal(assistant, false);
			//		gtk_window_set_transient_for(assistant, parent);


			//gtk_window_set_modal(assistant, true);
			// gtk_widget_set_parent(GTK_WIDGET(assistant), widget);
			//		g_signal_connect (G_OBJECT (assistant), "cancel",
			//						  G_CALLBACK (on_assistant_close_cancel), NULL);
			//		g_signal_connect (G_OBJECT (assistant), "close",
			//						  G_CALLBACK (on_assistant_close_cancel), NULL);
			//	g_signal_connect (G_OBJECT (assistant), "apply",
			//					  G_CALLBACK (on_assistant_apply), NULL);
			//		g_signal_connect (G_OBJECT (assistant), "prepare",
			//						  G_CALLBACK (on_assistant_prepare), NULL);
		}
	}

	g_object_unref (builder);
	g_object_unref (scope);

	return blk;
}




