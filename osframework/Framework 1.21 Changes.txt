Framework 1.21 changes:

Widgets are no longer removed from their container in WidgeContainer::RemovedFromManager.  
It's your responsibility to remove all widgets from a container before deleting the container.  
Widgets will assert if they have a parent or if they have haven children in their destructor.
To assist you, there's a helper method WidgetContainer::RemoveAllWidgets().

Added LinearBlending for DrawImageMatrix/DrawImageTransform.  
Call Graphics::SetLinearBlend to enable/disable linear blending.

Added good default faststretch and linear blend settings to WidgetManager drawing:
	aClipG.SetFastStretch(!is3D);
	aClipG.SetLinearBlend(is3D);

Changed 3d test to not require "nvidia", "ati", or "radeon" in the name of the driver 
for recommending the video card if the video card driver's year is >= 2002.

Changed Dialog::mYesButton and Dialog::mNoButton to Dialog::ID_YES and Dialog::ID_NO 
(1000 and 1001) to avoid conflict with user defined buttons.

Various bug fixes

------------------------------------------------------------------------------------
Debug key (ctrl-alt-d) changes:
F2 - Start/Stop Perf Timing

F3 - toggle framerate display
Shift F3 - toggle framerate/mouse coord display

F8 - Show current Video Stats (mostly used to see if 3d is currently on)
Shift F8 - Toggle 3d mode

F10 - Single Step Program (show one frame at a time)
Shift F10 - Stop single steping
Ctrl F10 - Toggle Slow Motion

F11 - Take Screenshot (goes into the _screenshots) directory
Shift F11 - Dump all program images in memory to the _dump directory
------------------------------------------------------------------------------------

