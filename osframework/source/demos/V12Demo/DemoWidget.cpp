#include "DemoWidget.h"
#include "Res.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ButtonWidget.h"
#include "V12DemoApp.h"

using namespace Sexy;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
DemoWidget::DemoWidget()
{

	// Just for the heck of it, we'll resize ourselves at instantiation time.
	int w = 200, h = 200;
	Resize(gSexyAppBase->mWidth / 2 - w / 2, gSexyAppBase->mHeight / 2 - h / 2, w, h);

	// Previously, it was annoying trying to place widgets on some sort of parent widget, 
	// since there was no notion of parent/child relationship. What you had to do was
	// override the AddedToManager and RemovedFromManager functions, create and add your
	// widgets or remove and nuke them, and in the case of AddedToManager, you also had
	// to then place the widget using global coordinates that had no relation to the coordinates
	// you set the parent widget at. Not anymore. What we can do now is to dispense with
	// overriding those methods altogether! So now, in the parent's constructor, we can
	// create our child widgets, resize them, and place them using relative coordinates.
	// What this also means is that moving the parent widget around results in moving
	// ALL the child widgets, which means you no longer have to manually move everything
	// just because the parent moved. In addition, another nice thing is that you no longer
	// have to remove your child widgets before deleting them: this is handled automatically
	// for you.
	mMoveButton = new ButtonWidget(0, this);
	mMoveButton->mLabel = _S("MOVE");
	mMoveButton->SetFont(FONT_DEFAULT);
	
	mCloseButton = new ButtonWidget(1, this);
	mCloseButton->mLabel = _S("CLOSE");
	mCloseButton->SetFont(FONT_DEFAULT);	

	// VERY IMPORTANT: Notice that we're calling THIS CLASS' (or really, it's parent, WidgetContainer's)
	// AddWidget method instead of the WidgetManager's method. In order to designate a widget as a child
	// widget, you have to call the AddWidget method of the class that will be its parent.
	AddWidget(mMoveButton);
	AddWidget(mCloseButton);

	mMoveButton->Resize(10, 150, 75, 50);
	mCloseButton->Resize(115, 150, 75, 50);

	// mPriority determines what..uh..priority...this widget (and thus its children)
	// draws at when another widget is drawing an overlay (please see Board.cpp
	// about the overlay stuff if you haven't already, in Board::Draw(...)).
	// The higher priority widgets get drawn ABOVE (and thus AFTER) widgets
	// with a lower priority. Thus, by changing the priority of a widget,
	// you could enforce that it's always drawn above any overlay layer, 
	// or you could do just the opposite. We'll
	// set it to 1, since in our Board::Draw method, you'll see that we 
	// toggle between drawing the overlay above and below this widget.
	// Note that widgets by default have priority 0 and dialogs have priority 1.
	mPriority = 1;

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
DemoWidget::~DemoWidget()
{
	// We need to remove child widgets before deleting them.
	RemoveAllWidgets();

	delete mMoveButton;
	delete mCloseButton;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoWidget::ButtonDepress(int id)
{
	if (id == mMoveButton->mId)
	{
		// See how easy it is to move widgets now, using the new hierarchy system?
		// By calling Move (or Resize as well), we also move ALL our child widgets,
		// preserving their relative positions. Previously, you would have had to
		// manually move all child widgets yourself.
		Move(Rand() % (gSexyAppBase->mWidth - mWidth), Rand() % (gSexyAppBase->mHeight - mHeight));
	}
	else if (id == mCloseButton->mId)
	{
		// We'll remove ourselves when the close button is pressed.
		gSexyAppBase->mWidgetManager->RemoveWidget(this);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DemoWidget::Draw(Graphics* g)
{
	g->SetColor(Color(0, 255, 0, 200));
	g->FillRect(0, 0, mWidth, mHeight);


}