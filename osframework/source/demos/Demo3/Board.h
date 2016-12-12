#ifndef __BOARD_H__
#define __BOARD_H__

//////////////////////////////////////////////////////////////////////////
//						Board.h
//
//	This is the third class to look at in this particular demo
//	(after main.cpp and GameApp.h/.cpp). The Board class is where most of
//	your actual game programming will go. It is here that we will do
//	all our game drawing, updating, and input processing. Of course, in
//	a larger application, you would probably do drawing and updating in
//	multiple files, but you would still most likely use something similar
//	to a Board class as the master game logic class. 
//
//	The reason that the Board class is a widget is because when a widget
//	is added to the GameApp's WidgetManager, it will automatically have its
//	Update and Draw methods called, and it will automatically receive input
//	at the appropriate times. Furthermore, by making it a widget and adding
//	it to the WidgetManager, the game logic loop, Update(), will be guaranteed
//	to run at a standard 100FPS on all machines. This is extremely important
//	as you always want your logic code to run at the same speed, but want
//	the drawing code to run as fast as possible. That way on faster machines
//	your program doesn't run its logic faster than on a slower machine.
//
//	You can think of the Board as a canvas upon which we do all our
//	drawing, and a central hub where if we need to, we instruct other
//	classes where and when to draw to.
//////////////////////////////////////////////////////////////////////////

// This file must be included so that we can derive our Board class from it
#include "SexyAppFramework/Widget.h"

// You'll learn about responding to button events in this demo. 
// Any class that wants to respond to them must derive from ButtonListener.
#include "SexyAppFramework/ButtonListener.h"

// We place all our classes inside the "Sexy" namespace to avoid name collisions
// with other libraries that might be added.
namespace Sexy
{


// Forward declare the graphics class. You will see the graphics class used
// and explained in Board.cpp: it is the main object used to draw all
// images, fonts, etc.
class Graphics;

// We maintain a pointer to the main game application in the Board class.
// The main game app contains functions that are often times needed
// by the Board class, such as registry reading/writing, file reading/writing,
// etc.
class GameApp;

// We're going to make a modified copy of GameApp::mTurbotImg, so we'll
// need to forward declare the DDImage class. See Board::CreateMirroredStretchedImg.
class DDImage;

// We're going to create a button in this demo and respond to its
// click event.
class ButtonWidget;

// In this demo, we're going to do some more advanced things like 
// handle the two cases where Board is added and removed from the 
// WidgetManager.
class WidgetManager;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Board : public Widget, public ButtonListener
{

	private:

		GameApp*		mApp;
		ButtonWidget*	mButton;			// The button that we're going to create in this demo
		int				mAnimFrame;			// The current frame of animation we're on for the mLightningImg image.
		int				mMouseX, mMouseY;	// As an example, we'll keep track of the mouse cursor position and display it on screen
		
		// We'll also keep track of which button is down
		bool			mLeftDown;
		bool			mRightDown;
		bool			mMiddleDown;

	public:

		//////////////////////////////////////////////////////////////////////////
		//	Function: Board
		//	Parameters:
		//		theApp	- Pointer to the main application class
		//	
		//	Returns: none
		//////////////////////////////////////////////////////////////////////////
		Board(GameApp* theApp);

		virtual ~Board();


		//////////////////////////////////////////////////////////////////////////
		//	Function: Draw
		//	Parameters:
		//		g	- Graphics object used to draw all images and fonts to the screen.
		//	
		//	Returns: none
		//
		//	Purpose: Called automatically by GameApp's WidgetManager, this function
		//	is the main method that is responsible for all graphical and textual
		//	displaying.
		//////////////////////////////////////////////////////////////////////////
		virtual void Draw(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	Function: Update
		//	Parameters: none
		//	Returns: none
		//
		//	Purpose: Called automatically by GameApp's WidgetManager, this method
		//	is GUARANTEED to be called 100 times per second (100FPS) and is where
		//	all main game logic is performed. Of course, if you had a larger more
		//	complex game, you'd most likely divide your logic between several
		//	other files, but this is commonly the central place where all game
		//	logic begins and is executed.
		//////////////////////////////////////////////////////////////////////////
		virtual void Update();

		//////////////////////////////////////////////////////////////////////////
		//	Function: ButtonDepress
		//	Parameters:
		//		theId	- Integer ID of the button that was clicked
		//
		//	Returns: none
		//
		//	Purpose: This method is called by the WidgetManager when a button widget
		//	is first pressed and THEN released. You can use ButtonPress if you want
		//	to know when the button is first pressed (before it is released).
		//	theId is the integer ID that was assigned to the button when it was
		//	first created. 
		//////////////////////////////////////////////////////////////////////////		
		virtual void	ButtonDepress(int theId);

		//////////////////////////////////////////////////////////////////////////
		//	Function: AddedToManager
		//	Parameters:
		//		theWidgetManager	- Pointer to the main widget manager from
		//								GameApp.
		//
		//	Returns: none
		//
		//	Purpose: This function is automatically called by the widget manager
		//	which also passes a pointer to itself, when the Board class is
		//	added to its list of widgets. Every widget gets this function
		//	called when it is first added. It useful to use this function to
		//	set up any other widgets that the class might contain, such as buttons.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	AddedToManager(WidgetManager* theWidgetManager);

		//////////////////////////////////////////////////////////////////////////
		//	Function: RemovedFromManager
		//	Parameters:
		//		theWidgetManager	- Pointer to the main widget manager from
		//								GameApp.
		//
		//	Returns: none
		//
		//	Purpose: This function is automatically called by the widget manager
		//	which also passes a pointer to itself, when the Board class is
		//	removed from its list of widgets. Every widget gets this function
		//	called when it is finally removed. It useful to use this function to
		//	also remove any widgets that were added and created in AddedToManager.
		//////////////////////////////////////////////////////////////////////////
		virtual void	RemovedFromManager(WidgetManager* theWidgetManager);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseMove
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime the
		//	mouse moves. The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored (you'll learn about how
		//	to bypass that in a later demo).
		//////////////////////////////////////////////////////////////////////////		
		virtual void	MouseMove(int x, int y);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseDrag
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime the
		//	mouse moves AND a button is held down. This is the same thing as
		//	MouseMove except that it represents a drag operation. When dragging,
		//	MouseDrag will be called INSTEAD OF MouseMove.
		//	The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored (you'll learn about how
		//	to bypass that in a later demo).
		//////////////////////////////////////////////////////////////////////////
		virtual void	MouseDrag(int x, int y);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseDown
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//		theClickCount - An integer indicating which mouse button
		//						was pressed. One of the following:
		//			1:  Left button
		//			2:  Double-left-click
		//			3:  Middle button
		//			-1: Right button
		//			-2: Double-right-click
		//
		//		IMPORTANT: Because you can't have a double click with a
		//		single click, you will receive BOTH a left (or right) button as well
		//		as a double left (or right) button message whenever you double-click.
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime 
		//	a mouse button is in the down state.
		//	The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored (you'll learn about how
		//	to bypass that in a later demo).
		//////////////////////////////////////////////////////////////////////////
		virtual void	MouseDown(int x, int y, int theClickCount);

		//////////////////////////////////////////////////////////////////////////
		//	Function: MouseUp
		//	Parameters:
		//		x - X coordinate relative to the application of the mouse 
		//		y - Y coordinate relative to the application of the mouse
		//		theClickCount - An integer indicating which mouse button
		//						was pressed. One of the following:
		//			1:  Left button
		//			2:  Double-left-click
		//			3:  Middle button
		//			-1: Right button
		//			-2: Double-right-click
		//
		//		IMPORTANT: Because you can't have a double click with a
		//		single click, you will receive BOTH a left (or right) button as well
		//		as a double left (or right) button message whenever you double-click.
		//
		//	Returns: none
		//
		//	Purpose: Called by the WidgetManager automatically anytime 
		//	a mouse button is in the up state after previously being in the down state.
		//	The topmost widget (i.e. the widget most immediately
		//	under the cursor) is the one who gets the function call, and any
		//	widgets underneath by default are ignored (you'll learn about how
		//	to bypass that in a later demo).
		//////////////////////////////////////////////////////////////////////////
		virtual void	MouseUp(int x, int y, int theClickCount);		
};


}

#endif // __BOARD_H__