// File:  pointPicker.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

// Standard C++ headers
#include <cstdlib>

// Local headers
#include "pointPicker.h"

#include <wx/wx.h>// TODO:  Remove

//==========================================================================
// Class:			PointPicker
// Function:		PointPicker
//
// Description:		Constructor for PointPicker class.
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
PointPicker::PointPicker()
{
	image = NULL;
}

//==========================================================================
// Class:			PointPicker
// Function:		AddPoint
//
// Description:		Processes new point data.
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
void PointPicker::AddPoint(const double& rawX, const double& rawY,
	const double& xScale, const double& yScale,
	const double& xOffset, const double& yOffset)
{
	const double x(ScaleOrdinate(rawX, xScale, xOffset));
	const double y(ScaleOrdinate(rawY, yScale, yOffset));
	wxMessageBox(wxString::Format(_T("%f, %f"), x, y));
}

//==========================================================================
// Class:			PointPicker
// Function:		ScaleOrdinate
//
// Description:		Scales the value according to the specified length and offset.
//
// Input Arguments:
//		value	= const double&
//		scale	= const double&
//		offset	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PointPicker::ScaleOrdinate(const double& value, const double& scale,
	const double& offset)
{
	return value * scale + offset;
}
