// File:  controlsFrame.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for point picker application where user can select options, etc.

// Standard C++ headers
#include <fstream>
#include <sstream>

// wxWidgets headers
#include <wx/tglbtn.h>

// Local headers
#include "controlsFrame.h"
#include "imageFrame.h"
#include "imageDropTarget.h"
#include "pointPickerApp.h"

//==========================================================================
// Class:			ControlsFrame
// Function:		ControlsFrame
//
// Description:		Constructor for ControlsFrame class.  Initializes the form
//					and creates the controls, etc.
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
ControlsFrame::ControlsFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
								 wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
	CreateControls();
	SetProperties();

	imageFrame = new ImageFrame(*this);
	imageFrame->Show();

	wxInitAllImageHandlers();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		OnActivate
//
// Description:		Handles window getting focus events.
//
// Input Arguments:
//		event	= wxFocusEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ControlsFrame::OnActivate(wxActivateEvent& event)
{
	if (imageFrame && !imageFrame->HasFocus())// TODO:  Not quite right
		imageFrame->Raise();

	event.Skip();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		CreateControls
//
// Description:		Creates sizers and controls and lays them out in the window.
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
void ControlsFrame::CreateControls()
{
	wxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxPanel *panel = new wxPanel(this);
	wxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(panel,1, wxGROW);
	panel->SetSizer(panelSizer);

	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	panelSizer->Add(mainSizer, 1, wxALL | wxGROW, 5);

	wxSizer *modeSizer = new wxBoxSizer(wxHORIZONTAL);
	modeSizer->Add(new wxToggleButton(panel, idCopyToClipboard, _T("Copy Pixel To Clipboard")), 1);
	modeSizer->Add(new wxToggleButton(panel, idExtractPlotData, _T("Extract Plot Data")), 1);
	mainSizer->Add(modeSizer);

	mainSizer->AddSpacer(15);

	plotDataGroup = new wxStaticBoxSizer(wxVERTICAL, panel, _T("Plot Data Extraction"));
	mainSizer->Add(plotDataGroup, 1, wxGROW);
	wxFlexGridSizer *plotButtonSizer = new wxFlexGridSizer(3, 4, 5, 5);
	plotButtonSizer->AddGrowableCol(2);
	plotDataGroup->Add(plotButtonSizer, 1, wxGROW);

	// Row 1
	plotButtonSizer->Add(new wxRadioButton(plotDataGroup->GetStaticBox(), idPointsAreXAxis, _T("Points are on x-axis")));
	plotButtonSizer->Add(new wxButton(plotDataGroup->GetStaticBox(), idResetXAxis, _T("Reset X-Axis")));
	plotButtonSizer->AddStretchSpacer();
	plotButtonSizer->Add(new wxButton(plotDataGroup->GetStaticBox(), idSavePlotData, _T("Save Data")));

	// Row 2
	plotButtonSizer->Add(new wxRadioButton(plotDataGroup->GetStaticBox(), idPointsAreYAxis, _T("Points are on y-axis")));
	plotButtonSizer->Add(new wxButton(plotDataGroup->GetStaticBox(), idResetYAxis, _T("Reset Y-Axis")));
	plotButtonSizer->AddStretchSpacer();
	plotButtonSizer->AddStretchSpacer();

	// Row 3
	plotButtonSizer->Add(new wxRadioButton(plotDataGroup->GetStaticBox(), idPointsAreCurveData, _T("Points are on curve")));

	// TODO:  Don't create grid yet?  Do it dynamically as points are picked?
	// TODO:  Sizers are not working as desired w.r.t. grid yet
	//wxSizer *gridSizer = new wxBoxSizer(wxHORIZONTAL);
	grid = new wxGrid(plotDataGroup->GetStaticBox(), wxID_ANY);
	grid->BeginBatch();
	grid->CreateGrid(5, 4, wxGrid::wxGridSelectColumns);
	grid->SetColLabelSize(0);
	grid->SetRowLabelSize(0);
	grid->EndBatch();

	//gridSizer->Add(grid, 1, wxGROW);
	plotDataGroup->Add(grid/*Sizer*/, 1, wxGROW);

	// Set defaults
	plotDataGroup->GetStaticBox()->Enable(false);
	static_cast<wxRadioButton*>(this->FindWindow(idPointsAreXAxis))->SetValue(true);

	SetSizerAndFit(topSizer);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default fonts.
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
void ControlsFrame::SetProperties()
{
	SetTitle(PointPickerApp::title);
	SetName(PointPickerApp::name);
	Center();

	SetDropTarget(dynamic_cast<wxDropTarget*>(new ImageDropTarget(*this)));
}

//==========================================================================
// Class:			ControlsFrame
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
BEGIN_EVENT_TABLE(ControlsFrame, wxFrame)
	EVT_TOGGLEBUTTON(idCopyToClipboard, ControlsFrame::CopyToClipboardToggle)
	EVT_TOGGLEBUTTON(idExtractPlotData, ControlsFrame::ExtractPlotDataToggle)
	EVT_BUTTON(idResetXAxis, ControlsFrame::ResetXAxisClicked)
	EVT_BUTTON(idResetYAxis, ControlsFrame::ResetYAxisClicked)
	EVT_BUTTON(idSavePlotData, ControlsFrame::SavePlotDataClicked)
	EVT_RADIOBUTTON(idPointsAreXAxis, ControlsFrame::PointAreXAxisClicked)
	EVT_RADIOBUTTON(idPointsAreYAxis, ControlsFrame::PointAreYAxisClicked)
	EVT_RADIOBUTTON(idPointsAreCurveData, ControlsFrame::PointAreCurveDataClicked)
	EVT_ACTIVATE(ControlsFrame::OnActivate)
END_EVENT_TABLE()

//==========================================================================
// Class:			ControlsFrame
// Function:		CopyToClipboardToggle
//
// Description:		Handles event for copy to clipboard button toggled.
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
void ControlsFrame::CopyToClipboardToggle(wxCommandEvent& event)
{
	if (event.IsChecked())
		picker.SetClipboardMode(PointPicker::ClipBoth);
	else
		picker.SetClipboardMode(PointPicker::ClipNone);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ExtractPlotDataToggle
//
// Description:		Handles event for extract plot data button toggle.
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
void ControlsFrame::ExtractPlotDataToggle(wxCommandEvent& event)
{
	plotDataGroup->GetStaticBox()->Enable(event.IsChecked());
	if (event.IsChecked())
	{
		wxRadioButton* xAxis(static_cast<wxRadioButton*>(FindWindowById(idPointsAreXAxis, this)));
		wxRadioButton* yAxis(static_cast<wxRadioButton*>(FindWindowById(idPointsAreXAxis, this)));
		assert(xAxis && yAxis);

		if (xAxis->GetValue())
			picker.SetDataExtractionMode(PointPicker::DataXAxis);
		else if (yAxis->GetValue())
			picker.SetDataExtractionMode(PointPicker::DataYAxis);
		else
			picker.SetDataExtractionMode(PointPicker::DataCurve);
	}
	else
		picker.SetDataExtractionMode(PointPicker::DataNone);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ResetXAxisClicked
//
// Description:		Handles button click events.
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
void ControlsFrame::ResetXAxisClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.ResetXAxis();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ResetYAxisClicked
//
// Description:		Handles button click events.
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
void ControlsFrame::ResetYAxisClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.ResetYAxis();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		SavePlotDataClicked
//
// Description:		Handles button click events.
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
void ControlsFrame::SavePlotDataClicked(wxCommandEvent& WXUNUSED(event))
{
	std::vector<std::vector<PointPicker::Point> > data(picker.GetCurveData());
	if (data.size() == 0 && !picker.GetErrorString().empty())
	{
		wxMessageBox(_T("The following errors occurred while estimating curve data:\n")
			+ picker.GetErrorString(), _T("Error"));
		return;
	}

	wxFileDialog dialog(this, _T("Save Plot Data"), wxEmptyString, wxEmptyString,
		_T("Comma-Separated Values (*.csv)|*.csv|Tab Delimited (*.txt)|*.txt"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (dialog.ShowModal() == wxID_CANCEL)
		return;

	std::ofstream file(dialog.GetPath().ToStdString().c_str());
	if (!file.is_open() || !file.good())
	{
		wxMessageBox(_T("Failed to open '") + dialog.GetPath() + _T("' for output."), _T("Error"));
		return;
	}

	wxChar delimiter;
	if (dialog.GetPath().Mid(dialog.GetPath().find_last_of('.')).CmpNoCase(_T(".txt")) == 0)
		delimiter = '\t';
	else
		delimiter = ',';

	unsigned int i;
	for (i = 0; i < data.size(); i++)
		// TODO:  Include real descriptions?
		file << "X(" << i << ")" << delimiter << "Y(" << i << "),";

	unsigned int j(0);
	bool finished(false);
	while (!finished)
	{
		finished = true;
		file << "\n";
		for (i = 0; i < data.size(); i++)
		{
			if (j < data[i].size())
			{
				file << data[i][j].x << delimiter << data[i][j].y << delimiter;
				finished = false;
			}
			else
				file << "0,0,";// TODO:  Better to leave blank? 
		}
		j++;
	}
}

//==========================================================================
// Class:			ControlsFrame
// Function:		PointAreXAxisClicked
//
// Description:		Handles radio button click events.
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
void ControlsFrame::PointAreXAxisClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.SetDataExtractionMode(PointPicker::DataXAxis);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		PointAreYAxisClicked
//
// Description:		Handles radio button click events.
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
void ControlsFrame::PointAreYAxisClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.SetDataExtractionMode(PointPicker::DataYAxis);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		PointAreCurveDataClicked
//
// Description:		Handles radio button click events.
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
void ControlsFrame::PointAreCurveDataClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.SetDataExtractionMode(PointPicker::DataCurve);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		GridClicked
//
// Description:		Handles grid click events.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ControlsFrame::GridClicked(wxGridEvent& event)
{
	// TODO:  Implement
}

//==========================================================================
// Class:			ControlsFrame
// Function:		LoadFiles
//
// Description:		Loads the first file from the specified list of files.
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
bool ControlsFrame::LoadFiles(const wxArrayString &fileList)
{
	if (fileList.Count() == 0)
		return false;

	wxImage newImage(fileList[0]);
	imageFrame->SetImage(newImage);
	picker.Reset();

	return true;
}
