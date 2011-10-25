#include <math.h>

const unsigned scroller_animationlength=2;
typedef struct{
	gboolean textbox_hasupdated;
	gboolean scroller_isleft;
}ScrollerAnimationData;

gboolean scroller_draw(GtkWidget *widget, GdkEventExpose *event, gpointer scrolled_window); //warning, interface doesn't update until this does;
gboolean scroller_notice_clickdown(GtkWidget* widget, GdkEventButton* event, gpointer user_data);
gboolean scroller_notice_clickup(GtkWidget* widget, GdkEventButton* event, gpointer user_data);

