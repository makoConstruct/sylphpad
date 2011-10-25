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

#ifndef _CALLBACK_H
#define _CALLBACK_H

GtkTextMark* insert_mark;
gboolean track_textview_movement(GtkWidget widget, GtkAllocation *allocation, gpointer user_data);
void track_cursor_catch_typing(GObject* obj, GParamSpec* pspec, gpointer ud);
//~ gboolean check_for_focus_loss(GtkWidget* menubar, GdkEventProperty* event, gpointer nothin);
gboolean on_mouse_move(GtkWidget* widget, GdkEventMotion* ev, gpointer ud);
//gboolean filter_out_alt_holds(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean returns_true(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean click_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean click_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean menu_on_release_key(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_release_key(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
gboolean on_press_key(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void on_file_new(void);
void on_file_open(void);
gint on_file_save(void);
gint on_file_save_as(void);
void on_file_print_preview(void);
void on_file_print(void);
void on_file_close(void);
void on_file_quit(void);
void on_edit_undo(void);
void on_edit_redo(void);
void on_edit_cut(void);
void on_edit_copy(void);
void on_edit_paste(void);
void on_edit_delete(void);
void on_edit_select_all(void);
void on_search_find(void);
void on_search_find_next(void);
void on_search_find_previous(void);
void on_search_replace(void);
void on_search_jump_to(void);
void on_option_font(void);
void on_option_word_wrap(void);
void on_option_line_numbers(void);
void on_option_always_on_top(void);
void on_option_auto_indent(void);
void on_help_about(void);

#endif /* _CALLBACK_H */
