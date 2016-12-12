#include "GameApp.h"
#include "Board.h"
#include "SexyAppFramework/WidgetManager.h"

// Why are we including ImageFont.h and not Font.h? Font.h is just a generic
// base class. ImageFont creates fonts from an image that contains all the
// text characters as well as a text file that indicates character widths
// and kerning information, as well as some more advanced features not used
// in this tutorial such as font layers, etc.
#include "SexyAppFramework/ImageFont.h"

// The Image.h file just declares basic functions. All images are either of 
// the DDImage or MemoryImage type. For this demo, we will use DDImage
// types, as they are the type returned by the image loading code.
// A DDImage is actually derived from MemoryImage, so where an Image or
// MemoryImage is required, a DDImage will suffice as well. A DDImage
// contains optimized code for use with DirectX 7+.
#include "SexyAppFramework/DDImage.h"

// This will let us load sounds
#include "SexyAppFramework/SoundManager.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::GameApp()
{
	// mProdName is used for internal purposes to indicate the game that we're working on
	mProdName = "Demo 2";

	// For internal uses, indicates the current product version
	mProductVersion = "1.0";

	// This is the text that appears in the title bar of the application window
	mTitle = StringToSexyStringFast("SexyAppFramework: " + mProdName + " - " + mProductVersion);

	// Indicates the registry location where all registry keys will be read from
	// and written to. This is stored under the HKEY_CURRENT_USER tree on 
	// Windows systems.
	mRegKey = "PopCap\\SexyAppFramework\\Demo2";

	// Set the application width/height in terms of pixels here. Let's
	// use a different resolution from Demo 1 just for fun.
	mWidth = 800;
	mHeight = 600;

	mBoard = NULL;

	mTextFont = NULL;
	mNumberFont = NULL;
	mTurbotImg = NULL;
	mMoonImg = NULL;
	mOpaqueBeamImg = NULL;
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
	// it, you will get an assert.
	mWidgetManager->RemoveWidget(mBoard);
	delete mBoard;

	// We need to clean up after ourselves and delete the image and
	// font information.
	delete mTextFont;
	delete mNumberFont;
	delete mTurbotImg;
	delete mMoonImg;
	delete mOpaqueBeamImg;

	// We need to release the memory allocated to our sounds too.
	// This call frees up the memory for ALL sound effects.
	mSoundManager->ReleaseSounds();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::Init()
{
	// Let the parent class perform any needed initializations first.
	// This should always be done.
	SexyAppBase::Init();

	// In later demos, you will see more done with this function.
	// For now, we have nothing else to initialize, so we are done.
	// Once complete, the LoadingThreadProc function will automatically
	// start and we will begin loading all our needed resources.
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LoadingThreadProc()
{
	// This time, we have things to load. Let's load in our two fonts
	// and our three images.	
	// Besides loading data,
	// this thread can also update the progress indicator for the loading
	// screen, which you will see in later demos.
	// Once complete, the LoadingThreadCompleted function will be called.

	//		Loading images is easy: you don't have to specify the image type.
	// Depending on the file extension, the appropriate image decoder
	// will be used. The following image types are supported:
	// Targa (.tga), JPEG (.jpg), PNG (.png), GIF (.gif). You do NOT have
	// to specify the extension when loading the file if you don't want to.
	// In this case, all of the above extensions will be looked for.
	// A discussion of image formats is beyond the scope of this tutorial.
	//		There is some important information to know about images.
	//	You will notice in the "images" directory that for each image, 
	//	there is a black and white image with the same name but with
	//	an underscore ("_") at the end of it. By default, when you load
	//	and image, the code automatically looks for the presence of
	//	that file to use for the alpha information. Some file formats
	//	have the alpha channel built into them, like PNG files. But
	//	others, like JPEG or GIF files, do not. The purpose of the alpha
	//	file is of course to generate an image that doesn't have jagged
	//	lines, or to control the opacity of various parts of the image.
	//	As a side not, the alpha image file may also begin with the
	//	underscore instead of ending with it, it matters not, and again,
	//	is automatically loaded in by the image loading code.
	//	You need to clean up the memory allocated by these functions yourself.
	mOpaqueBeamImg = (DDImage*) GetImage("images/beam_opaque");

	// If the file was not found or couldn't be loaded (i.e. due to an
	// incompatible file format) the returned value will be NULL.
	// You should always check this, and if it occurs, display an error
	// message, then set mLoadingFailed to true, and then immediately return.
	if (mOpaqueBeamImg == NULL)
	{

		// The PopUp method displays a standard Windows message box.
		// If in full screen mode, this will appropriately handle things such
		// that the GDI surface is properly rendered and the dialog box appears
		// as expected.
		Popup("There was an error loading the file: images/beam_opaque");
		mLoadingFailed = true;

		return;
	}

	// Now load the other two images
	mMoonImg = (DDImage*) GetImage("images/moon");
	if (mMoonImg == NULL)
	{
		Popup("There was an error loading the file: images/moon");
		mLoadingFailed = true;
		return;
	}

	mTurbotImg = (DDImage*) GetImage("images/turbot_worry");
	if (mTurbotImg == NULL)
	{
		Popup("There was an error loading the file: images/turbot_worry");
		mLoadingFailed = true;
		return;
	}

	// So we've loaded the images, that's all there is right? Wrong.
	// If possible, we should try to palletize the images. An image that
	// contains 255 or fewer colors can be palletized. This results in
	// a memory savings of about 4x and doesn't affect the image quality
	// at all. It's the same principals that the GIF format uses: instead
	// of representing each red, green, blue, alpha value as a separate
	// quantity (1 byte each, 4 bytes in total per pixel), we represent
	// the actual combined RGBA value as a single number, from 0-255.
	// This number is an index into a lookup table. Thus, every time
	// the value (200,43,11,128), for example, is used, instead of
	// representing that value as a 4 byte value every time it
	// appears, we'd represent it with a 1 byte index into a lookup
	// table that contained the above RGBA value. Don't worry, you
	// don't have to really know or care about any of that if you
	// didn't understand it. What you need to know is that by calling the
	// Palletize() method on an image, you potentially can reduce the
	// amount of RAM it consumes by 4 times. The Palletize method
	// returns a boolean indicating if it could or couldn't be palletized.
	((DDImage*)mOpaqueBeamImg)->Palletize();
	((DDImage*)mMoonImg)->Palletize();
	((DDImage*)mTurbotImg)->Palletize();

	// Now let's load and create some fonts. A font consists of an 
	// image and a text file. The image works on the same principles
	// as a normal image file (like the ones above) works on. Except
	// that with fonts, there is only 1 image, the alpha image.
	// Thus, you will see in the "fonts" directory two images that
	// begin with the underscore prefix. If there isn't a file
	// of the same name without an underscore, then it is assumed that
	// the file is of the same dimensions as the underscore file and that
	// it is to be all white and fully opaque. This is done with fonts because
	// it's common to want to change the color of the font, and to change
	// the color, the base, original image color, should be white and
	// fully opaque. More information on colorizing fonts and images can
	// be found in Board.cpp, but for now know that the image in the fonts
	// directory contains the alpha information for your font, and that it
	// is assumed that the "main" image is pure white.
	//		The other file in the directory is a text file, commonly with the
	//	same name as the image, but without the underscore character, but
	//	not always. The file will define the name of the image to load.
	//	This file defines the characters, their widths, their offsets
	//	within the image, the point size, and any layers (which are not
	//	used or discussed in this demo). This is the data file and is
	//	how the font knows how to take a string and convert it into
	//	the proper images representing each character. A font is really
	//	just an image but with an extra data file that tells the program
	//	how to map strings to their image representation.
	//		You load a font by specifying the text data file.
	mTextFont = new ImageFont(this, "fonts/Kiloton9.txt");

	// We need to check to see if the font was properly initialized.
	// If it wasn't, then an error occurred and we need to abort.
	if (!mTextFont->mFontData->mInitialized)	
	{
		delete mTextFont;
		Popup("There was an error loading fonts/Kiloton9.txt");
		mLoadingFailed = true;
		return;
	}

	mNumberFont = new ImageFont(this, "fonts/supernova20.txt");
	if (!mNumberFont->mFontData->mInitialized)	
	{
		delete mNumberFont;
		Popup("There was an error loading fonts/supernova20.txt");
		mLoadingFailed = true;
		return;
	}

	// Let's load some sounds. You assign a unique unsigned integer ID to each
	// sound. It is with this ID that you indicate which sound you
	// want to play. Valid types of sounds to load are: 
	// WAV, OGG, AU, and if you have FMod and enable FMod: MP3. Although
	// you should probably not use MP3 due to patent/copyright issues
	// unless of course that either doesn't bother you or you happen
	// to have the legal right to do so. Like images, you don't have
	// to specify the file extension. LoadSound returns a boolean
	// indicating success or failure.
	if (!mSoundManager->LoadSound(1, "sounds/timer"))
	{
		Popup("There was an error loading sounds/timer");
		mLoadingFailed = true;		
		return;
	}

	if (!mSoundManager->LoadSound(2, "sounds/mutator"))
	{
		Popup("There was an error loading sounds/mutator");
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

	// Now that we're done loading everything we need (which wasn't
	// anything in this particular demo), we need to get the main
	// game screen up and running: That is our "Board" class, and
	// it will handle all the drawing, updating, and input processing
	// for most of the game.
	mBoard = new Board(this);

	// This is a very important step: Because the Board class is a widget
	// (see Board.h/.cpp for more details) we need to tell it what
	// dimensions it has and where to place it. 
	// By default a widget is invisible because its
	// width/height are 0, 0. Since the Board class is our main
	// drawing area and game logic class, we want to make it the
	// same size as the application. For this particular demo, that means
	// 800x600. We will use mWidth and mHeight though, as those were
	// already set to the proper resolution in GameApp::Init().
	mBoard->Resize(0, 0, mWidth, mHeight);

	// Also an important step is to add the newly created Board widget to
	// the widget manager so that it will automatically have its update, draw,
	// and input processing methods called.
	mWidgetManager->AddWidget(mBoard);

	// And just to test out our sound playing abilities, let's play the
	// mutator sound to indicate that we're done loading.
	//  We do that by calling GameApp's PlaySample()
	// method and specifying the integral ID of the sound to play. This
	// ID is the same as we used when loading the sound in GameApp::LoadingThreadProc()
	PlaySample(2);
}