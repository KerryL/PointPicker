// File:  pointPicker.cpp
// Date:  7/29/2016
// Auth:  K. Loux
// Desc:  Object for picking points from images.

// Standard C++ headers
#include <cstdlib>
#include <cassert>
#include <cmath>

// wxWidgets headers
#include <wx/clipbrd.h>

// Eigen headers
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4018)// signed/unsigned mismatch
#pragma warning(disable:4456)// declaration hides previous local declaration
#pragma warning(disable:4714)// function marked as __forceinline not inlined
#endif
#include <Eigen/Dense>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Local headers
#include "pointPicker.h"
#include "pointEntryDialog.h"

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
	clipMode = ClipNone;
	dataMode = DataNone;
	curveIndex = 0;
	ResetErrorString();
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

	HandleClipboardMode(x, y);
	HandleDataMode(x, y);
}

//==========================================================================
// Class:			PointPicker
// Function:		HandleClipboardMode
//
// Description:		Handles necessary clipboard interaction.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::HandleClipboardMode(const double& x, const double& y) const
{
	if (clipMode != ClipNone && wxTheClipboard->Open())
	{
		if (clipMode == ClipX)
			wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%f"), x)));
		else if (clipMode == ClipY)
			wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%f"), y)));
		else if (clipMode == ClipBoth)
			wxTheClipboard->SetData(new wxTextDataObject(wxString::Format(_T("%f\t%f"), x, y)));
		else
			assert(false);

		wxTheClipboard->Close();
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		HandleDataMode
//
// Description:		Handles necessary data collection.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::HandleDataMode(const double& x, const double& y)
{
	if (dataMode == DataNone)
		return;

	if (dataMode == DataCurve)
	{
		while (curvePoints.size() <= curveIndex)
			curvePoints.push_back(std::vector<Point>(0));

		lastPoint.x = x;
		lastPoint.y = y;
		curvePoints[curveIndex].push_back(lastPoint);
		return;
	}

	PointEntryDialog dialog(NULL, wxID_ANY, _T("Coordinate Input"));
	if (dialog.ShowModal() == wxID_CANCEL)
		return;

	if (dataMode == DataReferences)
	{
		referencePoints.push_back(ReferencePair(Point(x, y), dialog.GetPoint()));
		UpdateTransformation();
	}
}

//==========================================================================
// Class:			PointPicker
// Function:		UpdateTransformation
//
// Description:		Updates the transformation matrix according to all stored
//					reference points.
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
void PointPicker::UpdateTransformation()
{
	if (referencePoints.size() < 4)
	{
		ResetErrorString();
		return;
	}
	else
		errorString.clear();

	// Find the matrices that map source image points (in pixels) to basis
	// vectors (sourceTransformation) and plot points (in user-specified
	// values) to basis vectors (plotTransformation)
	Eigen::MatrixXd sourceTransformation(3, referencePoints.size() - 1);
	Eigen::MatrixXd plotTransformation(3, referencePoints.size() - 1);
	Eigen::Vector3d sourceKnowns, plotKnowns;

	unsigned int i;
	for (i = 0; i < referencePoints.size() - 1; i++)
	{
		sourceTransformation(0, i) = referencePoints[i].imageCoords.x;
		sourceTransformation(1, i) = referencePoints[i].imageCoords.y;
		sourceTransformation(2, i) = 1.0;

		// TODO:  Need to handle log values, too, and combinations of x log, y lin, etc.
		plotTransformation(0, i) = referencePoints[i].valueCoords.x;
		plotTransformation(1, i) = referencePoints[i].valueCoords.y;
		plotTransformation(2, i) = 1.0;
	}

	sourceKnowns(0) = referencePoints.back().imageCoords.x;
	sourceKnowns(1) = referencePoints.back().imageCoords.y;
	sourceKnowns(2) = 1.0;

	plotKnowns(0) = referencePoints.back().valueCoords.x;
	plotKnowns(1) = referencePoints.back().valueCoords.y;
	plotKnowns(2) = 1.0;

	Eigen::Vector3d sourceScaleVector(sourceTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(sourceKnowns));
	Eigen::Vector3d plotScaleVector(plotTransformation.jacobiSvd(
		Eigen::ComputeThinU | Eigen::ComputeThinV).solve(plotKnowns));

	for (i = 0; i < (unsigned int)sourceTransformation.cols(); i++)
	{
		sourceTransformation.col(i) *= sourceScaleVector(i);
		plotTransformation.col(i) *= plotScaleVector(i);
	}

	transformationMatrix = plotTransformation * sourceTransformation.inverse();

	// TODO:  Check for ill conditioned matrixes (i.e. if all points specified are on a line?)
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

//==========================================================================
// Class:			PointPicker
// Function:		ResetReferences
//
// Description:		Resets the stored reference data.
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
void PointPicker::ResetReferences()
{
	referencePoints.clear();
	ResetErrorString();
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetCurveData
//
// Description:		Resets the stored curve data.
//
// Input Arguments:
//		curve	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PointPicker::ResetCurveData(const unsigned int& curve)
{
	curvePoints.erase(curvePoints.begin() + curve);
}

//==========================================================================
// Class:			PointPicker
// Function:		Reset
//
// Description:		Resets all stored data.
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
void PointPicker::Reset()
{
	ResetReferences();

	curvePoints.clear();
	curveIndex = 0;
}

//==========================================================================
// Class:			PointPicker
// Function:		GetCurveData
//
// Description:		Returns processed curve data.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		std::vector<std::vector<PointPicker::Point> >
//
//==========================================================================
std::vector<std::vector<PointPicker::Point> > PointPicker::GetCurveData() const
{
	if (!errorString.empty())
		return std::vector<std::vector<Point> >(0);

	std::vector<std::vector<Point> > data(curvePoints);
	unsigned int i, j;
	for (i = 0; i < curvePoints.size(); i++)
	{
		for (j = 0; j < curvePoints[i].size(); j++)
			data[i][j] = ScalePoint(curvePoints[i][j]);
	}

	return data;
}

//==========================================================================
// Class:			PointPicker
// Function:		ScalePoint
//
// Description:		Converts the specified point from image to plot coordinates.
//
// Input Arguments:
//		imagePointIn	= const Point&
//
// Output Arguments:
//		None
//
// Return Value:
//		PointPicker::Point
//
//==========================================================================
PointPicker::Point PointPicker::ScalePoint(const Point& imagePointIn) const
{
	Eigen::Vector3d imagePoint(imagePointIn.x, imagePointIn.y, 1.0);
	Eigen::Vector3d plotPoint(transformationMatrix * imagePoint);

	Point p(plotPoint(0) / plotPoint(2), plotPoint(1) / plotPoint(2));

	if (false)// TODO:  If x axis is log
		p.x = pow(10.0, p.x);

	if (false)// TODO:  If y axis is log
		p.y = pow(10.0, p.y);

	return p;
}

//==========================================================================
// Class:			PointPicker
// Function:		ScaleSinglePoint
//
// Description:		Processes data for single point.
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
//		x		= double&
//		y		= double&
//
// Return Value:
//		Point
//
//==========================================================================
PointPicker::Point PointPicker::ScaleSinglePoint(const double& rawX, const double& rawY,
	const double& xScale, const double& yScale,
	const double& xOffset, const double& yOffset, double& x, double& y) const
{
	x = ScaleOrdinate(rawX, xScale, xOffset);
	y = ScaleOrdinate(rawY, yScale, yOffset);

	if (!errorString.empty())
		return Point(0.0, 0.0);

	return ScalePoint(Point(x, y));
}

//==========================================================================
// Class:			PointPicker
// Function:		ResetErrorString
//
// Description:		Resets the error string.
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
void PointPicker::ResetErrorString() const
{
	errorString = _T("Not enough reference points");
}
