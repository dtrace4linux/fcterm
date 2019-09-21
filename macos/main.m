# include       <Foundation/Foundation.h>
# include       <AppKit/AppKit.h>

# include	<fcterm.h>
//# include	<build.h>

/**********************************************************************/
/*   Pointer to the current display.				      */
/**********************************************************************/
fcterm_t	*cur_ctw;

/**********************************************************************/
/*   Class interfaces.						      */
/**********************************************************************/
@interface CTWClass : NSApplication
{
}
- (id) init;
- (void) applicationDidFinishLaunching: (NSNotification *) n;
- (NSMenu *) applicationDockMenu: (NSApplication *) sender;
@end

@implementation CTWClass : NSApplication
- (id) init
{
	[super init];
//	create_menu(self);
	[self setDelegate:self];

	return self;
}
- (void) applicationDidBecomeActive: (NSNotification *) n
{
	printf("applicationDidBecomeActive\n");
}
- (void) applicationDidFinishLaunching: (NSNotification *) n
{
	printf("applicationDidFinishLaunching\n");
//	do_drawing(v, 2);
}
- (NSMenu *) applicationDockMenu: (NSApplication *) sender
{
	printf("%s\n", __func__);
	return NULL;
}
- (void) applicationWillFinishLaunching: (NSNotification *) n
{
	printf("%s\n", __func__);
}
@end

/**********************************************************************/
/*   MyWindow.							      */
/**********************************************************************/
@interface MyWindow : NSWindow
{
}
- (id) init;
- (BOOL) canBecomeKeyWindow;
- (void) becomeMainWindow;
@end

@implementation MyWindow : NSWindow
- (id) init
{
	[super init];
	[self setDelegate: self];
	return self;
}
- (void) becomeMainWindow
{
	[super becomeMainWindow];
	printf("becomeMainWindow\n");
}
- (BOOL) canBecomeKeyWindow
{
	printf("canBecomeKeyWindow\n");
	return YES;
}
- (BOOL) canBecomeMainWindow
{
	printf("canBecomeMainWindow\n");
	return YES;
}
@end

/**********************************************************************/
/*   Main.							      */
/**********************************************************************/
int
mac_main(int argc, char **argv)
{	NSAutoreleasePool *pool;
	NSRect rect;
	NSWindow *win;

	pool = [NSAutoreleasePool new];

	[CTWClass sharedApplication];

	rect.origin.x = 50;
	rect.origin.y = 700;
	rect.size.width = 700;
	rect.size.height = 400;
	win = [[MyWindow alloc] initWithContentRect: rect
			styleMask: (NSResizableWindowMask | NSMiniaturizableWindowMask | NSClosableWindowMask)
			backing: NSBackingStoreBuffered
			defer: TRUE];
	[win orderFront:win];
//	[win center];
	[win makeKeyWindow];
	[win makeMainWindow];
	
	cur_ctw = new_fcterm(0);

	[NSApp run];
}
