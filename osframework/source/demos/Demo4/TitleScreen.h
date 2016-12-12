#ifndef __TITLE_SCREEN_H__
#define __TITLE_SCREEN_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

namespace Sexy
{

class GameApp;
class Graphics;
class WidgetManager;

// A new widget that we'll be learning about. It's explained in the .CPP code.
class HyperlinkWidget;


// If you forgot about the ButtonListener class, you should review Demo3.
// The hyperlink widget is essentially the same thing as a button widget,
// and emits the same messages that a button does. Thus, to act on its
// messages we derive from the ButtonListener class.
class TitleScreen : public Widget, public ButtonListener
{

private:

	GameApp*			mApp;
	HyperlinkWidget*	mContinueLink;		

public:

	//////////////////////////////////////////////////////////////////////////
	//	Function: TitleScreen
	//	Parameters:
	//		theApp	- Pointer to the main application class
	//	
	//	Returns: none
	//////////////////////////////////////////////////////////////////////////
	TitleScreen(GameApp* pApp);

	virtual ~TitleScreen();

	//////////////////////////////////////////////////////////////////////////
	//	Function: Init
	//	Parameters: none
	//	Returns: none
	//
	//	Purpose: Called BEFORE the title screen is added to the widget manager
	//	by GameApp. This initializes some things like the images used for
	//	our hyperlink widget.
	//////////////////////////////////////////////////////////////////////////	
	void Init(void);

	//////////////////////////////////////////////////////////////////////////
	//	Function: AddedToManager
	//	Parameters:
	//		theWidgetManager	- Pointer to the main widget manager from
	//								GameApp.
	//
	//	Returns: none
	//
	//	Purpose: This function is automatically called by the widget manager
	//	which also passes a pointer to itself, when the TitleScreen class is
	//	added to its list of widgets. Every widget gets this function
	//	called when it is first added. It useful to use this function to
	//	set up any other widgets that the class might contain, such as buttons.
	//////////////////////////////////////////////////////////////////////////	
	void AddedToManager(WidgetManager* theWidgetManager);

	//////////////////////////////////////////////////////////////////////////
	//	Function: RemovedFromManager
	//	Parameters:
	//		theWidgetManager	- Pointer to the main widget manager from
	//								GameApp.
	//
	//	Returns: none
	//
	//	Purpose: This function is automatically called by the widget manager
	//	which also passes a pointer to itself, when the TitleScreen class is
	//	removed from its list of widgets. Every widget gets this function
	//	called when it is finally removed. It useful to use this function to
	//	also remove any widgets that were added and created in AddedToManager.
	//////////////////////////////////////////////////////////////////////////
	void RemovedFromManager(WidgetManager* theWidgetManager);

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
	virtual void ButtonDepress(int theId);

	//////////////////////////////////////////////////////////////////////////
	//	Function: Draw
	//	Parameters:
	//		g	- Graphics object used to draw all images and fonts to the screen.
	//	
	//	Returns: none
	//
	//	Purpose: Called automatically by GameApp's WidgetManager. This is where
	//	we'll do all our display routines for the loading screen.
	//////////////////////////////////////////////////////////////////////////
	void Draw(Graphics* g);

	//////////////////////////////////////////////////////////////////////////
	//	Function: LoadingComplete
	//	Parameters: none
	//	Returns: none
	//
	//	Purpose: Called manually by GameApp when we are done loading all
	//	resources, to let the title screen know that it should unhide and
	//	enable the continue link, so the user can start playing the game.
	//////////////////////////////////////////////////////////////////////////
	void LoadingComplete();
};

} 

#endif //__TITLE_SCREEN_H__