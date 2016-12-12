#ifndef __BOARD_H__
#define __BOARD_H__

#include "SexyAppFramework/Widget.h"

namespace Sexy
{

class XMLDemoApp;

class Board : public Widget
{

public:

	XMLDemoApp* mApp;

public:

	Board(XMLDemoApp* theApp);
	virtual ~Board();

	void		Update();
	void		Draw(Graphics* g);
};

}

#endif //__BOARD_H__
