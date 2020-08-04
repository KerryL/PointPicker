// File:  pointPickerApp.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Application object for point picker.

// Local headers
#include "pointPickerApp.h"
#include "controlsFrame.h"

// Implement the application (have wxWidgets set up the appropriate entry points, etc.)
IMPLEMENT_APP(PointPickerApp)

//==========================================================================
// Class:			PointPickerApp
// Function:		Constant Declarations
//
// Description:		Constant declarations for the PixelExtractorApp class.
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
const wxString PointPickerApp::title = _T("Point Picker");
const wxString PointPickerApp::name = _T("PointPickerApplication");
const wxString PointPickerApp::creator = _T("Kerry Loux");

//==========================================================================
// Class:			PointPickerApp
// Function:		OnInit
//
// Description:		Initializes the application window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool = true for successful window initialization, false for error
//
//==========================================================================
bool PointPickerApp::OnInit()
{
	// Set the application's name and the vendor's name
	SetAppName(name);
	SetVendorName(creator);

	// Create the MainFrame object - this is the parent for all other objects
	mainFrame = new ControlsFrame();

	// Make sure the MainFrame was successfully created
	if (!mainFrame)
		return false;

	// Make the window visible
	mainFrame->Show(true);

	return true;
}
