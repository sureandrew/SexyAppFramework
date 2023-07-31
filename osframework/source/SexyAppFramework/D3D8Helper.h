#ifndef __D3D8HELPER_H__
#define __D3D8HELPER_H__

#include <windows.h>
#include <stdio.h>

namespace Sexy
{
	bool GetD3D8AdapterInfo(GUID &theGUID, std::string &theDriver, std::string &theDescription);
}

#endif