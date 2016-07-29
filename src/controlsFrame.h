// File:  controlsFrame.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for point picker application where user can select options, etc.

#ifndef CONTROLS_FRAME_H_
#define CONTROLS_FRAME_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

class ImageFrame;

class ControlsFrame : public wxFrame
{
public:
	ControlsFrame();
	~ControlsFrame();

	bool LoadFiles(const wxArrayString &fileList);

private:
	void CreateControls();
	void SetProperties();

	enum EventIDs
	{
		idCopyToClipboard = wxID_HIGHEST + 100,
		idExtractPlotData,

		idResetXAxis,
		idResetYAxis,
		idSavePlotData,

		idPointsAreXAxis,
		idPointsAreYAxis,
		idPointsAreCurveData
	};

	void CopyToClipboardToggle(wxCommandEvent& event);
	void ExtractPlotDataToggle(wxCommandEvent& event);
	void ResetXAxisClicked(wxCommandEvent& event);
	void ResetYAxisClicked(wxCommandEvent& event);
	void SavePlotDataClicked(wxCommandEvent& event);
	void PointAreXAxisClicked(wxCommandEvent& event);
	void PointAreYAxisClicked(wxCommandEvent& event);
	void PointAreCurveDataClicked(wxCommandEvent& event);
	void GridClicked(wxGridEvent& event);

	wxStaticBoxSizer *plotDataGroup;
	wxGrid* grid;

	ImageFrame* imageFrame;

	DECLARE_EVENT_TABLE();
};

#endif// CONTROLS_FRAME_H_
