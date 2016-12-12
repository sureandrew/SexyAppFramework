#ifndef __V12DEMOAPP_H__
#define __V12DEMOAPP_H__

#include "..\SexyAppFramework\SexyAppBase.h"

namespace Sexy
{

class Board;

class V12DemoApp : public SexyAppBase
{

public:

	Board*					mBoard;

public:

	V12DemoApp();
	virtual ~V12DemoApp();

	//////////////////////////////////////////////////////////////////////////
	//	Function: ShutdownHook
	//	Purpose: Instead of overriding Shutdown() and having to remember to
	//	call ::Shutdown(), and to remember to check if mShutdown is true
	//	or not, you can use this and forget about those previous headaches.
	//	Guaranteed to not be called once mShutdown is true so no need to worry
	//	about it occurring multiple times (unless of course for some reason you
	//	manually call it more than once). Thus, you no longer need a ShutdownHook()
	//	override.
	//////////////////////////////////////////////////////////////////////////	
	virtual void	ShutdownHook();

	//////////////////////////////////////////////////////////////////////////
	//	Function: InitHook
	//	Purpose: This uses the same idea behind ShutdownHook in that you don't
	//	have to worry about calling the base class' Init method anymore
	//	and can forget about those previous hassels. This method is guaranteed
	//	to be called only once, right after SexyAppBase::Init finishes. Thus,
	//	you no longer need an Init() override.
	//////////////////////////////////////////////////////////////////////////	
	virtual void	InitHook();

	//////////////////////////////////////////////////////////////////////////
	//	With these 2 functions, we'll pause the board if focus shifts to another
	//	application.
	//////////////////////////////////////////////////////////////////////////	
	virtual void	LostFocus();
	virtual void	GotFocus();

	//////////////////////////////////////////////////////////////////////////
	//	Function: DialogButtonDepress
	//	Parameters:
	//		dialog_id	- The ID of the dialog box whose button is emitting
	//						the DialogButtonDepress signal.
	//		button_id	- The ID of the button in the above dialog box that
	//						has been depressed.
	//
	//	Purpose: Previously, dialog button (de)presses were all routed through
	//	a ButtonListener. This was annoying, as to check which button was
	//	pressed, you had to check if the ID of the button was the ID of the
	//	dialog box + 2000 or + 3000. It also had the tendency to create large
	//	Button(De)press functions with giant case statements. With this new
	//	implementation, button (de)presses from a dialog box will be passed
	//	into this alternative method, and the IDs are their simple, original
	//	IDs that you assigned to the widgets upon initialization. For dialog
	//	boxes with a single button, the button's ID is 1, and for dialogs with
	//	2 buttons, it's 1 for yes/ok and 0 for no/cancel. Of course, this is 
	//	fully backwards compatible, so if you don't want to custom override
	//	the DialogButton(De)press method(s) you don't have to, and can still 
	//	check which button was pressed using the old original method via the
	//	Button(De)press methods.
	//////////////////////////////////////////////////////////////////////////	
	virtual void	DialogButtonDepress(int dialog_id, int button_id);
};

}
#endif //__V12DEMOAPP_H__
