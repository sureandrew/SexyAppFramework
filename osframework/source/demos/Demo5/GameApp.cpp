#include "GameApp.h"
#include "TitleScreen.h"
#include "Board.h"
#include "DemoDialog.h"
#include "SexyAppFramework/WidgetManager.h"

#include "SexyAppFramework/Checkbox.h"

// We will be accessing the resource manager in this demo, so include it's header
#include "SexyAppFramework/ResourceManager.h"

// Required for playing music
#include "SexyAppFramework/BassMusicInterface.h"

// Contains all the resources from the resources.xml file in our
// properties directory. See that file for more information.
#include "Res.h"

#include "SexyAppFramework/Dialog.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::GameApp()
{
	// mProdName is used for internal purposes to indicate the game that we're working on
	mProdName = "Demo 5";

	// For internal uses, indicates the current product version
	mProductVersion = "1.0";

	// This is the text that appears in the title bar of the application window
	mTitle = StringToSexyStringFast("SexyAppFramework: " + mProdName + " - " + mProductVersion);

	// Indicates the registry location where all registry keys will be read from
	// and written to. This is stored under the HKEY_CURRENT_USER tree on 
	// Windows systems.
	mRegKey = "PopCap\\SexyAppFramework\\Demo5";

	// Set the application width/height in terms of pixels here.
	mWidth = 640;
	mHeight = 480;

	// By setting this to true, the framework will automatically check to see
	// if hardware acceleration can be turned on. This doesn't guarantee that it
	// WILL be turned on, however. Some cards just aren't compatible or have
	// known issues. Also, cards with less than 8MB of video RAM aren't supported.
	// There are ways to override the 3D enabled settings, which we will discuss
	// in a later demo. As a side note, if you want to see if you app is
	// running with 3D acceleration, first enable debug keys by pressing
	// CTRL-ALT-D and then press F8. To toggle 3D on/off, press shift-F8. That is just
	// for testing purposes.
	mAutoEnable3D = true;

	mBoard = NULL;
	mTitleScreen = NULL;

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::~GameApp()
{
	// Remove our "Board" class which was, in this particular demo,
	// responsible for all our game drawing and updating.
	// All widgets MUST be removed from the widget manager before deletion.
	// More information on the basics of widgets can be found in the Board
	// class file. If you tried to delete the Board widget before removing
	// it, you will get an assert. Because our board might not have been
	// added (if you shut down the app before closing the loading screen),
	// only remove it if it isn't null.
	if (mBoard != NULL)
		mWidgetManager->RemoveWidget(mBoard);

	// Take a look at TitleScreen::ButtonDepress if you haven't already.
	// It explains a function called SafeDeleteWidget. Notice that we're
	// directly deleting the widget here: that is because when our app's
	// destructor is called, it's at the very end of the shutdown sequence
	// and the safe delete widget list will NOT be processed. Thus we
	// have to delete the memory manually.
	delete mBoard;


	// If you shut down the app before closing the loading screen, then
	// it will need to be removed here. The rational for the next two
	// steps is the same as for Board:
	if (mTitleScreen != NULL)
		mWidgetManager->RemoveWidget(mTitleScreen);
	delete mTitleScreen;

	// We should also free up all the resources that we loaded
	// for ALL the resource groups. Deleting a group that was
	// already deleted doesn't do anything, it's ignored.
	mResourceManager->DeleteResources("Init");
	mResourceManager->DeleteResources("TitleScreen");
	mResourceManager->DeleteResources("Game");
	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::Init()
{
	// Let the parent class perform any needed initializations first.
	// This should always be done.
	SexyAppBase::Init();

	// We need to tell the resource manager to read in all the groups
	// and information from that main group we made, called ResourceManifest,
	// in the file "properties/resources.xml". The path/filename are
	// by default set up to load that file, so you must name it exactly as such.
	// This doesn't load any resources: it just parses the data and sets
	// things up for loading.
	LoadResourceManifest();

	// Next, we want to load our absolutely necessary files that have to
	// be loaded before anything else can run. You'll notice in the resources.xml
	// file that we created a group called Init that contains these resources.
	// You may call it whatever you like. Let's load those resources now.
	// We do that by calling the LoadResources method of our mResourceManager
	// variable and specifying in quotes the name of the resource group to 
	// load. This string is case sensitive.
	if (!mResourceManager->LoadResources("Init"))
	{
		mLoadingFailed = true;
		// This will display an informative error message indicating exactly
		// what went wrong in the resource loading process.
		ShowResourceError(true);
		return;
	}

	// Now we've loaded the resources, but we need to extract them.
	// Extraction is the phase that converts sound files to raw WAV
	// files, and sets up and initializes fonts and palletizes images.
	// The ResourceGen.exe program, when it generates C++ code for our
	// resources, also creates a function for EVERY resource group of the
	// form: Extract<GROUP>Resources, where <GROUP> is the exact name
	// of the resource group you made. In our case, we made an "Init"
	// group, so we have an ExtractInitResources method. You pass to it
	// the pointer to the resource manager. Because an error can occur
	// during this step, you should make sure to check for it.
	if (!ExtractInitResources(mResourceManager))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	// We also need to load our title screen graphics in, since you can't 
	// display the title screen without any graphics. For an explanation of why
	// we placed this in a separate group from Init, see properties/resources.xml.
	// This code works exactly like the above did for the Init group.
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

	// Let's also load in some music to play. We use the mMusicInterface
	// member for all our music needs, which requires the BassMusicInterface.h
	// header to be loaded, since we use the library BASS to play our music.
	// We can load in WAV, OGG, or MP3 files. BASS also supports a number
	// of tracker formats, such as .it, .xm, .mod, etc. It also supports
	// a format called MO3, which is a compressed version of a tracker
	// file. For this example, we will use the MO3 from AstroPop.
	// Why? Cause it's ours and we won't get sued for using it.
	// We load our file manually, we do not use the resource manager for this.
	// The first parameter is the ID to associate the song with. Just as sounds
	// have IDs, so do music tracks.
	mMusicInterface->LoadMusic(0, "music/music.mo3");

	// Let's load another copy of the file. Why? In order to fade from one
	// track to another, we need two instances of the track on different
	// channels. Let's load it again and give it a different ID, 1.
	mMusicInterface->LoadMusic(1, "music/music.mo3");

	// Now we need to start playing a track. Because we are using an MO3
	// and because the original format was a .it (Impulse Tracker) file,
	// there are actually multiple songs inside of it, differentiated
	// by various offsets. If you were just playing a single MP3 or OGG
	// or WAV file instead of a tracker file, you would ignore this
	// and use the default offset of 0 for the start of the song.
	// Because the person that made the song file was nice and
	// told us which offsets equated to which song pieces, I already
	// know the magic offset numbers. In this particular case, the
	// song for the intro screen is at offset 0, and the song
	// for the main game music is at offset 9. Our music artist
	// also was kind enough to put in tracker looping commands,
	// so you'll notice that the songs play over and over. A discussion
	// of tracker file formats is beyond the scope of this. Again,
	// if you are just playing a WAV/OGG/MP3, you use offset 0 (the default)
	// to indicate that you want to start playing from the start of the song.
	//
	// You can use PlayMusic to instantly play the track, or, like below,
	// you can use FadeIn to smoothly fade the song in. The first parameter
	// for both methods is the channel or song id that was used when the
	// track was first loaded (In our case, either 0 or 1 works). For both,
	// the second parameter is the offset to start playing at. Again, I just
	// happen to know that the intro song is at offset 0. For FadeIn, the
	// third parameter is how quickly to fade in, out of 1.0. The last parameter
	// for both indicates whether or not you want to loop. This is kind of weird,
	// but specify "false" to loop and "true" to not loop.
	mMusicInterface->FadeIn(0, 0, 0.002, false);

	// We'll cover changing the music and sound volumes in a later demo.

	// Next, we need to know how many resources there are to load.
	// This is necessary so we can display our progress bar on the title screen
	// and make it be the appropriate length. There's a variable in SexyAppBase
	// called mNumLoadingThreadTasks which holds the number of resources to
	// load in the LoadingThreadProc function. You get the number of resources
	// in a given group with a call to the resource manager's GetNumResources function
	// for each of your groups that you are going to load:
	mNumLoadingThreadTasks = mResourceManager->GetNumResources("Game");
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LoadingThreadProc()
{
	// For each of the groups that we want to load,
	// we first have to instruct the resource manager to begin the
	// loading phase and initialize its internal variables. 
	// We do that with the StartLoadResources method and pass in the 
	// exact string name of the group to begin loading:
	mResourceManager->StartLoadResources("Game");

	// Now we need to load each individual resource. We will loop,
	// calling LoadNextResource at the start. When it returns true,
	// there are no more resources to load for the current group.
	// LoadNextResource knows what group to load from because 
	// of the call to StartLoadResources above:
	while (mResourceManager->LoadNextResource())
	{
		// The SexyAppBase variable, mCompletedLoadingThreadTasks, indicates the
		// total number of resources that have so far been loaded. This is used
		// to tell our loading screen the % progress we've made. See TitleScreen::Draw
		// for an example of how this is used. We need to increment this value
		// ourselves everytime we load a resource:
		mCompletedLoadingThreadTasks++;

		// If there was an error loading our resource, the resource manager
		// will tell us to shut down by setting mShutdown to true. If that
		// happened, immediately abort and return:
		if (mShutdown)
			return;

		// Remember in demos 1-3 how we had the Board class call MarkDirty
		// every update? Well, the title screen doesn't need to be such a hog.
		// The title screen only needs to repaint when its progress bar changes
		// size. The progress bar only changes size when a resource gets loaded.
		// Because the game app is the only one that knows when this happens,
		// the game app will be the one to tell the title screen that it's a
		// dirty, dirty widget and that it needs a good and proper repainting.
		// You COULD make an update method for the title screen and mark dirty
		// every frame. But because this consumes more CPU time, it will take
		// longer to load our resources. And since you want the loading time
		// to be as quick as possible, you should only repaint when you need to.
		mTitleScreen->MarkDirty();
	}

	// Just like in our Init function, after loading resources we
	// need to extract them. Let's do that. Let's also ask the resource
	// manager if an error occurred in the above loop that we
	// didn't yet catch. We do that with the HadError method:
	if (mResourceManager->HadError() || !ExtractGameResources(mResourceManager))
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

	// When we're actually loading resources, we'll set the
	// mLoadingFailed variable to "true" if there were any problems
	// encountered along the way. If that is the case, just return
	// because we won't want the user to get to the main menu or any
	// other part of the game. We will want them to exit out.
	if (mLoadingFailed)
		return;

	
	// We aren't going to make and add the Board class here like we
	// did in the previous demos. Instead, since we are done loading
	// everything, we're going to tell the title screen that 
	// we're done and that it should unhide the continue link and let
	// the user enter the game.
	mTitleScreen->LoadingComplete();

	// Remember: since we didn't give our title screen an Update method,
	// this class is responsible for telling it when to repaint. If we
	// don't mark it dirty, you won't see the hyperlink widget
	// appear. So mark it dirty now:
	mTitleScreen->MarkDirty();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::TitleScreenIsFinished()
{
	mTitleScreen = NULL;
	mBoard = new Board(this);

	// Now that the title screen is done, we don't need its resources
	// wasting memory. Let's delete all of its resources. We do that
	// by calling DeleteResources and specifying the exact name of the
	// resource group we want to free up:
	mResourceManager->DeleteResources("TitleScreen");

	mBoard->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBoard);

	// This is a new step: We're going to tell the WidgetManager
	// that keyboard input and mouse wheel notifications should go to
	// the board object. This way, we'll be able to respond to keypresses:
	mWidgetManager->SetFocus(mBoard);

	

	// Let's fade out the intro song and fade in the main game music.
	// FadeOut works just like FadeIn did in Init() but with some
	// slightly different parameters. The first, is like with FadeIn and
	// PlayMusic, the channel or song id that you want to mess with.
	// The second indicates that the song fading out should stop when
	// done, if it is true. The final parameter indicates how fast
	// to fade out, and is from 0 to 1.
	mMusicInterface->FadeOut(0, true, 0.004);

	// Let's fade in the main game music. This is the same as in Init.
	// The only difference is we're using 1 instead of 0 for our song id.
	// Why? Well, channel/song id 0 is being used to fade out the 
	// previously playing track, we can't use it to also fade in.
	// That's why we loaded another copy of the song into channel 1.
	// Again, as explained in Init, I happen to know that offset 9
	// is the start of the main game music.
	mMusicInterface->FadeIn(1, 9, 0.002, false);

	// We'll cover changing the music and sound volumes in our options dialog.
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Dialog* GameApp::NewDialog(int theDialogId, bool isModal, const std::string& theDialogHeader, 
						   const std::string& theDialogLines, const std::string& theDialogFooter, int theButtonMode)
{
	// Rather than dupliate a lengthy explanation, check out the top of DemoDialog.cpp for a complete description
	// of what all the parameters and functions are.
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
void GameApp::SwitchScreenMode(bool wantWindowed, bool is3d)
{
	// Let the app handle the actual details of this call...
	SexyAppBase::SwitchScreenMode(wantWindowed, is3d);

	// We can see if the options dialog is up with a call to
	// GetDialog. You pass GetDialog the unique ID of the dialog box,
	// and if it exists it is returned to you, otherwise NULL is returned.
	DemoDialog* d = (DemoDialog*) GetDialog(DemoDialog::DIALOG_ID);

	// Set the checkbox state to our windowed state
	if ((d != NULL) && (d->mFSCheckbox != NULL))
		d->mFSCheckbox->SetChecked(!wantWindowed);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::ButtonPress(int theId)
{
	if (theId == DemoDialog::MESSAGE_BOX_ID + 2000)
		KillDialog(theId - 2000);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::SetFocusToBoard()
{
	if (mBoard != NULL)
		mWidgetManager->SetFocus(mBoard);
}