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

// The following are needed for some new widgets we're going to learn about:
#include "SexyAppFramework/EditWidget.h"
#include "SexyAppFramework/Checkbox.h"
#include "SexyAppFramework/ListWidget.h"
#include "SexyAppFramework/ScrollbarWidget.h"
#include "SexyAppFramework/ScrollListener.h"

// As part of our pitch shifted sound example, we'll need to 
// access the sound manager in our GameApp class as well as
// individual sound instances:
#include "SexyAppFramework/SoundManager.h"
#include "SexyAppFramework/SoundInstance.h"

// Used for file I/O:
#include "SexyAppFramework/Buffer.h"

// The SexyAppFramework resides in the "Sexy" namespace. As a convenience,
// you'll see in all the .cpp files "using namespace Sexy" to avoid
// having to prefix everything with Sexy::
using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::Board(GameApp* theApp)
{
	mApp = theApp;


	mButton1 = NULL;
	mButton2 = NULL;
	mEditWidget = NULL;
	mCheckboxWidget = NULL;
	mListWidget = NULL;
	mScrollbarWidget = NULL;

	mMotionX = mUpdateFMotionX = 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::~Board()
{
	delete mButton1;
	delete mButton2;
	delete mEditWidget;
	delete mCheckboxWidget;
	delete mListWidget;
	delete mScrollbarWidget;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Update()
{

	Widget::Update();

	// As an example to illustrate the difference between smooth motion via UpdateF
	// and the traditional motion via Update, we'll udpate the two MotionX variables
	// separately. 
	if ((mMotionX += 5.0f) >= mWidth)
		mMotionX = (float) -IMAGE_ROBOTROBOT->GetWidth();
	
	MarkDirty();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::UpdateF(float theFrac)
{
	// This is very similar to Update, except that since this function
	// gets called a different number of times depending on the refresh
	// rate of the monitor, "theFrac" can be a variable number, and we
	// need to multiply our normal motion change of 5.0 pixels per
	// update by "theFrac" to get our smooth motion. See the header
	// comment for this function in Board.h for more details.
	if ((mUpdateFMotionX += 5.0f * theFrac) >= mWidth)
		mUpdateFMotionX = (float) -IMAGE_ROBOTROBOT->GetWidth();

	// No need to MarkDirty: We're doing it in Board::Update already.
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Draw(Graphics* g)
{
	// Clear the screen to black
	g->SetColor(Color(0, 0, 0));
	g->FillRect(0, 0, mWidth, mHeight);

	// Draw the image first using the standard method, with the coordinate
	// updated via Board::Update as an example of non-smooth motion.
	// Let's also print some text too.
	g->SetFont(FONT_DEFAULT);
	g->SetColor(Color(255, 255, 255));
	g->DrawString(_S("Non smooth motion is jerky"), 10, 100);

	// What's this? A new DrawImage function? Yes. Believe it. It is true.
	// DrawImageF is just like it's best friend, DrawImage except that
	// it takes floating point values instead of integer ones. This is
	// slower than DrawImage, as the resulting image is anti-aliased to
	// give the illusion of moving at sub-pixel increments. A common
	// optimization technique at PopCap is to use DrawImageF 
	// for motion when the user has a supported 3D card and DrawImage if
	// the user has to run in software mode.
	g->DrawImageF(IMAGE_ROBOTROBOT, mMotionX, 120.0f);

	// Now let's draw the image but using the smooth motion amounts:
	g->DrawString(_S("Smooth motion is silky smoothness"), 10, 200);
	g->DrawImageF(IMAGE_ROBOTROBOT, mUpdateFMotionX, 220.0f);

	// Let's draw the currently selected list item:
	g->DrawString(mText, mListWidget->mX, mListWidget->mY + mListWidget->mHeight + 20);
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
	// go back and review. We're going to make two buttons which we'll use
	// to do some sound playing when clicked.
	mButton1 = new ButtonWidget(1, this);
	mButton1->Resize(5, 5, 100, 50);
	mButton1->SetFont(FONT_DEFAULT);
	mButton1->mLabel = _S("Sound Left");
	theWidgetManager->AddWidget(mButton1);

	mButton2 = new ButtonWidget(2, this);
	mButton2->Resize(106, 5, 100, 50);
	mButton2->SetFont(FONT_DEFAULT);
	mButton2->mLabel = _S("Sound Right");
	theWidgetManager->AddWidget(mButton2);

	// Let's set up our edit widget, shall we? The constructor is
	// just like it is with all widgets that emit messages: first parameter
	// is the ID to use, second is the class that will listen to its events.
	// We're reusing some IDs here because an edit widget isn't a button
	// widget and thus the IDs won't be confused. In your app however, you
	// will most likely just want to use unique enums or numbers to assign
	// to each specific widget.
	mEditWidget = new EditWidget(1, this);

	// We can't type in text if we don't have a font! Set one:
	mEditWidget->SetFont(FONT_DEFAULT);

	// Let's also set a maximum number of characters allowed. 
	// How about 15:
	mEditWidget->mMaxChars = 15;

	// If we wanted to emulate a password edit box, we could set
	// mEditWidget->mPasswordChar. Any character typed in would
	// then be visually replaced by mPasswordChar. We don't
	// want to do that in this demo though.
	
	mEditWidget->Resize(10, 300, 100, 15);
	theWidgetManager->AddWidget(mEditWidget);

	// Let's create a checkbox now. The checkbox widget does NOT
	// have any default drawing methods, unlike the other widgets.
	// You have to specify two images when creating a new one:
	// an unchecked and a checked image. There is another option though:
	// at PopCap it's common to place both the checked and unchecked image
	// into one image file. Observe how we handle this mind warp:
	mCheckboxWidget = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, 1, this);

	// No way! The check AND unchecked images are the same! Yes. Indeed.
	// But we're not done. Now, we need to tell the checkbox that
	// it has to use a different part of the image for checked and a different
	// part for unchecked. This is just like drawing a chunk of an image
	// by specifying a Rect. For this particular image, the first cel
	// contains the unchecked state and the second contains the checked one:
	int checkWidth = IMAGE_CHECKBOX->GetWidth() / 2;
	mCheckboxWidget->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	mCheckboxWidget->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	// Now add it and resize it
	mCheckboxWidget->Resize(200, 300, checkWidth, IMAGE_CHECKBOX->GetHeight());
	theWidgetManager->AddWidget(mCheckboxWidget);

	// Let's create our list box. The first parameter is the id, second
	// is the font to use, and third is who's going to listen to its events:
	mListWidget = new ListWidget(1, FONT_DEFAULT, this);

	// This next line instructs the list box to draw an outline around itself
	mListWidget->mDrawOutline = true;

	// Now we need to set up the colors to use for the listbox. We create
	// a 2D array containing RGB values, which we pass via SetColors.
	// An explanation of each of the colors is below:
	int listWidgetColors[][3] = 
	{
		{255, 255, 255},	//Background color
		{255, 0, 0},		//Outline color
		{0, 0, 0},			//Plain text not hilited
		{0, 0, 255},		//Text color when mouse is over text
		{128, 128, 128},	//Color of the bar displayed when an item is selected
		{190, 0, 80}		//Color of the text if it has the bar behind it and mouse is not over it
	};

	// Now we set the colors, and tell it that we're setting 6 of them
	mListWidget->SetColors(listWidgetColors, 6);

	// Let's create a scrollbar for the list widget.
	// The list widget is already a scrollbar listener, so we will
	// specify it as the widget responsible for dealing with the
	// scrollbar's messages. 
	mScrollbarWidget = new ScrollbarWidget(1, mListWidget);

	// We also need to let the list widget know that it actually has a scrollbar.
	// That way, it will automatically resize it as items are added/removed.
	// IMPORTANT: You should do this before resizing the widgets.
	mListWidget->mScrollbar = mScrollbarWidget;

	// Now resize the list widget
	mListWidget->Resize(300, 300, 100, 100);
	

	// Let's resize the scrollbar to the right of the list box. Instead of using
	// Resize(...), we use ResizeScrollbar to indicate that the entire scrollbar,
	// arrows and thumb included, are to be evenly resized. This will appropriately
	// scale all the items in the scrollbar.
	mScrollbarWidget->ResizeScrollbar(mListWidget->mX + mListWidget->mWidth, 
										mListWidget->mY,
										25,						// an arbitrary width for the bar itself
										mListWidget->mHeight);

	// Let's make the scrollbar invisible until there are enough items in the list box
	// to make them all not fit
	mScrollbarWidget->SetInvisIfNoScroll(true);

	theWidgetManager->AddWidget(mListWidget);
	theWidgetManager->AddWidget(mScrollbarWidget);

	// If you check out Board::EditWidgetText you'll see that we write out the
	// contents of the list box to disk every time something is added to it, or
	// it is cleared. For files that were created from a buffer, we can read
	// the data back into a buffer for ease of use.
	//
	//	IMPORTANT: You can NOT load files into a buffer if they were not
	//	created from a buffer in the first place. Thus, loading a random
	//	text or binary file is not a good idea, since the buffer will
	//	have no way to identify the datatypes beyond just a standard byte.
	//	Plase read Board::EditWidgetText for an explanation of writing
	//	with buffers.
	//
	// Let's load in the contents of that file, if it exists,
	// and repopulate the list box with it. The first step is to create
	// a Buffer object. The Buffer object will contain all the data 
	// in the file:
	Buffer buffer;

	// Check if the file exists:
	if (mApp->FileExists("list_items.dat"))
	{
		// Read in all the data from the file. It will be stored in a format
		// unique to the Buffer object:
		if (!mApp->ReadBufferFromFile("list_items.dat", &buffer))
		{
			// error, the file was corrupted or some other error occurred
			mApp->Popup("Could not read contents of list_items.txt");
		}

		// In Board::EditWidgetText we wrote out all the strings
		// via buffer.WriteString. The order in which data is written
		// to the buffer is the order in which it is read. Thus, if
		// you wrote out 2 integers, a string, and 10 booleans, when
		// reading from the buffer you'd first ask for the 2 integers,
		// then the string, then the 10 booleans. This is important:
		// the order DOES matter. If you asked to read a string when
		// a long was actually the next element in the file, you'd
		// get an error. Buffers are very useful for userprofiles or
		// data files like that where you can guarantee an explicit
		// file format.

		while (!buffer.AtEnd())
			mListWidget->AddLine(StringToSexyStringFast(buffer.ReadString()), true);
	}

	// If you read Board::ListClicked, you'll see that we
	// wrote the last selected item from the list box to the registry.
	// Let's read that value in, if it exists, and set the mText
	// variable to it so that it displays on screen. We use the
	// RegsitryRead... functions to grab values from the regsitry.
	// The values are assumed to be in the registry location that you
	// set by setting the mRegKey variable in GameApp's constructor.
	// The functions return false if there was an error reading the
	// key or the key doesn't exist:
	mApp->RegistryReadString("ListItem", &SexyStringToStringFast(mText));
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::RemovedFromManager(WidgetManager* theWidgetManager)
{
	// This is called after we've been removed from the widget manager.
	// Again, we should let our base class do anything it needs to, first.
	Widget::RemovedFromManager(theWidgetManager);

	// We should now also remove any widgets we are responsible for. 
	theWidgetManager->RemoveWidget(mButton1);
	theWidgetManager->RemoveWidget(mButton2);
	theWidgetManager->RemoveWidget(mEditWidget);
	theWidgetManager->RemoveWidget(mCheckboxWidget);
	theWidgetManager->RemoveWidget(mListWidget);
	theWidgetManager->RemoveWidget(mScrollbarWidget);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::ButtonDepress(int theId)
{
	if (theId == 1)
	{
		// Our "left" button was clicked. Let's play a sound 
		// in the left speaker with a slight pitch shift.
		// In order to play a pitch shifted sample, we have to do more
		// than just say "PlaySample." We have to get a pointer to the sound
		// instance that represents our sound effect. We do that by asking the
		// app's sound manager to return us a sound instance, and we tell it
		// the ID of the sound file that we want. Let's do that now,
		// using the sound "SOUND_MUTATOR" which we set up in properties/resources.xml:
		SoundInstance* sample = mApp->mSoundManager->GetSoundInstance(SOUND_MUTATOR);

		// It's good to make sure the sample isn't NULL. It would be NULL if you
		// specified an invalid sound id. 
		if (sample != NULL)
		{
			//Now we actually adjust the pitch. Specify the number of
			//steps to raise (positive) or lower (negative) the original sound by.
			//We'll just arbitrarily choose 13.
			sample->AdjustPitch(13);

			//Let's make it play on the left speaker only. We set a panning value
			//in decibels, which for DirectX range from -10000 to +10000, where
			//-10000 is fully left and +10000 is fully right:
			sample->SetPan(-10000);
			
			// Now we have the sample play. This is again slightly different than
			// our PlaySample from previous demos. The first parameter indicates
			// whether or not we want the sample to loop (in this case, no), and
			// the second indicates whether the memory taken up by this special
			// sound instance pointer should be reclaimed when the sample is done
			// playing. If true, then the pointer will be invalid once its
			// done playing, sine the memory will be reclaimed. If false, the
			// memory won't be reclaimed and you'll have to do it yourself.
			sample->Play(false, true);
		}
	}
	else if (theId == 2)
	{
		// Let's do the same as we did for the left button, except make it
		// play on the right speaker and pitch shift it down
		SoundInstance* sample = mApp->mSoundManager->GetSoundInstance(SOUND_MUTATOR);
		if (sample != NULL)
		{
			sample->AdjustPitch(-5);
			sample->SetPan(10000);
			sample->Play(false, true);
		}
	}


}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::EditWidgetText(int theId, const std::string& theString)
{
	// This is called when enter is pressed. Let's add the string
	// to the list box and tell it to sort it alphabetically, if the string
	// isn't blank.

	if (theString.length() > 0)
	{
		// Let's also clear everything out of the list box if the user types
		// in the word "CLEAR". StringToUpper is found in Common.h and returns
		// the uppercase version of the string.
		if (StringToUpper(theString) == "CLEAR")
			mListWidget->RemoveAll();
		else
			mListWidget->AddLine(StringToSexyStringFast(theString), true);

		// Now clear the edit box
		mEditWidget->SetText(_S(""));

		// As an example of saving data to files, let's write out the contents
		// of the list box. Instead of using the C or C++ way of 
		// file I/O, we use buffers. As you'll see in a later demo,
		// buffers are useful for the demo playback system and are a powerful
		// feature of the framework. Plus, they make file I/O easy. The first
		// step is to make a buffer:
		Buffer buffer;
		
		// And now all we do is say WriteString. There are other methods
		// for writing other data types too:
		for (unsigned int i = 0; i < mListWidget->mLines.size(); i++)
			buffer.WriteString(SexyStringToStringFast(mListWidget->mLines.at(i)));

		// Now we need to write the buffer to disk. GameApp can help us out with
		// that. This data is saved in a special binary format that allows
		// us to read the data easily back into another buffer later.
		mApp->WriteBufferToFile("list_items.dat", &buffer);

		// That's it!
		
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool Board::AllowChar(int theId, char theChar)
{
	// As an example of denying input, let's prevent the user
	// from typing in the following: :-+.@#$%^&*()
	switch (theChar)
	{
		case ':':
		case '-':
		case '+':
		case '.':
		case '@':
		case '#':
		case '$':
		case '%':
		case '^':
		case '&':
		case '*':
		case '(':
		case ')':
			return false;

		default:
			return true;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::ListClicked(int theId, int theIdx, int theClickCount)
{
	if (theId == 1)
	{
		// Actually select the index. This is done to allow us to
		// block the selection, if we chose to.
		mListWidget->SetSelect(theIdx);

		// And update the text that's displaying on screen...
		// The strings are stored in the list widget's mLines variable
		// and the one clicked can be indexed via theIdx.
		mText = mListWidget->mLines[theIdx];

		// As an example of writing to the registry, let's write this value
		// and later on load it in when we restart the app. You'll notice
		// a bunch of RegistryWrite... functions in SexyAppBase. Each one
		// of these takes as first argument the name of the value to 
		// add to the registry. The second argument is the actual data
		// to store in that value. The value is saved under the registry
		// location that you set in GameApp's constructor when you
		// set the mRegKey variable. The function returns false
		// if there was an error, such as a lack of permission:
		if (!mApp->RegistryWriteString("ListItem", SexyStringToStringFast(mText)))
			mApp->Popup("Couldn't save \"ListItem\" to registry");
	}
}