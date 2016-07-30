// File:  imageFrame.h
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Frame object for displaying the images.

#ifndef IMAGE_FRAME_H_
#define IMAGE_FRAME_H_

// wxWidgets headers
#include <wx/wx.h>

class ImageObject;
class ControlsFrame;

class ImageFrame : public wxFrame
{
public:
	ImageFrame(ControlsFrame& controlsFrame);

	void SetImage(wxImage &image);

private:
	void SetProperties();
	void CreateControls();

	void OnClose(wxCloseEvent& event);
	void OnResize(wxSizeEvent &event);

	ControlsFrame& controlsFrame;

	ImageObject *image;

	DECLARE_EVENT_TABLE()
};

#endif// IMAGE_FRAME_H_
