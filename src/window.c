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
#include "scroller.h"
/*
static void cb_scroll_event(GtkAdjustment *adj, GtkWidget *view)
{
	gtk_text_view_place_cursor_onscreen(GTK_TEXT_VIEW(view));
}
*/
MainWin *create_main_window(void)
{
	GtkWidget *window;
	GtkWidget *vbox;
 	GtkWidget *menu;
 	GtkWidget *sw;
 	GtkWidget *view;
// 	gint size;
	//GtkAdjustment *hadj, *vadj;
	
	MainWin *mw = g_malloc(sizeof(MainWin));
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(window, PACKAGE_NAME);
	
	
/*
#if GTK_CHECK_VERSION(2, 4, 0)
//	size = gtk_icon_size_lookup(GTK_ICON_SIZE_LARGE_TOOLBAR, &size, NULL);
	icon = gtk_icon_theme_load_icon(
		gtk_icon_theme_get_default(),
		PACKAGE,
		16, // size
		0,  // flags
		NULL);
	gtk_window_set_default_icon(icon);
*/
#if GTK_CHECK_VERSION(2, 6, 0)
	gtk_window_set_default_icon_name(PACKAGE);
#else
 	GdkPixbuf *icon = gdk_pixbuf_new_from_file(
		ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png", NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);
	if (icon)
		g_object_unref(icon);
#endif
	
	g_signal_connect(G_OBJECT(window), "delete-event",
		G_CALLBACK(on_file_quit), NULL);
	g_signal_connect_after(G_OBJECT(window), "delete-event",
		G_CALLBACK(gtk_widget_hide_on_delete), NULL);
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	menu = create_menu_line(window);
	//~ gtk_box_pack_start(GTK_BOX(vbox), menu, FALSE, FALSE, 0);
	
	g_signal_connect(G_OBJECT(menu), "can-activate-accel",
		G_CALLBACK(returns_true), NULL);

	view = create_text_view();
	
	GObject* basebuffer;
	g_object_get(G_OBJECT(view), "buffer",&basebuffer, NULL);
	insert_mark = gtk_text_buffer_get_insert(basebuffer); //to a global in callback.c
	
	g_signal_connect(G_OBJECT(basebuffer), "notify::cursor-position",
		G_CALLBACK(track_cursor_catch_typing), view);
	sw = gtk_scrolled_window_new(NULL, NULL);
	g_signal_connect(G_OBJECT(view), "motion-notify-event",
		G_CALLBACK(on_mouse_move), basebuffer);
	g_signal_connect(G_OBJECT(view), "button-press-event",
		G_CALLBACK(click_press), basebuffer);
	g_signal_connect(G_OBJECT(view), "button-release-event",
		G_CALLBACK(click_release), basebuffer);
	g_signal_connect(G_OBJECT(window), "key-press-event",
		G_CALLBACK(on_press_key), menu);
	g_signal_connect(G_OBJECT(window), "key-release-event",
		G_CALLBACK(on_release_key), menu);
	g_signal_connect(G_OBJECT(menu), "key-release-event",
		G_CALLBACK(menu_on_release_key), menu);
	/*g_signal_connect(G_OBJECT(menu), "key-press-event",
		G_CALLBACK(filter_out_alt_holds), NULL);*/
	
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw),  GTK_SHADOW_NONE);
	gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);
	
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	
	
	gtk_container_add(GTK_CONTAINER(sw), view);
	
	//~ g_signal_connect_after(G_OBJECT(view), "expose_event",
		//~ G_CALLBACK(scroller_redrawing), sw);
		
	//~ g_signal_connect_data(G_OBJECT(menu), "property-notify-event",
		//~ G_CALLBACK(check_for_focus_loss), menu, NULL, 1);
	
	//fprintf(stdout, "menu %x, scrollwindow %x", &(menu->window), &(sw->window));
	
	//hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(sw));
	//vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(sw));
	/*g_signal_connect_after(G_OBJECT(hadj), "value-changed",
		G_CALLBACK(on_adjustment), view);
	g_signal_connect_after(G_OBJECT(vadj), "value-changed",
		G_CALLBACK(cb_scroll_event), view);*/

	mw->window = window;
	mw->menubar = menu;
	mw->view = view;
	mw->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	
	return mw;
}

void set_main_window_title(void)
{
	gchar *title;
	
	title = get_file_basename(pub->fi->filename, TRUE);
	gtk_window_set_title(GTK_WINDOW(pub->mw->window), title);
	g_free(title);
}

