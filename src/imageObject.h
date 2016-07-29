// File:  imageObject.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

#ifndef IMAGE_OBJECT_H_
#define IMAGE_OBJECT_H_

// wxWidgets headers
#include <wx/statbmp.h>

class ImageObject : public wxStaticBitmap
{
public:
	ImageObject(wxWindow &parent, wxWindowID id, const wxBitmap &image,
		const wxPoint &pos, const wxSize &size);

	virtual ~ImageObject() {}

private:
	void OnClick(wxMouseEvent &event);

	DECLARE_EVENT_TABLE();
};

#endif// IMAGE_OBJECT_H_