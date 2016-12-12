#include "Board.h"
#include "GameApp.h"

// Contains all the resources from the resources.xml file in our
// properties directory. See that file for more information.
#include "Res.h"

// You should remember these files from the previous demos
#include "SexyAppFramework/Graphics.h"
#include "SexyAppFramework/Color.h"
#include "SexyAppFramework/Rect.h"
#include "SexyAppFramework/ButtonWidget.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ImageFont.h"
#include "SexyAppFramework/Image.h"

// Our example dialog box
#include "DemoDialog.h"

// And for our performance profiling example, we first have to define SEXY_PERF_ENABLED
// before including PerfTimer.h:
#define SEXY_PERF_ENABLED
#include "SexyAppFramework/PerfTimer.h"

// Lastly, for our example of how to catch memory leaks, we first
// enable leak detection with a #define in EACH of the files we want
// to find leaks in, then include the memory management file.
// IT IS VERY IMPORTANT THAT THIS DEFINE AND INCLUDE BE DONE AFTER ALL
// OTHER FILE INCLUDES OR ELSE IT WILL CAUSE LINKER AND COMPILER ERRORS!
// Memory leaks will automatically be dumped to "mem_leaks.txt" when
// the app is closed. 
#define SEXY_MEMTRACE
#include "SexyAppFramework/memmgr.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::Board(GameApp* theApp)
{
	mApp = theApp;


	mButton = NULL;

	// Set up our parallaxing layers
	for (int i = 0; i < 3; i++)
	{
		mLayer[i].mX = 0;

		// Get the image for this layer. You'll notice in our resource
		// file that we named the layers IMAGE_BG0, IMAGE_BG1, IMAGE_BG2.
		// Instead of having to manually type in the layer names for each one,
		// we can use some convenience routines from our generated Res.h/.cpp
		// resource files. First step: make a string of the ID of the image we want
		// to access:
		std::string imageStringID = StrFormat("IMAGE_BG%d", i);

		// what we need to do now is get the integer ID for our resource
		// that has the same name as imageStringID. 
		int id = GetIdByStringId(imageStringID.c_str());

		// Now that we have the integer ID, we can request the actual
		// image data with it. We do that with GetImageById:
		mLayer[i].mImage = GetImageById(id);

		// Set the Y coordinate of the background layer so that it's
		// base is at the bottom of the screen. Because the Board hasn't
		// been resized or added to the manager yet, it's own mHeight and
		// mWidth are at the default of 0, 0. But not to worry, we set the
		// overall game's width/height in GameApp's constructor, so we
		// can just use those variables instead:
		mLayer[i].mY = (float)(mApp->mHeight - mLayer[i].mImage->GetHeight());
	}

	// We will ON PURPOSE leak memory as an example of how to track
	// such things in your program. Review the comments at the top of this
	// file about memory leak detection:
	int* aLeakedInteger = new int;
	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::~Board()
{
	delete mButton;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Update()
{

	// As an example of how to profile things, we're going to profile
	// the update and draw functions. Clearly, profiling this almost
	// empty function isn't too useful, but the point is to show you
	// HOW to profile multiple things and how to view the results.
	// You tell the profiler that you want it to begin with the
	// statement below, passing in a string indicating WHAT you're
	// profiling. The string can be anything. Enabled debug keys by
	// pressing CTRL-ALT-D and then press F2 to enabled/disable profiling.
	SEXY_PERF_BEGIN("Start_Of_Update");

	Widget::Update();

	
	MarkDirty();

	// And you mark the end of a profiling section with
	// SEXY_PER_END, passing in the same string you passed to
	// SEXY_PERF_BEGIN.
	SEXY_PERF_END("Start_Of_Update");

	
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::KeyDown(KeyCode theKey)
{
	// If the user presses the right or left arrow keys, let's
	// scroll the layers:
	if (theKey == KEYCODE_RIGHT)
	{
		for (int i = 0; i < 3; i++)
		{
			// When the X coordinate has moved leftward (negative) by
			// the image's width or more, we reset the X coordinate back to
			// 0. Since we tile an image to the left and right of
			// this center image, the user won't see the snapping back to 0.
			mLayer[i].mX -= 1.0f * (i + 1);
			if (mLayer[i].mX <= -mLayer[i].mImage->GetWidth())
				mLayer[i].mX = 0;
		}
	}
	else if (theKey == KEYCODE_LEFT)
	{
		for (int i = 0; i < 3; i++)
		{
			// This is the opposite of above, but notice that we're checking
			// to see if the X coordinate is >= the width of the image, and not the
			// width of the application. That's because some of the layers are wider
			// than the screen: 960 wide, when the game is 640 wide. If we checked
			// against the game width instead, we'd get this odd flicker appearing
			// because the tiling would be off. You can try it and see the effect.
			mLayer[i].mX += 1.0f * (i + 1);
			if (mLayer[i].mX >= mLayer[i].mImage->GetWidth())
				mLayer[i].mX = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Draw(Graphics* g)
{
	// As an example of how to profile things, we're going to profile
	// the update and draw functions. 
	// You tell the profiler that you want it to begin with the
	// statement below, passing in a string indicating WHAT you're
	// profiling. The string can be anything. Enabled debug keys by
	// pressing CTRL-ALT-D and then press F2 to enabled/disable profiling.
	SEXY_PERF_BEGIN("Start_Of_Draw");

	// Clear the screen to black
	g->SetColor(Color(0, 0, 0));
	g->FillRect(0, 0, mWidth, mHeight);


	for (int i = 0; i < 3; i++)
	{
		int imgWidth = mLayer[i].mImage->GetWidth();

		
		// Let's learn about Graphics translation. Normally, you draw your image
		// at a given XY by passing the coordinates to DrawImage. As an alternative,
		// you can "translate" the Graphics object (change its XY drawing offset)
		// and then just draw your image at 0, 0. Think of the Graphics object by
		// default as always being at 0, 0. By translating it, you move that
		// to whereever you like. The usefulness is in saving extra typing,
		// as well as some other features that come into play when scrolling
		// large worlds with many objects. For each of the background layers,
		// let's move the graphics object to its base XY location:
		g->TranslateF(mLayer[i].mX, mLayer[i].mY);

		// Something else to know about translation is that it is the method
		// used when drawing widgets. For every widget, the Graphics object
		// is translated to its own X, Y coordinate. Thus, when drawing
		// in the widget's class, you should always draw as if the top left
		// corner of the widget itself was 0, 0. Since all along we've been making
		// our Board widget the size of the screen, this hasn't been something we've
		// had to care about. But as you'll see in the dialog box example, it's something
		// we have to take into consideration. Normally, you can't draw outside the
		// X, Y, width, height of a widget. However, as you'll also see, you can
		// change that to allow drawing anywhere you want.

		// Remember how in previous demos we mentioned that it's common to use the
		// smoother DrawImageF functions instead of DrawImage if the user has 3D
		// acceleration enabled? Let's do that know. We can check if hardware acceleration
		// is on or not with a call to SexyAppBase's Is3DAccelerated:
		if (mApp->Is3DAccelerated())
		{
			// Because we translated the graphics object, we can just draw our layers
			// offset by the image width. Had we not translated the graphics object, here
			// is the code we would have had to use instead:
			//	g->DrawImageF(mLayer[i].mImage, mLayer[i].mX - imgWidth, mLayer[i].mY);
			//	g->DrawImageF(mLayer[i].mImage, mLayer[i].mX, mLayer[i].mY);
			//	g->DrawImageF(mLayer[i].mImage, mLayer[i].mX + imgWidth,  mLayer[i].mY);

			g->DrawImageF(mLayer[i].mImage, (float)-imgWidth, 0.0f);
			g->DrawImageF(mLayer[i].mImage, 0.0f, 0.0f);
			g->DrawImageF(mLayer[i].mImage, (float)imgWidth, 0.0f);
		}
		else
		{
			// non-hardware accelerated
			g->DrawImage(mLayer[i].mImage, -imgWidth, 0);
			g->DrawImage(mLayer[i].mImage, 0, 0);
			g->DrawImage(mLayer[i].mImage, imgWidth, 0);
		}

		// You should remember to put the Graphics object back to where it was
		// before you translated it, otherwise all subsequent drawing commands will
		// be offset by that amount.
		g->TranslateF(-mLayer[i].mX, -mLayer[i].mY);
	}

	// And you mark the end of a profiling section with
	// SEXY_PER_END, passing in the same string you passed to
	// SEXY_PERF_BEGIN.
	SEXY_PERF_END("Start_Of_Draw");

}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::AddedToManager(WidgetManager* theWidgetManager)
{
	// At this point, the Board class has already been added to the
	// widget manager. We should call our parent class' method
	// so that it can be sure to perform any needed tasks, first.
	Widget::AddedToManager(theWidgetManager);

	// You should remember how to create buttons from Demo3. If not,
	// go back and review.
	mButton = new ButtonWidget(Board::OPTIONS_BUTTON_ID, this);	
	mButton->SetFont(FONT_DEFAULT);
	mButton->mLabel = _S("Click Me!");

	// This time, let's use some images for our button.
	// mOverImage is the image to use when the mouse cursor is over the button.
	// mDownImage is the image to use when a mouse button is held down on it
	// mButtonImage is the default image to use
	// If we wanted to, we could specify a disabled image as well.
	mButton->mOverImage = IMAGE_BUTTON_OVER;
	mButton->mDownImage = IMAGE_BUTTON_DOWN;
	mButton->mButtonImage = IMAGE_BUTTON_NORMAL;
	mButton->mDoFinger = true;
	mButton->Resize(56, 5, IMAGE_BUTTON_NORMAL->GetWidth(), IMAGE_BUTTON_NORMAL->GetHeight());

	theWidgetManager->AddWidget(mButton);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::RemovedFromManager(WidgetManager* theWidgetManager)
{
	// This is called after we've been removed from the widget manager.
	// Again, we should let our base class do anything it needs to, first.
	Widget::RemovedFromManager(theWidgetManager);

	// We should now also remove any widgets we are responsible for. 
	theWidgetManager->RemoveWidget(mButton);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::ButtonDepress(int theId)
{
	// As another PURPOSEFUL example of detecting memory leaks, let's
	// cause a leak here as well so you can see how the leak detection works.
	// Again, this is on purpose to illustrate a point. 
	struct ParallaxLayer* aLeakedParallaxLayer = new struct ParallaxLayer;

	if (theId == Board::OPTIONS_BUTTON_ID)
	{
		// Let's open our options dialog box. Everything works the same
		// except that instead of using the WidgetManager's AddWidget function,
		// we use a special AddDialog method. The first parameter is the ID
		// of the dialog box, the second is the dialog box itself.
		// In this case, we don't need to maintain a pointer to the dialog box
		// to delete later. It will automatically be cleaned up when the
		// dialog box is closed via the KillDialog command (see DemoDialog).
		DemoDialog* dlg = new DemoDialog("Header", "Hello! I am a dialog box.");
		dlg->Resize(50, 50, 300, 400);
		mApp->AddDialog(DemoDialog::DIALOG_ID, dlg);
	}
}