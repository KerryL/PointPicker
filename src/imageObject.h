// File:  imageObject.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

#ifndef IMAGE_OBJECT_H_
#define IMAGE_OBJECT_H_

// wxWidgets headers
#include <wx/statbmp.h>

// Local forward declarations
class PointPicker;

class ImageObject : public wxStaticBitmap
{
public:
	ImageObject(PointPicker& picker, wxWindow &parent, wxWindowID id, const wxBitmap &image,
		const wxPoint &pos, const wxSize &size);

	virtual ~ImageObject();

	virtual void SetBitmap(const wxBitmap& bitmap);
	void HandleSizeChange();

private:
	PointPicker &picker;
	wxBitmap originalImage;

	void OnClick(wxMouseEvent &event);
	void OnDrag(wxMouseEvent& event);
	void OnZoom(wxMouseEvent& event);

	bool mouseMoved;

	DECLARE_EVENT_TABLE();
};

#endif// IMAGE_OBJECT_H_