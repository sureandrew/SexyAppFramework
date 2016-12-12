#include "XMLDemoApp.h"
#include "SexyAppFramework/WidgetManager.h"
#include "SexyAppFramework/ResourceManager.h"
#include "SexyAppFramework/XMLParser.h"
#include "Board.h"
#include "Res.h"

using namespace Sexy;

XMLDemoApp::XMLDemoApp()
{
	mProdName = "XMLDemo";
	mProductVersion = "1.0";
	mTitle = StringToSexyStringFast("SexyAppFramework: " + mProdName + " - " + mProductVersion);
	mRegKey = "PopCap\\SexyAppFramework\\XMLDemo";

	mBoard = NULL;
	mParser = NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
XMLDemoApp::~XMLDemoApp()
{
	delete mBoard;
	delete mParser;
	mResourceManager->DeleteResources("");

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void XMLDemoApp::ShutdownHook()
{
	if (mBoard != NULL)
		mWidgetManager->RemoveWidget(mBoard);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void XMLDemoApp::InitHook()
{
	LoadResourceManifest();

	// We're just going to wind up using the default font from this resource group.
	// Since there's very little to load, we'll dispense with the whole
	// title screen loading stuff.
	if (!mResourceManager->LoadResources("Init"))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	if (!ExtractInitResources(mResourceManager))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	// Create our XML Parser object
	mParser = new XMLParser();

	// By default, comments are ignored and you won't see them at all once the file is read in.
	// If you would like to not strip out the comments, you can call the AllowComments method
	// of the XMLParser class and pass in true. Uncomment the line below to let XML comments
	// be retained when loading the demo.xml file:
	// mParser->AllowComments(true);

	// Load in our demo XML file. If an error occurs, it'll return false.
	if (!mParser->OpenFile("properties/demo.xml"))
	{
		mLoadingFailed = true;

		// The parser will set an error string in the event that a problem occurred.
		// You get this string via a call to GetErrorText().
		MsgBox(StrFormat("Couldn't open properties/demo.xml. Error returned:\n\"%s\"", 
							mParser->GetErrorText().c_str()), "Error");
		return;
	}

	ExtractXMLData();

	mBoard = new Board(this);
	mBoard->Resize(0, 0, mWidth, mHeight);
	mWidgetManager->AddWidget(mBoard);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool XMLDemoApp::ExtractXMLData()
{
	// Everything will revolve around XML elements. We'll create
	// one on the stack and pass it to the NextElement function below.
	// The XMLElement class contains information on each element, such as
	// whether it's a comment, start of a tag, end of a tag, what its
	// attributes/values are, etc.
	XMLElement e;
		
	// We loop until there are no more elements left to process. NextElement
	// returns false when done.
	while (mParser->NextElement(&e))
	{
		// If you had enabled the option to not strip out comments (by default, comments
		// are removed when loading an XML file, see line 65 of this file for more info),
		// the type would be TYPE_COMMENT, the mSection field would be filled in as per usual,
		// the mValue field would be the comment declaration syntax, !--, and the entire comment
		// text (even if split across many lines) would be placed in mInstruction. Course, the
		// default setup for this demo is to ignore comments, so you won't see them at all.

		// TYPE_START denotes the start of a new section.
		// For every TYPE_START, there will eventually be a corresponding
		// TYPE_END with a matching mValue. For our current demo, we actually
		// don't care about the end of a tag.
		if (e.mType == XMLElement::TYPE_START)
		{	
			// The type stays as TYPE_START until another item type appears, such as the
			// end of a section/item, TYPE_ELEMENT is found (see a few paragraphs below), 
			// a comment is found, etc.
			// In the meantime, the mSection field contains the full name of the current
			// section.
			if (e.mSection == _S("Section1"))
			{
				// Once inside a section, each item is read in. When an item is read in,
				// it's tag is placed in the mValue field, and all attributes will be stored
				// in a map (mAttributes) keyed by the attribute's string name, with a result of
				// type std::string.
				if (e.mValue == _S("Item1"))
				{
					// At this point, we've read the line from our demo.xml file:
					// <Item1 text="Some text for Section1/Item1"/>
					// that is, assuming you didn't change the contents of that file.
					// As you can see, "Item1" is in the mValue field, and "text" is
					// the key in our mAttributes map, with "Some text for Section1/Item1" as its value.
					mSection1.mItem1Text = e.mAttributes[_S("text")];	
				}
				else if (e.mValue == _S("BoolParam"))
				{
					// You'll notice that in demo.xml the line containing this part of the XML code reads:
					// 	<BoolParam text="Some text for Section1/BoolParam">true</BoolParam>
					//
					// At this point in the processing though, we have only covered this part:
					// <BoolParam text="Some text for Section1/BoolParam">
					//
					// The "true" part will be covered a few paragraphs below where we check for TYPE_ELEMENT.
					// For now though, we can read in all parameters (in this case, just "text") stored
					// as part of this item.
					mSection1.mBoolParamText = e.mAttributes[_S("text")];
				}
			}
			else if (e.mSection == _S("Section2"))
			{
				if (e.mValue == _S("IntParam"))
					mSection2.mIntParamText = e.mAttributes[_S("text")];
				else if (e.mValue == _S("MultiAttrib"))
				{
					// If you check out the demo.xml file, you'll see an entry declared like this:
					// 
					// <MultiAttrib 
					// 	 attrib1="attrib1"
					//	 attrib2="2"
					//	 attrib3="woo! attrib3!"
					//	 />
					//
					//	Again, the tag is in mValue, and all attributes are stored as key/value pairs
					// in the mAttributes map. Note that the whitespace doesn't matter: you could
					// have declared the above line on just a single line if you wanted to.
					mSection2.mMultiAttrib1 = e.mAttributes[_S("attrib1")];
					mSection2.mMultiAttrib2 = e.mAttributes[_S("attrib2")];
					mSection2.mMultiAttrib3 = e.mAttributes[_S("attrib3")];
				}

			}
			else if (e.mValue == _S("Section2"))
			{
				// Confused about this part? Don't be: When the very first line of a section
				// is read in (in this case, it is: <Section2 s2attrib="Section2 attrib text">),
				// if it doesn't also end on the same line (which in this case it doesn't, we get
				// the </Section2> a few lines later) then mSection isn't updated and instead, the
				// section name is stored in mValue. In this case, you'll see that mSection is "".
				// That's because whitespace is ignored, so it's possible that this isn't a section 
				// and is perhaps just an item like the <MultiAttrib example above, split across several lines.
				// Since when a line is read in, if it contains any attributes they too are read in,
				// this is a good time to extract out the "s2attrib" attribute and record it in our mSection2
				// structure. 
				mSection2.mSectionAttribText = e.mAttributes[_S("s2attrib")];
			}

		}
		else if (e.mType == XMLElement::TYPE_ELEMENT)
		{
			// For lines of the form:
			// <BoolParam text="Some text for Section1/BoolParam"> true </BoolParam>
			// where there is data in between the start/end of the item (in this case, the data is "true"),
			// the item is considered to be of type TYPE_ELEMENT. An element looks like a section declaration,
			// except that it just contains a singular data value rather than nested XML syntax. For elements,
			// the section name is the name of the previous sections + the tag name of the element. So in the
			// above example, the section name is "Section1/BoolParam" since the previous (parent) section is
			// "Section1" and the tag is "BoolParam". The data is contained in the mValue field.

			if (e.mSection == _S("Section1/BoolParam"))
				mSection1.mBoolParamElement = StringToLower(e.mValue) == _S("false") ? false : true;
			else if (e.mSection == _S("Section2/IntParam"))
			{
				// This is just a handy std::string to integer function, that will return a boolean
				// indicating success or failure. I'm ignoring the return type though in this
				// particular case as I don't care for this particular example.
				(void)StringToInt(e.mValue, &mSection2.mIntParamElement);
			}
			else if (e.mSection == _S("Section2/Subsection/Item2"))
				mSection2.mSubsectionItem2 = e.mValue;
		}
	}

	return true;
}
