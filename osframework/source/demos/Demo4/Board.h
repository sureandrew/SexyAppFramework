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


// Forward declare the graphics class. You will see the graphics class used
// and explained in Board.cpp: it is the main object used to draw all
// images, fonts, etc.
class Graphics;

// We maintain a pointer to the main game application in the Board class.
// The main game app contains functions that are often times needed
// by the Board class, such as registry reading/writing, file reading/writing,
// etc.
class GameApp;

// forward declare the widgets we're going to use in this demo:
class ButtonWidget;
class EditWidget;
class Checkbox;
class ListWidget;
class ScrollbarWidget;

// In this demo, we're going to do some more advanced things like 
// handle the two cases where Board is added and removed from the 
// WidgetManager.
class WidgetManager;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Board :	public Widget, public ButtonListener, 
				public EditListener, public CheckboxListener, 
				public ListListener
{

	private:

		GameApp*			mApp;
		ButtonWidget*		mButton1;		// We'll use these buttons for sound playing
		ButtonWidget*		mButton2;

		// These are explained in the C++ code, they are the new widgets we're learning about.
		EditWidget*			mEditWidget;
		Checkbox*			mCheckboxWidget;
		ListWidget*			mListWidget;
		ScrollbarWidget*	mScrollbarWidget;

		SexyString			mText;		// When we press enter on the edit box, we'll set this string and print it

		// Both are floats to ensure that the only difference in the movement demo
		// is the fact that one is updated in UpdateF and the other is in Update.
		float			mMotionX;		// For our movement example, this is the X coordinate of the image as it moves rightward
		float			mUpdateFMotionX;// Same as above, but only modified in UpdateF, to illustrate the difference in motion

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
		//	Function: EditWidgetText
		//	Parameters:
		//		theId		- Integer ID of the edit widget sending this message
		//		theString	- The contents of the edit widget
		//
		//	Returns: none
		//
		//	Purpose: Called whenever the return/enter key is pressed on
		//	an edit widget.
		//////////////////////////////////////////////////////////////////////////
		void EditWidgetText(int theId, const std::string& theString);

		//////////////////////////////////////////////////////////////////////////
		//	Function: AllowChar
		//	Parameters:
		//		theId	- Integer ID of the edit widget sending this message
		//		theChar	- Character just typed in
		//
		//	Returns: 
		//		true	- Indicates that the character is acceptible
		//		false	- Indicates that the character is invalid
		//
		//	Purpose: Whenever an ASCII character is typed into the edit box,
		//	this method is called first. If the method returns true, then the
		//	character just typed is accepted and appended to the current edit widget
		//	string. If it returns false, the character is rejected and is not added.
		//////////////////////////////////////////////////////////////////////////
		bool AllowChar(int theId, char theChar);

		//////////////////////////////////////////////////////////////////////////
		//	Function: CheckboxChecked
		//	Parameters:
		//		theId	- Integer ID of the checkbox widget sending this message
		//		checked	- Boolean indicating if the widget is checked or not
		//
		//	Returns: none
		//
		//	Purpose: Whenever a checkbox widget is checked or unchecked, this
		//	method is called. We're not actually going to do anything with this,
		//	we're just listing it here as an example of how you'd implement a
		//	function to respond to that event.
		//////////////////////////////////////////////////////////////////////////
		void CheckboxChecked(int theId, bool checked) {;}

		//////////////////////////////////////////////////////////////////////////
		//	Function: ListClicked
		//	Parameters:
		//		theId	- Integer ID of the listbox widget sending this message
		//		theIdx	- Integer indicating the index of the item selected in the list
		//		theClickCount	- An integer indicating which mouse button
		//						was pressed. One of the following:
		//			1:  Left button
		//			2:  Double-left-click
		//			3:  Middle button
		//			-1: Right button
		//			-2: Double-right-click
		//
		//	Returns: none
		//
		//	Purpose: Called any time a list widget is clicked on. The list
		//	widget by default doesn't automatically select the item you clicked on,
		//	it instead calls this method and in here you manually select the item.
		//	This is to allow you to prevent the selection of certain items, such as
		//	disabled items, etc.
		//////////////////////////////////////////////////////////////////////////		
		void ListClicked(int theId, int theIdx, int theClickCount);

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
		//	Function: UpdateF
		//	Parameters: 
		//		theFrac	- The number of updates this time slice represents.
		//
		//	Returns: none
		//
		//	Purpose:
		//	There has been a fundamental temporal aliasing issue in the previous
		//	demos because games run at a 100 Hz Update rate while the user's monitor 
		//	is refreshing at some other rate, generally between 60 and 85 Hz.  The fixed 
		//	100 Hz Update rate is convenient because it allows game logic to proceed 
		//	independantly from the refresh rate, but in some cases it's worth the extra 
		//	trouble of updating at a variable rate in order to provide smoother animation, 
		//	as in the case of a scrolling background, a shark with words written on it, 
		//	or an Arkanoid ball.
		//
		//	To illustrate the aliasing problem, imagine a ball that is supposed to move 
		//	200 pixels per second, running on a 75 Hz monitor.  The update rate of the 
		//	game is 100 Hz, so that means that we will add 2 pixels to the ball position 
		//	every update, and there will be 1.33 updates per monitor refresh (on average).  
		//	That means that that 2 out of every 3 monitor refreshes will show the ball 
		//	moving 2 pixels, and and the third will show it moving 4 pixels.  That isn't 
		//	smooth motion.  The correct solution would be for the ball to move 2.67 
		//	pixels every monitor refresh.  But how do we do that?
		//
		//	To support smooth motion, we use UpdateF.  Widget::UpdateF is similar to 
		//	Widget::Update, but Widget::UpdateF gets a float passed into it that 
		//	represents how many Update's this time slice represents.  In the 75 Hz 
		//	example, UpdateF would always be called with 1.33.  Update has certainly 
		//	not been made obsolete, however, and you can choose which 
		//	parts of your game logic should be in Update and which should be in 
		//	UpdateF.  To facilitate cooperation and good behavior between the two 
		//	update methods, there are some rules they follow:  Updating always occurs 
		//	in blocks, with one or two Update calls followed immediately with an 
		//	UpdateF call.  This means that the application will never get the chance 
		//	to draw or process input between an Update and a Draw without calling 
		//	UpdateF in the middle.  Therefore, you can assume that focus won't be 
		//	lost, nor will input change between an Update and an UpdateF, and you'll 
		//	know that you'll have a chance to finalize your state in UpdateF so things 
		//	can be left dangling (whatever that means for your app) after Update.  
		//	You are also guaranteed that the value passed in to UpdateF will be between 
		//	1.67 (for a 60 Hz monitor) and 1.0 (for a 100 Hz monitor).  Even if the 
		//	monitor is 60 Hz but the computer is only fast enough to draw at 30 FPS 
		//	you will get two Update blocks in a row before the draw, so it will still 
		//	appear to your app as if you are updating at 60 Hz.
		//
		//		IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
		//
		//	In order to fully use this, you need to set up a few things.
		//	Set GameApp::mVSyncUpdates to true, override UpdateF(float theFrac),
		//	and move some code from Update that used to look like 
		//	this: "mPos += 1.5;", changing it to "mPos += 1.5 * theFrac;".
		//	Check out the C++ code for an example of motion using both Update and
		//	UpdateF.
		//
		//		IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
		//
		//	Because UpdateF is called a variable number of times per second,
		//	you do NOT want to put game logic in it that needs to remain framerate
		//	independant. Use UpdateF ONLY for movement related operations, and not
		//	for your main game code.
		//
		//	If you really want to avoid shearing in windowed mode, you can
		//	set GameApp::mWaitForVSync to true and set GameApp::mSoftVSyncWait
		//	to false. NOTE: This winds up doing some busy waiting and consumes
		//	more processor time. 
		//	IMPORTANT: YOU MUST ALSO DELETE THE FOLLOWING REGISTRY KEY:
		//	Whereever your registry settings are stored 
		//	(HKEY_LOCAL_MACHINE\SOFTWARE\SexyAppFramework\Demo4 for this case),
		//	you must delete the key "WaitForVSync". This is VERY important, and it
		//	won't work otherwise.
		//////////////////////////////////////////////////////////////////////////
		virtual void UpdateF(float theFrac);

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