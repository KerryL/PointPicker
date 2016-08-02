// File:  pointEntryDialog.h
// Date:  8/2/2016
// Auth:  K. Loux
// Desc:  Frame object for point picker application where user can select options, etc.

#ifndef POINT_ENTRY_DIALOG_H_
#define POINT_ENTRY_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "pointPicker.h"

class PointEntryDialog : public wxDialog
{
public:
	PointEntryDialog(wxWindow* parent, wxWindowID id, const wxString& title);
	virtual ~PointEntryDialog() {}

	PointPicker::Point GetPoint() const { return point; }

private:
	void CreateControls();
	void SetProperties();

	PointPicker::Point point;

	wxTextCtrl* xValue;
	wxTextCtrl* yValue;

protected:
	virtual bool TransferDataFromWindow();
};

#endif// POINT_ENTRY_DIALOG_H_