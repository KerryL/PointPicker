// File:  pointEntryDialog.cpp
// Date:  8/2/2016
// Auth:  K. Loux
// Desc:  Frame object for point picker application where user can select options, etc.

// Local headers
#include "pointEntryDialog.h"

PointEntryDialog::PointEntryDialog(wxWindow* parent,
	wxWindowID id, const wxString& title) : wxDialog(parent, id, title)
{
	CreateControls();
	SetProperties();
}

void PointEntryDialog::CreateControls()
{
	wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 5);

	mainSizer->Add(new wxStaticText(this, wxID_ANY, _T("Specify plot coordinates:")));
	mainSizer->AddSpacer(15);

	wxFlexGridSizer* inputSizer = new wxFlexGridSizer(2, 4, 5, 5);
	mainSizer->Add(inputSizer, 1, wxGROW);

	xValue = new wxTextCtrl(this, wxID_ANY);
	yValue = new wxTextCtrl(this, wxID_ANY);

	xValue->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	yValue->SetValidator(wxTextValidator(wxFILTER_NUMERIC));

	inputSizer->AddStretchSpacer();
	inputSizer->Add(new wxStaticText(this, wxID_ANY, _T("X:")));
	inputSizer->Add(xValue);
	inputSizer->AddStretchSpacer();

	inputSizer->AddStretchSpacer();
	inputSizer->Add(new wxStaticText(this, wxID_ANY, _T("Y:")));
	inputSizer->Add(yValue);

	inputSizer->AddGrowableCol(0);
	inputSizer->AddGrowableCol(3);

	mainSizer->AddSpacer(15);
	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_RIGHT);

	SetSizerAndFit(topSizer);
}

void PointEntryDialog::SetProperties()
{
	Center();
}

bool PointEntryDialog::TransferDataFromWindow()
{
	if (!xValue->GetValue().ToDouble(&point.x) ||
		!yValue->GetValue().ToDouble(&point.y))
		return false;

	return true;
}
