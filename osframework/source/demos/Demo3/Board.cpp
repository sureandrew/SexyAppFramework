#include "Board.h"
#include "GameApp.h"
#include "SexyAppFramework/Graphics.h"

// See the Draw method for more information on using the Color class.
#include "SexyAppFramework/Color.h"

// The Image.h file just declares basic functions. All images are either of 
// the DDImage or MemoryImage type. For this demo, we will use DDImage
// types, as they are the type returned by the image loading code.
// A DDImage is actually derived from MemoryImage, so where an Image or
// MemoryImage is required, a DDImage will suffice as well. A DDImage
// contains optimized code for use with DirectX 7+.
#include "SexyAppFramework/DDImage.h"

// The Rectangle template, used to specify X, Y, Width, Height
#include "SexyAppFramework/Rect.h"

// We're going to be making a button in this demo so we need to
// include this file.
#include "SexyAppFramework/ButtonWidget.h"

// We're going to add our own button widget, which requires knowing about the
// WidgetManager.
#include "SexyAppFramework/WidgetManager.h"

#include "SexyAppFramework/ImageFont.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::Board(GameApp* theApp)
{
	mApp = theApp;

	// Start off drawing the first frame of mLightningImg
	mAnimFrame = 0;

	mButton = NULL;
	
	mMouseX = mMouseY = 0;
	mLeftDown = mRightDown = mMiddleDown = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::~Board()
{
	delete mButton;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Update()
{
	// Let the parent class update as well. This will increment
	// the variable mUpdateCnt which is an integer that indicates 
	// how many times the Update() method has been called. Since our
	// Board class is updated 100 times per second, this variable will
	// increment 100 times per second. As you will see in later demos,
	// we will use this variable for animation since its value represents
	// hundredths of a second, which is for almost all games a good
	// enough timer value and doesn't rely on the system clock function
	// call.
	Widget::Update();

	// Let's update our animation frame every 5 update ticks. This
	// is equivalent to 20 times per second.
	if (mUpdateCnt % 5 == 0)
	{
		// In GameApp we specified how many rows and columns this image
		// had. Thus, if our current frame of animation exceeds the number
		// of frames that we have, we should reset the animation frame back
		// to 0.
		if (++mAnimFrame >= mApp->mLightningImg->mNumRows)
			mAnimFrame = 0;
	}


	
	// For this and most of the other demos, you will see the function
	// below called every Update() call. MarkDirty() tells the widget
	// manager that something has changed graphically in the widget and
	// that it needs to be repainted. All widgets follow this convention.
	//		In general, if you don't need
	// to update your drawing every time you call the Update method
	// (the most common case is when the game is paused) you should
	// NOT mark dirty. Why? If you aren't marking dirty every frame,
	// then you aren't drawing every frame and thus you use less CPU
	// time. Because people like to multitask, or they may be on a laptop
	// with limited battery life, using less CPU time lets people do
	// other things besides play your game. Of course, everyone
	// will want to play your game at all times, but it's good to be
	// nice to those rare people that might want to read email or
	// do other things at the same time. 
	//		In this particular demo, we
	// won't be nice, as the purpose is to bring you up to speed as
	// quickly as possible, and so we'll dispense with optimizations
	// for now, so you can concentrate on other core issues first.
	//		In general, this is the last method called in the Update
	// function, but that is not necessary. In fact, the MarkDirty
	// function can be called anywhere, in any method (although
	// calling it in the Draw method doesn't make sense since it is
	// already drawing) and even multiple times. Calling it multiple
	// times does not do anything: only the first call makes a difference.
	MarkDirty();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Draw(Graphics* g)
{
	// The Graphics object, "g", is 
	// automatically created and passed to this method by the 
	// WidgetManager and can be thought of as the main screen 
	// bitmap/canvas upon which all drawing will be done. This object
	// is double buffered automatically so you don't need to worry
	// about those details. All you need to do is instruct the object
	// that you would like to draw something to it, and when the
	// WidgetManager gets done letting all widgets draw to the
	// Graphics object, it will then blit everything to the screen
	// at once. 

	// First, let's start by clearing the screen to black. 
	// As you'll recall from Demo1, we set the color with SetColor
	// and pass in a Color object that contains either 3 or 4 parameters,
	// that represent the r,g,b,a values (alpha is 255 if only 3 specified).
	g->SetColor(Color(0, 0, 0));
	g->FillRect(0, 0, mWidth, mHeight);

	// Now let's try drawing a stretched image. We'll draw the original image
	// stretched to twice its size. Drawing a stretched image is exactly like
	// drawing a normal image, except that you have two extra parameters:
	// the stretched width and height. You can use this to draw a shrunk version
	// of the image as well (which we'll do second)	
	g->DrawImage(mApp->mTurbotImg, 0, 0, mApp->mTurbotImg->GetWidth() * 2, mApp->mTurbotImg->GetHeight() * 2);
	g->DrawImage(mApp->mTurbotImg, 0, 275, mApp->mTurbotImg->GetWidth() / 2, mApp->mTurbotImg->GetHeight() / 2);

	// The default stretching algorithm (the one used in the two examples above) uses
	// the slow stretching method. The slow stretching method anti-aliases the resulting
	// image to give a smoother, less pixelated look. While this tends to look a lot
	// nicer, it also requires more processing power. Thus, if you either don't need
	// or don't care about the anti-aliasing, you can instruct the graphics object
	// to use fast stretching. Just remember: unless you turn fast stretching off when
	// done, it will remain on. Let's give it a try by drawing the same image 
	// stretched twice as large using fast stretching.
	g->SetFastStretch(true);
	g->DrawImage(mApp->mTurbotImg, 275, 0, mApp->mTurbotImg->GetWidth() * 2, mApp->mTurbotImg->GetHeight() * 2);
	g->SetFastStretch(false);

	// A good use of fast/non fast stretching is to enable the slower stretching method
	// for users with a supported 3D card and to disable it for users that have
	// to run in software mode. You'll learn more about determining if a user is in
	// 3D mode or not in a later demo, but keep this simple optimization in mind
	// when creating drawing code for your games.

	// Another cool thing we can do is to draw an image mirrored. This is exactly
	// like using the DrawImage command except that you use DrawImageMirror
	// instead. The optional fourth argument is just a convenience switch: if it's false,
	// it will draw the image normally. In 3D mode, this is just as fast as a normal draw.
	g->DrawImageMirror(mApp->mTurbotImg, 75, 275);

	// But what if you want to draw the image mirrored AND stretched? It's a little different,
	// but still easy. The first parameter, like usual, is the image you want to draw.
	// The second parameter is a Rect which indicates the destination region you want to draw
	// the image to: The XY of the Rect is just what you think it is, it's the pixel coordinate
	// the image should be drawn at. The Width, Height of the Rect however is how you accomplish
	// the stretching/shrinking. Specify a different width/height from the original image size
	// and you've just accomplished resizing the original image! Note that we have a third
	// parameter, also a Rect. This is the source rectangle, and indicates the region of
	// image you wish to draw. As you'll see in our animation example, you don't have to
	// always draw the entire image. You can specify a rectangular region of the source image
	// that you wish to draw, which is useful for large strips of animation (more below).
	// For our current case, however, we want to draw the entire image. So we specify that
	// we should draw from the top left (0,0) coordinate of the source image and we should
	// use it's full width/height. Again, just like with DrawImage, you can use the
	// SetFastStretch call to set whether you want the nice, slow, smooth scaling, or the quick
	// and efficient exact scaling. In 3D mode, this is just as fast as a normal stretched draw.
	g->DrawImageMirror(mApp->mTurbotImg, 
						Rect(200, 275, mApp->mTurbotImg->GetWidth() * 2, mApp->mTurbotImg->GetHeight() * 2),
						Rect(0, 0, mApp->mTurbotImg->GetWidth(), mApp->mTurbotImg->GetHeight()));
						

	// Remember that black and white image we made in GameApp::LoadingThreadCompleted?
	// How about we draw it now so you can see what the result looks like:
	g->DrawImage(mApp->mAlteredImg, 500, 0);

	// And now for the exciting part: animation! As you can see in ::Update, we
	// increment the animation frame every 5 update ticks. We don't want to use
	// DrawImage because that will render the entire image. Instead, we only
	// want to draw a particular cel. We do that with the DrawImageCel function,
	// specifying the row or column to draw like so:
	g->DrawImageCel(mApp->mLightningImg, 0, 540, mAnimFrame);

	// If your animation strips contain both multiple rows AND columns,
	// you will need to use one of the alternate forms of DrawImageCel.
	// DrawImageCel is really just a convenience wrapper around DrawImage.
	// As you may have seen, you can tell DrawImage to draw just a particular
	// rectangular region of the image. Here is the equivalent function
	// call that we could have used in place of DrawImageCel above:
	//g->DrawImage(mApp->mLightningImg, 
	//Rect(0, 540, mApp->mLightningImg->GetWidth(), mApp->mLightningImg->GetCelHeight()),
	//Rect(0, mApp->mLightningImg->GetCelHeight() * mAnimFrame, mApp->mLightningImg->GetWidth(), mApp->mLightningImg->GetCelHeight()));
	//
	// As you can see, DrawImageCel is a lot quicker to type. 

	// Let's also display the current mouse XY and which button(s) are held down.
	// You should recall how to set fonts and change their colors from Demo2.
	// You will notice that the X, Y is not updated when the cursor moves over
	// the button that we added. This can be explained by reading the comments
	// for the MouseMove/MouseDrag/MouseDown/MouseUp methods.
	g->SetFont(mApp->mFont);
	g->SetColor(Color(255, 255, 255));
	g->DrawString(StrFormat(_S("X, Y is %d, %d"), mMouseX, mMouseY), 630, 20);
	
	SexyString buttonStr;
	if (mLeftDown)
		buttonStr += _S("Left button is down. ");
	if (mRightDown)
		buttonStr += _S("Right button is down. ");
	if (mMiddleDown)
		buttonStr += _S("Middle button is down. ");

	WriteWordWrapped(g, Rect(630, 40, mWidth - 630, 300), buttonStr, -1, -1);

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::AddedToManager(WidgetManager* theWidgetManager)
{
	// At this point, the Board class has already been added to the
	// widget manager. We should call our parent class' method
	// so that it can be sure to perform any needed tasks, first.
	Widget::AddedToManager(theWidgetManager);

	// Now let's create our first widget: a button. We do that by
	// telling the button what integer ID it should be identified with,
	// and by passing it a pointer to a button listener. Widgets have
	// "listeners" which respond to their particular events. Any class
	// can be a listener. For this particular demo, it is convenient for the
	// Board class to be the listener, but it is also common to use GameApp
	// as the main listener for all application buttons. The choice is up
	// to you and your needs.
	mButton = new ButtonWidget(1, this);

	// Remember how we had to position and size the Board class when we first
	// created it? A button is no different, it too is a widget. Let's
	// place this button on screen and set it's size now:
	mButton->Resize(675, 500, 100, 50);

	// Because a button can have a label on it, we should set the font to use:
	mButton->SetFont(mApp->mFont);

	// And just what should that label be? How about the word "Off".
	// We'll make it so that when it's clicked, it changes to "On" and
	// back to "Off" again.
	mButton->mLabel = _S("Off");

	// We can also change some colors, like the label color and the color
	// the label gets when moused over using the constants below:
	mButton->SetColor(ButtonWidget::COLOR_LABEL, Color(0, 0, 0));
	mButton->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(0, 255, 0));

	// And finally, just like with the Board class, we have to add it
	// if we want to do anything with it.
	theWidgetManager->AddWidget(mButton);

	// If you want, you can also control the placement of the button.
	// You can put it in front of another widget, behind another widget,
	// at the top of the drawing order, or at the bottom of the
	// drawing order. You accomplish that with the WidgetManager's
	// PutInFront, PutBehind, BringToFront, BringToBack methods, respectively.

	// But wait, what does this button look like? If you don't specify an
	// image to use, the default Windows widget look will be used. We'll cover
	// images and widgets in a later demo.
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::RemovedFromManager(WidgetManager* theWidgetManager)
{
	// This is called after we've been removed from the widget manager.
	// Again, we should let our base class do anything it needs to, first.
	Widget::RemovedFromManager(theWidgetManager);

	// We should now also remove any widgets we are responsible for. In
	// our current case, we made a button in AddedToManager. Let's remove
	// it now:
	theWidgetManager->RemoveWidget(mButton);

	// We'll delete it in our destructor.
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::ButtonDepress(int theId)
{
	// Because we told our button that we, the Board class, are 
	// going to listen for its particular events, this method will 
	// thus be called when our button has any events it wants us
	// to know about. In our current case, we only want to know when
	// the button is clicked, but you could also respond to a few others.
	// Let's change the label on our button whenever it is clicked, from
	// "Off" to "On" and back again.
	if (theId == 1)
	{
		// We assigned ID of 1 to our button. You'd ideally want to use
		// a constant, but since this is a demo and there's only 1 button,
		// we're going to just use a literal instead. When a button
		// causes an action, it calls the appropriate listener function and
		// let's the listener know who it is via the ID parameter.
		if (mButton->mLabel == _S("Off"))
			mButton->mLabel = _S("On");
		else
			mButton->mLabel = _S("Off");
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseMove(int x, int y)
{
	// This is called because the mouse cursor changed position and
	// the Board class was the widget closest to the cursor.
	// We're going to keep track of the XY coordinate whenever we
	// get this message for the sake of this demo.
	mMouseX = x;
	mMouseY = y;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseDrag(int x, int y)
{
	// This is called because the mouse cursor changed position  while
	// a button was down (a drag) and
	// the Board class was the widget closest to the cursor.
	// We're going to keep track of the XY coordinate whenever we
	// get this message for the sake of this demo.
	// Note that MouseDrag is called instead of MouseMove under
	// this condition.
	mMouseX = x;
	mMouseY = y;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseDown(int x, int y, int theClickCount)
{
	// Let the parent class know about this and perform any actions
	// it needs to.
	Widget::MouseDown(x, y, theClickCount);

	// Let's just keep track of which button is currently held down. 
	if (theClickCount == 3)
		mMiddleDown = true;
	else if (theClickCount > 0)
		mLeftDown = true;
	else if (theClickCount < 0)
		mRightDown = true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::MouseUp(int x, int y, int theClickCount)
{
	// Let the parent class know about this and perform any actions
	// it needs to.
	Widget::MouseUp(x, y, theClickCount);

	//You can actually tell if the left, right,
	// or middle buttons are currently held down by calling one of these
	// WidgetManager methods: IsLeftButtonDown, IsRightButtonDown, 
	// IsMiddleButtonDown. However, we're going to keep track of this
	// manually just to illustrate a point.
	if (theClickCount == 3)
		mMiddleDown = false;
	else if (theClickCount > 0)
		mLeftDown = false;
	else if (theClickCount < 0)
		mRightDown = false;
}