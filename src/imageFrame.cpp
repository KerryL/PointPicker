// File:  imageFrame.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

// Local headers
#include "imageFrame.h"
#include "controlsFrame.h"

//==========================================================================
// Class:			ImageFrame
// Function:		ImageFrame
//
// Description:		Constructor for ImageFrame class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		controlsFrame	= ControlsFrame&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ImageFrame::ImageFrame(ControlsFrame& controlsFrame) : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
	wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR), controlsFrame(controlsFrame)
{
	CreateControls();
	SetProperties();
	wxInitAllImageHandlers();
}

//==========================================================================
// Class:			ImageFrame
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
BEGIN_EVENT_TABLE(ImageFrame, wxFrame)
	EVT_CLOSE(ImageFrame::OnClose)
END_EVENT_TABLE();

//==========================================================================
// Class:			ImageFrame
// Function:		CreateControls
//
// Description:		Creates controls for ImageFrame.
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
void ImageFrame::CreateControls()
{
}

//==========================================================================
// Class:			ImageFrame
// Function:		SetProperties
//
// Description:		Sets the title, initial position, etc.
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
void ImageFrame::SetProperties()
{
}

//==========================================================================
// Class:			ImageFrame
// Function:		OnClose
//
// Description:		Handles window close events.
//
// Input Arguments:
//		event	= wxCloseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageFrame::OnClose(wxCloseEvent& event)
{
	event.Skip();
	if (controlsFrame.IsClosing())
		return;

	if (!controlsFrame.Close())
		event.Veto();
}
