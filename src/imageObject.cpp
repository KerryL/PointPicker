// File:  imageObject.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

// Local headers
#include "imageObject.h"

//==========================================================================
// Class:			ImageObject
// Function:		ImageObject
//
// Description:		Constructor for ImageObject class.
//
// Input Arguments:
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
ImageObject::ImageObject(wxWindow &parent, wxWindowID id, const wxBitmap &label,
	const wxPoint &pos, const wxSize &size)
	: wxStaticBitmap(&parent, id, label, pos, size)
{
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
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		OnClick
//
// Description:		Handles click event.
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
	// TODO:  Implement
}