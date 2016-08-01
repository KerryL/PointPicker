// File:  imageObject.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

// Local headers
#include "imageObject.h"
#include "pointPicker.h"
#include "controlsFrame.h"

//==========================================================================
// Class:			ImageObject
// Function:		ImageObject
//
// Description:		Constructor for ImageObject class.
//
// Input Arguments:
//		picker			= PointPicker&
//		parent			= wxWindow&
//		id				= wxWindowID
//		image			= const wxBitmap&
//		pos				= const wxPoint&
//		size			= const wxSize&
//		controlsFrame	= ControlsFrame&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ImageObject::ImageObject(PointPicker& picker, wxWindow &parent, wxWindowID id,
	const wxBitmap &image, const wxPoint &pos, const wxSize &size,
	ControlsFrame& controlsFrame) : wxStaticBitmap(&parent, id, image, pos, size),
	picker(picker), controlsFrame(controlsFrame)
{
	mouseMoved = false;
	originalImage = image;
}

//==========================================================================
// Class:			ImageObjet
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
END_EVENT_TABLE()

//==========================================================================
// Class:			ImageObject
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
// Class:			ImageObject
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
	controlsFrame.UpdateStatusBar(event.GetX(), event.GetY(),
		(double)originalImage.GetWidth() / GetBitmap().GetWidth(),
		(double)originalImage.GetHeight() / GetBitmap().GetHeight(), 0.0, 0.0);

	if (!event.LeftDown())
		return;

	mouseMoved = true;
}

//==========================================================================
// Class:			ImageObject
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
void ImageObject::OnZoom(wxMouseEvent& WXUNUSED(event))
{
	/*if (event.GetWheelRotation() > 0)
	{
	}
	else
	{
	}*/
}

//==========================================================================
// Class:			ImageObject
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
// Class:			ImageObject
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
	originalImage = bitmap;
	HandleSizeChange();
}
