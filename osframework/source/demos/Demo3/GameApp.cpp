#include "GameApp.h"
#include "Board.h"
#include "SexyAppFramework/WidgetManager.h"


// The Image.h file just declares basic functions. All images are either of 
// the DDImage or MemoryImage type. For this demo, we will use DDImage
// types, as they are the type returned by the image loading code.
// A DDImage is actually derived from MemoryImage, so where an Image or
// MemoryImage is required, a DDImage will suffice as well. A DDImage
// contains optimized code for use with DirectX 7+.
#include "SexyAppFramework/DDImage.h"

// We're going to create a font for Board's button widget's label
#include "SexyAppFramework/ImageFont.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::GameApp()
{
	// mProdName is used for internal purposes to indicate the game that we're working on
	mProdName = "Demo 3";

	// For internal uses, indicates the current product version
	mProductVersion = "1.0";

	// This is the text that appears in the title bar of the application window
	mTitle = StringToSexyStringFast("SexyAppFramework: " + mProdName + " - " + mProductVersion);

	// Indicates the registry location where all registry keys will be read from
	// and written to. This is stored under the HKEY_CURRENT_USER tree on 
	// Windows systems.
	mRegKey = "PopCap\\SexyAppFramework\\Demo3";

	// Set the application width/height in terms of pixels here. Let's
	// use a different resolution from Demo 1 just for fun.
	mWidth = 800;
	mHeight = 600;

	// By setting this to true, the framework will automatically check to see
	// if hardware acceleration can be turned on. This doesn't guarantee that it
	// WILL be turned on, however. Some cards just aren't compatible or have
	// known issues. Also, cards with less than 8MB of video RAM aren't supported.
	// There are ways to override the 3D enabled settings, which we will discuss
	// in a later demo. As a side note, if you want to see if you app is
	// running with 3D acceleration, first enable debug keys by pressing
	// CTRL-ALT-D and then press F9. To toggle 3D on/off, press F8. That is just
	// for testing purposes. 
	//
	// When 3D mode is on, the standard drawing routines will automatically use
	// their hardware rendering versions, which in truns makes the game run faster.
	// You do not need to do anything different when drawing in 2D or 3D mode.
	// Although if 3D mode is disabled, you will most likely want to do less
	// drawing intensive operations like additive drawing, colorization, 
	// real-time flipping/mirroring, etc.
	mAutoEnable3D = true;

	mBoard = NULL;
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
	delete mTurbotImg;
	delete mLightningImg;
	delete mAlteredImg;
	delete mFont;
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
	mTurbotImg = (DDImage*) GetImage("images/turbot_worry");

	// If the file was not found or couldn't be loaded (i.e. due to an
	// incompatible file format) the returned value will be NULL.
	// You should always check this, and if it occurs, display an error
	// message, then set mLoadingFailed to true, and then immediately return.
	if (mTurbotImg == NULL)
	{
		mLoadingFailed = true;

		// The PopUp method displays a standard Windows message box.
		// If in full screen mode, this will appropriately handle things such
		// that the GDI surface is properly rendered and the dialog box appears
		// as expected.
		Popup("There was an error loading the file: images/turbot_worry");

		return;
	}

	mLightningImg = (DDImage*) GetImage("images/lightning");
	if (mLightningImg == NULL)
	{
		mLoadingFailed = true;
		Popup("There was an error loading the file: images/lightning");
		return;
	}

	// There's something different about mLightningImg. If you look at the
	// image in the iamges directory, you'll notice that it has multiple frames
	// arranged in rows. This is an animation strip and is the main way that we
	// accomplish animation. Rather than putting each frame in a separate file,
	// we place it in one file and make each frame have the same width/height.
	// When loading, the image loader has no idea that we even want to consider
	// the image as being made of multiple frames. So we have to manually tell
	// it how many rows and/or columns it has. In this case, there is only 1
	// column, but there are 8 rows. We set this via mNumRows and mNumCols.
	// You'll see why this is important in the Board class.
	mLightningImg->mNumRows = 8;
	mLightningImg->mNumCols = 1;


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
	mLightningImg->Palletize();
	mTurbotImg->Palletize();

	// This is the same as in Demo2. Check the comments there
	// if you've forgotten how to load and initialize a font.
	mFont = new ImageFont(this, "fonts/Kiloton9.txt");

	// We need to check to see if the font was properly initialized.
	// If it wasn't, then an error occurred and we need to abort.
	if (!mFont->mFontData->mInitialized)	
	{
		delete mFont;
		mLoadingFailed = true;
		Popup("There was an error loading fonts/Kiloton9.txt");
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


	// As part of a new topic in this demo, we're also going to modify the image
	// data for mTurbotImg and make a grayscale version of it. I'll explain
	// each step of the way:

	// 1. Let's make a copy of the image so we don't ruin the original.
	// We should make sure to delete this when we're done.
	mAlteredImg = (DDImage*) CopyImage(mTurbotImg);

	// 2. Now we need to get the pixel data. The pixel data is stored as
	// an unsigned long array, where each entry represents the RGBA value.
	// The data is actually stored in ARGB format, where alpha is
	// the leftmost byte and blue is the rightmost byte.
	unsigned long* bits = mAlteredImg->GetBits();

	// 3. Now we will loop over each pixel in the image. The size of the bits array
	// is simply the width times the height.
	for (int i = 0; i < mAlteredImg->GetWidth() * mAlteredImg->GetHeight(); i++)
	{
		// 4. Get the ARGB color value for this pixel
		unsigned long c = bits[i];

		// 5. To illustrate the ARGB storage format, we will assign each
		// component to a variable, although we're actually only going to care
		// about the RGB values, for this particular example. The 4 lines below
		// extract out the individual ARGB values.
		unsigned char alpha		= (unsigned char) (c >> 24);
		unsigned char red		= (unsigned char) ((c >> 16) & 0xFF);
		unsigned char green		= (unsigned char) ((c >> 8) & 0xFF);
		unsigned char blue		= (unsigned char) (c & 0xFF);

		// 6. Just like the Color class, the ARGB values are from 0-255.
		// Let's alter these to produce a grayscale image using one of many
		// conversion methods. This method uses 30% of the red value,
		// 59% of the green value, and 11% of the blue value:
		unsigned long gray = (unsigned long) ((float)red * 0.30f + (float)green * 0.59f + (float)blue * 0.11f);

		// 7. Now we need to put the pixel data back into the image's data.
		// We do the opposite of how we extracted the ARGB values above and
		// use a left shift instead of a right shift:

		//		      alpha          red           green       blue
		bits[i] = (alpha << 24) | (gray << 16) | (gray << 8) | gray;
	}

	// The image won't use this modified data until we inform it that we've 
	// done some messing around with it. We do that with the BitsChanged()
	// function call. After that, we're all done! Pretty simple. It just
	// depends on what you want to actually do with the RGBA data. Extracting
	// the information and putting it back is as simple as a few shifts.
	mAlteredImg->BitsChanged();
}