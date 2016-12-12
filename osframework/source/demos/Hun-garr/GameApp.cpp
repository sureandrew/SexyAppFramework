#include "GameApp.h"
#include "TitleScreen.h"
#include "Board.h"
#include "OptionsDialog.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ResourceManager.h"
#include "SexyAppFramework/BassMusicInterface.h"
#include "SexyAppFramework/Dialog.h"
#include "Res.h"

using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::GameApp()
{
	mProdName = "Hun-garr Needs Planets!";
	mProductVersion = "1.0";
	mTitle = StringToSexyStringFast("SexyAppFramework: " + mProdName + " - " + mProductVersion);
	mRegKey = "PopCap\\SexyAppFramework\\Hungarr";

	mWidth = 640;
	mHeight = 480;

	mDebugKeysEnabled = true;

	// By setting this to true, the framework will automatically check to see
	// if hardware acceleration can be turned on. This doesn't guarantee that it
	// WILL be turned on, however. Some cards just aren't compatible or have
	// known issues. Also, cards with less than 8MB of video RAM aren't supported.
	// There are ways to override the 3D enabled settings, which we will discuss
	// in a later demo. As a side note, if you want to see if you app is
	// running with 3D acceleration, first enable debug keys by pressing
	// CTRL-ALT-D and then press F9. To toggle 3D on/off, press F8. That is just
	// for testing purposes.
	mAutoEnable3D = true;

	mBoard = NULL;
	mTitleScreen = NULL;

	// Enable smooth motion via UpdateF
	mVSyncUpdates = true;

	// Because it's annoying to hear the sound of the planets hitting a wall too many
	// times in a second, we'll limit how many can occur.
	mLastPlanetHitSoundTime = -30;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::~GameApp()
{	
	if (mBoard != NULL)
		mWidgetManager->RemoveWidget(mBoard);

	delete mBoard;

	if (mTitleScreen != NULL)
		mWidgetManager->RemoveWidget(mTitleScreen);
	delete mTitleScreen;

	// Using "" deletes ALL resources
	mResourceManager->DeleteResources("");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::Init()
{

	SexyAppBase::Init();

	LoadResourceManifest();
	if (!mResourceManager->LoadResources("Init"))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	if (!ExtractInitResources(mResourceManager))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	// To use custom cursors, you have to specify an image resource
	// to use. The CURSOR_ defines indicate which cursor type you
	// want to assign the image to. 
	// CURSOR_POINTER is the normal Windows pointer.
	// CURSOR_HAND is used when mousing over widgets (if mDoFinger is true).
	// CURSOR_DRAGGING is used when a drag operation is happening.
	// CURSOR_TEXT is the icon used when entering text in an edit box.
	SetCursorImage(CURSOR_POINTER, IMAGE_CUSTOM_POINTER);
	SetCursorImage(CURSOR_HAND, IMAGE_CUSTOM_HAND);
	SetCursorImage(CURSOR_DRAGGING, IMAGE_CUSTOM_DRAGGING);
	SetCursorImage(CURSOR_TEXT, IMAGE_CUSTOM_TEXT);

	if (!mResourceManager->LoadResources("TitleScreen"))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	if (!ExtractTitleScreenResources(mResourceManager))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	// Now let's create and add our title screen to the widget manager
	mTitleScreen = new TitleScreen(this);
	mTitleScreen->Resize(0, 0, mWidth, mHeight);

	// Let's let the title screen initialize it's widgets and data
	// before adding it to the widget manager:
	mTitleScreen->Init();

	mWidgetManager->AddWidget(mTitleScreen);

	// This example just uses 1 standard OGG file instead of a tracker file.
	// Since we aren't cross fading at all, we'll only load it once.
	mMusicInterface->LoadMusic(0, "music/hungarr.ogg");
	mMusicInterface->FadeIn(0, 0, 0.002, false);

	mNumLoadingThreadTasks = mResourceManager->GetNumResources("Game") + 
							 mResourceManager->GetNumResources("Hungarr");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LoadingThreadProc()
{

	mResourceManager->StartLoadResources("Game");

	while (mResourceManager->LoadNextResource())
	{
		mCompletedLoadingThreadTasks++;
		if (mShutdown)
			return;
		
		mTitleScreen->MarkDirty();
	}

	if (mResourceManager->HadError() || !ExtractGameResources(mResourceManager))
	{		
		ShowResourceError(false);
		mLoadingFailed = true;
	
		return;
	}

	mResourceManager->StartLoadResources("Hungarr");

	while (mResourceManager->LoadNextResource())
	{
		mCompletedLoadingThreadTasks++;
		if (mShutdown)
			return;

		mTitleScreen->MarkDirty();
	}

	if (mResourceManager->HadError() || !ExtractHungarrResources(mResourceManager))
	{		
		ShowResourceError(false);
		mLoadingFailed = true;

		return;
	}
	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LoadingThreadCompleted()
{
	// Let the base app class also know that we have completed
	SexyAppBase::LoadingThreadCompleted();

	if (mLoadingFailed)
		return;

	mTitleScreen->LoadingComplete();

	mTitleScreen->MarkDirty();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::TitleScreenIsFinished()
{

	mTitleScreen = NULL;
	mBoard = new Board(this);
	mResourceManager->DeleteResources("TitleScreen");


	mBoard->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBoard);
	mWidgetManager->SetFocus(mBoard);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Dialog* GameApp::NewDialog(int theDialogId, bool isModal, const std::string& theDialogHeader, 
						   const std::string& theDialogLines, const std::string& theDialogFooter, int theButtonMode)
{
	// Rather than dupliate a lengthy explanation, check out the top of DemoDialog.cpp for a complete description
	// of what all the parameters and functions are, in Demo5.
	Dialog* d = new Dialog(IMAGE_DIALOG_BOX, IMAGE_DIALOG_BUTTON, theDialogId, isModal,
		StringToSexyStringFast(theDialogHeader), StringToSexyStringFast(theDialogLines), StringToSexyStringFast(theDialogFooter), theButtonMode);

	d->SetButtonFont(FONT_DEFAULT);
	d->SetLinesFont(FONT_DEFAULT);
	d->SetHeaderFont(FONT_DEFAULT);

	d->SetColor(Dialog::COLOR_HEADER, Color::Black);
	d->SetColor(Dialog::COLOR_LINES, Color::Black);

	d->mSpaceAfterHeader = 30;
	d->Resize(100, 100, 300, 250);

	return d;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LostFocus()
{
	SexyAppBase::LostFocus();

	// When losing focus, we should pause the game board
	if (mBoard)
		mBoard->Pause(true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::GotFocus()
{
	SexyAppBase::GotFocus();

	if (mBoard)
		mBoard->Pause(false);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::ButtonPress(int theId)
{
	if (theId == OptionsDialog::MESSAGE_BOX_ID + 2000)
		KillDialog(theId - 2000);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::PlaySample(int theSoundNum)
{
	// If playing the sound for when a planet hits a wall, only let it
	// play if it hasn't played the sound for at least 30 update frames
	// (30 hundredths of a second).
	if (theSoundNum == SOUND_PLANET_HIT)
	{
		if (mUpdateCount - mLastPlanetHitSoundTime > 30)
		{
			mLastPlanetHitSoundTime = mUpdateCount;
			SexyAppBase::PlaySample(theSoundNum);
		}
	}
	else
		SexyAppBase::PlaySample(theSoundNum);
}
