// File:  controlsFrame.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for point picker application where user can select options, etc.

// Standard C++ headers
#include <fstream>
#include <sstream>

// wxWidgets headers
#include <wx/tglbtn.h>
#include <wx/notebook.h>

// Local headers
#include "controlsFrame.h"
#include "imageFrame.h"
#include "imageDropTarget.h"
#include "pointPickerApp.h"

// *nix Icons
#ifdef __WXGTK__
#include "../res/icons/pointPicker16.xpm"
#include "../res/icons/pointPicker24.xpm"
#include "../res/icons/pointPicker32.xpm"
#include "../res/icons/pointPicker48.xpm"
#include "../res/icons/pointPicker64.xpm"
#include "../res/icons/pointPicker128.xpm"
#include "../res/icons/pointPicker256.xpm"
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
ControlsFrame::ControlsFrame() : wxFrame(nullptr, wxID_ANY, wxEmptyString, wxDefaultPosition,
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
//		event	= wxActivateEvent&
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
	if (imageFrame && !imageFrame->HasFocus())
		imageFrame->Raise();

	event.Skip();
}

//==========================================================================
// Class:			ControlsFrame
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
void ControlsFrame::OnClose(wxCloseEvent& event)
{
	if (!IsActive())
		wxQueueEvent(this, new wxActivateEvent());// fix for application not closing if closed from taskbar when not focused; see https://forums.wxwidgets.org/viewtopic.php?t=43498
		
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
	topSizer->Add(panel, wxSizerFlags().Expand().Proportion(1));
	panel->SetSizer(panelSizer);

	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	panelSizer->Add(mainSizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 5));

	wxSizer *modeSizer = new wxBoxSizer(wxHORIZONTAL);
	modeSizer->Add(new wxToggleButton(panel, idCopyToClipboard, _T("Copy Pixel To Clipboard")), 1);
	modeSizer->Add(new wxToggleButton(panel, idExtractPlotData, _T("Extract Plot Data")), 1);
	mainSizer->Add(modeSizer);

	mainSizer->AddSpacer(15);

	plotDataGroup = new wxStaticBoxSizer(wxVERTICAL, panel, _T("Plot Data Extraction"));
	mainSizer->Add(plotDataGroup, wxSizerFlags().Expand().Proportion(1));
	wxSizer *plotUpperSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *radioSizer = new wxBoxSizer(wxVERTICAL);
	plotDataGroup->Add(plotUpperSizer, wxSizerFlags().Expand());

	radioSizer->Add(new wxRadioButton(plotDataGroup->GetStaticBox(), idPointsAreReferences, _T("Points are references")));
	radioSizer->Add(new wxRadioButton(plotDataGroup->GetStaticBox(), idPointsAreCurveData, _T("Points are on curve")));
	plotUpperSizer->Add(radioSizer);
	plotUpperSizer->AddSpacer(15);

	plotUpperSizer->Add(new wxButton(plotDataGroup->GetStaticBox(), idResetReferences, _T("Reset References")));
	plotUpperSizer->AddStretchSpacer();
	plotUpperSizer->Add(new wxButton(plotDataGroup->GetStaticBox(), idSavePlotData, _T("Save Data")));
	plotDataGroup->AddSpacer(15);

	auto notebook(new wxNotebook(plotDataGroup->GetStaticBox(), wxID_ANY));

	curveGrid = new wxGrid(notebook, idCurveGrid);
	curveGrid->BeginBatch();
	curveGrid->CreateGrid(1, 2, wxGrid::wxGridSelectColumns);
	curveGrid->SetCellSize(0, 0, 1, 2);
	curveGrid->SetColLabelSize(0);
	curveGrid->SetRowLabelSize(0);
#ifdef __WXMSW__
	curveGrid->SetMinSize(wxSize(-1, 200));
#else
	curveGrid->SetMinSize(wxSize(450, 200));
#endif
	curveGrid->EndBatch();

	referenceGrid = new wxGrid(notebook, idReferenceGrid);
	referenceGrid->BeginBatch();
	referenceGrid->CreateGrid(1, 2, wxGrid::wxGridSelectRows);
	referenceGrid->SetCellSize(0, 0, 1, 2);
	referenceGrid->SetColLabelSize(0);
	referenceGrid->SetRowLabelSize(0);
#ifdef __WXMSW__
	referenceGrid->SetMinSize(wxSize(-1, 200));
#else
	referenceGrid->SetMinSize(wxSize(450, 200));
#endif
	referenceGrid->EndBatch();

	notebook->AddPage(curveGrid, _T("Curve"));
	notebook->AddPage(referenceGrid, _T("References"));

	plotDataGroup->Add(notebook, wxSizerFlags().Expand().Proportion(1));

	// Set defaults
	plotDataGroup->GetStaticBox()->Enable(false);
	static_cast<wxRadioButton*>(this->FindWindow(idPointsAreReferences))->SetValue(true);

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
	SetIcon(wxIcon(pointPicker16_xpm));
	SetIcon(wxIcon(pointPicker24_xpm));
	SetIcon(wxIcon(pointPicker32_xpm));
	SetIcon(wxIcon(pointPicker48_xpm));
	SetIcon(wxIcon(pointPicker64_xpm));
	SetIcon(wxIcon(pointPicker128_xpm));
	SetIcon(wxIcon(pointPicker256_xpm));
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

#ifdef __WXMSW__
	const int widths[] =
	{
		20,		// StatusRawLabel
		-1,		// StatusRaw
		20,		// StatusProcessedLabel
		-2,		// StatusProcessed
		75		// StatusVersionInfo
	};
#else
	const int widths[] =
	{
		40,		// StatusRawLabel
		-1,		// StatusRaw
		40,		// StatusProcessedLabel
		-2,		// StatusProcessed
		135		// StatusVersionInfo
	};
#endif
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
	EVT_BUTTON(idResetReferences, ControlsFrame::ResetReferencesClicked)
	EVT_BUTTON(idSavePlotData, ControlsFrame::SavePlotDataClicked)
	EVT_RADIOBUTTON(idPointsAreReferences, ControlsFrame::PointAreReferencesClicked)
	EVT_RADIOBUTTON(idPointsAreCurveData, ControlsFrame::PointAreCurveDataClicked)
	EVT_ACTIVATE(ControlsFrame::OnActivate)
	EVT_GRID_CMD_CELL_LEFT_CLICK(idCurveGrid, ControlsFrame::CurveGridClicked)
	EVT_GRID_CMD_SELECT_CELL(idCurveGrid, ControlsFrame::CurveGridClicked)
	EVT_GRID_CMD_CELL_RIGHT_CLICK(idReferenceGrid, ControlsFrame::ReferenceGridRightClicked)
	EVT_MENU(idMenuRemoveReference, ControlsFrame::RemoveReferenceMenuClicked)
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
		picker.SetClipboardMode(PointPicker::ClipboardMode::Both);
	else
		picker.SetClipboardMode(PointPicker::ClipboardMode::None);
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
		wxRadioButton* references(static_cast<wxRadioButton*>(FindWindowById(idPointsAreReferences, this)));
		assert(references);

		if (references->GetValue())
			picker.SetDataExtractionMode(PointPicker::DataExtractionMode::References);
		else
			picker.SetDataExtractionMode(PointPicker::DataExtractionMode::Curve);
	}
	else
		picker.SetDataExtractionMode(PointPicker::DataExtractionMode::None);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ResetReferencesClicked
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
void ControlsFrame::ResetReferencesClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.ResetReferences();
	UpdateReferenceGrid();
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
	if (!picker.GetErrorString().empty())
	{
		wxMessageBox(_T("The following errors occurred while estimating curve data:\n")
			+ picker.GetErrorString(), _T("Error"));
		return;
	}

	std::vector<std::vector<PointPicker::Point>> data(picker.GetCurveData());
	if (data.size() == 0)
	{
		wxMessageBox(_T("No point data specified."), _T("No Data"));
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
		wxString label(curveGrid->GetCellValue(0, i * 2));
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
// Function:		PointAreReferencesClicked
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
void ControlsFrame::PointAreReferencesClicked(wxCommandEvent& WXUNUSED(event))
{
	picker.SetDataExtractionMode(PointPicker::DataExtractionMode::References);
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
	picker.SetDataExtractionMode(PointPicker::DataExtractionMode::Curve);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		CurveGridClicked
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
void ControlsFrame::CurveGridClicked(wxGridEvent& event)
{
	picker.SetCurveIndex(event.GetCol() / 2);

	curveGrid->SelectCol(event.GetCol());
	if (event.GetCol() % 2 == 0)
		curveGrid->SelectCol(event.GetCol() + 1, true);
	else
		curveGrid->SelectCol(event.GetCol() - 1, true);

	event.Skip();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ReferenceGridRightClicked
//
// Description:		Handles grid right-click events.
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
void ControlsFrame::ReferenceGridRightClicked(wxGridEvent& event)
{
	referenceGrid->SelectRow(event.GetRow(), event.ControlDown());
	wxMenu menu;
	menu.Append(idMenuRemoveReference, _T("Remove"), _T("Remove selected reference(s)"));
	PopupMenu(&menu);
}

//==========================================================================
// Class:			ControlsFrame
// Function:		RemoveReferenceMenuClicked
//
// Description:		Handles remove reference menu item clicks.
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
void ControlsFrame::RemoveReferenceMenuClicked(wxCommandEvent& WXUNUSED(event))
{
	auto selections(referenceGrid->GetSelectedRows());

	auto sortDescending([](int *a, int *b) -> int
	{
		if (*a < *b)
			return 1;
		else if (*a > *b)
			return -1;
		return 0;
	});

	selections.Sort(sortDescending);
	for (const auto& r : selections)
		picker.RemoveReference(r);
	UpdateReferenceGrid();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		AddNewPoint
//
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
	if (picker.GetDataExtractionMode() == PointPicker::DataExtractionMode::References)
		UpdateReferenceGrid();
	
	if (picker.GetDataExtractionMode() != PointPicker::DataExtractionMode::Curve)
		return;

	const unsigned int xCol(picker.GetCurveIndex() * 2);
	const unsigned int yCol(xCol + 1);

	curveGrid->BeginBatch();

	int i(1);
	while (true)
	{
		if (curveGrid->GetNumberRows() == i)
		{
			curveGrid->AppendRows();
			for (int j = 0; j < curveGrid->GetNumberCols(); j++)
				curveGrid->SetReadOnly(i, j);
			break;
		}
		else if (curveGrid->GetCellValue(i, xCol).IsEmpty())
			break;

		i++;
	}

	curveGrid->SetCellValue(i, xCol, wxString::Format(_T("%f"), picker.GetNewestPoint().x));
	curveGrid->SetCellValue(i, yCol, wxString::Format(_T("%f"), picker.GetNewestPoint().y));

	// If first point in new column, append two columns to the grid, too
	if (i == 1)
	{
		curveGrid->AppendCols(2);
		curveGrid->SetCellSize(0, yCol + 1, 1, 2);
		curveGrid->SetReadOnly(0, yCol + 1, false);
		for (int j = 1; j < curveGrid->GetNumberRows(); j++)
		{
			curveGrid->SetReadOnly(j, yCol + 1);
			curveGrid->SetReadOnly(j, yCol + 2);
		}
	}

	curveGrid->EndBatch();
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
	ResetGrids();

	return true;
}

//==========================================================================
// Class:			ControlsFrame
// Function:		ResetGrids
//
// Description:		Resets the grids.
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
void ControlsFrame::ResetGrids()
{
	curveGrid->BeginBatch();
	if (curveGrid->GetNumberCols() > 2)
		curveGrid->DeleteCols(2, curveGrid->GetNumberCols() - 2);
	if (curveGrid->GetNumberRows() > 1)
		curveGrid->DeleteRows(1, curveGrid->GetNumberRows() - 1);
	curveGrid->SetCellValue(0, 0, wxEmptyString);
	curveGrid->EndBatch();

	UpdateReferenceGrid();
}

//==========================================================================
// Class:			ControlsFrame
// Function:		UpdateReferenceGrid
//
// Description:		Updates the reference grid to match the picker references.
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
void ControlsFrame::UpdateReferenceGrid()
{
	const auto refs(picker.GetReferences());
	referenceGrid->BeginBatch();
	if (static_cast<size_t>(referenceGrid->GetNumberRows()) > refs.size())
		referenceGrid->DeleteRows(0, referenceGrid->GetNumberRows() - refs.size());
	if (static_cast<size_t>(referenceGrid->GetNumberRows()) < refs.size())
		referenceGrid->AppendRows(refs.size() - referenceGrid->GetNumberRows());

	for (unsigned int r = 0; r < refs.size(); ++r)
	{
		referenceGrid->SetCellValue(r, 0, wxString::Format(_T("%f"), refs[r].x));
		referenceGrid->SetCellValue(r, 1, wxString::Format(_T("%f"), refs[r].y));
	}

	referenceGrid->EndBatch();
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
