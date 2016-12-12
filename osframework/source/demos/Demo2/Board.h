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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Board : public Widget
{

	private:

		GameApp*	mApp;
		int			mPulseAmt;		// See Draw(): used to make an image pulse over time
		bool		mIncPulse;		// If true, then we're increasing the pulse value, otherwise, decreasing
		

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

};


}

#endif // __BOARD_H__