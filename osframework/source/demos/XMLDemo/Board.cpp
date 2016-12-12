#include "Board.h"
#include "XMLDemoApp.h"
#include "..\SexyAppFramework\Graphics.h"
#include "Res.h"

using namespace Sexy;

Board::Board(XMLDemoApp* theApp)
{
	mApp = theApp;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Board::~Board()
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Update()
{
	Widget::Update();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Board::Draw(Graphics* g)
{
	g->SetColor(Color::Black);
	g->FillRect(0, 0, mWidth, mHeight);
	
	g->SetFont(FONT_DEFAULT);
	g->SetColor(Color::White);

	// Just to verify things, print out the values of everything we cared to read in from demo.xml:

	g->DrawString(StrFormat(_S("Section 1/Item1 text: %s"), mApp->mSection1.mItem1Text.c_str()), 10, 20);
	g->DrawString(StrFormat(_S("Section 1/BoolParam text: %s"), mApp->mSection1.mBoolParamText.c_str()), 10, 40);
	g->DrawString(StrFormat(_S("Section 1/BoolParam value: %s"), mApp->mSection1.mBoolParamElement ? _S("true") : _S("false")), 10, 60);

	g->DrawString(StrFormat(_S("Section 2's s2attrib = %s"), mApp->mSection2.mSectionAttribText.c_str()), 10, 90);
	g->DrawString(StrFormat(_S("Section 2/IntParam text: %s"), mApp->mSection2.mIntParamText.c_str()), 10, 110);
	g->DrawString(StrFormat(_S("Section 2/IntParam value: %d"), mApp->mSection2.mIntParamElement), 10, 130);
	g->DrawString(StrFormat(_S("Section 2/MultiAttrib/attrib1 = %s"), mApp->mSection2.mMultiAttrib1.c_str()), 10, 150);
	g->DrawString(StrFormat(_S("Section 2/MultiAttrib/attrib2 = %s"), mApp->mSection2.mMultiAttrib2.c_str()), 10, 170);
	g->DrawString(StrFormat(_S("Section 2/MultiAttrib/attrib3 = %s"), mApp->mSection2.mMultiAttrib3.c_str()), 10, 190);
	g->DrawString(StrFormat(_S("Section 2/Subsection/Item2 value: %s"), mApp->mSection2.mSubsectionItem2.c_str()), 10, 210);
}

