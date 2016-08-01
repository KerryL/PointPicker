// File:  imageFrame.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

// Local headers
#include "imageFrame.h"
#include "controlsFrame.h"
#include "imageObject.h"
#include "imageDropTarget.h"

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
ImageFrame::ImageFrame(ControlsFrame& controlsFrame) : wxFrame(&controlsFrame, wxID_ANY, wxEmptyString, wxDefaultPosition,
	wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFRAME_NO_TASKBAR), controlsFrame(controlsFrame)
{
	CreateControls();
	SetProperties();
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
	EVT_SIZE(ImageFrame::OnResize)
END_EVENT_TABLE()

//==========================================================================
// Class:			ImageFrame
// Function:		SetImage
//
// Description:		Sets a new image to be displayed.
//
// Input Arguments:
//		i	= wxImage&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageFrame::SetImage(wxImage &i)
{
	image->SetBitmap(i);
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
	SetTitle(_T("Source Image"));
	wxPoint position(controlsFrame.GetPosition());
	int parentWidth(controlsFrame.GetSize().GetWidth());
	position.x += parentWidth;
	SetPosition(position);

	SetDropTarget(dynamic_cast<wxDropTarget*>(new ImageDropTarget(controlsFrame)));
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
	if (!controlsFrame.Close())
		event.Veto();
}

//==========================================================================
// Class:			ImageFrame
// Function:		OnResize
//
// Description:		Handles window resize events.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ImageFrame::OnResize(wxSizeEvent& WXUNUSED(event))
{
	image->HandleSizeChange();
}

//==========================================================================
// Class:			ImageFrame
// Function:		CreateControls
//
// Description:		Creates image controls.
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
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxImage dummyImage(500, 500);
	image = new ImageObject(controlsFrame.GetPicker(), *this, wxID_ANY,
		dummyImage, wxDefaultPosition, wxDefaultSize);
	mainSizer->Add(image, 1, wxGROW);

	SetSizerAndFit(mainSizer);
}
