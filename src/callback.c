/*
 *  Leafpad - GTK+ based simple text editor
 *  Copyright (C) 2004-2005 Tarot Osuji
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "leafpad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdkkeysyms.h>

unsigned block_converse_once=0; //each of the following two methods lead to the calling of the other. One does it recursively, the other concludes before its converse starts. This is an ugly solution but it's all that I can find;
unsigned selecting_active=0;
void track_cursor_catch_typing(GObject* obj, GParamSpec* pspec, gpointer ud){
	if(block_converse_once){block_converse_once=0; return;}
	block_converse_once=1;
	gint offset;
	g_object_get(obj, "cursor-position",&offset,NULL);
	gint x,y;
	GdkRectangle location;
	GtkTextIter iter;
	gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(obj),&iter,offset);
	gtk_text_view_get_iter_location(GTK_TEXT_VIEW(ud),&iter,&location);
	x=location.x+location.width/2; y=location.y+location.height/2;
	gtk_text_view_buffer_to_window_coords(GTK_TEXT_VIEW(ud), GTK_TEXT_WINDOW_TEXT, x, y, &x,&y);
	gint offx,offy;
	gdk_window_get_origin (gtk_widget_get_window(GTK_WIDGET(ud)), &offx, &offy);
	gdk_display_warp_pointer(gtk_widget_get_display(GTK_WIDGET(ud)) ,gtk_widget_get_screen(GTK_WIDGET(ud)) ,offx+x,offy+y);
}
gboolean on_mouse_move(GtkWidget* widget, GdkEventMotion* ev, gpointer ud){
	if(block_converse_once){block_converse_once=0; return 1;}
	block_converse_once=1;
	GtkTextIter iter;
	GtkTextIter beforemoveiter;
	gint x,y;
	GdkRectangle location;
	gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(widget), GTK_TEXT_WINDOW_TEXT, ev->x, ev->y, &x,&y);
	gtk_text_view_get_iter_at_position(GTK_TEXT_VIEW(widget), &iter, NULL, x,y);
	gtk_text_view_get_iter_location(GTK_TEXT_VIEW(widget),&iter,&location);
	if(x > location.x+location.width/2) gtk_text_iter_forward_char(&iter);
	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(ud), &beforemoveiter, insert_mark);
	if(gtk_text_iter_compare(&iter,&beforemoveiter)){ //is 0 only if they're equal;
		if(selecting_active){
			//fprintf(stdout,"a\n");
			gtk_text_buffer_move_mark(GTK_TEXT_BUFFER(ud), insert_mark, &iter);
		}else{
			//fprintf(stdout,"b\n");
			gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(ud), &iter);
		}
	}
	return 1;
}

gboolean returns_true(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	return 1;
}

gboolean click_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	selecting_active=1;
	return 0;
	//((GdkEventButton*)event) //a useful stub for later;
}
gboolean click_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	selecting_active=0;
	return 0;
}

void position_func(GtkMenu* item, gint* x, gint* y, gboolean* push_in, gpointer window){
	*push_in = 1;
	gdk_window_get_origin (gtk_widget_get_window (GTK_WIDGET(window)), x, y);
}

gboolean on_press_key(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	if(event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R){
		gtk_menu_popup((GtkMenu*)user_data, NULL, NULL, position_func, widget, 0, event->time); //why the hell do you want the parent object in a separate variable?
	}else if(event->keyval == GDK_KEY_Shift_L || event->keyval == GDK_KEY_Shift_R){
		selecting_active=1;
	}
	return 0;
}

gboolean on_release_key(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	if(event->keyval == GDK_KEY_Shift_L || event->keyval == GDK_KEY_Shift_R){
		selecting_active=0 ;
	}
	return 0;
}

gboolean menu_on_release_key(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
	if(event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R){
		gtk_menu_popdown((GtkMenu*)user_data);
	}
	return 0;
}

static void set_selection_bound(GtkTextBuffer *buffer, gint start, gint end)
{
	GtkTextIter start_iter, end_iter;
	
	gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, start);
	if (end < 0)
		gtk_text_buffer_get_end_iter(buffer, &end_iter);
	else
		gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, end);
	gtk_text_buffer_place_cursor(buffer, &end_iter);
	gtk_text_buffer_move_mark_by_name(buffer, "selection_bound", &start_iter);
}

void on_file_new(void)
{
	gchar *comline;
	gchar *option;

	save_config_file();
	option = pub->fi->charset_flag ?
		g_strdup_printf("%s%s", " --codeset=", pub->fi->charset) : "";
	comline = g_strdup_printf("%s%s", PACKAGE, option);
	if (pub->fi->charset_flag)
		g_free(option);
	g_spawn_command_line_async(comline, NULL);
	g_free(comline);
}

void on_file_open(void)
#ifdef ENABLE_CSDI
{ // too slow...
	FileInfo *fi;
	gchar *comline;
	gchar *option = NULL;
	
	fi = get_fileinfo_from_selector(pub->fi, OPEN);
	if (fi) {
		save_config_file();
		option = g_strdup_printf("--codeset=%s ", fi->charset);
		comline = g_strdup_printf("%s %s%s", PACKAGE,
			fi->charset ? option : "",
			fi->filename);
		g_spawn_command_line_async(comline, NULL);
		g_free(option);
		g_free(comline);
		g_free(fi);
	}
}
#else
{
	FileInfo *fi;
	
	if (check_text_modification())
		return;
	fi = get_fileinfo_from_selector(pub->fi, OPEN);
	if (fi) {
		if (file_open_real(pub->mw->view, fi))
			g_free(fi);
		else {
			g_free(pub->fi);
			pub->fi = fi;
			undo_clear_all(pub->mw->buffer);
//			set_main_window_title();
			force_call_cb_modified_changed(pub->mw->view);
//			undo_init(sd->mainwin->textview, sd->mainwin->textbuffer, sd->mainwin->menubar);
		}
	}
}
#endif

gint on_file_save(void)
{
	if (pub->fi->filename == NULL)
		return on_file_save_as();
	if (check_file_writable(pub->fi->filename) == FALSE)
		return on_file_save_as();
	if (file_save_real(pub->mw->view, pub->fi))
		return -1;
//	set_main_window_title();
	force_call_cb_modified_changed(pub->mw->view);
//	undo_reset_step_modif();
	return 0;
}

gint on_file_save_as(void)
{
	FileInfo *fi;
	
	fi = get_fileinfo_from_selector(pub->fi, SAVE);
	if (fi == NULL)
		return -1;
	if (file_save_real(pub->mw->view, fi)) {
		g_free(fi);
		return -1;
	}
	g_free(pub->fi);
	pub->fi = fi;
	undo_clear_all(pub->mw->buffer);
//	set_main_window_title();
	force_call_cb_modified_changed(pub->mw->view);
//	undo_init(sd->mainwin->textview, sd->mainwin->textbuffer, sd->mainwin->menubar);
	return 0;
}
#ifdef ENABLE_PRINT
#	if GTK_CHECK_VERSION(2, 10, 0)
void on_file_print_preview(void)
{
	create_gtkprint_preview_session(GTK_TEXT_VIEW(pub->mw->view),
		get_file_basename(pub->fi->filename, FALSE));
}

void on_file_print(void)
{
	create_gtkprint_session(GTK_TEXT_VIEW(pub->mw->view),
		get_file_basename(pub->fi->filename, FALSE));
}
#	else
void on_file_print(void)
{
	create_gnomeprint_session();
}
#	endif
#endif
void on_file_close(void)
{
	if (!check_text_modification()) {
		force_block_cb_modified_changed(pub->mw->view);
//		undo_block_signal(textbuffer);
		gtk_text_buffer_set_text(pub->mw->buffer, "", 0);
		gtk_text_buffer_set_modified(pub->mw->buffer, FALSE);
		if (pub->fi->filename)
			g_free(pub->fi->filename);
		pub->fi->filename = NULL;
		if (pub->fi->charset)
			g_free(pub->fi->charset);
		pub->fi->charset = NULL;
		pub->fi->charset_flag = FALSE;
		pub->fi->lineend = LF;
		undo_clear_all(pub->mw->buffer);
//		set_main_window_title();
		force_call_cb_modified_changed(pub->mw->view);
		force_unblock_cb_modified_changed(pub->mw->view);
//		undo_unblock_signal(textbuffer);
//		undo_init(sd->mainwin->textview, textbuffer, sd->mainwin->menubar);
	}
}

void on_file_quit(void)
{
	if (!check_text_modification()) {
		save_config_file();
		gtk_main_quit();
	}
}

void on_edit_undo(void)
{
	undo_undo(pub->mw->buffer);
}

void on_edit_redo(void)
{
	undo_redo(pub->mw->buffer);
}

void on_edit_cut(void)
{
	g_signal_emit_by_name(G_OBJECT(pub->mw->view), "cut-clipboard");
}

void on_edit_copy(void)
{
	g_signal_emit_by_name(G_OBJECT(pub->mw->view), "copy-clipboard");
}

void on_edit_paste(void)
{
	g_signal_emit_by_name(G_OBJECT(pub->mw->view), "paste-clipboard");
//	TODO: Use modify signal!!
/*	gtk_text_view_scroll_mark_onscreen(
		GTK_TEXT_VIEW(pub->mw->view),
		gtk_text_buffer_get_insert(pub->mw->buffer));
*/}

void on_edit_delete(void)
{
	gtk_text_buffer_delete_selection(pub->mw->buffer, TRUE, TRUE);
}

void on_edit_select_all(void)
{
	set_selection_bound(pub->mw->buffer, 0, -1);
//	g_signal_emit_by_name(G_OBJECT(pub->mw->view), "select-all");
}

static void activate_quick_find(void)
{
	GtkItemFactory *ifactory;
	static gboolean flag = FALSE;
	
	if (!flag) {
		ifactory = gtk_item_factory_from_widget(pub->mw->menubar);
		gtk_widget_set_sensitive(
			gtk_item_factory_get_widget(ifactory, "/Search/Find Next"),
			TRUE);
		gtk_widget_set_sensitive(
			gtk_item_factory_get_widget(ifactory, "/Search/Find Previous"),
			TRUE);
		flag = TRUE;
	}
}

void on_search_find(void)
{
	if (run_dialog_search(pub->mw->view, 0) == GTK_RESPONSE_OK)
		activate_quick_find();
}

void on_search_find_next(void)
{
	document_search_real(pub->mw->view, 1);
}

void on_search_find_previous(void)
{
	document_search_real(pub->mw->view, -1);
}

void on_search_replace(void)
{
	if (run_dialog_search(pub->mw->view, 1) == GTK_RESPONSE_OK)
		activate_quick_find();
}

void on_search_jump_to(void)
{
	run_dialog_jump_to(pub->mw->view);
}

void on_option_font(void)
{
	change_text_font_by_selector(pub->mw->view);
}

void on_option_word_wrap(void)
{
	GtkItemFactory *ifactory;
	gboolean state;
	
	ifactory = gtk_item_factory_from_widget(pub->mw->menubar);
	state = gtk_check_menu_item_get_active(
		GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(ifactory, "/Options/Word Wrap")));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(pub->mw->view),
		state ? GTK_WRAP_WORD : GTK_WRAP_NONE);
}

void on_option_line_numbers(void)
{
	GtkItemFactory *ifactory;
	gboolean state;
	
	ifactory = gtk_item_factory_from_widget(pub->mw->menubar);
	state = gtk_check_menu_item_get_active(
		GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(ifactory, "/Options/Line Numbers")));
	show_line_numbers(pub->mw->view, state);
}

void on_option_always_on_top(void)
{
#if GTK_CHECK_VERSION(2, 4, 0)
	static gboolean flag = FALSE;
	
	flag =! flag;
	gtk_window_set_keep_above(GTK_WINDOW(pub->mw->window), flag);
#endif
}

void on_option_auto_indent(void)
{
	GtkItemFactory *ifactory;
	gboolean state;
	
	ifactory = gtk_item_factory_from_widget(pub->mw->menubar);
	state = gtk_check_menu_item_get_active(
		GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(ifactory, "/Options/Auto Indent")));
	indent_set_state(state);
}

void on_help_about(void)
{
	const gchar *copyright = "Copyright \xc2\xa9 2004-2009 Tarot Osuji";
	const gchar *comments = _("GTK+ based simple text editor");
	const gchar *authors[] = {
		"Tarot Osuji <tarot@sdf.lonestar.org>",
		NULL
	};
	const gchar *translator_credits = _("translator-credits");
	
	translator_credits = strcmp(translator_credits, "translator-credits")
		? translator_credits : NULL;
	
#if GTK_CHECK_VERSION(2, 6, 0)
	const gchar *artists[] = {
		"Lapo Calamandrei <calamandrei@gmail.com>",
		NULL
	};
	gtk_show_about_dialog(GTK_WINDOW(pub->mw->window),
//		"name", PACKAGE_NAME,
		"version", PACKAGE_VERSION,
		"copyright", copyright,
		"comments", comments,
		"authors", authors,
		"artists", artists,
		"translator-credits", translator_credits,
		"logo-icon-name", PACKAGE,
		NULL);
#else
	static GtkWidget *about = NULL;

	if (about != NULL) {
		gtk_window_present(GTK_WINDOW(about));
		return;
	}
	
	const gchar *documenters[] = {
		NULL
	};
	GdkPixbuf *logo = gdk_pixbuf_new_from_file(
		ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png", NULL);
	about = create_about_dialog(
		PACKAGE_NAME,
		PACKAGE_VERSION,
		copyright,
		comments,
		authors,
		documenters,
		translator_credits,
		logo);
	if (logo)
		g_object_unref(logo);
	gtk_window_set_transient_for(GTK_WINDOW(about),
		GTK_WINDOW(pub->mw->window));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(about), TRUE);
	g_signal_connect(G_OBJECT(about), "destroy",
		G_CALLBACK(gtk_widget_destroyed), &about);

	gtk_widget_show(about);
#endif
}
