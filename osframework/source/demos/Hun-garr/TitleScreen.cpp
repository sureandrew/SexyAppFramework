#include "TitleScreen.h"
#include "GameApp.h"
#include "Res.h"
#include "SexyAppFramework/Font.h"
#include "SexyAppFramework/Graphics.h"
#include "SexyAppFramework/Image.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/Rect.h"
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
	mContinueLink = new HyperlinkWidget(1, this);
	mContinueLink->SetFont(FONT_DEFAULT);
	mContinueLink->mLabel = _S("CLICK TO CONTINUE");
	mContinueLink->mColor = Color(255, 255, 255);
	mContinueLink->mOverColor = Color(0, 255, 0);
	mContinueLink->mUnderlineSize = 1;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::AddedToManager(WidgetManager *theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);	
	mContinueLink->SetVisible(false);
	mContinueLink->SetDisabled(true);

	int labelWidth = FONT_DEFAULT->StringWidth(mContinueLink->mLabel);
	int labelHeight = FONT_DEFAULT->GetHeight();
	mContinueLink->Resize(	mWidth / 2 - labelWidth / 2, 
							mHeight - labelHeight - 40,
							labelWidth,
							labelHeight+4);

	mContinueLink->mDoFinger = true;
	theWidgetManager->AddWidget(mContinueLink);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::RemovedFromManager(WidgetManager *theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mContinueLink);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::Draw(Graphics *g)
{
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
		g->DrawImage(IMAGE_LOADER_LOADINGTXT, mContinueLink->mX - 10, mContinueLink->mY - 80);	

	g->DrawImage(IMAGE_HUNGARR_LOGO, mWidth / 2 - IMAGE_HUNGARR_LOGO->mWidth / 2, 100);
	g->SetColor(Color::White);
	g->SetFont(FONT_HUNGARR);
	SexyString s = _S("HUN-GARR NEEDS PLANETS!");
	g->DrawString(s, mWidth / 2 - FONT_HUNGARR->StringWidth(s) / 2, 200);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void TitleScreen::LoadingComplete()
{
	// Since the app told us that we're done loading all our resources,
	// let's unhide and enable our continue link so the user can startr
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
		mApp->PlaySample(SOUND_CONTINUE);

		// Now let's tell the game app that it's ok to add the board widget:
		mApp->TitleScreenIsFinished();
	}
}