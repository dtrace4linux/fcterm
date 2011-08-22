/**********************************************************************/
/*   Useful routines for Motif programs.			      */
/**********************************************************************/
# include	"motif.h"

/**********************************************************************/
/*   Create a button in the menu bar and the menu items.	      */
/**********************************************************************/
Widget
create_menu_button(name, callback, tbl, menu_bar)
char	*name;
void	(*callback)();
struct menu *tbl;
Widget	menu_bar;
{	Widget	w, w1;
	int	n;
	Arg	args[20];
	void	menu_callback();

	w = (Widget) XmCreateCascadeButton(menu_bar, name, NULL, 0);
	XtManageChild(w);

	n = 0;
	XtSetArg(args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
	w1 = (Widget) XmCreatePulldownMenu(w, name, args, n);

	n = 0;
	XtSetArg(args[n], XmNsubMenuId, w1); n++;
	XtSetValues(w, args, n);
	if (callback != NULL)
		XtAddCallback(w, XmNcascadingCallback, callback, NULL);
	create_menu(w1, tbl, menu_callback);
	return w;
}
/**********************************************************************/
/*   Function to create a menu.					      */
/**********************************************************************/
Widget
create_menu(parent, menu, callback)
Widget	parent;
struct menu	*menu;
void	(*callback)();
{	int	n;
	Arg	args[20];
	Widget	w1;

	for ( ; (menu->flags & F_END) == 0; menu++) {
		/***********************************************/
		/*   Create  a  separator line if the flag is  */
		/*   set.				       */
		/***********************************************/
		if (menu->flags & F_SEP) {
			menu->widget = XtCreateManagedWidget("sep",
				xmSeparatorWidgetClass,
				parent,
				NULL, 0);
			continue;
			}
		/***********************************************/
		/*   Try for a glyph (pixmap) menu item.       */
		/***********************************************/
		if (menu->flags & F_GLYPH) {
			Pixmap	pixmap;
			pixmap = XCreatePixmapFromBitmapData(
				XtDisplay(parent), DefaultRootWindow(XtDisplay(parent)),
				menu->bits, menu->width, menu->height,
				menu->pixel, 0,
				DefaultDepth(XtDisplay(parent), 0));
			n = 0;
			XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
			XtSetArg(args[n], XmNlabelPixmap, pixmap); n++;
			menu->widget = XtCreateManagedWidget(menu->widget_name,
				xmPushButtonWidgetClass,
				parent, args, n);
			XtAddCallback(menu->widget, XmNactivateCallback, 
				callback, NULL);
			continue;
			}
		n = 0;
		XtSetArg(args[n], XmNlabelString, 
			XmStringCreateLtoR(menu->label, XmSTRING_DEFAULT_CHARSET)); n++;
		/***********************************************/
		/*   If  we  have  a  cascaded  menu  then we  */
		/*   need  to  create the cascade menu before  */
		/*   finishing this entry.		       */
		/***********************************************/
		if (menu->sub_menu != (struct menu *) NULL) {
			menu->widget  = XmCreateCascadeButton(parent, menu->widget_name, args, n);
			w1 = (Widget) XmCreatePulldownMenu(menu->widget, 
				"pulldown", NULL, 0);
			n = 0;
			XtSetArg(args[n], XmNsubMenuId, w1); n++;
			XtSetValues(menu->widget, args, n);
			create_menu(w1, menu->sub_menu, callback);
			XtManageChild(menu->widget);
			}
		else {
			menu->widget = XtCreateManagedWidget(menu->widget_name,
				xmPushButtonWidgetClass,
				parent, args, n);
			XtAddCallback(menu->widget, XmNactivateCallback, 
				callback, NULL);
			}
		}
	return NULL;
}
/**********************************************************************/
/*   Search  a  menu  to  see if we can match a widget passed into a  */
/*   callback routine to the actual menu entry.			      */
/**********************************************************************/
int
find_menu_widget(tbl, widget)
struct menu *tbl;
Widget	widget;
{	int	i = 0;

	while ((tbl->flags & F_END) == 0) {
		if (tbl->widget == widget)
			return i;
		i++;
		tbl++;
		}
	return -1;
}
/**********************************************************************/
/*   Convert  a  widget name to a widget pointer by searching a menu  */
/*   table.							      */
/**********************************************************************/
Widget
map_name_to_widget(tbl, name)
struct menu *tbl;
char	*name;
{
	for (; (tbl->flags & F_END) == 0; tbl++)
		if (tbl->widget_name != (char *) NULL &&
		    strcmp(tbl->widget_name, name) == 0)
			return tbl->widget;
	return (Widget) NULL;
}
