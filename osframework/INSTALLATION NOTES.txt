The most common reason why the demos and source fail to build are due to the lack 
of required software to build them. Ensure that you have the following before building:

* The microsoft DirectX SDK (not just the runtime). 
	http://www.microsoft.com/directx

* The Windows Platform SDK (VS2005 Express does NOT include this by default but Pro does).
	http://www.microsoft.com/downloads/details.aspx?FamilyId=484269E2-3B89-47E3-8EB7-1F2BE6D7123A&displaylang=en
	Do not be alarmed by the title "Windows Server 2003 SP1 Platform SDK Web Install".
	This allows you to develop for not just Windows Server 2003 but Windows XP/x64/etc.

	You will then need to manually add the paths for the Platform SDK to your search paths in
	Visual Studio:

	Go to Tools->Options->Projects And Solutions->VC++ Directories
	Add the paths to the appropriate subsection:

	Executable files - C:\Program Files\Microsoft SDK\Bin 
	Include files - C:\Program Files\Microsoft SDK\include 
	Library files - C:\Program Files\Microsoft SDK\lib


* If you are using Visual Studio 2005 Express, make sure you add these additional libraries to your
	Linker's include settings:  gdi32.lib advapi32.lib shell32.lib

* If you are making your own projects and are getting errors that look something like this:
Linking...
LIBCMTD.lib(tolower.obj) : error LNK2005: __tolower already defined in LIBCD.lib(tolower.obj)
LIBCMTD.lib(tolower.obj) : error LNK2005: _tolower already defined in LIBCD.lib(tolower.obj)
LINK : warning LNK4098: defaultlib "LIBCMTD" conflicts with use of other libs; use /NODEFAULTLIB:library
Debug/Temp.exe : fatal error LNK1169: one or more multiply defined symbols found
Error executing link.exe.

then you need to add LIBCD to the ignored default libraries for debug mode and LIBC for release.