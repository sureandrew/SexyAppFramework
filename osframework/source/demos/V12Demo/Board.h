#ifndef __BOARD_H__
#define __BOARD_H__

#include "..\SexyAppFramework\Widget.h"
#include "..\SexyAppFramework\ButtonListener.h"
#include "..\SexyAppFramework\Rect.h"

namespace Sexy
{

class V12DemoApp;
class DemoWidget;
class ButtonWidget;

class Board : public Widget, public ButtonListener
{

public:

	V12DemoApp* mApp;

private:

	// We're going to do a curtain open/close effect when the mCurtainButton button is pressed.
	enum
	{
		CURTAIN_CLOSING,
		CURTAIN_OPENING,
		CURTAIN_INACTIVE
	};

	DemoWidget*		mDemoWidget;		
	ButtonWidget*	mDemoButton;		// Creates our DemoWidget
	ButtonWidget*	mDialogButton;		// Creates a little dialog box
	ButtonWidget*	mCurtainButton;		// Initiates the curtain closing/opening transition
	Rect			mRect;				// A rectangle that gets larger/smaller over time
	
	bool			mExpanding;			// Is the above (mRect) rectangle shrinking or growing?
	bool			mLostFocus;			// If true, the app has lost focus, so we'll draw our overlay
	
	int				mCurtainMode;		// One of the above enums
	int				mCurtainWidth;		// How wide the curtain is. This ranges from 0 to app width / 2.
	int				mDeferPriority;		// See the .cpp for full info, but we'll be toggling the overlay priority every second
	int				mMsgX, mMsgY;		// For no valid reason, we'll move the "LOST FOCUS" text every second.

public:

	Board(V12DemoApp* theApp);
	virtual ~Board();

	void		Update();
	void		Draw(Graphics* g);
	void		ButtonDepress(int id);

	//////////////////////////////////////////////////////////////////////////
	//	Function: DrawOverlay
	//	Parameters: g - The graphics object to draw with
	//
	//	Purpose: Previously, you had to make a separate OverlayWidget object
	//	if you wanted to do a full screen overlay for say a paused state.
	//	That was annoying, since you had to deal with the messiness of ensuring
	//	that it covered all the appropriate widgets, and had to take care when
	//	removing it. Plus, it was a waste to have to make such a minor object
	//	just to draw a layer above other widgets. Enter the DrawOverlay method:
	//	In your main Draw(...) method, whenever you want the overlay to be drawn,
	//	you just make a call to DeferOverlay. You can pass in a priority that indicates
	//	when the overlay is drawn: the default is 0, but the higher the priority, the
	//	more toplevel (and thus the higher above the other widgets) it will be drawn at.
	//	This can be easily used to draw layers above ALL widgets, or to draw layers above
	//	most widgets but still below some sort of other widget, like an options dialog, etc.
	//	Widgets also have an mPriority variable (defaults to 0), which, setting it higher, will
	//	of course affect at which order in the drawing routine it runs at.
	//
	//	NOTE: This can be used for anything, not just the pause state. You could
	//	easily have an effect that transitions from one level to the next, and
	//	it could be drawn above all the buttons/widgets on screen, rather than
	//	using some sort of overlay widget as well.
	//////////////////////////////////////////////////////////////////////////	
	void		DrawOverlay(Graphics* g);
	

	void		SetFocusLost(bool f) {mLostFocus = f; mDeferPriority = 2;}
};

}

#endif //__BOARD_H__
