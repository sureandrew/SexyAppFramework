#include "TitleScreen.h"

#include "GameApp.h"

// Contains all the resources from the resources.xml file in our
// properties directory. See that file for more information.
#include "Res.h"

// Now that we're on our 4th demo, the reason for including the headers
// below should be apparent. If you forgot, review demos 1-3.
#include "SexyAppFramework/Font.h"
#include "SexyAppFramework/Graphics.h"
#include "SexyAppFramework/Image.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/Rect.h"

// A new class: the HyperlinkWidget. It acts and resembles a standard
// WWW hyperlink.
#include "SexyAppFramework/HyperlinkWidget.h"

using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TitleScreen::TitleScreen(GameApp *pApp)
{
	mApp = pApp;		
	mContinueLink = NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
TitleScreen::~TitleScreen()
{
	delete mContinueLink;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::Init(void)
{
	// A hyperlink widget is basically the same thing as a button widget
	// but with some slight differences. It is meant to emulate the look
	// and feel of an internet hyperlink. We create the widget the
	// same way we created a button widget in Demo3, specifying that
	// this class is to listen to the widget events.
	mContinueLink = new HyperlinkWidget(1, this);

	// We need to set the font to use. In previous demos, we had
	// a variable inside of GameApp that held the font. Now that we're
	// using the resource manager, we'll use the main (and only) one
	// that we declared from there. Remember how we named it FONT_DEFAULT?
	// Well, you can use that name exactly now to reference it.
	mContinueLink->SetFont(FONT_DEFAULT);

	// Like the button widget, we can set the text to display:
	mContinueLink->mLabel = _S("CLICK TO CONTINUE");

	// And like the button widget, we can set the default and over colors.
	// We don't use the SetColor method though, we just directly
	// set mColor and mOverColor:
	mContinueLink->mColor = Color(255, 255, 255);
	mContinueLink->mOverColor = Color(0, 255, 0);
	mContinueLink->mUnderlineSize = 1;


	// We're not going to place the widget just yet. Why? Well,
	// the TitleScreen hasn't been added to the widget manager.
	// We'll wait until the AddedToManager method is called before
	// adding it, just like in Demo3.
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::AddedToManager(WidgetManager *theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);	

	// Just like with the button widget from Demo3, we're going to add our
	// hyperlink widget in this method. However, we want the link to be
	// invisible at first. Why? Well, while we're loading our resources,
	// we don't want the user to be able to just start the game. We want
	// them to wait until it's all loaded and ready to go. Once everything
	// is loaded, then we'll make it visible and let them click to play.
	// We do that with the SetVisible command:
	mContinueLink->SetVisible(false);

	// Just making it invisible isn't enough. Technically the user could
	// still click on it even though they can't see it. We have to
	// disable the widget to prevent it from receiving input:
	mContinueLink->SetDisabled(true);

	// Let's resize it to be at the bottom of the screen, and to be exactly
	// the width/height of the lable that we set. Again, we use our friend
	// the Resize function. However, note that we're setting the height to
	// be a bit larger than that of the string. That's because the underline
	// part of the hyperlink widget (whose thickness can be controlled via mUnderlineSize)
	// draws a little below the string. If you didn't make the height a little
	// larger, the line would be clipped and thus wouldn't be visible. Clipping
	// is discussed in the next demo.
	int labelWidth = FONT_DEFAULT->StringWidth(mContinueLink->mLabel);
	int labelHeight = FONT_DEFAULT->GetHeight();
	mContinueLink->Resize(	mWidth / 2 - labelWidth / 2, 
							mHeight - labelHeight - 40,
							labelWidth,
							labelHeight+4);

	// Almost done. Let's make it so that when the user mouses over the widget,
	// that the cursor changes from the standard Windows arrow to a hand icon.
	// We do that by setting the variable mDoFinger to true. This instructs the
	// WidgetManager to chagne the cursor to the hand/finger icon when the mouse
	// is inside the widget's bounds, and to reset it back to the arrow when it
	// is not. Note that ALL widgets have this functionality.
	mContinueLink->mDoFinger = true;
	
	// And finally, we add the widget just like we added the button widget
	// from demo 3 and the Board widget.
	theWidgetManager->AddWidget(mContinueLink);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::RemovedFromManager(WidgetManager *theWidgetManager)
{
	// This is just like what we did in Board in Demo3.
	// Let our parent class know about the removal, and get rid
	// of our hyperlink widget. 
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mContinueLink);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::Draw(Graphics *g)
{
	// Instead of having to say Color(0, 0, 0) or Color(255, 255, 255)
	// for black/white, you can use Color::Black and Color::White.
	g->SetColor(Color::Black);
	g->FillRect(0, 0, mWidth, mHeight);


	// We want to draw our loader bar to indicate the progress made in
	// loading all our resources. As you recalll, GameApp::LoadingThreadProc is
	// the thread that is actually loading everything. The app has a function,
	// GetLoadingThreadProgress, that returns a value from 0.0 to 1.0 indicating
	// the % complete it is. We will multiply this % complete by the width
	// of our progress bar, so that we give the illusion of a growing bar.
	int loaderBarWidth = IMAGE_LOADER_BAR->GetWidth();
	int drawWidth = (int)(mApp->GetLoadingThreadProgress() * loaderBarWidth);
	if (drawWidth > 0)
	{
		// As you may recall from Demo3 where we drew the frames of animation
		// for the lightning image, the DrawImage call can take a source rectangle
		// which indicates what chunk of the original image we want to draw.
		// In our case, we want to start from from the upper left corner of
		// the loader bar, but we only want to draw "drawWidth" wide. This will
		// give the illusion that the progress bar is expanding as the resources
		// are loaded in.
		g->DrawImage(IMAGE_LOADER_BAR, mWidth / 2 - loaderBarWidth / 2, 
					 400, 
					Rect(0, 0, drawWidth, IMAGE_LOADER_BAR->GetHeight()));
	}

	// If our hyperlink widget is false, let's instead draw some
	// "Loading" text (er, actually in this case it's an image) where
	// it is located.
	if (mContinueLink->mVisible == false)
		g->DrawImage(IMAGE_LOADER_LOADINGTXT, mContinueLink->mX, mContinueLink->mY - 20);	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::LoadingComplete()
{
	// Since the app told us that we're done loading all our resources,
	// let's unhide and enable our continue link so the user can start
	// playing.
	mContinueLink->SetVisible(true);
	mContinueLink->SetDisabled(false);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::ButtonDepress(int theId)
{
	if (theId == 1)
	{
		// Our hyperlink widget was pressed. We want to remove ourselves
		// and the hyperlink widget, and tell the app to display the
		// main board and get the game started. 
		// You might be thinking, "If I delete the title screen and
		// hyperlink, won't I crash the program?" Yes, you will. That's
		// why we aren't going to delete them using "delete". We're going
		// to use SexyAppBase's SafeDeleteWidget method. This places the
		// widget in a queue that is processed after all widgets and data
		// get processed, and is then deleted at a time that is safe
		// and appropriate. We still have to remove ourself and the
		// hyperlink widget from the WidgetManager. We can easily access
		// the WidgetManager, as it is a public variable in our game app.
		mApp->mWidgetManager->RemoveWidget(this);
		mApp->mWidgetManager->RemoveWidget(mContinueLink);
		mApp->SafeDeleteWidget(this);
		mApp->SafeDeleteWidget(mContinueLink);
		mContinueLink = NULL;

		// Now let's tell the game app that it's ok to add the board widget:
		mApp->TitleScreenIsFinished();
	}
}