#include "Board.h"
#include "DemoWidget.h"
#include "Res.h"
#include "V12DemoApp.h"
#include "..\SexyAppFramework\Font.h"
#include "..\SexyAppFramework\Graphics.h"
#include "..\SexyAppFramework\ButtonWidget.h"
#include "..\SexyAppFramework\WidgetManager.h"
#include "..\SexyAppFramework\Dialog.h"
#include "..\SexyAppFramework\Flags.h"
#include "..\SexyAppFramework\SexyMatrix.h"
#include "..\SexyAppFramework\trivertex.h"

using namespace Sexy;

Board::Board(V12DemoApp* theApp)
{
	mApp = theApp;


	// Remeber the flag explanation in the 1.2 doc and V12Demo.cpp? Sure you do.
	// This is basically another use of it. All widgets by default have the following
	// flags set:
	//	WIDGETFLAGS_UPDATE		- The widget gets its Update and UpdateF methods called
	//	WIDGETFLAGS_DRAW		- The widget is allowed to draw
	//	WIDGETFLAGS_CLIP		- Set to clip Graphics passed into Draw to the widget's bounds
	//	WIDGETFLAGS_ALLOW_MOUSE	- Set to allow mouse interaction
	//	WIDGETFLAGS_ALLOW_FOCUS	- Set to allow focus to be maintained 
	//
	//	Previously, if you wanted to mark the widget dirty every frame (and thus draw every
	//	frame), you had to call MarkDirty() all the time. Instead of doing that, we can just
	//	use the flag system. By ORing WIDGETFLAGS_MARK_DIRTY with mWidgetFlagsMod.mAddFlags,
	//	when we are added to the widget manager, the manager will then know that we want to
	//	be marked dirty every frame automatically, instead of manually calling MarkDirty().
	//	Yes, I know, that's not a huge deal, but as you can see from the other flags,
	//	it's now easy to set/unset various flags for various widgets and..various reasons.
	//	If you wanted to remove flags set by default for this widget, you'd just OR the
	//	flags to remove with mWidgetFlagsMod.mRemoveFlags. You can affect ALL widgets if you
	//	want to set up widget defaults, by modifying mApp->mWidgetManager's mWidgetFlags
	//	structure in a similar way.

	mWidgetFlagsMod.mAddFlags |= WIDGETFLAGS_MARK_DIRTY;

	// Previously, it was annoying trying to place widgets on some sort of parent widget, 
	// since there was no notion of parent/child relationship. What you had to do was
	// override the AddedToManager and RemovedFromManager functions, create and add your
	// widgets or remove and nuke them, and in the case of AddedToManager, you also had
	// to then place the widget using global coordinates that had no relation to the coordinates
	// you set the parent widget at. Not anymore. What we can do now is to dispense with
	// overriding those methods altogether! So now, in the parent's constructor, we can
	// create our child widgets, resize them, and place them using relative coordinates.
	// What this also means is that moving the parent widget around results in moving
	// ALL the child widgets, which means you no longer have to manually move everything
	// just because the parent moved. In addition, another nice thing is that you no longer
	// have to remove your child widgets before deleting them: this is handled automatically
	// for you.
	mDemoButton = new ButtonWidget(0, this);
	mDemoButton->mLabel = _S("Demo Widget");
	mDemoButton->SetFont(FONT_DEFAULT);
	mDemoButton->Resize(10, 10, 10 + FONT_DEFAULT->StringWidth(mDemoButton->mLabel), 50);

	// VERY IMPORTANT: Notice that we're calling THIS CLASS' (or really, it's parent, WidgetContainer's)
	// AddWidget method instead of the WidgetManager's method. In order to designate a widget as a child
	// widget, you have to call the AddWidget method of the class that will be its parent.
	AddWidget(mDemoButton);

	mDialogButton = new ButtonWidget(1, this);
	mDialogButton->mLabel = _S("Do Dialog");
	mDialogButton->SetFont(FONT_DEFAULT);
	int w = FONT_DEFAULT->StringWidth(mDialogButton->mLabel);
	mDialogButton->Resize(mApp->mWidth - 20 - w, 10, w + 10, 50);
	AddWidget(mDialogButton);

	mCurtainButton = new ButtonWidget(2, this);
	mCurtainButton->mLabel = _S("Do Transition");
	mCurtainButton->SetFont(FONT_DEFAULT);
	w = FONT_DEFAULT->StringWidth(mCurtainButton->mLabel);
	mCurtainButton->Resize(mDemoButton->mX + 20 + mDemoButton->mWidth, 10, w + 10, 50);
	AddWidget(mCurtainButton);

	mDemoWidget = NULL;

	// Position and set up our pulsing rectangle, as well as the location of the lost focus text.
	mRect = Rect(mApp->mWidth / 2 - 1, mApp->mHeight / 2 - 1, 2, 2);
	mExpanding = true;
	mMsgX = Rand() % (mApp->mWidth - 100);
	mMsgY = Rand() % (mApp->mHeight - 100);
	mLostFocus = false;

	mCurtainWidth = 0;
	mCurtainMode = CURTAIN_INACTIVE;

	// We'll toggle this between 0 and 2 every second when we are unfocussed
	mDeferPriority = 0;
}

Board::~Board()
{
	// We need to remove child widgets before deleting them.
	RemoveAllWidgets();

	delete mDemoButton;
	delete mDialogButton;
	delete mCurtainButton;

	if (mDemoWidget != NULL)
		mApp->mWidgetManager->RemoveWidget(mDemoWidget);

	delete mDemoWidget;
}

void Board::Update()
{
	Widget::Update();

	if (mLostFocus)
	{
		if (mUpdateCnt % 100 == 0)
		{
			mMsgX = Rand() % (mApp->mWidth - 100);
			mMsgY = Rand() % (mApp->mHeight - 100);

			// Every second, we switch between drawing the overlay above or below
			// the mDemoWidget object (assuming of course you have the widget on screen
			//	when you lose focus).
			mDeferPriority = mDeferPriority == 0 ? 2 : 0;
		}
	}
	else
	{		
		// This just makes the rectangle in the middle grow/shrink. Nothing
		// too crazy.
		if (mExpanding)
		{
			mRect.mWidth += 2;
			mRect.mHeight += 2;

			if (mRect.mWidth >= mApp->mWidth)
			{
				mRect.mWidth = mApp->mWidth;
				mExpanding = false;
			}

			if (mRect.mHeight >= mApp->mHeight)
			{
				mRect.mHeight = mApp->mHeight;
				mExpanding = false;
			}
		}
		else
		{
			mRect.mWidth -= 2;
			mRect.mHeight -= 2;

			if (mRect.mHeight <= 0)
			{
				mRect.mHeight = 0;
				mExpanding = true;
			}

			if (mRect.mWidth <= 0)
			{
				mRect.mWidth = 0;
				mExpanding = true;
			}

		}

		mRect.mX = mApp->mWidth / 2 - mRect.mWidth / 2;
		mRect.mY = mApp->mHeight / 2 - mRect.mHeight / 2;
	}
}

void Board::Draw(Graphics* g)
{
	g->SetColor(Color::Black);
	g->FillRect(0, 0, mWidth, mHeight);

	// Draw our pulsing rectangle in the middle of the screen.
	g->SetColor(Color(255, 0, 0));
	g->FillRect(mRect);

	if (!mLostFocus)
	{
		// Previously, any time you changed the
		// graphic's state, you had to undo it, otherwise it affected anything
		// drawn later. Common examples are SetColorizeImages, SetColor, SetDrawMode, etc.
		// In Board::DrawOverlay, you'll see that we can use PushState and PopState to on-demand
		// save/restore the graphics state. However, if we're either lazy, or after we draw
		// our current stuff we don't have a need to reset the state and change it again, we
		// can use the GraphicsAutoState object. This nifty helper class, upon instantiation on 
		// the stack, will automatically push the graphics state of the graphics object you pass in.
		// Note that since stack variables are
		// removed (and have their con/destructors called automatically, for classes/structs) when they
		// go out of scope, the destructor for a GraphicsAutoState object takes care of popping the graphics
		// state and restoring things to their previous..uh...state.
		// NOTE: Graphics state is saved between widgets.
		GraphicsAutoState auto_state(g);
		g->DrawImage(IMAGE_HUNGARR_LOGO, 10, 100);
		g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
		g->SetColorizeImages(true);
		g->SetColor(Color(mUpdateCnt % 128, mUpdateCnt % 255, mUpdateCnt % 64));
		g->DrawImage(IMAGE_HUNGARR_LOGO, 10, 100);
	}

	if (mCurtainMode != CURTAIN_INACTIVE)
	{
		g->SetColor(Color(255, 255, 0));
		g->FillRect(0, 0, mCurtainWidth, mHeight);
		g->FillRect(mWidth - mCurtainWidth, 0, mCurtainWidth, mHeight);
	}

	// Instead of using an overlay widget to draw our stuff when focus is lost,
	// we can just use DeferOverlay. You can ONLY call this in a Draw routine. What happens
	// when you call DeferOverlay is that it schedules with the WidgetManager a call to
	// this widget's DrawOverlay method at a later time. You can control the order of the 
	// overlay layer, and place it above or below other widgets by optionally specifying
	// a defer priority in the call to DeferOverlay. The higher the priority, the more toplevel
	// and thus the more widgets above it will be drawn. You'll notice that in DemoWidget's constructor,
	// that we set its priority to 1. Thus, if we DeferOverlay with a value of 0, the overlay will be
	// drawn BELOW the demo widget, but still above all our child widgets and ourself. When mDeferPriority
	// is 2, it will be drawn above ALL widgets, including the demo widget, as its priority is only 1, which
	// last time I checked, is less than 2. Note that widgets by default have priority 0 and dialogs have priority 1.
	if (mLostFocus)
		DeferOverlay(mDeferPriority);

}

void Board::DrawOverlay(Graphics* g)
{
	// Make sure you've read through Board::Draw before this guy, as it explains
	// how and why we get here.
	g->SetColor(Color(0, 0, 255, 175));
	g->FillRect(0, 0, mWidth, mHeight);

	g->SetFont(FONT_DEFAULT);
	g->SetColor(Color::White);
	g->DrawString(_S("LOST FOCUS"), mMsgX, mMsgY);	

	// PushState is a new addition. Previously, any time you changed the
	// graphic's state, you had to undo it, otherwise it affected anything
	// drawn later. Common examples are SetColorizeImages, SetColor, SetDrawMode, etc.
	// With PushState, you save the entire previous state of things. Then, you can make
	// whatever changes you like, without needing to undo them, as a resulting call to
	// PopState restores things back to normal. Note that you can push/pop as much as you want.
	g->PushState();

	// No need to turn this off! When we PopState the previous, non-colorized state,
	// will return.
	g->SetColorizeImages(true);
	g->SetColor(Color(0, 255, 255));
	
	// MORE NEW STUFF? Yes. Think back to the previous framework version:
	// how would you draw in real-time, a rotated and scaled image at a given
	// location? Well, you could mess around with destination Rects for scaling,
	// but you would have a hard time scaling and rotating all at once. You could
	// create images in memory to hold the scaled or rotated versions, but that's annoying.
	// BEHOLD: DrawImageTransform/F. All you have to do is create a Transform object,
	// and apply various transformations to it, then call DrawImageTransform/F.
	// Let's take a look:
	Transform t;

	// Let's rotate the image between 0 and 360 degrees (note that you could also
	// use the radian version of the function). So far, not hard, right?
	t.RotateDeg((float)(mUpdateCnt % 360));
	
	float sw = 1.0f;
	float sh = 1.0f;
	
	// Don't get scared here. This is just a little trickery to expand/contract
	// the image depending on the update count. All it does is expand the image
	// to normal size for a second, and then shrink it to almost invisible for
	// another second, which makes that pulse effect happen.
	int mod = mUpdateCnt % 200;
	if (mod < 100)
	{
		sw = (float)(mod + 1) / 100.0f;
		sh = (float)(mod + 1) / 100.0f;
	}
	else
	{
		mod = 200 - mod;
		sw = (float)mod / 100.0f;
		sh = (float)mod / 100.0f;
	}

	// And now we just tell the transform object, t, that we want to scale. 
	// The value for the x and y direction should be a %, so 1.0 means no change in scale.
	// Let's also mirror and flip the image at the same time too. To mirror and flip, we'll
	// just multiply sw and sh by -1. Of course, it'll be hard to tell that it's flipped and mirrored
	// since it's rotating at the same time, so if you don't believe me you can try commenting
	// out the t.RotateDeg(...) call above and you'll see that it works.
	t.Scale(-sw, -sh);
	
	// And now we just pass in our transform object and it works in both 2D and 3D! Note that we can also
	// draw the image at a given XY as well. We'll make it move rightward depending on the update count.
	// ******************IMPORTANT NOTE:*********************
	// DrawImageTransform/F and DrawImageMatrix use the CENTER of the image rather than the top left
	// for XY drawing. For images of even size, the non-F form of the function will just truncate
	// the the center, but for the F forms, they will be offset by 0.5f since floating point is used.
	if (gSexyAppBase->Is3DAccelerated())
		g->DrawImageTransformF(IMAGE_HUNGARR_LOGO, t, (float)(mUpdateCnt % (mApp->mWidth + 340)), 200.0f);
	else
		g->DrawImageTransform(IMAGE_HUNGARR_LOGO, t, (float)(mUpdateCnt % (mApp->mWidth + 340)), 200.0f);
	
	g->PopState();

	// You can now draw using matrices. Why the heck would you want to use matrices? Besides doing some
	// crazy stuff, you can also do some cool little tricks. For example, previously it used be a pain
	// to in real-time flip and mirror an image. While I won't give a tutorial on matrix algebra
	// (that would take a loooooong time), I'll explain the essential parts:
	SexyTransform2D matrix;


	// Multiplying the X coordinate by -1 (which is the 0, 0 element of the matrix) will result in our
	// image being mirrored, while multiplying the Y coordinate by -1 (1, 1 in the matrix) will result in our
	// image being flipped.
	matrix.m[0][0] *= -1;
	matrix.m[1][1] *= -1;

	// Let's also shear it. While mirroring/flipping could easily be done with the DrawImageTransform methods,
	// doing more complex matrix manipulation (like shearing) can only be done with the DrawImageMatrix function.
	matrix.m[0][1] = 2;

	// And then we just make a call to DrawImageMatrix and give it our specified XY coordinates as well,
	// and that's it! This works in both 2D and 3D modes.
	g->DrawImageMatrix(IMAGE_HUNGARR_LOGO, matrix, 300, 400);	

	// IMPORTANT COMPARISON NOTE:
	// DrawImageTransform/F will try to use the faster drawing methods if it recognizes certain
	// common transforms, like rotating and scaling. This only works though if you are using
	// either one operation, or multiple operations of the same type (i.e. you only used scale
	// or rotate with the Transform object, or used the same one multiple times). If you mix
	// operations, like we do above, the DrawImageTransform method will actually use DrawImageMatrix.
	// With DrawImageTransform, you can't directly modify the underlying matrix, so if that's something
	// you need to do, then DrawImageMatrix is a better option.
}

void Board::ButtonDepress(int id)
{
	if (id == mDemoButton->mId)
	{
		delete mDemoWidget;
		mDemoWidget = new DemoWidget();
		mApp->mWidgetManager->AddWidget(mDemoWidget);
		
		// What, more flags? Yup. Since our little DemoWidget isn't a dialog, when we add it,
		// it won't change anything about the widgets drawn below it. Which means, unmodified, 
		// mouse clicks could still be passed down to the board (if the click wasn't in the DemoWidget),
		// the board still updates, still draws, etc. Let's turn off mouse clicks for all widgets below the
		// DemoWidget. But, let's still allow all widgets below it to update. Note that if we used the form
		// of the method that only takes one parameter, then it would use mDefaultBelowModalFlagsMod
		// which we modified in our app. By passing in our own flags though, they're used instead.
		// Which means the only flag we need to remove from the widgets below it is the allow mouse flag.
		// We do that by making a temp FlagsMod object, and setting
		// its mRemoveFlags variable to be ORed with WIDGETFLAGS_ALLOW_MOUSE. Upon calling
		// the WidgetManager's AddBaseModal method, we then pass in the DemoWidget, and the above flags.
		// AddBaseModal will then treat this new widget as a modal object, applying any flags we passed in.
		FlagsMod flags;
		flags.mRemoveFlags |= WIDGETFLAGS_ALLOW_MOUSE;
		mApp->mWidgetManager->AddBaseModal(mDemoWidget, flags);
	}
	else if (id == mDialogButton->mId)
	{
		// With the new 1.2 changes, you can create dialogs very easily. Previously, you had to supply
		// images in order to skin the dialog and be able to see it in the first place. You also had to
		// override the SexyAppBase::NewDialog method, otherwise calls to SexyAppBase::DoDialog wouldn't
		// do anything. Not so anymore. You can rapidly prototype with dialogs even without images.
		// A default NewDialog implementation now exists, and if a dialog doesn't have an image, it will
		// be drawn using colored rectangles. So now, to make a little dialog box appear, it's as simple as
		// the single line of code below. No messy functions to write, no images to create, that's a hassel
		// when you're starting a new app and don't care about the initial appearance of your UI elements and
		// just want to start testing gameplay immediately.
		Dialog* d = mApp->DoDialog(100, true, _S("Fun Dialog"), _S("Line 1\nLine 2\nLine 3"), _S("Close!"), Dialog::BUTTONS_FOOTER);

		// Using the default font, which is a system font, can sometimes cause problems on older OS's, like
		// Windows 95 or 98, in which printing with it appears to produce blank results. Let's set the font
		// for the button on the dialog box to be FONT_DEFAULT. 
		d->SetButtonFont(FONT_DEFAULT);
	}
	else if (id == mCurtainButton->mId)
	{
		mCurtainMode = CURTAIN_CLOSING;
		mCurtainWidth = 0;

		// Here's some more new stuff. Previously, if you wanted to do something like
		// a transition after a button is pressed (for example), you had to set a bunch of
		// variables in your class, check them, and process/draw differently depending on 
		// the transition state. That can be both messy and annoying to do. Why not just
		// do the logic for the transition right at the site where it has to trigger, in this
		// case, right when the button is pressed? If you ignore the UpdateApp() call below for 
		// a minute, what the loop looks like is just a simple loop that makes a variable expand
		// to 1/2 the app width, and then contract back to 0. If you removed the call to UpdateApp,
		// obviously the program would be stuck in the loop until it completed, making it look like the
		// whole thing is frozen, and not updating the display. By putting UpdateApp in the while loop,
		// you enable ALL the other widgets to update, and thus draw, which in turn means we can see
		// our little curtain transition effect working as expected. You could even use this method
		// to block while waiting for a dialog/widget to return, but to still allow other widgets
		// to update/draw in the process. ONE VERY IMPORTANT THING TO NOTE THOUGH:
		// If you have code like this in your Update methods, be careful, as you can run into some
		// reentrancy problems. If you place this sort of code in your Update methods, make sure you
		// protect it (you could just use a simple bool that's true if doing a transition, otherwise false).
		// Why? Think about it like this: UpdateApp updates ALL widgets, including the one in which the
		// while loop exists. Thus, when going back into Update for said widget, it will once again get
		// stuck in the same while loop, which will effectively prevent any drawing from occurring.
		// Just remember that UpdateApp will update EVERY widget, and so you will want to avoid having
		// your while loop called more often than it should. Of course, an easy way to avoid all that
		// is to have your transition code happen elsewhere that isn't updated every frame, like a
		// button down method.
		//
		// What you'll notice is that if the game lost focus, the curtain call still updates, although
		// the shrinking/expanding rectangle in the middle does not. That is because when focus is lost,
		// we don't update that part of the app, but this while loop is still running, which means the
		// curtain effect is still running too.
		while ((mCurtainMode != CURTAIN_INACTIVE) && mApp->UpdateApp())
		{
			if (mCurtainMode == CURTAIN_CLOSING)
			{
				mCurtainWidth += 4;
				if (mCurtainWidth >= mWidth / 2)
				{
					mCurtainWidth = mWidth / 2;
					mCurtainMode = CURTAIN_OPENING;
				}
			}
			else
			{
				mCurtainWidth -= 4;
				if (mCurtainWidth <= 0)
				{
					mCurtainWidth = 0;
					mCurtainMode = CURTAIN_INACTIVE;
				}
			}

		}

	}
}
