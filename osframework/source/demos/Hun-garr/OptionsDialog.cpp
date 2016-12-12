#include "OptionsDialog.h"
#include "Res.h"
#include "Board.h"
#include "SexyAppFramework/SexyAppBase.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/Font.h"
#include "SexyAppFramework/DialogButton.h"
#include "SexyAppFramework/Checkbox.h"
#include "SexyAppFramework/Slider.h"

using namespace Sexy;


OptionsDialog::OptionsDialog(Board* b) :
Dialog(IMAGE_DIALOG_BOX, IMAGE_DIALOG_BUTTON, OptionsDialog::DIALOG_ID, true, _S("OPTIONS"), _S(""), _S("CLOSE"), Dialog::BUTTONS_FOOTER)
{
	mContentInsets = Insets(23, 20, 23, 20);
	mSpaceAfterHeader = 30;

	mBoard = b;

	SetHeaderFont(FONT_DEFAULT);
	SetLinesFont(FONT_DEFAULT);
	SetButtonFont(FONT_DEFAULT);

	SetColor(COLOR_HEADER, Color::Black);
	SetColor(COLOR_LINES, Color::Black);

	mMusicVolumeSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, OptionsDialog::MUSIC_SLIDER_ID, this);
	mMusicVolumeSlider->SetValue(gSexyAppBase->GetMusicVolume());

	mSfxVolumeSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, OptionsDialog::SFX_SLIDER_ID, this);
	mSfxVolumeSlider->SetValue(gSexyAppBase->GetSfxVolume());
	
	mQuitBtn = new DialogButton(IMAGE_DIALOG_BUTTON, OptionsDialog::QUIT_BTN_ID, this);
	mQuitBtn->mLabel = _S("QUIT GAME");
	mQuitBtn->SetFont(FONT_DEFAULT);

	m3DCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, OptionsDialog::HARDWARE_CHECKBOX_ID, this);
	mFSCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, OptionsDialog::FS_CHECKBOX_ID, this);
	mCustomCursorsCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, OptionsDialog::FS_CHECKBOX_ID, this);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OptionsDialog::~OptionsDialog()
{
	delete mMusicVolumeSlider;
	delete mSfxVolumeSlider;
	delete mQuitBtn;
	delete mCustomCursorsCheckbox;
	delete m3DCheckbox;
	delete mFSCheckbox;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::Draw(Graphics* g)
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
	g->DrawString(_S("3D Mode:"), m3DCheckbox->mX - mX - 25, m3DCheckbox->mY - mY - m3DCheckbox->mHeight + 20);
	g->DrawString(_S("Full Screen:"), mFSCheckbox->mX - mX - 25, mFSCheckbox->mY - mY - mFSCheckbox->mHeight + 20);
	g->DrawString(_S("Custom Cursors:"), mCustomCursorsCheckbox->mX - mX - 25, 
					mCustomCursorsCheckbox->mY - mY - mCustomCursorsCheckbox->mHeight + 20);

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	Dialog::AddedToManager(theWidgetManager);
	theWidgetManager->AddWidget(mMusicVolumeSlider);
	theWidgetManager->AddWidget(mSfxVolumeSlider);
	theWidgetManager->AddWidget(mQuitBtn);

	int checkWidth = IMAGE_CHECKBOX->GetWidth() / 2;
	m3DCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	m3DCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	mFSCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	mFSCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	mCustomCursorsCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	mCustomCursorsCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	m3DCheckbox->mChecked = gSexyAppBase->Is3DAccelerated();
	mFSCheckbox->mChecked = !gSexyAppBase->mIsWindowed;
	mCustomCursorsCheckbox->mChecked = gSexyAppBase->mCustomCursorsEnabled;

	theWidgetManager->AddWidget(m3DCheckbox);
	theWidgetManager->AddWidget(mFSCheckbox);
	theWidgetManager->AddWidget(mCustomCursorsCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mMusicVolumeSlider);
	theWidgetManager->RemoveWidget(mSfxVolumeSlider);
	theWidgetManager->RemoveWidget(mQuitBtn);
	theWidgetManager->RemoveWidget(mCustomCursorsCheckbox);
	theWidgetManager->RemoveWidget(m3DCheckbox);
	theWidgetManager->RemoveWidget(mFSCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Dialog::Resize(theX, theY, theWidth, theHeight);

	mMusicVolumeSlider->Resize(theX + mContentInsets.mLeft + 50, 
		theY + 100, 245, IMAGE_SLIDER_THUMB->GetHeight());

	mSfxVolumeSlider->Layout(LAY_SameLeft | LAY_Below | LAY_SameWidth | LAY_SameHeight, 
		mMusicVolumeSlider, 0, 40, 0, 0);

	mQuitBtn->Layout(LAY_SameLeft | LAY_SameSize | LAY_Above, mYesButton);

	m3DCheckbox->Layout(LAY_SameLeft | LAY_Below, mSfxVolumeSlider, 0, 40, 0, 0);
	m3DCheckbox->Resize(m3DCheckbox->mX, m3DCheckbox->mY,
		IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

	mFSCheckbox->Layout(LAY_SameTop | LAY_SameWidth | LAY_SameHeight | LAY_Right, m3DCheckbox, 80);

	mCustomCursorsCheckbox->Layout(LAY_SameTop | LAY_SameWidth | LAY_SameHeight | LAY_Right, mFSCheckbox, 80);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::SliderVal(int theId, double theVal)
{
	if (theId == OptionsDialog::MUSIC_SLIDER_ID)
	{
		// Let's set the music volume to whatever the slider position is
		gSexyAppBase->SetMusicVolume(theVal);
	}
	else if (theId == OptionsDialog::SFX_SLIDER_ID)
	{
		// Set the sound value
		gSexyAppBase->SetSfxVolume(theVal);

		if (!mSfxVolumeSlider->mDragging)
			gSexyAppBase->PlaySample(SOUND_TIMER);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::ButtonDepress(int theId)
{
	Dialog::ButtonDepress(theId);

	// Previously, before framework 1.2, the line below looked like:
	// else if (theId == 2000 + mId)
	// With the 1.2 changes, there's just 1 slight incompatibility.
	// If the DialogListener for a dialog your app (by default), and
	// you don't implement your own DialogButton(De)press method (see V12Demo for
	// more info on this), and instead are trapping button presses within ButtonDepress
	// inside the dialog itself, then instead of the button ID being 2000 or 3000 + mId,
	// it's just 0 for no/cancel/1-button dialog, and 1 for yes/ok. Hence we changed our
	// check to look for 0, which is our default single close button at the bottom of this
	// dialog.
	if (theId == ID_YES)
	{
		// Close button pressed. Tell the app to remove ourselves. This
		// will automatically safe delete the dialog at a later time.
		// Let's apply the 3D and fullscreen mode settings first though.
		// We call SwitchScreenMode. The first parameter is whether or not to run
		// windowed (false means fullscreen), the second is whether or not to do 3d.
		gSexyAppBase->SwitchScreenMode(!mFSCheckbox->mChecked, m3DCheckbox->mChecked);

		gSexyAppBase->EnableCustomCursors(mCustomCursorsCheckbox->mChecked);

		gSexyAppBase->KillDialog(this);

		mBoard->OptionsDialogDone();
	}
	else if (theId == mQuitBtn->mId)
	{
		// Quit button pressed. Shut down the app.
		gSexyAppBase->Shutdown();
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::CheckboxChecked(int theId, bool checked)
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
				gSexyAppBase->DoDialog(OptionsDialog::MESSAGE_BOX_ID, true, _S("Not Supported"), 
					_S("Hardware acceleration can not be enabled on this computer. \nYour \
					video card does not meet the minimum requirements for this game."),
					_S("OK"), Dialog::BUTTONS_FOOTER);
			}
			else if(!gSexyAppBase->Is3DAccelerationRecommended())
			{
				// We can also check if 3D acceleration is not recommended for this computer
				// with a call to Is3DAccelerationRecommended. This allows the user to override
				// the default setting, but with a warning that it might not work or might cause
				// problems. Some cards fail the detection process but wind up being OK to use.
				gSexyAppBase->DoDialog(OptionsDialog::MESSAGE_BOX_ID, true, _S("Warning"), 
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
			gSexyAppBase->DoDialog(OptionsDialog::MESSAGE_BOX_ID, true, _S("No Windowed Mode"),
				_S("Windowed mode is only available if your desktop is running in\n\
				either 16 bit or 32 bit color mode, which it is not."), _S("OK"), Dialog::BUTTONS_FOOTER);

			// re-check the box to indicate that fullscreen is still the selected mode:
			mFSCheckbox->SetChecked(true);
		}
	}
}