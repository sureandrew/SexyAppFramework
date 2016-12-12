//////////////////////////////////////////////////////////////////////////
//				DemoDialog.h
//
//	This class demonstrates how to create a customized dialog box.
//	Most games will want to use custom implementations of the dialog
//	class if they want to go beyond the simple yes/no/ok ones.
//
//	Basically, all we want to do is implement custom drawing and updating
//	code, as well as do some little things when we get added/removed from
//	the widget manager and handle button presses slightly differently.
//
//	IMPORTANT: Make sure to read the comments in the .CPP file for the
//	Dialog constructor as they contain important information.
//////////////////////////////////////////////////////////////////////////

#ifndef __DEMO_DIALOG_H__
#define __DEMO_DIALOG_H__

#include "SexyAppFramework/Dialog.h"

// This lets us receive slider events
#include "SexyAppFramework/SliderListener.h"

#include "SexyAppFramework/CheckboxListener.h"

namespace Sexy
{

	class Graphics;

	// We're going to use slider widgets to control audio volume
	class Slider;

	// This is the same as a normal button, but it automatically tiles
	// the butotn image if we make the widget too large
	class DialogButton;

	class Checkbox;

class DemoDialog : public Dialog, public SliderListener, public CheckboxListener
{
	
	protected:

		Slider*			mMusicVolumeSlider;
		Slider*			mSfxVolumeSlider;
		DialogButton*	mRandomBtn;				// Click this for a random number
		DialogButton*	mClipBtn;				// Click this to toggle clipping		


	public:

		// Alright, fine, we'll use some constants for our widgets!
		enum
		{
			MUSIC_SLIDER_ID,
			SFX_SLIDER_ID,
			RANDOM_BTN_ID,
			CLIP_BTN_ID,
			FS_CHECKBOX_ID,
			HARDWARE_CHECKBOX_ID,
			MESSAGE_BOX_ID,
			DIALOG_ID
		};

		// For ease of accessing, we're going to make our checkboxes public,
		// as the GameApp class will need to query their states as well as
		// possibly alter the FSCheckbox's state.
		Checkbox*		m3DCheckbox;			// Toggle 3D enhanced mode
		Checkbox*		mFSCheckbox;			// Toggle fullscreen mode

	
	public:

		//////////////////////////////////////////////////////////////////////////
		//	Function: DemoDialog
		//	Parameters: 
		//		theHeader	- A string to use for the dialog box header
		//		theBody		- A string to use for the body of the text. May use
		//						\n characters to denote new line.
		//
		//	Returns: none
		//
		//	Purpose: Creates a new dialog box. You can specify a custom header
		//	and content with the two parameters.
		//////////////////////////////////////////////////////////////////////////		
		DemoDialog(std::string theHeader, std::string theBody);
		virtual ~DemoDialog();

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
		//	is GUARANTEED to be called 100 times per second (100FPS).
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
		//	Function: Resize
		//	Parameters:
		//		theX	- X coordinate to place the dialog at
		//		theY	- Y coordinate to place the dialog at
		//		theWidth	- Width of the dialog box
		//		theHeight	- Height of the dialog box
		//
		//	Returns: none
		//
		//	Purpose: Resizes and places the dialog box. We'll also
		//	make sure to position any widgets appropriately based on the new
		//	dimensions.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	Resize(int theX, int theY, int theWidth, int theHeight);

		//////////////////////////////////////////////////////////////////////////
		//	Function: SliderVal
		//	Parameters:
		//		theId	- ID of the slider object that generated this message
		//		theVal	- The value of the slider, i.e. where the thumb image
		//					is located. Leftmost is 0, rightmost is mVal in the slider widget.
		//
		//	Returns: none
		//
		//	Purpose: Whenever the thumb image on a slider is moved, this event is
		//	generated. The value passed in ranges from 0 to the maximum value on
		//	the slider, which is set in the slider's mVal variable.
		//////////////////////////////////////////////////////////////////////////		
		virtual void	SliderVal(int theId, double theVal);

		//////////////////////////////////////////////////////////////////////////
		//	Function: CheckboxChecked
		//	Parameters:
		//		theId	- Integer ID of the checkbox widget sending this message
		//		checked	- Boolean indicating if the widget is checked or not
		//
		//	Returns: none
		//
		//	Purpose: Whenever a checkbox widget is checked or unchecked, this
		//	method is called. 
		//////////////////////////////////////////////////////////////////////////
		void CheckboxChecked(int theId, bool checked);
};


}


#endif