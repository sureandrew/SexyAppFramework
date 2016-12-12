#ifndef __GAMEAPP_H__
#define __GAMEAPP_H__

//////////////////////////////////////////////////////////////////////////
//							GameApp.h
//
//	This is what drives the whole game. In here, you derive your class
//	from SexyAppBase and implement common game tasks, such as 
//	responding to widgets (covered later), initializing and loading
//	resources, setting up the various game screens, etc.
//	All applications at minimum must have a class that derives from
//	SexyAppBase.
//
//	The GameApp class is used to do such things as create the main 
//	menu screen, create the main game class (where all drawing/updating/
//	interaction takes place), etc.
//////////////////////////////////////////////////////////////////////////

#include "SexyAppFramework/SexyAppBase.h"

// We place all our classes inside the "Sexy" namespace to avoid name collisions
// with other libraries that might be added.
namespace Sexy
{

	// The GameApp class will be responsible for creating a class by the name
	// of "Board", which we will use to do all the game's drawing, input processing,
	// etc. Board is the second most important class and is where almost all of your
	// game logic code will originate from. It is a widget, which allows for
	// easy and automatic invocation of its update, drawing, and input processing
	// functions. See the "Board" class for more details. 
	class Board;

	// You'll learn more about images in GameApp.cpp and Demo2,
	// so check there for more info. You'll notice we're using DDImage instead
	// of Image, like we did in Demo 2. That's for convenience only, so that
	// we don't have to type cast the image every time we want to work with it.
	class DDImage;

	// We'll need a font for the label on our button (see Board for more info on buttons)
	class ImageFont;

class GameApp : public SexyAppBase
{

	private:

		Board*		mBoard;	

	public:

		// These are the images we'll be using for this demo. See
		// GameApp.cpp for full information. We're going to
		// make them public for easy accessing, since the point of this tutorial
		// is to quickly get you up to speed on using the framework.
		DDImage*	mTurbotImg;
		DDImage*	mLightningImg;
		DDImage*	mAlteredImg;		// Used in our example of modifying the image bits to make an altered version of an image.

		ImageFont*	mFont;				// The font we'll use for the label on our button in Board

	public:

		GameApp();
		virtual ~GameApp();

		//////////////////////////////////////////////////////////////////////////
		//	Function: Init
		//	Parameters: none
		//	Returns: none
		//
		//	Purpose: Initializes the application. Sets the resolution, overrides
		//	any default settings, and if there is a loader/intro screen (not in this demo)
		//	creates it and displays it. The framework will then automatically
		//	call the LoadingThreadProc() method after this method returns.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	Init();

		//////////////////////////////////////////////////////////////////////////
		//	Function: LoadingThreadProc
		//	Parameters: none
		//	Returns: none
		//
		//	Purpose: Loads all resources in a separate thread. If there is a 
		//	loader/intro screen (not in this demo), would also update the
		//	loader progress indicator. When the function returns, the
		//	LoadingThreadCompleted() method is automatically called.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	LoadingThreadProc();

		//////////////////////////////////////////////////////////////////////////
		//	Function: LoadingThreadCompleted
		//	Parameters: none
		//	Returns: none
		//
		//	Purpose: Called when LoadingThreadProc is complete and all resources
		//	have been loaded. It is in this function that you would then set up
		//	your main menu or similar screen. For this particular demo however,
		//	we will go straight to the main game class, "Board".
		//////////////////////////////////////////////////////////////////////////		
		virtual void	LoadingThreadCompleted();		

};

}


#endif	// __GAMEAPP_H__