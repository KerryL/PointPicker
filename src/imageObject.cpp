// File:  imageObject.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

// Local headers
#include "imageObject.h"
#include "pointPicker.h"

//==========================================================================
// Class:			ImageObject
// Function:		ImageObject
//
// Description:		Constructor for ImageObject class.
//
// Input Arguments:
//		picker	= PointPicker&
//		parent	= wxWindow&
//		id		= wxWindowID
//		image	= const wxBitmap&
//		pos		= const wxPoint&
//		size	= const wxSize&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ImageObject::ImageObject(PointPicker& picker, wxWindow &parent, wxWindowID id,
	const wxBitmap &image, const wxPoint &pos, const wxSize &size)
	: wxStaticBitmap(&parent, id, image, pos, size), picker(picker)
{
	picker.SetImageObject(this);
	mouseMoved = false;
	originalImage = image;
}

//==========================================================================
// Class:			ImageObject
// Function:		~ImageObject
//
// Description:		Destructor for ImageObject class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ImageObject::~ImageObject()
{
	picker.SetImageObject(NULL);
}

//==========================================================================
// Class:			ImageObject
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(ImageObject, wxStaticBitmap)
	EVT_LEFT_UP(ImageObject::OnClick)
	EVT_MOTION(ImageObject::OnDrag)
	EVT_MOUSEWHEEL(ImageObject::OnZoom)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		OnClick
//
// Description:		Handles click events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageObject::OnClick(wxMouseEvent &event)
{
	if (mouseMoved)
	{
		mouseMoved = false;
		return;
	}

	picker.AddPoint(event.GetX(), event.GetY(),
		(double)originalImage.GetWidth() / GetBitmap().GetWidth(),
		(double)originalImage.GetHeight() / GetBitmap().GetHeight(), 0.0, 0.0);
}

//==========================================================================
// Class:			MainFrame
// Function:		OnDrag
//
// Description:		Handles drag events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageObject::OnDrag(wxMouseEvent& event)
{
	if (!event.LeftDown())
		return;

	mouseMoved = true;
}

//==========================================================================
// Class:			MainFrame
// Function:		OnZoom
//
// Description:		Handles mouse wheel event.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageObject::OnZoom(wxMouseEvent& event)
{
	/*if (event.GetWheelRotation() > 0)
	{
	}
	else
	{
	}*/
}

//==========================================================================
// Class:			MainFrame
// Function:		HandleSizeChange
//
// Description:		Handles size changes.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageObject::HandleSizeChange()
{
	wxStaticBitmap::SetBitmap(originalImage.ConvertToImage().Scale(
		GetParent()->GetClientSize().GetWidth(), GetParent()->GetClientSize().GetHeight()));
}

//==========================================================================
// Class:			MainFrame
// Function:		HandleSizeChange
//
// Description:		Handles size changes.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageObject::SetBitmap(const wxBitmap& bitmap)
{
	SetImage(&bitmap);
	originalImage = bitmap;
	HandleSizeChange();
}