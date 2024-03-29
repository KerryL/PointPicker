// File:  imageDropTarget.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Drag-and-drop target for image files.

// Local headers
#include "imageDropTarget.h"
#include "controlsFrame.h"

//==========================================================================
// Class:			ImageDropTarget
// Function:		ImageDropTarget
//
// Description:		Constructor for ImageDropTarget class.
//
// Input Arguments:
//		mainFrame	= &ControlsFrame, reference to main application window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ImageDropTarget::ImageDropTarget(ControlsFrame &mainFrame) : mainFrame(mainFrame)
{
	wxDataObjectComposite *dataObject = new wxDataObjectComposite;

	dataObject->Add(new wxFileDataObject);
	dataObject->Add(new wxTextDataObject);

	SetDataObject(dataObject);

	buffer = nullptr;
}

//==========================================================================
// Class:			ImageDropTarget
// Function:		OnDropFiles
//
// Description:		Handles dragging and dropping of files.
//
// Input Arguments:
//		filenames	= const &wxArrayString containing the list of filenames
//					  being dropped
//
// Output Arguments:
//		None
//
// Return Value:
//		true to accept the data, false to veto
//
//==========================================================================
bool ImageDropTarget::OnDropFiles(const wxArrayString &filenames)
{
	mainFrame.LoadFiles(filenames);
	return true;
}

//==========================================================================
// Class:			ImageDropTarget
// Function:		OnData
//
// Description:		Overloaded virtual method from wxTextDropTarget.
//
// Input Arguments:
//		x		= wxCoord (unused)
//		y		= wxCoord (unused)
//		def		= wxDragResult
//
// Output Arguments:
//		None
//
// Return Value:
//		wxDragResult
//
//==========================================================================
wxDragResult ImageDropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def)
{
	if (!GetData())
		return wxDragNone;

	wxDataObjectComposite *dataObject = static_cast<wxDataObjectComposite*>(m_dataObject);
	size_t bufferSize = dataObject->GetDataSize(dataObject->GetReceivedFormat());

	buffer = std::make_unique<char[]>(bufferSize);
	if (!dataObject->GetDataHere(dataObject->GetReceivedFormat(), buffer.get()))
		return wxDragNone;

	if (dataObject->GetReceivedFormat().GetType() == wxDF_FILENAME)
	{
		wxFileDataObject fileData;
		fileData.SetData(bufferSize, buffer.get());
		return OnDropFiles(fileData.GetFilenames()) ? def : wxDragNone;
	}
	
	assert(false);
	return wxDragNone;
}
