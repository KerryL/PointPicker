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

// *nix Icons
#ifdef __WXGTK__
#include "../../res/icons/pp16.xpm"
#include "../../res/icons/pp24.xpm"
#include "../../res/icons/pp32.xpm"
#include "../../res/icons/pp48.xpm"
#include "../../res/icons/pp64.xpm"
#include "../../res/icons/pp128.xpm"
#include "../../res/icons/pp256.xpm"
#endif

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
	plotDataGroup->Add(plotButtonSizer, 0, wxGROW);

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

	grid = new wxGrid(plotDataGroup->GetStaticBox(), wxID_ANY);
	grid->BeginBatch();
	grid->CreateGrid(1, 2, wxGrid::wxGridSelectColumns);
	grid->SetCellSize(0, 0, 1, 2);
	grid->SetColLabelSize(0);
	grid->SetRowLabelSize(0);
	grid->SetMinSize(wxSize(-1, 200));
	grid->EndBatch();

	plotDataGroup->Add(grid, 1, wxGROW);

	// Set defaults
	plotDataGroup->GetStaticBox()->Enable(false);
	static_cast<wxRadioButton*>(this->FindWindow(idPointsAreXAxis))->SetValue(true);

	statusBar = BuildStatusBar();
	SetStatusBar(statusBar);

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

#ifdef __WXMSW__
	SetIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE));
#elif __WXGTK__
	SetIcon(wxIcon(pp16_xpm));
	SetIcon(wxIcon(pp24_xpm));
	SetIcon(wxIcon(pp32_xpm));
	SetIcon(wxIcon(pp48_xpm));
	SetIcon(wxIcon(pp64_xpm));
	SetIcon(wxIcon(pp128_xpm));
	SetIcon(wxIcon(pp256_xpm));
#endif

	SetDropTarget(dynamic_cast<wxDropTarget*>(new ImageDropTarget(*this)));
}

//==========================================================================
// Class:			ControlsFrame
// Function:		BuildStatusBar
//
// Description:		Builds the status bar for this frame.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxStatusBar*
//
//==========================================================================
wxStatusBar* ControlsFrame::BuildStatusBar()
{
	wxStatusBar* sb = new wxStatusBar(this);
	sb->SetFieldsCount(StatusFieldCount);
	const int styles[] =
	{
		wxSB_FLAT,		// StatusRawLabel
		wxSB_SUNKEN,	// StatusRaw
		wxSB_FLAT,		// StatusProcessedLabel
		wxSB_SUNKEN,	// StatusProcessed
		wxSB_FLAT		// StatusVersionInfo
	};
	sb->SetStatusStyles(StatusFieldCount, styles);

	sb->SetStatusText(_T("Px:"), StatusRawLabel);
	sb->SetStatusText(_T("Val:"), StatusProcessedLabel);
	sb->SetStatusText(PointPickerApp::versionString + _T(" (")
		+ PointPickerApp::gitHash + _T(")"), StatusVersionInfo);

	// TODO:  Better way to handle width calculation?  Does this look OK across platforms?
	const int widths[] =
	{
		20,		// StatusRawLabel
		-1,		// StatusRaw
		20,		// StatusProcessedLabel
		-2,		// StatusProcessed
		75		// StatusVersionInfo
	};
	sb->SetStatusWidths(StatusFieldCount, widths);

	return sb;
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
	EVT_GRID_CELL_LEFT_CLICK(ControlsFrame::GridClicked)
	EVT_GRID_SELECT_CELL(ControlsFrame::GridClicked)
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

	wxString delimiter;
	if (dialog.GetPath().Mid(dialog.GetPath().find_last_of('.')).CmpNoCase(_T(".txt")) == 0)
		delimiter = _T("\t");
	else
		delimiter = _T(",");

	std::stringstream ss;
	unsigned int i;
	for (i = 0; i < data.size(); i++)
	{
		wxString label(grid->GetCellValue(0, i * 2));
		if (label.IsEmpty())
			ss << "X" << i << delimiter << "Y" << i << delimiter;
		else
			ss << label << " X" << delimiter << label << " Y" << delimiter;
	}

	unsigned int j(0);
	bool finished(false);
	while (!finished)
	{
		finished = true;

		file << ss.str() << "\n";
		ss.str("");
		ss.clear();
		for (i = 0; i < data.size(); i++)
		{
			if (j < data[i].size())
			{
				ss << data[i][j].x << delimiter << data[i][j].y << delimiter;
				finished = false;
			}
			else
				ss << "0" << delimiter << "0" << delimiter;// TODO:  Better to leave blank, or use zeros?
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
	picker.SetCurveIndex(event.GetCol() / 2);

	grid->SelectCol(event.GetCol());
	if (event.GetCol() % 2 == 0)
		grid->SelectCol(event.GetCol() + 1, true);
	else
		grid->SelectCol(event.GetCol() - 1, true);

	event.Skip();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		AddNewPoint
//
// Description:		Adds the next point to the grid.
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
void ControlsFrame::AddNewPoint()
{
	if (picker.GetDataExtractionMode() != PointPicker::DataCurve)
		return;

	const unsigned int xCol(picker.GetCurveIndex() * 2);
	const unsigned int yCol(xCol + 1);

	grid->BeginBatch();

	int i(1);
	while (true)
	{
		if (grid->GetNumberRows() == i)
		{
			grid->AppendRows();
			int j;
			for (j = 0; j < grid->GetNumberCols(); j++)
				grid->SetReadOnly(i, j);
			break;
		}
		else if (grid->GetCellValue(i, xCol).IsEmpty())
			break;

		i++;
	}

	grid->SetCellValue(i, xCol, wxString::Format(_T("%f"), picker.GetNewestPoint().x));
	grid->SetCellValue(i, yCol, wxString::Format(_T("%f"), picker.GetNewestPoint().y));

	// If first point in new column, append two columns to the grid, too
	if (i == 1)
	{
		grid->AppendCols(2);
		grid->SetCellSize(0, yCol + 1, 1, 2);
		grid->SetReadOnly(0, yCol + 1, false);
		int j;
		for (j = 1; j < grid->GetNumberRows(); j++)
		{
			grid->SetReadOnly(j, yCol + 1);
			grid->SetReadOnly(j, yCol + 2);
		}
	}

	grid->EndBatch();
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
	ResetGrid();

	return true;
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ResetGrid
//
// Description:		Resets the grid.
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
void ControlsFrame::ResetGrid()
{
	grid->BeginBatch();
	if (grid->GetNumberCols() > 2)
		grid->DeleteCols(2, grid->GetNumberCols() - 2);
	if (grid->GetNumberRows() > 1)
		grid->DeleteCols(1, grid->GetNumberRows() - 1);
	grid->SetCellValue(0, 0, wxEmptyString);
	grid->EndBatch();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		UpdateStatusBar
//
// Description:		Updates the status bar text with information for the current
//					cursor position.
//
// Input Arguments:
//		rawX	= const double&
//		rawY	= const double&
//		xScale	= const double&
//		yScale	= const double&
//		xOffset	= const double&
//		yOffset	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ControlsFrame::UpdateStatusBar(const double& rawX, const double& rawY,
	const double& xScale, const double& yScale,
	const double& xOffset, const double& yOffset)
{
	double x, y;
	PointPicker::Point p(picker.ScaleSinglePoint(rawX, rawY, xScale, yScale, xOffset, yOffset, x, y));
	statusBar->SetStatusText(wxString::Format(_T("(%d, %d)"), (int)x, (int)y), StatusRaw);

	if (!picker.GetErrorString().empty())
		statusBar->SetStatusText(_T(""), StatusProcessed);
	else
		statusBar->SetStatusText(wxString::Format(_T("(%f, %f)"), p.x, p.y), StatusProcessed);
}
