#include "Board.h"
#include "GameApp.h"
#include "SexyAppFramework/Graphics.h"

// See the Draw method for more information on using the Color class.
#include "SexyAppFramework/Color.h"

// Why are we including ImageFont.h and not Font.h? Font.h is just a generic
// base class. ImageFont creates fonts from an image that contains all the
// text characters as well as a text file that indicates character widths
// and kerning information, as well as some more advanced features not used
// in this tutorial such as font layers, etc.
#include "SexyAppFramework/ImageFont.h"

// The Image.h file just declares basic functions. All images are either of 
// the DDImage or MemoryImage type. For this demo, we will use DDImage
// types, as they are the type returned by the image loading code.
// A DDImage is actually derived from MemoryImage, so where an Image or
// MemoryImage is required, a DDImage will suffice as well. A DDImage
// contains optimized code for use with DirectX 7+.
#include "SexyAppFramework/DDImage.h"

// The Rectangle template, used to specify X, Y, Width, Height
#include "SexyAppFramework/Rect.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::Board(GameApp* theApp)
{
	mApp = theApp;

	// Start by fully glowing and making the pulse decrease
	mPulseAmt = 255;
	mIncPulse = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::~Board()
{
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

	// Every frame (100 times per second) let's change the pulse amount by
	// 1. When we reach either 0 or 255 (the lower/upper bounds of
	// a color value) we'll reverse the pulsing direction. See the Draw()
	// method for its usage: we're going to draw an image with varying intenstiy
	// over time.
	if (mIncPulse)
	{
		if (++mPulseAmt >= 255)
		{
			mIncPulse = false;
			mPulseAmt = 255;
		}
	}
	else
	{
		if (--mPulseAmt <= 0)
		{
			mIncPulse = true;
			mPulseAmt = 0;
		}
	}

	// Let's play the timer sound effect once every 2 second.
	// Since this method is called 100 times per second, just mod
	// mUpdateCnt by 200. We do that by calling GameApp's PlaySample()
	// method and specifying the integral ID of the sound to play. This
	// ID is the same as we used when loading the sound in GameApp::LoadingThreadProc()
	if (mUpdateCnt % 200 == 0)
		mApp->PlaySample(1);

	// What about playing a sound but making it play on either the left or
	// right speaker? And what about pitch shifting? These are all possible,
	// but we won't learn about them until a later demo.



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
	// And now for the good stuff! The Graphics object, "g", is 
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

	// Now let's print some text. In order to do so, we need to instruct the
	// graphics object as to which font we want to use. We do that with
	// the SetFont method. Then we call the DrawString method and pass in
	// a normal string and give it an X, Y to draw at. But wait! If we just
	// draw a string right now, we won't see it. Can you guess why?
	// The graphics object is still using the color, white, which we
	// set a few lines above. Thus, when it goes to draw the string,
	// it will draw it in white. How do you change the text color?
	// Easy: it's just like you learned for drawing those primitives:
	// just use SetColor and you're done! It doesn't matter whether you
	// set the color before or after you set the font, fyi.
	g->SetColor(Color(255, 0, 0));
	g->SetFont(mApp->mTextFont);
	g->DrawString(_S("Woo! Text! Ya!"), 10, 10);

	// Wait a minute, didn't we say to draw at a Y value of 10? Then why does it 
	// appear to be drawing at about 0? That's because the text is offset
	// by the ascent of the font. A discussion of font terms is beyond the
	// scope of this demo, but basically just remember that if you want to
	// display the text at the exact Y coordinate, that you should add
	// the value of the font's ascent. You can get the ascent by calling
	// GetAscent() on the font in question. I'm going to use the same
	// Y value of 10 just to show you the slight difference using GetAscent makes.
	g->SetColor(Color(55, 90, 255));
	// We already set the font, no need to set it again unless we want a new one
	g->DrawString(_S("I like Techno and Drum n' Bass"), 170, 10 + mApp->mTextFont->GetAscent());

	// And to help you out visually, let's draw a line at Y coordinate 10:
	g->SetColor(Color(255, 255, 255));
	g->DrawLine(0, 10, mWidth, 10);

	// Just for fun, let's change the font. This font, as you can see from its image, 
	// only contains digits and a few symbols. As an example, I will draw a string
	// containing only those characters as well as a string containing characters that
	// the font can't represent to show you what would happen.
	g->SetFont(mApp->mNumberFont);
	g->SetColor(Color(255, 255, 0));
	g->DrawString(_S("+200"), 10, 40);
	g->DrawString(_S("+200 pts"), 10, 60);

	// You can also get the width of a string in pixels for any given font. In addition, you
	// can use a printf style function, StrFormat, which is defined in Common.h, to 
	// format strings. You might want to look at Common.h for some...common...and handy functions.
	g->SetColor(Color(0, 255, 255));
	SexyString myString = StrFormat(_S("You got %d points!"), 147);
	g->SetFont(mApp->mTextFont);
	g->DrawString(myString, 10, 80);
	g->SetColor(Color(0, 255, 0));
	g->DrawString(_S(" I am to the right of that previous string."), 10 + mApp->mTextFont->StringWidth(myString), 80);
	

	// What about some other common text functions, like justifying and word wrapping? 
	// Not a problem. The Widget class contains some useful features.
	// For justification, the easiest is to use the WriteString method. 
	// The valid justification values are:
	//	-1: Left justified (same as DrawString)
	//	0: Centered
	//	1: Right justified
	//	
	//	For centered/right justified, you can also specify a width indicating the size of the region you
	//	are printing in. The sole purpose of that is to control the centering/right aligning. 
	g->SetColor(Color(255, 255, 255));
	WriteString(g, _S("Left justified at X of 200"), 200, 95, -1, -1);
	WriteString(g, _S("Centered using app width"), 0, 110, mWidth, 0);
	WriteString(g, _S("Centered using width of 200, X of 200"), 200, 125, 200, 0);

	// And now for a word wrapping example. With this function, you specify a rectangular region
	// that represents the "box" in which you want the text to be displayed. You can also
	// set the line spacing, or use -1 for the default font height, and you can also set
	// the justification using the same values as for WriteString. We'll also draw the "box" using
	// the DrawRect function, for illustrative purposes. You may notice that I didn't offset the
	// Y coordinate of the text by the font's ascent: that's because WriteWordWrapped automatically
	// does that for you.
	g->SetColor(Color(255, 255, 255));
	g->DrawRect(30, 140, 200, 400);
	g->SetColor(Color(0, 255, 0));
	WriteWordWrapped(g, Rect(30, 140, 200, 400), _S("This is some text that is wrapped inside of the rectangle \
												at X of 30, Y of 140, width of 200, height of 400. It has \
												been wrapped using the default left justification and the \
												default line spacing."), -1, -1);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Enough talk, let's draw images!
	// The simplest way to draw an image is with, coincidentally, the DrawImage method.
	// Just specify the image and the X, Y to draw at and boom! It's done.
	g->DrawImage(mApp->mTurbotImg, 230, 140);

	// What about changing the color of an image? This is called colorization, and it works just like
	// all the previous examples: You call SetColor with the color you want. But wait! There's 
	// an extra step: Because colorizing an image on the fly is slower than normal drawing, you have
	// to explicity tell the graphics object that you want to enable it. This is done by calling the
	// SetColorizeImages method of the Graphics class with a parameter value of "true".
	// IMPORTANT: You should call SetColorizeImages again but with a parameter of "false" when you
	// are done drawing your colorized image! Otherwise all subsequent image drawing calls will
	// be colorized using the most recent value from SetColor. You may call SetColor either before
	// or after SetColorizeImages, it doesn't matter. For this example, we'll draw an image normally
	// and then colorized. We'll also use the GetHeight() method to draw this image just below the
	// previous one.
	g->DrawImage(mApp->mOpaqueBeamImg, 230, 140 + mApp->mTurbotImg->GetHeight());

	g->SetColor(Color(255, 0, 0));
	g->SetColorizeImages(true);
	g->DrawImage(mApp->mOpaqueBeamImg, 230 + mApp->mOpaqueBeamImg->GetWidth(), 140 + mApp->mTurbotImg->GetHeight());
	g->SetColorizeImages(false);

	// You may have noticed that part of the previous two images was drawn offscreen. The graphics object by
	// default sets a clipping region to the size of the widget, and anything drawn outside of it
	// is clipped, or not drawn. You'll see some more advanced uses of clipping and widgets in a later demo.

	// There's another cool effect you can do to images: Drawing additively instead of normally.
	// Additive drawing is just what the name implies: when you draw the image, instead of
	// drawing using the exact RGBA values of the image, it actually combines them with whatever
	// pixel values there are in the location in which it is placed. One common use of this is
	// to draw the same image, but brighter: To do so, you'd just draw the image normally first,
	// then draw the same image on top of it but additively. Let's try an example and draw
	// our first image, mApp->mTurbotImg, additively to get a bright effect. 
	g->DrawImage(mApp->mTurbotImg, 230 + mApp->mTurbotImg->GetWidth(), 140);

	// Enable additive drawing with the call below. IMPORTANT: You should set the
	// drawmode back to Graphics::DRAWMODE_NORMAL when done, so that other images
	// don't get drawn additively. Additive drawing is slower than colorization,
	// so keep that in mind if you are concerned about your game's performance.
	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);	
	g->DrawImage(mApp->mTurbotImg, 230 + mApp->mTurbotImg->GetWidth(), 140);
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);

	// You can even draw an image colorized and additively. This is comonly used to
	// implement a pulsing sort of effect. Let's do just that: the variable,
	// mPulseAmt, is updated in our Update method and controls the RGB value
	// that we'll use to make our image pulse. Again, be sure to set things back
	// to normal when you are done. Also, it matters not in which order the
	// SetDrawMode, SetColor, and SetColorizeImages calls are made.
	g->DrawImage(mApp->mTurbotImg, 230 + mApp->mTurbotImg->GetWidth() * 2, 140);

	g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);	
	g->SetColor(Color(mPulseAmt, mPulseAmt, mPulseAmt));
	g->SetColorizeImages(true);
	g->DrawImage(mApp->mTurbotImg, 230 + mApp->mTurbotImg->GetWidth() * 2, 140);
	g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
	g->SetColorizeImages(false);

	// And finally, let's just have a simple example of alpha blending. We'll
	// Draw our first image, a picture of a moon, normally, and draw
	// our friend Turbot at reduced opacity on top of it. The result is you can
	// see through turbot to the moon below. Notice that we set the color
	// for turbot to r=255, g=255, b=255, alpha=128: This results in the
	// RGB values not being changed, only the alpha. Using a value of 255 for
	// a colorization amount basically means that the color component will not
	// be altered.
	g->DrawImage(mApp->mMoonImg, 400, 300);
	g->SetColor(Color(255, 255, 255, 60));
	g->SetColorizeImages(true);
	g->DrawImage(mApp->mTurbotImg, 400, 300);
	g->SetColorizeImages(false);

}