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

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

// Because we're going to be learning about some new widgets, we
// need to include some more listener classes so we can respond to each one.
#include "SexyAppFramework/EditListener.h"
#include "SexyAppFramework/CheckboxListener.h"
#include "SexyAppFramework/ListListener.h"



// We place all our classes inside the "Sexy" namespace to avoid name collisions
// with other libraries that might be added.
namespace Sexy
{



class Graphics;
class GameApp;
class ButtonWidget;
class WidgetManager;
class Image;

struct ParallaxLayer
{
	float		mX;
	float		mY;
	Image*		mImage;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Board :	public Widget, public ButtonListener
{

	private:

		GameApp*			mApp;
		ButtonWidget*		mButton;

		// And in this demo, we're going to do some parallax scrolling whenever
		// the arrow keys are pressed.
		struct ParallaxLayer	mLayer[3];		

	public:

		// And let's finally use some enums for our widgets
		enum
		{
			OPTIONS_BUTTON_ID
		};

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
		//	Function: KeyDown
		//	Parameters:
		//		theKey	- A KEYCODE_  value from Keycodes.h that represents the
		//					key pressed.
		//
		//	Returns: none
		//
		//	Purpose: When a key is pressed, this method is called. This method
		//	differs from KeyChar (described below) in that you can detect non-ASCII
		//	keys with it, as well as ASCII.
		//////////////////////////////////////////////////////////////////////////		
		virtual void KeyDown(KeyCode theKey);

		//////////////////////////////////////////////////////////////////////////
		//	Function: KeyChar
		//	Parameters:
		//		theChar	- The ASCII character that was pressed.
		//
		//	Returns: none.
		//
		//	Purpose: Unlike KeyDown, this method only takes ASCII characters
		//	as input. It's common to use KeyChar for your normal ASCII processing
		//	needs, and KeyDown for your extended keyboard key needs. We're not
		//	actually going to do anything with KeyChar in this code. It's displayed
		//	here though to illustrate the differences between KeyDown and to 
		//	help explain its usage.
		//////////////////////////////////////////////////////////////////////////		
		//virtual void KeyChar(char theChar);

        
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
			
};


}

#endif // __BOARD_H__