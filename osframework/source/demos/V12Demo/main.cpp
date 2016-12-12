//////////////////////////////////////////////////////////////////////////
//						main.cpp
//
//	This is the starting point for all new projects. This file's purpose is
//	pretty small, but important. In here we create our application, initialize
//	it, and begin processing all the game code.
//
//	***********************************************************************
//	NOTE: You should have read and understood demos 1-5 before 
//	this, as topics that are already covered will be either not commented
//	or minimally so. In addition, you REALLY REALLY
//	should have read through the documentation included with the 1.2 release. 
//	***********************************************************************
//
//	This demo will teach you:
//	* How to use the new InitHook/ShutdownHook for slightly safer/easier 
//		init/shutdown procedures
//
//	* How to use the new DialogListener class, to listen to button events 
//		specifically from a dialog (in contrast to using ButtonListener and
//		checking if the button ID is dialog ID + 2000 or 3000).
//
//	* How to use flags to set the default behavior of a dialog box
//
//	* How to use flags to modify widget behavior, such as always marking
//		dirty, not allowing mouse events, etc.
//
//	* How to take advantage of the new widget heirarchy, which lets you
//		very easily add/place widgets within a parent widget with less code
//		and less mess and do some neat things without tons of code.
//
//	* How to use the new overlay drawing system, which eliminates the need
//		for a widget overlay (like the one in the Hun-garr demo for pausing).
//
//	* How to easily push/pop/preserve graphics state
//
//	* How to use the new and powerful Graphics::DrawImageTransform/F to
//		very quickly and easily apply rotation, scaling, translation, or any combo
//		to an image, as opposed to the previous messy way of doing things.
//
//	* How to use the also new and also cool Graphics::DrawImageMatrix
//		function to draw an image in 2D and/or 3D with a matrix. This allows
//		some neat and fast tricks like easy flipping, mirroring, scaling, etc
//		with a minimal number of lines.
//
//	* How to use UpdateApp to help with transitions
//
//	* How to be master of the universe. OK maybe not.
//////////////////////////////////////////////////////////////////////////

#include "V12DemoApp.h"

using namespace Sexy;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	gHInstance = hInstance;

	V12DemoApp* anApp = new V12DemoApp();
	anApp->Init();
	anApp->Start();
	delete anApp;

	return 0;
}
