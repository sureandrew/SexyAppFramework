#ifndef __XMLDEMOAPP_H__
#define __XMLDEMOAPP_H__

#include "SexyAppFramework/SexyAppBase.h"

namespace Sexy
{

class Board;

// This is the new class we're going to be learning about.
// It'll handle the loading and manipulation of XML files.
class XMLParser;

// These structs just represent the values from our properties/demo.xml
// file. In a real game, you'd have your own made up config file format.
// The names correspond to the attributes/sections from the XML file.
struct s_Section1
{
	SexyString	mItem1Text;
	
	SexyString  mBoolParamText;
	bool		mBoolParamElement;
};

struct s_Section2
{
	SexyString	mSectionAttribText;

	SexyString	mIntParamText;
	int			mIntParamElement;

	SexyString	mMultiAttrib1;
	SexyString	mMultiAttrib2;
	SexyString	mMultiAttrib3;

	SexyString		mSubsectionItem2;
};

class XMLDemoApp : public SexyAppBase
{

public:

	Board*			mBoard;
	s_Section1		mSection1;
	s_Section2		mSection2;
	SexyString		mInstructionTag;
	SexyString		mInstruction;

private:

	// With this class, we'll read in our sample XML file, properties/demo.xml
	XMLParser*		mParser;			

private:

	//////////////////////////////////////////////////////////////////////////
	// Function:	ExtractXMLData
	// Parameters:	none
	// Returns:		false on error, otherwise true
	//
	// Purpose: After opening an XML file via the XMLParser::OpenFile() method,
	// this function then extracts out the data it needs from the sample XML file.
	// When complete, mSection1 and mSection2 will have their values initialized.
	//////////////////////////////////////////////////////////////////////////
	bool	ExtractXMLData();

public:

	XMLDemoApp();

	virtual ~XMLDemoApp();

	virtual void		ShutdownHook();
	virtual void		InitHook();

	
	
};

}
#endif //__XMLDEMOAPP_H__
