// File:  pointPickerApp.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Application object for point picker.

#ifndef POINT_PICKER_APP_H_
#define POINT_PICKER_APP_H_

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class ControlsFrame;

// The application class
class PointPickerApp : public wxApp
{
public:
	bool OnInit();

	static const wxString title;// As displayed
	static const wxString name;// Internal
	static const wxString creator;
	static const wxString versionString;
	static const wxString gitHash;

private:
	ControlsFrame *mainFrame;
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(PointPickerApp)

#endif// POINT_PICKER_APP_H_
