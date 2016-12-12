#ifndef __DEMO_WIDGET_H__
#define __DEMO_WIDGET_H__

#include "SexyAppFramework/Widget.h"
#include "SexyAppFramework/ButtonListener.h"

namespace Sexy
{

	class ButtonWidget;

	//////////////////////////////////////////////////////////////////////////
	//	Noting new in this header. Check the previous demos if this makes
	//	no sense to you, as it's all covered there. CHECK OUT THE .CPP FILE
	//	THOUGH, there's new stuff there.
	//////////////////////////////////////////////////////////////////////////
	

	class DemoWidget : public Widget, public ButtonListener
	{

	private:

		ButtonWidget*	mMoveButton;
		ButtonWidget*	mCloseButton;

	public:

		DemoWidget();
		virtual ~DemoWidget();

		void		Draw(Graphics* g);
		void		ButtonDepress(int id);
	};

}

#endif //__DEMO_WIDGET_H__
