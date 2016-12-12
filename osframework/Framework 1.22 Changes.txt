Framework 1.22 changes:

* Support for Visual Studio 2005

* Fixed some C++ language conformance issues

* Fixed some bugs with the flash widget and rendering issues

* Got tired of casting warnings, properly casting stuff now

* Fixed some demo bugs and bad programming practices

* IMPORTANT NEW CHANGE: in order to cut filesize bloat, caused
		by the addition of the software triangle rasterizer
		code in 1.21, you now need to do the following IF you
		use DrawImageMatrix or DrawImageTransform (with a
		complex transformation) or if you get errors compiling
		or at run time you get the assert "You need to call
		SWTri_AddDrawTriFunc or SWTri_AddAllDrawTriFuncs"

		1. Include "SexyAppFramework/SWTri.h"
		2. Call the function SWTri_AddAllDrawTriFuncs()
		3. Alternatively to #2, if you know which triangle draw
		functions you need, you can add them specifically via
		SWTri_AddDrawTriFunc(...) and specifying which draw
		function you need (check SWTri.h for a list).

* Removed the ImageLib project files as they aren't needed, to avoid confusion