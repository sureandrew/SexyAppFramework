#include "DemoDialog.h"
#include "Res.h"
#include "GameApp.h"
#include "SexyAppFramework/SexyAppBase.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/Font.h"
#include "SexyAppFramework/DialogButton.h"
#include "SexyAppFramework/Checkbox.h"

// We're going to use a slider widget to control sound/music volume
#include "SexyAppFramework/Slider.h"

using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//	Let's do a little explanation of the parent Dialog class, shall we?
//	The first parameter we pass it is an image to use for the dialog box.
//
//	This image has to have the property that its width and height be
//	divisible by 3. Why? For stretching/tiling, in case you want the
//	dialog box to be bigger than the actual image. The image can
//	be thought of as being divided into regions. The 4 corners
//	are drawn normally, and the remaining chunks (which comprise a third
//	of the image height or the image width) are tiled to allow for 
//	the stretching of the dialog box.
//
//	The image for the dialog button works the same way and must have
//	the same properties. Dialogs use a DialogButton class which is exactly
//	the same as a normal button class, except that they support tiling
//	the image if it is stretched larger than the original image is.
//
//	The third parameter is the ID to use for the dialog box. 
//
//	The fourth indicates if the dialog is modal. If it is, then only the
//	dialog box and any widget above it (like its buttons) will receive
//	keyboard and mouse input. This is just like in Windows when a file
//	dialog appears and you can't click on the window of the application
//	below it.
//
//	The fifth parameter is the header to use for the dialog box.
//
//	The sixth parameter is the text to display in the body of the dialog
//	box. You can use the newline (\n) character for line breaks.
//
//	The seventh parameter is optional, and is only used if the eigth
//	parameter is Dialog::BUTTONS_FOOTER. This is the text to display on
//	the footer (bottom) dialog button.
//
//	The eigth parameter indicates what type of buttons are to display.
//	You can use BUTTONS_YES_NO or BUTTONS_OK_CANCEL for the standard
//	"Yes", "No" or "OK", "Cancel" options, BUTTONS_NONE for no buttons,
//	or BUTTONS_FOOTER for a single button that runs along the bottom.
//	Of course, you can change the labels on any of the buttons if you
//	like.
//
//	The YES/OK/FOOTER buttons have a value of 2000 + the dialog's ID.
//	The NO/CANCEL buttons have a value of 3000 + the dialog's ID.
//////////////////////////////////////////////////////////////////////////
DemoDialog::DemoDialog(std::string theHeader, std::string theBody) :
Dialog(IMAGE_DIALOG_BOX, IMAGE_DIALOG_BUTTON, DemoDialog::DIALOG_ID, true, StringToSexyStringFast(theHeader), StringToSexyStringFast(theBody), _S("CLOSE"), Dialog::BUTTONS_FOOTER)
{
	// We can set the rectangular region in which all text/buttons are to go
	// by changing the mContentInsets variable. The first parameter
	// is the distance from the left side of the dialog box, the second
	// is the distance from the top, third is distance from the right,
	// and fourth is distance from the bottom in which content
	// can be placed.
	mContentInsets = Insets(23, 20, 23, 20);

	// We can then set a value for how many pixels below the inset top
	// the text body should go:
	mSpaceAfterHeader = 30;

	// Let's also set the fonts to use for the header and the text. We should
	// use the SetXXXFont functions instead of directly modifying the font because
	// the dialog class makes a copy of the font and then deletes it in its destructor.
	// This is due to backwards compatibility with our old Java framework.
	SetHeaderFont(FONT_DEFAULT);
	SetLinesFont(FONT_DEFAULT);
	SetButtonFont(FONT_DEFAULT);

	// We can also change the color of the text
	SetColor(COLOR_HEADER, Color::Black);
	SetColor(COLOR_LINES, Color::Black);

	// Now let's create some sliders to control the music and sound volume with.
	// The first parameter is the image to use for the track of the slider. The
	// second is the image to use for the little movable "thumb" icon.
	// The third is the ID and the fourth is the widget who wants to listen to
	// the slider.
	mMusicVolumeSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, DemoDialog::MUSIC_SLIDER_ID, this);

	// Let's set the value of the music slider to what the current volume is.
	// SexyAppBase has a GetMusicVolume function that returns this amount.
	// But wait, we didn't get a SexyAppBase pointer passed into the constructor!
	// Don't worry. gSexyAppBase is externed in SexyAppBase.h and we can use that instead.
	mMusicVolumeSlider->SetValue(gSexyAppBase->GetMusicVolume());

	mSfxVolumeSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, DemoDialog::SFX_SLIDER_ID, this);

	// Let's set the sound volume to the current volume, just like we did with the music slider
	mSfxVolumeSlider->SetValue(gSexyAppBase->GetSfxVolume());

	// Let's make a button to show off how to use random numbers
	mRandomBtn = new DialogButton(IMAGE_DIALOG_BUTTON, DemoDialog::RANDOM_BTN_ID, this);
	mRandomBtn->mLabel = _S("Random #");
	mRandomBtn->SetFont(FONT_DEFAULT);

	// To illustrate the mClip example, we'll use a button to toggle it.
	mClipBtn = new DialogButton(IMAGE_DIALOG_BUTTON, DemoDialog::CLIP_BTN_ID, this);
	mClipBtn->mLabel = _S("Clipping");
	mClipBtn->SetFont(FONT_DEFAULT);

	m3DCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, DemoDialog::HARDWARE_CHECKBOX_ID, this);
	mFSCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, DemoDialog::FS_CHECKBOX_ID, this);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
DemoDialog::~DemoDialog()
{
	delete mMusicVolumeSlider;
	delete mSfxVolumeSlider;
	delete mRandomBtn;
	delete mClipBtn;
	delete m3DCheckbox;
	delete mFSCheckbox;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::Draw(Graphics* g)
{
	Dialog::Draw(g);

	// Let's draw some labels for the sliders
	g->SetFont(FONT_DEFAULT);
	g->SetColor(Color::Black);

	// We draw the X/Y at the slider's X/Y but we subtract off the dialog box's
	// X/Y's from that. Why? The slider's XY's are in absolute screen coordinates,
	// as are the dialog boxes. But as you may recall from Board::Draw(), where
	// we explain graphics translation, once we get inside the draw loop of a
	// widget, the graphics object is already moved (translated) to the widget's
	// coordinates. Thus, in using the Graphics object, g, (0, 0) is located
	// at the top left corner of the widget (our dialog box). So we
	// just take the difference between the widget locations to produce the 
	// actual location we want to drawour text at:
	g->DrawString(_S("Music volume:"), mMusicVolumeSlider->mX - mX, 
			mMusicVolumeSlider->mY - mY - mMusicVolumeSlider->mHeight);

	g->DrawString(_S("Sound volume:"), mSfxVolumeSlider->mX - mX, 
			mSfxVolumeSlider->mY - mY - mSfxVolumeSlider->mHeight);

	// Do the same for the checkboxes:
	g->DrawString(_S("3D Mode:"), m3DCheckbox->mX - mX, m3DCheckbox->mY - mY - m3DCheckbox->mHeight + 20);
	g->DrawString(_S("Full Screen:"), mFSCheckbox->mX - mX, mFSCheckbox->mY - mY - mFSCheckbox->mHeight + 20);

	// As an example of clipping and how to disable it, let's draw
	// a box and some text outside of the X/Y/Width/Height of this
	// dialog box. You'll notice that when mClip is true (default),
	// that anything drawn outside the bounds of the widget is clipped, or
	// ignored. When mClip is false, no clipping is performed and we
	// can draw anywhere.
	
	g->SetColor(Color(255, 0, 0, 128));	
	// Draw a box in the bottom right
	g->FillRect(mWidth, mHeight, 100, 100);
	// Draw one in the top left
	g->FillRect(-100, -100, 100, 100);
	g->SetColor(Color::Black);
	g->DrawString(_S("Top left"), -90, -20);
	g->DrawString(_S("Bottom right"), mWidth + 10, mHeight + 20);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::Update()
{
	Dialog::Update();
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	Dialog::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mMusicVolumeSlider);
	theWidgetManager->AddWidget(mSfxVolumeSlider);
	theWidgetManager->AddWidget(mRandomBtn);
	theWidgetManager->AddWidget(mClipBtn);

	int checkWidth = IMAGE_CHECKBOX->GetWidth() / 2;
	m3DCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	m3DCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	mFSCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	mFSCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	
	// Let's set the initial checked state of the 3d checkbox to true if the
	// user has 3D support enabled. We determine that via a call to Is3DAccelerated
	m3DCheckbox->mChecked = gSexyAppBase->Is3DAccelerated();
	
	// We can check if the app is fullscreen by examining the mIsWindowed variable:
	mFSCheckbox->mChecked = !gSexyAppBase->mIsWindowed;

	theWidgetManager->AddWidget(m3DCheckbox);
	theWidgetManager->AddWidget(mFSCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mMusicVolumeSlider);
	theWidgetManager->RemoveWidget(mSfxVolumeSlider);
	theWidgetManager->RemoveWidget(mRandomBtn);
	theWidgetManager->RemoveWidget(mClipBtn);
	theWidgetManager->RemoveWidget(m3DCheckbox);
	theWidgetManager->RemoveWidget(mFSCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Dialog::Resize(theX, theY, theWidth, theHeight);

	// Layout the music slider so that it's left aligned with the
	// boundary we set for the content insets. We'll make the slider fill the width
	// of the content region (it'll be stretched if the base image isn't
	// wide enough) too.
	mMusicVolumeSlider->Resize(theX + mContentInsets.mLeft, 
								theY + 140,
								mWidth - mContentInsets.mLeft - mContentInsets.mRight,
								IMAGE_SLIDER_THUMB->GetHeight());

	// Let's align the sound effect slider in the same way but using a different method.
	// We know that we want the SFX slider to have the same X, width, and height, and
	// to be just a few pixels below the music slider. Instead of having to mess around
	// with the settings like we did above, we can use the layout (LAY_) flags instead.
	// The first parameter is an ORed list of layout parameters and should be 
	// pretty self explanatory. You can find more in Widget.h.
	// The second parameter is the widget to use as the reference. 
	// The rest are any extra padding to use for left/top/width/height related
	// layout flags. As you can see, we wanted 60 pixels added to the top of 
	// our sound effect slider.
	mSfxVolumeSlider->Layout(LAY_SameLeft | LAY_Below | LAY_SameWidth | LAY_SameHeight, 
								mMusicVolumeSlider, 0, 40, 0, 0);

	// Place the button that will show off random number generation under the sound effect
	// slider. But let's only make it take up 1/2 of the width of the slider
	// and be only as tall as the original button image:
	mRandomBtn->Layout(LAY_SameLeft | LAY_SameTop, mSfxVolumeSlider, 0, 40, 0, 0);
	mRandomBtn->Resize(mRandomBtn->mX, mRandomBtn->mY, 
						mSfxVolumeSlider->mWidth / 2, IMAGE_DIALOG_BUTTON->mHeight);

	// And let's place the clipping toggle button to its right
	mClipBtn->Layout(LAY_Right | LAY_SameTop | LAY_SameWidth | LAY_SameHeight,
						mRandomBtn, 0, 0, 0, 0);

	// Place the 3D checkbox under the random button:
	m3DCheckbox->Layout(LAY_SameLeft | LAY_Below, mRandomBtn, 0, 40, 0, 0);
	m3DCheckbox->Resize(m3DCheckbox->mX, m3DCheckbox->mY,
						IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

	// And the fullscreen box to the right of the 3d one. Except, let's use
	// the mClipBtn's X coordinate to align with:
	mFSCheckbox->Layout(LAY_SameTop | LAY_SameWidth | LAY_SameHeight, m3DCheckbox);
	mFSCheckbox->Layout(LAY_SameLeft, mClipBtn);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::SliderVal(int theId, double theVal)
{
	if (theId == DemoDialog::MUSIC_SLIDER_ID)
	{
		// Let's set the music volume to whatever the slider position is
		gSexyAppBase->SetMusicVolume(theVal);
	}
	else if (theId == DemoDialog::SFX_SLIDER_ID)
	{
		// Set the sound value
		gSexyAppBase->SetSfxVolume(theVal);

		// A good idea is to play a "ding" or some similar sound when
		// the thumb is released, to indicate to the user what the current
		// sound volume is (since sounds might not be continuously playing,
		// unlike the music). We check for this by seeing if the mDragging
		// variable is true or not.
		if (!mSfxVolumeSlider->mDragging)
			gSexyAppBase->PlaySample(SOUND_TIMER);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::ButtonDepress(int theId)
{
	Dialog::ButtonDepress(theId);

	if (theId == mRandomBtn->mId)
	{
		// Let's get a random integer. The Rand() function is declared
		// in Common.h and returns a value from 0 to SEXY_RAND_MAX (2,147,483,647)
		int r = Rand();

		// Now let's change the text of the dialog box:
		mDialogLines = StrFormat(_S("Random number: %d"), r);
	}
	else if (theId == mClipBtn->mId)
	{
		// Toggle mClip. If mClip is off, we can draw outside the X/Y/width/height of
		// our dialog box.
		mClip = !mClip;
		if (mClip)
			mClipBtn->mLabel = _S("Clipping");
		else
			mClipBtn->mLabel = _S("No clipping");
	}
	// Previously, before framework 1.2, the line below looked like:
	// else if (theId == 2000 + mId)
	// With the 1.2 changes, there's just 1 slight incompatibility.
	// If the DialogListener for a dialog your app (by default), and
	// you don't implement your own DialogButton(De)press method (see V12Demo for
	// more info on this), and instead are trapping button presses within ButtonDepress
	// inside the dialog itself, then instead of the button ID being 2000 or 3000 + mId,
	// it's just ID_YES/ID_OK for ok/cancel/1-button dialog, and ID_NO/ID_CANCEL for no/cancel 2nd button. 
	// Hence we changed our check to look for ID_YES, which is our default single close button at the bottom of this
	// dialog.
	else if (theId == ID_YES)
	{
		// Close button pressed. Tell the app to remove ourselves. This
		// will automatically safe delete the dialog at a later time.
		// Let's apply the 3D and fullscreen mode settings first though.
		// We call SwitchScreenMode. The first parameter is whether or not to run
		// windowed (false means fullscreen), the second is whether or not to do 3d.
		gSexyAppBase->SwitchScreenMode(!mFSCheckbox->mChecked, m3DCheckbox->mChecked);

		gSexyAppBase->KillDialog(this);

		// Set focus back to the board
		((GameApp*)gSexyAppBase)->SetFocusToBoard();
	}
	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoDialog::CheckboxChecked(int theId, bool checked)
{
	// We'll wait until the dialog box is closed before actually applying any effects,
	// since it's rather jarring if as soon as a user clicks the 3d or fullscreen
	// toggle buttons to change right then and there.

	if (theId == m3DCheckbox->mId)
	{
		if (checked)
		{
			// Turn on 3D acceleration. But we need to check if the user is
			// even allowed to have it on. Some cards are just not compatible
			// with the framework (less than 8MB RAM for instance):
			if (!gSexyAppBase->Is3DAccelerationSupported())
			{
				// It's not supported. Don't let the checkbox get checked.
				// Display an error dialog to the user to let them know why this happened.
				// The reason we don't use a Popup or Windows-based message box here is because
				// if the user was in full screen mode, they might not see the error message. Using
				// a game dialog box is the safest way to warn them.
				m3DCheckbox->SetChecked(false);
				gSexyAppBase->DoDialog(DemoDialog::MESSAGE_BOX_ID, true, _S("Not Supported"), 
							_S("Hardware acceleration can not be enabled on this computer. \nYour\
							video card does not meet the minimum requirements for this game."),
							_S("OK"), Dialog::BUTTONS_FOOTER);
			}
			else if(!gSexyAppBase->Is3DAccelerationRecommended())
			{
				// We can also check if 3D acceleration is not recommended for this computer
				// with a call to Is3DAccelerationRecommended. This allows the user to override
				// the default setting, but with a warning that it might not work or might cause
				// problems. Some cards fail the detection process but wind up being OK to use.
				gSexyAppBase->DoDialog(DemoDialog::MESSAGE_BOX_ID, true, _S("Warning"), 
					_S("Your video card may not fully support this feature.\n\
					If you experience slower performance, please disable Hardware Acceleration."),
					_S("OK"), Dialog::BUTTONS_FOOTER);
			}
			
		}
	}
	else if (theId == mFSCheckbox->mId)
	{
		// Believe it or not, some people are not allowed to play in windowed mode.
		// You can only run in windowed mode if your desktop is 16 or 32 bit, and
		// if the resolution of their desktop is GREATER THAN the resolution of the
		// game. It doesn't make sense to run a 800x600 game windowed when your desktop
		// is 800x600 or less.
		// We can determine if the user is not allowed to run in windowed mode by
		// checking the value of SexyAppBase's mForceFullScreen variable.
		if (gSexyAppBase->mForceFullscreen && !checked)
		{
			gSexyAppBase->DoDialog(DemoDialog::MESSAGE_BOX_ID, true, _S("No Windowed Mode"),
			_S("Windowed mode is only available if your desktop is running in\n\
			either 16 bit or 32 bit color mode, which it is not."), _S("OK"), Dialog::BUTTONS_FOOTER);

			// re-check the box to indicate that fullscreen is still the selected mode:
			mFSCheckbox->SetChecked(true);
		}
	}
}