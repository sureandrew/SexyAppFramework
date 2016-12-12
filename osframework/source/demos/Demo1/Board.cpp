#include "Board.h"
#include "SexyAppFramework/Graphics.h"

// See the Draw method for more information on using the Color class.
#include "SexyAppFramework/Color.h"

// A generic X, Y point template. See Draw() below for usage info.
#include "SexyAppFramework/Point.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::Board(GameApp* theApp)
{
	mApp = theApp;
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

	// First, let's start by drawing some geometric primitives. By
	// default, the drawing color is black. We will change it later
	// on. The first command clears the screen by drawing a 
	// black rectangle (black due to the default color) that is
	// located at coordinate 0, 0 and is the same size as our 
	// Board widget, which is the same size as the application and
	// thus is the same size of the game window.
	g->FillRect(0, 0, mWidth, mHeight);

	// You may notice that there's another form of FillRect, one that
	// takes a Rect object as its sole parameter. That does the
	// same thing as the above function, but sometimes its more useful
	// to draw using a Rect object.

	// Now let's learn how to change the color of the shapes we're drawing.
	// Call the "SetColor" method to change the current drawing color.
	// The parameter is a Color object which can take 3 or 4 parameters.
	// If you call it with only 3 parameters, the 4th one, which is the
	// alpha value, is set to 255 for fully opaque. The first three are
	// in order, red, green, and blue. They range from 0 to 255.

	g->SetColor(Color(255, 128, 64));	// some ugly orangish color
	
	// Let's draw a smaller rectangle with its upper left corner
	// in the center of the screen. 
	g->FillRect(mWidth / 2, mHeight / 2, 50, 50);

	// Now let's continue with some other primitives. How about
	// drawing a few lines? We'll use a few other colors too.
	
	g->SetColor(Color(255, 0, 0));	// red

	// Parameter order is X1, Y1, X2, Y2
	g->DrawLine(0, 0, 200, 150);

	g->SetColor(Color(0, 255, 0));	// green
	g->DrawLine(mWidth, 0, mWidth - 200, 150);

	g->SetColor(Color(0, 0, 255));	// blue
	g->DrawLine(0, mHeight, 200, mHeight - 150);

	g->SetColor(Color(255, 255, 255));	// white
	g->DrawLine(mWidth, mHeight, mWidth - 200, mHeight - 150);

	// Let's draw another rectangle, with vertices where each of
	// the previous 4 lines were. This time, however, let's not
	// fill it in and just draw its outline instead. We
	// accomplish that with the DrawRect function, which doesn't
	// fill in the center. 
	g->SetColor(Color(255, 0, 255));	// purple
	g->DrawRect(200, 150, (mWidth - 200) - 200, (mHeight - 150) - 150);

	// Tired of drawing lines and quads? How about drawing a triangle
	// instad? To do that, we use the generic PolyFill function which
	// draws a closed polygon with any number of points, >= 2.
	// We pass this function an array containing these points. We'll
	// create such an array right now, although in reality you'd
	// probably make this a constant instead of creating it every time.
	// We'll make use of the Point template which will use the int type
	// to specify the X, Y coordinates of each point in our triangle.
	// It is assumed that the last point connects to the first point.
	Point trianglePoints[3];
	trianglePoints[0] = Point(30, 30);
	trianglePoints[1] = Point(30, 60);
	trianglePoints[2] = Point(60, 45);
	g->SetColor(Color(255, 255, 0));	// yellow
	g->PolyFill(trianglePoints, 3);

	// And how about a pentagram as well?
	g->SetColor(Color(0, 255, 255));	// cyan	
	Point pentaPoints[5];
	pentaPoints[0] = Point(200, 0);
	pentaPoints[1] = Point(150, 40);
	pentaPoints[2] = Point(150, 80);
	pentaPoints[3] = Point(250, 80);
	pentaPoints[4] = Point(250, 40);
	g->PolyFill(pentaPoints, 5);


}