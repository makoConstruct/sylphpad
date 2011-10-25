#include <math.h>

const unsigned scroller_animationlength=2;
typedef struct{
	GdkRectangle clickzone;
	gboolean textbox_hasupdated;
	gboolean scroller_isleft;
}ScrollerData;
ScrollerData scroller_data;

void init_ScrollerData(GtkTextView* view){ //took so long to realize this was the right idea;
	
	scroller_data.clickzone.width = 15;
	scroller_data.clickzone.x ;
}
gboolean scroller_draw(GtkWidget *widget, GdkEventExpose *event, gpointer scrolled_window){
	cairo_t* cr = gdk_cairo_create(GDK_DRAWABLE(gtk_widget_get_window(scrolled_window)));
	cairo_pattern_t* pattern;
	GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
	const double minimum_bar_h = 7;
	const double outeralpha = 0.1098;
	const double inneralpha = 0.2392;
	const double bar_gradw = 8;
	const double bar_offx = 2, bar_buffy=1;
	const double cornerw=6;
	const double span = GTK_WIDGET(scrolled_window)->allocation.height - 2*bar_buffy;
	double scroll = gtk_adjustment_get_upper(adj) - gtk_adjustment_get_lower(adj);
	double page = gtk_adjustment_get_page_size(adj);
	double barh = page/scroll * (span);
	if(barh<minimum_bar_h) barh = minimum_bar_h;
	double cornerx = GTK_WIDGET(scrolled_window)->allocation.x + GTK_WIDGET(scrolled_window)->allocation.width - bar_offx - scroller_data.clickzone.width;
	double cornery = GTK_WIDGET(scrolled_window)->allocation.y + bar_buffy + (gtk_adjustment_get_value(adj)/(scroll-page))*(span - barh);
	pub->sd.clickzone.x = cornerx;
	pub->sd.clickzone.y = cornery;
	pub->sd.clickzone.width = scroller_data.clickzone.width;
	pub->sd.clickzone.height = car_w;
	pattern = cairo_pattern_create_linear(cornerx+scroller_data.clickzone.width-bar_gradw,0,cornerx+scroller_data.clickzone.width,0);
	cairo_pattern_add_color_stop_rgba(pattern, 0, 0,0,0, inneralpha);
	cairo_pattern_add_color_stop_rgba(pattern, 1, 0,0,0, outeralpha);
	cairo_set_source(cr,pattern);
	gdk_window_invalidate_rectangle(
	if(leafpad 
	if(barh < cornerw*2.5){ //no corners to be drawn, flat;
		cairo_rectangle(cr, cornerx , cornery, barh, scroller_data.clickzone.width);
	}else{
		cairo_move_to(cr, cornerx, cornery+cornerw);
		cairo_arc(cr, cornerx+cornerw, cornery+cornerw, cornerw, M_PI, (M_PI*3)/2);
		cairo_line_to(cr, cornerx+scroller_data.clickzone.width, cornery);
		cairo_line_to(cr, cornerx+scroller_data.clickzone.width, cornery+barh);
		cairo_line_to(cr, cornerx+cornerw, cornery+barh);
		cairo_arc(cr, cornerx+cornerw, cornery+barh-cornerw, cornerw,  M_PI/2, M_PI);
		cairo_close_path(cr);
	}
	cairo_fill(cr);
}

gboolean scroller_notice_clickup(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
	record exactly where this happened
}

gboolean scroller_notice_clickdown(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
	if(leftbtn){
		if(inbounds){
			g_signal_connect(G_OBJECT(widget), "button-release-event",
				G_CALLBACK(scroller_notice_clickup), NULL);
			give mouse position pointer to animation code, activate animation code
			return 1;
		}
	}
	return 0;
}
