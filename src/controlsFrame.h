// File:  controlsFrame.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for point picker application where user can select options, etc.

#ifndef CONTROLS_FRAME_H_
#define CONTROLS_FRAME_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// Local headers
#include "pointPicker.h"

// Local forware declarations
class ImageFrame;

class ControlsFrame : public wxFrame
{
public:
	ControlsFrame();

	bool LoadFiles(const wxArrayString &fileList);

	inline PointPicker& GetPicker() { return picker; }
	void UpdateStatusBar(const double& rawX, const double& rawY,
		const double& xScale, const double& yScale,
		const double& xOffset, const double& yOffset);
	void AddNewPoint();

private:
	void CreateControls();
	wxStatusBar* BuildStatusBar();
	void SetProperties();
	void ResetGrids();
	void UpdateReferenceGrid();

	PointPicker picker;

	enum EventIDs
	{
		idCopyToClipboard = wxID_HIGHEST + 100,
		idExtractPlotData,

		idResetReferences,
		idSavePlotData,

		idCurveGrid,
		idReferenceGrid,

		idPointsAreReferences,
		idPointsAreCurveData,

		idMenuRemoveReference
	};

	void CopyToClipboardToggle(wxCommandEvent& event);
	void ExtractPlotDataToggle(wxCommandEvent& event);
	void ResetReferencesClicked(wxCommandEvent& event);
	void SavePlotDataClicked(wxCommandEvent& event);
	void PointAreReferencesClicked(wxCommandEvent& event);
	void PointAreCurveDataClicked(wxCommandEvent& event);
	void CurveGridClicked(wxGridEvent& event);
	void ReferenceGridRightClicked(wxGridEvent& event);
	void RemoveReferenceMenuClicked(wxCommandEvent& event);
	void OnActivate(wxActivateEvent& event);
	void OnClose(wxCloseEvent& event);

	wxStaticBoxSizer* plotDataGroup;
	wxGrid* curveGrid;
	wxGrid* referenceGrid;
	wxStatusBar* statusBar;

	ImageFrame* imageFrame;

	enum StatusFields
	{
		StatusRawLabel,
		StatusRaw,
		StatusProcessedLabel,
		StatusProcessed,
		StatusVersionInfo,

		StatusFieldCount
	};

	DECLARE_EVENT_TABLE()
};

#endif// CONTROLS_FRAME_H_
