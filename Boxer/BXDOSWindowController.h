/* 
 Boxer is copyright 2009 Alun Bestor and contributors.
 Boxer is released under the GNU General Public License 2.0. A full copy of this license can be
 found in this XCode project at Resources/English.lproj/GNU General Public License.txt, or read
 online at [http://www.gnu.org/licenses/gpl-2.0.txt].
 */


//BXDOSWindowController manages a session window and its dependent views and view controllers.
//Besides the usual window-controller responsibilities, it handles switching to and from fullscreen
//and passing frames to the emulator to the rendering view.


#import <Cocoa/Cocoa.h>

@class BXEmulator;
@class BXSession;
@class BXDOSWindow;
@class BXDOSFullScreenWindow;
@class BXProgramPanelController;
@class BXInputController;
@class BXStatusBarController;
@class BXEmulator;
@class BXFrameBuffer;
@class BXInputView;

@protocol BXFrameRenderingView;

//Produced by our rendering view when it begins/ends a live resize operation.
extern NSString * const BXViewWillLiveResizeNotification;
extern NSString * const BXViewDidLiveResizeNotification;

@interface BXDOSWindowController : NSWindowController
{
	IBOutlet NSView <BXFrameRenderingView> *renderingView;
	IBOutlet BXInputView *inputView;
	IBOutlet NSView *viewContainer;
	IBOutlet NSView *statusBar;
	IBOutlet NSView *programPanel;

	IBOutlet BXProgramPanelController *programPanelController;
	IBOutlet BXInputController *inputController;
	IBOutlet BXStatusBarController *statusBarController;
	
	NSSize currentScaledSize;
	NSSize currentScaledResolution;
	BOOL resizingProgrammatically;
}

#pragma mark -
#pragma mark Properties

//Our subsidiary view controllers.
@property (retain) BXProgramPanelController *programPanelController;
@property (retain) BXInputController *inputController;
@property (retain) BXStatusBarController *statusBarController;

@property (retain) NSView <BXFrameRenderingView> *renderingView;	//The view that displays DOSBox's graphical output.
@property (retain) BXInputView *inputView;	//The view that tracks user input. This is also be the view we use for fullscreen.
@property (retain) NSView *viewContainer;	//A wrapper for the input view to aid window-sizing behaviour.
@property (retain) NSView *programPanel;	//The slide-out program picker panel.
@property (retain) NSView *statusBar;		//The status bar at the bottom of the window.

//Indicates that the current resize event is internal and not triggered by user interaction.
//Used to change our window constraining behaviour and response to resize events.
@property (assign) BOOL resizingProgrammatically;

//Returns the size that the rendering view would currently be *if it were in windowed mode.*
//This will differ from the actual render view size if in fullscreen mode.
@property (readonly) NSSize windowedRenderingViewSize;

//Returns YES if the window is in the process of resizing itself.
@property (readonly) BOOL isResizing;

//The chromeless window used in fullscreen mode. Will be nil while in windowed mode.
@property (readonly) BXDOSFullScreenWindow *fullScreenWindow; 

//Sets/gets whether the rendering view is currently fullscreen.
//See also setFullScreenWithZoom:
@property (assign, getter=isFullScreen) BOOL fullScreen;

//The screen to which we will render in fullscreen mode.
//This is currently the screen with the main menu on it.
@property (readonly) NSScreen *fullScreenTarget;

//The maximum BXFrameBuffer size we can render.
@property (readonly) NSSize maxFrameSize;

//The current size of the DOS rendering viewport.
@property (readonly) NSSize viewportSize;


#pragma mark -
#pragma mark Inherited accessor overrides

//Recast NSWindowController's standard accessors so that we get our own classes
//(and don't have to keep recasting them ourselves)
- (BXSession *) document;
- (BXDOSWindow *) window;


#pragma mark -
#pragma mark Renderer-related methods

- (void) updateWithFrame: (BXFrameBuffer *)frame;

//Sets the window to use the specified frame-autosave name, and adjusts the resulting
//frame to ensure the aspect ratio is consistent with what it was before.
- (void) setFrameAutosaveName: (NSString *)savedName;


#pragma mark -
#pragma mark Window-sizing and fullscreen methods

//Zoom in and out of fullscreen mode with a smooth window sizing animation.
- (void) setFullScreenWithZoom: (BOOL)fullScreen;

//Resize the window to fit the specified render size, with an optional smooth resize animation.
- (void) resizeWindowToRenderingViewSize: (NSSize)newSize animate: (BOOL)performAnimation;


#pragma mark -
#pragma mark Drag and drop

//The session window responds to dropped files and folders, mounting them as new DOS drives and/or opening
//them in DOS if appropriate. These methods call corresponding methods on BXSession+BXDragDrop.
- (NSDragOperation)draggingEntered:	(id < NSDraggingInfo >)sender;
- (BOOL)performDragOperation:		(id < NSDraggingInfo >)sender;


#pragma mark -
#pragma mark Interface actions

//Toggle instantly in and out of fullscreen mode.
- (IBAction) toggleFullScreen: (id)sender;

//Zoom in and out of fullscreen mode with a smooth window sizing animation.
//Toggles setFullScreenWithZoom:
- (IBAction) toggleFullScreenWithZoom: (id)sender;

//Exit back to a window, if in fullscreen; otherwise do nothing.
//This is triggered by pressing ESC when at the DOS prompt.
- (IBAction) exitFullScreen: (id)sender;

//Toggle the status bar and program panel components on and off.
- (IBAction) toggleStatusBarShown:		(id)sender;
- (IBAction) toggleProgramPanelShown:	(id)sender;

//Unconditionally show/hide the program panel.
- (void) showProgramPanel;
- (void) hideProgramPanel;


//Toggle the emulator's active rendering filter.
- (IBAction) toggleFilterType: (id)sender;


#pragma mark -
#pragma mark Toggling UI components

//Get/set whether the statusbar should be shown.
- (BOOL) statusBarShown;
- (void) setStatusBarShown:		(BOOL)show;

//Get/set whether the program panel should be shown.
- (BOOL) programPanelShown;
- (void) setProgramPanelShown:	(BOOL)show;


#pragma mark -
#pragma mark Handling window and UI events

//These tell the emulator to pause itself while a resize is in progress, and clean up when it finishes.
- (void) renderingViewDidResize: (NSNotification *) notification;
- (void) renderingViewWillLiveResize: (NSNotification *) notification;
- (void) renderingViewDidLiveResize: (NSNotification *) notification;

//These listen for any time an NSMenu opens or closes, and warn the active emulator
//to pause or resume emulation. In practice this means muting it to avoid hanging
//music and sound effects while the menu is blocking the thread.
//TODO: BXEmulator itself should handle this at a lower level by watching out for
//whenever a new event loop gets created.
- (void) menuDidOpen:	(NSNotification *) notification;
- (void) menuDidClose:	(NSNotification *) notification;

@end