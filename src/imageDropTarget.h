// File:  imageDropTarget.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Drag-and-drop target for image files.

#ifndef IMAGE_DROP_TARGET_H_
#define IMAGE_DROP_TARGET_H_

// wxWidgets headers
#include <wx/dnd.h>

// Standard C++ headers
#include <memory>

// Local forward declarations
class ControlsFrame;

class ImageDropTarget : public wxDropTarget
{
public:
	ImageDropTarget(ControlsFrame &mainFrame);

	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

private:
	ControlsFrame &mainFrame;

	bool OnDropFiles(const wxArrayString &filenames);

	std::unique_ptr<char[]> buffer;
};

#endif// IMAGE_DROP_TARGET_H_